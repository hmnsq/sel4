/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>

/* When DLAB=1, MU_IO is a baud rate register.
 * Otherwise, write to TX, read to RX */
#परिभाषय MU_IO       0x00
/* When DLAB=1, MU_IIR is a baud rate register.
 * Otherwise IRQ enable */
#परिभाषय MU_IIR      0x04
#परिभाषय MU_IER      0x08
#परिभाषय MU_LCR      0x0C
#परिभाषय MU_MCR      0x10
#परिभाषय MU_LSR      0x14
#परिभाषय MU_MSR      0x18
#परिभाषय MU_SCRATCH  0x1C
#परिभाषय MU_CNTL     0x20

/* This bit is set if the transmit FIFO can accept at least one byte.*/
#परिभाषय MU_LSR_TXEMPTY   BIT(5)
/* This bit is set if the transmit FIFO is empty and the
 * transmitter is idle. (Finished shifting out the last bit). */
#परिभाषय MU_LSR_TXIDLE    BIT(6)
#परिभाषय MU_LSR_RXOVERRUN BIT(1)
#परिभाषय MU_LSR_DATAREADY BIT(0)
#परिभाषय MU_LCR_DLAB      BIT(7)
#परिभाषय MU_LCR_BREAK     BIT(6)
#परिभाषय MU_LCR_DATASIZE  BIT(0)

#परिभाषय UART_REG(x) ((चञ्चल uint32_t *)(UART_PPTR + (x)))

#यदि_परिभाषितम् CONFIG_PRINTING
शून्य uart_drv_putchar(अचिह्नित वर्ण c)
{
    यावत् (!(*UART_REG(MU_LSR) & MU_LSR_TXIDLE));
    *UART_REG(MU_IO) = (c & 0xff);
}
#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
अचिह्नित वर्ण uart_drv_getchar(शून्य)
{
    यावत् (!(*UART_REG(MU_LSR) & MU_LSR_DATAREADY));
    प्रत्यागम *UART_REG(MU_IO);
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
