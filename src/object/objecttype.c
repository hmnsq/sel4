/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <arch/object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/notification.h>
#ಅನ್ತರ್ಭಾವ <object/endpoint.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <object/interrupt.h>
#ifdef CONFIG_KERNEL_MCS
#ಅನ್ತರ್ಭಾವ <object/schedcontext.h>
#ಅನ್ತರ್ಭಾವ <object/schedcontrol.h>
#endif
#ಅನ್ತರ್ಭಾವ <object/tcb.h>
#ಅನ್ತರ್ಭಾವ <object/untyped.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <machine.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <string.h>

word_t getObjectSize(word_t t, word_t userObjSize)
{
    ಯದಿ (t >= se_L4_Non_Arch_Object_Type_ಎಣಿಕೆ) {
        ಪ್ರತ್ಯಾಗಮ Arch_getObjectSize(t);
    } ಅನ್ಯಥಾ {
        ವಿಕಲ್ಪ (t) {
        ಪಕ್ಷ seL4_TCBObject:
            ಪ್ರತ್ಯಾಗಮ seL4_TCBBits;
        ಪಕ್ಷ seL4_EndpointObject:
            ಪ್ರತ್ಯಾಗಮ seL4_EndpointBits;
        ಪಕ್ಷ seL4_NotificationObject:
            ಪ್ರತ್ಯಾಗಮ seL4_NotificationBits;
        ಪಕ್ಷ seL4_CapTableObject:
            ಪ್ರತ್ಯಾಗಮ seL4_SlotBits + userObjSize;
        ಪಕ್ಷ seL4_UntypedObject:
            ಪ್ರತ್ಯಾಗಮ userObjSize;
#ifdef CONFIG_KERNEL_MCS
        ಪಕ್ಷ seL4_SchedContextObject:
            ಪ್ರತ್ಯಾಗಮ userObjSize;
        ಪಕ್ಷ seL4_ReplyObject:
            ಪ್ರತ್ಯಾಗಮ seL4_ReplyBits;
#endif
        ಸ್ವಭಾವ:
            fail("Invalid object type");
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ಯದಿ (isArchCap(cap)) {
        ಪ್ರತ್ಯಾಗಮ Arch_deriveCap(slot, cap);
    }

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        ಯದಿ (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } ಅನ್ಯಥಾ {
            ret.cap = cap;
        }
        ಭಙ್ಗ;

#ifndef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ಭಙ್ಗ;
#endif
    ಸ್ವಭಾವ:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    ಯದಿ (isArchCap(cap)) {
        ಪ್ರತ್ಯಾಗಮ Arch_finaliseCap(cap, final);
    }

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_endpoint_cap:
        ಯದಿ (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ಪ್ರತ್ಯಾಗಮ fc_ret;

    ಪಕ್ಷ cap_notification_cap:
        ಯದಿ (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ifdef CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#endif
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ಪ್ರತ್ಯಾಗಮ fc_ret;

    ಪಕ್ಷ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ಯದಿ (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            ಯದಿ (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                ವಿಕಲ್ಪ (thread_state_get_tsType(tcb->tcbState)) {
                ಪಕ್ಷ ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    ಭಙ್ಗ;
                ಪಕ್ಷ ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    ಭಙ್ಗ;
                ಸ್ವಭಾವ:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ಪ್ರತ್ಯಾಗಮ fc_ret;
#endif
    ಪಕ್ಷ cap_null_cap:
    ಪಕ್ಷ cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        ಪ್ರತ್ಯಾಗಮ fc_ret;
    }

    ಯದಿ (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_cnode_cap: {
        ಯದಿ (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            ಪ್ರತ್ಯಾಗಮ fc_ret;
        }
        ಭಙ್ಗ;
    }

    ಪಕ್ಷ cap_thread_cap: {
        ಯದಿ (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ifdef CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            ಯದಿ (sc) {
                schedContext_unbindTCB(sc);
                ಯದಿ (sc->scYieldFrom) {
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
            ಪ್ರತ್ಯಾಗಮ fc_ret;
        }
        ಭಙ್ಗ;
    }

#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_sched_context_cap:
        ಯದಿ (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            ಯದಿ (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            ಪ್ರತ್ಯಾಗಮ fc_ret;
        }
        ಭಙ್ಗ;
#endif

    ಪಕ್ಷ cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        ಪ್ರತ್ಯಾಗಮ fc_ret;

    ಪಕ್ಷ cap_irq_handler_cap:
        ಯದಿ (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ಪ್ರತ್ಯಾಗಮ fc_ret;
        }
        ಭಙ್ಗ;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಪ್ರತ್ಯಾಗಮ fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_endpoint_cap:
        ಪ್ರತ್ಯಾಗಮ cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap_a)) {
    ಪಕ್ಷ cap_untyped_cap:
        ಯದಿ (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, a_ತುದಿ, b_ತುದಿ;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_ತುದಿ = aBase + MASK(cap_untyped_cap_get_capBlockSize(cap_a));
            b_ತುದಿ = bBase + MASK(cap_get_capSizeBits(cap_b));

            ಪ್ರತ್ಯಾಗಮ (aBase <= bBase) && (b_ತುದಿ <= a_ತುದಿ) && (bBase <= b_ತುದಿ);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_endpoint_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_endpoint_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_notification_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_notification_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_cnode_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_cnode_cap) {
            ಪ್ರತ್ಯಾಗಮ (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_thread_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_thread_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_reply_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            ಪ್ರತ್ಯಾಗಮ cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#ಅನ್ಯಥಾ
            ಪ್ರತ್ಯಾಗಮ cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_domain_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_domain_cap) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_irq_control_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_irq_handler_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            ಪ್ರತ್ಯಾಗಮ (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        ಭಙ್ಗ;

#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_sched_context_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_sched_context_cap) {
            ಪ್ರತ್ಯಾಗಮ (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_sched_control_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_sched_control_cap) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಭಙ್ಗ;
#endif
    ಸ್ವಭಾವ:
        ಯದಿ (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            ಪ್ರತ್ಯಾಗಮ Arch_sameRegionAs(cap_a, cap_b);
        }
        ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ಯದಿ (cap_get_capType(cap_a) == cap_untyped_cap) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಯದಿ (cap_get_capType(cap_a) == cap_irq_control_cap) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಯದಿ (isArchCap(cap_a) && isArchCap(cap_b)) {
        ಪ್ರತ್ಯಾಗಮ Arch_sameObjectAs(cap_a, cap_b);
    }
    ಪ್ರತ್ಯಾಗಮ sameRegionAs(cap_a, cap_b);
}

cap_t CONST updateCapData(bool_t preserve, word_t newData, cap_t cap)
{
    ಯದಿ (isArchCap(cap)) {
        ಪ್ರತ್ಯಾಗಮ Arch_updateCapData(preserve, newData, cap);
    }

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_endpoint_cap:
        ಯದಿ (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            ಪ್ರತ್ಯಾಗಮ cap_endpoint_cap_set_capEPBadge(cap, newData);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ cap_null_cap_new();
        }

    ಪಕ್ಷ cap_notification_cap:
        ಯದಿ (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            ಪ್ರತ್ಯಾಗಮ cap_notification_cap_set_capNtfnBadge(cap, newData);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ cap_null_cap_new();
        }

    ಪಕ್ಷ cap_cnode_cap: {
        word_t guard, guardSize;
        seL4_CNode_CapData_t w = { .ಪದಗಳು = { newData } };

        guardSize = seL4_CNode_CapData_get_guardSize(w);

        ಯದಿ (guardSize + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            ಪ್ರತ್ಯಾಗಮ cap_null_cap_new();
        } ಅನ್ಯಥಾ {
            cap_t new_cap;

            guard = seL4_CNode_CapData_get_guard(w) & MASK(guardSize);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_capCNodeGuardSize(new_cap,
                                                          guardSize);

            ಪ್ರತ್ಯಾಗಮ new_cap;
        }
    }

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    ಯದಿ (isArchCap(cap)) {
        ಪ್ರತ್ಯಾಗಮ Arch_maskCapRights(cap_rights, cap);
    }

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_null_cap:
    ಪಕ್ಷ cap_domain_cap:
    ಪಕ್ಷ cap_cnode_cap:
    ಪಕ್ಷ cap_untyped_cap:
    ಪಕ್ಷ cap_irq_control_cap:
    ಪಕ್ಷ cap_irq_handler_cap:
    ಪಕ್ಷ cap_zombie_cap:
    ಪಕ್ಷ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_sched_context_cap:
    ಪಕ್ಷ cap_sched_control_cap:
#endif
        ಪ್ರತ್ಯಾಗಮ cap;

    ಪಕ್ಷ cap_endpoint_cap: {
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

        ಪ್ರತ್ಯಾಗಮ new_cap;
    }

    ಪಕ್ಷ cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        ಪ್ರತ್ಯಾಗಮ new_cap;
    }
    ಪಕ್ಷ cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        ಪ್ರತ್ಯಾಗಮ new_cap;
    }


    ಸ್ವಭಾವ:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, ಶೂನ್ಯ *regionBase, word_t userSize, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    ಯದಿ (t >= (object_t) se_L4_Non_Arch_Object_Type_ಎಣಿಕೆ) {
        ಪ್ರತ್ಯಾಗಮ Arch_createObject(t, regionBase, userSize, deviceMemory);
    }

    /* Create objects. */
    ವಿಕಲ್ಪ ((api_object_t)t) {
    ಪಕ್ಷ seL4_TCBObject: {
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
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(ಗಂಟು_STATE(ksCurThread))->tcbName, TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_LENGTH);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        ಪ್ರತ್ಯಾಗಮ cap_thread_cap_new(TCB_REF(tcb));
    }

    ಪಕ್ಷ seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        ಪ್ರತ್ಯಾಗಮ cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    ಪಕ್ಷ seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        ಪ್ರತ್ಯಾಗಮ cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    ಪಕ್ಷ seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        ಪ್ರತ್ಯಾಗಮ cap_cnode_cap_new(userSize, 0, 0, CTE_REF(regionBase));

    ಪಕ್ಷ seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        ಪ್ರತ್ಯಾಗಮ cap_untyped_cap_new(0, !!deviceMemory, userSize, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        ಪ್ರತ್ಯಾಗಮ cap_sched_context_cap_new(SC_REF(regionBase), userSize);

    ಪಕ್ಷ seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        ಪ್ರತ್ಯಾಗಮ cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    ಸ್ವಭಾವ:
        fail("Invalid object type");
    }
}

ಶೂನ್ಯ createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t destLength,
                      ಶೂನ್ಯ *regionBase, word_t userSize, bool_t deviceMemory)
{
    word_t objectSize;
    ಶೂನ್ಯ *nextFreeArea;
    word_t i;
    word_t totalObjectSize UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    objectSize = getObjectSize(t, userSize);
    totalObjectSize = destLength << objectSize;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    nextFreeArea = regionBase;
    ಪ್ರತಿ (i = 0; i < destLength; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (ಶೂನ್ಯ *)((word_t)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        /* Insert the cap into the user's cspace. */
        ಸೇರಿಸು_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *buffer)
#ಅನ್ಯಥಾ
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *buffer)
#endif
{
    ಯದಿ (isArchCap(cap)) {
        ಪ್ರತ್ಯಾಗಮ Arch_decodeInvocation(invLabel, length, capIndex,
                                     slot, cap, call, buffer);
    }

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;

    ಪಕ್ಷ cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;

    ಪಕ್ಷ cap_endpoint_cap:
        ಯದಿ (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        ಪ್ರತ್ಯಾಗಮ performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    ಪಕ್ಷ cap_notification_cap: {
        ಯದಿ (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_reply_cap:
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performInvocation_Reply(
                   ಗಂಟು_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#ಅನ್ಯಥಾ
    ಪಕ್ಷ cap_reply_cap:
        ಯದಿ (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    ಪಕ್ಷ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        ಯದಿ (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ಪ್ರತ್ಯಾಗಮ decodeTCBInvocation(invLabel, length, cap, slot, call, buffer);

    ಪಕ್ಷ cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        ಯದಿ (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ಪ್ರತ್ಯಾಗಮ decodeDomainInvocation(invLabel, length, buffer);

    ಪಕ್ಷ cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        ಯದಿ (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        ಪ್ರತ್ಯಾಗಮ decodeCNodeInvocation(invLabel, length, cap, buffer);

    ಪಕ್ಷ cap_untyped_cap:
        ಪ್ರತ್ಯಾಗಮ decodeUntypedInvocation(invLabel, length, slot, cap, call, buffer);

    ಪಕ್ಷ cap_irq_control_cap:
        ಪ್ರತ್ಯಾಗಮ decodeIRQControlInvocation(invLabel, length, slot, buffer);

    ಪಕ್ಷ cap_irq_handler_cap:
        ಪ್ರತ್ಯಾಗಮ decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_sched_control_cap:
        ಯದಿ (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ decodeSchedControlInvocation(invLabel, cap, length, buffer);

    ಪಕ್ಷ cap_sched_context_cap:
        ಯದಿ (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        ಪ್ರತ್ಯಾಗಮ decodeSchedContextInvocation(invLabel, sc, call);
#endif
    ಸ್ವಭಾವ:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, ಗಂಟು_STATE(ksCurThread), ep);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#ಅನ್ಯಥಾ
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, ಗಂಟು_STATE(ksCurThread), ep);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#ಅನ್ಯಥಾ
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(ಗಂಟು_STATE(ksCurThread), thread, slot, canGrant);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_capSizeBits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    ವಿಕಲ್ಪ (ctag) {
    ಪಕ್ಷ cap_untyped_cap:
        ಪ್ರತ್ಯಾಗಮ cap_untyped_cap_get_capBlockSize(cap);

    ಪಕ್ಷ cap_endpoint_cap:
        ಪ್ರತ್ಯಾಗಮ seL4_EndpointBits;

    ಪಕ್ಷ cap_notification_cap:
        ಪ್ರತ್ಯಾಗಮ seL4_NotificationBits;

    ಪಕ್ಷ cap_cnode_cap:
        ಪ್ರತ್ಯಾಗಮ cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    ಪಕ್ಷ cap_thread_cap:
        ಪ್ರತ್ಯಾಗಮ seL4_TCBBits;

    ಪಕ್ಷ cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        ಯದಿ (type == ZombieType_ZombieTCB) {
            ಪ್ರತ್ಯಾಗಮ seL4_TCBBits;
        }
        ಪ್ರತ್ಯಾಗಮ ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    ಪಕ್ಷ cap_null_cap:
        ಪ್ರತ್ಯಾಗಮ 0;

    ಪಕ್ಷ cap_domain_cap:
        ಪ್ರತ್ಯಾಗಮ 0;

    ಪಕ್ಷ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ಪ್ರತ್ಯಾಗಮ seL4_ReplyBits;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ 0;
#endif

    ಪಕ್ಷ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_sched_control_cap:
#endif
        ಪ್ರತ್ಯಾಗಮ 0;

    ಪಕ್ಷ cap_irq_handler_cap:
        ಪ್ರತ್ಯಾಗಮ 0;

#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_sched_context_cap:
        ಪ್ರತ್ಯಾಗಮ cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ cap_get_archCapSizeBits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    ವಿಕಲ್ಪ (ctag) {
    ಪಕ್ಷ cap_untyped_cap:
        ಪ್ರತ್ಯಾಗಮ true;

    ಪಕ್ಷ cap_endpoint_cap:
        ಪ್ರತ್ಯಾಗಮ true;

    ಪಕ್ಷ cap_notification_cap:
        ಪ್ರತ್ಯಾಗಮ true;

    ಪಕ್ಷ cap_cnode_cap:
        ಪ್ರತ್ಯಾಗಮ true;

    ಪಕ್ಷ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_sched_context_cap:
#endif
        ಪ್ರತ್ಯಾಗಮ true;

    ಪಕ್ಷ cap_zombie_cap:
        ಪ್ರತ್ಯಾಗಮ true;

    ಪಕ್ಷ cap_domain_cap:
        ಪ್ರತ್ಯಾಗಮ false;

    ಪಕ್ಷ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ಪ್ರತ್ಯಾಗಮ true;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ false;
#endif

    ಪಕ್ಷ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_sched_control_cap:
#endif
        ಪ್ರತ್ಯಾಗಮ false;

    ಪಕ್ಷ cap_irq_handler_cap:
        ಪ್ರತ್ಯಾಗಮ false;

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ cap_get_archCapIsPhysical(cap);
    }
}

ಶೂನ್ಯ *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    ವಿಕಲ್ಪ (ctag) {
    ಪಕ್ಷ cap_untyped_cap:
        ಪ್ರತ್ಯಾಗಮ WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    ಪಕ್ಷ cap_endpoint_cap:
        ಪ್ರತ್ಯಾಗಮ EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    ಪಕ್ಷ cap_notification_cap:
        ಪ್ರತ್ಯಾಗಮ NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    ಪಕ್ಷ cap_cnode_cap:
        ಪ್ರತ್ಯಾಗಮ CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    ಪಕ್ಷ cap_thread_cap:
        ಪ್ರತ್ಯಾಗಮ TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    ಪಕ್ಷ cap_zombie_cap:
        ಪ್ರತ್ಯಾಗಮ CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    ಪಕ್ಷ cap_domain_cap:
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪಕ್ಷ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        ಪ್ರತ್ಯಾಗಮ REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif

    ಪಕ್ಷ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_sched_control_cap:
#endif
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪಕ್ಷ cap_irq_handler_cap:
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_sched_context_cap:
        ಪ್ರತ್ಯಾಗಮ SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    ಯದಿ (isArchCap(derivedCap)) {
        ಪ್ರತ್ಯಾಗಮ Arch_isCapRevocable(derivedCap, srcCap);
    }
    ವಿಕಲ್ಪ (cap_get_capType(derivedCap)) {
    ಪಕ್ಷ cap_endpoint_cap:
        ಪ್ರತ್ಯಾಗಮ (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    ಪಕ್ಷ cap_notification_cap:
        ಪ್ರತ್ಯಾಗಮ (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    ಪಕ್ಷ cap_irq_handler_cap:
        ಪ್ರತ್ಯಾಗಮ (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    ಪಕ್ಷ cap_untyped_cap:
        ಪ್ರತ್ಯಾಗಮ true;

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ false;
    }
}
