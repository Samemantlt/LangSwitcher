/*
 * hotkey.h - Global hotkey constants for LangSwitcher
 *
 * Hotkey 1: Shift + Pause/Break       -> switch keyboard layout
 * Hotkey 2: Shift + Alt + Pause/Break -> toggle caps (UPPER <-> lower)
 *
 * Change HOTKEY_MOD / HOTKEY_VK or HOTKEY_CAPS_MOD / HOTKEY_CAPS_VK to remap.
 */

#pragma once

#include <windows.h>

#define HOTKEY_ID           1   /* layout switch  */
#define HOTKEY_ID_CAPS      2   /* caps toggle    */

#ifndef MOD_NOREPEAT
/* MOD_NOREPEAT was added in Win8 SDK; define it ourselves for older toolchains */
#define MOD_NOREPEAT        0x4000
#endif

#define HOTKEY_MOD          (MOD_SHIFT | MOD_NOREPEAT)
#define HOTKEY_CAPS_MOD     (MOD_SHIFT | MOD_ALT | MOD_NOREPEAT)
#define HOTKEY_VK           VK_PAUSE    /* Shift + Pause/Break           */
#define HOTKEY_CAPS_VK      VK_PAUSE    /* Shift + Alt + Pause/Break     */
