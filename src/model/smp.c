/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <model/smp.h>
#ଅନ୍ତର୍ଭାଵ <object/tcb.h>

#ifdef ENABLE_SMP_SUPPORT

ଶୂନ୍ୟ migrateTCB(tcb_t *tcb, word_t new_core)
{
    ଯଦି (new_core == tcb->tcbAffinity) {
        ଫେରନ୍ତୁ;
    }
#ifdef CONFIG_DEBUG_BUILD
    tcbDebugRemove(tcb);
#endif
#ifdef CONFIG_HAVE_FPU
    /* If the thread owns the FPU of the core it is currently running on (which
     * is not necessarily the core, that we are now running on), then release
     * that cores's FPU.
     */
    fpuRelease(tcb);
#endif /* CONFIG_HAVE_FPU */
    tcb->tcbAffinity = new_core;
#ifdef CONFIG_DEBUG_BUILD
    tcbDebugAppend(tcb);
#endif
}

#endif /* ENABLE_SMP_SUPPORT */
