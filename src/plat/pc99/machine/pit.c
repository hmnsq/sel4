/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <linker.h>
#समावेश <machine/io.h>
#समावेश <plat/machine/pit.h>

/* PIT (i8253) registers */
#घोषणा PIT_MODE 0x43
#घोषणा PIT_CH0  0x40

/* Count frequency in Hz */
#घोषणा PIT_HZ 1193182

BOOT_CODE व्योम pit_init(व्योम)
{
    uint16_t भाजक = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, भाजक & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, भाजक >> 8);   /* Set high byte of divisor */
}

BOOT_CODE व्योम pit_wait_wraparound(व्योम)
{
    uint16_t गिनती;
    uint16_t गिनती_old;

    out8(PIT_MODE, 0x00);
    गिनती = in8(PIT_CH0);
    गिनती |= (in8(PIT_CH0) << 8);
    गिनती_old = गिनती;

    जबतक (गिनती <= गिनती_old) {
        गिनती_old = गिनती;
        out8(PIT_MODE, 0x00);
        गिनती = in8(PIT_CH0);
        गिनती |= (in8(PIT_CH0) << 8);
    }
}
