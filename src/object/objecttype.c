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
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
#अन्तर्भाव <object/schedcontext.h>
#अन्तर्भाव <object/schedcontrol.h>
#यदि_अन्तः
#अन्तर्भाव <object/tcb.h>
#अन्तर्भाव <object/untyped.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <machine.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <string.h>

word_t get_Object_आकार(word_t t, word_t user_Obj_आकार)
{
    यदि (t >= se_L4_Non_Arch_Object_Type_गणनम्) {
        प्रत्यागम Arch_get_Object_आकार(t);
    } अन्यथा {
        विकल्प (t) {
        पक्ष seL4_TCBObject:
            प्रत्यागम seL4_TCBBits;
        पक्ष seL4_EndpointObject:
            प्रत्यागम se_L4_Endpoint_द्वयङ्काः;
        पक्ष seL4_NotificationObject:
            प्रत्यागम se_L4_Notification_द्वयङ्काः;
        पक्ष se_L4_Cap_सारणी_Object:
            प्रत्यागम se_L4_Slot_द्वयङ्काः + user_Obj_आकार;
        पक्ष seL4_UntypedObject:
            प्रत्यागम user_Obj_आकार;
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        पक्ष seL4_SchedContextObject:
            प्रत्यागम user_Obj_आकार;
        पक्ष se_L4_प्रत्युत्तरम्_Object:
            प्रत्यागम se_L4_प्रत्युत्तरम्_द्वयङ्काः;
#यदि_अन्तः
        स्वभाव:
            fail("Invalid object type");
            प्रत्यागम 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    यदि (isArchCap(cap)) {
        प्रत्यागम Arch_deriveCap(slot, cap);
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_zombie_cap:
        ret.स्थितिः = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        भङ्ग;

    पक्ष cap_irq_control_cap:
        ret.स्थितिः = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        भङ्ग;

    पक्ष cap_untyped_cap:
        ret.स्थितिः = ensureNoChildren(slot);
        यदि (ret.स्थितिः != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } अन्यथा {
            ret.cap = cap;
        }
        भङ्ग;

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_प्रत्युत्तरम्_cap:
        ret.स्थितिः = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        भङ्ग;
#यदि_अन्तः
    स्वभाव:
        ret.स्थितिः = EXCEPTION_NONE;
        ret.cap = cap;
    }

    प्रत्यागम ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    यदि (isArchCap(cap)) {
        प्रत्यागम Arch_finaliseCap(cap, final);
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_endpoint_cap:
        यदि (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        प्रत्यागम fc_ret;

    पक्ष cap_notification_cap:
        यदि (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#यदि_अन्तः
            unbindMaybeNotification(ntfn);
            cancel_All_सङ्केताः(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        प्रत्यागम fc_ret;

    पक्ष cap_प्रत्युत्तरम्_cap:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        यदि (final) {
            प्रत्युत्तरम्_t *प्रत्युत्तरम् = प्रत्युत्तरम्_PTR(cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Ptr(cap));
            यदि (प्रत्युत्तरम् && प्रत्युत्तरम्->प्रत्युत्तरम्_TCB) {
                tcb_t *tcb = प्रत्युत्तरम्->प्रत्युत्तरम्_TCB;
                विकल्प (thread_state_get_tsType(tcb->tcbState)) {
                पक्ष Thread_State_Blocked_On_प्रत्युत्तरम्:
                    प्रत्युत्तरम्_remove(प्रत्युत्तरम्, tcb);
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
#यदि_अन्तः
    पक्ष cap_null_cap:
    पक्ष cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        प्रत्यागम fc_ret;
    }

    यदि (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_cnode_cap: {
        यदि (final) {
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
        यदि (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            यदि (sc) {
                schedContext_unbindTCB(sc);
                यदि (sc->scYieldFrom) {
                    sched_Context_complete_Yield_गन्तव्यम्(sc->scYieldFrom);
                }
            }
#यदि_अन्तः
            suspend(tcb);
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
            tcbDebugRemove(tcb);
#यदि_अन्तः
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

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
        यदि (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            sched_Context_unbind_प्रत्युत्तरम्(sc);
            यदि (sc->scYieldFrom) {
                sched_Context_complete_Yield_गन्तव्यम्(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            प्रत्यागम fc_ret;
        }
        भङ्ग;
#यदि_अन्तः

    पक्ष cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        प्रत्यागम fc_ret;

    पक्ष cap_irq_handler_cap:
        यदि (final) {
            irq_t irq = IDX_गन्तव्यम्_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

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
               cap_endpoint_cap_get_cap_Can_Grant_प्रत्युत्तरम्(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    स्वभाव:
        प्रत्यागम false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {
    पक्ष cap_untyped_cap:
        यदि (cap_get_capIsPhysical(cap_b)) {
            word_t a_आधारः, b_आधारः, a_शिखरम्, b_शिखरम्;

            a_आधारः = (word_t)WORD_PTR(cap_untyped_cap_get_capPtr(cap_a));
            b_आधारः = (word_t)cap_get_capPtr(cap_b);

            a_शिखरम् = a_आधारः + आवरणम्(cap_untyped_cap_get_cap_खण्डम्_आकार(cap_a));
            b_शिखरम् = b_आधारः + आवरणम्(cap_get_cap_आकार_द्वयङ्काः(cap_b));

            प्रत्यागम (a_आधारः <= b_आधारः) && (b_शिखरम् <= a_शिखरम्) && (b_आधारः <= b_शिखरम्);
        }
        भङ्ग;

    पक्ष cap_endpoint_cap:
        यदि (cap_get_capType(cap_b) == cap_endpoint_cap) {
            प्रत्यागम cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_notification_cap:
        यदि (cap_get_capType(cap_b) == cap_notification_cap) {
            प्रत्यागम cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_cnode_cap:
        यदि (cap_get_capType(cap_b) == cap_cnode_cap) {
            प्रत्यागम (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        भङ्ग;

    पक्ष cap_thread_cap:
        यदि (cap_get_capType(cap_b) == cap_thread_cap) {
            प्रत्यागम cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_प्रत्युत्तरम्_cap:
        यदि (cap_get_capType(cap_b) == cap_प्रत्युत्तरम्_cap) {
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
            प्रत्यागम cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Ptr(cap_a) ==
                   cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Ptr(cap_b);
#अन्यथा
            प्रत्यागम cap_प्रत्युत्तरम्_cap_get_cap_TCBPtr(cap_a) ==
                   cap_प्रत्युत्तरम्_cap_get_cap_TCBPtr(cap_b);
#यदि_अन्तः
        }
        भङ्ग;

    पक्ष cap_domain_cap:
        यदि (cap_get_capType(cap_b) == cap_domain_cap) {
            प्रत्यागम true;
        }
        भङ्ग;

    पक्ष cap_irq_control_cap:
        यदि (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            प्रत्यागम true;
        }
        भङ्ग;

    पक्ष cap_irq_handler_cap:
        यदि (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            प्रत्यागम (word_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (word_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        भङ्ग;

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
        यदि (cap_get_capType(cap_b) == cap_sched_context_cap) {
            प्रत्यागम (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_cap_SCSize_द्वयङ्काः(cap_a) ==
                    cap_sched_context_cap_get_cap_SCSize_द्वयङ्काः(cap_b));
        }
        भङ्ग;
    पक्ष cap_sched_control_cap:
        यदि (cap_get_capType(cap_b) == cap_sched_control_cap) {
            प्रत्यागम true;
        }
        भङ्ग;
#यदि_अन्तः
    स्वभाव:
        यदि (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            प्रत्यागम Arch_sameRegionAs(cap_a, cap_b);
        }
        भङ्ग;
    }

    प्रत्यागम false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    यदि (cap_get_capType(cap_a) == cap_untyped_cap) {
        प्रत्यागम false;
    }
    यदि (cap_get_capType(cap_a) == cap_irq_control_cap) {
        प्रत्यागम false;
    }
    यदि (isArchCap(cap_a) && isArchCap(cap_b)) {
        प्रत्यागम Arch_sameObjectAs(cap_a, cap_b);
    }
    प्रत्यागम sameRegionAs(cap_a, cap_b);
}

cap_t CONST update_Cap_दत्त(bool_t preserve, word_t new_दत्त, cap_t cap)
{
    यदि (isArchCap(cap)) {
        प्रत्यागम Arch_update_Cap_दत्त(preserve, new_दत्त, cap);
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_endpoint_cap:
        यदि (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            प्रत्यागम cap_endpoint_cap_समुच्चयः_cap_EPBadge(cap, new_दत्त);
        } अन्यथा {
            प्रत्यागम cap_null_cap_new();
        }

    पक्ष cap_notification_cap:
        यदि (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            प्रत्यागम cap_notification_cap_समुच्चयः_cap_Ntfn_Badge(cap, new_दत्त);
        } अन्यथा {
            प्रत्यागम cap_null_cap_new();
        }

    पक्ष cap_cnode_cap: {
        word_t guard, guard_आकार;
        se_L4_CNode_Cap_दत्त_t w = { .शब्दाः = { new_दत्त } };

        guard_आकार = se_L4_CNode_Cap_दत्त_get_guard_आकार(w);

        यदि (guard_आकार + cap_cnode_cap_get_capCNodeRadix(cap) > word_द्वयङ्काः) {
            प्रत्यागम cap_null_cap_new();
        } अन्यथा {
            cap_t new_cap;

            guard = se_L4_CNode_Cap_दत्त_get_guard(w) & आवरणम्(guard_आकार);
            new_cap = cap_cnode_cap_समुच्चयः_cap_CNode_Guard(cap, guard);
            new_cap = cap_cnode_cap_समुच्चयः_cap_CNode_Guard_आकार(new_cap,
                                                          guard_आकार);

            प्रत्यागम new_cap;
        }
    }

    स्वभाव:
        प्रत्यागम cap;
    }
}

cap_t CONST आवरणम्_Cap_Rights(seL4_CapRights_t cap_rights, cap_t cap)
{
    यदि (isArchCap(cap)) {
        प्रत्यागम Arch_आवरणम्_Cap_Rights(cap_rights, cap);
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
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
    पक्ष cap_sched_control_cap:
#यदि_अन्तः
        प्रत्यागम cap;

    पक्ष cap_endpoint_cap: {
        cap_t new_cap;

        new_cap = cap_endpoint_cap_समुच्चयः_cap_Can_Send(
                      cap, cap_endpoint_cap_get_capCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_endpoint_cap_समुच्चयः_cap_Can_Receive(
                      new_cap, cap_endpoint_cap_get_capCanReceive(cap) &
                      seL4_CapRights_get_capAllowRead(cap_rights));
        new_cap = cap_endpoint_cap_समुच्चयः_cap_Can_Grant(
                      new_cap, cap_endpoint_cap_get_capCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        new_cap = cap_endpoint_cap_समुच्चयः_cap_Can_Grant_प्रत्युत्तरम्(
                      new_cap, cap_endpoint_cap_get_cap_Can_Grant_प्रत्युत्तरम्(cap) &
                      se_L4_Cap_Rights_get_cap_Allow_Grant_प्रत्युत्तरम्(cap_rights));

        प्रत्यागम new_cap;
    }

    पक्ष cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_समुच्चयः_cap_Ntfn_Can_Send(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_समुच्चयः_cap_Ntfn_Can_Receive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        प्रत्यागम new_cap;
    }
    पक्ष cap_प्रत्युत्तरम्_cap: {
        cap_t new_cap;

        new_cap = cap_प्रत्युत्तरम्_cap_समुच्चयः_cap_प्रत्युत्तरम्_Can_Grant(
                      cap, cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Can_Grant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        प्रत्यागम new_cap;
    }


    स्वभाव:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, शून्य *region_आधारः, word_t user_आकार, bool_t device_स्मृतिः)
{
    /* Handle architecture-specific objects. */
    यदि (t >= (object_t) se_L4_Non_Arch_Object_Type_गणनम्) {
        प्रत्यागम Arch_createObject(t, region_आधारः, user_आकार, device_स्मृतिः);
    }

    /* Create objects. */
    विकल्प ((api_object_t)t) {
    पक्ष seL4_TCBObject: {
        tcb_t *tcb;
        tcb = TCB_PTR((word_t)region_आधारः + TCB_विस्थापनम्);
        /** AUXUPD: "(True, ptr_retyps 1
          (Ptr ((ptr_val \<acute>tcb) - ctcb_offset) :: (cte_C[5]) ptr)
            o (ptr_retyp \<acute>tcb))" */

        /* Setup non-zero parts of the TCB. */

        Arch_initContext(&tcb->tcbArch.tcbContext);
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
        tcb->tcb_Time_खण्डिका = CONFIG_TIME_खण्डिका;
#यदि_अन्तः
        tcb->tcbDomain = ksCurDomain;
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
        /* Initialize the new TCB to the current core */
        SMP_COND_STATEMENT(tcb->tcbAffinity = getCurrentCPUIndex());
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_दीर्घता);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(ग्रन्थिः_STATE(ksCurThread))->tcbName, TCB_NAME_दीर्घता);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_दीर्घता);
        tcbDebugAppend(tcb);
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */

        प्रत्यागम cap_thread_cap_new(TCB_REF(tcb));
    }

    पक्ष seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        प्रत्यागम cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(region_आधारः));

    पक्ष seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        प्रत्यागम cap_notification_cap_new(0, true, true,
                                        NTFN_REF(region_आधारः));

    पक्ष se_L4_Cap_सारणी_Object:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        प्रत्यागम cap_cnode_cap_new(user_आकार, 0, 0, CTE_REF(region_आधारः));

    पक्ष seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        प्रत्यागम cap_untyped_cap_new(0, !!device_स्मृतिः, user_आकार, WORD_REF(region_आधारः));

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
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
        प्रत्यागम cap_sched_context_cap_new(SC_REF(region_आधारः), user_आकार);

    पक्ष se_L4_प्रत्युत्तरम्_Object:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        प्रत्यागम cap_प्रत्युत्तरम्_cap_new(प्रत्युत्तरम्_REF(region_आधारः), true);
#यदि_अन्तः

    स्वभाव:
        fail("Invalid object type");
    }
}

शून्य createNewObjects(object_t t, cte_t *parent,
                      cte_t *destCNode, word_t dest_विस्थापनम्, word_t dest_दीर्घता,
                      शून्य *region_आधारः, word_t user_आकार, bool_t device_स्मृतिः)
{
    word_t object_आकार;
    शून्य *अग्र_Free_Area;
    word_t i;
    word_t योग_Object_आकार अप्रयुक्तम्;

    /* ghost check that we're visiting less bytes than the max object size */
    object_आकार = get_Object_आकार(t, user_आकार);
    योग_Object_आकार = dest_दीर्घता << object_आकार;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    अग्र_Free_Area = region_आधारः;
    प्रति (i = 0; i < dest_दीर्घता; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (शून्य *)((word_t)अग्र_Free_Area + (i << object_आकार)), user_आकार, device_स्मृतिः);

        /* Insert the cap into the user's cspace. */
        निवेशय_New_Cap(parent, &destCNode[dest_विस्थापनम् + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
exception_t decodeInvocation(word_t invLabel, word_t दीर्घता,
                             cptr_t cap_सूचकांक, cte_t *slot, cap_t cap,
                             bool_t खण्डम्_2, bool_t call,
                             bool_t canDonate, bool_t firstPhase, word_t *बफर)
#अन्यथा
exception_t decodeInvocation(word_t invLabel, word_t दीर्घता,
                             cptr_t cap_सूचकांक, cte_t *slot, cap_t cap,
                             bool_t खण्डम्_2, bool_t call,
                             word_t *बफर)
#यदि_अन्तः
{
    यदि (isArchCap(cap)) {
        प्रत्यागम Arch_decodeInvocation(invLabel, दीर्घता, cap_सूचकांक,
                                     slot, cap, call, बफर);
    }

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", cap_सूचकांक);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;

    पक्ष cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", cap_सूचकांक);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;

    पक्ष cap_endpoint_cap:
        यदि (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      cap_सूचकांक);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        प्रत्यागम performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_प्रत्युत्तरम्(cap), खण्डम्_2, call, canDonate);
#अन्यथा
        प्रत्यागम performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_प्रत्युत्तरम्(cap), खण्डम्_2, call);
#यदि_अन्तः

    पक्ष cap_notification_cap: {
        यदि (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      cap_सूचकांक);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_प्रत्युत्तरम्_cap:
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम perform_Invocation_प्रत्युत्तरम्(
                   ग्रन्थिः_STATE(ksCurThread),
                   प्रत्युत्तरम्_PTR(cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Ptr(cap)),
                   cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Can_Grant(cap));
#अन्यथा
    पक्ष cap_प्रत्युत्तरम्_cap:
        यदि (unlikely(cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Master(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      cap_सूचकांक);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम perform_Invocation_प्रत्युत्तरम्(
                   TCB_PTR(cap_प्रत्युत्तरम्_cap_get_cap_TCBPtr(cap)), slot,
                   cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Can_Grant(cap));

#यदि_अन्तः

    पक्ष cap_thread_cap:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        यदि (unlikely(firstPhase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#यदि_अन्तः
        प्रत्यागम decodeTCBInvocation(invLabel, दीर्घता, cap, slot, call, बफर);

    पक्ष cap_domain_cap:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        यदि (unlikely(firstPhase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#यदि_अन्तः
        प्रत्यागम decodeDomainInvocation(invLabel, दीर्घता, बफर);

    पक्ष cap_cnode_cap:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        यदि (unlikely(firstPhase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#यदि_अन्तः
        प्रत्यागम decodeCNodeInvocation(invLabel, दीर्घता, cap, बफर);

    पक्ष cap_untyped_cap:
        प्रत्यागम decodeUntypedInvocation(invLabel, दीर्घता, slot, cap, call, बफर);

    पक्ष cap_irq_control_cap:
        प्रत्यागम decodeIRQControlInvocation(invLabel, दीर्घता, slot, बफर);

    पक्ष cap_irq_handler_cap:
        प्रत्यागम decodeIRQHandlerInvocation(invLabel,
                                          IDX_गन्तव्यम्_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_sched_control_cap:
        यदि (unlikely(firstPhase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम decodeSchedControlInvocation(invLabel, cap, दीर्घता, बफर);

    पक्ष cap_sched_context_cap:
        यदि (unlikely(firstPhase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        प्रत्यागम decodeSchedContextInvocation(invLabel, sc, call);
#यदि_अन्तः
    स्वभाव:
        fail("Invalid cap type");
    }
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t can_Grant_प्रत्युत्तरम्,
                                       bool_t खण्डम्_2, bool_t call, bool_t canDonate)
{
    sendIPC(खण्डम्_2, call, badge, canGrant, can_Grant_प्रत्युत्तरम्, canDonate, ग्रन्थिः_STATE(ksCurThread), ep);

    प्रत्यागम EXCEPTION_NONE;
}
#अन्यथा
exception_t performInvocation_Endpoint(endpoint_t *ep, word_t badge,
                                       bool_t canGrant, bool_t can_Grant_प्रत्युत्तरम्,
                                       bool_t खण्डम्_2, bool_t call)
{
    sendIPC(खण्डम्_2, call, badge, canGrant, can_Grant_प्रत्युत्तरम्, ग्रन्थिः_STATE(ksCurThread), ep);

    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः

exception_t performInvocation_Notification(notification_t *ntfn, word_t badge)
{
    sendSignal(ntfn, badge);

    प्रत्यागम EXCEPTION_NONE;
}

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
exception_t perform_Invocation_प्रत्युत्तरम्(tcb_t *thread, प्रत्युत्तरम्_t *प्रत्युत्तरम्, bool_t canGrant)
{
    do_प्रत्युत्तरम्_Transfer(thread, प्रत्युत्तरम्, canGrant);
    प्रत्यागम EXCEPTION_NONE;
}
#अन्यथा
exception_t perform_Invocation_प्रत्युत्तरम्(tcb_t *thread, cte_t *slot, bool_t canGrant)
{
    do_प्रत्युत्तरम्_Transfer(ग्रन्थिः_STATE(ksCurThread), thread, slot, canGrant);
    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः

word_t CONST cap_get_cap_आकार_द्वयङ्काः(cap_t cap)
{

    cap_चिह्नकम्_t ctag;

    ctag = cap_get_capType(cap);

    विकल्प (ctag) {
    पक्ष cap_untyped_cap:
        प्रत्यागम cap_untyped_cap_get_cap_खण्डम्_आकार(cap);

    पक्ष cap_endpoint_cap:
        प्रत्यागम se_L4_Endpoint_द्वयङ्काः;

    पक्ष cap_notification_cap:
        प्रत्यागम se_L4_Notification_द्वयङ्काः;

    पक्ष cap_cnode_cap:
        प्रत्यागम cap_cnode_cap_get_capCNodeRadix(cap) + se_L4_Slot_द्वयङ्काः;

    पक्ष cap_thread_cap:
        प्रत्यागम seL4_TCBBits;

    पक्ष cap_zombie_cap: {
        word_t type = cap_zombie_cap_get_capZombieType(cap);
        यदि (type == ZombieType_ZombieTCB) {
            प्रत्यागम seL4_TCBBits;
        }
        प्रत्यागम ZombieType_ZombieCNode(type) + se_L4_Slot_द्वयङ्काः;
    }

    पक्ष cap_null_cap:
        प्रत्यागम 0;

    पक्ष cap_domain_cap:
        प्रत्यागम 0;

    पक्ष cap_प्रत्युत्तरम्_cap:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        प्रत्यागम se_L4_प्रत्युत्तरम्_द्वयङ्काः;
#अन्यथा
        प्रत्यागम 0;
#यदि_अन्तः

    पक्ष cap_irq_control_cap:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_sched_control_cap:
#यदि_अन्तः
        प्रत्यागम 0;

    पक्ष cap_irq_handler_cap:
        प्रत्यागम 0;

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
        प्रत्यागम cap_sched_context_cap_get_cap_SCSize_द्वयङ्काः(cap);
#यदि_अन्तः

    स्वभाव:
        प्रत्यागम cap_get_arch_Cap_आकार_द्वयङ्काः(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_चिह्नकम्_t ctag;

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
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
#यदि_अन्तः
        प्रत्यागम true;

    पक्ष cap_zombie_cap:
        प्रत्यागम true;

    पक्ष cap_domain_cap:
        प्रत्यागम false;

    पक्ष cap_प्रत्युत्तरम्_cap:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        प्रत्यागम true;
#अन्यथा
        प्रत्यागम false;
#यदि_अन्तः

    पक्ष cap_irq_control_cap:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_sched_control_cap:
#यदि_अन्तः
        प्रत्यागम false;

    पक्ष cap_irq_handler_cap:
        प्रत्यागम false;

    स्वभाव:
        प्रत्यागम cap_get_archCapIsPhysical(cap);
    }
}

शून्य *CONST cap_get_capPtr(cap_t cap)
{
    cap_चिह्नकम्_t ctag;

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

    पक्ष cap_प्रत्युत्तरम्_cap:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        प्रत्यागम प्रत्युत्तरम्_PTR(cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Ptr(cap));
#अन्यथा
        प्रत्यागम रिक्त;
#यदि_अन्तः

    पक्ष cap_irq_control_cap:
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_sched_control_cap:
#यदि_अन्तः
        प्रत्यागम रिक्त;

    पक्ष cap_irq_handler_cap:
        प्रत्यागम रिक्त;

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष cap_sched_context_cap:
        प्रत्यागम SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#यदि_अन्तः

    स्वभाव:
        प्रत्यागम cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    यदि (isArchCap(derivedCap)) {
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
