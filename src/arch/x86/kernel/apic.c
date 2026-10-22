/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/kernel/apic.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/devices.h>
#అన్తర్భావ <plat/machine/pit.h>

#నిర్వచించు CPUID_TSC_DEADLINE_బిట్టు 24u
#నిర్వచించు APIC_TIMER_బహుళకం_ONE_SHOT 0
#నిర్వచించు APIC_TIMER_బహుళకం_TSC_DEADLINE  2

స్థిర BOOT_సంకేతం uint32_t apic_measure_freq(శూన్య)
{
    pit_init();
    /* wait for 1st PIT wraparound */
    pit_wait_wraparound();

    /* start APIC timer countdown */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_లెక్క, 0xffffffff);

    /* wait for 2nd PIT wraparound */
    pit_wait_wraparound();

    /* calculate APIC/bus cycles per ms = frequency in kHz */
    తిరిగి (0xffffffff - apic_read_reg(APIC_TIMER_CURRENT)) / PIT_WRAPAROUND_MS;
}

BOOT_సంకేతం paddr_t apic_get_పునాది_paddr(శూన్య)
{
    apic_పునాది_msr_t apic_పునాది_msr;

    apic_పునాది_msr.పదాలు[0] = x86_rdmsr_దిగువ(IA32_APIC_పునాది_MSR);
    తిరిగి apic_పునాది_msr_get_పునాది_addr(apic_పునాది_msr);
}

BOOT_సంకేతం bool_t apic_init(bool_t ముసుగు_legacy_irqs)
{
    apic_సంస్కరణ_t apic_సంస్కరణ;
    uint32_t అంకె_lvt_entries;
    uint32_t apic_khz;

    ఒకవేళ (!apic_enable()) {
        తిరిగి false;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* find tsc KHz */
    x86KStscMhz = tsc_init();

    /* can we use tsc deadline mode? */
    uint32_t cpuid = x86_cpuid_ecx(0x1, 0x0);
    ఒకవేళ (!(cpuid & బిట్టు(CPUID_TSC_DEADLINE_బిట్టు))) {
        apic_khz = apic_measure_freq();
        x86_KSapic_నిష్పత్తి = div64((uint64_t)x86KStscMhz * 1000llu, apic_khz);
        రాయి("Apic Khz %lu, TSC Mhz %lu, ratio %lu\n", (దీర్ఘ) apic_khz, (దీర్ఘ) x86KStscMhz, (దీర్ఘ) x86_KSapic_నిష్పత్తి);
    } లేకపోతే {
        // use tsc deadline mode
        x86_KSapic_నిష్పత్తి = 0;
    }
#లేకపోతే
    apic_khz = apic_measure_freq();
#అయితే_ముగింపు
    apic_సంస్కరణ.పదాలు[0] = apic_read_reg(APIC_సంస్కరణ);

    /* check for correct version (both APIC and x2APIC): 0x1X */
    ఒకవేళ (apic_సంస్కరణ_get_సంస్కరణ(apic_సంస్కరణ) >> 4 != 1) {
        రాయి("APIC: apic_version must be 0x1X\n");
        తిరిగి false;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (x86_KSapic_నిష్పత్తి != 0) {
        /* initialise APIC timer */
        apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    }
#అయితే_ముగింపు

    /* check for correct number of LVT entries */
    అంకె_lvt_entries = apic_సంస్కరణ_get_max_lvt_నమోదు(apic_సంస్కరణ) + 1;
    ఒకవేళ (అంకె_lvt_entries < 3) {
        రాయి("APIC: number of LVT entries: %d\n", అంకె_lvt_entries);
        రాయి("APIC: number of LVT entries must be >= 3\n");
        తిరిగి false;
    }

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
    /* initialise APIC timer */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_లెక్క, apic_khz * CONFIG_TIMER_TICK_MS);
#అయితే_ముగింపు

    /* enable APIC using SVR register */
    apic_write_reg(
        APIC_SVR,
        apic_svr_new(
            0,           /* focus_processor_chk */
            1,           /* enabled             */
            int_spurious /* spurious_vector     */
        ).పదాలు[0]
    );

    /* mask/unmask LINT0 (used for legacy IRQ delivery) */
    apic_write_reg(
        APIC_LVT_LINT0,
        apic_lvt_new(
            0,                /* timer_mode      */
            ముసుగు_legacy_irqs, /* masked          */
            0,                /* trigger_mode    */
            0,                /* remote_irr      */
            0,                /* pin_polarity    */
            0,                /* delivery_status */
            7,                /* delivery_mode   */
            0                 /*సదిశ*/
        ).పదాలు[0]
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
            0   /*సదిశ*/
        ).పదాలు[0]
    );

    /* initialise timer */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    uint32_t timer_బహుళకం = x86_KSapic_నిష్పత్తి == 0 ? APIC_TIMER_బహుళకం_TSC_DEADLINE :
                          APIC_TIMER_బహుళకం_ONE_SHOT;
#లేకపోతే
    uint32_t timer_బహుళకం = 1;
#అయితే_ముగింపు
    apic_write_reg(
        APIC_LVT_TIMER,
        apic_lvt_new(
            timer_బహుళకం,
            0,        /* masked          */
            0,        /* trigger_mode    */
            0,        /* remote_irr      */
            0,        /* pin_polarity    */
            0,        /* delivery_status */
            0,        /* delivery_mode   */
            int_timer /*సదిశ*/
        ).పదాలు[0]
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
    తిరిగి true;
}

శూన్య apic_ack_చురుకైన_అంతరాయం(శూన్య)
{
    apic_write_reg(APIC_EOI, 0);
}
