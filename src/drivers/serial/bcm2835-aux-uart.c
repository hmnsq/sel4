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
#وصف_ڪريو MU_IO       0x00
/* When DLAB=1, MU_IIR is a baud rate register.
 * Otherwise IRQ enable */
#وصف_ڪريو MU_IIR      0x04
#وصف_ڪريو MU_IER      0x08
#وصف_ڪريو MU_LCR      0x0C
#وصف_ڪريو MU_MCR      0x10
#وصف_ڪريو MU_LSR      0x14
#وصف_ڪريو MU_MSR      0x18
#وصف_ڪريو MU_SCRATCH  0x1C
#وصف_ڪريو MU_CNTL     0x20

/* This bit is set if the transmit FIFO can accept at least one byte.*/
#وصف_ڪريو MU_LSR_TXEMPTY   ٻٽو_انگ(5)
/* This bit is set if the transmit FIFO is empty and the
 * transmitter is idle. (Finished shifting out the last bit). */
#وصف_ڪريو MU_LSR_TXIDLE    ٻٽو_انگ(6)
#وصف_ڪريو MU_LSR_RXOVERRUN ٻٽو_انگ(1)
#وصف_ڪريو MU_LSR_DATAREADY ٻٽو_انگ(0)
#وصف_ڪريو MU_LCR_DLAB      ٻٽو_انگ(7)
#وصف_ڪريو MU_LCR_BREAK     ٻٽو_انگ(6)
#وصف_ڪريو MU_LCR_DATASIZE  ٻٽو_انگ(0)

#وصف_ڪريو UART_REG(x) ((متطاير uint32_t *)(UART_PPTR + (x)))

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ uart_drv_putchar(بنانشان حرف c)
{
    طالما (!(*UART_REG(MU_LSR) & MU_LSR_TXIDLE));
    *UART_REG(MU_IO) = (c & 0xff);
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف uart_drv_getchar(فراغ)
{
    طالما (!(*UART_REG(MU_LSR) & MU_LSR_DATAREADY));
    أرجع *UART_REG(MU_IO);
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
