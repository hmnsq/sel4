/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <machine/timer.h>
#અન્તર્ભાવ <kernel/sporadic.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <object/schedcontext.h>

સ્થિર exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_thread_cap:
        schedContext_unbindTCB(sc);
        તોડો;
    સ્થિતિ cap_notification_cap:
        schedContext_unbindNtfn(sc);
        તોડો;
    મૂળભૂત:
        fail("invalid cap type");
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    જો (current_extra_caps.excaprefs[0] == ખાલી) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_thread_cap:
        જો (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        જો (sc->scTcb == ગાંઠ_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        તોડો;
    સ્થિતિ cap_notification_cap:
        જો (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        તોડો;

    મૂળભૂત:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeSchedContext_UnbindObject(sc, cap);
}

સ્થિર exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        તોડો;
    સ્થિતિ cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        તોડો;
    મૂળભૂત:
        fail("invalid cap type");
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    જો (current_extra_caps.excaprefs[0] == ખાલી) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_thread_cap:
        જો (sc->scTcb != ખાલી) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != ખાલી) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        તોડો;
    સ્થિતિ cap_notification_cap:
        જો (sc->scNotification != ખાલી) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        તોડો;
    મૂળભૂત:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeSchedContext_Bind(sc, cap);
}

સ્થિર exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    પાછા EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
સ્થિર અંતઃરેખ શૂન્ય maybeStallSC(sched_context_t *sc)
{
    જો (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

સ્થિર અંતઃરેખ શૂન્ય replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

સ્થિર અંતઃરેખ શૂન્ય setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_સંદેશ)
{
    time_t consumed = schedContext_updateConsumed(sc);

    જો (write_સંદેશ) {
        replyFromKernel_consumed(thread, consumed);
    }
}

સ્થિર exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, ગાંઠ_STATE(ksCurThread), call);

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    જો (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        પ્રતિજ્ઞા(sc->scYieldFrom == ખાલી);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    જો (isSchedulable(tcb)) {
        જો (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < ગાંઠ_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } નહીંતર {
            ગાંઠ_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = ગાંઠ_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(ગાંઠ_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } નહીંતર {
        return_now = true;
    }

    જો (return_now) {
        setConsumed(sc, ગાંઠ_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    જો (sc->scTcb == ખાલી) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    જો (tcb == ગાંઠ_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (tcb->tcbPriority > ગાંઠ_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  ગાંઠ_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    પ્રતિજ્ઞા(ગાંઠ_STATE(ksCurThread)->tcbYieldTo == ખાલી);
    જો (ગાંઠ_STATE(ksCurThread)->tcbYieldTo != ખાલી) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    પસંદગી (label) {
    સ્થિતિ SchedContextConsumed:
        /* no decode */
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeSchedContext_Consumed(sc, call);
    સ્થિતિ SchedContextBind:
        પાછા decodeSchedContext_Bind(sc);
    સ્થિતિ SchedContextUnbindObject:
        પાછા decodeSchedContext_UnbindObject(sc);
    સ્થિતિ SchedContextUnbind:
        /* no decode */
        જો (sc->scTcb == ગાંઠ_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeSchedContext_Unbind(sc);
    સ્થિતિ SchedContextYieldTo:
        પાછા decodeSchedContext_YieldTo(sc, call);
    મૂળભૂત:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}

શૂન્ય schedContext_resume(sched_context_t *sc)
{
    પ્રતિજ્ઞા(!sc || sc->scTcb != ખાલી);
    જો (likely(sc) && isSchedulable(sc->scTcb)) {
        જો (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            પ્રતિજ્ઞા(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

શૂન્ય schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    પ્રતિજ્ઞા(sc->scTcb == ખાલી);
    પ્રતિજ્ઞા(tcb->tcbSchedContext == ખાલી);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    જો (sc_sporadic(sc) && sc_active(sc) && sc != ગાંઠ_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    જો (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

શૂન્ય schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    પ્રતિજ્ઞા(tcb != ખાલી);

    /* tcb must already be stalled at this point */
    જો (tcb == ગાંઠ_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = ખાલી;
    sc->scTcb = ખાલી;
}

શૂન્ય schedContext_unbindAllTCBs(sched_context_t *sc)
{
    જો (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

શૂન્ય schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    પ્રતિજ્ઞા(sc != ખાલી);
    પ્રતિજ્ઞા(to != ખાલી);
    પ્રતિજ્ઞા(to->tcbSchedContext == ખાલી);

    tcb_t *from = sc->scTcb;
    જો (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = ખાલી;
        જો (from == ગાંઠ_STATE(ksCurThread) || from == ગાંઠ_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

શૂન્ય schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

શૂન્ય schedContext_unbindNtfn(sched_context_t *sc)
{
    જો (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = ખાલી;
    }
}

શૂન્ય schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    જો (sc) {
        schedContext_unbindNtfn(sc);
    }
}

શૂન્ય schedContext_unbindReply(sched_context_t *sc)
{
    જો (sc && sc->scReply) {
        પ્રતિજ્ઞા(call_ઢગલો_get_is_Head(sc->scReply->replyNext));
        sc->scReply->replyNext = call_ઢગલો_new(0, false);
        sc->scReply = ખાલી;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    જો (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        પાછા ticksToUs(getMaxTicksToUs());
    } નહીંતર {
        sc->scConsumed = 0;
        પાછા ticksToUs(consumed);
    }
}

શૂન્ય schedContext_cancelYieldTo(tcb_t *tcb)
{
    જો (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = ખાલી;
        tcb->tcbYieldTo = ખાલી;
    }
}

શૂન્ય schedContext_completeYieldTo(tcb_t *yielder)
{
    જો (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
