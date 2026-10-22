/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/capdl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/sporadic.h>
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵੇਖਿਆ_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t ਵੇਖਿਆ_list[ਵੇਖਿਆ_SZ];
ਪੂਰਨ watermark = 0;

ਖਾਲੀ ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    ਜੇ (watermark < ਵੇਖਿਆ_SZ) {
        ਵੇਖਿਆ_list[watermark] = c;
        watermark++;
    }
}

ਖਾਲੀ reset_ਵੇਖਿਆ_list(ਖਾਲੀ)
{
    memset(ਵੇਖਿਆ_list, 0, ਵੇਖਿਆ_SZ * ਆਕਾਰ(ਵੇਖਿਆ_list[0]));
    watermark = 0;
}

bool_t ਵੇਖਿਆ(cap_t c)
{
    ਲਈ (ਪੂਰਨ i = 0; i < watermark; i++) {
        ਜੇ (same_cap(ਵੇਖਿਆ_list[i], c)) {
            ਵਾਪਸ true;
        }
    }
    ਵਾਪਸ false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    ਵਾਪਸ (a.ਸ਼ਬਦ[0] == b.ਸ਼ਬਦ[0] && a.ਸ਼ਬਦ[1] == b.ਸ਼ਬਦ[1]);
}

/* Return true if strings are the same */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t ਲੜੀਆਂ_equal(ਅਚਲ ਅੱਖਰ *str1, ਅਚਲ ਅੱਖਰ *str2)
{
    ਜਦੋਂ (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    ਵਾਪਸ !(*(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)str1 - * (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t ਜੜ੍ਹ_or_idle_tcb(tcb_t *tcb)
{
    ਵਾਪਸ (ਲੜੀਆਂ_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || ਲੜੀਆਂ_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING

ਖਾਲੀ obj_tcb_print_attrs(tcb_t *tcb)
{
    ਲਿਖੋ("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)tcb->tcbIPCBuffer,
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)getRestartPC(tcb),
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)get_tcb_sp(tcb),
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)tcb->tcb_ਤਰਜੀਹ,
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)tcb->tcbMCP);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਲਿਖੋ(", affinity: %lu", (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)tcb->tcbAffinity);
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

    /* init */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    ਜੇ (cap_get_capType(ep_cap) != cap_null_cap) {
        ਲਿਖੋ(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#ਜੇ_ਅੰਤ

    ਲਿਖੋ(", dom: %ld)\n", tcb->tcbDomain);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS

ਸਥਿਰ ਸਤਰਅੰਦਰ ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t ਜੋੜ_2 = refill_ਸਿਰ(sc)->rAmount;
    ਸ਼ਬਦ_t current = sc->sc_Refill_ਸਿਰ;

    ਜਦੋਂ (current != sc->sc_Refill_ਪੂਛ) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        ਜੋੜ_2 += refill_ਸੂਚਕਾਂਕ(sc, current)->rAmount;
    }

    ਵਾਪਸ ਜੋੜ_2;
}

ਖਾਲੀ obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    ਲਿਖੋ("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_ਸ਼ਬਦ" bits)\n",
           ticks_ਮੰਜ਼ਿਲ_Us(period), period,
           ticks_ਮੰਜ਼ਿਲ_Us(budget), budget,
           (ਸ਼ਬਦ_t)cap_sched_context_cap_get_cap_SCSize_ਬਿੱਟ(sc_cap));
}
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */

ਖਾਲੀ obj_ut_print_attrs(cte_t *ਥਾਂ, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    ਲਿਖੋ("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (ਖਾਲੀ *)cap_untyped_cap_get_capPtr(ਥਾਂ->cap),
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(ਥਾਂ->cap),
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(ਥਾਂ->cap),
           ਸ਼ਬਦ_PTR(cap_untyped_cap_get_capPtr(ਥਾਂ->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    ਲਈ (cte_t *ਅਗਲਾ_Ptr = CTE_PTR(mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(ਥਾਂ->cteMDBNode));
         ਅਗਲਾ_Ptr && is_MDBParent_ਦਾ(ਥਾਂ, ਅਗਲਾ_Ptr);
         ਅਗਲਾ_Ptr = CTE_PTR(mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(ਥਾਂ->cteMDBNode))) {
        ਜੇ (!sameRegionAs(ਥਾਂ->cap, ਅਗਲਾ_Ptr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            print_cap(ਅਗਲਾ_Ptr->cap);
        }
    }
    ਲਿਖੋ("}\n");
}

ਖਾਲੀ obj_cnode_print_attrs(cap_t cnode)
{
    ਲਿਖੋ("(%lu bits)\n", (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_cnode_cap_get_capCNodeRadix(cnode));
}

ਖਾਲੀ obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    ਜੇ (ਵੇਖਿਆ(cnode)) {
        ਵਾਪਸ;
    }
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(cnode);
    ਲਿਖੋ("%p_cnode = cnode ", (ਖਾਲੀ *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    ਸ਼ਬਦ_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    ਲਈ (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_ਥਾਂ_ret_t c = lookup_Cap_And_ਥਾਂ(tcb, i);
        ਜੇ (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.ਥਾਂ, tcb);

        } ਨਹੀਂਤਾਂ ਜੇ (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } ਨਹੀਂਤਾਂ ਜੇ (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

ਖਾਲੀ cap_cnode_print_attrs(cap_t cnode)
{
    ਲਿਖੋ("(guard: %lu, guard_size: %lu)\n",
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_cnode_cap_get_capCNodeGuard(cnode),
           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_cnode_cap_get_cap_CNode_Guard_ਮਾਪ(cnode));
}

ਖਾਲੀ cap_ep_print_attrs(cap_t ep)
{
    ਲਿਖੋ("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? ਅੱਖਰ_ਲਿਖੋ('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? ਅੱਖਰ_ਲਿਖੋ('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? ਅੱਖਰ_ਲਿਖੋ('G') : 0;
    cap_endpoint_cap_get_cap_Can_Grant_ਜਵਾਬ(ep) ? ਅੱਖਰ_ਲਿਖੋ('P') : 0;
    ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? ਲਿਖੋ(", badge: %lu)\n", badge) : ਲਿਖੋ(")\n");
}

ਖਾਲੀ cap_ntfn_print_attrs(cap_t ntfn)
{
    ਲਿਖੋ("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? ਅੱਖਰ_ਲਿਖੋ('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? ਅੱਖਰ_ਲਿਖੋ('W') : 0;
    ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? ਲਿਖੋ(", badge: %lu)\n", badge) : ਲਿਖੋ(")\n");
}

/*
 * print object slots
 */

ਖਾਲੀ obj_tcb_print_slots(tcb_t *tcb)
{
    ਲਿਖੋ("%p_tcb {\n", tcb);

    /* CSpace root */
    ਜੇ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        ਲਿਖੋ("cspace: %p_cnode ",
               (ਖਾਲੀ *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    ਜੇ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        ਲਿਖੋ("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_ਜੜ੍ਹ_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    ਜੇ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_ਬਫਰ)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_ਬਫਰ_ਥਾਂ(tcb);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    ਜੇ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        ਲਿਖੋ("fault_ep_slot: %p_ep ",
               (ਖਾਲੀ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    ਜੇ (tcb->tcbSchedContext) {
        ਲਿਖੋ("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    ਜੇ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        ਲਿਖੋ("temp_fault_ep_slot: %p_ep ",
               (ਖਾਲੀ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# ਨਹੀਂਤਾਂ
    /* Reply cap slot */
    ਜੇ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_ਜਵਾਬ)->cap) != cap_null_cap) {
        ਲਿਖੋ("reply_slot: %p_reply\n",
               (ਖਾਲੀ *)cap_ਜਵਾਬ_cap_get_cap_TCBPtr(TCB_PTR_CTE_PTR(tcb, tcb_ਜਵਾਬ)->cap));
    }

    /* TCB of most recent IPC sender */
    ਜੇ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        ਲਿਖੋ("caller_slot: %p_tcb\n", caller);
    }
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_MCS */
    ਲਿਖੋ("}\n");
}

/* TBD: deal with nested cnodes */
ਖਾਲੀ obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t ਜੜ੍ਹ = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    ਜੇ (cap_get_capType(ਜੜ੍ਹ) != cap_cnode_cap) {
        ਵਾਪਸ;
    }

    ਸ਼ਬਦ_t radix = cap_cnode_cap_get_capCNodeRadix(ਜੜ੍ਹ);
    ਜੇ (ਵੇਖਿਆ(ਜੜ੍ਹ)) {
        ਵਾਪਸ;
    }
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(ਜੜ੍ਹ);

    ਲਿਖੋ("%p_cnode {\n", (ਖਾਲੀ *)cap_cnode_cap_get_capCNodePtr(ਜੜ੍ਹ));

    ਲਈ (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_ਥਾਂ_ret_t c = lookup_Cap_And_ਥਾਂ(tcb, i);
        ਜੇ (cap_get_capType(c.cap) != cap_null_cap) {
            ਲਿਖੋ("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    ਲਿਖੋ("}\n");

    ਲਈ (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_ਥਾਂ_ret_t c = lookup_Cap_And_ਥਾਂ(tcb, i);
        ਜੇ (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

ਖਾਲੀ obj_irq_print_maps(ਖਾਲੀ)
{
    ਲਿਖੋ("irq maps {\n");

    ਲਈ (se_L4_ਸ਼ਬਦ ਨਿਸ਼ਾਨਾ = 0; ਨਿਸ਼ਾਨਾ < CONFIG_MAX_ਅੰਕ_NODES; ਨਿਸ਼ਾਨਾ++) {
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(ਨਿਸ਼ਾਨਾ, i);
            ਜੇ (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_ਮੰਜ਼ਿਲ_IDX(irq)].cap;
                ਜੇ (cap_get_capType(cap) != cap_null_cap) {
                    ਲਿਖੋ("%d: 0x%lx_%lu_irq\n",
                           i,
#ਜੇ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)irq.irq,
#ਨਹੀਂਤਾਂ
                           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)irq,
#ਜੇ_ਅੰਤ
                           (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਨਿਸ਼ਾਨਾ);
                }
            }
        }
    }
    ਲਿਖੋ("}\n");
}

ਖਾਲੀ obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_ਮੰਜ਼ਿਲ_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    ਜੇ (isIRQActive(irq)) {
        ਲਿਖੋ("0x%lx_%lu_irq {\n",
#ਜੇ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)irq.irq,
#ਨਹੀਂਤਾਂ
               (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)irq,
#ਜੇ_ਅੰਤ
               (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_ਮੰਜ਼ਿਲ_IDX(irq)].cap;
        ਜੇ (cap_get_capType(ntfn_cap) != cap_null_cap) {
            ਲਿਖੋ("0x0: ");
            print_cap(ntfn_cap);
        }
        ਲਿਖੋ("}\n");
    }
}

ਖਾਲੀ print_objects(ਖਾਲੀ)
{
    ਲਈ (tcb_t *curr = ਗੰਢ_STATE(ksDebugTCBs); curr != ਨਿਰਾ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ਅਗਲਾ) {
        ਜੇ (ਜੜ੍ਹ_or_idle_tcb(curr)) {
            ਜਾਰੀ;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    ਲਈ (tcb_t *curr = ਗੰਢ_STATE(ksDebugTCBs); curr != ਨਿਰਾ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ਅਗਲਾ) {
        ਜੇ (ਜੜ੍ਹ_or_idle_tcb(curr)) {
            ਜਾਰੀ;
        }

        /* print the tcb as objects */
        ਲਿਖੋ("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        ਜੇ (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

ਖਾਲੀ print_caps(ਖਾਲੀ)
{
    ਲਈ (tcb_t *curr = ਗੰਢ_STATE(ksDebugTCBs); curr != ਨਿਰਾ; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ਅਗਲਾ) {
        ਜੇ (ਜੜ੍ਹ_or_idle_tcb(curr)) {
            ਜਾਰੀ;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

ਖਾਲੀ print_cap(cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_endpoint_cap: {
        ਲਿਖੋ("%p_ep ",
               (ਖਾਲੀ *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_notification_cap: {
        ਲਿਖੋ("%p_notification ",
               (ਖਾਲੀ *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_untyped_cap: {
        ਲਿਖੋ("%p_untyped\n",
               (ਖਾਲੀ *)cap_untyped_cap_get_capPtr(cap));
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_thread_cap: {
        ਲਿਖੋ("%p_tcb\n",
               (ਖਾਲੀ *)cap_thread_cap_get_capTCBPtr(cap));
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_cnode_cap: {
        ਲਿਖੋ("%p_cnode ",
               (ਖਾਲੀ *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        ਰੋਕੋ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_ਜਵਾਬ_cap: {
        ਲਿਖੋ("%p_reply\n",
               (ਖਾਲੀ *)cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Ptr(cap));
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_sched_context_cap: {
        ਲਿਖੋ("%p_sc\n",
               (ਖਾਲੀ *)cap_sched_context_cap_get_capSCPtr(cap));
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_sched_control_cap: {
        ਲਿਖੋ("%lu_sched_control\n",
               (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_sched_control_cap_get_core(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    ਹਾਲਤ cap_irq_control_cap: {
        ਲਿਖੋ("irq_control\n"); /* only one in the system */
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_irq_handler_cap: {
        ਲਿਖੋ("%p_%lu_irq\n",
               (ਖਾਲੀ *)cap_irq_handler_cap_get_capIRQ(cap),
               (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_CORE(IDX_ਮੰਜ਼ਿਲ_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ਰੋਕੋ;
    }
    ਮੂਲ: {
        print_cap_arch(cap);
        ਰੋਕੋ;
    }
    }
}

ਖਾਲੀ print_object(cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_endpoint_cap: {
        ਲਿਖੋ("%p_ep = ep\n",
               (ਖਾਲੀ *)cap_endpoint_cap_get_capEPPtr(cap));
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_notification_cap: {
        ਲਿਖੋ("%p_notification = notification\n",
               (ਖਾਲੀ *)cap_notification_cap_get_capNtfnPtr(cap));
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_cnode_cap: {
        ਪ੍ਰਤਿਜ੍ਞਾ(!"should not happend");
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ cap_ਜਵਾਬ_cap: {
        ਲਿਖੋ("%p_reply = rtreply\n",
               (ਖਾਲੀ *)cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Ptr(cap));
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_sched_context_cap: {
        ਲਿਖੋ("%p_sc = sc ",
               (ਖਾਲੀ *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    ਹਾਲਤ cap_irq_handler_cap: {
        ਲਿਖੋ("%p_%lu_irq = irq\n",
               (ਖਾਲੀ *)cap_irq_handler_cap_get_capIRQ(cap),
               (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)IRQT_ਮੰਜ਼ਿਲ_CORE(IDX_ਮੰਜ਼ਿਲ_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ਰੋਕੋ;
    }
    ਮੂਲ:
        print_object_arch(cap);
        ਰੋਕੋ;
    }
}

#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
