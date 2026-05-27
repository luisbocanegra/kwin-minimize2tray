#ifndef KWINRCWATCHER_H
#define KWINRCWATCHER_H

#pragma once
#include <QFileSystemWatcher>
#include <QTimer>

class KwinrcWatcher : public QObject
{
    Q_OBJECT

public:
    explicit KwinrcWatcher(QObject *parent = nullptr);

    ~KwinrcWatcher() override;

signals:
    void configChanged();

private:
    QFileSystemWatcher *m_fsw;
    QTimer m_debounce;

private slots:
    void reconfigure();
};

#endif
