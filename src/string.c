/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <assert.h>
#include <string.h>

word_t strnlen(const char *s, word_t maxlen)
{
    word_t len;
    для (len = 0; len < maxlen && s[len]; len++);
    вернуть len;
}

word_t strlcpy(char *dest, const char *src, word_t size)
{
    word_t len;
    для (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    вернуть len;
}

word_t strlcat(char *dest, const char *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    для (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    если (len < size) {
        для (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    вернуть len;
}
