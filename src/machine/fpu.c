/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/structures.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
ਖਾਲੀ switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    ਜੇ (ਗੰਢ_STATE(ksCurFPUOwner)) {
        saveFpuState(ਗੰਢ_STATE(ksCurFPUOwner));
    }
    ਜੇ (new_owner) {
        ਲੱਦੋ_Fpu_State(new_owner);
    } ਨਹੀਂਤਾਂ {
        disableFpu();
    }
    ਗੰਢ_STATE(ksCurFPUOwner) = new_owner;
}

ਖਾਲੀ switchFpuOwner(tcb_t *new_owner, ਸ਼ਬਦ_t cpu)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਜੇ (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
ਖਾਲੀ fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    ਜੇ (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(ਨਿਰਾ, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#ਜੇ_ਅੰਤ /* CONFIG_HAVE_FPU */
