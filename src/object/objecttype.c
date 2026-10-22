/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <assert.h>
#include <config.h>
#include <types.h>
#include <api/failures.h>
#include <api/syscall.h>
#include <arch/object/objecttype.h>
#include <machine/io.h>
#include <object/objecttype.h>
#include <object/structures.h>
#include <object/notification.h>
#include <object/endpoint.h>
#include <object/cnode.h>
#include <object/interrupt.h>
#ifdef CONFIG_KERNEL_MCS
#include <object/schedcontext.h>
#include <object/schedcontrol.h>
#endif
#include <object/tcb.h>
#include <object/untyped.h>
#include <model/statedata.h>
#include <kernel/thread.h>
#include <kernel/vspace.h>
#include <machine.h>
#include <util.h>
#include <string.h>

word_t getObjectSize(word_t t, word_t userObjSize)
{
    nếu (t >= seL4_NonArchObjectTypeCount) {
        trả_về Arch_getObjectSize(t);
    } không_thì {
        switch (t) {
        case seL4_TCBObject:
            trả_về seL4_TCBBits;
        case seL4_EndpointObject:
            trả_về seL4_EndpointBits;
        case seL4_NotificationObject:
            trả_về seL4_NotificationBits;
        case seL4_CapTableObject:
            trả_về seL4_SlotBits + userObjSize;
        case seL4_UntypedObject:
            trả_về userObjSize;
#ifdef CONFIG_KERNEL_MCS
        case seL4_SchedContextObject:
            trả_về userObjSize;
        case seL4_ReplyObject:
            trả_về seL4_ReplyBits;
#endif
        default:
            fail("Invalid object type");
            trả_về 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    nếu (isArchCap(cap)) {
        trả_về Arch_deriveCap(slot, cap);
    }

    switch (cap_get_capType(cap)) {
    case cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        break;

    case cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        break;

    case cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        nếu (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } không_thì {
            ret.cap = cap;
        }
        break;

#ifndef CONFIG_KERNEL_MCS
    case cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        break;
#endif
    default:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    trả_về ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    nếu (isArchCap(cap)) {
        trả_về Arch_finaliseCap(cap, final);
    }

    switch (cap_get_capType(cap)) {
    case cap_endpoint_cap:
        nếu (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        trả_về fc_ret;

    case cap_notification_cap:
        nếu (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ifdef CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#endif
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        trả_về fc_ret;

    case cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        nếu (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            nếu (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                switch (thread_state_get_tsType(tcb->tcbState)) {
                case ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    break;
                case ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    break;
                default:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        trả_về fc_ret;
#endif
    case cap_null_cap:
    case cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        trả_về fc_ret;
    }

    nếu (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    switch (cap_get_capType(cap)) {
    case cap_cnode_cap: {
        nếu (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            trả_về fc_ret;
        }
        break;
    }

    case cap_thread_cap: {
        nếu (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ifdef CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            nếu (sc) {
                schedContext_unbindTCB(sc);
                nếu (sc->scYieldFrom) {
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
            trả_về fc_ret;
        }
        break;
    }

#ifdef CONFIG_KERNEL_MCS
    case cap_sched_context_cap:
        nếu (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            nếu (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            trả_về fc_ret;
        }
        break;
#endif

    case cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        trả_về fc_ret;

    case cap_irq_handler_cap:
        nếu (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            trả_về fc_ret;
        }
        break;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    trả_về fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    switch (cap_get_capType(cap)) {
    case cap_endpoint_cap:
        trả_về cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    default:
        trả_về false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    switch (cap_get_capType(cap_a)) {
    case cap_untyped_cap:
        nếu (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, aTop, bTop;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            aTop = aBase + MASK(cap_untyped_cap_get_capBlockSize(cap_a));
            bTop = bBase + MASK(cap_get_capSizeBits(cap_b));

            trả_về (aBase <= bBase) && (bTop <= aTop) && (bBase <= bTop);
        }
        break;

    case cap_endpoint_cap:
        nếu (cap_get_capType(cap_b) == cap_endpoint_cap) {
            trả_về cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        break;

    case cap_notification_cap:
        nếu (cap_get_capType(cap_b) == cap_notification_cap) {
            trả_về cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        break;

    case cap_cnode_cap:
        nếu (cap_get_capType(cap_b) == cap_cnode_cap) {
            trả_về (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        break;

    case cap_thread_cap:
        nếu (cap_get_capType(cap_b) == cap_thread_cap) {
            trả_về cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        break;

    case cap_reply_cap:
        nếu (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            trả_về cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#không_thì
            trả_về cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
        }
        break;

    case cap_domain_cap:
        nếu (cap_get_capType(cap_b) == cap_domain_cap) {
            trả_về true;
        }
        break;

    case cap_irq_control_cap:
        nếu (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            trả_về true;
        }
        break;

    case cap_irq_handler_cap:
        nếu (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            trả_về (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        break;

#ifdef CONFIG_KERNEL_MCS
    case cap_sched_context_cap:
        nếu (cap_get_capType(cap_b) == cap_sched_context_cap) {
            trả_về (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        break;
    case cap_sched_control_cap:
        nếu (cap_get_capType(cap_b) == cap_sched_control_cap) {
            trả_về true;
        }
        break;
#endif
    default:
        nếu (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            trả_về Arch_sameRegionAs(cap_a, cap_b);
        }
        break;
    }

    trả_về false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    nếu (cap_get_capType(cap_a) == cap_untyped_cap) {
        trả_về false;
    }
    nếu (cap_get_capType(cap_a) == cap_irq_control_cap) {
        trả_về false;
    }
    nếu (isArchCap(cap_a) && isArchCap(cap_b)) {
        trả_về Arch_sameObjectAs(cap_a, cap_b);
    }
    trả_về sameRegionAs(cap_a, cap_b);
}

cap_t CONST updateCapData(bool_t preserve, word_t newData, cap_t cap)
{
    nếu (isArchCap(cap)) {
        trả_về Arch_updateCapData(preserve, newData, cap);
    }

    switch (cap_get_capType(cap)) {
    case cap_endpoint_cap:
        nếu (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            trả_về cap_endpoint_cap_set_capEPBadge(cap, newData);
        } không_thì {
            trả_về cap_null_cap_new();
        }

    case cap_notification_cap:
        nếu (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            trả_về cap_notification_cap_set_capNtfnBadge(cap, newData);
        } không_thì {
            trả_về cap_null_cap_new();
        }

    case cap_cnode_cap: {
        word_t guard, guardSize;
        seL4_CNode_CapData_t w = { .words = { newData } };

        guardSize = seL4_CNode_CapData_get_guardSize(w);

        nếu (guardSize + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            trả_về cap_null_cap_new();
        } không_thì {
            cap_t new_cap;

            guard = seL4_CNode_CapData_get_guard(w) & MASK(guardSize);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_capCNodeGuardSize(new_cap,
                                                          guardSize);

            trả_về new_cap;
        }
    }

    default:
        trả_về cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    nếu (isArchCap(cap)) {
        trả_về Arch_maskCapRights(cap_rights, cap);
    }

    switch (cap_get_capType(cap)) {
    case cap_null_cap:
    case cap_domain_cap:
    case cap_cnode_cap:
    case cap_untyped_cap:
    case cap_irq_control_cap:
    case cap_irq_handler_cap:
    case cap_zombie_cap:
    case cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    case cap_sched_context_cap:
    case cap_sched_control_cap:
#endif
        trả_về cap;

    case cap_endpoint_cap: {
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

        trả_về new_cap;
    }

    case cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        trả_về new_cap;
    }
    case cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        trả_về new_cap;
    }


    default:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, void *regionBase, word_t userSize, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    nếu (t >= (object_t) seL4_NonArchObjectTypeCount) {
        trả_về Arch_createObject(t, regionBase, userSize, deviceMemory);
    }

    /* Create objects. */
    switch ((api_object_t)t) {
    case seL4_TCBObject: {
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
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(NODE_STATE(ksCurThread))->tcbName, TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_LENGTH);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        trả_về cap_thread_cap_new(TCB_REF(tcb));
    }

    case seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        trả_về cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    case seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        trả_về cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    case seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        trả_về cap_cnode_cap_new(userSize, 0, 0, CTE_REF(regionBase));

    case seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        trả_về cap_untyped_cap_new(0, !!deviceMemory, userSize, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
    case seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        trả_về cap_sched_context_cap_new(SC_REF(regionBase), userSize);

    case seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        trả_về cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    default:
        fail("Invalid object type");
    }
}

void createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t destLength,
                      void *regionBase, word_t userSize, bool_t deviceMemory)
{
    word_t objectSize;
    void *nextFreeArea;
    word_t i;
    word_t totalObjectSize UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    objectSize = getObjectSize(t, userSize);
    totalObjectSize = destLength << objectSize;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    nextFreeArea = regionBase;
    cho (i = 0; i < destLength; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (void *)((word_t)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        /* Insert the cap into the user's cspace. */
        insertNewCap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *buffer)
#không_thì
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *buffer)
#endif
{
    nếu (isArchCap(cap)) {
        trả_về Arch_decodeInvocation(invLabel, length, capIndex,
                                     slot, cap, call, buffer);
    }

    switch (cap_get_capType(cap)) {
    case cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;
        trả_về EXCEPTION_SYSCALL_ERROR;

    case cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalidCapNumber = 0;
        trả_về EXCEPTION_SYSCALL_ERROR;

    case cap_endpoint_cap:
        nếu (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 0;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        trả_về performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#không_thì
        trả_về performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    case cap_notification_cap: {
        nếu (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 0;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        trả_về performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    case cap_reply_cap:
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        trả_về performInvocation_Reply(
                   NODE_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#không_thì
    case cap_reply_cap:
        nếu (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 0;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        trả_về performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    case cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        nếu (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 0;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }
#endif
        trả_về decodeTCBInvocation(invLabel, length, cap, slot, call, buffer);

    case cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        nếu (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 0;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }
#endif
        trả_về decodeDomainInvocation(invLabel, length, buffer);

    case cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        nếu (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 0;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }
#endif
        trả_về decodeCNodeInvocation(invLabel, length, cap, buffer);

    case cap_untyped_cap:
        trả_về decodeUntypedInvocation(invLabel, length, slot, cap, call, buffer);

    case cap_irq_control_cap:
        trả_về decodeIRQControlInvocation(invLabel, length, slot, buffer);

    case cap_irq_handler_cap:
        trả_về decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    case cap_sched_control_cap:
        nếu (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 0;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }
        trả_về decodeSchedControlInvocation(invLabel, cap, length, buffer);

    case cap_sched_context_cap:
        nếu (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalidCapNumber = 0;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        trả_về decodeSchedContextInvocation(invLabel, sc, call);
#endif
    default:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, NODE_STATE(ksCurThread), ep);

    trả_về EXCEPTION_NONE;
}
#không_thì
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, NODE_STATE(ksCurThread), ep);

    trả_về EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    trả_về EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    trả_về EXCEPTION_NONE;
}
#không_thì
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(NODE_STATE(ksCurThread), thread, slot, canGrant);
    trả_về EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_capSizeBits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    switch (ctag) {
    case cap_untyped_cap:
        trả_về cap_untyped_cap_get_capBlockSize(cap);

    case cap_endpoint_cap:
        trả_về seL4_EndpointBits;

    case cap_notification_cap:
        trả_về seL4_NotificationBits;

    case cap_cnode_cap:
        trả_về cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    case cap_thread_cap:
        trả_về seL4_TCBBits;

    case cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        nếu (type == ZombieType_ZombieTCB) {
            trả_về seL4_TCBBits;
        }
        trả_về ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    case cap_null_cap:
        trả_về 0;

    case cap_domain_cap:
        trả_về 0;

    case cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        trả_về seL4_ReplyBits;
#không_thì
        trả_về 0;
#endif

    case cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    case cap_sched_control_cap:
#endif
        trả_về 0;

    case cap_irq_handler_cap:
        trả_về 0;

#ifdef CONFIG_KERNEL_MCS
    case cap_sched_context_cap:
        trả_về cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    default:
        trả_về cap_get_archCapSizeBits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    switch (ctag) {
    case cap_untyped_cap:
        trả_về true;

    case cap_endpoint_cap:
        trả_về true;

    case cap_notification_cap:
        trả_về true;

    case cap_cnode_cap:
        trả_về true;

    case cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    case cap_sched_context_cap:
#endif
        trả_về true;

    case cap_zombie_cap:
        trả_về true;

    case cap_domain_cap:
        trả_về false;

    case cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        trả_về true;
#không_thì
        trả_về false;
#endif

    case cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    case cap_sched_control_cap:
#endif
        trả_về false;

    case cap_irq_handler_cap:
        trả_về false;

    default:
        trả_về cap_get_archCapIsPhysical(cap);
    }
}

void *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    switch (ctag) {
    case cap_untyped_cap:
        trả_về WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    case cap_endpoint_cap:
        trả_về EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    case cap_notification_cap:
        trả_về NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    case cap_cnode_cap:
        trả_về CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    case cap_thread_cap:
        trả_về TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    case cap_zombie_cap:
        trả_về CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    case cap_domain_cap:
        trả_về NULL;

    case cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        trả_về REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#không_thì
        trả_về NULL;
#endif

    case cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    case cap_sched_control_cap:
#endif
        trả_về NULL;

    case cap_irq_handler_cap:
        trả_về NULL;

#ifdef CONFIG_KERNEL_MCS
    case cap_sched_context_cap:
        trả_về SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

    default:
        trả_về cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    nếu (isArchCap(derivedCap)) {
        trả_về Arch_isCapRevocable(derivedCap, srcCap);
    }
    switch (cap_get_capType(derivedCap)) {
    case cap_endpoint_cap:
        trả_về (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    case cap_notification_cap:
        trả_về (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    case cap_irq_handler_cap:
        trả_về (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    case cap_untyped_cap:
        trả_về true;

    default:
        trả_về false;
    }
}
