/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <drivers/uart.h>

#ifdef CONFIG_PRINTING
শূন্য kernel_putDebugChar(unsigned বর্ণ c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned বর্ণ kernel_getDebugChar(শূন্য)
{
    প্রত্যাগম uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
