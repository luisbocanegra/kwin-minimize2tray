#include "trayicon.h"
#include <KService>
#include <QAction>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDebug>
#include <QPainter>
#include <QVariantMap>

TrayIcon::TrayIcon(QObject *parent) : QObject(parent) {
    connect(this, &TrayIcon::xdgNameChanged, this, [this]() { initializeTrayIcon(); });
}

TrayIcon::~TrayIcon() = default;

void TrayIcon::setIcon(const QIcon newIcon) {
    if (m_icon.cacheKey() != newIcon.cacheKey()) {
        m_icon = newIcon;
        emit iconChanged();
    }
}

void TrayIcon::setWindowId(const QUuid window) { m_windowId = window; }
void TrayIcon::setToolTipText(const QString toolTipText) {
    if (m_toolTipText != toolTipText) {
        m_toolTipText = toolTipText;
        emit toolTipTextChanged();
    }
}

void TrayIcon::launcherAPIUpdate(const QString &uri, const QMap<QString, QVariant> &properties) {

    QString fixed_uri = uri;
    if (!fixed_uri.startsWith(QLatin1String("application://"))) {
        fixed_uri.prepend(QLatin1String("application://"));
    }
    if (!fixed_uri.endsWith(QLatin1String(".desktop"))) {
        fixed_uri.append(QLatin1String(".desktop"));
    }

    // ignore signals from other applications
    if (m_launcherUrl != fixed_uri) {
        return;
    }

    auto propertiesEnd = properties.constEnd();
    auto foundCount = properties.constFind(QStringLiteral("count"));
    if (foundCount != propertiesEnd) {
        qint64 newCount = foundCount->toLongLong();
        // 2 billion unread emails ought to be enough for anybody
        if (newCount < std::numeric_limits<int>::max()) {
            int saneCount = static_cast<int>(newCount);
            if (saneCount != m_count) {
                m_count = saneCount;
                emit countChanged(saneCount);
            }
        }
    }

    auto foundCountVisible = properties.constFind(QStringLiteral("count-visible"));
    if (foundCountVisible != propertiesEnd) {
        bool newCountVisible = foundCountVisible->toBool();
        if (newCountVisible != m_countVisible) {
            m_countVisible = newCountVisible;
            emit countVisibleChanged(newCountVisible);
        }
    }

    // the API gives us progress as 0..1 double but we'll use percent to avoid unnecessary
    // changes when it just changed a fraction of a percent, hence not using our fancy updateLauncherProperty method
    auto foundProgress = properties.constFind(QStringLiteral("progress"));
    if (foundProgress != propertiesEnd) {
        int newProgress = qRound(foundProgress->toDouble() * 100);
        if (newProgress != m_progress) {
            m_progress = newProgress;
            emit progressChanged(newProgress);
        }
    }

    auto foundProgressVisible = properties.constFind(QStringLiteral("progress-visible"));
    if (foundProgressVisible != propertiesEnd) {
        bool newProgressVisible = foundProgressVisible->toBool();
        if (newProgressVisible != m_progressVisible) {
            m_progressVisible = newProgressVisible;
            emit progressVisibleChanged(newProgressVisible);
        }
    }

    auto foundUrgent = properties.constFind(QStringLiteral("urgent"));
    if (foundUrgent != propertiesEnd) {
        bool newUrgent = foundUrgent->toBool();
        if (newUrgent != m_urgent) {
            m_urgent = newUrgent;
            emit urgentChanged(newUrgent);
        }
    }

    qDebug() << fixed_uri << properties;
    qDebug() << "count: " << m_countVisible << " " << m_count;
    qDebug() << "progress: " << m_progressVisible << " " << m_progress;
    qDebug() << "urgent: " << m_urgent;
}

QString TrayIcon::launcherUrl() const { return m_launcherUrl; }

void TrayIcon::setLauncherUrl(const QString &launcherUrl) {
    if (launcherUrl != m_launcherUrl) {
        m_launcherUrl = launcherUrl;
        Q_EMIT launcherUrlChanged(launcherUrl);
    }
}

int TrayIcon::count() const { return m_count; }

void TrayIcon::setCount(int count) {
    if (m_count != count) {
        m_count = count;
        Q_EMIT countChanged(count);
    }
}

bool TrayIcon::countVisible() const { return m_countVisible; }

void TrayIcon::setCountVisible(bool countVisible) {
    if (m_countVisible != countVisible) {
        m_countVisible = countVisible;
        Q_EMIT countVisibleChanged(countVisible);
    }
}

int TrayIcon::progress() const { return m_progress; }

void TrayIcon::setProgress(int progress) {
    int boundedProgress = std::clamp(progress, 0, 100);

    if (progress != boundedProgress) {
        qDebug() << qUtf8Printable(m_launcherUrl) << ": Progress value " << progress << " is out of bounds!";
    }

    if (m_progress != boundedProgress) {
        m_progress = boundedProgress;
        Q_EMIT progressChanged(boundedProgress);
    }
}

bool TrayIcon::progressVisible() const { return m_progressVisible; }

void TrayIcon::setProgressVisible(bool progressVisible) {
    if (m_progressVisible != progressVisible) {
        m_progressVisible = progressVisible;
        Q_EMIT progressVisibleChanged(progressVisible);
    }
}

bool TrayIcon::urgent() const { return m_urgent; }

void TrayIcon::setUrgent(bool urgent) {
    if (m_urgent != urgent) {
        m_urgent = urgent;
        Q_EMIT urgentChanged(urgent);
    }
}

void TrayIcon::setXdgName(const QString id) {
    if (id != m_xdgName) {
        m_xdgName = id;
        emit xdgNameChanged();
    }
}

void TrayIcon::initializeTrayIcon() {
    if (trayIcon) {
        return;
    }

    qDebug() << "New Tray Icon" << m_xdgName;
    trayIcon = new KStatusNotifierItem(m_xdgName, this);
    QMenu *m_menu = new QMenu();

    QAction *showAction = new QAction("Show/Hide", m_menu);
    connect(showAction, &QAction::triggered, this, [this]() { emit requestShowHide(m_windowId); });
    showAction->setIcon(QIcon::fromTheme(QStringLiteral("view-visible-symbolic")));
    m_menu->addAction(showAction);

    QAction *unpinAction = new QAction("Unpin", m_menu);
    connect(unpinAction, &QAction::triggered, this, [this]() { emit requestUnpin(m_windowId); });
    unpinAction->setIcon(QIcon::fromTheme(QStringLiteral("window-unpin-symbolic")));
    m_menu->addAction(unpinAction);

    connect(trayIcon, &KStatusNotifierItem::quitRequested, this, [this]() {
        emit requestClose(m_windowId);
        // wants to kill KWin otherwise
        trayIcon->abortQuit();
    });

    connect(trayIcon, &KStatusNotifierItem::activateRequested, this, [this]() { emit requestShowHide(m_windowId); });

    connect(this, &TrayIcon::toolTipTextChanged, this, [this]() { trayIcon->setToolTipTitle(m_toolTipText); });

    connect(this, &TrayIcon::xdgNameChanged, this, [this]() { setAppName(m_xdgName); });

    connect(this, &TrayIcon::appNameChanged, this, [this]() { trayIcon->setTitle(m_appName); });

    connect(this, &TrayIcon::iconChanged, this, [this]() { trayIcon->setIconByPixmap(m_icon); });

    QDBusConnection::sessionBus().connect(QString(), QString(), "com.canonical.Unity.LauncherEntry", "Update", this,
                                          SLOT(launcherAPIUpdate(QString, QMap<QString, QVariant>)));

    connect(this, &TrayIcon::urgentChanged, this, [this]() {
        if (m_urgent) {
            trayIcon->setStatus(KStatusNotifierItem::NeedsAttention);
        } else {
            trayIcon->setStatus(KStatusNotifierItem::Active);
        }
    });

    connect(this, &TrayIcon::countVisibleChanged, this, [this]() { updateBadges(); });
    connect(this, &TrayIcon::progressVisibleChanged, this, [this]() { updateBadges(); });
    connect(this, &TrayIcon::countChanged, this, [this]() { updateBadges(); });
    connect(this, &TrayIcon::progressChanged, this, [this]() { updateBadges(); });

    setAppName(m_xdgName);
    trayIcon->setContextMenu(m_menu);
    trayIcon->setToolTipTitle(m_toolTipText);
    trayIcon->setToolTipSubTitle(m_xdgName);
    trayIcon->setIconByPixmap(m_icon);
    trayIcon->setTitle(m_appName);
    trayIcon->setStatus(KStatusNotifierItem::Active);
}

void TrayIcon::setAppName(const QString &xdgName) {
    QString newName;
    KService::Ptr service = KService::serviceByDesktopName(xdgName);
    if (service) {
        newName = service->name();
    } else {
        newName = xdgName;
    }
    qDebug() << xdgName << "XDG name:" << newName;
    if (m_appName != newName) {
        m_appName = newName;
        emit appNameChanged();
    }
}

void TrayIcon::updateBadges() {
    if (m_countVisible || m_progressVisible) {

        const QSize iconSize = trayIcon->iconPixmap().actualSize(QSize(64, 64));
        QPixmap basePixmap = m_icon.pixmap(iconSize);
        QPainter painter(&basePixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        if (m_countVisible && m_count > 0) {
            QFont font;
            font.setPixelSize(int(basePixmap.width() * 0.5));
            font.setBold(true);
            painter.setFont(font);
            const int alignFlags = Qt::AlignRight | Qt::AlignTop | Qt::TextDontClip;
            QString text = QString::number(m_count);
            QRect textRect = painter.boundingRect(basePixmap.rect(), alignFlags, text);
            // make text a little more readable
            painter.setBrush(QColor(0, 0, 0, 100));
            painter.setPen(Qt::NoPen);
            painter.drawRect(textRect.adjusted(-1, 0, 1, 0));

            painter.setPen(Qt::white);
            painter.drawText(basePixmap.rect(), alignFlags, text);
        }

        if (m_progressVisible && m_progress > 0) {
            QRect progressBarRect(0, basePixmap.height() - 5, basePixmap.width() * m_progress / 100, 5);
            painter.setBrush(QColor(0, 255, 0, 150));
            painter.setPen(Qt::NoPen);
            painter.drawRect(progressBarRect);
        }

        trayIcon->setIconByPixmap(basePixmap);
    } else {
        trayIcon->setIconByPixmap(m_icon);
    }
}
