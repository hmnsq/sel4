/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <assert.h>
#شامل <string.h>

word_t strnlen(ثابت نویسه *s, word_t maxlen)
{
    word_t len;
    برای (len = 0; len < maxlen && s[len]; len++);
    بازگشت len;
}

word_t strlcpy(نویسه *dest, ثابت نویسه *src, word_t size)
{
    word_t len;
    برای (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    بازگشت len;
}

word_t strlcat(نویسه *dest, ثابت نویسه *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    برای (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    اگر (len < size) {
        برای (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    بازگشت len;
}
