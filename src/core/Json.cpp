#include "core/Json.h"

#include "core/Paths.h"

#include <QJsonArray>
#include <QJsonDocument>

namespace iw {

QJsonObject toJson(const FileInfoRow &row, const QString &root)
{
    QJsonObject o;
    o.insert(QStringLiteral("rel"), row.rel);
    if (row.ref.isEmpty()) {
        if (!root.isEmpty())
            o.insert(QStringLiteral("path"), absolutePathFor(root, row.rel));
    } else {
        // No "path" for a row read out of a branch: there is no such file, and
        // a consumer that finds the key would go on to open it. What it gets
        // instead is git's own revision syntax, which `git show` accepts.
        o.insert(QStringLiteral("ref"), row.ref);
        o.insert(QStringLiteral("blob"), row.blob);
        o.insert(QStringLiteral("rev"), row.ref + QLatin1Char(':') + row.rel);
    }
    o.insert(QStringLiteral("size"), static_cast<double>(row.size));
    o.insert(QStringLiteral("width"), row.width);
    o.insert(QStringLiteral("height"), row.height);
    o.insert(QStringLiteral("mtime"), static_cast<double>(row.mtime));
    return o;
}

QJsonObject toJson(const Match &match, const QString &root)
{
    QJsonObject o = toJson(match.file, root);
    o.insert(QStringLiteral("type"), QStringLiteral("match"));
    o.insert(QStringLiteral("distance"), match.distance);
    o.insert(QStringLiteral("score"), match.score);
    return o;
}

QJsonObject toJson(const DuplicateGroup &group, const QString &root)
{
    QJsonArray files;
    for (const FileInfoRow &row : group.files)
        files.append(toJson(row, root));

    QJsonObject o;
    o.insert(QStringLiteral("type"), QStringLiteral("group"));
    o.insert(QStringLiteral("kind"),
             group.kind == GroupKind::Exact ? QStringLiteral("exact") : QStringLiteral("near"));
    o.insert(QStringLiteral("count"), static_cast<int>(group.files.size()));
    o.insert(QStringLiteral("wasted"), static_cast<double>(group.wastedBytes));
    o.insert(QStringLiteral("max_distance"), group.maxDistance);
    o.insert(QStringLiteral("files"), files);
    return o;
}

QJsonObject toJson(const IndexStats &stats)
{
    QJsonObject o;
    o.insert(QStringLiteral("type"), QStringLiteral("index_summary"));
    o.insert(QStringLiteral("scanned"), stats.scanned);
    o.insert(QStringLiteral("indexed"), stats.indexed);
    o.insert(QStringLiteral("skipped"), stats.skipped);
    o.insert(QStringLiteral("failed"), stats.failed);
    o.insert(QStringLiteral("pruned"), stats.pruned);
    o.insert(QStringLiteral("bytes_read"), static_cast<double>(stats.bytesRead));
    o.insert(QStringLiteral("elapsed_ms"), static_cast<double>(stats.elapsedMs));
    o.insert(QStringLiteral("cancelled"), stats.cancelled);
    o.insert(QStringLiteral("featured"), stats.featured);
    o.insert(QStringLiteral("feature_failed"), stats.featureFailed);
    o.insert(QStringLiteral("keypoints"), static_cast<double>(stats.keypointsTotal));
    o.insert(QStringLiteral("feature_bytes"), static_cast<double>(stats.featureBytes));
    o.insert(QStringLiteral("feature_provider"), stats.featureProvider);
    return o;
}

QJsonObject summaryJson(const DuplicateReport &report)
{
    QJsonObject o;
    o.insert(QStringLiteral("type"), QStringLiteral("dupes_summary"));
    o.insert(QStringLiteral("groups"), static_cast<int>(report.groups.size()));
    o.insert(QStringLiteral("wasted"), static_cast<double>(report.wastedBytes));
    o.insert(QStringLiteral("compared_pairs"), report.comparedPairs);
    o.insert(QStringLiteral("skipped_buckets"), report.skippedBuckets);
    o.insert(QStringLiteral("cancelled"), report.cancelled);
    return o;
}

QString toLine(const QJsonObject &object)
{
    return QString::fromUtf8(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

} // namespace iw
