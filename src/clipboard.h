/*
 * clipboard.h - Clipboard read / write helpers for LangSwitcher
 */

#pragma once

#include <windows.h>
#include <stdbool.h>

/*
 * Save the current Unicode text on the clipboard.
 *
 * Returns a newly allocated wchar_t* that the caller must free(),
 * or NULL if the clipboard is empty or holds no text.
 */
wchar_t *clipboard_save(void);

/*
 * Write `text` to the clipboard as CF_UNICODETEXT.
 *
 * Returns true on success.
 */
bool clipboard_write(const wchar_t *text);

/*
 * Restore the clipboard to a previously saved state.
 *
 * If `saved` is NULL the clipboard is simply emptied.
 * Does NOT free `saved`; the caller remains responsible.
 */
void clipboard_restore(const wchar_t *saved);
