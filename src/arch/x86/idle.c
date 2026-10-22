/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <api/debug.h>

/** DONT_TRANSLATE */
void VISIBLE halt(void)
{
    /* halt is actually, idle thread without the interrupts */
    asm volatile("cli");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_इलिम्बाग_Kernel_Entry_Reason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
