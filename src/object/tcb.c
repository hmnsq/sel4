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
#که_تعریف_شوی CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#د_که_پای
#ضمن <object/tcb.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <model/statedata.h>
#ضمن <util.h>
#ضمن <string.h>
#ضمن <stdint.h>
#ضمن <arch/smp/ipi_inline.h>

#تعریف_کړه NULL_PRIO 0

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

ساكن مضمن فراغ جمع_کړه_منزل_Bitmap(کلمه_t cpu, کلمه_t dom, کلمه_t prio)
{
    کلمه_t l1index;
    کلمه_t l1index_inverted;

    l1index = prio_منزل_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    غوټه_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= بټ(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    غوټه_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= بټ(prio & ماسک(کلمه_Radix));
}

ساكن مضمن فراغ removeFromBitmap(کلمه_t cpu, کلمه_t dom, کلمه_t prio)
{
    کلمه_t l1index;
    کلمه_t l1index_inverted;

    l1index = prio_منزل_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    غوټه_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~بټ(prio & ماسک(کلمه_Radix));
    إذا (unlikely(!غوټه_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        غوټه_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~بټ(l1index);
    }
}

tcb_کتار_t tcb_کتار_remove(tcb_کتار_t کتار, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcb_Sched_بل;

    إذا (کتار.سر == tcb && کتار.پای_ټکی == tcb) {
        کتار.سر = عدم;
        کتار.پای_ټکی = عدم;
    } وإلا {
        إذا (کتار.سر == tcb) {
            after->tcbSchedPrev = عدم;
            tcb->tcb_Sched_بل = عدم;
            کتار.سر = after;
        } وإلا {
            إذا (کتار.پای_ټکی == tcb) {
                before->tcb_Sched_بل = عدم;
                tcb->tcbSchedPrev = عدم;
                کتار.پای_ټکی = before;
            } وإلا {
                before->tcb_Sched_بل = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = عدم;
                tcb->tcb_Sched_بل = عدم;
            }
        }
    }

    أرجع کتار;
}

/* Add TCB to the head of a scheduler queue */
فراغ tcb_Sched_کتار_ته_ورزیاتول(tcb_t *tcb)
{
#که_تعریف_شوی CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
#د_که_پای

    إذا (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_کتار_t کتار;
        dom_t dom;
        prio_t prio;
        کلمه_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_لومړیتوب;
        idx = ready_queues_شاخص(dom, prio);
        کتار = غوټه_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        إذا (tcb_کتار_تشه(کتار)) {
            جمع_کړه_منزل_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        غوټه_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_کتار_prepend(کتار, tcb);

        thread_state_ptr_سېټ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
فراغ tcbSchedAppend(tcb_t *tcb)
{
#که_تعریف_شوی CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
    أكد(refill_ready(tcb->tcbSchedContext));
#د_که_پای
    إذا (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_کتار_t کتار;
        dom_t dom;
        prio_t prio;
        کلمه_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_لومړیتوب;
        idx = ready_queues_شاخص(dom, prio);
        کتار = غوټه_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        إذا (tcb_کتار_تشه(کتار)) {
            جمع_کړه_منزل_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        غوټه_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_کتار_append(کتار, tcb);

        thread_state_ptr_سېټ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
فراغ tcb_Sched_له_کتار_ایستل(tcb_t *tcb)
{
    إذا (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_کتار_t کتار;
        tcb_کتار_t new_کتار;
        dom_t dom;
        prio_t prio;
        کلمه_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_لومړیتوب;
        idx = ready_queues_شاخص(dom, prio);
        کتار = غوټه_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_کتار = tcb_کتار_remove(کتار, tcb);

        غوټه_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_کتار;

        thread_state_ptr_سېټ_tcb_Queued(&tcb->tcbState, false);

        إذا (likely(tcb_کتار_تشه(new_کتار))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#که_تعریف_شوی CONFIG_DEBUG_BUILD
فراغ tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = عدم;

    debug_tcb->tcb_Debug_بل = غوټه_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    إذا (غوټه_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(غوټه_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    غوټه_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

فراغ tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    أكد(غوټه_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != عدم);
    إذا (tcb == غوټه_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        غوټه_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(غوټه_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcb_Debug_بل;
    } وإلا {
        أكد(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcb_Debug_بل = debug_tcb->tcb_Debug_بل;
    }

    إذا (debug_tcb->tcb_Debug_بل) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_بل)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = عدم;
    debug_tcb->tcb_Debug_بل = عدم;
}
#د_که_پای /* CONFIG_DEBUG_BUILD */

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_کتار_t tcbEPAppend(tcb_t *tcb, tcb_کتار_t کتار)
{
    إذا (!کتار.سر) { /* Empty list */
        کتار.سر = tcb;
    } وإلا {
        کتار.پای_ټکی->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = کتار.پای_ټکی;
    tcb->tcbEPNext = عدم;
    کتار.پای_ټکی = tcb;

    أرجع کتار;
}

/* Remove TCB from an endpoint queue */
tcb_کتار_t tcbEPDequeue(tcb_t *tcb, tcb_کتار_t کتار)
{
    إذا (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } وإلا {
        کتار.سر = tcb->tcbEPNext;
    }

    إذا (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } وإلا {
        کتار.پای_ټکی = tcb->tcbEPPrev;
    }

    أرجع کتار;
}
#د_که_پای /* CONFIG_KERNEL_MCS */

#که_تعریف_شوی CONFIG_KERNEL_MCS

فراغ tcbReleaseRemove(tcb_t *tcb)
{
    إذا (likely(thread_state_get_tcb_دننه_Release_کتار(tcb->tcbState))) {
        tcb_کتار_t کتار = غوټه_STATE_ON_CORE(ks_Release_کتار, tcb->tcbAffinity);

        إذا (کتار.سر == tcb) {
            غوټه_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        غوټه_STATE_ON_CORE(ks_Release_کتار, tcb->tcbAffinity) = tcb_کتار_remove(کتار, tcb);

        thread_state_ptr_سېټ_tcb_دننه_Release_کتار(&tcb->tcbState, false);
    }
}

ساكن مضمن ticks_t خالص tcb_Ready_وخت(tcb_t *tcb)
{
    أرجع refill_سر(tcb->tcbSchedContext)->r_وخت;
}

ساكن مضمن bool_t خالص وخت_after(tcb_t *tcb, ticks_t new_وخت)
{
    أرجع tcb != عدم && new_وخت >= tcb_Ready_وخت(tcb);
}

ساكن tcb_t *پیدا_کړه_وخت_after(tcb_t *tcb, ticks_t new_وخت)
{
    tcb_t *after = tcb;

    طالما (وخت_after(after, new_وخت)) {
        after = after->tcb_Sched_بل;
    }

    أرجع after;
}

فراغ tcb_Release_کتار_ته_ورزیاتول(tcb_t *tcb)
{
    أكد(thread_state_get_tcb_دننه_Release_کتار(tcb->tcbState) == false);
    أكد(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_وخت;
    tcb_کتار_t کتار;

    کتار = غوټه_STATE_ON_CORE(ks_Release_کتار, tcb->tcbAffinity);
    new_وخت = tcb_Ready_وخت(tcb);

    إذا (tcb_کتار_تشه(کتار) || new_وخت < tcb_Ready_وخت(کتار.سر)) {
        غوټه_STATE_ON_CORE(ks_Release_کتار, tcb->tcbAffinity) = tcb_کتار_prepend(کتار, tcb);
    } وإلا {
        إذا (tcb_Ready_وخت(کتار.پای_ټکی) <= new_وخت) {
            غوټه_STATE_ON_CORE(ks_Release_کتار, tcb->tcbAffinity) = tcb_کتار_append(کتار, tcb);
        } وإلا {
            tcb_t *after;
            after = پیدا_کړه_وخت_after(کتار.سر, new_وخت);
            tcb_کتار_ورننباسه(tcb, after);
        }
    }

    thread_state_ptr_سېټ_tcb_دننه_Release_کتار(&tcb->tcbState, true);

    إذا (کتار.سر != غوټه_STATE_ON_CORE(ks_Release_کتار, tcb->tcbAffinity).سر) {
        غوټه_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#د_که_پای

cptr_t خالص getExtraCPtr(کلمه_t *بفر_Ptr, کلمه_t i)
{
    أرجع (cptr_t)بفر_Ptr[se_L4_پیغام_Max_اوږدتیا + 2 + i];
}

فراغ سېټ_Extra_Badge(کلمه_t *بفر_Ptr, کلمه_t badge,
                   کلمه_t i)
{
    بفر_Ptr[se_L4_پیغام_Max_اوږدتیا + 2 + i] = badge;
}

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
فراغ تیاری_Caller_Cap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *ځواب_خونه, *caller_خونه;
    cap_t masterCap ناکارول_شوی, callerCap ناکارول_شوی;

    سېټ_Thread_State(sender, Thread_State_Blocked_On_ځواب);
    ځواب_خونه = TCB_PTR_CTE_PTR(sender, tcb_ځواب);
    masterCap = ځواب_خونه->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    أكد(cap_get_capType(masterCap) == cap_ځواب_cap);
    أكد(cap_ځواب_cap_get_cap_ځواب_Master(masterCap));
    أكد(cap_ځواب_cap_get_cap_ځواب_Can_Grant(masterCap));
    أكد(TCB_PTR(cap_ځواب_cap_get_cap_TCBPtr(masterCap)) == sender);
    caller_خونه = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = caller_خونه->cap;
    /* Haskell error: "Caller cap must not already exist" */
    أكد(cap_get_capType(callerCap) == cap_null_cap);
    cte_ورننباسه(cap_ځواب_cap_new(canGrant, false, TCB_REF(sender)),
              ځواب_خونه, caller_خونه);
}

فراغ deleteCallerCap(tcb_t *receiver)
{
    cte_t *caller_خونه;

    caller_خونه = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(caller_خونه);
}
#د_که_پای

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, کلمه_t *بفر_Ptr, se_L4_پیغام_Info_t info)
{
    lookup_خونه_raw_ret_t lu_ret;
    cptr_t cptr;
    کلمه_t i, اوږدتیا;

    إذا (!بفر_Ptr) {
        current_extra_caps.excaprefs[0] = عدم;
        أرجع EXCEPTION_NONE;
    }

    اوږدتیا = se_L4_پیغام_Info_get_extra_Caps(info);

    لكل (i = 0; i < اوږدتیا; i++) {
        cptr = getExtraCPtr(بفر_Ptr, i);

        lu_ret = lookup_خونه(thread, cptr);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            أرجع lu_ret.حالت;
        }

        current_extra_caps.excaprefs[i] = lu_ret.خونه;
    }
    إذا (i < se_L4_پیغام_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = عدم;
    }

    أرجع EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
کلمه_t copyMRs(tcb_t *sender, کلمه_t *sendBuf, tcb_t *receiver,
               کلمه_t *recvBuf, کلمه_t n)
{
    کلمه_t i;

    /* Copy inline words */
    لكل (i = 0; i < n && i < n_پیغام_Registers; i++) {
        سېټ_ثبتوونکی(receiver, پیغام_Registers[i],
                    get_ثبتوونکی(sender, پیغام_Registers[i]));
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

#که_تعریف_شوی ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
فراغ remote_کتار_Update(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    إذا (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *موخه_Cur_Thread = غوټه_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        إذا (موخه_Cur_Thread == غوټه_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcb_لومړیتوب > موخه_Cur_Thread->tcb_لومړیتوب
#که_تعریف_شوی CONFIG_KERNEL_MCS
            || غوټه_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#د_که_پای
           ) {
            ARCH_غوټه_STATE(ipiReschedulePending) |= بټ(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
فراغ remoteTCBStall(tcb_t *tcb)
{

    إذا (
#که_تعریف_شوی CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#د_که_پای
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        غوټه_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_غوټه_STATE(ipiReschedulePending) |= بټ(tcb->tcbAffinity);
    }
}

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
ساكن exception_t invoke_TCB_سېټ_Affinity(tcb_t *thread, کلمه_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcb_Sched_له_کتار_ایستل(thread);
    migrateTCB(thread, affinity);
    إذا (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    إذا (thread == غوټه_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سېټ_Affinity(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    tcb_t *tcb;
    کلمه_t affinity;

    إذا (اوږدتیا < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, بفر);
    إذا (affinity >= ks_عدد_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_TCB_سېټ_Affinity(tcb, affinity);
}
#د_که_پای
#د_که_پای /* ENABLE_SMP_SUPPORT */

#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
ساكن exception_t invokeConfigureSingleStepping(bool_t call, کلمه_t *بفر, tcb_t *t,
                                                 uint16_t bp_عدد, کلمه_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = غوټه_STATE(ksCurThread);
    کلمه_t ارزښت;

    bp_was_consumed = configureSingleStepping(t, bp_عدد, n_instrs, false);
    إذا (n_instrs == 0) {
#که_نه_تعریف_شوی CONFIG_ARCH_AARCH64
        unset_Breakpoint_کارول_شوی_Flag(t, bp_عدد);
#د_که_پای /* CONFIG_ARCH_AARCH64 */
        ارزښت = false;
    } وإلا {
#که_نه_تعریف_شوی CONFIG_ARCH_AARCH64
        سېټ_Breakpoint_کارول_شوی_Flag(t, bp_عدد);
#د_که_پای /* CONFIG_ARCH_AARCH64 */
        ارزښت = bp_was_consumed;
    }

    إذا (call) {
        سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
        بینښه صحيح اوږدتیا = سېټ_MR(thread, بفر, 0, ارزښت);
        سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, اوږدتیا)));
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, کلمه_t *بفر)
{
    uint16_t bp_عدد;
    کلمه_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_عدد = getSyscallArg(0, بفر);
    n_instrs = getSyscallArg(1, بفر);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_عدد, n_instrs, false);
    إذا (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeConfigureSingleStepping(call, بفر, tcb, bp_عدد, n_instrs);
}

ساكن exception_t invoke_سېټ_Breakpoint(tcb_t *tcb, uint16_t bp_عدد,
                                       کلمه_t vaddr, کلمه_t type, کلمه_t اندازه, کلمه_t rw)
{
    سېټ_Breakpoint(tcb, bp_عدد, vaddr, type, اندازه, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    سېټ_Breakpoint_کارول_شوی_Flag(tcb, bp_عدد);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سېټ_Breakpoint(cap_t cap, کلمه_t *بفر)
{
    uint16_t bp_عدد;
    کلمه_t vaddr, type, اندازه, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_عدد = getSyscallArg(0, بفر);
    vaddr = getSyscallArg(1, بفر);
    type = getSyscallArg(2, بفر);
    اندازه = getSyscallArg(3, بفر);
    rw = getSyscallArg(4, بفر);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    إذا (vaddr >= (کلمه_t)USER_سر) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (type != seL4_InstructionBreakpoint && type != se_L4_معلومات_Breakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    } وإلا إذا (type == seL4_InstructionBreakpoint) {
        إذا (اندازه != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 3;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 4;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا ((se_L4_لومړی_Watchpoint == -1 || bp_عدد >= se_L4_لومړی_Watchpoint)
            && se_L4_لومړی_Breakpoint != se_L4_لومړی_Watchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 2;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    } وإلا إذا (type == se_L4_معلومات_Breakpoint) {
        إذا (اندازه == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 3;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (se_L4_لومړی_Watchpoint != -1 && bp_عدد < se_L4_لومړی_Watchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 2;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    } وإلا إذا (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (اندازه != 0 && اندازه != 1 && اندازه != 2 && اندازه != 4 && اندازه != 8) {
        userError("Debug: Invalid size %lu.", اندازه);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (اندازه > 0 && vaddr & (اندازه - 1)) {
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
                  vaddr, اندازه);

        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decode_سېټ_Breakpoint(tcb, bp_عدد, vaddr, type, اندازه, rw);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_سېټ_Breakpoint(tcb, bp_عدد,
                               vaddr, type, اندازه, rw);
}

ساكن exception_t invokeGetBreakpoint(bool_t call, کلمه_t *بفر, tcb_t *tcb, uint16_t bp_عدد)
{
    tcb_t *thread;
    thread = غوټه_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_عدد);
    إذا (call) {
        سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
        سېټ_MR(غوټه_STATE(ksCurThread), بفر, 0, res.vaddr);
        سېټ_MR(غوټه_STATE(ksCurThread), بفر, 1, res.type);
        سېټ_MR(غوټه_STATE(ksCurThread), بفر, 2, res.اندازه);
        سېټ_MR(غوټه_STATE(ksCurThread), بفر, 3, res.rw);
        سېټ_MR(غوټه_STATE(ksCurThread), بفر, 4, res.is_enabled);
        سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, 5)));
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeGetBreakpoint(cap_t cap, bool_t call, کلمه_t *بفر)
{
    tcb_t *tcb;
    uint16_t bp_عدد;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_عدد = getSyscallArg(0, بفر);

    error = Arch_decodeGetBreakpoint(tcb, bp_عدد);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeGetBreakpoint(call, بفر, tcb, bp_عدد);
}

ساكن exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_عدد)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_عدد);
    unset_Breakpoint_کارول_شوی_Flag(tcb, bp_عدد);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeUnsetBreakpoint(cap_t cap, کلمه_t *بفر)
{
    tcb_t *tcb;
    uint16_t bp_عدد;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_عدد = getSyscallArg(0, بفر);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_عدد);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeUnsetBreakpoint(tcb, bp_عدد);
}
#د_که_پای /* CONFIG_HARDWARE_DEBUG_API */

ساكن exception_t invoke_سېټ_TLSBase(tcb_t *thread, کلمه_t tls_بنسټ)
{
    سېټ_ثبتوونکی(thread, TLS_بنسټ, tls_بنسټ);
    إذا (thread == غوټه_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سېټ_TLSBase(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    کلمه_t tls_بنسټ;

    إذا (اوږدتیا < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tls_بنسټ = getSyscallArg(0, بفر);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_سېټ_TLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_بنسټ);
}

ساكن فراغ invoke_سېټ_Flags(tcb_t *thread, کلمه_t clear, کلمه_t سېټ, bool_t call)
{
    tcb_t *cur_thread = غوټه_STATE(ksCurThread);
    کلمه_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= سېټ & se_L4_TCBFlag_ماسک;
    thread->tcbFlags = flags;

#که_تعریف_شوی CONFIG_HAVE_FPU
    إذا (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } وإلا إذا (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#د_که_پای
    إذا (call) {
        کلمه_t *ipc_بفر = lookupIPCBuffer(true, cur_thread);
        سېټ_ثبتوونکی(cur_thread, badge_ثبتوونکی, 0);
        بینښه صحيح اوږدتیا = سېټ_MR(cur_thread, ipc_بفر, 0, flags);
        سېټ_ثبتوونکی(cur_thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, اوږدتیا)));
    }
    سېټ_Thread_State(cur_thread, ThreadState_Running);
}

ساكن exception_t decode_سېټ_Flags(cap_t cap, کلمه_t اوږدتیا, bool_t call, کلمه_t *بفر)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    إذا (اوږدتیا < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کلمه_t clear = getSyscallArg(0, بفر);
    کلمه_t سېټ   = getSyscallArg(1, بفر);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    invoke_سېټ_Flags(thread, clear, سېټ, call);
    أرجع EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(کلمه_t invLabel, کلمه_t اوږدتیا, cap_t cap,
                                cte_t *خونه, bool_t call, کلمه_t *بفر)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    بدل (invLabel) {
    حالة TCBReadRegisters:
        /* Second level of decoding */
        أرجع decodeReadRegisters(cap, اوږدتیا, call, بفر);

    حالة TCBWriteRegisters:
        أرجع decodeWriteRegisters(cap, اوږدتیا, بفر);

    حالة TCBCopyRegisters:
        أرجع decodeCopyRegisters(cap, اوږدتیا, بفر);

    حالة TCBSuspend:
        /* Jump straight to the invoke */
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالة TCBResume:
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالة TCBConfigure:
        أرجع decodeTCBConfigure(cap, اوږدتیا, خونه, بفر);

    حالة TCBSet_لومړیتوب:
        أرجع decode_سېټ_لومړیتوب(cap, اوږدتیا, بفر);

    حالة TCBSetMCPriority:
        أرجع decode_سېټ_MCPriority(cap, اوږدتیا, بفر);

    حالة TCBSetSchedParams:
#که_تعریف_شوی CONFIG_KERNEL_MCS
        أرجع decode_سېټ_Sched_Params(cap, اوږدتیا, خونه, بفر);
#وإلا
        أرجع decode_سېټ_Sched_Params(cap, اوږدتیا, بفر);
#د_که_پای

    حالة TCBSetIPCBuffer:
        أرجع decode_سېټ_IPCBuffer(cap, اوږدتیا, خونه, بفر);

    حالة TCBSetSpace:
        أرجع decode_سېټ_Space(cap, اوږدتیا, خونه, بفر);

    حالة TCBBindNotification:
        أرجع decodeBindNotification(cap);

    حالة TCBUnbindNotification:
        أرجع decodeUnbindNotification(cap);

#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة TCBSetTimeoutEndpoint:
        أرجع decode_سېټ_Timeout_Endpoint(cap, خونه);
#وإلا
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    حالة TCBSetAffinity:
        أرجع decode_سېټ_Affinity(cap, اوږدتیا, بفر);
#د_که_پای /* ENABLE_SMP_SUPPORT */
#د_که_پای

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#که_تعریف_شوی CONFIG_VTX
    حالة TCBSetEPTRoot:
        أرجع decode_سېټ_EPTRoot(cap);
#د_که_پای

#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
    حالة TCBConfigureSingleStepping:
        أرجع decodeConfigureSingleStepping(cap, call, بفر);

    حالة TCBSetBreakpoint:
        أرجع decode_سېټ_Breakpoint(cap, بفر);

    حالة TCBGetBreakpoint:
        أرجع decodeGetBreakpoint(cap, call, بفر);

    حالة TCBUnsetBreakpoint:
        أرجع decodeUnsetBreakpoint(cap, بفر);
#د_که_پای

    حالة TCBSetTLSBase:
        أرجع decode_سېټ_TLSBase(cap, اوږدتیا, بفر);

    حالة TCBSetFlags:
        أرجع decode_سېټ_Flags(cap, اوږدتیا, call, بفر);

    افتراضي:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

تعداد CopyRegistersFlags {
    Copy_Registers_suspend_سرچینه = 0,
    Copy_Registers_resume_موخه = 1,
    Copy_Registers_transfer_چوکاټ = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    کلمه_t transferArch;
    tcb_t *srcTCB;
    cap_t سرچینه_cap;
    کلمه_t flags;

    إذا (اوږدتیا < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);

    transferArch = Arch_decodeTransfer(flags >> 8);

    سرچینه_cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(سرچینه_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(سرچینه_cap));
    } وإلا {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & بټ(Copy_Registers_suspend_سرچینه),
               flags & بټ(Copy_Registers_resume_موخه),
               flags & بټ(Copy_Registers_transfer_چوکاټ),
               flags & بټ(CopyRegisters_transferInteger),
               transferArch);

}

تعداد ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, کلمه_t اوږدتیا, bool_t call,
                                کلمه_t *بفر)
{
    کلمه_t transferArch, flags, n;
    tcb_t *thread;

    إذا (اوږدتیا < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);
    n     = getSyscallArg(1, بفر);

    إذا (n < 1 || n > n_چوکاټ_Registers + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (صحيح)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_چوکاټ_Registers +
                                              n_gpRegisters;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    إذا (thread == غوټه_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & بټ(ReadRegisters_suspend),
               n, transferArch, call);
}

تعداد WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    کلمه_t flags, w;
    کلمه_t transferArch;
    tcb_t *thread;

    إذا (اوږدتیا < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);
    w     = getSyscallArg(1, بفر);

    إذا (اوږدتیا - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (صحيح)(اوږدتیا - 2), (صحيح)w);
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    إذا (thread == غوټه_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_WriteRegisters(thread,
                                    flags & بټ(WriteRegisters_resume),
                                    w, transferArch, بفر);
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap:
        أرجع (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_cap_Can_Grant_ځواب(cap)));
    حالة cap_null_cap:
        /* just has no fault endpoint */
        أرجع true;
    افتراضي:
        أرجع false;
    }
}
#د_که_پای

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, کلمه_t اوږدتیا, cte_t *خونه, کلمه_t *بفر)
{
    cte_t *بفر_خونه, *c_ریښه_خونه, *v_ریښه_خونه;
    cap_t بفر_Cap, c_ریښه_Cap, v_ریښه_Cap;
    deriveCap_ret_t dc_ret;
    کلمه_t c_ریښه_معلومات, v_ریښه_معلومات, بفر_Addr;
#که_تعریف_شوی CONFIG_KERNEL_MCS
#تعریف_کړه TCBCONFIGURE_دلیلونه 3
#وإلا
#تعریف_کړه TCBCONFIGURE_دلیلونه 4
#د_که_پای
    إذا (اوږدتیا < TCBCONFIGURE_دلیلونه || current_extra_caps.excaprefs[0] == عدم
        || current_extra_caps.excaprefs[1] == عدم
        || current_extra_caps.excaprefs[2] == عدم) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    c_ریښه_معلومات     = getSyscallArg(0, بفر);
    v_ریښه_معلومات     = getSyscallArg(1, بفر);
    بفر_Addr    = getSyscallArg(2, بفر);
#وإلا
    cptr_t faultEP       = getSyscallArg(0, بفر);
    c_ریښه_معلومات     = getSyscallArg(1, بفر);
    v_ریښه_معلومات     = getSyscallArg(2, بفر);
    بفر_Addr    = getSyscallArg(3, بفر);
#د_که_پای

    c_ریښه_خونه  = current_extra_caps.excaprefs[0];
    c_ریښه_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ریښه_خونه  = current_extra_caps.excaprefs[1];
    v_ریښه_Cap   = current_extra_caps.excaprefs[1]->cap;
    بفر_خونه = current_extra_caps.excaprefs[2];
    بفر_Cap  = current_extra_caps.excaprefs[2]->cap;

    إذا (بفر_Addr == 0) {
        بفر_خونه = عدم;
    } وإلا {
        dc_ret = deriveCap(بفر_خونه, بفر_Cap);
        إذا (dc_ret.حالت != EXCEPTION_NONE) {
            أرجع dc_ret.حالت;
        }
        بفر_Cap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(بفر_Addr, بفر_Cap);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (خونه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        خونه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (c_ریښه_معلومات != 0) {
        c_ریښه_Cap = update_Cap_معلومات(false, c_ریښه_معلومات, c_ریښه_Cap);
    }

    dc_ret = deriveCap(c_ریښه_خونه, c_ریښه_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    c_ریښه_Cap = dc_ret.cap;

    إذا (cap_get_capType(c_ریښه_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (v_ریښه_معلومات != 0) {
        v_ریښه_Cap = update_Cap_معلومات(false, v_ریښه_معلومات, v_ریښه_Cap);
    }

    dc_ret = deriveCap(v_ریښه_خونه, v_ریښه_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    v_ریښه_Cap = dc_ret.cap;

    إذا (!is_Valid_VTable_ریښه(v_ریښه_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
#که_تعریف_شوی CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خونه,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               c_ریښه_Cap, c_ریښه_خونه,
               v_ریښه_Cap, v_ریښه_خونه,
               بفر_Addr, بفر_Cap,
               بفر_خونه, thread_control_caps_update_space |
               thread_control_caps_update_ipc_بفر);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خونه,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_ریښه_Cap, c_ریښه_خونه,
               v_ریښه_Cap, v_ریښه_خونه,
               بفر_Addr, بفر_Cap,
               بفر_خونه, thread_control_update_space |
               thread_control_update_ipc_بفر);
#د_که_پای
}

exception_t decode_سېټ_لومړیتوب(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    إذا (اوږدتیا < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت = checkPrio(newPrio, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (بینښه اوږد) newPrio, (بینښه اوږد) authTCB->tcbMCP);
        أرجع حالت;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
#که_تعریف_شوی CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               cap_null_cap_new(), عدم,
               NULL_PRIO, newPrio,
               عدم, thread_control_sched_update_لومړیتوب);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(),
               عدم, thread_control_update_لومړیتوب);
#د_که_پای
}

exception_t decode_سېټ_MCPriority(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    إذا (اوږدتیا < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت = checkPrio(newMcp, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (بینښه اوږد) newMcp, (بینښه اوږد) authTCB->tcbMCP);
        أرجع حالت;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
#که_تعریف_شوی CONFIG_KERNEL_MCS
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
#د_که_پای
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
exception_t decode_سېټ_Timeout_Endpoint(cap_t cap, cte_t *خونه)
{
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *th_خونه = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    إذا (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خونه,
               cap_null_cap_new(), عدم,
               thCap, th_خونه,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(), عدم,
               thread_control_caps_update_timeout);
}
#د_که_پای

#که_تعریف_شوی CONFIG_KERNEL_MCS
exception_t decode_سېټ_Sched_Params(cap_t cap, کلمه_t اوږدتیا, cte_t *خونه, کلمه_t *بفر)
#وإلا
exception_t decode_سېټ_Sched_Params(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
#د_که_پای
{
    إذا (اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم
#که_تعریف_شوی CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == عدم || current_extra_caps.excaprefs[2] == عدم
#د_که_پای
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, بفر);
    prio_t newPrio = getSyscallArg(1, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#که_تعریف_شوی CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fh_خونه = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#د_که_پای

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت = checkPrio(newMcp, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (بینښه اوږد) newMcp, (بینښه اوږد) authTCB->tcbMCP);
        أرجع حالت;
    }

    حالت = checkPrio(newPrio, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (بینښه اوږد) newPrio, (بینښه اوږد) authTCB->tcbMCP);
        أرجع حالت;
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = عدم;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_لومړیتوب |
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
        إذا (tcb == غوټه_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        اكسر;
    افتراضي:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    إذا (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    إذا (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#د_که_پای
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
#که_تعریف_شوی CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خونه,
               fhCap, fh_خونه,
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
               thread_control_update_لومړیتوب);
#د_که_پای
}


exception_t decode_سېټ_IPCBuffer(cap_t cap, کلمه_t اوږدتیا, cte_t *خونه, کلمه_t *بفر)
{
    cptr_t cptr_بفر_Ptr;
    cap_t بفر_Cap;
    cte_t *بفر_خونه;

    إذا (اوږدتیا < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cptr_بفر_Ptr  = getSyscallArg(0, بفر);
    بفر_خونه = current_extra_caps.excaprefs[0];
    بفر_Cap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cptr_بفر_Ptr == 0) {
        بفر_خونه = عدم;
    } وإلا {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(بفر_خونه, بفر_Cap);
        إذا (dc_ret.حالت != EXCEPTION_NONE) {
            أرجع dc_ret.حالت;
        }
        بفر_Cap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_بفر_Ptr, بفر_Cap);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
#که_تعریف_شوی CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خونه,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cptr_بفر_Ptr, بفر_Cap,
               بفر_خونه, thread_control_caps_update_ipc_بفر);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خونه,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cptr_بفر_Ptr, بفر_Cap,
               بفر_خونه, thread_control_update_ipc_بفر);

#د_که_پای
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
#تعریف_کړه DECODE_سېټ_SPACE_PARAMS 2
#وإلا
#تعریف_کړه DECODE_سېټ_SPACE_PARAMS 3
#د_که_پای
exception_t decode_سېټ_Space(cap_t cap, کلمه_t اوږدتیا, cte_t *خونه, کلمه_t *بفر)
{
    کلمه_t c_ریښه_معلومات, v_ریښه_معلومات;
    cte_t *c_ریښه_خونه, *v_ریښه_خونه;
    cap_t c_ریښه_Cap, v_ریښه_Cap;
    deriveCap_ret_t dc_ret;

    إذا (اوږدتیا < DECODE_سېټ_SPACE_PARAMS || current_extra_caps.excaprefs[0] == عدم
        || current_extra_caps.excaprefs[1] == عدم
#که_تعریف_شوی CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == عدم
#د_که_پای
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    c_ریښه_معلومات = getSyscallArg(0, بفر);
    v_ریښه_معلومات = getSyscallArg(1, بفر);

    cte_t *fh_خونه     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_ریښه_خونه  = current_extra_caps.excaprefs[1];
    c_ریښه_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_ریښه_خونه  = current_extra_caps.excaprefs[2];
    v_ریښه_Cap   = current_extra_caps.excaprefs[2]->cap;
#وإلا
    cptr_t faultEP   = getSyscallArg(0, بفر);
    c_ریښه_معلومات = getSyscallArg(1, بفر);
    v_ریښه_معلومات = getSyscallArg(2, بفر);

    c_ریښه_خونه  = current_extra_caps.excaprefs[0];
    c_ریښه_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ریښه_خونه  = current_extra_caps.excaprefs[1];
    v_ریښه_Cap   = current_extra_caps.excaprefs[1]->cap;
#د_که_پای

    إذا (خونه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        خونه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (c_ریښه_معلومات != 0) {
        c_ریښه_Cap = update_Cap_معلومات(false, c_ریښه_معلومات, c_ریښه_Cap);
    }

    dc_ret = deriveCap(c_ریښه_خونه, c_ریښه_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    c_ریښه_Cap = dc_ret.cap;

    إذا (cap_get_capType(c_ریښه_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (v_ریښه_معلومات != 0) {
        v_ریښه_Cap = update_Cap_معلومات(false, v_ریښه_معلومات, v_ریښه_Cap);
    }

    dc_ret = deriveCap(v_ریښه_خونه, v_ریښه_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    v_ریښه_Cap = dc_ret.cap;

    إذا (!is_Valid_VTable_ریښه(v_ریښه_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    /* fault handler */
    إذا (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#د_که_پای

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
#که_تعریف_شوی CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خونه,
               fhCap, fh_خونه,
               cap_null_cap_new(), عدم,
               c_ریښه_Cap, c_ریښه_خونه,
               v_ریښه_Cap, v_ریښه_خونه,
               0, cap_null_cap_new(), عدم, thread_control_caps_update_space | thread_control_caps_update_fault);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خونه,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_ریښه_Cap, c_ریښه_خونه,
               v_ریښه_Cap, v_ریښه_خونه,
               0, cap_null_cap_new(), عدم, thread_control_update_space);
#د_که_پای
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
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

    إذا ((tcb_t *)notification_ptr_get_ntfn_کتار_سر(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }


    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
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

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
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

#که_تعریف_شوی CONFIG_KERNEL_MCS
ساكن مضمن exception_t installTCBCap(tcb_t *موخه, cap_t tCap, cte_t *خونه,
                                        tcb_cnode_شاخص_t شاخص, cap_t newCap, cte_t *src_خونه)
{
    cte_t *ریښه_خونه = TCB_PTR_CTE_PTR(موخه, شاخص);
    ناکارول_شوی exception_t e = cteDelete(ریښه_خونه, true);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    إذا (cap_get_capType(newCap) != cap_null_cap) {
        إذا (sameObjectAs(newCap, src_خونه->cap) &&
            sameObjectAs(tCap, خونه->cap)) {
            cte_ورننباسه(newCap, src_خونه, ریښه_خونه);
        }
    }
    أرجع e;
}
#د_که_پای

#که_تعریف_شوی CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *موخه, cte_t *خونه,
                                        cap_t fh_newCap, cte_t *fh_src_خونه,
                                        cap_t th_newCap, cte_t *th_src_خونه,
                                        cap_t c_ریښه_new_Cap, cte_t *c_ریښه_src_خونه,
                                        cap_t v_ریښه_new_Cap, cte_t *v_ریښه_src_خونه,
                                        کلمه_t بفر_Addr, cap_t بفر_Cap,
                                        cte_t *بفر_Src_خونه,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((کلمه_t)موخه);

    إذا (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(موخه, tCap, خونه, tcbFaultHandler, fh_newCap, fh_src_خونه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }

    }

    إذا (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(موخه, tCap, خونه, tcbTimeoutHandler, th_newCap, th_src_خونه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(موخه, tCap, خونه, tcbCTable, c_ریښه_new_Cap, c_ریښه_src_خونه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }

        e = installTCBCap(موخه, tCap, خونه, tcbVTable, v_ریښه_new_Cap, v_ریښه_src_خونه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_caps_update_ipc_بفر) {
        cte_t *بفر_خونه;

        بفر_خونه = TCB_PTR_CTE_PTR(موخه, tcb_بفر);
        e = cteDelete(بفر_خونه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        موخه->tcbIPCBuffer = بفر_Addr;

        إذا (بفر_Src_خونه && sameObjectAs(بفر_Cap, بفر_Src_خونه->cap) &&
            sameObjectAs(tCap, خونه->cap)) {
            cte_ورننباسه(بفر_Cap, بفر_Src_خونه, بفر_خونه);
        }

        إذا (موخه == غوټه_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t invokeTCB_ThreadControl(tcb_t *موخه, cte_t *خونه,
                                    cptr_t faultep, prio_t mcp, prio_t لومړیتوب,
                                    cap_t c_ریښه_new_Cap, cte_t *c_ریښه_src_خونه,
                                    cap_t v_ریښه_new_Cap, cte_t *v_ریښه_src_خونه,
                                    کلمه_t بفر_Addr, cap_t بفر_Cap,
                                    cte_t *بفر_Src_خونه,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((کلمه_t)موخه);

    إذا (updateFlags & thread_control_update_space) {
        موخه->tcbFaultHandler = faultep;
    }

    إذا (updateFlags & thread_control_update_mcp) {
        سېټ_MCPriority(موخه, mcp);
    }

    إذا (updateFlags & thread_control_update_space) {
        cte_t *ریښه_خونه;

        ریښه_خونه = TCB_PTR_CTE_PTR(موخه, tcbCTable);
        e = cteDelete(ریښه_خونه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        إذا (sameObjectAs(c_ریښه_new_Cap, c_ریښه_src_خونه->cap) &&
            sameObjectAs(tCap, خونه->cap)) {
            cte_ورننباسه(c_ریښه_new_Cap, c_ریښه_src_خونه, ریښه_خونه);
        }
    }

    إذا (updateFlags & thread_control_update_space) {
        cte_t *ریښه_خونه;

        ریښه_خونه = TCB_PTR_CTE_PTR(موخه, tcbVTable);
        e = cteDelete(ریښه_خونه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        إذا (sameObjectAs(v_ریښه_new_Cap, v_ریښه_src_خونه->cap) &&
            sameObjectAs(tCap, خونه->cap)) {
            cte_ورننباسه(v_ریښه_new_Cap, v_ریښه_src_خونه, ریښه_خونه);
        }
    }

    إذا (updateFlags & thread_control_update_ipc_بفر) {
        cte_t *بفر_خونه;

        بفر_خونه = TCB_PTR_CTE_PTR(موخه, tcb_بفر);
        e = cteDelete(بفر_خونه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        موخه->tcbIPCBuffer = بفر_Addr;

        إذا (بفر_Src_خونه && sameObjectAs(بفر_Cap, بفر_Src_خونه->cap) &&
            sameObjectAs(tCap, خونه->cap)) {
            cte_ورننباسه(بفر_Cap, بفر_Src_خونه, بفر_خونه);
        }

        إذا (موخه == غوټه_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    إذا (updateFlags & thread_control_update_لومړیتوب) {
        سېټ_لومړیتوب(موخه, لومړیتوب);
    }

    أرجع EXCEPTION_NONE;
}
#د_که_پای

#که_تعریف_شوی CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *موخه, cte_t *خونه,
                                         cap_t fh_newCap, cte_t *fh_src_خونه,
                                         prio_t mcp, prio_t لومړیتوب,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    إذا (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((کلمه_t)موخه);
        exception_t e = installTCBCap(موخه, tCap, خونه, tcbFaultHandler, fh_newCap, fh_src_خونه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_sched_update_mcp) {
        سېټ_MCPriority(موخه, mcp);
    }

    إذا (updateFlags & thread_control_sched_update_لومړیتوب) {
        سېټ_لومړیتوب(موخه, لومړیتوب);
    }

    إذا (updateFlags & thread_control_sched_update_sc) {
        إذا (sc != عدم) {
            schedContext_bindTCB(sc, موخه);
        } وإلا إذا (sc == عدم) {
            schedContext_unbindTCB(موخه->tcbSchedContext);
        }
    }

    أرجع EXCEPTION_NONE;
}
#د_که_پای

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspend_سرچینه, bool_t resume_موخه,
                                    bool_t transfer_چوکاټ, bool_t transferInteger,
                                    کلمه_t transferArch)
{
    إذا (suspend_سرچینه) {
        suspend(tcb_src);
    }

    إذا (resume_موخه) {
        restart(dest);
    }

    إذا (transfer_چوکاټ) {
        کلمه_t i;
        کلمه_t v;
        کلمه_t pc;

        لكل (i = 0; i < n_چوکاټ_Registers; i++) {
            v = get_ثبتوونکی(tcb_src, چوکاټ_Registers[i]);
            سېټ_ثبتوونکی(dest, چوکاټ_Registers[i], v);
        }

        pc = getRestartPC(dest);
        سېټ_بل_PC(dest, pc);
    }

    إذا (transferInteger) {
        کلمه_t i;
        کلمه_t v;

        لكل (i = 0; i < n_gpRegisters; i++) {
            v = get_ثبتوونکی(tcb_src, gpRegisters[i]);
            سېټ_ثبتوونکی(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    إذا (dest == غوټه_STATE(ksCurThread)) {
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
exception_t invokeTCB_ReadRegisters(tcb_t *tcb_src, bool_t suspend_سرچینه,
                                    کلمه_t n, کلمه_t arch, bool_t call)
{
    کلمه_t i, j;
    exception_t e;
    tcb_t *thread;

    thread = غوټه_STATE(ksCurThread);

    إذا (suspend_سرچینه) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, غوټه_STATE(ksCurThread));
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    إذا (call) {
        کلمه_t *ipc_بفر;

        ipc_بفر = lookupIPCBuffer(true, thread);

        سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);

        لكل (i = 0; i < n && i < n_چوکاټ_Registers && i < n_پیغام_Registers; i++) {
            سېټ_ثبتوونکی(thread, پیغام_Registers[i],
                        get_ثبتوونکی(tcb_src, چوکاټ_Registers[i]));
        }

        إذا (ipc_بفر != عدم && i < n && i < n_چوکاټ_Registers) {
            لكل (; i < n && i < n_چوکاټ_Registers; i++) {
                ipc_بفر[i + 1] = get_ثبتوونکی(tcb_src, چوکاټ_Registers[i]);
            }
        }

        j = i;

        لكل (i = 0; i < n_gpRegisters && i + n_چوکاټ_Registers < n
             && i + n_چوکاټ_Registers < n_پیغام_Registers; i++) {
            سېټ_ثبتوونکی(thread, پیغام_Registers[i + n_چوکاټ_Registers],
                        get_ثبتوونکی(tcb_src, gpRegisters[i]));
        }

        إذا (ipc_بفر != عدم && i < n_gpRegisters
            && i + n_چوکاټ_Registers < n) {
            لكل (; i < n_gpRegisters && i + n_چوکاټ_Registers < n; i++) {
                ipc_بفر[i + n_چوکاټ_Registers + 1] =
                    get_ثبتوونکی(tcb_src, gpRegisters[i]);
            }
        }

        سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, i + j)));
    }
    سېټ_Thread_State(thread, ThreadState_Running);

    أرجع EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resume_موخه,
                                     کلمه_t n, کلمه_t arch, کلمه_t *بفر)
{
    کلمه_t i;
    کلمه_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, غوټه_STATE(ksCurThread), dest);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    إذا (n > n_چوکاټ_Registers + n_gpRegisters) {
        n = n_چوکاټ_Registers + n_gpRegisters;
    }

    archInfo = Arch_get_Sanitise_ثبتوونکی_Info(dest);

    لكل (i = 0; i < n_چوکاټ_Registers && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        سېټ_ثبتوونکی(dest, چوکاټ_Registers[i],
                    sanitise_ثبتوونکی(چوکاټ_Registers[i],
                                     getSyscallArg(i + 2, بفر), archInfo));
    }

    لكل (i = 0; i < n_gpRegisters && i + n_چوکاټ_Registers < n; i++) {
        سېټ_ثبتوونکی(dest, gpRegisters[i],
                    sanitise_ثبتوونکی(gpRegisters[i],
                                     getSyscallArg(i + n_چوکاټ_Registers + 2,
                                                   بفر), archInfo));
    }

    pc = getRestartPC(dest);
    سېټ_بل_PC(dest, pc);

    Arch_postModifyRegisters(dest);

    إذا (resume_موخه) {
        restart(dest);
    }

    إذا (dest == غوټه_STATE(ksCurThread)) {
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

#که_تعریف_شوی CONFIG_DEBUG_BUILD
فراغ سېټ_Thread_Name(tcb_t *tcb, ثابت حرف *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_اوږدتیا);
}
#د_که_پای

کلمه_t سېټ_MRs_syscall_error(tcb_t *thread, کلمه_t *receiveIPCBuffer)
{
    بدل (current_syscall_error.type) {
    حالة se_L4_Invalid_دلیل:
        أرجع سېټ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_دلیل_شمېره);

    حالة seL4_InvalidCapability:
        أرجع سېټ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_شمېره);

    حالة seL4_IllegalOperation:
        أرجع 0;

    حالة seL4_RangeError:
        سېټ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        أرجع سېټ_MR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    حالة seL4_AlignmentError:
        أرجع 0;

    حالة seL4_FailedLookup:
        سېټ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failed_Lookup_Was_سرچینه ? 1 : 0);
        أرجع سېټ_MRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    حالة se_L4_Truncated_پیغام:
    حالة se_L4_Delete_لومړی:
    حالة se_L4_Revoke_لومړی:
        أرجع 0;
    حالة se_L4_Not_Enough_حافظه:
        أرجع سېټ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.حافظه_کیڼ);
    افتراضي:
        fail("Invalid syscall error");
    }
}
