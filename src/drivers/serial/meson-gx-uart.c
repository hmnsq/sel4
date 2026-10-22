/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices_gen.h>

#ವ್ಯಾಖ್ಯಾನಿಸು UART_WFIFO  0x0
#ವ್ಯಾಖ್ಯಾನಿಸು UART_RFIFO  0x4
#ವ್ಯಾಖ್ಯಾನಿಸು UART_ಸ್ಥಿತಿ 0xC

#ವ್ಯಾಖ್ಯಾನಿಸು UART_TX_ತುಂಬಿದ        ಬಿಟ್(21)
#ವ್ಯಾಖ್ಯಾನಿಸು UART_RX_ಖಾಲಿಗಳು       ಬಿಟ್(20)

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG(x) ((ಅಸ್ಥಿರ uint32_t *)(UART_PPTR + (x)))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    ಆಗ ((*UART_REG(UART_ಸ್ಥಿತಿ) & UART_TX_ತುಂಬಿದ));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಆಗ ((*UART_REG(UART_ಸ್ಥಿತಿ) & UART_RX_ಖಾಲಿಗಳು));
    ಹಿಂತಿರುಗಿ *UART_REG(UART_RFIFO);
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
