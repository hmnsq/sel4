/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <machine/io.h>
#समावेश <arch/sbi.h>

#अगर_घोषित CONFIG_PRINTING
व्योम kernel_putDebugChar(अचिन्हित अक्षर c)
{
    /* Don't use any UART driver, but write to the SBI console. It depends on
     * the SBI implementation if printing a '\r' (CR) before any '\n' (LF) is
     * required explicitly or if SBI takes care of this. Currently BBL requires
     * it, while OpenSBI takes care of this internally. Since we dropped BBL
     * support in favor of OpenSBI, we do not print a '\r' (CR) here.
     */
    sbi_console_putchar(c);
}
#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर_घोषित CONFIG_DEBUG_BUILD
अचिन्हित अक्षर kernel_getDebugChar(व्योम)
{
    /* Don't use UART, but read from the SBI console. */
    वापस sbi_console_getchar();
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
