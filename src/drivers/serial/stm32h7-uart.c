/*
 * Copyright 2026, STMicroelectronics
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <util.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <plat/machine/devices_gen.h>

#define USART_ISR               0x1C
#define USART_TDR               0x28

#define USART_ISR_TXE           0x80U

#define UART_REG(x) ((চঞ্চল uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
শূন্য uart_drv_putchar(unsigned বর্ণ c)
{
    যাবত্ (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned বর্ণ uart_drv_getchar(শূন্য)
{
    প্রত্যাগম -1;
}
#endif /* CONFIG_DEBUG_BUILD */
