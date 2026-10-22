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

#ವ್ಯಾಖ್ಯಾನಿಸು URXD  0x00 /* UART Receiver Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UTXD  0x40 /* UART Transmitter Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UCR1  0x80 /* UART Control Register 1 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCR2  0x84 /* UART Control Register 2 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCR3  0x88 /* UART Control Register 3 */
#ವ್ಯಾಖ್ಯಾನಿಸು UCR4  0x8c /* UART Control Register 4 */
#ವ್ಯಾಖ್ಯಾನಿಸು UFCR  0x90 /* UART FIFO Control Register */
#ವ್ಯಾಖ್ಯಾನಿಸು USR1  0x94 /* UART Status Register 1 */
#ವ್ಯಾಖ್ಯಾನಿಸು USR2  0x98 /* UART Status Register 2 */
#ವ್ಯಾಖ್ಯಾನಿಸು UESC  0x9c /* UART Escape Character Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UTIM  0xa0 /* UART Escape Timer Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UBIR  0xa4 /* UART BRM Incremental Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UBMR  0xa8 /* UART BRM Modulator Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UBRC  0xac /* UART Baud Rate Counter Register */
#ವ್ಯಾಖ್ಯಾನಿಸು ONEMS 0xb0 /* UART One Millisecond Register */
#ವ್ಯಾಖ್ಯಾನಿಸು UTS   0xb4 /* UART Test Register */

#ವ್ಯಾಖ್ಯಾನಿಸು UART_SR1_TRDY         ಬಿಟ್(13)
#ವ್ಯಾಖ್ಯಾನಿಸು UART_SR1_RRDY         ಬಿಟ್(9)
#ವ್ಯಾಖ್ಯಾನಿಸು UART_SR2_TXFIFO_ಖಾಲಿಗಳು ಬಿಟ್(14)
#ವ್ಯಾಖ್ಯಾನಿಸು UART_SR2_RXFIFO_RDR   ಬಿಟ್(0)

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG(x) ((ಅಸ್ಥಿರ uint32_t *)(UART_PPTR + (x)))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    ಆಗ (!(*UART_REG(USR2) & UART_SR2_TXFIFO_ಖಾಲಿಗಳು));
    *UART_REG(UTXD) = c;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಆಗ (!(*UART_REG(USR2) & UART_SR2_RXFIFO_RDR));
    ಹಿಂತಿರುಗಿ *UART_REG(URXD);
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
