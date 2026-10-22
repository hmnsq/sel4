/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sel4/constants.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <machine/registerset.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <object/notification.h>
#অন্তর্ভাব <object/cnode.h>
#অন্তর্ভাব <object/endpoint.h>
#অন্তর্ভাব <object/tcb.h>

#ifdef CONFIG_KERNEL_MCS
শূন্য sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#অন্যথা
শূন্য sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, tcb_t *thread, endpoint_t *epptr)
#endif
{
    নির্বাচন (endpoint_ptr_get_state(epptr)) {
    ক্ষেত্র EPState_Idle:
    ক্ষেত্র EPState_Send:
        যদি (blocking) {

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
#অন্যথা
            tcb_queue_t queue;
            queue = ep_ptr_get_queue(epptr);
            queue = tcbEPAppend(thread, queue);
            endpoint_ptr_set_state(epptr, EPState_Send);
            ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */

        }
        বিরতি;

    ক্ষেত্র EPState_Recv: {
        tcb_queue_t queue;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        queue = ep_ptr_get_queue(epptr);
        dest = queue.শীর্ষ;

        /* Haskell error "Receive endpoint queue must not be empty" */
        প্রতিজ্ঞা(dest);

        /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#অন্যথা
        queue = tcbEPDequeue(dest, queue);
        ep_ptr_set_queue(epptr, queue);

        যদি (!queue.শীর্ষ) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#ifdef CONFIG_KERNEL_MCS
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(dest->tcbState));
        যদি (reply) {
            reply_unlink(reply, dest);
        }

        যদি (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            যদি (reply != ফাঁকা && (canGrant || canGrantReply)) {
                reply_ঠেলো(thread, dest, reply, canDonate);
            } অন্যথা {
                setThreadState(thread, ThreadState_Inactive);
            }
        } অন্যথা যদি (canDonate && dest->tcbSchedContext == ফাঁকা) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        setThreadState(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        প্রতিজ্ঞা(dest_sc == ফাঁকা || refill_sufficient(dest_sc, 0));
        প্রতিজ্ঞা(dest_sc == ফাঁকা || refill_ready(dest_sc));
        যদি (sc_sporadic(dest_sc) && dest_sc != গ্রন্থি_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possibleSwitchTo(dest);
#অন্যথা
        bool_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        setThreadState(dest, ThreadState_Running);
        possibleSwitchTo(dest);

        যদি (do_call) {
            যদি (canGrant || canGrantReply) {
                setupCallerCap(thread, dest, replyCanGrant);
            } অন্যথা {
                setThreadState(thread, ThreadState_Inactive);
            }
        }
#endif
        বিরতি;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
শূন্য receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t replyCap)
#অন্যথা
শূন্য receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#endif
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    প্রতিজ্ঞা(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#ifdef CONFIG_KERNEL_MCS
    reply_t *replyPtr = ফাঁকা;
    যদি (cap_get_capType(replyCap) == cap_reply_cap) {
        replyPtr = REPLY_PTR(cap_reply_cap_get_capReplyPtr(replyCap));
        tcb_t *reply_tcb = replyPtr->replyTCB;
        যদি (unlikely(reply_tcb != ফাঁকা && reply_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(reply_tcb);
        }
    }
#endif

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    যদি (ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {
        completeSignal(ntfnPtr, thread);
    } অন্যথা {
#ifdef CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        যদি (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#endif
        নির্বাচন (endpoint_ptr_get_state(epptr)) {
        ক্ষেত্র EPState_Idle:
        ক্ষেত্র EPState_Recv: {

            যদি (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_set_tsType(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_set_blockingObject(
                    &thread->tcbState, EP_REF(epptr));
#ifdef CONFIG_KERNEL_MCS
                thread_state_ptr_set_replyObject(&thread->tcbState, REPLY_REF(replyPtr));
#অন্যথা
                thread_state_ptr_set_blockingIPCCanGrant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#endif
                scheduleTCB(thread);
#ifdef CONFIG_KERNEL_MCS
                যদি (replyPtr) {
                    replyPtr->replyTCB = thread;
                }
#endif

                /* Place calling thread in endpoint queue */
#ifdef CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#অন্যথা
                tcb_queue_t queue;
                queue = ep_ptr_get_queue(epptr);
                queue = tcbEPAppend(thread, queue);
                endpoint_ptr_set_state(epptr, EPState_Recv);
                ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */
            } অন্যথা {
                doNBRecvFailedTransfer(thread);
            }
            বিরতি;
        }

        ক্ষেত্র EPState_Send: {
            tcb_queue_t queue;
            tcb_t *sender;
            word_t badge;
            bool_t canGrant;
            bool_t canGrantReply;
            bool_t do_call;

            /* Get the head of the endpoint queue. */
            queue = ep_ptr_get_queue(epptr);
            sender = queue.শীর্ষ;

            /* Haskell error "Send endpoint queue must not be empty" */
            প্রতিজ্ঞা(sender);

            /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#অন্যথা
            queue = tcbEPDequeue(sender, queue);
            ep_ptr_set_queue(epptr, queue);

            যদি (!queue.শীর্ষ) {
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
            যদি (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                প্রতিজ্ঞা(sender->tcbSchedContext != গ্রন্থি_STATE(ksCurSC));
                যদি (sender->tcbSchedContext != গ্রন্থি_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            যদি (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                যদি ((canGrant || canGrantReply) && replyPtr != ফাঁকা) {
                    bool_t canDonate = sender->tcbSchedContext != ফাঁকা
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    reply_ঠেলো(sender, thread, replyPtr, canDonate);
                } অন্যথা {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } অন্যথা {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
                প্রতিজ্ঞা(sender->tcbSchedContext == ফাঁকা || refill_sufficient(sender->tcbSchedContext, 0));
            }
#অন্যথা
            যদি (do_call) {
                যদি (canGrant || canGrantReply) {
                    setupCallerCap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } অন্যথা {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } অন্যথা {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
            }
#endif
            বিরতি;
        }
        }
    }
}

শূন্য replyFromKernel_error(tcb_t *thread)
{
    word_t len;
    word_t *ipc_বাফার;

    ipc_বাফার = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    len = setMRs_syscall_error(thread, ipc_বাফার);

#ifdef CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    অক্ষর *debug_বাফার = (অক্ষর *)(ipc_বাফার + DEBUG_MESSAGE_START + 1);
    word_t add = strlcpy(debug_বাফার, (অক্ষর *)current_debug_error.errorMessage,
                         DEBUG_MESSAGE_MAXLEN * আকার(word_t));

    len += (add / আকার(word_t)) + 1;
#endif

    setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(current_syscall_error.type, 0, 0, len)));
}

শূন্য replyFromKernel_success_empty(tcb_t *thread)
{
    setRegister(thread, badgeRegister, 0);
    setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 0)));
}

শূন্য cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#ifdef CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#endif

    নির্বাচন (thread_state_ptr_get_tsType(state)) {
    ক্ষেত্র ThreadState_BlockedOnSend:
    ক্ষেত্র ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        প্রতিজ্ঞা(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#অন্যথা
        tcb_queue_t queue;
        queue = ep_ptr_get_queue(epptr);
        queue = tcbEPDequeue(tptr, queue);
        ep_ptr_set_queue(epptr, queue);

        যদি (!queue.শীর্ষ) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
        যদি (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            reply_t *reply = REPLY_PTR(thread_state_ptr_get_replyObject(state));
            যদি (reply != ফাঁকা) {
                reply_unlink(reply, tptr);
            }
        }
#endif
        setThreadState(tptr, ThreadState_Inactive);
        বিরতি;
    }

    ক্ষেত্র ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        বিরতি;

    ক্ষেত্র ThreadState_BlockedOnReply: {
#ifdef CONFIG_KERNEL_MCS
        reply_remove_tcb(tptr);
#অন্যথা
        cte_t *slot, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        slot = TCB_PTR_CTE_PTR(tptr, tcbReply);

        callerCap = CTE_PTR(mdb_গ্রন্থি_get_mdb_পরবর্তী(slot->cteMDBNode));
        যদি (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#endif

        বিরতি;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
স্থির অন্তঃরেখ শূন্য restart_thread_if_no_fault(tcb_t *thread)
{
    যদি (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        setThreadState(thread, ThreadState_Restart);
        যদি (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            প্রতিজ্ঞা(thread->tcbSchedContext != গ্রন্থি_STATE(ksCurSC));
            যদি (thread->tcbSchedContext != গ্রন্থি_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possibleSwitchTo(thread);
    } অন্যথা {
        setThreadState(thread, ThreadState_Inactive);
    }
}

স্থির অন্তঃরেখ শূন্য removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    যদি (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(thread->tcbState));
        যদি (reply != ফাঁকা) {
            reply_unlink(reply, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#endif

শূন্য cancelAllIPC(endpoint_t *epptr)
{
    নির্বাচন (endpoint_ptr_get_state(epptr)) {
    ক্ষেত্র EPState_Idle:
        বিরতি;

    পূর্বনির্ধারিত: {
        /* Clear the queue and set all blocked threads to restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *পরবর্তী;

        queue = ep_ptr_get_queue(epptr);

        জন্য (thread = queue.শীর্ষ; thread; thread = পরবর্তী) {
            পরবর্তী = thread->tcb_Sched_পরবর্তী;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#অন্যথা
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_ep_Queue_শীর্ষ(epptr));

        /* Make endpoint idle */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_ep_Queue_শীর্ষ(epptr, 0);
        endpoint_ptr_set_ep_Queue_লেজ(epptr, 0);

        জন্য (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif

        rescheduleRequired();
        বিরতি;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
স্থির অন্তঃরেখ শূন্য removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, word_t badge)
{
    word_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    প্রতিজ্ঞা(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    যদি (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#endif

শূন্য cancelBadgedSends(endpoint_t *epptr, word_t badge)
{
    নির্বাচন (endpoint_ptr_get_state(epptr)) {
    ক্ষেত্র EPState_Idle:
    ক্ষেত্র EPState_Recv:
        বিরতি;

    ক্ষেত্র EPState_Send: {
        tcb_t *thread, *পরবর্তী;
        tcb_queue_t queue = ep_ptr_get_queue(epptr);

#ifdef CONFIG_KERNEL_MCS
        জন্য (thread = queue.শীর্ষ; thread; thread = পরবর্তী) {
            পরবর্তী = thread->tcb_Sched_পরবর্তী;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#অন্যথা
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_ep_Queue_শীর্ষ(epptr, 0);
        endpoint_ptr_set_ep_Queue_লেজ(epptr, 0);

        জন্য (thread = queue.শীর্ষ; thread; thread = পরবর্তী) {
            word_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            পরবর্তী = thread->tcbEPNext;

            যদি (b == badge) {
                setThreadState(thread, ThreadState_Restart);
                SCHED_ENQUEUE(thread);
                queue = tcbEPDequeue(thread, queue);
            }

        }
        ep_ptr_set_queue(epptr, queue);

        যদি (queue.শীর্ষ) {
            endpoint_ptr_set_state(epptr, EPState_Send);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        বিরতি;
    }

    পূর্বনির্ধারিত:
        fail("invalid EP state");
    }
}

#ifdef CONFIG_KERNEL_MCS
শূন্য tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
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

শূন্য tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcb_queue_remove(queue, thread);
    ep_ptr_set_queue(epptr, new_queue);

    যদি (tcb_queue_empty(new_queue)) {
        endpoint_ptr_set_state(epptr, EPState_Idle);
    }
}

শূন্য reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_queue_t queue = ep_ptr_get_queue(epptr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ep_ptr_set_queue(epptr, queue);
}
#endif
