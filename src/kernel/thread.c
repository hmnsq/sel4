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
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#جے_انت
#ضمن <model/statedata.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/thread.h>
#ضمن <machine/registerset.h>
#ضمن <machine/fpu.h>
#ضمن <linker.h>

ساكن se_L4_سنیہا_Info_t
transferCaps(se_L4_سنیہا_Info_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             لفظ_t *receive_بفر);

BOOT_کوڈ فراغ configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    سیٹ_Thread_State(tcb, ThreadState_IdleThreadState);
}

فراغ activateThread(فراغ)
{
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    إذا (unlikely(گرہ_STATE(ksCurThread)->tcb_Yield_منزل)) {
        sched_Context_complete_Yield_منزل(گرہ_STATE(ksCurThread));
        أكد(thread_state_get_tsType(گرہ_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#جے_انت

    بدل (thread_state_get_tsType(گرہ_STATE(ksCurThread)->tcbState)) {
    حالة ThreadState_Running:
#جے_تعریف_شدہ CONFIG_VTX
    حالة ThreadState_RunningVM:
#جے_انت
        اكسر;

    حالة ThreadState_Restart: {
        لفظ_t pc;

        pc = getRestartPC(گرہ_STATE(ksCurThread));
        سیٹ_اگلا_PC(گرہ_STATE(ksCurThread), pc);
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
        اكسر;
    }

    حالة ThreadState_IdleThreadState:
        Arch_activateIdleThread(گرہ_STATE(ksCurThread));
        اكسر;

    افتراضي:
        fail("Current thread is blocked");
    }
}

فراغ suspend(tcb_t *نشانہ)
{
    cancelIPC(نشانہ);
    إذا (thread_state_get_tsType(نشانہ->tcbState) == ThreadState_Running) {
        /* whilst in the running state it is possible that restart pc of a thread is
         * incorrect. As we do not know what state this thread will transition to
         * after we make it inactive we update its restart pc so that the thread next
         * runs at the correct address whether it is restarted or moved directly to
         * running */
        updateRestartPC(نشانہ);
    }
    tcb_Sched_قطار_توں_کڈھو(نشانہ);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    tcbReleaseRemove(نشانہ);
    sched_Context_cancel_Yield_منزل(نشانہ);
#جے_انت
    سیٹ_Thread_State(نشانہ, ThreadState_Inactive);
}

فراغ restart(tcb_t *نشانہ)
{
    إذا (isStopped(نشانہ)) {
        cancelIPC(نشانہ);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        سیٹ_Thread_State(نشانہ, ThreadState_Restart);
        sched_context_t *sc = نشانہ->tcbSchedContext;
        إذا (sc_sporadic(sc) && sc != گرہ_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        إذا (isSchedulable(نشانہ)) {
            possible_Switch_منزل(نشانہ);
        }
#وإلا
        تیاری_جواب_Master(نشانہ);
        سیٹ_Thread_State(نشانہ, ThreadState_Restart);
        SCHED_قطار_وچ_پاؤ(نشانہ);
        possible_Switch_منزل(نشانہ);
#جے_انت
    }
}

فراغ doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, لفظ_t badge,
                   bool_t grant, tcb_t *receiver)
{
    فراغ *receive_بفر, *send_بفر;

    receive_بفر = lookupIPCBuffer(true, receiver);

    إذا (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        send_بفر = lookupIPCBuffer(false, sender);
        do_عام_Transfer(sender, send_بفر, endpoint, badge, grant,
                         receiver, receive_بفر);
    } وإلا {
        doFaultTransfer(badge, sender, receiver, receive_بفر);
    }
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
فراغ do_جواب_Transfer(tcb_t *sender, جواب_t *جواب, bool_t grant)
#وإلا
فراغ do_جواب_Transfer(tcb_t *sender, tcb_t *receiver, cte_t *تھاں, bool_t grant)
#جے_انت
{
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    إذا (جواب->جواب_TCB == عدم ||
        thread_state_get_tsType(جواب->جواب_TCB->tcbState) != Thread_State_Blocked_On_جواب) {
        /* nothing to do */
        أرجع;
    }

    tcb_t *receiver = جواب->جواب_TCB;
    جواب_remove(جواب, receiver);
    أكد(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    أكد(جواب->جواب_TCB == عدم);

    إذا (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != گرہ_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#وإلا
    أكد(thread_state_get_tsType(receiver->tcbState) ==
           Thread_State_Blocked_On_جواب);
#جے_انت

    لفظ_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    إذا (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, عدم, 0, grant, receiver);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        سیٹ_Thread_State(receiver, ThreadState_Running);
#وإلا
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(تھاں);
        سیٹ_Thread_State(receiver, ThreadState_Running);
        possible_Switch_منزل(receiver);
#جے_انت
    } وإلا {
#جے_بنا_تعریف CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(تھاں);
#جے_انت
        bool_t restart = handle_Fault_جواب(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        إذا (restart) {
            سیٹ_Thread_State(receiver, ThreadState_Restart);
#جے_بنا_تعریف CONFIG_KERNEL_MCS
            possible_Switch_منزل(receiver);
#جے_انت
        } وإلا {
            سیٹ_Thread_State(receiver, ThreadState_Inactive);
        }
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
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
#جے_انت
}

فراغ do_عام_Transfer(tcb_t *sender, لفظ_t *send_بفر, endpoint_t *endpoint,
                      لفظ_t badge, bool_t canGrant, tcb_t *receiver,
                      لفظ_t *receive_بفر)
{
    لفظ_t پیغام_Transferred;
    se_L4_سنیہا_Info_t ٹیگ;
    exception_t حالت;

    ٹیگ = سنیہا_Info_From_لفظ(get_رجسٹر(sender, پیغام_Info_رجسٹر));

    إذا (canGrant) {
        حالت = lookupExtraCaps(sender, send_بفر, ٹیگ);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = عدم;
        }
    } وإلا {
        current_extra_caps.excaprefs[0] = عدم;
    }

    پیغام_Transferred = copyMRs(sender, send_بفر, receiver, receive_بفر,
                             se_L4_سنیہا_Info_get_لمائی(ٹیگ));

    ٹیگ = transferCaps(ٹیگ, endpoint, receiver, receive_بفر);

    ٹیگ = se_L4_سنیہا_Info_سیٹ_لمائی(ٹیگ, پیغام_Transferred);
    سیٹ_رجسٹر(receiver, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(ٹیگ));
    سیٹ_رجسٹر(receiver, badge_رجسٹر, badge);
}

فراغ doFaultTransfer(لفظ_t badge, tcb_t *sender, tcb_t *receiver,
                     لفظ_t *receiverIPCBuffer)
{
    لفظ_t گھلیا;
    se_L4_سنیہا_Info_t پیغام_Info;

    گھلیا = سیٹ_MRs_fault(sender, receiver, receiverIPCBuffer);
    پیغام_Info = se_L4_سنیہا_Info_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, گھلیا);
    سیٹ_رجسٹر(receiver, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(پیغام_Info));
    سیٹ_رجسٹر(receiver, badge_رجسٹر, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ساكن se_L4_سنیہا_Info_t transferCaps(se_L4_سنیہا_Info_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       لفظ_t *receive_بفر)
{
    لفظ_t i;
    cte_t *dest_تھاں;

    info = se_L4_سنیہا_Info_سیٹ_extra_Caps(info, 0);
    info = se_L4_سنیہا_Info_سیٹ_caps_Unwrapped(info, 0);

    إذا (likely(!current_extra_caps.excaprefs[0] || !receive_بفر)) {
        أرجع info;
    }

    dest_تھاں = getReceiveSlots(receiver, receive_بفر);

    لكل (i = 0; i < se_L4_پیغام_Max_Extra_Caps && current_extra_caps.excaprefs[i] != عدم; i++) {
        cte_t *تھاں = current_extra_caps.excaprefs[i];
        cap_t cap = تھاں->cap;

        إذا (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            سیٹ_Extra_Badge(receive_بفر,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = se_L4_سنیہا_Info_سیٹ_caps_Unwrapped(info,
                                                      se_L4_سنیہا_Info_get_caps_Unwrapped(info) | (1 << i));

        } وإلا {
            deriveCap_ret_t dc_ret;

            إذا (!dest_تھاں) {
                اكسر;
            }

            dc_ret = deriveCap(تھاں, cap);

            إذا (dc_ret.حالت != EXCEPTION_NONE) {
                اكسر;
            }
            إذا (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                اكسر;
            }

            cte_داخل_کرو(dc_ret.cap, تھاں, dest_تھاں);

            dest_تھاں = عدم;
        }
    }

    أرجع se_L4_سنیہا_Info_سیٹ_extra_Caps(info, i);
}

فراغ doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
}

فراغ prepare_سیٹ_Domain(tcb_t *tptr, dom_t dom)
{
    إذا (ksCurDomain != dom) {
        Arch_prepare_سیٹ_Domain(tptr, dom);
#جے_تعریف_شدہ CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#جے_انت
    }
}

ساكن فراغ prepare_اگلا_Domain(فراغ)
{
    Arch_prepare_اگلا_Domain();
#جے_تعریف_شدہ CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(عدم);
#جے_انت
}

ساكن فراغ اگلا_Domain(فراغ)
{
    ks_Dom_ویلا_سارنی_Idx++;
    إذا (dschedule_is_سرا_marker(ks_Dom_ویلا_سارنی_Idx)) {
        ks_Dom_ویلا_سارنی_Idx = ks_Dom_ویلا_سارنی_شروع;
    }
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ksReprogram) = true;
#جے_انت
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ks_Dom_ویلا_سارنی[ks_Dom_ویلا_سارنی_Idx]);
    ks_Domain_ویلا = dschedule_duration(ks_Dom_ویلا_سارنی[ks_Dom_ویلا_سارنی_Idx]);
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
ساكن فراغ switchSchedContext(فراغ)
{
    إذا (unlikely(گرہ_STATE(ksCurSC) != گرہ_STATE(ksCurThread)->tcbSchedContext)) {
        گرہ_STATE(ksReprogram) = true;
        إذا (sc_constant_bandwidth(گرہ_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(گرہ_STATE(ksCurThread)->tcbSchedContext);
        }

        أكد(refill_ready(گرہ_STATE(ksCurThread)->tcbSchedContext));
        أكد(refill_sufficient(گرہ_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    إذا (گرہ_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commit_ویلا();
    }

    گرہ_STATE(ksCurSC) = گرہ_STATE(ksCurThread)->tcbSchedContext;
}
#جے_انت

ساكن فراغ ویلا_سارنی_چنو_New_Thread(فراغ)
{
    إذا (ks_Domain_ویلا == 0) {
        prepare_اگلا_Domain();
        اگلا_Domain();
    }
    چنو_Thread();
}

فراغ ویلا_سارنی(فراغ)
{
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    awaken();
    check_Domain_ویلا();
#جے_انت

    إذا (گرہ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        إذا (isSchedulable(گرہ_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_قطار_وچ_پاؤ_CURRENT_TCB;
        } وإلا {
            was_runnable = false;
        }

        إذا (گرہ_STATE(ksSchedulerAction) == Scheduler_Action_چنو_New_Thread) {
            ویلا_سارنی_چنو_New_Thread();
        } وإلا {
            tcb_t *candidate = گرہ_STATE(ksSchedulerAction);
            أكد(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                گرہ_STATE(ksCurThread) == گرہ_STATE(ksIdleThread)
                || (candidate->tcb_ترجیح < گرہ_STATE(ksCurThread)->tcb_ترجیح);
            إذا (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcb_ترجیح)) {
                SCHED_قطار_وچ_پاؤ(candidate);
                /* we can't, need to reschedule */
                گرہ_STATE(ksSchedulerAction) = Scheduler_Action_چنو_New_Thread;
                ویلا_سارنی_چنو_New_Thread();
            } وإلا إذا (was_runnable && candidate->tcb_ترجیح == گرہ_STATE(ksCurThread)->tcb_ترجیح) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                گرہ_STATE(ksSchedulerAction) = Scheduler_Action_چنو_New_Thread;
                ویلا_سارنی_چنو_New_Thread();
            } وإلا {
                أكد(candidate != گرہ_STATE(ksCurThread));
                switch_منزل_Thread(candidate);
            }
        }
    }
    گرہ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    do_نقاب_Reschedule(ARCH_گرہ_STATE(ipiReschedulePending));
    ARCH_گرہ_STATE(ipiReschedulePending) = 0;
#جے_انت /* ENABLE_SMP_SUPPORT */

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    switchSchedContext();

    إذا (گرہ_STATE(ksReprogram)) {
        سیٹ_اگلا_رکاوٹ();
        گرہ_STATE(ksReprogram) = false;
    }
#جے_انت
}

فراغ چنو_Thread(فراغ)
{
    لفظ_t prio;
    لفظ_t dom;
    tcb_t *thread;

    إذا (عدد_Domains > 1) {
        dom = ksCurDomain;
    } وإلا {
        dom = 0;
    }

    إذا (likely(گرہ_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = گرہ_STATE(ksReadyQueues)[ready_queues_اشاریہ(dom, prio)].سر;
        أكد(thread);
        أكد(isSchedulable(thread));
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        أكد(refill_sufficient(thread->tcbSchedContext, 0));
        أكد(refill_ready(thread->tcbSchedContext));
#جے_انت
        switch_منزل_Thread(thread);
    } وإلا {
        switch_منزل_Idle_Thread();
    }
}

فراغ switch_منزل_Thread(tcb_t *thread)
{
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    أكد(thread->tcbSchedContext != عدم);
    أكد(!thread_state_get_tcb_وچ_Release_قطار(thread->tcbState));
    أكد(refill_sufficient(thread->tcbSchedContext, 0));
    أكد(refill_ready(thread->tcbSchedContext));
#جے_انت

#جے_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(گرہ_STATE(ksCurThread), thread);
#جے_انت
    Arch_switch_منزل_Thread(thread);

#جے_تعریف_شدہ CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#جے_انت /* CONFIG_HAVE_FPU */

    tcb_Sched_قطار_توں_کڈھو(thread);
    گرہ_STATE(ksCurThread) = thread;
}

فراغ switch_منزل_Idle_Thread(فراغ)
{
#جے_تعریف_شدہ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(گرہ_STATE(ksCurThread), گرہ_STATE(ksIdleThread));
#جے_انت
    Arch_switch_منزل_Idle_Thread();
    گرہ_STATE(ksCurThread) = گرہ_STATE(ksIdleThread);
}

فراغ سیٹ_Domain(tcb_t *tptr, dom_t dom)
{
    tcb_Sched_قطار_توں_کڈھو(tptr);
    tptr->tcbDomain = dom;
    إذا (isSchedulable(tptr)) {
        SCHED_قطار_وچ_پاؤ(tptr);
    }
    إذا (tptr == گرہ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

فراغ سیٹ_MCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
فراغ سیٹ_ترجیح(tcb_t *tptr, prio_t prio)
{
    بدل (thread_state_get_tsType(tptr->tcbState)) {
    حالة ThreadState_Running:
    حالة ThreadState_Restart:
        إذا (thread_state_get_tcbQueued(tptr->tcbState) || tptr == گرہ_STATE(ksCurThread)) {
            tcb_Sched_قطار_توں_کڈھو(tptr);
            tptr->tcb_ترجیح = prio;
            SCHED_قطار_وچ_پاؤ(tptr);
            rescheduleRequired();
        } وإلا {
            tptr->tcb_ترجیح = prio;
        }
        اكسر;
    حالة ThreadState_BlockedOnReceive:
    حالة ThreadState_BlockedOnSend:
        tptr->tcb_ترجیح = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        اكسر;
    حالة ThreadState_BlockedOnNotification:
        tptr->tcb_ترجیح = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        اكسر;
    افتراضي:
        tptr->tcb_ترجیح = prio;
        اكسر;
    }
}
#وإلا
فراغ سیٹ_ترجیح(tcb_t *tptr, prio_t prio)
{
    tcb_Sched_قطار_توں_کڈھو(tptr);
    tptr->tcb_ترجیح = prio;
    إذا (isRunnable(tptr)) {
        إذا (tptr == گرہ_STATE(ksCurThread)) {
            rescheduleRequired();
        } وإلا {
            possible_Switch_منزل(tptr);
        }
    }
}
#جے_انت

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
فراغ possible_Switch_منزل(tcb_t *نشانہ)
{
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    إذا (نشانہ->tcbSchedContext != عدم && !thread_state_get_tcb_وچ_Release_قطار(نشانہ->tcbState)) {
#جے_انت
        إذا (ksCurDomain != نشانہ->tcbDomain
            SMP_COND_STATEMENT( || نشانہ->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_قطار_وچ_پاؤ(نشانہ);
        } وإلا إذا (گرہ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_قطار_وچ_پاؤ(نشانہ);
        } وإلا {
            گرہ_STATE(ksSchedulerAction) = نشانہ;
        }
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    }
#جے_انت

}

فراغ سیٹ_Thread_State(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_سیٹ_ts_Type(&tptr->tcbState, ts);
    ویلا_سارنی_TCB(tptr);
}

فراغ ویلا_سارنی_TCB(tcb_t *tptr)
{
    إذا (tptr == گرہ_STATE(ksCurThread) &&
        گرہ_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        گرہ_STATE(ksSchedulerAction) = Scheduler_Action_چنو_New_Thread;
    }
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
فراغ postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    أكد(tcb != عدم);

    tcb_Sched_قطار_توں_کڈھو(tcb);
    tcb_Release_قطار_وچ_پاؤ(tcb);
    گرہ_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

فراغ سیٹ_اگلا_رکاوٹ(فراغ)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_سر_refill = *refill_سر(گرہ_STATE(ksCurThread)->tcbSchedContext);
    ticks_t اگلا_رکاوٹ = گرہ_STATE(ks_Cur_ویلا) + ct_سر_refill.rAmount;

    إذا (عدد_Domains > 1) {
        اگلا_رکاوٹ = MIN(اگلا_رکاوٹ, گرہ_STATE(ks_Cur_ویلا) + ks_Domain_ویلا);
    }

    tcb_t *rlq_سر = گرہ_STATE(ks_Release_قطار.سر);
    إذا (rlq_سر != عدم) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_سر_refill = *refill_سر(rlq_سر->tcbSchedContext);
        اگلا_رکاوٹ = MIN(rlq_سر_refill.r_ویلا, اگلا_رکاوٹ);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    أكد(اگلا_رکاوٹ >= گرہ_STATE(ks_Cur_ویلا));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    سیٹ_Deadline(اگلا_رکاوٹ - getTimerPrecision());
}

فراغ chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    إذا (likely(گرہ_STATE(ksCurSC) != گرہ_STATE(ksIdleSC))) {
        إذا (is_گیڑا_Robin(گرہ_STATE(ksCurSC))) {
            أكد(refill_ناپ(گرہ_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t سر = *refill_سر(گرہ_STATE(ksCurSC));
            refill_t پوچھ = *refill_پوچھ(گرہ_STATE(ksCurSC));
            refill_سر(گرہ_STATE(ksCurSC))->rAmount = سر.rAmount + پوچھ.rAmount;
            refill_پوچھ(گرہ_STATE(ksCurSC))->rAmount = 0;
        } وإلا {
            refill_budget_check(consumed);
        }

        أكد(refill_سر(گرہ_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        گرہ_STATE(ksCurSC)->scConsumed += consumed;
    }
    گرہ_STATE(ksConsumed) = 0;
    إذا (likely(isSchedulable(گرہ_STATE(ksCurThread)))) {
        أكد(گرہ_STATE(ksCurThread)->tcbSchedContext == گرہ_STATE(ksCurSC));
        سرا_Timeslice(canTimeoutFault);
        rescheduleRequired();
        گرہ_STATE(ksReprogram) = true;
    }
}

فراغ سرا_Timeslice(bool_t can_timeout_fault)
{
    bool_t گیڑا_robin = is_گیڑا_Robin(گرہ_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(گرہ_STATE(ksCurThread));

    إذا (can_timeout_fault && !گیڑا_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(گرہ_STATE(ksCurSC)->scBadge);
        handleTimeout(گرہ_STATE(ksCurThread));
    } وإلا إذا (refill_ready(گرہ_STATE(ksCurSC)) && refill_sufficient(گرہ_STATE(ksCurSC), 0)) {
        /* apply round robin */
        أكد(!thread_state_get_tcbQueued(گرہ_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } وإلا {
        /* postpone until ready */
        postpone(گرہ_STATE(ksCurSC));
    }
}
#وإلا

فراغ timerTick(فراغ)
{
    إذا (likely(thread_state_get_tsType(گرہ_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#جے_تعریف_شدہ CONFIG_VTX
        || thread_state_get_tsType(گرہ_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#جے_انت
       ) {
        إذا (گرہ_STATE(ksCurThread)->tcb_ویلا_ٹکڑا > 1) {
            گرہ_STATE(ksCurThread)->tcb_ویلا_ٹکڑا--;
        } وإلا {
            گرہ_STATE(ksCurThread)->tcb_ویلا_ٹکڑا = CONFIG_ویلا_ٹکڑا;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    إذا (عدد_Domains > 1) {
        ks_Domain_ویلا--;
        إذا (ks_Domain_ویلا == 0) {
            rescheduleRequired();
        }
    }
}
#جے_انت

فراغ rescheduleRequired(فراغ)
{
    إذا (گرہ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && گرہ_STATE(ksSchedulerAction) != Scheduler_Action_چنو_New_Thread
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        && isSchedulable(گرہ_STATE(ksSchedulerAction))
#جے_انت
       ) {
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        أكد(refill_sufficient(گرہ_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        أكد(refill_ready(گرہ_STATE(ksSchedulerAction)->tcbSchedContext));
#جے_انت
        SCHED_قطار_وچ_پاؤ(گرہ_STATE(ksSchedulerAction));
    }
    گرہ_STATE(ksSchedulerAction) = Scheduler_Action_چنو_New_Thread;
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS

ساكن مضمن bool_t خالص release_q_non_خالی_تھاواں_and_ready(فراغ)
{
    أرجع گرہ_STATE(ks_Release_قطار.سر) != عدم
           && refill_ready(گرہ_STATE(ks_Release_قطار.سر)->tcbSchedContext);
}

ساكن فراغ tcb_Release_قطار_توں_کڈھو(فراغ)
{
    أكد(گرہ_STATE(ks_Release_قطار.سر) != عدم);
    أكد(گرہ_STATE(ks_Release_قطار.سر)->tcbSchedPrev == عدم);
    SMP_COND_STATEMENT(أكد(گرہ_STATE(ks_Release_قطار.سر)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = گرہ_STATE(ks_Release_قطار.سر);
    أكد(awakened != گرہ_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    أكد(!is_گیڑا_Robin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(أكد(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    أكد(refill_sufficient(awakened->tcbSchedContext, 0));
    possible_Switch_منزل(awakened);
}

فراغ awaken(فراغ)
{
    طالما (unlikely(release_q_non_خالی_تھاواں_and_ready())) {
        tcb_Release_قطار_توں_کڈھو();
    }
}
#جے_انت
