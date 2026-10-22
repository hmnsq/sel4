/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <object.h>
#ضمن <util.h>
#ضمن <api/faults.h>
#ضمن <api/types.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <object/domain.h>
#ifdef CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#endif
#ضمن <model/statedata.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/thread.h>
#ضمن <machine/registerset.h>
#ضمن <machine/fpu.h>
#ضمن <linker.h>

ساكن seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receiveBuffer);

BOOT_CODE فراغ configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

فراغ activateThread(فراغ)
{
#ifdef CONFIG_KERNEL_MCS
    𐤀𐤌 (unlikely(NODE_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(NODE_STATE(ksCurThread));
        أكد(thread_state_get_tsType(NODE_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#endif

    بدل (thread_state_get_tsType(NODE_STATE(ksCurThread)->tcbState)) {
    حالة ThreadState_Running:
#ifdef CONFIG_VTX
    حالة ThreadState_RunningVM:
#endif
        اكسر;

    حالة ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(NODE_STATE(ksCurThread));
        setNextPC(NODE_STATE(ksCurThread), pc);
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
        اكسر;
    }

    حالة ThreadState_IdleThreadState:
        Arch_activateIdleThread(NODE_STATE(ksCurThread));
        اكسر;

    افتراضي:
        fail("Current thread is blocked");
    }
}

فراغ suspend(tcb_t *target)
{
    cancelIPC(target);
    𐤀𐤌 (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
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

فراغ restart(tcb_t *target)
{
    𐤀𐤌 (isStopped(target)) {
        cancelIPC(target);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        𐤀𐤌 (sc_sporadic(sc) && sc != NODE_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        𐤀𐤌 (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#وإلا
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#endif
    }
}

فراغ doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    فراغ *receiveBuffer, *sendBuffer;

    receiveBuffer = lookupIPCBuffer(true, receiver);

    𐤀𐤌 (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        sendBuffer = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, sendBuffer, endpoint, badge, grant,
                         receiver, receiveBuffer);
    } وإلا {
        doFaultTransfer(badge, sender, receiver, receiveBuffer);
    }
}

#ifdef CONFIG_KERNEL_MCS
فراغ doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#وإلا
فراغ doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#endif
{
#ifdef CONFIG_KERNEL_MCS
    𐤀𐤌 (reply->replyTCB == عدم ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        أرجع;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    أكد(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    أكد(reply->replyTCB == عدم);

    𐤀𐤌 (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != NODE_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#وإلا
    أكد(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#endif

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    𐤀𐤌 (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, عدم, 0, grant, receiver);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#وإلا
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#endif
    } وإلا {
#ifndef CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#endif
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        𐤀𐤌 (restart) {
            setThreadState(receiver, ThreadState_Restart);
#ifndef CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#endif
        } وإلا {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    𐤀𐤌 (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        𐤀𐤌 ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } وإلا {
            𐤀𐤌 (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } وإلا {
                postpone(sc);
            }
        }
    }
#endif
}

فراغ doNormalTransfer(tcb_t *sender, word_t *sendBuffer, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receiveBuffer)
{
    word_t msgTransferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, msgInfoRegister));

    𐤀𐤌 (canGrant) {
        status = lookupExtraCaps(sender, sendBuffer, tag);
        𐤀𐤌 (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = عدم;
        }
    } وإلا {
        current_extra_caps.excaprefs[0] = عدم;
    }

    msgTransferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
                             seL4_MessageInfo_get_length(tag));

    tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

    tag = seL4_MessageInfo_set_length(tag, msgTransferred);
    setRegister(receiver, msgInfoRegister, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

فراغ doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
                     word_t *receiverIPCBuffer)
{
    word_t sent;
    seL4_MessageInfo_t msgInfo;

    sent = setMRs_fault(sender, receiver, receiverIPCBuffer);
    msgInfo = seL4_MessageInfo_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, sent);
    setRegister(receiver, msgInfoRegister, wordFromMessageInfo(msgInfo));
    setRegister(receiver, badgeRegister, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ساكن seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receiveBuffer)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    𐤀𐤌 (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
        أرجع info;
    }

    destSlot = getReceiveSlots(receiver, receiveBuffer);

    لكل (i = 0; i < seL4_MsgMaxExtraCaps && current_extra_caps.excaprefs[i] != عدم; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        𐤀𐤌 (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receiveBuffer,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } وإلا {
            deriveCap_ret_t dc_ret;

            𐤀𐤌 (!destSlot) {
                اكسر;
            }

            dc_ret = deriveCap(slot, cap);

            𐤀𐤌 (dc_ret.status != EXCEPTION_NONE) {
                اكسر;
            }
            𐤀𐤌 (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                اكسر;
            }

            cteInsert(dc_ret.cap, slot, destSlot);

            destSlot = عدم;
        }
    }

    أرجع seL4_MessageInfo_set_extraCaps(info, i);
}

فراغ doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

فراغ prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    𐤀𐤌 (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#ifdef CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#endif
    }
}

ساكن فراغ prepareNextDomain(فراغ)
{
    Arch_prepareNextDomain();
#ifdef CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(عدم);
#endif
}

ساكن فراغ nextDomain(فراغ)
{
    ksDomScheduleIdx++;
    𐤀𐤌 (dschedule_is_end_marker(ksDomScheduleIdx)) {
        ksDomScheduleIdx = ksDomScheduleStart;
    }
#ifdef CONFIG_KERNEL_MCS
    NODE_STATE(ksReprogram) = true;
#endif
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ksDomSchedule[ksDomScheduleIdx]);
    ksDomainTime = dschedule_duration(ksDomSchedule[ksDomScheduleIdx]);
}

#ifdef CONFIG_KERNEL_MCS
ساكن فراغ switchSchedContext(فراغ)
{
    𐤀𐤌 (unlikely(NODE_STATE(ksCurSC) != NODE_STATE(ksCurThread)->tcbSchedContext)) {
        NODE_STATE(ksReprogram) = true;
        𐤀𐤌 (sc_constant_bandwidth(NODE_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(NODE_STATE(ksCurThread)->tcbSchedContext);
        }

        أكد(refill_ready(NODE_STATE(ksCurThread)->tcbSchedContext));
        أكد(refill_sufficient(NODE_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    𐤀𐤌 (NODE_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    NODE_STATE(ksCurSC) = NODE_STATE(ksCurThread)->tcbSchedContext;
}
#endif

ساكن فراغ scheduleChooseNewThread(فراغ)
{
    𐤀𐤌 (ksDomainTime == 0) {
        prepareNextDomain();
        nextDomain();
    }
    chooseThread();
}

فراغ schedule(فراغ)
{
#ifdef CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#endif

    𐤀𐤌 (NODE_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        𐤀𐤌 (isSchedulable(NODE_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } وإلا {
            was_runnable = false;
        }

        𐤀𐤌 (NODE_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } وإلا {
            tcb_t *candidate = NODE_STATE(ksSchedulerAction);
            أكد(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                NODE_STATE(ksCurThread) == NODE_STATE(ksIdleThread)
                || (candidate->tcbPriority < NODE_STATE(ksCurThread)->tcbPriority);
            𐤀𐤌 (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                NODE_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } وإلا 𐤀𐤌 (was_runnable && candidate->tcbPriority == NODE_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                NODE_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } وإلا {
                أكد(candidate != NODE_STATE(ksCurThread));
                switchToThread(candidate);
            }
        }
    }
    NODE_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ifdef ENABLE_SMP_SUPPORT
    doMaskReschedule(ARCH_NODE_STATE(ipiReschedulePending));
    ARCH_NODE_STATE(ipiReschedulePending) = 0;
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_KERNEL_MCS
    switchSchedContext();

    𐤀𐤌 (NODE_STATE(ksReprogram)) {
        setNextInterrupt();
        NODE_STATE(ksReprogram) = false;
    }
#endif
}

فراغ chooseThread(فراغ)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    𐤀𐤌 (numDomains > 1) {
        dom = ksCurDomain;
    } وإلا {
        dom = 0;
    }

    𐤀𐤌 (likely(NODE_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = NODE_STATE(ksReadyQueues)[ready_queues_index(dom, prio)].head;
        أكد(thread);
        أكد(isSchedulable(thread));
#ifdef CONFIG_KERNEL_MCS
        أكد(refill_sufficient(thread->tcbSchedContext, 0));
        أكد(refill_ready(thread->tcbSchedContext));
#endif
        switchToThread(thread);
    } وإلا {
        switchToIdleThread();
    }
}

فراغ switchToThread(tcb_t *thread)
{
#ifdef CONFIG_KERNEL_MCS
    أكد(thread->tcbSchedContext != عدم);
    أكد(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    أكد(refill_sufficient(thread->tcbSchedContext, 0));
    أكد(refill_ready(thread->tcbSchedContext));
#endif

#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(NODE_STATE(ksCurThread), thread);
#endif
    Arch_switchToThread(thread);

#ifdef CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#endif /* CONFIG_HAVE_FPU */

    tcbSchedDequeue(thread);
    NODE_STATE(ksCurThread) = thread;
}

فراغ switchToIdleThread(فراغ)
{
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(NODE_STATE(ksCurThread), NODE_STATE(ksIdleThread));
#endif
    Arch_switchToIdleThread();
    NODE_STATE(ksCurThread) = NODE_STATE(ksIdleThread);
}

فراغ setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    𐤀𐤌 (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    𐤀𐤌 (tptr == NODE_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

فراغ setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ifdef CONFIG_KERNEL_MCS
فراغ setPriority(tcb_t *tptr, prio_t prio)
{
    بدل (thread_state_get_tsType(tptr->tcbState)) {
    حالة ThreadState_Running:
    حالة ThreadState_Restart:
        𐤀𐤌 (thread_state_get_tcbQueued(tptr->tcbState) || tptr == NODE_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } وإلا {
            tptr->tcbPriority = prio;
        }
        اكسر;
    حالة ThreadState_BlockedOnReceive:
    حالة ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        اكسر;
    حالة ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        اكسر;
    افتراضي:
        tptr->tcbPriority = prio;
        اكسر;
    }
}
#وإلا
فراغ setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    𐤀𐤌 (isRunnable(tptr)) {
        𐤀𐤌 (tptr == NODE_STATE(ksCurThread)) {
            rescheduleRequired();
        } وإلا {
            possibleSwitchTo(tptr);
        }
    }
}
#endif

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
فراغ possibleSwitchTo(tcb_t *target)
{
#ifdef CONFIG_KERNEL_MCS
    𐤀𐤌 (target->tcbSchedContext != عدم && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#endif
        𐤀𐤌 (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } وإلا 𐤀𐤌 (NODE_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } وإلا {
            NODE_STATE(ksSchedulerAction) = target;
        }
#ifdef CONFIG_KERNEL_MCS
    }
#endif

}

فراغ setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

فراغ scheduleTCB(tcb_t *tptr)
{
    𐤀𐤌 (tptr == NODE_STATE(ksCurThread) &&
        NODE_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        NODE_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#ifdef CONFIG_KERNEL_MCS
فراغ postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    أكد(tcb != عدم);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    NODE_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

فراغ setNextInterrupt(فراغ)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_head_refill = *refill_head(NODE_STATE(ksCurThread)->tcbSchedContext);
    ticks_t next_interrupt = NODE_STATE(ksCurTime) + ct_head_refill.rAmount;

    𐤀𐤌 (numDomains > 1) {
        next_interrupt = MIN(next_interrupt, NODE_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_head = NODE_STATE(ksReleaseQueue.head);
    𐤀𐤌 (rlq_head != عدم) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_head_refill = *refill_head(rlq_head->tcbSchedContext);
        next_interrupt = MIN(rlq_head_refill.rTime, next_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    أكد(next_interrupt >= NODE_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(next_interrupt - getTimerPrecision());
}

فراغ chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    𐤀𐤌 (likely(NODE_STATE(ksCurSC) != NODE_STATE(ksIdleSC))) {
        𐤀𐤌 (isRoundRobin(NODE_STATE(ksCurSC))) {
            أكد(refill_size(NODE_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t head = *refill_head(NODE_STATE(ksCurSC));
            refill_t tail = *refill_tail(NODE_STATE(ksCurSC));
            refill_head(NODE_STATE(ksCurSC))->rAmount = head.rAmount + tail.rAmount;
            refill_tail(NODE_STATE(ksCurSC))->rAmount = 0;
        } وإلا {
            refill_budget_check(consumed);
        }

        أكد(refill_head(NODE_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        NODE_STATE(ksCurSC)->scConsumed += consumed;
    }
    NODE_STATE(ksConsumed) = 0;
    𐤀𐤌 (likely(isSchedulable(NODE_STATE(ksCurThread)))) {
        أكد(NODE_STATE(ksCurThread)->tcbSchedContext == NODE_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        NODE_STATE(ksReprogram) = true;
    }
}

فراغ endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(NODE_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(NODE_STATE(ksCurThread));

    𐤀𐤌 (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(NODE_STATE(ksCurSC)->scBadge);
        handleTimeout(NODE_STATE(ksCurThread));
    } وإلا 𐤀𐤌 (refill_ready(NODE_STATE(ksCurSC)) && refill_sufficient(NODE_STATE(ksCurSC), 0)) {
        /* apply round robin */
        أكد(!thread_state_get_tcbQueued(NODE_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } وإلا {
        /* postpone until ready */
        postpone(NODE_STATE(ksCurSC));
    }
}
#وإلا

فراغ timerTick(فراغ)
{
    𐤀𐤌 (likely(thread_state_get_tsType(NODE_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ifdef CONFIG_VTX
        || thread_state_get_tsType(NODE_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#endif
       ) {
        𐤀𐤌 (NODE_STATE(ksCurThread)->tcbTimeSlice > 1) {
            NODE_STATE(ksCurThread)->tcbTimeSlice--;
        } وإلا {
            NODE_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    𐤀𐤌 (numDomains > 1) {
        ksDomainTime--;
        𐤀𐤌 (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#endif

فراغ rescheduleRequired(فراغ)
{
    𐤀𐤌 (NODE_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && NODE_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#ifdef CONFIG_KERNEL_MCS
        && isSchedulable(NODE_STATE(ksSchedulerAction))
#endif
       ) {
#ifdef CONFIG_KERNEL_MCS
        أكد(refill_sufficient(NODE_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        أكد(refill_ready(NODE_STATE(ksSchedulerAction)->tcbSchedContext));
#endif
        SCHED_ENQUEUE(NODE_STATE(ksSchedulerAction));
    }
    NODE_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#ifdef CONFIG_KERNEL_MCS

ساكن مضمن bool_t PURE release_q_non_empty_and_ready(فراغ)
{
    أرجع NODE_STATE(ksReleaseQueue.head) != عدم
           && refill_ready(NODE_STATE(ksReleaseQueue.head)->tcbSchedContext);
}

ساكن فراغ tcbReleaseDequeue(فراغ)
{
    أكد(NODE_STATE(ksReleaseQueue.head) != عدم);
    أكد(NODE_STATE(ksReleaseQueue.head)->tcbSchedPrev == عدم);
    SMP_COND_STATEMENT(أكد(NODE_STATE(ksReleaseQueue.head)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = NODE_STATE(ksReleaseQueue.head);
    أكد(awakened != NODE_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    أكد(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(أكد(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    أكد(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

فراغ awaken(فراغ)
{
    طالما (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#endif
