# Libyui-qt Keyboard Shortcuts

The libyui-qt interface implements several keyboard shortcuts.

## Global Shortcuts

These shortcuts can be used anytime, it does not matter which widget is active
(or if any widget is active at all).

- `Ctrl+PrtScn` - Save a screenshot to a file

- `Shift-F3` - Display a popup for switching the color theme

- `Shift-F4` - Toggle colors for vision impaired users

- `Shift+F6` - Ask for widget ID. Enter the ID of the button which should be
  activated. This is intended for automated tests.

- `Shift-F7` - Toggle debug logging (on/off)

- `Shift-F8` - Save the logs (using `/usr/sbin/save_y2logs`)

- `Ctrl+Alt+Shift+X` - Start a terminal console (`xterm`)

- `Ctrl+Alt+Shift+M` - Start/stop macro recording.

- `Ctrl+Alt+Shift+P` - Replay a macro file, see the previous shortcut.

- `Ctrl+Alt+Shift+D` - Send a debug event. This should start the debugger in the
  running application.

- `Ctrl+Alt+Shift+C` - Send a configuration event. This should display a special
  dialog for configuring the application.

- `Ctrl+Alt+Shift+T` - Dump the current widget tree into the log.

- `Ctrl+Alt+Shift+Y` - Start the widget inspector. Displays a special dialog which
  can navigate in the widget tree and display detailed widget properties. It can
  also change some widget properties and even delete or add new widgets.

- `Ctrl+Alt+Shift+S` - Open a style editor. Allows changing/editing the style
  at runtime.
