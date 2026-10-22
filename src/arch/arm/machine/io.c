/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <drivers/uart.h>

#ifdef CONFIG_PRINTING
ശൂന്യം kernel_putDebugChar(unsigned അക്ഷരം c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned അക്ഷരം kernel_getDebugChar(ശൂന്യം)
{
    തിരികെ uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
