#include "core/BowIndex.h"

#include "core/Database.h"
#include "core/DescriptorStore.h"
#include "core/Vocabulary.h"

#include <QFile>
#include <QHash>

#include <algorithm>
#include <cmath>
#include <vector>

namespace iw {

namespace {

constexpr quint32 kMagic   = 0x49574257u; // "IWBW"
constexpr quint32 kVersion = 1u;

/** @brief One word's contribution to one document. */
struct Posting {
    quint32 document = 0;
    float   weight   = 0.0f;
};

/** @brief Turn raw word counts into an L2-normalised tf-idf vector. */
void weighByIdf(QHash<quint32, float> &histogram, const std::vector<float> &idf)
{
    double norm = 0.0;
    for (auto it = histogram.begin(); it != histogram.end(); ++it) {
        const quint32 word = it.key();
        const float weight = it.value() * (word < idf.size() ? idf[word] : 0.0f);
        it.value() = weight;
        norm += double(weight) * weight;
    }
    if (norm <= 0.0)
        return;

    const float inverse = static_cast<float>(1.0 / std::sqrt(norm));
    for (auto it = histogram.begin(); it != histogram.end(); ++it)
        it.value() *= inverse;
}

} // namespace

struct BowIndex::Impl {
    quint32                       wordCount = 0;
    std::vector<float>            idf;
    std::vector<qint64>           documentIds;
    std::vector<std::vector<Posting>> inverted; ///< Indexed by word id.
    qint64                        postings = 0;

    void buildInverted(const std::vector<QHash<quint32, float>> &documents)
    {
        inverted.assign(wordCount, {});
        postings = 0;
        for (size_t doc = 0; doc < documents.size(); ++doc) {
            for (auto it = documents[doc].constBegin(); it != documents[doc].constEnd(); ++it) {
                if (it.key() >= wordCount)
                    continue;
                inverted[it.key()].push_back({ static_cast<quint32>(doc), it.value() });
                ++postings;
            }
        }
    }
};

BowIndex::BowIndex()
    : d(std::make_unique<Impl>())
{
}

BowIndex::~BowIndex() = default;

bool BowIndex::isValid() const
{
    return !d->documentIds.empty() && d->wordCount > 0;
}

BowStats BowIndex::stats() const
{
    BowStats s;
    s.documents = static_cast<int>(d->documentIds.size());
    s.words     = static_cast<int>(d->wordCount);
    s.postings  = d->postings;
    return s;
}

std::unique_ptr<BowIndex> BowIndex::build(Database &db,
                                          DescriptorStore &store,
                                          const Vocabulary &vocabulary,
                                          const QString &model,
                                          const std::atomic_bool *cancel,
                                          const std::function<void(int, int)> &progress,
                                          QString *error)
{
    if (!vocabulary.isValid()) {
        if (error)
            *error = QStringLiteral("vocabulary is not loaded");
        return nullptr;
    }

    const QList<FeatureRecord> records = db.allFeatures(model);
    if (records.isEmpty()) {
        if (error)
            *error = QStringLiteral("no descriptors indexed for model '%1'").arg(model);
        return nullptr;
    }

    std::unique_ptr<BowIndex> self(new BowIndex);
    self->d->wordCount = static_cast<quint32>(vocabulary.words());

    std::vector<QHash<quint32, float>> documents;
    documents.reserve(records.size());
    self->d->documentIds.reserve(records.size());

    std::vector<int> documentFrequency(self->d->wordCount, 0);

    int done = 0;
    for (const FeatureRecord &record : records) {
        if (cancel && cancel->load(std::memory_order_relaxed)) {
            if (error)
                *error = QStringLiteral("cancelled");
            return nullptr;
        }

        FeatureLocation location;
        location.descOffset = record.descOffset;
        location.kptsOffset = record.kptsOffset;
        location.count      = record.count;
        location.dim        = record.dim;

        const FeatureSet features = store.load(location, record.imageWidth, record.imageHeight);
        const QList<quint32> words = vocabulary.assign(features.descriptors, features.count());

        QHash<quint32, float> histogram;
        histogram.reserve(words.size());
        for (quint32 word : words)
            histogram[word] += 1.0f;

        // Document frequency counts images, not occurrences.
        for (auto it = histogram.constBegin(); it != histogram.constEnd(); ++it) {
            if (it.key() < self->d->wordCount)
                ++documentFrequency[it.key()];
        }

        self->d->documentIds.push_back(record.fileId);
        documents.push_back(std::move(histogram));

        if (progress && (++done % 128 == 0))
            progress(done, static_cast<int>(records.size()));
    }

    // Standard inverse document frequency, guarded against empty words.
    const double total = static_cast<double>(documents.size());
    self->d->idf.assign(self->d->wordCount, 0.0f);
    for (quint32 w = 0; w < self->d->wordCount; ++w) {
        const int df = documentFrequency[w];
        self->d->idf[w] = df > 0 ? static_cast<float>(std::log(total / double(df))) : 0.0f;
    }

    for (QHash<quint32, float> &histogram : documents)
        weighByIdf(histogram, self->d->idf);

    self->d->buildInverted(documents);

    if (progress)
        progress(static_cast<int>(records.size()), static_cast<int>(records.size()));
    return self;
}

QList<BowHit> BowIndex::query(const QList<quint32> &queryWords, int topK) const
{
    QList<BowHit> out;
    if (!isValid() || queryWords.isEmpty())
        return out;

    QHash<quint32, float> histogram;
    histogram.reserve(queryWords.size());
    for (quint32 word : queryWords)
        histogram[word] += 1.0f;
    weighByIdf(histogram, d->idf);

    // Accumulate only over words the query actually uses: that is the whole
    // point of an inverted index.
    std::vector<float> scores(d->documentIds.size(), 0.0f);
    for (auto it = histogram.constBegin(); it != histogram.constEnd(); ++it) {
        if (it.key() >= d->wordCount)
            continue;
        const float queryWeight = it.value();
        for (const Posting &posting : d->inverted[it.key()])
            scores[posting.document] += queryWeight * posting.weight;
    }

    std::vector<quint32> order(scores.size());
    for (size_t i = 0; i < order.size(); ++i)
        order[i] = static_cast<quint32>(i);

    const size_t keep = std::min<size_t>(std::max(1, topK), order.size());
    std::partial_sort(order.begin(), order.begin() + static_cast<std::ptrdiff_t>(keep), order.end(),
                      [&](quint32 a, quint32 b) {
                          if (scores[a] != scores[b])
                              return scores[a] > scores[b];
                          return d->documentIds[a] < d->documentIds[b];
                      });

    out.reserve(static_cast<int>(keep));
    for (size_t i = 0; i < keep; ++i) {
        if (scores[order[i]] <= 0.0f)
            break;
        out.append({ d->documentIds[order[i]], scores[order[i]] });
    }
    return out;
}

bool BowIndex::save(const QString &path, QString *error) const
{
    if (!isValid()) {
        if (error)
            *error = QStringLiteral("index is empty");
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error)
            *error = QStringLiteral("cannot write %1").arg(path);
        return false;
    }

    const quint32 documents = static_cast<quint32>(d->documentIds.size());
    file.write(reinterpret_cast<const char *>(&kMagic), sizeof(kMagic));
    file.write(reinterpret_cast<const char *>(&kVersion), sizeof(kVersion));
    file.write(reinterpret_cast<const char *>(&d->wordCount), sizeof(d->wordCount));
    file.write(reinterpret_cast<const char *>(&documents), sizeof(documents));
    file.write(reinterpret_cast<const char *>(d->idf.data()),
               qint64(d->idf.size()) * qint64(sizeof(float)));

    // Postings are stored word-major, exactly as they are used at query time.
    for (quint32 w = 0; w < d->wordCount; ++w) {
        const quint32 n = static_cast<quint32>(d->inverted[w].size());
        file.write(reinterpret_cast<const char *>(&n), sizeof(n));
        if (n > 0) {
            file.write(reinterpret_cast<const char *>(d->inverted[w].data()),
                       qint64(n) * qint64(sizeof(Posting)));
        }
    }
    file.write(reinterpret_cast<const char *>(d->documentIds.data()),
               qint64(documents) * qint64(sizeof(qint64)));
    return true;
}

std::unique_ptr<BowIndex> BowIndex::load(const QString &path, QString *error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error)
            *error = QStringLiteral("cannot read %1").arg(path);
        return nullptr;
    }

    quint32 magic = 0, version = 0, wordCount = 0, documents = 0;
    file.read(reinterpret_cast<char *>(&magic), sizeof(magic));
    file.read(reinterpret_cast<char *>(&version), sizeof(version));
    file.read(reinterpret_cast<char *>(&wordCount), sizeof(wordCount));
    file.read(reinterpret_cast<char *>(&documents), sizeof(documents));

    if (magic != kMagic || version != kVersion || wordCount == 0 || documents == 0) {
        if (error)
            *error = QStringLiteral("%1 is not an index file this build understands").arg(path);
        return nullptr;
    }

    std::unique_ptr<BowIndex> self(new BowIndex);
    self->d->wordCount = wordCount;
    self->d->idf.resize(wordCount);
    file.read(reinterpret_cast<char *>(self->d->idf.data()),
              qint64(wordCount) * qint64(sizeof(float)));

    self->d->inverted.assign(wordCount, {});
    for (quint32 w = 0; w < wordCount; ++w) {
        quint32 n = 0;
        if (file.read(reinterpret_cast<char *>(&n), sizeof(n)) != sizeof(n)) {
            if (error)
                *error = QStringLiteral("%1 is truncated").arg(path);
            return nullptr;
        }
        if (n == 0)
            continue;
        self->d->inverted[w].resize(n);
        const qint64 want = qint64(n) * qint64(sizeof(Posting));
        if (file.read(reinterpret_cast<char *>(self->d->inverted[w].data()), want) != want) {
            if (error)
                *error = QStringLiteral("%1 is truncated").arg(path);
            return nullptr;
        }
        self->d->postings += n;
    }

    self->d->documentIds.resize(documents);
    const qint64 want = qint64(documents) * qint64(sizeof(qint64));
    if (file.read(reinterpret_cast<char *>(self->d->documentIds.data()), want) != want) {
        if (error)
            *error = QStringLiteral("%1 is truncated").arg(path);
        return nullptr;
    }

    return self;
}

} // namespace iw
