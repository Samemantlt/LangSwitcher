/*
 * converter.h - EN <-> RU keyboard layout conversion
 *
 * convert_layout() replaces every character in-place:
 *   - EN key  ->  corresponding RU character
 *   - RU char ->  corresponding EN key
 * Both directions happen in a single pass; no pre-detection needed.
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
