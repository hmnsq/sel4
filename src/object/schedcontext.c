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

ساکن exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_thread_cap:
        schedContext_unbindTCB(sc);
        توڑو;
    حالت cap_notification_cap:
        schedContext_unbindNtfn(sc);
        توڑو;
    طےشدہ:
        fail("invalid cap type");
    }

    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    اگر (current_extra_caps.excaprefs[0] == تہی) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    انتخاب (cap_get_capType(cap)) {
    حالت cap_thread_cap:
        اگر (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        اگر (sc->scTcb == گرہ_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        توڑو;
    حالت cap_notification_cap:
        اگر (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        توڑو;

    طےشدہ:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;

    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeSchedContext_UnbindObject(sc, cap);
}

ساکن exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        توڑو;
    حالت cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        توڑو;
    طےشدہ:
        fail("invalid cap type");
    }

    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    اگر (current_extra_caps.excaprefs[0] == تہی) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_thread_cap:
        اگر (sc->scTcb != تہی) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != تہی) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        توڑو;
    حالت cap_notification_cap:
        اگر (sc->scNotification != تہی) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        توڑو;
    طےشدہ:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeSchedContext_Bind(sc, cap);
}

ساکن exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    sched_Context_unbind_جواب(sc);
    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
ساکن اندرونی خالی maybeStallSC(sched_context_t *sc)
{
    اگر (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#اگر_ختم

ساکن اندرونی خالی جواب_From_Kernel_consumed(tcb_t *thread, وقت_t consumed)
{
    لفظ_t *بفر = lookupIPCBuffer(true, thread);
    سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
    لفظ_t لمبائی = منوال_سیٹ_وقت_Arg(0, consumed, بفر, thread);
    سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(se_L4_پیغام_Info_new(0, 0, 0, لمبائی)));
}

ساکن اندرونی خالی سیٹ_Consumed(sched_context_t *sc, tcb_t *thread, bool_t write_پیغام)
{
    وقت_t consumed = schedContext_updateConsumed(sc);

    اگر (write_پیغام) {
        جواب_From_Kernel_consumed(thread, consumed);
    }
}

ساکن exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    سیٹ_Consumed(sc, گرہ_STATE(ksCurThread), call);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    واپس EXCEPTION_NONE;
}

ساکن exception_t invoke_Sched_Context_Yield_مقصد(sched_context_t *sc, bool_t call)
{
    اگر (sc->scYieldFrom) {
        sched_Context_complete_Yield_مقصد(sc->scYieldFrom);
        أكد(sc->scYieldFrom == تہی);
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
            tcb->tcb_ترجیح < گرہ_STATE(ksCurThread)->tcb_ترجیح) {
            tcb_Sched_قطار_سے_نکالو(tcb);
            SCHED_قطار_میں_ڈالو(tcb);
            return_now = true;
        } ورنہ {
            گرہ_STATE(ksCurThread)->tcb_Yield_مقصد = sc;
            sc->scYieldFrom = گرہ_STATE(ksCurThread);
            tcb_Sched_قطار_سے_نکالو(tcb);
            tcb_Sched_قطار_میں_ڈالو(گرہ_STATE(ksCurThread));
            tcb_Sched_قطار_میں_ڈالو(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } ورنہ {
        return_now = true;
    }

    اگر (return_now) {
        سیٹ_Consumed(sc, گرہ_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    }

    واپس EXCEPTION_NONE;
}

ساکن exception_t decode_Sched_Context_Yield_مقصد(sched_context_t *sc, bool_t call)
{
    اگر (sc->scTcb == تہی) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    اگر (tcb == گرہ_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (tcb->tcb_ترجیح > گرہ_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  گرہ_STATE(ksCurThread)->tcbMCP, tcb->tcb_ترجیح);
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    أكد(گرہ_STATE(ksCurThread)->tcb_Yield_مقصد == تہی);
    اگر (گرہ_STATE(ksCurThread)->tcb_Yield_مقصد != تہی) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invoke_Sched_Context_Yield_مقصد(sc, call);
}

exception_t decodeSchedContextInvocation(لفظ_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    انتخاب (label) {
    حالت SchedContextConsumed:
        /* no decode */
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeSchedContext_Consumed(sc, call);
    حالت SchedContextBind:
        واپس decodeSchedContext_Bind(sc);
    حالت SchedContextUnbindObject:
        واپس decodeSchedContext_UnbindObject(sc);
    حالت SchedContextUnbind:
        /* no decode */
        اگر (sc->scTcb == گرہ_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeSchedContext_Unbind(sc);
    حالت Sched_Context_Yield_مقصد:
        واپس decode_Sched_Context_Yield_مقصد(sc, call);
    طےشدہ:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}

خالی schedContext_resume(sched_context_t *sc)
{
    أكد(!sc || sc->scTcb != تہی);
    اگر (likely(sc) && isSchedulable(sc->scTcb)) {
        اگر (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            أكد(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

خالی schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    أكد(sc->scTcb == تہی);
    أكد(tcb->tcbSchedContext == تہی);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    اگر (sc_sporadic(sc) && sc_فعال(sc) && sc != گرہ_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    اگر (isSchedulable(tcb)) {
        SCHED_قطار_میں_ڈالو(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

خالی schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    أكد(tcb != تہی);

    /* tcb must already be stalled at this point */
    اگر (tcb == گرہ_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcb_Sched_قطار_سے_نکالو(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = تہی;
    sc->scTcb = تہی;
}

خالی schedContext_unbindAllTCBs(sched_context_t *sc)
{
    اگر (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

خالی schedContext_donate(sched_context_t *sc, tcb_t *مقصد)
{
    أكد(sc != تہی);
    أكد(مقصد != تہی);
    أكد(مقصد->tcbSchedContext == تہی);

    tcb_t *from = sc->scTcb;
    اگر (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcb_Sched_قطار_سے_نکالو(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = تہی;
        اگر (from == گرہ_STATE(ksCurThread) || from == گرہ_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = مقصد;
    مقصد->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(مقصد, sc->scCore));
}

خالی schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_سیٹ_ntfn_Sched_Context(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

خالی schedContext_unbindNtfn(sched_context_t *sc)
{
    اگر (sc && sc->scNotification) {
        notification_ptr_سیٹ_ntfn_Sched_Context(sc->scNotification, SC_REF(0));
        sc->scNotification = تہی;
    }
}

خالی schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    اگر (sc) {
        schedContext_unbindNtfn(sc);
    }
}

خالی sched_Context_unbind_جواب(sched_context_t *sc)
{
    اگر (sc && sc->sc_جواب) {
        أكد(call_ڈھیر_get_is_سر(sc->sc_جواب->جواب_اگلا));
        sc->sc_جواب->جواب_اگلا = call_ڈھیر_new(0, false);
        sc->sc_جواب = تہی;
    }
}

وقت_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    اگر (consumed >= get_Max_Ticks_مقصد_Us()) {
        sc->scConsumed -= get_Max_Ticks_مقصد_Us();
        واپس ticks_مقصد_Us(get_Max_Ticks_مقصد_Us());
    } ورنہ {
        sc->scConsumed = 0;
        واپس ticks_مقصد_Us(consumed);
    }
}

خالی sched_Context_cancel_Yield_مقصد(tcb_t *tcb)
{
    اگر (tcb && tcb->tcb_Yield_مقصد) {
        tcb->tcb_Yield_مقصد->scYieldFrom = تہی;
        tcb->tcb_Yield_مقصد = تہی;
    }
}

خالی sched_Context_complete_Yield_مقصد(tcb_t *yielder)
{
    اگر (yielder && yielder->tcb_Yield_مقصد) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        سیٹ_Consumed(yielder->tcb_Yield_مقصد, yielder, true);
        sched_Context_cancel_Yield_مقصد(yielder);
    }
}
