/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <arch/sbi.h>

#ifdef CONFIG_PRINTING
शून्य kernel_putDebugChar(अचिन्हित वर्ण c)
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
अचिन्हित वर्ण kernel_getDebugChar(शून्य)
{
    /* Don't use UART, but read from the SBI console. */
    प्रत्यागम sbi_console_getchar();
}
#endif /* CONFIG_DEBUG_BUILD */
