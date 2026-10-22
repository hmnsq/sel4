/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <machine/fpu.h>
#include <api/failures.h>
#include <model/statedata.h>
#include <arch/object/structures.h>

#ifdef CONFIG_HAVE_FPU
/* Switch the owner of the FPU to the given thread on local core. */
void switchLocalFpuOwner(tcb_t *new_owner)
{
    enableFpu();
    ꦪꦼꦤ꧀ (NODE_STATE(ksCurFPUOwner)) {
        saveFpuState(NODE_STATE(ksCurFPUOwner));
    }
    ꦪꦼꦤ꧀ (new_owner) {
        loadFpuState(new_owner);
    } ꦭꦶꦪ {
        disableFpu();
    }
    NODE_STATE(ksCurFPUOwner) = new_owner;
}

void switchFpuOwner(tcb_t *new_owner, word_t cpu)
{
#ifdef ENABLE_SMP_SUPPORT
    ꦪꦼꦤ꧀ (cpu != getCurrentCPUIndex()) {
        doRemoteswitchFpuOwner(new_owner, cpu);
    } ꦭꦶꦪ
#endif /* ENABLE_SMP_SUPPORT */
    {
        switchLocalFpuOwner(new_owner);
    }
}

/* Prepare for the deletion of the given thread. */
void fpuRelease(tcb_t *thread)
{
    /* If the thread being deleted currently owns the FPU, switch away from it
     * so that 'ksCurFPUOwner' doesn't point to invalid memory. */
    ꦪꦼꦤ꧀ (nativeThreadUsingFPU(thread)) {
        switchFpuOwner(NULL, SMP_TERNARY(thread->tcbAffinity, 0));
    }
}
#endif /* CONFIG_HAVE_FPU */
