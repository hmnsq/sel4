/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <linker.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <plat/machine/pit.h>

/* PIT (i8253) registers */
#నిర్వచించు PIT_బహుళకం 0x43
#నిర్వచించు PIT_CH0  0x40

/* Count frequency in Hz */
#నిర్వచించు PIT_HZ 1193182

BOOT_సంకేతం శూన్య pit_init(శూన్య)
{
    uint16_t భాజకం = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_బహుళకం, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, భాజకం & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, భాజకం >> 8);   /* Set high byte of divisor */
}

BOOT_సంకేతం శూన్య pit_wait_wraparound(శూన్య)
{
    uint16_t లెక్క;
    uint16_t లెక్క_old;

    out8(PIT_బహుళకం, 0x00);
    లెక్క = in8(PIT_CH0);
    లెక్క |= (in8(PIT_CH0) << 8);
    లెక్క_old = లెక్క;

    అయితే (లెక్క <= లెక్క_old) {
        లెక్క_old = లెక్క;
        out8(PIT_బహుళకం, 0x00);
        లెక్క = in8(PIT_CH0);
        లెక్క |= (in8(PIT_CH0) << 8);
    }
}
