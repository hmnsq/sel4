/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <object.h>
#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <api/faults.h>
#અન્તર્ભાવ <api/types.h>
#અન્તર્ભાવ <kernel/cspace.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <object/domain.h>
#ifdef CONFIG_KERNEL_MCS
#અન્તર્ભાવ <object/schedcontext.h>
#endif
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <arch/kernel/thread.h>
#અન્તર્ભાવ <machine/registerset.h>
#અન્તર્ભાવ <machine/fpu.h>
#અન્તર્ભાવ <linker.h>

સ્થિર seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receiveBuffer);

BOOT_CODE શૂન્ય configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

શૂન્ય activateThread(શૂન્ય)
{
#ifdef CONFIG_KERNEL_MCS
    જો (unlikely(ગાંઠ_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(ગાંઠ_STATE(ksCurThread));
        પ્રતિજ્ઞા(thread_state_get_tsType(ગાંઠ_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#endif

    પસંદગી (thread_state_get_tsType(ગાંઠ_STATE(ksCurThread)->tcbState)) {
    સ્થિતિ ThreadState_Running:
#ifdef CONFIG_VTX
    સ્થિતિ ThreadState_RunningVM:
#endif
        તોડો;

    સ્થિતિ ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(ગાંઠ_STATE(ksCurThread));
        setNextPC(ગાંઠ_STATE(ksCurThread), pc);
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
        તોડો;
    }

    સ્થિતિ ThreadState_IdleThreadState:
        Arch_activateIdleThread(ગાંઠ_STATE(ksCurThread));
        તોડો;

    મૂળભૂત:
        fail("Current thread is blocked");
    }
}

શૂન્ય suspend(tcb_t *target)
{
    cancelIPC(target);
    જો (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
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

શૂન્ય restart(tcb_t *target)
{
    જો (isStopped(target)) {
        cancelIPC(target);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        જો (sc_sporadic(sc) && sc != ગાંઠ_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        જો (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#નહીંતર
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#endif
    }
}

શૂન્ય doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    શૂન્ય *receiveBuffer, *sendBuffer;

    receiveBuffer = lookupIPCBuffer(true, receiver);

    જો (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        sendBuffer = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, sendBuffer, endpoint, badge, grant,
                         receiver, receiveBuffer);
    } નહીંતર {
        doFaultTransfer(badge, sender, receiver, receiveBuffer);
    }
}

#ifdef CONFIG_KERNEL_MCS
શૂન્ય doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#નહીંતર
શૂન્ય doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#endif
{
#ifdef CONFIG_KERNEL_MCS
    જો (reply->replyTCB == ખાલી ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        પાછા;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    પ્રતિજ્ઞા(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    પ્રતિજ્ઞા(reply->replyTCB == ખાલી);

    જો (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != ગાંઠ_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#નહીંતર
    પ્રતિજ્ઞા(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#endif

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    જો (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, ખાલી, 0, grant, receiver);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#નહીંતર
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#endif
    } નહીંતર {
#ifndef CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#endif
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        જો (restart) {
            setThreadState(receiver, ThreadState_Restart);
#ifndef CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#endif
        } નહીંતર {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    જો (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        જો ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } નહીંતર {
            જો (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } નહીંતર {
                postpone(sc);
            }
        }
    }
#endif
}

શૂન્ય doNormalTransfer(tcb_t *sender, word_t *sendBuffer, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receiveBuffer)
{
    word_t સંદેશ_Transferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, સંદેશ_Info_Register));

    જો (canGrant) {
        status = lookupExtraCaps(sender, sendBuffer, tag);
        જો (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = ખાલી;
        }
    } નહીંતર {
        current_extra_caps.excaprefs[0] = ખાલી;
    }

    સંદેશ_Transferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
                             seL4_MessageInfo_get_length(tag));

    tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

    tag = seL4_MessageInfo_set_length(tag, સંદેશ_Transferred);
    setRegister(receiver, સંદેશ_Info_Register, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

શૂન્ય doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t sent;
    seL4_MessageInfo_t સંદેશ_Info;

    sent = setMRs_fault(sender, receiver, receiverIPCBuffer);
    સંદેશ_Info = seL4_MessageInfo_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, sent);
    setRegister(receiver, સંદેશ_Info_Register, wordFromMessageInfo(સંદેશ_Info));
    setRegister(receiver, badgeRegister, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
સ્થિર seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receiveBuffer)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    જો (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
        પાછા info;
    }

    destSlot = getReceiveSlots(receiver, receiveBuffer);

    માટે (i = 0; i < se_L4_સંદેશ_Max_Extra_Caps && current_extra_caps.excaprefs[i] != ખાલી; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        જો (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receiveBuffer,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } નહીંતર {
            deriveCap_ret_t dc_ret;

            જો (!destSlot) {
                તોડો;
            }

            dc_ret = deriveCap(slot, cap);

            જો (dc_ret.status != EXCEPTION_NONE) {
                તોડો;
            }
            જો (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                તોડો;
            }

            cte_ઉમેરો(dc_ret.cap, slot, destSlot);

            destSlot = ખાલી;
        }
    }

    પાછા seL4_MessageInfo_set_extraCaps(info, i);
}

શૂન્ય doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

શૂન્ય prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    જો (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#ifdef CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#endif
    }
}

સ્થિર શૂન્ય prepareNextDomain(શૂન્ય)
{
    Arch_prepareNextDomain();
#ifdef CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(ખાલી);
#endif
}

સ્થિર શૂન્ય nextDomain(શૂન્ય)
{
    ksDomScheduleIdx++;
    જો (dschedule_is_end_marker(ksDomScheduleIdx)) {
        ksDomScheduleIdx = ksDomScheduleStart;
    }
#ifdef CONFIG_KERNEL_MCS
    ગાંઠ_STATE(ksReprogram) = true;
#endif
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ksDomSchedule[ksDomScheduleIdx]);
    ksDomainTime = dschedule_duration(ksDomSchedule[ksDomScheduleIdx]);
}

#ifdef CONFIG_KERNEL_MCS
સ્થિર શૂન્ય switchSchedContext(શૂન્ય)
{
    જો (unlikely(ગાંઠ_STATE(ksCurSC) != ગાંઠ_STATE(ksCurThread)->tcbSchedContext)) {
        ગાંઠ_STATE(ksReprogram) = true;
        જો (sc_constant_bandwidth(ગાંઠ_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(ગાંઠ_STATE(ksCurThread)->tcbSchedContext);
        }

        પ્રતિજ્ઞા(refill_ready(ગાંઠ_STATE(ksCurThread)->tcbSchedContext));
        પ્રતિજ્ઞા(refill_sufficient(ગાંઠ_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    જો (ગાંઠ_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    ગાંઠ_STATE(ksCurSC) = ગાંઠ_STATE(ksCurThread)->tcbSchedContext;
}
#endif

સ્થિર શૂન્ય scheduleChooseNewThread(શૂન્ય)
{
    જો (ksDomainTime == 0) {
        prepareNextDomain();
        nextDomain();
    }
    chooseThread();
}

શૂન્ય schedule(શૂન્ય)
{
#ifdef CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#endif

    જો (ગાંઠ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        જો (isSchedulable(ગાંઠ_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } નહીંતર {
            was_runnable = false;
        }

        જો (ગાંઠ_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } નહીંતર {
            tcb_t *candidate = ગાંઠ_STATE(ksSchedulerAction);
            પ્રતિજ્ઞા(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                ગાંઠ_STATE(ksCurThread) == ગાંઠ_STATE(ksIdleThread)
                || (candidate->tcbPriority < ગાંઠ_STATE(ksCurThread)->tcbPriority);
            જો (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                ગાંઠ_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } નહીંતર જો (was_runnable && candidate->tcbPriority == ગાંઠ_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                ગાંઠ_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } નહીંતર {
                પ્રતિજ્ઞા(candidate != ગાંઠ_STATE(ksCurThread));
                switchToThread(candidate);
            }
        }
    }
    ગાંઠ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ifdef ENABLE_SMP_SUPPORT
    doMaskReschedule(ARCH_ગાંઠ_STATE(ipiReschedulePending));
    ARCH_ગાંઠ_STATE(ipiReschedulePending) = 0;
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_KERNEL_MCS
    switchSchedContext();

    જો (ગાંઠ_STATE(ksReprogram)) {
        setNextInterrupt();
        ગાંઠ_STATE(ksReprogram) = false;
    }
#endif
}

શૂન્ય chooseThread(શૂન્ય)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    જો (અંક_Domains > 1) {
        dom = ksCurDomain;
    } નહીંતર {
        dom = 0;
    }

    જો (likely(ગાંઠ_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = ગાંઠ_STATE(ksReadyQueues)[ready_queues_index(dom, prio)].head;
        પ્રતિજ્ઞા(thread);
        પ્રતિજ્ઞા(isSchedulable(thread));
#ifdef CONFIG_KERNEL_MCS
        પ્રતિજ્ઞા(refill_sufficient(thread->tcbSchedContext, 0));
        પ્રતિજ્ઞા(refill_ready(thread->tcbSchedContext));
#endif
        switchToThread(thread);
    } નહીંતર {
        switchToIdleThread();
    }
}

શૂન્ય switchToThread(tcb_t *thread)
{
#ifdef CONFIG_KERNEL_MCS
    પ્રતિજ્ઞા(thread->tcbSchedContext != ખાલી);
    પ્રતિજ્ઞા(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    પ્રતિજ્ઞા(refill_sufficient(thread->tcbSchedContext, 0));
    પ્રતિજ્ઞા(refill_ready(thread->tcbSchedContext));
#endif

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ગાંઠ_STATE(ksCurThread), thread);
#endif
    Arch_switchToThread(thread);

#ifdef CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#endif /* CONFIG_HAVE_FPU */

    tcbSchedDequeue(thread);
    ગાંઠ_STATE(ksCurThread) = thread;
}

શૂન્ય switchToIdleThread(શૂન્ય)
{
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ગાંઠ_STATE(ksCurThread), ગાંઠ_STATE(ksIdleThread));
#endif
    Arch_switchToIdleThread();
    ગાંઠ_STATE(ksCurThread) = ગાંઠ_STATE(ksIdleThread);
}

શૂન્ય setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    જો (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    જો (tptr == ગાંઠ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

શૂન્ય setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ifdef CONFIG_KERNEL_MCS
શૂન્ય setPriority(tcb_t *tptr, prio_t prio)
{
    પસંદગી (thread_state_get_tsType(tptr->tcbState)) {
    સ્થિતિ ThreadState_Running:
    સ્થિતિ ThreadState_Restart:
        જો (thread_state_get_tcbQueued(tptr->tcbState) || tptr == ગાંઠ_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } નહીંતર {
            tptr->tcbPriority = prio;
        }
        તોડો;
    સ્થિતિ ThreadState_BlockedOnReceive:
    સ્થિતિ ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        તોડો;
    સ્થિતિ ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        તોડો;
    મૂળભૂત:
        tptr->tcbPriority = prio;
        તોડો;
    }
}
#નહીંતર
શૂન્ય setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    જો (isRunnable(tptr)) {
        જો (tptr == ગાંઠ_STATE(ksCurThread)) {
            rescheduleRequired();
        } નહીંતર {
            possibleSwitchTo(tptr);
        }
    }
}
#endif

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
શૂન્ય possibleSwitchTo(tcb_t *target)
{
#ifdef CONFIG_KERNEL_MCS
    જો (target->tcbSchedContext != ખાલી && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#endif
        જો (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } નહીંતર જો (ગાંઠ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } નહીંતર {
            ગાંઠ_STATE(ksSchedulerAction) = target;
        }
#ifdef CONFIG_KERNEL_MCS
    }
#endif

}

શૂન્ય setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

શૂન્ય scheduleTCB(tcb_t *tptr)
{
    જો (tptr == ગાંઠ_STATE(ksCurThread) &&
        ગાંઠ_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        ગાંઠ_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#ifdef CONFIG_KERNEL_MCS
શૂન્ય postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    પ્રતિજ્ઞા(tcb != ખાલી);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    ગાંઠ_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

શૂન્ય setNextInterrupt(શૂન્ય)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_head_refill = *refill_head(ગાંઠ_STATE(ksCurThread)->tcbSchedContext);
    ticks_t next_interrupt = ગાંઠ_STATE(ksCurTime) + ct_head_refill.rAmount;

    જો (અંક_Domains > 1) {
        next_interrupt = MIN(next_interrupt, ગાંઠ_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_head = ગાંઠ_STATE(ksReleaseQueue.head);
    જો (rlq_head != ખાલી) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_head_refill = *refill_head(rlq_head->tcbSchedContext);
        next_interrupt = MIN(rlq_head_refill.rTime, next_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    પ્રતિજ્ઞા(next_interrupt >= ગાંઠ_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(next_interrupt - getTimerPrecision());
}

શૂન્ય chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    જો (likely(ગાંઠ_STATE(ksCurSC) != ગાંઠ_STATE(ksIdleSC))) {
        જો (isRoundRobin(ગાંઠ_STATE(ksCurSC))) {
            પ્રતિજ્ઞા(refill_size(ગાંઠ_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t head = *refill_head(ગાંઠ_STATE(ksCurSC));
            refill_t tail = *refill_tail(ગાંઠ_STATE(ksCurSC));
            refill_head(ગાંઠ_STATE(ksCurSC))->rAmount = head.rAmount + tail.rAmount;
            refill_tail(ગાંઠ_STATE(ksCurSC))->rAmount = 0;
        } નહીંતર {
            refill_budget_check(consumed);
        }

        પ્રતિજ્ઞા(refill_head(ગાંઠ_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        ગાંઠ_STATE(ksCurSC)->scConsumed += consumed;
    }
    ગાંઠ_STATE(ksConsumed) = 0;
    જો (likely(isSchedulable(ગાંઠ_STATE(ksCurThread)))) {
        પ્રતિજ્ઞા(ગાંઠ_STATE(ksCurThread)->tcbSchedContext == ગાંઠ_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        ગાંઠ_STATE(ksReprogram) = true;
    }
}

શૂન્ય endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(ગાંઠ_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(ગાંઠ_STATE(ksCurThread));

    જો (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(ગાંઠ_STATE(ksCurSC)->scBadge);
        handleTimeout(ગાંઠ_STATE(ksCurThread));
    } નહીંતર જો (refill_ready(ગાંઠ_STATE(ksCurSC)) && refill_sufficient(ગાંઠ_STATE(ksCurSC), 0)) {
        /* apply round robin */
        પ્રતિજ્ઞા(!thread_state_get_tcbQueued(ગાંઠ_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } નહીંતર {
        /* postpone until ready */
        postpone(ગાંઠ_STATE(ksCurSC));
    }
}
#નહીંતર

શૂન્ય timerTick(શૂન્ય)
{
    જો (likely(thread_state_get_tsType(ગાંઠ_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ifdef CONFIG_VTX
        || thread_state_get_tsType(ગાંઠ_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#endif
       ) {
        જો (ગાંઠ_STATE(ksCurThread)->tcbTimeSlice > 1) {
            ગાંઠ_STATE(ksCurThread)->tcbTimeSlice--;
        } નહીંતર {
            ગાંઠ_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    જો (અંક_Domains > 1) {
        ksDomainTime--;
        જો (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#endif

શૂન્ય rescheduleRequired(શૂન્ય)
{
    જો (ગાંઠ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && ગાંઠ_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#ifdef CONFIG_KERNEL_MCS
        && isSchedulable(ગાંઠ_STATE(ksSchedulerAction))
#endif
       ) {
#ifdef CONFIG_KERNEL_MCS
        પ્રતિજ્ઞા(refill_sufficient(ગાંઠ_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        પ્રતિજ્ઞા(refill_ready(ગાંઠ_STATE(ksSchedulerAction)->tcbSchedContext));
#endif
        SCHED_ENQUEUE(ગાંઠ_STATE(ksSchedulerAction));
    }
    ગાંઠ_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#ifdef CONFIG_KERNEL_MCS

સ્થિર અંતઃરેખ bool_t PURE release_q_non_empty_and_ready(શૂન્ય)
{
    પાછા ગાંઠ_STATE(ksReleaseQueue.head) != ખાલી
           && refill_ready(ગાંઠ_STATE(ksReleaseQueue.head)->tcbSchedContext);
}

સ્થિર શૂન્ય tcbReleaseDequeue(શૂન્ય)
{
    પ્રતિજ્ઞા(ગાંઠ_STATE(ksReleaseQueue.head) != ખાલી);
    પ્રતિજ્ઞા(ગાંઠ_STATE(ksReleaseQueue.head)->tcbSchedPrev == ખાલી);
    SMP_COND_STATEMENT(પ્રતિજ્ઞા(ગાંઠ_STATE(ksReleaseQueue.head)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = ગાંઠ_STATE(ksReleaseQueue.head);
    પ્રતિજ્ઞા(awakened != ગાંઠ_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    પ્રતિજ્ઞા(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(પ્રતિજ્ઞા(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    પ્રતિજ્ઞા(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

શૂન્ય awaken(શૂન્ય)
{
    જ્યારે (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#endif
