/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <machine/timer.h>
#ضمن <kernel/sporadic.h>
#ضمن <kernel/thread.h>
#ضمن <object/structures.h>
#ضمن <object/schedcontext.h>

ساكن exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_thread_cap:
        schedContext_unbindTCB(sc);
        اكسر;
    حالة cap_notification_cap:
        schedContext_unbindNtfn(sc);
        اكسر;
    افتراضي:
        fail("invalid cap type");
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    بدل (cap_get_capType(cap)) {
    حالة cap_thread_cap:
        إذا (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (sc->scTcb == عقدة_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        اكسر;
    حالة cap_notification_cap:
        إذا (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        اكسر;

    افتراضي:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSchedContext_UnbindObject(sc, cap);
}

ساكن exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        اكسر;
    حالة cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        اكسر;
    افتراضي:
        fail("invalid cap type");
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    بدل (cap_get_capType(cap)) {
    حالة cap_thread_cap:
        إذا (sc->scTcb != عدم) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != عدم) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        اكسر;
    حالة cap_notification_cap:
        إذا (sc->scNotification != عدم) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        اكسر;
    افتراضي:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSchedContext_Bind(sc, cap);
}

ساكن exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    أرجع EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
ساكن مضمن فراغ maybeStallSC(sched_context_t *sc)
{
    إذا (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

ساكن مضمن فراغ replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, رسالة_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

ساكن مضمن فراغ setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_رسالة)
{
    time_t consumed = schedContext_updateConsumed(sc);

    إذا (write_رسالة) {
        replyFromKernel_consumed(thread, consumed);
    }
}

ساكن exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, عقدة_STATE(ksCurThread), call);

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    إذا (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        أكد(sc->scYieldFrom == عدم);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    إذا (isSchedulable(tcb)) {
        إذا (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < عقدة_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } وإلا {
            عقدة_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = عقدة_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(عقدة_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } وإلا {
        return_now = true;
    }

    إذا (return_now) {
        setConsumed(sc, عقدة_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Running);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    إذا (sc->scTcb == عدم) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    إذا (tcb == عقدة_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (tcb->tcbPriority > عقدة_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  عقدة_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    أكد(عقدة_STATE(ksCurThread)->tcbYieldTo == عدم);
    إذا (عقدة_STATE(ksCurThread)->tcbYieldTo != عدم) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    بدل (label) {
    حالة SchedContextConsumed:
        /* no decode */
        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeSchedContext_Consumed(sc, call);
    حالة SchedContextBind:
        أرجع decodeSchedContext_Bind(sc);
    حالة SchedContextUnbindObject:
        أرجع decodeSchedContext_UnbindObject(sc);
    حالة SchedContextUnbind:
        /* no decode */
        إذا (sc->scTcb == عقدة_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeSchedContext_Unbind(sc);
    حالة SchedContextYieldTo:
        أرجع decodeSchedContext_YieldTo(sc, call);
    افتراضي:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

فراغ schedContext_resume(sched_context_t *sc)
{
    أكد(!sc || sc->scTcb != عدم);
    إذا (likely(sc) && isSchedulable(sc->scTcb)) {
        إذا (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            أكد(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

فراغ schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    أكد(sc->scTcb == عدم);
    أكد(tcb->tcbSchedContext == عدم);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    إذا (sc_sporadic(sc) && sc_active(sc) && sc != عقدة_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    إذا (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

فراغ schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    أكد(tcb != عدم);

    /* tcb must already be stalled at this point */
    إذا (tcb == عقدة_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = عدم;
    sc->scTcb = عدم;
}

فراغ schedContext_unbindAllTCBs(sched_context_t *sc)
{
    إذا (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

فراغ schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    أكد(sc != عدم);
    أكد(to != عدم);
    أكد(to->tcbSchedContext == عدم);

    tcb_t *from = sc->scTcb;
    إذا (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = عدم;
        إذا (from == عقدة_STATE(ksCurThread) || from == عقدة_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

فراغ schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

فراغ schedContext_unbindNtfn(sched_context_t *sc)
{
    إذا (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = عدم;
    }
}

فراغ schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    إذا (sc) {
        schedContext_unbindNtfn(sc);
    }
}

فراغ schedContext_unbindReply(sched_context_t *sc)
{
    إذا (sc && sc->scReply) {
        أكد(call_مكدس_get_is_Head(sc->scReply->replyNext));
        sc->scReply->replyNext = call_مكدس_new(0, false);
        sc->scReply = عدم;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    إذا (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        أرجع ticksToUs(getMaxTicksToUs());
    } وإلا {
        sc->scConsumed = 0;
        أرجع ticksToUs(consumed);
    }
}

فراغ schedContext_cancelYieldTo(tcb_t *tcb)
{
    إذا (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = عدم;
        tcb->tcbYieldTo = عدم;
    }
}

فراغ schedContext_completeYieldTo(tcb_t *yielder)
{
    إذا (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
