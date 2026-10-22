/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <arch/kernel/xapic.h>
#अन्तर्भाव <arch/kernel/x2apic.h>

#यदि_परिभाषितम् CONFIG_XAPIC
#यदि_परिभाषितम् CONFIG_USE_LOGICAL_IDS
/* using flat cluster mode we only support 8 cores */
compile_assert(सङ्ख्या_सम्बन्धि_cores_invalid_for_logical_ids, CONFIG_MAX_अङ्कः_NODES <= 8)

BOOT_संहिता स्थिर शून्य
init_xapic_ldr(शून्य)
{
    uint32_t ldr;

    apic_write_reg(APIC_DEST_FORMAT, XAPIC_DFR_FLAT);
    ldr = apic_read_reg(APIC_LOGICAL_DEST) & आवरणम्(XAPIC_LDR_सरणम्);
    ldr |= (BIT(getCurrentCPUIndex()) << XAPIC_LDR_सरणम्);
    apic_write_reg(APIC_LOGICAL_DEST, ldr);
}
#यदि_अन्तः /* CONFIG_USE_LOGICAL_IDS */

BOOT_संहिता bool_t apic_enable(शून्य)
{
    apic_आधारः_msr_t apic_आधारः_msr;
    apic_आधारः_msr.शब्दाः[0] = x86_rdmsr_नीचः(IA32_APIC_आधारः_MSR);

    यदि (!apic_आधारः_msr_get_enabled(apic_आधारः_msr)) {
        प्रारूप_लिख("APIC: Enabled bit not set\n");
        प्रत्यागम false;
    }

    यदि (x2apic_is_enabled()) {
        प्रारूप_लिख("x2APIC enabled in BIOS but kernel does not support that\n");
        प्रत्यागम false;
    }

#यदि_परिभाषितम् CONFIG_USE_LOGICAL_IDS
    init_xapic_ldr();
#यदि_अन्तः /* CONFIG_USE_LOGICAL_IDS */

    प्रत्यागम true;
}

bool_t apic_is_व्यवधानम्_pending(शून्य)
{
    word_t i;

    /* read 256-bit register: each 32-bit word is 16 byte aligned */
    प्रतिज्ञा(int_irq_min % 32 == 0);
    प्रति (i = int_irq_min; i <= int_irq_max; i += 32) {
        यदि (apic_read_reg(APIC_IRR_आधारः + i / 2) != 0) {
            प्रत्यागम true;
        }
    }
    प्रत्यागम false;
}

BOOT_संहिता शून्य apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).शब्दाः[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*सदिशम्*/
        ).शब्दाः[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).शब्दाः[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*सदिशम्*/
        ).शब्दाः[0]
    );
}

BOOT_संहिता शून्य apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    प्रतिज्ञा(IS_ALIGNED(startup_addr, PAGE_द्वयङ्काः));
    /* check if startup_addr < 640K */
    प्रतिज्ञा(startup_addr < 0xa0000);
    startup_addr >>= PAGE_द्वयङ्काः;

    apic_write_icr(
        apic_icr2_new(
            cpu_id       /* dest */
        ).शब्दाः[0],
        apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* delivery_status */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*सदिशम्*/
        ).शब्दाः[0]
    );
}

शून्य apic_send_ipi_core(irq_t सदिशम्, cpu_id_t cpu_id)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    कुरु {
        icr1.शब्दाः[0] = apic_read_reg(APIC_ICR1);
    } यावत् (apic_icr1_get_delivery_स्थितिः(icr1));

    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).शब्दाः[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            सदिशम्      /*सदिशम्*/
        ).शब्दाः[0]
    );
}

शून्य apic_send_ipi_cluster(irq_t सदिशम्, word_t mda)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    कुरु {
        icr1.शब्दाः[0] = apic_read_reg(APIC_ICR1);
    } यावत् (apic_icr1_get_delivery_स्थितिः(icr1));

    apic_write_icr(
        apic_icr2_new(
            mda         /* message destination address */
        ).शब्दाः[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            सदिशम्      /*सदिशम्*/
        ).शब्दाः[0]
    );
}
#यदि_अन्तः /* CONFIG_XAPIC */
