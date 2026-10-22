/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/apic.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/pit.h>

#ವ್ಯಾಖ್ಯಾನಿಸು CPUID_TSC_DEADLINE_ಬಿಟ್ 24u
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TIMER_ಬಹುಲಕ_ONE_SHOT 0
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TIMER_ಬಹುಲಕ_TSC_DEADLINE  2

ಸ್ಥಿರ BOOT_ಸಂಕೇತ uint32_t apic_measure_freq(ಶೂನ್ಯ)
{
    pit_init();
    /* wait for 1st PIT wraparound */
    pit_wait_wraparound();

    /* start APIC timer countdown */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_ಎಣಿಕೆ, 0xffffffff);

    /* wait for 2nd PIT wraparound */
    pit_wait_wraparound();

    /* calculate APIC/bus cycles per ms = frequency in kHz */
    ಹಿಂತಿರುಗಿ (0xffffffff - apic_read_reg(APIC_TIMER_CURRENT)) / PIT_WRAPAROUND_MS;
}

BOOT_ಸಂಕೇತ paddr_t apic_get_ತಳ_paddr(ಶೂನ್ಯ)
{
    apic_ತಳ_msr_t apic_ತಳ_msr;

    apic_ತಳ_msr.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_APIC_ತಳ_MSR);
    ಹಿಂತಿರುಗಿ apic_ತಳ_msr_get_ತಳ_addr(apic_ತಳ_msr);
}

BOOT_ಸಂಕೇತ bool_t apic_init(bool_t ಮುಖವಾಡ_legacy_irqs)
{
    apic_ಆವೃತ್ತಿ_t apic_ಆವೃತ್ತಿ;
    uint32_t ಅಂಕೆ_lvt_entries;
    uint32_t apic_khz;

    ಆದರೆ (!apic_enable()) {
        ಹಿಂತಿರುಗಿ false;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* find tsc KHz */
    x86KStscMhz = tsc_init();

    /* can we use tsc deadline mode? */
    uint32_t cpuid = x86_cpuid_ecx(0x1, 0x0);
    ಆದರೆ (!(cpuid & ಬಿಟ್(CPUID_TSC_DEADLINE_ಬಿಟ್))) {
        apic_khz = apic_measure_freq();
        x86_KSapic_ಅನುಪಾತ = div64((uint64_t)x86KStscMhz * 1000llu, apic_khz);
        ಬರೆ("Apic Khz %lu, TSC Mhz %lu, ratio %lu\n", (ದೀರ್ಘ) apic_khz, (ದೀರ್ಘ) x86KStscMhz, (ದೀರ್ಘ) x86_KSapic_ಅನುಪಾತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // use tsc deadline mode
        x86_KSapic_ಅನುಪಾತ = 0;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    apic_khz = apic_measure_freq();
#ಆದರೆ_ಅಂತ್ಯ
    apic_ಆವೃತ್ತಿ.ಪದಗಳು[0] = apic_read_reg(APIC_ಆವೃತ್ತಿ);

    /* check for correct version (both APIC and x2APIC): 0x1X */
    ಆದರೆ (apic_ಆವೃತ್ತಿ_get_ಆವೃತ್ತಿ(apic_ಆವೃತ್ತಿ) >> 4 != 1) {
        ಬರೆ("APIC: apic_version must be 0x1X\n");
        ಹಿಂತಿರುಗಿ false;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (x86_KSapic_ಅನುಪಾತ != 0) {
        /* initialise APIC timer */
        apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* check for correct number of LVT entries */
    ಅಂಕೆ_lvt_entries = apic_ಆವೃತ್ತಿ_get_max_lvt_ನಮೂದು(apic_ಆವೃತ್ತಿ) + 1;
    ಆದರೆ (ಅಂಕೆ_lvt_entries < 3) {
        ಬರೆ("APIC: number of LVT entries: %d\n", ಅಂಕೆ_lvt_entries);
        ಬರೆ("APIC: number of LVT entries must be >= 3\n");
        ಹಿಂತಿರುಗಿ false;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
    /* initialise APIC timer */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_ಎಣಿಕೆ, apic_khz * CONFIG_TIMER_TICK_MS);
#ಆದರೆ_ಅಂತ್ಯ

    /* enable APIC using SVR register */
    apic_write_reg(
        APIC_SVR,
        apic_svr_new(
            0,           /* focus_processor_chk */
            1,           /* enabled             */
            int_spurious /* spurious_vector     */
        ).ಪದಗಳು[0]
    );

    /* mask/unmask LINT0 (used for legacy IRQ delivery) */
    apic_write_reg(
        APIC_LVT_LINT0,
        apic_lvt_new(
            0,                /* timer_mode      */
            ಮುಖವಾಡ_legacy_irqs, /* masked          */
            0,                /* trigger_mode    */
            0,                /* remote_irr      */
            0,                /* pin_polarity    */
            0,                /* delivery_status */
            7,                /* delivery_mode   */
            0                 /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
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
            0   /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
    );

    /* initialise timer */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    uint32_t timer_ಬಹುಲಕ = x86_KSapic_ಅನುಪಾತ == 0 ? APIC_TIMER_ಬಹುಲಕ_TSC_DEADLINE :
                          APIC_TIMER_ಬಹುಲಕ_ONE_SHOT;
#ಇಲ್ಲದಿದ್ದರೆ
    uint32_t timer_ಬಹುಲಕ = 1;
#ಆದರೆ_ಅಂತ್ಯ
    apic_write_reg(
        APIC_LVT_TIMER,
        apic_lvt_new(
            timer_ಬಹುಲಕ,
            0,        /* masked          */
            0,        /* trigger_mode    */
            0,        /* remote_irr      */
            0,        /* pin_polarity    */
            0,        /* delivery_status */
            0,        /* delivery_mode   */
            int_timer /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
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
    ಹಿಂತಿರುಗಿ true;
}

ಶೂನ್ಯ apic_ack_ಸಕ್ರಿಯ_ಅಡಚಣೆ(ಶೂನ್ಯ)
{
    apic_write_reg(APIC_EOI, 0);
}
