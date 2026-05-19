/*
 * switcher.h - High-level "switch layout" action
 *
 * do_switch() orchestrates: copy selection -> detect -> convert -> paste,
 * restoring the original clipboard contents when finished.
 */

#pragma once

/* Trigger a full layout-switch cycle on the currently selected text. */
void do_switch(void);
