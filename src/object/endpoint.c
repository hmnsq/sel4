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

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t can_Grant_प्रत्युत्तरम्, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#अन्यथा
शून्य sendIPC(bool_t blocking, bool_t do_call, word_t badge,
             bool_t canGrant, bool_t can_Grant_प्रत्युत्तरम्, tcb_t *thread, endpoint_t *epptr)
#यदि_अन्तः
{
    विकल्प (endpoint_ptr_get_state(epptr)) {
    पक्ष EPState_Idle:
    पक्ष EPState_Send:
        यदि (blocking) {

            /* Set thread state to BlockedOnSend */
            thread_state_ptr_समुच्चयः_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnSend);
            thread_state_ptr_समुच्चयः_blocking_Object(
                &thread->tcbState, EP_REF(epptr));
            thread_state_ptr_समुच्चयः_blocking_IPCBadge(
                &thread->tcbState, badge);
            thread_state_ptr_समुच्चयः_blocking_IPCCan_Grant(
                &thread->tcbState, canGrant);
            thread_state_ptr_समुच्चयः_blocking_IPCCan_Grant_प्रत्युत्तरम्(
                &thread->tcbState, can_Grant_प्रत्युत्तरम्);
            thread_state_ptr_समुच्चयः_blocking_IPCIs_Call(
                &thread->tcbState, do_call);

            कालसारणी_TCB(thread);

            /* Place calling thread in endpoint queue */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
            tcbEPAppend(thread, epptr, EPState_Send);
#अन्यथा
            tcb_queue_t queue;
            queue = ep_ptr_get_queue(epptr);
            queue = tcbEPAppend(thread, queue);
            endpoint_ptr_समुच्चयः_state(epptr, EPState_Send);
            ep_ptr_समुच्चयः_queue(epptr, queue);
#यदि_अन्तः /* CONFIG_KERNEL_MCS */

        }
        भङ्ग;

    पक्ष EPState_Recv: {
        tcb_queue_t queue;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        queue = ep_ptr_get_queue(epptr);
        dest = queue.शीर्ष;

        /* Haskell error "Receive endpoint queue must not be empty" */
        प्रतिज्ञा(dest);

        /* Dequeue the first TCB */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#अन्यथा
        queue = tcbEPDequeue(dest, queue);
        ep_ptr_समुच्चयः_queue(epptr, queue);

        यदि (!queue.शीर्ष) {
            endpoint_ptr_समुच्चयः_state(epptr, EPState_Idle);
        }
#यदि_अन्तः /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        प्रत्युत्तरम्_t *प्रत्युत्तरम् = प्रत्युत्तरम्_PTR(thread_state_get_प्रत्युत्तरम्_Object(dest->tcbState));
        यदि (प्रत्युत्तरम्) {
            प्रत्युत्तरम्_unlink(प्रत्युत्तरम्, dest);
        }

        यदि (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            यदि (प्रत्युत्तरम् != रिक्त && (canGrant || can_Grant_प्रत्युत्तरम्)) {
                प्रत्युत्तरम्_प्रक्षिप(thread, dest, प्रत्युत्तरम्, canDonate);
            } अन्यथा {
                समुच्चयः_Thread_State(thread, ThreadState_Inactive);
            }
        } अन्यथा यदि (canDonate && dest->tcbSchedContext == रिक्त) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        समुच्चयः_Thread_State(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        प्रतिज्ञा(dest_sc == रिक्त || refill_sufficient(dest_sc, 0));
        प्रतिज्ञा(dest_sc == रिक्त || refill_ready(dest_sc));
        यदि (sc_sporadic(dest_sc) && dest_sc != ग्रन्थिः_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possible_Switch_गन्तव्यम्(dest);
#अन्यथा
        bool_t प्रत्युत्तरम्_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        समुच्चयः_Thread_State(dest, ThreadState_Running);
        possible_Switch_गन्तव्यम्(dest);

        यदि (do_call) {
            यदि (canGrant || can_Grant_प्रत्युत्तरम्) {
                सज्जता_Caller_Cap(thread, dest, प्रत्युत्तरम्_Can_Grant);
            } अन्यथा {
                समुच्चयः_Thread_State(thread, ThreadState_Inactive);
            }
        }
#यदि_अन्तः
        भङ्ग;
    }
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t प्रत्युत्तरम्_Cap)
#अन्यथा
शून्य receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#यदि_अन्तः
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    प्रतिज्ञा(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रत्युत्तरम्_t *प्रत्युत्तरम्_Ptr = रिक्त;
    यदि (cap_get_capType(प्रत्युत्तरम्_Cap) == cap_प्रत्युत्तरम्_cap) {
        प्रत्युत्तरम्_Ptr = प्रत्युत्तरम्_PTR(cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Ptr(प्रत्युत्तरम्_Cap));
        tcb_t *प्रत्युत्तरम्_tcb = प्रत्युत्तरम्_Ptr->प्रत्युत्तरम्_TCB;
        यदि (unlikely(प्रत्युत्तरम्_tcb != रिक्त && प्रत्युत्तरम्_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(प्रत्युत्तरम्_tcb);
        }
    }
#यदि_अन्तः

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    यदि (ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_सक्रिय) {
        completeSignal(ntfnPtr, thread);
    } अन्यथा {
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        यदि (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#यदि_अन्तः
        विकल्प (endpoint_ptr_get_state(epptr)) {
        पक्ष EPState_Idle:
        पक्ष EPState_Recv: {

            यदि (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_समुच्चयः_ts_Type(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_समुच्चयः_blocking_Object(
                    &thread->tcbState, EP_REF(epptr));
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
                thread_state_ptr_समुच्चयः_प्रत्युत्तरम्_Object(&thread->tcbState, प्रत्युत्तरम्_REF(प्रत्युत्तरम्_Ptr));
#अन्यथा
                thread_state_ptr_समुच्चयः_blocking_IPCCan_Grant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#यदि_अन्तः
                कालसारणी_TCB(thread);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
                यदि (प्रत्युत्तरम्_Ptr) {
                    प्रत्युत्तरम्_Ptr->प्रत्युत्तरम्_TCB = thread;
                }
#यदि_अन्तः

                /* Place calling thread in endpoint queue */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#अन्यथा
                tcb_queue_t queue;
                queue = ep_ptr_get_queue(epptr);
                queue = tcbEPAppend(thread, queue);
                endpoint_ptr_समुच्चयः_state(epptr, EPState_Recv);
                ep_ptr_समुच्चयः_queue(epptr, queue);
#यदि_अन्तः /* CONFIG_KERNEL_MCS */
            } अन्यथा {
                doNBRecvFailedTransfer(thread);
            }
            भङ्ग;
        }

        पक्ष EPState_Send: {
            tcb_queue_t queue;
            tcb_t *sender;
            word_t badge;
            bool_t canGrant;
            bool_t can_Grant_प्रत्युत्तरम्;
            bool_t do_call;

            /* Get the head of the endpoint queue. */
            queue = ep_ptr_get_queue(epptr);
            sender = queue.शीर्ष;

            /* Haskell error "Send endpoint queue must not be empty" */
            प्रतिज्ञा(sender);

            /* Dequeue the first TCB */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#अन्यथा
            queue = tcbEPDequeue(sender, queue);
            ep_ptr_समुच्चयः_queue(epptr, queue);

            यदि (!queue.शीर्ष) {
                endpoint_ptr_समुच्चयः_state(epptr, EPState_Idle);
            }
#यदि_अन्तः /* CONFIG_KERNEL_MCS */

            /* Get sender IPC details */
            badge = thread_state_ptr_get_blockingIPCBadge(&sender->tcbState);
            canGrant =
                thread_state_ptr_get_blockingIPCCanGrant(&sender->tcbState);
            can_Grant_प्रत्युत्तरम् =
                thread_state_ptr_get_blocking_IPCCan_Grant_प्रत्युत्तरम्(&sender->tcbState);

            /* Do the transfer */
            doIPCTransfer(sender, epptr, badge,
                          canGrant, thread);

            do_call = thread_state_ptr_get_blockingIPCIsCall(&sender->tcbState);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
            यदि (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                प्रतिज्ञा(sender->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
                यदि (sender->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            यदि (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                यदि ((canGrant || can_Grant_प्रत्युत्तरम्) && प्रत्युत्तरम्_Ptr != रिक्त) {
                    bool_t canDonate = sender->tcbSchedContext != रिक्त
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    प्रत्युत्तरम्_प्रक्षिप(sender, thread, प्रत्युत्तरम्_Ptr, canDonate);
                } अन्यथा {
                    समुच्चयः_Thread_State(sender, ThreadState_Inactive);
                }
            } अन्यथा {
                समुच्चयः_Thread_State(sender, ThreadState_Running);
                possible_Switch_गन्तव्यम्(sender);
                प्रतिज्ञा(sender->tcbSchedContext == रिक्त || refill_sufficient(sender->tcbSchedContext, 0));
            }
#अन्यथा
            यदि (do_call) {
                यदि (canGrant || can_Grant_प्रत्युत्तरम्) {
                    सज्जता_Caller_Cap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } अन्यथा {
                    समुच्चयः_Thread_State(sender, ThreadState_Inactive);
                }
            } अन्यथा {
                समुच्चयः_Thread_State(sender, ThreadState_Running);
                possible_Switch_गन्तव्यम्(sender);
            }
#यदि_अन्तः
            भङ्ग;
        }
        }
    }
}

शून्य प्रत्युत्तरम्_From_Kernel_error(tcb_t *thread)
{
    word_t len;
    word_t *ipc_बफर;

    ipc_बफर = lookupIPCBuffer(true, thread);
    समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
    len = समुच्चयः_MRs_syscall_error(thread, ipc_बफर);

#यदि_परिभाषितम् CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    वर्ण *debug_बफर = (वर्ण *)(ipc_बफर + DEBUG_सन्देशः_START + 1);
    word_t सङ्कलय = strlcpy(debug_बफर, (वर्ण *)current_debug_error.error_सन्देशः,
                         DEBUG_सन्देशः_MAXLEN * परिमाण(word_t));

    len += (सङ्कलय / परिमाण(word_t)) + 1;
#यदि_अन्तः

    समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                    se_L4_सन्देशः_Info_new(current_syscall_error.type, 0, 0, len)));
}

शून्य प्रत्युत्तरम्_From_Kernel_success_रिक्तानि(tcb_t *thread)
{
    समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
    समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                    se_L4_सन्देशः_Info_new(0, 0, 0, 0)));
}

शून्य cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#यदि_अन्तः

    विकल्प (thread_state_ptr_get_tsType(state)) {
    पक्ष ThreadState_BlockedOnSend:
    पक्ष ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        प्रतिज्ञा(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#अन्यथा
        tcb_queue_t queue;
        queue = ep_ptr_get_queue(epptr);
        queue = tcbEPDequeue(tptr, queue);
        ep_ptr_समुच्चयः_queue(epptr, queue);

        यदि (!queue.शीर्ष) {
            endpoint_ptr_समुच्चयः_state(epptr, EPState_Idle);
        }
#यदि_अन्तः /* CONFIG_KERNEL_MCS */

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        यदि (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            प्रत्युत्तरम्_t *प्रत्युत्तरम् = प्रत्युत्तरम्_PTR(thread_state_ptr_get_प्रत्युत्तरम्_Object(state));
            यदि (प्रत्युत्तरम् != रिक्त) {
                प्रत्युत्तरम्_unlink(प्रत्युत्तरम्, tptr);
            }
        }
#यदि_अन्तः
        समुच्चयः_Thread_State(tptr, ThreadState_Inactive);
        भङ्ग;
    }

    पक्ष ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        भङ्ग;

    पक्ष Thread_State_Blocked_On_प्रत्युत्तरम्: {
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        प्रत्युत्तरम्_remove_tcb(tptr);
#अन्यथा
        cte_t *slot, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        slot = TCB_PTR_CTE_PTR(tptr, tcb_प्रत्युत्तरम्);

        callerCap = CTE_PTR(mdb_ग्रन्थिः_get_mdb_अग्र(slot->cteMDBNode));
        यदि (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#यदि_अन्तः

        भङ्ग;
    }
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ शून्य restart_thread_if_no_fault(tcb_t *thread)
{
    यदि (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        समुच्चयः_Thread_State(thread, ThreadState_Restart);
        यदि (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            प्रतिज्ञा(thread->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
            यदि (thread->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possible_Switch_गन्तव्यम्(thread);
    } अन्यथा {
        समुच्चयः_Thread_State(thread, ThreadState_Inactive);
    }
}

स्थिर अन्तःस्थ शून्य removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    यदि (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        प्रत्युत्तरम्_t *प्रत्युत्तरम् = प्रत्युत्तरम्_PTR(thread_state_get_प्रत्युत्तरम्_Object(thread->tcbState));
        यदि (प्रत्युत्तरम् != रिक्त) {
            प्रत्युत्तरम्_unlink(प्रत्युत्तरम्, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#यदि_अन्तः

शून्य cancelAllIPC(endpoint_t *epptr)
{
    विकल्प (endpoint_ptr_get_state(epptr)) {
    पक्ष EPState_Idle:
        भङ्ग;

    स्वभाव: {
        /* Clear the queue and set all blocked threads to restart */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        tcb_queue_t queue;
        tcb_t *thread, *अग्र;

        queue = ep_ptr_get_queue(epptr);

        प्रति (thread = queue.शीर्ष; thread; thread = अग्र) {
            अग्र = thread->tcb_Sched_अग्र;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#अन्यथा
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_ep_Queue_शीर्ष(epptr));

        /* Make endpoint idle */
        endpoint_ptr_समुच्चयः_state(epptr, EPState_Idle);
        endpoint_ptr_समुच्चयः_ep_Queue_शीर्ष(epptr, 0);
        endpoint_ptr_समुच्चयः_ep_Queue_पुच्छ(epptr, 0);

        प्रति (; thread; thread = thread->tcbEPNext) {
            समुच्चयः_Thread_State(thread, ThreadState_Restart);
            SCHED_पङ्क्तौ_निक्षिप(thread);
        }
#यदि_अन्तः

        rescheduleRequired();
        भङ्ग;
    }
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
स्थिर अन्तःस्थ शून्य removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, word_t badge)
{
    word_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    प्रतिज्ञा(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    यदि (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#यदि_अन्तः

शून्य cancelBadgedSends(endpoint_t *epptr, word_t badge)
{
    विकल्प (endpoint_ptr_get_state(epptr)) {
    पक्ष EPState_Idle:
    पक्ष EPState_Recv:
        भङ्ग;

    पक्ष EPState_Send: {
        tcb_t *thread, *अग्र;
        tcb_queue_t queue = ep_ptr_get_queue(epptr);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        प्रति (thread = queue.शीर्ष; thread; thread = अग्र) {
            अग्र = thread->tcb_Sched_अग्र;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#अन्यथा
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_समुच्चयः_state(epptr, EPState_Idle);
        endpoint_ptr_समुच्चयः_ep_Queue_शीर्ष(epptr, 0);
        endpoint_ptr_समुच्चयः_ep_Queue_पुच्छ(epptr, 0);

        प्रति (thread = queue.शीर्ष; thread; thread = अग्र) {
            word_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            अग्र = thread->tcbEPNext;

            यदि (b == badge) {
                समुच्चयः_Thread_State(thread, ThreadState_Restart);
                SCHED_पङ्क्तौ_निक्षिप(thread);
                queue = tcbEPDequeue(thread, queue);
            }

        }
        ep_ptr_समुच्चयः_queue(epptr, queue);

        यदि (queue.शीर्ष) {
            endpoint_ptr_समुच्चयः_state(epptr, EPState_Send);
        }
#यदि_अन्तः /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        भङ्ग;
    }

    स्वभाव:
        fail("invalid EP state");
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcbAppend(thread, queue);
    ep_ptr_समुच्चयः_queue(epptr, new_queue);

    /* Update the state of the endpoint with the state that was passed in. If the queue
     * was previously non-empty this must be the same state the endpoint is currently in. */
    endpoint_ptr_समुच्चयः_state(epptr, ep_state);
}

शून्य tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_queue_t queue;
    tcb_queue_t new_queue;

    queue = ep_ptr_get_queue(epptr);
    new_queue = tcb_queue_remove(queue, thread);
    ep_ptr_समुच्चयः_queue(epptr, new_queue);

    यदि (tcb_queue_रिक्तानि(new_queue)) {
        endpoint_ptr_समुच्चयः_state(epptr, EPState_Idle);
    }
}

शून्य reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_queue_t queue = ep_ptr_get_queue(epptr);
    queue = tcb_queue_remove(queue, thread);
    queue = tcbAppend(thread, queue);
    ep_ptr_समुच्चयः_queue(epptr, queue);
}
#यदि_अन्तः
