/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/pit.h>

/* PIT (i8253) registers */
#ವ್ಯಾಖ್ಯಾನಿಸು PIT_ಬಹುಲಕ 0x43
#ವ್ಯಾಖ್ಯಾನಿಸು PIT_CH0  0x40

/* Count frequency in Hz */
#ವ್ಯಾಖ್ಯಾನಿಸು PIT_HZ 1193182

BOOT_ಸಂಕೇತ ಶೂನ್ಯ pit_init(ಶೂನ್ಯ)
{
    uint16_t ಭಾಜಕ = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_ಬಹುಲಕ, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, ಭಾಜಕ & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, ಭಾಜಕ >> 8);   /* Set high byte of divisor */
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ pit_wait_wraparound(ಶೂನ್ಯ)
{
    uint16_t ಎಣಿಕೆ_2;
    uint16_t ಎಣಿಕೆ_old;

    out8(PIT_ಬಹುಲಕ, 0x00);
    ಎಣಿಕೆ_2 = in8(PIT_CH0);
    ಎಣಿಕೆ_2 |= (in8(PIT_CH0) << 8);
    ಎಣಿಕೆ_old = ಎಣಿಕೆ_2;

    ಆಗ (ಎಣಿಕೆ_2 <= ಎಣಿಕೆ_old) {
        ಎಣಿಕೆ_old = ಎಣಿಕೆ_2;
        out8(PIT_ಬಹುಲಕ, 0x00);
        ಎಣಿಕೆ_2 = in8(PIT_CH0);
        ಎಣಿಕೆ_2 |= (in8(PIT_CH0) << 8);
    }
}
