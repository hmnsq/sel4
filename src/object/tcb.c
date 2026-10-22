/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/invocation.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <sel4/shared_types.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <object/objecttype.h>
#అన్తర్భావ <object/cnode.h>
#నిర్వచితమైతే CONFIG_KERNEL_MCS
#అన్తర్భావ <object/schedcontext.h>
#అయితే_ముగింపు
#అన్తర్భావ <object/tcb.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <arch/smp/ipi_inline.h>

#నిర్వచించు NULL_PRIO 0

స్థిర exception_t checkPrio(prio_t prio, tcb_t *auth)
{
    prio_t mcp;

    mcp = auth->tcbMCP;

    /* system invariant: existing MCPs are bounded */
    ప్రతిజ్ఞా(mcp <= seL4_MaxPrio);

    /* can't assign a priority greater than our own mcp */
    ఒకవేళ (prio > mcp) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = seL4_MinPrio;
        current_syscall_error.rangeErrorMax = mcp;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర అంతఃవరుస శూన్య కూడు_గమ్యం_Bitmap(మాట_t cpu, మాట_t dom, మాట_t prio)
{
    మాట_t l1index;
    మాట_t l1index_inverted;

    l1index = prio_గమ్యం_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    కణుపు_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= బిట్టు(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    కణుపు_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= బిట్టు(prio & ముసుగు(మాట_Radix));
}

స్థిర అంతఃవరుస శూన్య removeFromBitmap(మాట_t cpu, మాట_t dom, మాట_t prio)
{
    మాట_t l1index;
    మాట_t l1index_inverted;

    l1index = prio_గమ్యం_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    కణుపు_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~బిట్టు(prio & ముసుగు(మాట_Radix));
    ఒకవేళ (unlikely(!కణుపు_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        కణుపు_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~బిట్టు(l1index);
    }
}

tcb_వరుస_t tcb_వరుస_remove(tcb_వరుస_t వరుస, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcb_Sched_తదుపరి;

    ఒకవేళ (వరుస.తల == tcb && వరుస.చివర == tcb) {
        వరుస.తల = ఖాళీ;
        వరుస.చివర = ఖాళీ;
    } లేకపోతే {
        ఒకవేళ (వరుస.తల == tcb) {
            after->tcbSchedPrev = ఖాళీ;
            tcb->tcb_Sched_తదుపరి = ఖాళీ;
            వరుస.తల = after;
        } లేకపోతే {
            ఒకవేళ (వరుస.చివర == tcb) {
                before->tcb_Sched_తదుపరి = ఖాళీ;
                tcb->tcbSchedPrev = ఖాళీ;
                వరుస.చివర = before;
            } లేకపోతే {
                before->tcb_Sched_తదుపరి = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = ఖాళీ;
                tcb->tcb_Sched_తదుపరి = ఖాళీ;
            }
        }
    }

    తిరిగి వరుస;
}

/* Add TCB to the head of a scheduler queue */
శూన్య tcb_Sched_వరుసలో_చేర్చు(tcb_t *tcb)
{
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ప్రతిజ్ఞా(isSchedulable(tcb));
    ప్రతిజ్ఞా(refill_sufficient(tcb->tcbSchedContext, 0));
#అయితే_ముగింపు

    ఒకవేళ (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_వరుస_t వరుస;
        dom_t dom;
        prio_t prio;
        మాట_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ప్రాధాన్యత;
        idx = ready_queues_సూచీ(dom, prio);
        వరుస = కణుపు_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ఒకవేళ (tcb_వరుస_ఖాళీలు(వరుస)) {
            కూడు_గమ్యం_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        కణుపు_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_వరుస_prepend(వరుస, tcb);

        thread_state_ptr_సమితి_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
శూన్య tcbSchedAppend(tcb_t *tcb)
{
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ప్రతిజ్ఞా(isSchedulable(tcb));
    ప్రతిజ్ఞా(refill_sufficient(tcb->tcbSchedContext, 0));
    ప్రతిజ్ఞా(refill_ready(tcb->tcbSchedContext));
#అయితే_ముగింపు
    ఒకవేళ (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_వరుస_t వరుస;
        dom_t dom;
        prio_t prio;
        మాట_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ప్రాధాన్యత;
        idx = ready_queues_సూచీ(dom, prio);
        వరుస = కణుపు_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        ఒకవేళ (tcb_వరుస_ఖాళీలు(వరుస)) {
            కూడు_గమ్యం_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        కణుపు_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_వరుస_append(వరుస, tcb);

        thread_state_ptr_సమితి_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
శూన్య tcb_Sched_వరుస_నుండి_తీయి(tcb_t *tcb)
{
    ఒకవేళ (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_వరుస_t వరుస;
        tcb_వరుస_t new_వరుస;
        dom_t dom;
        prio_t prio;
        మాట_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ప్రాధాన్యత;
        idx = ready_queues_సూచీ(dom, prio);
        వరుస = కణుపు_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_వరుస = tcb_వరుస_remove(వరుస, tcb);

        కణుపు_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_వరుస;

        thread_state_ptr_సమితి_tcb_Queued(&tcb->tcbState, false);

        ఒకవేళ (likely(tcb_వరుస_ఖాళీలు(new_వరుస))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
శూన్య tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = ఖాళీ;

    debug_tcb->tcb_Debug_తదుపరి = కణుపు_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    ఒకవేళ (కణుపు_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(కణుపు_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    కణుపు_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

శూన్య tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    ప్రతిజ్ఞా(కణుపు_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != ఖాళీ);
    ఒకవేళ (tcb == కణుపు_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        కణుపు_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(కణుపు_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcb_Debug_తదుపరి;
    } లేకపోతే {
        ప్రతిజ్ఞా(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcb_Debug_తదుపరి = debug_tcb->tcb_Debug_తదుపరి;
    }

    ఒకవేళ (debug_tcb->tcb_Debug_తదుపరి) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_తదుపరి)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = ఖాళీ;
    debug_tcb->tcb_Debug_తదుపరి = ఖాళీ;
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_వరుస_t tcbEPAppend(tcb_t *tcb, tcb_వరుస_t వరుస)
{
    ఒకవేళ (!వరుస.తల) { /* Empty list */
        వరుస.తల = tcb;
    } లేకపోతే {
        వరుస.చివర->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = వరుస.చివర;
    tcb->tcbEPNext = ఖాళీ;
    వరుస.చివర = tcb;

    తిరిగి వరుస;
}

/* Remove TCB from an endpoint queue */
tcb_వరుస_t tcbEPDequeue(tcb_t *tcb, tcb_వరుస_t వరుస)
{
    ఒకవేళ (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } లేకపోతే {
        వరుస.తల = tcb->tcbEPNext;
    }

    ఒకవేళ (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } లేకపోతే {
        వరుస.చివర = tcb->tcbEPPrev;
    }

    తిరిగి వరుస;
}
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */

#నిర్వచితమైతే CONFIG_KERNEL_MCS

శూన్య tcbReleaseRemove(tcb_t *tcb)
{
    ఒకవేళ (likely(thread_state_get_tcb_లోపల_Release_వరుస(tcb->tcbState))) {
        tcb_వరుస_t వరుస = కణుపు_STATE_ON_CORE(ks_Release_వరుస, tcb->tcbAffinity);

        ఒకవేళ (వరుస.తల == tcb) {
            కణుపు_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        కణుపు_STATE_ON_CORE(ks_Release_వరుస, tcb->tcbAffinity) = tcb_వరుస_remove(వరుస, tcb);

        thread_state_ptr_సమితి_tcb_లోపల_Release_వరుస(&tcb->tcbState, false);
    }
}

స్థిర అంతఃవరుస ticks_t శుద్ధ tcb_Ready_సమయం(tcb_t *tcb)
{
    తిరిగి refill_తల(tcb->tcbSchedContext)->r_సమయం;
}

స్థిర అంతఃవరుస bool_t శుద్ధ సమయం_after(tcb_t *tcb, ticks_t new_సమయం)
{
    తిరిగి tcb != ఖాళీ && new_సమయం >= tcb_Ready_సమయం(tcb);
}

స్థిర tcb_t *కనుగొను_సమయం_after(tcb_t *tcb, ticks_t new_సమయం)
{
    tcb_t *after = tcb;

    అయితే (సమయం_after(after, new_సమయం)) {
        after = after->tcb_Sched_తదుపరి;
    }

    తిరిగి after;
}

శూన్య tcb_Release_వరుసలో_చేర్చు(tcb_t *tcb)
{
    ప్రతిజ్ఞా(thread_state_get_tcb_లోపల_Release_వరుస(tcb->tcbState) == false);
    ప్రతిజ్ఞా(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_సమయం;
    tcb_వరుస_t వరుస;

    వరుస = కణుపు_STATE_ON_CORE(ks_Release_వరుస, tcb->tcbAffinity);
    new_సమయం = tcb_Ready_సమయం(tcb);

    ఒకవేళ (tcb_వరుస_ఖాళీలు(వరుస) || new_సమయం < tcb_Ready_సమయం(వరుస.తల)) {
        కణుపు_STATE_ON_CORE(ks_Release_వరుస, tcb->tcbAffinity) = tcb_వరుస_prepend(వరుస, tcb);
    } లేకపోతే {
        ఒకవేళ (tcb_Ready_సమయం(వరుస.చివర) <= new_సమయం) {
            కణుపు_STATE_ON_CORE(ks_Release_వరుస, tcb->tcbAffinity) = tcb_వరుస_append(వరుస, tcb);
        } లేకపోతే {
            tcb_t *after;
            after = కనుగొను_సమయం_after(వరుస.తల, new_సమయం);
            tcb_వరుస_చొప్పించు(tcb, after);
        }
    }

    thread_state_ptr_సమితి_tcb_లోపల_Release_వరుస(&tcb->tcbState, true);

    ఒకవేళ (వరుస.తల != కణుపు_STATE_ON_CORE(ks_Release_వరుస, tcb->tcbAffinity).తల) {
        కణుపు_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#అయితే_ముగింపు

cptr_t శుద్ధ getExtraCPtr(మాట_t *బఫరు_Ptr, మాట_t i)
{
    తిరిగి (cptr_t)బఫరు_Ptr[se_L4_సందేశం_Max_నిడివి + 2 + i];
}

శూన్య సమితి_Extra_Badge(మాట_t *బఫరు_Ptr, మాట_t badge,
                   మాట_t i)
{
    బఫరు_Ptr[se_L4_సందేశం_Max_నిడివి + 2 + i] = badge;
}

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
శూన్య అమరిక_Caller_Cap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *జవాబు_స్థానం, *caller_స్థానం;
    cap_t masterCap వాడని, callerCap వాడని;

    సమితి_Thread_State(sender, Thread_State_Blocked_On_జవాబు);
    జవాబు_స్థానం = TCB_PTR_CTE_PTR(sender, tcb_జవాబు);
    masterCap = జవాబు_స్థానం->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    ప్రతిజ్ఞా(cap_get_capType(masterCap) == cap_జవాబు_cap);
    ప్రతిజ్ఞా(cap_జవాబు_cap_get_cap_జవాబు_Master(masterCap));
    ప్రతిజ్ఞా(cap_జవాబు_cap_get_cap_జవాబు_Can_Grant(masterCap));
    ప్రతిజ్ఞా(TCB_PTR(cap_జవాబు_cap_get_cap_TCBPtr(masterCap)) == sender);
    caller_స్థానం = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = caller_స్థానం->cap;
    /* Haskell error: "Caller cap must not already exist" */
    ప్రతిజ్ఞా(cap_get_capType(callerCap) == cap_null_cap);
    cte_చొప్పించు(cap_జవాబు_cap_new(canGrant, false, TCB_REF(sender)),
              జవాబు_స్థానం, caller_స్థానం);
}

శూన్య deleteCallerCap(tcb_t *receiver)
{
    cte_t *caller_స్థానం;

    caller_స్థానం = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(caller_స్థానం);
}
#అయితే_ముగింపు

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, మాట_t *బఫరు_Ptr, se_L4_సందేశం_Info_t info)
{
    lookup_స్థానం_raw_ret_t lu_ret;
    cptr_t cptr;
    మాట_t i, నిడివి;

    ఒకవేళ (!బఫరు_Ptr) {
        current_extra_caps.excaprefs[0] = ఖాళీ;
        తిరిగి EXCEPTION_NONE;
    }

    నిడివి = se_L4_సందేశం_Info_get_extra_Caps(info);

    కొరకు (i = 0; i < నిడివి; i++) {
        cptr = getExtraCPtr(బఫరు_Ptr, i);

        lu_ret = lookup_స్థానం(thread, cptr);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            తిరిగి lu_ret.స్థితి_2;
        }

        current_extra_caps.excaprefs[i] = lu_ret.స్థానం;
    }
    ఒకవేళ (i < se_L4_సందేశం_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = ఖాళీ;
    }

    తిరిగి EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
మాట_t copyMRs(tcb_t *sender, మాట_t *sendBuf, tcb_t *receiver,
               మాట_t *recvBuf, మాట_t n)
{
    మాట_t i;

    /* Copy inline words */
    కొరకు (i = 0; i < n && i < n_సందేశం_Registers; i++) {
        సమితి_రిజిస్టరు(receiver, సందేశం_Registers[i],
                    get_రిజిస్టరు(sender, సందేశం_Registers[i]));
    }

    ఒకవేళ (!recvBuf || !sendBuf) {
        తిరిగి i;
    }

    /* Copy out-of-line words */
    కొరకు (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    తిరిగి i;
}

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
శూన్య remote_వరుస_Update(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    ఒకవేళ (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *లక్ష్యం_Cur_Thread = కణుపు_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        ఒకవేళ (లక్ష్యం_Cur_Thread == కణుపు_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcb_ప్రాధాన్యత > లక్ష్యం_Cur_Thread->tcb_ప్రాధాన్యత
#నిర్వచితమైతే CONFIG_KERNEL_MCS
            || కణుపు_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#అయితే_ముగింపు
           ) {
            ARCH_కణుపు_STATE(ipiReschedulePending) |= బిట్టు(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
శూన్య remoteTCBStall(tcb_t *tcb)
{

    ఒకవేళ (
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#అయితే_ముగింపు
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        కణుపు_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_కణుపు_STATE(ipiReschedulePending) |= బిట్టు(tcb->tcbAffinity);
    }
}

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
స్థిర exception_t invoke_TCB_సమితి_Affinity(tcb_t *thread, మాట_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcb_Sched_వరుస_నుండి_తీయి(thread);
    migrateTCB(thread, affinity);
    ఒకవేళ (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    ఒకవేళ (thread == కణుపు_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decode_సమితి_Affinity(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    tcb_t *tcb;
    మాట_t affinity;

    ఒకవేళ (నిడివి < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, బఫరు);
    ఒకవేళ (affinity >= ks_అంకె_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invoke_TCB_సమితి_Affinity(tcb, affinity);
}
#అయితే_ముగింపు
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
స్థిర exception_t invokeConfigureSingleStepping(bool_t call, మాట_t *బఫరు, tcb_t *t,
                                                 uint16_t bp_అంకె, మాట_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);
    మాట_t విలువ;

    bp_was_consumed = configureSingleStepping(t, bp_అంకె, n_instrs, false);
    ఒకవేళ (n_instrs == 0) {
#నిర్వచితం_కాకపోతే CONFIG_ARCH_AARCH64
        unset_Breakpoint_వాడిన_Flag(t, bp_అంకె);
#అయితే_ముగింపు /* CONFIG_ARCH_AARCH64 */
        విలువ = false;
    } లేకపోతే {
#నిర్వచితం_కాకపోతే CONFIG_ARCH_AARCH64
        సమితి_Breakpoint_వాడిన_Flag(t, bp_అంకె);
#అయితే_ముగింపు /* CONFIG_ARCH_AARCH64 */
        విలువ = bp_was_consumed;
    }

    ఒకవేళ (call) {
        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        గుర్తులేని పూర్ణాంకం నిడివి = సమితి_MR(thread, బఫరు, 0, విలువ);
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, మాట_t *బఫరు)
{
    uint16_t bp_అంకె;
    మాట_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_అంకె = getSyscallArg(0, బఫరు);
    n_instrs = getSyscallArg(1, బఫరు);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_అంకె, n_instrs, false);
    ఒకవేళ (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeConfigureSingleStepping(call, బఫరు, tcb, bp_అంకె, n_instrs);
}

స్థిర exception_t invoke_సమితి_Breakpoint(tcb_t *tcb, uint16_t bp_అంకె,
                                       మాట_t vaddr, మాట_t type, మాట_t కొలత, మాట_t rw)
{
    సమితి_Breakpoint(tcb, bp_అంకె, vaddr, type, కొలత, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    సమితి_Breakpoint_వాడిన_Flag(tcb, bp_అంకె);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decode_సమితి_Breakpoint(cap_t cap, మాట_t *బఫరు)
{
    uint16_t bp_అంకె;
    మాట_t vaddr, type, కొలత, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_అంకె = getSyscallArg(0, బఫరు);
    vaddr = getSyscallArg(1, బఫరు);
    type = getSyscallArg(2, బఫరు);
    కొలత = getSyscallArg(3, బఫరు);
    rw = getSyscallArg(4, బఫరు);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    ఒకవేళ (vaddr >= (మాట_t)USER_శిఖరం) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (type != seL4_InstructionBreakpoint && type != se_L4_దత్తాంశం_Breakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 2;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    } లేకపోతే ఒకవేళ (type == seL4_InstructionBreakpoint) {
        ఒకవేళ (కొలత != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 3;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఒకవేళ (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 4;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఒకవేళ ((se_L4_మొదటి_Watchpoint == -1 || bp_అంకె >= se_L4_మొదటి_Watchpoint)
            && se_L4_మొదటి_Breakpoint != se_L4_మొదటి_Watchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 2;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
    } లేకపోతే ఒకవేళ (type == se_L4_దత్తాంశం_Breakpoint) {
        ఒకవేళ (కొలత == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 3;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఒకవేళ (se_L4_మొదటి_Watchpoint != -1 && bp_అంకె < se_L4_మొదటి_Watchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 2;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
    } లేకపోతే ఒకవేళ (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 2;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 3;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (కొలత != 0 && కొలత != 1 && కొలత != 2 && కొలత != 4 && కొలత != 8) {
        userError("Debug: Invalid size %lu.", కొలత);
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 3;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (కొలత > 0 && vaddr & (కొలత - 1)) {
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
                  vaddr, కొలత);

        current_syscall_error.type = seL4_AlignmentError;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decode_సమితి_Breakpoint(tcb, bp_అంకె, vaddr, type, కొలత, rw);
    ఒకవేళ (error.type != seL4_NoError) {
        current_syscall_error = error;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invoke_సమితి_Breakpoint(tcb, bp_అంకె,
                               vaddr, type, కొలత, rw);
}

స్థిర exception_t invokeGetBreakpoint(bool_t call, మాట_t *బఫరు, tcb_t *tcb, uint16_t bp_అంకె)
{
    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_అంకె);
    ఒకవేళ (call) {
        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, 0, res.vaddr);
        సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, 1, res.type);
        సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, 2, res.కొలత);
        సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, 3, res.rw);
        సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, 4, res.is_enabled);
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, 5)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeGetBreakpoint(cap_t cap, bool_t call, మాట_t *బఫరు)
{
    tcb_t *tcb;
    uint16_t bp_అంకె;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_అంకె = getSyscallArg(0, బఫరు);

    error = Arch_decodeGetBreakpoint(tcb, bp_అంకె);
    ఒకవేళ (error.type != seL4_NoError) {
        current_syscall_error = error;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeGetBreakpoint(call, బఫరు, tcb, bp_అంకె);
}

స్థిర exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_అంకె)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_అంకె);
    unset_Breakpoint_వాడిన_Flag(tcb, bp_అంకె);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeUnsetBreakpoint(cap_t cap, మాట_t *బఫరు)
{
    tcb_t *tcb;
    uint16_t bp_అంకె;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_అంకె = getSyscallArg(0, బఫరు);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_అంకె);
    ఒకవేళ (error.type != seL4_NoError) {
        current_syscall_error = error;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeUnsetBreakpoint(tcb, bp_అంకె);
}
#అయితే_ముగింపు /* CONFIG_HARDWARE_DEBUG_API */

స్థిర exception_t invoke_సమితి_TLSBase(tcb_t *thread, మాట_t tls_పునాది)
{
    సమితి_రిజిస్టరు(thread, TLS_పునాది, tls_పునాది);
    ఒకవేళ (thread == కణుపు_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decode_సమితి_TLSBase(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    మాట_t tls_పునాది;

    ఒకవేళ (నిడివి < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    tls_పునాది = getSyscallArg(0, బఫరు);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invoke_సమితి_TLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_పునాది);
}

స్థిర శూన్య invoke_సమితి_Flags(tcb_t *thread, మాట_t clear, మాట_t సమితి, bool_t call)
{
    tcb_t *cur_thread = కణుపు_STATE(ksCurThread);
    మాట_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= సమితి & se_L4_TCBFlag_ముసుగు;
    thread->tcbFlags = flags;

#నిర్వచితమైతే CONFIG_HAVE_FPU
    ఒకవేళ (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } లేకపోతే ఒకవేళ (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#అయితే_ముగింపు
    ఒకవేళ (call) {
        మాట_t *ipc_బఫరు = lookupIPCBuffer(true, cur_thread);
        సమితి_రిజిస్టరు(cur_thread, badge_రిజిస్టరు, 0);
        గుర్తులేని పూర్ణాంకం నిడివి = సమితి_MR(cur_thread, ipc_బఫరు, 0, flags);
        సమితి_రిజిస్టరు(cur_thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(cur_thread, ThreadState_Running);
}

స్థిర exception_t decode_సమితి_Flags(cap_t cap, మాట_t నిడివి, bool_t call, మాట_t *బఫరు)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ఒకవేళ (నిడివి < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    మాట_t clear = getSyscallArg(0, బఫరు);
    మాట_t సమితి   = getSyscallArg(1, బఫరు);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    invoke_సమితి_Flags(thread, clear, సమితి, call);
    తిరిగి EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(మాట_t invLabel, మాట_t నిడివి, cap_t cap,
                                cte_t *స్థానం, bool_t call, మాట_t *బఫరు)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    ఎంపిక (invLabel) {
    స్థితి TCBReadRegisters:
        /* Second level of decoding */
        తిరిగి decodeReadRegisters(cap, నిడివి, call, బఫరు);

    స్థితి TCBWriteRegisters:
        తిరిగి decodeWriteRegisters(cap, నిడివి, బఫరు);

    స్థితి TCBCopyRegisters:
        తిరిగి decodeCopyRegisters(cap, నిడివి, బఫరు);

    స్థితి TCBSuspend:
        /* Jump straight to the invoke */
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    స్థితి TCBResume:
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    స్థితి TCBConfigure:
        తిరిగి decodeTCBConfigure(cap, నిడివి, స్థానం, బఫరు);

    స్థితి TCBSet_ప్రాధాన్యత:
        తిరిగి decode_సమితి_ప్రాధాన్యత(cap, నిడివి, బఫరు);

    స్థితి TCBSetMCPriority:
        తిరిగి decode_సమితి_MCPriority(cap, నిడివి, బఫరు);

    స్థితి TCBSetSchedParams:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        తిరిగి decode_సమితి_Sched_Params(cap, నిడివి, స్థానం, బఫరు);
#లేకపోతే
        తిరిగి decode_సమితి_Sched_Params(cap, నిడివి, బఫరు);
#అయితే_ముగింపు

    స్థితి TCBSetIPCBuffer:
        తిరిగి decode_సమితి_IPCBuffer(cap, నిడివి, స్థానం, బఫరు);

    స్థితి TCBSetSpace:
        తిరిగి decode_సమితి_Space(cap, నిడివి, స్థానం, బఫరు);

    స్థితి TCBBindNotification:
        తిరిగి decodeBindNotification(cap);

    స్థితి TCBUnbindNotification:
        తిరిగి decodeUnbindNotification(cap);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి TCBSetTimeoutEndpoint:
        తిరిగి decode_సమితి_Timeout_Endpoint(cap, స్థానం);
#లేకపోతే
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    స్థితి TCBSetAffinity:
        తిరిగి decode_సమితి_Affinity(cap, నిడివి, బఫరు);
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
#అయితే_ముగింపు

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#నిర్వచితమైతే CONFIG_VTX
    స్థితి TCBSetEPTRoot:
        తిరిగి decode_సమితి_EPTRoot(cap);
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    స్థితి TCBConfigureSingleStepping:
        తిరిగి decodeConfigureSingleStepping(cap, call, బఫరు);

    స్థితి TCBSetBreakpoint:
        తిరిగి decode_సమితి_Breakpoint(cap, బఫరు);

    స్థితి TCBGetBreakpoint:
        తిరిగి decodeGetBreakpoint(cap, call, బఫరు);

    స్థితి TCBUnsetBreakpoint:
        తిరిగి decodeUnsetBreakpoint(cap, బఫరు);
#అయితే_ముగింపు

    స్థితి TCBSetTLSBase:
        తిరిగి decode_సమితి_TLSBase(cap, నిడివి, బఫరు);

    స్థితి TCBSetFlags:
        తిరిగి decode_సమితి_Flags(cap, నిడివి, call, బఫరు);

    అప్రమేయ:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

గణన CopyRegistersFlags {
    Copy_Registers_suspend_ఆధారం = 0,
    Copy_Registers_resume_లక్ష్యం = 1,
    Copy_Registers_transfer_చట్రం = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    మాట_t transferArch;
    tcb_t *srcTCB;
    cap_t ఆధారం_cap;
    మాట_t flags;

    ఒకవేళ (నిడివి < 1 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, బఫరు);

    transferArch = Arch_decodeTransfer(flags >> 8);

    ఆధారం_cap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cap_get_capType(ఆధారం_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(ఆధారం_cap));
    } లేకపోతే {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & బిట్టు(Copy_Registers_suspend_ఆధారం),
               flags & బిట్టు(Copy_Registers_resume_లక్ష్యం),
               flags & బిట్టు(Copy_Registers_transfer_చట్రం),
               flags & బిట్టు(CopyRegisters_transferInteger),
               transferArch);

}

గణన ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, మాట_t నిడివి, bool_t call,
                                మాట_t *బఫరు)
{
    మాట_t transferArch, flags, n;
    tcb_t *thread;

    ఒకవేళ (నిడివి < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, బఫరు);
    n     = getSyscallArg(1, బఫరు);

    ఒకవేళ (n < 1 || n > n_చట్రం_Registers + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (పూర్ణాంకం)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_చట్రం_Registers +
                                              n_gpRegisters;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ఒకవేళ (thread == కణుపు_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & బిట్టు(ReadRegisters_suspend),
               n, transferArch, call);
}

గణన WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    మాట_t flags, w;
    మాట_t transferArch;
    tcb_t *thread;

    ఒకవేళ (నిడివి < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, బఫరు);
    w     = getSyscallArg(1, బఫరు);

    ఒకవేళ (నిడివి - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (పూర్ణాంకం)(నిడివి - 2), (పూర్ణాంకం)w);
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    ఒకవేళ (thread == కణుపు_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeTCB_WriteRegisters(thread,
                                    flags & బిట్టు(WriteRegisters_resume),
                                    w, transferArch, బఫరు);
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_endpoint_cap:
        తిరిగి (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_cap_Can_Grant_జవాబు(cap)));
    స్థితి cap_null_cap:
        /* just has no fault endpoint */
        తిరిగి true;
    అప్రమేయ:
        తిరిగి false;
    }
}
#అయితే_ముగింపు

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, మాట_t నిడివి, cte_t *స్థానం, మాట_t *బఫరు)
{
    cte_t *బఫరు_స్థానం, *c_మూలం_స్థానం, *v_మూలం_స్థానం;
    cap_t బఫరు_Cap, c_మూలం_Cap, v_మూలం_Cap;
    deriveCap_ret_t dc_ret;
    మాట_t c_మూలం_దత్తాంశం, v_మూలం_దత్తాంశం, బఫరు_Addr;
#నిర్వచితమైతే CONFIG_KERNEL_MCS
#నిర్వచించు TCBCONFIGURE_వాదనలు 3
#లేకపోతే
#నిర్వచించు TCBCONFIGURE_వాదనలు 4
#అయితే_ముగింపు
    ఒకవేళ (నిడివి < TCBCONFIGURE_వాదనలు || current_extra_caps.excaprefs[0] == ఖాళీ
        || current_extra_caps.excaprefs[1] == ఖాళీ
        || current_extra_caps.excaprefs[2] == ఖాళీ) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    c_మూలం_దత్తాంశం     = getSyscallArg(0, బఫరు);
    v_మూలం_దత్తాంశం     = getSyscallArg(1, బఫరు);
    బఫరు_Addr    = getSyscallArg(2, బఫరు);
#లేకపోతే
    cptr_t faultEP       = getSyscallArg(0, బఫరు);
    c_మూలం_దత్తాంశం     = getSyscallArg(1, బఫరు);
    v_మూలం_దత్తాంశం     = getSyscallArg(2, బఫరు);
    బఫరు_Addr    = getSyscallArg(3, బఫరు);
#అయితే_ముగింపు

    c_మూలం_స్థానం  = current_extra_caps.excaprefs[0];
    c_మూలం_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_మూలం_స్థానం  = current_extra_caps.excaprefs[1];
    v_మూలం_Cap   = current_extra_caps.excaprefs[1]->cap;
    బఫరు_స్థానం = current_extra_caps.excaprefs[2];
    బఫరు_Cap  = current_extra_caps.excaprefs[2]->cap;

    ఒకవేళ (బఫరు_Addr == 0) {
        బఫరు_స్థానం = ఖాళీ;
    } లేకపోతే {
        dc_ret = deriveCap(బఫరు_స్థానం, బఫరు_Cap);
        ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి dc_ret.స్థితి_2;
        }
        బఫరు_Cap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(బఫరు_Addr, బఫరు_Cap);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }
    }

    ఒకవేళ (స్థానం_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        స్థానం_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (c_మూలం_దత్తాంశం != 0) {
        c_మూలం_Cap = update_Cap_దత్తాంశం(false, c_మూలం_దత్తాంశం, c_మూలం_Cap);
    }

    dc_ret = deriveCap(c_మూలం_స్థానం, c_మూలం_Cap);
    ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి dc_ret.స్థితి_2;
    }
    c_మూలం_Cap = dc_ret.cap;

    ఒకవేళ (cap_get_capType(c_మూలం_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (v_మూలం_దత్తాంశం != 0) {
        v_మూలం_Cap = update_Cap_దత్తాంశం(false, v_మూలం_దత్తాంశం, v_మూలం_Cap);
    }

    dc_ret = deriveCap(v_మూలం_స్థానం, v_మూలం_Cap);
    ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి dc_ret.స్థితి_2;
    }
    v_మూలం_Cap = dc_ret.cap;

    ఒకవేళ (!is_Valid_VTable_మూలం(v_మూలం_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    తిరిగి invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), స్థానం,
               cap_null_cap_new(), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               c_మూలం_Cap, c_మూలం_స్థానం,
               v_మూలం_Cap, v_మూలం_స్థానం,
               బఫరు_Addr, బఫరు_Cap,
               బఫరు_స్థానం, thread_control_caps_update_space |
               thread_control_caps_update_ipc_బఫరు);
#లేకపోతే
    తిరిగి invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), స్థానం,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_మూలం_Cap, c_మూలం_స్థానం,
               v_మూలం_Cap, v_మూలం_స్థానం,
               బఫరు_Addr, బఫరు_Cap,
               బఫరు_స్థానం, thread_control_update_space |
               thread_control_update_ipc_బఫరు);
#అయితే_ముగింపు
}

exception_t decode_సమితి_ప్రాధాన్యత(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    ఒకవేళ (నిడివి < 1 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, బఫరు);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t స్థితి_2 = checkPrio(newPrio, authTCB);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (గుర్తులేని దీర్ఘ) newPrio, (గుర్తులేని దీర్ఘ) authTCB->tcbMCP);
        తిరిగి స్థితి_2;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    తిరిగి invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               NULL_PRIO, newPrio,
               ఖాళీ, thread_control_sched_update_ప్రాధాన్యత);
#లేకపోతే
    తిరిగి invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ఖాళీ,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               0, cap_null_cap_new(),
               ఖాళీ, thread_control_update_ప్రాధాన్యత);
#అయితే_ముగింపు
}

exception_t decode_సమితి_MCPriority(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    ఒకవేళ (నిడివి < 1 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, బఫరు);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t స్థితి_2 = checkPrio(newMcp, authTCB);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (గుర్తులేని దీర్ఘ) newMcp, (గుర్తులేని దీర్ఘ) authTCB->tcbMCP);
        తిరిగి స్థితి_2;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    తిరిగి invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               newMcp, NULL_PRIO,
               ఖాళీ, thread_control_sched_update_mcp);
#లేకపోతే
    తిరిగి invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ఖాళీ,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               0, cap_null_cap_new(),
               ఖాళీ, thread_control_update_mcp);
#అయితే_ముగింపు
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
exception_t decode_సమితి_Timeout_Endpoint(cap_t cap, cte_t *స్థానం)
{
    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *th_స్థానం = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    ఒకవేళ (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), స్థానం,
               cap_null_cap_new(), ఖాళీ,
               thCap, th_స్థానం,
               cap_null_cap_new(), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               0, cap_null_cap_new(), ఖాళీ,
               thread_control_caps_update_timeout);
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_KERNEL_MCS
exception_t decode_సమితి_Sched_Params(cap_t cap, మాట_t నిడివి, cte_t *స్థానం, మాట_t *బఫరు)
#లేకపోతే
exception_t decode_సమితి_Sched_Params(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
#అయితే_ముగింపు
{
    ఒకవేళ (నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == ఖాళీ || current_extra_caps.excaprefs[2] == ఖాళీ
#అయితే_ముగింపు
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, బఫరు);
    prio_t newPrio = getSyscallArg(1, బఫరు);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fh_స్థానం = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#అయితే_ముగింపు

    ఒకవేళ (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t స్థితి_2 = checkPrio(newMcp, authTCB);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (గుర్తులేని దీర్ఘ) newMcp, (గుర్తులేని దీర్ఘ) authTCB->tcbMCP);
        తిరిగి స్థితి_2;
    }

    స్థితి_2 = checkPrio(newPrio, authTCB);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (గుర్తులేని దీర్ఘ) newPrio, (గుర్తులేని దీర్ఘ) authTCB->tcbMCP);
        తిరిగి స్థితి_2;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = ఖాళీ;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_ప్రాధాన్యత |
                                         thread_control_sched_update_fault;
    ఎంపిక (cap_get_capType(scCap)) {
    స్థితి cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        ఒకవేళ (tcb->tcbSchedContext != sc) {
            ఒకవేళ (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
            ఒకవేళ (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        }
        ఒకవేళ (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఆపు;
    స్థితి cap_null_cap:
        ఒకవేళ (tcb == కణుపు_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఆపు;
    అప్రమేయ:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 2;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    ఒకవేళ (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    ఒకవేళ (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 3;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
#అయితే_ముగింపు
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    తిరిగి invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), స్థానం,
               fhCap, fh_స్థానం,
               newMcp, newPrio,
               sc, update_flags);
#లేకపోతే
    తిరిగి invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), ఖాళీ,
               0, newMcp, newPrio,
               cap_null_cap_new(), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               0, cap_null_cap_new(),
               ఖాళీ, thread_control_update_mcp |
               thread_control_update_ప్రాధాన్యత);
#అయితే_ముగింపు
}


exception_t decode_సమితి_IPCBuffer(cap_t cap, మాట_t నిడివి, cte_t *స్థానం, మాట_t *బఫరు)
{
    cptr_t cptr_బఫరు_Ptr;
    cap_t బఫరు_Cap;
    cte_t *బఫరు_స్థానం;

    ఒకవేళ (నిడివి < 1 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    cptr_బఫరు_Ptr  = getSyscallArg(0, బఫరు);
    బఫరు_స్థానం = current_extra_caps.excaprefs[0];
    బఫరు_Cap  = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cptr_బఫరు_Ptr == 0) {
        బఫరు_స్థానం = ఖాళీ;
    } లేకపోతే {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(బఫరు_స్థానం, బఫరు_Cap);
        ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి dc_ret.స్థితి_2;
        }
        బఫరు_Cap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_బఫరు_Ptr, బఫరు_Cap);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    తిరిగి invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), స్థానం,
               cap_null_cap_new(), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               cptr_బఫరు_Ptr, బఫరు_Cap,
               బఫరు_స్థానం, thread_control_caps_update_ipc_బఫరు);
#లేకపోతే
    తిరిగి invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), స్థానం,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), ఖాళీ,
               cap_null_cap_new(), ఖాళీ,
               cptr_బఫరు_Ptr, బఫరు_Cap,
               బఫరు_స్థానం, thread_control_update_ipc_బఫరు);

#అయితే_ముగింపు
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
#నిర్వచించు DECODE_సమితి_SPACE_PARAMS 2
#లేకపోతే
#నిర్వచించు DECODE_సమితి_SPACE_PARAMS 3
#అయితే_ముగింపు
exception_t decode_సమితి_Space(cap_t cap, మాట_t నిడివి, cte_t *స్థానం, మాట_t *బఫరు)
{
    మాట_t c_మూలం_దత్తాంశం, v_మూలం_దత్తాంశం;
    cte_t *c_మూలం_స్థానం, *v_మూలం_స్థానం;
    cap_t c_మూలం_Cap, v_మూలం_Cap;
    deriveCap_ret_t dc_ret;

    ఒకవేళ (నిడివి < DECODE_సమితి_SPACE_PARAMS || current_extra_caps.excaprefs[0] == ఖాళీ
        || current_extra_caps.excaprefs[1] == ఖాళీ
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == ఖాళీ
#అయితే_ముగింపు
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    c_మూలం_దత్తాంశం = getSyscallArg(0, బఫరు);
    v_మూలం_దత్తాంశం = getSyscallArg(1, బఫరు);

    cte_t *fh_స్థానం     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_మూలం_స్థానం  = current_extra_caps.excaprefs[1];
    c_మూలం_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_మూలం_స్థానం  = current_extra_caps.excaprefs[2];
    v_మూలం_Cap   = current_extra_caps.excaprefs[2]->cap;
#లేకపోతే
    cptr_t faultEP   = getSyscallArg(0, బఫరు);
    c_మూలం_దత్తాంశం = getSyscallArg(1, బఫరు);
    v_మూలం_దత్తాంశం = getSyscallArg(2, బఫరు);

    c_మూలం_స్థానం  = current_extra_caps.excaprefs[0];
    c_మూలం_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_మూలం_స్థానం  = current_extra_caps.excaprefs[1];
    v_మూలం_Cap   = current_extra_caps.excaprefs[1]->cap;
#అయితే_ముగింపు

    ఒకవేళ (స్థానం_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        స్థానం_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (c_మూలం_దత్తాంశం != 0) {
        c_మూలం_Cap = update_Cap_దత్తాంశం(false, c_మూలం_దత్తాంశం, c_మూలం_Cap);
    }

    dc_ret = deriveCap(c_మూలం_స్థానం, c_మూలం_Cap);
    ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి dc_ret.స్థితి_2;
    }
    c_మూలం_Cap = dc_ret.cap;

    ఒకవేళ (cap_get_capType(c_మూలం_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (v_మూలం_దత్తాంశం != 0) {
        v_మూలం_Cap = update_Cap_దత్తాంశం(false, v_మూలం_దత్తాంశం, v_మూలం_Cap);
    }

    dc_ret = deriveCap(v_మూలం_స్థానం, v_మూలం_Cap);
    ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి dc_ret.స్థితి_2;
    }
    v_మూలం_Cap = dc_ret.cap;

    ఒకవేళ (!is_Valid_VTable_మూలం(v_మూలం_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* fault handler */
    ఒకవేళ (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
#అయితే_ముగింపు

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    తిరిగి invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), స్థానం,
               fhCap, fh_స్థానం,
               cap_null_cap_new(), ఖాళీ,
               c_మూలం_Cap, c_మూలం_స్థానం,
               v_మూలం_Cap, v_మూలం_స్థానం,
               0, cap_null_cap_new(), ఖాళీ, thread_control_caps_update_space | thread_control_caps_update_fault);
#లేకపోతే
    తిరిగి invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), స్థానం,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_మూలం_Cap, c_మూలం_స్థానం,
               v_మూలం_Cap, v_మూలం_స్థానం,
               0, cap_null_cap_new(), ఖాళీ, thread_control_update_space);
#అయితే_ముగింపు
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ఒకవేళ (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } లేకపోతే {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ ((tcb_t *)notification_ptr_get_ntfn_వరుస_తల(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }


    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    ఒకవేళ (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeTCB_NotificationControl(tcb, ఖాళీ);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    తిరిగి EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
స్థిర అంతఃవరుస exception_t installTCBCap(tcb_t *లక్ష్యం, cap_t tCap, cte_t *స్థానం,
                                        tcb_cnode_సూచీ_t సూచీ, cap_t newCap, cte_t *src_స్థానం)
{
    cte_t *మూలం_స్థానం = TCB_PTR_CTE_PTR(లక్ష్యం, సూచీ);
    వాడని exception_t e = cteDelete(మూలం_స్థానం, true);
    ఒకవేళ (e != EXCEPTION_NONE) {
        తిరిగి e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    ఒకవేళ (cap_get_capType(newCap) != cap_null_cap) {
        ఒకవేళ (sameObjectAs(newCap, src_స్థానం->cap) &&
            sameObjectAs(tCap, స్థానం->cap)) {
            cte_చొప్పించు(newCap, src_స్థానం, మూలం_స్థానం);
        }
    }
    తిరిగి e;
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *లక్ష్యం, cte_t *స్థానం,
                                        cap_t fh_newCap, cte_t *fh_src_స్థానం,
                                        cap_t th_newCap, cte_t *th_src_స్థానం,
                                        cap_t c_మూలం_new_Cap, cte_t *c_మూలం_src_స్థానం,
                                        cap_t v_మూలం_new_Cap, cte_t *v_మూలం_src_స్థానం,
                                        మాట_t బఫరు_Addr, cap_t బఫరు_Cap,
                                        cte_t *బఫరు_Src_స్థానం,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((మాట_t)లక్ష్యం);

    ఒకవేళ (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(లక్ష్యం, tCap, స్థానం, tcbFaultHandler, fh_newCap, fh_src_స్థానం);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }

    }

    ఒకవేళ (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(లక్ష్యం, tCap, స్థానం, tcbTimeoutHandler, th_newCap, th_src_స్థానం);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }
    }

    ఒకవేళ (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(లక్ష్యం, tCap, స్థానం, tcbCTable, c_మూలం_new_Cap, c_మూలం_src_స్థానం);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }

        e = installTCBCap(లక్ష్యం, tCap, స్థానం, tcbVTable, v_మూలం_new_Cap, v_మూలం_src_స్థానం);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }
    }

    ఒకవేళ (updateFlags & thread_control_caps_update_ipc_బఫరు) {
        cte_t *బఫరు_స్థానం;

        బఫరు_స్థానం = TCB_PTR_CTE_PTR(లక్ష్యం, tcb_బఫరు);
        e = cteDelete(బఫరు_స్థానం, true);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }
        లక్ష్యం->tcbIPCBuffer = బఫరు_Addr;

        ఒకవేళ (బఫరు_Src_స్థానం && sameObjectAs(బఫరు_Cap, బఫరు_Src_స్థానం->cap) &&
            sameObjectAs(tCap, స్థానం->cap)) {
            cte_చొప్పించు(బఫరు_Cap, బఫరు_Src_స్థానం, బఫరు_స్థానం);
        }

        ఒకవేళ (లక్ష్యం == కణుపు_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    తిరిగి EXCEPTION_NONE;
}
#లేకపోతే
exception_t invokeTCB_ThreadControl(tcb_t *లక్ష్యం, cte_t *స్థానం,
                                    cptr_t faultep, prio_t mcp, prio_t ప్రాధాన్యత,
                                    cap_t c_మూలం_new_Cap, cte_t *c_మూలం_src_స్థానం,
                                    cap_t v_మూలం_new_Cap, cte_t *v_మూలం_src_స్థానం,
                                    మాట_t బఫరు_Addr, cap_t బఫరు_Cap,
                                    cte_t *బఫరు_Src_స్థానం,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((మాట_t)లక్ష్యం);

    ఒకవేళ (updateFlags & thread_control_update_space) {
        లక్ష్యం->tcbFaultHandler = faultep;
    }

    ఒకవేళ (updateFlags & thread_control_update_mcp) {
        సమితి_MCPriority(లక్ష్యం, mcp);
    }

    ఒకవేళ (updateFlags & thread_control_update_space) {
        cte_t *మూలం_స్థానం;

        మూలం_స్థానం = TCB_PTR_CTE_PTR(లక్ష్యం, tcbCTable);
        e = cteDelete(మూలం_స్థానం, true);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }
        ఒకవేళ (sameObjectAs(c_మూలం_new_Cap, c_మూలం_src_స్థానం->cap) &&
            sameObjectAs(tCap, స్థానం->cap)) {
            cte_చొప్పించు(c_మూలం_new_Cap, c_మూలం_src_స్థానం, మూలం_స్థానం);
        }
    }

    ఒకవేళ (updateFlags & thread_control_update_space) {
        cte_t *మూలం_స్థానం;

        మూలం_స్థానం = TCB_PTR_CTE_PTR(లక్ష్యం, tcbVTable);
        e = cteDelete(మూలం_స్థానం, true);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }
        ఒకవేళ (sameObjectAs(v_మూలం_new_Cap, v_మూలం_src_స్థానం->cap) &&
            sameObjectAs(tCap, స్థానం->cap)) {
            cte_చొప్పించు(v_మూలం_new_Cap, v_మూలం_src_స్థానం, మూలం_స్థానం);
        }
    }

    ఒకవేళ (updateFlags & thread_control_update_ipc_బఫరు) {
        cte_t *బఫరు_స్థానం;

        బఫరు_స్థానం = TCB_PTR_CTE_PTR(లక్ష్యం, tcb_బఫరు);
        e = cteDelete(బఫరు_స్థానం, true);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }
        లక్ష్యం->tcbIPCBuffer = బఫరు_Addr;

        ఒకవేళ (బఫరు_Src_స్థానం && sameObjectAs(బఫరు_Cap, బఫరు_Src_స్థానం->cap) &&
            sameObjectAs(tCap, స్థానం->cap)) {
            cte_చొప్పించు(బఫరు_Cap, బఫరు_Src_స్థానం, బఫరు_స్థానం);
        }

        ఒకవేళ (లక్ష్యం == కణుపు_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ఒకవేళ (updateFlags & thread_control_update_ప్రాధాన్యత) {
        సమితి_ప్రాధాన్యత(లక్ష్యం, ప్రాధాన్యత);
    }

    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *లక్ష్యం, cte_t *స్థానం,
                                         cap_t fh_newCap, cte_t *fh_src_స్థానం,
                                         prio_t mcp, prio_t ప్రాధాన్యత,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    ఒకవేళ (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((మాట_t)లక్ష్యం);
        exception_t e = installTCBCap(లక్ష్యం, tCap, స్థానం, tcbFaultHandler, fh_newCap, fh_src_స్థానం);
        ఒకవేళ (e != EXCEPTION_NONE) {
            తిరిగి e;
        }
    }

    ఒకవేళ (updateFlags & thread_control_sched_update_mcp) {
        సమితి_MCPriority(లక్ష్యం, mcp);
    }

    ఒకవేళ (updateFlags & thread_control_sched_update_ప్రాధాన్యత) {
        సమితి_ప్రాధాన్యత(లక్ష్యం, ప్రాధాన్యత);
    }

    ఒకవేళ (updateFlags & thread_control_sched_update_sc) {
        ఒకవేళ (sc != ఖాళీ) {
            schedContext_bindTCB(sc, లక్ష్యం);
        } లేకపోతే ఒకవేళ (sc == ఖాళీ) {
            schedContext_unbindTCB(లక్ష్యం->tcbSchedContext);
        }
    }

    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspend_ఆధారం, bool_t resume_లక్ష్యం,
                                    bool_t transfer_చట్రం, bool_t transferInteger,
                                    మాట_t transferArch)
{
    ఒకవేళ (suspend_ఆధారం) {
        suspend(tcb_src);
    }

    ఒకవేళ (resume_లక్ష్యం) {
        restart(dest);
    }

    ఒకవేళ (transfer_చట్రం) {
        మాట_t i;
        మాట_t v;
        మాట_t pc;

        కొరకు (i = 0; i < n_చట్రం_Registers; i++) {
            v = get_రిజిస్టరు(tcb_src, చట్రం_Registers[i]);
            సమితి_రిజిస్టరు(dest, చట్రం_Registers[i], v);
        }

        pc = getRestartPC(dest);
        సమితి_తదుపరి_PC(dest, pc);
    }

    ఒకవేళ (transferInteger) {
        మాట_t i;
        మాట_t v;

        కొరకు (i = 0; i < n_gpRegisters; i++) {
            v = get_రిజిస్టరు(tcb_src, gpRegisters[i]);
            సమితి_రిజిస్టరు(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    ఒకవేళ (dest == కణుపు_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    తిరిగి Arch_performTransfer(transferArch, tcb_src, dest);
}

/* ReadRegisters is a special case: replyFromKernel & setMRs are
 * unfolded here, in order to avoid passing the large reply message up
 * to the top level in a global (and double-copying). We prevent the
 * top-level replyFromKernel_success_empty() from running by setting the
 * thread state. Retype does this too.
 */
exception_t invokeTCB_ReadRegisters(tcb_t *tcb_src, bool_t suspend_ఆధారం,
                                    మాట_t n, మాట_t arch, bool_t call)
{
    మాట_t i, j;
    exception_t e;
    tcb_t *thread;

    thread = కణుపు_STATE(ksCurThread);

    ఒకవేళ (suspend_ఆధారం) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, కణుపు_STATE(ksCurThread));
    ఒకవేళ (e != EXCEPTION_NONE) {
        తిరిగి e;
    }

    ఒకవేళ (call) {
        మాట_t *ipc_బఫరు;

        ipc_బఫరు = lookupIPCBuffer(true, thread);

        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);

        కొరకు (i = 0; i < n && i < n_చట్రం_Registers && i < n_సందేశం_Registers; i++) {
            సమితి_రిజిస్టరు(thread, సందేశం_Registers[i],
                        get_రిజిస్టరు(tcb_src, చట్రం_Registers[i]));
        }

        ఒకవేళ (ipc_బఫరు != ఖాళీ && i < n && i < n_చట్రం_Registers) {
            కొరకు (; i < n && i < n_చట్రం_Registers; i++) {
                ipc_బఫరు[i + 1] = get_రిజిస్టరు(tcb_src, చట్రం_Registers[i]);
            }
        }

        j = i;

        కొరకు (i = 0; i < n_gpRegisters && i + n_చట్రం_Registers < n
             && i + n_చట్రం_Registers < n_సందేశం_Registers; i++) {
            సమితి_రిజిస్టరు(thread, సందేశం_Registers[i + n_చట్రం_Registers],
                        get_రిజిస్టరు(tcb_src, gpRegisters[i]));
        }

        ఒకవేళ (ipc_బఫరు != ఖాళీ && i < n_gpRegisters
            && i + n_చట్రం_Registers < n) {
            కొరకు (; i < n_gpRegisters && i + n_చట్రం_Registers < n; i++) {
                ipc_బఫరు[i + n_చట్రం_Registers + 1] =
                    get_రిజిస్టరు(tcb_src, gpRegisters[i]);
            }
        }

        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, i + j)));
    }
    సమితి_Thread_State(thread, ThreadState_Running);

    తిరిగి EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resume_లక్ష్యం,
                                     మాట_t n, మాట_t arch, మాట_t *బఫరు)
{
    మాట_t i;
    మాట_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, కణుపు_STATE(ksCurThread), dest);
    ఒకవేళ (e != EXCEPTION_NONE) {
        తిరిగి e;
    }

    ఒకవేళ (n > n_చట్రం_Registers + n_gpRegisters) {
        n = n_చట్రం_Registers + n_gpRegisters;
    }

    archInfo = Arch_get_Sanitise_రిజిస్టరు_Info(dest);

    కొరకు (i = 0; i < n_చట్రం_Registers && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        సమితి_రిజిస్టరు(dest, చట్రం_Registers[i],
                    sanitise_రిజిస్టరు(చట్రం_Registers[i],
                                     getSyscallArg(i + 2, బఫరు), archInfo));
    }

    కొరకు (i = 0; i < n_gpRegisters && i + n_చట్రం_Registers < n; i++) {
        సమితి_రిజిస్టరు(dest, gpRegisters[i],
                    sanitise_రిజిస్టరు(gpRegisters[i],
                                     getSyscallArg(i + n_చట్రం_Registers + 2,
                                                   బఫరు), archInfo));
    }

    pc = getRestartPC(dest);
    సమితి_తదుపరి_PC(dest, pc);

    Arch_postModifyRegisters(dest);

    ఒకవేళ (resume_లక్ష్యం) {
        restart(dest);
    }

    ఒకవేళ (dest == కణుపు_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    తిరిగి EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    ఒకవేళ (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } లేకపోతే {
        unbindNotification(tcb);
    }

    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
శూన్య సమితి_Thread_Name(tcb_t *tcb, స్థిరరాశి అక్షరం *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_నిడివి);
}
#అయితే_ముగింపు

మాట_t సమితి_MRs_syscall_error(tcb_t *thread, మాట_t *receiveIPCBuffer)
{
    ఎంపిక (current_syscall_error.type) {
    స్థితి se_L4_Invalid_వాదం:
        తిరిగి సమితి_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_వాదం_సంఖ్య);

    స్థితి seL4_InvalidCapability:
        తిరిగి సమితి_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_సంఖ్య);

    స్థితి seL4_IllegalOperation:
        తిరిగి 0;

    స్థితి seL4_RangeError:
        సమితి_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        తిరిగి సమితి_MR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    స్థితి seL4_AlignmentError:
        తిరిగి 0;

    స్థితి seL4_FailedLookup:
        సమితి_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failed_Lookup_Was_ఆధారం ? 1 : 0);
        తిరిగి సమితి_MRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    స్థితి se_L4_Truncated_సందేశం:
    స్థితి se_L4_Delete_మొదటి:
    స్థితి se_L4_Revoke_మొదటి:
        తిరిగి 0;
    స్థితి se_L4_Not_Enough_జ్ఞాపకం:
        తిరిగి సమితి_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.జ్ఞాపకం_ఎడమ);
    అప్రమేయ:
        fail("Invalid syscall error");
    }
}
