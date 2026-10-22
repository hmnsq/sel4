/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD

#ಅನ್ತರ್ಭಾವ <machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <machine/timer.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
#ಅನ್ತರ್ಭಾವ <kernel/sporadic.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು ನೋಡಿದ_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t ನೋಡಿದ_list[ನೋಡಿದ_SZ];
ಪೂರ್ಣಾಂಕ watermark = 0;

ಶೂನ್ಯ ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    ಆದರೆ (watermark < ನೋಡಿದ_SZ) {
        ನೋಡಿದ_list[watermark] = c;
        watermark++;
    }
}

ಶೂನ್ಯ reset_ನೋಡಿದ_list(ಶೂನ್ಯ)
{
    memset(ನೋಡಿದ_list, 0, ನೋಡಿದ_SZ * ಗಾತ್ರ(ನೋಡಿದ_list[0]));
    watermark = 0;
}

bool_t ನೋಡಿದ(cap_t c)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < watermark; i++) {
        ಆದರೆ (same_cap(ನೋಡಿದ_list[i], c)) {
            ಹಿಂತಿರುಗಿ true;
        }
    }
    ಹಿಂತಿರುಗಿ false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    ಹಿಂತಿರುಗಿ (a.ಪದಗಳು[0] == b.ಪದಗಳು[0] && a.ಪದಗಳು[1] == b.ಪದಗಳು[1]);
}

/* Return true if strings are the same */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t ಅಕ್ಷರಸರಣಿಗಳು_equal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str2)
{
    ಆಗ (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    ಹಿಂತಿರುಗಿ !(*(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)str1 - * (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t ಬೇರು_or_idle_tcb(tcb_t *tcb)
{
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರಸರಣಿಗಳು_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || ಅಕ್ಷರಸರಣಿಗಳು_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING

ಶೂನ್ಯ obj_tcb_print_attrs(tcb_t *tcb)
{
    ಬರೆ("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)tcb->tcbIPCBuffer,
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)getRestartPC(tcb),
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)get_tcb_sp(tcb),
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)tcb->tcb_ಆದ್ಯತೆ,
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)tcb->tcbMCP);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಬರೆ(", affinity: %lu", (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)tcb->tcbAffinity);
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    /* init */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    ಆದರೆ (cap_get_capType(ep_cap) != cap_null_cap) {
        ಬರೆ(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಬರೆ(", dom: %ld)\n", tcb->tcbDomain);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t ಮೊತ್ತ_2 = refill_ತಲೆ(sc)->rAmount;
    ಶಬ್ದ_t current = sc->sc_Refill_ತಲೆ;

    ಆಗ (current != sc->sc_Refill_ಬಾಲ) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        ಮೊತ್ತ_2 += refill_ಸೂಚ್ಯಂಕ(sc, current)->rAmount;
    }

    ಹಿಂತಿರುಗಿ ಮೊತ್ತ_2;
}

ಶೂನ್ಯ obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    ಬರೆ("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_ಶಬ್ದ" bits)\n",
           ticks_ಗಮ್ಯ_Us(period), period,
           ticks_ಗಮ್ಯ_Us(budget), budget,
           (ಶಬ್ದ_t)cap_sched_context_cap_get_cap_SCSize_ಬಿಟ್ಟುಗಳು(sc_cap));
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */

ಶೂನ್ಯ obj_ut_print_attrs(cte_t *ಸ್ಥಾನ, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    ಬರೆ("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (ಶೂನ್ಯ *)cap_untyped_cap_get_capPtr(ಸ್ಥಾನ->cap),
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(ಸ್ಥಾನ->cap),
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(ಸ್ಥಾನ->cap),
           ಶಬ್ದ_PTR(cap_untyped_cap_get_capPtr(ಸ್ಥಾನ->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    ಗಾಗಿ (cte_t *ಮುಂದಿನ_Ptr = CTE_PTR(mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(ಸ್ಥಾನ->cteMDBNode));
         ಮುಂದಿನ_Ptr && is_MDBParent_ದ(ಸ್ಥಾನ, ಮುಂದಿನ_Ptr);
         ಮುಂದಿನ_Ptr = CTE_PTR(mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(ಸ್ಥಾನ->cteMDBNode))) {
        ಆದರೆ (!sameRegionAs(ಸ್ಥಾನ->cap, ಮುಂದಿನ_Ptr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            print_cap(ಮುಂದಿನ_Ptr->cap);
        }
    }
    ಬರೆ("}\n");
}

ಶೂನ್ಯ obj_cnode_print_attrs(cap_t cnode)
{
    ಬರೆ("(%lu bits)\n", (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_cnode_cap_get_capCNodeRadix(cnode));
}

ಶೂನ್ಯ obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    ಆದರೆ (ನೋಡಿದ(cnode)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(cnode);
    ಬರೆ("%p_cnode = cnode ", (ಶೂನ್ಯ *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    ಶಬ್ದ_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    ಗಾಗಿ (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_ಸ್ಥಾನ_ret_t c = lookup_Cap_And_ಸ್ಥಾನ(tcb, i);
        ಆದರೆ (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.ಸ್ಥಾನ, tcb);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

ಶೂನ್ಯ cap_cnode_print_attrs(cap_t cnode)
{
    ಬರೆ("(guard: %lu, guard_size: %lu)\n",
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_cnode_cap_get_capCNodeGuard(cnode),
           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_cnode_cap_get_cap_CNode_Guard_ಗಾತ್ರ(cnode));
}

ಶೂನ್ಯ cap_ep_print_attrs(cap_t ep)
{
    ಬರೆ("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? ಅಕ್ಷರ_ಬರೆ('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? ಅಕ್ಷರ_ಬರೆ('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? ಅಕ್ಷರ_ಬರೆ('G') : 0;
    cap_endpoint_cap_get_cap_Can_Grant_ಉತ್ತರ(ep) ? ಅಕ್ಷರ_ಬರೆ('P') : 0;
    ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? ಬರೆ(", badge: %lu)\n", badge) : ಬರೆ(")\n");
}

ಶೂನ್ಯ cap_ntfn_print_attrs(cap_t ntfn)
{
    ಬರೆ("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? ಅಕ್ಷರ_ಬರೆ('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? ಅಕ್ಷರ_ಬರೆ('W') : 0;
    ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? ಬರೆ(", badge: %lu)\n", badge) : ಬರೆ(")\n");
}

/*
 * print object slots
 */

ಶೂನ್ಯ obj_tcb_print_slots(tcb_t *tcb)
{
    ಬರೆ("%p_tcb {\n", tcb);

    /* CSpace root */
    ಆದರೆ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        ಬರೆ("cspace: %p_cnode ",
               (ಶೂನ್ಯ *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    ಆದರೆ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        ಬರೆ("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_ಬೇರು_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    ಆದರೆ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_ಬಫರ್)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_ಬಫರ್_ಸ್ಥಾನ(tcb);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    ಆದರೆ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        ಬರೆ("fault_ep_slot: %p_ep ",
               (ಶೂನ್ಯ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    ಆದರೆ (tcb->tcbSchedContext) {
        ಬರೆ("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    ಆದರೆ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        ಬರೆ("temp_fault_ep_slot: %p_ep ",
               (ಶೂನ್ಯ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# ಇಲ್ಲದಿದ್ದರೆ
    /* Reply cap slot */
    ಆದರೆ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_ಉತ್ತರ)->cap) != cap_null_cap) {
        ಬರೆ("reply_slot: %p_reply\n",
               (ಶೂನ್ಯ *)cap_ಉತ್ತರ_cap_get_cap_TCBPtr(TCB_PTR_CTE_PTR(tcb, tcb_ಉತ್ತರ)->cap));
    }

    /* TCB of most recent IPC sender */
    ಆದರೆ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        ಬರೆ("caller_slot: %p_tcb\n", caller);
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_MCS */
    ಬರೆ("}\n");
}

/* TBD: deal with nested cnodes */
ಶೂನ್ಯ obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t ಬೇರು = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    ಆದರೆ (cap_get_capType(ಬೇರು) != cap_cnode_cap) {
        ಹಿಂತಿರುಗಿ;
    }

    ಶಬ್ದ_t radix = cap_cnode_cap_get_capCNodeRadix(ಬೇರು);
    ಆದರೆ (ನೋಡಿದ(ಬೇರು)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಕೂಡಿಸು_ಗಮ್ಯ_ನೋಡಿದ(ಬೇರು);

    ಬರೆ("%p_cnode {\n", (ಶೂನ್ಯ *)cap_cnode_cap_get_capCNodePtr(ಬೇರು));

    ಗಾಗಿ (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_ಸ್ಥಾನ_ret_t c = lookup_Cap_And_ಸ್ಥಾನ(tcb, i);
        ಆದರೆ (cap_get_capType(c.cap) != cap_null_cap) {
            ಬರೆ("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    ಬರೆ("}\n");

    ಗಾಗಿ (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_ಸ್ಥಾನ_ret_t c = lookup_Cap_And_ಸ್ಥಾನ(tcb, i);
        ಆದರೆ (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

ಶೂನ್ಯ obj_irq_print_maps(ಶೂನ್ಯ)
{
    ಬರೆ("irq maps {\n");

    ಗಾಗಿ (se_L4_ಶಬ್ದ ಗುರಿ = 0; ಗುರಿ < CONFIG_MAX_ಅಂಕೆ_NODES; ಗುರಿ++) {
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_ಗಮ್ಯ_IRQT(ಗುರಿ, i);
            ಆದರೆ (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_ಗಮ್ಯ_IDX(irq)].cap;
                ಆದರೆ (cap_get_capType(cap) != cap_null_cap) {
                    ಬರೆ("%d: 0x%lx_%lu_irq\n",
                           i,
#ಆದರೆ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)irq.irq,
#ಇಲ್ಲದಿದ್ದರೆ
                           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)irq,
#ಆದರೆ_ಅಂತ್ಯ
                           (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಗುರಿ);
                }
            }
        }
    }
    ಬರೆ("}\n");
}

ಶೂನ್ಯ obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_ಗಮ್ಯ_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    ಆದರೆ (isIRQActive(irq)) {
        ಬರೆ("0x%lx_%lu_irq {\n",
#ಆದರೆ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)irq.irq,
#ಇಲ್ಲದಿದ್ದರೆ
               (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)irq,
#ಆದರೆ_ಅಂತ್ಯ
               (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_ಗಮ್ಯ_IDX(irq)].cap;
        ಆದರೆ (cap_get_capType(ntfn_cap) != cap_null_cap) {
            ಬರೆ("0x0: ");
            print_cap(ntfn_cap);
        }
        ಬರೆ("}\n");
    }
}

ಶೂನ್ಯ print_objects(ಶೂನ್ಯ)
{
    ಗಾಗಿ (tcb_t *curr = ಗಂಟು_STATE(ksDebugTCBs); curr != ಖಾಲಿ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ಮುಂದಿನ) {
        ಆದರೆ (ಬೇರು_or_idle_tcb(curr)) {
            ಮುಂದುವರಿ;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    ಗಾಗಿ (tcb_t *curr = ಗಂಟು_STATE(ksDebugTCBs); curr != ಖಾಲಿ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ಮುಂದಿನ) {
        ಆದರೆ (ಬೇರು_or_idle_tcb(curr)) {
            ಮುಂದುವರಿ;
        }

        /* print the tcb as objects */
        ಬರೆ("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        ಆದರೆ (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

ಶೂನ್ಯ print_caps(ಶೂನ್ಯ)
{
    ಗಾಗಿ (tcb_t *curr = ಗಂಟು_STATE(ksDebugTCBs); curr != ಖಾಲಿ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ಮುಂದಿನ) {
        ಆದರೆ (ಬೇರು_or_idle_tcb(curr)) {
            ಮುಂದುವರಿ;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

ಶೂನ್ಯ print_cap(cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_endpoint_cap: {
        ಬರೆ("%p_ep ",
               (ಶೂನ್ಯ *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_notification_cap: {
        ಬರೆ("%p_notification ",
               (ಶೂನ್ಯ *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_untyped_cap: {
        ಬರೆ("%p_untyped\n",
               (ಶೂನ್ಯ *)cap_untyped_cap_get_capPtr(cap));
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_thread_cap: {
        ಬರೆ("%p_tcb\n",
               (ಶೂನ್ಯ *)cap_thread_cap_get_capTCBPtr(cap));
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_cnode_cap: {
        ಬರೆ("%p_cnode ",
               (ಶೂನ್ಯ *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap: {
        ಬರೆ("%p_reply\n",
               (ಶೂನ್ಯ *)cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Ptr(cap));
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_sched_context_cap: {
        ಬರೆ("%p_sc\n",
               (ಶೂನ್ಯ *)cap_sched_context_cap_get_capSCPtr(cap));
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_sched_control_cap: {
        ಬರೆ("%lu_sched_control\n",
               (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)cap_sched_control_cap_get_core(cap));
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ cap_irq_control_cap: {
        ಬರೆ("irq_control\n"); /* only one in the system */
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_irq_handler_cap: {
        ಬರೆ("%p_%lu_irq\n",
               (ಶೂನ್ಯ *)cap_irq_handler_cap_get_capIRQ(cap),
               (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_CORE(IDX_ಗಮ್ಯ_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ: {
        print_cap_arch(cap);
        ನಿಲ್ಲಿಸು;
    }
    }
}

ಶೂನ್ಯ print_object(cap_t cap)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_endpoint_cap: {
        ಬರೆ("%p_ep = ep\n",
               (ಶೂನ್ಯ *)cap_endpoint_cap_get_capEPPtr(cap));
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_notification_cap: {
        ಬರೆ("%p_notification = notification\n",
               (ಶೂನ್ಯ *)cap_notification_cap_get_capNtfnPtr(cap));
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_cnode_cap: {
        ಪ್ರತಿಜ್ಞಾ(!"should not happend");
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap: {
        ಬರೆ("%p_reply = rtreply\n",
               (ಶೂನ್ಯ *)cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Ptr(cap));
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ cap_sched_context_cap: {
        ಬರೆ("%p_sc = sc ",
               (ಶೂನ್ಯ *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        ನಿಲ್ಲಿಸು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ cap_irq_handler_cap: {
        ಬರೆ("%p_%lu_irq = irq\n",
               (ಶೂನ್ಯ *)cap_irq_handler_cap_get_capIRQ(cap),
               (ದೀರ್ಘ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)IRQT_ಗಮ್ಯ_CORE(IDX_ಗಮ್ಯ_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        print_object_arch(cap);
        ನಿಲ್ಲಿಸು;
    }
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */
