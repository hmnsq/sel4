/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <string.h>

word_t strnlen(ধ্রুব বর্ণ *s, word_t maxlen)
{
    word_t len;
    প্রতি (len = 0; len < maxlen && s[len]; len++);
    প্রত্যাগম len;
}

word_t strlcpy(বর্ণ *dest, ধ্রুব বর্ণ *src, word_t size)
{
    word_t len;
    প্রতি (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    প্রত্যাগম len;
}

word_t strlcat(বর্ণ *dest, ধ্রুব বর্ণ *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    প্রতি (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    যদি (len < size) {
        প্রতি (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    প্রত্যাগম len;
}
