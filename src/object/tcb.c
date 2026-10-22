/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <api/invocation.h>
#شامل <api/syscall.h>
#شامل <sel4/shared_types.h>
#شامل <machine/io.h>
#شامل <object/structures.h>
#شامل <object/objecttype.h>
#شامل <object/cnode.h>
#ifdef CONFIG_KERNEL_MCS
#شامل <object/schedcontext.h>
#endif
#شامل <object/tcb.h>
#شامل <kernel/cspace.h>
#شامل <kernel/thread.h>
#شامل <kernel/vspace.h>
#شامل <model/statedata.h>
#شامل <util.h>
#شامل <string.h>
#شامل <stdint.h>
#شامل <arch/smp/ipi_inline.h>

#define NULL_PRIO 0

ایستا exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    أكد(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    اگر (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    بازگشت EXCEPTION_NONE;
}

ایستا درونخطی تهی addToBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    گره_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    گره_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & MASK(wordRadix));
}

ایستا درونخطی تهی removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    گره_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & MASK(wordRadix));
    اگر (unlikely(!گره_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        گره_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcbSchedNext;

    اگر (queue.head == tcb && queue.end == tcb) {
        queue.head = پوچ;
        queue.end = پوچ;
    } وگرنه {
        اگر (queue.head == tcb) {
            after->tcbSchedPrev = پوچ;
            tcb->tcbSchedNext = پوچ;
            queue.head = after;
        } وگرنه {
            اگر (queue.end == tcb) {
                before->tcbSchedNext = پوچ;
                tcb->tcbSchedPrev = پوچ;
                queue.end = before;
            } وگرنه {
                before->tcbSchedNext = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = پوچ;
                tcb->tcbSchedNext = پوچ;
            }
        }
    }

    بازگشت queue;
}

/* Add TCB to the head of a scheduler queue */
تهی tcbSchedEnqueue(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
#endif

    اگر (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        اگر (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
تهی tcbSchedAppend(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
    أكد(refill_ready(tcb->tcbSchedContext));
#endif
    اگر (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        اگر (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
تهی tcbSchedDequeue(tcb_t *tcb)
{
    اگر (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, false);

        اگر (likely(tcb_queue_empty(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
تهی tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = پوچ;

    debug_tcb->tcbDebugNext = گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    اگر (گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

تهی tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    أكد(گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != پوچ);
    اگر (tcb == گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(گره_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcbDebugNext;
    } وگرنه {
        أكد(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcbDebugNext = debug_tcb->tcbDebugNext;
    }

    اگر (debug_tcb->tcbDebugNext) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugNext)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = پوچ;
    debug_tcb->tcbDebugNext = پوچ;
}
#endif /* CONFIG_DEBUG_BUILD */

#ifndef CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    اگر (!queue.head) { /* Empty list */
        queue.head = tcb;
    } وگرنه {
        queue.end->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.end;
    tcb->tcbEPNext = پوچ;
    queue.end = tcb;

    بازگشت queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    اگر (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } وگرنه {
        queue.head = tcb->tcbEPNext;
    }

    اگر (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } وگرنه {
        queue.end = tcb->tcbEPPrev;
    }

    بازگشت queue;
}
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS

تهی tcbReleaseRemove(tcb_t *tcb)
{
    اگر (likely(thread_state_get_tcbInReleaseQueue(tcb->tcbState))) {
        tcb_queue_t queue = گره_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        اگر (queue.head == tcb) {
            گره_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        گره_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, false);
    }
}

ایستا درونخطی ticks_t PURE tcbReadyTime(tcb_t *tcb)
{
    بازگشت refill_head(tcb->tcbSchedContext)->rTime;
}

ایستا درونخطی bool_t PURE time_after(tcb_t *tcb, ticks_t new_time)
{
    بازگشت tcb != پوچ && new_time >= tcbReadyTime(tcb);
}

ایستا tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    وقتی (time_after(after, new_time)) {
        after = after->tcbSchedNext;
    }

    بازگشت after;
}

تهی tcbReleaseEnqueue(tcb_t *tcb)
{
    أكد(thread_state_get_tcbInReleaseQueue(tcb->tcbState) == false);
    أكد(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = گره_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    اگر (tcb_queue_empty(queue) || new_time < tcbReadyTime(queue.head)) {
        گره_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } وگرنه {
        اگر (tcbReadyTime(queue.end) <= new_time) {
            گره_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } وگرنه {
            tcb_t *after;
            after = find_time_after(queue.head, new_time);
            tcb_queue_درج(tcb, after);
        }
    }

    thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, true);

    اگر (queue.head != گره_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).head) {
        گره_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#endif

cptr_t PURE getExtraCPtr(word_t *bufferPtr, word_t i)
{
    بازگشت (cptr_t)bufferPtr[se_L4_پیام_Max_Length + 2 + i];
}

تهی setExtraBadge(word_t *bufferPtr, word_t badge,
                   word_t i)
{
    bufferPtr[se_L4_پیام_Max_Length + 2 + i] = badge;
}

#ifndef CONFIG_KERNEL_MCS
تهی setupCallerCap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *replySlot, *callerSlot;
    cap_t masterCap UNUSED, callerCap UNUSED;

    setThreadState(sender, ThreadState_BlockedOnReply);
    replySlot = TCB_PTR_CTE_PTR(sender, tcbReply);
    masterCap = replySlot->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    أكد(cap_get_capType(masterCap) == cap_reply_cap);
    أكد(cap_reply_cap_get_capReplyMaster(masterCap));
    أكد(cap_reply_cap_get_capReplyCanGrant(masterCap));
    أكد(TCB_PTR(cap_reply_cap_get_capTCBPtr(masterCap)) == sender);
    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = callerSlot->cap;
    /* Haskell error: "Caller cap must not already exist" */
    أكد(cap_get_capType(callerCap) == cap_null_cap);
    cte_درج(cap_reply_cap_new(canGrant, false, TCB_REF(sender)),
              replySlot, callerSlot);
}

تهی deleteCallerCap(tcb_t *receiver)
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

    اگر (!bufferPtr) {
        current_extra_caps.excaprefs[0] = پوچ;
        بازگشت EXCEPTION_NONE;
    }

    length = seL4_MessageInfo_get_extraCaps(info);

    برای (i = 0; i < length; i++) {
        cptr = getExtraCPtr(bufferPtr, i);

        lu_ret = lookupSlot(thread, cptr);
        اگر (lu_ret.status != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            بازگشت lu_ret.status;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    اگر (i < se_L4_پیام_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = پوچ;
    }

    بازگشت EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    برای (i = 0; i < n && i < n_پیام_Registers; i++) {
        setRegister(receiver, پیام_Registers[i],
                    getRegister(sender, پیام_Registers[i]));
    }

    اگر (!recvBuf || !sendBuf) {
        بازگشت i;
    }

    /* Copy out-of-line words */
    برای (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    بازگشت i;
}

#ifdef ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
تهی remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    اگر (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = گره_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        اگر (targetCurThread == گره_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcbPriority > targetCurThread->tcbPriority
#ifdef CONFIG_KERNEL_MCS
            || گره_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#endif
           ) {
            ARCH_گره_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
تهی remoteTCBStall(tcb_t *tcb)
{

    اگر (
#ifdef CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#endif
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        گره_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_گره_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#ifndef CONFIG_KERNEL_MCS
ایستا exception_t invokeTCB_SetAffinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcbSchedDequeue(thread);
    migrateTCB(thread, affinity);
    اگر (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    اگر (thread == گره_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeSetAffinity(cap_t cap, word_t length, word_t *buffer)
{
    tcb_t *tcb;
    word_t affinity;

    اگر (length < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, buffer);
    اگر (affinity >= ks_رقم_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeTCB_SetAffinity(tcb, affinity);
}
#endif
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_HARDWARE_DEBUG_API
ایستا exception_t invokeConfigureSingleStepping(bool_t call, word_t *buffer, tcb_t *t,
                                                 uint16_t bp_رقم, word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    word_t مقدار;

    bp_was_consumed = configureSingleStepping(t, bp_رقم, n_instrs, false);
    اگر (n_instrs == 0) {
#ifndef CONFIG_ARCH_AARCH64
        unsetBreakpointUsedFlag(t, bp_رقم);
#endif /* CONFIG_ARCH_AARCH64 */
        مقدار = false;
    } وگرنه {
#ifndef CONFIG_ARCH_AARCH64
        setBreakpointUsedFlag(t, bp_رقم);
#endif /* CONFIG_ARCH_AARCH64 */
        مقدار = bp_was_consumed;
    }

    اگر (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned صحیح length = setMR(thread, buffer, 0, مقدار);
        setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *buffer)
{
    uint16_t bp_رقم;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_رقم = getSyscallArg(0, buffer);
    n_instrs = getSyscallArg(1, buffer);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_رقم, n_instrs, false);
    اگر (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeConfigureSingleStepping(call, buffer, tcb, bp_رقم, n_instrs);
}

ایستا exception_t invokeSetBreakpoint(tcb_t *tcb, uint16_t bp_رقم,
                                       word_t vaddr, word_t type, word_t size, word_t rw)
{
    setBreakpoint(tcb, bp_رقم, vaddr, type, size, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    setBreakpointUsedFlag(tcb, bp_رقم);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeSetBreakpoint(cap_t cap, word_t *buffer)
{
    uint16_t bp_رقم;
    word_t vaddr, type, size, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_رقم = getSyscallArg(0, buffer);
    vaddr = getSyscallArg(1, buffer);
    type = getSyscallArg(2, buffer);
    size = getSyscallArg(3, buffer);
    rw = getSyscallArg(4, buffer);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    اگر (vaddr >= (word_t)USER_بالاترین) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (type != seL4_InstructionBreakpoint && type != seL4_DataBreakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 2;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    } وگرنه اگر (type == seL4_InstructionBreakpoint) {
        اگر (size != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_عدد = 3;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        اگر (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_عدد = 4;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        اگر ((seL4_FirstWatchpoint == -1 || bp_رقم >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_عدد = 2;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
    } وگرنه اگر (type == seL4_DataBreakpoint) {
        اگر (size == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_عدد = 3;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        اگر (seL4_FirstWatchpoint != -1 && bp_رقم < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_عدد = 2;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
    } وگرنه اگر (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 2;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 3;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (size != 0 && size != 1 && size != 2 && size != 4 && size != 8) {
        userError("Debug: Invalid size %lu.", size);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 3;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (size > 0 && vaddr & (size - 1)) {
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
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decodeSetBreakpoint(tcb, bp_رقم, vaddr, type, size, rw);
    اگر (error.type != seL4_NoError) {
        current_syscall_error = error;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeSetBreakpoint(tcb, bp_رقم,
                               vaddr, type, size, rw);
}

ایستا exception_t invokeGetBreakpoint(bool_t call, word_t *buffer, tcb_t *tcb, uint16_t bp_رقم)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_رقم);
    اگر (call) {
        setRegister(thread, badgeRegister, 0);
        setMR(گره_STATE(ksCurThread), buffer, 0, res.vaddr);
        setMR(گره_STATE(ksCurThread), buffer, 1, res.type);
        setMR(گره_STATE(ksCurThread), buffer, 2, res.size);
        setMR(گره_STATE(ksCurThread), buffer, 3, res.rw);
        setMR(گره_STATE(ksCurThread), buffer, 4, res.is_enabled);
        setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, 5)));
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_رقم;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_رقم = getSyscallArg(0, buffer);

    error = Arch_decodeGetBreakpoint(tcb, bp_رقم);
    اگر (error.type != seL4_NoError) {
        current_syscall_error = error;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeGetBreakpoint(call, buffer, tcb, bp_رقم);
}

ایستا exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_رقم)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_رقم);
    unsetBreakpointUsedFlag(tcb, bp_رقم);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeUnsetBreakpoint(cap_t cap, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_رقم;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_رقم = getSyscallArg(0, buffer);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_رقم);
    اگر (error.type != seL4_NoError) {
        current_syscall_error = error;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeUnsetBreakpoint(tcb, bp_رقم);
}
#endif /* CONFIG_HARDWARE_DEBUG_API */

ایستا exception_t invokeSetTLSBase(tcb_t *thread, word_t tls_base)
{
    setRegister(thread, TLS_BASE, tls_base);
    اگر (thread == گره_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeSetTLSBase(cap_t cap, word_t length, word_t *buffer)
{
    word_t tls_base;

    اگر (length < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    tls_base = getSyscallArg(0, buffer);

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeSetTLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_base);
}

ایستا تهی invokeSetFlags(tcb_t *thread, word_t clear, word_t set, bool_t call)
{
    tcb_t *cur_thread = گره_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= set & seL4_TCBFlag_MASK;
    thread->tcbFlags = flags;

#ifdef CONFIG_HAVE_FPU
    اگر (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } وگرنه اگر (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#endif
    اگر (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, cur_thread);
        setRegister(cur_thread, badgeRegister, 0);
        unsigned صحیح length = setMR(cur_thread, ipcBuffer, 0, flags);
        setRegister(cur_thread, پیام_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(cur_thread, ThreadState_Running);
}

ایستا exception_t decodeSetFlags(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    اگر (length < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, buffer);
    word_t set   = getSyscallArg(1, buffer);

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    invokeSetFlags(thread, clear, set, call);
    بازگشت EXCEPTION_NONE;
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

    انتخاب (invLabel) {
    حالت TCBReadRegisters:
        /* Second level of decoding */
        بازگشت decodeReadRegisters(cap, length, call, buffer);

    حالت TCBWriteRegisters:
        بازگشت decodeWriteRegisters(cap, length, buffer);

    حالت TCBCopyRegisters:
        بازگشت decodeCopyRegisters(cap, length, buffer);

    حالت TCBSuspend:
        /* Jump straight to the invoke */
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالت TCBResume:
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالت TCBConfigure:
        بازگشت decodeTCBConfigure(cap, length, slot, buffer);

    حالت TCBSetPriority:
        بازگشت decodeSetPriority(cap, length, buffer);

    حالت TCBSetMCPriority:
        بازگشت decodeSetMCPriority(cap, length, buffer);

    حالت TCBSetSchedParams:
#ifdef CONFIG_KERNEL_MCS
        بازگشت decodeSetSchedParams(cap, length, slot, buffer);
#وگرنه
        بازگشت decodeSetSchedParams(cap, length, buffer);
#endif

    حالت TCBSetIPCBuffer:
        بازگشت decodeSetIPCBuffer(cap, length, slot, buffer);

    حالت TCBSetSpace:
        بازگشت decodeSetSpace(cap, length, slot, buffer);

    حالت TCBBindNotification:
        بازگشت decodeBindNotification(cap);

    حالت TCBUnbindNotification:
        بازگشت decodeUnbindNotification(cap);

#ifdef CONFIG_KERNEL_MCS
    حالت TCBSetTimeoutEndpoint:
        بازگشت decodeSetTimeoutEndpoint(cap, slot);
#وگرنه
#ifdef ENABLE_SMP_SUPPORT
    حالت TCBSetAffinity:
        بازگشت decodeSetAffinity(cap, length, buffer);
#endif /* ENABLE_SMP_SUPPORT */
#endif

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ifdef CONFIG_VTX
    حالت TCBSetEPTRoot:
        بازگشت decodeSetEPTRoot(cap);
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    حالت TCBConfigureSingleStepping:
        بازگشت decodeConfigureSingleStepping(cap, call, buffer);

    حالت TCBSetBreakpoint:
        بازگشت decodeSetBreakpoint(cap, buffer);

    حالت TCBGetBreakpoint:
        بازگشت decodeGetBreakpoint(cap, call, buffer);

    حالت TCBUnsetBreakpoint:
        بازگشت decodeUnsetBreakpoint(cap, buffer);
#endif

    حالت TCBSetTLSBase:
        بازگشت decodeSetTLSBase(cap, length, buffer);

    حالت TCBSetFlags:
        بازگشت decodeSetFlags(cap, length, call, buffer);

    پیشفرض:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}

شمارش CopyRegistersFlags {
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

    اگر (length < 1 || current_extra_caps.excaprefs[0] == پوچ) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } وگرنه {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & BIT(CopyRegisters_suspendSource),
               flags & BIT(CopyRegisters_resumeTarget),
               flags & BIT(CopyRegisters_transferFrame),
               flags & BIT(CopyRegisters_transferInteger),
               transferArch);

}

شمارش ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, word_t length, bool_t call,
                                word_t *buffer)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    اگر (length < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    n     = getSyscallArg(1, buffer);

    اگر (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (صحیح)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    اگر (thread == گره_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

شمارش WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    اگر (length < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    w     = getSyscallArg(1, buffer);

    اگر (length - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (صحیح)(length - 2), (صحیح)w);
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    اگر (thread == گره_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, buffer);
}

#ifdef CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_endpoint_cap:
        بازگشت (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_capCanGrantReply(cap)));
    حالت cap_null_cap:
        /* just has no fault endpoint */
        بازگشت true;
    پیشفرض:
        بازگشت false;
    }
}
#endif

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cte_t *bufferSlot, *c_ریشه_Slot, *v_ریشه_Slot;
    cap_t bufferCap, c_ریشه_Cap, v_ریشه_Cap;
    deriveCap_ret_t dc_ret;
    word_t c_ریشه_Data, v_ریشه_Data, bufferAddr;
#ifdef CONFIG_KERNEL_MCS
#define TCBCONFIGURE_ARGS 3
#وگرنه
#define TCBCONFIGURE_ARGS 4
#endif
    اگر (length < TCBCONFIGURE_ARGS || current_extra_caps.excaprefs[0] == پوچ
        || current_extra_caps.excaprefs[1] == پوچ
        || current_extra_caps.excaprefs[2] == پوچ) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_ریشه_Data     = getSyscallArg(0, buffer);
    v_ریشه_Data     = getSyscallArg(1, buffer);
    bufferAddr    = getSyscallArg(2, buffer);
#وگرنه
    cptr_t faultEP       = getSyscallArg(0, buffer);
    c_ریشه_Data     = getSyscallArg(1, buffer);
    v_ریشه_Data     = getSyscallArg(2, buffer);
    bufferAddr    = getSyscallArg(3, buffer);
#endif

    c_ریشه_Slot  = current_extra_caps.excaprefs[0];
    c_ریشه_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ریشه_Slot  = current_extra_caps.excaprefs[1];
    v_ریشه_Cap   = current_extra_caps.excaprefs[1]->cap;
    bufferSlot = current_extra_caps.excaprefs[2];
    bufferCap  = current_extra_caps.excaprefs[2]->cap;

    اگر (bufferAddr == 0) {
        bufferSlot = پوچ;
    } وگرنه {
        dc_ret = deriveCap(bufferSlot, bufferCap);
        اگر (dc_ret.status != EXCEPTION_NONE) {
            بازگشت dc_ret.status;
        }
        bufferCap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(bufferAddr, bufferCap);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }
    }

    اگر (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (c_ریشه_Data != 0) {
        c_ریشه_Cap = updateCapData(false, c_ریشه_Data, c_ریشه_Cap);
    }

    dc_ret = deriveCap(c_ریشه_Slot, c_ریشه_Cap);
    اگر (dc_ret.status != EXCEPTION_NONE) {
        بازگشت dc_ret.status;
    }
    c_ریشه_Cap = dc_ret.cap;

    اگر (cap_get_capType(c_ریشه_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (v_ریشه_Data != 0) {
        v_ریشه_Cap = updateCapData(false, v_ریشه_Data, v_ریشه_Cap);
    }

    dc_ret = deriveCap(v_ریشه_Slot, v_ریشه_Cap);
    اگر (dc_ret.status != EXCEPTION_NONE) {
        بازگشت dc_ret.status;
    }
    v_ریشه_Cap = dc_ret.cap;

    اگر (!is_Valid_VTable_ریشه(v_ریشه_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    بازگشت invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), پوچ,
               cap_null_cap_new(), پوچ,
               c_ریشه_Cap, c_ریشه_Slot,
               v_ریشه_Cap, v_ریشه_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_buffer);
#وگرنه
    بازگشت invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_ریشه_Cap, c_ریشه_Slot,
               v_ریشه_Cap, v_ریشه_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_update_space |
               thread_control_update_ipc_buffer);
#endif
}

exception_t decodeSetPriority(cap_t cap, word_t length, word_t *buffer)
{
    اگر (length < 1 || current_extra_caps.excaprefs[0] == پوچ) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newPrio, authTCB);
    اگر (status != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        بازگشت status;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    بازگشت invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), پوچ,
               cap_null_cap_new(), پوچ,
               NULL_PRIO, newPrio,
               پوچ, thread_control_sched_update_priority);
#وگرنه
    بازگشت invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), پوچ,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), پوچ,
               cap_null_cap_new(), پوچ,
               0, cap_null_cap_new(),
               پوچ, thread_control_update_priority);
#endif
}

exception_t decodeSetMCPriority(cap_t cap, word_t length, word_t *buffer)
{
    اگر (length < 1 || current_extra_caps.excaprefs[0] == پوچ) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    اگر (status != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        بازگشت status;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    بازگشت invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), پوچ,
               cap_null_cap_new(), پوچ,
               newMcp, NULL_PRIO,
               پوچ, thread_control_sched_update_mcp);
#وگرنه
    بازگشت invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), پوچ,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), پوچ,
               cap_null_cap_new(), پوچ,
               0, cap_null_cap_new(),
               پوچ, thread_control_update_mcp);
#endif
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetTimeoutEndpoint(cap_t cap, cte_t *slot)
{
    اگر (current_extra_caps.excaprefs[0] == پوچ) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    اگر (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), پوچ,
               thCap, thSlot,
               cap_null_cap_new(), پوچ,
               cap_null_cap_new(), پوچ,
               0, cap_null_cap_new(), پوچ,
               thread_control_caps_update_timeout);
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetSchedParams(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
#وگرنه
exception_t decodeSetSchedParams(cap_t cap, word_t length, word_t *buffer)
#endif
{
    اگر (length < 2 || current_extra_caps.excaprefs[0] == پوچ
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == پوچ || current_extra_caps.excaprefs[2] == پوچ
#endif
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    prio_t newPrio = getSyscallArg(1, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ifdef CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#endif

    اگر (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    اگر (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        بازگشت status;
    }

    status = checkPrio(newPrio, authTCB);
    اگر (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        بازگشت status;
    }

#ifdef CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = پوچ;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_priority |
                                         thread_control_sched_update_fault;
    انتخاب (cap_get_capType(scCap)) {
    حالت cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        اگر (tcb->tcbSchedContext != sc) {
            اگر (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                بازگشت EXCEPTION_SYSCALL_ERROR;
            }
            اگر (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                بازگشت EXCEPTION_SYSCALL_ERROR;
            }
        }
        اگر (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        بشکن;
    حالت cap_null_cap:
        اگر (tcb == گره_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        بشکن;
    پیشفرض:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 2;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    اگر (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    اگر (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 3;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
#endif
    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    بازگشت invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#وگرنه
    بازگشت invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), پوچ,
               0, newMcp, newPrio,
               cap_null_cap_new(), پوچ,
               cap_null_cap_new(), پوچ,
               0, cap_null_cap_new(),
               پوچ, thread_control_update_mcp |
               thread_control_update_priority);
#endif
}


exception_t decodeSetIPCBuffer(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cptr_t cptr_bufferPtr;
    cap_t bufferCap;
    cte_t *bufferSlot;

    اگر (length < 1 || current_extra_caps.excaprefs[0] == پوچ) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    cptr_bufferPtr  = getSyscallArg(0, buffer);
    bufferSlot = current_extra_caps.excaprefs[0];
    bufferCap  = current_extra_caps.excaprefs[0]->cap;

    اگر (cptr_bufferPtr == 0) {
        bufferSlot = پوچ;
    } وگرنه {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(bufferSlot, bufferCap);
        اگر (dc_ret.status != EXCEPTION_NONE) {
            بازگشت dc_ret.status;
        }
        bufferCap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_bufferPtr, bufferCap);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    بازگشت invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), پوچ,
               cap_null_cap_new(), پوچ,
               cap_null_cap_new(), پوچ,
               cap_null_cap_new(), پوچ,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_caps_update_ipc_buffer);
#وگرنه
    بازگشت invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), پوچ,
               cap_null_cap_new(), پوچ,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_update_ipc_buffer);

#endif
}

#ifdef CONFIG_KERNEL_MCS
#define DECODE_SET_SPACE_PARAMS 2
#وگرنه
#define DECODE_SET_SPACE_PARAMS 3
#endif
exception_t decodeSetSpace(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    word_t c_ریشه_Data, v_ریشه_Data;
    cte_t *c_ریشه_Slot, *v_ریشه_Slot;
    cap_t c_ریشه_Cap, v_ریشه_Cap;
    deriveCap_ret_t dc_ret;

    اگر (length < DECODE_SET_SPACE_PARAMS || current_extra_caps.excaprefs[0] == پوچ
        || current_extra_caps.excaprefs[1] == پوچ
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == پوچ
#endif
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_ریشه_Data = getSyscallArg(0, buffer);
    v_ریشه_Data = getSyscallArg(1, buffer);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_ریشه_Slot  = current_extra_caps.excaprefs[1];
    c_ریشه_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_ریشه_Slot  = current_extra_caps.excaprefs[2];
    v_ریشه_Cap   = current_extra_caps.excaprefs[2]->cap;
#وگرنه
    cptr_t faultEP   = getSyscallArg(0, buffer);
    c_ریشه_Data = getSyscallArg(1, buffer);
    v_ریشه_Data = getSyscallArg(2, buffer);

    c_ریشه_Slot  = current_extra_caps.excaprefs[0];
    c_ریشه_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ریشه_Slot  = current_extra_caps.excaprefs[1];
    v_ریشه_Cap   = current_extra_caps.excaprefs[1]->cap;
#endif

    اگر (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (c_ریشه_Data != 0) {
        c_ریشه_Cap = updateCapData(false, c_ریشه_Data, c_ریشه_Cap);
    }

    dc_ret = deriveCap(c_ریشه_Slot, c_ریشه_Cap);
    اگر (dc_ret.status != EXCEPTION_NONE) {
        بازگشت dc_ret.status;
    }
    c_ریشه_Cap = dc_ret.cap;

    اگر (cap_get_capType(c_ریشه_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (v_ریشه_Data != 0) {
        v_ریشه_Cap = updateCapData(false, v_ریشه_Data, v_ریشه_Cap);
    }

    dc_ret = deriveCap(v_ریشه_Slot, v_ریشه_Cap);
    اگر (dc_ret.status != EXCEPTION_NONE) {
        بازگشت dc_ret.status;
    }
    v_ریشه_Cap = dc_ret.cap;

    اگر (!is_Valid_VTable_ریشه(v_ریشه_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    /* fault handler */
    اگر (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
#endif

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    بازگشت invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), پوچ,
               c_ریشه_Cap, c_ریشه_Slot,
               v_ریشه_Cap, v_ریشه_Slot,
               0, cap_null_cap_new(), پوچ, thread_control_caps_update_space | thread_control_caps_update_fault);
#وگرنه
    بازگشت invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_ریشه_Cap, c_ریشه_Slot,
               v_ریشه_Cap, v_ریشه_Slot,
               0, cap_null_cap_new(), پوچ, thread_control_update_space);
#endif
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    اگر (current_extra_caps.excaprefs[0] == پوچ) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    اگر (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } وگرنه {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر ((tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }


    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    اگر (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeTCB_NotificationControl(tcb, پوچ);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    بازگشت EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    بازگشت EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ایستا درونخطی exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_index_t index, cap_t newCap, cte_t *srcSlot)
{
    cte_t *ریشه_Slot = TCB_PTR_CTE_PTR(target, index);
    UNUSED exception_t e = cteDelete(ریشه_Slot, true);
    اگر (e != EXCEPTION_NONE) {
        بازگشت e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    اگر (cap_get_capType(newCap) != cap_null_cap) {
        اگر (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_درج(newCap, srcSlot, ریشه_Slot);
        }
    }
    بازگشت e;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t c_ریشه_new_Cap, cte_t *c_ریشه_src_Slot,
                                        cap_t v_ریشه_new_Cap, cte_t *v_ریشه_src_Slot,
                                        word_t bufferAddr, cap_t bufferCap,
                                        cte_t *bufferSrcSlot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    اگر (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }

    }

    اگر (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }
    }

    اگر (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, c_ریشه_new_Cap, c_ریشه_src_Slot);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, v_ریشه_new_Cap, v_ریشه_src_Slot);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }
    }

    اگر (updateFlags & thread_control_caps_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }
        target->tcbIPCBuffer = bufferAddr;

        اگر (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_درج(bufferCap, bufferSrcSlot, bufferSlot);
        }

        اگر (target == گره_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    بازگشت EXCEPTION_NONE;
}
#وگرنه
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t priority,
                                    cap_t c_ریشه_new_Cap, cte_t *c_ریشه_src_Slot,
                                    cap_t v_ریشه_new_Cap, cte_t *v_ریشه_src_Slot,
                                    word_t bufferAddr, cap_t bufferCap,
                                    cte_t *bufferSrcSlot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    اگر (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    اگر (updateFlags & thread_control_update_mcp) {
        setMCPriority(target, mcp);
    }

    اگر (updateFlags & thread_control_update_space) {
        cte_t *ریشه_Slot;

        ریشه_Slot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(ریشه_Slot, true);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }
        اگر (sameObjectAs(c_ریشه_new_Cap, c_ریشه_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_درج(c_ریشه_new_Cap, c_ریشه_src_Slot, ریشه_Slot);
        }
    }

    اگر (updateFlags & thread_control_update_space) {
        cte_t *ریشه_Slot;

        ریشه_Slot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(ریشه_Slot, true);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }
        اگر (sameObjectAs(v_ریشه_new_Cap, v_ریشه_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_درج(v_ریشه_new_Cap, v_ریشه_src_Slot, ریشه_Slot);
        }
    }

    اگر (updateFlags & thread_control_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }
        target->tcbIPCBuffer = bufferAddr;

        اگر (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_درج(bufferCap, bufferSrcSlot, bufferSlot);
        }

        اگر (target == گره_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    اگر (updateFlags & thread_control_update_priority) {
        setPriority(target, priority);
    }

    بازگشت EXCEPTION_NONE;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t priority,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    اگر (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        اگر (e != EXCEPTION_NONE) {
            بازگشت e;
        }
    }

    اگر (updateFlags & thread_control_sched_update_mcp) {
        setMCPriority(target, mcp);
    }

    اگر (updateFlags & thread_control_sched_update_priority) {
        setPriority(target, priority);
    }

    اگر (updateFlags & thread_control_sched_update_sc) {
        اگر (sc != پوچ) {
            schedContext_bindTCB(sc, target);
        } وگرنه اگر (sc == پوچ) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    بازگشت EXCEPTION_NONE;
}
#endif

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    اگر (suspendSource) {
        suspend(tcb_src);
    }

    اگر (resumeTarget) {
        restart(dest);
    }

    اگر (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        برای (i = 0; i < n_frameRegisters; i++) {
            v = getRegister(tcb_src, frameRegisters[i]);
            setRegister(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        setNextPC(dest, pc);
    }

    اگر (transferInteger) {
        word_t i;
        word_t v;

        برای (i = 0; i < n_gpRegisters; i++) {
            v = getRegister(tcb_src, gpRegisters[i]);
            setRegister(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    اگر (dest == گره_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    بازگشت Arch_performTransfer(transferArch, tcb_src, dest);
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

    thread = گره_STATE(ksCurThread);

    اگر (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, گره_STATE(ksCurThread));
    اگر (e != EXCEPTION_NONE) {
        بازگشت e;
    }

    اگر (call) {
        word_t *ipcBuffer;

        ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);

        برای (i = 0; i < n && i < n_frameRegisters && i < n_پیام_Registers; i++) {
            setRegister(thread, پیام_Registers[i],
                        getRegister(tcb_src, frameRegisters[i]));
        }

        اگر (ipcBuffer != پوچ && i < n && i < n_frameRegisters) {
            برای (; i < n && i < n_frameRegisters; i++) {
                ipcBuffer[i + 1] = getRegister(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        برای (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_پیام_Registers; i++) {
            setRegister(thread, پیام_Registers[i + n_frameRegisters],
                        getRegister(tcb_src, gpRegisters[i]));
        }

        اگر (ipcBuffer != پوچ && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            برای (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipcBuffer[i + n_frameRegisters + 1] =
                    getRegister(tcb_src, gpRegisters[i]);
            }
        }

        setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, i + j)));
    }
    setThreadState(thread, ThreadState_Running);

    بازگشت EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *buffer)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, گره_STATE(ksCurThread), dest);
    اگر (e != EXCEPTION_NONE) {
        بازگشت e;
    }

    اگر (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_getSanitiseRegisterInfo(dest);

    برای (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        setRegister(dest, frameRegisters[i],
                    sanitiseRegister(frameRegisters[i],
                                     getSyscallArg(i + 2, buffer), archInfo));
    }

    برای (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        setRegister(dest, gpRegisters[i],
                    sanitiseRegister(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   buffer), archInfo));
    }

    pc = getRestartPC(dest);
    setNextPC(dest, pc);

    Arch_postModifyRegisters(dest);

    اگر (resumeTarget) {
        restart(dest);
    }

    اگر (dest == گره_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    بازگشت EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    اگر (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } وگرنه {
        unbindNotification(tcb);
    }

    بازگشت EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD
تهی setThreadName(tcb_t *tcb, ثابت نویسه *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_LENGTH);
}
#endif

word_t setMRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    انتخاب (current_syscall_error.type) {
    حالت seL4_InvalidArgument:
        بازگشت setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Argument_عدد);

    حالت seL4_InvalidCapability:
        بازگشت setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_عدد);

    حالت seL4_IllegalOperation:
        بازگشت 0;

    حالت seL4_RangeError:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        بازگشت setMR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    حالت seL4_AlignmentError:
        بازگشت 0;

    حالت seL4_FailedLookup:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        بازگشت setMRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    حالت seL4_TruncatedMessage:
    حالت seL4_DeleteFirst:
    حالت seL4_RevokeFirst:
        بازگشت 0;
    حالت seL4_NotEnoughMemory:
        بازگشت setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.memory_چپ);
    پیشفرض:
        fail("Invalid syscall error");
    }
}
