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

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/devices_gen.h>

#define STAT 0x14
#define TRANSMIT 0x1c

#define STAT_TDRE (1 << 23)

#define UART_REG(x) ((ଅସ୍ଥିର uint32_t *)(UART_PPTR + (x)))

#ଯଦି defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
ଶୂନ୍ୟ uart_drv_putchar(unsigned ଅକ୍ଷର c)
{
    ଯେତେବେଳେ (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#endif

#ifdef CONFIG_DEBUG_BUILD
unsigned ଅକ୍ଷର uart_drv_getchar(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ 0;
}
#endif /* CONFIG_DEBUG_BUILD */
