/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#અન્તર્ભાવ <machine/capdl.h>
#અન્તર્ભાવ <machine/registerset.h>
#અન્તર્ભાવ <machine/timer.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <kernel/cspace.h>
#ifdef CONFIG_KERNEL_MCS
#અન્તર્ભાવ <kernel/sporadic.h>
#endif

#define SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
પૂર્ણાંક watermark = 0;

શૂન્ય add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    જો (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

શૂન્ય reset_seen_list(શૂન્ય)
{
    memset(seen_list, 0, SEEN_SZ * કદ(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    માટે (પૂર્ણાંક i = 0; i < watermark; i++) {
        જો (same_cap(seen_list[i], c)) {
            પાછા true;
        }
    }
    પાછા false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    પાછા (a.શબ્દો[0] == b.શબ્દો[0] && a.શબ્દો[1] == b.શબ્દો[1]);
}

/* Return true if strings are the same */
સ્થિર અંતઃરેખ bool_t strings_equal(અચલ અક્ષર *str1, અચલ અક્ષર *str2)
{
    જ્યારે (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    પાછા !(*(અચલ unsigned અક્ષર *)str1 - * (અચલ unsigned અક્ષર *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t મૂળ_or_idle_tcb(tcb_t *tcb)
{
    પાછા (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

શૂન્ય obj_tcb_print_attrs(tcb_t *tcb)
{
    printf("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (long unsigned પૂર્ણાંક)tcb->tcbIPCBuffer,
           (long unsigned પૂર્ણાંક)getRestartPC(tcb),
           (long unsigned પૂર્ણાંક)get_tcb_sp(tcb),
           (long unsigned પૂર્ણાંક)tcb->tcbPriority,
           (long unsigned પૂર્ણાંક)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    printf(", affinity: %lu", (long unsigned પૂર્ણાંક)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    જો (cap_get_capType(ep_cap) != cap_null_cap) {
        printf(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    printf(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

સ્થિર અંતઃરેખ ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    જ્યારે (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    પાછા sum;
}

શૂન્ય obj_sc_print_attrs(cap_t sc_cap)
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

શૂન્ય obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    printf("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (શૂન્ય *)cap_untyped_cap_get_capPtr(slot->cap),
           (long unsigned પૂર્ણાંક)cap_untyped_cap_get_capBlockSize(slot->cap),
           (long unsigned પૂર્ણાંક)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    માટે (cte_t *nextPtr = CTE_PTR(mdb_ગાંઠ_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_ગાંઠ_get_mdb_Next(slot->cteMDBNode))) {
        જો (!sameRegionAs(slot->cap, nextPtr->cap)) {
            /* TBD:
             * - this will print out the attributes of the cap, which it shouldn't
             *
             * - might be a pathological case where an untyped has a child cap that
             *   isn't reachable from any of the non root threads. This would result
             *   in an object being mentioned but never properly defined
             */
            print_cap(nextPtr->cap);
        }
    }
    printf("}\n");
}

શૂન્ય obj_cnode_print_attrs(cap_t cnode)
{
    printf("(%lu bits)\n", (long unsigned પૂર્ણાંક)cap_cnode_cap_get_capCNodeRadix(cnode));
}

શૂન્ય obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    જો (seen(cnode)) {
        પાછા;
    }
    add_to_seen(cnode);
    printf("%p_cnode = cnode ", (શૂન્ય *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    માટે (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        જો (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } નહીંતર જો (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } નહીંતર જો (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

શૂન્ય cap_cnode_print_attrs(cap_t cnode)
{
    printf("(guard: %lu, guard_size: %lu)\n",
           (long unsigned પૂર્ણાંક)cap_cnode_cap_get_capCNodeGuard(cnode),
           (long unsigned પૂર્ણાંક)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

શૂન્ય cap_ep_print_attrs(cap_t ep)
{
    printf("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? putchar('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? putchar('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? putchar('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? putchar('P') : 0;
    long unsigned પૂર્ણાંક badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

શૂન્ય cap_ntfn_print_attrs(cap_t ntfn)
{
    printf("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? putchar('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? putchar('W') : 0;
    long unsigned પૂર્ણાંક badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

/*
 * print object slots
 */

શૂન્ય obj_tcb_print_slots(tcb_t *tcb)
{
    printf("%p_tcb {\n", tcb);

    /* CSpace root */
    જો (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        printf("cspace: %p_cnode ",
               (શૂન્ય *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    જો (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        printf("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_મૂળ_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    જો (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    જો (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        printf("fault_ep_slot: %p_ep ",
               (શૂન્ય *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    જો (tcb->tcbSchedContext) {
        printf("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    જો (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        printf("temp_fault_ep_slot: %p_ep ",
               (શૂન્ય *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# નહીંતર
    /* Reply cap slot */
    જો (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        printf("reply_slot: %p_reply\n",
               (શૂન્ય *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    જો (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        printf("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    printf("}\n");
}

/* TBD: deal with nested cnodes */
શૂન્ય obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t મૂળ = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    જો (cap_get_capType(મૂળ) != cap_cnode_cap) {
        પાછા;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(મૂળ);
    જો (seen(મૂળ)) {
        પાછા;
    }
    add_to_seen(મૂળ);

    printf("%p_cnode {\n", (શૂન્ય *)cap_cnode_cap_get_capCNodePtr(મૂળ));

    માટે (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        જો (cap_get_capType(c.cap) != cap_null_cap) {
            printf("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    printf("}\n");

    માટે (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        જો (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

શૂન્ય obj_irq_print_maps(શૂન્ય)
{
    printf("irq maps {\n");

    માટે (seL4_Word target = 0; target < CONFIG_MAX_અંક_NODES; target++) {
        માટે (unsigned i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            જો (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                જો (cap_get_capType(cap) != cap_null_cap) {
                    printf("%d: 0x%lx_%lu_irq\n",
                           i,
#જો defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (long unsigned પૂર્ણાંક)irq.irq,
#નહીંતર
                           (long unsigned પૂર્ણાંક)irq,
#endif
                           (long unsigned પૂર્ણાંક)target);
                }
            }
        }
    }
    printf("}\n");
}

શૂન્ય obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    જો (isIRQActive(irq)) {
        printf("0x%lx_%lu_irq {\n",
#જો defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (long unsigned પૂર્ણાંક)irq.irq,
#નહીંતર
               (long unsigned પૂર્ણાંક)irq,
#endif
               (long unsigned પૂર્ણાંક)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        જો (cap_get_capType(ntfn_cap) != cap_null_cap) {
            printf("0x0: ");
            print_cap(ntfn_cap);
        }
        printf("}\n");
    }
}

શૂન્ય print_objects(શૂન્ય)
{
    માટે (tcb_t *curr = ગાંઠ_STATE(ksDebugTCBs); curr != ખાલી; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        જો (મૂળ_or_idle_tcb(curr)) {
            ચાલુ;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    માટે (tcb_t *curr = ગાંઠ_STATE(ksDebugTCBs); curr != ખાલી; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        જો (મૂળ_or_idle_tcb(curr)) {
            ચાલુ;
        }

        /* print the tcb as objects */
        printf("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        જો (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

શૂન્ય print_caps(શૂન્ય)
{
    માટે (tcb_t *curr = ગાંઠ_STATE(ksDebugTCBs); curr != ખાલી; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        જો (મૂળ_or_idle_tcb(curr)) {
            ચાલુ;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

શૂન્ય print_cap(cap_t cap)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_endpoint_cap: {
        printf("%p_ep ",
               (શૂન્ય *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        તોડો;
    }
    સ્થિતિ cap_notification_cap: {
        printf("%p_notification ",
               (શૂન્ય *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        તોડો;
    }
    સ્થિતિ cap_untyped_cap: {
        printf("%p_untyped\n",
               (શૂન્ય *)cap_untyped_cap_get_capPtr(cap));
        તોડો;
    }
    સ્થિતિ cap_thread_cap: {
        printf("%p_tcb\n",
               (શૂન્ય *)cap_thread_cap_get_capTCBPtr(cap));
        તોડો;
    }
    સ્થિતિ cap_cnode_cap: {
        printf("%p_cnode ",
               (શૂન્ય *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        તોડો;
    }
#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_reply_cap: {
        printf("%p_reply\n",
               (શૂન્ય *)cap_reply_cap_get_capReplyPtr(cap));
        તોડો;
    }
    સ્થિતિ cap_sched_context_cap: {
        printf("%p_sc\n",
               (શૂન્ય *)cap_sched_context_cap_get_capSCPtr(cap));
        તોડો;
    }
    સ્થિતિ cap_sched_control_cap: {
        printf("%lu_sched_control\n",
               (long unsigned પૂર્ણાંક)cap_sched_control_cap_get_core(cap));
        તોડો;
    }
#endif
    સ્થિતિ cap_irq_control_cap: {
        printf("irq_control\n"); /* only one in the system */
        તોડો;
    }
    સ્થિતિ cap_irq_handler_cap: {
        printf("%p_%lu_irq\n",
               (શૂન્ય *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned પૂર્ણાંક)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        તોડો;
    }
    મૂળભૂત: {
        print_cap_arch(cap);
        તોડો;
    }
    }
}

શૂન્ય print_object(cap_t cap)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_endpoint_cap: {
        printf("%p_ep = ep\n",
               (શૂન્ય *)cap_endpoint_cap_get_capEPPtr(cap));
        તોડો;
    }
    સ્થિતિ cap_notification_cap: {
        printf("%p_notification = notification\n",
               (શૂન્ય *)cap_notification_cap_get_capNtfnPtr(cap));
        તોડો;
    }
    સ્થિતિ cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        તોડો;
    }
    સ્થિતિ cap_cnode_cap: {
        પ્રતિજ્ઞા(!"should not happend");
    }
#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ cap_reply_cap: {
        printf("%p_reply = rtreply\n",
               (શૂન્ય *)cap_reply_cap_get_capReplyPtr(cap));
        તોડો;
    }
    સ્થિતિ cap_sched_context_cap: {
        printf("%p_sc = sc ",
               (શૂન્ય *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        તોડો;
    }
#endif
    સ્થિતિ cap_irq_handler_cap: {
        printf("%p_%lu_irq = irq\n",
               (શૂન્ય *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned પૂર્ણાંક)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        તોડો;
    }
    મૂળભૂત:
        print_object_arch(cap);
        તોડો;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
