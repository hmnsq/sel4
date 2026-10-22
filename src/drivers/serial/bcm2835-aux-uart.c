/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <stdint.h>
#ضمن <util.h>
#ضمن <machine/io.h>

/* When DLAB=1, MU_IO is a baud rate register.
 * Otherwise, write to TX, read to RX */
#تعریف_کن MU_IO       0x00
/* When DLAB=1, MU_IIR is a baud rate register.
 * Otherwise IRQ enable */
#تعریف_کن MU_IIR      0x04
#تعریف_کن MU_IER      0x08
#تعریف_کن MU_LCR      0x0C
#تعریف_کن MU_MCR      0x10
#تعریف_کن MU_LSR      0x14
#تعریف_کن MU_MSR      0x18
#تعریف_کن MU_SCRATCH  0x1C
#تعریف_کن MU_CNTL     0x20

/* This bit is set if the transmit FIFO can accept at least one byte.*/
#تعریف_کن MU_LSR_TXEMPTY   بیت(5)
/* This bit is set if the transmit FIFO is empty and the
 * transmitter is idle. (Finished shifting out the last bit). */
#تعریف_کن MU_LSR_TXIDLE    بیت(6)
#تعریف_کن MU_LSR_RXOVERRUN بیت(1)
#تعریف_کن MU_LSR_DATAREADY بیت(0)
#تعریف_کن MU_LCR_DLAB      بیت(7)
#تعریف_کن MU_LCR_BREAK     بیت(6)
#تعریف_کن MU_LCR_DATASIZE  بیت(0)

#تعریف_کن UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#اگر_تعریف_شده CONFIG_PRINTING
فراغ uart_drv_putchar(بیعلامت حرف c)
{
    طالما (!(*UART_REG(MU_LSR) & MU_LSR_TXIDLE));
    *UART_REG(MU_IO) = (c & 0xff);
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
بیعلامت حرف uart_drv_getchar(فراغ)
{
    طالما (!(*UART_REG(MU_LSR) & MU_LSR_DATAREADY));
    أرجع *UART_REG(MU_IO);
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */
