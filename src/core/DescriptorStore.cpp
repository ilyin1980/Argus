#include "core/DescriptorStore.h"

#include <QDir>
#include <QFile>
#include <QFloat16>

namespace iw {

namespace {

constexpr const char *kDescName = "desc.f16";
constexpr const char *kKptsName = "kpts.f32";

/// Floats written per keypoint: x, y, score.
constexpr int kKeypointStride = 3;

} // namespace

struct DescriptorStore::Impl {
    QFile   desc;
    QFile   kpts;
    QString directory;
};

DescriptorStore::DescriptorStore()
    : d(std::make_unique<Impl>())
{
}

DescriptorStore::~DescriptorStore()
{
    close();
}

bool DescriptorStore::isOpen() const
{
    return d->desc.isOpen() && d->kpts.isOpen();
}

qint64 DescriptorStore::totalBytes() const
{
    return d->desc.size() + d->kpts.size();
}

bool DescriptorStore::open(const QString &directory, QString *error)
{
    close();

    if (!QDir().mkpath(directory)) {
        if (error)
            *error = QStringLiteral("cannot create %1").arg(QDir::toNativeSeparators(directory));
        return false;
    }
    d->directory = directory;

    d->desc.setFileName(directory + QLatin1Char('/') + QLatin1String(kDescName));
    d->kpts.setFileName(directory + QLatin1Char('/') + QLatin1String(kKptsName));

    // ReadWrite rather than Append: reads use the same handle, and appends seek
    // to the end explicitly.
    if (!d->desc.open(QIODevice::ReadWrite) || !d->kpts.open(QIODevice::ReadWrite)) {
        if (error)
            *error = QStringLiteral("cannot open descriptor files in %1")
                         .arg(QDir::toNativeSeparators(directory));
        close();
        return false;
    }
    return true;
}

void DescriptorStore::close()
{
    if (d->desc.isOpen())
        d->desc.close();
    if (d->kpts.isOpen())
        d->kpts.close();
}

bool DescriptorStore::clear(QString *error)
{
    if (!isOpen()) {
        if (error)
            *error = QStringLiteral("store is not open");
        return false;
    }
    if (!d->desc.resize(0) || !d->kpts.resize(0)) {
        if (error)
            *error = QStringLiteral("cannot truncate the descriptor files");
        return false;
    }
    return true;
}

bool DescriptorStore::append(const FeatureSet &features, FeatureLocation *location, QString *error)
{
    if (!isOpen()) {
        if (error)
            *error = QStringLiteral("store is not open");
        return false;
    }
    if (!location) {
        if (error)
            *error = QStringLiteral("no location out-parameter");
        return false;
    }

    location->count = features.count();
    location->dim   = features.dim;
    location->descOffset = d->desc.size();
    location->kptsOffset = d->kpts.size();

    if (features.isEmpty())
        return true; // a zero-count record is legitimate: nothing was detected

    if (!d->desc.seek(d->desc.size()) || !d->kpts.seek(d->kpts.size())) {
        if (error)
            *error = QStringLiteral("cannot seek to the end of the descriptor files");
        return false;
    }

    // Descriptors: float -> half, written contiguously row by row.
    QList<qfloat16> half;
    half.reserve(features.descriptors.size());
    for (float v : features.descriptors)
        half.append(qfloat16(v));

    const qint64 descBytes = static_cast<qint64>(half.size()) * qint64(sizeof(qfloat16));
    if (d->desc.write(reinterpret_cast<const char *>(half.constData()), descBytes) != descBytes) {
        if (error)
            *error = QStringLiteral("short write to %1").arg(QLatin1String(kDescName));
        return false;
    }

    // Keypoints stay in single precision: there are few of them and pixel
    // coordinates need the range.
    QList<float> packed;
    packed.reserve(features.count() * kKeypointStride);
    for (int i = 0; i < features.count(); ++i) {
        packed.append(features.keypoints.at(i * 2));
        packed.append(features.keypoints.at(i * 2 + 1));
        packed.append(features.scores.at(i));
    }

    const qint64 kptsBytes = static_cast<qint64>(packed.size()) * qint64(sizeof(float));
    if (d->kpts.write(reinterpret_cast<const char *>(packed.constData()), kptsBytes) != kptsBytes) {
        if (error)
            *error = QStringLiteral("short write to %1").arg(QLatin1String(kKptsName));
        return false;
    }

    return true;
}

FeatureSet DescriptorStore::load(const FeatureLocation &location,
                                 int imageWidth,
                                 int imageHeight) const
{
    FeatureSet out;
    out.imageWidth  = imageWidth;
    out.imageHeight = imageHeight;
    out.dim         = location.dim;

    if (!isOpen() || location.count <= 0 || location.dim <= 0)
        return out;

    const qint64 descBytes = static_cast<qint64>(location.count) * location.dim
                             * qint64(sizeof(qfloat16));
    const qint64 kptsBytes = static_cast<qint64>(location.count) * kKeypointStride
                             * qint64(sizeof(float));

    if (location.descOffset < 0 || location.descOffset + descBytes > d->desc.size())
        return out;
    if (location.kptsOffset < 0 || location.kptsOffset + kptsBytes > d->kpts.size())
        return out;

    QFile &desc = const_cast<QFile &>(d->desc);
    QFile &kpts = const_cast<QFile &>(d->kpts);

    if (!desc.seek(location.descOffset) || !kpts.seek(location.kptsOffset))
        return out;

    QByteArray descRaw = desc.read(descBytes);
    QByteArray kptsRaw = kpts.read(kptsBytes);
    if (descRaw.size() != descBytes || kptsRaw.size() != kptsBytes)
        return out;

    const auto *half = reinterpret_cast<const qfloat16 *>(descRaw.constData());
    out.descriptors.reserve(location.count * location.dim);
    for (qint64 i = 0; i < static_cast<qint64>(location.count) * location.dim; ++i)
        out.descriptors.append(static_cast<float>(half[i]));

    const auto *packed = reinterpret_cast<const float *>(kptsRaw.constData());
    out.keypoints.reserve(location.count * 2);
    out.scores.reserve(location.count);
    for (int i = 0; i < location.count; ++i) {
        out.keypoints.append(packed[i * kKeypointStride]);
        out.keypoints.append(packed[i * kKeypointStride + 1]);
        out.scores.append(packed[i * kKeypointStride + 2]);
    }

    return out;
}

} // namespace iw
