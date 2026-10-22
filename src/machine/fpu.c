/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <machine/fpu.h>
#समावेश <api/failures.h>
#समावेश <model/statedata.h>
#समावेश <arch/object/structures.h>

#अगर_घोषित CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
व्योम switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    अगर (गाँठ_STATE(ksCurFPUOwner)) {
        saveFpuState(गाँठ_STATE(ksCurFPUOwner));
    }
    अगर (new_owner) {
        loadFpuState(new_owner);
    } अन्यथा {
        disableFpu();
    }
    गाँठ_STATE(ksCurFPUOwner) = new_owner;
}

व्योम switchFpuOwner(tcb_t *new_owner, word_t cpu)
{
#अगर_घोषित ENABLE_SMP_SUPPORT
    अगर (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } अन्यथा
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
व्योम fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    अगर (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(शून्य, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#पूर्ण_अगर /* CONFIG_HAVE_FPU */
