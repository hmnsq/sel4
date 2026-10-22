/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <drivers/uart.h>

#ifdef CONFIG_PRINTING
શૂન્ય kernel_putDebugChar(unsigned અક્ષર c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned અક્ષર kernel_getDebugChar(શૂન્ય)
{
    પાછા uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
