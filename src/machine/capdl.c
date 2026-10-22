/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>

#ifdef CONFIG_DEBUG_BUILD

#അന്തര്ഭാവ <machine/capdl.h>
#അന്തര്ഭാവ <machine/registerset.h>
#അന്തര്ഭാവ <machine/timer.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <kernel/cspace.h>
#ifdef CONFIG_KERNEL_MCS
#അന്തര്ഭാവ <kernel/sporadic.h>
#endif

#define SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
പൂർണ്ണസംഖ്യ watermark = 0;

ശൂന്യം add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    എങ്കിൽ (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

ശൂന്യം reset_seen_list(ശൂന്യം)
{
    memset(seen_list, 0, SEEN_SZ * വലുപ്പം(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < watermark; i++) {
        എങ്കിൽ (same_cap(seen_list[i], c)) {
            തിരികെ true;
        }
    }
    തിരികെ false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    തിരികെ (a.വാക്കുകൾ[0] == b.വാക്കുകൾ[0] && a.വാക്കുകൾ[1] == b.വാക്കുകൾ[1]);
}

/* Return true if strings are the same */
സ്ഥിതം വരിയിൽ bool_t strings_equal(സ്ഥിരം അക്ഷരം *str1, സ്ഥിരം അക്ഷരം *str2)
{
    ആകുമ്പോൾ (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    തിരികെ !(*(സ്ഥിരം unsigned അക്ഷരം *)str1 - * (സ്ഥിരം unsigned അക്ഷരം *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t വേര്_or_idle_tcb(tcb_t *tcb)
{
    തിരികെ (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

ശൂന്യം obj_tcb_print_attrs(tcb_t *tcb)
{
    printf("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (long unsigned പൂർണ്ണസംഖ്യ)tcb->tcbIPCBuffer,
           (long unsigned പൂർണ്ണസംഖ്യ)getRestartPC(tcb),
           (long unsigned പൂർണ്ണസംഖ്യ)get_tcb_sp(tcb),
           (long unsigned പൂർണ്ണസംഖ്യ)tcb->tcbPriority,
           (long unsigned പൂർണ്ണസംഖ്യ)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    printf(", affinity: %lu", (long unsigned പൂർണ്ണസംഖ്യ)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    എങ്കിൽ (cap_get_capType(ep_cap) != cap_null_cap) {
        printf(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    printf(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

സ്ഥിതം വരിയിൽ ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    ആകുമ്പോൾ (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    തിരികെ sum;
}

ശൂന്യം obj_sc_print_attrs(cap_t sc_cap)
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

ശൂന്യം obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    printf("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (ശൂന്യം *)cap_untyped_cap_get_capPtr(slot->cap),
           (long unsigned പൂർണ്ണസംഖ്യ)cap_untyped_cap_get_capBlockSize(slot->cap),
           (long unsigned പൂർണ്ണസംഖ്യ)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    വേണ്ടി (cte_t *nextPtr = CTE_PTR(mdb_കണ്ണി_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_കണ്ണി_get_mdb_Next(slot->cteMDBNode))) {
        എങ്കിൽ (!sameRegionAs(slot->cap, nextPtr->cap)) {
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

ശൂന്യം obj_cnode_print_attrs(cap_t cnode)
{
    printf("(%lu bits)\n", (long unsigned പൂർണ്ണസംഖ്യ)cap_cnode_cap_get_capCNodeRadix(cnode));
}

ശൂന്യം obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    എങ്കിൽ (seen(cnode)) {
        തിരികെ;
    }
    add_to_seen(cnode);
    printf("%p_cnode = cnode ", (ശൂന്യം *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    വേണ്ടി (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        എങ്കിൽ (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } അല്ലെങ്കിൽ എങ്കിൽ (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } അല്ലെങ്കിൽ എങ്കിൽ (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

ശൂന്യം cap_cnode_print_attrs(cap_t cnode)
{
    printf("(guard: %lu, guard_size: %lu)\n",
           (long unsigned പൂർണ്ണസംഖ്യ)cap_cnode_cap_get_capCNodeGuard(cnode),
           (long unsigned പൂർണ്ണസംഖ്യ)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

ശൂന്യം cap_ep_print_attrs(cap_t ep)
{
    printf("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? putchar('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? putchar('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? putchar('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? putchar('P') : 0;
    long unsigned പൂർണ്ണസംഖ്യ badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

ശൂന്യം cap_ntfn_print_attrs(cap_t ntfn)
{
    printf("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? putchar('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? putchar('W') : 0;
    long unsigned പൂർണ്ണസംഖ്യ badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

/*
 * print object slots
 */

ശൂന്യം obj_tcb_print_slots(tcb_t *tcb)
{
    printf("%p_tcb {\n", tcb);

    /* CSpace root */
    എങ്കിൽ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        printf("cspace: %p_cnode ",
               (ശൂന്യം *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    എങ്കിൽ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        printf("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_വേര്_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    എങ്കിൽ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    എങ്കിൽ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        printf("fault_ep_slot: %p_ep ",
               (ശൂന്യം *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    എങ്കിൽ (tcb->tcbSchedContext) {
        printf("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    എങ്കിൽ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        printf("temp_fault_ep_slot: %p_ep ",
               (ശൂന്യം *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# അല്ലെങ്കിൽ
    /* Reply cap slot */
    എങ്കിൽ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        printf("reply_slot: %p_reply\n",
               (ശൂന്യം *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    എങ്കിൽ (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        printf("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    printf("}\n");
}

/* TBD: deal with nested cnodes */
ശൂന്യം obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t വേര് = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    എങ്കിൽ (cap_get_capType(വേര്) != cap_cnode_cap) {
        തിരികെ;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(വേര്);
    എങ്കിൽ (seen(വേര്)) {
        തിരികെ;
    }
    add_to_seen(വേര്);

    printf("%p_cnode {\n", (ശൂന്യം *)cap_cnode_cap_get_capCNodePtr(വേര്));

    വേണ്ടി (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        എങ്കിൽ (cap_get_capType(c.cap) != cap_null_cap) {
            printf("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    printf("}\n");

    വേണ്ടി (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        എങ്കിൽ (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

ശൂന്യം obj_irq_print_maps(ശൂന്യം)
{
    printf("irq maps {\n");

    വേണ്ടി (seL4_Word target = 0; target < CONFIG_MAX_അക്കം_NODES; target++) {
        വേണ്ടി (unsigned i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            എങ്കിൽ (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                എങ്കിൽ (cap_get_capType(cap) != cap_null_cap) {
                    printf("%d: 0x%lx_%lu_irq\n",
                           i,
#എങ്കിൽ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (long unsigned പൂർണ്ണസംഖ്യ)irq.irq,
#അല്ലെങ്കിൽ
                           (long unsigned പൂർണ്ണസംഖ്യ)irq,
#endif
                           (long unsigned പൂർണ്ണസംഖ്യ)target);
                }
            }
        }
    }
    printf("}\n");
}

ശൂന്യം obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    എങ്കിൽ (isIRQActive(irq)) {
        printf("0x%lx_%lu_irq {\n",
#എങ്കിൽ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (long unsigned പൂർണ്ണസംഖ്യ)irq.irq,
#അല്ലെങ്കിൽ
               (long unsigned പൂർണ്ണസംഖ്യ)irq,
#endif
               (long unsigned പൂർണ്ണസംഖ്യ)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        എങ്കിൽ (cap_get_capType(ntfn_cap) != cap_null_cap) {
            printf("0x0: ");
            print_cap(ntfn_cap);
        }
        printf("}\n");
    }
}

ശൂന്യം print_objects(ശൂന്യം)
{
    വേണ്ടി (tcb_t *curr = കണ്ണി_STATE(ksDebugTCBs); curr != ശൂന്യ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        എങ്കിൽ (വേര്_or_idle_tcb(curr)) {
            തുടരുക;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    വേണ്ടി (tcb_t *curr = കണ്ണി_STATE(ksDebugTCBs); curr != ശൂന്യ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        എങ്കിൽ (വേര്_or_idle_tcb(curr)) {
            തുടരുക;
        }

        /* print the tcb as objects */
        printf("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        എങ്കിൽ (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

ശൂന്യം print_caps(ശൂന്യം)
{
    വേണ്ടി (tcb_t *curr = കണ്ണി_STATE(ksDebugTCBs); curr != ശൂന്യ; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        എങ്കിൽ (വേര്_or_idle_tcb(curr)) {
            തുടരുക;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

ശൂന്യം print_cap(cap_t cap)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_endpoint_cap: {
        printf("%p_ep ",
               (ശൂന്യം *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        നിർത്തുക;
    }
    അവസ്ഥ cap_notification_cap: {
        printf("%p_notification ",
               (ശൂന്യം *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        നിർത്തുക;
    }
    അവസ്ഥ cap_untyped_cap: {
        printf("%p_untyped\n",
               (ശൂന്യം *)cap_untyped_cap_get_capPtr(cap));
        നിർത്തുക;
    }
    അവസ്ഥ cap_thread_cap: {
        printf("%p_tcb\n",
               (ശൂന്യം *)cap_thread_cap_get_capTCBPtr(cap));
        നിർത്തുക;
    }
    അവസ്ഥ cap_cnode_cap: {
        printf("%p_cnode ",
               (ശൂന്യം *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        നിർത്തുക;
    }
#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_reply_cap: {
        printf("%p_reply\n",
               (ശൂന്യം *)cap_reply_cap_get_capReplyPtr(cap));
        നിർത്തുക;
    }
    അവസ്ഥ cap_sched_context_cap: {
        printf("%p_sc\n",
               (ശൂന്യം *)cap_sched_context_cap_get_capSCPtr(cap));
        നിർത്തുക;
    }
    അവസ്ഥ cap_sched_control_cap: {
        printf("%lu_sched_control\n",
               (long unsigned പൂർണ്ണസംഖ്യ)cap_sched_control_cap_get_core(cap));
        നിർത്തുക;
    }
#endif
    അവസ്ഥ cap_irq_control_cap: {
        printf("irq_control\n"); /* only one in the system */
        നിർത്തുക;
    }
    അവസ്ഥ cap_irq_handler_cap: {
        printf("%p_%lu_irq\n",
               (ശൂന്യം *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned പൂർണ്ണസംഖ്യ)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി: {
        print_cap_arch(cap);
        നിർത്തുക;
    }
    }
}

ശൂന്യം print_object(cap_t cap)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_endpoint_cap: {
        printf("%p_ep = ep\n",
               (ശൂന്യം *)cap_endpoint_cap_get_capEPPtr(cap));
        നിർത്തുക;
    }
    അവസ്ഥ cap_notification_cap: {
        printf("%p_notification = notification\n",
               (ശൂന്യം *)cap_notification_cap_get_capNtfnPtr(cap));
        നിർത്തുക;
    }
    അവസ്ഥ cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        നിർത്തുക;
    }
    അവസ്ഥ cap_cnode_cap: {
        പ്രതിജ്ഞാ(!"should not happend");
    }
#ifdef CONFIG_KERNEL_MCS
    അവസ്ഥ cap_reply_cap: {
        printf("%p_reply = rtreply\n",
               (ശൂന്യം *)cap_reply_cap_get_capReplyPtr(cap));
        നിർത്തുക;
    }
    അവസ്ഥ cap_sched_context_cap: {
        printf("%p_sc = sc ",
               (ശൂന്യം *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        നിർത്തുക;
    }
#endif
    അവസ്ഥ cap_irq_handler_cap: {
        printf("%p_%lu_irq = irq\n",
               (ശൂന്യം *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned പൂർണ്ണസംഖ്യ)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
        print_object_arch(cap);
        നിർത്തുക;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
