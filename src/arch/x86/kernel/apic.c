/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/kernel/apic.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/devices.h>
#अन्तर्भाव <plat/machine/pit.h>

#परिभाषय CPUID_TSC_DEADLINE_BIT 24u
#परिभाषय APIC_TIMER_बहुलकम्_ONE_SHOT 0
#परिभाषय APIC_TIMER_बहुलकम्_TSC_DEADLINE  2

स्थिर BOOT_संहिता uint32_t apic_measure_freq(शून्य)
{
    pit_init();
    /* wait for 1st PIT wraparound */
    pit_wait_wraparound();

    /* start APIC timer countdown */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_गणनम्, 0xffffffff);

    /* wait for 2nd PIT wraparound */
    pit_wait_wraparound();

    /* calculate APIC/bus cycles per ms = frequency in kHz */
    प्रत्यागम (0xffffffff - apic_read_reg(APIC_TIMER_CURRENT)) / PIT_WRAPAROUND_MS;
}

BOOT_संहिता paddr_t apic_get_आधारः_paddr(शून्य)
{
    apic_आधारः_msr_t apic_आधारः_msr;

    apic_आधारः_msr.शब्दाः[0] = x86_rdmsr_नीचः(IA32_APIC_आधारः_MSR);
    प्रत्यागम apic_आधारः_msr_get_आधारः_addr(apic_आधारः_msr);
}

BOOT_संहिता bool_t apic_init(bool_t आवरणम्_legacy_irqs)
{
    apic_संस्करणम्_t apic_संस्करणम्;
    uint32_t अङ्कः_lvt_entries;
    uint32_t apic_khz;

    यदि (!apic_enable()) {
        प्रत्यागम false;
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* find tsc KHz */
    x86KStscMhz = tsc_init();

    /* can we use tsc deadline mode? */
    uint32_t cpuid = x86_cpuid_ecx(0x1, 0x0);
    यदि (!(cpuid & BIT(CPUID_TSC_DEADLINE_BIT))) {
        apic_khz = apic_measure_freq();
        x86_KSapic_अनुपातः = div64((uint64_t)x86KStscMhz * 1000llu, apic_khz);
        प्रारूप_लिख("Apic Khz %lu, TSC Mhz %lu, ratio %lu\n", (दीर्घ) apic_khz, (दीर्घ) x86KStscMhz, (दीर्घ) x86_KSapic_अनुपातः);
    } अन्यथा {
        // use tsc deadline mode
        x86_KSapic_अनुपातः = 0;
    }
#अन्यथा
    apic_khz = apic_measure_freq();
#यदि_अन्तः
    apic_संस्करणम्.शब्दाः[0] = apic_read_reg(APIC_संस्करणम्);

    /* check for correct version (both APIC and x2APIC): 0x1X */
    यदि (apic_संस्करणम्_get_संस्करणम्(apic_संस्करणम्) >> 4 != 1) {
        प्रारूप_लिख("APIC: apic_version must be 0x1X\n");
        प्रत्यागम false;
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (x86_KSapic_अनुपातः != 0) {
        /* initialise APIC timer */
        apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    }
#यदि_अन्तः

    /* check for correct number of LVT entries */
    अङ्कः_lvt_entries = apic_संस्करणम्_get_max_lvt_प्रविष्टिः(apic_संस्करणम्) + 1;
    यदि (अङ्कः_lvt_entries < 3) {
        प्रारूप_लिख("APIC: number of LVT entries: %d\n", अङ्कः_lvt_entries);
        प्रारूप_लिख("APIC: number of LVT entries must be >= 3\n");
        प्रत्यागम false;
    }

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
    /* initialise APIC timer */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_गणनम्, apic_khz * CONFIG_TIMER_TICK_MS);
#यदि_अन्तः

    /* enable APIC using SVR register */
    apic_write_reg(
        APIC_SVR,
        apic_svr_new(
            0,           /* focus_processor_chk */
            1,           /* enabled             */
            int_spurious /* spurious_vector     */
        ).शब्दाः[0]
    );

    /* mask/unmask LINT0 (used for legacy IRQ delivery) */
    apic_write_reg(
        APIC_LVT_LINT0,
        apic_lvt_new(
            0,                /* timer_mode      */
            आवरणम्_legacy_irqs, /* masked          */
            0,                /* trigger_mode    */
            0,                /* remote_irr      */
            0,                /* pin_polarity    */
            0,                /* delivery_status */
            7,                /* delivery_mode   */
            0                 /*सदिशम्*/
        ).शब्दाः[0]
    );

    /* mask LINT1 (used for NMI delivery) */
    apic_write_reg(
        APIC_LVT_LINT1,
        apic_lvt_new(
            0,  /* timer_mode      */
            1,  /* masked          */
            0,  /* trigger_mode    */
            0,  /* remote_irr      */
            0,  /* pin_polarity    */
            0,  /* delivery_status */
            0,  /* delivery_mode   */
            0   /*सदिशम्*/
        ).शब्दाः[0]
    );

    /* initialise timer */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    uint32_t timer_बहुलकम् = x86_KSapic_अनुपातः == 0 ? APIC_TIMER_बहुलकम्_TSC_DEADLINE :
                          APIC_TIMER_बहुलकम्_ONE_SHOT;
#अन्यथा
    uint32_t timer_बहुलकम् = 1;
#यदि_अन्तः
    apic_write_reg(
        APIC_LVT_TIMER,
        apic_lvt_new(
            timer_बहुलकम्,
            0,        /* masked          */
            0,        /* trigger_mode    */
            0,        /* remote_irr      */
            0,        /* pin_polarity    */
            0,        /* delivery_status */
            0,        /* delivery_mode   */
            int_timer /*सदिशम्*/
        ).शब्दाः[0]
    );

    /*
    printf("APIC: ID=0x%x\n", apic_read_reg(APIC_ID) >> 24);
    printf("APIC: SVR=0x%x\n", apic_read_reg(APIC_SVR));
    printf("APIC: LVT_TIMER=0x%x\n", apic_read_reg(APIC_LVT_TIMER));
    printf("APIC: LVT_LINT0=0x%x\n", apic_read_reg(APIC_LVT_LINT0));
    printf("APIC: LVT_LINT1=0x%x\n", apic_read_reg(APIC_LVT_LINT1));
    printf("APIC: LVT_ERROR=0x%x\n", apic_read_reg(APIC_LVT_ERROR));
    printf("APIC: LVT_PERF_CNTR=0x%x\n", apic_read_reg(APIC_LVT_PERF_CNTR));
    printf("APIC: LVT_THERMAL=0x%x\n", apic_read_reg(APIC_LVT_THERMAL));
    */
    प्रत्यागम true;
}

शून्य apic_ack_सक्रिय_व्यवधानम्(शून्य)
{
    apic_write_reg(APIC_EOI, 0);
}
