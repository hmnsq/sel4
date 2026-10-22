/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <machine/io.h>
#شامل <arch/sbi.h>

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی kernel_putDebugChar(غیرعلامتی حرف c)
{
    /* Don't use any UART driver, but write to the SBI console. It depends on
     * the SBI implementation if printing a '\r' (CR) before any '\n' (LF) is
     * required explicitly or if SBI takes care of this. Currently BBL requires
     * it, while OpenSBI takes care of this internally. Since we dropped BBL
     * support in favor of OpenSBI, we do not print a '\r' (CR) here.
     */
    sbi_console_putchar(c);
}
#اگر_ختم /* CONFIG_PRINTING */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
غیرعلامتی حرف kernel_getDebugChar(خالی)
{
    /* Don't use UART, but read from the SBI console. */
    واپس sbi_console_getchar();
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */
