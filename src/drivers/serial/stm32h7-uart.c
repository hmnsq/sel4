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

#تعریف_کن USART_ISR               0x1C
#تعریف_کن USART_TDR               0x28

#تعریف_کن USART_ISR_TXE           0x80U

#تعریف_کن UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شده CONFIG_PRINTING
فراغ uart_drv_putchar(بیعلامت حرف c)
{
    طالما (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
بیعلامت حرف uart_drv_getchar(فراغ)
{
    أرجع -1;
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */
