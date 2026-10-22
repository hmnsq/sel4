/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <drivers/uart.h>

#ifdef CONFIG_PRINTING
शून्य kernel_putDebugChar(unsigned वर्ण c)
{
    uart_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned वर्ण kernel_getDebugChar(शून्य)
{
    पटिनिवत्त uart_drv_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
