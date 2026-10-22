/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <arch/kernel/xapic.h>
#अन्तर्भाव <arch/kernel/x2apic.h>

#ifdef CONFIG_XAPIC
#ifdef CONFIG_USE_LOGICAL_IDS
/* using flat cluster mode we only support 8 cores */
compile_assert(संख्या_of_cores_invalid_for_logical_ids, CONFIG_MAX_अंक_NODES <= 8)

BOOT_CODE स्थिर शून्य
init_xapic_ldr(शून्य)
{
    uint32_t ldr;

    apic_write_reg(APIC_DEST_FORMAT, XAPIC_DFR_FLAT);
    ldr = apic_read_reg(APIC_LOGICAL_DEST) & MASK(XAPIC_LDR_SHIFT);
    ldr |= (BIT(getCurrentCPUIndex()) << XAPIC_LDR_SHIFT);
    apic_write_reg(APIC_LOGICAL_DEST, ldr);
}
#endif /* CONFIG_USE_LOGICAL_IDS */

BOOT_CODE bool_t apic_enable(शून्य)
{
    apic_base_msr_t apic_base_msr;
    apic_base_msr.शब्दगण[0] = x86_rdmsr_निचला(IA32_APIC_BASE_MSR);

    यदि (!apic_base_msr_get_enabled(apic_base_msr)) {
        printf("APIC: Enabled bit not set\n");
        प्रत्यागम false;
    }

    यदि (x2apic_is_enabled()) {
        printf("x2APIC enabled in BIOS but kernel does not support that\n");
        प्रत्यागम false;
    }

#ifdef CONFIG_USE_LOGICAL_IDS
    init_xapic_ldr();
#endif /* CONFIG_USE_LOGICAL_IDS */

    प्रत्यागम true;
}

bool_t apic_is_interrupt_pending(शून्य)
{
    word_t i;

    /* read 256-bit register: each 32-bit word is 16 byte aligned */
    प्रतिज्ञा(int_irq_min % 32 == 0);
    प्रति (i = int_irq_min; i <= int_irq_max; i += 32) {
        यदि (apic_read_reg(APIC_IRR_BASE + i / 2) != 0) {
            प्रत्यागम true;
        }
    }
    प्रत्यागम false;
}

BOOT_CODE शून्य apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).शब्दगण[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /* vector          */
        ).शब्दगण[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).शब्दगण[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /* vector          */
        ).शब्दगण[0]
    );
}

BOOT_CODE शून्य apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    प्रतिज्ञा(IS_ALIGNED(startup_addr, PAGE_BITS));
    /* check if startup_addr < 640K */
    प्रतिज्ञा(startup_addr < 0xa0000);
    startup_addr >>= PAGE_BITS;

    apic_write_icr(
        apic_icr2_new(
            cpu_id       /* dest */
        ).शब्दगण[0],
        apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* delivery_status */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /* vector          */
        ).शब्दगण[0]
    );
}

शून्य apic_send_ipi_core(irq_t vector, cpu_id_t cpu_id)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    कुरु {
        icr1.शब्दगण[0] = apic_read_reg(APIC_ICR1);
    } यावत् (apic_icr1_get_delivery_status(icr1));

    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).शब्दगण[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            vector      /* vector          */
        ).शब्दगण[0]
    );
}

शून्य apic_send_ipi_cluster(irq_t vector, word_t mda)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    कुरु {
        icr1.शब्दगण[0] = apic_read_reg(APIC_ICR1);
    } यावत् (apic_icr1_get_delivery_status(icr1));

    apic_write_icr(
        apic_icr2_new(
            mda         /* message destination address */
        ).शब्दगण[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            vector      /* vector          */
        ).शब्दगण[0]
    );
}
#endif /* CONFIG_XAPIC */
