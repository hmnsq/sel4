/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <string.h>

word_t strnlen(மாறிலி எழுத்து *s, word_t maxlen)
{
    word_t len;
    க்கு (len = 0; len < maxlen && s[len]; len++);
    திரும்பு len;
}

word_t strlcpy(எழுத்து *dest, மாறிலி எழுத்து *src, word_t size)
{
    word_t len;
    க்கு (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    திரும்பு len;
}

word_t strlcat(எழுத்து *dest, மாறிலி எழுத்து *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    க்கு (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    எனில் (len < size) {
        க்கு (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    திரும்பு len;
}
