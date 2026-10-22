/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن <config.h>
#ضمن <machine/io.h>
#ضمن <drivers/uart.h>

#جے_تعریف_شدہ CONFIG_PRINTING
فراغ kernel_putDebugChar(بنانشانی حرف c)
{
    uart_console_putchar(c);
}
#جے_انت /* CONFIG_PRINTING */

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
بنانشانی حرف kernel_getDebugChar(فراغ)
{
    أرجع uart_drv_getchar();
}
#جے_انت /* CONFIG_DEBUG_BUILD */
