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

#यदि_परिभाषितम् CONFIG_PRINTING
    प्रारूप_लिख("halting...");
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
    debug_print_Kernel_प्रविष्टिः_Reason();
#यदि_अन्तः
#यदि_अन्तः
    idle_thread();
    UNREACHABLE();
}
