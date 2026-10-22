/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/kernel/x2apic.h>

BOOT_ڪوڊ bool_t x2apic_is_enabled(فراغ)
{
    apic_بنياد_msr_t apic_بنياد_msr;
    apic_بنياد_msr.لفظ[0] = x86_rdmsr_هيٺيون(IA32_APIC_بنياد_MSR);

    إذا ((x86_cpuid_ecx(1, 0) & ٻٽو_انگ(21)) &&
        apic_بنياد_msr_get_enabled(apic_بنياد_msr) &&
        apic_بنياد_msr_get_x2apic(apic_بنياد_msr)) {
        أرجع true;
    }
    أرجع false;
}

#جيڪڏهن_وصف_ٿيل CONFIG_X2APIC
BOOT_ڪوڊ bool_t apic_enable(فراغ)
{
    apic_بنياد_msr_t apic_بنياد_msr;
    apic_بنياد_msr.لفظ[0] = x86_rdmsr_هيٺيون(IA32_APIC_بنياد_MSR);

    إذا (!apic_بنياد_msr_get_enabled(apic_بنياد_msr)) {
        لکو("APIC: Enabled bit not set\n");
        أرجع false;
    }

    إذا (x86_cpuid_ecx(1, 0) & ٻٽو_انگ(21)) {
        apic_بنياد_msr = apic_بنياد_msr_سيٽ_x2apic(apic_بنياد_msr, 1);
        x86_wrmsr(IA32_APIC_بنياد_MSR, apic_بنياد_msr.لفظ[0]);
    } وإلا {
        لکو("APIC: x2APIC is not supported on this machine\n");
        أرجع false;
    }

    أرجع true;
}

bool_t apic_is_مداخلت_pending(فراغ)
{
    لفظ_t i;

    أكد(int_irq_min % 32 == 0);
    لكل (i = int_irq_min; i <= int_irq_max; i += 32) {
        إذا (apic_read_reg(APIC_IRR_بنياد + ((i / 32) - 1)) != 0) {
            أرجع true;
        }
    }
    أرجع false;
}

BOOT_ڪوڊ فراغ apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).لفظ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ويڪٽر*/
        ).لفظ[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).لفظ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ويڪٽر*/
        ).لفظ[0]
    );
}

BOOT_ڪوڊ فراغ apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    أكد(IS_ALIGNED(startup_addr, صفحو_بٽ));
    /* check if startup_addr < 640K */
    أكد(startup_addr < 0xa0000);
    startup_addr >>= صفحو_بٽ;

    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).لفظ[0],
        x2apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*ويڪٽر*/
        ).لفظ[0]
    );
}

فراغ apic_send_ipi_core(irq_t ويڪٽر, cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).لفظ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            ويڪٽر      /*ويڪٽر*/
        ).لفظ[0]
    );
}

فراغ apic_send_ipi_cluster(irq_t ويڪٽر, لفظ_t mda)
{
    apic_write_icr(
        x2apic_icr2_new(
            mda         /* message destination address */
        ).لفظ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            ويڪٽر      /*ويڪٽر*/
        ).لفظ[0]
    );
}
#جيڪڏهن_پورو /* CONFIG_X2APIC */
