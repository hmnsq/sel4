/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <object.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <api/faults.h>
#അന്തര്ഭാവ <api/types.h>
#അന്തര്ഭാവ <kernel/cspace.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <object/domain.h>
#ifdef CONFIG_KERNEL_MCS
#അന്തര്ഭാവ <object/schedcontext.h>
#endif
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <arch/kernel/thread.h>
#അന്തര്ഭാവ <machine/registerset.h>
#അന്തര്ഭാവ <machine/fpu.h>
#അന്തര്ഭാവ <linker.h>

സ്ഥിതം seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receiveBuffer);

BOOT_CODE ശൂന്യം configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

ശൂന്യം activateThread(ശൂന്യം)
{
#ifdef CONFIG_KERNEL_MCS
    എങ്കിൽ (unlikely(കണ്ണി_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(കണ്ണി_STATE(ksCurThread));
        പ്രതിജ്ഞാ(thread_state_get_tsType(കണ്ണി_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#endif

    തിരഞ്ഞെടുപ്പ് (thread_state_get_tsType(കണ്ണി_STATE(ksCurThread)->tcbState)) {
    അവസ്ഥ ThreadState_Running:
#ifdef CONFIG_VTX
    അവസ്ഥ ThreadState_RunningVM:
#endif
        നിർത്തുക;

    അവസ്ഥ ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(കണ്ണി_STATE(ksCurThread));
        setNextPC(കണ്ണി_STATE(ksCurThread), pc);
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Running);
        നിർത്തുക;
    }

    അവസ്ഥ ThreadState_IdleThreadState:
        Arch_activateIdleThread(കണ്ണി_STATE(ksCurThread));
        നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
        fail("Current thread is blocked");
    }
}

ശൂന്യം suspend(tcb_t *target)
{
    cancelIPC(target);
    എങ്കിൽ (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
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

ശൂന്യം restart(tcb_t *target)
{
    എങ്കിൽ (isStopped(target)) {
        cancelIPC(target);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        എങ്കിൽ (sc_sporadic(sc) && sc != കണ്ണി_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        എങ്കിൽ (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#അല്ലെങ്കിൽ
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#endif
    }
}

ശൂന്യം doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    ശൂന്യം *receiveBuffer, *sendBuffer;

    receiveBuffer = lookupIPCBuffer(true, receiver);

    എങ്കിൽ (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        sendBuffer = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, sendBuffer, endpoint, badge, grant,
                         receiver, receiveBuffer);
    } അല്ലെങ്കിൽ {
        doFaultTransfer(badge, sender, receiver, receiveBuffer);
    }
}

#ifdef CONFIG_KERNEL_MCS
ശൂന്യം doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#അല്ലെങ്കിൽ
ശൂന്യം doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#endif
{
#ifdef CONFIG_KERNEL_MCS
    എങ്കിൽ (reply->replyTCB == ശൂന്യ ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        തിരികെ;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    പ്രതിജ്ഞാ(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    പ്രതിജ്ഞാ(reply->replyTCB == ശൂന്യ);

    എങ്കിൽ (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != കണ്ണി_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#അല്ലെങ്കിൽ
    പ്രതിജ്ഞാ(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#endif

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    എങ്കിൽ (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, ശൂന്യ, 0, grant, receiver);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#അല്ലെങ്കിൽ
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#endif
    } അല്ലെങ്കിൽ {
#ifndef CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#endif
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        എങ്കിൽ (restart) {
            setThreadState(receiver, ThreadState_Restart);
#ifndef CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#endif
        } അല്ലെങ്കിൽ {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    എങ്കിൽ (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        എങ്കിൽ ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } അല്ലെങ്കിൽ {
                postpone(sc);
            }
        }
    }
#endif
}

ശൂന്യം doNormalTransfer(tcb_t *sender, word_t *sendBuffer, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receiveBuffer)
{
    word_t സന്ദേശം_Transferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, സന്ദേശം_Info_Register));

    എങ്കിൽ (canGrant) {
        status = lookupExtraCaps(sender, sendBuffer, tag);
        എങ്കിൽ (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = ശൂന്യ;
        }
    } അല്ലെങ്കിൽ {
        current_extra_caps.excaprefs[0] = ശൂന്യ;
    }

    സന്ദേശം_Transferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
                             seL4_MessageInfo_get_length(tag));

    tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

    tag = seL4_MessageInfo_set_length(tag, സന്ദേശം_Transferred);
    setRegister(receiver, സന്ദേശം_Info_Register, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

ശൂന്യം doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t sent;
    seL4_MessageInfo_t സന്ദേശം_Info;

    sent = setMRs_fault(sender, receiver, receiverIPCBuffer);
    സന്ദേശം_Info = seL4_MessageInfo_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, sent);
    setRegister(receiver, സന്ദേശം_Info_Register, wordFromMessageInfo(സന്ദേശം_Info));
    setRegister(receiver, badgeRegister, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
സ്ഥിതം seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receiveBuffer)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    എങ്കിൽ (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
        തിരികെ info;
    }

    destSlot = getReceiveSlots(receiver, receiveBuffer);

    വേണ്ടി (i = 0; i < se_L4_സന്ദേശം_Max_Extra_Caps && current_extra_caps.excaprefs[i] != ശൂന്യ; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        എങ്കിൽ (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receiveBuffer,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } അല്ലെങ്കിൽ {
            deriveCap_ret_t dc_ret;

            എങ്കിൽ (!destSlot) {
                നിർത്തുക;
            }

            dc_ret = deriveCap(slot, cap);

            എങ്കിൽ (dc_ret.status != EXCEPTION_NONE) {
                നിർത്തുക;
            }
            എങ്കിൽ (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                നിർത്തുക;
            }

            cte_തിരുകുക(dc_ret.cap, slot, destSlot);

            destSlot = ശൂന്യ;
        }
    }

    തിരികെ seL4_MessageInfo_set_extraCaps(info, i);
}

ശൂന്യം doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

ശൂന്യം prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    എങ്കിൽ (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#ifdef CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#endif
    }
}

സ്ഥിതം ശൂന്യം prepareNextDomain(ശൂന്യം)
{
    Arch_prepareNextDomain();
#ifdef CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(ശൂന്യ);
#endif
}

സ്ഥിതം ശൂന്യം nextDomain(ശൂന്യം)
{
    ksDomScheduleIdx++;
    എങ്കിൽ (dschedule_is_end_marker(ksDomScheduleIdx)) {
        ksDomScheduleIdx = ksDomScheduleStart;
    }
#ifdef CONFIG_KERNEL_MCS
    കണ്ണി_STATE(ksReprogram) = true;
#endif
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ksDomSchedule[ksDomScheduleIdx]);
    ksDomainTime = dschedule_duration(ksDomSchedule[ksDomScheduleIdx]);
}

#ifdef CONFIG_KERNEL_MCS
സ്ഥിതം ശൂന്യം switchSchedContext(ശൂന്യം)
{
    എങ്കിൽ (unlikely(കണ്ണി_STATE(ksCurSC) != കണ്ണി_STATE(ksCurThread)->tcbSchedContext)) {
        കണ്ണി_STATE(ksReprogram) = true;
        എങ്കിൽ (sc_constant_bandwidth(കണ്ണി_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(കണ്ണി_STATE(ksCurThread)->tcbSchedContext);
        }

        പ്രതിജ്ഞാ(refill_ready(കണ്ണി_STATE(ksCurThread)->tcbSchedContext));
        പ്രതിജ്ഞാ(refill_sufficient(കണ്ണി_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    എങ്കിൽ (കണ്ണി_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    കണ്ണി_STATE(ksCurSC) = കണ്ണി_STATE(ksCurThread)->tcbSchedContext;
}
#endif

സ്ഥിതം ശൂന്യം scheduleChooseNewThread(ശൂന്യം)
{
    എങ്കിൽ (ksDomainTime == 0) {
        prepareNextDomain();
        nextDomain();
    }
    chooseThread();
}

ശൂന്യം schedule(ശൂന്യം)
{
#ifdef CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#endif

    എങ്കിൽ (കണ്ണി_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        എങ്കിൽ (isSchedulable(കണ്ണി_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } അല്ലെങ്കിൽ {
            was_runnable = false;
        }

        എങ്കിൽ (കണ്ണി_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } അല്ലെങ്കിൽ {
            tcb_t *candidate = കണ്ണി_STATE(ksSchedulerAction);
            പ്രതിജ്ഞാ(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                കണ്ണി_STATE(ksCurThread) == കണ്ണി_STATE(ksIdleThread)
                || (candidate->tcbPriority < കണ്ണി_STATE(ksCurThread)->tcbPriority);
            എങ്കിൽ (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                കണ്ണി_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } അല്ലെങ്കിൽ എങ്കിൽ (was_runnable && candidate->tcbPriority == കണ്ണി_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                കണ്ണി_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } അല്ലെങ്കിൽ {
                പ്രതിജ്ഞാ(candidate != കണ്ണി_STATE(ksCurThread));
                switchToThread(candidate);
            }
        }
    }
    കണ്ണി_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ifdef ENABLE_SMP_SUPPORT
    doMaskReschedule(ARCH_കണ്ണി_STATE(ipiReschedulePending));
    ARCH_കണ്ണി_STATE(ipiReschedulePending) = 0;
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_KERNEL_MCS
    switchSchedContext();

    എങ്കിൽ (കണ്ണി_STATE(ksReprogram)) {
        setNextInterrupt();
        കണ്ണി_STATE(ksReprogram) = false;
    }
#endif
}

ശൂന്യം chooseThread(ശൂന്യം)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    എങ്കിൽ (അക്കം_Domains > 1) {
        dom = ksCurDomain;
    } അല്ലെങ്കിൽ {
        dom = 0;
    }

    എങ്കിൽ (likely(കണ്ണി_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = കണ്ണി_STATE(ksReadyQueues)[ready_queues_index(dom, prio)].head;
        പ്രതിജ്ഞാ(thread);
        പ്രതിജ്ഞാ(isSchedulable(thread));
#ifdef CONFIG_KERNEL_MCS
        പ്രതിജ്ഞാ(refill_sufficient(thread->tcbSchedContext, 0));
        പ്രതിജ്ഞാ(refill_ready(thread->tcbSchedContext));
#endif
        switchToThread(thread);
    } അല്ലെങ്കിൽ {
        switchToIdleThread();
    }
}

ശൂന്യം switchToThread(tcb_t *thread)
{
#ifdef CONFIG_KERNEL_MCS
    പ്രതിജ്ഞാ(thread->tcbSchedContext != ശൂന്യ);
    പ്രതിജ്ഞാ(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    പ്രതിജ്ഞാ(refill_sufficient(thread->tcbSchedContext, 0));
    പ്രതിജ്ഞാ(refill_ready(thread->tcbSchedContext));
#endif

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(കണ്ണി_STATE(ksCurThread), thread);
#endif
    Arch_switchToThread(thread);

#ifdef CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#endif /* CONFIG_HAVE_FPU */

    tcbSchedDequeue(thread);
    കണ്ണി_STATE(ksCurThread) = thread;
}

ശൂന്യം switchToIdleThread(ശൂന്യം)
{
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(കണ്ണി_STATE(ksCurThread), കണ്ണി_STATE(ksIdleThread));
#endif
    Arch_switchToIdleThread();
    കണ്ണി_STATE(ksCurThread) = കണ്ണി_STATE(ksIdleThread);
}

ശൂന്യം setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    എങ്കിൽ (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    എങ്കിൽ (tptr == കണ്ണി_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

ശൂന്യം setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ifdef CONFIG_KERNEL_MCS
ശൂന്യം setPriority(tcb_t *tptr, prio_t prio)
{
    തിരഞ്ഞെടുപ്പ് (thread_state_get_tsType(tptr->tcbState)) {
    അവസ്ഥ ThreadState_Running:
    അവസ്ഥ ThreadState_Restart:
        എങ്കിൽ (thread_state_get_tcbQueued(tptr->tcbState) || tptr == കണ്ണി_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } അല്ലെങ്കിൽ {
            tptr->tcbPriority = prio;
        }
        നിർത്തുക;
    അവസ്ഥ ThreadState_BlockedOnReceive:
    അവസ്ഥ ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        നിർത്തുക;
    അവസ്ഥ ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        tptr->tcbPriority = prio;
        നിർത്തുക;
    }
}
#അല്ലെങ്കിൽ
ശൂന്യം setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    എങ്കിൽ (isRunnable(tptr)) {
        എങ്കിൽ (tptr == കണ്ണി_STATE(ksCurThread)) {
            rescheduleRequired();
        } അല്ലെങ്കിൽ {
            possibleSwitchTo(tptr);
        }
    }
}
#endif

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
ശൂന്യം possibleSwitchTo(tcb_t *target)
{
#ifdef CONFIG_KERNEL_MCS
    എങ്കിൽ (target->tcbSchedContext != ശൂന്യ && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#endif
        എങ്കിൽ (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } അല്ലെങ്കിൽ എങ്കിൽ (കണ്ണി_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } അല്ലെങ്കിൽ {
            കണ്ണി_STATE(ksSchedulerAction) = target;
        }
#ifdef CONFIG_KERNEL_MCS
    }
#endif

}

ശൂന്യം setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

ശൂന്യം scheduleTCB(tcb_t *tptr)
{
    എങ്കിൽ (tptr == കണ്ണി_STATE(ksCurThread) &&
        കണ്ണി_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        കണ്ണി_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#ifdef CONFIG_KERNEL_MCS
ശൂന്യം postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    പ്രതിജ്ഞാ(tcb != ശൂന്യ);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    കണ്ണി_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

ശൂന്യം setNextInterrupt(ശൂന്യം)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_head_refill = *refill_head(കണ്ണി_STATE(ksCurThread)->tcbSchedContext);
    ticks_t next_interrupt = കണ്ണി_STATE(ksCurTime) + ct_head_refill.rAmount;

    എങ്കിൽ (അക്കം_Domains > 1) {
        next_interrupt = MIN(next_interrupt, കണ്ണി_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_head = കണ്ണി_STATE(ksReleaseQueue.head);
    എങ്കിൽ (rlq_head != ശൂന്യ) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_head_refill = *refill_head(rlq_head->tcbSchedContext);
        next_interrupt = MIN(rlq_head_refill.rTime, next_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    പ്രതിജ്ഞാ(next_interrupt >= കണ്ണി_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(next_interrupt - getTimerPrecision());
}

ശൂന്യം chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    എങ്കിൽ (likely(കണ്ണി_STATE(ksCurSC) != കണ്ണി_STATE(ksIdleSC))) {
        എങ്കിൽ (isRoundRobin(കണ്ണി_STATE(ksCurSC))) {
            പ്രതിജ്ഞാ(refill_size(കണ്ണി_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t head = *refill_head(കണ്ണി_STATE(ksCurSC));
            refill_t tail = *refill_tail(കണ്ണി_STATE(ksCurSC));
            refill_head(കണ്ണി_STATE(ksCurSC))->rAmount = head.rAmount + tail.rAmount;
            refill_tail(കണ്ണി_STATE(ksCurSC))->rAmount = 0;
        } അല്ലെങ്കിൽ {
            refill_budget_check(consumed);
        }

        പ്രതിജ്ഞാ(refill_head(കണ്ണി_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        കണ്ണി_STATE(ksCurSC)->scConsumed += consumed;
    }
    കണ്ണി_STATE(ksConsumed) = 0;
    എങ്കിൽ (likely(isSchedulable(കണ്ണി_STATE(ksCurThread)))) {
        പ്രതിജ്ഞാ(കണ്ണി_STATE(ksCurThread)->tcbSchedContext == കണ്ണി_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        കണ്ണി_STATE(ksReprogram) = true;
    }
}

ശൂന്യം endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(കണ്ണി_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(കണ്ണി_STATE(ksCurThread));

    എങ്കിൽ (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(കണ്ണി_STATE(ksCurSC)->scBadge);
        handleTimeout(കണ്ണി_STATE(ksCurThread));
    } അല്ലെങ്കിൽ എങ്കിൽ (refill_ready(കണ്ണി_STATE(ksCurSC)) && refill_sufficient(കണ്ണി_STATE(ksCurSC), 0)) {
        /* apply round robin */
        പ്രതിജ്ഞാ(!thread_state_get_tcbQueued(കണ്ണി_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } അല്ലെങ്കിൽ {
        /* postpone until ready */
        postpone(കണ്ണി_STATE(ksCurSC));
    }
}
#അല്ലെങ്കിൽ

ശൂന്യം timerTick(ശൂന്യം)
{
    എങ്കിൽ (likely(thread_state_get_tsType(കണ്ണി_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ifdef CONFIG_VTX
        || thread_state_get_tsType(കണ്ണി_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#endif
       ) {
        എങ്കിൽ (കണ്ണി_STATE(ksCurThread)->tcbTimeSlice > 1) {
            കണ്ണി_STATE(ksCurThread)->tcbTimeSlice--;
        } അല്ലെങ്കിൽ {
            കണ്ണി_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    എങ്കിൽ (അക്കം_Domains > 1) {
        ksDomainTime--;
        എങ്കിൽ (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#endif

ശൂന്യം rescheduleRequired(ശൂന്യം)
{
    എങ്കിൽ (കണ്ണി_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && കണ്ണി_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#ifdef CONFIG_KERNEL_MCS
        && isSchedulable(കണ്ണി_STATE(ksSchedulerAction))
#endif
       ) {
#ifdef CONFIG_KERNEL_MCS
        പ്രതിജ്ഞാ(refill_sufficient(കണ്ണി_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        പ്രതിജ്ഞാ(refill_ready(കണ്ണി_STATE(ksSchedulerAction)->tcbSchedContext));
#endif
        SCHED_ENQUEUE(കണ്ണി_STATE(ksSchedulerAction));
    }
    കണ്ണി_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#ifdef CONFIG_KERNEL_MCS

സ്ഥിതം വരിയിൽ bool_t PURE release_q_non_empty_and_ready(ശൂന്യം)
{
    തിരികെ കണ്ണി_STATE(ksReleaseQueue.head) != ശൂന്യ
           && refill_ready(കണ്ണി_STATE(ksReleaseQueue.head)->tcbSchedContext);
}

സ്ഥിതം ശൂന്യം tcbReleaseDequeue(ശൂന്യം)
{
    പ്രതിജ്ഞാ(കണ്ണി_STATE(ksReleaseQueue.head) != ശൂന്യ);
    പ്രതിജ്ഞാ(കണ്ണി_STATE(ksReleaseQueue.head)->tcbSchedPrev == ശൂന്യ);
    SMP_COND_STATEMENT(പ്രതിജ്ഞാ(കണ്ണി_STATE(ksReleaseQueue.head)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = കണ്ണി_STATE(ksReleaseQueue.head);
    പ്രതിജ്ഞാ(awakened != കണ്ണി_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    പ്രതിജ്ഞാ(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(പ്രതിജ്ഞാ(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    പ്രതിജ്ഞാ(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

ശൂന്യം awaken(ശൂന്യം)
{
    ആകുമ്പോൾ (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#endif
