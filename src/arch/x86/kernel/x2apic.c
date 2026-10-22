/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/kernel/x2apic.h>

BOOT_کوڈ bool_t x2apic_is_enabled(فراغ)
{
    apic_ادھار_msr_t apic_ادھار_msr;
    apic_ادھار_msr.الفاظ[0] = x86_rdmsr_کم(IA32_APIC_ادھار_MSR);

    إذا ((x86_cpuid_ecx(1, 0) & ثنائی_ہندسہ(21)) &&
        apic_ادھار_msr_get_enabled(apic_ادھار_msr) &&
        apic_ادھار_msr_get_x2apic(apic_ادھار_msr)) {
        أرجع true;
    }
    أرجع false;
}

#جے_تعریف_شدہ CONFIG_X2APIC
BOOT_کوڈ bool_t apic_enable(فراغ)
{
    apic_ادھار_msr_t apic_ادھار_msr;
    apic_ادھار_msr.الفاظ[0] = x86_rdmsr_کم(IA32_APIC_ادھار_MSR);

    إذا (!apic_ادھار_msr_get_enabled(apic_ادھار_msr)) {
        لکھو("APIC: Enabled bit not set\n");
        أرجع false;
    }

    إذا (x86_cpuid_ecx(1, 0) & ثنائی_ہندسہ(21)) {
        apic_ادھار_msr = apic_ادھار_msr_سیٹ_x2apic(apic_ادھار_msr, 1);
        x86_wrmsr(IA32_APIC_ادھار_MSR, apic_ادھار_msr.الفاظ[0]);
    } وإلا {
        لکھو("APIC: x2APIC is not supported on this machine\n");
        أرجع false;
    }

    أرجع true;
}

bool_t apic_is_رکاوٹ_pending(فراغ)
{
    لفظ_t i;

    أكد(int_irq_min % 32 == 0);
    لكل (i = int_irq_min; i <= int_irq_max; i += 32) {
        إذا (apic_read_reg(APIC_IRR_ادھار + ((i / 32) - 1)) != 0) {
            أرجع true;
        }
    }
    أرجع false;
}

BOOT_کوڈ فراغ apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).الفاظ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*سمتیہ*/
        ).الفاظ[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).الفاظ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*سمتیہ*/
        ).الفاظ[0]
    );
}

BOOT_کوڈ فراغ apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    أكد(IS_ALIGNED(startup_addr, ورقہ_بٹ));
    /* check if startup_addr < 640K */
    أكد(startup_addr < 0xa0000);
    startup_addr >>= ورقہ_بٹ;

    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).الفاظ[0],
        x2apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*سمتیہ*/
        ).الفاظ[0]
    );
}

فراغ apic_send_ipi_core(irq_t سمتیہ, cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).الفاظ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            سمتیہ      /*سمتیہ*/
        ).الفاظ[0]
    );
}

فراغ apic_send_ipi_cluster(irq_t سمتیہ, لفظ_t mda)
{
    apic_write_icr(
        x2apic_icr2_new(
            mda         /* message destination address */
        ).الفاظ[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            سمتیہ      /*سمتیہ*/
        ).الفاظ[0]
    );
}
#جے_انت /* CONFIG_X2APIC */
