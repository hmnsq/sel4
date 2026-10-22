/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <plat/machine/devices_gen.h>

#define UART_WFIFO  0x0
#define UART_RFIFO  0x4
#define UART_STATUS 0xC

#define UART_TX_FULL        BIT(21)
#define UART_RX_EMPTY       BIT(20)

#define UART_REG(x) ((അസ്ഥിര uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
ശൂന്യം uart_drv_putchar(unsigned അക്ഷരം c)
{
    ആകുമ്പോൾ ((*UART_REG(UART_STATUS) & UART_TX_FULL));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned അക്ഷരം uart_drv_getchar(ശൂന്യം)
{
    ആകുമ്പോൾ ((*UART_REG(UART_STATUS) & UART_RX_EMPTY));
    തിരികെ *UART_REG(UART_RFIFO);
}
#endif /* CONFIG_DEBUG_BUILD */
