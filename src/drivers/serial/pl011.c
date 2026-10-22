/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>
#شامل <plat/machine/devices_gen.h>

#define UARTDR                    0x000
#define UARTFR                    0x018

#define PL011_UARTFR_TXFF         BIT(5)
#define PL011_UARTFR_RXFE         BIT(4)

#define UART_REG(x) ((فرار uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
تهی uart_drv_putchar(unsigned نویسه c)
{
    وقتی ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned نویسه uart_drv_getchar(تهی)
{
    وقتی ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    بازگشت *UART_REG(UARTDR);
}
#endif  /* CONFIG_DEBUG_BUILD */
