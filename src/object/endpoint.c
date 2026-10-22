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

#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ sendIPC(bool_t blocking, bool_t do_call, کلمه_t badge,
             bool_t canGrant, bool_t can_Grant_ځواب, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#وإلا
فراغ sendIPC(bool_t blocking, bool_t do_call, کلمه_t badge,
             bool_t canGrant, bool_t can_Grant_ځواب, tcb_t *thread, endpoint_t *epptr)
#د_که_پای
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
    حالة EPState_Send:
        إذا (blocking) {

            /* Set thread state to BlockedOnSend */
            thread_state_ptr_سېټ_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnSend);
            thread_state_ptr_سېټ_blocking_Object(
                &thread->tcbState, EP_REF(epptr));
            thread_state_ptr_سېټ_blocking_IPCBadge(
                &thread->tcbState, badge);
            thread_state_ptr_سېټ_blocking_IPCCan_Grant(
                &thread->tcbState, canGrant);
            thread_state_ptr_سېټ_blocking_IPCCan_Grant_ځواب(
                &thread->tcbState, can_Grant_ځواب);
            thread_state_ptr_سېټ_blocking_IPCIs_Call(
                &thread->tcbState, do_call);

            مهالوېش_TCB(thread);

            /* Place calling thread in endpoint queue */
#که_تعریف_شوی CONFIG_KERNEL_MCS
            tcbEPAppend(thread, epptr, EPState_Send);
#وإلا
            tcb_کتار_t کتار;
            کتار = ep_ptr_get_کتار(epptr);
            کتار = tcbEPAppend(thread, کتار);
            endpoint_ptr_سېټ_state(epptr, EPState_Send);
            ep_ptr_سېټ_کتار(epptr, کتار);
#د_که_پای /* CONFIG_KERNEL_MCS */

        }
        اكسر;

    حالة EPState_Recv: {
        tcb_کتار_t کتار;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        کتار = ep_ptr_get_کتار(epptr);
        dest = کتار.سر;

        /* Haskell error "Receive endpoint queue must not be empty" */
        أكد(dest);

        /* Dequeue the first TCB */
#که_تعریف_شوی CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#وإلا
        کتار = tcbEPDequeue(dest, کتار);
        ep_ptr_سېټ_کتار(epptr, کتار);

        إذا (!کتار.سر) {
            endpoint_ptr_سېټ_state(epptr, EPState_Idle);
        }
#د_که_پای /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#که_تعریف_شوی CONFIG_KERNEL_MCS
        ځواب_t *ځواب = ځواب_PTR(thread_state_get_ځواب_Object(dest->tcbState));
        إذا (ځواب) {
            ځواب_unlink(ځواب, dest);
        }

        إذا (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            إذا (ځواب != عدم && (canGrant || can_Grant_ځواب)) {
                ځواب_ټېل(thread, dest, ځواب, canDonate);
            } وإلا {
                سېټ_Thread_State(thread, ThreadState_Inactive);
            }
        } وإلا إذا (canDonate && dest->tcbSchedContext == عدم) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        سېټ_Thread_State(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        أكد(dest_sc == عدم || refill_sufficient(dest_sc, 0));
        أكد(dest_sc == عدم || refill_ready(dest_sc));
        إذا (sc_sporadic(dest_sc) && dest_sc != غوټه_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possible_Switch_منزل(dest);
#وإلا
        bool_t ځواب_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        سېټ_Thread_State(dest, ThreadState_Running);
        possible_Switch_منزل(dest);

        إذا (do_call) {
            إذا (canGrant || can_Grant_ځواب) {
                تیاری_Caller_Cap(thread, dest, ځواب_Can_Grant);
            } وإلا {
                سېټ_Thread_State(thread, ThreadState_Inactive);
            }
        }
#د_که_پای
        اكسر;
    }
    }
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t ځواب_Cap)
#وإلا
فراغ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#د_که_پای
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    أكد(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#که_تعریف_شوی CONFIG_KERNEL_MCS
    ځواب_t *ځواب_Ptr = عدم;
    إذا (cap_get_capType(ځواب_Cap) == cap_ځواب_cap) {
        ځواب_Ptr = ځواب_PTR(cap_ځواب_cap_get_cap_ځواب_Ptr(ځواب_Cap));
        tcb_t *ځواب_tcb = ځواب_Ptr->ځواب_TCB;
        إذا (unlikely(ځواب_tcb != عدم && ځواب_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(ځواب_tcb);
        }
    }
#د_که_پای

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    إذا (ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_فعال) {
        completeSignal(ntfnPtr, thread);
    } وإلا {
#که_تعریف_شوی CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        إذا (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#د_که_پای
        بدل (endpoint_ptr_get_state(epptr)) {
        حالة EPState_Idle:
        حالة EPState_Recv: {

            إذا (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_سېټ_ts_Type(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_سېټ_blocking_Object(
                    &thread->tcbState, EP_REF(epptr));
#که_تعریف_شوی CONFIG_KERNEL_MCS
                thread_state_ptr_سېټ_ځواب_Object(&thread->tcbState, ځواب_REF(ځواب_Ptr));
#وإلا
                thread_state_ptr_سېټ_blocking_IPCCan_Grant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#د_که_پای
                مهالوېش_TCB(thread);
#که_تعریف_شوی CONFIG_KERNEL_MCS
                إذا (ځواب_Ptr) {
                    ځواب_Ptr->ځواب_TCB = thread;
                }
#د_که_پای

                /* Place calling thread in endpoint queue */
#که_تعریف_شوی CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#وإلا
                tcb_کتار_t کتار;
                کتار = ep_ptr_get_کتار(epptr);
                کتار = tcbEPAppend(thread, کتار);
                endpoint_ptr_سېټ_state(epptr, EPState_Recv);
                ep_ptr_سېټ_کتار(epptr, کتار);
#د_که_پای /* CONFIG_KERNEL_MCS */
            } وإلا {
                doNBRecvFailedTransfer(thread);
            }
            اكسر;
        }

        حالة EPState_Send: {
            tcb_کتار_t کتار;
            tcb_t *sender;
            کلمه_t badge;
            bool_t canGrant;
            bool_t can_Grant_ځواب;
            bool_t do_call;

            /* Get the head of the endpoint queue. */
            کتار = ep_ptr_get_کتار(epptr);
            sender = کتار.سر;

            /* Haskell error "Send endpoint queue must not be empty" */
            أكد(sender);

            /* Dequeue the first TCB */
#که_تعریف_شوی CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#وإلا
            کتار = tcbEPDequeue(sender, کتار);
            ep_ptr_سېټ_کتار(epptr, کتار);

            إذا (!کتار.سر) {
                endpoint_ptr_سېټ_state(epptr, EPState_Idle);
            }
#د_که_پای /* CONFIG_KERNEL_MCS */

            /* Get sender IPC details */
            badge = thread_state_ptr_get_blockingIPCBadge(&sender->tcbState);
            canGrant =
                thread_state_ptr_get_blockingIPCCanGrant(&sender->tcbState);
            can_Grant_ځواب =
                thread_state_ptr_get_blocking_IPCCan_Grant_ځواب(&sender->tcbState);

            /* Do the transfer */
            doIPCTransfer(sender, epptr, badge,
                          canGrant, thread);

            do_call = thread_state_ptr_get_blockingIPCIsCall(&sender->tcbState);

#که_تعریف_شوی CONFIG_KERNEL_MCS
            إذا (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                أكد(sender->tcbSchedContext != غوټه_STATE(ksCurSC));
                إذا (sender->tcbSchedContext != غوټه_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            إذا (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                إذا ((canGrant || can_Grant_ځواب) && ځواب_Ptr != عدم) {
                    bool_t canDonate = sender->tcbSchedContext != عدم
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    ځواب_ټېل(sender, thread, ځواب_Ptr, canDonate);
                } وإلا {
                    سېټ_Thread_State(sender, ThreadState_Inactive);
                }
            } وإلا {
                سېټ_Thread_State(sender, ThreadState_Running);
                possible_Switch_منزل(sender);
                أكد(sender->tcbSchedContext == عدم || refill_sufficient(sender->tcbSchedContext, 0));
            }
#وإلا
            إذا (do_call) {
                إذا (canGrant || can_Grant_ځواب) {
                    تیاری_Caller_Cap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } وإلا {
                    سېټ_Thread_State(sender, ThreadState_Inactive);
                }
            } وإلا {
                سېټ_Thread_State(sender, ThreadState_Running);
                possible_Switch_منزل(sender);
            }
#د_که_پای
            اكسر;
        }
        }
    }
}

فراغ ځواب_From_Kernel_error(tcb_t *thread)
{
    کلمه_t len;
    کلمه_t *ipc_بفر;

    ipc_بفر = lookupIPCBuffer(true, thread);
    سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
    len = سېټ_MRs_syscall_error(thread, ipc_بفر);

#که_تعریف_شوی CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    حرف *debug_بفر = (حرف *)(ipc_بفر + DEBUG_پیغام_پیل + 1);
    کلمه_t جمع_کړه = strlcpy(debug_بفر, (حرف *)current_debug_error.error_پیغام,
                         DEBUG_پیغام_MAXLEN * حجم(کلمه_t));

    len += (جمع_کړه / حجم(کلمه_t)) + 1;
#د_که_پای

    سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                    se_L4_پیغام_Info_new(current_syscall_error.type, 0, 0, len)));
}

فراغ ځواب_From_Kernel_success_تشه(tcb_t *thread)
{
    سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
    سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                    se_L4_پیغام_Info_new(0, 0, 0, 0)));
}

فراغ cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#که_تعریف_شوی CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#د_که_پای

    بدل (thread_state_ptr_get_tsType(state)) {
    حالة ThreadState_BlockedOnSend:
    حالة ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        أكد(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#که_تعریف_شوی CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#وإلا
        tcb_کتار_t کتار;
        کتار = ep_ptr_get_کتار(epptr);
        کتار = tcbEPDequeue(tptr, کتار);
        ep_ptr_سېټ_کتار(epptr, کتار);

        إذا (!کتار.سر) {
            endpoint_ptr_سېټ_state(epptr, EPState_Idle);
        }
#د_که_پای /* CONFIG_KERNEL_MCS */

#که_تعریف_شوی CONFIG_KERNEL_MCS
        إذا (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            ځواب_t *ځواب = ځواب_PTR(thread_state_ptr_get_ځواب_Object(state));
            إذا (ځواب != عدم) {
                ځواب_unlink(ځواب, tptr);
            }
        }
#د_که_پای
        سېټ_Thread_State(tptr, ThreadState_Inactive);
        اكسر;
    }

    حالة ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        اكسر;

    حالة Thread_State_Blocked_On_ځواب: {
#که_تعریف_شوی CONFIG_KERNEL_MCS
        ځواب_remove_tcb(tptr);
#وإلا
        cte_t *خونه, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        خونه = TCB_PTR_CTE_PTR(tptr, tcb_ځواب);

        callerCap = CTE_PTR(mdb_غوټه_get_mdb_بل(خونه->cteMDBNode));
        إذا (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#د_که_پای

        اكسر;
    }
    }
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
ساكن مضمن فراغ restart_thread_if_no_fault(tcb_t *thread)
{
    إذا (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        سېټ_Thread_State(thread, ThreadState_Restart);
        إذا (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(thread->tcbSchedContext != غوټه_STATE(ksCurSC));
            إذا (thread->tcbSchedContext != غوټه_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possible_Switch_منزل(thread);
    } وإلا {
        سېټ_Thread_State(thread, ThreadState_Inactive);
    }
}

ساكن مضمن فراغ removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    إذا (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        ځواب_t *ځواب = ځواب_PTR(thread_state_get_ځواب_Object(thread->tcbState));
        إذا (ځواب != عدم) {
            ځواب_unlink(ځواب, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#د_که_پای

فراغ cancelAllIPC(endpoint_t *epptr)
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
        اكسر;

    افتراضي: {
        /* Clear the queue and set all blocked threads to restart */
#که_تعریف_شوی CONFIG_KERNEL_MCS
        tcb_کتار_t کتار;
        tcb_t *thread, *بل;

        کتار = ep_ptr_get_کتار(epptr);

        لكل (thread = کتار.سر; thread; thread = بل) {
            بل = thread->tcb_Sched_بل;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#وإلا
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_ep_کتار_سر(epptr));

        /* Make endpoint idle */
        endpoint_ptr_سېټ_state(epptr, EPState_Idle);
        endpoint_ptr_سېټ_ep_کتار_سر(epptr, 0);
        endpoint_ptr_سېټ_ep_کتار_لکۍ(epptr, 0);

        لكل (; thread; thread = thread->tcbEPNext) {
            سېټ_Thread_State(thread, ThreadState_Restart);
            SCHED_کتار_ته_ورزیاتول(thread);
        }
#د_که_پای

        rescheduleRequired();
        اكسر;
    }
    }
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
ساكن مضمن فراغ removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, کلمه_t badge)
{
    کلمه_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    أكد(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    إذا (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#د_که_پای

فراغ cancelBadgedSends(endpoint_t *epptr, کلمه_t badge)
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
    حالة EPState_Recv:
        اكسر;

    حالة EPState_Send: {
        tcb_t *thread, *بل;
        tcb_کتار_t کتار = ep_ptr_get_کتار(epptr);

#که_تعریف_شوی CONFIG_KERNEL_MCS
        لكل (thread = کتار.سر; thread; thread = بل) {
            بل = thread->tcb_Sched_بل;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#وإلا
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_سېټ_state(epptr, EPState_Idle);
        endpoint_ptr_سېټ_ep_کتار_سر(epptr, 0);
        endpoint_ptr_سېټ_ep_کتار_لکۍ(epptr, 0);

        لكل (thread = کتار.سر; thread; thread = بل) {
            کلمه_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            بل = thread->tcbEPNext;

            إذا (b == badge) {
                سېټ_Thread_State(thread, ThreadState_Restart);
                SCHED_کتار_ته_ورزیاتول(thread);
                کتار = tcbEPDequeue(thread, کتار);
            }

        }
        ep_ptr_سېټ_کتار(epptr, کتار);

        إذا (کتار.سر) {
            endpoint_ptr_سېټ_state(epptr, EPState_Send);
        }
#د_که_پای /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        اكسر;
    }

    افتراضي:
        fail("invalid EP state");
    }
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
{
    tcb_کتار_t کتار;
    tcb_کتار_t new_کتار;

    کتار = ep_ptr_get_کتار(epptr);
    new_کتار = tcbAppend(thread, کتار);
    ep_ptr_سېټ_کتار(epptr, new_کتار);

    /* Update the state of the endpoint with the state that was passed in. If the queue
     * was previously non-empty this must be the same state the endpoint is currently in. */
    endpoint_ptr_سېټ_state(epptr, ep_state);
}

فراغ tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_کتار_t کتار;
    tcb_کتار_t new_کتار;

    کتار = ep_ptr_get_کتار(epptr);
    new_کتار = tcb_کتار_remove(کتار, thread);
    ep_ptr_سېټ_کتار(epptr, new_کتار);

    إذا (tcb_کتار_تشه(new_کتار)) {
        endpoint_ptr_سېټ_state(epptr, EPState_Idle);
    }
}

فراغ reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_کتار_t کتار = ep_ptr_get_کتار(epptr);
    کتار = tcb_کتار_remove(کتار, thread);
    کتار = tcbAppend(thread, کتار);
    ep_ptr_سېټ_کتار(epptr, کتار);
}
#د_که_پای
