/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <arch/sbi.h>

#నిర్వచితమైతే CONFIG_PRINTING
శూన్య kernel_putDebugChar(గుర్తులేని అక్షరం c)
{
    /* Don't use any UART driver, but write to the SBI console. It depends on
     * the SBI implementation if printing a '\r' (CR) before any '\n' (LF) is
     * required explicitly or if SBI takes care of this. Currently BBL requires
     * it, while OpenSBI takes care of this internally. Since we dropped BBL
     * support in favor of OpenSBI, we do not print a '\r' (CR) here.
     */
    sbi_console_putchar(c);
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
గుర్తులేని అక్షరం kernel_getDebugChar(శూన్య)
{
    /* Don't use UART, but read from the SBI console. */
    తిరిగి sbi_console_getchar();
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */
