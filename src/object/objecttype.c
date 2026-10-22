/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <arch/object/objecttype.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <object/objecttype.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <object/notification.h>
#अन्तर्भाव <object/endpoint.h>
#अन्तर्भाव <object/cnode.h>
#अन्तर्भाव <object/interrupt.h>
#ifdef CONFIG_KERNEL_MCS
#अन्तर्भाव <object/schedcontext.h>
#अन्तर्भाव <object/schedcontrol.h>
#endif
#अन्तर्भाव <object/tcb.h>
#अन्तर्भाव <object/untyped.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <machine.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <string.h>

word_t getObjectSize(word_t t, word_t userObjSize)
{
    जइ (t >= se_L4_Non_Arch_Object_Type_गणनम्) {
        प्रत्यागम Arch_getObjectSize(t);
    } अण्णहा {
        विकल्प (t) {
        पक्ष seL4_TCBObject:
            प्रत्यागम seL4_TCBBits;
        पक्ष seL4_EndpointObject:
            प्रत्यागम seL4_EndpointBits;
        पक्ष seL4_NotificationObject:
            प्रत्यागम seL4_NotificationBits;
        पक्ष seL4_CapTableObject:
            प्रत्यागम seL4_SlotBits + userObjSize;
        पक्ष seL4_UntypedObject:
            प्रत्यागम userObjSize;
#ifdef CONFIG_KERNEL_MCS
        पक्ष seL4_SchedContextObject:
            प्रत्यागम userObjSize;
        पक्ष seL4_ReplyObject:
            प्रत्यागम seL4_ReplyBits;
#endif
        स्वभाव:
            fail("Invalid object type");
            प्रत्यागम 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    जइ (isArchCap(cap)) {
        प्रत्यागम Arch_deriveCap(slot, cap);
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        भङ्ग;

    पक्ष cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        भङ्ग;

    पक्ष cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        जइ (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } अण्णहा {
            ret.cap = cap;
        }
        भङ्ग;

#ifndef CONFIG_KERNEL_MCS
    पक्ष cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        भङ्ग;
#endif
    स्वभाव:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    प्रत्यागम ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    जइ (isArchCap(cap)) {
        प्रत्यागम Arch_finaliseCap(cap, final);
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_endpoint_cap:
        जइ (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        प्रत्यागम fc_ret;

    पक्ष cap_notification_cap:
        जइ (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ifdef CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#endif
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        प्रत्यागम fc_ret;

    पक्ष cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        जइ (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            जइ (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                विकल्प (thread_state_get_tsType(tcb->tcbState)) {
                पक्ष ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    भङ्ग;
                पक्ष ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    भङ्ग;
                स्वभाव:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        प्रत्यागम fc_ret;
#endif
    पक्ष cap_null_cap:
    पक्ष cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        प्रत्यागम fc_ret;
    }

    जइ (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_cnode_cap: {
        जइ (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            प्रत्यागम fc_ret;
        }
        भङ्ग;
    }

    पक्ष cap_thread_cap: {
        जइ (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ifdef CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            जइ (sc) {
                schedContext_unbindTCB(sc);
                जइ (sc->scYieldFrom) {
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
            प्रत्यागम fc_ret;
        }
        भङ्ग;
    }

#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
        जइ (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            जइ (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            प्रत्यागम fc_ret;
        }
        भङ्ग;
#endif

    पक्ष cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        प्रत्यागम fc_ret;

    पक्ष cap_irq_handler_cap:
        जइ (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            प्रत्यागम fc_ret;
        }
        भङ्ग;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    प्रत्यागम fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_endpoint_cap:
        प्रत्यागम cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    स्वभाव:
        प्रत्यागम false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {
    पक्ष cap_untyped_cap:
        जइ (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, a_शिखरम्, b_शिखरम्;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_शिखरम् = aBase + MASK(cap_untyped_cap_get_capBlockSize(cap_a));
            b_शिखरम् = bBase + MASK(cap_get_capSizeBits(cap_b));

            प्रत्यागम (aBase <= bBase) && (b_शिखरम् <= a_शिखरम्) && (bBase <= b_शिखरम्);
        }
        भङ्ग;

    पक्ष cap_endpoint_cap:
        जइ (cap_get_capType(cap_b) == cap_endpoint_cap) {
            प्रत्यागम cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_notification_cap:
        जइ (cap_get_capType(cap_b) == cap_notification_cap) {
            प्रत्यागम cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_cnode_cap:
        जइ (cap_get_capType(cap_b) == cap_cnode_cap) {
            प्रत्यागम (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        भङ्ग;

    पक्ष cap_thread_cap:
        जइ (cap_get_capType(cap_b) == cap_thread_cap) {
            प्रत्यागम cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_reply_cap:
        जइ (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            प्रत्यागम cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#अण्णहा
            प्रत्यागम cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
        }
        भङ्ग;

    पक्ष cap_domain_cap:
        जइ (cap_get_capType(cap_b) == cap_domain_cap) {
            प्रत्यागम true;
        }
        भङ्ग;

    पक्ष cap_irq_control_cap:
        जइ (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            प्रत्यागम true;
        }
        भङ्ग;

    पक्ष cap_irq_handler_cap:
        जइ (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            प्रत्यागम (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        भङ्ग;

#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
        जइ (cap_get_capType(cap_b) == cap_sched_context_cap) {
            प्रत्यागम (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        भङ्ग;
    पक्ष cap_sched_control_cap:
        जइ (cap_get_capType(cap_b) == cap_sched_control_cap) {
            प्रत्यागम true;
        }
        भङ्ग;
#endif
    स्वभाव:
        जइ (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            प्रत्यागम Arch_sameRegionAs(cap_a, cap_b);
        }
        भङ्ग;
    }

    प्रत्यागम false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    जइ (cap_get_capType(cap_a) == cap_untyped_cap) {
        प्रत्यागम false;
    }
    जइ (cap_get_capType(cap_a) == cap_irq_control_cap) {
        प्रत्यागम false;
    }
    जइ (isArchCap(cap_a) && isArchCap(cap_b)) {
        प्रत्यागम Arch_sameObjectAs(cap_a, cap_b);
    }
    प्रत्यागम sameRegionAs(cap_a, cap_b);
}

cap_t CONST updateCapData(bool_t preserve, word_t newData, cap_t cap)
{
    जइ (isArchCap(cap)) {
        प्रत्यागम Arch_updateCapData(preserve, newData, cap);
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_endpoint_cap:
        जइ (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            प्रत्यागम cap_endpoint_cap_set_capEPBadge(cap, newData);
        } अण्णहा {
            प्रत्यागम cap_null_cap_new();
        }

    पक्ष cap_notification_cap:
        जइ (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            प्रत्यागम cap_notification_cap_set_capNtfnBadge(cap, newData);
        } अण्णहा {
            प्रत्यागम cap_null_cap_new();
        }

    पक्ष cap_cnode_cap: {
        word_t guard, guardSize;
        seL4_CNode_CapData_t w = { .शब्दाः = { newData } };

        guardSize = seL4_CNode_CapData_get_guardSize(w);

        जइ (guardSize + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            प्रत्यागम cap_null_cap_new();
        } अण्णहा {
            cap_t new_cap;

            guard = seL4_CNode_CapData_get_guard(w) & MASK(guardSize);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_capCNodeGuardSize(new_cap,
                                                          guardSize);

            प्रत्यागम new_cap;
        }
    }

    स्वभाव:
        प्रत्यागम cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    जइ (isArchCap(cap)) {
        प्रत्यागम Arch_maskCapRights(cap_rights, cap);
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_null_cap:
    पक्ष cap_domain_cap:
    पक्ष cap_cnode_cap:
    पक्ष cap_untyped_cap:
    पक्ष cap_irq_control_cap:
    पक्ष cap_irq_handler_cap:
    पक्ष cap_zombie_cap:
    पक्ष cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
    पक्ष cap_sched_control_cap:
#endif
        प्रत्यागम cap;

    पक्ष cap_endpoint_cap: {
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

        प्रत्यागम new_cap;
    }

    पक्ष cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        प्रत्यागम new_cap;
    }
    पक्ष cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        प्रत्यागम new_cap;
    }


    स्वभाव:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    जइ (t >= (object_t) se_L4_Non_Arch_Object_Type_गणनम्) {
        प्रत्यागम Arch_createObject(t, regionBase, userSize, deviceMemory);
    }

    /* Create objects. */
    विकल्प ((api_object_t)t) {
    पक्ष seL4_TCBObject: {
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
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(ग्रन्थिः_STATE(ksCurThread))->tcbName, TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_LENGTH);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        प्रत्यागम cap_thread_cap_new(TCB_REF(tcb));
    }

    पक्ष seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        प्रत्यागम cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    पक्ष seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        प्रत्यागम cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    पक्ष seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        प्रत्यागम cap_cnode_cap_new(userSize, 0, 0, CTE_REF(regionBase));

    पक्ष seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        प्रत्यागम cap_untyped_cap_new(0, !!deviceMemory, userSize, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
    पक्ष seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        प्रत्यागम cap_sched_context_cap_new(SC_REF(regionBase), userSize);

    पक्ष seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        प्रत्यागम cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    स्वभाव:
        fail("Invalid object type");
    }
}

शून्य createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t destLength,
                      शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
    word_t objectSize;
    शून्य *nextFreeArea;
    word_t i;
    word_t totalObjectSize UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    objectSize = getObjectSize(t, userSize);
    totalObjectSize = destLength << objectSize;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    nextFreeArea = regionBase;
    प्रति (i = 0; i < destLength; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (शून्य *)((word_t)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        /* Insert the cap into the user's cspace. */
        निवेशय_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *buffer)
#अण्णहा
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *buffer)
#endif
{
    जइ (isArchCap(cap)) {
        प्रत्यागम Arch_decodeInvocation(invLabel, length, capIndex,
                                     slot, cap, call, buffer);
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;

    पक्ष cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;

    पक्ष cap_endpoint_cap:
        जइ (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        प्रत्यागम performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#अण्णहा
        प्रत्यागम performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    पक्ष cap_notification_cap: {
        जइ (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_reply_cap:
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performInvocation_Reply(
                   ग्रन्थिः_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#अण्णहा
    पक्ष cap_reply_cap:
        जइ (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    पक्ष cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        जइ (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#endif
        प्रत्यागम decodeTCBInvocation(invLabel, length, cap, slot, call, buffer);

    पक्ष cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        जइ (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#endif
        प्रत्यागम decodeDomainInvocation(invLabel, length, buffer);

    पक्ष cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        जइ (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#endif
        प्रत्यागम decodeCNodeInvocation(invLabel, length, cap, buffer);

    पक्ष cap_untyped_cap:
        प्रत्यागम decodeUntypedInvocation(invLabel, length, slot, cap, call, buffer);

    पक्ष cap_irq_control_cap:
        प्रत्यागम decodeIRQControlInvocation(invLabel, length, slot, buffer);

    पक्ष cap_irq_handler_cap:
        प्रत्यागम decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_sched_control_cap:
        जइ (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम decodeSchedControlInvocation(invLabel, cap, length, buffer);

    पक्ष cap_sched_context_cap:
        जइ (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        प्रत्यागम decodeSchedContextInvocation(invLabel, sc, call);
#endif
    स्वभाव:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, ग्रन्थिः_STATE(ksCurThread), ep);

    प्रत्यागम EXCEPTION_NONE;
}
#अण्णहा
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, ग्रन्थिः_STATE(ksCurThread), ep);

    प्रत्यागम EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    प्रत्यागम EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    प्रत्यागम EXCEPTION_NONE;
}
#अण्णहा
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(ग्रन्थिः_STATE(ksCurThread), thread, slot, canGrant);
    प्रत्यागम EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_capSizeBits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    विकल्प (ctag) {
    पक्ष cap_untyped_cap:
        प्रत्यागम cap_untyped_cap_get_capBlockSize(cap);

    पक्ष cap_endpoint_cap:
        प्रत्यागम seL4_EndpointBits;

    पक्ष cap_notification_cap:
        प्रत्यागम seL4_NotificationBits;

    पक्ष cap_cnode_cap:
        प्रत्यागम cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    पक्ष cap_thread_cap:
        प्रत्यागम seL4_TCBBits;

    पक्ष cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        जइ (type == ZombieType_ZombieTCB) {
            प्रत्यागम seL4_TCBBits;
        }
        प्रत्यागम ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    पक्ष cap_null_cap:
        प्रत्यागम 0;

    पक्ष cap_domain_cap:
        प्रत्यागम 0;

    पक्ष cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        प्रत्यागम seL4_ReplyBits;
#अण्णहा
        प्रत्यागम 0;
#endif

    पक्ष cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_sched_control_cap:
#endif
        प्रत्यागम 0;

    पक्ष cap_irq_handler_cap:
        प्रत्यागम 0;

#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
        प्रत्यागम cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    स्वभाव:
        प्रत्यागम cap_get_archCapSizeBits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    विकल्प (ctag) {
    पक्ष cap_untyped_cap:
        प्रत्यागम true;

    पक्ष cap_endpoint_cap:
        प्रत्यागम true;

    पक्ष cap_notification_cap:
        प्रत्यागम true;

    पक्ष cap_cnode_cap:
        प्रत्यागम true;

    पक्ष cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
#endif
        प्रत्यागम true;

    पक्ष cap_zombie_cap:
        प्रत्यागम true;

    पक्ष cap_domain_cap:
        प्रत्यागम false;

    पक्ष cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        प्रत्यागम true;
#अण्णहा
        प्रत्यागम false;
#endif

    पक्ष cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_sched_control_cap:
#endif
        प्रत्यागम false;

    पक्ष cap_irq_handler_cap:
        प्रत्यागम false;

    स्वभाव:
        प्रत्यागम cap_get_archCapIsPhysical(cap);
    }
}

शून्य *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    विकल्प (ctag) {
    पक्ष cap_untyped_cap:
        प्रत्यागम WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    पक्ष cap_endpoint_cap:
        प्रत्यागम EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    पक्ष cap_notification_cap:
        प्रत्यागम NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    पक्ष cap_cnode_cap:
        प्रत्यागम CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    पक्ष cap_thread_cap:
        प्रत्यागम TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    पक्ष cap_zombie_cap:
        प्रत्यागम CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    पक्ष cap_domain_cap:
        प्रत्यागम रिक्त;

    पक्ष cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        प्रत्यागम REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#अण्णहा
        प्रत्यागम रिक्त;
#endif

    पक्ष cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_sched_control_cap:
#endif
        प्रत्यागम रिक्त;

    पक्ष cap_irq_handler_cap:
        प्रत्यागम रिक्त;

#ifdef CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
        प्रत्यागम SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

    स्वभाव:
        प्रत्यागम cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    जइ (isArchCap(derivedCap)) {
        प्रत्यागम Arch_isCapRevocable(derivedCap, srcCap);
    }
    विकल्प (cap_get_capType(derivedCap)) {
    पक्ष cap_endpoint_cap:
        प्रत्यागम (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    पक्ष cap_notification_cap:
        प्रत्यागम (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    पक्ष cap_irq_handler_cap:
        प्रत्यागम (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    पक्ष cap_untyped_cap:
        प्रत्यागम true;

    स्वभाव:
        प्रत्यागम false;
    }
}
