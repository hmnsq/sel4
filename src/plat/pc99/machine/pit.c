/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <linker.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/pit.h>

/* PIT (i8253) registers */
#تعریف_کن PIT_نما 0x43
#تعریف_کن PIT_CH0  0x40

/* Count frequency in Hz */
#تعریف_کن PIT_HZ 1193182

BOOT_کد فراغ pit_init(فراغ)
{
    uint16_t مقسوم_علیه = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_نما, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, مقسوم_علیه & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, مقسوم_علیه >> 8);   /* Set high byte of divisor */
}

BOOT_کد فراغ pit_wait_wraparound(فراغ)
{
    uint16_t شمار;
    uint16_t شمار_old;

    out8(PIT_نما, 0x00);
    شمار = in8(PIT_CH0);
    شمار |= (in8(PIT_CH0) << 8);
    شمار_old = شمار;

    طالما (شمار <= شمار_old) {
        شمار_old = شمار;
        out8(PIT_نما, 0x00);
        شمار = in8(PIT_CH0);
        شمار |= (in8(PIT_CH0) << 8);
    }
}
