/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sel4/constants.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/notification.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <object/endpoint.h>
#ಅನ್ತರ್ಭಾವ <object/tcb.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ sendIPC(bool_t blocking, bool_t do_call, ಶಬ್ದ_t badge,
             bool_t canGrant, bool_t can_Grant_ಉತ್ತರ, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ sendIPC(bool_t blocking, bool_t do_call, ಶಬ್ದ_t badge,
             bool_t canGrant, bool_t can_Grant_ಉತ್ತರ, tcb_t *thread, endpoint_t *epptr)
#ಆದರೆ_ಅಂತ್ಯ
{
    ಆಯ್ಕೆ (endpoint_ptr_get_state(epptr)) {
    ಸ್ಥಿತಿ EPState_Idle:
    ಸ್ಥಿತಿ EPState_Send:
        ಆದರೆ (blocking) {

            /* Set thread state to BlockedOnSend */
            thread_state_ptr_ಗಣ_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnSend);
            thread_state_ptr_ಗಣ_blocking_Object(
                &thread->tcbState, EP_REF(epptr));
            thread_state_ptr_ಗಣ_blocking_IPCBadge(
                &thread->tcbState, badge);
            thread_state_ptr_ಗಣ_blocking_IPCCan_Grant(
                &thread->tcbState, canGrant);
            thread_state_ptr_ಗಣ_blocking_IPCCan_Grant_ಉತ್ತರ(
                &thread->tcbState, can_Grant_ಉತ್ತರ);
            thread_state_ptr_ಗಣ_blocking_IPCIs_Call(
                &thread->tcbState, do_call);

            ವೇಳಾಪಟ್ಟಿ_TCB(thread);

            /* Place calling thread in endpoint queue */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
            tcbEPAppend(thread, epptr, EPState_Send);
#ಇಲ್ಲದಿದ್ದರೆ
            tcb_ಸರತಿ_t ಸರತಿ;
            ಸರತಿ = ep_ptr_get_ಸರತಿ(epptr);
            ಸರತಿ = tcbEPAppend(thread, ಸರತಿ);
            endpoint_ptr_ಗಣ_state(epptr, EPState_Send);
            ep_ptr_ಗಣ_ಸರತಿ(epptr, ಸರತಿ);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */

        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ EPState_Recv: {
        tcb_ಸರತಿ_t ಸರತಿ;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        ಸರತಿ = ep_ptr_get_ಸರತಿ(epptr);
        dest = ಸರತಿ.ತಲೆ;

        /* Haskell error "Receive endpoint queue must not be empty" */
        ಪ್ರತಿಜ್ಞಾ(dest);

        /* Dequeue the first TCB */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#ಇಲ್ಲದಿದ್ದರೆ
        ಸರತಿ = tcbEPDequeue(dest, ಸರತಿ);
        ep_ptr_ಗಣ_ಸರತಿ(epptr, ಸರತಿ);

        ಆದರೆ (!ಸರತಿ.ತಲೆ) {
            endpoint_ptr_ಗಣ_state(epptr, EPState_Idle);
        }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಉತ್ತರ_t *ಉತ್ತರ = ಉತ್ತರ_PTR(thread_state_get_ಉತ್ತರ_Object(dest->tcbState));
        ಆದರೆ (ಉತ್ತರ) {
            ಉತ್ತರ_unlink(ಉತ್ತರ, dest);
        }

        ಆದರೆ (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            ಆದರೆ (ಉತ್ತರ != ಖಾಲಿ && (canGrant || can_Grant_ಉತ್ತರ)) {
                ಉತ್ತರ_ತಳ್ಳು(thread, dest, ಉತ್ತರ, canDonate);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಣ_Thread_State(thread, ThreadState_Inactive);
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (canDonate && dest->tcbSchedContext == ಖಾಲಿ) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        ಗಣ_Thread_State(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        ಪ್ರತಿಜ್ಞಾ(dest_sc == ಖಾಲಿ || refill_sufficient(dest_sc, 0));
        ಪ್ರತಿಜ್ಞಾ(dest_sc == ಖಾಲಿ || refill_ready(dest_sc));
        ಆದರೆ (sc_sporadic(dest_sc) && dest_sc != ಗಂಟು_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possible_Switch_ಗಮ್ಯ(dest);
#ಇಲ್ಲದಿದ್ದರೆ
        bool_t ಉತ್ತರ_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        ಗಣ_Thread_State(dest, ThreadState_Running);
        possible_Switch_ಗಮ್ಯ(dest);

        ಆದರೆ (do_call) {
            ಆದರೆ (canGrant || can_Grant_ಉತ್ತರ) {
                ಸಿದ್ಧತೆ_Caller_Cap(thread, dest, ಉತ್ತರ_Can_Grant);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಣ_Thread_State(thread, ThreadState_Inactive);
            }
        }
#ಆದರೆ_ಅಂತ್ಯ
        ನಿಲ್ಲಿಸು;
    }
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t ಉತ್ತರ_Cap)
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#ಆದರೆ_ಅಂತ್ಯ
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಉತ್ತರ_t *ಉತ್ತರ_Ptr = ಖಾಲಿ;
    ಆದರೆ (cap_get_capType(ಉತ್ತರ_Cap) == cap_ಉತ್ತರ_cap) {
        ಉತ್ತರ_Ptr = ಉತ್ತರ_PTR(cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Ptr(ಉತ್ತರ_Cap));
        tcb_t *ಉತ್ತರ_tcb = ಉತ್ತರ_Ptr->ಉತ್ತರ_TCB;
        ಆದರೆ (unlikely(ಉತ್ತರ_tcb != ಖಾಲಿ && ಉತ್ತರ_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(ಉತ್ತರ_tcb);
        }
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    ಆದರೆ (ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_ಸಕ್ರಿಯ) {
        completeSignal(ntfnPtr, thread);
    } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        ಆದರೆ (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಆಯ್ಕೆ (endpoint_ptr_get_state(epptr)) {
        ಸ್ಥಿತಿ EPState_Idle:
        ಸ್ಥಿತಿ EPState_Recv: {

            ಆದರೆ (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_ಗಣ_ts_Type(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_ಗಣ_blocking_Object(
                    &thread->tcbState, EP_REF(epptr));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
                thread_state_ptr_ಗಣ_ಉತ್ತರ_Object(&thread->tcbState, ಉತ್ತರ_REF(ಉತ್ತರ_Ptr));
#ಇಲ್ಲದಿದ್ದರೆ
                thread_state_ptr_ಗಣ_blocking_IPCCan_Grant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#ಆದರೆ_ಅಂತ್ಯ
                ವೇಳಾಪಟ್ಟಿ_TCB(thread);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
                ಆದರೆ (ಉತ್ತರ_Ptr) {
                    ಉತ್ತರ_Ptr->ಉತ್ತರ_TCB = thread;
                }
#ಆದರೆ_ಅಂತ್ಯ

                /* Place calling thread in endpoint queue */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#ಇಲ್ಲದಿದ್ದರೆ
                tcb_ಸರತಿ_t ಸರತಿ;
                ಸರತಿ = ep_ptr_get_ಸರತಿ(epptr);
                ಸರತಿ = tcbEPAppend(thread, ಸರತಿ);
                endpoint_ptr_ಗಣ_state(epptr, EPState_Recv);
                ep_ptr_ಗಣ_ಸರತಿ(epptr, ಸರತಿ);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */
            } ಇಲ್ಲದಿದ್ದರೆ {
                doNBRecvFailedTransfer(thread);
            }
            ನಿಲ್ಲಿಸು;
        }

        ಸ್ಥಿತಿ EPState_Send: {
            tcb_ಸರತಿ_t ಸರತಿ;
            tcb_t *sender;
            ಶಬ್ದ_t badge;
            bool_t canGrant;
            bool_t can_Grant_ಉತ್ತರ;
            bool_t do_call;

            /* Get the head of the endpoint queue. */
            ಸರತಿ = ep_ptr_get_ಸರತಿ(epptr);
            sender = ಸರತಿ.ತಲೆ;

            /* Haskell error "Send endpoint queue must not be empty" */
            ಪ್ರತಿಜ್ಞಾ(sender);

            /* Dequeue the first TCB */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#ಇಲ್ಲದಿದ್ದರೆ
            ಸರತಿ = tcbEPDequeue(sender, ಸರತಿ);
            ep_ptr_ಗಣ_ಸರತಿ(epptr, ಸರತಿ);

            ಆದರೆ (!ಸರತಿ.ತಲೆ) {
                endpoint_ptr_ಗಣ_state(epptr, EPState_Idle);
            }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */

            /* Get sender IPC details */
            badge = thread_state_ptr_get_blockingIPCBadge(&sender->tcbState);
            canGrant =
                thread_state_ptr_get_blockingIPCCanGrant(&sender->tcbState);
            can_Grant_ಉತ್ತರ =
                thread_state_ptr_get_blocking_IPCCan_Grant_ಉತ್ತರ(&sender->tcbState);

            /* Do the transfer */
            doIPCTransfer(sender, epptr, badge,
                          canGrant, thread);

            do_call = thread_state_ptr_get_blockingIPCIsCall(&sender->tcbState);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
            ಆದರೆ (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                ಪ್ರತಿಜ್ಞಾ(sender->tcbSchedContext != ಗಂಟು_STATE(ksCurSC));
                ಆದರೆ (sender->tcbSchedContext != ಗಂಟು_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            ಆದರೆ (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                ಆದರೆ ((canGrant || can_Grant_ಉತ್ತರ) && ಉತ್ತರ_Ptr != ಖಾಲಿ) {
                    bool_t canDonate = sender->tcbSchedContext != ಖಾಲಿ
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    ಉತ್ತರ_ತಳ್ಳು(sender, thread, ಉತ್ತರ_Ptr, canDonate);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಗಣ_Thread_State(sender, ThreadState_Inactive);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಣ_Thread_State(sender, ThreadState_Running);
                possible_Switch_ಗಮ್ಯ(sender);
                ಪ್ರತಿಜ್ಞಾ(sender->tcbSchedContext == ಖಾಲಿ || refill_sufficient(sender->tcbSchedContext, 0));
            }
#ಇಲ್ಲದಿದ್ದರೆ
            ಆದರೆ (do_call) {
                ಆದರೆ (canGrant || can_Grant_ಉತ್ತರ) {
                    ಸಿದ್ಧತೆ_Caller_Cap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಗಣ_Thread_State(sender, ThreadState_Inactive);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಣ_Thread_State(sender, ThreadState_Running);
                possible_Switch_ಗಮ್ಯ(sender);
            }
#ಆದರೆ_ಅಂತ್ಯ
            ನಿಲ್ಲಿಸು;
        }
        }
    }
}

ಶೂನ್ಯ ಉತ್ತರ_From_Kernel_error(tcb_t *thread)
{
    ಶಬ್ದ_t len;
    ಶಬ್ದ_t *ipc_ಬಫರ್;

    ipc_ಬಫರ್ = lookupIPCBuffer(true, thread);
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
    len = ಗಣ_MRs_syscall_error(thread, ipc_ಬಫರ್);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    ಅಕ್ಷರ *debug_ಬಫರ್ = (ಅಕ್ಷರ *)(ipc_ಬಫರ್ + DEBUG_ಸಂದೇಶ_ಆರಂಭ + 1);
    ಶಬ್ದ_t ಕೂಡಿಸು = strlcpy(debug_ಬಫರ್, (ಅಕ್ಷರ *)current_debug_error.error_ಸಂದೇಶ,
                         DEBUG_ಸಂದೇಶ_MAXLEN * ಗಾತ್ರ(ಶಬ್ದ_t));

    len += (ಕೂಡಿಸು / ಗಾತ್ರ(ಶಬ್ದ_t)) + 1;
#ಆದರೆ_ಅಂತ್ಯ

    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                    se_L4_ಸಂದೇಶ_Info_new(current_syscall_error.type, 0, 0, len)));
}

ಶೂನ್ಯ ಉತ್ತರ_From_Kernel_success_ಖಾಲಿಗಳು(tcb_t *thread)
{
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                    se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, 0)));
}

ಶೂನ್ಯ cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#ಆದರೆ_ಅಂತ್ಯ

    ಆಯ್ಕೆ (thread_state_ptr_get_tsType(state)) {
    ಸ್ಥಿತಿ ThreadState_BlockedOnSend:
    ಸ್ಥಿತಿ ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        ಪ್ರತಿಜ್ಞಾ(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#ಇಲ್ಲದಿದ್ದರೆ
        tcb_ಸರತಿ_t ಸರತಿ;
        ಸರತಿ = ep_ptr_get_ಸರತಿ(epptr);
        ಸರತಿ = tcbEPDequeue(tptr, ಸರತಿ);
        ep_ptr_ಗಣ_ಸರತಿ(epptr, ಸರತಿ);

        ಆದರೆ (!ಸರತಿ.ತಲೆ) {
            endpoint_ptr_ಗಣ_state(epptr, EPState_Idle);
        }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಆದರೆ (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            ಉತ್ತರ_t *ಉತ್ತರ = ಉತ್ತರ_PTR(thread_state_ptr_get_ಉತ್ತರ_Object(state));
            ಆದರೆ (ಉತ್ತರ != ಖಾಲಿ) {
                ಉತ್ತರ_unlink(ಉತ್ತರ, tptr);
            }
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಗಣ_Thread_State(tptr, ThreadState_Inactive);
        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ Thread_State_Blocked_On_ಉತ್ತರ: {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಉತ್ತರ_remove_tcb(tptr);
#ಇಲ್ಲದಿದ್ದರೆ
        cte_t *ಸ್ಥಾನ, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        ಸ್ಥಾನ = TCB_PTR_CTE_PTR(tptr, tcb_ಉತ್ತರ);

        callerCap = CTE_PTR(mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(ಸ್ಥಾನ->cteMDBNode));
        ಆದರೆ (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#ಆದರೆ_ಅಂತ್ಯ

        ನಿಲ್ಲಿಸು;
    }
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ restart_thread_if_no_fault(tcb_t *thread)
{
    ಆದರೆ (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        ಗಣ_Thread_State(thread, ThreadState_Restart);
        ಆದರೆ (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            ಪ್ರತಿಜ್ಞಾ(thread->tcbSchedContext != ಗಂಟು_STATE(ksCurSC));
            ಆದರೆ (thread->tcbSchedContext != ಗಂಟು_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possible_Switch_ಗಮ್ಯ(thread);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಣ_Thread_State(thread, ThreadState_Inactive);
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    ಆದರೆ (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        ಉತ್ತರ_t *ಉತ್ತರ = ಉತ್ತರ_PTR(thread_state_get_ಉತ್ತರ_Object(thread->tcbState));
        ಆದರೆ (ಉತ್ತರ != ಖಾಲಿ) {
            ಉತ್ತರ_unlink(ಉತ್ತರ, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ cancelAllIPC(endpoint_t *epptr)
{
    ಆಯ್ಕೆ (endpoint_ptr_get_state(epptr)) {
    ಸ್ಥಿತಿ EPState_Idle:
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ: {
        /* Clear the queue and set all blocked threads to restart */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        tcb_ಸರತಿ_t ಸರತಿ;
        tcb_t *thread, *ಮುಂದಿನ;

        ಸರತಿ = ep_ptr_get_ಸರತಿ(epptr);

        ಗಾಗಿ (thread = ಸರತಿ.ತಲೆ; thread; thread = ಮುಂದಿನ) {
            ಮುಂದಿನ = thread->tcb_Sched_ಮುಂದಿನ;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#ಇಲ್ಲದಿದ್ದರೆ
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_ep_ಸರತಿ_ತಲೆ(epptr));

        /* Make endpoint idle */
        endpoint_ptr_ಗಣ_state(epptr, EPState_Idle);
        endpoint_ptr_ಗಣ_ep_ಸರತಿ_ತಲೆ(epptr, 0);
        endpoint_ptr_ಗಣ_ep_ಸರತಿ_ಬಾಲ(epptr, 0);

        ಗಾಗಿ (; thread; thread = thread->tcbEPNext) {
            ಗಣ_Thread_State(thread, ThreadState_Restart);
            SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(thread);
        }
#ಆದರೆ_ಅಂತ್ಯ

        rescheduleRequired();
        ನಿಲ್ಲಿಸು;
    }
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, ಶಬ್ದ_t badge)
{
    ಶಬ್ದ_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    ಆದರೆ (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ cancelBadgedSends(endpoint_t *epptr, ಶಬ್ದ_t badge)
{
    ಆಯ್ಕೆ (endpoint_ptr_get_state(epptr)) {
    ಸ್ಥಿತಿ EPState_Idle:
    ಸ್ಥಿತಿ EPState_Recv:
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ EPState_Send: {
        tcb_t *thread, *ಮುಂದಿನ;
        tcb_ಸರತಿ_t ಸರತಿ = ep_ptr_get_ಸರತಿ(epptr);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಗಾಗಿ (thread = ಸರತಿ.ತಲೆ; thread; thread = ಮುಂದಿನ) {
            ಮುಂದಿನ = thread->tcb_Sched_ಮುಂದಿನ;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#ಇಲ್ಲದಿದ್ದರೆ
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_ಗಣ_state(epptr, EPState_Idle);
        endpoint_ptr_ಗಣ_ep_ಸರತಿ_ತಲೆ(epptr, 0);
        endpoint_ptr_ಗಣ_ep_ಸರತಿ_ಬಾಲ(epptr, 0);

        ಗಾಗಿ (thread = ಸರತಿ.ತಲೆ; thread; thread = ಮುಂದಿನ) {
            ಶಬ್ದ_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            ಮುಂದಿನ = thread->tcbEPNext;

            ಆದರೆ (b == badge) {
                ಗಣ_Thread_State(thread, ThreadState_Restart);
                SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(thread);
                ಸರತಿ = tcbEPDequeue(thread, ಸರತಿ);
            }

        }
        ep_ptr_ಗಣ_ಸರತಿ(epptr, ಸರತಿ);

        ಆದರೆ (ಸರತಿ.ತಲೆ) {
            endpoint_ptr_ಗಣ_state(epptr, EPState_Send);
        }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        ನಿಲ್ಲಿಸು;
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("invalid EP state");
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
{
    tcb_ಸರತಿ_t ಸರತಿ;
    tcb_ಸರತಿ_t new_ಸರತಿ;

    ಸರತಿ = ep_ptr_get_ಸರತಿ(epptr);
    new_ಸರತಿ = tcbAppend(thread, ಸರತಿ);
    ep_ptr_ಗಣ_ಸರತಿ(epptr, new_ಸರತಿ);

    /* Update the state of the endpoint with the state that was passed in. If the queue
     * was previously non-empty this must be the same state the endpoint is currently in. */
    endpoint_ptr_ಗಣ_state(epptr, ep_state);
}

ಶೂನ್ಯ tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_ಸರತಿ_t ಸರತಿ;
    tcb_ಸರತಿ_t new_ಸರತಿ;

    ಸರತಿ = ep_ptr_get_ಸರತಿ(epptr);
    new_ಸರತಿ = tcb_ಸರತಿ_remove(ಸರತಿ, thread);
    ep_ptr_ಗಣ_ಸರತಿ(epptr, new_ಸರತಿ);

    ಆದರೆ (tcb_ಸರತಿ_ಖಾಲಿಗಳು(new_ಸರತಿ)) {
        endpoint_ptr_ಗಣ_state(epptr, EPState_Idle);
    }
}

ಶೂನ್ಯ reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_ಸರತಿ_t ಸರತಿ = ep_ptr_get_ಸರತಿ(epptr);
    ಸರತಿ = tcb_ಸರತಿ_remove(ಸರತಿ, thread);
    ಸರತಿ = tcbAppend(thread, ಸರತಿ);
    ep_ptr_ಗಣ_ಸರತಿ(epptr, ಸರತಿ);
}
#ಆದರೆ_ಅಂತ್ಯ
