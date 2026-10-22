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
#اگر_تعریف_شده CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#ضمن <object/schedcontrol.h>
#پایان_اگر
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
    إذا (t >= se_L4_Non_Arch_Object_Type_شمار) {
        أرجع Arch_get_Object_اندازه(t);
    } وإلا {
        بدل (t) {
        حالة seL4_TCBObject:
            أرجع seL4_TCBBits;
        حالة seL4_EndpointObject:
            أرجع se_L4_Endpoint_بیتها;
        حالة seL4_NotificationObject:
            أرجع se_L4_Notification_بیتها;
        حالة se_L4_Cap_جدول_Object:
            أرجع se_L4_جایگاه_بیتها + user_Obj_اندازه;
        حالة seL4_UntypedObject:
            أرجع user_Obj_اندازه;
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        حالة seL4_SchedContextObject:
            أرجع user_Obj_اندازه;
        حالة se_L4_پاسخ_Object:
            أرجع se_L4_پاسخ_بیتها;
#پایان_اگر
        افتراضي:
            fail("Invalid object type");
            أرجع 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *جایگاه, cap_t cap)
{
    deriveCap_ret_t ret;

    إذا (isArchCap(cap)) {
        أرجع Arch_deriveCap(جایگاه, cap);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_zombie_cap:
        ret.وضعیت = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        اكسر;

    حالة cap_irq_control_cap:
        ret.وضعیت = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        اكسر;

    حالة cap_untyped_cap:
        ret.وضعیت = ensureNoChildren(جایگاه);
        إذا (ret.وضعیت != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } وإلا {
            ret.cap = cap;
        }
        اكسر;

#اگر_تعریف_نشده CONFIG_KERNEL_MCS
    حالة cap_پاسخ_cap:
        ret.وضعیت = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        اكسر;
#پایان_اگر
    افتراضي:
        ret.وضعیت = EXCEPTION_NONE;
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
#اگر_تعریف_شده CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#پایان_اگر
            unbindMaybeNotification(ntfn);
            cancel_All_سیگنالها(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        أرجع fc_ret;

    حالة cap_پاسخ_cap:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        إذا (final) {
            پاسخ_t *پاسخ = پاسخ_PTR(cap_پاسخ_cap_get_cap_پاسخ_Ptr(cap));
            إذا (پاسخ && پاسخ->پاسخ_TCB) {
                tcb_t *tcb = پاسخ->پاسخ_TCB;
                بدل (thread_state_get_tsType(tcb->tcbState)) {
                حالة Thread_State_Blocked_On_پاسخ:
                    پاسخ_remove(پاسخ, tcb);
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
#پایان_اگر
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
#اگر_تعریف_شده CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            إذا (sc) {
                schedContext_unbindTCB(sc);
                إذا (sc->scYieldFrom) {
                    sched_Context_complete_Yield_مقصد(sc->scYieldFrom);
                }
            }
#پایان_اگر
            suspend(tcb);
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
            tcbDebugRemove(tcb);
#پایان_اگر
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

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        إذا (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            sched_Context_unbind_پاسخ(sc);
            إذا (sc->scYieldFrom) {
                sched_Context_complete_Yield_مقصد(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            أرجع fc_ret;
        }
        اكسر;
#پایان_اگر

    حالة cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        أرجع fc_ret;

    حالة cap_irq_handler_cap:
        إذا (final) {
            irq_t irq = IDX_مقصد_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

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
               cap_endpoint_cap_get_cap_Can_Grant_پاسخ(cap) &&
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
            کلمه_t a_پایه, b_پایه, a_بالاترین, b_بالاترین;

            a_پایه = (کلمه_t)کلمه_PTR(cap_untyped_cap_get_capPtr(cap_a));
            b_پایه = (کلمه_t)cap_get_capPtr(cap_b);

            a_بالاترین = a_پایه + ماسک(cap_untyped_cap_get_cap_بلوک_اندازه(cap_a));
            b_بالاترین = b_پایه + ماسک(cap_get_cap_اندازه_بیتها(cap_b));

            أرجع (a_پایه <= b_پایه) && (b_بالاترین <= a_بالاترین) && (b_پایه <= b_بالاترین);
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

    حالة cap_پاسخ_cap:
        إذا (cap_get_capType(cap_b) == cap_پاسخ_cap) {
#اگر_تعریف_شده CONFIG_KERNEL_MCS
            أرجع cap_پاسخ_cap_get_cap_پاسخ_Ptr(cap_a) ==
                   cap_پاسخ_cap_get_cap_پاسخ_Ptr(cap_b);
#وإلا
            أرجع cap_پاسخ_cap_get_cap_TCBPtr(cap_a) ==
                   cap_پاسخ_cap_get_cap_TCBPtr(cap_b);
#پایان_اگر
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

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        إذا (cap_get_capType(cap_b) == cap_sched_context_cap) {
            أرجع (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_cap_SCSize_بیتها(cap_a) ==
                    cap_sched_context_cap_get_cap_SCSize_بیتها(cap_b));
        }
        اكسر;
    حالة cap_sched_control_cap:
        إذا (cap_get_capType(cap_b) == cap_sched_control_cap) {
            أرجع true;
        }
        اكسر;
#پایان_اگر
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

cap_t CONST update_Cap_داده(bool_t preserve, کلمه_t new_داده, cap_t cap)
{
    إذا (isArchCap(cap)) {
        أرجع Arch_update_Cap_داده(preserve, new_داده, cap);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap:
        إذا (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            أرجع cap_endpoint_cap_مجموعه_cap_EPBadge(cap, new_داده);
        } وإلا {
            أرجع cap_null_cap_new();
        }

    حالة cap_notification_cap:
        إذا (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            أرجع cap_notification_cap_مجموعه_cap_Ntfn_Badge(cap, new_داده);
        } وإلا {
            أرجع cap_null_cap_new();
        }

    حالة cap_cnode_cap: {
        کلمه_t guard, guard_اندازه;
        se_L4_CNode_Cap_داده_t w = { .واژگان = { new_داده } };

        guard_اندازه = se_L4_CNode_Cap_داده_get_guard_اندازه(w);

        إذا (guard_اندازه + cap_cnode_cap_get_capCNodeRadix(cap) > کلمه_بیتها) {
            أرجع cap_null_cap_new();
        } وإلا {
            cap_t new_cap;

            guard = se_L4_CNode_Cap_داده_get_guard(w) & ماسک(guard_اندازه);
            new_cap = cap_cnode_cap_مجموعه_cap_CNode_Guard(cap, guard);
            new_cap = cap_cnode_cap_مجموعه_cap_CNode_Guard_اندازه(new_cap,
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
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
    حالة cap_sched_control_cap:
#پایان_اگر
        أرجع cap;

    حالة cap_endpoint_cap: {
        cap_t new_cap;

        new_cap = cap_endpoint_cap_مجموعه_cap_Can_Send(
                      cap, cap_endpoint_cap_get_capCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_endpoint_cap_مجموعه_cap_Can_Receive(
                      new_cap, cap_endpoint_cap_get_capCanReceive(cap) &
                      seL4_CapRights_get_capAllowRead(cap_rights));
        new_cap = cap_endpoint_cap_مجموعه_cap_Can_Grant(
                      new_cap, cap_endpoint_cap_get_capCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        new_cap = cap_endpoint_cap_مجموعه_cap_Can_Grant_پاسخ(
                      new_cap, cap_endpoint_cap_get_cap_Can_Grant_پاسخ(cap) &
                      se_L4_Cap_Rights_get_cap_Allow_Grant_پاسخ(cap_rights));

        أرجع new_cap;
    }

    حالة cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_مجموعه_cap_Ntfn_Can_Send(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_مجموعه_cap_Ntfn_Can_Receive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        أرجع new_cap;
    }
    حالة cap_پاسخ_cap: {
        cap_t new_cap;

        new_cap = cap_پاسخ_cap_مجموعه_cap_پاسخ_Can_Grant(
                      cap, cap_پاسخ_cap_get_cap_پاسخ_Can_Grant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        أرجع new_cap;
    }


    افتراضي:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, فراغ *region_پایه, کلمه_t user_اندازه, bool_t device_حافظه)
{
    /* Handle architecture-specific objects. */
    إذا (t >= (object_t) se_L4_Non_Arch_Object_Type_شمار) {
        أرجع Arch_createObject(t, region_پایه, user_اندازه, device_حافظه);
    }

    /* Create objects. */
    بدل ((api_object_t)t) {
    حالة seL4_TCBObject: {
        tcb_t *tcb;
        tcb = TCB_PTR((کلمه_t)region_پایه + TCB_جابه_جایی);
        /** AUXUPD: "(True, ptr_retyps 1
          (Ptr ((ptr_val \<acute>tcb) - ctcb_offset) :: (cte_C[5]) ptr)
            o (ptr_retyp \<acute>tcb))" */

        /* Setup non-zero parts of the TCB. */

        Arch_initContext(&tcb->tcbArch.tcbContext);
#اگر_تعریف_نشده CONFIG_KERNEL_MCS
        tcb->tcb_زمان_برش = CONFIG_زمان_برش;
#پایان_اگر
        tcb->tcbDomain = ksCurDomain;
#اگر_تعریف_نشده CONFIG_KERNEL_MCS
        /* Initialize the new TCB to the current core */
        SMP_COND_STATEMENT(tcb->tcbAffinity = getCurrentCPUIndex());
#پایان_اگر
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_درازی);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(گره_STATE(ksCurThread))->tcbName, TCB_NAME_درازی);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_درازی);
        tcbDebugAppend(tcb);
#پایان_اگر /* CONFIG_DEBUG_BUILD */

        أرجع cap_thread_cap_new(TCB_REF(tcb));
    }

    حالة seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        أرجع cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(region_پایه));

    حالة seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        أرجع cap_notification_cap_new(0, true, true,
                                        NTFN_REF(region_پایه));

    حالة se_L4_Cap_جدول_Object:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        أرجع cap_cnode_cap_new(user_اندازه, 0, 0, CTE_REF(region_پایه));

    حالة seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        أرجع cap_untyped_cap_new(0, !!device_حافظه, user_اندازه, کلمه_REF(region_پایه));

#اگر_تعریف_شده CONFIG_KERNEL_MCS
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
        أرجع cap_sched_context_cap_new(SC_REF(region_پایه), user_اندازه);

    حالة se_L4_پاسخ_Object:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        أرجع cap_پاسخ_cap_new(پاسخ_REF(region_پایه), true);
#پایان_اگر

    افتراضي:
        fail("Invalid object type");
    }
}

فراغ createNewObjects(object_t t, cte_t *والد,
                      cte_t *destCNode, کلمه_t dest_جابه_جایی, کلمه_t dest_درازی,
                      فراغ *region_پایه, کلمه_t user_اندازه, bool_t device_حافظه)
{
    کلمه_t object_اندازه;
    فراغ *بعدی_Free_Area;
    کلمه_t i;
    کلمه_t مجموع_Object_اندازه استفادهنشده;

    /* ghost check that we're visiting less bytes than the max object size */
    object_اندازه = get_Object_اندازه(t, user_اندازه);
    مجموع_Object_اندازه = dest_درازی << object_اندازه;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    بعدی_Free_Area = region_پایه;
    لكل (i = 0; i < dest_درازی; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (فراغ *)((کلمه_t)بعدی_Free_Area + (i << object_اندازه)), user_اندازه, device_حافظه);

        /* Insert the cap into the user's cspace. */
        درج_New_Cap(والد, &destCNode[dest_جابه_جایی + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
exception_t decodeInvocation(کلمه_t invLabel, کلمه_t درازی,
                             cptr_t cap_شاخص, cte_t *جایگاه, cap_t cap,
                             bool_t بلوک_2, bool_t call,
                             bool_t canDonate, bool_t اولی_Phase, کلمه_t *میانگیر)
#وإلا
exception_t decodeInvocation(کلمه_t invLabel, کلمه_t درازی,
                             cptr_t cap_شاخص, cte_t *جایگاه, cap_t cap,
                             bool_t بلوک_2, bool_t call,
                             کلمه_t *میانگیر)
#پایان_اگر
{
    إذا (isArchCap(cap)) {
        أرجع Arch_decodeInvocation(invLabel, درازی, cap_شاخص,
                                     جایگاه, cap, call, میانگیر);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", cap_شاخص);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", cap_شاخص);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_endpoint_cap:
        إذا (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      cap_شاخص);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        أرجع performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_پاسخ(cap), بلوک_2, call, canDonate);
#وإلا
        أرجع performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_پاسخ(cap), بلوک_2, call);
#پایان_اگر

    حالة cap_notification_cap: {
        إذا (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      cap_شاخص);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_پاسخ_cap:
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_Invocation_پاسخ(
                   گره_STATE(ksCurThread),
                   پاسخ_PTR(cap_پاسخ_cap_get_cap_پاسخ_Ptr(cap)),
                   cap_پاسخ_cap_get_cap_پاسخ_Can_Grant(cap));
#وإلا
    حالة cap_پاسخ_cap:
        إذا (unlikely(cap_پاسخ_cap_get_cap_پاسخ_Master(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      cap_شاخص);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_Invocation_پاسخ(
                   TCB_PTR(cap_پاسخ_cap_get_cap_TCBPtr(cap)), جایگاه,
                   cap_پاسخ_cap_get_cap_پاسخ_Can_Grant(cap));

#پایان_اگر

    حالة cap_thread_cap:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        إذا (unlikely(اولی_Phase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#پایان_اگر
        أرجع decodeTCBInvocation(invLabel, درازی, cap, جایگاه, call, میانگیر);

    حالة cap_domain_cap:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        إذا (unlikely(اولی_Phase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#پایان_اگر
        أرجع decodeDomainInvocation(invLabel, درازی, میانگیر);

    حالة cap_cnode_cap:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        إذا (unlikely(اولی_Phase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#پایان_اگر
        أرجع decodeCNodeInvocation(invLabel, درازی, cap, میانگیر);

    حالة cap_untyped_cap:
        أرجع decodeUntypedInvocation(invLabel, درازی, جایگاه, cap, call, میانگیر);

    حالة cap_irq_control_cap:
        أرجع decodeIRQControlInvocation(invLabel, درازی, جایگاه, میانگیر);

    حالة cap_irq_handler_cap:
        أرجع decodeIRQHandlerInvocation(invLabel,
                                          IDX_مقصد_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
        إذا (unlikely(اولی_Phase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        أرجع decodeSchedControlInvocation(invLabel, cap, درازی, میانگیر);

    حالة cap_sched_context_cap:
        إذا (unlikely(اولی_Phase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        أرجع decodeSchedContextInvocation(invLabel, sc, call);
#پایان_اگر
    افتراضي:
        fail("Invalid cap type");
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, کلمه_t badge,
                                       bool_t canGrant, bool_t can_Grant_پاسخ,
                                       bool_t بلوک_2, bool_t call, bool_t canDonate)
{
    sendIPC(بلوک_2, call, badge, canGrant, can_Grant_پاسخ, canDonate, گره_STATE(ksCurThread), ep);

    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t performInvocation_Endpoint(endpoint_t *ep, کلمه_t badge,
                                       bool_t canGrant, bool_t can_Grant_پاسخ,
                                       bool_t بلوک_2, bool_t call)
{
    sendIPC(بلوک_2, call, badge, canGrant, can_Grant_پاسخ, گره_STATE(ksCurThread), ep);

    أرجع EXCEPTION_NONE;
}
#پایان_اگر

exception_t performInvocation_Notification(notification_t *ntfn, کلمه_t badge)
{
    sendSignal(ntfn, badge);

    أرجع EXCEPTION_NONE;
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS
exception_t perform_Invocation_پاسخ(tcb_t *thread, پاسخ_t *پاسخ, bool_t canGrant)
{
    do_پاسخ_Transfer(thread, پاسخ, canGrant);
    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t perform_Invocation_پاسخ(tcb_t *thread, cte_t *جایگاه, bool_t canGrant)
{
    do_پاسخ_Transfer(گره_STATE(ksCurThread), thread, جایگاه, canGrant);
    أرجع EXCEPTION_NONE;
}
#پایان_اگر

کلمه_t CONST cap_get_cap_اندازه_بیتها(cap_t cap)
{

    cap_برچسب_t ctag;

    ctag = cap_get_capType(cap);

    بدل (ctag) {
    حالة cap_untyped_cap:
        أرجع cap_untyped_cap_get_cap_بلوک_اندازه(cap);

    حالة cap_endpoint_cap:
        أرجع se_L4_Endpoint_بیتها;

    حالة cap_notification_cap:
        أرجع se_L4_Notification_بیتها;

    حالة cap_cnode_cap:
        أرجع cap_cnode_cap_get_capCNodeRadix(cap) + se_L4_جایگاه_بیتها;

    حالة cap_thread_cap:
        أرجع seL4_TCBBits;

    حالة cap_zombie_cap: {
        کلمه_t type = cap_zombie_cap_get_capZombieType(cap);
        إذا (type == ZombieType_ZombieTCB) {
            أرجع seL4_TCBBits;
        }
        أرجع ZombieType_ZombieCNode(type) + se_L4_جایگاه_بیتها;
    }

    حالة cap_null_cap:
        أرجع 0;

    حالة cap_domain_cap:
        أرجع 0;

    حالة cap_پاسخ_cap:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        أرجع se_L4_پاسخ_بیتها;
#وإلا
        أرجع 0;
#پایان_اگر

    حالة cap_irq_control_cap:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#پایان_اگر
        أرجع 0;

    حالة cap_irq_handler_cap:
        أرجع 0;

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        أرجع cap_sched_context_cap_get_cap_SCSize_بیتها(cap);
#پایان_اگر

    افتراضي:
        أرجع cap_get_arch_Cap_اندازه_بیتها(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_برچسب_t ctag;

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
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
#پایان_اگر
        أرجع true;

    حالة cap_zombie_cap:
        أرجع true;

    حالة cap_domain_cap:
        أرجع false;

    حالة cap_پاسخ_cap:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        أرجع true;
#وإلا
        أرجع false;
#پایان_اگر

    حالة cap_irq_control_cap:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#پایان_اگر
        أرجع false;

    حالة cap_irq_handler_cap:
        أرجع false;

    افتراضي:
        أرجع cap_get_archCapIsPhysical(cap);
    }
}

فراغ *CONST cap_get_capPtr(cap_t cap)
{
    cap_برچسب_t ctag;

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

    حالة cap_پاسخ_cap:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
        أرجع پاسخ_PTR(cap_پاسخ_cap_get_cap_پاسخ_Ptr(cap));
#وإلا
        أرجع عدم;
#پایان_اگر

    حالة cap_irq_control_cap:
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#پایان_اگر
        أرجع عدم;

    حالة cap_irq_handler_cap:
        أرجع عدم;

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        أرجع SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#پایان_اگر

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
