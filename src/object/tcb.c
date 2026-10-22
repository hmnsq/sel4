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
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#جيڪڏهن_پورو
#ضمن <object/tcb.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <model/statedata.h>
#ضمن <util.h>
#ضمن <string.h>
#ضمن <stdint.h>
#ضمن <arch/smp/ipi_inline.h>

#وصف_ڪريو NULL_PRIO 0

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

ساكن مضمن فراغ جوڙيو_منزل_Bitmap(لفظ_t cpu, لفظ_t dom, لفظ_t prio)
{
    لفظ_t l1index;
    لفظ_t l1index_inverted;

    l1index = prio_منزل_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    ڳنڍ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= ٻٽو_انگ(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    ڳنڍ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= ٻٽو_انگ(prio & نقاب(لفظ_Radix));
}

ساكن مضمن فراغ removeFromBitmap(لفظ_t cpu, لفظ_t dom, لفظ_t prio)
{
    لفظ_t l1index;
    لفظ_t l1index_inverted;

    l1index = prio_منزل_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    ڳنڍ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~ٻٽو_انگ(prio & نقاب(لفظ_Radix));
    إذا (unlikely(!ڳنڍ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        ڳنڍ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~ٻٽو_انگ(l1index);
    }
}

tcb_قطار_t tcb_قطار_remove(tcb_قطار_t قطار, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcb_Sched_ايندڙ;

    إذا (قطار.سر == tcb && قطار.پڇاڙي == tcb) {
        قطار.سر = عدم;
        قطار.پڇاڙي = عدم;
    } وإلا {
        إذا (قطار.سر == tcb) {
            after->tcbSchedPrev = عدم;
            tcb->tcb_Sched_ايندڙ = عدم;
            قطار.سر = after;
        } وإلا {
            إذا (قطار.پڇاڙي == tcb) {
                before->tcb_Sched_ايندڙ = عدم;
                tcb->tcbSchedPrev = عدم;
                قطار.پڇاڙي = before;
            } وإلا {
                before->tcb_Sched_ايندڙ = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = عدم;
                tcb->tcb_Sched_ايندڙ = عدم;
            }
        }
    }

    أرجع قطار;
}

/* Add TCB to the head of a scheduler queue */
فراغ tcb_Sched_قطار_اندر_وجهو(tcb_t *tcb)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
#جيڪڏهن_پورو

    إذا (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_قطار_t قطار;
        dom_t dom;
        prio_t prio;
        لفظ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ترجيح;
        idx = ready_queues_اشاريو(dom, prio);
        قطار = ڳنڍ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        إذا (tcb_قطار_خالي_جايون(قطار)) {
            جوڙيو_منزل_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ڳنڍ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_قطار_prepend(قطار, tcb);

        thread_state_ptr_سيٽ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
فراغ tcbSchedAppend(tcb_t *tcb)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
    أكد(refill_ready(tcb->tcbSchedContext));
#جيڪڏهن_پورو
    إذا (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_قطار_t قطار;
        dom_t dom;
        prio_t prio;
        لفظ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ترجيح;
        idx = ready_queues_اشاريو(dom, prio);
        قطار = ڳنڍ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        إذا (tcb_قطار_خالي_جايون(قطار)) {
            جوڙيو_منزل_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        ڳنڍ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_قطار_append(قطار, tcb);

        thread_state_ptr_سيٽ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
فراغ tcb_Sched_قطار_مان_ڪڍو(tcb_t *tcb)
{
    إذا (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_قطار_t قطار;
        tcb_قطار_t new_قطار;
        dom_t dom;
        prio_t prio;
        لفظ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ترجيح;
        idx = ready_queues_اشاريو(dom, prio);
        قطار = ڳنڍ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_قطار = tcb_قطار_remove(قطار, tcb);

        ڳنڍ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_قطار;

        thread_state_ptr_سيٽ_tcb_Queued(&tcb->tcbState, false);

        إذا (likely(tcb_قطار_خالي_جايون(new_قطار))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
فراغ tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = عدم;

    debug_tcb->tcb_Debug_ايندڙ = ڳنڍ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    إذا (ڳنڍ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(ڳنڍ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    ڳنڍ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

فراغ tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    أكد(ڳنڍ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != عدم);
    إذا (tcb == ڳنڍ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        ڳنڍ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(ڳنڍ_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcb_Debug_ايندڙ;
    } وإلا {
        أكد(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcb_Debug_ايندڙ = debug_tcb->tcb_Debug_ايندڙ;
    }

    إذا (debug_tcb->tcb_Debug_ايندڙ) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_ايندڙ)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = عدم;
    debug_tcb->tcb_Debug_ايندڙ = عدم;
}
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */

#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_قطار_t tcbEPAppend(tcb_t *tcb, tcb_قطار_t قطار)
{
    إذا (!قطار.سر) { /* Empty list */
        قطار.سر = tcb;
    } وإلا {
        قطار.پڇاڙي->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = قطار.پڇاڙي;
    tcb->tcbEPNext = عدم;
    قطار.پڇاڙي = tcb;

    أرجع قطار;
}

/* Remove TCB from an endpoint queue */
tcb_قطار_t tcbEPDequeue(tcb_t *tcb, tcb_قطار_t قطار)
{
    إذا (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } وإلا {
        قطار.سر = tcb->tcbEPNext;
    }

    إذا (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } وإلا {
        قطار.پڇاڙي = tcb->tcbEPPrev;
    }

    أرجع قطار;
}
#جيڪڏهن_پورو /* CONFIG_KERNEL_MCS */

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS

فراغ tcbReleaseRemove(tcb_t *tcb)
{
    إذا (likely(thread_state_get_tcb_اندريون_Release_قطار(tcb->tcbState))) {
        tcb_قطار_t قطار = ڳنڍ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity);

        إذا (قطار.سر == tcb) {
            ڳنڍ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        ڳنڍ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity) = tcb_قطار_remove(قطار, tcb);

        thread_state_ptr_سيٽ_tcb_اندريون_Release_قطار(&tcb->tcbState, false);
    }
}

ساكن مضمن ticks_t خالص tcb_Ready_وقت(tcb_t *tcb)
{
    أرجع refill_سر(tcb->tcbSchedContext)->r_وقت;
}

ساكن مضمن bool_t خالص وقت_after(tcb_t *tcb, ticks_t new_وقت)
{
    أرجع tcb != عدم && new_وقت >= tcb_Ready_وقت(tcb);
}

ساكن tcb_t *ڳوليو_وقت_after(tcb_t *tcb, ticks_t new_وقت)
{
    tcb_t *after = tcb;

    طالما (وقت_after(after, new_وقت)) {
        after = after->tcb_Sched_ايندڙ;
    }

    أرجع after;
}

فراغ tcb_Release_قطار_اندر_وجهو(tcb_t *tcb)
{
    أكد(thread_state_get_tcb_اندريون_Release_قطار(tcb->tcbState) == false);
    أكد(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_وقت;
    tcb_قطار_t قطار;

    قطار = ڳنڍ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity);
    new_وقت = tcb_Ready_وقت(tcb);

    إذا (tcb_قطار_خالي_جايون(قطار) || new_وقت < tcb_Ready_وقت(قطار.سر)) {
        ڳنڍ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity) = tcb_قطار_prepend(قطار, tcb);
    } وإلا {
        إذا (tcb_Ready_وقت(قطار.پڇاڙي) <= new_وقت) {
            ڳنڍ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity) = tcb_قطار_append(قطار, tcb);
        } وإلا {
            tcb_t *after;
            after = ڳوليو_وقت_after(قطار.سر, new_وقت);
            tcb_قطار_وجهو(tcb, after);
        }
    }

    thread_state_ptr_سيٽ_tcb_اندريون_Release_قطار(&tcb->tcbState, true);

    إذا (قطار.سر != ڳنڍ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity).سر) {
        ڳنڍ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#جيڪڏهن_پورو

cptr_t خالص getExtraCPtr(لفظ_t *بفر_Ptr, لفظ_t i)
{
    أرجع (cptr_t)بفر_Ptr[se_L4_نياپو_Max_ڊيگهائي + 2 + i];
}

فراغ سيٽ_Extra_Badge(لفظ_t *بفر_Ptr, لفظ_t badge,
                   لفظ_t i)
{
    بفر_Ptr[se_L4_نياپو_Max_ڊيگهائي + 2 + i] = badge;
}

#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
فراغ تياري_Caller_Cap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *جواب_جڳهه, *caller_جڳهه;
    cap_t masterCap اڻ_استعمال_ٿيل, callerCap اڻ_استعمال_ٿيل;

    سيٽ_Thread_State(sender, Thread_State_Blocked_On_جواب);
    جواب_جڳهه = TCB_PTR_CTE_PTR(sender, tcb_جواب);
    masterCap = جواب_جڳهه->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    أكد(cap_get_capType(masterCap) == cap_جواب_cap);
    أكد(cap_جواب_cap_get_cap_جواب_Master(masterCap));
    أكد(cap_جواب_cap_get_cap_جواب_Can_Grant(masterCap));
    أكد(TCB_PTR(cap_جواب_cap_get_cap_TCBPtr(masterCap)) == sender);
    caller_جڳهه = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = caller_جڳهه->cap;
    /* Haskell error: "Caller cap must not already exist" */
    أكد(cap_get_capType(callerCap) == cap_null_cap);
    cte_وجهو(cap_جواب_cap_new(canGrant, false, TCB_REF(sender)),
              جواب_جڳهه, caller_جڳهه);
}

فراغ deleteCallerCap(tcb_t *receiver)
{
    cte_t *caller_جڳهه;

    caller_جڳهه = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(caller_جڳهه);
}
#جيڪڏهن_پورو

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, لفظ_t *بفر_Ptr, se_L4_پيغام_Info_t info)
{
    lookup_جڳهه_raw_ret_t lu_ret;
    cptr_t cptr;
    لفظ_t i, ڊيگهائي;

    إذا (!بفر_Ptr) {
        current_extra_caps.excaprefs[0] = عدم;
        أرجع EXCEPTION_NONE;
    }

    ڊيگهائي = se_L4_پيغام_Info_get_extra_Caps(info);

    لكل (i = 0; i < ڊيگهائي; i++) {
        cptr = getExtraCPtr(بفر_Ptr, i);

        lu_ret = lookup_جڳهه(thread, cptr);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            أرجع lu_ret.حالت;
        }

        current_extra_caps.excaprefs[i] = lu_ret.جڳهه;
    }
    إذا (i < se_L4_نياپو_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = عدم;
    }

    أرجع EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
لفظ_t copyMRs(tcb_t *sender, لفظ_t *sendBuf, tcb_t *receiver,
               لفظ_t *recvBuf, لفظ_t n)
{
    لفظ_t i;

    /* Copy inline words */
    لكل (i = 0; i < n && i < n_نياپو_Registers; i++) {
        سيٽ_رجسٽر(receiver, نياپو_Registers[i],
                    get_رجسٽر(sender, نياپو_Registers[i]));
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

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
فراغ remote_قطار_Update(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    إذا (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *هدف_Cur_Thread = ڳنڍ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        إذا (هدف_Cur_Thread == ڳنڍ_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcb_ترجيح > هدف_Cur_Thread->tcb_ترجيح
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
            || ڳنڍ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#جيڪڏهن_پورو
           ) {
            ARCH_ڳنڍ_STATE(ipiReschedulePending) |= ٻٽو_انگ(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
فراغ remoteTCBStall(tcb_t *tcb)
{

    إذا (
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#جيڪڏهن_پورو
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        ڳنڍ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_ڳنڍ_STATE(ipiReschedulePending) |= ٻٽو_انگ(tcb->tcbAffinity);
    }
}

#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
ساكن exception_t invoke_TCB_سيٽ_Affinity(tcb_t *thread, لفظ_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcb_Sched_قطار_مان_ڪڍو(thread);
    migrateTCB(thread, affinity);
    إذا (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    إذا (thread == ڳنڍ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سيٽ_Affinity(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    tcb_t *tcb;
    لفظ_t affinity;

    إذا (ڊيگهائي < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, بفر);
    إذا (affinity >= ks_انگ_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_TCB_سيٽ_Affinity(tcb, affinity);
}
#جيڪڏهن_پورو
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
ساكن exception_t invokeConfigureSingleStepping(bool_t call, لفظ_t *بفر, tcb_t *t,
                                                 uint16_t bp_انگ, لفظ_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);
    لفظ_t قدر;

    bp_was_consumed = configureSingleStepping(t, bp_انگ, n_instrs, false);
    إذا (n_instrs == 0) {
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARCH_AARCH64
        unset_Breakpoint_استعمال_ٿيل_Flag(t, bp_انگ);
#جيڪڏهن_پورو /* CONFIG_ARCH_AARCH64 */
        قدر = false;
    } وإلا {
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARCH_AARCH64
        سيٽ_Breakpoint_استعمال_ٿيل_Flag(t, bp_انگ);
#جيڪڏهن_پورو /* CONFIG_ARCH_AARCH64 */
        قدر = bp_was_consumed;
    }

    إذا (call) {
        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
        بنانشان صحيح ڊيگهائي = سيٽ_MR(thread, بفر, 0, قدر);
        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, ڊيگهائي)));
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, لفظ_t *بفر)
{
    uint16_t bp_انگ;
    لفظ_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_انگ = getSyscallArg(0, بفر);
    n_instrs = getSyscallArg(1, بفر);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_انگ, n_instrs, false);
    إذا (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeConfigureSingleStepping(call, بفر, tcb, bp_انگ, n_instrs);
}

ساكن exception_t invoke_سيٽ_Breakpoint(tcb_t *tcb, uint16_t bp_انگ,
                                       لفظ_t vaddr, لفظ_t type, لفظ_t سائيز, لفظ_t rw)
{
    سيٽ_Breakpoint(tcb, bp_انگ, vaddr, type, سائيز, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    سيٽ_Breakpoint_استعمال_ٿيل_Flag(tcb, bp_انگ);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سيٽ_Breakpoint(cap_t cap, لفظ_t *بفر)
{
    uint16_t bp_انگ;
    لفظ_t vaddr, type, سائيز, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_انگ = getSyscallArg(0, بفر);
    vaddr = getSyscallArg(1, بفر);
    type = getSyscallArg(2, بفر);
    سائيز = getSyscallArg(3, بفر);
    rw = getSyscallArg(4, بفر);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    إذا (vaddr >= (لفظ_t)USER_چوٽي) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (type != seL4_InstructionBreakpoint && type != se_L4_معلومات_Breakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    } وإلا إذا (type == seL4_InstructionBreakpoint) {
        إذا (سائيز != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 3;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 4;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا ((se_L4_پهريون_Watchpoint == -1 || bp_انگ >= se_L4_پهريون_Watchpoint)
            && se_L4_پهريون_Breakpoint != se_L4_پهريون_Watchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 2;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    } وإلا إذا (type == se_L4_معلومات_Breakpoint) {
        إذا (سائيز == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 3;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (se_L4_پهريون_Watchpoint != -1 && bp_انگ < se_L4_پهريون_Watchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 2;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    } وإلا إذا (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (سائيز != 0 && سائيز != 1 && سائيز != 2 && سائيز != 4 && سائيز != 8) {
        userError("Debug: Invalid size %lu.", سائيز);
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (سائيز > 0 && vaddr & (سائيز - 1)) {
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
                  vaddr, سائيز);

        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decode_سيٽ_Breakpoint(tcb, bp_انگ, vaddr, type, سائيز, rw);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_سيٽ_Breakpoint(tcb, bp_انگ,
                               vaddr, type, سائيز, rw);
}

ساكن exception_t invokeGetBreakpoint(bool_t call, لفظ_t *بفر, tcb_t *tcb, uint16_t bp_انگ)
{
    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_انگ);
    إذا (call) {
        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
        سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, 0, res.vaddr);
        سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, 1, res.type);
        سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, 2, res.سائيز);
        سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, 3, res.rw);
        سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, 4, res.is_enabled);
        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, 5)));
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeGetBreakpoint(cap_t cap, bool_t call, لفظ_t *بفر)
{
    tcb_t *tcb;
    uint16_t bp_انگ;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_انگ = getSyscallArg(0, بفر);

    error = Arch_decodeGetBreakpoint(tcb, bp_انگ);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeGetBreakpoint(call, بفر, tcb, bp_انگ);
}

ساكن exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_انگ)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_انگ);
    unset_Breakpoint_استعمال_ٿيل_Flag(tcb, bp_انگ);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeUnsetBreakpoint(cap_t cap, لفظ_t *بفر)
{
    tcb_t *tcb;
    uint16_t bp_انگ;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_انگ = getSyscallArg(0, بفر);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_انگ);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeUnsetBreakpoint(tcb, bp_انگ);
}
#جيڪڏهن_پورو /* CONFIG_HARDWARE_DEBUG_API */

ساكن exception_t invoke_سيٽ_TLSBase(tcb_t *thread, لفظ_t tls_بنياد)
{
    سيٽ_رجسٽر(thread, TLS_بنياد, tls_بنياد);
    إذا (thread == ڳنڍ_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سيٽ_TLSBase(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    لفظ_t tls_بنياد;

    إذا (ڊيگهائي < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tls_بنياد = getSyscallArg(0, بفر);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_سيٽ_TLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_بنياد);
}

ساكن فراغ invoke_سيٽ_Flags(tcb_t *thread, لفظ_t clear, لفظ_t سيٽ, bool_t call)
{
    tcb_t *cur_thread = ڳنڍ_STATE(ksCurThread);
    لفظ_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= سيٽ & se_L4_TCBFlag_نقاب;
    thread->tcbFlags = flags;

#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
    إذا (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } وإلا إذا (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#جيڪڏهن_پورو
    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, cur_thread);
        سيٽ_رجسٽر(cur_thread, badge_رجسٽر, 0);
        بنانشان صحيح ڊيگهائي = سيٽ_MR(cur_thread, ipc_بفر, 0, flags);
        سيٽ_رجسٽر(cur_thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, ڊيگهائي)));
    }
    سيٽ_Thread_State(cur_thread, ThreadState_Running);
}

ساكن exception_t decode_سيٽ_Flags(cap_t cap, لفظ_t ڊيگهائي, bool_t call, لفظ_t *بفر)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    إذا (ڊيگهائي < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t clear = getSyscallArg(0, بفر);
    لفظ_t سيٽ   = getSyscallArg(1, بفر);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    invoke_سيٽ_Flags(thread, clear, سيٽ, call);
    أرجع EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي, cap_t cap,
                                cte_t *جڳهه, bool_t call, لفظ_t *بفر)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    بدل (invLabel) {
    حالة TCBReadRegisters:
        /* Second level of decoding */
        أرجع decodeReadRegisters(cap, ڊيگهائي, call, بفر);

    حالة TCBWriteRegisters:
        أرجع decodeWriteRegisters(cap, ڊيگهائي, بفر);

    حالة TCBCopyRegisters:
        أرجع decodeCopyRegisters(cap, ڊيگهائي, بفر);

    حالة TCBSuspend:
        /* Jump straight to the invoke */
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالة TCBResume:
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالة TCBConfigure:
        أرجع decodeTCBConfigure(cap, ڊيگهائي, جڳهه, بفر);

    حالة TCBSet_ترجيح:
        أرجع decode_سيٽ_ترجيح(cap, ڊيگهائي, بفر);

    حالة TCBSetMCPriority:
        أرجع decode_سيٽ_MCPriority(cap, ڊيگهائي, بفر);

    حالة TCBSetSchedParams:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        أرجع decode_سيٽ_Sched_Params(cap, ڊيگهائي, جڳهه, بفر);
#وإلا
        أرجع decode_سيٽ_Sched_Params(cap, ڊيگهائي, بفر);
#جيڪڏهن_پورو

    حالة TCBSetIPCBuffer:
        أرجع decode_سيٽ_IPCBuffer(cap, ڊيگهائي, جڳهه, بفر);

    حالة TCBSetSpace:
        أرجع decode_سيٽ_Space(cap, ڊيگهائي, جڳهه, بفر);

    حالة TCBBindNotification:
        أرجع decodeBindNotification(cap);

    حالة TCBUnbindNotification:
        أرجع decodeUnbindNotification(cap);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة TCBSetTimeoutEndpoint:
        أرجع decode_سيٽ_Timeout_Endpoint(cap, جڳهه);
#وإلا
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    حالة TCBSetAffinity:
        أرجع decode_سيٽ_Affinity(cap, ڊيگهائي, بفر);
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
#جيڪڏهن_پورو

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#جيڪڏهن_وصف_ٿيل CONFIG_VTX
    حالة TCBSetEPTRoot:
        أرجع decode_سيٽ_EPTRoot(cap);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
    حالة TCBConfigureSingleStepping:
        أرجع decodeConfigureSingleStepping(cap, call, بفر);

    حالة TCBSetBreakpoint:
        أرجع decode_سيٽ_Breakpoint(cap, بفر);

    حالة TCBGetBreakpoint:
        أرجع decodeGetBreakpoint(cap, call, بفر);

    حالة TCBUnsetBreakpoint:
        أرجع decodeUnsetBreakpoint(cap, بفر);
#جيڪڏهن_پورو

    حالة TCBSetTLSBase:
        أرجع decode_سيٽ_TLSBase(cap, ڊيگهائي, بفر);

    حالة TCBSetFlags:
        أرجع decode_سيٽ_Flags(cap, ڊيگهائي, call, بفر);

    افتراضي:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

تعداد CopyRegistersFlags {
    Copy_Registers_suspend_ذريعو = 0,
    Copy_Registers_resume_هدف = 1,
    Copy_Registers_transfer_چوکٺو = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    لفظ_t transferArch;
    tcb_t *srcTCB;
    cap_t ذريعو_cap;
    لفظ_t flags;

    إذا (ڊيگهائي < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);

    transferArch = Arch_decodeTransfer(flags >> 8);

    ذريعو_cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(ذريعو_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(ذريعو_cap));
    } وإلا {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & ٻٽو_انگ(Copy_Registers_suspend_ذريعو),
               flags & ٻٽو_انگ(Copy_Registers_resume_هدف),
               flags & ٻٽو_انگ(Copy_Registers_transfer_چوکٺو),
               flags & ٻٽو_انگ(CopyRegisters_transferInteger),
               transferArch);

}

تعداد ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, لفظ_t ڊيگهائي, bool_t call,
                                لفظ_t *بفر)
{
    لفظ_t transferArch, flags, n;
    tcb_t *thread;

    إذا (ڊيگهائي < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);
    n     = getSyscallArg(1, بفر);

    إذا (n < 1 || n > n_چوکٺو_Registers + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (صحيح)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_چوکٺو_Registers +
                                              n_gpRegisters;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    إذا (thread == ڳنڍ_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & ٻٽو_انگ(ReadRegisters_suspend),
               n, transferArch, call);
}

تعداد WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    لفظ_t flags, w;
    لفظ_t transferArch;
    tcb_t *thread;

    إذا (ڊيگهائي < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);
    w     = getSyscallArg(1, بفر);

    إذا (ڊيگهائي - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (صحيح)(ڊيگهائي - 2), (صحيح)w);
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    إذا (thread == ڳنڍ_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_WriteRegisters(thread,
                                    flags & ٻٽو_انگ(WriteRegisters_resume),
                                    w, transferArch, بفر);
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap:
        أرجع (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_cap_Can_Grant_جواب(cap)));
    حالة cap_null_cap:
        /* just has no fault endpoint */
        أرجع true;
    افتراضي:
        أرجع false;
    }
}
#جيڪڏهن_پورو

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, لفظ_t ڊيگهائي, cte_t *جڳهه, لفظ_t *بفر)
{
    cte_t *بفر_جڳهه, *c_پاڙ_جڳهه, *v_پاڙ_جڳهه;
    cap_t بفر_Cap, c_پاڙ_Cap, v_پاڙ_Cap;
    deriveCap_ret_t dc_ret;
    لفظ_t c_پاڙ_معلومات, v_پاڙ_معلومات, بفر_Addr;
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#وصف_ڪريو TCBCONFIGURE_دليلون 3
#وإلا
#وصف_ڪريو TCBCONFIGURE_دليلون 4
#جيڪڏهن_پورو
    إذا (ڊيگهائي < TCBCONFIGURE_دليلون || current_extra_caps.excaprefs[0] == عدم
        || current_extra_caps.excaprefs[1] == عدم
        || current_extra_caps.excaprefs[2] == عدم) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    c_پاڙ_معلومات     = getSyscallArg(0, بفر);
    v_پاڙ_معلومات     = getSyscallArg(1, بفر);
    بفر_Addr    = getSyscallArg(2, بفر);
#وإلا
    cptr_t faultEP       = getSyscallArg(0, بفر);
    c_پاڙ_معلومات     = getSyscallArg(1, بفر);
    v_پاڙ_معلومات     = getSyscallArg(2, بفر);
    بفر_Addr    = getSyscallArg(3, بفر);
#جيڪڏهن_پورو

    c_پاڙ_جڳهه  = current_extra_caps.excaprefs[0];
    c_پاڙ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_پاڙ_جڳهه  = current_extra_caps.excaprefs[1];
    v_پاڙ_Cap   = current_extra_caps.excaprefs[1]->cap;
    بفر_جڳهه = current_extra_caps.excaprefs[2];
    بفر_Cap  = current_extra_caps.excaprefs[2]->cap;

    إذا (بفر_Addr == 0) {
        بفر_جڳهه = عدم;
    } وإلا {
        dc_ret = deriveCap(بفر_جڳهه, بفر_Cap);
        إذا (dc_ret.حالت != EXCEPTION_NONE) {
            أرجع dc_ret.حالت;
        }
        بفر_Cap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(بفر_Addr, بفر_Cap);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (جڳهه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        جڳهه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (c_پاڙ_معلومات != 0) {
        c_پاڙ_Cap = update_Cap_معلومات(false, c_پاڙ_معلومات, c_پاڙ_Cap);
    }

    dc_ret = deriveCap(c_پاڙ_جڳهه, c_پاڙ_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    c_پاڙ_Cap = dc_ret.cap;

    إذا (cap_get_capType(c_پاڙ_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (v_پاڙ_معلومات != 0) {
        v_پاڙ_Cap = update_Cap_معلومات(false, v_پاڙ_معلومات, v_پاڙ_Cap);
    }

    dc_ret = deriveCap(v_پاڙ_جڳهه, v_پاڙ_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    v_پاڙ_Cap = dc_ret.cap;

    إذا (!is_Valid_VTable_پاڙ(v_پاڙ_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جڳهه,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               c_پاڙ_Cap, c_پاڙ_جڳهه,
               v_پاڙ_Cap, v_پاڙ_جڳهه,
               بفر_Addr, بفر_Cap,
               بفر_جڳهه, thread_control_caps_update_space |
               thread_control_caps_update_ipc_بفر);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جڳهه,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_پاڙ_Cap, c_پاڙ_جڳهه,
               v_پاڙ_Cap, v_پاڙ_جڳهه,
               بفر_Addr, بفر_Cap,
               بفر_جڳهه, thread_control_update_space |
               thread_control_update_ipc_بفر);
#جيڪڏهن_پورو
}

exception_t decode_سيٽ_ترجيح(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    إذا (ڊيگهائي < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت = checkPrio(newPrio, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (بنانشان ڊگهو) newPrio, (بنانشان ڊگهو) authTCB->tcbMCP);
        أرجع حالت;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               cap_null_cap_new(), عدم,
               NULL_PRIO, newPrio,
               عدم, thread_control_sched_update_ترجيح);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(),
               عدم, thread_control_update_ترجيح);
#جيڪڏهن_پورو
}

exception_t decode_سيٽ_MCPriority(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    إذا (ڊيگهائي < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت = checkPrio(newMcp, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (بنانشان ڊگهو) newMcp, (بنانشان ڊگهو) authTCB->tcbMCP);
        أرجع حالت;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
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
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
exception_t decode_سيٽ_Timeout_Endpoint(cap_t cap, cte_t *جڳهه)
{
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *th_جڳهه = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    إذا (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جڳهه,
               cap_null_cap_new(), عدم,
               thCap, th_جڳهه,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(), عدم,
               thread_control_caps_update_timeout);
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
exception_t decode_سيٽ_Sched_Params(cap_t cap, لفظ_t ڊيگهائي, cte_t *جڳهه, لفظ_t *بفر)
#وإلا
exception_t decode_سيٽ_Sched_Params(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
#جيڪڏهن_پورو
{
    إذا (ڊيگهائي < 2 || current_extra_caps.excaprefs[0] == عدم
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == عدم || current_extra_caps.excaprefs[2] == عدم
#جيڪڏهن_پورو
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, بفر);
    prio_t newPrio = getSyscallArg(1, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fh_جڳهه = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#جيڪڏهن_پورو

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت = checkPrio(newMcp, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (بنانشان ڊگهو) newMcp, (بنانشان ڊگهو) authTCB->tcbMCP);
        أرجع حالت;
    }

    حالت = checkPrio(newPrio, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (بنانشان ڊگهو) newPrio, (بنانشان ڊگهو) authTCB->tcbMCP);
        أرجع حالت;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = عدم;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_ترجيح |
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
        إذا (tcb == ڳنڍ_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        اكسر;
    افتراضي:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    إذا (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    إذا (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#جيڪڏهن_پورو
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جڳهه,
               fhCap, fh_جڳهه,
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
               thread_control_update_ترجيح);
#جيڪڏهن_پورو
}


exception_t decode_سيٽ_IPCBuffer(cap_t cap, لفظ_t ڊيگهائي, cte_t *جڳهه, لفظ_t *بفر)
{
    cptr_t cptr_بفر_Ptr;
    cap_t بفر_Cap;
    cte_t *بفر_جڳهه;

    إذا (ڊيگهائي < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cptr_بفر_Ptr  = getSyscallArg(0, بفر);
    بفر_جڳهه = current_extra_caps.excaprefs[0];
    بفر_Cap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cptr_بفر_Ptr == 0) {
        بفر_جڳهه = عدم;
    } وإلا {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(بفر_جڳهه, بفر_Cap);
        إذا (dc_ret.حالت != EXCEPTION_NONE) {
            أرجع dc_ret.حالت;
        }
        بفر_Cap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_بفر_Ptr, بفر_Cap);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جڳهه,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cptr_بفر_Ptr, بفر_Cap,
               بفر_جڳهه, thread_control_caps_update_ipc_بفر);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جڳهه,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cptr_بفر_Ptr, بفر_Cap,
               بفر_جڳهه, thread_control_update_ipc_بفر);

#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#وصف_ڪريو DECODE_سيٽ_SPACE_PARAMS 2
#وإلا
#وصف_ڪريو DECODE_سيٽ_SPACE_PARAMS 3
#جيڪڏهن_پورو
exception_t decode_سيٽ_Space(cap_t cap, لفظ_t ڊيگهائي, cte_t *جڳهه, لفظ_t *بفر)
{
    لفظ_t c_پاڙ_معلومات, v_پاڙ_معلومات;
    cte_t *c_پاڙ_جڳهه, *v_پاڙ_جڳهه;
    cap_t c_پاڙ_Cap, v_پاڙ_Cap;
    deriveCap_ret_t dc_ret;

    إذا (ڊيگهائي < DECODE_سيٽ_SPACE_PARAMS || current_extra_caps.excaprefs[0] == عدم
        || current_extra_caps.excaprefs[1] == عدم
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == عدم
#جيڪڏهن_پورو
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    c_پاڙ_معلومات = getSyscallArg(0, بفر);
    v_پاڙ_معلومات = getSyscallArg(1, بفر);

    cte_t *fh_جڳهه     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_پاڙ_جڳهه  = current_extra_caps.excaprefs[1];
    c_پاڙ_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_پاڙ_جڳهه  = current_extra_caps.excaprefs[2];
    v_پاڙ_Cap   = current_extra_caps.excaprefs[2]->cap;
#وإلا
    cptr_t faultEP   = getSyscallArg(0, بفر);
    c_پاڙ_معلومات = getSyscallArg(1, بفر);
    v_پاڙ_معلومات = getSyscallArg(2, بفر);

    c_پاڙ_جڳهه  = current_extra_caps.excaprefs[0];
    c_پاڙ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_پاڙ_جڳهه  = current_extra_caps.excaprefs[1];
    v_پاڙ_Cap   = current_extra_caps.excaprefs[1]->cap;
#جيڪڏهن_پورو

    إذا (جڳهه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        جڳهه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (c_پاڙ_معلومات != 0) {
        c_پاڙ_Cap = update_Cap_معلومات(false, c_پاڙ_معلومات, c_پاڙ_Cap);
    }

    dc_ret = deriveCap(c_پاڙ_جڳهه, c_پاڙ_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    c_پاڙ_Cap = dc_ret.cap;

    إذا (cap_get_capType(c_پاڙ_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (v_پاڙ_معلومات != 0) {
        v_پاڙ_Cap = update_Cap_معلومات(false, v_پاڙ_معلومات, v_پاڙ_Cap);
    }

    dc_ret = deriveCap(v_پاڙ_جڳهه, v_پاڙ_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    v_پاڙ_Cap = dc_ret.cap;

    إذا (!is_Valid_VTable_پاڙ(v_پاڙ_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    /* fault handler */
    إذا (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#جيڪڏهن_پورو

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جڳهه,
               fhCap, fh_جڳهه,
               cap_null_cap_new(), عدم,
               c_پاڙ_Cap, c_پاڙ_جڳهه,
               v_پاڙ_Cap, v_پاڙ_جڳهه,
               0, cap_null_cap_new(), عدم, thread_control_caps_update_space | thread_control_caps_update_fault);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جڳهه,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_پاڙ_Cap, c_پاڙ_جڳهه,
               v_پاڙ_Cap, v_پاڙ_جڳهه,
               0, cap_null_cap_new(), عدم, thread_control_update_space);
#جيڪڏهن_پورو
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
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

    إذا ((tcb_t *)notification_ptr_get_ntfn_قطار_سر(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }


    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
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

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
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

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
ساكن مضمن exception_t installTCBCap(tcb_t *هدف, cap_t tCap, cte_t *جڳهه,
                                        tcb_cnode_اشاريو_t اشاريو, cap_t newCap, cte_t *src_جڳهه)
{
    cte_t *پاڙ_جڳهه = TCB_PTR_CTE_PTR(هدف, اشاريو);
    اڻ_استعمال_ٿيل exception_t e = cteDelete(پاڙ_جڳهه, true);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    إذا (cap_get_capType(newCap) != cap_null_cap) {
        إذا (sameObjectAs(newCap, src_جڳهه->cap) &&
            sameObjectAs(tCap, جڳهه->cap)) {
            cte_وجهو(newCap, src_جڳهه, پاڙ_جڳهه);
        }
    }
    أرجع e;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *هدف, cte_t *جڳهه,
                                        cap_t fh_newCap, cte_t *fh_src_جڳهه,
                                        cap_t th_newCap, cte_t *th_src_جڳهه,
                                        cap_t c_پاڙ_new_Cap, cte_t *c_پاڙ_src_جڳهه,
                                        cap_t v_پاڙ_new_Cap, cte_t *v_پاڙ_src_جڳهه,
                                        لفظ_t بفر_Addr, cap_t بفر_Cap,
                                        cte_t *بفر_Src_جڳهه,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((لفظ_t)هدف);

    إذا (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(هدف, tCap, جڳهه, tcbFaultHandler, fh_newCap, fh_src_جڳهه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }

    }

    إذا (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(هدف, tCap, جڳهه, tcbTimeoutHandler, th_newCap, th_src_جڳهه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(هدف, tCap, جڳهه, tcbCTable, c_پاڙ_new_Cap, c_پاڙ_src_جڳهه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }

        e = installTCBCap(هدف, tCap, جڳهه, tcbVTable, v_پاڙ_new_Cap, v_پاڙ_src_جڳهه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_caps_update_ipc_بفر) {
        cte_t *بفر_جڳهه;

        بفر_جڳهه = TCB_PTR_CTE_PTR(هدف, tcb_بفر);
        e = cteDelete(بفر_جڳهه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        هدف->tcbIPCBuffer = بفر_Addr;

        إذا (بفر_Src_جڳهه && sameObjectAs(بفر_Cap, بفر_Src_جڳهه->cap) &&
            sameObjectAs(tCap, جڳهه->cap)) {
            cte_وجهو(بفر_Cap, بفر_Src_جڳهه, بفر_جڳهه);
        }

        إذا (هدف == ڳنڍ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t invokeTCB_ThreadControl(tcb_t *هدف, cte_t *جڳهه,
                                    cptr_t faultep, prio_t mcp, prio_t ترجيح,
                                    cap_t c_پاڙ_new_Cap, cte_t *c_پاڙ_src_جڳهه,
                                    cap_t v_پاڙ_new_Cap, cte_t *v_پاڙ_src_جڳهه,
                                    لفظ_t بفر_Addr, cap_t بفر_Cap,
                                    cte_t *بفر_Src_جڳهه,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((لفظ_t)هدف);

    إذا (updateFlags & thread_control_update_space) {
        هدف->tcbFaultHandler = faultep;
    }

    إذا (updateFlags & thread_control_update_mcp) {
        سيٽ_MCPriority(هدف, mcp);
    }

    إذا (updateFlags & thread_control_update_space) {
        cte_t *پاڙ_جڳهه;

        پاڙ_جڳهه = TCB_PTR_CTE_PTR(هدف, tcbCTable);
        e = cteDelete(پاڙ_جڳهه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        إذا (sameObjectAs(c_پاڙ_new_Cap, c_پاڙ_src_جڳهه->cap) &&
            sameObjectAs(tCap, جڳهه->cap)) {
            cte_وجهو(c_پاڙ_new_Cap, c_پاڙ_src_جڳهه, پاڙ_جڳهه);
        }
    }

    إذا (updateFlags & thread_control_update_space) {
        cte_t *پاڙ_جڳهه;

        پاڙ_جڳهه = TCB_PTR_CTE_PTR(هدف, tcbVTable);
        e = cteDelete(پاڙ_جڳهه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        إذا (sameObjectAs(v_پاڙ_new_Cap, v_پاڙ_src_جڳهه->cap) &&
            sameObjectAs(tCap, جڳهه->cap)) {
            cte_وجهو(v_پاڙ_new_Cap, v_پاڙ_src_جڳهه, پاڙ_جڳهه);
        }
    }

    إذا (updateFlags & thread_control_update_ipc_بفر) {
        cte_t *بفر_جڳهه;

        بفر_جڳهه = TCB_PTR_CTE_PTR(هدف, tcb_بفر);
        e = cteDelete(بفر_جڳهه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        هدف->tcbIPCBuffer = بفر_Addr;

        إذا (بفر_Src_جڳهه && sameObjectAs(بفر_Cap, بفر_Src_جڳهه->cap) &&
            sameObjectAs(tCap, جڳهه->cap)) {
            cte_وجهو(بفر_Cap, بفر_Src_جڳهه, بفر_جڳهه);
        }

        إذا (هدف == ڳنڍ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    إذا (updateFlags & thread_control_update_ترجيح) {
        سيٽ_ترجيح(هدف, ترجيح);
    }

    أرجع EXCEPTION_NONE;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *هدف, cte_t *جڳهه,
                                         cap_t fh_newCap, cte_t *fh_src_جڳهه,
                                         prio_t mcp, prio_t ترجيح,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    إذا (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((لفظ_t)هدف);
        exception_t e = installTCBCap(هدف, tCap, جڳهه, tcbFaultHandler, fh_newCap, fh_src_جڳهه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_sched_update_mcp) {
        سيٽ_MCPriority(هدف, mcp);
    }

    إذا (updateFlags & thread_control_sched_update_ترجيح) {
        سيٽ_ترجيح(هدف, ترجيح);
    }

    إذا (updateFlags & thread_control_sched_update_sc) {
        إذا (sc != عدم) {
            schedContext_bindTCB(sc, هدف);
        } وإلا إذا (sc == عدم) {
            schedContext_unbindTCB(هدف->tcbSchedContext);
        }
    }

    أرجع EXCEPTION_NONE;
}
#جيڪڏهن_پورو

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspend_ذريعو, bool_t resume_هدف,
                                    bool_t transfer_چوکٺو, bool_t transferInteger,
                                    لفظ_t transferArch)
{
    إذا (suspend_ذريعو) {
        suspend(tcb_src);
    }

    إذا (resume_هدف) {
        restart(dest);
    }

    إذا (transfer_چوکٺو) {
        لفظ_t i;
        لفظ_t v;
        لفظ_t pc;

        لكل (i = 0; i < n_چوکٺو_Registers; i++) {
            v = get_رجسٽر(tcb_src, چوکٺو_Registers[i]);
            سيٽ_رجسٽر(dest, چوکٺو_Registers[i], v);
        }

        pc = getRestartPC(dest);
        سيٽ_ايندڙ_PC(dest, pc);
    }

    إذا (transferInteger) {
        لفظ_t i;
        لفظ_t v;

        لكل (i = 0; i < n_gpRegisters; i++) {
            v = get_رجسٽر(tcb_src, gpRegisters[i]);
            سيٽ_رجسٽر(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    إذا (dest == ڳنڍ_STATE(ksCurThread)) {
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
exception_t invokeTCB_ReadRegisters(tcb_t *tcb_src, bool_t suspend_ذريعو,
                                    لفظ_t n, لفظ_t arch, bool_t call)
{
    لفظ_t i, j;
    exception_t e;
    tcb_t *thread;

    thread = ڳنڍ_STATE(ksCurThread);

    إذا (suspend_ذريعو) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, ڳنڍ_STATE(ksCurThread));
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    إذا (call) {
        لفظ_t *ipc_بفر;

        ipc_بفر = lookupIPCBuffer(true, thread);

        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);

        لكل (i = 0; i < n && i < n_چوکٺو_Registers && i < n_نياپو_Registers; i++) {
            سيٽ_رجسٽر(thread, نياپو_Registers[i],
                        get_رجسٽر(tcb_src, چوکٺو_Registers[i]));
        }

        إذا (ipc_بفر != عدم && i < n && i < n_چوکٺو_Registers) {
            لكل (; i < n && i < n_چوکٺو_Registers; i++) {
                ipc_بفر[i + 1] = get_رجسٽر(tcb_src, چوکٺو_Registers[i]);
            }
        }

        j = i;

        لكل (i = 0; i < n_gpRegisters && i + n_چوکٺو_Registers < n
             && i + n_چوکٺو_Registers < n_نياپو_Registers; i++) {
            سيٽ_رجسٽر(thread, نياپو_Registers[i + n_چوکٺو_Registers],
                        get_رجسٽر(tcb_src, gpRegisters[i]));
        }

        إذا (ipc_بفر != عدم && i < n_gpRegisters
            && i + n_چوکٺو_Registers < n) {
            لكل (; i < n_gpRegisters && i + n_چوکٺو_Registers < n; i++) {
                ipc_بفر[i + n_چوکٺو_Registers + 1] =
                    get_رجسٽر(tcb_src, gpRegisters[i]);
            }
        }

        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, i + j)));
    }
    سيٽ_Thread_State(thread, ThreadState_Running);

    أرجع EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resume_هدف,
                                     لفظ_t n, لفظ_t arch, لفظ_t *بفر)
{
    لفظ_t i;
    لفظ_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, ڳنڍ_STATE(ksCurThread), dest);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    إذا (n > n_چوکٺو_Registers + n_gpRegisters) {
        n = n_چوکٺو_Registers + n_gpRegisters;
    }

    archInfo = Arch_get_Sanitise_رجسٽر_Info(dest);

    لكل (i = 0; i < n_چوکٺو_Registers && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        سيٽ_رجسٽر(dest, چوکٺو_Registers[i],
                    sanitise_رجسٽر(چوکٺو_Registers[i],
                                     getSyscallArg(i + 2, بفر), archInfo));
    }

    لكل (i = 0; i < n_gpRegisters && i + n_چوکٺو_Registers < n; i++) {
        سيٽ_رجسٽر(dest, gpRegisters[i],
                    sanitise_رجسٽر(gpRegisters[i],
                                     getSyscallArg(i + n_چوکٺو_Registers + 2,
                                                   بفر), archInfo));
    }

    pc = getRestartPC(dest);
    سيٽ_ايندڙ_PC(dest, pc);

    Arch_postModifyRegisters(dest);

    إذا (resume_هدف) {
        restart(dest);
    }

    إذا (dest == ڳنڍ_STATE(ksCurThread)) {
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

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
فراغ سيٽ_Thread_Name(tcb_t *tcb, ثابت حرف *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_ڊيگهائي);
}
#جيڪڏهن_پورو

لفظ_t سيٽ_MRs_syscall_error(tcb_t *thread, لفظ_t *receiveIPCBuffer)
{
    بدل (current_syscall_error.type) {
    حالة se_L4_Invalid_دليل:
        أرجع سيٽ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_دليل_عدد);

    حالة seL4_InvalidCapability:
        أرجع سيٽ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_عدد);

    حالة seL4_IllegalOperation:
        أرجع 0;

    حالة seL4_RangeError:
        سيٽ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        أرجع سيٽ_MR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    حالة seL4_AlignmentError:
        أرجع 0;

    حالة seL4_FailedLookup:
        سيٽ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failed_Lookup_Was_ذريعو ? 1 : 0);
        أرجع سيٽ_MRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    حالة se_L4_Truncated_پيغام:
    حالة se_L4_Delete_پهريون:
    حالة se_L4_Revoke_پهريون:
        أرجع 0;
    حالة se_L4_Not_Enough_يادگيري:
        أرجع سيٽ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.يادگيري_کاٻو);
    افتراضي:
        fail("Invalid syscall error");
    }
}
