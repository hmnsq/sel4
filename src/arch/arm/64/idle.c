/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <mode/machine.h>
#समावेश <api/debug.h>

/** DONT_TRANSLATE */
व्योम NORETURN NO_INLINE VISIBLE halt(व्योम)
{
    /* halt is actually, idle thread without the interrupts */
    MSR("daif", (DAIF_DEBUG | DAIF_SERROR | DAIF_IRQ | DAIF_FIRQ));

#अगर_घोषित CONFIG_PRINTING
    म_लिखो("halting...");
#अगर_घोषित CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#पूर्ण_अगर
#पूर्ण_अगर
    idle_thread();
    UNREACHABLE();
}
