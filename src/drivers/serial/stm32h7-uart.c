/*
 * Copyright 2026, STMicroelectronics
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>
#شامل <plat/machine/devices_gen.h>

#define USART_ISR               0x1C
#define USART_TDR               0x28

#define USART_ISR_TXE           0x80U

#define UART_REG(x) ((فرار uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
تهی uart_drv_putchar(unsigned نویسه c)
{
    وقتی (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned نویسه uart_drv_getchar(تهی)
{
    بازگشت -1;
}
#endif /* CONFIG_DEBUG_BUILD */
