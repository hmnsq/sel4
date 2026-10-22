/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

word_t strnlen(ଅଚଳ ଅକ୍ଷର *s, word_t maxlen)
{
    word_t len;
    ପାଇଁ (len = 0; len < maxlen && s[len]; len++);
    ଫେରନ୍ତୁ len;
}

word_t strlcpy(ଅକ୍ଷର *dest, ଅଚଳ ଅକ୍ଷର *src, word_t size)
{
    word_t len;
    ପାଇଁ (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    ଫେରନ୍ତୁ len;
}

word_t strlcat(ଅକ୍ଷର *dest, ଅଚଳ ଅକ୍ଷର *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    ପାଇଁ (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    ଯଦି (len < size) {
        ପାଇଁ (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    ଫେରନ୍ତୁ len;
}
