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

#وصف_ڪريو CPUID_TSC_DEADLINE_ٻٽو_انگ 24u
#وصف_ڪريو APIC_TIMER_گهڻائي_ONE_SHOT 0
#وصف_ڪريو APIC_TIMER_گهڻائي_TSC_DEADLINE  2

ساكن BOOT_ڪوڊ uint32_t apic_measure_freq(فراغ)
{
    pit_init();
    /* wait for 1st PIT wraparound */
    pit_wait_wraparound();

    /* start APIC timer countdown */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_ڳڻپ, 0xffffffff);

    /* wait for 2nd PIT wraparound */
    pit_wait_wraparound();

    /* calculate APIC/bus cycles per ms = frequency in kHz */
    أرجع (0xffffffff - apic_read_reg(APIC_TIMER_CURRENT)) / PIT_WRAPAROUND_MS;
}

BOOT_ڪوڊ paddr_t apic_get_بنياد_paddr(فراغ)
{
    apic_بنياد_msr_t apic_بنياد_msr;

    apic_بنياد_msr.لفظ[0] = x86_rdmsr_هيٺيون(IA32_APIC_بنياد_MSR);
    أرجع apic_بنياد_msr_get_بنياد_addr(apic_بنياد_msr);
}

BOOT_ڪوڊ bool_t apic_init(bool_t نقاب_legacy_irqs)
{
    apic_نسخو_t apic_نسخو;
    uint32_t انگ_lvt_entries;
    uint32_t apic_khz;

    إذا (!apic_enable()) {
        أرجع false;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    /* find tsc KHz */
    x86KStscMhz = tsc_init();

    /* can we use tsc deadline mode? */
    uint32_t cpuid = x86_cpuid_ecx(0x1, 0x0);
    إذا (!(cpuid & ٻٽو_انگ(CPUID_TSC_DEADLINE_ٻٽو_انگ))) {
        apic_khz = apic_measure_freq();
        x86_KSapic_تناسب = div64((uint64_t)x86KStscMhz * 1000llu, apic_khz);
        لکو("Apic Khz %lu, TSC Mhz %lu, ratio %lu\n", (ڊگهو) apic_khz, (ڊگهو) x86KStscMhz, (ڊگهو) x86_KSapic_تناسب);
    } وإلا {
        // use tsc deadline mode
        x86_KSapic_تناسب = 0;
    }
#وإلا
    apic_khz = apic_measure_freq();
#جيڪڏهن_پورو
    apic_نسخو.لفظ[0] = apic_read_reg(APIC_نسخو);

    /* check for correct version (both APIC and x2APIC): 0x1X */
    إذا (apic_نسخو_get_نسخو(apic_نسخو) >> 4 != 1) {
        لکو("APIC: apic_version must be 0x1X\n");
        أرجع false;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    إذا (x86_KSapic_تناسب != 0) {
        /* initialise APIC timer */
        apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    }
#جيڪڏهن_پورو

    /* check for correct number of LVT entries */
    انگ_lvt_entries = apic_نسخو_get_max_lvt_داخلا(apic_نسخو) + 1;
    إذا (انگ_lvt_entries < 3) {
        لکو("APIC: number of LVT entries: %d\n", انگ_lvt_entries);
        لکو("APIC: number of LVT entries must be >= 3\n");
        أرجع false;
    }

#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
    /* initialise APIC timer */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_ڳڻپ, apic_khz * CONFIG_TIMER_TICK_MS);
#جيڪڏهن_پورو

    /* enable APIC using SVR register */
    apic_write_reg(
        APIC_SVR,
        apic_svr_new(
            0,           /* focus_processor_chk */
            1,           /* enabled             */
            int_spurious /* spurious_vector     */
        ).لفظ[0]
    );

    /* mask/unmask LINT0 (used for legacy IRQ delivery) */
    apic_write_reg(
        APIC_LVT_LINT0,
        apic_lvt_new(
            0,                /* timer_mode      */
            نقاب_legacy_irqs, /* masked          */
            0,                /* trigger_mode    */
            0,                /* remote_irr      */
            0,                /* pin_polarity    */
            0,                /* delivery_status */
            7,                /* delivery_mode   */
            0                 /*ويڪٽر*/
        ).لفظ[0]
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
            0   /*ويڪٽر*/
        ).لفظ[0]
    );

    /* initialise timer */
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    uint32_t timer_گهڻائي = x86_KSapic_تناسب == 0 ? APIC_TIMER_گهڻائي_TSC_DEADLINE :
                          APIC_TIMER_گهڻائي_ONE_SHOT;
#وإلا
    uint32_t timer_گهڻائي = 1;
#جيڪڏهن_پورو
    apic_write_reg(
        APIC_LVT_TIMER,
        apic_lvt_new(
            timer_گهڻائي,
            0,        /* masked          */
            0,        /* trigger_mode    */
            0,        /* remote_irr      */
            0,        /* pin_polarity    */
            0,        /* delivery_status */
            0,        /* delivery_mode   */
            int_timer /*ويڪٽر*/
        ).لفظ[0]
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

فراغ apic_ack_سرگرم_مداخلت(فراغ)
{
    apic_write_reg(APIC_EOI, 0);
}
