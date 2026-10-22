/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <plat/machine/devices_gen.h>

#నిర్వచించు URXD  0x00 /* UART Receiver Register */
#నిర్వచించు UTXD  0x40 /* UART Transmitter Register */
#నిర్వచించు UCR1  0x80 /* UART Control Register 1 */
#నిర్వచించు UCR2  0x84 /* UART Control Register 2 */
#నిర్వచించు UCR3  0x88 /* UART Control Register 3 */
#నిర్వచించు UCR4  0x8c /* UART Control Register 4 */
#నిర్వచించు UFCR  0x90 /* UART FIFO Control Register */
#నిర్వచించు USR1  0x94 /* UART Status Register 1 */
#నిర్వచించు USR2  0x98 /* UART Status Register 2 */
#నిర్వచించు UESC  0x9c /* UART Escape Character Register */
#నిర్వచించు UTIM  0xa0 /* UART Escape Timer Register */
#నిర్వచించు UBIR  0xa4 /* UART BRM Incremental Register */
#నిర్వచించు UBMR  0xa8 /* UART BRM Modulator Register */
#నిర్వచించు UBRC  0xac /* UART Baud Rate Counter Register */
#నిర్వచించు ONEMS 0xb0 /* UART One Millisecond Register */
#నిర్వచించు UTS   0xb4 /* UART Test Register */

#నిర్వచించు UART_SR1_TRDY         బిట్టు(13)
#నిర్వచించు UART_SR1_RRDY         బిట్టు(9)
#నిర్వచించు UART_SR2_TXFIFO_ఖాళీలు బిట్టు(14)
#నిర్వచించు UART_SR2_RXFIFO_RDR   బిట్టు(0)

#నిర్వచించు UART_REG(x) ((అస్థిర uint32_t *)(UART_PPTR + (x)))

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య uart_drv_putchar(గుర్తులేని అక్షరం c)
{
    అయితే (!(*UART_REG(USR2) & UART_SR2_TXFIFO_ఖాళీలు));
    *UART_REG(UTXD) = c;
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం uart_drv_getchar(శూన్య)
{
    అయితే (!(*UART_REG(USR2) & UART_SR2_RXFIFO_RDR));
    తిరిగి *UART_REG(URXD);
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
