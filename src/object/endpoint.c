/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sel4/constants.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <object/notification.h>
#अन्तर्भाव <object/cnode.h>
#अन्तर्भाव <object/endpoint.h>
#अन्तर्भाव <object/tcb.h>

#ifdef CONFIG_KERNEL_MCS
शून्य sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#अण्णहा
शून्य sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, tcb_t *thread, endpoint_t *epptr)
#endif
{
    विकल्प (endpoint_ptr_get_state(epptr)) {
    पक्ष EPState_Idle:
    पक्ष EPState_Send:
        जइ (blocking) {

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
#अण्णहा
            tcb_queue_t queue;
            queue = ep_ptr_get_queue(epptr);
            queue = tcbEPAppend(thread, queue);
            endpoint_ptr_set_state(epptr, EPState_Send);
            ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */

        }
        भङ्ग;

    पक्ष EPState_Recv: {
        tcb_queue_t queue;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        queue = ep_ptr_get_queue(epptr);
        dest = queue.head;

        /* Haskell error "Receive endpoint queue must not be empty" */
        प्रतिज्ञा(dest);

        /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#अण्णहा
        queue = tcbEPDequeue(dest, queue);
        ep_ptr_set_queue(epptr, queue);

        जइ (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#ifdef CONFIG_KERNEL_MCS
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(dest->tcbState));
        जइ (reply) {
            reply_unlink(reply, dest);
        }

        जइ (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            जइ (reply != रिक्त && (canGrant || canGrantReply)) {
                reply_प्रक्षिप(thread, dest, reply, canDonate);
            } अण्णहा {
                setThreadState(thread, ThreadState_Inactive);
            }
        } अण्णहा जइ (canDonate && dest->tcbSchedContext == रिक्त) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        setThreadState(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        प्रतिज्ञा(dest_sc == रिक्त || refill_sufficient(dest_sc, 0));
        प्रतिज्ञा(dest_sc == रिक्त || refill_ready(dest_sc));
        जइ (sc_sporadic(dest_sc) && dest_sc != ग्रन्थिः_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possibleSwitchTo(dest);
#अण्णहा
        bool_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        setThreadState(dest, ThreadState_Running);
        possibleSwitchTo(dest);

        जइ (do_call) {
            जइ (canGrant || canGrantReply) {
                setupCallerCap(thread, dest, replyCanGrant);
            } अण्णहा {
                setThreadState(thread, ThreadState_Inactive);
            }
        }
#endif
        भङ्ग;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
शून्य receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t replyCap)
#अण्णहा
शून्य receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#endif
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    प्रतिज्ञा(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#ifdef CONFIG_KERNEL_MCS
    reply_t *replyPtr = रिक्त;
    जइ (cap_get_capType(replyCap) == cap_reply_cap) {
        replyPtr = REPLY_PTR(cap_reply_cap_get_capReplyPtr(replyCap));
        tcb_t *reply_tcb = replyPtr->replyTCB;
        जइ (unlikely(reply_tcb != रिक्त && reply_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(reply_tcb);
        }
    }
#endif

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    जइ (ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {
        completeSignal(ntfnPtr, thread);
    } अण्णहा {
#ifdef CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        जइ (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#endif
        विकल्प (endpoint_ptr_get_state(epptr)) {
        पक्ष EPState_Idle:
        पक्ष EPState_Recv: {

            जइ (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_set_tsType(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_set_blockingObject(
                    &thread->tcbState, EP_REF(epptr));
#ifdef CONFIG_KERNEL_MCS
                thread_state_ptr_set_replyObject(&thread->tcbState, REPLY_REF(replyPtr));
#अण्णहा
                thread_state_ptr_set_blockingIPCCanGrant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#endif
                scheduleTCB(thread);
#ifdef CONFIG_KERNEL_MCS
                जइ (replyPtr) {
                    replyPtr->replyTCB = thread;
                }
#endif

                /* Place calling thread in endpoint queue */
#ifdef CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#अण्णहा
                tcb_queue_t queue;
                queue = ep_ptr_get_queue(epptr);
                queue = tcbEPAppend(thread, queue);
                endpoint_ptr_set_state(epptr, EPState_Recv);
                ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */
            } अण्णहा {
                doNBRecvFailedTransfer(thread);
            }
            भङ्ग;
        }

        पक्ष EPState_Send: {
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
            प्रतिज्ञा(sender);

            /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#अण्णहा
            queue = tcbEPDequeue(sender, queue);
            ep_ptr_set_queue(epptr, queue);

            जइ (!queue.head) {
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
            जइ (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                प्रतिज्ञा(sender->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
                जइ (sender->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            जइ (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                जइ ((canGrant || canGrantReply) && replyPtr != रिक्त) {
                    bool_t canDonate = sender->tcbSchedContext != रिक्त
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    reply_प्रक्षिप(sender, thread, replyPtr, canDonate);
                } अण्णहा {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } अण्णहा {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
                प्रतिज्ञा(sender->tcbSchedContext == रिक्त || refill_sufficient(sender->tcbSchedContext, 0));
            }
#अण्णहा
            जइ (do_call) {
                जइ (canGrant || canGrantReply) {
                    setupCallerCap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } अण्णहा {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } अण्णहा {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
            }
#endif
            भङ्ग;
        }
        }
    }
}

शून्य replyFromKernel_error(tcb_t *thread)
{
    word_t len;
    word_t *ipcBuffer;

    ipcBuffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    len = setMRs_syscall_error(thread, ipcBuffer);

#ifdef CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    वर्ण *debugBuffer = (वर्ण *)(ipcBuffer + DEBUG_MESSAGE_START + 1);
    word_t add = strlcpy(debugBuffer, (वर्ण *)current_debug_error.errorMessage,
                         DEBUG_MESSAGE_MAXLEN * परिमाण(word_t));

    len += (add / परिमाण(word_t)) + 1;
#endif

    setRegister(thread, सन्देशः_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(current_syscall_error.type, 0, 0, len)));
}

शून्य replyFromKernel_success_empty(tcb_t *thread)
{
    setRegister(thread, badgeRegister, 0);
    setRegister(thread, सन्देशः_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 0)));
}

शून्य cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#ifdef CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#endif

    विकल्प (thread_state_ptr_get_tsType(state)) {
    पक्ष ThreadState_BlockedOnSend:
    पक्ष ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        प्रतिज्ञा(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#अण्णहा
        tcb_queue_t queue;
        queue = ep_ptr_get_queue(epptr);
        queue = tcbEPDequeue(tptr, queue);
        ep_ptr_set_queue(epptr, queue);

        जइ (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
        जइ (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            reply_t *reply = REPLY_PTR(thread_state_ptr_get_replyObject(state));
            जइ (reply != रिक्त) {
                reply_unlink(reply, tptr);
            }
        }
#endif
        setThreadState(tptr, ThreadState_Inactive);
        भङ्ग;
    }

    पक्ष ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        भङ्ग;

    पक्ष ThreadState_BlockedOnReply: {
#ifdef CONFIG_KERNEL_MCS
        reply_remove_tcb(tptr);
#अण्णहा
        cte_t *slot, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        slot = TCB_PTR_CTE_PTR(tptr, tcbReply);

        callerCap = CTE_PTR(mdb_ग्रन्थिः_get_mdb_Next(slot->cteMDBNode));
        जइ (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#endif

        भङ्ग;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ शून्य restart_thread_if_no_fault(tcb_t *thread)
{
    जइ (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        setThreadState(thread, ThreadState_Restart);
        जइ (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            प्रतिज्ञा(thread->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
            जइ (thread->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possibleSwitchTo(thread);
    } अण्णहा {
        setThreadState(thread, ThreadState_Inactive);
    }
}

स्थिर अन्तःस्थ शून्य removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    जइ (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(thread->tcbState));
        जइ (reply != रिक्त) {
            reply_unlink(reply, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#endif

शून्य cancelAllIPC(endpoint_t *epptr)
{
    विकल्प (endpoint_ptr_get_state(epptr)) {
    पक्ष EPState_Idle:
        भङ्ग;

    स्वभाव: {
        /* Clear the queue and set all blocked threads to restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *next;

        queue = ep_ptr_get_queue(epptr);

        प्रति (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#अण्णहा
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_epQueue_head(epptr));

        /* Make endpoint idle */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        प्रति (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif

        rescheduleRequired();
        भङ्ग;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ शून्य removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, word_t badge)
{
    word_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    प्रतिज्ञा(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    जइ (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#endif

शून्य cancelBadgedSends(endpoint_t *epptr, word_t badge)
{
    विकल्प (endpoint_ptr_get_state(epptr)) {
    पक्ष EPState_Idle:
    पक्ष EPState_Recv:
        भङ्ग;

    पक्ष EPState_Send: {
        tcb_t *thread, *next;
        tcb_queue_t queue = ep_ptr_get_queue(epptr);

#ifdef CONFIG_KERNEL_MCS
        प्रति (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#अण्णहा
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        प्रति (thread = queue.head; thread; thread = next) {
            word_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            next = thread->tcbEPNext;

            जइ (b == badge) {
                setThreadState(thread, ThreadState_Restart);
                SCHED_ENQUEUE(thread);
                queue = tcbEPDequeue(thread, queue);
            }

        }
        ep_ptr_set_queue(epptr, queue);

        जइ (queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Send);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        भङ्ग;
    }

    स्वभाव:
        fail("invalid EP state");
    }
}

#ifdef CONFIG_KERNEL_MCS
शून्य tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
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

शून्य tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcb_queue_remove(queue, thread);
    ep_ptr_set_queue(epptr, new_queue);

    जइ (tcb_queue_empty(new_queue)) {
        endpoint_ptr_set_state(epptr, EPState_Idle);
    }
}

शून्य reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_queue_t queue = ep_ptr_get_queue(epptr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ep_ptr_set_queue(epptr, queue);
}
#endif
