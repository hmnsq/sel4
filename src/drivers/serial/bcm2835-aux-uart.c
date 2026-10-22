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
#تعریف_کړه MU_IO       0x00
/* When DLAB=1, MU_IIR is a baud rate register.
 * Otherwise IRQ enable */
#تعریف_کړه MU_IIR      0x04
#تعریف_کړه MU_IER      0x08
#تعریف_کړه MU_LCR      0x0C
#تعریف_کړه MU_MCR      0x10
#تعریف_کړه MU_LSR      0x14
#تعریف_کړه MU_MSR      0x18
#تعریف_کړه MU_SCRATCH  0x1C
#تعریف_کړه MU_CNTL     0x20

/* This bit is set if the transmit FIFO can accept at least one byte.*/
#تعریف_کړه MU_LSR_TXEMPTY   بټ(5)
/* This bit is set if the transmit FIFO is empty and the
 * transmitter is idle. (Finished shifting out the last bit). */
#تعریف_کړه MU_LSR_TXIDLE    بټ(6)
#تعریف_کړه MU_LSR_RXOVERRUN بټ(1)
#تعریف_کړه MU_LSR_DATAREADY بټ(0)
#تعریف_کړه MU_LCR_DLAB      بټ(7)
#تعریف_کړه MU_LCR_BREAK     بټ(6)
#تعریف_کړه MU_LCR_DATASIZE  بټ(0)

#تعریف_کړه UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#که_تعریف_شوی CONFIG_PRINTING
فراغ uart_drv_putchar(بینښه حرف c)
{
    طالما (!(*UART_REG(MU_LSR) & MU_LSR_TXIDLE));
    *UART_REG(MU_IO) = (c & 0xff);
}
#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف uart_drv_getchar(فراغ)
{
    طالما (!(*UART_REG(MU_LSR) & MU_LSR_DATAREADY));
    أرجع *UART_REG(MU_IO);
}
#د_که_پای /* CONFIG_DEBUG_BUILD */
