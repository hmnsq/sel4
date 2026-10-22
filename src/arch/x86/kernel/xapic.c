/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <arch/kernel/xapic.h>
#అన్తర్భావ <arch/kernel/x2apic.h>

#నిర్వచితమైతే CONFIG_XAPIC
#నిర్వచితమైతే CONFIG_USE_LOGICAL_IDS
/* using flat cluster mode we only support 8 cores */
compile_assert(సంఖ్య_యొక్క_cores_invalid_for_logical_ids, CONFIG_MAX_అంకె_NODES <= 8)

BOOT_సంకేతం స్థిర శూన్య
init_xapic_ldr(శూన్య)
{
    uint32_t ldr;

    apic_write_reg(APIC_DEST_FORMAT, XAPIC_DFR_FLAT);
    ldr = apic_read_reg(APIC_LOGICAL_DEST) & ముసుగు(XAPIC_LDR_జరుపు);
    ldr |= (బిట్టు(getCurrentCPUIndex()) << XAPIC_LDR_జరుపు);
    apic_write_reg(APIC_LOGICAL_DEST, ldr);
}
#అయితే_ముగింపు /* CONFIG_USE_LOGICAL_IDS */

BOOT_సంకేతం bool_t apic_enable(శూన్య)
{
    apic_పునాది_msr_t apic_పునాది_msr;
    apic_పునాది_msr.పదాలు[0] = x86_rdmsr_దిగువ(IA32_APIC_పునాది_MSR);

    ఒకవేళ (!apic_పునాది_msr_get_enabled(apic_పునాది_msr)) {
        రాయి("APIC: Enabled bit not set\n");
        తిరిగి false;
    }

    ఒకవేళ (x2apic_is_enabled()) {
        రాయి("x2APIC enabled in BIOS but kernel does not support that\n");
        తిరిగి false;
    }

#నిర్వచితమైతే CONFIG_USE_LOGICAL_IDS
    init_xapic_ldr();
#అయితే_ముగింపు /* CONFIG_USE_LOGICAL_IDS */

    తిరిగి true;
}

bool_t apic_is_అంతరాయం_pending(శూన్య)
{
    మాట_t i;

    /* read 256-bit register: each 32-bit word is 16 byte aligned */
    ప్రతిజ్ఞా(int_irq_min % 32 == 0);
    కొరకు (i = int_irq_min; i <= int_irq_max; i += 32) {
        ఒకవేళ (apic_read_reg(APIC_IRR_పునాది + i / 2) != 0) {
            తిరిగి true;
        }
    }
    తిరిగి false;
}

BOOT_సంకేతం శూన్య apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).పదాలు[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*సదిశ*/
        ).పదాలు[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).పదాలు[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
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
        apic_icr2_new(
            cpu_id       /* dest */
        ).పదాలు[0],
        apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* delivery_status */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*సదిశ*/
        ).పదాలు[0]
    );
}

శూన్య apic_send_ipi_core(irq_t సదిశ, cpu_id_t cpu_id)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    చేయి {
        icr1.పదాలు[0] = apic_read_reg(APIC_ICR1);
    } అయితే (apic_icr1_get_delivery_స్థితి(icr1));

    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).పదాలు[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            సదిశ      /*సదిశ*/
        ).పదాలు[0]
    );
}

శూన్య apic_send_ipi_cluster(irq_t సదిశ, మాట_t mda)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    చేయి {
        icr1.పదాలు[0] = apic_read_reg(APIC_ICR1);
    } అయితే (apic_icr1_get_delivery_స్థితి(icr1));

    apic_write_icr(
        apic_icr2_new(
            mda         /* message destination address */
        ).పదాలు[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            సదిశ      /*సదిశ*/
        ).పదాలు[0]
    );
}
#అయితే_ముగింపు /* CONFIG_XAPIC */
