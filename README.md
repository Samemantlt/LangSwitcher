# LangSwitcher

A lightweight Windows background utility that converts mistyped text between **English and Russian** keyboard layouts — and can also toggle letter case — without leaving your current application.

---

## Features

| Hotkey | Action |
|---|---|
| **Shift + Pause/Break** | Convert selected text between EN ↔ RU layouts |
| **Shift + Alt + Pause/Break** | Toggle case of selected letters (`HELLO guy` → `hello GUY`) |

- Runs silently in the **system tray** — no console window, no taskbar button.
- Works in **any application** (browsers, editors, chat apps, etc.).
- Restores the clipboard to its original content after each operation.
- Re-selects the converted text so you can keep editing immediately.
- Single tiny native executable; no runtime dependencies.

---

## How It Works

1. Select the mistyped text in any application.
2. Press the appropriate hotkey.
3. LangSwitcher copies the selection, converts it in-memory, pastes it back, and re-selects it — all in one seamless action.

**Layout conversion** maps every character to its counterpart on the opposite physical key:

```
EN → RU:  q→й  w→ц  e→у  r→к  t→е  ...
RU → EN:  й→q  ц→w  у→e  к→r  е→t  ...
```

Both directions are handled in a single pass with no pre-detection required. Characters with no mapping (digits, spaces, unpaired punctuation) are left unchanged.

**Case toggling** inverts the case of every letter in-place, supporting both Latin (A–Z / a–z) and Cyrillic (А–Я Ё / а–я ё) code points.

---

## Requirements

| Requirement | Details |
|---|---|
| **OS** | Windows 7 or later (64-bit or 32-bit) |
| **Compiler** | [MinGW-w64](https://www.mingw-w64.org/) (`gcc`) |
| **Build system** | [CMake](https://cmake.org/) ≥ 3.15 |

---

## Building

### Quick build (batch script)

```bat
build.bat
```

The script configures and builds in one step using `MinGW Makefiles`. The resulting executable is placed at `build\LangSwitcher.exe`.

### Manual build

```bat
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

> **Note:** CMake must be on your `PATH`, or edit `build.bat` to point to your CMake installation.

---

## Running

Double-click `build\LangSwitcher.exe`.  
A tray icon appears in the notification area. Right-click it for the **About** dialog or to **Exit**.

To launch automatically on login, create a shortcut to the executable and place it in:

```
%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup
```

---

## Project Structure

```
LangSwitcher/
├── src/
│   ├── main.c          # WinMain, message loop, hotkey & tray dispatch
│   ├── hotkey.h        # Hotkey IDs, modifiers, and virtual key constants
│   ├── switcher.c/.h   # High-level do_switch() and do_caps_switch() actions
│   ├── converter.c/.h  # convert_layout() and toggle_caps() — pure text transforms
│   ├── clipboard.c/.h  # Clipboard save / write / restore helpers
│   ├── input.c/.h      # SendInput() wrappers (key simulation, modifier release)
│   ├── tray.c/.h       # System tray icon and context menu
│   └── resources.rc    # Windows resource file (application icon)
├── icon.ico            # Application icon
├── CMakeLists.txt      # CMake build definition
├── build.bat           # One-click build script
└── cmake.bat           # CMake-only configure step
```

---

## Customizing the Hotkey

Open `src/hotkey.h` and change the constants for either hotkey:

```c
// Layout switch: Shift + Pause/Break (default)
#define HOTKEY_MOD   (MOD_SHIFT | MOD_NOREPEAT)
#define HOTKEY_VK    VK_PAUSE

// Caps toggle: Shift + Alt + Pause/Break (default)
#define HOTKEY_CAPS_MOD   (MOD_SHIFT | MOD_ALT | MOD_NOREPEAT)
#define HOTKEY_CAPS_VK    VK_PAUSE
```

Any `MOD_*` modifier and `VK_*` virtual key from the [Windows API](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes) can be used. Rebuild after editing.
