/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sel4/constants.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/notification.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/cnode.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/endpoint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/tcb.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ sendIPC(bool_t blocking, bool_t do_call, ਸ਼ਬਦ_t badge,
             bool_t canGrant, bool_t can_Grant_ਜਵਾਬ, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#ਨਹੀਂਤਾਂ
ਖਾਲੀ sendIPC(bool_t blocking, bool_t do_call, ਸ਼ਬਦ_t badge,
             bool_t canGrant, bool_t can_Grant_ਜਵਾਬ, tcb_t *thread, endpoint_t *epptr)
#ਜੇ_ਅੰਤ
{
    ਚੋਣ (endpoint_ptr_get_state(epptr)) {
    ਹਾਲਤ EPState_Idle:
    ਹਾਲਤ EPState_Send:
        ਜੇ (blocking) {

            /* Set thread state to BlockedOnSend */
            thread_state_ptr_ਸੈੱਟ_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnSend);
            thread_state_ptr_ਸੈੱਟ_blocking_Object(
                &thread->tcbState, EP_REF(epptr));
            thread_state_ptr_ਸੈੱਟ_blocking_IPCBadge(
                &thread->tcbState, badge);
            thread_state_ptr_ਸੈੱਟ_blocking_IPCCan_Grant(
                &thread->tcbState, canGrant);
            thread_state_ptr_ਸੈੱਟ_blocking_IPCCan_Grant_ਜਵਾਬ(
                &thread->tcbState, can_Grant_ਜਵਾਬ);
            thread_state_ptr_ਸੈੱਟ_blocking_IPCIs_Call(
                &thread->tcbState, do_call);

            ਸਮਾਂ_ਸਾਰਣੀ_TCB(thread);

            /* Place calling thread in endpoint queue */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
            tcbEPAppend(thread, epptr, EPState_Send);
#ਨਹੀਂਤਾਂ
            tcb_ਕਤਾਰ_t ਕਤਾਰ;
            ਕਤਾਰ = ep_ptr_get_ਕਤਾਰ(epptr);
            ਕਤਾਰ = tcbEPAppend(thread, ਕਤਾਰ);
            endpoint_ptr_ਸੈੱਟ_state(epptr, EPState_Send);
            ep_ptr_ਸੈੱਟ_ਕਤਾਰ(epptr, ਕਤਾਰ);
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */

        }
        ਰੋਕੋ;

    ਹਾਲਤ EPState_Recv: {
        tcb_ਕਤਾਰ_t ਕਤਾਰ;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        ਕਤਾਰ = ep_ptr_get_ਕਤਾਰ(epptr);
        dest = ਕਤਾਰ.ਸਿਰ;

        /* Haskell error "Receive endpoint queue must not be empty" */
        ਪ੍ਰਤਿਜ੍ਞਾ(dest);

        /* Dequeue the first TCB */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#ਨਹੀਂਤਾਂ
        ਕਤਾਰ = tcbEPDequeue(dest, ਕਤਾਰ);
        ep_ptr_ਸੈੱਟ_ਕਤਾਰ(epptr, ਕਤਾਰ);

        ਜੇ (!ਕਤਾਰ.ਸਿਰ) {
            endpoint_ptr_ਸੈੱਟ_state(epptr, EPState_Idle);
        }
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਜਵਾਬ_t *ਜਵਾਬ = ਜਵਾਬ_PTR(thread_state_get_ਜਵਾਬ_Object(dest->tcbState));
        ਜੇ (ਜਵਾਬ) {
            ਜਵਾਬ_unlink(ਜਵਾਬ, dest);
        }

        ਜੇ (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            ਜੇ (ਜਵਾਬ != ਨਿਰਾ && (canGrant || can_Grant_ਜਵਾਬ)) {
                ਜਵਾਬ_ਧੱਕੋ(thread, dest, ਜਵਾਬ, canDonate);
            } ਨਹੀਂਤਾਂ {
                ਸੈੱਟ_Thread_State(thread, ThreadState_Inactive);
            }
        } ਨਹੀਂਤਾਂ ਜੇ (canDonate && dest->tcbSchedContext == ਨਿਰਾ) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        ਸੈੱਟ_Thread_State(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        ਪ੍ਰਤਿਜ੍ਞਾ(dest_sc == ਨਿਰਾ || refill_sufficient(dest_sc, 0));
        ਪ੍ਰਤਿਜ੍ਞਾ(dest_sc == ਨਿਰਾ || refill_ready(dest_sc));
        ਜੇ (sc_sporadic(dest_sc) && dest_sc != ਗੰਢ_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possible_Switch_ਮੰਜ਼ਿਲ(dest);
#ਨਹੀਂਤਾਂ
        bool_t ਜਵਾਬ_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        ਸੈੱਟ_Thread_State(dest, ThreadState_Running);
        possible_Switch_ਮੰਜ਼ਿਲ(dest);

        ਜੇ (do_call) {
            ਜੇ (canGrant || can_Grant_ਜਵਾਬ) {
                ਤਿਆਰੀ_Caller_Cap(thread, dest, ਜਵਾਬ_Can_Grant);
            } ਨਹੀਂਤਾਂ {
                ਸੈੱਟ_Thread_State(thread, ThreadState_Inactive);
            }
        }
#ਜੇ_ਅੰਤ
        ਰੋਕੋ;
    }
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t ਜਵਾਬ_Cap)
#ਨਹੀਂਤਾਂ
ਖਾਲੀ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#ਜੇ_ਅੰਤ
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਜਵਾਬ_t *ਜਵਾਬ_Ptr = ਨਿਰਾ;
    ਜੇ (cap_get_capType(ਜਵਾਬ_Cap) == cap_ਜਵਾਬ_cap) {
        ਜਵਾਬ_Ptr = ਜਵਾਬ_PTR(cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Ptr(ਜਵਾਬ_Cap));
        tcb_t *ਜਵਾਬ_tcb = ਜਵਾਬ_Ptr->ਜਵਾਬ_TCB;
        ਜੇ (unlikely(ਜਵਾਬ_tcb != ਨਿਰਾ && ਜਵਾਬ_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(ਜਵਾਬ_tcb);
        }
    }
#ਜੇ_ਅੰਤ

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    ਜੇ (ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_ਸਰਗਰਮ) {
        completeSignal(ntfnPtr, thread);
    } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        ਜੇ (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#ਜੇ_ਅੰਤ
        ਚੋਣ (endpoint_ptr_get_state(epptr)) {
        ਹਾਲਤ EPState_Idle:
        ਹਾਲਤ EPState_Recv: {

            ਜੇ (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_ਸੈੱਟ_ts_Type(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_ਸੈੱਟ_blocking_Object(
                    &thread->tcbState, EP_REF(epptr));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
                thread_state_ptr_ਸੈੱਟ_ਜਵਾਬ_Object(&thread->tcbState, ਜਵਾਬ_REF(ਜਵਾਬ_Ptr));
#ਨਹੀਂਤਾਂ
                thread_state_ptr_ਸੈੱਟ_blocking_IPCCan_Grant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#ਜੇ_ਅੰਤ
                ਸਮਾਂ_ਸਾਰਣੀ_TCB(thread);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
                ਜੇ (ਜਵਾਬ_Ptr) {
                    ਜਵਾਬ_Ptr->ਜਵਾਬ_TCB = thread;
                }
#ਜੇ_ਅੰਤ

                /* Place calling thread in endpoint queue */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#ਨਹੀਂਤਾਂ
                tcb_ਕਤਾਰ_t ਕਤਾਰ;
                ਕਤਾਰ = ep_ptr_get_ਕਤਾਰ(epptr);
                ਕਤਾਰ = tcbEPAppend(thread, ਕਤਾਰ);
                endpoint_ptr_ਸੈੱਟ_state(epptr, EPState_Recv);
                ep_ptr_ਸੈੱਟ_ਕਤਾਰ(epptr, ਕਤਾਰ);
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */
            } ਨਹੀਂਤਾਂ {
                doNBRecvFailedTransfer(thread);
            }
            ਰੋਕੋ;
        }

        ਹਾਲਤ EPState_Send: {
            tcb_ਕਤਾਰ_t ਕਤਾਰ;
            tcb_t *sender;
            ਸ਼ਬਦ_t badge;
            bool_t canGrant;
            bool_t can_Grant_ਜਵਾਬ;
            bool_t do_call;

            /* Get the head of the endpoint queue. */
            ਕਤਾਰ = ep_ptr_get_ਕਤਾਰ(epptr);
            sender = ਕਤਾਰ.ਸਿਰ;

            /* Haskell error "Send endpoint queue must not be empty" */
            ਪ੍ਰਤਿਜ੍ਞਾ(sender);

            /* Dequeue the first TCB */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#ਨਹੀਂਤਾਂ
            ਕਤਾਰ = tcbEPDequeue(sender, ਕਤਾਰ);
            ep_ptr_ਸੈੱਟ_ਕਤਾਰ(epptr, ਕਤਾਰ);

            ਜੇ (!ਕਤਾਰ.ਸਿਰ) {
                endpoint_ptr_ਸੈੱਟ_state(epptr, EPState_Idle);
            }
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */

            /* Get sender IPC details */
            badge = thread_state_ptr_get_blockingIPCBadge(&sender->tcbState);
            canGrant =
                thread_state_ptr_get_blockingIPCCanGrant(&sender->tcbState);
            can_Grant_ਜਵਾਬ =
                thread_state_ptr_get_blocking_IPCCan_Grant_ਜਵਾਬ(&sender->tcbState);

            /* Do the transfer */
            doIPCTransfer(sender, epptr, badge,
                          canGrant, thread);

            do_call = thread_state_ptr_get_blockingIPCIsCall(&sender->tcbState);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
            ਜੇ (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                ਪ੍ਰਤਿਜ੍ਞਾ(sender->tcbSchedContext != ਗੰਢ_STATE(ksCurSC));
                ਜੇ (sender->tcbSchedContext != ਗੰਢ_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            ਜੇ (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                ਜੇ ((canGrant || can_Grant_ਜਵਾਬ) && ਜਵਾਬ_Ptr != ਨਿਰਾ) {
                    bool_t canDonate = sender->tcbSchedContext != ਨਿਰਾ
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    ਜਵਾਬ_ਧੱਕੋ(sender, thread, ਜਵਾਬ_Ptr, canDonate);
                } ਨਹੀਂਤਾਂ {
                    ਸੈੱਟ_Thread_State(sender, ThreadState_Inactive);
                }
            } ਨਹੀਂਤਾਂ {
                ਸੈੱਟ_Thread_State(sender, ThreadState_Running);
                possible_Switch_ਮੰਜ਼ਿਲ(sender);
                ਪ੍ਰਤਿਜ੍ਞਾ(sender->tcbSchedContext == ਨਿਰਾ || refill_sufficient(sender->tcbSchedContext, 0));
            }
#ਨਹੀਂਤਾਂ
            ਜੇ (do_call) {
                ਜੇ (canGrant || can_Grant_ਜਵਾਬ) {
                    ਤਿਆਰੀ_Caller_Cap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } ਨਹੀਂਤਾਂ {
                    ਸੈੱਟ_Thread_State(sender, ThreadState_Inactive);
                }
            } ਨਹੀਂਤਾਂ {
                ਸੈੱਟ_Thread_State(sender, ThreadState_Running);
                possible_Switch_ਮੰਜ਼ਿਲ(sender);
            }
#ਜੇ_ਅੰਤ
            ਰੋਕੋ;
        }
        }
    }
}

ਖਾਲੀ ਜਵਾਬ_From_Kernel_error(tcb_t *thread)
{
    ਸ਼ਬਦ_t len;
    ਸ਼ਬਦ_t *ipc_ਬਫਰ;

    ipc_ਬਫਰ = lookupIPCBuffer(true, thread);
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
    len = ਸੈੱਟ_MRs_syscall_error(thread, ipc_ਬਫਰ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    ਅੱਖਰ *debug_ਬਫਰ = (ਅੱਖਰ *)(ipc_ਬਫਰ + DEBUG_ਸੁਨੇਹਾ_ਸ਼ੁਰੂ + 1);
    ਸ਼ਬਦ_t ਜੋੜੋ = strlcpy(debug_ਬਫਰ, (ਅੱਖਰ *)current_debug_error.error_ਸੁਨੇਹਾ,
                         DEBUG_ਸੁਨੇਹਾ_MAXLEN * ਆਕਾਰ(ਸ਼ਬਦ_t));

    len += (ਜੋੜੋ / ਆਕਾਰ(ਸ਼ਬਦ_t)) + 1;
#ਜੇ_ਅੰਤ

    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                    se_L4_ਸੁਨੇਹਾ_Info_new(current_syscall_error.type, 0, 0, len)));
}

ਖਾਲੀ ਜਵਾਬ_From_Kernel_success_ਖਾਲੀ_ਥਾਵਾਂ(tcb_t *thread)
{
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                    se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, 0)));
}

ਖਾਲੀ cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#ਜੇ_ਅੰਤ

    ਚੋਣ (thread_state_ptr_get_tsType(state)) {
    ਹਾਲਤ ThreadState_BlockedOnSend:
    ਹਾਲਤ ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        ਪ੍ਰਤਿਜ੍ਞਾ(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#ਨਹੀਂਤਾਂ
        tcb_ਕਤਾਰ_t ਕਤਾਰ;
        ਕਤਾਰ = ep_ptr_get_ਕਤਾਰ(epptr);
        ਕਤਾਰ = tcbEPDequeue(tptr, ਕਤਾਰ);
        ep_ptr_ਸੈੱਟ_ਕਤਾਰ(epptr, ਕਤਾਰ);

        ਜੇ (!ਕਤਾਰ.ਸਿਰ) {
            endpoint_ptr_ਸੈੱਟ_state(epptr, EPState_Idle);
        }
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਜੇ (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            ਜਵਾਬ_t *ਜਵਾਬ = ਜਵਾਬ_PTR(thread_state_ptr_get_ਜਵਾਬ_Object(state));
            ਜੇ (ਜਵਾਬ != ਨਿਰਾ) {
                ਜਵਾਬ_unlink(ਜਵਾਬ, tptr);
            }
        }
#ਜੇ_ਅੰਤ
        ਸੈੱਟ_Thread_State(tptr, ThreadState_Inactive);
        ਰੋਕੋ;
    }

    ਹਾਲਤ ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        ਰੋਕੋ;

    ਹਾਲਤ Thread_State_Blocked_On_ਜਵਾਬ: {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਜਵਾਬ_remove_tcb(tptr);
#ਨਹੀਂਤਾਂ
        cte_t *ਥਾਂ, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        ਥਾਂ = TCB_PTR_CTE_PTR(tptr, tcb_ਜਵਾਬ);

        callerCap = CTE_PTR(mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(ਥਾਂ->cteMDBNode));
        ਜੇ (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#ਜੇ_ਅੰਤ

        ਰੋਕੋ;
    }
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ restart_thread_if_no_fault(tcb_t *thread)
{
    ਜੇ (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        ਸੈੱਟ_Thread_State(thread, ThreadState_Restart);
        ਜੇ (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            ਪ੍ਰਤਿਜ੍ਞਾ(thread->tcbSchedContext != ਗੰਢ_STATE(ksCurSC));
            ਜੇ (thread->tcbSchedContext != ਗੰਢ_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possible_Switch_ਮੰਜ਼ਿਲ(thread);
    } ਨਹੀਂਤਾਂ {
        ਸੈੱਟ_Thread_State(thread, ThreadState_Inactive);
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    ਜੇ (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        ਜਵਾਬ_t *ਜਵਾਬ = ਜਵਾਬ_PTR(thread_state_get_ਜਵਾਬ_Object(thread->tcbState));
        ਜੇ (ਜਵਾਬ != ਨਿਰਾ) {
            ਜਵਾਬ_unlink(ਜਵਾਬ, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ cancelAllIPC(endpoint_t *epptr)
{
    ਚੋਣ (endpoint_ptr_get_state(epptr)) {
    ਹਾਲਤ EPState_Idle:
        ਰੋਕੋ;

    ਮੂਲ: {
        /* Clear the queue and set all blocked threads to restart */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        tcb_ਕਤਾਰ_t ਕਤਾਰ;
        tcb_t *thread, *ਅਗਲਾ;

        ਕਤਾਰ = ep_ptr_get_ਕਤਾਰ(epptr);

        ਲਈ (thread = ਕਤਾਰ.ਸਿਰ; thread; thread = ਅਗਲਾ) {
            ਅਗਲਾ = thread->tcb_Sched_ਅਗਲਾ;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#ਨਹੀਂਤਾਂ
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_ep_ਕਤਾਰ_ਸਿਰ(epptr));

        /* Make endpoint idle */
        endpoint_ptr_ਸੈੱਟ_state(epptr, EPState_Idle);
        endpoint_ptr_ਸੈੱਟ_ep_ਕਤਾਰ_ਸਿਰ(epptr, 0);
        endpoint_ptr_ਸੈੱਟ_ep_ਕਤਾਰ_ਪੂਛ(epptr, 0);

        ਲਈ (; thread; thread = thread->tcbEPNext) {
            ਸੈੱਟ_Thread_State(thread, ThreadState_Restart);
            SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(thread);
        }
#ਜੇ_ਅੰਤ

        rescheduleRequired();
        ਰੋਕੋ;
    }
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, ਸ਼ਬਦ_t badge)
{
    ਸ਼ਬਦ_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    ਜੇ (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ cancelBadgedSends(endpoint_t *epptr, ਸ਼ਬਦ_t badge)
{
    ਚੋਣ (endpoint_ptr_get_state(epptr)) {
    ਹਾਲਤ EPState_Idle:
    ਹਾਲਤ EPState_Recv:
        ਰੋਕੋ;

    ਹਾਲਤ EPState_Send: {
        tcb_t *thread, *ਅਗਲਾ;
        tcb_ਕਤਾਰ_t ਕਤਾਰ = ep_ptr_get_ਕਤਾਰ(epptr);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਲਈ (thread = ਕਤਾਰ.ਸਿਰ; thread; thread = ਅਗਲਾ) {
            ਅਗਲਾ = thread->tcb_Sched_ਅਗਲਾ;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#ਨਹੀਂਤਾਂ
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_ਸੈੱਟ_state(epptr, EPState_Idle);
        endpoint_ptr_ਸੈੱਟ_ep_ਕਤਾਰ_ਸਿਰ(epptr, 0);
        endpoint_ptr_ਸੈੱਟ_ep_ਕਤਾਰ_ਪੂਛ(epptr, 0);

        ਲਈ (thread = ਕਤਾਰ.ਸਿਰ; thread; thread = ਅਗਲਾ) {
            ਸ਼ਬਦ_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            ਅਗਲਾ = thread->tcbEPNext;

            ਜੇ (b == badge) {
                ਸੈੱਟ_Thread_State(thread, ThreadState_Restart);
                SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(thread);
                ਕਤਾਰ = tcbEPDequeue(thread, ਕਤਾਰ);
            }

        }
        ep_ptr_ਸੈੱਟ_ਕਤਾਰ(epptr, ਕਤਾਰ);

        ਜੇ (ਕਤਾਰ.ਸਿਰ) {
            endpoint_ptr_ਸੈੱਟ_state(epptr, EPState_Send);
        }
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        ਰੋਕੋ;
    }

    ਮੂਲ:
        fail("invalid EP state");
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
{
    tcb_ਕਤਾਰ_t ਕਤਾਰ;
    tcb_ਕਤਾਰ_t new_ਕਤਾਰ;

    ਕਤਾਰ = ep_ptr_get_ਕਤਾਰ(epptr);
    new_ਕਤਾਰ = tcbAppend(thread, ਕਤਾਰ);
    ep_ptr_ਸੈੱਟ_ਕਤਾਰ(epptr, new_ਕਤਾਰ);

    /* Update the state of the endpoint with the state that was passed in. If the queue
     * was previously non-empty this must be the same state the endpoint is currently in. */
    endpoint_ptr_ਸੈੱਟ_state(epptr, ep_state);
}

ਖਾਲੀ tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_ਕਤਾਰ_t ਕਤਾਰ;
    tcb_ਕਤਾਰ_t new_ਕਤਾਰ;

    ਕਤਾਰ = ep_ptr_get_ਕਤਾਰ(epptr);
    new_ਕਤਾਰ = tcb_ਕਤਾਰ_remove(ਕਤਾਰ, thread);
    ep_ptr_ਸੈੱਟ_ਕਤਾਰ(epptr, new_ਕਤਾਰ);

    ਜੇ (tcb_ਕਤਾਰ_ਖਾਲੀ_ਥਾਵਾਂ(new_ਕਤਾਰ)) {
        endpoint_ptr_ਸੈੱਟ_state(epptr, EPState_Idle);
    }
}

ਖਾਲੀ reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_ਕਤਾਰ_t ਕਤਾਰ = ep_ptr_get_ਕਤਾਰ(epptr);
    ਕਤਾਰ = tcb_ਕਤਾਰ_remove(ਕਤਾਰ, thread);
    ਕਤਾਰ = tcbAppend(thread, ਕਤਾਰ);
    ep_ptr_ਸੈੱਟ_ਕਤਾਰ(epptr, ਕਤਾਰ);
}
#ਜੇ_ਅੰਤ
