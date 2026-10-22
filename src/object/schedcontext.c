/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <machine/timer.h>
#شامل <kernel/sporadic.h>
#شامل <kernel/thread.h>
#شامل <object/structures.h>
#شامل <object/schedcontext.h>

ایستا exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_thread_cap:
        schedContext_unbindTCB(sc);
        بشکن;
    حالت cap_notification_cap:
        schedContext_unbindNtfn(sc);
        بشکن;
    پیشفرض:
        fail("invalid cap type");
    }

    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    اگر (current_extra_caps.excaprefs[0] == پوچ) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    انتخاب (cap_get_capType(cap)) {
    حالت cap_thread_cap:
        اگر (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        اگر (sc->scTcb == گره_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        بشکن;
    حالت cap_notification_cap:
        اگر (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        بشکن;

    پیشفرض:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeSchedContext_UnbindObject(sc, cap);
}

ایستا exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        بشکن;
    حالت cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        بشکن;
    پیشفرض:
        fail("invalid cap type");
    }

    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    اگر (current_extra_caps.excaprefs[0] == پوچ) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_thread_cap:
        اگر (sc->scTcb != پوچ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        اگر (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != پوچ) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        اگر (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        بشکن;
    حالت cap_notification_cap:
        اگر (sc->scNotification != پوچ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        اگر (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        بشکن;
    پیشفرض:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeSchedContext_Bind(sc, cap);
}

ایستا exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    بازگشت EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
ایستا درونخطی تهی maybeStallSC(sched_context_t *sc)
{
    اگر (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

ایستا درونخطی تهی replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, پیام_Info_Register, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

ایستا درونخطی تهی setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_پیام)
{
    time_t consumed = schedContext_updateConsumed(sc);

    اگر (write_پیام) {
        replyFromKernel_consumed(thread, consumed);
    }
}

ایستا exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, گره_STATE(ksCurThread), call);

    setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    اگر (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        أكد(sc->scYieldFrom == پوچ);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    اگر (isSchedulable(tcb)) {
        اگر (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < گره_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } وگرنه {
            گره_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = گره_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(گره_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } وگرنه {
        return_now = true;
    }

    اگر (return_now) {
        setConsumed(sc, گره_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
    }

    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    اگر (sc->scTcb == پوچ) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    اگر (tcb == گره_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (tcb->tcbPriority > گره_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  گره_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    أكد(گره_STATE(ksCurThread)->tcbYieldTo == پوچ);
    اگر (گره_STATE(ksCurThread)->tcbYieldTo != پوچ) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    انتخاب (label) {
    حالت SchedContextConsumed:
        /* no decode */
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeSchedContext_Consumed(sc, call);
    حالت SchedContextBind:
        بازگشت decodeSchedContext_Bind(sc);
    حالت SchedContextUnbindObject:
        بازگشت decodeSchedContext_UnbindObject(sc);
    حالت SchedContextUnbind:
        /* no decode */
        اگر (sc->scTcb == گره_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeSchedContext_Unbind(sc);
    حالت SchedContextYieldTo:
        بازگشت decodeSchedContext_YieldTo(sc, call);
    پیشفرض:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}

تهی schedContext_resume(sched_context_t *sc)
{
    أكد(!sc || sc->scTcb != پوچ);
    اگر (likely(sc) && isSchedulable(sc->scTcb)) {
        اگر (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            أكد(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

تهی schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    أكد(sc->scTcb == پوچ);
    أكد(tcb->tcbSchedContext == پوچ);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    اگر (sc_sporadic(sc) && sc_active(sc) && sc != گره_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    اگر (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

تهی schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    أكد(tcb != پوچ);

    /* tcb must already be stalled at this point */
    اگر (tcb == گره_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = پوچ;
    sc->scTcb = پوچ;
}

تهی schedContext_unbindAllTCBs(sched_context_t *sc)
{
    اگر (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

تهی schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    أكد(sc != پوچ);
    أكد(to != پوچ);
    أكد(to->tcbSchedContext == پوچ);

    tcb_t *from = sc->scTcb;
    اگر (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = پوچ;
        اگر (from == گره_STATE(ksCurThread) || from == گره_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

تهی schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

تهی schedContext_unbindNtfn(sched_context_t *sc)
{
    اگر (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = پوچ;
    }
}

تهی schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    اگر (sc) {
        schedContext_unbindNtfn(sc);
    }
}

تهی schedContext_unbindReply(sched_context_t *sc)
{
    اگر (sc && sc->scReply) {
        أكد(call_پشته_get_is_Head(sc->scReply->replyNext));
        sc->scReply->replyNext = call_پشته_new(0, false);
        sc->scReply = پوچ;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    اگر (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        بازگشت ticksToUs(getMaxTicksToUs());
    } وگرنه {
        sc->scConsumed = 0;
        بازگشت ticksToUs(consumed);
    }
}

تهی schedContext_cancelYieldTo(tcb_t *tcb)
{
    اگر (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = پوچ;
        tcb->tcbYieldTo = پوچ;
    }
}

تهی schedContext_completeYieldTo(tcb_t *yielder)
{
    اگر (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
