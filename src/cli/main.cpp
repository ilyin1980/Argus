/**
 * @file main.cpp
 * @brief Headless entry point.
 *
 * Output contract, relied on by scripts and coding agents:
 *  - @c --json puts newline-delimited JSON on stdout, one object per line;
 *  - progress, warnings and errors always go to stderr, never stdout;
 *  - results are emitted in a deterministic order;
 *  - exit code 0 means "found something", 1 means "nothing found", 2 an error.
 */

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QImageReader>
#include <QJsonArray>
#include <QJsonObject>
#include <QLocale>
#include <QTextStream>

#include "core/BowIndex.h"
#include "core/Database.h"
#include "core/DescriptorStore.h"
#include "core/DuplicateFinder.h"
#include "core/GitRepo.h"
#include "core/FeatureExtractor.h"
#include "core/FeatureMatcher.h"
#include "core/ImageDecoder.h"
#include "core/Indexer.h"
#include "core/Json.h"
#include "core/ObjectFinder.h"
#include "core/Onnx.h"
#include "core/Paths.h"
#include "core/QueryEngine.h"
#include "core/TemplateMatcher.h"
#include "core/Vocabulary.h"

namespace {

/** @brief Process exit codes; see the file-level contract. */
enum ExitCode {
    ExitFound    = 0,
    ExitNotFound = 1,
    ExitError    = 2,
};

QTextStream &out()
{
    static QTextStream stream(stdout);
    return stream;
}

QTextStream &err()
{
    static QTextStream stream(stderr);
    return stream;
}

/** @brief Human-readable byte count, e.g. "1.4 GiB". */
QString humanBytes(qint64 bytes)
{
    return QLocale::system().formattedDataSize(bytes, 2, QLocale::DataSizeIecFormat);
}

void printUsage()
{
    err() << "argus " << ARGUS_VERSION << " — duplicate and similar image finder\n\n"
          << "Usage:\n"
          << "  argus index <dir> [options]        build or refresh the index\n"
          << "  argus dupes <dir> [options]        report duplicate groups\n"
          << "  argus query <dir> --image <file>   rank images by similarity\n"
          << "  argus find  <dir> --image <shot>   locate indexed assets in a screenshot\n"
          << "  argus stats <dir> [options]        summarise an existing index\n"
          << "  argus formats                      list readable image formats\n"
          << "  argus vocab <dir> [options]        train the visual vocabulary\n"
          << "  argus match --query q --asset a    match two images, verify geometry\n"
          << "  argus doctor [--model f.onnx]      report the neural backend state\n"
          << "\n"
          << "Run 'argus <command> --help' for per-command options.\n";
    err().flush();
}

/** @brief Options every command understands. */
struct CommonOptions {
    QString root;
    QString dbPath;
    bool    json  = false;
    bool    paths = false; ///< Emit bare absolute paths, one per line.
    bool    quiet = false;
};

/**
 * @brief Absolute, native-separator path of one result.
 * @param root Indexed root.
 * @param rel  Path relative to it.
 * @return A path that can be pasted into a shell or another tool.
 */
QString fullPath(const QString &root, const QString &rel)
{
    return QDir::toNativeSeparators(argus::absolutePathFor(root, rel));
}

/**
 * @brief How to name one result, whether it is a file or a version in a branch.
 * @param root Indexed directory.
 * @param row  Result row.
 * @return An absolute path, or git's own <tt>branch:path</tt> revision syntax.
 *
 * A row that came out of a branch has no file on disk, and printing an absolute
 * path for it would be a lie that scripts would then act on. <tt>git show</tt>
 * accepts what is printed instead, so the output stays directly usable.
 */
QString locationOf(const QString &root, const argus::FileInfoRow &row)
{
    if (row.ref.isEmpty())
        return fullPath(root, row.rel);
    return row.ref + QLatin1Char(':') + row.rel;
}

/**
 * @brief Warn when a hand-picked matcher will not work on this machine.
 * @param modelPath Model the user asked for.
 * @param provider  Execution provider the session ended up on.
 *
 * The half-precision export is only usable under DirectML. Under CUDA it runs
 * four times slower than the full-precision one, on the CPU it refuses to run,
 * and under CoreML it returns zero matches **without reporting an error** —
 * which is indistinguishable from "this asset is not in the picture".
 */
void warnAboutMatcherChoice(const QString &modelPath, const QString &provider)
{
    if (!modelPath.contains(QLatin1String("fp16")))
        return;
    if (provider == QLatin1String("DirectML"))
        return;

    err() << "warning: the fp16 matcher is only reliable on DirectML; on "
          << provider << " it will be slow or silently find nothing.\n"
          << "         drop --matcher to let Argus pick the right export.\n";
}

/** @brief Add the options shared by all commands to @p parser. */
void addCommonOptions(QCommandLineParser &parser)
{
    parser.addOption({ QStringLiteral("db"),
                       QStringLiteral("Database file. Defaults to <dir>/.argus/index.db."),
                       QStringLiteral("path") });
    parser.addOption({ QStringLiteral("json"),
                       QStringLiteral("Emit newline-delimited JSON on stdout.") });
    parser.addOption({ QStringLiteral("paths"),
                       QStringLiteral("Emit bare absolute file paths, one per line, "
                                      "for piping into other tools.") });
    parser.addOption({ QStringLiteral("quiet"),
                       QStringLiteral("Suppress progress output on stderr.") });
}

/**
 * @brief Resolve the positional directory and the shared options.
 * @param parser Parser that has already processed the argument list.
 * @param common Out-parameter receiving the resolved values.
 * @param requireExistingDb Fail when the database does not exist yet.
 * @return @c true when the command may proceed.
 */
bool resolveCommon(const QCommandLineParser &parser, CommonOptions &common, bool requireExistingDb)
{
    const QStringList positional = parser.positionalArguments();
    if (positional.isEmpty()) {
        err() << "error: missing <dir>\n";
        return false;
    }

    common.root  = argus::normalizeRoot(positional.first());
    common.json  = parser.isSet(QStringLiteral("json"));
    common.paths = parser.isSet(QStringLiteral("paths"));
    common.quiet = parser.isSet(QStringLiteral("quiet"));

    if (common.json && common.paths) {
        err() << "error: --json and --paths select different output formats; pick one\n";
        return false;
    }
    // Bare paths are meant to be piped, so nothing else may reach stdout.
    if (common.paths)
        common.quiet = true;

    if (!QFileInfo(common.root).isDir()) {
        err() << "error: not a directory: " << common.root << "\n";
        return false;
    }

    common.dbPath = parser.value(QStringLiteral("db"));
    if (common.dbPath.isEmpty()) {
        QString error;
        common.dbPath = argus::defaultDatabasePath(common.root, &error);
        if (common.dbPath.isEmpty()) {
            err() << "error: " << error << "\n";
            return false;
        }
    }

    if (requireExistingDb && !QFileInfo::exists(common.dbPath)) {
        err() << "error: no index at " << common.dbPath << "\n"
              << "hint: run 'argus index " << common.root << "' first\n";
        return false;
    }
    return true;
}

/** @brief Open a database, reporting failures on stderr. */
bool openDatabase(argus::Database &db, const QString &path)
{
    QString error;
    if (!db.open(path, &error)) {
        err() << "error: " << error << "\n";
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// index
// ---------------------------------------------------------------------------

int cmdIndex(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Build or refresh the index for a directory."));
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("dir"), QStringLiteral("Directory to index."));
    addCommonOptions(parser);
    parser.addOption({ QStringLiteral("jobs"),
                       QStringLiteral("Worker threads. Default: one per hardware thread."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("ext"),
                       QStringLiteral("Comma-separated extensions. Default: png,jpg,jpeg."),
                       QStringLiteral("list") });
    parser.addOption({ QStringLiteral("no-thumbs"),
                       QStringLiteral("Skip preview generation.") });
    parser.addOption({ QStringLiteral("thumb-size"),
                       QStringLiteral("Longest preview side in pixels. Default: 256."),
                       QStringLiteral("px") });
    parser.addOption({ QStringLiteral("force"),
                       QStringLiteral("Re-read every file, ignoring size and mtime.") });
    parser.addOption({ QStringLiteral("no-prune"),
                       QStringLiteral("Keep rows whose file has disappeared.") });
    parser.addOption({ QStringLiteral("features"),
                       QStringLiteral("Also extract neural local features, needed to find an "
                                      "object inside a screenshot.") });
    parser.addOption({ QStringLiteral("feature-model"),
                       QStringLiteral("Extractor .onnx file. Default: models/disk.onnx."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("keypoints"),
                       QStringLiteral("Keypoints kept per image. Default: 512."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("feature-side"),
                       QStringLiteral("Long side the extractor sees. Default: 1024."),
                       QStringLiteral("px") });
    parser.addOption({ QStringLiteral("feature-cpu"),
                       QStringLiteral("Run the extractor on the CPU instead of the GPU.") });
    parser.addOption({ QStringLiteral("branches"),
                       QStringLiteral("Also index these git branches, comma separated, or "
                                      "'all' for every local branch. The listed set becomes "
                                      "the complete set: branches indexed earlier and not "
                                      "named here are dropped."),
                       QStringLiteral("names") });
    parser.addOption({ QStringLiteral("remote-branches"),
                       QStringLiteral("Include remote-tracking branches when --branches is 'all'.") });
    parser.process(args);

    CommonOptions common;
    if (!resolveCommon(parser, common, false))
        return ExitError;

    argus::IndexOptions options;
    options.root        = common.root;
    options.dbPath      = common.dbPath;
    options.thumbnails  = !parser.isSet(QStringLiteral("no-thumbs"));
    options.force       = parser.isSet(QStringLiteral("force"));
    options.prune       = !parser.isSet(QStringLiteral("no-prune"));

    if (parser.isSet(QStringLiteral("jobs")))
        options.jobs = parser.value(QStringLiteral("jobs")).toInt();
    if (parser.isSet(QStringLiteral("thumb-size")))
        options.thumbSize = parser.value(QStringLiteral("thumb-size")).toInt();
    options.extractFeatures = parser.isSet(QStringLiteral("features"));
    options.featureUseGpu   = !parser.isSet(QStringLiteral("feature-cpu"));
    options.featureModelPath = parser.isSet(QStringLiteral("feature-model"))
                                   ? parser.value(QStringLiteral("feature-model"))
                                   : argus::defaultModelsDir() + QStringLiteral("/disk.onnx");
    if (parser.isSet(QStringLiteral("keypoints")))
        options.featureMaxKeypoints = parser.value(QStringLiteral("keypoints")).toInt();
    if (parser.isSet(QStringLiteral("feature-side")))
        options.featureMaxSide = parser.value(QStringLiteral("feature-side")).toInt();

    if (parser.isSet(QStringLiteral("ext"))) {
        const QStringList raw = parser.value(QStringLiteral("ext"))
                                    .split(QLatin1Char(','), Qt::SkipEmptyParts);
        for (const QString &e : raw)
            options.extensions << e.trimmed().toLower().remove(QLatin1Char('.'));
    }

    if (parser.isSet(QStringLiteral("branches"))) {
        // Naming the branches is also declaring what the index should contain,
        // so this is the one place allowed to drop refs it was not given.
        options.syncBranches = true;

        const QString value = parser.value(QStringLiteral("branches")).trimmed();
        const argus::git::RepoInfo repo = argus::git::inspect(common.root);
        if (!repo.isRepo) {
            err() << "error: " << common.root << " is not inside a git repository\n";
            return ExitError;
        }

        if (value.compare(QLatin1String("all"), Qt::CaseInsensitive) == 0) {
            QString branchError;
            options.branches = argus::git::branches(
                repo.topLevel, parser.isSet(QStringLiteral("remote-branches")), &branchError);
            if (options.branches.isEmpty()) {
                err() << "error: no branches found: " << branchError << "\n";
                return ExitError;
            }
        } else {
            options.branches = value.split(QLatin1Char(','), Qt::SkipEmptyParts);
            for (QString &name : options.branches)
                name = name.trimmed();
        }

        // The checked-out branch is already the working tree; indexing it again
        // stores every file twice under two names for the same bytes.
        options.branches.removeAll(repo.currentRef);
        options.branches.removeDuplicates();
    }

    argus::Indexer indexer;
    if (!common.quiet) {
        QObject::connect(&indexer, &argus::Indexer::message,
                         [](const QString &text) { err() << text << "\n"; err().flush(); });
        QObject::connect(&indexer, &argus::Indexer::progress,
                         [](int done, int total, const QString &stage) {
                             if (total > 0)
                                 err() << "\r" << stage << ": " << done << "/" << total << "   ";
                             else
                                 err() << "\r" << stage << ": " << done << "   ";
                             err().flush();
                         });
    }

    QString error;
    const argus::IndexStats stats = indexer.run(options, &error);
    if (!common.quiet)
        err() << "\n";

    if (!error.isEmpty()) {
        err() << "error: " << error << "\n";
        return ExitError;
    }

    if (common.json) {
        out() << argus::toLine(argus::toJson(stats)) << "\n";
        out().flush();
    } else {
        out() << "scanned " << stats.scanned
              << ", indexed " << stats.indexed
              << ", unchanged " << stats.skipped
              << ", failed " << stats.failed
              << ", pruned " << stats.pruned << "\n"
              << "read " << humanBytes(stats.bytesRead)
              << " in " << (stats.elapsedMs / 1000.0) << " s\n";
        if (stats.branchesIndexed > 0 || stats.branchesSkipped > 0) {
            out() << "branches " << stats.branchesIndexed << " read, "
                  << stats.branchesSkipped << " unchanged\n";
        }
        if (options.extractFeatures) {
            out() << "features " << stats.featured
                  << " images, " << stats.keypointsTotal << " keypoints"
                  << ", failed " << stats.featureFailed
                  << ", store " << humanBytes(stats.featureBytes)
                  << " (" << stats.featureProvider << ")\n";
        }
        out().flush();
    }
    return stats.indexed > 0 || stats.skipped > 0 ? ExitFound : ExitNotFound;
}

// ---------------------------------------------------------------------------
// dupes
// ---------------------------------------------------------------------------

int cmdDupes(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Report duplicate and near-duplicate groups."));
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("dir"), QStringLiteral("Indexed directory."));
    addCommonOptions(parser);
    parser.addOption({ QStringLiteral("distance"),
                       QStringLiteral("Perceptual-hash radius, 0..64. Default: 4. 0 = identical fingerprints."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("phash-only"),
                       QStringLiteral("Match on the DCT hash alone, without confirming "
                                      "with the gradient hash. Looser, more false positives.") });
    parser.addOption({ QStringLiteral("exact-only"),
                       QStringLiteral("Report byte-identical groups only.") });
    parser.addOption({ QStringLiteral("near-only"),
                       QStringLiteral("Report perceptually similar groups only.") });
    parser.addOption({ QStringLiteral("bucket-limit"),
                       QStringLiteral("Skip hash buckets larger than this. Default: 2000."),
                       QStringLiteral("n") });
    parser.process(args);

    CommonOptions common;
    if (!resolveCommon(parser, common, true))
        return ExitError;

    argus::DuplicateOptions options;
    if (parser.isSet(QStringLiteral("distance")))
        options.maxDistance = parser.value(QStringLiteral("distance")).toInt();
    if (parser.isSet(QStringLiteral("bucket-limit")))
        options.bucketLimit = parser.value(QStringLiteral("bucket-limit")).toInt();
    if (parser.isSet(QStringLiteral("exact-only")))
        options.includeNear = false;
    if (parser.isSet(QStringLiteral("near-only")))
        options.includeExact = false;
    if (parser.isSet(QStringLiteral("phash-only")))
        options.requireBothHashes = false;

    argus::Database db;
    if (!openDatabase(db, common.dbPath))
        return ExitError;

    const argus::DuplicateReport report =
        argus::findDuplicates(db, options, nullptr,
                           [&](int done, int total, const QString &stage) {
                               if (common.quiet)
                                   return;
                               err() << "\r" << stage << ": " << done << "/" << total << "   ";
                               err().flush();
                           });
    if (!common.quiet)
        err() << "\r                                        \r";

    if (report.skippedBuckets > 0) {
        err() << "note: " << report.skippedBuckets
              << " oversized hash buckets were not expanded (raise --bucket-limit to include them)\n";
    }

    if (common.paths) {
        // One group per block, blank line between blocks: enough structure to
        // regroup with awk, still trivially pipeable into xargs.
        bool first = true;
        for (const argus::DuplicateGroup &group : report.groups) {
            if (!first)
                out() << "\n";
            first = false;
            for (const argus::FileInfoRow &file : group.files)
                out() << locationOf(common.root, file) << "\n";
        }
    } else if (common.json) {
        for (const argus::DuplicateGroup &group : report.groups)
            out() << argus::toLine(argus::toJson(group, common.root)) << "\n";
        out() << argus::toLine(argus::summaryJson(report)) << "\n";
    } else {
        int number = 0;
        for (const argus::DuplicateGroup &group : report.groups) {
            out() << "\n#" << ++number << "  "
                  << (group.kind == argus::GroupKind::Exact ? "exact" : "near")
                  << "  " << group.files.size() << " files"
                  << "  wasted " << humanBytes(group.wastedBytes);
            if (group.kind == argus::GroupKind::Near)
                out() << "  distance<=" << group.maxDistance;
            out() << "\n";
            for (const argus::FileInfoRow &file : group.files) {
                out() << "    " << locationOf(common.root, file)
                      << "  (" << file.width << "x" << file.height
                      << ", " << humanBytes(file.size) << ")\n";
            }
        }
        out() << "\n" << report.groups.size() << " groups, "
              << humanBytes(report.wastedBytes) << " reclaimable\n";
    }
    out().flush();

    return report.groups.isEmpty() ? ExitNotFound : ExitFound;
}

// ---------------------------------------------------------------------------
// query
// ---------------------------------------------------------------------------

int cmdQuery(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Rank indexed images by similarity to a reference."));
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("dir"), QStringLiteral("Indexed directory."));
    addCommonOptions(parser);
    parser.addOption({ QStringLiteral("image"),
                       QStringLiteral("Reference image file."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("top"),
                       QStringLiteral("Maximum hits. Default: 20."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("max-distance"),
                       QStringLiteral("Combined distance cutoff, 0..128. Default: 128."),
                       QStringLiteral("n") });
    parser.process(args);

    CommonOptions common;
    if (!resolveCommon(parser, common, true))
        return ExitError;

    const QString imagePath = parser.value(QStringLiteral("image"));
    if (imagePath.isEmpty()) {
        err() << "error: --image is required\n";
        return ExitError;
    }
    if (!QFileInfo::exists(imagePath)) {
        err() << "error: no such file: " << imagePath << "\n";
        return ExitError;
    }

    argus::QueryOptions options;
    if (parser.isSet(QStringLiteral("top")))
        options.topK = parser.value(QStringLiteral("top")).toInt();
    if (parser.isSet(QStringLiteral("max-distance")))
        options.maxDistance = parser.value(QStringLiteral("max-distance")).toInt();

    argus::Database db;
    if (!openDatabase(db, common.dbPath))
        return ExitError;

    QString error;
    const argus::QueryResult result = argus::queryByImage(db, imagePath, options, &error);
    if (!error.isEmpty()) {
        err() << "error: " << error << "\n";
        return ExitError;
    }

    if (common.paths) {
        for (const argus::Match &match : result.matches)
            out() << locationOf(common.root, match.file) << "\n";
    } else if (common.json) {
        for (const argus::Match &match : result.matches)
            out() << argus::toLine(argus::toJson(match, common.root)) << "\n";
    } else {
        for (const argus::Match &match : result.matches) {
            out() << QString::number(match.score, 'f', 4) << "  "
                  << "d=" << match.distance << "  "
                  << locationOf(common.root, match.file) << "\n";
        }
        if (result.matches.isEmpty())
            err() << "no matches within distance " << options.maxDistance << "\n";
    }
    out().flush();

    return result.matches.isEmpty() ? ExitNotFound : ExitFound;
}

// ---------------------------------------------------------------------------
// stats
// ---------------------------------------------------------------------------

int cmdStats(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Summarise an existing index."));
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("dir"), QStringLiteral("Indexed directory."));
    addCommonOptions(parser);
    parser.process(args);

    CommonOptions common;
    if (!resolveCommon(parser, common, true))
        return ExitError;

    argus::Database db;
    if (!openDatabase(db, common.dbPath))
        return ExitError;

    const argus::DatabaseSummary summary = db.summary();
    const QList<argus::FileInfoRow> sparse =
        db.filesWithFewFeatures(QStringLiteral("disk"), 8);

    if (common.json) {
        QJsonObject o;
        o.insert(QStringLiteral("type"), QStringLiteral("stats"));
        o.insert(QStringLiteral("root"), db.metaValue(QStringLiteral("root")));
        o.insert(QStringLiteral("db"), common.dbPath);
        o.insert(QStringLiteral("files"), summary.files);
        o.insert(QStringLiteral("failed"), summary.failed);
        o.insert(QStringLiteral("thumbs"), summary.thumbs);
        o.insert(QStringLiteral("total_bytes"), static_cast<double>(summary.totalBytes));
        o.insert(QStringLiteral("last_indexed"), db.metaValue(QStringLiteral("last_indexed")));
        o.insert(QStringLiteral("low_texture"), static_cast<int>(sparse.size()));
        QJsonArray sparsePaths;
        for (const argus::FileInfoRow &row : sparse)
            sparsePaths.append(fullPath(common.root, row.rel));
        o.insert(QStringLiteral("low_texture_files"), sparsePaths);
        out() << argus::toLine(o) << "\n";
    } else {
        out() << "database : " << common.dbPath << "\n"
              << "root     : " << db.metaValue(QStringLiteral("root")) << "\n"
              << "files    : " << summary.files << "\n"
              << "failed   : " << summary.failed << "\n"
              << "previews : " << summary.thumbs << "\n"
              << "size     : " << humanBytes(summary.totalBytes) << "\n";
        if (!sparse.isEmpty()) {
            out() << "low texture : " << sparse.size()
                  << " images with 8 or fewer keypoints; local features cannot "
                     "locate these\n";
            for (const argus::FileInfoRow &row : std::as_const(sparse).first(std::min<qsizetype>(8, sparse.size())))
                out() << "              " << row.rel << "  (" << row.width << "x" << row.height << ")\n";
        }
    }
    out().flush();
    return summary.files > 0 ? ExitFound : ExitNotFound;
}

// ---------------------------------------------------------------------------
// formats
// ---------------------------------------------------------------------------

int cmdFormats(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("List image formats this build can read."));
    parser.addHelpOption();
    parser.addOption({ QStringLiteral("json"), QStringLiteral("Emit JSON.") });
    parser.process(args);

    const QStringList indexed   = argus::defaultExtensions();
    const QStringList available = argus::supportedExtensions();

    if (parser.isSet(QStringLiteral("json"))) {
        QJsonObject o;
        o.insert(QStringLiteral("type"), QStringLiteral("formats"));
        o.insert(QStringLiteral("indexed"), QJsonArray::fromStringList(indexed));
        o.insert(QStringLiteral("readable"), QJsonArray::fromStringList(available));
        out() << argus::toLine(o) << "\n";
    } else {
        out() << "indexed by default : " << indexed.join(QStringLiteral(", ")) << "\n"
              << "readable in build  : " << available.join(QStringLiteral(", ")) << "\n";
    }
    out().flush();
    return ExitFound;
}

// ---------------------------------------------------------------------------
// find — the main scenario
// ---------------------------------------------------------------------------

int cmdFind(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Find which indexed assets appear inside a screenshot."));
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("dir"), QStringLiteral("Indexed directory."));
    addCommonOptions(parser);
    parser.addOption({ QStringLiteral("image"), QStringLiteral("Screenshot to search in."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("roi"),
                       QStringLiteral("Search only this region: x,y,w,h in image pixels."),
                       QStringLiteral("rect") });
    parser.addOption({ QStringLiteral("shortlist"),
                       QStringLiteral("Candidates taken from the bag-of-words ranking. Default: 400."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("top"),
                       QStringLiteral("Results to report. Default: 10."), QStringLiteral("n") });
    parser.addOption({ QStringLiteral("keypoints"),
                       QStringLiteral("Keypoints taken from the query. Should match what the "
                                      "index was built with, or the shortlist compares a dense "
                                      "description against sparse ones. Default: 512."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("template"),
                       QStringLiteral("Also search low-texture assets by correlation. "
                                      "Finds artwork local features cannot, at the cost of "
                                      "seconds rather than milliseconds.") });
    parser.addOption({ QStringLiteral("min-inliers"),
                       QStringLiteral("Reject fits below this many inliers. Default: 8."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("extractor"),
                       QStringLiteral("Default: models/disk.onnx."), QStringLiteral("file") });
    parser.addOption({ QStringLiteral("matcher"),
                       QStringLiteral("Default: models/disk_lightglue_fused_fp16.onnx."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("match-threads"),
                       QStringLiteral("Concurrent matcher sessions. Default: auto — parallel only "
                                      "once the shortlist is long enough to pay for them."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("matcher-cpu"),
                       QStringLiteral("Run the matcher on the CPU. Its tensors are tiny, so "
                                      "GPU dispatch overhead can outweigh the arithmetic.") });
    parser.process(args);

    CommonOptions common;
    if (!resolveCommon(parser, common, true))
        return ExitError;

    const QString imagePath = parser.value(QStringLiteral("image"));
    if (imagePath.isEmpty()) {
        err() << "error: --image is required\n";
        return ExitError;
    }

    QImageReader reader(imagePath);
    reader.setAutoTransform(true);
    QImage image = reader.read();
    if (image.isNull()) {
        err() << "error: cannot read " << imagePath << ": " << reader.errorString() << "\n";
        return ExitError;
    }

    if (parser.isSet(QStringLiteral("roi"))) {
        const QStringList parts = parser.value(QStringLiteral("roi"))
                                      .split(QLatin1Char(','), Qt::SkipEmptyParts);
        if (parts.size() != 4) {
            err() << "error: --roi expects x,y,w,h\n";
            return ExitError;
        }
        const QRect rect(parts.at(0).toInt(), parts.at(1).toInt(),
                         parts.at(2).toInt(), parts.at(3).toInt());
        const QRect clipped = rect.intersected(image.rect());
        if (clipped.isEmpty()) {
            err() << "error: --roi does not overlap the image\n";
            return ExitError;
        }
        image = image.copy(clipped);
    }

    argus::FindOptions options;
    if (parser.isSet(QStringLiteral("shortlist")))
        options.shortlist = parser.value(QStringLiteral("shortlist")).toInt();
    if (parser.isSet(QStringLiteral("keypoints")))
        options.maxKeypoints = parser.value(QStringLiteral("keypoints")).toInt();
    if (parser.isSet(QStringLiteral("top")))
        options.topK = parser.value(QStringLiteral("top")).toInt();
    options.useTemplateChannel = parser.isSet(QStringLiteral("template"));
    if (parser.isSet(QStringLiteral("min-inliers")))
        options.geometry.minInliers = parser.value(QStringLiteral("min-inliers")).toInt();
    if (parser.isSet(QStringLiteral("extractor")))
        options.extractorPath = parser.value(QStringLiteral("extractor"));
    if (parser.isSet(QStringLiteral("matcher")))
        options.matcherPath = parser.value(QStringLiteral("matcher"));
    options.matcherUseGpu = !parser.isSet(QStringLiteral("matcher-cpu"));
    if (parser.isSet(QStringLiteral("match-threads")))
        options.matcherThreads = parser.value(QStringLiteral("match-threads")).toInt();

    QString error;
    QElapsedTimer timer;
    timer.start();
    auto finder = argus::ObjectFinder::create(common.dbPath, options, &error);
    if (!finder) {
        err() << "error: " << error << "\n";
        return ExitError;
    }
    const qint64 loadMs = timer.elapsed();

    timer.restart();
    const QList<argus::FindResult> results =
        finder->find(image, options, nullptr,
                     [&](int done, int total) {
                         if (common.quiet)
                             return;
                         err() << "\rverifying: " << done << "/" << total << "  ";
                         err().flush();
                     },
                     &error);
    const qint64 searchMs = timer.elapsed();
    if (!common.quiet)
        err() << "\r                            \r";

    if (!error.isEmpty() && results.isEmpty()) {
        err() << "error: " << error << "\n";
        return ExitError;
    }

    if (common.paths) {
        for (const argus::FindResult &r : results)
            out() << locationOf(common.root, r.file) << "\n";
    } else if (common.json) {
        for (const argus::FindResult &r : results) {
            QJsonObject o = argus::toJson(r.file, common.root);
            o.insert(QStringLiteral("type"), QStringLiteral("find"));
            o.insert(QStringLiteral("inliers"), r.inliers);
            o.insert(QStringLiteral("matches"), r.matches);
            o.insert(QStringLiteral("inlier_ratio"), r.inlierRatio);
            o.insert(QStringLiteral("score"), r.score);
            o.insert(QStringLiteral("bow_score"), r.bowScore);
            QJsonObject box;
            box.insert(QStringLiteral("x"), r.box.x());
            box.insert(QStringLiteral("y"), r.box.y());
            box.insert(QStringLiteral("w"), r.box.width());
            box.insert(QStringLiteral("h"), r.box.height());
            o.insert(QStringLiteral("box"), box);
            out() << argus::toLine(o) << "\n";
        }
        QJsonObject s;
        s.insert(QStringLiteral("type"), QStringLiteral("find_summary"));
        s.insert(QStringLiteral("results"), static_cast<int>(results.size()));
        s.insert(QStringLiteral("indexed"), finder->documentCount());
        s.insert(QStringLiteral("shortlist"), options.shortlist);
        s.insert(QStringLiteral("load_ms"), static_cast<double>(loadMs));
        s.insert(QStringLiteral("search_ms"), static_cast<double>(searchMs));
        s.insert(QStringLiteral("provider"), finder->provider());
        out() << argus::toLine(s) << "\n";
    } else {
        out() << "searched " << finder->documentCount() << " indexed tiles on "
              << finder->provider() << ", shortlist " << options.shortlist
              << ", " << searchMs << " ms (models loaded in " << loadMs << " ms)\n\n";
        if (results.isEmpty()) {
            out() << "nothing matched\n";
        } else {
            for (const argus::FindResult &r : results) {
                if (r.channel == QLatin1String("correlation")) {
                    out() << QString::asprintf("%.3f  correlation %4.2fx  ", r.score, r.scale);
                } else {
                    out() << QString::asprintf("%.3f  %4d inliers %3.0f%%  ",
                                               r.score, r.inliers, r.inlierRatio * 100.0);
                }
                out() << "at " << qRound(r.box.x()) << "," << qRound(r.box.y())
                      << " " << qRound(r.box.width()) << "x" << qRound(r.box.height())
                      << "   " << locationOf(common.root, r.file) << "\n";
            }
        }
    }
    out().flush();
    return results.isEmpty() ? ExitNotFound : ExitFound;
}

// ---------------------------------------------------------------------------
// match — two images, end to end, for validating the matching pipeline
// ---------------------------------------------------------------------------

int cmdMatch(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Match one image against another and verify the geometry."));
    parser.addHelpOption();
    parser.addOption({ QStringLiteral("query"), QStringLiteral("Screenshot or crop."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("asset"), QStringLiteral("Candidate asset. May be repeated."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("extractor"),
                       QStringLiteral("Default: models/disk.onnx."), QStringLiteral("file") });
    parser.addOption({ QStringLiteral("matcher"),
                       QStringLiteral("Default: models/disk_lightglue_fused_fp16.onnx."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("raw-keypoints"),
                       QStringLiteral("Feed pixel keypoints to the matcher instead of "
                                      "normalised ones.") });
    parser.addOption({ QStringLiteral("keypoints"),
                       QStringLiteral("Keypoints per image. Default: 512."), QStringLiteral("n") });
    parser.addOption({ QStringLiteral("json"), QStringLiteral("Emit JSON.") });
    parser.process(args);

    const QString queryPath = parser.value(QStringLiteral("query"));
    const QStringList assetPaths = parser.values(QStringLiteral("asset"));
    if (queryPath.isEmpty() || assetPaths.isEmpty()) {
        err() << "error: --query and at least one --asset are required\n";
        return ExitError;
    }

    const bool json = parser.isSet(QStringLiteral("json"));
    const QString extractorPath = parser.isSet(QStringLiteral("extractor"))
                                      ? parser.value(QStringLiteral("extractor"))
                                      : argus::defaultModelsDir() + QStringLiteral("/disk.onnx");
    const QString matcherPath = parser.isSet(QStringLiteral("matcher"))
                                    ? parser.value(QStringLiteral("matcher"))
                                    : argus::preferredMatcherModel(argus::defaultModelsDir());

    QString error;
    auto extractor = argus::FeatureExtractor::create(extractorPath, true, &error);
    if (!extractor) {
        err() << "error: " << error << "\n";
        return ExitError;
    }
    auto matcher = argus::FeatureMatcher::create(matcherPath, true, &error);
    if (!matcher) {
        err() << "error: " << error << "\n";
        return ExitError;
    }
    matcher->setNormalizeKeypoints(!parser.isSet(QStringLiteral("raw-keypoints")));
    if (parser.isSet(QStringLiteral("matcher")))
        warnAboutMatcherChoice(matcherPath, matcher->provider());

    argus::ExtractorOptions extractorOptions;
    if (parser.isSet(QStringLiteral("keypoints")))
        extractorOptions.maxKeypoints = parser.value(QStringLiteral("keypoints")).toInt();

    const auto readImage = [](const QString &path) {
        QImageReader reader(path);
        reader.setAutoTransform(true);
        return reader.read();
    };

    const QImage queryImage = readImage(queryPath);
    if (queryImage.isNull()) {
        err() << "error: cannot read " << queryPath << "\n";
        return ExitError;
    }

    // The query is a screenshot: it has no alpha to mask against.
    argus::ExtractorOptions queryOptions = extractorOptions;
    queryOptions.useAlphaMask = false;
    const argus::FeatureSet queryFeatures = extractor->extract(queryImage, queryOptions, &error);
    if (queryFeatures.isEmpty()) {
        err() << "error: no features in the query: " << error << "\n";
        return ExitError;
    }

    if (!json) {
        out() << "query      : " << QFileInfo(queryPath).fileName()
              << "  " << queryImage.width() << "x" << queryImage.height()
              << "  " << queryFeatures.count() << " keypoints\n"
              << "normalised : " << (matcher->normalizeKeypoints() ? "yes" : "no")
              << "   matcher on " << matcher->provider() << "\n\n";
    }

    QJsonArray results;
    bool anyVerified = false;

    for (const QString &assetPath : assetPaths) {
        const QImage assetImage = readImage(assetPath);
        if (assetImage.isNull()) {
            err() << "error: cannot read " << assetPath << "\n";
            continue;
        }

        const argus::FeatureSet assetFeatures = extractor->extract(assetImage, extractorOptions, &error);

        QElapsedTimer timer;
        timer.start();
        const QList<argus::FeatureMatch> matches = matcher->match(queryFeatures, assetFeatures, &error);
        const argus::GeometryResult geometry = argus::verifyHomography(queryFeatures, assetFeatures, matches);
        const qint64 ms = timer.elapsed();

        anyVerified = anyVerified || geometry.ok;

        QJsonObject o;
        o.insert(QStringLiteral("asset"), assetPath);
        o.insert(QStringLiteral("asset_keypoints"), assetFeatures.count());
        o.insert(QStringLiteral("matches"), static_cast<int>(matches.size()));
        o.insert(QStringLiteral("inliers"), geometry.inliers);
        o.insert(QStringLiteral("inlier_ratio"), geometry.inlierRatio);
        o.insert(QStringLiteral("verified"), geometry.ok);
        o.insert(QStringLiteral("ms"), static_cast<double>(ms));
        if (geometry.ok) {
            const QRectF box = geometry.boundingBox;
            QJsonObject b;
            b.insert(QStringLiteral("x"), box.x());
            b.insert(QStringLiteral("y"), box.y());
            b.insert(QStringLiteral("w"), box.width());
            b.insert(QStringLiteral("h"), box.height());
            o.insert(QStringLiteral("box"), b);
        }
        results.append(o);

        if (!json) {
            out() << "  " << QFileInfo(assetPath).fileName()
                  << "   kpts " << assetFeatures.count()
                  << "   matches " << matches.size()
                  << "   inliers " << geometry.inliers
                  << " (" << QString::number(geometry.inlierRatio * 100.0, 'f', 0) << "%)"
                  << "   " << (geometry.ok ? QStringLiteral("VERIFIED")
                                           : QStringLiteral("no [%1]").arg(geometry.rejection))
                  << "   " << ms << " ms\n";
            if (geometry.ok) {
                const QRectF box = geometry.boundingBox;
                out() << "      box at " << qRound(box.x()) << "," << qRound(box.y())
                      << "  " << qRound(box.width()) << "x" << qRound(box.height()) << "\n";
            }
        }
    }

    if (json) {
        QJsonObject root;
        root.insert(QStringLiteral("type"), QStringLiteral("match"));
        root.insert(QStringLiteral("query"), queryPath);
        root.insert(QStringLiteral("query_keypoints"), queryFeatures.count());
        root.insert(QStringLiteral("normalized"), matcher->normalizeKeypoints());
        root.insert(QStringLiteral("results"), results);
        out() << argus::toLine(root) << "\n";
    }
    out().flush();
    return anyVerified ? ExitFound : ExitNotFound;
}

// ---------------------------------------------------------------------------
// template — the correlation channel, on its own, for testing it
// ---------------------------------------------------------------------------

int cmdTemplate(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Locate an asset inside an image by masked correlation."));
    parser.addHelpOption();
    parser.addOption({ QStringLiteral("query"), QStringLiteral("Image to search in."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("asset"), QStringLiteral("Asset to look for. May be repeated."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("min-score"),
                       QStringLiteral("Correlation threshold, 0..1. Default: 0.90."),
                       QStringLiteral("v") });
    parser.addOption({ QStringLiteral("scales"),
                       QStringLiteral("Sizes tried between min and max. Default: 9."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("json"), QStringLiteral("Emit JSON.") });
    parser.process(args);

    const QString queryPath = parser.value(QStringLiteral("query"));
    const QStringList assets = parser.values(QStringLiteral("asset"));
    if (queryPath.isEmpty() || assets.isEmpty()) {
        err() << "error: --query and at least one --asset are required\n";
        return ExitError;
    }

    QImageReader reader(queryPath);
    reader.setAutoTransform(true);
    const QImage query = reader.read();
    if (query.isNull()) {
        err() << "error: cannot read " << queryPath << "\n";
        return ExitError;
    }

    argus::TemplateOptions options;
    if (parser.isSet(QStringLiteral("min-score")))
        options.minScore = parser.value(QStringLiteral("min-score")).toDouble();
    if (parser.isSet(QStringLiteral("scales")))
        options.scaleSteps = parser.value(QStringLiteral("scales")).toInt();

    const bool json = parser.isSet(QStringLiteral("json"));
    QJsonArray results;
    bool anyFound = false;

    for (const QString &assetPath : assets) {
        QImageReader assetReader(assetPath);
        assetReader.setAutoTransform(true);
        const QImage asset = assetReader.read();
        if (asset.isNull()) {
            err() << "error: cannot read " << assetPath << "\n";
            continue;
        }

        QElapsedTimer timer;
        timer.start();
        const argus::TemplateHit hit = argus::matchByTemplate(query, asset, options);
        const qint64 ms = timer.elapsed();
        anyFound = anyFound || hit.ok;

        if (json) {
            QJsonObject o;
            o.insert(QStringLiteral("type"), QStringLiteral("template"));
            o.insert(QStringLiteral("asset"), assetPath);
            o.insert(QStringLiteral("found"), hit.ok);
            o.insert(QStringLiteral("score"), hit.score);
            o.insert(QStringLiteral("scale"), hit.scale);
            o.insert(QStringLiteral("peak_z"), hit.peakZ);
            o.insert(QStringLiteral("ms"), static_cast<double>(ms));
            if (!hit.rejection.isEmpty())
                o.insert(QStringLiteral("rejected"), hit.rejection);
            QJsonObject box;
            box.insert(QStringLiteral("x"), hit.box.x());
            box.insert(QStringLiteral("y"), hit.box.y());
            box.insert(QStringLiteral("w"), hit.box.width());
            box.insert(QStringLiteral("h"), hit.box.height());
            o.insert(QStringLiteral("box"), box);
            results.append(o);
        } else {
            out() << QString::asprintf("  %-42s ", qPrintable(QFileInfo(assetPath).fileName()))
                  << QString::number(hit.score, 'f', 3)
                  << "  scale " << QString::number(hit.scale, 'f', 2)
                  << (hit.ok ? QStringLiteral("  FOUND") : QStringLiteral("  no"));
            if (hit.ok) {
                out() << " at " << qRound(hit.box.x()) << "," << qRound(hit.box.y())
                      << " " << qRound(hit.box.width()) << "x" << qRound(hit.box.height());
            } else if (!hit.rejection.isEmpty()) {
                out() << "  [" << hit.rejection << "]";
            }
            out() << "  " << ms << " ms\n";
        }
    }

    if (json) {
        for (const QJsonValue &v : std::as_const(results))
            out() << argus::toLine(v.toObject()) << "\n";
    }
    out().flush();
    return anyFound ? ExitFound : ExitNotFound;
}

// ---------------------------------------------------------------------------
// vocab
// ---------------------------------------------------------------------------

int cmdVocab(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Train the visual vocabulary and build the shortlist index."));
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("dir"), QStringLiteral("Indexed directory."));
    addCommonOptions(parser);
    parser.addOption({ QStringLiteral("words"),
                       QStringLiteral("Visual words to cluster. Default: 2048."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("sample"),
                       QStringLiteral("Descriptors used for training. Default: 120000."),
                       QStringLiteral("n") });
    parser.addOption({ QStringLiteral("model"),
                       QStringLiteral("Extractor identity. Default: disk."),
                       QStringLiteral("name") });
    parser.process(args);

    CommonOptions common;
    if (!resolveCommon(parser, common, true))
        return ExitError;

    const QString model = parser.isSet(QStringLiteral("model"))
                              ? parser.value(QStringLiteral("model"))
                              : QStringLiteral("disk");

    argus::VocabularyOptions vocabOptions;
    if (parser.isSet(QStringLiteral("words")))
        vocabOptions.words = parser.value(QStringLiteral("words")).toInt();
    if (parser.isSet(QStringLiteral("sample")))
        vocabOptions.sampleDescriptors = parser.value(QStringLiteral("sample")).toInt();

    argus::Database db;
    if (!openDatabase(db, common.dbPath))
        return ExitError;

    const QString featureDir = QFileInfo(common.dbPath).absolutePath() + QStringLiteral("/features");
    argus::DescriptorStore store;
    QString error;
    if (!store.open(featureDir, &error)) {
        err() << "error: " << error << "\n";
        return ExitError;
    }

    const QList<argus::FeatureRecord> records = db.allFeatures(model);
    if (records.isEmpty()) {
        err() << "error: no descriptors for model '" << model << "'\n"
              << "hint: run 'argus index " << common.root << " --features' first\n";
        return ExitError;
    }

    // Sample evenly across the whole library rather than taking the first N
    // images, so the vocabulary is not dominated by one folder.
    qint64 totalKeypoints = 0;
    for (const argus::FeatureRecord &r : records)
        totalKeypoints += r.count;
    const int stride = std::max<qint64>(
        1, totalKeypoints / std::max(1, vocabOptions.sampleDescriptors));

    if (!common.quiet) {
        err() << records.size() << " images, " << totalKeypoints
              << " keypoints; sampling every " << stride << "\n";
        err().flush();
    }

    QList<float> sample;
    int dim = 0;
    qint64 seen = 0;
    for (const argus::FeatureRecord &record : records) {
        argus::FeatureLocation location;
        location.descOffset = record.descOffset;
        location.kptsOffset = record.kptsOffset;
        location.count      = record.count;
        location.dim        = record.dim;

        const argus::FeatureSet features = store.load(location, record.imageWidth, record.imageHeight);
        if (features.isEmpty())
            continue;
        dim = features.dim;

        for (int i = 0; i < features.count(); ++i, ++seen) {
            if (seen % stride != 0)
                continue;
            for (int k = 0; k < dim; ++k)
                sample.append(features.descriptors.at(i * dim + k));
        }
    }

    if (dim <= 0 || sample.isEmpty()) {
        err() << "error: no descriptors could be read from " << featureDir << "\n";
        return ExitError;
    }

    QElapsedTimer timer;
    timer.start();
    auto vocabulary = argus::Vocabulary::train(sample, dim, model, vocabOptions, &error);
    if (!vocabulary) {
        err() << "error: " << error << "\n";
        return ExitError;
    }
    const qint64 trainMs = timer.elapsed();

    const QString vocabPath = featureDir + QStringLiteral("/vocab.bin");
    if (!vocabulary->save(vocabPath, &error)) {
        err() << "error: " << error << "\n";
        return ExitError;
    }

    timer.restart();
    auto index = argus::BowIndex::build(db, store, *vocabulary, model, nullptr,
                                     [&](int done, int total) {
                                         if (common.quiet)
                                             return;
                                         err() << "\rhistograms: " << done << "/" << total << "  ";
                                         err().flush();
                                     },
                                     &error);
    if (!common.quiet)
        err() << "\r                                   \r";
    if (!index) {
        err() << "error: " << error << "\n";
        return ExitError;
    }
    const qint64 buildMs = timer.elapsed();

    const QString bowPath = featureDir + QStringLiteral("/bow.bin");
    if (!index->save(bowPath, &error)) {
        err() << "error: " << error << "\n";
        return ExitError;
    }

    const argus::BowStats stats = index->stats();

    if (common.json) {
        QJsonObject o;
        o.insert(QStringLiteral("type"), QStringLiteral("vocab"));
        o.insert(QStringLiteral("model"), model);
        o.insert(QStringLiteral("words"), stats.words);
        o.insert(QStringLiteral("documents"), stats.documents);
        o.insert(QStringLiteral("postings"), static_cast<double>(stats.postings));
        o.insert(QStringLiteral("sampled"), static_cast<double>(sample.size() / dim));
        o.insert(QStringLiteral("train_ms"), static_cast<double>(trainMs));
        o.insert(QStringLiteral("build_ms"), static_cast<double>(buildMs));
        o.insert(QStringLiteral("vocab_path"), vocabPath);
        o.insert(QStringLiteral("bow_path"), bowPath);
        out() << argus::toLine(o) << "\n";
    } else {
        out() << "sampled    : " << (sample.size() / dim) << " descriptors of " << dim << "d\n"
              << "vocabulary : " << stats.words << " words, trained in "
              << (trainMs / 1000.0) << " s\n"
              << "index      : " << stats.documents << " images, " << stats.postings
              << " postings, built in " << (buildMs / 1000.0) << " s\n"
              << "written    : " << vocabPath << "\n"
              << "             " << bowPath << "\n";
    }
    out().flush();
    return ExitFound;
}

// ---------------------------------------------------------------------------
// doctor
// ---------------------------------------------------------------------------

/** @brief Render one tensor signature as "name: type[d0,d1,...]". */
QString describeTensor(const argus::TensorSpec &spec)
{
    QStringList dims;
    for (qint64 d : spec.shape)
        dims << (d < 0 ? QStringLiteral("?") : QString::number(d));
    return QStringLiteral("%1: %2[%3]").arg(spec.name, spec.type, dims.join(QLatin1Char(',')));
}

int cmdDoctor(const QStringList &args)
{
    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Report what the neural backend can do on this machine."));
    parser.addHelpOption();
    parser.addOption({ QStringLiteral("model"),
                       QStringLiteral("Inspect this .onnx file. May be repeated."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("cpu"),
                       QStringLiteral("Inspect models on the CPU provider instead of DirectML.") });
    parser.addOption({ QStringLiteral("extract"),
                       QStringLiteral("Run the extractor over this image and report what it found. "
                                      "May be repeated."),
                       QStringLiteral("image") });
    parser.addOption({ QStringLiteral("extractor"),
                       QStringLiteral("Extractor model used by --extract. Default: models/disk.onnx."),
                       QStringLiteral("file") });
    parser.addOption({ QStringLiteral("json"), QStringLiteral("Emit JSON.") });
    parser.process(args);

    const bool json = parser.isSet(QStringLiteral("json"));
    const bool preferDml = !parser.isSet(QStringLiteral("cpu"));
    const argus::RuntimeInfo rt = argus::probeRuntime();

    QJsonObject root;
    root.insert(QStringLiteral("type"), QStringLiteral("doctor"));
    root.insert(QStringLiteral("inference_built"), rt.built);
    root.insert(QStringLiteral("onnxruntime"), rt.ortVersion);
    root.insert(QStringLiteral("opencv"), rt.opencvVersion);
    root.insert(QStringLiteral("providers"), QJsonArray::fromStringList(rt.providers));
    root.insert(QStringLiteral("gpu_provider"), rt.gpuProvider);
    root.insert(QStringLiteral("gpu_usable"), rt.gpuUsable);
    if (!rt.gpuError.isEmpty())
        root.insert(QStringLiteral("gpu_error"), rt.gpuError);

    if (!json) {
        out() << "inference build : " << (rt.built ? "yes" : "no") << "\n";
        if (!rt.built) {
            out() << "hint            : reconfigure with -DARGUS_WITH_INFERENCE=ON\n";
            out().flush();
            return ExitNotFound;
        }
        out() << "onnxruntime     : " << rt.ortVersion << "\n"
              << "opencv          : " << rt.opencvVersion << "\n"
              << "providers       : " << rt.providers.join(QStringLiteral(", ")) << "\n"
              << "accelerator     : " << rt.gpuProvider
              << (rt.gpuUsable ? "  (usable)" : "  (falling back to CPU)") << "\n";
        if (!rt.gpuError.isEmpty())
            out() << "accelerator note: " << rt.gpuError << "\n";
    }

    QJsonArray models;
    bool allOk = true;
    for (const QString &path : parser.values(QStringLiteral("model"))) {
        const argus::ModelInfo info = argus::inspectModel(path, preferDml);
        allOk = allOk && info.ok;

        QJsonObject m;
        m.insert(QStringLiteral("path"), path);
        m.insert(QStringLiteral("ok"), info.ok);
        m.insert(QStringLiteral("provider"), info.provider);
        m.insert(QStringLiteral("load_ms"), static_cast<double>(info.loadMs));
        if (!info.error.isEmpty())
            m.insert(QStringLiteral("error"), info.error);

        QJsonArray ins, outs;
        for (const argus::TensorSpec &s : info.inputs)
            ins.append(describeTensor(s));
        for (const argus::TensorSpec &s : info.outputs)
            outs.append(describeTensor(s));
        m.insert(QStringLiteral("inputs"), ins);
        m.insert(QStringLiteral("outputs"), outs);
        models.append(m);

        if (!json) {
            out() << "\nmodel           : " << QFileInfo(path).fileName() << "\n";
            if (!info.ok) {
                out() << "  FAILED        : " << info.error << "\n";
                continue;
            }
            out() << "  provider      : " << info.provider
                  << "   (loaded in " << info.loadMs << " ms)\n";
            for (const argus::TensorSpec &s : info.inputs)
                out() << "  in            : " << describeTensor(s) << "\n";
            for (const argus::TensorSpec &s : info.outputs)
                out() << "  out           : " << describeTensor(s) << "\n";
        }
    }
    root.insert(QStringLiteral("models"), models);

    // ---- optional live extraction check -------------------------------------
    const QStringList images = parser.values(QStringLiteral("extract"));
    if (!images.isEmpty()) {
        const QString extractorPath = parser.isSet(QStringLiteral("extractor"))
                                          ? parser.value(QStringLiteral("extractor"))
                                          : argus::defaultModelsDir() + QStringLiteral("/disk.onnx");

        QString error;
        auto extractor = argus::FeatureExtractor::create(extractorPath, preferDml, &error);
        if (!extractor) {
            err() << "error: " << error << "\n";
            return ExitError;
        }
        if (!json)
            out() << "\nextractor       : " << QFileInfo(extractorPath).fileName()
                  << " on " << extractor->provider() << "\n";

        QJsonArray extractions;
        for (const QString &imagePath : images) {
            QImageReader reader(imagePath);
            reader.setAutoTransform(true);
            const QImage image = reader.read();
            if (image.isNull()) {
                err() << "error: cannot read " << imagePath << ": " << reader.errorString() << "\n";
                allOk = false;
                continue;
            }

            QElapsedTimer timer;
            timer.start();
            argus::ExtractorOptions eo;
            const argus::FeatureSet features = extractor->extract(image, eo, &error);
            const qint64 ms = timer.elapsed();

            QJsonObject e;
            e.insert(QStringLiteral("image"), imagePath);
            e.insert(QStringLiteral("width"), image.width());
            e.insert(QStringLiteral("height"), image.height());
            e.insert(QStringLiteral("keypoints"), features.count());
            e.insert(QStringLiteral("dim"), features.dim);
            e.insert(QStringLiteral("ms"), static_cast<double>(ms));
            if (features.isEmpty() && !error.isEmpty())
                e.insert(QStringLiteral("error"), error);
            extractions.append(e);

            if (!json) {
                out() << "  " << QFileInfo(imagePath).fileName()
                      << "  " << image.width() << "x" << image.height()
                      << "  ->  " << features.count() << " keypoints x " << features.dim << "d"
                      << "  in " << ms << " ms";
                if (features.isEmpty() && !error.isEmpty())
                    out() << "   [" << error << "]";
                out() << "\n";
            }
            if (features.isEmpty())
                allOk = false;
        }
        root.insert(QStringLiteral("extractions"), extractions);
    }

    if (json)
        out() << argus::toLine(root) << "\n";
    out().flush();

    // A CPU-only machine is a working machine, just a slower one; only a real
    // failure to load or run a model is worth a non-zero exit here.
    return allOk ? ExitFound : ExitNotFound;
}

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("argus"));
    QCoreApplication::setApplicationVersion(QStringLiteral(ARGUS_VERSION));

    QStringList args = QCoreApplication::arguments();
    const QString command = args.size() > 1 ? args.at(1) : QString();

    if (command.isEmpty()) {
        printUsage();
        return ExitError;
    }
    if (command == QLatin1String("-h") || command == QLatin1String("--help")
        || command == QLatin1String("help")) {
        printUsage();
        return ExitFound;
    }
    if (command == QLatin1String("-v") || command == QLatin1String("--version")
        || command == QLatin1String("version")) {
        out() << "argus " << ARGUS_VERSION << "\n";
        out().flush();
        return ExitFound;
    }

    args.removeAt(1); // parsers below expect argv[0] followed by options

    if (command == QLatin1String("index"))
        return cmdIndex(args);
    if (command == QLatin1String("dupes"))
        return cmdDupes(args);
    if (command == QLatin1String("query"))
        return cmdQuery(args);
    if (command == QLatin1String("stats"))
        return cmdStats(args);
    if (command == QLatin1String("formats"))
        return cmdFormats(args);
    if (command == QLatin1String("vocab"))
        return cmdVocab(args);
    if (command == QLatin1String("find"))
        return cmdFind(args);
    if (command == QLatin1String("match"))
        return cmdMatch(args);
    if (command == QLatin1String("template"))
        return cmdTemplate(args);
    if (command == QLatin1String("doctor"))
        return cmdDoctor(args);

    err() << "error: unknown command '" << command << "'\n\n";
    printUsage();
    return ExitError;
}
