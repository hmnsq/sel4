/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <object.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <api/faults.h>
#अन्तर्भाव <api/types.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <object/domain.h>
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
#अन्तर्भाव <object/schedcontext.h>
#यदि_अन्तः
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/kernel/thread.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <linker.h>

स्थिर se_L4_सन्देशः_Info_t
transferCaps(se_L4_सन्देशः_Info_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receive_बफर);

BOOT_संहिता शून्य configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    समुच्चयः_Thread_State(tcb, ThreadState_IdleThreadState);
}

शून्य activateThread(शून्य)
{
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (unlikely(ग्रन्थिः_STATE(ksCurThread)->tcb_Yield_गन्तव्यम्)) {
        sched_Context_complete_Yield_गन्तव्यम्(ग्रन्थिः_STATE(ksCurThread));
        प्रतिज्ञा(thread_state_get_tsType(ग्रन्थिः_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#यदि_अन्तः

    विकल्प (thread_state_get_tsType(ग्रन्थिः_STATE(ksCurThread)->tcbState)) {
    पक्ष ThreadState_Running:
#यदि_परिभाषितम् CONFIG_VTX
    पक्ष ThreadState_RunningVM:
#यदि_अन्तः
        भङ्ग;

    पक्ष ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(ग्रन्थिः_STATE(ksCurThread));
        समुच्चयः_अग्र_PC(ग्रन्थिः_STATE(ksCurThread), pc);
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
        भङ्ग;
    }

    पक्ष ThreadState_IdleThreadState:
        Arch_activateIdleThread(ग्रन्थिः_STATE(ksCurThread));
        भङ्ग;

    स्वभाव:
        fail("Current thread is blocked");
    }
}

शून्य suspend(tcb_t *target)
{
    cancelIPC(target);
    यदि (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
        /* whilst in the running state it is possible that restart pc of a thread is
         * incorrect. As we do not know what state this thread will transition to
         * after we make it inactive we update its restart pc so that the thread next
         * runs at the correct address whether it is restarted or moved directly to
         * running */
        updateRestartPC(target);
    }
    tcb_Sched_पङ्क्तेः_निष्कासय(target);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    tcbReleaseRemove(target);
    sched_Context_cancel_Yield_गन्तव्यम्(target);
#यदि_अन्तः
    समुच्चयः_Thread_State(target, ThreadState_Inactive);
}

शून्य restart(tcb_t *target)
{
    यदि (isStopped(target)) {
        cancelIPC(target);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        समुच्चयः_Thread_State(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        यदि (sc_sporadic(sc) && sc != ग्रन्थिः_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        यदि (isSchedulable(target)) {
            possible_Switch_गन्तव्यम्(target);
        }
#अन्यथा
        सज्जता_प्रत्युत्तरम्_Master(target);
        समुच्चयः_Thread_State(target, ThreadState_Restart);
        SCHED_पङ्क्तौ_निक्षिप(target);
        possible_Switch_गन्तव्यम्(target);
#यदि_अन्तः
    }
}

शून्य doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    शून्य *receive_बफर, *send_बफर;

    receive_बफर = lookupIPCBuffer(true, receiver);

    यदि (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        send_बफर = lookupIPCBuffer(false, sender);
        do_प्रसामान्यम्_Transfer(sender, send_बफर, endpoint, badge, grant,
                         receiver, receive_बफर);
    } अन्यथा {
        doFaultTransfer(badge, sender, receiver, receive_बफर);
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य do_प्रत्युत्तरम्_Transfer(tcb_t *sender, प्रत्युत्तरम्_t *प्रत्युत्तरम्, bool_t grant)
#अन्यथा
शून्य do_प्रत्युत्तरम्_Transfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#यदि_अन्तः
{
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (प्रत्युत्तरम्->प्रत्युत्तरम्_TCB == रिक्त ||
        thread_state_get_tsType(प्रत्युत्तरम्->प्रत्युत्तरम्_TCB->tcbState) != Thread_State_Blocked_On_प्रत्युत्तरम्) {
        /* nothing to do */
        प्रत्यागम;
    }

    tcb_t *receiver = प्रत्युत्तरम्->प्रत्युत्तरम्_TCB;
    प्रत्युत्तरम्_remove(प्रत्युत्तरम्, receiver);
    प्रतिज्ञा(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    प्रतिज्ञा(प्रत्युत्तरम्->प्रत्युत्तरम्_TCB == रिक्त);

    यदि (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != ग्रन्थिः_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#अन्यथा
    प्रतिज्ञा(thread_state_get_tsType(receiver->tcbState) ==
           Thread_State_Blocked_On_प्रत्युत्तरम्);
#यदि_अन्तः

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    यदि (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, रिक्त, 0, grant, receiver);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        समुच्चयः_Thread_State(receiver, ThreadState_Running);
#अन्यथा
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        समुच्चयः_Thread_State(receiver, ThreadState_Running);
        possible_Switch_गन्तव्यम्(receiver);
#यदि_अन्तः
    } अन्यथा {
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#यदि_अन्तः
        bool_t restart = handle_Fault_प्रत्युत्तरम्(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        यदि (restart) {
            समुच्चयः_Thread_State(receiver, ThreadState_Restart);
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
            possible_Switch_गन्तव्यम्(receiver);
#यदि_अन्तः
        } अन्यथा {
            समुच्चयः_Thread_State(receiver, ThreadState_Inactive);
        }
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        यदि ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possible_Switch_गन्तव्यम्(receiver);
        } अन्यथा {
            यदि (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } अन्यथा {
                postpone(sc);
            }
        }
    }
#यदि_अन्तः
}

शून्य do_प्रसामान्यम्_Transfer(tcb_t *sender, word_t *send_बफर, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receive_बफर)
{
    word_t सन्देशः_Transferred;
    se_L4_सन्देशः_Info_t चिह्नकम्;
    exception_t स्थितिः;

    चिह्नकम् = सन्देशः_Info_From_Word(get_पञ्जिकाकोशः(sender, सन्देशः_Info_पञ्जिकाकोशः));

    यदि (canGrant) {
        स्थितिः = lookupExtraCaps(sender, send_बफर, चिह्नकम्);
        यदि (unlikely(स्थितिः != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = रिक्त;
        }
    } अन्यथा {
        current_extra_caps.excaprefs[0] = रिक्त;
    }

    सन्देशः_Transferred = copyMRs(sender, send_बफर, receiver, receive_बफर,
                             se_L4_सन्देशः_Info_get_दीर्घता(चिह्नकम्));

    चिह्नकम् = transferCaps(चिह्नकम्, endpoint, receiver, receive_बफर);

    चिह्नकम् = se_L4_सन्देशः_Info_समुच्चयः_दीर्घता(चिह्नकम्, सन्देशः_Transferred);
    समुच्चयः_पञ्जिकाकोशः(receiver, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(चिह्नकम्));
    समुच्चयः_पञ्जिकाकोशः(receiver, badge_पञ्जिकाकोशः, badge);
}

शून्य doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t प्रेषितम्;
    se_L4_सन्देशः_Info_t सन्देशः_Info;

    प्रेषितम् = समुच्चयः_MRs_fault(sender, receiver, receiverIPCBuffer);
    सन्देशः_Info = se_L4_सन्देशः_Info_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, प्रेषितम्);
    समुच्चयः_पञ्जिकाकोशः(receiver, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(सन्देशः_Info));
    समुच्चयः_पञ्जिकाकोशः(receiver, badge_पञ्जिकाकोशः, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
स्थिर se_L4_सन्देशः_Info_t transferCaps(se_L4_सन्देशः_Info_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receive_बफर)
{
    word_t i;
    cte_t *destSlot;

    info = se_L4_सन्देशः_Info_समुच्चयः_extra_Caps(info, 0);
    info = se_L4_सन्देशः_Info_समुच्चयः_caps_Unwrapped(info, 0);

    यदि (likely(!current_extra_caps.excaprefs[0] || !receive_बफर)) {
        प्रत्यागम info;
    }

    destSlot = getReceiveSlots(receiver, receive_बफर);

    प्रति (i = 0; i < se_L4_सन्देशः_Max_Extra_Caps && current_extra_caps.excaprefs[i] != रिक्त; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        यदि (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            समुच्चयः_Extra_Badge(receive_बफर,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = se_L4_सन्देशः_Info_समुच्चयः_caps_Unwrapped(info,
                                                      se_L4_सन्देशः_Info_get_caps_Unwrapped(info) | (1 << i));

        } अन्यथा {
            deriveCap_ret_t dc_ret;

            यदि (!destSlot) {
                भङ्ग;
            }

            dc_ret = deriveCap(slot, cap);

            यदि (dc_ret.स्थितिः != EXCEPTION_NONE) {
                भङ्ग;
            }
            यदि (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                भङ्ग;
            }

            cte_निवेशय(dc_ret.cap, slot, destSlot);

            destSlot = रिक्त;
        }
    }

    प्रत्यागम se_L4_सन्देशः_Info_समुच्चयः_extra_Caps(info, i);
}

शून्य doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
}

शून्य prepare_समुच्चयः_Domain(tcb_t *tptr, dom_t dom)
{
    यदि (ksCurDomain != dom) {
        Arch_prepare_समुच्चयः_Domain(tptr, dom);
#यदि_परिभाषितम् CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#यदि_अन्तः
    }
}

स्थिर शून्य prepare_अग्र_Domain(शून्य)
{
    Arch_prepare_अग्र_Domain();
#यदि_परिभाषितम् CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(रिक्त);
#यदि_अन्तः
}

स्थिर शून्य अग्र_Domain(शून्य)
{
    ks_Dom_कालसारणी_Idx++;
    यदि (dschedule_is_अन्तः_marker(ks_Dom_कालसारणी_Idx)) {
        ks_Dom_कालसारणी_Idx = ks_Dom_कालसारणी_Start;
    }
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    ग्रन्थिः_STATE(ksReprogram) = true;
#यदि_अन्तः
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ks_Dom_कालसारणी[ks_Dom_कालसारणी_Idx]);
    ksDomainTime = dschedule_duration(ks_Dom_कालसारणी[ks_Dom_कालसारणी_Idx]);
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
स्थिर शून्य switchSchedContext(शून्य)
{
    यदि (unlikely(ग्रन्थिः_STATE(ksCurSC) != ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext)) {
        ग्रन्थिः_STATE(ksReprogram) = true;
        यदि (sc_constant_bandwidth(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext);
        }

        प्रतिज्ञा(refill_ready(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext));
        प्रतिज्ञा(refill_sufficient(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    यदि (ग्रन्थिः_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    ग्रन्थिः_STATE(ksCurSC) = ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext;
}
#यदि_अन्तः

स्थिर शून्य कालसारणी_वृणु_New_Thread(शून्य)
{
    यदि (ksDomainTime == 0) {
        prepare_अग्र_Domain();
        अग्र_Domain();
    }
    वृणु_Thread();
}

शून्य कालसारणी(शून्य)
{
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#यदि_अन्तः

    यदि (ग्रन्थिः_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        यदि (isSchedulable(ग्रन्थिः_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_पङ्क्तौ_निक्षिप_CURRENT_TCB;
        } अन्यथा {
            was_runnable = false;
        }

        यदि (ग्रन्थिः_STATE(ksSchedulerAction) == Scheduler_Action_वृणु_New_Thread) {
            कालसारणी_वृणु_New_Thread();
        } अन्यथा {
            tcb_t *candidate = ग्रन्थिः_STATE(ksSchedulerAction);
            प्रतिज्ञा(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                ग्रन्थिः_STATE(ksCurThread) == ग्रन्थिः_STATE(ksIdleThread)
                || (candidate->tcb_प्राथमिकता < ग्रन्थिः_STATE(ksCurThread)->tcb_प्राथमिकता);
            यदि (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcb_प्राथमिकता)) {
                SCHED_पङ्क्तौ_निक्षिप(candidate);
                /* we can't, need to reschedule */
                ग्रन्थिः_STATE(ksSchedulerAction) = Scheduler_Action_वृणु_New_Thread;
                कालसारणी_वृणु_New_Thread();
            } अन्यथा यदि (was_runnable && candidate->tcb_प्राथमिकता == ग्रन्थिः_STATE(ksCurThread)->tcb_प्राथमिकता) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                ग्रन्थिः_STATE(ksSchedulerAction) = Scheduler_Action_वृणु_New_Thread;
                कालसारणी_वृणु_New_Thread();
            } अन्यथा {
                प्रतिज्ञा(candidate != ग्रन्थिः_STATE(ksCurThread));
                switch_गन्तव्यम्_Thread(candidate);
            }
        }
    }
    ग्रन्थिः_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    do_आवरणम्_Reschedule(ARCH_ग्रन्थिः_STATE(ipiReschedulePending));
    ARCH_ग्रन्थिः_STATE(ipiReschedulePending) = 0;
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    switchSchedContext();

    यदि (ग्रन्थिः_STATE(ksReprogram)) {
        समुच्चयः_अग्र_व्यवधानम्();
        ग्रन्थिः_STATE(ksReprogram) = false;
    }
#यदि_अन्तः
}

शून्य वृणु_Thread(शून्य)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    यदि (अङ्कः_Domains > 1) {
        dom = ksCurDomain;
    } अन्यथा {
        dom = 0;
    }

    यदि (likely(ग्रन्थिः_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = ग्रन्थिः_STATE(ksReadyQueues)[ready_queues_सूचकांक(dom, prio)].शीर्ष;
        प्रतिज्ञा(thread);
        प्रतिज्ञा(isSchedulable(thread));
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        प्रतिज्ञा(refill_sufficient(thread->tcbSchedContext, 0));
        प्रतिज्ञा(refill_ready(thread->tcbSchedContext));
#यदि_अन्तः
        switch_गन्तव्यम्_Thread(thread);
    } अन्यथा {
        switch_गन्तव्यम्_Idle_Thread();
    }
}

शून्य switch_गन्तव्यम्_Thread(tcb_t *thread)
{
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रतिज्ञा(thread->tcbSchedContext != रिक्त);
    प्रतिज्ञा(!thread_state_get_tcb_अन्तर्_Release_Queue(thread->tcbState));
    प्रतिज्ञा(refill_sufficient(thread->tcbSchedContext, 0));
    प्रतिज्ञा(refill_ready(thread->tcbSchedContext));
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ग्रन्थिः_STATE(ksCurThread), thread);
#यदि_अन्तः
    Arch_switch_गन्तव्यम्_Thread(thread);

#यदि_परिभाषितम् CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#यदि_अन्तः /* CONFIG_HAVE_FPU */

    tcb_Sched_पङ्क्तेः_निष्कासय(thread);
    ग्रन्थिः_STATE(ksCurThread) = thread;
}

शून्य switch_गन्तव्यम्_Idle_Thread(शून्य)
{
#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ग्रन्थिः_STATE(ksCurThread), ग्रन्थिः_STATE(ksIdleThread));
#यदि_अन्तः
    Arch_switch_गन्तव्यम्_Idle_Thread();
    ग्रन्थिः_STATE(ksCurThread) = ग्रन्थिः_STATE(ksIdleThread);
}

शून्य समुच्चयः_Domain(tcb_t *tptr, dom_t dom)
{
    tcb_Sched_पङ्क्तेः_निष्कासय(tptr);
    tptr->tcbDomain = dom;
    यदि (isSchedulable(tptr)) {
        SCHED_पङ्क्तौ_निक्षिप(tptr);
    }
    यदि (tptr == ग्रन्थिः_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

शून्य समुच्चयः_MCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य समुच्चयः_प्राथमिकता(tcb_t *tptr, prio_t prio)
{
    विकल्प (thread_state_get_tsType(tptr->tcbState)) {
    पक्ष ThreadState_Running:
    पक्ष ThreadState_Restart:
        यदि (thread_state_get_tcbQueued(tptr->tcbState) || tptr == ग्रन्थिः_STATE(ksCurThread)) {
            tcb_Sched_पङ्क्तेः_निष्कासय(tptr);
            tptr->tcb_प्राथमिकता = prio;
            SCHED_पङ्क्तौ_निक्षिप(tptr);
            rescheduleRequired();
        } अन्यथा {
            tptr->tcb_प्राथमिकता = prio;
        }
        भङ्ग;
    पक्ष ThreadState_BlockedOnReceive:
    पक्ष ThreadState_BlockedOnSend:
        tptr->tcb_प्राथमिकता = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        भङ्ग;
    पक्ष ThreadState_BlockedOnNotification:
        tptr->tcb_प्राथमिकता = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        भङ्ग;
    स्वभाव:
        tptr->tcb_प्राथमिकता = prio;
        भङ्ग;
    }
}
#अन्यथा
शून्य समुच्चयः_प्राथमिकता(tcb_t *tptr, prio_t prio)
{
    tcb_Sched_पङ्क्तेः_निष्कासय(tptr);
    tptr->tcb_प्राथमिकता = prio;
    यदि (isRunnable(tptr)) {
        यदि (tptr == ग्रन्थिः_STATE(ksCurThread)) {
            rescheduleRequired();
        } अन्यथा {
            possible_Switch_गन्तव्यम्(tptr);
        }
    }
}
#यदि_अन्तः

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
शून्य possible_Switch_गन्तव्यम्(tcb_t *target)
{
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (target->tcbSchedContext != रिक्त && !thread_state_get_tcb_अन्तर्_Release_Queue(target->tcbState)) {
#यदि_अन्तः
        यदि (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_पङ्क्तौ_निक्षिप(target);
        } अन्यथा यदि (ग्रन्थिः_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_पङ्क्तौ_निक्षिप(target);
        } अन्यथा {
            ग्रन्थिः_STATE(ksSchedulerAction) = target;
        }
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    }
#यदि_अन्तः

}

शून्य समुच्चयः_Thread_State(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_समुच्चयः_ts_Type(&tptr->tcbState, ts);
    कालसारणी_TCB(tptr);
}

शून्य कालसारणी_TCB(tcb_t *tptr)
{
    यदि (tptr == ग्रन्थिः_STATE(ksCurThread) &&
        ग्रन्थिः_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        ग्रन्थिः_STATE(ksSchedulerAction) = Scheduler_Action_वृणु_New_Thread;
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    प्रतिज्ञा(tcb != रिक्त);

    tcb_Sched_पङ्क्तेः_निष्कासय(tcb);
    tcb_Release_पङ्क्तौ_निक्षिप(tcb);
    ग्रन्थिः_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

शून्य समुच्चयः_अग्र_व्यवधानम्(शून्य)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_शीर्ष_refill = *refill_शीर्ष(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext);
    ticks_t अग्र_व्यवधानम् = ग्रन्थिः_STATE(ksCurTime) + ct_शीर्ष_refill.rAmount;

    यदि (अङ्कः_Domains > 1) {
        अग्र_व्यवधानम् = MIN(अग्र_व्यवधानम्, ग्रन्थिः_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_शीर्ष = ग्रन्थिः_STATE(ksReleaseQueue.शीर्ष);
    यदि (rlq_शीर्ष != रिक्त) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_शीर्ष_refill = *refill_शीर्ष(rlq_शीर्ष->tcbSchedContext);
        अग्र_व्यवधानम् = MIN(rlq_शीर्ष_refill.rTime, अग्र_व्यवधानम्);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    प्रतिज्ञा(अग्र_व्यवधानम् >= ग्रन्थिः_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    समुच्चयः_Deadline(अग्र_व्यवधानम् - getTimerPrecision());
}

शून्य chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    यदि (likely(ग्रन्थिः_STATE(ksCurSC) != ग्रन्थिः_STATE(ksIdleSC))) {
        यदि (is_आवर्तनम्_Robin(ग्रन्थिः_STATE(ksCurSC))) {
            प्रतिज्ञा(refill_आकार(ग्रन्थिः_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t शीर्ष = *refill_शीर्ष(ग्रन्थिः_STATE(ksCurSC));
            refill_t पुच्छ = *refill_पुच्छ(ग्रन्थिः_STATE(ksCurSC));
            refill_शीर्ष(ग्रन्थिः_STATE(ksCurSC))->rAmount = शीर्ष.rAmount + पुच्छ.rAmount;
            refill_पुच्छ(ग्रन्थिः_STATE(ksCurSC))->rAmount = 0;
        } अन्यथा {
            refill_budget_check(consumed);
        }

        प्रतिज्ञा(refill_शीर्ष(ग्रन्थिः_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        ग्रन्थिः_STATE(ksCurSC)->scConsumed += consumed;
    }
    ग्रन्थिः_STATE(ksConsumed) = 0;
    यदि (likely(isSchedulable(ग्रन्थिः_STATE(ksCurThread)))) {
        प्रतिज्ञा(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext == ग्रन्थिः_STATE(ksCurSC));
        अन्तः_Timeslice(canTimeoutFault);
        rescheduleRequired();
        ग्रन्थिः_STATE(ksReprogram) = true;
    }
}

शून्य अन्तः_Timeslice(bool_t can_timeout_fault)
{
    bool_t आवर्तनम्_robin = is_आवर्तनम्_Robin(ग्रन्थिः_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(ग्रन्थिः_STATE(ksCurThread));

    यदि (can_timeout_fault && !आवर्तनम्_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(ग्रन्थिः_STATE(ksCurSC)->scBadge);
        handleTimeout(ग्रन्थिः_STATE(ksCurThread));
    } अन्यथा यदि (refill_ready(ग्रन्थिः_STATE(ksCurSC)) && refill_sufficient(ग्रन्थिः_STATE(ksCurSC), 0)) {
        /* apply round robin */
        प्रतिज्ञा(!thread_state_get_tcbQueued(ग्रन्थिः_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } अन्यथा {
        /* postpone until ready */
        postpone(ग्रन्थिः_STATE(ksCurSC));
    }
}
#अन्यथा

शून्य timerTick(शून्य)
{
    यदि (likely(thread_state_get_tsType(ग्रन्थिः_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#यदि_परिभाषितम् CONFIG_VTX
        || thread_state_get_tsType(ग्रन्थिः_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#यदि_अन्तः
       ) {
        यदि (ग्रन्थिः_STATE(ksCurThread)->tcb_Time_खण्डिका > 1) {
            ग्रन्थिः_STATE(ksCurThread)->tcb_Time_खण्डिका--;
        } अन्यथा {
            ग्रन्थिः_STATE(ksCurThread)->tcb_Time_खण्डिका = CONFIG_TIME_खण्डिका;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    यदि (अङ्कः_Domains > 1) {
        ksDomainTime--;
        यदि (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#यदि_अन्तः

शून्य rescheduleRequired(शून्य)
{
    यदि (ग्रन्थिः_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && ग्रन्थिः_STATE(ksSchedulerAction) != Scheduler_Action_वृणु_New_Thread
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        && isSchedulable(ग्रन्थिः_STATE(ksSchedulerAction))
#यदि_अन्तः
       ) {
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        प्रतिज्ञा(refill_sufficient(ग्रन्थिः_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        प्रतिज्ञा(refill_ready(ग्रन्थिः_STATE(ksSchedulerAction)->tcbSchedContext));
#यदि_अन्तः
        SCHED_पङ्क्तौ_निक्षिप(ग्रन्थिः_STATE(ksSchedulerAction));
    }
    ग्रन्थिः_STATE(ksSchedulerAction) = Scheduler_Action_वृणु_New_Thread;
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS

स्थिर अन्तःस्थ bool_t विशुद्धम् release_q_non_रिक्तानि_and_ready(शून्य)
{
    प्रत्यागम ग्रन्थिः_STATE(ksReleaseQueue.शीर्ष) != रिक्त
           && refill_ready(ग्रन्थिः_STATE(ksReleaseQueue.शीर्ष)->tcbSchedContext);
}

स्थिर शून्य tcb_Release_पङ्क्तेः_निष्कासय(शून्य)
{
    प्रतिज्ञा(ग्रन्थिः_STATE(ksReleaseQueue.शीर्ष) != रिक्त);
    प्रतिज्ञा(ग्रन्थिः_STATE(ksReleaseQueue.शीर्ष)->tcb_Sched_पूर्व == रिक्त);
    SMP_COND_STATEMENT(प्रतिज्ञा(ग्रन्थिः_STATE(ksReleaseQueue.शीर्ष)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = ग्रन्थिः_STATE(ksReleaseQueue.शीर्ष);
    प्रतिज्ञा(awakened != ग्रन्थिः_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    प्रतिज्ञा(!is_आवर्तनम्_Robin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(प्रतिज्ञा(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    प्रतिज्ञा(refill_sufficient(awakened->tcbSchedContext, 0));
    possible_Switch_गन्तव्यम्(awakened);
}

शून्य awaken(शून्य)
{
    यावत् (unlikely(release_q_non_रिक्तानि_and_ready())) {
        tcb_Release_पङ्क्तेः_निष्कासय();
    }
}
#यदि_अन्तः
