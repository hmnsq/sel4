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
#اگر_تعریف_شده CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#پایان_اگر
#ضمن <model/statedata.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/thread.h>
#ضمن <machine/registerset.h>
#ضمن <machine/fpu.h>
#ضمن <linker.h>

ساكن se_L4_پیام_Info_t
transferCaps(se_L4_پیام_Info_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             کلمه_t *receive_میانگیر);

BOOT_کد فراغ configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    مجموعه_Thread_State(tcb, ThreadState_IdleThreadState);
}

فراغ activateThread(فراغ)
{
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    إذا (unlikely(گره_STATE(ksCurThread)->tcb_Yield_مقصد)) {
        sched_Context_complete_Yield_مقصد(گره_STATE(ksCurThread));
        أكد(thread_state_get_tsType(گره_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#پایان_اگر

    بدل (thread_state_get_tsType(گره_STATE(ksCurThread)->tcbState)) {
    حالة ThreadState_Running:
#اگر_تعریف_شده CONFIG_VTX
    حالة ThreadState_RunningVM:
#پایان_اگر
        اكسر;

    حالة ThreadState_Restart: {
        کلمه_t pc;

        pc = getRestartPC(گره_STATE(ksCurThread));
        مجموعه_بعدی_PC(گره_STATE(ksCurThread), pc);
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
        اكسر;
    }

    حالة ThreadState_IdleThreadState:
        Arch_activateIdleThread(گره_STATE(ksCurThread));
        اكسر;

    افتراضي:
        fail("Current thread is blocked");
    }
}

فراغ suspend(tcb_t *هدف)
{
    cancelIPC(هدف);
    إذا (thread_state_get_tsType(هدف->tcbState) == ThreadState_Running) {
        /* whilst in the running state it is possible that restart pc of a thread is
         * incorrect. As we do not know what state this thread will transition to
         * after we make it inactive we update its restart pc so that the thread next
         * runs at the correct address whether it is restarted or moved directly to
         * running */
        updateRestartPC(هدف);
    }
    tcb_Sched_خارج_از_صف(هدف);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    tcbReleaseRemove(هدف);
    sched_Context_cancel_Yield_مقصد(هدف);
#پایان_اگر
    مجموعه_Thread_State(هدف, ThreadState_Inactive);
}

فراغ restart(tcb_t *هدف)
{
    إذا (isStopped(هدف)) {
        cancelIPC(هدف);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        مجموعه_Thread_State(هدف, ThreadState_Restart);
        sched_context_t *sc = هدف->tcbSchedContext;
        إذا (sc_sporadic(sc) && sc != گره_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        إذا (isSchedulable(هدف)) {
            possible_Switch_مقصد(هدف);
        }
#وإلا
        راهاندازی_پاسخ_Master(هدف);
        مجموعه_Thread_State(هدف, ThreadState_Restart);
        SCHED_افزودن_به_صف(هدف);
        possible_Switch_مقصد(هدف);
#پایان_اگر
    }
}

فراغ doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, کلمه_t badge,
                   bool_t grant, tcb_t *receiver)
{
    فراغ *receive_میانگیر, *send_میانگیر;

    receive_میانگیر = lookupIPCBuffer(true, receiver);

    إذا (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        send_میانگیر = lookupIPCBuffer(false, sender);
        do_نرمال_Transfer(sender, send_میانگیر, endpoint, badge, grant,
                         receiver, receive_میانگیر);
    } وإلا {
        doFaultTransfer(badge, sender, receiver, receive_میانگیر);
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ do_پاسخ_Transfer(tcb_t *sender, پاسخ_t *پاسخ, bool_t grant)
#وإلا
فراغ do_پاسخ_Transfer(tcb_t *sender, tcb_t *receiver, cte_t *جایگاه, bool_t grant)
#پایان_اگر
{
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    إذا (پاسخ->پاسخ_TCB == عدم ||
        thread_state_get_tsType(پاسخ->پاسخ_TCB->tcbState) != Thread_State_Blocked_On_پاسخ) {
        /* nothing to do */
        أرجع;
    }

    tcb_t *receiver = پاسخ->پاسخ_TCB;
    پاسخ_remove(پاسخ, receiver);
    أكد(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    أكد(پاسخ->پاسخ_TCB == عدم);

    إذا (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != گره_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#وإلا
    أكد(thread_state_get_tsType(receiver->tcbState) ==
           Thread_State_Blocked_On_پاسخ);
#پایان_اگر

    کلمه_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    إذا (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, عدم, 0, grant, receiver);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        مجموعه_Thread_State(receiver, ThreadState_Running);
#وإلا
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(جایگاه);
        مجموعه_Thread_State(receiver, ThreadState_Running);
        possible_Switch_مقصد(receiver);
#پایان_اگر
    } وإلا {
#اگر_تعریف_نشده CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(جایگاه);
#پایان_اگر
        bool_t restart = handle_Fault_پاسخ(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        إذا (restart) {
            مجموعه_Thread_State(receiver, ThreadState_Restart);
#اگر_تعریف_نشده CONFIG_KERNEL_MCS
            possible_Switch_مقصد(receiver);
#پایان_اگر
        } وإلا {
            مجموعه_Thread_State(receiver, ThreadState_Inactive);
        }
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    إذا (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        إذا ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possible_Switch_مقصد(receiver);
        } وإلا {
            إذا (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } وإلا {
                postpone(sc);
            }
        }
    }
#پایان_اگر
}

فراغ do_نرمال_Transfer(tcb_t *sender, کلمه_t *send_میانگیر, endpoint_t *endpoint,
                      کلمه_t badge, bool_t canGrant, tcb_t *receiver,
                      کلمه_t *receive_میانگیر)
{
    کلمه_t پیام_Transferred;
    se_L4_پیام_Info_t برچسب;
    exception_t وضعیت;

    برچسب = پیام_Info_From_کلمه(get_ثبات(sender, پیام_Info_ثبات));

    إذا (canGrant) {
        وضعیت = lookupExtraCaps(sender, send_میانگیر, برچسب);
        إذا (unlikely(وضعیت != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = عدم;
        }
    } وإلا {
        current_extra_caps.excaprefs[0] = عدم;
    }

    پیام_Transferred = copyMRs(sender, send_میانگیر, receiver, receive_میانگیر,
                             se_L4_پیام_Info_get_درازی(برچسب));

    برچسب = transferCaps(برچسب, endpoint, receiver, receive_میانگیر);

    برچسب = se_L4_پیام_Info_مجموعه_درازی(برچسب, پیام_Transferred);
    مجموعه_ثبات(receiver, پیام_Info_ثبات, کلمه_From_پیام_Info(برچسب));
    مجموعه_ثبات(receiver, badge_ثبات, badge);
}

فراغ doFaultTransfer(کلمه_t badge, tcb_t *sender, tcb_t *receiver,
                     کلمه_t *receiverIPCBuffer)
{
    کلمه_t فرستاده;
    se_L4_پیام_Info_t پیام_Info;

    فرستاده = مجموعه_MRs_fault(sender, receiver, receiverIPCBuffer);
    پیام_Info = se_L4_پیام_Info_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, فرستاده);
    مجموعه_ثبات(receiver, پیام_Info_ثبات, کلمه_From_پیام_Info(پیام_Info));
    مجموعه_ثبات(receiver, badge_ثبات, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ساكن se_L4_پیام_Info_t transferCaps(se_L4_پیام_Info_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       کلمه_t *receive_میانگیر)
{
    کلمه_t i;
    cte_t *dest_جایگاه;

    info = se_L4_پیام_Info_مجموعه_extra_Caps(info, 0);
    info = se_L4_پیام_Info_مجموعه_caps_Unwrapped(info, 0);

    إذا (likely(!current_extra_caps.excaprefs[0] || !receive_میانگیر)) {
        أرجع info;
    }

    dest_جایگاه = getReceiveSlots(receiver, receive_میانگیر);

    لكل (i = 0; i < se_L4_پیام_Max_Extra_Caps && current_extra_caps.excaprefs[i] != عدم; i++) {
        cte_t *جایگاه = current_extra_caps.excaprefs[i];
        cap_t cap = جایگاه->cap;

        إذا (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            مجموعه_Extra_Badge(receive_میانگیر,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = se_L4_پیام_Info_مجموعه_caps_Unwrapped(info,
                                                      se_L4_پیام_Info_get_caps_Unwrapped(info) | (1 << i));

        } وإلا {
            deriveCap_ret_t dc_ret;

            إذا (!dest_جایگاه) {
                اكسر;
            }

            dc_ret = deriveCap(جایگاه, cap);

            إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
                اكسر;
            }
            إذا (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                اكسر;
            }

            cte_درج(dc_ret.cap, جایگاه, dest_جایگاه);

            dest_جایگاه = عدم;
        }
    }

    أرجع se_L4_پیام_Info_مجموعه_extra_Caps(info, i);
}

فراغ doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    مجموعه_ثبات(thread, badge_ثبات, 0);
}

فراغ prepare_مجموعه_Domain(tcb_t *tptr, dom_t dom)
{
    إذا (ksCurDomain != dom) {
        Arch_prepare_مجموعه_Domain(tptr, dom);
#اگر_تعریف_شده CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#پایان_اگر
    }
}

ساكن فراغ prepare_بعدی_Domain(فراغ)
{
    Arch_prepare_بعدی_Domain();
#اگر_تعریف_شده CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(عدم);
#پایان_اگر
}

ساكن فراغ بعدی_Domain(فراغ)
{
    ks_Dom_زمانبندی_Idx++;
    إذا (dschedule_is_انتها_marker(ks_Dom_زمانبندی_Idx)) {
        ks_Dom_زمانبندی_Idx = ks_Dom_زمانبندی_شروع;
    }
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    گره_STATE(ksReprogram) = true;
#پایان_اگر
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ks_Dom_زمانبندی[ks_Dom_زمانبندی_Idx]);
    ks_Domain_زمان = dschedule_duration(ks_Dom_زمانبندی[ks_Dom_زمانبندی_Idx]);
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
ساكن فراغ switchSchedContext(فراغ)
{
    إذا (unlikely(گره_STATE(ksCurSC) != گره_STATE(ksCurThread)->tcbSchedContext)) {
        گره_STATE(ksReprogram) = true;
        إذا (sc_constant_bandwidth(گره_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(گره_STATE(ksCurThread)->tcbSchedContext);
        }

        أكد(refill_ready(گره_STATE(ksCurThread)->tcbSchedContext));
        أكد(refill_sufficient(گره_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    إذا (گره_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commit_زمان();
    }

    گره_STATE(ksCurSC) = گره_STATE(ksCurThread)->tcbSchedContext;
}
#پایان_اگر

ساكن فراغ زمانبندی_انتخاب_New_Thread(فراغ)
{
    إذا (ks_Domain_زمان == 0) {
        prepare_بعدی_Domain();
        بعدی_Domain();
    }
    انتخاب_Thread();
}

فراغ زمانبندی(فراغ)
{
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    awaken();
    check_Domain_زمان();
#پایان_اگر

    إذا (گره_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        إذا (isSchedulable(گره_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_افزودن_به_صف_CURRENT_TCB;
        } وإلا {
            was_runnable = false;
        }

        إذا (گره_STATE(ksSchedulerAction) == Scheduler_Action_انتخاب_New_Thread) {
            زمانبندی_انتخاب_New_Thread();
        } وإلا {
            tcb_t *candidate = گره_STATE(ksSchedulerAction);
            أكد(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                گره_STATE(ksCurThread) == گره_STATE(ksIdleThread)
                || (candidate->tcb_اولویت < گره_STATE(ksCurThread)->tcb_اولویت);
            إذا (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcb_اولویت)) {
                SCHED_افزودن_به_صف(candidate);
                /* we can't, need to reschedule */
                گره_STATE(ksSchedulerAction) = Scheduler_Action_انتخاب_New_Thread;
                زمانبندی_انتخاب_New_Thread();
            } وإلا إذا (was_runnable && candidate->tcb_اولویت == گره_STATE(ksCurThread)->tcb_اولویت) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                گره_STATE(ksSchedulerAction) = Scheduler_Action_انتخاب_New_Thread;
                زمانبندی_انتخاب_New_Thread();
            } وإلا {
                أكد(candidate != گره_STATE(ksCurThread));
                switch_مقصد_Thread(candidate);
            }
        }
    }
    گره_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    do_ماسک_Reschedule(ARCH_گره_STATE(ipiReschedulePending));
    ARCH_گره_STATE(ipiReschedulePending) = 0;
#پایان_اگر /* ENABLE_SMP_SUPPORT */

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    switchSchedContext();

    إذا (گره_STATE(ksReprogram)) {
        مجموعه_بعدی_وقفه();
        گره_STATE(ksReprogram) = false;
    }
#پایان_اگر
}

فراغ انتخاب_Thread(فراغ)
{
    کلمه_t prio;
    کلمه_t dom;
    tcb_t *thread;

    إذا (رقم_Domains > 1) {
        dom = ksCurDomain;
    } وإلا {
        dom = 0;
    }

    إذا (likely(گره_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = گره_STATE(ksReadyQueues)[ready_queues_شاخص(dom, prio)].سر;
        أكد(thread);
        أكد(isSchedulable(thread));
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        أكد(refill_sufficient(thread->tcbSchedContext, 0));
        أكد(refill_ready(thread->tcbSchedContext));
#پایان_اگر
        switch_مقصد_Thread(thread);
    } وإلا {
        switch_مقصد_Idle_Thread();
    }
}

فراغ switch_مقصد_Thread(tcb_t *thread)
{
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    أكد(thread->tcbSchedContext != عدم);
    أكد(!thread_state_get_tcb_درون_Release_صف(thread->tcbState));
    أكد(refill_sufficient(thread->tcbSchedContext, 0));
    أكد(refill_ready(thread->tcbSchedContext));
#پایان_اگر

#اگر_تعریف_شده CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(گره_STATE(ksCurThread), thread);
#پایان_اگر
    Arch_switch_مقصد_Thread(thread);

#اگر_تعریف_شده CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#پایان_اگر /* CONFIG_HAVE_FPU */

    tcb_Sched_خارج_از_صف(thread);
    گره_STATE(ksCurThread) = thread;
}

فراغ switch_مقصد_Idle_Thread(فراغ)
{
#اگر_تعریف_شده CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(گره_STATE(ksCurThread), گره_STATE(ksIdleThread));
#پایان_اگر
    Arch_switch_مقصد_Idle_Thread();
    گره_STATE(ksCurThread) = گره_STATE(ksIdleThread);
}

فراغ مجموعه_Domain(tcb_t *tptr, dom_t dom)
{
    tcb_Sched_خارج_از_صف(tptr);
    tptr->tcbDomain = dom;
    إذا (isSchedulable(tptr)) {
        SCHED_افزودن_به_صف(tptr);
    }
    إذا (tptr == گره_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

فراغ مجموعه_MCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ مجموعه_اولویت(tcb_t *tptr, prio_t prio)
{
    بدل (thread_state_get_tsType(tptr->tcbState)) {
    حالة ThreadState_Running:
    حالة ThreadState_Restart:
        إذا (thread_state_get_tcbQueued(tptr->tcbState) || tptr == گره_STATE(ksCurThread)) {
            tcb_Sched_خارج_از_صف(tptr);
            tptr->tcb_اولویت = prio;
            SCHED_افزودن_به_صف(tptr);
            rescheduleRequired();
        } وإلا {
            tptr->tcb_اولویت = prio;
        }
        اكسر;
    حالة ThreadState_BlockedOnReceive:
    حالة ThreadState_BlockedOnSend:
        tptr->tcb_اولویت = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        اكسر;
    حالة ThreadState_BlockedOnNotification:
        tptr->tcb_اولویت = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        اكسر;
    افتراضي:
        tptr->tcb_اولویت = prio;
        اكسر;
    }
}
#وإلا
فراغ مجموعه_اولویت(tcb_t *tptr, prio_t prio)
{
    tcb_Sched_خارج_از_صف(tptr);
    tptr->tcb_اولویت = prio;
    إذا (isRunnable(tptr)) {
        إذا (tptr == گره_STATE(ksCurThread)) {
            rescheduleRequired();
        } وإلا {
            possible_Switch_مقصد(tptr);
        }
    }
}
#پایان_اگر

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
فراغ possible_Switch_مقصد(tcb_t *هدف)
{
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    إذا (هدف->tcbSchedContext != عدم && !thread_state_get_tcb_درون_Release_صف(هدف->tcbState)) {
#پایان_اگر
        إذا (ksCurDomain != هدف->tcbDomain
            SMP_COND_STATEMENT( || هدف->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_افزودن_به_صف(هدف);
        } وإلا إذا (گره_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_افزودن_به_صف(هدف);
        } وإلا {
            گره_STATE(ksSchedulerAction) = هدف;
        }
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    }
#پایان_اگر

}

فراغ مجموعه_Thread_State(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_مجموعه_ts_Type(&tptr->tcbState, ts);
    زمانبندی_TCB(tptr);
}

فراغ زمانبندی_TCB(tcb_t *tptr)
{
    إذا (tptr == گره_STATE(ksCurThread) &&
        گره_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        گره_STATE(ksSchedulerAction) = Scheduler_Action_انتخاب_New_Thread;
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    أكد(tcb != عدم);

    tcb_Sched_خارج_از_صف(tcb);
    tcb_Release_افزودن_به_صف(tcb);
    گره_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

فراغ مجموعه_بعدی_وقفه(فراغ)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_سر_refill = *refill_سر(گره_STATE(ksCurThread)->tcbSchedContext);
    ticks_t بعدی_وقفه = گره_STATE(ks_Cur_زمان) + ct_سر_refill.rAmount;

    إذا (رقم_Domains > 1) {
        بعدی_وقفه = MIN(بعدی_وقفه, گره_STATE(ks_Cur_زمان) + ks_Domain_زمان);
    }

    tcb_t *rlq_سر = گره_STATE(ks_Release_صف.سر);
    إذا (rlq_سر != عدم) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_سر_refill = *refill_سر(rlq_سر->tcbSchedContext);
        بعدی_وقفه = MIN(rlq_سر_refill.r_زمان, بعدی_وقفه);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    أكد(بعدی_وقفه >= گره_STATE(ks_Cur_زمان));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    مجموعه_Deadline(بعدی_وقفه - getTimerPrecision());
}

فراغ chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    إذا (likely(گره_STATE(ksCurSC) != گره_STATE(ksIdleSC))) {
        إذا (is_دور_Robin(گره_STATE(ksCurSC))) {
            أكد(refill_اندازه(گره_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t سر = *refill_سر(گره_STATE(ksCurSC));
            refill_t دم = *refill_دم(گره_STATE(ksCurSC));
            refill_سر(گره_STATE(ksCurSC))->rAmount = سر.rAmount + دم.rAmount;
            refill_دم(گره_STATE(ksCurSC))->rAmount = 0;
        } وإلا {
            refill_budget_check(consumed);
        }

        أكد(refill_سر(گره_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        گره_STATE(ksCurSC)->scConsumed += consumed;
    }
    گره_STATE(ksConsumed) = 0;
    إذا (likely(isSchedulable(گره_STATE(ksCurThread)))) {
        أكد(گره_STATE(ksCurThread)->tcbSchedContext == گره_STATE(ksCurSC));
        انتها_Timeslice(canTimeoutFault);
        rescheduleRequired();
        گره_STATE(ksReprogram) = true;
    }
}

فراغ انتها_Timeslice(bool_t can_timeout_fault)
{
    bool_t دور_robin = is_دور_Robin(گره_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(گره_STATE(ksCurThread));

    إذا (can_timeout_fault && !دور_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(گره_STATE(ksCurSC)->scBadge);
        handleTimeout(گره_STATE(ksCurThread));
    } وإلا إذا (refill_ready(گره_STATE(ksCurSC)) && refill_sufficient(گره_STATE(ksCurSC), 0)) {
        /* apply round robin */
        أكد(!thread_state_get_tcbQueued(گره_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } وإلا {
        /* postpone until ready */
        postpone(گره_STATE(ksCurSC));
    }
}
#وإلا

فراغ timerTick(فراغ)
{
    إذا (likely(thread_state_get_tsType(گره_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#اگر_تعریف_شده CONFIG_VTX
        || thread_state_get_tsType(گره_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#پایان_اگر
       ) {
        إذا (گره_STATE(ksCurThread)->tcb_زمان_برش > 1) {
            گره_STATE(ksCurThread)->tcb_زمان_برش--;
        } وإلا {
            گره_STATE(ksCurThread)->tcb_زمان_برش = CONFIG_زمان_برش;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    إذا (رقم_Domains > 1) {
        ks_Domain_زمان--;
        إذا (ks_Domain_زمان == 0) {
            rescheduleRequired();
        }
    }
}
#پایان_اگر

فراغ rescheduleRequired(فراغ)
{
    إذا (گره_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && گره_STATE(ksSchedulerAction) != Scheduler_Action_انتخاب_New_Thread
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        && isSchedulable(گره_STATE(ksSchedulerAction))
#پایان_اگر
       ) {
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        أكد(refill_sufficient(گره_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        أكد(refill_ready(گره_STATE(ksSchedulerAction)->tcbSchedContext));
#پایان_اگر
        SCHED_افزودن_به_صف(گره_STATE(ksSchedulerAction));
    }
    گره_STATE(ksSchedulerAction) = Scheduler_Action_انتخاب_New_Thread;
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS

ساكن مضمن bool_t خالص release_q_non_خالی_and_ready(فراغ)
{
    أرجع گره_STATE(ks_Release_صف.سر) != عدم
           && refill_ready(گره_STATE(ks_Release_صف.سر)->tcbSchedContext);
}

ساكن فراغ tcb_Release_خارج_از_صف(فراغ)
{
    أكد(گره_STATE(ks_Release_صف.سر) != عدم);
    أكد(گره_STATE(ks_Release_صف.سر)->tcbSchedPrev == عدم);
    SMP_COND_STATEMENT(أكد(گره_STATE(ks_Release_صف.سر)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = گره_STATE(ks_Release_صف.سر);
    أكد(awakened != گره_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    أكد(!is_دور_Robin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(أكد(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    أكد(refill_sufficient(awakened->tcbSchedContext, 0));
    possible_Switch_مقصد(awakened);
}

فراغ awaken(فراغ)
{
    طالما (unlikely(release_q_non_خالی_and_ready())) {
        tcb_Release_خارج_از_صف();
    }
}
#پایان_اگر
