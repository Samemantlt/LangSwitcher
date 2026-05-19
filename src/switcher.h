/*
 * switcher.h - High-level layout-switch and caps-toggle actions
 *
 * do_switch()      orchestrates: copy selection -> detect -> convert -> paste,
 *                  restoring the original clipboard contents when finished.
 * do_caps_switch() does the same but inverts the case of every letter instead
 *                  of remapping to the other keyboard layout.
 */

#pragma once

/* Trigger a full layout-switch cycle on the currently selected text. */
void do_switch(void);

/* Trigger a caps-toggle cycle on the currently selected text:
 * uppercase letters become lowercase and vice-versa (HELLO guy -> hello GUY). */
void do_caps_switch(void);
