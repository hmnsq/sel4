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
        current_syscall_error.type = se_L4_Truncated_پیغام;
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
        إذا (sc->scTcb == غوټه_STATE(ksCurThread)) {
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
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;

    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
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
        current_syscall_error.type = se_L4_Truncated_پیغام;
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
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSchedContext_Bind(sc, cap);
}

ساكن exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    sched_Context_unbind_ځواب(sc);
    أرجع EXCEPTION_NONE;
}

#که_تعریف_شوی ENABLE_SMP_SUPPORT
ساكن مضمن فراغ maybeStallSC(sched_context_t *sc)
{
    إذا (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#د_که_پای

ساكن مضمن فراغ ځواب_From_Kernel_consumed(tcb_t *thread, وخت_t consumed)
{
    کلمه_t *بفر = lookupIPCBuffer(true, thread);
    سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
    کلمه_t اوږدتیا = ډېرپېښ_سېټ_وخت_Arg(0, consumed, بفر, thread);
    سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(se_L4_پیغام_Info_new(0, 0, 0, اوږدتیا)));
}

ساكن مضمن فراغ سېټ_Consumed(sched_context_t *sc, tcb_t *thread, bool_t write_پیغام)
{
    وخت_t consumed = schedContext_updateConsumed(sc);

    إذا (write_پیغام) {
        ځواب_From_Kernel_consumed(thread, consumed);
    }
}

ساكن exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    سېټ_Consumed(sc, غوټه_STATE(ksCurThread), call);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t invoke_Sched_Context_Yield_منزل(sched_context_t *sc, bool_t call)
{
    إذا (sc->scYieldFrom) {
        sched_Context_complete_Yield_منزل(sc->scYieldFrom);
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
            tcb->tcb_لومړیتوب < غوټه_STATE(ksCurThread)->tcb_لومړیتوب) {
            tcb_Sched_له_کتار_ایستل(tcb);
            SCHED_کتار_ته_ورزیاتول(tcb);
            return_now = true;
        } وإلا {
            غوټه_STATE(ksCurThread)->tcb_Yield_منزل = sc;
            sc->scYieldFrom = غوټه_STATE(ksCurThread);
            tcb_Sched_له_کتار_ایستل(tcb);
            tcb_Sched_کتار_ته_ورزیاتول(غوټه_STATE(ksCurThread));
            tcb_Sched_کتار_ته_ورزیاتول(tcb);
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
        سېټ_Consumed(sc, غوټه_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_Sched_Context_Yield_منزل(sched_context_t *sc, bool_t call)
{
    إذا (sc->scTcb == عدم) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    إذا (tcb == غوټه_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (tcb->tcb_لومړیتوب > غوټه_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  غوټه_STATE(ksCurThread)->tcbMCP, tcb->tcb_لومړیتوب);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    أكد(غوټه_STATE(ksCurThread)->tcb_Yield_منزل == عدم);
    إذا (غوټه_STATE(ksCurThread)->tcb_Yield_منزل != عدم) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_Sched_Context_Yield_منزل(sc, call);
}

exception_t decodeSchedContextInvocation(کلمه_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    بدل (label) {
    حالة SchedContextConsumed:
        /* no decode */
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeSchedContext_Consumed(sc, call);
    حالة SchedContextBind:
        أرجع decodeSchedContext_Bind(sc);
    حالة SchedContextUnbindObject:
        أرجع decodeSchedContext_UnbindObject(sc);
    حالة SchedContextUnbind:
        /* no decode */
        إذا (sc->scTcb == غوټه_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeSchedContext_Unbind(sc);
    حالة Sched_Context_Yield_منزل:
        أرجع decode_Sched_Context_Yield_منزل(sc, call);
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

    إذا (sc_sporadic(sc) && sc_فعال(sc) && sc != غوټه_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    إذا (isSchedulable(tcb)) {
        SCHED_کتار_ته_ورزیاتول(tcb);
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
    إذا (tcb == غوټه_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcb_Sched_له_کتار_ایستل(tcb);
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

فراغ schedContext_donate(sched_context_t *sc, tcb_t *منزل)
{
    أكد(sc != عدم);
    أكد(منزل != عدم);
    أكد(منزل->tcbSchedContext == عدم);

    tcb_t *from = sc->scTcb;
    إذا (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcb_Sched_له_کتار_ایستل(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = عدم;
        إذا (from == غوټه_STATE(ksCurThread) || from == غوټه_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = منزل;
    منزل->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(منزل, sc->scCore));
}

فراغ schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_سېټ_ntfn_Sched_Context(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

فراغ schedContext_unbindNtfn(sched_context_t *sc)
{
    إذا (sc && sc->scNotification) {
        notification_ptr_سېټ_ntfn_Sched_Context(sc->scNotification, SC_REF(0));
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

فراغ sched_Context_unbind_ځواب(sched_context_t *sc)
{
    إذا (sc && sc->sc_ځواب) {
        أكد(call_ډېری_get_is_سر(sc->sc_ځواب->ځواب_بل));
        sc->sc_ځواب->ځواب_بل = call_ډېری_new(0, false);
        sc->sc_ځواب = عدم;
    }
}

وخت_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    إذا (consumed >= get_Max_Ticks_منزل_Us()) {
        sc->scConsumed -= get_Max_Ticks_منزل_Us();
        أرجع ticks_منزل_Us(get_Max_Ticks_منزل_Us());
    } وإلا {
        sc->scConsumed = 0;
        أرجع ticks_منزل_Us(consumed);
    }
}

فراغ sched_Context_cancel_Yield_منزل(tcb_t *tcb)
{
    إذا (tcb && tcb->tcb_Yield_منزل) {
        tcb->tcb_Yield_منزل->scYieldFrom = عدم;
        tcb->tcb_Yield_منزل = عدم;
    }
}

فراغ sched_Context_complete_Yield_منزل(tcb_t *yielder)
{
    إذا (yielder && yielder->tcb_Yield_منزل) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        سېټ_Consumed(yielder->tcb_Yield_منزل, yielder, true);
        sched_Context_cancel_Yield_منزل(yielder);
    }
}
