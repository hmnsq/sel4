/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <plat/machine/devices_gen.h>

#నిర్వచించు UARTDR                    0x000
#నిర్వచించు UARTFR                    0x018

#నిర్వచించు PL011_UARTFR_TXFF         బిట్టు(5)
#నిర్వచించు PL011_UARTFR_RXFE         బిట్టు(4)

#నిర్వచించు UART_REG(x) ((అస్థిర uint32_t *)(UART_PPTR + (x)))

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య uart_drv_putchar(గుర్తులేని అక్షరం c)
{
    అయితే ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం uart_drv_getchar(శూన్య)
{
    అయితే ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    తిరిగి *UART_REG(UARTDR);
}
#అయితే_ముగింపు  /* CONFIG_DEBUG_BUILD */
