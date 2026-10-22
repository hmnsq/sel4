/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <drivers/timer/arm_generic.h>

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initGenericTimer(ಶೂನ್ಯ)
{
    ಆದರೆ (config_ಗಣ(CONFIG_DEBUG_BUILD)) {
        /* check the frequency is correct */
        ಶಬ್ದ_t gpt_cntfrq = 0;
        SYSTEM_READ_ಶಬ್ದ(CNTFRQ, gpt_cntfrq);
        /* The CNTFRQ register is a 32-bit register, its value can safely be
         * compared with TIMER_CLOCK_HZ.
         */
        ಆದರೆ ((gpt_cntfrq != 0) && (gpt_cntfrq != TIMER_CLOCK_HZ)) {
            /* TIMER_CLOCK_HZ is defined as a unsigned long long constant on
             * every architecture. */
            ಬರೆ("Warning:  gpt_cntfrq %"SEL4_PRIu_ಶಬ್ದ", expected %llu\n",
                   gpt_cntfrq, TIMER_CLOCK_HZ);
        }
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* this sets the irq to UINT64_MAX */
    ackDeadlineIRQ();
#ಇಲ್ಲದಿದ್ದರೆ /* CONFIG_KERNEL_MCS */
    resetTimer();
#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_KERNEL_MCS */
    SYSTEM_WRITE_ಶಬ್ದ(CNT_CTL, ಬಿಟ್(0));
}

/*
 * The exynos5 platforms require custom hardware initialisation before the
 * generic timer is usable. They need to overwrite initTimer before calling
 * initGenericTimer because of this. We cannot use a `weak` symbol definition
 * in this case because the kernel is built as a single file and multiple
 * symbol definitions with the same name are not allowed. We therefore resort
 * to ifdef'ing out this initTimer definition for exynos5 platforms.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_PLAT_EXYNOS5
BOOT_ಸಂಕೇತ ಶೂನ್ಯ initTimer(ಶೂನ್ಯ)
{
    initGenericTimer();
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT

#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <armv/vcpu.h>

/** MODIFIES: */
/** DONT_TRANSLATE */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t read_cntpct(ಶೂನ್ಯ)
{
    uint64_t val;
    SYSTEM_READ_64(CNTPCT, val);
    ಹಿಂತಿರುಗಿ val;
}

ಸ್ಥಿರ ಶೂನ್ಯ save_virt_timer(vcpu_t *vcpu)
{
    /* Save control register */
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
    vcpu_hw_write_reg(seL4_VCPUReg_CNTV_CTL, 0);
    /* Save Compare Value and Offset registers */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#ಇಲ್ಲದಿದ್ದರೆ
    uint64_t cval = get_cntv_cval_64();
    uint64_t cntvoff = get_cntv_off_64();
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh, (ಶಬ್ದ_t)(cval >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow, (ಶಬ್ದ_t)cval);
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (ಶಬ್ದ_t)(cntvoff >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (ಶಬ್ದ_t)cntvoff);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#ಆದರೆ_ಅಂತ್ಯ
    check_export_arch_timer();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTIMER_UPDATE_VOFFSET
    /* Save counter value at the time the vcpu is disabled */
    vcpu->virtTimer.ಕೊನೆಯ_pcount = read_cntpct();
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ restore_virt_timer(vcpu_t *vcpu)
{
    /* Restore virtual timer state */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#ಇಲ್ಲದಿದ್ದರೆ
    uint32_t cval_ಮೇಲಿನ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh);
    uint32_t cval_ಕೆಳಗಿನ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow);
    uint64_t cval = ((uint64_t)cval_ಮೇಲಿನ << 32) | (uint64_t) cval_ಕೆಳಗಿನ;
    ಗಣ_cntv_cval_64(cval);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#ಆದರೆ_ಅಂತ್ಯ

    /* Set virtual timer offset */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t pcount_delta;
    uint64_t current_cntpct = read_cntpct();
    pcount_delta = current_cntpct - vcpu->virtTimer.ಕೊನೆಯ_pcount;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t ಸ್ಥಾನಾಂತರ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    ಸ್ಥಾನಾಂತರ += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFF, ಸ್ಥಾನಾಂತರ);
#ಆದರೆ_ಅಂತ್ಯ
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTVOFF);
#ಇಲ್ಲದಿದ್ದರೆ
    uint32_t ಸ್ಥಾನಾಂತರ_ಮೇಲಿನ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh);
    uint32_t ಸ್ಥಾನಾಂತರ_ಕೆಳಗಿನ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFlow);
    uint64_t ಸ್ಥಾನಾಂತರ = ((uint64_t)ಸ್ಥಾನಾಂತರ_ಮೇಲಿನ << 32) | (uint64_t) ಸ್ಥಾನಾಂತರ_ಕೆಳಗಿನ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTIMER_UPDATE_VOFFSET
    ಸ್ಥಾನಾಂತರ += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (ಶಬ್ದ_t)(ಸ್ಥಾನಾಂತರ >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (ಶಬ್ದ_t) ಸ್ಥಾನಾಂತರ);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_cntv_off_64(ಸ್ಥಾನಾಂತರ);
#ಆದರೆ_ಅಂತ್ಯ
    /* For verification, need to ensure we don't unmask an inactive interrupt;
     * the virtual timer should never get disabled, but the knowledge is not
     * available at this point */
    /* Restore interrupt mask state */
    ಆದರೆ (likely(isIRQActive(CORE_IRQ_ಗಮ್ಯ_IRQT(CURRENT_CPU_ಸೂಚ್ಯಂಕ(), ಅಡಚಣೆ_VTIMER_EVENT)))) {
        ಮುಖವಾಡ_ಅಡಚಣೆ(vcpu->vppi_masked[irq_VPPIEvent_ಸೂಚ್ಯಂಕ(CORE_IRQ_ಗಮ್ಯ_IRQT(CURRENT_CPU_ಸೂಚ್ಯಂಕ(), ಅಡಚಣೆ_VTIMER_EVENT))],
                      CORE_IRQ_ಗಮ್ಯ_IRQT(CURRENT_CPU_ಸೂಚ್ಯಂಕ(), ಅಡಚಣೆ_VTIMER_EVENT));
    }
    /* Restore virtual timer control register */
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
