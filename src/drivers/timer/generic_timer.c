/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <drivers/timer/arm_generic.h>

BOOT_संहिता शून्य initGenericTimer(शून्य)
{
    यदि (config_समुच्चयः(CONFIG_DEBUG_BUILD)) {
        /* check the frequency is correct */
        word_t gpt_cntfrq = 0;
        SYSTEM_READ_WORD(CNTFRQ, gpt_cntfrq);
        /* The CNTFRQ register is a 32-bit register, its value can safely be
         * compared with TIMER_CLOCK_HZ.
         */
        यदि ((gpt_cntfrq != 0) && (gpt_cntfrq != TIMER_CLOCK_HZ)) {
            /* TIMER_CLOCK_HZ is defined as a unsigned long long constant on
             * every architecture. */
            प्रारूप_लिख("Warning:  gpt_cntfrq %"SEL4_PRIu_word", expected %llu\n",
                   gpt_cntfrq, TIMER_CLOCK_HZ);
        }
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* this sets the irq to UINT64_MAX */
    ackDeadlineIRQ();
#अन्यथा /* CONFIG_KERNEL_MCS */
    resetTimer();
#यदि_अन्तः /* !CONFIG_KERNEL_MCS */
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
#यदि_अपरिभाषितम् CONFIG_PLAT_EXYNOS5
BOOT_संहिता शून्य initTimer(शून्य)
{
    initGenericTimer();
}
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT

#अन्तर्भाव <arch/object/vcpu.h>
#अन्तर्भाव <armv/vcpu.h>

/** MODIFIES: */
/** DONT_TRANSLATE */
स्थिर अन्तःस्थ uint64_t read_cntpct(शून्य)
{
    uint64_t val;
    SYSTEM_READ_64(CNTPCT, val);
    प्रत्यागम val;
}

स्थिर शून्य save_virt_timer(vcpu_t *vcpu)
{
    /* Save control register */
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
    vcpu_hw_write_reg(seL4_VCPUReg_CNTV_CTL, 0);
    /* Save Compare Value and Offset registers */
#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#अन्यथा
    uint64_t cval = get_cntv_cval_64();
    uint64_t cntvoff = get_cntv_off_64();
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh, (word_t)(cval >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow, (word_t)cval);
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (word_t)(cntvoff >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (word_t)cntvoff);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#यदि_अन्तः
    check_export_arch_timer();
#यदि_परिभाषितम् CONFIG_VTIMER_UPDATE_VOFFSET
    /* Save counter value at the time the vcpu is disabled */
    vcpu->virtTimer.चरमः_pcount = read_cntpct();
#यदि_अन्तः
}

स्थिर शून्य restore_virt_timer(vcpu_t *vcpu)
{
    /* Restore virtual timer state */
#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#अन्यथा
    uint32_t cval_उच्चः = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh);
    uint32_t cval_नीचः = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow);
    uint64_t cval = ((uint64_t)cval_उच्चः << 32) | (uint64_t) cval_नीचः;
    समुच्चयः_cntv_cval_64(cval);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#यदि_अन्तः

    /* Set virtual timer offset */
#यदि_परिभाषितम् CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t pcount_delta;
    uint64_t current_cntpct = read_cntpct();
    pcount_delta = current_cntpct - vcpu->virtTimer.चरमः_pcount;
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
#यदि_परिभाषितम् CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t विस्थापनम् = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    विस्थापनम् += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFF, विस्थापनम्);
#यदि_अन्तः
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTVOFF);
#अन्यथा
    uint32_t विस्थापनम्_उच्चः = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh);
    uint32_t विस्थापनम्_नीचः = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFlow);
    uint64_t विस्थापनम् = ((uint64_t)विस्थापनम्_उच्चः << 32) | (uint64_t) विस्थापनम्_नीचः;
#यदि_परिभाषितम् CONFIG_VTIMER_UPDATE_VOFFSET
    विस्थापनम् += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (word_t)(विस्थापनम् >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (word_t) विस्थापनम्);
#यदि_अन्तः
    समुच्चयः_cntv_off_64(विस्थापनम्);
#यदि_अन्तः
    /* For verification, need to ensure we don't unmask an inactive interrupt;
     * the virtual timer should never get disabled, but the knowledge is not
     * available at this point */
    /* Restore interrupt mask state */
    यदि (likely(isIRQActive(CORE_IRQ_गन्तव्यम्_IRQT(CURRENT_CPU_सूचकांक(), व्यवधानम्_VTIMER_EVENT)))) {
        आवरणम्_व्यवधानम्(vcpu->vppi_masked[irq_VPPIEvent_सूचकांक(CORE_IRQ_गन्तव्यम्_IRQT(CURRENT_CPU_सूचकांक(), व्यवधानम्_VTIMER_EVENT))],
                      CORE_IRQ_गन्तव्यम्_IRQT(CURRENT_CPU_सूचकांक(), व्यवधानम्_VTIMER_EVENT));
    }
    /* Restore virtual timer control register */
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
}

#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
