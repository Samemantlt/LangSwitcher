/*
 * hotkey.h - Global hotkey constants for LangSwitcher
 *
 * Default hotkey: Shift + Pause/Break
 * Change HOTKEY_MOD / HOTKEY_VK to remap.
 */

#pragma once

#include <windows.h>

#define HOTKEY_ID       1

#ifndef MOD_NOREPEAT
/* MOD_NOREPEAT was added in Win8 SDK; define it ourselves for older toolchains */
#define MOD_NOREPEAT    0x4000
#endif

#define HOTKEY_MOD      (MOD_SHIFT | MOD_NOREPEAT)
#define HOTKEY_VK       VK_PAUSE    /* Shift + Pause/Break */
