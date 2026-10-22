/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/tcb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/endpoint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <object/notification.h>

ਸਥਿਰ ਸਤਰਅੰਦਰ tcb_ਕਤਾਰ_t ਸ਼ੁੱਧ ntfn_ptr_get_ਕਤਾਰ(notification_t *ntfnPtr)
{
    tcb_ਕਤਾਰ_t ntfn_ਕਤਾਰ;

    ntfn_ਕਤਾਰ.ਸਿਰ = (tcb_t *)notification_ptr_get_ntfn_ਕਤਾਰ_ਸਿਰ(ntfnPtr);
    ntfn_ਕਤਾਰ.ਸਿਰਾ = (tcb_t *)notification_ptr_get_ntfn_ਕਤਾਰ_ਪੂਛ(ntfnPtr);

    ਵਾਪਸ ntfn_ਕਤਾਰ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ntfn_ptr_ਸੈੱਟ_ਕਤਾਰ(notification_t *ntfnPtr, tcb_ਕਤਾਰ_t ntfn_ਕਤਾਰ)
{
    notification_ptr_ਸੈੱਟ_ntfn_ਕਤਾਰ_ਸਿਰ(ntfnPtr, (ਸ਼ਬਦ_t)ntfn_ਕਤਾਰ.ਸਿਰ);
    notification_ptr_ਸੈੱਟ_ntfn_ਕਤਾਰ_ਪੂਛ(ntfnPtr, (ਸ਼ਬਦ_t)ntfn_ਕਤਾਰ.ਸਿਰਾ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ maybeDonateSchedContext(tcb_t *tcb, notification_t *ntfnPtr)
{
    ਜੇ (tcb->tcbSchedContext == ਨਿਰਾ) {
        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        ਜੇ (sc != ਨਿਰਾ && sc->scTcb == ਨਿਰਾ) {
            schedContext_donate(sc, tcb);
            schedContext_resume(sc);
        }
    }
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MCS_DO_IF_SC(tcb, ntfnPtr, ਖੰਡ) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    ਜੇ (isSchedulable(tcb)) { \
        ਖੰਡ \
    }
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MCS_DO_IF_SC(tcb, ntfnPtr, ਖੰਡ) \
    { \
        ਖੰਡ \
    }
#ਜੇ_ਅੰਤ

ਖਾਲੀ sendSignal(notification_t *ntfnPtr, ਸ਼ਬਦ_t badge)
{
    ਚੋਣ (notification_ptr_get_state(ntfnPtr)) {
    ਹਾਲਤ NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        ਜੇ (tcb) {
            ਜੇ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                ਸੈੱਟ_Thread_State(tcb, ThreadState_Running);
                ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, badge_ਰਜਿਸਟਰ, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possible_Switch_ਮੰਜ਼ਿਲ(tcb);
                })
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
                ਜੇ (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    ਪ੍ਰਤਿਜ੍ਞਾ(tcb->tcbSchedContext != ਗੰਢ_STATE(ksCurSC));
                    ਜੇ (tcb->tcbSchedContext != ਗੰਢ_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
            } ਨਹੀਂਤਾਂ ਜੇ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
                ਜੇ (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_ਸੈੱਟ_ਸਰਗਰਮ(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
                {
                    ਸੈੱਟ_Thread_State(tcb, ThreadState_Running);
                    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, badge_ਰਜਿਸਟਰ, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possible_Switch_ਮੰਜ਼ਿਲ(tcb);
                    })
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
                    ਜੇ (tcb->tcbSchedContext != ਨਿਰਾ && sc_ਸਰਗਰਮ(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        ਜੇ (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != ਗੰਢ_STATE(ksCurSC)) {
                            /* We know that the tcb can't have the current SC
                             * as its own SC as this point as it should still be
                             * associated with the current thread, or no thread.
                             * This check is added here to reduce the cost of
                             * proving this to be true as a short-term stop-gap. */
                            /* Only unblock if the SC was donated from the
                             * notification */
                            refill_unblock_check(tcb->tcbSchedContext);
                        }
                    }
#ਜੇ_ਅੰਤ
                }
#ਜੇ_ਅੰਤ /* CONFIG_VTX */
            } ਨਹੀਂਤਾਂ {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_ਸੈੱਟ_ਸਰਗਰਮ(ntfnPtr, badge);
            }
        } ਨਹੀਂਤਾਂ {
            ntfn_ਸੈੱਟ_ਸਰਗਰਮ(ntfnPtr, badge);
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ Ntfn_State_ਉਡੀਕ: {
        tcb_ਕਤਾਰ_t ntfn_ਕਤਾਰ;
        tcb_t *dest;

        ntfn_ਕਤਾਰ = ntfn_ptr_get_ਕਤਾਰ(ntfnPtr);
        dest = ntfn_ਕਤਾਰ.ਸਿਰ;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        ਪ੍ਰਤਿਜ੍ਞਾ(dest);

        /* Dequeue TCB */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#ਨਹੀਂਤਾਂ
        ntfn_ਕਤਾਰ = tcbEPDequeue(dest, ntfn_ਕਤਾਰ);
        ntfn_ptr_ਸੈੱਟ_ਕਤਾਰ(ntfnPtr, ntfn_ਕਤਾਰ);

        /* set the thread state to idle if the queue is empty */
        ਜੇ (!ntfn_ਕਤਾਰ.ਸਿਰ) {
            notification_ptr_ਸੈੱਟ_state(ntfnPtr, NtfnState_Idle);
        }
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */

        ਸੈੱਟ_Thread_State(dest, ThreadState_Running);
        ਸੈੱਟ_ਰਜਿਸਟਰ(dest, badge_ਰਜਿਸਟਰ, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possible_Switch_ਮੰਜ਼ਿਲ(dest);
        })

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਜੇ (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            ਪ੍ਰਤਿਜ੍ਞਾ(dest->tcbSchedContext != ਗੰਢ_STATE(ksCurSC));
            ਜੇ (dest->tcbSchedContext != ਗੰਢ_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#ਜੇ_ਅੰਤ
        ਰੋਕੋ;
    }

    ਹਾਲਤ Ntfn_State_ਸਰਗਰਮ: {
        ਸ਼ਬਦ_t badge2;

        badge2 = notification_ptr_get_ntfn_ਸੁਨੇਹਾ_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_ਸੈੱਟ_ntfn_ਸੁਨੇਹਾ_Identifier(ntfnPtr, badge2);
        ਰੋਕੋ;
    }
    }
}

ਖਾਲੀ receiveSignal(tcb_t *thread, cap_t cap, bool_t isBlocking)
{
    notification_t *ntfnPtr;

    ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    ਚੋਣ (notification_ptr_get_state(ntfnPtr)) {
    ਹਾਲਤ NtfnState_Idle:
    ਹਾਲਤ Ntfn_State_ਉਡੀਕ: {

        ਜੇ (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_ਸੈੱਟ_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_ਸੈੱਟ_blocking_Object(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            ਸਮਾਂ_ਸਾਰਣੀ_TCB(thread);

            /* Enqueue TCB */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#ਨਹੀਂਤਾਂ
            tcb_ਕਤਾਰ_t ntfn_ਕਤਾਰ;
            ntfn_ਕਤਾਰ = ntfn_ptr_get_ਕਤਾਰ(ntfnPtr);
            ntfn_ਕਤਾਰ = tcbEPAppend(thread, ntfn_ਕਤਾਰ);

            notification_ptr_ਸੈੱਟ_state(ntfnPtr, Ntfn_State_ਉਡੀਕ);
            ntfn_ptr_ਸੈੱਟ_ਕਤਾਰ(ntfnPtr, ntfn_ਕਤਾਰ);
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#ਜੇ_ਅੰਤ
        } ਨਹੀਂਤਾਂ {
            doNBRecvFailedTransfer(thread);
        }

        ਰੋਕੋ;
    }

    ਹਾਲਤ Ntfn_State_ਸਰਗਰਮ:
        ਸੈੱਟ_ਰਜਿਸਟਰ(
            thread, badge_ਰਜਿਸਟਰ,
            notification_ptr_get_ntfn_ਸੁਨੇਹਾ_Identifier(ntfnPtr));
        notification_ptr_ਸੈੱਟ_state(ntfnPtr, NtfnState_Idle);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        ਜੇ (thread->tcbSchedContext != ਗੰਢ_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#ਜੇ_ਅੰਤ
        ਰੋਕੋ;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    ਸੈੱਟ_Thread_State(thread, ThreadState_Restart);
    ਜੇ (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        ਪ੍ਰਤਿਜ੍ਞਾ(thread->tcbSchedContext != ਗੰਢ_STATE(ksCurSC));
        ਜੇ (thread->tcbSchedContext != ਗੰਢ_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possible_Switch_ਮੰਜ਼ਿਲ(thread);
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ cancel_All_ਸੰਕੇਤ(notification_t *ntfnPtr)
{
    ਜੇ (notification_ptr_get_state(ntfnPtr) == Ntfn_State_ਉਡੀਕ) {
        /* Clear the queue and set all blocked threads to Restart */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        tcb_ਕਤਾਰ_t ਕਤਾਰ;
        tcb_t *thread, *ਅਗਲਾ;

        ਕਤਾਰ = ntfn_ptr_get_ਕਤਾਰ(ntfnPtr);

        ਲਈ (thread = ਕਤਾਰ.ਸਿਰ; thread; thread = ਅਗਲਾ) {
            ਅਗਲਾ = thread->tcb_Sched_ਅਗਲਾ;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#ਨਹੀਂਤਾਂ
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfn_ਕਤਾਰ_ਸਿਰ(ntfnPtr));

        notification_ptr_ਸੈੱਟ_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_ਸੈੱਟ_ntfn_ਕਤਾਰ_ਸਿਰ(ntfnPtr, 0);
        notification_ptr_ਸੈੱਟ_ntfn_ਕਤਾਰ_ਪੂਛ(ntfnPtr, 0);

        ਲਈ (; thread; thread = thread->tcbEPNext) {
            ਸੈੱਟ_Thread_State(thread, ThreadState_Restart);
            SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(thread);
        }
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

ਖਾਲੀ cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    ਪ੍ਰਤਿਜ੍ਞਾ(notification_ptr_get_state(ntfnPtr) == Ntfn_State_ਉਡੀਕ);

    /* Dequeue TCB */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#ਨਹੀਂਤਾਂ
    tcb_ਕਤਾਰ_t ntfn_ਕਤਾਰ;
    ntfn_ਕਤਾਰ = ntfn_ptr_get_ਕਤਾਰ(ntfnPtr);
    ntfn_ਕਤਾਰ = tcbEPDequeue(threadPtr, ntfn_ਕਤਾਰ);
    ntfn_ptr_ਸੈੱਟ_ਕਤਾਰ(ntfnPtr, ntfn_ਕਤਾਰ);

    /* Make notification object idle */
    ਜੇ (!ntfn_ਕਤਾਰ.ਸਿਰ) {
        notification_ptr_ਸੈੱਟ_state(ntfnPtr, NtfnState_Idle);
    }
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    ਸੈੱਟ_Thread_State(threadPtr, ThreadState_Inactive);
}

ਖਾਲੀ completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    ਸ਼ਬਦ_t badge;

    ਜੇ (likely(tcb && notification_ptr_get_state(ntfnPtr) == Ntfn_State_ਸਰਗਰਮ)) {
        badge = notification_ptr_get_ntfn_ਸੁਨੇਹਾ_Identifier(ntfnPtr);
        ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, badge_ਰਜਿਸਟਰ, badge);
        notification_ptr_ਸੈੱਟ_state(ntfnPtr, NtfnState_Idle);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        ਜੇ (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            ਜੇ (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != ਗੰਢ_STATE(ksCurSC)) {
                /* We know that the tcb can't have the current SC
                 * as its own SC as this point as it should still be
                 * associated with the current thread, or no thread.
                 * This check is added here to reduce the cost of
                 * proving this to be true as a short-term stop-gap. */
                /* Only unblock if the SC was donated from the
                 * notification */
                refill_unblock_check(tcb->tcbSchedContext);
            }
        }
#ਜੇ_ਅੰਤ
    } ਨਹੀਂਤਾਂ {
        fail("tried to complete signal with inactive notification object");
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_ਸੈੱਟ_ntfn_Bound_TCB(ntfnPtr, (ਸ਼ਬਦ_t) 0);
    tcbptr->tcbBoundNotification = ਨਿਰਾ;
}

ਖਾਲੀ unbindMaybeNotification(notification_t *ntfnPtr)
{
    tcb_t *boundTCB;
    boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);

    ਜੇ (boundTCB) {
        doUnbindNotification(ntfnPtr, boundTCB);
    }
}

ਖਾਲੀ unbindNotification(tcb_t *tcb)
{
    notification_t *ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    ਜੇ (ntfnPtr) {
        doUnbindNotification(ntfnPtr, tcb);
    }
}

ਖਾਲੀ bindNotification(tcb_t *tcb, notification_t *ntfnPtr)
{
    notification_ptr_ਸੈੱਟ_ntfn_Bound_TCB(ntfnPtr, (ਸ਼ਬਦ_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_ਕਤਾਰ_t ਕਤਾਰ;
    tcb_ਕਤਾਰ_t new_ਕਤਾਰ;

    ਕਤਾਰ = ntfn_ptr_get_ਕਤਾਰ(ntfnPtr);
    new_ਕਤਾਰ = tcbAppend(thread, ਕਤਾਰ);
    ntfn_ptr_ਸੈੱਟ_ਕਤਾਰ(ntfnPtr, new_ਕਤਾਰ);
    notification_ptr_ਸੈੱਟ_state(ntfnPtr, Ntfn_State_ਉਡੀਕ);
}

ਖਾਲੀ tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_ਕਤਾਰ_t ਕਤਾਰ;
    tcb_ਕਤਾਰ_t new_ਕਤਾਰ;

    ਕਤਾਰ = ntfn_ptr_get_ਕਤਾਰ(ntfnPtr);
    new_ਕਤਾਰ = tcb_ਕਤਾਰ_remove(ਕਤਾਰ, thread);
    ntfn_ptr_ਸੈੱਟ_ਕਤਾਰ(ntfnPtr, new_ਕਤਾਰ);

    ਜੇ (tcb_ਕਤਾਰ_ਖਾਲੀ_ਥਾਵਾਂ(new_ਕਤਾਰ)) {
        notification_ptr_ਸੈੱਟ_state(ntfnPtr, NtfnState_Idle);
    }
}

ਖਾਲੀ reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_ਕਤਾਰ_t ਕਤਾਰ = ntfn_ptr_get_ਕਤਾਰ(ntfnPtr);
    ਕਤਾਰ = tcb_ਕਤਾਰ_remove(ਕਤਾਰ, thread);
    ਕਤਾਰ = tcbAppend(thread, ਕਤਾਰ);
    ntfn_ptr_ਸੈੱਟ_ਕਤਾਰ(ntfnPtr, ਕਤਾਰ);
}
#ਜੇ_ਅੰਤ
