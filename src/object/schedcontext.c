/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/sporadic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/schedcontext.h>

ਸਥਿਰ exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_thread_cap:
        schedContext_unbindTCB(sc);
        ਰੋਕੋ;
    ਹਾਲਤ cap_notification_cap:
        schedContext_unbindNtfn(sc);
        ਰੋਕੋ;
    ਮੂਲ:
        fail("invalid cap type");
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_thread_cap:
        ਜੇ (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਜੇ (sc->scTcb == ਗੰਢ_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_notification_cap:
        ਜੇ (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਰੋਕੋ;

    ਮੂਲ:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;

    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeSchedContext_UnbindObject(sc, cap);
}

ਸਥਿਰ exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        ਰੋਕੋ;
    ਹਾਲਤ cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        ਰੋਕੋ;
    ਮੂਲ:
        fail("invalid cap type");
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_thread_cap:
        ਜੇ (sc->scTcb != ਨਿਰਾ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != ਨਿਰਾ) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਰੋਕੋ;
    ਹਾਲਤ cap_notification_cap:
        ਜੇ (sc->scNotification != ਨਿਰਾ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਰੋਕੋ;
    ਮੂਲ:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeSchedContext_Bind(sc, cap);
}

ਸਥਿਰ exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    sched_Context_unbind_ਜਵਾਬ(sc);
    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ maybeStallSC(sched_context_t *sc)
{
    ਜੇ (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਜਵਾਬ_From_Kernel_consumed(tcb_t *thread, ਸਮਾਂ_t consumed)
{
    ਸ਼ਬਦ_t *ਬਫਰ = lookupIPCBuffer(true, thread);
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
    ਸ਼ਬਦ_t ਲੰਮਾਈ = ਬਹੁਲਕ_ਸੈੱਟ_ਸਮਾਂ_Arg(0, consumed, ਬਫਰ, thread);
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ_Consumed(sched_context_t *sc, tcb_t *thread, bool_t write_ਸੁਨੇਹਾ)
{
    ਸਮਾਂ_t consumed = schedContext_updateConsumed(sc);

    ਜੇ (write_ਸੁਨੇਹਾ) {
        ਜਵਾਬ_From_Kernel_consumed(thread, consumed);
    }
}

ਸਥਿਰ exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    ਸੈੱਟ_Consumed(sc, ਗੰਢ_STATE(ksCurThread), call);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t invoke_Sched_Context_Yield_ਮੰਜ਼ਿਲ(sched_context_t *sc, bool_t call)
{
    ਜੇ (sc->scYieldFrom) {
        sched_Context_complete_Yield_ਮੰਜ਼ਿਲ(sc->scYieldFrom);
        ਪ੍ਰਤਿਜ੍ਞਾ(sc->scYieldFrom == ਨਿਰਾ);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    ਜੇ (isSchedulable(tcb)) {
        ਜੇ (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcb_ਤਰਜੀਹ < ਗੰਢ_STATE(ksCurThread)->tcb_ਤਰਜੀਹ) {
            tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(tcb);
            SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(tcb);
            return_now = true;
        } ਨਹੀਂਤਾਂ {
            ਗੰਢ_STATE(ksCurThread)->tcb_Yield_ਮੰਜ਼ਿਲ = sc;
            sc->scYieldFrom = ਗੰਢ_STATE(ksCurThread);
            tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(tcb);
            tcb_Sched_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ਗੰਢ_STATE(ksCurThread));
            tcb_Sched_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } ਨਹੀਂਤਾਂ {
        return_now = true;
    }

    ਜੇ (return_now) {
        ਸੈੱਟ_Consumed(sc, ਗੰਢ_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decode_Sched_Context_Yield_ਮੰਜ਼ਿਲ(sched_context_t *sc, bool_t call)
{
    ਜੇ (sc->scTcb == ਨਿਰਾ) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    ਜੇ (tcb == ਗੰਢ_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (tcb->tcb_ਤਰਜੀਹ > ਗੰਢ_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  ਗੰਢ_STATE(ksCurThread)->tcbMCP, tcb->tcb_ਤਰਜੀਹ);
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    ਪ੍ਰਤਿਜ੍ਞਾ(ਗੰਢ_STATE(ksCurThread)->tcb_Yield_ਮੰਜ਼ਿਲ == ਨਿਰਾ);
    ਜੇ (ਗੰਢ_STATE(ksCurThread)->tcb_Yield_ਮੰਜ਼ਿਲ != ਨਿਰਾ) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invoke_Sched_Context_Yield_ਮੰਜ਼ਿਲ(sc, call);
}

exception_t decodeSchedContextInvocation(ਸ਼ਬਦ_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    ਚੋਣ (label) {
    ਹਾਲਤ SchedContextConsumed:
        /* no decode */
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeSchedContext_Consumed(sc, call);
    ਹਾਲਤ SchedContextBind:
        ਵਾਪਸ decodeSchedContext_Bind(sc);
    ਹਾਲਤ SchedContextUnbindObject:
        ਵਾਪਸ decodeSchedContext_UnbindObject(sc);
    ਹਾਲਤ SchedContextUnbind:
        /* no decode */
        ਜੇ (sc->scTcb == ਗੰਢ_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeSchedContext_Unbind(sc);
    ਹਾਲਤ Sched_Context_Yield_ਮੰਜ਼ਿਲ:
        ਵਾਪਸ decode_Sched_Context_Yield_ਮੰਜ਼ਿਲ(sc, call);
    ਮੂਲ:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

ਖਾਲੀ schedContext_resume(sched_context_t *sc)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(!sc || sc->scTcb != ਨਿਰਾ);
    ਜੇ (likely(sc) && isSchedulable(sc->scTcb)) {
        ਜੇ (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            ਪ੍ਰਤਿਜ੍ਞਾ(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

ਖਾਲੀ schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(sc->scTcb == ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(tcb->tcbSchedContext == ਨਿਰਾ);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    ਜੇ (sc_sporadic(sc) && sc_ਸਰਗਰਮ(sc) && sc != ਗੰਢ_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    ਜੇ (isSchedulable(tcb)) {
        SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

ਖਾਲੀ schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ਪ੍ਰਤਿਜ੍ਞਾ(tcb != ਨਿਰਾ);

    /* tcb must already be stalled at this point */
    ਜੇ (tcb == ਗੰਢ_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = ਨਿਰਾ;
    sc->scTcb = ਨਿਰਾ;
}

ਖਾਲੀ schedContext_unbindAllTCBs(sched_context_t *sc)
{
    ਜੇ (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

ਖਾਲੀ schedContext_donate(sched_context_t *sc, tcb_t *ਮੰਜ਼ਿਲ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(sc != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਮੰਜ਼ਿਲ != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਮੰਜ਼ਿਲ->tcbSchedContext == ਨਿਰਾ);

    tcb_t *from = sc->scTcb;
    ਜੇ (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = ਨਿਰਾ;
        ਜੇ (from == ਗੰਢ_STATE(ksCurThread) || from == ਗੰਢ_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = ਮੰਜ਼ਿਲ;
    ਮੰਜ਼ਿਲ->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(ਮੰਜ਼ਿਲ, sc->scCore));
}

ਖਾਲੀ schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_ਸੈੱਟ_ntfn_Sched_Context(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

ਖਾਲੀ schedContext_unbindNtfn(sched_context_t *sc)
{
    ਜੇ (sc && sc->scNotification) {
        notification_ptr_ਸੈੱਟ_ntfn_Sched_Context(sc->scNotification, SC_REF(0));
        sc->scNotification = ਨਿਰਾ;
    }
}

ਖਾਲੀ schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    ਜੇ (sc) {
        schedContext_unbindNtfn(sc);
    }
}

ਖਾਲੀ sched_Context_unbind_ਜਵਾਬ(sched_context_t *sc)
{
    ਜੇ (sc && sc->sc_ਜਵਾਬ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(call_ਢੇਰ_get_is_ਸਿਰ(sc->sc_ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ));
        sc->sc_ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ = call_ਢੇਰ_new(0, false);
        sc->sc_ਜਵਾਬ = ਨਿਰਾ;
    }
}

ਸਮਾਂ_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    ਜੇ (consumed >= get_Max_Ticks_ਮੰਜ਼ਿਲ_Us()) {
        sc->scConsumed -= get_Max_Ticks_ਮੰਜ਼ਿਲ_Us();
        ਵਾਪਸ ticks_ਮੰਜ਼ਿਲ_Us(get_Max_Ticks_ਮੰਜ਼ਿਲ_Us());
    } ਨਹੀਂਤਾਂ {
        sc->scConsumed = 0;
        ਵਾਪਸ ticks_ਮੰਜ਼ਿਲ_Us(consumed);
    }
}

ਖਾਲੀ sched_Context_cancel_Yield_ਮੰਜ਼ਿਲ(tcb_t *tcb)
{
    ਜੇ (tcb && tcb->tcb_Yield_ਮੰਜ਼ਿਲ) {
        tcb->tcb_Yield_ਮੰਜ਼ਿਲ->scYieldFrom = ਨਿਰਾ;
        tcb->tcb_Yield_ਮੰਜ਼ਿਲ = ਨਿਰਾ;
    }
}

ਖਾਲੀ sched_Context_complete_Yield_ਮੰਜ਼ਿਲ(tcb_t *yielder)
{
    ਜੇ (yielder && yielder->tcb_Yield_ਮੰਜ਼ਿਲ) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        ਸੈੱਟ_Consumed(yielder->tcb_Yield_ਮੰਜ਼ਿਲ, yielder, true);
        sched_Context_cancel_Yield_ਮੰਜ਼ਿਲ(yielder);
    }
}
