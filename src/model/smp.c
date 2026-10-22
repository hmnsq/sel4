/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <model/smp.h>
#अन्तर्भाव <object/tcb.h>

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT

शून्य migrateTCB(tcb_t *tcb, word_t new_core)
{
    यदि (new_core == tcb->tcbAffinity) {
        प्रत्यागम;
    }
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
    tcbDebugRemove(tcb);
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_HAVE_FPU
    /* If the thread owns the FPU of the core it is currently running on (which
     * is not necessarily the core, that we are now running on), then release
     * that cores's FPU.
     */
    fpuRelease(tcb);
#यदि_अन्तः /* CONFIG_HAVE_FPU */
    tcb->tcbAffinity = new_core;
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
    tcbDebugAppend(tcb);
#यदि_अन्तः
}

#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
