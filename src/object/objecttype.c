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
    जर (t >= se_L4_Non_Arch_Object_Type_मोजणी) {
        परत Arch_getObjectSize(t);
    } अन्यथा {
        निवड (t) {
        स्थिती seL4_TCBObject:
            परत seL4_TCBBits;
        स्थिती seL4_EndpointObject:
            परत seL4_EndpointBits;
        स्थिती seL4_NotificationObject:
            परत seL4_NotificationBits;
        स्थिती seL4_CapTableObject:
            परत seL4_SlotBits + userObjSize;
        स्थिती seL4_UntypedObject:
            परत userObjSize;
#ifdef CONFIG_KERNEL_MCS
        स्थिती seL4_SchedContextObject:
            परत userObjSize;
        स्थिती seL4_ReplyObject:
            परत seL4_ReplyBits;
#endif
        पूर्वनिर्धारित:
            fail("Invalid object type");
            परत 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    जर (isArchCap(cap)) {
        परत Arch_deriveCap(slot, cap);
    }

    निवड (cap_get_capType(cap)) {
    स्थिती cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        थांबा;

    स्थिती cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        थांबा;

    स्थिती cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        जर (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } अन्यथा {
            ret.cap = cap;
        }
        थांबा;

#ifndef CONFIG_KERNEL_MCS
    स्थिती cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        थांबा;
#endif
    पूर्वनिर्धारित:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    परत ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    जर (isArchCap(cap)) {
        परत Arch_finaliseCap(cap, final);
    }

    निवड (cap_get_capType(cap)) {
    स्थिती cap_endpoint_cap:
        जर (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        परत fc_ret;

    स्थिती cap_notification_cap:
        जर (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ifdef CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#endif
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        परत fc_ret;

    स्थिती cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        जर (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            जर (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                निवड (thread_state_get_tsType(tcb->tcbState)) {
                स्थिती ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    थांबा;
                स्थिती ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    थांबा;
                पूर्वनिर्धारित:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        परत fc_ret;
#endif
    स्थिती cap_null_cap:
    स्थिती cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        परत fc_ret;
    }

    जर (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    निवड (cap_get_capType(cap)) {
    स्थिती cap_cnode_cap: {
        जर (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            परत fc_ret;
        }
        थांबा;
    }

    स्थिती cap_thread_cap: {
        जर (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ifdef CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            जर (sc) {
                schedContext_unbindTCB(sc);
                जर (sc->scYieldFrom) {
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
            परत fc_ret;
        }
        थांबा;
    }

#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_sched_context_cap:
        जर (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            जर (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            परत fc_ret;
        }
        थांबा;
#endif

    स्थिती cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        परत fc_ret;

    स्थिती cap_irq_handler_cap:
        जर (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            परत fc_ret;
        }
        थांबा;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    परत fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    निवड (cap_get_capType(cap)) {
    स्थिती cap_endpoint_cap:
        परत cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    पूर्वनिर्धारित:
        परत false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    निवड (cap_get_capType(cap_a)) {
    स्थिती cap_untyped_cap:
        जर (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, a_शीर्ष, b_शीर्ष;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_शीर्ष = aBase + MASK(cap_untyped_cap_get_capBlockSize(cap_a));
            b_शीर्ष = bBase + MASK(cap_get_capSizeBits(cap_b));

            परत (aBase <= bBase) && (b_शीर्ष <= a_शीर्ष) && (bBase <= b_शीर्ष);
        }
        थांबा;

    स्थिती cap_endpoint_cap:
        जर (cap_get_capType(cap_b) == cap_endpoint_cap) {
            परत cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        थांबा;

    स्थिती cap_notification_cap:
        जर (cap_get_capType(cap_b) == cap_notification_cap) {
            परत cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        थांबा;

    स्थिती cap_cnode_cap:
        जर (cap_get_capType(cap_b) == cap_cnode_cap) {
            परत (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        थांबा;

    स्थिती cap_thread_cap:
        जर (cap_get_capType(cap_b) == cap_thread_cap) {
            परत cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        थांबा;

    स्थिती cap_reply_cap:
        जर (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            परत cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#अन्यथा
            परत cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
        }
        थांबा;

    स्थिती cap_domain_cap:
        जर (cap_get_capType(cap_b) == cap_domain_cap) {
            परत true;
        }
        थांबा;

    स्थिती cap_irq_control_cap:
        जर (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            परत true;
        }
        थांबा;

    स्थिती cap_irq_handler_cap:
        जर (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            परत (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        थांबा;

#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_sched_context_cap:
        जर (cap_get_capType(cap_b) == cap_sched_context_cap) {
            परत (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        थांबा;
    स्थिती cap_sched_control_cap:
        जर (cap_get_capType(cap_b) == cap_sched_control_cap) {
            परत true;
        }
        थांबा;
#endif
    पूर्वनिर्धारित:
        जर (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            परत Arch_sameRegionAs(cap_a, cap_b);
        }
        थांबा;
    }

    परत false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    जर (cap_get_capType(cap_a) == cap_untyped_cap) {
        परत false;
    }
    जर (cap_get_capType(cap_a) == cap_irq_control_cap) {
        परत false;
    }
    जर (isArchCap(cap_a) && isArchCap(cap_b)) {
        परत Arch_sameObjectAs(cap_a, cap_b);
    }
    परत sameRegionAs(cap_a, cap_b);
}

cap_t CONST updateCapData(bool_t preserve, word_t newData, cap_t cap)
{
    जर (isArchCap(cap)) {
        परत Arch_updateCapData(preserve, newData, cap);
    }

    निवड (cap_get_capType(cap)) {
    स्थिती cap_endpoint_cap:
        जर (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            परत cap_endpoint_cap_set_capEPBadge(cap, newData);
        } अन्यथा {
            परत cap_null_cap_new();
        }

    स्थिती cap_notification_cap:
        जर (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            परत cap_notification_cap_set_capNtfnBadge(cap, newData);
        } अन्यथा {
            परत cap_null_cap_new();
        }

    स्थिती cap_cnode_cap: {
        word_t guard, guardSize;
        seL4_CNode_CapData_t w = { .शब्द = { newData } };

        guardSize = seL4_CNode_CapData_get_guardSize(w);

        जर (guardSize + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            परत cap_null_cap_new();
        } अन्यथा {
            cap_t new_cap;

            guard = seL4_CNode_CapData_get_guard(w) & MASK(guardSize);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_capCNodeGuardSize(new_cap,
                                                          guardSize);

            परत new_cap;
        }
    }

    पूर्वनिर्धारित:
        परत cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    जर (isArchCap(cap)) {
        परत Arch_maskCapRights(cap_rights, cap);
    }

    निवड (cap_get_capType(cap)) {
    स्थिती cap_null_cap:
    स्थिती cap_domain_cap:
    स्थिती cap_cnode_cap:
    स्थिती cap_untyped_cap:
    स्थिती cap_irq_control_cap:
    स्थिती cap_irq_handler_cap:
    स्थिती cap_zombie_cap:
    स्थिती cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_sched_context_cap:
    स्थिती cap_sched_control_cap:
#endif
        परत cap;

    स्थिती cap_endpoint_cap: {
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

        परत new_cap;
    }

    स्थिती cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        परत new_cap;
    }
    स्थिती cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        परत new_cap;
    }


    पूर्वनिर्धारित:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    जर (t >= (object_t) se_L4_Non_Arch_Object_Type_मोजणी) {
        परत Arch_createObject(t, regionBase, userSize, deviceMemory);
    }

    /* Create objects. */
    निवड ((api_object_t)t) {
    स्थिती seL4_TCBObject: {
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
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(गाठ_STATE(ksCurThread))->tcbName, TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_LENGTH);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        परत cap_thread_cap_new(TCB_REF(tcb));
    }

    स्थिती seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        परत cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    स्थिती seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        परत cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    स्थिती seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        परत cap_cnode_cap_new(userSize, 0, 0, CTE_REF(regionBase));

    स्थिती seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        परत cap_untyped_cap_new(0, !!deviceMemory, userSize, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
    स्थिती seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        परत cap_sched_context_cap_new(SC_REF(regionBase), userSize);

    स्थिती seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        परत cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    पूर्वनिर्धारित:
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
    साठी (i = 0; i < destLength; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (शून्य *)((word_t)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        /* Insert the cap into the user's cspace. */
        घाला_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *buffer)
#अन्यथा
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *buffer)
#endif
{
    जर (isArchCap(cap)) {
        परत Arch_decodeInvocation(invLabel, length, capIndex,
                                     slot, cap, call, buffer);
    }

    निवड (cap_get_capType(cap)) {
    स्थिती cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        परत EXCEPTION_SYSCALL_ERROR;

    स्थिती cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        परत EXCEPTION_SYSCALL_ERROR;

    स्थिती cap_endpoint_cap:
        जर (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            परत EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        परत performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#अन्यथा
        परत performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    स्थिती cap_notification_cap: {
        जर (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            परत EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        परत performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_reply_cap:
        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        परत performInvocation_Reply(
                   गाठ_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#अन्यथा
    स्थिती cap_reply_cap:
        जर (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            परत EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        परत performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    स्थिती cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        जर (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            परत EXCEPTION_SYSCALL_ERROR;
        }
#endif
        परत decodeTCBInvocation(invLabel, length, cap, slot, call, buffer);

    स्थिती cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        जर (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            परत EXCEPTION_SYSCALL_ERROR;
        }
#endif
        परत decodeDomainInvocation(invLabel, length, buffer);

    स्थिती cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        जर (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            परत EXCEPTION_SYSCALL_ERROR;
        }
#endif
        परत decodeCNodeInvocation(invLabel, length, cap, buffer);

    स्थिती cap_untyped_cap:
        परत decodeUntypedInvocation(invLabel, length, slot, cap, call, buffer);

    स्थिती cap_irq_control_cap:
        परत decodeIRQControlInvocation(invLabel, length, slot, buffer);

    स्थिती cap_irq_handler_cap:
        परत decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_sched_control_cap:
        जर (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            परत EXCEPTION_SYSCALL_ERROR;
        }
        परत decodeSchedControlInvocation(invLabel, cap, length, buffer);

    स्थिती cap_sched_context_cap:
        जर (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            परत EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        परत decodeSchedContextInvocation(invLabel, sc, call);
#endif
    पूर्वनिर्धारित:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, गाठ_STATE(ksCurThread), ep);

    परत EXCEPTION_NONE;
}
#अन्यथा
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, गाठ_STATE(ksCurThread), ep);

    परत EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    परत EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    परत EXCEPTION_NONE;
}
#अन्यथा
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(गाठ_STATE(ksCurThread), thread, slot, canGrant);
    परत EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_capSizeBits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    निवड (ctag) {
    स्थिती cap_untyped_cap:
        परत cap_untyped_cap_get_capBlockSize(cap);

    स्थिती cap_endpoint_cap:
        परत seL4_EndpointBits;

    स्थिती cap_notification_cap:
        परत seL4_NotificationBits;

    स्थिती cap_cnode_cap:
        परत cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    स्थिती cap_thread_cap:
        परत seL4_TCBBits;

    स्थिती cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        जर (type == ZombieType_ZombieTCB) {
            परत seL4_TCBBits;
        }
        परत ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    स्थिती cap_null_cap:
        परत 0;

    स्थिती cap_domain_cap:
        परत 0;

    स्थिती cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        परत seL4_ReplyBits;
#अन्यथा
        परत 0;
#endif

    स्थिती cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_sched_control_cap:
#endif
        परत 0;

    स्थिती cap_irq_handler_cap:
        परत 0;

#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_sched_context_cap:
        परत cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    पूर्वनिर्धारित:
        परत cap_get_archCapSizeBits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    निवड (ctag) {
    स्थिती cap_untyped_cap:
        परत true;

    स्थिती cap_endpoint_cap:
        परत true;

    स्थिती cap_notification_cap:
        परत true;

    स्थिती cap_cnode_cap:
        परत true;

    स्थिती cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_sched_context_cap:
#endif
        परत true;

    स्थिती cap_zombie_cap:
        परत true;

    स्थिती cap_domain_cap:
        परत false;

    स्थिती cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        परत true;
#अन्यथा
        परत false;
#endif

    स्थिती cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_sched_control_cap:
#endif
        परत false;

    स्थिती cap_irq_handler_cap:
        परत false;

    पूर्वनिर्धारित:
        परत cap_get_archCapIsPhysical(cap);
    }
}

शून्य *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    निवड (ctag) {
    स्थिती cap_untyped_cap:
        परत WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    स्थिती cap_endpoint_cap:
        परत EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    स्थिती cap_notification_cap:
        परत NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    स्थिती cap_cnode_cap:
        परत CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    स्थिती cap_thread_cap:
        परत TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    स्थिती cap_zombie_cap:
        परत CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    स्थिती cap_domain_cap:
        परत रिक्त;

    स्थिती cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        परत REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#अन्यथा
        परत रिक्त;
#endif

    स्थिती cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_sched_control_cap:
#endif
        परत रिक्त;

    स्थिती cap_irq_handler_cap:
        परत रिक्त;

#ifdef CONFIG_KERNEL_MCS
    स्थिती cap_sched_context_cap:
        परत SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

    पूर्वनिर्धारित:
        परत cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    जर (isArchCap(derivedCap)) {
        परत Arch_isCapRevocable(derivedCap, srcCap);
    }
    निवड (cap_get_capType(derivedCap)) {
    स्थिती cap_endpoint_cap:
        परत (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    स्थिती cap_notification_cap:
        परत (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    स्थिती cap_irq_handler_cap:
        परत (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    स्थिती cap_untyped_cap:
        परत true;

    पूर्वनिर्धारित:
        परत false;
    }
}
