/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <api/invocation.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <sel4/shared_types.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <object/objecttype.h>
#ଅନ୍ତର୍ଭାଵ <object/cnode.h>
#ifdef CONFIG_KERNEL_MCS
#ଅନ୍ତର୍ଭାଵ <object/schedcontext.h>
#endif
#ଅନ୍ତର୍ଭାଵ <object/tcb.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <arch/smp/ipi_inline.h>

#define NULL_PRIO 0

ସ୍ଥିର exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    ପ୍ରତିଜ୍ଞା(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    ଯଦି (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ addToBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & MASK(wordRadix));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & MASK(wordRadix));
    ଯଦି (unlikely(!ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcbSchedNext;

    ଯଦି (queue.head == tcb && queue.end == tcb) {
        queue.head = ଖାଲି;
        queue.end = ଖାଲି;
    } ଅନ୍ୟଥା {
        ଯଦି (queue.head == tcb) {
            after->tcbSchedPrev = ଖାଲି;
            tcb->tcbSchedNext = ଖାଲି;
            queue.head = after;
        } ଅନ୍ୟଥା {
            ଯଦି (queue.end == tcb) {
                before->tcbSchedNext = ଖାଲି;
                tcb->tcbSchedPrev = ଖାଲି;
                queue.end = before;
            } ଅନ୍ୟଥା {
                before->tcbSchedNext = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = ଖାଲି;
                tcb->tcbSchedNext = ଖାଲି;
            }
        }
    }

    ଫେରନ୍ତୁ queue;
}

/* Add TCB to the head of a scheduler queue */
ଶୂନ୍ୟ tcbSchedEnqueue(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    ପ୍ରତିଜ୍ଞା(isSchedulable(tcb));
    ପ୍ରତିଜ୍ଞା(refill_sufficient(tcb->tcbSchedContext, 0));
#endif

    ଯଦି (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ଯଦି (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
ଶୂନ୍ୟ tcbSchedAppend(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    ପ୍ରତିଜ୍ଞା(isSchedulable(tcb));
    ପ୍ରତିଜ୍ଞା(refill_sufficient(tcb->tcbSchedContext, 0));
    ପ୍ରତିଜ୍ଞା(refill_ready(tcb->tcbSchedContext));
#endif
    ଯଦି (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ଯଦି (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
ଶୂନ୍ୟ tcbSchedDequeue(tcb_t *tcb)
{
    ଯଦି (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        ଗଣ୍ଠି_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, false);

        ଯଦି (likely(tcb_queue_empty(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
ଶୂନ୍ୟ tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = ଖାଲି;

    debug_tcb->tcbDebugNext = ଗଣ୍ଠି_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    ଯଦି (ଗଣ୍ଠି_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(ଗଣ୍ଠି_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    ଗଣ୍ଠି_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

ଶୂନ୍ୟ tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    ପ୍ରତିଜ୍ଞା(ଗଣ୍ଠି_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != ଖାଲି);
    ଯଦି (tcb == ଗଣ୍ଠି_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        ଗଣ୍ଠି_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(ଗଣ୍ଠି_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcbDebugNext;
    } ଅନ୍ୟଥା {
        ପ୍ରତିଜ୍ଞା(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcbDebugNext = debug_tcb->tcbDebugNext;
    }

    ଯଦି (debug_tcb->tcbDebugNext) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugNext)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = ଖାଲି;
    debug_tcb->tcbDebugNext = ଖାଲି;
}
#endif /* CONFIG_DEBUG_BUILD */

#ifndef CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    ଯଦି (!queue.head) { /* Empty list */
        queue.head = tcb;
    } ଅନ୍ୟଥା {
        queue.end->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.end;
    tcb->tcbEPNext = ଖାଲି;
    queue.end = tcb;

    ଫେରନ୍ତୁ queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    ଯଦି (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } ଅନ୍ୟଥା {
        queue.head = tcb->tcbEPNext;
    }

    ଯଦି (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } ଅନ୍ୟଥା {
        queue.end = tcb->tcbEPPrev;
    }

    ଫେରନ୍ତୁ queue;
}
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS

ଶୂନ୍ୟ tcbReleaseRemove(tcb_t *tcb)
{
    ଯଦି (likely(thread_state_get_tcbInReleaseQueue(tcb->tcbState))) {
        tcb_queue_t queue = ଗଣ୍ଠି_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        ଯଦି (queue.head == tcb) {
            ଗଣ୍ଠି_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        ଗଣ୍ଠି_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, false);
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ticks_t PURE tcbReadyTime(tcb_t *tcb)
{
    ଫେରନ୍ତୁ refill_head(tcb->tcbSchedContext)->rTime;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool_t PURE time_after(tcb_t *tcb, ticks_t new_time)
{
    ଫେରନ୍ତୁ tcb != ଖାଲି && new_time >= tcbReadyTime(tcb);
}

ସ୍ଥିର tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    ଯେତେବେଳେ (time_after(after, new_time)) {
        after = after->tcbSchedNext;
    }

    ଫେରନ୍ତୁ after;
}

ଶୂନ୍ୟ tcbReleaseEnqueue(tcb_t *tcb)
{
    ପ୍ରତିଜ୍ଞା(thread_state_get_tcbInReleaseQueue(tcb->tcbState) == false);
    ପ୍ରତିଜ୍ଞା(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = ଗଣ୍ଠି_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    ଯଦି (tcb_queue_empty(queue) || new_time < tcbReadyTime(queue.head)) {
        ଗଣ୍ଠି_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } ଅନ୍ୟଥା {
        ଯଦି (tcbReadyTime(queue.end) <= new_time) {
            ଗଣ୍ଠି_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } ଅନ୍ୟଥା {
            tcb_t *after;
            after = find_time_after(queue.head, new_time);
            tcb_queue_ଭର୍ତ୍ତି(tcb, after);
        }
    }

    thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, true);

    ଯଦି (queue.head != ଗଣ୍ଠି_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).head) {
        ଗଣ୍ଠି_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#endif

cptr_t PURE getExtraCPtr(word_t *bufferPtr, word_t i)
{
    ଫେରନ୍ତୁ (cptr_t)bufferPtr[se_L4_ସନ୍ଦେଶ_Max_Length + 2 + i];
}

ଶୂନ୍ୟ setExtraBadge(word_t *bufferPtr, word_t badge,
                   word_t i)
{
    bufferPtr[se_L4_ସନ୍ଦେଶ_Max_Length + 2 + i] = badge;
}

#ifndef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ setupCallerCap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *replySlot, *callerSlot;
    cap_t masterCap UNUSED, callerCap UNUSED;

    setThreadState(sender, ThreadState_BlockedOnReply);
    replySlot = TCB_PTR_CTE_PTR(sender, tcbReply);
    masterCap = replySlot->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    ପ୍ରତିଜ୍ଞା(cap_get_capType(masterCap) == cap_reply_cap);
    ପ୍ରତିଜ୍ଞା(cap_reply_cap_get_capReplyMaster(masterCap));
    ପ୍ରତିଜ୍ଞା(cap_reply_cap_get_capReplyCanGrant(masterCap));
    ପ୍ରତିଜ୍ଞା(TCB_PTR(cap_reply_cap_get_capTCBPtr(masterCap)) == sender);
    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = callerSlot->cap;
    /* Haskell error: "Caller cap must not already exist" */
    ପ୍ରତିଜ୍ଞା(cap_get_capType(callerCap) == cap_null_cap);
    cte_ଭର୍ତ୍ତି(cap_reply_cap_new(canGrant, false, TCB_REF(sender)),
              replySlot, callerSlot);
}

ଶୂନ୍ୟ deleteCallerCap(tcb_t *receiver)
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

    ଯଦି (!bufferPtr) {
        current_extra_caps.excaprefs[0] = ଖାଲି;
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }

    length = seL4_MessageInfo_get_extraCaps(info);

    ପାଇଁ (i = 0; i < length; i++) {
        cptr = getExtraCPtr(bufferPtr, i);

        lu_ret = lookupSlot(thread, cptr);
        ଯଦି (lu_ret.status != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            ଫେରନ୍ତୁ lu_ret.status;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    ଯଦି (i < se_L4_ସନ୍ଦେଶ_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = ଖାଲି;
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    ପାଇଁ (i = 0; i < n && i < n_ସନ୍ଦେଶ_Registers; i++) {
        setRegister(receiver, ସନ୍ଦେଶ_Registers[i],
                    getRegister(sender, ସନ୍ଦେଶ_Registers[i]));
    }

    ଯଦି (!recvBuf || !sendBuf) {
        ଫେରନ୍ତୁ i;
    }

    /* Copy out-of-line words */
    ପାଇଁ (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    ଫେରନ୍ତୁ i;
}

#ifdef ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
ଶୂନ୍ୟ remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    ଯଦି (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = ଗଣ୍ଠି_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        ଯଦି (targetCurThread == ଗଣ୍ଠି_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcbPriority > targetCurThread->tcbPriority
#ifdef CONFIG_KERNEL_MCS
            || ଗଣ୍ଠି_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#endif
           ) {
            ARCH_ଗଣ୍ଠି_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
ଶୂନ୍ୟ remoteTCBStall(tcb_t *tcb)
{

    ଯଦି (
#ifdef CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#endif
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        ଗଣ୍ଠି_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_ଗଣ୍ଠି_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#ifndef CONFIG_KERNEL_MCS
ସ୍ଥିର exception_t invokeTCB_SetAffinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcbSchedDequeue(thread);
    migrateTCB(thread, affinity);
    ଯଦି (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    ଯଦି (thread == ଗଣ୍ଠି_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeSetAffinity(cap_t cap, word_t length, word_t *buffer)
{
    tcb_t *tcb;
    word_t affinity;

    ଯଦି (length < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, buffer);
    ଯଦି (affinity >= ks_ଅଙ୍କ_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeTCB_SetAffinity(tcb, affinity);
}
#endif
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_HARDWARE_DEBUG_API
ସ୍ଥିର exception_t invokeConfigureSingleStepping(bool_t call, word_t *buffer, tcb_t *t,
                                                 uint16_t bp_ଅଙ୍କ, word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = ଗଣ୍ଠି_STATE(ksCurThread);
    word_t ମୂଲ୍ୟ;

    bp_was_consumed = configureSingleStepping(t, bp_ଅଙ୍କ, n_instrs, false);
    ଯଦି (n_instrs == 0) {
#ifndef CONFIG_ARCH_AARCH64
        unsetBreakpointUsedFlag(t, bp_ଅଙ୍କ);
#endif /* CONFIG_ARCH_AARCH64 */
        ମୂଲ୍ୟ = false;
    } ଅନ୍ୟଥା {
#ifndef CONFIG_ARCH_AARCH64
        setBreakpointUsedFlag(t, bp_ଅଙ୍କ);
#endif /* CONFIG_ARCH_AARCH64 */
        ମୂଲ୍ୟ = bp_was_consumed;
    }

    ଯଦି (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length = setMR(thread, buffer, 0, ମୂଲ୍ୟ);
        setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Running);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *buffer)
{
    uint16_t bp_ଅଙ୍କ;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_ଅଙ୍କ = getSyscallArg(0, buffer);
    n_instrs = getSyscallArg(1, buffer);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_ଅଙ୍କ, n_instrs, false);
    ଯଦି (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeConfigureSingleStepping(call, buffer, tcb, bp_ଅଙ୍କ, n_instrs);
}

ସ୍ଥିର exception_t invokeSetBreakpoint(tcb_t *tcb, uint16_t bp_ଅଙ୍କ,
                                       word_t vaddr, word_t type, word_t size, word_t rw)
{
    setBreakpoint(tcb, bp_ଅଙ୍କ, vaddr, type, size, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    setBreakpointUsedFlag(tcb, bp_ଅଙ୍କ);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeSetBreakpoint(cap_t cap, word_t *buffer)
{
    uint16_t bp_ଅଙ୍କ;
    word_t vaddr, type, size, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ଅଙ୍କ = getSyscallArg(0, buffer);
    vaddr = getSyscallArg(1, buffer);
    type = getSyscallArg(2, buffer);
    size = getSyscallArg(3, buffer);
    rw = getSyscallArg(4, buffer);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    ଯଦି (vaddr >= (word_t)USER_ଶୀର୍ଷ) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (type != seL4_InstructionBreakpoint && type != seL4_DataBreakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 2;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    } ଅନ୍ୟଥା ଯଦି (type == seL4_InstructionBreakpoint) {
        ଯଦି (size != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 3;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ଯଦି (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 4;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ଯଦି ((seL4_FirstWatchpoint == -1 || bp_ଅଙ୍କ >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 2;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
    } ଅନ୍ୟଥା ଯଦି (type == seL4_DataBreakpoint) {
        ଯଦି (size == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 3;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ଯଦି (seL4_FirstWatchpoint != -1 && bp_ଅଙ୍କ < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 2;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
    } ଅନ୍ୟଥା ଯଦି (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 2;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 3;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (size != 0 && size != 1 && size != 2 && size != 4 && size != 8) {
        userError("Debug: Invalid size %lu.", size);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 3;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (size > 0 && vaddr & (size - 1)) {
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
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decodeSetBreakpoint(tcb, bp_ଅଙ୍କ, vaddr, type, size, rw);
    ଯଦି (error.type != seL4_NoError) {
        current_syscall_error = error;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeSetBreakpoint(tcb, bp_ଅଙ୍କ,
                               vaddr, type, size, rw);
}

ସ୍ଥିର exception_t invokeGetBreakpoint(bool_t call, word_t *buffer, tcb_t *tcb, uint16_t bp_ଅଙ୍କ)
{
    tcb_t *thread;
    thread = ଗଣ୍ଠି_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_ଅଙ୍କ);
    ଯଦି (call) {
        setRegister(thread, badgeRegister, 0);
        setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, 0, res.vaddr);
        setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, 1, res.type);
        setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, 2, res.size);
        setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, 3, res.rw);
        setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, 4, res.is_enabled);
        setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, 5)));
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Running);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_ଅଙ୍କ;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ଅଙ୍କ = getSyscallArg(0, buffer);

    error = Arch_decodeGetBreakpoint(tcb, bp_ଅଙ୍କ);
    ଯଦି (error.type != seL4_NoError) {
        current_syscall_error = error;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeGetBreakpoint(call, buffer, tcb, bp_ଅଙ୍କ);
}

ସ୍ଥିର exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_ଅଙ୍କ)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_ଅଙ୍କ);
    unsetBreakpointUsedFlag(tcb, bp_ଅଙ୍କ);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeUnsetBreakpoint(cap_t cap, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_ଅଙ୍କ;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_ଅଙ୍କ = getSyscallArg(0, buffer);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_ଅଙ୍କ);
    ଯଦି (error.type != seL4_NoError) {
        current_syscall_error = error;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeUnsetBreakpoint(tcb, bp_ଅଙ୍କ);
}
#endif /* CONFIG_HARDWARE_DEBUG_API */

ସ୍ଥିର exception_t invokeSetTLSBase(tcb_t *thread, word_t tls_base)
{
    setRegister(thread, TLS_BASE, tls_base);
    ଯଦି (thread == ଗଣ୍ଠି_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeSetTLSBase(cap_t cap, word_t length, word_t *buffer)
{
    word_t tls_base;

    ଯଦି (length < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    tls_base = getSyscallArg(0, buffer);

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeSetTLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_base);
}

ସ୍ଥିର ଶୂନ୍ୟ invokeSetFlags(tcb_t *thread, word_t clear, word_t set, bool_t call)
{
    tcb_t *cur_thread = ଗଣ୍ଠି_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= set & seL4_TCBFlag_MASK;
    thread->tcbFlags = flags;

#ifdef CONFIG_HAVE_FPU
    ଯଦି (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } ଅନ୍ୟଥା ଯଦି (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#endif
    ଯଦି (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, cur_thread);
        setRegister(cur_thread, badgeRegister, 0);
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length = setMR(cur_thread, ipcBuffer, 0, flags);
        setRegister(cur_thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(cur_thread, ThreadState_Running);
}

ସ୍ଥିର exception_t decodeSetFlags(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ଯଦି (length < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, buffer);
    word_t set   = getSyscallArg(1, buffer);

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    invokeSetFlags(thread, clear, set, call);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
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

    ବାଛ (invLabel) {
    ସ୍ଥିତି TCBReadRegisters:
        /* Second level of decoding */
        ଫେରନ୍ତୁ decodeReadRegisters(cap, length, call, buffer);

    ସ୍ଥିତି TCBWriteRegisters:
        ଫେରନ୍ତୁ decodeWriteRegisters(cap, length, buffer);

    ସ୍ଥିତି TCBCopyRegisters:
        ଫେରନ୍ତୁ decodeCopyRegisters(cap, length, buffer);

    ସ୍ଥିତି TCBSuspend:
        /* Jump straight to the invoke */
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    ସ୍ଥିତି TCBResume:
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    ସ୍ଥିତି TCBConfigure:
        ଫେରନ୍ତୁ decodeTCBConfigure(cap, length, slot, buffer);

    ସ୍ଥିତି TCBSetPriority:
        ଫେରନ୍ତୁ decodeSetPriority(cap, length, buffer);

    ସ୍ଥିତି TCBSetMCPriority:
        ଫେରନ୍ତୁ decodeSetMCPriority(cap, length, buffer);

    ସ୍ଥିତି TCBSetSchedParams:
#ifdef CONFIG_KERNEL_MCS
        ଫେରନ୍ତୁ decodeSetSchedParams(cap, length, slot, buffer);
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ decodeSetSchedParams(cap, length, buffer);
#endif

    ସ୍ଥିତି TCBSetIPCBuffer:
        ଫେରନ୍ତୁ decodeSetIPCBuffer(cap, length, slot, buffer);

    ସ୍ଥିତି TCBSetSpace:
        ଫେରନ୍ତୁ decodeSetSpace(cap, length, slot, buffer);

    ସ୍ଥିତି TCBBindNotification:
        ଫେରନ୍ତୁ decodeBindNotification(cap);

    ସ୍ଥିତି TCBUnbindNotification:
        ଫେରନ୍ତୁ decodeUnbindNotification(cap);

#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି TCBSetTimeoutEndpoint:
        ଫେରନ୍ତୁ decodeSetTimeoutEndpoint(cap, slot);
#ଅନ୍ୟଥା
#ifdef ENABLE_SMP_SUPPORT
    ସ୍ଥିତି TCBSetAffinity:
        ଫେରନ୍ତୁ decodeSetAffinity(cap, length, buffer);
#endif /* ENABLE_SMP_SUPPORT */
#endif

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ifdef CONFIG_VTX
    ସ୍ଥିତି TCBSetEPTRoot:
        ଫେରନ୍ତୁ decodeSetEPTRoot(cap);
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    ସ୍ଥିତି TCBConfigureSingleStepping:
        ଫେରନ୍ତୁ decodeConfigureSingleStepping(cap, call, buffer);

    ସ୍ଥିତି TCBSetBreakpoint:
        ଫେରନ୍ତୁ decodeSetBreakpoint(cap, buffer);

    ସ୍ଥିତି TCBGetBreakpoint:
        ଫେରନ୍ତୁ decodeGetBreakpoint(cap, call, buffer);

    ସ୍ଥିତି TCBUnsetBreakpoint:
        ଫେରନ୍ତୁ decodeUnsetBreakpoint(cap, buffer);
#endif

    ସ୍ଥିତି TCBSetTLSBase:
        ଫେରନ୍ତୁ decodeSetTLSBase(cap, length, buffer);

    ସ୍ଥିତି TCBSetFlags:
        ଫେରନ୍ତୁ decodeSetFlags(cap, length, call, buffer);

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}

ଗଣନା CopyRegistersFlags {
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

    ଯଦି (length < 1 || current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    ଯଦି (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } ଅନ୍ୟଥା {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & BIT(CopyRegisters_suspendSource),
               flags & BIT(CopyRegisters_resumeTarget),
               flags & BIT(CopyRegisters_transferFrame),
               flags & BIT(CopyRegisters_transferInteger),
               transferArch);

}

ଗଣନା ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, word_t length, bool_t call,
                                word_t *buffer)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    ଯଦି (length < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    n     = getSyscallArg(1, buffer);

    ଯଦି (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ଯଦି (thread == ଗଣ୍ଠି_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

ଗଣନା WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    ଯଦି (length < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    w     = getSyscallArg(1, buffer);

    ଯଦି (length - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(length - 2), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)w);
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ଯଦି (thread == ଗଣ୍ଠି_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, buffer);
}

#ifdef CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_endpoint_cap:
        ଫେରନ୍ତୁ (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_capCanGrantReply(cap)));
    ସ୍ଥିତି cap_null_cap:
        /* just has no fault endpoint */
        ଫେରନ୍ତୁ true;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ false;
    }
}
#endif

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cte_t *bufferSlot, *c_ମୂଳ_Slot, *v_ମୂଳ_Slot;
    cap_t bufferCap, c_ମୂଳ_Cap, v_ମୂଳ_Cap;
    deriveCap_ret_t dc_ret;
    word_t c_ମୂଳ_Data, v_ମୂଳ_Data, bufferAddr;
#ifdef CONFIG_KERNEL_MCS
#define TCBCONFIGURE_ARGS 3
#ଅନ୍ୟଥା
#define TCBCONFIGURE_ARGS 4
#endif
    ଯଦି (length < TCBCONFIGURE_ARGS || current_extra_caps.excaprefs[0] == ଖାଲି
        || current_extra_caps.excaprefs[1] == ଖାଲି
        || current_extra_caps.excaprefs[2] == ଖାଲି) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_ମୂଳ_Data     = getSyscallArg(0, buffer);
    v_ମୂଳ_Data     = getSyscallArg(1, buffer);
    bufferAddr    = getSyscallArg(2, buffer);
#ଅନ୍ୟଥା
    cptr_t faultEP       = getSyscallArg(0, buffer);
    c_ମୂଳ_Data     = getSyscallArg(1, buffer);
    v_ମୂଳ_Data     = getSyscallArg(2, buffer);
    bufferAddr    = getSyscallArg(3, buffer);
#endif

    c_ମୂଳ_Slot  = current_extra_caps.excaprefs[0];
    c_ମୂଳ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ମୂଳ_Slot  = current_extra_caps.excaprefs[1];
    v_ମୂଳ_Cap   = current_extra_caps.excaprefs[1]->cap;
    bufferSlot = current_extra_caps.excaprefs[2];
    bufferCap  = current_extra_caps.excaprefs[2]->cap;

    ଯଦି (bufferAddr == 0) {
        bufferSlot = ଖାଲି;
    } ଅନ୍ୟଥା {
        dc_ret = deriveCap(bufferSlot, bufferCap);
        ଯଦି (dc_ret.status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ dc_ret.status;
        }
        bufferCap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(bufferAddr, bufferCap);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }
    }

    ଯଦି (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (c_ମୂଳ_Data != 0) {
        c_ମୂଳ_Cap = updateCapData(false, c_ମୂଳ_Data, c_ମୂଳ_Cap);
    }

    dc_ret = deriveCap(c_ମୂଳ_Slot, c_ମୂଳ_Cap);
    ଯଦି (dc_ret.status != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ dc_ret.status;
    }
    c_ମୂଳ_Cap = dc_ret.cap;

    ଯଦି (cap_get_capType(c_ମୂଳ_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (v_ମୂଳ_Data != 0) {
        v_ମୂଳ_Cap = updateCapData(false, v_ମୂଳ_Data, v_ମୂଳ_Cap);
    }

    dc_ret = deriveCap(v_ମୂଳ_Slot, v_ମୂଳ_Cap);
    ଯଦି (dc_ret.status != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ dc_ret.status;
    }
    v_ମୂଳ_Cap = dc_ret.cap;

    ଯଦି (!is_Valid_VTable_ମୂଳ(v_ମୂଳ_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ଫେରନ୍ତୁ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               c_ମୂଳ_Cap, c_ମୂଳ_Slot,
               v_ମୂଳ_Cap, v_ମୂଳ_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_buffer);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_ମୂଳ_Cap, c_ମୂଳ_Slot,
               v_ମୂଳ_Cap, v_ମୂଳ_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_update_space |
               thread_control_update_ipc_buffer);
#endif
}

exception_t decodeSetPriority(cap_t cap, word_t length, word_t *buffer)
{
    ଯଦି (length < 1 || current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ଯଦି (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newPrio, authTCB);
    ଯଦି (status != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        ଫେରନ୍ତୁ status;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ଫେରନ୍ତୁ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               NULL_PRIO, newPrio,
               ଖାଲି, thread_control_sched_update_priority);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ଖାଲି,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               0, cap_null_cap_new(),
               ଖାଲି, thread_control_update_priority);
#endif
}

exception_t decodeSetMCPriority(cap_t cap, word_t length, word_t *buffer)
{
    ଯଦି (length < 1 || current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ଯଦି (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    ଯଦି (status != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        ଫେରନ୍ତୁ status;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ଫେରନ୍ତୁ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               newMcp, NULL_PRIO,
               ଖାଲି, thread_control_sched_update_mcp);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ଖାଲି,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               0, cap_null_cap_new(),
               ଖାଲି, thread_control_update_mcp);
#endif
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetTimeoutEndpoint(cap_t cap, cte_t *slot)
{
    ଯଦି (current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    ଯଦି (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), ଖାଲି,
               thCap, thSlot,
               cap_null_cap_new(), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               0, cap_null_cap_new(), ଖାଲି,
               thread_control_caps_update_timeout);
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetSchedParams(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
#ଅନ୍ୟଥା
exception_t decodeSetSchedParams(cap_t cap, word_t length, word_t *buffer)
#endif
{
    ଯଦି (length < 2 || current_extra_caps.excaprefs[0] == ଖାଲି
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == ଖାଲି || current_extra_caps.excaprefs[2] == ଖାଲି
#endif
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    prio_t newPrio = getSyscallArg(1, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ifdef CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#endif

    ଯଦି (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    ଯଦି (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        ଫେରନ୍ତୁ status;
    }

    status = checkPrio(newPrio, authTCB);
    ଯଦି (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        ଫେରନ୍ତୁ status;
    }

#ifdef CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = ଖାଲି;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_priority |
                                         thread_control_sched_update_fault;
    ବାଛ (cap_get_capType(scCap)) {
    ସ୍ଥିତି cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        ଯଦି (tcb->tcbSchedContext != sc) {
            ଯଦି (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
            }
            ଯଦି (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
            }
        }
        ଯଦି (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_null_cap:
        ଯଦି (tcb == ଗଣ୍ଠି_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 2;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    ଯଦି (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    ଯଦି (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 3;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
#endif
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ଫେରନ୍ତୁ invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ଖାଲି,
               0, newMcp, newPrio,
               cap_null_cap_new(), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               0, cap_null_cap_new(),
               ଖାଲି, thread_control_update_mcp |
               thread_control_update_priority);
#endif
}


exception_t decodeSetIPCBuffer(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cptr_t cptr_bufferPtr;
    cap_t bufferCap;
    cte_t *bufferSlot;

    ଯଦି (length < 1 || current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    cptr_bufferPtr  = getSyscallArg(0, buffer);
    bufferSlot = current_extra_caps.excaprefs[0];
    bufferCap  = current_extra_caps.excaprefs[0]->cap;

    ଯଦି (cptr_bufferPtr == 0) {
        bufferSlot = ଖାଲି;
    } ଅନ୍ୟଥା {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(bufferSlot, bufferCap);
        ଯଦି (dc_ret.status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ dc_ret.status;
        }
        bufferCap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_bufferPtr, bufferCap);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ଫେରନ୍ତୁ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_caps_update_ipc_buffer);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), ଖାଲି,
               cap_null_cap_new(), ଖାଲି,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_update_ipc_buffer);

#endif
}

#ifdef CONFIG_KERNEL_MCS
#define DECODE_SET_SPACE_PARAMS 2
#ଅନ୍ୟଥା
#define DECODE_SET_SPACE_PARAMS 3
#endif
exception_t decodeSetSpace(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    word_t c_ମୂଳ_Data, v_ମୂଳ_Data;
    cte_t *c_ମୂଳ_Slot, *v_ମୂଳ_Slot;
    cap_t c_ମୂଳ_Cap, v_ମୂଳ_Cap;
    deriveCap_ret_t dc_ret;

    ଯଦି (length < DECODE_SET_SPACE_PARAMS || current_extra_caps.excaprefs[0] == ଖାଲି
        || current_extra_caps.excaprefs[1] == ଖାଲି
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == ଖାଲି
#endif
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_ମୂଳ_Data = getSyscallArg(0, buffer);
    v_ମୂଳ_Data = getSyscallArg(1, buffer);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_ମୂଳ_Slot  = current_extra_caps.excaprefs[1];
    c_ମୂଳ_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_ମୂଳ_Slot  = current_extra_caps.excaprefs[2];
    v_ମୂଳ_Cap   = current_extra_caps.excaprefs[2]->cap;
#ଅନ୍ୟଥା
    cptr_t faultEP   = getSyscallArg(0, buffer);
    c_ମୂଳ_Data = getSyscallArg(1, buffer);
    v_ମୂଳ_Data = getSyscallArg(2, buffer);

    c_ମୂଳ_Slot  = current_extra_caps.excaprefs[0];
    c_ମୂଳ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ମୂଳ_Slot  = current_extra_caps.excaprefs[1];
    v_ମୂଳ_Cap   = current_extra_caps.excaprefs[1]->cap;
#endif

    ଯଦି (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (c_ମୂଳ_Data != 0) {
        c_ମୂଳ_Cap = updateCapData(false, c_ମୂଳ_Data, c_ମୂଳ_Cap);
    }

    dc_ret = deriveCap(c_ମୂଳ_Slot, c_ମୂଳ_Cap);
    ଯଦି (dc_ret.status != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ dc_ret.status;
    }
    c_ମୂଳ_Cap = dc_ret.cap;

    ଯଦି (cap_get_capType(c_ମୂଳ_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (v_ମୂଳ_Data != 0) {
        v_ମୂଳ_Cap = updateCapData(false, v_ମୂଳ_Data, v_ମୂଳ_Cap);
    }

    dc_ret = deriveCap(v_ମୂଳ_Slot, v_ମୂଳ_Cap);
    ଯଦି (dc_ret.status != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ dc_ret.status;
    }
    v_ମୂଳ_Cap = dc_ret.cap;

    ଯଦି (!is_Valid_VTable_ମୂଳ(v_ମୂଳ_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    /* fault handler */
    ଯଦି (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
#endif

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    ଫେରନ୍ତୁ invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), ଖାଲି,
               c_ମୂଳ_Cap, c_ମୂଳ_Slot,
               v_ମୂଳ_Cap, v_ମୂଳ_Slot,
               0, cap_null_cap_new(), ଖାଲି, thread_control_caps_update_space | thread_control_caps_update_fault);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_ମୂଳ_Cap, c_ମୂଳ_Slot,
               v_ମୂଳ_Cap, v_ମୂଳ_Slot,
               0, cap_null_cap_new(), ଖାଲି, thread_control_update_space);
#endif
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    ଯଦି (current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ଯଦି (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    ଯଦି (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } ଅନ୍ୟଥା {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି ((tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }


    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ଯଦି (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeTCB_NotificationControl(tcb, ଖାଲି);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ସ୍ଥିର ଧାଡ଼ିଭିତରେ exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_index_t index, cap_t newCap, cte_t *srcSlot)
{
    cte_t *ମୂଳ_Slot = TCB_PTR_CTE_PTR(target, index);
    UNUSED exception_t e = cteDelete(ମୂଳ_Slot, true);
    ଯଦି (e != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    ଯଦି (cap_get_capType(newCap) != cap_null_cap) {
        ଯଦି (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ଭର୍ତ୍ତି(newCap, srcSlot, ମୂଳ_Slot);
        }
    }
    ଫେରନ୍ତୁ e;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t c_ମୂଳ_new_Cap, cte_t *c_ମୂଳ_src_Slot,
                                        cap_t v_ମୂଳ_new_Cap, cte_t *v_ମୂଳ_src_Slot,
                                        word_t bufferAddr, cap_t bufferCap,
                                        cte_t *bufferSrcSlot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    ଯଦି (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }

    }

    ଯଦି (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }
    }

    ଯଦି (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, c_ମୂଳ_new_Cap, c_ମୂଳ_src_Slot);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, v_ମୂଳ_new_Cap, v_ମୂଳ_src_Slot);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }
    }

    ଯଦି (updateFlags & thread_control_caps_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }
        target->tcbIPCBuffer = bufferAddr;

        ଯଦି (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ଭର୍ତ୍ତି(bufferCap, bufferSrcSlot, bufferSlot);
        }

        ଯଦି (target == ଗଣ୍ଠି_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
#ଅନ୍ୟଥା
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t priority,
                                    cap_t c_ମୂଳ_new_Cap, cte_t *c_ମୂଳ_src_Slot,
                                    cap_t v_ମୂଳ_new_Cap, cte_t *v_ମୂଳ_src_Slot,
                                    word_t bufferAddr, cap_t bufferCap,
                                    cte_t *bufferSrcSlot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    ଯଦି (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    ଯଦି (updateFlags & thread_control_update_mcp) {
        setMCPriority(target, mcp);
    }

    ଯଦି (updateFlags & thread_control_update_space) {
        cte_t *ମୂଳ_Slot;

        ମୂଳ_Slot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(ମୂଳ_Slot, true);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }
        ଯଦି (sameObjectAs(c_ମୂଳ_new_Cap, c_ମୂଳ_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ଭର୍ତ୍ତି(c_ମୂଳ_new_Cap, c_ମୂଳ_src_Slot, ମୂଳ_Slot);
        }
    }

    ଯଦି (updateFlags & thread_control_update_space) {
        cte_t *ମୂଳ_Slot;

        ମୂଳ_Slot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(ମୂଳ_Slot, true);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }
        ଯଦି (sameObjectAs(v_ମୂଳ_new_Cap, v_ମୂଳ_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ଭର୍ତ୍ତି(v_ମୂଳ_new_Cap, v_ମୂଳ_src_Slot, ମୂଳ_Slot);
        }
    }

    ଯଦି (updateFlags & thread_control_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }
        target->tcbIPCBuffer = bufferAddr;

        ଯଦି (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ଭର୍ତ୍ତି(bufferCap, bufferSrcSlot, bufferSlot);
        }

        ଯଦି (target == ଗଣ୍ଠି_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ଯଦି (updateFlags & thread_control_update_priority) {
        setPriority(target, priority);
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t priority,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    ଯଦି (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        ଯଦି (e != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ e;
        }
    }

    ଯଦି (updateFlags & thread_control_sched_update_mcp) {
        setMCPriority(target, mcp);
    }

    ଯଦି (updateFlags & thread_control_sched_update_priority) {
        setPriority(target, priority);
    }

    ଯଦି (updateFlags & thread_control_sched_update_sc) {
        ଯଦି (sc != ଖାଲି) {
            schedContext_bindTCB(sc, target);
        } ଅନ୍ୟଥା ଯଦି (sc == ଖାଲି) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
#endif

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    ଯଦି (suspendSource) {
        suspend(tcb_src);
    }

    ଯଦି (resumeTarget) {
        restart(dest);
    }

    ଯଦି (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        ପାଇଁ (i = 0; i < n_frameRegisters; i++) {
            v = getRegister(tcb_src, frameRegisters[i]);
            setRegister(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        setNextPC(dest, pc);
    }

    ଯଦି (transferInteger) {
        word_t i;
        word_t v;

        ପାଇଁ (i = 0; i < n_gpRegisters; i++) {
            v = getRegister(tcb_src, gpRegisters[i]);
            setRegister(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    ଯଦି (dest == ଗଣ୍ଠି_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    ଫେରନ୍ତୁ Arch_performTransfer(transferArch, tcb_src, dest);
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

    thread = ଗଣ୍ଠି_STATE(ksCurThread);

    ଯଦି (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, ଗଣ୍ଠି_STATE(ksCurThread));
    ଯଦି (e != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ e;
    }

    ଯଦି (call) {
        word_t *ipcBuffer;

        ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);

        ପାଇଁ (i = 0; i < n && i < n_frameRegisters && i < n_ସନ୍ଦେଶ_Registers; i++) {
            setRegister(thread, ସନ୍ଦେଶ_Registers[i],
                        getRegister(tcb_src, frameRegisters[i]));
        }

        ଯଦି (ipcBuffer != ଖାଲି && i < n && i < n_frameRegisters) {
            ପାଇଁ (; i < n && i < n_frameRegisters; i++) {
                ipcBuffer[i + 1] = getRegister(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        ପାଇଁ (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_ସନ୍ଦେଶ_Registers; i++) {
            setRegister(thread, ସନ୍ଦେଶ_Registers[i + n_frameRegisters],
                        getRegister(tcb_src, gpRegisters[i]));
        }

        ଯଦି (ipcBuffer != ଖାଲି && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            ପାଇଁ (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipcBuffer[i + n_frameRegisters + 1] =
                    getRegister(tcb_src, gpRegisters[i]);
            }
        }

        setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, i + j)));
    }
    setThreadState(thread, ThreadState_Running);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *buffer)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, ଗଣ୍ଠି_STATE(ksCurThread), dest);
    ଯଦି (e != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ e;
    }

    ଯଦି (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_getSanitiseRegisterInfo(dest);

    ପାଇଁ (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        setRegister(dest, frameRegisters[i],
                    sanitiseRegister(frameRegisters[i],
                                     getSyscallArg(i + 2, buffer), archInfo));
    }

    ପାଇଁ (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        setRegister(dest, gpRegisters[i],
                    sanitiseRegister(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   buffer), archInfo));
    }

    pc = getRestartPC(dest);
    setNextPC(dest, pc);

    Arch_postModifyRegisters(dest);

    ଯଦି (resumeTarget) {
        restart(dest);
    }

    ଯଦି (dest == ଗଣ୍ଠି_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    ଯଦି (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } ଅନ୍ୟଥା {
        unbindNotification(tcb);
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD
ଶୂନ୍ୟ setThreadName(tcb_t *tcb, ଅଚଳ ଅକ୍ଷର *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_LENGTH);
}
#endif

word_t setMRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    ବାଛ (current_syscall_error.type) {
    ସ୍ଥିତି seL4_InvalidArgument:
        ଫେରନ୍ତୁ setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Argument_ସଂଖ୍ୟା);

    ସ୍ଥିତି seL4_InvalidCapability:
        ଫେରନ୍ତୁ setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_ସଂଖ୍ୟା);

    ସ୍ଥିତି seL4_IllegalOperation:
        ଫେରନ୍ତୁ 0;

    ସ୍ଥିତି seL4_RangeError:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        ଫେରନ୍ତୁ setMR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    ସ୍ଥିତି seL4_AlignmentError:
        ଫେରନ୍ତୁ 0;

    ସ୍ଥିତି seL4_FailedLookup:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        ଫେରନ୍ତୁ setMRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    ସ୍ଥିତି seL4_TruncatedMessage:
    ସ୍ଥିତି seL4_DeleteFirst:
    ସ୍ଥିତି seL4_RevokeFirst:
        ଫେରନ୍ତୁ 0;
    ସ୍ଥିତି seL4_NotEnoughMemory:
        ଫେରନ୍ତୁ setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.memory_ବାମ);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid syscall error");
    }
}
