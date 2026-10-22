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
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#شامل <object/schedcontext.h>
#اگر_ختم
#شامل <object/tcb.h>
#شامل <kernel/cspace.h>
#شامل <kernel/thread.h>
#شامل <kernel/vspace.h>
#شامل <model/statedata.h>
#شامل <util.h>
#شامل <string.h>
#شامل <stdint.h>
#شامل <arch/smp/ipi_inline.h>

#تعریف_کرو NULL_PRIO 0

ساکن exception_t checkPrio(prio_t prio, tcb_t *auth)
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
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    واپس EXCEPTION_NONE;
}

ساکن اندرونی خالی جوڑو_مقصد_Bitmap(لفظ_t cpu, لفظ_t dom, لفظ_t prio)
{
    لفظ_t l1index;
    لفظ_t l1index_inverted;

    l1index = prio_مقصد_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    گرہ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= ثنائی_ہندسہ(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    گرہ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= ثنائی_ہندسہ(prio & نقاب(لفظ_Radix));
}

ساکن اندرونی خالی removeFromBitmap(لفظ_t cpu, لفظ_t dom, لفظ_t prio)
{
    لفظ_t l1index;
    لفظ_t l1index_inverted;

    l1index = prio_مقصد_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    گرہ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~ثنائی_ہندسہ(prio & نقاب(لفظ_Radix));
    اگر (unlikely(!گرہ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        گرہ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~ثنائی_ہندسہ(l1index);
    }
}

tcb_قطار_t tcb_قطار_remove(tcb_قطار_t قطار, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcb_Sched_پچھلا;
    after = tcb->tcb_Sched_اگلا;

    اگر (قطار.سر == tcb && قطار.انتہا == tcb) {
        قطار.سر = تہی;
        قطار.انتہا = تہی;
    } ورنہ {
        اگر (قطار.سر == tcb) {
            after->tcb_Sched_پچھلا = تہی;
            tcb->tcb_Sched_اگلا = تہی;
            قطار.سر = after;
        } ورنہ {
            اگر (قطار.انتہا == tcb) {
                before->tcb_Sched_اگلا = تہی;
                tcb->tcb_Sched_پچھلا = تہی;
                قطار.انتہا = before;
            } ورنہ {
                before->tcb_Sched_اگلا = after;
                after->tcb_Sched_پچھلا = before;
                tcb->tcb_Sched_پچھلا = تہی;
                tcb->tcb_Sched_اگلا = تہی;
            }
        }
    }

    واپس قطار;
}

/* Add TCB to the head of a scheduler queue */
خالی tcb_Sched_قطار_میں_ڈالو(tcb_t *tcb)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
#اگر_ختم

    اگر (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_قطار_t قطار;
        dom_t dom;
        prio_t prio;
        لفظ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ترجیح;
        idx = ready_queues_اشاریہ(dom, prio);
        قطار = گرہ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        اگر (tcb_قطار_خالی_خانے(قطار)) {
            جوڑو_مقصد_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        گرہ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_قطار_prepend(قطار, tcb);

        thread_state_ptr_سیٹ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
خالی tcbSchedAppend(tcb_t *tcb)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
    أكد(refill_ready(tcb->tcbSchedContext));
#اگر_ختم
    اگر (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_قطار_t قطار;
        dom_t dom;
        prio_t prio;
        لفظ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ترجیح;
        idx = ready_queues_اشاریہ(dom, prio);
        قطار = گرہ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        اگر (tcb_قطار_خالی_خانے(قطار)) {
            جوڑو_مقصد_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        گرہ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_قطار_append(قطار, tcb);

        thread_state_ptr_سیٹ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
خالی tcb_Sched_قطار_سے_نکالو(tcb_t *tcb)
{
    اگر (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_قطار_t قطار;
        tcb_قطار_t new_قطار;
        dom_t dom;
        prio_t prio;
        لفظ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ترجیح;
        idx = ready_queues_اشاریہ(dom, prio);
        قطار = گرہ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_قطار = tcb_قطار_remove(قطار, tcb);

        گرہ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_قطار;

        thread_state_ptr_سیٹ_tcb_Queued(&tcb->tcbState, false);

        اگر (likely(tcb_قطار_خالی_خانے(new_قطار))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
خالی tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcb_Debug_پچھلا = تہی;

    debug_tcb->tcb_Debug_اگلا = گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    اگر (گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcb_Debug_پچھلا = tcb;
    }

    گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

خالی tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    أكد(گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != تہی);
    اگر (tcb == گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(گرہ_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcb_Debug_اگلا;
    } ورنہ {
        أكد(TCB_PTR_DEBUG_PTR(tcb)->tcb_Debug_پچھلا);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_پچھلا)->tcb_Debug_اگلا = debug_tcb->tcb_Debug_اگلا;
    }

    اگر (debug_tcb->tcb_Debug_اگلا) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_اگلا)->tcb_Debug_پچھلا = debug_tcb->tcb_Debug_پچھلا;
    }

    debug_tcb->tcb_Debug_پچھلا = تہی;
    debug_tcb->tcb_Debug_اگلا = تہی;
}
#اگر_ختم /* CONFIG_DEBUG_BUILD */

#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_قطار_t tcbEPAppend(tcb_t *tcb, tcb_قطار_t قطار)
{
    اگر (!قطار.سر) { /* Empty list */
        قطار.سر = tcb;
    } ورنہ {
        قطار.انتہا->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = قطار.انتہا;
    tcb->tcbEPNext = تہی;
    قطار.انتہا = tcb;

    واپس قطار;
}

/* Remove TCB from an endpoint queue */
tcb_قطار_t tcbEPDequeue(tcb_t *tcb, tcb_قطار_t قطار)
{
    اگر (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } ورنہ {
        قطار.سر = tcb->tcbEPNext;
    }

    اگر (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } ورنہ {
        قطار.انتہا = tcb->tcbEPPrev;
    }

    واپس قطار;
}
#اگر_ختم /* CONFIG_KERNEL_MCS */

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS

خالی tcbReleaseRemove(tcb_t *tcb)
{
    اگر (likely(thread_state_get_tcb_اندر_Release_قطار(tcb->tcbState))) {
        tcb_قطار_t قطار = گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity);

        اگر (قطار.سر == tcb) {
            گرہ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity) = tcb_قطار_remove(قطار, tcb);

        thread_state_ptr_سیٹ_tcb_اندر_Release_قطار(&tcb->tcbState, false);
    }
}

ساکن اندرونی ticks_t خالص tcb_Ready_وقت(tcb_t *tcb)
{
    واپس refill_سر(tcb->tcbSchedContext)->r_وقت;
}

ساکن اندرونی bool_t خالص وقت_after(tcb_t *tcb, ticks_t new_وقت)
{
    واپس tcb != تہی && new_وقت >= tcb_Ready_وقت(tcb);
}

ساکن tcb_t *ڈھونڈو_وقت_after(tcb_t *tcb, ticks_t new_وقت)
{
    tcb_t *after = tcb;

    جب (وقت_after(after, new_وقت)) {
        after = after->tcb_Sched_اگلا;
    }

    واپس after;
}

خالی tcb_Release_قطار_میں_ڈالو(tcb_t *tcb)
{
    أكد(thread_state_get_tcb_اندر_Release_قطار(tcb->tcbState) == false);
    أكد(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_وقت;
    tcb_قطار_t قطار;

    قطار = گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity);
    new_وقت = tcb_Ready_وقت(tcb);

    اگر (tcb_قطار_خالی_خانے(قطار) || new_وقت < tcb_Ready_وقت(قطار.سر)) {
        گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity) = tcb_قطار_prepend(قطار, tcb);
    } ورنہ {
        اگر (tcb_Ready_وقت(قطار.انتہا) <= new_وقت) {
            گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity) = tcb_قطار_append(قطار, tcb);
        } ورنہ {
            tcb_t *after;
            after = ڈھونڈو_وقت_after(قطار.سر, new_وقت);
            tcb_قطار_داخل_کرو(tcb, after);
        }
    }

    thread_state_ptr_سیٹ_tcb_اندر_Release_قطار(&tcb->tcbState, true);

    اگر (قطار.سر != گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity).سر) {
        گرہ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#اگر_ختم

cptr_t خالص getExtraCPtr(لفظ_t *بفر_Ptr, لفظ_t i)
{
    واپس (cptr_t)بفر_Ptr[se_L4_پیغام_Max_لمبائی + 2 + i];
}

خالی سیٹ_Extra_Badge(لفظ_t *بفر_Ptr, لفظ_t badge,
                   لفظ_t i)
{
    بفر_Ptr[se_L4_پیغام_Max_لمبائی + 2 + i] = badge;
}

#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی تیاری_Caller_Cap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *جواب_خانہ, *caller_خانہ;
    cap_t masterCap غیر_مستعمل, callerCap غیر_مستعمل;

    سیٹ_Thread_State(sender, Thread_State_Blocked_On_جواب);
    جواب_خانہ = TCB_PTR_CTE_PTR(sender, tcb_جواب);
    masterCap = جواب_خانہ->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    أكد(cap_get_capType(masterCap) == cap_جواب_cap);
    أكد(cap_جواب_cap_get_cap_جواب_Master(masterCap));
    أكد(cap_جواب_cap_get_cap_جواب_Can_Grant(masterCap));
    أكد(TCB_PTR(cap_جواب_cap_get_cap_TCBPtr(masterCap)) == sender);
    caller_خانہ = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = caller_خانہ->cap;
    /* Haskell error: "Caller cap must not already exist" */
    أكد(cap_get_capType(callerCap) == cap_null_cap);
    cte_داخل_کرو(cap_جواب_cap_new(canGrant, false, TCB_REF(sender)),
              جواب_خانہ, caller_خانہ);
}

خالی deleteCallerCap(tcb_t *receiver)
{
    cte_t *caller_خانہ;

    caller_خانہ = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(caller_خانہ);
}
#اگر_ختم

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, لفظ_t *بفر_Ptr, se_L4_پیغام_Info_t info)
{
    lookup_خانہ_raw_ret_t lu_ret;
    cptr_t cptr;
    لفظ_t i, لمبائی;

    اگر (!بفر_Ptr) {
        current_extra_caps.excaprefs[0] = تہی;
        واپس EXCEPTION_NONE;
    }

    لمبائی = se_L4_پیغام_Info_get_extra_Caps(info);

    برائے (i = 0; i < لمبائی; i++) {
        cptr = getExtraCPtr(بفر_Ptr, i);

        lu_ret = lookup_خانہ(thread, cptr);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            واپس lu_ret.حالت_2;
        }

        current_extra_caps.excaprefs[i] = lu_ret.خانہ;
    }
    اگر (i < se_L4_پیغام_Max_Extra_Caps) {
        current_extra_caps.excaprefs[i] = تہی;
    }

    واپس EXCEPTION_NONE;
}

/* Copy IPC MRs from one thread to another */
لفظ_t copyMRs(tcb_t *sender, لفظ_t *sendBuf, tcb_t *receiver,
               لفظ_t *recvBuf, لفظ_t n)
{
    لفظ_t i;

    /* Copy inline words */
    برائے (i = 0; i < n && i < n_پیغام_Registers; i++) {
        سیٹ_رجسٹر(receiver, پیغام_Registers[i],
                    get_رجسٹر(sender, پیغام_Registers[i]));
    }

    اگر (!recvBuf || !sendBuf) {
        واپس i;
    }

    /* Copy out-of-line words */
    برائے (; i < n; i++) {
        recvBuf[i + 1] = sendBuf[i + 1];
    }

    واپس i;
}

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
خالی remote_قطار_Update(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    اگر (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *ہدف_Cur_Thread = گرہ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        اگر (ہدف_Cur_Thread == گرہ_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcb_ترجیح > ہدف_Cur_Thread->tcb_ترجیح
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
            || گرہ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#اگر_ختم
           ) {
            ARCH_گرہ_STATE(ipiReschedulePending) |= ثنائی_ہندسہ(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
خالی remoteTCBStall(tcb_t *tcb)
{

    اگر (
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#اگر_ختم
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        گرہ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_گرہ_STATE(ipiReschedulePending) |= ثنائی_ہندسہ(tcb->tcbAffinity);
    }
}

#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن exception_t invoke_TCB_سیٹ_Affinity(tcb_t *thread, لفظ_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcb_Sched_قطار_سے_نکالو(thread);
    migrateTCB(thread, affinity);
    اگر (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    اگر (thread == گرہ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    واپس EXCEPTION_NONE;
}

ساکن exception_t decode_سیٹ_Affinity(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    tcb_t *tcb;
    لفظ_t affinity;

    اگر (لمبائی < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, بفر);
    اگر (affinity >= ks_عدد_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invoke_TCB_سیٹ_Affinity(tcb, affinity);
}
#اگر_ختم
#اگر_ختم /* ENABLE_SMP_SUPPORT */

#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
ساکن exception_t invokeConfigureSingleStepping(bool_t call, لفظ_t *بفر, tcb_t *t,
                                                 uint16_t bp_عدد, لفظ_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    لفظ_t قدر;

    bp_was_consumed = configureSingleStepping(t, bp_عدد, n_instrs, false);
    اگر (n_instrs == 0) {
#اگر_غیر_تعریف_شدہ CONFIG_ARCH_AARCH64
        unset_Breakpoint_مستعمل_Flag(t, bp_عدد);
#اگر_ختم /* CONFIG_ARCH_AARCH64 */
        قدر = false;
    } ورنہ {
#اگر_غیر_تعریف_شدہ CONFIG_ARCH_AARCH64
        سیٹ_Breakpoint_مستعمل_Flag(t, bp_عدد);
#اگر_ختم /* CONFIG_ARCH_AARCH64 */
        قدر = bp_was_consumed;
    }

    اگر (call) {
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        غیرعلامتی صحیح لمبائی = سیٹ_MR(thread, بفر, 0, قدر);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, لمبائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, لفظ_t *بفر)
{
    uint16_t bp_عدد;
    لفظ_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_عدد = getSyscallArg(0, بفر);
    n_instrs = getSyscallArg(1, بفر);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_عدد, n_instrs, false);
    اگر (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeConfigureSingleStepping(call, بفر, tcb, bp_عدد, n_instrs);
}

ساکن exception_t invoke_سیٹ_Breakpoint(tcb_t *tcb, uint16_t bp_عدد,
                                       لفظ_t vaddr, لفظ_t type, لفظ_t سائز, لفظ_t rw)
{
    سیٹ_Breakpoint(tcb, bp_عدد, vaddr, type, سائز, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    سیٹ_Breakpoint_مستعمل_Flag(tcb, bp_عدد);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decode_سیٹ_Breakpoint(cap_t cap, لفظ_t *بفر)
{
    uint16_t bp_عدد;
    لفظ_t vaddr, type, سائز, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_عدد = getSyscallArg(0, بفر);
    vaddr = getSyscallArg(1, بفر);
    type = getSyscallArg(2, بفر);
    سائز = getSyscallArg(3, بفر);
    rw = getSyscallArg(4, بفر);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    اگر (vaddr >= (لفظ_t)USER_چوٹی) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (type != seL4_InstructionBreakpoint && type != se_L4_ڈیٹا_Breakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 2;
        واپس EXCEPTION_SYSCALL_ERROR;
    } ورنہ اگر (type == seL4_InstructionBreakpoint) {
        اگر (سائز != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 3;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        اگر (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 4;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        اگر ((se_L4_پہلا_Watchpoint == -1 || bp_عدد >= se_L4_پہلا_Watchpoint)
            && se_L4_پہلا_Breakpoint != se_L4_پہلا_Watchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 2;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
    } ورنہ اگر (type == se_L4_ڈیٹا_Breakpoint) {
        اگر (سائز == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 3;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        اگر (se_L4_پہلا_Watchpoint != -1 && bp_عدد < se_L4_پہلا_Watchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 2;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
    } ورنہ اگر (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 2;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 3;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (سائز != 0 && سائز != 1 && سائز != 2 && سائز != 4 && سائز != 8) {
        userError("Debug: Invalid size %lu.", سائز);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 3;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (سائز > 0 && vaddr & (سائز - 1)) {
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
                  vaddr, سائز);

        current_syscall_error.type = seL4_AlignmentError;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decode_سیٹ_Breakpoint(tcb, bp_عدد, vaddr, type, سائز, rw);
    اگر (error.type != seL4_NoError) {
        current_syscall_error = error;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invoke_سیٹ_Breakpoint(tcb, bp_عدد,
                               vaddr, type, سائز, rw);
}

ساکن exception_t invokeGetBreakpoint(bool_t call, لفظ_t *بفر, tcb_t *tcb, uint16_t bp_عدد)
{
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_عدد);
    اگر (call) {
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, 0, res.vaddr);
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, 1, res.type);
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, 2, res.سائز);
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, 3, res.rw);
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, 4, res.is_enabled);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, 5)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeGetBreakpoint(cap_t cap, bool_t call, لفظ_t *بفر)
{
    tcb_t *tcb;
    uint16_t bp_عدد;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_عدد = getSyscallArg(0, بفر);

    error = Arch_decodeGetBreakpoint(tcb, bp_عدد);
    اگر (error.type != seL4_NoError) {
        current_syscall_error = error;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeGetBreakpoint(call, بفر, tcb, bp_عدد);
}

ساکن exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_عدد)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_عدد);
    unset_Breakpoint_مستعمل_Flag(tcb, bp_عدد);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeUnsetBreakpoint(cap_t cap, لفظ_t *بفر)
{
    tcb_t *tcb;
    uint16_t bp_عدد;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_عدد = getSyscallArg(0, بفر);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_عدد);
    اگر (error.type != seL4_NoError) {
        current_syscall_error = error;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeUnsetBreakpoint(tcb, bp_عدد);
}
#اگر_ختم /* CONFIG_HARDWARE_DEBUG_API */

ساکن exception_t invoke_سیٹ_TLSBase(tcb_t *thread, لفظ_t tls_بنیاد)
{
    سیٹ_رجسٹر(thread, TLS_بنیاد, tls_بنیاد);
    اگر (thread == گرہ_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    واپس EXCEPTION_NONE;
}

ساکن exception_t decode_سیٹ_TLSBase(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    لفظ_t tls_بنیاد;

    اگر (لمبائی < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    tls_بنیاد = getSyscallArg(0, بفر);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invoke_سیٹ_TLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_بنیاد);
}

ساکن خالی invoke_سیٹ_Flags(tcb_t *thread, لفظ_t clear, لفظ_t سیٹ, bool_t call)
{
    tcb_t *cur_thread = گرہ_STATE(ksCurThread);
    لفظ_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= سیٹ & se_L4_TCBFlag_نقاب;
    thread->tcbFlags = flags;

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    اگر (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } ورنہ اگر (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#اگر_ختم
    اگر (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, cur_thread);
        سیٹ_رجسٹر(cur_thread, badge_رجسٹر, 0);
        غیرعلامتی صحیح لمبائی = سیٹ_MR(cur_thread, ipc_بفر, 0, flags);
        سیٹ_رجسٹر(cur_thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, لمبائی)));
    }
    سیٹ_Thread_State(cur_thread, ThreadState_Running);
}

ساکن exception_t decode_سیٹ_Flags(cap_t cap, لفظ_t لمبائی, bool_t call, لفظ_t *بفر)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    اگر (لمبائی < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t clear = getSyscallArg(0, بفر);
    لفظ_t سیٹ   = getSyscallArg(1, بفر);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invoke_سیٹ_Flags(thread, clear, سیٹ, call);
    واپس EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(لفظ_t invLabel, لفظ_t لمبائی, cap_t cap,
                                cte_t *خانہ, bool_t call, لفظ_t *بفر)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    انتخاب (invLabel) {
    حالت TCBReadRegisters:
        /* Second level of decoding */
        واپس decodeReadRegisters(cap, لمبائی, call, بفر);

    حالت TCBWriteRegisters:
        واپس decodeWriteRegisters(cap, لمبائی, بفر);

    حالت TCBCopyRegisters:
        واپس decodeCopyRegisters(cap, لمبائی, بفر);

    حالت TCBSuspend:
        /* Jump straight to the invoke */
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالت TCBResume:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالت TCBConfigure:
        واپس decodeTCBConfigure(cap, لمبائی, خانہ, بفر);

    حالت TCBSet_ترجیح:
        واپس decode_سیٹ_ترجیح(cap, لمبائی, بفر);

    حالت TCBSetMCPriority:
        واپس decode_سیٹ_MCPriority(cap, لمبائی, بفر);

    حالت TCBSetSchedParams:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        واپس decode_سیٹ_Sched_Params(cap, لمبائی, خانہ, بفر);
#ورنہ
        واپس decode_سیٹ_Sched_Params(cap, لمبائی, بفر);
#اگر_ختم

    حالت TCBSetIPCBuffer:
        واپس decode_سیٹ_IPCBuffer(cap, لمبائی, خانہ, بفر);

    حالت TCBSetSpace:
        واپس decode_سیٹ_Space(cap, لمبائی, خانہ, بفر);

    حالت TCBBindNotification:
        واپس decodeBindNotification(cap);

    حالت TCBUnbindNotification:
        واپس decodeUnbindNotification(cap);

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت TCBSetTimeoutEndpoint:
        واپس decode_سیٹ_Timeout_Endpoint(cap, خانہ);
#ورنہ
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    حالت TCBSetAffinity:
        واپس decode_سیٹ_Affinity(cap, لمبائی, بفر);
#اگر_ختم /* ENABLE_SMP_SUPPORT */
#اگر_ختم

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#اگر_تعریف_شدہ CONFIG_VTX
    حالت TCBSetEPTRoot:
        واپس decode_سیٹ_EPTRoot(cap);
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    حالت TCBConfigureSingleStepping:
        واپس decodeConfigureSingleStepping(cap, call, بفر);

    حالت TCBSetBreakpoint:
        واپس decode_سیٹ_Breakpoint(cap, بفر);

    حالت TCBGetBreakpoint:
        واپس decodeGetBreakpoint(cap, call, بفر);

    حالت TCBUnsetBreakpoint:
        واپس decodeUnsetBreakpoint(cap, بفر);
#اگر_ختم

    حالت TCBSetTLSBase:
        واپس decode_سیٹ_TLSBase(cap, لمبائی, بفر);

    حالت TCBSetFlags:
        واپس decode_سیٹ_Flags(cap, لمبائی, call, بفر);

    طےشدہ:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}

شمار CopyRegistersFlags {
    Copy_Registers_suspend_ماخذ = 0,
    Copy_Registers_resume_ہدف = 1,
    Copy_Registers_transfer_چوکھٹا = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    لفظ_t transferArch;
    tcb_t *srcTCB;
    cap_t ماخذ_cap;
    لفظ_t flags;

    اگر (لمبائی < 1 || current_extra_caps.excaprefs[0] == تہی) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);

    transferArch = Arch_decodeTransfer(flags >> 8);

    ماخذ_cap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(ماخذ_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(ماخذ_cap));
    } ورنہ {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & ثنائی_ہندسہ(Copy_Registers_suspend_ماخذ),
               flags & ثنائی_ہندسہ(Copy_Registers_resume_ہدف),
               flags & ثنائی_ہندسہ(Copy_Registers_transfer_چوکھٹا),
               flags & ثنائی_ہندسہ(CopyRegisters_transferInteger),
               transferArch);

}

شمار ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, لفظ_t لمبائی, bool_t call,
                                لفظ_t *بفر)
{
    لفظ_t transferArch, flags, n;
    tcb_t *thread;

    اگر (لمبائی < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);
    n     = getSyscallArg(1, بفر);

    اگر (n < 1 || n > n_چوکھٹا_Registers + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (صحیح)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_چوکھٹا_Registers +
                                              n_gpRegisters;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    اگر (thread == گرہ_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & ثنائی_ہندسہ(ReadRegisters_suspend),
               n, transferArch, call);
}

شمار WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    لفظ_t flags, w;
    لفظ_t transferArch;
    tcb_t *thread;

    اگر (لمبائی < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);
    w     = getSyscallArg(1, بفر);

    اگر (لمبائی - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (صحیح)(لمبائی - 2), (صحیح)w);
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    اگر (thread == گرہ_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeTCB_WriteRegisters(thread,
                                    flags & ثنائی_ہندسہ(WriteRegisters_resume),
                                    w, transferArch, بفر);
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_endpoint_cap:
        واپس (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_cap_Can_Grant_جواب(cap)));
    حالت cap_null_cap:
        /* just has no fault endpoint */
        واپس true;
    طےشدہ:
        واپس false;
    }
}
#اگر_ختم

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, لفظ_t لمبائی, cte_t *خانہ, لفظ_t *بفر)
{
    cte_t *بفر_خانہ, *c_جڑ_خانہ, *v_جڑ_خانہ;
    cap_t بفر_Cap, c_جڑ_Cap, v_جڑ_Cap;
    deriveCap_ret_t dc_ret;
    لفظ_t c_جڑ_ڈیٹا, v_جڑ_ڈیٹا, بفر_Addr;
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#تعریف_کرو TCBCONFIGURE_دلائل 3
#ورنہ
#تعریف_کرو TCBCONFIGURE_دلائل 4
#اگر_ختم
    اگر (لمبائی < TCBCONFIGURE_دلائل || current_extra_caps.excaprefs[0] == تہی
        || current_extra_caps.excaprefs[1] == تہی
        || current_extra_caps.excaprefs[2] == تہی) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    c_جڑ_ڈیٹا     = getSyscallArg(0, بفر);
    v_جڑ_ڈیٹا     = getSyscallArg(1, بفر);
    بفر_Addr    = getSyscallArg(2, بفر);
#ورنہ
    cptr_t faultEP       = getSyscallArg(0, بفر);
    c_جڑ_ڈیٹا     = getSyscallArg(1, بفر);
    v_جڑ_ڈیٹا     = getSyscallArg(2, بفر);
    بفر_Addr    = getSyscallArg(3, بفر);
#اگر_ختم

    c_جڑ_خانہ  = current_extra_caps.excaprefs[0];
    c_جڑ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_جڑ_خانہ  = current_extra_caps.excaprefs[1];
    v_جڑ_Cap   = current_extra_caps.excaprefs[1]->cap;
    بفر_خانہ = current_extra_caps.excaprefs[2];
    بفر_Cap  = current_extra_caps.excaprefs[2]->cap;

    اگر (بفر_Addr == 0) {
        بفر_خانہ = تہی;
    } ورنہ {
        dc_ret = deriveCap(بفر_خانہ, بفر_Cap);
        اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
            واپس dc_ret.حالت_2;
        }
        بفر_Cap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(بفر_Addr, بفر_Cap);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }
    }

    اگر (خانہ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        خانہ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (c_جڑ_ڈیٹا != 0) {
        c_جڑ_Cap = update_Cap_ڈیٹا(false, c_جڑ_ڈیٹا, c_جڑ_Cap);
    }

    dc_ret = deriveCap(c_جڑ_خانہ, c_جڑ_Cap);
    اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
        واپس dc_ret.حالت_2;
    }
    c_جڑ_Cap = dc_ret.cap;

    اگر (cap_get_capType(c_جڑ_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (v_جڑ_ڈیٹا != 0) {
        v_جڑ_Cap = update_Cap_ڈیٹا(false, v_جڑ_ڈیٹا, v_جڑ_Cap);
    }

    dc_ret = deriveCap(v_جڑ_خانہ, v_جڑ_Cap);
    اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
        واپس dc_ret.حالت_2;
    }
    v_جڑ_Cap = dc_ret.cap;

    اگر (!is_Valid_VTable_جڑ(v_جڑ_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    واپس invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خانہ,
               cap_null_cap_new(), تہی,
               cap_null_cap_new(), تہی,
               c_جڑ_Cap, c_جڑ_خانہ,
               v_جڑ_Cap, v_جڑ_خانہ,
               بفر_Addr, بفر_Cap,
               بفر_خانہ, thread_control_caps_update_space |
               thread_control_caps_update_ipc_بفر);
#ورنہ
    واپس invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خانہ,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_جڑ_Cap, c_جڑ_خانہ,
               v_جڑ_Cap, v_جڑ_خانہ,
               بفر_Addr, بفر_Cap,
               بفر_خانہ, thread_control_update_space |
               thread_control_update_ipc_بفر);
#اگر_ختم
}

exception_t decode_سیٹ_ترجیح(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    اگر (لمبائی < 1 || current_extra_caps.excaprefs[0] == تہی) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت_2 = checkPrio(newPrio, authTCB);
    اگر (حالت_2 != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (غیرعلامتی طویل) newPrio, (غیرعلامتی طویل) authTCB->tcbMCP);
        واپس حالت_2;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    واپس invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تہی,
               cap_null_cap_new(), تہی,
               NULL_PRIO, newPrio,
               تہی, thread_control_sched_update_ترجیح);
#ورنہ
    واپس invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تہی,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), تہی,
               cap_null_cap_new(), تہی,
               0, cap_null_cap_new(),
               تہی, thread_control_update_ترجیح);
#اگر_ختم
}

exception_t decode_سیٹ_MCPriority(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    اگر (لمبائی < 1 || current_extra_caps.excaprefs[0] == تہی) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت_2 = checkPrio(newMcp, authTCB);
    اگر (حالت_2 != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (غیرعلامتی طویل) newMcp, (غیرعلامتی طویل) authTCB->tcbMCP);
        واپس حالت_2;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    واپس invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تہی,
               cap_null_cap_new(), تہی,
               newMcp, NULL_PRIO,
               تہی, thread_control_sched_update_mcp);
#ورنہ
    واپس invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تہی,
               0, newMcp, NULL_PRIO,
               cap_null_cap_new(), تہی,
               cap_null_cap_new(), تہی,
               0, cap_null_cap_new(),
               تہی, thread_control_update_mcp);
#اگر_ختم
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t decode_سیٹ_Timeout_Endpoint(cap_t cap, cte_t *خانہ)
{
    اگر (current_extra_caps.excaprefs[0] == تہی) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *th_خانہ = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    اگر (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خانہ,
               cap_null_cap_new(), تہی,
               thCap, th_خانہ,
               cap_null_cap_new(), تہی,
               cap_null_cap_new(), تہی,
               0, cap_null_cap_new(), تہی,
               thread_control_caps_update_timeout);
}
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t decode_سیٹ_Sched_Params(cap_t cap, لفظ_t لمبائی, cte_t *خانہ, لفظ_t *بفر)
#ورنہ
exception_t decode_سیٹ_Sched_Params(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
#اگر_ختم
{
    اگر (لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == تہی || current_extra_caps.excaprefs[2] == تہی
#اگر_ختم
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, بفر);
    prio_t newPrio = getSyscallArg(1, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fh_خانہ = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#اگر_ختم

    اگر (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت_2 = checkPrio(newMcp, authTCB);
    اگر (حالت_2 != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (غیرعلامتی طویل) newMcp, (غیرعلامتی طویل) authTCB->tcbMCP);
        واپس حالت_2;
    }

    حالت_2 = checkPrio(newPrio, authTCB);
    اگر (حالت_2 != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (غیرعلامتی طویل) newPrio, (غیرعلامتی طویل) authTCB->tcbMCP);
        واپس حالت_2;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = تہی;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_ترجیح |
                                         thread_control_sched_update_fault;
    انتخاب (cap_get_capType(scCap)) {
    حالت cap_sched_context_cap:
        sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(scCap));
        اگر (tcb->tcbSchedContext != sc) {
            اگر (tcb->tcbSchedContext) {
                userError("TCB Configure: tcb already has a scheduling context.");
                current_syscall_error.type = seL4_IllegalOperation;
                واپس EXCEPTION_SYSCALL_ERROR;
            }
            اگر (sc->scTcb) {
                userError("TCB Configure: sched context already bound.");
                current_syscall_error.type = seL4_IllegalOperation;
                واپس EXCEPTION_SYSCALL_ERROR;
            }
        }
        اگر (isBlocked(tcb) && !sc_released(sc)) {
            userError("TCB Configure: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        توڑو;
    حالت cap_null_cap:
        اگر (tcb == گرہ_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        توڑو;
    طےشدہ:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 2;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    اگر (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    اگر (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 3;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
#اگر_ختم
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    واپس invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خانہ,
               fhCap, fh_خانہ,
               newMcp, newPrio,
               sc, update_flags);
#ورنہ
    واپس invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تہی,
               0, newMcp, newPrio,
               cap_null_cap_new(), تہی,
               cap_null_cap_new(), تہی,
               0, cap_null_cap_new(),
               تہی, thread_control_update_mcp |
               thread_control_update_ترجیح);
#اگر_ختم
}


exception_t decode_سیٹ_IPCBuffer(cap_t cap, لفظ_t لمبائی, cte_t *خانہ, لفظ_t *بفر)
{
    cptr_t cptr_بفر_Ptr;
    cap_t بفر_Cap;
    cte_t *بفر_خانہ;

    اگر (لمبائی < 1 || current_extra_caps.excaprefs[0] == تہی) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    cptr_بفر_Ptr  = getSyscallArg(0, بفر);
    بفر_خانہ = current_extra_caps.excaprefs[0];
    بفر_Cap  = current_extra_caps.excaprefs[0]->cap;

    اگر (cptr_بفر_Ptr == 0) {
        بفر_خانہ = تہی;
    } ورنہ {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(بفر_خانہ, بفر_Cap);
        اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
            واپس dc_ret.حالت_2;
        }
        بفر_Cap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_بفر_Ptr, بفر_Cap);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    واپس invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خانہ,
               cap_null_cap_new(), تہی,
               cap_null_cap_new(), تہی,
               cap_null_cap_new(), تہی,
               cap_null_cap_new(), تہی,
               cptr_بفر_Ptr, بفر_Cap,
               بفر_خانہ, thread_control_caps_update_ipc_بفر);
#ورنہ
    واپس invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خانہ,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), تہی,
               cap_null_cap_new(), تہی,
               cptr_بفر_Ptr, بفر_Cap,
               بفر_خانہ, thread_control_update_ipc_بفر);

#اگر_ختم
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#تعریف_کرو DECODE_سیٹ_SPACE_PARAMS 2
#ورنہ
#تعریف_کرو DECODE_سیٹ_SPACE_PARAMS 3
#اگر_ختم
exception_t decode_سیٹ_Space(cap_t cap, لفظ_t لمبائی, cte_t *خانہ, لفظ_t *بفر)
{
    لفظ_t c_جڑ_ڈیٹا, v_جڑ_ڈیٹا;
    cte_t *c_جڑ_خانہ, *v_جڑ_خانہ;
    cap_t c_جڑ_Cap, v_جڑ_Cap;
    deriveCap_ret_t dc_ret;

    اگر (لمبائی < DECODE_سیٹ_SPACE_PARAMS || current_extra_caps.excaprefs[0] == تہی
        || current_extra_caps.excaprefs[1] == تہی
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == تہی
#اگر_ختم
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    c_جڑ_ڈیٹا = getSyscallArg(0, بفر);
    v_جڑ_ڈیٹا = getSyscallArg(1, بفر);

    cte_t *fh_خانہ     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_جڑ_خانہ  = current_extra_caps.excaprefs[1];
    c_جڑ_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_جڑ_خانہ  = current_extra_caps.excaprefs[2];
    v_جڑ_Cap   = current_extra_caps.excaprefs[2]->cap;
#ورنہ
    cptr_t faultEP   = getSyscallArg(0, بفر);
    c_جڑ_ڈیٹا = getSyscallArg(1, بفر);
    v_جڑ_ڈیٹا = getSyscallArg(2, بفر);

    c_جڑ_خانہ  = current_extra_caps.excaprefs[0];
    c_جڑ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_جڑ_خانہ  = current_extra_caps.excaprefs[1];
    v_جڑ_Cap   = current_extra_caps.excaprefs[1]->cap;
#اگر_ختم

    اگر (خانہ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        خانہ_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (c_جڑ_ڈیٹا != 0) {
        c_جڑ_Cap = update_Cap_ڈیٹا(false, c_جڑ_ڈیٹا, c_جڑ_Cap);
    }

    dc_ret = deriveCap(c_جڑ_خانہ, c_جڑ_Cap);
    اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
        واپس dc_ret.حالت_2;
    }
    c_جڑ_Cap = dc_ret.cap;

    اگر (cap_get_capType(c_جڑ_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (v_جڑ_ڈیٹا != 0) {
        v_جڑ_Cap = update_Cap_ڈیٹا(false, v_جڑ_ڈیٹا, v_جڑ_Cap);
    }

    dc_ret = deriveCap(v_جڑ_خانہ, v_جڑ_Cap);
    اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
        واپس dc_ret.حالت_2;
    }
    v_جڑ_Cap = dc_ret.cap;

    اگر (!is_Valid_VTable_جڑ(v_جڑ_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* fault handler */
    اگر (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
#اگر_ختم

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    واپس invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خانہ,
               fhCap, fh_خانہ,
               cap_null_cap_new(), تہی,
               c_جڑ_Cap, c_جڑ_خانہ,
               v_جڑ_Cap, v_جڑ_خانہ,
               0, cap_null_cap_new(), تہی, thread_control_caps_update_space | thread_control_caps_update_fault);
#ورنہ
    واپس invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), خانہ,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_جڑ_Cap, c_جڑ_خانہ,
               v_جڑ_Cap, v_جڑ_خانہ,
               0, cap_null_cap_new(), تہی, thread_control_update_space);
#اگر_ختم
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    اگر (current_extra_caps.excaprefs[0] == تہی) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    اگر (tcb->tcbBoundNotification) {
        userError("TCB BindNotification: TCB already has a bound notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    ntfn_cap = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(ntfn_cap) == cap_notification_cap) {
        ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(ntfn_cap));
    } ورنہ {
        userError("TCB BindNotification: Notification is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (!cap_notification_cap_get_capNtfnCanReceive(ntfn_cap)) {
        userError("TCB BindNotification: Insufficient access rights");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر ((tcb_t *)notification_ptr_get_ntfn_قطار_سر(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }


    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeTCB_NotificationControl(tcb, ntfnPtr);
}

exception_t decodeUnbindNotification(cap_t cap)
{
    tcb_t *tcb;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    اگر (!tcb->tcbBoundNotification) {
        userError("TCB UnbindNotification: TCB already has no bound Notification.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeTCB_NotificationControl(tcb, تہی);
}

/* The following functions sit in the preemption monad and implement the
 * preemptible, non-faulting bottom end of a TCB invocation. */
exception_t invokeTCB_Suspend(tcb_t *thread)
{
    suspend(thread);
    واپس EXCEPTION_NONE;
}

exception_t invokeTCB_Resume(tcb_t *thread)
{
    restart(thread);
    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن اندرونی exception_t installTCBCap(tcb_t *ہدف, cap_t tCap, cte_t *خانہ,
                                        tcb_cnode_اشاریہ_t اشاریہ, cap_t newCap, cte_t *src_خانہ)
{
    cte_t *جڑ_خانہ = TCB_PTR_CTE_PTR(ہدف, اشاریہ);
    غیر_مستعمل exception_t e = cteDelete(جڑ_خانہ, true);
    اگر (e != EXCEPTION_NONE) {
        واپس e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    اگر (cap_get_capType(newCap) != cap_null_cap) {
        اگر (sameObjectAs(newCap, src_خانہ->cap) &&
            sameObjectAs(tCap, خانہ->cap)) {
            cte_داخل_کرو(newCap, src_خانہ, جڑ_خانہ);
        }
    }
    واپس e;
}
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *ہدف, cte_t *خانہ,
                                        cap_t fh_newCap, cte_t *fh_src_خانہ,
                                        cap_t th_newCap, cte_t *th_src_خانہ,
                                        cap_t c_جڑ_new_Cap, cte_t *c_جڑ_src_خانہ,
                                        cap_t v_جڑ_new_Cap, cte_t *v_جڑ_src_خانہ,
                                        لفظ_t بفر_Addr, cap_t بفر_Cap,
                                        cte_t *بفر_Src_خانہ,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((لفظ_t)ہدف);

    اگر (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(ہدف, tCap, خانہ, tcbFaultHandler, fh_newCap, fh_src_خانہ);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }

    }

    اگر (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(ہدف, tCap, خانہ, tcbTimeoutHandler, th_newCap, th_src_خانہ);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }
    }

    اگر (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(ہدف, tCap, خانہ, tcbCTable, c_جڑ_new_Cap, c_جڑ_src_خانہ);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }

        e = installTCBCap(ہدف, tCap, خانہ, tcbVTable, v_جڑ_new_Cap, v_جڑ_src_خانہ);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }
    }

    اگر (updateFlags & thread_control_caps_update_ipc_بفر) {
        cte_t *بفر_خانہ;

        بفر_خانہ = TCB_PTR_CTE_PTR(ہدف, tcb_بفر);
        e = cteDelete(بفر_خانہ, true);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }
        ہدف->tcbIPCBuffer = بفر_Addr;

        اگر (بفر_Src_خانہ && sameObjectAs(بفر_Cap, بفر_Src_خانہ->cap) &&
            sameObjectAs(tCap, خانہ->cap)) {
            cte_داخل_کرو(بفر_Cap, بفر_Src_خانہ, بفر_خانہ);
        }

        اگر (ہدف == گرہ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    واپس EXCEPTION_NONE;
}
#ورنہ
exception_t invokeTCB_ThreadControl(tcb_t *ہدف, cte_t *خانہ,
                                    cptr_t faultep, prio_t mcp, prio_t ترجیح,
                                    cap_t c_جڑ_new_Cap, cte_t *c_جڑ_src_خانہ,
                                    cap_t v_جڑ_new_Cap, cte_t *v_جڑ_src_خانہ,
                                    لفظ_t بفر_Addr, cap_t بفر_Cap,
                                    cte_t *بفر_Src_خانہ,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((لفظ_t)ہدف);

    اگر (updateFlags & thread_control_update_space) {
        ہدف->tcbFaultHandler = faultep;
    }

    اگر (updateFlags & thread_control_update_mcp) {
        سیٹ_MCPriority(ہدف, mcp);
    }

    اگر (updateFlags & thread_control_update_space) {
        cte_t *جڑ_خانہ;

        جڑ_خانہ = TCB_PTR_CTE_PTR(ہدف, tcbCTable);
        e = cteDelete(جڑ_خانہ, true);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }
        اگر (sameObjectAs(c_جڑ_new_Cap, c_جڑ_src_خانہ->cap) &&
            sameObjectAs(tCap, خانہ->cap)) {
            cte_داخل_کرو(c_جڑ_new_Cap, c_جڑ_src_خانہ, جڑ_خانہ);
        }
    }

    اگر (updateFlags & thread_control_update_space) {
        cte_t *جڑ_خانہ;

        جڑ_خانہ = TCB_PTR_CTE_PTR(ہدف, tcbVTable);
        e = cteDelete(جڑ_خانہ, true);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }
        اگر (sameObjectAs(v_جڑ_new_Cap, v_جڑ_src_خانہ->cap) &&
            sameObjectAs(tCap, خانہ->cap)) {
            cte_داخل_کرو(v_جڑ_new_Cap, v_جڑ_src_خانہ, جڑ_خانہ);
        }
    }

    اگر (updateFlags & thread_control_update_ipc_بفر) {
        cte_t *بفر_خانہ;

        بفر_خانہ = TCB_PTR_CTE_PTR(ہدف, tcb_بفر);
        e = cteDelete(بفر_خانہ, true);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }
        ہدف->tcbIPCBuffer = بفر_Addr;

        اگر (بفر_Src_خانہ && sameObjectAs(بفر_Cap, بفر_Src_خانہ->cap) &&
            sameObjectAs(tCap, خانہ->cap)) {
            cte_داخل_کرو(بفر_Cap, بفر_Src_خانہ, بفر_خانہ);
        }

        اگر (ہدف == گرہ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    اگر (updateFlags & thread_control_update_ترجیح) {
        سیٹ_ترجیح(ہدف, ترجیح);
    }

    واپس EXCEPTION_NONE;
}
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *ہدف, cte_t *خانہ,
                                         cap_t fh_newCap, cte_t *fh_src_خانہ,
                                         prio_t mcp, prio_t ترجیح,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    اگر (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((لفظ_t)ہدف);
        exception_t e = installTCBCap(ہدف, tCap, خانہ, tcbFaultHandler, fh_newCap, fh_src_خانہ);
        اگر (e != EXCEPTION_NONE) {
            واپس e;
        }
    }

    اگر (updateFlags & thread_control_sched_update_mcp) {
        سیٹ_MCPriority(ہدف, mcp);
    }

    اگر (updateFlags & thread_control_sched_update_ترجیح) {
        سیٹ_ترجیح(ہدف, ترجیح);
    }

    اگر (updateFlags & thread_control_sched_update_sc) {
        اگر (sc != تہی) {
            schedContext_bindTCB(sc, ہدف);
        } ورنہ اگر (sc == تہی) {
            schedContext_unbindTCB(ہدف->tcbSchedContext);
        }
    }

    واپس EXCEPTION_NONE;
}
#اگر_ختم

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspend_ماخذ, bool_t resume_ہدف,
                                    bool_t transfer_چوکھٹا, bool_t transferInteger,
                                    لفظ_t transferArch)
{
    اگر (suspend_ماخذ) {
        suspend(tcb_src);
    }

    اگر (resume_ہدف) {
        restart(dest);
    }

    اگر (transfer_چوکھٹا) {
        لفظ_t i;
        لفظ_t v;
        لفظ_t pc;

        برائے (i = 0; i < n_چوکھٹا_Registers; i++) {
            v = get_رجسٹر(tcb_src, چوکھٹا_Registers[i]);
            سیٹ_رجسٹر(dest, چوکھٹا_Registers[i], v);
        }

        pc = getRestartPC(dest);
        سیٹ_اگلا_PC(dest, pc);
    }

    اگر (transferInteger) {
        لفظ_t i;
        لفظ_t v;

        برائے (i = 0; i < n_gpRegisters; i++) {
            v = get_رجسٹر(tcb_src, gpRegisters[i]);
            سیٹ_رجسٹر(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    اگر (dest == گرہ_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    واپس Arch_performTransfer(transferArch, tcb_src, dest);
}

/* ReadRegisters is a special case: replyFromKernel & setMRs are
 * unfolded here, in order to avoid passing the large reply message up
 * to the top level in a global (and double-copying). We prevent the
 * top-level replyFromKernel_success_empty() from running by setting the
 * thread state. Retype does this too.
 */
exception_t invokeTCB_ReadRegisters(tcb_t *tcb_src, bool_t suspend_ماخذ,
                                    لفظ_t n, لفظ_t arch, bool_t call)
{
    لفظ_t i, j;
    exception_t e;
    tcb_t *thread;

    thread = گرہ_STATE(ksCurThread);

    اگر (suspend_ماخذ) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, گرہ_STATE(ksCurThread));
    اگر (e != EXCEPTION_NONE) {
        واپس e;
    }

    اگر (call) {
        لفظ_t *ipc_بفر;

        ipc_بفر = lookupIPCBuffer(true, thread);

        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);

        برائے (i = 0; i < n && i < n_چوکھٹا_Registers && i < n_پیغام_Registers; i++) {
            سیٹ_رجسٹر(thread, پیغام_Registers[i],
                        get_رجسٹر(tcb_src, چوکھٹا_Registers[i]));
        }

        اگر (ipc_بفر != تہی && i < n && i < n_چوکھٹا_Registers) {
            برائے (; i < n && i < n_چوکھٹا_Registers; i++) {
                ipc_بفر[i + 1] = get_رجسٹر(tcb_src, چوکھٹا_Registers[i]);
            }
        }

        j = i;

        برائے (i = 0; i < n_gpRegisters && i + n_چوکھٹا_Registers < n
             && i + n_چوکھٹا_Registers < n_پیغام_Registers; i++) {
            سیٹ_رجسٹر(thread, پیغام_Registers[i + n_چوکھٹا_Registers],
                        get_رجسٹر(tcb_src, gpRegisters[i]));
        }

        اگر (ipc_بفر != تہی && i < n_gpRegisters
            && i + n_چوکھٹا_Registers < n) {
            برائے (; i < n_gpRegisters && i + n_چوکھٹا_Registers < n; i++) {
                ipc_بفر[i + n_چوکھٹا_Registers + 1] =
                    get_رجسٹر(tcb_src, gpRegisters[i]);
            }
        }

        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, i + j)));
    }
    سیٹ_Thread_State(thread, ThreadState_Running);

    واپس EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resume_ہدف,
                                     لفظ_t n, لفظ_t arch, لفظ_t *بفر)
{
    لفظ_t i;
    لفظ_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, گرہ_STATE(ksCurThread), dest);
    اگر (e != EXCEPTION_NONE) {
        واپس e;
    }

    اگر (n > n_چوکھٹا_Registers + n_gpRegisters) {
        n = n_چوکھٹا_Registers + n_gpRegisters;
    }

    archInfo = Arch_get_Sanitise_رجسٹر_Info(dest);

    برائے (i = 0; i < n_چوکھٹا_Registers && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        سیٹ_رجسٹر(dest, چوکھٹا_Registers[i],
                    sanitise_رجسٹر(چوکھٹا_Registers[i],
                                     getSyscallArg(i + 2, بفر), archInfo));
    }

    برائے (i = 0; i < n_gpRegisters && i + n_چوکھٹا_Registers < n; i++) {
        سیٹ_رجسٹر(dest, gpRegisters[i],
                    sanitise_رجسٹر(gpRegisters[i],
                                     getSyscallArg(i + n_چوکھٹا_Registers + 2,
                                                   بفر), archInfo));
    }

    pc = getRestartPC(dest);
    سیٹ_اگلا_PC(dest, pc);

    Arch_postModifyRegisters(dest);

    اگر (resume_ہدف) {
        restart(dest);
    }

    اگر (dest == گرہ_STATE(ksCurThread)) {
        /* If we modified the current thread we may need to reschedule
         * due to changing registers are only reloaded in Arch_switchToThread */
        rescheduleRequired();
    }

    واپس EXCEPTION_NONE;
}

exception_t invokeTCB_NotificationControl(tcb_t *tcb, notification_t *ntfnPtr)
{
    اگر (ntfnPtr) {
        bindNotification(tcb, ntfnPtr);
    } ورنہ {
        unbindNotification(tcb);
    }

    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
خالی سیٹ_Thread_Name(tcb_t *tcb, ثابت حرف *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_لمبائی);
}
#اگر_ختم

لفظ_t سیٹ_MRs_syscall_error(tcb_t *thread, لفظ_t *receiveIPCBuffer)
{
    انتخاب (current_syscall_error.type) {
    حالت se_L4_Invalid_دلیل:
        واپس سیٹ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_دلیل_نمبر);

    حالت seL4_InvalidCapability:
        واپس سیٹ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_نمبر);

    حالت seL4_IllegalOperation:
        واپس 0;

    حالت seL4_RangeError:
        سیٹ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        واپس سیٹ_MR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    حالت seL4_AlignmentError:
        واپس 0;

    حالت seL4_FailedLookup:
        سیٹ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failed_Lookup_Was_ماخذ ? 1 : 0);
        واپس سیٹ_MRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    حالت se_L4_Truncated_پیغام:
    حالت se_L4_Delete_پہلا:
    حالت se_L4_Revoke_پہلا:
        واپس 0;
    حالت se_L4_Not_Enough_حافظہ:
        واپس سیٹ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.حافظہ_بایاں);
    طےشدہ:
        fail("Invalid syscall error");
    }
}
