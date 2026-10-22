/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/devices_gen.h>

#define UARTDR                    0x000
#define UARTFR                    0x018

#define PL011_UARTFR_TXFF         BIT(5)
#define PL011_UARTFR_RXFE         BIT(4)

#define UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
فراغ uart_drv_putchar(unsigned حرف c)
{
    ܟܡܐ ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned حرف uart_drv_getchar(فراغ)
{
    ܟܡܐ ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    ܦܢܐ *UART_REG(UARTDR);
}
#endif  /* CONFIG_DEBUG_BUILD */
