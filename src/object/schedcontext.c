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
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
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
        यदि (sc->scTcb == ग्रन्थिः_STATE(ksCurThread)) {
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
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;

    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
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
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
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
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeSchedContext_Bind(sc, cap);
}

स्थिर exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    sched_Context_unbind_प्रत्युत्तरम्(sc);
    प्रत्यागम EXCEPTION_NONE;
}

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
स्थिर अन्तःस्थ शून्य maybeStallSC(sched_context_t *sc)
{
    यदि (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#यदि_अन्तः

स्थिर अन्तःस्थ शून्य प्रत्युत्तरम्_From_Kernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *बफर = lookupIPCBuffer(true, thread);
    समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
    word_t दीर्घता = बहुलकम्_समुच्चयः_Time_Arg(0, consumed, बफर, thread);
    समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
}

स्थिर अन्तःस्थ शून्य समुच्चयः_Consumed(sched_context_t *sc, tcb_t *thread, bool_t write_सन्देशः)
{
    time_t consumed = schedContext_updateConsumed(sc);

    यदि (write_सन्देशः) {
        प्रत्युत्तरम्_From_Kernel_consumed(thread, consumed);
    }
}

स्थिर exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    समुच्चयः_Consumed(sc, ग्रन्थिः_STATE(ksCurThread), call);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t invoke_Sched_Context_Yield_गन्तव्यम्(sched_context_t *sc, bool_t call)
{
    यदि (sc->scYieldFrom) {
        sched_Context_complete_Yield_गन्तव्यम्(sc->scYieldFrom);
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
            tcb->tcb_प्राथमिकता < ग्रन्थिः_STATE(ksCurThread)->tcb_प्राथमिकता) {
            tcb_Sched_पङ्क्तेः_निष्कासय(tcb);
            SCHED_पङ्क्तौ_निक्षिप(tcb);
            return_now = true;
        } अन्यथा {
            ग्रन्थिः_STATE(ksCurThread)->tcb_Yield_गन्तव्यम् = sc;
            sc->scYieldFrom = ग्रन्थिः_STATE(ksCurThread);
            tcb_Sched_पङ्क्तेः_निष्कासय(tcb);
            tcb_Sched_पङ्क्तौ_निक्षिप(ग्रन्थिः_STATE(ksCurThread));
            tcb_Sched_पङ्क्तौ_निक्षिप(tcb);
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
        समुच्चयः_Consumed(sc, ग्रन्थिः_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    }

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decode_Sched_Context_Yield_गन्तव्यम्(sched_context_t *sc, bool_t call)
{
    यदि (sc->scTcb == रिक्त) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    यदि (tcb == ग्रन्थिः_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (tcb->tcb_प्राथमिकता > ग्रन्थिः_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  ग्रन्थिः_STATE(ksCurThread)->tcbMCP, tcb->tcb_प्राथमिकता);
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    प्रतिज्ञा(ग्रन्थिः_STATE(ksCurThread)->tcb_Yield_गन्तव्यम् == रिक्त);
    यदि (ग्रन्थिः_STATE(ksCurThread)->tcb_Yield_गन्तव्यम् != रिक्त) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invoke_Sched_Context_Yield_गन्तव्यम्(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    विकल्प (label) {
    पक्ष SchedContextConsumed:
        /* no decode */
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeSchedContext_Consumed(sc, call);
    पक्ष SchedContextBind:
        प्रत्यागम decodeSchedContext_Bind(sc);
    पक्ष SchedContextUnbindObject:
        प्रत्यागम decodeSchedContext_UnbindObject(sc);
    पक्ष SchedContextUnbind:
        /* no decode */
        यदि (sc->scTcb == ग्रन्थिः_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeSchedContext_Unbind(sc);
    पक्ष Sched_Context_Yield_गन्तव्यम्:
        प्रत्यागम decode_Sched_Context_Yield_गन्तव्यम्(sc, call);
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

    यदि (sc_sporadic(sc) && sc_सक्रिय(sc) && sc != ग्रन्थिः_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    यदि (isSchedulable(tcb)) {
        SCHED_पङ्क्तौ_निक्षिप(tcb);
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
    यदि (tcb == ग्रन्थिः_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcb_Sched_पङ्क्तेः_निष्कासय(tcb);
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

शून्य schedContext_donate(sched_context_t *sc, tcb_t *गन्तव्यम्)
{
    प्रतिज्ञा(sc != रिक्त);
    प्रतिज्ञा(गन्तव्यम् != रिक्त);
    प्रतिज्ञा(गन्तव्यम्->tcbSchedContext == रिक्त);

    tcb_t *from = sc->scTcb;
    यदि (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcb_Sched_पङ्क्तेः_निष्कासय(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = रिक्त;
        यदि (from == ग्रन्थिः_STATE(ksCurThread) || from == ग्रन्थिः_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = गन्तव्यम्;
    गन्तव्यम्->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(गन्तव्यम्, sc->scCore));
}

शून्य schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_समुच्चयः_ntfn_Sched_Context(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

शून्य schedContext_unbindNtfn(sched_context_t *sc)
{
    यदि (sc && sc->scNotification) {
        notification_ptr_समुच्चयः_ntfn_Sched_Context(sc->scNotification, SC_REF(0));
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

शून्य sched_Context_unbind_प्रत्युत्तरम्(sched_context_t *sc)
{
    यदि (sc && sc->sc_प्रत्युत्तरम्) {
        प्रतिज्ञा(call_राशिः_get_is_शीर्ष(sc->sc_प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र));
        sc->sc_प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र = call_राशिः_new(0, false);
        sc->sc_प्रत्युत्तरम् = रिक्त;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    यदि (consumed >= get_Max_Ticks_गन्तव्यम्_Us()) {
        sc->scConsumed -= get_Max_Ticks_गन्तव्यम्_Us();
        प्रत्यागम ticks_गन्तव्यम्_Us(get_Max_Ticks_गन्तव्यम्_Us());
    } अन्यथा {
        sc->scConsumed = 0;
        प्रत्यागम ticks_गन्तव्यम्_Us(consumed);
    }
}

शून्य sched_Context_cancel_Yield_गन्तव्यम्(tcb_t *tcb)
{
    यदि (tcb && tcb->tcb_Yield_गन्तव्यम्) {
        tcb->tcb_Yield_गन्तव्यम्->scYieldFrom = रिक्त;
        tcb->tcb_Yield_गन्तव्यम् = रिक्त;
    }
}

शून्य sched_Context_complete_Yield_गन्तव्यम्(tcb_t *yielder)
{
    यदि (yielder && yielder->tcb_Yield_गन्तव्यम्) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        समुच्चयः_Consumed(yielder->tcb_Yield_गन्तव्यम्, yielder, true);
        sched_Context_cancel_Yield_गन्तव्यम्(yielder);
    }
}
