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
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#جيڪڏهن_پورو
#ضمن <model/statedata.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/thread.h>
#ضمن <machine/registerset.h>
#ضمن <machine/fpu.h>
#ضمن <linker.h>

ساكن se_L4_پيغام_Info_t
transferCaps(se_L4_پيغام_Info_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             لفظ_t *receive_بفر);

BOOT_ڪوڊ فراغ configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    سيٽ_Thread_State(tcb, ThreadState_IdleThreadState);
}

فراغ activateThread(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    إذا (unlikely(ڳنڍ_STATE(ksCurThread)->tcb_Yield_منزل)) {
        sched_Context_complete_Yield_منزل(ڳنڍ_STATE(ksCurThread));
        أكد(thread_state_get_tsType(ڳنڍ_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#جيڪڏهن_پورو

    بدل (thread_state_get_tsType(ڳنڍ_STATE(ksCurThread)->tcbState)) {
    حالة ThreadState_Running:
#جيڪڏهن_وصف_ٿيل CONFIG_VTX
    حالة ThreadState_RunningVM:
#جيڪڏهن_پورو
        اكسر;

    حالة ThreadState_Restart: {
        لفظ_t pc;

        pc = getRestartPC(ڳنڍ_STATE(ksCurThread));
        سيٽ_ايندڙ_PC(ڳنڍ_STATE(ksCurThread), pc);
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
        اكسر;
    }

    حالة ThreadState_IdleThreadState:
        Arch_activateIdleThread(ڳنڍ_STATE(ksCurThread));
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
    tcb_Sched_قطار_مان_ڪڍو(هدف);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    tcbReleaseRemove(هدف);
    sched_Context_cancel_Yield_منزل(هدف);
#جيڪڏهن_پورو
    سيٽ_Thread_State(هدف, ThreadState_Inactive);
}

فراغ restart(tcb_t *هدف)
{
    إذا (isStopped(هدف)) {
        cancelIPC(هدف);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        سيٽ_Thread_State(هدف, ThreadState_Restart);
        sched_context_t *sc = هدف->tcbSchedContext;
        إذا (sc_sporadic(sc) && sc != ڳنڍ_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        إذا (isSchedulable(هدف)) {
            possible_Switch_منزل(هدف);
        }
#وإلا
        تياري_جواب_Master(هدف);
        سيٽ_Thread_State(هدف, ThreadState_Restart);
        SCHED_قطار_اندر_وجهو(هدف);
        possible_Switch_منزل(هدف);
#جيڪڏهن_پورو
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

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
فراغ do_جواب_Transfer(tcb_t *sender, جواب_t *جواب, bool_t grant)
#وإلا
فراغ do_جواب_Transfer(tcb_t *sender, tcb_t *receiver, cte_t *جڳهه, bool_t grant)
#جيڪڏهن_پورو
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
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
        && receiver->tcbSchedContext != ڳنڍ_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#وإلا
    أكد(thread_state_get_tsType(receiver->tcbState) ==
           Thread_State_Blocked_On_جواب);
#جيڪڏهن_پورو

    لفظ_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    إذا (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, عدم, 0, grant, receiver);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        سيٽ_Thread_State(receiver, ThreadState_Running);
#وإلا
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(جڳهه);
        سيٽ_Thread_State(receiver, ThreadState_Running);
        possible_Switch_منزل(receiver);
#جيڪڏهن_پورو
    } وإلا {
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(جڳهه);
#جيڪڏهن_پورو
        bool_t restart = handle_Fault_جواب(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        إذا (restart) {
            سيٽ_Thread_State(receiver, ThreadState_Restart);
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
            possible_Switch_منزل(receiver);
#جيڪڏهن_پورو
        } وإلا {
            سيٽ_Thread_State(receiver, ThreadState_Inactive);
        }
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
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
#جيڪڏهن_پورو
}

فراغ do_عام_Transfer(tcb_t *sender, لفظ_t *send_بفر, endpoint_t *endpoint,
                      لفظ_t badge, bool_t canGrant, tcb_t *receiver,
                      لفظ_t *receive_بفر)
{
    لفظ_t نياپو_Transferred;
    se_L4_پيغام_Info_t ٽيگ;
    exception_t حالت;

    ٽيگ = پيغام_Info_From_لفظ(get_رجسٽر(sender, نياپو_Info_رجسٽر));

    إذا (canGrant) {
        حالت = lookupExtraCaps(sender, send_بفر, ٽيگ);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = عدم;
        }
    } وإلا {
        current_extra_caps.excaprefs[0] = عدم;
    }

    نياپو_Transferred = copyMRs(sender, send_بفر, receiver, receive_بفر,
                             se_L4_پيغام_Info_get_ڊيگهائي(ٽيگ));

    ٽيگ = transferCaps(ٽيگ, endpoint, receiver, receive_بفر);

    ٽيگ = se_L4_پيغام_Info_سيٽ_ڊيگهائي(ٽيگ, نياپو_Transferred);
    سيٽ_رجسٽر(receiver, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(ٽيگ));
    سيٽ_رجسٽر(receiver, badge_رجسٽر, badge);
}

فراغ doFaultTransfer(لفظ_t badge, tcb_t *sender, tcb_t *receiver,
                     لفظ_t *receiverIPCBuffer)
{
    لفظ_t موڪليل;
    se_L4_پيغام_Info_t نياپو_Info;

    موڪليل = سيٽ_MRs_fault(sender, receiver, receiverIPCBuffer);
    نياپو_Info = se_L4_پيغام_Info_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, موڪليل);
    سيٽ_رجسٽر(receiver, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(نياپو_Info));
    سيٽ_رجسٽر(receiver, badge_رجسٽر, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ساكن se_L4_پيغام_Info_t transferCaps(se_L4_پيغام_Info_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       لفظ_t *receive_بفر)
{
    لفظ_t i;
    cte_t *dest_جڳهه;

    info = se_L4_پيغام_Info_سيٽ_extra_Caps(info, 0);
    info = se_L4_پيغام_Info_سيٽ_caps_Unwrapped(info, 0);

    إذا (likely(!current_extra_caps.excaprefs[0] || !receive_بفر)) {
        أرجع info;
    }

    dest_جڳهه = getReceiveSlots(receiver, receive_بفر);

    لكل (i = 0; i < se_L4_نياپو_Max_Extra_Caps && current_extra_caps.excaprefs[i] != عدم; i++) {
        cte_t *جڳهه = current_extra_caps.excaprefs[i];
        cap_t cap = جڳهه->cap;

        إذا (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            سيٽ_Extra_Badge(receive_بفر,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = se_L4_پيغام_Info_سيٽ_caps_Unwrapped(info,
                                                      se_L4_پيغام_Info_get_caps_Unwrapped(info) | (1 << i));

        } وإلا {
            deriveCap_ret_t dc_ret;

            إذا (!dest_جڳهه) {
                اكسر;
            }

            dc_ret = deriveCap(جڳهه, cap);

            إذا (dc_ret.حالت != EXCEPTION_NONE) {
                اكسر;
            }
            إذا (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                اكسر;
            }

            cte_وجهو(dc_ret.cap, جڳهه, dest_جڳهه);

            dest_جڳهه = عدم;
        }
    }

    أرجع se_L4_پيغام_Info_سيٽ_extra_Caps(info, i);
}

فراغ doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
}

فراغ prepare_سيٽ_Domain(tcb_t *tptr, dom_t dom)
{
    إذا (ksCurDomain != dom) {
        Arch_prepare_سيٽ_Domain(tptr, dom);
#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#جيڪڏهن_پورو
    }
}

ساكن فراغ prepare_ايندڙ_Domain(فراغ)
{
    Arch_prepare_ايندڙ_Domain();
#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(عدم);
#جيڪڏهن_پورو
}

ساكن فراغ ايندڙ_Domain(فراغ)
{
    ks_Dom_وقت_جدول_Idx++;
    إذا (dschedule_is_پڇاڙي_marker(ks_Dom_وقت_جدول_Idx)) {
        ks_Dom_وقت_جدول_Idx = ks_Dom_وقت_جدول_شروعات;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    ڳنڍ_STATE(ksReprogram) = true;
#جيڪڏهن_پورو
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ks_Dom_وقت_جدول[ks_Dom_وقت_جدول_Idx]);
    ks_Domain_وقت = dschedule_duration(ks_Dom_وقت_جدول[ks_Dom_وقت_جدول_Idx]);
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
ساكن فراغ switchSchedContext(فراغ)
{
    إذا (unlikely(ڳنڍ_STATE(ksCurSC) != ڳنڍ_STATE(ksCurThread)->tcbSchedContext)) {
        ڳنڍ_STATE(ksReprogram) = true;
        إذا (sc_constant_bandwidth(ڳنڍ_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(ڳنڍ_STATE(ksCurThread)->tcbSchedContext);
        }

        أكد(refill_ready(ڳنڍ_STATE(ksCurThread)->tcbSchedContext));
        أكد(refill_sufficient(ڳنڍ_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    إذا (ڳنڍ_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commit_وقت();
    }

    ڳنڍ_STATE(ksCurSC) = ڳنڍ_STATE(ksCurThread)->tcbSchedContext;
}
#جيڪڏهن_پورو

ساكن فراغ وقت_جدول_چونڊيو_New_Thread(فراغ)
{
    إذا (ks_Domain_وقت == 0) {
        prepare_ايندڙ_Domain();
        ايندڙ_Domain();
    }
    چونڊيو_Thread();
}

فراغ وقت_جدول(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    awaken();
    check_Domain_وقت();
#جيڪڏهن_پورو

    إذا (ڳنڍ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        إذا (isSchedulable(ڳنڍ_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_قطار_اندر_وجهو_CURRENT_TCB;
        } وإلا {
            was_runnable = false;
        }

        إذا (ڳنڍ_STATE(ksSchedulerAction) == Scheduler_Action_چونڊيو_New_Thread) {
            وقت_جدول_چونڊيو_New_Thread();
        } وإلا {
            tcb_t *candidate = ڳنڍ_STATE(ksSchedulerAction);
            أكد(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                ڳنڍ_STATE(ksCurThread) == ڳنڍ_STATE(ksIdleThread)
                || (candidate->tcb_ترجيح < ڳنڍ_STATE(ksCurThread)->tcb_ترجيح);
            إذا (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcb_ترجيح)) {
                SCHED_قطار_اندر_وجهو(candidate);
                /* we can't, need to reschedule */
                ڳنڍ_STATE(ksSchedulerAction) = Scheduler_Action_چونڊيو_New_Thread;
                وقت_جدول_چونڊيو_New_Thread();
            } وإلا إذا (was_runnable && candidate->tcb_ترجيح == ڳنڍ_STATE(ksCurThread)->tcb_ترجيح) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                ڳنڍ_STATE(ksSchedulerAction) = Scheduler_Action_چونڊيو_New_Thread;
                وقت_جدول_چونڊيو_New_Thread();
            } وإلا {
                أكد(candidate != ڳنڍ_STATE(ksCurThread));
                switch_منزل_Thread(candidate);
            }
        }
    }
    ڳنڍ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    do_نقاب_Reschedule(ARCH_ڳنڍ_STATE(ipiReschedulePending));
    ARCH_ڳنڍ_STATE(ipiReschedulePending) = 0;
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    switchSchedContext();

    إذا (ڳنڍ_STATE(ksReprogram)) {
        سيٽ_ايندڙ_مداخلت();
        ڳنڍ_STATE(ksReprogram) = false;
    }
#جيڪڏهن_پورو
}

فراغ چونڊيو_Thread(فراغ)
{
    لفظ_t prio;
    لفظ_t dom;
    tcb_t *thread;

    إذا (انگ_Domains > 1) {
        dom = ksCurDomain;
    } وإلا {
        dom = 0;
    }

    إذا (likely(ڳنڍ_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = ڳنڍ_STATE(ksReadyQueues)[ready_queues_اشاريو(dom, prio)].سر;
        أكد(thread);
        أكد(isSchedulable(thread));
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        أكد(refill_sufficient(thread->tcbSchedContext, 0));
        أكد(refill_ready(thread->tcbSchedContext));
#جيڪڏهن_پورو
        switch_منزل_Thread(thread);
    } وإلا {
        switch_منزل_Idle_Thread();
    }
}

فراغ switch_منزل_Thread(tcb_t *thread)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    أكد(thread->tcbSchedContext != عدم);
    أكد(!thread_state_get_tcb_اندريون_Release_قطار(thread->tcbState));
    أكد(refill_sufficient(thread->tcbSchedContext, 0));
    أكد(refill_ready(thread->tcbSchedContext));
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ڳنڍ_STATE(ksCurThread), thread);
#جيڪڏهن_پورو
    Arch_switch_منزل_Thread(thread);

#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#جيڪڏهن_پورو /* CONFIG_HAVE_FPU */

    tcb_Sched_قطار_مان_ڪڍو(thread);
    ڳنڍ_STATE(ksCurThread) = thread;
}

فراغ switch_منزل_Idle_Thread(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ڳنڍ_STATE(ksCurThread), ڳنڍ_STATE(ksIdleThread));
#جيڪڏهن_پورو
    Arch_switch_منزل_Idle_Thread();
    ڳنڍ_STATE(ksCurThread) = ڳنڍ_STATE(ksIdleThread);
}

فراغ سيٽ_Domain(tcb_t *tptr, dom_t dom)
{
    tcb_Sched_قطار_مان_ڪڍو(tptr);
    tptr->tcbDomain = dom;
    إذا (isSchedulable(tptr)) {
        SCHED_قطار_اندر_وجهو(tptr);
    }
    إذا (tptr == ڳنڍ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

فراغ سيٽ_MCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
فراغ سيٽ_ترجيح(tcb_t *tptr, prio_t prio)
{
    بدل (thread_state_get_tsType(tptr->tcbState)) {
    حالة ThreadState_Running:
    حالة ThreadState_Restart:
        إذا (thread_state_get_tcbQueued(tptr->tcbState) || tptr == ڳنڍ_STATE(ksCurThread)) {
            tcb_Sched_قطار_مان_ڪڍو(tptr);
            tptr->tcb_ترجيح = prio;
            SCHED_قطار_اندر_وجهو(tptr);
            rescheduleRequired();
        } وإلا {
            tptr->tcb_ترجيح = prio;
        }
        اكسر;
    حالة ThreadState_BlockedOnReceive:
    حالة ThreadState_BlockedOnSend:
        tptr->tcb_ترجيح = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        اكسر;
    حالة ThreadState_BlockedOnNotification:
        tptr->tcb_ترجيح = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        اكسر;
    افتراضي:
        tptr->tcb_ترجيح = prio;
        اكسر;
    }
}
#وإلا
فراغ سيٽ_ترجيح(tcb_t *tptr, prio_t prio)
{
    tcb_Sched_قطار_مان_ڪڍو(tptr);
    tptr->tcb_ترجيح = prio;
    إذا (isRunnable(tptr)) {
        إذا (tptr == ڳنڍ_STATE(ksCurThread)) {
            rescheduleRequired();
        } وإلا {
            possible_Switch_منزل(tptr);
        }
    }
}
#جيڪڏهن_پورو

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
فراغ possible_Switch_منزل(tcb_t *هدف)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    إذا (هدف->tcbSchedContext != عدم && !thread_state_get_tcb_اندريون_Release_قطار(هدف->tcbState)) {
#جيڪڏهن_پورو
        إذا (ksCurDomain != هدف->tcbDomain
            SMP_COND_STATEMENT( || هدف->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_قطار_اندر_وجهو(هدف);
        } وإلا إذا (ڳنڍ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_قطار_اندر_وجهو(هدف);
        } وإلا {
            ڳنڍ_STATE(ksSchedulerAction) = هدف;
        }
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    }
#جيڪڏهن_پورو

}

فراغ سيٽ_Thread_State(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_سيٽ_ts_Type(&tptr->tcbState, ts);
    وقت_جدول_TCB(tptr);
}

فراغ وقت_جدول_TCB(tcb_t *tptr)
{
    إذا (tptr == ڳنڍ_STATE(ksCurThread) &&
        ڳنڍ_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        ڳنڍ_STATE(ksSchedulerAction) = Scheduler_Action_چونڊيو_New_Thread;
    }
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
فراغ postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    أكد(tcb != عدم);

    tcb_Sched_قطار_مان_ڪڍو(tcb);
    tcb_Release_قطار_اندر_وجهو(tcb);
    ڳنڍ_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

فراغ سيٽ_ايندڙ_مداخلت(فراغ)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_سر_refill = *refill_سر(ڳنڍ_STATE(ksCurThread)->tcbSchedContext);
    ticks_t ايندڙ_مداخلت = ڳنڍ_STATE(ks_Cur_وقت) + ct_سر_refill.rAmount;

    إذا (انگ_Domains > 1) {
        ايندڙ_مداخلت = MIN(ايندڙ_مداخلت, ڳنڍ_STATE(ks_Cur_وقت) + ks_Domain_وقت);
    }

    tcb_t *rlq_سر = ڳنڍ_STATE(ks_Release_قطار.سر);
    إذا (rlq_سر != عدم) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_سر_refill = *refill_سر(rlq_سر->tcbSchedContext);
        ايندڙ_مداخلت = MIN(rlq_سر_refill.r_وقت, ايندڙ_مداخلت);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    أكد(ايندڙ_مداخلت >= ڳنڍ_STATE(ks_Cur_وقت));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    سيٽ_Deadline(ايندڙ_مداخلت - getTimerPrecision());
}

فراغ chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    إذا (likely(ڳنڍ_STATE(ksCurSC) != ڳنڍ_STATE(ksIdleSC))) {
        إذا (is_ڦيرو_Robin(ڳنڍ_STATE(ksCurSC))) {
            أكد(refill_سائيز(ڳنڍ_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t سر = *refill_سر(ڳنڍ_STATE(ksCurSC));
            refill_t پڇ = *refill_پڇ(ڳنڍ_STATE(ksCurSC));
            refill_سر(ڳنڍ_STATE(ksCurSC))->rAmount = سر.rAmount + پڇ.rAmount;
            refill_پڇ(ڳنڍ_STATE(ksCurSC))->rAmount = 0;
        } وإلا {
            refill_budget_check(consumed);
        }

        أكد(refill_سر(ڳنڍ_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        ڳنڍ_STATE(ksCurSC)->scConsumed += consumed;
    }
    ڳنڍ_STATE(ksConsumed) = 0;
    إذا (likely(isSchedulable(ڳنڍ_STATE(ksCurThread)))) {
        أكد(ڳنڍ_STATE(ksCurThread)->tcbSchedContext == ڳنڍ_STATE(ksCurSC));
        پڇاڙي_Timeslice(canTimeoutFault);
        rescheduleRequired();
        ڳنڍ_STATE(ksReprogram) = true;
    }
}

فراغ پڇاڙي_Timeslice(bool_t can_timeout_fault)
{
    bool_t ڦيرو_robin = is_ڦيرو_Robin(ڳنڍ_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(ڳنڍ_STATE(ksCurThread));

    إذا (can_timeout_fault && !ڦيرو_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(ڳنڍ_STATE(ksCurSC)->scBadge);
        handleTimeout(ڳنڍ_STATE(ksCurThread));
    } وإلا إذا (refill_ready(ڳنڍ_STATE(ksCurSC)) && refill_sufficient(ڳنڍ_STATE(ksCurSC), 0)) {
        /* apply round robin */
        أكد(!thread_state_get_tcbQueued(ڳنڍ_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } وإلا {
        /* postpone until ready */
        postpone(ڳنڍ_STATE(ksCurSC));
    }
}
#وإلا

فراغ timerTick(فراغ)
{
    إذا (likely(thread_state_get_tsType(ڳنڍ_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#جيڪڏهن_وصف_ٿيل CONFIG_VTX
        || thread_state_get_tsType(ڳنڍ_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#جيڪڏهن_پورو
       ) {
        إذا (ڳنڍ_STATE(ksCurThread)->tcb_وقت_ٽڪرو > 1) {
            ڳنڍ_STATE(ksCurThread)->tcb_وقت_ٽڪرو--;
        } وإلا {
            ڳنڍ_STATE(ksCurThread)->tcb_وقت_ٽڪرو = CONFIG_وقت_ٽڪرو;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    إذا (انگ_Domains > 1) {
        ks_Domain_وقت--;
        إذا (ks_Domain_وقت == 0) {
            rescheduleRequired();
        }
    }
}
#جيڪڏهن_پورو

فراغ rescheduleRequired(فراغ)
{
    إذا (ڳنڍ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && ڳنڍ_STATE(ksSchedulerAction) != Scheduler_Action_چونڊيو_New_Thread
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        && isSchedulable(ڳنڍ_STATE(ksSchedulerAction))
#جيڪڏهن_پورو
       ) {
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        أكد(refill_sufficient(ڳنڍ_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        أكد(refill_ready(ڳنڍ_STATE(ksSchedulerAction)->tcbSchedContext));
#جيڪڏهن_پورو
        SCHED_قطار_اندر_وجهو(ڳنڍ_STATE(ksSchedulerAction));
    }
    ڳنڍ_STATE(ksSchedulerAction) = Scheduler_Action_چونڊيو_New_Thread;
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS

ساكن مضمن bool_t خالص release_q_non_خالي_جايون_and_ready(فراغ)
{
    أرجع ڳنڍ_STATE(ks_Release_قطار.سر) != عدم
           && refill_ready(ڳنڍ_STATE(ks_Release_قطار.سر)->tcbSchedContext);
}

ساكن فراغ tcb_Release_قطار_مان_ڪڍو(فراغ)
{
    أكد(ڳنڍ_STATE(ks_Release_قطار.سر) != عدم);
    أكد(ڳنڍ_STATE(ks_Release_قطار.سر)->tcbSchedPrev == عدم);
    SMP_COND_STATEMENT(أكد(ڳنڍ_STATE(ks_Release_قطار.سر)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = ڳنڍ_STATE(ks_Release_قطار.سر);
    أكد(awakened != ڳنڍ_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    أكد(!is_ڦيرو_Robin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(أكد(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    أكد(refill_sufficient(awakened->tcbSchedContext, 0));
    possible_Switch_منزل(awakened);
}

فراغ awaken(فراغ)
{
    طالما (unlikely(release_q_non_خالي_جايون_and_ready())) {
        tcb_Release_قطار_مان_ڪڍو();
    }
}
#جيڪڏهن_پورو
