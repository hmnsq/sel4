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

#تعریف_کرو CPUID_TSC_DEADLINE_ثنائی_ہندسہ 24u
#تعریف_کرو APIC_TIMER_منوال_ONE_SHOT 0
#تعریف_کرو APIC_TIMER_منوال_TSC_DEADLINE  2

ساکن BOOT_رمز uint32_t apic_measure_freq(خالی)
{
    pit_init();
    /* wait for 1st PIT wraparound */
    pit_wait_wraparound();

    /* start APIC timer countdown */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_گنتی, 0xffffffff);

    /* wait for 2nd PIT wraparound */
    pit_wait_wraparound();

    /* calculate APIC/bus cycles per ms = frequency in kHz */
    واپس (0xffffffff - apic_read_reg(APIC_TIMER_CURRENT)) / PIT_WRAPAROUND_MS;
}

BOOT_رمز paddr_t apic_get_بنیاد_paddr(خالی)
{
    apic_بنیاد_msr_t apic_بنیاد_msr;

    apic_بنیاد_msr.الفاظ[0] = x86_rdmsr_کم(IA32_APIC_بنیاد_MSR);
    واپس apic_بنیاد_msr_get_بنیاد_addr(apic_بنیاد_msr);
}

BOOT_رمز bool_t apic_init(bool_t نقاب_legacy_irqs)
{
    apic_نسخہ_t apic_نسخہ;
    uint32_t عدد_lvt_entries;
    uint32_t apic_khz;

    اگر (!apic_enable()) {
        واپس false;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* find tsc KHz */
    x86KStscMhz = tsc_init();

    /* can we use tsc deadline mode? */
    uint32_t cpuid = x86_cpuid_ecx(0x1, 0x0);
    اگر (!(cpuid & ثنائی_ہندسہ(CPUID_TSC_DEADLINE_ثنائی_ہندسہ))) {
        apic_khz = apic_measure_freq();
        x86_KSapic_تناسب = div64((uint64_t)x86KStscMhz * 1000llu, apic_khz);
        لکھو("Apic Khz %lu, TSC Mhz %lu, ratio %lu\n", (طویل) apic_khz, (طویل) x86KStscMhz, (طویل) x86_KSapic_تناسب);
    } ورنہ {
        // use tsc deadline mode
        x86_KSapic_تناسب = 0;
    }
#ورنہ
    apic_khz = apic_measure_freq();
#اگر_ختم
    apic_نسخہ.الفاظ[0] = apic_read_reg(APIC_نسخہ);

    /* check for correct version (both APIC and x2APIC): 0x1X */
    اگر (apic_نسخہ_get_نسخہ(apic_نسخہ) >> 4 != 1) {
        لکھو("APIC: apic_version must be 0x1X\n");
        واپس false;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (x86_KSapic_تناسب != 0) {
        /* initialise APIC timer */
        apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    }
#اگر_ختم

    /* check for correct number of LVT entries */
    عدد_lvt_entries = apic_نسخہ_get_max_lvt_اندراج(apic_نسخہ) + 1;
    اگر (عدد_lvt_entries < 3) {
        لکھو("APIC: number of LVT entries: %d\n", عدد_lvt_entries);
        لکھو("APIC: number of LVT entries must be >= 3\n");
        واپس false;
    }

#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* initialise APIC timer */
    apic_write_reg(APIC_TIMER_DIVIDE, 0xb); /* divisor = 1 */
    apic_write_reg(APIC_TIMER_گنتی, apic_khz * CONFIG_TIMER_TICK_MS);
#اگر_ختم

    /* enable APIC using SVR register */
    apic_write_reg(
        APIC_SVR,
        apic_svr_new(
            0,           /* focus_processor_chk */
            1,           /* enabled             */
            int_spurious /* spurious_vector     */
        ).الفاظ[0]
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
            0                 /*سمتیہ*/
        ).الفاظ[0]
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
            0   /*سمتیہ*/
        ).الفاظ[0]
    );

    /* initialise timer */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    uint32_t timer_منوال = x86_KSapic_تناسب == 0 ? APIC_TIMER_منوال_TSC_DEADLINE :
                          APIC_TIMER_منوال_ONE_SHOT;
#ورنہ
    uint32_t timer_منوال = 1;
#اگر_ختم
    apic_write_reg(
        APIC_LVT_TIMER,
        apic_lvt_new(
            timer_منوال,
            0,        /* masked          */
            0,        /* trigger_mode    */
            0,        /* remote_irr      */
            0,        /* pin_polarity    */
            0,        /* delivery_status */
            0,        /* delivery_mode   */
            int_timer /*سمتیہ*/
        ).الفاظ[0]
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
    واپس true;
}

خالی apic_ack_فعال_مداخلت(خالی)
{
    apic_write_reg(APIC_EOI, 0);
}
