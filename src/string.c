/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

ਸ਼ਬਦ_t strnlen(ਅਚਲ ਅੱਖਰ *s, ਸ਼ਬਦ_t maxlen)
{
    ਸ਼ਬਦ_t len;
    ਲਈ (len = 0; len < maxlen && s[len]; len++);
    ਵਾਪਸ len;
}

ਸ਼ਬਦ_t strlcpy(ਅੱਖਰ *dest, ਅਚਲ ਅੱਖਰ *src, ਸ਼ਬਦ_t ਮਾਪ)
{
    ਸ਼ਬਦ_t len;
    ਲਈ (len = 0; len + 1 < ਮਾਪ && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    ਵਾਪਸ len;
}

ਸ਼ਬਦ_t strlcat(ਅੱਖਰ *dest, ਅਚਲ ਅੱਖਰ *src, ਸ਼ਬਦ_t ਮਾਪ)
{
    ਸ਼ਬਦ_t len;
    /* get to the end of dest */
    ਲਈ (len = 0; len < ਮਾਪ && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    ਜੇ (len < ਮਾਪ) {
        ਲਈ (; len + 1 < ਮਾਪ && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    ਵਾਪਸ len;
}
