/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <arch/object/objecttype.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <object/objecttype.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <object/notification.h>
#அந்தர்பாவ <object/endpoint.h>
#அந்தர்பாவ <object/cnode.h>
#அந்தர்பாவ <object/interrupt.h>
#ifdef CONFIG_KERNEL_MCS
#அந்தர்பாவ <object/schedcontext.h>
#அந்தர்பாவ <object/schedcontrol.h>
#endif
#அந்தர்பாவ <object/tcb.h>
#அந்தர்பாவ <object/untyped.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <kernel/vspace.h>
#அந்தர்பாவ <machine.h>
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <string.h>

word_t getObjectSize(word_t t, word_t userObjSize)
{
    எனில் (t >= se_L4_Non_Arch_Object_Type_மொத்தம்) {
        திரும்பு Arch_getObjectSize(t);
    } இல்லையெனில் {
        தேர்வு (t) {
        நிலை seL4_TCBObject:
            திரும்பு seL4_TCBBits;
        நிலை seL4_EndpointObject:
            திரும்பு seL4_EndpointBits;
        நிலை seL4_NotificationObject:
            திரும்பு seL4_NotificationBits;
        நிலை seL4_CapTableObject:
            திரும்பு seL4_SlotBits + userObjSize;
        நிலை seL4_UntypedObject:
            திரும்பு userObjSize;
#ifdef CONFIG_KERNEL_MCS
        நிலை seL4_SchedContextObject:
            திரும்பு userObjSize;
        நிலை seL4_ReplyObject:
            திரும்பு seL4_ReplyBits;
#endif
        இயல்பு:
            fail("Invalid object type");
            திரும்பு 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    எனில் (isArchCap(cap)) {
        திரும்பு Arch_deriveCap(slot, cap);
    }

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        நிறுத்து;

    நிலை cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        நிறுத்து;

    நிலை cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        எனில் (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } இல்லையெனில் {
            ret.cap = cap;
        }
        நிறுத்து;

#ifndef CONFIG_KERNEL_MCS
    நிலை cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        நிறுத்து;
#endif
    இயல்பு:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    திரும்பு ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    எனில் (isArchCap(cap)) {
        திரும்பு Arch_finaliseCap(cap, final);
    }

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_endpoint_cap:
        எனில் (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        திரும்பு fc_ret;

    நிலை cap_notification_cap:
        எனில் (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ifdef CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#endif
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        திரும்பு fc_ret;

    நிலை cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        எனில் (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            எனில் (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                தேர்வு (thread_state_get_tsType(tcb->tcbState)) {
                நிலை ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    நிறுத்து;
                நிலை ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    நிறுத்து;
                இயல்பு:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        திரும்பு fc_ret;
#endif
    நிலை cap_null_cap:
    நிலை cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        திரும்பு fc_ret;
    }

    எனில் (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_cnode_cap: {
        எனில் (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            திரும்பு fc_ret;
        }
        நிறுத்து;
    }

    நிலை cap_thread_cap: {
        எனில் (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ifdef CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            எனில் (sc) {
                schedContext_unbindTCB(sc);
                எனில் (sc->scYieldFrom) {
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
            திரும்பு fc_ret;
        }
        நிறுத்து;
    }

#ifdef CONFIG_KERNEL_MCS
    நிலை cap_sched_context_cap:
        எனில் (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            எனில் (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            திரும்பு fc_ret;
        }
        நிறுத்து;
#endif

    நிலை cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        திரும்பு fc_ret;

    நிலை cap_irq_handler_cap:
        எனில் (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            திரும்பு fc_ret;
        }
        நிறுத்து;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    திரும்பு fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_endpoint_cap:
        திரும்பு cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    இயல்பு:
        திரும்பு false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    தேர்வு (cap_get_capType(cap_a)) {
    நிலை cap_untyped_cap:
        எனில் (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, a_உச்சி, b_உச்சி;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_உச்சி = aBase + MASK(cap_untyped_cap_get_capBlockSize(cap_a));
            b_உச்சி = bBase + MASK(cap_get_capSizeBits(cap_b));

            திரும்பு (aBase <= bBase) && (b_உச்சி <= a_உச்சி) && (bBase <= b_உச்சி);
        }
        நிறுத்து;

    நிலை cap_endpoint_cap:
        எனில் (cap_get_capType(cap_b) == cap_endpoint_cap) {
            திரும்பு cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_notification_cap:
        எனில் (cap_get_capType(cap_b) == cap_notification_cap) {
            திரும்பு cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_cnode_cap:
        எனில் (cap_get_capType(cap_b) == cap_cnode_cap) {
            திரும்பு (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        நிறுத்து;

    நிலை cap_thread_cap:
        எனில் (cap_get_capType(cap_b) == cap_thread_cap) {
            திரும்பு cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_reply_cap:
        எனில் (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            திரும்பு cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#இல்லையெனில்
            திரும்பு cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
        }
        நிறுத்து;

    நிலை cap_domain_cap:
        எனில் (cap_get_capType(cap_b) == cap_domain_cap) {
            திரும்பு true;
        }
        நிறுத்து;

    நிலை cap_irq_control_cap:
        எனில் (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            திரும்பு true;
        }
        நிறுத்து;

    நிலை cap_irq_handler_cap:
        எனில் (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            திரும்பு (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        நிறுத்து;

#ifdef CONFIG_KERNEL_MCS
    நிலை cap_sched_context_cap:
        எனில் (cap_get_capType(cap_b) == cap_sched_context_cap) {
            திரும்பு (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        நிறுத்து;
    நிலை cap_sched_control_cap:
        எனில் (cap_get_capType(cap_b) == cap_sched_control_cap) {
            திரும்பு true;
        }
        நிறுத்து;
#endif
    இயல்பு:
        எனில் (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            திரும்பு Arch_sameRegionAs(cap_a, cap_b);
        }
        நிறுத்து;
    }

    திரும்பு false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    எனில் (cap_get_capType(cap_a) == cap_untyped_cap) {
        திரும்பு false;
    }
    எனில் (cap_get_capType(cap_a) == cap_irq_control_cap) {
        திரும்பு false;
    }
    எனில் (isArchCap(cap_a) && isArchCap(cap_b)) {
        திரும்பு Arch_sameObjectAs(cap_a, cap_b);
    }
    திரும்பு sameRegionAs(cap_a, cap_b);
}

cap_t CONST updateCapData(bool_t preserve, word_t newData, cap_t cap)
{
    எனில் (isArchCap(cap)) {
        திரும்பு Arch_updateCapData(preserve, newData, cap);
    }

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_endpoint_cap:
        எனில் (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            திரும்பு cap_endpoint_cap_set_capEPBadge(cap, newData);
        } இல்லையெனில் {
            திரும்பு cap_null_cap_new();
        }

    நிலை cap_notification_cap:
        எனில் (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            திரும்பு cap_notification_cap_set_capNtfnBadge(cap, newData);
        } இல்லையெனில் {
            திரும்பு cap_null_cap_new();
        }

    நிலை cap_cnode_cap: {
        word_t guard, guardSize;
        seL4_CNode_CapData_t w = { .சொற்கள் = { newData } };

        guardSize = seL4_CNode_CapData_get_guardSize(w);

        எனில் (guardSize + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            திரும்பு cap_null_cap_new();
        } இல்லையெனில் {
            cap_t new_cap;

            guard = seL4_CNode_CapData_get_guard(w) & MASK(guardSize);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_capCNodeGuardSize(new_cap,
                                                          guardSize);

            திரும்பு new_cap;
        }
    }

    இயல்பு:
        திரும்பு cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    எனில் (isArchCap(cap)) {
        திரும்பு Arch_maskCapRights(cap_rights, cap);
    }

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_null_cap:
    நிலை cap_domain_cap:
    நிலை cap_cnode_cap:
    நிலை cap_untyped_cap:
    நிலை cap_irq_control_cap:
    நிலை cap_irq_handler_cap:
    நிலை cap_zombie_cap:
    நிலை cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    நிலை cap_sched_context_cap:
    நிலை cap_sched_control_cap:
#endif
        திரும்பு cap;

    நிலை cap_endpoint_cap: {
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

        திரும்பு new_cap;
    }

    நிலை cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        திரும்பு new_cap;
    }
    நிலை cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        திரும்பு new_cap;
    }


    இயல்பு:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, வெறுமை *regionBase, word_t userSize, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    எனில் (t >= (object_t) se_L4_Non_Arch_Object_Type_மொத்தம்) {
        திரும்பு Arch_createObject(t, regionBase, userSize, deviceMemory);
    }

    /* Create objects. */
    தேர்வு ((api_object_t)t) {
    நிலை seL4_TCBObject: {
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
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(முனை_STATE(ksCurThread))->tcbName, TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_LENGTH);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        திரும்பு cap_thread_cap_new(TCB_REF(tcb));
    }

    நிலை seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        திரும்பு cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    நிலை seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        திரும்பு cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    நிலை seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        திரும்பு cap_cnode_cap_new(userSize, 0, 0, CTE_REF(regionBase));

    நிலை seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        திரும்பு cap_untyped_cap_new(0, !!deviceMemory, userSize, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
    நிலை seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        திரும்பு cap_sched_context_cap_new(SC_REF(regionBase), userSize);

    நிலை seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        திரும்பு cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    இயல்பு:
        fail("Invalid object type");
    }
}

வெறுமை createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t destLength,
                      வெறுமை *regionBase, word_t userSize, bool_t deviceMemory)
{
    word_t objectSize;
    வெறுமை *nextFreeArea;
    word_t i;
    word_t totalObjectSize UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    objectSize = getObjectSize(t, userSize);
    totalObjectSize = destLength << objectSize;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    nextFreeArea = regionBase;
    க்கு (i = 0; i < destLength; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (வெறுமை *)((word_t)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        /* Insert the cap into the user's cspace. */
        செருகு_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *buffer)
#இல்லையெனில்
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *buffer)
#endif
{
    எனில் (isArchCap(cap)) {
        திரும்பு Arch_decodeInvocation(invLabel, length, capIndex,
                                     slot, cap, call, buffer);
    }

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 0;
        திரும்பு EXCEPTION_SYSCALL_ERROR;

    நிலை cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 0;
        திரும்பு EXCEPTION_SYSCALL_ERROR;

    நிலை cap_endpoint_cap:
        எனில் (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        திரும்பு performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#இல்லையெனில்
        திரும்பு performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    நிலை cap_notification_cap: {
        எனில் (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    நிலை cap_reply_cap:
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு performInvocation_Reply(
                   முனை_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#இல்லையெனில்
    நிலை cap_reply_cap:
        எனில் (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    நிலை cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        எனில் (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
#endif
        திரும்பு decodeTCBInvocation(invLabel, length, cap, slot, call, buffer);

    நிலை cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        எனில் (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
#endif
        திரும்பு decodeDomainInvocation(invLabel, length, buffer);

    நிலை cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        எனில் (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
#endif
        திரும்பு decodeCNodeInvocation(invLabel, length, cap, buffer);

    நிலை cap_untyped_cap:
        திரும்பு decodeUntypedInvocation(invLabel, length, slot, cap, call, buffer);

    நிலை cap_irq_control_cap:
        திரும்பு decodeIRQControlInvocation(invLabel, length, slot, buffer);

    நிலை cap_irq_handler_cap:
        திரும்பு decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    நிலை cap_sched_control_cap:
        எனில் (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு decodeSchedControlInvocation(invLabel, cap, length, buffer);

    நிலை cap_sched_context_cap:
        எனில் (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        திரும்பு decodeSchedContextInvocation(invLabel, sc, call);
#endif
    இயல்பு:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, முனை_STATE(ksCurThread), ep);

    திரும்பு EXCEPTION_NONE;
}
#இல்லையெனில்
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, முனை_STATE(ksCurThread), ep);

    திரும்பு EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    திரும்பு EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    திரும்பு EXCEPTION_NONE;
}
#இல்லையெனில்
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(முனை_STATE(ksCurThread), thread, slot, canGrant);
    திரும்பு EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_capSizeBits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    தேர்வு (ctag) {
    நிலை cap_untyped_cap:
        திரும்பு cap_untyped_cap_get_capBlockSize(cap);

    நிலை cap_endpoint_cap:
        திரும்பு seL4_EndpointBits;

    நிலை cap_notification_cap:
        திரும்பு seL4_NotificationBits;

    நிலை cap_cnode_cap:
        திரும்பு cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    நிலை cap_thread_cap:
        திரும்பு seL4_TCBBits;

    நிலை cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        எனில் (type == ZombieType_ZombieTCB) {
            திரும்பு seL4_TCBBits;
        }
        திரும்பு ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    நிலை cap_null_cap:
        திரும்பு 0;

    நிலை cap_domain_cap:
        திரும்பு 0;

    நிலை cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        திரும்பு seL4_ReplyBits;
#இல்லையெனில்
        திரும்பு 0;
#endif

    நிலை cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    நிலை cap_sched_control_cap:
#endif
        திரும்பு 0;

    நிலை cap_irq_handler_cap:
        திரும்பு 0;

#ifdef CONFIG_KERNEL_MCS
    நிலை cap_sched_context_cap:
        திரும்பு cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    இயல்பு:
        திரும்பு cap_get_archCapSizeBits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    தேர்வு (ctag) {
    நிலை cap_untyped_cap:
        திரும்பு true;

    நிலை cap_endpoint_cap:
        திரும்பு true;

    நிலை cap_notification_cap:
        திரும்பு true;

    நிலை cap_cnode_cap:
        திரும்பு true;

    நிலை cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    நிலை cap_sched_context_cap:
#endif
        திரும்பு true;

    நிலை cap_zombie_cap:
        திரும்பு true;

    நிலை cap_domain_cap:
        திரும்பு false;

    நிலை cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        திரும்பு true;
#இல்லையெனில்
        திரும்பு false;
#endif

    நிலை cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    நிலை cap_sched_control_cap:
#endif
        திரும்பு false;

    நிலை cap_irq_handler_cap:
        திரும்பு false;

    இயல்பு:
        திரும்பு cap_get_archCapIsPhysical(cap);
    }
}

வெறுமை *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    தேர்வு (ctag) {
    நிலை cap_untyped_cap:
        திரும்பு WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    நிலை cap_endpoint_cap:
        திரும்பு EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    நிலை cap_notification_cap:
        திரும்பு NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    நிலை cap_cnode_cap:
        திரும்பு CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    நிலை cap_thread_cap:
        திரும்பு TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    நிலை cap_zombie_cap:
        திரும்பு CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    நிலை cap_domain_cap:
        திரும்பு வெற்று;

    நிலை cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        திரும்பு REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#இல்லையெனில்
        திரும்பு வெற்று;
#endif

    நிலை cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    நிலை cap_sched_control_cap:
#endif
        திரும்பு வெற்று;

    நிலை cap_irq_handler_cap:
        திரும்பு வெற்று;

#ifdef CONFIG_KERNEL_MCS
    நிலை cap_sched_context_cap:
        திரும்பு SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

    இயல்பு:
        திரும்பு cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    எனில் (isArchCap(derivedCap)) {
        திரும்பு Arch_isCapRevocable(derivedCap, srcCap);
    }
    தேர்வு (cap_get_capType(derivedCap)) {
    நிலை cap_endpoint_cap:
        திரும்பு (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    நிலை cap_notification_cap:
        திரும்பு (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    நிலை cap_irq_handler_cap:
        திரும்பு (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    நிலை cap_untyped_cap:
        திரும்பு true;

    இயல்பு:
        திரும்பு false;
    }
}
