/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <string.h>

word_t strnlen(സ്ഥിരം അക്ഷരം *s, word_t maxlen)
{
    word_t len;
    വേണ്ടി (len = 0; len < maxlen && s[len]; len++);
    തിരികെ len;
}

word_t strlcpy(അക്ഷരം *dest, സ്ഥിരം അക്ഷരം *src, word_t size)
{
    word_t len;
    വേണ്ടി (len = 0; len + 1 < size && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    തിരികെ len;
}

word_t strlcat(അക്ഷരം *dest, സ്ഥിരം അക്ഷരം *src, word_t size)
{
    word_t len;
    /* get to the end of dest */
    വേണ്ടി (len = 0; len < size && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    എങ്കിൽ (len < size) {
        വേണ്ടി (; len + 1 < size && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    തിരികെ len;
}
