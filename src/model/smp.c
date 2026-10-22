/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <model/smp.h>
#ضمن <object/tcb.h>

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT

فراغ migrateTCB(tcb_t *tcb, لفظ_t new_core)
{
    إذا (new_core == tcb->tcbAffinity) {
        أرجع;
    }
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
    tcbDebugRemove(tcb);
#جے_انت
#جے_تعریف_شدہ CONFIG_HAVE_FPU
    /* If the thread owns the FPU of the core it is currently running on (which
     * is not necessarily the core, that we are now running on), then release
     * that cores's FPU.
     */
    fpuRelease(tcb);
#جے_انت /* CONFIG_HAVE_FPU */
    tcb->tcbAffinity = new_core;
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
    tcbDebugAppend(tcb);
#جے_انت
}

#جے_انت /* ENABLE_SMP_SUPPORT */
