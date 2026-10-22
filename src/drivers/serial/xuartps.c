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

#నిర్వచించు UART_CONTROL                 0x00
#నిర్వచించు UART_బహుళకం                    0x04
#నిర్వచించు UART_INTRPT_EN               0x08
#నిర్వచించు UART_INTRPT_DIS              0x0C
#నిర్వచించు UART_INTRPT_ముసుగు             0x10
#నిర్వచించు UART_CHNL_INT_STS            0x14
#నిర్వచించు UART_BAUD_రేటు_GEN           0x18
#నిర్వచించు UART_RCVR_TIMEOUT            0x1C
#నిర్వచించు UART_RCVR_FIFO_TRIGGER_LEVEL 0x20
#నిర్వచించు UART_MODEM_CTRL              0x24
#నిర్వచించు UART_MODEM_STS               0x28
#నిర్వచించు UART_CHANNEL_STS             0x2C
#నిర్వచించు UART_TX_RX_FIFO              0x30
#నిర్వచించు UART_BAUD_రేటు_DIVIDER       0x34
#నిర్వచించు UART_FLOW_DELAY              0x38
#నిర్వచించు UART_TX_FIFO_TRIGGER_LEVEL   0x44

#నిర్వచించు UART_INTRPT_ముసుగు_TXEMPTY     బిట్టు(3)
#నిర్వచించు UART_CHANNEL_STS_TXEMPTY     బిట్టు(3)

#నిర్వచించు UART_REG(x) ((అస్థిర uint32_t *)(UART_PPTR + (x)))

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య uart_drv_putchar(గుర్తులేని అక్షరం c)
{
    అయితే (!(*UART_REG(UART_CHANNEL_STS) & UART_CHANNEL_STS_TXEMPTY));
    *UART_REG(UART_TX_RX_FIFO) = c;
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం uart_drv_getchar(శూన్య)
{
    అయితే (!(*UART_REG(UART_CHANNEL_STS) & బిట్టు(UART_CHANNEL_STS_TXEMPTY)));
    తిరిగి *UART_REG(UART_TX_RX_FIFO);
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
