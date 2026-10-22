/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <linker.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/pit.h>

/* PIT (i8253) registers */
#تعریف_کړه PIT_ډېرپېښ 0x43
#تعریف_کړه PIT_CH0  0x40

/* Count frequency in Hz */
#تعریف_کړه PIT_HZ 1193182

BOOT_کوډ فراغ pit_init(فراغ)
{
    uint16_t وېشونکی = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_ډېرپېښ, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, وېشونکی & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, وېشونکی >> 8);   /* Set high byte of divisor */
}

BOOT_کوډ فراغ pit_wait_wraparound(فراغ)
{
    uint16_t شمېر;
    uint16_t شمېر_old;

    out8(PIT_ډېرپېښ, 0x00);
    شمېر = in8(PIT_CH0);
    شمېر |= (in8(PIT_CH0) << 8);
    شمېر_old = شمېر;

    طالما (شمېر <= شمېر_old) {
        شمېر_old = شمېر;
        out8(PIT_ډېرپېښ, 0x00);
        شمېر = in8(PIT_CH0);
        شمېر |= (in8(PIT_CH0) << 8);
    }
}
