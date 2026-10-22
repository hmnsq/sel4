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

ساكن مضمن tcb_قطار_t خالص ntfn_ptr_get_قطار(notification_t *ntfnPtr)
{
    tcb_قطار_t ntfn_قطار;

    ntfn_قطار.سر = (tcb_t *)notification_ptr_get_ntfn_قطار_سر(ntfnPtr);
    ntfn_قطار.پڇاڙي = (tcb_t *)notification_ptr_get_ntfn_قطار_پڇ(ntfnPtr);

    أرجع ntfn_قطار;
}

ساكن مضمن فراغ ntfn_ptr_سيٽ_قطار(notification_t *ntfnPtr, tcb_قطار_t ntfn_قطار)
{
    notification_ptr_سيٽ_ntfn_قطار_سر(ntfnPtr, (لفظ_t)ntfn_قطار.سر);
    notification_ptr_سيٽ_ntfn_قطار_پڇ(ntfnPtr, (لفظ_t)ntfn_قطار.پڇاڙي);
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
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

#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#وصف_ڪريو MCS_DO_IF_SC(tcb, ntfnPtr, بلاڪ) \
    maybeDonateSchedContext(tcb, ntfnPtr); \
    إذا (isSchedulable(tcb)) { \
        بلاڪ \
    }
#وإلا
#وصف_ڪريو MCS_DO_IF_SC(tcb, ntfnPtr, بلاڪ) \
    { \
        بلاڪ \
    }
#جيڪڏهن_پورو

فراغ sendSignal(notification_t *ntfnPtr, لفظ_t badge)
{
    بدل (notification_ptr_get_state(ntfnPtr)) {
    حالة NtfnState_Idle: {
        tcb_t *tcb = (tcb_t *)notification_ptr_get_ntfnBoundTCB(ntfnPtr);
        /* Check if we are bound and that thread is waiting for a message */
        إذا (tcb) {
            إذا (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_BlockedOnReceive) {
                /* Send and start thread running */
                cancelIPC(tcb);
                سيٽ_Thread_State(tcb, ThreadState_Running);
                سيٽ_رجسٽر(tcb, badge_رجسٽر, badge);
                MCS_DO_IF_SC(tcb, ntfnPtr, {
                    possible_Switch_منزل(tcb);
                })
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
                إذا (sc_sporadic(tcb->tcbSchedContext)) {
                    /* We know that the tcb can't have the current SC
                     * as its own SC as this point as it should still be
                     * associated with the current thread, or no thread.
                     * This check is added here to reduce the cost of
                     * proving this to be true as a short-term stop-gap. */
                    أكد(tcb->tcbSchedContext != ڳنڍ_STATE(ksCurSC));
                    إذا (tcb->tcbSchedContext != ڳنڍ_STATE(ksCurSC)) {
                        refill_unblock_check(tcb->tcbSchedContext);
                    }
                }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_VTX
            } وإلا إذا (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM) {
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
                إذا (tcb->tcbAffinity != getCurrentCPUIndex()) {
                    ntfn_سيٽ_سرگرم(ntfnPtr, badge);
                    doRemoteVMCheckBoundNotification(tcb->tcbAffinity, tcb);
                } وإلا
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
                {
                    سيٽ_Thread_State(tcb, ThreadState_Running);
                    سيٽ_رجسٽر(tcb, badge_رجسٽر, badge);
                    Arch_leaveVMAsyncTransfer(tcb);
                    MCS_DO_IF_SC(tcb, ntfnPtr, {
                        possible_Switch_منزل(tcb);
                    })
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
                    إذا (tcb->tcbSchedContext != عدم && sc_سرگرم(tcb->tcbSchedContext)) {
                        sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
                        إذا (tcb->tcbSchedContext == sc && sc_sporadic(sc) && tcb->tcbSchedContext != ڳنڍ_STATE(ksCurSC)) {
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
#جيڪڏهن_پورو
                }
#جيڪڏهن_پورو /* CONFIG_VTX */
            } وإلا {
                /* In particular, this path is taken when a thread
                 * is waiting on a reply cap since BlockedOnReply
                 * would also trigger this path. I.e, a thread
                 * with a bound notification will not be awakened
                 * by signals on that bound notification if it is
                 * in the middle of an seL4_Call.
                 */
                ntfn_سيٽ_سرگرم(ntfnPtr, badge);
            }
        } وإلا {
            ntfn_سيٽ_سرگرم(ntfnPtr, badge);
        }
        اكسر;
    }
    حالة Ntfn_State_انتظار: {
        tcb_قطار_t ntfn_قطار;
        tcb_t *dest;

        ntfn_قطار = ntfn_ptr_get_قطار(ntfnPtr);
        dest = ntfn_قطار.سر;

        /* Haskell error "WaitingNtfn Notification must have non-empty queue" */
        أكد(dest);

        /* Dequeue TCB */
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        tcbNTFNDequeue(dest, ntfnPtr);
#وإلا
        ntfn_قطار = tcbEPDequeue(dest, ntfn_قطار);
        ntfn_ptr_سيٽ_قطار(ntfnPtr, ntfn_قطار);

        /* set the thread state to idle if the queue is empty */
        إذا (!ntfn_قطار.سر) {
            notification_ptr_سيٽ_state(ntfnPtr, NtfnState_Idle);
        }
#جيڪڏهن_پورو /* CONFIG_KERNEL_MCS */

        سيٽ_Thread_State(dest, ThreadState_Running);
        سيٽ_رجسٽر(dest, badge_رجسٽر, badge);
        MCS_DO_IF_SC(dest, ntfnPtr, {
            possible_Switch_منزل(dest);
        })

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        إذا (sc_sporadic(dest->tcbSchedContext)) {
            /* We know that the receiver can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(dest->tcbSchedContext != ڳنڍ_STATE(ksCurSC));
            إذا (dest->tcbSchedContext != ڳنڍ_STATE(ksCurSC)) {
                refill_unblock_check(dest->tcbSchedContext);
            }
        }
#جيڪڏهن_پورو
        اكسر;
    }

    حالة Ntfn_State_سرگرم: {
        لفظ_t badge2;

        badge2 = notification_ptr_get_ntfn_نياپو_Identifier(ntfnPtr);
        badge2 |= badge;

        notification_ptr_سيٽ_ntfn_نياپو_Identifier(ntfnPtr, badge2);
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
            thread_state_ptr_سيٽ_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnNotification);
            thread_state_ptr_سيٽ_blocking_Object(&thread->tcbState,
                                                NTFN_REF(ntfnPtr));
            وقت_جدول_TCB(thread);

            /* Enqueue TCB */
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
            tcbNTFNAppend(thread, ntfnPtr);
#وإلا
            tcb_قطار_t ntfn_قطار;
            ntfn_قطار = ntfn_ptr_get_قطار(ntfnPtr);
            ntfn_قطار = tcbEPAppend(thread, ntfn_قطار);

            notification_ptr_سيٽ_state(ntfnPtr, Ntfn_State_انتظار);
            ntfn_ptr_سيٽ_قطار(ntfnPtr, ntfn_قطار);
#جيڪڏهن_پورو /* CONFIG_KERNEL_MCS */

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
            maybeReturnSchedContext(ntfnPtr, thread);
#جيڪڏهن_پورو
        } وإلا {
            doNBRecvFailedTransfer(thread);
        }

        اكسر;
    }

    حالة Ntfn_State_سرگرم:
        سيٽ_رجسٽر(
            thread, badge_رجسٽر,
            notification_ptr_get_ntfn_نياپو_Identifier(ntfnPtr));
        notification_ptr_سيٽ_state(ntfnPtr, NtfnState_Idle);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        maybeDonateSchedContext(thread, ntfnPtr);
        // If the SC has been donated to the current thread (in a reply_recv, send_recv scenario) then
        // we may need to perform refill_unblock_check if the SC is becoming activated.
        إذا (thread->tcbSchedContext != ڳنڍ_STATE(ksCurSC) && sc_sporadic(thread->tcbSchedContext)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
#جيڪڏهن_پورو
        اكسر;
    }
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
ساكن مضمن فراغ removeAndRestartNTFNQueuedThread(tcb_t *thread, notification_t *ntfnPtr)
{
    tcbNTFNDequeue(thread, ntfnPtr);
    سيٽ_Thread_State(thread, ThreadState_Restart);
    إذا (sc_sporadic(thread->tcbSchedContext)) {
        /* We know that the thread can't have the current SC as its own SC at
         * this point as it should still be associated with the current thread,
         * or no thread. This check is added here to reduce the cost of proving
         * this to be true as a short-term stop-gap. */
        أكد(thread->tcbSchedContext != ڳنڍ_STATE(ksCurSC));
        إذا (thread->tcbSchedContext != ڳنڍ_STATE(ksCurSC)) {
            refill_unblock_check(thread->tcbSchedContext);
        }
    }
    possible_Switch_منزل(thread);
}
#جيڪڏهن_پورو

فراغ cancel_All_سگنل(notification_t *ntfnPtr)
{
    إذا (notification_ptr_get_state(ntfnPtr) == Ntfn_State_انتظار) {
        /* Clear the queue and set all blocked threads to Restart */
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        tcb_قطار_t قطار;
        tcb_t *thread, *ايندڙ;

        قطار = ntfn_ptr_get_قطار(ntfnPtr);

        لكل (thread = قطار.سر; thread; thread = ايندڙ) {
            ايندڙ = thread->tcb_Sched_ايندڙ;
            removeAndRestartNTFNQueuedThread(thread, ntfnPtr);
        }
#وإلا
        tcb_t *thread = TCB_PTR(notification_ptr_get_ntfn_قطار_سر(ntfnPtr));

        notification_ptr_سيٽ_state(ntfnPtr, NtfnState_Idle);
        notification_ptr_سيٽ_ntfn_قطار_سر(ntfnPtr, 0);
        notification_ptr_سيٽ_ntfn_قطار_پڇ(ntfnPtr, 0);

        لكل (; thread; thread = thread->tcbEPNext) {
            سيٽ_Thread_State(thread, ThreadState_Restart);
            SCHED_قطار_اندر_وجهو(thread);
        }
#جيڪڏهن_پورو /* CONFIG_KERNEL_MCS */
        rescheduleRequired();
    }
}

فراغ cancelSignal(tcb_t *threadPtr, notification_t *ntfnPtr)
{

    /* Haskell error "cancelSignal: notification object must be in a waiting" state */
    أكد(notification_ptr_get_state(ntfnPtr) == Ntfn_State_انتظار);

    /* Dequeue TCB */
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    tcbNTFNDequeue(threadPtr, ntfnPtr);
#وإلا
    tcb_قطار_t ntfn_قطار;
    ntfn_قطار = ntfn_ptr_get_قطار(ntfnPtr);
    ntfn_قطار = tcbEPDequeue(threadPtr, ntfn_قطار);
    ntfn_ptr_سيٽ_قطار(ntfnPtr, ntfn_قطار);

    /* Make notification object idle */
    إذا (!ntfn_قطار.سر) {
        notification_ptr_سيٽ_state(ntfnPtr, NtfnState_Idle);
    }
#جيڪڏهن_پورو /* CONFIG_KERNEL_MCS */

    /* Make thread inactive */
    سيٽ_Thread_State(threadPtr, ThreadState_Inactive);
}

فراغ completeSignal(notification_t *ntfnPtr, tcb_t *tcb)
{
    لفظ_t badge;

    إذا (likely(tcb && notification_ptr_get_state(ntfnPtr) == Ntfn_State_سرگرم)) {
        badge = notification_ptr_get_ntfn_نياپو_Identifier(ntfnPtr);
        سيٽ_رجسٽر(tcb, badge_رجسٽر, badge);
        notification_ptr_سيٽ_state(ntfnPtr, NtfnState_Idle);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        maybeDonateSchedContext(tcb, ntfnPtr);
        إذا (sc_sporadic(tcb->tcbSchedContext)) {
            sched_context_t *sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
            إذا (tcb->tcbSchedContext == sc && tcb->tcbSchedContext != ڳنڍ_STATE(ksCurSC)) {
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
#جيڪڏهن_پورو
    } وإلا {
        fail("tried to complete signal with inactive notification object");
    }
}

ساكن مضمن فراغ doUnbindNotification(notification_t *ntfnPtr, tcb_t *tcbptr)
{
    notification_ptr_سيٽ_ntfn_Bound_TCB(ntfnPtr, (لفظ_t) 0);
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
    notification_ptr_سيٽ_ntfn_Bound_TCB(ntfnPtr, (لفظ_t)tcb);
    tcb->tcbBoundNotification = ntfnPtr;
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
فراغ tcbNTFNAppend(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_قطار_t قطار;
    tcb_قطار_t new_قطار;

    قطار = ntfn_ptr_get_قطار(ntfnPtr);
    new_قطار = tcbAppend(thread, قطار);
    ntfn_ptr_سيٽ_قطار(ntfnPtr, new_قطار);
    notification_ptr_سيٽ_state(ntfnPtr, Ntfn_State_انتظار);
}

فراغ tcbNTFNDequeue(tcb_t *thread, notification_t *ntfnPtr)
{
    tcb_قطار_t قطار;
    tcb_قطار_t new_قطار;

    قطار = ntfn_ptr_get_قطار(ntfnPtr);
    new_قطار = tcb_قطار_remove(قطار, thread);
    ntfn_ptr_سيٽ_قطار(ntfnPtr, new_قطار);

    إذا (tcb_قطار_خالي_جايون(new_قطار)) {
        notification_ptr_سيٽ_state(ntfnPtr, NtfnState_Idle);
    }
}

فراغ reorderNTFN(notification_t *ntfnPtr, tcb_t *thread)
{
    tcb_قطار_t قطار = ntfn_ptr_get_قطار(ntfnPtr);
    قطار = tcb_قطار_remove(قطار, thread);
    قطار = tcbAppend(thread, قطار);
    ntfn_ptr_سيٽ_قطار(ntfnPtr, قطار);
}
#جيڪڏهن_پورو
