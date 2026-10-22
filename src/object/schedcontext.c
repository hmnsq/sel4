/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <machine/timer.h>
#अन्तर्भाव <kernel/sporadic.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <object/schedcontext.h>

स्थिर exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_thread_cap:
        schedContext_unbindTCB(sc);
        भङ्ग;
    पक्ष cap_notification_cap:
        schedContext_unbindNtfn(sc);
        भङ्ग;
    स्वभाव:
        fail("invalid cap type");
    }

    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    सचे (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_thread_cap:
        सचे (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        सचे (sc->scTcb == ग्रन्थिः_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        भङ्ग;
    पक्ष cap_notification_cap:
        सचे (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        भङ्ग;

    स्वभाव:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeSchedContext_UnbindObject(sc, cap);
}

स्थिर exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        भङ्ग;
    पक्ष cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        भङ्ग;
    स्वभाव:
        fail("invalid cap type");
    }

    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    सचे (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_thread_cap:
        सचे (sc->scTcb != रिक्त) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }

        सचे (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != रिक्त) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }

        सचे (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }

        भङ्ग;
    पक्ष cap_notification_cap:
        सचे (sc->scNotification != रिक्त) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }

        सचे (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        भङ्ग;
    स्वभाव:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeSchedContext_Bind(sc, cap);
}

स्थिर exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    पटिनिवत्त EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
स्थिर अन्तःस्थ शून्य maybeStallSC(sched_context_t *sc)
{
    सचे (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

स्थिर अन्तःस्थ शून्य replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, सन्देशः_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

स्थिर अन्तःस्थ शून्य setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_सन्देशः)
{
    time_t consumed = schedContext_updateConsumed(sc);

    सचे (write_सन्देशः) {
        replyFromKernel_consumed(thread, consumed);
    }
}

स्थिर exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, ग्रन्थिः_STATE(ksCurThread), call);

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    सचे (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        प्रतिज्ञा(sc->scYieldFrom == रिक्त);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    सचे (isSchedulable(tcb)) {
        सचे (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < ग्रन्थिः_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } अञ्ञथा {
            ग्रन्थिः_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = ग्रन्थिः_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(ग्रन्थिः_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } अञ्ञथा {
        return_now = true;
    }

    सचे (return_now) {
        setConsumed(sc, ग्रन्थिः_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    }

    पटिनिवत्त EXCEPTION_NONE;
}

स्थिर exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    सचे (sc->scTcb == रिक्त) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    सचे (tcb == ग्रन्थिः_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    सचे (tcb->tcbPriority > ग्रन्थिः_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  ग्रन्थिः_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    प्रतिज्ञा(ग्रन्थिः_STATE(ksCurThread)->tcbYieldTo == रिक्त);
    सचे (ग्रन्थिः_STATE(ksCurThread)->tcbYieldTo != रिक्त) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    विकल्प (label) {
    पक्ष SchedContextConsumed:
        /* no decode */
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        पटिनिवत्त invokeSchedContext_Consumed(sc, call);
    पक्ष SchedContextBind:
        पटिनिवत्त decodeSchedContext_Bind(sc);
    पक्ष SchedContextUnbindObject:
        पटिनिवत्त decodeSchedContext_UnbindObject(sc);
    पक्ष SchedContextUnbind:
        /* no decode */
        सचे (sc->scTcb == ग्रन्थिः_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        पटिनिवत्त invokeSchedContext_Unbind(sc);
    पक्ष SchedContextYieldTo:
        पटिनिवत्त decodeSchedContext_YieldTo(sc, call);
    स्वभाव:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
}

शून्य schedContext_resume(sched_context_t *sc)
{
    प्रतिज्ञा(!sc || sc->scTcb != रिक्त);
    सचे (likely(sc) && isSchedulable(sc->scTcb)) {
        सचे (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            प्रतिज्ञा(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

शून्य schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    प्रतिज्ञा(sc->scTcb == रिक्त);
    प्रतिज्ञा(tcb->tcbSchedContext == रिक्त);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    सचे (sc_sporadic(sc) && sc_active(sc) && sc != ग्रन्थिः_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    सचे (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

शून्य schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    प्रतिज्ञा(tcb != रिक्त);

    /* tcb must already be stalled at this point */
    सचे (tcb == ग्रन्थिः_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = रिक्त;
    sc->scTcb = रिक्त;
}

शून्य schedContext_unbindAllTCBs(sched_context_t *sc)
{
    सचे (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

शून्य schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    प्रतिज्ञा(sc != रिक्त);
    प्रतिज्ञा(to != रिक्त);
    प्रतिज्ञा(to->tcbSchedContext == रिक्त);

    tcb_t *from = sc->scTcb;
    सचे (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = रिक्त;
        सचे (from == ग्रन्थिः_STATE(ksCurThread) || from == ग्रन्थिः_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

शून्य schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

शून्य schedContext_unbindNtfn(sched_context_t *sc)
{
    सचे (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = रिक्त;
    }
}

शून्य schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    सचे (sc) {
        schedContext_unbindNtfn(sc);
    }
}

शून्य schedContext_unbindReply(sched_context_t *sc)
{
    सचे (sc && sc->scReply) {
        प्रतिज्ञा(call_राशिः_get_is_Head(sc->scReply->replyNext));
        sc->scReply->replyNext = call_राशिः_new(0, false);
        sc->scReply = रिक्त;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    सचे (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        पटिनिवत्त ticksToUs(getMaxTicksToUs());
    } अञ्ञथा {
        sc->scConsumed = 0;
        पटिनिवत्त ticksToUs(consumed);
    }
}

शून्य schedContext_cancelYieldTo(tcb_t *tcb)
{
    सचे (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = रिक्त;
        tcb->tcbYieldTo = रिक्त;
    }
}

शून्य schedContext_completeYieldTo(tcb_t *yielder)
{
    सचे (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
