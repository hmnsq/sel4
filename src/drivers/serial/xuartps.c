/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices_gen.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CONTROL                 0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_ਬਹੁਲਕ                    0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_INTRPT_EN               0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_INTRPT_DIS              0x0C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_INTRPT_ਨਕਾਬ             0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CHNL_INT_STS            0x14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_BAUD_ਦਰ_GEN           0x18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RCVR_TIMEOUT            0x1C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_RCVR_FIFO_TRIGGER_LEVEL 0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_MODEM_CTRL              0x24
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_MODEM_STS               0x28
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CHANNEL_STS             0x2C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_TX_RX_FIFO              0x30
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_BAUD_ਦਰ_DIVIDER       0x34
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_FLOW_DELAY              0x38
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_TX_FIFO_TRIGGER_LEVEL   0x44

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_INTRPT_ਨਕਾਬ_TXEMPTY     ਦੋਆਧਾਰੀ_ਅੰਕ(3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_CHANNEL_STS_TXEMPTY     ਦੋਆਧਾਰੀ_ਅੰਕ(3)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG(x) ((ਅਸਥਿਰ uint32_t *)(UART_PPTR + (x)))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਖਾਲੀ uart_drv_putchar(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    ਜਦੋਂ (!(*UART_REG(UART_CHANNEL_STS) & UART_CHANNEL_STS_TXEMPTY));
    *UART_REG(UART_TX_RX_FIFO) = c;
}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ uart_drv_getchar(ਖਾਲੀ)
{
    ਜਦੋਂ (!(*UART_REG(UART_CHANNEL_STS) & ਦੋਆਧਾਰੀ_ਅੰਕ(UART_CHANNEL_STS_TXEMPTY)));
    ਵਾਪਸ *UART_REG(UART_TX_RX_FIFO);
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
