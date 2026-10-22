/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <machine/io.h>

#నిర్వచించు USR                   0x08
#నిర్వచించు UTF                   0x70
#నిర్వచించు UNTX                  0x40

#నిర్వచించు USR_RXRDY             బిట్టు(0)
#నిర్వచించు USR_RXFUL             బిట్టు(1)
#నిర్వచించు USR_TXRDY             బిట్టు(2)
#నిర్వచించు USR_TXEMP             బిట్టు(3)

#నిర్వచించు UART_REG(X) ((అస్థిర uint32_t *)(UART_PPTR + (X)))

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య uart_drv_putchar(గుర్తులేని అక్షరం c)
{
    అయితే ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం uart_drv_getchar(శూన్య)
{
    అయితే ((*UART_REG(USR) & USR_RXRDY) == 0);

    తిరిగి *UART_REG(UTF) & 0xff;
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
