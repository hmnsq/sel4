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

स्थिर अन्तःस्थ tcb_queue_t विशुद्धम् ntfn_ptr_get_queue(notification_t *ntfnPtr)
{
    tcb_queue_t ntfn_queue;

    ntfn_queue.शीर्ष = (tcb_t *)notification_ptr_get_ntfn_Queue_शीर्ष(ntfnPtr);
    ntfn_queue.अन्तः_2 = (tcb_t *)notification_ptr_get_ntfn_Queue_पुच्छ(ntfnPtr);

    प्रत्यागम ntfn_queue;
}

स्थिर अन्तःस्थ शून्य ntfn_ptr_समुच्चयः_queue(notification_t *ntfnPtr, tcb_queue_t ntfn_queue)
{
    notification_ptr_समुच्चयः_ntfn_Queue_शीर्ष(ntfnPtr, (word_t)ntfn_queue.शीर्ष);
    notification_ptr_समुच्चयः_ntfn_Queue_पुच्छ(ntfnPtr, (word_t)ntfn_queue.अन्तः_2);
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ शून्य maybeDonateSchedContext(tcb_t *tcb, notification_t *ntfnPtr)
{
    यदि (tcb->tcbSchedContext == रिक्त) {
        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        यदि (sc != रिक्त && sc->scTcb == रिक्त) {
            schedContext_donate(sc, tcb);
            schedContext_resume(sc);
        }
    }
}

#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
#परिभाषय MCS_DO_IF_SC(tcb, ntfnPtr, खण्डम्) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    यदि (isSchedulable(tcb)) { \
        खण्डम् \
    }
#अन्यथा
#परिभाषय MCS_DO_IF_SC(tcb, ntfnPtr, खण्डम्) \
    { \
        खण्डम् \
    }
#यदि_अन्तः

शून्य sendSignal(notification_t *ntfnPtr, word_t badge)
{
    विकल्प (notification_ptr_get_state(ntfnPtr)) {
    पक्ष NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        यदि (tcb) {
            यदि (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                समुच्चयः_Thread_State(tcb, ThreadState_Running);
                समुच्चयः_पञ्जिकाकोशः(tcb, badge_पञ्जिकाकोशः, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possible_Switch_गन्तव्यम्(tcb);
                })
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
                यदि (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    प्रतिज्ञा(tcb->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
                    यदि (tcb->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_VTX
            } अन्यथा यदि (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
                यदि (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_समुच्चयः_सक्रिय(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } अन्यथा
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
                {
                    समुच्चयः_Thread_State(tcb, ThreadState_Running);
                    समुच्चयः_पञ्जिकाकोशः(tcb, badge_पञ्जिकाकोशः, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possible_Switch_गन्तव्यम्(tcb);
                    })
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
                    यदि (tcb->tcbSchedContext != रिक्त && sc_सक्रिय(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        यदि (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
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
#यदि_अन्तः
                }
#यदि_अन्तः /* CONFIG_VTX */
            } अन्यथा {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_समुच्चयः_सक्रिय(ntfnPtr, badge);
            }
        } अन्यथा {
            ntfn_समुच्चयः_सक्रिय(ntfnPtr, badge);
        }
        भङ्ग;
    }
    पक्ष Ntfn_State_प्रतीक्षा: {
        tcb_queue_t ntfn_queue;
        tcb_t *dest;

        ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
        dest = ntfn_queue.शीर्ष;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        प्रतिज्ञा(dest);

        /* Dequeue TCB */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#अन्यथा
        ntfn_queue = tcbEPDequeue(dest, ntfn_queue);
        ntfn_ptr_समुच्चयः_queue(ntfnPtr, ntfn_queue);

        /* set the thread state to idle if the queue is empty */
        यदि (!ntfn_queue.शीर्ष) {
            notification_ptr_समुच्चयः_state(ntfnPtr, NtfnState_Idle);
        }
#यदि_अन्तः /* CONFIG_KERNEL_MCS */

        समुच्चयः_Thread_State(dest, ThreadState_Running);
        समुच्चयः_पञ्जिकाकोशः(dest, badge_पञ्जिकाकोशः, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possible_Switch_गन्तव्यम्(dest);
        })

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        यदि (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            प्रतिज्ञा(dest->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
            यदि (dest->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#यदि_अन्तः
        भङ्ग;
    }

    पक्ष Ntfn_State_सक्रिय: {
        word_t badge2;

        badge2 = notification_ptr_get_ntfn_सन्देशः_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_समुच्चयः_ntfn_सन्देशः_Identifier(ntfnPtr, badge2);
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
    पक्ष Ntfn_State_प्रतीक्षा: {

        यदि (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_समुच्चयः_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_समुच्चयः_blocking_Object(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            कालसारणी_TCB(thread);

            /* Enqueue TCB */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#अन्यथा
            tcb_queue_t ntfn_queue;
            ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
            ntfn_queue = tcbEPAppend(thread, ntfn_queue);

            notification_ptr_समुच्चयः_state(ntfnPtr, Ntfn_State_प्रतीक्षा);
            ntfn_ptr_समुच्चयः_queue(ntfnPtr, ntfn_queue);
#यदि_अन्तः /* CONFIG_KERNEL_MCS */

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#यदि_अन्तः
        } अन्यथा {
            doNBRecvFailedTransfer(thread);
        }

        भङ्ग;
    }

    पक्ष Ntfn_State_सक्रिय:
        समुच्चयः_पञ्जिकाकोशः(
            thread, badge_पञ्जिकाकोशः,
            notification_ptr_get_ntfn_सन्देशः_Identifier(ntfnPtr));
        notification_ptr_समुच्चयः_state(ntfnPtr, NtfnState_Idle);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        यदि (thread->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#यदि_अन्तः
        भङ्ग;
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ शून्य removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    समुच्चयः_Thread_State(thread, ThreadState_Restart);
    यदि (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        प्रतिज्ञा(thread->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
        यदि (thread->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possible_Switch_गन्तव्यम्(thread);
}
#यदि_अन्तः

शून्य cancel_All_सङ्केताः(notification_t *ntfnPtr)
{
    यदि (notification_ptr_get_state(ntfnPtr) == Ntfn_State_प्रतीक्षा) {
        /* Clear the queue and set all blocked threads to Restart */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *अग्र;

        queue = ntfn_ptr_get_queue(ntfnPtr);

        प्रति (thread = queue.शीर्ष; thread; thread = अग्र) {
            अग्र = thread->tcb_Sched_अग्र;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#अन्यथा
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfn_Queue_शीर्ष(ntfnPtr));

        notification_ptr_समुच्चयः_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_समुच्चयः_ntfn_Queue_शीर्ष(ntfnPtr, 0);
        notification_ptr_समुच्चयः_ntfn_Queue_पुच्छ(ntfnPtr, 0);

        प्रति (; thread; thread = thread->tcbEPNext) {
            समुच्चयः_Thread_State(thread, ThreadState_Restart);
            SCHED_पङ्क्तौ_निक्षिप(thread);
        }
#यदि_अन्तः /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

शून्य cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    प्रतिज्ञा(notification_ptr_get_state(ntfnPtr) == Ntfn_State_प्रतीक्षा);

    /* Dequeue TCB */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#अन्यथा
    tcb_queue_t ntfn_queue;
    ntfn_queue = ntfn_ptr_get_queue(ntfnPtr);
    ntfn_queue = tcbEPDequeue(threadPtr, ntfn_queue);
    ntfn_ptr_समुच्चयः_queue(ntfnPtr, ntfn_queue);

    /* Make notification object idle */
    यदि (!ntfn_queue.शीर्ष) {
        notification_ptr_समुच्चयः_state(ntfnPtr, NtfnState_Idle);
    }
#यदि_अन्तः /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    समुच्चयः_Thread_State(threadPtr, ThreadState_Inactive);
}

शून्य completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    word_t badge;

    यदि (likely(tcb && notification_ptr_get_state(ntfnPtr) == Ntfn_State_सक्रिय)) {
        badge = notification_ptr_get_ntfn_सन्देशः_Identifier(ntfnPtr);
        समुच्चयः_पञ्जिकाकोशः(tcb, badge_पञ्जिकाकोशः, badge);
        notification_ptr_समुच्चयः_state(ntfnPtr, NtfnState_Idle);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        यदि (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            यदि (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
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
#यदि_अन्तः
    } अन्यथा {
        fail("tried to complete signal with inactive notification object");
    }
}

स्थिर अन्तःस्थ शून्य doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_समुच्चयः_ntfn_Bound_TCB(ntfnPtr, (word_t) 0);
    tcbptr->tcbBoundNotification = रिक्त;
}

शून्य unbindMaybeNotification(notification_t *ntfnPtr)
{
    tcb_t *boundTCB;
    boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);

    यदि (boundTCB) {
        doUnbindNotification(ntfnPtr, boundTCB);
    }
}

शून्य unbindNotification(tcb_t *tcb)
{
    notification_t *ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    यदि (ntfnPtr) {
        doUnbindNotification(ntfnPtr, tcb);
    }
}

शून्य bindNotification(tcb_t *tcb, notification_t *ntfnPtr)
{
    notification_ptr_समुच्चयः_ntfn_Bound_TCB(ntfnPtr, (word_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcbAppend(thread, queue);
    ntfn_ptr_समुच्चयः_queue(ntfnPtr, new_queue);
    notification_ptr_समुच्चयः_state(ntfnPtr, Ntfn_State_प्रतीक्षा);
}

शून्य tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ntfn_ptr_get_queue(ntfnPtr);
    new_queue = tcb_queue_remove(queue, thread);
    ntfn_ptr_समुच्चयः_queue(ntfnPtr, new_queue);

    यदि (tcb_queue_रिक्तानि(new_queue)) {
        notification_ptr_समुच्चयः_state(ntfnPtr, NtfnState_Idle);
    }
}

शून्य reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_queue_t queue = ntfn_ptr_get_queue(ntfnPtr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ntfn_ptr_समुच्चयः_queue(ntfnPtr, queue);
}
#यदि_अन्तः
