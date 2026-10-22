/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <string.h>

మాట_t strnlen(స్థిరరాశి అక్షరం *s, మాట_t maxlen)
{
    మాట_t len;
    కొరకు (len = 0; len < maxlen && s[len]; len++);
    తిరిగి len;
}

మాట_t strlcpy(అక్షరం *dest, స్థిరరాశి అక్షరం *src, మాట_t కొలత)
{
    మాట_t len;
    కొరకు (len = 0; len + 1 < కొలత && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    తిరిగి len;
}

మాట_t strlcat(అక్షరం *dest, స్థిరరాశి అక్షరం *src, మాట_t కొలత)
{
    మాట_t len;
    /* get to the end of dest */
    కొరకు (len = 0; len < కొలత && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    ఒకవేళ (len < కొలత) {
        కొరకు (; len + 1 < కొలత && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    తిరిగి len;
}
