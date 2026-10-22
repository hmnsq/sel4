/*
 * Copyright 2026, STMicroelectronics
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <plat/machine/devices_gen.h>

#నిర్వచించు USART_ISR               0x1C
#నిర్వచించు USART_TDR               0x28

#నిర్వచించు USART_ISR_TXE           0x80U

#నిర్వచించు UART_REG(x) ((అస్థిర uint32_t *)(UART_PPTR + (x)))

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య uart_drv_putchar(గుర్తులేని అక్షరం c)
{
    అయితే (!(*UART_REG(USART_ISR) & USART_ISR_TXE));
    *UART_REG(USART_TDR) = c;
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం uart_drv_getchar(శూన్య)
{
    తిరిగి -1;
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
