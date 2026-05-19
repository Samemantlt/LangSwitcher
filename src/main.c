/*
 * main.c - LangSwitcher entry point
 *
 * Responsibilities:
 *   - Register the window class and create a hidden message-only window.
 *   - Register the global hotkey.
 *   - Add the system tray icon.
 *   - Run the message loop.
 *   - Dispatch hotkey / tray / command messages to the appropriate modules.
 *
 * Note: UNICODE, _UNICODE, _WIN32_WINNT, WIN32_LEAN_AND_MEAN are defined
 *       via compiler flags (CMakeLists.txt) — do not redefine here.
 */

#include <windows.h>

#include "hotkey.h"
#include "startup.h"
#include "switcher.h"
#include "tray.h"

/* ---------------------------------------------------------------------------
 * Window procedure
 * --------------------------------------------------------------------------- */
static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg,
                                 WPARAM wParam, LPARAM lParam)
{
    switch (msg) {

    /* ------------------------------------------------------------------
     * Global hotkey fired
     * ------------------------------------------------------------------ */
    case WM_HOTKEY:
        if ((int)wParam == HOTKEY_ID)
            do_switch();
        else if ((int)wParam == HOTKEY_ID_CAPS)
            do_caps_switch();
        return 0;

    /* ------------------------------------------------------------------
     * Tray icon interaction
     * ------------------------------------------------------------------ */
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONUP)
            tray_show_menu(hWnd);
        return 0;

    /* ------------------------------------------------------------------
     * Tray menu commands
     * ------------------------------------------------------------------ */
    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case ID_TRAY_STARTUP:
            startup_set(!startup_is_enabled());
            break;

        case ID_TRAY_EXIT:
            PostQuitMessage(0);
            break;

        case ID_TRAY_ABOUT:
            MessageBoxW(NULL,
                L"LangSwitcher\n\n"
                L"Select text in any application, then:\n\n"
                L"  Shift + PauseBreak\n"
                L"    Switch keyboard layout (EN \u2194 RU)\n\n"
                L"  Shift + Alt + PauseBreak\n"
                L"    Toggle caps  (HELLO guy \u2192 hello GUY)\n\n"
                L"Right-click the tray icon to exit.",
                L"About LangSwitcher",
                MB_OK | MB_ICONINFORMATION);
            break;
        }
        return 0;

    /* ------------------------------------------------------------------
     * Window is being destroyed — clean up
     * ------------------------------------------------------------------ */
    case WM_DESTROY:
        tray_remove();
        UnregisterHotKey(hWnd, HOTKEY_ID);
        UnregisterHotKey(hWnd, HOTKEY_ID_CAPS);
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}

/* ---------------------------------------------------------------------------
 * WinMain
 * --------------------------------------------------------------------------- */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    /* Register a hidden window class */
    WNDCLASSEXW wc    = {0};
    wc.cbSize         = sizeof(wc);
    wc.lpfnWndProc    = WndProc;
    wc.hInstance      = hInstance;
    wc.lpszClassName  = L"LangSwitcherClass";

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Failed to register window class.",
                    L"LangSwitcher", MB_ICONERROR);
        return 1;
    }

    /* Create a message-only window (invisible, no taskbar button) */
    HWND hWnd = CreateWindowExW(
        0, L"LangSwitcherClass", L"LangSwitcher",
        0, 0, 0, 0, 0,
        HWND_MESSAGE, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBoxW(NULL, L"Failed to create window.",
                    L"LangSwitcher", MB_ICONERROR);
        return 1;
    }

    /* Register the global hotkeys */
    if (!RegisterHotKey(hWnd, HOTKEY_ID, HOTKEY_MOD, HOTKEY_VK)) {
        MessageBoxW(NULL,
            L"Failed to register Shift+Pause hotkey.\n"
            L"Another instance may already be running.",
            L"LangSwitcher", MB_ICONWARNING);
        return 1;
    }
    if (!RegisterHotKey(hWnd, HOTKEY_ID_CAPS, HOTKEY_CAPS_MOD, HOTKEY_CAPS_VK)) {
        MessageBoxW(NULL,
            L"Failed to register Shift+Alt+Pause hotkey.\n"
            L"The caps-toggle feature will be unavailable.",
            L"LangSwitcher", MB_ICONWARNING);
        /* Non-fatal: continue running with layout-switch only */
    }

    /* Add tray icon */
    tray_add(hWnd);

    /* On first run, ask the user whether to start with Windows */
    startup_prompt_if_first_run();

    /* Message loop */
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}
