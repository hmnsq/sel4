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

ساكن مضمن tcb_کتار_t خالص ntfn_ptr_get_کتار(notification_t *ntfnPtr)
{
    tcb_کتار_t ntfn_کتار;

    ntfn_کتار.سر = (tcb_t *)notification_ptr_get_ntfn_کتار_سر(ntfnPtr);
    ntfn_کتار.پای_ټکی = (tcb_t *)notification_ptr_get_ntfn_کتار_لکۍ(ntfnPtr);

    أرجع ntfn_کتار;
}

ساكن مضمن فراغ ntfn_ptr_سېټ_کتار(notification_t *ntfnPtr, tcb_کتار_t ntfn_کتار)
{
    notification_ptr_سېټ_ntfn_کتار_سر(ntfnPtr, (کلمه_t)ntfn_کتار.سر);
    notification_ptr_سېټ_ntfn_کتار_لکۍ(ntfnPtr, (کلمه_t)ntfn_کتار.پای_ټکی);
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
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

#د_که_پای

#که_تعریف_شوی CONFIG_KERNEL_MCS
#تعریف_کړه MCS_DO_IF_SC(tcb, ntfnPtr, بلاک) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    إذا (isSchedulable(tcb)) { \
        بلاک \
    }
#وإلا
#تعریف_کړه MCS_DO_IF_SC(tcb, ntfnPtr, بلاک) \
    { \
        بلاک \
    }
#د_که_پای

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
                سېټ_Thread_State(tcb, ThreadState_Running);
                سېټ_ثبتوونکی(tcb, badge_ثبتوونکی, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possible_Switch_منزل(tcb);
                })
#که_تعریف_شوی CONFIG_KERNEL_MCS
                إذا (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    أكد(tcb->tcbSchedContext != غوټه_STATE(ksCurSC));
                    إذا (tcb->tcbSchedContext != غوټه_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#د_که_پای
#که_تعریف_شوی CONFIG_VTX
            } وإلا إذا (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#که_تعریف_شوی ENABLE_SMP_SUPPORT
                إذا (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_سېټ_فعال(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } وإلا
#د_که_پای /* ENABLE_SMP_SUPPORT */
                {
                    سېټ_Thread_State(tcb, ThreadState_Running);
                    سېټ_ثبتوونکی(tcb, badge_ثبتوونکی, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possible_Switch_منزل(tcb);
                    })
#که_تعریف_شوی CONFIG_KERNEL_MCS
                    إذا (tcb->tcbSchedContext != عدم && sc_فعال(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        إذا (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != غوټه_STATE(ksCurSC)) {
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
#د_که_پای
                }
#د_که_پای /* CONFIG_VTX */
            } وإلا {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_سېټ_فعال(ntfnPtr, badge);
            }
        } وإلا {
            ntfn_سېټ_فعال(ntfnPtr, badge);
        }
        اكسر;
    }
    حالة Ntfn_State_انتظار: {
        tcb_کتار_t ntfn_کتار;
        tcb_t *dest;

        ntfn_کتار = ntfn_ptr_get_کتار(ntfnPtr);
        dest = ntfn_کتار.سر;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        أكد(dest);

        /* Dequeue TCB */
#که_تعریف_شوی CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#وإلا
        ntfn_کتار = tcbEPDequeue(dest, ntfn_کتار);
        ntfn_ptr_سېټ_کتار(ntfnPtr, ntfn_کتار);

        /* set the thread state to idle if the queue is empty */
        إذا (!ntfn_کتار.سر) {
            notification_ptr_سېټ_state(ntfnPtr, NtfnState_Idle);
        }
#د_که_پای /* CONFIG_KERNEL_MCS */

        سېټ_Thread_State(dest, ThreadState_Running);
        سېټ_ثبتوونکی(dest, badge_ثبتوونکی, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possible_Switch_منزل(dest);
        })

#که_تعریف_شوی CONFIG_KERNEL_MCS
        إذا (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(dest->tcbSchedContext != غوټه_STATE(ksCurSC));
            إذا (dest->tcbSchedContext != غوټه_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#د_که_پای
        اكسر;
    }

    حالة Ntfn_State_فعال: {
        کلمه_t badge2;

        badge2 = notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_سېټ_ntfn_پیغام_Identifier(ntfnPtr, badge2);
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
            thread_state_ptr_سېټ_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_سېټ_blocking_Object(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            مهالوېش_TCB(thread);

            /* Enqueue TCB */
#که_تعریف_شوی CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#وإلا
            tcb_کتار_t ntfn_کتار;
            ntfn_کتار = ntfn_ptr_get_کتار(ntfnPtr);
            ntfn_کتار = tcbEPAppend(thread, ntfn_کتار);

            notification_ptr_سېټ_state(ntfnPtr, Ntfn_State_انتظار);
            ntfn_ptr_سېټ_کتار(ntfnPtr, ntfn_کتار);
#د_که_پای /* CONFIG_KERNEL_MCS */

#که_تعریف_شوی CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#د_که_پای
        } وإلا {
            doNBRecvFailedTransfer(thread);
        }

        اكسر;
    }

    حالة Ntfn_State_فعال:
        سېټ_ثبتوونکی(
            thread, badge_ثبتوونکی,
            notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr));
        notification_ptr_سېټ_state(ntfnPtr, NtfnState_Idle);
#که_تعریف_شوی CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        إذا (thread->tcbSchedContext != غوټه_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#د_که_پای
        اكسر;
    }
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
ساكن مضمن فراغ removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    سېټ_Thread_State(thread, ThreadState_Restart);
    إذا (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        أكد(thread->tcbSchedContext != غوټه_STATE(ksCurSC));
        إذا (thread->tcbSchedContext != غوټه_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possible_Switch_منزل(thread);
}
#د_که_پای

فراغ cancel_All_سیګنالونه(notification_t *ntfnPtr)
{
    إذا (notification_ptr_get_state(ntfnPtr) == Ntfn_State_انتظار) {
        /* Clear the queue and set all blocked threads to Restart */
#که_تعریف_شوی CONFIG_KERNEL_MCS
        tcb_کتار_t کتار;
        tcb_t *thread, *بل;

        کتار = ntfn_ptr_get_کتار(ntfnPtr);

        لكل (thread = کتار.سر; thread; thread = بل) {
            بل = thread->tcb_Sched_بل;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#وإلا
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfn_کتار_سر(ntfnPtr));

        notification_ptr_سېټ_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_سېټ_ntfn_کتار_سر(ntfnPtr, 0);
        notification_ptr_سېټ_ntfn_کتار_لکۍ(ntfnPtr, 0);

        لكل (; thread; thread = thread->tcbEPNext) {
            سېټ_Thread_State(thread, ThreadState_Restart);
            SCHED_کتار_ته_ورزیاتول(thread);
        }
#د_که_پای /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

فراغ cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    أكد(notification_ptr_get_state(ntfnPtr) == Ntfn_State_انتظار);

    /* Dequeue TCB */
#که_تعریف_شوی CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#وإلا
    tcb_کتار_t ntfn_کتار;
    ntfn_کتار = ntfn_ptr_get_کتار(ntfnPtr);
    ntfn_کتار = tcbEPDequeue(threadPtr, ntfn_کتار);
    ntfn_ptr_سېټ_کتار(ntfnPtr, ntfn_کتار);

    /* Make notification object idle */
    إذا (!ntfn_کتار.سر) {
        notification_ptr_سېټ_state(ntfnPtr, NtfnState_Idle);
    }
#د_که_پای /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    سېټ_Thread_State(threadPtr, ThreadState_Inactive);
}

فراغ completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    کلمه_t badge;

    إذا (likely(tcb && notification_ptr_get_state(ntfnPtr) == Ntfn_State_فعال)) {
        badge = notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr);
        سېټ_ثبتوونکی(tcb, badge_ثبتوونکی, badge);
        notification_ptr_سېټ_state(ntfnPtr, NtfnState_Idle);
#که_تعریف_شوی CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        إذا (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            إذا (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != غوټه_STATE(ksCurSC)) {
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
#د_که_پای
    } وإلا {
        fail("tried to complete signal with inactive notification object");
    }
}

ساكن مضمن فراغ doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_سېټ_ntfn_Bound_TCB(ntfnPtr, (کلمه_t) 0);
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
    notification_ptr_سېټ_ntfn_Bound_TCB(ntfnPtr, (کلمه_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_کتار_t کتار;
    tcb_کتار_t new_کتار;

    کتار = ntfn_ptr_get_کتار(ntfnPtr);
    new_کتار = tcbAppend(thread, کتار);
    ntfn_ptr_سېټ_کتار(ntfnPtr, new_کتار);
    notification_ptr_سېټ_state(ntfnPtr, Ntfn_State_انتظار);
}

فراغ tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_کتار_t کتار;
    tcb_کتار_t new_کتار;

    کتار = ntfn_ptr_get_کتار(ntfnPtr);
    new_کتار = tcb_کتار_remove(کتار, thread);
    ntfn_ptr_سېټ_کتار(ntfnPtr, new_کتار);

    إذا (tcb_کتار_تشه(new_کتار)) {
        notification_ptr_سېټ_state(ntfnPtr, NtfnState_Idle);
    }
}

فراغ reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_کتار_t کتار = ntfn_ptr_get_کتار(ntfnPtr);
    کتار = tcb_کتار_remove(کتار, thread);
    کتار = tcbAppend(thread, کتار);
    ntfn_ptr_سېټ_کتار(ntfnPtr, کتار);
}
#د_که_پای
