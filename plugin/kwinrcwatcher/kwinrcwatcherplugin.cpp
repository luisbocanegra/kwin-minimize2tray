#include "kwinrcwatcher.h"

#include <QQmlEngine>
#include <QQmlExtensionPlugin>

class KwinrcWatcherPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override
    {
        qmlRegisterType<KwinrcWatcher>(uri, 1, 0, "KwinrcWatcher");
    }
};

#include "kwinrcwatcherplugin.moc"
