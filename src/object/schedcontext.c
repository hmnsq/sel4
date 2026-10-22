/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <machine/timer.h>
#ಅನ್ತರ್ಭಾವ <kernel/sporadic.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/schedcontext.h>

ಸ್ಥಿರ exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_thread_cap:
        schedContext_unbindTCB(sc);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_notification_cap:
        schedContext_unbindNtfn(sc);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("invalid cap type");
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_thread_cap:
        ಆದರೆ (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಆದರೆ (sc->scTcb == ಗಂಟು_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_notification_cap:
        ಆದರೆ (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;

    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeSchedContext_UnbindObject(sc, cap);
}

ಸ್ಥಿರ exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("invalid cap type");
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_thread_cap:
        ಆದರೆ (sc->scTcb != ಖಾಲಿ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != ಖಾಲಿ) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_notification_cap:
        ಆದರೆ (sc->scNotification != ಖಾಲಿ) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeSchedContext_Bind(sc, cap);
}

ಸ್ಥಿರ exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    sched_Context_unbind_ಉತ್ತರ(sc);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ maybeStallSC(sched_context_t *sc)
{
    ಆದರೆ (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಉತ್ತರ_From_Kernel_consumed(tcb_t *thread, ಸಮಯ_t consumed)
{
    ಶಬ್ದ_t *ಬಫರ್ = lookupIPCBuffer(true, thread);
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
    ಶಬ್ದ_t ಉದ್ದಳತೆ = ಬಹುಲಕ_ಗಣ_ಸಮಯ_Arg(0, consumed, ಬಫರ್, thread);
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ_Consumed(sched_context_t *sc, tcb_t *thread, bool_t write_ಸಂದೇಶ)
{
    ಸಮಯ_t consumed = schedContext_updateConsumed(sc);

    ಆದರೆ (write_ಸಂದೇಶ) {
        ಉತ್ತರ_From_Kernel_consumed(thread, consumed);
    }
}

ಸ್ಥಿರ exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    ಗಣ_Consumed(sc, ಗಂಟು_STATE(ksCurThread), call);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t invoke_Sched_Context_Yield_ಗಮ್ಯ(sched_context_t *sc, bool_t call)
{
    ಆದರೆ (sc->scYieldFrom) {
        sched_Context_complete_Yield_ಗಮ್ಯ(sc->scYieldFrom);
        ಪ್ರತಿಜ್ಞಾ(sc->scYieldFrom == ಖಾಲಿ);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    ಆದರೆ (isSchedulable(tcb)) {
        ಆದರೆ (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcb_ಆದ್ಯತೆ < ಗಂಟು_STATE(ksCurThread)->tcb_ಆದ್ಯತೆ) {
            tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(tcb);
            SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(tcb);
            return_now = true;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಂಟು_STATE(ksCurThread)->tcb_Yield_ಗಮ್ಯ = sc;
            sc->scYieldFrom = ಗಂಟು_STATE(ksCurThread);
            tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(tcb);
            tcb_Sched_ಸರತಿಗೆ_ಸೇರಿಸು(ಗಂಟು_STATE(ksCurThread));
            tcb_Sched_ಸರತಿಗೆ_ಸೇರಿಸು(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        return_now = true;
    }

    ಆದರೆ (return_now) {
        ಗಣ_Consumed(sc, ಗಂಟು_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decode_Sched_Context_Yield_ಗಮ್ಯ(sched_context_t *sc, bool_t call)
{
    ಆದರೆ (sc->scTcb == ಖಾಲಿ) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    ಆದರೆ (tcb == ಗಂಟು_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (tcb->tcb_ಆದ್ಯತೆ > ಗಂಟು_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  ಗಂಟು_STATE(ksCurThread)->tcbMCP, tcb->tcb_ಆದ್ಯತೆ);
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE(ksCurThread)->tcb_Yield_ಗಮ್ಯ == ಖಾಲಿ);
    ಆದರೆ (ಗಂಟು_STATE(ksCurThread)->tcb_Yield_ಗಮ್ಯ != ಖಾಲಿ) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invoke_Sched_Context_Yield_ಗಮ್ಯ(sc, call);
}

exception_t decodeSchedContextInvocation(ಶಬ್ದ_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    ಆಯ್ಕೆ (label) {
    ಸ್ಥಿತಿ SchedContextConsumed:
        /* no decode */
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeSchedContext_Consumed(sc, call);
    ಸ್ಥಿತಿ SchedContextBind:
        ಹಿಂತಿರುಗಿ decodeSchedContext_Bind(sc);
    ಸ್ಥಿತಿ SchedContextUnbindObject:
        ಹಿಂತಿರುಗಿ decodeSchedContext_UnbindObject(sc);
    ಸ್ಥಿತಿ SchedContextUnbind:
        /* no decode */
        ಆದರೆ (sc->scTcb == ಗಂಟು_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeSchedContext_Unbind(sc);
    ಸ್ಥಿತಿ Sched_Context_Yield_ಗಮ್ಯ:
        ಹಿಂತಿರುಗಿ decode_Sched_Context_Yield_ಗಮ್ಯ(sc, call);
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

ಶೂನ್ಯ schedContext_resume(sched_context_t *sc)
{
    ಪ್ರತಿಜ್ಞಾ(!sc || sc->scTcb != ಖಾಲಿ);
    ಆದರೆ (likely(sc) && isSchedulable(sc->scTcb)) {
        ಆದರೆ (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            ಪ್ರತಿಜ್ಞಾ(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

ಶೂನ್ಯ schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    ಪ್ರತಿಜ್ಞಾ(sc->scTcb == ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(tcb->tcbSchedContext == ಖಾಲಿ);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    ಆದರೆ (sc_sporadic(sc) && sc_ಸಕ್ರಿಯ(sc) && sc != ಗಂಟು_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    ಆದರೆ (isSchedulable(tcb)) {
        SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

ಶೂನ್ಯ schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ಪ್ರತಿಜ್ಞಾ(tcb != ಖಾಲಿ);

    /* tcb must already be stalled at this point */
    ಆದರೆ (tcb == ಗಂಟು_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = ಖಾಲಿ;
    sc->scTcb = ಖಾಲಿ;
}

ಶೂನ್ಯ schedContext_unbindAllTCBs(sched_context_t *sc)
{
    ಆದರೆ (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

ಶೂನ್ಯ schedContext_donate(sched_context_t *sc, tcb_t *ಗಮ್ಯ)
{
    ಪ್ರತಿಜ್ಞಾ(sc != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(ಗಮ್ಯ != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(ಗಮ್ಯ->tcbSchedContext == ಖಾಲಿ);

    tcb_t *from = sc->scTcb;
    ಆದರೆ (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = ಖಾಲಿ;
        ಆದರೆ (from == ಗಂಟು_STATE(ksCurThread) || from == ಗಂಟು_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = ಗಮ್ಯ;
    ಗಮ್ಯ->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(ಗಮ್ಯ, sc->scCore));
}

ಶೂನ್ಯ schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_ಗಣ_ntfn_Sched_Context(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

ಶೂನ್ಯ schedContext_unbindNtfn(sched_context_t *sc)
{
    ಆದರೆ (sc && sc->scNotification) {
        notification_ptr_ಗಣ_ntfn_Sched_Context(sc->scNotification, SC_REF(0));
        sc->scNotification = ಖಾಲಿ;
    }
}

ಶೂನ್ಯ schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    ಆದರೆ (sc) {
        schedContext_unbindNtfn(sc);
    }
}

ಶೂನ್ಯ sched_Context_unbind_ಉತ್ತರ(sched_context_t *sc)
{
    ಆದರೆ (sc && sc->sc_ಉತ್ತರ) {
        ಪ್ರತಿಜ್ಞಾ(call_ರಾಶಿ_get_is_ತಲೆ(sc->sc_ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ));
        sc->sc_ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(0, false);
        sc->sc_ಉತ್ತರ = ಖಾಲಿ;
    }
}

ಸಮಯ_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    ಆದರೆ (consumed >= get_Max_Ticks_ಗಮ್ಯ_Us()) {
        sc->scConsumed -= get_Max_Ticks_ಗಮ್ಯ_Us();
        ಹಿಂತಿರುಗಿ ticks_ಗಮ್ಯ_Us(get_Max_Ticks_ಗಮ್ಯ_Us());
    } ಇಲ್ಲದಿದ್ದರೆ {
        sc->scConsumed = 0;
        ಹಿಂತಿರುಗಿ ticks_ಗಮ್ಯ_Us(consumed);
    }
}

ಶೂನ್ಯ sched_Context_cancel_Yield_ಗಮ್ಯ(tcb_t *tcb)
{
    ಆದರೆ (tcb && tcb->tcb_Yield_ಗಮ್ಯ) {
        tcb->tcb_Yield_ಗಮ್ಯ->scYieldFrom = ಖಾಲಿ;
        tcb->tcb_Yield_ಗಮ್ಯ = ಖಾಲಿ;
    }
}

ಶೂನ್ಯ sched_Context_complete_Yield_ಗಮ್ಯ(tcb_t *yielder)
{
    ಆದರೆ (yielder && yielder->tcb_Yield_ಗಮ್ಯ) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        ಗಣ_Consumed(yielder->tcb_Yield_ಗಮ್ಯ, yielder, true);
        sched_Context_cancel_Yield_ಗಮ್ಯ(yielder);
    }
}
