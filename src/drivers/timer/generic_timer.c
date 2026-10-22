/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <drivers/timer/arm_generic.h>

BOOT_సంకేతం శూన్య initGenericTimer(శూన్య)
{
    ఒకవేళ (config_సమితి(CONFIG_DEBUG_BUILD)) {
        /* check the frequency is correct */
        మాట_t gpt_cntfrq = 0;
        SYSTEM_READ_మాట(CNTFRQ, gpt_cntfrq);
        /* The CNTFRQ register is a 32-bit register, its value can safely be
         * compared with TIMER_CLOCK_HZ.
         */
        ఒకవేళ ((gpt_cntfrq != 0) && (gpt_cntfrq != TIMER_CLOCK_HZ)) {
            /* TIMER_CLOCK_HZ is defined as a unsigned long long constant on
             * every architecture. */
            రాయి("Warning:  gpt_cntfrq %"SEL4_PRIu_మాట", expected %llu\n",
                   gpt_cntfrq, TIMER_CLOCK_HZ);
        }
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* this sets the irq to UINT64_MAX */
    ackDeadlineIRQ();
#లేకపోతే /* CONFIG_KERNEL_MCS */
    resetTimer();
#అయితే_ముగింపు /* !CONFIG_KERNEL_MCS */
    SYSTEM_WRITE_మాట(CNT_CTL, బిట్టు(0));
}

/*
 * The exynos5 platforms require custom hardware initialisation before the
 * generic timer is usable. They need to overwrite initTimer before calling
 * initGenericTimer because of this. We cannot use a `weak` symbol definition
 * in this case because the kernel is built as a single file and multiple
 * symbol definitions with the same name are not allowed. We therefore resort
 * to ifdef'ing out this initTimer definition for exynos5 platforms.
 */
#నిర్వచితం_కాకపోతే CONFIG_PLAT_EXYNOS5
BOOT_సంకేతం శూన్య initTimer(శూన్య)
{
    initGenericTimer();
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT

#అన్తర్భావ <arch/object/vcpu.h>
#అన్తర్భావ <armv/vcpu.h>

/** MODIFIES: */
/** DONT_TRANSLATE */
స్థిర అంతఃవరుస uint64_t read_cntpct(శూన్య)
{
    uint64_t val;
    SYSTEM_READ_64(CNTPCT, val);
    తిరిగి val;
}

స్థిర శూన్య save_virt_timer(vcpu_t *vcpu)
{
    /* Save control register */
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
    vcpu_hw_write_reg(seL4_VCPUReg_CNTV_CTL, 0);
    /* Save Compare Value and Offset registers */
#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#లేకపోతే
    uint64_t cval = get_cntv_cval_64();
    uint64_t cntvoff = get_cntv_off_64();
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh, (మాట_t)(cval >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow, (మాట_t)cval);
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (మాట_t)(cntvoff >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (మాట_t)cntvoff);
    vcpu_save_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#అయితే_ముగింపు
    check_export_arch_timer();
#నిర్వచితమైతే CONFIG_VTIMER_UPDATE_VOFFSET
    /* Save counter value at the time the vcpu is disabled */
    vcpu->virtTimer.చివరి_pcount = read_cntpct();
#అయితే_ముగింపు
}

స్థిర శూన్య restore_virt_timer(vcpu_t *vcpu)
{
    /* Restore virtual timer state */
#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CVAL);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL_EL1);
#లేకపోతే
    uint32_t cval_ఎగువ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALhigh);
    uint32_t cval_దిగువ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTV_CVALlow);
    uint64_t cval = ((uint64_t)cval_ఎగువ << 32) | (uint64_t) cval_దిగువ;
    సమితి_cntv_cval_64(cval);
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTKCTL);
#అయితే_ముగింపు

    /* Set virtual timer offset */
#నిర్వచితమైతే CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t pcount_delta;
    uint64_t current_cntpct = read_cntpct();
    pcount_delta = current_cntpct - vcpu->virtTimer.చివరి_pcount;
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_ARCH_AARCH64
#నిర్వచితమైతే CONFIG_VTIMER_UPDATE_VOFFSET
    uint64_t స్థానభ్రంశం = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFF);
    స్థానభ్రంశం += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFF, స్థానభ్రంశం);
#అయితే_ముగింపు
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTVOFF);
#లేకపోతే
    uint32_t స్థానభ్రంశం_ఎగువ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh);
    uint32_t స్థానభ్రంశం_దిగువ = vcpu_read_reg(vcpu, seL4_VCPUReg_CNTVOFFlow);
    uint64_t స్థానభ్రంశం = ((uint64_t)స్థానభ్రంశం_ఎగువ << 32) | (uint64_t) స్థానభ్రంశం_దిగువ;
#నిర్వచితమైతే CONFIG_VTIMER_UPDATE_VOFFSET
    స్థానభ్రంశం += pcount_delta;
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFhigh, (మాట_t)(స్థానభ్రంశం >> 32));
    vcpu_write_reg(vcpu, seL4_VCPUReg_CNTVOFFlow, (మాట_t) స్థానభ్రంశం);
#అయితే_ముగింపు
    సమితి_cntv_off_64(స్థానభ్రంశం);
#అయితే_ముగింపు
    /* For verification, need to ensure we don't unmask an inactive interrupt;
     * the virtual timer should never get disabled, but the knowledge is not
     * available at this point */
    /* Restore interrupt mask state */
    ఒకవేళ (likely(isIRQActive(CORE_IRQ_గమ్యం_IRQT(CURRENT_CPU_సూచీ(), అంతరాయం_VTIMER_EVENT)))) {
        ముసుగు_అంతరాయం(vcpu->vppi_masked[irq_VPPIEvent_సూచీ(CORE_IRQ_గమ్యం_IRQT(CURRENT_CPU_సూచీ(), అంతరాయం_VTIMER_EVENT))],
                      CORE_IRQ_గమ్యం_IRQT(CURRENT_CPU_సూచీ(), అంతరాయం_VTIMER_EVENT));
    }
    /* Restore virtual timer control register */
    vcpu_restore_reg(vcpu, seL4_VCPUReg_CNTV_CTL);
}

#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
