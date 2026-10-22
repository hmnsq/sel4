/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/object/structures.h>

#నిర్వచితమైతే CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
శూన్య switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    ఒకవేళ (కణుపు_STATE(ksCurFPUOwner)) {
        saveFpuState(కణుపు_STATE(ksCurFPUOwner));
    }
    ఒకవేళ (new_owner) {
        ఎక్కించు_Fpu_State(new_owner);
    } లేకపోతే {
        disableFpu();
    }
    కణుపు_STATE(ksCurFPUOwner) = new_owner;
}

శూన్య switchFpuOwner(tcb_t *new_owner, మాట_t cpu)
{
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    ఒకవేళ (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } లేకపోతే
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
శూన్య fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    ఒకవేళ (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(ఖాళీ, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#అయితే_ముగింపు /* CONFIG_HAVE_FPU */
