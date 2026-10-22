/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>

/* When DLAB=1, MU_IO is a baud rate register.
 * Otherwise, write to TX, read to RX */
#ವ್ಯಾಖ್ಯಾನಿಸು MU_IO       0x00
/* When DLAB=1, MU_IIR is a baud rate register.
 * Otherwise IRQ enable */
#ವ್ಯಾಖ್ಯಾನಿಸು MU_IIR      0x04
#ವ್ಯಾಖ್ಯಾನಿಸು MU_IER      0x08
#ವ್ಯಾಖ್ಯಾನಿಸು MU_LCR      0x0C
#ವ್ಯಾಖ್ಯಾನಿಸು MU_MCR      0x10
#ವ್ಯಾಖ್ಯಾನಿಸು MU_LSR      0x14
#ವ್ಯಾಖ್ಯಾನಿಸು MU_MSR      0x18
#ವ್ಯಾಖ್ಯಾನಿಸು MU_SCRATCH  0x1C
#ವ್ಯಾಖ್ಯಾನಿಸು MU_CNTL     0x20

/* This bit is set if the transmit FIFO can accept at least one byte.*/
#ವ್ಯಾಖ್ಯಾನಿಸು MU_LSR_TXEMPTY   ಬಿಟ್(5)
/* This bit is set if the transmit FIFO is empty and the
 * transmitter is idle. (Finished shifting out the last bit). */
#ವ್ಯಾಖ್ಯಾನಿಸು MU_LSR_TXIDLE    ಬಿಟ್(6)
#ವ್ಯಾಖ್ಯಾನಿಸು MU_LSR_RXOVERRUN ಬಿಟ್(1)
#ವ್ಯಾಖ್ಯಾನಿಸು MU_LSR_DATAREADY ಬಿಟ್(0)
#ವ್ಯಾಖ್ಯಾನಿಸು MU_LCR_DLAB      ಬಿಟ್(7)
#ವ್ಯಾಖ್ಯಾನಿಸು MU_LCR_BREAK     ಬಿಟ್(6)
#ವ್ಯಾಖ್ಯಾನಿಸು MU_LCR_DATASIZE  ಬಿಟ್(0)

#ವ್ಯಾಖ್ಯಾನಿಸು UART_REG(x) ((ಅಸ್ಥಿರ uint32_t *)(UART_PPTR + (x)))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಶೂನ್ಯ uart_drv_putchar(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c)
{
    ಆಗ (!(*UART_REG(MU_LSR) & MU_LSR_TXIDLE));
    *UART_REG(MU_IO) = (c & 0xff);
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ uart_drv_getchar(ಶೂನ್ಯ)
{
    ಆಗ (!(*UART_REG(MU_LSR) & MU_LSR_DATAREADY));
    ಹಿಂತಿರುಗಿ *UART_REG(MU_IO);
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
