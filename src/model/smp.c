/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <model/smp.h>
#అన్తర్భావ <object/tcb.h>

#నిర్వచితమైతే ENABLE_SMP_SUPPORT

శూన్య migrateTCB(tcb_t *tcb, మాట_t new_core)
{
    ఒకవేళ (new_core == tcb->tcbAffinity) {
        తిరిగి;
    }
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
    tcbDebugRemove(tcb);
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_HAVE_FPU
    /* If the thread owns the FPU of the core it is currently running on (which
     * is not necessarily the core, that we are now running on), then release
     * that cores's FPU.
     */
    fpuRelease(tcb);
#అయితే_ముగింపు /* CONFIG_HAVE_FPU */
    tcb->tcbAffinity = new_core;
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
    tcbDebugAppend(tcb);
#అయితే_ముగింపు
}

#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
