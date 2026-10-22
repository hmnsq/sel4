/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <linker.h>
#شامل <machine/io.h>
#شامل <plat/machine/pit.h>

/* PIT (i8253) registers */
#تعریف_کرو PIT_منوال 0x43
#تعریف_کرو PIT_CH0  0x40

/* Count frequency in Hz */
#تعریف_کرو PIT_HZ 1193182

BOOT_رمز خالی pit_init(خالی)
{
    uint16_t مقسوم_علیہ = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_منوال, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, مقسوم_علیہ & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, مقسوم_علیہ >> 8);   /* Set high byte of divisor */
}

BOOT_رمز خالی pit_wait_wraparound(خالی)
{
    uint16_t گنتی;
    uint16_t گنتی_old;

    out8(PIT_منوال, 0x00);
    گنتی = in8(PIT_CH0);
    گنتی |= (in8(PIT_CH0) << 8);
    گنتی_old = گنتی;

    جب (گنتی <= گنتی_old) {
        گنتی_old = گنتی;
        out8(PIT_منوال, 0x00);
        گنتی = in8(PIT_CH0);
        گنتی |= (in8(PIT_CH0) << 8);
    }
}
