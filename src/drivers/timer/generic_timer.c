/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <drivers/timer/arm_generic.h>

BOOT_ڪوڊ فراغ initGenericTimer(فراغ)
{
    إذا (config_سيٽ(CONFIG_DEBUG_BUILD)) {
        /* check the frequency is correct */
        لفظ_t gpt_cntfrq = 0;
        SYSTEM_READ_لفظ(CNTFRQ, gpt_cntfrq);
        /* The CNTFRQ register is a 32-bit register, its value can safely be
         * compared with TIMER_CLOCK_HZ.
         */
        إذا ((gpt_cntfrq != 0) && (gpt_cntfrq != TIMER_CLOCK_HZ)) {
            /* TIMER_CLOCK_HZ is defined as a unsigned long long constant on
             * every architecture. */
            لکو("Warning:  gpt_cntfrq %"SEL4_PRIu_لفظ", expected %llu\n",
                   gpt_cntfrq, TIMER_CLOCK_HZ);
        }
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    /* this sets the irq to UINT64_MAX */
    ackDeadlineIRQ();
#وإلا /* CONFIG_KERNEL_MCS */
    resetTimer();
#جيڪڏهن_پورو /* !CONFIG_KERNEL_MCS */
    SYSTEM_WRITE_لفظ(CNT_CTL, ٻٽو_انگ(0));
}

/*
 * The exynos5 platforms require custom hardware initialisation before the
 * generic timer is usable. They need to overwrite initTimer before calling
 * initGenericTimer because of this. We cannot use a `weak` symbol definition
 * in this case because the kernel is built as a single file and multiple
 * symbol definitions with the same name are not allowed. We therefore resort
 * to ifdef'ing out this initTimer definition for exynos5 platforms.
 */
#جيڪڏهن_وصف_نه_ٿيل CONFIG_PLAT_EXYNOS5
BOOT_ڪوڊ فراغ initTimer(فراغ)
{
    initGenericTimer();
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT

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
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#وإلا
    uint64_t cval = get_cntv_cval_64();
    uint64_t cntvoff = get_cntv_off_64();
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh, (لفظ_t)(cval >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow, (لفظ_t)cval);
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (لفظ_t)(cntvoff >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (لفظ_t)cntvoff);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#جيڪڏهن_پورو
    check_export_arch_timer();
#جيڪڏهن_وصف_ٿيل CONFIG_VTIMER_UPDATE_VOFFSET
    /* Save counter value at the time the vcpu is disabled */
    vcpu->virtTimer.آخري_pcount = read_cntpct();
#جيڪڏهن_پورو
}

ساكن فراغ restore_virt_timer(vcpu_t *vcpu)
{
    /* Restore virtual timer state */
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#وإلا
    uint32_t cval_مٿاهون = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh);
    uint32_t cval_هيٺيون = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow);
    uint64_t cval = ((uint64_t)cval_مٿاهون << 32) | (uint64_t) cval_هيٺيون;
    سيٽ_cntv_cval_64(cval);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#جيڪڏهن_پورو

    /* Set virtual timer offset */
#جيڪڏهن_وصف_ٿيل CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t pcount_delta;
    uint64_t current_cntpct = read_cntpct();
    pcount_delta = current_cntpct - vcpu->virtTimer.آخري_pcount;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
#جيڪڏهن_وصف_ٿيل CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t وٿي = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    وٿي += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFF, وٿي);
#جيڪڏهن_پورو
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTVOFF);
#وإلا
    uint32_t وٿي_مٿاهون = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh);
    uint32_t وٿي_هيٺيون = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFlow);
    uint64_t وٿي = ((uint64_t)وٿي_مٿاهون << 32) | (uint64_t) وٿي_هيٺيون;
#جيڪڏهن_وصف_ٿيل CONFIG_VTIMER_UPDATE_VOFFSET
    وٿي += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (لفظ_t)(وٿي >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (لفظ_t) وٿي);
#جيڪڏهن_پورو
    سيٽ_cntv_off_64(وٿي);
#جيڪڏهن_پورو
    /* For verification, need to ensure we don't unmask an inactive interrupt;
     * the virtual timer should never get disabled, but the knowledge is not
     * available at this point */
    /* Restore interrupt mask state */
    إذا (likely(isIRQActive(CORE_IRQ_منزل_IRQT(CURRENT_CPU_اشاريو(), مداخلت_VTIMER_EVENT)))) {
        نقاب_مداخلت(vcpu->vppi_masked[irq_VPPIEvent_اشاريو(CORE_IRQ_منزل_IRQT(CURRENT_CPU_اشاريو(), مداخلت_VTIMER_EVENT))],
                      CORE_IRQ_منزل_IRQT(CURRENT_CPU_اشاريو(), مداخلت_VTIMER_EVENT));
    }
    /* Restore virtual timer control register */
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
}

#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
