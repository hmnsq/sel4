/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <machine/io.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/apic.h>
#شامل <linker.h>
#شامل <plat/machine/devices.h>
#شامل <plat/machine/pit.h>

#define CPUID_TSC_DEADLINE_BIT 24u
#define APIC_TIMER_MODE_ONE_SHOT 0
#define APIC_TIMER_MODE_TSC_DEADLINE  2

ایستا BOOT_CODE uint32_t apic_measure_freq(تهی)
{
    pit_init();
    /* wait for 1st PIT wraparound */
    pit_wait_wraparound();

    /* start APIC timer countdown */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_شمار, 0xffffffff);

    /* wait for 2nd PIT wraparound */
    pit_wait_wraparound();

    /* calculate APIC/bus cycles per ms = frequency in kHz */
    بازگشت (0xffffffff - apic_read_reg(APIC_TIMER_CURRENT)) / PIT_WRAPAROUND_MS;
}

BOOT_CODE paddr_t apic_get_base_paddr(تهی)
{
    apic_base_msr_t apic_base_msr;

    apic_base_msr.واژگان[0] = x86_rdmsr_کم(IA32_APIC_BASE_MSR);
    بازگشت apic_base_msr_get_base_addr(apic_base_msr);
}

BOOT_CODE bool_t apic_init(bool_t mask_legacy_irqs)
{
    apic_version_t apic_version;
    uint32_t رقم_lvt_entries;
    uint32_t apic_khz;

    اگر (!apic_enable()) {
        بازگشت false;
    }

#ifdef CONFIG_KERNEL_MCS
    /* find tsc KHz */
    x86KStscMhz = tsc_init();

    /* can we use tsc deadline mode? */
    uint32_t cpuid = x86_cpuid_ecx(0x1, 0x0);
    اگر (!(cpuid & BIT(CPUID_TSC_DEADLINE_BIT))) {
        apic_khz = apic_measure_freq();
        x86KSapicRatio = div64((uint64_t)x86KStscMhz * 1000llu, apic_khz);
        printf("Apic Khz %lu, TSC Mhz %lu, ratio %lu\n", (long) apic_khz, (long) x86KStscMhz, (long) x86KSapicRatio);
    } وگرنه {
        // use tsc deadline mode
        x86KSapicRatio = 0;
    }
#وگرنه
    apic_khz = apic_measure_freq();
#endif
    apic_version.واژگان[0] = apic_read_reg(APIC_VERSION);

    /* check for correct version (both APIC and x2APIC): 0x1X */
    اگر (apic_version_get_version(apic_version) >> 4 != 1) {
        printf("APIC: apic_version must be 0x1X\n");
        بازگشت false;
    }

#ifdef CONFIG_KERNEL_MCS
    اگر (x86KSapicRatio != 0) {
        /* initialise APIC timer */
        apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    }
#endif

    /* check for correct number of LVT entries */
    رقم_lvt_entries = apic_version_get_max_lvt_entry(apic_version) + 1;
    اگر (رقم_lvt_entries < 3) {
        printf("APIC: number of LVT entries: %d\n", رقم_lvt_entries);
        printf("APIC: number of LVT entries must be >= 3\n");
        بازگشت false;
    }

#ifndef CONFIG_KERNEL_MCS
    /* initialise APIC timer */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_شمار, apic_khz * CONFIG_TIMER_TICK_MS);
#endif

    /* enable APIC using SVR register */
    apic_write_reg(
        APIC_SVR,
        apic_svr_new(
            0,           /* focus_processor_chk */
            1,           /* enabled             */
            int_spurious /* spurious_vector     */
        ).واژگان[0]
    );

    /* mask/unmask LINT0 (used for legacy IRQ delivery) */
    apic_write_reg(
        APIC_LVT_LINT0,
        apic_lvt_new(
            0,                /* timer_mode      */
            mask_legacy_irqs, /* masked          */
            0,                /* trigger_mode    */
            0,                /* remote_irr      */
            0,                /* pin_polarity    */
            0,                /* delivery_status */
            7,                /* delivery_mode   */
            0                 /* vector          */
        ).واژگان[0]
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
            0   /* vector          */
        ).واژگان[0]
    );

    /* initialise timer */
#ifdef CONFIG_KERNEL_MCS
    uint32_t timer_mode = x86KSapicRatio == 0 ? APIC_TIMER_MODE_TSC_DEADLINE :
                          APIC_TIMER_MODE_ONE_SHOT;
#وگرنه
    uint32_t timer_mode = 1;
#endif
    apic_write_reg(
        APIC_LVT_TIMER,
        apic_lvt_new(
            timer_mode,
            0,        /* masked          */
            0,        /* trigger_mode    */
            0,        /* remote_irr      */
            0,        /* pin_polarity    */
            0,        /* delivery_status */
            0,        /* delivery_mode   */
            int_timer /* vector          */
        ).واژگان[0]
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
    بازگشت true;
}

تهی apic_ack_active_interrupt(تهی)
{
    apic_write_reg(APIC_EOI, 0);
}
