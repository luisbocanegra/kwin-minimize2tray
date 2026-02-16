import QtQuick
import org.kde.kwin
import com.github.luisbocanegra.trayicon 1.0
import com.github.luisbocanegra.kwinrcwatcher 1.0
import "../code/utils.js" as Utils

Item {
    id: root
    property bool debugEnabled
    property var hideByDefaultClass
    property bool countUseDot
    property bool hideOnMinimize
    property var trayIcons: new Object()
    readonly property Component trayIconComponent: TrayIcon {}

    function reloadConfig() {
        console.log("KWin Minimize2Tray Reloading configuration");
        debugEnabled = KWin.readConfig("debugEnabled", false);
        hideByDefaultClass = Utils.commaSeparate(KWin.readConfig("hideByDefaultClass", ""));
        countUseDot = KWin.readConfig("countUseDot", false);
        hideOnMinimize = KWin.readConfig("hideOnMinimize", false);
        console.log(JSON.stringify(hideByDefaultClass));
    }

    function addTrayIcon(window) {
        const windowId = window.internalId;
        if (windowId in trayIcons) {
            return;
        }
        let launcherUrl = window.desktopFileName;
        launcherUrl = "application://" + launcherUrl;
        launcherUrl = launcherUrl + ".desktop";

        const trayItem = trayIconComponent.createObject(root, {
            "icon": window.icon,
            "windowId": windowId,
            "toolTipText": Utils.formattedWindowInfo(window),
            "launcherUrl": launcherUrl,
            "xdgName": window.desktopFileName,
            "countUseDot": Qt.binding(function () { return root.countUseDot })
        });
        trayItem.requestShowHide.connect(Utils.toggleShowHide);
        trayItem.requestClose.connect(Utils.closeWindow);
        trayItem.requestUnpin.connect(Utils.unpinIcon);
        trayIcons[windowId] = trayItem;
    }

    KwinrcWatcher {
        onConfigChanged: root.reloadConfig()
    }

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
            root.addTrayIcon(window);
        }
    }

    Component.onCompleted: {
        reloadConfig();
        Workspace.windowAdded.connect(window => Utils.setup(window));
        Workspace.windowAdded.connect(window => {
            Utils.setupAutoHide(window);
        });
        Workspace.windowRemoved.connect(window => Utils.removeTrayIcon(window));
        Workspace.windows.forEach(window => Utils.setup(window));
    }
}
