/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <machine/registerset.h>
#ضمن <machine/timer.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>
#اگر_تعریف_شده CONFIG_KERNEL_MCS
#ضمن <kernel/sporadic.h>
#پایان_اگر

#تعریف_کن دیده_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t دیده_list[دیده_SZ];
صحيح watermark = 0;

فراغ جمع_کن_مقصد_دیده(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    إذا (watermark < دیده_SZ) {
        دیده_list[watermark] = c;
        watermark++;
    }
}

فراغ reset_دیده_list(فراغ)
{
    memset(دیده_list, 0, دیده_SZ * حجم(دیده_list[0]));
    watermark = 0;
}

bool_t دیده(cap_t c)
{
    لكل (صحيح i = 0; i < watermark; i++) {
        إذا (same_cap(دیده_list[i], c)) {
            أرجع true;
        }
    }
    أرجع false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    أرجع (a.واژگان[0] == b.واژگان[0] && a.واژگان[1] == b.واژگان[1]);
}

/* Return true if strings are the same */
ساكن مضمن bool_t رشتهها_equal(ثابت حرف *str1, ثابت حرف *str2)
{
    طالما (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    أرجع !(*(ثابت بیعلامت حرف *)str1 - * (ثابت بیعلامت حرف *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t ریشه_or_idle_tcb(tcb_t *tcb)
{
    أرجع (رشتهها_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || رشتهها_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#اگر_تعریف_شده CONFIG_PRINTING

فراغ obj_tcb_print_attrs(tcb_t *tcb)
{
    چاپ_کن("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (دراز بیعلامت صحيح)tcb->tcbIPCBuffer,
           (دراز بیعلامت صحيح)getRestartPC(tcb),
           (دراز بیعلامت صحيح)get_tcb_sp(tcb),
           (دراز بیعلامت صحيح)tcb->tcb_اولویت,
           (دراز بیعلامت صحيح)tcb->tcbMCP);

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    چاپ_کن(", affinity: %lu", (دراز بیعلامت صحيح)tcb->tcbAffinity);
#پایان_اگر /* ENABLE_SMP_SUPPORT */

    /* init */

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    إذا (cap_get_capType(ep_cap) != cap_null_cap) {
        چاپ_کن(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#پایان_اگر

    چاپ_کن(", dom: %ld)\n", tcb->tcbDomain);
}

#اگر_تعریف_شده CONFIG_KERNEL_MCS

ساكن مضمن ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t جمع_2 = refill_سر(sc)->rAmount;
    کلمه_t current = sc->sc_Refill_سر;

    طالما (current != sc->sc_Refill_دم) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        جمع_2 += refill_شاخص(sc, current)->rAmount;
    }

    أرجع جمع_2;
}

فراغ obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    چاپ_کن("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_کلمه" bits)\n",
           ticks_مقصد_Us(period), period,
           ticks_مقصد_Us(budget), budget,
           (کلمه_t)cap_sched_context_cap_get_cap_SCSize_بیتها(sc_cap));
}
#پایان_اگر /* CONFIG_KERNEL_MCS */

فراغ obj_ut_print_attrs(cte_t *جایگاه, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    چاپ_کن("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (فراغ *)cap_untyped_cap_get_capPtr(جایگاه->cap),
           (دراز بیعلامت صحيح)cap_untyped_cap_get_cap_بلوک_اندازه(جایگاه->cap),
           (دراز بیعلامت صحيح)cap_untyped_cap_get_cap_بلوک_اندازه(جایگاه->cap),
           کلمه_PTR(cap_untyped_cap_get_capPtr(جایگاه->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    لكل (cte_t *بعدی_Ptr = CTE_PTR(mdb_گره_get_mdb_بعدی(جایگاه->cteMDBNode));
         بعدی_Ptr && is_MDBParent_از(جایگاه, بعدی_Ptr);
         بعدی_Ptr = CTE_PTR(mdb_گره_get_mdb_بعدی(جایگاه->cteMDBNode))) {
        إذا (!sameRegionAs(جایگاه->cap, بعدی_Ptr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            print_cap(بعدی_Ptr->cap);
        }
    }
    چاپ_کن("}\n");
}

فراغ obj_cnode_print_attrs(cap_t cnode)
{
    چاپ_کن("(%lu bits)\n", (دراز بیعلامت صحيح)cap_cnode_cap_get_capCNodeRadix(cnode));
}

فراغ obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    إذا (دیده(cnode)) {
        أرجع;
    }
    جمع_کن_مقصد_دیده(cnode);
    چاپ_کن("%p_cnode = cnode ", (فراغ *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    کلمه_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_جایگاه_ret_t c = lookup_Cap_And_جایگاه(tcb, i);
        إذا (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.جایگاه, tcb);

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
    چاپ_کن("(guard: %lu, guard_size: %lu)\n",
           (دراز بیعلامت صحيح)cap_cnode_cap_get_capCNodeGuard(cnode),
           (دراز بیعلامت صحيح)cap_cnode_cap_get_cap_CNode_Guard_اندازه(cnode));
}

فراغ cap_ep_print_attrs(cap_t ep)
{
    چاپ_کن("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? حرف_چاپ_کن('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? حرف_چاپ_کن('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? حرف_چاپ_کن('G') : 0;
    cap_endpoint_cap_get_cap_Can_Grant_پاسخ(ep) ? حرف_چاپ_کن('P') : 0;
    دراز بیعلامت صحيح badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? چاپ_کن(", badge: %lu)\n", badge) : چاپ_کن(")\n");
}

فراغ cap_ntfn_print_attrs(cap_t ntfn)
{
    چاپ_کن("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? حرف_چاپ_کن('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? حرف_چاپ_کن('W') : 0;
    دراز بیعلامت صحيح badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? چاپ_کن(", badge: %lu)\n", badge) : چاپ_کن(")\n");
}

/*
 * print object slots
 */

فراغ obj_tcb_print_slots(tcb_t *tcb)
{
    چاپ_کن("%p_tcb {\n", tcb);

    /* CSpace root */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        چاپ_کن("cspace: %p_cnode ",
               (فراغ *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        چاپ_کن("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_ریشه_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_میانگیر)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_میانگیر_جایگاه(tcb);
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        چاپ_کن("fault_ep_slot: %p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    إذا (tcb->tcbSchedContext) {
        چاپ_کن("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        چاپ_کن("temp_fault_ep_slot: %p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# وإلا
    /* Reply cap slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_پاسخ)->cap) != cap_null_cap) {
        چاپ_کن("reply_slot: %p_reply\n",
               (فراغ *)cap_پاسخ_cap_get_cap_TCBPtr(TCB_PTR_CTE_PTR(tcb, tcb_پاسخ)->cap));
    }

    /* TCB of most recent IPC sender */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        چاپ_کن("caller_slot: %p_tcb\n", caller);
    }
#پایان_اگر /* CONFIG_KERNEL_MCS */
    چاپ_کن("}\n");
}

/* TBD: deal with nested cnodes */
فراغ obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t ریشه = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    إذا (cap_get_capType(ریشه) != cap_cnode_cap) {
        أرجع;
    }

    کلمه_t radix = cap_cnode_cap_get_capCNodeRadix(ریشه);
    إذا (دیده(ریشه)) {
        أرجع;
    }
    جمع_کن_مقصد_دیده(ریشه);

    چاپ_کن("%p_cnode {\n", (فراغ *)cap_cnode_cap_get_capCNodePtr(ریشه));

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_جایگاه_ret_t c = lookup_Cap_And_جایگاه(tcb, i);
        إذا (cap_get_capType(c.cap) != cap_null_cap) {
            چاپ_کن("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    چاپ_کن("}\n");

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_جایگاه_ret_t c = lookup_Cap_And_جایگاه(tcb, i);
        إذا (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

فراغ obj_irq_print_maps(فراغ)
{
    چاپ_کن("irq maps {\n");

    لكل (se_L4_کلمه هدف = 0; هدف < CONFIG_MAX_رقم_NODES; هدف++) {
        لكل (بیعلامت i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_مقصد_IRQT(هدف, i);
            إذا (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_مقصد_IDX(irq)].cap;
                إذا (cap_get_capType(cap) != cap_null_cap) {
                    چاپ_کن("%d: 0x%lx_%lu_irq\n",
                           i,
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (دراز بیعلامت صحيح)irq.irq,
#وإلا
                           (دراز بیعلامت صحيح)irq,
#پایان_اگر
                           (دراز بیعلامت صحيح)هدف);
                }
            }
        }
    }
    چاپ_کن("}\n");
}

فراغ obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_مقصد_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    إذا (isIRQActive(irq)) {
        چاپ_کن("0x%lx_%lu_irq {\n",
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (دراز بیعلامت صحيح)irq.irq,
#وإلا
               (دراز بیعلامت صحيح)irq,
#پایان_اگر
               (دراز بیعلامت صحيح)IRQT_مقصد_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_مقصد_IDX(irq)].cap;
        إذا (cap_get_capType(ntfn_cap) != cap_null_cap) {
            چاپ_کن("0x0: ");
            print_cap(ntfn_cap);
        }
        چاپ_کن("}\n");
    }
}

فراغ print_objects(فراغ)
{
    لكل (tcb_t *curr = گره_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بعدی) {
        إذا (ریشه_or_idle_tcb(curr)) {
            استمر;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    لكل (tcb_t *curr = گره_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بعدی) {
        إذا (ریشه_or_idle_tcb(curr)) {
            استمر;
        }

        /* print the tcb as objects */
        چاپ_کن("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        إذا (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

فراغ print_caps(فراغ)
{
    لكل (tcb_t *curr = گره_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_بعدی) {
        إذا (ریشه_or_idle_tcb(curr)) {
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
        چاپ_کن("%p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        اكسر;
    }
    حالة cap_notification_cap: {
        چاپ_کن("%p_notification ",
               (فراغ *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        اكسر;
    }
    حالة cap_untyped_cap: {
        چاپ_کن("%p_untyped\n",
               (فراغ *)cap_untyped_cap_get_capPtr(cap));
        اكسر;
    }
    حالة cap_thread_cap: {
        چاپ_کن("%p_tcb\n",
               (فراغ *)cap_thread_cap_get_capTCBPtr(cap));
        اكسر;
    }
    حالة cap_cnode_cap: {
        چاپ_کن("%p_cnode ",
               (فراغ *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        اكسر;
    }
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_پاسخ_cap: {
        چاپ_کن("%p_reply\n",
               (فراغ *)cap_پاسخ_cap_get_cap_پاسخ_Ptr(cap));
        اكسر;
    }
    حالة cap_sched_context_cap: {
        چاپ_کن("%p_sc\n",
               (فراغ *)cap_sched_context_cap_get_capSCPtr(cap));
        اكسر;
    }
    حالة cap_sched_control_cap: {
        چاپ_کن("%lu_sched_control\n",
               (دراز بیعلامت صحيح)cap_sched_control_cap_get_core(cap));
        اكسر;
    }
#پایان_اگر
    حالة cap_irq_control_cap: {
        چاپ_کن("irq_control\n"); /* only one in the system */
        اكسر;
    }
    حالة cap_irq_handler_cap: {
        چاپ_کن("%p_%lu_irq\n",
               (فراغ *)cap_irq_handler_cap_get_capIRQ(cap),
               (دراز بیعلامت صحيح)IRQT_مقصد_CORE(IDX_مقصد_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
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
        چاپ_کن("%p_ep = ep\n",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(cap));
        اكسر;
    }
    حالة cap_notification_cap: {
        چاپ_کن("%p_notification = notification\n",
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
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة cap_پاسخ_cap: {
        چاپ_کن("%p_reply = rtreply\n",
               (فراغ *)cap_پاسخ_cap_get_cap_پاسخ_Ptr(cap));
        اكسر;
    }
    حالة cap_sched_context_cap: {
        چاپ_کن("%p_sc = sc ",
               (فراغ *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        اكسر;
    }
#پایان_اگر
    حالة cap_irq_handler_cap: {
        چاپ_کن("%p_%lu_irq = irq\n",
               (فراغ *)cap_irq_handler_cap_get_capIRQ(cap),
               (دراز بیعلامت صحيح)IRQT_مقصد_CORE(IDX_مقصد_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        اكسر;
    }
    افتراضي:
        print_object_arch(cap);
        اكسر;
    }
}

#پایان_اگر /* CONFIG_PRINTING */

#پایان_اگر /* CONFIG_DEBUG_BUILD */
