/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <machine/timer.h>
#ଅନ୍ତର୍ଭାଵ <kernel/sporadic.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <object/schedcontext.h>

ସ୍ଥିର exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_thread_cap:
        schedContext_unbindTCB(sc);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_notification_cap:
        schedContext_unbindNtfn(sc);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("invalid cap type");
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    ଯଦି (current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_thread_cap:
        ଯଦି (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ଯଦି (sc->scTcb == ଗଣ୍ଠି_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_notification_cap:
        ଯଦି (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeSchedContext_UnbindObject(sc, cap);
}

ସ୍ଥିର exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("invalid cap type");
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    ଯଦି (current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_thread_cap:
        ଯଦି (sc->scTcb != ଖାଲି) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ଯଦି (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != ଖାଲି) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ଯଦି (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_notification_cap:
        ଯଦି (sc->scNotification != ଖାଲି) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ଯଦି (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeSchedContext_Bind(sc, cap);
}

ସ୍ଥିର exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ maybeStallSC(sched_context_t *sc)
{
    ଯଦି (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_ସନ୍ଦେଶ)
{
    time_t consumed = schedContext_updateConsumed(sc);

    ଯଦି (write_ସନ୍ଦେଶ) {
        replyFromKernel_consumed(thread, consumed);
    }
}

ସ୍ଥିର exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, ଗଣ୍ଠି_STATE(ksCurThread), call);

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Running);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    ଯଦି (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        ପ୍ରତିଜ୍ଞା(sc->scYieldFrom == ଖାଲି);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    ଯଦି (isSchedulable(tcb)) {
        ଯଦି (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < ଗଣ୍ଠି_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } ଅନ୍ୟଥା {
            ଗଣ୍ଠି_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = ଗଣ୍ଠି_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(ଗଣ୍ଠି_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } ଅନ୍ୟଥା {
        return_now = true;
    }

    ଯଦି (return_now) {
        setConsumed(sc, ଗଣ୍ଠି_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Running);
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    ଯଦି (sc->scTcb == ଖାଲି) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    ଯଦି (tcb == ଗଣ୍ଠି_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (tcb->tcbPriority > ଗଣ୍ଠି_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  ଗଣ୍ଠି_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    ପ୍ରତିଜ୍ଞା(ଗଣ୍ଠି_STATE(ksCurThread)->tcbYieldTo == ଖାଲି);
    ଯଦି (ଗଣ୍ଠି_STATE(ksCurThread)->tcbYieldTo != ଖାଲି) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    ବାଛ (label) {
    ସ୍ଥିତି SchedContextConsumed:
        /* no decode */
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeSchedContext_Consumed(sc, call);
    ସ୍ଥିତି SchedContextBind:
        ଫେରନ୍ତୁ decodeSchedContext_Bind(sc);
    ସ୍ଥିତି SchedContextUnbindObject:
        ଫେରନ୍ତୁ decodeSchedContext_UnbindObject(sc);
    ସ୍ଥିତି SchedContextUnbind:
        /* no decode */
        ଯଦି (sc->scTcb == ଗଣ୍ଠି_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeSchedContext_Unbind(sc);
    ସ୍ଥିତି SchedContextYieldTo:
        ଫେରନ୍ତୁ decodeSchedContext_YieldTo(sc, call);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}

ଶୂନ୍ୟ schedContext_resume(sched_context_t *sc)
{
    ପ୍ରତିଜ୍ଞା(!sc || sc->scTcb != ଖାଲି);
    ଯଦି (likely(sc) && isSchedulable(sc->scTcb)) {
        ଯଦି (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            ପ୍ରତିଜ୍ଞା(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

ଶୂନ୍ୟ schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    ପ୍ରତିଜ୍ଞା(sc->scTcb == ଖାଲି);
    ପ୍ରତିଜ୍ଞା(tcb->tcbSchedContext == ଖାଲି);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    ଯଦି (sc_sporadic(sc) && sc_active(sc) && sc != ଗଣ୍ଠି_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    ଯଦି (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

ଶୂନ୍ୟ schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ପ୍ରତିଜ୍ଞା(tcb != ଖାଲି);

    /* tcb must already be stalled at this point */
    ଯଦି (tcb == ଗଣ୍ଠି_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = ଖାଲି;
    sc->scTcb = ଖାଲି;
}

ଶୂନ୍ୟ schedContext_unbindAllTCBs(sched_context_t *sc)
{
    ଯଦି (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

ଶୂନ୍ୟ schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    ପ୍ରତିଜ୍ଞା(sc != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(to != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(to->tcbSchedContext == ଖାଲି);

    tcb_t *from = sc->scTcb;
    ଯଦି (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = ଖାଲି;
        ଯଦି (from == ଗଣ୍ଠି_STATE(ksCurThread) || from == ଗଣ୍ଠି_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

ଶୂନ୍ୟ schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

ଶୂନ୍ୟ schedContext_unbindNtfn(sched_context_t *sc)
{
    ଯଦି (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = ଖାଲି;
    }
}

ଶୂନ୍ୟ schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    ଯଦି (sc) {
        schedContext_unbindNtfn(sc);
    }
}

ଶୂନ୍ୟ schedContext_unbindReply(sched_context_t *sc)
{
    ଯଦି (sc && sc->scReply) {
        ପ୍ରତିଜ୍ଞା(call_ଗଦା_get_is_Head(sc->scReply->replyNext));
        sc->scReply->replyNext = call_ଗଦା_new(0, false);
        sc->scReply = ଖାଲି;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    ଯଦି (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        ଫେରନ୍ତୁ ticksToUs(getMaxTicksToUs());
    } ଅନ୍ୟଥା {
        sc->scConsumed = 0;
        ଫେରନ୍ତୁ ticksToUs(consumed);
    }
}

ଶୂନ୍ୟ schedContext_cancelYieldTo(tcb_t *tcb)
{
    ଯଦି (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = ଖାଲି;
        tcb->tcbYieldTo = ଖାଲି;
    }
}

ଶୂନ୍ୟ schedContext_completeYieldTo(tcb_t *yielder)
{
    ଯଦି (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
