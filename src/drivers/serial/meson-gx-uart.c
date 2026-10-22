/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <plat/machine/devices_gen.h>

#నిర్వచించు UART_WFIFO  0x0
#నిర్వచించు UART_RFIFO  0x4
#నిర్వచించు UART_స్థితి 0xC

#నిర్వచించు UART_TX_నిండిన        బిట్టు(21)
#నిర్వచించు UART_RX_ఖాళీలు       బిట్టు(20)

#నిర్వచించు UART_REG(x) ((అస్థిర uint32_t *)(UART_PPTR + (x)))

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య uart_drv_putchar(గుర్తులేని అక్షరం c)
{
    అయితే ((*UART_REG(UART_స్థితి) & UART_TX_నిండిన));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం uart_drv_getchar(శూన్య)
{
    అయితే ((*UART_REG(UART_స్థితి) & UART_RX_ఖాళీలు));
    తిరిగి *UART_REG(UART_RFIFO);
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
