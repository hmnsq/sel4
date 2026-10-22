/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <util.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <plat/machine/devices_gen.h>

#define UARTDR                    0x000
#define UARTFR                    0x018

#define PL011_UARTFR_TXFF         BIT(5)
#define PL011_UARTFR_RXFE         BIT(4)

#define UART_REG(x) ((অস্থির uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
শূন্য uart_drv_putchar(unsigned অক্ষর c)
{
    যখন ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned অক্ষর uart_drv_getchar(শূন্য)
{
    যখন ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    ফেরত *UART_REG(UARTDR);
}
#endif  /* CONFIG_DEBUG_BUILD */
