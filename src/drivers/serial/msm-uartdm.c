/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>

#ವ್ಯಾಖ್ಯಾನಿಸು USR                   0x08
#ವ್ಯಾಖ್ಯಾನಿಸು UTF                   0x70
#ವ್ಯಾಖ್ಯಾನಿಸು UNTX                  0x40

#ವ್ಯಾಖ್ಯಾನಿಸು USR_RXRDY             ಬಿಟ್(0)
#ವ್ಯಾಖ್ಯಾನಿಸು USR_RXFUL             ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು USR_TXRDY             ಬಿಟ್(2)
#ವ್ಯಾಖ್ಯಾನಿಸು USR_TXEMP             ಬಿಟ್(3)

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG(X) ((ಅಸ್ಥಿರ uint32_t *)(UART_PPTR + (X)))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    ಆಗ ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಆಗ ((*UART_REG(USR) & USR_RXRDY) == 0);

    ಹಿಂತಿರುಗಿ *UART_REG(UTF) & 0xff;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
