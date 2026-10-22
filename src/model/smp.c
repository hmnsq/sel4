/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <model/smp.h>
#அந்தர்பாவ <object/tcb.h>

#ifdef ENABLE_SMP_SUPPORT

வெறுமை migrateTCB(tcb_t *tcb, word_t new_core)
{
    எனில் (new_core == tcb->tcbAffinity) {
        திரும்பு;
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
