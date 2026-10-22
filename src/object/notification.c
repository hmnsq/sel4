/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/tcb.h>
#ಅನ್ತರ್ಭಾವ <object/endpoint.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>

#ಅನ್ತರ್ಭಾವ <object/notification.h>

ಸ್ಥಿರ ಸಾಲೊಳಗೆ tcb_ಸರತಿ_t ಶುದ್ಧ ntfn_ptr_get_ಸರತಿ(notification_t *ntfnPtr)
{
    tcb_ಸರತಿ_t ntfn_ಸರತಿ;

    ntfn_ಸರತಿ.ತಲೆ = (tcb_t *)notification_ptr_get_ntfn_ಸರತಿ_ತಲೆ(ntfnPtr);
    ntfn_ಸರತಿ.ಕೊನೆ = (tcb_t *)notification_ptr_get_ntfn_ಸರತಿ_ಬಾಲ(ntfnPtr);

    ಹಿಂತಿರುಗಿ ntfn_ಸರತಿ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ntfn_ptr_ಗಣ_ಸರತಿ(notification_t *ntfnPtr, tcb_ಸರತಿ_t ntfn_ಸರತಿ)
{
    notification_ptr_ಗಣ_ntfn_ಸರತಿ_ತಲೆ(ntfnPtr, (ಶಬ್ದ_t)ntfn_ಸರತಿ.ತಲೆ);
    notification_ptr_ಗಣ_ntfn_ಸರತಿ_ಬಾಲ(ntfnPtr, (ಶಬ್ದ_t)ntfn_ಸರತಿ.ಕೊನೆ);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ maybeDonateSchedContext(tcb_t *tcb, notification_t *ntfnPtr)
{
    ಆದರೆ (tcb->tcbSchedContext == ಖಾಲಿ) {
        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        ಆದರೆ (sc != ಖಾಲಿ && sc->scTcb == ಖಾಲಿ) {
            schedContext_donate(sc, tcb);
            schedContext_resume(sc);
        }
    }
}

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
#ವ್ಯಾಖ್ಯಾನಿಸು MCS_DO_IF_SC(tcb, ntfnPtr, ಖಂಡ) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    ಆದರೆ (isSchedulable(tcb)) { \
        ಖಂಡ \
    }
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು MCS_DO_IF_SC(tcb, ntfnPtr, ಖಂಡ) \
    { \
        ಖಂಡ \
    }
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ sendSignal(notification_t *ntfnPtr, ಶಬ್ದ_t badge)
{
    ಆಯ್ಕೆ (notification_ptr_get_state(ntfnPtr)) {
    ಸ್ಥಿತಿ NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        ಆದರೆ (tcb) {
            ಆದರೆ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                ಗಣ_Thread_State(tcb, ThreadState_Running);
                ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, badge_ರಿಜಿಸ್ಟರ್, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possible_Switch_ಗಮ್ಯ(tcb);
                })
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
                ಆದರೆ (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    ಪ್ರತಿಜ್ಞಾ(tcb->tcbSchedContext != ಗಂಟು_STATE(ksCurSC));
                    ಆದರೆ (tcb->tcbSchedContext != ಗಂಟು_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
                ಆದರೆ (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_ಗಣ_ಸಕ್ರಿಯ(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
                {
                    ಗಣ_Thread_State(tcb, ThreadState_Running);
                    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, badge_ರಿಜಿಸ್ಟರ್, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possible_Switch_ಗಮ್ಯ(tcb);
                    })
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
                    ಆದರೆ (tcb->tcbSchedContext != ಖಾಲಿ && sc_ಸಕ್ರಿಯ(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        ಆದರೆ (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != ಗಂಟು_STATE(ksCurSC)) {
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
#ಆದರೆ_ಅಂತ್ಯ
                }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_VTX */
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_ಗಣ_ಸಕ್ರಿಯ(ntfnPtr, badge);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ntfn_ಗಣ_ಸಕ್ರಿಯ(ntfnPtr, badge);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ Ntfn_State_ಕಾಯುವಿಕೆ: {
        tcb_ಸರತಿ_t ntfn_ಸರತಿ;
        tcb_t *dest;

        ntfn_ಸರತಿ = ntfn_ptr_get_ಸರತಿ(ntfnPtr);
        dest = ntfn_ಸರತಿ.ತಲೆ;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        ಪ್ರತಿಜ್ಞಾ(dest);

        /* Dequeue TCB */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#ಇಲ್ಲದಿದ್ದರೆ
        ntfn_ಸರತಿ = tcbEPDequeue(dest, ntfn_ಸರತಿ);
        ntfn_ptr_ಗಣ_ಸರತಿ(ntfnPtr, ntfn_ಸರತಿ);

        /* set the thread state to idle if the queue is empty */
        ಆದರೆ (!ntfn_ಸರತಿ.ತಲೆ) {
            notification_ptr_ಗಣ_state(ntfnPtr, NtfnState_Idle);
        }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */

        ಗಣ_Thread_State(dest, ThreadState_Running);
        ಗಣ_ರಿಜಿಸ್ಟರ್(dest, badge_ರಿಜಿಸ್ಟರ್, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possible_Switch_ಗಮ್ಯ(dest);
        })

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಆದರೆ (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            ಪ್ರತಿಜ್ಞಾ(dest->tcbSchedContext != ಗಂಟು_STATE(ksCurSC));
            ಆದರೆ (dest->tcbSchedContext != ಗಂಟು_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#ಆದರೆ_ಅಂತ್ಯ
        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ Ntfn_State_ಸಕ್ರಿಯ: {
        ಶಬ್ದ_t badge2;

        badge2 = notification_ptr_get_ntfn_ಸಂದೇಶ_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_ಗಣ_ntfn_ಸಂದೇಶ_Identifier(ntfnPtr, badge2);
        ನಿಲ್ಲಿಸು;
    }
    }
}

ಶೂನ್ಯ receiveSignal(tcb_t *thread, cap_t cap, bool_t isBlocking)
{
    notification_t *ntfnPtr;

    ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    ಆಯ್ಕೆ (notification_ptr_get_state(ntfnPtr)) {
    ಸ್ಥಿತಿ NtfnState_Idle:
    ಸ್ಥಿತಿ Ntfn_State_ಕಾಯುವಿಕೆ: {

        ಆದರೆ (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_ಗಣ_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_ಗಣ_blocking_Object(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            ವೇಳಾಪಟ್ಟಿ_TCB(thread);

            /* Enqueue TCB */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#ಇಲ್ಲದಿದ್ದರೆ
            tcb_ಸರತಿ_t ntfn_ಸರತಿ;
            ntfn_ಸರತಿ = ntfn_ptr_get_ಸರತಿ(ntfnPtr);
            ntfn_ಸರತಿ = tcbEPAppend(thread, ntfn_ಸರತಿ);

            notification_ptr_ಗಣ_state(ntfnPtr, Ntfn_State_ಕಾಯುವಿಕೆ);
            ntfn_ptr_ಗಣ_ಸರತಿ(ntfnPtr, ntfn_ಸರತಿ);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#ಆದರೆ_ಅಂತ್ಯ
        } ಇಲ್ಲದಿದ್ದರೆ {
            doNBRecvFailedTransfer(thread);
        }

        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ Ntfn_State_ಸಕ್ರಿಯ:
        ಗಣ_ರಿಜಿಸ್ಟರ್(
            thread, badge_ರಿಜಿಸ್ಟರ್,
            notification_ptr_get_ntfn_ಸಂದೇಶ_Identifier(ntfnPtr));
        notification_ptr_ಗಣ_state(ntfnPtr, NtfnState_Idle);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        ಆದರೆ (thread->tcbSchedContext != ಗಂಟು_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#ಆದರೆ_ಅಂತ್ಯ
        ನಿಲ್ಲಿಸು;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    ಗಣ_Thread_State(thread, ThreadState_Restart);
    ಆದರೆ (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        ಪ್ರತಿಜ್ಞಾ(thread->tcbSchedContext != ಗಂಟು_STATE(ksCurSC));
        ಆದರೆ (thread->tcbSchedContext != ಗಂಟು_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possible_Switch_ಗಮ್ಯ(thread);
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ cancel_All_ಸಂಕೇತಗಳು(notification_t *ntfnPtr)
{
    ಆದರೆ (notification_ptr_get_state(ntfnPtr) == Ntfn_State_ಕಾಯುವಿಕೆ) {
        /* Clear the queue and set all blocked threads to Restart */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        tcb_ಸರತಿ_t ಸರತಿ;
        tcb_t *thread, *ಮುಂದಿನ;

        ಸರತಿ = ntfn_ptr_get_ಸರತಿ(ntfnPtr);

        ಗಾಗಿ (thread = ಸರತಿ.ತಲೆ; thread; thread = ಮುಂದಿನ) {
            ಮುಂದಿನ = thread->tcb_Sched_ಮುಂದಿನ;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#ಇಲ್ಲದಿದ್ದರೆ
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfn_ಸರತಿ_ತಲೆ(ntfnPtr));

        notification_ptr_ಗಣ_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_ಗಣ_ntfn_ಸರತಿ_ತಲೆ(ntfnPtr, 0);
        notification_ptr_ಗಣ_ntfn_ಸರತಿ_ಬಾಲ(ntfnPtr, 0);

        ಗಾಗಿ (; thread; thread = thread->tcbEPNext) {
            ಗಣ_Thread_State(thread, ThreadState_Restart);
            SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(thread);
        }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

ಶೂನ್ಯ cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    ಪ್ರತಿಜ್ಞಾ(notification_ptr_get_state(ntfnPtr) == Ntfn_State_ಕಾಯುವಿಕೆ);

    /* Dequeue TCB */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#ಇಲ್ಲದಿದ್ದರೆ
    tcb_ಸರತಿ_t ntfn_ಸರತಿ;
    ntfn_ಸರತಿ = ntfn_ptr_get_ಸರತಿ(ntfnPtr);
    ntfn_ಸರತಿ = tcbEPDequeue(threadPtr, ntfn_ಸರತಿ);
    ntfn_ptr_ಗಣ_ಸರತಿ(ntfnPtr, ntfn_ಸರತಿ);

    /* Make notification object idle */
    ಆದರೆ (!ntfn_ಸರತಿ.ತಲೆ) {
        notification_ptr_ಗಣ_state(ntfnPtr, NtfnState_Idle);
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    ಗಣ_Thread_State(threadPtr, ThreadState_Inactive);
}

ಶೂನ್ಯ completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    ಶಬ್ದ_t badge;

    ಆದರೆ (likely(tcb && notification_ptr_get_state(ntfnPtr) == Ntfn_State_ಸಕ್ರಿಯ)) {
        badge = notification_ptr_get_ntfn_ಸಂದೇಶ_Identifier(ntfnPtr);
        ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, badge_ರಿಜಿಸ್ಟರ್, badge);
        notification_ptr_ಗಣ_state(ntfnPtr, NtfnState_Idle);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        ಆದರೆ (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            ಆದರೆ (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != ಗಂಟು_STATE(ksCurSC)) {
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
#ಆದರೆ_ಅಂತ್ಯ
    } ಇಲ್ಲದಿದ್ದರೆ {
        fail("tried to complete signal with inactive notification object");
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_ಗಣ_ntfn_Bound_TCB(ntfnPtr, (ಶಬ್ದ_t) 0);
    tcbptr->tcbBoundNotification = ಖಾಲಿ;
}

ಶೂನ್ಯ unbindMaybeNotification(notification_t *ntfnPtr)
{
    tcb_t *boundTCB;
    boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);

    ಆದರೆ (boundTCB) {
        doUnbindNotification(ntfnPtr, boundTCB);
    }
}

ಶೂನ್ಯ unbindNotification(tcb_t *tcb)
{
    notification_t *ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    ಆದರೆ (ntfnPtr) {
        doUnbindNotification(ntfnPtr, tcb);
    }
}

ಶೂನ್ಯ bindNotification(tcb_t *tcb, notification_t *ntfnPtr)
{
    notification_ptr_ಗಣ_ntfn_Bound_TCB(ntfnPtr, (ಶಬ್ದ_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_ಸರತಿ_t ಸರತಿ;
    tcb_ಸರತಿ_t new_ಸರತಿ;

    ಸರತಿ = ntfn_ptr_get_ಸರತಿ(ntfnPtr);
    new_ಸರತಿ = tcbAppend(thread, ಸರತಿ);
    ntfn_ptr_ಗಣ_ಸರತಿ(ntfnPtr, new_ಸರತಿ);
    notification_ptr_ಗಣ_state(ntfnPtr, Ntfn_State_ಕಾಯುವಿಕೆ);
}

ಶೂನ್ಯ tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_ಸರತಿ_t ಸರತಿ;
    tcb_ಸರತಿ_t new_ಸರತಿ;

    ಸರತಿ = ntfn_ptr_get_ಸರತಿ(ntfnPtr);
    new_ಸರತಿ = tcb_ಸರತಿ_remove(ಸರತಿ, thread);
    ntfn_ptr_ಗಣ_ಸರತಿ(ntfnPtr, new_ಸರತಿ);

    ಆದರೆ (tcb_ಸರತಿ_ಖಾಲಿಗಳು(new_ಸರತಿ)) {
        notification_ptr_ಗಣ_state(ntfnPtr, NtfnState_Idle);
    }
}

ಶೂನ್ಯ reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_ಸರತಿ_t ಸರತಿ = ntfn_ptr_get_ಸರತಿ(ntfnPtr);
    ಸರತಿ = tcb_ಸರತಿ_remove(ಸರತಿ, thread);
    ಸರತಿ = tcbAppend(thread, ಸರತಿ);
    ntfn_ptr_ಗಣ_ಸರತಿ(ntfnPtr, ಸರತಿ);
}
#ಆದರೆ_ಅಂತ್ಯ
