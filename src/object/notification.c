/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>

#अन्तर्भाव <types.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <object/tcb.h>
#अन्तर्भाव <object/endpoint.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <machine/io.h>

#अन्तर्भाव <object/notification.h>

स्थिर अन्तःस्थ tcb_queue_t PURE ntfn_ptr_get_queue(notification_t *ntfnPtr)
{
    tcb_queue_t ntfn_queue;

    ntfn_queue.head = (tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr);
    ntfn_queue.end = (tcb_t *)notification_ptr_get_ntfnQueue_tail(ntfnPtr);

    प्रत्यागम ntfn_queue;
}

स्थिर अन्तःस्थ शून्य ntfn_ptr_set_queue(notification_t *ntfnPtr, tcb_queue_t ntfn_queue)
{
    notification_ptr_set_ntfnQueue_head(ntfnPtr, (word_t)ntfn_queue.head);
    notification_ptr_set_ntfnQueue_tail(ntfnPtr, (word_t)ntfn_queue.end);
}

#ifdef CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ शून्य maybeDonateSchedContext(tcb_t *tcb, notification_t *ntfnPtr)
{
    जइ (tcb->tcbSchedContext == रिक्त) {
        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        जइ (sc != रिक्त && sc->scTcb == रिक्त) {
            schedContext_donate(sc, tcb);
            schedContext_resume(sc);
        }
    }
}

#endif

#ifdef CONFIG_KERNEL_MCS
#define MCS_DO_IF_SC(tcb, ntfnPtr, _block) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    जइ (isSchedulable(tcb)) { \
        _block \
    }
#अण्णहा
#define MCS_DO_IF_SC(tcb, ntfnPtr, _block) \
    { \
        _block \
    }
#endif

शून्य sendSignal(notification_t *ntfnPtr, word_t badge)
{
    विकल्प (notification_ptr_get_state(ntfnPtr)) {
    पक्ष NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        जइ (tcb) {
            जइ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                setThreadState(tcb, ThreadState_Running);
                setRegister(tcb, badgeRegister, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possibleSwitchTo(tcb);
                })
#ifdef CONFIG_KERNEL_MCS
                जइ (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    प्रतिज्ञा(tcb->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
                    जइ (tcb->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#endif
#ifdef CONFIG_VTX
            } अण्णहा जइ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#ifdef ENABLE_SMP_SUPPORT
                जइ (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_set_active(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } अण्णहा
#endif /* ENABLE_SMP_SUPPORT */
                {
                    setThreadState(tcb, ThreadState_Running);
                    setRegister(tcb, badgeRegister, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possibleSwitchTo(tcb);
                    })
#ifdef CONFIG_KERNEL_MCS
                    जइ (tcb->tcbSchedContext != रिक्त && sc_active(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        जइ (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
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
#endif
                }
#endif /* CONFIG_VTX */
            } अण्णहा {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_set_active(ntfnPtr, badge);
            }
        } अण्णहा {
            ntfn_set_active(ntfnPtr, badge);
        }
        भङ्ग;
    }
    पक्ष NtfnState_Waiting: {
        tcb_queue_t ntfn_queue;
        tcb_t *dest;

        ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
        dest = ntfn_queue.head;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        प्रतिज्ञा(dest);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#अण्णहा
        ntfn_queue = tcbEPDequeue(dest, ntfn_queue);
        ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);

        /* set the thread state to idle if the queue is empty */
        जइ (!ntfn_queue.head) {
            notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        setThreadState(dest, ThreadState_Running);
        setRegister(dest, badgeRegister, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possibleSwitchTo(dest);
        })

#ifdef CONFIG_KERNEL_MCS
        जइ (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            प्रतिज्ञा(dest->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
            जइ (dest->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#endif
        भङ्ग;
    }

    पक्ष NtfnState_Active: {
        word_t badge2;

        badge2 = notification_ptr_get_ntfn_सन्देशः_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_set_ntfn_सन्देशः_Identifier(ntfnPtr, badge2);
        भङ्ग;
    }
    }
}

शून्य receiveSignal(tcb_t *thread, cap_t cap, bool_t isBlocking)
{
    notification_t *ntfnPtr;

    ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    विकल्प (notification_ptr_get_state(ntfnPtr)) {
    पक्ष NtfnState_Idle:
    पक्ष NtfnState_Waiting: {

        जइ (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_set_tsType(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_set_blockingObject(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            scheduleTCB(thread);

            /* Enqueue TCB */
#ifdef CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#अण्णहा
            tcb_queue_t ntfn_queue;
            ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
            ntfn_queue = tcbEPAppend(thread, ntfn_queue);

            notification_ptr_set_state(ntfnPtr, NtfnState_Waiting);
            ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#endif
        } अण्णहा {
            doNBRecvFailedTransfer(thread);
        }

        भङ्ग;
    }

    पक्ष NtfnState_Active:
        setRegister(
            thread, badgeRegister,
            notification_ptr_get_ntfn_सन्देशः_Identifier(ntfnPtr));
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
#ifdef CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        जइ (thread->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#endif
        भङ्ग;
    }
}

#ifdef CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ शून्य removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    setThreadState(thread, ThreadState_Restart);
    जइ (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        प्रतिज्ञा(thread->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
        जइ (thread->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possibleSwitchTo(thread);
}
#endif

शून्य cancelAllSignals(notification_t *ntfnPtr)
{
    जइ (notification_ptr_get_state(ntfnPtr) == NtfnState_Waiting) {
        /* Clear the queue and set all blocked threads to Restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *next;

        queue = ntfn_ptr_get_queue(ntfnPtr);

        प्रति (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#अण्णहा
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfnQueue_head(ntfnPtr));

        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_set_ntfnQueue_head(ntfnPtr, 0);
        notification_ptr_set_ntfnQueue_tail(ntfnPtr, 0);

        प्रति (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

शून्य cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    प्रतिज्ञा(notification_ptr_get_state(ntfnPtr) == NtfnState_Waiting);

    /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#अण्णहा
    tcb_queue_t ntfn_queue;
    ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
    ntfn_queue = tcbEPDequeue(threadPtr, ntfn_queue);
    ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);

    /* Make notification object idle */
    जइ (!ntfn_queue.head) {
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
    }
#endif /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    setThreadState(threadPtr, ThreadState_Inactive);
}

शून्य completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    word_t badge;

    जइ (likely(tcb && notification_ptr_get_state(ntfnPtr) == NtfnState_Active)) {
        badge = notification_ptr_get_ntfn_सन्देशः_Identifier(ntfnPtr);
        setRegister(tcb, badgeRegister, badge);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
#ifdef CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        जइ (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            जइ (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
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
#endif
    } अण्णहा {
        fail("tried to complete signal with inactive notification object");
    }
}

स्थिर अन्तःस्थ शून्य doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_set_ntfnBoundTCB(ntfnPtr, (word_t) 0);
    tcbptr->tcbBoundNotification = रिक्त;
}

शून्य unbindMaybeNotification(notification_t *ntfnPtr)
{
    tcb_t *boundTCB;
    boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);

    जइ (boundTCB) {
        doUnbindNotification(ntfnPtr, boundTCB);
    }
}

शून्य unbindNotification(tcb_t *tcb)
{
    notification_t *ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    जइ (ntfnPtr) {
        doUnbindNotification(ntfnPtr, tcb);
    }
}

शून्य bindNotification(tcb_t *tcb, notification_t *ntfnPtr)
{
    notification_ptr_set_ntfnBoundTCB(ntfnPtr, (word_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#ifdef CONFIG_KERNEL_MCS
शून्य tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcbAppend(thread, queue);
    ntfn_ptr_set_queue(ntfnPtr, new_queue);
    notification_ptr_set_state(ntfnPtr, NtfnState_Waiting);
}

शून्य tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcb_queue_remove(queue, thread);
    ntfn_ptr_set_queue(ntfnPtr, new_queue);

    जइ (tcb_queue_empty(new_queue)) {
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
    }
}

शून्य reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_queue_t queue = ntfn_ptr_get_queue(ntfnPtr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ntfn_ptr_set_queue(ntfnPtr, queue);
}
#endif
