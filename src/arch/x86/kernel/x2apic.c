/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <arch/kernel/x2apic.h>

BOOT_సంకేతం bool_t x2apic_is_enabled(శూన్య)
{
    apic_పునాది_msr_t apic_పునాది_msr;
    apic_పునాది_msr.పదాలు[0] = x86_rdmsr_దిగువ(IA32_APIC_పునాది_MSR);

    ఒకవేళ ((x86_cpuid_ecx(1, 0) & బిట్టు(21)) &&
        apic_పునాది_msr_get_enabled(apic_పునాది_msr) &&
        apic_పునాది_msr_get_x2apic(apic_పునాది_msr)) {
        తిరిగి true;
    }
    తిరిగి false;
}

#నిర్వచితమైతే CONFIG_X2APIC
BOOT_సంకేతం bool_t apic_enable(శూన్య)
{
    apic_పునాది_msr_t apic_పునాది_msr;
    apic_పునాది_msr.పదాలు[0] = x86_rdmsr_దిగువ(IA32_APIC_పునాది_MSR);

    ఒకవేళ (!apic_పునాది_msr_get_enabled(apic_పునాది_msr)) {
        రాయి("APIC: Enabled bit not set\n");
        తిరిగి false;
    }

    ఒకవేళ (x86_cpuid_ecx(1, 0) & బిట్టు(21)) {
        apic_పునాది_msr = apic_పునాది_msr_సమితి_x2apic(apic_పునాది_msr, 1);
        x86_wrmsr(IA32_APIC_పునాది_MSR, apic_పునాది_msr.పదాలు[0]);
    } లేకపోతే {
        రాయి("APIC: x2APIC is not supported on this machine\n");
        తిరిగి false;
    }

    తిరిగి true;
}

bool_t apic_is_అంతరాయం_pending(శూన్య)
{
    మాట_t i;

    ప్రతిజ్ఞా(int_irq_min % 32 == 0);
    కొరకు (i = int_irq_min; i <= int_irq_max; i += 32) {
        ఒకవేళ (apic_read_reg(APIC_IRR_పునాది + ((i / 32) - 1)) != 0) {
            తిరిగి true;
        }
    }
    తిరిగి false;
}

BOOT_సంకేతం శూన్య apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).పదాలు[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*సదిశ*/
        ).పదాలు[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).పదాలు[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*సదిశ*/
        ).పదాలు[0]
    );
}

BOOT_సంకేతం శూన్య apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    ప్రతిజ్ఞా(IS_ALIGNED(startup_addr, పుట_బిట్లు));
    /* check if startup_addr < 640K */
    ప్రతిజ్ఞా(startup_addr < 0xa0000);
    startup_addr >>= పుట_బిట్లు;

    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).పదాలు[0],
        x2apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*సదిశ*/
        ).పదాలు[0]
    );
}

శూన్య apic_send_ipi_core(irq_t సదిశ, cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).పదాలు[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            సదిశ      /*సదిశ*/
        ).పదాలు[0]
    );
}

శూన్య apic_send_ipi_cluster(irq_t సదిశ, మాట_t mda)
{
    apic_write_icr(
        x2apic_icr2_new(
            mda         /* message destination address */
        ).పదాలు[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            సదిశ      /*సదిశ*/
        ).పదాలు[0]
    );
}
#అయితే_ముగింపు /* CONFIG_X2APIC */
