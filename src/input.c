/*
 * input.c - Low-level keyboard input simulation implementation
 */

#include "input.h"

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
