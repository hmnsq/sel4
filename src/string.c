/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <assert.h>
#समावेश <string.h>

word_t strnlen(स्थिर अक्षर *s, word_t maxlen)
{
    word_t len;
    क्रम (len = 0; len < maxlen && s[len]; len++);
    वापस len;
}

word_t strlcpy(अक्षर *dest, स्थिर अक्षर *src, word_t आकार)
{
    word_t len;
    क्रम (len = 0; len + 1 < आकार && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    वापस len;
}

word_t strlcat(अक्षर *dest, स्थिर अक्षर *src, word_t आकार)
{
    word_t len;
    /* get to the end of dest */
    क्रम (len = 0; len < आकार && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    अगर (len < आकार) {
        क्रम (; len + 1 < आकार && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    वापस len;
}
