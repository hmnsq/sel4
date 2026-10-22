/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>

#شامل <types.h>
#شامل <kernel/thread.h>
#شامل <object/structures.h>
#شامل <object/tcb.h>
#شامل <object/endpoint.h>
#شامل <model/statedata.h>
#شامل <machine/io.h>

#شامل <object/notification.h>

ساکن اندرونی tcb_قطار_t خالص ntfn_ptr_get_قطار(notification_t *ntfnPtr)
{
    tcb_قطار_t ntfn_قطار;

    ntfn_قطار.سر = (tcb_t *)notification_ptr_get_ntfn_قطار_سر(ntfnPtr);
    ntfn_قطار.انتہا = (tcb_t *)notification_ptr_get_ntfn_قطار_دم(ntfnPtr);

    واپس ntfn_قطار;
}

ساکن اندرونی خالی ntfn_ptr_سیٹ_قطار(notification_t *ntfnPtr, tcb_قطار_t ntfn_قطار)
{
    notification_ptr_سیٹ_ntfn_قطار_سر(ntfnPtr, (لفظ_t)ntfn_قطار.سر);
    notification_ptr_سیٹ_ntfn_قطار_دم(ntfnPtr, (لفظ_t)ntfn_قطار.انتہا);
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن اندرونی خالی maybeDonateSchedContext(tcb_t *tcb, notification_t *ntfnPtr)
{
    اگر (tcb->tcbSchedContext == تہی) {
        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        اگر (sc != تہی && sc->scTcb == تہی) {
            schedContext_donate(sc, tcb);
            schedContext_resume(sc);
        }
    }
}

#اگر_ختم

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#تعریف_کرو MCS_DO_IF_SC(tcb, ntfnPtr, بلاک) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    اگر (isSchedulable(tcb)) { \
        بلاک \
    }
#ورنہ
#تعریف_کرو MCS_DO_IF_SC(tcb, ntfnPtr, بلاک) \
    { \
        بلاک \
    }
#اگر_ختم

خالی sendSignal(notification_t *ntfnPtr, لفظ_t badge)
{
    انتخاب (notification_ptr_get_state(ntfnPtr)) {
    حالت NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        اگر (tcb) {
            اگر (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                سیٹ_Thread_State(tcb, ThreadState_Running);
                سیٹ_رجسٹر(tcb, badge_رجسٹر, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possible_Switch_مقصد(tcb);
                })
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
                اگر (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    أكد(tcb->tcbSchedContext != گرہ_STATE(ksCurSC));
                    اگر (tcb->tcbSchedContext != گرہ_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_VTX
            } ورنہ اگر (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
                اگر (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_سیٹ_فعال(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } ورنہ
#اگر_ختم /* ENABLE_SMP_SUPPORT */
                {
                    سیٹ_Thread_State(tcb, ThreadState_Running);
                    سیٹ_رجسٹر(tcb, badge_رجسٹر, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possible_Switch_مقصد(tcb);
                    })
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
                    اگر (tcb->tcbSchedContext != تہی && sc_فعال(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        اگر (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != گرہ_STATE(ksCurSC)) {
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
#اگر_ختم
                }
#اگر_ختم /* CONFIG_VTX */
            } ورنہ {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_سیٹ_فعال(ntfnPtr, badge);
            }
        } ورنہ {
            ntfn_سیٹ_فعال(ntfnPtr, badge);
        }
        توڑو;
    }
    حالت Ntfn_State_انتظار: {
        tcb_قطار_t ntfn_قطار;
        tcb_t *dest;

        ntfn_قطار = ntfn_ptr_get_قطار(ntfnPtr);
        dest = ntfn_قطار.سر;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        أكد(dest);

        /* Dequeue TCB */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#ورنہ
        ntfn_قطار = tcbEPDequeue(dest, ntfn_قطار);
        ntfn_ptr_سیٹ_قطار(ntfnPtr, ntfn_قطار);

        /* set the thread state to idle if the queue is empty */
        اگر (!ntfn_قطار.سر) {
            notification_ptr_سیٹ_state(ntfnPtr, NtfnState_Idle);
        }
#اگر_ختم /* CONFIG_KERNEL_MCS */

        سیٹ_Thread_State(dest, ThreadState_Running);
        سیٹ_رجسٹر(dest, badge_رجسٹر, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possible_Switch_مقصد(dest);
        })

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        اگر (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(dest->tcbSchedContext != گرہ_STATE(ksCurSC));
            اگر (dest->tcbSchedContext != گرہ_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#اگر_ختم
        توڑو;
    }

    حالت Ntfn_State_فعال: {
        لفظ_t badge2;

        badge2 = notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_سیٹ_ntfn_پیغام_Identifier(ntfnPtr, badge2);
        توڑو;
    }
    }
}

خالی receiveSignal(tcb_t *thread, cap_t cap, bool_t isBlocking)
{
    notification_t *ntfnPtr;

    ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    انتخاب (notification_ptr_get_state(ntfnPtr)) {
    حالت NtfnState_Idle:
    حالت Ntfn_State_انتظار: {

        اگر (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_سیٹ_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_سیٹ_blocking_Object(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            جدول_وقت_TCB(thread);

            /* Enqueue TCB */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#ورنہ
            tcb_قطار_t ntfn_قطار;
            ntfn_قطار = ntfn_ptr_get_قطار(ntfnPtr);
            ntfn_قطار = tcbEPAppend(thread, ntfn_قطار);

            notification_ptr_سیٹ_state(ntfnPtr, Ntfn_State_انتظار);
            ntfn_ptr_سیٹ_قطار(ntfnPtr, ntfn_قطار);
#اگر_ختم /* CONFIG_KERNEL_MCS */

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#اگر_ختم
        } ورنہ {
            doNBRecvFailedTransfer(thread);
        }

        توڑو;
    }

    حالت Ntfn_State_فعال:
        سیٹ_رجسٹر(
            thread, badge_رجسٹر,
            notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr));
        notification_ptr_سیٹ_state(ntfnPtr, NtfnState_Idle);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        اگر (thread->tcbSchedContext != گرہ_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#اگر_ختم
        توڑو;
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن اندرونی خالی removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    سیٹ_Thread_State(thread, ThreadState_Restart);
    اگر (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        أكد(thread->tcbSchedContext != گرہ_STATE(ksCurSC));
        اگر (thread->tcbSchedContext != گرہ_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possible_Switch_مقصد(thread);
}
#اگر_ختم

خالی cancel_All_اشارات(notification_t *ntfnPtr)
{
    اگر (notification_ptr_get_state(ntfnPtr) == Ntfn_State_انتظار) {
        /* Clear the queue and set all blocked threads to Restart */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        tcb_قطار_t قطار;
        tcb_t *thread, *اگلا;

        قطار = ntfn_ptr_get_قطار(ntfnPtr);

        برائے (thread = قطار.سر; thread; thread = اگلا) {
            اگلا = thread->tcb_Sched_اگلا;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#ورنہ
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfn_قطار_سر(ntfnPtr));

        notification_ptr_سیٹ_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_سیٹ_ntfn_قطار_سر(ntfnPtr, 0);
        notification_ptr_سیٹ_ntfn_قطار_دم(ntfnPtr, 0);

        برائے (; thread; thread = thread->tcbEPNext) {
            سیٹ_Thread_State(thread, ThreadState_Restart);
            SCHED_قطار_میں_ڈالو(thread);
        }
#اگر_ختم /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

خالی cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    أكد(notification_ptr_get_state(ntfnPtr) == Ntfn_State_انتظار);

    /* Dequeue TCB */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#ورنہ
    tcb_قطار_t ntfn_قطار;
    ntfn_قطار = ntfn_ptr_get_قطار(ntfnPtr);
    ntfn_قطار = tcbEPDequeue(threadPtr, ntfn_قطار);
    ntfn_ptr_سیٹ_قطار(ntfnPtr, ntfn_قطار);

    /* Make notification object idle */
    اگر (!ntfn_قطار.سر) {
        notification_ptr_سیٹ_state(ntfnPtr, NtfnState_Idle);
    }
#اگر_ختم /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    سیٹ_Thread_State(threadPtr, ThreadState_Inactive);
}

خالی completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    لفظ_t badge;

    اگر (likely(tcb && notification_ptr_get_state(ntfnPtr) == Ntfn_State_فعال)) {
        badge = notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr);
        سیٹ_رجسٹر(tcb, badge_رجسٹر, badge);
        notification_ptr_سیٹ_state(ntfnPtr, NtfnState_Idle);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        اگر (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            اگر (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != گرہ_STATE(ksCurSC)) {
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
#اگر_ختم
    } ورنہ {
        fail("tried to complete signal with inactive notification object");
    }
}

ساکن اندرونی خالی doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_سیٹ_ntfn_Bound_TCB(ntfnPtr, (لفظ_t) 0);
    tcbptr->tcbBoundNotification = تہی;
}

خالی unbindMaybeNotification(notification_t *ntfnPtr)
{
    tcb_t *boundTCB;
    boundTCB = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);

    اگر (boundTCB) {
        doUnbindNotification(ntfnPtr, boundTCB);
    }
}

خالی unbindNotification(tcb_t *tcb)
{
    notification_t *ntfnPtr;
    ntfnPtr = tcb->tcbBoundNotification;

    اگر (ntfnPtr) {
        doUnbindNotification(ntfnPtr, tcb);
    }
}

خالی bindNotification(tcb_t *tcb, notification_t *ntfnPtr)
{
    notification_ptr_سیٹ_ntfn_Bound_TCB(ntfnPtr, (لفظ_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_قطار_t قطار;
    tcb_قطار_t new_قطار;

    قطار = ntfn_ptr_get_قطار(ntfnPtr);
    new_قطار = tcbAppend(thread, قطار);
    ntfn_ptr_سیٹ_قطار(ntfnPtr, new_قطار);
    notification_ptr_سیٹ_state(ntfnPtr, Ntfn_State_انتظار);
}

خالی tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_قطار_t قطار;
    tcb_قطار_t new_قطار;

    قطار = ntfn_ptr_get_قطار(ntfnPtr);
    new_قطار = tcb_قطار_remove(قطار, thread);
    ntfn_ptr_سیٹ_قطار(ntfnPtr, new_قطار);

    اگر (tcb_قطار_خالی_خانے(new_قطار)) {
        notification_ptr_سیٹ_state(ntfnPtr, NtfnState_Idle);
    }
}

خالی reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_قطار_t قطار = ntfn_ptr_get_قطار(ntfnPtr);
    قطار = tcb_قطار_remove(قطار, thread);
    قطار = tcbAppend(thread, قطار);
    ntfn_ptr_سیٹ_قطار(ntfnPtr, قطار);
}
#اگر_ختم
