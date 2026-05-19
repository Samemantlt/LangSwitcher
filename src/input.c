/*
 * input.c - Low-level keyboard input simulation implementation
 */

#include "input.h"

#include <stddef.h>
#include <stdlib.h>

/* ---------------------------------------------------------------------------
 * send_key
 * --------------------------------------------------------------------------- */
void send_key(WORD vk, bool down)
{
    INPUT inp      = {0};
    inp.type       = INPUT_KEYBOARD;
    inp.ki.wVk     = vk;
    inp.ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
    SendInput(1, &inp, sizeof(INPUT));
}

/* ---------------------------------------------------------------------------
 * send_ctrl
 * --------------------------------------------------------------------------- */
void send_ctrl(WORD vk)
{
    send_key(VK_CONTROL, true);
    send_key(vk,         true);
    send_key(vk,         false);
    send_key(VK_CONTROL, false);
}

/* ---------------------------------------------------------------------------
 * release_modifiers
 *
 * Sends a key-up event for every modifier that GetAsyncKeyState reports as
 * currently held.  This prevents a physically-held Shift (from Shift+hotkey)
 * from leaking into subsequent synthetic Ctrl+C / Ctrl+V presses.
 * --------------------------------------------------------------------------- */
void release_modifiers(void)
{
    static const WORD mods[] = {
        VK_LSHIFT,   VK_RSHIFT,
        VK_LCONTROL, VK_RCONTROL,
        VK_LMENU,    VK_RMENU,    /* Alt */
        VK_LWIN,     VK_RWIN,
    };

    for (int i = 0; i < (int)(sizeof(mods) / sizeof(mods[0])); i++) {
        if (GetAsyncKeyState(mods[i]) & 0x8000)
            send_key(mods[i], false);
    }
}

/* ---------------------------------------------------------------------------
 * select_backward
 *
 * Re-selects `n` characters to the left of the cursor after a paste by
 * batching all Shift+Left events into a single SendInput call.
 * Layout: [Shift-down, Left-down, Left-up] × n, then [Shift-up]
 * --------------------------------------------------------------------------- */
void select_backward(size_t n)
{
    if (n == 0) return;

    /* Layout: Shift↓, [Left↓ Left↑] × n, Shift↑
     * Total: 2 + n*2 events */
    size_t  nevents = 2 + n * 2;
    INPUT  *events  = (INPUT *)calloc(nevents, sizeof(INPUT));
    if (!events) return;

    size_t idx = 0;

    /* Shift down — held for the entire selection */
    events[idx].type       = INPUT_KEYBOARD;
    events[idx].ki.wVk     = VK_SHIFT;
    events[idx].ki.dwFlags = 0;
    idx++;

    /* Left × n while Shift is held.
     * VK_LEFT on the navigation cluster is an extended key — the flag is
     * required, otherwise the event may be interpreted as Numpad 4. */
    for (size_t i = 0; i < n; i++) {
        events[idx].type       = INPUT_KEYBOARD;
        events[idx].ki.wVk     = VK_LEFT;
        events[idx].ki.dwFlags = KEYEVENTF_EXTENDEDKEY;           /* Left down */
        idx++;

        events[idx].type       = INPUT_KEYBOARD;
        events[idx].ki.wVk     = VK_LEFT;
        events[idx].ki.dwFlags = KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP; /* Left up */
        idx++;
    }

    /* Shift up */
    events[idx].type       = INPUT_KEYBOARD;
    events[idx].ki.wVk     = VK_SHIFT;
    events[idx].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput((UINT)nevents, events, sizeof(INPUT));
    free(events);
}
