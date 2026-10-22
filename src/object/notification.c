/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <assert.h>

#समावेश <types.h>
#समावेश <kernel/thread.h>
#समावेश <object/structures.h>
#समावेश <object/tcb.h>
#समावेश <object/endpoint.h>
#समावेश <model/statedata.h>
#समावेश <machine/io.h>

#समावेश <object/notification.h>

अटल अंतरभूत tcb_queue_t PURE ntfn_ptr_get_queue(notification_t *ntfnPtr)
{
    tcb_queue_t ntfn_queue;

    ntfn_queue.शीर्ष = (tcb_t *)notification_ptr_get_ntfn_Queue_शीर्ष(ntfnPtr);
    ntfn_queue.end = (tcb_t *)notification_ptr_get_ntfn_Queue_पुच्छ(ntfnPtr);

    वापस ntfn_queue;
}

अटल अंतरभूत व्योम ntfn_ptr_set_queue(notification_t *ntfnPtr, tcb_queue_t ntfn_queue)
{
    notification_ptr_set_ntfn_Queue_शीर्ष(ntfnPtr, (word_t)ntfn_queue.शीर्ष);
    notification_ptr_set_ntfn_Queue_पुच्छ(ntfnPtr, (word_t)ntfn_queue.end);
}

#अगर_घोषित CONFIG_KERNEL_MCS
अटल अंतरभूत व्योम maybeDonateSchedContext(tcb_t *tcb, notification_t *ntfnPtr)
{
    अगर (tcb->tcbSchedContext == शून्य) {
        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        अगर (sc != शून्य && sc->scTcb == शून्य) {
            schedContext_donate(sc, tcb);
            schedContext_resume(sc);
        }
    }
}

#पूर्ण_अगर

#अगर_घोषित CONFIG_KERNEL_MCS
#घोषणा MCS_DO_IF_SC(tcb, ntfnPtr, _block) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    अगर (isSchedulable(tcb)) { \
        _block \
    }
#अन्यथा
#घोषणा MCS_DO_IF_SC(tcb, ntfnPtr, _block) \
    { \
        _block \
    }
#पूर्ण_अगर

व्योम sendSignal(notification_t *ntfnPtr, word_t badge)
{
    चयन (notification_ptr_get_state(ntfnPtr)) {
    हाल NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        अगर (tcb) {
            अगर (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                setThreadState(tcb, ThreadState_Running);
                setRegister(tcb, badgeRegister, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possibleSwitchTo(tcb);
                })
#अगर_घोषित CONFIG_KERNEL_MCS
                अगर (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    निश्चित(tcb->tcbSchedContext != गाँठ_STATE(ksCurSC));
                    अगर (tcb->tcbSchedContext != गाँठ_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#पूर्ण_अगर
#अगर_घोषित CONFIG_VTX
            } अन्यथा अगर (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#अगर_घोषित ENABLE_SMP_SUPPORT
                अगर (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_set_active(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } अन्यथा
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
                {
                    setThreadState(tcb, ThreadState_Running);
                    setRegister(tcb, badgeRegister, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possibleSwitchTo(tcb);
                    })
#अगर_घोषित CONFIG_KERNEL_MCS
                    अगर (tcb->tcbSchedContext != शून्य && sc_active(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        अगर (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != गाँठ_STATE(ksCurSC)) {
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
#पूर्ण_अगर
                }
#पूर्ण_अगर /* CONFIG_VTX */
            } अन्यथा {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_set_active(ntfnPtr, badge);
            }
        } अन्यथा {
            ntfn_set_active(ntfnPtr, badge);
        }
        अवरोध;
    }
    हाल NtfnState_Waiting: {
        tcb_queue_t ntfn_queue;
        tcb_t *dest;

        ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
        dest = ntfn_queue.शीर्ष;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        निश्चित(dest);

        /* Dequeue TCB */
#अगर_घोषित CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#अन्यथा
        ntfn_queue = tcbEPDequeue(dest, ntfn_queue);
        ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);

        /* set the thread state to idle if the queue is empty */
        अगर (!ntfn_queue.शीर्ष) {
            notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        }
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */

        setThreadState(dest, ThreadState_Running);
        setRegister(dest, badgeRegister, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possibleSwitchTo(dest);
        })

#अगर_घोषित CONFIG_KERNEL_MCS
        अगर (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            निश्चित(dest->tcbSchedContext != गाँठ_STATE(ksCurSC));
            अगर (dest->tcbSchedContext != गाँठ_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#पूर्ण_अगर
        अवरोध;
    }

    हाल NtfnState_Active: {
        word_t badge2;

        badge2 = notification_ptr_get_ntfn_संदेश_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_set_ntfn_संदेश_Identifier(ntfnPtr, badge2);
        अवरोध;
    }
    }
}

व्योम receiveSignal(tcb_t *thread, cap_t cap, bool_t isBlocking)
{
    notification_t *ntfnPtr;

    ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    चयन (notification_ptr_get_state(ntfnPtr)) {
    हाल NtfnState_Idle:
    हाल NtfnState_Waiting: {

        अगर (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_set_tsType(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_set_blockingObject(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            scheduleTCB(thread);

            /* Enqueue TCB */
#अगर_घोषित CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#अन्यथा
            tcb_queue_t ntfn_queue;
            ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
            ntfn_queue = tcbEPAppend(thread, ntfn_queue);

            notification_ptr_set_state(ntfnPtr, NtfnState_Waiting);
            ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */

#अगर_घोषित CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#पूर्ण_अगर
        } अन्यथा {
            doNBRecvFailedTransfer(thread);
        }

        अवरोध;
    }

    हाल NtfnState_Active:
        setRegister(
            thread, badgeRegister,
            notification_ptr_get_ntfn_संदेश_Identifier(ntfnPtr));
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
#अगर_घोषित CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        अगर (thread->tcbSchedContext != गाँठ_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#पूर्ण_अगर
        अवरोध;
    }
}

#अगर_घोषित CONFIG_KERNEL_MCS
अटल अंतरभूत व्योम removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    setThreadState(thread, ThreadState_Restart);
    अगर (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        निश्चित(thread->tcbSchedContext != गाँठ_STATE(ksCurSC));
        अगर (thread->tcbSchedContext != गाँठ_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possibleSwitchTo(thread);
}
#पूर्ण_अगर

व्योम cancelAllSignals(notification_t *ntfnPtr)
{
    अगर (notification_ptr_get_state(ntfnPtr) == NtfnState_Waiting) {
        /* Clear the queue and set all blocked threads to Restart */
#अगर_घोषित CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *अग्र;

        queue = ntfn_ptr_get_queue(ntfnPtr);

        क्रम (thread = queue.शीर्ष; thread; thread = अग्र) {
            अग्र = thread->tcb_Sched_अग्र;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#अन्यथा
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfn_Queue_शीर्ष(ntfnPtr));

        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_set_ntfn_Queue_शीर्ष(ntfnPtr, 0);
        notification_ptr_set_ntfn_Queue_पुच्छ(ntfnPtr, 0);

        क्रम (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

व्योम cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    निश्चित(notification_ptr_get_state(ntfnPtr) == NtfnState_Waiting);

    /* Dequeue TCB */
#अगर_घोषित CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#अन्यथा
    tcb_queue_t ntfn_queue;
    ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
    ntfn_queue = tcbEPDequeue(threadPtr, ntfn_queue);
    ntfn_ptr_set_queue(ntfnPtr, ntfn_queue);

    /* Make notification object idle */
    अगर (!ntfn_queue.शीर्ष) {
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
    }
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    setThreadState(threadPtr, ThreadState_Inactive);
}

व्योम completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    word_t badge;

    अगर (likely(tcb && notification_ptr_get_state(ntfnPtr) == NtfnState_Active)) {
        badge = notification_ptr_get_ntfn_संदेश_Identifier(ntfnPtr);
        setRegister(tcb, badgeRegister, badge);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
#अगर_घोषित CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        अगर (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            अगर (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != गाँठ_STATE(ksCurSC)) {
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
#पूर्ण_अगर
    } अन्यथा {
        fail("tried to complete signal with inactive notification object");
    }
}

अटल अंतरभूत व्योम doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_set_ntfnBoundTCB(ntfnPtr, (word_t) 0);
    tcbptr->tcbBoundNotification = शून्य;
}

व्योम unbindMaybeNotification(notification_t *ntfnPtr)
{
    tcb_t *boundTCB;
    boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);

    अगर (boundTCB) {
        doUnbindNotification(ntfnPtr, boundTCB);
    }
}

व्योम unbindNotification(tcb_t *tcb)
{
    notification_t *ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    अगर (ntfnPtr) {
        doUnbindNotification(ntfnPtr, tcb);
    }
}

व्योम bindNotification(tcb_t *tcb, notification_t *ntfnPtr)
{
    notification_ptr_set_ntfnBoundTCB(ntfnPtr, (word_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#अगर_घोषित CONFIG_KERNEL_MCS
व्योम tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcbAppend(thread, queue);
    ntfn_ptr_set_queue(ntfnPtr, new_queue);
    notification_ptr_set_state(ntfnPtr, NtfnState_Waiting);
}

व्योम tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcb_queue_remove(queue, thread);
    ntfn_ptr_set_queue(ntfnPtr, new_queue);

    अगर (tcb_queue_empty(new_queue)) {
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
    }
}

व्योम reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_queue_t queue = ntfn_ptr_get_queue(ntfnPtr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ntfn_ptr_set_queue(ntfnPtr, queue);
}
#पूर्ण_अगर
