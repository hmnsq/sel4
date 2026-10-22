/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <stdint.h>
#समावेश <util.h>
#समावेश <machine/io.h>
#समावेश <plat/machine/devices_gen.h>

#घोषणा UART_WFIFO  0x0
#घोषणा UART_RFIFO  0x4
#घोषणा UART_STATUS 0xC

#घोषणा UART_TX_FULL        BIT(21)
#घोषणा UART_RX_EMPTY       BIT(20)

#घोषणा UART_REG(x) ((अस्थिर uint32_t *)(UART_PPTR + (x)))

#अगर_घोषित CONFIG_PRINTING
व्योम uart_drv_putchar(अचिन्हित अक्षर c)
{
    जबतक ((*UART_REG(UART_STATUS) & UART_TX_FULL));

    /* Add character to the buffer. */
    *UART_REG(UART_WFIFO) = c;
}
#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर uart_drv_getchar(व्योम)
{
    जबतक ((*UART_REG(UART_STATUS) & UART_RX_EMPTY));
    वापस *UART_REG(UART_RFIFO);
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
