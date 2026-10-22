/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <machine/io.h>

/* When DLAB=1, MU_IO is a baud rate register.
 * Otherwise, write to TX, read to RX */
#నిర్వచించు MU_IO       0x00
/* When DLAB=1, MU_IIR is a baud rate register.
 * Otherwise IRQ enable */
#నిర్వచించు MU_IIR      0x04
#నిర్వచించు MU_IER      0x08
#నిర్వచించు MU_LCR      0x0C
#నిర్వచించు MU_MCR      0x10
#నిర్వచించు MU_LSR      0x14
#నిర్వచించు MU_MSR      0x18
#నిర్వచించు MU_SCRATCH  0x1C
#నిర్వచించు MU_CNTL     0x20

/* This bit is set if the transmit FIFO can accept at least one byte.*/
#నిర్వచించు MU_LSR_TXEMPTY   బిట్టు(5)
/* This bit is set if the transmit FIFO is empty and the
 * transmitter is idle. (Finished shifting out the last bit). */
#నిర్వచించు MU_LSR_TXIDLE    బిట్టు(6)
#నిర్వచించు MU_LSR_RXOVERRUN బిట్టు(1)
#నిర్వచించు MU_LSR_DATAREADY బిట్టు(0)
#నిర్వచించు MU_LCR_DLAB      బిట్టు(7)
#నిర్వచించు MU_LCR_BREAK     బిట్టు(6)
#నిర్వచించు MU_LCR_DATASIZE  బిట్టు(0)

#నిర్వచించు UART_REG(x) ((అస్థిర uint32_t *)(UART_PPTR + (x)))

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య uart_drv_putchar(గుర్తులేని అక్షరం c)
{
    అయితే (!(*UART_REG(MU_LSR) & MU_LSR_TXIDLE));
    *UART_REG(MU_IO) = (c & 0xff);
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం uart_drv_getchar(శూన్య)
{
    అయితే (!(*UART_REG(MU_LSR) & MU_LSR_DATAREADY));
    తిరిగి *UART_REG(MU_IO);
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
