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

word_t getObjectSize(word_t t, word_t userObjSize)
{
    যদি (t >= se_L4_Non_Arch_Object_Type_গুনতি) {
        প্রত্যাগম Arch_getObjectSize(t);
    } অন্যথা {
        বিকল্প (t) {
        পক্ষ seL4_TCBObject:
            প্রত্যাগম seL4_TCBBits;
        পক্ষ seL4_EndpointObject:
            প্রত্যাগম seL4_EndpointBits;
        পক্ষ seL4_NotificationObject:
            প্রত্যাগম seL4_NotificationBits;
        পক্ষ seL4_CapTableObject:
            প্রত্যাগম seL4_SlotBits + userObjSize;
        পক্ষ seL4_UntypedObject:
            প্রত্যাগম userObjSize;
#ifdef CONFIG_KERNEL_MCS
        পক্ষ seL4_SchedContextObject:
            প্রত্যাগম userObjSize;
        পক্ষ seL4_ReplyObject:
            প্রত্যাগম seL4_ReplyBits;
#endif
        স্বভাব:
            fail("Invalid object type");
            প্রত্যাগম 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    যদি (isArchCap(cap)) {
        প্রত্যাগম Arch_deriveCap(slot, cap);
    }

    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ভঙ্গ;

    পক্ষ cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ভঙ্গ;

    পক্ষ cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        যদি (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } অন্যথা {
            ret.cap = cap;
        }
        ভঙ্গ;

#ifndef CONFIG_KERNEL_MCS
    পক্ষ cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ভঙ্গ;
#endif
    স্বভাব:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    প্রত্যাগম ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    যদি (isArchCap(cap)) {
        প্রত্যাগম Arch_finaliseCap(cap, final);
    }

    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_endpoint_cap:
        যদি (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        প্রত্যাগম fc_ret;

    পক্ষ cap_notification_cap:
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
        প্রত্যাগম fc_ret;

    পক্ষ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        যদি (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            যদি (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                বিকল্প (thread_state_get_tsType(tcb->tcbState)) {
                পক্ষ ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    ভঙ্গ;
                পক্ষ ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    ভঙ্গ;
                স্বভাব:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        প্রত্যাগম fc_ret;
#endif
    পক্ষ cap_null_cap:
    পক্ষ cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        প্রত্যাগম fc_ret;
    }

    যদি (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_cnode_cap: {
        যদি (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            প্রত্যাগম fc_ret;
        }
        ভঙ্গ;
    }

    পক্ষ cap_thread_cap: {
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
            প্রত্যাগম fc_ret;
        }
        ভঙ্গ;
    }

#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_sched_context_cap:
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
            প্রত্যাগম fc_ret;
        }
        ভঙ্গ;
#endif

    পক্ষ cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        প্রত্যাগম fc_ret;

    পক্ষ cap_irq_handler_cap:
        যদি (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            প্রত্যাগম fc_ret;
        }
        ভঙ্গ;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    প্রত্যাগম fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_endpoint_cap:
        প্রত্যাগম cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    স্বভাব:
        প্রত্যাগম false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    বিকল্প (cap_get_capType(cap_a)) {
    পক্ষ cap_untyped_cap:
        যদি (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, a_শীর্ষ, b_শীর্ষ;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_শীর্ষ = aBase + MASK(cap_untyped_cap_get_capBlockSize(cap_a));
            b_শীর্ষ = bBase + MASK(cap_get_capSizeBits(cap_b));

            প্রত্যাগম (aBase <= bBase) && (b_শীর্ষ <= a_শীর্ষ) && (bBase <= b_শীর্ষ);
        }
        ভঙ্গ;

    পক্ষ cap_endpoint_cap:
        যদি (cap_get_capType(cap_b) == cap_endpoint_cap) {
            প্রত্যাগম cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_notification_cap:
        যদি (cap_get_capType(cap_b) == cap_notification_cap) {
            প্রত্যাগম cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_cnode_cap:
        যদি (cap_get_capType(cap_b) == cap_cnode_cap) {
            প্রত্যাগম (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        ভঙ্গ;

    পক্ষ cap_thread_cap:
        যদি (cap_get_capType(cap_b) == cap_thread_cap) {
            প্রত্যাগম cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_reply_cap:
        যদি (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            প্রত্যাগম cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#অন্যথা
            প্রত্যাগম cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
        }
        ভঙ্গ;

    পক্ষ cap_domain_cap:
        যদি (cap_get_capType(cap_b) == cap_domain_cap) {
            প্রত্যাগম true;
        }
        ভঙ্গ;

    পক্ষ cap_irq_control_cap:
        যদি (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            প্রত্যাগম true;
        }
        ভঙ্গ;

    পক্ষ cap_irq_handler_cap:
        যদি (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            প্রত্যাগম (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        ভঙ্গ;

#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_sched_context_cap:
        যদি (cap_get_capType(cap_b) == cap_sched_context_cap) {
            প্রত্যাগম (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        ভঙ্গ;
    পক্ষ cap_sched_control_cap:
        যদি (cap_get_capType(cap_b) == cap_sched_control_cap) {
            প্রত্যাগম true;
        }
        ভঙ্গ;
#endif
    স্বভাব:
        যদি (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            প্রত্যাগম Arch_sameRegionAs(cap_a, cap_b);
        }
        ভঙ্গ;
    }

    প্রত্যাগম false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    যদি (cap_get_capType(cap_a) == cap_untyped_cap) {
        প্রত্যাগম false;
    }
    যদি (cap_get_capType(cap_a) == cap_irq_control_cap) {
        প্রত্যাগম false;
    }
    যদি (isArchCap(cap_a) && isArchCap(cap_b)) {
        প্রত্যাগম Arch_sameObjectAs(cap_a, cap_b);
    }
    প্রত্যাগম sameRegionAs(cap_a, cap_b);
}

cap_t CONST updateCapData(bool_t preserve, word_t newData, cap_t cap)
{
    যদি (isArchCap(cap)) {
        প্রত্যাগম Arch_updateCapData(preserve, newData, cap);
    }

    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_endpoint_cap:
        যদি (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            প্রত্যাগম cap_endpoint_cap_set_capEPBadge(cap, newData);
        } অন্যথা {
            প্রত্যাগম cap_null_cap_new();
        }

    পক্ষ cap_notification_cap:
        যদি (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            প্রত্যাগম cap_notification_cap_set_capNtfnBadge(cap, newData);
        } অন্যথা {
            প্রত্যাগম cap_null_cap_new();
        }

    পক্ষ cap_cnode_cap: {
        word_t guard, guardSize;
        seL4_CNode_CapData_t w = { .শব্দগুলি = { newData } };

        guardSize = seL4_CNode_CapData_get_guardSize(w);

        যদি (guardSize + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            প্রত্যাগম cap_null_cap_new();
        } অন্যথা {
            cap_t new_cap;

            guard = seL4_CNode_CapData_get_guard(w) & MASK(guardSize);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_capCNodeGuardSize(new_cap,
                                                          guardSize);

            প্রত্যাগম new_cap;
        }
    }

    স্বভাব:
        প্রত্যাগম cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    যদি (isArchCap(cap)) {
        প্রত্যাগম Arch_maskCapRights(cap_rights, cap);
    }

    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_null_cap:
    পক্ষ cap_domain_cap:
    পক্ষ cap_cnode_cap:
    পক্ষ cap_untyped_cap:
    পক্ষ cap_irq_control_cap:
    পক্ষ cap_irq_handler_cap:
    পক্ষ cap_zombie_cap:
    পক্ষ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_sched_context_cap:
    পক্ষ cap_sched_control_cap:
#endif
        প্রত্যাগম cap;

    পক্ষ cap_endpoint_cap: {
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

        প্রত্যাগম new_cap;
    }

    পক্ষ cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        প্রত্যাগম new_cap;
    }
    পক্ষ cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        প্রত্যাগম new_cap;
    }


    স্বভাব:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, শূন্য *regionBase, word_t userSize, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    যদি (t >= (object_t) se_L4_Non_Arch_Object_Type_গুনতি) {
        প্রত্যাগম Arch_createObject(t, regionBase, userSize, deviceMemory);
    }

    /* Create objects. */
    বিকল্প ((api_object_t)t) {
    পক্ষ seL4_TCBObject: {
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
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(গ্রন্থি_STATE(ksCurThread))->tcbName, TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_LENGTH);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        প্রত্যাগম cap_thread_cap_new(TCB_REF(tcb));
    }

    পক্ষ seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        প্রত্যাগম cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    পক্ষ seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        প্রত্যাগম cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    পক্ষ seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        প্রত্যাগম cap_cnode_cap_new(userSize, 0, 0, CTE_REF(regionBase));

    পক্ষ seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        প্রত্যাগম cap_untyped_cap_new(0, !!deviceMemory, userSize, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
    পক্ষ seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        প্রত্যাগম cap_sched_context_cap_new(SC_REF(regionBase), userSize);

    পক্ষ seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        প্রত্যাগম cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    স্বভাব:
        fail("Invalid object type");
    }
}

শূন্য createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t destLength,
                      শূন্য *regionBase, word_t userSize, bool_t deviceMemory)
{
    word_t objectSize;
    শূন্য *nextFreeArea;
    word_t i;
    word_t totalObjectSize UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    objectSize = getObjectSize(t, userSize);
    totalObjectSize = destLength << objectSize;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    nextFreeArea = regionBase;
    প্রতি (i = 0; i < destLength; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (শূন্য *)((word_t)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        /* Insert the cap into the user's cspace. */
        ঢোকাও_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *buffer)
#অন্যথা
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *buffer)
#endif
{
    যদি (isArchCap(cap)) {
        প্রত্যাগম Arch_decodeInvocation(invLabel, length, capIndex,
                                     slot, cap, call, buffer);
    }

    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;

    পক্ষ cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;
        প্রত্যাগম EXCEPTION_SYSCALL_ERROR;

    পক্ষ cap_endpoint_cap:
        যদি (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        প্রত্যাগম performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#অন্যথা
        প্রত্যাগম performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    পক্ষ cap_notification_cap: {
        যদি (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        প্রত্যাগম performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_reply_cap:
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        প্রত্যাগম performInvocation_Reply(
                   গ্রন্থি_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#অন্যথা
    পক্ষ cap_reply_cap:
        যদি (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        প্রত্যাগম performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    পক্ষ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        যদি (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
#endif
        প্রত্যাগম decodeTCBInvocation(invLabel, length, cap, slot, call, buffer);

    পক্ষ cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        যদি (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
#endif
        প্রত্যাগম decodeDomainInvocation(invLabel, length, buffer);

    পক্ষ cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        যদি (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
#endif
        প্রত্যাগম decodeCNodeInvocation(invLabel, length, cap, buffer);

    পক্ষ cap_untyped_cap:
        প্রত্যাগম decodeUntypedInvocation(invLabel, length, slot, cap, call, buffer);

    পক্ষ cap_irq_control_cap:
        প্রত্যাগম decodeIRQControlInvocation(invLabel, length, slot, buffer);

    পক্ষ cap_irq_handler_cap:
        প্রত্যাগম decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_sched_control_cap:
        যদি (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        প্রত্যাগম decodeSchedControlInvocation(invLabel, cap, length, buffer);

    পক্ষ cap_sched_context_cap:
        যদি (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            প্রত্যাগম EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        প্রত্যাগম decodeSchedContextInvocation(invLabel, sc, call);
#endif
    স্বভাব:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, গ্রন্থি_STATE(ksCurThread), ep);

    প্রত্যাগম EXCEPTION_NONE;
}
#অন্যথা
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, গ্রন্থি_STATE(ksCurThread), ep);

    প্রত্যাগম EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    প্রত্যাগম EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    প্রত্যাগম EXCEPTION_NONE;
}
#অন্যথা
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(গ্রন্থি_STATE(ksCurThread), thread, slot, canGrant);
    প্রত্যাগম EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_capSizeBits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    বিকল্প (ctag) {
    পক্ষ cap_untyped_cap:
        প্রত্যাগম cap_untyped_cap_get_capBlockSize(cap);

    পক্ষ cap_endpoint_cap:
        প্রত্যাগম seL4_EndpointBits;

    পক্ষ cap_notification_cap:
        প্রত্যাগম seL4_NotificationBits;

    পক্ষ cap_cnode_cap:
        প্রত্যাগম cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    পক্ষ cap_thread_cap:
        প্রত্যাগম seL4_TCBBits;

    পক্ষ cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        যদি (type == ZombieType_ZombieTCB) {
            প্রত্যাগম seL4_TCBBits;
        }
        প্রত্যাগম ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    পক্ষ cap_null_cap:
        প্রত্যাগম 0;

    পক্ষ cap_domain_cap:
        প্রত্যাগম 0;

    পক্ষ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        প্রত্যাগম seL4_ReplyBits;
#অন্যথা
        প্রত্যাগম 0;
#endif

    পক্ষ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_sched_control_cap:
#endif
        প্রত্যাগম 0;

    পক্ষ cap_irq_handler_cap:
        প্রত্যাগম 0;

#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_sched_context_cap:
        প্রত্যাগম cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    স্বভাব:
        প্রত্যাগম cap_get_archCapSizeBits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    বিকল্প (ctag) {
    পক্ষ cap_untyped_cap:
        প্রত্যাগম true;

    পক্ষ cap_endpoint_cap:
        প্রত্যাগম true;

    পক্ষ cap_notification_cap:
        প্রত্যাগম true;

    পক্ষ cap_cnode_cap:
        প্রত্যাগম true;

    পক্ষ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_sched_context_cap:
#endif
        প্রত্যাগম true;

    পক্ষ cap_zombie_cap:
        প্রত্যাগম true;

    পক্ষ cap_domain_cap:
        প্রত্যাগম false;

    পক্ষ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        প্রত্যাগম true;
#অন্যথা
        প্রত্যাগম false;
#endif

    পক্ষ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_sched_control_cap:
#endif
        প্রত্যাগম false;

    পক্ষ cap_irq_handler_cap:
        প্রত্যাগম false;

    স্বভাব:
        প্রত্যাগম cap_get_archCapIsPhysical(cap);
    }
}

শূন্য *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    বিকল্প (ctag) {
    পক্ষ cap_untyped_cap:
        প্রত্যাগম WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    পক্ষ cap_endpoint_cap:
        প্রত্যাগম EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    পক্ষ cap_notification_cap:
        প্রত্যাগম NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    পক্ষ cap_cnode_cap:
        প্রত্যাগম CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    পক্ষ cap_thread_cap:
        প্রত্যাগম TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    পক্ষ cap_zombie_cap:
        প্রত্যাগম CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    পক্ষ cap_domain_cap:
        প্রত্যাগম রিক্ত;

    পক্ষ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        প্রত্যাগম REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#অন্যথা
        প্রত্যাগম রিক্ত;
#endif

    পক্ষ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_sched_control_cap:
#endif
        প্রত্যাগম রিক্ত;

    পক্ষ cap_irq_handler_cap:
        প্রত্যাগম রিক্ত;

#ifdef CONFIG_KERNEL_MCS
    পক্ষ cap_sched_context_cap:
        প্রত্যাগম SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

    স্বভাব:
        প্রত্যাগম cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    যদি (isArchCap(derivedCap)) {
        প্রত্যাগম Arch_isCapRevocable(derivedCap, srcCap);
    }
    বিকল্প (cap_get_capType(derivedCap)) {
    পক্ষ cap_endpoint_cap:
        প্রত্যাগম (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    পক্ষ cap_notification_cap:
        প্রত্যাগম (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    পক্ষ cap_irq_handler_cap:
        প্রত্যাগম (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    পক্ষ cap_untyped_cap:
        প্রত্যাগম true;

    স্বভাব:
        প্রত্যাগম false;
    }
}
