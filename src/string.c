/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <string.h>

word_t strnlen(ধ্রুৱ ৱর্ণ *s, word_t maxlen)
{
    word_t len;
    বাবে (len = 0; len < maxlen && s[len]; len++);
    ঘূৰাই len;
}

word_t strlcpy(ৱর্ণ *dest, ধ্রুৱ ৱর্ণ *src, word_t size)
{
    word_t len;
    বাবে (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    ঘূৰাই len;
}

word_t strlcat(ৱর্ণ *dest, ধ্রুৱ ৱর্ণ *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    বাবে (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    যদি (len < size) {
        বাবে (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    ঘূৰাই len;
}
