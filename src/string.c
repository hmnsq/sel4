/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <assert.h>
#ضمن <string.h>

لفظ_t strnlen(ثابت حرف *s, لفظ_t maxlen)
{
    لفظ_t len;
    لكل (len = 0; len < maxlen && s[len]; len++);
    أرجع len;
}

لفظ_t strlcpy(حرف *dest, ثابت حرف *src, لفظ_t سائيز)
{
    لفظ_t len;
    لكل (len = 0; len + 1 < سائيز && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    أرجع len;
}

لفظ_t strlcat(حرف *dest, ثابت حرف *src, لفظ_t سائيز)
{
    لفظ_t len;
    /* get to the end of dest */
    لكل (len = 0; len < سائيز && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    إذا (len < سائيز) {
        لكل (; len + 1 < سائيز && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    أرجع len;
}
