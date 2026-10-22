/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <object.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <api/faults.h>
#ಅನ್ತರ್ಭಾವ <api/types.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <object/domain.h>
#ifdef CONFIG_KERNEL_MCS
#ಅನ್ತರ್ಭಾವ <object/schedcontext.h>
#endif
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <linker.h>

ಸ್ಥಿರ seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receiveBuffer);

BOOT_CODE ಶೂನ್ಯ configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

ಶೂನ್ಯ activateThread(ಶೂನ್ಯ)
{
#ifdef CONFIG_KERNEL_MCS
    ಯದಿ (unlikely(ಗಂಟು_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(ಗಂಟು_STATE(ksCurThread));
        ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(ಗಂಟು_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#endif

    ವಿಕಲ್ಪ (thread_state_get_tsType(ಗಂಟು_STATE(ksCurThread)->tcbState)) {
    ಪಕ್ಷ ThreadState_Running:
#ifdef CONFIG_VTX
    ಪಕ್ಷ ThreadState_RunningVM:
#endif
        ಭಙ್ಗ;

    ಪಕ್ಷ ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(ಗಂಟು_STATE(ksCurThread));
        setNextPC(ಗಂಟು_STATE(ksCurThread), pc);
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
        ಭಙ್ಗ;
    }

    ಪಕ್ಷ ThreadState_IdleThreadState:
        Arch_activateIdleThread(ಗಂಟು_STATE(ksCurThread));
        ಭಙ್ಗ;

    ಸ್ವಭಾವ:
        fail("Current thread is blocked");
    }
}

ಶೂನ್ಯ suspend(tcb_t *target)
{
    cancelIPC(target);
    ಯದಿ (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
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

ಶೂನ್ಯ restart(tcb_t *target)
{
    ಯದಿ (isStopped(target)) {
        cancelIPC(target);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        ಯದಿ (sc_sporadic(sc) && sc != ಗಂಟು_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        ಯದಿ (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#ಅನ್ಯಥಾ
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#endif
    }
}

ಶೂನ್ಯ doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    ಶೂನ್ಯ *receiveBuffer, *sendBuffer;

    receiveBuffer = lookupIPCBuffer(true, receiver);

    ಯದಿ (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        sendBuffer = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, sendBuffer, endpoint, badge, grant,
                         receiver, receiveBuffer);
    } ಅನ್ಯಥಾ {
        doFaultTransfer(badge, sender, receiver, receiveBuffer);
    }
}

#ifdef CONFIG_KERNEL_MCS
ಶೂನ್ಯ doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#ಅನ್ಯಥಾ
ಶೂನ್ಯ doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#endif
{
#ifdef CONFIG_KERNEL_MCS
    ಯದಿ (reply->replyTCB == ರಿಕ್ತ ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    ಪ್ರತಿಜ್ಞಾ(reply->replyTCB == ರಿಕ್ತ);

    ಯದಿ (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != ಗಂಟು_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#ಅನ್ಯಥಾ
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#endif

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    ಯದಿ (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, ರಿಕ್ತ, 0, grant, receiver);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#ಅನ್ಯಥಾ
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#endif
    } ಅನ್ಯಥಾ {
#ifndef CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#endif
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        ಯದಿ (restart) {
            setThreadState(receiver, ThreadState_Restart);
#ifndef CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#endif
        } ಅನ್ಯಥಾ {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    ಯದಿ (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        ಯದಿ ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } ಅನ್ಯಥಾ {
            ಯದಿ (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } ಅನ್ಯಥಾ {
                postpone(sc);
            }
        }
    }
#endif
}

ಶೂನ್ಯ doNormalTransfer(tcb_t *sender, word_t *sendBuffer, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receiveBuffer)
{
    word_t ಸಂದೇಶ_Transferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, ಸಂದೇಶ_Info_Register));

    ಯದಿ (canGrant) {
        status = lookupExtraCaps(sender, sendBuffer, tag);
        ಯದಿ (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = ರಿಕ್ತ;
        }
    } ಅನ್ಯಥಾ {
        current_extra_caps.excaprefs[0] = ರಿಕ್ತ;
    }

    ಸಂದೇಶ_Transferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
                             seL4_MessageInfo_get_length(tag));

    tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

    tag = seL4_MessageInfo_set_length(tag, ಸಂದೇಶ_Transferred);
    setRegister(receiver, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

ಶೂನ್ಯ doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t sent;
    seL4_MessageInfo_t ಸಂದೇಶ_Info;

    sent = setMRs_fault(sender, receiver, receiverIPCBuffer);
    ಸಂದೇಶ_Info = seL4_MessageInfo_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, sent);
    setRegister(receiver, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(ಸಂದೇಶ_Info));
    setRegister(receiver, badgeRegister, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ಸ್ಥಿರ seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receiveBuffer)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    ಯದಿ (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
        ಪ್ರತ್ಯಾಗಮ info;
    }

    destSlot = getReceiveSlots(receiver, receiveBuffer);

    ಪ್ರತಿ (i = 0; i < se_L4_ಸಂದೇಶ_Max_Extra_Caps && current_extra_caps.excaprefs[i] != ರಿಕ್ತ; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        ಯದಿ (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receiveBuffer,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } ಅನ್ಯಥಾ {
            deriveCap_ret_t dc_ret;

            ಯದಿ (!destSlot) {
                ಭಙ್ಗ;
            }

            dc_ret = deriveCap(slot, cap);

            ಯದಿ (dc_ret.status != EXCEPTION_NONE) {
                ಭಙ್ಗ;
            }
            ಯದಿ (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                ಭಙ್ಗ;
            }

            cte_ಸೇರಿಸು(dc_ret.cap, slot, destSlot);

            destSlot = ರಿಕ್ತ;
        }
    }

    ಪ್ರತ್ಯಾಗಮ seL4_MessageInfo_set_extraCaps(info, i);
}

ಶೂನ್ಯ doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

ಶೂನ್ಯ prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    ಯದಿ (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#ifdef CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#endif
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ prepareNextDomain(ಶೂನ್ಯ)
{
    Arch_prepareNextDomain();
#ifdef CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(ರಿಕ್ತ);
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ nextDomain(ಶೂನ್ಯ)
{
    ksDomScheduleIdx++;
    ಯದಿ (dschedule_is_end_marker(ksDomScheduleIdx)) {
        ksDomScheduleIdx = ksDomScheduleStart;
    }
#ifdef CONFIG_KERNEL_MCS
    ಗಂಟು_STATE(ksReprogram) = true;
#endif
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ksDomSchedule[ksDomScheduleIdx]);
    ksDomainTime = dschedule_duration(ksDomSchedule[ksDomScheduleIdx]);
}

#ifdef CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಶೂನ್ಯ switchSchedContext(ಶೂನ್ಯ)
{
    ಯದಿ (unlikely(ಗಂಟು_STATE(ksCurSC) != ಗಂಟು_STATE(ksCurThread)->tcbSchedContext)) {
        ಗಂಟು_STATE(ksReprogram) = true;
        ಯದಿ (sc_constant_bandwidth(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext);
        }

        ಪ್ರತಿಜ್ಞಾ(refill_ready(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext));
        ಪ್ರತಿಜ್ಞಾ(refill_sufficient(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    ಯದಿ (ಗಂಟು_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    ಗಂಟು_STATE(ksCurSC) = ಗಂಟು_STATE(ksCurThread)->tcbSchedContext;
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ scheduleChooseNewThread(ಶೂನ್ಯ)
{
    ಯದಿ (ksDomainTime == 0) {
        prepareNextDomain();
        nextDomain();
    }
    chooseThread();
}

ಶೂನ್ಯ schedule(ಶೂನ್ಯ)
{
#ifdef CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#endif

    ಯದಿ (ಗಂಟು_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        ಯದಿ (isSchedulable(ಗಂಟು_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } ಅನ್ಯಥಾ {
            was_runnable = false;
        }

        ಯದಿ (ಗಂಟು_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } ಅನ್ಯಥಾ {
            tcb_t *candidate = ಗಂಟು_STATE(ksSchedulerAction);
            ಪ್ರತಿಜ್ಞಾ(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                ಗಂಟು_STATE(ksCurThread) == ಗಂಟು_STATE(ksIdleThread)
                || (candidate->tcbPriority < ಗಂಟು_STATE(ksCurThread)->tcbPriority);
            ಯದಿ (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                ಗಂಟು_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } ಅನ್ಯಥಾ ಯದಿ (was_runnable && candidate->tcbPriority == ಗಂಟು_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                ಗಂಟು_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } ಅನ್ಯಥಾ {
                ಪ್ರತಿಜ್ಞಾ(candidate != ಗಂಟು_STATE(ksCurThread));
                switchToThread(candidate);
            }
        }
    }
    ಗಂಟು_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ifdef ENABLE_SMP_SUPPORT
    doMaskReschedule(ARCH_ಗಂಟು_STATE(ipiReschedulePending));
    ARCH_ಗಂಟು_STATE(ipiReschedulePending) = 0;
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_KERNEL_MCS
    switchSchedContext();

    ಯದಿ (ಗಂಟು_STATE(ksReprogram)) {
        setNextInterrupt();
        ಗಂಟು_STATE(ksReprogram) = false;
    }
#endif
}

ಶೂನ್ಯ chooseThread(ಶೂನ್ಯ)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    ಯದಿ (ಅಂಕೆ_Domains > 1) {
        dom = ksCurDomain;
    } ಅನ್ಯಥಾ {
        dom = 0;
    }

    ಯದಿ (likely(ಗಂಟು_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = ಗಂಟು_STATE(ksReadyQueues)[ready_queues_index(dom, prio)].head;
        ಪ್ರತಿಜ್ಞಾ(thread);
        ಪ್ರತಿಜ್ಞಾ(isSchedulable(thread));
#ifdef CONFIG_KERNEL_MCS
        ಪ್ರತಿಜ್ಞಾ(refill_sufficient(thread->tcbSchedContext, 0));
        ಪ್ರತಿಜ್ಞಾ(refill_ready(thread->tcbSchedContext));
#endif
        switchToThread(thread);
    } ಅನ್ಯಥಾ {
        switchToIdleThread();
    }
}

ಶೂನ್ಯ switchToThread(tcb_t *thread)
{
#ifdef CONFIG_KERNEL_MCS
    ಪ್ರತಿಜ್ಞಾ(thread->tcbSchedContext != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    ಪ್ರತಿಜ್ಞಾ(refill_sufficient(thread->tcbSchedContext, 0));
    ಪ್ರತಿಜ್ಞಾ(refill_ready(thread->tcbSchedContext));
#endif

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ಗಂಟು_STATE(ksCurThread), thread);
#endif
    Arch_switchToThread(thread);

#ifdef CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#endif /* CONFIG_HAVE_FPU */

    tcbSchedDequeue(thread);
    ಗಂಟು_STATE(ksCurThread) = thread;
}

ಶೂನ್ಯ switchToIdleThread(ಶೂನ್ಯ)
{
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ಗಂಟು_STATE(ksCurThread), ಗಂಟು_STATE(ksIdleThread));
#endif
    Arch_switchToIdleThread();
    ಗಂಟು_STATE(ksCurThread) = ಗಂಟು_STATE(ksIdleThread);
}

ಶೂನ್ಯ setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    ಯದಿ (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    ಯದಿ (tptr == ಗಂಟು_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

ಶೂನ್ಯ setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ifdef CONFIG_KERNEL_MCS
ಶೂನ್ಯ setPriority(tcb_t *tptr, prio_t prio)
{
    ವಿಕಲ್ಪ (thread_state_get_tsType(tptr->tcbState)) {
    ಪಕ್ಷ ThreadState_Running:
    ಪಕ್ಷ ThreadState_Restart:
        ಯದಿ (thread_state_get_tcbQueued(tptr->tcbState) || tptr == ಗಂಟು_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } ಅನ್ಯಥಾ {
            tptr->tcbPriority = prio;
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ ThreadState_BlockedOnReceive:
    ಪಕ್ಷ ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ಭಙ್ಗ;
    ಪಕ್ಷ ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        tptr->tcbPriority = prio;
        ಭಙ್ಗ;
    }
}
#ಅನ್ಯಥಾ
ಶೂನ್ಯ setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    ಯದಿ (isRunnable(tptr)) {
        ಯದಿ (tptr == ಗಂಟು_STATE(ksCurThread)) {
            rescheduleRequired();
        } ಅನ್ಯಥಾ {
            possibleSwitchTo(tptr);
        }
    }
}
#endif

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
ಶೂನ್ಯ possibleSwitchTo(tcb_t *target)
{
#ifdef CONFIG_KERNEL_MCS
    ಯದಿ (target->tcbSchedContext != ರಿಕ್ತ && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#endif
        ಯದಿ (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } ಅನ್ಯಥಾ ಯದಿ (ಗಂಟು_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } ಅನ್ಯಥಾ {
            ಗಂಟು_STATE(ksSchedulerAction) = target;
        }
#ifdef CONFIG_KERNEL_MCS
    }
#endif

}

ಶೂನ್ಯ setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

ಶೂನ್ಯ scheduleTCB(tcb_t *tptr)
{
    ಯದಿ (tptr == ಗಂಟು_STATE(ksCurThread) &&
        ಗಂಟು_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        ಗಂಟು_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#ifdef CONFIG_KERNEL_MCS
ಶೂನ್ಯ postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ಪ್ರತಿಜ್ಞಾ(tcb != ರಿಕ್ತ);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    ಗಂಟು_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

ಶೂನ್ಯ setNextInterrupt(ಶೂನ್ಯ)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_head_refill = *refill_head(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext);
    ticks_t next_interrupt = ಗಂಟು_STATE(ksCurTime) + ct_head_refill.rAmount;

    ಯದಿ (ಅಂಕೆ_Domains > 1) {
        next_interrupt = MIN(next_interrupt, ಗಂಟು_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_head = ಗಂಟು_STATE(ksReleaseQueue.head);
    ಯದಿ (rlq_head != ರಿಕ್ತ) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_head_refill = *refill_head(rlq_head->tcbSchedContext);
        next_interrupt = MIN(rlq_head_refill.rTime, next_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    ಪ್ರತಿಜ್ಞಾ(next_interrupt >= ಗಂಟು_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(next_interrupt - getTimerPrecision());
}

ಶೂನ್ಯ chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    ಯದಿ (likely(ಗಂಟು_STATE(ksCurSC) != ಗಂಟು_STATE(ksIdleSC))) {
        ಯದಿ (isRoundRobin(ಗಂಟು_STATE(ksCurSC))) {
            ಪ್ರತಿಜ್ಞಾ(refill_size(ಗಂಟು_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t head = *refill_head(ಗಂಟು_STATE(ksCurSC));
            refill_t tail = *refill_tail(ಗಂಟು_STATE(ksCurSC));
            refill_head(ಗಂಟು_STATE(ksCurSC))->rAmount = head.rAmount + tail.rAmount;
            refill_tail(ಗಂಟು_STATE(ksCurSC))->rAmount = 0;
        } ಅನ್ಯಥಾ {
            refill_budget_check(consumed);
        }

        ಪ್ರತಿಜ್ಞಾ(refill_head(ಗಂಟು_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        ಗಂಟು_STATE(ksCurSC)->scConsumed += consumed;
    }
    ಗಂಟು_STATE(ksConsumed) = 0;
    ಯದಿ (likely(isSchedulable(ಗಂಟು_STATE(ksCurThread)))) {
        ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext == ಗಂಟು_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        ಗಂಟು_STATE(ksReprogram) = true;
    }
}

ಶೂನ್ಯ endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(ಗಂಟು_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(ಗಂಟು_STATE(ksCurThread));

    ಯದಿ (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(ಗಂಟು_STATE(ksCurSC)->scBadge);
        handleTimeout(ಗಂಟು_STATE(ksCurThread));
    } ಅನ್ಯಥಾ ಯದಿ (refill_ready(ಗಂಟು_STATE(ksCurSC)) && refill_sufficient(ಗಂಟು_STATE(ksCurSC), 0)) {
        /* apply round robin */
        ಪ್ರತಿಜ್ಞಾ(!thread_state_get_tcbQueued(ಗಂಟು_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } ಅನ್ಯಥಾ {
        /* postpone until ready */
        postpone(ಗಂಟು_STATE(ksCurSC));
    }
}
#ಅನ್ಯಥಾ

ಶೂನ್ಯ timerTick(ಶೂನ್ಯ)
{
    ಯದಿ (likely(thread_state_get_tsType(ಗಂಟು_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ifdef CONFIG_VTX
        || thread_state_get_tsType(ಗಂಟು_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#endif
       ) {
        ಯದಿ (ಗಂಟು_STATE(ksCurThread)->tcbTimeSlice > 1) {
            ಗಂಟು_STATE(ksCurThread)->tcbTimeSlice--;
        } ಅನ್ಯಥಾ {
            ಗಂಟು_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    ಯದಿ (ಅಂಕೆ_Domains > 1) {
        ksDomainTime--;
        ಯದಿ (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#endif

ಶೂನ್ಯ rescheduleRequired(ಶೂನ್ಯ)
{
    ಯದಿ (ಗಂಟು_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && ಗಂಟು_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#ifdef CONFIG_KERNEL_MCS
        && isSchedulable(ಗಂಟು_STATE(ksSchedulerAction))
#endif
       ) {
#ifdef CONFIG_KERNEL_MCS
        ಪ್ರತಿಜ್ಞಾ(refill_sufficient(ಗಂಟು_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        ಪ್ರತಿಜ್ಞಾ(refill_ready(ಗಂಟು_STATE(ksSchedulerAction)->tcbSchedContext));
#endif
        SCHED_ENQUEUE(ಗಂಟು_STATE(ksSchedulerAction));
    }
    ಗಂಟು_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#ifdef CONFIG_KERNEL_MCS

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t PURE release_q_non_empty_and_ready(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ಗಂಟು_STATE(ksReleaseQueue.head) != ರಿಕ್ತ
           && refill_ready(ಗಂಟು_STATE(ksReleaseQueue.head)->tcbSchedContext);
}

ಸ್ಥಿರ ಶೂನ್ಯ tcbReleaseDequeue(ಶೂನ್ಯ)
{
    ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE(ksReleaseQueue.head) != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE(ksReleaseQueue.head)->tcbSchedPrev == ರಿಕ್ತ);
    SMP_COND_STATEMENT(ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE(ksReleaseQueue.head)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = ಗಂಟು_STATE(ksReleaseQueue.head);
    ಪ್ರತಿಜ್ಞಾ(awakened != ಗಂಟು_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    ಪ್ರತಿಜ್ಞಾ(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(ಪ್ರತಿಜ್ಞಾ(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    ಪ್ರತಿಜ್ಞಾ(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

ಶೂನ್ಯ awaken(ಶೂನ್ಯ)
{
    ಯಾವತ್ (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#endif
