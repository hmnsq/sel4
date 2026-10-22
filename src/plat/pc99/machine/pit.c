/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/pit.h>

/* PIT (i8253) registers */
#define PIT_MODE 0x43
#define PIT_CH0  0x40

/* Count frequency in Hz */
#define PIT_HZ 1193182

BOOT_CODE ಶೂನ್ಯ pit_init(ಶೂನ್ಯ)
{
    uint16_t ಭಾಜಕ = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, ಭಾಜಕ & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, ಭಾಜಕ >> 8);   /* Set high byte of divisor */
}

BOOT_CODE ಶೂನ್ಯ pit_wait_wraparound(ಶೂನ್ಯ)
{
    uint16_t ಎಣಿಕೆ;
    uint16_t ಎಣಿಕೆ_old;

    out8(PIT_MODE, 0x00);
    ಎಣಿಕೆ = in8(PIT_CH0);
    ಎಣಿಕೆ |= (in8(PIT_CH0) << 8);
    ಎಣಿಕೆ_old = ಎಣಿಕೆ;

    ಯಾವತ್ (ಎಣಿಕೆ <= ಎಣಿಕೆ_old) {
        ಎಣಿಕೆ_old = ಎಣಿಕೆ;
        out8(PIT_MODE, 0x00);
        ಎಣಿಕೆ = in8(PIT_CH0);
        ಎಣಿಕೆ |= (in8(PIT_CH0) << 8);
    }
}
