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
#ifdef CONFIG_KERNEL_MCS
#अन्तर्भाव <object/schedcontext.h>
#endif
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/kernel/thread.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <linker.h>

स्थिर seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receiveBuffer);

BOOT_CODE शून्य configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

शून्य activateThread(शून्य)
{
#ifdef CONFIG_KERNEL_MCS
    जइ (unlikely(ग्रन्थिः_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(ग्रन्थिः_STATE(ksCurThread));
        प्रतिज्ञा(thread_state_get_tsType(ग्रन्थिः_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#endif

    विकल्प (thread_state_get_tsType(ग्रन्थिः_STATE(ksCurThread)->tcbState)) {
    पक्ष ThreadState_Running:
#ifdef CONFIG_VTX
    पक्ष ThreadState_RunningVM:
#endif
        भङ्ग;

    पक्ष ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(ग्रन्थिः_STATE(ksCurThread));
        setNextPC(ग्रन्थिः_STATE(ksCurThread), pc);
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
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
    जइ (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
        /* whilst in the running state it is possible that restart pc of a thread is
         * incorrect. As we do not know what state this thread will transition to
         * after we make it inactive we update its restart pc so that the thread next
         * runs at the correct address whether it is restarted or moved directly to
         * running */
        updateRestartPC(target);
    }
    tcbSchedDequeue(target);
#ifdef CONFIG_KERNEL_MCS
    tcbReleaseRemove(target);
    schedContext_cancelYieldTo(target);
#endif
    setThreadState(target, ThreadState_Inactive);
}

शून्य restart(tcb_t *target)
{
    जइ (isStopped(target)) {
        cancelIPC(target);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        जइ (sc_sporadic(sc) && sc != ग्रन्थिः_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        जइ (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#अण्णहा
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#endif
    }
}

शून्य doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    शून्य *receiveBuffer, *sendBuffer;

    receiveBuffer = lookupIPCBuffer(true, receiver);

    जइ (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        sendBuffer = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, sendBuffer, endpoint, badge, grant,
                         receiver, receiveBuffer);
    } अण्णहा {
        doFaultTransfer(badge, sender, receiver, receiveBuffer);
    }
}

#ifdef CONFIG_KERNEL_MCS
शून्य doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#अण्णहा
शून्य doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#endif
{
#ifdef CONFIG_KERNEL_MCS
    जइ (reply->replyTCB == रिक्त ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        प्रत्यागम;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    प्रतिज्ञा(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    प्रतिज्ञा(reply->replyTCB == रिक्त);

    जइ (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != ग्रन्थिः_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#अण्णहा
    प्रतिज्ञा(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#endif

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    जइ (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, रिक्त, 0, grant, receiver);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#अण्णहा
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#endif
    } अण्णहा {
#ifndef CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#endif
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        जइ (restart) {
            setThreadState(receiver, ThreadState_Restart);
#ifndef CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#endif
        } अण्णहा {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    जइ (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        जइ ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } अण्णहा {
            जइ (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } अण्णहा {
                postpone(sc);
            }
        }
    }
#endif
}

शून्य doNormalTransfer(tcb_t *sender, word_t *sendBuffer, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receiveBuffer)
{
    word_t सन्देशः_Transferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, सन्देशः_Info_Register));

    जइ (canGrant) {
        status = lookupExtraCaps(sender, sendBuffer, tag);
        जइ (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = रिक्त;
        }
    } अण्णहा {
        current_extra_caps.excaprefs[0] = रिक्त;
    }

    सन्देशः_Transferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
                             seL4_MessageInfo_get_length(tag));

    tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

    tag = seL4_MessageInfo_set_length(tag, सन्देशः_Transferred);
    setRegister(receiver, सन्देशः_Info_Register, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

शून्य doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t sent;
    seL4_MessageInfo_t सन्देशः_Info;

    sent = setMRs_fault(sender, receiver, receiverIPCBuffer);
    सन्देशः_Info = seL4_MessageInfo_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, sent);
    setRegister(receiver, सन्देशः_Info_Register, wordFromMessageInfo(सन्देशः_Info));
    setRegister(receiver, badgeRegister, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
स्थिर seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receiveBuffer)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    जइ (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
        प्रत्यागम info;
    }

    destSlot = getReceiveSlots(receiver, receiveBuffer);

    प्रति (i = 0; i < se_L4_सन्देशः_Max_Extra_Caps && current_extra_caps.excaprefs[i] != रिक्त; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        जइ (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receiveBuffer,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } अण्णहा {
            deriveCap_ret_t dc_ret;

            जइ (!destSlot) {
                भङ्ग;
            }

            dc_ret = deriveCap(slot, cap);

            जइ (dc_ret.status != EXCEPTION_NONE) {
                भङ्ग;
            }
            जइ (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                भङ्ग;
            }

            cte_निवेशय(dc_ret.cap, slot, destSlot);

            destSlot = रिक्त;
        }
    }

    प्रत्यागम seL4_MessageInfo_set_extraCaps(info, i);
}

शून्य doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

शून्य prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    जइ (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#ifdef CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#endif
    }
}

स्थिर शून्य prepareNextDomain(शून्य)
{
    Arch_prepareNextDomain();
#ifdef CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(रिक्त);
#endif
}

स्थिर शून्य nextDomain(शून्य)
{
    ksDomScheduleIdx++;
    जइ (dschedule_is_end_marker(ksDomScheduleIdx)) {
        ksDomScheduleIdx = ksDomScheduleStart;
    }
#ifdef CONFIG_KERNEL_MCS
    ग्रन्थिः_STATE(ksReprogram) = true;
#endif
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ksDomSchedule[ksDomScheduleIdx]);
    ksDomainTime = dschedule_duration(ksDomSchedule[ksDomScheduleIdx]);
}

#ifdef CONFIG_KERNEL_MCS
स्थिर शून्य switchSchedContext(शून्य)
{
    जइ (unlikely(ग्रन्थिः_STATE(ksCurSC) != ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext)) {
        ग्रन्थिः_STATE(ksReprogram) = true;
        जइ (sc_constant_bandwidth(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext);
        }

        प्रतिज्ञा(refill_ready(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext));
        प्रतिज्ञा(refill_sufficient(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    जइ (ग्रन्थिः_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    ग्रन्थिः_STATE(ksCurSC) = ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext;
}
#endif

स्थिर शून्य scheduleChooseNewThread(शून्य)
{
    जइ (ksDomainTime == 0) {
        prepareNextDomain();
        nextDomain();
    }
    chooseThread();
}

शून्य schedule(शून्य)
{
#ifdef CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#endif

    जइ (ग्रन्थिः_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        जइ (isSchedulable(ग्रन्थिः_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } अण्णहा {
            was_runnable = false;
        }

        जइ (ग्रन्थिः_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } अण्णहा {
            tcb_t *candidate = ग्रन्थिः_STATE(ksSchedulerAction);
            प्रतिज्ञा(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                ग्रन्थिः_STATE(ksCurThread) == ग्रन्थिः_STATE(ksIdleThread)
                || (candidate->tcbPriority < ग्रन्थिः_STATE(ksCurThread)->tcbPriority);
            जइ (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                ग्रन्थिः_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } अण्णहा जइ (was_runnable && candidate->tcbPriority == ग्रन्थिः_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                ग्रन्थिः_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } अण्णहा {
                प्रतिज्ञा(candidate != ग्रन्थिः_STATE(ksCurThread));
                switchToThread(candidate);
            }
        }
    }
    ग्रन्थिः_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ifdef ENABLE_SMP_SUPPORT
    doMaskReschedule(ARCH_ग्रन्थिः_STATE(ipiReschedulePending));
    ARCH_ग्रन्थिः_STATE(ipiReschedulePending) = 0;
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_KERNEL_MCS
    switchSchedContext();

    जइ (ग्रन्थिः_STATE(ksReprogram)) {
        setNextInterrupt();
        ग्रन्थिः_STATE(ksReprogram) = false;
    }
#endif
}

शून्य chooseThread(शून्य)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    जइ (अङ्कः_Domains > 1) {
        dom = ksCurDomain;
    } अण्णहा {
        dom = 0;
    }

    जइ (likely(ग्रन्थिः_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = ग्रन्थिः_STATE(ksReadyQueues)[ready_queues_index(dom, prio)].head;
        प्रतिज्ञा(thread);
        प्रतिज्ञा(isSchedulable(thread));
#ifdef CONFIG_KERNEL_MCS
        प्रतिज्ञा(refill_sufficient(thread->tcbSchedContext, 0));
        प्रतिज्ञा(refill_ready(thread->tcbSchedContext));
#endif
        switchToThread(thread);
    } अण्णहा {
        switchToIdleThread();
    }
}

शून्य switchToThread(tcb_t *thread)
{
#ifdef CONFIG_KERNEL_MCS
    प्रतिज्ञा(thread->tcbSchedContext != रिक्त);
    प्रतिज्ञा(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    प्रतिज्ञा(refill_sufficient(thread->tcbSchedContext, 0));
    प्रतिज्ञा(refill_ready(thread->tcbSchedContext));
#endif

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ग्रन्थिः_STATE(ksCurThread), thread);
#endif
    Arch_switchToThread(thread);

#ifdef CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#endif /* CONFIG_HAVE_FPU */

    tcbSchedDequeue(thread);
    ग्रन्थिः_STATE(ksCurThread) = thread;
}

शून्य switchToIdleThread(शून्य)
{
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ग्रन्थिः_STATE(ksCurThread), ग्रन्थिः_STATE(ksIdleThread));
#endif
    Arch_switchToIdleThread();
    ग्रन्थिः_STATE(ksCurThread) = ग्रन्थिः_STATE(ksIdleThread);
}

शून्य setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    जइ (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    जइ (tptr == ग्रन्थिः_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

शून्य setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ifdef CONFIG_KERNEL_MCS
शून्य setPriority(tcb_t *tptr, prio_t prio)
{
    विकल्प (thread_state_get_tsType(tptr->tcbState)) {
    पक्ष ThreadState_Running:
    पक्ष ThreadState_Restart:
        जइ (thread_state_get_tcbQueued(tptr->tcbState) || tptr == ग्रन्थिः_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } अण्णहा {
            tptr->tcbPriority = prio;
        }
        भङ्ग;
    पक्ष ThreadState_BlockedOnReceive:
    पक्ष ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        भङ्ग;
    पक्ष ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        भङ्ग;
    स्वभाव:
        tptr->tcbPriority = prio;
        भङ्ग;
    }
}
#अण्णहा
शून्य setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    जइ (isRunnable(tptr)) {
        जइ (tptr == ग्रन्थिः_STATE(ksCurThread)) {
            rescheduleRequired();
        } अण्णहा {
            possibleSwitchTo(tptr);
        }
    }
}
#endif

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
शून्य possibleSwitchTo(tcb_t *target)
{
#ifdef CONFIG_KERNEL_MCS
    जइ (target->tcbSchedContext != रिक्त && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#endif
        जइ (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } अण्णहा जइ (ग्रन्थिः_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } अण्णहा {
            ग्रन्थिः_STATE(ksSchedulerAction) = target;
        }
#ifdef CONFIG_KERNEL_MCS
    }
#endif

}

शून्य setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

शून्य scheduleTCB(tcb_t *tptr)
{
    जइ (tptr == ग्रन्थिः_STATE(ksCurThread) &&
        ग्रन्थिः_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        ग्रन्थिः_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#ifdef CONFIG_KERNEL_MCS
शून्य postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    प्रतिज्ञा(tcb != रिक्त);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    ग्रन्थिः_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

शून्य setNextInterrupt(शून्य)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_head_refill = *refill_head(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext);
    ticks_t next_interrupt = ग्रन्थिः_STATE(ksCurTime) + ct_head_refill.rAmount;

    जइ (अङ्कः_Domains > 1) {
        next_interrupt = MIN(next_interrupt, ग्रन्थिः_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_head = ग्रन्थिः_STATE(ksReleaseQueue.head);
    जइ (rlq_head != रिक्त) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_head_refill = *refill_head(rlq_head->tcbSchedContext);
        next_interrupt = MIN(rlq_head_refill.rTime, next_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    प्रतिज्ञा(next_interrupt >= ग्रन्थिः_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(next_interrupt - getTimerPrecision());
}

शून्य chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    जइ (likely(ग्रन्थिः_STATE(ksCurSC) != ग्रन्थिः_STATE(ksIdleSC))) {
        जइ (isRoundRobin(ग्रन्थिः_STATE(ksCurSC))) {
            प्रतिज्ञा(refill_size(ग्रन्थिः_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t head = *refill_head(ग्रन्थिः_STATE(ksCurSC));
            refill_t tail = *refill_tail(ग्रन्थिः_STATE(ksCurSC));
            refill_head(ग्रन्थिः_STATE(ksCurSC))->rAmount = head.rAmount + tail.rAmount;
            refill_tail(ग्रन्थिः_STATE(ksCurSC))->rAmount = 0;
        } अण्णहा {
            refill_budget_check(consumed);
        }

        प्रतिज्ञा(refill_head(ग्रन्थिः_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        ग्रन्थिः_STATE(ksCurSC)->scConsumed += consumed;
    }
    ग्रन्थिः_STATE(ksConsumed) = 0;
    जइ (likely(isSchedulable(ग्रन्थिः_STATE(ksCurThread)))) {
        प्रतिज्ञा(ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext == ग्रन्थिः_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        ग्रन्थिः_STATE(ksReprogram) = true;
    }
}

शून्य endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(ग्रन्थिः_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(ग्रन्थिः_STATE(ksCurThread));

    जइ (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(ग्रन्थिः_STATE(ksCurSC)->scBadge);
        handleTimeout(ग्रन्थिः_STATE(ksCurThread));
    } अण्णहा जइ (refill_ready(ग्रन्थिः_STATE(ksCurSC)) && refill_sufficient(ग्रन्थिः_STATE(ksCurSC), 0)) {
        /* apply round robin */
        प्रतिज्ञा(!thread_state_get_tcbQueued(ग्रन्थिः_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } अण्णहा {
        /* postpone until ready */
        postpone(ग्रन्थिः_STATE(ksCurSC));
    }
}
#अण्णहा

शून्य timerTick(शून्य)
{
    जइ (likely(thread_state_get_tsType(ग्रन्थिः_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ifdef CONFIG_VTX
        || thread_state_get_tsType(ग्रन्थिः_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#endif
       ) {
        जइ (ग्रन्थिः_STATE(ksCurThread)->tcbTimeSlice > 1) {
            ग्रन्थिः_STATE(ksCurThread)->tcbTimeSlice--;
        } अण्णहा {
            ग्रन्थिः_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    जइ (अङ्कः_Domains > 1) {
        ksDomainTime--;
        जइ (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#endif

शून्य rescheduleRequired(शून्य)
{
    जइ (ग्रन्थिः_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && ग्रन्थिः_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#ifdef CONFIG_KERNEL_MCS
        && isSchedulable(ग्रन्थिः_STATE(ksSchedulerAction))
#endif
       ) {
#ifdef CONFIG_KERNEL_MCS
        प्रतिज्ञा(refill_sufficient(ग्रन्थिः_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        प्रतिज्ञा(refill_ready(ग्रन्थिः_STATE(ksSchedulerAction)->tcbSchedContext));
#endif
        SCHED_ENQUEUE(ग्रन्थिः_STATE(ksSchedulerAction));
    }
    ग्रन्थिः_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#ifdef CONFIG_KERNEL_MCS

स्थिर अन्तःस्थ bool_t PURE release_q_non_empty_and_ready(शून्य)
{
    प्रत्यागम ग्रन्थिः_STATE(ksReleaseQueue.head) != रिक्त
           && refill_ready(ग्रन्थिः_STATE(ksReleaseQueue.head)->tcbSchedContext);
}

स्थिर शून्य tcbReleaseDequeue(शून्य)
{
    प्रतिज्ञा(ग्रन्थिः_STATE(ksReleaseQueue.head) != रिक्त);
    प्रतिज्ञा(ग्रन्थिः_STATE(ksReleaseQueue.head)->tcbSchedPrev == रिक्त);
    SMP_COND_STATEMENT(प्रतिज्ञा(ग्रन्थिः_STATE(ksReleaseQueue.head)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = ग्रन्थिः_STATE(ksReleaseQueue.head);
    प्रतिज्ञा(awakened != ग्रन्थिः_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    प्रतिज्ञा(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(प्रतिज्ञा(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    प्रतिज्ञा(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

शून्य awaken(शून्य)
{
    जाव (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#endif
