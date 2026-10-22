/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <linker.h>
#شامل <machine/io.h>
#شامل <plat/machine/pit.h>

/* PIT (i8253) registers */
#define PIT_MODE 0x43
#define PIT_CH0  0x40

/* Count frequency in Hz */
#define PIT_HZ 1193182

BOOT_CODE تهی pit_init(تهی)
{
    uint16_t مقسوم_علیه = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, مقسوم_علیه & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, مقسوم_علیه >> 8);   /* Set high byte of divisor */
}

BOOT_CODE تهی pit_wait_wraparound(تهی)
{
    uint16_t شمار;
    uint16_t شمار_old;

    out8(PIT_MODE, 0x00);
    شمار = in8(PIT_CH0);
    شمار |= (in8(PIT_CH0) << 8);
    شمار_old = شمار;

    وقتی (شمار <= شمار_old) {
        شمار_old = شمار;
        out8(PIT_MODE, 0x00);
        شمار = in8(PIT_CH0);
        شمار |= (in8(PIT_CH0) << 8);
    }
}
