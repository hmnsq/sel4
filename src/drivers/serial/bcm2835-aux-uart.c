/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>

/* When DLAB=1, MU_IO is a baud rate register.
 * Otherwise, write to TX, read to RX */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_IO       0x00
/* When DLAB=1, MU_IIR is a baud rate register.
 * Otherwise IRQ enable */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_IIR      0x04
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_IER      0x08
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_LCR      0x0C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_MCR      0x10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_LSR      0x14
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_MSR      0x18
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_SCRATCH  0x1C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_CNTL     0x20

/* This bit is set if the transmit FIFO can accept at least one byte.*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_LSR_TXEMPTY   ਦੋਆਧਾਰੀ_ਅੰਕ(5)
/* This bit is set if the transmit FIFO is empty and the
 * transmitter is idle. (Finished shifting out the last bit). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_LSR_TXIDLE    ਦੋਆਧਾਰੀ_ਅੰਕ(6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_LSR_RXOVERRUN ਦੋਆਧਾਰੀ_ਅੰਕ(1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_LSR_DATAREADY ਦੋਆਧਾਰੀ_ਅੰਕ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_LCR_DLAB      ਦੋਆਧਾਰੀ_ਅੰਕ(7)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_LCR_BREAK     ਦੋਆਧਾਰੀ_ਅੰਕ(6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MU_LCR_DATASIZE  ਦੋਆਧਾਰੀ_ਅੰਕ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UART_REG(x) ((ਅਸਥਿਰ uint32_t *)(UART_PPTR + (x)))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਖਾਲੀ uart_drv_putchar(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    ਜਦੋਂ (!(*UART_REG(MU_LSR) & MU_LSR_TXIDLE));
    *UART_REG(MU_IO) = (c & 0xff);
}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ uart_drv_getchar(ਖਾਲੀ)
{
    ਜਦੋਂ (!(*UART_REG(MU_LSR) & MU_LSR_DATAREADY));
    ਵਾਪਸ *UART_REG(MU_IO);
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
