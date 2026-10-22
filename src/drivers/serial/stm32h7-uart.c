/*
 * Copyright 2026, STMicroelectronics
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>

#define USART_ISR               0x1C
#define USART_TDR               0x28

#define USART_ISR_TXE           0x80U

#define UART_REG(x) ((ಚಞ್ಚಲ uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(unsigned ವರ್ಣ c)
{
    ಯಾವತ್ (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ವರ್ಣ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ -1;
}
#endif /* CONFIG_DEBUG_BUILD */
