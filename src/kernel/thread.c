/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <object.h>
#ضمن <util.h>
#ضمن <api/faults.h>
#ضمن <api/types.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <object/domain.h>
#که_تعریف_شوی CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#د_که_پای
#ضمن <model/statedata.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/thread.h>
#ضمن <machine/registerset.h>
#ضمن <machine/fpu.h>
#ضمن <linker.h>

ساكن se_L4_پیغام_Info_t
transferCaps(se_L4_پیغام_Info_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             کلمه_t *receive_بفر);

BOOT_کوډ فراغ configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    سېټ_Thread_State(tcb, ThreadState_IdleThreadState);
}

فراغ activateThread(فراغ)
{
#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (unlikely(غوټه_STATE(ksCurThread)->tcb_Yield_منزل)) {
        sched_Context_complete_Yield_منزل(غوټه_STATE(ksCurThread));
        أكد(thread_state_get_tsType(غوټه_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#د_که_پای

    بدل (thread_state_get_tsType(غوټه_STATE(ksCurThread)->tcbState)) {
    حالة ThreadState_Running:
#که_تعریف_شوی CONFIG_VTX
    حالة ThreadState_RunningVM:
#د_که_پای
        اكسر;

    حالة ThreadState_Restart: {
        کلمه_t pc;

        pc = getRestartPC(غوټه_STATE(ksCurThread));
        سېټ_بل_PC(غوټه_STATE(ksCurThread), pc);
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
        اكسر;
    }

    حالة ThreadState_IdleThreadState:
        Arch_activateIdleThread(غوټه_STATE(ksCurThread));
        اكسر;

    افتراضي:
        fail("Current thread is blocked");
    }
}

فراغ suspend(tcb_t *موخه)
{
    cancelIPC(موخه);
    إذا (thread_state_get_tsType(موخه->tcbState) == ThreadState_Running) {
        /* whilst in the running state it is possible that restart pc of a thread is
         * incorrect. As we do not know what state this thread will transition to
         * after we make it inactive we update its restart pc so that the thread next
         * runs at the correct address whether it is restarted or moved directly to
         * running */
        updateRestartPC(موخه);
    }
    tcb_Sched_له_کتار_ایستل(موخه);
#که_تعریف_شوی CONFIG_KERNEL_MCS
    tcbReleaseRemove(موخه);
    sched_Context_cancel_Yield_منزل(موخه);
#د_که_پای
    سېټ_Thread_State(موخه, ThreadState_Inactive);
}

فراغ restart(tcb_t *موخه)
{
    إذا (isStopped(موخه)) {
        cancelIPC(موخه);
#که_تعریف_شوی CONFIG_KERNEL_MCS
        سېټ_Thread_State(موخه, ThreadState_Restart);
        sched_context_t *sc = موخه->tcbSchedContext;
        إذا (sc_sporadic(sc) && sc != غوټه_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        إذا (isSchedulable(موخه)) {
            possible_Switch_منزل(موخه);
        }
#وإلا
        تیاری_ځواب_Master(موخه);
        سېټ_Thread_State(موخه, ThreadState_Restart);
        SCHED_کتار_ته_ورزیاتول(موخه);
        possible_Switch_منزل(موخه);
#د_که_پای
    }
}

فراغ doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, کلمه_t badge,
                   bool_t grant, tcb_t *receiver)
{
    فراغ *receive_بفر, *send_بفر;

    receive_بفر = lookupIPCBuffer(true, receiver);

    إذا (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        send_بفر = lookupIPCBuffer(false, sender);
        do_نورمال_Transfer(sender, send_بفر, endpoint, badge, grant,
                         receiver, receive_بفر);
    } وإلا {
        doFaultTransfer(badge, sender, receiver, receive_بفر);
    }
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ do_ځواب_Transfer(tcb_t *sender, ځواب_t *ځواب, bool_t grant)
#وإلا
فراغ do_ځواب_Transfer(tcb_t *sender, tcb_t *receiver, cte_t *خونه, bool_t grant)
#د_که_پای
{
#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (ځواب->ځواب_TCB == عدم ||
        thread_state_get_tsType(ځواب->ځواب_TCB->tcbState) != Thread_State_Blocked_On_ځواب) {
        /* nothing to do */
        أرجع;
    }

    tcb_t *receiver = ځواب->ځواب_TCB;
    ځواب_remove(ځواب, receiver);
    أكد(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    أكد(ځواب->ځواب_TCB == عدم);

    إذا (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != غوټه_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#وإلا
    أكد(thread_state_get_tsType(receiver->tcbState) ==
           Thread_State_Blocked_On_ځواب);
#د_که_پای

    کلمه_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    إذا (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, عدم, 0, grant, receiver);
#که_تعریف_شوی CONFIG_KERNEL_MCS
        سېټ_Thread_State(receiver, ThreadState_Running);
#وإلا
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(خونه);
        سېټ_Thread_State(receiver, ThreadState_Running);
        possible_Switch_منزل(receiver);
#د_که_پای
    } وإلا {
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(خونه);
#د_که_پای
        bool_t restart = handle_Fault_ځواب(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        إذا (restart) {
            سېټ_Thread_State(receiver, ThreadState_Restart);
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
            possible_Switch_منزل(receiver);
#د_که_پای
        } وإلا {
            سېټ_Thread_State(receiver, ThreadState_Inactive);
        }
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        إذا ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possible_Switch_منزل(receiver);
        } وإلا {
            إذا (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } وإلا {
                postpone(sc);
            }
        }
    }
#د_که_پای
}

فراغ do_نورمال_Transfer(tcb_t *sender, کلمه_t *send_بفر, endpoint_t *endpoint,
                      کلمه_t badge, bool_t canGrant, tcb_t *receiver,
                      کلمه_t *receive_بفر)
{
    کلمه_t پیغام_Transferred;
    se_L4_پیغام_Info_t نښان;
    exception_t حالت;

    نښان = پیغام_Info_From_کلمه(get_ثبتوونکی(sender, پیغام_Info_ثبتوونکی));

    إذا (canGrant) {
        حالت = lookupExtraCaps(sender, send_بفر, نښان);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = عدم;
        }
    } وإلا {
        current_extra_caps.excaprefs[0] = عدم;
    }

    پیغام_Transferred = copyMRs(sender, send_بفر, receiver, receive_بفر,
                             se_L4_پیغام_Info_get_اوږدتیا(نښان));

    نښان = transferCaps(نښان, endpoint, receiver, receive_بفر);

    نښان = se_L4_پیغام_Info_سېټ_اوږدتیا(نښان, پیغام_Transferred);
    سېټ_ثبتوونکی(receiver, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(نښان));
    سېټ_ثبتوونکی(receiver, badge_ثبتوونکی, badge);
}

فراغ doFaultTransfer(کلمه_t badge, tcb_t *sender, tcb_t *receiver,
                     کلمه_t *receiverIPCBuffer)
{
    کلمه_t لېږل_شوی;
    se_L4_پیغام_Info_t پیغام_Info;

    لېږل_شوی = سېټ_MRs_fault(sender, receiver, receiverIPCBuffer);
    پیغام_Info = se_L4_پیغام_Info_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, لېږل_شوی);
    سېټ_ثبتوونکی(receiver, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(پیغام_Info));
    سېټ_ثبتوونکی(receiver, badge_ثبتوونکی, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ساكن se_L4_پیغام_Info_t transferCaps(se_L4_پیغام_Info_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       کلمه_t *receive_بفر)
{
    کلمه_t i;
    cte_t *dest_خونه;

    info = se_L4_پیغام_Info_سېټ_extra_Caps(info, 0);
    info = se_L4_پیغام_Info_سېټ_caps_Unwrapped(info, 0);

    إذا (likely(!current_extra_caps.excaprefs[0] || !receive_بفر)) {
        أرجع info;
    }

    dest_خونه = getReceiveSlots(receiver, receive_بفر);

    لكل (i = 0; i < se_L4_پیغام_Max_Extra_Caps && current_extra_caps.excaprefs[i] != عدم; i++) {
        cte_t *خونه = current_extra_caps.excaprefs[i];
        cap_t cap = خونه->cap;

        إذا (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            سېټ_Extra_Badge(receive_بفر,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = se_L4_پیغام_Info_سېټ_caps_Unwrapped(info,
                                                      se_L4_پیغام_Info_get_caps_Unwrapped(info) | (1 << i));

        } وإلا {
            deriveCap_ret_t dc_ret;

            إذا (!dest_خونه) {
                اكسر;
            }

            dc_ret = deriveCap(خونه, cap);

            إذا (dc_ret.حالت != EXCEPTION_NONE) {
                اكسر;
            }
            إذا (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                اكسر;
            }

            cte_ورننباسه(dc_ret.cap, خونه, dest_خونه);

            dest_خونه = عدم;
        }
    }

    أرجع se_L4_پیغام_Info_سېټ_extra_Caps(info, i);
}

فراغ doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
}

فراغ prepare_سېټ_Domain(tcb_t *tptr, dom_t dom)
{
    إذا (ksCurDomain != dom) {
        Arch_prepare_سېټ_Domain(tptr, dom);
#که_تعریف_شوی CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#د_که_پای
    }
}

ساكن فراغ prepare_بل_Domain(فراغ)
{
    Arch_prepare_بل_Domain();
#که_تعریف_شوی CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(عدم);
#د_که_پای
}

ساكن فراغ بل_Domain(فراغ)
{
    ks_Dom_مهالوېش_Idx++;
    إذا (dschedule_is_پای_ټکی_marker(ks_Dom_مهالوېش_Idx)) {
        ks_Dom_مهالوېش_Idx = ks_Dom_مهالوېش_پیل;
    }
#که_تعریف_شوی CONFIG_KERNEL_MCS
    غوټه_STATE(ksReprogram) = true;
#د_که_پای
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ks_Dom_مهالوېش[ks_Dom_مهالوېش_Idx]);
    ks_Domain_وخت = dschedule_duration(ks_Dom_مهالوېش[ks_Dom_مهالوېش_Idx]);
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
ساكن فراغ switchSchedContext(فراغ)
{
    إذا (unlikely(غوټه_STATE(ksCurSC) != غوټه_STATE(ksCurThread)->tcbSchedContext)) {
        غوټه_STATE(ksReprogram) = true;
        إذا (sc_constant_bandwidth(غوټه_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(غوټه_STATE(ksCurThread)->tcbSchedContext);
        }

        أكد(refill_ready(غوټه_STATE(ksCurThread)->tcbSchedContext));
        أكد(refill_sufficient(غوټه_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    إذا (غوټه_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commit_وخت();
    }

    غوټه_STATE(ksCurSC) = غوټه_STATE(ksCurThread)->tcbSchedContext;
}
#د_که_پای

ساكن فراغ مهالوېش_ټاکنه_New_Thread(فراغ)
{
    إذا (ks_Domain_وخت == 0) {
        prepare_بل_Domain();
        بل_Domain();
    }
    ټاکنه_Thread();
}

فراغ مهالوېش(فراغ)
{
#که_تعریف_شوی CONFIG_KERNEL_MCS
    awaken();
    check_Domain_وخت();
#د_که_پای

    إذا (غوټه_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        إذا (isSchedulable(غوټه_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_کتار_ته_ورزیاتول_CURRENT_TCB;
        } وإلا {
            was_runnable = false;
        }

        إذا (غوټه_STATE(ksSchedulerAction) == Scheduler_Action_ټاکنه_New_Thread) {
            مهالوېش_ټاکنه_New_Thread();
        } وإلا {
            tcb_t *candidate = غوټه_STATE(ksSchedulerAction);
            أكد(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                غوټه_STATE(ksCurThread) == غوټه_STATE(ksIdleThread)
                || (candidate->tcb_لومړیتوب < غوټه_STATE(ksCurThread)->tcb_لومړیتوب);
            إذا (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcb_لومړیتوب)) {
                SCHED_کتار_ته_ورزیاتول(candidate);
                /* we can't, need to reschedule */
                غوټه_STATE(ksSchedulerAction) = Scheduler_Action_ټاکنه_New_Thread;
                مهالوېش_ټاکنه_New_Thread();
            } وإلا إذا (was_runnable && candidate->tcb_لومړیتوب == غوټه_STATE(ksCurThread)->tcb_لومړیتوب) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                غوټه_STATE(ksSchedulerAction) = Scheduler_Action_ټاکنه_New_Thread;
                مهالوېش_ټاکنه_New_Thread();
            } وإلا {
                أكد(candidate != غوټه_STATE(ksCurThread));
                switch_منزل_Thread(candidate);
            }
        }
    }
    غوټه_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    do_ماسک_Reschedule(ARCH_غوټه_STATE(ipiReschedulePending));
    ARCH_غوټه_STATE(ipiReschedulePending) = 0;
#د_که_پای /* ENABLE_SMP_SUPPORT */

#که_تعریف_شوی CONFIG_KERNEL_MCS
    switchSchedContext();

    إذا (غوټه_STATE(ksReprogram)) {
        سېټ_بل_مداخله();
        غوټه_STATE(ksReprogram) = false;
    }
#د_که_پای
}

فراغ ټاکنه_Thread(فراغ)
{
    کلمه_t prio;
    کلمه_t dom;
    tcb_t *thread;

    إذا (عدد_Domains > 1) {
        dom = ksCurDomain;
    } وإلا {
        dom = 0;
    }

    إذا (likely(غوټه_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = غوټه_STATE(ksReadyQueues)[ready_queues_شاخص(dom, prio)].سر;
        أكد(thread);
        أكد(isSchedulable(thread));
#که_تعریف_شوی CONFIG_KERNEL_MCS
        أكد(refill_sufficient(thread->tcbSchedContext, 0));
        أكد(refill_ready(thread->tcbSchedContext));
#د_که_پای
        switch_منزل_Thread(thread);
    } وإلا {
        switch_منزل_Idle_Thread();
    }
}

فراغ switch_منزل_Thread(tcb_t *thread)
{
#که_تعریف_شوی CONFIG_KERNEL_MCS
    أكد(thread->tcbSchedContext != عدم);
    أكد(!thread_state_get_tcb_دننه_Release_کتار(thread->tcbState));
    أكد(refill_sufficient(thread->tcbSchedContext, 0));
    أكد(refill_ready(thread->tcbSchedContext));
#د_که_پای

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(غوټه_STATE(ksCurThread), thread);
#د_که_پای
    Arch_switch_منزل_Thread(thread);

#که_تعریف_شوی CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#د_که_پای /* CONFIG_HAVE_FPU */

    tcb_Sched_له_کتار_ایستل(thread);
    غوټه_STATE(ksCurThread) = thread;
}

فراغ switch_منزل_Idle_Thread(فراغ)
{
#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(غوټه_STATE(ksCurThread), غوټه_STATE(ksIdleThread));
#د_که_پای
    Arch_switch_منزل_Idle_Thread();
    غوټه_STATE(ksCurThread) = غوټه_STATE(ksIdleThread);
}

فراغ سېټ_Domain(tcb_t *tptr, dom_t dom)
{
    tcb_Sched_له_کتار_ایستل(tptr);
    tptr->tcbDomain = dom;
    إذا (isSchedulable(tptr)) {
        SCHED_کتار_ته_ورزیاتول(tptr);
    }
    إذا (tptr == غوټه_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

فراغ سېټ_MCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ سېټ_لومړیتوب(tcb_t *tptr, prio_t prio)
{
    بدل (thread_state_get_tsType(tptr->tcbState)) {
    حالة ThreadState_Running:
    حالة ThreadState_Restart:
        إذا (thread_state_get_tcbQueued(tptr->tcbState) || tptr == غوټه_STATE(ksCurThread)) {
            tcb_Sched_له_کتار_ایستل(tptr);
            tptr->tcb_لومړیتوب = prio;
            SCHED_کتار_ته_ورزیاتول(tptr);
            rescheduleRequired();
        } وإلا {
            tptr->tcb_لومړیتوب = prio;
        }
        اكسر;
    حالة ThreadState_BlockedOnReceive:
    حالة ThreadState_BlockedOnSend:
        tptr->tcb_لومړیتوب = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        اكسر;
    حالة ThreadState_BlockedOnNotification:
        tptr->tcb_لومړیتوب = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        اكسر;
    افتراضي:
        tptr->tcb_لومړیتوب = prio;
        اكسر;
    }
}
#وإلا
فراغ سېټ_لومړیتوب(tcb_t *tptr, prio_t prio)
{
    tcb_Sched_له_کتار_ایستل(tptr);
    tptr->tcb_لومړیتوب = prio;
    إذا (isRunnable(tptr)) {
        إذا (tptr == غوټه_STATE(ksCurThread)) {
            rescheduleRequired();
        } وإلا {
            possible_Switch_منزل(tptr);
        }
    }
}
#د_که_پای

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
فراغ possible_Switch_منزل(tcb_t *موخه)
{
#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (موخه->tcbSchedContext != عدم && !thread_state_get_tcb_دننه_Release_کتار(موخه->tcbState)) {
#د_که_پای
        إذا (ksCurDomain != موخه->tcbDomain
            SMP_COND_STATEMENT( || موخه->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_کتار_ته_ورزیاتول(موخه);
        } وإلا إذا (غوټه_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_کتار_ته_ورزیاتول(موخه);
        } وإلا {
            غوټه_STATE(ksSchedulerAction) = موخه;
        }
#که_تعریف_شوی CONFIG_KERNEL_MCS
    }
#د_که_پای

}

فراغ سېټ_Thread_State(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_سېټ_ts_Type(&tptr->tcbState, ts);
    مهالوېش_TCB(tptr);
}

فراغ مهالوېش_TCB(tcb_t *tptr)
{
    إذا (tptr == غوټه_STATE(ksCurThread) &&
        غوټه_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        غوټه_STATE(ksSchedulerAction) = Scheduler_Action_ټاکنه_New_Thread;
    }
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    أكد(tcb != عدم);

    tcb_Sched_له_کتار_ایستل(tcb);
    tcb_Release_کتار_ته_ورزیاتول(tcb);
    غوټه_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

فراغ سېټ_بل_مداخله(فراغ)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_سر_refill = *refill_سر(غوټه_STATE(ksCurThread)->tcbSchedContext);
    ticks_t بل_مداخله = غوټه_STATE(ks_Cur_وخت) + ct_سر_refill.rAmount;

    إذا (عدد_Domains > 1) {
        بل_مداخله = MIN(بل_مداخله, غوټه_STATE(ks_Cur_وخت) + ks_Domain_وخت);
    }

    tcb_t *rlq_سر = غوټه_STATE(ks_Release_کتار.سر);
    إذا (rlq_سر != عدم) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_سر_refill = *refill_سر(rlq_سر->tcbSchedContext);
        بل_مداخله = MIN(rlq_سر_refill.r_وخت, بل_مداخله);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    أكد(بل_مداخله >= غوټه_STATE(ks_Cur_وخت));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    سېټ_Deadline(بل_مداخله - getTimerPrecision());
}

فراغ chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    إذا (likely(غوټه_STATE(ksCurSC) != غوټه_STATE(ksIdleSC))) {
        إذا (is_پړاو_Robin(غوټه_STATE(ksCurSC))) {
            أكد(refill_اندازه(غوټه_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t سر = *refill_سر(غوټه_STATE(ksCurSC));
            refill_t لکۍ = *refill_لکۍ(غوټه_STATE(ksCurSC));
            refill_سر(غوټه_STATE(ksCurSC))->rAmount = سر.rAmount + لکۍ.rAmount;
            refill_لکۍ(غوټه_STATE(ksCurSC))->rAmount = 0;
        } وإلا {
            refill_budget_check(consumed);
        }

        أكد(refill_سر(غوټه_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        غوټه_STATE(ksCurSC)->scConsumed += consumed;
    }
    غوټه_STATE(ksConsumed) = 0;
    إذا (likely(isSchedulable(غوټه_STATE(ksCurThread)))) {
        أكد(غوټه_STATE(ksCurThread)->tcbSchedContext == غوټه_STATE(ksCurSC));
        پای_ټکی_Timeslice(canTimeoutFault);
        rescheduleRequired();
        غوټه_STATE(ksReprogram) = true;
    }
}

فراغ پای_ټکی_Timeslice(bool_t can_timeout_fault)
{
    bool_t پړاو_robin = is_پړاو_Robin(غوټه_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(غوټه_STATE(ksCurThread));

    إذا (can_timeout_fault && !پړاو_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(غوټه_STATE(ksCurSC)->scBadge);
        handleTimeout(غوټه_STATE(ksCurThread));
    } وإلا إذا (refill_ready(غوټه_STATE(ksCurSC)) && refill_sufficient(غوټه_STATE(ksCurSC), 0)) {
        /* apply round robin */
        أكد(!thread_state_get_tcbQueued(غوټه_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } وإلا {
        /* postpone until ready */
        postpone(غوټه_STATE(ksCurSC));
    }
}
#وإلا

فراغ timerTick(فراغ)
{
    إذا (likely(thread_state_get_tsType(غوټه_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#که_تعریف_شوی CONFIG_VTX
        || thread_state_get_tsType(غوټه_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#د_که_پای
       ) {
        إذا (غوټه_STATE(ksCurThread)->tcb_وخت_ټوټه > 1) {
            غوټه_STATE(ksCurThread)->tcb_وخت_ټوټه--;
        } وإلا {
            غوټه_STATE(ksCurThread)->tcb_وخت_ټوټه = CONFIG_وخت_ټوټه;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    إذا (عدد_Domains > 1) {
        ks_Domain_وخت--;
        إذا (ks_Domain_وخت == 0) {
            rescheduleRequired();
        }
    }
}
#د_که_پای

فراغ rescheduleRequired(فراغ)
{
    إذا (غوټه_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && غوټه_STATE(ksSchedulerAction) != Scheduler_Action_ټاکنه_New_Thread
#که_تعریف_شوی CONFIG_KERNEL_MCS
        && isSchedulable(غوټه_STATE(ksSchedulerAction))
#د_که_پای
       ) {
#که_تعریف_شوی CONFIG_KERNEL_MCS
        أكد(refill_sufficient(غوټه_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        أكد(refill_ready(غوټه_STATE(ksSchedulerAction)->tcbSchedContext));
#د_که_پای
        SCHED_کتار_ته_ورزیاتول(غوټه_STATE(ksSchedulerAction));
    }
    غوټه_STATE(ksSchedulerAction) = Scheduler_Action_ټاکنه_New_Thread;
}

#که_تعریف_شوی CONFIG_KERNEL_MCS

ساكن مضمن bool_t خالص release_q_non_تشه_and_ready(فراغ)
{
    أرجع غوټه_STATE(ks_Release_کتار.سر) != عدم
           && refill_ready(غوټه_STATE(ks_Release_کتار.سر)->tcbSchedContext);
}

ساكن فراغ tcb_Release_له_کتار_ایستل(فراغ)
{
    أكد(غوټه_STATE(ks_Release_کتار.سر) != عدم);
    أكد(غوټه_STATE(ks_Release_کتار.سر)->tcbSchedPrev == عدم);
    SMP_COND_STATEMENT(أكد(غوټه_STATE(ks_Release_کتار.سر)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = غوټه_STATE(ks_Release_کتار.سر);
    أكد(awakened != غوټه_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    أكد(!is_پړاو_Robin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(أكد(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    أكد(refill_sufficient(awakened->tcbSchedContext, 0));
    possible_Switch_منزل(awakened);
}

فراغ awaken(فراغ)
{
    طالما (unlikely(release_q_non_تشه_and_ready())) {
        tcb_Release_له_کتار_ایستل();
    }
}
#د_که_پای
