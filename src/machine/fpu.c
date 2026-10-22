/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/object/structures.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
ಶೂನ್ಯ switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    ಆದರೆ (ಗಂಟು_STATE(ksCurFPUOwner)) {
        saveFpuState(ಗಂಟು_STATE(ksCurFPUOwner));
    }
    ಆದರೆ (new_owner) {
        ತುಂಬು_Fpu_State(new_owner);
    } ಇಲ್ಲದಿದ್ದರೆ {
        disableFpu();
    }
    ಗಂಟು_STATE(ksCurFPUOwner) = new_owner;
}

ಶೂನ್ಯ switchFpuOwner(tcb_t *new_owner, ಶಬ್ದ_t cpu)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಆದರೆ (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
ಶೂನ್ಯ fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    ಆದರೆ (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(ಖಾಲಿ, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HAVE_FPU */
