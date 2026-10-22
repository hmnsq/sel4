/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/smp.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/tcb.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT

ਖਾਲੀ migrateTCB(tcb_t *tcb, ਸ਼ਬਦ_t new_core)
{
    ਜੇ (new_core == tcb->tcbAffinity) {
        ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
    tcbDebugRemove(tcb);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
    /* If the thread owns the FPU of the core it is currently running on (which
     * is not necessarily the core, that we are now running on), then release
     * that cores's FPU.
     */
    fpuRelease(tcb);
#ਜੇ_ਅੰਤ /* CONFIG_HAVE_FPU */
    tcb->tcbAffinity = new_core;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
    tcbDebugAppend(tcb);
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
