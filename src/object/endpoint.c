/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <string.h>
#شامل <sel4/constants.h>
#شامل <kernel/thread.h>
#شامل <kernel/vspace.h>
#شامل <machine/registerset.h>
#شامل <model/statedata.h>
#شامل <object/notification.h>
#شامل <object/cnode.h>
#شامل <object/endpoint.h>
#شامل <object/tcb.h>

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی sendIPC(bool_t blocking, bool_t do_call, لفظ_t badge,
             bool_t canGrant, bool_t can_Grant_جواب, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#ورنہ
خالی sendIPC(bool_t blocking, bool_t do_call, لفظ_t badge,
             bool_t canGrant, bool_t can_Grant_جواب, tcb_t *thread, endpoint_t *epptr)
#اگر_ختم
{
    انتخاب (endpoint_ptr_get_state(epptr)) {
    حالت EPState_Idle:
    حالت EPState_Send:
        اگر (blocking) {

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

            جدول_وقت_TCB(thread);

            /* Place calling thread in endpoint queue */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
            tcbEPAppend(thread, epptr, EPState_Send);
#ورنہ
            tcb_قطار_t قطار;
            قطار = ep_ptr_get_قطار(epptr);
            قطار = tcbEPAppend(thread, قطار);
            endpoint_ptr_سیٹ_state(epptr, EPState_Send);
            ep_ptr_سیٹ_قطار(epptr, قطار);
#اگر_ختم /* CONFIG_KERNEL_MCS */

        }
        توڑو;

    حالت EPState_Recv: {
        tcb_قطار_t قطار;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        قطار = ep_ptr_get_قطار(epptr);
        dest = قطار.سر;

        /* Haskell error "Receive endpoint queue must not be empty" */
        أكد(dest);

        /* Dequeue the first TCB */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#ورنہ
        قطار = tcbEPDequeue(dest, قطار);
        ep_ptr_سیٹ_قطار(epptr, قطار);

        اگر (!قطار.سر) {
            endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
        }
#اگر_ختم /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        جواب_t *جواب = جواب_PTR(thread_state_get_جواب_Object(dest->tcbState));
        اگر (جواب) {
            جواب_unlink(جواب, dest);
        }

        اگر (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            اگر (جواب != تہی && (canGrant || can_Grant_جواب)) {
                جواب_ڈالو(thread, dest, جواب, canDonate);
            } ورنہ {
                سیٹ_Thread_State(thread, ThreadState_Inactive);
            }
        } ورنہ اگر (canDonate && dest->tcbSchedContext == تہی) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        سیٹ_Thread_State(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        أكد(dest_sc == تہی || refill_sufficient(dest_sc, 0));
        أكد(dest_sc == تہی || refill_ready(dest_sc));
        اگر (sc_sporadic(dest_sc) && dest_sc != گرہ_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possible_Switch_مقصد(dest);
#ورنہ
        bool_t جواب_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        سیٹ_Thread_State(dest, ThreadState_Running);
        possible_Switch_مقصد(dest);

        اگر (do_call) {
            اگر (canGrant || can_Grant_جواب) {
                تیاری_Caller_Cap(thread, dest, جواب_Can_Grant);
            } ورنہ {
                سیٹ_Thread_State(thread, ThreadState_Inactive);
            }
        }
#اگر_ختم
        توڑو;
    }
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t جواب_Cap)
#ورنہ
خالی receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#اگر_ختم
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    أكد(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    جواب_t *جواب_Ptr = تہی;
    اگر (cap_get_capType(جواب_Cap) == cap_جواب_cap) {
        جواب_Ptr = جواب_PTR(cap_جواب_cap_get_cap_جواب_Ptr(جواب_Cap));
        tcb_t *جواب_tcb = جواب_Ptr->جواب_TCB;
        اگر (unlikely(جواب_tcb != تہی && جواب_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(جواب_tcb);
        }
    }
#اگر_ختم

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    اگر (ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_فعال) {
        completeSignal(ntfnPtr, thread);
    } ورنہ {
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        اگر (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#اگر_ختم
        انتخاب (endpoint_ptr_get_state(epptr)) {
        حالت EPState_Idle:
        حالت EPState_Recv: {

            اگر (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_سیٹ_ts_Type(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_سیٹ_blocking_Object(
                    &thread->tcbState, EP_REF(epptr));
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
                thread_state_ptr_سیٹ_جواب_Object(&thread->tcbState, جواب_REF(جواب_Ptr));
#ورنہ
                thread_state_ptr_سیٹ_blocking_IPCCan_Grant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#اگر_ختم
                جدول_وقت_TCB(thread);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
                اگر (جواب_Ptr) {
                    جواب_Ptr->جواب_TCB = thread;
                }
#اگر_ختم

                /* Place calling thread in endpoint queue */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#ورنہ
                tcb_قطار_t قطار;
                قطار = ep_ptr_get_قطار(epptr);
                قطار = tcbEPAppend(thread, قطار);
                endpoint_ptr_سیٹ_state(epptr, EPState_Recv);
                ep_ptr_سیٹ_قطار(epptr, قطار);
#اگر_ختم /* CONFIG_KERNEL_MCS */
            } ورنہ {
                doNBRecvFailedTransfer(thread);
            }
            توڑو;
        }

        حالت EPState_Send: {
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
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#ورنہ
            قطار = tcbEPDequeue(sender, قطار);
            ep_ptr_سیٹ_قطار(epptr, قطار);

            اگر (!قطار.سر) {
                endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
            }
#اگر_ختم /* CONFIG_KERNEL_MCS */

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

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
            اگر (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                أكد(sender->tcbSchedContext != گرہ_STATE(ksCurSC));
                اگر (sender->tcbSchedContext != گرہ_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            اگر (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                اگر ((canGrant || can_Grant_جواب) && جواب_Ptr != تہی) {
                    bool_t canDonate = sender->tcbSchedContext != تہی
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    جواب_ڈالو(sender, thread, جواب_Ptr, canDonate);
                } ورنہ {
                    سیٹ_Thread_State(sender, ThreadState_Inactive);
                }
            } ورنہ {
                سیٹ_Thread_State(sender, ThreadState_Running);
                possible_Switch_مقصد(sender);
                أكد(sender->tcbSchedContext == تہی || refill_sufficient(sender->tcbSchedContext, 0));
            }
#ورنہ
            اگر (do_call) {
                اگر (canGrant || can_Grant_جواب) {
                    تیاری_Caller_Cap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } ورنہ {
                    سیٹ_Thread_State(sender, ThreadState_Inactive);
                }
            } ورنہ {
                سیٹ_Thread_State(sender, ThreadState_Running);
                possible_Switch_مقصد(sender);
            }
#اگر_ختم
            توڑو;
        }
        }
    }
}

خالی جواب_From_Kernel_error(tcb_t *thread)
{
    لفظ_t len;
    لفظ_t *ipc_بفر;

    ipc_بفر = lookupIPCBuffer(true, thread);
    سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
    len = سیٹ_MRs_syscall_error(thread, ipc_بفر);

#اگر_تعریف_شدہ CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    حرف *debug_بفر = (حرف *)(ipc_بفر + DEBUG_پیغام_آغاز + 1);
    لفظ_t جوڑو = strlcpy(debug_بفر, (حرف *)current_debug_error.error_پیغام,
                         DEBUG_پیغام_MAXLEN * حجم(لفظ_t));

    len += (جوڑو / حجم(لفظ_t)) + 1;
#اگر_ختم

    سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                    se_L4_پیغام_Info_new(current_syscall_error.type, 0, 0, len)));
}

خالی جواب_From_Kernel_success_خالی_خانے(tcb_t *thread)
{
    سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
    سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                    se_L4_پیغام_Info_new(0, 0, 0, 0)));
}

خالی cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#اگر_ختم

    انتخاب (thread_state_ptr_get_tsType(state)) {
    حالت ThreadState_BlockedOnSend:
    حالت ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        أكد(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#ورنہ
        tcb_قطار_t قطار;
        قطار = ep_ptr_get_قطار(epptr);
        قطار = tcbEPDequeue(tptr, قطار);
        ep_ptr_سیٹ_قطار(epptr, قطار);

        اگر (!قطار.سر) {
            endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
        }
#اگر_ختم /* CONFIG_KERNEL_MCS */

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        اگر (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            جواب_t *جواب = جواب_PTR(thread_state_ptr_get_جواب_Object(state));
            اگر (جواب != تہی) {
                جواب_unlink(جواب, tptr);
            }
        }
#اگر_ختم
        سیٹ_Thread_State(tptr, ThreadState_Inactive);
        توڑو;
    }

    حالت ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        توڑو;

    حالت Thread_State_Blocked_On_جواب: {
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        جواب_remove_tcb(tptr);
#ورنہ
        cte_t *خانہ, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        خانہ = TCB_PTR_CTE_PTR(tptr, tcb_جواب);

        callerCap = CTE_PTR(mdb_گرہ_get_mdb_اگلا(خانہ->cteMDBNode));
        اگر (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#اگر_ختم

        توڑو;
    }
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن اندرونی خالی restart_thread_if_no_fault(tcb_t *thread)
{
    اگر (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        سیٹ_Thread_State(thread, ThreadState_Restart);
        اگر (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(thread->tcbSchedContext != گرہ_STATE(ksCurSC));
            اگر (thread->tcbSchedContext != گرہ_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possible_Switch_مقصد(thread);
    } ورنہ {
        سیٹ_Thread_State(thread, ThreadState_Inactive);
    }
}

ساکن اندرونی خالی removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    اگر (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        جواب_t *جواب = جواب_PTR(thread_state_get_جواب_Object(thread->tcbState));
        اگر (جواب != تہی) {
            جواب_unlink(جواب, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#اگر_ختم

خالی cancelAllIPC(endpoint_t *epptr)
{
    انتخاب (endpoint_ptr_get_state(epptr)) {
    حالت EPState_Idle:
        توڑو;

    طےشدہ: {
        /* Clear the queue and set all blocked threads to restart */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        tcb_قطار_t قطار;
        tcb_t *thread, *اگلا;

        قطار = ep_ptr_get_قطار(epptr);

        برائے (thread = قطار.سر; thread; thread = اگلا) {
            اگلا = thread->tcb_Sched_اگلا;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#ورنہ
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_ep_قطار_سر(epptr));

        /* Make endpoint idle */
        endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
        endpoint_ptr_سیٹ_ep_قطار_سر(epptr, 0);
        endpoint_ptr_سیٹ_ep_قطار_دم(epptr, 0);

        برائے (; thread; thread = thread->tcbEPNext) {
            سیٹ_Thread_State(thread, ThreadState_Restart);
            SCHED_قطار_میں_ڈالو(thread);
        }
#اگر_ختم

        rescheduleRequired();
        توڑو;
    }
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
ساکن اندرونی خالی removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, لفظ_t badge)
{
    لفظ_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    أكد(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    اگر (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#اگر_ختم

خالی cancelBadgedSends(endpoint_t *epptr, لفظ_t badge)
{
    انتخاب (endpoint_ptr_get_state(epptr)) {
    حالت EPState_Idle:
    حالت EPState_Recv:
        توڑو;

    حالت EPState_Send: {
        tcb_t *thread, *اگلا;
        tcb_قطار_t قطار = ep_ptr_get_قطار(epptr);

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        برائے (thread = قطار.سر; thread; thread = اگلا) {
            اگلا = thread->tcb_Sched_اگلا;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#ورنہ
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
        endpoint_ptr_سیٹ_ep_قطار_سر(epptr, 0);
        endpoint_ptr_سیٹ_ep_قطار_دم(epptr, 0);

        برائے (thread = قطار.سر; thread; thread = اگلا) {
            لفظ_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            اگلا = thread->tcbEPNext;

            اگر (b == badge) {
                سیٹ_Thread_State(thread, ThreadState_Restart);
                SCHED_قطار_میں_ڈالو(thread);
                قطار = tcbEPDequeue(thread, قطار);
            }

        }
        ep_ptr_سیٹ_قطار(epptr, قطار);

        اگر (قطار.سر) {
            endpoint_ptr_سیٹ_state(epptr, EPState_Send);
        }
#اگر_ختم /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        توڑو;
    }

    طےشدہ:
        fail("invalid EP state");
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
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

خالی tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_قطار_t قطار;
    tcb_قطار_t new_قطار;

    قطار = ep_ptr_get_قطار(epptr);
    new_قطار = tcb_قطار_remove(قطار, thread);
    ep_ptr_سیٹ_قطار(epptr, new_قطار);

    اگر (tcb_قطار_خالی_خانے(new_قطار)) {
        endpoint_ptr_سیٹ_state(epptr, EPState_Idle);
    }
}

خالی reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_قطار_t قطار = ep_ptr_get_قطار(epptr);
    قطار = tcb_قطار_remove(قطار, thread);
    قطار = tcbAppend(thread, قطار);
    ep_ptr_سیٹ_قطار(epptr, قطار);
}
#اگر_ختم
