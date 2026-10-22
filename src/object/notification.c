/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <assert.h>

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <object/tcb.h>
#অন্তর্ভাব <object/endpoint.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <machine/io.h>

#অন্তর্ভাব <object/notification.h>

স্থির অন্তঃস্থ tcb_queue_t PURE ntfn_ptr_get_queue(notification_t *ntfnPtr)
{
    tcb_queue_t ntfn_queue;

    ntfn_queue.head = (tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr);
    ntfn_queue.end = (tcb_t *)notification_ptr_get_ntfnQueue_tail(ntfnPtr);

    প্রত্যাগম ntfn_queue;
}

স্থির অন্তঃস্থ শূন্য ntfn_ptr_set_queue(notification_t *ntfnPtr, tcb_queue_t ntfn_queue)
{
    notification_ptr_set_ntfnQueue_head(ntfnPtr, (word_t)ntfn_queue.head);
    notification_ptr_set_ntfnQueue_tail(ntfnPtr, (word_t)ntfn_queue.end);
}

#ifdef CONFIG_KERNEL_MCS
স্থির অন্তঃস্থ শূন্য maybeDonateSchedContext(tcb_t *tcb, notification_t *ntfnPtr)
{
    যদি (tcb->tcbSchedContext == রিক্ত) {
        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        যদি (sc != রিক্ত && sc->scTcb == রিক্ত) {
            schedContext_donate(sc, tcb);
            schedContext_resume(sc);
        }
    }
}

#endif

#ifdef CONFIG_KERNEL_MCS
#define MCS_DO_IF_SC(tcb, ntfnPtr, _block) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    যদি (isSchedulable(tcb)) { \
        _block \
    }
#অন্যথা
#define MCS_DO_IF_SC(tcb, ntfnPtr, _block) \
    { \
        _block \
    }
#endif

শূন্য sendSignal(notification_t *ntfnPtr, word_t badge)
{
    বিকল্প (notification_ptr_get_state(ntfnPtr)) {
    পক্ষ NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        যদি (tcb) {
            যদি (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                setThreadState(tcb, ThreadState_Running);
                setRegister(tcb, badgeRegister, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possibleSwitchTo(tcb);
                })
#ifdef CONFIG_KERNEL_MCS
                যদি (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    প্রতিজ্ঞা(tcb->tcbSchedContext != গ্রন্থি_STATE(ksCurSC));
                    যদি (tcb->tcbSchedContext != গ্রন্থি_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#endif
#ifdef CONFIG_VTX
            } অন্যথা যদি (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#ifdef ENABLE_SMP_SUPPORT
                যদি (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_set_active(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } অন্যথা
#endif /* ENABLE_SMP_SUPPORT */
                {
                    setThreadState(tcb, ThreadState_Running);
                    setRegister(tcb, badgeRegister, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possibleSwitchTo(tcb);
                    })
#ifdef CONFIG_KERNEL_MCS
                    যদি (tcb->tcbSchedContext != রিক্ত && sc_active(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        যদি (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != গ্রন্থি_STATE(ksCurSC)) {
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
            } অন্যথা {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_set_active(ntfnPtr, badge);
            }
        } অন্যথা {
            ntfn_set_active(ntfnPtr, badge);
        }
        ভঙ্গ;
    }
    পক্ষ NtfnState_Waiting: {
        tcb_queue_t ntfn_queue;
        tcb_t *dest;

        ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
        dest = ntfn_queue.head;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        প্রতিজ্ঞা(dest);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#অন্যথা
        ntfn_queue = tcbEPDequeue(dest, ntfn_queue);
        ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);

        /* set the thread state to idle if the queue is empty */
        যদি (!ntfn_queue.head) {
            notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        setThreadState(dest, ThreadState_Running);
        setRegister(dest, badgeRegister, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possibleSwitchTo(dest);
        })

#ifdef CONFIG_KERNEL_MCS
        যদি (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            প্রতিজ্ঞা(dest->tcbSchedContext != গ্রন্থি_STATE(ksCurSC));
            যদি (dest->tcbSchedContext != গ্রন্থি_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#endif
        ভঙ্গ;
    }

    পক্ষ NtfnState_Active: {
        word_t badge2;

        badge2 = notification_ptr_get_ntfn_বার্তা_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_set_ntfn_বার্তা_Identifier(ntfnPtr, badge2);
        ভঙ্গ;
    }
    }
}

শূন্য receiveSignal(tcb_t *thread, cap_t cap, bool_t isBlocking)
{
    notification_t *ntfnPtr;

    ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    বিকল্প (notification_ptr_get_state(ntfnPtr)) {
    পক্ষ NtfnState_Idle:
    পক্ষ NtfnState_Waiting: {

        যদি (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_set_tsType(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_set_blockingObject(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            scheduleTCB(thread);

            /* Enqueue TCB */
#ifdef CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#অন্যথা
            tcb_queue_t ntfn_queue;
            ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
            ntfn_queue = tcbEPAppend(thread, ntfn_queue);

            notification_ptr_set_state(ntfnPtr, NtfnState_Waiting);
            ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#endif
        } অন্যথা {
            doNBRecvFailedTransfer(thread);
        }

        ভঙ্গ;
    }

    পক্ষ NtfnState_Active:
        setRegister(
            thread, badgeRegister,
            notification_ptr_get_ntfn_বার্তা_Identifier(ntfnPtr));
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
#ifdef CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        যদি (thread->tcbSchedContext != গ্রন্থি_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#endif
        ভঙ্গ;
    }
}

#ifdef CONFIG_KERNEL_MCS
স্থির অন্তঃস্থ শূন্য removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    setThreadState(thread, ThreadState_Restart);
    যদি (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        প্রতিজ্ঞা(thread->tcbSchedContext != গ্রন্থি_STATE(ksCurSC));
        যদি (thread->tcbSchedContext != গ্রন্থি_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possibleSwitchTo(thread);
}
#endif

শূন্য cancelAllSignals(notification_t *ntfnPtr)
{
    যদি (notification_ptr_get_state(ntfnPtr) == NtfnState_Waiting) {
        /* Clear the queue and set all blocked threads to Restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *next;

        queue = ntfn_ptr_get_queue(ntfnPtr);

        প্রতি (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#অন্যথা
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfnQueue_head(ntfnPtr));

        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_set_ntfnQueue_head(ntfnPtr, 0);
        notification_ptr_set_ntfnQueue_tail(ntfnPtr, 0);

        প্রতি (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

শূন্য cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    প্রতিজ্ঞা(notification_ptr_get_state(ntfnPtr) == NtfnState_Waiting);

    /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#অন্যথা
    tcb_queue_t ntfn_queue;
    ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
    ntfn_queue = tcbEPDequeue(threadPtr, ntfn_queue);
    ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);

    /* Make notification object idle */
    যদি (!ntfn_queue.head) {
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
    }
#endif /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    setThreadState(threadPtr, ThreadState_Inactive);
}

শূন্য completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    word_t badge;

    যদি (likely(tcb && notification_ptr_get_state(ntfnPtr) == NtfnState_Active)) {
        badge = notification_ptr_get_ntfn_বার্তা_Identifier(ntfnPtr);
        setRegister(tcb, badgeRegister, badge);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
#ifdef CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        যদি (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            যদি (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != গ্রন্থি_STATE(ksCurSC)) {
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
    } অন্যথা {
        fail("tried to complete signal with inactive notification object");
    }
}

স্থির অন্তঃস্থ শূন্য doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_set_ntfnBoundTCB(ntfnPtr, (word_t) 0);
    tcbptr->tcbBoundNotification = রিক্ত;
}

শূন্য unbindMaybeNotification(notification_t *ntfnPtr)
{
    tcb_t *boundTCB;
    boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);

    যদি (boundTCB) {
        doUnbindNotification(ntfnPtr, boundTCB);
    }
}

শূন্য unbindNotification(tcb_t *tcb)
{
    notification_t *ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    যদি (ntfnPtr) {
        doUnbindNotification(ntfnPtr, tcb);
    }
}

শূন্য bindNotification(tcb_t *tcb, notification_t *ntfnPtr)
{
    notification_ptr_set_ntfnBoundTCB(ntfnPtr, (word_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#ifdef CONFIG_KERNEL_MCS
শূন্য tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcbAppend(thread, queue);
    ntfn_ptr_set_queue(ntfnPtr, new_queue);
    notification_ptr_set_state(ntfnPtr, NtfnState_Waiting);
}

শূন্য tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcb_queue_remove(queue, thread);
    ntfn_ptr_set_queue(ntfnPtr, new_queue);

    যদি (tcb_queue_empty(new_queue)) {
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
    }
}

শূন্য reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_queue_t queue = ntfn_ptr_get_queue(ntfnPtr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ntfn_ptr_set_queue(ntfnPtr, queue);
}
#endif
