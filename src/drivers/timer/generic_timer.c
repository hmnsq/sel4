/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/timer/arm_generic.h>

BOOT_ਕੋਡ ਖਾਲੀ initGenericTimer(ਖਾਲੀ)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_DEBUG_BUILD)) {
        /* check the frequency is correct */
        ਸ਼ਬਦ_t gpt_cntfrq = 0;
        SYSTEM_READ_ਸ਼ਬਦ(CNTFRQ, gpt_cntfrq);
        /* The CNTFRQ register is a 32-bit register, its value can safely be
         * compared with TIMER_CLOCK_HZ.
         */
        ਜੇ ((gpt_cntfrq != 0) && (gpt_cntfrq != TIMER_CLOCK_HZ)) {
            /* TIMER_CLOCK_HZ is defined as a unsigned long long constant on
             * every architecture. */
            ਲਿਖੋ("Warning:  gpt_cntfrq %"SEL4_PRIu_ਸ਼ਬਦ", expected %llu\n",
                   gpt_cntfrq, TIMER_CLOCK_HZ);
        }
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    /* this sets the irq to UINT64_MAX */
    ackDeadlineIRQ();
#ਨਹੀਂਤਾਂ /* CONFIG_KERNEL_MCS */
    resetTimer();
#ਜੇ_ਅੰਤ /* !CONFIG_KERNEL_MCS */
    SYSTEM_WRITE_ਸ਼ਬਦ(CNT_CTL, ਦੋਆਧਾਰੀ_ਅੰਕ(0));
}

/*
 * The exynos5 platforms require custom hardware initialisation before the
 * generic timer is usable. They need to overwrite initTimer before calling
 * initGenericTimer because of this. We cannot use a `weak` symbol definition
 * in this case because the kernel is built as a single file and multiple
 * symbol definitions with the same name are not allowed. We therefore resort
 * to ifdef'ing out this initTimer definition for exynos5 platforms.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_PLAT_EXYNOS5
BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{
    initGenericTimer();
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/vcpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <armv/vcpu.h>

/** MODIFIES: */
/** DONT_TRANSLATE */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t read_cntpct(ਖਾਲੀ)
{
    uint64_t val;
    SYSTEM_READ_64(CNTPCT, val);
    ਵਾਪਸ val;
}

ਸਥਿਰ ਖਾਲੀ save_virt_timer(vcpu_t *vcpu)
{
    /* Save control register */
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
    vcpu_hw_write_reg(seL4_VCPUReg_CNTV_CTL, 0);
    /* Save Compare Value and Offset registers */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#ਨਹੀਂਤਾਂ
    uint64_t cval = get_cntv_cval_64();
    uint64_t cntvoff = get_cntv_off_64();
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh, (ਸ਼ਬਦ_t)(cval >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow, (ਸ਼ਬਦ_t)cval);
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (ਸ਼ਬਦ_t)(cntvoff >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (ਸ਼ਬਦ_t)cntvoff);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#ਜੇ_ਅੰਤ
    check_export_arch_timer();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTIMER_UPDATE_VOFFSET
    /* Save counter value at the time the vcpu is disabled */
    vcpu->virtTimer.ਆਖਰੀ_pcount = read_cntpct();
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ restore_virt_timer(vcpu_t *vcpu)
{
    /* Restore virtual timer state */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#ਨਹੀਂਤਾਂ
    uint32_t cval_ਉੱਚਾ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh);
    uint32_t cval_ਨੀਵਾਂ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow);
    uint64_t cval = ((uint64_t)cval_ਉੱਚਾ << 32) | (uint64_t) cval_ਨੀਵਾਂ;
    ਸੈੱਟ_cntv_cval_64(cval);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#ਜੇ_ਅੰਤ

    /* Set virtual timer offset */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t pcount_delta;
    uint64_t current_cntpct = read_cntpct();
    pcount_delta = current_cntpct - vcpu->virtTimer.ਆਖਰੀ_pcount;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t ਵਿੱਥ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    ਵਿੱਥ += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFF, ਵਿੱਥ);
#ਜੇ_ਅੰਤ
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTVOFF);
#ਨਹੀਂਤਾਂ
    uint32_t ਵਿੱਥ_ਉੱਚਾ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh);
    uint32_t ਵਿੱਥ_ਨੀਵਾਂ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFlow);
    uint64_t ਵਿੱਥ = ((uint64_t)ਵਿੱਥ_ਉੱਚਾ << 32) | (uint64_t) ਵਿੱਥ_ਨੀਵਾਂ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTIMER_UPDATE_VOFFSET
    ਵਿੱਥ += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (ਸ਼ਬਦ_t)(ਵਿੱਥ >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (ਸ਼ਬਦ_t) ਵਿੱਥ);
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_cntv_off_64(ਵਿੱਥ);
#ਜੇ_ਅੰਤ
    /* For verification, need to ensure we don't unmask an inactive interrupt;
     * the virtual timer should never get disabled, but the knowledge is not
     * available at this point */
    /* Restore interrupt mask state */
    ਜੇ (likely(isIRQActive(CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(CURRENT_CPU_ਸੂਚਕਾਂਕ(), ਰੁਕਾਵਟ_VTIMER_EVENT)))) {
        ਨਕਾਬ_ਰੁਕਾਵਟ(vcpu->vppi_masked[irq_VPPIEvent_ਸੂਚਕਾਂਕ(CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(CURRENT_CPU_ਸੂਚਕਾਂਕ(), ਰੁਕਾਵਟ_VTIMER_EVENT))],
                      CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(CURRENT_CPU_ਸੂਚਕਾਂਕ(), ਰੁਕਾਵਟ_VTIMER_EVENT));
    }
    /* Restore virtual timer control register */
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
}

#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
