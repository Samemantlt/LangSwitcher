/*
 * converter.h - EN <-> RU keyboard layout conversion and case toggling
 *
 * convert_layout() replaces every character in-place:
 *   - EN key  ->  corresponding RU character
 *   - RU char ->  corresponding EN key
 * Both directions happen in a single pass; no pre-detection needed.
 *
 * toggle_caps() inverts the case of every letter in-place:
 *   - Uppercase letter -> lowercase
 *   - Lowercase letter -> uppercase
 * Works for both Latin and Cyrillic letters.
 */

#pragma once

#include <stddef.h>
#include <wchar.h>

/*
 * Remap every character of `text` (length `len` wchar_t units) in-place.
 *
 * Each character is matched against the full EN and RU layout tables:
 *   EN key found  -> replaced with the paired RU character
 *   RU char found -> replaced with the paired EN key
 *
 * Characters with no mapping (spaces, digits, unpaired punctuation, etc.)
 * are left unchanged.
 */
void convert_layout(wchar_t *text, size_t len);

/*
 * Invert the case of every letter in `text` (length `len` wchar_t units)
 * in-place.  Uppercase becomes lowercase and vice-versa for both Latin
 * (A-Z / a-z) and Cyrillic (А-Я Ё / а-я ё) code points.
 * Non-letter characters are left unchanged.
 */
void toggle_caps(wchar_t *text, size_t len);
