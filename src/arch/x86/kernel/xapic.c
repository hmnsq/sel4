/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <arch/kernel/xapic.h>
#অন্তর্ভাব <arch/kernel/x2apic.h>

#ifdef CONFIG_XAPIC
#ifdef CONFIG_USE_LOGICAL_IDS
/* using flat cluster mode we only support 8 cores */
compile_assert(সংখ্যা_of_cores_invalid_for_logical_ids, CONFIG_MAX_অঙ্ক_NODES <= 8)

BOOT_CODE স্থির শূন্য
init_xapic_ldr(শূন্য)
{
    uint32_t ldr;

    apic_write_reg(APIC_DEST_FORMAT, XAPIC_DFR_FLAT);
    ldr = apic_read_reg(APIC_LOGICAL_DEST) & MASK(XAPIC_LDR_SHIFT);
    ldr |= (BIT(getCurrentCPUIndex()) << XAPIC_LDR_SHIFT);
    apic_write_reg(APIC_LOGICAL_DEST, ldr);
}
#endif /* CONFIG_USE_LOGICAL_IDS */

BOOT_CODE bool_t apic_enable(শূন্য)
{
    apic_base_msr_t apic_base_msr;
    apic_base_msr.শব্দগুলি[0] = x86_rdmsr_নিচের(IA32_APIC_BASE_MSR);

    যদি (!apic_base_msr_get_enabled(apic_base_msr)) {
        printf("APIC: Enabled bit not set\n");
        প্রত্যাগম false;
    }

    যদি (x2apic_is_enabled()) {
        printf("x2APIC enabled in BIOS but kernel does not support that\n");
        প্রত্যাগম false;
    }

#ifdef CONFIG_USE_LOGICAL_IDS
    init_xapic_ldr();
#endif /* CONFIG_USE_LOGICAL_IDS */

    প্রত্যাগম true;
}

bool_t apic_is_interrupt_pending(শূন্য)
{
    word_t i;

    /* read 256-bit register: each 32-bit word is 16 byte aligned */
    প্রতিজ্ঞা(int_irq_min % 32 == 0);
    প্রতি (i = int_irq_min; i <= int_irq_max; i += 32) {
        যদি (apic_read_reg(APIC_IRR_BASE + i / 2) != 0) {
            প্রত্যাগম true;
        }
    }
    প্রত্যাগম false;
}

BOOT_CODE শূন্য apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).শব্দগুলি[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /* vector          */
        ).শব্দগুলি[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).শব্দগুলি[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /* vector          */
        ).শব্দগুলি[0]
    );
}

BOOT_CODE শূন্য apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    প্রতিজ্ঞা(IS_ALIGNED(startup_addr, PAGE_BITS));
    /* check if startup_addr < 640K */
    প্রতিজ্ঞা(startup_addr < 0xa0000);
    startup_addr >>= PAGE_BITS;

    apic_write_icr(
        apic_icr2_new(
            cpu_id       /* dest */
        ).শব্দগুলি[0],
        apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* delivery_status */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /* vector          */
        ).শব্দগুলি[0]
    );
}

শূন্য apic_send_ipi_core(irq_t vector, cpu_id_t cpu_id)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    কুরু {
        icr1.শব্দগুলি[0] = apic_read_reg(APIC_ICR1);
    } যাবত্ (apic_icr1_get_delivery_status(icr1));

    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).শব্দগুলি[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            vector      /* vector          */
        ).শব্দগুলি[0]
    );
}

শূন্য apic_send_ipi_cluster(irq_t vector, word_t mda)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    কুরু {
        icr1.শব্দগুলি[0] = apic_read_reg(APIC_ICR1);
    } যাবত্ (apic_icr1_get_delivery_status(icr1));

    apic_write_icr(
        apic_icr2_new(
            mda         /* message destination address */
        ).শব্দগুলি[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            vector      /* vector          */
        ).শব্দগুলি[0]
    );
}
#endif /* CONFIG_XAPIC */
