/*
 * input.h - Low-level keyboard input simulation helpers
 *
 * Uses SendInput() to synthesise key events without affecting the
 * system's current keyboard state.
 */

#pragma once

#include <windows.h>
#include <stdbool.h>

/* Press (down=true) or release (down=false) a virtual key. */
void send_key(WORD vk, bool down);

/* Send Ctrl+<vk> as a complete press-and-release sequence. */
void send_ctrl(WORD vk);

/*
 * Release any modifier keys (Shift, Ctrl, Alt, Win) that are currently
 * physically held. Call this before synthesizing Ctrl+C / Ctrl+V so that
 * a held Shift does not turn them into Ctrl+Shift+C / Ctrl+Shift+V.
 */
void release_modifiers(void);

/*
 * Re-select the just-pasted text by sending Shift+Left n times as a single
 * batched SendInput call.  Call this after Ctrl+V to restore the selection
 * that was lost during the copy-convert-paste cycle.
 * If n is 0 the function is a no-op.
 */
void select_backward(size_t n);
