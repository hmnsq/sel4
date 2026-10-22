/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <drivers/uart.h>

#ifdef CONFIG_PRINTING
ଶୂନ୍ୟ kernel_putDebugChar(unsigned ଅକ୍ଷର c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ଅକ୍ଷର kernel_getDebugChar(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
