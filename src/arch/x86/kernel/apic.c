/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/apic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/pit.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPUID_TSC_DEADLINE_ਦੋਆਧਾਰੀ_ਅੰਕ 24u
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TIMER_ਬਹੁਲਕ_ONE_SHOT 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ APIC_TIMER_ਬਹੁਲਕ_TSC_DEADLINE  2

ਸਥਿਰ BOOT_ਕੋਡ uint32_t apic_measure_freq(ਖਾਲੀ)
{
    pit_init();
    /* wait for 1st PIT wraparound */
    pit_wait_wraparound();

    /* start APIC timer countdown */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_ਗਿਣਤੀ, 0xffffffff);

    /* wait for 2nd PIT wraparound */
    pit_wait_wraparound();

    /* calculate APIC/bus cycles per ms = frequency in kHz */
    ਵਾਪਸ (0xffffffff - apic_read_reg(APIC_TIMER_CURRENT)) / PIT_WRAPAROUND_MS;
}

BOOT_ਕੋਡ paddr_t apic_get_ਅਧਾਰ_paddr(ਖਾਲੀ)
{
    apic_ਅਧਾਰ_msr_t apic_ਅਧਾਰ_msr;

    apic_ਅਧਾਰ_msr.ਸ਼ਬਦ[0] = x86_rdmsr_ਨੀਵਾਂ(IA32_APIC_ਅਧਾਰ_MSR);
    ਵਾਪਸ apic_ਅਧਾਰ_msr_get_ਅਧਾਰ_addr(apic_ਅਧਾਰ_msr);
}

BOOT_ਕੋਡ bool_t apic_init(bool_t ਨਕਾਬ_legacy_irqs)
{
    apic_ਸੰਸਕਰਣ_t apic_ਸੰਸਕਰਣ;
    uint32_t ਅੰਕ_lvt_entries;
    uint32_t apic_khz;

    ਜੇ (!apic_enable()) {
        ਵਾਪਸ false;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    /* find tsc KHz */
    x86KStscMhz = tsc_init();

    /* can we use tsc deadline mode? */
    uint32_t cpuid = x86_cpuid_ecx(0x1, 0x0);
    ਜੇ (!(cpuid & ਦੋਆਧਾਰੀ_ਅੰਕ(CPUID_TSC_DEADLINE_ਦੋਆਧਾਰੀ_ਅੰਕ))) {
        apic_khz = apic_measure_freq();
        x86_KSapic_ਅਨੁਪਾਤ = div64((uint64_t)x86KStscMhz * 1000llu, apic_khz);
        ਲਿਖੋ("Apic Khz %lu, TSC Mhz %lu, ratio %lu\n", (ਲੰਮਾ) apic_khz, (ਲੰਮਾ) x86KStscMhz, (ਲੰਮਾ) x86_KSapic_ਅਨੁਪਾਤ);
    } ਨਹੀਂਤਾਂ {
        // use tsc deadline mode
        x86_KSapic_ਅਨੁਪਾਤ = 0;
    }
#ਨਹੀਂਤਾਂ
    apic_khz = apic_measure_freq();
#ਜੇ_ਅੰਤ
    apic_ਸੰਸਕਰਣ.ਸ਼ਬਦ[0] = apic_read_reg(APIC_ਸੰਸਕਰਣ);

    /* check for correct version (both APIC and x2APIC): 0x1X */
    ਜੇ (apic_ਸੰਸਕਰਣ_get_ਸੰਸਕਰਣ(apic_ਸੰਸਕਰਣ) >> 4 != 1) {
        ਲਿਖੋ("APIC: apic_version must be 0x1X\n");
        ਵਾਪਸ false;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਜੇ (x86_KSapic_ਅਨੁਪਾਤ != 0) {
        /* initialise APIC timer */
        apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    }
#ਜੇ_ਅੰਤ

    /* check for correct number of LVT entries */
    ਅੰਕ_lvt_entries = apic_ਸੰਸਕਰਣ_get_max_lvt_ਇੰਦਰਾਜ(apic_ਸੰਸਕਰਣ) + 1;
    ਜੇ (ਅੰਕ_lvt_entries < 3) {
        ਲਿਖੋ("APIC: number of LVT entries: %d\n", ਅੰਕ_lvt_entries);
        ਲਿਖੋ("APIC: number of LVT entries must be >= 3\n");
        ਵਾਪਸ false;
    }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    /* initialise APIC timer */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_ਗਿਣਤੀ, apic_khz * CONFIG_TIMER_TICK_MS);
#ਜੇ_ਅੰਤ

    /* enable APIC using SVR register */
    apic_write_reg(
        APIC_SVR,
        apic_svr_new(
            0,           /* focus_processor_chk */
            1,           /* enabled             */
            int_spurious /* spurious_vector     */
        ).ਸ਼ਬਦ[0]
    );

    /* mask/unmask LINT0 (used for legacy IRQ delivery) */
    apic_write_reg(
        APIC_LVT_LINT0,
        apic_lvt_new(
            0,                /* timer_mode      */
            ਨਕਾਬ_legacy_irqs, /* masked          */
            0,                /* trigger_mode    */
            0,                /* remote_irr      */
            0,                /* pin_polarity    */
            0,                /* delivery_status */
            7,                /* delivery_mode   */
            0                 /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
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
            0   /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );

    /* initialise timer */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    uint32_t timer_ਬਹੁਲਕ = x86_KSapic_ਅਨੁਪਾਤ == 0 ? APIC_TIMER_ਬਹੁਲਕ_TSC_DEADLINE :
                          APIC_TIMER_ਬਹੁਲਕ_ONE_SHOT;
#ਨਹੀਂਤਾਂ
    uint32_t timer_ਬਹੁਲਕ = 1;
#ਜੇ_ਅੰਤ
    apic_write_reg(
        APIC_LVT_TIMER,
        apic_lvt_new(
            timer_ਬਹੁਲਕ,
            0,        /* masked          */
            0,        /* trigger_mode    */
            0,        /* remote_irr      */
            0,        /* pin_polarity    */
            0,        /* delivery_status */
            0,        /* delivery_mode   */
            int_timer /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
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
    ਵਾਪਸ true;
}

ਖਾਲੀ apic_ack_ਸਰਗਰਮ_ਰੁਕਾਵਟ(ਖਾਲੀ)
{
    apic_write_reg(APIC_EOI, 0);
}
