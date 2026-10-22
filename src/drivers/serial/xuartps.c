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

#تعریف_کن UART_CONTROL                 0x00
#تعریف_کن UART_نما                    0x04
#تعریف_کن UART_INTRPT_EN               0x08
#تعریف_کن UART_INTRPT_DIS              0x0C
#تعریف_کن UART_INTRPT_ماسک             0x10
#تعریف_کن UART_CHNL_INT_STS            0x14
#تعریف_کن UART_BAUD_نرخ_GEN           0x18
#تعریف_کن UART_RCVR_TIMEOUT            0x1C
#تعریف_کن UART_RCVR_FIFO_TRIGGER_LEVEL 0x20
#تعریف_کن UART_MODEM_CTRL              0x24
#تعریف_کن UART_MODEM_STS               0x28
#تعریف_کن UART_CHANNEL_STS             0x2C
#تعریف_کن UART_TX_RX_FIFO              0x30
#تعریف_کن UART_BAUD_نرخ_DIVIDER       0x34
#تعریف_کن UART_FLOW_DELAY              0x38
#تعریف_کن UART_TX_FIFO_TRIGGER_LEVEL   0x44

#تعریف_کن UART_INTRPT_ماسک_TXEMPTY     بیت(3)
#تعریف_کن UART_CHANNEL_STS_TXEMPTY     بیت(3)

#تعریف_کن UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شده CONFIG_PRINTING
فراغ uart_drv_putchar(بیعلامت حرف c)
{
    طالما (!(*UART_REG(UART_CHANNEL_STS) & UART_CHANNEL_STS_TXEMPTY));
    *UART_REG(UART_TX_RX_FIFO) = c;
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
بیعلامت حرف uart_drv_getchar(فراغ)
{
    طالما (!(*UART_REG(UART_CHANNEL_STS) & بیت(UART_CHANNEL_STS_TXEMPTY)));
    أرجع *UART_REG(UART_TX_RX_FIFO);
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */
