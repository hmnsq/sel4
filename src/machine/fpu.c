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

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
خالی switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    اگر (گرہ_STATE(ksCurFPUOwner)) {
        saveFpuState(گرہ_STATE(ksCurFPUOwner));
    }
    اگر (new_owner) {
        بھرو_Fpu_State(new_owner);
    } ورنہ {
        disableFpu();
    }
    گرہ_STATE(ksCurFPUOwner) = new_owner;
}

خالی switchFpuOwner(tcb_t *new_owner, لفظ_t cpu)
{
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    اگر (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } ورنہ
#اگر_ختم /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
خالی fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    اگر (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(تہی, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#اگر_ختم /* CONFIG_HAVE_FPU */
