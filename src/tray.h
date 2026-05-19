/*
 * tray.h - System tray icon and context menu for LangSwitcher
 */

#pragma once

#include <windows.h>

/* Message sent by the shell when the user interacts with the tray icon */
#define WM_TRAYICON     (WM_USER + 1)

/* Context-menu command IDs */
#define ID_TRAY_ABOUT   1001
#define ID_TRAY_EXIT    1002
#define ID_TRAY_STARTUP 1003

/* Add the tray icon for `hWnd`. Call once at startup. */
void tray_add(HWND hWnd);

/* Remove the tray icon. Call before the program exits. */
void tray_remove(void);

/* Show the right-click context menu at the current cursor position. */
void tray_show_menu(HWND hWnd);
