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

#تعریف_کړه UART_CONTROL                 0x00
#تعریف_کړه UART_ډېرپېښ                    0x04
#تعریف_کړه UART_INTRPT_EN               0x08
#تعریف_کړه UART_INTRPT_DIS              0x0C
#تعریف_کړه UART_INTRPT_ماسک             0x10
#تعریف_کړه UART_CHNL_INT_STS            0x14
#تعریف_کړه UART_BAUD_شرح_GEN           0x18
#تعریف_کړه UART_RCVR_TIMEOUT            0x1C
#تعریف_کړه UART_RCVR_FIFO_TRIGGER_LEVEL 0x20
#تعریف_کړه UART_MODEM_CTRL              0x24
#تعریف_کړه UART_MODEM_STS               0x28
#تعریف_کړه UART_CHANNEL_STS             0x2C
#تعریف_کړه UART_TX_RX_FIFO              0x30
#تعریف_کړه UART_BAUD_شرح_DIVIDER       0x34
#تعریف_کړه UART_FLOW_DELAY              0x38
#تعریف_کړه UART_TX_FIFO_TRIGGER_LEVEL   0x44

#تعریف_کړه UART_INTRPT_ماسک_TXEMPTY     بټ(3)
#تعریف_کړه UART_CHANNEL_STS_TXEMPTY     بټ(3)

#تعریف_کړه UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#که_تعریف_شوی CONFIG_PRINTING
فراغ uart_drv_putchar(بینښه حرف c)
{
    طالما (!(*UART_REG(UART_CHANNEL_STS) & UART_CHANNEL_STS_TXEMPTY));
    *UART_REG(UART_TX_RX_FIFO) = c;
}
#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف uart_drv_getchar(فراغ)
{
    طالما (!(*UART_REG(UART_CHANNEL_STS) & بټ(UART_CHANNEL_STS_TXEMPTY)));
    أرجع *UART_REG(UART_TX_RX_FIFO);
}
#د_که_پای /* CONFIG_DEBUG_BUILD */
