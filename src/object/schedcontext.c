/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <machine/timer.h>
#அந்தர்பாவ <kernel/sporadic.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <object/schedcontext.h>

நிலையான exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_thread_cap:
        schedContext_unbindTCB(sc);
        நிறுத்து;
    நிலை cap_notification_cap:
        schedContext_unbindNtfn(sc);
        நிறுத்து;
    இயல்பு:
        fail("invalid cap type");
    }

    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    எனில் (current_extra_caps.excaprefs[0] == வெற்று) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_thread_cap:
        எனில் (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        எனில் (sc->scTcb == முனை_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        நிறுத்து;
    நிலை cap_notification_cap:
        எனில் (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        நிறுத்து;

    இயல்பு:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeSchedContext_UnbindObject(sc, cap);
}

நிலையான exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        நிறுத்து;
    நிலை cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        நிறுத்து;
    இயல்பு:
        fail("invalid cap type");
    }

    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    எனில் (current_extra_caps.excaprefs[0] == வெற்று) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_thread_cap:
        எனில் (sc->scTcb != வெற்று) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != வெற்று) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        நிறுத்து;
    நிலை cap_notification_cap:
        எனில் (sc->scNotification != வெற்று) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        நிறுத்து;
    இயல்பு:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeSchedContext_Bind(sc, cap);
}

நிலையான exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    திரும்பு EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
நிலையான உள்வரி வெறுமை maybeStallSC(sched_context_t *sc)
{
    எனில் (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

நிலையான உள்வரி வெறுமை replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

நிலையான உள்வரி வெறுமை setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_செய்தி)
{
    time_t consumed = schedContext_updateConsumed(sc);

    எனில் (write_செய்தி) {
        replyFromKernel_consumed(thread, consumed);
    }
}

நிலையான exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, முனை_STATE(ksCurThread), call);

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    எனில் (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        ப்ரதிஜ்ஞா(sc->scYieldFrom == வெற்று);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    எனில் (isSchedulable(tcb)) {
        எனில் (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < முனை_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } இல்லையெனில் {
            முனை_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = முனை_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(முனை_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } இல்லையெனில் {
        return_now = true;
    }

    எனில் (return_now) {
        setConsumed(sc, முனை_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
    }

    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    எனில் (sc->scTcb == வெற்று) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    எனில் (tcb == முனை_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (tcb->tcbPriority > முனை_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  முனை_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    ப்ரதிஜ்ஞா(முனை_STATE(ksCurThread)->tcbYieldTo == வெற்று);
    எனில் (முனை_STATE(ksCurThread)->tcbYieldTo != வெற்று) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    தேர்வு (label) {
    நிலை SchedContextConsumed:
        /* no decode */
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeSchedContext_Consumed(sc, call);
    நிலை SchedContextBind:
        திரும்பு decodeSchedContext_Bind(sc);
    நிலை SchedContextUnbindObject:
        திரும்பு decodeSchedContext_UnbindObject(sc);
    நிலை SchedContextUnbind:
        /* no decode */
        எனில் (sc->scTcb == முனை_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeSchedContext_Unbind(sc);
    நிலை SchedContextYieldTo:
        திரும்பு decodeSchedContext_YieldTo(sc, call);
    இயல்பு:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
}

வெறுமை schedContext_resume(sched_context_t *sc)
{
    ப்ரதிஜ்ஞா(!sc || sc->scTcb != வெற்று);
    எனில் (likely(sc) && isSchedulable(sc->scTcb)) {
        எனில் (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            ப்ரதிஜ்ஞா(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

வெறுமை schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    ப்ரதிஜ்ஞா(sc->scTcb == வெற்று);
    ப்ரதிஜ்ஞா(tcb->tcbSchedContext == வெற்று);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    எனில் (sc_sporadic(sc) && sc_active(sc) && sc != முனை_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    எனில் (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

வெறுமை schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ப்ரதிஜ்ஞா(tcb != வெற்று);

    /* tcb must already be stalled at this point */
    எனில் (tcb == முனை_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = வெற்று;
    sc->scTcb = வெற்று;
}

வெறுமை schedContext_unbindAllTCBs(sched_context_t *sc)
{
    எனில் (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

வெறுமை schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    ப்ரதிஜ்ஞா(sc != வெற்று);
    ப்ரதிஜ்ஞா(to != வெற்று);
    ப்ரதிஜ்ஞா(to->tcbSchedContext == வெற்று);

    tcb_t *from = sc->scTcb;
    எனில் (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = வெற்று;
        எனில் (from == முனை_STATE(ksCurThread) || from == முனை_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

வெறுமை schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

வெறுமை schedContext_unbindNtfn(sched_context_t *sc)
{
    எனில் (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = வெற்று;
    }
}

வெறுமை schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    எனில் (sc) {
        schedContext_unbindNtfn(sc);
    }
}

வெறுமை schedContext_unbindReply(sched_context_t *sc)
{
    எனில் (sc && sc->scReply) {
        ப்ரதிஜ்ஞா(call_அடுக்கு_get_is_Head(sc->scReply->replyNext));
        sc->scReply->replyNext = call_அடுக்கு_new(0, false);
        sc->scReply = வெற்று;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    எனில் (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        திரும்பு ticksToUs(getMaxTicksToUs());
    } இல்லையெனில் {
        sc->scConsumed = 0;
        திரும்பு ticksToUs(consumed);
    }
}

வெறுமை schedContext_cancelYieldTo(tcb_t *tcb)
{
    எனில் (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = வெற்று;
        tcb->tcbYieldTo = வெற்று;
    }
}

வெறுமை schedContext_completeYieldTo(tcb_t *yielder)
{
    எனில் (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
