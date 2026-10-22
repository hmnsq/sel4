/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <plat/machine/devices_gen.h>

#define UARTDR                    0x000
#define UARTFR                    0x018

#define PL011_UARTFR_TXFF         BIT(5)
#define PL011_UARTFR_RXFE         BIT(4)

#define UART_REG(x) ((અસ્થિર uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
શૂન્ય uart_drv_putchar(unsigned અક્ષર c)
{
    જ્યારે ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned અક્ષર uart_drv_getchar(શૂન્ય)
{
    જ્યારે ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    પાછા *UART_REG(UARTDR);
}
#endif  /* CONFIG_DEBUG_BUILD */
