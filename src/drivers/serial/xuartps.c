/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <stdint.h>
#समावेश <util.h>
#समावेश <machine/io.h>
#समावेश <plat/machine/devices_gen.h>

#घोषणा UART_CONTROL                 0x00
#घोषणा UART_MODE                    0x04
#घोषणा UART_INTRPT_EN               0x08
#घोषणा UART_INTRPT_DIS              0x0C
#घोषणा UART_INTRPT_MASK             0x10
#घोषणा UART_CHNL_INT_STS            0x14
#घोषणा UART_BAUD_RATE_GEN           0x18
#घोषणा UART_RCVR_TIMEOUT            0x1C
#घोषणा UART_RCVR_FIFO_TRIGGER_LEVEL 0x20
#घोषणा UART_MODEM_CTRL              0x24
#घोषणा UART_MODEM_STS               0x28
#घोषणा UART_CHANNEL_STS             0x2C
#घोषणा UART_TX_RX_FIFO              0x30
#घोषणा UART_BAUD_RATE_DIVIDER       0x34
#घोषणा UART_FLOW_DELAY              0x38
#घोषणा UART_TX_FIFO_TRIGGER_LEVEL   0x44

#घोषणा UART_INTRPT_MASK_TXEMPTY     BIT(3)
#घोषणा UART_CHANNEL_STS_TXEMPTY     BIT(3)

#घोषणा UART_REG(x) ((अस्थिर uint32_t *)(UART_PPTR + (x)))

#अगर_घोषित CONFIG_PRINTING
व्योम uart_drv_putchar(अचिन्हित अक्षर c)
{
    जबतक (!(*UART_REG(UART_CHANNEL_STS) & UART_CHANNEL_STS_TXEMPTY));
    *UART_REG(UART_TX_RX_FIFO) = c;
}
#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर uart_drv_getchar(व्योम)
{
    जबतक (!(*UART_REG(UART_CHANNEL_STS) & BIT(UART_CHANNEL_STS_TXEMPTY)));
    वापस *UART_REG(UART_TX_RX_FIFO);
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
