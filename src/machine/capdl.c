/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>

#ifdef CONFIG_DEBUG_BUILD

#include <machine/capdl.h>
#include <machine/registerset.h>
#include <machine/timer.h>
#include <string.h>
#include <kernel/cspace.h>
#ifdef CONFIG_KERNEL_MCS
#include <kernel/sporadic.h>
#endif

#define SEEN_SZ 256

/* seen list - check this array before we print cnode and vspace */
/* TBD: This is to avoid traversing the same cnode. It should be applied to object
 * as well since the extractor might comes across multiple caps to the same object.
 */
cap_t seen_list[SEEN_SZ];
ཧྲིལ watermark = 0;

void add_to_seen(cap_t c)
{
    /* Won't work well if there're more than SEEN_SZ cnode */
    གལ་སྲིད (watermark < SEEN_SZ) {
        seen_list[watermark] = c;
        watermark++;
    }
}

void reset_seen_list(void)
{
    memset(seen_list, 0, SEEN_SZ * parimaa_na(seen_list[0]));
    watermark = 0;
}

bool_t seen(cap_t c)
{
    དོན་ལུ (ཧྲིལ i = 0; i < watermark; i++) {
        གལ་སྲིད (same_cap(seen_list[i], c)) {
            ལོག true;
        }
    }
    ལོག false;
}

bool_t same_cap(cap_t a, cap_t b)
{
    ལོག (a.words[0] == b.words[0] && a.words[1] == b.words[1]);
}

/* Return true if strings are the same */
static anta_hstha bool_t strings_equal(const var_na *str1, const var_na *str2)
{
    སྐབས (*str1 && *str2 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    ལོག !(*(const unsigned var_na *)str1 - * (const unsigned var_na *)str2);
}

/* Return true if the tcb is for rootserver or idle thread */
bool_t root_or_idle_tcb(tcb_t *tcb)
{
    ལོག (strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "rootserver")
            || strings_equal(TCB_PTR_DEBUG_PTR(tcb)->tcbName, "idle_thread"));
}

/*
 * Print objects
 */

#ifdef CONFIG_PRINTING

void obj_tcb_print_attrs(tcb_t *tcb)
{
    printf("(addr: 0x%lx, ip: 0x%lx, sp: 0x%lx, prio: %lu, max_prio: %lu",
           (long unsigned ཧྲིལ)tcb->tcbIPCBuffer,
           (long unsigned ཧྲིལ)getRestartPC(tcb),
           (long unsigned ཧྲིལ)get_tcb_sp(tcb),
           (long unsigned ཧྲིལ)tcb->tcbPriority,
           (long unsigned ཧྲིལ)tcb->tcbMCP);

#ifdef ENABLE_SMP_SUPPORT
    printf(", affinity: %lu", (long unsigned ཧྲིལ)tcb->tcbAffinity);
#endif /* ENABLE_SMP_SUPPORT */

    /* init */

#ifdef CONFIG_KERNEL_MCS
    cap_t ep_cap = TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap;
    གལ་སྲིད (cap_get_capType(ep_cap) != cap_null_cap) {
        printf(", fault_ep: %p", EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap)));
    }
#endif

    printf(", dom: %ld)\n", tcb->tcbDomain);
}

#ifdef CONFIG_KERNEL_MCS

static anta_hstha ticks_t sc_get_budget(sched_context_t *sc)
{
    ticks_t sum = refill_head(sc)->rAmount;
    word_t current = sc->scRefillHead;

    སྐབས (current != sc->scRefillTail) {
        current = ((current == sc->scRefillMax - 1u) ? (0) : current + 1u);
        sum += refill_index(sc, current)->rAmount;
    }

    ལོག sum;
}

void obj_sc_print_attrs(cap_t sc_cap)
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

void obj_ut_print_attrs(cte_t *slot, tcb_t *tcb)
{
    /* might have two untypeds with the same address but different size */
    printf("%p_%lu_untyped = ut (%lu bits, paddr: %p) {",
           (void *)cap_untyped_cap_get_capPtr(slot->cap),
           (long unsigned ཧྲིལ)cap_untyped_cap_get_capBlockSize(slot->cap),
           (long unsigned ཧྲིལ)cap_untyped_cap_get_capBlockSize(slot->cap),
           WORD_PTR(cap_untyped_cap_get_capPtr(slot->cap)));

    /* there is no need to check for a NullCap as NullCaps are
    always accompanied by null mdb pointers */
    དོན་ལུ (cte_t *nextPtr = CTE_PTR(mdb_node_get_mdbNext(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_node_get_mdbNext(slot->cteMDBNode))) {
        གལ་སྲིད (!sameRegionAs(slot->cap, nextPtr->cap)) {
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

void obj_cnode_print_attrs(cap_t cnode)
{
    printf("(%lu bits)\n", (long unsigned ཧྲིལ)cap_cnode_cap_get_capCNodeRadix(cnode));
}

void obj_tcb_print_cnodes(cap_t cnode, tcb_t *tcb)
{
    གལ་སྲིད (seen(cnode)) {
        ལོག;
    }
    add_to_seen(cnode);
    printf("%p_cnode = cnode ", (void *)cap_cnode_cap_get_capCNodePtr(cnode));
    obj_cnode_print_attrs(cnode);
    word_t radix = cap_cnode_cap_get_capCNodeRadix(cnode);

    དོན་ལུ (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        གལ་སྲིད (cap_get_capType(c.cap) == cap_untyped_cap) {
            /* we need `cte_t *` to print out the slots of an untyped object */
            obj_ut_print_attrs(c.slot, tcb);

        } དེ་མིན གལ་སྲིད (cap_get_capType(c.cap) == cap_cnode_cap) {
            /* TBD: deal with nested cnodes */

        } དེ་མིན གལ་སྲིད (cap_get_capType(c.cap) != cap_null_cap) {
            print_object(c.cap);
        }
    }
}

/*
 * Caps
 */

void cap_cnode_print_attrs(cap_t cnode)
{
    printf("(guard: %lu, guard_size: %lu)\n",
           (long unsigned ཧྲིལ)cap_cnode_cap_get_capCNodeGuard(cnode),
           (long unsigned ཧྲིལ)cap_cnode_cap_get_capCNodeGuardSize(cnode));
}

void cap_ep_print_attrs(cap_t ep)
{
    printf("(");
    cap_endpoint_cap_get_capCanReceive(ep) ? putchar('R') : 0;
    cap_endpoint_cap_get_capCanSend(ep) ? putchar('W') : 0;
    cap_endpoint_cap_get_capCanGrant(ep) ? putchar('G') : 0;
    cap_endpoint_cap_get_capCanGrantReply(ep) ? putchar('P') : 0;
    long unsigned ཧྲིལ badge = cap_endpoint_cap_get_capEPBadge(ep);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

void cap_ntfn_print_attrs(cap_t ntfn)
{
    printf("(");
    cap_notification_cap_get_capNtfnCanReceive(ntfn) ? putchar('R') : 0;
    cap_notification_cap_get_capNtfnCanSend(ntfn) ? putchar('W') : 0;
    long unsigned ཧྲིལ badge = cap_notification_cap_get_capNtfnBadge(ntfn);
    badge ? printf(", badge: %lu)\n", badge) : printf(")\n");
}

/*
 * print object slots
 */

void obj_tcb_print_slots(tcb_t *tcb)
{
    printf("%p_tcb {\n", tcb);

    /* CSpace root */
    གལ་སྲིད (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap) != cap_null_cap) {
        printf("cspace: %p_cnode ",
               (void *)cap_cnode_cap_get_capCNodePtr(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap));
        cap_cnode_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap);
    }

    /* VSpace root */
    གལ་སྲིད (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) != cap_null_cap) {
        printf("vspace: %p_pd\n",
               cap_vtable_cap_get_vspace_root_fp(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

    }

    /* IPC buffer cap slot */
    གལ་སྲིད (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbBuffer)->cap) != cap_null_cap) {
        /* TBD: print out the bound vcpu */
        print_ipc_buffer_slot(tcb);
    }

#ifdef CONFIG_KERNEL_MCS

    /* Fault endpoint slot */
    གལ་སྲིད (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap) != cap_null_cap) {
        printf("fault_ep_slot: %p_ep ",
               (void *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbFaultHandler)->cap);
    }

    /* sc */
    གལ་སྲིད (tcb->tcbSchedContext) {
        printf("sc_slot: %p_sc\n", tcb->tcbSchedContext);
    }

    /* Timeout endpoint slot */
    གལ་སྲིད (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap) != cap_null_cap) {
        printf("temp_fault_ep_slot: %p_ep ",
               (void *)cap_endpoint_cap_get_capEPPtr(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap));
        cap_ep_print_attrs(TCB_PTR_CTE_PTR(tcb, tcbTimeoutHandler)->cap);
    }

# དེ་མིན
    /* Reply cap slot */
    གལ་སྲིད (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap) != cap_null_cap) {
        printf("reply_slot: %p_reply\n",
               (void *)cap_reply_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbReply)->cap));
    }

    /* TCB of most recent IPC sender */
    གལ་སྲིད (cap_get_capType(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap) != cap_null_cap) {
        tcb_t *caller = TCB_PTR(cap_thread_cap_get_capTCBPtr(TCB_PTR_CTE_PTR(tcb, tcbCaller)->cap));
        printf("caller_slot: %p_tcb\n", caller);
    }
#endif /* CONFIG_KERNEL_MCS */
    printf("}\n");
}

/* TBD: deal with nested cnodes */
void obj_cnode_print_slots(tcb_t *tcb)
{
    cap_t root = TCB_PTR_CTE_PTR(tcb, tcbCTable)->cap;
    གལ་སྲིད (cap_get_capType(root) != cap_cnode_cap) {
        ལོག;
    }

    word_t radix = cap_cnode_cap_get_capCNodeRadix(root);
    གལ་སྲིད (seen(root)) {
        ལོག;
    }
    add_to_seen(root);

    printf("%p_cnode {\n", (void *)cap_cnode_cap_get_capCNodePtr(root));

    དོན་ལུ (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        གལ་སྲིད (cap_get_capType(c.cap) != cap_null_cap) {
            printf("0x%x: ", i);
            print_cap(c.cap);
        }
    }
    printf("}\n");

    དོན་ལུ (uint32_t i = 0; i < (1 << radix); i++) {
        lookupCapAndSlot_ret_t c = lookupCapAndSlot(tcb, i);
        གལ་སྲིད (cap_get_capType(c.cap) == cap_irq_handler_cap) {
            /* TBD: should instead print it from IRQNode */
            obj_irq_print_slots(c.cap);
        }
    }
}

void obj_irq_print_maps(void)
{
    printf("irq maps {\n");

    དོན་ལུ (seL4_Word target = 0; target < CONFIG_MAX_NUM_NODES; target++) {
        དོན་ལུ (unsigned i = 0; i <= maxIRQ; i++) {
            irq_t irq = CORE_IRQ_TO_IRQT(target, i);
            གལ་སྲིད (isIRQActive(irq)) {
                cap_t cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
                གལ་སྲིད (cap_get_capType(cap) != cap_null_cap) {
                    printf("%d: 0x%lx_%lu_irq\n",
                           i,
#གལ་སྲིད defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
                           (long unsigned ཧྲིལ)irq.irq,
#དེ་མིན
                           (long unsigned ཧྲིལ)irq,
#endif
                           (long unsigned ཧྲིལ)target);
                }
            }
        }
    }
    printf("}\n");
}

void obj_irq_print_slots(cap_t irq_cap)
{
    irq_t irq = IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(irq_cap));
    གལ་སྲིད (isIRQActive(irq)) {
        printf("0x%lx_%lu_irq {\n",
#གལ་སྲིད defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_ARCH_ARM)
               (long unsigned ཧྲིལ)irq.irq,
#དེ་མིན
               (long unsigned ཧྲིལ)irq,
#endif
               (long unsigned ཧྲིལ)IRQT_TO_CORE(irq));
        cap_t ntfn_cap = intStateIRQNode[IRQT_TO_IDX(irq)].cap;
        གལ་སྲིད (cap_get_capType(ntfn_cap) != cap_null_cap) {
            printf("0x0: ");
            print_cap(ntfn_cap);
        }
        printf("}\n");
    }
}

void print_objects(void)
{
    དོན་ལུ (tcb_t *curr = NODE_STATE(ksDebugTCBs); curr != NULL; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        གལ་སྲིད (root_or_idle_tcb(curr)) {
            continue;
        }
        /* print the contains of the tcb's vtable as objects */
        obj_tcb_print_vtable(curr);
    }

    དོན་ལུ (tcb_t *curr = NODE_STATE(ksDebugTCBs); curr != NULL; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        གལ་སྲིད (root_or_idle_tcb(curr)) {
            continue;
        }

        /* print the tcb as objects */
        printf("%p_tcb = tcb ", curr);
        obj_tcb_print_attrs(curr);

        /* print the contains of the tcb's ctable as objects */
        གལ་སྲིད (cap_get_capType(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap) == cap_cnode_cap) {
            obj_tcb_print_cnodes(TCB_PTR_CTE_PTR(curr, tcbCTable)->cap, curr);
        }
    }
}

void print_caps(void)
{
    དོན་ལུ (tcb_t *curr = NODE_STATE(ksDebugTCBs); curr != NULL; curr = TCB_PTR_DEBUG_PTR(curr)->tcbDebugNext) {
        གལ་སྲིད (root_or_idle_tcb(curr)) {
            continue;
        }
        obj_cnode_print_slots(curr);
        obj_vtable_print_slots(curr);
        obj_tcb_print_slots(curr);
    }
}

void print_cap(cap_t cap)
{
    switch (cap_get_capType(cap)) {
    pak_sa cap_endpoint_cap: {
        printf("%p_ep ",
               (void *)cap_endpoint_cap_get_capEPPtr(cap));
        cap_ep_print_attrs(cap);
        bha_nga;
    }
    pak_sa cap_notification_cap: {
        printf("%p_notification ",
               (void *)cap_notification_cap_get_capNtfnPtr(cap));
        cap_ntfn_print_attrs(cap);
        bha_nga;
    }
    pak_sa cap_untyped_cap: {
        printf("%p_untyped\n",
               (void *)cap_untyped_cap_get_capPtr(cap));
        bha_nga;
    }
    pak_sa cap_thread_cap: {
        printf("%p_tcb\n",
               (void *)cap_thread_cap_get_capTCBPtr(cap));
        bha_nga;
    }
    pak_sa cap_cnode_cap: {
        printf("%p_cnode ",
               (void *)cap_cnode_cap_get_capCNodePtr(cap));
        cap_cnode_print_attrs(cap);
        bha_nga;
    }
#ifdef CONFIG_KERNEL_MCS
    pak_sa cap_reply_cap: {
        printf("%p_reply\n",
               (void *)cap_reply_cap_get_capReplyPtr(cap));
        bha_nga;
    }
    pak_sa cap_sched_context_cap: {
        printf("%p_sc\n",
               (void *)cap_sched_context_cap_get_capSCPtr(cap));
        bha_nga;
    }
    pak_sa cap_sched_control_cap: {
        printf("%lu_sched_control\n",
               (long unsigned ཧྲིལ)cap_sched_control_cap_get_core(cap));
        bha_nga;
    }
#endif
    pak_sa cap_irq_control_cap: {
        printf("irq_control\n"); /* only one in the system */
        bha_nga;
    }
    pak_sa cap_irq_handler_cap: {
        printf("%p_%lu_irq\n",
               (void *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned ཧྲིལ)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        bha_nga;
    }
    default: {
        print_cap_arch(cap);
        bha_nga;
    }
    }
}

void print_object(cap_t cap)
{
    switch (cap_get_capType(cap)) {
    pak_sa cap_endpoint_cap: {
        printf("%p_ep = ep\n",
               (void *)cap_endpoint_cap_get_capEPPtr(cap));
        bha_nga;
    }
    pak_sa cap_notification_cap: {
        printf("%p_notification = notification\n",
               (void *)cap_notification_cap_get_capNtfnPtr(cap));
        bha_nga;
    }
    pak_sa cap_thread_cap: {
        /* this object has already got handle by `print_objects` */
        bha_nga;
    }
    pak_sa cap_cnode_cap: {
        assert(!"should not happend");
    }
#ifdef CONFIG_KERNEL_MCS
    pak_sa cap_reply_cap: {
        printf("%p_reply = rtreply\n",
               (void *)cap_reply_cap_get_capReplyPtr(cap));
        bha_nga;
    }
    pak_sa cap_sched_context_cap: {
        printf("%p_sc = sc ",
               (void *)cap_sched_context_cap_get_capSCPtr(cap));
        obj_sc_print_attrs(cap);
        bha_nga;
    }
#endif
    pak_sa cap_irq_handler_cap: {
        printf("%p_%lu_irq = irq\n",
               (void *)cap_irq_handler_cap_get_capIRQ(cap),
               (long unsigned ཧྲིལ)IRQT_TO_CORE(IDX_TO_IRQT(cap_irq_handler_cap_get_capIRQ(cap))));
        bha_nga;
    }
    default:
        print_object_arch(cap);
        bha_nga;
    }
}

#endif /* CONFIG_PRINTING */

#endif /* CONFIG_DEBUG_BUILD */
