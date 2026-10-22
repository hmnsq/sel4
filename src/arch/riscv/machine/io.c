/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <arch/sbi.h>

#ifdef CONFIG_PRINTING
ಶೂನ್ಯ kernel_putDebugChar(unsigned ವರ್ಣ c)
{
    /* Don't use any UART driver, but write to the SBI console. It depends on
     * the SBI implementation if printing a '\r' (CR) before any '\n' (LF) is
     * required explicitly or if SBI takes care of this. Currently BBL requires
     * it, while OpenSBI takes care of this internally. Since we dropped BBL
     * support in favor of OpenSBI, we do not print a '\r' (CR) here.
     */
    sbi_console_putchar(c);
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
unsigned ವರ್ಣ kernel_getDebugChar(ಶೂನ್ಯ)
{
    /* Don't use UART, but read from the SBI console. */
    ಪ್ರತ್ಯಾಗಮ sbi_console_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
