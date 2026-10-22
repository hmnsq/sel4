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

#تعریف_کرو USART_ISR               0x1C
#تعریف_کرو USART_TDR               0x28

#تعریف_کرو USART_ISR_TXE           0x80U

#تعریف_کرو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جے_تعریف_شدہ CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشانی حرف c)
{
    طالما (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#جے_انت /* CONFIG_PRINTING */

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
بنانشانی حرف uart_drv_getchar(فراغ)
{
    أرجع -1;
}
#جے_انت /* CONFIG_DEBUG_BUILD */
