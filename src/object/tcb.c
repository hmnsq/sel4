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
#ifdef CONFIG_KERNEL_MCS
#ಅನ್ತರ್ಭಾವ <object/schedcontext.h>
#endif
#ಅನ್ತರ್ಭಾವ <object/tcb.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <arch/smp/ipi_inline.h>

#define NULL_PRIO 0

ಸ್ಥಿರ exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    ಪ್ರತಿಜ್ಞಾ(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    ಯದಿ (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ addToBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    ಗಂಟು_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    ಗಂಟು_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & MASK(wordRadix));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    ಗಂಟು_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & MASK(wordRadix));
    ಯದಿ (unlikely(!ಗಂಟು_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        ಗಂಟು_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcbSchedNext;

    ಯದಿ (queue.head == tcb && queue.end == tcb) {
        queue.head = ರಿಕ್ತ;
        queue.end = ರಿಕ್ತ;
    } ಅನ್ಯಥಾ {
        ಯದಿ (queue.head == tcb) {
            after->tcbSchedPrev = ರಿಕ್ತ;
            tcb->tcbSchedNext = ರಿಕ್ತ;
            queue.head = after;
        } ಅನ್ಯಥಾ {
            ಯದಿ (queue.end == tcb) {
                before->tcbSchedNext = ರಿಕ್ತ;
                tcb->tcbSchedPrev = ರಿಕ್ತ;
                queue.end = before;
            } ಅನ್ಯಥಾ {
                before->tcbSchedNext = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = ರಿಕ್ತ;
                tcb->tcbSchedNext = ರಿಕ್ತ;
            }
        }
    }

    ಪ್ರತ್ಯಾಗಮ queue;
}

/* Add TCB to the head of a scheduler queue */
ಶೂನ್ಯ tcbSchedEnqueue(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    ಪ್ರತಿಜ್ಞಾ(isSchedulable(tcb));
    ಪ್ರತಿಜ್ಞಾ(refill_sufficient(tcb->tcbSchedContext, 0));
#endif

    ಯದಿ (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ಯದಿ (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
ಶೂನ್ಯ tcbSchedAppend(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    ಪ್ರತಿಜ್ಞಾ(isSchedulable(tcb));
    ಪ್ರತಿಜ್ಞಾ(refill_sufficient(tcb->tcbSchedContext, 0));
    ಪ್ರತಿಜ್ಞಾ(refill_ready(tcb->tcbSchedContext));
#endif
    ಯದಿ (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ಯದಿ (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
ಶೂನ್ಯ tcbSchedDequeue(tcb_t *tcb)
{
    ಯದಿ (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        ಗಂಟು_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, false);

        ಯದಿ (likely(tcb_queue_empty(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
ಶೂನ್ಯ tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = ರಿಕ್ತ;

    debug_tcb->tcbDebugNext = ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    ಯದಿ (ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

ಶೂನ್ಯ tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != ರಿಕ್ತ);
    ಯದಿ (tcb == ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        ಗಂಟು_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(ಗಂಟು_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcbDebugNext;
    } ಅನ್ಯಥಾ {
        ಪ್ರತಿಜ್ಞಾ(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcbDebugNext = debug_tcb->tcbDebugNext;
    }

    ಯದಿ (debug_tcb->tcbDebugNext) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugNext)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = ರಿಕ್ತ;
    debug_tcb->tcbDebugNext = ರಿಕ್ತ;
}
#endif /* CONFIG_DEBUG_BUILD */

#ifndef CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    ಯದಿ (!queue.head) { /* Empty list */
        queue.head = tcb;
    } ಅನ್ಯಥಾ {
        queue.end->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.end;
    tcb->tcbEPNext = ರಿಕ್ತ;
    queue.end = tcb;

    ಪ್ರತ್ಯಾಗಮ queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    ಯದಿ (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } ಅನ್ಯಥಾ {
        queue.head = tcb->tcbEPNext;
    }

    ಯದಿ (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } ಅನ್ಯಥಾ {
        queue.end = tcb->tcbEPPrev;
    }

    ಪ್ರತ್ಯಾಗಮ queue;
}
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS

ಶೂನ್ಯ tcbReleaseRemove(tcb_t *tcb)
{
    ಯದಿ (likely(thread_state_get_tcbInReleaseQueue(tcb->tcbState))) {
        tcb_queue_t queue = ಗಂಟು_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        ಯದಿ (queue.head == tcb) {
            ಗಂಟು_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        ಗಂಟು_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, false);
    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ticks_t PURE tcbReadyTime(tcb_t *tcb)
{
    ಪ್ರತ್ಯಾಗಮ refill_head(tcb->tcbSchedContext)->rTime;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t PURE time_after(tcb_t *tcb, ticks_t new_time)
{
    ಪ್ರತ್ಯಾಗಮ tcb != ರಿಕ್ತ && new_time >= tcbReadyTime(tcb);
}

ಸ್ಥಿರ tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    ಯಾವತ್ (time_after(after, new_time)) {
        after = after->tcbSchedNext;
    }

    ಪ್ರತ್ಯಾಗಮ after;
}

ಶೂನ್ಯ tcbReleaseEnqueue(tcb_t *tcb)
{
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tcbInReleaseQueue(tcb->tcbState) == false);
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = ಗಂಟು_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    ಯದಿ (tcb_queue_empty(queue) || new_time < tcbReadyTime(queue.head)) {
        ಗಂಟು_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } ಅನ್ಯಥಾ {
        ಯದಿ (tcbReadyTime(queue.end) <= new_time) {
            ಗಂಟು_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } ಅನ್ಯಥಾ {
            tcb_t *after;
            after = find_time_after(queue.head, new_time);
            tcb_queue_ಸೇರಿಸು(tcb, after);
        }
    }

    thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, true);

    ಯದಿ (queue.head != ಗಂಟು_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).head) {
        ಗಂಟು_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#endif

cptr_t PURE getExtraCPtr(word_t *bufferPtr, word_t i)
{
    ಪ್ರತ್ಯಾಗಮ (cptr_t)bufferPtr[se_L4_ಸಂದೇಶ_Max_Length + 2 + i];
}

ಶೂನ್ಯ setExtraBadge(word_t *bufferPtr, word_t badge,
                   word_t i)
{
    bufferPtr[se_L4_ಸಂದೇಶ_Max_Length + 2 + i] = badge;
}

#ifndef CONFIG_KERNEL_MCS
ಶೂನ್ಯ setupCallerCap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *replySlot, *callerSlot;
    cap_t masterCap UNUSED, callerCap UNUSED;

    setThreadState(sender, ThreadState_BlockedOnReply);
    replySlot = TCB_PTR_CTE_PTR(sender, tcbReply);
    masterCap = replySlot->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(masterCap) == cap_reply_cap);
    ಪ್ರತಿಜ್ಞಾ(cap_reply_cap_get_capReplyMaster(masterCap));
    ಪ್ರತಿಜ್ಞಾ(cap_reply_cap_get_capReplyCanGrant(masterCap));
    ಪ್ರತಿಜ್ಞಾ(TCB_PTR(cap_reply_cap_get_capTCBPtr(masterCap)) == sender);
    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = callerSlot->cap;
    /* Haskell error: "Caller cap must not already exist" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(callerCap) == cap_null_cap);
    cte_ಸೇರಿಸು(cap_reply_cap_new(canGrant, false, TCB_REF(sender)),
              replySlot, callerSlot);
}

ಶೂನ್ಯ deleteCallerCap(tcb_t *receiver)
{
    cte_t *callerSlot;

    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(callerSlot);
}
#endif

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, word_t *bufferPtr, seL4_MessageInfo_t info)
{
    lookupSlot_raw_ret_t lu_ret;
    cptr_t cptr;
    word_t i, length;

    ಯದಿ (!bufferPtr) {
        current_extra_caps.excaprefs[0] = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }

    length = seL4_MessageInfo_get_extraCaps(info);

    ಪ್ರತಿ (i = 0; i < length; i++) {
        cptr = getExtraCPtr(bufferPtr, i);

        lu_ret = lookupSlot(thread, cptr);
        ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    ಯದಿ (i < se_L4_ಸಂದೇಶ_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    ಪ್ರತಿ (i = 0; i < n && i < n_ಸಂದೇಶ_Registers; i++) {
        setRegister(receiver, ಸಂದೇಶ_Registers[i],
                    getRegister(sender, ಸಂದೇಶ_Registers[i]));
    }

    ಯದಿ (!recvBuf || !sendBuf) {
        ಪ್ರತ್ಯಾಗಮ i;
    }

    /* Copy out-of-line words */
    ಪ್ರತಿ (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    ಪ್ರತ್ಯಾಗಮ i;
}

#ifdef ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
ಶೂನ್ಯ remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    ಯದಿ (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = ಗಂಟು_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        ಯದಿ (targetCurThread == ಗಂಟು_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcbPriority > targetCurThread->tcbPriority
#ifdef CONFIG_KERNEL_MCS
            || ಗಂಟು_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#endif
           ) {
            ARCH_ಗಂಟು_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
ಶೂನ್ಯ remoteTCBStall(tcb_t *tcb)
{

    ಯದಿ (
#ifdef CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#endif
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        ಗಂಟು_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_ಗಂಟು_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#ifndef CONFIG_KERNEL_MCS
ಸ್ಥಿರ exception_t invokeTCB_SetAffinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcbSchedDequeue(thread);
    migrateTCB(thread, affinity);
    ಯದಿ (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    ಯದಿ (thread == ಗಂಟು_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeSetAffinity(cap_t cap, word_t length, word_t *buffer)
{
    tcb_t *tcb;
    word_t affinity;

    ಯದಿ (length < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, buffer);
    ಯದಿ (affinity >= ks_ಅಂಕೆ_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeTCB_SetAffinity(tcb, affinity);
}
#endif
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_HARDWARE_DEBUG_API
ಸ್ಥಿರ exception_t invokeConfigureSingleStepping(bool_t call, word_t *buffer, tcb_t *t,
                                                 uint16_t bp_ಅಂಕೆ, word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    word_t ಮೌಲ್ಯ;

    bp_was_consumed = configureSingleStepping(t, bp_ಅಂಕೆ, n_instrs, false);
    ಯದಿ (n_instrs == 0) {
#ifndef CONFIG_ARCH_AARCH64
        unsetBreakpointUsedFlag(t, bp_ಅಂಕೆ);
#endif /* CONFIG_ARCH_AARCH64 */
        ಮೌಲ್ಯ = false;
    } ಅನ್ಯಥಾ {
#ifndef CONFIG_ARCH_AARCH64
        setBreakpointUsedFlag(t, bp_ಅಂಕೆ);
#endif /* CONFIG_ARCH_AARCH64 */
        ಮೌಲ್ಯ = bp_was_consumed;
    }

    ಯದಿ (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned ಪೂರ್ಣಾಙ್ಕ length = setMR(thread, buffer, 0, ಮೌಲ್ಯ);
        setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *buffer)
{
    uint16_t bp_ಅಂಕೆ;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_ಅಂಕೆ = getSyscallArg(0, buffer);
    n_instrs = getSyscallArg(1, buffer);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_ಅಂಕೆ, n_instrs, false);
    ಯದಿ (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeConfigureSingleStepping(call, buffer, tcb, bp_ಅಂಕೆ, n_instrs);
}

ಸ್ಥಿರ exception_t invokeSetBreakpoint(tcb_t *tcb, uint16_t bp_ಅಂಕೆ,
                                       word_t vaddr, word_t type, word_t size, word_t rw)
{
    setBreakpoint(tcb, bp_ಅಂಕೆ, vaddr, type, size, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    setBreakpointUsedFlag(tcb, bp_ಅಂಕೆ);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeSetBreakpoint(cap_t cap, word_t *buffer)
{
    uint16_t bp_ಅಂಕೆ;
    word_t vaddr, type, size, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ಅಂಕೆ = getSyscallArg(0, buffer);
    vaddr = getSyscallArg(1, buffer);
    type = getSyscallArg(2, buffer);
    size = getSyscallArg(3, buffer);
    rw = getSyscallArg(4, buffer);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    ಯದಿ (vaddr >= (word_t)USER_ತುದಿ) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (type != seL4_InstructionBreakpoint && type != seL4_DataBreakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 2;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    } ಅನ್ಯಥಾ ಯದಿ (type == seL4_InstructionBreakpoint) {
        ಯದಿ (size != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 3;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ಯದಿ (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 4;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ಯದಿ ((seL4_FirstWatchpoint == -1 || bp_ಅಂಕೆ >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 2;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
    } ಅನ್ಯಥಾ ಯದಿ (type == seL4_DataBreakpoint) {
        ಯದಿ (size == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 3;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ಯದಿ (seL4_FirstWatchpoint != -1 && bp_ಅಂಕೆ < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 2;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
    } ಅನ್ಯಥಾ ಯದಿ (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 2;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 3;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (size != 0 && size != 1 && size != 2 && size != 4 && size != 8) {
        userError("Debug: Invalid size %lu.", size);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 3;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (size > 0 && vaddr & (size - 1)) {
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
                  vaddr, size);

        current_syscall_error.type = seL4_AlignmentError;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decodeSetBreakpoint(tcb, bp_ಅಂಕೆ, vaddr, type, size, rw);
    ಯದಿ (error.type != seL4_NoError) {
        current_syscall_error = error;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeSetBreakpoint(tcb, bp_ಅಂಕೆ,
                               vaddr, type, size, rw);
}

ಸ್ಥಿರ exception_t invokeGetBreakpoint(bool_t call, word_t *buffer, tcb_t *tcb, uint16_t bp_ಅಂಕೆ)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_ಅಂಕೆ);
    ಯದಿ (call) {
        setRegister(thread, badgeRegister, 0);
        setMR(ಗಂಟು_STATE(ksCurThread), buffer, 0, res.vaddr);
        setMR(ಗಂಟು_STATE(ksCurThread), buffer, 1, res.type);
        setMR(ಗಂಟು_STATE(ksCurThread), buffer, 2, res.size);
        setMR(ಗಂಟು_STATE(ksCurThread), buffer, 3, res.rw);
        setMR(ಗಂಟು_STATE(ksCurThread), buffer, 4, res.is_enabled);
        setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, 5)));
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_ಅಂಕೆ;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ಅಂಕೆ = getSyscallArg(0, buffer);

    error = Arch_decodeGetBreakpoint(tcb, bp_ಅಂಕೆ);
    ಯದಿ (error.type != seL4_NoError) {
        current_syscall_error = error;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeGetBreakpoint(call, buffer, tcb, bp_ಅಂಕೆ);
}

ಸ್ಥಿರ exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_ಅಂಕೆ)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_ಅಂಕೆ);
    unsetBreakpointUsedFlag(tcb, bp_ಅಂಕೆ);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeUnsetBreakpoint(cap_t cap, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_ಅಂಕೆ;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ಅಂಕೆ = getSyscallArg(0, buffer);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_ಅಂಕೆ);
    ಯದಿ (error.type != seL4_NoError) {
        current_syscall_error = error;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeUnsetBreakpoint(tcb, bp_ಅಂಕೆ);
}
#endif /* CONFIG_HARDWARE_DEBUG_API */

ಸ್ಥಿರ exception_t invokeSetTLSBase(tcb_t *thread, word_t tls_base)
{
    setRegister(thread, TLS_BASE, tls_base);
    ಯದಿ (thread == ಗಂಟು_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeSetTLSBase(cap_t cap, word_t length, word_t *buffer)
{
    word_t tls_base;

    ಯದಿ (length < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    tls_base = getSyscallArg(0, buffer);

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeSetTLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_base);
}

ಸ್ಥಿರ ಶೂನ್ಯ invokeSetFlags(tcb_t *thread, word_t clear, word_t set, bool_t call)
{
    tcb_t *cur_thread = ಗಂಟು_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= set & seL4_TCBFlag_MASK;
    thread->tcbFlags = flags;

#ifdef CONFIG_HAVE_FPU
    ಯದಿ (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } ಅನ್ಯಥಾ ಯದಿ (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#endif
    ಯದಿ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, cur_thread);
        setRegister(cur_thread, badgeRegister, 0);
        unsigned ಪೂರ್ಣಾಙ್ಕ length = setMR(cur_thread, ipcBuffer, 0, flags);
        setRegister(cur_thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(cur_thread, ThreadState_Running);
}

ಸ್ಥಿರ exception_t decodeSetFlags(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ಯದಿ (length < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, buffer);
    word_t set   = getSyscallArg(1, buffer);

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    invokeSetFlags(thread, clear, set, call);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(word_t invLabel, word_t length, cap_t cap,
                                cte_t *slot, bool_t call, word_t *buffer)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ TCBReadRegisters:
        /* Second level of decoding */
        ಪ್ರತ್ಯಾಗಮ decodeReadRegisters(cap, length, call, buffer);

    ಪಕ್ಷ TCBWriteRegisters:
        ಪ್ರತ್ಯಾಗಮ decodeWriteRegisters(cap, length, buffer);

    ಪಕ್ಷ TCBCopyRegisters:
        ಪ್ರತ್ಯಾಗಮ decodeCopyRegisters(cap, length, buffer);

    ಪಕ್ಷ TCBSuspend:
        /* Jump straight to the invoke */
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    ಪಕ್ಷ TCBResume:
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    ಪಕ್ಷ TCBConfigure:
        ಪ್ರತ್ಯಾಗಮ decodeTCBConfigure(cap, length, slot, buffer);

    ಪಕ್ಷ TCBSetPriority:
        ಪ್ರತ್ಯಾಗಮ decodeSetPriority(cap, length, buffer);

    ಪಕ್ಷ TCBSetMCPriority:
        ಪ್ರತ್ಯಾಗಮ decodeSetMCPriority(cap, length, buffer);

    ಪಕ್ಷ TCBSetSchedParams:
#ifdef CONFIG_KERNEL_MCS
        ಪ್ರತ್ಯಾಗಮ decodeSetSchedParams(cap, length, slot, buffer);
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ decodeSetSchedParams(cap, length, buffer);
#endif

    ಪಕ್ಷ TCBSetIPCBuffer:
        ಪ್ರತ್ಯಾಗಮ decodeSetIPCBuffer(cap, length, slot, buffer);

    ಪಕ್ಷ TCBSetSpace:
        ಪ್ರತ್ಯಾಗಮ decodeSetSpace(cap, length, slot, buffer);

    ಪಕ್ಷ TCBBindNotification:
        ಪ್ರತ್ಯಾಗಮ decodeBindNotification(cap);

    ಪಕ್ಷ TCBUnbindNotification:
        ಪ್ರತ್ಯಾಗಮ decodeUnbindNotification(cap);

#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ TCBSetTimeoutEndpoint:
        ಪ್ರತ್ಯಾಗಮ decodeSetTimeoutEndpoint(cap, slot);
#ಅನ್ಯಥಾ
#ifdef ENABLE_SMP_SUPPORT
    ಪಕ್ಷ TCBSetAffinity:
        ಪ್ರತ್ಯಾಗಮ decodeSetAffinity(cap, length, buffer);
#endif /* ENABLE_SMP_SUPPORT */
#endif

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ifdef CONFIG_VTX
    ಪಕ್ಷ TCBSetEPTRoot:
        ಪ್ರತ್ಯಾಗಮ decodeSetEPTRoot(cap);
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    ಪಕ್ಷ TCBConfigureSingleStepping:
        ಪ್ರತ್ಯಾಗಮ decodeConfigureSingleStepping(cap, call, buffer);

    ಪಕ್ಷ TCBSetBreakpoint:
        ಪ್ರತ್ಯಾಗಮ decodeSetBreakpoint(cap, buffer);

    ಪಕ್ಷ TCBGetBreakpoint:
        ಪ್ರತ್ಯಾಗಮ decodeGetBreakpoint(cap, call, buffer);

    ಪಕ್ಷ TCBUnsetBreakpoint:
        ಪ್ರತ್ಯಾಗಮ decodeUnsetBreakpoint(cap, buffer);
#endif

    ಪಕ್ಷ TCBSetTLSBase:
        ಪ್ರತ್ಯಾಗಮ decodeSetTLSBase(cap, length, buffer);

    ಪಕ್ಷ TCBSetFlags:
        ಪ್ರತ್ಯಾಗಮ decodeSetFlags(cap, length, call, buffer);

    ಸ್ವಭಾವ:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}

ಗಣನಾ CopyRegistersFlags {
    CopyRegisters_suspendSource = 0,
    CopyRegisters_resumeTarget = 1,
    CopyRegisters_transferFrame = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t transferArch;
    tcb_t *srcTCB;
    cap_t source_cap;
    word_t flags;

    ಯದಿ (length < 1 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } ಅನ್ಯಥಾ {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & BIT(CopyRegisters_suspendSource),
               flags & BIT(CopyRegisters_resumeTarget),
               flags & BIT(CopyRegisters_transferFrame),
               flags & BIT(CopyRegisters_transferInteger),
               transferArch);

}

ಗಣನಾ ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, word_t length, bool_t call,
                                word_t *buffer)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    ಯದಿ (length < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    n     = getSyscallArg(1, buffer);

    ಯದಿ (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (ಪೂರ್ಣಾಙ್ಕ)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ಯದಿ (thread == ಗಂಟು_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

ಗಣನಾ WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    ಯದಿ (length < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    w     = getSyscallArg(1, buffer);

    ಯದಿ (length - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (ಪೂರ್ಣಾಙ್ಕ)(length - 2), (ಪೂರ್ಣಾಙ್ಕ)w);
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ಯದಿ (thread == ಗಂಟು_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, buffer);
}

#ifdef CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_endpoint_cap:
        ಪ್ರತ್ಯಾಗಮ (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_capCanGrantReply(cap)));
    ಪಕ್ಷ cap_null_cap:
        /* just has no fault endpoint */
        ಪ್ರತ್ಯಾಗಮ true;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ false;
    }
}
#endif

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cte_t *bufferSlot, *c_ಬೇರು_Slot, *v_ಬೇರು_Slot;
    cap_t bufferCap, c_ಬೇರು_Cap, v_ಬೇರು_Cap;
    deriveCap_ret_t dc_ret;
    word_t c_ಬೇರು_Data, v_ಬೇರು_Data, bufferAddr;
#ifdef CONFIG_KERNEL_MCS
#define TCBCONFIGURE_ARGS 3
#ಅನ್ಯಥಾ
#define TCBCONFIGURE_ARGS 4
#endif
    ಯದಿ (length < TCBCONFIGURE_ARGS || current_extra_caps.excaprefs[0] == ರಿಕ್ತ
        || current_extra_caps.excaprefs[1] == ರಿಕ್ತ
        || current_extra_caps.excaprefs[2] == ರಿಕ್ತ) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_ಬೇರು_Data     = getSyscallArg(0, buffer);
    v_ಬೇರು_Data     = getSyscallArg(1, buffer);
    bufferAddr    = getSyscallArg(2, buffer);
#ಅನ್ಯಥಾ
    cptr_t faultEP       = getSyscallArg(0, buffer);
    c_ಬೇರು_Data     = getSyscallArg(1, buffer);
    v_ಬೇರು_Data     = getSyscallArg(2, buffer);
    bufferAddr    = getSyscallArg(3, buffer);
#endif

    c_ಬೇರು_Slot  = current_extra_caps.excaprefs[0];
    c_ಬೇರು_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ಬೇರು_Slot  = current_extra_caps.excaprefs[1];
    v_ಬೇರು_Cap   = current_extra_caps.excaprefs[1]->cap;
    bufferSlot = current_extra_caps.excaprefs[2];
    bufferCap  = current_extra_caps.excaprefs[2]->cap;

    ಯದಿ (bufferAddr == 0) {
        bufferSlot = ರಿಕ್ತ;
    } ಅನ್ಯಥಾ {
        dc_ret = deriveCap(bufferSlot, bufferCap);
        ಯದಿ (dc_ret.status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ dc_ret.status;
        }
        bufferCap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(bufferAddr, bufferCap);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }
    }

    ಯದಿ (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (c_ಬೇರು_Data != 0) {
        c_ಬೇರು_Cap = updateCapData(false, c_ಬೇರು_Data, c_ಬೇರು_Cap);
    }

    dc_ret = deriveCap(c_ಬೇರು_Slot, c_ಬೇರು_Cap);
    ಯದಿ (dc_ret.status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ dc_ret.status;
    }
    c_ಬೇರು_Cap = dc_ret.cap;

    ಯದಿ (cap_get_capType(c_ಬೇರು_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (v_ಬೇರು_Data != 0) {
        v_ಬೇರು_Cap = updateCapData(false, v_ಬೇರು_Data, v_ಬೇರು_Cap);
    }

    dc_ret = deriveCap(v_ಬೇರು_Slot, v_ಬೇರು_Cap);
    ಯದಿ (dc_ret.status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ dc_ret.status;
    }
    v_ಬೇರು_Cap = dc_ret.cap;

    ಯದಿ (!is_Valid_VTable_ಬೇರು(v_ಬೇರು_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               c_ಬೇರು_Cap, c_ಬೇರು_Slot,
               v_ಬೇರು_Cap, v_ಬೇರು_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_buffer);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_ಬೇರು_Cap, c_ಬೇರು_Slot,
               v_ಬೇರು_Cap, v_ಬೇರು_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_update_space |
               thread_control_update_ipc_buffer);
#endif
}

exception_t decodeSetPriority(cap_t cap, word_t length, word_t *buffer)
{
    ಯದಿ (length < 1 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newPrio, authTCB);
    ಯದಿ (status != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        ಪ್ರತ್ಯಾಗಮ status;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               NULL_PRIO, newPrio,
               ರಿಕ್ತ, thread_control_sched_update_priority);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ರಿಕ್ತ,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               0, cap_null_cap_new(),
               ರಿಕ್ತ, thread_control_update_priority);
#endif
}

exception_t decodeSetMCPriority(cap_t cap, word_t length, word_t *buffer)
{
    ಯದಿ (length < 1 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    ಯದಿ (status != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        ಪ್ರತ್ಯಾಗಮ status;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               newMcp, NULL_PRIO,
               ರಿಕ್ತ, thread_control_sched_update_mcp);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ರಿಕ್ತ,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               0, cap_null_cap_new(),
               ರಿಕ್ತ, thread_control_update_mcp);
#endif
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetTimeoutEndpoint(cap_t cap, cte_t *slot)
{
    ಯದಿ (current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    ಯದಿ (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), ರಿಕ್ತ,
               thCap, thSlot,
               cap_null_cap_new(), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               0, cap_null_cap_new(), ರಿಕ್ತ,
               thread_control_caps_update_timeout);
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetSchedParams(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
#ಅನ್ಯಥಾ
exception_t decodeSetSchedParams(cap_t cap, word_t length, word_t *buffer)
#endif
{
    ಯದಿ (length < 2 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == ರಿಕ್ತ || current_extra_caps.excaprefs[2] == ರಿಕ್ತ
#endif
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    prio_t newPrio = getSyscallArg(1, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ifdef CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#endif

    ಯದಿ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    ಯದಿ (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        ಪ್ರತ್ಯಾಗಮ status;
    }

    status = checkPrio(newPrio, authTCB);
    ಯದಿ (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        ಪ್ರತ್ಯಾಗಮ status;
    }

#ifdef CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = ರಿಕ್ತ;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_priority |
                                         thread_control_sched_update_fault;
    ವಿಕಲ್ಪ (cap_get_capType(scCap)) {
    ಪಕ್ಷ cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        ಯದಿ (tcb->tcbSchedContext != sc) {
            ಯದಿ (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }
            ಯದಿ (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }
        }
        ಯದಿ (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_null_cap:
        ಯದಿ (tcb == ಗಂಟು_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 2;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    ಯದಿ (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    ಯದಿ (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 3;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
#endif
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ರಿಕ್ತ,
               0, newMcp, newPrio,
               cap_null_cap_new(), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               0, cap_null_cap_new(),
               ರಿಕ್ತ, thread_control_update_mcp |
               thread_control_update_priority);
#endif
}


exception_t decodeSetIPCBuffer(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cptr_t cptr_bufferPtr;
    cap_t bufferCap;
    cte_t *bufferSlot;

    ಯದಿ (length < 1 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    cptr_bufferPtr  = getSyscallArg(0, buffer);
    bufferSlot = current_extra_caps.excaprefs[0];
    bufferCap  = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (cptr_bufferPtr == 0) {
        bufferSlot = ರಿಕ್ತ;
    } ಅನ್ಯಥಾ {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(bufferSlot, bufferCap);
        ಯದಿ (dc_ret.status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ dc_ret.status;
        }
        bufferCap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_bufferPtr, bufferCap);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_caps_update_ipc_buffer);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), ರಿಕ್ತ,
               cap_null_cap_new(), ರಿಕ್ತ,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_update_ipc_buffer);

#endif
}

#ifdef CONFIG_KERNEL_MCS
#define DECODE_SET_SPACE_PARAMS 2
#ಅನ್ಯಥಾ
#define DECODE_SET_SPACE_PARAMS 3
#endif
exception_t decodeSetSpace(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    word_t c_ಬೇರು_Data, v_ಬೇರು_Data;
    cte_t *c_ಬೇರು_Slot, *v_ಬೇರು_Slot;
    cap_t c_ಬೇರು_Cap, v_ಬೇರು_Cap;
    deriveCap_ret_t dc_ret;

    ಯದಿ (length < DECODE_SET_SPACE_PARAMS || current_extra_caps.excaprefs[0] == ರಿಕ್ತ
        || current_extra_caps.excaprefs[1] == ರಿಕ್ತ
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == ರಿಕ್ತ
#endif
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_ಬೇರು_Data = getSyscallArg(0, buffer);
    v_ಬೇರು_Data = getSyscallArg(1, buffer);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_ಬೇರು_Slot  = current_extra_caps.excaprefs[1];
    c_ಬೇರು_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_ಬೇರು_Slot  = current_extra_caps.excaprefs[2];
    v_ಬೇರು_Cap   = current_extra_caps.excaprefs[2]->cap;
#ಅನ್ಯಥಾ
    cptr_t faultEP   = getSyscallArg(0, buffer);
    c_ಬೇರು_Data = getSyscallArg(1, buffer);
    v_ಬೇರು_Data = getSyscallArg(2, buffer);

    c_ಬೇರು_Slot  = current_extra_caps.excaprefs[0];
    c_ಬೇರು_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ಬೇರು_Slot  = current_extra_caps.excaprefs[1];
    v_ಬೇರು_Cap   = current_extra_caps.excaprefs[1]->cap;
#endif

    ಯದಿ (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (c_ಬೇರು_Data != 0) {
        c_ಬೇರು_Cap = updateCapData(false, c_ಬೇರು_Data, c_ಬೇರು_Cap);
    }

    dc_ret = deriveCap(c_ಬೇರು_Slot, c_ಬೇರು_Cap);
    ಯದಿ (dc_ret.status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ dc_ret.status;
    }
    c_ಬೇರು_Cap = dc_ret.cap;

    ಯದಿ (cap_get_capType(c_ಬೇರು_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (v_ಬೇರು_Data != 0) {
        v_ಬೇರು_Cap = updateCapData(false, v_ಬೇರು_Data, v_ಬೇರು_Cap);
    }

    dc_ret = deriveCap(v_ಬೇರು_Slot, v_ಬೇರು_Cap);
    ಯದಿ (dc_ret.status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ dc_ret.status;
    }
    v_ಬೇರು_Cap = dc_ret.cap;

    ಯದಿ (!is_Valid_VTable_ಬೇರು(v_ಬೇರು_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    /* fault handler */
    ಯದಿ (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
#endif

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), ರಿಕ್ತ,
               c_ಬೇರು_Cap, c_ಬೇರು_Slot,
               v_ಬೇರು_Cap, v_ಬೇರು_Slot,
               0, cap_null_cap_new(), ರಿಕ್ತ, thread_control_caps_update_space | thread_control_caps_update_fault);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_ಬೇರು_Cap, c_ಬೇರು_Slot,
               v_ಬೇರು_Cap, v_ಬೇರು_Slot,
               0, cap_null_cap_new(), ರಿಕ್ತ, thread_control_update_space);
#endif
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    ಯದಿ (current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ಯದಿ (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } ಅನ್ಯಥಾ {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ ((tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }


    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ಯದಿ (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeTCB_NotificationControl(tcb, ರಿಕ್ತ);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_index_t index, cap_t newCap, cte_t *srcSlot)
{
    cte_t *ಬೇರು_Slot = TCB_PTR_CTE_PTR(target, index);
    UNUSED exception_t e = cteDelete(ಬೇರು_Slot, true);
    ಯದಿ (e != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    ಯದಿ (cap_get_capType(newCap) != cap_null_cap) {
        ಯದಿ (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ಸೇರಿಸು(newCap, srcSlot, ಬೇರು_Slot);
        }
    }
    ಪ್ರತ್ಯಾಗಮ e;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t c_ಬೇರು_new_Cap, cte_t *c_ಬೇರು_src_Slot,
                                        cap_t v_ಬೇರು_new_Cap, cte_t *v_ಬೇರು_src_Slot,
                                        word_t bufferAddr, cap_t bufferCap,
                                        cte_t *bufferSrcSlot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    ಯದಿ (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }

    }

    ಯದಿ (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }
    }

    ಯದಿ (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, c_ಬೇರು_new_Cap, c_ಬೇರು_src_Slot);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, v_ಬೇರು_new_Cap, v_ಬೇರು_src_Slot);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }
    }

    ಯದಿ (updateFlags & thread_control_caps_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }
        target->tcbIPCBuffer = bufferAddr;

        ಯದಿ (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ಸೇರಿಸು(bufferCap, bufferSrcSlot, bufferSlot);
        }

        ಯದಿ (target == ಗಂಟು_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#ಅನ್ಯಥಾ
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t priority,
                                    cap_t c_ಬೇರು_new_Cap, cte_t *c_ಬೇರು_src_Slot,
                                    cap_t v_ಬೇರು_new_Cap, cte_t *v_ಬೇರು_src_Slot,
                                    word_t bufferAddr, cap_t bufferCap,
                                    cte_t *bufferSrcSlot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    ಯದಿ (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    ಯದಿ (updateFlags & thread_control_update_mcp) {
        setMCPriority(target, mcp);
    }

    ಯದಿ (updateFlags & thread_control_update_space) {
        cte_t *ಬೇರು_Slot;

        ಬೇರು_Slot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(ಬೇರು_Slot, true);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }
        ಯದಿ (sameObjectAs(c_ಬೇರು_new_Cap, c_ಬೇರು_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ಸೇರಿಸು(c_ಬೇರು_new_Cap, c_ಬೇರು_src_Slot, ಬೇರು_Slot);
        }
    }

    ಯದಿ (updateFlags & thread_control_update_space) {
        cte_t *ಬೇರು_Slot;

        ಬೇರು_Slot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(ಬೇರು_Slot, true);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }
        ಯದಿ (sameObjectAs(v_ಬೇರು_new_Cap, v_ಬೇರು_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ಸೇರಿಸು(v_ಬೇರು_new_Cap, v_ಬೇರು_src_Slot, ಬೇರು_Slot);
        }
    }

    ಯದಿ (updateFlags & thread_control_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }
        target->tcbIPCBuffer = bufferAddr;

        ಯದಿ (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ಸೇರಿಸು(bufferCap, bufferSrcSlot, bufferSlot);
        }

        ಯದಿ (target == ಗಂಟು_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ಯದಿ (updateFlags & thread_control_update_priority) {
        setPriority(target, priority);
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t priority,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    ಯದಿ (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        ಯದಿ (e != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ e;
        }
    }

    ಯದಿ (updateFlags & thread_control_sched_update_mcp) {
        setMCPriority(target, mcp);
    }

    ಯದಿ (updateFlags & thread_control_sched_update_priority) {
        setPriority(target, priority);
    }

    ಯದಿ (updateFlags & thread_control_sched_update_sc) {
        ಯದಿ (sc != ರಿಕ್ತ) {
            schedContext_bindTCB(sc, target);
        } ಅನ್ಯಥಾ ಯದಿ (sc == ರಿಕ್ತ) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#endif

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    ಯದಿ (suspendSource) {
        suspend(tcb_src);
    }

    ಯದಿ (resumeTarget) {
        restart(dest);
    }

    ಯದಿ (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        ಪ್ರತಿ (i = 0; i < n_frameRegisters; i++) {
            v = getRegister(tcb_src, frameRegisters[i]);
            setRegister(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        setNextPC(dest, pc);
    }

    ಯದಿ (transferInteger) {
        word_t i;
        word_t v;

        ಪ್ರತಿ (i = 0; i < n_gpRegisters; i++) {
            v = getRegister(tcb_src, gpRegisters[i]);
            setRegister(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    ಯದಿ (dest == ಗಂಟು_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    ಪ್ರತ್ಯಾಗಮ Arch_performTransfer(transferArch, tcb_src, dest);
}

/* ReadRegisters is a special case: replyFromKernel & setMRs are
 * unfolded here, in order to avoid passing the large reply message up
 * to the top level in a global (and double-copying). We prevent the
 * top-level replyFromKernel_success_empty() from running by setting the
 * thread state. Retype does this too.
 */
exception_t invokeTCB_ReadRegisters(tcb_t *tcb_src, bool_t suspendSource,
                                    word_t n, word_t arch, bool_t call)
{
    word_t i, j;
    exception_t e;
    tcb_t *thread;

    thread = ಗಂಟು_STATE(ksCurThread);

    ಯದಿ (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, ಗಂಟು_STATE(ksCurThread));
    ಯದಿ (e != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ e;
    }

    ಯದಿ (call) {
        word_t *ipcBuffer;

        ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);

        ಪ್ರತಿ (i = 0; i < n && i < n_frameRegisters && i < n_ಸಂದೇಶ_Registers; i++) {
            setRegister(thread, ಸಂದೇಶ_Registers[i],
                        getRegister(tcb_src, frameRegisters[i]));
        }

        ಯದಿ (ipcBuffer != ರಿಕ್ತ && i < n && i < n_frameRegisters) {
            ಪ್ರತಿ (; i < n && i < n_frameRegisters; i++) {
                ipcBuffer[i + 1] = getRegister(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        ಪ್ರತಿ (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_ಸಂದೇಶ_Registers; i++) {
            setRegister(thread, ಸಂದೇಶ_Registers[i + n_frameRegisters],
                        getRegister(tcb_src, gpRegisters[i]));
        }

        ಯದಿ (ipcBuffer != ರಿಕ್ತ && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            ಪ್ರತಿ (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipcBuffer[i + n_frameRegisters + 1] =
                    getRegister(tcb_src, gpRegisters[i]);
            }
        }

        setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, i + j)));
    }
    setThreadState(thread, ThreadState_Running);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *buffer)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, ಗಂಟು_STATE(ksCurThread), dest);
    ಯದಿ (e != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ e;
    }

    ಯದಿ (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_getSanitiseRegisterInfo(dest);

    ಪ್ರತಿ (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        setRegister(dest, frameRegisters[i],
                    sanitiseRegister(frameRegisters[i],
                                     getSyscallArg(i + 2, buffer), archInfo));
    }

    ಪ್ರತಿ (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        setRegister(dest, gpRegisters[i],
                    sanitiseRegister(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   buffer), archInfo));
    }

    pc = getRestartPC(dest);
    setNextPC(dest, pc);

    Arch_postModifyRegisters(dest);

    ಯದಿ (resumeTarget) {
        restart(dest);
    }

    ಯದಿ (dest == ಗಂಟು_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    ಯದಿ (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } ಅನ್ಯಥಾ {
        unbindNotification(tcb);
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD
ಶೂನ್ಯ setThreadName(tcb_t *tcb, ಧ್ರುವ ವರ್ಣ *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_LENGTH);
}
#endif

word_t setMRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    ವಿಕಲ್ಪ (current_syscall_error.type) {
    ಪಕ್ಷ seL4_InvalidArgument:
        ಪ್ರತ್ಯಾಗಮ setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ);

    ಪಕ್ಷ seL4_InvalidCapability:
        ಪ್ರತ್ಯಾಗಮ setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ);

    ಪಕ್ಷ seL4_IllegalOperation:
        ಪ್ರತ್ಯಾಗಮ 0;

    ಪಕ್ಷ seL4_RangeError:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        ಪ್ರತ್ಯಾಗಮ setMR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    ಪಕ್ಷ seL4_AlignmentError:
        ಪ್ರತ್ಯಾಗಮ 0;

    ಪಕ್ಷ seL4_FailedLookup:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        ಪ್ರತ್ಯಾಗಮ setMRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    ಪಕ್ಷ seL4_TruncatedMessage:
    ಪಕ್ಷ seL4_DeleteFirst:
    ಪಕ್ಷ seL4_RevokeFirst:
        ಪ್ರತ್ಯಾಗಮ 0;
    ಪಕ್ಷ seL4_NotEnoughMemory:
        ಪ್ರತ್ಯಾಗಮ setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.memory_ಎಡ);
    ಸ್ವಭಾವ:
        fail("Invalid syscall error");
    }
}
