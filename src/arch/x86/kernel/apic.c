/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <machine/io.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/apic.h>
#ضمن <linker.h>
#ضمن <plat/machine/devices.h>
#ضمن <plat/machine/pit.h>

#تعریف_کړه CPUID_TSC_DEADLINE_بټ 24u
#تعریف_کړه APIC_TIMER_ډېرپېښ_ONE_SHOT 0
#تعریف_کړه APIC_TIMER_ډېرپېښ_TSC_DEADLINE  2

ساكن BOOT_کوډ uint32_t apic_measure_freq(فراغ)
{
    pit_init();
    /* wait for 1st PIT wraparound */
    pit_wait_wraparound();

    /* start APIC timer countdown */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_شمېر, 0xffffffff);

    /* wait for 2nd PIT wraparound */
    pit_wait_wraparound();

    /* calculate APIC/bus cycles per ms = frequency in kHz */
    أرجع (0xffffffff - apic_read_reg(APIC_TIMER_CURRENT)) / PIT_WRAPAROUND_MS;
}

BOOT_کوډ paddr_t apic_get_بنسټ_paddr(فراغ)
{
    apic_بنسټ_msr_t apic_بنسټ_msr;

    apic_بنسټ_msr.ټکي[0] = x86_rdmsr_ټیټ(IA32_APIC_بنسټ_MSR);
    أرجع apic_بنسټ_msr_get_بنسټ_addr(apic_بنسټ_msr);
}

BOOT_کوډ bool_t apic_init(bool_t ماسک_legacy_irqs)
{
    apic_نسخه_t apic_نسخه;
    uint32_t عدد_lvt_entries;
    uint32_t apic_khz;

    إذا (!apic_enable()) {
        أرجع false;
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    /* find tsc KHz */
    x86KStscMhz = tsc_init();

    /* can we use tsc deadline mode? */
    uint32_t cpuid = x86_cpuid_ecx(0x1, 0x0);
    إذا (!(cpuid & بټ(CPUID_TSC_DEADLINE_بټ))) {
        apic_khz = apic_measure_freq();
        x86_KSapic_نسبت = div64((uint64_t)x86KStscMhz * 1000llu, apic_khz);
        چاپ_کړه("Apic Khz %lu, TSC Mhz %lu, ratio %lu\n", (اوږد) apic_khz, (اوږد) x86KStscMhz, (اوږد) x86_KSapic_نسبت);
    } وإلا {
        // use tsc deadline mode
        x86_KSapic_نسبت = 0;
    }
#وإلا
    apic_khz = apic_measure_freq();
#د_که_پای
    apic_نسخه.ټکي[0] = apic_read_reg(APIC_نسخه);

    /* check for correct version (both APIC and x2APIC): 0x1X */
    إذا (apic_نسخه_get_نسخه(apic_نسخه) >> 4 != 1) {
        چاپ_کړه("APIC: apic_version must be 0x1X\n");
        أرجع false;
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (x86_KSapic_نسبت != 0) {
        /* initialise APIC timer */
        apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    }
#د_که_پای

    /* check for correct number of LVT entries */
    عدد_lvt_entries = apic_نسخه_get_max_lvt_ثبت(apic_نسخه) + 1;
    إذا (عدد_lvt_entries < 3) {
        چاپ_کړه("APIC: number of LVT entries: %d\n", عدد_lvt_entries);
        چاپ_کړه("APIC: number of LVT entries must be >= 3\n");
        أرجع false;
    }

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
    /* initialise APIC timer */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_شمېر, apic_khz * CONFIG_TIMER_TICK_MS);
#د_که_پای

    /* enable APIC using SVR register */
    apic_write_reg(
        APIC_SVR,
        apic_svr_new(
            0,           /* focus_processor_chk */
            1,           /* enabled             */
            int_spurious /* spurious_vector     */
        ).ټکي[0]
    );

    /* mask/unmask LINT0 (used for legacy IRQ delivery) */
    apic_write_reg(
        APIC_LVT_LINT0,
        apic_lvt_new(
            0,                /* timer_mode      */
            ماسک_legacy_irqs, /* masked          */
            0,                /* trigger_mode    */
            0,                /* remote_irr      */
            0,                /* pin_polarity    */
            0,                /* delivery_status */
            7,                /* delivery_mode   */
            0                 /*ویکټور*/
        ).ټکي[0]
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
            0   /*ویکټور*/
        ).ټکي[0]
    );

    /* initialise timer */
#که_تعریف_شوی CONFIG_KERNEL_MCS
    uint32_t timer_ډېرپېښ = x86_KSapic_نسبت == 0 ? APIC_TIMER_ډېرپېښ_TSC_DEADLINE :
                          APIC_TIMER_ډېرپېښ_ONE_SHOT;
#وإلا
    uint32_t timer_ډېرپېښ = 1;
#د_که_پای
    apic_write_reg(
        APIC_LVT_TIMER,
        apic_lvt_new(
            timer_ډېرپېښ,
            0,        /* masked          */
            0,        /* trigger_mode    */
            0,        /* remote_irr      */
            0,        /* pin_polarity    */
            0,        /* delivery_status */
            0,        /* delivery_mode   */
            int_timer /*ویکټور*/
        ).ټکي[0]
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
    أرجع true;
}

فراغ apic_ack_فعال_مداخله(فراغ)
{
    apic_write_reg(APIC_EOI, 0);
}
