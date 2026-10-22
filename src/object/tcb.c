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
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
#अन्तर्भाव <object/schedcontext.h>
#यदि_अन्तः
#अन्तर्भाव <object/tcb.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <arch/smp/ipi_inline.h>

#परिभाषय NULL_PRIO 0

स्थिर exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    प्रतिज्ञा(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    यदि (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर अन्तःस्थ शून्य सङ्कलय_गन्तव्यम्_Bitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_गन्तव्यम्_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    ग्रन्थिः_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= BIT(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    ग्रन्थिः_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= BIT(prio & आवरणम्(wordRadix));
}

स्थिर अन्तःस्थ शून्य removeFromBitmap(word_t cpu, word_t dom, word_t prio)
{
    word_t l1index;
    word_t l1index_inverted;

    l1index = prio_गन्तव्यम्_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    ग्रन्थिः_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~BIT(prio & आवरणम्(wordRadix));
    यदि (unlikely(!ग्रन्थिः_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        ग्रन्थिः_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~BIT(l1index);
    }
}

tcb_queue_t tcb_queue_remove(tcb_queue_t queue, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcb_Sched_पूर्व;
    after = tcb->tcb_Sched_अग्र;

    यदि (queue.शीर्ष == tcb && queue.अन्तः_2 == tcb) {
        queue.शीर्ष = रिक्त;
        queue.अन्तः_2 = रिक्त;
    } अन्यथा {
        यदि (queue.शीर्ष == tcb) {
            after->tcb_Sched_पूर्व = रिक्त;
            tcb->tcb_Sched_अग्र = रिक्त;
            queue.शीर्ष = after;
        } अन्यथा {
            यदि (queue.अन्तः_2 == tcb) {
                before->tcb_Sched_अग्र = रिक्त;
                tcb->tcb_Sched_पूर्व = रिक्त;
                queue.अन्तः_2 = before;
            } अन्यथा {
                before->tcb_Sched_अग्र = after;
                after->tcb_Sched_पूर्व = before;
                tcb->tcb_Sched_पूर्व = रिक्त;
                tcb->tcb_Sched_अग्र = रिक्त;
            }
        }
    }

    प्रत्यागम queue;
}

/* Add TCB to the head of a scheduler queue */
शून्य tcb_Sched_पङ्क्तौ_निक्षिप(tcb_t *tcb)
{
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रतिज्ञा(isSchedulable(tcb));
    प्रतिज्ञा(refill_sufficient(tcb->tcbSchedContext, 0));
#यदि_अन्तः

    यदि (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_प्राथमिकता;
        idx = ready_queues_सूचकांक(dom, prio);
        queue = ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        यदि (tcb_queue_रिक्तानि(queue)) {
            सङ्कलय_गन्तव्यम्_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);

        thread_state_ptr_समुच्चयः_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
शून्य tcbSchedAppend(tcb_t *tcb)
{
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रतिज्ञा(isSchedulable(tcb));
    प्रतिज्ञा(refill_sufficient(tcb->tcbSchedContext, 0));
    प्रतिज्ञा(refill_ready(tcb->tcbSchedContext));
#यदि_अन्तः
    यदि (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_प्राथमिकता;
        idx = ready_queues_सूचकांक(dom, prio);
        queue = ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        यदि (tcb_queue_रिक्तानि(queue)) {
            सङ्कलय_गन्तव्यम्_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_queue_append(queue, tcb);

        thread_state_ptr_समुच्चयः_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
शून्य tcb_Sched_पङ्क्तेः_निष्कासय(tcb_t *tcb)
{
    यदि (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_queue_t queue;
        tcb_queue_t new_queue;
        dom_t dom;
        prio_t prio;
        word_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_प्राथमिकता;
        idx = ready_queues_सूचकांक(dom, prio);
        queue = ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_queue = tcb_queue_remove(queue, tcb);

        ग्रन्थिः_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_queue;

        thread_state_ptr_समुच्चयः_tcb_Queued(&tcb->tcbState, false);

        यदि (likely(tcb_queue_रिक्तानि(new_queue))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
शून्य tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcb_Debug_पूर्व = रिक्त;

    debug_tcb->tcb_Debug_अग्र = ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    यदि (ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcb_Debug_पूर्व = tcb;
    }

    ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

शून्य tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    प्रतिज्ञा(ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != रिक्त);
    यदि (tcb == ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(ग्रन्थिः_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcb_Debug_अग्र;
    } अन्यथा {
        प्रतिज्ञा(TCB_PTR_DEBUG_PTR(tcb)->tcb_Debug_पूर्व);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_पूर्व)->tcb_Debug_अग्र = debug_tcb->tcb_Debug_अग्र;
    }

    यदि (debug_tcb->tcb_Debug_अग्र) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_अग्र)->tcb_Debug_पूर्व = debug_tcb->tcb_Debug_पूर्व;
    }

    debug_tcb->tcb_Debug_पूर्व = रिक्त;
    debug_tcb->tcb_Debug_अग्र = रिक्त;
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_queue_t tcbEPAppend(tcb_t *tcb, tcb_queue_t queue)
{
    यदि (!queue.शीर्ष) { /* Empty list */
        queue.शीर्ष = tcb;
    } अन्यथा {
        queue.अन्तः_2->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = queue.अन्तः_2;
    tcb->tcbEPNext = रिक्त;
    queue.अन्तः_2 = tcb;

    प्रत्यागम queue;
}

/* Remove TCB from an endpoint queue */
tcb_queue_t tcbEPDequeue(tcb_t *tcb, tcb_queue_t queue)
{
    यदि (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } अन्यथा {
        queue.शीर्ष = tcb->tcbEPNext;
    }

    यदि (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } अन्यथा {
        queue.अन्तः_2 = tcb->tcbEPPrev;
    }

    प्रत्यागम queue;
}
#यदि_अन्तः /* CONFIG_KERNEL_MCS */

#यदि_परिभाषितम् CONFIG_KERNEL_MCS

शून्य tcbReleaseRemove(tcb_t *tcb)
{
    यदि (likely(thread_state_get_tcb_अन्तर्_Release_Queue(tcb->tcbState))) {
        tcb_queue_t queue = ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);

        यदि (queue.शीर्ष == tcb) {
            ग्रन्थिः_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_remove(queue, tcb);

        thread_state_ptr_समुच्चयः_tcb_अन्तर्_Release_Queue(&tcb->tcbState, false);
    }
}

स्थिर अन्तःस्थ ticks_t विशुद्धम् tcbReadyTime(tcb_t *tcb)
{
    प्रत्यागम refill_शीर्ष(tcb->tcbSchedContext)->rTime;
}

स्थिर अन्तःस्थ bool_t विशुद्धम् time_after(tcb_t *tcb, ticks_t new_time)
{
    प्रत्यागम tcb != रिक्त && new_time >= tcbReadyTime(tcb);
}

स्थिर tcb_t *find_time_after(tcb_t *tcb, ticks_t new_time)
{
    tcb_t *after = tcb;

    यावत् (time_after(after, new_time)) {
        after = after->tcb_Sched_अग्र;
    }

    प्रत्यागम after;
}

शून्य tcb_Release_पङ्क्तौ_निक्षिप(tcb_t *tcb)
{
    प्रतिज्ञा(thread_state_get_tcb_अन्तर्_Release_Queue(tcb->tcbState) == false);
    प्रतिज्ञा(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_time;
    tcb_queue_t queue;

    queue = ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity);
    new_time = tcbReadyTime(tcb);

    यदि (tcb_queue_रिक्तानि(queue) || new_time < tcbReadyTime(queue.शीर्ष)) {
        ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_prepend(queue, tcb);
    } अन्यथा {
        यदि (tcbReadyTime(queue.अन्तः_2) <= new_time) {
            ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity) = tcb_queue_append(queue, tcb);
        } अन्यथा {
            tcb_t *after;
            after = find_time_after(queue.शीर्ष, new_time);
            tcb_queue_निवेशय(tcb, after);
        }
    }

    thread_state_ptr_समुच्चयः_tcb_अन्तर्_Release_Queue(&tcb->tcbState, true);

    यदि (queue.शीर्ष != ग्रन्थिः_STATE_ON_CORE(ksReleaseQueue, tcb->tcbAffinity).शीर्ष) {
        ग्रन्थिः_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#यदि_अन्तः

cptr_t विशुद्धम् getExtraCPtr(word_t *बफर_Ptr, word_t i)
{
    प्रत्यागम (cptr_t)बफर_Ptr[se_L4_सन्देशः_Max_दीर्घता + 2 + i];
}

शून्य समुच्चयः_Extra_Badge(word_t *बफर_Ptr, word_t badge,
                   word_t i)
{
    बफर_Ptr[se_L4_सन्देशः_Max_दीर्घता + 2 + i] = badge;
}

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
शून्य सज्जता_Caller_Cap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *प्रत्युत्तरम्_Slot, *callerSlot;
    cap_t masterCap अप्रयुक्तम्, callerCap अप्रयुक्तम्;

    समुच्चयः_Thread_State(sender, Thread_State_Blocked_On_प्रत्युत्तरम्);
    प्रत्युत्तरम्_Slot = TCB_PTR_CTE_PTR(sender, tcb_प्रत्युत्तरम्);
    masterCap = प्रत्युत्तरम्_Slot->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    प्रतिज्ञा(cap_get_capType(masterCap) == cap_प्रत्युत्तरम्_cap);
    प्रतिज्ञा(cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Master(masterCap));
    प्रतिज्ञा(cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Can_Grant(masterCap));
    प्रतिज्ञा(TCB_PTR(cap_प्रत्युत्तरम्_cap_get_cap_TCBPtr(masterCap)) == sender);
    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = callerSlot->cap;
    /* Haskell error: "Caller cap must not already exist" */
    प्रतिज्ञा(cap_get_capType(callerCap) == cap_null_cap);
    cte_निवेशय(cap_प्रत्युत्तरम्_cap_new(canGrant, false, TCB_REF(sender)),
              प्रत्युत्तरम्_Slot, callerSlot);
}

शून्य deleteCallerCap(tcb_t *receiver)
{
    cte_t *callerSlot;

    callerSlot = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(callerSlot);
}
#यदि_अन्तः

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, word_t *बफर_Ptr, se_L4_सन्देशः_Info_t info)
{
    lookupSlot_raw_ret_t lu_ret;
    cptr_t cptr;
    word_t i, दीर्घता;

    यदि (!बफर_Ptr) {
        current_extra_caps.excaprefs[0] = रिक्त;
        प्रत्यागम EXCEPTION_NONE;
    }

    दीर्घता = se_L4_सन्देशः_Info_get_extra_Caps(info);

    प्रति (i = 0; i < दीर्घता; i++) {
        cptr = getExtraCPtr(बफर_Ptr, i);

        lu_ret = lookupSlot(thread, cptr);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            प्रत्यागम lu_ret.स्थितिः;
        }

        current_extra_caps.excaprefs[i] = lu_ret.slot;
    }
    यदि (i < se_L4_सन्देशः_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = रिक्त;
    }

    प्रत्यागम EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
word_t copyMRs(tcb_t *sender, word_t *sendBuf, tcb_t *receiver,
               word_t *recvBuf, word_t n)
{
    word_t i;

    /* Copy inline words */
    प्रति (i = 0; i < n && i < n_सन्देशः_Registers; i++) {
        समुच्चयः_पञ्जिकाकोशः(receiver, सन्देशः_Registers[i],
                    get_पञ्जिकाकोशः(sender, सन्देशः_Registers[i]));
    }

    यदि (!recvBuf || !sendBuf) {
        प्रत्यागम i;
    }

    /* Copy out-of-line words */
    प्रति (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    प्रत्यागम i;
}

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
शून्य remoteQueueUpdate(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    यदि (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *targetCurThread = ग्रन्थिः_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        यदि (targetCurThread == ग्रन्थिः_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcb_प्राथमिकता > targetCurThread->tcb_प्राथमिकता
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
            || ग्रन्थिः_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#यदि_अन्तः
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

    यदि (
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#यदि_अन्तः
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        ग्रन्थिः_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_ग्रन्थिः_STATE(ipiReschedulePending) |= BIT(tcb->tcbAffinity);
    }
}

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
स्थिर exception_t invoke_TCB_समुच्चयः_Affinity(tcb_t *thread, word_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcb_Sched_पङ्क्तेः_निष्कासय(thread);
    migrateTCB(thread, affinity);
    यदि (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    यदि (thread == ग्रन्थिः_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decode_समुच्चयः_Affinity(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    tcb_t *tcb;
    word_t affinity;

    यदि (दीर्घता < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, बफर);
    यदि (affinity >= ks_अङ्कः_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invoke_TCB_समुच्चयः_Affinity(tcb, affinity);
}
#यदि_अन्तः
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
स्थिर exception_t invokeConfigureSingleStepping(bool_t call, word_t *बफर, tcb_t *t,
                                                 uint16_t bp_अङ्कः, word_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    word_t मानम्;

    bp_was_consumed = configureSingleStepping(t, bp_अङ्कः, n_instrs, false);
    यदि (n_instrs == 0) {
#यदि_अपरिभाषितम् CONFIG_ARCH_AARCH64
        unset_Breakpoint_प्रयुक्तम्_Flag(t, bp_अङ्कः);
#यदि_अन्तः /* CONFIG_ARCH_AARCH64 */
        मानम् = false;
    } अन्यथा {
#यदि_अपरिभाषितम् CONFIG_ARCH_AARCH64
        समुच्चयः_Breakpoint_प्रयुक्तम्_Flag(t, bp_अङ्कः);
#यदि_अन्तः /* CONFIG_ARCH_AARCH64 */
        मानम् = bp_was_consumed;
    }

    यदि (call) {
        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
        अचिह्नित पूर्णाङ्क दीर्घता = समुच्चयः_MR(thread, बफर, 0, मानम्);
        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, word_t *बफर)
{
    uint16_t bp_अङ्कः;
    word_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_अङ्कः = getSyscallArg(0, बफर);
    n_instrs = getSyscallArg(1, बफर);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_अङ्कः, n_instrs, false);
    यदि (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeConfigureSingleStepping(call, बफर, tcb, bp_अङ्कः, n_instrs);
}

स्थिर exception_t invoke_समुच्चयः_Breakpoint(tcb_t *tcb, uint16_t bp_अङ्कः,
                                       word_t vaddr, word_t type, word_t आकार, word_t rw)
{
    समुच्चयः_Breakpoint(tcb, bp_अङ्कः, vaddr, type, आकार, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    समुच्चयः_Breakpoint_प्रयुक्तम्_Flag(tcb, bp_अङ्कः);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decode_समुच्चयः_Breakpoint(cap_t cap, word_t *बफर)
{
    uint16_t bp_अङ्कः;
    word_t vaddr, type, आकार, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_अङ्कः = getSyscallArg(0, बफर);
    vaddr = getSyscallArg(1, बफर);
    type = getSyscallArg(2, बफर);
    आकार = getSyscallArg(3, बफर);
    rw = getSyscallArg(4, बफर);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    यदि (vaddr >= (word_t)USER_शिखरम्) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (type != seL4_InstructionBreakpoint && type != se_L4_दत्त_Breakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 2;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    } अन्यथा यदि (type == seL4_InstructionBreakpoint) {
        यदि (आकार != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 3;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        यदि (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 4;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        यदि ((seL4_FirstWatchpoint == -1 || bp_अङ्कः >= seL4_FirstWatchpoint)
            && seL4_FirstBreakpoint != seL4_FirstWatchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 2;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
    } अन्यथा यदि (type == se_L4_दत्त_Breakpoint) {
        यदि (आकार == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 3;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        यदि (seL4_FirstWatchpoint != -1 && bp_अङ्कः < seL4_FirstWatchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 2;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
    } अन्यथा यदि (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 2;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 3;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (आकार != 0 && आकार != 1 && आकार != 2 && आकार != 4 && आकार != 8) {
        userError("Debug: Invalid size %lu.", आकार);
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 3;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (आकार > 0 && vaddr & (आकार - 1)) {
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
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decode_समुच्चयः_Breakpoint(tcb, bp_अङ्कः, vaddr, type, आकार, rw);
    यदि (error.type != seL4_NoError) {
        current_syscall_error = error;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invoke_समुच्चयः_Breakpoint(tcb, bp_अङ्कः,
                               vaddr, type, आकार, rw);
}

स्थिर exception_t invokeGetBreakpoint(bool_t call, word_t *बफर, tcb_t *tcb, uint16_t bp_अङ्कः)
{
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_अङ्कः);
    यदि (call) {
        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
        समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, 0, res.vaddr);
        समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, 1, res.type);
        समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, 2, res.आकार);
        समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, 3, res.rw);
        समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, 4, res.is_enabled);
        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, 5)));
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeGetBreakpoint(cap_t cap, bool_t call, word_t *बफर)
{
    tcb_t *tcb;
    uint16_t bp_अङ्कः;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_अङ्कः = getSyscallArg(0, बफर);

    error = Arch_decodeGetBreakpoint(tcb, bp_अङ्कः);
    यदि (error.type != seL4_NoError) {
        current_syscall_error = error;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeGetBreakpoint(call, बफर, tcb, bp_अङ्कः);
}

स्थिर exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_अङ्कः)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_अङ्कः);
    unset_Breakpoint_प्रयुक्तम्_Flag(tcb, bp_अङ्कः);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeUnsetBreakpoint(cap_t cap, word_t *बफर)
{
    tcb_t *tcb;
    uint16_t bp_अङ्कः;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_अङ्कः = getSyscallArg(0, बफर);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_अङ्कः);
    यदि (error.type != seL4_NoError) {
        current_syscall_error = error;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeUnsetBreakpoint(tcb, bp_अङ्कः);
}
#यदि_अन्तः /* CONFIG_HARDWARE_DEBUG_API */

स्थिर exception_t invoke_समुच्चयः_TLSBase(tcb_t *thread, word_t tls_आधारः)
{
    समुच्चयः_पञ्जिकाकोशः(thread, TLS_आधारः, tls_आधारः);
    यदि (thread == ग्रन्थिः_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decode_समुच्चयः_TLSBase(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    word_t tls_आधारः;

    यदि (दीर्घता < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    tls_आधारः = getSyscallArg(0, बफर);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invoke_समुच्चयः_TLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_आधारः);
}

स्थिर शून्य invoke_समुच्चयः_Flags(tcb_t *thread, word_t clear, word_t समुच्चयः, bool_t call)
{
    tcb_t *cur_thread = ग्रन्थिः_STATE(ksCurThread);
    word_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= समुच्चयः & se_L4_TCBFlag_आवरणम्;
    thread->tcbFlags = flags;

#यदि_परिभाषितम् CONFIG_HAVE_FPU
    यदि (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } अन्यथा यदि (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#यदि_अन्तः
    यदि (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, cur_thread);
        समुच्चयः_पञ्जिकाकोशः(cur_thread, badge_पञ्जिकाकोशः, 0);
        अचिह्नित पूर्णाङ्क दीर्घता = समुच्चयः_MR(cur_thread, ipc_बफर, 0, flags);
        समुच्चयः_पञ्जिकाकोशः(cur_thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
    }
    समुच्चयः_Thread_State(cur_thread, ThreadState_Running);
}

स्थिर exception_t decode_समुच्चयः_Flags(cap_t cap, word_t दीर्घता, bool_t call, word_t *बफर)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    यदि (दीर्घता < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    word_t clear = getSyscallArg(0, बफर);
    word_t समुच्चयः   = getSyscallArg(1, बफर);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    invoke_समुच्चयः_Flags(thread, clear, समुच्चयः, call);
    प्रत्यागम EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(word_t invLabel, word_t दीर्घता, cap_t cap,
                                cte_t *slot, bool_t call, word_t *बफर)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    विकल्प (invLabel) {
    पक्ष TCBReadRegisters:
        /* Second level of decoding */
        प्रत्यागम decodeReadRegisters(cap, दीर्घता, call, बफर);

    पक्ष TCBWriteRegisters:
        प्रत्यागम decodeWriteRegisters(cap, दीर्घता, बफर);

    पक्ष TCBCopyRegisters:
        प्रत्यागम decodeCopyRegisters(cap, दीर्घता, बफर);

    पक्ष TCBSuspend:
        /* Jump straight to the invoke */
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    पक्ष TCBResume:
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    पक्ष TCBConfigure:
        प्रत्यागम decodeTCBConfigure(cap, दीर्घता, slot, बफर);

    पक्ष TCBSet_प्राथमिकता:
        प्रत्यागम decode_समुच्चयः_प्राथमिकता(cap, दीर्घता, बफर);

    पक्ष TCBSetMCPriority:
        प्रत्यागम decode_समुच्चयः_MCPriority(cap, दीर्घता, बफर);

    पक्ष TCBSetSchedParams:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        प्रत्यागम decode_समुच्चयः_Sched_Params(cap, दीर्घता, slot, बफर);
#अन्यथा
        प्रत्यागम decode_समुच्चयः_Sched_Params(cap, दीर्घता, बफर);
#यदि_अन्तः

    पक्ष TCBSetIPCBuffer:
        प्रत्यागम decode_समुच्चयः_IPCBuffer(cap, दीर्घता, slot, बफर);

    पक्ष TCBSetSpace:
        प्रत्यागम decode_समुच्चयः_Space(cap, दीर्घता, slot, बफर);

    पक्ष TCBBindNotification:
        प्रत्यागम decodeBindNotification(cap);

    पक्ष TCBUnbindNotification:
        प्रत्यागम decodeUnbindNotification(cap);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष TCBSetTimeoutEndpoint:
        प्रत्यागम decode_समुच्चयः_Timeout_Endpoint(cap, slot);
#अन्यथा
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    पक्ष TCBSetAffinity:
        प्रत्यागम decode_समुच्चयः_Affinity(cap, दीर्घता, बफर);
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
#यदि_अन्तः

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#यदि_परिभाषितम् CONFIG_VTX
    पक्ष TCBSetEPTRoot:
        प्रत्यागम decode_समुच्चयः_EPTRoot(cap);
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
    पक्ष TCBConfigureSingleStepping:
        प्रत्यागम decodeConfigureSingleStepping(cap, call, बफर);

    पक्ष TCBSetBreakpoint:
        प्रत्यागम decode_समुच्चयः_Breakpoint(cap, बफर);

    पक्ष TCBGetBreakpoint:
        प्रत्यागम decodeGetBreakpoint(cap, call, बफर);

    पक्ष TCBUnsetBreakpoint:
        प्रत्यागम decodeUnsetBreakpoint(cap, बफर);
#यदि_अन्तः

    पक्ष TCBSetTLSBase:
        प्रत्यागम decode_समुच्चयः_TLSBase(cap, दीर्घता, बफर);

    पक्ष TCBSetFlags:
        प्रत्यागम decode_समुच्चयः_Flags(cap, दीर्घता, call, बफर);

    स्वभाव:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

गणना CopyRegistersFlags {
    CopyRegisters_suspendSource = 0,
    CopyRegisters_resumeTarget = 1,
    CopyRegisters_transferFrame = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    word_t transferArch;
    tcb_t *srcTCB;
    cap_t source_cap;
    word_t flags;

    यदि (दीर्घता < 1 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, बफर);

    transferArch = Arch_decodeTransfer(flags >> 8);

    source_cap = current_extra_caps.excaprefs[0]->cap;

    यदि (cap_get_capType(source_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(source_cap));
    } अन्यथा {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeTCB_CopyRegisters(
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

exception_t decodeReadRegisters(cap_t cap, word_t दीर्घता, bool_t call,
                                word_t *बफर)
{
    word_t transferArch, flags, n;
    tcb_t *thread;

    यदि (दीर्घता < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, बफर);
    n     = getSyscallArg(1, बफर);

    यदि (n < 1 || n > n_frameRegisters + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (पूर्णाङ्क)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_frameRegisters +
                                              n_gpRegisters;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    यदि (thread == ग्रन्थिः_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & BIT(ReadRegisters_suspend),
               n, transferArch, call);
}

गणना WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    word_t flags, w;
    word_t transferArch;
    tcb_t *thread;

    यदि (दीर्घता < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, बफर);
    w     = getSyscallArg(1, बफर);

    यदि (दीर्घता - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (पूर्णाङ्क)(दीर्घता - 2), (पूर्णाङ्क)w);
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    यदि (thread == ग्रन्थिः_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeTCB_WriteRegisters(thread,
                                    flags & BIT(WriteRegisters_resume),
                                    w, transferArch, बफर);
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_endpoint_cap:
        प्रत्यागम (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_cap_Can_Grant_प्रत्युत्तरम्(cap)));
    पक्ष cap_null_cap:
        /* just has no fault endpoint */
        प्रत्यागम true;
    स्वभाव:
        प्रत्यागम false;
    }
}
#यदि_अन्तः

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, word_t दीर्घता, cte_t *slot, word_t *बफर)
{
    cte_t *बफर_Slot, *c_मूलम्_Slot, *v_मूलम्_Slot;
    cap_t बफर_Cap, c_मूलम्_Cap, v_मूलम्_Cap;
    deriveCap_ret_t dc_ret;
    word_t c_मूलम्_दत्त, v_मूलम्_दत्त, बफर_Addr;
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
#परिभाषय TCBCONFIGURE_तर्काः 3
#अन्यथा
#परिभाषय TCBCONFIGURE_तर्काः 4
#यदि_अन्तः
    यदि (दीर्घता < TCBCONFIGURE_तर्काः || current_extra_caps.excaprefs[0] == रिक्त
        || current_extra_caps.excaprefs[1] == रिक्त
        || current_extra_caps.excaprefs[2] == रिक्त) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    c_मूलम्_दत्त     = getSyscallArg(0, बफर);
    v_मूलम्_दत्त     = getSyscallArg(1, बफर);
    बफर_Addr    = getSyscallArg(2, बफर);
#अन्यथा
    cptr_t faultEP       = getSyscallArg(0, बफर);
    c_मूलम्_दत्त     = getSyscallArg(1, बफर);
    v_मूलम्_दत्त     = getSyscallArg(2, बफर);
    बफर_Addr    = getSyscallArg(3, बफर);
#यदि_अन्तः

    c_मूलम्_Slot  = current_extra_caps.excaprefs[0];
    c_मूलम्_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_मूलम्_Slot  = current_extra_caps.excaprefs[1];
    v_मूलम्_Cap   = current_extra_caps.excaprefs[1]->cap;
    बफर_Slot = current_extra_caps.excaprefs[2];
    बफर_Cap  = current_extra_caps.excaprefs[2]->cap;

    यदि (बफर_Addr == 0) {
        बफर_Slot = रिक्त;
    } अन्यथा {
        dc_ret = deriveCap(बफर_Slot, बफर_Cap);
        यदि (dc_ret.स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम dc_ret.स्थितिः;
        }
        बफर_Cap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(बफर_Addr, बफर_Cap);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }
    }

    यदि (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (c_मूलम्_दत्त != 0) {
        c_मूलम्_Cap = update_Cap_दत्त(false, c_मूलम्_दत्त, c_मूलम्_Cap);
    }

    dc_ret = deriveCap(c_मूलम्_Slot, c_मूलम्_Cap);
    यदि (dc_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम dc_ret.स्थितिः;
    }
    c_मूलम्_Cap = dc_ret.cap;

    यदि (cap_get_capType(c_मूलम्_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (v_मूलम्_दत्त != 0) {
        v_मूलम्_Cap = update_Cap_दत्त(false, v_मूलम्_दत्त, v_मूलम्_Cap);
    }

    dc_ret = deriveCap(v_मूलम्_Slot, v_मूलम्_Cap);
    यदि (dc_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम dc_ret.स्थितिः;
    }
    v_मूलम्_Cap = dc_ret.cap;

    यदि (!is_Valid_VTable_मूलम्(v_मूलम्_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रत्यागम invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               c_मूलम्_Cap, c_मूलम्_Slot,
               v_मूलम्_Cap, v_मूलम्_Slot,
               बफर_Addr, बफर_Cap,
               बफर_Slot, thread_control_caps_update_space |
               thread_control_caps_update_ipc_बफर);
#अन्यथा
    प्रत्यागम invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_मूलम्_Cap, c_मूलम्_Slot,
               v_मूलम्_Cap, v_मूलम्_Slot,
               बफर_Addr, बफर_Cap,
               बफर_Slot, thread_control_update_space |
               thread_control_update_ipc_बफर);
#यदि_अन्तः
}

exception_t decode_समुच्चयः_प्राथमिकता(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    यदि (दीर्घता < 1 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, बफर);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    यदि (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t स्थितिः = checkPrio(newPrio, authTCB);
    यदि (स्थितिः != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (अचिह्नित दीर्घ) newPrio, (अचिह्नित दीर्घ) authTCB->tcbMCP);
        प्रत्यागम स्थितिः;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रत्यागम invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), रिक्त,
               cap_null_cap_new(), रिक्त,
               NULL_PRIO, newPrio,
               रिक्त, thread_control_sched_update_प्राथमिकता);
#अन्यथा
    प्रत्यागम invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), रिक्त,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               0, cap_null_cap_new(),
               रिक्त, thread_control_update_प्राथमिकता);
#यदि_अन्तः
}

exception_t decode_समुच्चयः_MCPriority(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    यदि (दीर्घता < 1 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, बफर);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    यदि (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t स्थितिः = checkPrio(newMcp, authTCB);
    यदि (स्थितिः != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (अचिह्नित दीर्घ) newMcp, (अचिह्नित दीर्घ) authTCB->tcbMCP);
        प्रत्यागम स्थितिः;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रत्यागम invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), रिक्त,
               cap_null_cap_new(), रिक्त,
               newMcp, NULL_PRIO,
               रिक्त, thread_control_sched_update_mcp);
#अन्यथा
    प्रत्यागम invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), रिक्त,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               0, cap_null_cap_new(),
               रिक्त, thread_control_update_mcp);
#यदि_अन्तः
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
exception_t decode_समुच्चयः_Timeout_Endpoint(cap_t cap, cte_t *slot)
{
    यदि (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *thSlot = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    यदि (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), रिक्त,
               thCap, thSlot,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               0, cap_null_cap_new(), रिक्त,
               thread_control_caps_update_timeout);
}
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
exception_t decode_समुच्चयः_Sched_Params(cap_t cap, word_t दीर्घता, cte_t *slot, word_t *बफर)
#अन्यथा
exception_t decode_समुच्चयः_Sched_Params(cap_t cap, word_t दीर्घता, word_t *बफर)
#यदि_अन्तः
{
    यदि (दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == रिक्त || current_extra_caps.excaprefs[2] == रिक्त
#यदि_अन्तः
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, बफर);
    prio_t newPrio = getSyscallArg(1, बफर);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fhSlot = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#यदि_अन्तः

    यदि (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t स्थितिः = checkPrio(newMcp, authTCB);
    यदि (स्थितिः != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (अचिह्नित दीर्घ) newMcp, (अचिह्नित दीर्घ) authTCB->tcbMCP);
        प्रत्यागम स्थितिः;
    }

    स्थितिः = checkPrio(newPrio, authTCB);
    यदि (स्थितिः != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (अचिह्नित दीर्घ) newPrio, (अचिह्नित दीर्घ) authTCB->tcbMCP);
        प्रत्यागम स्थितिः;
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = रिक्त;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_प्राथमिकता |
                                         thread_control_sched_update_fault;
    विकल्प (cap_get_capType(scCap)) {
    पक्ष cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        यदि (tcb->tcbSchedContext != sc) {
            यदि (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
            यदि (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
        }
        यदि (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        भङ्ग;
    पक्ष cap_null_cap:
        यदि (tcb == ग्रन्थिः_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        भङ्ग;
    स्वभाव:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 2;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    यदि (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    यदि (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 3;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
#यदि_अन्तः
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रत्यागम invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               newMcp, newPrio,
               sc, update_flags);
#अन्यथा
    प्रत्यागम invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), रिक्त,
               0, newMcp, newPrio,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               0, cap_null_cap_new(),
               रिक्त, thread_control_update_mcp |
               thread_control_update_प्राथमिकता);
#यदि_अन्तः
}


exception_t decode_समुच्चयः_IPCBuffer(cap_t cap, word_t दीर्घता, cte_t *slot, word_t *बफर)
{
    cptr_t cptr_बफर_Ptr;
    cap_t बफर_Cap;
    cte_t *बफर_Slot;

    यदि (दीर्घता < 1 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    cptr_बफर_Ptr  = getSyscallArg(0, बफर);
    बफर_Slot = current_extra_caps.excaprefs[0];
    बफर_Cap  = current_extra_caps.excaprefs[0]->cap;

    यदि (cptr_बफर_Ptr == 0) {
        बफर_Slot = रिक्त;
    } अन्यथा {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(बफर_Slot, बफर_Cap);
        यदि (dc_ret.स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम dc_ret.स्थितिः;
        }
        बफर_Cap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_बफर_Ptr, बफर_Cap);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रत्यागम invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               cptr_बफर_Ptr, बफर_Cap,
               बफर_Slot, thread_control_caps_update_ipc_बफर);
#अन्यथा
    प्रत्यागम invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), रिक्त,
               cap_null_cap_new(), रिक्त,
               cptr_बफर_Ptr, बफर_Cap,
               बफर_Slot, thread_control_update_ipc_बफर);

#यदि_अन्तः
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
#परिभाषय DECODE_समुच्चयः_SPACE_PARAMS 2
#अन्यथा
#परिभाषय DECODE_समुच्चयः_SPACE_PARAMS 3
#यदि_अन्तः
exception_t decode_समुच्चयः_Space(cap_t cap, word_t दीर्घता, cte_t *slot, word_t *बफर)
{
    word_t c_मूलम्_दत्त, v_मूलम्_दत्त;
    cte_t *c_मूलम्_Slot, *v_मूलम्_Slot;
    cap_t c_मूलम्_Cap, v_मूलम्_Cap;
    deriveCap_ret_t dc_ret;

    यदि (दीर्घता < DECODE_समुच्चयः_SPACE_PARAMS || current_extra_caps.excaprefs[0] == रिक्त
        || current_extra_caps.excaprefs[1] == रिक्त
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == रिक्त
#यदि_अन्तः
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    c_मूलम्_दत्त = getSyscallArg(0, बफर);
    v_मूलम्_दत्त = getSyscallArg(1, बफर);

    cte_t *fhSlot     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_मूलम्_Slot  = current_extra_caps.excaprefs[1];
    c_मूलम्_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_मूलम्_Slot  = current_extra_caps.excaprefs[2];
    v_मूलम्_Cap   = current_extra_caps.excaprefs[2]->cap;
#अन्यथा
    cptr_t faultEP   = getSyscallArg(0, बफर);
    c_मूलम्_दत्त = getSyscallArg(1, बफर);
    v_मूलम्_दत्त = getSyscallArg(2, बफर);

    c_मूलम्_Slot  = current_extra_caps.excaprefs[0];
    c_मूलम्_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_मूलम्_Slot  = current_extra_caps.excaprefs[1];
    v_मूलम्_Cap   = current_extra_caps.excaprefs[1]->cap;
#यदि_अन्तः

    यदि (slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        slotCapLongRunningDelete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (c_मूलम्_दत्त != 0) {
        c_मूलम्_Cap = update_Cap_दत्त(false, c_मूलम्_दत्त, c_मूलम्_Cap);
    }

    dc_ret = deriveCap(c_मूलम्_Slot, c_मूलम्_Cap);
    यदि (dc_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम dc_ret.स्थितिः;
    }
    c_मूलम्_Cap = dc_ret.cap;

    यदि (cap_get_capType(c_मूलम्_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (v_मूलम्_दत्त != 0) {
        v_मूलम्_Cap = update_Cap_दत्त(false, v_मूलम्_दत्त, v_मूलम्_Cap);
    }

    dc_ret = deriveCap(v_मूलम्_Slot, v_मूलम्_Cap);
    यदि (dc_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम dc_ret.स्थितिः;
    }
    v_मूलम्_Cap = dc_ret.cap;

    यदि (!is_Valid_VTable_मूलम्(v_मूलम्_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* fault handler */
    यदि (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
#यदि_अन्तः

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रत्यागम invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               fhCap, fhSlot,
               cap_null_cap_new(), रिक्त,
               c_मूलम्_Cap, c_मूलम्_Slot,
               v_मूलम्_Cap, v_मूलम्_Slot,
               0, cap_null_cap_new(), रिक्त, thread_control_caps_update_space | thread_control_caps_update_fault);
#अन्यथा
    प्रत्यागम invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), slot,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_मूलम्_Cap, c_मूलम्_Slot,
               v_मूलम्_Cap, v_मूलम्_Slot,
               0, cap_null_cap_new(), रिक्त, thread_control_update_space);
#यदि_अन्तः
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    यदि (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    यदि (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    यदि (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } अन्यथा {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि ((tcb_t *)notification_ptr_get_ntfn_Queue_शीर्ष(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }


    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    यदि (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeTCB_NotificationControl(tcb, रिक्त);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    प्रत्यागम EXCEPTION_NONE;
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ exception_t installTCBCap(tcb_t *target, cap_t tCap, cte_t *slot,
                                        tcb_cnode_सूचकांक_t सूचकांक, cap_t newCap, cte_t *srcSlot)
{
    cte_t *मूलम्_Slot = TCB_PTR_CTE_PTR(target, सूचकांक);
    अप्रयुक्तम् exception_t e = cteDelete(मूलम्_Slot, true);
    यदि (e != EXCEPTION_NONE) {
        प्रत्यागम e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    यदि (cap_get_capType(newCap) != cap_null_cap) {
        यदि (sameObjectAs(newCap, srcSlot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_निवेशय(newCap, srcSlot, मूलम्_Slot);
        }
    }
    प्रत्यागम e;
}
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *target, cte_t *slot,
                                        cap_t fh_newCap, cte_t *fh_srcSlot,
                                        cap_t th_newCap, cte_t *th_srcSlot,
                                        cap_t c_मूलम्_new_Cap, cte_t *c_मूलम्_src_Slot,
                                        cap_t v_मूलम्_new_Cap, cte_t *v_मूलम्_src_Slot,
                                        word_t बफर_Addr, cap_t बफर_Cap,
                                        cte_t *बफर_Src_Slot,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    यदि (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }

    }

    यदि (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(target, tCap, slot, tcbTimeoutHandler, th_newCap, th_srcSlot);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }
    }

    यदि (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(target, tCap, slot, tcbCTable, c_मूलम्_new_Cap, c_मूलम्_src_Slot);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }

        e = installTCBCap(target, tCap, slot, tcbVTable, v_मूलम्_new_Cap, v_मूलम्_src_Slot);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }
    }

    यदि (updateFlags & thread_control_caps_update_ipc_बफर) {
        cte_t *बफर_Slot;

        बफर_Slot = TCB_PTR_CTE_PTR(target, tcb_बफर);
        e = cteDelete(बफर_Slot, true);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }
        target->tcbIPCBuffer = बफर_Addr;

        यदि (बफर_Src_Slot && sameObjectAs(बफर_Cap, बफर_Src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_निवेशय(बफर_Cap, बफर_Src_Slot, बफर_Slot);
        }

        यदि (target == ग्रन्थिः_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    प्रत्यागम EXCEPTION_NONE;
}
#अन्यथा
exception_t invokeTCB_ThreadControl(tcb_t *target, cte_t *slot,
                                    cptr_t faultep, prio_t mcp, prio_t प्राथमिकता,
                                    cap_t c_मूलम्_new_Cap, cte_t *c_मूलम्_src_Slot,
                                    cap_t v_मूलम्_new_Cap, cte_t *v_मूलम्_src_Slot,
                                    word_t बफर_Addr, cap_t बफर_Cap,
                                    cte_t *बफर_Src_Slot,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((word_t)target);

    यदि (updateFlags & thread_control_update_space) {
        target->tcbFaultHandler = faultep;
    }

    यदि (updateFlags & thread_control_update_mcp) {
        समुच्चयः_MCPriority(target, mcp);
    }

    यदि (updateFlags & thread_control_update_space) {
        cte_t *मूलम्_Slot;

        मूलम्_Slot = TCB_PTR_CTE_PTR(target, tcbCTable);
        e = cteDelete(मूलम्_Slot, true);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }
        यदि (sameObjectAs(c_मूलम्_new_Cap, c_मूलम्_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_निवेशय(c_मूलम्_new_Cap, c_मूलम्_src_Slot, मूलम्_Slot);
        }
    }

    यदि (updateFlags & thread_control_update_space) {
        cte_t *मूलम्_Slot;

        मूलम्_Slot = TCB_PTR_CTE_PTR(target, tcbVTable);
        e = cteDelete(मूलम्_Slot, true);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }
        यदि (sameObjectAs(v_मूलम्_new_Cap, v_मूलम्_src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_निवेशय(v_मूलम्_new_Cap, v_मूलम्_src_Slot, मूलम्_Slot);
        }
    }

    यदि (updateFlags & thread_control_update_ipc_बफर) {
        cte_t *बफर_Slot;

        बफर_Slot = TCB_PTR_CTE_PTR(target, tcb_बफर);
        e = cteDelete(बफर_Slot, true);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }
        target->tcbIPCBuffer = बफर_Addr;

        यदि (बफर_Src_Slot && sameObjectAs(बफर_Cap, बफर_Src_Slot->cap) &&
            sameObjectAs(tCap, slot->cap)) {
            cte_निवेशय(बफर_Cap, बफर_Src_Slot, बफर_Slot);
        }

        यदि (target == ग्रन्थिः_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    यदि (updateFlags & thread_control_update_प्राथमिकता) {
        समुच्चयः_प्राथमिकता(target, प्राथमिकता);
    }

    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *target, cte_t *slot,
                                         cap_t fh_newCap, cte_t *fh_srcSlot,
                                         prio_t mcp, prio_t प्राथमिकता,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    यदि (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((word_t)target);
        exception_t e = installTCBCap(target, tCap, slot, tcbFaultHandler, fh_newCap, fh_srcSlot);
        यदि (e != EXCEPTION_NONE) {
            प्रत्यागम e;
        }
    }

    यदि (updateFlags & thread_control_sched_update_mcp) {
        समुच्चयः_MCPriority(target, mcp);
    }

    यदि (updateFlags & thread_control_sched_update_प्राथमिकता) {
        समुच्चयः_प्राथमिकता(target, प्राथमिकता);
    }

    यदि (updateFlags & thread_control_sched_update_sc) {
        यदि (sc != रिक्त) {
            schedContext_bindTCB(sc, target);
        } अन्यथा यदि (sc == रिक्त) {
            schedContext_unbindTCB(target->tcbSchedContext);
        }
    }

    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspendSource, bool_t resumeTarget,
                                    bool_t transferFrame, bool_t transferInteger,
                                    word_t transferArch)
{
    यदि (suspendSource) {
        suspend(tcb_src);
    }

    यदि (resumeTarget) {
        restart(dest);
    }

    यदि (transferFrame) {
        word_t i;
        word_t v;
        word_t pc;

        प्रति (i = 0; i < n_frameRegisters; i++) {
            v = get_पञ्जिकाकोशः(tcb_src, frameRegisters[i]);
            समुच्चयः_पञ्जिकाकोशः(dest, frameRegisters[i], v);
        }

        pc = getRestartPC(dest);
        समुच्चयः_अग्र_PC(dest, pc);
    }

    यदि (transferInteger) {
        word_t i;
        word_t v;

        प्रति (i = 0; i < n_gpRegisters; i++) {
            v = get_पञ्जिकाकोशः(tcb_src, gpRegisters[i]);
            समुच्चयः_पञ्जिकाकोशः(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    यदि (dest == ग्रन्थिः_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    प्रत्यागम Arch_performTransfer(transferArch, tcb_src, dest);
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

    यदि (suspendSource) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, ग्रन्थिः_STATE(ksCurThread));
    यदि (e != EXCEPTION_NONE) {
        प्रत्यागम e;
    }

    यदि (call) {
        word_t *ipc_बफर;

        ipc_बफर = lookupIPCBuffer(true, thread);

        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);

        प्रति (i = 0; i < n && i < n_frameRegisters && i < n_सन्देशः_Registers; i++) {
            समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Registers[i],
                        get_पञ्जिकाकोशः(tcb_src, frameRegisters[i]));
        }

        यदि (ipc_बफर != रिक्त && i < n && i < n_frameRegisters) {
            प्रति (; i < n && i < n_frameRegisters; i++) {
                ipc_बफर[i + 1] = get_पञ्जिकाकोशः(tcb_src, frameRegisters[i]);
            }
        }

        j = i;

        प्रति (i = 0; i < n_gpRegisters && i + n_frameRegisters < n
             && i + n_frameRegisters < n_सन्देशः_Registers; i++) {
            समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Registers[i + n_frameRegisters],
                        get_पञ्जिकाकोशः(tcb_src, gpRegisters[i]));
        }

        यदि (ipc_बफर != रिक्त && i < n_gpRegisters
            && i + n_frameRegisters < n) {
            प्रति (; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
                ipc_बफर[i + n_frameRegisters + 1] =
                    get_पञ्जिकाकोशः(tcb_src, gpRegisters[i]);
            }
        }

        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, i + j)));
    }
    समुच्चयः_Thread_State(thread, ThreadState_Running);

    प्रत्यागम EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resumeTarget,
                                     word_t n, word_t arch, word_t *बफर)
{
    word_t i;
    word_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, ग्रन्थिः_STATE(ksCurThread), dest);
    यदि (e != EXCEPTION_NONE) {
        प्रत्यागम e;
    }

    यदि (n > n_frameRegisters + n_gpRegisters) {
        n = n_frameRegisters + n_gpRegisters;
    }

    archInfo = Arch_get_Sanitise_पञ्जिकाकोशः_Info(dest);

    प्रति (i = 0; i < n_frameRegisters && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        समुच्चयः_पञ्जिकाकोशः(dest, frameRegisters[i],
                    sanitise_पञ्जिकाकोशः(frameRegisters[i],
                                     getSyscallArg(i + 2, बफर), archInfo));
    }

    प्रति (i = 0; i < n_gpRegisters && i + n_frameRegisters < n; i++) {
        समुच्चयः_पञ्जिकाकोशः(dest, gpRegisters[i],
                    sanitise_पञ्जिकाकोशः(gpRegisters[i],
                                     getSyscallArg(i + n_frameRegisters + 2,
                                                   बफर), archInfo));
    }

    pc = getRestartPC(dest);
    समुच्चयः_अग्र_PC(dest, pc);

    Arch_postModifyRegisters(dest);

    यदि (resumeTarget) {
        restart(dest);
    }

    यदि (dest == ग्रन्थिः_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    प्रत्यागम EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    यदि (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } अन्यथा {
        unbindNotification(tcb);
    }

    प्रत्यागम EXCEPTION_NONE;
}

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
शून्य समुच्चयः_Thread_Name(tcb_t *tcb, ध्रुव वर्ण *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_दीर्घता);
}
#यदि_अन्तः

word_t समुच्चयः_MRs_syscall_error(tcb_t *thread, word_t *receiveIPCBuffer)
{
    विकल्प (current_syscall_error.type) {
    पक्ष se_L4_Invalid_तर्कः:
        प्रत्यागम समुच्चयः_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_तर्कः_सङ्ख्या);

    पक्ष seL4_InvalidCapability:
        प्रत्यागम समुच्चयः_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_सङ्ख्या);

    पक्ष seL4_IllegalOperation:
        प्रत्यागम 0;

    पक्ष seL4_RangeError:
        समुच्चयः_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        प्रत्यागम समुच्चयः_MR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    पक्ष seL4_AlignmentError:
        प्रत्यागम 0;

    पक्ष seL4_FailedLookup:
        समुच्चयः_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failedLookupWasSource ? 1 : 0);
        प्रत्यागम समुच्चयः_MRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    पक्ष se_L4_Truncated_सन्देशः:
    पक्ष seL4_DeleteFirst:
    पक्ष seL4_RevokeFirst:
        प्रत्यागम 0;
    पक्ष se_L4_Not_Enough_स्मृतिः:
        प्रत्यागम समुच्चयः_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.स्मृतिः_वामः);
    स्वभाव:
        fail("Invalid syscall error");
    }
}
