/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/objecttype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/objecttype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/notification.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/endpoint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/cnode.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/interrupt.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਅਨ੍ਤਰ੍ਭਾਵ <object/schedcontext.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/schedcontrol.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <object/tcb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/untyped.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

ਸ਼ਬਦ_t get_Object_ਮਾਪ(ਸ਼ਬਦ_t t, ਸ਼ਬਦ_t user_Obj_ਮਾਪ)
{
    ਜੇ (t >= se_L4_Non_Arch_Object_Type_ਗਿਣਤੀ) {
        ਵਾਪਸ Arch_get_Object_ਮਾਪ(t);
    } ਨਹੀਂਤਾਂ {
        ਚੋਣ (t) {
        ਹਾਲਤ seL4_TCBObject:
            ਵਾਪਸ seL4_TCBBits;
        ਹਾਲਤ seL4_EndpointObject:
            ਵਾਪਸ se_L4_Endpoint_ਬਿੱਟ;
        ਹਾਲਤ seL4_NotificationObject:
            ਵਾਪਸ se_L4_Notification_ਬਿੱਟ;
        ਹਾਲਤ se_L4_Cap_ਸਾਰਣੀ_Object:
            ਵਾਪਸ se_L4_ਥਾਂ_ਬਿੱਟ + user_Obj_ਮਾਪ;
        ਹਾਲਤ seL4_UntypedObject:
            ਵਾਪਸ user_Obj_ਮਾਪ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਹਾਲਤ seL4_SchedContextObject:
            ਵਾਪਸ user_Obj_ਮਾਪ;
        ਹਾਲਤ se_L4_ਜਵਾਬ_Object:
            ਵਾਪਸ se_L4_ਜਵਾਬ_ਬਿੱਟ;
#ਜੇ_ਅੰਤ
        ਮੂਲ:
            fail("Invalid object type");
            ਵਾਪਸ 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *ਥਾਂ, cap_t cap)
{
    deriveCap_ret_t ret;

    ਜੇ (isArchCap(cap)) {
        ਵਾਪਸ Arch_deriveCap(ਥਾਂ, cap);
    }

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_zombie_cap:
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ਰੋਕੋ;

    ਹਾਲਤ cap_irq_control_cap:
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ਰੋਕੋ;

    ਹਾਲਤ cap_untyped_cap:
        ret.ਸਥਿਤੀ = ensureNoChildren(ਥਾਂ);
        ਜੇ (ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } ਨਹੀਂਤਾਂ {
            ret.cap = cap;
        }
        ਰੋਕੋ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_ਜਵਾਬ_cap:
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ret.cap = cap;
    }

    ਵਾਪਸ ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    ਜੇ (isArchCap(cap)) {
        ਵਾਪਸ Arch_finaliseCap(cap, final);
    }

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_endpoint_cap:
        ਜੇ (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ਵਾਪਸ fc_ret;

    ਹਾਲਤ cap_notification_cap:
        ਜੇ (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#ਜੇ_ਅੰਤ
            unbindMaybeNotification(ntfn);
            cancel_All_ਸੰਕੇਤ(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ਵਾਪਸ fc_ret;

    ਹਾਲਤ cap_ਜਵਾਬ_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਜੇ (final) {
            ਜਵਾਬ_t *ਜਵਾਬ = ਜਵਾਬ_PTR(cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Ptr(cap));
            ਜੇ (ਜਵਾਬ && ਜਵਾਬ->ਜਵਾਬ_TCB) {
                tcb_t *tcb = ਜਵਾਬ->ਜਵਾਬ_TCB;
                ਚੋਣ (thread_state_get_tsType(tcb->tcbState)) {
                ਹਾਲਤ Thread_State_Blocked_On_ਜਵਾਬ:
                    ਜਵਾਬ_remove(ਜਵਾਬ, tcb);
                    ਰੋਕੋ;
                ਹਾਲਤ ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    ਰੋਕੋ;
                ਮੂਲ:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ਵਾਪਸ fc_ret;
#ਜੇ_ਅੰਤ
    ਹਾਲਤ cap_null_cap:
    ਹਾਲਤ cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ਵਾਪਸ fc_ret;
    }

    ਜੇ (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_cnode_cap: {
        ਜੇ (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            ਵਾਪਸ fc_ret;
        }
        ਰੋਕੋ;
    }

    ਹਾਲਤ cap_thread_cap: {
        ਜੇ (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            ਜੇ (sc) {
                schedContext_unbindTCB(sc);
                ਜੇ (sc->scYieldFrom) {
                    sched_Context_complete_Yield_ਮੰਜ਼ਿਲ(sc->scYieldFrom);
                }
            }
#ਜੇ_ਅੰਤ
            suspend(tcb);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
            tcbDebugRemove(tcb);
#ਜੇ_ਅੰਤ
            Arch_prepareThreadDelete(tcb);
            fc_ret.remainder =
                Zombie_new(
                    tcbArchCNodeEntries,
                    ZombieType_ZombieTCB,
                    CTE_REF(cte_ptr)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            ਵਾਪਸ fc_ret;
        }
        ਰੋਕੋ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_sched_context_cap:
        ਜੇ (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            sched_Context_unbind_ਜਵਾਬ(sc);
            ਜੇ (sc->scYieldFrom) {
                sched_Context_complete_Yield_ਮੰਜ਼ਿਲ(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            ਵਾਪਸ fc_ret;
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ

    ਹਾਲਤ cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        ਵਾਪਸ fc_ret;

    ਹਾਲਤ cap_irq_handler_cap:
        ਜੇ (final) {
            irq_t irq = IDX_ਮੰਜ਼ਿਲ_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ਵਾਪਸ fc_ret;
        }
        ਰੋਕੋ;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ਵਾਪਸ fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_endpoint_cap:
        ਵਾਪਸ cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_cap_Can_Grant_ਜਵਾਬ(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    ਮੂਲ:
        ਵਾਪਸ false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ਚੋਣ (cap_get_capType(cap_a)) {
    ਹਾਲਤ cap_untyped_cap:
        ਜੇ (cap_get_capIsPhysical(cap_b)) {
            ਸ਼ਬਦ_t a_ਅਧਾਰ, b_ਅਧਾਰ, a_ਸਿਖਰ, b_ਸਿਖਰ;

            a_ਅਧਾਰ = (ਸ਼ਬਦ_t)ਸ਼ਬਦ_PTR(cap_untyped_cap_get_capPtr(cap_a));
            b_ਅਧਾਰ = (ਸ਼ਬਦ_t)cap_get_capPtr(cap_b);

            a_ਸਿਖਰ = a_ਅਧਾਰ + ਨਕਾਬ(cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(cap_a));
            b_ਸਿਖਰ = b_ਅਧਾਰ + ਨਕਾਬ(cap_get_cap_ਮਾਪ_ਬਿੱਟ(cap_b));

            ਵਾਪਸ (a_ਅਧਾਰ <= b_ਅਧਾਰ) && (b_ਸਿਖਰ <= a_ਸਿਖਰ) && (b_ਅਧਾਰ <= b_ਸਿਖਰ);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_endpoint_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_endpoint_cap) {
            ਵਾਪਸ cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_notification_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_notification_cap) {
            ਵਾਪਸ cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_cnode_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_cnode_cap) {
            ਵਾਪਸ (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_thread_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_thread_cap) {
            ਵਾਪਸ cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਜਵਾਬ_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ਜਵਾਬ_cap) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
            ਵਾਪਸ cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Ptr(cap_a) ==
                   cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Ptr(cap_b);
#ਨਹੀਂਤਾਂ
            ਵਾਪਸ cap_ਜਵਾਬ_cap_get_cap_TCBPtr(cap_a) ==
                   cap_ਜਵਾਬ_cap_get_cap_TCBPtr(cap_b);
#ਜੇ_ਅੰਤ
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_domain_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_domain_cap) {
            ਵਾਪਸ true;
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_irq_control_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            ਵਾਪਸ true;
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_irq_handler_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            ਵਾਪਸ (ਸ਼ਬਦ_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (ਸ਼ਬਦ_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_sched_context_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_sched_context_cap) {
            ਵਾਪਸ (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_cap_SCSize_ਬਿੱਟ(cap_a) ==
                    cap_sched_context_cap_get_cap_SCSize_ਬਿੱਟ(cap_b));
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_sched_control_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_sched_control_cap) {
            ਵਾਪਸ true;
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        ਜੇ (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            ਵਾਪਸ Arch_sameRegionAs(cap_a, cap_b);
        }
        ਰੋਕੋ;
    }

    ਵਾਪਸ false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ਜੇ (cap_get_capType(cap_a) == cap_untyped_cap) {
        ਵਾਪਸ false;
    }
    ਜੇ (cap_get_capType(cap_a) == cap_irq_control_cap) {
        ਵਾਪਸ false;
    }
    ਜੇ (isArchCap(cap_a) && isArchCap(cap_b)) {
        ਵਾਪਸ Arch_sameObjectAs(cap_a, cap_b);
    }
    ਵਾਪਸ sameRegionAs(cap_a, cap_b);
}

cap_t CONST update_Cap_ਅੰਕੜੇ(bool_t preserve, ਸ਼ਬਦ_t new_ਅੰਕੜੇ, cap_t cap)
{
    ਜੇ (isArchCap(cap)) {
        ਵਾਪਸ Arch_update_Cap_ਅੰਕੜੇ(preserve, new_ਅੰਕੜੇ, cap);
    }

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_endpoint_cap:
        ਜੇ (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            ਵਾਪਸ cap_endpoint_cap_ਸੈੱਟ_cap_EPBadge(cap, new_ਅੰਕੜੇ);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ cap_null_cap_new();
        }

    ਹਾਲਤ cap_notification_cap:
        ਜੇ (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            ਵਾਪਸ cap_notification_cap_ਸੈੱਟ_cap_Ntfn_Badge(cap, new_ਅੰਕੜੇ);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ cap_null_cap_new();
        }

    ਹਾਲਤ cap_cnode_cap: {
        ਸ਼ਬਦ_t guard, guard_ਮਾਪ;
        se_L4_CNode_Cap_ਅੰਕੜੇ_t w = { .ਸ਼ਬਦ = { new_ਅੰਕੜੇ } };

        guard_ਮਾਪ = se_L4_CNode_Cap_ਅੰਕੜੇ_get_guard_ਮਾਪ(w);

        ਜੇ (guard_ਮਾਪ + cap_cnode_cap_get_capCNodeRadix(cap) > ਸ਼ਬਦ_ਬਿੱਟ) {
            ਵਾਪਸ cap_null_cap_new();
        } ਨਹੀਂਤਾਂ {
            cap_t new_cap;

            guard = se_L4_CNode_Cap_ਅੰਕੜੇ_get_guard(w) & ਨਕਾਬ(guard_ਮਾਪ);
            new_cap = cap_cnode_cap_ਸੈੱਟ_cap_CNode_Guard(cap, guard);
            new_cap = cap_cnode_cap_ਸੈੱਟ_cap_CNode_Guard_ਮਾਪ(new_cap,
                                                          guard_ਮਾਪ);

            ਵਾਪਸ new_cap;
        }
    }

    ਮੂਲ:
        ਵਾਪਸ cap;
    }
}

cap_t CONST ਨਕਾਬ_Cap_Rights(seL4_CapRights_t cap_rights, cap_t cap)
{
    ਜੇ (isArchCap(cap)) {
        ਵਾਪਸ Arch_ਨਕਾਬ_Cap_Rights(cap_rights, cap);
    }

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_null_cap:
    ਹਾਲਤ cap_domain_cap:
    ਹਾਲਤ cap_cnode_cap:
    ਹਾਲਤ cap_untyped_cap:
    ਹਾਲਤ cap_irq_control_cap:
    ਹਾਲਤ cap_irq_handler_cap:
    ਹਾਲਤ cap_zombie_cap:
    ਹਾਲਤ cap_thread_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_sched_context_cap:
    ਹਾਲਤ cap_sched_control_cap:
#ਜੇ_ਅੰਤ
        ਵਾਪਸ cap;

    ਹਾਲਤ cap_endpoint_cap: {
        cap_t new_cap;

        new_cap = cap_endpoint_cap_ਸੈੱਟ_cap_Can_Send(
                      cap, cap_endpoint_cap_get_capCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_endpoint_cap_ਸੈੱਟ_cap_Can_Receive(
                      new_cap, cap_endpoint_cap_get_capCanReceive(cap) &
                      seL4_CapRights_get_capAllowRead(cap_rights));
        new_cap = cap_endpoint_cap_ਸੈੱਟ_cap_Can_Grant(
                      new_cap, cap_endpoint_cap_get_capCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        new_cap = cap_endpoint_cap_ਸੈੱਟ_cap_Can_Grant_ਜਵਾਬ(
                      new_cap, cap_endpoint_cap_get_cap_Can_Grant_ਜਵਾਬ(cap) &
                      se_L4_Cap_Rights_get_cap_Allow_Grant_ਜਵਾਬ(cap_rights));

        ਵਾਪਸ new_cap;
    }

    ਹਾਲਤ cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_ਸੈੱਟ_cap_Ntfn_Can_Send(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_ਸੈੱਟ_cap_Ntfn_Can_Receive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        ਵਾਪਸ new_cap;
    }
    ਹਾਲਤ cap_ਜਵਾਬ_cap: {
        cap_t new_cap;

        new_cap = cap_ਜਵਾਬ_cap_ਸੈੱਟ_cap_ਜਵਾਬ_Can_Grant(
                      cap, cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Can_Grant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        ਵਾਪਸ new_cap;
    }


    ਮੂਲ:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, ਖਾਲੀ *region_ਅਧਾਰ, ਸ਼ਬਦ_t user_ਮਾਪ, bool_t device_ਸਮ੍ਰਿਤੀ)
{
    /* Handle architecture-specific objects. */
    ਜੇ (t >= (object_t) se_L4_Non_Arch_Object_Type_ਗਿਣਤੀ) {
        ਵਾਪਸ Arch_createObject(t, region_ਅਧਾਰ, user_ਮਾਪ, device_ਸਮ੍ਰਿਤੀ);
    }

    /* Create objects. */
    ਚੋਣ ((api_object_t)t) {
    ਹਾਲਤ seL4_TCBObject: {
        tcb_t *tcb;
        tcb = TCB_PTR((ਸ਼ਬਦ_t)region_ਅਧਾਰ + TCB_ਵਿੱਥ);
        /** AUXUPD: "(True, ptr_retyps 1
          (Ptr ((ptr_val \<acute>tcb) - ctcb_offset) :: (cte_C[5]) ptr)
            o (ptr_retyp \<acute>tcb))" */

        /* Setup non-zero parts of the TCB. */

        Arch_initContext(&tcb->tcbArch.tcbContext);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        tcb->tcb_ਸਮਾਂ_ਟੁਕੜਾ = CONFIG_ਸਮਾਂ_ਟੁਕੜਾ;
#ਜੇ_ਅੰਤ
        tcb->tcbDomain = ksCurDomain;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        /* Initialize the new TCB to the current core */
        SMP_COND_STATEMENT(tcb->tcbAffinity = getCurrentCPUIndex());
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_ਲੰਮਾਈ);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(ਗੰਢ_STATE(ksCurThread))->tcbName, TCB_NAME_ਲੰਮਾਈ);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_ਲੰਮਾਈ);
        tcbDebugAppend(tcb);
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */

        ਵਾਪਸ cap_thread_cap_new(TCB_REF(tcb));
    }

    ਹਾਲਤ seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        ਵਾਪਸ cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(region_ਅਧਾਰ));

    ਹਾਲਤ seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        ਵਾਪਸ cap_notification_cap_new(0, true, true,
                                        NTFN_REF(region_ਅਧਾਰ));

    ਹਾਲਤ se_L4_Cap_ਸਾਰਣੀ_Object:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        ਵਾਪਸ cap_cnode_cap_new(user_ਮਾਪ, 0, 0, CTE_REF(region_ਅਧਾਰ));

    ਹਾਲਤ seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        ਵਾਪਸ cap_untyped_cap_new(0, !!device_ਸਮ੍ਰਿਤੀ, user_ਮਾਪ, ਸ਼ਬਦ_REF(region_ਅਧਾਰ));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        ਵਾਪਸ cap_sched_context_cap_new(SC_REF(region_ਅਧਾਰ), user_ਮਾਪ);

    ਹਾਲਤ se_L4_ਜਵਾਬ_Object:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        ਵਾਪਸ cap_ਜਵਾਬ_cap_new(ਜਵਾਬ_REF(region_ਅਧਾਰ), true);
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        fail("Invalid object type");
    }
}

ਖਾਲੀ createNewObjects(object_t t, cte_t *ਮਾਪਾ,
                      cte_t *destCNode, ਸ਼ਬਦ_t dest_ਵਿੱਥ, ਸ਼ਬਦ_t dest_ਲੰਮਾਈ,
                      ਖਾਲੀ *region_ਅਧਾਰ, ਸ਼ਬਦ_t user_ਮਾਪ, bool_t device_ਸਮ੍ਰਿਤੀ)
{
    ਸ਼ਬਦ_t object_ਮਾਪ;
    ਖਾਲੀ *ਅਗਲਾ_Free_Area;
    ਸ਼ਬਦ_t i;
    ਸ਼ਬਦ_t ਕੁੱਲ_Object_ਮਾਪ ਅਣਵਰਤਿਆ;

    /* ghost check that we're visiting less bytes than the max object size */
    object_ਮਾਪ = get_Object_ਮਾਪ(t, user_ਮਾਪ);
    ਕੁੱਲ_Object_ਮਾਪ = dest_ਲੰਮਾਈ << object_ਮਾਪ;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    ਅਗਲਾ_Free_Area = region_ਅਧਾਰ;
    ਲਈ (i = 0; i < dest_ਲੰਮਾਈ; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (ਖਾਲੀ *)((ਸ਼ਬਦ_t)ਅਗਲਾ_Free_Area + (i << object_ਮਾਪ)), user_ਮਾਪ, device_ਸਮ੍ਰਿਤੀ);

        /* Insert the cap into the user's cspace. */
        ਪਾਓ_New_Cap(ਮਾਪਾ, &destCNode[dest_ਵਿੱਥ + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
exception_t decodeInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                             cptr_t cap_ਸੂਚਕਾਂਕ, cte_t *ਥਾਂ, cap_t cap,
                             bool_t ਖੰਡ_2, bool_t call,
                             bool_t canDonate, bool_t ਪਹਿਲਾ_Phase, ਸ਼ਬਦ_t *ਬਫਰ)
#ਨਹੀਂਤਾਂ
exception_t decodeInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                             cptr_t cap_ਸੂਚਕਾਂਕ, cte_t *ਥਾਂ, cap_t cap,
                             bool_t ਖੰਡ_2, bool_t call,
                             ਸ਼ਬਦ_t *ਬਫਰ)
#ਜੇ_ਅੰਤ
{
    ਜੇ (isArchCap(cap)) {
        ਵਾਪਸ Arch_decodeInvocation(invLabel, ਲੰਮਾਈ, cap_ਸੂਚਕਾਂਕ,
                                     ਥਾਂ, cap, call, ਬਫਰ);
    }

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", cap_ਸੂਚਕਾਂਕ);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;

    ਹਾਲਤ cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", cap_ਸੂਚਕਾਂਕ);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;

    ਹਾਲਤ cap_endpoint_cap:
        ਜੇ (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      cap_ਸੂਚਕਾਂਕ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਵਾਪਸ performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_ਜਵਾਬ(cap), ਖੰਡ_2, call, canDonate);
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_ਜਵਾਬ(cap), ਖੰਡ_2, call);
#ਜੇ_ਅੰਤ

    ਹਾਲਤ cap_notification_cap: {
        ਜੇ (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      cap_ਸੂਚਕਾਂਕ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_ਜਵਾਬ_cap:
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_Invocation_ਜਵਾਬ(
                   ਗੰਢ_STATE(ksCurThread),
                   ਜਵਾਬ_PTR(cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Ptr(cap)),
                   cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Can_Grant(cap));
#ਨਹੀਂਤਾਂ
    ਹਾਲਤ cap_ਜਵਾਬ_cap:
        ਜੇ (unlikely(cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Master(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      cap_ਸੂਚਕਾਂਕ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_Invocation_ਜਵਾਬ(
                   TCB_PTR(cap_ਜਵਾਬ_cap_get_cap_TCBPtr(cap)), ਥਾਂ,
                   cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Can_Grant(cap));

#ਜੇ_ਅੰਤ

    ਹਾਲਤ cap_thread_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਜੇ (unlikely(ਪਹਿਲਾ_Phase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਜੇ_ਅੰਤ
        ਵਾਪਸ decodeTCBInvocation(invLabel, ਲੰਮਾਈ, cap, ਥਾਂ, call, ਬਫਰ);

    ਹਾਲਤ cap_domain_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਜੇ (unlikely(ਪਹਿਲਾ_Phase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਜੇ_ਅੰਤ
        ਵਾਪਸ decodeDomainInvocation(invLabel, ਲੰਮਾਈ, ਬਫਰ);

    ਹਾਲਤ cap_cnode_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਜੇ (unlikely(ਪਹਿਲਾ_Phase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਜੇ_ਅੰਤ
        ਵਾਪਸ decodeCNodeInvocation(invLabel, ਲੰਮਾਈ, cap, ਬਫਰ);

    ਹਾਲਤ cap_untyped_cap:
        ਵਾਪਸ decodeUntypedInvocation(invLabel, ਲੰਮਾਈ, ਥਾਂ, cap, call, ਬਫਰ);

    ਹਾਲਤ cap_irq_control_cap:
        ਵਾਪਸ decodeIRQControlInvocation(invLabel, ਲੰਮਾਈ, ਥਾਂ, ਬਫਰ);

    ਹਾਲਤ cap_irq_handler_cap:
        ਵਾਪਸ decodeIRQHandlerInvocation(invLabel,
                                          IDX_ਮੰਜ਼ਿਲ_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_sched_control_cap:
        ਜੇ (unlikely(ਪਹਿਲਾ_Phase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ decodeSchedControlInvocation(invLabel, cap, ਲੰਮਾਈ, ਬਫਰ);

    ਹਾਲਤ cap_sched_context_cap:
        ਜੇ (unlikely(ਪਹਿਲਾ_Phase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        ਵਾਪਸ decodeSchedContextInvocation(invLabel, sc, call);
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        fail("Invalid cap type");
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, ਸ਼ਬਦ_t badge,
                                       bool_t canGrant, bool_t can_Grant_ਜਵਾਬ,
                                       bool_t ਖੰਡ_2, bool_t call, bool_t canDonate)
{
    sendIPC(ਖੰਡ_2, call, badge, canGrant, can_Grant_ਜਵਾਬ, canDonate, ਗੰਢ_STATE(ksCurThread), ep);

    ਵਾਪਸ EXCEPTION_NONE;
}
#ਨਹੀਂਤਾਂ
exception_t performInvocation_Endpoint(endpoint_t *ep, ਸ਼ਬਦ_t badge,
                                       bool_t canGrant, bool_t can_Grant_ਜਵਾਬ,
                                       bool_t ਖੰਡ_2, bool_t call)
{
    sendIPC(ਖੰਡ_2, call, badge, canGrant, can_Grant_ਜਵਾਬ, ਗੰਢ_STATE(ksCurThread), ep);

    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ

exception_t performInvocation_Notification(notification_t *ntfn, ਸ਼ਬਦ_t badge)
{
    sendSignal(ntfn, badge);

    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
exception_t perform_Invocation_ਜਵਾਬ(tcb_t *thread, ਜਵਾਬ_t *ਜਵਾਬ, bool_t canGrant)
{
    do_ਜਵਾਬ_Transfer(thread, ਜਵਾਬ, canGrant);
    ਵਾਪਸ EXCEPTION_NONE;
}
#ਨਹੀਂਤਾਂ
exception_t perform_Invocation_ਜਵਾਬ(tcb_t *thread, cte_t *ਥਾਂ, bool_t canGrant)
{
    do_ਜਵਾਬ_Transfer(ਗੰਢ_STATE(ksCurThread), thread, ਥਾਂ, canGrant);
    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ

ਸ਼ਬਦ_t CONST cap_get_cap_ਮਾਪ_ਬਿੱਟ(cap_t cap)
{

    cap_ਟੈਗ_t ctag;

    ctag = cap_get_capType(cap);

    ਚੋਣ (ctag) {
    ਹਾਲਤ cap_untyped_cap:
        ਵਾਪਸ cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(cap);

    ਹਾਲਤ cap_endpoint_cap:
        ਵਾਪਸ se_L4_Endpoint_ਬਿੱਟ;

    ਹਾਲਤ cap_notification_cap:
        ਵਾਪਸ se_L4_Notification_ਬਿੱਟ;

    ਹਾਲਤ cap_cnode_cap:
        ਵਾਪਸ cap_cnode_cap_get_capCNodeRadix(cap) + se_L4_ਥਾਂ_ਬਿੱਟ;

    ਹਾਲਤ cap_thread_cap:
        ਵਾਪਸ seL4_TCBBits;

    ਹਾਲਤ cap_zombie_cap: {
        ਸ਼ਬਦ_t type = cap_zombie_cap_get_capZombieType(cap);
        ਜੇ (type == ZombieType_ZombieTCB) {
            ਵਾਪਸ seL4_TCBBits;
        }
        ਵਾਪਸ ZombieType_ZombieCNode(type) + se_L4_ਥਾਂ_ਬਿੱਟ;
    }

    ਹਾਲਤ cap_null_cap:
        ਵਾਪਸ 0;

    ਹਾਲਤ cap_domain_cap:
        ਵਾਪਸ 0;

    ਹਾਲਤ cap_ਜਵਾਬ_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਵਾਪਸ se_L4_ਜਵਾਬ_ਬਿੱਟ;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ

    ਹਾਲਤ cap_irq_control_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_sched_control_cap:
#ਜੇ_ਅੰਤ
        ਵਾਪਸ 0;

    ਹਾਲਤ cap_irq_handler_cap:
        ਵਾਪਸ 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_sched_context_cap:
        ਵਾਪਸ cap_sched_context_cap_get_cap_SCSize_ਬਿੱਟ(cap);
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        ਵਾਪਸ cap_get_arch_Cap_ਮਾਪ_ਬਿੱਟ(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_ਟੈਗ_t ctag;

    ctag = cap_get_capType(cap);

    ਚੋਣ (ctag) {
    ਹਾਲਤ cap_untyped_cap:
        ਵਾਪਸ true;

    ਹਾਲਤ cap_endpoint_cap:
        ਵਾਪਸ true;

    ਹਾਲਤ cap_notification_cap:
        ਵਾਪਸ true;

    ਹਾਲਤ cap_cnode_cap:
        ਵਾਪਸ true;

    ਹਾਲਤ cap_thread_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_sched_context_cap:
#ਜੇ_ਅੰਤ
        ਵਾਪਸ true;

    ਹਾਲਤ cap_zombie_cap:
        ਵਾਪਸ true;

    ਹਾਲਤ cap_domain_cap:
        ਵਾਪਸ false;

    ਹਾਲਤ cap_ਜਵਾਬ_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਵਾਪਸ true;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ false;
#ਜੇ_ਅੰਤ

    ਹਾਲਤ cap_irq_control_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_sched_control_cap:
#ਜੇ_ਅੰਤ
        ਵਾਪਸ false;

    ਹਾਲਤ cap_irq_handler_cap:
        ਵਾਪਸ false;

    ਮੂਲ:
        ਵਾਪਸ cap_get_archCapIsPhysical(cap);
    }
}

ਖਾਲੀ *CONST cap_get_capPtr(cap_t cap)
{
    cap_ਟੈਗ_t ctag;

    ctag = cap_get_capType(cap);

    ਚੋਣ (ctag) {
    ਹਾਲਤ cap_untyped_cap:
        ਵਾਪਸ ਸ਼ਬਦ_PTR(cap_untyped_cap_get_capPtr(cap));

    ਹਾਲਤ cap_endpoint_cap:
        ਵਾਪਸ EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    ਹਾਲਤ cap_notification_cap:
        ਵਾਪਸ NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    ਹਾਲਤ cap_cnode_cap:
        ਵਾਪਸ CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    ਹਾਲਤ cap_thread_cap:
        ਵਾਪਸ TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    ਹਾਲਤ cap_zombie_cap:
        ਵਾਪਸ CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    ਹਾਲਤ cap_domain_cap:
        ਵਾਪਸ ਨਿਰਾ;

    ਹਾਲਤ cap_ਜਵਾਬ_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        ਵਾਪਸ ਜਵਾਬ_PTR(cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Ptr(cap));
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    ਹਾਲਤ cap_irq_control_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_sched_control_cap:
#ਜੇ_ਅੰਤ
        ਵਾਪਸ ਨਿਰਾ;

    ਹਾਲਤ cap_irq_handler_cap:
        ਵਾਪਸ ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_sched_context_cap:
        ਵਾਪਸ SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        ਵਾਪਸ cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    ਜੇ (isArchCap(derivedCap)) {
        ਵਾਪਸ Arch_isCapRevocable(derivedCap, srcCap);
    }
    ਚੋਣ (cap_get_capType(derivedCap)) {
    ਹਾਲਤ cap_endpoint_cap:
        ਵਾਪਸ (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    ਹਾਲਤ cap_notification_cap:
        ਵਾਪਸ (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    ਹਾਲਤ cap_irq_handler_cap:
        ਵਾਪਸ (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    ਹਾਲਤ cap_untyped_cap:
        ਵਾਪਸ true;

    ਮੂਲ:
        ਵਾਪਸ false;
    }
}
