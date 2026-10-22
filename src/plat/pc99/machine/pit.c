/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <linker.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <plat/machine/pit.h>

/* PIT (i8253) registers */
#define PIT_MODE 0x43
#define PIT_CH0  0x40

/* Count frequency in Hz */
#define PIT_HZ 1193182

BOOT_CODE শূন্য pit_init(শূন্য)
{
    uint16_t ভাজক = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, ভাজক & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, ভাজক >> 8);   /* Set high byte of divisor */
}

BOOT_CODE শূন্য pit_wait_wraparound(শূন্য)
{
    uint16_t গুনতি;
    uint16_t গুনতি_old;

    out8(PIT_MODE, 0x00);
    গুনতি = in8(PIT_CH0);
    গুনতি |= (in8(PIT_CH0) << 8);
    গুনতি_old = গুনতি;

    যাবত্ (গুনতি <= গুনতি_old) {
        গুনতি_old = গুনতি;
        out8(PIT_MODE, 0x00);
        গুনতি = in8(PIT_CH0);
        গুনতি |= (in8(PIT_CH0) << 8);
    }
}
