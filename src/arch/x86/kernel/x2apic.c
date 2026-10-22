/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <arch/kernel/x2apic.h>

BOOT_رمز bool_t x2apic_is_enabled(خالی)
{
    apic_بنیاد_msr_t apic_بنیاد_msr;
    apic_بنیاد_msr.الفاظ[0] = x86_rdmsr_کم(IA32_APIC_بنیاد_MSR);

    اگر ((x86_cpuid_ecx(1, 0) & ثنائی_ہندسہ(21)) &&
        apic_بنیاد_msr_get_enabled(apic_بنیاد_msr) &&
        apic_بنیاد_msr_get_x2apic(apic_بنیاد_msr)) {
        واپس true;
    }
    واپس false;
}

#اگر_تعریف_شدہ CONFIG_X2APIC
BOOT_رمز bool_t apic_enable(خالی)
{
    apic_بنیاد_msr_t apic_بنیاد_msr;
    apic_بنیاد_msr.الفاظ[0] = x86_rdmsr_کم(IA32_APIC_بنیاد_MSR);

    اگر (!apic_بنیاد_msr_get_enabled(apic_بنیاد_msr)) {
        لکھو("APIC: Enabled bit not set\n");
        واپس false;
    }

    اگر (x86_cpuid_ecx(1, 0) & ثنائی_ہندسہ(21)) {
        apic_بنیاد_msr = apic_بنیاد_msr_سیٹ_x2apic(apic_بنیاد_msr, 1);
        x86_wrmsr(IA32_APIC_بنیاد_MSR, apic_بنیاد_msr.الفاظ[0]);
    } ورنہ {
        لکھو("APIC: x2APIC is not supported on this machine\n");
        واپس false;
    }

    واپس true;
}

bool_t apic_is_مداخلت_pending(خالی)
{
    لفظ_t i;

    أكد(int_irq_min % 32 == 0);
    برائے (i = int_irq_min; i <= int_irq_max; i += 32) {
        اگر (apic_read_reg(APIC_IRR_بنیاد + ((i / 32) - 1)) != 0) {
            واپس true;
        }
    }
    واپس false;
}

BOOT_رمز خالی apic_send_init_ipi(cpu_id_t cpu_id)
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

BOOT_رمز خالی apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    أكد(IS_ALIGNED(startup_addr, صفحہ_بٹ));
    /* check if startup_addr < 640K */
    أكد(startup_addr < 0xa0000);
    startup_addr >>= صفحہ_بٹ;

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

خالی apic_send_ipi_core(irq_t سمتیہ, cpu_id_t cpu_id)
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

خالی apic_send_ipi_cluster(irq_t سمتیہ, لفظ_t mda)
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
#اگر_ختم /* CONFIG_X2APIC */
