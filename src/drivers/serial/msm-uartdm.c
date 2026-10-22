/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <stdint.h>
#समावेश <util.h>
#समावेश <machine/io.h>

#घोषणा USR                   0x08
#घोषणा UTF                   0x70
#घोषणा UNTX                  0x40

#घोषणा USR_RXRDY             BIT(0)
#घोषणा USR_RXFUL             BIT(1)
#घोषणा USR_TXRDY             BIT(2)
#घोषणा USR_TXEMP             BIT(3)

#घोषणा UART_REG(X) ((अस्थिर uint32_t *)(UART_PPTR + (X)))

#अगर_घोषित CONFIG_PRINTING
व्योम uart_drv_putchar(अचिन्हित अक्षर c)
{
    जबतक ((*UART_REG(USR) & USR_TXEMP) == 0);
    /* Tell the peripheral how many characters to send */
    *UART_REG(UNTX) = 1;
    /* Write the character into the FIFO */
    *UART_REG(UTF) = c & 0xff;
}
#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर uart_drv_getchar(व्योम)
{
    जबतक ((*UART_REG(USR) & USR_RXRDY) == 0);

    वापस *UART_REG(UTF) & 0xff;
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
