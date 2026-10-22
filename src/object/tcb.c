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
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#جے_انت
#ضمن <object/tcb.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <model/statedata.h>
#ضمن <util.h>
#ضمن <string.h>
#ضمن <stdint.h>
#ضمن <arch/smp/ipi_inline.h>

#تعریف_کرو NULL_PRIO 0

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

ساكن مضمن فراغ جوڑو_منزل_Bitmap(لفظ_t cpu, لفظ_t dom, لفظ_t prio)
{
    لفظ_t l1index;
    لفظ_t l1index_inverted;

    l1index = prio_منزل_l1index(prio);
    l1index_inverted = invert_l1index(l1index);

    گرہ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) |= ثنائی_ہندسہ(l1index);
    /* we invert the l1 index when accessed the 2nd level of the bitmap in
       order to increase the likelihood that high prio threads l2 index word will
       be on the same cache line as the l1 index word - this makes sure the
       fastpath is fastest for high prio threads */
    گرہ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) |= ثنائی_ہندسہ(prio & نقاب(لفظ_Radix));
}

ساكن مضمن فراغ removeFromBitmap(لفظ_t cpu, لفظ_t dom, لفظ_t prio)
{
    لفظ_t l1index;
    لفظ_t l1index_inverted;

    l1index = prio_منزل_l1index(prio);
    l1index_inverted = invert_l1index(l1index);
    گرہ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu) &= ~ثنائی_ہندسہ(prio & نقاب(لفظ_Radix));
    إذا (unlikely(!گرہ_STATE_ON_CORE(ksReadyQueuesL2Bitmap[dom][l1index_inverted], cpu))) {
        گرہ_STATE_ON_CORE(ksReadyQueuesL1Bitmap[dom], cpu) &= ~ثنائی_ہندسہ(l1index);
    }
}

tcb_قطار_t tcb_قطار_remove(tcb_قطار_t قطار, tcb_t *tcb)
{
    tcb_t *before;
    tcb_t *after;

    before = tcb->tcbSchedPrev;
    after = tcb->tcb_Sched_اگلا;

    إذا (قطار.سر == tcb && قطار.سرا == tcb) {
        قطار.سر = عدم;
        قطار.سرا = عدم;
    } وإلا {
        إذا (قطار.سر == tcb) {
            after->tcbSchedPrev = عدم;
            tcb->tcb_Sched_اگلا = عدم;
            قطار.سر = after;
        } وإلا {
            إذا (قطار.سرا == tcb) {
                before->tcb_Sched_اگلا = عدم;
                tcb->tcbSchedPrev = عدم;
                قطار.سرا = before;
            } وإلا {
                before->tcb_Sched_اگلا = after;
                after->tcbSchedPrev = before;
                tcb->tcbSchedPrev = عدم;
                tcb->tcb_Sched_اگلا = عدم;
            }
        }
    }

    أرجع قطار;
}

/* Add TCB to the head of a scheduler queue */
فراغ tcb_Sched_قطار_وچ_پاؤ(tcb_t *tcb)
{
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
#جے_انت

    إذا (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_قطار_t قطار;
        dom_t dom;
        prio_t prio;
        لفظ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ترجیح;
        idx = ready_queues_اشاریہ(dom, prio);
        قطار = گرہ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        إذا (tcb_قطار_خالی_تھاواں(قطار)) {
            جوڑو_منزل_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        گرہ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_قطار_prepend(قطار, tcb);

        thread_state_ptr_سیٹ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Add TCB to the end of a scheduler queue */
فراغ tcbSchedAppend(tcb_t *tcb)
{
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    أكد(isSchedulable(tcb));
    أكد(refill_sufficient(tcb->tcbSchedContext, 0));
    أكد(refill_ready(tcb->tcbSchedContext));
#جے_انت
    إذا (!thread_state_get_tcbQueued(tcb->tcbState)) {
        tcb_قطار_t قطار;
        dom_t dom;
        prio_t prio;
        لفظ_t idx;

        dom = tcb->tcbDomain;
        prio = tcb->tcb_ترجیح;
        idx = ready_queues_اشاریہ(dom, prio);
        قطار = گرہ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity);

        إذا (tcb_قطار_خالی_تھاواں(قطار)) {
            جوڑو_منزل_Bitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }

        گرہ_STATE_ON_CORE(ksReadyQueues[idx], tcb->tcbAffinity) = tcb_قطار_append(قطار, tcb);

        thread_state_ptr_سیٹ_tcb_Queued(&tcb->tcbState, true);
    }
}

/* Remove TCB from a scheduler queue */
فراغ tcb_Sched_قطار_توں_کڈھو(tcb_t *tcb)
{
    إذا (thread_state_get_tcbQueued(tcb->tcbState)) {
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

        إذا (likely(tcb_قطار_خالی_تھاواں(new_قطار))) {
            removeFromBitmap(SMP_TERNARY(tcb->tcbAffinity, 0), dom, prio);
        }
    }
}

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
فراغ tcbDebugAppend(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);
    /* prepend to the list */
    debug_tcb->tcbDebugPrev = عدم;

    debug_tcb->tcb_Debug_اگلا = گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity);

    إذا (گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        TCB_PTR_DEBUG_PTR(گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity))->tcbDebugPrev = tcb;
    }

    گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = tcb;
}

فراغ tcbDebugRemove(tcb_t *tcb)
{
    debug_tcb_t *debug_tcb = TCB_PTR_DEBUG_PTR(tcb);

    أكد(گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) != عدم);
    إذا (tcb == گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity)) {
        گرہ_STATE_ON_CORE(ksDebugTCBs, tcb->tcbAffinity) = TCB_PTR_DEBUG_PTR(گرہ_STATE_ON_CORE(ksDebugTCBs,
                                                                                                 tcb->tcbAffinity))->tcb_Debug_اگلا;
    } وإلا {
        أكد(TCB_PTR_DEBUG_PTR(tcb)->tcbDebugPrev);
        TCB_PTR_DEBUG_PTR(debug_tcb->tcbDebugPrev)->tcb_Debug_اگلا = debug_tcb->tcb_Debug_اگلا;
    }

    إذا (debug_tcb->tcb_Debug_اگلا) {
        TCB_PTR_DEBUG_PTR(debug_tcb->tcb_Debug_اگلا)->tcbDebugPrev = debug_tcb->tcbDebugPrev;
    }

    debug_tcb->tcbDebugPrev = عدم;
    debug_tcb->tcb_Debug_اگلا = عدم;
}
#جے_انت /* CONFIG_DEBUG_BUILD */

#جے_بنا_تعریف CONFIG_KERNEL_MCS
/* Add TCB to the end of an endpoint queue */
tcb_قطار_t tcbEPAppend(tcb_t *tcb, tcb_قطار_t قطار)
{
    إذا (!قطار.سر) { /* Empty list */
        قطار.سر = tcb;
    } وإلا {
        قطار.سرا->tcbEPNext = tcb;
    }
    tcb->tcbEPPrev = قطار.سرا;
    tcb->tcbEPNext = عدم;
    قطار.سرا = tcb;

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
        قطار.سرا = tcb->tcbEPPrev;
    }

    أرجع قطار;
}
#جے_انت /* CONFIG_KERNEL_MCS */

#جے_تعریف_شدہ CONFIG_KERNEL_MCS

فراغ tcbReleaseRemove(tcb_t *tcb)
{
    إذا (likely(thread_state_get_tcb_وچ_Release_قطار(tcb->tcbState))) {
        tcb_قطار_t قطار = گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity);

        إذا (قطار.سر == tcb) {
            گرہ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
        }

        گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity) = tcb_قطار_remove(قطار, tcb);

        thread_state_ptr_سیٹ_tcb_وچ_Release_قطار(&tcb->tcbState, false);
    }
}

ساكن مضمن ticks_t خالص tcb_Ready_ویلا(tcb_t *tcb)
{
    أرجع refill_سر(tcb->tcbSchedContext)->r_ویلا;
}

ساكن مضمن bool_t خالص ویلا_after(tcb_t *tcb, ticks_t new_ویلا)
{
    أرجع tcb != عدم && new_ویلا >= tcb_Ready_ویلا(tcb);
}

ساكن tcb_t *لبھو_ویلا_after(tcb_t *tcb, ticks_t new_ویلا)
{
    tcb_t *after = tcb;

    طالما (ویلا_after(after, new_ویلا)) {
        after = after->tcb_Sched_اگلا;
    }

    أرجع after;
}

فراغ tcb_Release_قطار_وچ_پاؤ(tcb_t *tcb)
{
    أكد(thread_state_get_tcb_وچ_Release_قطار(tcb->tcbState) == false);
    أكد(thread_state_get_tcbQueued(tcb->tcbState) == false);

    ticks_t new_ویلا;
    tcb_قطار_t قطار;

    قطار = گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity);
    new_ویلا = tcb_Ready_ویلا(tcb);

    إذا (tcb_قطار_خالی_تھاواں(قطار) || new_ویلا < tcb_Ready_ویلا(قطار.سر)) {
        گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity) = tcb_قطار_prepend(قطار, tcb);
    } وإلا {
        إذا (tcb_Ready_ویلا(قطار.سرا) <= new_ویلا) {
            گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity) = tcb_قطار_append(قطار, tcb);
        } وإلا {
            tcb_t *after;
            after = لبھو_ویلا_after(قطار.سر, new_ویلا);
            tcb_قطار_داخل_کرو(tcb, after);
        }
    }

    thread_state_ptr_سیٹ_tcb_وچ_Release_قطار(&tcb->tcbState, true);

    إذا (قطار.سر != گرہ_STATE_ON_CORE(ks_Release_قطار, tcb->tcbAffinity).سر) {
        گرہ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity) = true;
    }
}
#جے_انت

cptr_t خالص getExtraCPtr(لفظ_t *بفر_Ptr, لفظ_t i)
{
    أرجع (cptr_t)بفر_Ptr[se_L4_پیغام_Max_لمائی + 2 + i];
}

فراغ سیٹ_Extra_Badge(لفظ_t *بفر_Ptr, لفظ_t badge,
                   لفظ_t i)
{
    بفر_Ptr[se_L4_پیغام_Max_لمائی + 2 + i] = badge;
}

#جے_بنا_تعریف CONFIG_KERNEL_MCS
فراغ تیاری_Caller_Cap(tcb_t *sender, tcb_t *receiver, bool_t canGrant)
{
    cte_t *جواب_تھاں, *caller_تھاں;
    cap_t masterCap بن_ورتیا, callerCap بن_ورتیا;

    سیٹ_Thread_State(sender, Thread_State_Blocked_On_جواب);
    جواب_تھاں = TCB_PTR_CTE_PTR(sender, tcb_جواب);
    masterCap = جواب_تھاں->cap;
    /* Haskell error: "Sender must have a valid master reply cap" */
    أكد(cap_get_capType(masterCap) == cap_جواب_cap);
    أكد(cap_جواب_cap_get_cap_جواب_Master(masterCap));
    أكد(cap_جواب_cap_get_cap_جواب_Can_Grant(masterCap));
    أكد(TCB_PTR(cap_جواب_cap_get_cap_TCBPtr(masterCap)) == sender);
    caller_تھاں = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    callerCap = caller_تھاں->cap;
    /* Haskell error: "Caller cap must not already exist" */
    أكد(cap_get_capType(callerCap) == cap_null_cap);
    cte_داخل_کرو(cap_جواب_cap_new(canGrant, false, TCB_REF(sender)),
              جواب_تھاں, caller_تھاں);
}

فراغ deleteCallerCap(tcb_t *receiver)
{
    cte_t *caller_تھاں;

    caller_تھاں = TCB_PTR_CTE_PTR(receiver, tcbCaller);
    /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
    cteDeleteOne(caller_تھاں);
}
#جے_انت

extra_caps_t current_extra_caps;

exception_t lookupExtraCaps(tcb_t *thread, لفظ_t *بفر_Ptr, se_L4_سنیہا_Info_t info)
{
    lookup_تھاں_raw_ret_t lu_ret;
    cptr_t cptr;
    لفظ_t i, لمائی;

    إذا (!بفر_Ptr) {
        current_extra_caps.excaprefs[0] = عدم;
        أرجع EXCEPTION_NONE;
    }

    لمائی = se_L4_سنیہا_Info_get_extra_Caps(info);

    لكل (i = 0; i < لمائی; i++) {
        cptr = getExtraCPtr(بفر_Ptr, i);

        lu_ret = lookup_تھاں(thread, cptr);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            current_fault = seL4_Fault_CapFault_new(cptr, false);
            أرجع lu_ret.حالت;
        }

        current_extra_caps.excaprefs[i] = lu_ret.تھاں;
    }
    إذا (i < se_L4_پیغام_Max_Extra_Caps) {
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
    لكل (i = 0; i < n && i < n_پیغام_Registers; i++) {
        سیٹ_رجسٹر(receiver, پیغام_Registers[i],
                    get_رجسٹر(sender, پیغام_Registers[i]));
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

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
/* This checks if the current updated to scheduler queue is changing the previous scheduling
 * decision made by the scheduler. If its a case, an `irq_reschedule_ipi` is sent */
فراغ remote_قطار_Update(tcb_t *tcb)
{
    /* only ipi if the target is for the current domain */
    إذا (tcb->tcbAffinity != getCurrentCPUIndex() && tcb->tcbDomain == ksCurDomain) {
        tcb_t *نشانہ_Cur_Thread = گرہ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity);

        /* reschedule if the target core is idle or we are waking a higher priority thread (or
         * if a new irq would need to be set on MCS) */
        إذا (نشانہ_Cur_Thread == گرہ_STATE_ON_CORE(ksIdleThread, tcb->tcbAffinity)  ||
            tcb->tcb_ترجیح > نشانہ_Cur_Thread->tcb_ترجیح
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
            || گرہ_STATE_ON_CORE(ksReprogram, tcb->tcbAffinity)
#جے_انت
           ) {
            ARCH_گرہ_STATE(ipiReschedulePending) |= ثنائی_ہندسہ(tcb->tcbAffinity);
        }
    }
}

/* This makes sure the the TCB is not being run on other core.
 * It would request 'IpiRemoteCall_Stall' to switch the core from this TCB
 * We also request the 'irq_reschedule_ipi' to restore the state of target core */
فراغ remoteTCBStall(tcb_t *tcb)
{

    إذا (
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        tcb->tcbSchedContext &&
#جے_انت
        tcb->tcbAffinity != getCurrentCPUIndex() &&
        گرہ_STATE_ON_CORE(ksCurThread, tcb->tcbAffinity) == tcb) {
        doRemoteStall(tcb->tcbAffinity);
        ARCH_گرہ_STATE(ipiReschedulePending) |= ثنائی_ہندسہ(tcb->tcbAffinity);
    }
}

#جے_بنا_تعریف CONFIG_KERNEL_MCS
ساكن exception_t invoke_TCB_سیٹ_Affinity(tcb_t *thread, لفظ_t affinity)
{
    /* remove the tcb from scheduler queue in case it is already in one
     * and add it to new queue if required */
    tcb_Sched_قطار_توں_کڈھو(thread);
    migrateTCB(thread, affinity);
    إذا (isRunnable(thread)) {
        SCHED_APPEND(thread);
    }
    /* reschedule current cpu if tcb moves itself */
    إذا (thread == گرہ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سیٹ_Affinity(cap_t cap, لفظ_t لمائی, لفظ_t *بفر)
{
    tcb_t *tcb;
    لفظ_t affinity;

    إذا (لمائی < 1) {
        userError("TCB SetAffinity: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    affinity = getSyscallArg(0, بفر);
    إذا (affinity >= ks_عدد_CPUs) {
        userError("TCB SetAffinity: Requested CPU does not exist.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_TCB_سیٹ_Affinity(tcb, affinity);
}
#جے_انت
#جے_انت /* ENABLE_SMP_SUPPORT */

#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
ساكن exception_t invokeConfigureSingleStepping(bool_t call, لفظ_t *بفر, tcb_t *t,
                                                 uint16_t bp_عدد, لفظ_t n_instrs)
{
    bool_t bp_was_consumed;
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    لفظ_t قدر;

    bp_was_consumed = configureSingleStepping(t, bp_عدد, n_instrs, false);
    إذا (n_instrs == 0) {
#جے_بنا_تعریف CONFIG_ARCH_AARCH64
        unset_Breakpoint_ورتیا_Flag(t, bp_عدد);
#جے_انت /* CONFIG_ARCH_AARCH64 */
        قدر = false;
    } وإلا {
#جے_بنا_تعریف CONFIG_ARCH_AARCH64
        سیٹ_Breakpoint_ورتیا_Flag(t, bp_عدد);
#جے_انت /* CONFIG_ARCH_AARCH64 */
        قدر = bp_was_consumed;
    }

    إذا (call) {
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        بنانشانی صحيح لمائی = سیٹ_MR(thread, بفر, 0, قدر);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                        se_L4_سنیہا_Info_new(0, 0, 0, لمائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeConfigureSingleStepping(cap_t cap, bool_t call, لفظ_t *بفر)
{
    uint16_t bp_عدد;
    لفظ_t n_instrs;
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

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeConfigureSingleStepping(call, بفر, tcb, bp_عدد, n_instrs);
}

ساكن exception_t invoke_سیٹ_Breakpoint(tcb_t *tcb, uint16_t bp_عدد,
                                       لفظ_t vaddr, لفظ_t type, لفظ_t ناپ, لفظ_t rw)
{
    سیٹ_Breakpoint(tcb, bp_عدد, vaddr, type, ناپ, rw);
    /* Signal restore_user_context() to pop the breakpoint context on return. */
    سیٹ_Breakpoint_ورتیا_Flag(tcb, bp_عدد);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سیٹ_Breakpoint(cap_t cap, لفظ_t *بفر)
{
    uint16_t bp_عدد;
    لفظ_t vaddr, type, ناپ, rw;
    tcb_t *tcb;
    syscall_error_t error;

    tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    bp_عدد = getSyscallArg(0, بفر);
    vaddr = getSyscallArg(1, بفر);
    type = getSyscallArg(2, بفر);
    ناپ = getSyscallArg(3, بفر);
    rw = getSyscallArg(4, بفر);

    /* We disallow the user to set breakpoint addresses that are in the kernel
     * vaddr range.
     */
    إذا (vaddr >= (لفظ_t)USER_چوٹی) {
        userError("Debug: Invalid address %lx: bp addresses must be userspace "
                  "addresses.",
                  vaddr);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (type != seL4_InstructionBreakpoint && type != se_L4_انکڑے_Breakpoint) {
        userError("Debug: Unknown breakpoint type %lx.", type);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    } وإلا إذا (type == seL4_InstructionBreakpoint) {
        إذا (ناپ != 0) {
            userError("Debug: Instruction bps must have size of 0.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 3;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (rw != seL4_BreakOnRead) {
            userError("Debug: Instruction bps must be break-on-read.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 4;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا ((se_L4_پہلا_Watchpoint == -1 || bp_عدد >= se_L4_پہلا_Watchpoint)
            && se_L4_پہلا_Breakpoint != se_L4_پہلا_Watchpoint) {
            userError("Debug: Can't specify a watchpoint ID with type seL4_InstructionBreakpoint.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 2;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    } وإلا إذا (type == se_L4_انکڑے_Breakpoint) {
        إذا (ناپ == 0) {
            userError("Debug: Data bps cannot have size of 0.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 3;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (se_L4_پہلا_Watchpoint != -1 && bp_عدد < se_L4_پہلا_Watchpoint) {
            userError("Debug: Data watchpoints cannot specify non-data watchpoint ID.");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 2;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    } وإلا إذا (type == seL4_SoftwareBreakRequest) {
        userError("Debug: Use a software breakpoint instruction to trigger a "
                  "software breakpoint.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (rw != seL4_BreakOnRead && rw != seL4_BreakOnWrite
        && rw != seL4_BreakOnReadWrite) {
        userError("Debug: Unknown access-type %lu.", rw);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (ناپ != 0 && ناپ != 1 && ناپ != 2 && ناپ != 4 && ناپ != 8) {
        userError("Debug: Invalid size %lu.", ناپ);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (ناپ > 0 && vaddr & (ناپ - 1)) {
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
                  vaddr, ناپ);

        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    error = Arch_decode_سیٹ_Breakpoint(tcb, bp_عدد, vaddr, type, ناپ, rw);
    إذا (error.type != seL4_NoError) {
        current_syscall_error = error;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_سیٹ_Breakpoint(tcb, bp_عدد,
                               vaddr, type, ناپ, rw);
}

ساكن exception_t invokeGetBreakpoint(bool_t call, لفظ_t *بفر, tcb_t *tcb, uint16_t bp_عدد)
{
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    getBreakpoint_t res;
    res = getBreakpoint(tcb, bp_عدد);
    إذا (call) {
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, 0, res.vaddr);
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, 1, res.type);
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, 2, res.ناپ);
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, 3, res.rw);
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, 4, res.is_enabled);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                        se_L4_سنیہا_Info_new(0, 0, 0, 5)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeGetBreakpoint(cap_t cap, bool_t call, لفظ_t *بفر)
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

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeGetBreakpoint(call, بفر, tcb, bp_عدد);
}

ساكن exception_t invokeUnsetBreakpoint(tcb_t *tcb, uint16_t bp_عدد)
{
    /* Maintain the bitfield of in-use breakpoints. */
    unsetBreakpoint(tcb, bp_عدد);
    unset_Breakpoint_ورتیا_Flag(tcb, bp_عدد);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeUnsetBreakpoint(cap_t cap, لفظ_t *بفر)
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

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeUnsetBreakpoint(tcb, bp_عدد);
}
#جے_انت /* CONFIG_HARDWARE_DEBUG_API */

ساكن exception_t invoke_سیٹ_TLSBase(tcb_t *thread, لفظ_t tls_ادھار)
{
    سیٹ_رجسٹر(thread, TLS_ادھار, tls_ادھار);
    إذا (thread == گرہ_STATE(ksCurThread)) {
        /* If this is the current thread force a reschedule to ensure that any changes
         * to the TLS_BASE are realized */
        rescheduleRequired();
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سیٹ_TLSBase(cap_t cap, لفظ_t لمائی, لفظ_t *بفر)
{
    لفظ_t tls_ادھار;

    إذا (لمائی < 1) {
        userError("TCB SetTLSBase: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tls_ادھار = getSyscallArg(0, بفر);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_سیٹ_TLSBase(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), tls_ادھار);
}

ساكن فراغ invoke_سیٹ_Flags(tcb_t *thread, لفظ_t clear, لفظ_t سیٹ, bool_t call)
{
    tcb_t *cur_thread = گرہ_STATE(ksCurThread);
    لفظ_t flags = thread->tcbFlags;

    flags &= ~clear;
    flags |= سیٹ & se_L4_TCBFlag_نقاب;
    thread->tcbFlags = flags;

#جے_تعریف_شدہ CONFIG_HAVE_FPU
    إذا (flags & seL4_TCBFlag_fpuDisabled) {
        /* Save current FPU state before disabling FPU: */
        fpuRelease(thread);
    } وإلا إذا (thread == cur_thread) {
        /* Restore FPU here as switchToThread() won't be called: */
        lazyFPURestore(thread);
    }
#جے_انت
    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, cur_thread);
        سیٹ_رجسٹر(cur_thread, badge_رجسٹر, 0);
        بنانشانی صحيح لمائی = سیٹ_MR(cur_thread, ipc_بفر, 0, flags);
        سیٹ_رجسٹر(cur_thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                        se_L4_سنیہا_Info_new(0, 0, 0, لمائی)));
    }
    سیٹ_Thread_State(cur_thread, ThreadState_Running);
}

ساكن exception_t decode_سیٹ_Flags(cap_t cap, لفظ_t لمائی, bool_t call, لفظ_t *بفر)
{
    tcb_t *thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));

    إذا (لمائی < 2) {
        userError("TCB SetFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t clear = getSyscallArg(0, بفر);
    لفظ_t سیٹ   = getSyscallArg(1, بفر);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invoke_سیٹ_Flags(thread, clear, سیٹ, call);
    أرجع EXCEPTION_NONE;
}

/* The following functions sit in the syscall error monad, but include the
 * exception cases for the preemptible bottom end, as they call the invoke
 * functions directly.  This is a significant deviation from the Haskell
 * spec. */
exception_t decodeTCBInvocation(لفظ_t invLabel, لفظ_t لمائی, cap_t cap,
                                cte_t *تھاں, bool_t call, لفظ_t *بفر)
{
    /* Stall the core if we are operating on a remote TCB that is currently running */
    SMP_COND_STATEMENT(remoteTCBStall(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));)

    بدل (invLabel) {
    حالة TCBReadRegisters:
        /* Second level of decoding */
        أرجع decodeReadRegisters(cap, لمائی, call, بفر);

    حالة TCBWriteRegisters:
        أرجع decodeWriteRegisters(cap, لمائی, بفر);

    حالة TCBCopyRegisters:
        أرجع decodeCopyRegisters(cap, لمائی, بفر);

    حالة TCBSuspend:
        /* Jump straight to the invoke */
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeTCB_Suspend(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالة TCBResume:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeTCB_Resume(
                   TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));

    حالة TCBConfigure:
        أرجع decodeTCBConfigure(cap, لمائی, تھاں, بفر);

    حالة TCBSet_ترجیح:
        أرجع decode_سیٹ_ترجیح(cap, لمائی, بفر);

    حالة TCBSetMCPriority:
        أرجع decode_سیٹ_MCPriority(cap, لمائی, بفر);

    حالة TCBSetSchedParams:
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        أرجع decode_سیٹ_Sched_Params(cap, لمائی, تھاں, بفر);
#وإلا
        أرجع decode_سیٹ_Sched_Params(cap, لمائی, بفر);
#جے_انت

    حالة TCBSetIPCBuffer:
        أرجع decode_سیٹ_IPCBuffer(cap, لمائی, تھاں, بفر);

    حالة TCBSetSpace:
        أرجع decode_سیٹ_Space(cap, لمائی, تھاں, بفر);

    حالة TCBBindNotification:
        أرجع decodeBindNotification(cap);

    حالة TCBUnbindNotification:
        أرجع decodeUnbindNotification(cap);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    حالة TCBSetTimeoutEndpoint:
        أرجع decode_سیٹ_Timeout_Endpoint(cap, تھاں);
#وإلا
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    حالة TCBSetAffinity:
        أرجع decode_سیٹ_Affinity(cap, لمائی, بفر);
#جے_انت /* ENABLE_SMP_SUPPORT */
#جے_انت

        /* There is no notion of arch specific TCB invocations so this needs to go here */
#جے_تعریف_شدہ CONFIG_VTX
    حالة TCBSetEPTRoot:
        أرجع decode_سیٹ_EPTRoot(cap);
#جے_انت

#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    حالة TCBConfigureSingleStepping:
        أرجع decodeConfigureSingleStepping(cap, call, بفر);

    حالة TCBSetBreakpoint:
        أرجع decode_سیٹ_Breakpoint(cap, بفر);

    حالة TCBGetBreakpoint:
        أرجع decodeGetBreakpoint(cap, call, بفر);

    حالة TCBUnsetBreakpoint:
        أرجع decodeUnsetBreakpoint(cap, بفر);
#جے_انت

    حالة TCBSetTLSBase:
        أرجع decode_سیٹ_TLSBase(cap, لمائی, بفر);

    حالة TCBSetFlags:
        أرجع decode_سیٹ_Flags(cap, لمائی, call, بفر);

    افتراضي:
        /* Haskell: "throw IllegalOperation" */
        userError("TCB: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

تعداد CopyRegistersFlags {
    Copy_Registers_suspend_سروت = 0,
    Copy_Registers_resume_نشانہ = 1,
    Copy_Registers_transfer_چوکھٹا = 2,
    CopyRegisters_transferInteger = 3
};

exception_t decodeCopyRegisters(cap_t cap, لفظ_t لمائی, لفظ_t *بفر)
{
    لفظ_t transferArch;
    tcb_t *srcTCB;
    cap_t سروت_cap;
    لفظ_t flags;

    إذا (لمائی < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB CopyRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);

    transferArch = Arch_decodeTransfer(flags >> 8);

    سروت_cap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(سروت_cap) == cap_thread_cap) {
        srcTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(سروت_cap));
    } وإلا {
        userError("TCB CopyRegisters: Invalid source TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_CopyRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), srcTCB,
               flags & ثنائی_ہندسہ(Copy_Registers_suspend_سروت),
               flags & ثنائی_ہندسہ(Copy_Registers_resume_نشانہ),
               flags & ثنائی_ہندسہ(Copy_Registers_transfer_چوکھٹا),
               flags & ثنائی_ہندسہ(CopyRegisters_transferInteger),
               transferArch);

}

تعداد ReadRegistersFlags {
    ReadRegisters_suspend = 0
};

exception_t decodeReadRegisters(cap_t cap, لفظ_t لمائی, bool_t call,
                                لفظ_t *بفر)
{
    لفظ_t transferArch, flags, n;
    tcb_t *thread;

    إذا (لمائی < 2) {
        userError("TCB ReadRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);
    n     = getSyscallArg(1, بفر);

    إذا (n < 1 || n > n_چوکھٹا_Registers + n_gpRegisters) {
        userError("TCB ReadRegisters: Attempted to read an invalid number of registers (%d).",
                  (صحيح)n);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = n_چوکھٹا_Registers +
                                              n_gpRegisters;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    إذا (thread == گرہ_STATE(ksCurThread)) {
        userError("TCB ReadRegisters: Attempted to read our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_ReadRegisters(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)),
               flags & ثنائی_ہندسہ(ReadRegisters_suspend),
               n, transferArch, call);
}

تعداد WriteRegistersFlags {
    WriteRegisters_resume = 0
};

exception_t decodeWriteRegisters(cap_t cap, لفظ_t لمائی, لفظ_t *بفر)
{
    لفظ_t flags, w;
    لفظ_t transferArch;
    tcb_t *thread;

    إذا (لمائی < 2) {
        userError("TCB WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    flags = getSyscallArg(0, بفر);
    w     = getSyscallArg(1, بفر);

    إذا (لمائی - 2 < w) {
        userError("TCB WriteRegisters: Message too short for requested write size (%d/%d).",
                  (صحيح)(لمائی - 2), (صحيح)w);
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    transferArch = Arch_decodeTransfer(flags >> 8);

    thread = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    إذا (thread == گرہ_STATE(ksCurThread)) {
        userError("TCB WriteRegisters: Attempted to write our own registers.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_WriteRegisters(thread,
                                    flags & ثنائی_ہندسہ(WriteRegisters_resume),
                                    w, transferArch, بفر);
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
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
#جے_انت

/* TCBConfigure batches SetIPCBuffer and parts of SetSpace. */
exception_t decodeTCBConfigure(cap_t cap, لفظ_t لمائی, cte_t *تھاں, لفظ_t *بفر)
{
    cte_t *بفر_تھاں, *c_جڑ_تھاں, *v_جڑ_تھاں;
    cap_t بفر_Cap, c_جڑ_Cap, v_جڑ_Cap;
    deriveCap_ret_t dc_ret;
    لفظ_t c_جڑ_انکڑے, v_جڑ_انکڑے, بفر_Addr;
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
#تعریف_کرو TCBCONFIGURE_دلیلاں 3
#وإلا
#تعریف_کرو TCBCONFIGURE_دلیلاں 4
#جے_انت
    إذا (لمائی < TCBCONFIGURE_دلیلاں || current_extra_caps.excaprefs[0] == عدم
        || current_extra_caps.excaprefs[1] == عدم
        || current_extra_caps.excaprefs[2] == عدم) {
        userError("TCB Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    c_جڑ_انکڑے     = getSyscallArg(0, بفر);
    v_جڑ_انکڑے     = getSyscallArg(1, بفر);
    بفر_Addr    = getSyscallArg(2, بفر);
#وإلا
    cptr_t faultEP       = getSyscallArg(0, بفر);
    c_جڑ_انکڑے     = getSyscallArg(1, بفر);
    v_جڑ_انکڑے     = getSyscallArg(2, بفر);
    بفر_Addr    = getSyscallArg(3, بفر);
#جے_انت

    c_جڑ_تھاں  = current_extra_caps.excaprefs[0];
    c_جڑ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_جڑ_تھاں  = current_extra_caps.excaprefs[1];
    v_جڑ_Cap   = current_extra_caps.excaprefs[1]->cap;
    بفر_تھاں = current_extra_caps.excaprefs[2];
    بفر_Cap  = current_extra_caps.excaprefs[2]->cap;

    إذا (بفر_Addr == 0) {
        بفر_تھاں = عدم;
    } وإلا {
        dc_ret = deriveCap(بفر_تھاں, بفر_Cap);
        إذا (dc_ret.حالت != EXCEPTION_NONE) {
            أرجع dc_ret.حالت;
        }
        بفر_Cap = dc_ret.cap;

        exception_t e = checkValidIPCBuffer(بفر_Addr, بفر_Cap);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (تھاں_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        تھاں_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB Configure: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (c_جڑ_انکڑے != 0) {
        c_جڑ_Cap = update_Cap_انکڑے(false, c_جڑ_انکڑے, c_جڑ_Cap);
    }

    dc_ret = deriveCap(c_جڑ_تھاں, c_جڑ_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    c_جڑ_Cap = dc_ret.cap;

    إذا (cap_get_capType(c_جڑ_Cap) != cap_cnode_cap) {
        userError("TCB Configure: CSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (v_جڑ_انکڑے != 0) {
        v_جڑ_Cap = update_Cap_انکڑے(false, v_جڑ_انکڑے, v_جڑ_Cap);
    }

    dc_ret = deriveCap(v_جڑ_تھاں, v_جڑ_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    v_جڑ_Cap = dc_ret.cap;

    إذا (!is_Valid_VTable_جڑ(v_جڑ_Cap)) {
        userError("TCB Configure: VSpace cap is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تھاں,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               c_جڑ_Cap, c_جڑ_تھاں,
               v_جڑ_Cap, v_جڑ_تھاں,
               بفر_Addr, بفر_Cap,
               بفر_تھاں, thread_control_caps_update_space |
               thread_control_caps_update_ipc_بفر);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تھاں,
               faultEP, NULL_PRIO, NULL_PRIO,
               c_جڑ_Cap, c_جڑ_تھاں,
               v_جڑ_Cap, v_جڑ_تھاں,
               بفر_Addr, بفر_Cap,
               بفر_تھاں, thread_control_update_space |
               thread_control_update_ipc_بفر);
#جے_انت
}

exception_t decode_سیٹ_ترجیح(cap_t cap, لفظ_t لمائی, لفظ_t *بفر)
{
    إذا (لمائی < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newPrio = getSyscallArg(0, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("Set priority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت = checkPrio(newPrio, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetPriority: Requested priority %lu too high (max %lu).",
                  (بنانشانی لما) newPrio, (بنانشانی لما) authTCB->tcbMCP);
        أرجع حالت;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               cap_null_cap_new(), عدم,
               NULL_PRIO, newPrio,
               عدم, thread_control_sched_update_ترجیح);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), عدم,
               0, NULL_PRIO, newPrio,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(),
               عدم, thread_control_update_ترجیح);
#جے_انت
}

exception_t decode_سیٹ_MCPriority(cap_t cap, لفظ_t لمائی, لفظ_t *بفر)
{
    إذا (لمائی < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetMCPriority: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetMCPriority: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت = checkPrio(newMcp, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetMCPriority: Requested maximum controlled priority %lu too high (max %lu).",
                  (بنانشانی لما) newMcp, (بنانشانی لما) authTCB->tcbMCP);
        أرجع حالت;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
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
#جے_انت
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t decode_سیٹ_Timeout_Endpoint(cap_t cap, cte_t *تھاں)
{
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetTimeoutEndpoint: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cte_t *th_تھاں = current_extra_caps.excaprefs[0];
    cap_t thCap   = current_extra_caps.excaprefs[0]->cap;

    /* timeout handler */
    إذا (!validFaultHandler(thCap)) {
        userError("TCB SetTimeoutEndpoint: timeout endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تھاں,
               cap_null_cap_new(), عدم,
               thCap, th_تھاں,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               0, cap_null_cap_new(), عدم,
               thread_control_caps_update_timeout);
}
#جے_انت

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t decode_سیٹ_Sched_Params(cap_t cap, لفظ_t لمائی, cte_t *تھاں, لفظ_t *بفر)
#وإلا
exception_t decode_سیٹ_Sched_Params(cap_t cap, لفظ_t لمائی, لفظ_t *بفر)
#جے_انت
{
    إذا (لمائی < 2 || current_extra_caps.excaprefs[0] == عدم
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[1] == عدم || current_extra_caps.excaprefs[2] == عدم
#جے_انت
       ) {
        userError("TCB SetSchedParams: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    prio_t newMcp = getSyscallArg(0, بفر);
    prio_t newPrio = getSyscallArg(1, بفر);
    cap_t authCap = current_extra_caps.excaprefs[0]->cap;
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    cap_t scCap   = current_extra_caps.excaprefs[1]->cap;
    cte_t *fh_تھاں = current_extra_caps.excaprefs[2];
    cap_t fhCap   = current_extra_caps.excaprefs[2]->cap;
#جے_انت

    إذا (cap_get_capType(authCap) != cap_thread_cap) {
        userError("TCB SetSchedParams: authority cap not a TCB.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *authTCB = TCB_PTR(cap_thread_cap_get_capTCBPtr(authCap));
    exception_t حالت = checkPrio(newMcp, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested maximum controlled priority %lu too high (max %lu).",
                  (بنانشانی لما) newMcp, (بنانشانی لما) authTCB->tcbMCP);
        أرجع حالت;
    }

    حالت = checkPrio(newPrio, authTCB);
    إذا (حالت != EXCEPTION_NONE) {
        userError("TCB SetSchedParams: Requested priority %lu too high (max %lu).",
                  (بنانشانی لما) newPrio, (بنانشانی لما) authTCB->tcbMCP);
        أرجع حالت;
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    tcb_t *tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
    sched_context_t *sc = عدم;
    thread_control_flag_t update_flags = thread_control_sched_update_mcp |
                                         thread_control_sched_update_ترجیح |
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
        إذا (tcb == گرہ_STATE(ksCurThread)) {
            userError("TCB SetSchedParams: Cannot change sched_context of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        اكسر;
    افتراضي:
        userError("TCB Configure: sched context cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If we are setting or unsetting the scheduling context, update the flags */
    إذا (tcb->tcbSchedContext != sc) {
        update_flags |= thread_control_sched_update_sc;
    }

    إذا (!validFaultHandler(fhCap)) {
        userError("TCB Configure: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 3;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#جے_انت
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlSched(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تھاں,
               fhCap, fh_تھاں,
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
               thread_control_update_ترجیح);
#جے_انت
}


exception_t decode_سیٹ_IPCBuffer(cap_t cap, لفظ_t لمائی, cte_t *تھاں, لفظ_t *بفر)
{
    cptr_t cptr_بفر_Ptr;
    cap_t بفر_Cap;
    cte_t *بفر_تھاں;

    إذا (لمائی < 1 || current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB SetIPCBuffer: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cptr_بفر_Ptr  = getSyscallArg(0, بفر);
    بفر_تھاں = current_extra_caps.excaprefs[0];
    بفر_Cap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cptr_بفر_Ptr == 0) {
        بفر_تھاں = عدم;
    } وإلا {
        exception_t e;
        deriveCap_ret_t dc_ret;

        dc_ret = deriveCap(بفر_تھاں, بفر_Cap);
        إذا (dc_ret.حالت != EXCEPTION_NONE) {
            أرجع dc_ret.حالت;
        }
        بفر_Cap = dc_ret.cap;
        e = checkValidIPCBuffer(cptr_بفر_Ptr, بفر_Cap);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تھاں,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cptr_بفر_Ptr, بفر_Cap,
               بفر_تھاں, thread_control_caps_update_ipc_بفر);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تھاں,
               0, NULL_PRIO, NULL_PRIO,
               cap_null_cap_new(), عدم,
               cap_null_cap_new(), عدم,
               cptr_بفر_Ptr, بفر_Cap,
               بفر_تھاں, thread_control_update_ipc_بفر);

#جے_انت
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
#تعریف_کرو DECODE_سیٹ_SPACE_PARAMS 2
#وإلا
#تعریف_کرو DECODE_سیٹ_SPACE_PARAMS 3
#جے_انت
exception_t decode_سیٹ_Space(cap_t cap, لفظ_t لمائی, cte_t *تھاں, لفظ_t *بفر)
{
    لفظ_t c_جڑ_انکڑے, v_جڑ_انکڑے;
    cte_t *c_جڑ_تھاں, *v_جڑ_تھاں;
    cap_t c_جڑ_Cap, v_جڑ_Cap;
    deriveCap_ret_t dc_ret;

    إذا (لمائی < DECODE_سیٹ_SPACE_PARAMS || current_extra_caps.excaprefs[0] == عدم
        || current_extra_caps.excaprefs[1] == عدم
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        || current_extra_caps.excaprefs[2] == عدم
#جے_انت
       ) {
        userError("TCB SetSpace: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    c_جڑ_انکڑے = getSyscallArg(0, بفر);
    v_جڑ_انکڑے = getSyscallArg(1, بفر);

    cte_t *fh_تھاں     = current_extra_caps.excaprefs[0];
    cap_t fhCap      = current_extra_caps.excaprefs[0]->cap;
    c_جڑ_تھاں  = current_extra_caps.excaprefs[1];
    c_جڑ_Cap   = current_extra_caps.excaprefs[1]->cap;
    v_جڑ_تھاں  = current_extra_caps.excaprefs[2];
    v_جڑ_Cap   = current_extra_caps.excaprefs[2]->cap;
#وإلا
    cptr_t faultEP   = getSyscallArg(0, بفر);
    c_جڑ_انکڑے = getSyscallArg(1, بفر);
    v_جڑ_انکڑے = getSyscallArg(2, بفر);

    c_جڑ_تھاں  = current_extra_caps.excaprefs[0];
    c_جڑ_Cap   = current_extra_caps.excaprefs[0]->cap;
    v_جڑ_تھاں  = current_extra_caps.excaprefs[1];
    v_جڑ_Cap   = current_extra_caps.excaprefs[1]->cap;
#جے_انت

    إذا (تھاں_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbCTable)) ||
        تھاں_Cap_Long_Running_Delete(
            TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), tcbVTable))) {
        userError("TCB SetSpace: CSpace or VSpace currently being deleted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (c_جڑ_انکڑے != 0) {
        c_جڑ_Cap = update_Cap_انکڑے(false, c_جڑ_انکڑے, c_جڑ_Cap);
    }

    dc_ret = deriveCap(c_جڑ_تھاں, c_جڑ_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    c_جڑ_Cap = dc_ret.cap;

    إذا (cap_get_capType(c_جڑ_Cap) != cap_cnode_cap) {
        userError("TCB SetSpace: Invalid CNode cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (v_جڑ_انکڑے != 0) {
        v_جڑ_Cap = update_Cap_انکڑے(false, v_جڑ_انکڑے, v_جڑ_Cap);
    }

    dc_ret = deriveCap(v_جڑ_تھاں, v_جڑ_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }
    v_جڑ_Cap = dc_ret.cap;

    إذا (!is_Valid_VTable_جڑ(v_جڑ_Cap)) {
        userError("TCB SetSpace: Invalid VSpace cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    /* fault handler */
    إذا (!validFaultHandler(fhCap)) {
        userError("TCB SetSpace: fault endpoint cap invalid.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#جے_انت

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    أرجع invokeTCB_ThreadControlCaps(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تھاں,
               fhCap, fh_تھاں,
               cap_null_cap_new(), عدم,
               c_جڑ_Cap, c_جڑ_تھاں,
               v_جڑ_Cap, v_جڑ_تھاں,
               0, cap_null_cap_new(), عدم, thread_control_caps_update_space | thread_control_caps_update_fault);
#وإلا
    أرجع invokeTCB_ThreadControl(
               TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), تھاں,
               faultEP,
               NULL_PRIO, NULL_PRIO,
               c_جڑ_Cap, c_جڑ_تھاں,
               v_جڑ_Cap, v_جڑ_تھاں,
               0, cap_null_cap_new(), عدم, thread_control_update_space);
#جے_انت
}

exception_t decodeBindNotification(cap_t cap)
{
    notification_t *ntfnPtr;
    tcb_t *tcb;
    cap_t ntfn_cap;

    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("TCB BindNotification: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
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


    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
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

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
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

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
ساكن مضمن exception_t installTCBCap(tcb_t *نشانہ, cap_t tCap, cte_t *تھاں,
                                        tcb_cnode_اشاریہ_t اشاریہ, cap_t newCap, cte_t *src_تھاں)
{
    cte_t *جڑ_تھاں = TCB_PTR_CTE_PTR(نشانہ, اشاریہ);
    بن_ورتیا exception_t e = cteDelete(جڑ_تھاں, true);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    /* cteDelete on a cap installed in the tcb cannot fail */
    إذا (cap_get_capType(newCap) != cap_null_cap) {
        إذا (sameObjectAs(newCap, src_تھاں->cap) &&
            sameObjectAs(tCap, تھاں->cap)) {
            cte_داخل_کرو(newCap, src_تھاں, جڑ_تھاں);
        }
    }
    أرجع e;
}
#جے_انت

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlCaps(tcb_t *نشانہ, cte_t *تھاں,
                                        cap_t fh_newCap, cte_t *fh_src_تھاں,
                                        cap_t th_newCap, cte_t *th_src_تھاں,
                                        cap_t c_جڑ_new_Cap, cte_t *c_جڑ_src_تھاں,
                                        cap_t v_جڑ_new_Cap, cte_t *v_جڑ_src_تھاں,
                                        لفظ_t بفر_Addr, cap_t بفر_Cap,
                                        cte_t *بفر_Src_تھاں,
                                        thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((لفظ_t)نشانہ);

    إذا (updateFlags & thread_control_caps_update_fault) {
        e = installTCBCap(نشانہ, tCap, تھاں, tcbFaultHandler, fh_newCap, fh_src_تھاں);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }

    }

    إذا (updateFlags & thread_control_caps_update_timeout) {
        e = installTCBCap(نشانہ, tCap, تھاں, tcbTimeoutHandler, th_newCap, th_src_تھاں);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_caps_update_space) {
        e = installTCBCap(نشانہ, tCap, تھاں, tcbCTable, c_جڑ_new_Cap, c_جڑ_src_تھاں);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }

        e = installTCBCap(نشانہ, tCap, تھاں, tcbVTable, v_جڑ_new_Cap, v_جڑ_src_تھاں);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_caps_update_ipc_بفر) {
        cte_t *بفر_تھاں;

        بفر_تھاں = TCB_PTR_CTE_PTR(نشانہ, tcb_بفر);
        e = cteDelete(بفر_تھاں, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        نشانہ->tcbIPCBuffer = بفر_Addr;

        إذا (بفر_Src_تھاں && sameObjectAs(بفر_Cap, بفر_Src_تھاں->cap) &&
            sameObjectAs(tCap, تھاں->cap)) {
            cte_داخل_کرو(بفر_Cap, بفر_Src_تھاں, بفر_تھاں);
        }

        إذا (نشانہ == گرہ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t invokeTCB_ThreadControl(tcb_t *نشانہ, cte_t *تھاں,
                                    cptr_t faultep, prio_t mcp, prio_t ترجیح,
                                    cap_t c_جڑ_new_Cap, cte_t *c_جڑ_src_تھاں,
                                    cap_t v_جڑ_new_Cap, cte_t *v_جڑ_src_تھاں,
                                    لفظ_t بفر_Addr, cap_t بفر_Cap,
                                    cte_t *بفر_Src_تھاں,
                                    thread_control_flag_t updateFlags)
{
    exception_t e;
    cap_t tCap = cap_thread_cap_new((لفظ_t)نشانہ);

    إذا (updateFlags & thread_control_update_space) {
        نشانہ->tcbFaultHandler = faultep;
    }

    إذا (updateFlags & thread_control_update_mcp) {
        سیٹ_MCPriority(نشانہ, mcp);
    }

    إذا (updateFlags & thread_control_update_space) {
        cte_t *جڑ_تھاں;

        جڑ_تھاں = TCB_PTR_CTE_PTR(نشانہ, tcbCTable);
        e = cteDelete(جڑ_تھاں, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        إذا (sameObjectAs(c_جڑ_new_Cap, c_جڑ_src_تھاں->cap) &&
            sameObjectAs(tCap, تھاں->cap)) {
            cte_داخل_کرو(c_جڑ_new_Cap, c_جڑ_src_تھاں, جڑ_تھاں);
        }
    }

    إذا (updateFlags & thread_control_update_space) {
        cte_t *جڑ_تھاں;

        جڑ_تھاں = TCB_PTR_CTE_PTR(نشانہ, tcbVTable);
        e = cteDelete(جڑ_تھاں, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        إذا (sameObjectAs(v_جڑ_new_Cap, v_جڑ_src_تھاں->cap) &&
            sameObjectAs(tCap, تھاں->cap)) {
            cte_داخل_کرو(v_جڑ_new_Cap, v_جڑ_src_تھاں, جڑ_تھاں);
        }
    }

    إذا (updateFlags & thread_control_update_ipc_بفر) {
        cte_t *بفر_تھاں;

        بفر_تھاں = TCB_PTR_CTE_PTR(نشانہ, tcb_بفر);
        e = cteDelete(بفر_تھاں, true);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
        نشانہ->tcbIPCBuffer = بفر_Addr;

        إذا (بفر_Src_تھاں && sameObjectAs(بفر_Cap, بفر_Src_تھاں->cap) &&
            sameObjectAs(tCap, تھاں->cap)) {
            cte_داخل_کرو(بفر_Cap, بفر_Src_تھاں, بفر_تھاں);
        }

        إذا (نشانہ == گرہ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    إذا (updateFlags & thread_control_update_ترجیح) {
        سیٹ_ترجیح(نشانہ, ترجیح);
    }

    أرجع EXCEPTION_NONE;
}
#جے_انت

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t invokeTCB_ThreadControlSched(tcb_t *نشانہ, cte_t *تھاں,
                                         cap_t fh_newCap, cte_t *fh_src_تھاں,
                                         prio_t mcp, prio_t ترجیح,
                                         sched_context_t *sc,
                                         thread_control_flag_t updateFlags)
{
    إذا (updateFlags & thread_control_sched_update_fault) {
        cap_t tCap = cap_thread_cap_new((لفظ_t)نشانہ);
        exception_t e = installTCBCap(نشانہ, tCap, تھاں, tcbFaultHandler, fh_newCap, fh_src_تھاں);
        إذا (e != EXCEPTION_NONE) {
            أرجع e;
        }
    }

    إذا (updateFlags & thread_control_sched_update_mcp) {
        سیٹ_MCPriority(نشانہ, mcp);
    }

    إذا (updateFlags & thread_control_sched_update_ترجیح) {
        سیٹ_ترجیح(نشانہ, ترجیح);
    }

    إذا (updateFlags & thread_control_sched_update_sc) {
        إذا (sc != عدم) {
            schedContext_bindTCB(sc, نشانہ);
        } وإلا إذا (sc == عدم) {
            schedContext_unbindTCB(نشانہ->tcbSchedContext);
        }
    }

    أرجع EXCEPTION_NONE;
}
#جے_انت

exception_t invokeTCB_CopyRegisters(tcb_t *dest, tcb_t *tcb_src,
                                    bool_t suspend_سروت, bool_t resume_نشانہ,
                                    bool_t transfer_چوکھٹا, bool_t transferInteger,
                                    لفظ_t transferArch)
{
    إذا (suspend_سروت) {
        suspend(tcb_src);
    }

    إذا (resume_نشانہ) {
        restart(dest);
    }

    إذا (transfer_چوکھٹا) {
        لفظ_t i;
        لفظ_t v;
        لفظ_t pc;

        لكل (i = 0; i < n_چوکھٹا_Registers; i++) {
            v = get_رجسٹر(tcb_src, چوکھٹا_Registers[i]);
            سیٹ_رجسٹر(dest, چوکھٹا_Registers[i], v);
        }

        pc = getRestartPC(dest);
        سیٹ_اگلا_PC(dest, pc);
    }

    إذا (transferInteger) {
        لفظ_t i;
        لفظ_t v;

        لكل (i = 0; i < n_gpRegisters; i++) {
            v = get_رجسٹر(tcb_src, gpRegisters[i]);
            سیٹ_رجسٹر(dest, gpRegisters[i], v);
        }
    }

    Arch_postModifyRegisters(dest);

    إذا (dest == گرہ_STATE(ksCurThread)) {
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
exception_t invokeTCB_ReadRegisters(tcb_t *tcb_src, bool_t suspend_سروت,
                                    لفظ_t n, لفظ_t arch, bool_t call)
{
    لفظ_t i, j;
    exception_t e;
    tcb_t *thread;

    thread = گرہ_STATE(ksCurThread);

    إذا (suspend_سروت) {
        suspend(tcb_src);
    }

    e = Arch_performTransfer(arch, tcb_src, گرہ_STATE(ksCurThread));
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    إذا (call) {
        لفظ_t *ipc_بفر;

        ipc_بفر = lookupIPCBuffer(true, thread);

        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);

        لكل (i = 0; i < n && i < n_چوکھٹا_Registers && i < n_پیغام_Registers; i++) {
            سیٹ_رجسٹر(thread, پیغام_Registers[i],
                        get_رجسٹر(tcb_src, چوکھٹا_Registers[i]));
        }

        إذا (ipc_بفر != عدم && i < n && i < n_چوکھٹا_Registers) {
            لكل (; i < n && i < n_چوکھٹا_Registers; i++) {
                ipc_بفر[i + 1] = get_رجسٹر(tcb_src, چوکھٹا_Registers[i]);
            }
        }

        j = i;

        لكل (i = 0; i < n_gpRegisters && i + n_چوکھٹا_Registers < n
             && i + n_چوکھٹا_Registers < n_پیغام_Registers; i++) {
            سیٹ_رجسٹر(thread, پیغام_Registers[i + n_چوکھٹا_Registers],
                        get_رجسٹر(tcb_src, gpRegisters[i]));
        }

        إذا (ipc_بفر != عدم && i < n_gpRegisters
            && i + n_چوکھٹا_Registers < n) {
            لكل (; i < n_gpRegisters && i + n_چوکھٹا_Registers < n; i++) {
                ipc_بفر[i + n_چوکھٹا_Registers + 1] =
                    get_رجسٹر(tcb_src, gpRegisters[i]);
            }
        }

        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                        se_L4_سنیہا_Info_new(0, 0, 0, i + j)));
    }
    سیٹ_Thread_State(thread, ThreadState_Running);

    أرجع EXCEPTION_NONE;
}

exception_t invokeTCB_WriteRegisters(tcb_t *dest, bool_t resume_نشانہ,
                                     لفظ_t n, لفظ_t arch, لفظ_t *بفر)
{
    لفظ_t i;
    لفظ_t pc;
    exception_t e;
    bool_t archInfo;

    e = Arch_performTransfer(arch, گرہ_STATE(ksCurThread), dest);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    إذا (n > n_چوکھٹا_Registers + n_gpRegisters) {
        n = n_چوکھٹا_Registers + n_gpRegisters;
    }

    archInfo = Arch_get_Sanitise_رجسٹر_Info(dest);

    لكل (i = 0; i < n_چوکھٹا_Registers && i < n; i++) {
        /* Offset of 2 to get past the initial syscall arguments */
        سیٹ_رجسٹر(dest, چوکھٹا_Registers[i],
                    sanitise_رجسٹر(چوکھٹا_Registers[i],
                                     getSyscallArg(i + 2, بفر), archInfo));
    }

    لكل (i = 0; i < n_gpRegisters && i + n_چوکھٹا_Registers < n; i++) {
        سیٹ_رجسٹر(dest, gpRegisters[i],
                    sanitise_رجسٹر(gpRegisters[i],
                                     getSyscallArg(i + n_چوکھٹا_Registers + 2,
                                                   بفر), archInfo));
    }

    pc = getRestartPC(dest);
    سیٹ_اگلا_PC(dest, pc);

    Arch_postModifyRegisters(dest);

    إذا (resume_نشانہ) {
        restart(dest);
    }

    إذا (dest == گرہ_STATE(ksCurThread)) {
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

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
فراغ سیٹ_Thread_Name(tcb_t *tcb, ثابت حرف *name)
{
    strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, name, TCB_NAME_لمائی);
}
#جے_انت

لفظ_t سیٹ_MRs_syscall_error(tcb_t *thread, لفظ_t *receiveIPCBuffer)
{
    بدل (current_syscall_error.type) {
    حالة se_L4_Invalid_دلیل:
        أرجع سیٹ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_دلیل_نمبر);

    حالة seL4_InvalidCapability:
        أرجع سیٹ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.invalid_Cap_نمبر);

    حالة seL4_IllegalOperation:
        أرجع 0;

    حالة seL4_RangeError:
        سیٹ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.rangeErrorMin);
        أرجع سیٹ_MR(thread, receiveIPCBuffer, 1,
                     current_syscall_error.rangeErrorMax);

    حالة seL4_AlignmentError:
        أرجع 0;

    حالة seL4_FailedLookup:
        سیٹ_MR(thread, receiveIPCBuffer, 0,
              current_syscall_error.failed_Lookup_Was_سروت ? 1 : 0);
        أرجع سیٹ_MRs_lookup_failure(thread, receiveIPCBuffer,
                                     current_lookup_fault, 1);

    حالة se_L4_Truncated_سنیہا:
    حالة se_L4_Delete_پہلا:
    حالة se_L4_Revoke_پہلا:
        أرجع 0;
    حالة se_L4_Not_Enough_حافظہ:
        أرجع سیٹ_MR(thread, receiveIPCBuffer, 0,
                     current_syscall_error.حافظہ_بایاں);
    افتراضي:
        fail("Invalid syscall error");
    }
}
