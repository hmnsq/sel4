/*
 * Copyright 2014, General Dynamics C4 Systems
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
    यंत्र अस्थिर("cpsid iaf");

#ifdef CONFIG_PRINTING
    म_लिखो("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
