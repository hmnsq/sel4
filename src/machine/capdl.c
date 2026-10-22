/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#ifdef CONFIG_DEBUG_BUILD

#شامل <machine/capdl.h>
#شامل <machine/registerset.h>
#شامل <machine/timer.h>
#شامل <string.h>
#شامل <kernel/cspace.h>
#ifdef CONFIG_KERNEL_MCS
#شامل <kernel/sporadic.h>
#endif

#define SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
صحیح watermark = 0;

تهی add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    اگر (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

تهی reset_seen_list(تهی)
{
    memset(seen_list, 0, SEEN_SZ * اندازه(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    برای (صحیح i = 0; i < watermark; i++) {
        اگر (same_cap(seen_list[i], c)) {
            بازگشت true;
        }
    }
    بازگشت false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    بازگشت (a.واژگان[0] == b.واژگان[0] && a.واژگان[1] == b.واژگان[1]);
}

/* Return true if strings are the same */
ایستا درونخطی bool_t strings_equal(ثابت نویسه *str1, ثابت نویسه *str2)
{
    وقتی (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    بازگشت !(*(ثابت unsigned نویسه *)str1 - * (ثابت unsigned نویسه *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t ریشه_or_idle_tcb(tcb_t *tcb)
{
    بازگشت (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

تهی obj_tcb_print_attrs(tcb_t *tcb)
{
    printf("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (long unsigned صحیح)tcb->tcbIPCBuffer,
           (long unsigned صحیح)getRestartPC(tcb),
           (long unsigned صحیح)get_tcb_sp(tcb),
           (long unsigned صحیح)tcb->tcbPriority,
           (long unsigned صحیح)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    printf(", affinity: %lu", (long unsigned صحیح)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    اگر (cap_get_capType(ep_cap) != cap_null_cap) {
        printf(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    printf(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

ایستا درونخطی ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    وقتی (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    بازگشت sum;
}

تهی obj_sc_print_attrs(cap_t sc_cap)
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

تهی obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    printf("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (تهی *)cap_untyped_cap_get_capPtr(slot->cap),
           (long unsigned صحیح)cap_untyped_cap_get_capBlockSize(slot->cap),
           (long unsigned صحیح)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    برای (cte_t *nextPtr = CTE_PTR(mdb_گره_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_گره_get_mdb_Next(slot->cteMDBNode))) {
        اگر (!sameRegionAs(slot->cap, nextPtr->cap)) {
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

تهی obj_cnode_print_attrs(cap_t cnode)
{
    printf("(%lu bits)\n", (long unsigned صحیح)cap_cnode_cap_get_capCNodeRadix(cnode));
}

تهی obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    اگر (seen(cnode)) {
        بازگشت;
    }
    add_to_seen(cnode);
    printf("%p_cnode = cnode ", (تهی *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    برای (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        اگر (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } وگرنه اگر (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } وگرنه اگر (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

تهی cap_cnode_print_attrs(cap_t cnode)
{
    printf("(guard: %lu, guard_size: %lu)\n",
           (long unsigned صحیح)cap_cnode_cap_get_capCNodeGuard(cnode),
           (long unsigned صحیح)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

تهی cap_ep_print_attrs(cap_t ep)
{
    printf("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? putchar('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? putchar('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? putchar('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? putchar('P') : 0;
    long unsigned صحیح badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

تهی cap_ntfn_print_attrs(cap_t ntfn)
{
    printf("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? putchar('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? putchar('W') : 0;
    long unsigned صحیح badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

/*
 * print object slots
 */

تهی obj_tcb_print_slots(tcb_t *tcb)
{
    printf("%p_tcb {\n", tcb);

    /* CSpace root */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        printf("cspace: %p_cnode ",
               (تهی *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        printf("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_ریشه_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        printf("fault_ep_slot: %p_ep ",
               (تهی *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    اگر (tcb->tcbSchedContext) {
        printf("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        printf("temp_fault_ep_slot: %p_ep ",
               (تهی *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# وگرنه
    /* Reply cap slot */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        printf("reply_slot: %p_reply\n",
               (تهی *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    اگر (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        printf("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    printf("}\n");
}

/* TBD: deal with nested cnodes */
تهی obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t ریشه = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    اگر (cap_get_capType(ریشه) != cap_cnode_cap) {
        بازگشت;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(ریشه);
    اگر (seen(ریشه)) {
        بازگشت;
    }
    add_to_seen(ریشه);

    printf("%p_cnode {\n", (تهی *)cap_cnode_cap_get_capCNodePtr(ریشه));

    برای (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        اگر (cap_get_capType(c.cap) != cap_null_cap) {
            printf("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    printf("}\n");

    برای (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        اگر (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

تهی obj_irq_print_maps(تهی)
{
    printf("irq maps {\n");

    برای (seL4_Word target = 0; target < CONFIG_MAX_رقم_NODES; target++) {
        برای (unsigned i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            اگر (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                اگر (cap_get_capType(cap) != cap_null_cap) {
                    printf("%d: 0x%lx_%lu_irq\n",
                           i,
#اگر defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (long unsigned صحیح)irq.irq,
#وگرنه
                           (long unsigned صحیح)irq,
#endif
                           (long unsigned صحیح)target);
                }
            }
        }
    }
    printf("}\n");
}

تهی obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    اگر (isIRQActive(irq)) {
        printf("0x%lx_%lu_irq {\n",
#اگر defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (long unsigned صحیح)irq.irq,
#وگرنه
               (long unsigned صحیح)irq,
#endif
               (long unsigned صحیح)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        اگر (cap_get_capType(ntfn_cap) != cap_null_cap) {
            printf("0x0: ");
            print_cap(ntfn_cap);
        }
        printf("}\n");
    }
}

تهی print_objects(تهی)
{
    برای (tcb_t *curr = گره_STATE(ksDebugTCBs); curr != پوچ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        اگر (ریشه_or_idle_tcb(curr)) {
            ادامه;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    برای (tcb_t *curr = گره_STATE(ksDebugTCBs); curr != پوچ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        اگر (ریشه_or_idle_tcb(curr)) {
            ادامه;
        }

        /* print the tcb as objects */
        printf("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        اگر (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

تهی print_caps(تهی)
{
    برای (tcb_t *curr = گره_STATE(ksDebugTCBs); curr != پوچ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        اگر (ریشه_or_idle_tcb(curr)) {
            ادامه;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

تهی print_cap(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_endpoint_cap: {
        printf("%p_ep ",
               (تهی *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        بشکن;
    }
    حالت cap_notification_cap: {
        printf("%p_notification ",
               (تهی *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        بشکن;
    }
    حالت cap_untyped_cap: {
        printf("%p_untyped\n",
               (تهی *)cap_untyped_cap_get_capPtr(cap));
        بشکن;
    }
    حالت cap_thread_cap: {
        printf("%p_tcb\n",
               (تهی *)cap_thread_cap_get_capTCBPtr(cap));
        بشکن;
    }
    حالت cap_cnode_cap: {
        printf("%p_cnode ",
               (تهی *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        بشکن;
    }
#ifdef CONFIG_KERNEL_MCS
    حالت cap_reply_cap: {
        printf("%p_reply\n",
               (تهی *)cap_reply_cap_get_capReplyPtr(cap));
        بشکن;
    }
    حالت cap_sched_context_cap: {
        printf("%p_sc\n",
               (تهی *)cap_sched_context_cap_get_capSCPtr(cap));
        بشکن;
    }
    حالت cap_sched_control_cap: {
        printf("%lu_sched_control\n",
               (long unsigned صحیح)cap_sched_control_cap_get_core(cap));
        بشکن;
    }
#endif
    حالت cap_irq_control_cap: {
        printf("irq_control\n"); /* only one in the system */
        بشکن;
    }
    حالت cap_irq_handler_cap: {
        printf("%p_%lu_irq\n",
               (تهی *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned صحیح)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        بشکن;
    }
    پیشفرض: {
        print_cap_arch(cap);
        بشکن;
    }
    }
}

تهی print_object(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_endpoint_cap: {
        printf("%p_ep = ep\n",
               (تهی *)cap_endpoint_cap_get_capEPPtr(cap));
        بشکن;
    }
    حالت cap_notification_cap: {
        printf("%p_notification = notification\n",
               (تهی *)cap_notification_cap_get_capNtfnPtr(cap));
        بشکن;
    }
    حالت cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        بشکن;
    }
    حالت cap_cnode_cap: {
        أكد(!"should not happend");
    }
#ifdef CONFIG_KERNEL_MCS
    حالت cap_reply_cap: {
        printf("%p_reply = rtreply\n",
               (تهی *)cap_reply_cap_get_capReplyPtr(cap));
        بشکن;
    }
    حالت cap_sched_context_cap: {
        printf("%p_sc = sc ",
               (تهی *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        بشکن;
    }
#endif
    حالت cap_irq_handler_cap: {
        printf("%p_%lu_irq = irq\n",
               (تهی *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned صحیح)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        بشکن;
    }
    پیشفرض:
        print_object_arch(cap);
        بشکن;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
