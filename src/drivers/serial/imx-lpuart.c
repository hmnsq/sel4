/*
 * Copyright 2021, Breakaway Consulting Pty. Ltd.
 *
 * A simple output only UART driver for the NXP i.MX Low Power UART.
 *
 * Technical Reference:
 *   i.MX 8DualX/8DualXPlus/8QuadXPlus Applications Processor Reference Manual
 *   Revision 0 (IMX8DQXPRM.pdf)
 *   Chapter 16.13 (page 7908)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>

#ವ್ಯಾಖ್ಯಾನಿಸು STAT 0x14
#ವ್ಯಾಖ್ಯಾನಿಸು TRANSMIT 0x1c

#ವ್ಯಾಖ್ಯಾನಿಸು STAT_TDRE (1 << 23)

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG(x) ((ಅಸ್ಥಿರ uint32_t *)(UART_PPTR + (x)))

#ಆದರೆ defined(CONFIG_DEBUG_BUILD) || defined(CONFIG_PRINTING)
ಶೂನ್ಯ uart_drv_putchar(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    ಆಗ (!(*UART_REG(STAT) & STAT_TDRE)) { }
    *UART_REG(TRANSMIT) = c;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
