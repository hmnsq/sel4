/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <plat/machine/pit.h>

/* PIT (i8253) registers */
#define PIT_MODE 0x43
#define PIT_CH0  0x40

/* Count frequency in Hz */
#define PIT_HZ 1193182

BOOT_CODE શૂન્ય pit_init(શૂન્ય)
{
    uint16_t ભાજક = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, ભાજક & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, ભાજક >> 8);   /* Set high byte of divisor */
}

BOOT_CODE શૂન્ય pit_wait_wraparound(શૂન્ય)
{
    uint16_t ગણતરી;
    uint16_t ગણતરી_old;

    out8(PIT_MODE, 0x00);
    ગણતરી = in8(PIT_CH0);
    ગણતરી |= (in8(PIT_CH0) << 8);
    ગણતરી_old = ગણતરી;

    જ્યારે (ગણતરી <= ગણતરી_old) {
        ગણતરી_old = ગણતરી;
        out8(PIT_MODE, 0x00);
        ગણતરી = in8(PIT_CH0);
        ગણતરી |= (in8(PIT_CH0) << 8);
    }
}
