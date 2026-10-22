/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>

#ضمن <types.h>
#ضمن <kernel/thread.h>
#ضمن <object/structures.h>
#ضمن <object/tcb.h>
#ضمن <object/endpoint.h>
#ضمن <model/statedata.h>
#ضمن <machine/io.h>

#ضمن <object/notification.h>

ساكن مضمن tcb_queue_t PURE ntfn_ptr_get_queue(notification_t *ntfnPtr)
{
    tcb_queue_t ntfn_queue;

    ntfn_queue.head = (tcb_t *)notification_ptr_get_ntfnQueue_head(ntfnPtr);
    ntfn_queue.end = (tcb_t *)notification_ptr_get_ntfnQueue_tail(ntfnPtr);

    أرجع ntfn_queue;
}

ساكن مضمن فراغ ntfn_ptr_set_queue(notification_t *ntfnPtr, tcb_queue_t ntfn_queue)
{
    notification_ptr_set_ntfnQueue_head(ntfnPtr, (word_t)ntfn_queue.head);
    notification_ptr_set_ntfnQueue_tail(ntfnPtr, (word_t)ntfn_queue.end);
}

#ifdef CONFIG_KERNEL_MCS
ساكن مضمن فراغ maybeDonateSchedContext(tcb_t *tcb, notification_t *ntfnPtr)
{
    إذا (tcb->tcbSchedContext == عدم) {
        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        إذا (sc != عدم && sc->scTcb == عدم) {
            schedContext_donate(sc, tcb);
            schedContext_resume(sc);
        }
    }
}

#endif

#ifdef CONFIG_KERNEL_MCS
#define MCS_DO_IF_SC(tcb, ntfnPtr, _block) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    إذا (isSchedulable(tcb)) { \
        _block \
    }
#وإلا
#define MCS_DO_IF_SC(tcb, ntfnPtr, _block) \
    { \
        _block \
    }
#endif

فراغ sendSignal(notification_t *ntfnPtr, word_t badge)
{
    بدل (notification_ptr_get_state(ntfnPtr)) {
    حالة NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        إذا (tcb) {
            إذا (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                setThreadState(tcb, ThreadState_Running);
                setRegister(tcb, badgeRegister, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possibleSwitchTo(tcb);
                })
#ifdef CONFIG_KERNEL_MCS
                إذا (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    أكد(tcb->tcbSchedContext != گرہ_STATE(ksCurSC));
                    إذا (tcb->tcbSchedContext != گرہ_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#endif
#ifdef CONFIG_VTX
            } وإلا إذا (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#ifdef ENABLE_SMP_SUPPORT
                إذا (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_set_active(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } وإلا
#endif /* ENABLE_SMP_SUPPORT */
                {
                    setThreadState(tcb, ThreadState_Running);
                    setRegister(tcb, badgeRegister, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possibleSwitchTo(tcb);
                    })
#ifdef CONFIG_KERNEL_MCS
                    إذا (tcb->tcbSchedContext != عدم && sc_active(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        إذا (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != گرہ_STATE(ksCurSC)) {
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
            } وإلا {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_set_active(ntfnPtr, badge);
            }
        } وإلا {
            ntfn_set_active(ntfnPtr, badge);
        }
        اكسر;
    }
    حالة NtfnState_Waiting: {
        tcb_queue_t ntfn_queue;
        tcb_t *dest;

        ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
        dest = ntfn_queue.head;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        أكد(dest);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#وإلا
        ntfn_queue = tcbEPDequeue(dest, ntfn_queue);
        ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);

        /* set the thread state to idle if the queue is empty */
        إذا (!ntfn_queue.head) {
            notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        setThreadState(dest, ThreadState_Running);
        setRegister(dest, badgeRegister, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possibleSwitchTo(dest);
        })

#ifdef CONFIG_KERNEL_MCS
        إذا (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(dest->tcbSchedContext != گرہ_STATE(ksCurSC));
            إذا (dest->tcbSchedContext != گرہ_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#endif
        اكسر;
    }

    حالة NtfnState_Active: {
        word_t badge2;

        badge2 = notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_set_ntfn_پیغام_Identifier(ntfnPtr, badge2);
        اكسر;
    }
    }
}

فراغ receiveSignal(tcb_t *thread, cap_t cap, bool_t isBlocking)
{
    notification_t *ntfnPtr;

    ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    بدل (notification_ptr_get_state(ntfnPtr)) {
    حالة NtfnState_Idle:
    حالة NtfnState_Waiting: {

        إذا (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_set_tsType(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_set_blockingObject(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            scheduleTCB(thread);

            /* Enqueue TCB */
#ifdef CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#وإلا
            tcb_queue_t ntfn_queue;
            ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
            ntfn_queue = tcbEPAppend(thread, ntfn_queue);

            notification_ptr_set_state(ntfnPtr, NtfnState_Waiting);
            ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#endif
        } وإلا {
            doNBRecvFailedTransfer(thread);
        }

        اكسر;
    }

    حالة NtfnState_Active:
        setRegister(
            thread, badgeRegister,
            notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr));
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
#ifdef CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        إذا (thread->tcbSchedContext != گرہ_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#endif
        اكسر;
    }
}

#ifdef CONFIG_KERNEL_MCS
ساكن مضمن فراغ removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    setThreadState(thread, ThreadState_Restart);
    إذا (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        أكد(thread->tcbSchedContext != گرہ_STATE(ksCurSC));
        إذا (thread->tcbSchedContext != گرہ_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possibleSwitchTo(thread);
}
#endif

فراغ cancelAllSignals(notification_t *ntfnPtr)
{
    إذا (notification_ptr_get_state(ntfnPtr) == NtfnState_Waiting) {
        /* Clear the queue and set all blocked threads to Restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *next;

        queue = ntfn_ptr_get_queue(ntfnPtr);

        لكل (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#وإلا
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfnQueue_head(ntfnPtr));

        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_set_ntfnQueue_head(ntfnPtr, 0);
        notification_ptr_set_ntfnQueue_tail(ntfnPtr, 0);

        لكل (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

فراغ cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    أكد(notification_ptr_get_state(ntfnPtr) == NtfnState_Waiting);

    /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#وإلا
    tcb_queue_t ntfn_queue;
    ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
    ntfn_queue = tcbEPDequeue(threadPtr, ntfn_queue);
    ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);

    /* Make notification object idle */
    إذا (!ntfn_queue.head) {
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
    }
#endif /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    setThreadState(threadPtr, ThreadState_Inactive);
}

فراغ completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    word_t badge;

    إذا (likely(tcb && notification_ptr_get_state(ntfnPtr) == NtfnState_Active)) {
        badge = notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr);
        setRegister(tcb, badgeRegister, badge);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
#ifdef CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        إذا (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            إذا (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != گرہ_STATE(ksCurSC)) {
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
    } وإلا {
        fail("tried to complete signal with inactive notification object");
    }
}

ساكن مضمن فراغ doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_set_ntfnBoundTCB(ntfnPtr, (word_t) 0);
    tcbptr->tcbBoundNotification = عدم;
}

فراغ unbindMaybeNotification(notification_t *ntfnPtr)
{
    tcb_t *boundTCB;
    boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);

    إذا (boundTCB) {
        doUnbindNotification(ntfnPtr, boundTCB);
    }
}

فراغ unbindNotification(tcb_t *tcb)
{
    notification_t *ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    إذا (ntfnPtr) {
        doUnbindNotification(ntfnPtr, tcb);
    }
}

فراغ bindNotification(tcb_t *tcb, notification_t *ntfnPtr)
{
    notification_ptr_set_ntfnBoundTCB(ntfnPtr, (word_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#ifdef CONFIG_KERNEL_MCS
فراغ tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcbAppend(thread, queue);
    ntfn_ptr_set_queue(ntfnPtr, new_queue);
    notification_ptr_set_state(ntfnPtr, NtfnState_Waiting);
}

فراغ tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcb_queue_remove(queue, thread);
    ntfn_ptr_set_queue(ntfnPtr, new_queue);

    إذا (tcb_queue_empty(new_queue)) {
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
    }
}

فراغ reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_queue_t queue = ntfn_ptr_get_queue(ntfnPtr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ntfn_ptr_set_queue(ntfnPtr, queue);
}
#endif
