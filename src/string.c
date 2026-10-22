/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <string.h>

word_t strnlen(ಧ್ರುವ ವರ್ಣ *s, word_t maxlen)
{
    word_t len;
    ಪ್ರತಿ (len = 0; len < maxlen && s[len]; len++);
    ಪ್ರತ್ಯಾಗಮ len;
}

word_t strlcpy(ವರ್ಣ *dest, ಧ್ರುವ ವರ್ಣ *src, word_t size)
{
    word_t len;
    ಪ್ರತಿ (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    ಪ್ರತ್ಯಾಗಮ len;
}

word_t strlcat(ವರ್ಣ *dest, ಧ್ರುವ ವರ್ಣ *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    ಪ್ರತಿ (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    ಯದಿ (len < size) {
        ಪ್ರತಿ (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    ಪ್ರತ್ಯಾಗಮ len;
}
