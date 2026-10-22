/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/uart.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਖਾਲੀ kernel_putDebugChar(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c)
{
    uart_console_putchar(c);
}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ kernel_getDebugChar(ਖਾਲੀ)
{
    ਵਾਪਸ uart_drv_getchar();
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
