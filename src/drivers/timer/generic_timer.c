/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <drivers/timer/arm_generic.h>

BOOT_CODE تهی initGenericTimer(تهی)
{
    اگر (config_set(CONFIG_DEBUG_BUILD)) {
        /* check the frequency is correct */
        word_t gpt_cntfrq = 0;
        SYSTEM_READ_WORD(CNTFRQ, gpt_cntfrq);
        /* The CNTFRQ register is a 32-bit register, its value can safely be
         * compared with TIMER_CLOCK_HZ.
         */
        اگر ((gpt_cntfrq != 0) && (gpt_cntfrq != TIMER_CLOCK_HZ)) {
            /* TIMER_CLOCK_HZ is defined as a unsigned long long constant on
             * every architecture. */
            printf("Warning:  gpt_cntfrq %"SEL4_PRIu_word", expected %llu\n",
                   gpt_cntfrq, TIMER_CLOCK_HZ);
        }
    }

#ifdef CONFIG_KERNEL_MCS
    /* this sets the irq to UINT64_MAX */
    ackDeadlineIRQ();
#وگرنه /* CONFIG_KERNEL_MCS */
    resetTimer();
#endif /* !CONFIG_KERNEL_MCS */
    SYSTEM_WRITE_WORD(CNT_CTL, BIT(0));
}

/*
 * The exynos5 platforms require custom hardware initialisation before the
 * generic timer is usable. They need to overwrite initTimer before calling
 * initGenericTimer because of this. We cannot use a `weak` symbol definition
 * in this case because the kernel is built as a single file and multiple
 * symbol definitions with the same name are not allowed. We therefore resort
 * to ifdef'ing out this initTimer definition for exynos5 platforms.
 */
#ifndef CONFIG_PLAT_EXYNOS5
BOOT_CODE تهی initTimer(تهی)
{
    initGenericTimer();
}
#endif

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#شامل <arch/object/vcpu.h>
#شامل <armv/vcpu.h>

/** MODIFIES: */
/** DONT_TRANSLATE */
ایستا درونخطی uint64_t read_cntpct(تهی)
{
    uint64_t val;
    SYSTEM_READ_64(CNTPCT, val);
    بازگشت val;
}

ایستا تهی save_virt_timer(vcpu_t *vcpu)
{
    /* Save control register */
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
    vcpu_hw_write_reg(seL4_VCPUReg_CNTV_CTL, 0);
    /* Save Compare Value and Offset registers */
#ifdef CONFIG_ARCH_AARCH64
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#وگرنه
    uint64_t cval = get_cntv_cval_64();
    uint64_t cntvoff = get_cntv_off_64();
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh, (word_t)(cval >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow, (word_t)cval);
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (word_t)(cntvoff >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (word_t)cntvoff);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#endif
    check_export_arch_timer();
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Save counter value at the time the vcpu is disabled */
    vcpu->virtTimer.last_pcount = read_cntpct();
#endif
}

ایستا تهی restore_virt_timer(vcpu_t *vcpu)
{
    /* Restore virtual timer state */
#ifdef CONFIG_ARCH_AARCH64
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#وگرنه
    uint32_t cval_بالا = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh);
    uint32_t cval_کم = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow);
    uint64_t cval = ((uint64_t)cval_بالا << 32) | (uint64_t) cval_کم;
    set_cntv_cval_64(cval);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#endif

    /* Set virtual timer offset */
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t pcount_delta;
    uint64_t current_cntpct = read_cntpct();
    pcount_delta = current_cntpct - vcpu->virtTimer.last_pcount;
#endif
#ifdef CONFIG_ARCH_AARCH64
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t offset = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    offset += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFF, offset);
#endif
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTVOFF);
#وگرنه
    uint32_t offset_بالا = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh);
    uint32_t offset_کم = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFlow);
    uint64_t offset = ((uint64_t)offset_بالا << 32) | (uint64_t) offset_کم;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    offset += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (word_t)(offset >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (word_t) offset);
#endif
    set_cntv_off_64(offset);
#endif
    /* For verification, need to ensure we don't unmask an inactive interrupt;
     * the virtual timer should never get disabled, but the knowledge is not
     * available at this point */
    /* Restore interrupt mask state */
    اگر (likely(isIRQActive(CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), INTERRUPT_VTIMER_EVENT)))) {
        maskInterrupt(vcpu->vppi_masked[irqVPPIEventIndex(CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), INTERRUPT_VTIMER_EVENT))],
                      CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), INTERRUPT_VTIMER_EVENT));
    }
    /* Restore virtual timer control register */
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
}

#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
