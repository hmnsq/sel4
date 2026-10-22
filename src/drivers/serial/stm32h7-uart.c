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

#define USART_ISR               0x1C
#define USART_TDR               0x28

#define USART_ISR_TXE           0x80U

#define UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
فراغ uart_drv_putchar(unsigned حرف c)
{
    ܟܡܐ (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned حرف uart_drv_getchar(فراغ)
{
    ܦܢܐ -1;
}
#endif /* CONFIG_DEBUG_BUILD */
