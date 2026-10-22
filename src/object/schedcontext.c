/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <machine/timer.h>
#అన్తర్భావ <kernel/sporadic.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <object/schedcontext.h>

స్థిర exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_thread_cap:
        schedContext_unbindTCB(sc);
        ఆపు;
    స్థితి cap_notification_cap:
        schedContext_unbindNtfn(sc);
        ఆపు;
    అప్రమేయ:
        fail("invalid cap type");
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_thread_cap:
        ఒకవేళ (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఒకవేళ (sc->scTcb == కణుపు_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఆపు;
    స్థితి cap_notification_cap:
        ఒకవేళ (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఆపు;

    అప్రమేయ:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;

    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeSchedContext_UnbindObject(sc, cap);
}

స్థిర exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        ఆపు;
    స్థితి cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        ఆపు;
    అప్రమేయ:
        fail("invalid cap type");
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_thread_cap:
        ఒకవేళ (sc->scTcb != ఖాళీ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != ఖాళీ) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఆపు;
    స్థితి cap_notification_cap:
        ఒకవేళ (sc->scNotification != ఖాళీ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఆపు;
    అప్రమేయ:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeSchedContext_Bind(sc, cap);
}

స్థిర exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    sched_Context_unbind_జవాబు(sc);
    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
స్థిర అంతఃవరుస శూన్య maybeStallSC(sched_context_t *sc)
{
    ఒకవేళ (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#అయితే_ముగింపు

స్థిర అంతఃవరుస శూన్య జవాబు_From_Kernel_consumed(tcb_t *thread, సమయం_t consumed)
{
    మాట_t *బఫరు = lookupIPCBuffer(true, thread);
    సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
    మాట_t నిడివి = బహుళకం_సమితి_సమయం_Arg(0, consumed, బఫరు, thread);
    సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
}

స్థిర అంతఃవరుస శూన్య సమితి_Consumed(sched_context_t *sc, tcb_t *thread, bool_t write_సందేశం)
{
    సమయం_t consumed = schedContext_updateConsumed(sc);

    ఒకవేళ (write_సందేశం) {
        జవాబు_From_Kernel_consumed(thread, consumed);
    }
}

స్థిర exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    సమితి_Consumed(sc, కణుపు_STATE(ksCurThread), call);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t invoke_Sched_Context_Yield_గమ్యం(sched_context_t *sc, bool_t call)
{
    ఒకవేళ (sc->scYieldFrom) {
        sched_Context_complete_Yield_గమ్యం(sc->scYieldFrom);
        ప్రతిజ్ఞా(sc->scYieldFrom == ఖాళీ);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    ఒకవేళ (isSchedulable(tcb)) {
        ఒకవేళ (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcb_ప్రాధాన్యత < కణుపు_STATE(ksCurThread)->tcb_ప్రాధాన్యత) {
            tcb_Sched_వరుస_నుండి_తీయి(tcb);
            SCHED_వరుసలో_చేర్చు(tcb);
            return_now = true;
        } లేకపోతే {
            కణుపు_STATE(ksCurThread)->tcb_Yield_గమ్యం = sc;
            sc->scYieldFrom = కణుపు_STATE(ksCurThread);
            tcb_Sched_వరుస_నుండి_తీయి(tcb);
            tcb_Sched_వరుసలో_చేర్చు(కణుపు_STATE(ksCurThread));
            tcb_Sched_వరుసలో_చేర్చు(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } లేకపోతే {
        return_now = true;
    }

    ఒకవేళ (return_now) {
        సమితి_Consumed(sc, కణుపు_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decode_Sched_Context_Yield_గమ్యం(sched_context_t *sc, bool_t call)
{
    ఒకవేళ (sc->scTcb == ఖాళీ) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    ఒకవేళ (tcb == కణుపు_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (tcb->tcb_ప్రాధాన్యత > కణుపు_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  కణుపు_STATE(ksCurThread)->tcbMCP, tcb->tcb_ప్రాధాన్యత);
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    ప్రతిజ్ఞా(కణుపు_STATE(ksCurThread)->tcb_Yield_గమ్యం == ఖాళీ);
    ఒకవేళ (కణుపు_STATE(ksCurThread)->tcb_Yield_గమ్యం != ఖాళీ) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invoke_Sched_Context_Yield_గమ్యం(sc, call);
}

exception_t decodeSchedContextInvocation(మాట_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    ఎంపిక (label) {
    స్థితి SchedContextConsumed:
        /* no decode */
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeSchedContext_Consumed(sc, call);
    స్థితి SchedContextBind:
        తిరిగి decodeSchedContext_Bind(sc);
    స్థితి SchedContextUnbindObject:
        తిరిగి decodeSchedContext_UnbindObject(sc);
    స్థితి SchedContextUnbind:
        /* no decode */
        ఒకవేళ (sc->scTcb == కణుపు_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeSchedContext_Unbind(sc);
    స్థితి Sched_Context_Yield_గమ్యం:
        తిరిగి decode_Sched_Context_Yield_గమ్యం(sc, call);
    అప్రమేయ:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

శూన్య schedContext_resume(sched_context_t *sc)
{
    ప్రతిజ్ఞా(!sc || sc->scTcb != ఖాళీ);
    ఒకవేళ (likely(sc) && isSchedulable(sc->scTcb)) {
        ఒకవేళ (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            ప్రతిజ్ఞా(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

శూన్య schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    ప్రతిజ్ఞా(sc->scTcb == ఖాళీ);
    ప్రతిజ్ఞా(tcb->tcbSchedContext == ఖాళీ);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    ఒకవేళ (sc_sporadic(sc) && sc_చురుకైన(sc) && sc != కణుపు_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    ఒకవేళ (isSchedulable(tcb)) {
        SCHED_వరుసలో_చేర్చు(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

శూన్య schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ప్రతిజ్ఞా(tcb != ఖాళీ);

    /* tcb must already be stalled at this point */
    ఒకవేళ (tcb == కణుపు_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcb_Sched_వరుస_నుండి_తీయి(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = ఖాళీ;
    sc->scTcb = ఖాళీ;
}

శూన్య schedContext_unbindAllTCBs(sched_context_t *sc)
{
    ఒకవేళ (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

శూన్య schedContext_donate(sched_context_t *sc, tcb_t *గమ్యం)
{
    ప్రతిజ్ఞా(sc != ఖాళీ);
    ప్రతిజ్ఞా(గమ్యం != ఖాళీ);
    ప్రతిజ్ఞా(గమ్యం->tcbSchedContext == ఖాళీ);

    tcb_t *from = sc->scTcb;
    ఒకవేళ (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcb_Sched_వరుస_నుండి_తీయి(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = ఖాళీ;
        ఒకవేళ (from == కణుపు_STATE(ksCurThread) || from == కణుపు_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = గమ్యం;
    గమ్యం->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(గమ్యం, sc->scCore));
}

శూన్య schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_సమితి_ntfn_Sched_Context(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

శూన్య schedContext_unbindNtfn(sched_context_t *sc)
{
    ఒకవేళ (sc && sc->scNotification) {
        notification_ptr_సమితి_ntfn_Sched_Context(sc->scNotification, SC_REF(0));
        sc->scNotification = ఖాళీ;
    }
}

శూన్య schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    ఒకవేళ (sc) {
        schedContext_unbindNtfn(sc);
    }
}

శూన్య sched_Context_unbind_జవాబు(sched_context_t *sc)
{
    ఒకవేళ (sc && sc->sc_జవాబు) {
        ప్రతిజ్ఞా(call_దొంతర_get_is_తల(sc->sc_జవాబు->జవాబు_తదుపరి));
        sc->sc_జవాబు->జవాబు_తదుపరి = call_దొంతర_new(0, false);
        sc->sc_జవాబు = ఖాళీ;
    }
}

సమయం_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    ఒకవేళ (consumed >= get_Max_Ticks_గమ్యం_Us()) {
        sc->scConsumed -= get_Max_Ticks_గమ్యం_Us();
        తిరిగి ticks_గమ్యం_Us(get_Max_Ticks_గమ్యం_Us());
    } లేకపోతే {
        sc->scConsumed = 0;
        తిరిగి ticks_గమ్యం_Us(consumed);
    }
}

శూన్య sched_Context_cancel_Yield_గమ్యం(tcb_t *tcb)
{
    ఒకవేళ (tcb && tcb->tcb_Yield_గమ్యం) {
        tcb->tcb_Yield_గమ్యం->scYieldFrom = ఖాళీ;
        tcb->tcb_Yield_గమ్యం = ఖాళీ;
    }
}

శూన్య sched_Context_complete_Yield_గమ్యం(tcb_t *yielder)
{
    ఒకవేళ (yielder && yielder->tcb_Yield_గమ్యం) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        సమితి_Consumed(yielder->tcb_Yield_గమ్యం, yielder, true);
        sched_Context_cancel_Yield_గమ్యం(yielder);
    }
}
