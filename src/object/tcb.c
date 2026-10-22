/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <api/invocation.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <sel4/shared_types.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <object/objecttype.h>
#அந்தர்பாவ <object/cnode.h>
#ifdef CONFIG_KERNEL_MCS
#அந்தர்பாவ <object/schedcontext.h>
#endif
#அந்தர்பாவ <object/tcb.h>
#அந்தர்பாவ <kernel/cspace.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <kernel/vspace.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <arch/smp/ipi_inline.h>

#define NULL_PRIO 0

நிலையான exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    ப்ரதிஜ்ஞா(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    எனில் (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    திரும்பு EXCEPTION_NONE;
}

நிலையான உள்வரி வெறுமை addToBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    முனை_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    முனை_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & MASK(wordRadix));
}

நிலையான உள்வரி வெறுமை removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    முனை_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & MASK(wordRadix));
    எனில் (unlikely(!முனை_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        முனை_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcbSchedNext;

    எனில் (queue.head == tcb && queue.end == tcb) {
        queue.head = வெற்று;
        queue.end = வெற்று;
    } இல்லையெனில் {
        எனில் (queue.head == tcb) {
            after->tcbSchedPrev = வெற்று;
            tcb->tcbSchedNext = வெற்று;
            queue.head = after;
        } இல்லையெனில் {
            எனில் (queue.end == tcb) {
                before->tcbSchedNext = வெற்று;
                tcb->tcbSchedPrev = வெற்று;
                queue.end = before;
            } இல்லையெனில் {
                before->tcbSchedNext = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = வெற்று;
                tcb->tcbSchedNext = வெற்று;
            }
        }
    }

    திரும்பு queue;
}

/* Add TCB to the head of a scheduler queue */
வெறுமை tcbSchedEnqueue(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    ப்ரதிஜ்ஞா(isSchedulable(tcb));
    ப்ரதிஜ்ஞா(refill_sufficient(tcb->tcbSchedContext, 0));
#endif

    எனில் (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = முனை_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        எனில் (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        முனை_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
வெறுமை tcbSchedAppend(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    ப்ரதிஜ்ஞா(isSchedulable(tcb));
    ப்ரதிஜ்ஞா(refill_sufficient(tcb->tcbSchedContext, 0));
    ப்ரதிஜ்ஞா(refill_ready(tcb->tcbSchedContext));
#endif
    எனில் (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = முனை_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        எனில் (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        முனை_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
வெறுமை tcbSchedDequeue(tcb_t *tcb)
{
    எனில் (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = முனை_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        முனை_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, false);

        எனில் (likely(tcb_queue_empty(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
வெறுமை tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = வெற்று;

    debug_tcb->tcbDebugNext = முனை_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    எனில் (முனை_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(முனை_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    முனை_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

வெறுமை tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    ப்ரதிஜ்ஞா(முனை_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != வெற்று);
    எனில் (tcb == முனை_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        முனை_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(முனை_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcbDebugNext;
    } இல்லையெனில் {
        ப்ரதிஜ்ஞா(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcbDebugNext = debug_tcb->tcbDebugNext;
    }

    எனில் (debug_tcb->tcbDebugNext) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugNext)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = வெற்று;
    debug_tcb->tcbDebugNext = வெற்று;
}
#endif /* CONFIG_DEBUG_BUILD */

#ifndef CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    எனில் (!queue.head) { /* Empty list */
        queue.head = tcb;
    } இல்லையெனில் {
        queue.end->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.end;
    tcb->tcbEPNext = வெற்று;
    queue.end = tcb;

    திரும்பு queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    எனில் (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } இல்லையெனில் {
        queue.head = tcb->tcbEPNext;
    }

    எனில் (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } இல்லையெனில் {
        queue.end = tcb->tcbEPPrev;
    }

    திரும்பு queue;
}
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS

வெறுமை tcbReleaseRemove(tcb_t *tcb)
{
    எனில் (likely(thread_state_get_tcbInReleaseQueue(tcb->tcbState))) {
        tcb_queue_t queue = முனை_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        எனில் (queue.head == tcb) {
            முனை_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        முனை_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, false);
    }
}

நிலையான உள்வரி ticks_t PURE tcbReadyTime(tcb_t *tcb)
{
    திரும்பு refill_head(tcb->tcbSchedContext)->rTime;
}

நிலையான உள்வரி bool_t PURE time_after(tcb_t *tcb, ticks_t new_time)
{
    திரும்பு tcb != வெற்று && new_time >= tcbReadyTime(tcb);
}

நிலையான tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    போது (time_after(after, new_time)) {
        after = after->tcbSchedNext;
    }

    திரும்பு after;
}

வெறுமை tcbReleaseEnqueue(tcb_t *tcb)
{
    ப்ரதிஜ்ஞா(thread_state_get_tcbInReleaseQueue(tcb->tcbState) == false);
    ப்ரதிஜ்ஞா(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = முனை_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    எனில் (tcb_queue_empty(queue) || new_time < tcbReadyTime(queue.head)) {
        முனை_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } இல்லையெனில் {
        எனில் (tcbReadyTime(queue.end) <= new_time) {
            முனை_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } இல்லையெனில் {
            tcb_t *after;
            after = find_time_after(queue.head, new_time);
            tcb_queue_செருகு(tcb, after);
        }
    }

    thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, true);

    எனில் (queue.head != முனை_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).head) {
        முனை_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#endif

cptr_t PURE getExtraCPtr(word_t *bufferPtr, word_t i)
{
    திரும்பு (cptr_t)bufferPtr[se_L4_செய்தி_Max_Length + 2 + i];
}

வெறுமை setExtraBadge(word_t *bufferPtr, word_t badge,
                   word_t i)
{
    bufferPtr[se_L4_செய்தி_Max_Length + 2 + i] = badge;
}

#ifndef CONFIG_KERNEL_MCS
வெறுமை setupCallerCap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *replySlot, *callerSlot;
    cap_t masterCap UNUSED, callerCap UNUSED;

    setThreadState(sender, ThreadState_BlockedOnReply);
    replySlot = TCB_PTR_CTE_PTR(sender, tcbReply);
    masterCap = replySlot->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    ப்ரதிஜ்ஞா(cap_get_capType(masterCap) == cap_reply_cap);
    ப்ரதிஜ்ஞா(cap_reply_cap_get_capReplyMaster(masterCap));
    ப்ரதிஜ்ஞா(cap_reply_cap_get_capReplyCanGrant(masterCap));
    ப்ரதிஜ்ஞா(TCB_PTR(cap_reply_cap_get_capTCBPtr(masterCap)) == sender);
    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = callerSlot->cap;
    /* Haskell error: "Caller cap must not already exist" */
    ப்ரதிஜ்ஞா(cap_get_capType(callerCap) == cap_null_cap);
    cte_செருகு(cap_reply_cap_new(canGrant, false, TCB_REF(sender)),
              replySlot, callerSlot);
}

வெறுமை deleteCallerCap(tcb_t *receiver)
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

    எனில் (!bufferPtr) {
        current_extra_caps.excaprefs[0] = வெற்று;
        திரும்பு EXCEPTION_NONE;
    }

    length = seL4_MessageInfo_get_extraCaps(info);

    க்கு (i = 0; i < length; i++) {
        cptr = getExtraCPtr(bufferPtr, i);

        lu_ret = lookupSlot(thread, cptr);
        எனில் (lu_ret.status != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            திரும்பு lu_ret.status;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    எனில் (i < se_L4_செய்தி_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = வெற்று;
    }

    திரும்பு EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    க்கு (i = 0; i < n && i < n_செய்தி_Registers; i++) {
        setRegister(receiver, செய்தி_Registers[i],
                    getRegister(sender, செய்தி_Registers[i]));
    }

    எனில் (!recvBuf || !sendBuf) {
        திரும்பு i;
    }

    /* Copy out-of-line words */
    க்கு (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    திரும்பு i;
}

#ifdef ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
வெறுமை remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    எனில் (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = முனை_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        எனில் (targetCurThread == முனை_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcbPriority > targetCurThread->tcbPriority
#ifdef CONFIG_KERNEL_MCS
            || முனை_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#endif
           ) {
            ARCH_முனை_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
வெறுமை remoteTCBStall(tcb_t *tcb)
{

    எனில் (
#ifdef CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#endif
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        முனை_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_முனை_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#ifndef CONFIG_KERNEL_MCS
நிலையான exception_t invokeTCB_SetAffinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcbSchedDequeue(thread);
    migrateTCB(thread, affinity);
    எனில் (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    எனில் (thread == முனை_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeSetAffinity(cap_t cap, word_t length, word_t *buffer)
{
    tcb_t *tcb;
    word_t affinity;

    எனில் (length < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, buffer);
    எனில் (affinity >= ks_இலக்கம்_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeTCB_SetAffinity(tcb, affinity);
}
#endif
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_HARDWARE_DEBUG_API
நிலையான exception_t invokeConfigureSingleStepping(bool_t call, word_t *buffer, tcb_t *t,
                                                 uint16_t bp_இலக்கம், word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = முனை_STATE(ksCurThread);
    word_t மதிப்பு;

    bp_was_consumed = configureSingleStepping(t, bp_இலக்கம், n_instrs, false);
    எனில் (n_instrs == 0) {
#ifndef CONFIG_ARCH_AARCH64
        unsetBreakpointUsedFlag(t, bp_இலக்கம்);
#endif /* CONFIG_ARCH_AARCH64 */
        மதிப்பு = false;
    } இல்லையெனில் {
#ifndef CONFIG_ARCH_AARCH64
        setBreakpointUsedFlag(t, bp_இலக்கம்);
#endif /* CONFIG_ARCH_AARCH64 */
        மதிப்பு = bp_was_consumed;
    }

    எனில் (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned முழு length = setMR(thread, buffer, 0, மதிப்பு);
        setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *buffer)
{
    uint16_t bp_இலக்கம்;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_இலக்கம் = getSyscallArg(0, buffer);
    n_instrs = getSyscallArg(1, buffer);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_இலக்கம், n_instrs, false);
    எனில் (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeConfigureSingleStepping(call, buffer, tcb, bp_இலக்கம், n_instrs);
}

நிலையான exception_t invokeSetBreakpoint(tcb_t *tcb, uint16_t bp_இலக்கம்,
                                       word_t vaddr, word_t type, word_t size, word_t rw)
{
    setBreakpoint(tcb, bp_இலக்கம், vaddr, type, size, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    setBreakpointUsedFlag(tcb, bp_இலக்கம்);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeSetBreakpoint(cap_t cap, word_t *buffer)
{
    uint16_t bp_இலக்கம்;
    word_t vaddr, type, size, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_இலக்கம் = getSyscallArg(0, buffer);
    vaddr = getSyscallArg(1, buffer);
    type = getSyscallArg(2, buffer);
    size = getSyscallArg(3, buffer);
    rw = getSyscallArg(4, buffer);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    எனில் (vaddr >= (word_t)USER_உச்சி) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (type != seL4_InstructionBreakpoint && type != seL4_DataBreakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 2;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    } இல்லையெனில் எனில் (type == seL4_InstructionBreakpoint) {
        எனில் (size != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_எண் = 3;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        எனில் (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_எண் = 4;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        எனில் ((seL4_FirstWatchpoint == -1 || bp_இலக்கம் >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_எண் = 2;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
    } இல்லையெனில் எனில் (type == seL4_DataBreakpoint) {
        எனில் (size == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_எண் = 3;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        எனில் (seL4_FirstWatchpoint != -1 && bp_இலக்கம் < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_எண் = 2;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
    } இல்லையெனில் எனில் (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 2;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 3;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (size != 0 && size != 1 && size != 2 && size != 4 && size != 8) {
        userError("Debug: Invalid size %lu.", size);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 3;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (size > 0 && vaddr & (size - 1)) {
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
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decodeSetBreakpoint(tcb, bp_இலக்கம், vaddr, type, size, rw);
    எனில் (error.type != seL4_NoError) {
        current_syscall_error = error;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeSetBreakpoint(tcb, bp_இலக்கம்,
                               vaddr, type, size, rw);
}

நிலையான exception_t invokeGetBreakpoint(bool_t call, word_t *buffer, tcb_t *tcb, uint16_t bp_இலக்கம்)
{
    tcb_t *thread;
    thread = முனை_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_இலக்கம்);
    எனில் (call) {
        setRegister(thread, badgeRegister, 0);
        setMR(முனை_STATE(ksCurThread), buffer, 0, res.vaddr);
        setMR(முனை_STATE(ksCurThread), buffer, 1, res.type);
        setMR(முனை_STATE(ksCurThread), buffer, 2, res.size);
        setMR(முனை_STATE(ksCurThread), buffer, 3, res.rw);
        setMR(முனை_STATE(ksCurThread), buffer, 4, res.is_enabled);
        setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, 5)));
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_இலக்கம்;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_இலக்கம் = getSyscallArg(0, buffer);

    error = Arch_decodeGetBreakpoint(tcb, bp_இலக்கம்);
    எனில் (error.type != seL4_NoError) {
        current_syscall_error = error;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeGetBreakpoint(call, buffer, tcb, bp_இலக்கம்);
}

நிலையான exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_இலக்கம்)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_இலக்கம்);
    unsetBreakpointUsedFlag(tcb, bp_இலக்கம்);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeUnsetBreakpoint(cap_t cap, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_இலக்கம்;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_இலக்கம் = getSyscallArg(0, buffer);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_இலக்கம்);
    எனில் (error.type != seL4_NoError) {
        current_syscall_error = error;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeUnsetBreakpoint(tcb, bp_இலக்கம்);
}
#endif /* CONFIG_HARDWARE_DEBUG_API */

நிலையான exception_t invokeSetTLSBase(tcb_t *thread, word_t tls_base)
{
    setRegister(thread, TLS_BASE, tls_base);
    எனில் (thread == முனை_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeSetTLSBase(cap_t cap, word_t length, word_t *buffer)
{
    word_t tls_base;

    எனில் (length < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    tls_base = getSyscallArg(0, buffer);

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeSetTLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_base);
}

நிலையான வெறுமை invokeSetFlags(tcb_t *thread, word_t clear, word_t set, bool_t call)
{
    tcb_t *cur_thread = முனை_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= set & seL4_TCBFlag_MASK;
    thread->tcbFlags = flags;

#ifdef CONFIG_HAVE_FPU
    எனில் (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } இல்லையெனில் எனில் (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#endif
    எனில் (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, cur_thread);
        setRegister(cur_thread, badgeRegister, 0);
        unsigned முழு length = setMR(cur_thread, ipcBuffer, 0, flags);
        setRegister(cur_thread, செய்தி_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(cur_thread, ThreadState_Running);
}

நிலையான exception_t decodeSetFlags(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    எனில் (length < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, buffer);
    word_t set   = getSyscallArg(1, buffer);

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    invokeSetFlags(thread, clear, set, call);
    திரும்பு EXCEPTION_NONE;
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

    தேர்வு (invLabel) {
    நிலை TCBReadRegisters:
        /* Second level of decoding */
        திரும்பு decodeReadRegisters(cap, length, call, buffer);

    நிலை TCBWriteRegisters:
        திரும்பு decodeWriteRegisters(cap, length, buffer);

    நிலை TCBCopyRegisters:
        திரும்பு decodeCopyRegisters(cap, length, buffer);

    நிலை TCBSuspend:
        /* Jump straight to the invoke */
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    நிலை TCBResume:
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    நிலை TCBConfigure:
        திரும்பு decodeTCBConfigure(cap, length, slot, buffer);

    நிலை TCBSetPriority:
        திரும்பு decodeSetPriority(cap, length, buffer);

    நிலை TCBSetMCPriority:
        திரும்பு decodeSetMCPriority(cap, length, buffer);

    நிலை TCBSetSchedParams:
#ifdef CONFIG_KERNEL_MCS
        திரும்பு decodeSetSchedParams(cap, length, slot, buffer);
#இல்லையெனில்
        திரும்பு decodeSetSchedParams(cap, length, buffer);
#endif

    நிலை TCBSetIPCBuffer:
        திரும்பு decodeSetIPCBuffer(cap, length, slot, buffer);

    நிலை TCBSetSpace:
        திரும்பு decodeSetSpace(cap, length, slot, buffer);

    நிலை TCBBindNotification:
        திரும்பு decodeBindNotification(cap);

    நிலை TCBUnbindNotification:
        திரும்பு decodeUnbindNotification(cap);

#ifdef CONFIG_KERNEL_MCS
    நிலை TCBSetTimeoutEndpoint:
        திரும்பு decodeSetTimeoutEndpoint(cap, slot);
#இல்லையெனில்
#ifdef ENABLE_SMP_SUPPORT
    நிலை TCBSetAffinity:
        திரும்பு decodeSetAffinity(cap, length, buffer);
#endif /* ENABLE_SMP_SUPPORT */
#endif

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ifdef CONFIG_VTX
    நிலை TCBSetEPTRoot:
        திரும்பு decodeSetEPTRoot(cap);
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    நிலை TCBConfigureSingleStepping:
        திரும்பு decodeConfigureSingleStepping(cap, call, buffer);

    நிலை TCBSetBreakpoint:
        திரும்பு decodeSetBreakpoint(cap, buffer);

    நிலை TCBGetBreakpoint:
        திரும்பு decodeGetBreakpoint(cap, call, buffer);

    நிலை TCBUnsetBreakpoint:
        திரும்பு decodeUnsetBreakpoint(cap, buffer);
#endif

    நிலை TCBSetTLSBase:
        திரும்பு decodeSetTLSBase(cap, length, buffer);

    நிலை TCBSetFlags:
        திரும்பு decodeSetFlags(cap, length, call, buffer);

    இயல்பு:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
}

எண்ணிக்கை CopyRegistersFlags {
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

    எனில் (length < 1 || current_extra_caps.excaprefs[0] == வெற்று) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    எனில் (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } இல்லையெனில் {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & BIT(CopyRegisters_suspendSource),
               flags & BIT(CopyRegisters_resumeTarget),
               flags & BIT(CopyRegisters_transferFrame),
               flags & BIT(CopyRegisters_transferInteger),
               transferArch);

}

எண்ணிக்கை ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, word_t length, bool_t call,
                                word_t *buffer)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    எனில் (length < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    n     = getSyscallArg(1, buffer);

    எனில் (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (முழு)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    எனில் (thread == முனை_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

எண்ணிக்கை WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    எனில் (length < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    w     = getSyscallArg(1, buffer);

    எனில் (length - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (முழு)(length - 2), (முழு)w);
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    எனில் (thread == முனை_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, buffer);
}

#ifdef CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_endpoint_cap:
        திரும்பு (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_capCanGrantReply(cap)));
    நிலை cap_null_cap:
        /* just has no fault endpoint */
        திரும்பு true;
    இயல்பு:
        திரும்பு false;
    }
}
#endif

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cte_t *bufferSlot, *c_வேர்_Slot, *v_வேர்_Slot;
    cap_t bufferCap, c_வேர்_Cap, v_வேர்_Cap;
    deriveCap_ret_t dc_ret;
    word_t c_வேர்_Data, v_வேர்_Data, bufferAddr;
#ifdef CONFIG_KERNEL_MCS
#define TCBCONFIGURE_ARGS 3
#இல்லையெனில்
#define TCBCONFIGURE_ARGS 4
#endif
    எனில் (length < TCBCONFIGURE_ARGS || current_extra_caps.excaprefs[0] == வெற்று
        || current_extra_caps.excaprefs[1] == வெற்று
        || current_extra_caps.excaprefs[2] == வெற்று) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_வேர்_Data     = getSyscallArg(0, buffer);
    v_வேர்_Data     = getSyscallArg(1, buffer);
    bufferAddr    = getSyscallArg(2, buffer);
#இல்லையெனில்
    cptr_t faultEP       = getSyscallArg(0, buffer);
    c_வேர்_Data     = getSyscallArg(1, buffer);
    v_வேர்_Data     = getSyscallArg(2, buffer);
    bufferAddr    = getSyscallArg(3, buffer);
#endif

    c_வேர்_Slot  = current_extra_caps.excaprefs[0];
    c_வேர்_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_வேர்_Slot  = current_extra_caps.excaprefs[1];
    v_வேர்_Cap   = current_extra_caps.excaprefs[1]->cap;
    bufferSlot = current_extra_caps.excaprefs[2];
    bufferCap  = current_extra_caps.excaprefs[2]->cap;

    எனில் (bufferAddr == 0) {
        bufferSlot = வெற்று;
    } இல்லையெனில் {
        dc_ret = deriveCap(bufferSlot, bufferCap);
        எனில் (dc_ret.status != EXCEPTION_NONE) {
            திரும்பு dc_ret.status;
        }
        bufferCap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(bufferAddr, bufferCap);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }
    }

    எனில் (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (c_வேர்_Data != 0) {
        c_வேர்_Cap = updateCapData(false, c_வேர்_Data, c_வேர்_Cap);
    }

    dc_ret = deriveCap(c_வேர்_Slot, c_வேர்_Cap);
    எனில் (dc_ret.status != EXCEPTION_NONE) {
        திரும்பு dc_ret.status;
    }
    c_வேர்_Cap = dc_ret.cap;

    எனில் (cap_get_capType(c_வேர்_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (v_வேர்_Data != 0) {
        v_வேர்_Cap = updateCapData(false, v_வேர்_Data, v_வேர்_Cap);
    }

    dc_ret = deriveCap(v_வேர்_Slot, v_வேர்_Cap);
    எனில் (dc_ret.status != EXCEPTION_NONE) {
        திரும்பு dc_ret.status;
    }
    v_வேர்_Cap = dc_ret.cap;

    எனில் (!is_Valid_VTable_வேர்(v_வேர்_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    திரும்பு invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), வெற்று,
               cap_null_cap_new(), வெற்று,
               c_வேர்_Cap, c_வேர்_Slot,
               v_வேர்_Cap, v_வேர்_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_buffer);
#இல்லையெனில்
    திரும்பு invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_வேர்_Cap, c_வேர்_Slot,
               v_வேர்_Cap, v_வேர்_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_update_space |
               thread_control_update_ipc_buffer);
#endif
}

exception_t decodeSetPriority(cap_t cap, word_t length, word_t *buffer)
{
    எனில் (length < 1 || current_extra_caps.excaprefs[0] == வெற்று) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    எனில் (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newPrio, authTCB);
    எனில் (status != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        திரும்பு status;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    திரும்பு invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), வெற்று,
               cap_null_cap_new(), வெற்று,
               NULL_PRIO, newPrio,
               வெற்று, thread_control_sched_update_priority);
#இல்லையெனில்
    திரும்பு invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), வெற்று,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), வெற்று,
               cap_null_cap_new(), வெற்று,
               0, cap_null_cap_new(),
               வெற்று, thread_control_update_priority);
#endif
}

exception_t decodeSetMCPriority(cap_t cap, word_t length, word_t *buffer)
{
    எனில் (length < 1 || current_extra_caps.excaprefs[0] == வெற்று) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    எனில் (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    எனில் (status != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        திரும்பு status;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    திரும்பு invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), வெற்று,
               cap_null_cap_new(), வெற்று,
               newMcp, NULL_PRIO,
               வெற்று, thread_control_sched_update_mcp);
#இல்லையெனில்
    திரும்பு invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), வெற்று,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), வெற்று,
               cap_null_cap_new(), வெற்று,
               0, cap_null_cap_new(),
               வெற்று, thread_control_update_mcp);
#endif
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetTimeoutEndpoint(cap_t cap, cte_t *slot)
{
    எனில் (current_extra_caps.excaprefs[0] == வெற்று) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    எனில் (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), வெற்று,
               thCap, thSlot,
               cap_null_cap_new(), வெற்று,
               cap_null_cap_new(), வெற்று,
               0, cap_null_cap_new(), வெற்று,
               thread_control_caps_update_timeout);
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetSchedParams(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
#இல்லையெனில்
exception_t decodeSetSchedParams(cap_t cap, word_t length, word_t *buffer)
#endif
{
    எனில் (length < 2 || current_extra_caps.excaprefs[0] == வெற்று
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == வெற்று || current_extra_caps.excaprefs[2] == வெற்று
#endif
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    prio_t newPrio = getSyscallArg(1, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ifdef CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#endif

    எனில் (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    எனில் (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        திரும்பு status;
    }

    status = checkPrio(newPrio, authTCB);
    எனில் (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        திரும்பு status;
    }

#ifdef CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = வெற்று;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_priority |
                                         thread_control_sched_update_fault;
    தேர்வு (cap_get_capType(scCap)) {
    நிலை cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        எனில் (tcb->tcbSchedContext != sc) {
            எனில் (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }
            எனில் (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }
        }
        எனில் (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        நிறுத்து;
    நிலை cap_null_cap:
        எனில் (tcb == முனை_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        நிறுத்து;
    இயல்பு:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 2;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    எனில் (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    எனில் (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 3;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
#endif
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    திரும்பு invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#இல்லையெனில்
    திரும்பு invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), வெற்று,
               0, newMcp, newPrio,
               cap_null_cap_new(), வெற்று,
               cap_null_cap_new(), வெற்று,
               0, cap_null_cap_new(),
               வெற்று, thread_control_update_mcp |
               thread_control_update_priority);
#endif
}


exception_t decodeSetIPCBuffer(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cptr_t cptr_bufferPtr;
    cap_t bufferCap;
    cte_t *bufferSlot;

    எனில் (length < 1 || current_extra_caps.excaprefs[0] == வெற்று) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    cptr_bufferPtr  = getSyscallArg(0, buffer);
    bufferSlot = current_extra_caps.excaprefs[0];
    bufferCap  = current_extra_caps.excaprefs[0]->cap;

    எனில் (cptr_bufferPtr == 0) {
        bufferSlot = வெற்று;
    } இல்லையெனில் {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(bufferSlot, bufferCap);
        எனில் (dc_ret.status != EXCEPTION_NONE) {
            திரும்பு dc_ret.status;
        }
        bufferCap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_bufferPtr, bufferCap);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    திரும்பு invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), வெற்று,
               cap_null_cap_new(), வெற்று,
               cap_null_cap_new(), வெற்று,
               cap_null_cap_new(), வெற்று,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_caps_update_ipc_buffer);
#இல்லையெனில்
    திரும்பு invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), வெற்று,
               cap_null_cap_new(), வெற்று,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_update_ipc_buffer);

#endif
}

#ifdef CONFIG_KERNEL_MCS
#define DECODE_SET_SPACE_PARAMS 2
#இல்லையெனில்
#define DECODE_SET_SPACE_PARAMS 3
#endif
exception_t decodeSetSpace(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    word_t c_வேர்_Data, v_வேர்_Data;
    cte_t *c_வேர்_Slot, *v_வேர்_Slot;
    cap_t c_வேர்_Cap, v_வேர்_Cap;
    deriveCap_ret_t dc_ret;

    எனில் (length < DECODE_SET_SPACE_PARAMS || current_extra_caps.excaprefs[0] == வெற்று
        || current_extra_caps.excaprefs[1] == வெற்று
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == வெற்று
#endif
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_வேர்_Data = getSyscallArg(0, buffer);
    v_வேர்_Data = getSyscallArg(1, buffer);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_வேர்_Slot  = current_extra_caps.excaprefs[1];
    c_வேர்_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_வேர்_Slot  = current_extra_caps.excaprefs[2];
    v_வேர்_Cap   = current_extra_caps.excaprefs[2]->cap;
#இல்லையெனில்
    cptr_t faultEP   = getSyscallArg(0, buffer);
    c_வேர்_Data = getSyscallArg(1, buffer);
    v_வேர்_Data = getSyscallArg(2, buffer);

    c_வேர்_Slot  = current_extra_caps.excaprefs[0];
    c_வேர்_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_வேர்_Slot  = current_extra_caps.excaprefs[1];
    v_வேர்_Cap   = current_extra_caps.excaprefs[1]->cap;
#endif

    எனில் (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (c_வேர்_Data != 0) {
        c_வேர்_Cap = updateCapData(false, c_வேர்_Data, c_வேர்_Cap);
    }

    dc_ret = deriveCap(c_வேர்_Slot, c_வேர்_Cap);
    எனில் (dc_ret.status != EXCEPTION_NONE) {
        திரும்பு dc_ret.status;
    }
    c_வேர்_Cap = dc_ret.cap;

    எனில் (cap_get_capType(c_வேர்_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (v_வேர்_Data != 0) {
        v_வேர்_Cap = updateCapData(false, v_வேர்_Data, v_வேர்_Cap);
    }

    dc_ret = deriveCap(v_வேர்_Slot, v_வேர்_Cap);
    எனில் (dc_ret.status != EXCEPTION_NONE) {
        திரும்பு dc_ret.status;
    }
    v_வேர்_Cap = dc_ret.cap;

    எனில் (!is_Valid_VTable_வேர்(v_வேர்_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    /* fault handler */
    எனில் (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
#endif

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    திரும்பு invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), வெற்று,
               c_வேர்_Cap, c_வேர்_Slot,
               v_வேர்_Cap, v_வேர்_Slot,
               0, cap_null_cap_new(), வெற்று, thread_control_caps_update_space | thread_control_caps_update_fault);
#இல்லையெனில்
    திரும்பு invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_வேர்_Cap, c_வேர்_Slot,
               v_வேர்_Cap, v_வேர்_Slot,
               0, cap_null_cap_new(), வெற்று, thread_control_update_space);
#endif
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    எனில் (current_extra_caps.excaprefs[0] == வெற்று) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    எனில் (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    எனில் (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } இல்லையெனில் {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் ((tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }


    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    எனில் (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeTCB_NotificationControl(tcb, வெற்று);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    திரும்பு EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    திரும்பு EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
நிலையான உள்வரி exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_index_t index, cap_t newCap, cte_t *srcSlot)
{
    cte_t *வேர்_Slot = TCB_PTR_CTE_PTR(target, index);
    UNUSED exception_t e = cteDelete(வேர்_Slot, true);
    எனில் (e != EXCEPTION_NONE) {
        திரும்பு e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    எனில் (cap_get_capType(newCap) != cap_null_cap) {
        எனில் (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_செருகு(newCap, srcSlot, வேர்_Slot);
        }
    }
    திரும்பு e;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t c_வேர்_new_Cap, cte_t *c_வேர்_src_Slot,
                                        cap_t v_வேர்_new_Cap, cte_t *v_வேர்_src_Slot,
                                        word_t bufferAddr, cap_t bufferCap,
                                        cte_t *bufferSrcSlot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    எனில் (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }

    }

    எனில் (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }
    }

    எனில் (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, c_வேர்_new_Cap, c_வேர்_src_Slot);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, v_வேர்_new_Cap, v_வேர்_src_Slot);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }
    }

    எனில் (updateFlags & thread_control_caps_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }
        target->tcbIPCBuffer = bufferAddr;

        எனில் (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_செருகு(bufferCap, bufferSrcSlot, bufferSlot);
        }

        எனில் (target == முனை_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    திரும்பு EXCEPTION_NONE;
}
#இல்லையெனில்
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t priority,
                                    cap_t c_வேர்_new_Cap, cte_t *c_வேர்_src_Slot,
                                    cap_t v_வேர்_new_Cap, cte_t *v_வேர்_src_Slot,
                                    word_t bufferAddr, cap_t bufferCap,
                                    cte_t *bufferSrcSlot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    எனில் (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    எனில் (updateFlags & thread_control_update_mcp) {
        setMCPriority(target, mcp);
    }

    எனில் (updateFlags & thread_control_update_space) {
        cte_t *வேர்_Slot;

        வேர்_Slot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(வேர்_Slot, true);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }
        எனில் (sameObjectAs(c_வேர்_new_Cap, c_வேர்_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_செருகு(c_வேர்_new_Cap, c_வேர்_src_Slot, வேர்_Slot);
        }
    }

    எனில் (updateFlags & thread_control_update_space) {
        cte_t *வேர்_Slot;

        வேர்_Slot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(வேர்_Slot, true);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }
        எனில் (sameObjectAs(v_வேர்_new_Cap, v_வேர்_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_செருகு(v_வேர்_new_Cap, v_வேர்_src_Slot, வேர்_Slot);
        }
    }

    எனில் (updateFlags & thread_control_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }
        target->tcbIPCBuffer = bufferAddr;

        எனில் (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_செருகு(bufferCap, bufferSrcSlot, bufferSlot);
        }

        எனில் (target == முனை_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    எனில் (updateFlags & thread_control_update_priority) {
        setPriority(target, priority);
    }

    திரும்பு EXCEPTION_NONE;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t priority,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    எனில் (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        எனில் (e != EXCEPTION_NONE) {
            திரும்பு e;
        }
    }

    எனில் (updateFlags & thread_control_sched_update_mcp) {
        setMCPriority(target, mcp);
    }

    எனில் (updateFlags & thread_control_sched_update_priority) {
        setPriority(target, priority);
    }

    எனில் (updateFlags & thread_control_sched_update_sc) {
        எனில் (sc != வெற்று) {
            schedContext_bindTCB(sc, target);
        } இல்லையெனில் எனில் (sc == வெற்று) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    திரும்பு EXCEPTION_NONE;
}
#endif

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    எனில் (suspendSource) {
        suspend(tcb_src);
    }

    எனில் (resumeTarget) {
        restart(dest);
    }

    எனில் (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        க்கு (i = 0; i < n_frameRegisters; i++) {
            v = getRegister(tcb_src, frameRegisters[i]);
            setRegister(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        setNextPC(dest, pc);
    }

    எனில் (transferInteger) {
        word_t i;
        word_t v;

        க்கு (i = 0; i < n_gpRegisters; i++) {
            v = getRegister(tcb_src, gpRegisters[i]);
            setRegister(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    எனில் (dest == முனை_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    திரும்பு Arch_performTransfer(transferArch, tcb_src, dest);
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

    thread = முனை_STATE(ksCurThread);

    எனில் (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, முனை_STATE(ksCurThread));
    எனில் (e != EXCEPTION_NONE) {
        திரும்பு e;
    }

    எனில் (call) {
        word_t *ipcBuffer;

        ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);

        க்கு (i = 0; i < n && i < n_frameRegisters && i < n_செய்தி_Registers; i++) {
            setRegister(thread, செய்தி_Registers[i],
                        getRegister(tcb_src, frameRegisters[i]));
        }

        எனில் (ipcBuffer != வெற்று && i < n && i < n_frameRegisters) {
            க்கு (; i < n && i < n_frameRegisters; i++) {
                ipcBuffer[i + 1] = getRegister(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        க்கு (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_செய்தி_Registers; i++) {
            setRegister(thread, செய்தி_Registers[i + n_frameRegisters],
                        getRegister(tcb_src, gpRegisters[i]));
        }

        எனில் (ipcBuffer != வெற்று && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            க்கு (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipcBuffer[i + n_frameRegisters + 1] =
                    getRegister(tcb_src, gpRegisters[i]);
            }
        }

        setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, i + j)));
    }
    setThreadState(thread, ThreadState_Running);

    திரும்பு EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *buffer)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, முனை_STATE(ksCurThread), dest);
    எனில் (e != EXCEPTION_NONE) {
        திரும்பு e;
    }

    எனில் (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_getSanitiseRegisterInfo(dest);

    க்கு (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        setRegister(dest, frameRegisters[i],
                    sanitiseRegister(frameRegisters[i],
                                     getSyscallArg(i + 2, buffer), archInfo));
    }

    க்கு (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        setRegister(dest, gpRegisters[i],
                    sanitiseRegister(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   buffer), archInfo));
    }

    pc = getRestartPC(dest);
    setNextPC(dest, pc);

    Arch_postModifyRegisters(dest);

    எனில் (resumeTarget) {
        restart(dest);
    }

    எனில் (dest == முனை_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    திரும்பு EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    எனில் (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } இல்லையெனில் {
        unbindNotification(tcb);
    }

    திரும்பு EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD
வெறுமை setThreadName(tcb_t *tcb, மாறிலி எழுத்து *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_LENGTH);
}
#endif

word_t setMRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    தேர்வு (current_syscall_error.type) {
    நிலை seL4_InvalidArgument:
        திரும்பு setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Argument_எண்);

    நிலை seL4_InvalidCapability:
        திரும்பு setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_எண்);

    நிலை seL4_IllegalOperation:
        திரும்பு 0;

    நிலை seL4_RangeError:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        திரும்பு setMR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    நிலை seL4_AlignmentError:
        திரும்பு 0;

    நிலை seL4_FailedLookup:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        திரும்பு setMRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    நிலை seL4_TruncatedMessage:
    நிலை seL4_DeleteFirst:
    நிலை seL4_RevokeFirst:
        திரும்பு 0;
    நிலை seL4_NotEnoughMemory:
        திரும்பு setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.memory_இடது);
    இயல்பு:
        fail("Invalid syscall error");
    }
}
