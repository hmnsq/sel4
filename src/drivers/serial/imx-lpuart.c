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

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>

#define STAT 0x14
#define TRANSMIT 0x1c

#define STAT_TDRE (1 << 23)

#define UART_REG(x) ((ಚಞ್ಚಲ uint32_t *)(UART_PPTR + (x)))

#ಯದಿ defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
ಶೂನ್ಯ uart_drv_putchar(unsigned ವರ್ಣ c)
{
    ಯಾವತ್ (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#endif

#ifdef CONFIG_DEBUG_BUILD
unsigned ವರ್ಣ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ 0;
}
#endif /* CONFIG_DEBUG_BUILD */
