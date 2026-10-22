/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <api/invocation.h>
#ضمن <api/syscall.h>
#ضمن <sel4/shared_types.h>
#ضمن <machine/io.h>
#ضمن <object/structures.h>
#ضمن <object/objecttype.h>
#ضمن <object/cnode.h>
#ifdef CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#endif
#ضمن <object/tcb.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <model/statedata.h>
#ضمن <util.h>
#ضمن <string.h>
#ضمن <stdint.h>
#ضمن <arch/smp/ipi_inline.h>

#define NULL_PRIO 0

ساكن exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    أكد(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    إذا (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

ساكن مضمن فراغ addToBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    عقدة_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    عقدة_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & MASK(wordRadix));
}

ساكن مضمن فراغ removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    عقدة_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & MASK(wordRadix));
    إذا (unlikely(!عقدة_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        عقدة_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcbSchedNext;

    إذا (queue.head == tcb && queue.end == tcb) {
        queue.head = عدم;
        queue.end = عدم;
    } وإلا {
        إذا (queue.head == tcb) {
            after->tcbSchedPrev = عدم;
            tcb->tcbSchedNext = عدم;
            queue.head = after;
        } وإلا {
            إذا (queue.end == tcb) {
                before->tcbSchedNext = عدم;
                tcb->tcbSchedPrev = عدم;
                queue.end = before;
            } وإلا {
                before->tcbSchedNext = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = عدم;
                tcb->tcbSchedNext = عدم;
            }
        }
    }

    أرجع queue;
}

/* Add TCB to the head of a scheduler queue */
فراغ tcbSchedEnqueue(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
#endif

    إذا (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = عقدة_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        إذا (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        عقدة_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
فراغ tcbSchedAppend(tcb_t *tcb)
{
#ifdef CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
    أكد(refill_ready(tcb->tcbSchedContext));
#endif
    إذا (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = عقدة_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        إذا (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        عقدة_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
فراغ tcbSchedDequeue(tcb_t *tcb)
{
    إذا (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_index(dom, prio);
        queue = عقدة_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        عقدة_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, false);

        إذا (likely(tcb_queue_empty(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#ifdef CONFIG_DEBUG_BUILD
فراغ tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = عدم;

    debug_tcb->tcbDebugNext = عقدة_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    إذا (عقدة_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(عقدة_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    عقدة_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

فراغ tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    أكد(عقدة_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != عدم);
    إذا (tcb == عقدة_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        عقدة_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(عقدة_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcbDebugNext;
    } وإلا {
        أكد(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcbDebugNext = debug_tcb->tcbDebugNext;
    }

    إذا (debug_tcb->tcbDebugNext) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugNext)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = عدم;
    debug_tcb->tcbDebugNext = عدم;
}
#endif /* CONFIG_DEBUG_BUILD */

#ifndef CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    إذا (!queue.head) { /* Empty list */
        queue.head = tcb;
    } وإلا {
        queue.end->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.end;
    tcb->tcbEPNext = عدم;
    queue.end = tcb;

    أرجع queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    إذا (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } وإلا {
        queue.head = tcb->tcbEPNext;
    }

    إذا (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } وإلا {
        queue.end = tcb->tcbEPPrev;
    }

    أرجع queue;
}
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS

فراغ tcbReleaseRemove(tcb_t *tcb)
{
    إذا (likely(thread_state_get_tcbInReleaseQueue(tcb->tcbState))) {
        tcb_queue_t queue = عقدة_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        إذا (queue.head == tcb) {
            عقدة_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        عقدة_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, false);
    }
}

ساكن مضمن ticks_t PURE tcbReadyTime(tcb_t *tcb)
{
    أرجع refill_head(tcb->tcbSchedContext)->rTime;
}

ساكن مضمن bool_t PURE time_after(tcb_t *tcb, ticks_t new_time)
{
    أرجع tcb != عدم && new_time >= tcbReadyTime(tcb);
}

ساكن tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    طالما (time_after(after, new_time)) {
        after = after->tcbSchedNext;
    }

    أرجع after;
}

فراغ tcbReleaseEnqueue(tcb_t *tcb)
{
    أكد(thread_state_get_tcbInReleaseQueue(tcb->tcbState) == false);
    أكد(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = عقدة_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    إذا (tcb_queue_empty(queue) || new_time < tcbReadyTime(queue.head)) {
        عقدة_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } وإلا {
        إذا (tcbReadyTime(queue.end) <= new_time) {
            عقدة_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } وإلا {
            tcb_t *after;
            after = find_time_after(queue.head, new_time);
            tcb_queue_أدرج(tcb, after);
        }
    }

    thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, true);

    إذا (queue.head != عقدة_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).head) {
        عقدة_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#endif

cptr_t PURE getExtraCPtr(word_t *bufferPtr, word_t i)
{
    أرجع (cptr_t)bufferPtr[se_L4_رسالة_Max_Length + 2 + i];
}

فراغ setExtraBadge(word_t *bufferPtr, word_t badge,
                   word_t i)
{
    bufferPtr[se_L4_رسالة_Max_Length + 2 + i] = badge;
}

#ifndef CONFIG_KERNEL_MCS
فراغ setupCallerCap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
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
    cte_أدرج(cap_reply_cap_new(canGrant, false, TCB_REF(sender)),
              replySlot, callerSlot);
}

فراغ deleteCallerCap(tcb_t *receiver)
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

    إذا (!bufferPtr) {
        current_extra_caps.excaprefs[0] = عدم;
        أرجع EXCEPTION_NONE;
    }

    length = seL4_MessageInfo_get_extraCaps(info);

    لكل (i = 0; i < length; i++) {
        cptr = getExtraCPtr(bufferPtr, i);

        lu_ret = lookupSlot(thread, cptr);
        إذا (lu_ret.status != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            أرجع lu_ret.status;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    إذا (i < se_L4_رسالة_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = عدم;
    }

    أرجع EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    لكل (i = 0; i < n && i < n_رسالة_Registers; i++) {
        setRegister(receiver, رسالة_Registers[i],
                    getRegister(sender, رسالة_Registers[i]));
    }

    إذا (!recvBuf || !sendBuf) {
        أرجع i;
    }

    /* Copy out-of-line words */
    لكل (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    أرجع i;
}

#ifdef ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
فراغ remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    إذا (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = عقدة_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        إذا (targetCurThread == عقدة_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcbPriority > targetCurThread->tcbPriority
#ifdef CONFIG_KERNEL_MCS
            || عقدة_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#endif
           ) {
            ARCH_عقدة_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
فراغ remoteTCBStall(tcb_t *tcb)
{

    إذا (
#ifdef CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#endif
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        عقدة_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_عقدة_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#ifndef CONFIG_KERNEL_MCS
ساكن exception_t invokeTCB_SetAffinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcbSchedDequeue(thread);
    migrateTCB(thread, affinity);
    إذا (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    إذا (thread == عقدة_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeSetAffinity(cap_t cap, word_t length, word_t *buffer)
{
    tcb_t *tcb;
    word_t affinity;

    إذا (length < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, buffer);
    إذا (affinity >= ks_رقم_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_SetAffinity(tcb, affinity);
}
#endif
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_HARDWARE_DEBUG_API
ساكن exception_t invokeConfigureSingleStepping(bool_t call, word_t *buffer, tcb_t *t,
                                                 uint16_t bp_رقم, word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = عقدة_STATE(ksCurThread);
    word_t قيمة;

    bp_was_consumed = configureSingleStepping(t, bp_رقم, n_instrs, false);
    إذا (n_instrs == 0) {
#ifndef CONFIG_ARCH_AARCH64
        unsetBreakpointUsedFlag(t, bp_رقم);
#endif /* CONFIG_ARCH_AARCH64 */
        قيمة = false;
    } وإلا {
#ifndef CONFIG_ARCH_AARCH64
        setBreakpointUsedFlag(t, bp_رقم);
#endif /* CONFIG_ARCH_AARCH64 */
        قيمة = bp_was_consumed;
    }

    إذا (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned صحيح length = setMR(thread, buffer, 0, قيمة);
        setRegister(thread, رسالة_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *buffer)
{
    uint16_t bp_رقم;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_رقم = getSyscallArg(0, buffer);
    n_instrs = getSyscallArg(1, buffer);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_رقم, n_instrs, false);
    إذا (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeConfigureSingleStepping(call, buffer, tcb, bp_رقم, n_instrs);
}

ساكن exception_t invokeSetBreakpoint(tcb_t *tcb, uint16_t bp_رقم,
                                       word_t vaddr, word_t type, word_t size, word_t rw)
{
    setBreakpoint(tcb, bp_رقم, vaddr, type, size, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    setBreakpointUsedFlag(tcb, bp_رقم);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeSetBreakpoint(cap_t cap, word_t *buffer)
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
    إذا (vaddr >= (word_t)USER_قمة) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (type != seL4_InstructionBreakpoint && type != seL4_DataBreakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_رقم = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    } وإلا إذا (type == seL4_InstructionBreakpoint) {
        إذا (size != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_رقم = 3;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_رقم = 4;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا ((seL4_FirstWatchpoint == -1 || bp_رقم >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_رقم = 2;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    } وإلا إذا (type == seL4_DataBreakpoint) {
        إذا (size == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_رقم = 3;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (seL4_FirstWatchpoint != -1 && bp_رقم < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_رقم = 2;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    } وإلا إذا (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_رقم = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_رقم = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (size != 0 && size != 1 && size != 2 && size != 4 && size != 8) {
        userError("Debug: Invalid size %lu.", size);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_رقم = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (size > 0 && vaddr & (size - 1)) {
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
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decodeSetBreakpoint(tcb, bp_رقم, vaddr, type, size, rw);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSetBreakpoint(tcb, bp_رقم,
                               vaddr, type, size, rw);
}

ساكن exception_t invokeGetBreakpoint(bool_t call, word_t *buffer, tcb_t *tcb, uint16_t bp_رقم)
{
    tcb_t *thread;
    thread = عقدة_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_رقم);
    إذا (call) {
        setRegister(thread, badgeRegister, 0);
        setMR(عقدة_STATE(ksCurThread), buffer, 0, res.vaddr);
        setMR(عقدة_STATE(ksCurThread), buffer, 1, res.type);
        setMR(عقدة_STATE(ksCurThread), buffer, 2, res.size);
        setMR(عقدة_STATE(ksCurThread), buffer, 3, res.rw);
        setMR(عقدة_STATE(ksCurThread), buffer, 4, res.is_enabled);
        setRegister(thread, رسالة_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, 5)));
    }
    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_رقم;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_رقم = getSyscallArg(0, buffer);

    error = Arch_decodeGetBreakpoint(tcb, bp_رقم);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeGetBreakpoint(call, buffer, tcb, bp_رقم);
}

ساكن exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_رقم)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_رقم);
    unsetBreakpointUsedFlag(tcb, bp_رقم);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeUnsetBreakpoint(cap_t cap, word_t *buffer)
{
    tcb_t *tcb;
    uint16_t bp_رقم;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_رقم = getSyscallArg(0, buffer);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_رقم);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeUnsetBreakpoint(tcb, bp_رقم);
}
#endif /* CONFIG_HARDWARE_DEBUG_API */

ساكن exception_t invokeSetTLSBase(tcb_t *thread, word_t tls_base)
{
    setRegister(thread, TLS_BASE, tls_base);
    إذا (thread == عقدة_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeSetTLSBase(cap_t cap, word_t length, word_t *buffer)
{
    word_t tls_base;

    إذا (length < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tls_base = getSyscallArg(0, buffer);

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSetTLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_base);
}

ساكن فراغ invokeSetFlags(tcb_t *thread, word_t clear, word_t set, bool_t call)
{
    tcb_t *cur_thread = عقدة_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= set & seL4_TCBFlag_MASK;
    thread->tcbFlags = flags;

#ifdef CONFIG_HAVE_FPU
    إذا (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } وإلا إذا (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#endif
    إذا (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, cur_thread);
        setRegister(cur_thread, badgeRegister, 0);
        unsigned صحيح length = setMR(cur_thread, ipcBuffer, 0, flags);
        setRegister(cur_thread, رسالة_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(cur_thread, ThreadState_Running);
}

ساكن exception_t decodeSetFlags(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    إذا (length < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, buffer);
    word_t set   = getSyscallArg(1, buffer);

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    invokeSetFlags(thread, clear, set, call);
    أرجع EXCEPTION_NONE;
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

    بدل (invLabel) {
    حالة TCBReadRegisters:
        /* Second level of decoding */
        أرجع decodeReadRegisters(cap, length, call, buffer);

    حالة TCBWriteRegisters:
        أرجع decodeWriteRegisters(cap, length, buffer);

    حالة TCBCopyRegisters:
        أرجع decodeCopyRegisters(cap, length, buffer);

    حالة TCBSuspend:
        /* Jump straight to the invoke */
        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالة TCBResume:
        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالة TCBConfigure:
        أرجع decodeTCBConfigure(cap, length, slot, buffer);

    حالة TCBSetPriority:
        أرجع decodeSetPriority(cap, length, buffer);

    حالة TCBSetMCPriority:
        أرجع decodeSetMCPriority(cap, length, buffer);

    حالة TCBSetSchedParams:
#ifdef CONFIG_KERNEL_MCS
        أرجع decodeSetSchedParams(cap, length, slot, buffer);
#وإلا
        أرجع decodeSetSchedParams(cap, length, buffer);
#endif

    حالة TCBSetIPCBuffer:
        أرجع decodeSetIPCBuffer(cap, length, slot, buffer);

    حالة TCBSetSpace:
        أرجع decodeSetSpace(cap, length, slot, buffer);

    حالة TCBBindNotification:
        أرجع decodeBindNotification(cap);

    حالة TCBUnbindNotification:
        أرجع decodeUnbindNotification(cap);

#ifdef CONFIG_KERNEL_MCS
    حالة TCBSetTimeoutEndpoint:
        أرجع decodeSetTimeoutEndpoint(cap, slot);
#وإلا
#ifdef ENABLE_SMP_SUPPORT
    حالة TCBSetAffinity:
        أرجع decodeSetAffinity(cap, length, buffer);
#endif /* ENABLE_SMP_SUPPORT */
#endif

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#ifdef CONFIG_VTX
    حالة TCBSetEPTRoot:
        أرجع decodeSetEPTRoot(cap);
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    حالة TCBConfigureSingleStepping:
        أرجع decodeConfigureSingleStepping(cap, call, buffer);

    حالة TCBSetBreakpoint:
        أرجع decodeSetBreakpoint(cap, buffer);

    حالة TCBGetBreakpoint:
        أرجع decodeGetBreakpoint(cap, call, buffer);

    حالة TCBUnsetBreakpoint:
        أرجع decodeUnsetBreakpoint(cap, buffer);
#endif

    حالة TCBSetTLSBase:
        أرجع decodeSetTLSBase(cap, length, buffer);

    حالة TCBSetFlags:
        أرجع decodeSetFlags(cap, length, call, buffer);

    افتراضي:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

تعداد CopyRegistersFlags {
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

    إذا (length < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } وإلا {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & BIT(CopyRegisters_suspendSource),
               flags & BIT(CopyRegisters_resumeTarget),
               flags & BIT(CopyRegisters_transferFrame),
               flags & BIT(CopyRegisters_transferInteger),
               transferArch);

}

تعداد ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, word_t length, bool_t call,
                                word_t *buffer)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    إذا (length < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    n     = getSyscallArg(1, buffer);

    إذا (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (صحيح)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    إذا (thread == عقدة_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

تعداد WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    إذا (length < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, buffer);
    w     = getSyscallArg(1, buffer);

    إذا (length - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (صحيح)(length - 2), (صحيح)w);
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    إذا (thread == عقدة_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, buffer);
}

#ifdef CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap:
        أرجع (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_capCanGrantReply(cap)));
    حالة cap_null_cap:
        /* just has no fault endpoint */
        أرجع true;
    افتراضي:
        أرجع false;
    }
}
#endif

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cte_t *bufferSlot, *c_جذر_Slot, *v_جذر_Slot;
    cap_t bufferCap, c_جذر_Cap, v_جذر_Cap;
    deriveCap_ret_t dc_ret;
    word_t c_جذر_Data, v_جذر_Data, bufferAddr;
#ifdef CONFIG_KERNEL_MCS
#define TCBCONFIGURE_وسائط 3
#وإلا
#define TCBCONFIGURE_وسائط 4
#endif
    إذا (length < TCBCONFIGURE_وسائط || current_extra_caps.excaprefs[0] == عدم
        || current_extra_caps.excaprefs[1] == عدم
        || current_extra_caps.excaprefs[2] == عدم) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_جذر_Data     = getSyscallArg(0, buffer);
    v_جذر_Data     = getSyscallArg(1, buffer);
    bufferAddr    = getSyscallArg(2, buffer);
#وإلا
    cptr_t faultEP       = getSyscallArg(0, buffer);
    c_جذر_Data     = getSyscallArg(1, buffer);
    v_جذر_Data     = getSyscallArg(2, buffer);
    bufferAddr    = getSyscallArg(3, buffer);
#endif

    c_جذر_Slot  = current_extra_caps.excaprefs[0];
    c_جذر_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_جذر_Slot  = current_extra_caps.excaprefs[1];
    v_جذر_Cap   = current_extra_caps.excaprefs[1]->cap;
    bufferSlot = current_extra_caps.excaprefs[2];
    bufferCap  = current_extra_caps.excaprefs[2]->cap;

    إذا (bufferAddr == 0) {
        bufferSlot = عدم;
    } وإلا {
        dc_ret = deriveCap(bufferSlot, bufferCap);
        إذا (dc_ret.status != EXCEPTION_NONE) {
            أرجع dc_ret.status;
        }
        bufferCap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(bufferAddr, bufferCap);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (c_جذر_Data != 0) {
        c_جذر_Cap = updateCapData(false, c_جذر_Data, c_جذر_Cap);
    }

    dc_ret = deriveCap(c_جذر_Slot, c_جذر_Cap);
    إذا (dc_ret.status != EXCEPTION_NONE) {
        أرجع dc_ret.status;
    }
    c_جذر_Cap = dc_ret.cap;

    إذا (cap_get_capType(c_جذر_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (v_جذر_Data != 0) {
        v_جذر_Cap = updateCapData(false, v_جذر_Data, v_جذر_Cap);
    }

    dc_ret = deriveCap(v_جذر_Slot, v_جذر_Cap);
    إذا (dc_ret.status != EXCEPTION_NONE) {
        أرجع dc_ret.status;
    }
    v_جذر_Cap = dc_ret.cap;

    إذا (!is_Valid_VTable_جذر(v_جذر_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               c_جذر_Cap, c_جذر_Slot,
               v_جذر_Cap, v_جذر_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_buffer);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_جذر_Cap, c_جذر_Slot,
               v_جذر_Cap, v_جذر_Slot,
               bufferAddr, bufferCap,
               bufferSlot, thread_control_update_space |
               thread_control_update_ipc_buffer);
#endif
}

exception_t decodeSetPriority(cap_t cap, word_t length, word_t *buffer)
{
    إذا (length < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newPrio, authTCB);
    إذا (status != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        أرجع status;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               cap_null_cap_new(), عدم,
               NULL_PRIO, newPrio,
               عدم, thread_control_sched_update_priority);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(),
               عدم, thread_control_update_priority);
#endif
}

exception_t decodeSetMCPriority(cap_t cap, word_t length, word_t *buffer)
{
    إذا (length < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    إذا (status != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        أرجع status;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               cap_null_cap_new(), عدم,
               newMcp, NULL_PRIO,
               عدم, thread_control_sched_update_mcp);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(),
               عدم, thread_control_update_mcp);
#endif
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetTimeoutEndpoint(cap_t cap, cte_t *slot)
{
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    إذا (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), عدم,
               thCap, thSlot,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(), عدم,
               thread_control_caps_update_timeout);
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t decodeSetSchedParams(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
#وإلا
exception_t decodeSetSchedParams(cap_t cap, word_t length, word_t *buffer)
#endif
{
    إذا (length < 2 || current_extra_caps.excaprefs[0] == عدم
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == عدم || current_extra_caps.excaprefs[2] == عدم
#endif
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, buffer);
    prio_t newPrio = getSyscallArg(1, buffer);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#ifdef CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#endif

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    إذا (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (unsigned long) newMcp, (unsigned long) authTCB->tcbMCP);
        أرجع status;
    }

    status = checkPrio(newPrio, authTCB);
    إذا (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (unsigned long) newPrio, (unsigned long) authTCB->tcbMCP);
        أرجع status;
    }

#ifdef CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = عدم;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_priority |
                                         thread_control_sched_update_fault;
    بدل (cap_get_capType(scCap)) {
    حالة cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        إذا (tcb->tcbSchedContext != sc) {
            إذا (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
            إذا (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }
        إذا (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        اكسر;
    حالة cap_null_cap:
        إذا (tcb == عقدة_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        اكسر;
    افتراضي:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_رقم = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    إذا (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    إذا (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_رقم = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#endif
    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               0, newMcp, newPrio,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(),
               عدم, thread_control_update_mcp |
               thread_control_update_priority);
#endif
}


exception_t decodeSetIPCBuffer(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    cptr_t cptr_bufferPtr;
    cap_t bufferCap;
    cte_t *bufferSlot;

    إذا (length < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cptr_bufferPtr  = getSyscallArg(0, buffer);
    bufferSlot = current_extra_caps.excaprefs[0];
    bufferCap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cptr_bufferPtr == 0) {
        bufferSlot = عدم;
    } وإلا {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(bufferSlot, bufferCap);
        إذا (dc_ret.status != EXCEPTION_NONE) {
            أرجع dc_ret.status;
        }
        bufferCap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_bufferPtr, bufferCap);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_caps_update_ipc_buffer);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cptr_bufferPtr, bufferCap,
               bufferSlot, thread_control_update_ipc_buffer);

#endif
}

#ifdef CONFIG_KERNEL_MCS
#define DECODE_SET_SPACE_PARAMS 2
#وإلا
#define DECODE_SET_SPACE_PARAMS 3
#endif
exception_t decodeSetSpace(cap_t cap, word_t length, cte_t *slot, word_t *buffer)
{
    word_t c_جذر_Data, v_جذر_Data;
    cte_t *c_جذر_Slot, *v_جذر_Slot;
    cap_t c_جذر_Cap, v_جذر_Cap;
    deriveCap_ret_t dc_ret;

    إذا (length < DECODE_SET_SPACE_PARAMS || current_extra_caps.excaprefs[0] == عدم
        || current_extra_caps.excaprefs[1] == عدم
#ifdef CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == عدم
#endif
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    c_جذر_Data = getSyscallArg(0, buffer);
    v_جذر_Data = getSyscallArg(1, buffer);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_جذر_Slot  = current_extra_caps.excaprefs[1];
    c_جذر_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_جذر_Slot  = current_extra_caps.excaprefs[2];
    v_جذر_Cap   = current_extra_caps.excaprefs[2]->cap;
#وإلا
    cptr_t faultEP   = getSyscallArg(0, buffer);
    c_جذر_Data = getSyscallArg(1, buffer);
    v_جذر_Data = getSyscallArg(2, buffer);

    c_جذر_Slot  = current_extra_caps.excaprefs[0];
    c_جذر_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_جذر_Slot  = current_extra_caps.excaprefs[1];
    v_جذر_Cap   = current_extra_caps.excaprefs[1]->cap;
#endif

    إذا (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (c_جذر_Data != 0) {
        c_جذر_Cap = updateCapData(false, c_جذر_Data, c_جذر_Cap);
    }

    dc_ret = deriveCap(c_جذر_Slot, c_جذر_Cap);
    إذا (dc_ret.status != EXCEPTION_NONE) {
        أرجع dc_ret.status;
    }
    c_جذر_Cap = dc_ret.cap;

    إذا (cap_get_capType(c_جذر_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (v_جذر_Data != 0) {
        v_جذر_Cap = updateCapData(false, v_جذر_Data, v_جذر_Cap);
    }

    dc_ret = deriveCap(v_جذر_Slot, v_جذر_Cap);
    إذا (dc_ret.status != EXCEPTION_NONE) {
        أرجع dc_ret.status;
    }
    v_جذر_Cap = dc_ret.cap;

    إذا (!is_Valid_VTable_جذر(v_جذر_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    /* fault handler */
    إذا (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#endif

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), عدم,
               c_جذر_Cap, c_جذر_Slot,
               v_جذر_Cap, v_جذر_Slot,
               0, cap_null_cap_new(), عدم, thread_control_caps_update_space | thread_control_caps_update_fault);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_جذر_Cap, c_جذر_Slot,
               v_جذر_Cap, v_جذر_Slot,
               0, cap_null_cap_new(), عدم, thread_control_update_space);
#endif
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    إذا (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } وإلا {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا ((tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }


    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    إذا (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_NotificationControl(tcb, عدم);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    أرجع EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    أرجع EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
ساكن مضمن exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_index_t index, cap_t newCap, cte_t *srcSlot)
{
    cte_t *جذر_Slot = TCB_PTR_CTE_PTR(target, index);
    UNUSED exception_t e = cteDelete(جذر_Slot, true);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    إذا (cap_get_capType(newCap) != cap_null_cap) {
        إذا (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_أدرج(newCap, srcSlot, جذر_Slot);
        }
    }
    أرجع e;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t c_جذر_new_Cap, cte_t *c_جذر_src_Slot,
                                        cap_t v_جذر_new_Cap, cte_t *v_جذر_src_Slot,
                                        word_t bufferAddr, cap_t bufferCap,
                                        cte_t *bufferSrcSlot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    إذا (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }

    }

    إذا (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, c_جذر_new_Cap, c_جذر_src_Slot);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, v_جذر_new_Cap, v_جذر_src_Slot);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_caps_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        target->tcbIPCBuffer = bufferAddr;

        إذا (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_أدرج(bufferCap, bufferSrcSlot, bufferSlot);
        }

        إذا (target == عقدة_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t priority,
                                    cap_t c_جذر_new_Cap, cte_t *c_جذر_src_Slot,
                                    cap_t v_جذر_new_Cap, cte_t *v_جذر_src_Slot,
                                    word_t bufferAddr, cap_t bufferCap,
                                    cte_t *bufferSrcSlot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    إذا (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    إذا (updateFlags & thread_control_update_mcp) {
        setMCPriority(target, mcp);
    }

    إذا (updateFlags & thread_control_update_space) {
        cte_t *جذر_Slot;

        جذر_Slot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(جذر_Slot, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        إذا (sameObjectAs(c_جذر_new_Cap, c_جذر_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_أدرج(c_جذر_new_Cap, c_جذر_src_Slot, جذر_Slot);
        }
    }

    إذا (updateFlags & thread_control_update_space) {
        cte_t *جذر_Slot;

        جذر_Slot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(جذر_Slot, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        إذا (sameObjectAs(v_جذر_new_Cap, v_جذر_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_أدرج(v_جذر_new_Cap, v_جذر_src_Slot, جذر_Slot);
        }
    }

    إذا (updateFlags & thread_control_update_ipc_buffer) {
        cte_t *bufferSlot;

        bufferSlot = TCB_PTR_CTE_PTR(target, tcbBuffer);
        e = cteDelete(bufferSlot, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        target->tcbIPCBuffer = bufferAddr;

        إذا (bufferSrcSlot && sameObjectAs(bufferCap, bufferSrcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_أدرج(bufferCap, bufferSrcSlot, bufferSlot);
        }

        إذا (target == عقدة_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    إذا (updateFlags & thread_control_update_priority) {
        setPriority(target, priority);
    }

    أرجع EXCEPTION_NONE;
}
#endif

#ifdef CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t priority,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    إذا (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_sched_update_mcp) {
        setMCPriority(target, mcp);
    }

    إذا (updateFlags & thread_control_sched_update_priority) {
        setPriority(target, priority);
    }

    إذا (updateFlags & thread_control_sched_update_sc) {
        إذا (sc != عدم) {
            schedContext_bindTCB(sc, target);
        } وإلا إذا (sc == عدم) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    أرجع EXCEPTION_NONE;
}
#endif

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    إذا (suspendSource) {
        suspend(tcb_src);
    }

    إذا (resumeTarget) {
        restart(dest);
    }

    إذا (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        لكل (i = 0; i < n_frameRegisters; i++) {
            v = getRegister(tcb_src, frameRegisters[i]);
            setRegister(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        setNextPC(dest, pc);
    }

    إذا (transferInteger) {
        word_t i;
        word_t v;

        لكل (i = 0; i < n_gpRegisters; i++) {
            v = getRegister(tcb_src, gpRegisters[i]);
            setRegister(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    إذا (dest == عقدة_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    أرجع Arch_performTransfer(transferArch, tcb_src, dest);
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

    thread = عقدة_STATE(ksCurThread);

    إذا (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, عقدة_STATE(ksCurThread));
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    إذا (call) {
        word_t *ipcBuffer;

        ipcBuffer = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);

        لكل (i = 0; i < n && i < n_frameRegisters && i < n_رسالة_Registers; i++) {
            setRegister(thread, رسالة_Registers[i],
                        getRegister(tcb_src, frameRegisters[i]));
        }

        إذا (ipcBuffer != عدم && i < n && i < n_frameRegisters) {
            لكل (; i < n && i < n_frameRegisters; i++) {
                ipcBuffer[i + 1] = getRegister(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        لكل (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_رسالة_Registers; i++) {
            setRegister(thread, رسالة_Registers[i + n_frameRegisters],
                        getRegister(tcb_src, gpRegisters[i]));
        }

        إذا (ipcBuffer != عدم && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            لكل (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipcBuffer[i + n_frameRegisters + 1] =
                    getRegister(tcb_src, gpRegisters[i]);
            }
        }

        setRegister(thread, رسالة_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, i + j)));
    }
    setThreadState(thread, ThreadState_Running);

    أرجع EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *buffer)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, عقدة_STATE(ksCurThread), dest);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    إذا (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_getSanitiseRegisterInfo(dest);

    لكل (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        setRegister(dest, frameRegisters[i],
                    sanitiseRegister(frameRegisters[i],
                                     getSyscallArg(i + 2, buffer), archInfo));
    }

    لكل (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        setRegister(dest, gpRegisters[i],
                    sanitiseRegister(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   buffer), archInfo));
    }

    pc = getRestartPC(dest);
    setNextPC(dest, pc);

    Arch_postModifyRegisters(dest);

    إذا (resumeTarget) {
        restart(dest);
    }

    إذا (dest == عقدة_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    أرجع EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    إذا (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } وإلا {
        unbindNotification(tcb);
    }

    أرجع EXCEPTION_NONE;
}

#ifdef CONFIG_DEBUG_BUILD
فراغ setThreadName(tcb_t *tcb, ثابت حرف *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_LENGTH);
}
#endif

word_t setMRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    بدل (current_syscall_error.type) {
    حالة seL4_InvalidArgument:
        أرجع setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Argument_رقم);

    حالة seL4_InvalidCapability:
        أرجع setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_رقم);

    حالة seL4_IllegalOperation:
        أرجع 0;

    حالة seL4_RangeError:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        أرجع setMR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    حالة seL4_AlignmentError:
        أرجع 0;

    حالة seL4_FailedLookup:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        أرجع setMRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    حالة seL4_TruncatedMessage:
    حالة seL4_DeleteFirst:
    حالة seL4_RevokeFirst:
        أرجع 0;
    حالة seL4_NotEnoughMemory:
        أرجع setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.memory_يسار);
    افتراضي:
        fail("Invalid syscall error");
    }
}
