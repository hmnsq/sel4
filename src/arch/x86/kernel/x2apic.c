/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/kernel/x2apic.h>

BOOT_کد bool_t x2apic_is_enabled(فراغ)
{
    apic_پایه_msr_t apic_پایه_msr;
    apic_پایه_msr.واژگان[0] = x86_rdmsr_کم(IA32_APIC_پایه_MSR);

    إذا ((x86_cpuid_ecx(1, 0) & بیت(21)) &&
        apic_پایه_msr_get_enabled(apic_پایه_msr) &&
        apic_پایه_msr_get_x2apic(apic_پایه_msr)) {
        أرجع true;
    }
    أرجع false;
}

#اگر_تعریف_شده CONFIG_X2APIC
BOOT_کد bool_t apic_enable(فراغ)
{
    apic_پایه_msr_t apic_پایه_msr;
    apic_پایه_msr.واژگان[0] = x86_rdmsr_کم(IA32_APIC_پایه_MSR);

    إذا (!apic_پایه_msr_get_enabled(apic_پایه_msr)) {
        چاپ_کن("APIC: Enabled bit not set\n");
        أرجع false;
    }

    إذا (x86_cpuid_ecx(1, 0) & بیت(21)) {
        apic_پایه_msr = apic_پایه_msr_مجموعه_x2apic(apic_پایه_msr, 1);
        x86_wrmsr(IA32_APIC_پایه_MSR, apic_پایه_msr.واژگان[0]);
    } وإلا {
        چاپ_کن("APIC: x2APIC is not supported on this machine\n");
        أرجع false;
    }

    أرجع true;
}

bool_t apic_is_وقفه_pending(فراغ)
{
    کلمه_t i;

    أكد(int_irq_min % 32 == 0);
    لكل (i = int_irq_min; i <= int_irq_max; i += 32) {
        إذا (apic_read_reg(APIC_IRR_پایه + ((i / 32) - 1)) != 0) {
            أرجع true;
        }
    }
    أرجع false;
}

BOOT_کد فراغ apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).واژگان[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*وکتور*/
        ).واژگان[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).واژگان[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*وکتور*/
        ).واژگان[0]
    );
}

BOOT_کد فراغ apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    أكد(IS_ALIGNED(startup_addr, صفحه_بیتها));
    /* check if startup_addr < 640K */
    أكد(startup_addr < 0xa0000);
    startup_addr >>= صفحه_بیتها;

    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).واژگان[0],
        x2apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*وکتور*/
        ).واژگان[0]
    );
}

فراغ apic_send_ipi_core(irq_t وکتور, cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).واژگان[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            وکتور      /*وکتور*/
        ).واژگان[0]
    );
}

فراغ apic_send_ipi_cluster(irq_t وکتور, کلمه_t mda)
{
    apic_write_icr(
        x2apic_icr2_new(
            mda         /* message destination address */
        ).واژگان[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            وکتور      /*وکتور*/
        ).واژگان[0]
    );
}
#پایان_اگر /* CONFIG_X2APIC */
