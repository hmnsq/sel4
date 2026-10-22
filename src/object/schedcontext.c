/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <machine/timer.h>
#ಅನ್ತರ್ಭಾವ <kernel/sporadic.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/schedcontext.h>

ಸ್ಥಿರ exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_thread_cap:
        schedContext_unbindTCB(sc);
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_notification_cap:
        schedContext_unbindNtfn(sc);
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        fail("invalid cap type");
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    ಯದಿ (current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_thread_cap:
        ಯದಿ (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ಯದಿ (sc->scTcb == ಗಂಟು_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_notification_cap:
        ಯದಿ (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ಭಙ್ಗ;

    ಸ್ವಭಾವ:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeSchedContext_UnbindObject(sc, cap);
}

ಸ್ಥಿರ exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        fail("invalid cap type");
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    ಯದಿ (current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_thread_cap:
        ಯದಿ (sc->scTcb != ರಿಕ್ತ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != ರಿಕ್ತ) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಭಙ್ಗ;
    ಪಕ್ಷ cap_notification_cap:
        ಯದಿ (sc->scNotification != ರಿಕ್ತ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeSchedContext_Bind(sc, cap);
}

ಸ್ಥಿರ exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ maybeStallSC(sched_context_t *sc)
{
    ಯದಿ (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_ಸಂದೇಶ)
{
    time_t consumed = schedContext_updateConsumed(sc);

    ಯದಿ (write_ಸಂದೇಶ) {
        replyFromKernel_consumed(thread, consumed);
    }
}

ಸ್ಥಿರ exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, ಗಂಟು_STATE(ksCurThread), call);

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    ಯದಿ (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        ಪ್ರತಿಜ್ಞಾ(sc->scYieldFrom == ರಿಕ್ತ);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    ಯದಿ (isSchedulable(tcb)) {
        ಯದಿ (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < ಗಂಟು_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } ಅನ್ಯಥಾ {
            ಗಂಟು_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = ಗಂಟು_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(ಗಂಟು_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } ಅನ್ಯಥಾ {
        return_now = true;
    }

    ಯದಿ (return_now) {
        setConsumed(sc, ಗಂಟು_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    ಯದಿ (sc->scTcb == ರಿಕ್ತ) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    ಯದಿ (tcb == ಗಂಟು_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (tcb->tcbPriority > ಗಂಟು_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  ಗಂಟು_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE(ksCurThread)->tcbYieldTo == ರಿಕ್ತ);
    ಯದಿ (ಗಂಟು_STATE(ksCurThread)->tcbYieldTo != ರಿಕ್ತ) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    ವಿಕಲ್ಪ (label) {
    ಪಕ್ಷ SchedContextConsumed:
        /* no decode */
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeSchedContext_Consumed(sc, call);
    ಪಕ್ಷ SchedContextBind:
        ಪ್ರತ್ಯಾಗಮ decodeSchedContext_Bind(sc);
    ಪಕ್ಷ SchedContextUnbindObject:
        ಪ್ರತ್ಯಾಗಮ decodeSchedContext_UnbindObject(sc);
    ಪಕ್ಷ SchedContextUnbind:
        /* no decode */
        ಯದಿ (sc->scTcb == ಗಂಟು_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeSchedContext_Unbind(sc);
    ಪಕ್ಷ SchedContextYieldTo:
        ಪ್ರತ್ಯಾಗಮ decodeSchedContext_YieldTo(sc, call);
    ಸ್ವಭಾವ:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}

ಶೂನ್ಯ schedContext_resume(sched_context_t *sc)
{
    ಪ್ರತಿಜ್ಞಾ(!sc || sc->scTcb != ರಿಕ್ತ);
    ಯದಿ (likely(sc) && isSchedulable(sc->scTcb)) {
        ಯದಿ (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            ಪ್ರತಿಜ್ಞಾ(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

ಶೂನ್ಯ schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    ಪ್ರತಿಜ್ಞಾ(sc->scTcb == ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(tcb->tcbSchedContext == ರಿಕ್ತ);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    ಯದಿ (sc_sporadic(sc) && sc_active(sc) && sc != ಗಂಟು_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    ಯದಿ (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

ಶೂನ್ಯ schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ಪ್ರತಿಜ್ಞಾ(tcb != ರಿಕ್ತ);

    /* tcb must already be stalled at this point */
    ಯದಿ (tcb == ಗಂಟು_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = ರಿಕ್ತ;
    sc->scTcb = ರಿಕ್ತ;
}

ಶೂನ್ಯ schedContext_unbindAllTCBs(sched_context_t *sc)
{
    ಯದಿ (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

ಶೂನ್ಯ schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    ಪ್ರತಿಜ್ಞಾ(sc != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(to != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(to->tcbSchedContext == ರಿಕ್ತ);

    tcb_t *from = sc->scTcb;
    ಯದಿ (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = ರಿಕ್ತ;
        ಯದಿ (from == ಗಂಟು_STATE(ksCurThread) || from == ಗಂಟು_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

ಶೂನ್ಯ schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

ಶೂನ್ಯ schedContext_unbindNtfn(sched_context_t *sc)
{
    ಯದಿ (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = ರಿಕ್ತ;
    }
}

ಶೂನ್ಯ schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    ಯದಿ (sc) {
        schedContext_unbindNtfn(sc);
    }
}

ಶೂನ್ಯ schedContext_unbindReply(sched_context_t *sc)
{
    ಯದಿ (sc && sc->scReply) {
        ಪ್ರತಿಜ್ಞಾ(call_ರಾಶಿ_get_is_Head(sc->scReply->replyNext));
        sc->scReply->replyNext = call_ರಾಶಿ_new(0, false);
        sc->scReply = ರಿಕ್ತ;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    ಯದಿ (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        ಪ್ರತ್ಯಾಗಮ ticksToUs(getMaxTicksToUs());
    } ಅನ್ಯಥಾ {
        sc->scConsumed = 0;
        ಪ್ರತ್ಯಾಗಮ ticksToUs(consumed);
    }
}

ಶೂನ್ಯ schedContext_cancelYieldTo(tcb_t *tcb)
{
    ಯದಿ (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = ರಿಕ್ತ;
        tcb->tcbYieldTo = ರಿಕ್ತ;
    }
}

ಶೂನ್ಯ schedContext_completeYieldTo(tcb_t *yielder)
{
    ಯದಿ (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
