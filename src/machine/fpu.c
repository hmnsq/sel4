/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <machine/fpu.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <arch/object/structures.h>

#ifdef CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
શૂન્ય switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    જો (ગાંઠ_STATE(ksCurFPUOwner)) {
        saveFpuState(ગાંઠ_STATE(ksCurFPUOwner));
    }
    જો (new_owner) {
        loadFpuState(new_owner);
    } નહીંતર {
        disableFpu();
    }
    ગાંઠ_STATE(ksCurFPUOwner) = new_owner;
}

શૂન્ય switchFpuOwner(tcb_t *new_owner, word_t cpu)
{
#ifdef ENABLE_SMP_SUPPORT
    જો (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } નહીંતર
#endif /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
શૂન્ય fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    જો (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(ખાલી, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#endif /* CONFIG_HAVE_FPU */
