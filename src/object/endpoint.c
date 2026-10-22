/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sel4/constants.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <object/notification.h>
#ଅନ୍ତର୍ଭାଵ <object/cnode.h>
#ଅନ୍ତର୍ଭାଵ <object/endpoint.h>
#ଅନ୍ତର୍ଭାଵ <object/tcb.h>

#ifdef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#ଅନ୍ୟଥା
ଶୂନ୍ୟ sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, tcb_t *thread, endpoint_t *epptr)
#endif
{
    ବାଛ (endpoint_ptr_get_state(epptr)) {
    ସ୍ଥିତି EPState_Idle:
    ସ୍ଥିତି EPState_Send:
        ଯଦି (blocking) {

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
#ଅନ୍ୟଥା
            tcb_queue_t queue;
            queue = ep_ptr_get_queue(epptr);
            queue = tcbEPAppend(thread, queue);
            endpoint_ptr_set_state(epptr, EPState_Send);
            ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */

        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି EPState_Recv: {
        tcb_queue_t queue;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        queue = ep_ptr_get_queue(epptr);
        dest = queue.head;

        /* Haskell error "Receive endpoint queue must not be empty" */
        ପ୍ରତିଜ୍ଞା(dest);

        /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#ଅନ୍ୟଥା
        queue = tcbEPDequeue(dest, queue);
        ep_ptr_set_queue(epptr, queue);

        ଯଦି (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#ifdef CONFIG_KERNEL_MCS
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(dest->tcbState));
        ଯଦି (reply) {
            reply_unlink(reply, dest);
        }

        ଯଦି (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            ଯଦି (reply != ଖାଲି && (canGrant || canGrantReply)) {
                reply_ଠେଲ(thread, dest, reply, canDonate);
            } ଅନ୍ୟଥା {
                setThreadState(thread, ThreadState_Inactive);
            }
        } ଅନ୍ୟଥା ଯଦି (canDonate && dest->tcbSchedContext == ଖାଲି) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        setThreadState(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        ପ୍ରତିଜ୍ଞା(dest_sc == ଖାଲି || refill_sufficient(dest_sc, 0));
        ପ୍ରତିଜ୍ଞା(dest_sc == ଖାଲି || refill_ready(dest_sc));
        ଯଦି (sc_sporadic(dest_sc) && dest_sc != ଗଣ୍ଠି_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possibleSwitchTo(dest);
#ଅନ୍ୟଥା
        bool_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        setThreadState(dest, ThreadState_Running);
        possibleSwitchTo(dest);

        ଯଦି (do_call) {
            ଯଦି (canGrant || canGrantReply) {
                setupCallerCap(thread, dest, replyCanGrant);
            } ଅନ୍ୟଥା {
                setThreadState(thread, ThreadState_Inactive);
            }
        }
#endif
        ଭାଙ୍ଗ;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t replyCap)
#ଅନ୍ୟଥା
ଶୂନ୍ୟ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#endif
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    ପ୍ରତିଜ୍ଞା(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#ifdef CONFIG_KERNEL_MCS
    reply_t *replyPtr = ଖାଲି;
    ଯଦି (cap_get_capType(replyCap) == cap_reply_cap) {
        replyPtr = REPLY_PTR(cap_reply_cap_get_capReplyPtr(replyCap));
        tcb_t *reply_tcb = replyPtr->replyTCB;
        ଯଦି (unlikely(reply_tcb != ଖାଲି && reply_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(reply_tcb);
        }
    }
#endif

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    ଯଦି (ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {
        completeSignal(ntfnPtr, thread);
    } ଅନ୍ୟଥା {
#ifdef CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        ଯଦି (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#endif
        ବାଛ (endpoint_ptr_get_state(epptr)) {
        ସ୍ଥିତି EPState_Idle:
        ସ୍ଥିତି EPState_Recv: {

            ଯଦି (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_set_tsType(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_set_blockingObject(
                    &thread->tcbState, EP_REF(epptr));
#ifdef CONFIG_KERNEL_MCS
                thread_state_ptr_set_replyObject(&thread->tcbState, REPLY_REF(replyPtr));
#ଅନ୍ୟଥା
                thread_state_ptr_set_blockingIPCCanGrant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#endif
                scheduleTCB(thread);
#ifdef CONFIG_KERNEL_MCS
                ଯଦି (replyPtr) {
                    replyPtr->replyTCB = thread;
                }
#endif

                /* Place calling thread in endpoint queue */
#ifdef CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#ଅନ୍ୟଥା
                tcb_queue_t queue;
                queue = ep_ptr_get_queue(epptr);
                queue = tcbEPAppend(thread, queue);
                endpoint_ptr_set_state(epptr, EPState_Recv);
                ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */
            } ଅନ୍ୟଥା {
                doNBRecvFailedTransfer(thread);
            }
            ଭାଙ୍ଗ;
        }

        ସ୍ଥିତି EPState_Send: {
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
            ପ୍ରତିଜ୍ଞା(sender);

            /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#ଅନ୍ୟଥା
            queue = tcbEPDequeue(sender, queue);
            ep_ptr_set_queue(epptr, queue);

            ଯଦି (!queue.head) {
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
            ଯଦି (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                ପ୍ରତିଜ୍ଞା(sender->tcbSchedContext != ଗଣ୍ଠି_STATE(ksCurSC));
                ଯଦି (sender->tcbSchedContext != ଗଣ୍ଠି_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            ଯଦି (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                ଯଦି ((canGrant || canGrantReply) && replyPtr != ଖାଲି) {
                    bool_t canDonate = sender->tcbSchedContext != ଖାଲି
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    reply_ଠେଲ(sender, thread, replyPtr, canDonate);
                } ଅନ୍ୟଥା {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } ଅନ୍ୟଥା {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
                ପ୍ରତିଜ୍ଞା(sender->tcbSchedContext == ଖାଲି || refill_sufficient(sender->tcbSchedContext, 0));
            }
#ଅନ୍ୟଥା
            ଯଦି (do_call) {
                ଯଦି (canGrant || canGrantReply) {
                    setupCallerCap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } ଅନ୍ୟଥା {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } ଅନ୍ୟଥା {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
            }
#endif
            ଭାଙ୍ଗ;
        }
        }
    }
}

ଶୂନ୍ୟ replyFromKernel_error(tcb_t *thread)
{
    word_t len;
    word_t *ipcBuffer;

    ipcBuffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    len = setMRs_syscall_error(thread, ipcBuffer);

#ifdef CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    ଅକ୍ଷର *debugBuffer = (ଅକ୍ଷର *)(ipcBuffer + DEBUG_MESSAGE_START + 1);
    word_t add = strlcpy(debugBuffer, (ଅକ୍ଷର *)current_debug_error.errorMessage,
                         DEBUG_MESSAGE_MAXLEN * ଆକାର(word_t));

    len += (add / ଆକାର(word_t)) + 1;
#endif

    setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(current_syscall_error.type, 0, 0, len)));
}

ଶୂନ୍ୟ replyFromKernel_success_empty(tcb_t *thread)
{
    setRegister(thread, badgeRegister, 0);
    setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 0)));
}

ଶୂନ୍ୟ cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#ifdef CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#endif

    ବାଛ (thread_state_ptr_get_tsType(state)) {
    ସ୍ଥିତି ThreadState_BlockedOnSend:
    ସ୍ଥିତି ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        ପ୍ରତିଜ୍ଞା(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#ଅନ୍ୟଥା
        tcb_queue_t queue;
        queue = ep_ptr_get_queue(epptr);
        queue = tcbEPDequeue(tptr, queue);
        ep_ptr_set_queue(epptr, queue);

        ଯଦି (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
        ଯଦି (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            reply_t *reply = REPLY_PTR(thread_state_ptr_get_replyObject(state));
            ଯଦି (reply != ଖାଲି) {
                reply_unlink(reply, tptr);
            }
        }
#endif
        setThreadState(tptr, ThreadState_Inactive);
        ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        ଭାଙ୍ଗ;

    ସ୍ଥିତି ThreadState_BlockedOnReply: {
#ifdef CONFIG_KERNEL_MCS
        reply_remove_tcb(tptr);
#ଅନ୍ୟଥା
        cte_t *slot, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        slot = TCB_PTR_CTE_PTR(tptr, tcbReply);

        callerCap = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Next(slot->cteMDBNode));
        ଯଦି (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#endif

        ଭାଙ୍ଗ;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ restart_thread_if_no_fault(tcb_t *thread)
{
    ଯଦି (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        setThreadState(thread, ThreadState_Restart);
        ଯଦି (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            ପ୍ରତିଜ୍ଞା(thread->tcbSchedContext != ଗଣ୍ଠି_STATE(ksCurSC));
            ଯଦି (thread->tcbSchedContext != ଗଣ୍ଠି_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possibleSwitchTo(thread);
    } ଅନ୍ୟଥା {
        setThreadState(thread, ThreadState_Inactive);
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    ଯଦି (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(thread->tcbState));
        ଯଦି (reply != ଖାଲି) {
            reply_unlink(reply, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#endif

ଶୂନ୍ୟ cancelAllIPC(endpoint_t *epptr)
{
    ବାଛ (endpoint_ptr_get_state(epptr)) {
    ସ୍ଥିତି EPState_Idle:
        ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
        /* Clear the queue and set all blocked threads to restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *next;

        queue = ep_ptr_get_queue(epptr);

        ପାଇଁ (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#ଅନ୍ୟଥା
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_epQueue_head(epptr));

        /* Make endpoint idle */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        ପାଇଁ (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif

        rescheduleRequired();
        ଭାଙ୍ଗ;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, word_t badge)
{
    word_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    ପ୍ରତିଜ୍ଞା(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    ଯଦି (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#endif

ଶୂନ୍ୟ cancelBadgedSends(endpoint_t *epptr, word_t badge)
{
    ବାଛ (endpoint_ptr_get_state(epptr)) {
    ସ୍ଥିତି EPState_Idle:
    ସ୍ଥିତି EPState_Recv:
        ଭାଙ୍ଗ;

    ସ୍ଥିତି EPState_Send: {
        tcb_t *thread, *next;
        tcb_queue_t queue = ep_ptr_get_queue(epptr);

#ifdef CONFIG_KERNEL_MCS
        ପାଇଁ (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#ଅନ୍ୟଥା
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        ପାଇଁ (thread = queue.head; thread; thread = next) {
            word_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            next = thread->tcbEPNext;

            ଯଦି (b == badge) {
                setThreadState(thread, ThreadState_Restart);
                SCHED_ENQUEUE(thread);
                queue = tcbEPDequeue(thread, queue);
            }

        }
        ep_ptr_set_queue(epptr, queue);

        ଯଦି (queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Send);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        ଭାଙ୍ଗ;
    }

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("invalid EP state");
    }
}

#ifdef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
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

ଶୂନ୍ୟ tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcb_queue_remove(queue, thread);
    ep_ptr_set_queue(epptr, new_queue);

    ଯଦି (tcb_queue_empty(new_queue)) {
        endpoint_ptr_set_state(epptr, EPState_Idle);
    }
}

ଶୂନ୍ୟ reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_queue_t queue = ep_ptr_get_queue(epptr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ep_ptr_set_queue(epptr, queue);
}
#endif
