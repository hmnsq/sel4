/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <model/smp.h>
#ضمن <object/tcb.h>

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT

فراغ migrateTCB(tcb_t *tcb, لفظ_t new_core)
{
    إذا (new_core == tcb->tcbAffinity) {
        أرجع;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
    tcbDebugRemove(tcb);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
    /* If the thread owns the FPU of the core it is currently running on (which
     * is not necessarily the core, that we are now running on), then release
     * that cores's FPU.
     */
    fpuRelease(tcb);
#جيڪڏهن_پورو /* CONFIG_HAVE_FPU */
    tcb->tcbAffinity = new_core;
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
    tcbDebugAppend(tcb);
#جيڪڏهن_پورو
}

#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
