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

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <plat/machine/devices_gen.h>

#define STAT 0x14
#define TRANSMIT 0x1c

#define STAT_TDRE (1 << 23)

#define UART_REG(x) ((நிலையற்ற uint32_t *)(UART_PPTR + (x)))

#எனில் defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
வெறுமை uart_drv_putchar(unsigned எழுத்து c)
{
    போது (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#endif

#ifdef CONFIG_DEBUG_BUILD
unsigned எழுத்து uart_drv_getchar(வெறுமை)
{
    திரும்பு 0;
}
#endif /* CONFIG_DEBUG_BUILD */
