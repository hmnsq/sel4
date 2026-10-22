/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/structures.h>

#ifdef CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
ଶୂନ୍ୟ switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    ଯଦି (ଗଣ୍ଠି_STATE(ksCurFPUOwner)) {
        saveFpuState(ଗଣ୍ଠି_STATE(ksCurFPUOwner));
    }
    ଯଦି (new_owner) {
        loadFpuState(new_owner);
    } ଅନ୍ୟଥା {
        disableFpu();
    }
    ଗଣ୍ଠି_STATE(ksCurFPUOwner) = new_owner;
}

ଶୂନ୍ୟ switchFpuOwner(tcb_t *new_owner, word_t cpu)
{
#ifdef ENABLE_SMP_SUPPORT
    ଯଦି (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } ଅନ୍ୟଥା
#endif /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
ଶୂନ୍ୟ fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    ଯଦି (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(ଖାଲି, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#endif /* CONFIG_HAVE_FPU */
