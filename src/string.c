/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <string.h>

word_t strnlen(અચલ અક્ષર *s, word_t maxlen)
{
    word_t len;
    માટે (len = 0; len < maxlen && s[len]; len++);
    પાછા len;
}

word_t strlcpy(અક્ષર *dest, અચલ અક્ષર *src, word_t size)
{
    word_t len;
    માટે (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    પાછા len;
}

word_t strlcat(અક્ષર *dest, અચલ અક્ષર *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    માટે (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    જો (len < size) {
        માટે (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    પાછા len;
}
