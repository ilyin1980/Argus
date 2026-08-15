#include "IndexController.h"

#include <QMetaObject>

IndexController::IndexController(QObject *parent)
    : QObject(parent)
    , m_indexer(new iw::Indexer)
{
    // No metatype registration is needed: work crosses threads inside lambdas
    // that capture by value, never through queued signal arguments.
    m_indexer->moveToThread(&m_thread);
    connect(&m_thread, &QThread::finished, m_indexer, &QObject::deleteLater);
    connect(m_indexer, &iw::Indexer::progress, this, &IndexController::progress);
    connect(m_indexer, &iw::Indexer::message, this, &IndexController::message);
    m_thread.start();
}

IndexController::~IndexController()
{
    cancel();
    m_thread.quit();
    m_thread.wait();
}

void IndexController::start(const iw::IndexOptions &options)
{
    if (m_running)
        return;
    m_running = true;
    m_indexer->reset();

    // Queued into the worker thread; the lambda runs there, the signal it emits
    // comes back to the UI thread through the default connection type.
    QMetaObject::invokeMethod(m_indexer, [this, options] {
        QString error;
        const iw::IndexStats stats = m_indexer->run(options, &error);
        QMetaObject::invokeMethod(this, [this, stats, error] {
            m_running = false;
            emit finished(stats, error);
        }, Qt::QueuedConnection);
    }, Qt::QueuedConnection);
}

void IndexController::cancel()
{
    if (m_indexer)
        m_indexer->cancel();
}
