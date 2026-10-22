/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <stdint.h>
#شامل <util.h>
#شامل <machine/io.h>

/* When DLAB=1, MU_IO is a baud rate register.
 * Otherwise, write to TX, read to RX */
#تعریف_کرو MU_IO       0x00
/* When DLAB=1, MU_IIR is a baud rate register.
 * Otherwise IRQ enable */
#تعریف_کرو MU_IIR      0x04
#تعریف_کرو MU_IER      0x08
#تعریف_کرو MU_LCR      0x0C
#تعریف_کرو MU_MCR      0x10
#تعریف_کرو MU_LSR      0x14
#تعریف_کرو MU_MSR      0x18
#تعریف_کرو MU_SCRATCH  0x1C
#تعریف_کرو MU_CNTL     0x20

/* This bit is set if the transmit FIFO can accept at least one byte.*/
#تعریف_کرو MU_LSR_TXEMPTY   ثنائی_ہندسہ(5)
/* This bit is set if the transmit FIFO is empty and the
 * transmitter is idle. (Finished shifting out the last bit). */
#تعریف_کرو MU_LSR_TXIDLE    ثنائی_ہندسہ(6)
#تعریف_کرو MU_LSR_RXOVERRUN ثنائی_ہندسہ(1)
#تعریف_کرو MU_LSR_DATAREADY ثنائی_ہندسہ(0)
#تعریف_کرو MU_LCR_DLAB      ثنائی_ہندسہ(7)
#تعریف_کرو MU_LCR_BREAK     ثنائی_ہندسہ(6)
#تعریف_کرو MU_LCR_DATASIZE  ثنائی_ہندسہ(0)

#تعریف_کرو UART_REG(x) ((متلون uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی uart_drv_putchar(غیرعلامتی حرف c)
{
    جب (!(*UART_REG(MU_LSR) & MU_LSR_TXIDLE));
    *UART_REG(MU_IO) = (c & 0xff);
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف uart_drv_getchar(خالی)
{
    جب (!(*UART_REG(MU_LSR) & MU_LSR_DATAREADY));
    واپس *UART_REG(MU_IO);
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
