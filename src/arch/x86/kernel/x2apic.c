/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <arch/kernel/x2apic.h>

BOOT_CODE bool_t x2apic_is_enabled(ശൂന്യം)
{
    apic_base_msr_t apic_base_msr;
    apic_base_msr.വാക്കുകൾ[0] = x86_rdmsr_താഴെയുള്ള(IA32_APIC_BASE_MSR);

    എങ്കിൽ ((x86_cpuid_ecx(1, 0) & BIT(21)) &&
        apic_base_msr_get_enabled(apic_base_msr) &&
        apic_base_msr_get_x2apic(apic_base_msr)) {
        തിരികെ true;
    }
    തിരികെ false;
}

#ifdef CONFIG_X2APIC
BOOT_CODE bool_t apic_enable(ശൂന്യം)
{
    apic_base_msr_t apic_base_msr;
    apic_base_msr.വാക്കുകൾ[0] = x86_rdmsr_താഴെയുള്ള(IA32_APIC_BASE_MSR);

    എങ്കിൽ (!apic_base_msr_get_enabled(apic_base_msr)) {
        printf("APIC: Enabled bit not set\n");
        തിരികെ false;
    }

    എങ്കിൽ (x86_cpuid_ecx(1, 0) & BIT(21)) {
        apic_base_msr = apic_base_msr_set_x2apic(apic_base_msr, 1);
        x86_wrmsr(IA32_APIC_BASE_MSR, apic_base_msr.വാക്കുകൾ[0]);
    } അല്ലെങ്കിൽ {
        printf("APIC: x2APIC is not supported on this machine\n");
        തിരികെ false;
    }

    തിരികെ true;
}

bool_t apic_is_interrupt_pending(ശൂന്യം)
{
    word_t i;

    പ്രതിജ്ഞാ(int_irq_min % 32 == 0);
    വേണ്ടി (i = int_irq_min; i <= int_irq_max; i += 32) {
        എങ്കിൽ (apic_read_reg(APIC_IRR_BASE + ((i / 32) - 1)) != 0) {
            തിരികെ true;
        }
    }
    തിരികെ false;
}

BOOT_CODE ശൂന്യം apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).വാക്കുകൾ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /* vector          */
        ).വാക്കുകൾ[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).വാക്കുകൾ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /* vector          */
        ).വാക്കുകൾ[0]
    );
}

BOOT_CODE ശൂന്യം apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    പ്രതിജ്ഞാ(IS_ALIGNED(startup_addr, PAGE_BITS));
    /* check if startup_addr < 640K */
    പ്രതിജ്ഞാ(startup_addr < 0xa0000);
    startup_addr >>= PAGE_BITS;

    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).വാക്കുകൾ[0],
        x2apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /* vector          */
        ).വാക്കുകൾ[0]
    );
}

ശൂന്യം apic_send_ipi_core(irq_t vector, cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).വാക്കുകൾ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            vector      /* vector          */
        ).വാക്കുകൾ[0]
    );
}

ശൂന്യം apic_send_ipi_cluster(irq_t vector, word_t mda)
{
    apic_write_icr(
        x2apic_icr2_new(
            mda         /* message destination address */
        ).വാക്കുകൾ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            vector      /* vector          */
        ).വാക്കുകൾ[0]
    );
}
#endif /* CONFIG_X2APIC */
