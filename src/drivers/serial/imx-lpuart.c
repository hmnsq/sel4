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

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <stdint.h>
#অন্তর্ভাৱ <util.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <plat/machine/devices_gen.h>

#define STAT 0x14
#define TRANSMIT 0x1c

#define STAT_TDRE (1 << 23)

#define UART_REG(x) ((চঞ্চল uint32_t *)(UART_PPTR + (x)))

#যদি defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
শূন্য uart_drv_putchar(unsigned ৱর্ণ c)
{
    যেতিয়ালৈ (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#endif

#ifdef CONFIG_DEBUG_BUILD
unsigned ৱর্ণ uart_drv_getchar(শূন্য)
{
    ঘূৰাই 0;
}
#endif /* CONFIG_DEBUG_BUILD */
