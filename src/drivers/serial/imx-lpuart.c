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

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>
#شامل <plat/machine/devices_gen.h>

#تعریف_کرو STAT 0x14
#تعریف_کرو TRANSMIT 0x1c

#تعریف_کرو STAT_TDRE (1 << 23)

#تعریف_کرو UART_REG(x) ((متلون uint32_t *)(UART_PPTR + (x)))

#اگر defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
خالی uart_drv_putchar(غیرعلامتی حرف c)
{
    جب (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف uart_drv_getchar(خالی)
{
    واپس 0;
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
