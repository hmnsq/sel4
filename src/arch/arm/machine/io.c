/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#شامل <config.h>
#شامل <machine/io.h>
#شامل <drivers/uart.h>

#ifdef CONFIG_PRINTING
تهی kernel_putDebugChar(unsigned نویسه c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned نویسه kernel_getDebugChar(تهی)
{
    بازگشت uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
