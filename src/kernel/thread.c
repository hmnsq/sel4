/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <object.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <api/faults.h>
#ಅನ್ತರ್ಭಾವ <api/types.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <object/domain.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
#ಅನ್ತರ್ಭಾವ <object/schedcontext.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <linker.h>

ಸ್ಥಿರ se_L4_ಸಂದೇಶ_Info_t
transferCaps(se_L4_ಸಂದೇಶ_Info_t info,
             endpoint_t *endpoint, tcb_t *receiver,
             ಶಬ್ದ_t *receive_ಬಫರ್);

BOOT_ಸಂಕೇತ ಶೂನ್ಯ configureIdleThread(tcb_t *tcb)
{
    tcb->tcbFlags = seL4_TCBFlag_fpuDisabled;
    Arch_configureIdleThread(tcb);
    ಗಣ_Thread_State(tcb, ThreadState_IdleThreadState);
}

ಶೂನ್ಯ activateThread(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (unlikely(ಗಂಟು_STATE(ksCurThread)->tcb_Yield_ಗಮ್ಯ)) {
        sched_Context_complete_Yield_ಗಮ್ಯ(ಗಂಟು_STATE(ksCurThread));
        ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(ಗಂಟು_STATE(ksCurThread)->tcbState) == ThreadState_Running);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆಯ್ಕೆ (thread_state_get_tsType(ಗಂಟು_STATE(ksCurThread)->tcbState)) {
    ಸ್ಥಿತಿ ThreadState_Running:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಸ್ಥಿತಿ ThreadState_RunningVM:
#ಆದರೆ_ಅಂತ್ಯ
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ ThreadState_Restart: {
        ಶಬ್ದ_t pc;

        pc = getRestartPC(ಗಂಟು_STATE(ksCurThread));
        ಗಣ_ಮುಂದಿನ_PC(ಗಂಟು_STATE(ksCurThread), pc);
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ ThreadState_IdleThreadState:
        Arch_activateIdleThread(ಗಂಟು_STATE(ksCurThread));
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Current thread is blocked");
    }
}

ಶೂನ್ಯ suspend(tcb_t *ಗುರಿ)
{
    cancelIPC(ಗುರಿ);
    ಆದರೆ (thread_state_get_tsType(ಗುರಿ->tcbState) == ThreadState_Running) {
        /* whilst in the running state it is possible that restart pc of a thread is
         * incorrect. As we do not know what state this thread will transition to
         * after we make it inactive we update its restart pc so that the thread next
         * runs at the correct address whether it is restarted or moved directly to
         * running */
        updateRestartPC(ಗುರಿ);
    }
    tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(ಗುರಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    tcbReleaseRemove(ಗುರಿ);
    sched_Context_cancel_Yield_ಗಮ್ಯ(ಗುರಿ);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_Thread_State(ಗುರಿ, ThreadState_Inactive);
}

ಶೂನ್ಯ restart(tcb_t *ಗುರಿ)
{
    ಆದರೆ (isStopped(ಗುರಿ)) {
        cancelIPC(ಗುರಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಗಣ_Thread_State(ಗುರಿ, ThreadState_Restart);
        sched_context_t *sc = ಗುರಿ->tcbSchedContext;
        ಆದರೆ (sc_sporadic(sc) && sc != ಗಂಟು_STATE(ksCurSC)) {
            refill_unblock_check(sc);
        }
        schedContext_resume(sc);
        ಆದರೆ (isSchedulable(ಗುರಿ)) {
            possible_Switch_ಗಮ್ಯ(ಗುರಿ);
        }
#ಇಲ್ಲದಿದ್ದರೆ
        ಸಿದ್ಧತೆ_ಉತ್ತರ_Master(ಗುರಿ);
        ಗಣ_Thread_State(ಗುರಿ, ThreadState_Restart);
        SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(ಗುರಿ);
        possible_Switch_ಗಮ್ಯ(ಗುರಿ);
#ಆದರೆ_ಅಂತ್ಯ
    }
}

ಶೂನ್ಯ doIPCTransfer(tcb_t *sender, endpoint_t *endpoint, ಶಬ್ದ_t badge,
                   bool_t grant, tcb_t *receiver)
{
    ಶೂನ್ಯ *receive_ಬಫರ್, *send_ಬಫರ್;

    receive_ಬಫರ್ = lookupIPCBuffer(true, receiver);

    ಆದರೆ (likely(seL4_Fault_get_seL4_FaultType(sender->tcbFault) == seL4_Fault_NullFault)) {
        send_ಬಫರ್ = lookupIPCBuffer(false, sender);
        do_ಸಾಮಾನ್ಯ_Transfer(sender, send_ಬಫರ್, endpoint, badge, grant,
                         receiver, receive_ಬಫರ್);
    } ಇಲ್ಲದಿದ್ದರೆ {
        doFaultTransfer(badge, sender, receiver, receive_ಬಫರ್);
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ do_ಉತ್ತರ_Transfer(tcb_t *sender, ಉತ್ತರ_t *ಉತ್ತರ, bool_t grant)
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ do_ಉತ್ತರ_Transfer(tcb_t *sender, tcb_t *receiver, cte_t *ಸ್ಥಾನ, bool_t grant)
#ಆದರೆ_ಅಂತ್ಯ
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (ಉತ್ತರ->ಉತ್ತರ_TCB == ಖಾಲಿ ||
        thread_state_get_tsType(ಉತ್ತರ->ಉತ್ತರ_TCB->tcbState) != Thread_State_Blocked_On_ಉತ್ತರ) {
        /* nothing to do */
        ಹಿಂತಿರುಗಿ;
    }

    tcb_t *receiver = ಉತ್ತರ->ಉತ್ತರ_TCB;
    ಉತ್ತರ_remove(ಉತ್ತರ, receiver);
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(receiver->tcbState) == ThreadState_Inactive);
    ಪ್ರತಿಜ್ಞಾ(ಉತ್ತರ->ಉತ್ತರ_TCB == ಖಾಲಿ);

    ಆದರೆ (sc_sporadic(receiver->tcbSchedContext)
        && receiver->tcbSchedContext != ಗಂಟು_STATE_ON_CORE(ksCurSC, receiver->tcbSchedContext->scCore)) {
        refill_unblock_check(receiver->tcbSchedContext);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(receiver->tcbState) ==
           Thread_State_Blocked_On_ಉತ್ತರ);
#ಆದರೆ_ಅಂತ್ಯ

    ಶಬ್ದ_t fault_type = seL4_Fault_get_seL4_FaultType(receiver->tcbFault);
    ಆದರೆ (likely(fault_type == seL4_Fault_NullFault)) {
        doIPCTransfer(sender, ಖಾಲಿ, 0, grant, receiver);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಗಣ_Thread_State(receiver, ThreadState_Running);
#ಇಲ್ಲದಿದ್ದರೆ
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(ಸ್ಥಾನ);
        ಗಣ_Thread_State(receiver, ThreadState_Running);
        possible_Switch_ಗಮ್ಯ(receiver);
#ಆದರೆ_ಅಂತ್ಯ
    } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
        /** GHOSTUPD: "(True, gs_set_assn cteDeleteOne_'proc (ucast cap_reply_cap))" */
        cteDeleteOne(ಸ್ಥಾನ);
#ಆದರೆ_ಅಂತ್ಯ
        bool_t restart = handle_Fault_ಉತ್ತರ(receiver, sender);
        receiver->tcbFault = seL4_Fault_NullFault_new();
        ಆದರೆ (restart) {
            ಗಣ_Thread_State(receiver, ThreadState_Restart);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
            possible_Switch_ಗಮ್ಯ(receiver);
#ಆದರೆ_ಅಂತ್ಯ
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಣ_Thread_State(receiver, ThreadState_Inactive);
        }
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (receiver->tcbSchedContext && isRunnable(receiver)) {
        sched_context_t *sc = receiver->tcbSchedContext;
        ಆದರೆ ((refill_ready(sc) && refill_sufficient(sc, 0))) {
            possible_Switch_ಗಮ್ಯ(receiver);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (validTimeoutHandler(receiver) && fault_type != seL4_Fault_Timeout) {
                current_fault = seL4_Fault_Timeout_new(sc->scBadge);
                handleTimeout(receiver);
            } ಇಲ್ಲದಿದ್ದರೆ {
                postpone(sc);
            }
        }
    }
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ do_ಸಾಮಾನ್ಯ_Transfer(tcb_t *sender, ಶಬ್ದ_t *send_ಬಫರ್, endpoint_t *endpoint,
                      ಶಬ್ದ_t badge, bool_t canGrant, tcb_t *receiver,
                      ಶಬ್ದ_t *receive_ಬಫರ್)
{
    ಶಬ್ದ_t ಸಂದೇಶ_Transferred;
    se_L4_ಸಂದೇಶ_Info_t ಟ್ಯಾಗ್;
    exception_t ಸ್ಥಿತಿ_2;

    ಟ್ಯಾಗ್ = ಸಂದೇಶ_Info_From_ಶಬ್ದ(get_ರಿಜಿಸ್ಟರ್(sender, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್));

    ಆದರೆ (canGrant) {
        ಸ್ಥಿತಿ_2 = lookupExtraCaps(sender, send_ಬಫರ್, ಟ್ಯಾಗ್);
        ಆದರೆ (unlikely(ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            current_extra_caps.excaprefs[0] = ಖಾಲಿ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        current_extra_caps.excaprefs[0] = ಖಾಲಿ;
    }

    ಸಂದೇಶ_Transferred = copyMRs(sender, send_ಬಫರ್, receiver, receive_ಬಫರ್,
                             se_L4_ಸಂದೇಶ_Info_get_ಉದ್ದಳತೆ(ಟ್ಯಾಗ್));

    ಟ್ಯಾಗ್ = transferCaps(ಟ್ಯಾಗ್, endpoint, receiver, receive_ಬಫರ್);

    ಟ್ಯಾಗ್ = se_L4_ಸಂದೇಶ_Info_ಗಣ_ಉದ್ದಳತೆ(ಟ್ಯಾಗ್, ಸಂದೇಶ_Transferred);
    ಗಣ_ರಿಜಿಸ್ಟರ್(receiver, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(ಟ್ಯಾಗ್));
    ಗಣ_ರಿಜಿಸ್ಟರ್(receiver, badge_ರಿಜಿಸ್ಟರ್, badge);
}

ಶೂನ್ಯ doFaultTransfer(ಶಬ್ದ_t badge, tcb_t *sender, tcb_t *receiver,
                     ಶಬ್ದ_t *receiverIPCBuffer)
{
    ಶಬ್ದ_t ಕಳುಹಿಸಿದ;
    se_L4_ಸಂದೇಶ_Info_t ಸಂದೇಶ_Info;

    ಕಳುಹಿಸಿದ = ಗಣ_MRs_fault(sender, receiver, receiverIPCBuffer);
    ಸಂದೇಶ_Info = se_L4_ಸಂದೇಶ_Info_new(
                  seL4_Fault_get_seL4_FaultType(sender->tcbFault), 0, 0, ಕಳುಹಿಸಿದ);
    ಗಣ_ರಿಜಿಸ್ಟರ್(receiver, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(ಸಂದೇಶ_Info));
    ಗಣ_ರಿಜಿಸ್ಟರ್(receiver, badge_ರಿಜಿಸ್ಟರ್, badge);
}

/* Like getReceiveSlots, this is specialised for single-cap transfer. */
ಸ್ಥಿರ se_L4_ಸಂದೇಶ_Info_t transferCaps(se_L4_ಸಂದೇಶ_Info_t info,
                                       endpoint_t *endpoint, tcb_t *receiver,
                                       ಶಬ್ದ_t *receive_ಬಫರ್)
{
    ಶಬ್ದ_t i;
    cte_t *dest_ಸ್ಥಾನ;

    info = se_L4_ಸಂದೇಶ_Info_ಗಣ_extra_Caps(info, 0);
    info = se_L4_ಸಂದೇಶ_Info_ಗಣ_caps_Unwrapped(info, 0);

    ಆದರೆ (likely(!current_extra_caps.excaprefs[0] || !receive_ಬಫರ್)) {
        ಹಿಂತಿರುಗಿ info;
    }

    dest_ಸ್ಥಾನ = getReceiveSlots(receiver, receive_ಬಫರ್);

    ಗಾಗಿ (i = 0; i < se_L4_ಸಂದೇಶ_Max_Extra_Caps && current_extra_caps.excaprefs[i] != ಖಾಲಿ; i++) {
        cte_t *ಸ್ಥಾನ = current_extra_caps.excaprefs[i];
        cap_t cap = ಸ್ಥಾನ->cap;

        ಆದರೆ (cap_get_capType(cap) == cap_endpoint_cap &&
            EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)) == endpoint) {
            /* If this is a cap to the endpoint on which the message was sent,
             * only transfer the badge, not the cap. */
            ಗಣ_Extra_Badge(receive_ಬಫರ್,
                          cap_endpoint_cap_get_capEPBadge(cap), i);

            info = se_L4_ಸಂದೇಶ_Info_ಗಣ_caps_Unwrapped(info,
                                                      se_L4_ಸಂದೇಶ_Info_get_caps_Unwrapped(info) | (1 << i));

        } ಇಲ್ಲದಿದ್ದರೆ {
            deriveCap_ret_t dc_ret;

            ಆದರೆ (!dest_ಸ್ಥಾನ) {
                ನಿಲ್ಲಿಸು;
            }

            dc_ret = deriveCap(ಸ್ಥಾನ, cap);

            ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ (cap_get_capType(dc_ret.cap) == cap_null_cap) {
                ನಿಲ್ಲಿಸು;
            }

            cte_ಸೇರಿಸು(dc_ret.cap, ಸ್ಥಾನ, dest_ಸ್ಥಾನ);

            dest_ಸ್ಥಾನ = ಖಾಲಿ;
        }
    }

    ಹಿಂತಿರುಗಿ se_L4_ಸಂದೇಶ_Info_ಗಣ_extra_Caps(info, i);
}

ಶೂನ್ಯ doNBRecvFailedTransfer(tcb_t *thread)
{
    /* Set the badge register to 0 to indicate there was no message */
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
}

ಶೂನ್ಯ prepare_ಗಣ_Domain(tcb_t *tptr, dom_t dom)
{
    ಆದರೆ (ksCurDomain != dom) {
        Arch_prepare_ಗಣ_Domain(tptr, dom);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
        /* Save FPU state now to avoid touching cross-domain state later */
        fpuRelease(tptr);
#ಆದರೆ_ಅಂತ್ಯ
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ prepare_ಮುಂದಿನ_Domain(ಶೂನ್ಯ)
{
    Arch_prepare_ಮುಂದಿನ_Domain();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
    /* Save FPU state now to avoid touching cross-domain state later */
    switchLocalFpuOwner(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ ಮುಂದಿನ_Domain(ಶೂನ್ಯ)
{
    ks_Dom_ವೇಳಾಪಟ್ಟಿ_Idx++;
    ಆದರೆ (dschedule_is_ಕೊನೆ_marker(ks_Dom_ವೇಳಾಪಟ್ಟಿ_Idx)) {
        ks_Dom_ವೇಳಾಪಟ್ಟಿ_Idx = ks_Dom_ವೇಳಾಪಟ್ಟಿ_ಆರಂಭ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಗಂಟು_STATE(ksReprogram) = true;
#ಆದರೆ_ಅಂತ್ಯ
    ksWorkUnitsCompleted = 0;
    ksCurDomain = dschedule_domain(ks_Dom_ವೇಳಾಪಟ್ಟಿ[ks_Dom_ವೇಳಾಪಟ್ಟಿ_Idx]);
    ks_Domain_ಸಮಯ = dschedule_duration(ks_Dom_ವೇಳಾಪಟ್ಟಿ[ks_Dom_ವೇಳಾಪಟ್ಟಿ_Idx]);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಸ್ಥಿರ ಶೂನ್ಯ switchSchedContext(ಶೂನ್ಯ)
{
    ಆದರೆ (unlikely(ಗಂಟು_STATE(ksCurSC) != ಗಂಟು_STATE(ksCurThread)->tcbSchedContext)) {
        ಗಂಟು_STATE(ksReprogram) = true;
        ಆದರೆ (sc_constant_bandwidth(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext)) {
            refill_unblock_check(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext);
        }

        ಪ್ರತಿಜ್ಞಾ(refill_ready(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext));
        ಪ್ರತಿಜ್ಞಾ(refill_sufficient(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext, 0));
    }

    ಆದರೆ (ಗಂಟು_STATE(ksReprogram)) {
        /* if we are reprogamming, we have acted on the new kernel time and cannot
         * rollback -> charge the current thread */
        commit_ಸಮಯ();
    }

    ಗಂಟು_STATE(ksCurSC) = ಗಂಟು_STATE(ksCurThread)->tcbSchedContext;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_ಆರಿಸು_New_Thread(ಶೂನ್ಯ)
{
    ಆದರೆ (ks_Domain_ಸಮಯ == 0) {
        prepare_ಮುಂದಿನ_Domain();
        ಮುಂದಿನ_Domain();
    }
    ಆರಿಸು_Thread();
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    awaken();
    check_Domain_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಗಂಟು_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
        bool_t was_runnable;
        ಆದರೆ (isSchedulable(ಗಂಟು_STATE(ksCurThread))) {
            was_runnable = true;
            SCHED_ಸರತಿಗೆ_ಸೇರಿಸು_CURRENT_TCB;
        } ಇಲ್ಲದಿದ್ದರೆ {
            was_runnable = false;
        }

        ಆದರೆ (ಗಂಟು_STATE(ksSchedulerAction) == Scheduler_Action_ಆರಿಸು_New_Thread) {
            ವೇಳಾಪಟ್ಟಿ_ಆರಿಸು_New_Thread();
        } ಇಲ್ಲದಿದ್ದರೆ {
            tcb_t *candidate = ಗಂಟು_STATE(ksSchedulerAction);
            ಪ್ರತಿಜ್ಞಾ(isSchedulable(candidate));
            /* Avoid checking bitmap when ksCurThread is higher prio, to
             * match fast path.
             * Don't look at ksCurThread prio when it's idle, to respect
             * information flow in non-fastpath cases. */
            bool_t fastfail =
                ಗಂಟು_STATE(ksCurThread) == ಗಂಟು_STATE(ksIdleThread)
                || (candidate->tcb_ಆದ್ಯತೆ < ಗಂಟು_STATE(ksCurThread)->tcb_ಆದ್ಯತೆ);
            ಆದರೆ (fastfail &&
                !isHighestPrio(ksCurDomain, candidate->tcb_ಆದ್ಯತೆ)) {
                SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(candidate);
                /* we can't, need to reschedule */
                ಗಂಟು_STATE(ksSchedulerAction) = Scheduler_Action_ಆರಿಸು_New_Thread;
                ವೇಳಾಪಟ್ಟಿ_ಆರಿಸು_New_Thread();
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (was_runnable && candidate->tcb_ಆದ್ಯತೆ == ಗಂಟು_STATE(ksCurThread)->tcb_ಆದ್ಯತೆ) {
                /* We append the candidate at the end of the scheduling queue, that way the
                 * current thread, that was enqueued at the start of the scheduling queue
                 * will get picked during chooseNewThread */
                SCHED_APPEND(candidate);
                ಗಂಟು_STATE(ksSchedulerAction) = Scheduler_Action_ಆರಿಸು_New_Thread;
                ವೇಳಾಪಟ್ಟಿ_ಆರಿಸು_New_Thread();
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಪ್ರತಿಜ್ಞಾ(candidate != ಗಂಟು_STATE(ksCurThread));
                switch_ಗಮ್ಯ_Thread(candidate);
            }
        }
    }
    ಗಂಟು_STATE(ksSchedulerAction) = SchedulerAction_ResumeCurrentThread;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    do_ಮುಖವಾಡ_Reschedule(ARCH_ಗಂಟು_STATE(ipiReschedulePending));
    ARCH_ಗಂಟು_STATE(ipiReschedulePending) = 0;
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    switchSchedContext();

    ಆದರೆ (ಗಂಟು_STATE(ksReprogram)) {
        ಗಣ_ಮುಂದಿನ_ಅಡಚಣೆ();
        ಗಂಟು_STATE(ksReprogram) = false;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ ಆರಿಸು_Thread(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t prio;
    ಶಬ್ದ_t dom;
    tcb_t *thread;

    ಆದರೆ (ಅಂಕೆ_Domains > 1) {
        dom = ksCurDomain;
    } ಇಲ್ಲದಿದ್ದರೆ {
        dom = 0;
    }

    ಆದರೆ (likely(ಗಂಟು_STATE(ksReadyQueuesL1Bitmap[dom]))) {
        prio = getHighestPrio(dom);
        thread = ಗಂಟು_STATE(ksReadyQueues)[ready_queues_ಸೂಚ್ಯಂಕ(dom, prio)].ತಲೆ;
        ಪ್ರತಿಜ್ಞಾ(thread);
        ಪ್ರತಿಜ್ಞಾ(isSchedulable(thread));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಪ್ರತಿಜ್ಞಾ(refill_sufficient(thread->tcbSchedContext, 0));
        ಪ್ರತಿಜ್ಞಾ(refill_ready(thread->tcbSchedContext));
#ಆದರೆ_ಅಂತ್ಯ
        switch_ಗಮ್ಯ_Thread(thread);
    } ಇಲ್ಲದಿದ್ದರೆ {
        switch_ಗಮ್ಯ_Idle_Thread();
    }
}

ಶೂನ್ಯ switch_ಗಮ್ಯ_Thread(tcb_t *thread)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಪ್ರತಿಜ್ಞಾ(thread->tcbSchedContext != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(!thread_state_get_tcb_ಒಳ_Release_ಸರತಿ(thread->tcbState));
    ಪ್ರತಿಜ್ಞಾ(refill_sufficient(thread->tcbSchedContext, 0));
    ಪ್ರತಿಜ್ಞಾ(refill_ready(thread->tcbSchedContext));
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ಗಂಟು_STATE(ksCurThread), thread);
#ಆದರೆ_ಅಂತ್ಯ
    Arch_switch_ಗಮ್ಯ_Thread(thread);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
    lazyFPURestore(thread);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HAVE_FPU */

    tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(thread);
    ಗಂಟು_STATE(ksCurThread) = thread;
}

ಶೂನ್ಯ switch_ಗಮ್ಯ_Idle_Thread(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_UTILISATION
    benchmark_utilisation_switch(ಗಂಟು_STATE(ksCurThread), ಗಂಟು_STATE(ksIdleThread));
#ಆದರೆ_ಅಂತ್ಯ
    Arch_switch_ಗಮ್ಯ_Idle_Thread();
    ಗಂಟು_STATE(ksCurThread) = ಗಂಟು_STATE(ksIdleThread);
}

ಶೂನ್ಯ ಗಣ_Domain(tcb_t *tptr, dom_t dom)
{
    tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(tptr);
    tptr->tcbDomain = dom;
    ಆದರೆ (isSchedulable(tptr)) {
        SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(tptr);
    }
    ಆದರೆ (tptr == ಗಂಟು_STATE(ksCurThread)) {
        rescheduleRequired();
    }
}

ಶೂನ್ಯ ಗಣ_MCPriority(tcb_t *tptr, prio_t mcp)
{
    tptr->tcbMCP = mcp;
}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ ಗಣ_ಆದ್ಯತೆ(tcb_t *tptr, prio_t prio)
{
    ಆಯ್ಕೆ (thread_state_get_tsType(tptr->tcbState)) {
    ಸ್ಥಿತಿ ThreadState_Running:
    ಸ್ಥಿತಿ ThreadState_Restart:
        ಆದರೆ (thread_state_get_tcbQueued(tptr->tcbState) || tptr == ಗಂಟು_STATE(ksCurThread)) {
            tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(tptr);
            tptr->tcb_ಆದ್ಯತೆ = prio;
            SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(tptr);
            rescheduleRequired();
        } ಇಲ್ಲದಿದ್ದರೆ {
            tptr->tcb_ಆದ್ಯತೆ = prio;
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ThreadState_BlockedOnReceive:
    ಸ್ಥಿತಿ ThreadState_BlockedOnSend:
        tptr->tcb_ಆದ್ಯತೆ = prio;
        reorderEP(EP_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ThreadState_BlockedOnNotification:
        tptr->tcb_ಆದ್ಯತೆ = prio;
        reorderNTFN(NTFN_PTR(thread_state_get_blockingObject(tptr->tcbState)), tptr);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        tptr->tcb_ಆದ್ಯತೆ = prio;
        ನಿಲ್ಲಿಸು;
    }
}
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ ಗಣ_ಆದ್ಯತೆ(tcb_t *tptr, prio_t prio)
{
    tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(tptr);
    tptr->tcb_ಆದ್ಯತೆ = prio;
    ಆದರೆ (isRunnable(tptr)) {
        ಆದರೆ (tptr == ಗಂಟು_STATE(ksCurThread)) {
            rescheduleRequired();
        } ಇಲ್ಲದಿದ್ದರೆ {
            possible_Switch_ಗಮ್ಯ(tptr);
        }
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/* Note that this thread will possibly continue at the end of this kernel
 * entry. Do not queue it yet, since a queue+unqueue operation is wasteful
 * if it will be picked. Instead, it waits in the 'ksSchedulerAction' site
 * on which the scheduler will take action. */
ಶೂನ್ಯ possible_Switch_ಗಮ್ಯ(tcb_t *ಗುರಿ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (ಗುರಿ->tcbSchedContext != ಖಾಲಿ && !thread_state_get_tcb_ಒಳ_Release_ಸರತಿ(ಗುರಿ->tcbState)) {
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ (ksCurDomain != ಗುರಿ->tcbDomain
            SMP_COND_STATEMENT( || ಗುರಿ->tcbAffinity != getCurrentCPUIndex())) {
            SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(ಗುರಿ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗಂಟು_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread) {
            /* Too many threads want special treatment, use regular queues. */
            rescheduleRequired();
            SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(ಗುರಿ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಂಟು_STATE(ksSchedulerAction) = ಗುರಿ;
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    }
#ಆದರೆ_ಅಂತ್ಯ

}

ಶೂನ್ಯ ಗಣ_Thread_State(tcb_t *tptr, _thread_state_t ts)
{
    thread_state_ptr_ಗಣ_ts_Type(&tptr->tcbState, ts);
    ವೇಳಾಪಟ್ಟಿ_TCB(tptr);
}

ಶೂನ್ಯ ವೇಳಾಪಟ್ಟಿ_TCB(tcb_t *tptr)
{
    ಆದರೆ (tptr == ಗಂಟು_STATE(ksCurThread) &&
        ಗಂಟು_STATE(ksSchedulerAction) == SchedulerAction_ResumeCurrentThread &&
        !isSchedulable(tptr)) {
        /* short-cut rescheduleRequired(), because we know what the scheduler action is. */
        ಗಂಟು_STATE(ksSchedulerAction) = Scheduler_Action_ಆರಿಸು_New_Thread;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ postpone(sched_context_t *sc)
{
    tcb_t *tcb = sc->scTcb;
    ಪ್ರತಿಜ್ಞಾ(tcb != ಖಾಲಿ);

    tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(tcb);
    tcb_Release_ಸರತಿಗೆ_ಸೇರಿಸು(tcb);
    ಗಂಟು_STATE_ON_CORE(ksReprogram, sc->scCore) = true;
}

ಶೂನ್ಯ ಗಣ_ಮುಂದಿನ_ಅಡಚಣೆ(ಶೂನ್ಯ)
{
    /* fetch the head refill separately to ease verification */
    refill_t ct_ತಲೆ_refill = *refill_ತಲೆ(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext);
    ticks_t ಮುಂದಿನ_ಅಡಚಣೆ = ಗಂಟು_STATE(ks_Cur_ಸಮಯ) + ct_ತಲೆ_refill.rAmount;

    ಆದರೆ (ಅಂಕೆ_Domains > 1) {
        ಮುಂದಿನ_ಅಡಚಣೆ = MIN(ಮುಂದಿನ_ಅಡಚಣೆ, ಗಂಟು_STATE(ks_Cur_ಸಮಯ) + ks_Domain_ಸಮಯ);
    }

    tcb_t *rlq_ತಲೆ = ಗಂಟು_STATE(ks_Release_ಸರತಿ.ತಲೆ);
    ಆದರೆ (rlq_ತಲೆ != ಖಾಲಿ) {
        /* fetch the head refill separately to ease verification */
        refill_t rlq_ತಲೆ_refill = *refill_ತಲೆ(rlq_ತಲೆ->tcbSchedContext);
        ಮುಂದಿನ_ಅಡಚಣೆ = MIN(rlq_ತಲೆ_refill.r_ಸಮಯ, ಮುಂದಿನ_ಅಡಚಣೆ);
    }

    /* We should never be attempting to schedule anything earlier than ksCurTime */
    ಪ್ರತಿಜ್ಞಾ(ಮುಂದಿನ_ಅಡಚಣೆ >= ಗಂಟು_STATE(ks_Cur_ಸಮಯ));

    /* Our lower bound ksCurTime is slightly in the past (at kernel entry) and
       we are further subtracting getTimerPrecision(), so we may be setting a
       deadline in the past. If that is the case, we assume the IRQ will be
       raised immediately after we leave the kernel. */
    ಗಣ_Deadline(ಮುಂದಿನ_ಅಡಚಣೆ - getTimerPrecision());
}

ಶೂನ್ಯ chargeBudget(ticks_t consumed, bool_t canTimeoutFault)
{
    ಆದರೆ (likely(ಗಂಟು_STATE(ksCurSC) != ಗಂಟು_STATE(ksIdleSC))) {
        ಆದರೆ (is_ಸುತ್ತು_Robin(ಗಂಟು_STATE(ksCurSC))) {
            ಪ್ರತಿಜ್ಞಾ(refill_ಗಾತ್ರ(ಗಂಟು_STATE(ksCurSC)) == MIN_REFILLS);
            refill_t ತಲೆ = *refill_ತಲೆ(ಗಂಟು_STATE(ksCurSC));
            refill_t ಬಾಲ = *refill_ಬಾಲ(ಗಂಟು_STATE(ksCurSC));
            refill_ತಲೆ(ಗಂಟು_STATE(ksCurSC))->rAmount = ತಲೆ.rAmount + ಬಾಲ.rAmount;
            refill_ಬಾಲ(ಗಂಟು_STATE(ksCurSC))->rAmount = 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            refill_budget_check(consumed);
        }

        ಪ್ರತಿಜ್ಞಾ(refill_ತಲೆ(ಗಂಟು_STATE(ksCurSC))->rAmount >= MIN_BUDGET);
        ಗಂಟು_STATE(ksCurSC)->scConsumed += consumed;
    }
    ಗಂಟು_STATE(ksConsumed) = 0;
    ಆದರೆ (likely(isSchedulable(ಗಂಟು_STATE(ksCurThread)))) {
        ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE(ksCurThread)->tcbSchedContext == ಗಂಟು_STATE(ksCurSC));
        ಕೊನೆ_Timeslice(canTimeoutFault);
        rescheduleRequired();
        ಗಂಟು_STATE(ksReprogram) = true;
    }
}

ಶೂನ್ಯ ಕೊನೆ_Timeslice(bool_t can_timeout_fault)
{
    bool_t ಸುತ್ತು_robin = is_ಸುತ್ತು_Robin(ಗಂಟು_STATE(ksCurSC));
    bool_t valid = validTimeoutHandler(ಗಂಟು_STATE(ksCurThread));

    ಆದರೆ (can_timeout_fault && !ಸುತ್ತು_robin && valid) {
        current_fault = seL4_Fault_Timeout_new(ಗಂಟು_STATE(ksCurSC)->scBadge);
        handleTimeout(ಗಂಟು_STATE(ksCurThread));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (refill_ready(ಗಂಟು_STATE(ksCurSC)) && refill_sufficient(ಗಂಟು_STATE(ksCurSC), 0)) {
        /* apply round robin */
        ಪ್ರತಿಜ್ಞಾ(!thread_state_get_tcbQueued(ಗಂಟು_STATE(ksCurThread)->tcbState));
        SCHED_APPEND_CURRENT_TCB;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* postpone until ready */
        postpone(ಗಂಟು_STATE(ksCurSC));
    }
}
#ಇಲ್ಲದಿದ್ದರೆ

ಶೂನ್ಯ timerTick(ಶೂನ್ಯ)
{
    ಆದರೆ (likely(thread_state_get_tsType(ಗಂಟು_STATE(ksCurThread)->tcbState) ==
               ThreadState_Running)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
        || thread_state_get_tsType(ಗಂಟು_STATE(ksCurThread)->tcbState) ==
        ThreadState_RunningVM
#ಆದರೆ_ಅಂತ್ಯ
       ) {
        ಆದರೆ (ಗಂಟು_STATE(ksCurThread)->tcb_ಸಮಯ_ತುಂಡು > 1) {
            ಗಂಟು_STATE(ksCurThread)->tcb_ಸಮಯ_ತುಂಡು--;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಂಟು_STATE(ksCurThread)->tcb_ಸಮಯ_ತುಂಡು = CONFIG_ಸಮಯ_ತುಂಡು;
            SCHED_APPEND_CURRENT_TCB;
            rescheduleRequired();
        }
    }

    ಆದರೆ (ಅಂಕೆ_Domains > 1) {
        ks_Domain_ಸಮಯ--;
        ಆದರೆ (ks_Domain_ಸಮಯ == 0) {
            rescheduleRequired();
        }
    }
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ rescheduleRequired(ಶೂನ್ಯ)
{
    ಆದರೆ (ಗಂಟು_STATE(ksSchedulerAction) != SchedulerAction_ResumeCurrentThread
        && ಗಂಟು_STATE(ksSchedulerAction) != Scheduler_Action_ಆರಿಸು_New_Thread
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        && isSchedulable(ಗಂಟು_STATE(ksSchedulerAction))
#ಆದರೆ_ಅಂತ್ಯ
       ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಪ್ರತಿಜ್ಞಾ(refill_sufficient(ಗಂಟು_STATE(ksSchedulerAction)->tcbSchedContext, 0));
        ಪ್ರತಿಜ್ಞಾ(refill_ready(ಗಂಟು_STATE(ksSchedulerAction)->tcbSchedContext));
#ಆದರೆ_ಅಂತ್ಯ
        SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(ಗಂಟು_STATE(ksSchedulerAction));
    }
    ಗಂಟು_STATE(ksSchedulerAction) = Scheduler_Action_ಆರಿಸು_New_Thread;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t ಶುದ್ಧ release_q_non_ಖಾಲಿಗಳು_and_ready(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಗಂಟು_STATE(ks_Release_ಸರತಿ.ತಲೆ) != ಖಾಲಿ
           && refill_ready(ಗಂಟು_STATE(ks_Release_ಸರತಿ.ತಲೆ)->tcbSchedContext);
}

ಸ್ಥಿರ ಶೂನ್ಯ tcb_Release_ಸರತಿಯಿಂದ_ತೆಗೆ(ಶೂನ್ಯ)
{
    ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE(ks_Release_ಸರತಿ.ತಲೆ) != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE(ks_Release_ಸರತಿ.ತಲೆ)->tcbSchedPrev == ಖಾಲಿ);
    SMP_COND_STATEMENT(ಪ್ರತಿಜ್ಞಾ(ಗಂಟು_STATE(ks_Release_ಸರತಿ.ತಲೆ)->tcbAffinity == getCurrentCPUIndex()));

    tcb_t *awakened = ಗಂಟು_STATE(ks_Release_ಸರತಿ.ತಲೆ);
    ಪ್ರತಿಜ್ಞಾ(awakened != ಗಂಟು_STATE(ksCurThread));
    tcbReleaseRemove(awakened);
    /* round robin threads should not be in the release queue */
    ಪ್ರತಿಜ್ಞಾ(!is_ಸುತ್ತು_Robin(awakened->tcbSchedContext));
    /* threads should wake up on the correct core */
    SMP_COND_STATEMENT(ಪ್ರತಿಜ್ಞಾ(awakened->tcbAffinity == getCurrentCPUIndex()));
    /* threads HEAD refill should always be >= MIN_BUDGET */
    ಪ್ರತಿಜ್ಞಾ(refill_sufficient(awakened->tcbSchedContext, 0));
    possible_Switch_ಗಮ್ಯ(awakened);
}

ಶೂನ್ಯ awaken(ಶೂನ್ಯ)
{
    ಆಗ (unlikely(release_q_non_ಖಾಲಿಗಳು_and_ready())) {
        tcb_Release_ಸರತಿಯಿಂದ_ತೆಗೆ();
    }
}
#ಆದರೆ_ಅಂತ್ಯ
