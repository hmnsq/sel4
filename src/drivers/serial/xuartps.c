/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>

#define UART_CONTROL                 0x00
#define UART_MODE                    0x04
#define UART_INTRPT_EN               0x08
#define UART_INTRPT_DIS              0x0C
#define UART_INTRPT_MASK             0x10
#define UART_CHNL_INT_STS            0x14
#define UART_BAUD_RATE_GEN           0x18
#define UART_RCVR_TIMEOUT            0x1C
#define UART_RCVR_FIFO_TRIGGER_LEVEL 0x20
#define UART_MODEM_CTRL              0x24
#define UART_MODEM_STS               0x28
#define UART_CHANNEL_STS             0x2C
#define UART_TX_RX_FIFO              0x30
#define UART_BAUD_RATE_DIVIDER       0x34
#define UART_FLOW_DELAY              0x38
#define UART_TX_FIFO_TRIGGER_LEVEL   0x44

#define UART_INTRPT_MASK_TXEMPTY     BIT(3)
#define UART_CHANNEL_STS_TXEMPTY     BIT(3)

#define UART_REG(x) ((ಚಞ್ಚಲ uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(unsigned ವರ್ಣ c)
{
    ಯಾವತ್ (!(*UART_REG(UART_CHANNEL_STS) & UART_CHANNEL_STS_TXEMPTY));
    *UART_REG(UART_TX_RX_FIFO) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ವರ್ಣ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಯಾವತ್ (!(*UART_REG(UART_CHANNEL_STS) & BIT(UART_CHANNEL_STS_TXEMPTY)));
    ಪ್ರತ್ಯಾಗಮ *UART_REG(UART_TX_RX_FIFO);
}
#endif /* CONFIG_DEBUG_BUILD */
