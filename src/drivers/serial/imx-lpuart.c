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

#define STAT 0x14
#define TRANSMIT 0x1c

#define STAT_TDRE (1 << 23)

#define UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#ܐܢ defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
فراغ uart_drv_putchar(unsigned حرف c)
{
    ܟܡܐ (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#endif

#ifdef CONFIG_DEBUG_BUILD
unsigned حرف uart_drv_getchar(فراغ)
{
    ܦܢܐ 0;
}
#endif /* CONFIG_DEBUG_BUILD */
