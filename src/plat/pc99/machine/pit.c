/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <linker.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/pit.h>

/* PIT (i8253) registers */
#define PIT_MODE 0x43
#define PIT_CH0  0x40

/* Count frequency in Hz */
#define PIT_HZ 1193182

BOOT_CODE فراغ pit_init(فراغ)
{
    uint16_t بۆلگۈچ = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, بۆلگۈچ & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, بۆلگۈچ >> 8);   /* Set high byte of divisor */
}

BOOT_CODE فراغ pit_wait_wraparound(فراغ)
{
    uint16_t سانى;
    uint16_t سانى_old;

    out8(PIT_MODE, 0x00);
    سانى = in8(PIT_CH0);
    سانى |= (in8(PIT_CH0) << 8);
    سانى_old = سانى;

    طالما (سانى <= سانى_old) {
        سانى_old = سانى;
        out8(PIT_MODE, 0x00);
        سانى = in8(PIT_CH0);
        سانى |= (in8(PIT_CH0) << 8);
    }
}
