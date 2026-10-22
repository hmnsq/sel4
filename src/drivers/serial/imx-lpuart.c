/*
 * Copyright 2021, Breakaway Consulting Pty. Ltd.
 *
 * A simple output only UART driver for the NXP i.MX Low Power UART.
 *
 * Technical Reference:
 *   i.MX 8DualX/8DualXPlus/8QuadXPlus Applications Processor Reference Manual
 *   Revision 0 (IMX8DQXPRM.pdf)
 *   Chapter 16.13 (page 7908)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices_gen.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STAT 0x14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRANSMIT 0x1c

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STAT_TDRE (1 << 23)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG(x) ((ਅਸਥਿਰ uint32_t *)(UART_PPTR + (x)))

#ਜੇ defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
ਖਾਲੀ uart_drv_putchar(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    ਜਦੋਂ (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ uart_drv_getchar(ਖਾਲੀ)
{
    ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
