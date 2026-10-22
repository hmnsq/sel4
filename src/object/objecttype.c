/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <config.h>
#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <api/syscall.h>
#ضمن <arch/object/objecttype.h>
#ضمن <machine/io.h>
#ضمن <object/objecttype.h>
#ضمن <object/structures.h>
#ضمن <object/notification.h>
#ضمن <object/endpoint.h>
#ضمن <object/cnode.h>
#ضمن <object/interrupt.h>
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#ضمن <object/schedcontrol.h>
#جيڪڏهن_پورو
#ضمن <object/tcb.h>
#ضمن <object/untyped.h>
#ضمن <model/statedata.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <machine.h>
#ضمن <util.h>
#ضمن <string.h>

لفظ_t get_Object_سائيز(لفظ_t t, لفظ_t user_Obj_سائيز)
{
    إذا (t >= se_L4_Non_Arch_Object_Type_ڳڻپ) {
        أرجع Arch_get_Object_سائيز(t);
    } وإلا {
        بدل (t) {
        حالة seL4_TCBObject:
            أرجع seL4_TCBBits;
        حالة seL4_EndpointObject:
            أرجع se_L4_Endpoint_بٽ;
        حالة seL4_NotificationObject:
            أرجع se_L4_Notification_بٽ;
        حالة se_L4_Cap_جدول_Object:
            أرجع se_L4_جڳهه_بٽ + user_Obj_سائيز;
        حالة seL4_UntypedObject:
            أرجع user_Obj_سائيز;
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        حالة seL4_SchedContextObject:
            أرجع user_Obj_سائيز;
        حالة se_L4_جواب_Object:
            أرجع se_L4_جواب_بٽ;
#جيڪڏهن_پورو
        افتراضي:
            fail("Invalid object type");
            أرجع 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *جڳهه, cap_t cap)
{
    deriveCap_ret_t ret;

    إذا (isArchCap(cap)) {
        أرجع Arch_deriveCap(جڳهه, cap);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_zombie_cap:
        ret.حالت = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        اكسر;

    حالة cap_irq_control_cap:
        ret.حالت = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        اكسر;

    حالة cap_untyped_cap:
        ret.حالت = ensureNoChildren(جڳهه);
        إذا (ret.حالت != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } وإلا {
            ret.cap = cap;
        }
        اكسر;

#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
    حالة cap_جواب_cap:
        ret.حالت = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        اكسر;
#جيڪڏهن_پورو
    افتراضي:
        ret.حالت = EXCEPTION_NONE;
        ret.cap = cap;
    }

    أرجع ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    إذا (isArchCap(cap)) {
        أرجع Arch_finaliseCap(cap, final);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap:
        إذا (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        أرجع fc_ret;

    حالة cap_notification_cap:
        إذا (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#جيڪڏهن_پورو
            unbindMaybeNotification(ntfn);
            cancel_All_سگنل(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        أرجع fc_ret;

    حالة cap_جواب_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        إذا (final) {
            جواب_t *جواب = جواب_PTR(cap_جواب_cap_get_cap_جواب_Ptr(cap));
            إذا (جواب && جواب->جواب_TCB) {
                tcb_t *tcb = جواب->جواب_TCB;
                بدل (thread_state_get_tsType(tcb->tcbState)) {
                حالة Thread_State_Blocked_On_جواب:
                    جواب_remove(جواب, tcb);
                    اكسر;
                حالة ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    اكسر;
                افتراضي:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        أرجع fc_ret;
#جيڪڏهن_پورو
    حالة cap_null_cap:
    حالة cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        أرجع fc_ret;
    }

    إذا (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_cnode_cap: {
        إذا (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            أرجع fc_ret;
        }
        اكسر;
    }

    حالة cap_thread_cap: {
        إذا (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            إذا (sc) {
                schedContext_unbindTCB(sc);
                إذا (sc->scYieldFrom) {
                    sched_Context_complete_Yield_منزل(sc->scYieldFrom);
                }
            }
#جيڪڏهن_پورو
            suspend(tcb);
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
            tcbDebugRemove(tcb);
#جيڪڏهن_پورو
            Arch_prepareThreadDelete(tcb);
            fc_ret.remainder =
                Zombie_new(
                    tcbArchCNodeEntries,
                    ZombieType_ZombieTCB,
                    CTE_REF(cte_ptr)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            أرجع fc_ret;
        }
        اكسر;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        إذا (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            sched_Context_unbind_جواب(sc);
            إذا (sc->scYieldFrom) {
                sched_Context_complete_Yield_منزل(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            أرجع fc_ret;
        }
        اكسر;
#جيڪڏهن_پورو

    حالة cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        أرجع fc_ret;

    حالة cap_irq_handler_cap:
        إذا (final) {
            irq_t irq = IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            أرجع fc_ret;
        }
        اكسر;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap:
        أرجع cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_cap_Can_Grant_جواب(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    افتراضي:
        أرجع false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {
    حالة cap_untyped_cap:
        إذا (cap_get_capIsPhysical(cap_b)) {
            لفظ_t a_بنياد, b_بنياد, a_چوٽي, b_چوٽي;

            a_بنياد = (لفظ_t)لفظ_PTR(cap_untyped_cap_get_capPtr(cap_a));
            b_بنياد = (لفظ_t)cap_get_capPtr(cap_b);

            a_چوٽي = a_بنياد + نقاب(cap_untyped_cap_get_cap_بلاڪ_سائيز(cap_a));
            b_چوٽي = b_بنياد + نقاب(cap_get_cap_سائيز_بٽ(cap_b));

            أرجع (a_بنياد <= b_بنياد) && (b_چوٽي <= a_چوٽي) && (b_بنياد <= b_چوٽي);
        }
        اكسر;

    حالة cap_endpoint_cap:
        إذا (cap_get_capType(cap_b) == cap_endpoint_cap) {
            أرجع cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        اكسر;

    حالة cap_notification_cap:
        إذا (cap_get_capType(cap_b) == cap_notification_cap) {
            أرجع cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        اكسر;

    حالة cap_cnode_cap:
        إذا (cap_get_capType(cap_b) == cap_cnode_cap) {
            أرجع (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        اكسر;

    حالة cap_thread_cap:
        إذا (cap_get_capType(cap_b) == cap_thread_cap) {
            أرجع cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        اكسر;

    حالة cap_جواب_cap:
        إذا (cap_get_capType(cap_b) == cap_جواب_cap) {
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
            أرجع cap_جواب_cap_get_cap_جواب_Ptr(cap_a) ==
                   cap_جواب_cap_get_cap_جواب_Ptr(cap_b);
#وإلا
            أرجع cap_جواب_cap_get_cap_TCBPtr(cap_a) ==
                   cap_جواب_cap_get_cap_TCBPtr(cap_b);
#جيڪڏهن_پورو
        }
        اكسر;

    حالة cap_domain_cap:
        إذا (cap_get_capType(cap_b) == cap_domain_cap) {
            أرجع true;
        }
        اكسر;

    حالة cap_irq_control_cap:
        إذا (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            أرجع true;
        }
        اكسر;

    حالة cap_irq_handler_cap:
        إذا (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            أرجع (لفظ_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (لفظ_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        اكسر;

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        إذا (cap_get_capType(cap_b) == cap_sched_context_cap) {
            أرجع (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_cap_SCSize_بٽ(cap_a) ==
                    cap_sched_context_cap_get_cap_SCSize_بٽ(cap_b));
        }
        اكسر;
    حالة cap_sched_control_cap:
        إذا (cap_get_capType(cap_b) == cap_sched_control_cap) {
            أرجع true;
        }
        اكسر;
#جيڪڏهن_پورو
    افتراضي:
        إذا (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            أرجع Arch_sameRegionAs(cap_a, cap_b);
        }
        اكسر;
    }

    أرجع false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    إذا (cap_get_capType(cap_a) == cap_untyped_cap) {
        أرجع false;
    }
    إذا (cap_get_capType(cap_a) == cap_irq_control_cap) {
        أرجع false;
    }
    إذا (isArchCap(cap_a) && isArchCap(cap_b)) {
        أرجع Arch_sameObjectAs(cap_a, cap_b);
    }
    أرجع sameRegionAs(cap_a, cap_b);
}

cap_t CONST update_Cap_معلومات(bool_t preserve, لفظ_t new_معلومات, cap_t cap)
{
    إذا (isArchCap(cap)) {
        أرجع Arch_update_Cap_معلومات(preserve, new_معلومات, cap);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap:
        إذا (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            أرجع cap_endpoint_cap_سيٽ_cap_EPBadge(cap, new_معلومات);
        } وإلا {
            أرجع cap_null_cap_new();
        }

    حالة cap_notification_cap:
        إذا (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            أرجع cap_notification_cap_سيٽ_cap_Ntfn_Badge(cap, new_معلومات);
        } وإلا {
            أرجع cap_null_cap_new();
        }

    حالة cap_cnode_cap: {
        لفظ_t guard, guard_سائيز;
        se_L4_CNode_Cap_معلومات_t w = { .لفظ = { new_معلومات } };

        guard_سائيز = se_L4_CNode_Cap_معلومات_get_guard_سائيز(w);

        إذا (guard_سائيز + cap_cnode_cap_get_capCNodeRadix(cap) > لفظ_بٽ) {
            أرجع cap_null_cap_new();
        } وإلا {
            cap_t new_cap;

            guard = se_L4_CNode_Cap_معلومات_get_guard(w) & نقاب(guard_سائيز);
            new_cap = cap_cnode_cap_سيٽ_cap_CNode_Guard(cap, guard);
            new_cap = cap_cnode_cap_سيٽ_cap_CNode_Guard_سائيز(new_cap,
                                                          guard_سائيز);

            أرجع new_cap;
        }
    }

    افتراضي:
        أرجع cap;
    }
}

cap_t CONST نقاب_Cap_Rights(seL4_CapRights_t cap_rights, cap_t cap)
{
    إذا (isArchCap(cap)) {
        أرجع Arch_نقاب_Cap_Rights(cap_rights, cap);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
    حالة cap_domain_cap:
    حالة cap_cnode_cap:
    حالة cap_untyped_cap:
    حالة cap_irq_control_cap:
    حالة cap_irq_handler_cap:
    حالة cap_zombie_cap:
    حالة cap_thread_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
    حالة cap_sched_control_cap:
#جيڪڏهن_پورو
        أرجع cap;

    حالة cap_endpoint_cap: {
        cap_t new_cap;

        new_cap = cap_endpoint_cap_سيٽ_cap_Can_Send(
                      cap, cap_endpoint_cap_get_capCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_endpoint_cap_سيٽ_cap_Can_Receive(
                      new_cap, cap_endpoint_cap_get_capCanReceive(cap) &
                      seL4_CapRights_get_capAllowRead(cap_rights));
        new_cap = cap_endpoint_cap_سيٽ_cap_Can_Grant(
                      new_cap, cap_endpoint_cap_get_capCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        new_cap = cap_endpoint_cap_سيٽ_cap_Can_Grant_جواب(
                      new_cap, cap_endpoint_cap_get_cap_Can_Grant_جواب(cap) &
                      se_L4_Cap_Rights_get_cap_Allow_Grant_جواب(cap_rights));

        أرجع new_cap;
    }

    حالة cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_سيٽ_cap_Ntfn_Can_Send(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_سيٽ_cap_Ntfn_Can_Receive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        أرجع new_cap;
    }
    حالة cap_جواب_cap: {
        cap_t new_cap;

        new_cap = cap_جواب_cap_سيٽ_cap_جواب_Can_Grant(
                      cap, cap_جواب_cap_get_cap_جواب_Can_Grant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        أرجع new_cap;
    }


    افتراضي:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, فراغ *region_بنياد, لفظ_t user_سائيز, bool_t device_يادگيري)
{
    /* Handle architecture-specific objects. */
    إذا (t >= (object_t) se_L4_Non_Arch_Object_Type_ڳڻپ) {
        أرجع Arch_createObject(t, region_بنياد, user_سائيز, device_يادگيري);
    }

    /* Create objects. */
    بدل ((api_object_t)t) {
    حالة seL4_TCBObject: {
        tcb_t *tcb;
        tcb = TCB_PTR((لفظ_t)region_بنياد + TCB_وٿي);
        /** AUXUPD: "(True, ptr_retyps 1
          (Ptr ((ptr_val \<acute>tcb) - ctcb_offset) :: (cte_C[5]) ptr)
            o (ptr_retyp \<acute>tcb))" */

        /* Setup non-zero parts of the TCB. */

        Arch_initContext(&tcb->tcbArch.tcbContext);
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
        tcb->tcb_وقت_ٽڪرو = CONFIG_وقت_ٽڪرو;
#جيڪڏهن_پورو
        tcb->tcbDomain = ksCurDomain;
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
        /* Initialize the new TCB to the current core */
        SMP_COND_STATEMENT(tcb->tcbAffinity = getCurrentCPUIndex());
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_ڊيگهائي);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(ڳنڍ_STATE(ksCurThread))->tcbName, TCB_NAME_ڊيگهائي);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_ڊيگهائي);
        tcbDebugAppend(tcb);
#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */

        أرجع cap_thread_cap_new(TCB_REF(tcb));
    }

    حالة seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        أرجع cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(region_بنياد));

    حالة seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        أرجع cap_notification_cap_new(0, true, true,
                                        NTFN_REF(region_بنياد));

    حالة se_L4_Cap_جدول_Object:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        أرجع cap_cnode_cap_new(user_سائيز, 0, 0, CTE_REF(region_بنياد));

    حالة seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        أرجع cap_untyped_cap_new(0, !!device_يادگيري, user_سائيز, لفظ_REF(region_بنياد));

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        أرجع cap_sched_context_cap_new(SC_REF(region_بنياد), user_سائيز);

    حالة se_L4_جواب_Object:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        أرجع cap_جواب_cap_new(جواب_REF(region_بنياد), true);
#جيڪڏهن_پورو

    افتراضي:
        fail("Invalid object type");
    }
}

فراغ createNewObjects(object_t t, cte_t *والد,
                      cte_t *destCNode, لفظ_t dest_وٿي, لفظ_t dest_ڊيگهائي,
                      فراغ *region_بنياد, لفظ_t user_سائيز, bool_t device_يادگيري)
{
    لفظ_t object_سائيز;
    فراغ *ايندڙ_Free_Area;
    لفظ_t i;
    لفظ_t ڪل_Object_سائيز اڻ_استعمال_ٿيل;

    /* ghost check that we're visiting less bytes than the max object size */
    object_سائيز = get_Object_سائيز(t, user_سائيز);
    ڪل_Object_سائيز = dest_ڊيگهائي << object_سائيز;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    ايندڙ_Free_Area = region_بنياد;
    لكل (i = 0; i < dest_ڊيگهائي; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (فراغ *)((لفظ_t)ايندڙ_Free_Area + (i << object_سائيز)), user_سائيز, device_يادگيري);

        /* Insert the cap into the user's cspace. */
        وجهو_New_Cap(والد, &destCNode[dest_وٿي + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
exception_t decodeInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                             cptr_t cap_اشاريو, cte_t *جڳهه, cap_t cap,
                             bool_t بلاڪ_2, bool_t call,
                             bool_t canDonate, bool_t پهريون_Phase, لفظ_t *بفر)
#وإلا
exception_t decodeInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                             cptr_t cap_اشاريو, cte_t *جڳهه, cap_t cap,
                             bool_t بلاڪ_2, bool_t call,
                             لفظ_t *بفر)
#جيڪڏهن_پورو
{
    إذا (isArchCap(cap)) {
        أرجع Arch_decodeInvocation(invLabel, ڊيگهائي, cap_اشاريو,
                                     جڳهه, cap, call, بفر);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", cap_اشاريو);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", cap_اشاريو);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_endpoint_cap:
        إذا (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      cap_اشاريو);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        أرجع performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_جواب(cap), بلاڪ_2, call, canDonate);
#وإلا
        أرجع performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_جواب(cap), بلاڪ_2, call);
#جيڪڏهن_پورو

    حالة cap_notification_cap: {
        إذا (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      cap_اشاريو);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_جواب_cap:
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_Invocation_جواب(
                   ڳنڍ_STATE(ksCurThread),
                   جواب_PTR(cap_جواب_cap_get_cap_جواب_Ptr(cap)),
                   cap_جواب_cap_get_cap_جواب_Can_Grant(cap));
#وإلا
    حالة cap_جواب_cap:
        إذا (unlikely(cap_جواب_cap_get_cap_جواب_Master(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      cap_اشاريو);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_Invocation_جواب(
                   TCB_PTR(cap_جواب_cap_get_cap_TCBPtr(cap)), جڳهه,
                   cap_جواب_cap_get_cap_جواب_Can_Grant(cap));

#جيڪڏهن_پورو

    حالة cap_thread_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        إذا (unlikely(پهريون_Phase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جيڪڏهن_پورو
        أرجع decodeTCBInvocation(invLabel, ڊيگهائي, cap, جڳهه, call, بفر);

    حالة cap_domain_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        إذا (unlikely(پهريون_Phase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جيڪڏهن_پورو
        أرجع decodeDomainInvocation(invLabel, ڊيگهائي, بفر);

    حالة cap_cnode_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        إذا (unlikely(پهريون_Phase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جيڪڏهن_پورو
        أرجع decodeCNodeInvocation(invLabel, ڊيگهائي, cap, بفر);

    حالة cap_untyped_cap:
        أرجع decodeUntypedInvocation(invLabel, ڊيگهائي, جڳهه, cap, call, بفر);

    حالة cap_irq_control_cap:
        أرجع decodeIRQControlInvocation(invLabel, ڊيگهائي, جڳهه, بفر);

    حالة cap_irq_handler_cap:
        أرجع decodeIRQHandlerInvocation(invLabel,
                                          IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
        إذا (unlikely(پهريون_Phase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        أرجع decodeSchedControlInvocation(invLabel, cap, ڊيگهائي, بفر);

    حالة cap_sched_context_cap:
        إذا (unlikely(پهريون_Phase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        أرجع decodeSchedContextInvocation(invLabel, sc, call);
#جيڪڏهن_پورو
    افتراضي:
        fail("Invalid cap type");
    }
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, لفظ_t badge,
                                       bool_t canGrant, bool_t can_Grant_جواب,
                                       bool_t بلاڪ_2, bool_t call, bool_t canDonate)
{
    sendIPC(بلاڪ_2, call, badge, canGrant, can_Grant_جواب, canDonate, ڳنڍ_STATE(ksCurThread), ep);

    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t performInvocation_Endpoint(endpoint_t *ep, لفظ_t badge,
                                       bool_t canGrant, bool_t can_Grant_جواب,
                                       bool_t بلاڪ_2, bool_t call)
{
    sendIPC(بلاڪ_2, call, badge, canGrant, can_Grant_جواب, ڳنڍ_STATE(ksCurThread), ep);

    أرجع EXCEPTION_NONE;
}
#جيڪڏهن_پورو

exception_t performInvocation_Notification(notification_t *ntfn, لفظ_t badge)
{
    sendSignal(ntfn, badge);

    أرجع EXCEPTION_NONE;
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
exception_t perform_Invocation_جواب(tcb_t *thread, جواب_t *جواب, bool_t canGrant)
{
    do_جواب_Transfer(thread, جواب, canGrant);
    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t perform_Invocation_جواب(tcb_t *thread, cte_t *جڳهه, bool_t canGrant)
{
    do_جواب_Transfer(ڳنڍ_STATE(ksCurThread), thread, جڳهه, canGrant);
    أرجع EXCEPTION_NONE;
}
#جيڪڏهن_پورو

لفظ_t CONST cap_get_cap_سائيز_بٽ(cap_t cap)
{

    cap_ٽيگ_t ctag;

    ctag = cap_get_capType(cap);

    بدل (ctag) {
    حالة cap_untyped_cap:
        أرجع cap_untyped_cap_get_cap_بلاڪ_سائيز(cap);

    حالة cap_endpoint_cap:
        أرجع se_L4_Endpoint_بٽ;

    حالة cap_notification_cap:
        أرجع se_L4_Notification_بٽ;

    حالة cap_cnode_cap:
        أرجع cap_cnode_cap_get_capCNodeRadix(cap) + se_L4_جڳهه_بٽ;

    حالة cap_thread_cap:
        أرجع seL4_TCBBits;

    حالة cap_zombie_cap: {
        لفظ_t type = cap_zombie_cap_get_capZombieType(cap);
        إذا (type == ZombieType_ZombieTCB) {
            أرجع seL4_TCBBits;
        }
        أرجع ZombieType_ZombieCNode(type) + se_L4_جڳهه_بٽ;
    }

    حالة cap_null_cap:
        أرجع 0;

    حالة cap_domain_cap:
        أرجع 0;

    حالة cap_جواب_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        أرجع se_L4_جواب_بٽ;
#وإلا
        أرجع 0;
#جيڪڏهن_پورو

    حالة cap_irq_control_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#جيڪڏهن_پورو
        أرجع 0;

    حالة cap_irq_handler_cap:
        أرجع 0;

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        أرجع cap_sched_context_cap_get_cap_SCSize_بٽ(cap);
#جيڪڏهن_پورو

    افتراضي:
        أرجع cap_get_arch_Cap_سائيز_بٽ(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_ٽيگ_t ctag;

    ctag = cap_get_capType(cap);

    بدل (ctag) {
    حالة cap_untyped_cap:
        أرجع true;

    حالة cap_endpoint_cap:
        أرجع true;

    حالة cap_notification_cap:
        أرجع true;

    حالة cap_cnode_cap:
        أرجع true;

    حالة cap_thread_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
#جيڪڏهن_پورو
        أرجع true;

    حالة cap_zombie_cap:
        أرجع true;

    حالة cap_domain_cap:
        أرجع false;

    حالة cap_جواب_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        أرجع true;
#وإلا
        أرجع false;
#جيڪڏهن_پورو

    حالة cap_irq_control_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#جيڪڏهن_پورو
        أرجع false;

    حالة cap_irq_handler_cap:
        أرجع false;

    افتراضي:
        أرجع cap_get_archCapIsPhysical(cap);
    }
}

فراغ *CONST cap_get_capPtr(cap_t cap)
{
    cap_ٽيگ_t ctag;

    ctag = cap_get_capType(cap);

    بدل (ctag) {
    حالة cap_untyped_cap:
        أرجع لفظ_PTR(cap_untyped_cap_get_capPtr(cap));

    حالة cap_endpoint_cap:
        أرجع EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    حالة cap_notification_cap:
        أرجع NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    حالة cap_cnode_cap:
        أرجع CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    حالة cap_thread_cap:
        أرجع TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    حالة cap_zombie_cap:
        أرجع CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    حالة cap_domain_cap:
        أرجع عدم;

    حالة cap_جواب_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        أرجع جواب_PTR(cap_جواب_cap_get_cap_جواب_Ptr(cap));
#وإلا
        أرجع عدم;
#جيڪڏهن_پورو

    حالة cap_irq_control_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#جيڪڏهن_پورو
        أرجع عدم;

    حالة cap_irq_handler_cap:
        أرجع عدم;

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        أرجع SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#جيڪڏهن_پورو

    افتراضي:
        أرجع cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    إذا (isArchCap(derivedCap)) {
        أرجع Arch_isCapRevocable(derivedCap, srcCap);
    }
    بدل (cap_get_capType(derivedCap)) {
    حالة cap_endpoint_cap:
        أرجع (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    حالة cap_notification_cap:
        أرجع (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    حالة cap_irq_handler_cap:
        أرجع (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    حالة cap_untyped_cap:
        أرجع true;

    افتراضي:
        أرجع false;
    }
}
