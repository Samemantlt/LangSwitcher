/*
 * switcher.c - High-level layout-switch action implementation
 *
 * Sequence:
 *   1. Save current clipboard text.
 *   2. Empty clipboard (so we can detect whether Ctrl+C worked).
 *   3. Simulate Ctrl+C to copy the user's selection.
 *   4. Read the clipboard text.
 *   5. Detect dominant script and convert accordingly.
 *   6. Write converted text to clipboard.
 *   7. Simulate Ctrl+V to paste.
 *   8. Restore the original clipboard content.
 */

#include "switcher.h"

#include "clipboard.h"
#include "converter.h"
#include "input.h"

#include <windows.h>
#include <stdlib.h>
#include <wchar.h>

/* Milliseconds to wait for the target application to process clipboard events */
#define DELAY_BEFORE_COPY_MS    50
#define DELAY_AFTER_COPY_MS    200
#define DELAY_BEFORE_PASTE_MS   50
#define DELAY_AFTER_PASTE_MS   150
#define DELAY_BEFORE_RESTORE_MS 50

void do_switch(void)
{
    /* --- 1. Save current clipboard ---------------------------------------- */
    wchar_t *saved = clipboard_save();

    /* --- 2. Empty clipboard so we can detect whether Ctrl+C produced text -- */
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        CloseClipboard();
    }

    /* --- 3. Copy selection ------------------------------------------------- */
    /* Release any physically-held modifiers (Shift is held because the user
     * pressed Shift+hotkey). Without this, Ctrl+C becomes Ctrl+Shift+C.   */
    release_modifiers();
    Sleep(DELAY_BEFORE_COPY_MS);
    send_ctrl('C');
    Sleep(DELAY_AFTER_COPY_MS);

    /* --- 4. Read clipboard ------------------------------------------------- */
    if (!OpenClipboard(NULL)) {
        clipboard_restore(saved);
        free(saved);
        return;
    }

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (!hData) {
        CloseClipboard();
        clipboard_restore(saved);
        free(saved);
        return;
    }

    const wchar_t *raw = (const wchar_t *)GlobalLock(hData);
    if (!raw) {
        CloseClipboard();
        clipboard_restore(saved);
        free(saved);
        return;
    }

    size_t   len = wcslen(raw);
    wchar_t *buf = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
    if (!buf) {
        GlobalUnlock(hData);
        CloseClipboard();
        clipboard_restore(saved);
        free(saved);
        return;
    }

    wcscpy(buf, raw);
    GlobalUnlock(hData);
    CloseClipboard();

    /* --- 5. Convert (swap every character to the other layout) ------------ */
    convert_layout(buf, len);

    /* --- 6. Write converted text to clipboard ------------------------------ */
    clipboard_write(buf);
    free(buf);

    /* --- 7. Paste ---------------------------------------------------------- */
    Sleep(DELAY_BEFORE_PASTE_MS);
    send_ctrl('V');
    Sleep(DELAY_AFTER_PASTE_MS);

    /* --- 8. Restore original clipboard ------------------------------------- */
    Sleep(DELAY_BEFORE_RESTORE_MS);
    clipboard_restore(saved);
    free(saved);
}
