/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/devices_gen.h>

#define UART_WFIFO  0x0
#define UART_RFIFO  0x4
#define UART_STATUS 0xC

#define UART_TX_FULL        BIT(21)
#define UART_RX_EMPTY       BIT(20)

#define UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
فراغ uart_drv_putchar(unsigned حرف c)
{
    כלעוד ((*UART_REG(UART_STATUS) & UART_TX_FULL));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned حرف uart_drv_getchar(فراغ)
{
    כלעוד ((*UART_REG(UART_STATUS) & UART_RX_EMPTY));
    החזר *UART_REG(UART_RFIFO);
}
#endif /* CONFIG_DEBUG_BUILD */
