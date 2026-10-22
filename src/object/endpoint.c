/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <types.h>
#समावेश <string.h>
#समावेश <sel4/constants.h>
#समावेश <kernel/thread.h>
#समावेश <kernel/vspace.h>
#समावेश <machine/registerset.h>
#समावेश <model/statedata.h>
#समावेश <object/notification.h>
#समावेश <object/cnode.h>
#समावेश <object/endpoint.h>
#समावेश <object/tcb.h>

#अगर_घोषित CONFIG_KERNEL_MCS
व्योम sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#अन्यथा
व्योम sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, tcb_t *thread, endpoint_t *epptr)
#पूर्ण_अगर
{
    चयन (endpoint_ptr_get_state(epptr)) {
    हाल EPState_Idle:
    हाल EPState_Send:
        अगर (blocking) {

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
#अगर_घोषित CONFIG_KERNEL_MCS
            tcbEPAppend(thread, epptr, EPState_Send);
#अन्यथा
            tcb_queue_t queue;
            queue = ep_ptr_get_queue(epptr);
            queue = tcbEPAppend(thread, queue);
            endpoint_ptr_set_state(epptr, EPState_Send);
            ep_ptr_set_queue(epptr, queue);
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */

        }
        अवरोध;

    हाल EPState_Recv: {
        tcb_queue_t queue;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        queue = ep_ptr_get_queue(epptr);
        dest = queue.शीर्ष;

        /* Haskell error "Receive endpoint queue must not be empty" */
        निश्चित(dest);

        /* Dequeue the first TCB */
#अगर_घोषित CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#अन्यथा
        queue = tcbEPDequeue(dest, queue);
        ep_ptr_set_queue(epptr, queue);

        अगर (!queue.शीर्ष) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#अगर_घोषित CONFIG_KERNEL_MCS
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(dest->tcbState));
        अगर (reply) {
            reply_unlink(reply, dest);
        }

        अगर (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            अगर (reply != शून्य && (canGrant || canGrantReply)) {
                reply_धकेलो(thread, dest, reply, canDonate);
            } अन्यथा {
                setThreadState(thread, ThreadState_Inactive);
            }
        } अन्यथा अगर (canDonate && dest->tcbSchedContext == शून्य) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        setThreadState(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        निश्चित(dest_sc == शून्य || refill_sufficient(dest_sc, 0));
        निश्चित(dest_sc == शून्य || refill_ready(dest_sc));
        अगर (sc_sporadic(dest_sc) && dest_sc != गाँठ_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possibleSwitchTo(dest);
#अन्यथा
        bool_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        setThreadState(dest, ThreadState_Running);
        possibleSwitchTo(dest);

        अगर (do_call) {
            अगर (canGrant || canGrantReply) {
                setupCallerCap(thread, dest, replyCanGrant);
            } अन्यथा {
                setThreadState(thread, ThreadState_Inactive);
            }
        }
#पूर्ण_अगर
        अवरोध;
    }
    }
}

#अगर_घोषित CONFIG_KERNEL_MCS
व्योम receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t replyCap)
#अन्यथा
व्योम receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#पूर्ण_अगर
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    निश्चित(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#अगर_घोषित CONFIG_KERNEL_MCS
    reply_t *replyPtr = शून्य;
    अगर (cap_get_capType(replyCap) == cap_reply_cap) {
        replyPtr = REPLY_PTR(cap_reply_cap_get_capReplyPtr(replyCap));
        tcb_t *reply_tcb = replyPtr->replyTCB;
        अगर (unlikely(reply_tcb != शून्य && reply_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(reply_tcb);
        }
    }
#पूर्ण_अगर

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    अगर (ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {
        completeSignal(ntfnPtr, thread);
    } अन्यथा {
#अगर_घोषित CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        अगर (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#पूर्ण_अगर
        चयन (endpoint_ptr_get_state(epptr)) {
        हाल EPState_Idle:
        हाल EPState_Recv: {

            अगर (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_set_tsType(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_set_blockingObject(
                    &thread->tcbState, EP_REF(epptr));
#अगर_घोषित CONFIG_KERNEL_MCS
                thread_state_ptr_set_replyObject(&thread->tcbState, REPLY_REF(replyPtr));
#अन्यथा
                thread_state_ptr_set_blockingIPCCanGrant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#पूर्ण_अगर
                scheduleTCB(thread);
#अगर_घोषित CONFIG_KERNEL_MCS
                अगर (replyPtr) {
                    replyPtr->replyTCB = thread;
                }
#पूर्ण_अगर

                /* Place calling thread in endpoint queue */
#अगर_घोषित CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#अन्यथा
                tcb_queue_t queue;
                queue = ep_ptr_get_queue(epptr);
                queue = tcbEPAppend(thread, queue);
                endpoint_ptr_set_state(epptr, EPState_Recv);
                ep_ptr_set_queue(epptr, queue);
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */
            } अन्यथा {
                doNBRecvFailedTransfer(thread);
            }
            अवरोध;
        }

        हाल EPState_Send: {
            tcb_queue_t queue;
            tcb_t *sender;
            word_t badge;
            bool_t canGrant;
            bool_t canGrantReply;
            bool_t do_call;

            /* Get the head of the endpoint queue. */
            queue = ep_ptr_get_queue(epptr);
            sender = queue.शीर्ष;

            /* Haskell error "Send endpoint queue must not be empty" */
            निश्चित(sender);

            /* Dequeue the first TCB */
#अगर_घोषित CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#अन्यथा
            queue = tcbEPDequeue(sender, queue);
            ep_ptr_set_queue(epptr, queue);

            अगर (!queue.शीर्ष) {
                endpoint_ptr_set_state(epptr, EPState_Idle);
            }
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */

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

#अगर_घोषित CONFIG_KERNEL_MCS
            अगर (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                निश्चित(sender->tcbSchedContext != गाँठ_STATE(ksCurSC));
                अगर (sender->tcbSchedContext != गाँठ_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            अगर (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                अगर ((canGrant || canGrantReply) && replyPtr != शून्य) {
                    bool_t canDonate = sender->tcbSchedContext != शून्य
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    reply_धकेलो(sender, thread, replyPtr, canDonate);
                } अन्यथा {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } अन्यथा {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
                निश्चित(sender->tcbSchedContext == शून्य || refill_sufficient(sender->tcbSchedContext, 0));
            }
#अन्यथा
            अगर (do_call) {
                अगर (canGrant || canGrantReply) {
                    setupCallerCap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } अन्यथा {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } अन्यथा {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
            }
#पूर्ण_अगर
            अवरोध;
        }
        }
    }
}

व्योम replyFromKernel_error(tcb_t *thread)
{
    word_t len;
    word_t *ipc_बफर;

    ipc_बफर = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    len = setMRs_syscall_error(thread, ipc_बफर);

#अगर_घोषित CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    अक्षर *debug_बफर = (अक्षर *)(ipc_बफर + DEBUG_MESSAGE_START + 1);
    word_t add = strlcpy(debug_बफर, (अक्षर *)current_debug_error.errorMessage,
                         DEBUG_MESSAGE_MAXLEN * माप(word_t));

    len += (add / माप(word_t)) + 1;
#पूर्ण_अगर

    setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(current_syscall_error.type, 0, 0, len)));
}

व्योम replyFromKernel_success_empty(tcb_t *thread)
{
    setRegister(thread, badgeRegister, 0);
    setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 0)));
}

व्योम cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#अगर_घोषित CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#पूर्ण_अगर

    चयन (thread_state_ptr_get_tsType(state)) {
    हाल ThreadState_BlockedOnSend:
    हाल ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        निश्चित(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#अगर_घोषित CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#अन्यथा
        tcb_queue_t queue;
        queue = ep_ptr_get_queue(epptr);
        queue = tcbEPDequeue(tptr, queue);
        ep_ptr_set_queue(epptr, queue);

        अगर (!queue.शीर्ष) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */

#अगर_घोषित CONFIG_KERNEL_MCS
        अगर (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            reply_t *reply = REPLY_PTR(thread_state_ptr_get_replyObject(state));
            अगर (reply != शून्य) {
                reply_unlink(reply, tptr);
            }
        }
#पूर्ण_अगर
        setThreadState(tptr, ThreadState_Inactive);
        अवरोध;
    }

    हाल ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        अवरोध;

    हाल ThreadState_BlockedOnReply: {
#अगर_घोषित CONFIG_KERNEL_MCS
        reply_remove_tcb(tptr);
#अन्यथा
        cte_t *slot, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        slot = TCB_PTR_CTE_PTR(tptr, tcbReply);

        callerCap = CTE_PTR(mdb_गाँठ_get_mdb_अग्र(slot->cteMDBNode));
        अगर (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#पूर्ण_अगर

        अवरोध;
    }
    }
}

#अगर_घोषित CONFIG_KERNEL_MCS
अटल अंतरभूत व्योम restart_thread_if_no_fault(tcb_t *thread)
{
    अगर (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        setThreadState(thread, ThreadState_Restart);
        अगर (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            निश्चित(thread->tcbSchedContext != गाँठ_STATE(ksCurSC));
            अगर (thread->tcbSchedContext != गाँठ_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possibleSwitchTo(thread);
    } अन्यथा {
        setThreadState(thread, ThreadState_Inactive);
    }
}

अटल अंतरभूत व्योम removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    अगर (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(thread->tcbState));
        अगर (reply != शून्य) {
            reply_unlink(reply, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#पूर्ण_अगर

व्योम cancelAllIPC(endpoint_t *epptr)
{
    चयन (endpoint_ptr_get_state(epptr)) {
    हाल EPState_Idle:
        अवरोध;

    शेष: {
        /* Clear the queue and set all blocked threads to restart */
#अगर_घोषित CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *अग्र;

        queue = ep_ptr_get_queue(epptr);

        क्रम (thread = queue.शीर्ष; thread; thread = अग्र) {
            अग्र = thread->tcb_Sched_अग्र;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#अन्यथा
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_ep_Queue_शीर्ष(epptr));

        /* Make endpoint idle */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_ep_Queue_शीर्ष(epptr, 0);
        endpoint_ptr_set_ep_Queue_पुच्छ(epptr, 0);

        क्रम (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#पूर्ण_अगर

        rescheduleRequired();
        अवरोध;
    }
    }
}

#अगर_घोषित CONFIG_KERNEL_MCS
अटल अंतरभूत व्योम removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, word_t badge)
{
    word_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    निश्चित(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    अगर (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#पूर्ण_अगर

व्योम cancelBadgedSends(endpoint_t *epptr, word_t badge)
{
    चयन (endpoint_ptr_get_state(epptr)) {
    हाल EPState_Idle:
    हाल EPState_Recv:
        अवरोध;

    हाल EPState_Send: {
        tcb_t *thread, *अग्र;
        tcb_queue_t queue = ep_ptr_get_queue(epptr);

#अगर_घोषित CONFIG_KERNEL_MCS
        क्रम (thread = queue.शीर्ष; thread; thread = अग्र) {
            अग्र = thread->tcb_Sched_अग्र;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#अन्यथा
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_ep_Queue_शीर्ष(epptr, 0);
        endpoint_ptr_set_ep_Queue_पुच्छ(epptr, 0);

        क्रम (thread = queue.शीर्ष; thread; thread = अग्र) {
            word_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            अग्र = thread->tcbEPNext;

            अगर (b == badge) {
                setThreadState(thread, ThreadState_Restart);
                SCHED_ENQUEUE(thread);
                queue = tcbEPDequeue(thread, queue);
            }

        }
        ep_ptr_set_queue(epptr, queue);

        अगर (queue.शीर्ष) {
            endpoint_ptr_set_state(epptr, EPState_Send);
        }
#पूर्ण_अगर /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        अवरोध;
    }

    शेष:
        fail("invalid EP state");
    }
}

#अगर_घोषित CONFIG_KERNEL_MCS
व्योम tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
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

व्योम tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcb_queue_remove(queue, thread);
    ep_ptr_set_queue(epptr, new_queue);

    अगर (tcb_queue_empty(new_queue)) {
        endpoint_ptr_set_state(epptr, EPState_Idle);
    }
}

व्योम reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_queue_t queue = ep_ptr_get_queue(epptr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ep_ptr_set_queue(epptr, queue);
}
#पूर्ण_अगर
