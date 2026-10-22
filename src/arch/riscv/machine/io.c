/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <machine/io.h>
#ضمن <arch/sbi.h>

#جے_تعریف_شدہ CONFIG_PRINTING
فراغ kernel_putDebugChar(بنانشانی حرف c)
{
    /* Don't use any UART driver, but write to the SBI console. It depends on
     * the SBI implementation if printing a '\r' (CR) before any '\n' (LF) is
     * required explicitly or if SBI takes care of this. Currently BBL requires
     * it, while OpenSBI takes care of this internally. Since we dropped BBL
     * support in favor of OpenSBI, we do not print a '\r' (CR) here.
     */
    sbi_console_putchar(c);
}
#جے_انت /* CONFIG_PRINTING */

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
بنانشانی حرف kernel_getDebugChar(فراغ)
{
    /* Don't use UART, but read from the SBI console. */
    أرجع sbi_console_getchar();
}
#جے_انت /* CONFIG_DEBUG_BUILD */
