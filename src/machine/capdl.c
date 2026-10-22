/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#ಅನ್ತರ್ಭಾವ <machine/capdl.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <machine/timer.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ifdef CONFIG_KERNEL_MCS
#ಅನ್ತರ್ಭಾವ <kernel/sporadic.h>
#endif

#define SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
ಪೂರ್ಣಾಙ್ಕ watermark = 0;

ಶೂನ್ಯ add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    ಯದಿ (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

ಶೂನ್ಯ reset_seen_list(ಶೂನ್ಯ)
{
    memset(seen_list, 0, SEEN_SZ * ಪರಿಮಾಣ(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < watermark; i++) {
        ಯದಿ (same_cap(seen_list[i], c)) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
    }
    ಪ್ರತ್ಯಾಗಮ false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    ಪ್ರತ್ಯಾಗಮ (a.ಪದಗಳು[0] == b.ಪದಗಳು[0] && a.ಪದಗಳು[1] == b.ಪದಗಳು[1]);
}

/* Return true if strings are the same */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t strings_equal(ಧ್ರುವ ವರ್ಣ *str1, ಧ್ರುವ ವರ್ಣ *str2)
{
    ಯಾವತ್ (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    ಪ್ರತ್ಯಾಗಮ !(*(ಧ್ರುವ unsigned ವರ್ಣ *)str1 - * (ಧ್ರುವ unsigned ವರ್ಣ *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t ಬೇರು_or_idle_tcb(tcb_t *tcb)
{
    ಪ್ರತ್ಯಾಗಮ (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

ಶೂನ್ಯ obj_tcb_print_attrs(tcb_t *tcb)
{
    printf("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (long unsigned ಪೂರ್ಣಾಙ್ಕ)tcb->tcbIPCBuffer,
           (long unsigned ಪೂರ್ಣಾಙ್ಕ)getRestartPC(tcb),
           (long unsigned ಪೂರ್ಣಾಙ್ಕ)get_tcb_sp(tcb),
           (long unsigned ಪೂರ್ಣಾಙ್ಕ)tcb->tcbPriority,
           (long unsigned ಪೂರ್ಣಾಙ್ಕ)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    printf(", affinity: %lu", (long unsigned ಪೂರ್ಣಾಙ್ಕ)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    ಯದಿ (cap_get_capType(ep_cap) != cap_null_cap) {
        printf(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    printf(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    ಯಾವತ್ (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    ಪ್ರತ್ಯಾಗಮ sum;
}

ಶೂನ್ಯ obj_sc_print_attrs(cap_t sc_cap)
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

ಶೂನ್ಯ obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    printf("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (ಶೂನ್ಯ *)cap_untyped_cap_get_capPtr(slot->cap),
           (long unsigned ಪೂರ್ಣಾಙ್ಕ)cap_untyped_cap_get_capBlockSize(slot->cap),
           (long unsigned ಪೂರ್ಣಾಙ್ಕ)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    ಪ್ರತಿ (cte_t *nextPtr = CTE_PTR(mdb_ಗಂಟು_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_ಗಂಟು_get_mdb_Next(slot->cteMDBNode))) {
        ಯದಿ (!sameRegionAs(slot->cap, nextPtr->cap)) {
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

ಶೂನ್ಯ obj_cnode_print_attrs(cap_t cnode)
{
    printf("(%lu bits)\n", (long unsigned ಪೂರ್ಣಾಙ್ಕ)cap_cnode_cap_get_capCNodeRadix(cnode));
}

ಶೂನ್ಯ obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    ಯದಿ (seen(cnode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    add_to_seen(cnode);
    printf("%p_cnode = cnode ", (ಶೂನ್ಯ *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    ಪ್ರತಿ (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        ಯದಿ (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } ಅನ್ಯಥಾ ಯದಿ (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } ಅನ್ಯಥಾ ಯದಿ (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

ಶೂನ್ಯ cap_cnode_print_attrs(cap_t cnode)
{
    printf("(guard: %lu, guard_size: %lu)\n",
           (long unsigned ಪೂರ್ಣಾಙ್ಕ)cap_cnode_cap_get_capCNodeGuard(cnode),
           (long unsigned ಪೂರ್ಣಾಙ್ಕ)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

ಶೂನ್ಯ cap_ep_print_attrs(cap_t ep)
{
    printf("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? putchar('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? putchar('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? putchar('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? putchar('P') : 0;
    long unsigned ಪೂರ್ಣಾಙ್ಕ badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

ಶೂನ್ಯ cap_ntfn_print_attrs(cap_t ntfn)
{
    printf("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? putchar('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? putchar('W') : 0;
    long unsigned ಪೂರ್ಣಾಙ್ಕ badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

/*
 * print object slots
 */

ಶೂನ್ಯ obj_tcb_print_slots(tcb_t *tcb)
{
    printf("%p_tcb {\n", tcb);

    /* CSpace root */
    ಯದಿ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        printf("cspace: %p_cnode ",
               (ಶೂನ್ಯ *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    ಯದಿ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        printf("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_ಬೇರು_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    ಯದಿ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    ಯದಿ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        printf("fault_ep_slot: %p_ep ",
               (ಶೂನ್ಯ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    ಯದಿ (tcb->tcbSchedContext) {
        printf("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    ಯದಿ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        printf("temp_fault_ep_slot: %p_ep ",
               (ಶೂನ್ಯ *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# ಅನ್ಯಥಾ
    /* Reply cap slot */
    ಯದಿ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        printf("reply_slot: %p_reply\n",
               (ಶೂನ್ಯ *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    ಯದಿ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        printf("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    printf("}\n");
}

/* TBD: deal with nested cnodes */
ಶೂನ್ಯ obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t ಬೇರು = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    ಯದಿ (cap_get_capType(ಬೇರು) != cap_cnode_cap) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(ಬೇರು);
    ಯದಿ (seen(ಬೇರು)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    add_to_seen(ಬೇರು);

    printf("%p_cnode {\n", (ಶೂನ್ಯ *)cap_cnode_cap_get_capCNodePtr(ಬೇರು));

    ಪ್ರತಿ (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        ಯದಿ (cap_get_capType(c.cap) != cap_null_cap) {
            printf("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    printf("}\n");

    ಪ್ರತಿ (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        ಯದಿ (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

ಶೂನ್ಯ obj_irq_print_maps(ಶೂನ್ಯ)
{
    printf("irq maps {\n");

    ಪ್ರತಿ (seL4_Word target = 0; target < CONFIG_MAX_ಅಂಕೆ_NODES; target++) {
        ಪ್ರತಿ (unsigned i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            ಯದಿ (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                ಯದಿ (cap_get_capType(cap) != cap_null_cap) {
                    printf("%d: 0x%lx_%lu_irq\n",
                           i,
#ಯದಿ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (long unsigned ಪೂರ್ಣಾಙ್ಕ)irq.irq,
#ಅನ್ಯಥಾ
                           (long unsigned ಪೂರ್ಣಾಙ್ಕ)irq,
#endif
                           (long unsigned ಪೂರ್ಣಾಙ್ಕ)target);
                }
            }
        }
    }
    printf("}\n");
}

ಶೂನ್ಯ obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    ಯದಿ (isIRQActive(irq)) {
        printf("0x%lx_%lu_irq {\n",
#ಯದಿ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (long unsigned ಪೂರ್ಣಾಙ್ಕ)irq.irq,
#ಅನ್ಯಥಾ
               (long unsigned ಪೂರ್ಣಾಙ್ಕ)irq,
#endif
               (long unsigned ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        ಯದಿ (cap_get_capType(ntfn_cap) != cap_null_cap) {
            printf("0x0: ");
            print_cap(ntfn_cap);
        }
        printf("}\n");
    }
}

ಶೂನ್ಯ print_objects(ಶೂನ್ಯ)
{
    ಪ್ರತಿ (tcb_t *curr = ಗಂಟು_STATE(ksDebugTCBs); curr != ರಿಕ್ತ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        ಯದಿ (ಬೇರು_or_idle_tcb(curr)) {
            ಅನುವರ್ತ;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    ಪ್ರತಿ (tcb_t *curr = ಗಂಟು_STATE(ksDebugTCBs); curr != ರಿಕ್ತ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        ಯದಿ (ಬೇರು_or_idle_tcb(curr)) {
            ಅನುವರ್ತ;
        }

        /* print the tcb as objects */
        printf("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        ಯದಿ (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

ಶೂನ್ಯ print_caps(ಶೂನ್ಯ)
{
    ಪ್ರತಿ (tcb_t *curr = ಗಂಟು_STATE(ksDebugTCBs); curr != ರಿಕ್ತ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        ಯದಿ (ಬೇರು_or_idle_tcb(curr)) {
            ಅನುವರ್ತ;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

ಶೂನ್ಯ print_cap(cap_t cap)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_endpoint_cap: {
        printf("%p_ep ",
               (ಶೂನ್ಯ *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_notification_cap: {
        printf("%p_notification ",
               (ಶೂನ್ಯ *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_untyped_cap: {
        printf("%p_untyped\n",
               (ಶೂನ್ಯ *)cap_untyped_cap_get_capPtr(cap));
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_thread_cap: {
        printf("%p_tcb\n",
               (ಶೂನ್ಯ *)cap_thread_cap_get_capTCBPtr(cap));
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_cnode_cap: {
        printf("%p_cnode ",
               (ಶೂನ್ಯ *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        ಭಙ್ಗ;
    }
#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_reply_cap: {
        printf("%p_reply\n",
               (ಶೂನ್ಯ *)cap_reply_cap_get_capReplyPtr(cap));
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_sched_context_cap: {
        printf("%p_sc\n",
               (ಶೂನ್ಯ *)cap_sched_context_cap_get_capSCPtr(cap));
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_sched_control_cap: {
        printf("%lu_sched_control\n",
               (long unsigned ಪೂರ್ಣಾಙ್ಕ)cap_sched_control_cap_get_core(cap));
        ಭಙ್ಗ;
    }
#endif
    ಪಕ್ಷ cap_irq_control_cap: {
        printf("irq_control\n"); /* only one in the system */
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_irq_handler_cap: {
        printf("%p_%lu_irq\n",
               (ಶೂನ್ಯ *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ: {
        print_cap_arch(cap);
        ಭಙ್ಗ;
    }
    }
}

ಶೂನ್ಯ print_object(cap_t cap)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_endpoint_cap: {
        printf("%p_ep = ep\n",
               (ಶೂನ್ಯ *)cap_endpoint_cap_get_capEPPtr(cap));
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_notification_cap: {
        printf("%p_notification = notification\n",
               (ಶೂನ್ಯ *)cap_notification_cap_get_capNtfnPtr(cap));
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_cnode_cap: {
        ಪ್ರತಿಜ್ಞಾ(!"should not happend");
    }
#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ cap_reply_cap: {
        printf("%p_reply = rtreply\n",
               (ಶೂನ್ಯ *)cap_reply_cap_get_capReplyPtr(cap));
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ cap_sched_context_cap: {
        printf("%p_sc = sc ",
               (ಶೂನ್ಯ *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        ಭಙ್ಗ;
    }
#endif
    ಪಕ್ಷ cap_irq_handler_cap: {
        printf("%p_%lu_irq = irq\n",
               (ಶೂನ್ಯ *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned ಪೂರ್ಣಾಙ್ಕ)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        print_object_arch(cap);
        ಭಙ್ಗ;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
