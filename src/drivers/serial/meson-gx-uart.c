/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <plat/machine/devices_gen.h>

#define UART_WFIFO  0x0
#define UART_RFIFO  0x4
#define UART_STATUS 0xC

#define UART_TX_FULL        BIT(21)
#define UART_RX_EMPTY       BIT(20)

#define UART_REG(x) ((અસ્થિર uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
શૂન્ય uart_drv_putchar(unsigned અક્ષર c)
{
    જ્યારે ((*UART_REG(UART_STATUS) & UART_TX_FULL));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned અક્ષર uart_drv_getchar(શૂન્ય)
{
    જ્યારે ((*UART_REG(UART_STATUS) & UART_RX_EMPTY));
    પાછા *UART_REG(UART_RFIFO);
}
#endif /* CONFIG_DEBUG_BUILD */
