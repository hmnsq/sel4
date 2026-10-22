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

#ifdef CONFIG_KERNEL_MCS
فراغ sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#وإلا
فراغ sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, tcb_t *thread, endpoint_t *epptr)
#endif
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
    حالة EPState_Send:
        إذا (blocking) {

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
#وإلا
            tcb_queue_t queue;
            queue = ep_ptr_get_queue(epptr);
            queue = tcbEPAppend(thread, queue);
            endpoint_ptr_set_state(epptr, EPState_Send);
            ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */

        }
        اكسر;

    حالة EPState_Recv: {
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
#وإلا
        queue = tcbEPDequeue(dest, queue);
        ep_ptr_set_queue(epptr, queue);

        إذا (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#ifdef CONFIG_KERNEL_MCS
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(dest->tcbState));
        إذا (reply) {
            reply_unlink(reply, dest);
        }

        إذا (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            إذا (reply != عدم && (canGrant || canGrantReply)) {
                reply_ئىتتىرىش(thread, dest, reply, canDonate);
            } وإلا {
                setThreadState(thread, ThreadState_Inactive);
            }
        } وإلا إذا (canDonate && dest->tcbSchedContext == عدم) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        setThreadState(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        أكد(dest_sc == عدم || refill_sufficient(dest_sc, 0));
        أكد(dest_sc == عدم || refill_ready(dest_sc));
        إذا (sc_sporadic(dest_sc) && dest_sc != تۈگۈن_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possibleSwitchTo(dest);
#وإلا
        bool_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        setThreadState(dest, ThreadState_Running);
        possibleSwitchTo(dest);

        إذا (do_call) {
            إذا (canGrant || canGrantReply) {
                setupCallerCap(thread, dest, replyCanGrant);
            } وإلا {
                setThreadState(thread, ThreadState_Inactive);
            }
        }
#endif
        اكسر;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
فراغ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t replyCap)
#وإلا
فراغ receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#endif
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    أكد(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#ifdef CONFIG_KERNEL_MCS
    reply_t *replyPtr = عدم;
    إذا (cap_get_capType(replyCap) == cap_reply_cap) {
        replyPtr = REPLY_PTR(cap_reply_cap_get_capReplyPtr(replyCap));
        tcb_t *reply_tcb = replyPtr->replyTCB;
        إذا (unlikely(reply_tcb != عدم && reply_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(reply_tcb);
        }
    }
#endif

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    إذا (ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {
        completeSignal(ntfnPtr, thread);
    } وإلا {
#ifdef CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        إذا (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#endif
        بدل (endpoint_ptr_get_state(epptr)) {
        حالة EPState_Idle:
        حالة EPState_Recv: {

            إذا (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_set_tsType(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_set_blockingObject(
                    &thread->tcbState, EP_REF(epptr));
#ifdef CONFIG_KERNEL_MCS
                thread_state_ptr_set_replyObject(&thread->tcbState, REPLY_REF(replyPtr));
#وإلا
                thread_state_ptr_set_blockingIPCCanGrant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#endif
                scheduleTCB(thread);
#ifdef CONFIG_KERNEL_MCS
                إذا (replyPtr) {
                    replyPtr->replyTCB = thread;
                }
#endif

                /* Place calling thread in endpoint queue */
#ifdef CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#وإلا
                tcb_queue_t queue;
                queue = ep_ptr_get_queue(epptr);
                queue = tcbEPAppend(thread, queue);
                endpoint_ptr_set_state(epptr, EPState_Recv);
                ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */
            } وإلا {
                doNBRecvFailedTransfer(thread);
            }
            اكسر;
        }

        حالة EPState_Send: {
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
#وإلا
            queue = tcbEPDequeue(sender, queue);
            ep_ptr_set_queue(epptr, queue);

            إذا (!queue.head) {
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
            إذا (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                أكد(sender->tcbSchedContext != تۈگۈن_STATE(ksCurSC));
                إذا (sender->tcbSchedContext != تۈگۈن_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            إذا (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                إذا ((canGrant || canGrantReply) && replyPtr != عدم) {
                    bool_t canDonate = sender->tcbSchedContext != عدم
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    reply_ئىتتىرىش(sender, thread, replyPtr, canDonate);
                } وإلا {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } وإلا {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
                أكد(sender->tcbSchedContext == عدم || refill_sufficient(sender->tcbSchedContext, 0));
            }
#وإلا
            إذا (do_call) {
                إذا (canGrant || canGrantReply) {
                    setupCallerCap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } وإلا {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } وإلا {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
            }
#endif
            اكسر;
        }
        }
    }
}

فراغ replyFromKernel_error(tcb_t *thread)
{
    word_t len;
    word_t *ipcBuffer;

    ipcBuffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    len = setMRs_syscall_error(thread, ipcBuffer);

#ifdef CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    حرف *debugBuffer = (حرف *)(ipcBuffer + DEBUG_MESSAGE_START + 1);
    word_t add = strlcpy(debugBuffer, (حرف *)current_debug_error.errorMessage,
                         DEBUG_MESSAGE_MAXLEN * حجم(word_t));

    len += (add / حجم(word_t)) + 1;
#endif

    setRegister(thread, ئۇچۇر_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(current_syscall_error.type, 0, 0, len)));
}

فراغ replyFromKernel_success_empty(tcb_t *thread)
{
    setRegister(thread, badgeRegister, 0);
    setRegister(thread, ئۇچۇر_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 0)));
}

فراغ cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#ifdef CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#endif

    بدل (thread_state_ptr_get_tsType(state)) {
    حالة ThreadState_BlockedOnSend:
    حالة ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        أكد(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#وإلا
        tcb_queue_t queue;
        queue = ep_ptr_get_queue(epptr);
        queue = tcbEPDequeue(tptr, queue);
        ep_ptr_set_queue(epptr, queue);

        إذا (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
        إذا (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            reply_t *reply = REPLY_PTR(thread_state_ptr_get_replyObject(state));
            إذا (reply != عدم) {
                reply_unlink(reply, tptr);
            }
        }
#endif
        setThreadState(tptr, ThreadState_Inactive);
        اكسر;
    }

    حالة ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        اكسر;

    حالة ThreadState_BlockedOnReply: {
#ifdef CONFIG_KERNEL_MCS
        reply_remove_tcb(tptr);
#وإلا
        cte_t *slot, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        slot = TCB_PTR_CTE_PTR(tptr, tcbReply);

        callerCap = CTE_PTR(mdb_تۈگۈن_get_mdb_Next(slot->cteMDBNode));
        إذا (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#endif

        اكسر;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ساكن مضمن فراغ restart_thread_if_no_fault(tcb_t *thread)
{
    إذا (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        setThreadState(thread, ThreadState_Restart);
        إذا (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            أكد(thread->tcbSchedContext != تۈگۈن_STATE(ksCurSC));
            إذا (thread->tcbSchedContext != تۈگۈن_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possibleSwitchTo(thread);
    } وإلا {
        setThreadState(thread, ThreadState_Inactive);
    }
}

ساكن مضمن فراغ removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    إذا (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(thread->tcbState));
        إذا (reply != عدم) {
            reply_unlink(reply, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#endif

فراغ cancelAllIPC(endpoint_t *epptr)
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
        اكسر;

    افتراضي: {
        /* Clear the queue and set all blocked threads to restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *next;

        queue = ep_ptr_get_queue(epptr);

        لكل (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#وإلا
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_epQueue_head(epptr));

        /* Make endpoint idle */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        لكل (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif

        rescheduleRequired();
        اكسر;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ساكن مضمن فراغ removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, word_t badge)
{
    word_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    أكد(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    إذا (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#endif

فراغ cancelBadgedSends(endpoint_t *epptr, word_t badge)
{
    بدل (endpoint_ptr_get_state(epptr)) {
    حالة EPState_Idle:
    حالة EPState_Recv:
        اكسر;

    حالة EPState_Send: {
        tcb_t *thread, *next;
        tcb_queue_t queue = ep_ptr_get_queue(epptr);

#ifdef CONFIG_KERNEL_MCS
        لكل (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#وإلا
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        لكل (thread = queue.head; thread; thread = next) {
            word_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            next = thread->tcbEPNext;

            إذا (b == badge) {
                setThreadState(thread, ThreadState_Restart);
                SCHED_ENQUEUE(thread);
                queue = tcbEPDequeue(thread, queue);
            }

        }
        ep_ptr_set_queue(epptr, queue);

        إذا (queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Send);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        اكسر;
    }

    افتراضي:
        fail("invalid EP state");
    }
}

#ifdef CONFIG_KERNEL_MCS
فراغ tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
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

فراغ tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcb_queue_remove(queue, thread);
    ep_ptr_set_queue(epptr, new_queue);

    إذا (tcb_queue_empty(new_queue)) {
        endpoint_ptr_set_state(epptr, EPState_Idle);
    }
}

فراغ reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_queue_t queue = ep_ptr_get_queue(epptr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ep_ptr_set_queue(epptr, queue);
}
#endif
