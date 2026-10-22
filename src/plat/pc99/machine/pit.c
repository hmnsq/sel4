/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/pit.h>

/* PIT (i8253) registers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIT_ਬਹੁਲਕ 0x43
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIT_CH0  0x40

/* Count frequency in Hz */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIT_HZ 1193182

BOOT_ਕੋਡ ਖਾਲੀ pit_init(ਖਾਲੀ)
{
    uint16_t ਭਾਜਕ = (PIT_HZ * PIT_WRAPAROUND_MS) / 1000;

    out8(PIT_ਬਹੁਲਕ, 0x34);          /* Set mode 2 and wait for divisor bytes */
    out8(PIT_CH0, ਭਾਜਕ & 0xff); /* Set low byte of divisor */
    out8(PIT_CH0, ਭਾਜਕ >> 8);   /* Set high byte of divisor */
}

BOOT_ਕੋਡ ਖਾਲੀ pit_wait_wraparound(ਖਾਲੀ)
{
    uint16_t ਗਿਣਤੀ_2;
    uint16_t ਗਿਣਤੀ_old;

    out8(PIT_ਬਹੁਲਕ, 0x00);
    ਗਿਣਤੀ_2 = in8(PIT_CH0);
    ਗਿਣਤੀ_2 |= (in8(PIT_CH0) << 8);
    ਗਿਣਤੀ_old = ਗਿਣਤੀ_2;

    ਜਦੋਂ (ਗਿਣਤੀ_2 <= ਗਿਣਤੀ_old) {
        ਗਿਣਤੀ_old = ਗਿਣਤੀ_2;
        out8(PIT_ਬਹੁਲਕ, 0x00);
        ਗਿਣਤੀ_2 = in8(PIT_CH0);
        ਗਿਣਤੀ_2 |= (in8(PIT_CH0) << 8);
    }
}
