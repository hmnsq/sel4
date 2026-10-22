/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <model/smp.h>
#ಅನ್ತರ್ಭಾವ <object/tcb.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT

ಶೂನ್ಯ migrateTCB(tcb_t *tcb, ಶಬ್ದ_t new_core)
{
    ಆದರೆ (new_core == tcb->tcbAffinity) {
        ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
    tcbDebugRemove(tcb);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
    /* If the thread owns the FPU of the core it is currently running on (which
     * is not necessarily the core, that we are now running on), then release
     * that cores's FPU.
     */
    fpuRelease(tcb);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HAVE_FPU */
    tcb->tcbAffinity = new_core;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
    tcbDebugAppend(tcb);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
