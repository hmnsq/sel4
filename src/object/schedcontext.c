/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <machine/timer.h>
#অন্তর্ভাব <kernel/sporadic.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <object/schedcontext.h>

স্থির exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_thread_cap:
        schedContext_unbindTCB(sc);
        ভঙ্গ;
    পক্ষ cap_notification_cap:
        schedContext_unbindNtfn(sc);
        ভঙ্গ;
    স্বভাব:
        fail("invalid cap type");
    }

    প্রত্যাগম EXCEPTION_NONE;
}

স্থির exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    যদি (current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_thread_cap:
        যদি (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        যদি (sc->scTcb == গ্রন্থি_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        ভঙ্গ;
    পক্ষ cap_notification_cap:
        যদি (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        ভঙ্গ;

    স্বভাব:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeSchedContext_UnbindObject(sc, cap);
}

স্থির exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        ভঙ্গ;
    পক্ষ cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        ভঙ্গ;
    স্বভাব:
        fail("invalid cap type");
    }

    প্রত্যাগম EXCEPTION_NONE;
}

স্থির exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    যদি (current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_thread_cap:
        যদি (sc->scTcb != রিক্ত) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        যদি (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != রিক্ত) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        যদি (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        ভঙ্গ;
    পক্ষ cap_notification_cap:
        যদি (sc->scNotification != রিক্ত) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        যদি (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        ভঙ্গ;
    স্বভাব:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeSchedContext_Bind(sc, cap);
}

স্থির exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    প্রত্যাগম EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
স্থির অন্তঃস্থ শূন্য maybeStallSC(sched_context_t *sc)
{
    যদি (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

স্থির অন্তঃস্থ শূন্য replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

স্থির অন্তঃস্থ শূন্য setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_বার্তা)
{
    time_t consumed = schedContext_updateConsumed(sc);

    যদি (write_বার্তা) {
        replyFromKernel_consumed(thread, consumed);
    }
}

স্থির exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, গ্রন্থি_STATE(ksCurThread), call);

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);
    প্রত্যাগম EXCEPTION_NONE;
}

স্থির exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    যদি (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        প্রতিজ্ঞা(sc->scYieldFrom == রিক্ত);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    যদি (isSchedulable(tcb)) {
        যদি (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < গ্রন্থি_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } অন্যথা {
            গ্রন্থি_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = গ্রন্থি_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(গ্রন্থি_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } অন্যথা {
        return_now = true;
    }

    যদি (return_now) {
        setConsumed(sc, গ্রন্থি_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);
    }

    প্রত্যাগম EXCEPTION_NONE;
}

স্থির exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    যদি (sc->scTcb == রিক্ত) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    যদি (tcb == গ্রন্থি_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    যদি (tcb->tcbPriority > গ্রন্থি_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  গ্রন্থি_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    প্রতিজ্ঞা(গ্রন্থি_STATE(ksCurThread)->tcbYieldTo == রিক্ত);
    যদি (গ্রন্থি_STATE(ksCurThread)->tcbYieldTo != রিক্ত) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    প্রত্যাগম invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    বিকল্প (label) {
    পক্ষ SchedContextConsumed:
        /* no decode */
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        প্রত্যাগম invokeSchedContext_Consumed(sc, call);
    পক্ষ SchedContextBind:
        প্রত্যাগম decodeSchedContext_Bind(sc);
    পক্ষ SchedContextUnbindObject:
        প্রত্যাগম decodeSchedContext_UnbindObject(sc);
    পক্ষ SchedContextUnbind:
        /* no decode */
        যদি (sc->scTcb == গ্রন্থি_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        প্রত্যাগম invokeSchedContext_Unbind(sc);
    পক্ষ SchedContextYieldTo:
        প্রত্যাগম decodeSchedContext_YieldTo(sc, call);
    স্বভাব:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
    }
}

শূন্য schedContext_resume(sched_context_t *sc)
{
    প্রতিজ্ঞা(!sc || sc->scTcb != রিক্ত);
    যদি (likely(sc) && isSchedulable(sc->scTcb)) {
        যদি (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            প্রতিজ্ঞা(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

শূন্য schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    প্রতিজ্ঞা(sc->scTcb == রিক্ত);
    প্রতিজ্ঞা(tcb->tcbSchedContext == রিক্ত);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    যদি (sc_sporadic(sc) && sc_active(sc) && sc != গ্রন্থি_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    যদি (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

শূন্য schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    প্রতিজ্ঞা(tcb != রিক্ত);

    /* tcb must already be stalled at this point */
    যদি (tcb == গ্রন্থি_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = রিক্ত;
    sc->scTcb = রিক্ত;
}

শূন্য schedContext_unbindAllTCBs(sched_context_t *sc)
{
    যদি (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

শূন্য schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    প্রতিজ্ঞা(sc != রিক্ত);
    প্রতিজ্ঞা(to != রিক্ত);
    প্রতিজ্ঞা(to->tcbSchedContext == রিক্ত);

    tcb_t *from = sc->scTcb;
    যদি (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = রিক্ত;
        যদি (from == গ্রন্থি_STATE(ksCurThread) || from == গ্রন্থি_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

শূন্য schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

শূন্য schedContext_unbindNtfn(sched_context_t *sc)
{
    যদি (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = রিক্ত;
    }
}

শূন্য schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    যদি (sc) {
        schedContext_unbindNtfn(sc);
    }
}

শূন্য schedContext_unbindReply(sched_context_t *sc)
{
    যদি (sc && sc->scReply) {
        প্রতিজ্ঞা(call_স্তূপ_get_is_Head(sc->scReply->replyNext));
        sc->scReply->replyNext = call_স্তূপ_new(0, false);
        sc->scReply = রিক্ত;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    যদি (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        প্রত্যাগম ticksToUs(getMaxTicksToUs());
    } অন্যথা {
        sc->scConsumed = 0;
        প্রত্যাগম ticksToUs(consumed);
    }
}

শূন্য schedContext_cancelYieldTo(tcb_t *tcb)
{
    যদি (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = রিক্ত;
        tcb->tcbYieldTo = রিক্ত;
    }
}

শূন্য schedContext_completeYieldTo(tcb_t *yielder)
{
    যদি (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
