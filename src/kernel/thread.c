/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <object.h>
#include <util.h>
#include <api/faults.h>
#include <api/types.h>
#include <kernel/cspace.h>
#include <kernel/thread.h>
#include <kernel/vspace.h>
#include <object/domain.h>
#ifdef CONFIG_KERNEL_MCS
#include <object/schedcontext.h>
#endif
#include <model/statedata.h>
#include <arch/machine.h>
#include <arch/kernel/thread.h>
#include <machine/registerset.h>
#include <machine/fpu.h>
#include <linker.h>

static seL4_MessageInfo_t
transferCaps(seL4_MessageInfo_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             word_t *receiveBuffer);

BOOT_CODE void configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    setThreadState(tcb, ThreadState_IdleThreadState);
}

void activateThread(void)
{
#ifdef CONFIG_KERNEL_MCS
    ᬬᬾᬦ᭄ (unlikely(NODE_STATE(ksCurThread)->tcbYieldTo)) {
        schedContext_completeYieldTo(NODE_STATE(ksCurThread));
        assert(thread_state_get_tsType(NODE_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#endif

    switch (thread_state_get_tsType(NODE_STATE(ksCurThread)->tcbState)) {
    pak_sa ThreadState_Running:
#ifdef CONFIG_VTX
    pak_sa ThreadState_RunningVM:
#endif
        bha_nga;

    pak_sa ThreadState_Restart: {
        word_t pc;

        pc = getRestartPC(NODE_STATE(ksCurThread));
        setNextPC(NODE_STATE(ksCurThread), pc);
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
        bha_nga;
    }

    pak_sa ThreadState_IdleThreadState:
        Arch_activateIdleThread(NODE_STATE(ksCurThread));
        bha_nga;

    default:
        fail("Current thread is blocked");
    }
}

void suspend(tcb_t *target)
{
    cancelIPC(target);
    ᬬᬾᬦ᭄ (thread_state_get_tsType(target->tcbState) == ThreadState_Running) {
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

void restart(tcb_t *target)
{
    ᬬᬾᬦ᭄ (isStopped(target)) {
        cancelIPC(target);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(target, ThreadState_Restart);
        sched_context_t *sc = target->tcbSchedContext;
        ᬬᬾᬦ᭄ (sc_sporadic(sc) && sc != NODE_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        ᬬᬾᬦ᭄ (isSchedulable(target)) {
            possibleSwitchTo(target);
        }
#ᬮᬶᬬ
        setupReplyMaster(target);
        setThreadState(target, ThreadState_Restart);
        SCHED_ENQUEUE(target);
        possibleSwitchTo(target);
#endif
    }
}

void doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, word_t badge,
                   bool_t grant, tcb_t *receiver)
{
    void *receiveBuffer, *sendBuffer;

    receiveBuffer = lookupIPCBuffer(true, receiver);

    ᬬᬾᬦ᭄ (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        sendBuffer = lookupIPCBuffer(false, sender);
        doNormalTransfer(sender, sendBuffer, endpoint, badge, grant,
                         receiver, receiveBuffer);
    } ᬮᬶᬬ {
        doFaultTransfer(badge, sender, receiver, receiveBuffer);
    }
}

#ifdef CONFIG_KERNEL_MCS
void doReplyTransfer(tcb_t *sender, reply_t *reply, bool_t grant)
#ᬮᬶᬬ
void doReplyTransfer(tcb_t *sender, tcb_t *receiver, cte_t *slot, bool_t grant)
#endif
{
#ifdef CONFIG_KERNEL_MCS
    ᬬᬾᬦ᭄ (reply->replyTCB == NULL ||
        thread_state_get_tsType(reply->replyTCB->tcbState) != ThreadState_BlockedOnReply) {
        /* nothing to do */
        ᬯᬮᬶ;
    }

    tcb_t *receiver = reply->replyTCB;
    reply_remove(reply, receiver);
    assert(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    assert(reply->replyTCB == NULL);

    ᬬᬾᬦ᭄ (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != NODE_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#ᬮᬶᬬ
    assert(thread_state_get_tsType(receiver->tcbState) ==
           ThreadState_BlockedOnReply);
#endif

    word_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    ᬬᬾᬦ᭄ (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, NULL, 0, grant, receiver);
#ifdef CONFIG_KERNEL_MCS
        setThreadState(receiver, ThreadState_Running);
#ᬮᬶᬬ
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
        setThreadState(receiver, ThreadState_Running);
        possibleSwitchTo(receiver);
#endif
    } ᬮᬶᬬ {
#ifndef CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(slot);
#endif
        bool_t restart = handleFaultReply(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        ᬬᬾᬦ᭄ (restart) {
            setThreadState(receiver, ThreadState_Restart);
#ifndef CONFIG_KERNEL_MCS
            possibleSwitchTo(receiver);
#endif
        } ᬮᬶᬬ {
            setThreadState(receiver, ThreadState_Inactive);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    ᬬᬾᬦ᭄ (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        ᬬᬾᬦ᭄ ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possibleSwitchTo(receiver);
        } ᬮᬶᬬ {
            ᬬᬾᬦ᭄ (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } ᬮᬶᬬ {
                postpone(sc);
            }
        }
    }
#endif
}

void doNormalTransfer(tcb_t *sender, word_t *sendBuffer, endpoint_t *endpoint,
                      word_t badge, bool_t canGrant, tcb_t *receiver,
                      word_t *receiveBuffer)
{
    word_t msgTransferred;
    seL4_MessageInfo_t tag;
    exception_t status;

    tag = messageInfoFromWord(getRegister(sender, msgInfoRegister));

    ᬬᬾᬦ᭄ (canGrant) {
        status = lookupExtraCaps(sender, sendBuffer, tag);
        ᬬᬾᬦ᭄ (unlikely(status != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = NULL;
        }
    } ᬮᬶᬬ {
        current_extra_caps.excaprefs[0] = NULL;
    }

    msgTransferred = copyMRs(sender, sendBuffer, receiver, receiveBuffer,
                             seL4_MessageInfo_get_length(tag));

    tag = transferCaps(tag, endpoint, receiver, receiveBuffer);

    tag = seL4_MessageInfo_set_length(tag, msgTransferred);
    setRegister(receiver, msgInfoRegister, wordFromMessageInfo(tag));
    setRegister(receiver, badgeRegister, badge);
}

void doFaultTransfer(word_t badge, tcb_t *sender, tcb_t *receiver,
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
static seL4_MessageInfo_t transferCaps(seL4_MessageInfo_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       word_t *receiveBuffer)
{
    word_t i;
    cte_t *destSlot;

    info = seL4_MessageInfo_set_extraCaps(info, 0);
    info = seL4_MessageInfo_set_capsUnwrapped(info, 0);

    ᬬᬾᬦ᭄ (likely(!current_extra_caps.excaprefs[0] || !receiveBuffer)) {
        ᬯᬮᬶ info;
    }

    destSlot = getReceiveSlots(receiver, receiveBuffer);

    for (i = 0; i < seL4_MsgMaxExtraCaps && current_extra_caps.excaprefs[i] != NULL; i++) {
        cte_t *slot = current_extra_caps.excaprefs[i];
        cap_t cap = slot->cap;

        ᬬᬾᬦ᭄ (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            setExtraBadge(receiveBuffer,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = seL4_MessageInfo_set_capsUnwrapped(info,
                                                      seL4_MessageInfo_get_capsUnwrapped(info) | (1 << i));

        } ᬮᬶᬬ {
            deriveCap_ret_t dc_ret;

            ᬬᬾᬦ᭄ (!destSlot) {
                bha_nga;
            }

            dc_ret = deriveCap(slot, cap);

            ᬬᬾᬦ᭄ (dc_ret.status != EXCEPTION_NONE) {
                bha_nga;
            }
            ᬬᬾᬦ᭄ (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                bha_nga;
            }

            cteInsert(dc_ret.cap, slot, destSlot);

            destSlot = NULL;
        }
    }

    ᬯᬮᬶ seL4_MessageInfo_set_extraCaps(info, i);
}

void doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    setRegister(thread, badgeRegister, 0);
}

void prepareSetDomain(tcb_t *tptr, dom_t dom)
{
    ᬬᬾᬦ᭄ (ksCurDomain != dom) {
        Arch_prepareSetDomain(tptr, dom);
#ifdef CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#endif
    }
}

static void prepareNextDomain(void)
{
    Arch_prepareNextDomain();
#ifdef CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(NULL);
#endif
}

static void nextDomain(void)
{
    ksDomScheduleIdx++;
    ᬬᬾᬦ᭄ (dschedule_is_end_marker(ksDomScheduleIdx)) {
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
static void switchSchedContext(void)
{
    ᬬᬾᬦ᭄ (unlikely(NODE_STATE(ksCurSC) != NODE_STATE(ksCurThread)->tcbSchedContext)) {
        NODE_STATE(ksReprogram) = true;
        ᬬᬾᬦ᭄ (sc_constant_bandwidth(NODE_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(NODE_STATE(ksCurThread)->tcbSchedContext);
        }

        assert(refill_ready(NODE_STATE(ksCurThread)->tcbSchedContext));
        assert(refill_sufficient(NODE_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    ᬬᬾᬦ᭄ (NODE_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commitTime();
    }

    NODE_STATE(ksCurSC) = NODE_STATE(ksCurThread)->tcbSchedContext;
}
#endif

static void scheduleChooseNewThread(void)
{
    ᬬᬾᬦ᭄ (ksDomainTime == 0) {
        prepareNextDomain();
        nextDomain();
    }
    chooseThread();
}

void schedule(void)
{
#ifdef CONFIG_KERNEL_MCS
    awaken();
    checkDomainTime();
#endif

    ᬬᬾᬦ᭄ (NODE_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        ᬬᬾᬦ᭄ (isSchedulable(NODE_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ENQUEUE_CURRENT_TCB;
        } ᬮᬶᬬ {
            was_runnable = false;
        }

        ᬬᬾᬦ᭄ (NODE_STATE(ksSchedulerAction) == SchedulerAction_ChooseNewThread) {
            scheduleChooseNewThread();
        } ᬮᬶᬬ {
            tcb_t *candidate = NODE_STATE(ksSchedulerAction);
            assert(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                NODE_STATE(ksCurThread) == NODE_STATE(ksIdleThread)
                || (candidate->tcbPriority < NODE_STATE(ksCurThread)->tcbPriority);
            ᬬᬾᬦ᭄ (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcbPriority)) {
                SCHED_ENQUEUE(candidate);
                /* we can't, need to reschedule */
                NODE_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } ᬮᬶᬬ ᬬᬾᬦ᭄ (was_runnable && candidate->tcbPriority == NODE_STATE(ksCurThread)->tcbPriority) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                NODE_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
                scheduleChooseNewThread();
            } ᬮᬶᬬ {
                assert(candidate != NODE_STATE(ksCurThread));
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

    ᬬᬾᬦ᭄ (NODE_STATE(ksReprogram)) {
        setNextInterrupt();
        NODE_STATE(ksReprogram) = false;
    }
#endif
}

void chooseThread(void)
{
    word_t prio;
    word_t dom;
    tcb_t *thread;

    ᬬᬾᬦ᭄ (numDomains > 1) {
        dom = ksCurDomain;
    } ᬮᬶᬬ {
        dom = 0;
    }

    ᬬᬾᬦ᭄ (likely(NODE_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = NODE_STATE(ksReadyQueues)[ready_queues_index(dom, prio)].head;
        assert(thread);
        assert(isSchedulable(thread));
#ifdef CONFIG_KERNEL_MCS
        assert(refill_sufficient(thread->tcbSchedContext, 0));
        assert(refill_ready(thread->tcbSchedContext));
#endif
        switchToThread(thread);
    } ᬮᬶᬬ {
        switchToIdleThread();
    }
}

void switchToThread(tcb_t *thread)
{
#ifdef CONFIG_KERNEL_MCS
    assert(thread->tcbSchedContext != NULL);
    assert(!thread_state_get_tcbInReleaseQueue(thread->tcbState));
    assert(refill_sufficient(thread->tcbSchedContext, 0));
    assert(refill_ready(thread->tcbSchedContext));
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

void switchToIdleThread(void)
{
#ifdef CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(NODE_STATE(ksCurThread), NODE_STATE(ksIdleThread));
#endif
    Arch_switchToIdleThread();
    NODE_STATE(ksCurThread) = NODE_STATE(ksIdleThread);
}

void setDomain(tcb_t *tptr, dom_t dom)
{
    tcbSchedDequeue(tptr);
    tptr->tcbDomain = dom;
    ᬬᬾᬦ᭄ (isSchedulable(tptr)) {
        SCHED_ENQUEUE(tptr);
    }
    ᬬᬾᬦ᭄ (tptr == NODE_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

void setMCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ifdef CONFIG_KERNEL_MCS
void setPriority(tcb_t *tptr, prio_t prio)
{
    switch (thread_state_get_tsType(tptr->tcbState)) {
    pak_sa ThreadState_Running:
    pak_sa ThreadState_Restart:
        ᬬᬾᬦ᭄ (thread_state_get_tcbQueued(tptr->tcbState) || tptr == NODE_STATE(ksCurThread)) {
            tcbSchedDequeue(tptr);
            tptr->tcbPriority = prio;
            SCHED_ENQUEUE(tptr);
            rescheduleRequired();
        } ᬮᬶᬬ {
            tptr->tcbPriority = prio;
        }
        bha_nga;
    pak_sa ThreadState_BlockedOnReceive:
    pak_sa ThreadState_BlockedOnSend:
        tptr->tcbPriority = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        bha_nga;
    pak_sa ThreadState_BlockedOnNotification:
        tptr->tcbPriority = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        bha_nga;
    default:
        tptr->tcbPriority = prio;
        bha_nga;
    }
}
#ᬮᬶᬬ
void setPriority(tcb_t *tptr, prio_t prio)
{
    tcbSchedDequeue(tptr);
    tptr->tcbPriority = prio;
    ᬬᬾᬦ᭄ (isRunnable(tptr)) {
        ᬬᬾᬦ᭄ (tptr == NODE_STATE(ksCurThread)) {
            rescheduleRequired();
        } ᬮᬶᬬ {
            possibleSwitchTo(tptr);
        }
    }
}
#endif

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
void possibleSwitchTo(tcb_t *target)
{
#ifdef CONFIG_KERNEL_MCS
    ᬬᬾᬦ᭄ (target->tcbSchedContext != NULL && !thread_state_get_tcbInReleaseQueue(target->tcbState)) {
#endif
        ᬬᬾᬦ᭄ (ksCurDomain != target->tcbDomain
            SMP_COND_STATEMENT( || target->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ENQUEUE(target);
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (NODE_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ENQUEUE(target);
        } ᬮᬶᬬ {
            NODE_STATE(ksSchedulerAction) = target;
        }
#ifdef CONFIG_KERNEL_MCS
    }
#endif

}

void setThreadState(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_set_tsType(&tptr->tcbState, ts);
    scheduleTCB(tptr);
}

void scheduleTCB(tcb_t *tptr)
{
    ᬬᬾᬦ᭄ (tptr == NODE_STATE(ksCurThread) &&
        NODE_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        NODE_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
    }
}

#ifdef CONFIG_KERNEL_MCS
void postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    assert(tcb != NULL);

    tcbSchedDequeue(tcb);
    tcbReleaseEnqueue(tcb);
    NODE_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

void setNextInterrupt(void)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_head_refill = *refill_head(NODE_STATE(ksCurThread)->tcbSchedContext);
    ticks_t next_interrupt = NODE_STATE(ksCurTime) + ct_head_refill.rAmount;

    ᬬᬾᬦ᭄ (numDomains > 1) {
        next_interrupt = MIN(next_interrupt, NODE_STATE(ksCurTime) + ksDomainTime);
    }

    tcb_t *rlq_head = NODE_STATE(ksReleaseQueue.head);
    ᬬᬾᬦ᭄ (rlq_head != NULL) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_head_refill = *refill_head(rlq_head->tcbSchedContext);
        next_interrupt = MIN(rlq_head_refill.rTime, next_interrupt);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    assert(next_interrupt >= NODE_STATE(ksCurTime));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    setDeadline(next_interrupt - getTimerPrecision());
}

void chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    ᬬᬾᬦ᭄ (likely(NODE_STATE(ksCurSC) != NODE_STATE(ksIdleSC))) {
        ᬬᬾᬦ᭄ (isRoundRobin(NODE_STATE(ksCurSC))) {
            assert(refill_size(NODE_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t head = *refill_head(NODE_STATE(ksCurSC));
            refill_t tail = *refill_tail(NODE_STATE(ksCurSC));
            refill_head(NODE_STATE(ksCurSC))->rAmount = head.rAmount + tail.rAmount;
            refill_tail(NODE_STATE(ksCurSC))->rAmount = 0;
        } ᬮᬶᬬ {
            refill_budget_check(consumed);
        }

        assert(refill_head(NODE_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        NODE_STATE(ksCurSC)->scConsumed += consumed;
    }
    NODE_STATE(ksConsumed) = 0;
    ᬬᬾᬦ᭄ (likely(isSchedulable(NODE_STATE(ksCurThread)))) {
        assert(NODE_STATE(ksCurThread)->tcbSchedContext == NODE_STATE(ksCurSC));
        endTimeslice(canTimeoutFault);
        rescheduleRequired();
        NODE_STATE(ksReprogram) = true;
    }
}

void endTimeslice(bool_t can_timeout_fault)
{
    bool_t round_robin = isRoundRobin(NODE_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(NODE_STATE(ksCurThread));

    ᬬᬾᬦ᭄ (can_timeout_fault && !round_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(NODE_STATE(ksCurSC)->scBadge);
        handleTimeout(NODE_STATE(ksCurThread));
    } ᬮᬶᬬ ᬬᬾᬦ᭄ (refill_ready(NODE_STATE(ksCurSC)) && refill_sufficient(NODE_STATE(ksCurSC), 0)) {
        /* apply round robin */
        assert(!thread_state_get_tcbQueued(NODE_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } ᬮᬶᬬ {
        /* postpone until ready */
        postpone(NODE_STATE(ksCurSC));
    }
}
#ᬮᬶᬬ

void timerTick(void)
{
    ᬬᬾᬦ᭄ (likely(thread_state_get_tsType(NODE_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ifdef CONFIG_VTX
        || thread_state_get_tsType(NODE_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#endif
       ) {
        ᬬᬾᬦ᭄ (NODE_STATE(ksCurThread)->tcbTimeSlice > 1) {
            NODE_STATE(ksCurThread)->tcbTimeSlice--;
        } ᬮᬶᬬ {
            NODE_STATE(ksCurThread)->tcbTimeSlice = CONFIG_TIME_SLICE;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    ᬬᬾᬦ᭄ (numDomains > 1) {
        ksDomainTime--;
        ᬬᬾᬦ᭄ (ksDomainTime == 0) {
            rescheduleRequired();
        }
    }
}
#endif

void rescheduleRequired(void)
{
    ᬬᬾᬦ᭄ (NODE_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && NODE_STATE(ksSchedulerAction) != SchedulerAction_ChooseNewThread
#ifdef CONFIG_KERNEL_MCS
        && isSchedulable(NODE_STATE(ksSchedulerAction))
#endif
       ) {
#ifdef CONFIG_KERNEL_MCS
        assert(refill_sufficient(NODE_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        assert(refill_ready(NODE_STATE(ksSchedulerAction)->tcbSchedContext));
#endif
        SCHED_ENQUEUE(NODE_STATE(ksSchedulerAction));
    }
    NODE_STATE(ksSchedulerAction) = SchedulerAction_ChooseNewThread;
}

#ifdef CONFIG_KERNEL_MCS

static anta_hstha bool_t PURE release_q_non_empty_and_ready(void)
{
    ᬯᬮᬶ NODE_STATE(ksReleaseQueue.head) != NULL
           && refill_ready(NODE_STATE(ksReleaseQueue.head)->tcbSchedContext);
}

static void tcbReleaseDequeue(void)
{
    assert(NODE_STATE(ksReleaseQueue.head) != NULL);
    assert(NODE_STATE(ksReleaseQueue.head)->tcbSchedPrev == NULL);
    SMP_COND_STATEMENT(assert(NODE_STATE(ksReleaseQueue.head)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = NODE_STATE(ksReleaseQueue.head);
    assert(awakened != NODE_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    assert(!isRoundRobin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(assert(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    assert(refill_sufficient(awakened->tcbSchedContext, 0));
    possibleSwitchTo(awakened);
}

void awaken(void)
{
    ᬲᭂᬤᭂᬮ (unlikely(release_q_non_empty_and_ready())) {
        tcbReleaseDequeue();
    }
}
#endif
