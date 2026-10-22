/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <machine/fpu.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <arch/object/structures.h>

#ifdef CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
শূন্য switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    যদি (গ্রন্থি_STATE(ksCurFPUOwner)) {
        saveFpuState(গ্রন্থি_STATE(ksCurFPUOwner));
    }
    যদি (new_owner) {
        loadFpuState(new_owner);
    } অন্যথা {
        disableFpu();
    }
    গ্রন্থি_STATE(ksCurFPUOwner) = new_owner;
}

শূন্য switchFpuOwner(tcb_t *new_owner, word_t cpu)
{
#ifdef ENABLE_SMP_SUPPORT
    যদি (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } অন্যথা
#endif /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
শূন্য fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    যদি (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(ফাঁকা, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#endif /* CONFIG_HAVE_FPU */
