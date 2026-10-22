/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <arch/kernel/x2apic.h>

BOOT_CODE bool_t x2apic_is_enabled(வெறுமை)
{
    apic_base_msr_t apic_base_msr;
    apic_base_msr.சொற்கள்[0] = x86_rdmsr_கீழ்(IA32_APIC_BASE_MSR);

    எனில் ((x86_cpuid_ecx(1, 0) & BIT(21)) &&
        apic_base_msr_get_enabled(apic_base_msr) &&
        apic_base_msr_get_x2apic(apic_base_msr)) {
        திரும்பு true;
    }
    திரும்பு false;
}

#ifdef CONFIG_X2APIC
BOOT_CODE bool_t apic_enable(வெறுமை)
{
    apic_base_msr_t apic_base_msr;
    apic_base_msr.சொற்கள்[0] = x86_rdmsr_கீழ்(IA32_APIC_BASE_MSR);

    எனில் (!apic_base_msr_get_enabled(apic_base_msr)) {
        printf("APIC: Enabled bit not set\n");
        திரும்பு false;
    }

    எனில் (x86_cpuid_ecx(1, 0) & BIT(21)) {
        apic_base_msr = apic_base_msr_set_x2apic(apic_base_msr, 1);
        x86_wrmsr(IA32_APIC_BASE_MSR, apic_base_msr.சொற்கள்[0]);
    } இல்லையெனில் {
        printf("APIC: x2APIC is not supported on this machine\n");
        திரும்பு false;
    }

    திரும்பு true;
}

bool_t apic_is_interrupt_pending(வெறுமை)
{
    word_t i;

    ப்ரதிஜ்ஞா(int_irq_min % 32 == 0);
    க்கு (i = int_irq_min; i <= int_irq_max; i += 32) {
        எனில் (apic_read_reg(APIC_IRR_BASE + ((i / 32) - 1)) != 0) {
            திரும்பு true;
        }
    }
    திரும்பு false;
}

BOOT_CODE வெறுமை apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).சொற்கள்[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /* vector          */
        ).சொற்கள்[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).சொற்கள்[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /* vector          */
        ).சொற்கள்[0]
    );
}

BOOT_CODE வெறுமை apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    ப்ரதிஜ்ஞா(IS_ALIGNED(startup_addr, PAGE_BITS));
    /* check if startup_addr < 640K */
    ப்ரதிஜ்ஞா(startup_addr < 0xa0000);
    startup_addr >>= PAGE_BITS;

    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).சொற்கள்[0],
        x2apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /* vector          */
        ).சொற்கள்[0]
    );
}

வெறுமை apic_send_ipi_core(irq_t vector, cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).சொற்கள்[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            vector      /* vector          */
        ).சொற்கள்[0]
    );
}

வெறுமை apic_send_ipi_cluster(irq_t vector, word_t mda)
{
    apic_write_icr(
        x2apic_icr2_new(
            mda         /* message destination address */
        ).சொற்கள்[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            vector      /* vector          */
        ).சொற்கள்[0]
    );
}
#endif /* CONFIG_X2APIC */
