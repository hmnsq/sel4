/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <arch/object/objecttype.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <object/objecttype.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <object/notification.h>
#અન્તર્ભાવ <object/endpoint.h>
#અન્તર્ભાવ <object/cnode.h>
#અન્તર્ભાવ <object/interrupt.h>
#ifdef CONFIG_KERNEL_MCS
#અન્તર્ભાવ <object/schedcontext.h>
#અન્તર્ભાવ <object/schedcontrol.h>
#endif
#અન્તર્ભાવ <object/tcb.h>
#અન્તર્ભાવ <object/untyped.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <machine.h>
#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <string.h>

word_t getObjectSize(word_t t, word_t userObjSize)
{
    જો (t >= se_L4_Non_Arch_Object_Type_ગણતરી) {
        પાછા Arch_getObjectSize(t);
    } નહીંતર {
        પસંદગી (t) {
        સ્થિતિ seL4_TCBObject:
            પાછા seL4_TCBBits;
        સ્થિતિ seL4_EndpointObject:
            પાછા seL4_EndpointBits;
        સ્થિતિ seL4_NotificationObject:
            પાછા seL4_NotificationBits;
        સ્થિતિ seL4_CapTableObject:
            પાછા seL4_SlotBits + userObjSize;
        સ્થિતિ seL4_UntypedObject:
            પાછા userObjSize;
#ifdef CONFIG_KERNEL_MCS
        સ્થિતિ seL4_SchedContextObject:
            પાછા userObjSize;
        સ્થિતિ seL4_ReplyObject:
            પાછા seL4_ReplyBits;
#endif
        મૂળભૂત:
            fail("Invalid object type");
            પાછા 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    જો (isArchCap(cap)) {
        પાછા Arch_deriveCap(slot, cap);
    }

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        તોડો;

    સ્થિતિ cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        તોડો;

    સ્થિતિ cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        જો (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } નહીંતર {
            ret.cap = cap;
        }
        તોડો;

#ifndef CONFIG_KERNEL_MCS
    સ્થિતિ cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        તોડો;
#endif
    મૂળભૂત:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    પાછા ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    જો (isArchCap(cap)) {
        પાછા Arch_finaliseCap(cap, final);
    }

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_endpoint_cap:
        જો (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        પાછા fc_ret;

    સ્થિતિ cap_notification_cap:
        જો (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ifdef CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#endif
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        પાછા fc_ret;

    સ્થિતિ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        જો (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            જો (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                પસંદગી (thread_state_get_tsType(tcb->tcbState)) {
                સ્થિતિ ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    તોડો;
                સ્થિતિ ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    તોડો;
                મૂળભૂત:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        પાછા fc_ret;
#endif
    સ્થિતિ cap_null_cap:
    સ્થિતિ cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        પાછા fc_ret;
    }

    જો (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_cnode_cap: {
        જો (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            પાછા fc_ret;
        }
        તોડો;
    }

    સ્થિતિ cap_thread_cap: {
        જો (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ifdef CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            જો (sc) {
                schedContext_unbindTCB(sc);
                જો (sc->scYieldFrom) {
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
            પાછા fc_ret;
        }
        તોડો;
    }

#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_sched_context_cap:
        જો (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            જો (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            પાછા fc_ret;
        }
        તોડો;
#endif

    સ્થિતિ cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        પાછા fc_ret;

    સ્થિતિ cap_irq_handler_cap:
        જો (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            પાછા fc_ret;
        }
        તોડો;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    પાછા fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_endpoint_cap:
        પાછા cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    મૂળભૂત:
        પાછા false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    પસંદગી (cap_get_capType(cap_a)) {
    સ્થિતિ cap_untyped_cap:
        જો (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, a_ટોચ, b_ટોચ;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_ટોચ = aBase + MASK(cap_untyped_cap_get_capBlockSize(cap_a));
            b_ટોચ = bBase + MASK(cap_get_capSizeBits(cap_b));

            પાછા (aBase <= bBase) && (b_ટોચ <= a_ટોચ) && (bBase <= b_ટોચ);
        }
        તોડો;

    સ્થિતિ cap_endpoint_cap:
        જો (cap_get_capType(cap_b) == cap_endpoint_cap) {
            પાછા cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_notification_cap:
        જો (cap_get_capType(cap_b) == cap_notification_cap) {
            પાછા cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_cnode_cap:
        જો (cap_get_capType(cap_b) == cap_cnode_cap) {
            પાછા (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        તોડો;

    સ્થિતિ cap_thread_cap:
        જો (cap_get_capType(cap_b) == cap_thread_cap) {
            પાછા cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_reply_cap:
        જો (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            પાછા cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#નહીંતર
            પાછા cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
        }
        તોડો;

    સ્થિતિ cap_domain_cap:
        જો (cap_get_capType(cap_b) == cap_domain_cap) {
            પાછા true;
        }
        તોડો;

    સ્થિતિ cap_irq_control_cap:
        જો (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            પાછા true;
        }
        તોડો;

    સ્થિતિ cap_irq_handler_cap:
        જો (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            પાછા (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        તોડો;

#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_sched_context_cap:
        જો (cap_get_capType(cap_b) == cap_sched_context_cap) {
            પાછા (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        તોડો;
    સ્થિતિ cap_sched_control_cap:
        જો (cap_get_capType(cap_b) == cap_sched_control_cap) {
            પાછા true;
        }
        તોડો;
#endif
    મૂળભૂત:
        જો (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            પાછા Arch_sameRegionAs(cap_a, cap_b);
        }
        તોડો;
    }

    પાછા false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    જો (cap_get_capType(cap_a) == cap_untyped_cap) {
        પાછા false;
    }
    જો (cap_get_capType(cap_a) == cap_irq_control_cap) {
        પાછા false;
    }
    જો (isArchCap(cap_a) && isArchCap(cap_b)) {
        પાછા Arch_sameObjectAs(cap_a, cap_b);
    }
    પાછા sameRegionAs(cap_a, cap_b);
}

cap_t CONST updateCapData(bool_t preserve, word_t newData, cap_t cap)
{
    જો (isArchCap(cap)) {
        પાછા Arch_updateCapData(preserve, newData, cap);
    }

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_endpoint_cap:
        જો (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            પાછા cap_endpoint_cap_set_capEPBadge(cap, newData);
        } નહીંતર {
            પાછા cap_null_cap_new();
        }

    સ્થિતિ cap_notification_cap:
        જો (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            પાછા cap_notification_cap_set_capNtfnBadge(cap, newData);
        } નહીંતર {
            પાછા cap_null_cap_new();
        }

    સ્થિતિ cap_cnode_cap: {
        word_t guard, guardSize;
        seL4_CNode_CapData_t w = { .શબ્દો = { newData } };

        guardSize = seL4_CNode_CapData_get_guardSize(w);

        જો (guardSize + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            પાછા cap_null_cap_new();
        } નહીંતર {
            cap_t new_cap;

            guard = seL4_CNode_CapData_get_guard(w) & MASK(guardSize);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_capCNodeGuardSize(new_cap,
                                                          guardSize);

            પાછા new_cap;
        }
    }

    મૂળભૂત:
        પાછા cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    જો (isArchCap(cap)) {
        પાછા Arch_maskCapRights(cap_rights, cap);
    }

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_null_cap:
    સ્થિતિ cap_domain_cap:
    સ્થિતિ cap_cnode_cap:
    સ્થિતિ cap_untyped_cap:
    સ્થિતિ cap_irq_control_cap:
    સ્થિતિ cap_irq_handler_cap:
    સ્થિતિ cap_zombie_cap:
    સ્થિતિ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_sched_context_cap:
    સ્થિતિ cap_sched_control_cap:
#endif
        પાછા cap;

    સ્થિતિ cap_endpoint_cap: {
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

        પાછા new_cap;
    }

    સ્થિતિ cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        પાછા new_cap;
    }
    સ્થિતિ cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        પાછા new_cap;
    }


    મૂળભૂત:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, શૂન્ય *regionBase, word_t userSize, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    જો (t >= (object_t) se_L4_Non_Arch_Object_Type_ગણતરી) {
        પાછા Arch_createObject(t, regionBase, userSize, deviceMemory);
    }

    /* Create objects. */
    પસંદગી ((api_object_t)t) {
    સ્થિતિ seL4_TCBObject: {
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
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(ગાંઠ_STATE(ksCurThread))->tcbName, TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_LENGTH);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        પાછા cap_thread_cap_new(TCB_REF(tcb));
    }

    સ્થિતિ seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        પાછા cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    સ્થિતિ seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        પાછા cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    સ્થિતિ seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        પાછા cap_cnode_cap_new(userSize, 0, 0, CTE_REF(regionBase));

    સ્થિતિ seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        પાછા cap_untyped_cap_new(0, !!deviceMemory, userSize, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        પાછા cap_sched_context_cap_new(SC_REF(regionBase), userSize);

    સ્થિતિ seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        પાછા cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    મૂળભૂત:
        fail("Invalid object type");
    }
}

શૂન્ય createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t destLength,
                      શૂન્ય *regionBase, word_t userSize, bool_t deviceMemory)
{
    word_t objectSize;
    શૂન્ય *nextFreeArea;
    word_t i;
    word_t totalObjectSize UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    objectSize = getObjectSize(t, userSize);
    totalObjectSize = destLength << objectSize;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    nextFreeArea = regionBase;
    માટે (i = 0; i < destLength; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (શૂન્ય *)((word_t)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        /* Insert the cap into the user's cspace. */
        ઉમેરો_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *buffer)
#નહીંતર
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *buffer)
#endif
{
    જો (isArchCap(cap)) {
        પાછા Arch_decodeInvocation(invLabel, length, capIndex,
                                     slot, cap, call, buffer);
    }

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;

    સ્થિતિ cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;

    સ્થિતિ cap_endpoint_cap:
        જો (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        પાછા performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#નહીંતર
        પાછા performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    સ્થિતિ cap_notification_cap: {
        જો (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_reply_cap:
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performInvocation_Reply(
                   ગાંઠ_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#નહીંતર
    સ્થિતિ cap_reply_cap:
        જો (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    સ્થિતિ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        જો (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
#endif
        પાછા decodeTCBInvocation(invLabel, length, cap, slot, call, buffer);

    સ્થિતિ cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        જો (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
#endif
        પાછા decodeDomainInvocation(invLabel, length, buffer);

    સ્થિતિ cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        જો (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
#endif
        પાછા decodeCNodeInvocation(invLabel, length, cap, buffer);

    સ્થિતિ cap_untyped_cap:
        પાછા decodeUntypedInvocation(invLabel, length, slot, cap, call, buffer);

    સ્થિતિ cap_irq_control_cap:
        પાછા decodeIRQControlInvocation(invLabel, length, slot, buffer);

    સ્થિતિ cap_irq_handler_cap:
        પાછા decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_sched_control_cap:
        જો (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        પાછા decodeSchedControlInvocation(invLabel, cap, length, buffer);

    સ્થિતિ cap_sched_context_cap:
        જો (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        પાછા decodeSchedContextInvocation(invLabel, sc, call);
#endif
    મૂળભૂત:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, ગાંઠ_STATE(ksCurThread), ep);

    પાછા EXCEPTION_NONE;
}
#નહીંતર
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, ગાંઠ_STATE(ksCurThread), ep);

    પાછા EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    પાછા EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    પાછા EXCEPTION_NONE;
}
#નહીંતર
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(ગાંઠ_STATE(ksCurThread), thread, slot, canGrant);
    પાછા EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_capSizeBits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    પસંદગી (ctag) {
    સ્થિતિ cap_untyped_cap:
        પાછા cap_untyped_cap_get_capBlockSize(cap);

    સ્થિતિ cap_endpoint_cap:
        પાછા seL4_EndpointBits;

    સ્થિતિ cap_notification_cap:
        પાછા seL4_NotificationBits;

    સ્થિતિ cap_cnode_cap:
        પાછા cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    સ્થિતિ cap_thread_cap:
        પાછા seL4_TCBBits;

    સ્થિતિ cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        જો (type == ZombieType_ZombieTCB) {
            પાછા seL4_TCBBits;
        }
        પાછા ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    સ્થિતિ cap_null_cap:
        પાછા 0;

    સ્થિતિ cap_domain_cap:
        પાછા 0;

    સ્થિતિ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        પાછા seL4_ReplyBits;
#નહીંતર
        પાછા 0;
#endif

    સ્થિતિ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_sched_control_cap:
#endif
        પાછા 0;

    સ્થિતિ cap_irq_handler_cap:
        પાછા 0;

#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_sched_context_cap:
        પાછા cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    મૂળભૂત:
        પાછા cap_get_archCapSizeBits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    પસંદગી (ctag) {
    સ્થિતિ cap_untyped_cap:
        પાછા true;

    સ્થિતિ cap_endpoint_cap:
        પાછા true;

    સ્થિતિ cap_notification_cap:
        પાછા true;

    સ્થિતિ cap_cnode_cap:
        પાછા true;

    સ્થિતિ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_sched_context_cap:
#endif
        પાછા true;

    સ્થિતિ cap_zombie_cap:
        પાછા true;

    સ્થિતિ cap_domain_cap:
        પાછા false;

    સ્થિતિ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        પાછા true;
#નહીંતર
        પાછા false;
#endif

    સ્થિતિ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_sched_control_cap:
#endif
        પાછા false;

    સ્થિતિ cap_irq_handler_cap:
        પાછા false;

    મૂળભૂત:
        પાછા cap_get_archCapIsPhysical(cap);
    }
}

શૂન્ય *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    પસંદગી (ctag) {
    સ્થિતિ cap_untyped_cap:
        પાછા WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    સ્થિતિ cap_endpoint_cap:
        પાછા EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    સ્થિતિ cap_notification_cap:
        પાછા NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    સ્થિતિ cap_cnode_cap:
        પાછા CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    સ્થિતિ cap_thread_cap:
        પાછા TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    સ્થિતિ cap_zombie_cap:
        પાછા CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    સ્થિતિ cap_domain_cap:
        પાછા ખાલી;

    સ્થિતિ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        પાછા REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#નહીંતર
        પાછા ખાલી;
#endif

    સ્થિતિ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_sched_control_cap:
#endif
        પાછા ખાલી;

    સ્થિતિ cap_irq_handler_cap:
        પાછા ખાલી;

#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_sched_context_cap:
        પાછા SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

    મૂળભૂત:
        પાછા cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    જો (isArchCap(derivedCap)) {
        પાછા Arch_isCapRevocable(derivedCap, srcCap);
    }
    પસંદગી (cap_get_capType(derivedCap)) {
    સ્થિતિ cap_endpoint_cap:
        પાછા (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    સ્થિતિ cap_notification_cap:
        પાછા (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    સ્થિતિ cap_irq_handler_cap:
        પાછા (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    સ્થિતિ cap_untyped_cap:
        પાછા true;

    મૂળભૂત:
        પાછા false;
    }
}
