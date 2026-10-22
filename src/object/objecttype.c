/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <assert.h>
#समावेश <config.h>
#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <api/syscall.h>
#समावेश <arch/object/objecttype.h>
#समावेश <machine/io.h>
#समावेश <object/objecttype.h>
#समावेश <object/structures.h>
#समावेश <object/notification.h>
#समावेश <object/endpoint.h>
#समावेश <object/cnode.h>
#समावेश <object/interrupt.h>
#अगर_घोषित CONFIG_KERNEL_MCS
#समावेश <object/schedcontext.h>
#समावेश <object/schedcontrol.h>
#पूर्ण_अगर
#समावेश <object/tcb.h>
#समावेश <object/untyped.h>
#समावेश <model/statedata.h>
#समावेश <kernel/thread.h>
#समावेश <kernel/vspace.h>
#समावेश <machine.h>
#समावेश <util.h>
#समावेश <string.h>

word_t get_Object_आकार(word_t t, word_t user_Obj_आकार)
{
    अगर (t >= se_L4_Non_Arch_Object_Type_गिनती) {
        वापस Arch_get_Object_आकार(t);
    } अन्यथा {
        चयन (t) {
        हाल seL4_TCBObject:
            वापस seL4_TCBBits;
        हाल seL4_EndpointObject:
            वापस seL4_EndpointBits;
        हाल seL4_NotificationObject:
            वापस seL4_NotificationBits;
        हाल seL4_CapTableObject:
            वापस seL4_SlotBits + user_Obj_आकार;
        हाल seL4_UntypedObject:
            वापस user_Obj_आकार;
#अगर_घोषित CONFIG_KERNEL_MCS
        हाल seL4_SchedContextObject:
            वापस user_Obj_आकार;
        हाल seL4_ReplyObject:
            वापस seL4_ReplyBits;
#पूर्ण_अगर
        शेष:
            fail("Invalid object type");
            वापस 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    अगर (isArchCap(cap)) {
        वापस Arch_deriveCap(slot, cap);
    }

    चयन (cap_get_capType(cap)) {
    हाल cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        अवरोध;

    हाल cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        अवरोध;

    हाल cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        अगर (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } अन्यथा {
            ret.cap = cap;
        }
        अवरोध;

#अगर_अघोषित CONFIG_KERNEL_MCS
    हाल cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        अवरोध;
#पूर्ण_अगर
    शेष:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    वापस ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    अगर (isArchCap(cap)) {
        वापस Arch_finaliseCap(cap, final);
    }

    चयन (cap_get_capType(cap)) {
    हाल cap_endpoint_cap:
        अगर (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        वापस fc_ret;

    हाल cap_notification_cap:
        अगर (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#अगर_घोषित CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#पूर्ण_अगर
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        वापस fc_ret;

    हाल cap_reply_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
        अगर (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            अगर (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                चयन (thread_state_get_tsType(tcb->tcbState)) {
                हाल ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    अवरोध;
                हाल ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    अवरोध;
                शेष:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        वापस fc_ret;
#पूर्ण_अगर
    हाल cap_null_cap:
    हाल cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        वापस fc_ret;
    }

    अगर (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    चयन (cap_get_capType(cap)) {
    हाल cap_cnode_cap: {
        अगर (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            वापस fc_ret;
        }
        अवरोध;
    }

    हाल cap_thread_cap: {
        अगर (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#अगर_घोषित CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            अगर (sc) {
                schedContext_unbindTCB(sc);
                अगर (sc->scYieldFrom) {
                    schedContext_completeYieldTo(sc->scYieldFrom);
                }
            }
#पूर्ण_अगर
            suspend(tcb);
#अगर_घोषित CONFIG_DEBUG_BUILD
            tcbDebugRemove(tcb);
#पूर्ण_अगर
            Arch_prepareThreadDelete(tcb);
            fc_ret.remainder =
                Zombie_new(
                    tcbArchCNodeEntries,
                    ZombieType_ZombieTCB,
                    CTE_REF(cte_ptr)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            वापस fc_ret;
        }
        अवरोध;
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_sched_context_cap:
        अगर (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            अगर (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            वापस fc_ret;
        }
        अवरोध;
#पूर्ण_अगर

    हाल cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        वापस fc_ret;

    हाल cap_irq_handler_cap:
        अगर (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            वापस fc_ret;
        }
        अवरोध;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    वापस fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_endpoint_cap:
        वापस cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    शेष:
        वापस false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    चयन (cap_get_capType(cap_a)) {
    हाल cap_untyped_cap:
        अगर (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, a_शीर्ष, b_शीर्ष;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_शीर्ष = aBase + MASK(cap_untyped_cap_get_cap_Block_आकार(cap_a));
            b_शीर्ष = bBase + MASK(cap_get_cap_आकार_Bits(cap_b));

            वापस (aBase <= bBase) && (b_शीर्ष <= a_शीर्ष) && (bBase <= b_शीर्ष);
        }
        अवरोध;

    हाल cap_endpoint_cap:
        अगर (cap_get_capType(cap_b) == cap_endpoint_cap) {
            वापस cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        अवरोध;

    हाल cap_notification_cap:
        अगर (cap_get_capType(cap_b) == cap_notification_cap) {
            वापस cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        अवरोध;

    हाल cap_cnode_cap:
        अगर (cap_get_capType(cap_b) == cap_cnode_cap) {
            वापस (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        अवरोध;

    हाल cap_thread_cap:
        अगर (cap_get_capType(cap_b) == cap_thread_cap) {
            वापस cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        अवरोध;

    हाल cap_reply_cap:
        अगर (cap_get_capType(cap_b) == cap_reply_cap) {
#अगर_घोषित CONFIG_KERNEL_MCS
            वापस cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#अन्यथा
            वापस cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#पूर्ण_अगर
        }
        अवरोध;

    हाल cap_domain_cap:
        अगर (cap_get_capType(cap_b) == cap_domain_cap) {
            वापस true;
        }
        अवरोध;

    हाल cap_irq_control_cap:
        अगर (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            वापस true;
        }
        अवरोध;

    हाल cap_irq_handler_cap:
        अगर (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            वापस (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        अवरोध;

#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_sched_context_cap:
        अगर (cap_get_capType(cap_b) == cap_sched_context_cap) {
            वापस (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        अवरोध;
    हाल cap_sched_control_cap:
        अगर (cap_get_capType(cap_b) == cap_sched_control_cap) {
            वापस true;
        }
        अवरोध;
#पूर्ण_अगर
    शेष:
        अगर (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            वापस Arch_sameRegionAs(cap_a, cap_b);
        }
        अवरोध;
    }

    वापस false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    अगर (cap_get_capType(cap_a) == cap_untyped_cap) {
        वापस false;
    }
    अगर (cap_get_capType(cap_a) == cap_irq_control_cap) {
        वापस false;
    }
    अगर (isArchCap(cap_a) && isArchCap(cap_b)) {
        वापस Arch_sameObjectAs(cap_a, cap_b);
    }
    वापस sameRegionAs(cap_a, cap_b);
}

cap_t CONST update_Cap_तथ्य(bool_t preserve, word_t new_तथ्य, cap_t cap)
{
    अगर (isArchCap(cap)) {
        वापस Arch_update_Cap_तथ्य(preserve, new_तथ्य, cap);
    }

    चयन (cap_get_capType(cap)) {
    हाल cap_endpoint_cap:
        अगर (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            वापस cap_endpoint_cap_set_capEPBadge(cap, new_तथ्य);
        } अन्यथा {
            वापस cap_null_cap_new();
        }

    हाल cap_notification_cap:
        अगर (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            वापस cap_notification_cap_set_capNtfnBadge(cap, new_तथ्य);
        } अन्यथा {
            वापस cap_null_cap_new();
        }

    हाल cap_cnode_cap: {
        word_t guard, guard_आकार;
        se_L4_CNode_Cap_तथ्य_t w = { .शब्दगण = { new_तथ्य } };

        guard_आकार = se_L4_CNode_Cap_तथ्य_get_guard_आकार(w);

        अगर (guard_आकार + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            वापस cap_null_cap_new();
        } अन्यथा {
            cap_t new_cap;

            guard = se_L4_CNode_Cap_तथ्य_get_guard(w) & MASK(guard_आकार);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_cap_CNode_Guard_आकार(new_cap,
                                                          guard_आकार);

            वापस new_cap;
        }
    }

    शेष:
        वापस cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    अगर (isArchCap(cap)) {
        वापस Arch_maskCapRights(cap_rights, cap);
    }

    चयन (cap_get_capType(cap)) {
    हाल cap_null_cap:
    हाल cap_domain_cap:
    हाल cap_cnode_cap:
    हाल cap_untyped_cap:
    हाल cap_irq_control_cap:
    हाल cap_irq_handler_cap:
    हाल cap_zombie_cap:
    हाल cap_thread_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_sched_context_cap:
    हाल cap_sched_control_cap:
#पूर्ण_अगर
        वापस cap;

    हाल cap_endpoint_cap: {
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

        वापस new_cap;
    }

    हाल cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        वापस new_cap;
    }
    हाल cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        वापस new_cap;
    }


    शेष:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, व्योम *regionBase, word_t user_आकार, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    अगर (t >= (object_t) se_L4_Non_Arch_Object_Type_गिनती) {
        वापस Arch_createObject(t, regionBase, user_आकार, deviceMemory);
    }

    /* Create objects. */
    चयन ((api_object_t)t) {
    हाल seL4_TCBObject: {
        tcb_t *tcb;
        tcb = TCB_PTR((word_t)regionBase + TCB_OFFSET);
        /** AUXUPD: "(True, ptr_retyps 1
          (Ptr ((ptr_val \<acute>tcb) - ctcb_offset) :: (cte_C[5]) ptr)
            o (ptr_retyp \<acute>tcb))" */

        /* Setup non-zero parts of the TCB. */

        Arch_initContext(&tcb->tcbArch.tcbContext);
#अगर_अघोषित CONFIG_KERNEL_MCS
        tcb->tcbTimeSlice = CONFIG_TIME_SLICE;
#पूर्ण_अगर
        tcb->tcbDomain = ksCurDomain;
#अगर_अघोषित CONFIG_KERNEL_MCS
        /* Initialize the new TCB to the current core */
        SMP_COND_STATEMENT(tcb->tcbAffinity = getCurrentCPUIndex());
#पूर्ण_अगर
#अगर_घोषित CONFIG_DEBUG_BUILD
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_लंबाई);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(गाँठ_STATE(ksCurThread))->tcbName, TCB_NAME_लंबाई);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_लंबाई);
        tcbDebugAppend(tcb);
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */

        वापस cap_thread_cap_new(TCB_REF(tcb));
    }

    हाल seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        वापस cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    हाल seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        वापस cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    हाल seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        वापस cap_cnode_cap_new(user_आकार, 0, 0, CTE_REF(regionBase));

    हाल seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        वापस cap_untyped_cap_new(0, !!deviceMemory, user_आकार, WORD_REF(regionBase));

#अगर_घोषित CONFIG_KERNEL_MCS
    हाल seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        वापस cap_sched_context_cap_new(SC_REF(regionBase), user_आकार);

    हाल seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        वापस cap_reply_cap_new(REPLY_REF(regionBase), true);
#पूर्ण_अगर

    शेष:
        fail("Invalid object type");
    }
}

व्योम createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t dest_लंबाई,
                      व्योम *regionBase, word_t user_आकार, bool_t deviceMemory)
{
    word_t object_आकार;
    व्योम *अग्र_Free_Area;
    word_t i;
    word_t योग_Object_आकार UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    object_आकार = get_Object_आकार(t, user_आकार);
    योग_Object_आकार = dest_लंबाई << object_आकार;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    अग्र_Free_Area = regionBase;
    क्रम (i = 0; i < dest_लंबाई; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (व्योम *)((word_t)अग्र_Free_Area + (i << object_आकार)), user_आकार, deviceMemory);

        /* Insert the cap into the user's cspace. */
        जोड़ो_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#अगर_घोषित CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t लंबाई,
                             cptr_t cap_सूचकांक, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *बफर)
#अन्यथा
exception_t decodeInvocation(word_t invLabel, word_t लंबाई,
                             cptr_t cap_सूचकांक, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *बफर)
#पूर्ण_अगर
{
    अगर (isArchCap(cap)) {
        वापस Arch_decodeInvocation(invLabel, लंबाई, cap_सूचकांक,
                                     slot, cap, call, बफर);
    }

    चयन (cap_get_capType(cap)) {
    हाल cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", cap_सूचकांक);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;

    हाल cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", cap_सूचकांक);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;

    हाल cap_endpoint_cap:
        अगर (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      cap_सूचकांक);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
#अगर_घोषित CONFIG_KERNEL_MCS
        वापस performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#अन्यथा
        वापस performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#पूर्ण_अगर

    हाल cap_notification_cap: {
        अगर (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      cap_सूचकांक);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_reply_cap:
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performInvocation_Reply(
                   गाँठ_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#अन्यथा
    हाल cap_reply_cap:
        अगर (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      cap_सूचकांक);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#पूर्ण_अगर

    हाल cap_thread_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
        अगर (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#पूर्ण_अगर
        वापस decodeTCBInvocation(invLabel, लंबाई, cap, slot, call, बफर);

    हाल cap_domain_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
        अगर (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#पूर्ण_अगर
        वापस decodeDomainInvocation(invLabel, लंबाई, बफर);

    हाल cap_cnode_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
        अगर (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#पूर्ण_अगर
        वापस decodeCNodeInvocation(invLabel, लंबाई, cap, बफर);

    हाल cap_untyped_cap:
        वापस decodeUntypedInvocation(invLabel, लंबाई, slot, cap, call, बफर);

    हाल cap_irq_control_cap:
        वापस decodeIRQControlInvocation(invLabel, लंबाई, slot, बफर);

    हाल cap_irq_handler_cap:
        वापस decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_sched_control_cap:
        अगर (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        वापस decodeSchedControlInvocation(invLabel, cap, लंबाई, बफर);

    हाल cap_sched_context_cap:
        अगर (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        वापस decodeSchedContextInvocation(invLabel, sc, call);
#पूर्ण_अगर
    शेष:
        fail("Invalid cap type");
    }
}

#अगर_घोषित CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, गाँठ_STATE(ksCurThread), ep);

    वापस EXCEPTION_NONE;
}
#अन्यथा
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, गाँठ_STATE(ksCurThread), ep);

    वापस EXCEPTION_NONE;
}
#पूर्ण_अगर

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    वापस EXCEPTION_NONE;
}

#अगर_घोषित CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    वापस EXCEPTION_NONE;
}
#अन्यथा
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(गाँठ_STATE(ksCurThread), thread, slot, canGrant);
    वापस EXCEPTION_NONE;
}
#पूर्ण_अगर

word_t CONST cap_get_cap_आकार_Bits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    चयन (ctag) {
    हाल cap_untyped_cap:
        वापस cap_untyped_cap_get_cap_Block_आकार(cap);

    हाल cap_endpoint_cap:
        वापस seL4_EndpointBits;

    हाल cap_notification_cap:
        वापस seL4_NotificationBits;

    हाल cap_cnode_cap:
        वापस cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    हाल cap_thread_cap:
        वापस seL4_TCBBits;

    हाल cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        अगर (type == ZombieType_ZombieTCB) {
            वापस seL4_TCBBits;
        }
        वापस ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    हाल cap_null_cap:
        वापस 0;

    हाल cap_domain_cap:
        वापस 0;

    हाल cap_reply_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
        वापस seL4_ReplyBits;
#अन्यथा
        वापस 0;
#पूर्ण_अगर

    हाल cap_irq_control_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_sched_control_cap:
#पूर्ण_अगर
        वापस 0;

    हाल cap_irq_handler_cap:
        वापस 0;

#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_sched_context_cap:
        वापस cap_sched_context_cap_get_capSCSizeBits(cap);
#पूर्ण_अगर

    शेष:
        वापस cap_get_arch_Cap_आकार_Bits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    चयन (ctag) {
    हाल cap_untyped_cap:
        वापस true;

    हाल cap_endpoint_cap:
        वापस true;

    हाल cap_notification_cap:
        वापस true;

    हाल cap_cnode_cap:
        वापस true;

    हाल cap_thread_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_sched_context_cap:
#पूर्ण_अगर
        वापस true;

    हाल cap_zombie_cap:
        वापस true;

    हाल cap_domain_cap:
        वापस false;

    हाल cap_reply_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
        वापस true;
#अन्यथा
        वापस false;
#पूर्ण_अगर

    हाल cap_irq_control_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_sched_control_cap:
#पूर्ण_अगर
        वापस false;

    हाल cap_irq_handler_cap:
        वापस false;

    शेष:
        वापस cap_get_archCapIsPhysical(cap);
    }
}

व्योम *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    चयन (ctag) {
    हाल cap_untyped_cap:
        वापस WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    हाल cap_endpoint_cap:
        वापस EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    हाल cap_notification_cap:
        वापस NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    हाल cap_cnode_cap:
        वापस CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    हाल cap_thread_cap:
        वापस TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    हाल cap_zombie_cap:
        वापस CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    हाल cap_domain_cap:
        वापस शून्य;

    हाल cap_reply_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
        वापस REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#अन्यथा
        वापस शून्य;
#पूर्ण_अगर

    हाल cap_irq_control_cap:
#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_sched_control_cap:
#पूर्ण_अगर
        वापस शून्य;

    हाल cap_irq_handler_cap:
        वापस शून्य;

#अगर_घोषित CONFIG_KERNEL_MCS
    हाल cap_sched_context_cap:
        वापस SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#पूर्ण_अगर

    शेष:
        वापस cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    अगर (isArchCap(derivedCap)) {
        वापस Arch_isCapRevocable(derivedCap, srcCap);
    }
    चयन (cap_get_capType(derivedCap)) {
    हाल cap_endpoint_cap:
        वापस (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    हाल cap_notification_cap:
        वापस (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    हाल cap_irq_handler_cap:
        वापस (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    हाल cap_untyped_cap:
        वापस true;

    शेष:
        वापस false;
    }
}
