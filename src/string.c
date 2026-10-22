/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <assert.h>
#ضمن <string.h>

کلمه_t strnlen(ثابت حرف *s, کلمه_t maxlen)
{
    کلمه_t len;
    لكل (len = 0; len < maxlen && s[len]; len++);
    أرجع len;
}

کلمه_t strlcpy(حرف *dest, ثابت حرف *src, کلمه_t اندازه)
{
    کلمه_t len;
    لكل (len = 0; len + 1 < اندازه && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    أرجع len;
}

کلمه_t strlcat(حرف *dest, ثابت حرف *src, کلمه_t اندازه)
{
    کلمه_t len;
    /* get to the end of dest */
    لكل (len = 0; len < اندازه && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    إذا (len < اندازه) {
        لكل (; len + 1 < اندازه && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    أرجع len;
}
