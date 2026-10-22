/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <machine/timer.h>
#समावेश <kernel/sporadic.h>
#समावेश <kernel/thread.h>
#समावेश <object/structures.h>
#समावेश <object/schedcontext.h>

अटल exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_thread_cap:
        schedContext_unbindTCB(sc);
        अवरोध;
    हाल cap_notification_cap:
        schedContext_unbindNtfn(sc);
        अवरोध;
    शेष:
        fail("invalid cap type");
    }

    वापस EXCEPTION_NONE;
}

अटल exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    अगर (current_extra_caps.excaprefs[0] == शून्य) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    चयन (cap_get_capType(cap)) {
    हाल cap_thread_cap:
        अगर (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अगर (sc->scTcb == गाँठ_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अवरोध;
    हाल cap_notification_cap:
        अगर (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अवरोध;

    शेष:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeSchedContext_UnbindObject(sc, cap);
}

अटल exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        अवरोध;
    हाल cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        अवरोध;
    शेष:
        fail("invalid cap type");
    }

    वापस EXCEPTION_NONE;
}

अटल exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    अगर (current_extra_caps.excaprefs[0] == शून्य) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    चयन (cap_get_capType(cap)) {
    हाल cap_thread_cap:
        अगर (sc->scTcb != शून्य) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != शून्य) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अवरोध;
    हाल cap_notification_cap:
        अगर (sc->scNotification != शून्य) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अवरोध;
    शेष:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeSchedContext_Bind(sc, cap);
}

अटल exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    वापस EXCEPTION_NONE;
}

#अगर_घोषित ENABLE_SMP_SUPPORT
अटल अंतरभूत व्योम maybeStallSC(sched_context_t *sc)
{
    अगर (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#पूर्ण_अगर

अटल अंतरभूत व्योम replyFromKernel_consumed(tcb_t *thread, समय_प्रकार consumed)
{
    word_t *बफर = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t लंबाई = mode_setTimeArg(0, consumed, बफर, thread);
    setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, लंबाई)));
}

अटल अंतरभूत व्योम setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_संदेश)
{
    समय_प्रकार consumed = schedContext_updateConsumed(sc);

    अगर (write_संदेश) {
        replyFromKernel_consumed(thread, consumed);
    }
}

अटल exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, गाँठ_STATE(ksCurThread), call);

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
    वापस EXCEPTION_NONE;
}

अटल exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    अगर (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        निश्चित(sc->scYieldFrom == शून्य);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    अगर (isSchedulable(tcb)) {
        अगर (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < गाँठ_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } अन्यथा {
            गाँठ_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = गाँठ_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(गाँठ_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } अन्यथा {
        return_now = true;
    }

    अगर (return_now) {
        setConsumed(sc, गाँठ_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
    }

    वापस EXCEPTION_NONE;
}

अटल exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    अगर (sc->scTcb == शून्य) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    अगर (tcb == गाँठ_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (tcb->tcbPriority > गाँठ_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  गाँठ_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    निश्चित(गाँठ_STATE(ksCurThread)->tcbYieldTo == शून्य);
    अगर (गाँठ_STATE(ksCurThread)->tcbYieldTo != शून्य) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    चयन (label) {
    हाल SchedContextConsumed:
        /* no decode */
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeSchedContext_Consumed(sc, call);
    हाल SchedContextBind:
        वापस decodeSchedContext_Bind(sc);
    हाल SchedContextUnbindObject:
        वापस decodeSchedContext_UnbindObject(sc);
    हाल SchedContextUnbind:
        /* no decode */
        अगर (sc->scTcb == गाँठ_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeSchedContext_Unbind(sc);
    हाल SchedContextYieldTo:
        वापस decodeSchedContext_YieldTo(sc, call);
    शेष:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
}

व्योम schedContext_resume(sched_context_t *sc)
{
    निश्चित(!sc || sc->scTcb != शून्य);
    अगर (likely(sc) && isSchedulable(sc->scTcb)) {
        अगर (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            निश्चित(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

व्योम schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    निश्चित(sc->scTcb == शून्य);
    निश्चित(tcb->tcbSchedContext == शून्य);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    अगर (sc_sporadic(sc) && sc_active(sc) && sc != गाँठ_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    अगर (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

व्योम schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    निश्चित(tcb != शून्य);

    /* tcb must already be stalled at this point */
    अगर (tcb == गाँठ_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = शून्य;
    sc->scTcb = शून्य;
}

व्योम schedContext_unbindAllTCBs(sched_context_t *sc)
{
    अगर (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

व्योम schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    निश्चित(sc != शून्य);
    निश्चित(to != शून्य);
    निश्चित(to->tcbSchedContext == शून्य);

    tcb_t *from = sc->scTcb;
    अगर (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = शून्य;
        अगर (from == गाँठ_STATE(ksCurThread) || from == गाँठ_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

व्योम schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

व्योम schedContext_unbindNtfn(sched_context_t *sc)
{
    अगर (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = शून्य;
    }
}

व्योम schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    अगर (sc) {
        schedContext_unbindNtfn(sc);
    }
}

व्योम schedContext_unbindReply(sched_context_t *sc)
{
    अगर (sc && sc->scReply) {
        निश्चित(call_ढेर_get_is_शीर्ष(sc->scReply->reply_अग्र));
        sc->scReply->reply_अग्र = call_ढेर_new(0, false);
        sc->scReply = शून्य;
    }
}

समय_प्रकार schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    अगर (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        वापस ticksToUs(getMaxTicksToUs());
    } अन्यथा {
        sc->scConsumed = 0;
        वापस ticksToUs(consumed);
    }
}

व्योम schedContext_cancelYieldTo(tcb_t *tcb)
{
    अगर (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = शून्य;
        tcb->tcbYieldTo = शून्य;
    }
}

व्योम schedContext_completeYieldTo(tcb_t *yielder)
{
    अगर (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
