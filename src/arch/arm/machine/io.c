/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <drivers/uart.h>

#ifdef CONFIG_PRINTING
শূন্য kernel_putDebugChar(unsigned অক্ষর c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned অক্ষর kernel_getDebugChar(শূন্য)
{
    ফেরত uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
