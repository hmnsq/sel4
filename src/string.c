/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <string.h>

ಶಬ್ದ_t strnlen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಶಬ್ದ_t maxlen)
{
    ಶಬ್ದ_t len;
    ಗಾಗಿ (len = 0; len < maxlen && s[len]; len++);
    ಹಿಂತಿರುಗಿ len;
}

ಶಬ್ದ_t strlcpy(ಅಕ್ಷರ *dest, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ಶಬ್ದ_t ಗಾತ್ರ_2)
{
    ಶಬ್ದ_t len;
    ಗಾಗಿ (len = 0; len + 1 < ಗಾತ್ರ_2 && src[len]; len++) {
        dest[len] = src[len];
    }
    dest[len] = '\0';
    ಹಿಂತಿರುಗಿ len;
}

ಶಬ್ದ_t strlcat(ಅಕ್ಷರ *dest, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ಶಬ್ದ_t ಗಾತ್ರ_2)
{
    ಶಬ್ದ_t len;
    /* get to the end of dest */
    ಗಾಗಿ (len = 0; len < ಗಾತ್ರ_2 && dest[len]; len++);
    /* check that dest was at least 'size' length to prevent inserting
     * a null byte when we shouldn't */
    ಆದರೆ (len < ಗಾತ್ರ_2) {
        ಗಾಗಿ (; len + 1 < ಗಾತ್ರ_2 && *src; len++, src++) {
            dest[len] = *src;
        }
        dest[len] = '\0';
    }
    ಹಿಂತಿರುಗಿ len;
}
