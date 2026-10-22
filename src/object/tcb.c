/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <api/invocation.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <sel4/shared_types.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <object/objecttype.h>
#অন্তর্ভাব <object/cnode.h>
#ifdef CONFIG_KERNEL_MCS
#অন্তর্ভাব <object/schedcontext.h>
#endif
#অন্তর্ভাব <object/tcb.h>
#অন্তর্ভাব <kernel/cspace.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <util.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <arch/smp/ipi_inline.h>

#define NULL_PRIO 0

স্থির exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    প্রতিজ্ঞা(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    যদি (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    প্রত্যাগম EXCEPTION_NONE;
}

স্থির অন্তঃস্থ শূন্য addToBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    গ্রন্থি_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    গ্রন্থি_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & MASK(wordRadix));
}

স্থির অন্তঃস্থ শূন্য removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    গ্রন্থি_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & MASK(wordRadix));
    যদি (unlikely(!গ্রন্থি_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        গ্রন্থি_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcbSchedNext;

    যদি (queue.head == tcb && queue.end == tcb) {
        queue.head = রিক্ত;
        queue.end = রিক্ত;
    } অন্যথা {
        যদি (queue.head == tcb) {
            after->tcbSchedPrev = রিক্ত;
            tcb->tcbSchedNext = রিক্ত;
            queue.head = after;
        } অন্যথা {
            যদি (queue.end == tcb) {
                before->tcbSchedNext = রিক্ত;
                tcb->tcbSchedPrev = রিক্ত;
                queue.end = before;
            } অন্যথা {
                before->tcbSchedNext = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = রিক্ত;
                tcb->tcbSchedNext = রিক্ত;
            }
        }
    }

    প্রত্যাগম queue;
}

/* Add TCB to the head of a scheduler queue */
শূন্য tcbSchedEnqueue(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    প্রতিজ্ঞা(isSchedulable(tcb));
    প্রতিজ্ঞা(refill_sufficient(tcb->tcbSchedContext, 0));
#endif

    যদি (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = গ্রন্থি_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        যদি (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        গ্রন্থি_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
শূন্য tcbSchedAppend(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    প্রতিজ্ঞা(isSchedulable(tcb));
    প্রতিজ্ঞা(refill_sufficient(tcb->tcbSchedContext, 0));
    প্রতিজ্ঞা(refill_ready(tcb->tcbSchedContext));
#endif
    যদি (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = গ্রন্থি_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        যদি (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        গ্রন্থি_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
শূন্য tcbSchedDequeue(tcb_t *tcb)
{
    যদি (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = গ্রন্থি_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        গ্রন্থি_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, false);

        যদি (likely(tcb_queue_empty(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
শূন্য tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = রিক্ত;

    debug_tcb->tcbDebugNext = গ্রন্থি_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    যদি (গ্রন্থি_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(গ্রন্থি_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    গ্রন্থি_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

শূন্য tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    প্রতিজ্ঞা(গ্রন্থি_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != রিক্ত);
    যদি (tcb == গ্রন্থি_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        গ্রন্থি_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(গ্রন্থি_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcbDebugNext;
    } অন্যথা {
        প্রতিজ্ঞা(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcbDebugNext = debug_tcb->tcbDebugNext;
    }

    যদি (debug_tcb->tcbDebugNext) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugNext)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = রিক্ত;
    debug_tcb->tcbDebugNext = রিক্ত;
}
#endif /* CONFIG_DEBUG_BUILD */

#ifndef CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    যদি (!queue.head) { /* Empty list */
        queue.head = tcb;
    } অন্যথা {
        queue.end->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.end;
    tcb->tcbEPNext = রিক্ত;
    queue.end = tcb;

    প্রত্যাগম queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    যদি (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } অন্যথা {
        queue.head = tcb->tcbEPNext;
    }

    যদি (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } অন্যথা {
        queue.end = tcb->tcbEPPrev;
    }

    প্রত্যাগম queue;
}
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS

শূন্য tcbReleaseRemove(tcb_t *tcb)
{
    যদি (likely(thread_state_get_tcbInReleaseQueue(tcb->tcbState))) {
        tcb_queue_t queue = গ্রন্থি_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        যদি (queue.head == tcb) {
            গ্রন্থি_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        গ্রন্থি_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, false);
    }
}

স্থির অন্তঃস্থ ticks_t PURE tcbReadyTime(tcb_t *tcb)
{
    প্রত্যাগম refill_head(tcb->tcbSchedContext)->rTime;
}

স্থির অন্তঃস্থ bool_t PURE time_after(tcb_t *tcb, ticks_t new_time)
{
    প্রত্যাগম tcb != রিক্ত && new_time >= tcbReadyTime(tcb);
}

স্থির tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    যাবত্ (time_after(after, new_time)) {
        after = after->tcbSchedNext;
    }

    প্রত্যাগম after;
}

শূন্য tcbReleaseEnqueue(tcb_t *tcb)
{
    প্রতিজ্ঞা(thread_state_get_tcbInReleaseQueue(tcb->tcbState) == false);
    প্রতিজ্ঞা(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = গ্রন্থি_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    যদি (tcb_queue_empty(queue) || new_time < tcbReadyTime(queue.head)) {
        গ্রন্থি_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } অন্যথা {
        যদি (tcbReadyTime(queue.end) <= new_time) {
            গ্রন্থি_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } অন্যথা {
            tcb_t *after;
            after = find_time_after(queue.head, new_time);
            tcb_queue_ঢোকাও(tcb, after);
        }
    }

    thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, true);

    যদি (queue.head != গ্রন্থি_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).head) {
        গ্রন্থি_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#endif

cptr_t PURE getExtraCPtr(word_t *bufferPtr, word_t i)
{
    প্রত্যাগম (cptr_t)bufferPtr[se_L4_বার্তা_Max_Length + 2 + i];
}

শূন্য setExtraBadge(word_t *bufferPtr, word_t badge,
                   word_t i)
{
    bufferPtr[se_L4_বার্তা_Max_Length + 2 + i] = badge;
}

#ifndef CONFIG_KERNEL_MCS
শূন্য setupCallerCap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *replySlot, *callerSlot;
    cap_t masterCap UNUSED, callerCap UNUSED;

    setThreadState(sender, ThreadState_BlockedOnReply);
    replySlot = TCB_PTR_CTE_PTR(sender, tcbReply);
    masterCap = replySlot->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    প্রতিজ্ঞা(cap_get_capType(masterCap) == cap_reply_cap);
    প্রতিজ্ঞা(cap_reply_cap_get_capReplyMaster(masterCap));
    প্রতিজ্ঞা(cap_reply_cap_get_capReplyCanGrant(masterCap));
    প্রতিজ্ঞা(TCB_PTR(cap_reply_cap_get_capTCBPtr(masterCap)) == sender);
    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = callerSlot->cap;
    /* Haskell error: "Caller cap must not already exist" */
    প্রতিজ্ঞা(cap_get_capType(callerCap) == cap_null_cap);
    cte_ঢোকাও(cap_reply_cap_new(canGrant, false, TCB_REF(sender)),
              replySlot, callerSlot);
}

শূন্য deleteCallerCap(tcb_t *receiver)
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

    যদি (!bufferPtr) {
        current_extra_caps.excaprefs[0] = রিক্ত;
        প্রত্যাগম EXCEPTION_NONE;
    }

    length = seL4_MessageInfo_get_extraCaps(info);

    প্রতি (i = 0; i < length; i++) {
        cptr = getExtraCPtr(bufferPtr, i);

        lu_ret = lookupSlot(thread, cptr);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            প্রত্যাগম lu_ret.status;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    যদি (i < se_L4_বার্তা_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = রিক্ত;
    }

    প্রত্যাগম EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    প্রতি (i = 0; i < n && i < n_বার্তা_Registers; i++) {
        setRegister(receiver, বার্তা_Registers[i],
                    getRegister(sender, বার্তা_Registers[i]));
    }

    যদি (!recvBuf || !sendBuf) {
        প্রত্যাগম i;
    }

    /* Copy out-of-line words */
    প্রতি (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    প্রত্যাগম i;
}

#ifdef ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
শূন্য remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    যদি (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = গ্রন্থি_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        যদি (targetCurThread == গ্রন্থি_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcbPriority > targetCurThread->tcbPriority
#ifdef CONFIG_KERNEL_MCS
            || গ্রন্থি_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#endif
           ) {
            ARCH_গ্রন্থি_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
শূন্য remoteTCBStall(tcb_t *tcb)
{

    যদি (
#ifdef CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#endif
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        গ্রন্থি_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_গ্রন্থি_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#ifndef CONFIG_KERNEL_MCS
স্থির exception_t invokeTCB_SetAffinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcbSchedDequeue(thread);
    migrateTCB(thread, affinity);
    যদি (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    যদি (thread == গ্রন্থি_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    প্রত্যাগম EXCEPTION_NONE;
}

স্থির exception_t decodeSetAffinity(cap_t cap, word_t length, word_t *buffer)
{
    tcb_t *tcb;
    word_t affinity;

    যদি (length < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, buffer);
    যদি (affinity >= ks_অঙ্ক_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeTCB_SetAffinity(tcb, affinity);
}
#endif
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_HARDWARE_DEBUG_API
স্থির exception_t invokeConfigureSingleStepping(bool_t call, word_t *buffer, tcb_t *t,
                                                 uint16_t bp_অঙ্ক, word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = গ্রন্থি_STATE(ksCurThread);
    word_t মান;

    bp_was_consumed = configureSingleStepping(t, bp_অঙ্ক, n_instrs, false);
    যদি (n_instrs == 0) {
#ifndef CONFIG_ARCH_AARCH64
        unsetBreakpointUsedFlag(t, bp_অঙ্ক);
#endif /* CONFIG_ARCH_AARCH64 */
        মান = false;
    } অন্যথা {
#ifndef CONFIG_ARCH_AARCH64
        setBreakpointUsedFlag(t, bp_অঙ্ক);
#endif /* CONFIG_ARCH_AARCH64 */
        মান = bp_was_consumed;
    }

    যদি (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned পূর্ণাঙ্ক length = setMR(thread, buffer, 0, মান);
        setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);
    প্রত্যাগম EXCEPTION_NONE;
}

স্থির exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *buffer)
{
    uint16_t bp_অঙ্ক;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_অঙ্ক = getSyscallArg(0, buffer);
    n_instrs = getSyscallArg(1, buffer);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_অঙ্ক, n_instrs, false);
    যদি (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeConfigureSingleStepping(call, buffer, tcb, bp_অঙ্ক, n_instrs);
}

স্থির exception_t invokeSetBreakpoint(tcb_t *tcb, uint16_t bp_অঙ্ক,
                                       word_t vaddr, word_t type, word_t size, word_t rw)
{
    setBreakpoint(tcb, bp_অঙ্ক, vaddr, type, size, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    setBreakpointUsedFlag(tcb, bp_অঙ্ক);
    প্রত্যাগম EXCEPTION_NONE;
}

স্থির exception_t decodeSetBreakpoint(cap_t cap, word_t *buffer)
{
    uint16_t bp_অঙ্ক;
    word_t vaddr, type, size, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_অঙ্ক = getSyscallArg(0, buffer);
    vaddr = getSyscallArg(1, buffer);
    type = getSyscallArg(2, buffer);
    size = getSyscallArg(3, buffer);
    rw = getSyscallArg(4, buffer);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    যদি (vaddr >= (word_t)USER_শীর্ষ) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    যদি (type != seL4_InstructionBreakpoint && type != seL4_DataBreakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 2;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    } অন্যথা যদি (type == seL4_InstructionBreakpoint) {
        যদি (size != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 3;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        যদি (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 4;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        যদি ((seL4_FirstWatchpoint == -1 || bp_অঙ্ক >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 2;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
    } অন্যথা যদি (type == seL4_DataBreakpoint) {
        যদি (size == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 3;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        যদি (seL4_FirstWatchpoint != -1 && bp_অঙ্ক < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 2;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
    } অন্যথা যদি (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 2;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    যদি (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 3;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }
    যদি (size != 0 && size != 1 && size != 2 && size != 4 && size != 8) {
        userError("Debug: Invalid size %lu.", size);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 3;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }
    যদি (size > 0 && vaddr & (size - 1)) {
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
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decodeSetBreakpoint(tcb, bp_অঙ্ক, vaddr, type, size, rw);
    যদি (error.type != seL4_NoError) {
        current_syscall_error = error;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeSetBreakpoint(tcb, bp_অঙ্ক,
                               vaddr, type, size, rw);
}

স্থির exception_t invokeGetBreakpoint(bool_t call, word_t *buffer, tcb_t *tcb, uint16_t bp_অঙ্ক)
{
    tcb_t *thread;
    thread = গ্রন্থি_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_অঙ্ক);
    যদি (call) {
        setRegister(thread, badgeRegister, 0);
        setMR(গ্রন্থি_STATE(ksCurThread), buffer, 0, res.vaddr);
        setMR(গ্রন্থি_STATE(ksCurThread), buffer, 1, res.type);
        setMR(গ্রন্থি_STATE(ksCurThread), buffer, 2, res.size);
        setMR(গ্রন্থি_STATE(ksCurThread), buffer, 3, res.rw);
        setMR(গ্রন্থি_STATE(ksCurThread), buffer, 4, res.is_enabled);
        setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, 5)));
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);
    প্রত্যাগম EXCEPTION_NONE;
}

স্থির exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_অঙ্ক;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_অঙ্ক = getSyscallArg(0, buffer);

    error = Arch_decodeGetBreakpoint(tcb, bp_অঙ্ক);
    যদি (error.type != seL4_NoError) {
        current_syscall_error = error;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeGetBreakpoint(call, buffer, tcb, bp_অঙ্ক);
}

স্থির exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_অঙ্ক)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_অঙ্ক);
    unsetBreakpointUsedFlag(tcb, bp_অঙ্ক);
    প্রত্যাগম EXCEPTION_NONE;
}

স্থির exception_t decodeUnsetBreakpoint(cap_t cap, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_অঙ্ক;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_অঙ্ক = getSyscallArg(0, buffer);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_অঙ্ক);
    যদি (error.type != seL4_NoError) {
        current_syscall_error = error;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeUnsetBreakpoint(tcb, bp_অঙ্ক);
}
#endif /* CONFIG_HARDWARE_DEBUG_API */

স্থির exception_t invokeSetTLSBase(tcb_t *thread, word_t tls_base)
{
    setRegister(thread, TLS_BASE, tls_base);
    যদি (thread == গ্রন্থি_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    প্রত্যাগম EXCEPTION_NONE;
}

স্থির exception_t decodeSetTLSBase(cap_t cap, word_t length, word_t *buffer)
{
    word_t tls_base;

    যদি (length < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    tls_base = getSyscallArg(0, buffer);

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeSetTLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_base);
}

স্থির শূন্য invokeSetFlags(tcb_t *thread, word_t clear, word_t set, bool_t call)
{
    tcb_t *cur_thread = গ্রন্থি_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= set & seL4_TCBFlag_MASK;
    thread->tcbFlags = flags;

#ifdef CONFIG_HAVE_FPU
    যদি (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } অন্যথা যদি (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#endif
    যদি (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, cur_thread);
        setRegister(cur_thread, badgeRegister, 0);
        unsigned পূর্ণাঙ্ক length = setMR(cur_thread, ipcBuffer, 0, flags);
        setRegister(cur_thread, বার্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(cur_thread, ThreadState_Running);
}

স্থির exception_t decodeSetFlags(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    যদি (length < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, buffer);
    word_t set   = getSyscallArg(1, buffer);

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    invokeSetFlags(thread, clear, set, call);
    প্রত্যাগম EXCEPTION_NONE;
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

    বিকল্প (invLabel) {
    পক্ষ TCBReadRegisters:
        /* Second level of decoding */
        প্রত্যাগম decodeReadRegisters(cap, length, call, buffer);

    পক্ষ TCBWriteRegisters:
        প্রত্যাগম decodeWriteRegisters(cap, length, buffer);

    পক্ষ TCBCopyRegisters:
        প্রত্যাগম decodeCopyRegisters(cap, length, buffer);

    পক্ষ TCBSuspend:
        /* Jump straight to the invoke */
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        প্রত্যাগম invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    পক্ষ TCBResume:
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        প্রত্যাগম invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    পক্ষ TCBConfigure:
        প্রত্যাগম decodeTCBConfigure(cap, length, slot, buffer);

    পক্ষ TCBSetPriority:
        প্রত্যাগম decodeSetPriority(cap, length, buffer);

    পক্ষ TCBSetMCPriority:
        প্রত্যাগম decodeSetMCPriority(cap, length, buffer);

    পক্ষ TCBSetSchedParams:
#ifdef CONFIG_KERNEL_MCS
        প্রত্যাগম decodeSetSchedParams(cap, length, slot, buffer);
#অন্যথা
        প্রত্যাগম decodeSetSchedParams(cap, length, buffer);
#endif

    পক্ষ TCBSetIPCBuffer:
        প্রত্যাগম decodeSetIPCBuffer(cap, length, slot, buffer);

    পক্ষ TCBSetSpace:
        প্রত্যাগম decodeSetSpace(cap, length, slot, buffer);

    পক্ষ TCBBindNotification:
        প্রত্যাগম decodeBindNotification(cap);

    পক্ষ TCBUnbindNotification:
        প্রত্যাগম decodeUnbindNotification(cap);

#ifdef CONFIG_KERNEL_MCS
    পক্ষ TCBSetTimeoutEndpoint:
        প্রত্যাগম decodeSetTimeoutEndpoint(cap, slot);
#অন্যথা
#ifdef ENABLE_SMP_SUPPORT
    পক্ষ TCBSetAffinity:
        প্রত্যাগম decodeSetAffinity(cap, length, buffer);
#endif /* ENABLE_SMP_SUPPORT */
#endif

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ifdef CONFIG_VTX
    পক্ষ TCBSetEPTRoot:
        প্রত্যাগম decodeSetEPTRoot(cap);
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    পক্ষ TCBConfigureSingleStepping:
        প্রত্যাগম decodeConfigureSingleStepping(cap, call, buffer);

    পক্ষ TCBSetBreakpoint:
        প্রত্যাগম decodeSetBreakpoint(cap, buffer);

    পক্ষ TCBGetBreakpoint:
        প্রত্যাগম decodeGetBreakpoint(cap, call, buffer);

    পক্ষ TCBUnsetBreakpoint:
        প্রত্যাগম decodeUnsetBreakpoint(cap, buffer);
#endif

    পক্ষ TCBSetTLSBase:
        প্রত্যাগম decodeSetTLSBase(cap, length, buffer);

    পক্ষ TCBSetFlags:
        প্রত্যাগম decodeSetFlags(cap, length, call, buffer);

    স্বভাব:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }
}

গণনা CopyRegistersFlags {
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

    যদি (length < 1 || current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } অন্যথা {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & BIT(CopyRegisters_suspendSource),
               flags & BIT(CopyRegisters_resumeTarget),
               flags & BIT(CopyRegisters_transferFrame),
               flags & BIT(CopyRegisters_transferInteger),
               transferArch);

}

গণনা ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, word_t length, bool_t call,
                                word_t *buffer)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    যদি (length < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    n     = getSyscallArg(1, buffer);

    যদি (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (পূর্ণাঙ্ক)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    যদি (thread == গ্রন্থি_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

গণনা WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    যদি (length < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    w     = getSyscallArg(1, buffer);

    যদি (length - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (পূর্ণাঙ্ক)(length - 2), (পূর্ণাঙ্ক)w);
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    যদি (thread == গ্রন্থি_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, buffer);
}

#ifdef CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_endpoint_cap:
        প্রত্যাগম (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_capCanGrantReply(cap)));
    পক্ষ cap_null_cap:
        /* just has no fault endpoint */
        প্রত্যাগম true;
    স্বভাব:
        প্রত্যাগম false;
    }
}
#endif

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cte_t *bufferSlot, *c_মূল_Slot, *v_মূল_Slot;
    cap_t bufferCap, c_মূল_Cap, v_মূল_Cap;
    deriveCap_ret_t dc_ret;
    word_t c_মূল_Data, v_মূল_Data, bufferAddr;
#ifdef CONFIG_KERNEL_MCS
#define TCBCONFIGURE_ARGS 3
#অন্যথা
#define TCBCONFIGURE_ARGS 4
#endif
    যদি (length < TCBCONFIGURE_ARGS || current_extra_caps.excaprefs[0] == রিক্ত
        || current_extra_caps.excaprefs[1] == রিক্ত
        || current_extra_caps.excaprefs[2] == রিক্ত) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_মূল_Data     = getSyscallArg(0, buffer);
    v_মূল_Data     = getSyscallArg(1, buffer);
    bufferAddr    = getSyscallArg(2, buffer);
#অন্যথা
    cptr_t faultEP       = getSyscallArg(0, buffer);
    c_মূল_Data     = getSyscallArg(1, buffer);
    v_মূল_Data     = getSyscallArg(2, buffer);
    bufferAddr    = getSyscallArg(3, buffer);
#endif

    c_মূল_Slot  = current_extra_caps.excaprefs[0];
    c_মূল_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_মূল_Slot  = current_extra_caps.excaprefs[1];
    v_মূল_Cap   = current_extra_caps.excaprefs[1]->cap;
    bufferSlot = current_extra_caps.excaprefs[2];
    bufferCap  = current_extra_caps.excaprefs[2]->cap;

    যদি (bufferAddr == 0) {
        bufferSlot = রিক্ত;
    } অন্যথা {
        dc_ret = deriveCap(bufferSlot, bufferCap);
        যদি (dc_ret.status != EXCEPTION_NONE) {
            প্রত্যাগম dc_ret.status;
        }
        bufferCap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(bufferAddr, bufferCap);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }
    }

    যদি (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    যদি (c_মূল_Data != 0) {
        c_মূল_Cap = updateCapData(false, c_মূল_Data, c_মূল_Cap);
    }

    dc_ret = deriveCap(c_মূল_Slot, c_মূল_Cap);
    যদি (dc_ret.status != EXCEPTION_NONE) {
        প্রত্যাগম dc_ret.status;
    }
    c_মূল_Cap = dc_ret.cap;

    যদি (cap_get_capType(c_মূল_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    যদি (v_মূল_Data != 0) {
        v_মূল_Cap = updateCapData(false, v_মূল_Data, v_মূল_Cap);
    }

    dc_ret = deriveCap(v_মূল_Slot, v_মূল_Cap);
    যদি (dc_ret.status != EXCEPTION_NONE) {
        প্রত্যাগম dc_ret.status;
    }
    v_মূল_Cap = dc_ret.cap;

    যদি (!is_Valid_VTable_মূল(v_মূল_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    প্রত্যাগম invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               c_মূল_Cap, c_মূল_Slot,
               v_মূল_Cap, v_মূল_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_buffer);
#অন্যথা
    প্রত্যাগম invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_মূল_Cap, c_মূল_Slot,
               v_মূল_Cap, v_মূল_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_update_space |
               thread_control_update_ipc_buffer);
#endif
}

exception_t decodeSetPriority(cap_t cap, word_t length, word_t *buffer)
{
    যদি (length < 1 || current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newPrio, authTCB);
    যদি (status != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        প্রত্যাগম status;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    প্রত্যাগম invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               NULL_PRIO, newPrio,
               রিক্ত, thread_control_sched_update_priority);
#অন্যথা
    প্রত্যাগম invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), রিক্ত,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               0, cap_null_cap_new(),
               রিক্ত, thread_control_update_priority);
#endif
}

exception_t decodeSetMCPriority(cap_t cap, word_t length, word_t *buffer)
{
    যদি (length < 1 || current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    যদি (status != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        প্রত্যাগম status;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    প্রত্যাগম invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               newMcp, NULL_PRIO,
               রিক্ত, thread_control_sched_update_mcp);
#অন্যথা
    প্রত্যাগম invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), রিক্ত,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               0, cap_null_cap_new(),
               রিক্ত, thread_control_update_mcp);
#endif
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetTimeoutEndpoint(cap_t cap, cte_t *slot)
{
    যদি (current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    যদি (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), রিক্ত,
               thCap, thSlot,
               cap_null_cap_new(), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               0, cap_null_cap_new(), রিক্ত,
               thread_control_caps_update_timeout);
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetSchedParams(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
#অন্যথা
exception_t decodeSetSchedParams(cap_t cap, word_t length, word_t *buffer)
#endif
{
    যদি (length < 2 || current_extra_caps.excaprefs[0] == রিক্ত
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == রিক্ত || current_extra_caps.excaprefs[2] == রিক্ত
#endif
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    prio_t newPrio = getSyscallArg(1, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ifdef CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#endif

    যদি (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    যদি (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        প্রত্যাগম status;
    }

    status = checkPrio(newPrio, authTCB);
    যদি (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        প্রত্যাগম status;
    }

#ifdef CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = রিক্ত;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_priority |
                                         thread_control_sched_update_fault;
    বিকল্প (cap_get_capType(scCap)) {
    পক্ষ cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        যদি (tcb->tcbSchedContext != sc) {
            যদি (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
            }
            যদি (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
            }
        }
        যদি (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        ভঙ্গ;
    পক্ষ cap_null_cap:
        যদি (tcb == গ্রন্থি_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        ভঙ্গ;
    স্বভাব:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 2;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    যদি (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    যদি (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 3;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }
#endif
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    প্রত্যাগম invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#অন্যথা
    প্রত্যাগম invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), রিক্ত,
               0, newMcp, newPrio,
               cap_null_cap_new(), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               0, cap_null_cap_new(),
               রিক্ত, thread_control_update_mcp |
               thread_control_update_priority);
#endif
}


exception_t decodeSetIPCBuffer(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cptr_t cptr_bufferPtr;
    cap_t bufferCap;
    cte_t *bufferSlot;

    যদি (length < 1 || current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    cptr_bufferPtr  = getSyscallArg(0, buffer);
    bufferSlot = current_extra_caps.excaprefs[0];
    bufferCap  = current_extra_caps.excaprefs[0]->cap;

    যদি (cptr_bufferPtr == 0) {
        bufferSlot = রিক্ত;
    } অন্যথা {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(bufferSlot, bufferCap);
        যদি (dc_ret.status != EXCEPTION_NONE) {
            প্রত্যাগম dc_ret.status;
        }
        bufferCap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_bufferPtr, bufferCap);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    প্রত্যাগম invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_caps_update_ipc_buffer);
#অন্যথা
    প্রত্যাগম invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), রিক্ত,
               cap_null_cap_new(), রিক্ত,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_update_ipc_buffer);

#endif
}

#ifdef CONFIG_KERNEL_MCS
#define DECODE_SET_SPACE_PARAMS 2
#অন্যথা
#define DECODE_SET_SPACE_PARAMS 3
#endif
exception_t decodeSetSpace(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    word_t c_মূল_Data, v_মূল_Data;
    cte_t *c_মূল_Slot, *v_মূল_Slot;
    cap_t c_মূল_Cap, v_মূল_Cap;
    deriveCap_ret_t dc_ret;

    যদি (length < DECODE_SET_SPACE_PARAMS || current_extra_caps.excaprefs[0] == রিক্ত
        || current_extra_caps.excaprefs[1] == রিক্ত
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == রিক্ত
#endif
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_মূল_Data = getSyscallArg(0, buffer);
    v_মূল_Data = getSyscallArg(1, buffer);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_মূল_Slot  = current_extra_caps.excaprefs[1];
    c_মূল_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_মূল_Slot  = current_extra_caps.excaprefs[2];
    v_মূল_Cap   = current_extra_caps.excaprefs[2]->cap;
#অন্যথা
    cptr_t faultEP   = getSyscallArg(0, buffer);
    c_মূল_Data = getSyscallArg(1, buffer);
    v_মূল_Data = getSyscallArg(2, buffer);

    c_মূল_Slot  = current_extra_caps.excaprefs[0];
    c_মূল_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_মূল_Slot  = current_extra_caps.excaprefs[1];
    v_মূল_Cap   = current_extra_caps.excaprefs[1]->cap;
#endif

    যদি (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    যদি (c_মূল_Data != 0) {
        c_মূল_Cap = updateCapData(false, c_মূল_Data, c_মূল_Cap);
    }

    dc_ret = deriveCap(c_মূল_Slot, c_মূল_Cap);
    যদি (dc_ret.status != EXCEPTION_NONE) {
        প্রত্যাগম dc_ret.status;
    }
    c_মূল_Cap = dc_ret.cap;

    যদি (cap_get_capType(c_মূল_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    যদি (v_মূল_Data != 0) {
        v_মূল_Cap = updateCapData(false, v_মূল_Data, v_মূল_Cap);
    }

    dc_ret = deriveCap(v_মূল_Slot, v_মূল_Cap);
    যদি (dc_ret.status != EXCEPTION_NONE) {
        প্রত্যাগম dc_ret.status;
    }
    v_মূল_Cap = dc_ret.cap;

    যদি (!is_Valid_VTable_মূল(v_মূল_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    /* fault handler */
    যদি (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }
#endif

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    প্রত্যাগম invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), রিক্ত,
               c_মূল_Cap, c_মূল_Slot,
               v_মূল_Cap, v_মূল_Slot,
               0, cap_null_cap_new(), রিক্ত, thread_control_caps_update_space | thread_control_caps_update_fault);
#অন্যথা
    প্রত্যাগম invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_মূল_Cap, c_মূল_Slot,
               v_মূল_Cap, v_মূল_Slot,
               0, cap_null_cap_new(), রিক্ত, thread_control_update_space);
#endif
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    যদি (current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    যদি (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } অন্যথা {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    যদি (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    যদি ((tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }


    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    যদি (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeTCB_NotificationControl(tcb, রিক্ত);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    প্রত্যাগম EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    প্রত্যাগম EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
স্থির অন্তঃস্থ exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_index_t index, cap_t newCap, cte_t *srcSlot)
{
    cte_t *মূল_Slot = TCB_PTR_CTE_PTR(target, index);
    UNUSED exception_t e = cteDelete(মূল_Slot, true);
    যদি (e != EXCEPTION_NONE) {
        প্রত্যাগম e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    যদি (cap_get_capType(newCap) != cap_null_cap) {
        যদি (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ঢোকাও(newCap, srcSlot, মূল_Slot);
        }
    }
    প্রত্যাগম e;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t c_মূল_new_Cap, cte_t *c_মূল_src_Slot,
                                        cap_t v_মূল_new_Cap, cte_t *v_মূল_src_Slot,
                                        word_t bufferAddr, cap_t bufferCap,
                                        cte_t *bufferSrcSlot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    যদি (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }

    }

    যদি (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }
    }

    যদি (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, c_মূল_new_Cap, c_মূল_src_Slot);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, v_মূল_new_Cap, v_মূল_src_Slot);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }
    }

    যদি (updateFlags & thread_control_caps_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }
        target->tcbIPCBuffer = bufferAddr;

        যদি (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ঢোকাও(bufferCap, bufferSrcSlot, bufferSlot);
        }

        যদি (target == গ্রন্থি_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    প্রত্যাগম EXCEPTION_NONE;
}
#অন্যথা
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t priority,
                                    cap_t c_মূল_new_Cap, cte_t *c_মূল_src_Slot,
                                    cap_t v_মূল_new_Cap, cte_t *v_মূল_src_Slot,
                                    word_t bufferAddr, cap_t bufferCap,
                                    cte_t *bufferSrcSlot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    যদি (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    যদি (updateFlags & thread_control_update_mcp) {
        setMCPriority(target, mcp);
    }

    যদি (updateFlags & thread_control_update_space) {
        cte_t *মূল_Slot;

        মূল_Slot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(মূল_Slot, true);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }
        যদি (sameObjectAs(c_মূল_new_Cap, c_মূল_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ঢোকাও(c_মূল_new_Cap, c_মূল_src_Slot, মূল_Slot);
        }
    }

    যদি (updateFlags & thread_control_update_space) {
        cte_t *মূল_Slot;

        মূল_Slot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(মূল_Slot, true);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }
        যদি (sameObjectAs(v_মূল_new_Cap, v_মূল_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ঢোকাও(v_মূল_new_Cap, v_মূল_src_Slot, মূল_Slot);
        }
    }

    যদি (updateFlags & thread_control_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }
        target->tcbIPCBuffer = bufferAddr;

        যদি (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ঢোকাও(bufferCap, bufferSrcSlot, bufferSlot);
        }

        যদি (target == গ্রন্থি_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    যদি (updateFlags & thread_control_update_priority) {
        setPriority(target, priority);
    }

    প্রত্যাগম EXCEPTION_NONE;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t priority,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    যদি (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        যদি (e != EXCEPTION_NONE) {
            প্রত্যাগম e;
        }
    }

    যদি (updateFlags & thread_control_sched_update_mcp) {
        setMCPriority(target, mcp);
    }

    যদি (updateFlags & thread_control_sched_update_priority) {
        setPriority(target, priority);
    }

    যদি (updateFlags & thread_control_sched_update_sc) {
        যদি (sc != রিক্ত) {
            schedContext_bindTCB(sc, target);
        } অন্যথা যদি (sc == রিক্ত) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    প্রত্যাগম EXCEPTION_NONE;
}
#endif

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    যদি (suspendSource) {
        suspend(tcb_src);
    }

    যদি (resumeTarget) {
        restart(dest);
    }

    যদি (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        প্রতি (i = 0; i < n_frameRegisters; i++) {
            v = getRegister(tcb_src, frameRegisters[i]);
            setRegister(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        setNextPC(dest, pc);
    }

    যদি (transferInteger) {
        word_t i;
        word_t v;

        প্রতি (i = 0; i < n_gpRegisters; i++) {
            v = getRegister(tcb_src, gpRegisters[i]);
            setRegister(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    যদি (dest == গ্রন্থি_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    প্রত্যাগম Arch_performTransfer(transferArch, tcb_src, dest);
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

    thread = গ্রন্থি_STATE(ksCurThread);

    যদি (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, গ্রন্থি_STATE(ksCurThread));
    যদি (e != EXCEPTION_NONE) {
        প্রত্যাগম e;
    }

    যদি (call) {
        word_t *ipcBuffer;

        ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);

        প্রতি (i = 0; i < n && i < n_frameRegisters && i < n_বার্তা_Registers; i++) {
            setRegister(thread, বার্তা_Registers[i],
                        getRegister(tcb_src, frameRegisters[i]));
        }

        যদি (ipcBuffer != রিক্ত && i < n && i < n_frameRegisters) {
            প্রতি (; i < n && i < n_frameRegisters; i++) {
                ipcBuffer[i + 1] = getRegister(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        প্রতি (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_বার্তা_Registers; i++) {
            setRegister(thread, বার্তা_Registers[i + n_frameRegisters],
                        getRegister(tcb_src, gpRegisters[i]));
        }

        যদি (ipcBuffer != রিক্ত && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            প্রতি (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipcBuffer[i + n_frameRegisters + 1] =
                    getRegister(tcb_src, gpRegisters[i]);
            }
        }

        setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, i + j)));
    }
    setThreadState(thread, ThreadState_Running);

    প্রত্যাগম EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *buffer)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, গ্রন্থি_STATE(ksCurThread), dest);
    যদি (e != EXCEPTION_NONE) {
        প্রত্যাগম e;
    }

    যদি (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_getSanitiseRegisterInfo(dest);

    প্রতি (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        setRegister(dest, frameRegisters[i],
                    sanitiseRegister(frameRegisters[i],
                                     getSyscallArg(i + 2, buffer), archInfo));
    }

    প্রতি (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        setRegister(dest, gpRegisters[i],
                    sanitiseRegister(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   buffer), archInfo));
    }

    pc = getRestartPC(dest);
    setNextPC(dest, pc);

    Arch_postModifyRegisters(dest);

    যদি (resumeTarget) {
        restart(dest);
    }

    যদি (dest == গ্রন্থি_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    প্রত্যাগম EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    যদি (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } অন্যথা {
        unbindNotification(tcb);
    }

    প্রত্যাগম EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD
শূন্য setThreadName(tcb_t *tcb, ধ্রুব বর্ণ *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_LENGTH);
}
#endif

word_t setMRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    বিকল্প (current_syscall_error.type) {
    পক্ষ seL4_InvalidArgument:
        প্রত্যাগম setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Argument_সংখ্যা);

    পক্ষ seL4_InvalidCapability:
        প্রত্যাগম setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_সংখ্যা);

    পক্ষ seL4_IllegalOperation:
        প্রত্যাগম 0;

    পক্ষ seL4_RangeError:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        প্রত্যাগম setMR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    পক্ষ seL4_AlignmentError:
        প্রত্যাগম 0;

    পক্ষ seL4_FailedLookup:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        প্রত্যাগম setMRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    পক্ষ seL4_TruncatedMessage:
    পক্ষ seL4_DeleteFirst:
    পক্ষ seL4_RevokeFirst:
        প্রত্যাগম 0;
    পক্ষ seL4_NotEnoughMemory:
        প্রত্যাগম setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.memory_বাম);
    স্বভাব:
        fail("Invalid syscall error");
    }
}
