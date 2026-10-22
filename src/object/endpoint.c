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

#ifdef CONFIG_KERNEL_MCS
ಶೂನ್ಯ sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#ಅನ್ಯಥಾ
ಶೂನ್ಯ sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, tcb_t *thread, endpoint_t *epptr)
#endif
{
    ವಿಕಲ್ಪ (endpoint_ptr_get_state(epptr)) {
    ಪಕ್ಷ EPState_Idle:
    ಪಕ್ಷ EPState_Send:
        ಯದಿ (blocking) {

            /* Set thread state to BlockedOnSend */
            thread_state_ptr_set_tsType(&thread->tcbState,
                                        ThreadState_BlockedOnSend);
            thread_state_ptr_set_blockingObject(
                &thread->tcbState, EP_REF(epptr));
            thread_state_ptr_set_blockingIPCBadge(
                &thread->tcbState, badge);
            thread_state_ptr_set_blockingIPCCanGrant(
                &thread->tcbState, canGrant);
            thread_state_ptr_set_blockingIPCCanGrantReply(
                &thread->tcbState, canGrantReply);
            thread_state_ptr_set_blockingIPCIsCall(
                &thread->tcbState, do_call);

            scheduleTCB(thread);

            /* Place calling thread in endpoint queue */
#ifdef CONFIG_KERNEL_MCS
            tcbEPAppend(thread, epptr, EPState_Send);
#ಅನ್ಯಥಾ
            tcb_queue_t queue;
            queue = ep_ptr_get_queue(epptr);
            queue = tcbEPAppend(thread, queue);
            endpoint_ptr_set_state(epptr, EPState_Send);
            ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */

        }
        ಭಙ್ಗ;

    ಪಕ್ಷ EPState_Recv: {
        tcb_queue_t queue;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        queue = ep_ptr_get_queue(epptr);
        dest = queue.head;

        /* Haskell error "Receive endpoint queue must not be empty" */
        ಪ್ರತಿಜ್ಞಾ(dest);

        /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#ಅನ್ಯಥಾ
        queue = tcbEPDequeue(dest, queue);
        ep_ptr_set_queue(epptr, queue);

        ಯದಿ (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#ifdef CONFIG_KERNEL_MCS
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(dest->tcbState));
        ಯದಿ (reply) {
            reply_unlink(reply, dest);
        }

        ಯದಿ (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            ಯದಿ (reply != ರಿಕ್ತ && (canGrant || canGrantReply)) {
                reply_ತಳ್ಳು(thread, dest, reply, canDonate);
            } ಅನ್ಯಥಾ {
                setThreadState(thread, ThreadState_Inactive);
            }
        } ಅನ್ಯಥಾ ಯದಿ (canDonate && dest->tcbSchedContext == ರಿಕ್ತ) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        setThreadState(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        ಪ್ರತಿಜ್ಞಾ(dest_sc == ರಿಕ್ತ || refill_sufficient(dest_sc, 0));
        ಪ್ರತಿಜ್ಞಾ(dest_sc == ರಿಕ್ತ || refill_ready(dest_sc));
        ಯದಿ (sc_sporadic(dest_sc) && dest_sc != ಗಂಟು_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possibleSwitchTo(dest);
#ಅನ್ಯಥಾ
        bool_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        setThreadState(dest, ThreadState_Running);
        possibleSwitchTo(dest);

        ಯದಿ (do_call) {
            ಯದಿ (canGrant || canGrantReply) {
                setupCallerCap(thread, dest, replyCanGrant);
            } ಅನ್ಯಥಾ {
                setThreadState(thread, ThreadState_Inactive);
            }
        }
#endif
        ಭಙ್ಗ;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ಶೂನ್ಯ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t replyCap)
#ಅನ್ಯಥಾ
ಶೂನ್ಯ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#endif
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#ifdef CONFIG_KERNEL_MCS
    reply_t *replyPtr = ರಿಕ್ತ;
    ಯದಿ (cap_get_capType(replyCap) == cap_reply_cap) {
        replyPtr = REPLY_PTR(cap_reply_cap_get_capReplyPtr(replyCap));
        tcb_t *reply_tcb = replyPtr->replyTCB;
        ಯದಿ (unlikely(reply_tcb != ರಿಕ್ತ && reply_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(reply_tcb);
        }
    }
#endif

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    ಯದಿ (ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {
        completeSignal(ntfnPtr, thread);
    } ಅನ್ಯಥಾ {
#ifdef CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        ಯದಿ (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#endif
        ವಿಕಲ್ಪ (endpoint_ptr_get_state(epptr)) {
        ಪಕ್ಷ EPState_Idle:
        ಪಕ್ಷ EPState_Recv: {

            ಯದಿ (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_set_tsType(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_set_blockingObject(
                    &thread->tcbState, EP_REF(epptr));
#ifdef CONFIG_KERNEL_MCS
                thread_state_ptr_set_replyObject(&thread->tcbState, REPLY_REF(replyPtr));
#ಅನ್ಯಥಾ
                thread_state_ptr_set_blockingIPCCanGrant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#endif
                scheduleTCB(thread);
#ifdef CONFIG_KERNEL_MCS
                ಯದಿ (replyPtr) {
                    replyPtr->replyTCB = thread;
                }
#endif

                /* Place calling thread in endpoint queue */
#ifdef CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#ಅನ್ಯಥಾ
                tcb_queue_t queue;
                queue = ep_ptr_get_queue(epptr);
                queue = tcbEPAppend(thread, queue);
                endpoint_ptr_set_state(epptr, EPState_Recv);
                ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */
            } ಅನ್ಯಥಾ {
                doNBRecvFailedTransfer(thread);
            }
            ಭಙ್ಗ;
        }

        ಪಕ್ಷ EPState_Send: {
            tcb_queue_t queue;
            tcb_t *sender;
            word_t badge;
            bool_t canGrant;
            bool_t canGrantReply;
            bool_t do_call;

            /* Get the head of the endpoint queue. */
            queue = ep_ptr_get_queue(epptr);
            sender = queue.head;

            /* Haskell error "Send endpoint queue must not be empty" */
            ಪ್ರತಿಜ್ಞಾ(sender);

            /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#ಅನ್ಯಥಾ
            queue = tcbEPDequeue(sender, queue);
            ep_ptr_set_queue(epptr, queue);

            ಯದಿ (!queue.head) {
                endpoint_ptr_set_state(epptr, EPState_Idle);
            }
#endif /* CONFIG_KERNEL_MCS */

            /* Get sender IPC details */
            badge = thread_state_ptr_get_blockingIPCBadge(&sender->tcbState);
            canGrant =
                thread_state_ptr_get_blockingIPCCanGrant(&sender->tcbState);
            canGrantReply =
                thread_state_ptr_get_blockingIPCCanGrantReply(&sender->tcbState);

            /* Do the transfer */
            doIPCTransfer(sender, epptr, badge,
                          canGrant, thread);

            do_call = thread_state_ptr_get_blockingIPCIsCall(&sender->tcbState);

#ifdef CONFIG_KERNEL_MCS
            ಯದಿ (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                ಪ್ರತಿಜ್ಞಾ(sender->tcbSchedContext != ಗಂಟು_STATE(ksCurSC));
                ಯದಿ (sender->tcbSchedContext != ಗಂಟು_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            ಯದಿ (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                ಯದಿ ((canGrant || canGrantReply) && replyPtr != ರಿಕ್ತ) {
                    bool_t canDonate = sender->tcbSchedContext != ರಿಕ್ತ
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    reply_ತಳ್ಳು(sender, thread, replyPtr, canDonate);
                } ಅನ್ಯಥಾ {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } ಅನ್ಯಥಾ {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
                ಪ್ರತಿಜ್ಞಾ(sender->tcbSchedContext == ರಿಕ್ತ || refill_sufficient(sender->tcbSchedContext, 0));
            }
#ಅನ್ಯಥಾ
            ಯದಿ (do_call) {
                ಯದಿ (canGrant || canGrantReply) {
                    setupCallerCap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } ಅನ್ಯಥಾ {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } ಅನ್ಯಥಾ {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
            }
#endif
            ಭಙ್ಗ;
        }
        }
    }
}

ಶೂನ್ಯ replyFromKernel_error(tcb_t *thread)
{
    word_t len;
    word_t *ipcBuffer;

    ipcBuffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    len = setMRs_syscall_error(thread, ipcBuffer);

#ifdef CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    ವರ್ಣ *debugBuffer = (ವರ್ಣ *)(ipcBuffer + DEBUG_MESSAGE_START + 1);
    word_t add = strlcpy(debugBuffer, (ವರ್ಣ *)current_debug_error.errorMessage,
                         DEBUG_MESSAGE_MAXLEN * ಪರಿಮಾಣ(word_t));

    len += (add / ಪರಿಮಾಣ(word_t)) + 1;
#endif

    setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(current_syscall_error.type, 0, 0, len)));
}

ಶೂನ್ಯ replyFromKernel_success_empty(tcb_t *thread)
{
    setRegister(thread, badgeRegister, 0);
    setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 0)));
}

ಶೂನ್ಯ cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#ifdef CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#endif

    ವಿಕಲ್ಪ (thread_state_ptr_get_tsType(state)) {
    ಪಕ್ಷ ThreadState_BlockedOnSend:
    ಪಕ್ಷ ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        ಪ್ರತಿಜ್ಞಾ(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#ಅನ್ಯಥಾ
        tcb_queue_t queue;
        queue = ep_ptr_get_queue(epptr);
        queue = tcbEPDequeue(tptr, queue);
        ep_ptr_set_queue(epptr, queue);

        ಯದಿ (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
        ಯದಿ (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            reply_t *reply = REPLY_PTR(thread_state_ptr_get_replyObject(state));
            ಯದಿ (reply != ರಿಕ್ತ) {
                reply_unlink(reply, tptr);
            }
        }
#endif
        setThreadState(tptr, ThreadState_Inactive);
        ಭಙ್ಗ;
    }

    ಪಕ್ಷ ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        ಭಙ್ಗ;

    ಪಕ್ಷ ThreadState_BlockedOnReply: {
#ifdef CONFIG_KERNEL_MCS
        reply_remove_tcb(tptr);
#ಅನ್ಯಥಾ
        cte_t *slot, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        slot = TCB_PTR_CTE_PTR(tptr, tcbReply);

        callerCap = CTE_PTR(mdb_ಗಂಟು_get_mdb_Next(slot->cteMDBNode));
        ಯದಿ (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#endif

        ಭಙ್ಗ;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ restart_thread_if_no_fault(tcb_t *thread)
{
    ಯದಿ (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        setThreadState(thread, ThreadState_Restart);
        ಯದಿ (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            ಪ್ರತಿಜ್ಞಾ(thread->tcbSchedContext != ಗಂಟು_STATE(ksCurSC));
            ಯದಿ (thread->tcbSchedContext != ಗಂಟು_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possibleSwitchTo(thread);
    } ಅನ್ಯಥಾ {
        setThreadState(thread, ThreadState_Inactive);
    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    ಯದಿ (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(thread->tcbState));
        ಯದಿ (reply != ರಿಕ್ತ) {
            reply_unlink(reply, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#endif

ಶೂನ್ಯ cancelAllIPC(endpoint_t *epptr)
{
    ವಿಕಲ್ಪ (endpoint_ptr_get_state(epptr)) {
    ಪಕ್ಷ EPState_Idle:
        ಭಙ್ಗ;

    ಸ್ವಭಾವ: {
        /* Clear the queue and set all blocked threads to restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *next;

        queue = ep_ptr_get_queue(epptr);

        ಪ್ರತಿ (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#ಅನ್ಯಥಾ
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_epQueue_head(epptr));

        /* Make endpoint idle */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        ಪ್ರತಿ (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif

        rescheduleRequired();
        ಭಙ್ಗ;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, word_t badge)
{
    word_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    ಯದಿ (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#endif

ಶೂನ್ಯ cancelBadgedSends(endpoint_t *epptr, word_t badge)
{
    ವಿಕಲ್ಪ (endpoint_ptr_get_state(epptr)) {
    ಪಕ್ಷ EPState_Idle:
    ಪಕ್ಷ EPState_Recv:
        ಭಙ್ಗ;

    ಪಕ್ಷ EPState_Send: {
        tcb_t *thread, *next;
        tcb_queue_t queue = ep_ptr_get_queue(epptr);

#ifdef CONFIG_KERNEL_MCS
        ಪ್ರತಿ (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#ಅನ್ಯಥಾ
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        ಪ್ರತಿ (thread = queue.head; thread; thread = next) {
            word_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            next = thread->tcbEPNext;

            ಯದಿ (b == badge) {
                setThreadState(thread, ThreadState_Restart);
                SCHED_ENQUEUE(thread);
                queue = tcbEPDequeue(thread, queue);
            }

        }
        ep_ptr_set_queue(epptr, queue);

        ಯದಿ (queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Send);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        ಭಙ್ಗ;
    }

    ಸ್ವಭಾವ:
        fail("invalid EP state");
    }
}

#ifdef CONFIG_KERNEL_MCS
ಶೂನ್ಯ tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcbAppend(thread, queue);
    ep_ptr_set_queue(epptr, new_queue);

    /* Update the state of the endpoint with the state that was passed in. If the queue
     * was previously non-empty this must be the same state the endpoint is currently in. */
    endpoint_ptr_set_state(epptr, ep_state);
}

ಶೂನ್ಯ tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcb_queue_remove(queue, thread);
    ep_ptr_set_queue(epptr, new_queue);

    ಯದಿ (tcb_queue_empty(new_queue)) {
        endpoint_ptr_set_state(epptr, EPState_Idle);
    }
}

ಶೂನ್ಯ reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_queue_t queue = ep_ptr_get_queue(epptr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ep_ptr_set_queue(epptr, queue);
}
#endif
