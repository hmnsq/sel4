/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <linker.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/pit.h>

/* PIT (i8253) registers */
#وصف_ڪريو PIT_گهڻائي 0x43
#وصف_ڪريو PIT_CH0  0x40

/* Count frequency in Hz */
#وصف_ڪريو PIT_HZ 1193182

BOOT_ڪوڊ فراغ pit_init(فراغ)
{
    uint16_t ورهائيندڙ = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_گهڻائي, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, ورهائيندڙ & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, ورهائيندڙ >> 8);   /* Set high byte of divisor */
}

BOOT_ڪوڊ فراغ pit_wait_wraparound(فراغ)
{
    uint16_t ڳڻپ;
    uint16_t ڳڻپ_old;

    out8(PIT_گهڻائي, 0x00);
    ڳڻپ = in8(PIT_CH0);
    ڳڻپ |= (in8(PIT_CH0) << 8);
    ڳڻپ_old = ڳڻپ;

    طالما (ڳڻپ <= ڳڻپ_old) {
        ڳڻپ_old = ڳڻپ;
        out8(PIT_گهڻائي, 0x00);
        ڳڻپ = in8(PIT_CH0);
        ڳڻپ |= (in8(PIT_CH0) << 8);
    }
}
