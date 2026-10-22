/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD

#شامل <machine/capdl.h>
#شامل <machine/registerset.h>
#شامل <machine/timer.h>
#شامل <string.h>
#شامل <kernel/cspace.h>
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#شامل <kernel/sporadic.h>
#اگر_ختم

#تعریف_کرو نظر_آیا_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t نظر_آیا_list[نظر_آیا_SZ];
صحیح watermark = 0;

خالی جوڑو_مقصد_نظر_آیا(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    اگر (watermark < نظر_آیا_SZ) {
        نظر_آیا_list[watermark] = c;
        watermark++;
    }
}

خالی reset_نظر_آیا_list(خالی)
{
    memset(نظر_آیا_list, 0, نظر_آیا_SZ * حجم(نظر_آیا_list[0]));
    watermark = 0;
}

bool_t نظر_آیا(cap_t c)
{
    برائے (صحیح i = 0; i < watermark; i++) {
        اگر (same_cap(نظر_آیا_list[i], c)) {
            واپس true;
        }
    }
    واپس false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    واپس (a.الفاظ[0] == b.الفاظ[0] && a.الفاظ[1] == b.الفاظ[1]);
}

/* Return true if strings are the same */
ساکن اندرونی bool_t لڑیاں_equal(ثابت حرف *str1, ثابت حرف *str2)
{
    جب (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    واپس !(*(ثابت غیرعلامتی حرف *)str1 - * (ثابت غیرعلامتی حرف *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t جڑ_or_idle_tcb(tcb_t *tcb)
{
    واپس (لڑیاں_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || لڑیاں_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#اگر_تعریف_شدہ CONFIG_PRINTING

خالی obj_tcb_print_attrs(tcb_t *tcb)
{
    لکھو("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (طویل غیرعلامتی صحیح)tcb->tcbIPCBuffer,
           (طویل غیرعلامتی صحیح)getRestartPC(tcb),
           (طویل غیرعلامتی صحیح)get_tcb_sp(tcb),
           (طویل غیرعلامتی صحیح)tcb->tcb_ترجیح,
           (طویل غیرعلامتی صحیح)tcb->tcbMCP);

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    لکھو(", affinity: %lu", (طویل غیرعلامتی صحیح)tcb->tcbAffinity);
#اگر_ختم /* ENABLE_SMP_SUPPORT */

    /* init */

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    اگر (cap_get_capType(ep_cap) != cap_null_cap) {
        لکھو(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#اگر_ختم

    لکھو(", dom: %ld)\n", tcb->tcbDomain);
}

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS

ساکن اندرونی ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t مجموع_2 = refill_سر(sc)->rAmount;
    لفظ_t current = sc->sc_Refill_سر;

    جب (current != sc->sc_Refill_دم) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        مجموع_2 += refill_اشاریہ(sc, current)->rAmount;
    }

    واپس مجموع_2;
}

خالی obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    لکھو("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_لفظ" bits)\n",
           ticks_مقصد_Us(period), period,
           ticks_مقصد_Us(budget), budget,
           (لفظ_t)cap_sched_context_cap_get_cap_SCSize_بٹ(sc_cap));
}
#اگر_ختم /* CONFIG_KERNEL_MCS */

خالی obj_ut_print_attrs(cte_t *خانہ, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    لکھو("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (خالی *)cap_untyped_cap_get_capPtr(خانہ->cap),
           (طویل غیرعلامتی صحیح)cap_untyped_cap_get_cap_بلاک_سائز(خانہ->cap),
           (طویل غیرعلامتی صحیح)cap_untyped_cap_get_cap_بلاک_سائز(خانہ->cap),
           لفظ_PTR(cap_untyped_cap_get_capPtr(خانہ->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    برائے (cte_t *اگلا_Ptr = CTE_PTR(mdb_گرہ_get_mdb_اگلا(خانہ->cteMDBNode));
         اگلا_Ptr && is_MDBParent_کا(خانہ, اگلا_Ptr);
         اگلا_Ptr = CTE_PTR(mdb_گرہ_get_mdb_اگلا(خانہ->cteMDBNode))) {
        اگر (!sameRegionAs(خانہ->cap, اگلا_Ptr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            print_cap(اگلا_Ptr->cap);
        }
    }
    لکھو("}\n");
}

خالی obj_cnode_print_attrs(cap_t cnode)
{
    لکھو("(%lu bits)\n", (طویل غیرعلامتی صحیح)cap_cnode_cap_get_capCNodeRadix(cnode));
}

خالی obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    اگر (نظر_آیا(cnode)) {
        واپس;
    }
    جوڑو_مقصد_نظر_آیا(cnode);
    لکھو("%p_cnode = cnode ", (خالی *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    لفظ_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    برائے (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_خانہ_ret_t c = lookup_Cap_And_خانہ(tcb, i);
        اگر (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.خانہ, tcb);

        } ورنہ اگر (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } ورنہ اگر (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

خالی cap_cnode_print_attrs(cap_t cnode)
{
    لکھو("(guard: %lu, guard_size: %lu)\n",
           (طویل غیرعلامتی صحیح)cap_cnode_cap_get_capCNodeGuard(cnode),
           (طویل غیرعلامتی صحیح)cap_cnode_cap_get_cap_CNode_Guard_سائز(cnode));
}

خالی cap_ep_print_attrs(cap_t ep)
{
    لکھو("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? حرف_لکھو('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? حرف_لکھو('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? حرف_لکھو('G') : 0;
    cap_endpoint_cap_get_cap_Can_Grant_جواب(ep) ? حرف_لکھو('P') : 0;
    طویل غیرعلامتی صحیح badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? لکھو(", badge: %lu)\n", badge) : لکھو(")\n");
}

خالی cap_ntfn_print_attrs(cap_t ntfn)
{
    لکھو("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? حرف_لکھو('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? حرف_لکھو('W') : 0;
    طویل غیرعلامتی صحیح badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? لکھو(", badge: %lu)\n", badge) : لکھو(")\n");
}

/*
 * print object slots
 */

خالی obj_tcb_print_slots(tcb_t *tcb)
{
    لکھو("%p_tcb {\n", tcb);

    /* CSpace root */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        لکھو("cspace: %p_cnode ",
               (خالی *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        لکھو("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_جڑ_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_بفر)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_بفر_خانہ(tcb);
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        لکھو("fault_ep_slot: %p_ep ",
               (خالی *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    اگر (tcb->tcbSchedContext) {
        لکھو("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        لکھو("temp_fault_ep_slot: %p_ep ",
               (خالی *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# ورنہ
    /* Reply cap slot */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_جواب)->cap) != cap_null_cap) {
        لکھو("reply_slot: %p_reply\n",
               (خالی *)cap_جواب_cap_get_cap_TCBPtr(TCB_PTR_CTE_PTR(tcb, tcb_جواب)->cap));
    }

    /* TCB of most recent IPC sender */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        لکھو("caller_slot: %p_tcb\n", caller);
    }
#اگر_ختم /* CONFIG_KERNEL_MCS */
    لکھو("}\n");
}

/* TBD: deal with nested cnodes */
خالی obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t جڑ = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    اگر (cap_get_capType(جڑ) != cap_cnode_cap) {
        واپس;
    }

    لفظ_t radix = cap_cnode_cap_get_capCNodeRadix(جڑ);
    اگر (نظر_آیا(جڑ)) {
        واپس;
    }
    جوڑو_مقصد_نظر_آیا(جڑ);

    لکھو("%p_cnode {\n", (خالی *)cap_cnode_cap_get_capCNodePtr(جڑ));

    برائے (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_خانہ_ret_t c = lookup_Cap_And_خانہ(tcb, i);
        اگر (cap_get_capType(c.cap) != cap_null_cap) {
            لکھو("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    لکھو("}\n");

    برائے (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_خانہ_ret_t c = lookup_Cap_And_خانہ(tcb, i);
        اگر (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

خالی obj_irq_print_maps(خالی)
{
    لکھو("irq maps {\n");

    برائے (se_L4_لفظ ہدف = 0; ہدف < CONFIG_MAX_عدد_NODES; ہدف++) {
        برائے (غیرعلامتی i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_مقصد_IRQT(ہدف, i);
            اگر (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_مقصد_IDX(irq)].cap;
                اگر (cap_get_capType(cap) != cap_null_cap) {
                    لکھو("%d: 0x%lx_%lu_irq\n",
                           i,
#اگر defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (طویل غیرعلامتی صحیح)irq.irq,
#ورنہ
                           (طویل غیرعلامتی صحیح)irq,
#اگر_ختم
                           (طویل غیرعلامتی صحیح)ہدف);
                }
            }
        }
    }
    لکھو("}\n");
}

خالی obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_مقصد_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    اگر (isIRQActive(irq)) {
        لکھو("0x%lx_%lu_irq {\n",
#اگر defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (طویل غیرعلامتی صحیح)irq.irq,
#ورنہ
               (طویل غیرعلامتی صحیح)irq,
#اگر_ختم
               (طویل غیرعلامتی صحیح)IRQT_مقصد_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_مقصد_IDX(irq)].cap;
        اگر (cap_get_capType(ntfn_cap) != cap_null_cap) {
            لکھو("0x0: ");
            print_cap(ntfn_cap);
        }
        لکھو("}\n");
    }
}

خالی print_objects(خالی)
{
    برائے (tcb_t *curr = گرہ_STATE(ksDebugTCBs); curr != تہی; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا) {
        اگر (جڑ_or_idle_tcb(curr)) {
            جاری;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    برائے (tcb_t *curr = گرہ_STATE(ksDebugTCBs); curr != تہی; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا) {
        اگر (جڑ_or_idle_tcb(curr)) {
            جاری;
        }

        /* print the tcb as objects */
        لکھو("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        اگر (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

خالی print_caps(خالی)
{
    برائے (tcb_t *curr = گرہ_STATE(ksDebugTCBs); curr != تہی; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا) {
        اگر (جڑ_or_idle_tcb(curr)) {
            جاری;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

خالی print_cap(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_endpoint_cap: {
        لکھو("%p_ep ",
               (خالی *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        توڑو;
    }
    حالت cap_notification_cap: {
        لکھو("%p_notification ",
               (خالی *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        توڑو;
    }
    حالت cap_untyped_cap: {
        لکھو("%p_untyped\n",
               (خالی *)cap_untyped_cap_get_capPtr(cap));
        توڑو;
    }
    حالت cap_thread_cap: {
        لکھو("%p_tcb\n",
               (خالی *)cap_thread_cap_get_capTCBPtr(cap));
        توڑو;
    }
    حالت cap_cnode_cap: {
        لکھو("%p_cnode ",
               (خالی *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        توڑو;
    }
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_جواب_cap: {
        لکھو("%p_reply\n",
               (خالی *)cap_جواب_cap_get_cap_جواب_Ptr(cap));
        توڑو;
    }
    حالت cap_sched_context_cap: {
        لکھو("%p_sc\n",
               (خالی *)cap_sched_context_cap_get_capSCPtr(cap));
        توڑو;
    }
    حالت cap_sched_control_cap: {
        لکھو("%lu_sched_control\n",
               (طویل غیرعلامتی صحیح)cap_sched_control_cap_get_core(cap));
        توڑو;
    }
#اگر_ختم
    حالت cap_irq_control_cap: {
        لکھو("irq_control\n"); /* only one in the system */
        توڑو;
    }
    حالت cap_irq_handler_cap: {
        لکھو("%p_%lu_irq\n",
               (خالی *)cap_irq_handler_cap_get_capIRQ(cap),
               (طویل غیرعلامتی صحیح)IRQT_مقصد_CORE(IDX_مقصد_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        توڑو;
    }
    طےشدہ: {
        print_cap_arch(cap);
        توڑو;
    }
    }
}

خالی print_object(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_endpoint_cap: {
        لکھو("%p_ep = ep\n",
               (خالی *)cap_endpoint_cap_get_capEPPtr(cap));
        توڑو;
    }
    حالت cap_notification_cap: {
        لکھو("%p_notification = notification\n",
               (خالی *)cap_notification_cap_get_capNtfnPtr(cap));
        توڑو;
    }
    حالت cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        توڑو;
    }
    حالت cap_cnode_cap: {
        أكد(!"should not happend");
    }
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت cap_جواب_cap: {
        لکھو("%p_reply = rtreply\n",
               (خالی *)cap_جواب_cap_get_cap_جواب_Ptr(cap));
        توڑو;
    }
    حالت cap_sched_context_cap: {
        لکھو("%p_sc = sc ",
               (خالی *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        توڑو;
    }
#اگر_ختم
    حالت cap_irq_handler_cap: {
        لکھو("%p_%lu_irq = irq\n",
               (خالی *)cap_irq_handler_cap_get_capIRQ(cap),
               (طویل غیرعلامتی صحیح)IRQT_مقصد_CORE(IDX_مقصد_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        توڑو;
    }
    طےشدہ:
        print_object_arch(cap);
        توڑو;
    }
}

#اگر_ختم /* CONFIG_PRINTING */

#اگر_ختم /* CONFIG_DEBUG_BUILD */
