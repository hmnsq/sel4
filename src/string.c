/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <assert.h>
#ضمن <string.h>

word_t strnlen(ثابت حرف *s, word_t maxlen)
{
    word_t len;
    لكل (len = 0; len < maxlen && s[len]; len++);
    أرجع len;
}

word_t strlcpy(حرف *dest, ثابت حرف *src, word_t size)
{
    word_t len;
    لكل (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    أرجع len;
}

word_t strlcat(حرف *dest, ثابت حرف *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    لكل (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    𐤀𐤌 (len < size) {
        لكل (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    أرجع len;
}
