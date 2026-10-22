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
#اگر_تعریف_شده CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#پایان_اگر
#ضمن <object/tcb.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <model/statedata.h>
#ضمن <util.h>
#ضمن <string.h>
#ضمن <stdint.h>
#ضمن <arch/smp/ipi_inline.h>

#تعریف_کن NULL_PRIO 0

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

ساكن مضمن فراغ جمع_کن_مقصد_Bitmap(کلمه_t cpu, کلمه_t dom, کلمه_t prio)
{
    کلمه_t l1index;
    کلمه_t l1index_inverted;

    l1index = prio_مقصد_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    گره_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= بیت(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    گره_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= بیت(prio & ماسک(کلمه_Radix));
}

ساكن مضمن فراغ removeFromBitmap(کلمه_t cpu, کلمه_t dom, کلمه_t prio)
{
    کلمه_t l1index;
    کلمه_t l1index_inverted;

    l1index = prio_مقصد_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    گره_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~بیت(prio & ماسک(کلمه_Radix));
    إذا (unlikely(!گره_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        گره_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~بیت(l1index);
    }
}

tcb_صف_t tcb_صف_remove(tcb_صف_t صف, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcb_Sched_بعدی;

    إذا (صف.سر == tcb && صف.انتها == tcb) {
        صف.سر = عدم;
        صف.انتها = عدم;
    } وإلا {
        إذا (صف.سر == tcb) {
            after->tcbSchedPrev = عدم;
            tcb->tcb_Sched_بعدی = عدم;
            صف.سر = after;
        } وإلا {
            إذا (صف.انتها == tcb) {
                before->tcb_Sched_بعدی = عدم;
                tcb->tcbSchedPrev = عدم;
                صف.انتها = before;
            } وإلا {
                before->tcb_Sched_بعدی = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = عدم;
                tcb->tcb_Sched_بعدی = عدم;
            }
        }
    }

    أرجع صف;
}

/* Add TCB to the head of a scheduler queue */
فراغ tcb_Sched_افزودن_به_صف(tcb_t *tcb)
{
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
#پایان_اگر

    إذا (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_صف_t صف;
        dom_t dom;
        prio_t prio;
        کلمه_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_اولویت;
        idx = ready_queues_شاخص(dom, prio);
        صف = گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        إذا (tcb_صف_خالی(صف)) {
            جمع_کن_مقصد_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_صف_prepend(صف, tcb);

        thread_state_ptr_مجموعه_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
فراغ tcbSchedAppend(tcb_t *tcb)
{
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
    أكد(refill_ready(tcb->tcbSchedContext));
#پایان_اگر
    إذا (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_صف_t صف;
        dom_t dom;
        prio_t prio;
        کلمه_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_اولویت;
        idx = ready_queues_شاخص(dom, prio);
        صف = گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        إذا (tcb_صف_خالی(صف)) {
            جمع_کن_مقصد_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_صف_append(صف, tcb);

        thread_state_ptr_مجموعه_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
فراغ tcb_Sched_خارج_از_صف(tcb_t *tcb)
{
    إذا (thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_صف_t صف;
        tcb_صف_t new_صف;
        dom_t dom;
        prio_t prio;
        کلمه_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_اولویت;
        idx = ready_queues_شاخص(dom, prio);
        صف = گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        new_صف = tcb_صف_remove(صف, tcb);

        گره_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = new_صف;

        thread_state_ptr_مجموعه_tcb_Queued(&tcb->tcbState, false);

        إذا (likely(tcb_صف_خالی(new_صف))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
فراغ tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = عدم;

    debug_tcb->tcb_Debug_بعدی = گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    إذا (گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

فراغ tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    أكد(گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != عدم);
    إذا (tcb == گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        گره_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(گره_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcb_Debug_بعدی;
    } وإلا {
        أكد(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcb_Debug_بعدی = debug_tcb->tcb_Debug_بعدی;
    }

    إذا (debug_tcb->tcb_Debug_بعدی) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_بعدی)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = عدم;
    debug_tcb->tcb_Debug_بعدی = عدم;
}
#پایان_اگر /* CONFIG_DEBUG_BUILD */

#اگر_تعریف_نشده CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_صف_t tcbEPAppend(tcb_t *tcb, tcb_صف_t صف)
{
    إذا (!صف.سر) { /* Empty list */
        صف.سر = tcb;
    } وإلا {
        صف.انتها->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = صف.انتها;
    tcb->tcbEPNext = عدم;
    صف.انتها = tcb;

    أرجع صف;
}

/* Remove TCB from an endpoint queue */
tcb_صف_t tcbEPDequeue(tcb_t *tcb, tcb_صف_t صف)
{
    إذا (tcb->tcbEPPrev) {
        tcb->tcbEPPrev->tcbEPNext = tcb->tcbEPNext;
    } وإلا {
        صف.سر = tcb->tcbEPNext;
    }

    إذا (tcb->tcbEPNext) {
        tcb->tcbEPNext->tcbEPPrev = tcb->tcbEPPrev;
    } وإلا {
        صف.انتها = tcb->tcbEPPrev;
    }

    أرجع صف;
}
#پایان_اگر /* CONFIG_KERNEL_MCS */

#اگر_تعریف_شده CONFIG_KERNEL_MCS

فراغ tcbReleaseRemove(tcb_t *tcb)
{
    إذا (likely(thread_state_get_tcb_درون_Release_صف(tcb->tcbState))) {
        tcb_صف_t صف = گره_STATE_ON_CORE(ks_Release_صف, tcb->tcbAffinity);

        إذا (صف.سر == tcb) {
            گره_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        گره_STATE_ON_CORE(ks_Release_صف, tcb->tcbAffinity) = tcb_صف_remove(صف, tcb);

        thread_state_ptr_مجموعه_tcb_درون_Release_صف(&tcb->tcbState, false);
    }
}

ساكن مضمن ticks_t خالص tcb_Ready_زمان(tcb_t *tcb)
{
    أرجع refill_سر(tcb->tcbSchedContext)->r_زمان;
}

ساكن مضمن bool_t خالص زمان_after(tcb_t *tcb, ticks_t new_زمان)
{
    أرجع tcb != عدم && new_زمان >= tcb_Ready_زمان(tcb);
}

ساكن tcb_t *پیدا_کن_زمان_after(tcb_t *tcb, ticks_t new_زمان)
{
    tcb_t *after = tcb;

    طالما (زمان_after(after, new_زمان)) {
        after = after->tcb_Sched_بعدی;
    }

    أرجع after;
}

فراغ tcb_Release_افزودن_به_صف(tcb_t *tcb)
{
    أكد(thread_state_get_tcb_درون_Release_صف(tcb->tcbState) == false);
    أكد(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_زمان;
    tcb_صف_t صف;

    صف = گره_STATE_ON_CORE(ks_Release_صف, tcb->tcbAffinity);
    new_زمان = tcb_Ready_زمان(tcb);

    إذا (tcb_صف_خالی(صف) || new_زمان < tcb_Ready_زمان(صف.سر)) {
        گره_STATE_ON_CORE(ks_Release_صف, tcb->tcbAffinity) = tcb_صف_prepend(صف, tcb);
    } وإلا {
        إذا (tcb_Ready_زمان(صف.انتها) <= new_زمان) {
            گره_STATE_ON_CORE(ks_Release_صف, tcb->tcbAffinity) = tcb_صف_append(صف, tcb);
        } وإلا {
            tcb_t *after;
            after = پیدا_کن_زمان_after(صف.سر, new_زمان);
            tcb_صف_درج(tcb, after);
        }
    }

    thread_state_ptr_مجموعه_tcb_درون_Release_صف(&tcb->tcbState, true);

    إذا (صف.سر != گره_STATE_ON_CORE(ks_Release_صف, tcb->tcbAffinity).سر) {
        گره_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#پایان_اگر

cptr_t خالص getExtraCPtr(کلمه_t *میانگیر_Ptr, کلمه_t i)
{
    أرجع (cptr_t)میانگیر_Ptr[se_L4_پیام_Max_درازی + 2 + i];
}

فراغ مجموعه_Extra_Badge(کلمه_t *میانگیر_Ptr, کلمه_t badge,
                   کلمه_t i)
{
    میانگیر_Ptr[se_L4_پیام_Max_درازی + 2 + i] = badge;
}

#اگر_تعریف_نشده CONFIG_KERNEL_MCS
فراغ راهاندازی_Caller_Cap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *پاسخ_جایگاه, *caller_جایگاه;
    cap_t masterCap استفادهنشده, callerCap استفادهنشده;

    مجموعه_Thread_State(sender, Thread_State_Blocked_On_پاسخ);
    پاسخ_جایگاه = TCB_PTR_CTE_PTR(sender, tcb_پاسخ);
    masterCap = پاسخ_جایگاه->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    أكد(cap_get_capType(masterCap) == cap_پاسخ_cap);
    أكد(cap_پاسخ_cap_get_cap_پاسخ_Master(masterCap));
    أكد(cap_پاسخ_cap_get_cap_پاسخ_Can_Grant(masterCap));
    أكد(TCB_PTR(cap_پاسخ_cap_get_cap_TCBPtr(masterCap)) == sender);
    caller_جایگاه = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = caller_جایگاه->cap;
    /* Haskell error: "Caller cap must not already exist" */
    أكد(cap_get_capType(callerCap) == cap_null_cap);
    cte_درج(cap_پاسخ_cap_new(canGrant, false, TCB_REF(sender)),
              پاسخ_جایگاه, caller_جایگاه);
}

فراغ deleteCallerCap(tcb_t *receiver)
{
    cte_t *caller_جایگاه;

    caller_جایگاه = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(caller_جایگاه);
}
#پایان_اگر

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, کلمه_t *میانگیر_Ptr, se_L4_پیام_Info_t info)
{
    lookup_جایگاه_raw_ret_t lu_ret;
    cptr_t cptr;
    کلمه_t i, درازی;

    إذا (!میانگیر_Ptr) {
        current_extra_caps.excaprefs[0] = عدم;
        أرجع EXCEPTION_NONE;
    }

    درازی = se_L4_پیام_Info_get_extra_Caps(info);

    لكل (i = 0; i < درازی; i++) {
        cptr = getExtraCPtr(میانگیر_Ptr, i);

        lu_ret = lookup_جایگاه(thread, cptr);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            أرجع lu_ret.وضعیت;
        }

        current_extra_caps.excaprefs[i] = lu_ret.جایگاه;
    }
    إذا (i < se_L4_پیام_Max_Extra_Caps) {
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
    لكل (i = 0; i < n && i < n_پیام_Registers; i++) {
        مجموعه_ثبات(receiver, پیام_Registers[i],
                    get_ثبات(sender, پیام_Registers[i]));
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

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
فراغ remote_صف_Update(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    إذا (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *هدف_Cur_Thread = گره_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        إذا (هدف_Cur_Thread == گره_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcb_اولویت > هدف_Cur_Thread->tcb_اولویت
#اگر_تعریف_شده CONFIG_KERNEL_MCS
            || گره_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#پایان_اگر
           ) {
            ARCH_گره_STATE(ipiReschedulePending) |= بیت(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
فراغ remoteTCBStall(tcb_t *tcb)
{

    إذا (
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#پایان_اگر
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        گره_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_گره_STATE(ipiReschedulePending) |= بیت(tcb->tcbAffinity);
    }
}

#اگر_تعریف_نشده CONFIG_KERNEL_MCS
ساكن exception_t invoke_TCB_مجموعه_Affinity(tcb_t *thread, کلمه_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcb_Sched_خارج_از_صف(thread);
    migrateTCB(thread, affinity);
    إذا (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    إذا (thread == گره_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_مجموعه_Affinity(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    tcb_t *tcb;
    کلمه_t affinity;

    إذا (درازی < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, میانگیر);
    إذا (affinity >= ks_رقم_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_TCB_مجموعه_Affinity(tcb, affinity);
}
#پایان_اگر
#پایان_اگر /* ENABLE_SMP_SUPPORT */

#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
ساكن exception_t invokeConfigureSingleStepping(bool_t call, کلمه_t *میانگیر, tcb_t *t,
                                                 uint16_t bp_رقم, کلمه_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    کلمه_t مقدار;

    bp_was_consumed = configureSingleStepping(t, bp_رقم, n_instrs, false);
    إذا (n_instrs == 0) {
#اگر_تعریف_نشده CONFIG_ARCH_AARCH64
        unset_Breakpoint_استفاده_شده_Flag(t, bp_رقم);
#پایان_اگر /* CONFIG_ARCH_AARCH64 */
        مقدار = false;
    } وإلا {
#اگر_تعریف_نشده CONFIG_ARCH_AARCH64
        مجموعه_Breakpoint_استفاده_شده_Flag(t, bp_رقم);
#پایان_اگر /* CONFIG_ARCH_AARCH64 */
        مقدار = bp_was_consumed;
    }

    إذا (call) {
        مجموعه_ثبات(thread, badge_ثبات, 0);
        بیعلامت صحيح درازی = مجموعه_MR(thread, میانگیر, 0, مقدار);
        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, درازی)));
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    uint16_t bp_رقم;
    کلمه_t n_instrs;
    tcb_t *tcb;
    syscall_error_t syserr;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    bp_رقم = getSyscallArg(0, میانگیر);
    n_instrs = getSyscallArg(1, میانگیر);

    syserr = Arch_decodeConfigureSingleStepping(tcb, bp_رقم, n_instrs, false);
    إذا (syserr.type != seL4_NoError) {
        current_syscall_error = syserr;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeConfigureSingleStepping(call, میانگیر, tcb, bp_رقم, n_instrs);
}

ساكن exception_t invoke_مجموعه_Breakpoint(tcb_t *tcb, uint16_t bp_رقم,
                                       کلمه_t vaddr, کلمه_t type, کلمه_t اندازه, کلمه_t rw)
{
    مجموعه_Breakpoint(tcb, bp_رقم, vaddr, type, اندازه, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    مجموعه_Breakpoint_استفاده_شده_Flag(tcb, bp_رقم);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_مجموعه_Breakpoint(cap_t cap, کلمه_t *میانگیر)
{
    uint16_t bp_رقم;
    کلمه_t vaddr, type, اندازه, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_رقم = getSyscallArg(0, میانگیر);
    vaddr = getSyscallArg(1, میانگیر);
    type = getSyscallArg(2, میانگیر);
    اندازه = getSyscallArg(3, میانگیر);
    rw = getSyscallArg(4, میانگیر);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    إذا (vaddr >= (کلمه_t)USER_بالاترین) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (type != seL4_InstructionBreakpoint && type != se_L4_داده_Breakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    } وإلا إذا (type == seL4_InstructionBreakpoint) {
        إذا (اندازه != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 3;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 4;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا ((se_L4_اولی_Watchpoint == -1 || bp_رقم >= se_L4_اولی_Watchpoint)
            && se_L4_اولی_Breakpoint != se_L4_اولی_Watchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 2;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    } وإلا إذا (type == se_L4_داده_Breakpoint) {
        إذا (اندازه == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 3;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (se_L4_اولی_Watchpoint != -1 && bp_رقم < se_L4_اولی_Watchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 2;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    } وإلا إذا (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (اندازه != 0 && اندازه != 1 && اندازه != 2 && اندازه != 4 && اندازه != 8) {
        userError("Debug: Invalid size %lu.", اندازه);
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 3;
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

    error = Arch_decode_مجموعه_Breakpoint(tcb, bp_رقم, vaddr, type, اندازه, rw);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_مجموعه_Breakpoint(tcb, bp_رقم,
                               vaddr, type, اندازه, rw);
}

ساكن exception_t invokeGetBreakpoint(bool_t call, کلمه_t *میانگیر, tcb_t *tcb, uint16_t bp_رقم)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_رقم);
    إذا (call) {
        مجموعه_ثبات(thread, badge_ثبات, 0);
        مجموعه_MR(گره_STATE(ksCurThread), میانگیر, 0, res.vaddr);
        مجموعه_MR(گره_STATE(ksCurThread), میانگیر, 1, res.type);
        مجموعه_MR(گره_STATE(ksCurThread), میانگیر, 2, res.اندازه);
        مجموعه_MR(گره_STATE(ksCurThread), میانگیر, 3, res.rw);
        مجموعه_MR(گره_STATE(ksCurThread), میانگیر, 4, res.is_enabled);
        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, 5)));
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeGetBreakpoint(cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    tcb_t *tcb;
    uint16_t bp_رقم;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_رقم = getSyscallArg(0, میانگیر);

    error = Arch_decodeGetBreakpoint(tcb, bp_رقم);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeGetBreakpoint(call, میانگیر, tcb, bp_رقم);
}

ساكن exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_رقم)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_رقم);
    unset_Breakpoint_استفاده_شده_Flag(tcb, bp_رقم);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeUnsetBreakpoint(cap_t cap, کلمه_t *میانگیر)
{
    tcb_t *tcb;
    uint16_t bp_رقم;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_رقم = getSyscallArg(0, میانگیر);

    error = Arch_decodeUnsetBreakpoint(tcb, bp_رقم);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeUnsetBreakpoint(tcb, bp_رقم);
}
#پایان_اگر /* CONFIG_HARDWARE_DEBUG_API */

ساكن exception_t invoke_مجموعه_TLSBase(tcb_t *thread, کلمه_t tls_پایه)
{
    مجموعه_ثبات(thread, TLS_پایه, tls_پایه);
    إذا (thread == گره_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_مجموعه_TLSBase(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    کلمه_t tls_پایه;

    إذا (درازی < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tls_پایه = getSyscallArg(0, میانگیر);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_مجموعه_TLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_پایه);
}

ساكن فراغ invoke_مجموعه_Flags(tcb_t *thread, کلمه_t clear, کلمه_t مجموعه, bool_t call)
{
    tcb_t *cur_thread = گره_STATE(ksCurThread);
    کلمه_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= مجموعه & se_L4_TCBFlag_ماسک;
    thread->tcbFlags = flags;

#اگر_تعریف_شده CONFIG_HAVE_FPU
    إذا (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } وإلا إذا (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#پایان_اگر
    إذا (call) {
        کلمه_t *ipc_میانگیر = lookupIPCBuffer(true, cur_thread);
        مجموعه_ثبات(cur_thread, badge_ثبات, 0);
        بیعلامت صحيح درازی = مجموعه_MR(cur_thread, ipc_میانگیر, 0, flags);
        مجموعه_ثبات(cur_thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, درازی)));
    }
    مجموعه_Thread_State(cur_thread, ThreadState_Running);
}

ساكن exception_t decode_مجموعه_Flags(cap_t cap, کلمه_t درازی, bool_t call, کلمه_t *میانگیر)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    إذا (درازی < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کلمه_t clear = getSyscallArg(0, میانگیر);
    کلمه_t مجموعه   = getSyscallArg(1, میانگیر);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    invoke_مجموعه_Flags(thread, clear, مجموعه, call);
    أرجع EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(کلمه_t invLabel, کلمه_t درازی, cap_t cap,
                                cte_t *جایگاه, bool_t call, کلمه_t *میانگیر)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    بدل (invLabel) {
    حالة TCBReadRegisters:
        /* Second level of decoding */
        أرجع decodeReadRegisters(cap, درازی, call, میانگیر);

    حالة TCBWriteRegisters:
        أرجع decodeWriteRegisters(cap, درازی, میانگیر);

    حالة TCBCopyRegisters:
        أرجع decodeCopyRegisters(cap, درازی, میانگیر);

    حالة TCBSuspend:
        /* Jump straight to the invoke */
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالة TCBResume:
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالة TCBConfigure:
        أرجع decodeTCBConfigure(cap, درازی, جایگاه, میانگیر);

    حالة TCBSet_اولویت:
        أرجع decode_مجموعه_اولویت(cap, درازی, میانگیر);

    حالة TCBSetMCPriority:
        أرجع decode_مجموعه_MCPriority(cap, درازی, میانگیر);

    حالة TCBSetSchedParams:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        أرجع decode_مجموعه_Sched_Params(cap, درازی, جایگاه, میانگیر);
#وإلا
        أرجع decode_مجموعه_Sched_Params(cap, درازی, میانگیر);
#پایان_اگر

    حالة TCBSetIPCBuffer:
        أرجع decode_مجموعه_IPCBuffer(cap, درازی, جایگاه, میانگیر);

    حالة TCBSetSpace:
        أرجع decode_مجموعه_Space(cap, درازی, جایگاه, میانگیر);

    حالة TCBBindNotification:
        أرجع decodeBindNotification(cap);

    حالة TCBUnbindNotification:
        أرجع decodeUnbindNotification(cap);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة TCBSetTimeoutEndpoint:
        أرجع decode_مجموعه_Timeout_Endpoint(cap, جایگاه);
#وإلا
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    حالة TCBSetAffinity:
        أرجع decode_مجموعه_Affinity(cap, درازی, میانگیر);
#پایان_اگر /* ENABLE_SMP_SUPPORT */
#پایان_اگر

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#اگر_تعریف_شده CONFIG_VTX
    حالة TCBSetEPTRoot:
        أرجع decode_مجموعه_EPTRoot(cap);
#پایان_اگر

#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
    حالة TCBConfigureSingleStepping:
        أرجع decodeConfigureSingleStepping(cap, call, میانگیر);

    حالة TCBSetBreakpoint:
        أرجع decode_مجموعه_Breakpoint(cap, میانگیر);

    حالة TCBGetBreakpoint:
        أرجع decodeGetBreakpoint(cap, call, میانگیر);

    حالة TCBUnsetBreakpoint:
        أرجع decodeUnsetBreakpoint(cap, میانگیر);
#پایان_اگر

    حالة TCBSetTLSBase:
        أرجع decode_مجموعه_TLSBase(cap, درازی, میانگیر);

    حالة TCBSetFlags:
        أرجع decode_مجموعه_Flags(cap, درازی, call, میانگیر);

    افتراضي:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

تعداد CopyRegistersFlags {
    Copy_Registers_suspend_منبع = 0,
    Copy_Registers_resume_هدف = 1,
    Copy_Registers_transfer_قاب = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    کلمه_t transferArch;
    tcb_t *srcTCB;
    cap_t منبع_cap;
    کلمه_t flags;

    إذا (درازی < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, میانگیر);

    transferArch = Arch_decodeTransfer(flags >> 8);

    منبع_cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(منبع_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(منبع_cap));
    } وإلا {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & بیت(Copy_Registers_suspend_منبع),
               flags & بیت(Copy_Registers_resume_هدف),
               flags & بیت(Copy_Registers_transfer_قاب),
               flags & بیت(CopyRegisters_transferInteger),
               transferArch);

}

تعداد ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, کلمه_t درازی, bool_t call,
                                کلمه_t *میانگیر)
{
    کلمه_t transferArch, flags, n;
    tcb_t *thread;

    إذا (درازی < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, میانگیر);
    n     = getSyscallArg(1, میانگیر);

    إذا (n < 1 || n > n_قاب_Registers + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (صحيح)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_قاب_Registers +
                                              n_gpRegisters;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    إذا (thread == گره_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & بیت(ReadRegisters_suspend),
               n, transferArch, call);
}

تعداد WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    کلمه_t flags, w;
    کلمه_t transferArch;
    tcb_t *thread;

    إذا (درازی < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, میانگیر);
    w     = getSyscallArg(1, میانگیر);

    إذا (درازی - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (صحيح)(درازی - 2), (صحيح)w);
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    إذا (thread == گره_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_WriteRegisters(thread,
                                    flags & بیت(WriteRegisters_resume),
                                    w, transferArch, میانگیر);
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
bool_t validFaultHandler(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap:
        أرجع (cap_endpoint_cap_get_capCanSend(cap) &&
                (cap_endpoint_cap_get_capCanGrant(cap) ||
                 cap_endpoint_cap_get_cap_Can_Grant_پاسخ(cap)));
    حالة cap_null_cap:
        /* just has no fault endpoint */
        أرجع true;
    افتراضي:
        أرجع false;
    }
}
#پایان_اگر

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, کلمه_t درازی, cte_t *جایگاه, کلمه_t *میانگیر)
{
    cte_t *میانگیر_جایگاه, *c_ریشه_جایگاه, *v_ریشه_جایگاه;
    cap_t میانگیر_Cap, c_ریشه_Cap, v_ریشه_Cap;
    deriveCap_ret_t dc_ret;
    کلمه_t c_ریشه_داده, v_ریشه_داده, میانگیر_Addr;
#اگر_تعریف_شده CONFIG_KERNEL_MCS
#تعریف_کن TCBCONFIGURE_آرگومانها 3
#وإلا
#تعریف_کن TCBCONFIGURE_آرگومانها 4
#پایان_اگر
    إذا (درازی < TCBCONFIGURE_آرگومانها || current_extra_caps.excaprefs[0] == عدم
        || current_extra_caps.excaprefs[1] == عدم
        || current_extra_caps.excaprefs[2] == عدم) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    c_ریشه_داده     = getSyscallArg(0, میانگیر);
    v_ریشه_داده     = getSyscallArg(1, میانگیر);
    میانگیر_Addr    = getSyscallArg(2, میانگیر);
#وإلا
    cptr_t faultEP       = getSyscallArg(0, میانگیر);
    c_ریشه_داده     = getSyscallArg(1, میانگیر);
    v_ریشه_داده     = getSyscallArg(2, میانگیر);
    میانگیر_Addr    = getSyscallArg(3, میانگیر);
#پایان_اگر

    c_ریشه_جایگاه  = current_extra_caps.excaprefs[0];
    c_ریشه_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ریشه_جایگاه  = current_extra_caps.excaprefs[1];
    v_ریشه_Cap   = current_extra_caps.excaprefs[1]->cap;
    میانگیر_جایگاه = current_extra_caps.excaprefs[2];
    میانگیر_Cap  = current_extra_caps.excaprefs[2]->cap;

    إذا (میانگیر_Addr == 0) {
        میانگیر_جایگاه = عدم;
    } وإلا {
        dc_ret = deriveCap(میانگیر_جایگاه, میانگیر_Cap);
        إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
            أرجع dc_ret.وضعیت;
        }
        میانگیر_Cap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(میانگیر_Addr, میانگیر_Cap);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (جایگاه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        جایگاه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (c_ریشه_داده != 0) {
        c_ریشه_Cap = update_Cap_داده(false, c_ریشه_داده, c_ریشه_Cap);
    }

    dc_ret = deriveCap(c_ریشه_جایگاه, c_ریشه_Cap);
    إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
        أرجع dc_ret.وضعیت;
    }
    c_ریشه_Cap = dc_ret.cap;

    إذا (cap_get_capType(c_ریشه_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (v_ریشه_داده != 0) {
        v_ریشه_Cap = update_Cap_داده(false, v_ریشه_داده, v_ریشه_Cap);
    }

    dc_ret = deriveCap(v_ریشه_جایگاه, v_ریشه_Cap);
    إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
        أرجع dc_ret.وضعیت;
    }
    v_ریشه_Cap = dc_ret.cap;

    إذا (!is_Valid_VTable_ریشه(v_ریشه_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جایگاه,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               c_ریشه_Cap, c_ریشه_جایگاه,
               v_ریشه_Cap, v_ریشه_جایگاه,
               میانگیر_Addr, میانگیر_Cap,
               میانگیر_جایگاه, thread_control_caps_update_space |
               thread_control_caps_update_ipc_میانگیر);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جایگاه,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_ریشه_Cap, c_ریشه_جایگاه,
               v_ریشه_Cap, v_ریشه_جایگاه,
               میانگیر_Addr, میانگیر_Cap,
               میانگیر_جایگاه, thread_control_update_space |
               thread_control_update_ipc_میانگیر);
#پایان_اگر
}

exception_t decode_مجموعه_اولویت(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    إذا (درازی < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, میانگیر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t وضعیت = checkPrio(newPrio, authTCB);
    إذا (وضعیت != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (بیعلامت دراز) newPrio, (بیعلامت دراز) authTCB->tcbMCP);
        أرجع وضعیت;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               cap_null_cap_new(), عدم,
               NULL_PRIO, newPrio,
               عدم, thread_control_sched_update_اولویت);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(),
               عدم, thread_control_update_اولویت);
#پایان_اگر
}

exception_t decode_مجموعه_MCPriority(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    إذا (درازی < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, میانگیر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t وضعیت = checkPrio(newMcp, authTCB);
    إذا (وضعیت != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (بیعلامت دراز) newMcp, (بیعلامت دراز) authTCB->tcbMCP);
        أرجع وضعیت;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
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
#پایان_اگر
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
exception_t decode_مجموعه_Timeout_Endpoint(cap_t cap, cte_t *جایگاه)
{
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *th_جایگاه = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    إذا (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جایگاه,
               cap_null_cap_new(), عدم,
               thCap, th_جایگاه,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(), عدم,
               thread_control_caps_update_timeout);
}
#پایان_اگر

#اگر_تعریف_شده CONFIG_KERNEL_MCS
exception_t decode_مجموعه_Sched_Params(cap_t cap, کلمه_t درازی, cte_t *جایگاه, کلمه_t *میانگیر)
#وإلا
exception_t decode_مجموعه_Sched_Params(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
#پایان_اگر
{
    إذا (درازی < 2 || current_extra_caps.excaprefs[0] == عدم
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == عدم || current_extra_caps.excaprefs[2] == عدم
#پایان_اگر
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, میانگیر);
    prio_t newPrio = getSyscallArg(1, میانگیر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fh_جایگاه = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#پایان_اگر

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t وضعیت = checkPrio(newMcp, authTCB);
    إذا (وضعیت != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (بیعلامت دراز) newMcp, (بیعلامت دراز) authTCB->tcbMCP);
        أرجع وضعیت;
    }

    وضعیت = checkPrio(newPrio, authTCB);
    إذا (وضعیت != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (بیعلامت دراز) newPrio, (بیعلامت دراز) authTCB->tcbMCP);
        أرجع وضعیت;
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = عدم;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_اولویت |
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
        إذا (tcb == گره_STATE(ksCurThread)) {
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
#پایان_اگر
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جایگاه,
               fhCap, fh_جایگاه,
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
               thread_control_update_اولویت);
#پایان_اگر
}


exception_t decode_مجموعه_IPCBuffer(cap_t cap, کلمه_t درازی, cte_t *جایگاه, کلمه_t *میانگیر)
{
    cptr_t cptr_میانگیر_Ptr;
    cap_t میانگیر_Cap;
    cte_t *میانگیر_جایگاه;

    إذا (درازی < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cptr_میانگیر_Ptr  = getSyscallArg(0, میانگیر);
    میانگیر_جایگاه = current_extra_caps.excaprefs[0];
    میانگیر_Cap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cptr_میانگیر_Ptr == 0) {
        میانگیر_جایگاه = عدم;
    } وإلا {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(میانگیر_جایگاه, میانگیر_Cap);
        إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
            أرجع dc_ret.وضعیت;
        }
        میانگیر_Cap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_میانگیر_Ptr, میانگیر_Cap);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جایگاه,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cptr_میانگیر_Ptr, میانگیر_Cap,
               میانگیر_جایگاه, thread_control_caps_update_ipc_میانگیر);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جایگاه,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cptr_میانگیر_Ptr, میانگیر_Cap,
               میانگیر_جایگاه, thread_control_update_ipc_میانگیر);

#پایان_اگر
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
#تعریف_کن DECODE_مجموعه_SPACE_PARAMS 2
#وإلا
#تعریف_کن DECODE_مجموعه_SPACE_PARAMS 3
#پایان_اگر
exception_t decode_مجموعه_Space(cap_t cap, کلمه_t درازی, cte_t *جایگاه, کلمه_t *میانگیر)
{
    کلمه_t c_ریشه_داده, v_ریشه_داده;
    cte_t *c_ریشه_جایگاه, *v_ریشه_جایگاه;
    cap_t c_ریشه_Cap, v_ریشه_Cap;
    deriveCap_ret_t dc_ret;

    إذا (درازی < DECODE_مجموعه_SPACE_PARAMS || current_extra_caps.excaprefs[0] == عدم
        || current_extra_caps.excaprefs[1] == عدم
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == عدم
#پایان_اگر
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    c_ریشه_داده = getSyscallArg(0, میانگیر);
    v_ریشه_داده = getSyscallArg(1, میانگیر);

    cte_t *fh_جایگاه     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_ریشه_جایگاه  = current_extra_caps.excaprefs[1];
    c_ریشه_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_ریشه_جایگاه  = current_extra_caps.excaprefs[2];
    v_ریشه_Cap   = current_extra_caps.excaprefs[2]->cap;
#وإلا
    cptr_t faultEP   = getSyscallArg(0, میانگیر);
    c_ریشه_داده = getSyscallArg(1, میانگیر);
    v_ریشه_داده = getSyscallArg(2, میانگیر);

    c_ریشه_جایگاه  = current_extra_caps.excaprefs[0];
    c_ریشه_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_ریشه_جایگاه  = current_extra_caps.excaprefs[1];
    v_ریشه_Cap   = current_extra_caps.excaprefs[1]->cap;
#پایان_اگر

    إذا (جایگاه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        جایگاه_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (c_ریشه_داده != 0) {
        c_ریشه_Cap = update_Cap_داده(false, c_ریشه_داده, c_ریشه_Cap);
    }

    dc_ret = deriveCap(c_ریشه_جایگاه, c_ریشه_Cap);
    إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
        أرجع dc_ret.وضعیت;
    }
    c_ریشه_Cap = dc_ret.cap;

    إذا (cap_get_capType(c_ریشه_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (v_ریشه_داده != 0) {
        v_ریشه_Cap = update_Cap_داده(false, v_ریشه_داده, v_ریشه_Cap);
    }

    dc_ret = deriveCap(v_ریشه_جایگاه, v_ریشه_Cap);
    إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
        أرجع dc_ret.وضعیت;
    }
    v_ریشه_Cap = dc_ret.cap;

    إذا (!is_Valid_VTable_ریشه(v_ریشه_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    /* fault handler */
    إذا (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#پایان_اگر

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جایگاه,
               fhCap, fh_جایگاه,
               cap_null_cap_new(), عدم,
               c_ریشه_Cap, c_ریشه_جایگاه,
               v_ریشه_Cap, v_ریشه_جایگاه,
               0, cap_null_cap_new(), عدم, thread_control_caps_update_space | thread_control_caps_update_fault);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), جایگاه,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_ریشه_Cap, c_ریشه_جایگاه,
               v_ریشه_Cap, v_ریشه_جایگاه,
               0, cap_null_cap_new(), عدم, thread_control_update_space);
#پایان_اگر
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
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

    إذا ((tcb_t *)notification_ptr_get_ntfn_صف_سر(ntfnPtr)
        || (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr)) {
        userError("TCB BindNotification: Notification cannot be bound.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }


    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
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

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
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

#اگر_تعریف_شده CONFIG_KERNEL_MCS
ساكن مضمن exception_t installTCBCap(tcb_t *هدف, cap_t tCap, cte_t *جایگاه,
                                        tcb_cnode_شاخص_t شاخص, cap_t newCap, cte_t *src_جایگاه)
{
    cte_t *ریشه_جایگاه = TCB_PTR_CTE_PTR(هدف, شاخص);
    استفادهنشده exception_t e = cteDelete(ریشه_جایگاه, true);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    إذا (cap_get_capType(newCap) != cap_null_cap) {
        إذا (sameObjectAs(newCap, src_جایگاه->cap) &&
            sameObjectAs(tCap, جایگاه->cap)) {
            cte_درج(newCap, src_جایگاه, ریشه_جایگاه);
        }
    }
    أرجع e;
}
#پایان_اگر

#اگر_تعریف_شده CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *هدف, cte_t *جایگاه,
                                        cap_t fh_newCap, cte_t *fh_src_جایگاه,
                                        cap_t th_newCap, cte_t *th_src_جایگاه,
                                        cap_t c_ریشه_new_Cap, cte_t *c_ریشه_src_جایگاه,
                                        cap_t v_ریشه_new_Cap, cte_t *v_ریشه_src_جایگاه,
                                        کلمه_t میانگیر_Addr, cap_t میانگیر_Cap,
                                        cte_t *میانگیر_Src_جایگاه,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((کلمه_t)هدف);

    إذا (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(هدف, tCap, جایگاه, tcbFaultHandler, fh_newCap, fh_src_جایگاه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }

    }

    إذا (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(هدف, tCap, جایگاه, tcbTimeoutHandler, th_newCap, th_src_جایگاه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(هدف, tCap, جایگاه, tcbCTable, c_ریشه_new_Cap, c_ریشه_src_جایگاه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }

        e = installTCBCap(هدف, tCap, جایگاه, tcbVTable, v_ریشه_new_Cap, v_ریشه_src_جایگاه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_caps_update_ipc_میانگیر) {
        cte_t *میانگیر_جایگاه;

        میانگیر_جایگاه = TCB_PTR_CTE_PTR(هدف, tcb_میانگیر);
        e = cteDelete(میانگیر_جایگاه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        هدف->tcbIPCBuffer = میانگیر_Addr;

        إذا (میانگیر_Src_جایگاه && sameObjectAs(میانگیر_Cap, میانگیر_Src_جایگاه->cap) &&
            sameObjectAs(tCap, جایگاه->cap)) {
            cte_درج(میانگیر_Cap, میانگیر_Src_جایگاه, میانگیر_جایگاه);
        }

        إذا (هدف == گره_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t invokeTCB_ThreadControl(tcb_t *هدف, cte_t *جایگاه,
                                    cptr_t faultep, prio_t mcp, prio_t اولویت,
                                    cap_t c_ریشه_new_Cap, cte_t *c_ریشه_src_جایگاه,
                                    cap_t v_ریشه_new_Cap, cte_t *v_ریشه_src_جایگاه,
                                    کلمه_t میانگیر_Addr, cap_t میانگیر_Cap,
                                    cte_t *میانگیر_Src_جایگاه,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((کلمه_t)هدف);

    إذا (updateFlags & thread_control_update_space) {
        هدف->tcbFaultHandler = faultep;
    }

    إذا (updateFlags & thread_control_update_mcp) {
        مجموعه_MCPriority(هدف, mcp);
    }

    إذا (updateFlags & thread_control_update_space) {
        cte_t *ریشه_جایگاه;

        ریشه_جایگاه = TCB_PTR_CTE_PTR(هدف, tcbCTable);
        e = cteDelete(ریشه_جایگاه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        إذا (sameObjectAs(c_ریشه_new_Cap, c_ریشه_src_جایگاه->cap) &&
            sameObjectAs(tCap, جایگاه->cap)) {
            cte_درج(c_ریشه_new_Cap, c_ریشه_src_جایگاه, ریشه_جایگاه);
        }
    }

    إذا (updateFlags & thread_control_update_space) {
        cte_t *ریشه_جایگاه;

        ریشه_جایگاه = TCB_PTR_CTE_PTR(هدف, tcbVTable);
        e = cteDelete(ریشه_جایگاه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        إذا (sameObjectAs(v_ریشه_new_Cap, v_ریشه_src_جایگاه->cap) &&
            sameObjectAs(tCap, جایگاه->cap)) {
            cte_درج(v_ریشه_new_Cap, v_ریشه_src_جایگاه, ریشه_جایگاه);
        }
    }

    إذا (updateFlags & thread_control_update_ipc_میانگیر) {
        cte_t *میانگیر_جایگاه;

        میانگیر_جایگاه = TCB_PTR_CTE_PTR(هدف, tcb_میانگیر);
        e = cteDelete(میانگیر_جایگاه, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        هدف->tcbIPCBuffer = میانگیر_Addr;

        إذا (میانگیر_Src_جایگاه && sameObjectAs(میانگیر_Cap, میانگیر_Src_جایگاه->cap) &&
            sameObjectAs(tCap, جایگاه->cap)) {
            cte_درج(میانگیر_Cap, میانگیر_Src_جایگاه, میانگیر_جایگاه);
        }

        إذا (هدف == گره_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    إذا (updateFlags & thread_control_update_اولویت) {
        مجموعه_اولویت(هدف, اولویت);
    }

    أرجع EXCEPTION_NONE;
}
#پایان_اگر

#اگر_تعریف_شده CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *هدف, cte_t *جایگاه,
                                         cap_t fh_newCap, cte_t *fh_src_جایگاه,
                                         prio_t mcp, prio_t اولویت,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    إذا (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((کلمه_t)هدف);
        exception_t e = installTCBCap(هدف, tCap, جایگاه, tcbFaultHandler, fh_newCap, fh_src_جایگاه);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_sched_update_mcp) {
        مجموعه_MCPriority(هدف, mcp);
    }

    إذا (updateFlags & thread_control_sched_update_اولویت) {
        مجموعه_اولویت(هدف, اولویت);
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
#پایان_اگر

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspend_منبع, bool_t resume_هدف,
                                    bool_t transfer_قاب, bool_t transferInteger,
                                    کلمه_t transferArch)
{
    إذا (suspend_منبع) {
        suspend(tcb_src);
    }

    إذا (resume_هدف) {
        restart(dest);
    }

    إذا (transfer_قاب) {
        کلمه_t i;
        کلمه_t v;
        کلمه_t pc;

        لكل (i = 0; i < n_قاب_Registers; i++) {
            v = get_ثبات(tcb_src, قاب_Registers[i]);
            مجموعه_ثبات(dest, قاب_Registers[i], v);
        }

        pc = getRestartPC(dest);
        مجموعه_بعدی_PC(dest, pc);
    }

    إذا (transferInteger) {
        کلمه_t i;
        کلمه_t v;

        لكل (i = 0; i < n_gpRegisters; i++) {
            v = get_ثبات(tcb_src, gpRegisters[i]);
            مجموعه_ثبات(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    إذا (dest == گره_STATE(ksCurThread)) {
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
exception_t invokeTCB_ReadRegisters(tcb_t *tcb_src, bool_t suspend_منبع,
                                    کلمه_t n, کلمه_t arch, bool_t call)
{
    کلمه_t i, j;
    exception_t e;
    tcb_t *thread;

    thread = گره_STATE(ksCurThread);

    إذا (suspend_منبع) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, گره_STATE(ksCurThread));
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    إذا (call) {
        کلمه_t *ipc_میانگیر;

        ipc_میانگیر = lookupIPCBuffer(true, thread);

        مجموعه_ثبات(thread, badge_ثبات, 0);

        لكل (i = 0; i < n && i < n_قاب_Registers && i < n_پیام_Registers; i++) {
            مجموعه_ثبات(thread, پیام_Registers[i],
                        get_ثبات(tcb_src, قاب_Registers[i]));
        }

        إذا (ipc_میانگیر != عدم && i < n && i < n_قاب_Registers) {
            لكل (; i < n && i < n_قاب_Registers; i++) {
                ipc_میانگیر[i + 1] = get_ثبات(tcb_src, قاب_Registers[i]);
            }
        }

        j = i;

        لكل (i = 0; i < n_gpRegisters && i + n_قاب_Registers < n
             && i + n_قاب_Registers < n_پیام_Registers; i++) {
            مجموعه_ثبات(thread, پیام_Registers[i + n_قاب_Registers],
                        get_ثبات(tcb_src, gpRegisters[i]));
        }

        إذا (ipc_میانگیر != عدم && i < n_gpRegisters
            && i + n_قاب_Registers < n) {
            لكل (; i < n_gpRegisters && i + n_قاب_Registers < n; i++) {
                ipc_میانگیر[i + n_قاب_Registers + 1] =
                    get_ثبات(tcb_src, gpRegisters[i]);
            }
        }

        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, i + j)));
    }
    مجموعه_Thread_State(thread, ThreadState_Running);

    أرجع EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resume_هدف,
                                     کلمه_t n, کلمه_t arch, کلمه_t *میانگیر)
{
    کلمه_t i;
    کلمه_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, گره_STATE(ksCurThread), dest);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    إذا (n > n_قاب_Registers + n_gpRegisters) {
        n = n_قاب_Registers + n_gpRegisters;
    }

    archInfo = Arch_get_Sanitise_ثبات_Info(dest);

    لكل (i = 0; i < n_قاب_Registers && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        مجموعه_ثبات(dest, قاب_Registers[i],
                    sanitise_ثبات(قاب_Registers[i],
                                     getSyscallArg(i + 2, میانگیر), archInfo));
    }

    لكل (i = 0; i < n_gpRegisters && i + n_قاب_Registers < n; i++) {
        مجموعه_ثبات(dest, gpRegisters[i],
                    sanitise_ثبات(gpRegisters[i],
                                     getSyscallArg(i + n_قاب_Registers + 2,
                                                   میانگیر), archInfo));
    }

    pc = getRestartPC(dest);
    مجموعه_بعدی_PC(dest, pc);

    Arch_postModifyRegisters(dest);

    إذا (resume_هدف) {
        restart(dest);
    }

    إذا (dest == گره_STATE(ksCurThread)) {
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

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
فراغ مجموعه_Thread_Name(tcb_t *tcb, ثابت حرف *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_درازی);
}
#پایان_اگر

کلمه_t مجموعه_MRs_syscall_error(tcb_t *thread, کلمه_t *receiveIPCBuffer)
{
    بدل (current_syscall_error.type) {
    حالة se_L4_Invalid_آرگومان:
        أرجع مجموعه_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_آرگومان_عدد);

    حالة seL4_InvalidCapability:
        أرجع مجموعه_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_عدد);

    حالة seL4_IllegalOperation:
        أرجع 0;

    حالة seL4_RangeError:
        مجموعه_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        أرجع مجموعه_MR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    حالة seL4_AlignmentError:
        أرجع 0;

    حالة seL4_FailedLookup:
        مجموعه_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failed_Lookup_Was_منبع ? 1 : 0);
        أرجع مجموعه_MRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    حالة se_L4_Truncated_پیام:
    حالة se_L4_Delete_اولی:
    حالة se_L4_Revoke_اولی:
        أرجع 0;
    حالة se_L4_Not_Enough_حافظه:
        أرجع مجموعه_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.حافظه_چپ);
    افتراضي:
        fail("Invalid syscall error");
    }
}
