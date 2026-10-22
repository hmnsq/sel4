/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <object.h>
#شامل <util.h>
#شامل <api/faults.h>
#شامل <api/types.h>
#شامل <kernel/cspace.h>
#شامل <kernel/thread.h>
#شامل <kernel/vspace.h>
#شامل <object/domain.h>
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#شامل <object/schedcontext.h>
#اگر_ختم
#شامل <model/statedata.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/thread.h>
#شامل <machine/registerset.h>
#شامل <machine/fpu.h>
#شامل <linker.h>

ساکن se_L4_پیغام_Info_t
transferCaps(se_L4_پیغام_Info_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             لفظ_t *receive_بفر);

BOOT_رمز خالی configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    سیٹ_Thread_State(tcb, ThreadState_IdleThreadState);
}

خالی activateThread(خالی)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (unlikely(گرہ_STATE(ksCurThread)->tcb_Yield_مقصد)) {
        sched_Context_complete_Yield_مقصد(گرہ_STATE(ksCurThread));
        أكد(thread_state_get_tsType(گرہ_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#اگر_ختم

    انتخاب (thread_state_get_tsType(گرہ_STATE(ksCurThread)->tcbState)) {
    حالت ThreadState_Running:
#اگر_تعریف_شدہ CONFIG_VTX
    حالت ThreadState_RunningVM:
#اگر_ختم
        توڑو;

    حالت ThreadState_Restart: {
        لفظ_t pc;

        pc = getRestartPC(گرہ_STATE(ksCurThread));
        سیٹ_اگلا_PC(گرہ_STATE(ksCurThread), pc);
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
        توڑو;
    }

    حالت ThreadState_IdleThreadState:
        Arch_activateIdleThread(گرہ_STATE(ksCurThread));
        توڑو;

    طےشدہ:
        fail("Current thread is blocked");
    }
}

خالی suspend(tcb_t *ہدف)
{
    cancelIPC(ہدف);
    اگر (thread_state_get_tsType(ہدف->tcbState) == ThreadState_Running) {
        /* whilst in the running state it is possible that restart pc of a thread is
         * incorrect. As we do not know what state this thread will transition to
         * after we make it inactive we update its restart pc so that the thread next
         * runs at the correct address whether it is restarted or moved directly to
         * running */
        updateRestartPC(ہدف);
    }
    tcb_Sched_قطار_سے_نکالو(ہدف);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    tcbReleaseRemove(ہدف);
    sched_Context_cancel_Yield_مقصد(ہدف);
#اگر_ختم
    سیٹ_Thread_State(ہدف, ThreadState_Inactive);
}

خالی restart(tcb_t *ہدف)
{
    اگر (isStopped(ہدف)) {
        cancelIPC(ہدف);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        سیٹ_Thread_State(ہدف, ThreadState_Restart);
        sched_context_t *sc = ہدف->tcbSchedContext;
        اگر (sc_sporadic(sc) && sc != گرہ_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        اگر (isSchedulable(ہدف)) {
            possible_Switch_مقصد(ہدف);
        }
#ورنہ
        تیاری_جواب_Master(ہدف);
        سیٹ_Thread_State(ہدف, ThreadState_Restart);
        SCHED_قطار_میں_ڈالو(ہدف);
        possible_Switch_مقصد(ہدف);
#اگر_ختم
    }
}

خالی doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, لفظ_t badge,
                   bool_t grant, tcb_t *receiver)
{
    خالی *receive_بفر, *send_بفر;

    receive_بفر = lookupIPCBuffer(true, receiver);

    اگر (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        send_بفر = lookupIPCBuffer(false, sender);
        do_معمول_Transfer(sender, send_بفر, endpoint, badge, grant,
                         receiver, receive_بفر);
    } ورنہ {
        doFaultTransfer(badge, sender, receiver, receive_بفر);
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی do_جواب_Transfer(tcb_t *sender, جواب_t *جواب, bool_t grant)
#ورنہ
خالی do_جواب_Transfer(tcb_t *sender, tcb_t *receiver, cte_t *خانہ, bool_t grant)
#اگر_ختم
{
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (جواب->جواب_TCB == تہی ||
        thread_state_get_tsType(جواب->جواب_TCB->tcbState) != Thread_State_Blocked_On_جواب) {
        /* nothing to do */
        واپس;
    }

    tcb_t *receiver = جواب->جواب_TCB;
    جواب_remove(جواب, receiver);
    أكد(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    أكد(جواب->جواب_TCB == تہی);

    اگر (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != گرہ_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#ورنہ
    أكد(thread_state_get_tsType(receiver->tcbState) ==
           Thread_State_Blocked_On_جواب);
#اگر_ختم

    لفظ_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    اگر (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, تہی, 0, grant, receiver);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        سیٹ_Thread_State(receiver, ThreadState_Running);
#ورنہ
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(خانہ);
        سیٹ_Thread_State(receiver, ThreadState_Running);
        possible_Switch_مقصد(receiver);
#اگر_ختم
    } ورنہ {
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(خانہ);
#اگر_ختم
        bool_t restart = handle_Fault_جواب(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        اگر (restart) {
            سیٹ_Thread_State(receiver, ThreadState_Restart);
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
            possible_Switch_مقصد(receiver);
#اگر_ختم
        } ورنہ {
            سیٹ_Thread_State(receiver, ThreadState_Inactive);
        }
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        اگر ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possible_Switch_مقصد(receiver);
        } ورنہ {
            اگر (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } ورنہ {
                postpone(sc);
            }
        }
    }
#اگر_ختم
}

خالی do_معمول_Transfer(tcb_t *sender, لفظ_t *send_بفر, endpoint_t *endpoint,
                      لفظ_t badge, bool_t canGrant, tcb_t *receiver,
                      لفظ_t *receive_بفر)
{
    لفظ_t پیغام_Transferred;
    se_L4_پیغام_Info_t لیبل;
    exception_t حالت_2;

    لیبل = پیغام_Info_From_لفظ(get_رجسٹر(sender, پیغام_Info_رجسٹر));

    اگر (canGrant) {
        حالت_2 = lookupExtraCaps(sender, send_بفر, لیبل);
        اگر (unlikely(حالت_2 != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = تہی;
        }
    } ورنہ {
        current_extra_caps.excaprefs[0] = تہی;
    }

    پیغام_Transferred = copyMRs(sender, send_بفر, receiver, receive_بفر,
                             se_L4_پیغام_Info_get_لمبائی(لیبل));

    لیبل = transferCaps(لیبل, endpoint, receiver, receive_بفر);

    لیبل = se_L4_پیغام_Info_سیٹ_لمبائی(لیبل, پیغام_Transferred);
    سیٹ_رجسٹر(receiver, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(لیبل));
    سیٹ_رجسٹر(receiver, badge_رجسٹر, badge);
}

خالی doFaultTransfer(لفظ_t badge, tcb_t *sender, tcb_t *receiver,
                     لفظ_t *receiverIPCBuffer)
{
    لفظ_t بھیجا;
    se_L4_پیغام_Info_t پیغام_Info;

    بھیجا = سیٹ_MRs_fault(sender, receiver, receiverIPCBuffer);
    پیغام_Info = se_L4_پیغام_Info_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, بھیجا);
    سیٹ_رجسٹر(receiver, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(پیغام_Info));
    سیٹ_رجسٹر(receiver, badge_رجسٹر, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ساکن se_L4_پیغام_Info_t transferCaps(se_L4_پیغام_Info_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       لفظ_t *receive_بفر)
{
    لفظ_t i;
    cte_t *dest_خانہ;

    info = se_L4_پیغام_Info_سیٹ_extra_Caps(info, 0);
    info = se_L4_پیغام_Info_سیٹ_caps_Unwrapped(info, 0);

    اگر (likely(!current_extra_caps.excaprefs[0] || !receive_بفر)) {
        واپس info;
    }

    dest_خانہ = getReceiveSlots(receiver, receive_بفر);

    برائے (i = 0; i < se_L4_پیغام_Max_Extra_Caps && current_extra_caps.excaprefs[i] != تہی; i++) {
        cte_t *خانہ = current_extra_caps.excaprefs[i];
        cap_t cap = خانہ->cap;

        اگر (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            سیٹ_Extra_Badge(receive_بفر,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = se_L4_پیغام_Info_سیٹ_caps_Unwrapped(info,
                                                      se_L4_پیغام_Info_get_caps_Unwrapped(info) | (1 << i));

        } ورنہ {
            deriveCap_ret_t dc_ret;

            اگر (!dest_خانہ) {
                توڑو;
            }

            dc_ret = deriveCap(خانہ, cap);

            اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
                توڑو;
            }
            اگر (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                توڑو;
            }

            cte_داخل_کرو(dc_ret.cap, خانہ, dest_خانہ);

            dest_خانہ = تہی;
        }
    }

    واپس se_L4_پیغام_Info_سیٹ_extra_Caps(info, i);
}

خالی doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
}

خالی prepare_سیٹ_Domain(tcb_t *tptr, dom_t dom)
{
    اگر (ksCurDomain != dom) {
        Arch_prepare_سیٹ_Domain(tptr, dom);
#اگر_تعریف_شدہ CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#اگر_ختم
    }
}

ساکن خالی prepare_اگلا_Domain(خالی)
{
    Arch_prepare_اگلا_Domain();
#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(تہی);
#اگر_ختم
}

ساکن خالی اگلا_Domain(خالی)
{
    ks_Dom_جدول_وقت_Idx++;
    اگر (dschedule_is_انتہا_marker(ks_Dom_جدول_وقت_Idx)) {
        ks_Dom_جدول_وقت_Idx = ks_Dom_جدول_وقت_آغاز;
    }
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ksReprogram) = true;
#اگر_ختم
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ks_Dom_جدول_وقت[ks_Dom_جدول_وقت_Idx]);
    ks_Domain_وقت = dschedule_duration(ks_Dom_جدول_وقت[ks_Dom_جدول_وقت_Idx]);
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن خالی switchSchedContext(خالی)
{
    اگر (unlikely(گرہ_STATE(ksCurSC) != گرہ_STATE(ksCurThread)->tcbSchedContext)) {
        گرہ_STATE(ksReprogram) = true;
        اگر (sc_constant_bandwidth(گرہ_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(گرہ_STATE(ksCurThread)->tcbSchedContext);
        }

        أكد(refill_ready(گرہ_STATE(ksCurThread)->tcbSchedContext));
        أكد(refill_sufficient(گرہ_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    اگر (گرہ_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commit_وقت();
    }

    گرہ_STATE(ksCurSC) = گرہ_STATE(ksCurThread)->tcbSchedContext;
}
#اگر_ختم

ساکن خالی جدول_وقت_انتخاب_New_Thread(خالی)
{
    اگر (ks_Domain_وقت == 0) {
        prepare_اگلا_Domain();
        اگلا_Domain();
    }
    انتخاب_Thread();
}

خالی جدول_وقت(خالی)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    awaken();
    check_Domain_وقت();
#اگر_ختم

    اگر (گرہ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        اگر (isSchedulable(گرہ_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_قطار_میں_ڈالو_CURRENT_TCB;
        } ورنہ {
            was_runnable = false;
        }

        اگر (گرہ_STATE(ksSchedulerAction) == Scheduler_Action_انتخاب_New_Thread) {
            جدول_وقت_انتخاب_New_Thread();
        } ورنہ {
            tcb_t *candidate = گرہ_STATE(ksSchedulerAction);
            أكد(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                گرہ_STATE(ksCurThread) == گرہ_STATE(ksIdleThread)
                || (candidate->tcb_ترجیح < گرہ_STATE(ksCurThread)->tcb_ترجیح);
            اگر (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcb_ترجیح)) {
                SCHED_قطار_میں_ڈالو(candidate);
                /* we can't, need to reschedule */
                گرہ_STATE(ksSchedulerAction) = Scheduler_Action_انتخاب_New_Thread;
                جدول_وقت_انتخاب_New_Thread();
            } ورنہ اگر (was_runnable && candidate->tcb_ترجیح == گرہ_STATE(ksCurThread)->tcb_ترجیح) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                گرہ_STATE(ksSchedulerAction) = Scheduler_Action_انتخاب_New_Thread;
                جدول_وقت_انتخاب_New_Thread();
            } ورنہ {
                أكد(candidate != گرہ_STATE(ksCurThread));
                switch_مقصد_Thread(candidate);
            }
        }
    }
    گرہ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    do_نقاب_Reschedule(ARCH_گرہ_STATE(ipiReschedulePending));
    ARCH_گرہ_STATE(ipiReschedulePending) = 0;
#اگر_ختم /* ENABLE_SMP_SUPPORT */

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    switchSchedContext();

    اگر (گرہ_STATE(ksReprogram)) {
        سیٹ_اگلا_مداخلت();
        گرہ_STATE(ksReprogram) = false;
    }
#اگر_ختم
}

خالی انتخاب_Thread(خالی)
{
    لفظ_t prio;
    لفظ_t dom;
    tcb_t *thread;

    اگر (عدد_Domains > 1) {
        dom = ksCurDomain;
    } ورنہ {
        dom = 0;
    }

    اگر (likely(گرہ_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = گرہ_STATE(ksReadyQueues)[ready_queues_اشاریہ(dom, prio)].سر;
        أكد(thread);
        أكد(isSchedulable(thread));
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        أكد(refill_sufficient(thread->tcbSchedContext, 0));
        أكد(refill_ready(thread->tcbSchedContext));
#اگر_ختم
        switch_مقصد_Thread(thread);
    } ورنہ {
        switch_مقصد_Idle_Thread();
    }
}

خالی switch_مقصد_Thread(tcb_t *thread)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    أكد(thread->tcbSchedContext != تہی);
    أكد(!thread_state_get_tcb_اندر_Release_قطار(thread->tcbState));
    أكد(refill_sufficient(thread->tcbSchedContext, 0));
    أكد(refill_ready(thread->tcbSchedContext));
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(گرہ_STATE(ksCurThread), thread);
#اگر_ختم
    Arch_switch_مقصد_Thread(thread);

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#اگر_ختم /* CONFIG_HAVE_FPU */

    tcb_Sched_قطار_سے_نکالو(thread);
    گرہ_STATE(ksCurThread) = thread;
}

خالی switch_مقصد_Idle_Thread(خالی)
{
#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(گرہ_STATE(ksCurThread), گرہ_STATE(ksIdleThread));
#اگر_ختم
    Arch_switch_مقصد_Idle_Thread();
    گرہ_STATE(ksCurThread) = گرہ_STATE(ksIdleThread);
}

خالی سیٹ_Domain(tcb_t *tptr, dom_t dom)
{
    tcb_Sched_قطار_سے_نکالو(tptr);
    tptr->tcbDomain = dom;
    اگر (isSchedulable(tptr)) {
        SCHED_قطار_میں_ڈالو(tptr);
    }
    اگر (tptr == گرہ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

خالی سیٹ_MCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی سیٹ_ترجیح(tcb_t *tptr, prio_t prio)
{
    انتخاب (thread_state_get_tsType(tptr->tcbState)) {
    حالت ThreadState_Running:
    حالت ThreadState_Restart:
        اگر (thread_state_get_tcbQueued(tptr->tcbState) || tptr == گرہ_STATE(ksCurThread)) {
            tcb_Sched_قطار_سے_نکالو(tptr);
            tptr->tcb_ترجیح = prio;
            SCHED_قطار_میں_ڈالو(tptr);
            rescheduleRequired();
        } ورنہ {
            tptr->tcb_ترجیح = prio;
        }
        توڑو;
    حالت ThreadState_BlockedOnReceive:
    حالت ThreadState_BlockedOnSend:
        tptr->tcb_ترجیح = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        توڑو;
    حالت ThreadState_BlockedOnNotification:
        tptr->tcb_ترجیح = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        توڑو;
    طےشدہ:
        tptr->tcb_ترجیح = prio;
        توڑو;
    }
}
#ورنہ
خالی سیٹ_ترجیح(tcb_t *tptr, prio_t prio)
{
    tcb_Sched_قطار_سے_نکالو(tptr);
    tptr->tcb_ترجیح = prio;
    اگر (isRunnable(tptr)) {
        اگر (tptr == گرہ_STATE(ksCurThread)) {
            rescheduleRequired();
        } ورنہ {
            possible_Switch_مقصد(tptr);
        }
    }
}
#اگر_ختم

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
خالی possible_Switch_مقصد(tcb_t *ہدف)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (ہدف->tcbSchedContext != تہی && !thread_state_get_tcb_اندر_Release_قطار(ہدف->tcbState)) {
#اگر_ختم
        اگر (ksCurDomain != ہدف->tcbDomain
            SMP_COND_STATEMENT( || ہدف->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_قطار_میں_ڈالو(ہدف);
        } ورنہ اگر (گرہ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_قطار_میں_ڈالو(ہدف);
        } ورنہ {
            گرہ_STATE(ksSchedulerAction) = ہدف;
        }
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    }
#اگر_ختم

}

خالی سیٹ_Thread_State(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_سیٹ_ts_Type(&tptr->tcbState, ts);
    جدول_وقت_TCB(tptr);
}

خالی جدول_وقت_TCB(tcb_t *tptr)
{
    اگر (tptr == گرہ_STATE(ksCurThread) &&
        گرہ_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        گرہ_STATE(ksSchedulerAction) = Scheduler_Action_انتخاب_New_Thread;
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    أكد(tcb != تہی);

    tcb_Sched_قطار_سے_نکالو(tcb);
    tcb_Release_قطار_میں_ڈالو(tcb);
    گرہ_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

خالی سیٹ_اگلا_مداخلت(خالی)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_سر_refill = *refill_سر(گرہ_STATE(ksCurThread)->tcbSchedContext);
    ticks_t اگلا_مداخلت = گرہ_STATE(ks_Cur_وقت) + ct_سر_refill.rAmount;

    اگر (عدد_Domains > 1) {
        اگلا_مداخلت = MIN(اگلا_مداخلت, گرہ_STATE(ks_Cur_وقت) + ks_Domain_وقت);
    }

    tcb_t *rlq_سر = گرہ_STATE(ks_Release_قطار.سر);
    اگر (rlq_سر != تہی) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_سر_refill = *refill_سر(rlq_سر->tcbSchedContext);
        اگلا_مداخلت = MIN(rlq_سر_refill.r_وقت, اگلا_مداخلت);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    أكد(اگلا_مداخلت >= گرہ_STATE(ks_Cur_وقت));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    سیٹ_Deadline(اگلا_مداخلت - getTimerPrecision());
}

خالی chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    اگر (likely(گرہ_STATE(ksCurSC) != گرہ_STATE(ksIdleSC))) {
        اگر (is_باری_Robin(گرہ_STATE(ksCurSC))) {
            أكد(refill_سائز(گرہ_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t سر = *refill_سر(گرہ_STATE(ksCurSC));
            refill_t دم = *refill_دم(گرہ_STATE(ksCurSC));
            refill_سر(گرہ_STATE(ksCurSC))->rAmount = سر.rAmount + دم.rAmount;
            refill_دم(گرہ_STATE(ksCurSC))->rAmount = 0;
        } ورنہ {
            refill_budget_check(consumed);
        }

        أكد(refill_سر(گرہ_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        گرہ_STATE(ksCurSC)->scConsumed += consumed;
    }
    گرہ_STATE(ksConsumed) = 0;
    اگر (likely(isSchedulable(گرہ_STATE(ksCurThread)))) {
        أكد(گرہ_STATE(ksCurThread)->tcbSchedContext == گرہ_STATE(ksCurSC));
        انتہا_Timeslice(canTimeoutFault);
        rescheduleRequired();
        گرہ_STATE(ksReprogram) = true;
    }
}

خالی انتہا_Timeslice(bool_t can_timeout_fault)
{
    bool_t باری_robin = is_باری_Robin(گرہ_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(گرہ_STATE(ksCurThread));

    اگر (can_timeout_fault && !باری_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(گرہ_STATE(ksCurSC)->scBadge);
        handleTimeout(گرہ_STATE(ksCurThread));
    } ورنہ اگر (refill_ready(گرہ_STATE(ksCurSC)) && refill_sufficient(گرہ_STATE(ksCurSC), 0)) {
        /* apply round robin */
        أكد(!thread_state_get_tcbQueued(گرہ_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } ورنہ {
        /* postpone until ready */
        postpone(گرہ_STATE(ksCurSC));
    }
}
#ورنہ

خالی timerTick(خالی)
{
    اگر (likely(thread_state_get_tsType(گرہ_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#اگر_تعریف_شدہ CONFIG_VTX
        || thread_state_get_tsType(گرہ_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#اگر_ختم
       ) {
        اگر (گرہ_STATE(ksCurThread)->tcb_وقت_قاش > 1) {
            گرہ_STATE(ksCurThread)->tcb_وقت_قاش--;
        } ورنہ {
            گرہ_STATE(ksCurThread)->tcb_وقت_قاش = CONFIG_وقت_قاش;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    اگر (عدد_Domains > 1) {
        ks_Domain_وقت--;
        اگر (ks_Domain_وقت == 0) {
            rescheduleRequired();
        }
    }
}
#اگر_ختم

خالی rescheduleRequired(خالی)
{
    اگر (گرہ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && گرہ_STATE(ksSchedulerAction) != Scheduler_Action_انتخاب_New_Thread
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        && isSchedulable(گرہ_STATE(ksSchedulerAction))
#اگر_ختم
       ) {
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        أكد(refill_sufficient(گرہ_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        أكد(refill_ready(گرہ_STATE(ksSchedulerAction)->tcbSchedContext));
#اگر_ختم
        SCHED_قطار_میں_ڈالو(گرہ_STATE(ksSchedulerAction));
    }
    گرہ_STATE(ksSchedulerAction) = Scheduler_Action_انتخاب_New_Thread;
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS

ساکن اندرونی bool_t خالص release_q_non_خالی_خانے_and_ready(خالی)
{
    واپس گرہ_STATE(ks_Release_قطار.سر) != تہی
           && refill_ready(گرہ_STATE(ks_Release_قطار.سر)->tcbSchedContext);
}

ساکن خالی tcb_Release_قطار_سے_نکالو(خالی)
{
    أكد(گرہ_STATE(ks_Release_قطار.سر) != تہی);
    أكد(گرہ_STATE(ks_Release_قطار.سر)->tcb_Sched_پچھلا == تہی);
    SMP_COND_STATEMENT(أكد(گرہ_STATE(ks_Release_قطار.سر)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = گرہ_STATE(ks_Release_قطار.سر);
    أكد(awakened != گرہ_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    أكد(!is_باری_Robin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(أكد(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    أكد(refill_sufficient(awakened->tcbSchedContext, 0));
    possible_Switch_مقصد(awakened);
}

خالی awaken(خالی)
{
    جب (unlikely(release_q_non_خالی_خانے_and_ready())) {
        tcb_Release_قطار_سے_نکالو();
    }
}
#اگر_ختم
