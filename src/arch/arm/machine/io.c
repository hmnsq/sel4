/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن <config.h>
#ضمن <machine/io.h>
#ضمن <drivers/uart.h>

#ifdef CONFIG_PRINTING
فراغ kernel_putDebugChar(unsigned حرف c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned حرف kernel_getDebugChar(فراغ)
{
    أرجع uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
