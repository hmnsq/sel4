/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <plat/machine/devices_gen.h>

#define UARTDR                    0x000
#define UARTFR                    0x018

#define PL011_UARTFR_TXFF         BIT(5)
#define PL011_UARTFR_RXFE         BIT(4)

#define UART_REG(x) ((അസ്ഥിര uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
ശൂന്യം uart_drv_putchar(unsigned അക്ഷരം c)
{
    ആകുമ്പോൾ ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned അക്ഷരം uart_drv_getchar(ശൂന്യം)
{
    ആകുമ്പോൾ ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    തിരികെ *UART_REG(UARTDR);
}
#endif  /* CONFIG_DEBUG_BUILD */
