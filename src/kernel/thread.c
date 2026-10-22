/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <object.h>
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <api/faults.h>
#அந்தர்பாவ <api/types.h>
#அந்தர்பாவ <kernel/cspace.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <kernel/vspace.h>
#அந்தர்பாவ <object/domain.h>
#ifdef CONFIG_KERNEL_MCS
#அந்தர்பாவ <object/schedcontext.h>
#endif
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <arch/machine.h>
#அந்தர்பாவ <arch/kernel/thread.h>
#அந்தர்பாவ <machine/registerset.h>
#அந்தர்பாவ <machine/fpu.h>
#அந்தர்பாவ <linker.h>

நிலையான seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receiveBuffer);

BOOT_CODE வெறுமை configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

வெறுமை activateThread(வெறுமை)
{
#ifdef CONFIG_KERNEL_MCS
    எனில் (unlikely(முனை_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(முனை_STATE(ksCurThread));
        ப்ரதிஜ்ஞா(thread_state_get_tsType(முனை_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#endif

    தேர்வு (thread_state_get_tsType(முனை_STATE(ksCurThread)->tcbState)) {
    நிலை ThreadState_Running:
#ifdef CONFIG_VTX
    நிலை ThreadState_RunningVM:
#endif
        நிறுத்து;

    நிலை ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(முனை_STATE(ksCurThread));
        setNextPC(முனை_STATE(ksCurThread), pc);
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
        நிறுத்து;
    }

    நிலை ThreadState_IdleThreadState:
        Arch_activateIdleThread(முனை_STATE(ksCurThread));
        நிறுத்து;

    இயல்பு:
        fail("Current thread is blocked");
    }
}

வெறுமை suspend(tcb_t *target)
{
    cancelIPC(target);
    எனில் (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
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

வெறுமை restart(tcb_t *target)
{
    எனில் (isStopped(target)) {
        cancelIPC(target);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        எனில் (sc_sporadic(sc) && sc != முனை_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        எனில் (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#இல்லையெனில்
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#endif
    }
}

வெறுமை doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    வெறுமை *receiveBuffer, *sendBuffer;

    receiveBuffer = lookupIPCBuffer(true, receiver);

    எனில் (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        sendBuffer = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, sendBuffer, endpoint, badge, grant,
                         receiver, receiveBuffer);
    } இல்லையெனில் {
        doFaultTransfer(badge, sender, receiver, receiveBuffer);
    }
}

#ifdef CONFIG_KERNEL_MCS
வெறுமை doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#இல்லையெனில்
வெறுமை doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#endif
{
#ifdef CONFIG_KERNEL_MCS
    எனில் (reply->replyTCB == வெற்று ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        திரும்பு;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    ப்ரதிஜ்ஞா(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    ப்ரதிஜ்ஞா(reply->replyTCB == வெற்று);

    எனில் (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != முனை_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#இல்லையெனில்
    ப்ரதிஜ்ஞா(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#endif

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    எனில் (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, வெற்று, 0, grant, receiver);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#இல்லையெனில்
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#endif
    } இல்லையெனில் {
#ifndef CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#endif
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        எனில் (restart) {
            setThreadState(receiver, ThreadState_Restart);
#ifndef CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#endif
        } இல்லையெனில் {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    எனில் (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        எனில் ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } இல்லையெனில் {
            எனில் (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } இல்லையெனில் {
                postpone(sc);
            }
        }
    }
#endif
}

வெறுமை doNormalTransfer(tcb_t *sender, word_t *sendBuffer, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receiveBuffer)
{
    word_t செய்தி_Transferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, செய்தி_Info_Register));

    எனில் (canGrant) {
        status = lookupExtraCaps(sender, sendBuffer, tag);
        எனில் (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = வெற்று;
        }
    } இல்லையெனில் {
        current_extra_caps.excaprefs[0] = வெற்று;
    }

    செய்தி_Transferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
                             seL4_MessageInfo_get_length(tag));

    tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

    tag = seL4_MessageInfo_set_length(tag, செய்தி_Transferred);
    setRegister(receiver, செய்தி_Info_Register, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

வெறுமை doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t sent;
    seL4_MessageInfo_t செய்தி_Info;

    sent = setMRs_fault(sender, receiver, receiverIPCBuffer);
    செய்தி_Info = seL4_MessageInfo_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, sent);
    setRegister(receiver, செய்தி_Info_Register, wordFromMessageInfo(செய்தி_Info));
    setRegister(receiver, badgeRegister, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
நிலையான seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receiveBuffer)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    எனில் (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
        திரும்பு info;
    }

    destSlot = getReceiveSlots(receiver, receiveBuffer);

    க்கு (i = 0; i < se_L4_செய்தி_Max_Extra_Caps && current_extra_caps.excaprefs[i] != வெற்று; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        எனில் (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receiveBuffer,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } இல்லையெனில் {
            deriveCap_ret_t dc_ret;

            எனில் (!destSlot) {
                நிறுத்து;
            }

            dc_ret = deriveCap(slot, cap);

            எனில் (dc_ret.status != EXCEPTION_NONE) {
                நிறுத்து;
            }
            எனில் (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                நிறுத்து;
            }

            cte_செருகு(dc_ret.cap, slot, destSlot);

            destSlot = வெற்று;
        }
    }

    திரும்பு seL4_MessageInfo_set_extraCaps(info, i);
}

வெறுமை doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

வெறுமை prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    எனில் (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#ifdef CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#endif
    }
}

நிலையான வெறுமை prepareNextDomain(வெறுமை)
{
    Arch_prepareNextDomain();
#ifdef CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(வெற்று);
#endif
}

நிலையான வெறுமை nextDomain(வெறுமை)
{
    ksDomScheduleIdx++;
    எனில் (dschedule_is_end_marker(ksDomScheduleIdx)) {
        ksDomScheduleIdx = ksDomScheduleStart;
    }
#ifdef CONFIG_KERNEL_MCS
    முனை_STATE(ksReprogram) = true;
#endif
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ksDomSchedule[ksDomScheduleIdx]);
    ksDomainTime = dschedule_duration(ksDomSchedule[ksDomScheduleIdx]);
}

#ifdef CONFIG_KERNEL_MCS
நிலையான வெறுமை switchSchedContext(வெறுமை)
{
    எனில் (unlikely(முனை_STATE(ksCurSC) != முனை_STATE(ksCurThread)->tcbSchedContext)) {
        முனை_STATE(ksReprogram) = true;
        எனில் (sc_constant_bandwidth(முனை_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(முனை_STATE(ksCurThread)->tcbSchedContext);
        }

        ப்ரதிஜ்ஞா(refill_ready(முனை_STATE(ksCurThread)->tcbSchedContext));
        ப்ரதிஜ்ஞா(refill_sufficient(முனை_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    எனில் (முனை_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    முனை_STATE(ksCurSC) = முனை_STATE(ksCurThread)->tcbSchedContext;
}
#endif

நிலையான வெறுமை scheduleChooseNewThread(வெறுமை)
{
    எனில் (ksDomainTime == 0) {
        prepareNextDomain();
        nextDomain();
    }
    chooseThread();
}

வெறுமை schedule(வெறுமை)
{
#ifdef CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#endif

    எனில் (முனை_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        எனில் (isSchedulable(முனை_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } இல்லையெனில் {
            was_runnable = false;
        }

        எனில் (முனை_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } இல்லையெனில் {
            tcb_t *candidate = முனை_STATE(ksSchedulerAction);
            ப்ரதிஜ்ஞா(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                முனை_STATE(ksCurThread) == முனை_STATE(ksIdleThread)
                || (candidate->tcbPriority < முனை_STATE(ksCurThread)->tcbPriority);
            எனில் (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                முனை_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } இல்லையெனில் எனில் (was_runnable && candidate->tcbPriority == முனை_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                முனை_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } இல்லையெனில் {
                ப்ரதிஜ்ஞா(candidate != முனை_STATE(ksCurThread));
                switchToThread(candidate);
            }
        }
    }
    முனை_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ifdef ENABLE_SMP_SUPPORT
    doMaskReschedule(ARCH_முனை_STATE(ipiReschedulePending));
    ARCH_முனை_STATE(ipiReschedulePending) = 0;
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_KERNEL_MCS
    switchSchedContext();

    எனில் (முனை_STATE(ksReprogram)) {
        setNextInterrupt();
        முனை_STATE(ksReprogram) = false;
    }
#endif
}

வெறுமை chooseThread(வெறுமை)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    எனில் (இலக்கம்_Domains > 1) {
        dom = ksCurDomain;
    } இல்லையெனில் {
        dom = 0;
    }

    எனில் (likely(முனை_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = முனை_STATE(ksReadyQueues)[ready_queues_index(dom, prio)].head;
        ப்ரதிஜ்ஞா(thread);
        ப்ரதிஜ்ஞா(isSchedulable(thread));
#ifdef CONFIG_KERNEL_MCS
        ப்ரதிஜ்ஞா(refill_sufficient(thread->tcbSchedContext, 0));
        ப்ரதிஜ்ஞா(refill_ready(thread->tcbSchedContext));
#endif
        switchToThread(thread);
    } இல்லையெனில் {
        switchToIdleThread();
    }
}

வெறுமை switchToThread(tcb_t *thread)
{
#ifdef CONFIG_KERNEL_MCS
    ப்ரதிஜ்ஞா(thread->tcbSchedContext != வெற்று);
    ப்ரதிஜ்ஞா(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    ப்ரதிஜ்ஞா(refill_sufficient(thread->tcbSchedContext, 0));
    ப்ரதிஜ்ஞா(refill_ready(thread->tcbSchedContext));
#endif

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(முனை_STATE(ksCurThread), thread);
#endif
    Arch_switchToThread(thread);

#ifdef CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#endif /* CONFIG_HAVE_FPU */

    tcbSchedDequeue(thread);
    முனை_STATE(ksCurThread) = thread;
}

வெறுமை switchToIdleThread(வெறுமை)
{
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(முனை_STATE(ksCurThread), முனை_STATE(ksIdleThread));
#endif
    Arch_switchToIdleThread();
    முனை_STATE(ksCurThread) = முனை_STATE(ksIdleThread);
}

வெறுமை setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    எனில் (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    எனில் (tptr == முனை_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

வெறுமை setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ifdef CONFIG_KERNEL_MCS
வெறுமை setPriority(tcb_t *tptr, prio_t prio)
{
    தேர்வு (thread_state_get_tsType(tptr->tcbState)) {
    நிலை ThreadState_Running:
    நிலை ThreadState_Restart:
        எனில் (thread_state_get_tcbQueued(tptr->tcbState) || tptr == முனை_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } இல்லையெனில் {
            tptr->tcbPriority = prio;
        }
        நிறுத்து;
    நிலை ThreadState_BlockedOnReceive:
    நிலை ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        நிறுத்து;
    நிலை ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        நிறுத்து;
    இயல்பு:
        tptr->tcbPriority = prio;
        நிறுத்து;
    }
}
#இல்லையெனில்
வெறுமை setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    எனில் (isRunnable(tptr)) {
        எனில் (tptr == முனை_STATE(ksCurThread)) {
            rescheduleRequired();
        } இல்லையெனில் {
            possibleSwitchTo(tptr);
        }
    }
}
#endif

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
வெறுமை possibleSwitchTo(tcb_t *target)
{
#ifdef CONFIG_KERNEL_MCS
    எனில் (target->tcbSchedContext != வெற்று && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#endif
        எனில் (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } இல்லையெனில் எனில் (முனை_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } இல்லையெனில் {
            முனை_STATE(ksSchedulerAction) = target;
        }
#ifdef CONFIG_KERNEL_MCS
    }
#endif

}

வெறுமை setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

வெறுமை scheduleTCB(tcb_t *tptr)
{
    எனில் (tptr == முனை_STATE(ksCurThread) &&
        முனை_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        முனை_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#ifdef CONFIG_KERNEL_MCS
வெறுமை postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ப்ரதிஜ்ஞா(tcb != வெற்று);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    முனை_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

வெறுமை setNextInterrupt(வெறுமை)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_head_refill = *refill_head(முனை_STATE(ksCurThread)->tcbSchedContext);
    ticks_t next_interrupt = முனை_STATE(ksCurTime) + ct_head_refill.rAmount;

    எனில் (இலக்கம்_Domains > 1) {
        next_interrupt = MIN(next_interrupt, முனை_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_head = முனை_STATE(ksReleaseQueue.head);
    எனில் (rlq_head != வெற்று) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_head_refill = *refill_head(rlq_head->tcbSchedContext);
        next_interrupt = MIN(rlq_head_refill.rTime, next_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    ப்ரதிஜ்ஞா(next_interrupt >= முனை_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(next_interrupt - getTimerPrecision());
}

வெறுமை chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    எனில் (likely(முனை_STATE(ksCurSC) != முனை_STATE(ksIdleSC))) {
        எனில் (isRoundRobin(முனை_STATE(ksCurSC))) {
            ப்ரதிஜ்ஞா(refill_size(முனை_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t head = *refill_head(முனை_STATE(ksCurSC));
            refill_t tail = *refill_tail(முனை_STATE(ksCurSC));
            refill_head(முனை_STATE(ksCurSC))->rAmount = head.rAmount + tail.rAmount;
            refill_tail(முனை_STATE(ksCurSC))->rAmount = 0;
        } இல்லையெனில் {
            refill_budget_check(consumed);
        }

        ப்ரதிஜ்ஞா(refill_head(முனை_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        முனை_STATE(ksCurSC)->scConsumed += consumed;
    }
    முனை_STATE(ksConsumed) = 0;
    எனில் (likely(isSchedulable(முனை_STATE(ksCurThread)))) {
        ப்ரதிஜ்ஞா(முனை_STATE(ksCurThread)->tcbSchedContext == முனை_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        முனை_STATE(ksReprogram) = true;
    }
}

வெறுமை endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(முனை_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(முனை_STATE(ksCurThread));

    எனில் (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(முனை_STATE(ksCurSC)->scBadge);
        handleTimeout(முனை_STATE(ksCurThread));
    } இல்லையெனில் எனில் (refill_ready(முனை_STATE(ksCurSC)) && refill_sufficient(முனை_STATE(ksCurSC), 0)) {
        /* apply round robin */
        ப்ரதிஜ்ஞா(!thread_state_get_tcbQueued(முனை_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } இல்லையெனில் {
        /* postpone until ready */
        postpone(முனை_STATE(ksCurSC));
    }
}
#இல்லையெனில்

வெறுமை timerTick(வெறுமை)
{
    எனில் (likely(thread_state_get_tsType(முனை_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ifdef CONFIG_VTX
        || thread_state_get_tsType(முனை_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#endif
       ) {
        எனில் (முனை_STATE(ksCurThread)->tcbTimeSlice > 1) {
            முனை_STATE(ksCurThread)->tcbTimeSlice--;
        } இல்லையெனில் {
            முனை_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    எனில் (இலக்கம்_Domains > 1) {
        ksDomainTime--;
        எனில் (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#endif

வெறுமை rescheduleRequired(வெறுமை)
{
    எனில் (முனை_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && முனை_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#ifdef CONFIG_KERNEL_MCS
        && isSchedulable(முனை_STATE(ksSchedulerAction))
#endif
       ) {
#ifdef CONFIG_KERNEL_MCS
        ப்ரதிஜ்ஞா(refill_sufficient(முனை_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        ப்ரதிஜ்ஞா(refill_ready(முனை_STATE(ksSchedulerAction)->tcbSchedContext));
#endif
        SCHED_ENQUEUE(முனை_STATE(ksSchedulerAction));
    }
    முனை_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#ifdef CONFIG_KERNEL_MCS

நிலையான உள்வரி bool_t PURE release_q_non_empty_and_ready(வெறுமை)
{
    திரும்பு முனை_STATE(ksReleaseQueue.head) != வெற்று
           && refill_ready(முனை_STATE(ksReleaseQueue.head)->tcbSchedContext);
}

நிலையான வெறுமை tcbReleaseDequeue(வெறுமை)
{
    ப்ரதிஜ்ஞா(முனை_STATE(ksReleaseQueue.head) != வெற்று);
    ப்ரதிஜ்ஞா(முனை_STATE(ksReleaseQueue.head)->tcbSchedPrev == வெற்று);
    SMP_COND_STATEMENT(ப்ரதிஜ்ஞா(முனை_STATE(ksReleaseQueue.head)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = முனை_STATE(ksReleaseQueue.head);
    ப்ரதிஜ்ஞா(awakened != முனை_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    ப்ரதிஜ்ஞா(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(ப்ரதிஜ்ஞா(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    ப்ரதிஜ்ஞா(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

வெறுமை awaken(வெறுமை)
{
    போது (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#endif
