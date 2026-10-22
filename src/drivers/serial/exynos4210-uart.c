/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>

#ವ್ಯಾಖ್ಯಾನಿಸು ULCON       0x0000 /* line control */
#ವ್ಯಾಖ್ಯಾನಿಸು UCON        0x0004 /* control */
#ವ್ಯಾಖ್ಯಾನಿಸು UFCON       0x0008 /* fifo control */
#ವ್ಯಾಖ್ಯಾನಿಸು UMCON       0x000C /* modem control */
#ವ್ಯಾಖ್ಯಾನಿಸು UTRSTAT     0x0010 /* TX/RX status */
#ವ್ಯಾಖ್ಯಾನಿಸು UERSTAT     0x0014 /* RX error status */
#ವ್ಯಾಖ್ಯಾನಿಸು UFSTAT      0x0018 /* FIFO status */
#ವ್ಯಾಖ್ಯಾನಿಸು UMSTAT      0x001C /* modem status */
#ವ್ಯಾಖ್ಯಾನಿಸು UTXH        0x0020 /* TX buffer */
#ವ್ಯಾಖ್ಯಾನಿಸು URXH        0x0024 /* RX buffer */
#ವ್ಯಾಖ್ಯಾನಿಸು UBRDIV      0x0028 /* baud rate divisor */
#ವ್ಯಾಖ್ಯಾನಿಸು UFRACVAL    0x002C /* divisor fractional value */
#ವ್ಯಾಖ್ಯಾನಿಸು UINTP       0x0030 /* interrupt pending */
#ವ್ಯಾಖ್ಯಾನಿಸು UINTSP      0x0034 /* interrupt source pending */
#ವ್ಯಾಖ್ಯಾನಿಸು UINTM       0x0038 /* interrupt mask */

/* UTRSTAT */
#ವ್ಯಾಖ್ಯಾನಿಸು TX_ಖಾಲಿಗಳು        ಬಿಟ್(2)
#ವ್ಯಾಖ್ಯಾನಿಸು TXBUF_ಖಾಲಿಗಳು     ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು RXBUF_READY     ಬಿಟ್(0)

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG(X) ((ಅಸ್ಥಿರ uint32_t *)(UART_PPTR + (X)))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    ಆಗ ((*UART_REG(UTRSTAT) & TXBUF_ಖಾಲಿಗಳು) == 0);
    *UART_REG(UTXH) = (c & 0xff);
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಆದರೆ ((*UART_REG(UTRSTAT) & RXBUF_READY)) {
        ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) * UART_REG(URXH);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ -1;
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
