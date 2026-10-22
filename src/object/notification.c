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

ساكن مضمن tcb_صف_t خالص ntfn_ptr_get_صف(notification_t *ntfnPtr)
{
    tcb_صف_t ntfn_صف;

    ntfn_صف.سر = (tcb_t *)notification_ptr_get_ntfn_صف_سر(ntfnPtr);
    ntfn_صف.انتها = (tcb_t *)notification_ptr_get_ntfn_صف_دم(ntfnPtr);

    أرجع ntfn_صف;
}

ساكن مضمن فراغ ntfn_ptr_مجموعه_صف(notification_t *ntfnPtr, tcb_صف_t ntfn_صف)
{
    notification_ptr_مجموعه_ntfn_صف_سر(ntfnPtr, (کلمه_t)ntfn_صف.سر);
    notification_ptr_مجموعه_ntfn_صف_دم(ntfnPtr, (کلمه_t)ntfn_صف.انتها);
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
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

#پایان_اگر

#اگر_تعریف_شده CONFIG_KERNEL_MCS
#تعریف_کن MCS_DO_IF_SC(tcb, ntfnPtr, بلوک) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    إذا (isSchedulable(tcb)) { \
        بلوک \
    }
#وإلا
#تعریف_کن MCS_DO_IF_SC(tcb, ntfnPtr, بلوک) \
    { \
        بلوک \
    }
#پایان_اگر

فراغ sendSignal(notification_t *ntfnPtr, کلمه_t badge)
{
    بدل (notification_ptr_get_state(ntfnPtr)) {
    حالة NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        إذا (tcb) {
            إذا (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                مجموعه_Thread_State(tcb, ThreadState_Running);
                مجموعه_ثبات(tcb, badge_ثبات, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possible_Switch_مقصد(tcb);
                })
#اگر_تعریف_شده CONFIG_KERNEL_MCS
                إذا (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    أكد(tcb->tcbSchedContext != گره_STATE(ksCurSC));
                    إذا (tcb->tcbSchedContext != گره_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#پایان_اگر
#اگر_تعریف_شده CONFIG_VTX
            } وإلا إذا (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
                إذا (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_مجموعه_فعال(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } وإلا
#پایان_اگر /* ENABLE_SMP_SUPPORT */
                {
                    مجموعه_Thread_State(tcb, ThreadState_Running);
                    مجموعه_ثبات(tcb, badge_ثبات, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possible_Switch_مقصد(tcb);
                    })
#اگر_تعریف_شده CONFIG_KERNEL_MCS
                    إذا (tcb->tcbSchedContext != عدم && sc_فعال(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        إذا (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != گره_STATE(ksCurSC)) {
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
#پایان_اگر
                }
#پایان_اگر /* CONFIG_VTX */
            } وإلا {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_مجموعه_فعال(ntfnPtr, badge);
            }
        } وإلا {
            ntfn_مجموعه_فعال(ntfnPtr, badge);
        }
        اكسر;
    }
    حالة Ntfn_State_انتظار: {
        tcb_صف_t ntfn_صف;
        tcb_t *dest;

        ntfn_صف = ntfn_ptr_get_صف(ntfnPtr);
        dest = ntfn_صف.سر;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        أكد(dest);

        /* Dequeue TCB */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#وإلا
        ntfn_صف = tcbEPDequeue(dest, ntfn_صف);
        ntfn_ptr_مجموعه_صف(ntfnPtr, ntfn_صف);

        /* set the thread state to idle if the queue is empty */
        إذا (!ntfn_صف.سر) {
            notification_ptr_مجموعه_state(ntfnPtr, NtfnState_Idle);
        }
#پایان_اگر /* CONFIG_KERNEL_MCS */

        مجموعه_Thread_State(dest, ThreadState_Running);
        مجموعه_ثبات(dest, badge_ثبات, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possible_Switch_مقصد(dest);
        })

#اگر_تعریف_شده CONFIG_KERNEL_MCS
        إذا (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(dest->tcbSchedContext != گره_STATE(ksCurSC));
            إذا (dest->tcbSchedContext != گره_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#پایان_اگر
        اكسر;
    }

    حالة Ntfn_State_فعال: {
        کلمه_t badge2;

        badge2 = notification_ptr_get_ntfn_پیام_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_مجموعه_ntfn_پیام_Identifier(ntfnPtr, badge2);
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
    حالة Ntfn_State_انتظار: {

        إذا (isBlocking) {
            /* Block thread on notification object */
            thread_state_ptr_مجموعه_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_مجموعه_blocking_Object(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            زمانبندی_TCB(thread);

            /* Enqueue TCB */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#وإلا
            tcb_صف_t ntfn_صف;
            ntfn_صف = ntfn_ptr_get_صف(ntfnPtr);
            ntfn_صف = tcbEPAppend(thread, ntfn_صف);

            notification_ptr_مجموعه_state(ntfnPtr, Ntfn_State_انتظار);
            ntfn_ptr_مجموعه_صف(ntfnPtr, ntfn_صف);
#پایان_اگر /* CONFIG_KERNEL_MCS */

#اگر_تعریف_شده CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#پایان_اگر
        } وإلا {
            doNBRecvFailedTransfer(thread);
        }

        اكسر;
    }

    حالة Ntfn_State_فعال:
        مجموعه_ثبات(
            thread, badge_ثبات,
            notification_ptr_get_ntfn_پیام_Identifier(ntfnPtr));
        notification_ptr_مجموعه_state(ntfnPtr, NtfnState_Idle);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        إذا (thread->tcbSchedContext != گره_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#پایان_اگر
        اكسر;
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
ساكن مضمن فراغ removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    مجموعه_Thread_State(thread, ThreadState_Restart);
    إذا (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        أكد(thread->tcbSchedContext != گره_STATE(ksCurSC));
        إذا (thread->tcbSchedContext != گره_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possible_Switch_مقصد(thread);
}
#پایان_اگر

فراغ cancel_All_سیگنالها(notification_t *ntfnPtr)
{
    إذا (notification_ptr_get_state(ntfnPtr) == Ntfn_State_انتظار) {
        /* Clear the queue and set all blocked threads to Restart */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        tcb_صف_t صف;
        tcb_t *thread, *بعدی;

        صف = ntfn_ptr_get_صف(ntfnPtr);

        لكل (thread = صف.سر; thread; thread = بعدی) {
            بعدی = thread->tcb_Sched_بعدی;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#وإلا
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfn_صف_سر(ntfnPtr));

        notification_ptr_مجموعه_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_مجموعه_ntfn_صف_سر(ntfnPtr, 0);
        notification_ptr_مجموعه_ntfn_صف_دم(ntfnPtr, 0);

        لكل (; thread; thread = thread->tcbEPNext) {
            مجموعه_Thread_State(thread, ThreadState_Restart);
            SCHED_افزودن_به_صف(thread);
        }
#پایان_اگر /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

فراغ cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    أكد(notification_ptr_get_state(ntfnPtr) == Ntfn_State_انتظار);

    /* Dequeue TCB */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#وإلا
    tcb_صف_t ntfn_صف;
    ntfn_صف = ntfn_ptr_get_صف(ntfnPtr);
    ntfn_صف = tcbEPDequeue(threadPtr, ntfn_صف);
    ntfn_ptr_مجموعه_صف(ntfnPtr, ntfn_صف);

    /* Make notification object idle */
    إذا (!ntfn_صف.سر) {
        notification_ptr_مجموعه_state(ntfnPtr, NtfnState_Idle);
    }
#پایان_اگر /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    مجموعه_Thread_State(threadPtr, ThreadState_Inactive);
}

فراغ completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    کلمه_t badge;

    إذا (likely(tcb && notification_ptr_get_state(ntfnPtr) == Ntfn_State_فعال)) {
        badge = notification_ptr_get_ntfn_پیام_Identifier(ntfnPtr);
        مجموعه_ثبات(tcb, badge_ثبات, badge);
        notification_ptr_مجموعه_state(ntfnPtr, NtfnState_Idle);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        إذا (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            إذا (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != گره_STATE(ksCurSC)) {
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
#پایان_اگر
    } وإلا {
        fail("tried to complete signal with inactive notification object");
    }
}

ساكن مضمن فراغ doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_مجموعه_ntfn_Bound_TCB(ntfnPtr, (کلمه_t) 0);
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
    notification_ptr_مجموعه_ntfn_Bound_TCB(ntfnPtr, (کلمه_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_صف_t صف;
    tcb_صف_t new_صف;

    صف = ntfn_ptr_get_صف(ntfnPtr);
    new_صف = tcbAppend(thread, صف);
    ntfn_ptr_مجموعه_صف(ntfnPtr, new_صف);
    notification_ptr_مجموعه_state(ntfnPtr, Ntfn_State_انتظار);
}

فراغ tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_صف_t صف;
    tcb_صف_t new_صف;

    صف = ntfn_ptr_get_صف(ntfnPtr);
    new_صف = tcb_صف_remove(صف, thread);
    ntfn_ptr_مجموعه_صف(ntfnPtr, new_صف);

    إذا (tcb_صف_خالی(new_صف)) {
        notification_ptr_مجموعه_state(ntfnPtr, NtfnState_Idle);
    }
}

فراغ reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_صف_t صف = ntfn_ptr_get_صف(ntfnPtr);
    صف = tcb_صف_remove(صف, thread);
    صف = tcbAppend(thread, صف);
    ntfn_ptr_مجموعه_صف(ntfnPtr, صف);
}
#پایان_اگر
