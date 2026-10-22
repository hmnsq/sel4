/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <machine/registerset.h>
#ضمن <machine/timer.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>
#ifdef CONFIG_KERNEL_MCS
#ضمن <kernel/sporadic.h>
#endif

#define SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
صحيح watermark = 0;

فراغ add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    إذا (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

فراغ reset_seen_list(فراغ)
{
    memset(seen_list, 0, SEEN_SZ * حجم(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    لكل (صحيح i = 0; i < watermark; i++) {
        إذا (same_cap(seen_list[i], c)) {
            أرجع true;
        }
    }
    أرجع false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    أرجع (a.كلموم[0] == b.كلموم[0] && a.كلموم[1] == b.كلموم[1]);
}

/* Return true if strings are the same */
ساكن مضمن bool_t strings_equal(ثابت حرف *str1, ثابت حرف *str2)
{
    طالما (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    أرجع !(*(ثابت unsigned حرف *)str1 - * (ثابت unsigned حرف *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t سيو_or_idle_tcb(tcb_t *tcb)
{
    أرجع (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

فراغ obj_tcb_بغ_attrs(tcb_t *tcb)
{
    printf("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (long unsigned صحيح)tcb->tcbIPCBuffer,
           (long unsigned صحيح)getRestartPC(tcb),
           (long unsigned صحيح)get_tcb_sp(tcb),
           (long unsigned صحيح)tcb->tcbPriority,
           (long unsigned صحيح)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    printf(", affinity: %lu", (long unsigned صحيح)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    إذا (cap_get_capType(ep_cap) != cap_null_cap) {
        printf(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    printf(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

ساكن مضمن ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    طالما (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    أرجع sum;
}

فراغ obj_sc_بغ_attrs(cap_t sc_cap)
{
    sched_context_t *sc = SC_PTR(cap_sched_context_cap_get_capSCPtr(sc_cap));
    ticks_t period = sc->scPeriod;
    ticks_t budget = sc_get_budget(sc);
    printf("(period: %"PRIu64" us (%"PRIu64" ticks), budget: %"PRIu64 " us "
           "(%"PRIu64" ticks), %"SEL4_PRIu_word" bits)\n",
           ticksToUs(period), period,
           ticksToUs(budget), budget,
           (word_t)cap_sched_context_cap_get_capSCSizeBits(sc_cap));
}
#endif /* CONFIG_KERNEL_MCS */

فراغ obj_ut_بغ_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    printf("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (فراغ *)cap_untyped_cap_get_capPtr(slot->cap),
           (long unsigned صحيح)cap_untyped_cap_get_capBlockSize(slot->cap),
           (long unsigned صحيح)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    لكل (cte_t *nextPtr = CTE_PTR(mdb_كلي_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_كلي_get_mdb_Next(slot->cteMDBNode))) {
        إذا (!sameRegionAs(slot->cap, nextPtr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            بغ_cap(nextPtr->cap);
        }
    }
    printf("}\n");
}

فراغ obj_cnode_بغ_attrs(cap_t cnode)
{
    printf("(%lu bits)\n", (long unsigned صحيح)cap_cnode_cap_get_capCNodeRadix(cnode));
}

فراغ obj_tcb_بغ_cnodes(cap_t cnode, tcb_t *tcb)
{
    إذا (seen(cnode)) {
        أرجع;
    }
    add_to_seen(cnode);
    printf("%p_cnode = cnode ", (فراغ *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_بغ_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        إذا (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_بغ_attrs(c.slot, tcb);

        } وإلا إذا (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } وإلا إذا (cap_get_capType(c.cap) != cap_null_cap) {
            بغ_object(c.cap);
        }
    }
}

/*
 * Caps
 */

فراغ cap_cnode_بغ_attrs(cap_t cnode)
{
    printf("(guard: %lu, guard_size: %lu)\n",
           (long unsigned صحيح)cap_cnode_cap_get_capCNodeGuard(cnode),
           (long unsigned صحيح)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

فراغ cap_ep_بغ_attrs(cap_t ep)
{
    printf("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? putchar('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? putchar('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? putchar('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? putchar('P') : 0;
    long unsigned صحيح badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

فراغ cap_ntfn_بغ_attrs(cap_t ntfn)
{
    printf("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? putchar('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? putchar('W') : 0;
    long unsigned صحيح badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

/*
 * print object slots
 */

فراغ obj_tcb_بغ_slots(tcb_t *tcb)
{
    printf("%p_tcb {\n", tcb);

    /* CSpace root */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        printf("cspace: %p_cnode ",
               (فراغ *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_بغ_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        printf("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_سيو_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        بغ_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        printf("fault_ep_slot: %p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_بغ_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    إذا (tcb->tcbSchedContext) {
        printf("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        printf("temp_fault_ep_slot: %p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_بغ_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# وإلا
    /* Reply cap slot */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        printf("reply_slot: %p_reply\n",
               (فراغ *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    إذا (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        printf("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    printf("}\n");
}

/* TBD: deal with nested cnodes */
فراغ obj_cnode_بغ_slots(tcb_t *tcb)
{
    cap_t سيو = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    إذا (cap_get_capType(سيو) != cap_cnode_cap) {
        أرجع;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(سيو);
    إذا (seen(سيو)) {
        أرجع;
    }
    add_to_seen(سيو);

    printf("%p_cnode {\n", (فراغ *)cap_cnode_cap_get_capCNodePtr(سيو));

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        إذا (cap_get_capType(c.cap) != cap_null_cap) {
            printf("0x%x: ", i);
            بغ_cap(c.cap);
        }
    }
    printf("}\n");

    لكل (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        إذا (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_بغ_slots(c.cap);
        }
    }
}

فراغ obj_irq_بغ_maps(فراغ)
{
    printf("irq maps {\n");

    لكل (seL4_Word target = 0; target < CONFIG_MAX_لمب_NODES; target++) {
        لكل (unsigned i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            إذا (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                إذا (cap_get_capType(cap) != cap_null_cap) {
                    printf("%d: 0x%lx_%lu_irq\n",
                           i,
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (long unsigned صحيح)irq.irq,
#وإلا
                           (long unsigned صحيح)irq,
#endif
                           (long unsigned صحيح)target);
                }
            }
        }
    }
    printf("}\n");
}

فراغ obj_irq_بغ_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    إذا (isIRQActive(irq)) {
        printf("0x%lx_%lu_irq {\n",
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (long unsigned صحيح)irq.irq,
#وإلا
               (long unsigned صحيح)irq,
#endif
               (long unsigned صحيح)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        إذا (cap_get_capType(ntfn_cap) != cap_null_cap) {
            printf("0x0: ");
            بغ_cap(ntfn_cap);
        }
        printf("}\n");
    }
}

فراغ بغ_objects(فراغ)
{
    لكل (tcb_t *curr = كلي_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        إذا (سيو_or_idle_tcb(curr)) {
            استمر;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_بغ_vtable(curr);
    }

    لكل (tcb_t *curr = كلي_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        إذا (سيو_or_idle_tcb(curr)) {
            استمر;
        }

        /* print the tcb as objects */
        printf("%p_tcb = tcb ", curr);
        obj_tcb_بغ_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        إذا (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_بغ_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

فراغ بغ_caps(فراغ)
{
    لكل (tcb_t *curr = كلي_STATE(ksDebugTCBs); curr != عدم; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        إذا (سيو_or_idle_tcb(curr)) {
            استمر;
        }
        obj_cnode_بغ_slots(curr);
        obj_vtable_بغ_slots(curr);
        obj_tcb_بغ_slots(curr);
    }
}

فراغ بغ_cap(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap: {
        printf("%p_ep ",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_بغ_attrs(cap);
        اكسر;
    }
    حالة cap_notification_cap: {
        printf("%p_notification ",
               (فراغ *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_بغ_attrs(cap);
        اكسر;
    }
    حالة cap_untyped_cap: {
        printf("%p_untyped\n",
               (فراغ *)cap_untyped_cap_get_capPtr(cap));
        اكسر;
    }
    حالة cap_thread_cap: {
        printf("%p_tcb\n",
               (فراغ *)cap_thread_cap_get_capTCBPtr(cap));
        اكسر;
    }
    حالة cap_cnode_cap: {
        printf("%p_cnode ",
               (فراغ *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_بغ_attrs(cap);
        اكسر;
    }
#ifdef CONFIG_KERNEL_MCS
    حالة cap_reply_cap: {
        printf("%p_reply\n",
               (فراغ *)cap_reply_cap_get_capReplyPtr(cap));
        اكسر;
    }
    حالة cap_sched_context_cap: {
        printf("%p_sc\n",
               (فراغ *)cap_sched_context_cap_get_capSCPtr(cap));
        اكسر;
    }
    حالة cap_sched_control_cap: {
        printf("%lu_sched_control\n",
               (long unsigned صحيح)cap_sched_control_cap_get_core(cap));
        اكسر;
    }
#endif
    حالة cap_irq_control_cap: {
        printf("irq_control\n"); /* only one in the system */
        اكسر;
    }
    حالة cap_irq_handler_cap: {
        printf("%p_%lu_irq\n",
               (فراغ *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned صحيح)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        اكسر;
    }
    افتراضي: {
        بغ_cap_arch(cap);
        اكسر;
    }
    }
}

فراغ بغ_object(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_endpoint_cap: {
        printf("%p_ep = ep\n",
               (فراغ *)cap_endpoint_cap_get_capEPPtr(cap));
        اكسر;
    }
    حالة cap_notification_cap: {
        printf("%p_notification = notification\n",
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
#ifdef CONFIG_KERNEL_MCS
    حالة cap_reply_cap: {
        printf("%p_reply = rtreply\n",
               (فراغ *)cap_reply_cap_get_capReplyPtr(cap));
        اكسر;
    }
    حالة cap_sched_context_cap: {
        printf("%p_sc = sc ",
               (فراغ *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_بغ_attrs(cap);
        اكسر;
    }
#endif
    حالة cap_irq_handler_cap: {
        printf("%p_%lu_irq = irq\n",
               (فراغ *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned صحيح)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        اكسر;
    }
    افتراضي:
        بغ_object_arch(cap);
        اكسر;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
