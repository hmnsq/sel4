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
#ifdef CONFIG_KERNEL_MCS
#شامل <object/schedcontext.h>
#endif
#شامل <model/statedata.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/thread.h>
#شامل <machine/registerset.h>
#شامل <machine/fpu.h>
#شامل <linker.h>

ایستا seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receiveBuffer);

BOOT_CODE تهی configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

تهی activateThread(تهی)
{
#ifdef CONFIG_KERNEL_MCS
    اگر (unlikely(گره_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(گره_STATE(ksCurThread));
        أكد(thread_state_get_tsType(گره_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#endif

    انتخاب (thread_state_get_tsType(گره_STATE(ksCurThread)->tcbState)) {
    حالت ThreadState_Running:
#ifdef CONFIG_VTX
    حالت ThreadState_RunningVM:
#endif
        بشکن;

    حالت ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(گره_STATE(ksCurThread));
        setNextPC(گره_STATE(ksCurThread), pc);
        setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
        بشکن;
    }

    حالت ThreadState_IdleThreadState:
        Arch_activateIdleThread(گره_STATE(ksCurThread));
        بشکن;

    پیشفرض:
        fail("Current thread is blocked");
    }
}

تهی suspend(tcb_t *target)
{
    cancelIPC(target);
    اگر (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
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

تهی restart(tcb_t *target)
{
    اگر (isStopped(target)) {
        cancelIPC(target);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        اگر (sc_sporadic(sc) && sc != گره_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        اگر (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#وگرنه
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#endif
    }
}

تهی doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    تهی *receiveBuffer, *sendBuffer;

    receiveBuffer = lookupIPCBuffer(true, receiver);

    اگر (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        sendBuffer = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, sendBuffer, endpoint, badge, grant,
                         receiver, receiveBuffer);
    } وگرنه {
        doFaultTransfer(badge, sender, receiver, receiveBuffer);
    }
}

#ifdef CONFIG_KERNEL_MCS
تهی doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#وگرنه
تهی doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#endif
{
#ifdef CONFIG_KERNEL_MCS
    اگر (reply->replyTCB == پوچ ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        بازگشت;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    أكد(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    أكد(reply->replyTCB == پوچ);

    اگر (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != گره_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#وگرنه
    أكد(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#endif

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    اگر (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, پوچ, 0, grant, receiver);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#وگرنه
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#endif
    } وگرنه {
#ifndef CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#endif
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        اگر (restart) {
            setThreadState(receiver, ThreadState_Restart);
#ifndef CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#endif
        } وگرنه {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    اگر (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        اگر ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } وگرنه {
            اگر (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } وگرنه {
                postpone(sc);
            }
        }
    }
#endif
}

تهی doNormalTransfer(tcb_t *sender, word_t *sendBuffer, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receiveBuffer)
{
    word_t پیام_Transferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, پیام_Info_Register));

    اگر (canGrant) {
        status = lookupExtraCaps(sender, sendBuffer, tag);
        اگر (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = پوچ;
        }
    } وگرنه {
        current_extra_caps.excaprefs[0] = پوچ;
    }

    پیام_Transferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
                             seL4_MessageInfo_get_length(tag));

    tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

    tag = seL4_MessageInfo_set_length(tag, پیام_Transferred);
    setRegister(receiver, پیام_Info_Register, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

تهی doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t sent;
    seL4_MessageInfo_t پیام_Info;

    sent = setMRs_fault(sender, receiver, receiverIPCBuffer);
    پیام_Info = seL4_MessageInfo_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, sent);
    setRegister(receiver, پیام_Info_Register, wordFromMessageInfo(پیام_Info));
    setRegister(receiver, badgeRegister, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ایستا seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receiveBuffer)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    اگر (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
        بازگشت info;
    }

    destSlot = getReceiveSlots(receiver, receiveBuffer);

    برای (i = 0; i < se_L4_پیام_Max_Extra_Caps && current_extra_caps.excaprefs[i] != پوچ; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        اگر (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receiveBuffer,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } وگرنه {
            deriveCap_ret_t dc_ret;

            اگر (!destSlot) {
                بشکن;
            }

            dc_ret = deriveCap(slot, cap);

            اگر (dc_ret.status != EXCEPTION_NONE) {
                بشکن;
            }
            اگر (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                بشکن;
            }

            cte_درج(dc_ret.cap, slot, destSlot);

            destSlot = پوچ;
        }
    }

    بازگشت seL4_MessageInfo_set_extraCaps(info, i);
}

تهی doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

تهی prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    اگر (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#ifdef CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#endif
    }
}

ایستا تهی prepareNextDomain(تهی)
{
    Arch_prepareNextDomain();
#ifdef CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(پوچ);
#endif
}

ایستا تهی nextDomain(تهی)
{
    ksDomScheduleIdx++;
    اگر (dschedule_is_end_marker(ksDomScheduleIdx)) {
        ksDomScheduleIdx = ksDomScheduleStart;
    }
#ifdef CONFIG_KERNEL_MCS
    گره_STATE(ksReprogram) = true;
#endif
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ksDomSchedule[ksDomScheduleIdx]);
    ksDomainTime = dschedule_duration(ksDomSchedule[ksDomScheduleIdx]);
}

#ifdef CONFIG_KERNEL_MCS
ایستا تهی switchSchedContext(تهی)
{
    اگر (unlikely(گره_STATE(ksCurSC) != گره_STATE(ksCurThread)->tcbSchedContext)) {
        گره_STATE(ksReprogram) = true;
        اگر (sc_constant_bandwidth(گره_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(گره_STATE(ksCurThread)->tcbSchedContext);
        }

        أكد(refill_ready(گره_STATE(ksCurThread)->tcbSchedContext));
        أكد(refill_sufficient(گره_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    اگر (گره_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    گره_STATE(ksCurSC) = گره_STATE(ksCurThread)->tcbSchedContext;
}
#endif

ایستا تهی scheduleChooseNewThread(تهی)
{
    اگر (ksDomainTime == 0) {
        prepareNextDomain();
        nextDomain();
    }
    chooseThread();
}

تهی schedule(تهی)
{
#ifdef CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#endif

    اگر (گره_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        اگر (isSchedulable(گره_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } وگرنه {
            was_runnable = false;
        }

        اگر (گره_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } وگرنه {
            tcb_t *candidate = گره_STATE(ksSchedulerAction);
            أكد(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                گره_STATE(ksCurThread) == گره_STATE(ksIdleThread)
                || (candidate->tcbPriority < گره_STATE(ksCurThread)->tcbPriority);
            اگر (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                گره_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } وگرنه اگر (was_runnable && candidate->tcbPriority == گره_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                گره_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } وگرنه {
                أكد(candidate != گره_STATE(ksCurThread));
                switchToThread(candidate);
            }
        }
    }
    گره_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ifdef ENABLE_SMP_SUPPORT
    doMaskReschedule(ARCH_گره_STATE(ipiReschedulePending));
    ARCH_گره_STATE(ipiReschedulePending) = 0;
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_KERNEL_MCS
    switchSchedContext();

    اگر (گره_STATE(ksReprogram)) {
        setNextInterrupt();
        گره_STATE(ksReprogram) = false;
    }
#endif
}

تهی chooseThread(تهی)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    اگر (رقم_Domains > 1) {
        dom = ksCurDomain;
    } وگرنه {
        dom = 0;
    }

    اگر (likely(گره_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = گره_STATE(ksReadyQueues)[ready_queues_index(dom, prio)].head;
        أكد(thread);
        أكد(isSchedulable(thread));
#ifdef CONFIG_KERNEL_MCS
        أكد(refill_sufficient(thread->tcbSchedContext, 0));
        أكد(refill_ready(thread->tcbSchedContext));
#endif
        switchToThread(thread);
    } وگرنه {
        switchToIdleThread();
    }
}

تهی switchToThread(tcb_t *thread)
{
#ifdef CONFIG_KERNEL_MCS
    أكد(thread->tcbSchedContext != پوچ);
    أكد(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    أكد(refill_sufficient(thread->tcbSchedContext, 0));
    أكد(refill_ready(thread->tcbSchedContext));
#endif

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(گره_STATE(ksCurThread), thread);
#endif
    Arch_switchToThread(thread);

#ifdef CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#endif /* CONFIG_HAVE_FPU */

    tcbSchedDequeue(thread);
    گره_STATE(ksCurThread) = thread;
}

تهی switchToIdleThread(تهی)
{
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(گره_STATE(ksCurThread), گره_STATE(ksIdleThread));
#endif
    Arch_switchToIdleThread();
    گره_STATE(ksCurThread) = گره_STATE(ksIdleThread);
}

تهی setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    اگر (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    اگر (tptr == گره_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

تهی setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ifdef CONFIG_KERNEL_MCS
تهی setPriority(tcb_t *tptr, prio_t prio)
{
    انتخاب (thread_state_get_tsType(tptr->tcbState)) {
    حالت ThreadState_Running:
    حالت ThreadState_Restart:
        اگر (thread_state_get_tcbQueued(tptr->tcbState) || tptr == گره_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } وگرنه {
            tptr->tcbPriority = prio;
        }
        بشکن;
    حالت ThreadState_BlockedOnReceive:
    حالت ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        بشکن;
    حالت ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        بشکن;
    پیشفرض:
        tptr->tcbPriority = prio;
        بشکن;
    }
}
#وگرنه
تهی setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    اگر (isRunnable(tptr)) {
        اگر (tptr == گره_STATE(ksCurThread)) {
            rescheduleRequired();
        } وگرنه {
            possibleSwitchTo(tptr);
        }
    }
}
#endif

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
تهی possibleSwitchTo(tcb_t *target)
{
#ifdef CONFIG_KERNEL_MCS
    اگر (target->tcbSchedContext != پوچ && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#endif
        اگر (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } وگرنه اگر (گره_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } وگرنه {
            گره_STATE(ksSchedulerAction) = target;
        }
#ifdef CONFIG_KERNEL_MCS
    }
#endif

}

تهی setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

تهی scheduleTCB(tcb_t *tptr)
{
    اگر (tptr == گره_STATE(ksCurThread) &&
        گره_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        گره_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#ifdef CONFIG_KERNEL_MCS
تهی postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    أكد(tcb != پوچ);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    گره_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

تهی setNextInterrupt(تهی)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_head_refill = *refill_head(گره_STATE(ksCurThread)->tcbSchedContext);
    ticks_t next_interrupt = گره_STATE(ksCurTime) + ct_head_refill.rAmount;

    اگر (رقم_Domains > 1) {
        next_interrupt = MIN(next_interrupt, گره_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_head = گره_STATE(ksReleaseQueue.head);
    اگر (rlq_head != پوچ) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_head_refill = *refill_head(rlq_head->tcbSchedContext);
        next_interrupt = MIN(rlq_head_refill.rTime, next_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    أكد(next_interrupt >= گره_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(next_interrupt - getTimerPrecision());
}

تهی chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    اگر (likely(گره_STATE(ksCurSC) != گره_STATE(ksIdleSC))) {
        اگر (isRoundRobin(گره_STATE(ksCurSC))) {
            أكد(refill_size(گره_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t head = *refill_head(گره_STATE(ksCurSC));
            refill_t tail = *refill_tail(گره_STATE(ksCurSC));
            refill_head(گره_STATE(ksCurSC))->rAmount = head.rAmount + tail.rAmount;
            refill_tail(گره_STATE(ksCurSC))->rAmount = 0;
        } وگرنه {
            refill_budget_check(consumed);
        }

        أكد(refill_head(گره_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        گره_STATE(ksCurSC)->scConsumed += consumed;
    }
    گره_STATE(ksConsumed) = 0;
    اگر (likely(isSchedulable(گره_STATE(ksCurThread)))) {
        أكد(گره_STATE(ksCurThread)->tcbSchedContext == گره_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        گره_STATE(ksReprogram) = true;
    }
}

تهی endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(گره_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(گره_STATE(ksCurThread));

    اگر (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(گره_STATE(ksCurSC)->scBadge);
        handleTimeout(گره_STATE(ksCurThread));
    } وگرنه اگر (refill_ready(گره_STATE(ksCurSC)) && refill_sufficient(گره_STATE(ksCurSC), 0)) {
        /* apply round robin */
        أكد(!thread_state_get_tcbQueued(گره_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } وگرنه {
        /* postpone until ready */
        postpone(گره_STATE(ksCurSC));
    }
}
#وگرنه

تهی timerTick(تهی)
{
    اگر (likely(thread_state_get_tsType(گره_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ifdef CONFIG_VTX
        || thread_state_get_tsType(گره_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#endif
       ) {
        اگر (گره_STATE(ksCurThread)->tcbTimeSlice > 1) {
            گره_STATE(ksCurThread)->tcbTimeSlice--;
        } وگرنه {
            گره_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    اگر (رقم_Domains > 1) {
        ksDomainTime--;
        اگر (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#endif

تهی rescheduleRequired(تهی)
{
    اگر (گره_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && گره_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#ifdef CONFIG_KERNEL_MCS
        && isSchedulable(گره_STATE(ksSchedulerAction))
#endif
       ) {
#ifdef CONFIG_KERNEL_MCS
        أكد(refill_sufficient(گره_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        أكد(refill_ready(گره_STATE(ksSchedulerAction)->tcbSchedContext));
#endif
        SCHED_ENQUEUE(گره_STATE(ksSchedulerAction));
    }
    گره_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#ifdef CONFIG_KERNEL_MCS

ایستا درونخطی bool_t PURE release_q_non_empty_and_ready(تهی)
{
    بازگشت گره_STATE(ksReleaseQueue.head) != پوچ
           && refill_ready(گره_STATE(ksReleaseQueue.head)->tcbSchedContext);
}

ایستا تهی tcbReleaseDequeue(تهی)
{
    أكد(گره_STATE(ksReleaseQueue.head) != پوچ);
    أكد(گره_STATE(ksReleaseQueue.head)->tcbSchedPrev == پوچ);
    SMP_COND_STATEMENT(أكد(گره_STATE(ksReleaseQueue.head)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = گره_STATE(ksReleaseQueue.head);
    أكد(awakened != گره_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    أكد(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(أكد(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    أكد(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

تهی awaken(تهی)
{
    وقتی (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#endif
