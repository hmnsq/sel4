/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <machine/registerset.h>
#ضمن <machine/timer.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#ضمن <kernel/sporadic.h>
#جيڪڏهن_پورو

#وصف_ڪريو ڏٺل_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t ڏٺل_list[ڏٺل_SZ];
صحيح watermark = 0;

فراغ جوڙيو_منزل_ڏٺل(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    إذا (watermark < ڏٺل_SZ) {
        ڏٺل_list[watermark] = c;
        watermark++;
    }
}

فراغ reset_ڏٺل_list(فراغ)
{
    memset(ڏٺل_list, 0, ڏٺل_SZ * حجم(ڏٺل_list[0]));
    watermark = 0;
}

bool_t ڏٺل(cap_t c)
{
    لكل (صحيح i = 0; i < watermark; i++) {
        إذا (same_cap(ڏٺل_list[i], c)) {
            أرجع true;
        }
    }
    أرجع false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    أرجع (a.لفظ[0] == b.لفظ[0] && a.لفظ[1] == b.لفظ[1]);
}

/* Return true if strings are the same */
ساكن مضمن bool_t لڙيون_equal(ثابت حرف *str1, ثابت حرف *str2)
{
    طالما (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    أرجع !(*(ثابت بنانشان حرف *)str1 - * (ثابت بنانشان حرف *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t پاڙ_or_idle_tcb(tcb_t *tcb)
{
    أرجع (لڙيون_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || لڙيون_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING

فراغ obj_tcb_print_attrs(tcb_t *tcb)
{
    لکو("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (ڊگهو بنانشان صحيح)tcb->tcbIPCBuffer,
           (ڊگهو بنانشان صحيح)getRestartPC(tcb),
           (ڊگهو بنانشان صحيح)get_tcb_sp(tcb),
           (ڊگهو بنانشان صحيح)tcb->tcb_ترجيح,
           (ڊگهو بنانشان صحيح)tcb->tcbMCP);

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    لکو(", affinity: %lu", (ڊگهو بنانشان صحيح)tcb->tcbAffinity);
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    /* init */

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    إذا (cap_get_capType(ep_cap) != cap_null_cap) {
        لکو(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#جيڪڏهن_پورو

    لکو(", dom: %ld)\n", tcb->tcbDomain);
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS

ساكن مضمن ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t جوڙ_2 = refill_سر(sc)->rAmount;
    لفظ_t current = sc->sc_Refill_سر;

    طالما (current != sc->sc_Refill_پڇ) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        جوڙ_2 += refill_اشاريو(sc, current)->rAmount;
    }

    أرجع جوڙ_2;
}

فراغ obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    لکو("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_لفظ" bits)\n",
           ticks_منزل_Us(period), period,
           ticks_منزل_Us(budget), budget,
           (لفظ_t)cap_sched_context_cap_get_cap_SCSize_بٽ(sc_cap));
}
#جيڪڏهن_پورو /* CONFIG_KERNEL_MCS */

فراغ obj_ut_print_attrs(cte_t *جڳهه, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    لکو("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (فراغ *)cap_untyped_cap_get_capPtr(جڳهه->cap),
           (ڊگهو بنانشان صحيح)cap_untyped_cap_get_cap_بلاڪ_سائيز(جڳهه->cap),
           (ڊگهو بنانشان صحيح)cap_untyped_cap_get_cap_بلاڪ_سائيز(جڳهه->cap),
           لفظ_PTR(cap_untyped_cap_get_capPtr(جڳهه->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    لكل (cte_t *ايندڙ_Ptr = CTE_PTR(mdb_ڳنڍ_get_mdb_ايندڙ(جڳهه->cteMDBNode));
         ايندڙ_Ptr && is_MDBParent_جو(جڳهه, ايندڙ_Ptr);
         ايندڙ_Ptr = CTE_PTR(mdb_ڳنڍ_get_mdb_ايندڙ(جڳهه->cteMDBNode))) {
        إذا (!sameRegionAs(جڳهه->cap, ايندڙ_Ptr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            print_cap(ايندڙ_Ptr->cap);
        }
    }
    لکو("}\n");
}

فراغ obj_cnode_print_attrs(cap_t cnode)
{
    لکو("(%lu bits)\n", (ڊگهو بنانشان صحيح)cap_cnode_cap_get_capCNodeRadix(cnode));
}

فراغ obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    إذا (ڏٺل(cnode)) {
        أرجع;
    }
    جوڙيو_منزل_ڏٺل(cnode);
    لکو("%p_cnode = cnode ", (فراغ *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    لفظ_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_جڳهه_ret_t c = lookup_Cap_And_جڳهه(tcb, i);
        إذا (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.جڳهه, tcb);

        } وإلا إذا (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } وإلا إذا (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

فراغ cap_cnode_print_attrs(cap_t cnode)
{
    لکو("(guard: %lu, guard_size: %lu)\n",
           (ڊگهو بنانشان صحيح)cap_cnode_cap_get_capCNodeGuard(cnode),
           (ڊگهو بنانشان صحيح)cap_cnode_cap_get_cap_CNode_Guard_سائيز(cnode));
}

فراغ cap_ep_print_attrs(cap_t ep)
{
    لکو("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? اکر_لکو('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? اکر_لکو('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? اکر_لکو('G') : 0;
    cap_endpoint_cap_get_cap_Can_Grant_جواب(ep) ? اکر_لکو('P') : 0;
    ڊگهو بنانشان صحيح badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? لکو(", badge: %lu)\n", badge) : لکو(")\n");
}

فراغ cap_ntfn_print_attrs(cap_t ntfn)
{
    لکو("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? اکر_لکو('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? اکر_لکو('W') : 0;
    ڊگهو بنانشان صحيح badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? لکو(", badge: %lu)\n", badge) : لکو(")\n");
}

/*
 * print object slots
 */

فراغ obj_tcb_print_slots(tcb_t *tcb)
{
    لکو("%p_tcb {\n", tcb);

    /* CSpace root */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        لکو("cspace: %p_cnode ",
               (فراغ *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        لکو("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_پاڙ_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_بفر)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_بفر_جڳهه(tcb);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        لکو("fault_ep_slot: %p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    إذا (tcb->tcbSchedContext) {
        لکو("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        لکو("temp_fault_ep_slot: %p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# وإلا
    /* Reply cap slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_جواب)->cap) != cap_null_cap) {
        لکو("reply_slot: %p_reply\n",
               (فراغ *)cap_جواب_cap_get_cap_TCBPtr(TCB_PTR_CTE_PTR(tcb, tcb_جواب)->cap));
    }

    /* TCB of most recent IPC sender */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        لکو("caller_slot: %p_tcb\n", caller);
    }
#جيڪڏهن_پورو /* CONFIG_KERNEL_MCS */
    لکو("}\n");
}

/* TBD: deal with nested cnodes */
فراغ obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t پاڙ = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    إذا (cap_get_capType(پاڙ) != cap_cnode_cap) {
        أرجع;
    }

    لفظ_t radix = cap_cnode_cap_get_capCNodeRadix(پاڙ);
    إذا (ڏٺل(پاڙ)) {
        أرجع;
    }
    جوڙيو_منزل_ڏٺل(پاڙ);

    لکو("%p_cnode {\n", (فراغ *)cap_cnode_cap_get_capCNodePtr(پاڙ));

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_جڳهه_ret_t c = lookup_Cap_And_جڳهه(tcb, i);
        إذا (cap_get_capType(c.cap) != cap_null_cap) {
            لکو("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    لکو("}\n");

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_جڳهه_ret_t c = lookup_Cap_And_جڳهه(tcb, i);
        إذا (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

فراغ obj_irq_print_maps(فراغ)
{
    لکو("irq maps {\n");

    لكل (se_L4_لفظ هدف = 0; هدف < CONFIG_MAX_انگ_NODES; هدف++) {
        لكل (بنانشان i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_منزل_IRQT(هدف, i);
            إذا (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_منزل_IDX(irq)].cap;
                إذا (cap_get_capType(cap) != cap_null_cap) {
                    لکو("%d: 0x%lx_%lu_irq\n",
                           i,
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (ڊگهو بنانشان صحيح)irq.irq,
#وإلا
                           (ڊگهو بنانشان صحيح)irq,
#جيڪڏهن_پورو
                           (ڊگهو بنانشان صحيح)هدف);
                }
            }
        }
    }
    لکو("}\n");
}

فراغ obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    إذا (isIRQActive(irq)) {
        لکو("0x%lx_%lu_irq {\n",
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (ڊگهو بنانشان صحيح)irq.irq,
#وإلا
               (ڊگهو بنانشان صحيح)irq,
#جيڪڏهن_پورو
               (ڊگهو بنانشان صحيح)IRQT_منزل_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_منزل_IDX(irq)].cap;
        إذا (cap_get_capType(ntfn_cap) != cap_null_cap) {
            لکو("0x0: ");
            print_cap(ntfn_cap);
        }
        لکو("}\n");
    }
}

فراغ print_objects(فراغ)
{
    لكل (tcb_t *curr = ڳنڍ_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ايندڙ) {
        إذا (پاڙ_or_idle_tcb(curr)) {
            استمر;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    لكل (tcb_t *curr = ڳنڍ_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ايندڙ) {
        إذا (پاڙ_or_idle_tcb(curr)) {
            استمر;
        }

        /* print the tcb as objects */
        لکو("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        إذا (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

فراغ print_caps(فراغ)
{
    لكل (tcb_t *curr = ڳنڍ_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_ايندڙ) {
        إذا (پاڙ_or_idle_tcb(curr)) {
            استمر;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

فراغ print_cap(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap: {
        لکو("%p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        اكسر;
    }
    حالة cap_notification_cap: {
        لکو("%p_notification ",
               (فراغ *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        اكسر;
    }
    حالة cap_untyped_cap: {
        لکو("%p_untyped\n",
               (فراغ *)cap_untyped_cap_get_capPtr(cap));
        اكسر;
    }
    حالة cap_thread_cap: {
        لکو("%p_tcb\n",
               (فراغ *)cap_thread_cap_get_capTCBPtr(cap));
        اكسر;
    }
    حالة cap_cnode_cap: {
        لکو("%p_cnode ",
               (فراغ *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        اكسر;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_جواب_cap: {
        لکو("%p_reply\n",
               (فراغ *)cap_جواب_cap_get_cap_جواب_Ptr(cap));
        اكسر;
    }
    حالة cap_sched_context_cap: {
        لکو("%p_sc\n",
               (فراغ *)cap_sched_context_cap_get_capSCPtr(cap));
        اكسر;
    }
    حالة cap_sched_control_cap: {
        لکو("%lu_sched_control\n",
               (ڊگهو بنانشان صحيح)cap_sched_control_cap_get_core(cap));
        اكسر;
    }
#جيڪڏهن_پورو
    حالة cap_irq_control_cap: {
        لکو("irq_control\n"); /* only one in the system */
        اكسر;
    }
    حالة cap_irq_handler_cap: {
        لکو("%p_%lu_irq\n",
               (فراغ *)cap_irq_handler_cap_get_capIRQ(cap),
               (ڊگهو بنانشان صحيح)IRQT_منزل_CORE(IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        اكسر;
    }
    افتراضي: {
        print_cap_arch(cap);
        اكسر;
    }
    }
}

فراغ print_object(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap: {
        لکو("%p_ep = ep\n",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(cap));
        اكسر;
    }
    حالة cap_notification_cap: {
        لکو("%p_notification = notification\n",
               (فراغ *)cap_notification_cap_get_capNtfnPtr(cap));
        اكسر;
    }
    حالة cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        اكسر;
    }
    حالة cap_cnode_cap: {
        أكد(!"should not happend");
    }
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    حالة cap_جواب_cap: {
        لکو("%p_reply = rtreply\n",
               (فراغ *)cap_جواب_cap_get_cap_جواب_Ptr(cap));
        اكسر;
    }
    حالة cap_sched_context_cap: {
        لکو("%p_sc = sc ",
               (فراغ *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        اكسر;
    }
#جيڪڏهن_پورو
    حالة cap_irq_handler_cap: {
        لکو("%p_%lu_irq = irq\n",
               (فراغ *)cap_irq_handler_cap_get_capIRQ(cap),
               (ڊگهو بنانشان صحيح)IRQT_منزل_CORE(IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        اكسر;
    }
    افتراضي:
        print_object_arch(cap);
        اكسر;
    }
}

#جيڪڏهن_پورو /* CONFIG_PRINTING */

#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
