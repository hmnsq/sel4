/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <drivers/timer/arm_generic.h>

BOOT_کوډ فراغ initGenericTimer(فراغ)
{
    إذا (config_سېټ(CONFIG_DEBUG_BUILD)) {
        /* check the frequency is correct */
        کلمه_t gpt_cntfrq = 0;
        SYSTEM_READ_کلمه(CNTFRQ, gpt_cntfrq);
        /* The CNTFRQ register is a 32-bit register, its value can safely be
         * compared with TIMER_CLOCK_HZ.
         */
        إذا ((gpt_cntfrq != 0) && (gpt_cntfrq != TIMER_CLOCK_HZ)) {
            /* TIMER_CLOCK_HZ is defined as a unsigned long long constant on
             * every architecture. */
            چاپ_کړه("Warning:  gpt_cntfrq %"SEL4_PRIu_کلمه", expected %llu\n",
                   gpt_cntfrq, TIMER_CLOCK_HZ);
        }
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    /* this sets the irq to UINT64_MAX */
    ackDeadlineIRQ();
#وإلا /* CONFIG_KERNEL_MCS */
    resetTimer();
#د_که_پای /* !CONFIG_KERNEL_MCS */
    SYSTEM_WRITE_کلمه(CNT_CTL, بټ(0));
}

/*
 * The exynos5 platforms require custom hardware initialisation before the
 * generic timer is usable. They need to overwrite initTimer before calling
 * initGenericTimer because of this. We cannot use a `weak` symbol definition
 * in this case because the kernel is built as a single file and multiple
 * symbol definitions with the same name are not allowed. We therefore resort
 * to ifdef'ing out this initTimer definition for exynos5 platforms.
 */
#که_نه_تعریف_شوی CONFIG_PLAT_EXYNOS5
BOOT_کوډ فراغ initTimer(فراغ)
{
    initGenericTimer();
}
#د_که_پای

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT

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
#که_تعریف_شوی CONFIG_ARCH_AARCH64
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
#د_که_پای
    check_export_arch_timer();
#که_تعریف_شوی CONFIG_VTIMER_UPDATE_VOFFSET
    /* Save counter value at the time the vcpu is disabled */
    vcpu->virtTimer.وروستی_pcount = read_cntpct();
#د_که_پای
}

ساكن فراغ restore_virt_timer(vcpu_t *vcpu)
{
    /* Restore virtual timer state */
#که_تعریف_شوی CONFIG_ARCH_AARCH64
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#وإلا
    uint32_t cval_لوړ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh);
    uint32_t cval_ټیټ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow);
    uint64_t cval = ((uint64_t)cval_لوړ << 32) | (uint64_t) cval_ټیټ;
    سېټ_cntv_cval_64(cval);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#د_که_پای

    /* Set virtual timer offset */
#که_تعریف_شوی CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t pcount_delta;
    uint64_t current_cntpct = read_cntpct();
    pcount_delta = current_cntpct - vcpu->virtTimer.وروستی_pcount;
#د_که_پای
#که_تعریف_شوی CONFIG_ARCH_AARCH64
#که_تعریف_شوی CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t دننه_واټن = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    دننه_واټن += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFF, دننه_واټن);
#د_که_پای
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTVOFF);
#وإلا
    uint32_t دننه_واټن_لوړ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh);
    uint32_t دننه_واټن_ټیټ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFlow);
    uint64_t دننه_واټن = ((uint64_t)دننه_واټن_لوړ << 32) | (uint64_t) دننه_واټن_ټیټ;
#که_تعریف_شوی CONFIG_VTIMER_UPDATE_VOFFSET
    دننه_واټن += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (کلمه_t)(دننه_واټن >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (کلمه_t) دننه_واټن);
#د_که_پای
    سېټ_cntv_off_64(دننه_واټن);
#د_که_پای
    /* For verification, need to ensure we don't unmask an inactive interrupt;
     * the virtual timer should never get disabled, but the knowledge is not
     * available at this point */
    /* Restore interrupt mask state */
    إذا (likely(isIRQActive(CORE_IRQ_منزل_IRQT(CURRENT_CPU_شاخص(), مداخله_VTIMER_EVENT)))) {
        ماسک_مداخله(vcpu->vppi_masked[irq_VPPIEvent_شاخص(CORE_IRQ_منزل_IRQT(CURRENT_CPU_شاخص(), مداخله_VTIMER_EVENT))],
                      CORE_IRQ_منزل_IRQT(CURRENT_CPU_شاخص(), مداخله_VTIMER_EVENT));
    }
    /* Restore virtual timer control register */
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
}

#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */
