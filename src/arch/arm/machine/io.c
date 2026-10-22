/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <drivers/uart.h>

#यदि_परिभाषितम् CONFIG_PRINTING
शून्य kernel_putDebugChar(अचिह्नित वर्ण c)
{
    uart_console_putchar(c);
}
#यदि_अन्तः /* CONFIG_PRINTING */

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
अचिह्नित वर्ण kernel_getDebugChar(शून्य)
{
    प्रत्यागम uart_drv_getchar();
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */
