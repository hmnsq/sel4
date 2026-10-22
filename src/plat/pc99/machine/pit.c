/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <linker.h>
#include <machine/io.h>
#include <plat/machine/pit.h>

/* PIT (i8253) registers */
#define PIT_MODE 0x43
#define PIT_CH0  0x40

/* Count frequency in Hz */
#define PIT_HZ 1193182

BOOT_CODE void pit_init(void)
{
    uint16_t पांगहाति = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, पांगहाति & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, पांगहाति >> 8);   /* Set high byte of divisor */
}

BOOT_CODE void pit_wait_wraparound(void)
{
    uint16_t बिलांग;
    uint16_t बिलांग_old;

    out8(PIT_MODE, 0x00);
    बिलांग = in8(PIT_CH0);
    बिलांग |= (in8(PIT_CH0) << 8);
    बिलांग_old = बिलांग;

    while (बिलांग <= बिलांग_old) {
        बिलांग_old = बिलांग;
        out8(PIT_MODE, 0x00);
        बिलांग = in8(PIT_CH0);
        बिलांग |= (in8(PIT_CH0) << 8);
    }
}
