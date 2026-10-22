/*
 * Copyright 2026, STMicroelectronics
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <plat/machine/devices_gen.h>

#define USART_ISR               0x1C
#define USART_TDR               0x28

#define USART_ISR_TXE           0x80U

#define UART_REG(x) ((അസ്ഥിര uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
ശൂന്യം uart_drv_putchar(unsigned അക്ഷരം c)
{
    ആകുമ്പോൾ (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned അക്ഷരം uart_drv_getchar(ശൂന്യം)
{
    തിരികെ -1;
}
#endif /* CONFIG_DEBUG_BUILD */
