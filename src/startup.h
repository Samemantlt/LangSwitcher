/*
 * startup.h - "Start with Windows" registry helpers for LangSwitcher
 *
 * Uses HKCU\Software\Microsoft\Windows\CurrentVersion\Run so that no
 * elevated privileges are required.
 */

#pragma once

#include <windows.h>

/*
 * Returns TRUE if the autostart registry value is present and points to
 * the current executable.
 */
BOOL startup_is_enabled(void);

/*
 * Adds or removes the autostart registry value.
 * Pass TRUE to enable, FALSE to disable.
 * Returns TRUE on success.
 */
BOOL startup_set(BOOL enable);

/*
 * If this is the very first run (no "FirstRunDone" registry flag found),
 * shows a MessageBox asking the user whether to start LangSwitcher with
 * Windows.  Records the flag so the prompt is never shown again.
 */
void startup_prompt_if_first_run(void);
