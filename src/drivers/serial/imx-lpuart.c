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

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/devices_gen.h>

#وصف_ڪريو STAT 0x14
#وصف_ڪريو TRANSMIT 0x1c

#وصف_ڪريو STAT_TDRE (1 << 23)

#وصف_ڪريو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#إذا defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
فراغ uart_drv_putchar(بنانشان حرف c)
{
    طالما (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف uart_drv_getchar(فراغ)
{
    أرجع 0;
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
