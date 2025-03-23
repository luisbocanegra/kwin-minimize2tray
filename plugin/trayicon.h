#ifndef TRAYICON_H
#define TRAYICON_H

#pragma once

#include <KStatusNotifierItem>
#include <QDBusMessage>
#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QUuid>

class KStatusNotifierItem;

class TrayIcon : public QObject {
    Q_OBJECT
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged);
    Q_PROPERTY(QUuid windowId READ windowId WRITE setWindowId NOTIFY windowIdChanged);
    Q_PROPERTY(QString toolTipText READ toolTipText WRITE setToolTipText NOTIFY toolTipTextChanged)
    Q_PROPERTY(QString launcherUrl READ launcherUrl WRITE setLauncherUrl NOTIFY launcherUrlChanged)
    Q_PROPERTY(QString xdgName READ xdgName WRITE setXdgName NOTIFY xdgNameChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool countVisible READ countVisible NOTIFY countVisibleChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool progressVisible READ progressVisible NOTIFY progressVisibleChanged)
    Q_PROPERTY(bool urgent READ urgent NOTIFY urgentChanged)

  public:
    explicit TrayIcon(QObject *parent = nullptr);

    QString launcherUrl() const;
    void setLauncherUrl(const QString &launcherUrl);
    int count() const;
    bool countVisible() const;
    int progress() const;
    bool progressVisible() const;
    bool urgent() const;

    ~TrayIcon() override;

  signals:
    void iconChanged();
    void windowIdChanged();
    void toolTipTextChanged();
    void requestRemoveTrayIcon();
    void requestShowHide(const QUuid windowId);
    void requestClose(const QUuid windowId);
    void requestUnpin(const QUuid windowId);
    void xdgNameChanged();

    void launcherUrlChanged(const QString &launcherUrl);
    void countChanged(int count);
    void countVisibleChanged(bool countVisible);
    void progressChanged(int progress);
    void progressVisibleChanged(bool progressVisible);
    void urgentChanged(bool urgent);
    void appNameChanged();

  public slots:
    void launcherAPIUpdate(const QString &uri, const QMap<QString, QVariant> &properties);

  private:
    KStatusNotifierItem *trayIcon = nullptr;
    QMenu *m_menu;
    QIcon m_icon;
    QIcon icon() const { return m_icon; }
    void setIcon(QIcon icon);
    QUuid m_windowId;
    QUuid windowId() const { return m_windowId; }
    void setWindowId(QUuid windowId);
    QString m_toolTipText;
    QString toolTipText() const { return m_toolTipText; }
    void setToolTipText(QString toolTipText);
    QString m_xdgName;
    QString xdgName() const { return m_xdgName; }
    void setXdgName(QString id);
    void initializeTrayIcon();
    QString m_appName;
    QString appName() const { return m_appName; }
    void setAppName(const QString &xdgName);

    void setCount(int count);
    void setCountVisible(bool countVisible);
    void setProgress(int progress);
    void setProgressVisible(bool progressVisible);
    void setUrgent(bool urgent);

    QString m_launcherUrl;
    int m_count = 0;
    bool m_countVisible = false;
    int m_progress = 0;
    bool m_progressVisible = false;
    bool m_urgent = false;
};

#endif
