/*
 * Copyright 2021, Axel Heider <axelheider@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <drivers/uart.h>

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య kernel_putDebugChar(గుర్తులేని అక్షరం c)
{
    uart_console_putchar(c);
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం kernel_getDebugChar(శూన్య)
{
    తిరిగి uart_drv_getchar();
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
