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

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    यदि (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_thread_cap:
        यदि (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        यदि (sc->scTcb == गाठ_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        भङ्ग;
    पक्ष cap_notification_cap:
        यदि (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        भङ्ग;

    स्वभाव:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeSchedContext_UnbindObject(sc, cap);
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

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    यदि (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_thread_cap:
        यदि (sc->scTcb != रिक्त) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != रिक्त) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        भङ्ग;
    पक्ष cap_notification_cap:
        यदि (sc->scNotification != रिक्त) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        भङ्ग;
    स्वभाव:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeSchedContext_Bind(sc, cap);
}

स्थिर exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    प्रत्यागम EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
स्थिर अन्तःस्थ शून्य maybeStallSC(sched_context_t *sc)
{
    यदि (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

स्थिर अन्तःस्थ शून्य replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

स्थिर अन्तःस्थ शून्य setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_संदेश)
{
    time_t consumed = schedContext_updateConsumed(sc);

    यदि (write_संदेश) {
        replyFromKernel_consumed(thread, consumed);
    }
}

स्थिर exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, गाठ_STATE(ksCurThread), call);

    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    यदि (sc->scYieldFrom) {
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
    यदि (isSchedulable(tcb)) {
        यदि (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < गाठ_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } अन्यथा {
            गाठ_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = गाठ_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(गाठ_STATE(ksCurThread));
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

    यदि (return_now) {
        setConsumed(sc, गाठ_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Running);
    }

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    यदि (sc->scTcb == रिक्त) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    यदि (tcb == गाठ_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (tcb->tcbPriority > गाठ_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  गाठ_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    प्रतिज्ञा(गाठ_STATE(ksCurThread)->tcbYieldTo == रिक्त);
    यदि (गाठ_STATE(ksCurThread)->tcbYieldTo != रिक्त) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    विकल्प (label) {
    पक्ष SchedContextConsumed:
        /* no decode */
        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeSchedContext_Consumed(sc, call);
    पक्ष SchedContextBind:
        प्रत्यागम decodeSchedContext_Bind(sc);
    पक्ष SchedContextUnbindObject:
        प्रत्यागम decodeSchedContext_UnbindObject(sc);
    पक्ष SchedContextUnbind:
        /* no decode */
        यदि (sc->scTcb == गाठ_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeSchedContext_Unbind(sc);
    पक्ष SchedContextYieldTo:
        प्रत्यागम decodeSchedContext_YieldTo(sc, call);
    स्वभाव:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

शून्य schedContext_resume(sched_context_t *sc)
{
    प्रतिज्ञा(!sc || sc->scTcb != रिक्त);
    यदि (likely(sc) && isSchedulable(sc->scTcb)) {
        यदि (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
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

    यदि (sc_sporadic(sc) && sc_active(sc) && sc != गाठ_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    यदि (isSchedulable(tcb)) {
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
    यदि (tcb == गाठ_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = रिक्त;
    sc->scTcb = रिक्त;
}

शून्य schedContext_unbindAllTCBs(sched_context_t *sc)
{
    यदि (sc->scTcb) {
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
    यदि (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = रिक्त;
        यदि (from == गाठ_STATE(ksCurThread) || from == गाठ_STATE(ksSchedulerAction)) {
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
    यदि (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = रिक्त;
    }
}

शून्य schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    यदि (sc) {
        schedContext_unbindNtfn(sc);
    }
}

शून्य schedContext_unbindReply(sched_context_t *sc)
{
    यदि (sc && sc->scReply) {
        प्रतिज्ञा(call_चळत_get_is_Head(sc->scReply->replyNext));
        sc->scReply->replyNext = call_चळत_new(0, false);
        sc->scReply = रिक्त;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    यदि (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        प्रत्यागम ticksToUs(getMaxTicksToUs());
    } अन्यथा {
        sc->scConsumed = 0;
        प्रत्यागम ticksToUs(consumed);
    }
}

शून्य schedContext_cancelYieldTo(tcb_t *tcb)
{
    यदि (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = रिक्त;
        tcb->tcbYieldTo = रिक्त;
    }
}

शून्य schedContext_completeYieldTo(tcb_t *yielder)
{
    यदि (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
