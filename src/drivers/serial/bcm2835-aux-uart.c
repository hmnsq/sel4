/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <stdint.h>
#समावेश <util.h>
#समावेश <machine/io.h>

/* When DLAB=1, MU_IO is a baud rate register.
 * Otherwise, write to TX, read to RX */
#घोषणा MU_IO       0x00
/* When DLAB=1, MU_IIR is a baud rate register.
 * Otherwise IRQ enable */
#घोषणा MU_IIR      0x04
#घोषणा MU_IER      0x08
#घोषणा MU_LCR      0x0C
#घोषणा MU_MCR      0x10
#घोषणा MU_LSR      0x14
#घोषणा MU_MSR      0x18
#घोषणा MU_SCRATCH  0x1C
#घोषणा MU_CNTL     0x20

/* This bit is set if the transmit FIFO can accept at least one byte.*/
#घोषणा MU_LSR_TXEMPTY   BIT(5)
/* This bit is set if the transmit FIFO is empty and the
 * transmitter is idle. (Finished shifting out the last bit). */
#घोषणा MU_LSR_TXIDLE    BIT(6)
#घोषणा MU_LSR_RXOVERRUN BIT(1)
#घोषणा MU_LSR_DATAREADY BIT(0)
#घोषणा MU_LCR_DLAB      BIT(7)
#घोषणा MU_LCR_BREAK     BIT(6)
#घोषणा MU_LCR_DATASIZE  BIT(0)

#घोषणा UART_REG(x) ((अस्थिर uint32_t *)(UART_PPTR + (x)))

#अगर_घोषित CONFIG_PRINTING
व्योम uart_drv_putchar(अचिन्हित अक्षर c)
{
    जबतक (!(*UART_REG(MU_LSR) & MU_LSR_TXIDLE));
    *UART_REG(MU_IO) = (c & 0xff);
}
#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर uart_drv_getchar(व्योम)
{
    जबतक (!(*UART_REG(MU_LSR) & MU_LSR_DATAREADY));
    वापस *UART_REG(MU_IO);
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
