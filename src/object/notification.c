/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>

#అన్తర్భావ <types.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <object/tcb.h>
#అన్తర్భావ <object/endpoint.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <machine/io.h>

#అన్తర్భావ <object/notification.h>

స్థిర అంతఃవరుస tcb_వరుస_t శుద్ధ ntfn_ptr_get_వరుస(notification_t *ntfnPtr)
{
    tcb_వరుస_t ntfn_వరుస;

    ntfn_వరుస.తల = (tcb_t *)notification_ptr_get_ntfn_వరుస_తల(ntfnPtr);
    ntfn_వరుస.చివర = (tcb_t *)notification_ptr_get_ntfn_వరుస_తోక(ntfnPtr);

    తిరిగి ntfn_వరుస;
}

స్థిర అంతఃవరుస శూన్య ntfn_ptr_సమితి_వరుస(notification_t *ntfnPtr, tcb_వరుస_t ntfn_వరుస)
{
    notification_ptr_సమితి_ntfn_వరుస_తల(ntfnPtr, (మాట_t)ntfn_వరుస.తల);
    notification_ptr_సమితి_ntfn_వరుస_తోక(ntfnPtr, (మాట_t)ntfn_వరుస.చివర);
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
స్థిర అంతఃవరుస శూన్య maybeDonateSchedContext(tcb_t *tcb, notification_t *ntfnPtr)
{
    ఒకవేళ (tcb->tcbSchedContext == ఖాళీ) {
        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        ఒకవేళ (sc != ఖాళీ && sc->scTcb == ఖాళీ) {
            schedContext_donate(sc, tcb);
            schedContext_resume(sc);
        }
    }
}

#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_KERNEL_MCS
#నిర్వచించు MCS_DO_IF_SC(tcb, ntfnPtr, ఖండం) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    ఒకవేళ (isSchedulable(tcb)) { \
        ఖండం \
    }
#లేకపోతే
#నిర్వచించు MCS_DO_IF_SC(tcb, ntfnPtr, ఖండం) \
    { \
        ఖండం \
    }
#అయితే_ముగింపు

శూన్య sendSignal(notification_t *ntfnPtr, మాట_t badge)
{
    ఎంపిక (notification_ptr_get_state(ntfnPtr)) {
    స్థితి NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        ఒకవేళ (tcb) {
            ఒకవేళ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                సమితి_Thread_State(tcb, ThreadState_Running);
                సమితి_రిజిస్టరు(tcb, badge_రిజిస్టరు, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possible_Switch_గమ్యం(tcb);
                })
#నిర్వచితమైతే CONFIG_KERNEL_MCS
                ఒకవేళ (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    ప్రతిజ్ఞా(tcb->tcbSchedContext != కణుపు_STATE(ksCurSC));
                    ఒకవేళ (tcb->tcbSchedContext != కణుపు_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_VTX
            } లేకపోతే ఒకవేళ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
                ఒకవేళ (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_సమితి_చురుకైన(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } లేకపోతే
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
                {
                    సమితి_Thread_State(tcb, ThreadState_Running);
                    సమితి_రిజిస్టరు(tcb, badge_రిజిస్టరు, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possible_Switch_గమ్యం(tcb);
                    })
#నిర్వచితమైతే CONFIG_KERNEL_MCS
                    ఒకవేళ (tcb->tcbSchedContext != ఖాళీ && sc_చురుకైన(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        ఒకవేళ (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != కణుపు_STATE(ksCurSC)) {
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
#అయితే_ముగింపు
                }
#అయితే_ముగింపు /* CONFIG_VTX */
            } లేకపోతే {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_సమితి_చురుకైన(ntfnPtr, badge);
            }
        } లేకపోతే {
            ntfn_సమితి_చురుకైన(ntfnPtr, badge);
        }
        ఆపు;
    }
    స్థితి Ntfn_State_నిరీక్షణ: {
        tcb_వరుస_t ntfn_వరుస;
        tcb_t *dest;

        ntfn_వరుస = ntfn_ptr_get_వరుస(ntfnPtr);
        dest = ntfn_వరుస.తల;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        ప్రతిజ్ఞా(dest);

        /* Dequeue TCB */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#లేకపోతే
        ntfn_వరుస = tcbEPDequeue(dest, ntfn_వరుస);
        ntfn_ptr_సమితి_వరుస(ntfnPtr, ntfn_వరుస);

        /* set the thread state to idle if the queue is empty */
        ఒకవేళ (!ntfn_వరుస.తల) {
            notification_ptr_సమితి_state(ntfnPtr, NtfnState_Idle);
        }
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */

        సమితి_Thread_State(dest, ThreadState_Running);
        సమితి_రిజిస్టరు(dest, badge_రిజిస్టరు, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possible_Switch_గమ్యం(dest);
        })

#నిర్వచితమైతే CONFIG_KERNEL_MCS
        ఒకవేళ (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            ప్రతిజ్ఞా(dest->tcbSchedContext != కణుపు_STATE(ksCurSC));
            ఒకవేళ (dest->tcbSchedContext != కణుపు_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#అయితే_ముగింపు
        ఆపు;
    }

    స్థితి Ntfn_State_చురుకైన: {
        మాట_t badge2;

        badge2 = notification_ptr_get_ntfn_సందేశం_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_సమితి_ntfn_సందేశం_Identifier(ntfnPtr, badge2);
        ఆపు;
    }
    }
}

శూన్య receiveSignal(tcb_t *thread, cap_t cap, bool_t isBlocking)
{
    notification_t *ntfnPtr;

    ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    ఎంపిక (notification_ptr_get_state(ntfnPtr)) {
    స్థితి NtfnState_Idle:
    స్థితి Ntfn_State_నిరీక్షణ: {

        ఒకవేళ (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_సమితి_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_సమితి_blocking_Object(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            కాలపట్టిక_TCB(thread);

            /* Enqueue TCB */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#లేకపోతే
            tcb_వరుస_t ntfn_వరుస;
            ntfn_వరుస = ntfn_ptr_get_వరుస(ntfnPtr);
            ntfn_వరుస = tcbEPAppend(thread, ntfn_వరుస);

            notification_ptr_సమితి_state(ntfnPtr, Ntfn_State_నిరీక్షణ);
            ntfn_ptr_సమితి_వరుస(ntfnPtr, ntfn_వరుస);
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */

#నిర్వచితమైతే CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#అయితే_ముగింపు
        } లేకపోతే {
            doNBRecvFailedTransfer(thread);
        }

        ఆపు;
    }

    స్థితి Ntfn_State_చురుకైన:
        సమితి_రిజిస్టరు(
            thread, badge_రిజిస్టరు,
            notification_ptr_get_ntfn_సందేశం_Identifier(ntfnPtr));
        notification_ptr_సమితి_state(ntfnPtr, NtfnState_Idle);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        ఒకవేళ (thread->tcbSchedContext != కణుపు_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#అయితే_ముగింపు
        ఆపు;
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
స్థిర అంతఃవరుస శూన్య removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    సమితి_Thread_State(thread, ThreadState_Restart);
    ఒకవేళ (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        ప్రతిజ్ఞా(thread->tcbSchedContext != కణుపు_STATE(ksCurSC));
        ఒకవేళ (thread->tcbSchedContext != కణుపు_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possible_Switch_గమ్యం(thread);
}
#అయితే_ముగింపు

శూన్య cancel_All_సంకేతాలు(notification_t *ntfnPtr)
{
    ఒకవేళ (notification_ptr_get_state(ntfnPtr) == Ntfn_State_నిరీక్షణ) {
        /* Clear the queue and set all blocked threads to Restart */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        tcb_వరుస_t వరుస;
        tcb_t *thread, *తదుపరి;

        వరుస = ntfn_ptr_get_వరుస(ntfnPtr);

        కొరకు (thread = వరుస.తల; thread; thread = తదుపరి) {
            తదుపరి = thread->tcb_Sched_తదుపరి;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#లేకపోతే
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfn_వరుస_తల(ntfnPtr));

        notification_ptr_సమితి_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_సమితి_ntfn_వరుస_తల(ntfnPtr, 0);
        notification_ptr_సమితి_ntfn_వరుస_తోక(ntfnPtr, 0);

        కొరకు (; thread; thread = thread->tcbEPNext) {
            సమితి_Thread_State(thread, ThreadState_Restart);
            SCHED_వరుసలో_చేర్చు(thread);
        }
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

శూన్య cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    ప్రతిజ్ఞా(notification_ptr_get_state(ntfnPtr) == Ntfn_State_నిరీక్షణ);

    /* Dequeue TCB */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#లేకపోతే
    tcb_వరుస_t ntfn_వరుస;
    ntfn_వరుస = ntfn_ptr_get_వరుస(ntfnPtr);
    ntfn_వరుస = tcbEPDequeue(threadPtr, ntfn_వరుస);
    ntfn_ptr_సమితి_వరుస(ntfnPtr, ntfn_వరుస);

    /* Make notification object idle */
    ఒకవేళ (!ntfn_వరుస.తల) {
        notification_ptr_సమితి_state(ntfnPtr, NtfnState_Idle);
    }
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    సమితి_Thread_State(threadPtr, ThreadState_Inactive);
}

శూన్య completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    మాట_t badge;

    ఒకవేళ (likely(tcb && notification_ptr_get_state(ntfnPtr) == Ntfn_State_చురుకైన)) {
        badge = notification_ptr_get_ntfn_సందేశం_Identifier(ntfnPtr);
        సమితి_రిజిస్టరు(tcb, badge_రిజిస్టరు, badge);
        notification_ptr_సమితి_state(ntfnPtr, NtfnState_Idle);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        ఒకవేళ (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            ఒకవేళ (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != కణుపు_STATE(ksCurSC)) {
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
#అయితే_ముగింపు
    } లేకపోతే {
        fail("tried to complete signal with inactive notification object");
    }
}

స్థిర అంతఃవరుస శూన్య doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_సమితి_ntfn_Bound_TCB(ntfnPtr, (మాట_t) 0);
    tcbptr->tcbBoundNotification = ఖాళీ;
}

శూన్య unbindMaybeNotification(notification_t *ntfnPtr)
{
    tcb_t *boundTCB;
    boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);

    ఒకవేళ (boundTCB) {
        doUnbindNotification(ntfnPtr, boundTCB);
    }
}

శూన్య unbindNotification(tcb_t *tcb)
{
    notification_t *ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    ఒకవేళ (ntfnPtr) {
        doUnbindNotification(ntfnPtr, tcb);
    }
}

శూన్య bindNotification(tcb_t *tcb, notification_t *ntfnPtr)
{
    notification_ptr_సమితి_ntfn_Bound_TCB(ntfnPtr, (మాట_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_వరుస_t వరుస;
    tcb_వరుస_t new_వరుస;

    వరుస = ntfn_ptr_get_వరుస(ntfnPtr);
    new_వరుస = tcbAppend(thread, వరుస);
    ntfn_ptr_సమితి_వరుస(ntfnPtr, new_వరుస);
    notification_ptr_సమితి_state(ntfnPtr, Ntfn_State_నిరీక్షణ);
}

శూన్య tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_వరుస_t వరుస;
    tcb_వరుస_t new_వరుస;

    వరుస = ntfn_ptr_get_వరుస(ntfnPtr);
    new_వరుస = tcb_వరుస_remove(వరుస, thread);
    ntfn_ptr_సమితి_వరుస(ntfnPtr, new_వరుస);

    ఒకవేళ (tcb_వరుస_ఖాళీలు(new_వరుస)) {
        notification_ptr_సమితి_state(ntfnPtr, NtfnState_Idle);
    }
}

శూన్య reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_వరుస_t వరుస = ntfn_ptr_get_వరుస(ntfnPtr);
    వరుస = tcb_వరుస_remove(వరుస, thread);
    వరుస = tcbAppend(thread, వరుస);
    ntfn_ptr_సమితి_వరుస(ntfnPtr, వరుస);
}
#అయితే_ముగింపు
