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

#అన్తర్భావ <config.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <plat/machine/devices_gen.h>

#నిర్వచించు STAT 0x14
#నిర్వచించు TRANSMIT 0x1c

#నిర్వచించు STAT_TDRE (1 << 23)

#నిర్వచించు UART_REG(x) ((అస్థిర uint32_t *)(UART_PPTR + (x)))

#ఒకవేళ defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
శూన్య uart_drv_putchar(గుర్తులేని అక్షరం c)
{
    అయితే (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం uart_drv_getchar(శూన్య)
{
    తిరిగి 0;
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
