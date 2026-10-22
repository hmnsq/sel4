/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>
#شامل <plat/machine/devices_gen.h>

#تعریف_کرو UART_CONTROL                 0x00
#تعریف_کرو UART_منوال                    0x04
#تعریف_کرو UART_INTRPT_EN               0x08
#تعریف_کرو UART_INTRPT_DIS              0x0C
#تعریف_کرو UART_INTRPT_نقاب             0x10
#تعریف_کرو UART_CHNL_INT_STS            0x14
#تعریف_کرو UART_BAUD_شرح_GEN           0x18
#تعریف_کرو UART_RCVR_TIMEOUT            0x1C
#تعریف_کرو UART_RCVR_FIFO_TRIGGER_LEVEL 0x20
#تعریف_کرو UART_MODEM_CTRL              0x24
#تعریف_کرو UART_MODEM_STS               0x28
#تعریف_کرو UART_CHANNEL_STS             0x2C
#تعریف_کرو UART_TX_RX_FIFO              0x30
#تعریف_کرو UART_BAUD_شرح_DIVIDER       0x34
#تعریف_کرو UART_FLOW_DELAY              0x38
#تعریف_کرو UART_TX_FIFO_TRIGGER_LEVEL   0x44

#تعریف_کرو UART_INTRPT_نقاب_TXEMPTY     ثنائی_ہندسہ(3)
#تعریف_کرو UART_CHANNEL_STS_TXEMPTY     ثنائی_ہندسہ(3)

#تعریف_کرو UART_REG(x) ((متلون uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی uart_drv_putchar(غیرعلامتی حرف c)
{
    جب (!(*UART_REG(UART_CHANNEL_STS) & UART_CHANNEL_STS_TXEMPTY));
    *UART_REG(UART_TX_RX_FIFO) = c;
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف uart_drv_getchar(خالی)
{
    جب (!(*UART_REG(UART_CHANNEL_STS) & ثنائی_ہندسہ(UART_CHANNEL_STS_TXEMPTY)));
    واپس *UART_REG(UART_TX_RX_FIFO);
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
