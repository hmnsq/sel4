/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/invocation.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <sel4/shared_types.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
#ಅನ್ತರ್ಭಾವ <object/schedcontext.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <object/tcb.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <arch/smp/ipi_inline.h>

#ವ್ಯಾಖ್ಯಾನಿಸು NULL_PRIO 0

ಸ್ಥಿರ exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    ಪ್ರತಿಜ್ಞಾ(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    ಆದರೆ (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಕೂಡಿಸು_ಗಮ್ಯ_Bitmap(ಶಬ್ದ_t cpu, ಶಬ್ದ_t dom, ಶಬ್ದ_t prio)
{
    ಶಬ್ದ_t l1index;
    ಶಬ್ದ_t l1index_inverted;

    l1index = prio_ಗಮ್ಯ_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    ಗಂಟು_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= ಬಿಟ್(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    ಗಂಟು_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= ಬಿಟ್(prio & ಮುಖವಾಡ(ಶಬ್ದ_Radix));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ removeFromBitmap(ಶಬ್ದ_t cpu, ಶಬ್ದ_t dom, ಶಬ್ದ_t prio)
{
    ಶಬ್ದ_t l1index;
    ಶಬ್ದ_t l1index_inverted;

    l1index = prio_ಗಮ್ಯ_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    ಗಂಟು_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~ಬಿಟ್(prio & ಮುಖವಾಡ(ಶಬ್ದ_Radix));
    ಆದರೆ (unlikely(!ಗಂಟು_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        ಗಂಟು_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~ಬಿಟ್(l1index);
    }
}

tcb_ಸರತಿ_t tcb_ಸರತಿ_remove(tcb_ಸರತಿ_t ಸರತಿ, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcb_Sched_ಮುಂದಿನ;

    ಆದರೆ (ಸರತಿ.ತಲೆ == tcb && ಸರತಿ.ಕೊನೆ == tcb) {
        ಸರತಿ.ತಲೆ = ಖಾಲಿ;
        ಸರತಿ.ಕೊನೆ = ಖಾಲಿ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (ಸರತಿ.ತಲೆ == tcb) {
            after->tcbSchedPrev = ಖಾಲಿ;
            tcb->tcb_Sched_ಮುಂದಿನ = ಖಾಲಿ;
            ಸರತಿ.ತಲೆ = after;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (ಸರತಿ.ಕೊನೆ == tcb) {
                before->tcb_Sched_ಮುಂದಿನ = ಖಾಲಿ;
                tcb->tcbSchedPrev = ಖಾಲಿ;
                ಸರತಿ.ಕೊನೆ = before;
            } ಇಲ್ಲದಿದ್ದರೆ {
                before->tcb_Sched_ಮುಂದಿನ = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = ಖಾಲಿ;
                tcb->tcb_Sched_ಮುಂದಿನ = ಖಾಲಿ;
            }
        }
    }

    ಹಿಂತಿರುಗಿ ಸರತಿ;
}

/* Add TCB to the head of a scheduler queue */
ಶೂನ್ಯ tcb_Sched_ಸರತಿಗೆ_ಸೇರಿಸು(tcb_t *tcb)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಪ್ರತಿಜ್ಞಾ(isSchedulable(tcb));
    ಪ್ರತಿಜ್ಞಾ(refill_sufficient(tcb->tcbSchedContext, 0));
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_ಸರತಿ_t ಸರತಿ;
        dom_t dom;
        prio_t prio;
        ಶಬ್ದ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ಆದ್ಯತೆ;
        idx = ready_queues_ಸೂಚ್ಯಂಕ(dom, prio);
        ಸರತಿ = ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ಆದರೆ (tcb_ಸರತಿ_ಖಾಲಿಗಳು(ಸರತಿ)) {
            ಕೂಡಿಸು_ಗಮ್ಯ_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_ಸರತಿ_prepend(ಸರತಿ, tcb);

        thread_state_ptr_ಗಣ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
ಶೂನ್ಯ tcbSchedAppend(tcb_t *tcb)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಪ್ರತಿಜ್ಞಾ(isSchedulable(tcb));
    ಪ್ರತಿಜ್ಞಾ(refill_sufficient(tcb->tcbSchedContext, 0));
    ಪ್ರತಿಜ್ಞಾ(refill_ready(tcb->tcbSchedContext));
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_ಸರತಿ_t ಸರತಿ;
        dom_t dom;
        prio_t prio;
        ಶಬ್ದ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ಆದ್ಯತೆ;
        idx = ready_queues_ಸೂಚ್ಯಂಕ(dom, prio);
        ಸರತಿ = ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ಆದರೆ (tcb_ಸರತಿ_ಖಾಲಿಗಳು(ಸರತಿ)) {
            ಕೂಡಿಸು_ಗಮ್ಯ_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_ಸರತಿ_append(ಸರತಿ, tcb);

        thread_state_ptr_ಗಣ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
ಶೂನ್ಯ tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(tcb_t *tcb)
{
    ಆದರೆ (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_ಸರತಿ_t ಸರತಿ;
        tcb_ಸರತಿ_t new_ಸರತಿ;
        dom_t dom;
        prio_t prio;
        ಶಬ್ದ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ಆದ್ಯತೆ;
        idx = ready_queues_ಸೂಚ್ಯಂಕ(dom, prio);
        ಸರತಿ = ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_ಸರತಿ = tcb_ಸರತಿ_remove(ಸರತಿ, tcb);

        ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_ಸರತಿ;

        thread_state_ptr_ಗಣ_tcb_Queued(&tcb->tcbState, false);

        ಆದರೆ (likely(tcb_ಸರತಿ_ಖಾಲಿಗಳು(new_ಸರತಿ))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಶೂನ್ಯ tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = ಖಾಲಿ;

    debug_tcb->tcb_Debug_ಮುಂದಿನ = ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    ಆದರೆ (ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

ಶೂನ್ಯ tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != ಖಾಲಿ);
    ಆದರೆ (tcb == ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(ಗಂಟು_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcb_Debug_ಮುಂದಿನ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಪ್ರತಿಜ್ಞಾ(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcb_Debug_ಮುಂದಿನ = debug_tcb->tcb_Debug_ಮುಂದಿನ;
    }

    ಆದರೆ (debug_tcb->tcb_Debug_ಮುಂದಿನ) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_ಮುಂದಿನ)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = ಖಾಲಿ;
    debug_tcb->tcb_Debug_ಮುಂದಿನ = ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_ಸರತಿ_t tcbEPAppend(tcb_t *tcb, tcb_ಸರತಿ_t ಸರತಿ)
{
    ಆದರೆ (!ಸರತಿ.ತಲೆ) { /* Empty list */
        ಸರತಿ.ತಲೆ = tcb;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸರತಿ.ಕೊನೆ->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = ಸರತಿ.ಕೊನೆ;
    tcb->tcbEPNext = ಖಾಲಿ;
    ಸರತಿ.ಕೊನೆ = tcb;

    ಹಿಂತಿರುಗಿ ಸರತಿ;
}

/* Remove TCB from an endpoint queue */
tcb_ಸರತಿ_t tcbEPDequeue(tcb_t *tcb, tcb_ಸರತಿ_t ಸರತಿ)
{
    ಆದರೆ (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸರತಿ.ತಲೆ = tcb->tcbEPNext;
    }

    ಆದರೆ (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸರತಿ.ಕೊನೆ = tcb->tcbEPPrev;
    }

    ಹಿಂತಿರುಗಿ ಸರತಿ;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS

ಶೂನ್ಯ tcbReleaseRemove(tcb_t *tcb)
{
    ಆದರೆ (likely(thread_state_get_tcb_ಒಳ_Release_ಸರತಿ(tcb->tcbState))) {
        tcb_ಸರತಿ_t ಸರತಿ = ಗಂಟು_STATE_ON_CORE(ks_Release_ಸರತಿ, tcb->tcbAffinity);

        ಆದರೆ (ಸರತಿ.ತಲೆ == tcb) {
            ಗಂಟು_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        ಗಂಟು_STATE_ON_CORE(ks_Release_ಸರತಿ, tcb->tcbAffinity) = tcb_ಸರತಿ_remove(ಸರತಿ, tcb);

        thread_state_ptr_ಗಣ_tcb_ಒಳ_Release_ಸರತಿ(&tcb->tcbState, false);
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ticks_t ಶುದ್ಧ tcb_Ready_ಸಮಯ(tcb_t *tcb)
{
    ಹಿಂತಿರುಗಿ refill_ತಲೆ(tcb->tcbSchedContext)->r_ಸಮಯ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t ಶುದ್ಧ ಸಮಯ_after(tcb_t *tcb, ticks_t new_ಸಮಯ)
{
    ಹಿಂತಿರುಗಿ tcb != ಖಾಲಿ && new_ಸಮಯ >= tcb_Ready_ಸಮಯ(tcb);
}

ಸ್ಥಿರ tcb_t *ಕಂಡುಹಿಡಿ_ಸಮಯ_after(tcb_t *tcb, ticks_t new_ಸಮಯ)
{
    tcb_t *after = tcb;

    ಆಗ (ಸಮಯ_after(after, new_ಸಮಯ)) {
        after = after->tcb_Sched_ಮುಂದಿನ;
    }

    ಹಿಂತಿರುಗಿ after;
}

ಶೂನ್ಯ tcb_Release_ಸರತಿಗೆ_ಸೇರಿಸು(tcb_t *tcb)
{
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tcb_ಒಳ_Release_ಸರತಿ(tcb->tcbState) == false);
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_ಸಮಯ;
    tcb_ಸರತಿ_t ಸರತಿ;

    ಸರತಿ = ಗಂಟು_STATE_ON_CORE(ks_Release_ಸರತಿ, tcb->tcbAffinity);
    new_ಸಮಯ = tcb_Ready_ಸಮಯ(tcb);

    ಆದರೆ (tcb_ಸರತಿ_ಖಾಲಿಗಳು(ಸರತಿ) || new_ಸಮಯ < tcb_Ready_ಸಮಯ(ಸರತಿ.ತಲೆ)) {
        ಗಂಟು_STATE_ON_CORE(ks_Release_ಸರತಿ, tcb->tcbAffinity) = tcb_ಸರತಿ_prepend(ಸರತಿ, tcb);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (tcb_Ready_ಸಮಯ(ಸರತಿ.ಕೊನೆ) <= new_ಸಮಯ) {
            ಗಂಟು_STATE_ON_CORE(ks_Release_ಸರತಿ, tcb->tcbAffinity) = tcb_ಸರತಿ_append(ಸರತಿ, tcb);
        } ಇಲ್ಲದಿದ್ದರೆ {
            tcb_t *after;
            after = ಕಂಡುಹಿಡಿ_ಸಮಯ_after(ಸರತಿ.ತಲೆ, new_ಸಮಯ);
            tcb_ಸರತಿ_ಸೇರಿಸು(tcb, after);
        }
    }

    thread_state_ptr_ಗಣ_tcb_ಒಳ_Release_ಸರತಿ(&tcb->tcbState, true);

    ಆದರೆ (ಸರತಿ.ತಲೆ != ಗಂಟು_STATE_ON_CORE(ks_Release_ಸರತಿ, tcb->tcbAffinity).ತಲೆ) {
        ಗಂಟು_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

cptr_t ಶುದ್ಧ getExtraCPtr(ಶಬ್ದ_t *ಬಫರ್_Ptr, ಶಬ್ದ_t i)
{
    ಹಿಂತಿರುಗಿ (cptr_t)ಬಫರ್_Ptr[se_L4_ಸಂದೇಶ_Max_ಉದ್ದಳತೆ + 2 + i];
}

ಶೂನ್ಯ ಗಣ_Extra_Badge(ಶಬ್ದ_t *ಬಫರ್_Ptr, ಶಬ್ದ_t badge,
                   ಶಬ್ದ_t i)
{
    ಬಫರ್_Ptr[se_L4_ಸಂದೇಶ_Max_ಉದ್ದಳತೆ + 2 + i] = badge;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ ಸಿದ್ಧತೆ_Caller_Cap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *ಉತ್ತರ_ಸ್ಥಾನ, *caller_ಸ್ಥಾನ;
    cap_t masterCap ಬಳಸದ, callerCap ಬಳಸದ;

    ಗಣ_Thread_State(sender, Thread_State_Blocked_On_ಉತ್ತರ);
    ಉತ್ತರ_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(sender, tcb_ಉತ್ತರ);
    masterCap = ಉತ್ತರ_ಸ್ಥಾನ->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(masterCap) == cap_ಉತ್ತರ_cap);
    ಪ್ರತಿಜ್ಞಾ(cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Master(masterCap));
    ಪ್ರತಿಜ್ಞಾ(cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Can_Grant(masterCap));
    ಪ್ರತಿಜ್ಞಾ(TCB_PTR(cap_ಉತ್ತರ_cap_get_cap_TCBPtr(masterCap)) == sender);
    caller_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = caller_ಸ್ಥಾನ->cap;
    /* Haskell error: "Caller cap must not already exist" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(callerCap) == cap_null_cap);
    cte_ಸೇರಿಸು(cap_ಉತ್ತರ_cap_new(canGrant, false, TCB_REF(sender)),
              ಉತ್ತರ_ಸ್ಥಾನ, caller_ಸ್ಥಾನ);
}

ಶೂನ್ಯ deleteCallerCap(tcb_t *receiver)
{
    cte_t *caller_ಸ್ಥಾನ;

    caller_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(caller_ಸ್ಥಾನ);
}
#ಆದರೆ_ಅಂತ್ಯ

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, ಶಬ್ದ_t *ಬಫರ್_Ptr, se_L4_ಸಂದೇಶ_Info_t info)
{
    lookup_ಸ್ಥಾನ_raw_ret_t lu_ret;
    cptr_t cptr;
    ಶಬ್ದ_t i, ಉದ್ದಳತೆ;

    ಆದರೆ (!ಬಫರ್_Ptr) {
        current_extra_caps.excaprefs[0] = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    ಉದ್ದಳತೆ = se_L4_ಸಂದೇಶ_Info_get_extra_Caps(info);

    ಗಾಗಿ (i = 0; i < ಉದ್ದಳತೆ; i++) {
        cptr = getExtraCPtr(ಬಫರ್_Ptr, i);

        lu_ret = lookup_ಸ್ಥಾನ(thread, cptr);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }

        current_extra_caps.excaprefs[i] = lu_ret.ಸ್ಥಾನ;
    }
    ಆದರೆ (i < se_L4_ಸಂದೇಶ_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
ಶಬ್ದ_t copyMRs(tcb_t *sender, ಶಬ್ದ_t *sendBuf, tcb_t *receiver,
               ಶಬ್ದ_t *recvBuf, ಶಬ್ದ_t n)
{
    ಶಬ್ದ_t i;

    /* Copy inline words */
    ಗಾಗಿ (i = 0; i < n && i < n_ಸಂದೇಶ_Registers; i++) {
        ಗಣ_ರಿಜಿಸ್ಟರ್(receiver, ಸಂದೇಶ_Registers[i],
                    get_ರಿಜಿಸ್ಟರ್(sender, ಸಂದೇಶ_Registers[i]));
    }

    ಆದರೆ (!recvBuf || !sendBuf) {
        ಹಿಂತಿರುಗಿ i;
    }

    /* Copy out-of-line words */
    ಗಾಗಿ (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    ಹಿಂತಿರುಗಿ i;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
ಶೂನ್ಯ remote_ಸರತಿ_Update(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    ಆದರೆ (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *ಗುರಿ_Cur_Thread = ಗಂಟು_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        ಆದರೆ (ಗುರಿ_Cur_Thread == ಗಂಟು_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcb_ಆದ್ಯತೆ > ಗುರಿ_Cur_Thread->tcb_ಆದ್ಯತೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
            || ಗಂಟು_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#ಆದರೆ_ಅಂತ್ಯ
           ) {
            ARCH_ಗಂಟು_STATE(ipiReschedulePending) |= ಬಿಟ್(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
ಶೂನ್ಯ remoteTCBStall(tcb_t *tcb)
{

    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#ಆದರೆ_ಅಂತ್ಯ
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        ಗಂಟು_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_ಗಂಟು_STATE(ipiReschedulePending) |= ಬಿಟ್(tcb->tcbAffinity);
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ exception_t invoke_TCB_ಗಣ_Affinity(tcb_t *thread, ಶಬ್ದ_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(thread);
    migrateTCB(thread, affinity);
    ಆದರೆ (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    ಆದರೆ (thread == ಗಂಟು_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decode_ಗಣ_Affinity(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    tcb_t *tcb;
    ಶಬ್ದ_t affinity;

    ಆದರೆ (ಉದ್ದಳತೆ < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, ಬಫರ್);
    ಆದರೆ (affinity >= ks_ಅಂಕೆ_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invoke_TCB_ಗಣ_Affinity(tcb, affinity);
}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
ಸ್ಥಿರ exception_t invokeConfigureSingleStepping(bool_t call, ಶಬ್ದ_t *ಬಫರ್, tcb_t *t,
                                                 uint16_t bp_ಅಂಕೆ, ಶಬ್ದ_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    ಶಬ್ದ_t ಮೌಲ್ಯ;

    bp_was_consumed = configureSingleStepping(t, bp_ಅಂಕೆ, n_instrs, false);
    ಆದರೆ (n_instrs == 0) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARCH_AARCH64
        unset_Breakpoint_ಬಳಸಿದ_Flag(t, bp_ಅಂಕೆ);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARCH_AARCH64 */
        ಮೌಲ್ಯ = false;
    } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARCH_AARCH64
        ಗಣ_Breakpoint_ಬಳಸಿದ_Flag(t, bp_ಅಂಕೆ);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARCH_AARCH64 */
        ಮೌಲ್ಯ = bp_was_consumed;
    }

    ಆದರೆ (call) {
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ಗಣ_MR(thread, ಬಫರ್, 0, ಮೌಲ್ಯ);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    uint16_t bp_ಅಂಕೆ;
    ಶಬ್ದ_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_ಅಂಕೆ = getSyscallArg(0, ಬಫರ್);
    n_instrs = getSyscallArg(1, ಬಫರ್);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_ಅಂಕೆ, n_instrs, false);
    ಆದರೆ (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeConfigureSingleStepping(call, ಬಫರ್, tcb, bp_ಅಂಕೆ, n_instrs);
}

ಸ್ಥಿರ exception_t invoke_ಗಣ_Breakpoint(tcb_t *tcb, uint16_t bp_ಅಂಕೆ,
                                       ಶಬ್ದ_t vaddr, ಶಬ್ದ_t type, ಶಬ್ದ_t ಗಾತ್ರ_2, ಶಬ್ದ_t rw)
{
    ಗಣ_Breakpoint(tcb, bp_ಅಂಕೆ, vaddr, type, ಗಾತ್ರ_2, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    ಗಣ_Breakpoint_ಬಳಸಿದ_Flag(tcb, bp_ಅಂಕೆ);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decode_ಗಣ_Breakpoint(cap_t cap, ಶಬ್ದ_t *ಬಫರ್)
{
    uint16_t bp_ಅಂಕೆ;
    ಶಬ್ದ_t vaddr, type, ಗಾತ್ರ_2, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ಅಂಕೆ = getSyscallArg(0, ಬಫರ್);
    vaddr = getSyscallArg(1, ಬಫರ್);
    type = getSyscallArg(2, ಬಫರ್);
    ಗಾತ್ರ_2 = getSyscallArg(3, ಬಫರ್);
    rw = getSyscallArg(4, ಬಫರ್);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    ಆದರೆ (vaddr >= (ಶಬ್ದ_t)USER_ತುದಿ) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (type != seL4_InstructionBreakpoint && type != se_L4_ದತ್ತಾಂಶ_Breakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 2;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == seL4_InstructionBreakpoint) {
        ಆದರೆ (ಗಾತ್ರ_2 != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 3;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಆದರೆ (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 4;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಆದರೆ ((se_L4_ಮೊದಲ_Watchpoint == -1 || bp_ಅಂಕೆ >= se_L4_ಮೊದಲ_Watchpoint)
            && se_L4_ಮೊದಲ_Breakpoint != se_L4_ಮೊದಲ_Watchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 2;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == se_L4_ದತ್ತಾಂಶ_Breakpoint) {
        ಆದರೆ (ಗಾತ್ರ_2 == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 3;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಆದರೆ (se_L4_ಮೊದಲ_Watchpoint != -1 && bp_ಅಂಕೆ < se_L4_ಮೊದಲ_Watchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 2;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 2;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 3;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (ಗಾತ್ರ_2 != 0 && ಗಾತ್ರ_2 != 1 && ಗಾತ್ರ_2 != 2 && ಗಾತ್ರ_2 != 4 && ಗಾತ್ರ_2 != 8) {
        userError("Debug: Invalid size %lu.", ಗಾತ್ರ_2);
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 3;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (ಗಾತ್ರ_2 > 0 && vaddr & (ಗಾತ್ರ_2 - 1)) {
        /* Just Don't allow unaligned watchpoints. They are undefined
         * both ARM and x86.
         *
         * X86: Intel manuals, vol3, 17.2.5:
         *  "Two-byte ranges must be aligned on word boundaries; 4-byte
         *   ranges must be aligned on doubleword boundaries"
         *  "Unaligned data or I/O breakpoint addresses do not yield valid
         *   results"
         *
         * ARM: ARMv7 manual, C11.11.44:
         *  "A DBGWVR is programmed with a word-aligned address."
         */
        userError("Debug: Unaligned data watchpoint address %lx (size %lx) "
                  "rejected.\n",
                  vaddr, ಗಾತ್ರ_2);

        current_syscall_error.type = seL4_AlignmentError;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decode_ಗಣ_Breakpoint(tcb, bp_ಅಂಕೆ, vaddr, type, ಗಾತ್ರ_2, rw);
    ಆದರೆ (error.type != seL4_NoError) {
        current_syscall_error = error;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invoke_ಗಣ_Breakpoint(tcb, bp_ಅಂಕೆ,
                               vaddr, type, ಗಾತ್ರ_2, rw);
}

ಸ್ಥಿರ exception_t invokeGetBreakpoint(bool_t call, ಶಬ್ದ_t *ಬಫರ್, tcb_t *tcb, uint16_t bp_ಅಂಕೆ)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_ಅಂಕೆ);
    ಆದರೆ (call) {
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, 0, res.vaddr);
        ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, 1, res.type);
        ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, 2, res.ಗಾತ್ರ_2);
        ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, 3, res.rw);
        ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, 4, res.is_enabled);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, 5)));
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeGetBreakpoint(cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    tcb_t *tcb;
    uint16_t bp_ಅಂಕೆ;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ಅಂಕೆ = getSyscallArg(0, ಬಫರ್);

    error = Arch_decodeGetBreakpoint(tcb, bp_ಅಂಕೆ);
    ಆದರೆ (error.type != seL4_NoError) {
        current_syscall_error = error;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeGetBreakpoint(call, ಬಫರ್, tcb, bp_ಅಂಕೆ);
}

ಸ್ಥಿರ exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_ಅಂಕೆ)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_ಅಂಕೆ);
    unset_Breakpoint_ಬಳಸಿದ_Flag(tcb, bp_ಅಂಕೆ);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeUnsetBreakpoint(cap_t cap, ಶಬ್ದ_t *ಬಫರ್)
{
    tcb_t *tcb;
    uint16_t bp_ಅಂಕೆ;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ಅಂಕೆ = getSyscallArg(0, ಬಫರ್);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_ಅಂಕೆ);
    ಆದರೆ (error.type != seL4_NoError) {
        current_syscall_error = error;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeUnsetBreakpoint(tcb, bp_ಅಂಕೆ);
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HARDWARE_DEBUG_API */

ಸ್ಥಿರ exception_t invoke_ಗಣ_TLSBase(tcb_t *thread, ಶಬ್ದ_t tls_ತಳ)
{
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, TLS_ತಳ, tls_ತಳ);
    ಆದರೆ (thread == ಗಂಟು_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decode_ಗಣ_TLSBase(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t tls_ತಳ;

    ಆದರೆ (ಉದ್ದಳತೆ < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    tls_ತಳ = getSyscallArg(0, ಬಫರ್);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invoke_ಗಣ_TLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_ತಳ);
}

ಸ್ಥಿರ ಶೂನ್ಯ invoke_ಗಣ_Flags(tcb_t *thread, ಶಬ್ದ_t clear, ಶಬ್ದ_t ಗಣ, bool_t call)
{
    tcb_t *cur_thread = ಗಂಟು_STATE(ksCurThread);
    ಶಬ್ದ_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= ಗಣ & se_L4_TCBFlag_ಮುಖವಾಡ;
    thread->tcbFlags = flags;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
    ಆದರೆ (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (call) {
        ಶಬ್ದ_t *ipc_ಬಫರ್ = lookupIPCBuffer(true, cur_thread);
        ಗಣ_ರಿಜಿಸ್ಟರ್(cur_thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ಗಣ_MR(cur_thread, ipc_ಬಫರ್, 0, flags);
        ಗಣ_ರಿಜಿಸ್ಟರ್(cur_thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
    }
    ಗಣ_Thread_State(cur_thread, ThreadState_Running);
}

ಸ್ಥಿರ exception_t decode_ಗಣ_Flags(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ಆದರೆ (ಉದ್ದಳತೆ < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಶಬ್ದ_t clear = getSyscallArg(0, ಬಫರ್);
    ಶಬ್ದ_t ಗಣ   = getSyscallArg(1, ಬಫರ್);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    invoke_ಗಣ_Flags(thread, clear, ಗಣ, call);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ, cap_t cap,
                                cte_t *ಸ್ಥಾನ, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ TCBReadRegisters:
        /* Second level of decoding */
        ಹಿಂತಿರುಗಿ decodeReadRegisters(cap, ಉದ್ದಳತೆ, call, ಬಫರ್);

    ಸ್ಥಿತಿ TCBWriteRegisters:
        ಹಿಂತಿರುಗಿ decodeWriteRegisters(cap, ಉದ್ದಳತೆ, ಬಫರ್);

    ಸ್ಥಿತಿ TCBCopyRegisters:
        ಹಿಂತಿರುಗಿ decodeCopyRegisters(cap, ಉದ್ದಳತೆ, ಬಫರ್);

    ಸ್ಥಿತಿ TCBSuspend:
        /* Jump straight to the invoke */
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    ಸ್ಥಿತಿ TCBResume:
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    ಸ್ಥಿತಿ TCBConfigure:
        ಹಿಂತಿರುಗಿ decodeTCBConfigure(cap, ಉದ್ದಳತೆ, ಸ್ಥಾನ, ಬಫರ್);

    ಸ್ಥಿತಿ TCBSet_ಆದ್ಯತೆ:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_ಆದ್ಯತೆ(cap, ಉದ್ದಳತೆ, ಬಫರ್);

    ಸ್ಥಿತಿ TCBSetMCPriority:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_MCPriority(cap, ಉದ್ದಳತೆ, ಬಫರ್);

    ಸ್ಥಿತಿ TCBSetSchedParams:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಹಿಂತಿರುಗಿ decode_ಗಣ_Sched_Params(cap, ಉದ್ದಳತೆ, ಸ್ಥಾನ, ಬಫರ್);
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ decode_ಗಣ_Sched_Params(cap, ಉದ್ದಳತೆ, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ TCBSetIPCBuffer:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_IPCBuffer(cap, ಉದ್ದಳತೆ, ಸ್ಥಾನ, ಬಫರ್);

    ಸ್ಥಿತಿ TCBSetSpace:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_Space(cap, ಉದ್ದಳತೆ, ಸ್ಥಾನ, ಬಫರ್);

    ಸ್ಥಿತಿ TCBBindNotification:
        ಹಿಂತಿರುಗಿ decodeBindNotification(cap);

    ಸ್ಥಿತಿ TCBUnbindNotification:
        ಹಿಂತಿರುಗಿ decodeUnbindNotification(cap);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ TCBSetTimeoutEndpoint:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_Timeout_Endpoint(cap, ಸ್ಥಾನ);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಸ್ಥಿತಿ TCBSetAffinity:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_Affinity(cap, ಉದ್ದಳತೆ, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
#ಆದರೆ_ಅಂತ್ಯ

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಸ್ಥಿತಿ TCBSetEPTRoot:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_EPTRoot(cap);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    ಸ್ಥಿತಿ TCBConfigureSingleStepping:
        ಹಿಂತಿರುಗಿ decodeConfigureSingleStepping(cap, call, ಬಫರ್);

    ಸ್ಥಿತಿ TCBSetBreakpoint:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_Breakpoint(cap, ಬಫರ್);

    ಸ್ಥಿತಿ TCBGetBreakpoint:
        ಹಿಂತಿರುಗಿ decodeGetBreakpoint(cap, call, ಬಫರ್);

    ಸ್ಥಿತಿ TCBUnsetBreakpoint:
        ಹಿಂತಿರುಗಿ decodeUnsetBreakpoint(cap, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ TCBSetTLSBase:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_TLSBase(cap, ಉದ್ದಳತೆ, ಬಫರ್);

    ಸ್ಥಿತಿ TCBSetFlags:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_Flags(cap, ಉದ್ದಳತೆ, call, ಬಫರ್);

    ಪೂರ್ವನಿಯೋಜಿತ:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

ಎಣಿಕೆ CopyRegistersFlags {
    Copy_Registers_suspend_ಮೂಲ = 0,
    Copy_Registers_resume_ಗುರಿ = 1,
    Copy_Registers_transfer_ಚೌಕಟ್ಟು = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t transferArch;
    tcb_t *srcTCB;
    cap_t ಮೂಲ_cap;
    ಶಬ್ದ_t flags;

    ಆದರೆ (ಉದ್ದಳತೆ < 1 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, ಬಫರ್);

    transferArch = Arch_decodeTransfer(flags >> 8);

    ಮೂಲ_cap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cap_get_capType(ಮೂಲ_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(ಮೂಲ_cap));
    } ಇಲ್ಲದಿದ್ದರೆ {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & ಬಿಟ್(Copy_Registers_suspend_ಮೂಲ),
               flags & ಬಿಟ್(Copy_Registers_resume_ಗುರಿ),
               flags & ಬಿಟ್(Copy_Registers_transfer_ಚೌಕಟ್ಟು),
               flags & ಬಿಟ್(CopyRegisters_transferInteger),
               transferArch);

}

ಎಣಿಕೆ ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, bool_t call,
                                ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t transferArch, flags, n;
    tcb_t *thread;

    ಆದರೆ (ಉದ್ದಳತೆ < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, ಬಫರ್);
    n     = getSyscallArg(1, ಬಫರ್);

    ಆದರೆ (n < 1 || n > n_ಚೌಕಟ್ಟು_Registers + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (ಪೂರ್ಣಾಂಕ)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_ಚೌಕಟ್ಟು_Registers +
                                              n_gpRegisters;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ಆದರೆ (thread == ಗಂಟು_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & ಬಿಟ್(ReadRegisters_suspend),
               n, transferArch, call);
}

ಎಣಿಕೆ WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t flags, w;
    ಶಬ್ದ_t transferArch;
    tcb_t *thread;

    ಆದರೆ (ಉದ್ದಳತೆ < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, ಬಫರ್);
    w     = getSyscallArg(1, ಬಫರ್);

    ಆದರೆ (ಉದ್ದಳತೆ - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (ಪೂರ್ಣಾಂಕ)(ಉದ್ದಳತೆ - 2), (ಪೂರ್ಣಾಂಕ)w);
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ಆದರೆ (thread == ಗಂಟು_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeTCB_WriteRegisters(thread,
                                    flags & ಬಿಟ್(WriteRegisters_resume),
                                    w, transferArch, ಬಫರ್);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಹಿಂತಿರುಗಿ (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_cap_Can_Grant_ಉತ್ತರ(cap)));
    ಸ್ಥಿತಿ cap_null_cap:
        /* just has no fault endpoint */
        ಹಿಂತಿರುಗಿ true;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, cte_t *ಸ್ಥಾನ, ಶಬ್ದ_t *ಬಫರ್)
{
    cte_t *ಬಫರ್_ಸ್ಥಾನ, *c_ಬೇರು_ಸ್ಥಾನ, *v_ಬೇರು_ಸ್ಥಾನ;
    cap_t ಬಫರ್_Cap, c_ಬೇರು_Cap, v_ಬೇರು_Cap;
    deriveCap_ret_t dc_ret;
    ಶಬ್ದ_t c_ಬೇರು_ದತ್ತಾಂಶ, v_ಬೇರು_ದತ್ತಾಂಶ, ಬಫರ್_Addr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
#ವ್ಯಾಖ್ಯಾನಿಸು TCBCONFIGURE_ವಾದಗಳು 3
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು TCBCONFIGURE_ವಾದಗಳು 4
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಉದ್ದಳತೆ < TCBCONFIGURE_ವಾದಗಳು || current_extra_caps.excaprefs[0] == ಖಾಲಿ
        || current_extra_caps.excaprefs[1] == ಖಾಲಿ
        || current_extra_caps.excaprefs[2] == ಖಾಲಿ) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    c_ಬೇರು_ದತ್ತಾಂಶ     = getSyscallArg(0, ಬಫರ್);
    v_ಬೇರು_ದತ್ತಾಂಶ     = getSyscallArg(1, ಬಫರ್);
    ಬಫರ್_Addr    = getSyscallArg(2, ಬಫರ್);
#ಇಲ್ಲದಿದ್ದರೆ
    cptr_t faultEP       = getSyscallArg(0, ಬಫರ್);
    c_ಬೇರು_ದತ್ತಾಂಶ     = getSyscallArg(1, ಬಫರ್);
    v_ಬೇರು_ದತ್ತಾಂಶ     = getSyscallArg(2, ಬಫರ್);
    ಬಫರ್_Addr    = getSyscallArg(3, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ

    c_ಬೇರು_ಸ್ಥಾನ  = current_extra_caps.excaprefs[0];
    c_ಬೇರು_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ಬೇರು_ಸ್ಥಾನ  = current_extra_caps.excaprefs[1];
    v_ಬೇರು_Cap   = current_extra_caps.excaprefs[1]->cap;
    ಬಫರ್_ಸ್ಥಾನ = current_extra_caps.excaprefs[2];
    ಬಫರ್_Cap  = current_extra_caps.excaprefs[2]->cap;

    ಆದರೆ (ಬಫರ್_Addr == 0) {
        ಬಫರ್_ಸ್ಥಾನ = ಖಾಲಿ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        dc_ret = deriveCap(ಬಫರ್_ಸ್ಥಾನ, ಬಫರ್_Cap);
        ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ dc_ret.ಸ್ಥಿತಿ_2;
        }
        ಬಫರ್_Cap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(ಬಫರ್_Addr, ಬಫರ್_Cap);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }
    }

    ಆದರೆ (ಸ್ಥಾನ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        ಸ್ಥಾನ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (c_ಬೇರು_ದತ್ತಾಂಶ != 0) {
        c_ಬೇರು_Cap = update_Cap_ದತ್ತಾಂಶ(false, c_ಬೇರು_ದತ್ತಾಂಶ, c_ಬೇರು_Cap);
    }

    dc_ret = deriveCap(c_ಬೇರು_ಸ್ಥಾನ, c_ಬೇರು_Cap);
    ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ dc_ret.ಸ್ಥಿತಿ_2;
    }
    c_ಬೇರು_Cap = dc_ret.cap;

    ಆದರೆ (cap_get_capType(c_ಬೇರು_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (v_ಬೇರು_ದತ್ತಾಂಶ != 0) {
        v_ಬೇರು_Cap = update_Cap_ದತ್ತಾಂಶ(false, v_ಬೇರು_ದತ್ತಾಂಶ, v_ಬೇರು_Cap);
    }

    dc_ret = deriveCap(v_ಬೇರು_ಸ್ಥಾನ, v_ಬೇರು_Cap);
    ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ dc_ret.ಸ್ಥಿತಿ_2;
    }
    v_ಬೇರು_Cap = dc_ret.cap;

    ಆದರೆ (!is_Valid_VTable_ಬೇರು(v_ಬೇರು_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಸ್ಥಾನ,
               cap_null_cap_new(), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               c_ಬೇರು_Cap, c_ಬೇರು_ಸ್ಥಾನ,
               v_ಬೇರು_Cap, v_ಬೇರು_ಸ್ಥಾನ,
               ಬಫರ್_Addr, ಬಫರ್_Cap,
               ಬಫರ್_ಸ್ಥಾನ, thread_control_caps_update_space |
               thread_control_caps_update_ipc_ಬಫರ್);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಸ್ಥಾನ,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_ಬೇರು_Cap, c_ಬೇರು_ಸ್ಥಾನ,
               v_ಬೇರು_Cap, v_ಬೇರು_ಸ್ಥಾನ,
               ಬಫರ್_Addr, ಬಫರ್_Cap,
               ಬಫರ್_ಸ್ಥಾನ, thread_control_update_space |
               thread_control_update_ipc_ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ
}

exception_t decode_ಗಣ_ಆದ್ಯತೆ(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆದರೆ (ಉದ್ದಳತೆ < 1 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, ಬಫರ್);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t ಸ್ಥಿತಿ_2 = checkPrio(newPrio, authTCB);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) newPrio, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) authTCB->tcbMCP);
        ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               NULL_PRIO, newPrio,
               ಖಾಲಿ, thread_control_sched_update_ಆದ್ಯತೆ);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಖಾಲಿ,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               0, cap_null_cap_new(),
               ಖಾಲಿ, thread_control_update_ಆದ್ಯತೆ);
#ಆದರೆ_ಅಂತ್ಯ
}

exception_t decode_ಗಣ_MCPriority(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆದರೆ (ಉದ್ದಳತೆ < 1 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, ಬಫರ್);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t ಸ್ಥಿತಿ_2 = checkPrio(newMcp, authTCB);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) newMcp, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) authTCB->tcbMCP);
        ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               newMcp, NULL_PRIO,
               ಖಾಲಿ, thread_control_sched_update_mcp);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಖಾಲಿ,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               0, cap_null_cap_new(),
               ಖಾಲಿ, thread_control_update_mcp);
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
exception_t decode_ಗಣ_Timeout_Endpoint(cap_t cap, cte_t *ಸ್ಥಾನ)
{
    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *th_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    ಆದರೆ (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಸ್ಥಾನ,
               cap_null_cap_new(), ಖಾಲಿ,
               thCap, th_ಸ್ಥಾನ,
               cap_null_cap_new(), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               0, cap_null_cap_new(), ಖಾಲಿ,
               thread_control_caps_update_timeout);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
exception_t decode_ಗಣ_Sched_Params(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, cte_t *ಸ್ಥಾನ, ಶಬ್ದ_t *ಬಫರ್)
#ಇಲ್ಲದಿದ್ದರೆ
exception_t decode_ಗಣ_Sched_Params(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
#ಆದರೆ_ಅಂತ್ಯ
{
    ಆದರೆ (ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == ಖಾಲಿ || current_extra_caps.excaprefs[2] == ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, ಬಫರ್);
    prio_t newPrio = getSyscallArg(1, ಬಫರ್);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fh_ಸ್ಥಾನ = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t ಸ್ಥಿತಿ_2 = checkPrio(newMcp, authTCB);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) newMcp, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) authTCB->tcbMCP);
        ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }

    ಸ್ಥಿತಿ_2 = checkPrio(newPrio, authTCB);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) newPrio, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) authTCB->tcbMCP);
        ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = ಖಾಲಿ;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_ಆದ್ಯತೆ |
                                         thread_control_sched_update_fault;
    ಆಯ್ಕೆ (cap_get_capType(scCap)) {
    ಸ್ಥಿತಿ cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        ಆದರೆ (tcb->tcbSchedContext != sc) {
            ಆದರೆ (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
            ಆದರೆ (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        }
        ಆದರೆ (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_null_cap:
        ಆದರೆ (tcb == ಗಂಟು_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 2;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    ಆದರೆ (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    ಆದರೆ (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 3;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಸ್ಥಾನ,
               fhCap, fh_ಸ್ಥಾನ,
               newMcp, newPrio,
               sc, update_flags);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಖಾಲಿ,
               0, newMcp, newPrio,
               cap_null_cap_new(), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               0, cap_null_cap_new(),
               ಖಾಲಿ, thread_control_update_mcp |
               thread_control_update_ಆದ್ಯತೆ);
#ಆದರೆ_ಅಂತ್ಯ
}


exception_t decode_ಗಣ_IPCBuffer(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, cte_t *ಸ್ಥಾನ, ಶಬ್ದ_t *ಬಫರ್)
{
    cptr_t cptr_ಬಫರ್_Ptr;
    cap_t ಬಫರ್_Cap;
    cte_t *ಬಫರ್_ಸ್ಥಾನ;

    ಆದರೆ (ಉದ್ದಳತೆ < 1 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    cptr_ಬಫರ್_Ptr  = getSyscallArg(0, ಬಫರ್);
    ಬಫರ್_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
    ಬಫರ್_Cap  = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cptr_ಬಫರ್_Ptr == 0) {
        ಬಫರ್_ಸ್ಥಾನ = ಖಾಲಿ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(ಬಫರ್_ಸ್ಥಾನ, ಬಫರ್_Cap);
        ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ dc_ret.ಸ್ಥಿತಿ_2;
        }
        ಬಫರ್_Cap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_ಬಫರ್_Ptr, ಬಫರ್_Cap);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಸ್ಥಾನ,
               cap_null_cap_new(), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               cptr_ಬಫರ್_Ptr, ಬಫರ್_Cap,
               ಬಫರ್_ಸ್ಥಾನ, thread_control_caps_update_ipc_ಬಫರ್);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಸ್ಥಾನ,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), ಖಾಲಿ,
               cap_null_cap_new(), ಖಾಲಿ,
               cptr_ಬಫರ್_Ptr, ಬಫರ್_Cap,
               ಬಫರ್_ಸ್ಥಾನ, thread_control_update_ipc_ಬಫರ್);

#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
#ವ್ಯಾಖ್ಯಾನಿಸು DECODE_ಗಣ_SPACE_PARAMS 2
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು DECODE_ಗಣ_SPACE_PARAMS 3
#ಆದರೆ_ಅಂತ್ಯ
exception_t decode_ಗಣ_Space(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, cte_t *ಸ್ಥಾನ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t c_ಬೇರು_ದತ್ತಾಂಶ, v_ಬೇರು_ದತ್ತಾಂಶ;
    cte_t *c_ಬೇರು_ಸ್ಥಾನ, *v_ಬೇರು_ಸ್ಥಾನ;
    cap_t c_ಬೇರು_Cap, v_ಬೇರು_Cap;
    deriveCap_ret_t dc_ret;

    ಆದರೆ (ಉದ್ದಳತೆ < DECODE_ಗಣ_SPACE_PARAMS || current_extra_caps.excaprefs[0] == ಖಾಲಿ
        || current_extra_caps.excaprefs[1] == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    c_ಬೇರು_ದತ್ತಾಂಶ = getSyscallArg(0, ಬಫರ್);
    v_ಬೇರು_ದತ್ತಾಂಶ = getSyscallArg(1, ಬಫರ್);

    cte_t *fh_ಸ್ಥಾನ     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_ಬೇರು_ಸ್ಥಾನ  = current_extra_caps.excaprefs[1];
    c_ಬೇರು_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_ಬೇರು_ಸ್ಥಾನ  = current_extra_caps.excaprefs[2];
    v_ಬೇರು_Cap   = current_extra_caps.excaprefs[2]->cap;
#ಇಲ್ಲದಿದ್ದರೆ
    cptr_t faultEP   = getSyscallArg(0, ಬಫರ್);
    c_ಬೇರು_ದತ್ತಾಂಶ = getSyscallArg(1, ಬಫರ್);
    v_ಬೇರು_ದತ್ತಾಂಶ = getSyscallArg(2, ಬಫರ್);

    c_ಬೇರು_ಸ್ಥಾನ  = current_extra_caps.excaprefs[0];
    c_ಬೇರು_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ಬೇರು_ಸ್ಥಾನ  = current_extra_caps.excaprefs[1];
    v_ಬೇರು_Cap   = current_extra_caps.excaprefs[1]->cap;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಸ್ಥಾನ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        ಸ್ಥಾನ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (c_ಬೇರು_ದತ್ತಾಂಶ != 0) {
        c_ಬೇರು_Cap = update_Cap_ದತ್ತಾಂಶ(false, c_ಬೇರು_ದತ್ತಾಂಶ, c_ಬೇರು_Cap);
    }

    dc_ret = deriveCap(c_ಬೇರು_ಸ್ಥಾನ, c_ಬೇರು_Cap);
    ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ dc_ret.ಸ್ಥಿತಿ_2;
    }
    c_ಬೇರು_Cap = dc_ret.cap;

    ಆದರೆ (cap_get_capType(c_ಬೇರು_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (v_ಬೇರು_ದತ್ತಾಂಶ != 0) {
        v_ಬೇರು_Cap = update_Cap_ದತ್ತಾಂಶ(false, v_ಬೇರು_ದತ್ತಾಂಶ, v_ಬೇರು_Cap);
    }

    dc_ret = deriveCap(v_ಬೇರು_ಸ್ಥಾನ, v_ಬೇರು_Cap);
    ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ dc_ret.ಸ್ಥಿತಿ_2;
    }
    v_ಬೇರು_Cap = dc_ret.cap;

    ಆದರೆ (!is_Valid_VTable_ಬೇರು(v_ಬೇರು_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* fault handler */
    ಆದರೆ (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಸ್ಥಾನ,
               fhCap, fh_ಸ್ಥಾನ,
               cap_null_cap_new(), ಖಾಲಿ,
               c_ಬೇರು_Cap, c_ಬೇರು_ಸ್ಥಾನ,
               v_ಬೇರು_Cap, v_ಬೇರು_ಸ್ಥಾನ,
               0, cap_null_cap_new(), ಖಾಲಿ, thread_control_caps_update_space | thread_control_caps_update_fault);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ಸ್ಥಾನ,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_ಬೇರು_Cap, c_ಬೇರು_ಸ್ಥಾನ,
               v_ಬೇರು_Cap, v_ಬೇರು_ಸ್ಥಾನ,
               0, cap_null_cap_new(), ಖಾಲಿ, thread_control_update_space);
#ಆದರೆ_ಅಂತ್ಯ
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ಆದರೆ (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } ಇಲ್ಲದಿದ್ದರೆ {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ ((tcb_t *)notification_ptr_get_ntfn_ಸರತಿ_ತಲೆ(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }


    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ಆದರೆ (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeTCB_NotificationControl(tcb, ಖಾಲಿ);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಸಾಲೊಳಗೆ exception_t installTCBCap(tcb_t *ಗುರಿ, cap_t tCap, cte_t *ಸ್ಥಾನ,
                                        tcb_cnode_ಸೂಚ್ಯಂಕ_t ಸೂಚ್ಯಂಕ, cap_t newCap, cte_t *src_ಸ್ಥಾನ)
{
    cte_t *ಬೇರು_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(ಗುರಿ, ಸೂಚ್ಯಂಕ);
    ಬಳಸದ exception_t e = cteDelete(ಬೇರು_ಸ್ಥಾನ, true);
    ಆದರೆ (e != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    ಆದರೆ (cap_get_capType(newCap) != cap_null_cap) {
        ಆದರೆ (sameObjectAs(newCap, src_ಸ್ಥಾನ->cap) &&
            sameObjectAs(tCap, ಸ್ಥಾನ->cap)) {
            cte_ಸೇರಿಸು(newCap, src_ಸ್ಥಾನ, ಬೇರು_ಸ್ಥಾನ);
        }
    }
    ಹಿಂತಿರುಗಿ e;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *ಗುರಿ, cte_t *ಸ್ಥಾನ,
                                        cap_t fh_newCap, cte_t *fh_src_ಸ್ಥಾನ,
                                        cap_t th_newCap, cte_t *th_src_ಸ್ಥಾನ,
                                        cap_t c_ಬೇರು_new_Cap, cte_t *c_ಬೇರು_src_ಸ್ಥಾನ,
                                        cap_t v_ಬೇರು_new_Cap, cte_t *v_ಬೇರು_src_ಸ್ಥಾನ,
                                        ಶಬ್ದ_t ಬಫರ್_Addr, cap_t ಬಫರ್_Cap,
                                        cte_t *ಬಫರ್_Src_ಸ್ಥಾನ,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((ಶಬ್ದ_t)ಗುರಿ);

    ಆದರೆ (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(ಗುರಿ, tCap, ಸ್ಥಾನ, tcbFaultHandler, fh_newCap, fh_src_ಸ್ಥಾನ);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }

    }

    ಆದರೆ (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(ಗುರಿ, tCap, ಸ್ಥಾನ, tcbTimeoutHandler, th_newCap, th_src_ಸ್ಥಾನ);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }
    }

    ಆದರೆ (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(ಗುರಿ, tCap, ಸ್ಥಾನ, tcbCTable, c_ಬೇರು_new_Cap, c_ಬೇರು_src_ಸ್ಥಾನ);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }

        e = installTCBCap(ಗುರಿ, tCap, ಸ್ಥಾನ, tcbVTable, v_ಬೇರು_new_Cap, v_ಬೇರು_src_ಸ್ಥಾನ);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }
    }

    ಆದರೆ (updateFlags & thread_control_caps_update_ipc_ಬಫರ್) {
        cte_t *ಬಫರ್_ಸ್ಥಾನ;

        ಬಫರ್_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(ಗುರಿ, tcb_ಬಫರ್);
        e = cteDelete(ಬಫರ್_ಸ್ಥಾನ, true);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }
        ಗುರಿ->tcbIPCBuffer = ಬಫರ್_Addr;

        ಆದರೆ (ಬಫರ್_Src_ಸ್ಥಾನ && sameObjectAs(ಬಫರ್_Cap, ಬಫರ್_Src_ಸ್ಥಾನ->cap) &&
            sameObjectAs(tCap, ಸ್ಥಾನ->cap)) {
            cte_ಸೇರಿಸು(ಬಫರ್_Cap, ಬಫರ್_Src_ಸ್ಥಾನ, ಬಫರ್_ಸ್ಥಾನ);
        }

        ಆದರೆ (ಗುರಿ == ಗಂಟು_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಇಲ್ಲದಿದ್ದರೆ
exception_t invokeTCB_ThreadControl(tcb_t *ಗುರಿ, cte_t *ಸ್ಥಾನ,
                                    cptr_t faultep, prio_t mcp, prio_t ಆದ್ಯತೆ,
                                    cap_t c_ಬೇರು_new_Cap, cte_t *c_ಬೇರು_src_ಸ್ಥಾನ,
                                    cap_t v_ಬೇರು_new_Cap, cte_t *v_ಬೇರು_src_ಸ್ಥಾನ,
                                    ಶಬ್ದ_t ಬಫರ್_Addr, cap_t ಬಫರ್_Cap,
                                    cte_t *ಬಫರ್_Src_ಸ್ಥಾನ,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((ಶಬ್ದ_t)ಗುರಿ);

    ಆದರೆ (updateFlags & thread_control_update_space) {
        ಗುರಿ->tcbFaultHandler = faultep;
    }

    ಆದರೆ (updateFlags & thread_control_update_mcp) {
        ಗಣ_MCPriority(ಗುರಿ, mcp);
    }

    ಆದರೆ (updateFlags & thread_control_update_space) {
        cte_t *ಬೇರು_ಸ್ಥಾನ;

        ಬೇರು_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(ಗುರಿ, tcbCTable);
        e = cteDelete(ಬೇರು_ಸ್ಥಾನ, true);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }
        ಆದರೆ (sameObjectAs(c_ಬೇರು_new_Cap, c_ಬೇರು_src_ಸ್ಥಾನ->cap) &&
            sameObjectAs(tCap, ಸ್ಥಾನ->cap)) {
            cte_ಸೇರಿಸು(c_ಬೇರು_new_Cap, c_ಬೇರು_src_ಸ್ಥಾನ, ಬೇರು_ಸ್ಥಾನ);
        }
    }

    ಆದರೆ (updateFlags & thread_control_update_space) {
        cte_t *ಬೇರು_ಸ್ಥಾನ;

        ಬೇರು_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(ಗುರಿ, tcbVTable);
        e = cteDelete(ಬೇರು_ಸ್ಥಾನ, true);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }
        ಆದರೆ (sameObjectAs(v_ಬೇರು_new_Cap, v_ಬೇರು_src_ಸ್ಥಾನ->cap) &&
            sameObjectAs(tCap, ಸ್ಥಾನ->cap)) {
            cte_ಸೇರಿಸು(v_ಬೇರು_new_Cap, v_ಬೇರು_src_ಸ್ಥಾನ, ಬೇರು_ಸ್ಥಾನ);
        }
    }

    ಆದರೆ (updateFlags & thread_control_update_ipc_ಬಫರ್) {
        cte_t *ಬಫರ್_ಸ್ಥಾನ;

        ಬಫರ್_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(ಗುರಿ, tcb_ಬಫರ್);
        e = cteDelete(ಬಫರ್_ಸ್ಥಾನ, true);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }
        ಗುರಿ->tcbIPCBuffer = ಬಫರ್_Addr;

        ಆದರೆ (ಬಫರ್_Src_ಸ್ಥಾನ && sameObjectAs(ಬಫರ್_Cap, ಬಫರ್_Src_ಸ್ಥಾನ->cap) &&
            sameObjectAs(tCap, ಸ್ಥಾನ->cap)) {
            cte_ಸೇರಿಸು(ಬಫರ್_Cap, ಬಫರ್_Src_ಸ್ಥಾನ, ಬಫರ್_ಸ್ಥಾನ);
        }

        ಆದರೆ (ಗುರಿ == ಗಂಟು_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ಆದರೆ (updateFlags & thread_control_update_ಆದ್ಯತೆ) {
        ಗಣ_ಆದ್ಯತೆ(ಗುರಿ, ಆದ್ಯತೆ);
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *ಗುರಿ, cte_t *ಸ್ಥಾನ,
                                         cap_t fh_newCap, cte_t *fh_src_ಸ್ಥಾನ,
                                         prio_t mcp, prio_t ಆದ್ಯತೆ,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    ಆದರೆ (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((ಶಬ್ದ_t)ಗುರಿ);
        exception_t e = installTCBCap(ಗುರಿ, tCap, ಸ್ಥಾನ, tcbFaultHandler, fh_newCap, fh_src_ಸ್ಥಾನ);
        ಆದರೆ (e != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ e;
        }
    }

    ಆದರೆ (updateFlags & thread_control_sched_update_mcp) {
        ಗಣ_MCPriority(ಗುರಿ, mcp);
    }

    ಆದರೆ (updateFlags & thread_control_sched_update_ಆದ್ಯತೆ) {
        ಗಣ_ಆದ್ಯತೆ(ಗುರಿ, ಆದ್ಯತೆ);
    }

    ಆದರೆ (updateFlags & thread_control_sched_update_sc) {
        ಆದರೆ (sc != ಖಾಲಿ) {
            schedContext_bindTCB(sc, ಗುರಿ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sc == ಖಾಲಿ) {
            schedContext_unbindTCB(ಗುರಿ->tcbSchedContext);
        }
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspend_ಮೂಲ, bool_t resume_ಗುರಿ,
                                    bool_t transfer_ಚೌಕಟ್ಟು, bool_t transferInteger,
                                    ಶಬ್ದ_t transferArch)
{
    ಆದರೆ (suspend_ಮೂಲ) {
        suspend(tcb_src);
    }

    ಆದರೆ (resume_ಗುರಿ) {
        restart(dest);
    }

    ಆದರೆ (transfer_ಚೌಕಟ್ಟು) {
        ಶಬ್ದ_t i;
        ಶಬ್ದ_t v;
        ಶಬ್ದ_t pc;

        ಗಾಗಿ (i = 0; i < n_ಚೌಕಟ್ಟು_Registers; i++) {
            v = get_ರಿಜಿಸ್ಟರ್(tcb_src, ಚೌಕಟ್ಟು_Registers[i]);
            ಗಣ_ರಿಜಿಸ್ಟರ್(dest, ಚೌಕಟ್ಟು_Registers[i], v);
        }

        pc = getRestartPC(dest);
        ಗಣ_ಮುಂದಿನ_PC(dest, pc);
    }

    ಆದರೆ (transferInteger) {
        ಶಬ್ದ_t i;
        ಶಬ್ದ_t v;

        ಗಾಗಿ (i = 0; i < n_gpRegisters; i++) {
            v = get_ರಿಜಿಸ್ಟರ್(tcb_src, gpRegisters[i]);
            ಗಣ_ರಿಜಿಸ್ಟರ್(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    ಆದರೆ (dest == ಗಂಟು_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    ಹಿಂತಿರುಗಿ Arch_performTransfer(transferArch, tcb_src, dest);
}

/* ReadRegisters is a special case: replyFromKernel & setMRs are
 * unfolded here, in order to avoid passing the large reply message up
 * to the top level in a global (and double-copying). We prevent the
 * top-level replyFromKernel_success_empty() from running by setting the
 * thread state. Retype does this too.
 */
exception_t invokeTCB_ReadRegisters(tcb_t *tcb_src, bool_t suspend_ಮೂಲ,
                                    ಶಬ್ದ_t n, ಶಬ್ದ_t arch, bool_t call)
{
    ಶಬ್ದ_t i, j;
    exception_t e;
    tcb_t *thread;

    thread = ಗಂಟು_STATE(ksCurThread);

    ಆದರೆ (suspend_ಮೂಲ) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, ಗಂಟು_STATE(ksCurThread));
    ಆದರೆ (e != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ e;
    }

    ಆದರೆ (call) {
        ಶಬ್ದ_t *ipc_ಬಫರ್;

        ipc_ಬಫರ್ = lookupIPCBuffer(true, thread);

        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);

        ಗಾಗಿ (i = 0; i < n && i < n_ಚೌಕಟ್ಟು_Registers && i < n_ಸಂದೇಶ_Registers; i++) {
            ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Registers[i],
                        get_ರಿಜಿಸ್ಟರ್(tcb_src, ಚೌಕಟ್ಟು_Registers[i]));
        }

        ಆದರೆ (ipc_ಬಫರ್ != ಖಾಲಿ && i < n && i < n_ಚೌಕಟ್ಟು_Registers) {
            ಗಾಗಿ (; i < n && i < n_ಚೌಕಟ್ಟು_Registers; i++) {
                ipc_ಬಫರ್[i + 1] = get_ರಿಜಿಸ್ಟರ್(tcb_src, ಚೌಕಟ್ಟು_Registers[i]);
            }
        }

        j = i;

        ಗಾಗಿ (i = 0; i < n_gpRegisters && i + n_ಚೌಕಟ್ಟು_Registers < n
             && i + n_ಚೌಕಟ್ಟು_Registers < n_ಸಂದೇಶ_Registers; i++) {
            ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Registers[i + n_ಚೌಕಟ್ಟು_Registers],
                        get_ರಿಜಿಸ್ಟರ್(tcb_src, gpRegisters[i]));
        }

        ಆದರೆ (ipc_ಬಫರ್ != ಖಾಲಿ && i < n_gpRegisters
            && i + n_ಚೌಕಟ್ಟು_Registers < n) {
            ಗಾಗಿ (; i < n_gpRegisters && i + n_ಚೌಕಟ್ಟು_Registers < n; i++) {
                ipc_ಬಫರ್[i + n_ಚೌಕಟ್ಟು_Registers + 1] =
                    get_ರಿಜಿಸ್ಟರ್(tcb_src, gpRegisters[i]);
            }
        }

        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, i + j)));
    }
    ಗಣ_Thread_State(thread, ThreadState_Running);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resume_ಗುರಿ,
                                     ಶಬ್ದ_t n, ಶಬ್ದ_t arch, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t i;
    ಶಬ್ದ_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, ಗಂಟು_STATE(ksCurThread), dest);
    ಆದರೆ (e != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ e;
    }

    ಆದರೆ (n > n_ಚೌಕಟ್ಟು_Registers + n_gpRegisters) {
        n = n_ಚೌಕಟ್ಟು_Registers + n_gpRegisters;
    }

    archInfo = Arch_get_Sanitise_ರಿಜಿಸ್ಟರ್_Info(dest);

    ಗಾಗಿ (i = 0; i < n_ಚೌಕಟ್ಟು_Registers && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        ಗಣ_ರಿಜಿಸ್ಟರ್(dest, ಚೌಕಟ್ಟು_Registers[i],
                    sanitise_ರಿಜಿಸ್ಟರ್(ಚೌಕಟ್ಟು_Registers[i],
                                     getSyscallArg(i + 2, ಬಫರ್), archInfo));
    }

    ಗಾಗಿ (i = 0; i < n_gpRegisters && i + n_ಚೌಕಟ್ಟು_Registers < n; i++) {
        ಗಣ_ರಿಜಿಸ್ಟರ್(dest, gpRegisters[i],
                    sanitise_ರಿಜಿಸ್ಟರ್(gpRegisters[i],
                                     getSyscallArg(i + n_ಚೌಕಟ್ಟು_Registers + 2,
                                                   ಬಫರ್), archInfo));
    }

    pc = getRestartPC(dest);
    ಗಣ_ಮುಂದಿನ_PC(dest, pc);

    Arch_postModifyRegisters(dest);

    ಆದರೆ (resume_ಗುರಿ) {
        restart(dest);
    }

    ಆದರೆ (dest == ಗಂಟು_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    ಆದರೆ (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } ಇಲ್ಲದಿದ್ದರೆ {
        unbindNotification(tcb);
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಶೂನ್ಯ ಗಣ_Thread_Name(tcb_t *tcb, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_ಉದ್ದಳತೆ);
}
#ಆದರೆ_ಅಂತ್ಯ

ಶಬ್ದ_t ಗಣ_MRs_syscall_error(tcb_t *thread, ಶಬ್ದ_t *receiveIPCBuffer)
{
    ಆಯ್ಕೆ (current_syscall_error.type) {
    ಸ್ಥಿತಿ se_L4_Invalid_ವಾದ:
        ಹಿಂತಿರುಗಿ ಗಣ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ);

    ಸ್ಥಿತಿ seL4_InvalidCapability:
        ಹಿಂತಿರುಗಿ ಗಣ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ);

    ಸ್ಥಿತಿ seL4_IllegalOperation:
        ಹಿಂತಿರುಗಿ 0;

    ಸ್ಥಿತಿ seL4_RangeError:
        ಗಣ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        ಹಿಂತಿರುಗಿ ಗಣ_MR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    ಸ್ಥಿತಿ seL4_AlignmentError:
        ಹಿಂತಿರುಗಿ 0;

    ಸ್ಥಿತಿ seL4_FailedLookup:
        ಗಣ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failed_Lookup_Was_ಮೂಲ ? 1 : 0);
        ಹಿಂತಿರುಗಿ ಗಣ_MRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    ಸ್ಥಿತಿ se_L4_Truncated_ಸಂದೇಶ:
    ಸ್ಥಿತಿ se_L4_Delete_ಮೊದಲ:
    ಸ್ಥಿತಿ se_L4_Revoke_ಮೊದಲ:
        ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಿತಿ se_L4_Not_Enough_ಸ್ಮರಣೆ:
        ಹಿಂತಿರುಗಿ ಗಣ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.ಸ್ಮರಣೆ_ಎಡ);
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid syscall error");
    }
}
