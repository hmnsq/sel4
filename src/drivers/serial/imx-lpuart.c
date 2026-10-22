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

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <plat/machine/devices_gen.h>

#define STAT 0x14
#define TRANSMIT 0x1c

#define STAT_TDRE (1 << 23)

#define UART_REG(x) ((അസ്ഥിര uint32_t *)(UART_PPTR + (x)))

#എങ്കിൽ defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
ശൂന്യം uart_drv_putchar(unsigned അക്ഷരം c)
{
    ആകുമ്പോൾ (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#endif

#ifdef CONFIG_DEBUG_BUILD
unsigned അക്ഷരം uart_drv_getchar(ശൂന്യം)
{
    തിരികെ 0;
}
#endif /* CONFIG_DEBUG_BUILD */
