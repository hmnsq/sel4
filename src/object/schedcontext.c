/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <machine/timer.h>
#include <kernel/sporadic.h>
#include <kernel/thread.h>
#include <object/structures.h>
#include <object/schedcontext.h>

static exception_t invokeSchedContext_UnbindObject(sched_context_t *sc, cap_t cap)
{
    switch (cap_get_capType(cap)) {
    pak_sa cap_thread_cap:
        schedContext_unbindTCB(sc);
        bha_nga;
    pak_sa cap_notification_cap:
        schedContext_unbindNtfn(sc);
        bha_nga;
    default:
        fail("invalid cap type");
    }

    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeSchedContext_UnbindObject(sched_context_t *sc)
{
    ᰀᰧ (current_extra_caps.excaprefs[0] == NULL) {
        userError("SchedContext_Unbind: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;
    switch (cap_get_capType(cap)) {
    pak_sa cap_thread_cap:
        ᰀᰧ (sc->scTcb != TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
        }
        ᰀᰧ (sc->scTcb == NODE_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
        }
        bha_nga;
    pak_sa cap_notification_cap:
        ᰀᰧ (sc->scNotification != NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap))) {
            userError("SchedContext UnbindObject: object not bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
        }
        bha_nga;

    default:
        userError("SchedContext_Unbind: invalid cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;

    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ᰀᰧᰌ invokeSchedContext_UnbindObject(sc, cap);
}

static exception_t invokeSchedContext_Bind(sched_context_t *sc, cap_t cap)
{
    switch (cap_get_capType(cap)) {
    pak_sa cap_thread_cap:
        schedContext_bindTCB(sc, TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)));
        bha_nga;
    pak_sa cap_notification_cap:
        schedContext_bindNtfn(sc, NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)));
        bha_nga;
    default:
        fail("invalid cap type");
    }

    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeSchedContext_Bind(sched_context_t *sc)
{
    ᰀᰧ (current_extra_caps.excaprefs[0] == NULL) {
        userError("SchedContext_Bind: Truncated Message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t cap = current_extra_caps.excaprefs[0]->cap;

    switch (cap_get_capType(cap)) {
    pak_sa cap_thread_cap:
        ᰀᰧ (sc->scTcb != NULL) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
        }

        ᰀᰧ (TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))->tcbSchedContext != NULL) {
            userError("SchedContext_Bind: tcb already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
        }

        ᰀᰧ (isBlocked(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap))) && !sc_released(sc)) {
            userError("SchedContext_Bind: tcb blocked and scheduling context not schedulable.");
            current_syscall_error.type = seL4_IllegalOperation;
            ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
        }

        bha_nga;
    pak_sa cap_notification_cap:
        ᰀᰧ (sc->scNotification != NULL) {
            userError("SchedContext_Bind: sched context already bound.");
            current_syscall_error.type = seL4_IllegalOperation;
            ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
        }

        ᰀᰧ (notification_ptr_get_ntfnSchedContext(NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)))) {
            userError("SchedContext_Bind: notification already bound");
            current_syscall_error.type = seL4_IllegalOperation;
            ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
        }
        bha_nga;
    default:
        userError("SchedContext_Bind: invalid cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 1;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ᰀᰧᰌ invokeSchedContext_Bind(sc, cap);
}

static exception_t invokeSchedContext_Unbind(sched_context_t *sc)
{
    schedContext_unbindAllTCBs(sc);
    schedContext_unbindNtfn(sc);
    schedContext_unbindReply(sc);
    ᰀᰧᰌ EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
static anta_hstha void maybeStallSC(sched_context_t *sc)
{
    ᰀᰧ (sc->scTcb) {
        remoteTCBStall(sc->scTcb);
    }
}
#endif

static anta_hstha void replyFromKernel_consumed(tcb_t *thread, time_t consumed)
{
    word_t *buffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    word_t length = mode_setTimeArg(0, consumed, buffer, thread);
    setRegister(thread, msgInfoRegister, wordFromMessageInfo(seL4_MessageInfo_new(0, 0, 0, length)));
}

static anta_hstha void setConsumed(sched_context_t *sc, tcb_t *thread, bool_t write_msg)
{
    time_t consumed = schedContext_updateConsumed(sc);

    ᰀᰧ (write_msg) {
        replyFromKernel_consumed(thread, consumed);
    }
}

static exception_t invokeSchedContext_Consumed(sched_context_t *sc, bool_t call)
{
    setConsumed(sc, NODE_STATE(ksCurThread), call);

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t invokeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    ᰀᰧ (sc->scYieldFrom) {
        schedContext_completeYieldTo(sc->scYieldFrom);
        assert(sc->scYieldFrom == NULL);
    }

    /* if the tcb is in the scheduler, it's ready and sufficient.
     * Otherwise, check that it is ready and sufficient and if not,
     * place the thread in the release queue. This way, from this point,
     * if the thread isSchedulable, it is ready and sufficient.*/
    schedContext_resume(sc);

    tcb_t *tcb = sc->scTcb;

    bool_t return_now;
    ᰀᰧ (isSchedulable(tcb)) {
        ᰀᰧ (SMP_COND_STATEMENT(sc->scCore != getCurrentCPUIndex() ||)
            tcb->tcbPriority < NODE_STATE(ksCurThread)->tcbPriority) {
            tcbSchedDequeue(tcb);
            SCHED_ENQUEUE(tcb);
            return_now = true;
        } ᰀᰧᰀ {
            NODE_STATE(ksCurThread)->tcbYieldTo = sc;
            sc->scYieldFrom = NODE_STATE(ksCurThread);
            tcbSchedDequeue(tcb);
            tcbSchedEnqueue(NODE_STATE(ksCurThread));
            tcbSchedEnqueue(tcb);
            rescheduleRequired();

            /* we are scheduling the thread associated with sc,
             * so we don't need to write to the ipc buffer
             * until the caller is scheduled again */
            return_now = false;
        }
    } ᰀᰧᰀ {
        return_now = true;
    }

    ᰀᰧ (return_now) {
        setConsumed(sc, NODE_STATE(ksCurThread), call);
        /* Only set to Running if there is a kernel reply message for the user.
           Restart will create a default empty success message. */
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    }

    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeSchedContext_YieldTo(sched_context_t *sc, bool_t call)
{
    ᰀᰧ (sc->scTcb == NULL) {
        userError("SchedContext_YieldTo: cannot yield to an inactive sched context");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    tcb_t *tcb = sc->scTcb;

    ᰀᰧ (tcb == NODE_STATE(ksCurThread)) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo on self");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    ᰀᰧ (tcb->tcbPriority > NODE_STATE(ksCurThread)->tcbMCP) {
        userError("SchedContext_YieldTo: insufficient mcp (%lu) to yield to a thread with prio (%lu)",
                  NODE_STATE(ksCurThread)->tcbMCP, tcb->tcbPriority);
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    // This should not be possible as the currently running thread
    // should never have a non-null yieldTo, however verifying this
    // invariant is being left to future work.
    assert(NODE_STATE(ksCurThread)->tcbYieldTo == NULL);
    ᰀᰧ (NODE_STATE(ksCurThread)->tcbYieldTo != NULL) {
        userError("SchedContext_YieldTo: cannot seL4_SchedContext_YieldTo to more than on SC at a time");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ᰀᰧᰌ invokeSchedContext_YieldTo(sc, call);
}

exception_t decodeSchedContextInvocation(word_t label, sched_context_t *sc, bool_t call)
{
    SMP_COND_STATEMENT((maybeStallSC(sc));)

    switch (label) {
    pak_sa SchedContextConsumed:
        /* no decode */
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        ᰀᰧᰌ invokeSchedContext_Consumed(sc, call);
    pak_sa SchedContextBind:
        ᰀᰧᰌ decodeSchedContext_Bind(sc);
    pak_sa SchedContextUnbindObject:
        ᰀᰧᰌ decodeSchedContext_UnbindObject(sc);
    pak_sa SchedContextUnbind:
        /* no decode */
        ᰀᰧ (sc->scTcb == NODE_STATE(ksCurThread)) {
            userError("SchedContext UnbindObject: cannot unbind sc of current thread");
            current_syscall_error.type = seL4_IllegalOperation;
            ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        ᰀᰧᰌ invokeSchedContext_Unbind(sc);
    pak_sa SchedContextYieldTo:
        ᰀᰧᰌ decodeSchedContext_YieldTo(sc, call);
    default:
        userError("SchedContext invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
}

void schedContext_resume(sched_context_t *sc)
{
    assert(!sc || sc->scTcb != NULL);
    ᰀᰧ (likely(sc) && isSchedulable(sc->scTcb)) {
        ᰀᰧ (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            assert(!thread_state_get_tcbQueued(sc->scTcb->tcbState));
            postpone(sc);
        }
    }
}

void schedContext_bindTCB(sched_context_t *sc, tcb_t *tcb)
{
    assert(sc->scTcb == NULL);
    assert(tcb->tcbSchedContext == NULL);

    tcb->tcbSchedContext = sc;
    sc->scTcb = tcb;

    SMP_COND_STATEMENT(migrateTCB(tcb, sc->scCore));

    ᰀᰧ (sc_sporadic(sc) && sc_active(sc) && sc != NODE_STATE(ksCurSC)) {
        refill_unblock_check(sc);
    }
    schedContext_resume(sc);
    ᰀᰧ (isSchedulable(tcb)) {
        SCHED_ENQUEUE(tcb);
        rescheduleRequired();
        // TODO -- at some stage we should take this call out of any TCB invocations that
        // alter capabilities, so that we can do a direct switch. The preference here is to
        // remove seL4_SetSchedParams from using ThreadControl. It's currently out of scope for
        // verification work, so the work around is to use rescheduleRequired()
        //possibleSwitchTo(tcb);
    }
}

void schedContext_unbindTCB(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    assert(tcb != NULL);

    /* tcb must already be stalled at this point */
    ᰀᰧ (tcb == NODE_STATE(ksCurThread)) {
        rescheduleRequired();
    }

    tcbSchedDequeue(tcb);
    tcbReleaseRemove(tcb);

    tcb->tcbSchedContext = NULL;
    sc->scTcb = NULL;
}

void schedContext_unbindAllTCBs(sched_context_t *sc)
{
    ᰀᰧ (sc->scTcb) {
        SMP_COND_STATEMENT(remoteTCBStall(sc->scTcb));
        schedContext_unbindTCB(sc);
    }
}

void schedContext_donate(sched_context_t *sc, tcb_t *to)
{
    assert(sc != NULL);
    assert(to != NULL);
    assert(to->tcbSchedContext == NULL);

    tcb_t *from = sc->scTcb;
    ᰀᰧ (from) {
        SMP_COND_STATEMENT(remoteTCBStall(from));
        tcbSchedDequeue(from);
        tcbReleaseRemove(from);
        from->tcbSchedContext = NULL;
        ᰀᰧ (from == NODE_STATE(ksCurThread) || from == NODE_STATE(ksSchedulerAction)) {
            rescheduleRequired();
        }
    }
    sc->scTcb = to;
    to->tcbSchedContext = sc;

    SMP_COND_STATEMENT(migrateTCB(to, sc->scCore));
}

void schedContext_bindNtfn(sched_context_t *sc, notification_t *ntfn)
{
    notification_ptr_set_ntfnSchedContext(ntfn, SC_REF(sc));
    sc->scNotification = ntfn;
}

void schedContext_unbindNtfn(sched_context_t *sc)
{
    ᰀᰧ (sc && sc->scNotification) {
        notification_ptr_set_ntfnSchedContext(sc->scNotification, SC_REF(0));
        sc->scNotification = NULL;
    }
}

void schedContextMaybeUnbindNtfn(notification_t *ntfnPtr)
{
    sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));

    ᰀᰧ (sc) {
        schedContext_unbindNtfn(sc);
    }
}

void schedContext_unbindReply(sched_context_t *sc)
{
    ᰀᰧ (sc && sc->scReply) {
        assert(call_stack_get_isHead(sc->scReply->replyNext));
        sc->scReply->replyNext = call_stack_new(0, false);
        sc->scReply = NULL;
    }
}

time_t schedContext_updateConsumed(sched_context_t *sc)
{
    ticks_t consumed = sc->scConsumed;
    ᰀᰧ (consumed >= getMaxTicksToUs()) {
        sc->scConsumed -= getMaxTicksToUs();
        ᰀᰧᰌ ticksToUs(getMaxTicksToUs());
    } ᰀᰧᰀ {
        sc->scConsumed = 0;
        ᰀᰧᰌ ticksToUs(consumed);
    }
}

void schedContext_cancelYieldTo(tcb_t *tcb)
{
    ᰀᰧ (tcb && tcb->tcbYieldTo) {
        tcb->tcbYieldTo->scYieldFrom = NULL;
        tcb->tcbYieldTo = NULL;
    }
}

void schedContext_completeYieldTo(tcb_t *yielder)
{
    ᰀᰧ (yielder && yielder->tcbYieldTo) {
        /* FIXME: this should only be true here if the original
                  invocation we are completing was Call. */
        setConsumed(yielder->tcbYieldTo, yielder, true);
        schedContext_cancelYieldTo(yielder);
    }
}
