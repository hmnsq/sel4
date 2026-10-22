/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <object.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <api/faults.h>
#అన్తర్భావ <api/types.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <object/domain.h>
#నిర్వచితమైతే CONFIG_KERNEL_MCS
#అన్తర్భావ <object/schedcontext.h>
#అయితే_ముగింపు
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/kernel/thread.h>
#అన్తర్భావ <machine/registerset.h>
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <linker.h>

స్థిర se_L4_సందేశం_Info_t
transferCaps(se_L4_సందేశం_Info_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             మాట_t *receive_బఫరు);

BOOT_సంకేతం శూన్య configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    సమితి_Thread_State(tcb, ThreadState_IdleThreadState);
}

శూన్య activateThread(శూన్య)
{
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (unlikely(కణుపు_STATE(ksCurThread)->tcb_Yield_గమ్యం)) {
        sched_Context_complete_Yield_గమ్యం(కణుపు_STATE(ksCurThread));
        ప్రతిజ్ఞా(thread_state_get_tsType(కణుపు_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#అయితే_ముగింపు

    ఎంపిక (thread_state_get_tsType(కణుపు_STATE(ksCurThread)->tcbState)) {
    స్థితి ThreadState_Running:
#నిర్వచితమైతే CONFIG_VTX
    స్థితి ThreadState_RunningVM:
#అయితే_ముగింపు
        ఆపు;

    స్థితి ThreadState_Restart: {
        మాట_t pc;

        pc = getRestartPC(కణుపు_STATE(ksCurThread));
        సమితి_తదుపరి_PC(కణుపు_STATE(ksCurThread), pc);
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
        ఆపు;
    }

    స్థితి ThreadState_IdleThreadState:
        Arch_activateIdleThread(కణుపు_STATE(ksCurThread));
        ఆపు;

    అప్రమేయ:
        fail("Current thread is blocked");
    }
}

శూన్య suspend(tcb_t *లక్ష్యం)
{
    cancelIPC(లక్ష్యం);
    ఒకవేళ (thread_state_get_tsType(లక్ష్యం->tcbState) == ThreadState_Running) {
        /* whilst in the running state it is possible that restart pc of a thread is
         * incorrect. As we do not know what state this thread will transition to
         * after we make it inactive we update its restart pc so that the thread next
         * runs at the correct address whether it is restarted or moved directly to
         * running */
        updateRestartPC(లక్ష్యం);
    }
    tcb_Sched_వరుస_నుండి_తీయి(లక్ష్యం);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    tcbReleaseRemove(లక్ష్యం);
    sched_Context_cancel_Yield_గమ్యం(లక్ష్యం);
#అయితే_ముగింపు
    సమితి_Thread_State(లక్ష్యం, ThreadState_Inactive);
}

శూన్య restart(tcb_t *లక్ష్యం)
{
    ఒకవేళ (isStopped(లక్ష్యం)) {
        cancelIPC(లక్ష్యం);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        సమితి_Thread_State(లక్ష్యం, ThreadState_Restart);
        sched_context_t *sc = లక్ష్యం->tcbSchedContext;
        ఒకవేళ (sc_sporadic(sc) && sc != కణుపు_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        ఒకవేళ (isSchedulable(లక్ష్యం)) {
            possible_Switch_గమ్యం(లక్ష్యం);
        }
#లేకపోతే
        అమరిక_జవాబు_Master(లక్ష్యం);
        సమితి_Thread_State(లక్ష్యం, ThreadState_Restart);
        SCHED_వరుసలో_చేర్చు(లక్ష్యం);
        possible_Switch_గమ్యం(లక్ష్యం);
#అయితే_ముగింపు
    }
}

శూన్య doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, మాట_t badge,
                   bool_t grant, tcb_t *receiver)
{
    శూన్య *receive_బఫరు, *send_బఫరు;

    receive_బఫరు = lookupIPCBuffer(true, receiver);

    ఒకవేళ (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        send_బఫరు = lookupIPCBuffer(false, sender);
        do_సాధారణ_Transfer(sender, send_బఫరు, endpoint, badge, grant,
                         receiver, receive_బఫరు);
    } లేకపోతే {
        doFaultTransfer(badge, sender, receiver, receive_బఫరు);
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య do_జవాబు_Transfer(tcb_t *sender, జవాబు_t *జవాబు, bool_t grant)
#లేకపోతే
శూన్య do_జవాబు_Transfer(tcb_t *sender, tcb_t *receiver, cte_t *స్థానం, bool_t grant)
#అయితే_ముగింపు
{
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (జవాబు->జవాబు_TCB == ఖాళీ ||
        thread_state_get_tsType(జవాబు->జవాబు_TCB->tcbState) != Thread_State_Blocked_On_జవాబు) {
        /* nothing to do */
        తిరిగి;
    }

    tcb_t *receiver = జవాబు->జవాబు_TCB;
    జవాబు_remove(జవాబు, receiver);
    ప్రతిజ్ఞా(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    ప్రతిజ్ఞా(జవాబు->జవాబు_TCB == ఖాళీ);

    ఒకవేళ (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != కణుపు_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#లేకపోతే
    ప్రతిజ్ఞా(thread_state_get_tsType(receiver->tcbState) ==
           Thread_State_Blocked_On_జవాబు);
#అయితే_ముగింపు

    మాట_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    ఒకవేళ (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, ఖాళీ, 0, grant, receiver);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        సమితి_Thread_State(receiver, ThreadState_Running);
#లేకపోతే
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(స్థానం);
        సమితి_Thread_State(receiver, ThreadState_Running);
        possible_Switch_గమ్యం(receiver);
#అయితే_ముగింపు
    } లేకపోతే {
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(స్థానం);
#అయితే_ముగింపు
        bool_t restart = handle_Fault_జవాబు(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        ఒకవేళ (restart) {
            సమితి_Thread_State(receiver, ThreadState_Restart);
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
            possible_Switch_గమ్యం(receiver);
#అయితే_ముగింపు
        } లేకపోతే {
            సమితి_Thread_State(receiver, ThreadState_Inactive);
        }
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        ఒకవేళ ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possible_Switch_గమ్యం(receiver);
        } లేకపోతే {
            ఒకవేళ (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } లేకపోతే {
                postpone(sc);
            }
        }
    }
#అయితే_ముగింపు
}

శూన్య do_సాధారణ_Transfer(tcb_t *sender, మాట_t *send_బఫరు, endpoint_t *endpoint,
                      మాట_t badge, bool_t canGrant, tcb_t *receiver,
                      మాట_t *receive_బఫరు)
{
    మాట_t సందేశం_Transferred;
    se_L4_సందేశం_Info_t ట్యాగ్;
    exception_t స్థితి_2;

    ట్యాగ్ = సందేశం_Info_From_మాట(get_రిజిస్టరు(sender, సందేశం_Info_రిజిస్టరు));

    ఒకవేళ (canGrant) {
        స్థితి_2 = lookupExtraCaps(sender, send_బఫరు, ట్యాగ్);
        ఒకవేళ (unlikely(స్థితి_2 != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = ఖాళీ;
        }
    } లేకపోతే {
        current_extra_caps.excaprefs[0] = ఖాళీ;
    }

    సందేశం_Transferred = copyMRs(sender, send_బఫరు, receiver, receive_బఫరు,
                             se_L4_సందేశం_Info_get_నిడివి(ట్యాగ్));

    ట్యాగ్ = transferCaps(ట్యాగ్, endpoint, receiver, receive_బఫరు);

    ట్యాగ్ = se_L4_సందేశం_Info_సమితి_నిడివి(ట్యాగ్, సందేశం_Transferred);
    సమితి_రిజిస్టరు(receiver, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(ట్యాగ్));
    సమితి_రిజిస్టరు(receiver, badge_రిజిస్టరు, badge);
}

శూన్య doFaultTransfer(మాట_t badge, tcb_t *sender, tcb_t *receiver,
                     మాట_t *receiverIPCBuffer)
{
    మాట_t పంపిన;
    se_L4_సందేశం_Info_t సందేశం_Info;

    పంపిన = సమితి_MRs_fault(sender, receiver, receiverIPCBuffer);
    సందేశం_Info = se_L4_సందేశం_Info_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, పంపిన);
    సమితి_రిజిస్టరు(receiver, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(సందేశం_Info));
    సమితి_రిజిస్టరు(receiver, badge_రిజిస్టరు, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
స్థిర se_L4_సందేశం_Info_t transferCaps(se_L4_సందేశం_Info_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       మాట_t *receive_బఫరు)
{
    మాట_t i;
    cte_t *dest_స్థానం;

    info = se_L4_సందేశం_Info_సమితి_extra_Caps(info, 0);
    info = se_L4_సందేశం_Info_సమితి_caps_Unwrapped(info, 0);

    ఒకవేళ (likely(!current_extra_caps.excaprefs[0] || !receive_బఫరు)) {
        తిరిగి info;
    }

    dest_స్థానం = getReceiveSlots(receiver, receive_బఫరు);

    కొరకు (i = 0; i < se_L4_సందేశం_Max_Extra_Caps && current_extra_caps.excaprefs[i] != ఖాళీ; i++) {
        cte_t *స్థానం = current_extra_caps.excaprefs[i];
        cap_t cap = స్థానం->cap;

        ఒకవేళ (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            సమితి_Extra_Badge(receive_బఫరు,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = se_L4_సందేశం_Info_సమితి_caps_Unwrapped(info,
                                                      se_L4_సందేశం_Info_get_caps_Unwrapped(info) | (1 << i));

        } లేకపోతే {
            deriveCap_ret_t dc_ret;

            ఒకవేళ (!dest_స్థానం) {
                ఆపు;
            }

            dc_ret = deriveCap(స్థానం, cap);

            ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
                ఆపు;
            }
            ఒకవేళ (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                ఆపు;
            }

            cte_చొప్పించు(dc_ret.cap, స్థానం, dest_స్థానం);

            dest_స్థానం = ఖాళీ;
        }
    }

    తిరిగి se_L4_సందేశం_Info_సమితి_extra_Caps(info, i);
}

శూన్య doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
}

శూన్య prepare_సమితి_Domain(tcb_t *tptr, dom_t dom)
{
    ఒకవేళ (ksCurDomain != dom) {
        Arch_prepare_సమితి_Domain(tptr, dom);
#నిర్వచితమైతే CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#అయితే_ముగింపు
    }
}

స్థిర శూన్య prepare_తదుపరి_Domain(శూన్య)
{
    Arch_prepare_తదుపరి_Domain();
#నిర్వచితమైతే CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(ఖాళీ);
#అయితే_ముగింపు
}

స్థిర శూన్య తదుపరి_Domain(శూన్య)
{
    ks_Dom_కాలపట్టిక_Idx++;
    ఒకవేళ (dschedule_is_చివర_marker(ks_Dom_కాలపట్టిక_Idx)) {
        ks_Dom_కాలపట్టిక_Idx = ks_Dom_కాలపట్టిక_ప్రారంభం;
    }
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    కణుపు_STATE(ksReprogram) = true;
#అయితే_ముగింపు
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ks_Dom_కాలపట్టిక[ks_Dom_కాలపట్టిక_Idx]);
    ks_Domain_సమయం = dschedule_duration(ks_Dom_కాలపట్టిక[ks_Dom_కాలపట్టిక_Idx]);
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
స్థిర శూన్య switchSchedContext(శూన్య)
{
    ఒకవేళ (unlikely(కణుపు_STATE(ksCurSC) != కణుపు_STATE(ksCurThread)->tcbSchedContext)) {
        కణుపు_STATE(ksReprogram) = true;
        ఒకవేళ (sc_constant_bandwidth(కణుపు_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(కణుపు_STATE(ksCurThread)->tcbSchedContext);
        }

        ప్రతిజ్ఞా(refill_ready(కణుపు_STATE(ksCurThread)->tcbSchedContext));
        ప్రతిజ్ఞా(refill_sufficient(కణుపు_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    ఒకవేళ (కణుపు_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commit_సమయం();
    }

    కణుపు_STATE(ksCurSC) = కణుపు_STATE(ksCurThread)->tcbSchedContext;
}
#అయితే_ముగింపు

స్థిర శూన్య కాలపట్టిక_ఎంచుకో_New_Thread(శూన్య)
{
    ఒకవేళ (ks_Domain_సమయం == 0) {
        prepare_తదుపరి_Domain();
        తదుపరి_Domain();
    }
    ఎంచుకో_Thread();
}

శూన్య కాలపట్టిక(శూన్య)
{
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    awaken();
    check_Domain_సమయం();
#అయితే_ముగింపు

    ఒకవేళ (కణుపు_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        ఒకవేళ (isSchedulable(కణుపు_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_వరుసలో_చేర్చు_CURRENT_TCB;
        } లేకపోతే {
            was_runnable = false;
        }

        ఒకవేళ (కణుపు_STATE(ksSchedulerAction) == Scheduler_Action_ఎంచుకో_New_Thread) {
            కాలపట్టిక_ఎంచుకో_New_Thread();
        } లేకపోతే {
            tcb_t *candidate = కణుపు_STATE(ksSchedulerAction);
            ప్రతిజ్ఞా(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                కణుపు_STATE(ksCurThread) == కణుపు_STATE(ksIdleThread)
                || (candidate->tcb_ప్రాధాన్యత < కణుపు_STATE(ksCurThread)->tcb_ప్రాధాన్యత);
            ఒకవేళ (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcb_ప్రాధాన్యత)) {
                SCHED_వరుసలో_చేర్చు(candidate);
                /* we can't, need to reschedule */
                కణుపు_STATE(ksSchedulerAction) = Scheduler_Action_ఎంచుకో_New_Thread;
                కాలపట్టిక_ఎంచుకో_New_Thread();
            } లేకపోతే ఒకవేళ (was_runnable && candidate->tcb_ప్రాధాన్యత == కణుపు_STATE(ksCurThread)->tcb_ప్రాధాన్యత) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                కణుపు_STATE(ksSchedulerAction) = Scheduler_Action_ఎంచుకో_New_Thread;
                కాలపట్టిక_ఎంచుకో_New_Thread();
            } లేకపోతే {
                ప్రతిజ్ఞా(candidate != కణుపు_STATE(ksCurThread));
                switch_గమ్యం_Thread(candidate);
            }
        }
    }
    కణుపు_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    do_ముసుగు_Reschedule(ARCH_కణుపు_STATE(ipiReschedulePending));
    ARCH_కణుపు_STATE(ipiReschedulePending) = 0;
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    switchSchedContext();

    ఒకవేళ (కణుపు_STATE(ksReprogram)) {
        సమితి_తదుపరి_అంతరాయం();
        కణుపు_STATE(ksReprogram) = false;
    }
#అయితే_ముగింపు
}

శూన్య ఎంచుకో_Thread(శూన్య)
{
    మాట_t prio;
    మాట_t dom;
    tcb_t *thread;

    ఒకవేళ (అంకె_Domains > 1) {
        dom = ksCurDomain;
    } లేకపోతే {
        dom = 0;
    }

    ఒకవేళ (likely(కణుపు_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = కణుపు_STATE(ksReadyQueues)[ready_queues_సూచీ(dom, prio)].తల;
        ప్రతిజ్ఞా(thread);
        ప్రతిజ్ఞా(isSchedulable(thread));
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        ప్రతిజ్ఞా(refill_sufficient(thread->tcbSchedContext, 0));
        ప్రతిజ్ఞా(refill_ready(thread->tcbSchedContext));
#అయితే_ముగింపు
        switch_గమ్యం_Thread(thread);
    } లేకపోతే {
        switch_గమ్యం_Idle_Thread();
    }
}

శూన్య switch_గమ్యం_Thread(tcb_t *thread)
{
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ప్రతిజ్ఞా(thread->tcbSchedContext != ఖాళీ);
    ప్రతిజ్ఞా(!thread_state_get_tcb_లోపల_Release_వరుస(thread->tcbState));
    ప్రతిజ్ఞా(refill_sufficient(thread->tcbSchedContext, 0));
    ప్రతిజ్ఞా(refill_ready(thread->tcbSchedContext));
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(కణుపు_STATE(ksCurThread), thread);
#అయితే_ముగింపు
    Arch_switch_గమ్యం_Thread(thread);

#నిర్వచితమైతే CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#అయితే_ముగింపు /* CONFIG_HAVE_FPU */

    tcb_Sched_వరుస_నుండి_తీయి(thread);
    కణుపు_STATE(ksCurThread) = thread;
}

శూన్య switch_గమ్యం_Idle_Thread(శూన్య)
{
#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(కణుపు_STATE(ksCurThread), కణుపు_STATE(ksIdleThread));
#అయితే_ముగింపు
    Arch_switch_గమ్యం_Idle_Thread();
    కణుపు_STATE(ksCurThread) = కణుపు_STATE(ksIdleThread);
}

శూన్య సమితి_Domain(tcb_t *tptr, dom_t dom)
{
    tcb_Sched_వరుస_నుండి_తీయి(tptr);
    tptr->tcbDomain = dom;
    ఒకవేళ (isSchedulable(tptr)) {
        SCHED_వరుసలో_చేర్చు(tptr);
    }
    ఒకవేళ (tptr == కణుపు_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

శూన్య సమితి_MCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య సమితి_ప్రాధాన్యత(tcb_t *tptr, prio_t prio)
{
    ఎంపిక (thread_state_get_tsType(tptr->tcbState)) {
    స్థితి ThreadState_Running:
    స్థితి ThreadState_Restart:
        ఒకవేళ (thread_state_get_tcbQueued(tptr->tcbState) || tptr == కణుపు_STATE(ksCurThread)) {
            tcb_Sched_వరుస_నుండి_తీయి(tptr);
            tptr->tcb_ప్రాధాన్యత = prio;
            SCHED_వరుసలో_చేర్చు(tptr);
            rescheduleRequired();
        } లేకపోతే {
            tptr->tcb_ప్రాధాన్యత = prio;
        }
        ఆపు;
    స్థితి ThreadState_BlockedOnReceive:
    స్థితి ThreadState_BlockedOnSend:
        tptr->tcb_ప్రాధాన్యత = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ఆపు;
    స్థితి ThreadState_BlockedOnNotification:
        tptr->tcb_ప్రాధాన్యత = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ఆపు;
    అప్రమేయ:
        tptr->tcb_ప్రాధాన్యత = prio;
        ఆపు;
    }
}
#లేకపోతే
శూన్య సమితి_ప్రాధాన్యత(tcb_t *tptr, prio_t prio)
{
    tcb_Sched_వరుస_నుండి_తీయి(tptr);
    tptr->tcb_ప్రాధాన్యత = prio;
    ఒకవేళ (isRunnable(tptr)) {
        ఒకవేళ (tptr == కణుపు_STATE(ksCurThread)) {
            rescheduleRequired();
        } లేకపోతే {
            possible_Switch_గమ్యం(tptr);
        }
    }
}
#అయితే_ముగింపు

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
శూన్య possible_Switch_గమ్యం(tcb_t *లక్ష్యం)
{
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (లక్ష్యం->tcbSchedContext != ఖాళీ && !thread_state_get_tcb_లోపల_Release_వరుస(లక్ష్యం->tcbState)) {
#అయితే_ముగింపు
        ఒకవేళ (ksCurDomain != లక్ష్యం->tcbDomain
            SMP_COND_STATEMENT( || లక్ష్యం->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_వరుసలో_చేర్చు(లక్ష్యం);
        } లేకపోతే ఒకవేళ (కణుపు_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_వరుసలో_చేర్చు(లక్ష్యం);
        } లేకపోతే {
            కణుపు_STATE(ksSchedulerAction) = లక్ష్యం;
        }
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    }
#అయితే_ముగింపు

}

శూన్య సమితి_Thread_State(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_సమితి_ts_Type(&tptr->tcbState, ts);
    కాలపట్టిక_TCB(tptr);
}

శూన్య కాలపట్టిక_TCB(tcb_t *tptr)
{
    ఒకవేళ (tptr == కణుపు_STATE(ksCurThread) &&
        కణుపు_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        కణుపు_STATE(ksSchedulerAction) = Scheduler_Action_ఎంచుకో_New_Thread;
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ప్రతిజ్ఞా(tcb != ఖాళీ);

    tcb_Sched_వరుస_నుండి_తీయి(tcb);
    tcb_Release_వరుసలో_చేర్చు(tcb);
    కణుపు_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

శూన్య సమితి_తదుపరి_అంతరాయం(శూన్య)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_తల_refill = *refill_తల(కణుపు_STATE(ksCurThread)->tcbSchedContext);
    ticks_t తదుపరి_అంతరాయం = కణుపు_STATE(ks_Cur_సమయం) + ct_తల_refill.rAmount;

    ఒకవేళ (అంకె_Domains > 1) {
        తదుపరి_అంతరాయం = MIN(తదుపరి_అంతరాయం, కణుపు_STATE(ks_Cur_సమయం) + ks_Domain_సమయం);
    }

    tcb_t *rlq_తల = కణుపు_STATE(ks_Release_వరుస.తల);
    ఒకవేళ (rlq_తల != ఖాళీ) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_తల_refill = *refill_తల(rlq_తల->tcbSchedContext);
        తదుపరి_అంతరాయం = MIN(rlq_తల_refill.r_సమయం, తదుపరి_అంతరాయం);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    ప్రతిజ్ఞా(తదుపరి_అంతరాయం >= కణుపు_STATE(ks_Cur_సమయం));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    సమితి_Deadline(తదుపరి_అంతరాయం - getTimerPrecision());
}

శూన్య chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    ఒకవేళ (likely(కణుపు_STATE(ksCurSC) != కణుపు_STATE(ksIdleSC))) {
        ఒకవేళ (is_విడత_Robin(కణుపు_STATE(ksCurSC))) {
            ప్రతిజ్ఞా(refill_కొలత(కణుపు_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t తల = *refill_తల(కణుపు_STATE(ksCurSC));
            refill_t తోక = *refill_తోక(కణుపు_STATE(ksCurSC));
            refill_తల(కణుపు_STATE(ksCurSC))->rAmount = తల.rAmount + తోక.rAmount;
            refill_తోక(కణుపు_STATE(ksCurSC))->rAmount = 0;
        } లేకపోతే {
            refill_budget_check(consumed);
        }

        ప్రతిజ్ఞా(refill_తల(కణుపు_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        కణుపు_STATE(ksCurSC)->scConsumed += consumed;
    }
    కణుపు_STATE(ksConsumed) = 0;
    ఒకవేళ (likely(isSchedulable(కణుపు_STATE(ksCurThread)))) {
        ప్రతిజ్ఞా(కణుపు_STATE(ksCurThread)->tcbSchedContext == కణుపు_STATE(ksCurSC));
        చివర_Timeslice(canTimeoutFault);
        rescheduleRequired();
        కణుపు_STATE(ksReprogram) = true;
    }
}

శూన్య చివర_Timeslice(bool_t can_timeout_fault)
{
    bool_t విడత_robin = is_విడత_Robin(కణుపు_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(కణుపు_STATE(ksCurThread));

    ఒకవేళ (can_timeout_fault && !విడత_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(కణుపు_STATE(ksCurSC)->scBadge);
        handleTimeout(కణుపు_STATE(ksCurThread));
    } లేకపోతే ఒకవేళ (refill_ready(కణుపు_STATE(ksCurSC)) && refill_sufficient(కణుపు_STATE(ksCurSC), 0)) {
        /* apply round robin */
        ప్రతిజ్ఞా(!thread_state_get_tcbQueued(కణుపు_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } లేకపోతే {
        /* postpone until ready */
        postpone(కణుపు_STATE(ksCurSC));
    }
}
#లేకపోతే

శూన్య timerTick(శూన్య)
{
    ఒకవేళ (likely(thread_state_get_tsType(కణుపు_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#నిర్వచితమైతే CONFIG_VTX
        || thread_state_get_tsType(కణుపు_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#అయితే_ముగింపు
       ) {
        ఒకవేళ (కణుపు_STATE(ksCurThread)->tcb_సమయం_ముక్క > 1) {
            కణుపు_STATE(ksCurThread)->tcb_సమయం_ముక్క--;
        } లేకపోతే {
            కణుపు_STATE(ksCurThread)->tcb_సమయం_ముక్క = CONFIG_సమయం_ముక్క;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    ఒకవేళ (అంకె_Domains > 1) {
        ks_Domain_సమయం--;
        ఒకవేళ (ks_Domain_సమయం == 0) {
            rescheduleRequired();
        }
    }
}
#అయితే_ముగింపు

శూన్య rescheduleRequired(శూన్య)
{
    ఒకవేళ (కణుపు_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && కణుపు_STATE(ksSchedulerAction) != Scheduler_Action_ఎంచుకో_New_Thread
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        && isSchedulable(కణుపు_STATE(ksSchedulerAction))
#అయితే_ముగింపు
       ) {
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        ప్రతిజ్ఞా(refill_sufficient(కణుపు_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        ప్రతిజ్ఞా(refill_ready(కణుపు_STATE(ksSchedulerAction)->tcbSchedContext));
#అయితే_ముగింపు
        SCHED_వరుసలో_చేర్చు(కణుపు_STATE(ksSchedulerAction));
    }
    కణుపు_STATE(ksSchedulerAction) = Scheduler_Action_ఎంచుకో_New_Thread;
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS

స్థిర అంతఃవరుస bool_t శుద్ధ release_q_non_ఖాళీలు_and_ready(శూన్య)
{
    తిరిగి కణుపు_STATE(ks_Release_వరుస.తల) != ఖాళీ
           && refill_ready(కణుపు_STATE(ks_Release_వరుస.తల)->tcbSchedContext);
}

స్థిర శూన్య tcb_Release_వరుస_నుండి_తీయి(శూన్య)
{
    ప్రతిజ్ఞా(కణుపు_STATE(ks_Release_వరుస.తల) != ఖాళీ);
    ప్రతిజ్ఞా(కణుపు_STATE(ks_Release_వరుస.తల)->tcbSchedPrev == ఖాళీ);
    SMP_COND_STATEMENT(ప్రతిజ్ఞా(కణుపు_STATE(ks_Release_వరుస.తల)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = కణుపు_STATE(ks_Release_వరుస.తల);
    ప్రతిజ్ఞా(awakened != కణుపు_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    ప్రతిజ్ఞా(!is_విడత_Robin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(ప్రతిజ్ఞా(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    ప్రతిజ్ఞా(refill_sufficient(awakened->tcbSchedContext, 0));
    possible_Switch_గమ్యం(awakened);
}

శూన్య awaken(శూన్య)
{
    అయితే (unlikely(release_q_non_ఖాళీలు_and_ready())) {
        tcb_Release_వరుస_నుండి_తీయి();
    }
}
#అయితే_ముగింపు
