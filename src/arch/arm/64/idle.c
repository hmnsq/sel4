/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <mode/machine.h>
#include <api/debug.h>

/** DONT_TRANSLATE */
void NORETURN NO_INLINE VISIBLE halt(void)
{
    /* halt is actually, idle thread without the interrupts */
    MSR("daif", (DAIF_DEBUG | DAIF_SERROR | DAIF_IRQ | DAIF_FIRQ));

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_इलिम्बाग_Kernel_Entry_Reason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
