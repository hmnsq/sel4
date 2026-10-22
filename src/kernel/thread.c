/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/faults.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/domain.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਅਨ੍ਤਰ੍ਭਾਵ <object/schedcontext.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>

ਸਥਿਰ se_L4_ਸੁਨੇਹਾ_Info_t
transferCaps(se_L4_ਸੁਨੇਹਾ_Info_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             ਸ਼ਬਦ_t *receive_ਬਫਰ);

BOOT_ਕੋਡ ਖਾਲੀ configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    ਸੈੱਟ_Thread_State(tcb, ThreadState_IdleThreadState);
}

ਖਾਲੀ activateThread(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਜੇ (unlikely(ਗੰਢ_STATE(ksCurThread)->tcb_Yield_ਮੰਜ਼ਿਲ)) {
        sched_Context_complete_Yield_ਮੰਜ਼ਿਲ(ਗੰਢ_STATE(ksCurThread));
        ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tsType(ਗੰਢ_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#ਜੇ_ਅੰਤ

    ਚੋਣ (thread_state_get_tsType(ਗੰਢ_STATE(ksCurThread)->tcbState)) {
    ਹਾਲਤ ThreadState_Running:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਹਾਲਤ ThreadState_RunningVM:
#ਜੇ_ਅੰਤ
        ਰੋਕੋ;

    ਹਾਲਤ ThreadState_Restart: {
        ਸ਼ਬਦ_t pc;

        pc = getRestartPC(ਗੰਢ_STATE(ksCurThread));
        ਸੈੱਟ_ਅਗਲਾ_PC(ਗੰਢ_STATE(ksCurThread), pc);
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
        ਰੋਕੋ;
    }

    ਹਾਲਤ ThreadState_IdleThreadState:
        Arch_activateIdleThread(ਗੰਢ_STATE(ksCurThread));
        ਰੋਕੋ;

    ਮੂਲ:
        fail("Current thread is blocked");
    }
}

ਖਾਲੀ suspend(tcb_t *ਨਿਸ਼ਾਨਾ)
{
    cancelIPC(ਨਿਸ਼ਾਨਾ);
    ਜੇ (thread_state_get_tsType(ਨਿਸ਼ਾਨਾ->tcbState) == ThreadState_Running) {
        /* whilst in the running state it is possible that restart pc of a thread is
         * incorrect. As we do not know what state this thread will transition to
         * after we make it inactive we update its restart pc so that the thread next
         * runs at the correct address whether it is restarted or moved directly to
         * running */
        updateRestartPC(ਨਿਸ਼ਾਨਾ);
    }
    tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(ਨਿਸ਼ਾਨਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    tcbReleaseRemove(ਨਿਸ਼ਾਨਾ);
    sched_Context_cancel_Yield_ਮੰਜ਼ਿਲ(ਨਿਸ਼ਾਨਾ);
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_Thread_State(ਨਿਸ਼ਾਨਾ, ThreadState_Inactive);
}

ਖਾਲੀ restart(tcb_t *ਨਿਸ਼ਾਨਾ)
{
    ਜੇ (isStopped(ਨਿਸ਼ਾਨਾ)) {
        cancelIPC(ਨਿਸ਼ਾਨਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਸੈੱਟ_Thread_State(ਨਿਸ਼ਾਨਾ, ThreadState_Restart);
        sched_context_t *sc = ਨਿਸ਼ਾਨਾ->tcbSchedContext;
        ਜੇ (sc_sporadic(sc) && sc != ਗੰਢ_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        ਜੇ (isSchedulable(ਨਿਸ਼ਾਨਾ)) {
            possible_Switch_ਮੰਜ਼ਿਲ(ਨਿਸ਼ਾਨਾ);
        }
#ਨਹੀਂਤਾਂ
        ਤਿਆਰੀ_ਜਵਾਬ_Master(ਨਿਸ਼ਾਨਾ);
        ਸੈੱਟ_Thread_State(ਨਿਸ਼ਾਨਾ, ThreadState_Restart);
        SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ਨਿਸ਼ਾਨਾ);
        possible_Switch_ਮੰਜ਼ਿਲ(ਨਿਸ਼ਾਨਾ);
#ਜੇ_ਅੰਤ
    }
}

ਖਾਲੀ doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, ਸ਼ਬਦ_t badge,
                   bool_t grant, tcb_t *receiver)
{
    ਖਾਲੀ *receive_ਬਫਰ, *send_ਬਫਰ;

    receive_ਬਫਰ = lookupIPCBuffer(true, receiver);

    ਜੇ (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        send_ਬਫਰ = lookupIPCBuffer(false, sender);
        do_ਸਾਧਾਰਨ_Transfer(sender, send_ਬਫਰ, endpoint, badge, grant,
                         receiver, receive_ਬਫਰ);
    } ਨਹੀਂਤਾਂ {
        doFaultTransfer(badge, sender, receiver, receive_ਬਫਰ);
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ do_ਜਵਾਬ_Transfer(tcb_t *sender, ਜਵਾਬ_t *ਜਵਾਬ, bool_t grant)
#ਨਹੀਂਤਾਂ
ਖਾਲੀ do_ਜਵਾਬ_Transfer(tcb_t *sender, tcb_t *receiver, cte_t *ਥਾਂ, bool_t grant)
#ਜੇ_ਅੰਤ
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਜੇ (ਜਵਾਬ->ਜਵਾਬ_TCB == ਨਿਰਾ ||
        thread_state_get_tsType(ਜਵਾਬ->ਜਵਾਬ_TCB->tcbState) != Thread_State_Blocked_On_ਜਵਾਬ) {
        /* nothing to do */
        ਵਾਪਸ;
    }

    tcb_t *receiver = ਜਵਾਬ->ਜਵਾਬ_TCB;
    ਜਵਾਬ_remove(ਜਵਾਬ, receiver);
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਜਵਾਬ->ਜਵਾਬ_TCB == ਨਿਰਾ);

    ਜੇ (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != ਗੰਢ_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#ਨਹੀਂਤਾਂ
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tsType(receiver->tcbState) ==
           Thread_State_Blocked_On_ਜਵਾਬ);
#ਜੇ_ਅੰਤ

    ਸ਼ਬਦ_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    ਜੇ (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, ਨਿਰਾ, 0, grant, receiver);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਸੈੱਟ_Thread_State(receiver, ThreadState_Running);
#ਨਹੀਂਤਾਂ
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(ਥਾਂ);
        ਸੈੱਟ_Thread_State(receiver, ThreadState_Running);
        possible_Switch_ਮੰਜ਼ਿਲ(receiver);
#ਜੇ_ਅੰਤ
    } ਨਹੀਂਤਾਂ {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(ਥਾਂ);
#ਜੇ_ਅੰਤ
        bool_t restart = handle_Fault_ਜਵਾਬ(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        ਜੇ (restart) {
            ਸੈੱਟ_Thread_State(receiver, ThreadState_Restart);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
            possible_Switch_ਮੰਜ਼ਿਲ(receiver);
#ਜੇ_ਅੰਤ
        } ਨਹੀਂਤਾਂ {
            ਸੈੱਟ_Thread_State(receiver, ThreadState_Inactive);
        }
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਜੇ (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        ਜੇ ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possible_Switch_ਮੰਜ਼ਿਲ(receiver);
        } ਨਹੀਂਤਾਂ {
            ਜੇ (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } ਨਹੀਂਤਾਂ {
                postpone(sc);
            }
        }
    }
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ do_ਸਾਧਾਰਨ_Transfer(tcb_t *sender, ਸ਼ਬਦ_t *send_ਬਫਰ, endpoint_t *endpoint,
                      ਸ਼ਬਦ_t badge, bool_t canGrant, tcb_t *receiver,
                      ਸ਼ਬਦ_t *receive_ਬਫਰ)
{
    ਸ਼ਬਦ_t ਸੁਨੇਹਾ_Transferred;
    se_L4_ਸੁਨੇਹਾ_Info_t ਟੈਗ;
    exception_t ਸਥਿਤੀ;

    ਟੈਗ = ਸੁਨੇਹਾ_Info_From_ਸ਼ਬਦ(get_ਰਜਿਸਟਰ(sender, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ));

    ਜੇ (canGrant) {
        ਸਥਿਤੀ = lookupExtraCaps(sender, send_ਬਫਰ, ਟੈਗ);
        ਜੇ (unlikely(ਸਥਿਤੀ != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = ਨਿਰਾ;
        }
    } ਨਹੀਂਤਾਂ {
        current_extra_caps.excaprefs[0] = ਨਿਰਾ;
    }

    ਸੁਨੇਹਾ_Transferred = copyMRs(sender, send_ਬਫਰ, receiver, receive_ਬਫਰ,
                             se_L4_ਸੁਨੇਹਾ_Info_get_ਲੰਮਾਈ(ਟੈਗ));

    ਟੈਗ = transferCaps(ਟੈਗ, endpoint, receiver, receive_ਬਫਰ);

    ਟੈਗ = se_L4_ਸੁਨੇਹਾ_Info_ਸੈੱਟ_ਲੰਮਾਈ(ਟੈਗ, ਸੁਨੇਹਾ_Transferred);
    ਸੈੱਟ_ਰਜਿਸਟਰ(receiver, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(ਟੈਗ));
    ਸੈੱਟ_ਰਜਿਸਟਰ(receiver, badge_ਰਜਿਸਟਰ, badge);
}

ਖਾਲੀ doFaultTransfer(ਸ਼ਬਦ_t badge, tcb_t *sender, tcb_t *receiver,
                     ਸ਼ਬਦ_t *receiverIPCBuffer)
{
    ਸ਼ਬਦ_t ਭੇਜਿਆ;
    se_L4_ਸੁਨੇਹਾ_Info_t ਸੁਨੇਹਾ_Info;

    ਭੇਜਿਆ = ਸੈੱਟ_MRs_fault(sender, receiver, receiverIPCBuffer);
    ਸੁਨੇਹਾ_Info = se_L4_ਸੁਨੇਹਾ_Info_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, ਭੇਜਿਆ);
    ਸੈੱਟ_ਰਜਿਸਟਰ(receiver, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(ਸੁਨੇਹਾ_Info));
    ਸੈੱਟ_ਰਜਿਸਟਰ(receiver, badge_ਰਜਿਸਟਰ, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ਸਥਿਰ se_L4_ਸੁਨੇਹਾ_Info_t transferCaps(se_L4_ਸੁਨੇਹਾ_Info_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       ਸ਼ਬਦ_t *receive_ਬਫਰ)
{
    ਸ਼ਬਦ_t i;
    cte_t *dest_ਥਾਂ;

    info = se_L4_ਸੁਨੇਹਾ_Info_ਸੈੱਟ_extra_Caps(info, 0);
    info = se_L4_ਸੁਨੇਹਾ_Info_ਸੈੱਟ_caps_Unwrapped(info, 0);

    ਜੇ (likely(!current_extra_caps.excaprefs[0] || !receive_ਬਫਰ)) {
        ਵਾਪਸ info;
    }

    dest_ਥਾਂ = getReceiveSlots(receiver, receive_ਬਫਰ);

    ਲਈ (i = 0; i < se_L4_ਸੁਨੇਹਾ_Max_Extra_Caps && current_extra_caps.excaprefs[i] != ਨਿਰਾ; i++) {
        cte_t *ਥਾਂ = current_extra_caps.excaprefs[i];
        cap_t cap = ਥਾਂ->cap;

        ਜੇ (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            ਸੈੱਟ_Extra_Badge(receive_ਬਫਰ,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = se_L4_ਸੁਨੇਹਾ_Info_ਸੈੱਟ_caps_Unwrapped(info,
                                                      se_L4_ਸੁਨੇਹਾ_Info_get_caps_Unwrapped(info) | (1 << i));

        } ਨਹੀਂਤਾਂ {
            deriveCap_ret_t dc_ret;

            ਜੇ (!dest_ਥਾਂ) {
                ਰੋਕੋ;
            }

            dc_ret = deriveCap(ਥਾਂ, cap);

            ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
                ਰੋਕੋ;
            }
            ਜੇ (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                ਰੋਕੋ;
            }

            cte_ਪਾਓ(dc_ret.cap, ਥਾਂ, dest_ਥਾਂ);

            dest_ਥਾਂ = ਨਿਰਾ;
        }
    }

    ਵਾਪਸ se_L4_ਸੁਨੇਹਾ_Info_ਸੈੱਟ_extra_Caps(info, i);
}

ਖਾਲੀ doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
}

ਖਾਲੀ prepare_ਸੈੱਟ_Domain(tcb_t *tptr, dom_t dom)
{
    ਜੇ (ksCurDomain != dom) {
        Arch_prepare_ਸੈੱਟ_Domain(tptr, dom);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#ਜੇ_ਅੰਤ
    }
}

ਸਥਿਰ ਖਾਲੀ prepare_ਅਗਲਾ_Domain(ਖਾਲੀ)
{
    Arch_prepare_ਅਗਲਾ_Domain();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(ਨਿਰਾ);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ ਅਗਲਾ_Domain(ਖਾਲੀ)
{
    ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_Idx++;
    ਜੇ (dschedule_is_ਸਿਰਾ_marker(ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_Idx)) {
        ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_Idx = ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_ਸ਼ੁਰੂ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਗੰਢ_STATE(ksReprogram) = true;
#ਜੇ_ਅੰਤ
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ[ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_Idx]);
    ks_Domain_ਸਮਾਂ = dschedule_duration(ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ[ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_Idx]);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਸਥਿਰ ਖਾਲੀ switchSchedContext(ਖਾਲੀ)
{
    ਜੇ (unlikely(ਗੰਢ_STATE(ksCurSC) != ਗੰਢ_STATE(ksCurThread)->tcbSchedContext)) {
        ਗੰਢ_STATE(ksReprogram) = true;
        ਜੇ (sc_constant_bandwidth(ਗੰਢ_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(ਗੰਢ_STATE(ksCurThread)->tcbSchedContext);
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(refill_ready(ਗੰਢ_STATE(ksCurThread)->tcbSchedContext));
        ਪ੍ਰਤਿਜ੍ਞਾ(refill_sufficient(ਗੰਢ_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    ਜੇ (ਗੰਢ_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commit_ਸਮਾਂ();
    }

    ਗੰਢ_STATE(ksCurSC) = ਗੰਢ_STATE(ksCurThread)->tcbSchedContext;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_ਚੁਣੋ_New_Thread(ਖਾਲੀ)
{
    ਜੇ (ks_Domain_ਸਮਾਂ == 0) {
        prepare_ਅਗਲਾ_Domain();
        ਅਗਲਾ_Domain();
    }
    ਚੁਣੋ_Thread();
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    awaken();
    check_Domain_ਸਮਾਂ();
#ਜੇ_ਅੰਤ

    ਜੇ (ਗੰਢ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        ਜੇ (isSchedulable(ਗੰਢ_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_CURRENT_TCB;
        } ਨਹੀਂਤਾਂ {
            was_runnable = false;
        }

        ਜੇ (ਗੰਢ_STATE(ksSchedulerAction) == Scheduler_Action_ਚੁਣੋ_New_Thread) {
            ਸਮਾਂ_ਸਾਰਣੀ_ਚੁਣੋ_New_Thread();
        } ਨਹੀਂਤਾਂ {
            tcb_t *candidate = ਗੰਢ_STATE(ksSchedulerAction);
            ਪ੍ਰਤਿਜ੍ਞਾ(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                ਗੰਢ_STATE(ksCurThread) == ਗੰਢ_STATE(ksIdleThread)
                || (candidate->tcb_ਤਰਜੀਹ < ਗੰਢ_STATE(ksCurThread)->tcb_ਤਰਜੀਹ);
            ਜੇ (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcb_ਤਰਜੀਹ)) {
                SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(candidate);
                /* we can't, need to reschedule */
                ਗੰਢ_STATE(ksSchedulerAction) = Scheduler_Action_ਚੁਣੋ_New_Thread;
                ਸਮਾਂ_ਸਾਰਣੀ_ਚੁਣੋ_New_Thread();
            } ਨਹੀਂਤਾਂ ਜੇ (was_runnable && candidate->tcb_ਤਰਜੀਹ == ਗੰਢ_STATE(ksCurThread)->tcb_ਤਰਜੀਹ) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                ਗੰਢ_STATE(ksSchedulerAction) = Scheduler_Action_ਚੁਣੋ_New_Thread;
                ਸਮਾਂ_ਸਾਰਣੀ_ਚੁਣੋ_New_Thread();
            } ਨਹੀਂਤਾਂ {
                ਪ੍ਰਤਿਜ੍ਞਾ(candidate != ਗੰਢ_STATE(ksCurThread));
                switch_ਮੰਜ਼ਿਲ_Thread(candidate);
            }
        }
    }
    ਗੰਢ_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    do_ਨਕਾਬ_Reschedule(ARCH_ਗੰਢ_STATE(ipiReschedulePending));
    ARCH_ਗੰਢ_STATE(ipiReschedulePending) = 0;
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    switchSchedContext();

    ਜੇ (ਗੰਢ_STATE(ksReprogram)) {
        ਸੈੱਟ_ਅਗਲਾ_ਰੁਕਾਵਟ();
        ਗੰਢ_STATE(ksReprogram) = false;
    }
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ ਚੁਣੋ_Thread(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t prio;
    ਸ਼ਬਦ_t dom;
    tcb_t *thread;

    ਜੇ (ਅੰਕ_Domains > 1) {
        dom = ksCurDomain;
    } ਨਹੀਂਤਾਂ {
        dom = 0;
    }

    ਜੇ (likely(ਗੰਢ_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = ਗੰਢ_STATE(ksReadyQueues)[ready_queues_ਸੂਚਕਾਂਕ(dom, prio)].ਸਿਰ;
        ਪ੍ਰਤਿਜ੍ਞਾ(thread);
        ਪ੍ਰਤਿਜ੍ਞਾ(isSchedulable(thread));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਪ੍ਰਤਿਜ੍ਞਾ(refill_sufficient(thread->tcbSchedContext, 0));
        ਪ੍ਰਤਿਜ੍ਞਾ(refill_ready(thread->tcbSchedContext));
#ਜੇ_ਅੰਤ
        switch_ਮੰਜ਼ਿਲ_Thread(thread);
    } ਨਹੀਂਤਾਂ {
        switch_ਮੰਜ਼ਿਲ_Idle_Thread();
    }
}

ਖਾਲੀ switch_ਮੰਜ਼ਿਲ_Thread(tcb_t *thread)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਪ੍ਰਤਿਜ੍ਞਾ(thread->tcbSchedContext != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(!thread_state_get_tcb_ਵਿੱਚ_Release_ਕਤਾਰ(thread->tcbState));
    ਪ੍ਰਤਿਜ੍ਞਾ(refill_sufficient(thread->tcbSchedContext, 0));
    ਪ੍ਰਤਿਜ੍ਞਾ(refill_ready(thread->tcbSchedContext));
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ਗੰਢ_STATE(ksCurThread), thread);
#ਜੇ_ਅੰਤ
    Arch_switch_ਮੰਜ਼ਿਲ_Thread(thread);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#ਜੇ_ਅੰਤ /* CONFIG_HAVE_FPU */

    tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(thread);
    ਗੰਢ_STATE(ksCurThread) = thread;
}

ਖਾਲੀ switch_ਮੰਜ਼ਿਲ_Idle_Thread(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ਗੰਢ_STATE(ksCurThread), ਗੰਢ_STATE(ksIdleThread));
#ਜੇ_ਅੰਤ
    Arch_switch_ਮੰਜ਼ਿਲ_Idle_Thread();
    ਗੰਢ_STATE(ksCurThread) = ਗੰਢ_STATE(ksIdleThread);
}

ਖਾਲੀ ਸੈੱਟ_Domain(tcb_t *tptr, dom_t dom)
{
    tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(tptr);
    tptr->tcbDomain = dom;
    ਜੇ (isSchedulable(tptr)) {
        SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(tptr);
    }
    ਜੇ (tptr == ਗੰਢ_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

ਖਾਲੀ ਸੈੱਟ_MCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ ਸੈੱਟ_ਤਰਜੀਹ(tcb_t *tptr, prio_t prio)
{
    ਚੋਣ (thread_state_get_tsType(tptr->tcbState)) {
    ਹਾਲਤ ThreadState_Running:
    ਹਾਲਤ ThreadState_Restart:
        ਜੇ (thread_state_get_tcbQueued(tptr->tcbState) || tptr == ਗੰਢ_STATE(ksCurThread)) {
            tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(tptr);
            tptr->tcb_ਤਰਜੀਹ = prio;
            SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(tptr);
            rescheduleRequired();
        } ਨਹੀਂਤਾਂ {
            tptr->tcb_ਤਰਜੀਹ = prio;
        }
        ਰੋਕੋ;
    ਹਾਲਤ ThreadState_BlockedOnReceive:
    ਹਾਲਤ ThreadState_BlockedOnSend:
        tptr->tcb_ਤਰਜੀਹ = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ਰੋਕੋ;
    ਹਾਲਤ ThreadState_BlockedOnNotification:
        tptr->tcb_ਤਰਜੀਹ = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ਰੋਕੋ;
    ਮੂਲ:
        tptr->tcb_ਤਰਜੀਹ = prio;
        ਰੋਕੋ;
    }
}
#ਨਹੀਂਤਾਂ
ਖਾਲੀ ਸੈੱਟ_ਤਰਜੀਹ(tcb_t *tptr, prio_t prio)
{
    tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(tptr);
    tptr->tcb_ਤਰਜੀਹ = prio;
    ਜੇ (isRunnable(tptr)) {
        ਜੇ (tptr == ਗੰਢ_STATE(ksCurThread)) {
            rescheduleRequired();
        } ਨਹੀਂਤਾਂ {
            possible_Switch_ਮੰਜ਼ਿਲ(tptr);
        }
    }
}
#ਜੇ_ਅੰਤ

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
ਖਾਲੀ possible_Switch_ਮੰਜ਼ਿਲ(tcb_t *ਨਿਸ਼ਾਨਾ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਜੇ (ਨਿਸ਼ਾਨਾ->tcbSchedContext != ਨਿਰਾ && !thread_state_get_tcb_ਵਿੱਚ_Release_ਕਤਾਰ(ਨਿਸ਼ਾਨਾ->tcbState)) {
#ਜੇ_ਅੰਤ
        ਜੇ (ksCurDomain != ਨਿਸ਼ਾਨਾ->tcbDomain
            SMP_COND_STATEMENT( || ਨਿਸ਼ਾਨਾ->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ਨਿਸ਼ਾਨਾ);
        } ਨਹੀਂਤਾਂ ਜੇ (ਗੰਢ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ਨਿਸ਼ਾਨਾ);
        } ਨਹੀਂਤਾਂ {
            ਗੰਢ_STATE(ksSchedulerAction) = ਨਿਸ਼ਾਨਾ;
        }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    }
#ਜੇ_ਅੰਤ

}

ਖਾਲੀ ਸੈੱਟ_Thread_State(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_ਸੈੱਟ_ts_Type(&tptr->tcbState, ts);
    ਸਮਾਂ_ਸਾਰਣੀ_TCB(tptr);
}

ਖਾਲੀ ਸਮਾਂ_ਸਾਰਣੀ_TCB(tcb_t *tptr)
{
    ਜੇ (tptr == ਗੰਢ_STATE(ksCurThread) &&
        ਗੰਢ_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        ਗੰਢ_STATE(ksSchedulerAction) = Scheduler_Action_ਚੁਣੋ_New_Thread;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ਪ੍ਰਤਿਜ੍ਞਾ(tcb != ਨਿਰਾ);

    tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(tcb);
    tcb_Release_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(tcb);
    ਗੰਢ_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

ਖਾਲੀ ਸੈੱਟ_ਅਗਲਾ_ਰੁਕਾਵਟ(ਖਾਲੀ)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_ਸਿਰ_refill = *refill_ਸਿਰ(ਗੰਢ_STATE(ksCurThread)->tcbSchedContext);
    ticks_t ਅਗਲਾ_ਰੁਕਾਵਟ = ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) + ct_ਸਿਰ_refill.rAmount;

    ਜੇ (ਅੰਕ_Domains > 1) {
        ਅਗਲਾ_ਰੁਕਾਵਟ = MIN(ਅਗਲਾ_ਰੁਕਾਵਟ, ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) + ks_Domain_ਸਮਾਂ);
    }

    tcb_t *rlq_ਸਿਰ = ਗੰਢ_STATE(ks_Release_ਕਤਾਰ.ਸਿਰ);
    ਜੇ (rlq_ਸਿਰ != ਨਿਰਾ) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_ਸਿਰ_refill = *refill_ਸਿਰ(rlq_ਸਿਰ->tcbSchedContext);
        ਅਗਲਾ_ਰੁਕਾਵਟ = MIN(rlq_ਸਿਰ_refill.r_ਸਮਾਂ, ਅਗਲਾ_ਰੁਕਾਵਟ);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗਲਾ_ਰੁਕਾਵਟ >= ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    ਸੈੱਟ_Deadline(ਅਗਲਾ_ਰੁਕਾਵਟ - getTimerPrecision());
}

ਖਾਲੀ chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    ਜੇ (likely(ਗੰਢ_STATE(ksCurSC) != ਗੰਢ_STATE(ksIdleSC))) {
        ਜੇ (is_ਗੇੜਾ_Robin(ਗੰਢ_STATE(ksCurSC))) {
            ਪ੍ਰਤਿਜ੍ਞਾ(refill_ਮਾਪ(ਗੰਢ_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t ਸਿਰ = *refill_ਸਿਰ(ਗੰਢ_STATE(ksCurSC));
            refill_t ਪੂਛ = *refill_ਪੂਛ(ਗੰਢ_STATE(ksCurSC));
            refill_ਸਿਰ(ਗੰਢ_STATE(ksCurSC))->rAmount = ਸਿਰ.rAmount + ਪੂਛ.rAmount;
            refill_ਪੂਛ(ਗੰਢ_STATE(ksCurSC))->rAmount = 0;
        } ਨਹੀਂਤਾਂ {
            refill_budget_check(consumed);
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(refill_ਸਿਰ(ਗੰਢ_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        ਗੰਢ_STATE(ksCurSC)->scConsumed += consumed;
    }
    ਗੰਢ_STATE(ksConsumed) = 0;
    ਜੇ (likely(isSchedulable(ਗੰਢ_STATE(ksCurThread)))) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਗੰਢ_STATE(ksCurThread)->tcbSchedContext == ਗੰਢ_STATE(ksCurSC));
        ਸਿਰਾ_Timeslice(canTimeoutFault);
        rescheduleRequired();
        ਗੰਢ_STATE(ksReprogram) = true;
    }
}

ਖਾਲੀ ਸਿਰਾ_Timeslice(bool_t can_timeout_fault)
{
    bool_t ਗੇੜਾ_robin = is_ਗੇੜਾ_Robin(ਗੰਢ_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(ਗੰਢ_STATE(ksCurThread));

    ਜੇ (can_timeout_fault && !ਗੇੜਾ_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(ਗੰਢ_STATE(ksCurSC)->scBadge);
        handleTimeout(ਗੰਢ_STATE(ksCurThread));
    } ਨਹੀਂਤਾਂ ਜੇ (refill_ready(ਗੰਢ_STATE(ksCurSC)) && refill_sufficient(ਗੰਢ_STATE(ksCurSC), 0)) {
        /* apply round robin */
        ਪ੍ਰਤਿਜ੍ਞਾ(!thread_state_get_tcbQueued(ਗੰਢ_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } ਨਹੀਂਤਾਂ {
        /* postpone until ready */
        postpone(ਗੰਢ_STATE(ksCurSC));
    }
}
#ਨਹੀਂਤਾਂ

ਖਾਲੀ timerTick(ਖਾਲੀ)
{
    ਜੇ (likely(thread_state_get_tsType(ਗੰਢ_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
        || thread_state_get_tsType(ਗੰਢ_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#ਜੇ_ਅੰਤ
       ) {
        ਜੇ (ਗੰਢ_STATE(ksCurThread)->tcb_ਸਮਾਂ_ਟੁਕੜਾ > 1) {
            ਗੰਢ_STATE(ksCurThread)->tcb_ਸਮਾਂ_ਟੁਕੜਾ--;
        } ਨਹੀਂਤਾਂ {
            ਗੰਢ_STATE(ksCurThread)->tcb_ਸਮਾਂ_ਟੁਕੜਾ = CONFIG_ਸਮਾਂ_ਟੁਕੜਾ;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    ਜੇ (ਅੰਕ_Domains > 1) {
        ks_Domain_ਸਮਾਂ--;
        ਜੇ (ks_Domain_ਸਮਾਂ == 0) {
            rescheduleRequired();
        }
    }
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ rescheduleRequired(ਖਾਲੀ)
{
    ਜੇ (ਗੰਢ_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && ਗੰਢ_STATE(ksSchedulerAction) != Scheduler_Action_ਚੁਣੋ_New_Thread
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        && isSchedulable(ਗੰਢ_STATE(ksSchedulerAction))
#ਜੇ_ਅੰਤ
       ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਪ੍ਰਤਿਜ੍ਞਾ(refill_sufficient(ਗੰਢ_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        ਪ੍ਰਤਿਜ੍ਞਾ(refill_ready(ਗੰਢ_STATE(ksSchedulerAction)->tcbSchedContext));
#ਜੇ_ਅੰਤ
        SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ਗੰਢ_STATE(ksSchedulerAction));
    }
    ਗੰਢ_STATE(ksSchedulerAction) = Scheduler_Action_ਚੁਣੋ_New_Thread;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t ਸ਼ੁੱਧ release_q_non_ਖਾਲੀ_ਥਾਵਾਂ_and_ready(ਖਾਲੀ)
{
    ਵਾਪਸ ਗੰਢ_STATE(ks_Release_ਕਤਾਰ.ਸਿਰ) != ਨਿਰਾ
           && refill_ready(ਗੰਢ_STATE(ks_Release_ਕਤਾਰ.ਸਿਰ)->tcbSchedContext);
}

ਸਥਿਰ ਖਾਲੀ tcb_Release_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(ਖਾਲੀ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਗੰਢ_STATE(ks_Release_ਕਤਾਰ.ਸਿਰ) != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਗੰਢ_STATE(ks_Release_ਕਤਾਰ.ਸਿਰ)->tcbSchedPrev == ਨਿਰਾ);
    SMP_COND_STATEMENT(ਪ੍ਰਤਿਜ੍ਞਾ(ਗੰਢ_STATE(ks_Release_ਕਤਾਰ.ਸਿਰ)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = ਗੰਢ_STATE(ks_Release_ਕਤਾਰ.ਸਿਰ);
    ਪ੍ਰਤਿਜ੍ਞਾ(awakened != ਗੰਢ_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    ਪ੍ਰਤਿਜ੍ਞਾ(!is_ਗੇੜਾ_Robin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(ਪ੍ਰਤਿਜ੍ਞਾ(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    ਪ੍ਰਤਿਜ੍ਞਾ(refill_sufficient(awakened->tcbSchedContext, 0));
    possible_Switch_ਮੰਜ਼ਿਲ(awakened);
}

ਖਾਲੀ awaken(ਖਾਲੀ)
{
    ਜਦੋਂ (unlikely(release_q_non_ਖਾਲੀ_ਥਾਵਾਂ_and_ready())) {
        tcb_Release_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ();
    }
}
#ਜੇ_ਅੰਤ
