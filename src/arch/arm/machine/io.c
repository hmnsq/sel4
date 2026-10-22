/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن <config.h>
#ضمن <machine/io.h>
#ضمن <drivers/uart.h>

#که_تعریف_شوی CONFIG_PRINTING
فراغ kernel_putDebugChar(بینښه حرف c)
{
    uart_console_putchar(c);
}
#د_که_پای /* CONFIG_PRINTING */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
بینښه حرف kernel_getDebugChar(فراغ)
{
    أرجع uart_drv_getchar();
}
#د_که_پای /* CONFIG_DEBUG_BUILD */
