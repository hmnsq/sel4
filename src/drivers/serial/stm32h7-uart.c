/*
 * Copyright 2026, STMicroelectronics
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <stdint.h>
#অন্তর্ভাৱ <util.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <plat/machine/devices_gen.h>

#define USART_ISR               0x1C
#define USART_TDR               0x28

#define USART_ISR_TXE           0x80U

#define UART_REG(x) ((চঞ্চল uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
শূন্য uart_drv_putchar(unsigned ৱর্ণ c)
{
    যেতিয়ালৈ (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ৱর্ণ uart_drv_getchar(শূন্য)
{
    ঘূৰাই -1;
}
#endif /* CONFIG_DEBUG_BUILD */
