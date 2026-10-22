/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/machine.h>
#अन्तर्भाव <api/debug.h>

/** DONT_TRANSLATE */
शून्य NORETURN NO_INLINE VISIBLE halt(शून्य)
{
    /* halt is actually, idle thread without the interrupts */
    MSR("daif", (DAIF_DEBUG | DAIF_SERROR | DAIF_IRQ | DAIF_FIRQ));

#ifdef CONFIG_PRINTING
    म_लिखो("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
