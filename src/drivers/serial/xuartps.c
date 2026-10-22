/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/devices_gen.h>

#परिभाषय UART_CONTROL                 0x00
#परिभाषय UART_बहुलकम्                    0x04
#परिभाषय UART_INTRPT_EN               0x08
#परिभाषय UART_INTRPT_DIS              0x0C
#परिभाषय UART_INTRPT_आवरणम्             0x10
#परिभाषय UART_CHNL_INT_STS            0x14
#परिभाषय UART_BAUD_दरः_GEN           0x18
#परिभाषय UART_RCVR_TIMEOUT            0x1C
#परिभाषय UART_RCVR_FIFO_TRIGGER_LEVEL 0x20
#परिभाषय UART_MODEM_CTRL              0x24
#परिभाषय UART_MODEM_STS               0x28
#परिभाषय UART_CHANNEL_STS             0x2C
#परिभाषय UART_TX_RX_FIFO              0x30
#परिभाषय UART_BAUD_दरः_DIVIDER       0x34
#परिभाषय UART_FLOW_DELAY              0x38
#परिभाषय UART_TX_FIFO_TRIGGER_LEVEL   0x44

#परिभाषय UART_INTRPT_आवरणम्_TXEMPTY     BIT(3)
#परिभाषय UART_CHANNEL_STS_TXEMPTY     BIT(3)

#परिभाषय UART_REG(x) ((चञ्चल uint32_t *)(UART_PPTR + (x)))

#यदि_परिभाषितम् CONFIG_PRINTING
शून्य uart_drv_putchar(अचिह्नित वर्ण c)
{
    यावत् (!(*UART_REG(UART_CHANNEL_STS) & UART_CHANNEL_STS_TXEMPTY));
    *UART_REG(UART_TX_RX_FIFO) = c;
}
#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
अचिह्नित वर्ण uart_drv_getchar(शून्य)
{
    यावत् (!(*UART_REG(UART_CHANNEL_STS) & BIT(UART_CHANNEL_STS_TXEMPTY)));
    प्रत्यागम *UART_REG(UART_TX_RX_FIFO);
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
