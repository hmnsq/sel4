/*
 * Copyright 2026, STMicroelectronics
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <plat/machine/devices_gen.h>

#define USART_ISR               0x1C
#define USART_TDR               0x28

#define USART_ISR_TXE           0x80U

#define UART_REG(x) ((અસ્થિર uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
શૂન્ય uart_drv_putchar(unsigned અક્ષર c)
{
    જ્યારે (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned અક્ષર uart_drv_getchar(શૂન્ય)
{
    પાછા -1;
}
#endif /* CONFIG_DEBUG_BUILD */
