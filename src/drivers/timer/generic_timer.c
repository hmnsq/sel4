/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <drivers/timer/arm_generic.h>

BOOT_کد فراغ initGenericTimer(فراغ)
{
    إذا (config_مجموعه(CONFIG_DEBUG_BUILD)) {
        /* check the frequency is correct */
        کلمه_t gpt_cntfrq = 0;
        SYSTEM_READ_کلمه(CNTFRQ, gpt_cntfrq);
        /* The CNTFRQ register is a 32-bit register, its value can safely be
         * compared with TIMER_CLOCK_HZ.
         */
        إذا ((gpt_cntfrq != 0) && (gpt_cntfrq != TIMER_CLOCK_HZ)) {
            /* TIMER_CLOCK_HZ is defined as a unsigned long long constant on
             * every architecture. */
            چاپ_کن("Warning:  gpt_cntfrq %"SEL4_PRIu_کلمه", expected %llu\n",
                   gpt_cntfrq, TIMER_CLOCK_HZ);
        }
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    /* this sets the irq to UINT64_MAX */
    ackDeadlineIRQ();
#وإلا /* CONFIG_KERNEL_MCS */
    resetTimer();
#پایان_اگر /* !CONFIG_KERNEL_MCS */
    SYSTEM_WRITE_کلمه(CNT_CTL, بیت(0));
}

/*
 * The exynos5 platforms require custom hardware initialisation before the
 * generic timer is usable. They need to overwrite initTimer before calling
 * initGenericTimer because of this. We cannot use a `weak` symbol definition
 * in this case because the kernel is built as a single file and multiple
 * symbol definitions with the same name are not allowed. We therefore resort
 * to ifdef'ing out this initTimer definition for exynos5 platforms.
 */
#اگر_تعریف_نشده CONFIG_PLAT_EXYNOS5
BOOT_کد فراغ initTimer(فراغ)
{
    initGenericTimer();
}
#پایان_اگر

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT

#ضمن <arch/object/vcpu.h>
#ضمن <armv/vcpu.h>

/** MODIFIES: */
/** DONT_TRANSLATE */
ساكن مضمن uint64_t read_cntpct(فراغ)
{
    uint64_t val;
    SYSTEM_READ_64(CNTPCT, val);
    أرجع val;
}

ساكن فراغ save_virt_timer(vcpu_t *vcpu)
{
    /* Save control register */
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
    vcpu_hw_write_reg(seL4_VCPUReg_CNTV_CTL, 0);
    /* Save Compare Value and Offset registers */
#اگر_تعریف_شده CONFIG_ARCH_AARCH64
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#وإلا
    uint64_t cval = get_cntv_cval_64();
    uint64_t cntvoff = get_cntv_off_64();
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh, (کلمه_t)(cval >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow, (کلمه_t)cval);
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (کلمه_t)(cntvoff >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (کلمه_t)cntvoff);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#پایان_اگر
    check_export_arch_timer();
#اگر_تعریف_شده CONFIG_VTIMER_UPDATE_VOFFSET
    /* Save counter value at the time the vcpu is disabled */
    vcpu->virtTimer.آخرین_pcount = read_cntpct();
#پایان_اگر
}

ساكن فراغ restore_virt_timer(vcpu_t *vcpu)
{
    /* Restore virtual timer state */
#اگر_تعریف_شده CONFIG_ARCH_AARCH64
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#وإلا
    uint32_t cval_بالا = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh);
    uint32_t cval_کم = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow);
    uint64_t cval = ((uint64_t)cval_بالا << 32) | (uint64_t) cval_کم;
    مجموعه_cntv_cval_64(cval);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#پایان_اگر

    /* Set virtual timer offset */
#اگر_تعریف_شده CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t pcount_delta;
    uint64_t current_cntpct = read_cntpct();
    pcount_delta = current_cntpct - vcpu->virtTimer.آخرین_pcount;
#پایان_اگر
#اگر_تعریف_شده CONFIG_ARCH_AARCH64
#اگر_تعریف_شده CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t جابه_جایی = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    جابه_جایی += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFF, جابه_جایی);
#پایان_اگر
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTVOFF);
#وإلا
    uint32_t جابه_جایی_بالا = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh);
    uint32_t جابه_جایی_کم = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFlow);
    uint64_t جابه_جایی = ((uint64_t)جابه_جایی_بالا << 32) | (uint64_t) جابه_جایی_کم;
#اگر_تعریف_شده CONFIG_VTIMER_UPDATE_VOFFSET
    جابه_جایی += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (کلمه_t)(جابه_جایی >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (کلمه_t) جابه_جایی);
#پایان_اگر
    مجموعه_cntv_off_64(جابه_جایی);
#پایان_اگر
    /* For verification, need to ensure we don't unmask an inactive interrupt;
     * the virtual timer should never get disabled, but the knowledge is not
     * available at this point */
    /* Restore interrupt mask state */
    إذا (likely(isIRQActive(CORE_IRQ_مقصد_IRQT(CURRENT_CPU_شاخص(), وقفه_VTIMER_EVENT)))) {
        ماسک_وقفه(vcpu->vppi_masked[irq_VPPIEvent_شاخص(CORE_IRQ_مقصد_IRQT(CURRENT_CPU_شاخص(), وقفه_VTIMER_EVENT))],
                      CORE_IRQ_مقصد_IRQT(CURRENT_CPU_شاخص(), وقفه_VTIMER_EVENT));
    }
    /* Restore virtual timer control register */
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
}

#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
