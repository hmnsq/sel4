/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <drivers/uart.h>

#ifdef CONFIG_PRINTING
শূন্য kernel_putDebugChar(unsigned ৱর্ণ c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ৱর্ণ kernel_getDebugChar(শূন্য)
{
    ঘূৰাই uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
