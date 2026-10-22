/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <api/invocation.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <sel4/shared_types.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <object/objecttype.h>
#અન્તર્ભાવ <object/cnode.h>
#ifdef CONFIG_KERNEL_MCS
#અન્તર્ભાવ <object/schedcontext.h>
#endif
#અન્તર્ભાવ <object/tcb.h>
#અન્તર્ભાવ <kernel/cspace.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <arch/smp/ipi_inline.h>

#define NULL_PRIO 0

સ્થિર exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    પ્રતિજ્ઞા(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    જો (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર અંતઃરેખ શૂન્ય addToBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    ગાંઠ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    ગાંઠ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & MASK(wordRadix));
}

સ્થિર અંતઃરેખ શૂન્ય removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    ગાંઠ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & MASK(wordRadix));
    જો (unlikely(!ગાંઠ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        ગાંઠ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcbSchedNext;

    જો (queue.head == tcb && queue.end == tcb) {
        queue.head = ખાલી;
        queue.end = ખાલી;
    } નહીંતર {
        જો (queue.head == tcb) {
            after->tcbSchedPrev = ખાલી;
            tcb->tcbSchedNext = ખાલી;
            queue.head = after;
        } નહીંતર {
            જો (queue.end == tcb) {
                before->tcbSchedNext = ખાલી;
                tcb->tcbSchedPrev = ખાલી;
                queue.end = before;
            } નહીંતર {
                before->tcbSchedNext = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = ખાલી;
                tcb->tcbSchedNext = ખાલી;
            }
        }
    }

    પાછા queue;
}

/* Add TCB to the head of a scheduler queue */
શૂન્ય tcbSchedEnqueue(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    પ્રતિજ્ઞા(isSchedulable(tcb));
    પ્રતિજ્ઞા(refill_sufficient(tcb->tcbSchedContext, 0));
#endif

    જો (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ગાંઠ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        જો (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ગાંઠ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
શૂન્ય tcbSchedAppend(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    પ્રતિજ્ઞા(isSchedulable(tcb));
    પ્રતિજ્ઞા(refill_sufficient(tcb->tcbSchedContext, 0));
    પ્રતિજ્ઞા(refill_ready(tcb->tcbSchedContext));
#endif
    જો (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ગાંઠ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        જો (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ગાંઠ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
શૂન્ય tcbSchedDequeue(tcb_t *tcb)
{
    જો (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = ગાંઠ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        ગાંઠ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, false);

        જો (likely(tcb_queue_empty(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
શૂન્ય tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = ખાલી;

    debug_tcb->tcbDebugNext = ગાંઠ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    જો (ગાંઠ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(ગાંઠ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    ગાંઠ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

શૂન્ય tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    પ્રતિજ્ઞા(ગાંઠ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != ખાલી);
    જો (tcb == ગાંઠ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        ગાંઠ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(ગાંઠ_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcbDebugNext;
    } નહીંતર {
        પ્રતિજ્ઞા(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcbDebugNext = debug_tcb->tcbDebugNext;
    }

    જો (debug_tcb->tcbDebugNext) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugNext)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = ખાલી;
    debug_tcb->tcbDebugNext = ખાલી;
}
#endif /* CONFIG_DEBUG_BUILD */

#ifndef CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    જો (!queue.head) { /* Empty list */
        queue.head = tcb;
    } નહીંતર {
        queue.end->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.end;
    tcb->tcbEPNext = ખાલી;
    queue.end = tcb;

    પાછા queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    જો (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } નહીંતર {
        queue.head = tcb->tcbEPNext;
    }

    જો (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } નહીંતર {
        queue.end = tcb->tcbEPPrev;
    }

    પાછા queue;
}
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS

શૂન્ય tcbReleaseRemove(tcb_t *tcb)
{
    જો (likely(thread_state_get_tcbInReleaseQueue(tcb->tcbState))) {
        tcb_queue_t queue = ગાંઠ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        જો (queue.head == tcb) {
            ગાંઠ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        ગાંઠ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, false);
    }
}

સ્થિર અંતઃરેખ ticks_t PURE tcbReadyTime(tcb_t *tcb)
{
    પાછા refill_head(tcb->tcbSchedContext)->rTime;
}

સ્થિર અંતઃરેખ bool_t PURE time_after(tcb_t *tcb, ticks_t new_time)
{
    પાછા tcb != ખાલી && new_time >= tcbReadyTime(tcb);
}

સ્થિર tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    જ્યારે (time_after(after, new_time)) {
        after = after->tcbSchedNext;
    }

    પાછા after;
}

શૂન્ય tcbReleaseEnqueue(tcb_t *tcb)
{
    પ્રતિજ્ઞા(thread_state_get_tcbInReleaseQueue(tcb->tcbState) == false);
    પ્રતિજ્ઞા(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = ગાંઠ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    જો (tcb_queue_empty(queue) || new_time < tcbReadyTime(queue.head)) {
        ગાંઠ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } નહીંતર {
        જો (tcbReadyTime(queue.end) <= new_time) {
            ગાંઠ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } નહીંતર {
            tcb_t *after;
            after = find_time_after(queue.head, new_time);
            tcb_queue_ઉમેરો(tcb, after);
        }
    }

    thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, true);

    જો (queue.head != ગાંઠ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).head) {
        ગાંઠ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#endif

cptr_t PURE getExtraCPtr(word_t *bufferPtr, word_t i)
{
    પાછા (cptr_t)bufferPtr[se_L4_સંદેશ_Max_Length + 2 + i];
}

શૂન્ય setExtraBadge(word_t *bufferPtr, word_t badge,
                   word_t i)
{
    bufferPtr[se_L4_સંદેશ_Max_Length + 2 + i] = badge;
}

#ifndef CONFIG_KERNEL_MCS
શૂન્ય setupCallerCap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *replySlot, *callerSlot;
    cap_t masterCap UNUSED, callerCap UNUSED;

    setThreadState(sender, ThreadState_BlockedOnReply);
    replySlot = TCB_PTR_CTE_PTR(sender, tcbReply);
    masterCap = replySlot->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    પ્રતિજ્ઞા(cap_get_capType(masterCap) == cap_reply_cap);
    પ્રતિજ્ઞા(cap_reply_cap_get_capReplyMaster(masterCap));
    પ્રતિજ્ઞા(cap_reply_cap_get_capReplyCanGrant(masterCap));
    પ્રતિજ્ઞા(TCB_PTR(cap_reply_cap_get_capTCBPtr(masterCap)) == sender);
    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = callerSlot->cap;
    /* Haskell error: "Caller cap must not already exist" */
    પ્રતિજ્ઞા(cap_get_capType(callerCap) == cap_null_cap);
    cte_ઉમેરો(cap_reply_cap_new(canGrant, false, TCB_REF(sender)),
              replySlot, callerSlot);
}

શૂન્ય deleteCallerCap(tcb_t *receiver)
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

    જો (!bufferPtr) {
        current_extra_caps.excaprefs[0] = ખાલી;
        પાછા EXCEPTION_NONE;
    }

    length = seL4_MessageInfo_get_extraCaps(info);

    માટે (i = 0; i < length; i++) {
        cptr = getExtraCPtr(bufferPtr, i);

        lu_ret = lookupSlot(thread, cptr);
        જો (lu_ret.status != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            પાછા lu_ret.status;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    જો (i < se_L4_સંદેશ_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = ખાલી;
    }

    પાછા EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    માટે (i = 0; i < n && i < n_સંદેશ_Registers; i++) {
        setRegister(receiver, સંદેશ_Registers[i],
                    getRegister(sender, સંદેશ_Registers[i]));
    }

    જો (!recvBuf || !sendBuf) {
        પાછા i;
    }

    /* Copy out-of-line words */
    માટે (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    પાછા i;
}

#ifdef ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
શૂન્ય remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    જો (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = ગાંઠ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        જો (targetCurThread == ગાંઠ_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcbPriority > targetCurThread->tcbPriority
#ifdef CONFIG_KERNEL_MCS
            || ગાંઠ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#endif
           ) {
            ARCH_ગાંઠ_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
શૂન્ય remoteTCBStall(tcb_t *tcb)
{

    જો (
#ifdef CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#endif
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        ગાંઠ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_ગાંઠ_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#ifndef CONFIG_KERNEL_MCS
સ્થિર exception_t invokeTCB_SetAffinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcbSchedDequeue(thread);
    migrateTCB(thread, affinity);
    જો (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    જો (thread == ગાંઠ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeSetAffinity(cap_t cap, word_t length, word_t *buffer)
{
    tcb_t *tcb;
    word_t affinity;

    જો (length < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, buffer);
    જો (affinity >= ks_અંક_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeTCB_SetAffinity(tcb, affinity);
}
#endif
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_HARDWARE_DEBUG_API
સ્થિર exception_t invokeConfigureSingleStepping(bool_t call, word_t *buffer, tcb_t *t,
                                                 uint16_t bp_અંક, word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);
    word_t કિંમત;

    bp_was_consumed = configureSingleStepping(t, bp_અંક, n_instrs, false);
    જો (n_instrs == 0) {
#ifndef CONFIG_ARCH_AARCH64
        unsetBreakpointUsedFlag(t, bp_અંક);
#endif /* CONFIG_ARCH_AARCH64 */
        કિંમત = false;
    } નહીંતર {
#ifndef CONFIG_ARCH_AARCH64
        setBreakpointUsedFlag(t, bp_અંક);
#endif /* CONFIG_ARCH_AARCH64 */
        કિંમત = bp_was_consumed;
    }

    જો (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned પૂર્ણાંક length = setMR(thread, buffer, 0, કિંમત);
        setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *buffer)
{
    uint16_t bp_અંક;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_અંક = getSyscallArg(0, buffer);
    n_instrs = getSyscallArg(1, buffer);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_અંક, n_instrs, false);
    જો (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeConfigureSingleStepping(call, buffer, tcb, bp_અંક, n_instrs);
}

સ્થિર exception_t invokeSetBreakpoint(tcb_t *tcb, uint16_t bp_અંક,
                                       word_t vaddr, word_t type, word_t size, word_t rw)
{
    setBreakpoint(tcb, bp_અંક, vaddr, type, size, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    setBreakpointUsedFlag(tcb, bp_અંક);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeSetBreakpoint(cap_t cap, word_t *buffer)
{
    uint16_t bp_અંક;
    word_t vaddr, type, size, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_અંક = getSyscallArg(0, buffer);
    vaddr = getSyscallArg(1, buffer);
    type = getSyscallArg(2, buffer);
    size = getSyscallArg(3, buffer);
    rw = getSyscallArg(4, buffer);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    જો (vaddr >= (word_t)USER_ટોચ) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (type != seL4_InstructionBreakpoint && type != seL4_DataBreakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 2;
        પાછા EXCEPTION_SYSCALL_ERROR;
    } નહીંતર જો (type == seL4_InstructionBreakpoint) {
        જો (size != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 3;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        જો (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 4;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        જો ((seL4_FirstWatchpoint == -1 || bp_અંક >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 2;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
    } નહીંતર જો (type == seL4_DataBreakpoint) {
        જો (size == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 3;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        જો (seL4_FirstWatchpoint != -1 && bp_અંક < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 2;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
    } નહીંતર જો (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 2;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 3;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (size != 0 && size != 1 && size != 2 && size != 4 && size != 8) {
        userError("Debug: Invalid size %lu.", size);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 3;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (size > 0 && vaddr & (size - 1)) {
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
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decodeSetBreakpoint(tcb, bp_અંક, vaddr, type, size, rw);
    જો (error.type != seL4_NoError) {
        current_syscall_error = error;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeSetBreakpoint(tcb, bp_અંક,
                               vaddr, type, size, rw);
}

સ્થિર exception_t invokeGetBreakpoint(bool_t call, word_t *buffer, tcb_t *tcb, uint16_t bp_અંક)
{
    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_અંક);
    જો (call) {
        setRegister(thread, badgeRegister, 0);
        setMR(ગાંઠ_STATE(ksCurThread), buffer, 0, res.vaddr);
        setMR(ગાંઠ_STATE(ksCurThread), buffer, 1, res.type);
        setMR(ગાંઠ_STATE(ksCurThread), buffer, 2, res.size);
        setMR(ગાંઠ_STATE(ksCurThread), buffer, 3, res.rw);
        setMR(ગાંઠ_STATE(ksCurThread), buffer, 4, res.is_enabled);
        setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, 5)));
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_અંક;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_અંક = getSyscallArg(0, buffer);

    error = Arch_decodeGetBreakpoint(tcb, bp_અંક);
    જો (error.type != seL4_NoError) {
        current_syscall_error = error;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeGetBreakpoint(call, buffer, tcb, bp_અંક);
}

સ્થિર exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_અંક)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_અંક);
    unsetBreakpointUsedFlag(tcb, bp_અંક);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeUnsetBreakpoint(cap_t cap, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_અંક;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_અંક = getSyscallArg(0, buffer);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_અંક);
    જો (error.type != seL4_NoError) {
        current_syscall_error = error;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeUnsetBreakpoint(tcb, bp_અંક);
}
#endif /* CONFIG_HARDWARE_DEBUG_API */

સ્થિર exception_t invokeSetTLSBase(tcb_t *thread, word_t tls_base)
{
    setRegister(thread, TLS_BASE, tls_base);
    જો (thread == ગાંઠ_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeSetTLSBase(cap_t cap, word_t length, word_t *buffer)
{
    word_t tls_base;

    જો (length < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    tls_base = getSyscallArg(0, buffer);

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeSetTLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_base);
}

સ્થિર શૂન્ય invokeSetFlags(tcb_t *thread, word_t clear, word_t set, bool_t call)
{
    tcb_t *cur_thread = ગાંઠ_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= set & seL4_TCBFlag_MASK;
    thread->tcbFlags = flags;

#ifdef CONFIG_HAVE_FPU
    જો (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } નહીંતર જો (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#endif
    જો (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, cur_thread);
        setRegister(cur_thread, badgeRegister, 0);
        unsigned પૂર્ણાંક length = setMR(cur_thread, ipcBuffer, 0, flags);
        setRegister(cur_thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(cur_thread, ThreadState_Running);
}

સ્થિર exception_t decodeSetFlags(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    જો (length < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, buffer);
    word_t set   = getSyscallArg(1, buffer);

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    invokeSetFlags(thread, clear, set, call);
    પાછા EXCEPTION_NONE;
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

    પસંદગી (invLabel) {
    સ્થિતિ TCBReadRegisters:
        /* Second level of decoding */
        પાછા decodeReadRegisters(cap, length, call, buffer);

    સ્થિતિ TCBWriteRegisters:
        પાછા decodeWriteRegisters(cap, length, buffer);

    સ્થિતિ TCBCopyRegisters:
        પાછા decodeCopyRegisters(cap, length, buffer);

    સ્થિતિ TCBSuspend:
        /* Jump straight to the invoke */
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    સ્થિતિ TCBResume:
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    સ્થિતિ TCBConfigure:
        પાછા decodeTCBConfigure(cap, length, slot, buffer);

    સ્થિતિ TCBSetPriority:
        પાછા decodeSetPriority(cap, length, buffer);

    સ્થિતિ TCBSetMCPriority:
        પાછા decodeSetMCPriority(cap, length, buffer);

    સ્થિતિ TCBSetSchedParams:
#ifdef CONFIG_KERNEL_MCS
        પાછા decodeSetSchedParams(cap, length, slot, buffer);
#નહીંતર
        પાછા decodeSetSchedParams(cap, length, buffer);
#endif

    સ્થિતિ TCBSetIPCBuffer:
        પાછા decodeSetIPCBuffer(cap, length, slot, buffer);

    સ્થિતિ TCBSetSpace:
        પાછા decodeSetSpace(cap, length, slot, buffer);

    સ્થિતિ TCBBindNotification:
        પાછા decodeBindNotification(cap);

    સ્થિતિ TCBUnbindNotification:
        પાછા decodeUnbindNotification(cap);

#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ TCBSetTimeoutEndpoint:
        પાછા decodeSetTimeoutEndpoint(cap, slot);
#નહીંતર
#ifdef ENABLE_SMP_SUPPORT
    સ્થિતિ TCBSetAffinity:
        પાછા decodeSetAffinity(cap, length, buffer);
#endif /* ENABLE_SMP_SUPPORT */
#endif

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ifdef CONFIG_VTX
    સ્થિતિ TCBSetEPTRoot:
        પાછા decodeSetEPTRoot(cap);
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    સ્થિતિ TCBConfigureSingleStepping:
        પાછા decodeConfigureSingleStepping(cap, call, buffer);

    સ્થિતિ TCBSetBreakpoint:
        પાછા decodeSetBreakpoint(cap, buffer);

    સ્થિતિ TCBGetBreakpoint:
        પાછા decodeGetBreakpoint(cap, call, buffer);

    સ્થિતિ TCBUnsetBreakpoint:
        પાછા decodeUnsetBreakpoint(cap, buffer);
#endif

    સ્થિતિ TCBSetTLSBase:
        પાછા decodeSetTLSBase(cap, length, buffer);

    સ્થિતિ TCBSetFlags:
        પાછા decodeSetFlags(cap, length, call, buffer);

    મૂળભૂત:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}

ગણના CopyRegistersFlags {
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

    જો (length < 1 || current_extra_caps.excaprefs[0] == ખાલી) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    જો (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } નહીંતર {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & BIT(CopyRegisters_suspendSource),
               flags & BIT(CopyRegisters_resumeTarget),
               flags & BIT(CopyRegisters_transferFrame),
               flags & BIT(CopyRegisters_transferInteger),
               transferArch);

}

ગણના ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, word_t length, bool_t call,
                                word_t *buffer)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    જો (length < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    n     = getSyscallArg(1, buffer);

    જો (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (પૂર્ણાંક)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    જો (thread == ગાંઠ_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

ગણના WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    જો (length < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    w     = getSyscallArg(1, buffer);

    જો (length - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (પૂર્ણાંક)(length - 2), (પૂર્ણાંક)w);
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    જો (thread == ગાંઠ_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, buffer);
}

#ifdef CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_endpoint_cap:
        પાછા (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_capCanGrantReply(cap)));
    સ્થિતિ cap_null_cap:
        /* just has no fault endpoint */
        પાછા true;
    મૂળભૂત:
        પાછા false;
    }
}
#endif

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cte_t *bufferSlot, *c_મૂળ_Slot, *v_મૂળ_Slot;
    cap_t bufferCap, c_મૂળ_Cap, v_મૂળ_Cap;
    deriveCap_ret_t dc_ret;
    word_t c_મૂળ_Data, v_મૂળ_Data, bufferAddr;
#ifdef CONFIG_KERNEL_MCS
#define TCBCONFIGURE_ARGS 3
#નહીંતર
#define TCBCONFIGURE_ARGS 4
#endif
    જો (length < TCBCONFIGURE_ARGS || current_extra_caps.excaprefs[0] == ખાલી
        || current_extra_caps.excaprefs[1] == ખાલી
        || current_extra_caps.excaprefs[2] == ખાલી) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_મૂળ_Data     = getSyscallArg(0, buffer);
    v_મૂળ_Data     = getSyscallArg(1, buffer);
    bufferAddr    = getSyscallArg(2, buffer);
#નહીંતર
    cptr_t faultEP       = getSyscallArg(0, buffer);
    c_મૂળ_Data     = getSyscallArg(1, buffer);
    v_મૂળ_Data     = getSyscallArg(2, buffer);
    bufferAddr    = getSyscallArg(3, buffer);
#endif

    c_મૂળ_Slot  = current_extra_caps.excaprefs[0];
    c_મૂળ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_મૂળ_Slot  = current_extra_caps.excaprefs[1];
    v_મૂળ_Cap   = current_extra_caps.excaprefs[1]->cap;
    bufferSlot = current_extra_caps.excaprefs[2];
    bufferCap  = current_extra_caps.excaprefs[2]->cap;

    જો (bufferAddr == 0) {
        bufferSlot = ખાલી;
    } નહીંતર {
        dc_ret = deriveCap(bufferSlot, bufferCap);
        જો (dc_ret.status != EXCEPTION_NONE) {
            પાછા dc_ret.status;
        }
        bufferCap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(bufferAddr, bufferCap);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }
    }

    જો (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (c_મૂળ_Data != 0) {
        c_મૂળ_Cap = updateCapData(false, c_મૂળ_Data, c_મૂળ_Cap);
    }

    dc_ret = deriveCap(c_મૂળ_Slot, c_મૂળ_Cap);
    જો (dc_ret.status != EXCEPTION_NONE) {
        પાછા dc_ret.status;
    }
    c_મૂળ_Cap = dc_ret.cap;

    જો (cap_get_capType(c_મૂળ_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (v_મૂળ_Data != 0) {
        v_મૂળ_Cap = updateCapData(false, v_મૂળ_Data, v_મૂળ_Cap);
    }

    dc_ret = deriveCap(v_મૂળ_Slot, v_મૂળ_Cap);
    જો (dc_ret.status != EXCEPTION_NONE) {
        પાછા dc_ret.status;
    }
    v_મૂળ_Cap = dc_ret.cap;

    જો (!is_Valid_VTable_મૂળ(v_મૂળ_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    પાછા invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), ખાલી,
               cap_null_cap_new(), ખાલી,
               c_મૂળ_Cap, c_મૂળ_Slot,
               v_મૂળ_Cap, v_મૂળ_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_buffer);
#નહીંતર
    પાછા invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_મૂળ_Cap, c_મૂળ_Slot,
               v_મૂળ_Cap, v_મૂળ_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_update_space |
               thread_control_update_ipc_buffer);
#endif
}

exception_t decodeSetPriority(cap_t cap, word_t length, word_t *buffer)
{
    જો (length < 1 || current_extra_caps.excaprefs[0] == ખાલી) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    જો (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newPrio, authTCB);
    જો (status != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        પાછા status;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    પાછા invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ખાલી,
               cap_null_cap_new(), ખાલી,
               NULL_PRIO, newPrio,
               ખાલી, thread_control_sched_update_priority);
#નહીંતર
    પાછા invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ખાલી,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), ખાલી,
               cap_null_cap_new(), ખાલી,
               0, cap_null_cap_new(),
               ખાલી, thread_control_update_priority);
#endif
}

exception_t decodeSetMCPriority(cap_t cap, word_t length, word_t *buffer)
{
    જો (length < 1 || current_extra_caps.excaprefs[0] == ખાલી) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    જો (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    જો (status != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        પાછા status;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    પાછા invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ખાલી,
               cap_null_cap_new(), ખાલી,
               newMcp, NULL_PRIO,
               ખાલી, thread_control_sched_update_mcp);
#નહીંતર
    પાછા invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ખાલી,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), ખાલી,
               cap_null_cap_new(), ખાલી,
               0, cap_null_cap_new(),
               ખાલી, thread_control_update_mcp);
#endif
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetTimeoutEndpoint(cap_t cap, cte_t *slot)
{
    જો (current_extra_caps.excaprefs[0] == ખાલી) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    જો (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), ખાલી,
               thCap, thSlot,
               cap_null_cap_new(), ખાલી,
               cap_null_cap_new(), ખાલી,
               0, cap_null_cap_new(), ખાલી,
               thread_control_caps_update_timeout);
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetSchedParams(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
#નહીંતર
exception_t decodeSetSchedParams(cap_t cap, word_t length, word_t *buffer)
#endif
{
    જો (length < 2 || current_extra_caps.excaprefs[0] == ખાલી
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == ખાલી || current_extra_caps.excaprefs[2] == ખાલી
#endif
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    prio_t newPrio = getSyscallArg(1, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ifdef CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#endif

    જો (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    જો (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        પાછા status;
    }

    status = checkPrio(newPrio, authTCB);
    જો (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        પાછા status;
    }

#ifdef CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = ખાલી;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_priority |
                                         thread_control_sched_update_fault;
    પસંદગી (cap_get_capType(scCap)) {
    સ્થિતિ cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        જો (tcb->tcbSchedContext != sc) {
            જો (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }
            જો (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }
        }
        જો (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        તોડો;
    સ્થિતિ cap_null_cap:
        જો (tcb == ગાંઠ_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        તોડો;
    મૂળભૂત:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 2;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    જો (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    જો (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 3;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
#endif
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    પાછા invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#નહીંતર
    પાછા invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ખાલી,
               0, newMcp, newPrio,
               cap_null_cap_new(), ખાલી,
               cap_null_cap_new(), ખાલી,
               0, cap_null_cap_new(),
               ખાલી, thread_control_update_mcp |
               thread_control_update_priority);
#endif
}


exception_t decodeSetIPCBuffer(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cptr_t cptr_bufferPtr;
    cap_t bufferCap;
    cte_t *bufferSlot;

    જો (length < 1 || current_extra_caps.excaprefs[0] == ખાલી) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    cptr_bufferPtr  = getSyscallArg(0, buffer);
    bufferSlot = current_extra_caps.excaprefs[0];
    bufferCap  = current_extra_caps.excaprefs[0]->cap;

    જો (cptr_bufferPtr == 0) {
        bufferSlot = ખાલી;
    } નહીંતર {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(bufferSlot, bufferCap);
        જો (dc_ret.status != EXCEPTION_NONE) {
            પાછા dc_ret.status;
        }
        bufferCap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_bufferPtr, bufferCap);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    પાછા invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), ખાલી,
               cap_null_cap_new(), ખાલી,
               cap_null_cap_new(), ખાલી,
               cap_null_cap_new(), ખાલી,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_caps_update_ipc_buffer);
#નહીંતર
    પાછા invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), ખાલી,
               cap_null_cap_new(), ખાલી,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_update_ipc_buffer);

#endif
}

#ifdef CONFIG_KERNEL_MCS
#define DECODE_SET_SPACE_PARAMS 2
#નહીંતર
#define DECODE_SET_SPACE_PARAMS 3
#endif
exception_t decodeSetSpace(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    word_t c_મૂળ_Data, v_મૂળ_Data;
    cte_t *c_મૂળ_Slot, *v_મૂળ_Slot;
    cap_t c_મૂળ_Cap, v_મૂળ_Cap;
    deriveCap_ret_t dc_ret;

    જો (length < DECODE_SET_SPACE_PARAMS || current_extra_caps.excaprefs[0] == ખાલી
        || current_extra_caps.excaprefs[1] == ખાલી
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == ખાલી
#endif
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_મૂળ_Data = getSyscallArg(0, buffer);
    v_મૂળ_Data = getSyscallArg(1, buffer);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_મૂળ_Slot  = current_extra_caps.excaprefs[1];
    c_મૂળ_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_મૂળ_Slot  = current_extra_caps.excaprefs[2];
    v_મૂળ_Cap   = current_extra_caps.excaprefs[2]->cap;
#નહીંતર
    cptr_t faultEP   = getSyscallArg(0, buffer);
    c_મૂળ_Data = getSyscallArg(1, buffer);
    v_મૂળ_Data = getSyscallArg(2, buffer);

    c_મૂળ_Slot  = current_extra_caps.excaprefs[0];
    c_મૂળ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_મૂળ_Slot  = current_extra_caps.excaprefs[1];
    v_મૂળ_Cap   = current_extra_caps.excaprefs[1]->cap;
#endif

    જો (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (c_મૂળ_Data != 0) {
        c_મૂળ_Cap = updateCapData(false, c_મૂળ_Data, c_મૂળ_Cap);
    }

    dc_ret = deriveCap(c_મૂળ_Slot, c_મૂળ_Cap);
    જો (dc_ret.status != EXCEPTION_NONE) {
        પાછા dc_ret.status;
    }
    c_મૂળ_Cap = dc_ret.cap;

    જો (cap_get_capType(c_મૂળ_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (v_મૂળ_Data != 0) {
        v_મૂળ_Cap = updateCapData(false, v_મૂળ_Data, v_મૂળ_Cap);
    }

    dc_ret = deriveCap(v_મૂળ_Slot, v_મૂળ_Cap);
    જો (dc_ret.status != EXCEPTION_NONE) {
        પાછા dc_ret.status;
    }
    v_મૂળ_Cap = dc_ret.cap;

    જો (!is_Valid_VTable_મૂળ(v_મૂળ_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    /* fault handler */
    જો (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
#endif

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    પાછા invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), ખાલી,
               c_મૂળ_Cap, c_મૂળ_Slot,
               v_મૂળ_Cap, v_મૂળ_Slot,
               0, cap_null_cap_new(), ખાલી, thread_control_caps_update_space | thread_control_caps_update_fault);
#નહીંતર
    પાછા invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_મૂળ_Cap, c_મૂળ_Slot,
               v_મૂળ_Cap, v_મૂળ_Slot,
               0, cap_null_cap_new(), ખાલી, thread_control_update_space);
#endif
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    જો (current_extra_caps.excaprefs[0] == ખાલી) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    જો (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    જો (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } નહીંતર {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો ((tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }


    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    જો (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeTCB_NotificationControl(tcb, ખાલી);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    પાછા EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    પાછા EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
સ્થિર અંતઃરેખ exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_index_t index, cap_t newCap, cte_t *srcSlot)
{
    cte_t *મૂળ_Slot = TCB_PTR_CTE_PTR(target, index);
    UNUSED exception_t e = cteDelete(મૂળ_Slot, true);
    જો (e != EXCEPTION_NONE) {
        પાછા e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    જો (cap_get_capType(newCap) != cap_null_cap) {
        જો (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ઉમેરો(newCap, srcSlot, મૂળ_Slot);
        }
    }
    પાછા e;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t c_મૂળ_new_Cap, cte_t *c_મૂળ_src_Slot,
                                        cap_t v_મૂળ_new_Cap, cte_t *v_મૂળ_src_Slot,
                                        word_t bufferAddr, cap_t bufferCap,
                                        cte_t *bufferSrcSlot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    જો (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }

    }

    જો (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }
    }

    જો (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, c_મૂળ_new_Cap, c_મૂળ_src_Slot);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, v_મૂળ_new_Cap, v_મૂળ_src_Slot);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }
    }

    જો (updateFlags & thread_control_caps_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }
        target->tcbIPCBuffer = bufferAddr;

        જો (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ઉમેરો(bufferCap, bufferSrcSlot, bufferSlot);
        }

        જો (target == ગાંઠ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    પાછા EXCEPTION_NONE;
}
#નહીંતર
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t priority,
                                    cap_t c_મૂળ_new_Cap, cte_t *c_મૂળ_src_Slot,
                                    cap_t v_મૂળ_new_Cap, cte_t *v_મૂળ_src_Slot,
                                    word_t bufferAddr, cap_t bufferCap,
                                    cte_t *bufferSrcSlot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    જો (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    જો (updateFlags & thread_control_update_mcp) {
        setMCPriority(target, mcp);
    }

    જો (updateFlags & thread_control_update_space) {
        cte_t *મૂળ_Slot;

        મૂળ_Slot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(મૂળ_Slot, true);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }
        જો (sameObjectAs(c_મૂળ_new_Cap, c_મૂળ_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ઉમેરો(c_મૂળ_new_Cap, c_મૂળ_src_Slot, મૂળ_Slot);
        }
    }

    જો (updateFlags & thread_control_update_space) {
        cte_t *મૂળ_Slot;

        મૂળ_Slot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(મૂળ_Slot, true);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }
        જો (sameObjectAs(v_મૂળ_new_Cap, v_મૂળ_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ઉમેરો(v_મૂળ_new_Cap, v_મૂળ_src_Slot, મૂળ_Slot);
        }
    }

    જો (updateFlags & thread_control_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }
        target->tcbIPCBuffer = bufferAddr;

        જો (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_ઉમેરો(bufferCap, bufferSrcSlot, bufferSlot);
        }

        જો (target == ગાંઠ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    જો (updateFlags & thread_control_update_priority) {
        setPriority(target, priority);
    }

    પાછા EXCEPTION_NONE;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t priority,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    જો (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        જો (e != EXCEPTION_NONE) {
            પાછા e;
        }
    }

    જો (updateFlags & thread_control_sched_update_mcp) {
        setMCPriority(target, mcp);
    }

    જો (updateFlags & thread_control_sched_update_priority) {
        setPriority(target, priority);
    }

    જો (updateFlags & thread_control_sched_update_sc) {
        જો (sc != ખાલી) {
            schedContext_bindTCB(sc, target);
        } નહીંતર જો (sc == ખાલી) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    પાછા EXCEPTION_NONE;
}
#endif

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    જો (suspendSource) {
        suspend(tcb_src);
    }

    જો (resumeTarget) {
        restart(dest);
    }

    જો (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        માટે (i = 0; i < n_frameRegisters; i++) {
            v = getRegister(tcb_src, frameRegisters[i]);
            setRegister(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        setNextPC(dest, pc);
    }

    જો (transferInteger) {
        word_t i;
        word_t v;

        માટે (i = 0; i < n_gpRegisters; i++) {
            v = getRegister(tcb_src, gpRegisters[i]);
            setRegister(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    જો (dest == ગાંઠ_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    પાછા Arch_performTransfer(transferArch, tcb_src, dest);
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

    thread = ગાંઠ_STATE(ksCurThread);

    જો (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, ગાંઠ_STATE(ksCurThread));
    જો (e != EXCEPTION_NONE) {
        પાછા e;
    }

    જો (call) {
        word_t *ipcBuffer;

        ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);

        માટે (i = 0; i < n && i < n_frameRegisters && i < n_સંદેશ_Registers; i++) {
            setRegister(thread, સંદેશ_Registers[i],
                        getRegister(tcb_src, frameRegisters[i]));
        }

        જો (ipcBuffer != ખાલી && i < n && i < n_frameRegisters) {
            માટે (; i < n && i < n_frameRegisters; i++) {
                ipcBuffer[i + 1] = getRegister(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        માટે (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_સંદેશ_Registers; i++) {
            setRegister(thread, સંદેશ_Registers[i + n_frameRegisters],
                        getRegister(tcb_src, gpRegisters[i]));
        }

        જો (ipcBuffer != ખાલી && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            માટે (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipcBuffer[i + n_frameRegisters + 1] =
                    getRegister(tcb_src, gpRegisters[i]);
            }
        }

        setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, i + j)));
    }
    setThreadState(thread, ThreadState_Running);

    પાછા EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *buffer)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, ગાંઠ_STATE(ksCurThread), dest);
    જો (e != EXCEPTION_NONE) {
        પાછા e;
    }

    જો (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_getSanitiseRegisterInfo(dest);

    માટે (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        setRegister(dest, frameRegisters[i],
                    sanitiseRegister(frameRegisters[i],
                                     getSyscallArg(i + 2, buffer), archInfo));
    }

    માટે (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        setRegister(dest, gpRegisters[i],
                    sanitiseRegister(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   buffer), archInfo));
    }

    pc = getRestartPC(dest);
    setNextPC(dest, pc);

    Arch_postModifyRegisters(dest);

    જો (resumeTarget) {
        restart(dest);
    }

    જો (dest == ગાંઠ_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    પાછા EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    જો (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } નહીંતર {
        unbindNotification(tcb);
    }

    પાછા EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD
શૂન્ય setThreadName(tcb_t *tcb, અચલ અક્ષર *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_LENGTH);
}
#endif

word_t setMRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    પસંદગી (current_syscall_error.type) {
    સ્થિતિ seL4_InvalidArgument:
        પાછા setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Argument_સંખ્યા);

    સ્થિતિ seL4_InvalidCapability:
        પાછા setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_સંખ્યા);

    સ્થિતિ seL4_IllegalOperation:
        પાછા 0;

    સ્થિતિ seL4_RangeError:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        પાછા setMR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    સ્થિતિ seL4_AlignmentError:
        પાછા 0;

    સ્થિતિ seL4_FailedLookup:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        પાછા setMRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    સ્થિતિ seL4_TruncatedMessage:
    સ્થિતિ seL4_DeleteFirst:
    સ્થિતિ seL4_RevokeFirst:
        પાછા 0;
    સ્થિતિ seL4_NotEnoughMemory:
        પાછા setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.memory_ડાબું);
    મૂળભૂત:
        fail("Invalid syscall error");
    }
}
