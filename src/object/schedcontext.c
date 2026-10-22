/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <machine/timer.h>
#അന്തര്ഭാവ <kernel/sporadic.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <object/schedcontext.h>

സ്ഥിതം exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_thread_cap:
        schedContext_unbindTCB(sc);
        നിർത്തുക;
    അവസ്ഥ cap_notification_cap:
        schedContext_unbindNtfn(sc);
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        fail("invalid cap type");
    }

    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    എങ്കിൽ (current_extra_caps.excaprefs[0] == ശൂന്യ) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_thread_cap:
        എങ്കിൽ (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        എങ്കിൽ (sc->scTcb == കണ്ണി_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        നിർത്തുക;
    അവസ്ഥ cap_notification_cap:
        എങ്കിൽ (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeSchedContext_UnbindObject(sc, cap);
}

സ്ഥിതം exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        നിർത്തുക;
    അവസ്ഥ cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        fail("invalid cap type");
    }

    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    എങ്കിൽ (current_extra_caps.excaprefs[0] == ശൂന്യ) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_thread_cap:
        എങ്കിൽ (sc->scTcb != ശൂന്യ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != ശൂന്യ) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        നിർത്തുക;
    അവസ്ഥ cap_notification_cap:
        എങ്കിൽ (sc->scNotification != ശൂന്യ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeSchedContext_Bind(sc, cap);
}

സ്ഥിതം exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    തിരികെ EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
സ്ഥിതം വരിയിൽ ശൂന്യം maybeStallSC(sched_context_t *sc)
{
    എങ്കിൽ (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

സ്ഥിതം വരിയിൽ ശൂന്യം replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, സന്ദേശം_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

സ്ഥിതം വരിയിൽ ശൂന്യം setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_സന്ദേശം)
{
    time_t consumed = schedContext_updateConsumed(sc);

    എങ്കിൽ (write_സന്ദേശം) {
        replyFromKernel_consumed(thread, consumed);
    }
}

സ്ഥിതം exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, കണ്ണി_STATE(ksCurThread), call);

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Running);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    എങ്കിൽ (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        പ്രതിജ്ഞാ(sc->scYieldFrom == ശൂന്യ);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    എങ്കിൽ (isSchedulable(tcb)) {
        എങ്കിൽ (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < കണ്ണി_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } അല്ലെങ്കിൽ {
            കണ്ണി_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = കണ്ണി_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(കണ്ണി_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } അല്ലെങ്കിൽ {
        return_now = true;
    }

    എങ്കിൽ (return_now) {
        setConsumed(sc, കണ്ണി_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Running);
    }

    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    എങ്കിൽ (sc->scTcb == ശൂന്യ) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    എങ്കിൽ (tcb == കണ്ണി_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    എങ്കിൽ (tcb->tcbPriority > കണ്ണി_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  കണ്ണി_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    പ്രതിജ്ഞാ(കണ്ണി_STATE(ksCurThread)->tcbYieldTo == ശൂന്യ);
    എങ്കിൽ (കണ്ണി_STATE(ksCurThread)->tcbYieldTo != ശൂന്യ) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    തിരഞ്ഞെടുപ്പ് (label) {
    അവസ്ഥ SchedContextConsumed:
        /* no decode */
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ invokeSchedContext_Consumed(sc, call);
    അവസ്ഥ SchedContextBind:
        തിരികെ decodeSchedContext_Bind(sc);
    അവസ്ഥ SchedContextUnbindObject:
        തിരികെ decodeSchedContext_UnbindObject(sc);
    അവസ്ഥ SchedContextUnbind:
        /* no decode */
        എങ്കിൽ (sc->scTcb == കണ്ണി_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ invokeSchedContext_Unbind(sc);
    അവസ്ഥ SchedContextYieldTo:
        തിരികെ decodeSchedContext_YieldTo(sc, call);
    സ്ഥിരസ്ഥിതി:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
}

ശൂന്യം schedContext_resume(sched_context_t *sc)
{
    പ്രതിജ്ഞാ(!sc || sc->scTcb != ശൂന്യ);
    എങ്കിൽ (likely(sc) && isSchedulable(sc->scTcb)) {
        എങ്കിൽ (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            പ്രതിജ്ഞാ(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

ശൂന്യം schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    പ്രതിജ്ഞാ(sc->scTcb == ശൂന്യ);
    പ്രതിജ്ഞാ(tcb->tcbSchedContext == ശൂന്യ);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    എങ്കിൽ (sc_sporadic(sc) && sc_active(sc) && sc != കണ്ണി_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    എങ്കിൽ (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

ശൂന്യം schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    പ്രതിജ്ഞാ(tcb != ശൂന്യ);

    /* tcb must already be stalled at this point */
    എങ്കിൽ (tcb == കണ്ണി_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = ശൂന്യ;
    sc->scTcb = ശൂന്യ;
}

ശൂന്യം schedContext_unbindAllTCBs(sched_context_t *sc)
{
    എങ്കിൽ (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

ശൂന്യം schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    പ്രതിജ്ഞാ(sc != ശൂന്യ);
    പ്രതിജ്ഞാ(to != ശൂന്യ);
    പ്രതിജ്ഞാ(to->tcbSchedContext == ശൂന്യ);

    tcb_t *from = sc->scTcb;
    എങ്കിൽ (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = ശൂന്യ;
        എങ്കിൽ (from == കണ്ണി_STATE(ksCurThread) || from == കണ്ണി_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

ശൂന്യം schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

ശൂന്യം schedContext_unbindNtfn(sched_context_t *sc)
{
    എങ്കിൽ (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = ശൂന്യ;
    }
}

ശൂന്യം schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    എങ്കിൽ (sc) {
        schedContext_unbindNtfn(sc);
    }
}

ശൂന്യം schedContext_unbindReply(sched_context_t *sc)
{
    എങ്കിൽ (sc && sc->scReply) {
        പ്രതിജ്ഞാ(call_അട്ടി_get_is_Head(sc->scReply->replyNext));
        sc->scReply->replyNext = call_അട്ടി_new(0, false);
        sc->scReply = ശൂന്യ;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    എങ്കിൽ (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        തിരികെ ticksToUs(getMaxTicksToUs());
    } അല്ലെങ്കിൽ {
        sc->scConsumed = 0;
        തിരികെ ticksToUs(consumed);
    }
}

ശൂന്യം schedContext_cancelYieldTo(tcb_t *tcb)
{
    എങ്കിൽ (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = ശൂന്യ;
        tcb->tcbYieldTo = ശൂന്യ;
    }
}

ശൂന്യം schedContext_completeYieldTo(tcb_t *yielder)
{
    എങ്കിൽ (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
