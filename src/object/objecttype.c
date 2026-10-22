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
#که_تعریف_شوی CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#ضمن <object/schedcontrol.h>
#د_که_پای
#ضمن <object/tcb.h>
#ضمن <object/untyped.h>
#ضمن <model/statedata.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <machine.h>
#ضمن <util.h>
#ضمن <string.h>

کلمه_t get_Object_اندازه(کلمه_t t, کلمه_t user_Obj_اندازه)
{
    إذا (t >= se_L4_Non_Arch_Object_Type_شمېر) {
        أرجع Arch_get_Object_اندازه(t);
    } وإلا {
        بدل (t) {
        حالة seL4_TCBObject:
            أرجع seL4_TCBBits;
        حالة seL4_EndpointObject:
            أرجع se_L4_Endpoint_بټونه;
        حالة seL4_NotificationObject:
            أرجع se_L4_Notification_بټونه;
        حالة se_L4_Cap_جدول_Object:
            أرجع se_L4_خونه_بټونه + user_Obj_اندازه;
        حالة seL4_UntypedObject:
            أرجع user_Obj_اندازه;
#که_تعریف_شوی CONFIG_KERNEL_MCS
        حالة seL4_SchedContextObject:
            أرجع user_Obj_اندازه;
        حالة se_L4_ځواب_Object:
            أرجع se_L4_ځواب_بټونه;
#د_که_پای
        افتراضي:
            fail("Invalid object type");
            أرجع 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *خونه, cap_t cap)
{
    deriveCap_ret_t ret;

    إذا (isArchCap(cap)) {
        أرجع Arch_deriveCap(خونه, cap);
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
        ret.حالت = ensureNoChildren(خونه);
        إذا (ret.حالت != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } وإلا {
            ret.cap = cap;
        }
        اكسر;

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_ځواب_cap:
        ret.حالت = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        اكسر;
#د_که_پای
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
#که_تعریف_شوی CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#د_که_پای
            unbindMaybeNotification(ntfn);
            cancel_All_سیګنالونه(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        أرجع fc_ret;

    حالة cap_ځواب_cap:
#که_تعریف_شوی CONFIG_KERNEL_MCS
        إذا (final) {
            ځواب_t *ځواب = ځواب_PTR(cap_ځواب_cap_get_cap_ځواب_Ptr(cap));
            إذا (ځواب && ځواب->ځواب_TCB) {
                tcb_t *tcb = ځواب->ځواب_TCB;
                بدل (thread_state_get_tsType(tcb->tcbState)) {
                حالة Thread_State_Blocked_On_ځواب:
                    ځواب_remove(ځواب, tcb);
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
#د_که_پای
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
#که_تعریف_شوی CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            إذا (sc) {
                schedContext_unbindTCB(sc);
                إذا (sc->scYieldFrom) {
                    sched_Context_complete_Yield_منزل(sc->scYieldFrom);
                }
            }
#د_که_پای
            suspend(tcb);
#که_تعریف_شوی CONFIG_DEBUG_BUILD
            tcbDebugRemove(tcb);
#د_که_پای
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

#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        إذا (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            sched_Context_unbind_ځواب(sc);
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
#د_که_پای

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
               cap_endpoint_cap_get_cap_Can_Grant_ځواب(cap) &&
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
            کلمه_t a_بنسټ, b_بنسټ, a_سر, b_سر;

            a_بنسټ = (کلمه_t)کلمه_PTR(cap_untyped_cap_get_capPtr(cap_a));
            b_بنسټ = (کلمه_t)cap_get_capPtr(cap_b);

            a_سر = a_بنسټ + ماسک(cap_untyped_cap_get_cap_بلاک_اندازه(cap_a));
            b_سر = b_بنسټ + ماسک(cap_get_cap_اندازه_بټونه(cap_b));

            أرجع (a_بنسټ <= b_بنسټ) && (b_سر <= a_سر) && (b_بنسټ <= b_سر);
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

    حالة cap_ځواب_cap:
        إذا (cap_get_capType(cap_b) == cap_ځواب_cap) {
#که_تعریف_شوی CONFIG_KERNEL_MCS
            أرجع cap_ځواب_cap_get_cap_ځواب_Ptr(cap_a) ==
                   cap_ځواب_cap_get_cap_ځواب_Ptr(cap_b);
#وإلا
            أرجع cap_ځواب_cap_get_cap_TCBPtr(cap_a) ==
                   cap_ځواب_cap_get_cap_TCBPtr(cap_b);
#د_که_پای
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
            أرجع (کلمه_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (کلمه_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        اكسر;

#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        إذا (cap_get_capType(cap_b) == cap_sched_context_cap) {
            أرجع (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_cap_SCSize_بټونه(cap_a) ==
                    cap_sched_context_cap_get_cap_SCSize_بټونه(cap_b));
        }
        اكسر;
    حالة cap_sched_control_cap:
        إذا (cap_get_capType(cap_b) == cap_sched_control_cap) {
            أرجع true;
        }
        اكسر;
#د_که_پای
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

cap_t CONST update_Cap_معلومات(bool_t preserve, کلمه_t new_معلومات, cap_t cap)
{
    إذا (isArchCap(cap)) {
        أرجع Arch_update_Cap_معلومات(preserve, new_معلومات, cap);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap:
        إذا (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            أرجع cap_endpoint_cap_سېټ_cap_EPBadge(cap, new_معلومات);
        } وإلا {
            أرجع cap_null_cap_new();
        }

    حالة cap_notification_cap:
        إذا (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            أرجع cap_notification_cap_سېټ_cap_Ntfn_Badge(cap, new_معلومات);
        } وإلا {
            أرجع cap_null_cap_new();
        }

    حالة cap_cnode_cap: {
        کلمه_t guard, guard_اندازه;
        se_L4_CNode_Cap_معلومات_t w = { .ټکي = { new_معلومات } };

        guard_اندازه = se_L4_CNode_Cap_معلومات_get_guard_اندازه(w);

        إذا (guard_اندازه + cap_cnode_cap_get_capCNodeRadix(cap) > کلمه_بټونه) {
            أرجع cap_null_cap_new();
        } وإلا {
            cap_t new_cap;

            guard = se_L4_CNode_Cap_معلومات_get_guard(w) & ماسک(guard_اندازه);
            new_cap = cap_cnode_cap_سېټ_cap_CNode_Guard(cap, guard);
            new_cap = cap_cnode_cap_سېټ_cap_CNode_Guard_اندازه(new_cap,
                                                          guard_اندازه);

            أرجع new_cap;
        }
    }

    افتراضي:
        أرجع cap;
    }
}

cap_t CONST ماسک_Cap_Rights(seL4_CapRights_t cap_rights, cap_t cap)
{
    إذا (isArchCap(cap)) {
        أرجع Arch_ماسک_Cap_Rights(cap_rights, cap);
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
#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
    حالة cap_sched_control_cap:
#د_که_پای
        أرجع cap;

    حالة cap_endpoint_cap: {
        cap_t new_cap;

        new_cap = cap_endpoint_cap_سېټ_cap_Can_Send(
                      cap, cap_endpoint_cap_get_capCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_endpoint_cap_سېټ_cap_Can_Receive(
                      new_cap, cap_endpoint_cap_get_capCanReceive(cap) &
                      seL4_CapRights_get_capAllowRead(cap_rights));
        new_cap = cap_endpoint_cap_سېټ_cap_Can_Grant(
                      new_cap, cap_endpoint_cap_get_capCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        new_cap = cap_endpoint_cap_سېټ_cap_Can_Grant_ځواب(
                      new_cap, cap_endpoint_cap_get_cap_Can_Grant_ځواب(cap) &
                      se_L4_Cap_Rights_get_cap_Allow_Grant_ځواب(cap_rights));

        أرجع new_cap;
    }

    حالة cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_سېټ_cap_Ntfn_Can_Send(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_سېټ_cap_Ntfn_Can_Receive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        أرجع new_cap;
    }
    حالة cap_ځواب_cap: {
        cap_t new_cap;

        new_cap = cap_ځواب_cap_سېټ_cap_ځواب_Can_Grant(
                      cap, cap_ځواب_cap_get_cap_ځواب_Can_Grant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        أرجع new_cap;
    }


    افتراضي:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, فراغ *region_بنسټ, کلمه_t user_اندازه, bool_t device_حافظه)
{
    /* Handle architecture-specific objects. */
    إذا (t >= (object_t) se_L4_Non_Arch_Object_Type_شمېر) {
        أرجع Arch_createObject(t, region_بنسټ, user_اندازه, device_حافظه);
    }

    /* Create objects. */
    بدل ((api_object_t)t) {
    حالة seL4_TCBObject: {
        tcb_t *tcb;
        tcb = TCB_PTR((کلمه_t)region_بنسټ + TCB_دننه_واټن);
        /** AUXUPD: "(True, ptr_retyps 1
          (Ptr ((ptr_val \<acute>tcb) - ctcb_offset) :: (cte_C[5]) ptr)
            o (ptr_retyp \<acute>tcb))" */

        /* Setup non-zero parts of the TCB. */

        Arch_initContext(&tcb->tcbArch.tcbContext);
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
        tcb->tcb_وخت_ټوټه = CONFIG_وخت_ټوټه;
#د_که_پای
        tcb->tcbDomain = ksCurDomain;
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
        /* Initialize the new TCB to the current core */
        SMP_COND_STATEMENT(tcb->tcbAffinity = getCurrentCPUIndex());
#د_که_پای
#که_تعریف_شوی CONFIG_DEBUG_BUILD
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_اوږدتیا);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(غوټه_STATE(ksCurThread))->tcbName, TCB_NAME_اوږدتیا);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_اوږدتیا);
        tcbDebugAppend(tcb);
#د_که_پای /* CONFIG_DEBUG_BUILD */

        أرجع cap_thread_cap_new(TCB_REF(tcb));
    }

    حالة seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        أرجع cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(region_بنسټ));

    حالة seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        أرجع cap_notification_cap_new(0, true, true,
                                        NTFN_REF(region_بنسټ));

    حالة se_L4_Cap_جدول_Object:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        أرجع cap_cnode_cap_new(user_اندازه, 0, 0, CTE_REF(region_بنسټ));

    حالة seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        أرجع cap_untyped_cap_new(0, !!device_حافظه, user_اندازه, کلمه_REF(region_بنسټ));

#که_تعریف_شوی CONFIG_KERNEL_MCS
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
        أرجع cap_sched_context_cap_new(SC_REF(region_بنسټ), user_اندازه);

    حالة se_L4_ځواب_Object:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        أرجع cap_ځواب_cap_new(ځواب_REF(region_بنسټ), true);
#د_که_پای

    افتراضي:
        fail("Invalid object type");
    }
}

فراغ createNewObjects(object_t t, cte_t *پلار,
                      cte_t *destCNode, کلمه_t dest_دننه_واټن, کلمه_t dest_اوږدتیا,
                      فراغ *region_بنسټ, کلمه_t user_اندازه, bool_t device_حافظه)
{
    کلمه_t object_اندازه;
    فراغ *بل_Free_Area;
    کلمه_t i;
    کلمه_t ټول_Object_اندازه ناکارول_شوی;

    /* ghost check that we're visiting less bytes than the max object size */
    object_اندازه = get_Object_اندازه(t, user_اندازه);
    ټول_Object_اندازه = dest_اوږدتیا << object_اندازه;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    بل_Free_Area = region_بنسټ;
    لكل (i = 0; i < dest_اوږدتیا; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (فراغ *)((کلمه_t)بل_Free_Area + (i << object_اندازه)), user_اندازه, device_حافظه);

        /* Insert the cap into the user's cspace. */
        ورننباسه_New_Cap(پلار, &destCNode[dest_دننه_واټن + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
exception_t decodeInvocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                             cptr_t cap_شاخص, cte_t *خونه, cap_t cap,
                             bool_t بلاک_2, bool_t call,
                             bool_t canDonate, bool_t لومړی_Phase, کلمه_t *بفر)
#وإلا
exception_t decodeInvocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                             cptr_t cap_شاخص, cte_t *خونه, cap_t cap,
                             bool_t بلاک_2, bool_t call,
                             کلمه_t *بفر)
#د_که_پای
{
    إذا (isArchCap(cap)) {
        أرجع Arch_decodeInvocation(invLabel, اوږدتیا, cap_شاخص,
                                     خونه, cap, call, بفر);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", cap_شاخص);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", cap_شاخص);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_endpoint_cap:
        إذا (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      cap_شاخص);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
#که_تعریف_شوی CONFIG_KERNEL_MCS
        أرجع performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_ځواب(cap), بلاک_2, call, canDonate);
#وإلا
        أرجع performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_ځواب(cap), بلاک_2, call);
#د_که_پای

    حالة cap_notification_cap: {
        إذا (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      cap_شاخص);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_ځواب_cap:
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_Invocation_ځواب(
                   غوټه_STATE(ksCurThread),
                   ځواب_PTR(cap_ځواب_cap_get_cap_ځواب_Ptr(cap)),
                   cap_ځواب_cap_get_cap_ځواب_Can_Grant(cap));
#وإلا
    حالة cap_ځواب_cap:
        إذا (unlikely(cap_ځواب_cap_get_cap_ځواب_Master(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      cap_شاخص);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_Invocation_ځواب(
                   TCB_PTR(cap_ځواب_cap_get_cap_TCBPtr(cap)), خونه,
                   cap_ځواب_cap_get_cap_ځواب_Can_Grant(cap));

#د_که_پای

    حالة cap_thread_cap:
#که_تعریف_شوی CONFIG_KERNEL_MCS
        إذا (unlikely(لومړی_Phase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#د_که_پای
        أرجع decodeTCBInvocation(invLabel, اوږدتیا, cap, خونه, call, بفر);

    حالة cap_domain_cap:
#که_تعریف_شوی CONFIG_KERNEL_MCS
        إذا (unlikely(لومړی_Phase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#د_که_پای
        أرجع decodeDomainInvocation(invLabel, اوږدتیا, بفر);

    حالة cap_cnode_cap:
#که_تعریف_شوی CONFIG_KERNEL_MCS
        إذا (unlikely(لومړی_Phase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#د_که_پای
        أرجع decodeCNodeInvocation(invLabel, اوږدتیا, cap, بفر);

    حالة cap_untyped_cap:
        أرجع decodeUntypedInvocation(invLabel, اوږدتیا, خونه, cap, call, بفر);

    حالة cap_irq_control_cap:
        أرجع decodeIRQControlInvocation(invLabel, اوږدتیا, خونه, بفر);

    حالة cap_irq_handler_cap:
        أرجع decodeIRQHandlerInvocation(invLabel,
                                          IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
        إذا (unlikely(لومړی_Phase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        أرجع decodeSchedControlInvocation(invLabel, cap, اوږدتیا, بفر);

    حالة cap_sched_context_cap:
        إذا (unlikely(لومړی_Phase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        أرجع decodeSchedContextInvocation(invLabel, sc, call);
#د_که_پای
    افتراضي:
        fail("Invalid cap type");
    }
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, کلمه_t badge,
                                       bool_t canGrant, bool_t can_Grant_ځواب,
                                       bool_t بلاک_2, bool_t call, bool_t canDonate)
{
    sendIPC(بلاک_2, call, badge, canGrant, can_Grant_ځواب, canDonate, غوټه_STATE(ksCurThread), ep);

    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t performInvocation_Endpoint(endpoint_t *ep, کلمه_t badge,
                                       bool_t canGrant, bool_t can_Grant_ځواب,
                                       bool_t بلاک_2, bool_t call)
{
    sendIPC(بلاک_2, call, badge, canGrant, can_Grant_ځواب, غوټه_STATE(ksCurThread), ep);

    أرجع EXCEPTION_NONE;
}
#د_که_پای

exception_t performInvocation_Notification(notification_t *ntfn, کلمه_t badge)
{
    sendSignal(ntfn, badge);

    أرجع EXCEPTION_NONE;
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
exception_t perform_Invocation_ځواب(tcb_t *thread, ځواب_t *ځواب, bool_t canGrant)
{
    do_ځواب_Transfer(thread, ځواب, canGrant);
    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t perform_Invocation_ځواب(tcb_t *thread, cte_t *خونه, bool_t canGrant)
{
    do_ځواب_Transfer(غوټه_STATE(ksCurThread), thread, خونه, canGrant);
    أرجع EXCEPTION_NONE;
}
#د_که_پای

کلمه_t CONST cap_get_cap_اندازه_بټونه(cap_t cap)
{

    cap_نښان_t ctag;

    ctag = cap_get_capType(cap);

    بدل (ctag) {
    حالة cap_untyped_cap:
        أرجع cap_untyped_cap_get_cap_بلاک_اندازه(cap);

    حالة cap_endpoint_cap:
        أرجع se_L4_Endpoint_بټونه;

    حالة cap_notification_cap:
        أرجع se_L4_Notification_بټونه;

    حالة cap_cnode_cap:
        أرجع cap_cnode_cap_get_capCNodeRadix(cap) + se_L4_خونه_بټونه;

    حالة cap_thread_cap:
        أرجع seL4_TCBBits;

    حالة cap_zombie_cap: {
        کلمه_t type = cap_zombie_cap_get_capZombieType(cap);
        إذا (type == ZombieType_ZombieTCB) {
            أرجع seL4_TCBBits;
        }
        أرجع ZombieType_ZombieCNode(type) + se_L4_خونه_بټونه;
    }

    حالة cap_null_cap:
        أرجع 0;

    حالة cap_domain_cap:
        أرجع 0;

    حالة cap_ځواب_cap:
#که_تعریف_شوی CONFIG_KERNEL_MCS
        أرجع se_L4_ځواب_بټونه;
#وإلا
        أرجع 0;
#د_که_پای

    حالة cap_irq_control_cap:
#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#د_که_پای
        أرجع 0;

    حالة cap_irq_handler_cap:
        أرجع 0;

#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        أرجع cap_sched_context_cap_get_cap_SCSize_بټونه(cap);
#د_که_پای

    افتراضي:
        أرجع cap_get_arch_Cap_اندازه_بټونه(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_نښان_t ctag;

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
#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
#د_که_پای
        أرجع true;

    حالة cap_zombie_cap:
        أرجع true;

    حالة cap_domain_cap:
        أرجع false;

    حالة cap_ځواب_cap:
#که_تعریف_شوی CONFIG_KERNEL_MCS
        أرجع true;
#وإلا
        أرجع false;
#د_که_پای

    حالة cap_irq_control_cap:
#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#د_که_پای
        أرجع false;

    حالة cap_irq_handler_cap:
        أرجع false;

    افتراضي:
        أرجع cap_get_archCapIsPhysical(cap);
    }
}

فراغ *CONST cap_get_capPtr(cap_t cap)
{
    cap_نښان_t ctag;

    ctag = cap_get_capType(cap);

    بدل (ctag) {
    حالة cap_untyped_cap:
        أرجع کلمه_PTR(cap_untyped_cap_get_capPtr(cap));

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

    حالة cap_ځواب_cap:
#که_تعریف_شوی CONFIG_KERNEL_MCS
        أرجع ځواب_PTR(cap_ځواب_cap_get_cap_ځواب_Ptr(cap));
#وإلا
        أرجع عدم;
#د_که_پای

    حالة cap_irq_control_cap:
#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#د_که_پای
        أرجع عدم;

    حالة cap_irq_handler_cap:
        أرجع عدم;

#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        أرجع SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#د_که_پای

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
