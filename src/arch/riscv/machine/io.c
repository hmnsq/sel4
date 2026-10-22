/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <arch/sbi.h>

#ifdef CONFIG_PRINTING
ଶୂନ୍ୟ kernel_putDebugChar(unsigned ଅକ୍ଷର c)
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
unsigned ଅକ୍ଷର kernel_getDebugChar(ଶୂନ୍ୟ)
{
    /* Don't use UART, but read from the SBI console. */
    ଫେରନ୍ତୁ sbi_console_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
