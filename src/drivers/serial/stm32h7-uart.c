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

#وصف_ڪريو USART_ISR               0x1C
#وصف_ڪريو USART_TDR               0x28

#وصف_ڪريو USART_ISR_TXE           0x80U

#وصف_ڪريو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشان حرف c)
{
    طالما (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف uart_drv_getchar(فراغ)
{
    أرجع -1;
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
