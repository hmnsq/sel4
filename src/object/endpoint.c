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

#ifdef CONFIG_KERNEL_MCS
تهی sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#وگرنه
تهی sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, tcb_t *thread, endpoint_t *epptr)
#endif
{
    انتخاب (endpoint_ptr_get_state(epptr)) {
    حالت EPState_Idle:
    حالت EPState_Send:
        اگر (blocking) {

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
#وگرنه
            tcb_queue_t queue;
            queue = ep_ptr_get_queue(epptr);
            queue = tcbEPAppend(thread, queue);
            endpoint_ptr_set_state(epptr, EPState_Send);
            ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */

        }
        بشکن;

    حالت EPState_Recv: {
        tcb_queue_t queue;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        queue = ep_ptr_get_queue(epptr);
        dest = queue.head;

        /* Haskell error "Receive endpoint queue must not be empty" */
        أكد(dest);

        /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#وگرنه
        queue = tcbEPDequeue(dest, queue);
        ep_ptr_set_queue(epptr, queue);

        اگر (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#ifdef CONFIG_KERNEL_MCS
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(dest->tcbState));
        اگر (reply) {
            reply_unlink(reply, dest);
        }

        اگر (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            اگر (reply != پوچ && (canGrant || canGrantReply)) {
                reply_افزودن(thread, dest, reply, canDonate);
            } وگرنه {
                setThreadState(thread, ThreadState_Inactive);
            }
        } وگرنه اگر (canDonate && dest->tcbSchedContext == پوچ) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        setThreadState(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        أكد(dest_sc == پوچ || refill_sufficient(dest_sc, 0));
        أكد(dest_sc == پوچ || refill_ready(dest_sc));
        اگر (sc_sporadic(dest_sc) && dest_sc != گره_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possibleSwitchTo(dest);
#وگرنه
        bool_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        setThreadState(dest, ThreadState_Running);
        possibleSwitchTo(dest);

        اگر (do_call) {
            اگر (canGrant || canGrantReply) {
                setupCallerCap(thread, dest, replyCanGrant);
            } وگرنه {
                setThreadState(thread, ThreadState_Inactive);
            }
        }
#endif
        بشکن;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
تهی receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t replyCap)
#وگرنه
تهی receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#endif
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    أكد(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#ifdef CONFIG_KERNEL_MCS
    reply_t *replyPtr = پوچ;
    اگر (cap_get_capType(replyCap) == cap_reply_cap) {
        replyPtr = REPLY_PTR(cap_reply_cap_get_capReplyPtr(replyCap));
        tcb_t *reply_tcb = replyPtr->replyTCB;
        اگر (unlikely(reply_tcb != پوچ && reply_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(reply_tcb);
        }
    }
#endif

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    اگر (ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {
        completeSignal(ntfnPtr, thread);
    } وگرنه {
#ifdef CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        اگر (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#endif
        انتخاب (endpoint_ptr_get_state(epptr)) {
        حالت EPState_Idle:
        حالت EPState_Recv: {

            اگر (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_set_tsType(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_set_blockingObject(
                    &thread->tcbState, EP_REF(epptr));
#ifdef CONFIG_KERNEL_MCS
                thread_state_ptr_set_replyObject(&thread->tcbState, REPLY_REF(replyPtr));
#وگرنه
                thread_state_ptr_set_blockingIPCCanGrant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#endif
                scheduleTCB(thread);
#ifdef CONFIG_KERNEL_MCS
                اگر (replyPtr) {
                    replyPtr->replyTCB = thread;
                }
#endif

                /* Place calling thread in endpoint queue */
#ifdef CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#وگرنه
                tcb_queue_t queue;
                queue = ep_ptr_get_queue(epptr);
                queue = tcbEPAppend(thread, queue);
                endpoint_ptr_set_state(epptr, EPState_Recv);
                ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */
            } وگرنه {
                doNBRecvFailedTransfer(thread);
            }
            بشکن;
        }

        حالت EPState_Send: {
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
            أكد(sender);

            /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#وگرنه
            queue = tcbEPDequeue(sender, queue);
            ep_ptr_set_queue(epptr, queue);

            اگر (!queue.head) {
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
            اگر (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                أكد(sender->tcbSchedContext != گره_STATE(ksCurSC));
                اگر (sender->tcbSchedContext != گره_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            اگر (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                اگر ((canGrant || canGrantReply) && replyPtr != پوچ) {
                    bool_t canDonate = sender->tcbSchedContext != پوچ
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    reply_افزودن(sender, thread, replyPtr, canDonate);
                } وگرنه {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } وگرنه {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
                أكد(sender->tcbSchedContext == پوچ || refill_sufficient(sender->tcbSchedContext, 0));
            }
#وگرنه
            اگر (do_call) {
                اگر (canGrant || canGrantReply) {
                    setupCallerCap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } وگرنه {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } وگرنه {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
            }
#endif
            بشکن;
        }
        }
    }
}

تهی replyFromKernel_error(tcb_t *thread)
{
    word_t len;
    word_t *ipcBuffer;

    ipcBuffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    len = setMRs_syscall_error(thread, ipcBuffer);

#ifdef CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    نویسه *debugBuffer = (نویسه *)(ipcBuffer + DEBUG_MESSAGE_START + 1);
    word_t add = strlcpy(debugBuffer, (نویسه *)current_debug_error.errorMessage,
                         DEBUG_MESSAGE_MAXLEN * اندازه(word_t));

    len += (add / اندازه(word_t)) + 1;
#endif

    setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(current_syscall_error.type, 0, 0, len)));
}

تهی replyFromKernel_success_empty(tcb_t *thread)
{
    setRegister(thread, badgeRegister, 0);
    setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 0)));
}

تهی cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#ifdef CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#endif

    انتخاب (thread_state_ptr_get_tsType(state)) {
    حالت ThreadState_BlockedOnSend:
    حالت ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        أكد(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#وگرنه
        tcb_queue_t queue;
        queue = ep_ptr_get_queue(epptr);
        queue = tcbEPDequeue(tptr, queue);
        ep_ptr_set_queue(epptr, queue);

        اگر (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
        اگر (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            reply_t *reply = REPLY_PTR(thread_state_ptr_get_replyObject(state));
            اگر (reply != پوچ) {
                reply_unlink(reply, tptr);
            }
        }
#endif
        setThreadState(tptr, ThreadState_Inactive);
        بشکن;
    }

    حالت ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        بشکن;

    حالت ThreadState_BlockedOnReply: {
#ifdef CONFIG_KERNEL_MCS
        reply_remove_tcb(tptr);
#وگرنه
        cte_t *slot, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        slot = TCB_PTR_CTE_PTR(tptr, tcbReply);

        callerCap = CTE_PTR(mdb_گره_get_mdb_Next(slot->cteMDBNode));
        اگر (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#endif

        بشکن;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ایستا درونخطی تهی restart_thread_if_no_fault(tcb_t *thread)
{
    اگر (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        setThreadState(thread, ThreadState_Restart);
        اگر (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(thread->tcbSchedContext != گره_STATE(ksCurSC));
            اگر (thread->tcbSchedContext != گره_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possibleSwitchTo(thread);
    } وگرنه {
        setThreadState(thread, ThreadState_Inactive);
    }
}

ایستا درونخطی تهی removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    اگر (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(thread->tcbState));
        اگر (reply != پوچ) {
            reply_unlink(reply, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#endif

تهی cancelAllIPC(endpoint_t *epptr)
{
    انتخاب (endpoint_ptr_get_state(epptr)) {
    حالت EPState_Idle:
        بشکن;

    پیشفرض: {
        /* Clear the queue and set all blocked threads to restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *next;

        queue = ep_ptr_get_queue(epptr);

        برای (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#وگرنه
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_epQueue_head(epptr));

        /* Make endpoint idle */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        برای (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif

        rescheduleRequired();
        بشکن;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ایستا درونخطی تهی removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, word_t badge)
{
    word_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    أكد(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    اگر (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#endif

تهی cancelBadgedSends(endpoint_t *epptr, word_t badge)
{
    انتخاب (endpoint_ptr_get_state(epptr)) {
    حالت EPState_Idle:
    حالت EPState_Recv:
        بشکن;

    حالت EPState_Send: {
        tcb_t *thread, *next;
        tcb_queue_t queue = ep_ptr_get_queue(epptr);

#ifdef CONFIG_KERNEL_MCS
        برای (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#وگرنه
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        برای (thread = queue.head; thread; thread = next) {
            word_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            next = thread->tcbEPNext;

            اگر (b == badge) {
                setThreadState(thread, ThreadState_Restart);
                SCHED_ENQUEUE(thread);
                queue = tcbEPDequeue(thread, queue);
            }

        }
        ep_ptr_set_queue(epptr, queue);

        اگر (queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Send);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        بشکن;
    }

    پیشفرض:
        fail("invalid EP state");
    }
}

#ifdef CONFIG_KERNEL_MCS
تهی tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
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

تهی tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcb_queue_remove(queue, thread);
    ep_ptr_set_queue(epptr, new_queue);

    اگر (tcb_queue_empty(new_queue)) {
        endpoint_ptr_set_state(epptr, EPState_Idle);
    }
}

تهی reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_queue_t queue = ep_ptr_get_queue(epptr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ep_ptr_set_queue(epptr, queue);
}
#endif
