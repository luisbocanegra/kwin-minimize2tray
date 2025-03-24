import QtQuick
import org.kde.kwin
import com.github.luisbocanegra.trayicon 1.0

Item {
    id: root
    readonly property bool enableDebug: KWin.readConfig("EnableDebug", false)
    property var trayIcons: new Object()
    readonly property Component trayIconComponent: TrayIcon {}

    function dumpProps(obj) {
        for (var k of Object.keys(obj)) {
            const val = obj[k];
            if (k === 'metaData')
                continue;
            print(k + "=" + val + "\n");
        }
    }

    function setup(window) {
        if (!window.normalWindow) return
        window.captionChanged.connect(() => {
            updateToolTip(window)
        })
        window.desktopsChanged.connect(() => {
            updateToolTip(window)
        })
        window.outputChanged.connect(() => {
            updateToolTip(window)
        })
        window.demandsAttentionChanged.connect(() => {
            updateDemandsAttention(window)
        })
    }

    function setupTrayicon(window) {
        if (window.skipTaskbar) {
            addTrayIcon(window)
        } else {
            removeTrayIcon(window)
        }
    }

    function formattedWindowInfo(w) {
        let desktops
        if (w.onAllDesktops) {
            desktops = "All Desktops"
        } else {
            desktops = w.desktops.map(desktop => desktop.name).join(", ")
        }
        return `${w.caption||w.resourceName||w.resourceClass}\n${desktops} @ ${w.output.name}`
    }

    function addTrayIcon(window) {
        const windowId = window.internalId
        if (windowId in trayIcons) {
            return;
        }
        let launcherUrl = window.desktopFileName
        launcherUrl = "application://" + launcherUrl
        launcherUrl = launcherUrl + ".desktop"

        const trayItem = trayIconComponent.createObject(root, {
            "icon": window.icon,
            "windowId": windowId,
            "toolTipText": formattedWindowInfo(window),
            "launcherUrl": launcherUrl,
            "xdgName": window.desktopFileName
        });
        trayItem.requestShowHide.connect(toggleShowHide)
        trayItem.requestClose.connect(closeWindow)
        trayItem.requestUnpin.connect(unpinIcon)
        trayIcons[windowId] = trayItem;
    }

    function removeTrayIcon(window) {
        const windowId = window.internalId
        if (!(windowId in trayIcons)) {
            return
        }
        trayIcons[windowId].destroy();
        delete trayIcons[windowId];
    }

    function setMinimize(minimize, window) {
        window.minimized = minimize;
    }

    function setSkip(skip, window) {
        window.skipPager = skip;
        window.skipTaskbar = skip;
        window.skipSwitcher = skip;
    }

    function toggleMinimize(windowId) {
        const window = getWindow(windowId)
        if (!window) return
        window.minimized = !window.minimized
    }

    function toggleShowHide(windowId) {
        const window = getWindow(windowId)
        if (!window) return

        if (window.minimized) {
            setSkip(false, window)
        } else {
            setSkip(true, window)
        }
        window.minimized = !window.minimized
        if (!window.minimized) Workspace.activeWindow = window
    }

    function getWindow(windowId) {
        for (let window of Workspace.windows) {
            if (window.internalId === windowId) {
                return window
            }
        }
        return null
    }

    function closeWindow(windowId) {
        const window = getWindow(windowId)
        window.closeWindow()
    }

    function unpinIcon(windowId) {
        const window = getWindow(windowId)
        if (!window) return
        setMinimize(false, window)
        setSkip(false, window)
        removeTrayIcon(window)
        Workspace.activeWindow = window
    }

    function updateToolTip(window) {
        if (window.internalId in trayIcons) {
            trayIcons[window.internalId].toolTipText = formattedWindowInfo(window)
        }
    }

    function updateDemandsAttention(window) {
        if (window.internalId in trayIcons) {
            trayIcons[window.internalId].demandsAttention = window.demandsAttention
        }
    }


    ShortcutHandler {
        name: "Minimize to tray"
        text: "Minimize window to tray (sets skip Taskbar, Switcher & Pager)"
        sequence: "Alt+S"
        onActivated: {
            const window = Workspace.activeWindow
            toggleShowHide(window.internalId)
            root.addTrayIcon(window)
        }
    }

    Component.onCompleted: {
        Workspace.windowAdded.connect(w => setup(w));
        Workspace.windowRemoved.connect(w => removeTrayIcon(w));
        Workspace.windows.forEach(w => setup(w));
    }
}
