/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <model/smp.h>
#ضمن <object/tcb.h>

#که_تعریف_شوی ENABLE_SMP_SUPPORT

فراغ migrateTCB(tcb_t *tcb, کلمه_t new_core)
{
    إذا (new_core == tcb->tcbAffinity) {
        أرجع;
    }
#که_تعریف_شوی CONFIG_DEBUG_BUILD
    tcbDebugRemove(tcb);
#د_که_پای
#که_تعریف_شوی CONFIG_HAVE_FPU
    /* If the thread owns the FPU of the core it is currently running on (which
     * is not necessarily the core, that we are now running on), then release
     * that cores's FPU.
     */
    fpuRelease(tcb);
#د_که_پای /* CONFIG_HAVE_FPU */
    tcb->tcbAffinity = new_core;
#که_تعریف_شوی CONFIG_DEBUG_BUILD
    tcbDebugAppend(tcb);
#د_که_پای
}

#د_که_پای /* ENABLE_SMP_SUPPORT */
