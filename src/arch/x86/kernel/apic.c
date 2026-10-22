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

#تعریف_کن CPUID_TSC_DEADLINE_بیت 24u
#تعریف_کن APIC_TIMER_نما_ONE_SHOT 0
#تعریف_کن APIC_TIMER_نما_TSC_DEADLINE  2

ساكن BOOT_کد uint32_t apic_measure_freq(فراغ)
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
    أرجع (0xffffffff - apic_read_reg(APIC_TIMER_CURRENT)) / PIT_WRAPAROUND_MS;
}

BOOT_کد paddr_t apic_get_پایه_paddr(فراغ)
{
    apic_پایه_msr_t apic_پایه_msr;

    apic_پایه_msr.واژگان[0] = x86_rdmsr_کم(IA32_APIC_پایه_MSR);
    أرجع apic_پایه_msr_get_پایه_addr(apic_پایه_msr);
}

BOOT_کد bool_t apic_init(bool_t ماسک_legacy_irqs)
{
    apic_نسخه_t apic_نسخه;
    uint32_t رقم_lvt_entries;
    uint32_t apic_khz;

    إذا (!apic_enable()) {
        أرجع false;
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    /* find tsc KHz */
    x86KStscMhz = tsc_init();

    /* can we use tsc deadline mode? */
    uint32_t cpuid = x86_cpuid_ecx(0x1, 0x0);
    إذا (!(cpuid & بیت(CPUID_TSC_DEADLINE_بیت))) {
        apic_khz = apic_measure_freq();
        x86_KSapic_نسبت = div64((uint64_t)x86KStscMhz * 1000llu, apic_khz);
        چاپ_کن("Apic Khz %lu, TSC Mhz %lu, ratio %lu\n", (دراز) apic_khz, (دراز) x86KStscMhz, (دراز) x86_KSapic_نسبت);
    } وإلا {
        // use tsc deadline mode
        x86_KSapic_نسبت = 0;
    }
#وإلا
    apic_khz = apic_measure_freq();
#پایان_اگر
    apic_نسخه.واژگان[0] = apic_read_reg(APIC_نسخه);

    /* check for correct version (both APIC and x2APIC): 0x1X */
    إذا (apic_نسخه_get_نسخه(apic_نسخه) >> 4 != 1) {
        چاپ_کن("APIC: apic_version must be 0x1X\n");
        أرجع false;
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    إذا (x86_KSapic_نسبت != 0) {
        /* initialise APIC timer */
        apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    }
#پایان_اگر

    /* check for correct number of LVT entries */
    رقم_lvt_entries = apic_نسخه_get_max_lvt_مدخل(apic_نسخه) + 1;
    إذا (رقم_lvt_entries < 3) {
        چاپ_کن("APIC: number of LVT entries: %d\n", رقم_lvt_entries);
        چاپ_کن("APIC: number of LVT entries must be >= 3\n");
        أرجع false;
    }

#اگر_تعریف_نشده CONFIG_KERNEL_MCS
    /* initialise APIC timer */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_شمار, apic_khz * CONFIG_TIMER_TICK_MS);
#پایان_اگر

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
            ماسک_legacy_irqs, /* masked          */
            0,                /* trigger_mode    */
            0,                /* remote_irr      */
            0,                /* pin_polarity    */
            0,                /* delivery_status */
            7,                /* delivery_mode   */
            0                 /*وکتور*/
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
            0   /*وکتور*/
        ).واژگان[0]
    );

    /* initialise timer */
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    uint32_t timer_نما = x86_KSapic_نسبت == 0 ? APIC_TIMER_نما_TSC_DEADLINE :
                          APIC_TIMER_نما_ONE_SHOT;
#وإلا
    uint32_t timer_نما = 1;
#پایان_اگر
    apic_write_reg(
        APIC_LVT_TIMER,
        apic_lvt_new(
            timer_نما,
            0,        /* masked          */
            0,        /* trigger_mode    */
            0,        /* remote_irr      */
            0,        /* pin_polarity    */
            0,        /* delivery_status */
            0,        /* delivery_mode   */
            int_timer /*وکتور*/
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
    أرجع true;
}

فراغ apic_ack_فعال_وقفه(فراغ)
{
    apic_write_reg(APIC_EOI, 0);
}
