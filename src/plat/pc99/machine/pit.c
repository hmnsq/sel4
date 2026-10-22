/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <linker.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/pit.h>

/* PIT (i8253) registers */
#define PIT_MODE 0x43
#define PIT_CH0  0x40

/* Count frequency in Hz */
#define PIT_HZ 1193182

BOOT_CODE ଶୂନ୍ୟ pit_init(ଶୂନ୍ୟ)
{
    uint16_t ଭାଜକ = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, ଭାଜକ & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, ଭାଜକ >> 8);   /* Set high byte of divisor */
}

BOOT_CODE ଶୂନ୍ୟ pit_wait_wraparound(ଶୂନ୍ୟ)
{
    uint16_t ଗଣନା_2;
    uint16_t ଗଣନା_old;

    out8(PIT_MODE, 0x00);
    ଗଣନା_2 = in8(PIT_CH0);
    ଗଣନା_2 |= (in8(PIT_CH0) << 8);
    ଗଣନା_old = ଗଣନା_2;

    ଯେତେବେଳେ (ଗଣନା_2 <= ଗଣନା_old) {
        ଗଣନା_old = ଗଣନା_2;
        out8(PIT_MODE, 0x00);
        ଗଣନା_2 = in8(PIT_CH0);
        ଗଣନା_2 |= (in8(PIT_CH0) << 8);
    }
}
