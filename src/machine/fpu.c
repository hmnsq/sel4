/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/object/structures.h>

#यदि_परिभाषितम् CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
शून्य switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    यदि (ग्रन्थिः_STATE(ksCurFPUOwner)) {
        saveFpuState(ग्रन्थिः_STATE(ksCurFPUOwner));
    }
    यदि (new_owner) {
        आरोपय_Fpu_State(new_owner);
    } अन्यथा {
        disableFpu();
    }
    ग्रन्थिः_STATE(ksCurFPUOwner) = new_owner;
}

शून्य switchFpuOwner(tcb_t *new_owner, word_t cpu)
{
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    यदि (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } अन्यथा
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
शून्य fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    यदि (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(रिक्त, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#यदि_अन्तः /* CONFIG_HAVE_FPU */
