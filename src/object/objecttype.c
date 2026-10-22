/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <arch/object/objecttype.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <object/objecttype.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <object/notification.h>
#അന്തര്ഭാവ <object/endpoint.h>
#അന്തര്ഭാവ <object/cnode.h>
#അന്തര്ഭാവ <object/interrupt.h>
#ifdef CONFIG_KERNEL_MCS
#അന്തര്ഭാവ <object/schedcontext.h>
#അന്തര്ഭാവ <object/schedcontrol.h>
#endif
#അന്തര്ഭാവ <object/tcb.h>
#അന്തര്ഭാവ <object/untyped.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <machine.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <string.h>

word_t getObjectSize(word_t t, word_t userObjSize)
{
    എങ്കിൽ (t >= se_L4_Non_Arch_Object_Type_എണ്ണം) {
        തിരികെ Arch_getObjectSize(t);
    } അല്ലെങ്കിൽ {
        തിരഞ്ഞെടുപ്പ് (t) {
        അവസ്ഥ seL4_TCBObject:
            തിരികെ seL4_TCBBits;
        അവസ്ഥ seL4_EndpointObject:
            തിരികെ seL4_EndpointBits;
        അവസ്ഥ seL4_NotificationObject:
            തിരികെ seL4_NotificationBits;
        അവസ്ഥ seL4_CapTableObject:
            തിരികെ seL4_SlotBits + userObjSize;
        അവസ്ഥ seL4_UntypedObject:
            തിരികെ userObjSize;
#ifdef CONFIG_KERNEL_MCS
        അവസ്ഥ seL4_SchedContextObject:
            തിരികെ userObjSize;
        അവസ്ഥ seL4_ReplyObject:
            തിരികെ seL4_ReplyBits;
#endif
        സ്ഥിരസ്ഥിതി:
            fail("Invalid object type");
            തിരികെ 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    എങ്കിൽ (isArchCap(cap)) {
        തിരികെ Arch_deriveCap(slot, cap);
    }

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_zombie_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        നിർത്തുക;

    അവസ്ഥ cap_irq_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        നിർത്തുക;

    അവസ്ഥ cap_untyped_cap:
        ret.status = ensureNoChildren(slot);
        എങ്കിൽ (ret.status != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } അല്ലെങ്കിൽ {
            ret.cap = cap;
        }
        നിർത്തുക;

#ifndef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_reply_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        നിർത്തുക;
#endif
    സ്ഥിരസ്ഥിതി:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap;
    }

    തിരികെ ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    എങ്കിൽ (isArchCap(cap)) {
        തിരികെ Arch_finaliseCap(cap, final);
    }

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_endpoint_cap:
        എങ്കിൽ (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        തിരികെ fc_ret;

    അവസ്ഥ cap_notification_cap:
        എങ്കിൽ (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#ifdef CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#endif
            unbindMaybeNotification(ntfn);
            cancelAllSignals(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        തിരികെ fc_ret;

    അവസ്ഥ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        എങ്കിൽ (final) {
            reply_t *reply = REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
            എങ്കിൽ (reply && reply->replyTCB) {
                tcb_t *tcb = reply->replyTCB;
                തിരഞ്ഞെടുപ്പ് (thread_state_get_tsType(tcb->tcbState)) {
                അവസ്ഥ ThreadState_BlockedOnReply:
                    reply_remove(reply, tcb);
                    നിർത്തുക;
                അവസ്ഥ ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    നിർത്തുക;
                സ്ഥിരസ്ഥിതി:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        തിരികെ fc_ret;
#endif
    അവസ്ഥ cap_null_cap:
    അവസ്ഥ cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        തിരികെ fc_ret;
    }

    എങ്കിൽ (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_cnode_cap: {
        എങ്കിൽ (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            തിരികെ fc_ret;
        }
        നിർത്തുക;
    }

    അവസ്ഥ cap_thread_cap: {
        എങ്കിൽ (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#ifdef CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            എങ്കിൽ (sc) {
                schedContext_unbindTCB(sc);
                എങ്കിൽ (sc->scYieldFrom) {
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
            തിരികെ fc_ret;
        }
        നിർത്തുക;
    }

#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_sched_context_cap:
        എങ്കിൽ (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            schedContext_unbindReply(sc);
            എങ്കിൽ (sc->scYieldFrom) {
                schedContext_completeYieldTo(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            തിരികെ fc_ret;
        }
        നിർത്തുക;
#endif

    അവസ്ഥ cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        തിരികെ fc_ret;

    അവസ്ഥ cap_irq_handler_cap:
        എങ്കിൽ (final) {
            irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            തിരികെ fc_ret;
        }
        നിർത്തുക;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    തിരികെ fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_endpoint_cap:
        തിരികെ cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_capCanGrantReply(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    സ്ഥിരസ്ഥിതി:
        തിരികെ false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap_a)) {
    അവസ്ഥ cap_untyped_cap:
        എങ്കിൽ (cap_get_capIsPhysical(cap_b)) {
            word_t aBase, bBase, a_മുകൾ, b_മുകൾ;

            aBase = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            bBase = (word_t)cap_get_capPtr(cap_b);

            a_മുകൾ = aBase + MASK(cap_untyped_cap_get_capBlockSize(cap_a));
            b_മുകൾ = bBase + MASK(cap_get_capSizeBits(cap_b));

            തിരികെ (aBase <= bBase) && (b_മുകൾ <= a_മുകൾ) && (bBase <= b_മുകൾ);
        }
        നിർത്തുക;

    അവസ്ഥ cap_endpoint_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_endpoint_cap) {
            തിരികെ cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_notification_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_notification_cap) {
            തിരികെ cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_cnode_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_cnode_cap) {
            തിരികെ (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        നിർത്തുക;

    അവസ്ഥ cap_thread_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_thread_cap) {
            തിരികെ cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_reply_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_reply_cap) {
#ifdef CONFIG_KERNEL_MCS
            തിരികെ cap_reply_cap_get_capReplyPtr(cap_a) ==
                   cap_reply_cap_get_capReplyPtr(cap_b);
#അല്ലെങ്കിൽ
            തിരികെ cap_reply_cap_get_capTCBPtr(cap_a) ==
                   cap_reply_cap_get_capTCBPtr(cap_b);
#endif
        }
        നിർത്തുക;

    അവസ്ഥ cap_domain_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_domain_cap) {
            തിരികെ true;
        }
        നിർത്തുക;

    അവസ്ഥ cap_irq_control_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            തിരികെ true;
        }
        നിർത്തുക;

    അവസ്ഥ cap_irq_handler_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            തിരികെ (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        നിർത്തുക;

#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_sched_context_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_sched_context_cap) {
            തിരികെ (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_capSCSizeBits(cap_a) ==
                    cap_sched_context_cap_get_capSCSizeBits(cap_b));
        }
        നിർത്തുക;
    അവസ്ഥ cap_sched_control_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_sched_control_cap) {
            തിരികെ true;
        }
        നിർത്തുക;
#endif
    സ്ഥിരസ്ഥിതി:
        എങ്കിൽ (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            തിരികെ Arch_sameRegionAs(cap_a, cap_b);
        }
        നിർത്തുക;
    }

    തിരികെ false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    എങ്കിൽ (cap_get_capType(cap_a) == cap_untyped_cap) {
        തിരികെ false;
    }
    എങ്കിൽ (cap_get_capType(cap_a) == cap_irq_control_cap) {
        തിരികെ false;
    }
    എങ്കിൽ (isArchCap(cap_a) && isArchCap(cap_b)) {
        തിരികെ Arch_sameObjectAs(cap_a, cap_b);
    }
    തിരികെ sameRegionAs(cap_a, cap_b);
}

cap_t CONST updateCapData(bool_t preserve, word_t newData, cap_t cap)
{
    എങ്കിൽ (isArchCap(cap)) {
        തിരികെ Arch_updateCapData(preserve, newData, cap);
    }

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_endpoint_cap:
        എങ്കിൽ (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            തിരികെ cap_endpoint_cap_set_capEPBadge(cap, newData);
        } അല്ലെങ്കിൽ {
            തിരികെ cap_null_cap_new();
        }

    അവസ്ഥ cap_notification_cap:
        എങ്കിൽ (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            തിരികെ cap_notification_cap_set_capNtfnBadge(cap, newData);
        } അല്ലെങ്കിൽ {
            തിരികെ cap_null_cap_new();
        }

    അവസ്ഥ cap_cnode_cap: {
        word_t guard, guardSize;
        seL4_CNode_CapData_t w = { .വാക്കുകൾ = { newData } };

        guardSize = seL4_CNode_CapData_get_guardSize(w);

        എങ്കിൽ (guardSize + cap_cnode_cap_get_capCNodeRadix(cap) > wordBits) {
            തിരികെ cap_null_cap_new();
        } അല്ലെങ്കിൽ {
            cap_t new_cap;

            guard = seL4_CNode_CapData_get_guard(w) & MASK(guardSize);
            new_cap = cap_cnode_cap_set_capCNodeGuard(cap, guard);
            new_cap = cap_cnode_cap_set_capCNodeGuardSize(new_cap,
                                                          guardSize);

            തിരികെ new_cap;
        }
    }

    സ്ഥിരസ്ഥിതി:
        തിരികെ cap;
    }
}

cap_t CONST maskCapRights(seL4_CapRights_t cap_rights, cap_t cap)
{
    എങ്കിൽ (isArchCap(cap)) {
        തിരികെ Arch_maskCapRights(cap_rights, cap);
    }

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_null_cap:
    അവസ്ഥ cap_domain_cap:
    അവസ്ഥ cap_cnode_cap:
    അവസ്ഥ cap_untyped_cap:
    അവസ്ഥ cap_irq_control_cap:
    അവസ്ഥ cap_irq_handler_cap:
    അവസ്ഥ cap_zombie_cap:
    അവസ്ഥ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_sched_context_cap:
    അവസ്ഥ cap_sched_control_cap:
#endif
        തിരികെ cap;

    അവസ്ഥ cap_endpoint_cap: {
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

        തിരികെ new_cap;
    }

    അവസ്ഥ cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_set_capNtfnCanSend(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_set_capNtfnCanReceive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        തിരികെ new_cap;
    }
    അവസ്ഥ cap_reply_cap: {
        cap_t new_cap;

        new_cap = cap_reply_cap_set_capReplyCanGrant(
                      cap, cap_reply_cap_get_capReplyCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        തിരികെ new_cap;
    }


    സ്ഥിരസ്ഥിതി:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, ശൂന്യം *regionBase, word_t userSize, bool_t deviceMemory)
{
    /* Handle architecture-specific objects. */
    എങ്കിൽ (t >= (object_t) se_L4_Non_Arch_Object_Type_എണ്ണം) {
        തിരികെ Arch_createObject(t, regionBase, userSize, deviceMemory);
    }

    /* Create objects. */
    തിരഞ്ഞെടുപ്പ് ((api_object_t)t) {
    അവസ്ഥ seL4_TCBObject: {
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
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(കണ്ണി_STATE(ksCurThread))->tcbName, TCB_NAME_LENGTH);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_LENGTH);
        tcbDebugAppend(tcb);
#endif /* CONFIG_DEBUG_BUILD */

        തിരികെ cap_thread_cap_new(TCB_REF(tcb));
    }

    അവസ്ഥ seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        തിരികെ cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(regionBase));

    അവസ്ഥ seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        തിരികെ cap_notification_cap_new(0, true, true,
                                        NTFN_REF(regionBase));

    അവസ്ഥ seL4_CapTableObject:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        തിരികെ cap_cnode_cap_new(userSize, 0, 0, CTE_REF(regionBase));

    അവസ്ഥ seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        തിരികെ cap_untyped_cap_new(0, !!deviceMemory, userSize, WORD_REF(regionBase));

#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        തിരികെ cap_sched_context_cap_new(SC_REF(regionBase), userSize);

    അവസ്ഥ seL4_ReplyObject:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        തിരികെ cap_reply_cap_new(REPLY_REF(regionBase), true);
#endif

    സ്ഥിരസ്ഥിതി:
        fail("Invalid object type");
    }
}

ശൂന്യം createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t destOffset, word_t destLength,
                      ശൂന്യം *regionBase, word_t userSize, bool_t deviceMemory)
{
    word_t objectSize;
    ശൂന്യം *nextFreeArea;
    word_t i;
    word_t totalObjectSize UNUSED;

    /* ghost check that we're visiting less bytes than the max object size */
    objectSize = getObjectSize(t, userSize);
    totalObjectSize = destLength << objectSize;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    nextFreeArea = regionBase;
    വേണ്ടി (i = 0; i < destLength; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (ശൂന്യം *)((word_t)nextFreeArea + (i << objectSize)), userSize, deviceMemory);

        /* Insert the cap into the user's cspace. */
        തിരുകുക_New_Cap(parent, &destCNode[destOffset + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *buffer)
#അല്ലെങ്കിൽ
exception_t decodeInvocation(word_t invLabel, word_t length,
                             cptr_t capIndex, cte_t *slot, cap_t cap,
                             bool_t block, bool_t call,
                             word_t *buffer)
#endif
{
    എങ്കിൽ (isArchCap(cap)) {
        തിരികെ Arch_decodeInvocation(invLabel, length, capIndex,
                                     slot, cap, call, buffer);
    }

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_സംഖ്യ = 0;
        തിരികെ EXCEPTION_SYSCALL_ERROR;

    അവസ്ഥ cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", capIndex);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_സംഖ്യ = 0;
        തിരികെ EXCEPTION_SYSCALL_ERROR;

    അവസ്ഥ cap_endpoint_cap:
        എങ്കിൽ (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
#ifdef CONFIG_KERNEL_MCS
        തിരികെ performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call, canDonate);
#അല്ലെങ്കിൽ
        തിരികെ performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_capCanGrantReply(cap), block, call);
#endif

    അവസ്ഥ cap_notification_cap: {
        എങ്കിൽ (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_reply_cap:
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performInvocation_Reply(
                   കണ്ണി_STATE(ksCurThread),
                   REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap)),
                   cap_reply_cap_get_capReplyCanGrant(cap));
#അല്ലെങ്കിൽ
    അവസ്ഥ cap_reply_cap:
        എങ്കിൽ (unlikely(cap_reply_cap_get_capReplyMaster(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      capIndex);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performInvocation_Reply(
                   TCB_PTR(cap_reply_cap_get_capTCBPtr(cap)), slot,
                   cap_reply_cap_get_capReplyCanGrant(cap));

#endif

    അവസ്ഥ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
        എങ്കിൽ (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        തിരികെ decodeTCBInvocation(invLabel, length, cap, slot, call, buffer);

    അവസ്ഥ cap_domain_cap:
#ifdef CONFIG_KERNEL_MCS
        എങ്കിൽ (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        തിരികെ decodeDomainInvocation(invLabel, length, buffer);

    അവസ്ഥ cap_cnode_cap:
#ifdef CONFIG_KERNEL_MCS
        എങ്കിൽ (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        തിരികെ decodeCNodeInvocation(invLabel, length, cap, buffer);

    അവസ്ഥ cap_untyped_cap:
        തിരികെ decodeUntypedInvocation(invLabel, length, slot, cap, call, buffer);

    അവസ്ഥ cap_irq_control_cap:
        തിരികെ decodeIRQControlInvocation(invLabel, length, slot, buffer);

    അവസ്ഥ cap_irq_handler_cap:
        തിരികെ decodeIRQHandlerInvocation(invLabel,
                                          IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_sched_control_cap:
        എങ്കിൽ (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ decodeSchedControlInvocation(invLabel, cap, length, buffer);

    അവസ്ഥ cap_sched_context_cap:
        എങ്കിൽ (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        തിരികെ decodeSchedContextInvocation(invLabel, sc, call);
#endif
    സ്ഥിരസ്ഥിതി:
        fail("Invalid cap type");
    }
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call, bool_t canDonate)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, canDonate, കണ്ണി_STATE(ksCurThread), ep);

    തിരികെ EXCEPTION_NONE;
}
#അല്ലെങ്കിൽ
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t canGrantReply,
                                       bool_t block, bool_t call)
{
    sendIPC(block, call, badge, canGrant, canGrantReply, കണ്ണി_STATE(ksCurThread), ep);

    തിരികെ EXCEPTION_NONE;
}
#endif

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    തിരികെ EXCEPTION_NONE;
}

#ifdef CONFIG_KERNEL_MCS
exception_t performInvocation_Reply(tcb_t *thread, reply_t *reply, bool_t canGrant)
{
    doReplyTransfer(thread, reply, canGrant);
    തിരികെ EXCEPTION_NONE;
}
#അല്ലെങ്കിൽ
exception_t performInvocation_Reply(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    doReplyTransfer(കണ്ണി_STATE(ksCurThread), thread, slot, canGrant);
    തിരികെ EXCEPTION_NONE;
}
#endif

word_t CONST cap_get_capSizeBits(cap_t cap)
{

    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    തിരഞ്ഞെടുപ്പ് (ctag) {
    അവസ്ഥ cap_untyped_cap:
        തിരികെ cap_untyped_cap_get_capBlockSize(cap);

    അവസ്ഥ cap_endpoint_cap:
        തിരികെ seL4_EndpointBits;

    അവസ്ഥ cap_notification_cap:
        തിരികെ seL4_NotificationBits;

    അവസ്ഥ cap_cnode_cap:
        തിരികെ cap_cnode_cap_get_capCNodeRadix(cap) + seL4_SlotBits;

    അവസ്ഥ cap_thread_cap:
        തിരികെ seL4_TCBBits;

    അവസ്ഥ cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        എങ്കിൽ (type == ZombieType_ZombieTCB) {
            തിരികെ seL4_TCBBits;
        }
        തിരികെ ZombieType_ZombieCNode(type) + seL4_SlotBits;
    }

    അവസ്ഥ cap_null_cap:
        തിരികെ 0;

    അവസ്ഥ cap_domain_cap:
        തിരികെ 0;

    അവസ്ഥ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        തിരികെ seL4_ReplyBits;
#അല്ലെങ്കിൽ
        തിരികെ 0;
#endif

    അവസ്ഥ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_sched_control_cap:
#endif
        തിരികെ 0;

    അവസ്ഥ cap_irq_handler_cap:
        തിരികെ 0;

#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_sched_context_cap:
        തിരികെ cap_sched_context_cap_get_capSCSizeBits(cap);
#endif

    സ്ഥിരസ്ഥിതി:
        തിരികെ cap_get_archCapSizeBits(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    തിരഞ്ഞെടുപ്പ് (ctag) {
    അവസ്ഥ cap_untyped_cap:
        തിരികെ true;

    അവസ്ഥ cap_endpoint_cap:
        തിരികെ true;

    അവസ്ഥ cap_notification_cap:
        തിരികെ true;

    അവസ്ഥ cap_cnode_cap:
        തിരികെ true;

    അവസ്ഥ cap_thread_cap:
#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_sched_context_cap:
#endif
        തിരികെ true;

    അവസ്ഥ cap_zombie_cap:
        തിരികെ true;

    അവസ്ഥ cap_domain_cap:
        തിരികെ false;

    അവസ്ഥ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        തിരികെ true;
#അല്ലെങ്കിൽ
        തിരികെ false;
#endif

    അവസ്ഥ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_sched_control_cap:
#endif
        തിരികെ false;

    അവസ്ഥ cap_irq_handler_cap:
        തിരികെ false;

    സ്ഥിരസ്ഥിതി:
        തിരികെ cap_get_archCapIsPhysical(cap);
    }
}

ശൂന്യം *CONST cap_get_capPtr(cap_t cap)
{
    cap_tag_t ctag;

    ctag = cap_get_capType(cap);

    തിരഞ്ഞെടുപ്പ് (ctag) {
    അവസ്ഥ cap_untyped_cap:
        തിരികെ WORD_PTR(cap_untyped_cap_get_capPtr(cap));

    അവസ്ഥ cap_endpoint_cap:
        തിരികെ EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    അവസ്ഥ cap_notification_cap:
        തിരികെ NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    അവസ്ഥ cap_cnode_cap:
        തിരികെ CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    അവസ്ഥ cap_thread_cap:
        തിരികെ TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    അവസ്ഥ cap_zombie_cap:
        തിരികെ CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    അവസ്ഥ cap_domain_cap:
        തിരികെ ശൂന്യ;

    അവസ്ഥ cap_reply_cap:
#ifdef CONFIG_KERNEL_MCS
        തിരികെ REPLY_PTR(cap_reply_cap_get_capReplyPtr(cap));
#അല്ലെങ്കിൽ
        തിരികെ ശൂന്യ;
#endif

    അവസ്ഥ cap_irq_control_cap:
#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_sched_control_cap:
#endif
        തിരികെ ശൂന്യ;

    അവസ്ഥ cap_irq_handler_cap:
        തിരികെ ശൂന്യ;

#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_sched_context_cap:
        തിരികെ SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#endif

    സ്ഥിരസ്ഥിതി:
        തിരികെ cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    എങ്കിൽ (isArchCap(derivedCap)) {
        തിരികെ Arch_isCapRevocable(derivedCap, srcCap);
    }
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(derivedCap)) {
    അവസ്ഥ cap_endpoint_cap:
        തിരികെ (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    അവസ്ഥ cap_notification_cap:
        തിരികെ (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    അവസ്ഥ cap_irq_handler_cap:
        തിരികെ (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    അവസ്ഥ cap_untyped_cap:
        തിരികെ true;

    സ്ഥിരസ്ഥിതി:
        തിരികെ false;
    }
}
