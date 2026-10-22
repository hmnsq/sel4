/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <string.h>

word_t strnlen(स्थिरांक अक्षर *s, word_t maxlen)
{
    word_t len;
    साठी (len = 0; len < maxlen && s[len]; len++);
    परत len;
}

word_t strlcpy(अक्षर *dest, स्थिरांक अक्षर *src, word_t size)
{
    word_t len;
    साठी (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    परत len;
}

word_t strlcat(अक्षर *dest, स्थिरांक अक्षर *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    साठी (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    जर (len < size) {
        साठी (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    परत len;
}
