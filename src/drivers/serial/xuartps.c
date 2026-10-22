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

#ವ್ಯಾಖ್ಯಾನಿಸು UART_CONTROL                 0x00
#ವ್ಯಾಖ್ಯಾನಿಸು UART_ಬಹುಲಕ                    0x04
#ವ್ಯಾಖ್ಯಾನಿಸು UART_INTRPT_EN               0x08
#ವ್ಯಾಖ್ಯಾನಿಸು UART_INTRPT_DIS              0x0C
#ವ್ಯಾಖ್ಯಾನಿಸು UART_INTRPT_ಮುಖವಾಡ             0x10
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CHNL_INT_STS            0x14
#ವ್ಯಾಖ್ಯಾನಿಸು UART_BAUD_ದರ_GEN           0x18
#ವ್ಯಾಖ್ಯಾನಿಸು UART_RCVR_TIMEOUT            0x1C
#ವ್ಯಾಖ್ಯಾನಿಸು UART_RCVR_FIFO_TRIGGER_LEVEL 0x20
#ವ್ಯಾಖ್ಯಾನಿಸು UART_MODEM_CTRL              0x24
#ವ್ಯಾಖ್ಯಾನಿಸು UART_MODEM_STS               0x28
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CHANNEL_STS             0x2C
#ವ್ಯಾಖ್ಯಾನಿಸು UART_TX_RX_FIFO              0x30
#ವ್ಯಾಖ್ಯಾನಿಸು UART_BAUD_ದರ_DIVIDER       0x34
#ವ್ಯಾಖ್ಯಾನಿಸು UART_FLOW_DELAY              0x38
#ವ್ಯಾಖ್ಯಾನಿಸು UART_TX_FIFO_TRIGGER_LEVEL   0x44

#ವ್ಯಾಖ್ಯಾನಿಸು UART_INTRPT_ಮುಖವಾಡ_TXEMPTY     ಬಿಟ್(3)
#ವ್ಯಾಖ್ಯಾನಿಸು UART_CHANNEL_STS_TXEMPTY     ಬಿಟ್(3)

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG(x) ((ಅಸ್ಥಿರ uint32_t *)(UART_PPTR + (x)))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    ಆಗ (!(*UART_REG(UART_CHANNEL_STS) & UART_CHANNEL_STS_TXEMPTY));
    *UART_REG(UART_TX_RX_FIFO) = c;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಆಗ (!(*UART_REG(UART_CHANNEL_STS) & ಬಿಟ್(UART_CHANNEL_STS_TXEMPTY)));
    ಹಿಂತಿರುಗಿ *UART_REG(UART_TX_RX_FIFO);
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
