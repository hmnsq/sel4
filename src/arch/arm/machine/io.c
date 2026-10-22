/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <drivers/uart.h>

#ifdef CONFIG_PRINTING
ಶೂನ್ಯ kernel_putDebugChar(unsigned ವರ್ಣ c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ವರ್ಣ kernel_getDebugChar(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
