/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <arch/sbi.h>

/** DONT_TRANSLATE */
व्योम VISIBLE NO_INLINE halt(व्योम)
{
#अगर_घोषित CONFIG_PRINTING
    म_लिखो("halting...");
#अगर_घोषित CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */
#पूर्ण_अगर /* CONFIG_PRINTING */

    sbi_shutdown();

    UNREACHABLE();
}
