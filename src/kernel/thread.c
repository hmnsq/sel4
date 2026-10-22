/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <object.h>
#অন্তর্ভাৱ <util.h>
#অন্তর্ভাৱ <api/faults.h>
#অন্তর্ভাৱ <api/types.h>
#অন্তর্ভাৱ <kernel/cspace.h>
#অন্তর্ভাৱ <kernel/thread.h>
#অন্তর্ভাৱ <kernel/vspace.h>
#অন্তর্ভাৱ <object/domain.h>
#ifdef CONFIG_KERNEL_MCS
#অন্তর্ভাৱ <object/schedcontext.h>
#endif
#অন্তর্ভাৱ <model/statedata.h>
#অন্তর্ভাৱ <arch/machine.h>
#অন্তর্ভাৱ <arch/kernel/thread.h>
#অন্তর্ভাৱ <machine/registerset.h>
#অন্তর্ভাৱ <machine/fpu.h>
#অন্তর্ভাৱ <linker.h>

স্থির seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receiveBuffer);

BOOT_CODE শূন্য configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

শূন্য activateThread(শূন্য)
{
#ifdef CONFIG_KERNEL_MCS
    যদি (unlikely(গ্ৰন্থি_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(গ্ৰন্থি_STATE(ksCurThread));
        প্রতিজ্ঞা(thread_state_get_tsType(গ্ৰন্থি_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#endif

    ৱিকল্প (thread_state_get_tsType(গ্ৰন্থি_STATE(ksCurThread)->tcbState)) {
    পক্ষ ThreadState_Running:
#ifdef CONFIG_VTX
    পক্ষ ThreadState_RunningVM:
#endif
        ভঙ্গ;

    পক্ষ ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(গ্ৰন্থি_STATE(ksCurThread));
        setNextPC(গ্ৰন্থি_STATE(ksCurThread), pc);
        setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Running);
        ভঙ্গ;
    }

    পক্ষ ThreadState_IdleThreadState:
        Arch_activateIdleThread(গ্ৰন্থি_STATE(ksCurThread));
        ভঙ্গ;

    স্ৱভাৱ:
        fail("Current thread is blocked");
    }
}

শূন্য suspend(tcb_t *target)
{
    cancelIPC(target);
    যদি (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
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

শূন্য restart(tcb_t *target)
{
    যদি (isStopped(target)) {
        cancelIPC(target);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        যদি (sc_sporadic(sc) && sc != গ্ৰন্থি_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        যদি (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#অন্যথা
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#endif
    }
}

শূন্য doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    শূন্য *receiveBuffer, *sendBuffer;

    receiveBuffer = lookupIPCBuffer(true, receiver);

    যদি (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        sendBuffer = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, sendBuffer, endpoint, badge, grant,
                         receiver, receiveBuffer);
    } অন্যথা {
        doFaultTransfer(badge, sender, receiver, receiveBuffer);
    }
}

#ifdef CONFIG_KERNEL_MCS
শূন্য doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#অন্যথা
শূন্য doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#endif
{
#ifdef CONFIG_KERNEL_MCS
    যদি (reply->replyTCB == রিক্ত ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        ঘূৰাই;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    প্রতিজ্ঞা(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    প্রতিজ্ঞা(reply->replyTCB == রিক্ত);

    যদি (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != গ্ৰন্থি_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#অন্যথা
    প্রতিজ্ঞা(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#endif

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    যদি (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, রিক্ত, 0, grant, receiver);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#অন্যথা
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#endif
    } অন্যথা {
#ifndef CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#endif
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        যদি (restart) {
            setThreadState(receiver, ThreadState_Restart);
#ifndef CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#endif
        } অন্যথা {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    যদি (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        যদি ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } অন্যথা {
            যদি (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } অন্যথা {
                postpone(sc);
            }
        }
    }
#endif
}

শূন্য doNormalTransfer(tcb_t *sender, word_t *sendBuffer, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receiveBuffer)
{
    word_t বাৰ্তা_Transferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, বাৰ্তা_Info_Register));

    যদি (canGrant) {
        status = lookupExtraCaps(sender, sendBuffer, tag);
        যদি (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = রিক্ত;
        }
    } অন্যথা {
        current_extra_caps.excaprefs[0] = রিক্ত;
    }

    বাৰ্তা_Transferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
                             seL4_MessageInfo_get_length(tag));

    tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

    tag = seL4_MessageInfo_set_length(tag, বাৰ্তা_Transferred);
    setRegister(receiver, বাৰ্তা_Info_Register, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

শূন্য doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t sent;
    seL4_MessageInfo_t বাৰ্তা_Info;

    sent = setMRs_fault(sender, receiver, receiverIPCBuffer);
    বাৰ্তা_Info = seL4_MessageInfo_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, sent);
    setRegister(receiver, বাৰ্তা_Info_Register, wordFromMessageInfo(বাৰ্তা_Info));
    setRegister(receiver, badgeRegister, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
স্থির seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receiveBuffer)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    যদি (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
        ঘূৰাই info;
    }

    destSlot = getReceiveSlots(receiver, receiveBuffer);

    বাবে (i = 0; i < se_L4_বাৰ্তা_Max_Extra_Caps && current_extra_caps.excaprefs[i] != রিক্ত; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        যদি (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receiveBuffer,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } অন্যথা {
            deriveCap_ret_t dc_ret;

            যদি (!destSlot) {
                ভঙ্গ;
            }

            dc_ret = deriveCap(slot, cap);

            যদি (dc_ret.status != EXCEPTION_NONE) {
                ভঙ্গ;
            }
            যদি (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                ভঙ্গ;
            }

            cte_ঢোকাও(dc_ret.cap, slot, destSlot);

            destSlot = রিক্ত;
        }
    }

    ঘূৰাই seL4_MessageInfo_set_extraCaps(info, i);
}

শূন্য doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

শূন্য prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    যদি (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#ifdef CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#endif
    }
}

স্থির শূন্য prepareNextDomain(শূন্য)
{
    Arch_prepareNextDomain();
#ifdef CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(রিক্ত);
#endif
}

স্থির শূন্য nextDomain(শূন্য)
{
    ksDomScheduleIdx++;
    যদি (dschedule_is_end_marker(ksDomScheduleIdx)) {
        ksDomScheduleIdx = ksDomScheduleStart;
    }
#ifdef CONFIG_KERNEL_MCS
    গ্ৰন্থি_STATE(ksReprogram) = true;
#endif
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ksDomSchedule[ksDomScheduleIdx]);
    ksDomainTime = dschedule_duration(ksDomSchedule[ksDomScheduleIdx]);
}

#ifdef CONFIG_KERNEL_MCS
স্থির শূন্য switchSchedContext(শূন্য)
{
    যদি (unlikely(গ্ৰন্থি_STATE(ksCurSC) != গ্ৰন্থি_STATE(ksCurThread)->tcbSchedContext)) {
        গ্ৰন্থি_STATE(ksReprogram) = true;
        যদি (sc_constant_bandwidth(গ্ৰন্থি_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(গ্ৰন্থি_STATE(ksCurThread)->tcbSchedContext);
        }

        প্রতিজ্ঞা(refill_ready(গ্ৰন্থি_STATE(ksCurThread)->tcbSchedContext));
        প্রতিজ্ঞা(refill_sufficient(গ্ৰন্থি_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    যদি (গ্ৰন্থি_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    গ্ৰন্থি_STATE(ksCurSC) = গ্ৰন্থি_STATE(ksCurThread)->tcbSchedContext;
}
#endif

স্থির শূন্য scheduleChooseNewThread(শূন্য)
{
    যদি (ksDomainTime == 0) {
        prepareNextDomain();
        nextDomain();
    }
    chooseThread();
}

শূন্য schedule(শূন্য)
{
#ifdef CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#endif

    যদি (গ্ৰন্থি_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        যদি (isSchedulable(গ্ৰন্থি_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } অন্যথা {
            was_runnable = false;
        }

        যদি (গ্ৰন্থি_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } অন্যথা {
            tcb_t *candidate = গ্ৰন্থি_STATE(ksSchedulerAction);
            প্রতিজ্ঞা(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                গ্ৰন্থি_STATE(ksCurThread) == গ্ৰন্থি_STATE(ksIdleThread)
                || (candidate->tcbPriority < গ্ৰন্থি_STATE(ksCurThread)->tcbPriority);
            যদি (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                গ্ৰন্থি_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } অন্যথা যদি (was_runnable && candidate->tcbPriority == গ্ৰন্থি_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                গ্ৰন্থি_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } অন্যথা {
                প্রতিজ্ঞা(candidate != গ্ৰন্থি_STATE(ksCurThread));
                switchToThread(candidate);
            }
        }
    }
    গ্ৰন্থি_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ifdef ENABLE_SMP_SUPPORT
    doMaskReschedule(ARCH_গ্ৰন্থি_STATE(ipiReschedulePending));
    ARCH_গ্ৰন্থি_STATE(ipiReschedulePending) = 0;
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_KERNEL_MCS
    switchSchedContext();

    যদি (গ্ৰন্থি_STATE(ksReprogram)) {
        setNextInterrupt();
        গ্ৰন্থি_STATE(ksReprogram) = false;
    }
#endif
}

শূন্য chooseThread(শূন্য)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    যদি (অঙ্ক_Domains > 1) {
        dom = ksCurDomain;
    } অন্যথা {
        dom = 0;
    }

    যদি (likely(গ্ৰন্থি_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = গ্ৰন্থি_STATE(ksReadyQueues)[ready_queues_index(dom, prio)].head;
        প্রতিজ্ঞা(thread);
        প্রতিজ্ঞা(isSchedulable(thread));
#ifdef CONFIG_KERNEL_MCS
        প্রতিজ্ঞা(refill_sufficient(thread->tcbSchedContext, 0));
        প্রতিজ্ঞা(refill_ready(thread->tcbSchedContext));
#endif
        switchToThread(thread);
    } অন্যথা {
        switchToIdleThread();
    }
}

শূন্য switchToThread(tcb_t *thread)
{
#ifdef CONFIG_KERNEL_MCS
    প্রতিজ্ঞা(thread->tcbSchedContext != রিক্ত);
    প্রতিজ্ঞা(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    প্রতিজ্ঞা(refill_sufficient(thread->tcbSchedContext, 0));
    প্রতিজ্ঞা(refill_ready(thread->tcbSchedContext));
#endif

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(গ্ৰন্থি_STATE(ksCurThread), thread);
#endif
    Arch_switchToThread(thread);

#ifdef CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#endif /* CONFIG_HAVE_FPU */

    tcbSchedDequeue(thread);
    গ্ৰন্থি_STATE(ksCurThread) = thread;
}

শূন্য switchToIdleThread(শূন্য)
{
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(গ্ৰন্থি_STATE(ksCurThread), গ্ৰন্থি_STATE(ksIdleThread));
#endif
    Arch_switchToIdleThread();
    গ্ৰন্থি_STATE(ksCurThread) = গ্ৰন্থি_STATE(ksIdleThread);
}

শূন্য setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    যদি (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    যদি (tptr == গ্ৰন্থি_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

শূন্য setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ifdef CONFIG_KERNEL_MCS
শূন্য setPriority(tcb_t *tptr, prio_t prio)
{
    ৱিকল্প (thread_state_get_tsType(tptr->tcbState)) {
    পক্ষ ThreadState_Running:
    পক্ষ ThreadState_Restart:
        যদি (thread_state_get_tcbQueued(tptr->tcbState) || tptr == গ্ৰন্থি_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } অন্যথা {
            tptr->tcbPriority = prio;
        }
        ভঙ্গ;
    পক্ষ ThreadState_BlockedOnReceive:
    পক্ষ ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ভঙ্গ;
    পক্ষ ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ভঙ্গ;
    স্ৱভাৱ:
        tptr->tcbPriority = prio;
        ভঙ্গ;
    }
}
#অন্যথা
শূন্য setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    যদি (isRunnable(tptr)) {
        যদি (tptr == গ্ৰন্থি_STATE(ksCurThread)) {
            rescheduleRequired();
        } অন্যথা {
            possibleSwitchTo(tptr);
        }
    }
}
#endif

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
শূন্য possibleSwitchTo(tcb_t *target)
{
#ifdef CONFIG_KERNEL_MCS
    যদি (target->tcbSchedContext != রিক্ত && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#endif
        যদি (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } অন্যথা যদি (গ্ৰন্থি_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } অন্যথা {
            গ্ৰন্থি_STATE(ksSchedulerAction) = target;
        }
#ifdef CONFIG_KERNEL_MCS
    }
#endif

}

শূন্য setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

শূন্য scheduleTCB(tcb_t *tptr)
{
    যদি (tptr == গ্ৰন্থি_STATE(ksCurThread) &&
        গ্ৰন্থি_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        গ্ৰন্থি_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#ifdef CONFIG_KERNEL_MCS
শূন্য postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    প্রতিজ্ঞা(tcb != রিক্ত);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    গ্ৰন্থি_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

শূন্য setNextInterrupt(শূন্য)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_head_refill = *refill_head(গ্ৰন্থি_STATE(ksCurThread)->tcbSchedContext);
    ticks_t next_interrupt = গ্ৰন্থি_STATE(ksCurTime) + ct_head_refill.rAmount;

    যদি (অঙ্ক_Domains > 1) {
        next_interrupt = MIN(next_interrupt, গ্ৰন্থি_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_head = গ্ৰন্থি_STATE(ksReleaseQueue.head);
    যদি (rlq_head != রিক্ত) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_head_refill = *refill_head(rlq_head->tcbSchedContext);
        next_interrupt = MIN(rlq_head_refill.rTime, next_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    প্রতিজ্ঞা(next_interrupt >= গ্ৰন্থি_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(next_interrupt - getTimerPrecision());
}

শূন্য chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    যদি (likely(গ্ৰন্থি_STATE(ksCurSC) != গ্ৰন্থি_STATE(ksIdleSC))) {
        যদি (isRoundRobin(গ্ৰন্থি_STATE(ksCurSC))) {
            প্রতিজ্ঞা(refill_size(গ্ৰন্থি_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t head = *refill_head(গ্ৰন্থি_STATE(ksCurSC));
            refill_t tail = *refill_tail(গ্ৰন্থি_STATE(ksCurSC));
            refill_head(গ্ৰন্থি_STATE(ksCurSC))->rAmount = head.rAmount + tail.rAmount;
            refill_tail(গ্ৰন্থি_STATE(ksCurSC))->rAmount = 0;
        } অন্যথা {
            refill_budget_check(consumed);
        }

        প্রতিজ্ঞা(refill_head(গ্ৰন্থি_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        গ্ৰন্থি_STATE(ksCurSC)->scConsumed += consumed;
    }
    গ্ৰন্থি_STATE(ksConsumed) = 0;
    যদি (likely(isSchedulable(গ্ৰন্থি_STATE(ksCurThread)))) {
        প্রতিজ্ঞা(গ্ৰন্থি_STATE(ksCurThread)->tcbSchedContext == গ্ৰন্থি_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        গ্ৰন্থি_STATE(ksReprogram) = true;
    }
}

শূন্য endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(গ্ৰন্থি_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(গ্ৰন্থি_STATE(ksCurThread));

    যদি (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(গ্ৰন্থি_STATE(ksCurSC)->scBadge);
        handleTimeout(গ্ৰন্থি_STATE(ksCurThread));
    } অন্যথা যদি (refill_ready(গ্ৰন্থি_STATE(ksCurSC)) && refill_sufficient(গ্ৰন্থি_STATE(ksCurSC), 0)) {
        /* apply round robin */
        প্রতিজ্ঞা(!thread_state_get_tcbQueued(গ্ৰন্থি_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } অন্যথা {
        /* postpone until ready */
        postpone(গ্ৰন্থি_STATE(ksCurSC));
    }
}
#অন্যথা

শূন্য timerTick(শূন্য)
{
    যদি (likely(thread_state_get_tsType(গ্ৰন্থি_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ifdef CONFIG_VTX
        || thread_state_get_tsType(গ্ৰন্থি_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#endif
       ) {
        যদি (গ্ৰন্থি_STATE(ksCurThread)->tcbTimeSlice > 1) {
            গ্ৰন্থি_STATE(ksCurThread)->tcbTimeSlice--;
        } অন্যথা {
            গ্ৰন্থি_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    যদি (অঙ্ক_Domains > 1) {
        ksDomainTime--;
        যদি (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#endif

শূন্য rescheduleRequired(শূন্য)
{
    যদি (গ্ৰন্থি_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && গ্ৰন্থি_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#ifdef CONFIG_KERNEL_MCS
        && isSchedulable(গ্ৰন্থি_STATE(ksSchedulerAction))
#endif
       ) {
#ifdef CONFIG_KERNEL_MCS
        প্রতিজ্ঞা(refill_sufficient(গ্ৰন্থি_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        প্রতিজ্ঞা(refill_ready(গ্ৰন্থি_STATE(ksSchedulerAction)->tcbSchedContext));
#endif
        SCHED_ENQUEUE(গ্ৰন্থি_STATE(ksSchedulerAction));
    }
    গ্ৰন্থি_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#ifdef CONFIG_KERNEL_MCS

স্থির অন্তঃস্থ bool_t PURE release_q_non_empty_and_ready(শূন্য)
{
    ঘূৰাই গ্ৰন্থি_STATE(ksReleaseQueue.head) != রিক্ত
           && refill_ready(গ্ৰন্থি_STATE(ksReleaseQueue.head)->tcbSchedContext);
}

স্থির শূন্য tcbReleaseDequeue(শূন্য)
{
    প্রতিজ্ঞা(গ্ৰন্থি_STATE(ksReleaseQueue.head) != রিক্ত);
    প্রতিজ্ঞা(গ্ৰন্থি_STATE(ksReleaseQueue.head)->tcbSchedPrev == রিক্ত);
    SMP_COND_STATEMENT(প্রতিজ্ঞা(গ্ৰন্থি_STATE(ksReleaseQueue.head)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = গ্ৰন্থি_STATE(ksReleaseQueue.head);
    প্রতিজ্ঞা(awakened != গ্ৰন্থি_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    প্রতিজ্ঞা(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(প্রতিজ্ঞা(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    প্রতিজ্ঞা(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

শূন্য awaken(শূন্য)
{
    যেতিয়ালৈ (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#endif
