/*
 * clipboard.c - Clipboard read / write helpers implementation
 */

#include "clipboard.h"

#include <stdlib.h>
#include <wchar.h>

/* ---------------------------------------------------------------------------
 * clipboard_save
 * --------------------------------------------------------------------------- */
wchar_t *clipboard_save(void)
{
    if (!OpenClipboard(NULL))
        return NULL;

    wchar_t *saved = NULL;
    HANDLE h = GetClipboardData(CF_UNICODETEXT);

    if (h) {
        const wchar_t *src = (const wchar_t *)GlobalLock(h);
        if (src) {
            size_t n = wcslen(src) + 1;
            saved = (wchar_t *)malloc(n * sizeof(wchar_t));
            if (saved)
                wcscpy(saved, src);
            GlobalUnlock(h);
        }
    }

    CloseClipboard();
    return saved;
}

/* ---------------------------------------------------------------------------
 * clipboard_write
 * --------------------------------------------------------------------------- */
bool clipboard_write(const wchar_t *text)
{
    if (!text)
        return false;

    size_t  bytes = (wcslen(text) + 1) * sizeof(wchar_t);
    HGLOBAL hMem  = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!hMem)
        return false;

    wchar_t *dst = (wchar_t *)GlobalLock(hMem);
    if (!dst) {
        GlobalFree(hMem);
        return false;
    }
    wcscpy(dst, text);
    GlobalUnlock(hMem);

    if (!OpenClipboard(NULL)) {
        GlobalFree(hMem);
        return false;
    }

    EmptyClipboard();
    HANDLE h = SetClipboardData(CF_UNICODETEXT, hMem);
    CloseClipboard();

    /* If SetClipboardData failed the system owns hMem; don't double-free */
    return (h != NULL);
}

/* ---------------------------------------------------------------------------
 * clipboard_restore
 * --------------------------------------------------------------------------- */
void clipboard_restore(const wchar_t *saved)
{
    if (saved) {
        clipboard_write(saved);
    } else {
        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            CloseClipboard();
        }
    }
}
