/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ضمن <config.h>
#ضمن <machine/io.h>
#ضمن <drivers/uart.h>

#اگر_تعریف_شده CONFIG_PRINTING
فراغ kernel_putDebugChar(بیعلامت حرف c)
{
    uart_console_putchar(c);
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
بیعلامت حرف kernel_getDebugChar(فراغ)
{
    أرجع uart_drv_getchar();
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */
