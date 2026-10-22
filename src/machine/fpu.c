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

#ifdef CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
ಶೂನ್ಯ switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    ಯದಿ (ಗಂಟು_STATE(ksCurFPUOwner)) {
        saveFpuState(ಗಂಟು_STATE(ksCurFPUOwner));
    }
    ಯದಿ (new_owner) {
        loadFpuState(new_owner);
    } ಅನ್ಯಥಾ {
        disableFpu();
    }
    ಗಂಟು_STATE(ksCurFPUOwner) = new_owner;
}

ಶೂನ್ಯ switchFpuOwner(tcb_t *new_owner, word_t cpu)
{
#ifdef ENABLE_SMP_SUPPORT
    ಯದಿ (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } ಅನ್ಯಥಾ
#endif /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
ಶೂನ್ಯ fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    ಯದಿ (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(ರಿಕ್ತ, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#endif /* CONFIG_HAVE_FPU */
