/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <arch/object/objecttype.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <object/objecttype.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <object/notification.h>
#অন্তর্ভাব <object/endpoint.h>
#অন্তর্ভাব <object/cnode.h>
#অন্তর্ভাব <object/interrupt.h>
#ifdef CONFIG_KERNEL_MCS
#অন্তর্ভাব <object/schedcontext.h>
#অন্তর্ভাব <object/schedcontrol.h>
#endif
#অন্তর্ভাব <object/tcb.h>
#অন্তর্ভাব <object/untyped.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <machine.h>
#অন্তর্ভাব <util.h>
#অন্তর্ভাব <string.h>

word_t get_Object_আকার(word_t t, word_t user_Obj_আকার)
{
    যদি (t >= se_L4_Non_Arch_Object_Type_গুনতি) {
        ফেরত Arch_get_Object_আকার(t);
    } অন্যথা {
        নির্বাচন (t) {
        ক্ষেত্র seL4_TCBObject:
            ফেরত seL4_TCBBits;
        ক্ষেত্র seL4_EndpointObject:
            ফেরত seL4_EndpointBits;
        ক্ষেত্র seL4_NotificationObject:
            ফেরত seL4_NotificationBits;
        ক্ষেত্র seL4_CapTableObject:
            ফেরত seL4_SlotBits + user_Obj_আকার;
        ক্ষেত্র seL4_UntypedObject:
            ফেরত user_Obj_আকার;
#ifdef CONFIG_KERNEL_MCS
        ক্ষেত্র seL4_SchedContextObject:
            ফেরত user_Obj_আকার;
        ক্ষেত্র seL4_ReplyObject:
            ফেরত seL4_ReplyBits;
#endif
        পূর্বনির্ধারিত:
            fail("Invalid object type");
            ফেরত 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    যদি (isArchCap(cap)) {
        ফেরত Arch_deriveCap(slot, cap);
    }

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        বিরতি;

    ক্ষেত্র cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        বিরতি;

    ক্ষেত্র cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        যদি (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } অন্যথা {
            ret.cap = cap;
        }
        বিরতি;

#ifndef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        বিরতি;
#endif
    পূর্বনির্ধারিত:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    ফেরত ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    যদি (isArchCap(cap)) {
        ফেরত Arch_finaliseCap(cap, final);
    }

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_endpoint_cap:
        যদি (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ফেরত fc_ret;

    ক্ষেত্র cap_notification_cap:
        যদি (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ifdef CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#endif
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ফেরত fc_ret;

    ক্ষেত্র cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        যদি (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            যদি (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                নির্বাচন (thread_state_get_tsType(tcb->tcbState)) {
                ক্ষেত্র ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    বিরতি;
                ক্ষেত্র ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    বিরতি;
                পূর্বনির্ধারিত:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ফেরত fc_ret;
#endif
    ক্ষেত্র cap_null_cap:
    ক্ষেত্র cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ফেরত fc_ret;
    }

    যদি (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_cnode_cap: {
        যদি (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            ফেরত fc_ret;
        }
        বিরতি;
    }

    ক্ষেত্র cap_thread_cap: {
        যদি (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ifdef CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            যদি (sc) {
                schedContext_unbindTCB(sc);
                যদি (sc->scYieldFrom) {
                    schedContext_completeYieldTo(sc->scYieldFrom);
                }
            }
#endif
            suspend(tcb);
#ifdef CONFIG_DEBUG_BUILD
            tcbDebugRemove(tcb);
#endif
            Arch_prepareThreadDelete(tcb);
            fc_ret.remainder =
                Zombie_new(
                    tcbArchCNodeEntries,
                    ZombieType_ZombieTCB,
                    CTE_REF(cte_ptr)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            ফেরত fc_ret;
        }
        বিরতি;
    }

#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_sched_context_cap:
        যদি (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            যদি (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            ফেরত fc_ret;
        }
        বিরতি;
#endif

    ক্ষেত্র cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        ফেরত fc_ret;

    ক্ষেত্র cap_irq_handler_cap:
        যদি (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ফেরত fc_ret;
        }
        বিরতি;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ফেরত fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_endpoint_cap:
        ফেরত cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    পূর্বনির্ধারিত:
        ফেরত false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    নির্বাচন (cap_get_capType(cap_a)) {
    ক্ষেত্র cap_untyped_cap:
        যদি (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, a_শীর্ষ, b_শীর্ষ;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_শীর্ষ = aBase + MASK(cap_untyped_cap_get_cap_Block_আকার(cap_a));
            b_শীর্ষ = bBase + MASK(cap_get_cap_আকার_Bits(cap_b));

            ফেরত (aBase <= bBase) && (b_শীর্ষ <= a_শীর্ষ) && (bBase <= b_শীর্ষ);
        }
        বিরতি;

    ক্ষেত্র cap_endpoint_cap:
        যদি (cap_get_capType(cap_b) == cap_endpoint_cap) {
            ফেরত cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_notification_cap:
        যদি (cap_get_capType(cap_b) == cap_notification_cap) {
            ফেরত cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_cnode_cap:
        যদি (cap_get_capType(cap_b) == cap_cnode_cap) {
            ফেরত (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        বিরতি;

    ক্ষেত্র cap_thread_cap:
        যদি (cap_get_capType(cap_b) == cap_thread_cap) {
            ফেরত cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_reply_cap:
        যদি (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            ফেরত cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#অন্যথা
            ফেরত cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
        }
        বিরতি;

    ক্ষেত্র cap_domain_cap:
        যদি (cap_get_capType(cap_b) == cap_domain_cap) {
            ফেরত true;
        }
        বিরতি;

    ক্ষেত্র cap_irq_control_cap:
        যদি (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            ফেরত true;
        }
        বিরতি;

    ক্ষেত্র cap_irq_handler_cap:
        যদি (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            ফেরত (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        বিরতি;

#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_sched_context_cap:
        যদি (cap_get_capType(cap_b) == cap_sched_context_cap) {
            ফেরত (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        বিরতি;
    ক্ষেত্র cap_sched_control_cap:
        যদি (cap_get_capType(cap_b) == cap_sched_control_cap) {
            ফেরত true;
        }
        বিরতি;
#endif
    পূর্বনির্ধারিত:
        যদি (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            ফেরত Arch_sameRegionAs(cap_a, cap_b);
        }
        বিরতি;
    }

    ফেরত false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    যদি (cap_get_capType(cap_a) == cap_untyped_cap) {
        ফেরত false;
    }
    যদি (cap_get_capType(cap_a) == cap_irq_control_cap) {
        ফেরত false;
    }
    যদি (isArchCap(cap_a) && isArchCap(cap_b)) {
        ফেরত Arch_sameObjectAs(cap_a, cap_b);
    }
    ফেরত sameRegionAs(cap_a, cap_b);
}

cap_t CONST update_Cap_তথ্য(bool_t preserve, word_t new_তথ্য, cap_t cap)
{
    যদি (isArchCap(cap)) {
        ফেরত Arch_update_Cap_তথ্য(preserve, new_তথ্য, cap);
    }

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_endpoint_cap:
        যদি (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            ফেরত cap_endpoint_cap_set_capEPBadge(cap, new_তথ্য);
        } অন্যথা {
            ফেরত cap_null_cap_new();
        }

    ক্ষেত্র cap_notification_cap:
        যদি (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            ফেরত cap_notification_cap_set_capNtfnBadge(cap, new_তথ্য);
        } অন্যথা {
            ফেরত cap_null_cap_new();
        }

    ক্ষেত্র cap_cnode_cap: {
        word_t guard, guard_আকার;
        se_L4_CNode_Cap_তথ্য_t w = { .শব্দগুলি = { new_তথ্য } };

        guard_আকার = se_L4_CNode_Cap_তথ্য_get_guard_আকার(w);

        যদি (guard_আকার + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            ফেরত cap_null_cap_new();
        } অন্যথা {
            cap_t new_cap;

            guard = se_L4_CNode_Cap_তথ্য_get_guard(w) & MASK(guard_আকার);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_cap_CNode_Guard_আকার(new_cap,
                                                          guard_আকার);

            ফেরত new_cap;
        }
    }

    পূর্বনির্ধারিত:
        ফেরত cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    যদি (isArchCap(cap)) {
        ফেরত Arch_maskCapRights(cap_rights, cap);
    }

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_null_cap:
    ক্ষেত্র cap_domain_cap:
    ক্ষেত্র cap_cnode_cap:
    ক্ষেত্র cap_untyped_cap:
    ক্ষেত্র cap_irq_control_cap:
    ক্ষেত্র cap_irq_handler_cap:
    ক্ষেত্র cap_zombie_cap:
    ক্ষেত্র cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_sched_context_cap:
    ক্ষেত্র cap_sched_control_cap:
#endif
        ফেরত cap;

    ক্ষেত্র cap_endpoint_cap: {
        cap_t new_cap;

        new_cap = cap_endpoint_cap_set_capCanSend(
                      cap, cap_endpoint_cap_get_capCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_endpoint_cap_set_capCanReceive(
                      new_cap, cap_endpoint_cap_get_capCanReceive(cap) &
                      seL4_CapRights_get_capAllowRead(cap_rights));
        new_cap = cap_endpoint_cap_set_capCanGrant(
                      new_cap, cap_endpoint_cap_get_capCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        new_cap = cap_endpoint_cap_set_capCanGrantReply(
                      new_cap, cap_endpoint_cap_get_capCanGrantReply(cap) &
                      seL4_CapRights_get_capAllowGrantReply(cap_rights));

        ফেরত new_cap;
    }

    ক্ষেত্র cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        ফেরত new_cap;
    }
    ক্ষেত্র cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        ফেরত new_cap;
    }


    পূর্বনির্ধারিত:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, শূন্য *regionBase, word_t user_আকার, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    যদি (t >= (object_t) se_L4_Non_Arch_Object_Type_গুনতি) {
        ফেরত Arch_createObject(t, regionBase, user_আকার, deviceMemory);
    }

    /* Create objects. */
    নির্বাচন ((api_object_t)t) {
    ক্ষেত্র seL4_TCBObject: {
        tcb_t *tcb;
        tcb = TCB_PTR((word_t)regionBase + TCB_OFFSET);
        /** AUXUPD: "(True, ptr_retyps 1
          (Ptr ((ptr_val \<acute>tcb) - ctcb_offset) :: (cte_C[5]) ptr)
            o (ptr_retyp \<acute>tcb))" */

        /* Setup non-zero parts of the TCB. */

        Arch_initContext(&tcb->tcbArch.tcbContext);
#ifndef CONFIG_KERNEL_MCS
        tcb->tcbTimeSlice = CONFIG_TIME_SLICE;
#endif
        tcb->tcbDomain = ksCurDomain;
#ifndef CONFIG_KERNEL_MCS
        /* Initialize the new TCB to the current core */
        SMP_COND_STATEMENT(tcb->tcbAffinity = getCurrentCPUIndex());
#endif
#ifdef CONFIG_DEBUG_BUILD
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_দৈর্ঘ্য);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(গ্রন্থি_STATE(ksCurThread))->tcbName, TCB_NAME_দৈর্ঘ্য);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_দৈর্ঘ্য);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        ফেরত cap_thread_cap_new(TCB_REF(tcb));
    }

    ক্ষেত্র seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        ফেরত cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    ক্ষেত্র seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        ফেরত cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    ক্ষেত্র seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        ফেরত cap_cnode_cap_new(user_আকার, 0, 0, CTE_REF(regionBase));

    ক্ষেত্র seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        ফেরত cap_untyped_cap_new(0, !!deviceMemory, user_আকার, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        ফেরত cap_sched_context_cap_new(SC_REF(regionBase), user_আকার);

    ক্ষেত্র seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        ফেরত cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    পূর্বনির্ধারিত:
        fail("Invalid object type");
    }
}

শূন্য createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t dest_দৈর্ঘ্য,
                      শূন্য *regionBase, word_t user_আকার, bool_t deviceMemory)
{
    word_t object_আকার;
    শূন্য *পরবর্তী_Free_Area;
    word_t i;
    word_t যোগ_Object_আকার UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    object_আকার = get_Object_আকার(t, user_আকার);
    যোগ_Object_আকার = dest_দৈর্ঘ্য << object_আকার;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    পরবর্তী_Free_Area = regionBase;
    জন্য (i = 0; i < dest_দৈর্ঘ্য; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (শূন্য *)((word_t)পরবর্তী_Free_Area + (i << object_আকার)), user_আকার, deviceMemory);

        /* Insert the cap into the user's cspace. */
        ঢোকাও_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t দৈর্ঘ্য,
                             cptr_t cap_সূচক, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *বাফার)
#অন্যথা
exception_t decodeInvocation(word_t invLabel, word_t দৈর্ঘ্য,
                             cptr_t cap_সূচক, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *বাফার)
#endif
{
    যদি (isArchCap(cap)) {
        ফেরত Arch_decodeInvocation(invLabel, দৈর্ঘ্য, cap_সূচক,
                                     slot, cap, call, বাফার);
    }

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", cap_সূচক);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;
        ফেরত EXCEPTION_SYSCALL_ERROR;

    ক্ষেত্র cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", cap_সূচক);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;
        ফেরত EXCEPTION_SYSCALL_ERROR;

    ক্ষেত্র cap_endpoint_cap:
        যদি (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      cap_সূচক);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        ফেরত performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#অন্যথা
        ফেরত performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    ক্ষেত্র cap_notification_cap: {
        যদি (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      cap_সূচক);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_reply_cap:
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performInvocation_Reply(
                   গ্রন্থি_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#অন্যথা
    ক্ষেত্র cap_reply_cap:
        যদি (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      cap_সূচক);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    ক্ষেত্র cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        যদি (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ফেরত decodeTCBInvocation(invLabel, দৈর্ঘ্য, cap, slot, call, বাফার);

    ক্ষেত্র cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        যদি (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ফেরত decodeDomainInvocation(invLabel, দৈর্ঘ্য, বাফার);

    ক্ষেত্র cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        যদি (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ফেরত decodeCNodeInvocation(invLabel, দৈর্ঘ্য, cap, বাফার);

    ক্ষেত্র cap_untyped_cap:
        ফেরত decodeUntypedInvocation(invLabel, দৈর্ঘ্য, slot, cap, call, বাফার);

    ক্ষেত্র cap_irq_control_cap:
        ফেরত decodeIRQControlInvocation(invLabel, দৈর্ঘ্য, slot, বাফার);

    ক্ষেত্র cap_irq_handler_cap:
        ফেরত decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_sched_control_cap:
        যদি (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত decodeSchedControlInvocation(invLabel, cap, দৈর্ঘ্য, বাফার);

    ক্ষেত্র cap_sched_context_cap:
        যদি (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        ফেরত decodeSchedContextInvocation(invLabel, sc, call);
#endif
    পূর্বনির্ধারিত:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, গ্রন্থি_STATE(ksCurThread), ep);

    ফেরত EXCEPTION_NONE;
}
#অন্যথা
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, গ্রন্থি_STATE(ksCurThread), ep);

    ফেরত EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    ফেরত EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    ফেরত EXCEPTION_NONE;
}
#অন্যথা
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(গ্রন্থি_STATE(ksCurThread), thread, slot, canGrant);
    ফেরত EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_cap_আকার_Bits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    নির্বাচন (ctag) {
    ক্ষেত্র cap_untyped_cap:
        ফেরত cap_untyped_cap_get_cap_Block_আকার(cap);

    ক্ষেত্র cap_endpoint_cap:
        ফেরত seL4_EndpointBits;

    ক্ষেত্র cap_notification_cap:
        ফেরত seL4_NotificationBits;

    ক্ষেত্র cap_cnode_cap:
        ফেরত cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    ক্ষেত্র cap_thread_cap:
        ফেরত seL4_TCBBits;

    ক্ষেত্র cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        যদি (type == ZombieType_ZombieTCB) {
            ফেরত seL4_TCBBits;
        }
        ফেরত ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    ক্ষেত্র cap_null_cap:
        ফেরত 0;

    ক্ষেত্র cap_domain_cap:
        ফেরত 0;

    ক্ষেত্র cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ফেরত seL4_ReplyBits;
#অন্যথা
        ফেরত 0;
#endif

    ক্ষেত্র cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_sched_control_cap:
#endif
        ফেরত 0;

    ক্ষেত্র cap_irq_handler_cap:
        ফেরত 0;

#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_sched_context_cap:
        ফেরত cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    পূর্বনির্ধারিত:
        ফেরত cap_get_arch_Cap_আকার_Bits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    নির্বাচন (ctag) {
    ক্ষেত্র cap_untyped_cap:
        ফেরত true;

    ক্ষেত্র cap_endpoint_cap:
        ফেরত true;

    ক্ষেত্র cap_notification_cap:
        ফেরত true;

    ক্ষেত্র cap_cnode_cap:
        ফেরত true;

    ক্ষেত্র cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_sched_context_cap:
#endif
        ফেরত true;

    ক্ষেত্র cap_zombie_cap:
        ফেরত true;

    ক্ষেত্র cap_domain_cap:
        ফেরত false;

    ক্ষেত্র cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ফেরত true;
#অন্যথা
        ফেরত false;
#endif

    ক্ষেত্র cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_sched_control_cap:
#endif
        ফেরত false;

    ক্ষেত্র cap_irq_handler_cap:
        ফেরত false;

    পূর্বনির্ধারিত:
        ফেরত cap_get_archCapIsPhysical(cap);
    }
}

শূন্য *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    নির্বাচন (ctag) {
    ক্ষেত্র cap_untyped_cap:
        ফেরত WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    ক্ষেত্র cap_endpoint_cap:
        ফেরত EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    ক্ষেত্র cap_notification_cap:
        ফেরত NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    ক্ষেত্র cap_cnode_cap:
        ফেরত CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    ক্ষেত্র cap_thread_cap:
        ফেরত TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    ক্ষেত্র cap_zombie_cap:
        ফেরত CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    ক্ষেত্র cap_domain_cap:
        ফেরত ফাঁকা;

    ক্ষেত্র cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ফেরত REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#অন্যথা
        ফেরত ফাঁকা;
#endif

    ক্ষেত্র cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_sched_control_cap:
#endif
        ফেরত ফাঁকা;

    ক্ষেত্র cap_irq_handler_cap:
        ফেরত ফাঁকা;

#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র cap_sched_context_cap:
        ফেরত SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

    পূর্বনির্ধারিত:
        ফেরত cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    যদি (isArchCap(derivedCap)) {
        ফেরত Arch_isCapRevocable(derivedCap, srcCap);
    }
    নির্বাচন (cap_get_capType(derivedCap)) {
    ক্ষেত্র cap_endpoint_cap:
        ফেরত (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    ক্ষেত্র cap_notification_cap:
        ফেরত (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    ক্ষেত্র cap_irq_handler_cap:
        ফেরত (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    ক্ষেত্র cap_untyped_cap:
        ফেরত true;

    পূর্বনির্ধারিত:
        ফেরত false;
    }
}
