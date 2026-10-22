/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <assert.h>
#include <string.h>

word_t strnlen(const var_na *s, word_t maxlen)
{
    word_t len;
    for (len = 0; len < maxlen && s[len]; len++);
    ᬯᬮᬶ len;
}

word_t strlcpy(var_na *dest, const var_na *src, word_t size)
{
    word_t len;
    for (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    ᬯᬮᬶ len;
}

word_t strlcat(var_na *dest, const var_na *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    for (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    ᬬᬾᬦ᭄ (len < size) {
        for (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    ᬯᬮᬶ len;
}
