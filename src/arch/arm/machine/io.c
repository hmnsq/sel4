/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <config.h>
#include <machine/io.h>
#include <drivers/uart.h>

#ifdef CONFIG_PRINTING
void kernel_putDebugChar(unsigned var_na c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned var_na kernel_getDebugChar(void)
{
    ཕྱིར uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
