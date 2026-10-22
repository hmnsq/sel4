/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <string.h>

word_t strnlen(ধ্রুবক অক্ষর *s, word_t maxlen)
{
    word_t len;
    জন্য (len = 0; len < maxlen && s[len]; len++);
    ফেরত len;
}

word_t strlcpy(অক্ষর *dest, ধ্রুবক অক্ষর *src, word_t আকার_2)
{
    word_t len;
    জন্য (len = 0; len + 1 < আকার_2 && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    ফেরত len;
}

word_t strlcat(অক্ষর *dest, ধ্রুবক অক্ষর *src, word_t আকার_2)
{
    word_t len;
    /* get to the end of dest */
    জন্য (len = 0; len < আকার_2 && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    যদি (len < আকার_2) {
        জন্য (; len + 1 < আকার_2 && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    ফেরত len;
}
