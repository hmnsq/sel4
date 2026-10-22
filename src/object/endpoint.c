/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sel4/constants.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <machine/registerset.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <object/notification.h>
#అన్తర్భావ <object/cnode.h>
#అన్తర్భావ <object/endpoint.h>
#అన్తర్భావ <object/tcb.h>

#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య sendIPC(bool_t blocking, bool_t do_call, మాట_t badge,
             bool_t canGrant, bool_t can_Grant_జవాబు, bool_t canDonate, tcb_t *thread, endpoint_t *epptr)
#లేకపోతే
శూన్య sendIPC(bool_t blocking, bool_t do_call, మాట_t badge,
             bool_t canGrant, bool_t can_Grant_జవాబు, tcb_t *thread, endpoint_t *epptr)
#అయితే_ముగింపు
{
    ఎంపిక (endpoint_ptr_get_state(epptr)) {
    స్థితి EPState_Idle:
    స్థితి EPState_Send:
        ఒకవేళ (blocking) {

            /* Set thread state to BlockedOnSend */
            thread_state_ptr_సమితి_ts_Type(&thread->tcbState,
                                        ThreadState_BlockedOnSend);
            thread_state_ptr_సమితి_blocking_Object(
                &thread->tcbState, EP_REF(epptr));
            thread_state_ptr_సమితి_blocking_IPCBadge(
                &thread->tcbState, badge);
            thread_state_ptr_సమితి_blocking_IPCCan_Grant(
                &thread->tcbState, canGrant);
            thread_state_ptr_సమితి_blocking_IPCCan_Grant_జవాబు(
                &thread->tcbState, can_Grant_జవాబు);
            thread_state_ptr_సమితి_blocking_IPCIs_Call(
                &thread->tcbState, do_call);

            కాలపట్టిక_TCB(thread);

            /* Place calling thread in endpoint queue */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
            tcbEPAppend(thread, epptr, EPState_Send);
#లేకపోతే
            tcb_వరుస_t వరుస;
            వరుస = ep_ptr_get_వరుస(epptr);
            వరుస = tcbEPAppend(thread, వరుస);
            endpoint_ptr_సమితి_state(epptr, EPState_Send);
            ep_ptr_సమితి_వరుస(epptr, వరుస);
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */

        }
        ఆపు;

    స్థితి EPState_Recv: {
        tcb_వరుస_t వరుస;
        tcb_t *dest;

        /* Get the head of the endpoint queue. */
        వరుస = ep_ptr_get_వరుస(epptr);
        dest = వరుస.తల;

        /* Haskell error "Receive endpoint queue must not be empty" */
        ప్రతిజ్ఞా(dest);

        /* Dequeue the first TCB */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        tcbEPDequeue(dest, epptr);
#లేకపోతే
        వరుస = tcbEPDequeue(dest, వరుస);
        ep_ptr_సమితి_వరుస(epptr, వరుస);

        ఒకవేళ (!వరుస.తల) {
            endpoint_ptr_సమితి_state(epptr, EPState_Idle);
        }
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */

        /* Do the transfer */
        doIPCTransfer(thread, epptr, badge, canGrant, dest);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
        జవాబు_t *జవాబు = జవాబు_PTR(thread_state_get_జవాబు_Object(dest->tcbState));
        ఒకవేళ (జవాబు) {
            జవాబు_unlink(జవాబు, dest);
        }

        ఒకవేళ (do_call ||
            seL4_Fault_ptr_get_seL4_FaultType(&thread->tcbFault) != seL4_Fault_NullFault) {
            ఒకవేళ (జవాబు != ఖాళీ && (canGrant || can_Grant_జవాబు)) {
                జవాబు_నెట్టు(thread, dest, జవాబు, canDonate);
            } లేకపోతే {
                సమితి_Thread_State(thread, ThreadState_Inactive);
            }
        } లేకపోతే ఒకవేళ (canDonate && dest->tcbSchedContext == ఖాళీ) {
            schedContext_donate(thread->tcbSchedContext, dest);
        }

        సమితి_Thread_State(dest, ThreadState_Running);
        sched_context_t *dest_sc = dest->tcbSchedContext;
        /* blocked threads should have enough budget to get out of the kernel */
        ప్రతిజ్ఞా(dest_sc == ఖాళీ || refill_sufficient(dest_sc, 0));
        ప్రతిజ్ఞా(dest_sc == ఖాళీ || refill_ready(dest_sc));
        ఒకవేళ (sc_sporadic(dest_sc) && dest_sc != కణుపు_STATE(ksCurSC)) {
            refill_unblock_check(dest_sc);
        }
        possible_Switch_గమ్యం(dest);
#లేకపోతే
        bool_t జవాబు_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;

        సమితి_Thread_State(dest, ThreadState_Running);
        possible_Switch_గమ్యం(dest);

        ఒకవేళ (do_call) {
            ఒకవేళ (canGrant || can_Grant_జవాబు) {
                అమరిక_Caller_Cap(thread, dest, జవాబు_Can_Grant);
            } లేకపోతే {
                సమితి_Thread_State(thread, ThreadState_Inactive);
            }
        }
#అయితే_ముగింపు
        ఆపు;
    }
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking, cap_t జవాబు_Cap)
#లేకపోతే
శూన్య receiveIPC(tcb_t *thread, cap_t cap, bool_t isBlocking)
#అయితే_ముగింపు
{
    endpoint_t *epptr;
    notification_t *ntfnPtr;

    /* Haskell error "receiveIPC: invalid cap" */
    ప్రతిజ్ఞా(cap_get_capType(cap) == cap_endpoint_cap);

    epptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    జవాబు_t *జవాబు_Ptr = ఖాళీ;
    ఒకవేళ (cap_get_capType(జవాబు_Cap) == cap_జవాబు_cap) {
        జవాబు_Ptr = జవాబు_PTR(cap_జవాబు_cap_get_cap_జవాబు_Ptr(జవాబు_Cap));
        tcb_t *జవాబు_tcb = జవాబు_Ptr->జవాబు_TCB;
        ఒకవేళ (unlikely(జవాబు_tcb != ఖాళీ && జవాబు_tcb != thread)) {
            userError("Reply object already has unexecuted reply!");
            cancelIPC(జవాబు_tcb);
        }
    }
#అయితే_ముగింపు

    /* Check for anything waiting in the notification */
    ntfnPtr = thread->tcbBoundNotification;
    ఒకవేళ (ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_చురుకైన) {
        completeSignal(ntfnPtr, thread);
    } లేకపోతే {
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        /* If this is a blocking recv and we didn't have a pending notification,
         * then if we are running on an SC from a bound notification, then we
         * need to return it so that we can passively wait on the EP for potentially
         * SC donations from client threads.
         */
        ఒకవేళ (ntfnPtr && isBlocking) {
            maybeReturnSchedContext(ntfnPtr, thread);
        }
#అయితే_ముగింపు
        ఎంపిక (endpoint_ptr_get_state(epptr)) {
        స్థితి EPState_Idle:
        స్థితి EPState_Recv: {

            ఒకవేళ (isBlocking) {
                /* Set thread state to BlockedOnReceive */
                thread_state_ptr_సమితి_ts_Type(&thread->tcbState,
                                            ThreadState_BlockedOnReceive);
                thread_state_ptr_సమితి_blocking_Object(
                    &thread->tcbState, EP_REF(epptr));
#నిర్వచితమైతే CONFIG_KERNEL_MCS
                thread_state_ptr_సమితి_జవాబు_Object(&thread->tcbState, జవాబు_REF(జవాబు_Ptr));
#లేకపోతే
                thread_state_ptr_సమితి_blocking_IPCCan_Grant(
                    &thread->tcbState, cap_endpoint_cap_get_capCanGrant(cap));
#అయితే_ముగింపు
                కాలపట్టిక_TCB(thread);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
                ఒకవేళ (జవాబు_Ptr) {
                    జవాబు_Ptr->జవాబు_TCB = thread;
                }
#అయితే_ముగింపు

                /* Place calling thread in endpoint queue */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
                tcbEPAppend(thread, epptr, EPState_Recv);
#లేకపోతే
                tcb_వరుస_t వరుస;
                వరుస = ep_ptr_get_వరుస(epptr);
                వరుస = tcbEPAppend(thread, వరుస);
                endpoint_ptr_సమితి_state(epptr, EPState_Recv);
                ep_ptr_సమితి_వరుస(epptr, వరుస);
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */
            } లేకపోతే {
                doNBRecvFailedTransfer(thread);
            }
            ఆపు;
        }

        స్థితి EPState_Send: {
            tcb_వరుస_t వరుస;
            tcb_t *sender;
            మాట_t badge;
            bool_t canGrant;
            bool_t can_Grant_జవాబు;
            bool_t do_call;

            /* Get the head of the endpoint queue. */
            వరుస = ep_ptr_get_వరుస(epptr);
            sender = వరుస.తల;

            /* Haskell error "Send endpoint queue must not be empty" */
            ప్రతిజ్ఞా(sender);

            /* Dequeue the first TCB */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
            tcbEPDequeue(sender, epptr);
#లేకపోతే
            వరుస = tcbEPDequeue(sender, వరుస);
            ep_ptr_సమితి_వరుస(epptr, వరుస);

            ఒకవేళ (!వరుస.తల) {
                endpoint_ptr_సమితి_state(epptr, EPState_Idle);
            }
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */

            /* Get sender IPC details */
            badge = thread_state_ptr_get_blockingIPCBadge(&sender->tcbState);
            canGrant =
                thread_state_ptr_get_blockingIPCCanGrant(&sender->tcbState);
            can_Grant_జవాబు =
                thread_state_ptr_get_blocking_IPCCan_Grant_జవాబు(&sender->tcbState);

            /* Do the transfer */
            doIPCTransfer(sender, epptr, badge,
                          canGrant, thread);

            do_call = thread_state_ptr_get_blockingIPCIsCall(&sender->tcbState);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
            ఒకవేళ (sc_sporadic(sender->tcbSchedContext)) {
                /* We know that the sender can't have the current SC as
                 * its own SC as this point as it should still be
                 * associated with the current thread, no thread, or a
                 * thread that isn't blocked. This check is added here
                 * to reduce the cost of proving this to be true as a
                 * short-term stop-gap. */
                ప్రతిజ్ఞా(sender->tcbSchedContext != కణుపు_STATE(ksCurSC));
                ఒకవేళ (sender->tcbSchedContext != కణుపు_STATE(ksCurSC)) {
                    refill_unblock_check(sender->tcbSchedContext);
                }
            }

            ఒకవేళ (do_call ||
                seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_NullFault) {
                ఒకవేళ ((canGrant || can_Grant_జవాబు) && జవాబు_Ptr != ఖాళీ) {
                    bool_t canDonate = sender->tcbSchedContext != ఖాళీ
                                       && seL4_Fault_get_seL4_FaultType(sender->tcbFault) != seL4_Fault_Timeout;
                    జవాబు_నెట్టు(sender, thread, జవాబు_Ptr, canDonate);
                } లేకపోతే {
                    సమితి_Thread_State(sender, ThreadState_Inactive);
                }
            } లేకపోతే {
                సమితి_Thread_State(sender, ThreadState_Running);
                possible_Switch_గమ్యం(sender);
                ప్రతిజ్ఞా(sender->tcbSchedContext == ఖాళీ || refill_sufficient(sender->tcbSchedContext, 0));
            }
#లేకపోతే
            ఒకవేళ (do_call) {
                ఒకవేళ (canGrant || can_Grant_జవాబు) {
                    అమరిక_Caller_Cap(sender, thread, cap_endpoint_cap_get_capCanGrant(cap));
                } లేకపోతే {
                    సమితి_Thread_State(sender, ThreadState_Inactive);
                }
            } లేకపోతే {
                సమితి_Thread_State(sender, ThreadState_Running);
                possible_Switch_గమ్యం(sender);
            }
#అయితే_ముగింపు
            ఆపు;
        }
        }
    }
}

శూన్య జవాబు_From_Kernel_error(tcb_t *thread)
{
    మాట_t len;
    మాట_t *ipc_బఫరు;

    ipc_బఫరు = lookupIPCBuffer(true, thread);
    సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
    len = సమితి_MRs_syscall_error(thread, ipc_బఫరు);

#నిర్వచితమైతే CONFIG_KERNEL_INVOCATION_REPORT_ERROR_IPC
    అక్షరం *debug_బఫరు = (అక్షరం *)(ipc_బఫరు + DEBUG_సందేశం_ప్రారంభం + 1);
    మాట_t కూడు = strlcpy(debug_బఫరు, (అక్షరం *)current_debug_error.error_సందేశం,
                         DEBUG_సందేశం_MAXLEN * పరిమాణం(మాట_t));

    len += (కూడు / పరిమాణం(మాట_t)) + 1;
#అయితే_ముగింపు

    సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                    se_L4_సందేశం_Info_new(current_syscall_error.type, 0, 0, len)));
}

శూన్య జవాబు_From_Kernel_success_ఖాళీలు(tcb_t *thread)
{
    సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
    సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                    se_L4_సందేశం_Info_new(0, 0, 0, 0)));
}

శూన్య cancelIPC(tcb_t *tptr)
{
    thread_state_t *state = &tptr->tcbState;

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* cancel ipc cancels all faults */
    tptr->tcbFault = seL4_Fault_NullFault_new();
#అయితే_ముగింపు

    ఎంపిక (thread_state_ptr_get_tsType(state)) {
    స్థితి ThreadState_BlockedOnSend:
    స్థితి ThreadState_BlockedOnReceive: {
        /* blockedIPCCancel state */
        endpoint_t *epptr;

        epptr = EP_PTR(thread_state_ptr_get_blockingObject(state));

        /* Haskell error "blockedIPCCancel: endpoint must not be idle" */
        ప్రతిజ్ఞా(endpoint_ptr_get_state(epptr) != EPState_Idle);

        /* Dequeue TCB */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        tcbEPDequeue(tptr, epptr);
#లేకపోతే
        tcb_వరుస_t వరుస;
        వరుస = ep_ptr_get_వరుస(epptr);
        వరుస = tcbEPDequeue(tptr, వరుస);
        ep_ptr_సమితి_వరుస(epptr, వరుస);

        ఒకవేళ (!వరుస.తల) {
            endpoint_ptr_సమితి_state(epptr, EPState_Idle);
        }
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */

#నిర్వచితమైతే CONFIG_KERNEL_MCS
        ఒకవేళ (thread_state_ptr_get_tsType(state) == ThreadState_BlockedOnReceive) {
            జవాబు_t *జవాబు = జవాబు_PTR(thread_state_ptr_get_జవాబు_Object(state));
            ఒకవేళ (జవాబు != ఖాళీ) {
                జవాబు_unlink(జవాబు, tptr);
            }
        }
#అయితే_ముగింపు
        సమితి_Thread_State(tptr, ThreadState_Inactive);
        ఆపు;
    }

    స్థితి ThreadState_BlockedOnNotification:
        cancelSignal(tptr,
                     NTFN_PTR(thread_state_ptr_get_blockingObject(state)));
        ఆపు;

    స్థితి Thread_State_Blocked_On_జవాబు: {
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        జవాబు_remove_tcb(tptr);
#లేకపోతే
        cte_t *స్థానం, *callerCap;

        tptr->tcbFault = seL4_Fault_NullFault_new();

        /* Get the reply cap slot */
        స్థానం = TCB_PTR_CTE_PTR(tptr, tcb_జవాబు);

        callerCap = CTE_PTR(mdb_కణుపు_get_mdb_తదుపరి(స్థానం->cteMDBNode));
        ఒకవేళ (callerCap) {
            /** GHOSTUPD: "(True,
                gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
            cteDeleteOne(callerCap);
        }
#అయితే_ముగింపు

        ఆపు;
    }
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
స్థిర అంతఃవరుస శూన్య restart_thread_if_no_fault(tcb_t *thread)
{
    ఒకవేళ (seL4_Fault_get_seL4_FaultType(thread->tcbFault) == seL4_Fault_NullFault) {
        సమితి_Thread_State(thread, ThreadState_Restart);
        ఒకవేళ (sc_sporadic(thread->tcbSchedContext)) {
            /* We know that the thread can't have the current SC
             * as its own SC as this point as it should still be
             * associated with the current thread, or no thread.
             * This check is added here to reduce the cost of
             * proving this to be true as a short-term stop-gap. */
            ప్రతిజ్ఞా(thread->tcbSchedContext != కణుపు_STATE(ksCurSC));
            ఒకవేళ (thread->tcbSchedContext != కణుపు_STATE(ksCurSC)) {
                refill_unblock_check(thread->tcbSchedContext);
            }
        }
        possible_Switch_గమ్యం(thread);
    } లేకపోతే {
        సమితి_Thread_State(thread, ThreadState_Inactive);
    }
}

స్థిర అంతఃవరుస శూన్య removeAndRestartEPQueuedThread(tcb_t *thread, endpoint_t *epptr)
{
    tcbEPDequeue(thread, epptr);
    ఒకవేళ (thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnReceive) {
        జవాబు_t *జవాబు = జవాబు_PTR(thread_state_get_జవాబు_Object(thread->tcbState));
        ఒకవేళ (జవాబు != ఖాళీ) {
            జవాబు_unlink(జవాబు, thread);
        }
    }
    restart_thread_if_no_fault(thread);
}
#అయితే_ముగింపు

శూన్య cancelAllIPC(endpoint_t *epptr)
{
    ఎంపిక (endpoint_ptr_get_state(epptr)) {
    స్థితి EPState_Idle:
        ఆపు;

    అప్రమేయ: {
        /* Clear the queue and set all blocked threads to restart */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        tcb_వరుస_t వరుస;
        tcb_t *thread, *తదుపరి;

        వరుస = ep_ptr_get_వరుస(epptr);

        కొరకు (thread = వరుస.తల; thread; thread = తదుపరి) {
            తదుపరి = thread->tcb_Sched_తదుపరి;
            removeAndRestartEPQueuedThread(thread, epptr);
        }
#లేకపోతే
        tcb_t *thread = TCB_PTR(endpoint_ptr_get_ep_వరుస_తల(epptr));

        /* Make endpoint idle */
        endpoint_ptr_సమితి_state(epptr, EPState_Idle);
        endpoint_ptr_సమితి_ep_వరుస_తల(epptr, 0);
        endpoint_ptr_సమితి_ep_వరుస_తోక(epptr, 0);

        కొరకు (; thread; thread = thread->tcbEPNext) {
            సమితి_Thread_State(thread, ThreadState_Restart);
            SCHED_వరుసలో_చేర్చు(thread);
        }
#అయితే_ముగింపు

        rescheduleRequired();
        ఆపు;
    }
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
స్థిర అంతఃవరుస శూన్య removeAndRestartBadgedThread(tcb_t *thread, endpoint_t *epptr, మాట_t badge)
{
    మాట_t b = thread_state_ptr_get_blockingIPCBadge(&thread->tcbState);

    /* senders do not have reply objects in their state, and we are only cancelling sends */
    ప్రతిజ్ఞా(thread_state_get_tsType(thread->tcbState) == ThreadState_BlockedOnSend);
    ఒకవేళ (b == badge) {
        tcbEPDequeue(thread, epptr);
        restart_thread_if_no_fault(thread);
    }
}
#అయితే_ముగింపు

శూన్య cancelBadgedSends(endpoint_t *epptr, మాట_t badge)
{
    ఎంపిక (endpoint_ptr_get_state(epptr)) {
    స్థితి EPState_Idle:
    స్థితి EPState_Recv:
        ఆపు;

    స్థితి EPState_Send: {
        tcb_t *thread, *తదుపరి;
        tcb_వరుస_t వరుస = ep_ptr_get_వరుస(epptr);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
        కొరకు (thread = వరుస.తల; thread; thread = తదుపరి) {
            తదుపరి = thread->tcb_Sched_తదుపరి;
            removeAndRestartBadgedThread(thread, epptr, badge);
        }
#లేకపోతే
        /* this is a de-optimisation for verification
         * reasons. it allows the contents of the endpoint
         * queue to be ignored during the for loop. */
        endpoint_ptr_సమితి_state(epptr, EPState_Idle);
        endpoint_ptr_సమితి_ep_వరుస_తల(epptr, 0);
        endpoint_ptr_సమితి_ep_వరుస_తోక(epptr, 0);

        కొరకు (thread = వరుస.తల; thread; thread = తదుపరి) {
            మాట_t b = thread_state_ptr_get_blockingIPCBadge(
                           &thread->tcbState);
            తదుపరి = thread->tcbEPNext;

            ఒకవేళ (b == badge) {
                సమితి_Thread_State(thread, ThreadState_Restart);
                SCHED_వరుసలో_చేర్చు(thread);
                వరుస = tcbEPDequeue(thread, వరుస);
            }

        }
        ep_ptr_సమితి_వరుస(epptr, వరుస);

        ఒకవేళ (వరుస.తల) {
            endpoint_ptr_సమితి_state(epptr, EPState_Send);
        }
#అయితే_ముగింపు /* CONFIG_KERNEL_MCS */
        rescheduleRequired();

        ఆపు;
    }

    అప్రమేయ:
        fail("invalid EP state");
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య tcbEPAppend(tcb_t *thread, endpoint_t *epptr, endpoint_state_t ep_state)
{
    tcb_వరుస_t వరుస;
    tcb_వరుస_t new_వరుస;

    వరుస = ep_ptr_get_వరుస(epptr);
    new_వరుస = tcbAppend(thread, వరుస);
    ep_ptr_సమితి_వరుస(epptr, new_వరుస);

    /* Update the state of the endpoint with the state that was passed in. If the queue
     * was previously non-empty this must be the same state the endpoint is currently in. */
    endpoint_ptr_సమితి_state(epptr, ep_state);
}

శూన్య tcbEPDequeue(tcb_t *thread, endpoint_t *epptr)
{
    tcb_వరుస_t వరుస;
    tcb_వరుస_t new_వరుస;

    వరుస = ep_ptr_get_వరుస(epptr);
    new_వరుస = tcb_వరుస_remove(వరుస, thread);
    ep_ptr_సమితి_వరుస(epptr, new_వరుస);

    ఒకవేళ (tcb_వరుస_ఖాళీలు(new_వరుస)) {
        endpoint_ptr_సమితి_state(epptr, EPState_Idle);
    }
}

శూన్య reorderEP(endpoint_t *epptr, tcb_t *thread)
{
    tcb_వరుస_t వరుస = ep_ptr_get_వరుస(epptr);
    వరుస = tcb_వరుస_remove(వరుస, thread);
    వరుస = tcbAppend(thread, వరుస);
    ep_ptr_సమితి_వరుస(epptr, వరుస);
}
#అయితే_ముగింపు
