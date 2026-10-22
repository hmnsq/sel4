/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <object.h>
#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <api/faults.h>
#ଅନ୍ତର୍ଭାଵ <api/types.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <object/domain.h>
#ifdef CONFIG_KERNEL_MCS
#ଅନ୍ତର୍ଭାଵ <object/schedcontext.h>
#endif
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>

ସ୍ଥିର seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receiveBuffer);

BOOT_CODE ଶୂନ୍ୟ configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

ଶୂନ୍ୟ activateThread(ଶୂନ୍ୟ)
{
#ifdef CONFIG_KERNEL_MCS
    ଯଦି (unlikely(ଗଣ୍ଠି_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(ଗଣ୍ଠି_STATE(ksCurThread));
        ପ୍ରତିଜ୍ଞା(thread_state_get_tsType(ଗଣ୍ଠି_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#endif

    ବାଛ (thread_state_get_tsType(ଗଣ୍ଠି_STATE(ksCurThread)->tcbState)) {
    ସ୍ଥିତି ThreadState_Running:
#ifdef CONFIG_VTX
    ସ୍ଥିତି ThreadState_RunningVM:
#endif
        ଭାଙ୍ଗ;

    ସ୍ଥିତି ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(ଗଣ୍ଠି_STATE(ksCurThread));
        setNextPC(ଗଣ୍ଠି_STATE(ksCurThread), pc);
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Running);
        ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି ThreadState_IdleThreadState:
        Arch_activateIdleThread(ଗଣ୍ଠି_STATE(ksCurThread));
        ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Current thread is blocked");
    }
}

ଶୂନ୍ୟ suspend(tcb_t *target)
{
    cancelIPC(target);
    ଯଦି (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
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

ଶୂନ୍ୟ restart(tcb_t *target)
{
    ଯଦି (isStopped(target)) {
        cancelIPC(target);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        ଯଦି (sc_sporadic(sc) && sc != ଗଣ୍ଠି_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        ଯଦି (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#ଅନ୍ୟଥା
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#endif
    }
}

ଶୂନ୍ୟ doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    ଶୂନ୍ୟ *receiveBuffer, *sendBuffer;

    receiveBuffer = lookupIPCBuffer(true, receiver);

    ଯଦି (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        sendBuffer = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, sendBuffer, endpoint, badge, grant,
                         receiver, receiveBuffer);
    } ଅନ୍ୟଥା {
        doFaultTransfer(badge, sender, receiver, receiveBuffer);
    }
}

#ifdef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#ଅନ୍ୟଥା
ଶୂନ୍ୟ doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#endif
{
#ifdef CONFIG_KERNEL_MCS
    ଯଦି (reply->replyTCB == ଖାଲି ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        ଫେରନ୍ତୁ;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    ପ୍ରତିଜ୍ଞା(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    ପ୍ରତିଜ୍ଞା(reply->replyTCB == ଖାଲି);

    ଯଦି (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != ଗଣ୍ଠି_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#ଅନ୍ୟଥା
    ପ୍ରତିଜ୍ଞା(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#endif

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    ଯଦି (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, ଖାଲି, 0, grant, receiver);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#ଅନ୍ୟଥା
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#endif
    } ଅନ୍ୟଥା {
#ifndef CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#endif
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        ଯଦି (restart) {
            setThreadState(receiver, ThreadState_Restart);
#ifndef CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#endif
        } ଅନ୍ୟଥା {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    ଯଦି (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        ଯଦି ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } ଅନ୍ୟଥା {
            ଯଦି (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } ଅନ୍ୟଥା {
                postpone(sc);
            }
        }
    }
#endif
}

ଶୂନ୍ୟ doNormalTransfer(tcb_t *sender, word_t *sendBuffer, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receiveBuffer)
{
    word_t ସନ୍ଦେଶ_Transferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, ସନ୍ଦେଶ_Info_Register));

    ଯଦି (canGrant) {
        status = lookupExtraCaps(sender, sendBuffer, tag);
        ଯଦି (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = ଖାଲି;
        }
    } ଅନ୍ୟଥା {
        current_extra_caps.excaprefs[0] = ଖାଲି;
    }

    ସନ୍ଦେଶ_Transferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
                             seL4_MessageInfo_get_length(tag));

    tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

    tag = seL4_MessageInfo_set_length(tag, ସନ୍ଦେଶ_Transferred);
    setRegister(receiver, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

ଶୂନ୍ୟ doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t sent;
    seL4_MessageInfo_t ସନ୍ଦେଶ_Info;

    sent = setMRs_fault(sender, receiver, receiverIPCBuffer);
    ସନ୍ଦେଶ_Info = seL4_MessageInfo_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, sent);
    setRegister(receiver, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(ସନ୍ଦେଶ_Info));
    setRegister(receiver, badgeRegister, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ସ୍ଥିର seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receiveBuffer)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    ଯଦି (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
        ଫେରନ୍ତୁ info;
    }

    destSlot = getReceiveSlots(receiver, receiveBuffer);

    ପାଇଁ (i = 0; i < se_L4_ସନ୍ଦେଶ_Max_Extra_Caps && current_extra_caps.excaprefs[i] != ଖାଲି; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        ଯଦି (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receiveBuffer,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } ଅନ୍ୟଥା {
            deriveCap_ret_t dc_ret;

            ଯଦି (!destSlot) {
                ଭାଙ୍ଗ;
            }

            dc_ret = deriveCap(slot, cap);

            ଯଦି (dc_ret.status != EXCEPTION_NONE) {
                ଭାଙ୍ଗ;
            }
            ଯଦି (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                ଭାଙ୍ଗ;
            }

            cte_ଭର୍ତ୍ତି(dc_ret.cap, slot, destSlot);

            destSlot = ଖାଲି;
        }
    }

    ଫେରନ୍ତୁ seL4_MessageInfo_set_extraCaps(info, i);
}

ଶୂନ୍ୟ doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

ଶୂନ୍ୟ prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    ଯଦି (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#ifdef CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#endif
    }
}

ସ୍ଥିର ଶୂନ୍ୟ prepareNextDomain(ଶୂନ୍ୟ)
{
    Arch_prepareNextDomain();
#ifdef CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(ଖାଲି);
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ nextDomain(ଶୂନ୍ୟ)
{
    ksDomScheduleIdx++;
    ଯଦି (dschedule_is_end_marker(ksDomScheduleIdx)) {
        ksDomScheduleIdx = ksDomScheduleStart;
    }
#ifdef CONFIG_KERNEL_MCS
    ଗଣ୍ଠି_STATE(ksReprogram) = true;
#endif
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ksDomSchedule[ksDomScheduleIdx]);
    ksDomainTime = dschedule_duration(ksDomSchedule[ksDomScheduleIdx]);
}

#ifdef CONFIG_KERNEL_MCS
ସ୍ଥିର ଶୂନ୍ୟ switchSchedContext(ଶୂନ୍ୟ)
{
    ଯଦି (unlikely(ଗଣ୍ଠି_STATE(ksCurSC) != ଗଣ୍ଠି_STATE(ksCurThread)->tcbSchedContext)) {
        ଗଣ୍ଠି_STATE(ksReprogram) = true;
        ଯଦି (sc_constant_bandwidth(ଗଣ୍ଠି_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(ଗଣ୍ଠି_STATE(ksCurThread)->tcbSchedContext);
        }

        ପ୍ରତିଜ୍ଞା(refill_ready(ଗଣ୍ଠି_STATE(ksCurThread)->tcbSchedContext));
        ପ୍ରତିଜ୍ଞା(refill_sufficient(ଗଣ୍ଠି_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    ଯଦି (ଗଣ୍ଠି_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    ଗଣ୍ଠି_STATE(ksCurSC) = ଗଣ୍ଠି_STATE(ksCurThread)->tcbSchedContext;
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ scheduleChooseNewThread(ଶୂନ୍ୟ)
{
    ଯଦି (ksDomainTime == 0) {
        prepareNextDomain();
        nextDomain();
    }
    chooseThread();
}

ଶୂନ୍ୟ schedule(ଶୂନ୍ୟ)
{
#ifdef CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#endif

    ଯଦି (ଗଣ୍ଠି_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        ଯଦି (isSchedulable(ଗଣ୍ଠି_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } ଅନ୍ୟଥା {
            was_runnable = false;
        }

        ଯଦି (ଗଣ୍ଠି_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } ଅନ୍ୟଥା {
            tcb_t *candidate = ଗଣ୍ଠି_STATE(ksSchedulerAction);
            ପ୍ରତିଜ୍ଞା(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                ଗଣ୍ଠି_STATE(ksCurThread) == ଗଣ୍ଠି_STATE(ksIdleThread)
                || (candidate->tcbPriority < ଗଣ୍ଠି_STATE(ksCurThread)->tcbPriority);
            ଯଦି (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                ଗଣ୍ଠି_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } ଅନ୍ୟଥା ଯଦି (was_runnable && candidate->tcbPriority == ଗଣ୍ଠି_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                ଗଣ୍ଠି_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } ଅନ୍ୟଥା {
                ପ୍ରତିଜ୍ଞା(candidate != ଗଣ୍ଠି_STATE(ksCurThread));
                switchToThread(candidate);
            }
        }
    }
    ଗଣ୍ଠି_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ifdef ENABLE_SMP_SUPPORT
    doMaskReschedule(ARCH_ଗଣ୍ଠି_STATE(ipiReschedulePending));
    ARCH_ଗଣ୍ଠି_STATE(ipiReschedulePending) = 0;
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_KERNEL_MCS
    switchSchedContext();

    ଯଦି (ଗଣ୍ଠି_STATE(ksReprogram)) {
        setNextInterrupt();
        ଗଣ୍ଠି_STATE(ksReprogram) = false;
    }
#endif
}

ଶୂନ୍ୟ chooseThread(ଶୂନ୍ୟ)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    ଯଦି (ଅଙ୍କ_Domains > 1) {
        dom = ksCurDomain;
    } ଅନ୍ୟଥା {
        dom = 0;
    }

    ଯଦି (likely(ଗଣ୍ଠି_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = ଗଣ୍ଠି_STATE(ksReadyQueues)[ready_queues_index(dom, prio)].head;
        ପ୍ରତିଜ୍ଞା(thread);
        ପ୍ରତିଜ୍ଞା(isSchedulable(thread));
#ifdef CONFIG_KERNEL_MCS
        ପ୍ରତିଜ୍ଞା(refill_sufficient(thread->tcbSchedContext, 0));
        ପ୍ରତିଜ୍ଞା(refill_ready(thread->tcbSchedContext));
#endif
        switchToThread(thread);
    } ଅନ୍ୟଥା {
        switchToIdleThread();
    }
}

ଶୂନ୍ୟ switchToThread(tcb_t *thread)
{
#ifdef CONFIG_KERNEL_MCS
    ପ୍ରତିଜ୍ଞା(thread->tcbSchedContext != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    ପ୍ରତିଜ୍ଞା(refill_sufficient(thread->tcbSchedContext, 0));
    ପ୍ରତିଜ୍ଞା(refill_ready(thread->tcbSchedContext));
#endif

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ଗଣ୍ଠି_STATE(ksCurThread), thread);
#endif
    Arch_switchToThread(thread);

#ifdef CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#endif /* CONFIG_HAVE_FPU */

    tcbSchedDequeue(thread);
    ଗଣ୍ଠି_STATE(ksCurThread) = thread;
}

ଶୂନ୍ୟ switchToIdleThread(ଶୂନ୍ୟ)
{
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ଗଣ୍ଠି_STATE(ksCurThread), ଗଣ୍ଠି_STATE(ksIdleThread));
#endif
    Arch_switchToIdleThread();
    ଗଣ୍ଠି_STATE(ksCurThread) = ଗଣ୍ଠି_STATE(ksIdleThread);
}

ଶୂନ୍ୟ setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    ଯଦି (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    ଯଦି (tptr == ଗଣ୍ଠି_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

ଶୂନ୍ୟ setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ifdef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ setPriority(tcb_t *tptr, prio_t prio)
{
    ବାଛ (thread_state_get_tsType(tptr->tcbState)) {
    ସ୍ଥିତି ThreadState_Running:
    ସ୍ଥିତି ThreadState_Restart:
        ଯଦି (thread_state_get_tcbQueued(tptr->tcbState) || tptr == ଗଣ୍ଠି_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } ଅନ୍ୟଥା {
            tptr->tcbPriority = prio;
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ThreadState_BlockedOnReceive:
    ସ୍ଥିତି ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        tptr->tcbPriority = prio;
        ଭାଙ୍ଗ;
    }
}
#ଅନ୍ୟଥା
ଶୂନ୍ୟ setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    ଯଦି (isRunnable(tptr)) {
        ଯଦି (tptr == ଗଣ୍ଠି_STATE(ksCurThread)) {
            rescheduleRequired();
        } ଅନ୍ୟଥା {
            possibleSwitchTo(tptr);
        }
    }
}
#endif

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
ଶୂନ୍ୟ possibleSwitchTo(tcb_t *target)
{
#ifdef CONFIG_KERNEL_MCS
    ଯଦି (target->tcbSchedContext != ଖାଲି && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#endif
        ଯଦି (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } ଅନ୍ୟଥା ଯଦି (ଗଣ୍ଠି_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } ଅନ୍ୟଥା {
            ଗଣ୍ଠି_STATE(ksSchedulerAction) = target;
        }
#ifdef CONFIG_KERNEL_MCS
    }
#endif

}

ଶୂନ୍ୟ setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

ଶୂନ୍ୟ scheduleTCB(tcb_t *tptr)
{
    ଯଦି (tptr == ଗଣ୍ଠି_STATE(ksCurThread) &&
        ଗଣ୍ଠି_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        ଗଣ୍ଠି_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#ifdef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ପ୍ରତିଜ୍ଞା(tcb != ଖାଲି);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    ଗଣ୍ଠି_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

ଶୂନ୍ୟ setNextInterrupt(ଶୂନ୍ୟ)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_head_refill = *refill_head(ଗଣ୍ଠି_STATE(ksCurThread)->tcbSchedContext);
    ticks_t next_interrupt = ଗଣ୍ଠି_STATE(ksCurTime) + ct_head_refill.rAmount;

    ଯଦି (ଅଙ୍କ_Domains > 1) {
        next_interrupt = MIN(next_interrupt, ଗଣ୍ଠି_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_head = ଗଣ୍ଠି_STATE(ksReleaseQueue.head);
    ଯଦି (rlq_head != ଖାଲି) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_head_refill = *refill_head(rlq_head->tcbSchedContext);
        next_interrupt = MIN(rlq_head_refill.rTime, next_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    ପ୍ରତିଜ୍ଞା(next_interrupt >= ଗଣ୍ଠି_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(next_interrupt - getTimerPrecision());
}

ଶୂନ୍ୟ chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    ଯଦି (likely(ଗଣ୍ଠି_STATE(ksCurSC) != ଗଣ୍ଠି_STATE(ksIdleSC))) {
        ଯଦି (isRoundRobin(ଗଣ୍ଠି_STATE(ksCurSC))) {
            ପ୍ରତିଜ୍ଞା(refill_size(ଗଣ୍ଠି_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t head = *refill_head(ଗଣ୍ଠି_STATE(ksCurSC));
            refill_t tail = *refill_tail(ଗଣ୍ଠି_STATE(ksCurSC));
            refill_head(ଗଣ୍ଠି_STATE(ksCurSC))->rAmount = head.rAmount + tail.rAmount;
            refill_tail(ଗଣ୍ଠି_STATE(ksCurSC))->rAmount = 0;
        } ଅନ୍ୟଥା {
            refill_budget_check(consumed);
        }

        ପ୍ରତିଜ୍ଞା(refill_head(ଗଣ୍ଠି_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        ଗଣ୍ଠି_STATE(ksCurSC)->scConsumed += consumed;
    }
    ଗଣ୍ଠି_STATE(ksConsumed) = 0;
    ଯଦି (likely(isSchedulable(ଗଣ୍ଠି_STATE(ksCurThread)))) {
        ପ୍ରତିଜ୍ଞା(ଗଣ୍ଠି_STATE(ksCurThread)->tcbSchedContext == ଗଣ୍ଠି_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        ଗଣ୍ଠି_STATE(ksReprogram) = true;
    }
}

ଶୂନ୍ୟ endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(ଗଣ୍ଠି_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(ଗଣ୍ଠି_STATE(ksCurThread));

    ଯଦି (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(ଗଣ୍ଠି_STATE(ksCurSC)->scBadge);
        handleTimeout(ଗଣ୍ଠି_STATE(ksCurThread));
    } ଅନ୍ୟଥା ଯଦି (refill_ready(ଗଣ୍ଠି_STATE(ksCurSC)) && refill_sufficient(ଗଣ୍ଠି_STATE(ksCurSC), 0)) {
        /* apply round robin */
        ପ୍ରତିଜ୍ଞା(!thread_state_get_tcbQueued(ଗଣ୍ଠି_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } ଅନ୍ୟଥା {
        /* postpone until ready */
        postpone(ଗଣ୍ଠି_STATE(ksCurSC));
    }
}
#ଅନ୍ୟଥା

ଶୂନ୍ୟ timerTick(ଶୂନ୍ୟ)
{
    ଯଦି (likely(thread_state_get_tsType(ଗଣ୍ଠି_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ifdef CONFIG_VTX
        || thread_state_get_tsType(ଗଣ୍ଠି_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#endif
       ) {
        ଯଦି (ଗଣ୍ଠି_STATE(ksCurThread)->tcbTimeSlice > 1) {
            ଗଣ୍ଠି_STATE(ksCurThread)->tcbTimeSlice--;
        } ଅନ୍ୟଥା {
            ଗଣ୍ଠି_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    ଯଦି (ଅଙ୍କ_Domains > 1) {
        ksDomainTime--;
        ଯଦି (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#endif

ଶୂନ୍ୟ rescheduleRequired(ଶୂନ୍ୟ)
{
    ଯଦି (ଗଣ୍ଠି_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && ଗଣ୍ଠି_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#ifdef CONFIG_KERNEL_MCS
        && isSchedulable(ଗଣ୍ଠି_STATE(ksSchedulerAction))
#endif
       ) {
#ifdef CONFIG_KERNEL_MCS
        ପ୍ରତିଜ୍ଞା(refill_sufficient(ଗଣ୍ଠି_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        ପ୍ରତିଜ୍ଞା(refill_ready(ଗଣ୍ଠି_STATE(ksSchedulerAction)->tcbSchedContext));
#endif
        SCHED_ENQUEUE(ଗଣ୍ଠି_STATE(ksSchedulerAction));
    }
    ଗଣ୍ଠି_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#ifdef CONFIG_KERNEL_MCS

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool_t PURE release_q_non_empty_and_ready(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ଗଣ୍ଠି_STATE(ksReleaseQueue.head) != ଖାଲି
           && refill_ready(ଗଣ୍ଠି_STATE(ksReleaseQueue.head)->tcbSchedContext);
}

ସ୍ଥିର ଶୂନ୍ୟ tcbReleaseDequeue(ଶୂନ୍ୟ)
{
    ପ୍ରତିଜ୍ଞା(ଗଣ୍ଠି_STATE(ksReleaseQueue.head) != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(ଗଣ୍ଠି_STATE(ksReleaseQueue.head)->tcbSchedPrev == ଖାଲି);
    SMP_COND_STATEMENT(ପ୍ରତିଜ୍ଞା(ଗଣ୍ଠି_STATE(ksReleaseQueue.head)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = ଗଣ୍ଠି_STATE(ksReleaseQueue.head);
    ପ୍ରତିଜ୍ଞା(awakened != ଗଣ୍ଠି_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    ପ୍ରତିଜ୍ଞା(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(ପ୍ରତିଜ୍ଞା(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    ପ୍ରତିଜ୍ଞା(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

ଶୂନ୍ୟ awaken(ଶୂନ୍ୟ)
{
    ଯେତେବେଳେ (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#endif
