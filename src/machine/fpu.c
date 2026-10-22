/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <machine/fpu.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <arch/object/structures.h>

#ifdef CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
வெறுமை switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    எனில் (முனை_STATE(ksCurFPUOwner)) {
        saveFpuState(முனை_STATE(ksCurFPUOwner));
    }
    எனில் (new_owner) {
        loadFpuState(new_owner);
    } இல்லையெனில் {
        disableFpu();
    }
    முனை_STATE(ksCurFPUOwner) = new_owner;
}

வெறுமை switchFpuOwner(tcb_t *new_owner, word_t cpu)
{
#ifdef ENABLE_SMP_SUPPORT
    எனில் (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } இல்லையெனில்
#endif /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
வெறுமை fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    எனில் (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(வெற்று, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#endif /* CONFIG_HAVE_FPU */
