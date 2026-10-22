/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#समावेश <config.h>
#समावेश <machine/io.h>
#समावेश <drivers/uart.h>

#अगर_घोषित CONFIG_PRINTING
व्योम kernel_putDebugChar(अचिन्हित अक्षर c)
{
    uart_console_putchar(c);
}
#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर kernel_getDebugChar(व्योम)
{
    वापस uart_drv_getchar();
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
