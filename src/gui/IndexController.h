/**
 * @file IndexController.h
 * @brief Runs the blocking indexer on a worker thread for the GUI.
 */
#pragma once

#include <QObject>
#include <QThread>

#include "core/Indexer.h"
#include "core/Types.h"

/**
 * @brief Owns a worker thread and the iw::Indexer living on it.
 *
 * The GUI never calls into the core synchronously: it calls @ref start, then
 * reacts to @ref progress and @ref finished, which arrive as queued signals on
 * the UI thread.
 */
class IndexController : public QObject
{
    Q_OBJECT

public:
    explicit IndexController(QObject *parent = nullptr);
    ~IndexController() override;

    /** @return @c true while an indexing run is in flight. */
    bool isRunning() const { return m_running; }

public slots:
    /**
     * @brief Begin an indexing run.
     * @param options Fully populated options; ignored if a run is in flight.
     */
    void start(const iw::IndexOptions &options);

    /** @brief Ask the running indexer to stop at the next safe point. */
    void cancel();

signals:
    /** @brief Forwarded from iw::Indexer::progress. */
    void progress(int done, int total, const QString &stage);

    /** @brief Forwarded from iw::Indexer::message. */
    void message(const QString &text);

    /**
     * @brief Emitted once a run has ended, successfully or not.
     * @param stats Counters for the run.
     * @param error Empty on success.
     */
    void finished(const iw::IndexStats &stats, const QString &error);

private:
    QThread       m_thread;
    iw::Indexer  *m_indexer = nullptr;
    bool          m_running = false;
};
