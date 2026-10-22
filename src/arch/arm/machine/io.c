/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#شامل <config.h>
#شامل <machine/io.h>
#شامل <drivers/uart.h>

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی kernel_putDebugChar(غیرعلامتی حرف c)
{
    uart_console_putchar(c);
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف kernel_getDebugChar(خالی)
{
    واپس uart_drv_getchar();
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
