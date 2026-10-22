/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن <config.h>
#ضمن <machine/io.h>
#ضمن <drivers/uart.h>

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ kernel_putDebugChar(بنانشان حرف c)
{
    uart_console_putchar(c);
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
بنانشان حرف kernel_getDebugChar(فراغ)
{
    أرجع uart_drv_getchar();
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
