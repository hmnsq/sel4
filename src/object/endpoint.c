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

#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ sendIPC(bool_t blocking, bool_t do_call, کلمه_t badge,
             bool_t canGrant, bool_t can_Grant_پاسخ, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#وإلا
فراغ sendIPC(bool_t blocking, bool_t do_call, کلمه_t badge,
             bool_t canGrant, bool_t can_Grant_پاسخ, tcb_t *thread, endpoint_t *epptr)
#پایان_اگر
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
    حالة EPState_Send:
        إذا (blocking) {

            /* Set thread state to BlockedOnSend */
            thread_state_ptr_مجموعه_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnSend);
            thread_state_ptr_مجموعه_blocking_Object(
                &thread->tcbState, EP_REF(epptr));
            thread_state_ptr_مجموعه_blocking_IPCBadge(
                &thread->tcbState, badge);
            thread_state_ptr_مجموعه_blocking_IPCCan_Grant(
                &thread->tcbState, canGrant);
            thread_state_ptr_مجموعه_blocking_IPCCan_Grant_پاسخ(
                &thread->tcbState, can_Grant_پاسخ);
            thread_state_ptr_مجموعه_blocking_IPCIs_Call(
                &thread->tcbState, do_call);

            زمانبندی_TCB(thread);

            /* Place calling thread in endpoint queue */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
            tcbEPAppend(thread, epptr, EPState_Send);
#وإلا
            tcb_صف_t صف;
            صف = ep_ptr_get_صف(epptr);
            صف = tcbEPAppend(thread, صف);
            endpoint_ptr_مجموعه_state(epptr, EPState_Send);
            ep_ptr_مجموعه_صف(epptr, صف);
#پایان_اگر /* CONFIG_KERNEL_MCS */

        }
        اكسر;

    حالة EPState_Recv: {
        tcb_صف_t صف;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        صف = ep_ptr_get_صف(epptr);
        dest = صف.سر;

        /* Haskell error "Receive endpoint queue must not be empty" */
        أكد(dest);

        /* Dequeue the first TCB */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#وإلا
        صف = tcbEPDequeue(dest, صف);
        ep_ptr_مجموعه_صف(epptr, صف);

        إذا (!صف.سر) {
            endpoint_ptr_مجموعه_state(epptr, EPState_Idle);
        }
#پایان_اگر /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
        پاسخ_t *پاسخ = پاسخ_PTR(thread_state_get_پاسخ_Object(dest->tcbState));
        إذا (پاسخ) {
            پاسخ_unlink(پاسخ, dest);
        }

        إذا (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            إذا (پاسخ != عدم && (canGrant || can_Grant_پاسخ)) {
                پاسخ_افزودن(thread, dest, پاسخ, canDonate);
            } وإلا {
                مجموعه_Thread_State(thread, ThreadState_Inactive);
            }
        } وإلا إذا (canDonate && dest->tcbSchedContext == عدم) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        مجموعه_Thread_State(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        أكد(dest_sc == عدم || refill_sufficient(dest_sc, 0));
        أكد(dest_sc == عدم || refill_ready(dest_sc));
        إذا (sc_sporadic(dest_sc) && dest_sc != گره_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possible_Switch_مقصد(dest);
#وإلا
        bool_t پاسخ_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        مجموعه_Thread_State(dest, ThreadState_Running);
        possible_Switch_مقصد(dest);

        إذا (do_call) {
            إذا (canGrant || can_Grant_پاسخ) {
                راهاندازی_Caller_Cap(thread, dest, پاسخ_Can_Grant);
            } وإلا {
                مجموعه_Thread_State(thread, ThreadState_Inactive);
            }
        }
#پایان_اگر
        اكسر;
    }
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t پاسخ_Cap)
#وإلا
فراغ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#پایان_اگر
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    أكد(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    پاسخ_t *پاسخ_Ptr = عدم;
    إذا (cap_get_capType(پاسخ_Cap) == cap_پاسخ_cap) {
        پاسخ_Ptr = پاسخ_PTR(cap_پاسخ_cap_get_cap_پاسخ_Ptr(پاسخ_Cap));
        tcb_t *پاسخ_tcb = پاسخ_Ptr->پاسخ_TCB;
        إذا (unlikely(پاسخ_tcb != عدم && پاسخ_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(پاسخ_tcb);
        }
    }
#پایان_اگر

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    إذا (ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_فعال) {
        completeSignal(ntfnPtr, thread);
    } وإلا {
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        إذا (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#پایان_اگر
        بدل (endpoint_ptr_get_state(epptr)) {
        حالة EPState_Idle:
        حالة EPState_Recv: {

            إذا (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_مجموعه_ts_Type(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_مجموعه_blocking_Object(
                    &thread->tcbState, EP_REF(epptr));
#اگر_تعریف_شده CONFIG_KERNEL_MCS
                thread_state_ptr_مجموعه_پاسخ_Object(&thread->tcbState, پاسخ_REF(پاسخ_Ptr));
#وإلا
                thread_state_ptr_مجموعه_blocking_IPCCan_Grant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#پایان_اگر
                زمانبندی_TCB(thread);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
                إذا (پاسخ_Ptr) {
                    پاسخ_Ptr->پاسخ_TCB = thread;
                }
#پایان_اگر

                /* Place calling thread in endpoint queue */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#وإلا
                tcb_صف_t صف;
                صف = ep_ptr_get_صف(epptr);
                صف = tcbEPAppend(thread, صف);
                endpoint_ptr_مجموعه_state(epptr, EPState_Recv);
                ep_ptr_مجموعه_صف(epptr, صف);
#پایان_اگر /* CONFIG_KERNEL_MCS */
            } وإلا {
                doNBRecvFailedTransfer(thread);
            }
            اكسر;
        }

        حالة EPState_Send: {
            tcb_صف_t صف;
            tcb_t *sender;
            کلمه_t badge;
            bool_t canGrant;
            bool_t can_Grant_پاسخ;
            bool_t do_call;

            /* Get the head of the endpoint queue. */
            صف = ep_ptr_get_صف(epptr);
            sender = صف.سر;

            /* Haskell error "Send endpoint queue must not be empty" */
            أكد(sender);

            /* Dequeue the first TCB */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#وإلا
            صف = tcbEPDequeue(sender, صف);
            ep_ptr_مجموعه_صف(epptr, صف);

            إذا (!صف.سر) {
                endpoint_ptr_مجموعه_state(epptr, EPState_Idle);
            }
#پایان_اگر /* CONFIG_KERNEL_MCS */

            /* Get sender IPC details */
            badge = thread_state_ptr_get_blockingIPCBadge(&sender->tcbState);
            canGrant =
                thread_state_ptr_get_blockingIPCCanGrant(&sender->tcbState);
            can_Grant_پاسخ =
                thread_state_ptr_get_blocking_IPCCan_Grant_پاسخ(&sender->tcbState);

            /* Do the transfer */
            doIPCTransfer(sender, epptr, badge,
                          canGrant, thread);

            do_call = thread_state_ptr_get_blockingIPCIsCall(&sender->tcbState);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
            إذا (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                أكد(sender->tcbSchedContext != گره_STATE(ksCurSC));
                إذا (sender->tcbSchedContext != گره_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            إذا (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                إذا ((canGrant || can_Grant_پاسخ) && پاسخ_Ptr != عدم) {
                    bool_t canDonate = sender->tcbSchedContext != عدم
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    پاسخ_افزودن(sender, thread, پاسخ_Ptr, canDonate);
                } وإلا {
                    مجموعه_Thread_State(sender, ThreadState_Inactive);
                }
            } وإلا {
                مجموعه_Thread_State(sender, ThreadState_Running);
                possible_Switch_مقصد(sender);
                أكد(sender->tcbSchedContext == عدم || refill_sufficient(sender->tcbSchedContext, 0));
            }
#وإلا
            إذا (do_call) {
                إذا (canGrant || can_Grant_پاسخ) {
                    راهاندازی_Caller_Cap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } وإلا {
                    مجموعه_Thread_State(sender, ThreadState_Inactive);
                }
            } وإلا {
                مجموعه_Thread_State(sender, ThreadState_Running);
                possible_Switch_مقصد(sender);
            }
#پایان_اگر
            اكسر;
        }
        }
    }
}

فراغ پاسخ_From_Kernel_error(tcb_t *thread)
{
    کلمه_t len;
    کلمه_t *ipc_میانگیر;

    ipc_میانگیر = lookupIPCBuffer(true, thread);
    مجموعه_ثبات(thread, badge_ثبات, 0);
    len = مجموعه_MRs_syscall_error(thread, ipc_میانگیر);

#اگر_تعریف_شده CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    حرف *debug_میانگیر = (حرف *)(ipc_میانگیر + DEBUG_پیام_شروع + 1);
    کلمه_t جمع_کن = strlcpy(debug_میانگیر, (حرف *)current_debug_error.error_پیام,
                         DEBUG_پیام_MAXLEN * حجم(کلمه_t));

    len += (جمع_کن / حجم(کلمه_t)) + 1;
#پایان_اگر

    مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                    se_L4_پیام_Info_new(current_syscall_error.type, 0, 0, len)));
}

فراغ پاسخ_From_Kernel_success_خالی(tcb_t *thread)
{
    مجموعه_ثبات(thread, badge_ثبات, 0);
    مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                    se_L4_پیام_Info_new(0, 0, 0, 0)));
}

فراغ cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#پایان_اگر

    بدل (thread_state_ptr_get_tsType(state)) {
    حالة ThreadState_BlockedOnSend:
    حالة ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        أكد(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#وإلا
        tcb_صف_t صف;
        صف = ep_ptr_get_صف(epptr);
        صف = tcbEPDequeue(tptr, صف);
        ep_ptr_مجموعه_صف(epptr, صف);

        إذا (!صف.سر) {
            endpoint_ptr_مجموعه_state(epptr, EPState_Idle);
        }
#پایان_اگر /* CONFIG_KERNEL_MCS */

#اگر_تعریف_شده CONFIG_KERNEL_MCS
        إذا (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            پاسخ_t *پاسخ = پاسخ_PTR(thread_state_ptr_get_پاسخ_Object(state));
            إذا (پاسخ != عدم) {
                پاسخ_unlink(پاسخ, tptr);
            }
        }
#پایان_اگر
        مجموعه_Thread_State(tptr, ThreadState_Inactive);
        اكسر;
    }

    حالة ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        اكسر;

    حالة Thread_State_Blocked_On_پاسخ: {
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        پاسخ_remove_tcb(tptr);
#وإلا
        cte_t *جایگاه, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        جایگاه = TCB_PTR_CTE_PTR(tptr, tcb_پاسخ);

        callerCap = CTE_PTR(mdb_گره_get_mdb_بعدی(جایگاه->cteMDBNode));
        إذا (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#پایان_اگر

        اكسر;
    }
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
ساكن مضمن فراغ restart_thread_if_no_fault(tcb_t *thread)
{
    إذا (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        مجموعه_Thread_State(thread, ThreadState_Restart);
        إذا (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(thread->tcbSchedContext != گره_STATE(ksCurSC));
            إذا (thread->tcbSchedContext != گره_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possible_Switch_مقصد(thread);
    } وإلا {
        مجموعه_Thread_State(thread, ThreadState_Inactive);
    }
}

ساكن مضمن فراغ removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    إذا (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        پاسخ_t *پاسخ = پاسخ_PTR(thread_state_get_پاسخ_Object(thread->tcbState));
        إذا (پاسخ != عدم) {
            پاسخ_unlink(پاسخ, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#پایان_اگر

فراغ cancelAllIPC(endpoint_t *epptr)
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
        اكسر;

    افتراضي: {
        /* Clear the queue and set all blocked threads to restart */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        tcb_صف_t صف;
        tcb_t *thread, *بعدی;

        صف = ep_ptr_get_صف(epptr);

        لكل (thread = صف.سر; thread; thread = بعدی) {
            بعدی = thread->tcb_Sched_بعدی;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#وإلا
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_ep_صف_سر(epptr));

        /* Make endpoint idle */
        endpoint_ptr_مجموعه_state(epptr, EPState_Idle);
        endpoint_ptr_مجموعه_ep_صف_سر(epptr, 0);
        endpoint_ptr_مجموعه_ep_صف_دم(epptr, 0);

        لكل (; thread; thread = thread->tcbEPNext) {
            مجموعه_Thread_State(thread, ThreadState_Restart);
            SCHED_افزودن_به_صف(thread);
        }
#پایان_اگر

        rescheduleRequired();
        اكسر;
    }
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
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
#پایان_اگر

فراغ cancelBadgedSends(endpoint_t *epptr, کلمه_t badge)
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
    حالة EPState_Recv:
        اكسر;

    حالة EPState_Send: {
        tcb_t *thread, *بعدی;
        tcb_صف_t صف = ep_ptr_get_صف(epptr);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
        لكل (thread = صف.سر; thread; thread = بعدی) {
            بعدی = thread->tcb_Sched_بعدی;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#وإلا
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_مجموعه_state(epptr, EPState_Idle);
        endpoint_ptr_مجموعه_ep_صف_سر(epptr, 0);
        endpoint_ptr_مجموعه_ep_صف_دم(epptr, 0);

        لكل (thread = صف.سر; thread; thread = بعدی) {
            کلمه_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            بعدی = thread->tcbEPNext;

            إذا (b == badge) {
                مجموعه_Thread_State(thread, ThreadState_Restart);
                SCHED_افزودن_به_صف(thread);
                صف = tcbEPDequeue(thread, صف);
            }

        }
        ep_ptr_مجموعه_صف(epptr, صف);

        إذا (صف.سر) {
            endpoint_ptr_مجموعه_state(epptr, EPState_Send);
        }
#پایان_اگر /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        اكسر;
    }

    افتراضي:
        fail("invalid EP state");
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
{
    tcb_صف_t صف;
    tcb_صف_t new_صف;

    صف = ep_ptr_get_صف(epptr);
    new_صف = tcbAppend(thread, صف);
    ep_ptr_مجموعه_صف(epptr, new_صف);

    /* Update the state of the endpoint with the state that was passed in. If the queue
     * was previously non-empty this must be the same state the endpoint is currently in. */
    endpoint_ptr_مجموعه_state(epptr, ep_state);
}

فراغ tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_صف_t صف;
    tcb_صف_t new_صف;

    صف = ep_ptr_get_صف(epptr);
    new_صف = tcb_صف_remove(صف, thread);
    ep_ptr_مجموعه_صف(epptr, new_صف);

    إذا (tcb_صف_خالی(new_صف)) {
        endpoint_ptr_مجموعه_state(epptr, EPState_Idle);
    }
}

فراغ reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_صف_t صف = ep_ptr_get_صف(epptr);
    صف = tcb_صف_remove(صف, thread);
    صف = tcbAppend(thread, صف);
    ep_ptr_مجموعه_صف(epptr, صف);
}
#پایان_اگر
