/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <drivers/uart.h>

#ifdef CONFIG_PRINTING
வெறுமை kernel_putDebugChar(unsigned எழுத்து c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned எழுத்து kernel_getDebugChar(வெறுமை)
{
    திரும்பு uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
