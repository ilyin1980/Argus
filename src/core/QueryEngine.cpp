#include "core/QueryEngine.h"

#include "core/Database.h"
#include "core/ImageDecoder.h"
#include "core/PerceptualHash.h"

#include <QHash>

#include <algorithm>
#include <utility>
#include <vector>

namespace argus {

namespace {

/** @brief A row's distance to the query, before paths are materialised. */
struct ScoredId {
    qint64 id       = 0;
    int    distance = 0;
};

} // namespace

QueryResult queryByHashes(Database &db,
                          quint64 phash,
                          quint64 dhash,
                          const QueryOptions &options)
{
    QueryResult result;
    result.queryPhash = phash;
    result.queryDhash = dhash;

    const QList<CompactRow> rows = db.loadCompactRows();
    result.scannedRows = static_cast<int>(rows.size());
    if (rows.isEmpty())
        return result;

    // Both fingerprints contribute equally; combined range is 0..128.
    std::vector<ScoredId> scored;
    scored.reserve(rows.size());
    for (const CompactRow &row : rows) {
        // No zero-hash guard here. A flat colour hashes to zero on both
        // fingerprints because it has neither frequency content nor gradients,
        // and skipping those rows made an ordinary 16x16 black square
        // unfindable by its own file. loadCompactRows() already returns only
        // rows that decoded, so a zero here means flat, not unread.
        const int distance = hamming(row.phash, phash) + hamming(row.dhash, dhash);
        if (distance > options.maxDistance)
            continue;
        scored.push_back({ row.id, distance });
    }
    if (scored.empty())
        return result;

    const int topK = std::max(1, options.topK);
    const size_t keep = std::min(static_cast<size_t>(topK), scored.size());

    std::partial_sort(scored.begin(),
                      scored.begin() + static_cast<std::ptrdiff_t>(keep),
                      scored.end(),
                      [](const ScoredId &a, const ScoredId &b) {
                          if (a.distance != b.distance)
                              return a.distance < b.distance;
                          return a.id < b.id;
                      });
    scored.resize(keep);

    QList<qint64> ids;
    ids.reserve(static_cast<int>(keep));
    for (const ScoredId &s : scored)
        ids.append(s.id);

    QHash<qint64, FileInfoRow> byId;
    for (const FileInfoRow &row : db.filesByIds(ids))
        byId.insert(row.id, row);

    result.matches.reserve(static_cast<int>(keep));
    for (const ScoredId &s : scored) {
        const auto it = byId.constFind(s.id);
        if (it == byId.constEnd())
            continue;
        Match m;
        m.file     = *it;
        m.distance = s.distance;
        m.score    = 1.0 - static_cast<double>(s.distance) / 128.0;
        result.matches.append(m);
    }
    return result;
}

namespace {

/** @brief Shared tail of both query-by-example entry points. */
QueryResult finishQuery(Database &db, const DecodedImage &decoded, const QueryOptions &options)
{
    const quint64 phash = dctPerceptualHash(decoded.gray32);
    const quint64 dhash = differenceHash(decoded.gray9x8);
    return queryByHashes(db, phash, dhash, options);
}

} // namespace

QueryResult queryByImage(Database &db,
                         const QString &imagePath,
                         const QueryOptions &options,
                         QString *error)
{
    DecodeRequest request;
    request.wantThumbnail   = false;
    request.alphaBackground = options.alphaBackground;

    const DecodedImage decoded = decodeForIndex(imagePath, request);
    if (!decoded.ok) {
        if (error)
            *error = QStringLiteral("cannot decode %1: %2").arg(imagePath, decoded.error);
        return {};
    }
    return finishQuery(db, decoded, options);
}

QueryResult queryByImage(Database &db,
                         const QImage &image,
                         const QueryOptions &options,
                         QString *error)
{
    DecodeRequest request;
    request.wantThumbnail   = false;
    request.alphaBackground = options.alphaBackground;

    const DecodedImage decoded = decodeFromImage(image, request);
    if (!decoded.ok) {
        if (error)
            *error = QStringLiteral("cannot use the supplied image: %1").arg(decoded.error);
        return {};
    }
    return finishQuery(db, decoded, options);
}

} // namespace argus
