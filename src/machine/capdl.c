/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <machine/registerset.h>
#ضمن <machine/timer.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>
#که_تعریف_شوی CONFIG_KERNEL_MCS
#ضمن <kernel/sporadic.h>
#د_که_پای

#تعریف_کړه کتل_شوی_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t کتل_شوی_list[کتل_شوی_SZ];
صحيح watermark = 0;

فراغ جمع_کړه_منزل_کتل_شوی(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    إذا (watermark < کتل_شوی_SZ) {
        کتل_شوی_list[watermark] = c;
        watermark++;
    }
}

فراغ reset_کتل_شوی_list(فراغ)
{
    memset(کتل_شوی_list, 0, کتل_شوی_SZ * حجم(کتل_شوی_list[0]));
    watermark = 0;
}

bool_t کتل_شوی(cap_t c)
{
    لكل (صحيح i = 0; i < watermark; i++) {
        إذا (same_cap(کتل_شوی_list[i], c)) {
            أرجع true;
        }
    }
    أرجع false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    أرجع (a.ټکي[0] == b.ټکي[0] && a.ټکي[1] == b.ټکي[1]);
}

/* Return true if strings are the same */
ساكن مضمن bool_t مزي_ګانې_equal(ثابت حرف *str1, ثابت حرف *str2)
{
    طالما (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    أرجع !(*(ثابت بینښه حرف *)str1 - * (ثابت بینښه حرف *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t ریښه_or_idle_tcb(tcb_t *tcb)
{
    أرجع (مزي_ګانې_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || مزي_ګانې_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#که_تعریف_شوی CONFIG_PRINTING

فراغ obj_tcb_print_attrs(tcb_t *tcb)
{
    چاپ_کړه("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (اوږد بینښه صحيح)tcb->tcbIPCBuffer,
           (اوږد بینښه صحيح)getRestartPC(tcb),
           (اوږد بینښه صحيح)get_tcb_sp(tcb),
           (اوږد بینښه صحيح)tcb->tcb_لومړیتوب,
           (اوږد بینښه صحيح)tcb->tcbMCP);

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    چاپ_کړه(", affinity: %lu", (اوږد بینښه صحيح)tcb->tcbAffinity);
#د_که_پای /* ENABLE_SMP_SUPPORT */

    /* init */

#که_تعریف_شوی CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    إذا (cap_get_capType(ep_cap) != cap_null_cap) {
        چاپ_کړه(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#د_که_پای

    چاپ_کړه(", dom: %ld)\n", tcb->tcbDomain);
}

#که_تعریف_شوی CONFIG_KERNEL_MCS

ساكن مضمن ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t مجموعه_2 = refill_سر(sc)->rAmount;
    کلمه_t current = sc->sc_Refill_سر;

    طالما (current != sc->sc_Refill_لکۍ) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        مجموعه_2 += refill_شاخص(sc, current)->rAmount;
    }

    أرجع مجموعه_2;
}

فراغ obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    چاپ_کړه("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_کلمه" bits)\n",
           ticks_منزل_Us(period), period,
           ticks_منزل_Us(budget), budget,
           (کلمه_t)cap_sched_context_cap_get_cap_SCSize_بټونه(sc_cap));
}
#د_که_پای /* CONFIG_KERNEL_MCS */

فراغ obj_ut_print_attrs(cte_t *خونه, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    چاپ_کړه("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (فراغ *)cap_untyped_cap_get_capPtr(خونه->cap),
           (اوږد بینښه صحيح)cap_untyped_cap_get_cap_بلاک_اندازه(خونه->cap),
           (اوږد بینښه صحيح)cap_untyped_cap_get_cap_بلاک_اندازه(خونه->cap),
           کلمه_PTR(cap_untyped_cap_get_capPtr(خونه->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    لكل (cte_t *بل_Ptr = CTE_PTR(mdb_غوټه_get_mdb_بل(خونه->cteMDBNode));
         بل_Ptr && is_MDBParent_د(خونه, بل_Ptr);
         بل_Ptr = CTE_PTR(mdb_غوټه_get_mdb_بل(خونه->cteMDBNode))) {
        إذا (!sameRegionAs(خونه->cap, بل_Ptr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            print_cap(بل_Ptr->cap);
        }
    }
    چاپ_کړه("}\n");
}

فراغ obj_cnode_print_attrs(cap_t cnode)
{
    چاپ_کړه("(%lu bits)\n", (اوږد بینښه صحيح)cap_cnode_cap_get_capCNodeRadix(cnode));
}

فراغ obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    إذا (کتل_شوی(cnode)) {
        أرجع;
    }
    جمع_کړه_منزل_کتل_شوی(cnode);
    چاپ_کړه("%p_cnode = cnode ", (فراغ *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    کلمه_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_خونه_ret_t c = lookup_Cap_And_خونه(tcb, i);
        إذا (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.خونه, tcb);

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
    چاپ_کړه("(guard: %lu, guard_size: %lu)\n",
           (اوږد بینښه صحيح)cap_cnode_cap_get_capCNodeGuard(cnode),
           (اوږد بینښه صحيح)cap_cnode_cap_get_cap_CNode_Guard_اندازه(cnode));
}

فراغ cap_ep_print_attrs(cap_t ep)
{
    چاپ_کړه("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? توری_چاپ_کړه('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? توری_چاپ_کړه('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? توری_چاپ_کړه('G') : 0;
    cap_endpoint_cap_get_cap_Can_Grant_ځواب(ep) ? توری_چاپ_کړه('P') : 0;
    اوږد بینښه صحيح badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? چاپ_کړه(", badge: %lu)\n", badge) : چاپ_کړه(")\n");
}

فراغ cap_ntfn_print_attrs(cap_t ntfn)
{
    چاپ_کړه("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? توری_چاپ_کړه('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? توری_چاپ_کړه('W') : 0;
    اوږد بینښه صحيح badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? چاپ_کړه(", badge: %lu)\n", badge) : چاپ_کړه(")\n");
}

/*
 * print object slots
 */

فراغ obj_tcb_print_slots(tcb_t *tcb)
{
    چاپ_کړه("%p_tcb {\n", tcb);

    /* CSpace root */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        چاپ_کړه("cspace: %p_cnode ",
               (فراغ *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        چاپ_کړه("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_ریښه_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_بفر)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_بفر_خونه(tcb);
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        چاپ_کړه("fault_ep_slot: %p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    إذا (tcb->tcbSchedContext) {
        چاپ_کړه("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        چاپ_کړه("temp_fault_ep_slot: %p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# وإلا
    /* Reply cap slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_ځواب)->cap) != cap_null_cap) {
        چاپ_کړه("reply_slot: %p_reply\n",
               (فراغ *)cap_ځواب_cap_get_cap_TCBPtr(TCB_PTR_CTE_PTR(tcb, tcb_ځواب)->cap));
    }

    /* TCB of most recent IPC sender */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        چاپ_کړه("caller_slot: %p_tcb\n", caller);
    }
#د_که_پای /* CONFIG_KERNEL_MCS */
    چاپ_کړه("}\n");
}

/* TBD: deal with nested cnodes */
فراغ obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t ریښه = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    إذا (cap_get_capType(ریښه) != cap_cnode_cap) {
        أرجع;
    }

    کلمه_t radix = cap_cnode_cap_get_capCNodeRadix(ریښه);
    إذا (کتل_شوی(ریښه)) {
        أرجع;
    }
    جمع_کړه_منزل_کتل_شوی(ریښه);

    چاپ_کړه("%p_cnode {\n", (فراغ *)cap_cnode_cap_get_capCNodePtr(ریښه));

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_خونه_ret_t c = lookup_Cap_And_خونه(tcb, i);
        إذا (cap_get_capType(c.cap) != cap_null_cap) {
            چاپ_کړه("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    چاپ_کړه("}\n");

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_خونه_ret_t c = lookup_Cap_And_خونه(tcb, i);
        إذا (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

فراغ obj_irq_print_maps(فراغ)
{
    چاپ_کړه("irq maps {\n");

    لكل (se_L4_کلمه موخه = 0; موخه < CONFIG_MAX_عدد_NODES; موخه++) {
        لكل (بینښه i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_منزل_IRQT(موخه, i);
            إذا (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_منزل_IDX(irq)].cap;
                إذا (cap_get_capType(cap) != cap_null_cap) {
                    چاپ_کړه("%d: 0x%lx_%lu_irq\n",
                           i,
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (اوږد بینښه صحيح)irq.irq,
#وإلا
                           (اوږد بینښه صحيح)irq,
#د_که_پای
                           (اوږد بینښه صحيح)موخه);
                }
            }
        }
    }
    چاپ_کړه("}\n");
}

فراغ obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    إذا (isIRQActive(irq)) {
        چاپ_کړه("0x%lx_%lu_irq {\n",
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (اوږد بینښه صحيح)irq.irq,
#وإلا
               (اوږد بینښه صحيح)irq,
#د_که_پای
               (اوږد بینښه صحيح)IRQT_منزل_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_منزل_IDX(irq)].cap;
        إذا (cap_get_capType(ntfn_cap) != cap_null_cap) {
            چاپ_کړه("0x0: ");
            print_cap(ntfn_cap);
        }
        چاپ_کړه("}\n");
    }
}

فراغ print_objects(فراغ)
{
    لكل (tcb_t *curr = غوټه_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بل) {
        إذا (ریښه_or_idle_tcb(curr)) {
            استمر;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    لكل (tcb_t *curr = غوټه_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بل) {
        إذا (ریښه_or_idle_tcb(curr)) {
            استمر;
        }

        /* print the tcb as objects */
        چاپ_کړه("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        إذا (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

فراغ print_caps(فراغ)
{
    لكل (tcb_t *curr = غوټه_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بل) {
        إذا (ریښه_or_idle_tcb(curr)) {
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
        چاپ_کړه("%p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        اكسر;
    }
    حالة cap_notification_cap: {
        چاپ_کړه("%p_notification ",
               (فراغ *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        اكسر;
    }
    حالة cap_untyped_cap: {
        چاپ_کړه("%p_untyped\n",
               (فراغ *)cap_untyped_cap_get_capPtr(cap));
        اكسر;
    }
    حالة cap_thread_cap: {
        چاپ_کړه("%p_tcb\n",
               (فراغ *)cap_thread_cap_get_capTCBPtr(cap));
        اكسر;
    }
    حالة cap_cnode_cap: {
        چاپ_کړه("%p_cnode ",
               (فراغ *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        اكسر;
    }
#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_ځواب_cap: {
        چاپ_کړه("%p_reply\n",
               (فراغ *)cap_ځواب_cap_get_cap_ځواب_Ptr(cap));
        اكسر;
    }
    حالة cap_sched_context_cap: {
        چاپ_کړه("%p_sc\n",
               (فراغ *)cap_sched_context_cap_get_capSCPtr(cap));
        اكسر;
    }
    حالة cap_sched_control_cap: {
        چاپ_کړه("%lu_sched_control\n",
               (اوږد بینښه صحيح)cap_sched_control_cap_get_core(cap));
        اكسر;
    }
#د_که_پای
    حالة cap_irq_control_cap: {
        چاپ_کړه("irq_control\n"); /* only one in the system */
        اكسر;
    }
    حالة cap_irq_handler_cap: {
        چاپ_کړه("%p_%lu_irq\n",
               (فراغ *)cap_irq_handler_cap_get_capIRQ(cap),
               (اوږد بینښه صحيح)IRQT_منزل_CORE(IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
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
        چاپ_کړه("%p_ep = ep\n",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(cap));
        اكسر;
    }
    حالة cap_notification_cap: {
        چاپ_کړه("%p_notification = notification\n",
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
#که_تعریف_شوی CONFIG_KERNEL_MCS
    حالة cap_ځواب_cap: {
        چاپ_کړه("%p_reply = rtreply\n",
               (فراغ *)cap_ځواب_cap_get_cap_ځواب_Ptr(cap));
        اكسر;
    }
    حالة cap_sched_context_cap: {
        چاپ_کړه("%p_sc = sc ",
               (فراغ *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        اكسر;
    }
#د_که_پای
    حالة cap_irq_handler_cap: {
        چاپ_کړه("%p_%lu_irq = irq\n",
               (فراغ *)cap_irq_handler_cap_get_capIRQ(cap),
               (اوږد بینښه صحيح)IRQT_منزل_CORE(IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        اكسر;
    }
    افتراضي:
        print_object_arch(cap);
        اكسر;
    }
}

#د_که_پای /* CONFIG_PRINTING */

#د_که_پای /* CONFIG_DEBUG_BUILD */
