/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <plat/machine/pit.h>

/* PIT (i8253) registers */
#define PIT_MODE 0x43
#define PIT_CH0  0x40

/* Count frequency in Hz */
#define PIT_HZ 1193182

BOOT_CODE வெறுமை pit_init(வெறுமை)
{
    uint16_t வகுஎண் = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_MODE, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, வகுஎண் & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, வகுஎண் >> 8);   /* Set high byte of divisor */
}

BOOT_CODE வெறுமை pit_wait_wraparound(வெறுமை)
{
    uint16_t மொத்தம்;
    uint16_t மொத்தம்_old;

    out8(PIT_MODE, 0x00);
    மொத்தம் = in8(PIT_CH0);
    மொத்தம் |= (in8(PIT_CH0) << 8);
    மொத்தம்_old = மொத்தம்;

    போது (மொத்தம் <= மொத்தம்_old) {
        மொத்தம்_old = மொத்தம்;
        out8(PIT_MODE, 0x00);
        மொத்தம் = in8(PIT_CH0);
        மொத்தம் |= (in8(PIT_CH0) << 8);
    }
}
