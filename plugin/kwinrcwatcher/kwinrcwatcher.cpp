#include "kwinrcwatcher.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QFile>
#include <QStandardPaths>

KwinrcWatcher::KwinrcWatcher(QObject *parent)
    : QObject(parent)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/kwinrc";

    m_fsw = new QFileSystemWatcher(this);

    if (QFile::exists(path))
        m_fsw->addPath(path);

    m_debounce.setInterval(200);
    m_debounce.setSingleShot(true);

    connect(&m_debounce, &QTimer::timeout, this, &KwinrcWatcher::reconfigure);

    connect(m_fsw, &QFileSystemWatcher::fileChanged, this, [this, path]() {
        if (!m_fsw->files().contains(path))
            m_fsw->addPath(path);

        m_debounce.start();
    });
}

void KwinrcWatcher::reconfigure()
{
    QDBusMessage msg = QDBusMessage::createMethodCall("org.kde.KWin", "/KWin", "org.kde.KWin", "reconfigure");
    QDBusPendingReply<QStringList> reply = QDBusConnection::sessionBus().asyncCall(msg);
    auto *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, reply, watcher]() {
        watcher->deleteLater();
        QTimer::singleShot(500, this, [this]() {
            emit configChanged();
        });
    });
}

KwinrcWatcher::~KwinrcWatcher() = default;
