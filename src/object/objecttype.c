/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>
#شامل <config.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <api/syscall.h>
#شامل <arch/object/objecttype.h>
#شامل <machine/io.h>
#شامل <object/objecttype.h>
#شامل <object/structures.h>
#شامل <object/notification.h>
#شامل <object/endpoint.h>
#شامل <object/cnode.h>
#شامل <object/interrupt.h>
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#شامل <object/schedcontext.h>
#شامل <object/schedcontrol.h>
#اگر_ختم
#شامل <object/tcb.h>
#شامل <object/untyped.h>
#شامل <model/statedata.h>
#شامل <kernel/thread.h>
#شامل <kernel/vspace.h>
#شامل <machine.h>
#شامل <util.h>
#شامل <string.h>

لفظ_t get_Object_سائز(لفظ_t t, لفظ_t user_Obj_سائز)
{
    اگر (t >= se_L4_Non_Arch_Object_Type_گنتی) {
        واپس Arch_get_Object_سائز(t);
    } ورنہ {
        انتخاب (t) {
        حالت seL4_TCBObject:
            واپس seL4_TCBBits;
        حالت seL4_EndpointObject:
            واپس se_L4_Endpoint_بٹ;
        حالت seL4_NotificationObject:
            واپس se_L4_Notification_بٹ;
        حالت se_L4_Cap_جدول_Object:
            واپس se_L4_خانہ_بٹ + user_Obj_سائز;
        حالت seL4_UntypedObject:
            واپس user_Obj_سائز;
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        حالت seL4_SchedContextObject:
            واپس user_Obj_سائز;
        حالت se_L4_جواب_Object:
            واپس se_L4_جواب_بٹ;
#اگر_ختم
        طےشدہ:
            fail("Invalid object type");
            واپس 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *خانہ, cap_t cap)
{
    deriveCap_ret_t ret;

    اگر (isArchCap(cap)) {
        واپس Arch_deriveCap(خانہ, cap);
    }

    انتخاب (cap_get_capType(cap)) {
    حالت cap_zombie_cap:
        ret.حالت_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        توڑو;

    حالت cap_irq_control_cap:
        ret.حالت_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        توڑو;

    حالت cap_untyped_cap:
        ret.حالت_2 = ensureNoChildren(خانہ);
        اگر (ret.حالت_2 != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } ورنہ {
            ret.cap = cap;
        }
        توڑو;

#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_جواب_cap:
        ret.حالت_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        توڑو;
#اگر_ختم
    طےشدہ:
        ret.حالت_2 = EXCEPTION_NONE;
        ret.cap = cap;
    }

    واپس ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    اگر (isArchCap(cap)) {
        واپس Arch_finaliseCap(cap, final);
    }

    انتخاب (cap_get_capType(cap)) {
    حالت cap_endpoint_cap:
        اگر (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        واپس fc_ret;

    حالت cap_notification_cap:
        اگر (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#اگر_ختم
            unbindMaybeNotification(ntfn);
            cancel_All_اشارات(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        واپس fc_ret;

    حالت cap_جواب_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        اگر (final) {
            جواب_t *جواب = جواب_PTR(cap_جواب_cap_get_cap_جواب_Ptr(cap));
            اگر (جواب && جواب->جواب_TCB) {
                tcb_t *tcb = جواب->جواب_TCB;
                انتخاب (thread_state_get_tsType(tcb->tcbState)) {
                حالت Thread_State_Blocked_On_جواب:
                    جواب_remove(جواب, tcb);
                    توڑو;
                حالت ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    توڑو;
                طےشدہ:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        واپس fc_ret;
#اگر_ختم
    حالت cap_null_cap:
    حالت cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        واپس fc_ret;
    }

    اگر (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    انتخاب (cap_get_capType(cap)) {
    حالت cap_cnode_cap: {
        اگر (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            واپس fc_ret;
        }
        توڑو;
    }

    حالت cap_thread_cap: {
        اگر (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            اگر (sc) {
                schedContext_unbindTCB(sc);
                اگر (sc->scYieldFrom) {
                    sched_Context_complete_Yield_مقصد(sc->scYieldFrom);
                }
            }
#اگر_ختم
            suspend(tcb);
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
            tcbDebugRemove(tcb);
#اگر_ختم
            Arch_prepareThreadDelete(tcb);
            fc_ret.remainder =
                Zombie_new(
                    tcbArchCNodeEntries,
                    ZombieType_ZombieTCB,
                    CTE_REF(cte_ptr)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            واپس fc_ret;
        }
        توڑو;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_sched_context_cap:
        اگر (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            sched_Context_unbind_جواب(sc);
            اگر (sc->scYieldFrom) {
                sched_Context_complete_Yield_مقصد(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            واپس fc_ret;
        }
        توڑو;
#اگر_ختم

    حالت cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        واپس fc_ret;

    حالت cap_irq_handler_cap:
        اگر (final) {
            irq_t irq = IDX_مقصد_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            واپس fc_ret;
        }
        توڑو;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    واپس fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_endpoint_cap:
        واپس cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_cap_Can_Grant_جواب(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    طےشدہ:
        واپس false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    انتخاب (cap_get_capType(cap_a)) {
    حالت cap_untyped_cap:
        اگر (cap_get_capIsPhysical(cap_b)) {
            لفظ_t a_بنیاد, b_بنیاد, a_چوٹی, b_چوٹی;

            a_بنیاد = (لفظ_t)لفظ_PTR(cap_untyped_cap_get_capPtr(cap_a));
            b_بنیاد = (لفظ_t)cap_get_capPtr(cap_b);

            a_چوٹی = a_بنیاد + نقاب(cap_untyped_cap_get_cap_بلاک_سائز(cap_a));
            b_چوٹی = b_بنیاد + نقاب(cap_get_cap_سائز_بٹ(cap_b));

            واپس (a_بنیاد <= b_بنیاد) && (b_چوٹی <= a_چوٹی) && (b_بنیاد <= b_چوٹی);
        }
        توڑو;

    حالت cap_endpoint_cap:
        اگر (cap_get_capType(cap_b) == cap_endpoint_cap) {
            واپس cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        توڑو;

    حالت cap_notification_cap:
        اگر (cap_get_capType(cap_b) == cap_notification_cap) {
            واپس cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        توڑو;

    حالت cap_cnode_cap:
        اگر (cap_get_capType(cap_b) == cap_cnode_cap) {
            واپس (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        توڑو;

    حالت cap_thread_cap:
        اگر (cap_get_capType(cap_b) == cap_thread_cap) {
            واپس cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        توڑو;

    حالت cap_جواب_cap:
        اگر (cap_get_capType(cap_b) == cap_جواب_cap) {
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
            واپس cap_جواب_cap_get_cap_جواب_Ptr(cap_a) ==
                   cap_جواب_cap_get_cap_جواب_Ptr(cap_b);
#ورنہ
            واپس cap_جواب_cap_get_cap_TCBPtr(cap_a) ==
                   cap_جواب_cap_get_cap_TCBPtr(cap_b);
#اگر_ختم
        }
        توڑو;

    حالت cap_domain_cap:
        اگر (cap_get_capType(cap_b) == cap_domain_cap) {
            واپس true;
        }
        توڑو;

    حالت cap_irq_control_cap:
        اگر (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            واپس true;
        }
        توڑو;

    حالت cap_irq_handler_cap:
        اگر (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            واپس (لفظ_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (لفظ_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        توڑو;

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_sched_context_cap:
        اگر (cap_get_capType(cap_b) == cap_sched_context_cap) {
            واپس (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_cap_SCSize_بٹ(cap_a) ==
                    cap_sched_context_cap_get_cap_SCSize_بٹ(cap_b));
        }
        توڑو;
    حالت cap_sched_control_cap:
        اگر (cap_get_capType(cap_b) == cap_sched_control_cap) {
            واپس true;
        }
        توڑو;
#اگر_ختم
    طےشدہ:
        اگر (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            واپس Arch_sameRegionAs(cap_a, cap_b);
        }
        توڑو;
    }

    واپس false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    اگر (cap_get_capType(cap_a) == cap_untyped_cap) {
        واپس false;
    }
    اگر (cap_get_capType(cap_a) == cap_irq_control_cap) {
        واپس false;
    }
    اگر (isArchCap(cap_a) && isArchCap(cap_b)) {
        واپس Arch_sameObjectAs(cap_a, cap_b);
    }
    واپس sameRegionAs(cap_a, cap_b);
}

cap_t CONST update_Cap_ڈیٹا(bool_t preserve, لفظ_t new_ڈیٹا, cap_t cap)
{
    اگر (isArchCap(cap)) {
        واپس Arch_update_Cap_ڈیٹا(preserve, new_ڈیٹا, cap);
    }

    انتخاب (cap_get_capType(cap)) {
    حالت cap_endpoint_cap:
        اگر (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            واپس cap_endpoint_cap_سیٹ_cap_EPBadge(cap, new_ڈیٹا);
        } ورنہ {
            واپس cap_null_cap_new();
        }

    حالت cap_notification_cap:
        اگر (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            واپس cap_notification_cap_سیٹ_cap_Ntfn_Badge(cap, new_ڈیٹا);
        } ورنہ {
            واپس cap_null_cap_new();
        }

    حالت cap_cnode_cap: {
        لفظ_t guard, guard_سائز;
        se_L4_CNode_Cap_ڈیٹا_t w = { .الفاظ = { new_ڈیٹا } };

        guard_سائز = se_L4_CNode_Cap_ڈیٹا_get_guard_سائز(w);

        اگر (guard_سائز + cap_cnode_cap_get_capCNodeRadix(cap) > لفظ_بٹ) {
            واپس cap_null_cap_new();
        } ورنہ {
            cap_t new_cap;

            guard = se_L4_CNode_Cap_ڈیٹا_get_guard(w) & نقاب(guard_سائز);
            new_cap = cap_cnode_cap_سیٹ_cap_CNode_Guard(cap, guard);
            new_cap = cap_cnode_cap_سیٹ_cap_CNode_Guard_سائز(new_cap,
                                                          guard_سائز);

            واپس new_cap;
        }
    }

    طےشدہ:
        واپس cap;
    }
}

cap_t CONST نقاب_Cap_Rights(seL4_CapRights_t cap_rights, cap_t cap)
{
    اگر (isArchCap(cap)) {
        واپس Arch_نقاب_Cap_Rights(cap_rights, cap);
    }

    انتخاب (cap_get_capType(cap)) {
    حالت cap_null_cap:
    حالت cap_domain_cap:
    حالت cap_cnode_cap:
    حالت cap_untyped_cap:
    حالت cap_irq_control_cap:
    حالت cap_irq_handler_cap:
    حالت cap_zombie_cap:
    حالت cap_thread_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_sched_context_cap:
    حالت cap_sched_control_cap:
#اگر_ختم
        واپس cap;

    حالت cap_endpoint_cap: {
        cap_t new_cap;

        new_cap = cap_endpoint_cap_سیٹ_cap_Can_Send(
                      cap, cap_endpoint_cap_get_capCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_endpoint_cap_سیٹ_cap_Can_Receive(
                      new_cap, cap_endpoint_cap_get_capCanReceive(cap) &
                      seL4_CapRights_get_capAllowRead(cap_rights));
        new_cap = cap_endpoint_cap_سیٹ_cap_Can_Grant(
                      new_cap, cap_endpoint_cap_get_capCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        new_cap = cap_endpoint_cap_سیٹ_cap_Can_Grant_جواب(
                      new_cap, cap_endpoint_cap_get_cap_Can_Grant_جواب(cap) &
                      se_L4_Cap_Rights_get_cap_Allow_Grant_جواب(cap_rights));

        واپس new_cap;
    }

    حالت cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_سیٹ_cap_Ntfn_Can_Send(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_سیٹ_cap_Ntfn_Can_Receive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        واپس new_cap;
    }
    حالت cap_جواب_cap: {
        cap_t new_cap;

        new_cap = cap_جواب_cap_سیٹ_cap_جواب_Can_Grant(
                      cap, cap_جواب_cap_get_cap_جواب_Can_Grant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        واپس new_cap;
    }


    طےشدہ:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, خالی *region_بنیاد, لفظ_t user_سائز, bool_t device_حافظہ)
{
    /* Handle architecture-specific objects. */
    اگر (t >= (object_t) se_L4_Non_Arch_Object_Type_گنتی) {
        واپس Arch_createObject(t, region_بنیاد, user_سائز, device_حافظہ);
    }

    /* Create objects. */
    انتخاب ((api_object_t)t) {
    حالت seL4_TCBObject: {
        tcb_t *tcb;
        tcb = TCB_PTR((لفظ_t)region_بنیاد + TCB_انحراف);
        /** AUXUPD: "(True, ptr_retyps 1
          (Ptr ((ptr_val \<acute>tcb) - ctcb_offset) :: (cte_C[5]) ptr)
            o (ptr_retyp \<acute>tcb))" */

        /* Setup non-zero parts of the TCB. */

        Arch_initContext(&tcb->tcbArch.tcbContext);
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
        tcb->tcb_وقت_قاش = CONFIG_وقت_قاش;
#اگر_ختم
        tcb->tcbDomain = ksCurDomain;
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
        /* Initialize the new TCB to the current core */
        SMP_COND_STATEMENT(tcb->tcbAffinity = getCurrentCPUIndex());
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_لمبائی);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(گرہ_STATE(ksCurThread))->tcbName, TCB_NAME_لمبائی);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_لمبائی);
        tcbDebugAppend(tcb);
#اگر_ختم /* CONFIG_DEBUG_BUILD */

        واپس cap_thread_cap_new(TCB_REF(tcb));
    }

    حالت seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        واپس cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(region_بنیاد));

    حالت seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        واپس cap_notification_cap_new(0, true, true,
                                        NTFN_REF(region_بنیاد));

    حالت se_L4_Cap_جدول_Object:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        واپس cap_cnode_cap_new(user_سائز, 0, 0, CTE_REF(region_بنیاد));

    حالت seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        واپس cap_untyped_cap_new(0, !!device_حافظہ, user_سائز, لفظ_REF(region_بنیاد));

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        واپس cap_sched_context_cap_new(SC_REF(region_بنیاد), user_سائز);

    حالت se_L4_جواب_Object:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        واپس cap_جواب_cap_new(جواب_REF(region_بنیاد), true);
#اگر_ختم

    طےشدہ:
        fail("Invalid object type");
    }
}

خالی createNewObjects(object_t t, cte_t *والد,
                      cte_t *destCNode, لفظ_t dest_انحراف, لفظ_t dest_لمبائی,
                      خالی *region_بنیاد, لفظ_t user_سائز, bool_t device_حافظہ)
{
    لفظ_t object_سائز;
    خالی *اگلا_Free_Area;
    لفظ_t i;
    لفظ_t کل_Object_سائز غیر_مستعمل;

    /* ghost check that we're visiting less bytes than the max object size */
    object_سائز = get_Object_سائز(t, user_سائز);
    کل_Object_سائز = dest_لمبائی << object_سائز;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    اگلا_Free_Area = region_بنیاد;
    برائے (i = 0; i < dest_لمبائی; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (خالی *)((لفظ_t)اگلا_Free_Area + (i << object_سائز)), user_سائز, device_حافظہ);

        /* Insert the cap into the user's cspace. */
        داخل_کرو_New_Cap(والد, &destCNode[dest_انحراف + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t decodeInvocation(لفظ_t invLabel, لفظ_t لمبائی,
                             cptr_t cap_اشاریہ, cte_t *خانہ, cap_t cap,
                             bool_t بلاک_2, bool_t call,
                             bool_t canDonate, bool_t پہلا_Phase, لفظ_t *بفر)
#ورنہ
exception_t decodeInvocation(لفظ_t invLabel, لفظ_t لمبائی,
                             cptr_t cap_اشاریہ, cte_t *خانہ, cap_t cap,
                             bool_t بلاک_2, bool_t call,
                             لفظ_t *بفر)
#اگر_ختم
{
    اگر (isArchCap(cap)) {
        واپس Arch_decodeInvocation(invLabel, لمبائی, cap_اشاریہ,
                                     خانہ, cap, call, بفر);
    }

    انتخاب (cap_get_capType(cap)) {
    حالت cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", cap_اشاریہ);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;

    حالت cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", cap_اشاریہ);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;

    حالت cap_endpoint_cap:
        اگر (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      cap_اشاریہ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        واپس performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_جواب(cap), بلاک_2, call, canDonate);
#ورنہ
        واپس performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_جواب(cap), بلاک_2, call);
#اگر_ختم

    حالت cap_notification_cap: {
        اگر (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      cap_اشاریہ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_جواب_cap:
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_Invocation_جواب(
                   گرہ_STATE(ksCurThread),
                   جواب_PTR(cap_جواب_cap_get_cap_جواب_Ptr(cap)),
                   cap_جواب_cap_get_cap_جواب_Can_Grant(cap));
#ورنہ
    حالت cap_جواب_cap:
        اگر (unlikely(cap_جواب_cap_get_cap_جواب_Master(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      cap_اشاریہ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_Invocation_جواب(
                   TCB_PTR(cap_جواب_cap_get_cap_TCBPtr(cap)), خانہ,
                   cap_جواب_cap_get_cap_جواب_Can_Grant(cap));

#اگر_ختم

    حالت cap_thread_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        اگر (unlikely(پہلا_Phase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#اگر_ختم
        واپس decodeTCBInvocation(invLabel, لمبائی, cap, خانہ, call, بفر);

    حالت cap_domain_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        اگر (unlikely(پہلا_Phase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#اگر_ختم
        واپس decodeDomainInvocation(invLabel, لمبائی, بفر);

    حالت cap_cnode_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        اگر (unlikely(پہلا_Phase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#اگر_ختم
        واپس decodeCNodeInvocation(invLabel, لمبائی, cap, بفر);

    حالت cap_untyped_cap:
        واپس decodeUntypedInvocation(invLabel, لمبائی, خانہ, cap, call, بفر);

    حالت cap_irq_control_cap:
        واپس decodeIRQControlInvocation(invLabel, لمبائی, خانہ, بفر);

    حالت cap_irq_handler_cap:
        واپس decodeIRQHandlerInvocation(invLabel,
                                          IDX_مقصد_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_sched_control_cap:
        اگر (unlikely(پہلا_Phase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        واپس decodeSchedControlInvocation(invLabel, cap, لمبائی, بفر);

    حالت cap_sched_context_cap:
        اگر (unlikely(پہلا_Phase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        واپس decodeSchedContextInvocation(invLabel, sc, call);
#اگر_ختم
    طےشدہ:
        fail("Invalid cap type");
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, لفظ_t badge,
                                       bool_t canGrant, bool_t can_Grant_جواب,
                                       bool_t بلاک_2, bool_t call, bool_t canDonate)
{
    sendIPC(بلاک_2, call, badge, canGrant, can_Grant_جواب, canDonate, گرہ_STATE(ksCurThread), ep);

    واپس EXCEPTION_NONE;
}
#ورنہ
exception_t performInvocation_Endpoint(endpoint_t *ep, لفظ_t badge,
                                       bool_t canGrant, bool_t can_Grant_جواب,
                                       bool_t بلاک_2, bool_t call)
{
    sendIPC(بلاک_2, call, badge, canGrant, can_Grant_جواب, گرہ_STATE(ksCurThread), ep);

    واپس EXCEPTION_NONE;
}
#اگر_ختم

exception_t performInvocation_Notification(notification_t *ntfn, لفظ_t badge)
{
    sendSignal(ntfn, badge);

    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t perform_Invocation_جواب(tcb_t *thread, جواب_t *جواب, bool_t canGrant)
{
    do_جواب_Transfer(thread, جواب, canGrant);
    واپس EXCEPTION_NONE;
}
#ورنہ
exception_t perform_Invocation_جواب(tcb_t *thread, cte_t *خانہ, bool_t canGrant)
{
    do_جواب_Transfer(گرہ_STATE(ksCurThread), thread, خانہ, canGrant);
    واپس EXCEPTION_NONE;
}
#اگر_ختم

لفظ_t CONST cap_get_cap_سائز_بٹ(cap_t cap)
{

    cap_لیبل_t ctag;

    ctag = cap_get_capType(cap);

    انتخاب (ctag) {
    حالت cap_untyped_cap:
        واپس cap_untyped_cap_get_cap_بلاک_سائز(cap);

    حالت cap_endpoint_cap:
        واپس se_L4_Endpoint_بٹ;

    حالت cap_notification_cap:
        واپس se_L4_Notification_بٹ;

    حالت cap_cnode_cap:
        واپس cap_cnode_cap_get_capCNodeRadix(cap) + se_L4_خانہ_بٹ;

    حالت cap_thread_cap:
        واپس seL4_TCBBits;

    حالت cap_zombie_cap: {
        لفظ_t type = cap_zombie_cap_get_capZombieType(cap);
        اگر (type == ZombieType_ZombieTCB) {
            واپس seL4_TCBBits;
        }
        واپس ZombieType_ZombieCNode(type) + se_L4_خانہ_بٹ;
    }

    حالت cap_null_cap:
        واپس 0;

    حالت cap_domain_cap:
        واپس 0;

    حالت cap_جواب_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        واپس se_L4_جواب_بٹ;
#ورنہ
        واپس 0;
#اگر_ختم

    حالت cap_irq_control_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_sched_control_cap:
#اگر_ختم
        واپس 0;

    حالت cap_irq_handler_cap:
        واپس 0;

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_sched_context_cap:
        واپس cap_sched_context_cap_get_cap_SCSize_بٹ(cap);
#اگر_ختم

    طےشدہ:
        واپس cap_get_arch_Cap_سائز_بٹ(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_لیبل_t ctag;

    ctag = cap_get_capType(cap);

    انتخاب (ctag) {
    حالت cap_untyped_cap:
        واپس true;

    حالت cap_endpoint_cap:
        واپس true;

    حالت cap_notification_cap:
        واپس true;

    حالت cap_cnode_cap:
        واپس true;

    حالت cap_thread_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_sched_context_cap:
#اگر_ختم
        واپس true;

    حالت cap_zombie_cap:
        واپس true;

    حالت cap_domain_cap:
        واپس false;

    حالت cap_جواب_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        واپس true;
#ورنہ
        واپس false;
#اگر_ختم

    حالت cap_irq_control_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_sched_control_cap:
#اگر_ختم
        واپس false;

    حالت cap_irq_handler_cap:
        واپس false;

    طےشدہ:
        واپس cap_get_archCapIsPhysical(cap);
    }
}

خالی *CONST cap_get_capPtr(cap_t cap)
{
    cap_لیبل_t ctag;

    ctag = cap_get_capType(cap);

    انتخاب (ctag) {
    حالت cap_untyped_cap:
        واپس لفظ_PTR(cap_untyped_cap_get_capPtr(cap));

    حالت cap_endpoint_cap:
        واپس EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    حالت cap_notification_cap:
        واپس NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    حالت cap_cnode_cap:
        واپس CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    حالت cap_thread_cap:
        واپس TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    حالت cap_zombie_cap:
        واپس CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    حالت cap_domain_cap:
        واپس تہی;

    حالت cap_جواب_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        واپس جواب_PTR(cap_جواب_cap_get_cap_جواب_Ptr(cap));
#ورنہ
        واپس تہی;
#اگر_ختم

    حالت cap_irq_control_cap:
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_sched_control_cap:
#اگر_ختم
        واپس تہی;

    حالت cap_irq_handler_cap:
        واپس تہی;

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_sched_context_cap:
        واپس SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#اگر_ختم

    طےشدہ:
        واپس cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    اگر (isArchCap(derivedCap)) {
        واپس Arch_isCapRevocable(derivedCap, srcCap);
    }
    انتخاب (cap_get_capType(derivedCap)) {
    حالت cap_endpoint_cap:
        واپس (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    حالت cap_notification_cap:
        واپس (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    حالت cap_irq_handler_cap:
        واپس (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    حالت cap_untyped_cap:
        واپس true;

    طےشدہ:
        واپس false;
    }
}
