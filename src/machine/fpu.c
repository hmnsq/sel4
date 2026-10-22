/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <machine/fpu.h>
#شامل <api/failures.h>
#شامل <model/statedata.h>
#شامل <arch/object/structures.h>

#ifdef CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
تهی switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    اگر (گره_STATE(ksCurFPUOwner)) {
        saveFpuState(گره_STATE(ksCurFPUOwner));
    }
    اگر (new_owner) {
        loadFpuState(new_owner);
    } وگرنه {
        disableFpu();
    }
    گره_STATE(ksCurFPUOwner) = new_owner;
}

تهی switchFpuOwner(tcb_t *new_owner, word_t cpu)
{
#ifdef ENABLE_SMP_SUPPORT
    اگر (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } وگرنه
#endif /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
تهی fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    اگر (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(پوچ, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#endif /* CONFIG_HAVE_FPU */
