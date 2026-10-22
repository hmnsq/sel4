/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <api/invocation.h>
#समावेश <api/syscall.h>
#समावेश <sel4/shared_types.h>
#समावेश <machine/io.h>
#समावेश <object/structures.h>
#समावेश <object/objecttype.h>
#समावेश <object/cnode.h>
#अगर_घोषित CONFIG_KERNEL_MCS
#समावेश <object/schedcontext.h>
#पूर्ण_अगर
#समावेश <object/tcb.h>
#समावेश <kernel/cspace.h>
#समावेश <kernel/thread.h>
#समावेश <kernel/vspace.h>
#समावेश <model/statedata.h>
#समावेश <util.h>
#समावेश <string.h>
#समावेश <stdint.h>
#समावेश <arch/smp/ipi_inline.h>

#घोषणा NULL_PRIO 0

अटल exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    निश्चित(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    अगर (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    वापस EXCEPTION_NONE;
}

अटल अंतरभूत व्योम addToBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    गाँठ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    गाँठ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & MASK(wordRadix));
}

अटल अंतरभूत व्योम removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_to_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    गाँठ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & MASK(wordRadix));
    अगर (unlikely(!गाँठ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        गाँठ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcb_Sched_पूर्व;
    after = tcb->tcb_Sched_अग्र;

    अगर (queue.शीर्ष == tcb && queue.end == tcb) {
        queue.शीर्ष = शून्य;
        queue.end = शून्य;
    } अन्यथा {
        अगर (queue.शीर्ष == tcb) {
            after->tcb_Sched_पूर्व = शून्य;
            tcb->tcb_Sched_अग्र = शून्य;
            queue.शीर्ष = after;
        } अन्यथा {
            अगर (queue.end == tcb) {
                before->tcb_Sched_अग्र = शून्य;
                tcb->tcb_Sched_पूर्व = शून्य;
                queue.end = before;
            } अन्यथा {
                before->tcb_Sched_अग्र = after;
                after->tcb_Sched_पूर्व = before;
                tcb->tcb_Sched_पूर्व = शून्य;
                tcb->tcb_Sched_अग्र = शून्य;
            }
        }
    }

    वापस queue;
}

/* Add TCB to the head of a scheduler queue */
व्योम tcbSchedEnqueue(tcb_t *tcb)
{
#अगर_घोषित CONFIG_KERNEL_MCS
    निश्चित(isSchedulable(tcb));
    निश्चित(refill_sufficient(tcb->tcbSchedContext, 0));
#पूर्ण_अगर

    अगर (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_सूचकांक(dom, prio);
        queue = गाँठ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        अगर (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        गाँठ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
व्योम tcbSchedAppend(tcb_t *tcb)
{
#अगर_घोषित CONFIG_KERNEL_MCS
    निश्चित(isSchedulable(tcb));
    निश्चित(refill_sufficient(tcb->tcbSchedContext, 0));
    निश्चित(refill_ready(tcb->tcbSchedContext));
#पूर्ण_अगर
    अगर (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_सूचकांक(dom, prio);
        queue = गाँठ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        अगर (tcb_queue_empty(queue)) {
            addToBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        गाँठ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
व्योम tcbSchedDequeue(tcb_t *tcb)
{
    अगर (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcbPriority;
        idx = ready_queues_सूचकांक(dom, prio);
        queue = गाँठ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        गाँठ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_set_tcbQueued(&tcb->tcbState, false);

        अगर (likely(tcb_queue_empty(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#अगर_घोषित CONFIG_DEBUG_BUILD
व्योम tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcb_Debug_पूर्व = शून्य;

    debug_tcb->tcb_Debug_अग्र = गाँठ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    अगर (गाँठ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(गाँठ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcb_Debug_पूर्व = tcb;
    }

    गाँठ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

व्योम tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    निश्चित(गाँठ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != शून्य);
    अगर (tcb == गाँठ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        गाँठ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(गाँठ_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcb_Debug_अग्र;
    } अन्यथा {
        निश्चित(TCB_PTR_DEBUG_PTR(tcb)->tcb_Debug_पूर्व);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_पूर्व)->tcb_Debug_अग्र = debug_tcb->tcb_Debug_अग्र;
    }

    अगर (debug_tcb->tcb_Debug_अग्र) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_अग्र)->tcb_Debug_पूर्व = debug_tcb->tcb_Debug_पूर्व;
    }

    debug_tcb->tcb_Debug_पूर्व = शून्य;
    debug_tcb->tcb_Debug_अग्र = शून्य;
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */

#अगर_अघोषित CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    अगर (!queue.शीर्ष) { /* Empty list */
        queue.शीर्ष = tcb;
    } अन्यथा {
        queue.end->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.end;
    tcb->tcbEPNext = शून्य;
    queue.end = tcb;

    वापस queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    अगर (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } अन्यथा {
        queue.शीर्ष = tcb->tcbEPNext;
    }

    अगर (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } अन्यथा {
        queue.end = tcb->tcbEPPrev;
    }

    वापस queue;
}
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */

#अगर_घोषित CONFIG_KERNEL_MCS

व्योम tcbReleaseRemove(tcb_t *tcb)
{
    अगर (likely(thread_state_get_tcbInReleaseQueue(tcb->tcbState))) {
        tcb_queue_t queue = गाँठ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        अगर (queue.शीर्ष == tcb) {
            गाँठ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        गाँठ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, false);
    }
}

अटल अंतरभूत ticks_t PURE tcbReadyTime(tcb_t *tcb)
{
    वापस refill_शीर्ष(tcb->tcbSchedContext)->rTime;
}

अटल अंतरभूत bool_t PURE time_after(tcb_t *tcb, ticks_t new_time)
{
    वापस tcb != शून्य && new_time >= tcbReadyTime(tcb);
}

अटल tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    जबतक (time_after(after, new_time)) {
        after = after->tcb_Sched_अग्र;
    }

    वापस after;
}

व्योम tcbReleaseEnqueue(tcb_t *tcb)
{
    निश्चित(thread_state_get_tcbInReleaseQueue(tcb->tcbState) == false);
    निश्चित(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = गाँठ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    अगर (tcb_queue_empty(queue) || new_time < tcbReadyTime(queue.शीर्ष)) {
        गाँठ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } अन्यथा {
        अगर (tcbReadyTime(queue.end) <= new_time) {
            गाँठ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } अन्यथा {
            tcb_t *after;
            after = find_time_after(queue.शीर्ष, new_time);
            tcb_queue_जोड़ो(tcb, after);
        }
    }

    thread_state_ptr_set_tcbInReleaseQueue(&tcb->tcbState, true);

    अगर (queue.शीर्ष != गाँठ_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).शीर्ष) {
        गाँठ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#पूर्ण_अगर

cptr_t PURE getExtraCPtr(word_t *बफर_Ptr, word_t i)
{
    वापस (cptr_t)बफर_Ptr[se_L4_संदेश_Max_लंबाई + 2 + i];
}

व्योम setExtraBadge(word_t *बफर_Ptr, word_t badge,
                   word_t i)
{
    बफर_Ptr[se_L4_संदेश_Max_लंबाई + 2 + i] = badge;
}

#अगर_अघोषित CONFIG_KERNEL_MCS
व्योम setupCallerCap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *replySlot, *callerSlot;
    cap_t masterCap UNUSED, callerCap UNUSED;

    setThreadState(sender, ThreadState_BlockedOnReply);
    replySlot = TCB_PTR_CTE_PTR(sender, tcbReply);
    masterCap = replySlot->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    निश्चित(cap_get_capType(masterCap) == cap_reply_cap);
    निश्चित(cap_reply_cap_get_capReplyMaster(masterCap));
    निश्चित(cap_reply_cap_get_capReplyCanGrant(masterCap));
    निश्चित(TCB_PTR(cap_reply_cap_get_capTCBPtr(masterCap)) == sender);
    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = callerSlot->cap;
    /* Haskell error: "Caller cap must not already exist" */
    निश्चित(cap_get_capType(callerCap) == cap_null_cap);
    cte_जोड़ो(cap_reply_cap_new(canGrant, false, TCB_REF(sender)),
              replySlot, callerSlot);
}

व्योम deleteCallerCap(tcb_t *receiver)
{
    cte_t *callerSlot;

    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(callerSlot);
}
#पूर्ण_अगर

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, word_t *बफर_Ptr, seL4_MessageInfo_t info)
{
    lookupSlot_raw_ret_t lu_ret;
    cptr_t cptr;
    word_t i, लंबाई;

    अगर (!बफर_Ptr) {
        current_extra_caps.excaprefs[0] = शून्य;
        वापस EXCEPTION_NONE;
    }

    लंबाई = seL4_MessageInfo_get_extraCaps(info);

    क्रम (i = 0; i < लंबाई; i++) {
        cptr = getExtraCPtr(बफर_Ptr, i);

        lu_ret = lookupSlot(thread, cptr);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            वापस lu_ret.status;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    अगर (i < se_L4_संदेश_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = शून्य;
    }

    वापस EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    क्रम (i = 0; i < n && i < n_संदेश_Registers; i++) {
        setRegister(receiver, संदेश_Registers[i],
                    getRegister(sender, संदेश_Registers[i]));
    }

    अगर (!recvBuf || !sendBuf) {
        वापस i;
    }

    /* Copy out-of-line words */
    क्रम (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    वापस i;
}

#अगर_घोषित ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
व्योम remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    अगर (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = गाँठ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        अगर (targetCurThread == गाँठ_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcbPriority > targetCurThread->tcbPriority
#अगर_घोषित CONFIG_KERNEL_MCS
            || गाँठ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#पूर्ण_अगर
           ) {
            ARCH_गाँठ_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
व्योम remoteTCBStall(tcb_t *tcb)
{

    अगर (
#अगर_घोषित CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#पूर्ण_अगर
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        गाँठ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_गाँठ_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#अगर_अघोषित CONFIG_KERNEL_MCS
अटल exception_t invokeTCB_SetAffinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcbSchedDequeue(thread);
    migrateTCB(thread, affinity);
    अगर (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    अगर (thread == गाँठ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    वापस EXCEPTION_NONE;
}

अटल exception_t decodeSetAffinity(cap_t cap, word_t लंबाई, word_t *बफर)
{
    tcb_t *tcb;
    word_t affinity;

    अगर (लंबाई < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, बफर);
    अगर (affinity >= ks_अंक_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeTCB_SetAffinity(tcb, affinity);
}
#पूर्ण_अगर
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
अटल exception_t invokeConfigureSingleStepping(bool_t call, word_t *बफर, tcb_t *t,
                                                 uint16_t bp_अंक, word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = गाँठ_STATE(ksCurThread);
    word_t मूल्य;

    bp_was_consumed = configureSingleStepping(t, bp_अंक, n_instrs, false);
    अगर (n_instrs == 0) {
#अगर_अघोषित CONFIG_ARCH_AARCH64
        unsetBreakpointUsedFlag(t, bp_अंक);
#पूर्ण_अगर /* CONFIG_ARCH_AARCH64 */
        मूल्य = false;
    } अन्यथा {
#अगर_अघोषित CONFIG_ARCH_AARCH64
        setBreakpointUsedFlag(t, bp_अंक);
#पूर्ण_अगर /* CONFIG_ARCH_AARCH64 */
        मूल्य = bp_was_consumed;
    }

    अगर (call) {
        setRegister(thread, badgeRegister, 0);
        अचिन्हित पूर्णांक लंबाई = setMR(thread, बफर, 0, मूल्य);
        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, लंबाई)));
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
    वापस EXCEPTION_NONE;
}

अटल exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *बफर)
{
    uint16_t bp_अंक;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_अंक = getSyscallArg(0, बफर);
    n_instrs = getSyscallArg(1, बफर);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_अंक, n_instrs, false);
    अगर (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeConfigureSingleStepping(call, बफर, tcb, bp_अंक, n_instrs);
}

अटल exception_t invokeSetBreakpoint(tcb_t *tcb, uint16_t bp_अंक,
                                       word_t vaddr, word_t type, word_t आकार, word_t rw)
{
    setBreakpoint(tcb, bp_अंक, vaddr, type, आकार, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    setBreakpointUsedFlag(tcb, bp_अंक);
    वापस EXCEPTION_NONE;
}

अटल exception_t decodeSetBreakpoint(cap_t cap, word_t *बफर)
{
    uint16_t bp_अंक;
    word_t vaddr, type, आकार, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_अंक = getSyscallArg(0, बफर);
    vaddr = getSyscallArg(1, बफर);
    type = getSyscallArg(2, बफर);
    आकार = getSyscallArg(3, बफर);
    rw = getSyscallArg(4, बफर);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    अगर (vaddr >= (word_t)USER_शीर्ष) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (type != seL4_InstructionBreakpoint && type != se_L4_तथ्य_Breakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 2;
        वापस EXCEPTION_SYSCALL_ERROR;
    } अन्यथा अगर (type == seL4_InstructionBreakpoint) {
        अगर (आकार != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 3;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अगर (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 4;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अगर ((seL4_FirstWatchpoint == -1 || bp_अंक >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 2;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
    } अन्यथा अगर (type == se_L4_तथ्य_Breakpoint) {
        अगर (आकार == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 3;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अगर (seL4_FirstWatchpoint != -1 && bp_अंक < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 2;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
    } अन्यथा अगर (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 2;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 3;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (आकार != 0 && आकार != 1 && आकार != 2 && आकार != 4 && आकार != 8) {
        userError("Debug: Invalid size %lu.", आकार);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 3;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (आकार > 0 && vaddr & (आकार - 1)) {
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
                  vaddr, आकार);

        current_syscall_error.type = seL4_AlignmentError;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decodeSetBreakpoint(tcb, bp_अंक, vaddr, type, आकार, rw);
    अगर (error.type != seL4_NoError) {
        current_syscall_error = error;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeSetBreakpoint(tcb, bp_अंक,
                               vaddr, type, आकार, rw);
}

अटल exception_t invokeGetBreakpoint(bool_t call, word_t *बफर, tcb_t *tcb, uint16_t bp_अंक)
{
    tcb_t *thread;
    thread = गाँठ_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_अंक);
    अगर (call) {
        setRegister(thread, badgeRegister, 0);
        setMR(गाँठ_STATE(ksCurThread), बफर, 0, res.vaddr);
        setMR(गाँठ_STATE(ksCurThread), बफर, 1, res.type);
        setMR(गाँठ_STATE(ksCurThread), बफर, 2, res.आकार);
        setMR(गाँठ_STATE(ksCurThread), बफर, 3, res.rw);
        setMR(गाँठ_STATE(ksCurThread), बफर, 4, res.is_enabled);
        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, 5)));
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
    वापस EXCEPTION_NONE;
}

अटल exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *बफर)
{
    tcb_t *tcb;
    uint16_t bp_अंक;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_अंक = getSyscallArg(0, बफर);

    error = Arch_decodeGetBreakpoint(tcb, bp_अंक);
    अगर (error.type != seL4_NoError) {
        current_syscall_error = error;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeGetBreakpoint(call, बफर, tcb, bp_अंक);
}

अटल exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_अंक)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_अंक);
    unsetBreakpointUsedFlag(tcb, bp_अंक);
    वापस EXCEPTION_NONE;
}

अटल exception_t decodeUnsetBreakpoint(cap_t cap, word_t *बफर)
{
    tcb_t *tcb;
    uint16_t bp_अंक;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_अंक = getSyscallArg(0, बफर);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_अंक);
    अगर (error.type != seL4_NoError) {
        current_syscall_error = error;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeUnsetBreakpoint(tcb, bp_अंक);
}
#पूर्ण_अगर /* CONFIG_HARDWARE_DEBUG_API */

अटल exception_t invokeSetTLSBase(tcb_t *thread, word_t tls_base)
{
    setRegister(thread, TLS_BASE, tls_base);
    अगर (thread == गाँठ_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    वापस EXCEPTION_NONE;
}

अटल exception_t decodeSetTLSBase(cap_t cap, word_t लंबाई, word_t *बफर)
{
    word_t tls_base;

    अगर (लंबाई < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    tls_base = getSyscallArg(0, बफर);

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeSetTLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_base);
}

अटल व्योम invokeSetFlags(tcb_t *thread, word_t clear, word_t set, bool_t call)
{
    tcb_t *cur_thread = गाँठ_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= set & seL4_TCBFlag_MASK;
    thread->tcbFlags = flags;

#अगर_घोषित CONFIG_HAVE_FPU
    अगर (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } अन्यथा अगर (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#पूर्ण_अगर
    अगर (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, cur_thread);
        setRegister(cur_thread, badgeRegister, 0);
        अचिन्हित पूर्णांक लंबाई = setMR(cur_thread, ipc_बफर, 0, flags);
        setRegister(cur_thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, लंबाई)));
    }
    setThreadState(cur_thread, ThreadState_Running);
}

अटल exception_t decodeSetFlags(cap_t cap, word_t लंबाई, bool_t call, word_t *बफर)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    अगर (लंबाई < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, बफर);
    word_t set   = getSyscallArg(1, बफर);

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    invokeSetFlags(thread, clear, set, call);
    वापस EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(word_t invLabel, word_t लंबाई, cap_t cap,
                                cte_t *slot, bool_t call, word_t *बफर)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    चयन (invLabel) {
    हाल TCBReadRegisters:
        /* Second level of decoding */
        वापस decodeReadRegisters(cap, लंबाई, call, बफर);

    हाल TCBWriteRegisters:
        वापस decodeWriteRegisters(cap, लंबाई, बफर);

    हाल TCBCopyRegisters:
        वापस decodeCopyRegisters(cap, लंबाई, बफर);

    हाल TCBSuspend:
        /* Jump straight to the invoke */
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    हाल TCBResume:
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    हाल TCBConfigure:
        वापस decodeTCBConfigure(cap, लंबाई, slot, बफर);

    हाल TCBSetPriority:
        वापस decodeSetPriority(cap, लंबाई, बफर);

    हाल TCBSetMCPriority:
        वापस decodeSetMCPriority(cap, लंबाई, बफर);

    हाल TCBSetSchedParams:
#अगर_घोषित CONFIG_KERNEL_MCS
        वापस decodeSetSchedParams(cap, लंबाई, slot, बफर);
#अन्यथा
        वापस decodeSetSchedParams(cap, लंबाई, बफर);
#पूर्ण_अगर

    हाल TCBSetIPCBuffer:
        वापस decodeSetIPCBuffer(cap, लंबाई, slot, बफर);

    हाल TCBSetSpace:
        वापस decodeSetSpace(cap, लंबाई, slot, बफर);

    हाल TCBBindNotification:
        वापस decodeBindNotification(cap);

    हाल TCBUnbindNotification:
        वापस decodeUnbindNotification(cap);

#अगर_घोषित CONFIG_KERNEL_MCS
    हाल TCBSetTimeoutEndpoint:
        वापस decodeSetTimeoutEndpoint(cap, slot);
#अन्यथा
#अगर_घोषित ENABLE_SMP_SUPPORT
    हाल TCBSetAffinity:
        वापस decodeSetAffinity(cap, लंबाई, बफर);
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
#पूर्ण_अगर

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#अगर_घोषित CONFIG_VTX
    हाल TCBSetEPTRoot:
        वापस decodeSetEPTRoot(cap);
#पूर्ण_अगर

#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
    हाल TCBConfigureSingleStepping:
        वापस decodeConfigureSingleStepping(cap, call, बफर);

    हाल TCBSetBreakpoint:
        वापस decodeSetBreakpoint(cap, बफर);

    हाल TCBGetBreakpoint:
        वापस decodeGetBreakpoint(cap, call, बफर);

    हाल TCBUnsetBreakpoint:
        वापस decodeUnsetBreakpoint(cap, बफर);
#पूर्ण_अगर

    हाल TCBSetTLSBase:
        वापस decodeSetTLSBase(cap, लंबाई, बफर);

    हाल TCBSetFlags:
        वापस decodeSetFlags(cap, लंबाई, call, बफर);

    शेष:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
}

क्रमागत CopyRegistersFlags {
    CopyRegisters_suspendSource = 0,
    CopyRegisters_resumeTarget = 1,
    CopyRegisters_transferFrame = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, word_t लंबाई, word_t *बफर)
{
    word_t transferArch;
    tcb_t *srcTCB;
    cap_t source_cap;
    word_t flags;

    अगर (लंबाई < 1 || current_extra_caps.excaprefs[0] == शून्य) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, बफर);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    अगर (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } अन्यथा {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & BIT(CopyRegisters_suspendSource),
               flags & BIT(CopyRegisters_resumeTarget),
               flags & BIT(CopyRegisters_transferFrame),
               flags & BIT(CopyRegisters_transferInteger),
               transferArch);

}

क्रमागत ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, word_t लंबाई, bool_t call,
                                word_t *बफर)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    अगर (लंबाई < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, बफर);
    n     = getSyscallArg(1, बफर);

    अगर (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (पूर्णांक)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    अगर (thread == गाँठ_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

क्रमागत WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t लंबाई, word_t *बफर)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    अगर (लंबाई < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, बफर);
    w     = getSyscallArg(1, बफर);

    अगर (लंबाई - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (पूर्णांक)(लंबाई - 2), (पूर्णांक)w);
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    अगर (thread == गाँठ_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, बफर);
}

#अगर_घोषित CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_endpoint_cap:
        वापस (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_capCanGrantReply(cap)));
    हाल cap_null_cap:
        /* just has no fault endpoint */
        वापस true;
    शेष:
        वापस false;
    }
}
#पूर्ण_अगर

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t लंबाई, cte_t *slot, word_t *बफर)
{
    cte_t *बफर_Slot, *c_मूल_Slot, *v_मूल_Slot;
    cap_t बफर_Cap, c_मूल_Cap, v_मूल_Cap;
    deriveCap_ret_t dc_ret;
    word_t c_मूल_तथ्य, v_मूल_तथ्य, बफर_Addr;
#अगर_घोषित CONFIG_KERNEL_MCS
#घोषणा TCBCONFIGURE_ARGS 3
#अन्यथा
#घोषणा TCBCONFIGURE_ARGS 4
#पूर्ण_अगर
    अगर (लंबाई < TCBCONFIGURE_ARGS || current_extra_caps.excaprefs[0] == शून्य
        || current_extra_caps.excaprefs[1] == शून्य
        || current_extra_caps.excaprefs[2] == शून्य) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    c_मूल_तथ्य     = getSyscallArg(0, बफर);
    v_मूल_तथ्य     = getSyscallArg(1, बफर);
    बफर_Addr    = getSyscallArg(2, बफर);
#अन्यथा
    cptr_t faultEP       = getSyscallArg(0, बफर);
    c_मूल_तथ्य     = getSyscallArg(1, बफर);
    v_मूल_तथ्य     = getSyscallArg(2, बफर);
    बफर_Addr    = getSyscallArg(3, बफर);
#पूर्ण_अगर

    c_मूल_Slot  = current_extra_caps.excaprefs[0];
    c_मूल_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_मूल_Slot  = current_extra_caps.excaprefs[1];
    v_मूल_Cap   = current_extra_caps.excaprefs[1]->cap;
    बफर_Slot = current_extra_caps.excaprefs[2];
    बफर_Cap  = current_extra_caps.excaprefs[2]->cap;

    अगर (बफर_Addr == 0) {
        बफर_Slot = शून्य;
    } अन्यथा {
        dc_ret = deriveCap(बफर_Slot, बफर_Cap);
        अगर (dc_ret.status != EXCEPTION_NONE) {
            वापस dc_ret.status;
        }
        बफर_Cap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(बफर_Addr, बफर_Cap);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }
    }

    अगर (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (c_मूल_तथ्य != 0) {
        c_मूल_Cap = update_Cap_तथ्य(false, c_मूल_तथ्य, c_मूल_Cap);
    }

    dc_ret = deriveCap(c_मूल_Slot, c_मूल_Cap);
    अगर (dc_ret.status != EXCEPTION_NONE) {
        वापस dc_ret.status;
    }
    c_मूल_Cap = dc_ret.cap;

    अगर (cap_get_capType(c_मूल_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (v_मूल_तथ्य != 0) {
        v_मूल_Cap = update_Cap_तथ्य(false, v_मूल_तथ्य, v_मूल_Cap);
    }

    dc_ret = deriveCap(v_मूल_Slot, v_मूल_Cap);
    अगर (dc_ret.status != EXCEPTION_NONE) {
        वापस dc_ret.status;
    }
    v_मूल_Cap = dc_ret.cap;

    अगर (!is_Valid_VTable_मूल(v_मूल_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
#अगर_घोषित CONFIG_KERNEL_MCS
    वापस invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), शून्य,
               cap_null_cap_new(), शून्य,
               c_मूल_Cap, c_मूल_Slot,
               v_मूल_Cap, v_मूल_Slot,
               बफर_Addr, बफर_Cap,
               बफर_Slot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_बफर);
#अन्यथा
    वापस invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_मूल_Cap, c_मूल_Slot,
               v_मूल_Cap, v_मूल_Slot,
               बफर_Addr, बफर_Cap,
               बफर_Slot, thread_control_update_space |
               thread_control_update_ipc_बफर);
#पूर्ण_अगर
}

exception_t decodeSetPriority(cap_t cap, word_t लंबाई, word_t *बफर)
{
    अगर (लंबाई < 1 || current_extra_caps.excaprefs[0] == शून्य) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, बफर);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    अगर (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newPrio, authTCB);
    अगर (status != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (अचिन्हित दीर्घ) newPrio, (अचिन्हित दीर्घ) authTCB->tcbMCP);
        वापस status;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
#अगर_घोषित CONFIG_KERNEL_MCS
    वापस invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), शून्य,
               cap_null_cap_new(), शून्य,
               NULL_PRIO, newPrio,
               शून्य, thread_control_sched_update_priority);
#अन्यथा
    वापस invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), शून्य,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), शून्य,
               cap_null_cap_new(), शून्य,
               0, cap_null_cap_new(),
               शून्य, thread_control_update_priority);
#पूर्ण_अगर
}

exception_t decodeSetMCPriority(cap_t cap, word_t लंबाई, word_t *बफर)
{
    अगर (लंबाई < 1 || current_extra_caps.excaprefs[0] == शून्य) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, बफर);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    अगर (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    अगर (status != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (अचिन्हित दीर्घ) newMcp, (अचिन्हित दीर्घ) authTCB->tcbMCP);
        वापस status;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
#अगर_घोषित CONFIG_KERNEL_MCS
    वापस invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), शून्य,
               cap_null_cap_new(), शून्य,
               newMcp, NULL_PRIO,
               शून्य, thread_control_sched_update_mcp);
#अन्यथा
    वापस invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), शून्य,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), शून्य,
               cap_null_cap_new(), शून्य,
               0, cap_null_cap_new(),
               शून्य, thread_control_update_mcp);
#पूर्ण_अगर
}

#अगर_घोषित CONFIG_KERNEL_MCS
exception_t decodeSetTimeoutEndpoint(cap_t cap, cte_t *slot)
{
    अगर (current_extra_caps.excaprefs[0] == शून्य) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    अगर (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), शून्य,
               thCap, thSlot,
               cap_null_cap_new(), शून्य,
               cap_null_cap_new(), शून्य,
               0, cap_null_cap_new(), शून्य,
               thread_control_caps_update_timeout);
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_KERNEL_MCS
exception_t decodeSetSchedParams(cap_t cap, word_t लंबाई, cte_t *slot, word_t *बफर)
#अन्यथा
exception_t decodeSetSchedParams(cap_t cap, word_t लंबाई, word_t *बफर)
#पूर्ण_अगर
{
    अगर (लंबाई < 2 || current_extra_caps.excaprefs[0] == शून्य
#अगर_घोषित CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == शून्य || current_extra_caps.excaprefs[2] == शून्य
#पूर्ण_अगर
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, बफर);
    prio_t newPrio = getSyscallArg(1, बफर);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#अगर_घोषित CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#पूर्ण_अगर

    अगर (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t status = checkPrio(newMcp, authTCB);
    अगर (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (अचिन्हित दीर्घ) newMcp, (अचिन्हित दीर्घ) authTCB->tcbMCP);
        वापस status;
    }

    status = checkPrio(newPrio, authTCB);
    अगर (status != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (अचिन्हित दीर्घ) newPrio, (अचिन्हित दीर्घ) authTCB->tcbMCP);
        वापस status;
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = शून्य;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_priority |
                                         thread_control_sched_update_fault;
    चयन (cap_get_capType(scCap)) {
    हाल cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        अगर (tcb->tcbSchedContext != sc) {
            अगर (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                वापस EXCEPTION_SYSCALL_ERROR;
            }
            अगर (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                वापस EXCEPTION_SYSCALL_ERROR;
            }
        }
        अगर (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अवरोध;
    हाल cap_null_cap:
        अगर (tcb == गाँठ_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अवरोध;
    शेष:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 2;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    अगर (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    अगर (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 3;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
#पूर्ण_अगर
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
#अगर_घोषित CONFIG_KERNEL_MCS
    वापस invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#अन्यथा
    वापस invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), शून्य,
               0, newMcp, newPrio,
               cap_null_cap_new(), शून्य,
               cap_null_cap_new(), शून्य,
               0, cap_null_cap_new(),
               शून्य, thread_control_update_mcp |
               thread_control_update_priority);
#पूर्ण_अगर
}


exception_t decodeSetIPCBuffer(cap_t cap, word_t लंबाई, cte_t *slot, word_t *बफर)
{
    cptr_t cptr_बफर_Ptr;
    cap_t बफर_Cap;
    cte_t *बफर_Slot;

    अगर (लंबाई < 1 || current_extra_caps.excaprefs[0] == शून्य) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    cptr_बफर_Ptr  = getSyscallArg(0, बफर);
    बफर_Slot = current_extra_caps.excaprefs[0];
    बफर_Cap  = current_extra_caps.excaprefs[0]->cap;

    अगर (cptr_बफर_Ptr == 0) {
        बफर_Slot = शून्य;
    } अन्यथा {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(बफर_Slot, बफर_Cap);
        अगर (dc_ret.status != EXCEPTION_NONE) {
            वापस dc_ret.status;
        }
        बफर_Cap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_बफर_Ptr, बफर_Cap);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
#अगर_घोषित CONFIG_KERNEL_MCS
    वापस invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), शून्य,
               cap_null_cap_new(), शून्य,
               cap_null_cap_new(), शून्य,
               cap_null_cap_new(), शून्य,
               cptr_बफर_Ptr, बफर_Cap,
               बफर_Slot, thread_control_caps_update_ipc_बफर);
#अन्यथा
    वापस invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), शून्य,
               cap_null_cap_new(), शून्य,
               cptr_बफर_Ptr, बफर_Cap,
               बफर_Slot, thread_control_update_ipc_बफर);

#पूर्ण_अगर
}

#अगर_घोषित CONFIG_KERNEL_MCS
#घोषणा DECODE_SET_SPACE_PARAMS 2
#अन्यथा
#घोषणा DECODE_SET_SPACE_PARAMS 3
#पूर्ण_अगर
exception_t decodeSetSpace(cap_t cap, word_t लंबाई, cte_t *slot, word_t *बफर)
{
    word_t c_मूल_तथ्य, v_मूल_तथ्य;
    cte_t *c_मूल_Slot, *v_मूल_Slot;
    cap_t c_मूल_Cap, v_मूल_Cap;
    deriveCap_ret_t dc_ret;

    अगर (लंबाई < DECODE_SET_SPACE_PARAMS || current_extra_caps.excaprefs[0] == शून्य
        || current_extra_caps.excaprefs[1] == शून्य
#अगर_घोषित CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == शून्य
#पूर्ण_अगर
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    c_मूल_तथ्य = getSyscallArg(0, बफर);
    v_मूल_तथ्य = getSyscallArg(1, बफर);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_मूल_Slot  = current_extra_caps.excaprefs[1];
    c_मूल_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_मूल_Slot  = current_extra_caps.excaprefs[2];
    v_मूल_Cap   = current_extra_caps.excaprefs[2]->cap;
#अन्यथा
    cptr_t faultEP   = getSyscallArg(0, बफर);
    c_मूल_तथ्य = getSyscallArg(1, बफर);
    v_मूल_तथ्य = getSyscallArg(2, बफर);

    c_मूल_Slot  = current_extra_caps.excaprefs[0];
    c_मूल_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_मूल_Slot  = current_extra_caps.excaprefs[1];
    v_मूल_Cap   = current_extra_caps.excaprefs[1]->cap;
#पूर्ण_अगर

    अगर (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (c_मूल_तथ्य != 0) {
        c_मूल_Cap = update_Cap_तथ्य(false, c_मूल_तथ्य, c_मूल_Cap);
    }

    dc_ret = deriveCap(c_मूल_Slot, c_मूल_Cap);
    अगर (dc_ret.status != EXCEPTION_NONE) {
        वापस dc_ret.status;
    }
    c_मूल_Cap = dc_ret.cap;

    अगर (cap_get_capType(c_मूल_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (v_मूल_तथ्य != 0) {
        v_मूल_Cap = update_Cap_तथ्य(false, v_मूल_तथ्य, v_मूल_Cap);
    }

    dc_ret = deriveCap(v_मूल_Slot, v_मूल_Cap);
    अगर (dc_ret.status != EXCEPTION_NONE) {
        वापस dc_ret.status;
    }
    v_मूल_Cap = dc_ret.cap;

    अगर (!is_Valid_VTable_मूल(v_मूल_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    /* fault handler */
    अगर (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
#पूर्ण_अगर

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
#अगर_घोषित CONFIG_KERNEL_MCS
    वापस invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), शून्य,
               c_मूल_Cap, c_मूल_Slot,
               v_मूल_Cap, v_मूल_Slot,
               0, cap_null_cap_new(), शून्य, thread_control_caps_update_space | thread_control_caps_update_fault);
#अन्यथा
    वापस invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_मूल_Cap, c_मूल_Slot,
               v_मूल_Cap, v_मूल_Slot,
               0, cap_null_cap_new(), शून्य, thread_control_update_space);
#पूर्ण_अगर
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    अगर (current_extra_caps.excaprefs[0] == शून्य) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    अगर (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    अगर (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } अन्यथा {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर ((tcb_t *)notification_ptr_get_ntfn_Queue_शीर्ष(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }


    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    अगर (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeTCB_NotificationControl(tcb, शून्य);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    वापस EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    वापस EXCEPTION_NONE;
}

#अगर_घोषित CONFIG_KERNEL_MCS
अटल अंतरभूत exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_सूचकांक_t सूचकांक, cap_t newCap, cte_t *srcSlot)
{
    cte_t *मूल_Slot = TCB_PTR_CTE_PTR(target, सूचकांक);
    UNUSED exception_t e = cteDelete(मूल_Slot, true);
    अगर (e != EXCEPTION_NONE) {
        वापस e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    अगर (cap_get_capType(newCap) != cap_null_cap) {
        अगर (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_जोड़ो(newCap, srcSlot, मूल_Slot);
        }
    }
    वापस e;
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t c_मूल_new_Cap, cte_t *c_मूल_src_Slot,
                                        cap_t v_मूल_new_Cap, cte_t *v_मूल_src_Slot,
                                        word_t बफर_Addr, cap_t बफर_Cap,
                                        cte_t *बफर_Src_Slot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    अगर (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }

    }

    अगर (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }
    }

    अगर (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, c_मूल_new_Cap, c_मूल_src_Slot);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, v_मूल_new_Cap, v_मूल_src_Slot);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }
    }

    अगर (updateFlags & thread_control_caps_update_ipc_बफर) {
        cte_t *बफर_Slot;

        बफर_Slot = TCB_PTR_CTE_PTR(target, tcb_बफर);
        e = cteDelete(बफर_Slot, true);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }
        target->tcbIPCBuffer = बफर_Addr;

        अगर (बफर_Src_Slot && sameObjectAs(बफर_Cap, बफर_Src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_जोड़ो(बफर_Cap, बफर_Src_Slot, बफर_Slot);
        }

        अगर (target == गाँठ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    वापस EXCEPTION_NONE;
}
#अन्यथा
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t priority,
                                    cap_t c_मूल_new_Cap, cte_t *c_मूल_src_Slot,
                                    cap_t v_मूल_new_Cap, cte_t *v_मूल_src_Slot,
                                    word_t बफर_Addr, cap_t बफर_Cap,
                                    cte_t *बफर_Src_Slot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    अगर (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    अगर (updateFlags & thread_control_update_mcp) {
        setMCPriority(target, mcp);
    }

    अगर (updateFlags & thread_control_update_space) {
        cte_t *मूल_Slot;

        मूल_Slot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(मूल_Slot, true);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }
        अगर (sameObjectAs(c_मूल_new_Cap, c_मूल_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_जोड़ो(c_मूल_new_Cap, c_मूल_src_Slot, मूल_Slot);
        }
    }

    अगर (updateFlags & thread_control_update_space) {
        cte_t *मूल_Slot;

        मूल_Slot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(मूल_Slot, true);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }
        अगर (sameObjectAs(v_मूल_new_Cap, v_मूल_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_जोड़ो(v_मूल_new_Cap, v_मूल_src_Slot, मूल_Slot);
        }
    }

    अगर (updateFlags & thread_control_update_ipc_बफर) {
        cte_t *बफर_Slot;

        बफर_Slot = TCB_PTR_CTE_PTR(target, tcb_बफर);
        e = cteDelete(बफर_Slot, true);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }
        target->tcbIPCBuffer = बफर_Addr;

        अगर (बफर_Src_Slot && sameObjectAs(बफर_Cap, बफर_Src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_जोड़ो(बफर_Cap, बफर_Src_Slot, बफर_Slot);
        }

        अगर (target == गाँठ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    अगर (updateFlags & thread_control_update_priority) {
        setPriority(target, priority);
    }

    वापस EXCEPTION_NONE;
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t priority,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    अगर (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        अगर (e != EXCEPTION_NONE) {
            वापस e;
        }
    }

    अगर (updateFlags & thread_control_sched_update_mcp) {
        setMCPriority(target, mcp);
    }

    अगर (updateFlags & thread_control_sched_update_priority) {
        setPriority(target, priority);
    }

    अगर (updateFlags & thread_control_sched_update_sc) {
        अगर (sc != शून्य) {
            schedContext_bindTCB(sc, target);
        } अन्यथा अगर (sc == शून्य) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    वापस EXCEPTION_NONE;
}
#पूर्ण_अगर

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    अगर (suspendSource) {
        suspend(tcb_src);
    }

    अगर (resumeTarget) {
        restart(dest);
    }

    अगर (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        क्रम (i = 0; i < n_frameRegisters; i++) {
            v = getRegister(tcb_src, frameRegisters[i]);
            setRegister(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        set_अग्र_PC(dest, pc);
    }

    अगर (transferInteger) {
        word_t i;
        word_t v;

        क्रम (i = 0; i < n_gpRegisters; i++) {
            v = getRegister(tcb_src, gpRegisters[i]);
            setRegister(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    अगर (dest == गाँठ_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    वापस Arch_performTransfer(transferArch, tcb_src, dest);
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

    thread = गाँठ_STATE(ksCurThread);

    अगर (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, गाँठ_STATE(ksCurThread));
    अगर (e != EXCEPTION_NONE) {
        वापस e;
    }

    अगर (call) {
        word_t *ipc_बफर;

        ipc_बफर = lookupIPCBuffer(true, thread);

        setRegister(thread, badgeRegister, 0);

        क्रम (i = 0; i < n && i < n_frameRegisters && i < n_संदेश_Registers; i++) {
            setRegister(thread, संदेश_Registers[i],
                        getRegister(tcb_src, frameRegisters[i]));
        }

        अगर (ipc_बफर != शून्य && i < n && i < n_frameRegisters) {
            क्रम (; i < n && i < n_frameRegisters; i++) {
                ipc_बफर[i + 1] = getRegister(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        क्रम (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_संदेश_Registers; i++) {
            setRegister(thread, संदेश_Registers[i + n_frameRegisters],
                        getRegister(tcb_src, gpRegisters[i]));
        }

        अगर (ipc_बफर != शून्य && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            क्रम (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipc_बफर[i + n_frameRegisters + 1] =
                    getRegister(tcb_src, gpRegisters[i]);
            }
        }

        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, i + j)));
    }
    setThreadState(thread, ThreadState_Running);

    वापस EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *बफर)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, गाँठ_STATE(ksCurThread), dest);
    अगर (e != EXCEPTION_NONE) {
        वापस e;
    }

    अगर (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_getSanitiseRegisterInfo(dest);

    क्रम (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        setRegister(dest, frameRegisters[i],
                    sanitiseRegister(frameRegisters[i],
                                     getSyscallArg(i + 2, बफर), archInfo));
    }

    क्रम (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        setRegister(dest, gpRegisters[i],
                    sanitiseRegister(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   बफर), archInfo));
    }

    pc = getRestartPC(dest);
    set_अग्र_PC(dest, pc);

    Arch_postModifyRegisters(dest);

    अगर (resumeTarget) {
        restart(dest);
    }

    अगर (dest == गाँठ_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    वापस EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    अगर (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } अन्यथा {
        unbindNotification(tcb);
    }

    वापस EXCEPTION_NONE;
}

#अगर_घोषित CONFIG_DEBUG_BUILD
व्योम setThreadName(tcb_t *tcb, स्थिर अक्षर *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_लंबाई);
}
#पूर्ण_अगर

word_t setMRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    चयन (current_syscall_error.type) {
    हाल seL4_InvalidArgument:
        वापस setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Argument_संख्या);

    हाल seL4_InvalidCapability:
        वापस setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_संख्या);

    हाल seL4_IllegalOperation:
        वापस 0;

    हाल seL4_RangeError:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        वापस setMR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    हाल seL4_AlignmentError:
        वापस 0;

    हाल seL4_FailedLookup:
        setMR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        वापस setMRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    हाल seL4_TruncatedMessage:
    हाल seL4_DeleteFirst:
    हाल seL4_RevokeFirst:
        वापस 0;
    हाल seL4_NotEnoughMemory:
        वापस setMR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.memory_बायाँ);
    शेष:
        fail("Invalid syscall error");
    }
}
