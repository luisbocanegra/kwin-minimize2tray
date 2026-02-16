import QtQuick
import org.kde.kwin
import com.github.luisbocanegra.trayicon 1.0
import "../code/utils.js" as Utils

Item {
    id: root
    property bool debugEnabled: KWin.readConfig("debugEnabled", false)
    property var hideByDefaultClass: Utils.commaSeparate(KWin.readConfig("hideByDefaultClass", ""))
    property bool countUseDot: KWin.readConfig("countUseDot", false)
    property bool hideOnMinimize: KWin.readConfig("hideOnMinimize", false)
    property var trayIcons: new Object()
    readonly property Component trayIconComponent: TrayIcon {}

    ShortcutHandler {
        name: "Minimize to tray"
        text: "Minimize window to tray (sets skip Taskbar, Switcher & Pager)"
        sequence: "Meta+Alt+PgDown"
        onActivated: {
            let window = Workspace.activeWindow;
            while (window.transient) {
                window = window.transientFor;
            }
            if (!Utils.isValidWindow(window))
                return;
            Utils.toggleShowHide(window.internalId);
            Utils.addTrayIcon(window);
        }
    }

    Component.onCompleted: {
        Workspace.windowAdded.connect(window => Utils.setup(window));
        Workspace.windowAdded.connect(window => {
            Utils.setupAutoHide(window);
        });
        Workspace.windowRemoved.connect(window => Utils.removeTrayIcon(window));
        Workspace.windows.forEach(window => Utils.setup(window));
    }
}
