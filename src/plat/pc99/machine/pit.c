/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <linker.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/pit.h>

/* PIT (i8253) registers */
#define PIT_MODE 0x43
#define PIT_CH0  0x40

/* Count frequency in Hz */
#define PIT_HZ 1193182

BOOT_CODE शून्य pit_init(शून्य)
{
    uint16_t भाजक = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, भाजक & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, भाजक >> 8);   /* Set high byte of divisor */
}

BOOT_CODE शून्य pit_wait_wraparound(शून्य)
{
    uint16_t मोजणी;
    uint16_t मोजणी_old;

    out8(PIT_MODE, 0x00);
    मोजणी = in8(PIT_CH0);
    मोजणी |= (in8(PIT_CH0) << 8);
    मोजणी_old = मोजणी;

    यावत् (मोजणी <= मोजणी_old) {
        मोजणी_old = मोजणी;
        out8(PIT_MODE, 0x00);
        मोजणी = in8(PIT_CH0);
        मोजणी |= (in8(PIT_CH0) << 8);
    }
}
