#include "core/Database.h"

#include "core/ContentHash.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

#include <algorithm>
#include <atomic>

namespace iw {

namespace {

/** @brief Serial number keeping QSqlDatabase connection names unique per instance. */
std::atomic<quint64> g_connectionSerial{0};

/** @brief Split a 64-bit hash into the four 16-bit bands used for multi-index hashing. */
inline void bandsOf(quint64 hash, int out[4])
{
    out[0] = static_cast<int>((hash >> 0)  & 0xFFFFu);
    out[1] = static_cast<int>((hash >> 16) & 0xFFFFu);
    out[2] = static_cast<int>((hash >> 32) & 0xFFFFu);
    out[3] = static_cast<int>((hash >> 48) & 0xFFFFu);
}

/**
 * @brief Read an unsigned hash back out of SQLite's signed integer storage.
 * @note QVariant::toULongLong() refuses negative values, so go through the
 *       signed accessor and reinterpret the bits.
 */
inline quint64 toHash(const QVariant &v)
{
    return static_cast<quint64>(v.toLongLong());
}

/** @brief Record a query failure into an optional out-parameter. */
bool fail(QString *error, const QSqlQuery &query, const char *what)
{
    if (error)
        *error = QStringLiteral("%1: %2").arg(QLatin1String(what), query.lastError().text());
    return false;
}

} // namespace

Database::Database()
    : m_connectionName(QStringLiteral("imageworker_%1")
                           .arg(g_connectionSerial.fetch_add(1, std::memory_order_relaxed)))
{
}

Database::~Database()
{
    close();
}

bool Database::isOpen() const
{
    return m_db.isValid() && m_db.isOpen();
}

bool Database::open(const QString &path, QString *error)
{
    close();

    m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    m_db.setDatabaseName(path);
    if (!m_db.open()) {
        if (error)
            *error = QStringLiteral("cannot open %1: %2").arg(path, m_db.lastError().text());
        QSqlDatabase::removeDatabase(m_connectionName);
        m_db = QSqlDatabase();
        return false;
    }
    m_path = path;

    QSqlQuery pragma(m_db);
    // WAL keeps readers running while the indexer writes; NORMAL trades an
    // fsync per commit for durability we do not need in a rebuildable index.
    pragma.exec(QStringLiteral("PRAGMA journal_mode=WAL"));
    pragma.exec(QStringLiteral("PRAGMA synchronous=NORMAL"));
    pragma.exec(QStringLiteral("PRAGMA temp_store=MEMORY"));
    pragma.exec(QStringLiteral("PRAGMA cache_size=-65536")); // 64 MiB
    pragma.exec(QStringLiteral("PRAGMA foreign_keys=ON"));

    if (!applySchema(error) || !prepareStatements(error)) {
        close();
        return false;
    }
    return true;
}

void Database::close()
{
    m_upsertFile.reset();
    m_selectId.reset();
    m_upsertThumb.reset();
    m_deleteThumb.reset();

    if (m_db.isValid()) {
        if (m_db.isOpen())
            m_db.close();
        m_db = QSqlDatabase();
        QSqlDatabase::removeDatabase(m_connectionName);
    }
    m_path.clear();
}

bool Database::applySchema(QString *error)
{
    static const char *statements[] = {
        "CREATE TABLE IF NOT EXISTS files ("
        "  id           INTEGER PRIMARY KEY,"
        "  rel          TEXT    NOT NULL,"
        // Empty for the working tree, a branch name for anything read out of
        // git. Part of the key rather than a tag: the same path exists on every
        // branch, usually with different contents.
        "  ref          TEXT    NOT NULL DEFAULT '',"
        "  blob         TEXT    NOT NULL DEFAULT '',"
        "  size         INTEGER NOT NULL,"
        "  mtime        INTEGER NOT NULL,"
        "  width        INTEGER NOT NULL DEFAULT 0,"
        "  height       INTEGER NOT NULL DEFAULT 0,"
        "  content_hash BLOB,"
        "  chash64      INTEGER NOT NULL DEFAULT 0,"
        "  phash        INTEGER NOT NULL DEFAULT 0,"
        "  dhash        INTEGER NOT NULL DEFAULT 0,"
        "  b0           INTEGER NOT NULL DEFAULT 0,"
        "  b1           INTEGER NOT NULL DEFAULT 0,"
        "  b2           INTEGER NOT NULL DEFAULT 0,"
        "  b3           INTEGER NOT NULL DEFAULT 0,"
        "  status       INTEGER NOT NULL DEFAULT 0,"
        "  indexed_at   INTEGER NOT NULL DEFAULT 0)",

        "CREATE TABLE IF NOT EXISTS thumbs ("
        "  file_id INTEGER PRIMARY KEY REFERENCES files(id) ON DELETE CASCADE,"
        "  data    BLOB NOT NULL)",

        "CREATE TABLE IF NOT EXISTS features ("
        "  file_id  INTEGER PRIMARY KEY REFERENCES files(id) ON DELETE CASCADE,"
        "  model    TEXT    NOT NULL,"
        "  count    INTEGER NOT NULL,"
        "  dim      INTEGER NOT NULL,"
        "  img_w    INTEGER NOT NULL,"
        "  img_h    INTEGER NOT NULL,"
        "  desc_off INTEGER NOT NULL,"
        "  kpts_off INTEGER NOT NULL)",

        "CREATE INDEX IF NOT EXISTS idx_features_model ON features(model)",

        "CREATE TABLE IF NOT EXISTS meta ("
        "  key   TEXT PRIMARY KEY,"
        "  value TEXT NOT NULL)",

        "CREATE UNIQUE INDEX IF NOT EXISTS idx_files_rel_ref ON files(rel,ref)",
        "CREATE INDEX IF NOT EXISTS idx_files_ref     ON files(ref)",
        "CREATE INDEX IF NOT EXISTS idx_files_chash64 ON files(chash64)",
        "CREATE INDEX IF NOT EXISTS idx_files_size    ON files(size)",
        "CREATE INDEX IF NOT EXISTS idx_files_b0      ON files(b0)",
        "CREATE INDEX IF NOT EXISTS idx_files_b1      ON files(b1)",
        "CREATE INDEX IF NOT EXISTS idx_files_b2      ON files(b2)",
        "CREATE INDEX IF NOT EXISTS idx_files_b3      ON files(b3)",
    };

    if (!migrateToRefs(error))
        return false;

    QSqlQuery query(m_db);
    for (const char *sql : statements) {
        if (!query.exec(QLatin1String(sql)))
            return fail(error, query, "schema");
    }

    setMetaValue(QStringLiteral("schema_version"), QStringLiteral("2"));
    return true;
}

namespace {

/** @brief The current definition of the files table, used by migrations too. */
const char *kFilesTableSql =
    "CREATE TABLE %1 ("
    "  id           INTEGER PRIMARY KEY,"
    "  rel          TEXT    NOT NULL,"
    "  ref          TEXT    NOT NULL DEFAULT '',"
    "  blob         TEXT    NOT NULL DEFAULT '',"
    "  size         INTEGER NOT NULL,"
    "  mtime        INTEGER NOT NULL,"
    "  width        INTEGER NOT NULL DEFAULT 0,"
    "  height       INTEGER NOT NULL DEFAULT 0,"
    "  content_hash BLOB,"
    "  chash64      INTEGER NOT NULL DEFAULT 0,"
    "  phash        INTEGER NOT NULL DEFAULT 0,"
    "  dhash        INTEGER NOT NULL DEFAULT 0,"
    "  b0           INTEGER NOT NULL DEFAULT 0,"
    "  b1           INTEGER NOT NULL DEFAULT 0,"
    "  b2           INTEGER NOT NULL DEFAULT 0,"
    "  b3           INTEGER NOT NULL DEFAULT 0,"
    "  status       INTEGER NOT NULL DEFAULT 0,"
    "  indexed_at   INTEGER NOT NULL DEFAULT 0)";

} // namespace

bool Database::migrateToRefs(QString *error)
{
    QSqlQuery query(m_db);

    // Nothing to migrate in a database this build is creating from scratch.
    if (!query.exec(QStringLiteral("SELECT name FROM sqlite_master"
                                   " WHERE type='table' AND name='files'")))
        return fail(error, query, "inspect schema");
    const bool hasFiles = query.next();
    query.finish();

    bool hasRef = false;
    if (hasFiles) {
        if (!query.exec(QStringLiteral("PRAGMA table_info(files)")))
            return fail(error, query, "inspect files");
        while (query.next()) {
            if (query.value(1).toString() == QLatin1String("ref"))
                hasRef = true;
        }
        query.finish();
    }

    if (hasFiles && !hasRef) {
        // The old table declared `rel` UNIQUE as a column constraint, and SQLite
        // cannot drop a column constraint in place, so the table is rebuilt.
        //
        // The order below is SQLite's documented one, and it is not
        // interchangeable: build the new table, copy, drop the old, rename.
        // Renaming the *old* table out of the way first looks equivalent and is
        // not - since 3.25 a rename rewrites references to it in other tables,
        // so `thumbs` and `features` would silently start pointing at a table
        // that the next statement deletes. That mistake got as far as a
        // "no such table: main.files_v1" from an unrelated INSERT.
        //
        // Ids are carried across, so previews and descriptors, which reference
        // them, survive untouched.
        static const char *migration[] = {
            "INSERT INTO files_new"
            " (id,rel,ref,blob,size,mtime,width,height,content_hash,chash64,"
            "  phash,dhash,b0,b1,b2,b3,status,indexed_at)"
            " SELECT id,rel,'','',size,mtime,width,height,content_hash,chash64,"
            "        phash,dhash,b0,b1,b2,b3,status,indexed_at FROM files",
            "DROP TABLE files",
            "ALTER TABLE files_new RENAME TO files",
        };

        // Foreign keys must be off across a table rebuild, and the pragma is a
        // no-op inside a transaction, so it goes first.
        query.exec(QStringLiteral("PRAGMA foreign_keys=OFF"));

        if (!m_db.transaction()) {
            if (error)
                *error = m_db.lastError().text();
            return false;
        }
        bool ok = query.exec(QString::fromLatin1(kFilesTableSql)
                                 .arg(QStringLiteral("files_new")));
        for (const char *sql : migration) {
            if (!ok)
                break;
            ok = query.exec(QLatin1String(sql));
        }
        if (!ok) {
            const bool failed = fail(error, query, "migrate to refs");
            m_db.rollback();
            query.exec(QStringLiteral("PRAGMA foreign_keys=ON"));
            return failed;
        }
        if (!m_db.commit()) {
            if (error)
                *error = m_db.lastError().text();
            query.exec(QStringLiteral("PRAGMA foreign_keys=ON"));
            return false;
        }
        query.exec(QStringLiteral("PRAGMA foreign_keys=ON"));
    }

    return repairDanglingReferences(error);
}

bool Database::repairDanglingReferences(QString *error)
{
    // Databases migrated by the first version of the code above have `thumbs`
    // and `features` pointing at a table that no longer exists, which only
    // surfaces when something writes to them. Rebuilding the two tables keeps
    // every row and costs nothing on a healthy database, where the loop below
    // finds nothing to do.
    struct Broken { const char *name; const char *create; const char *columns; };
    static const Broken tables[] = {
        { "thumbs",
          "CREATE TABLE %1 ("
          "  file_id INTEGER PRIMARY KEY REFERENCES files(id) ON DELETE CASCADE,"
          "  data    BLOB NOT NULL)",
          "file_id,data" },
        { "features",
          "CREATE TABLE %1 ("
          "  file_id  INTEGER PRIMARY KEY REFERENCES files(id) ON DELETE CASCADE,"
          "  model    TEXT    NOT NULL,"
          "  count    INTEGER NOT NULL,"
          "  dim      INTEGER NOT NULL,"
          "  img_w    INTEGER NOT NULL,"
          "  img_h    INTEGER NOT NULL,"
          "  desc_off INTEGER NOT NULL,"
          "  kpts_off INTEGER NOT NULL)",
          "file_id,model,count,dim,img_w,img_h,desc_off,kpts_off" },
    };

    QSqlQuery query(m_db);
    for (const Broken &table : tables) {
        query.prepare(QStringLiteral("SELECT sql FROM sqlite_master"
                                     " WHERE type='table' AND name=:name"));
        query.bindValue(QStringLiteral(":name"), QLatin1String(table.name));
        if (!query.exec() || !query.next())
            continue;
        const QString sql = query.value(0).toString();
        query.finish();
        if (!sql.contains(QLatin1String("files_v1")))
            continue;

        const QString temp = QLatin1String(table.name) + QStringLiteral("_fixed");
        query.exec(QStringLiteral("PRAGMA foreign_keys=OFF"));
        if (!m_db.transaction()) {
            if (error)
                *error = m_db.lastError().text();
            return false;
        }

        bool ok = query.exec(QString::fromLatin1(table.create).arg(temp));
        if (ok) {
            ok = query.exec(QStringLiteral("INSERT INTO %1 (%2) SELECT %2 FROM %3")
                                .arg(temp, QLatin1String(table.columns),
                                     QLatin1String(table.name)));
        }
        if (ok)
            ok = query.exec(QStringLiteral("DROP TABLE %1").arg(QLatin1String(table.name)));
        if (ok) {
            ok = query.exec(QStringLiteral("ALTER TABLE %1 RENAME TO %2")
                                .arg(temp, QLatin1String(table.name)));
        }

        if (!ok) {
            const bool failed = fail(error, query, "repair references");
            m_db.rollback();
            query.exec(QStringLiteral("PRAGMA foreign_keys=ON"));
            return failed;
        }
        if (!m_db.commit()) {
            if (error)
                *error = m_db.lastError().text();
            query.exec(QStringLiteral("PRAGMA foreign_keys=ON"));
            return false;
        }
        query.exec(QStringLiteral("PRAGMA foreign_keys=ON"));
    }
    return true;
}

bool Database::prepareStatements(QString *error)
{
    m_upsertFile = std::make_unique<QSqlQuery>(m_db);
    if (!m_upsertFile->prepare(QStringLiteral(
            "INSERT INTO files"
            " (rel,ref,blob,size,mtime,width,height,content_hash,chash64,phash,dhash,b0,b1,b2,b3,status,indexed_at)"
            " VALUES"
            " (:rel,:ref,:blob,:size,:mtime,:width,:height,:chash,:chash64,:phash,:dhash,:b0,:b1,:b2,:b3,:status,:ts)"
            " ON CONFLICT(rel,ref) DO UPDATE SET"
            "  blob=excluded.blob, size=excluded.size, mtime=excluded.mtime,"
            "  width=excluded.width, height=excluded.height,"
            "  content_hash=excluded.content_hash, chash64=excluded.chash64,"
            "  phash=excluded.phash, dhash=excluded.dhash,"
            "  b0=excluded.b0, b1=excluded.b1, b2=excluded.b2, b3=excluded.b3,"
            "  status=excluded.status, indexed_at=excluded.indexed_at"))) {
        return fail(error, *m_upsertFile, "prepare upsert");
    }

    m_selectId = std::make_unique<QSqlQuery>(m_db);
    if (!m_selectId->prepare(QStringLiteral("SELECT id FROM files WHERE rel=:rel AND ref=:ref")))
        return fail(error, *m_selectId, "prepare select id");

    m_upsertThumb = std::make_unique<QSqlQuery>(m_db);
    if (!m_upsertThumb->prepare(QStringLiteral(
            "INSERT INTO thumbs (file_id,data) VALUES (:id,:data)"
            " ON CONFLICT(file_id) DO UPDATE SET data=excluded.data"))) {
        return fail(error, *m_upsertThumb, "prepare upsert thumb");
    }

    m_deleteThumb = std::make_unique<QSqlQuery>(m_db);
    if (!m_deleteThumb->prepare(QStringLiteral("DELETE FROM thumbs WHERE file_id=:id")))
        return fail(error, *m_deleteThumb, "prepare delete thumb");

    return true;
}

bool Database::beginTransaction(QString *error)
{
    if (m_db.transaction())
        return true;
    if (error)
        *error = m_db.lastError().text();
    return false;
}

bool Database::commit(QString *error)
{
    if (m_db.commit())
        return true;
    if (error)
        *error = m_db.lastError().text();
    return false;
}

bool Database::upsert(const IndexRecord &record, QString *error)
{
    if (!m_upsertFile)
        return false;

    int bands[4] = { 0, 0, 0, 0 };
    bandsOf(record.phash, bands);

    // A default-constructed QString is null, and Qt binds null as SQL NULL,
    // which a NOT NULL column rejects. The working tree is spelled as the empty
    // string, not as the absence of a value.
    const QString ref  = record.ref.isNull() ? QString::fromLatin1("") : record.ref;
    const QString blob = record.blob.isNull() ? QString::fromLatin1("") : record.blob;

    m_upsertFile->bindValue(QStringLiteral(":rel"), record.rel);
    m_upsertFile->bindValue(QStringLiteral(":ref"), ref);
    m_upsertFile->bindValue(QStringLiteral(":blob"), blob);
    m_upsertFile->bindValue(QStringLiteral(":size"), record.size);
    m_upsertFile->bindValue(QStringLiteral(":mtime"), record.mtime);
    m_upsertFile->bindValue(QStringLiteral(":width"), record.width);
    m_upsertFile->bindValue(QStringLiteral(":height"), record.height);
    m_upsertFile->bindValue(QStringLiteral(":chash"),
                            record.contentHash.isEmpty() ? QVariant() : QVariant(record.contentHash));
    m_upsertFile->bindValue(QStringLiteral(":chash64"), shortHash(record.contentHash));
    m_upsertFile->bindValue(QStringLiteral(":phash"), static_cast<qint64>(record.phash));
    m_upsertFile->bindValue(QStringLiteral(":dhash"), static_cast<qint64>(record.dhash));
    m_upsertFile->bindValue(QStringLiteral(":b0"), bands[0]);
    m_upsertFile->bindValue(QStringLiteral(":b1"), bands[1]);
    m_upsertFile->bindValue(QStringLiteral(":b2"), bands[2]);
    m_upsertFile->bindValue(QStringLiteral(":b3"), bands[3]);
    m_upsertFile->bindValue(QStringLiteral(":status"), static_cast<int>(record.status));
    m_upsertFile->bindValue(QStringLiteral(":ts"), QDateTime::currentSecsSinceEpoch());

    if (!m_upsertFile->exec())
        return fail(error, *m_upsertFile, "upsert");

    // The upsert may have updated rather than inserted, so lastInsertId() is
    // not usable; look the id up explicitly.
    m_selectId->bindValue(QStringLiteral(":rel"), record.rel);
    m_selectId->bindValue(QStringLiteral(":ref"), ref);
    if (!m_selectId->exec() || !m_selectId->next())
        return fail(error, *m_selectId, "select id");
    const qint64 id = m_selectId->value(0).toLongLong();
    m_selectId->finish();

    if (!record.thumb.isEmpty()) {
        m_upsertThumb->bindValue(QStringLiteral(":id"), id);
        m_upsertThumb->bindValue(QStringLiteral(":data"), record.thumb);
        if (!m_upsertThumb->exec())
            return fail(error, *m_upsertThumb, "upsert thumb");
    } else if (record.status != FileStatus::Ok) {
        m_deleteThumb->bindValue(QStringLiteral(":id"), id);
        m_deleteThumb->exec();
    }

    return true;
}

QHash<QString, QPair<qint64, qint64>> Database::loadSignatures()
{
    QHash<QString, QPair<qint64, qint64>> out;
    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    if (!query.exec(QStringLiteral("SELECT rel,size,mtime FROM files WHERE ref=''")))
        return out;

    while (query.next()) {
        out.insert(query.value(0).toString(),
                   qMakePair(query.value(1).toLongLong(), query.value(2).toLongLong()));
    }
    return out;
}

int Database::pruneMissing(const QSet<QString> &present, QString *error)
{
    QList<qint64> doomed;
    {
        QSqlQuery query(m_db);
        query.setForwardOnly(true);
        if (!query.exec(QStringLiteral("SELECT id,rel FROM files WHERE ref=''"))) {
            fail(error, query, "prune scan");
            return -1;
        }
        while (query.next()) {
            if (!present.contains(query.value(1).toString()))
                doomed.append(query.value(0).toLongLong());
        }
    }
    if (doomed.isEmpty())
        return 0;

    if (!beginTransaction(error))
        return -1;

    QSqlQuery del(m_db);
    if (!del.prepare(QStringLiteral("DELETE FROM files WHERE id=:id"))) {
        fail(error, del, "prune prepare");
        return -1;
    }
    for (qint64 id : doomed) {
        del.bindValue(QStringLiteral(":id"), id);
        if (!del.exec()) {
            fail(error, del, "prune delete");
            return -1;
        }
    }
    if (!commit(error))
        return -1;

    return static_cast<int>(doomed.size());
}

QList<CompactRow> Database::loadCompactRows()
{
    QList<CompactRow> out;

    QSqlQuery count(m_db);
    if (count.exec(QStringLiteral("SELECT COUNT(*) FROM files WHERE status=0")) && count.next())
        out.reserve(count.value(0).toInt());

    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    if (!query.exec(QStringLiteral(
            "SELECT id,size,chash64,phash,dhash FROM files WHERE status=0")))
        return out;

    while (query.next()) {
        CompactRow row;
        row.id      = query.value(0).toLongLong();
        row.size    = query.value(1).toLongLong();
        row.chash64 = query.value(2).toLongLong();
        row.phash   = toHash(query.value(3));
        row.dhash   = toHash(query.value(4));
        out.append(row);
    }
    return out;
}

QList<FileInfoRow> Database::filesByIds(const QList<qint64> &ids)
{
    QList<FileInfoRow> out;
    if (ids.isEmpty())
        return out;
    out.reserve(ids.size());

    // Chunked so the statement never approaches SQLite's variable limit.
    constexpr int kChunk = 500;
    for (int start = 0; start < ids.size(); start += kChunk) {
        const int end = std::min<int>(start + kChunk, static_cast<int>(ids.size()));

        QStringList placeholders;
        placeholders.reserve(end - start);
        for (int i = start; i < end; ++i)
            placeholders << QString::number(ids.at(i));

        QSqlQuery query(m_db);
        query.setForwardOnly(true);
        if (!query.exec(QStringLiteral("SELECT id,rel,size,mtime,width,height,phash,ref,blob"
                                       " FROM files WHERE id IN (%1)")
                            .arg(placeholders.join(QLatin1Char(','))))) {
            continue;
        }
        while (query.next()) {
            FileInfoRow row;
            row.id     = query.value(0).toLongLong();
            row.rel    = query.value(1).toString();
            row.size   = query.value(2).toLongLong();
            row.mtime  = query.value(3).toLongLong();
            row.width  = query.value(4).toInt();
            row.height = query.value(5).toInt();
            row.phash  = toHash(query.value(6));
            row.ref    = query.value(7).toString();
            row.blob   = query.value(8).toString();
            out.append(row);
        }
    }
    return out;
}

QByteArray Database::thumbnail(qint64 id)
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("SELECT data FROM thumbs WHERE file_id=:id"));
    query.bindValue(QStringLiteral(":id"), id);
    if (query.exec() && query.next())
        return query.value(0).toByteArray();
    return {};
}

QByteArray Database::contentHash(qint64 id)
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("SELECT content_hash FROM files WHERE id=:id"));
    query.bindValue(QStringLiteral(":id"), id);
    if (query.exec() && query.next())
        return query.value(0).toByteArray();
    return {};
}

bool Database::upsertFeatures(const FeatureRecord &record, QString *error)
{
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO features (file_id,model,count,dim,img_w,img_h,desc_off,kpts_off)"
            " VALUES (:id,:model,:count,:dim,:w,:h,:doff,:koff)"
            " ON CONFLICT(file_id) DO UPDATE SET"
            "  model=excluded.model, count=excluded.count, dim=excluded.dim,"
            "  img_w=excluded.img_w, img_h=excluded.img_h,"
            "  desc_off=excluded.desc_off, kpts_off=excluded.kpts_off"))) {
        return fail(error, query, "prepare upsert features");
    }

    query.bindValue(QStringLiteral(":id"), record.fileId);
    query.bindValue(QStringLiteral(":model"), record.model);
    query.bindValue(QStringLiteral(":count"), record.count);
    query.bindValue(QStringLiteral(":dim"), record.dim);
    query.bindValue(QStringLiteral(":w"), record.imageWidth);
    query.bindValue(QStringLiteral(":h"), record.imageHeight);
    query.bindValue(QStringLiteral(":doff"), record.descOffset);
    query.bindValue(QStringLiteral(":koff"), record.kptsOffset);

    if (!query.exec())
        return fail(error, query, "upsert features");
    return true;
}

bool Database::featuresFor(qint64 fileId, const QString &model, FeatureRecord *out)
{
    if (!out)
        return false;

    QSqlQuery query(m_db);
    query.prepare(QStringLiteral(
        "SELECT model,count,dim,img_w,img_h,desc_off,kpts_off"
        " FROM features WHERE file_id=:id AND model=:model"));
    query.bindValue(QStringLiteral(":id"), fileId);
    query.bindValue(QStringLiteral(":model"), model);
    if (!query.exec() || !query.next())
        return false;

    out->fileId      = fileId;
    out->model       = query.value(0).toString();
    out->count       = query.value(1).toInt();
    out->dim         = query.value(2).toInt();
    out->imageWidth  = query.value(3).toInt();
    out->imageHeight = query.value(4).toInt();
    out->descOffset  = query.value(5).toLongLong();
    out->kptsOffset  = query.value(6).toLongLong();
    return true;
}

QList<PendingFeature> Database::filesWithoutFeatures(const QString &model)
{
    QList<PendingFeature> out;

    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    query.prepare(QStringLiteral(
        "SELECT f.id, f.rel, f.ref, f.blob FROM files f"
        " LEFT JOIN features x ON x.file_id = f.id AND x.model = :model"
        " WHERE f.status = 0 AND x.file_id IS NULL"
        " ORDER BY f.ref, f.id"));
    query.bindValue(QStringLiteral(":model"), model);
    if (!query.exec())
        return out;

    while (query.next()) {
        PendingFeature p;
        p.fileId = query.value(0).toLongLong();
        p.rel    = query.value(1).toString();
        p.ref    = query.value(2).toString();
        p.blob   = query.value(3).toString();
        out.append(p);
    }
    return out;
}

QHash<QString, QString> Database::loadBlobs(const QString &ref)
{
    QHash<QString, QString> out;
    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    query.prepare(QStringLiteral("SELECT rel,blob FROM files WHERE ref=:ref"));
    query.bindValue(QStringLiteral(":ref"), ref);
    if (!query.exec())
        return out;

    while (query.next())
        out.insert(query.value(0).toString(), query.value(1).toString());
    return out;
}

QStringList Database::refs()
{
    QStringList out;
    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    if (!query.exec(QStringLiteral("SELECT DISTINCT ref FROM files ORDER BY ref")))
        return out;
    while (query.next())
        out << query.value(0).toString();
    return out;
}

int Database::removeRef(const QString &ref, QString *error)
{
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("DELETE FROM files WHERE ref=:ref"))) {
        fail(error, query, "remove ref prepare");
        return -1;
    }
    query.bindValue(QStringLiteral(":ref"), ref);
    if (!query.exec()) {
        fail(error, query, "remove ref");
        return -1;
    }
    return query.numRowsAffected();
}

int Database::pruneMissingInRef(const QString &ref, const QSet<QString> &present, QString *error)
{
    QList<qint64> doomed;
    {
        QSqlQuery query(m_db);
        query.setForwardOnly(true);
        query.prepare(QStringLiteral("SELECT id,rel FROM files WHERE ref=:ref"));
        query.bindValue(QStringLiteral(":ref"), ref);
        if (!query.exec()) {
            fail(error, query, "prune ref scan");
            return -1;
        }
        while (query.next()) {
            if (!present.contains(query.value(1).toString()))
                doomed.append(query.value(0).toLongLong());
        }
    }
    if (doomed.isEmpty())
        return 0;

    if (!beginTransaction(error))
        return -1;

    QSqlQuery del(m_db);
    if (!del.prepare(QStringLiteral("DELETE FROM files WHERE id=:id"))) {
        fail(error, del, "prune ref prepare");
        return -1;
    }
    for (qint64 id : doomed) {
        del.bindValue(QStringLiteral(":id"), id);
        if (!del.exec()) {
            fail(error, del, "prune ref delete");
            return -1;
        }
    }
    if (!commit(error))
        return -1;

    return static_cast<int>(doomed.size());
}

QList<FeatureRecord> Database::allFeatures(const QString &model)
{
    QList<FeatureRecord> out;

    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    query.prepare(QStringLiteral(
        "SELECT file_id,count,dim,img_w,img_h,desc_off,kpts_off"
        " FROM features WHERE model=:model AND count > 0 ORDER BY file_id"));
    query.bindValue(QStringLiteral(":model"), model);
    if (!query.exec())
        return out;

    while (query.next()) {
        FeatureRecord r;
        r.model       = model;
        r.fileId      = query.value(0).toLongLong();
        r.count       = query.value(1).toInt();
        r.dim         = query.value(2).toInt();
        r.imageWidth  = query.value(3).toInt();
        r.imageHeight = query.value(4).toInt();
        r.descOffset  = query.value(5).toLongLong();
        r.kptsOffset  = query.value(6).toLongLong();
        out.append(r);
    }
    return out;
}

int Database::clearFeatures()
{
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("DELETE FROM features")))
        return -1;
    return query.numRowsAffected();
}

QList<FileInfoRow> Database::filesWithFewFeatures(const QString &model, int maxCount)
{
    QList<FileInfoRow> out;

    QSqlQuery query(m_db);
    query.setForwardOnly(true);
    query.prepare(QStringLiteral(
        "SELECT f.id, f.rel, f.size, f.mtime, f.width, f.height, f.phash,"
        "       f.ref, f.blob, COALESCE(x.count, 0) AS kp"
        " FROM files f"
        " LEFT JOIN features x ON x.file_id = f.id AND x.model = :model"
        " WHERE f.status = 0 AND COALESCE(x.count, 0) <= :maxCount"
        " ORDER BY kp ASC, f.rel ASC"));
    query.bindValue(QStringLiteral(":model"), model);
    query.bindValue(QStringLiteral(":maxCount"), maxCount);
    if (!query.exec())
        return out;

    while (query.next()) {
        FileInfoRow row;
        row.id     = query.value(0).toLongLong();
        row.rel    = query.value(1).toString();
        row.size   = query.value(2).toLongLong();
        row.mtime  = query.value(3).toLongLong();
        row.width  = query.value(4).toInt();
        row.height = query.value(5).toInt();
        row.phash  = toHash(query.value(6));
        row.ref    = query.value(7).toString();
        row.blob   = query.value(8).toString();
        out.append(row);
    }
    return out;
}

DatabaseSummary Database::summary()
{
    DatabaseSummary s;
    QSqlQuery query(m_db);

    if (query.exec(QStringLiteral(
            "SELECT COUNT(*), SUM(size), SUM(status=0), SUM(status<>0) FROM files"))
        && query.next()) {
        s.totalBytes = query.value(1).toLongLong();
        s.files      = query.value(2).toInt();
        s.failed     = query.value(3).toInt();
    }
    if (query.exec(QStringLiteral("SELECT COUNT(*) FROM thumbs")) && query.next())
        s.thumbs = query.value(0).toInt();

    return s;
}

QString Database::metaValue(const QString &key)
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("SELECT value FROM meta WHERE key=:k"));
    query.bindValue(QStringLiteral(":k"), key);
    if (query.exec() && query.next())
        return query.value(0).toString();
    return {};
}

bool Database::setMetaValue(const QString &key, const QString &value)
{
    QSqlQuery query(m_db);
    query.prepare(QStringLiteral("INSERT INTO meta (key,value) VALUES (:k,:v)"
                                 " ON CONFLICT(key) DO UPDATE SET value=excluded.value"));
    query.bindValue(QStringLiteral(":k"), key);
    query.bindValue(QStringLiteral(":v"), value);
    return query.exec();
}

} // namespace iw
