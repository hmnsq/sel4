/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <string.h>
#ضمن <sel4/constants.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <machine/registerset.h>
#ضمن <model/statedata.h>
#ضمن <object/notification.h>
#ضمن <object/cnode.h>
#ضمن <object/endpoint.h>
#ضمن <object/tcb.h>

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
فراغ sendIPC(bool_t blocking, bool_t do_call, لفظ_t badge,
             bool_t canGrant, bool_t can_Grant_جواب, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#وإلا
فراغ sendIPC(bool_t blocking, bool_t do_call, لفظ_t badge,
             bool_t canGrant, bool_t can_Grant_جواب, tcb_t *thread, endpoint_t *epptr)
#جے_انت
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
    حالة EPState_Send:
        إذا (blocking) {

            /* Set thread state to BlockedOnSend */
            thread_state_ptr_سیٹ_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnSend);
            thread_state_ptr_سیٹ_blocking_Object(
                &thread->tcbState, EP_REF(epptr));
            thread_state_ptr_سیٹ_blocking_IPCBadge(
                &thread->tcbState, badge);
            thread_state_ptr_سیٹ_blocking_IPCCan_Grant(
                &thread->tcbState, canGrant);
            thread_state_ptr_سیٹ_blocking_IPCCan_Grant_جواب(
                &thread->tcbState, can_Grant_جواب);
            thread_state_ptr_سیٹ_blocking_IPCIs_Call(
                &thread->tcbState, do_call);

            ویلا_سارنی_TCB(thread);

            /* Place calling thread in endpoint queue */
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
            tcbEPAppend(thread, epptr, EPState_Send);
#وإلا
            tcb_قطار_t قطار;
            قطار = ep_ptr_get_قطار(epptr);
            قطار = tcbEPAppend(thread, قطار);
            endpoint_ptr_سیٹ_state(epptr, EPState_Send);
            ep_ptr_سیٹ_قطار(epptr, قطار);
#جے_انت /* CONFIG_KERNEL_MCS */

        }
        اكسر;

    حالة EPState_Recv: {
        tcb_قطار_t قطار;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        قطار = ep_ptr_get_قطار(epptr);
        dest = قطار.سر;

        /* Haskell error "Receive endpoint queue must not be empty" */
        أكد(dest);

        /* Dequeue the first TCB */
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#وإلا
        قطار = tcbEPDequeue(dest, قطار);
        ep_ptr_سیٹ_قطار(epptr, قطار);

        إذا (!قطار.سر) {
            endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
        }
#جے_انت /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        جواب_t *جواب = جواب_PTR(thread_state_get_جواب_Object(dest->tcbState));
        إذا (جواب) {
            جواب_unlink(جواب, dest);
        }

        إذا (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            إذا (جواب != عدم && (canGrant || can_Grant_جواب)) {
                جواب_ڈالو(thread, dest, جواب, canDonate);
            } وإلا {
                سیٹ_Thread_State(thread, ThreadState_Inactive);
            }
        } وإلا إذا (canDonate && dest->tcbSchedContext == عدم) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        سیٹ_Thread_State(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        أكد(dest_sc == عدم || refill_sufficient(dest_sc, 0));
        أكد(dest_sc == عدم || refill_ready(dest_sc));
        إذا (sc_sporadic(dest_sc) && dest_sc != گرہ_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possible_Switch_منزل(dest);
#وإلا
        bool_t جواب_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        سیٹ_Thread_State(dest, ThreadState_Running);
        possible_Switch_منزل(dest);

        إذا (do_call) {
            إذا (canGrant || can_Grant_جواب) {
                تیاری_Caller_Cap(thread, dest, جواب_Can_Grant);
            } وإلا {
                سیٹ_Thread_State(thread, ThreadState_Inactive);
            }
        }
#جے_انت
        اكسر;
    }
    }
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
فراغ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t جواب_Cap)
#وإلا
فراغ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#جے_انت
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    أكد(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    جواب_t *جواب_Ptr = عدم;
    إذا (cap_get_capType(جواب_Cap) == cap_جواب_cap) {
        جواب_Ptr = جواب_PTR(cap_جواب_cap_get_cap_جواب_Ptr(جواب_Cap));
        tcb_t *جواب_tcb = جواب_Ptr->جواب_TCB;
        إذا (unlikely(جواب_tcb != عدم && جواب_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(جواب_tcb);
        }
    }
#جے_انت

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    إذا (ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_سرگرم) {
        completeSignal(ntfnPtr, thread);
    } وإلا {
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        إذا (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#جے_انت
        بدل (endpoint_ptr_get_state(epptr)) {
        حالة EPState_Idle:
        حالة EPState_Recv: {

            إذا (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_سیٹ_ts_Type(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_سیٹ_blocking_Object(
                    &thread->tcbState, EP_REF(epptr));
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
                thread_state_ptr_سیٹ_جواب_Object(&thread->tcbState, جواب_REF(جواب_Ptr));
#وإلا
                thread_state_ptr_سیٹ_blocking_IPCCan_Grant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#جے_انت
                ویلا_سارنی_TCB(thread);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
                إذا (جواب_Ptr) {
                    جواب_Ptr->جواب_TCB = thread;
                }
#جے_انت

                /* Place calling thread in endpoint queue */
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#وإلا
                tcb_قطار_t قطار;
                قطار = ep_ptr_get_قطار(epptr);
                قطار = tcbEPAppend(thread, قطار);
                endpoint_ptr_سیٹ_state(epptr, EPState_Recv);
                ep_ptr_سیٹ_قطار(epptr, قطار);
#جے_انت /* CONFIG_KERNEL_MCS */
            } وإلا {
                doNBRecvFailedTransfer(thread);
            }
            اكسر;
        }

        حالة EPState_Send: {
            tcb_قطار_t قطار;
            tcb_t *sender;
            لفظ_t badge;
            bool_t canGrant;
            bool_t can_Grant_جواب;
            bool_t do_call;

            /* Get the head of the endpoint queue. */
            قطار = ep_ptr_get_قطار(epptr);
            sender = قطار.سر;

            /* Haskell error "Send endpoint queue must not be empty" */
            أكد(sender);

            /* Dequeue the first TCB */
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#وإلا
            قطار = tcbEPDequeue(sender, قطار);
            ep_ptr_سیٹ_قطار(epptr, قطار);

            إذا (!قطار.سر) {
                endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
            }
#جے_انت /* CONFIG_KERNEL_MCS */

            /* Get sender IPC details */
            badge = thread_state_ptr_get_blockingIPCBadge(&sender->tcbState);
            canGrant =
                thread_state_ptr_get_blockingIPCCanGrant(&sender->tcbState);
            can_Grant_جواب =
                thread_state_ptr_get_blocking_IPCCan_Grant_جواب(&sender->tcbState);

            /* Do the transfer */
            doIPCTransfer(sender, epptr, badge,
                          canGrant, thread);

            do_call = thread_state_ptr_get_blockingIPCIsCall(&sender->tcbState);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
            إذا (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                أكد(sender->tcbSchedContext != گرہ_STATE(ksCurSC));
                إذا (sender->tcbSchedContext != گرہ_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            إذا (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                إذا ((canGrant || can_Grant_جواب) && جواب_Ptr != عدم) {
                    bool_t canDonate = sender->tcbSchedContext != عدم
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    جواب_ڈالو(sender, thread, جواب_Ptr, canDonate);
                } وإلا {
                    سیٹ_Thread_State(sender, ThreadState_Inactive);
                }
            } وإلا {
                سیٹ_Thread_State(sender, ThreadState_Running);
                possible_Switch_منزل(sender);
                أكد(sender->tcbSchedContext == عدم || refill_sufficient(sender->tcbSchedContext, 0));
            }
#وإلا
            إذا (do_call) {
                إذا (canGrant || can_Grant_جواب) {
                    تیاری_Caller_Cap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } وإلا {
                    سیٹ_Thread_State(sender, ThreadState_Inactive);
                }
            } وإلا {
                سیٹ_Thread_State(sender, ThreadState_Running);
                possible_Switch_منزل(sender);
            }
#جے_انت
            اكسر;
        }
        }
    }
}

فراغ جواب_From_Kernel_error(tcb_t *thread)
{
    لفظ_t len;
    لفظ_t *ipc_بفر;

    ipc_بفر = lookupIPCBuffer(true, thread);
    سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
    len = سیٹ_MRs_syscall_error(thread, ipc_بفر);

#جے_تعریف_شدہ CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    حرف *debug_بفر = (حرف *)(ipc_بفر + DEBUG_سنیہا_شروع + 1);
    لفظ_t جوڑو = strlcpy(debug_بفر, (حرف *)current_debug_error.error_سنیہا,
                         DEBUG_سنیہا_MAXLEN * حجم(لفظ_t));

    len += (جوڑو / حجم(لفظ_t)) + 1;
#جے_انت

    سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                    se_L4_سنیہا_Info_new(current_syscall_error.type, 0, 0, len)));
}

فراغ جواب_From_Kernel_success_خالی_تھاواں(tcb_t *thread)
{
    سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
    سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                    se_L4_سنیہا_Info_new(0, 0, 0, 0)));
}

فراغ cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#جے_انت

    بدل (thread_state_ptr_get_tsType(state)) {
    حالة ThreadState_BlockedOnSend:
    حالة ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        أكد(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#وإلا
        tcb_قطار_t قطار;
        قطار = ep_ptr_get_قطار(epptr);
        قطار = tcbEPDequeue(tptr, قطار);
        ep_ptr_سیٹ_قطار(epptr, قطار);

        إذا (!قطار.سر) {
            endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
        }
#جے_انت /* CONFIG_KERNEL_MCS */

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        إذا (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            جواب_t *جواب = جواب_PTR(thread_state_ptr_get_جواب_Object(state));
            إذا (جواب != عدم) {
                جواب_unlink(جواب, tptr);
            }
        }
#جے_انت
        سیٹ_Thread_State(tptr, ThreadState_Inactive);
        اكسر;
    }

    حالة ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        اكسر;

    حالة Thread_State_Blocked_On_جواب: {
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        جواب_remove_tcb(tptr);
#وإلا
        cte_t *تھاں, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        تھاں = TCB_PTR_CTE_PTR(tptr, tcb_جواب);

        callerCap = CTE_PTR(mdb_گرہ_get_mdb_اگلا(تھاں->cteMDBNode));
        إذا (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#جے_انت

        اكسر;
    }
    }
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
ساكن مضمن فراغ restart_thread_if_no_fault(tcb_t *thread)
{
    إذا (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        سیٹ_Thread_State(thread, ThreadState_Restart);
        إذا (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(thread->tcbSchedContext != گرہ_STATE(ksCurSC));
            إذا (thread->tcbSchedContext != گرہ_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possible_Switch_منزل(thread);
    } وإلا {
        سیٹ_Thread_State(thread, ThreadState_Inactive);
    }
}

ساكن مضمن فراغ removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    إذا (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        جواب_t *جواب = جواب_PTR(thread_state_get_جواب_Object(thread->tcbState));
        إذا (جواب != عدم) {
            جواب_unlink(جواب, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#جے_انت

فراغ cancelAllIPC(endpoint_t *epptr)
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
        اكسر;

    افتراضي: {
        /* Clear the queue and set all blocked threads to restart */
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        tcb_قطار_t قطار;
        tcb_t *thread, *اگلا;

        قطار = ep_ptr_get_قطار(epptr);

        لكل (thread = قطار.سر; thread; thread = اگلا) {
            اگلا = thread->tcb_Sched_اگلا;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#وإلا
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_ep_قطار_سر(epptr));

        /* Make endpoint idle */
        endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
        endpoint_ptr_سیٹ_ep_قطار_سر(epptr, 0);
        endpoint_ptr_سیٹ_ep_قطار_پوچھ(epptr, 0);

        لكل (; thread; thread = thread->tcbEPNext) {
            سیٹ_Thread_State(thread, ThreadState_Restart);
            SCHED_قطار_وچ_پاؤ(thread);
        }
#جے_انت

        rescheduleRequired();
        اكسر;
    }
    }
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
ساكن مضمن فراغ removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, لفظ_t badge)
{
    لفظ_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    أكد(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    إذا (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#جے_انت

فراغ cancelBadgedSends(endpoint_t *epptr, لفظ_t badge)
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
    حالة EPState_Recv:
        اكسر;

    حالة EPState_Send: {
        tcb_t *thread, *اگلا;
        tcb_قطار_t قطار = ep_ptr_get_قطار(epptr);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        لكل (thread = قطار.سر; thread; thread = اگلا) {
            اگلا = thread->tcb_Sched_اگلا;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#وإلا
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
        endpoint_ptr_سیٹ_ep_قطار_سر(epptr, 0);
        endpoint_ptr_سیٹ_ep_قطار_پوچھ(epptr, 0);

        لكل (thread = قطار.سر; thread; thread = اگلا) {
            لفظ_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            اگلا = thread->tcbEPNext;

            إذا (b == badge) {
                سیٹ_Thread_State(thread, ThreadState_Restart);
                SCHED_قطار_وچ_پاؤ(thread);
                قطار = tcbEPDequeue(thread, قطار);
            }

        }
        ep_ptr_سیٹ_قطار(epptr, قطار);

        إذا (قطار.سر) {
            endpoint_ptr_سیٹ_state(epptr, EPState_Send);
        }
#جے_انت /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        اكسر;
    }

    افتراضي:
        fail("invalid EP state");
    }
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
فراغ tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
{
    tcb_قطار_t قطار;
    tcb_قطار_t new_قطار;

    قطار = ep_ptr_get_قطار(epptr);
    new_قطار = tcbAppend(thread, قطار);
    ep_ptr_سیٹ_قطار(epptr, new_قطار);

    /* Update the state of the endpoint with the state that was passed in. If the queue
     * was previously non-empty this must be the same state the endpoint is currently in. */
    endpoint_ptr_سیٹ_state(epptr, ep_state);
}

فراغ tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_قطار_t قطار;
    tcb_قطار_t new_قطار;

    قطار = ep_ptr_get_قطار(epptr);
    new_قطار = tcb_قطار_remove(قطار, thread);
    ep_ptr_سیٹ_قطار(epptr, new_قطار);

    إذا (tcb_قطار_خالی_تھاواں(new_قطار)) {
        endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
    }
}

فراغ reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_قطار_t قطار = ep_ptr_get_قطار(epptr);
    قطار = tcb_قطار_remove(قطار, thread);
    قطار = tcbAppend(thread, قطار);
    ep_ptr_سیٹ_قطار(epptr, قطار);
}
#جے_انت
