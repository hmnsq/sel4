/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/x2apic.h>

BOOT_ਕੋਡ bool_t x2apic_is_enabled(ਖਾਲੀ)
{
    apic_ਅਧਾਰ_msr_t apic_ਅਧਾਰ_msr;
    apic_ਅਧਾਰ_msr.ਸ਼ਬਦ[0] = x86_rdmsr_ਨੀਵਾਂ(IA32_APIC_ਅਧਾਰ_MSR);

    ਜੇ ((x86_cpuid_ecx(1, 0) & ਦੋਆਧਾਰੀ_ਅੰਕ(21)) &&
        apic_ਅਧਾਰ_msr_get_enabled(apic_ਅਧਾਰ_msr) &&
        apic_ਅਧਾਰ_msr_get_x2apic(apic_ਅਧਾਰ_msr)) {
        ਵਾਪਸ true;
    }
    ਵਾਪਸ false;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X2APIC
BOOT_ਕੋਡ bool_t apic_enable(ਖਾਲੀ)
{
    apic_ਅਧਾਰ_msr_t apic_ਅਧਾਰ_msr;
    apic_ਅਧਾਰ_msr.ਸ਼ਬਦ[0] = x86_rdmsr_ਨੀਵਾਂ(IA32_APIC_ਅਧਾਰ_MSR);

    ਜੇ (!apic_ਅਧਾਰ_msr_get_enabled(apic_ਅਧਾਰ_msr)) {
        ਲਿਖੋ("APIC: Enabled bit not set\n");
        ਵਾਪਸ false;
    }

    ਜੇ (x86_cpuid_ecx(1, 0) & ਦੋਆਧਾਰੀ_ਅੰਕ(21)) {
        apic_ਅਧਾਰ_msr = apic_ਅਧਾਰ_msr_ਸੈੱਟ_x2apic(apic_ਅਧਾਰ_msr, 1);
        x86_wrmsr(IA32_APIC_ਅਧਾਰ_MSR, apic_ਅਧਾਰ_msr.ਸ਼ਬਦ[0]);
    } ਨਹੀਂਤਾਂ {
        ਲਿਖੋ("APIC: x2APIC is not supported on this machine\n");
        ਵਾਪਸ false;
    }

    ਵਾਪਸ true;
}

bool_t apic_is_ਰੁਕਾਵਟ_pending(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t i;

    ਪ੍ਰਤਿਜ੍ਞਾ(int_irq_min % 32 == 0);
    ਲਈ (i = int_irq_min; i <= int_irq_max; i += 32) {
        ਜੇ (apic_read_reg(APIC_IRR_ਅਧਾਰ + ((i / 32) - 1)) != 0) {
            ਵਾਪਸ true;
        }
    }
    ਵਾਪਸ false;
}

BOOT_ਕੋਡ ਖਾਲੀ apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).ਸ਼ਬਦ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ਸ਼ਬਦ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );
}

BOOT_ਕੋਡ ਖਾਲੀ apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(startup_addr, ਪੰਨਾ_ਬਿੱਟ));
    /* check if startup_addr < 640K */
    ਪ੍ਰਤਿਜ੍ਞਾ(startup_addr < 0xa0000);
    startup_addr >>= ਪੰਨਾ_ਬਿੱਟ;

    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).ਸ਼ਬਦ[0],
        x2apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );
}

ਖਾਲੀ apic_send_ipi_core(irq_t ਸਦਿਸ਼, cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).ਸ਼ਬਦ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            ਸਦਿਸ਼      /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );
}

ਖਾਲੀ apic_send_ipi_cluster(irq_t ਸਦਿਸ਼, ਸ਼ਬਦ_t mda)
{
    apic_write_icr(
        x2apic_icr2_new(
            mda         /* message destination address */
        ).ਸ਼ਬਦ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            ਸਦਿਸ਼      /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );
}
#ਜੇ_ਅੰਤ /* CONFIG_X2APIC */
