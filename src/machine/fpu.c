/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <machine/fpu.h>
#ضمن <api/failures.h>
#ضمن <model/statedata.h>
#ضمن <arch/object/structures.h>

#که_تعریف_شوی CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
فراغ switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    إذا (غوټه_STATE(ksCurFPUOwner)) {
        saveFpuState(غوټه_STATE(ksCurFPUOwner));
    }
    إذا (new_owner) {
        بارول_Fpu_State(new_owner);
    } وإلا {
        disableFpu();
    }
    غوټه_STATE(ksCurFPUOwner) = new_owner;
}

فراغ switchFpuOwner(tcb_t *new_owner, کلمه_t cpu)
{
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    إذا (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } وإلا
#د_که_پای /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
فراغ fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    إذا (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(عدم, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#د_که_پای /* CONFIG_HAVE_FPU */
