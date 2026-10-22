/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <machine/io.h>

#నిర్వచించు ULCON       0x0000 /* line control */
#నిర్వచించు UCON        0x0004 /* control */
#నిర్వచించు UFCON       0x0008 /* fifo control */
#నిర్వచించు UMCON       0x000C /* modem control */
#నిర్వచించు UTRSTAT     0x0010 /* TX/RX status */
#నిర్వచించు UERSTAT     0x0014 /* RX error status */
#నిర్వచించు UFSTAT      0x0018 /* FIFO status */
#నిర్వచించు UMSTAT      0x001C /* modem status */
#నిర్వచించు UTXH        0x0020 /* TX buffer */
#నిర్వచించు URXH        0x0024 /* RX buffer */
#నిర్వచించు UBRDIV      0x0028 /* baud rate divisor */
#నిర్వచించు UFRACVAL    0x002C /* divisor fractional value */
#నిర్వచించు UINTP       0x0030 /* interrupt pending */
#నిర్వచించు UINTSP      0x0034 /* interrupt source pending */
#నిర్వచించు UINTM       0x0038 /* interrupt mask */

/* UTRSTAT */
#నిర్వచించు TX_ఖాళీలు        బిట్టు(2)
#నిర్వచించు TXBUF_ఖాళీలు     బిట్టు(1)
#నిర్వచించు RXBUF_READY     బిట్టు(0)

#నిర్వచించు UART_REG(X) ((అస్థిర uint32_t *)(UART_PPTR + (X)))

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య uart_drv_putchar(గుర్తులేని అక్షరం c)
{
    అయితే ((*UART_REG(UTRSTAT) & TXBUF_ఖాళీలు) == 0);
    *UART_REG(UTXH) = (c & 0xff);
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం uart_drv_getchar(శూన్య)
{
    ఒకవేళ ((*UART_REG(UTRSTAT) & RXBUF_READY)) {
        తిరిగి (గుర్తులేని అక్షరం) * UART_REG(URXH);
    } లేకపోతే {
        తిరిగి -1;
    }
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
