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
#ifdef CONFIG_KERNEL_MCS
#ضمن <object/schedcontext.h>
#ضمن <object/schedcontrol.h>
#endif
#ضمن <object/tcb.h>
#ضمن <object/untyped.h>
#ضمن <model/statedata.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/vspace.h>
#ضمن <machine.h>
#ضمن <util.h>
#ضمن <string.h>

word_t getObjectSize(word_t t, word_t userObjSize)
{
    إذا (t >= se_L4_Non_Arch_Object_Type_سانى) {
        أرجع Arch_getObjectSize(t);
    } وإلا {
        بدل (t) {
        حالة seL4_TCBObject:
            أرجع seL4_TCBBits;
        حالة seL4_EndpointObject:
            أرجع seL4_EndpointBits;
        حالة seL4_NotificationObject:
            أرجع seL4_NotificationBits;
        حالة seL4_CapTableObject:
            أرجع seL4_SlotBits + userObjSize;
        حالة seL4_UntypedObject:
            أرجع userObjSize;
#ifdef CONFIG_KERNEL_MCS
        حالة seL4_SchedContextObject:
            أرجع userObjSize;
        حالة seL4_ReplyObject:
            أرجع seL4_ReplyBits;
#endif
        افتراضي:
            fail("Invalid object type");
            أرجع 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    إذا (isArchCap(cap)) {
        أرجع Arch_deriveCap(slot, cap);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        اكسر;

    حالة cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        اكسر;

    حالة cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        إذا (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } وإلا {
            ret.cap = cap;
        }
        اكسر;

#ifndef CONFIG_KERNEL_MCS
    حالة cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        اكسر;
#endif
    افتراضي:
        ret.status = EXCEPTION_NONE;
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
#ifdef CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#endif
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        أرجع fc_ret;

    حالة cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        إذا (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            إذا (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                بدل (thread_state_get_tsType(tcb->tcbState)) {
                حالة ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
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
#endif
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
#ifdef CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            إذا (sc) {
                schedContext_unbindTCB(sc);
                إذا (sc->scYieldFrom) {
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
            أرجع fc_ret;
        }
        اكسر;
    }

#ifdef CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        إذا (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            إذا (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            أرجع fc_ret;
        }
        اكسر;
#endif

    حالة cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        أرجع fc_ret;

    حالة cap_irq_handler_cap:
        إذا (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

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
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
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
            word_t aBase, bBase, a_چوققا, b_چوققا;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_چوققا = aBase + MASK(cap_untyped_cap_get_capBlockSize(cap_a));
            b_چوققا = bBase + MASK(cap_get_capSizeBits(cap_b));

            أرجع (aBase <= bBase) && (b_چوققا <= a_چوققا) && (bBase <= b_چوققا);
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

    حالة cap_reply_cap:
        إذا (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            أرجع cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#وإلا
            أرجع cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
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
            أرجع (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        اكسر;

#ifdef CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        إذا (cap_get_capType(cap_b) == cap_sched_context_cap) {
            أرجع (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        اكسر;
    حالة cap_sched_control_cap:
        إذا (cap_get_capType(cap_b) == cap_sched_control_cap) {
            أرجع true;
        }
        اكسر;
#endif
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

cap_t CONST updateCapData(bool_t preserve, word_t newData, cap_t cap)
{
    إذا (isArchCap(cap)) {
        أرجع Arch_updateCapData(preserve, newData, cap);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap:
        إذا (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            أرجع cap_endpoint_cap_set_capEPBadge(cap, newData);
        } وإلا {
            أرجع cap_null_cap_new();
        }

    حالة cap_notification_cap:
        إذا (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            أرجع cap_notification_cap_set_capNtfnBadge(cap, newData);
        } وإلا {
            أرجع cap_null_cap_new();
        }

    حالة cap_cnode_cap: {
        word_t guard, guardSize;
        seL4_CNode_CapData_t w = { .سۆزلەر = { newData } };

        guardSize = seL4_CNode_CapData_get_guardSize(w);

        إذا (guardSize + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            أرجع cap_null_cap_new();
        } وإلا {
            cap_t new_cap;

            guard = seL4_CNode_CapData_get_guard(w) & MASK(guardSize);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_capCNodeGuardSize(new_cap,
                                                          guardSize);

            أرجع new_cap;
        }
    }

    افتراضي:
        أرجع cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    إذا (isArchCap(cap)) {
        أرجع Arch_maskCapRights(cap_rights, cap);
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
#ifdef CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
    حالة cap_sched_control_cap:
#endif
        أرجع cap;

    حالة cap_endpoint_cap: {
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

        أرجع new_cap;
    }

    حالة cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        أرجع new_cap;
    }
    حالة cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        أرجع new_cap;
    }


    افتراضي:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, فراغ *regionBase, word_t userSize, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    إذا (t >= (object_t) se_L4_Non_Arch_Object_Type_سانى) {
        أرجع Arch_createObject(t, regionBase, userSize, deviceMemory);
    }

    /* Create objects. */
    بدل ((api_object_t)t) {
    حالة seL4_TCBObject: {
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
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(تۈگۈن_STATE(ksCurThread))->tcbName, TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_LENGTH);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        أرجع cap_thread_cap_new(TCB_REF(tcb));
    }

    حالة seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        أرجع cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    حالة seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        أرجع cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    حالة seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        أرجع cap_cnode_cap_new(userSize, 0, 0, CTE_REF(regionBase));

    حالة seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        أرجع cap_untyped_cap_new(0, !!deviceMemory, userSize, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
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
        أرجع cap_sched_context_cap_new(SC_REF(regionBase), userSize);

    حالة seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        أرجع cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    افتراضي:
        fail("Invalid object type");
    }
}

فراغ createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t destLength,
                      فراغ *regionBase, word_t userSize, bool_t deviceMemory)
{
    word_t objectSize;
    فراغ *nextFreeArea;
    word_t i;
    word_t totalObjectSize UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    objectSize = getObjectSize(t, userSize);
    totalObjectSize = destLength << objectSize;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    nextFreeArea = regionBase;
    لكل (i = 0; i < destLength; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (فراغ *)((word_t)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        /* Insert the cap into the user's cspace. */
        قىستۇرۇش_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *buffer)
#وإلا
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *buffer)
#endif
{
    إذا (isArchCap(cap)) {
        أرجع Arch_decodeInvocation(invLabel, length, capIndex,
                                     slot, cap, call, buffer);
    }

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_سان = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_سان = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_endpoint_cap:
        إذا (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_سان = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(تۈگۈن_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        أرجع performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#وإلا
        أرجع performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    حالة cap_notification_cap: {
        إذا (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_سان = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(تۈگۈن_STATE(ksCurThread), ThreadState_Restart);
        أرجع performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    حالة cap_reply_cap:
        setThreadState(تۈگۈن_STATE(ksCurThread), ThreadState_Restart);
        أرجع performInvocation_Reply(
                   تۈگۈن_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#وإلا
    حالة cap_reply_cap:
        إذا (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_سان = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(تۈگۈن_STATE(ksCurThread), ThreadState_Restart);
        أرجع performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    حالة cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        إذا (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_سان = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#endif
        أرجع decodeTCBInvocation(invLabel, length, cap, slot, call, buffer);

    حالة cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        إذا (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_سان = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#endif
        أرجع decodeDomainInvocation(invLabel, length, buffer);

    حالة cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        إذا (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_سان = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#endif
        أرجع decodeCNodeInvocation(invLabel, length, cap, buffer);

    حالة cap_untyped_cap:
        أرجع decodeUntypedInvocation(invLabel, length, slot, cap, call, buffer);

    حالة cap_irq_control_cap:
        أرجع decodeIRQControlInvocation(invLabel, length, slot, buffer);

    حالة cap_irq_handler_cap:
        أرجع decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
        إذا (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_سان = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        أرجع decodeSchedControlInvocation(invLabel, cap, length, buffer);

    حالة cap_sched_context_cap:
        إذا (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_سان = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        أرجع decodeSchedContextInvocation(invLabel, sc, call);
#endif
    افتراضي:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, تۈگۈن_STATE(ksCurThread), ep);

    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, تۈگۈن_STATE(ksCurThread), ep);

    أرجع EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    أرجع EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    أرجع EXCEPTION_NONE;
}
#وإلا
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(تۈگۈن_STATE(ksCurThread), thread, slot, canGrant);
    أرجع EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_capSizeBits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    بدل (ctag) {
    حالة cap_untyped_cap:
        أرجع cap_untyped_cap_get_capBlockSize(cap);

    حالة cap_endpoint_cap:
        أرجع seL4_EndpointBits;

    حالة cap_notification_cap:
        أرجع seL4_NotificationBits;

    حالة cap_cnode_cap:
        أرجع cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    حالة cap_thread_cap:
        أرجع seL4_TCBBits;

    حالة cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        إذا (type == ZombieType_ZombieTCB) {
            أرجع seL4_TCBBits;
        }
        أرجع ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    حالة cap_null_cap:
        أرجع 0;

    حالة cap_domain_cap:
        أرجع 0;

    حالة cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        أرجع seL4_ReplyBits;
#وإلا
        أرجع 0;
#endif

    حالة cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#endif
        أرجع 0;

    حالة cap_irq_handler_cap:
        أرجع 0;

#ifdef CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        أرجع cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    افتراضي:
        أرجع cap_get_archCapSizeBits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

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
#ifdef CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
#endif
        أرجع true;

    حالة cap_zombie_cap:
        أرجع true;

    حالة cap_domain_cap:
        أرجع false;

    حالة cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        أرجع true;
#وإلا
        أرجع false;
#endif

    حالة cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#endif
        أرجع false;

    حالة cap_irq_handler_cap:
        أرجع false;

    افتراضي:
        أرجع cap_get_archCapIsPhysical(cap);
    }
}

فراغ *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    بدل (ctag) {
    حالة cap_untyped_cap:
        أرجع WORD_PTR(cap_untyped_cap_get_capPtr(cap));

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

    حالة cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        أرجع REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#وإلا
        أرجع عدم;
#endif

    حالة cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    حالة cap_sched_control_cap:
#endif
        أرجع عدم;

    حالة cap_irq_handler_cap:
        أرجع عدم;

#ifdef CONFIG_KERNEL_MCS
    حالة cap_sched_context_cap:
        أرجع SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

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
