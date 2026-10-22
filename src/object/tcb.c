/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sel4/shared_types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/objecttype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/cnode.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਅਨ੍ਤਰ੍ਭਾਵ <object/schedcontext.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <object/tcb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/smp/ipi_inline.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NULL_PRIO 0

ਸਥਿਰ exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    ਪ੍ਰਤਿਜ੍ਞਾ(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    ਜੇ (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਜੋੜੋ_ਮੰਜ਼ਿਲ_Bitmap(ਸ਼ਬਦ_t cpu, ਸ਼ਬਦ_t dom, ਸ਼ਬਦ_t prio)
{
    ਸ਼ਬਦ_t l1index;
    ਸ਼ਬਦ_t l1index_inverted;

    l1index = prio_ਮੰਜ਼ਿਲ_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    ਗੰਢ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= ਦੋਆਧਾਰੀ_ਅੰਕ(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    ਗੰਢ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= ਦੋਆਧਾਰੀ_ਅੰਕ(prio & ਨਕਾਬ(ਸ਼ਬਦ_Radix));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ removeFromBitmap(ਸ਼ਬਦ_t cpu, ਸ਼ਬਦ_t dom, ਸ਼ਬਦ_t prio)
{
    ਸ਼ਬਦ_t l1index;
    ਸ਼ਬਦ_t l1index_inverted;

    l1index = prio_ਮੰਜ਼ਿਲ_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    ਗੰਢ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(prio & ਨਕਾਬ(ਸ਼ਬਦ_Radix));
    ਜੇ (unlikely(!ਗੰਢ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        ਗੰਢ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(l1index);
    }
}

tcb_ਕਤਾਰ_t tcb_ਕਤਾਰ_remove(tcb_ਕਤਾਰ_t ਕਤਾਰ, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcb_Sched_ਅਗਲਾ;

    ਜੇ (ਕਤਾਰ.ਸਿਰ == tcb && ਕਤਾਰ.ਸਿਰਾ == tcb) {
        ਕਤਾਰ.ਸਿਰ = ਨਿਰਾ;
        ਕਤਾਰ.ਸਿਰਾ = ਨਿਰਾ;
    } ਨਹੀਂਤਾਂ {
        ਜੇ (ਕਤਾਰ.ਸਿਰ == tcb) {
            after->tcbSchedPrev = ਨਿਰਾ;
            tcb->tcb_Sched_ਅਗਲਾ = ਨਿਰਾ;
            ਕਤਾਰ.ਸਿਰ = after;
        } ਨਹੀਂਤਾਂ {
            ਜੇ (ਕਤਾਰ.ਸਿਰਾ == tcb) {
                before->tcb_Sched_ਅਗਲਾ = ਨਿਰਾ;
                tcb->tcbSchedPrev = ਨਿਰਾ;
                ਕਤਾਰ.ਸਿਰਾ = before;
            } ਨਹੀਂਤਾਂ {
                before->tcb_Sched_ਅਗਲਾ = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = ਨਿਰਾ;
                tcb->tcb_Sched_ਅਗਲਾ = ਨਿਰਾ;
            }
        }
    }

    ਵਾਪਸ ਕਤਾਰ;
}

/* Add TCB to the head of a scheduler queue */
ਖਾਲੀ tcb_Sched_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(tcb_t *tcb)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਪ੍ਰਤਿਜ੍ਞਾ(isSchedulable(tcb));
    ਪ੍ਰਤਿਜ੍ਞਾ(refill_sufficient(tcb->tcbSchedContext, 0));
#ਜੇ_ਅੰਤ

    ਜੇ (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_ਕਤਾਰ_t ਕਤਾਰ;
        dom_t dom;
        prio_t prio;
        ਸ਼ਬਦ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ਤਰਜੀਹ;
        idx = ready_queues_ਸੂਚਕਾਂਕ(dom, prio);
        ਕਤਾਰ = ਗੰਢ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ਜੇ (tcb_ਕਤਾਰ_ਖਾਲੀ_ਥਾਵਾਂ(ਕਤਾਰ)) {
            ਜੋੜੋ_ਮੰਜ਼ਿਲ_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ਗੰਢ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_ਕਤਾਰ_prepend(ਕਤਾਰ, tcb);

        thread_state_ptr_ਸੈੱਟ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
ਖਾਲੀ tcbSchedAppend(tcb_t *tcb)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਪ੍ਰਤਿਜ੍ਞਾ(isSchedulable(tcb));
    ਪ੍ਰਤਿਜ੍ਞਾ(refill_sufficient(tcb->tcbSchedContext, 0));
    ਪ੍ਰਤਿਜ੍ਞਾ(refill_ready(tcb->tcbSchedContext));
#ਜੇ_ਅੰਤ
    ਜੇ (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_ਕਤਾਰ_t ਕਤਾਰ;
        dom_t dom;
        prio_t prio;
        ਸ਼ਬਦ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ਤਰਜੀਹ;
        idx = ready_queues_ਸੂਚਕਾਂਕ(dom, prio);
        ਕਤਾਰ = ਗੰਢ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ਜੇ (tcb_ਕਤਾਰ_ਖਾਲੀ_ਥਾਵਾਂ(ਕਤਾਰ)) {
            ਜੋੜੋ_ਮੰਜ਼ਿਲ_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ਗੰਢ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_ਕਤਾਰ_append(ਕਤਾਰ, tcb);

        thread_state_ptr_ਸੈੱਟ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
ਖਾਲੀ tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(tcb_t *tcb)
{
    ਜੇ (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_ਕਤਾਰ_t ਕਤਾਰ;
        tcb_ਕਤਾਰ_t new_ਕਤਾਰ;
        dom_t dom;
        prio_t prio;
        ਸ਼ਬਦ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ਤਰਜੀਹ;
        idx = ready_queues_ਸੂਚਕਾਂਕ(dom, prio);
        ਕਤਾਰ = ਗੰਢ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_ਕਤਾਰ = tcb_ਕਤਾਰ_remove(ਕਤਾਰ, tcb);

        ਗੰਢ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_ਕਤਾਰ;

        thread_state_ptr_ਸੈੱਟ_tcb_Queued(&tcb->tcbState, false);

        ਜੇ (likely(tcb_ਕਤਾਰ_ਖਾਲੀ_ਥਾਵਾਂ(new_ਕਤਾਰ))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਖਾਲੀ tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = ਨਿਰਾ;

    debug_tcb->tcb_Debug_ਅਗਲਾ = ਗੰਢ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    ਜੇ (ਗੰਢ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(ਗੰਢ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    ਗੰਢ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

ਖਾਲੀ tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    ਪ੍ਰਤਿਜ੍ਞਾ(ਗੰਢ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != ਨਿਰਾ);
    ਜੇ (tcb == ਗੰਢ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        ਗੰਢ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(ਗੰਢ_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcb_Debug_ਅਗਲਾ;
    } ਨਹੀਂਤਾਂ {
        ਪ੍ਰਤਿਜ੍ਞਾ(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcb_Debug_ਅਗਲਾ = debug_tcb->tcb_Debug_ਅਗਲਾ;
    }

    ਜੇ (debug_tcb->tcb_Debug_ਅਗਲਾ) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_ਅਗਲਾ)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = ਨਿਰਾ;
    debug_tcb->tcb_Debug_ਅਗਲਾ = ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_ਕਤਾਰ_t tcbEPAppend(tcb_t *tcb, tcb_ਕਤਾਰ_t ਕਤਾਰ)
{
    ਜੇ (!ਕਤਾਰ.ਸਿਰ) { /* Empty list */
        ਕਤਾਰ.ਸਿਰ = tcb;
    } ਨਹੀਂਤਾਂ {
        ਕਤਾਰ.ਸਿਰਾ->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = ਕਤਾਰ.ਸਿਰਾ;
    tcb->tcbEPNext = ਨਿਰਾ;
    ਕਤਾਰ.ਸਿਰਾ = tcb;

    ਵਾਪਸ ਕਤਾਰ;
}

/* Remove TCB from an endpoint queue */
tcb_ਕਤਾਰ_t tcbEPDequeue(tcb_t *tcb, tcb_ਕਤਾਰ_t ਕਤਾਰ)
{
    ਜੇ (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } ਨਹੀਂਤਾਂ {
        ਕਤਾਰ.ਸਿਰ = tcb->tcbEPNext;
    }

    ਜੇ (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } ਨਹੀਂਤਾਂ {
        ਕਤਾਰ.ਸਿਰਾ = tcb->tcbEPPrev;
    }

    ਵਾਪਸ ਕਤਾਰ;
}
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS

ਖਾਲੀ tcbReleaseRemove(tcb_t *tcb)
{
    ਜੇ (likely(thread_state_get_tcb_ਵਿੱਚ_Release_ਕਤਾਰ(tcb->tcbState))) {
        tcb_ਕਤਾਰ_t ਕਤਾਰ = ਗੰਢ_STATE_ON_CORE(ks_Release_ਕਤਾਰ, tcb->tcbAffinity);

        ਜੇ (ਕਤਾਰ.ਸਿਰ == tcb) {
            ਗੰਢ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        ਗੰਢ_STATE_ON_CORE(ks_Release_ਕਤਾਰ, tcb->tcbAffinity) = tcb_ਕਤਾਰ_remove(ਕਤਾਰ, tcb);

        thread_state_ptr_ਸੈੱਟ_tcb_ਵਿੱਚ_Release_ਕਤਾਰ(&tcb->tcbState, false);
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ticks_t ਸ਼ੁੱਧ tcb_Ready_ਸਮਾਂ(tcb_t *tcb)
{
    ਵਾਪਸ refill_ਸਿਰ(tcb->tcbSchedContext)->r_ਸਮਾਂ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t ਸ਼ੁੱਧ ਸਮਾਂ_after(tcb_t *tcb, ticks_t new_ਸਮਾਂ)
{
    ਵਾਪਸ tcb != ਨਿਰਾ && new_ਸਮਾਂ >= tcb_Ready_ਸਮਾਂ(tcb);
}

ਸਥਿਰ tcb_t *ਲੱਭੋ_ਸਮਾਂ_after(tcb_t *tcb, ticks_t new_ਸਮਾਂ)
{
    tcb_t *after = tcb;

    ਜਦੋਂ (ਸਮਾਂ_after(after, new_ਸਮਾਂ)) {
        after = after->tcb_Sched_ਅਗਲਾ;
    }

    ਵਾਪਸ after;
}

ਖਾਲੀ tcb_Release_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(tcb_t *tcb)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tcb_ਵਿੱਚ_Release_ਕਤਾਰ(tcb->tcbState) == false);
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_ਸਮਾਂ;
    tcb_ਕਤਾਰ_t ਕਤਾਰ;

    ਕਤਾਰ = ਗੰਢ_STATE_ON_CORE(ks_Release_ਕਤਾਰ, tcb->tcbAffinity);
    new_ਸਮਾਂ = tcb_Ready_ਸਮਾਂ(tcb);

    ਜੇ (tcb_ਕਤਾਰ_ਖਾਲੀ_ਥਾਵਾਂ(ਕਤਾਰ) || new_ਸਮਾਂ < tcb_Ready_ਸਮਾਂ(ਕਤਾਰ.ਸਿਰ)) {
        ਗੰਢ_STATE_ON_CORE(ks_Release_ਕਤਾਰ, tcb->tcbAffinity) = tcb_ਕਤਾਰ_prepend(ਕਤਾਰ, tcb);
    } ਨਹੀਂਤਾਂ {
        ਜੇ (tcb_Ready_ਸਮਾਂ(ਕਤਾਰ.ਸਿਰਾ) <= new_ਸਮਾਂ) {
            ਗੰਢ_STATE_ON_CORE(ks_Release_ਕਤਾਰ, tcb->tcbAffinity) = tcb_ਕਤਾਰ_append(ਕਤਾਰ, tcb);
        } ਨਹੀਂਤਾਂ {
            tcb_t *after;
            after = ਲੱਭੋ_ਸਮਾਂ_after(ਕਤਾਰ.ਸਿਰ, new_ਸਮਾਂ);
            tcb_ਕਤਾਰ_ਪਾਓ(tcb, after);
        }
    }

    thread_state_ptr_ਸੈੱਟ_tcb_ਵਿੱਚ_Release_ਕਤਾਰ(&tcb->tcbState, true);

    ਜੇ (ਕਤਾਰ.ਸਿਰ != ਗੰਢ_STATE_ON_CORE(ks_Release_ਕਤਾਰ, tcb->tcbAffinity).ਸਿਰ) {
        ਗੰਢ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#ਜੇ_ਅੰਤ

cptr_t ਸ਼ੁੱਧ getExtraCPtr(ਸ਼ਬਦ_t *ਬਫਰ_Ptr, ਸ਼ਬਦ_t i)
{
    ਵਾਪਸ (cptr_t)ਬਫਰ_Ptr[se_L4_ਸੁਨੇਹਾ_Max_ਲੰਮਾਈ + 2 + i];
}

ਖਾਲੀ ਸੈੱਟ_Extra_Badge(ਸ਼ਬਦ_t *ਬਫਰ_Ptr, ਸ਼ਬਦ_t badge,
                   ਸ਼ਬਦ_t i)
{
    ਬਫਰ_Ptr[se_L4_ਸੁਨੇਹਾ_Max_ਲੰਮਾਈ + 2 + i] = badge;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ ਤਿਆਰੀ_Caller_Cap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *ਜਵਾਬ_ਥਾਂ, *caller_ਥਾਂ;
    cap_t masterCap ਅਣਵਰਤਿਆ, callerCap ਅਣਵਰਤਿਆ;

    ਸੈੱਟ_Thread_State(sender, Thread_State_Blocked_On_ਜਵਾਬ);
    ਜਵਾਬ_ਥਾਂ = TCB_PTR_CTE_PTR(sender, tcb_ਜਵਾਬ);
    masterCap = ਜਵਾਬ_ਥਾਂ->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_get_capType(masterCap) == cap_ਜਵਾਬ_cap);
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Master(masterCap));
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Can_Grant(masterCap));
    ਪ੍ਰਤਿਜ੍ਞਾ(TCB_PTR(cap_ਜਵਾਬ_cap_get_cap_TCBPtr(masterCap)) == sender);
    caller_ਥਾਂ = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = caller_ਥਾਂ->cap;
    /* Haskell error: "Caller cap must not already exist" */
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_get_capType(callerCap) == cap_null_cap);
    cte_ਪਾਓ(cap_ਜਵਾਬ_cap_new(canGrant, false, TCB_REF(sender)),
              ਜਵਾਬ_ਥਾਂ, caller_ਥਾਂ);
}

ਖਾਲੀ deleteCallerCap(tcb_t *receiver)
{
    cte_t *caller_ਥਾਂ;

    caller_ਥਾਂ = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(caller_ਥਾਂ);
}
#ਜੇ_ਅੰਤ

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, ਸ਼ਬਦ_t *ਬਫਰ_Ptr, se_L4_ਸੁਨੇਹਾ_Info_t info)
{
    lookup_ਥਾਂ_raw_ret_t lu_ret;
    cptr_t cptr;
    ਸ਼ਬਦ_t i, ਲੰਮਾਈ;

    ਜੇ (!ਬਫਰ_Ptr) {
        current_extra_caps.excaprefs[0] = ਨਿਰਾ;
        ਵਾਪਸ EXCEPTION_NONE;
    }

    ਲੰਮਾਈ = se_L4_ਸੁਨੇਹਾ_Info_get_extra_Caps(info);

    ਲਈ (i = 0; i < ਲੰਮਾਈ; i++) {
        cptr = getExtraCPtr(ਬਫਰ_Ptr, i);

        lu_ret = lookup_ਥਾਂ(thread, cptr);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }

        current_extra_caps.excaprefs[i] = lu_ret.ਥਾਂ;
    }
    ਜੇ (i < se_L4_ਸੁਨੇਹਾ_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = ਨਿਰਾ;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
ਸ਼ਬਦ_t copyMRs(tcb_t *sender, ਸ਼ਬਦ_t *sendBuf, tcb_t *receiver,
               ਸ਼ਬਦ_t *recvBuf, ਸ਼ਬਦ_t n)
{
    ਸ਼ਬਦ_t i;

    /* Copy inline words */
    ਲਈ (i = 0; i < n && i < n_ਸੁਨੇਹਾ_Registers; i++) {
        ਸੈੱਟ_ਰਜਿਸਟਰ(receiver, ਸੁਨੇਹਾ_Registers[i],
                    get_ਰਜਿਸਟਰ(sender, ਸੁਨੇਹਾ_Registers[i]));
    }

    ਜੇ (!recvBuf || !sendBuf) {
        ਵਾਪਸ i;
    }

    /* Copy out-of-line words */
    ਲਈ (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    ਵਾਪਸ i;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
ਖਾਲੀ remote_ਕਤਾਰ_Update(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    ਜੇ (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *ਨਿਸ਼ਾਨਾ_Cur_Thread = ਗੰਢ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        ਜੇ (ਨਿਸ਼ਾਨਾ_Cur_Thread == ਗੰਢ_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcb_ਤਰਜੀਹ > ਨਿਸ਼ਾਨਾ_Cur_Thread->tcb_ਤਰਜੀਹ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
            || ਗੰਢ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#ਜੇ_ਅੰਤ
           ) {
            ARCH_ਗੰਢ_STATE(ipiReschedulePending) |= ਦੋਆਧਾਰੀ_ਅੰਕ(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
ਖਾਲੀ remoteTCBStall(tcb_t *tcb)
{

    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#ਜੇ_ਅੰਤ
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        ਗੰਢ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_ਗੰਢ_STATE(ipiReschedulePending) |= ਦੋਆਧਾਰੀ_ਅੰਕ(tcb->tcbAffinity);
    }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ exception_t invoke_TCB_ਸੈੱਟ_Affinity(tcb_t *thread, ਸ਼ਬਦ_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(thread);
    migrateTCB(thread, affinity);
    ਜੇ (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    ਜੇ (thread == ਗੰਢ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decode_ਸੈੱਟ_Affinity(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    tcb_t *tcb;
    ਸ਼ਬਦ_t affinity;

    ਜੇ (ਲੰਮਾਈ < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, ਬਫਰ);
    ਜੇ (affinity >= ks_ਅੰਕ_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invoke_TCB_ਸੈੱਟ_Affinity(tcb, affinity);
}
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
ਸਥਿਰ exception_t invokeConfigureSingleStepping(bool_t call, ਸ਼ਬਦ_t *ਬਫਰ, tcb_t *t,
                                                 uint16_t bp_ਅੰਕ, ਸ਼ਬਦ_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);
    ਸ਼ਬਦ_t ਮੁੱਲ;

    bp_was_consumed = configureSingleStepping(t, bp_ਅੰਕ, n_instrs, false);
    ਜੇ (n_instrs == 0) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
        unset_Breakpoint_ਵਰਤਿਆ_Flag(t, bp_ਅੰਕ);
#ਜੇ_ਅੰਤ /* CONFIG_ARCH_AARCH64 */
        ਮੁੱਲ = false;
    } ਨਹੀਂਤਾਂ {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
        ਸੈੱਟ_Breakpoint_ਵਰਤਿਆ_Flag(t, bp_ਅੰਕ);
#ਜੇ_ਅੰਤ /* CONFIG_ARCH_AARCH64 */
        ਮੁੱਲ = bp_was_consumed;
    }

    ਜੇ (call) {
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ = ਸੈੱਟ_MR(thread, ਬਫਰ, 0, ਮੁੱਲ);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    uint16_t bp_ਅੰਕ;
    ਸ਼ਬਦ_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_ਅੰਕ = getSyscallArg(0, ਬਫਰ);
    n_instrs = getSyscallArg(1, ਬਫਰ);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_ਅੰਕ, n_instrs, false);
    ਜੇ (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeConfigureSingleStepping(call, ਬਫਰ, tcb, bp_ਅੰਕ, n_instrs);
}

ਸਥਿਰ exception_t invoke_ਸੈੱਟ_Breakpoint(tcb_t *tcb, uint16_t bp_ਅੰਕ,
                                       ਸ਼ਬਦ_t vaddr, ਸ਼ਬਦ_t type, ਸ਼ਬਦ_t ਮਾਪ, ਸ਼ਬਦ_t rw)
{
    ਸੈੱਟ_Breakpoint(tcb, bp_ਅੰਕ, vaddr, type, ਮਾਪ, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    ਸੈੱਟ_Breakpoint_ਵਰਤਿਆ_Flag(tcb, bp_ਅੰਕ);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decode_ਸੈੱਟ_Breakpoint(cap_t cap, ਸ਼ਬਦ_t *ਬਫਰ)
{
    uint16_t bp_ਅੰਕ;
    ਸ਼ਬਦ_t vaddr, type, ਮਾਪ, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ਅੰਕ = getSyscallArg(0, ਬਫਰ);
    vaddr = getSyscallArg(1, ਬਫਰ);
    type = getSyscallArg(2, ਬਫਰ);
    ਮਾਪ = getSyscallArg(3, ਬਫਰ);
    rw = getSyscallArg(4, ਬਫਰ);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    ਜੇ (vaddr >= (ਸ਼ਬਦ_t)USER_ਸਿਖਰ) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (type != seL4_InstructionBreakpoint && type != se_L4_ਅੰਕੜੇ_Breakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 2;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    } ਨਹੀਂਤਾਂ ਜੇ (type == seL4_InstructionBreakpoint) {
        ਜੇ (ਮਾਪ != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 3;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਜੇ (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 4;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਜੇ ((se_L4_ਪਹਿਲਾ_Watchpoint == -1 || bp_ਅੰਕ >= se_L4_ਪਹਿਲਾ_Watchpoint)
            && se_L4_ਪਹਿਲਾ_Breakpoint != se_L4_ਪਹਿਲਾ_Watchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 2;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
    } ਨਹੀਂਤਾਂ ਜੇ (type == se_L4_ਅੰਕੜੇ_Breakpoint) {
        ਜੇ (ਮਾਪ == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 3;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਜੇ (se_L4_ਪਹਿਲਾ_Watchpoint != -1 && bp_ਅੰਕ < se_L4_ਪਹਿਲਾ_Watchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 2;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
    } ਨਹੀਂਤਾਂ ਜੇ (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 2;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 3;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (ਮਾਪ != 0 && ਮਾਪ != 1 && ਮਾਪ != 2 && ਮਾਪ != 4 && ਮਾਪ != 8) {
        userError("Debug: Invalid size %lu.", ਮਾਪ);
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 3;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (ਮਾਪ > 0 && vaddr & (ਮਾਪ - 1)) {
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
                  vaddr, ਮਾਪ);

        current_syscall_error.type = seL4_AlignmentError;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decode_ਸੈੱਟ_Breakpoint(tcb, bp_ਅੰਕ, vaddr, type, ਮਾਪ, rw);
    ਜੇ (error.type != seL4_NoError) {
        current_syscall_error = error;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invoke_ਸੈੱਟ_Breakpoint(tcb, bp_ਅੰਕ,
                               vaddr, type, ਮਾਪ, rw);
}

ਸਥਿਰ exception_t invokeGetBreakpoint(bool_t call, ਸ਼ਬਦ_t *ਬਫਰ, tcb_t *tcb, uint16_t bp_ਅੰਕ)
{
    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_ਅੰਕ);
    ਜੇ (call) {
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
        ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, 0, res.vaddr);
        ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, 1, res.type);
        ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, 2, res.ਮਾਪ);
        ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, 3, res.rw);
        ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, 4, res.is_enabled);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, 5)));
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeGetBreakpoint(cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    tcb_t *tcb;
    uint16_t bp_ਅੰਕ;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ਅੰਕ = getSyscallArg(0, ਬਫਰ);

    error = Arch_decodeGetBreakpoint(tcb, bp_ਅੰਕ);
    ਜੇ (error.type != seL4_NoError) {
        current_syscall_error = error;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeGetBreakpoint(call, ਬਫਰ, tcb, bp_ਅੰਕ);
}

ਸਥਿਰ exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_ਅੰਕ)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_ਅੰਕ);
    unset_Breakpoint_ਵਰਤਿਆ_Flag(tcb, bp_ਅੰਕ);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeUnsetBreakpoint(cap_t cap, ਸ਼ਬਦ_t *ਬਫਰ)
{
    tcb_t *tcb;
    uint16_t bp_ਅੰਕ;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ਅੰਕ = getSyscallArg(0, ਬਫਰ);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_ਅੰਕ);
    ਜੇ (error.type != seL4_NoError) {
        current_syscall_error = error;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeUnsetBreakpoint(tcb, bp_ਅੰਕ);
}
#ਜੇ_ਅੰਤ /* CONFIG_HARDWARE_DEBUG_API */

ਸਥਿਰ exception_t invoke_ਸੈੱਟ_TLSBase(tcb_t *thread, ਸ਼ਬਦ_t tls_ਅਧਾਰ)
{
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, TLS_ਅਧਾਰ, tls_ਅਧਾਰ);
    ਜੇ (thread == ਗੰਢ_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decode_ਸੈੱਟ_TLSBase(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t tls_ਅਧਾਰ;

    ਜੇ (ਲੰਮਾਈ < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    tls_ਅਧਾਰ = getSyscallArg(0, ਬਫਰ);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invoke_ਸੈੱਟ_TLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_ਅਧਾਰ);
}

ਸਥਿਰ ਖਾਲੀ invoke_ਸੈੱਟ_Flags(tcb_t *thread, ਸ਼ਬਦ_t clear, ਸ਼ਬਦ_t ਸੈੱਟ, bool_t call)
{
    tcb_t *cur_thread = ਗੰਢ_STATE(ksCurThread);
    ਸ਼ਬਦ_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= ਸੈੱਟ & se_L4_TCBFlag_ਨਕਾਬ;
    thread->tcbFlags = flags;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
    ਜੇ (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } ਨਹੀਂਤਾਂ ਜੇ (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#ਜੇ_ਅੰਤ
    ਜੇ (call) {
        ਸ਼ਬਦ_t *ipc_ਬਫਰ = lookupIPCBuffer(true, cur_thread);
        ਸੈੱਟ_ਰਜਿਸਟਰ(cur_thread, badge_ਰਜਿਸਟਰ, 0);
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ = ਸੈੱਟ_MR(cur_thread, ipc_ਬਫਰ, 0, flags);
        ਸੈੱਟ_ਰਜਿਸਟਰ(cur_thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
    }
    ਸੈੱਟ_Thread_State(cur_thread, ThreadState_Running);
}

ਸਥਿਰ exception_t decode_ਸੈੱਟ_Flags(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ਜੇ (ਲੰਮਾਈ < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸ਼ਬਦ_t clear = getSyscallArg(0, ਬਫਰ);
    ਸ਼ਬਦ_t ਸੈੱਟ   = getSyscallArg(1, ਬਫਰ);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    invoke_ਸੈੱਟ_Flags(thread, clear, ਸੈੱਟ, call);
    ਵਾਪਸ EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ, cap_t cap,
                                cte_t *ਥਾਂ, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    ਚੋਣ (invLabel) {
    ਹਾਲਤ TCBReadRegisters:
        /* Second level of decoding */
        ਵਾਪਸ decodeReadRegisters(cap, ਲੰਮਾਈ, call, ਬਫਰ);

    ਹਾਲਤ TCBWriteRegisters:
        ਵਾਪਸ decodeWriteRegisters(cap, ਲੰਮਾਈ, ਬਫਰ);

    ਹਾਲਤ TCBCopyRegisters:
        ਵਾਪਸ decodeCopyRegisters(cap, ਲੰਮਾਈ, ਬਫਰ);

    ਹਾਲਤ TCBSuspend:
        /* Jump straight to the invoke */
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    ਹਾਲਤ TCBResume:
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    ਹਾਲਤ TCBConfigure:
        ਵਾਪਸ decodeTCBConfigure(cap, ਲੰਮਾਈ, ਥਾਂ, ਬਫਰ);

    ਹਾਲਤ TCBSet_ਤਰਜੀਹ:
        ਵਾਪਸ decode_ਸੈੱਟ_ਤਰਜੀਹ(cap, ਲੰਮਾਈ, ਬਫਰ);

    ਹਾਲਤ TCBSetMCPriority:
        ਵਾਪਸ decode_ਸੈੱਟ_MCPriority(cap, ਲੰਮਾਈ, ਬਫਰ);

    ਹਾਲਤ TCBSetSchedParams:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਵਾਪਸ decode_ਸੈੱਟ_Sched_Params(cap, ਲੰਮਾਈ, ਥਾਂ, ਬਫਰ);
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ decode_ਸੈੱਟ_Sched_Params(cap, ਲੰਮਾਈ, ਬਫਰ);
#ਜੇ_ਅੰਤ

    ਹਾਲਤ TCBSetIPCBuffer:
        ਵਾਪਸ decode_ਸੈੱਟ_IPCBuffer(cap, ਲੰਮਾਈ, ਥਾਂ, ਬਫਰ);

    ਹਾਲਤ TCBSetSpace:
        ਵਾਪਸ decode_ਸੈੱਟ_Space(cap, ਲੰਮਾਈ, ਥਾਂ, ਬਫਰ);

    ਹਾਲਤ TCBBindNotification:
        ਵਾਪਸ decodeBindNotification(cap);

    ਹਾਲਤ TCBUnbindNotification:
        ਵਾਪਸ decodeUnbindNotification(cap);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ TCBSetTimeoutEndpoint:
        ਵਾਪਸ decode_ਸੈੱਟ_Timeout_Endpoint(cap, ਥਾਂ);
#ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਹਾਲਤ TCBSetAffinity:
        ਵਾਪਸ decode_ਸੈੱਟ_Affinity(cap, ਲੰਮਾਈ, ਬਫਰ);
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
#ਜੇ_ਅੰਤ

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਹਾਲਤ TCBSetEPTRoot:
        ਵਾਪਸ decode_ਸੈੱਟ_EPTRoot(cap);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    ਹਾਲਤ TCBConfigureSingleStepping:
        ਵਾਪਸ decodeConfigureSingleStepping(cap, call, ਬਫਰ);

    ਹਾਲਤ TCBSetBreakpoint:
        ਵਾਪਸ decode_ਸੈੱਟ_Breakpoint(cap, ਬਫਰ);

    ਹਾਲਤ TCBGetBreakpoint:
        ਵਾਪਸ decodeGetBreakpoint(cap, call, ਬਫਰ);

    ਹਾਲਤ TCBUnsetBreakpoint:
        ਵਾਪਸ decodeUnsetBreakpoint(cap, ਬਫਰ);
#ਜੇ_ਅੰਤ

    ਹਾਲਤ TCBSetTLSBase:
        ਵਾਪਸ decode_ਸੈੱਟ_TLSBase(cap, ਲੰਮਾਈ, ਬਫਰ);

    ਹਾਲਤ TCBSetFlags:
        ਵਾਪਸ decode_ਸੈੱਟ_Flags(cap, ਲੰਮਾਈ, call, ਬਫਰ);

    ਮੂਲ:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

ਗਿਣਤੀ CopyRegistersFlags {
    Copy_Registers_suspend_ਸਰੋਤ = 0,
    Copy_Registers_resume_ਨਿਸ਼ਾਨਾ = 1,
    Copy_Registers_transfer_ਚੌਖਟਾ = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t transferArch;
    tcb_t *srcTCB;
    cap_t ਸਰੋਤ_cap;
    ਸ਼ਬਦ_t flags;

    ਜੇ (ਲੰਮਾਈ < 1 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, ਬਫਰ);

    transferArch = Arch_decodeTransfer(flags >> 8);

    ਸਰੋਤ_cap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cap_get_capType(ਸਰੋਤ_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(ਸਰੋਤ_cap));
    } ਨਹੀਂਤਾਂ {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & ਦੋਆਧਾਰੀ_ਅੰਕ(Copy_Registers_suspend_ਸਰੋਤ),
               flags & ਦੋਆਧਾਰੀ_ਅੰਕ(Copy_Registers_resume_ਨਿਸ਼ਾਨਾ),
               flags & ਦੋਆਧਾਰੀ_ਅੰਕ(Copy_Registers_transfer_ਚੌਖਟਾ),
               flags & ਦੋਆਧਾਰੀ_ਅੰਕ(CopyRegisters_transferInteger),
               transferArch);

}

ਗਿਣਤੀ ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, bool_t call,
                                ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t transferArch, flags, n;
    tcb_t *thread;

    ਜੇ (ਲੰਮਾਈ < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, ਬਫਰ);
    n     = getSyscallArg(1, ਬਫਰ);

    ਜੇ (n < 1 || n > n_ਚੌਖਟਾ_Registers + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (ਪੂਰਨ)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_ਚੌਖਟਾ_Registers +
                                              n_gpRegisters;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ਜੇ (thread == ਗੰਢ_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & ਦੋਆਧਾਰੀ_ਅੰਕ(ReadRegisters_suspend),
               n, transferArch, call);
}

ਗਿਣਤੀ WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t flags, w;
    ਸ਼ਬਦ_t transferArch;
    tcb_t *thread;

    ਜੇ (ਲੰਮਾਈ < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, ਬਫਰ);
    w     = getSyscallArg(1, ਬਫਰ);

    ਜੇ (ਲੰਮਾਈ - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (ਪੂਰਨ)(ਲੰਮਾਈ - 2), (ਪੂਰਨ)w);
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ਜੇ (thread == ਗੰਢ_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeTCB_WriteRegisters(thread,
                                    flags & ਦੋਆਧਾਰੀ_ਅੰਕ(WriteRegisters_resume),
                                    w, transferArch, ਬਫਰ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_endpoint_cap:
        ਵਾਪਸ (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_cap_Can_Grant_ਜਵਾਬ(cap)));
    ਹਾਲਤ cap_null_cap:
        /* just has no fault endpoint */
        ਵਾਪਸ true;
    ਮੂਲ:
        ਵਾਪਸ false;
    }
}
#ਜੇ_ਅੰਤ

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, cte_t *ਥਾਂ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    cte_t *ਬਫਰ_ਥਾਂ, *c_ਜੜ੍ਹ_ਥਾਂ, *v_ਜੜ੍ਹ_ਥਾਂ;
    cap_t ਬਫਰ_Cap, c_ਜੜ੍ਹ_Cap, v_ਜੜ੍ਹ_Cap;
    deriveCap_ret_t dc_ret;
    ਸ਼ਬਦ_t c_ਜੜ੍ਹ_ਅੰਕੜੇ, v_ਜੜ੍ਹ_ਅੰਕੜੇ, ਬਫਰ_Addr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCBCONFIGURE_ਦਲੀਲਾਂ 3
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TCBCONFIGURE_ਦਲੀਲਾਂ 4
#ਜੇ_ਅੰਤ
    ਜੇ (ਲੰਮਾਈ < TCBCONFIGURE_ਦਲੀਲਾਂ || current_extra_caps.excaprefs[0] == ਨਿਰਾ
        || current_extra_caps.excaprefs[1] == ਨਿਰਾ
        || current_extra_caps.excaprefs[2] == ਨਿਰਾ) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    c_ਜੜ੍ਹ_ਅੰਕੜੇ     = getSyscallArg(0, ਬਫਰ);
    v_ਜੜ੍ਹ_ਅੰਕੜੇ     = getSyscallArg(1, ਬਫਰ);
    ਬਫਰ_Addr    = getSyscallArg(2, ਬਫਰ);
#ਨਹੀਂਤਾਂ
    cptr_t faultEP       = getSyscallArg(0, ਬਫਰ);
    c_ਜੜ੍ਹ_ਅੰਕੜੇ     = getSyscallArg(1, ਬਫਰ);
    v_ਜੜ੍ਹ_ਅੰਕੜੇ     = getSyscallArg(2, ਬਫਰ);
    ਬਫਰ_Addr    = getSyscallArg(3, ਬਫਰ);
#ਜੇ_ਅੰਤ

    c_ਜੜ੍ਹ_ਥਾਂ  = current_extra_caps.excaprefs[0];
    c_ਜੜ੍ਹ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ਜੜ੍ਹ_ਥਾਂ  = current_extra_caps.excaprefs[1];
    v_ਜੜ੍ਹ_Cap   = current_extra_caps.excaprefs[1]->cap;
    ਬਫਰ_ਥਾਂ = current_extra_caps.excaprefs[2];
    ਬਫਰ_Cap  = current_extra_caps.excaprefs[2]->cap;

    ਜੇ (ਬਫਰ_Addr == 0) {
        ਬਫਰ_ਥਾਂ = ਨਿਰਾ;
    } ਨਹੀਂਤਾਂ {
        dc_ret = deriveCap(ਬਫਰ_ਥਾਂ, ਬਫਰ_Cap);
        ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ dc_ret.ਸਥਿਤੀ;
        }
        ਬਫਰ_Cap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(ਬਫਰ_Addr, ਬਫਰ_Cap);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }
    }

    ਜੇ (ਥਾਂ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        ਥਾਂ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (c_ਜੜ੍ਹ_ਅੰਕੜੇ != 0) {
        c_ਜੜ੍ਹ_Cap = update_Cap_ਅੰਕੜੇ(false, c_ਜੜ੍ਹ_ਅੰਕੜੇ, c_ਜੜ੍ਹ_Cap);
    }

    dc_ret = deriveCap(c_ਜੜ੍ਹ_ਥਾਂ, c_ਜੜ੍ਹ_Cap);
    ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ dc_ret.ਸਥਿਤੀ;
    }
    c_ਜੜ੍ਹ_Cap = dc_ret.cap;

    ਜੇ (cap_get_capType(c_ਜੜ੍ਹ_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (v_ਜੜ੍ਹ_ਅੰਕੜੇ != 0) {
        v_ਜੜ੍ਹ_Cap = update_Cap_ਅੰਕੜੇ(false, v_ਜੜ੍ਹ_ਅੰਕੜੇ, v_ਜੜ੍ਹ_Cap);
    }

    dc_ret = deriveCap(v_ਜੜ੍ਹ_ਥਾਂ, v_ਜੜ੍ਹ_Cap);
    ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ dc_ret.ਸਥਿਤੀ;
    }
    v_ਜੜ੍ਹ_Cap = dc_ret.cap;

    ਜੇ (!is_Valid_VTable_ਜੜ੍ਹ(v_ਜੜ੍ਹ_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਵਾਪਸ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਥਾਂ,
               cap_null_cap_new(), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               c_ਜੜ੍ਹ_Cap, c_ਜੜ੍ਹ_ਥਾਂ,
               v_ਜੜ੍ਹ_Cap, v_ਜੜ੍ਹ_ਥਾਂ,
               ਬਫਰ_Addr, ਬਫਰ_Cap,
               ਬਫਰ_ਥਾਂ, thread_control_caps_update_space |
               thread_control_caps_update_ipc_ਬਫਰ);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਥਾਂ,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_ਜੜ੍ਹ_Cap, c_ਜੜ੍ਹ_ਥਾਂ,
               v_ਜੜ੍ਹ_Cap, v_ਜੜ੍ਹ_ਥਾਂ,
               ਬਫਰ_Addr, ਬਫਰ_Cap,
               ਬਫਰ_ਥਾਂ, thread_control_update_space |
               thread_control_update_ipc_ਬਫਰ);
#ਜੇ_ਅੰਤ
}

exception_t decode_ਸੈੱਟ_ਤਰਜੀਹ(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਜੇ (ਲੰਮਾਈ < 1 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, ਬਫਰ);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t ਸਥਿਤੀ = checkPrio(newPrio, authTCB);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) newPrio, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) authTCB->tcbMCP);
        ਵਾਪਸ ਸਥਿਤੀ;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਵਾਪਸ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               NULL_PRIO, newPrio,
               ਨਿਰਾ, thread_control_sched_update_ਤਰਜੀਹ);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਨਿਰਾ,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               0, cap_null_cap_new(),
               ਨਿਰਾ, thread_control_update_ਤਰਜੀਹ);
#ਜੇ_ਅੰਤ
}

exception_t decode_ਸੈੱਟ_MCPriority(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਜੇ (ਲੰਮਾਈ < 1 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, ਬਫਰ);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t ਸਥਿਤੀ = checkPrio(newMcp, authTCB);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) newMcp, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) authTCB->tcbMCP);
        ਵਾਪਸ ਸਥਿਤੀ;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਵਾਪਸ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               newMcp, NULL_PRIO,
               ਨਿਰਾ, thread_control_sched_update_mcp);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਨਿਰਾ,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               0, cap_null_cap_new(),
               ਨਿਰਾ, thread_control_update_mcp);
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
exception_t decode_ਸੈੱਟ_Timeout_Endpoint(cap_t cap, cte_t *ਥਾਂ)
{
    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *th_ਥਾਂ = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    ਜੇ (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਥਾਂ,
               cap_null_cap_new(), ਨਿਰਾ,
               thCap, th_ਥਾਂ,
               cap_null_cap_new(), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               0, cap_null_cap_new(), ਨਿਰਾ,
               thread_control_caps_update_timeout);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
exception_t decode_ਸੈੱਟ_Sched_Params(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, cte_t *ਥਾਂ, ਸ਼ਬਦ_t *ਬਫਰ)
#ਨਹੀਂਤਾਂ
exception_t decode_ਸੈੱਟ_Sched_Params(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
#ਜੇ_ਅੰਤ
{
    ਜੇ (ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == ਨਿਰਾ || current_extra_caps.excaprefs[2] == ਨਿਰਾ
#ਜੇ_ਅੰਤ
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, ਬਫਰ);
    prio_t newPrio = getSyscallArg(1, ਬਫਰ);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fh_ਥਾਂ = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#ਜੇ_ਅੰਤ

    ਜੇ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t ਸਥਿਤੀ = checkPrio(newMcp, authTCB);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) newMcp, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) authTCB->tcbMCP);
        ਵਾਪਸ ਸਥਿਤੀ;
    }

    ਸਥਿਤੀ = checkPrio(newPrio, authTCB);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) newPrio, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) authTCB->tcbMCP);
        ਵਾਪਸ ਸਥਿਤੀ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = ਨਿਰਾ;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_ਤਰਜੀਹ |
                                         thread_control_sched_update_fault;
    ਚੋਣ (cap_get_capType(scCap)) {
    ਹਾਲਤ cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        ਜੇ (tcb->tcbSchedContext != sc) {
            ਜੇ (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
            ਜੇ (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        }
        ਜੇ (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_null_cap:
        ਜੇ (tcb == ਗੰਢ_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਰੋਕੋ;
    ਮੂਲ:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 2;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    ਜੇ (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    ਜੇ (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 3;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਵਾਪਸ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਥਾਂ,
               fhCap, fh_ਥਾਂ,
               newMcp, newPrio,
               sc, update_flags);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਨਿਰਾ,
               0, newMcp, newPrio,
               cap_null_cap_new(), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               0, cap_null_cap_new(),
               ਨਿਰਾ, thread_control_update_mcp |
               thread_control_update_ਤਰਜੀਹ);
#ਜੇ_ਅੰਤ
}


exception_t decode_ਸੈੱਟ_IPCBuffer(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, cte_t *ਥਾਂ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    cptr_t cptr_ਬਫਰ_Ptr;
    cap_t ਬਫਰ_Cap;
    cte_t *ਬਫਰ_ਥਾਂ;

    ਜੇ (ਲੰਮਾਈ < 1 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    cptr_ਬਫਰ_Ptr  = getSyscallArg(0, ਬਫਰ);
    ਬਫਰ_ਥਾਂ = current_extra_caps.excaprefs[0];
    ਬਫਰ_Cap  = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cptr_ਬਫਰ_Ptr == 0) {
        ਬਫਰ_ਥਾਂ = ਨਿਰਾ;
    } ਨਹੀਂਤਾਂ {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(ਬਫਰ_ਥਾਂ, ਬਫਰ_Cap);
        ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ dc_ret.ਸਥਿਤੀ;
        }
        ਬਫਰ_Cap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_ਬਫਰ_Ptr, ਬਫਰ_Cap);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਵਾਪਸ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਥਾਂ,
               cap_null_cap_new(), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               cptr_ਬਫਰ_Ptr, ਬਫਰ_Cap,
               ਬਫਰ_ਥਾਂ, thread_control_caps_update_ipc_ਬਫਰ);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਥਾਂ,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), ਨਿਰਾ,
               cap_null_cap_new(), ਨਿਰਾ,
               cptr_ਬਫਰ_Ptr, ਬਫਰ_Cap,
               ਬਫਰ_ਥਾਂ, thread_control_update_ipc_ਬਫਰ);

#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECODE_ਸੈੱਟ_SPACE_PARAMS 2
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECODE_ਸੈੱਟ_SPACE_PARAMS 3
#ਜੇ_ਅੰਤ
exception_t decode_ਸੈੱਟ_Space(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, cte_t *ਥਾਂ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t c_ਜੜ੍ਹ_ਅੰਕੜੇ, v_ਜੜ੍ਹ_ਅੰਕੜੇ;
    cte_t *c_ਜੜ੍ਹ_ਥਾਂ, *v_ਜੜ੍ਹ_ਥਾਂ;
    cap_t c_ਜੜ੍ਹ_Cap, v_ਜੜ੍ਹ_Cap;
    deriveCap_ret_t dc_ret;

    ਜੇ (ਲੰਮਾਈ < DECODE_ਸੈੱਟ_SPACE_PARAMS || current_extra_caps.excaprefs[0] == ਨਿਰਾ
        || current_extra_caps.excaprefs[1] == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == ਨਿਰਾ
#ਜੇ_ਅੰਤ
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    c_ਜੜ੍ਹ_ਅੰਕੜੇ = getSyscallArg(0, ਬਫਰ);
    v_ਜੜ੍ਹ_ਅੰਕੜੇ = getSyscallArg(1, ਬਫਰ);

    cte_t *fh_ਥਾਂ     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_ਜੜ੍ਹ_ਥਾਂ  = current_extra_caps.excaprefs[1];
    c_ਜੜ੍ਹ_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_ਜੜ੍ਹ_ਥਾਂ  = current_extra_caps.excaprefs[2];
    v_ਜੜ੍ਹ_Cap   = current_extra_caps.excaprefs[2]->cap;
#ਨਹੀਂਤਾਂ
    cptr_t faultEP   = getSyscallArg(0, ਬਫਰ);
    c_ਜੜ੍ਹ_ਅੰਕੜੇ = getSyscallArg(1, ਬਫਰ);
    v_ਜੜ੍ਹ_ਅੰਕੜੇ = getSyscallArg(2, ਬਫਰ);

    c_ਜੜ੍ਹ_ਥਾਂ  = current_extra_caps.excaprefs[0];
    c_ਜੜ੍ਹ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ਜੜ੍ਹ_ਥਾਂ  = current_extra_caps.excaprefs[1];
    v_ਜੜ੍ਹ_Cap   = current_extra_caps.excaprefs[1]->cap;
#ਜੇ_ਅੰਤ

    ਜੇ (ਥਾਂ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        ਥਾਂ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (c_ਜੜ੍ਹ_ਅੰਕੜੇ != 0) {
        c_ਜੜ੍ਹ_Cap = update_Cap_ਅੰਕੜੇ(false, c_ਜੜ੍ਹ_ਅੰਕੜੇ, c_ਜੜ੍ਹ_Cap);
    }

    dc_ret = deriveCap(c_ਜੜ੍ਹ_ਥਾਂ, c_ਜੜ੍ਹ_Cap);
    ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ dc_ret.ਸਥਿਤੀ;
    }
    c_ਜੜ੍ਹ_Cap = dc_ret.cap;

    ਜੇ (cap_get_capType(c_ਜੜ੍ਹ_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (v_ਜੜ੍ਹ_ਅੰਕੜੇ != 0) {
        v_ਜੜ੍ਹ_Cap = update_Cap_ਅੰਕੜੇ(false, v_ਜੜ੍ਹ_ਅੰਕੜੇ, v_ਜੜ੍ਹ_Cap);
    }

    dc_ret = deriveCap(v_ਜੜ੍ਹ_ਥਾਂ, v_ਜੜ੍ਹ_Cap);
    ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ dc_ret.ਸਥਿਤੀ;
    }
    v_ਜੜ੍ਹ_Cap = dc_ret.cap;

    ਜੇ (!is_Valid_VTable_ਜੜ੍ਹ(v_ਜੜ੍ਹ_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    /* fault handler */
    ਜੇ (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
#ਜੇ_ਅੰਤ

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਵਾਪਸ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਥਾਂ,
               fhCap, fh_ਥਾਂ,
               cap_null_cap_new(), ਨਿਰਾ,
               c_ਜੜ੍ਹ_Cap, c_ਜੜ੍ਹ_ਥਾਂ,
               v_ਜੜ੍ਹ_Cap, v_ਜੜ੍ਹ_ਥਾਂ,
               0, cap_null_cap_new(), ਨਿਰਾ, thread_control_caps_update_space | thread_control_caps_update_fault);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ਥਾਂ,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_ਜੜ੍ਹ_Cap, c_ਜੜ੍ਹ_ਥਾਂ,
               v_ਜੜ੍ਹ_Cap, v_ਜੜ੍ਹ_ਥਾਂ,
               0, cap_null_cap_new(), ਨਿਰਾ, thread_control_update_space);
#ਜੇ_ਅੰਤ
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ਜੇ (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } ਨਹੀਂਤਾਂ {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ ((tcb_t *)notification_ptr_get_ntfn_ਕਤਾਰ_ਸਿਰ(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }


    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ਜੇ (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeTCB_NotificationControl(tcb, ਨਿਰਾ);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ ਸਤਰਅੰਦਰ exception_t installTCBCap(tcb_t *ਨਿਸ਼ਾਨਾ, cap_t tCap, cte_t *ਥਾਂ,
                                        tcb_cnode_ਸੂਚਕਾਂਕ_t ਸੂਚਕਾਂਕ, cap_t newCap, cte_t *src_ਥਾਂ)
{
    cte_t *ਜੜ੍ਹ_ਥਾਂ = TCB_PTR_CTE_PTR(ਨਿਸ਼ਾਨਾ, ਸੂਚਕਾਂਕ);
    ਅਣਵਰਤਿਆ exception_t e = cteDelete(ਜੜ੍ਹ_ਥਾਂ, true);
    ਜੇ (e != EXCEPTION_NONE) {
        ਵਾਪਸ e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    ਜੇ (cap_get_capType(newCap) != cap_null_cap) {
        ਜੇ (sameObjectAs(newCap, src_ਥਾਂ->cap) &&
            sameObjectAs(tCap, ਥਾਂ->cap)) {
            cte_ਪਾਓ(newCap, src_ਥਾਂ, ਜੜ੍ਹ_ਥਾਂ);
        }
    }
    ਵਾਪਸ e;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *ਨਿਸ਼ਾਨਾ, cte_t *ਥਾਂ,
                                        cap_t fh_newCap, cte_t *fh_src_ਥਾਂ,
                                        cap_t th_newCap, cte_t *th_src_ਥਾਂ,
                                        cap_t c_ਜੜ੍ਹ_new_Cap, cte_t *c_ਜੜ੍ਹ_src_ਥਾਂ,
                                        cap_t v_ਜੜ੍ਹ_new_Cap, cte_t *v_ਜੜ੍ਹ_src_ਥਾਂ,
                                        ਸ਼ਬਦ_t ਬਫਰ_Addr, cap_t ਬਫਰ_Cap,
                                        cte_t *ਬਫਰ_Src_ਥਾਂ,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((ਸ਼ਬਦ_t)ਨਿਸ਼ਾਨਾ);

    ਜੇ (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(ਨਿਸ਼ਾਨਾ, tCap, ਥਾਂ, tcbFaultHandler, fh_newCap, fh_src_ਥਾਂ);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }

    }

    ਜੇ (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(ਨਿਸ਼ਾਨਾ, tCap, ਥਾਂ, tcbTimeoutHandler, th_newCap, th_src_ਥਾਂ);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }
    }

    ਜੇ (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(ਨਿਸ਼ਾਨਾ, tCap, ਥਾਂ, tcbCTable, c_ਜੜ੍ਹ_new_Cap, c_ਜੜ੍ਹ_src_ਥਾਂ);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }

        e = installTCBCap(ਨਿਸ਼ਾਨਾ, tCap, ਥਾਂ, tcbVTable, v_ਜੜ੍ਹ_new_Cap, v_ਜੜ੍ਹ_src_ਥਾਂ);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }
    }

    ਜੇ (updateFlags & thread_control_caps_update_ipc_ਬਫਰ) {
        cte_t *ਬਫਰ_ਥਾਂ;

        ਬਫਰ_ਥਾਂ = TCB_PTR_CTE_PTR(ਨਿਸ਼ਾਨਾ, tcb_ਬਫਰ);
        e = cteDelete(ਬਫਰ_ਥਾਂ, true);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }
        ਨਿਸ਼ਾਨਾ->tcbIPCBuffer = ਬਫਰ_Addr;

        ਜੇ (ਬਫਰ_Src_ਥਾਂ && sameObjectAs(ਬਫਰ_Cap, ਬਫਰ_Src_ਥਾਂ->cap) &&
            sameObjectAs(tCap, ਥਾਂ->cap)) {
            cte_ਪਾਓ(ਬਫਰ_Cap, ਬਫਰ_Src_ਥਾਂ, ਬਫਰ_ਥਾਂ);
        }

        ਜੇ (ਨਿਸ਼ਾਨਾ == ਗੰਢ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ਵਾਪਸ EXCEPTION_NONE;
}
#ਨਹੀਂਤਾਂ
exception_t invokeTCB_ThreadControl(tcb_t *ਨਿਸ਼ਾਨਾ, cte_t *ਥਾਂ,
                                    cptr_t faultep, prio_t mcp, prio_t ਤਰਜੀਹ,
                                    cap_t c_ਜੜ੍ਹ_new_Cap, cte_t *c_ਜੜ੍ਹ_src_ਥਾਂ,
                                    cap_t v_ਜੜ੍ਹ_new_Cap, cte_t *v_ਜੜ੍ਹ_src_ਥਾਂ,
                                    ਸ਼ਬਦ_t ਬਫਰ_Addr, cap_t ਬਫਰ_Cap,
                                    cte_t *ਬਫਰ_Src_ਥਾਂ,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((ਸ਼ਬਦ_t)ਨਿਸ਼ਾਨਾ);

    ਜੇ (updateFlags & thread_control_update_space) {
        ਨਿਸ਼ਾਨਾ->tcbFaultHandler = faultep;
    }

    ਜੇ (updateFlags & thread_control_update_mcp) {
        ਸੈੱਟ_MCPriority(ਨਿਸ਼ਾਨਾ, mcp);
    }

    ਜੇ (updateFlags & thread_control_update_space) {
        cte_t *ਜੜ੍ਹ_ਥਾਂ;

        ਜੜ੍ਹ_ਥਾਂ = TCB_PTR_CTE_PTR(ਨਿਸ਼ਾਨਾ, tcbCTable);
        e = cteDelete(ਜੜ੍ਹ_ਥਾਂ, true);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }
        ਜੇ (sameObjectAs(c_ਜੜ੍ਹ_new_Cap, c_ਜੜ੍ਹ_src_ਥਾਂ->cap) &&
            sameObjectAs(tCap, ਥਾਂ->cap)) {
            cte_ਪਾਓ(c_ਜੜ੍ਹ_new_Cap, c_ਜੜ੍ਹ_src_ਥਾਂ, ਜੜ੍ਹ_ਥਾਂ);
        }
    }

    ਜੇ (updateFlags & thread_control_update_space) {
        cte_t *ਜੜ੍ਹ_ਥਾਂ;

        ਜੜ੍ਹ_ਥਾਂ = TCB_PTR_CTE_PTR(ਨਿਸ਼ਾਨਾ, tcbVTable);
        e = cteDelete(ਜੜ੍ਹ_ਥਾਂ, true);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }
        ਜੇ (sameObjectAs(v_ਜੜ੍ਹ_new_Cap, v_ਜੜ੍ਹ_src_ਥਾਂ->cap) &&
            sameObjectAs(tCap, ਥਾਂ->cap)) {
            cte_ਪਾਓ(v_ਜੜ੍ਹ_new_Cap, v_ਜੜ੍ਹ_src_ਥਾਂ, ਜੜ੍ਹ_ਥਾਂ);
        }
    }

    ਜੇ (updateFlags & thread_control_update_ipc_ਬਫਰ) {
        cte_t *ਬਫਰ_ਥਾਂ;

        ਬਫਰ_ਥਾਂ = TCB_PTR_CTE_PTR(ਨਿਸ਼ਾਨਾ, tcb_ਬਫਰ);
        e = cteDelete(ਬਫਰ_ਥਾਂ, true);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }
        ਨਿਸ਼ਾਨਾ->tcbIPCBuffer = ਬਫਰ_Addr;

        ਜੇ (ਬਫਰ_Src_ਥਾਂ && sameObjectAs(ਬਫਰ_Cap, ਬਫਰ_Src_ਥਾਂ->cap) &&
            sameObjectAs(tCap, ਥਾਂ->cap)) {
            cte_ਪਾਓ(ਬਫਰ_Cap, ਬਫਰ_Src_ਥਾਂ, ਬਫਰ_ਥਾਂ);
        }

        ਜੇ (ਨਿਸ਼ਾਨਾ == ਗੰਢ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ਜੇ (updateFlags & thread_control_update_ਤਰਜੀਹ) {
        ਸੈੱਟ_ਤਰਜੀਹ(ਨਿਸ਼ਾਨਾ, ਤਰਜੀਹ);
    }

    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *ਨਿਸ਼ਾਨਾ, cte_t *ਥਾਂ,
                                         cap_t fh_newCap, cte_t *fh_src_ਥਾਂ,
                                         prio_t mcp, prio_t ਤਰਜੀਹ,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    ਜੇ (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((ਸ਼ਬਦ_t)ਨਿਸ਼ਾਨਾ);
        exception_t e = installTCBCap(ਨਿਸ਼ਾਨਾ, tCap, ਥਾਂ, tcbFaultHandler, fh_newCap, fh_src_ਥਾਂ);
        ਜੇ (e != EXCEPTION_NONE) {
            ਵਾਪਸ e;
        }
    }

    ਜੇ (updateFlags & thread_control_sched_update_mcp) {
        ਸੈੱਟ_MCPriority(ਨਿਸ਼ਾਨਾ, mcp);
    }

    ਜੇ (updateFlags & thread_control_sched_update_ਤਰਜੀਹ) {
        ਸੈੱਟ_ਤਰਜੀਹ(ਨਿਸ਼ਾਨਾ, ਤਰਜੀਹ);
    }

    ਜੇ (updateFlags & thread_control_sched_update_sc) {
        ਜੇ (sc != ਨਿਰਾ) {
            schedContext_bindTCB(sc, ਨਿਸ਼ਾਨਾ);
        } ਨਹੀਂਤਾਂ ਜੇ (sc == ਨਿਰਾ) {
            schedContext_unbindTCB(ਨਿਸ਼ਾਨਾ->tcbSchedContext);
        }
    }

    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspend_ਸਰੋਤ, bool_t resume_ਨਿਸ਼ਾਨਾ,
                                    bool_t transfer_ਚੌਖਟਾ, bool_t transferInteger,
                                    ਸ਼ਬਦ_t transferArch)
{
    ਜੇ (suspend_ਸਰੋਤ) {
        suspend(tcb_src);
    }

    ਜੇ (resume_ਨਿਸ਼ਾਨਾ) {
        restart(dest);
    }

    ਜੇ (transfer_ਚੌਖਟਾ) {
        ਸ਼ਬਦ_t i;
        ਸ਼ਬਦ_t v;
        ਸ਼ਬਦ_t pc;

        ਲਈ (i = 0; i < n_ਚੌਖਟਾ_Registers; i++) {
            v = get_ਰਜਿਸਟਰ(tcb_src, ਚੌਖਟਾ_Registers[i]);
            ਸੈੱਟ_ਰਜਿਸਟਰ(dest, ਚੌਖਟਾ_Registers[i], v);
        }

        pc = getRestartPC(dest);
        ਸੈੱਟ_ਅਗਲਾ_PC(dest, pc);
    }

    ਜੇ (transferInteger) {
        ਸ਼ਬਦ_t i;
        ਸ਼ਬਦ_t v;

        ਲਈ (i = 0; i < n_gpRegisters; i++) {
            v = get_ਰਜਿਸਟਰ(tcb_src, gpRegisters[i]);
            ਸੈੱਟ_ਰਜਿਸਟਰ(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    ਜੇ (dest == ਗੰਢ_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    ਵਾਪਸ Arch_performTransfer(transferArch, tcb_src, dest);
}

/* ReadRegisters is a special case: replyFromKernel & setMRs are
 * unfolded here, in order to avoid passing the large reply message up
 * to the top level in a global (and double-copying). We prevent the
 * top-level replyFromKernel_success_empty() from running by setting the
 * thread state. Retype does this too.
 */
exception_t invokeTCB_ReadRegisters(tcb_t *tcb_src, bool_t suspend_ਸਰੋਤ,
                                    ਸ਼ਬਦ_t n, ਸ਼ਬਦ_t arch, bool_t call)
{
    ਸ਼ਬਦ_t i, j;
    exception_t e;
    tcb_t *thread;

    thread = ਗੰਢ_STATE(ksCurThread);

    ਜੇ (suspend_ਸਰੋਤ) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, ਗੰਢ_STATE(ksCurThread));
    ਜੇ (e != EXCEPTION_NONE) {
        ਵਾਪਸ e;
    }

    ਜੇ (call) {
        ਸ਼ਬਦ_t *ipc_ਬਫਰ;

        ipc_ਬਫਰ = lookupIPCBuffer(true, thread);

        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);

        ਲਈ (i = 0; i < n && i < n_ਚੌਖਟਾ_Registers && i < n_ਸੁਨੇਹਾ_Registers; i++) {
            ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Registers[i],
                        get_ਰਜਿਸਟਰ(tcb_src, ਚੌਖਟਾ_Registers[i]));
        }

        ਜੇ (ipc_ਬਫਰ != ਨਿਰਾ && i < n && i < n_ਚੌਖਟਾ_Registers) {
            ਲਈ (; i < n && i < n_ਚੌਖਟਾ_Registers; i++) {
                ipc_ਬਫਰ[i + 1] = get_ਰਜਿਸਟਰ(tcb_src, ਚੌਖਟਾ_Registers[i]);
            }
        }

        j = i;

        ਲਈ (i = 0; i < n_gpRegisters && i + n_ਚੌਖਟਾ_Registers < n
             && i + n_ਚੌਖਟਾ_Registers < n_ਸੁਨੇਹਾ_Registers; i++) {
            ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Registers[i + n_ਚੌਖਟਾ_Registers],
                        get_ਰਜਿਸਟਰ(tcb_src, gpRegisters[i]));
        }

        ਜੇ (ipc_ਬਫਰ != ਨਿਰਾ && i < n_gpRegisters
            && i + n_ਚੌਖਟਾ_Registers < n) {
            ਲਈ (; i < n_gpRegisters && i + n_ਚੌਖਟਾ_Registers < n; i++) {
                ipc_ਬਫਰ[i + n_ਚੌਖਟਾ_Registers + 1] =
                    get_ਰਜਿਸਟਰ(tcb_src, gpRegisters[i]);
            }
        }

        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, i + j)));
    }
    ਸੈੱਟ_Thread_State(thread, ThreadState_Running);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resume_ਨਿਸ਼ਾਨਾ,
                                     ਸ਼ਬਦ_t n, ਸ਼ਬਦ_t arch, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t i;
    ਸ਼ਬਦ_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, ਗੰਢ_STATE(ksCurThread), dest);
    ਜੇ (e != EXCEPTION_NONE) {
        ਵਾਪਸ e;
    }

    ਜੇ (n > n_ਚੌਖਟਾ_Registers + n_gpRegisters) {
        n = n_ਚੌਖਟਾ_Registers + n_gpRegisters;
    }

    archInfo = Arch_get_Sanitise_ਰਜਿਸਟਰ_Info(dest);

    ਲਈ (i = 0; i < n_ਚੌਖਟਾ_Registers && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        ਸੈੱਟ_ਰਜਿਸਟਰ(dest, ਚੌਖਟਾ_Registers[i],
                    sanitise_ਰਜਿਸਟਰ(ਚੌਖਟਾ_Registers[i],
                                     getSyscallArg(i + 2, ਬਫਰ), archInfo));
    }

    ਲਈ (i = 0; i < n_gpRegisters && i + n_ਚੌਖਟਾ_Registers < n; i++) {
        ਸੈੱਟ_ਰਜਿਸਟਰ(dest, gpRegisters[i],
                    sanitise_ਰਜਿਸਟਰ(gpRegisters[i],
                                     getSyscallArg(i + n_ਚੌਖਟਾ_Registers + 2,
                                                   ਬਫਰ), archInfo));
    }

    pc = getRestartPC(dest);
    ਸੈੱਟ_ਅਗਲਾ_PC(dest, pc);

    Arch_postModifyRegisters(dest);

    ਜੇ (resume_ਨਿਸ਼ਾਨਾ) {
        restart(dest);
    }

    ਜੇ (dest == ਗੰਢ_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    ਜੇ (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } ਨਹੀਂਤਾਂ {
        unbindNotification(tcb);
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਖਾਲੀ ਸੈੱਟ_Thread_Name(tcb_t *tcb, ਅਚਲ ਅੱਖਰ *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_ਲੰਮਾਈ);
}
#ਜੇ_ਅੰਤ

ਸ਼ਬਦ_t ਸੈੱਟ_MRs_syscall_error(tcb_t *thread, ਸ਼ਬਦ_t *receiveIPCBuffer)
{
    ਚੋਣ (current_syscall_error.type) {
    ਹਾਲਤ se_L4_Invalid_ਦਲੀਲ:
        ਵਾਪਸ ਸੈੱਟ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ);

    ਹਾਲਤ seL4_InvalidCapability:
        ਵਾਪਸ ਸੈੱਟ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_ਸੰਖਿਆ);

    ਹਾਲਤ seL4_IllegalOperation:
        ਵਾਪਸ 0;

    ਹਾਲਤ seL4_RangeError:
        ਸੈੱਟ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        ਵਾਪਸ ਸੈੱਟ_MR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    ਹਾਲਤ seL4_AlignmentError:
        ਵਾਪਸ 0;

    ਹਾਲਤ seL4_FailedLookup:
        ਸੈੱਟ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failed_Lookup_Was_ਸਰੋਤ ? 1 : 0);
        ਵਾਪਸ ਸੈੱਟ_MRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    ਹਾਲਤ se_L4_Truncated_ਸੁਨੇਹਾ:
    ਹਾਲਤ se_L4_Delete_ਪਹਿਲਾ:
    ਹਾਲਤ se_L4_Revoke_ਪਹਿਲਾ:
        ਵਾਪਸ 0;
    ਹਾਲਤ se_L4_Not_Enough_ਸਮ੍ਰਿਤੀ:
        ਵਾਪਸ ਸੈੱਟ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.ਸਮ੍ਰਿਤੀ_ਖੱਬਾ);
    ਮੂਲ:
        fail("Invalid syscall error");
    }
}
