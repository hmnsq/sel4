/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/devices_gen.h>

#وصف_ڪريو UART_CONTROL                 0x00
#وصف_ڪريو UART_گهڻائي                    0x04
#وصف_ڪريو UART_INTRPT_EN               0x08
#وصف_ڪريو UART_INTRPT_DIS              0x0C
#وصف_ڪريو UART_INTRPT_نقاب             0x10
#وصف_ڪريو UART_CHNL_INT_STS            0x14
#وصف_ڪريو UART_BAUD_شرح_GEN           0x18
#وصف_ڪريو UART_RCVR_TIMEOUT            0x1C
#وصف_ڪريو UART_RCVR_FIFO_TRIGGER_LEVEL 0x20
#وصف_ڪريو UART_MODEM_CTRL              0x24
#وصف_ڪريو UART_MODEM_STS               0x28
#وصف_ڪريو UART_CHANNEL_STS             0x2C
#وصف_ڪريو UART_TX_RX_FIFO              0x30
#وصف_ڪريو UART_BAUD_شرح_DIVIDER       0x34
#وصف_ڪريو UART_FLOW_DELAY              0x38
#وصف_ڪريو UART_TX_FIFO_TRIGGER_LEVEL   0x44

#وصف_ڪريو UART_INTRPT_نقاب_TXEMPTY     ٻٽو_انگ(3)
#وصف_ڪريو UART_CHANNEL_STS_TXEMPTY     ٻٽو_انگ(3)

#وصف_ڪريو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشان حرف c)
{
    طالما (!(*UART_REG(UART_CHANNEL_STS) & UART_CHANNEL_STS_TXEMPTY));
    *UART_REG(UART_TX_RX_FIFO) = c;
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف uart_drv_getchar(فراغ)
{
    طالما (!(*UART_REG(UART_CHANNEL_STS) & ٻٽو_انگ(UART_CHANNEL_STS_TXEMPTY)));
    أرجع *UART_REG(UART_TX_RX_FIFO);
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
