/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <model/smp.h>
#شامل <object/tcb.h>

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT

خالی migrateTCB(tcb_t *tcb, لفظ_t new_core)
{
    اگر (new_core == tcb->tcbAffinity) {
        واپس;
    }
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
    tcbDebugRemove(tcb);
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    /* If the thread owns the FPU of the core it is currently running on (which
     * is not necessarily the core, that we are now running on), then release
     * that cores's FPU.
     */
    fpuRelease(tcb);
#اگر_ختم /* CONFIG_HAVE_FPU */
    tcb->tcbAffinity = new_core;
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
    tcbDebugAppend(tcb);
#اگر_ختم
}

#اگر_ختم /* ENABLE_SMP_SUPPORT */
