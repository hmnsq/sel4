/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sel4/constants.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <machine/registerset.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <object/notification.h>
#അന്തര്ഭാവ <object/cnode.h>
#അന്തര്ഭാവ <object/endpoint.h>
#അന്തര്ഭാവ <object/tcb.h>

#ifdef CONFIG_KERNEL_MCS
ശൂന്യം sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#അല്ലെങ്കിൽ
ശൂന്യം sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t canGrantReply, tcb_t *thread, endpoint_t *epptr)
#endif
{
    തിരഞ്ഞെടുപ്പ് (endpoint_ptr_get_state(epptr)) {
    അവസ്ഥ EPState_Idle:
    അവസ്ഥ EPState_Send:
        എങ്കിൽ (blocking) {

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
#അല്ലെങ്കിൽ
            tcb_queue_t queue;
            queue = ep_ptr_get_queue(epptr);
            queue = tcbEPAppend(thread, queue);
            endpoint_ptr_set_state(epptr, EPState_Send);
            ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */

        }
        നിർത്തുക;

    അവസ്ഥ EPState_Recv: {
        tcb_queue_t queue;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        queue = ep_ptr_get_queue(epptr);
        dest = queue.head;

        /* Haskell error "Receive endpoint queue must not be empty" */
        പ്രതിജ്ഞാ(dest);

        /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#അല്ലെങ്കിൽ
        queue = tcbEPDequeue(dest, queue);
        ep_ptr_set_queue(epptr, queue);

        എങ്കിൽ (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#ifdef CONFIG_KERNEL_MCS
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(dest->tcbState));
        എങ്കിൽ (reply) {
            reply_unlink(reply, dest);
        }

        എങ്കിൽ (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            എങ്കിൽ (reply != ശൂന്യ && (canGrant || canGrantReply)) {
                reply_തള്ളുക(thread, dest, reply, canDonate);
            } അല്ലെങ്കിൽ {
                setThreadState(thread, ThreadState_Inactive);
            }
        } അല്ലെങ്കിൽ എങ്കിൽ (canDonate && dest->tcbSchedContext == ശൂന്യ) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        setThreadState(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        പ്രതിജ്ഞാ(dest_sc == ശൂന്യ || refill_sufficient(dest_sc, 0));
        പ്രതിജ്ഞാ(dest_sc == ശൂന്യ || refill_ready(dest_sc));
        എങ്കിൽ (sc_sporadic(dest_sc) && dest_sc != കണ്ണി_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possibleSwitchTo(dest);
#അല്ലെങ്കിൽ
        bool_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        setThreadState(dest, ThreadState_Running);
        possibleSwitchTo(dest);

        എങ്കിൽ (do_call) {
            എങ്കിൽ (canGrant || canGrantReply) {
                setupCallerCap(thread, dest, replyCanGrant);
            } അല്ലെങ്കിൽ {
                setThreadState(thread, ThreadState_Inactive);
            }
        }
#endif
        നിർത്തുക;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
ശൂന്യം receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t replyCap)
#അല്ലെങ്കിൽ
ശൂന്യം receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#endif
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    പ്രതിജ്ഞാ(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#ifdef CONFIG_KERNEL_MCS
    reply_t *replyPtr = ശൂന്യ;
    എങ്കിൽ (cap_get_capType(replyCap) == cap_reply_cap) {
        replyPtr = REPLY_PTR(cap_reply_cap_get_capReplyPtr(replyCap));
        tcb_t *reply_tcb = replyPtr->replyTCB;
        എങ്കിൽ (unlikely(reply_tcb != ശൂന്യ && reply_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(reply_tcb);
        }
    }
#endif

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    എങ്കിൽ (ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {
        completeSignal(ntfnPtr, thread);
    } അല്ലെങ്കിൽ {
#ifdef CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        എങ്കിൽ (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#endif
        തിരഞ്ഞെടുപ്പ് (endpoint_ptr_get_state(epptr)) {
        അവസ്ഥ EPState_Idle:
        അവസ്ഥ EPState_Recv: {

            എങ്കിൽ (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_set_tsType(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_set_blockingObject(
                    &thread->tcbState, EP_REF(epptr));
#ifdef CONFIG_KERNEL_MCS
                thread_state_ptr_set_replyObject(&thread->tcbState, REPLY_REF(replyPtr));
#അല്ലെങ്കിൽ
                thread_state_ptr_set_blockingIPCCanGrant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#endif
                scheduleTCB(thread);
#ifdef CONFIG_KERNEL_MCS
                എങ്കിൽ (replyPtr) {
                    replyPtr->replyTCB = thread;
                }
#endif

                /* Place calling thread in endpoint queue */
#ifdef CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#അല്ലെങ്കിൽ
                tcb_queue_t queue;
                queue = ep_ptr_get_queue(epptr);
                queue = tcbEPAppend(thread, queue);
                endpoint_ptr_set_state(epptr, EPState_Recv);
                ep_ptr_set_queue(epptr, queue);
#endif /* CONFIG_KERNEL_MCS */
            } അല്ലെങ്കിൽ {
                doNBRecvFailedTransfer(thread);
            }
            നിർത്തുക;
        }

        അവസ്ഥ EPState_Send: {
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
            പ്രതിജ്ഞാ(sender);

            /* Dequeue the first TCB */
#ifdef CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#അല്ലെങ്കിൽ
            queue = tcbEPDequeue(sender, queue);
            ep_ptr_set_queue(epptr, queue);

            എങ്കിൽ (!queue.head) {
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
            എങ്കിൽ (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                പ്രതിജ്ഞാ(sender->tcbSchedContext != കണ്ണി_STATE(ksCurSC));
                എങ്കിൽ (sender->tcbSchedContext != കണ്ണി_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            എങ്കിൽ (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                എങ്കിൽ ((canGrant || canGrantReply) && replyPtr != ശൂന്യ) {
                    bool_t canDonate = sender->tcbSchedContext != ശൂന്യ
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    reply_തള്ളുക(sender, thread, replyPtr, canDonate);
                } അല്ലെങ്കിൽ {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } അല്ലെങ്കിൽ {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
                പ്രതിജ്ഞാ(sender->tcbSchedContext == ശൂന്യ || refill_sufficient(sender->tcbSchedContext, 0));
            }
#അല്ലെങ്കിൽ
            എങ്കിൽ (do_call) {
                എങ്കിൽ (canGrant || canGrantReply) {
                    setupCallerCap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } അല്ലെങ്കിൽ {
                    setThreadState(sender, ThreadState_Inactive);
                }
            } അല്ലെങ്കിൽ {
                setThreadState(sender, ThreadState_Running);
                possibleSwitchTo(sender);
            }
#endif
            നിർത്തുക;
        }
        }
    }
}

ശൂന്യം replyFromKernel_error(tcb_t *thread)
{
    word_t len;
    word_t *ipcBuffer;

    ipcBuffer = lookupIPCBuffer(true, thread);
    setRegister(thread, badgeRegister, 0);
    len = setMRs_syscall_error(thread, ipcBuffer);

#ifdef CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    അക്ഷരം *debugBuffer = (അക്ഷരം *)(ipcBuffer + DEBUG_MESSAGE_START + 1);
    word_t add = strlcpy(debugBuffer, (അക്ഷരം *)current_debug_error.errorMessage,
                         DEBUG_MESSAGE_MAXLEN * വലുപ്പം(word_t));

    len += (add / വലുപ്പം(word_t)) + 1;
#endif

    setRegister(thread, സന്ദേശം_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(current_syscall_error.type, 0, 0, len)));
}

ശൂന്യം replyFromKernel_success_empty(tcb_t *thread)
{
    setRegister(thread, badgeRegister, 0);
    setRegister(thread, സന്ദേശം_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 0)));
}

ശൂന്യം cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#ifdef CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#endif

    തിരഞ്ഞെടുപ്പ് (thread_state_ptr_get_tsType(state)) {
    അവസ്ഥ ThreadState_BlockedOnSend:
    അവസ്ഥ ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        പ്രതിജ്ഞാ(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#ifdef CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#അല്ലെങ്കിൽ
        tcb_queue_t queue;
        queue = ep_ptr_get_queue(epptr);
        queue = tcbEPDequeue(tptr, queue);
        ep_ptr_set_queue(epptr, queue);

        എങ്കിൽ (!queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Idle);
        }
#endif /* CONFIG_KERNEL_MCS */

#ifdef CONFIG_KERNEL_MCS
        എങ്കിൽ (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            reply_t *reply = REPLY_PTR(thread_state_ptr_get_replyObject(state));
            എങ്കിൽ (reply != ശൂന്യ) {
                reply_unlink(reply, tptr);
            }
        }
#endif
        setThreadState(tptr, ThreadState_Inactive);
        നിർത്തുക;
    }

    അവസ്ഥ ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        നിർത്തുക;

    അവസ്ഥ ThreadState_BlockedOnReply: {
#ifdef CONFIG_KERNEL_MCS
        reply_remove_tcb(tptr);
#അല്ലെങ്കിൽ
        cte_t *slot, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        slot = TCB_PTR_CTE_PTR(tptr, tcbReply);

        callerCap = CTE_PTR(mdb_കണ്ണി_get_mdb_Next(slot->cteMDBNode));
        എങ്കിൽ (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#endif

        നിർത്തുക;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
സ്ഥിതം വരിയിൽ ശൂന്യം restart_thread_if_no_fault(tcb_t *thread)
{
    എങ്കിൽ (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        setThreadState(thread, ThreadState_Restart);
        എങ്കിൽ (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            പ്രതിജ്ഞാ(thread->tcbSchedContext != കണ്ണി_STATE(ksCurSC));
            എങ്കിൽ (thread->tcbSchedContext != കണ്ണി_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possibleSwitchTo(thread);
    } അല്ലെങ്കിൽ {
        setThreadState(thread, ThreadState_Inactive);
    }
}

സ്ഥിതം വരിയിൽ ശൂന്യം removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    എങ്കിൽ (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        reply_t *reply = REPLY_PTR(thread_state_get_replyObject(thread->tcbState));
        എങ്കിൽ (reply != ശൂന്യ) {
            reply_unlink(reply, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#endif

ശൂന്യം cancelAllIPC(endpoint_t *epptr)
{
    തിരഞ്ഞെടുപ്പ് (endpoint_ptr_get_state(epptr)) {
    അവസ്ഥ EPState_Idle:
        നിർത്തുക;

    സ്ഥിരസ്ഥിതി: {
        /* Clear the queue and set all blocked threads to restart */
#ifdef CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *next;

        queue = ep_ptr_get_queue(epptr);

        വേണ്ടി (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#അല്ലെങ്കിൽ
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_epQueue_head(epptr));

        /* Make endpoint idle */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        വേണ്ടി (; thread; thread = thread->tcbEPNext) {
            setThreadState(thread, ThreadState_Restart);
            SCHED_ENQUEUE(thread);
        }
#endif

        rescheduleRequired();
        നിർത്തുക;
    }
    }
}

#ifdef CONFIG_KERNEL_MCS
സ്ഥിതം വരിയിൽ ശൂന്യം removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, word_t badge)
{
    word_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    പ്രതിജ്ഞാ(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    എങ്കിൽ (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#endif

ശൂന്യം cancelBadgedSends(endpoint_t *epptr, word_t badge)
{
    തിരഞ്ഞെടുപ്പ് (endpoint_ptr_get_state(epptr)) {
    അവസ്ഥ EPState_Idle:
    അവസ്ഥ EPState_Recv:
        നിർത്തുക;

    അവസ്ഥ EPState_Send: {
        tcb_t *thread, *next;
        tcb_queue_t queue = ep_ptr_get_queue(epptr);

#ifdef CONFIG_KERNEL_MCS
        വേണ്ടി (thread = queue.head; thread; thread = next) {
            next = thread->tcbSchedNext;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#അല്ലെങ്കിൽ
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_set_state(epptr, EPState_Idle);
        endpoint_ptr_set_epQueue_head(epptr, 0);
        endpoint_ptr_set_epQueue_tail(epptr, 0);

        വേണ്ടി (thread = queue.head; thread; thread = next) {
            word_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            next = thread->tcbEPNext;

            എങ്കിൽ (b == badge) {
                setThreadState(thread, ThreadState_Restart);
                SCHED_ENQUEUE(thread);
                queue = tcbEPDequeue(thread, queue);
            }

        }
        ep_ptr_set_queue(epptr, queue);

        എങ്കിൽ (queue.head) {
            endpoint_ptr_set_state(epptr, EPState_Send);
        }
#endif /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        നിർത്തുക;
    }

    സ്ഥിരസ്ഥിതി:
        fail("invalid EP state");
    }
}

#ifdef CONFIG_KERNEL_MCS
ശൂന്യം tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
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

ശൂന്യം tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcb_queue_remove(queue, thread);
    ep_ptr_set_queue(epptr, new_queue);

    എങ്കിൽ (tcb_queue_empty(new_queue)) {
        endpoint_ptr_set_state(epptr, EPState_Idle);
    }
}

ശൂന്യം reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_queue_t queue = ep_ptr_get_queue(epptr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ep_ptr_set_queue(epptr, queue);
}
#endif
