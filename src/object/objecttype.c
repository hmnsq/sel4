/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/objecttype.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <object/objecttype.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <object/notification.h>
#ଅନ୍ତର୍ଭାଵ <object/endpoint.h>
#ଅନ୍ତର୍ଭାଵ <object/cnode.h>
#ଅନ୍ତର୍ଭାଵ <object/interrupt.h>
#ifdef CONFIG_KERNEL_MCS
#ଅନ୍ତର୍ଭାଵ <object/schedcontext.h>
#ଅନ୍ତର୍ଭାଵ <object/schedcontrol.h>
#endif
#ଅନ୍ତର୍ଭାଵ <object/tcb.h>
#ଅନ୍ତର୍ଭାଵ <object/untyped.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <machine.h>
#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

word_t getObjectSize(word_t t, word_t userObjSize)
{
    ଯଦି (t >= se_L4_Non_Arch_Object_Type_ଗଣନା) {
        ଫେରନ୍ତୁ Arch_getObjectSize(t);
    } ଅନ୍ୟଥା {
        ବାଛ (t) {
        ସ୍ଥିତି seL4_TCBObject:
            ଫେରନ୍ତୁ seL4_TCBBits;
        ସ୍ଥିତି seL4_EndpointObject:
            ଫେରନ୍ତୁ seL4_EndpointBits;
        ସ୍ଥିତି seL4_NotificationObject:
            ଫେରନ୍ତୁ seL4_NotificationBits;
        ସ୍ଥିତି seL4_CapTableObject:
            ଫେରନ୍ତୁ seL4_SlotBits + userObjSize;
        ସ୍ଥିତି seL4_UntypedObject:
            ଫେରନ୍ତୁ userObjSize;
#ifdef CONFIG_KERNEL_MCS
        ସ୍ଥିତି seL4_SchedContextObject:
            ଫେରନ୍ତୁ userObjSize;
        ସ୍ଥିତି seL4_ReplyObject:
            ଫେରନ୍ତୁ seL4_ReplyBits;
#endif
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            fail("Invalid object type");
            ଫେରନ୍ତୁ 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ଯଦି (isArchCap(cap)) {
        ଫେରନ୍ତୁ Arch_deriveCap(slot, cap);
    }

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        ଯଦି (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } ଅନ୍ୟଥା {
            ret.cap = cap;
        }
        ଭାଙ୍ଗ;

#ifndef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ଭାଙ୍ଗ;
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    ଫେରନ୍ତୁ ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    ଯଦି (isArchCap(cap)) {
        ଫେରନ୍ତୁ Arch_finaliseCap(cap, final);
    }

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_endpoint_cap:
        ଯଦି (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ଫେରନ୍ତୁ fc_ret;

    ସ୍ଥିତି cap_notification_cap:
        ଯଦି (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ifdef CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#endif
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ଫେରନ୍ତୁ fc_ret;

    ସ୍ଥିତି cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ଯଦି (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            ଯଦି (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                ବାଛ (thread_state_get_tsType(tcb->tcbState)) {
                ସ୍ଥିତି ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    ଭାଙ୍ଗ;
                ସ୍ଥିତି ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    ଭାଙ୍ଗ;
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ଫେରନ୍ତୁ fc_ret;
#endif
    ସ୍ଥିତି cap_null_cap:
    ସ୍ଥିତି cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ଫେରନ୍ତୁ fc_ret;
    }

    ଯଦି (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_cnode_cap: {
        ଯଦି (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            ଫେରନ୍ତୁ fc_ret;
        }
        ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି cap_thread_cap: {
        ଯଦି (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ifdef CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            ଯଦି (sc) {
                schedContext_unbindTCB(sc);
                ଯଦି (sc->scYieldFrom) {
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
            ଫେରନ୍ତୁ fc_ret;
        }
        ଭାଙ୍ଗ;
    }

#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_sched_context_cap:
        ଯଦି (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            ଯଦି (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            ଫେରନ୍ତୁ fc_ret;
        }
        ଭାଙ୍ଗ;
#endif

    ସ୍ଥିତି cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        ଫେରନ୍ତୁ fc_ret;

    ସ୍ଥିତି cap_irq_handler_cap:
        ଯଦି (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ଫେରନ୍ତୁ fc_ret;
        }
        ଭାଙ୍ଗ;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ଫେରନ୍ତୁ fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_endpoint_cap:
        ଫେରନ୍ତୁ cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ବାଛ (cap_get_capType(cap_a)) {
    ସ୍ଥିତି cap_untyped_cap:
        ଯଦି (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, a_ଶୀର୍ଷ, b_ଶୀର୍ଷ;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_ଶୀର୍ଷ = aBase + MASK(cap_untyped_cap_get_capBlockSize(cap_a));
            b_ଶୀର୍ଷ = bBase + MASK(cap_get_capSizeBits(cap_b));

            ଫେରନ୍ତୁ (aBase <= bBase) && (b_ଶୀର୍ଷ <= a_ଶୀର୍ଷ) && (bBase <= b_ଶୀର୍ଷ);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_endpoint_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_endpoint_cap) {
            ଫେରନ୍ତୁ cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_notification_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_notification_cap) {
            ଫେରନ୍ତୁ cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_cnode_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_cnode_cap) {
            ଫେରନ୍ତୁ (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_thread_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_thread_cap) {
            ଫେରନ୍ତୁ cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_reply_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            ଫେରନ୍ତୁ cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#ଅନ୍ୟଥା
            ଫେରନ୍ତୁ cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_domain_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_domain_cap) {
            ଫେରନ୍ତୁ true;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_irq_control_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            ଫେରନ୍ତୁ true;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_irq_handler_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            ଫେରନ୍ତୁ (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        ଭାଙ୍ଗ;

#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_sched_context_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_sched_context_cap) {
            ଫେରନ୍ତୁ (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_sched_control_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_sched_control_cap) {
            ଫେରନ୍ତୁ true;
        }
        ଭାଙ୍ଗ;
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଯଦି (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            ଫେରନ୍ତୁ Arch_sameRegionAs(cap_a, cap_b);
        }
        ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ଯଦି (cap_get_capType(cap_a) == cap_untyped_cap) {
        ଫେରନ୍ତୁ false;
    }
    ଯଦି (cap_get_capType(cap_a) == cap_irq_control_cap) {
        ଫେରନ୍ତୁ false;
    }
    ଯଦି (isArchCap(cap_a) && isArchCap(cap_b)) {
        ଫେରନ୍ତୁ Arch_sameObjectAs(cap_a, cap_b);
    }
    ଫେରନ୍ତୁ sameRegionAs(cap_a, cap_b);
}

cap_t CONST updateCapData(bool_t preserve, word_t newData, cap_t cap)
{
    ଯଦି (isArchCap(cap)) {
        ଫେରନ୍ତୁ Arch_updateCapData(preserve, newData, cap);
    }

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_endpoint_cap:
        ଯଦି (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            ଫେରନ୍ତୁ cap_endpoint_cap_set_capEPBadge(cap, newData);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ cap_null_cap_new();
        }

    ସ୍ଥିତି cap_notification_cap:
        ଯଦି (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            ଫେରନ୍ତୁ cap_notification_cap_set_capNtfnBadge(cap, newData);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ cap_null_cap_new();
        }

    ସ୍ଥିତି cap_cnode_cap: {
        word_t guard, guardSize;
        seL4_CNode_CapData_t w = { .ଶବ୍ଦଗୁଡ଼ିକ = { newData } };

        guardSize = seL4_CNode_CapData_get_guardSize(w);

        ଯଦି (guardSize + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            ଫେରନ୍ତୁ cap_null_cap_new();
        } ଅନ୍ୟଥା {
            cap_t new_cap;

            guard = seL4_CNode_CapData_get_guard(w) & MASK(guardSize);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_capCNodeGuardSize(new_cap,
                                                          guardSize);

            ଫେରନ୍ତୁ new_cap;
        }
    }

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    ଯଦି (isArchCap(cap)) {
        ଫେରନ୍ତୁ Arch_maskCapRights(cap_rights, cap);
    }

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_null_cap:
    ସ୍ଥିତି cap_domain_cap:
    ସ୍ଥିତି cap_cnode_cap:
    ସ୍ଥିତି cap_untyped_cap:
    ସ୍ଥିତି cap_irq_control_cap:
    ସ୍ଥିତି cap_irq_handler_cap:
    ସ୍ଥିତି cap_zombie_cap:
    ସ୍ଥିତି cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_sched_context_cap:
    ସ୍ଥିତି cap_sched_control_cap:
#endif
        ଫେରନ୍ତୁ cap;

    ସ୍ଥିତି cap_endpoint_cap: {
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

        ଫେରନ୍ତୁ new_cap;
    }

    ସ୍ଥିତି cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        ଫେରନ୍ତୁ new_cap;
    }
    ସ୍ଥିତି cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        ଫେରନ୍ତୁ new_cap;
    }


    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, ଶୂନ୍ୟ *regionBase, word_t userSize, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    ଯଦି (t >= (object_t) se_L4_Non_Arch_Object_Type_ଗଣନା) {
        ଫେରନ୍ତୁ Arch_createObject(t, regionBase, userSize, deviceMemory);
    }

    /* Create objects. */
    ବାଛ ((api_object_t)t) {
    ସ୍ଥିତି seL4_TCBObject: {
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
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(ଗଣ୍ଠି_STATE(ksCurThread))->tcbName, TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_LENGTH);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        ଫେରନ୍ତୁ cap_thread_cap_new(TCB_REF(tcb));
    }

    ସ୍ଥିତି seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        ଫେରନ୍ତୁ cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    ସ୍ଥିତି seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        ଫେରନ୍ତୁ cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    ସ୍ଥିତି seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        ଫେରନ୍ତୁ cap_cnode_cap_new(userSize, 0, 0, CTE_REF(regionBase));

    ସ୍ଥିତି seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        ଫେରନ୍ତୁ cap_untyped_cap_new(0, !!deviceMemory, userSize, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        ଫେରନ୍ତୁ cap_sched_context_cap_new(SC_REF(regionBase), userSize);

    ସ୍ଥିତି seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        ଫେରନ୍ତୁ cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid object type");
    }
}

ଶୂନ୍ୟ createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t destLength,
                      ଶୂନ୍ୟ *regionBase, word_t userSize, bool_t deviceMemory)
{
    word_t objectSize;
    ଶୂନ୍ୟ *nextFreeArea;
    word_t i;
    word_t totalObjectSize UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    objectSize = getObjectSize(t, userSize);
    totalObjectSize = destLength << objectSize;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    nextFreeArea = regionBase;
    ପାଇଁ (i = 0; i < destLength; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (ଶୂନ୍ୟ *)((word_t)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        /* Insert the cap into the user's cspace. */
        ଭର୍ତ୍ତି_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *buffer)
#ଅନ୍ୟଥା
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *buffer)
#endif
{
    ଯଦି (isArchCap(cap)) {
        ଫେରନ୍ତୁ Arch_decodeInvocation(invLabel, length, capIndex,
                                     slot, cap, call, buffer);
    }

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;

    ସ୍ଥିତି cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;

    ସ୍ଥିତି cap_endpoint_cap:
        ଯଦି (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        ଫେରନ୍ତୁ performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    ସ୍ଥିତି cap_notification_cap: {
        ଯଦି (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_reply_cap:
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ performInvocation_Reply(
                   ଗଣ୍ଠି_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#ଅନ୍ୟଥା
    ସ୍ଥିତି cap_reply_cap:
        ଯଦି (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    ସ୍ଥିତି cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        ଯଦି (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ଫେରନ୍ତୁ decodeTCBInvocation(invLabel, length, cap, slot, call, buffer);

    ସ୍ଥିତି cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        ଯଦି (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ଫେରନ୍ତୁ decodeDomainInvocation(invLabel, length, buffer);

    ସ୍ଥିତି cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        ଯଦି (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ଫେରନ୍ତୁ decodeCNodeInvocation(invLabel, length, cap, buffer);

    ସ୍ଥିତି cap_untyped_cap:
        ଫେରନ୍ତୁ decodeUntypedInvocation(invLabel, length, slot, cap, call, buffer);

    ସ୍ଥିତି cap_irq_control_cap:
        ଫେରନ୍ତୁ decodeIRQControlInvocation(invLabel, length, slot, buffer);

    ସ୍ଥିତି cap_irq_handler_cap:
        ଫେରନ୍ତୁ decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_sched_control_cap:
        ଯଦି (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ decodeSchedControlInvocation(invLabel, cap, length, buffer);

    ସ୍ଥିତି cap_sched_context_cap:
        ଯଦି (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ସଂଖ୍ୟା = 0;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        ଫେରନ୍ତୁ decodeSchedContextInvocation(invLabel, sc, call);
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, ଗଣ୍ଠି_STATE(ksCurThread), ep);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
#ଅନ୍ୟଥା
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, ଗଣ୍ଠି_STATE(ksCurThread), ep);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
#ଅନ୍ୟଥା
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(ଗଣ୍ଠି_STATE(ksCurThread), thread, slot, canGrant);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_capSizeBits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    ବାଛ (ctag) {
    ସ୍ଥିତି cap_untyped_cap:
        ଫେରନ୍ତୁ cap_untyped_cap_get_capBlockSize(cap);

    ସ୍ଥିତି cap_endpoint_cap:
        ଫେରନ୍ତୁ seL4_EndpointBits;

    ସ୍ଥିତି cap_notification_cap:
        ଫେରନ୍ତୁ seL4_NotificationBits;

    ସ୍ଥିତି cap_cnode_cap:
        ଫେରନ୍ତୁ cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    ସ୍ଥିତି cap_thread_cap:
        ଫେରନ୍ତୁ seL4_TCBBits;

    ସ୍ଥିତି cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        ଯଦି (type == ZombieType_ZombieTCB) {
            ଫେରନ୍ତୁ seL4_TCBBits;
        }
        ଫେରନ୍ତୁ ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    ସ୍ଥିତି cap_null_cap:
        ଫେରନ୍ତୁ 0;

    ସ୍ଥିତି cap_domain_cap:
        ଫେରନ୍ତୁ 0;

    ସ୍ଥିତି cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ଫେରନ୍ତୁ seL4_ReplyBits;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ 0;
#endif

    ସ୍ଥିତି cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_sched_control_cap:
#endif
        ଫେରନ୍ତୁ 0;

    ସ୍ଥିତି cap_irq_handler_cap:
        ଫେରନ୍ତୁ 0;

#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_sched_context_cap:
        ଫେରନ୍ତୁ cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ cap_get_archCapSizeBits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    ବାଛ (ctag) {
    ସ୍ଥିତି cap_untyped_cap:
        ଫେରନ୍ତୁ true;

    ସ୍ଥିତି cap_endpoint_cap:
        ଫେରନ୍ତୁ true;

    ସ୍ଥିତି cap_notification_cap:
        ଫେରନ୍ତୁ true;

    ସ୍ଥିତି cap_cnode_cap:
        ଫେରନ୍ତୁ true;

    ସ୍ଥିତି cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_sched_context_cap:
#endif
        ଫେରନ୍ତୁ true;

    ସ୍ଥିତି cap_zombie_cap:
        ଫେରନ୍ତୁ true;

    ସ୍ଥିତି cap_domain_cap:
        ଫେରନ୍ତୁ false;

    ସ୍ଥିତି cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ଫେରନ୍ତୁ true;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ false;
#endif

    ସ୍ଥିତି cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_sched_control_cap:
#endif
        ଫେରନ୍ତୁ false;

    ସ୍ଥିତି cap_irq_handler_cap:
        ଫେରନ୍ତୁ false;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ cap_get_archCapIsPhysical(cap);
    }
}

ଶୂନ୍ୟ *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    ବାଛ (ctag) {
    ସ୍ଥିତି cap_untyped_cap:
        ଫେରନ୍ତୁ WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    ସ୍ଥିତି cap_endpoint_cap:
        ଫେରନ୍ତୁ EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    ସ୍ଥିତି cap_notification_cap:
        ଫେରନ୍ତୁ NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    ସ୍ଥିତି cap_cnode_cap:
        ଫେରନ୍ତୁ CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    ସ୍ଥିତି cap_thread_cap:
        ଫେରନ୍ତୁ TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    ସ୍ଥିତି cap_zombie_cap:
        ଫେରନ୍ତୁ CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    ସ୍ଥିତି cap_domain_cap:
        ଫେରନ୍ତୁ ଖାଲି;

    ସ୍ଥିତି cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ଫେରନ୍ତୁ REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ ଖାଲି;
#endif

    ସ୍ଥିତି cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_sched_control_cap:
#endif
        ଫେରନ୍ତୁ ଖାଲି;

    ସ୍ଥିତି cap_irq_handler_cap:
        ଫେରନ୍ତୁ ଖାଲି;

#ifdef CONFIG_KERNEL_MCS
    ସ୍ଥିତି cap_sched_context_cap:
        ଫେରନ୍ତୁ SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    ଯଦି (isArchCap(derivedCap)) {
        ଫେରନ୍ତୁ Arch_isCapRevocable(derivedCap, srcCap);
    }
    ବାଛ (cap_get_capType(derivedCap)) {
    ସ୍ଥିତି cap_endpoint_cap:
        ଫେରନ୍ତୁ (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    ସ୍ଥିତି cap_notification_cap:
        ଫେରନ୍ତୁ (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    ସ୍ଥିତି cap_irq_handler_cap:
        ଫେରନ୍ତୁ (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    ସ୍ଥିତି cap_untyped_cap:
        ଫେରନ୍ତୁ true;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ false;
    }
}
