# KWin Minimize2Tray

**This is work in progress, don't expect feature parity with other implementations right away**

Hide windows to the system tray, similar to [KDocker](https://github.com/user-none/KDocker) but in the form of a KWin Script that works on Wayland.

![preview](screenshots/preview.png)

> [!IMPORTANT]
> Some applications like Google Chrome or Chromium/Electron based may run in Xwayland mode and have issues hiding from taskbar or minimizing on startup, enabling native Wayland support for them can help. Usually setting an environment variable or command-line argument with KDE's `kmenuedit` is needed, refer to [Wayland - ArchWiki](https://wiki.archlinux.org/title/Wayland#GUI_libraries) or search "linux APP_NAME enable wayland support".
>
> To verify if an application is running on Xwayland instead of Wayland use the KWin Debug Console.
> ![wayland](screenshots/wayland.png)
> You can also use the `xlsclients` in a terminal (usually from x11-tools or x11-utils packages).

## Features

- Wayland support (X11 hasn't been tested, let me know if it works)
- Should work with windows from any application **as long as they are minimizable**
- Windows are hidden from Task Manager (taskbar), Task Switcher (Alt+Tab), Desktop Pager and restored when clicking on the tray icon
- Per application visibility can be configured from System Tray widget configuration
- Application and window status
  - [Unity LauncherAPI](https://wiki.ubuntu.com/Unity/LauncherAPI)
    - Counter: Top-right corner (optional dot style can be enabled from KWin script settings)
    - Progress bar: Green bottom bar
    - Urgent: Sets *NeedsAttention* status of icon

    **LauncherAPI doesn't specify a window so all hidden windows from the same application will get the same status**
  - *Demands attention* from KWin also sets *NeedsAttention* status of icon per window

## Installation

No KDE Store this time as the C++ plugin needs to be compiled manually

**You will need to install any required dependencies**

```sh
git clone https://github.com/luisbocanegra/kwin-minimize2tray.git
cd kwin-minimize2tray
./install.sh
```

### Immutable distributions

1. Run `./install-immutable.sh` to install
2. Add `QML_IMPORT_PATH` environment variable for the C++ plugin to work:

    Create the file `~/.config/plasma-workspace/env/path.sh` (and folders if they don't exist) with the following:

    ```sh
    export QML_IMPORT_PATH="$HOME/.local/lib64/qml:$HOME/.local/lib/qml:$QML_IMPORT_PATH"
    ```

3. Log-out or reboot to apply the change

## Configuration/Usage

Enable/disable

1. *Minimize to tray* from *System Settings* > *Window Management* > *KWin Scripts*

By default the shortcut is set to `Meta+Alt+PgDown`, to change it:

1. *System Settings* > *Keyboard* > *Shortcuts*
2. From the list on the left select *KWin*
3. Search for "Minimize window to tray" and change it to whatever you prefer

## Acknowledgements

[Unity LauncherAPI](https://wiki.ubuntu.com/Unity/LauncherAPI) implementation is a simplified version of [KDE/plasma-desktop/applets/taskmanager/plugin/smartlaunchers](https://github.com/KDE/plasma-desktop/tree/e3ba92b113d8a4e2d47a589835e9d867059dc2b9/applets/taskmanager/plugin/smartlaunchers)

## Support the development

If you like what I do consider donating/sponsoring this and [my other open source work](https://github.com/luisbocanegra?tab=repositories&q=&type=source&language=&sort=stargazers)

[![GitHub Sponsors](https://img.shields.io/badge/GitHub_Sponsors-supporter?logo=githubsponsors&color=%2329313C)](https://github.com/sponsors/luisbocanegra) [![Ko-fi](https://img.shields.io/badge/Ko--fi-supporter?logo=ko-fi&logoColor=%23ffffff&color=%23467BEB)](https://ko-fi.com/luisbocanegra) [!["Buy Me A Coffee"](https://img.shields.io/badge/Buy%20Me%20a%20Coffe-supporter?logo=buymeacoffee&logoColor=%23282828&color=%23FF803F)](https://www.buymeacoffee.com/luisbocanegra) [![Liberapay](https://img.shields.io/badge/Liberapay-supporter?logo=liberapay&logoColor=%23282828&color=%23F6C814)](https://liberapay.com/luisbocanegra/) [![PayPal](https://img.shields.io/badge/PayPal-supporter?logo=paypal&logoColor=%23ffffff&color=%23003087)](https://www.paypal.com/donate/?hosted_button_id=Y5TMH3Z4YZRDA)
