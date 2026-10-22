/*
 * Copyright 2026, STMicroelectronics
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/devices_gen.h>

#تعریف_کړه USART_ISR               0x1C
#تعریف_کړه USART_TDR               0x28

#تعریف_کړه USART_ISR_TXE           0x80U

#تعریف_کړه UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#که_تعریف_شوی CONFIG_PRINTING
فراغ uart_drv_putchar(بینښه حرف c)
{
    طالما (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف uart_drv_getchar(فراغ)
{
    أرجع -1;
}
#د_که_پای /* CONFIG_DEBUG_BUILD */
