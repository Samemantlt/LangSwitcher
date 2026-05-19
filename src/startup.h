/*
 * startup.h - "Start with Windows" helpers for LangSwitcher
 *
 * Because the executable requires administrator elevation, autostart is
 * implemented via a Task Scheduler task (run level HIGHEST) rather than the
 * HKCU\...\Run registry key — Windows silently skips Run entries that would
 * trigger a UAC prompt at login.
 */

#pragma once

#include <windows.h>

/*
 * Returns TRUE if the LangSwitcher autostart task exists in Task Scheduler.
 */
BOOL startup_is_enabled(void);

/*
 * Creates or deletes the Task Scheduler autostart task.
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
