/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <drivers/timer/arm_generic.h>

BOOT_رمز خالی initGenericTimer(خالی)
{
    اگر (config_سیٹ(CONFIG_DEBUG_BUILD)) {
        /* check the frequency is correct */
        لفظ_t gpt_cntfrq = 0;
        SYSTEM_READ_لفظ(CNTFRQ, gpt_cntfrq);
        /* The CNTFRQ register is a 32-bit register, its value can safely be
         * compared with TIMER_CLOCK_HZ.
         */
        اگر ((gpt_cntfrq != 0) && (gpt_cntfrq != TIMER_CLOCK_HZ)) {
            /* TIMER_CLOCK_HZ is defined as a unsigned long long constant on
             * every architecture. */
            لکھو("Warning:  gpt_cntfrq %"SEL4_PRIu_لفظ", expected %llu\n",
                   gpt_cntfrq, TIMER_CLOCK_HZ);
        }
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* this sets the irq to UINT64_MAX */
    ackDeadlineIRQ();
#ورنہ /* CONFIG_KERNEL_MCS */
    resetTimer();
#اگر_ختم /* !CONFIG_KERNEL_MCS */
    SYSTEM_WRITE_لفظ(CNT_CTL, ثنائی_ہندسہ(0));
}

/*
 * The exynos5 platforms require custom hardware initialisation before the
 * generic timer is usable. They need to overwrite initTimer before calling
 * initGenericTimer because of this. We cannot use a `weak` symbol definition
 * in this case because the kernel is built as a single file and multiple
 * symbol definitions with the same name are not allowed. We therefore resort
 * to ifdef'ing out this initTimer definition for exynos5 platforms.
 */
#اگر_غیر_تعریف_شدہ CONFIG_PLAT_EXYNOS5
BOOT_رمز خالی initTimer(خالی)
{
    initGenericTimer();
}
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT

#شامل <arch/object/vcpu.h>
#شامل <armv/vcpu.h>

/** MODIFIES: */
/** DONT_TRANSLATE */
ساکن اندرونی uint64_t read_cntpct(خالی)
{
    uint64_t val;
    SYSTEM_READ_64(CNTPCT, val);
    واپس val;
}

ساکن خالی save_virt_timer(vcpu_t *vcpu)
{
    /* Save control register */
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
    vcpu_hw_write_reg(seL4_VCPUReg_CNTV_CTL, 0);
    /* Save Compare Value and Offset registers */
#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#ورنہ
    uint64_t cval = get_cntv_cval_64();
    uint64_t cntvoff = get_cntv_off_64();
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh, (لفظ_t)(cval >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow, (لفظ_t)cval);
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (لفظ_t)(cntvoff >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (لفظ_t)cntvoff);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#اگر_ختم
    check_export_arch_timer();
#اگر_تعریف_شدہ CONFIG_VTIMER_UPDATE_VOFFSET
    /* Save counter value at the time the vcpu is disabled */
    vcpu->virtTimer.آخری_pcount = read_cntpct();
#اگر_ختم
}

ساکن خالی restore_virt_timer(vcpu_t *vcpu)
{
    /* Restore virtual timer state */
#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#ورنہ
    uint32_t cval_بلند = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh);
    uint32_t cval_کم = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow);
    uint64_t cval = ((uint64_t)cval_بلند << 32) | (uint64_t) cval_کم;
    سیٹ_cntv_cval_64(cval);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#اگر_ختم

    /* Set virtual timer offset */
#اگر_تعریف_شدہ CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t pcount_delta;
    uint64_t current_cntpct = read_cntpct();
    pcount_delta = current_cntpct - vcpu->virtTimer.آخری_pcount;
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
#اگر_تعریف_شدہ CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t انحراف = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    انحراف += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFF, انحراف);
#اگر_ختم
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTVOFF);
#ورنہ
    uint32_t انحراف_بلند = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh);
    uint32_t انحراف_کم = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFlow);
    uint64_t انحراف = ((uint64_t)انحراف_بلند << 32) | (uint64_t) انحراف_کم;
#اگر_تعریف_شدہ CONFIG_VTIMER_UPDATE_VOFFSET
    انحراف += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (لفظ_t)(انحراف >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (لفظ_t) انحراف);
#اگر_ختم
    سیٹ_cntv_off_64(انحراف);
#اگر_ختم
    /* For verification, need to ensure we don't unmask an inactive interrupt;
     * the virtual timer should never get disabled, but the knowledge is not
     * available at this point */
    /* Restore interrupt mask state */
    اگر (likely(isIRQActive(CORE_IRQ_مقصد_IRQT(CURRENT_CPU_اشاریہ(), مداخلت_VTIMER_EVENT)))) {
        نقاب_مداخلت(vcpu->vppi_masked[irq_VPPIEvent_اشاریہ(CORE_IRQ_مقصد_IRQT(CURRENT_CPU_اشاریہ(), مداخلت_VTIMER_EVENT))],
                      CORE_IRQ_مقصد_IRQT(CURRENT_CPU_اشاریہ(), مداخلت_VTIMER_EVENT));
    }
    /* Restore virtual timer control register */
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
}

#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
