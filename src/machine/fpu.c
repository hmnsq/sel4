/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <machine/fpu.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <arch/object/structures.h>

#ifdef CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
ശൂന്യം switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    എങ്കിൽ (കണ്ണി_STATE(ksCurFPUOwner)) {
        saveFpuState(കണ്ണി_STATE(ksCurFPUOwner));
    }
    എങ്കിൽ (new_owner) {
        loadFpuState(new_owner);
    } അല്ലെങ്കിൽ {
        disableFpu();
    }
    കണ്ണി_STATE(ksCurFPUOwner) = new_owner;
}

ശൂന്യം switchFpuOwner(tcb_t *new_owner, word_t cpu)
{
#ifdef ENABLE_SMP_SUPPORT
    എങ്കിൽ (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } അല്ലെങ്കിൽ
#endif /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
ശൂന്യം fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    എങ്കിൽ (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(ശൂന്യ, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#endif /* CONFIG_HAVE_FPU */
