#include "core/Vocabulary.h"

#include <QFile>
#include <QFileInfo>

#include <algorithm>

#ifdef IMAGEWORKER_WITH_INFERENCE
#include <opencv2/core.hpp>
#include <opencv2/flann.hpp>
#endif

namespace iw {

namespace {

/// File magic, so a stale or foreign file is rejected instead of misread.
constexpr quint32 kMagic   = 0x49575643u; // "IWVC"
constexpr quint32 kVersion = 1u;

/// Fewer than this many descriptors per cluster and the clusters model noise.
constexpr int kMinSamplesPerWord = 12;

} // namespace

#ifdef IMAGEWORKER_WITH_INFERENCE

struct Vocabulary::Impl {
    cv::Mat centres;                       ///< words x dim, CV_32F.
    std::unique_ptr<cv::flann::Index> index;
    QString modelId;

    void rebuildIndex()
    {
        index.reset();
        if (centres.empty())
            return;
        // A handful of randomised kd-trees turns nearest-word lookup from a
        // linear scan over every centre into a logarithmic one.
        index = std::make_unique<cv::flann::Index>(centres, cv::flann::KDTreeIndexParams(4));
    }
};

Vocabulary::Vocabulary()
    : d(std::make_unique<Impl>())
{
}

Vocabulary::~Vocabulary() = default;

bool Vocabulary::isValid() const { return !d->centres.empty() && d->index != nullptr; }
int  Vocabulary::words() const   { return d->centres.rows; }
int  Vocabulary::dim() const     { return d->centres.cols; }
QString Vocabulary::modelId() const { return d->modelId; }

std::unique_ptr<Vocabulary> Vocabulary::train(const QList<float> &sample,
                                              int dim,
                                              const QString &modelId,
                                              const VocabularyOptions &options,
                                              QString *error)
{
    if (dim <= 0 || sample.isEmpty() || sample.size() % dim != 0) {
        if (error)
            *error = QStringLiteral("descriptor sample is empty or not a multiple of %1").arg(dim);
        return nullptr;
    }

    const int available = static_cast<int>(sample.size() / dim);
    int words = std::max(2, options.words);
    if (available / kMinSamplesPerWord < words) {
        words = std::max(2, available / kMinSamplesPerWord);
    }
    if (words >= available) {
        if (error)
            *error = QStringLiteral("only %1 descriptors available, too few to cluster")
                         .arg(available);
        return nullptr;
    }

    try {
        // cv::kmeans wants a non-owning view; the sample outlives the call.
        const cv::Mat data(available, dim, CV_32F,
                           const_cast<float *>(sample.constData()));

        cv::theRNG().state = options.seed; // reproducible centres

        cv::Mat labels;
        cv::Mat centres;
        const cv::TermCriteria criteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
                                        options.iterations, options.epsilon);
        cv::kmeans(data, words, labels, criteria, 1, cv::KMEANS_PP_CENTERS, centres);

        std::unique_ptr<Vocabulary> self(new Vocabulary);
        self->d->centres = centres.clone(); // own the memory
        self->d->modelId = modelId;
        self->d->rebuildIndex();
        return self;
    } catch (const std::exception &e) {
        if (error)
            *error = QString::fromUtf8(e.what());
        return nullptr;
    }
}

bool Vocabulary::save(const QString &path, QString *error) const
{
    if (!isValid()) {
        if (error)
            *error = QStringLiteral("vocabulary is empty");
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error)
            *error = QStringLiteral("cannot write %1").arg(path);
        return false;
    }

    const QByteArray model = d->modelId.toUtf8();
    const quint32 words = static_cast<quint32>(d->centres.rows);
    const quint32 dims  = static_cast<quint32>(d->centres.cols);
    const quint32 nameLength = static_cast<quint32>(model.size());

    file.write(reinterpret_cast<const char *>(&kMagic), sizeof(kMagic));
    file.write(reinterpret_cast<const char *>(&kVersion), sizeof(kVersion));
    file.write(reinterpret_cast<const char *>(&words), sizeof(words));
    file.write(reinterpret_cast<const char *>(&dims), sizeof(dims));
    file.write(reinterpret_cast<const char *>(&nameLength), sizeof(nameLength));
    file.write(model);

    for (int r = 0; r < d->centres.rows; ++r) {
        file.write(reinterpret_cast<const char *>(d->centres.ptr<float>(r)),
                   qint64(dims) * qint64(sizeof(float)));
    }
    return true;
}

std::unique_ptr<Vocabulary> Vocabulary::load(const QString &path, QString *error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error)
            *error = QStringLiteral("cannot read %1").arg(path);
        return nullptr;
    }

    quint32 magic = 0, version = 0, words = 0, dims = 0, nameLength = 0;
    file.read(reinterpret_cast<char *>(&magic), sizeof(magic));
    file.read(reinterpret_cast<char *>(&version), sizeof(version));
    file.read(reinterpret_cast<char *>(&words), sizeof(words));
    file.read(reinterpret_cast<char *>(&dims), sizeof(dims));
    file.read(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));

    if (magic != kMagic || version != kVersion || words == 0 || dims == 0) {
        if (error)
            *error = QStringLiteral("%1 is not a vocabulary file this build understands").arg(path);
        return nullptr;
    }

    const QByteArray model = file.read(nameLength);

    std::unique_ptr<Vocabulary> self(new Vocabulary);
    self->d->modelId = QString::fromUtf8(model);
    self->d->centres = cv::Mat(static_cast<int>(words), static_cast<int>(dims), CV_32F);

    for (quint32 r = 0; r < words; ++r) {
        const qint64 want = qint64(dims) * qint64(sizeof(float));
        if (file.read(reinterpret_cast<char *>(self->d->centres.ptr<float>(int(r))), want) != want) {
            if (error)
                *error = QStringLiteral("%1 is truncated").arg(path);
            return nullptr;
        }
    }

    self->d->rebuildIndex();
    return self;
}

QList<quint32> Vocabulary::assign(const QList<float> &descriptors, int count) const
{
    QList<quint32> out;
    if (!isValid() || count <= 0)
        return out;
    if (descriptors.size() != static_cast<qsizetype>(count) * dim())
        return out;

    try {
        const cv::Mat queries(count, dim(), CV_32F,
                              const_cast<float *>(descriptors.constData()));
        cv::Mat indices, distances;
        d->index->knnSearch(queries, indices, distances, 1, cv::flann::SearchParams(32));

        out.reserve(count);
        for (int i = 0; i < count; ++i)
            out.append(static_cast<quint32>(indices.at<int>(i, 0)));
    } catch (const std::exception &) {
        return {};
    }
    return out;
}

#else // IMAGEWORKER_WITH_INFERENCE

struct Vocabulary::Impl {};

Vocabulary::Vocabulary() : d(std::make_unique<Impl>()) {}
Vocabulary::~Vocabulary() = default;
bool Vocabulary::isValid() const { return false; }
int Vocabulary::words() const { return 0; }
int Vocabulary::dim() const { return 0; }
QString Vocabulary::modelId() const { return {}; }

std::unique_ptr<Vocabulary> Vocabulary::train(const QList<float> &, int, const QString &,
                                              const VocabularyOptions &, QString *error)
{
    if (error)
        *error = QStringLiteral("built without inference support");
    return nullptr;
}

bool Vocabulary::save(const QString &, QString *error) const
{
    if (error)
        *error = QStringLiteral("built without inference support");
    return false;
}

std::unique_ptr<Vocabulary> Vocabulary::load(const QString &, QString *error)
{
    if (error)
        *error = QStringLiteral("built without inference support");
    return nullptr;
}

QList<quint32> Vocabulary::assign(const QList<float> &, int) const { return {}; }

#endif // IMAGEWORKER_WITH_INFERENCE

} // namespace iw
