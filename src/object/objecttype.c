/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <arch/object/objecttype.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <object/objecttype.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <object/notification.h>
#అన్తర్భావ <object/endpoint.h>
#అన్తర్భావ <object/cnode.h>
#అన్తర్భావ <object/interrupt.h>
#నిర్వచితమైతే CONFIG_KERNEL_MCS
#అన్తర్భావ <object/schedcontext.h>
#అన్తర్భావ <object/schedcontrol.h>
#అయితే_ముగింపు
#అన్తర్భావ <object/tcb.h>
#అన్తర్భావ <object/untyped.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <machine.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <string.h>

మాట_t get_Object_కొలత(మాట_t t, మాట_t user_Obj_కొలత)
{
    ఒకవేళ (t >= se_L4_Non_Arch_Object_Type_లెక్క) {
        తిరిగి Arch_get_Object_కొలత(t);
    } లేకపోతే {
        ఎంపిక (t) {
        స్థితి seL4_TCBObject:
            తిరిగి seL4_TCBBits;
        స్థితి seL4_EndpointObject:
            తిరిగి se_L4_Endpoint_బిట్లు;
        స్థితి seL4_NotificationObject:
            తిరిగి se_L4_Notification_బిట్లు;
        స్థితి se_L4_Cap_పట్టిక_Object:
            తిరిగి se_L4_స్థానం_బిట్లు + user_Obj_కొలత;
        స్థితి seL4_UntypedObject:
            తిరిగి user_Obj_కొలత;
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        స్థితి seL4_SchedContextObject:
            తిరిగి user_Obj_కొలత;
        స్థితి se_L4_జవాబు_Object:
            తిరిగి se_L4_జవాబు_బిట్లు;
#అయితే_ముగింపు
        అప్రమేయ:
            fail("Invalid object type");
            తిరిగి 0;
        }
    }
}

deriveCap_ret_t deriveCap(cte_t *స్థానం, cap_t cap)
{
    deriveCap_ret_t ret;

    ఒకవేళ (isArchCap(cap)) {
        తిరిగి Arch_deriveCap(స్థానం, cap);
    }

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_zombie_cap:
        ret.స్థితి_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ఆపు;

    స్థితి cap_irq_control_cap:
        ret.స్థితి_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ఆపు;

    స్థితి cap_untyped_cap:
        ret.స్థితి_2 = ensureNoChildren(స్థానం);
        ఒకవేళ (ret.స్థితి_2 != EXCEPTION_NONE) {
            ret.cap = cap_null_cap_new();
        } లేకపోతే {
            ret.cap = cap;
        }
        ఆపు;

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
    స్థితి cap_జవాబు_cap:
        ret.స్థితి_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ఆపు;
#అయితే_ముగింపు
    అప్రమేయ:
        ret.స్థితి_2 = EXCEPTION_NONE;
        ret.cap = cap;
    }

    తిరిగి ret;
}

finaliseCap_ret_t finaliseCap(cap_t cap, bool_t final, bool_t exposed)
{
    finaliseCap_ret_t fc_ret;

    ఒకవేళ (isArchCap(cap)) {
        తిరిగి Arch_finaliseCap(cap, final);
    }

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_endpoint_cap:
        ఒకవేళ (final) {
            cancelAllIPC(EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)));
        }

        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        తిరిగి fc_ret;

    స్థితి cap_notification_cap:
        ఒకవేళ (final) {
            notification_t *ntfn = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));
#నిర్వచితమైతే CONFIG_KERNEL_MCS
            schedContextMaybeUnbindNtfn(ntfn);
#అయితే_ముగింపు
            unbindMaybeNotification(ntfn);
            cancel_All_సంకేతాలు(ntfn);
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        తిరిగి fc_ret;

    స్థితి cap_జవాబు_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        ఒకవేళ (final) {
            జవాబు_t *జవాబు = జవాబు_PTR(cap_జవాబు_cap_get_cap_జవాబు_Ptr(cap));
            ఒకవేళ (జవాబు && జవాబు->జవాబు_TCB) {
                tcb_t *tcb = జవాబు->జవాబు_TCB;
                ఎంపిక (thread_state_get_tsType(tcb->tcbState)) {
                స్థితి Thread_State_Blocked_On_జవాబు:
                    జవాబు_remove(జవాబు, tcb);
                    ఆపు;
                స్థితి ThreadState_BlockedOnReceive:
                    cancelIPC(tcb);
                    ఆపు;
                అప్రమేయ:
                    fail("Invalid tcb state");
                }
            }
        }
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        తిరిగి fc_ret;
#అయితే_ముగింపు
    స్థితి cap_null_cap:
    స్థితి cap_domain_cap:
        fc_ret.remainder = cap_null_cap_new();
        fc_ret.cleanupInfo = cap_null_cap_new();
        తిరిగి fc_ret;
    }

    ఒకవేళ (exposed) {
        fail("finaliseCap: failed to finalise immediately.");
    }

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_cnode_cap: {
        ఒకవేళ (final) {
            fc_ret.remainder =
                Zombie_new(
                    1ul << cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodeRadix(cap),
                    cap_cnode_cap_get_capCNodePtr(cap)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            తిరిగి fc_ret;
        }
        ఆపు;
    }

    స్థితి cap_thread_cap: {
        ఒకవేళ (final) {
            tcb_t *tcb;
            cte_t *cte_ptr;

            tcb = TCB_PTR(cap_thread_cap_get_capTCBPtr(cap));
            SMP_COND_STATEMENT(remoteTCBStall(tcb);)
            cte_ptr = TCB_PTR_CTE_PTR(tcb, tcbCTable);
            unbindNotification(tcb);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
            sched_context_t *sc = SC_PTR(tcb->tcbSchedContext);
            ఒకవేళ (sc) {
                schedContext_unbindTCB(sc);
                ఒకవేళ (sc->scYieldFrom) {
                    sched_Context_complete_Yield_గమ్యం(sc->scYieldFrom);
                }
            }
#అయితే_ముగింపు
            suspend(tcb);
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
            tcbDebugRemove(tcb);
#అయితే_ముగింపు
            Arch_prepareThreadDelete(tcb);
            fc_ret.remainder =
                Zombie_new(
                    tcbArchCNodeEntries,
                    ZombieType_ZombieTCB,
                    CTE_REF(cte_ptr)
                );
            fc_ret.cleanupInfo = cap_null_cap_new();
            తిరిగి fc_ret;
        }
        ఆపు;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_sched_context_cap:
        ఒకవేళ (final) {
            sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
            schedContext_unbindAllTCBs(sc);
            schedContext_unbindNtfn(sc);
            sched_Context_unbind_జవాబు(sc);
            ఒకవేళ (sc->scYieldFrom) {
                sched_Context_complete_Yield_గమ్యం(sc->scYieldFrom);
            }
            /* mark the sc as no longer valid */
            sc->scRefillMax = 0;
            sc->scSporadic = false;
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap_null_cap_new();
            తిరిగి fc_ret;
        }
        ఆపు;
#అయితే_ముగింపు

    స్థితి cap_zombie_cap:
        fc_ret.remainder = cap;
        fc_ret.cleanupInfo = cap_null_cap_new();
        తిరిగి fc_ret;

    స్థితి cap_irq_handler_cap:
        ఒకవేళ (final) {
            irq_t irq = IDX_గమ్యం_IRQT(cap_irq_handler_cap_get_capIRQ(cap));

            deletingIRQHandler(irq);

            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            తిరిగి fc_ret;
        }
        ఆపు;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    తిరిగి fc_ret;
}

bool_t CONST hasCancelSendRights(cap_t cap)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_endpoint_cap:
        తిరిగి cap_endpoint_cap_get_capCanSend(cap) &&
               cap_endpoint_cap_get_capCanReceive(cap) &&
               cap_endpoint_cap_get_cap_Can_Grant_జవాబు(cap) &&
               cap_endpoint_cap_get_capCanGrant(cap);

    అప్రమేయ:
        తిరిగి false;
    }
}

bool_t CONST sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ఎంపిక (cap_get_capType(cap_a)) {
    స్థితి cap_untyped_cap:
        ఒకవేళ (cap_get_capIsPhysical(cap_b)) {
            మాట_t a_పునాది, b_పునాది, a_శిఖరం, b_శిఖరం;

            a_పునాది = (మాట_t)మాట_PTR(cap_untyped_cap_get_capPtr(cap_a));
            b_పునాది = (మాట_t)cap_get_capPtr(cap_b);

            a_శిఖరం = a_పునాది + ముసుగు(cap_untyped_cap_get_cap_ఖండం_కొలత(cap_a));
            b_శిఖరం = b_పునాది + ముసుగు(cap_get_cap_కొలత_బిట్లు(cap_b));

            తిరిగి (a_పునాది <= b_పునాది) && (b_శిఖరం <= a_శిఖరం) && (b_పునాది <= b_శిఖరం);
        }
        ఆపు;

    స్థితి cap_endpoint_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_endpoint_cap) {
            తిరిగి cap_endpoint_cap_get_capEPPtr(cap_a) ==
                   cap_endpoint_cap_get_capEPPtr(cap_b);
        }
        ఆపు;

    స్థితి cap_notification_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_notification_cap) {
            తిరిగి cap_notification_cap_get_capNtfnPtr(cap_a) ==
                   cap_notification_cap_get_capNtfnPtr(cap_b);
        }
        ఆపు;

    స్థితి cap_cnode_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_cnode_cap) {
            తిరిగి (cap_cnode_cap_get_capCNodePtr(cap_a) ==
                    cap_cnode_cap_get_capCNodePtr(cap_b)) &&
                   (cap_cnode_cap_get_capCNodeRadix(cap_a) ==
                    cap_cnode_cap_get_capCNodeRadix(cap_b));
        }
        ఆపు;

    స్థితి cap_thread_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_thread_cap) {
            తిరిగి cap_thread_cap_get_capTCBPtr(cap_a) ==
                   cap_thread_cap_get_capTCBPtr(cap_b);
        }
        ఆపు;

    స్థితి cap_జవాబు_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_జవాబు_cap) {
#నిర్వచితమైతే CONFIG_KERNEL_MCS
            తిరిగి cap_జవాబు_cap_get_cap_జవాబు_Ptr(cap_a) ==
                   cap_జవాబు_cap_get_cap_జవాబు_Ptr(cap_b);
#లేకపోతే
            తిరిగి cap_జవాబు_cap_get_cap_TCBPtr(cap_a) ==
                   cap_జవాబు_cap_get_cap_TCBPtr(cap_b);
#అయితే_ముగింపు
        }
        ఆపు;

    స్థితి cap_domain_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_domain_cap) {
            తిరిగి true;
        }
        ఆపు;

    స్థితి cap_irq_control_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_irq_control_cap ||
            cap_get_capType(cap_b) == cap_irq_handler_cap ||
            Arch_isIRQControlDescendant(cap_b)) {
            తిరిగి true;
        }
        ఆపు;

    స్థితి cap_irq_handler_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_irq_handler_cap) {
            తిరిగి (మాట_t)cap_irq_handler_cap_get_capIRQ(cap_a) ==
                   (మాట_t)cap_irq_handler_cap_get_capIRQ(cap_b);
        }
        ఆపు;

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_sched_context_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_sched_context_cap) {
            తిరిగి (cap_sched_context_cap_get_capSCPtr(cap_a) ==
                    cap_sched_context_cap_get_capSCPtr(cap_b)) &&
                   (cap_sched_context_cap_get_cap_SCSize_బిట్లు(cap_a) ==
                    cap_sched_context_cap_get_cap_SCSize_బిట్లు(cap_b));
        }
        ఆపు;
    స్థితి cap_sched_control_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_sched_control_cap) {
            తిరిగి true;
        }
        ఆపు;
#అయితే_ముగింపు
    అప్రమేయ:
        ఒకవేళ (isArchCap(cap_a) &&
            isArchCap(cap_b)) {
            తిరిగి Arch_sameRegionAs(cap_a, cap_b);
        }
        ఆపు;
    }

    తిరిగి false;
}

bool_t CONST sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ఒకవేళ (cap_get_capType(cap_a) == cap_untyped_cap) {
        తిరిగి false;
    }
    ఒకవేళ (cap_get_capType(cap_a) == cap_irq_control_cap) {
        తిరిగి false;
    }
    ఒకవేళ (isArchCap(cap_a) && isArchCap(cap_b)) {
        తిరిగి Arch_sameObjectAs(cap_a, cap_b);
    }
    తిరిగి sameRegionAs(cap_a, cap_b);
}

cap_t CONST update_Cap_దత్తాంశం(bool_t preserve, మాట_t new_దత్తాంశం, cap_t cap)
{
    ఒకవేళ (isArchCap(cap)) {
        తిరిగి Arch_update_Cap_దత్తాంశం(preserve, new_దత్తాంశం, cap);
    }

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_endpoint_cap:
        ఒకవేళ (!preserve && cap_endpoint_cap_get_capEPBadge(cap) == 0) {
            తిరిగి cap_endpoint_cap_సమితి_cap_EPBadge(cap, new_దత్తాంశం);
        } లేకపోతే {
            తిరిగి cap_null_cap_new();
        }

    స్థితి cap_notification_cap:
        ఒకవేళ (!preserve && cap_notification_cap_get_capNtfnBadge(cap) == 0) {
            తిరిగి cap_notification_cap_సమితి_cap_Ntfn_Badge(cap, new_దత్తాంశం);
        } లేకపోతే {
            తిరిగి cap_null_cap_new();
        }

    స్థితి cap_cnode_cap: {
        మాట_t guard, guard_కొలత;
        se_L4_CNode_Cap_దత్తాంశం_t w = { .పదాలు = { new_దత్తాంశం } };

        guard_కొలత = se_L4_CNode_Cap_దత్తాంశం_get_guard_కొలత(w);

        ఒకవేళ (guard_కొలత + cap_cnode_cap_get_capCNodeRadix(cap) > మాట_బిట్లు) {
            తిరిగి cap_null_cap_new();
        } లేకపోతే {
            cap_t new_cap;

            guard = se_L4_CNode_Cap_దత్తాంశం_get_guard(w) & ముసుగు(guard_కొలత);
            new_cap = cap_cnode_cap_సమితి_cap_CNode_Guard(cap, guard);
            new_cap = cap_cnode_cap_సమితి_cap_CNode_Guard_కొలత(new_cap,
                                                          guard_కొలత);

            తిరిగి new_cap;
        }
    }

    అప్రమేయ:
        తిరిగి cap;
    }
}

cap_t CONST ముసుగు_Cap_Rights(seL4_CapRights_t cap_rights, cap_t cap)
{
    ఒకవేళ (isArchCap(cap)) {
        తిరిగి Arch_ముసుగు_Cap_Rights(cap_rights, cap);
    }

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_null_cap:
    స్థితి cap_domain_cap:
    స్థితి cap_cnode_cap:
    స్థితి cap_untyped_cap:
    స్థితి cap_irq_control_cap:
    స్థితి cap_irq_handler_cap:
    స్థితి cap_zombie_cap:
    స్థితి cap_thread_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_sched_context_cap:
    స్థితి cap_sched_control_cap:
#అయితే_ముగింపు
        తిరిగి cap;

    స్థితి cap_endpoint_cap: {
        cap_t new_cap;

        new_cap = cap_endpoint_cap_సమితి_cap_Can_Send(
                      cap, cap_endpoint_cap_get_capCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_endpoint_cap_సమితి_cap_Can_Receive(
                      new_cap, cap_endpoint_cap_get_capCanReceive(cap) &
                      seL4_CapRights_get_capAllowRead(cap_rights));
        new_cap = cap_endpoint_cap_సమితి_cap_Can_Grant(
                      new_cap, cap_endpoint_cap_get_capCanGrant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        new_cap = cap_endpoint_cap_సమితి_cap_Can_Grant_జవాబు(
                      new_cap, cap_endpoint_cap_get_cap_Can_Grant_జవాబు(cap) &
                      se_L4_Cap_Rights_get_cap_Allow_Grant_జవాబు(cap_rights));

        తిరిగి new_cap;
    }

    స్థితి cap_notification_cap: {
        cap_t new_cap;

        new_cap = cap_notification_cap_సమితి_cap_Ntfn_Can_Send(
                      cap, cap_notification_cap_get_capNtfnCanSend(cap) &
                      seL4_CapRights_get_capAllowWrite(cap_rights));
        new_cap = cap_notification_cap_సమితి_cap_Ntfn_Can_Receive(new_cap,
                                                             cap_notification_cap_get_capNtfnCanReceive(cap) &
                                                             seL4_CapRights_get_capAllowRead(cap_rights));

        తిరిగి new_cap;
    }
    స్థితి cap_జవాబు_cap: {
        cap_t new_cap;

        new_cap = cap_జవాబు_cap_సమితి_cap_జవాబు_Can_Grant(
                      cap, cap_జవాబు_cap_get_cap_జవాబు_Can_Grant(cap) &
                      seL4_CapRights_get_capAllowGrant(cap_rights));
        తిరిగి new_cap;
    }


    అప్రమేయ:
        fail("Invalid cap type"); /* Sentinel for invalid enums */
    }
}

cap_t createObject(object_t t, శూన్య *region_పునాది, మాట_t user_కొలత, bool_t device_జ్ఞాపకం)
{
    /* Handle architecture-specific objects. */
    ఒకవేళ (t >= (object_t) se_L4_Non_Arch_Object_Type_లెక్క) {
        తిరిగి Arch_createObject(t, region_పునాది, user_కొలత, device_జ్ఞాపకం);
    }

    /* Create objects. */
    ఎంపిక ((api_object_t)t) {
    స్థితి seL4_TCBObject: {
        tcb_t *tcb;
        tcb = TCB_PTR((మాట_t)region_పునాది + TCB_స్థానభ్రంశం);
        /** AUXUPD: "(True, ptr_retyps 1
          (Ptr ((ptr_val \<acute>tcb) - ctcb_offset) :: (cte_C[5]) ptr)
            o (ptr_retyp \<acute>tcb))" */

        /* Setup non-zero parts of the TCB. */

        Arch_initContext(&tcb->tcbArch.tcbContext);
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
        tcb->tcb_సమయం_ముక్క = CONFIG_సమయం_ముక్క;
#అయితే_ముగింపు
        tcb->tcbDomain = ksCurDomain;
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
        /* Initialize the new TCB to the current core */
        SMP_COND_STATEMENT(tcb->tcbAffinity = getCurrentCPUIndex());
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
        strlcpy(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "child of: '", TCB_NAME_నిడివి);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, TCB_PTR_DEBUG_PTR(కణుపు_STATE(ksCurThread))->tcbName, TCB_NAME_నిడివి);
        strlcat(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "'", TCB_NAME_నిడివి);
        tcbDebugAppend(tcb);
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */

        తిరిగి cap_thread_cap_new(TCB_REF(tcb));
    }

    స్థితి seL4_EndpointObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: endpoint_C ptr))" */
        తిరిగి cap_endpoint_cap_new(0, true, true, true, true,
                                    EP_REF(region_పునాది));

    స్థితి seL4_NotificationObject:
        /** AUXUPD: "(True, ptr_retyp
              (Ptr (ptr_val \<acute>regionBase) :: notification_C ptr))" */
        తిరిగి cap_notification_cap_new(0, true, true,
                                        NTFN_REF(region_పునాది));

    స్థితి se_L4_Cap_పట్టిక_Object:
        /** AUXUPD: "(True, ptr_arr_retyps (2 ^ (unat \<acute>userSize))
          (Ptr (ptr_val \<acute>regionBase) :: cte_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_cnodes (unat \<acute>userSize)
                                (ptr_val \<acute>regionBase)
                                (4 + unat \<acute>userSize))" */
        తిరిగి cap_cnode_cap_new(user_కొలత, 0, 0, CTE_REF(region_పునాది));

    స్థితి seL4_UntypedObject:
        /*
         * No objects need to be created; instead, just insert caps into
         * the destination slots.
         */
        తిరిగి cap_untyped_cap_new(0, !!device_జ్ఞాపకం, user_కొలత, మాట_REF(region_పునాది));

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి seL4_SchedContextObject:
        /** AUXUPD:
            "(True,
              ptr_arr_retyps (refills_len (unat \<acute>userSize)
                                          (size_of TYPE(sched_context_C))
                                          (size_of TYPE(refill_C)))
                             (Ptr ((ptr_val \<acute>regionBase) +
                                   word_of_nat (size_of TYPE(sched_context_C))) :: refill_C ptr)
              \<circ> ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: sched_context_C ptr))" */
        /** GHOSTUPD: "(True, gs_new_sc_size (ptr_val \<acute>regionBase) (unat \<acute>userSize))" */
        తిరిగి cap_sched_context_cap_new(SC_REF(region_పునాది), user_కొలత);

    స్థితి se_L4_జవాబు_Object:
        /** AUXUPD: "(True, ptr_retyp (Ptr (ptr_val \<acute>regionBase) :: reply_C ptr))" */
        తిరిగి cap_జవాబు_cap_new(జవాబు_REF(region_పునాది), true);
#అయితే_ముగింపు

    అప్రమేయ:
        fail("Invalid object type");
    }
}

శూన్య createNewObjects(object_t t, cte_t *మాతృ,
                      cte_t *destCNode, మాట_t dest_స్థానభ్రంశం, మాట_t dest_నిడివి,
                      శూన్య *region_పునాది, మాట_t user_కొలత, bool_t device_జ్ఞాపకం)
{
    మాట_t object_కొలత;
    శూన్య *తదుపరి_Free_Area;
    మాట_t i;
    మాట_t మొత్తం_Object_కొలత వాడని;

    /* ghost check that we're visiting less bytes than the max object size */
    object_కొలత = get_Object_కొలత(t, user_కొలత);
    మొత్తం_Object_కొలత = dest_నిడివి << object_కొలత;
    /** GHOSTUPD: "(gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
        \<or> \<acute>totalObjectSize <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state, id)" */

    /* Create the objects. */
    తదుపరి_Free_Area = region_పునాది;
    కొరకు (i = 0; i < dest_నిడివి; i++) {
        /* Create the object. */
        /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute> nextFreeArea + ((\<acute> i) << unat (\<acute> objectSize))) (unat (\<acute> objectSize)))" */
        cap_t cap = createObject(t, (శూన్య *)((మాట_t)తదుపరి_Free_Area + (i << object_కొలత)), user_కొలత, device_జ్ఞాపకం);

        /* Insert the cap into the user's cspace. */
        చొప్పించు_New_Cap(మాతృ, &destCNode[dest_స్థానభ్రంశం + i], cap);

        /* Move along to the next region of memory. been merged into a formula of i */
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
exception_t decodeInvocation(మాట_t invLabel, మాట_t నిడివి,
                             cptr_t cap_సూచీ, cte_t *స్థానం, cap_t cap,
                             bool_t ఖండం_2, bool_t call,
                             bool_t canDonate, bool_t మొదటి_Phase, మాట_t *బఫరు)
#లేకపోతే
exception_t decodeInvocation(మాట_t invLabel, మాట_t నిడివి,
                             cptr_t cap_సూచీ, cte_t *స్థానం, cap_t cap,
                             bool_t ఖండం_2, bool_t call,
                             మాట_t *బఫరు)
#అయితే_ముగింపు
{
    ఒకవేళ (isArchCap(cap)) {
        తిరిగి Arch_decodeInvocation(invLabel, నిడివి, cap_సూచీ,
                                     స్థానం, cap, call, బఫరు);
    }

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_null_cap:
        userError("Attempted to invoke a null cap #%lu.", cap_సూచీ);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;

    స్థితి cap_zombie_cap:
        userError("Attempted to invoke a zombie cap #%lu.", cap_సూచీ);
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;

    స్థితి cap_endpoint_cap:
        ఒకవేళ (unlikely(!cap_endpoint_cap_get_capCanSend(cap))) {
            userError("Attempted to invoke a read-only endpoint cap #%lu.",
                      cap_సూచీ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        తిరిగి performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_జవాబు(cap), ఖండం_2, call, canDonate);
#లేకపోతే
        తిరిగి performInvocation_Endpoint(
                   EP_PTR(cap_endpoint_cap_get_capEPPtr(cap)),
                   cap_endpoint_cap_get_capEPBadge(cap),
                   cap_endpoint_cap_get_capCanGrant(cap),
                   cap_endpoint_cap_get_cap_Can_Grant_జవాబు(cap), ఖండం_2, call);
#అయితే_ముగింపు

    స్థితి cap_notification_cap: {
        ఒకవేళ (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
            userError("Attempted to invoke a read-only notification cap #%lu.",
                      cap_సూచీ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performInvocation_Notification(
                   NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap)),
                   cap_notification_cap_get_capNtfnBadge(cap));
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_జవాబు_cap:
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_Invocation_జవాబు(
                   కణుపు_STATE(ksCurThread),
                   జవాబు_PTR(cap_జవాబు_cap_get_cap_జవాబు_Ptr(cap)),
                   cap_జవాబు_cap_get_cap_జవాబు_Can_Grant(cap));
#లేకపోతే
    స్థితి cap_జవాబు_cap:
        ఒకవేళ (unlikely(cap_జవాబు_cap_get_cap_జవాబు_Master(cap))) {
            userError("Attempted to invoke an invalid reply cap #%lu.",
                      cap_సూచీ);
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_Invocation_జవాబు(
                   TCB_PTR(cap_జవాబు_cap_get_cap_TCBPtr(cap)), స్థానం,
                   cap_జవాబు_cap_get_cap_జవాబు_Can_Grant(cap));

#అయితే_ముగింపు

    స్థితి cap_thread_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        ఒకవేళ (unlikely(మొదటి_Phase)) {
            userError("Cannot invoke thread capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#అయితే_ముగింపు
        తిరిగి decodeTCBInvocation(invLabel, నిడివి, cap, స్థానం, call, బఫరు);

    స్థితి cap_domain_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        ఒకవేళ (unlikely(మొదటి_Phase)) {
            userError("Cannot invoke domain capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#అయితే_ముగింపు
        తిరిగి decodeDomainInvocation(invLabel, నిడివి, బఫరు);

    స్థితి cap_cnode_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        ఒకవేళ (unlikely(మొదటి_Phase)) {
            userError("Cannot invoke cnode capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#అయితే_ముగింపు
        తిరిగి decodeCNodeInvocation(invLabel, నిడివి, cap, బఫరు);

    స్థితి cap_untyped_cap:
        తిరిగి decodeUntypedInvocation(invLabel, నిడివి, స్థానం, cap, call, బఫరు);

    స్థితి cap_irq_control_cap:
        తిరిగి decodeIRQControlInvocation(invLabel, నిడివి, స్థానం, బఫరు);

    స్థితి cap_irq_handler_cap:
        తిరిగి decodeIRQHandlerInvocation(invLabel,
                                          IDX_గమ్యం_IRQT(cap_irq_handler_cap_get_capIRQ(cap)));

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_sched_control_cap:
        ఒకవేళ (unlikely(మొదటి_Phase)) {
            userError("Cannot invoke sched control capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి decodeSchedControlInvocation(invLabel, cap, నిడివి, బఫరు);

    స్థితి cap_sched_context_cap:
        ఒకవేళ (unlikely(మొదటి_Phase)) {
            userError("Cannot invoke sched context capabilities in the first phase of an invocation");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
        తిరిగి decodeSchedContextInvocation(invLabel, sc, call);
#అయితే_ముగింపు
    అప్రమేయ:
        fail("Invalid cap type");
    }
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
exception_t performInvocation_Endpoint(endpoint_t *ep, మాట_t badge,
                                       bool_t canGrant, bool_t can_Grant_జవాబు,
                                       bool_t ఖండం_2, bool_t call, bool_t canDonate)
{
    sendIPC(ఖండం_2, call, badge, canGrant, can_Grant_జవాబు, canDonate, కణుపు_STATE(ksCurThread), ep);

    తిరిగి EXCEPTION_NONE;
}
#లేకపోతే
exception_t performInvocation_Endpoint(endpoint_t *ep, మాట_t badge,
                                       bool_t canGrant, bool_t can_Grant_జవాబు,
                                       bool_t ఖండం_2, bool_t call)
{
    sendIPC(ఖండం_2, call, badge, canGrant, can_Grant_జవాబు, కణుపు_STATE(ksCurThread), ep);

    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు

exception_t performInvocation_Notification(notification_t *ntfn, మాట_t badge)
{
    sendSignal(ntfn, badge);

    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
exception_t perform_Invocation_జవాబు(tcb_t *thread, జవాబు_t *జవాబు, bool_t canGrant)
{
    do_జవాబు_Transfer(thread, జవాబు, canGrant);
    తిరిగి EXCEPTION_NONE;
}
#లేకపోతే
exception_t perform_Invocation_జవాబు(tcb_t *thread, cte_t *స్థానం, bool_t canGrant)
{
    do_జవాబు_Transfer(కణుపు_STATE(ksCurThread), thread, స్థానం, canGrant);
    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు

మాట_t CONST cap_get_cap_కొలత_బిట్లు(cap_t cap)
{

    cap_ట్యాగ్_t ctag;

    ctag = cap_get_capType(cap);

    ఎంపిక (ctag) {
    స్థితి cap_untyped_cap:
        తిరిగి cap_untyped_cap_get_cap_ఖండం_కొలత(cap);

    స్థితి cap_endpoint_cap:
        తిరిగి se_L4_Endpoint_బిట్లు;

    స్థితి cap_notification_cap:
        తిరిగి se_L4_Notification_బిట్లు;

    స్థితి cap_cnode_cap:
        తిరిగి cap_cnode_cap_get_capCNodeRadix(cap) + se_L4_స్థానం_బిట్లు;

    స్థితి cap_thread_cap:
        తిరిగి seL4_TCBBits;

    స్థితి cap_zombie_cap: {
        మాట_t type = cap_zombie_cap_get_capZombieType(cap);
        ఒకవేళ (type == ZombieType_ZombieTCB) {
            తిరిగి seL4_TCBBits;
        }
        తిరిగి ZombieType_ZombieCNode(type) + se_L4_స్థానం_బిట్లు;
    }

    స్థితి cap_null_cap:
        తిరిగి 0;

    స్థితి cap_domain_cap:
        తిరిగి 0;

    స్థితి cap_జవాబు_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        తిరిగి se_L4_జవాబు_బిట్లు;
#లేకపోతే
        తిరిగి 0;
#అయితే_ముగింపు

    స్థితి cap_irq_control_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_sched_control_cap:
#అయితే_ముగింపు
        తిరిగి 0;

    స్థితి cap_irq_handler_cap:
        తిరిగి 0;

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_sched_context_cap:
        తిరిగి cap_sched_context_cap_get_cap_SCSize_బిట్లు(cap);
#అయితే_ముగింపు

    అప్రమేయ:
        తిరిగి cap_get_arch_Cap_కొలత_బిట్లు(cap);
    }

}

/* Returns whether or not this capability has memory associated
 * with it or not. Referring to this as 'being physical' is to
 * match up with the Haskell and abstract specifications */
bool_t CONST cap_get_capIsPhysical(cap_t cap)
{
    cap_ట్యాగ్_t ctag;

    ctag = cap_get_capType(cap);

    ఎంపిక (ctag) {
    స్థితి cap_untyped_cap:
        తిరిగి true;

    స్థితి cap_endpoint_cap:
        తిరిగి true;

    స్థితి cap_notification_cap:
        తిరిగి true;

    స్థితి cap_cnode_cap:
        తిరిగి true;

    స్థితి cap_thread_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_sched_context_cap:
#అయితే_ముగింపు
        తిరిగి true;

    స్థితి cap_zombie_cap:
        తిరిగి true;

    స్థితి cap_domain_cap:
        తిరిగి false;

    స్థితి cap_జవాబు_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        తిరిగి true;
#లేకపోతే
        తిరిగి false;
#అయితే_ముగింపు

    స్థితి cap_irq_control_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_sched_control_cap:
#అయితే_ముగింపు
        తిరిగి false;

    స్థితి cap_irq_handler_cap:
        తిరిగి false;

    అప్రమేయ:
        తిరిగి cap_get_archCapIsPhysical(cap);
    }
}

శూన్య *CONST cap_get_capPtr(cap_t cap)
{
    cap_ట్యాగ్_t ctag;

    ctag = cap_get_capType(cap);

    ఎంపిక (ctag) {
    స్థితి cap_untyped_cap:
        తిరిగి మాట_PTR(cap_untyped_cap_get_capPtr(cap));

    స్థితి cap_endpoint_cap:
        తిరిగి EP_PTR(cap_endpoint_cap_get_capEPPtr(cap));

    స్థితి cap_notification_cap:
        తిరిగి NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    స్థితి cap_cnode_cap:
        తిరిగి CTE_PTR(cap_cnode_cap_get_capCNodePtr(cap));

    స్థితి cap_thread_cap:
        తిరిగి TCB_PTR_CTE_PTR(cap_thread_cap_get_capTCBPtr(cap), 0);

    స్థితి cap_zombie_cap:
        తిరిగి CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap));

    స్థితి cap_domain_cap:
        తిరిగి ఖాళీ;

    స్థితి cap_జవాబు_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        తిరిగి జవాబు_PTR(cap_జవాబు_cap_get_cap_జవాబు_Ptr(cap));
#లేకపోతే
        తిరిగి ఖాళీ;
#అయితే_ముగింపు

    స్థితి cap_irq_control_cap:
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_sched_control_cap:
#అయితే_ముగింపు
        తిరిగి ఖాళీ;

    స్థితి cap_irq_handler_cap:
        తిరిగి ఖాళీ;

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి cap_sched_context_cap:
        తిరిగి SC_PTR(cap_sched_context_cap_get_capSCPtr(cap));
#అయితే_ముగింపు

    అప్రమేయ:
        తిరిగి cap_get_archCapPtr(cap);

    }
}

bool_t CONST isCapRevocable(cap_t derivedCap, cap_t srcCap)
{
    ఒకవేళ (isArchCap(derivedCap)) {
        తిరిగి Arch_isCapRevocable(derivedCap, srcCap);
    }
    ఎంపిక (cap_get_capType(derivedCap)) {
    స్థితి cap_endpoint_cap:
        తిరిగి (cap_endpoint_cap_get_capEPBadge(derivedCap) !=
                cap_endpoint_cap_get_capEPBadge(srcCap));

    స్థితి cap_notification_cap:
        తిరిగి (cap_notification_cap_get_capNtfnBadge(derivedCap) !=
                cap_notification_cap_get_capNtfnBadge(srcCap));

    స్థితి cap_irq_handler_cap:
        తిరిగి (cap_get_capType(srcCap) ==
                cap_irq_control_cap);

    స్థితి cap_untyped_cap:
        తిరిగి true;

    అప్రమేయ:
        తిరిగి false;
    }
}
