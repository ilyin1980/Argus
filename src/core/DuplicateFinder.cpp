#include "core/DuplicateFinder.h"

#include "core/Database.h"

#include <QHash>
#include <QSet>

#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

namespace argus {

namespace {

/** @brief Union-find over row indices, used to turn pairs into groups. */
class DisjointSet
{
public:
    explicit DisjointSet(int n)
        : m_parent(static_cast<size_t>(n))
        , m_rank(static_cast<size_t>(n), 0)
    {
        std::iota(m_parent.begin(), m_parent.end(), 0);
    }

    int find(int x)
    {
        while (m_parent[static_cast<size_t>(x)] != x) {
            m_parent[static_cast<size_t>(x)] =
                m_parent[static_cast<size_t>(m_parent[static_cast<size_t>(x)])];
            x = m_parent[static_cast<size_t>(x)];
        }
        return x;
    }

    void unite(int a, int b)
    {
        a = find(a);
        b = find(b);
        if (a == b)
            return;
        if (m_rank[static_cast<size_t>(a)] < m_rank[static_cast<size_t>(b)])
            std::swap(a, b);
        m_parent[static_cast<size_t>(b)] = a;
        if (m_rank[static_cast<size_t>(a)] == m_rank[static_cast<size_t>(b)])
            ++m_rank[static_cast<size_t>(a)];
    }

private:
    std::vector<int> m_parent;
    std::vector<int> m_rank;
};

/** @brief Extract the four 16-bit multi-index-hashing bands of a hash. */
inline void bandsOf(quint64 hash, int out[4])
{
    out[0] = static_cast<int>((hash >> 0)  & 0xFFFFu);
    out[1] = static_cast<int>((hash >> 16) & 0xFFFFu);
    out[2] = static_cast<int>((hash >> 32) & 0xFFFFu);
    out[3] = static_cast<int>((hash >> 48) & 0xFFFFu);
}

inline bool cancelled(const std::atomic_bool *cancel)
{
    return cancel && cancel->load(std::memory_order_relaxed);
}

/** @brief Order group members largest-first so the keeper is obvious. */
void sortMembers(QList<FileInfoRow> &files)
{
    std::sort(files.begin(), files.end(), [](const FileInfoRow &a, const FileInfoRow &b) {
        const qint64 pixelsA = qint64(a.width) * a.height;
        const qint64 pixelsB = qint64(b.width) * b.height;
        if (pixelsA != pixelsB)
            return pixelsA > pixelsB;
        if (a.size != b.size)
            return a.size > b.size;
        return a.rel < b.rel;
    });
}

/**
 * @brief Bytes that could be reclaimed by keeping only the first member.
 *
 * Copies living in a git branch are counted as members of the group, because
 * the group is the honest answer to "where else is this picture", but never as
 * reclaimable space: nothing on disk is freed by deleting them, and git stores
 * identical content once regardless of how many branches reference it.
 */
qint64 wastedIn(const QList<FileInfoRow> &files)
{
    qint64 total = 0;
    qint64 largest = 0;
    int onDisk = 0;
    for (const FileInfoRow &f : files) {
        if (!f.ref.isEmpty())
            continue;
        ++onDisk;
        total += f.size;
        largest = std::max(largest, f.size);
    }
    return onDisk < 2 ? 0 : total - largest;
}

} // namespace

DuplicateReport findDuplicates(Database &db,
                               const DuplicateOptions &options,
                               const std::atomic_bool *cancel,
                               const std::function<void(int, int, const QString &)> &progress)
{
    DuplicateReport report;

    if (progress)
        progress(0, 0, QStringLiteral("load"));

    const QList<CompactRow> rows = db.loadCompactRows();
    if (rows.isEmpty())
        return report;

    const int n = static_cast<int>(rows.size());

    // ---- 1. Exact groups, keyed by (size, short hash) -----------------------
    // Collisions on the 64-bit prefix are confirmed against the full digest.
    QHash<QPair<qint64, qint64>, QList<int>> exactBuckets;
    exactBuckets.reserve(n);
    for (int i = 0; i < n; ++i) {
        const CompactRow &row = rows.at(i);
        if (row.chash64 == 0)
            continue;
        exactBuckets[qMakePair(row.size, row.chash64)].append(i);
    }

    std::vector<int> representativeOf(static_cast<size_t>(n));
    std::iota(representativeOf.begin(), representativeOf.end(), 0);

    QList<QList<int>> exactGroups;
    /// Representative index -> every row folded under it, so the near pass can
    /// expand a group back out without rescanning all rows.
    QHash<int, QList<int>> membersOf;

    for (auto it = exactBuckets.constBegin(); it != exactBuckets.constEnd(); ++it) {
        if (it.value().size() < 2)
            continue;
        if (cancelled(cancel)) {
            report.cancelled = true;
            return report;
        }

        // Confirm with the full digest before calling anything identical.
        QHash<QByteArray, QList<int>> confirmed;
        for (int idx : it.value()) {
            const QByteArray digest = db.contentHash(rows.at(idx).id);
            if (digest.isEmpty())
                continue;
            confirmed[digest].append(idx);
        }
        for (auto c = confirmed.constBegin(); c != confirmed.constEnd(); ++c) {
            if (c.value().size() < 2)
                continue;
            exactGroups.append(c.value());
            membersOf.insert(c.value().first(), c.value());
            for (int idx : c.value())
                representativeOf[static_cast<size_t>(idx)] = c.value().first();
        }
    }

    // ---- 2. Near groups over exact-group representatives --------------------
    DisjointSet sets(n);
    int comparedPairs = 0;

    if (options.includeNear) {
        QList<int> candidates;
        candidates.reserve(n);
        for (int i = 0; i < n; ++i) {
            if (representativeOf[static_cast<size_t>(i)] != i)
                continue; // folded into an exact group already
            if (rows.at(i).phash == 0)
                continue; // undecodable or degenerate
            candidates.append(i);
        }

        QHash<int, QList<int>> buckets[4];
        for (int idx : candidates) {
            int bands[4] = { 0, 0, 0, 0 };
            bandsOf(rows.at(idx).phash, bands);
            for (int b = 0; b < 4; ++b)
                buckets[b][bands[b]].append(idx);
        }

        int bucketsDone = 0;
        int bucketsTotal = 0;
        for (int b = 0; b < 4; ++b)
            bucketsTotal += static_cast<int>(buckets[b].size());

        for (int b = 0; b < 4; ++b) {
            for (auto it = buckets[b].constBegin(); it != buckets[b].constEnd(); ++it) {
                if (cancelled(cancel)) {
                    report.cancelled = true;
                    return report;
                }
                if (progress && (++bucketsDone % 512 == 0))
                    progress(bucketsDone, bucketsTotal, QStringLiteral("match"));

                const QList<int> &bucket = it.value();
                if (bucket.size() < 2)
                    continue;
                if (options.bucketLimit > 0 && bucket.size() > options.bucketLimit) {
                    // Degenerate bucket (flat images, solid colours). Expanding it
                    // would be quadratic; report the skip rather than hide it.
                    ++report.skippedBuckets;
                    continue;
                }

                // No pair-dedup set: a pair that appears in several bands is
                // simply united twice, which union-find handles for free and
                // which costs far less than remembering every pair seen.
                for (int i = 0; i < bucket.size(); ++i) {
                    for (int j = i + 1; j < bucket.size(); ++j) {
                        const int a = bucket.at(i);
                        const int c = bucket.at(j);
                        ++comparedPairs;

                        if (hamming(rows.at(a).phash, rows.at(c).phash) > options.maxDistance)
                            continue;
                        if (options.requireBothHashes
                            && hamming(rows.at(a).dhash, rows.at(c).dhash) > options.maxDistance) {
                            continue;
                        }
                        sets.unite(a, c);
                    }
                }
            }
        }
    }

    report.comparedPairs = comparedPairs;

    // ---- 3. Materialise -----------------------------------------------------
    if (progress)
        progress(0, 0, QStringLiteral("collect"));

    QList<DuplicateGroup> out;

    // Near groups are built first so that an exact group already contained in
    // one can be suppressed: reporting both would list the same files twice and
    // double-count the reclaimable bytes.
    QSet<qint64> absorbedByNear;

    if (options.includeNear) {
        QHash<int, QList<int>> nearGroups;
        for (int i = 0; i < n; ++i) {
            if (representativeOf[static_cast<size_t>(i)] != i)
                continue;
            if (rows.at(i).phash == 0)
                continue;
            const int root = sets.find(i);
            nearGroups[root].append(i);
        }

        for (auto it = nearGroups.constBegin(); it != nearGroups.constEnd(); ++it) {
            if (it.value().size() < 2)
                continue;

            // Pull in every member of each folded exact group.
            QList<qint64> ids;
            for (int representative : it.value()) {
                const auto members = membersOf.constFind(representative);
                if (members != membersOf.constEnd()) {
                    for (int idx : *members)
                        ids.append(rows.at(idx).id);
                } else {
                    ids.append(rows.at(representative).id);
                }
            }

            DuplicateGroup g;
            g.kind  = GroupKind::Near;
            g.files = db.filesByIds(ids);
            if (g.files.size() < 2)
                continue;
            sortMembers(g.files);
            g.wastedBytes = wastedIn(g.files);

            int worst = 0;
            for (int a = 0; a < g.files.size(); ++a)
                for (int b = a + 1; b < g.files.size(); ++b)
                    worst = std::max(worst, hamming(g.files.at(a).phash, g.files.at(b).phash));
            g.maxDistance = worst;

            for (const FileInfoRow &file : std::as_const(g.files))
                absorbedByNear.insert(file.id);

            out.append(g);
        }
    }

    if (options.includeExact) {
        for (const QList<int> &group : std::as_const(exactGroups)) {
            QList<qint64> ids;
            ids.reserve(group.size());
            bool allAbsorbed = true;
            for (int idx : group) {
                const qint64 id = rows.at(idx).id;
                ids.append(id);
                if (!absorbedByNear.contains(id))
                    allAbsorbed = false;
            }
            if (allAbsorbed && !absorbedByNear.isEmpty())
                continue; // already reported, in full, as part of a near group

            DuplicateGroup g;
            g.kind  = GroupKind::Exact;
            g.files = db.filesByIds(ids);
            if (g.files.size() < 2)
                continue;
            sortMembers(g.files);
            g.wastedBytes = wastedIn(g.files);
            out.append(g);
        }
    }

    std::sort(out.begin(), out.end(), [](const DuplicateGroup &a, const DuplicateGroup &b) {
        if (a.wastedBytes != b.wastedBytes)
            return a.wastedBytes > b.wastedBytes;
        return a.files.first().rel < b.files.first().rel;
    });

    for (const DuplicateGroup &g : std::as_const(out))
        report.wastedBytes += g.wastedBytes;

    report.groups = out;
    return report;
}

} // namespace argus
