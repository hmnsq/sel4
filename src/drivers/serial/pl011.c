/*
 * Copyright 2016, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <stdint.h>
#समावेश <util.h>
#समावेश <machine/io.h>
#समावेश <plat/machine/devices_gen.h>

#घोषणा UARTDR                    0x000
#घोषणा UARTFR                    0x018

#घोषणा PL011_UARTFR_TXFF         BIT(5)
#घोषणा PL011_UARTFR_RXFE         BIT(4)

#घोषणा UART_REG(x) ((अस्थिर uint32_t *)(UART_PPTR + (x)))

#अगर_घोषित CONFIG_PRINTING
व्योम uart_drv_putchar(अचिन्हित अक्षर c)
{
    जबतक ((*UART_REG(UARTFR) & PL011_UARTFR_TXFF) != 0);

    *UART_REG(UARTDR) = c;
}
#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर uart_drv_getchar(व्योम)
{
    जबतक ((*UART_REG(UARTFR) & PL011_UARTFR_RXFE) != 0);

    वापस *UART_REG(UARTDR);
}
#पूर्ण_अगर  /* CONFIG_DEBUG_BUILD */
