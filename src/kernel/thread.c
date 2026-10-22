/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <object.h>
#समावेश <util.h>
#समावेश <api/faults.h>
#समावेश <api/types.h>
#समावेश <kernel/cspace.h>
#समावेश <kernel/thread.h>
#समावेश <kernel/vspace.h>
#समावेश <object/domain.h>
#अगर_घोषित CONFIG_KERNEL_MCS
#समावेश <object/schedcontext.h>
#पूर्ण_अगर
#समावेश <model/statedata.h>
#समावेश <arch/machine.h>
#समावेश <arch/kernel/thread.h>
#समावेश <machine/registerset.h>
#समावेश <machine/fpu.h>
#समावेश <linker.h>

अटल seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receive_बफर);

BOOT_CODE व्योम configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

व्योम activateThread(व्योम)
{
#अगर_घोषित CONFIG_KERNEL_MCS
    अगर (unlikely(गाँठ_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(गाँठ_STATE(ksCurThread));
        निश्चित(thread_state_get_tsType(गाँठ_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#पूर्ण_अगर

    चयन (thread_state_get_tsType(गाँठ_STATE(ksCurThread)->tcbState)) {
    हाल ThreadState_Running:
#अगर_घोषित CONFIG_VTX
    हाल ThreadState_RunningVM:
#पूर्ण_अगर
        अवरोध;

    हाल ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(गाँठ_STATE(ksCurThread));
        set_अग्र_PC(गाँठ_STATE(ksCurThread), pc);
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
        अवरोध;
    }

    हाल ThreadState_IdleThreadState:
        Arch_activateIdleThread(गाँठ_STATE(ksCurThread));
        अवरोध;

    शेष:
        fail("Current thread is blocked");
    }
}

व्योम suspend(tcb_t *target)
{
    cancelIPC(target);
    अगर (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
        /* whilst in the running state it is possible that restart pc of a thread is
         * incorrect. As we do not know what state this thread will transition to
         * after we make it inactive we update its restart pc so that the thread next
         * runs at the correct address whether it is restarted or moved directly to
         * running */
        updateRestartPC(target);
    }
    tcbSchedDequeue(target);
#अगर_घोषित CONFIG_KERNEL_MCS
    tcbReleaseRemove(target);
    schedContext_cancelYieldTo(target);
#पूर्ण_अगर
    setThreadState(target, ThreadState_Inactive);
}

व्योम restart(tcb_t *target)
{
    अगर (isStopped(target)) {
        cancelIPC(target);
#अगर_घोषित CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        अगर (sc_sporadic(sc) && sc != गाँठ_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        अगर (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#अन्यथा
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#पूर्ण_अगर
    }
}

व्योम doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    व्योम *receive_बफर, *send_बफर;

    receive_बफर = lookupIPCBuffer(true, receiver);

    अगर (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        send_बफर = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, send_बफर, endpoint, badge, grant,
                         receiver, receive_बफर);
    } अन्यथा {
        doFaultTransfer(badge, sender, receiver, receive_बफर);
    }
}

#अगर_घोषित CONFIG_KERNEL_MCS
व्योम doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#अन्यथा
व्योम doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#पूर्ण_अगर
{
#अगर_घोषित CONFIG_KERNEL_MCS
    अगर (reply->replyTCB == शून्य ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        वापस;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    निश्चित(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    निश्चित(reply->replyTCB == शून्य);

    अगर (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != गाँठ_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#अन्यथा
    निश्चित(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#पूर्ण_अगर

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    अगर (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, शून्य, 0, grant, receiver);
#अगर_घोषित CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#अन्यथा
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#पूर्ण_अगर
    } अन्यथा {
#अगर_अघोषित CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#पूर्ण_अगर
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        अगर (restart) {
            setThreadState(receiver, ThreadState_Restart);
#अगर_अघोषित CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#पूर्ण_अगर
        } अन्यथा {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    अगर (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        अगर ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } अन्यथा {
            अगर (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } अन्यथा {
                postpone(sc);
            }
        }
    }
#पूर्ण_अगर
}

व्योम doNormalTransfer(tcb_t *sender, word_t *send_बफर, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receive_बफर)
{
    word_t संदेश_Transferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, संदेश_Info_Register));

    अगर (canGrant) {
        status = lookupExtraCaps(sender, send_बफर, tag);
        अगर (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = शून्य;
        }
    } अन्यथा {
        current_extra_caps.excaprefs[0] = शून्य;
    }

    संदेश_Transferred = copyMRs(sender, send_बफर, receiver, receive_बफर,
                             se_L4_Message_Info_get_लंबाई(tag));

    tag = transferCaps(tag, endpoint, receiver, receive_बफर);

    tag = se_L4_Message_Info_set_लंबाई(tag, संदेश_Transferred);
    setRegister(receiver, संदेश_Info_Register, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

व्योम doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t sent;
    seL4_MessageInfo_t संदेश_Info;

    sent = setMRs_fault(sender, receiver, receiverIPCBuffer);
    संदेश_Info = seL4_MessageInfo_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, sent);
    setRegister(receiver, संदेश_Info_Register, wordFromMessageInfo(संदेश_Info));
    setRegister(receiver, badgeRegister, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
अटल seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receive_बफर)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    अगर (likely(!current_extra_caps.excaprefs[0] || !receive_बफर)) {
        वापस info;
    }

    destSlot = getReceiveSlots(receiver, receive_बफर);

    क्रम (i = 0; i < se_L4_संदेश_Max_Extra_Caps && current_extra_caps.excaprefs[i] != शून्य; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        अगर (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receive_बफर,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } अन्यथा {
            deriveCap_ret_t dc_ret;

            अगर (!destSlot) {
                अवरोध;
            }

            dc_ret = deriveCap(slot, cap);

            अगर (dc_ret.status != EXCEPTION_NONE) {
                अवरोध;
            }
            अगर (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                अवरोध;
            }

            cte_जोड़ो(dc_ret.cap, slot, destSlot);

            destSlot = शून्य;
        }
    }

    वापस seL4_MessageInfo_set_extraCaps(info, i);
}

व्योम doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

व्योम prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    अगर (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#अगर_घोषित CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#पूर्ण_अगर
    }
}

अटल व्योम prepare_अग्र_Domain(व्योम)
{
    Arch_prepare_अग्र_Domain();
#अगर_घोषित CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(शून्य);
#पूर्ण_अगर
}

अटल व्योम अग्र_Domain(व्योम)
{
    ksDomScheduleIdx++;
    अगर (dschedule_is_end_marker(ksDomScheduleIdx)) {
        ksDomScheduleIdx = ksDomScheduleStart;
    }
#अगर_घोषित CONFIG_KERNEL_MCS
    गाँठ_STATE(ksReprogram) = true;
#पूर्ण_अगर
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ksDomSchedule[ksDomScheduleIdx]);
    ksDomainTime = dschedule_duration(ksDomSchedule[ksDomScheduleIdx]);
}

#अगर_घोषित CONFIG_KERNEL_MCS
अटल व्योम switchSchedContext(व्योम)
{
    अगर (unlikely(गाँठ_STATE(ksCurSC) != गाँठ_STATE(ksCurThread)->tcbSchedContext)) {
        गाँठ_STATE(ksReprogram) = true;
        अगर (sc_constant_bandwidth(गाँठ_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(गाँठ_STATE(ksCurThread)->tcbSchedContext);
        }

        निश्चित(refill_ready(गाँठ_STATE(ksCurThread)->tcbSchedContext));
        निश्चित(refill_sufficient(गाँठ_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    अगर (गाँठ_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    गाँठ_STATE(ksCurSC) = गाँठ_STATE(ksCurThread)->tcbSchedContext;
}
#पूर्ण_अगर

अटल व्योम scheduleChooseNewThread(व्योम)
{
    अगर (ksDomainTime == 0) {
        prepare_अग्र_Domain();
        अग्र_Domain();
    }
    chooseThread();
}

व्योम schedule(व्योम)
{
#अगर_घोषित CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#पूर्ण_अगर

    अगर (गाँठ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        अगर (isSchedulable(गाँठ_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } अन्यथा {
            was_runnable = false;
        }

        अगर (गाँठ_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } अन्यथा {
            tcb_t *candidate = गाँठ_STATE(ksSchedulerAction);
            निश्चित(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                गाँठ_STATE(ksCurThread) == गाँठ_STATE(ksIdleThread)
                || (candidate->tcbPriority < गाँठ_STATE(ksCurThread)->tcbPriority);
            अगर (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                गाँठ_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } अन्यथा अगर (was_runnable && candidate->tcbPriority == गाँठ_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                गाँठ_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } अन्यथा {
                निश्चित(candidate != गाँठ_STATE(ksCurThread));
                switchToThread(candidate);
            }
        }
    }
    गाँठ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#अगर_घोषित ENABLE_SMP_SUPPORT
    doMaskReschedule(ARCH_गाँठ_STATE(ipiReschedulePending));
    ARCH_गाँठ_STATE(ipiReschedulePending) = 0;
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

#अगर_घोषित CONFIG_KERNEL_MCS
    switchSchedContext();

    अगर (गाँठ_STATE(ksReprogram)) {
        set_अग्र_Interrupt();
        गाँठ_STATE(ksReprogram) = false;
    }
#पूर्ण_अगर
}

व्योम chooseThread(व्योम)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    अगर (अंक_Domains > 1) {
        dom = ksCurDomain;
    } अन्यथा {
        dom = 0;
    }

    अगर (likely(गाँठ_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = गाँठ_STATE(ksReadyQueues)[ready_queues_सूचकांक(dom, prio)].शीर्ष;
        निश्चित(thread);
        निश्चित(isSchedulable(thread));
#अगर_घोषित CONFIG_KERNEL_MCS
        निश्चित(refill_sufficient(thread->tcbSchedContext, 0));
        निश्चित(refill_ready(thread->tcbSchedContext));
#पूर्ण_अगर
        switchToThread(thread);
    } अन्यथा {
        switchToIdleThread();
    }
}

व्योम switchToThread(tcb_t *thread)
{
#अगर_घोषित CONFIG_KERNEL_MCS
    निश्चित(thread->tcbSchedContext != शून्य);
    निश्चित(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    निश्चित(refill_sufficient(thread->tcbSchedContext, 0));
    निश्चित(refill_ready(thread->tcbSchedContext));
#पूर्ण_अगर

#अगर_घोषित CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(गाँठ_STATE(ksCurThread), thread);
#पूर्ण_अगर
    Arch_switchToThread(thread);

#अगर_घोषित CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#पूर्ण_अगर /* CONFIG_HAVE_FPU */

    tcbSchedDequeue(thread);
    गाँठ_STATE(ksCurThread) = thread;
}

व्योम switchToIdleThread(व्योम)
{
#अगर_घोषित CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(गाँठ_STATE(ksCurThread), गाँठ_STATE(ksIdleThread));
#पूर्ण_अगर
    Arch_switchToIdleThread();
    गाँठ_STATE(ksCurThread) = गाँठ_STATE(ksIdleThread);
}

व्योम setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    अगर (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    अगर (tptr == गाँठ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

व्योम setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#अगर_घोषित CONFIG_KERNEL_MCS
व्योम setPriority(tcb_t *tptr, prio_t prio)
{
    चयन (thread_state_get_tsType(tptr->tcbState)) {
    हाल ThreadState_Running:
    हाल ThreadState_Restart:
        अगर (thread_state_get_tcbQueued(tptr->tcbState) || tptr == गाँठ_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } अन्यथा {
            tptr->tcbPriority = prio;
        }
        अवरोध;
    हाल ThreadState_BlockedOnReceive:
    हाल ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        अवरोध;
    हाल ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        अवरोध;
    शेष:
        tptr->tcbPriority = prio;
        अवरोध;
    }
}
#अन्यथा
व्योम setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    अगर (isRunnable(tptr)) {
        अगर (tptr == गाँठ_STATE(ksCurThread)) {
            rescheduleRequired();
        } अन्यथा {
            possibleSwitchTo(tptr);
        }
    }
}
#पूर्ण_अगर

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
व्योम possibleSwitchTo(tcb_t *target)
{
#अगर_घोषित CONFIG_KERNEL_MCS
    अगर (target->tcbSchedContext != शून्य && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#पूर्ण_अगर
        अगर (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } अन्यथा अगर (गाँठ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } अन्यथा {
            गाँठ_STATE(ksSchedulerAction) = target;
        }
#अगर_घोषित CONFIG_KERNEL_MCS
    }
#पूर्ण_अगर

}

व्योम setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

व्योम scheduleTCB(tcb_t *tptr)
{
    अगर (tptr == गाँठ_STATE(ksCurThread) &&
        गाँठ_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        गाँठ_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#अगर_घोषित CONFIG_KERNEL_MCS
व्योम postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    निश्चित(tcb != शून्य);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    गाँठ_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

व्योम set_अग्र_Interrupt(व्योम)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_शीर्ष_refill = *refill_शीर्ष(गाँठ_STATE(ksCurThread)->tcbSchedContext);
    ticks_t अग्र_interrupt = गाँठ_STATE(ksCurTime) + ct_शीर्ष_refill.rAmount;

    अगर (अंक_Domains > 1) {
        अग्र_interrupt = MIN(अग्र_interrupt, गाँठ_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_शीर्ष = गाँठ_STATE(ksReleaseQueue.शीर्ष);
    अगर (rlq_शीर्ष != शून्य) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_शीर्ष_refill = *refill_शीर्ष(rlq_शीर्ष->tcbSchedContext);
        अग्र_interrupt = MIN(rlq_शीर्ष_refill.rTime, अग्र_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    निश्चित(अग्र_interrupt >= गाँठ_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(अग्र_interrupt - getTimerPrecision());
}

व्योम chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    अगर (likely(गाँठ_STATE(ksCurSC) != गाँठ_STATE(ksIdleSC))) {
        अगर (isRoundRobin(गाँठ_STATE(ksCurSC))) {
            निश्चित(refill_आकार(गाँठ_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t शीर्ष = *refill_शीर्ष(गाँठ_STATE(ksCurSC));
            refill_t पुच्छ = *refill_पुच्छ(गाँठ_STATE(ksCurSC));
            refill_शीर्ष(गाँठ_STATE(ksCurSC))->rAmount = शीर्ष.rAmount + पुच्छ.rAmount;
            refill_पुच्छ(गाँठ_STATE(ksCurSC))->rAmount = 0;
        } अन्यथा {
            refill_budget_check(consumed);
        }

        निश्चित(refill_शीर्ष(गाँठ_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        गाँठ_STATE(ksCurSC)->scConsumed += consumed;
    }
    गाँठ_STATE(ksConsumed) = 0;
    अगर (likely(isSchedulable(गाँठ_STATE(ksCurThread)))) {
        निश्चित(गाँठ_STATE(ksCurThread)->tcbSchedContext == गाँठ_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        गाँठ_STATE(ksReprogram) = true;
    }
}

व्योम endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(गाँठ_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(गाँठ_STATE(ksCurThread));

    अगर (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(गाँठ_STATE(ksCurSC)->scBadge);
        handleTimeout(गाँठ_STATE(ksCurThread));
    } अन्यथा अगर (refill_ready(गाँठ_STATE(ksCurSC)) && refill_sufficient(गाँठ_STATE(ksCurSC), 0)) {
        /* apply round robin */
        निश्चित(!thread_state_get_tcbQueued(गाँठ_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } अन्यथा {
        /* postpone until ready */
        postpone(गाँठ_STATE(ksCurSC));
    }
}
#अन्यथा

व्योम timerTick(व्योम)
{
    अगर (likely(thread_state_get_tsType(गाँठ_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#अगर_घोषित CONFIG_VTX
        || thread_state_get_tsType(गाँठ_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#पूर्ण_अगर
       ) {
        अगर (गाँठ_STATE(ksCurThread)->tcbTimeSlice > 1) {
            गाँठ_STATE(ksCurThread)->tcbTimeSlice--;
        } अन्यथा {
            गाँठ_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    अगर (अंक_Domains > 1) {
        ksDomainTime--;
        अगर (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#पूर्ण_अगर

व्योम rescheduleRequired(व्योम)
{
    अगर (गाँठ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && गाँठ_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#अगर_घोषित CONFIG_KERNEL_MCS
        && isSchedulable(गाँठ_STATE(ksSchedulerAction))
#पूर्ण_अगर
       ) {
#अगर_घोषित CONFIG_KERNEL_MCS
        निश्चित(refill_sufficient(गाँठ_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        निश्चित(refill_ready(गाँठ_STATE(ksSchedulerAction)->tcbSchedContext));
#पूर्ण_अगर
        SCHED_ENQUEUE(गाँठ_STATE(ksSchedulerAction));
    }
    गाँठ_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#अगर_घोषित CONFIG_KERNEL_MCS

अटल अंतरभूत bool_t PURE release_q_non_empty_and_ready(व्योम)
{
    वापस गाँठ_STATE(ksReleaseQueue.शीर्ष) != शून्य
           && refill_ready(गाँठ_STATE(ksReleaseQueue.शीर्ष)->tcbSchedContext);
}

अटल व्योम tcbReleaseDequeue(व्योम)
{
    निश्चित(गाँठ_STATE(ksReleaseQueue.शीर्ष) != शून्य);
    निश्चित(गाँठ_STATE(ksReleaseQueue.शीर्ष)->tcb_Sched_पूर्व == शून्य);
    SMP_COND_STATEMENT(निश्चित(गाँठ_STATE(ksReleaseQueue.शीर्ष)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = गाँठ_STATE(ksReleaseQueue.शीर्ष);
    निश्चित(awakened != गाँठ_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    निश्चित(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(निश्चित(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    निश्चित(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

व्योम awaken(व्योम)
{
    जबतक (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#पूर्ण_अगर
