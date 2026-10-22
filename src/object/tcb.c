/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/invocation.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <sel4/shared_types.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <object/objecttype.h>
#अन्तर्भाव <object/cnode.h>
#ifdef CONFIG_KERNEL_MCS
#अन्तर्भाव <object/schedcontext.h>
#endif
#अन्तर्भाव <object/tcb.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <arch/smp/ipi_inline.h>

#define NULL_PRIO 0

स्थिर exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    प्रतिज्ञा(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    सचे (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर अन्तःस्थ शून्य addToBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    ग्रन्थिः_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    ग्रन्थिः_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & MASK(wordRadix));
}

स्थिर अन्तःस्थ शून्य removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    ग्रन्थिः_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & MASK(wordRadix));
    सचे (unlikely(!ग्रन्थिः_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        ग्रन्थिः_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcbSchedNext;

    सचे (queue.head == tcb && queue.end == tcb) {
        queue.head = रिक्त;
        queue.end = रिक्त;
    } अञ्ञथा {
        सचे (queue.head == tcb) {
            after->tcbSchedPrev = रिक्त;
            tcb->tcbSchedNext = रिक्त;
            queue.head = after;
        } अञ्ञथा {
            सचे (queue.end == tcb) {
                before->tcbSchedNext = रिक्त;
                tcb->tcbSchedPrev = रिक्त;
                queue.end = before;
            } अञ्ञथा {
                before->tcbSchedNext = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = रिक्त;
                tcb->tcbSchedNext = रिक्त;
            }
        }
    }

    पटिनिवत्त queue;
}

/* Add TCB to the head of a scheduler queue */
शून्य tcbSchedEnqueue(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    प्रतिज्ञा(isSchedulable(tcb));
    प्रतिज्ञा(refill_sufficient(tcb->tcbSchedContext, 0));
#endif

    सचे (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        सचे (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
शून्य tcbSchedAppend(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    प्रतिज्ञा(isSchedulable(tcb));
    प्रतिज्ञा(refill_sufficient(tcb->tcbSchedContext, 0));
    प्रतिज्ञा(refill_ready(tcb->tcbSchedContext));
#endif
    सचे (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        सचे (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
शून्य tcbSchedDequeue(tcb_t *tcb)
{
    सचे (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, false);

        सचे (likely(tcb_queue_empty(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
शून्य tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = रिक्त;

    debug_tcb->tcbDebugNext = ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    सचे (ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

शून्य tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    प्रतिज्ञा(ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != रिक्त);
    सचे (tcb == ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcbDebugNext;
    } अञ्ञथा {
        प्रतिज्ञा(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcbDebugNext = debug_tcb->tcbDebugNext;
    }

    सचे (debug_tcb->tcbDebugNext) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugNext)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = रिक्त;
    debug_tcb->tcbDebugNext = रिक्त;
}
#endif /* CONFIG_DEBUG_BUILD */

#ifndef CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    सचे (!queue.head) { /* Empty list */
        queue.head = tcb;
    } अञ्ञथा {
        queue.end->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.end;
    tcb->tcbEPNext = रिक्त;
    queue.end = tcb;

    पटिनिवत्त queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    सचे (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } अञ्ञथा {
        queue.head = tcb->tcbEPNext;
    }

    सचे (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } अञ्ञथा {
        queue.end = tcb->tcbEPPrev;
    }

    पटिनिवत्त queue;
}
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS

शून्य tcbReleaseRemove(tcb_t *tcb)
{
    सचे (likely(thread_state_get_tcbInReleaseQueue(tcb->tcbState))) {
        tcb_queue_t queue = ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        सचे (queue.head == tcb) {
            ग्रन्थिः_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, false);
    }
}

स्थिर अन्तःस्थ ticks_t PURE tcbReadyTime(tcb_t *tcb)
{
    पटिनिवत्त refill_head(tcb->tcbSchedContext)->rTime;
}

स्थिर अन्तःस्थ bool_t PURE time_after(tcb_t *tcb, ticks_t new_time)
{
    पटिनिवत्त tcb != रिक्त && new_time >= tcbReadyTime(tcb);
}

स्थिर tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    याव (time_after(after, new_time)) {
        after = after->tcbSchedNext;
    }

    पटिनिवत्त after;
}

शून्य tcbReleaseEnqueue(tcb_t *tcb)
{
    प्रतिज्ञा(thread_state_get_tcbInReleaseQueue(tcb->tcbState) == false);
    प्रतिज्ञा(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    सचे (tcb_queue_empty(queue) || new_time < tcbReadyTime(queue.head)) {
        ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } अञ्ञथा {
        सचे (tcbReadyTime(queue.end) <= new_time) {
            ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } अञ्ञथा {
            tcb_t *after;
            after = find_time_after(queue.head, new_time);
            tcb_queue_निवेशय(tcb, after);
        }
    }

    thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, true);

    सचे (queue.head != ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).head) {
        ग्रन्थिः_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#endif

cptr_t PURE getExtraCPtr(word_t *bufferPtr, word_t i)
{
    पटिनिवत्त (cptr_t)bufferPtr[se_L4_सन्देशः_Max_Length + 2 + i];
}

शून्य setExtraBadge(word_t *bufferPtr, word_t badge,
                   word_t i)
{
    bufferPtr[se_L4_सन्देशः_Max_Length + 2 + i] = badge;
}

#ifndef CONFIG_KERNEL_MCS
शून्य setupCallerCap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *replySlot, *callerSlot;
    cap_t masterCap UNUSED, callerCap UNUSED;

    setThreadState(sender, ThreadState_BlockedOnReply);
    replySlot = TCB_PTR_CTE_PTR(sender, tcbReply);
    masterCap = replySlot->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    प्रतिज्ञा(cap_get_capType(masterCap) == cap_reply_cap);
    प्रतिज्ञा(cap_reply_cap_get_capReplyMaster(masterCap));
    प्रतिज्ञा(cap_reply_cap_get_capReplyCanGrant(masterCap));
    प्रतिज्ञा(TCB_PTR(cap_reply_cap_get_capTCBPtr(masterCap)) == sender);
    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = callerSlot->cap;
    /* Haskell error: "Caller cap must not already exist" */
    प्रतिज्ञा(cap_get_capType(callerCap) == cap_null_cap);
    cte_निवेशय(cap_reply_cap_new(canGrant, false, TCB_REF(sender)),
              replySlot, callerSlot);
}

शून्य deleteCallerCap(tcb_t *receiver)
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

    सचे (!bufferPtr) {
        current_extra_caps.excaprefs[0] = रिक्त;
        पटिनिवत्त EXCEPTION_NONE;
    }

    length = seL4_MessageInfo_get_extraCaps(info);

    अत्थाय (i = 0; i < length; i++) {
        cptr = getExtraCPtr(bufferPtr, i);

        lu_ret = lookupSlot(thread, cptr);
        सचे (lu_ret.status != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            पटिनिवत्त lu_ret.status;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    सचे (i < se_L4_सन्देशः_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = रिक्त;
    }

    पटिनिवत्त EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    अत्थाय (i = 0; i < n && i < n_सन्देशः_Registers; i++) {
        setRegister(receiver, सन्देशः_Registers[i],
                    getRegister(sender, सन्देशः_Registers[i]));
    }

    सचे (!recvBuf || !sendBuf) {
        पटिनिवत्त i;
    }

    /* Copy out-of-line words */
    अत्थाय (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    पटिनिवत्त i;
}

#ifdef ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
शून्य remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    सचे (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = ग्रन्थिः_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        सचे (targetCurThread == ग्रन्थिः_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcbPriority > targetCurThread->tcbPriority
#ifdef CONFIG_KERNEL_MCS
            || ग्रन्थिः_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#endif
           ) {
            ARCH_ग्रन्थिः_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
शून्य remoteTCBStall(tcb_t *tcb)
{

    सचे (
#ifdef CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#endif
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        ग्रन्थिः_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_ग्रन्थिः_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#ifndef CONFIG_KERNEL_MCS
स्थिर exception_t invokeTCB_SetAffinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcbSchedDequeue(thread);
    migrateTCB(thread, affinity);
    सचे (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    सचे (thread == ग्रन्थिः_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर exception_t decodeSetAffinity(cap_t cap, word_t length, word_t *buffer)
{
    tcb_t *tcb;
    word_t affinity;

    सचे (length < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, buffer);
    सचे (affinity >= ks_अङ्कः_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeTCB_SetAffinity(tcb, affinity);
}
#endif
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_HARDWARE_DEBUG_API
स्थिर exception_t invokeConfigureSingleStepping(bool_t call, word_t *buffer, tcb_t *t,
                                                 uint16_t bp_अङ्कः, word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    word_t मानम्;

    bp_was_consumed = configureSingleStepping(t, bp_अङ्कः, n_instrs, false);
    सचे (n_instrs == 0) {
#ifndef CONFIG_ARCH_AARCH64
        unsetBreakpointUsedFlag(t, bp_अङ्कः);
#endif /* CONFIG_ARCH_AARCH64 */
        मानम् = false;
    } अञ्ञथा {
#ifndef CONFIG_ARCH_AARCH64
        setBreakpointUsedFlag(t, bp_अङ्कः);
#endif /* CONFIG_ARCH_AARCH64 */
        मानम् = bp_was_consumed;
    }

    सचे (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned पूर्णाङ्क length = setMR(thread, buffer, 0, मानम्);
        setRegister(thread, सन्देशः_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *buffer)
{
    uint16_t bp_अङ्कः;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_अङ्कः = getSyscallArg(0, buffer);
    n_instrs = getSyscallArg(1, buffer);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_अङ्कः, n_instrs, false);
    सचे (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeConfigureSingleStepping(call, buffer, tcb, bp_अङ्कः, n_instrs);
}

स्थिर exception_t invokeSetBreakpoint(tcb_t *tcb, uint16_t bp_अङ्कः,
                                       word_t vaddr, word_t type, word_t size, word_t rw)
{
    setBreakpoint(tcb, bp_अङ्कः, vaddr, type, size, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    setBreakpointUsedFlag(tcb, bp_अङ्कः);
    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर exception_t decodeSetBreakpoint(cap_t cap, word_t *buffer)
{
    uint16_t bp_अङ्कः;
    word_t vaddr, type, size, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_अङ्कः = getSyscallArg(0, buffer);
    vaddr = getSyscallArg(1, buffer);
    type = getSyscallArg(2, buffer);
    size = getSyscallArg(3, buffer);
    rw = getSyscallArg(4, buffer);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    सचे (vaddr >= (word_t)USER_शिखरम्) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे (type != seL4_InstructionBreakpoint && type != seL4_DataBreakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 2;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    } अञ्ञथा सचे (type == seL4_InstructionBreakpoint) {
        सचे (size != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_सङ्ख्या = 3;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        सचे (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_सङ्ख्या = 4;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        सचे ((seL4_FirstWatchpoint == -1 || bp_अङ्कः >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_सङ्ख्या = 2;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
    } अञ्ञथा सचे (type == seL4_DataBreakpoint) {
        सचे (size == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_सङ्ख्या = 3;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        सचे (seL4_FirstWatchpoint != -1 && bp_अङ्कः < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_सङ्ख्या = 2;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
    } अञ्ञथा सचे (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 2;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 3;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (size != 0 && size != 1 && size != 2 && size != 4 && size != 8) {
        userError("Debug: Invalid size %lu.", size);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 3;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (size > 0 && vaddr & (size - 1)) {
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
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decodeSetBreakpoint(tcb, bp_अङ्कः, vaddr, type, size, rw);
    सचे (error.type != seL4_NoError) {
        current_syscall_error = error;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeSetBreakpoint(tcb, bp_अङ्कः,
                               vaddr, type, size, rw);
}

स्थिर exception_t invokeGetBreakpoint(bool_t call, word_t *buffer, tcb_t *tcb, uint16_t bp_अङ्कः)
{
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_अङ्कः);
    सचे (call) {
        setRegister(thread, badgeRegister, 0);
        setMR(ग्रन्थिः_STATE(ksCurThread), buffer, 0, res.vaddr);
        setMR(ग्रन्थिः_STATE(ksCurThread), buffer, 1, res.type);
        setMR(ग्रन्थिः_STATE(ksCurThread), buffer, 2, res.size);
        setMR(ग्रन्थिः_STATE(ksCurThread), buffer, 3, res.rw);
        setMR(ग्रन्थिः_STATE(ksCurThread), buffer, 4, res.is_enabled);
        setRegister(thread, सन्देशः_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, 5)));
    }
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_अङ्कः;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_अङ्कः = getSyscallArg(0, buffer);

    error = Arch_decodeGetBreakpoint(tcb, bp_अङ्कः);
    सचे (error.type != seL4_NoError) {
        current_syscall_error = error;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeGetBreakpoint(call, buffer, tcb, bp_अङ्कः);
}

स्थिर exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_अङ्कः)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_अङ्कः);
    unsetBreakpointUsedFlag(tcb, bp_अङ्कः);
    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर exception_t decodeUnsetBreakpoint(cap_t cap, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_अङ्कः;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_अङ्कः = getSyscallArg(0, buffer);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_अङ्कः);
    सचे (error.type != seL4_NoError) {
        current_syscall_error = error;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeUnsetBreakpoint(tcb, bp_अङ्कः);
}
#endif /* CONFIG_HARDWARE_DEBUG_API */

स्थिर exception_t invokeSetTLSBase(tcb_t *thread, word_t tls_base)
{
    setRegister(thread, TLS_BASE, tls_base);
    सचे (thread == ग्रन्थिः_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर exception_t decodeSetTLSBase(cap_t cap, word_t length, word_t *buffer)
{
    word_t tls_base;

    सचे (length < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    tls_base = getSyscallArg(0, buffer);

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeSetTLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_base);
}

स्थिर शून्य invokeSetFlags(tcb_t *thread, word_t clear, word_t set, bool_t call)
{
    tcb_t *cur_thread = ग्रन्थिः_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= set & seL4_TCBFlag_MASK;
    thread->tcbFlags = flags;

#ifdef CONFIG_HAVE_FPU
    सचे (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } अञ्ञथा सचे (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#endif
    सचे (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, cur_thread);
        setRegister(cur_thread, badgeRegister, 0);
        unsigned पूर्णाङ्क length = setMR(cur_thread, ipcBuffer, 0, flags);
        setRegister(cur_thread, सन्देशः_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(cur_thread, ThreadState_Running);
}

स्थिर exception_t decodeSetFlags(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    सचे (length < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, buffer);
    word_t set   = getSyscallArg(1, buffer);

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    invokeSetFlags(thread, clear, set, call);
    पटिनिवत्त EXCEPTION_NONE;
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

    विकल्प (invLabel) {
    पक्ष TCBReadRegisters:
        /* Second level of decoding */
        पटिनिवत्त decodeReadRegisters(cap, length, call, buffer);

    पक्ष TCBWriteRegisters:
        पटिनिवत्त decodeWriteRegisters(cap, length, buffer);

    पक्ष TCBCopyRegisters:
        पटिनिवत्त decodeCopyRegisters(cap, length, buffer);

    पक्ष TCBSuspend:
        /* Jump straight to the invoke */
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        पटिनिवत्त invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    पक्ष TCBResume:
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        पटिनिवत्त invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    पक्ष TCBConfigure:
        पटिनिवत्त decodeTCBConfigure(cap, length, slot, buffer);

    पक्ष TCBSetPriority:
        पटिनिवत्त decodeSetPriority(cap, length, buffer);

    पक्ष TCBSetMCPriority:
        पटिनिवत्त decodeSetMCPriority(cap, length, buffer);

    पक्ष TCBSetSchedParams:
#ifdef CONFIG_KERNEL_MCS
        पटिनिवत्त decodeSetSchedParams(cap, length, slot, buffer);
#अञ्ञथा
        पटिनिवत्त decodeSetSchedParams(cap, length, buffer);
#endif

    पक्ष TCBSetIPCBuffer:
        पटिनिवत्त decodeSetIPCBuffer(cap, length, slot, buffer);

    पक्ष TCBSetSpace:
        पटिनिवत्त decodeSetSpace(cap, length, slot, buffer);

    पक्ष TCBBindNotification:
        पटिनिवत्त decodeBindNotification(cap);

    पक्ष TCBUnbindNotification:
        पटिनिवत्त decodeUnbindNotification(cap);

#ifdef CONFIG_KERNEL_MCS
    पक्ष TCBSetTimeoutEndpoint:
        पटिनिवत्त decodeSetTimeoutEndpoint(cap, slot);
#अञ्ञथा
#ifdef ENABLE_SMP_SUPPORT
    पक्ष TCBSetAffinity:
        पटिनिवत्त decodeSetAffinity(cap, length, buffer);
#endif /* ENABLE_SMP_SUPPORT */
#endif

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ifdef CONFIG_VTX
    पक्ष TCBSetEPTRoot:
        पटिनिवत्त decodeSetEPTRoot(cap);
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    पक्ष TCBConfigureSingleStepping:
        पटिनिवत्त decodeConfigureSingleStepping(cap, call, buffer);

    पक्ष TCBSetBreakpoint:
        पटिनिवत्त decodeSetBreakpoint(cap, buffer);

    पक्ष TCBGetBreakpoint:
        पटिनिवत्त decodeGetBreakpoint(cap, call, buffer);

    पक्ष TCBUnsetBreakpoint:
        पटिनिवत्त decodeUnsetBreakpoint(cap, buffer);
#endif

    पक्ष TCBSetTLSBase:
        पटिनिवत्त decodeSetTLSBase(cap, length, buffer);

    पक्ष TCBSetFlags:
        पटिनिवत्त decodeSetFlags(cap, length, call, buffer);

    स्वभाव:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
}

गणना CopyRegistersFlags {
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

    सचे (length < 1 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    सचे (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } अञ्ञथा {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & BIT(CopyRegisters_suspendSource),
               flags & BIT(CopyRegisters_resumeTarget),
               flags & BIT(CopyRegisters_transferFrame),
               flags & BIT(CopyRegisters_transferInteger),
               transferArch);

}

गणना ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, word_t length, bool_t call,
                                word_t *buffer)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    सचे (length < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    n     = getSyscallArg(1, buffer);

    सचे (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (पूर्णाङ्क)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    सचे (thread == ग्रन्थिः_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

गणना WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    सचे (length < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    w     = getSyscallArg(1, buffer);

    सचे (length - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (पूर्णाङ्क)(length - 2), (पूर्णाङ्क)w);
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    सचे (thread == ग्रन्थिः_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, buffer);
}

#ifdef CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_endpoint_cap:
        पटिनिवत्त (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_capCanGrantReply(cap)));
    पक्ष cap_null_cap:
        /* just has no fault endpoint */
        पटिनिवत्त true;
    स्वभाव:
        पटिनिवत्त false;
    }
}
#endif

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cte_t *bufferSlot, *c_मूलम्_Slot, *v_मूलम्_Slot;
    cap_t bufferCap, c_मूलम्_Cap, v_मूलम्_Cap;
    deriveCap_ret_t dc_ret;
    word_t c_मूलम्_Data, v_मूलम्_Data, bufferAddr;
#ifdef CONFIG_KERNEL_MCS
#define TCBCONFIGURE_ARGS 3
#अञ्ञथा
#define TCBCONFIGURE_ARGS 4
#endif
    सचे (length < TCBCONFIGURE_ARGS || current_extra_caps.excaprefs[0] == रिक्त
        || current_extra_caps.excaprefs[1] == रिक्त
        || current_extra_caps.excaprefs[2] == रिक्त) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_मूलम्_Data     = getSyscallArg(0, buffer);
    v_मूलम्_Data     = getSyscallArg(1, buffer);
    bufferAddr    = getSyscallArg(2, buffer);
#अञ्ञथा
    cptr_t faultEP       = getSyscallArg(0, buffer);
    c_मूलम्_Data     = getSyscallArg(1, buffer);
    v_मूलम्_Data     = getSyscallArg(2, buffer);
    bufferAddr    = getSyscallArg(3, buffer);
#endif

    c_मूलम्_Slot  = current_extra_caps.excaprefs[0];
    c_मूलम्_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_मूलम्_Slot  = current_extra_caps.excaprefs[1];
    v_मूलम्_Cap   = current_extra_caps.excaprefs[1]->cap;
    bufferSlot = current_extra_caps.excaprefs[2];
    bufferCap  = current_extra_caps.excaprefs[2]->cap;

    सचे (bufferAddr == 0) {
        bufferSlot = रिक्त;
    } अञ्ञथा {
        dc_ret = deriveCap(bufferSlot, bufferCap);
        सचे (dc_ret.status != EXCEPTION_NONE) {
            पटिनिवत्त dc_ret.status;
        }
        bufferCap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(bufferAddr, bufferCap);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }
    }

    सचे (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे (c_मूलम्_Data != 0) {
        c_मूलम्_Cap = updateCapData(false, c_मूलम्_Data, c_मूलम्_Cap);
    }

    dc_ret = deriveCap(c_मूलम्_Slot, c_मूलम्_Cap);
    सचे (dc_ret.status != EXCEPTION_NONE) {
        पटिनिवत्त dc_ret.status;
    }
    c_मूलम्_Cap = dc_ret.cap;

    सचे (cap_get_capType(c_मूलम्_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे (v_मूलम्_Data != 0) {
        v_मूलम्_Cap = updateCapData(false, v_मूलम्_Data, v_मूलम्_Cap);
    }

    dc_ret = deriveCap(v_मूलम्_Slot, v_मूलम्_Cap);
    सचे (dc_ret.status != EXCEPTION_NONE) {
        पटिनिवत्त dc_ret.status;
    }
    v_मूलम्_Cap = dc_ret.cap;

    सचे (!is_Valid_VTable_मूलम्(v_मूलम्_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    पटिनिवत्त invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               c_मूलम्_Cap, c_मूलम्_Slot,
               v_मूलम्_Cap, v_मूलम्_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_buffer);
#अञ्ञथा
    पटिनिवत्त invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_मूलम्_Cap, c_मूलम्_Slot,
               v_मूलम्_Cap, v_मूलम्_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_update_space |
               thread_control_update_ipc_buffer);
#endif
}

exception_t decodeSetPriority(cap_t cap, word_t length, word_t *buffer)
{
    सचे (length < 1 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    सचे (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newPrio, authTCB);
    सचे (status != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        पटिनिवत्त status;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    पटिनिवत्त invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), रिक्त,
               cap_null_cap_new(), रिक्त,
               NULL_PRIO, newPrio,
               रिक्त, thread_control_sched_update_priority);
#अञ्ञथा
    पटिनिवत्त invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), रिक्त,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               0, cap_null_cap_new(),
               रिक्त, thread_control_update_priority);
#endif
}

exception_t decodeSetMCPriority(cap_t cap, word_t length, word_t *buffer)
{
    सचे (length < 1 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    सचे (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    सचे (status != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        पटिनिवत्त status;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    पटिनिवत्त invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), रिक्त,
               cap_null_cap_new(), रिक्त,
               newMcp, NULL_PRIO,
               रिक्त, thread_control_sched_update_mcp);
#अञ्ञथा
    पटिनिवत्त invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), रिक्त,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               0, cap_null_cap_new(),
               रिक्त, thread_control_update_mcp);
#endif
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetTimeoutEndpoint(cap_t cap, cte_t *slot)
{
    सचे (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    सचे (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), रिक्त,
               thCap, thSlot,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               0, cap_null_cap_new(), रिक्त,
               thread_control_caps_update_timeout);
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetSchedParams(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
#अञ्ञथा
exception_t decodeSetSchedParams(cap_t cap, word_t length, word_t *buffer)
#endif
{
    सचे (length < 2 || current_extra_caps.excaprefs[0] == रिक्त
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == रिक्त || current_extra_caps.excaprefs[2] == रिक्त
#endif
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    prio_t newPrio = getSyscallArg(1, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ifdef CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#endif

    सचे (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    सचे (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        पटिनिवत्त status;
    }

    status = checkPrio(newPrio, authTCB);
    सचे (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        पटिनिवत्त status;
    }

#ifdef CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = रिक्त;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_priority |
                                         thread_control_sched_update_fault;
    विकल्प (cap_get_capType(scCap)) {
    पक्ष cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        सचे (tcb->tcbSchedContext != sc) {
            सचे (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
            }
            सचे (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
            }
        }
        सचे (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        भङ्ग;
    पक्ष cap_null_cap:
        सचे (tcb == ग्रन्थिः_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        भङ्ग;
    स्वभाव:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 2;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    सचे (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    सचे (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 3;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
#endif
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    पटिनिवत्त invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#अञ्ञथा
    पटिनिवत्त invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), रिक्त,
               0, newMcp, newPrio,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               0, cap_null_cap_new(),
               रिक्त, thread_control_update_mcp |
               thread_control_update_priority);
#endif
}


exception_t decodeSetIPCBuffer(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cptr_t cptr_bufferPtr;
    cap_t bufferCap;
    cte_t *bufferSlot;

    सचे (length < 1 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    cptr_bufferPtr  = getSyscallArg(0, buffer);
    bufferSlot = current_extra_caps.excaprefs[0];
    bufferCap  = current_extra_caps.excaprefs[0]->cap;

    सचे (cptr_bufferPtr == 0) {
        bufferSlot = रिक्त;
    } अञ्ञथा {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(bufferSlot, bufferCap);
        सचे (dc_ret.status != EXCEPTION_NONE) {
            पटिनिवत्त dc_ret.status;
        }
        bufferCap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_bufferPtr, bufferCap);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    पटिनिवत्त invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_caps_update_ipc_buffer);
#अञ्ञथा
    पटिनिवत्त invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_update_ipc_buffer);

#endif
}

#ifdef CONFIG_KERNEL_MCS
#define DECODE_SET_SPACE_PARAMS 2
#अञ्ञथा
#define DECODE_SET_SPACE_PARAMS 3
#endif
exception_t decodeSetSpace(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    word_t c_मूलम्_Data, v_मूलम्_Data;
    cte_t *c_मूलम्_Slot, *v_मूलम्_Slot;
    cap_t c_मूलम्_Cap, v_मूलम्_Cap;
    deriveCap_ret_t dc_ret;

    सचे (length < DECODE_SET_SPACE_PARAMS || current_extra_caps.excaprefs[0] == रिक्त
        || current_extra_caps.excaprefs[1] == रिक्त
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == रिक्त
#endif
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_मूलम्_Data = getSyscallArg(0, buffer);
    v_मूलम्_Data = getSyscallArg(1, buffer);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_मूलम्_Slot  = current_extra_caps.excaprefs[1];
    c_मूलम्_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_मूलम्_Slot  = current_extra_caps.excaprefs[2];
    v_मूलम्_Cap   = current_extra_caps.excaprefs[2]->cap;
#अञ्ञथा
    cptr_t faultEP   = getSyscallArg(0, buffer);
    c_मूलम्_Data = getSyscallArg(1, buffer);
    v_मूलम्_Data = getSyscallArg(2, buffer);

    c_मूलम्_Slot  = current_extra_caps.excaprefs[0];
    c_मूलम्_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_मूलम्_Slot  = current_extra_caps.excaprefs[1];
    v_मूलम्_Cap   = current_extra_caps.excaprefs[1]->cap;
#endif

    सचे (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे (c_मूलम्_Data != 0) {
        c_मूलम्_Cap = updateCapData(false, c_मूलम्_Data, c_मूलम्_Cap);
    }

    dc_ret = deriveCap(c_मूलम्_Slot, c_मूलम्_Cap);
    सचे (dc_ret.status != EXCEPTION_NONE) {
        पटिनिवत्त dc_ret.status;
    }
    c_मूलम्_Cap = dc_ret.cap;

    सचे (cap_get_capType(c_मूलम्_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे (v_मूलम्_Data != 0) {
        v_मूलम्_Cap = updateCapData(false, v_मूलम्_Data, v_मूलम्_Cap);
    }

    dc_ret = deriveCap(v_मूलम्_Slot, v_मूलम्_Cap);
    सचे (dc_ret.status != EXCEPTION_NONE) {
        पटिनिवत्त dc_ret.status;
    }
    v_मूलम्_Cap = dc_ret.cap;

    सचे (!is_Valid_VTable_मूलम्(v_मूलम्_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    /* fault handler */
    सचे (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
#endif

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    पटिनिवत्त invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), रिक्त,
               c_मूलम्_Cap, c_मूलम्_Slot,
               v_मूलम्_Cap, v_मूलम्_Slot,
               0, cap_null_cap_new(), रिक्त, thread_control_caps_update_space | thread_control_caps_update_fault);
#अञ्ञथा
    पटिनिवत्त invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_मूलम्_Cap, c_मूलम्_Slot,
               v_मूलम्_Cap, v_मूलम्_Slot,
               0, cap_null_cap_new(), रिक्त, thread_control_update_space);
#endif
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    सचे (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    सचे (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    सचे (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } अञ्ञथा {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे ((tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }


    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    सचे (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeTCB_NotificationControl(tcb, रिक्त);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    पटिनिवत्त EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    पटिनिवत्त EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_index_t index, cap_t newCap, cte_t *srcSlot)
{
    cte_t *मूलम्_Slot = TCB_PTR_CTE_PTR(target, index);
    UNUSED exception_t e = cteDelete(मूलम्_Slot, true);
    सचे (e != EXCEPTION_NONE) {
        पटिनिवत्त e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    सचे (cap_get_capType(newCap) != cap_null_cap) {
        सचे (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_निवेशय(newCap, srcSlot, मूलम्_Slot);
        }
    }
    पटिनिवत्त e;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t c_मूलम्_new_Cap, cte_t *c_मूलम्_src_Slot,
                                        cap_t v_मूलम्_new_Cap, cte_t *v_मूलम्_src_Slot,
                                        word_t bufferAddr, cap_t bufferCap,
                                        cte_t *bufferSrcSlot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    सचे (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }

    }

    सचे (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }
    }

    सचे (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, c_मूलम्_new_Cap, c_मूलम्_src_Slot);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, v_मूलम्_new_Cap, v_मूलम्_src_Slot);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }
    }

    सचे (updateFlags & thread_control_caps_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }
        target->tcbIPCBuffer = bufferAddr;

        सचे (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_निवेशय(bufferCap, bufferSrcSlot, bufferSlot);
        }

        सचे (target == ग्रन्थिः_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    पटिनिवत्त EXCEPTION_NONE;
}
#अञ्ञथा
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t priority,
                                    cap_t c_मूलम्_new_Cap, cte_t *c_मूलम्_src_Slot,
                                    cap_t v_मूलम्_new_Cap, cte_t *v_मूलम्_src_Slot,
                                    word_t bufferAddr, cap_t bufferCap,
                                    cte_t *bufferSrcSlot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    सचे (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    सचे (updateFlags & thread_control_update_mcp) {
        setMCPriority(target, mcp);
    }

    सचे (updateFlags & thread_control_update_space) {
        cte_t *मूलम्_Slot;

        मूलम्_Slot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(मूलम्_Slot, true);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }
        सचे (sameObjectAs(c_मूलम्_new_Cap, c_मूलम्_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_निवेशय(c_मूलम्_new_Cap, c_मूलम्_src_Slot, मूलम्_Slot);
        }
    }

    सचे (updateFlags & thread_control_update_space) {
        cte_t *मूलम्_Slot;

        मूलम्_Slot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(मूलम्_Slot, true);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }
        सचे (sameObjectAs(v_मूलम्_new_Cap, v_मूलम्_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_निवेशय(v_मूलम्_new_Cap, v_मूलम्_src_Slot, मूलम्_Slot);
        }
    }

    सचे (updateFlags & thread_control_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }
        target->tcbIPCBuffer = bufferAddr;

        सचे (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_निवेशय(bufferCap, bufferSrcSlot, bufferSlot);
        }

        सचे (target == ग्रन्थिः_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    सचे (updateFlags & thread_control_update_priority) {
        setPriority(target, priority);
    }

    पटिनिवत्त EXCEPTION_NONE;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t priority,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    सचे (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        सचे (e != EXCEPTION_NONE) {
            पटिनिवत्त e;
        }
    }

    सचे (updateFlags & thread_control_sched_update_mcp) {
        setMCPriority(target, mcp);
    }

    सचे (updateFlags & thread_control_sched_update_priority) {
        setPriority(target, priority);
    }

    सचे (updateFlags & thread_control_sched_update_sc) {
        सचे (sc != रिक्त) {
            schedContext_bindTCB(sc, target);
        } अञ्ञथा सचे (sc == रिक्त) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    पटिनिवत्त EXCEPTION_NONE;
}
#endif

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    सचे (suspendSource) {
        suspend(tcb_src);
    }

    सचे (resumeTarget) {
        restart(dest);
    }

    सचे (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        अत्थाय (i = 0; i < n_frameRegisters; i++) {
            v = getRegister(tcb_src, frameRegisters[i]);
            setRegister(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        setNextPC(dest, pc);
    }

    सचे (transferInteger) {
        word_t i;
        word_t v;

        अत्थाय (i = 0; i < n_gpRegisters; i++) {
            v = getRegister(tcb_src, gpRegisters[i]);
            setRegister(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    सचे (dest == ग्रन्थिः_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    पटिनिवत्त Arch_performTransfer(transferArch, tcb_src, dest);
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

    thread = ग्रन्थिः_STATE(ksCurThread);

    सचे (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, ग्रन्थिः_STATE(ksCurThread));
    सचे (e != EXCEPTION_NONE) {
        पटिनिवत्त e;
    }

    सचे (call) {
        word_t *ipcBuffer;

        ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);

        अत्थाय (i = 0; i < n && i < n_frameRegisters && i < n_सन्देशः_Registers; i++) {
            setRegister(thread, सन्देशः_Registers[i],
                        getRegister(tcb_src, frameRegisters[i]));
        }

        सचे (ipcBuffer != रिक्त && i < n && i < n_frameRegisters) {
            अत्थाय (; i < n && i < n_frameRegisters; i++) {
                ipcBuffer[i + 1] = getRegister(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        अत्थाय (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_सन्देशः_Registers; i++) {
            setRegister(thread, सन्देशः_Registers[i + n_frameRegisters],
                        getRegister(tcb_src, gpRegisters[i]));
        }

        सचे (ipcBuffer != रिक्त && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            अत्थाय (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipcBuffer[i + n_frameRegisters + 1] =
                    getRegister(tcb_src, gpRegisters[i]);
            }
        }

        setRegister(thread, सन्देशः_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, i + j)));
    }
    setThreadState(thread, ThreadState_Running);

    पटिनिवत्त EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *buffer)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, ग्रन्थिः_STATE(ksCurThread), dest);
    सचे (e != EXCEPTION_NONE) {
        पटिनिवत्त e;
    }

    सचे (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_getSanitiseRegisterInfo(dest);

    अत्थाय (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        setRegister(dest, frameRegisters[i],
                    sanitiseRegister(frameRegisters[i],
                                     getSyscallArg(i + 2, buffer), archInfo));
    }

    अत्थाय (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        setRegister(dest, gpRegisters[i],
                    sanitiseRegister(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   buffer), archInfo));
    }

    pc = getRestartPC(dest);
    setNextPC(dest, pc);

    Arch_postModifyRegisters(dest);

    सचे (resumeTarget) {
        restart(dest);
    }

    सचे (dest == ग्रन्थिः_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    पटिनिवत्त EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    सचे (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } अञ्ञथा {
        unbindNotification(tcb);
    }

    पटिनिवत्त EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD
शून्य setThreadName(tcb_t *tcb, ध्रुव वर्ण *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_LENGTH);
}
#endif

word_t setMRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    विकल्प (current_syscall_error.type) {
    पक्ष seL4_InvalidArgument:
        पटिनिवत्त setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Argument_सङ्ख्या);

    पक्ष seL4_InvalidCapability:
        पटिनिवत्त setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_सङ्ख्या);

    पक्ष seL4_IllegalOperation:
        पटिनिवत्त 0;

    पक्ष seL4_RangeError:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        पटिनिवत्त setMR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    पक्ष seL4_AlignmentError:
        पटिनिवत्त 0;

    पक्ष seL4_FailedLookup:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        पटिनिवत्त setMRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    पक्ष seL4_TruncatedMessage:
    पक्ष seL4_DeleteFirst:
    पक्ष seL4_RevokeFirst:
        पटिनिवत्त 0;
    पक्ष seL4_NotEnoughMemory:
        पटिनिवत्त setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.memory_वामः);
    स्वभाव:
        fail("Invalid syscall error");
    }
}
