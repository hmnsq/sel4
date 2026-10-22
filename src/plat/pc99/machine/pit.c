/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <linker.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <plat/machine/pit.h>

/* PIT (i8253) registers */
#define PIT_MODE 0x43
#define PIT_CH0  0x40

/* Count frequency in Hz */
#define PIT_HZ 1193182

BOOT_CODE ശൂന്യം pit_init(ശൂന്യം)
{
    uint16_t ഹാരകം = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, ഹാരകം & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, ഹാരകം >> 8);   /* Set high byte of divisor */
}

BOOT_CODE ശൂന്യം pit_wait_wraparound(ശൂന്യം)
{
    uint16_t എണ്ണം;
    uint16_t എണ്ണം_old;

    out8(PIT_MODE, 0x00);
    എണ്ണം = in8(PIT_CH0);
    എണ്ണം |= (in8(PIT_CH0) << 8);
    എണ്ണം_old = എണ്ണം;

    ആകുമ്പോൾ (എണ്ണം <= എണ്ണം_old) {
        എണ്ണം_old = എണ്ണം;
        out8(PIT_MODE, 0x00);
        എണ്ണം = in8(PIT_CH0);
        എണ്ണം |= (in8(PIT_CH0) << 8);
    }
}
