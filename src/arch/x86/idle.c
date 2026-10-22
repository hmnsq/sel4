/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <api/debug.h>

/** DONT_TRANSLATE */
ଶୂନ୍ୟ VISIBLE halt(ଶୂନ୍ୟ)
{
    /* halt is actually, idle thread without the interrupts */
    asm ଅସ୍ଥିର("cli");

#ifdef CONFIG_PRINTING
    printf("halting...");
#ifdef CONFIG_DEBUG_BUILD
    debug_printKernelEntryReason();
#endif
#endif
    idle_thread();
    UNREACHABLE();
}
