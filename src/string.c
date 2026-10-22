/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <assert.h>
#شامل <string.h>

لفظ_t strnlen(ثابت حرف *s, لفظ_t maxlen)
{
    لفظ_t len;
    برائے (len = 0; len < maxlen && s[len]; len++);
    واپس len;
}

لفظ_t strlcpy(حرف *dest, ثابت حرف *src, لفظ_t سائز)
{
    لفظ_t len;
    برائے (len = 0; len + 1 < سائز && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    واپس len;
}

لفظ_t strlcat(حرف *dest, ثابت حرف *src, لفظ_t سائز)
{
    لفظ_t len;
    /* get to the end of dest */
    برائے (len = 0; len < سائز && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    اگر (len < سائز) {
        برائے (; len + 1 < سائز && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    واپس len;
}
