/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <arch/kernel/xapic.h>
#અન્તર્ભાવ <arch/kernel/x2apic.h>

#ifdef CONFIG_XAPIC
#ifdef CONFIG_USE_LOGICAL_IDS
/* using flat cluster mode we only support 8 cores */
compile_assert(સંખ્યા_of_cores_invalid_for_logical_ids, CONFIG_MAX_અંક_NODES <= 8)

BOOT_CODE સ્થિર શૂન્ય
init_xapic_ldr(શૂન્ય)
{
    uint32_t ldr;

    apic_write_reg(APIC_DEST_FORMAT, XAPIC_DFR_FLAT);
    ldr = apic_read_reg(APIC_LOGICAL_DEST) & MASK(XAPIC_LDR_SHIFT);
    ldr |= (BIT(getCurrentCPUIndex()) << XAPIC_LDR_SHIFT);
    apic_write_reg(APIC_LOGICAL_DEST, ldr);
}
#endif /* CONFIG_USE_LOGICAL_IDS */

BOOT_CODE bool_t apic_enable(શૂન્ય)
{
    apic_base_msr_t apic_base_msr;
    apic_base_msr.શબ્દો[0] = x86_rdmsr_નીચલું(IA32_APIC_BASE_MSR);

    જો (!apic_base_msr_get_enabled(apic_base_msr)) {
        printf("APIC: Enabled bit not set\n");
        પાછા false;
    }

    જો (x2apic_is_enabled()) {
        printf("x2APIC enabled in BIOS but kernel does not support that\n");
        પાછા false;
    }

#ifdef CONFIG_USE_LOGICAL_IDS
    init_xapic_ldr();
#endif /* CONFIG_USE_LOGICAL_IDS */

    પાછા true;
}

bool_t apic_is_interrupt_pending(શૂન્ય)
{
    word_t i;

    /* read 256-bit register: each 32-bit word is 16 byte aligned */
    પ્રતિજ્ઞા(int_irq_min % 32 == 0);
    માટે (i = int_irq_min; i <= int_irq_max; i += 32) {
        જો (apic_read_reg(APIC_IRR_BASE + i / 2) != 0) {
            પાછા true;
        }
    }
    પાછા false;
}

BOOT_CODE શૂન્ય apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).શબ્દો[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /* vector          */
        ).શબ્દો[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).શબ્દો[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /* vector          */
        ).શબ્દો[0]
    );
}

BOOT_CODE શૂન્ય apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    પ્રતિજ્ઞા(IS_ALIGNED(startup_addr, PAGE_BITS));
    /* check if startup_addr < 640K */
    પ્રતિજ્ઞા(startup_addr < 0xa0000);
    startup_addr >>= PAGE_BITS;

    apic_write_icr(
        apic_icr2_new(
            cpu_id       /* dest */
        ).શબ્દો[0],
        apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* delivery_status */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /* vector          */
        ).શબ્દો[0]
    );
}

શૂન્ય apic_send_ipi_core(irq_t vector, cpu_id_t cpu_id)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    કરો {
        icr1.શબ્દો[0] = apic_read_reg(APIC_ICR1);
    } જ્યારે (apic_icr1_get_delivery_status(icr1));

    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).શબ્દો[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            vector      /* vector          */
        ).શબ્દો[0]
    );
}

શૂન્ય apic_send_ipi_cluster(irq_t vector, word_t mda)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    કરો {
        icr1.શબ્દો[0] = apic_read_reg(APIC_ICR1);
    } જ્યારે (apic_icr1_get_delivery_status(icr1));

    apic_write_icr(
        apic_icr2_new(
            mda         /* message destination address */
        ).શબ્દો[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            vector      /* vector          */
        ).શબ્દો[0]
    );
}
#endif /* CONFIG_XAPIC */
