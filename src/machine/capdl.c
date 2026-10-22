/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#அந்தர்பாவ <machine/capdl.h>
#அந்தர்பாவ <machine/registerset.h>
#அந்தர்பாவ <machine/timer.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <kernel/cspace.h>
#ifdef CONFIG_KERNEL_MCS
#அந்தர்பாவ <kernel/sporadic.h>
#endif

#define SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
முழு watermark = 0;

வெறுமை add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    எனில் (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

வெறுமை reset_seen_list(வெறுமை)
{
    memset(seen_list, 0, SEEN_SZ * அளவு(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    க்கு (முழு i = 0; i < watermark; i++) {
        எனில் (same_cap(seen_list[i], c)) {
            திரும்பு true;
        }
    }
    திரும்பு false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    திரும்பு (a.சொற்கள்[0] == b.சொற்கள்[0] && a.சொற்கள்[1] == b.சொற்கள்[1]);
}

/* Return true if strings are the same */
நிலையான உள்வரி bool_t strings_equal(மாறிலி எழுத்து *str1, மாறிலி எழுத்து *str2)
{
    போது (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    திரும்பு !(*(மாறிலி unsigned எழுத்து *)str1 - * (மாறிலி unsigned எழுத்து *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t வேர்_or_idle_tcb(tcb_t *tcb)
{
    திரும்பு (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

வெறுமை obj_tcb_print_attrs(tcb_t *tcb)
{
    printf("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (long unsigned முழு)tcb->tcbIPCBuffer,
           (long unsigned முழு)getRestartPC(tcb),
           (long unsigned முழு)get_tcb_sp(tcb),
           (long unsigned முழு)tcb->tcbPriority,
           (long unsigned முழு)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    printf(", affinity: %lu", (long unsigned முழு)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    எனில் (cap_get_capType(ep_cap) != cap_null_cap) {
        printf(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    printf(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

நிலையான உள்வரி ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    போது (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    திரும்பு sum;
}

வெறுமை obj_sc_print_attrs(cap_t sc_cap)
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

வெறுமை obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    printf("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (வெறுமை *)cap_untyped_cap_get_capPtr(slot->cap),
           (long unsigned முழு)cap_untyped_cap_get_capBlockSize(slot->cap),
           (long unsigned முழு)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    க்கு (cte_t *nextPtr = CTE_PTR(mdb_முனை_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_முனை_get_mdb_Next(slot->cteMDBNode))) {
        எனில் (!sameRegionAs(slot->cap, nextPtr->cap)) {
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

வெறுமை obj_cnode_print_attrs(cap_t cnode)
{
    printf("(%lu bits)\n", (long unsigned முழு)cap_cnode_cap_get_capCNodeRadix(cnode));
}

வெறுமை obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    எனில் (seen(cnode)) {
        திரும்பு;
    }
    add_to_seen(cnode);
    printf("%p_cnode = cnode ", (வெறுமை *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    க்கு (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        எனில் (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } இல்லையெனில் எனில் (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } இல்லையெனில் எனில் (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

வெறுமை cap_cnode_print_attrs(cap_t cnode)
{
    printf("(guard: %lu, guard_size: %lu)\n",
           (long unsigned முழு)cap_cnode_cap_get_capCNodeGuard(cnode),
           (long unsigned முழு)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

வெறுமை cap_ep_print_attrs(cap_t ep)
{
    printf("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? putchar('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? putchar('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? putchar('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? putchar('P') : 0;
    long unsigned முழு badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

வெறுமை cap_ntfn_print_attrs(cap_t ntfn)
{
    printf("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? putchar('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? putchar('W') : 0;
    long unsigned முழு badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

/*
 * print object slots
 */

வெறுமை obj_tcb_print_slots(tcb_t *tcb)
{
    printf("%p_tcb {\n", tcb);

    /* CSpace root */
    எனில் (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        printf("cspace: %p_cnode ",
               (வெறுமை *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    எனில் (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        printf("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_வேர்_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    எனில் (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    எனில் (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        printf("fault_ep_slot: %p_ep ",
               (வெறுமை *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    எனில் (tcb->tcbSchedContext) {
        printf("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    எனில் (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        printf("temp_fault_ep_slot: %p_ep ",
               (வெறுமை *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# இல்லையெனில்
    /* Reply cap slot */
    எனில் (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        printf("reply_slot: %p_reply\n",
               (வெறுமை *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    எனில் (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        printf("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    printf("}\n");
}

/* TBD: deal with nested cnodes */
வெறுமை obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t வேர் = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    எனில் (cap_get_capType(வேர்) != cap_cnode_cap) {
        திரும்பு;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(வேர்);
    எனில் (seen(வேர்)) {
        திரும்பு;
    }
    add_to_seen(வேர்);

    printf("%p_cnode {\n", (வெறுமை *)cap_cnode_cap_get_capCNodePtr(வேர்));

    க்கு (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        எனில் (cap_get_capType(c.cap) != cap_null_cap) {
            printf("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    printf("}\n");

    க்கு (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        எனில் (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

வெறுமை obj_irq_print_maps(வெறுமை)
{
    printf("irq maps {\n");

    க்கு (seL4_Word target = 0; target < CONFIG_MAX_இலக்கம்_NODES; target++) {
        க்கு (unsigned i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            எனில் (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                எனில் (cap_get_capType(cap) != cap_null_cap) {
                    printf("%d: 0x%lx_%lu_irq\n",
                           i,
#எனில் defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (long unsigned முழு)irq.irq,
#இல்லையெனில்
                           (long unsigned முழு)irq,
#endif
                           (long unsigned முழு)target);
                }
            }
        }
    }
    printf("}\n");
}

வெறுமை obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    எனில் (isIRQActive(irq)) {
        printf("0x%lx_%lu_irq {\n",
#எனில் defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (long unsigned முழு)irq.irq,
#இல்லையெனில்
               (long unsigned முழு)irq,
#endif
               (long unsigned முழு)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        எனில் (cap_get_capType(ntfn_cap) != cap_null_cap) {
            printf("0x0: ");
            print_cap(ntfn_cap);
        }
        printf("}\n");
    }
}

வெறுமை print_objects(வெறுமை)
{
    க்கு (tcb_t *curr = முனை_STATE(ksDebugTCBs); curr != வெற்று; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        எனில் (வேர்_or_idle_tcb(curr)) {
            தொடர்;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    க்கு (tcb_t *curr = முனை_STATE(ksDebugTCBs); curr != வெற்று; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        எனில் (வேர்_or_idle_tcb(curr)) {
            தொடர்;
        }

        /* print the tcb as objects */
        printf("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        எனில் (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

வெறுமை print_caps(வெறுமை)
{
    க்கு (tcb_t *curr = முனை_STATE(ksDebugTCBs); curr != வெற்று; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        எனில் (வேர்_or_idle_tcb(curr)) {
            தொடர்;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

வெறுமை print_cap(cap_t cap)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_endpoint_cap: {
        printf("%p_ep ",
               (வெறுமை *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        நிறுத்து;
    }
    நிலை cap_notification_cap: {
        printf("%p_notification ",
               (வெறுமை *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        நிறுத்து;
    }
    நிலை cap_untyped_cap: {
        printf("%p_untyped\n",
               (வெறுமை *)cap_untyped_cap_get_capPtr(cap));
        நிறுத்து;
    }
    நிலை cap_thread_cap: {
        printf("%p_tcb\n",
               (வெறுமை *)cap_thread_cap_get_capTCBPtr(cap));
        நிறுத்து;
    }
    நிலை cap_cnode_cap: {
        printf("%p_cnode ",
               (வெறுமை *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        நிறுத்து;
    }
#ifdef CONFIG_KERNEL_MCS
    நிலை cap_reply_cap: {
        printf("%p_reply\n",
               (வெறுமை *)cap_reply_cap_get_capReplyPtr(cap));
        நிறுத்து;
    }
    நிலை cap_sched_context_cap: {
        printf("%p_sc\n",
               (வெறுமை *)cap_sched_context_cap_get_capSCPtr(cap));
        நிறுத்து;
    }
    நிலை cap_sched_control_cap: {
        printf("%lu_sched_control\n",
               (long unsigned முழு)cap_sched_control_cap_get_core(cap));
        நிறுத்து;
    }
#endif
    நிலை cap_irq_control_cap: {
        printf("irq_control\n"); /* only one in the system */
        நிறுத்து;
    }
    நிலை cap_irq_handler_cap: {
        printf("%p_%lu_irq\n",
               (வெறுமை *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned முழு)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        நிறுத்து;
    }
    இயல்பு: {
        print_cap_arch(cap);
        நிறுத்து;
    }
    }
}

வெறுமை print_object(cap_t cap)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_endpoint_cap: {
        printf("%p_ep = ep\n",
               (வெறுமை *)cap_endpoint_cap_get_capEPPtr(cap));
        நிறுத்து;
    }
    நிலை cap_notification_cap: {
        printf("%p_notification = notification\n",
               (வெறுமை *)cap_notification_cap_get_capNtfnPtr(cap));
        நிறுத்து;
    }
    நிலை cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        நிறுத்து;
    }
    நிலை cap_cnode_cap: {
        ப்ரதிஜ்ஞா(!"should not happend");
    }
#ifdef CONFIG_KERNEL_MCS
    நிலை cap_reply_cap: {
        printf("%p_reply = rtreply\n",
               (வெறுமை *)cap_reply_cap_get_capReplyPtr(cap));
        நிறுத்து;
    }
    நிலை cap_sched_context_cap: {
        printf("%p_sc = sc ",
               (வெறுமை *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        நிறுத்து;
    }
#endif
    நிலை cap_irq_handler_cap: {
        printf("%p_%lu_irq = irq\n",
               (வெறுமை *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned முழு)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        நிறுத்து;
    }
    இயல்பு:
        print_object_arch(cap);
        நிறுத்து;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
