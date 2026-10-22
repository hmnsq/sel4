/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <arch/object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/notification.h>
#ಅನ್ತರ್ಭಾವ <object/endpoint.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <object/interrupt.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
#ಅನ್ತರ್ಭಾವ <object/schedcontext.h>
#ಅನ್ತರ್ಭಾವ <object/schedcontrol.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <object/tcb.h>
#ಅನ್ತರ್ಭಾವ <object/untyped.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <machine.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <string.h>

ಶಬ್ದ_t get_Object_ಗಾತ್ರ(ಶಬ್ದ_t t, ಶಬ್ದ_t user_Obj_ಗಾತ್ರ)
{
    ಆದರೆ (t >= se_L4_Non_Arch_Object_Type_ಎಣಿಕೆ) {
        ಹಿಂತಿರುಗಿ Arch_get_Object_ಗಾತ್ರ(t);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆಯ್ಕೆ (t) {
        ಸ್ಥಿತಿ seL4_TCBObject:
            ಹಿಂತಿರುಗಿ seL4_TCBBits;
        ಸ್ಥಿತಿ seL4_EndpointObject:
            ಹಿಂತಿರುಗಿ se_L4_Endpoint_ಬಿಟ್ಟುಗಳು;
        ಸ್ಥಿತಿ seL4_NotificationObject:
            ಹಿಂತಿರುಗಿ se_L4_Notification_ಬಿಟ್ಟುಗಳು;
        ಸ್ಥಿತಿ se_L4_Cap_ಕೋಷ್ಟಕ_Object:
            ಹಿಂತಿರುಗಿ se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು + user_Obj_ಗಾತ್ರ;
        ಸ್ಥಿತಿ seL4_UntypedObject:
            ಹಿಂತಿರುಗಿ user_Obj_ಗಾತ್ರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಸ್ಥಿತಿ seL4_SchedContextObject:
            ಹಿಂತಿರುಗಿ user_Obj_ಗಾತ್ರ;
        ಸ್ಥಿತಿ se_L4_ಉತ್ತರ_Object:
            ಹಿಂತಿರುಗಿ se_L4_ಉತ್ತರ_ಬಿಟ್ಟುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
        ಪೂರ್ವನಿಯೋಜಿತ:
            fail("Invalid object type");
            ಹಿಂತಿರುಗಿ 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *ಸ್ಥಾನ, cap_t cap)
{
    deriveCap_ret_t ret;

    ಆದರೆ (isArchCap(cap)) {
        ಹಿಂತಿರುಗಿ Arch_deriveCap(ಸ್ಥಾನ, cap);
    }

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_zombie_cap:
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_irq_control_cap:
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_untyped_cap:
        ret.ಸ್ಥಿತಿ_2 = ensureNoChildren(ಸ್ಥಾನ);
        ಆದರೆ (ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ret.cap = cap;
        }
        ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap:
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ret.cap = cap;
    }

    ಹಿಂತಿರುಗಿ ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    ಆದರೆ (isArchCap(cap)) {
        ಹಿಂತಿರುಗಿ Arch_finaliseCap(cap, final);
    }

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಆದರೆ (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ಹಿಂತಿರುಗಿ fc_ret;

    ಸ್ಥಿತಿ cap_notification_cap:
        ಆದರೆ (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#ಆದರೆ_ಅಂತ್ಯ
            unbindMaybeNotification(ntfn);
            cancel_All_ಸಂಕೇತಗಳು(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ಹಿಂತಿರುಗಿ fc_ret;

    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಆದರೆ (final) {
            ಉತ್ತರ_t *ಉತ್ತರ = ಉತ್ತರ_PTR(cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Ptr(cap));
            ಆದರೆ (ಉತ್ತರ && ಉತ್ತರ->ಉತ್ತರ_TCB) {
                tcb_t *tcb = ಉತ್ತರ->ಉತ್ತರ_TCB;
                ಆಯ್ಕೆ (thread_state_get_tsType(tcb->tcbState)) {
                ಸ್ಥಿತಿ Thread_State_Blocked_On_ಉತ್ತರ:
                    ಉತ್ತರ_remove(ಉತ್ತರ, tcb);
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    ನಿಲ್ಲಿಸು;
                ಪೂರ್ವನಿಯೋಜಿತ:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ಹಿಂತಿರುಗಿ fc_ret;
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ cap_null_cap:
    ಸ್ಥಿತಿ cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ಹಿಂತಿರುಗಿ fc_ret;
    }

    ಆದರೆ (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_cnode_cap: {
        ಆದರೆ (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            ಹಿಂತಿರುಗಿ fc_ret;
        }
        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ cap_thread_cap: {
        ಆದರೆ (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            ಆದರೆ (sc) {
                schedContext_unbindTCB(sc);
                ಆದರೆ (sc->scYieldFrom) {
                    sched_Context_complete_Yield_ಗಮ್ಯ(sc->scYieldFrom);
                }
            }
#ಆದರೆ_ಅಂತ್ಯ
            suspend(tcb);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
            tcbDebugRemove(tcb);
#ಆದರೆ_ಅಂತ್ಯ
            Arch_prepareThreadDelete(tcb);
            fc_ret.remainder =
                Zombie_new(
                    tcbArchCNodeEntries,
                    ZombieType_ZombieTCB,
                    CTE_REF(cte_ptr)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            ಹಿಂತಿರುಗಿ fc_ret;
        }
        ನಿಲ್ಲಿಸು;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_sched_context_cap:
        ಆದರೆ (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            sched_Context_unbind_ಉತ್ತರ(sc);
            ಆದರೆ (sc->scYieldFrom) {
                sched_Context_complete_Yield_ಗಮ್ಯ(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            ಹಿಂತಿರುಗಿ fc_ret;
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        ಹಿಂತಿರುಗಿ fc_ret;

    ಸ್ಥಿತಿ cap_irq_handler_cap:
        ಆದರೆ (final) {
            irq_t irq = IDX_ಗಮ್ಯ_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ಹಿಂತಿರುಗಿ fc_ret;
        }
        ನಿಲ್ಲಿಸು;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಹಿಂತಿರುಗಿ fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಹಿಂತಿರುಗಿ cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_cap_Can_Grant_ಉತ್ತರ(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ಆಯ್ಕೆ (cap_get_capType(cap_a)) {
    ಸ್ಥಿತಿ cap_untyped_cap:
        ಆದರೆ (cap_get_capIsPhysical(cap_b)) {
            ಶಬ್ದ_t a_ತಳ, b_ತಳ, a_ತುದಿ, b_ತುದಿ;

            a_ತಳ = (ಶಬ್ದ_t)ಶಬ್ದ_PTR(cap_untyped_cap_get_capPtr(cap_a));
            b_ತಳ = (ಶಬ್ದ_t)cap_get_capPtr(cap_b);

            a_ತುದಿ = a_ತಳ + ಮುಖವಾಡ(cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(cap_a));
            b_ತುದಿ = b_ತಳ + ಮುಖವಾಡ(cap_get_cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap_b));

            ಹಿಂತಿರುಗಿ (a_ತಳ <= b_ತಳ) && (b_ತುದಿ <= a_ತುದಿ) && (b_ತಳ <= b_ತುದಿ);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_endpoint_cap) {
            ಹಿಂತಿರುಗಿ cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_notification_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_notification_cap) {
            ಹಿಂತಿರುಗಿ cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_cnode_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_cnode_cap) {
            ಹಿಂತಿರುಗಿ (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_thread_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_thread_cap) {
            ಹಿಂತಿರುಗಿ cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ಉತ್ತರ_cap) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
            ಹಿಂತಿರುಗಿ cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Ptr(cap_a) ==
                   cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Ptr(cap_b);
#ಇಲ್ಲದಿದ್ದರೆ
            ಹಿಂತಿರುಗಿ cap_ಉತ್ತರ_cap_get_cap_TCBPtr(cap_a) ==
                   cap_ಉತ್ತರ_cap_get_cap_TCBPtr(cap_b);
#ಆದರೆ_ಅಂತ್ಯ
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_domain_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_domain_cap) {
            ಹಿಂತಿರುಗಿ true;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_irq_control_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            ಹಿಂತಿರುಗಿ true;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_irq_handler_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            ಹಿಂತಿರುಗಿ (ಶಬ್ದ_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (ಶಬ್ದ_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_sched_context_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_sched_context_cap) {
            ಹಿಂತಿರುಗಿ (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_cap_SCSize_ಬಿಟ್ಟುಗಳು(cap_a) ==
                    cap_sched_context_cap_get_cap_SCSize_ಬಿಟ್ಟುಗಳು(cap_b));
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_sched_control_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_sched_control_cap) {
            ಹಿಂತಿರುಗಿ true;
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಆದರೆ (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            ಹಿಂತಿರುಗಿ Arch_sameRegionAs(cap_a, cap_b);
        }
        ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ಆದರೆ (cap_get_capType(cap_a) == cap_untyped_cap) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಆದರೆ (cap_get_capType(cap_a) == cap_irq_control_cap) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಆದರೆ (isArchCap(cap_a) && isArchCap(cap_b)) {
        ಹಿಂತಿರುಗಿ Arch_sameObjectAs(cap_a, cap_b);
    }
    ಹಿಂತಿರುಗಿ sameRegionAs(cap_a, cap_b);
}

cap_t CONST update_Cap_ದತ್ತಾಂಶ(bool_t preserve, ಶಬ್ದ_t new_ದತ್ತಾಂಶ, cap_t cap)
{
    ಆದರೆ (isArchCap(cap)) {
        ಹಿಂತಿರುಗಿ Arch_update_Cap_ದತ್ತಾಂಶ(preserve, new_ದತ್ತಾಂಶ, cap);
    }

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಆದರೆ (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            ಹಿಂತಿರುಗಿ cap_endpoint_cap_ಗಣ_cap_EPBadge(cap, new_ದತ್ತಾಂಶ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }

    ಸ್ಥಿತಿ cap_notification_cap:
        ಆದರೆ (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            ಹಿಂತಿರುಗಿ cap_notification_cap_ಗಣ_cap_Ntfn_Badge(cap, new_ದತ್ತಾಂಶ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }

    ಸ್ಥಿತಿ cap_cnode_cap: {
        ಶಬ್ದ_t guard, guard_ಗಾತ್ರ;
        se_L4_CNode_Cap_ದತ್ತಾಂಶ_t w = { .ಪದಗಳು = { new_ದತ್ತಾಂಶ } };

        guard_ಗಾತ್ರ = se_L4_CNode_Cap_ದತ್ತಾಂಶ_get_guard_ಗಾತ್ರ(w);

        ಆದರೆ (guard_ಗಾತ್ರ + cap_cnode_cap_get_capCNodeRadix(cap) > ಶಬ್ದ_ಬಿಟ್ಟುಗಳು) {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        } ಇಲ್ಲದಿದ್ದರೆ {
            cap_t new_cap;

            guard = se_L4_CNode_Cap_ದತ್ತಾಂಶ_get_guard(w) & ಮುಖವಾಡ(guard_ಗಾತ್ರ);
            new_cap = cap_cnode_cap_ಗಣ_cap_CNode_Guard(cap, guard);
            new_cap = cap_cnode_cap_ಗಣ_cap_CNode_Guard_ಗಾತ್ರ(new_cap,
                                                          guard_ಗಾತ್ರ);

            ಹಿಂತಿರುಗಿ new_cap;
        }
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ cap;
    }
}

cap_t CONST ಮುಖವಾಡ_Cap_Rights(seL4_CapRights_t cap_rights, cap_t cap)
{
    ಆದರೆ (isArchCap(cap)) {
        ಹಿಂತಿರುಗಿ Arch_ಮುಖವಾಡ_Cap_Rights(cap_rights, cap);
    }

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_null_cap:
    ಸ್ಥಿತಿ cap_domain_cap:
    ಸ್ಥಿತಿ cap_cnode_cap:
    ಸ್ಥಿತಿ cap_untyped_cap:
    ಸ್ಥಿತಿ cap_irq_control_cap:
    ಸ್ಥಿತಿ cap_irq_handler_cap:
    ಸ್ಥಿತಿ cap_zombie_cap:
    ಸ್ಥಿತಿ cap_thread_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_sched_context_cap:
    ಸ್ಥಿತಿ cap_sched_control_cap:
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ cap;

    ಸ್ಥಿತಿ cap_endpoint_cap: {
        cap_t new_cap;

        new_cap = cap_endpoint_cap_ಗಣ_cap_Can_Send(
                      cap, cap_endpoint_cap_get_capCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_endpoint_cap_ಗಣ_cap_Can_Receive(
                      new_cap, cap_endpoint_cap_get_capCanReceive(cap) &
                      seL4_CapRights_get_capAllowRead(cap_rights));
        new_cap = cap_endpoint_cap_ಗಣ_cap_Can_Grant(
                      new_cap, cap_endpoint_cap_get_capCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        new_cap = cap_endpoint_cap_ಗಣ_cap_Can_Grant_ಉತ್ತರ(
                      new_cap, cap_endpoint_cap_get_cap_Can_Grant_ಉತ್ತರ(cap) &
                      se_L4_Cap_Rights_get_cap_Allow_Grant_ಉತ್ತರ(cap_rights));

        ಹಿಂತಿರುಗಿ new_cap;
    }

    ಸ್ಥಿತಿ cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_ಗಣ_cap_Ntfn_Can_Send(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_ಗಣ_cap_Ntfn_Can_Receive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        ಹಿಂತಿರುಗಿ new_cap;
    }
    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap: {
        cap_t new_cap;

        new_cap = cap_ಉತ್ತರ_cap_ಗಣ_cap_ಉತ್ತರ_Can_Grant(
                      cap, cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Can_Grant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        ಹಿಂತಿರುಗಿ new_cap;
    }


    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, ಶೂನ್ಯ *region_ತಳ, ಶಬ್ದ_t user_ಗಾತ್ರ, bool_t device_ಸ್ಮರಣೆ)
{
    /* Handle architecture-specific objects. */
    ಆದರೆ (t >= (object_t) se_L4_Non_Arch_Object_Type_ಎಣಿಕೆ) {
        ಹಿಂತಿರುಗಿ Arch_createObject(t, region_ತಳ, user_ಗಾತ್ರ, device_ಸ್ಮರಣೆ);
    }

    /* Create objects. */
    ಆಯ್ಕೆ ((api_object_t)t) {
    ಸ್ಥಿತಿ seL4_TCBObject: {
        tcb_t *tcb;
        tcb = TCB_PTR((ಶಬ್ದ_t)region_ತಳ + TCB_ಸ್ಥಾನಾಂತರ);
        /** AUXUPD: "(True, ptr_retyps 1
          (Ptr ((ptr_val \<acute>tcb) - ctcb_offset) :: (cte_C[5]) ptr)
            o (ptr_retyp \<acute>tcb))" */

        /* Setup non-zero parts of the TCB. */

        Arch_initContext(&tcb->tcbArch.tcbContext);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
        tcb->tcb_ಸಮಯ_ತುಂಡು = CONFIG_ಸಮಯ_ತುಂಡು;
#ಆದರೆ_ಅಂತ್ಯ
        tcb->tcbDomain = ksCurDomain;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
        /* Initialize the new TCB to the current core */
        SMP_COND_STATEMENT(tcb->tcbAffinity = getCurrentCPUIndex());
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_ಉದ್ದಳತೆ);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(ಗಂಟು_STATE(ksCurThread))->tcbName, TCB_NAME_ಉದ್ದಳತೆ);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_ಉದ್ದಳತೆ);
        tcbDebugAppend(tcb);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */

        ಹಿಂತಿರುಗಿ cap_thread_cap_new(TCB_REF(tcb));
    }

    ಸ್ಥಿತಿ seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        ಹಿಂತಿರುಗಿ cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(region_ತಳ));

    ಸ್ಥಿತಿ seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        ಹಿಂತಿರುಗಿ cap_notification_cap_new(0, true, true,
                                        NTFN_REF(region_ತಳ));

    ಸ್ಥಿತಿ se_L4_Cap_ಕೋಷ್ಟಕ_Object:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        ಹಿಂತಿರುಗಿ cap_cnode_cap_new(user_ಗಾತ್ರ, 0, 0, CTE_REF(region_ತಳ));

    ಸ್ಥಿತಿ seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        ಹಿಂತಿರುಗಿ cap_untyped_cap_new(0, !!device_ಸ್ಮರಣೆ, user_ಗಾತ್ರ, ಶಬ್ದ_REF(region_ತಳ));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        ಹಿಂತಿರುಗಿ cap_sched_context_cap_new(SC_REF(region_ತಳ), user_ಗಾತ್ರ);

    ಸ್ಥಿತಿ se_L4_ಉತ್ತರ_Object:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        ಹಿಂತಿರುಗಿ cap_ಉತ್ತರ_cap_new(ಉತ್ತರ_REF(region_ತಳ), true);
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid object type");
    }
}

ಶೂನ್ಯ createNewObjects(object_t t, cte_t *ಪೋಷಕ,
                      cte_t *destCNode, ಶಬ್ದ_t dest_ಸ್ಥಾನಾಂತರ, ಶಬ್ದ_t dest_ಉದ್ದಳತೆ,
                      ಶೂನ್ಯ *region_ತಳ, ಶಬ್ದ_t user_ಗಾತ್ರ, bool_t device_ಸ್ಮರಣೆ)
{
    ಶಬ್ದ_t object_ಗಾತ್ರ;
    ಶೂನ್ಯ *ಮುಂದಿನ_Free_Area;
    ಶಬ್ದ_t i;
    ಶಬ್ದ_t ಒಟ್ಟು_Object_ಗಾತ್ರ ಬಳಸದ;

    /* ghost check that we're visiting less bytes than the max object size */
    object_ಗಾತ್ರ = get_Object_ಗಾತ್ರ(t, user_ಗಾತ್ರ);
    ಒಟ್ಟು_Object_ಗಾತ್ರ = dest_ಉದ್ದಳತೆ << object_ಗಾತ್ರ;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    ಮುಂದಿನ_Free_Area = region_ತಳ;
    ಗಾಗಿ (i = 0; i < dest_ಉದ್ದಳತೆ; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (ಶೂನ್ಯ *)((ಶಬ್ದ_t)ಮುಂದಿನ_Free_Area + (i << object_ಗಾತ್ರ)), user_ಗಾತ್ರ, device_ಸ್ಮರಣೆ);

        /* Insert the cap into the user's cspace. */
        ಸೇರಿಸು_New_Cap(ಪೋಷಕ, &destCNode[dest_ಸ್ಥಾನಾಂತರ + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
exception_t decodeInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                             cptr_t cap_ಸೂಚ್ಯಂಕ, cte_t *ಸ್ಥಾನ, cap_t cap,
                             bool_t ಖಂಡ_2, bool_t call,
                             bool_t canDonate, bool_t ಮೊದಲ_Phase, ಶಬ್ದ_t *ಬಫರ್)
#ಇಲ್ಲದಿದ್ದರೆ
exception_t decodeInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                             cptr_t cap_ಸೂಚ್ಯಂಕ, cte_t *ಸ್ಥಾನ, cap_t cap,
                             bool_t ಖಂಡ_2, bool_t call,
                             ಶಬ್ದ_t *ಬಫರ್)
#ಆದರೆ_ಅಂತ್ಯ
{
    ಆದರೆ (isArchCap(cap)) {
        ಹಿಂತಿರುಗಿ Arch_decodeInvocation(invLabel, ಉದ್ದಳತೆ, cap_ಸೂಚ್ಯಂಕ,
                                     ಸ್ಥಾನ, cap, call, ಬಫರ್);
    }

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", cap_ಸೂಚ್ಯಂಕ);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;

    ಸ್ಥಿತಿ cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", cap_ಸೂಚ್ಯಂಕ);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;

    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಆದರೆ (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      cap_ಸೂಚ್ಯಂಕ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಹಿಂತಿರುಗಿ performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_ಉತ್ತರ(cap), ಖಂಡ_2, call, canDonate);
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_ಉತ್ತರ(cap), ಖಂಡ_2, call);
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ cap_notification_cap: {
        ಆದರೆ (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      cap_ಸೂಚ್ಯಂಕ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap:
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_Invocation_ಉತ್ತರ(
                   ಗಂಟು_STATE(ksCurThread),
                   ಉತ್ತರ_PTR(cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Ptr(cap)),
                   cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Can_Grant(cap));
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap:
        ಆದರೆ (unlikely(cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Master(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      cap_ಸೂಚ್ಯಂಕ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_Invocation_ಉತ್ತರ(
                   TCB_PTR(cap_ಉತ್ತರ_cap_get_cap_TCBPtr(cap)), ಸ್ಥಾನ,
                   cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Can_Grant(cap));

#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ cap_thread_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಆದರೆ (unlikely(ಮೊದಲ_Phase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ decodeTCBInvocation(invLabel, ಉದ್ದಳತೆ, cap, ಸ್ಥಾನ, call, ಬಫರ್);

    ಸ್ಥಿತಿ cap_domain_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಆದರೆ (unlikely(ಮೊದಲ_Phase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ decodeDomainInvocation(invLabel, ಉದ್ದಳತೆ, ಬಫರ್);

    ಸ್ಥಿತಿ cap_cnode_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಆದರೆ (unlikely(ಮೊದಲ_Phase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ decodeCNodeInvocation(invLabel, ಉದ್ದಳತೆ, cap, ಬಫರ್);

    ಸ್ಥಿತಿ cap_untyped_cap:
        ಹಿಂತಿರುಗಿ decodeUntypedInvocation(invLabel, ಉದ್ದಳತೆ, ಸ್ಥಾನ, cap, call, ಬಫರ್);

    ಸ್ಥಿತಿ cap_irq_control_cap:
        ಹಿಂತಿರುಗಿ decodeIRQControlInvocation(invLabel, ಉದ್ದಳತೆ, ಸ್ಥಾನ, ಬಫರ್);

    ಸ್ಥಿತಿ cap_irq_handler_cap:
        ಹಿಂತಿರುಗಿ decodeIRQHandlerInvocation(invLabel,
                                          IDX_ಗಮ್ಯ_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_sched_control_cap:
        ಆದರೆ (unlikely(ಮೊದಲ_Phase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ decodeSchedControlInvocation(invLabel, cap, ಉದ್ದಳತೆ, ಬಫರ್);

    ಸ್ಥಿತಿ cap_sched_context_cap:
        ಆದರೆ (unlikely(ಮೊದಲ_Phase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        ಹಿಂತಿರುಗಿ decodeSchedContextInvocation(invLabel, sc, call);
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid cap type");
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, ಶಬ್ದ_t badge,
                                       bool_t canGrant, bool_t can_Grant_ಉತ್ತರ,
                                       bool_t ಖಂಡ_2, bool_t call, bool_t canDonate)
{
    sendIPC(ಖಂಡ_2, call, badge, canGrant, can_Grant_ಉತ್ತರ, canDonate, ಗಂಟು_STATE(ksCurThread), ep);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಇಲ್ಲದಿದ್ದರೆ
exception_t performInvocation_Endpoint(endpoint_t *ep, ಶಬ್ದ_t badge,
                                       bool_t canGrant, bool_t can_Grant_ಉತ್ತರ,
                                       bool_t ಖಂಡ_2, bool_t call)
{
    sendIPC(ಖಂಡ_2, call, badge, canGrant, can_Grant_ಉತ್ತರ, ಗಂಟು_STATE(ksCurThread), ep);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ

exception_t performInvocation_Notification(notification_t *ntfn, ಶಬ್ದ_t badge)
{
    sendSignal(ntfn, badge);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
exception_t perform_Invocation_ಉತ್ತರ(tcb_t *thread, ಉತ್ತರ_t *ಉತ್ತರ, bool_t canGrant)
{
    do_ಉತ್ತರ_Transfer(thread, ಉತ್ತರ, canGrant);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಇಲ್ಲದಿದ್ದರೆ
exception_t perform_Invocation_ಉತ್ತರ(tcb_t *thread, cte_t *ಸ್ಥಾನ, bool_t canGrant)
{
    do_ಉತ್ತರ_Transfer(ಗಂಟು_STATE(ksCurThread), thread, ಸ್ಥಾನ, canGrant);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶಬ್ದ_t CONST cap_get_cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap_t cap)
{

    cap_ಟ್ಯಾಗ್_t ctag;

    ctag = cap_get_capType(cap);

    ಆಯ್ಕೆ (ctag) {
    ಸ್ಥಿತಿ cap_untyped_cap:
        ಹಿಂತಿರುಗಿ cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(cap);

    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಹಿಂತಿರುಗಿ se_L4_Endpoint_ಬಿಟ್ಟುಗಳು;

    ಸ್ಥಿತಿ cap_notification_cap:
        ಹಿಂತಿರುಗಿ se_L4_Notification_ಬಿಟ್ಟುಗಳು;

    ಸ್ಥಿತಿ cap_cnode_cap:
        ಹಿಂತಿರುಗಿ cap_cnode_cap_get_capCNodeRadix(cap) + se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು;

    ಸ್ಥಿತಿ cap_thread_cap:
        ಹಿಂತಿರುಗಿ seL4_TCBBits;

    ಸ್ಥಿತಿ cap_zombie_cap: {
        ಶಬ್ದ_t type = cap_zombie_cap_get_capZombieType(cap);
        ಆದರೆ (type == ZombieType_ZombieTCB) {
            ಹಿಂತಿರುಗಿ seL4_TCBBits;
        }
        ಹಿಂತಿರುಗಿ ZombieType_ZombieCNode(type) + se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು;
    }

    ಸ್ಥಿತಿ cap_null_cap:
        ಹಿಂತಿರುಗಿ 0;

    ಸ್ಥಿತಿ cap_domain_cap:
        ಹಿಂತಿರುಗಿ 0;

    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಹಿಂತಿರುಗಿ se_L4_ಉತ್ತರ_ಬಿಟ್ಟುಗಳು;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ cap_irq_control_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_sched_control_cap:
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ 0;

    ಸ್ಥಿತಿ cap_irq_handler_cap:
        ಹಿಂತಿರುಗಿ 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_sched_context_cap:
        ಹಿಂತಿರುಗಿ cap_sched_context_cap_get_cap_SCSize_ಬಿಟ್ಟುಗಳು(cap);
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ cap_get_arch_Cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_ಟ್ಯಾಗ್_t ctag;

    ctag = cap_get_capType(cap);

    ಆಯ್ಕೆ (ctag) {
    ಸ್ಥಿತಿ cap_untyped_cap:
        ಹಿಂತಿರುಗಿ true;

    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಹಿಂತಿರುಗಿ true;

    ಸ್ಥಿತಿ cap_notification_cap:
        ಹಿಂತಿರುಗಿ true;

    ಸ್ಥಿತಿ cap_cnode_cap:
        ಹಿಂತಿರುಗಿ true;

    ಸ್ಥಿತಿ cap_thread_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_sched_context_cap:
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ true;

    ಸ್ಥಿತಿ cap_zombie_cap:
        ಹಿಂತಿರುಗಿ true;

    ಸ್ಥಿತಿ cap_domain_cap:
        ಹಿಂತಿರುಗಿ false;

    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಹಿಂತಿರುಗಿ true;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ false;
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ cap_irq_control_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_sched_control_cap:
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ false;

    ಸ್ಥಿತಿ cap_irq_handler_cap:
        ಹಿಂತಿರುಗಿ false;

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ cap_get_archCapIsPhysical(cap);
    }
}

ಶೂನ್ಯ *CONST cap_get_capPtr(cap_t cap)
{
    cap_ಟ್ಯಾಗ್_t ctag;

    ctag = cap_get_capType(cap);

    ಆಯ್ಕೆ (ctag) {
    ಸ್ಥಿತಿ cap_untyped_cap:
        ಹಿಂತಿರುಗಿ ಶಬ್ದ_PTR(cap_untyped_cap_get_capPtr(cap));

    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಹಿಂತಿರುಗಿ EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    ಸ್ಥಿತಿ cap_notification_cap:
        ಹಿಂತಿರುಗಿ NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    ಸ್ಥಿತಿ cap_cnode_cap:
        ಹಿಂತಿರುಗಿ CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    ಸ್ಥಿತಿ cap_thread_cap:
        ಹಿಂತಿರುಗಿ TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    ಸ್ಥಿತಿ cap_zombie_cap:
        ಹಿಂತಿರುಗಿ CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    ಸ್ಥಿತಿ cap_domain_cap:
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        ಹಿಂತಿರುಗಿ ಉತ್ತರ_PTR(cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Ptr(cap));
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ cap_irq_control_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_sched_control_cap:
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಸ್ಥಿತಿ cap_irq_handler_cap:
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_sched_context_cap:
        ಹಿಂತಿರುಗಿ SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    ಆದರೆ (isArchCap(derivedCap)) {
        ಹಿಂತಿರುಗಿ Arch_isCapRevocable(derivedCap, srcCap);
    }
    ಆಯ್ಕೆ (cap_get_capType(derivedCap)) {
    ಸ್ಥಿತಿ cap_endpoint_cap:
        ಹಿಂತಿರುಗಿ (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    ಸ್ಥಿತಿ cap_notification_cap:
        ಹಿಂತಿರುಗಿ (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    ಸ್ಥಿತಿ cap_irq_handler_cap:
        ಹಿಂತಿರುಗಿ (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    ಸ್ಥಿತಿ cap_untyped_cap:
        ಹಿಂತಿರುಗಿ true;

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }
}
