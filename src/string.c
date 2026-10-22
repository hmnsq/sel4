/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <string.h>

word_t strnlen(ध्रुव वर्ण *s, word_t maxlen)
{
    word_t len;
    अत्थाय (len = 0; len < maxlen && s[len]; len++);
    पटिनिवत्त len;
}

word_t strlcpy(वर्ण *dest, ध्रुव वर्ण *src, word_t size)
{
    word_t len;
    अत्थाय (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    पटिनिवत्त len;
}

word_t strlcat(वर्ण *dest, ध्रुव वर्ण *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    अत्थाय (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    सचे (len < size) {
        अत्थाय (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    पटिनिवत्त len;
}
