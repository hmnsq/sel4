/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <machine/registerset.h>
#ضمن <machine/timer.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
#ضمن <kernel/sporadic.h>
#جے_انت

#تعریف_کرو تکیا_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t تکیا_list[تکیا_SZ];
صحيح watermark = 0;

فراغ جوڑو_منزل_تکیا(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    إذا (watermark < تکیا_SZ) {
        تکیا_list[watermark] = c;
        watermark++;
    }
}

فراغ reset_تکیا_list(فراغ)
{
    memset(تکیا_list, 0, تکیا_SZ * حجم(تکیا_list[0]));
    watermark = 0;
}

bool_t تکیا(cap_t c)
{
    لكل (صحيح i = 0; i < watermark; i++) {
        إذا (same_cap(تکیا_list[i], c)) {
            أرجع true;
        }
    }
    أرجع false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    أرجع (a.الفاظ[0] == b.الفاظ[0] && a.الفاظ[1] == b.الفاظ[1]);
}

/* Return true if strings are the same */
ساكن مضمن bool_t لڑیاں_equal(ثابت حرف *str1, ثابت حرف *str2)
{
    طالما (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    أرجع !(*(ثابت بنانشانی حرف *)str1 - * (ثابت بنانشانی حرف *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t جڑ_or_idle_tcb(tcb_t *tcb)
{
    أرجع (لڑیاں_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || لڑیاں_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#جے_تعریف_شدہ CONFIG_PRINTING

فراغ obj_tcb_print_attrs(tcb_t *tcb)
{
    لکھو("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (لما بنانشانی صحيح)tcb->tcbIPCBuffer,
           (لما بنانشانی صحيح)getRestartPC(tcb),
           (لما بنانشانی صحيح)get_tcb_sp(tcb),
           (لما بنانشانی صحيح)tcb->tcb_ترجیح,
           (لما بنانشانی صحيح)tcb->tcbMCP);

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    لکھو(", affinity: %lu", (لما بنانشانی صحيح)tcb->tcbAffinity);
#جے_انت /* ENABLE_SMP_SUPPORT */

    /* init */

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    إذا (cap_get_capType(ep_cap) != cap_null_cap) {
        لکھو(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#جے_انت

    لکھو(", dom: %ld)\n", tcb->tcbDomain);
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS

ساكن مضمن ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t جوڑ_2 = refill_سر(sc)->rAmount;
    لفظ_t current = sc->sc_Refill_سر;

    طالما (current != sc->sc_Refill_پوچھ) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        جوڑ_2 += refill_اشاریہ(sc, current)->rAmount;
    }

    أرجع جوڑ_2;
}

فراغ obj_sc_print_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    لکھو("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_لفظ" bits)\n",
           ticks_منزل_Us(period), period,
           ticks_منزل_Us(budget), budget,
           (لفظ_t)cap_sched_context_cap_get_cap_SCSize_بٹ(sc_cap));
}
#جے_انت /* CONFIG_KERNEL_MCS */

فراغ obj_ut_print_attrs(cte_t *تھاں, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    لکھو("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (فراغ *)cap_untyped_cap_get_capPtr(تھاں->cap),
           (لما بنانشانی صحيح)cap_untyped_cap_get_cap_بلاک_ناپ(تھاں->cap),
           (لما بنانشانی صحيح)cap_untyped_cap_get_cap_بلاک_ناپ(تھاں->cap),
           لفظ_PTR(cap_untyped_cap_get_capPtr(تھاں->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    لكل (cte_t *اگلا_Ptr = CTE_PTR(mdb_گرہ_get_mdb_اگلا(تھاں->cteMDBNode));
         اگلا_Ptr && is_MDBParent_دا(تھاں, اگلا_Ptr);
         اگلا_Ptr = CTE_PTR(mdb_گرہ_get_mdb_اگلا(تھاں->cteMDBNode))) {
        إذا (!sameRegionAs(تھاں->cap, اگلا_Ptr->cap)) {
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

فراغ obj_cnode_print_attrs(cap_t cnode)
{
    لکھو("(%lu bits)\n", (لما بنانشانی صحيح)cap_cnode_cap_get_capCNodeRadix(cnode));
}

فراغ obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    إذا (تکیا(cnode)) {
        أرجع;
    }
    جوڑو_منزل_تکیا(cnode);
    لکھو("%p_cnode = cnode ", (فراغ *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    لفظ_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_تھاں_ret_t c = lookup_Cap_And_تھاں(tcb, i);
        إذا (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.تھاں, tcb);

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
    لکھو("(guard: %lu, guard_size: %lu)\n",
           (لما بنانشانی صحيح)cap_cnode_cap_get_capCNodeGuard(cnode),
           (لما بنانشانی صحيح)cap_cnode_cap_get_cap_CNode_Guard_ناپ(cnode));
}

فراغ cap_ep_print_attrs(cap_t ep)
{
    لکھو("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? حرف_لکھو('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? حرف_لکھو('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? حرف_لکھو('G') : 0;
    cap_endpoint_cap_get_cap_Can_Grant_جواب(ep) ? حرف_لکھو('P') : 0;
    لما بنانشانی صحيح badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? لکھو(", badge: %lu)\n", badge) : لکھو(")\n");
}

فراغ cap_ntfn_print_attrs(cap_t ntfn)
{
    لکھو("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? حرف_لکھو('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? حرف_لکھو('W') : 0;
    لما بنانشانی صحيح badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? لکھو(", badge: %lu)\n", badge) : لکھو(")\n");
}

/*
 * print object slots
 */

فراغ obj_tcb_print_slots(tcb_t *tcb)
{
    لکھو("%p_tcb {\n", tcb);

    /* CSpace root */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        لکھو("cspace: %p_cnode ",
               (فراغ *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        لکھو("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_جڑ_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_بفر)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_بفر_تھاں(tcb);
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        لکھو("fault_ep_slot: %p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    إذا (tcb->tcbSchedContext) {
        لکھو("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        لکھو("temp_fault_ep_slot: %p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# وإلا
    /* Reply cap slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcb_جواب)->cap) != cap_null_cap) {
        لکھو("reply_slot: %p_reply\n",
               (فراغ *)cap_جواب_cap_get_cap_TCBPtr(TCB_PTR_CTE_PTR(tcb, tcb_جواب)->cap));
    }

    /* TCB of most recent IPC sender */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        لکھو("caller_slot: %p_tcb\n", caller);
    }
#جے_انت /* CONFIG_KERNEL_MCS */
    لکھو("}\n");
}

/* TBD: deal with nested cnodes */
فراغ obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t جڑ = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    إذا (cap_get_capType(جڑ) != cap_cnode_cap) {
        أرجع;
    }

    لفظ_t radix = cap_cnode_cap_get_capCNodeRadix(جڑ);
    إذا (تکیا(جڑ)) {
        أرجع;
    }
    جوڑو_منزل_تکیا(جڑ);

    لکھو("%p_cnode {\n", (فراغ *)cap_cnode_cap_get_capCNodePtr(جڑ));

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_تھاں_ret_t c = lookup_Cap_And_تھاں(tcb, i);
        إذا (cap_get_capType(c.cap) != cap_null_cap) {
            لکھو("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    لکھو("}\n");

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookup_Cap_And_تھاں_ret_t c = lookup_Cap_And_تھاں(tcb, i);
        إذا (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

فراغ obj_irq_print_maps(فراغ)
{
    لکھو("irq maps {\n");

    لكل (se_L4_لفظ نشانہ = 0; نشانہ < CONFIG_MAX_عدد_NODES; نشانہ++) {
        لكل (بنانشانی i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_منزل_IRQT(نشانہ, i);
            إذا (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_منزل_IDX(irq)].cap;
                إذا (cap_get_capType(cap) != cap_null_cap) {
                    لکھو("%d: 0x%lx_%lu_irq\n",
                           i,
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (لما بنانشانی صحيح)irq.irq,
#وإلا
                           (لما بنانشانی صحيح)irq,
#جے_انت
                           (لما بنانشانی صحيح)نشانہ);
                }
            }
        }
    }
    لکھو("}\n");
}

فراغ obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    إذا (isIRQActive(irq)) {
        لکھو("0x%lx_%lu_irq {\n",
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (لما بنانشانی صحيح)irq.irq,
#وإلا
               (لما بنانشانی صحيح)irq,
#جے_انت
               (لما بنانشانی صحيح)IRQT_منزل_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_منزل_IDX(irq)].cap;
        إذا (cap_get_capType(ntfn_cap) != cap_null_cap) {
            لکھو("0x0: ");
            print_cap(ntfn_cap);
        }
        لکھو("}\n");
    }
}

فراغ print_objects(فراغ)
{
    لكل (tcb_t *curr = گرہ_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا) {
        إذا (جڑ_or_idle_tcb(curr)) {
            استمر;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    لكل (tcb_t *curr = گرہ_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا) {
        إذا (جڑ_or_idle_tcb(curr)) {
            استمر;
        }

        /* print the tcb as objects */
        لکھو("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        إذا (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

فراغ print_caps(فراغ)
{
    لكل (tcb_t *curr = گرہ_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcb_Debug_اگلا) {
        إذا (جڑ_or_idle_tcb(curr)) {
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
        لکھو("%p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        اكسر;
    }
    حالة cap_notification_cap: {
        لکھو("%p_notification ",
               (فراغ *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        اكسر;
    }
    حالة cap_untyped_cap: {
        لکھو("%p_untyped\n",
               (فراغ *)cap_untyped_cap_get_capPtr(cap));
        اكسر;
    }
    حالة cap_thread_cap: {
        لکھو("%p_tcb\n",
               (فراغ *)cap_thread_cap_get_capTCBPtr(cap));
        اكسر;
    }
    حالة cap_cnode_cap: {
        لکھو("%p_cnode ",
               (فراغ *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        اكسر;
    }
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    حالة cap_جواب_cap: {
        لکھو("%p_reply\n",
               (فراغ *)cap_جواب_cap_get_cap_جواب_Ptr(cap));
        اكسر;
    }
    حالة cap_sched_context_cap: {
        لکھو("%p_sc\n",
               (فراغ *)cap_sched_context_cap_get_capSCPtr(cap));
        اكسر;
    }
    حالة cap_sched_control_cap: {
        لکھو("%lu_sched_control\n",
               (لما بنانشانی صحيح)cap_sched_control_cap_get_core(cap));
        اكسر;
    }
#جے_انت
    حالة cap_irq_control_cap: {
        لکھو("irq_control\n"); /* only one in the system */
        اكسر;
    }
    حالة cap_irq_handler_cap: {
        لکھو("%p_%lu_irq\n",
               (فراغ *)cap_irq_handler_cap_get_capIRQ(cap),
               (لما بنانشانی صحيح)IRQT_منزل_CORE(IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
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
        لکھو("%p_ep = ep\n",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(cap));
        اكسر;
    }
    حالة cap_notification_cap: {
        لکھو("%p_notification = notification\n",
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
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    حالة cap_جواب_cap: {
        لکھو("%p_reply = rtreply\n",
               (فراغ *)cap_جواب_cap_get_cap_جواب_Ptr(cap));
        اكسر;
    }
    حالة cap_sched_context_cap: {
        لکھو("%p_sc = sc ",
               (فراغ *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        اكسر;
    }
#جے_انت
    حالة cap_irq_handler_cap: {
        لکھو("%p_%lu_irq = irq\n",
               (فراغ *)cap_irq_handler_cap_get_capIRQ(cap),
               (لما بنانشانی صحيح)IRQT_منزل_CORE(IDX_منزل_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        اكسر;
    }
    افتراضي:
        print_object_arch(cap);
        اكسر;
    }
}

#جے_انت /* CONFIG_PRINTING */

#جے_انت /* CONFIG_DEBUG_BUILD */
