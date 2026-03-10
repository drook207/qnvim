<!--SPDX-FileCopyrightText: none-->
<!--SPDX-License-Identifier: CC0-1.0-->

# qnvim

qnvim is a Qt Creator plugin that embeds a real Neovim instance as the text editor.
It lets you use your full `init.lua` / `init.vim`, all your Neovim plugins, and every
customisation you have — while keeping Qt Creator's project management, code navigation,
debugger, and build system at your fingertips.

<p align="center">
  <a href="https://www.youtube.com/watch?v=twwnnduujzw">
    <img src="https://user-images.githubusercontent.com/1270688/51085365-02e51900-174d-11e9-92f7-c6daa5ec33de.gif"/>
  </a>
</p>

## How it works

qnvim spawns a headless Neovim process and connects to it over the msgpack-RPC channel
provided by [neovim-qt](https://github.com/equalsraf/neovim-qt).  Every open file in Qt
Creator is mirrored as a Neovim buffer.  Keyboard input is forwarded to Neovim; cursor
movements, selections, and text changes flow back to the Qt Creator editor in real time.
The Neovim command line is embedded in the Qt Creator status bar.

A set of Ex commands is registered automatically so you can drive Qt Creator's build
system from Neovim:

| Ex command        | Qt Creator action                     |
|-------------------|---------------------------------------|
| `:Build`          | Build active project                  |
| `:BuildAll`       | Build all projects in session         |
| `:Rebuild`        | Rebuild active project                |
| `:RebuildAll`     | Rebuild all projects in session       |
| `:Clean`          | Clean active project                  |
| `:CleanAll`       | Clean all projects in session         |
| `:Deploy`         | Deploy active project                 |
| `:DeployAll`      | Deploy all projects in session        |
| `:Run`            | Run active project                    |
| `:Debug`          | Start debugger                        |
| `:DebugContinue`  | Continue in debugger                  |
| `:QMake`          | Run qmake                             |
| `:Target`         | Quick-select build target             |

## Status

qnvim is under active development and is mostly stable for daily use.  Known limitations:

- Splits and windows are not supported (Neovim operates as a single-pane UI).
- Buffers that rely on Neovim-specific highlights (e.g. Vim help pages, special file types)
  may not render colours correctly — Qt Creator's own syntax highlighting takes precedence
  for source files.

Most Neovim plugins work out of the box.  Plugins that depend on custom highlight groups or
terminal emulation inside Neovim splits may have limited functionality.

Please [report issues](https://github.com/sassanh/qnvim/issues) and consider contributing
if you have the time.

## How does qnvim compare with Qt Creator's built-in Vim mode?

| | qnvim | Qt Creator Vim Mode |
|---|---|---|
| Neovim engine | Real Neovim process | Vim emulation layer |
| `init.lua` / `init.vim` | Full support | Not supported |
| Neovim plugins | Full support | Not supported |
| Performance | Near-native | Native |
| Qt Creator integration | Full | Full |
| Splits / windows | Not yet | N/A |

The bottom line: qnvim runs an actual Neovim instance so everything you already have in
your Neovim configuration just works.  Qt Creator Vim mode is a self-contained emulation
that requires no external dependencies but does not support the Neovim plugin ecosystem.

## Installation

### From the Releases page

Download the plugin archive that matches your Qt Creator **major.minor** version and
operating system from the [Releases](https://github.com/sassanh/qnvim/releases) section.

1. Open Qt Creator → **Help** → **About Plugins** → **Install Plugin…**
2. Select the downloaded archive and relaunch Qt Creator.

### Building from source

> **Warning** — Qt Creator plugin APIs are versioned strictly by major and minor version.
> The `master` branch targets the Qt Creator version pinned in
> `cmake/FetchQtCreator.cmake`.  Check out the Git tag that matches your installed Qt
> Creator version before building.

**Prerequisites:** Qt 6 development files and a C++17-capable compiler.

```bash
# 1. Clone and enter the repository
git clone https://github.com/sassanh/qnvim.git
cd qnvim

# 2. (Optional) check out the tag matching your Qt Creator version
git checkout <tag>

# 3. Configure and build
cmake -S . -B build/
cmake --build build/
```

The compiled plugin ends up in `build/lib/qtcreator/plugins/`.  Install it via
**Help** → **About Plugins** → **Install Plugin…** as above.

#### Updating from source

Delete the `build/` directory before reconfiguring to avoid stale-cache issues:

```bash
rm -rf build/
cmake -S . -B build/
cmake --build build/
```

### Arch Linux (AUR)

```bash
# Via an AUR helper
paru -S qnvim-git

# Or manually
git clone https://aur.archlinux.org/qnvim-git.git
cd qnvim-git
makepkg -si
```

## Configuration

### Toggling QNVim

Use **Tools** → **QNVim** → **Toggle QNVim** or the default shortcut
**Alt+Shift+V, Alt+Shift+V** to enable or disable qnvim at runtime without restarting
Qt Creator.

### `qnvim.vim`

Place a `qnvim.vim` file in the same directory as your `init.vim` / `init.lua`
(`:help $MYVIMRC` shows the path).  The path is exposed to Neovim as `$MYQVIMRC` and the
file is sourced automatically on startup.

This is the right place to put Qt Creator-specific keybindings and settings that you do
not want to bleed into your standalone Neovim sessions.

### Sample `qnvim.vim`

`examples/qnvim.vim` in this repository demonstrates how to:

- Map keys to Qt Creator actions (build, deploy, run, debug, etc.).
- Switch between open editors and tabs.
- Call any registered Qt Creator action by ID via `rpcnotify`.

## Logging

Two Qt logging categories are available for debugging:

| Category | Default level | Content |
|---|---|---|
| `qtcreator.plugin.qnvim.core` | `debug` | General plugin lifecycle and RPC events |
| `qtcreator.plugin.qnvim.buffer` | `warning` | Buffer synchronisation details |

Enable them at runtime with:

```bash
QT_LOGGING_RULES="qtcreator.plugin.qnvim.*=true" qtcreator
```

## Credits

- [Neovim](https://neovim.io) — the embedded editor engine
- [Qt](https://www.qt.io) — the application framework
- [Qt Creator](https://www.qt.io/product) — the IDE this plugin extends
- [neovim-qt](https://github.com/equalsraf/neovim-qt) — the Qt/msgpack-RPC bridge
