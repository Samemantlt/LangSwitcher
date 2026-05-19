/*
 * tray.c - System tray icon and context menu implementation
 */

#include "tray.h"

#include <ole2.h>       /* provides REFIID needed by old MinGW shellapi.h */
#include <shellapi.h>
#include <wchar.h>

/* Module-private tray data */
static NOTIFYICONDATAW s_nid = {0};

/* ---------------------------------------------------------------------------
 * tray_add
 * --------------------------------------------------------------------------- */
void tray_add(HWND hWnd)
{
    s_nid.cbSize           = sizeof(s_nid);
    s_nid.hWnd             = hWnd;
    s_nid.uID              = 1;
    s_nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    s_nid.uCallbackMessage = WM_TRAYICON;
    s_nid.hIcon            = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy(s_nid.szTip, L"LangSwitcher  [Shift+Scroll Lock]");
    Shell_NotifyIconW(NIM_ADD, &s_nid);
}

/* ---------------------------------------------------------------------------
 * tray_remove
 * --------------------------------------------------------------------------- */
void tray_remove(void)
{
    Shell_NotifyIconW(NIM_DELETE, &s_nid);
}

/* ---------------------------------------------------------------------------
 * tray_show_menu
 * --------------------------------------------------------------------------- */
void tray_show_menu(HWND hWnd)
{
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING | MF_GRAYED, ID_TRAY_ABOUT,
                L"LangSwitcher  [Shift+Scroll Lock]");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, ID_TRAY_ABOUT, L"About...");
    AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT,  L"Exit");

    POINT pt;
    GetCursorPos(&pt);
    /* Required so the menu closes when clicking elsewhere */
    SetForegroundWindow(hWnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
    DestroyMenu(hMenu);
}
