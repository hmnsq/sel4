/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <assert.h>

#அந்தர்பாவ <types.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <object/tcb.h>
#அந்தர்பாவ <object/endpoint.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <machine/io.h>

#அந்தர்பாவ <object/notification.h>

நிலையான உள்வரி tcb_queue_t PURE ntfn_ptr_get_queue(notification_t *ntfnPtr)
{
    tcb_queue_t ntfn_queue;

    ntfn_queue.head = (tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr);
    ntfn_queue.end = (tcb_t *)notification_ptr_get_ntfnQueue_tail(ntfnPtr);

    திரும்பு ntfn_queue;
}

நிலையான உள்வரி வெறுமை ntfn_ptr_set_queue(notification_t *ntfnPtr, tcb_queue_t ntfn_queue)
{
    notification_ptr_set_ntfnQueue_head(ntfnPtr, (word_t)ntfn_queue.head);
    notification_ptr_set_ntfnQueue_tail(ntfnPtr, (word_t)ntfn_queue.end);
}

#ifdef CONFIG_KERNEL_MCS
நிலையான உள்வரி வெறுமை maybeDonateSchedContext(tcb_t *tcb, notification_t *ntfnPtr)
{
    எனில் (tcb->tcbSchedContext == வெற்று) {
        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        எனில் (sc != வெற்று && sc->scTcb == வெற்று) {
            schedContext_donate(sc, tcb);
            schedContext_resume(sc);
        }
    }
}

#endif

#ifdef CONFIG_KERNEL_MCS
#define MCS_DO_IF_SC(tcb, ntfnPtr, _block) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    எனில் (isSchedulable(tcb)) { \
        _block \
    }
#இல்லையெனில்
#define MCS_DO_IF_SC(tcb, ntfnPtr, _block) \
    { \
        _block \
    }
#endif

வெறுமை sendSignal(notification_t *ntfnPtr, word_t badge)
{
    தேர்வு (notification_ptr_get_state(ntfnPtr)) {
    நிலை NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        எனில் (tcb) {
            எனில் (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                setThreadState(tcb, ThreadState_Running);
                setRegister(tcb, badgeRegister, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possibleSwitchTo(tcb);
                })
#ifdef CONFIG_KERNEL_MCS
                எனில் (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    ப்ரதிஜ்ஞா(tcb->tcbSchedContext != முனை_STATE(ksCurSC));
                    எனில் (tcb->tcbSchedContext != முனை_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#endif
#ifdef CONFIG_VTX
            } இல்லையெனில் எனில் (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#ifdef ENABLE_SMP_SUPPORT
                எனில் (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_set_active(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } இல்லையெனில்
#endif /* ENABLE_SMP_SUPPORT */
                {
                    setThreadState(tcb, ThreadState_Running);
                    setRegister(tcb, badgeRegister, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possibleSwitchTo(tcb);
                    })
#ifdef CONFIG_KERNEL_MCS
                    எனில் (tcb->tcbSchedContext != வெற்று && sc_active(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        எனில் (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != முனை_STATE(ksCurSC)) {
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
            } இல்லையெனில் {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_set_active(ntfnPtr, badge);
            }
        } இல்லையெனில் {
            ntfn_set_active(ntfnPtr, badge);
        }
        நிறுத்து;
    }
    நிலை NtfnState_Waiting: {
        tcb_queue_t ntfn_queue;
        tcb_t *dest;

        ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
        dest = ntfn_queue.head;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        ப்ரதிஜ்ஞா(dest);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#இல்லையெனில்
        ntfn_queue = tcbEPDequeue(dest, ntfn_queue);
        ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);

        /* set the thread state to idle if the queue is empty */
        எனில் (!ntfn_queue.head) {
            notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        setThreadState(dest, ThreadState_Running);
        setRegister(dest, badgeRegister, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possibleSwitchTo(dest);
        })

#ifdef CONFIG_KERNEL_MCS
        எனில் (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            ப்ரதிஜ்ஞா(dest->tcbSchedContext != முனை_STATE(ksCurSC));
            எனில் (dest->tcbSchedContext != முனை_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#endif
        நிறுத்து;
    }

    நிலை NtfnState_Active: {
        word_t badge2;

        badge2 = notification_ptr_get_ntfn_செய்தி_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_set_ntfn_செய்தி_Identifier(ntfnPtr, badge2);
        நிறுத்து;
    }
    }
}

வெறுமை receiveSignal(tcb_t *thread, cap_t cap, bool_t isBlocking)
{
    notification_t *ntfnPtr;

    ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    தேர்வு (notification_ptr_get_state(ntfnPtr)) {
    நிலை NtfnState_Idle:
    நிலை NtfnState_Waiting: {

        எனில் (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_set_tsType(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_set_blockingObject(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            scheduleTCB(thread);

            /* Enqueue TCB */
#ifdef CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#இல்லையெனில்
            tcb_queue_t ntfn_queue;
            ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
            ntfn_queue = tcbEPAppend(thread, ntfn_queue);

            notification_ptr_set_state(ntfnPtr, NtfnState_Waiting);
            ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#endif
        } இல்லையெனில் {
            doNBRecvFailedTransfer(thread);
        }

        நிறுத்து;
    }

    நிலை NtfnState_Active:
        setRegister(
            thread, badgeRegister,
            notification_ptr_get_ntfn_செய்தி_Identifier(ntfnPtr));
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
#ifdef CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        எனில் (thread->tcbSchedContext != முனை_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#endif
        நிறுத்து;
    }
}

#ifdef CONFIG_KERNEL_MCS
நிலையான உள்வரி வெறுமை removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    setThreadState(thread, ThreadState_Restart);
    எனில் (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        ப்ரதிஜ்ஞா(thread->tcbSchedContext != முனை_STATE(ksCurSC));
        எனில் (thread->tcbSchedContext != முனை_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possibleSwitchTo(thread);
}
#endif

வெறுமை cancelAllSignals(notification_t *ntfnPtr)
{
    எனில் (notification_ptr_get_state(ntfnPtr) == NtfnState_Waiting) {
        /* Clear the queue and set all blocked threads to Restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *next;

        queue = ntfn_ptr_get_queue(ntfnPtr);

        க்கு (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#இல்லையெனில்
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfnQueue_head(ntfnPtr));

        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_set_ntfnQueue_head(ntfnPtr, 0);
        notification_ptr_set_ntfnQueue_tail(ntfnPtr, 0);

        க்கு (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

வெறுமை cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    ப்ரதிஜ்ஞா(notification_ptr_get_state(ntfnPtr) == NtfnState_Waiting);

    /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#இல்லையெனில்
    tcb_queue_t ntfn_queue;
    ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
    ntfn_queue = tcbEPDequeue(threadPtr, ntfn_queue);
    ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);

    /* Make notification object idle */
    எனில் (!ntfn_queue.head) {
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
    }
#endif /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    setThreadState(threadPtr, ThreadState_Inactive);
}

வெறுமை completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    word_t badge;

    எனில் (likely(tcb && notification_ptr_get_state(ntfnPtr) == NtfnState_Active)) {
        badge = notification_ptr_get_ntfn_செய்தி_Identifier(ntfnPtr);
        setRegister(tcb, badgeRegister, badge);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
#ifdef CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        எனில் (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            எனில் (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != முனை_STATE(ksCurSC)) {
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
    } இல்லையெனில் {
        fail("tried to complete signal with inactive notification object");
    }
}

நிலையான உள்வரி வெறுமை doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_set_ntfnBoundTCB(ntfnPtr, (word_t) 0);
    tcbptr->tcbBoundNotification = வெற்று;
}

வெறுமை unbindMaybeNotification(notification_t *ntfnPtr)
{
    tcb_t *boundTCB;
    boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);

    எனில் (boundTCB) {
        doUnbindNotification(ntfnPtr, boundTCB);
    }
}

வெறுமை unbindNotification(tcb_t *tcb)
{
    notification_t *ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    எனில் (ntfnPtr) {
        doUnbindNotification(ntfnPtr, tcb);
    }
}

வெறுமை bindNotification(tcb_t *tcb, notification_t *ntfnPtr)
{
    notification_ptr_set_ntfnBoundTCB(ntfnPtr, (word_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#ifdef CONFIG_KERNEL_MCS
வெறுமை tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcbAppend(thread, queue);
    ntfn_ptr_set_queue(ntfnPtr, new_queue);
    notification_ptr_set_state(ntfnPtr, NtfnState_Waiting);
}

வெறுமை tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcb_queue_remove(queue, thread);
    ntfn_ptr_set_queue(ntfnPtr, new_queue);

    எனில் (tcb_queue_empty(new_queue)) {
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
    }
}

வெறுமை reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_queue_t queue = ntfn_ptr_get_queue(ntfnPtr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ntfn_ptr_set_queue(ntfnPtr, queue);
}
#endif
