/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <machine/registerset.h>
#అన్తర్భావ <machine/timer.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/smp/ipi.h>

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
#నిర్వచించు RESET_చక్రాలు ((TIMER_CLOCK_HZ / MS_లోపల_S) * CONFIG_TIMER_TICK_MS)
#అయితే_ముగింపు /* !CONFIG_KERNEL_MCS */

#నిర్వచించు IS_IRQ_VALID(X) (((X)) <= maxIRQ && (X) != irqInvalid)

మాట_t శుద్ధ getRestartPC(tcb_t *thread)
{
    తిరిగి get_రిజిస్టరు(thread, FaultIP);
}

శూన్య సమితి_తదుపరి_PC(tcb_t *thread, మాట_t v)
{
    సమితి_రిజిస్టరు(thread, తదుపరి_IP, v);
}

BOOT_సంకేతం శూన్య map_kernel_devices(శూన్య)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    కొరకు (పూర్ణాంకం i = 0; i < అంకె_KERNEL_DEVICE_చట్రాలు; i++) {
        స్థిరరాశి kernel_చట్రం_t *చట్రం = &kernel_device_చట్రాలు[i];
        map_kernel_చట్రం(చట్రం->paddr, చట్రం->pptr, VMKernelOnly);
        ఒకవేళ (!చట్రం->user_అందుబాటు) {
            reserve_region((p_region_t) {
                .ప్రారంభం = చట్రం->paddr,
                .చివర   = చట్రం->paddr + బిట్టు(se_L4_Large_పుట_బిట్లు)
            });
        }
    }
}

/*
 * The following assumes familiarity with RISC-V interrupt delivery and the PLIC.
 * See the RISC-V privileged specification v1.10 and the comment in
 * include/plat/spike/plat/machine.h for more information.
 * RISC-V IRQ handling on seL4 works as follows:
 *
 * On other architectures the kernel masks interrupts between delivering them to
 * userlevel and receiving the acknowledgment invocation. This strategy doesn't
 * work on RISC-V as an IRQ is implicitly masked when it is claimed, until the
 * claim is acknowledged. If we mask and unmask the interrupt at the PLIC while
 * a claim is in progress we sometimes experience IRQ sources not being masked
 * and unmasked as expected. Because of this, we don't mask and unmask IRQs that
 * are for user level, and also call plic_complete_claim for seL4_IRQHandler_Ack.
 */

స్థిర irq_t చురుకైన_irq[CONFIG_MAX_అంకె_NODES];


/**
 * Gets the active irq. Returns the same irq if called again before ackInterrupt.
 *
 * This function is called by the kernel to get the interrupt that is currently
 * active. It not interrupt is currently active, it will try to find one and
 * put it in the active state. If no interrupt is found, irqInvalid is returned.
 * It can't be assumed that if isIRQPending() returned true, there will always
 * be an active interrupt then this is called. It may hold in mayn cases, but
 * there are corner cases with level-triggered interrupts or on multicore
 * systems.
 * This function can be called multiple times during one kernel entry. It must
 * guarantee that once one interrupt is reported as active, this interrupt is
 * always returned until ackInterrupt() is called eventually.
 *
 * @return     The active irq or irqInvalid.
 */
స్థిర అంతఃవరుస irq_t get_చురుకైన_IRQ(శూన్య)
{
    irq_t *చురుకైన_irq_స్థానం = &చురుకైన_irq[CURRENT_CPU_సూచీ()];

    /* If an interrupt is currently active, then return it. */
    irq_t irq = *చురుకైన_irq_స్థానం;
    ఒకవేళ (IS_IRQ_VALID(irq)) {
        తిరిగి irq;
    }

    /* No interrupt currently active, find a new one from the sources. The
     * priorities are: external -> software -> timer.
     */
    మాట_t sip = read_sip();
    ఒకవేళ (sip & బిట్టు(SIP_SEIP)) {
        /* Even if we say an external interrupt is pending, the PLIC may not
         * return any pending interrupt here in some corner cases. A level
         * triggered interrupt might have been deasserted again or another hard
         * has claimed it in a multicore system.
         */
        irq = plic_get_claim();
#నిర్వచితమైతే CONFIG_PLAT_QEMU_RISCV_VIRT
        /* QEMU bug requires external interrupts to be immediately claimed. For
         * other platforms, the claim is done in invokeIRQHandler_AckIRQ.
         */
        plic_complete_claim(irq);
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    } లేకపోతే ఒకవేళ (sip & బిట్టు(SIP_SSIP)) {
        sbi_clear_ipi();
        irq = ipi_get_irq();
#అయితే_ముగింపు
    } లేకపోతే ఒకవేళ (sip & బిట్టు(SIP_STIP)) {
        irq = KERNEL_TIMER_IRQ;
    } లేకపోతే {
        /* Seems none of the known sources has a pending interrupt. This can
         * happen if e.g. if another hart context has claimed the interrupt
         * already.
         */
        irq = irqInvalid;
    }

    /* There is no guarantee that there is a new interrupt. */
    ఒకవేళ (!IS_IRQ_VALID(irq)) {
        /* Sanity check: the slot can't hold an interrupt either. */
        ప్రతిజ్ఞా(!IS_IRQ_VALID(*చురుకైన_irq_స్థానం));
        తిరిగి irqInvalid;
    }

    /* A new interrupt is active, remember it. */
    *చురుకైన_irq_స్థానం = irq;
    తిరిగి irq;
}

#నిర్వచితమైతే HAVE_సమితి_TRIGGER
/**
 * Sets the irq trigger.
 *
 * setIRQTrigger can change the trigger between edge and level at the PLIC for
 * external interrupts. It is implementation specific as whether the PLIC has
 * support for this operation.
 *
 * @param[in]  irq             The irq
 * @param[in]  edge_triggered  edge triggered otherwise level triggered
 */
శూన్య సమితి_IRQTrigger(irq_t irq, bool_t అంచు_triggered)
{
    plic_irq_సమితి_trigger(irq, అంచు_triggered);
}
#అయితే_ముగింపు

/* isIRQPending is used to determine whether to preempt long running
 * operations at various preemption points throughout the kernel. If this
 * returns true, it means that if the Kernel were to return to user mode, it
 * would then immediately take an interrupt. We check the SIP register for if
 * either a timer interrupt (STIP) or an external interrupt (SEIP) is pending.
 * We don't check software generated interrupts. These are used to perform cross
 * core signalling which isn't currently supported.
 * TODO: Add SSIP check when SMP support is added.
 */
స్థిర అంతఃవరుస bool_t isIRQPending(శూన్య)
{
    మాట_t sip = read_sip();
    తిరిగి (sip & (బిట్టు(SIP_STIP) | బిట్టు(SIP_SEIP)));
}

/**
 * Disable or enable IRQs.
 *
 * maskInterrupt disables and enables IRQs. When an IRQ is disabled, it should
 * not raise an interrupt on the Kernel's HART context. This either masks the
 * core timer on the sie register or masks an external IRQ at the plic.
 *
 * @param[in]  disable  The disable
 * @param[in]  irq      The irq
 */
స్థిర అంతఃవరుస శూన్య ముసుగు_అంతరాయం(bool_t disable, irq_t irq)
{
    ప్రతిజ్ఞా(IS_IRQ_VALID(irq));
    ఒకవేళ (irq == KERNEL_TIMER_IRQ) {
        ఒకవేళ (disable) {
            clear_sie_ముసుగు(బిట్టు(SIE_STIE));
        } లేకపోతే {
            సమితి_sie_ముసుగు(బిట్టు(SIE_STIE));
        }
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    } లేకపోతే ఒకవేళ (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        తిరిగి;
#అయితే_ముగింపు
    } లేకపోతే {
        plic_ముసుగు_irq(disable, irq);
    }
}

/**
 * Kernel has dealt with the pending interrupt getActiveIRQ can return next IRQ.
 *
 * ackInterrupt is used by the kernel to indicate it has processed the interrupt
 * delivery and getActiveIRQ is now able to return a different IRQ number. Note
 * that this is called after a notification has been signalled to user level,
 * but before user level has handled the cause.
 *
 * @param[in]  irq   The irq
 */
స్థిర అంతఃవరుస శూన్య ack_అంతరాయం(irq_t irq)
{
    ప్రతిజ్ఞా(IS_IRQ_VALID(irq));
    చురుకైన_irq[CURRENT_CPU_సూచీ()] = irqInvalid;

    ఒకవేళ (irq == KERNEL_TIMER_IRQ) {
        /* Reprogramming the timer has cleared the interrupt. */
        తిరిగి;
    }
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    ఒకవేళ (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        ipi_clear_irq(irq);
    }
#అయితే_ముగింపు
}

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
శూన్య resetTimer(శూన్య)
{
    uint64_t లక్ష్యం;
    // repeatedly try and set the timer in a loop as otherwise there is a race and we
    // may set a timeout in the past, resulting in it never getting triggered
    చేయి {
        లక్ష్యం = riscv_read_సమయం() + RESET_చక్రాలు;
        sbi_సమితి_timer(లక్ష్యం);
    } అయితే (riscv_read_సమయం() > లక్ష్యం);
}

/**
   DONT_TRANSLATE
 */
BOOT_సంకేతం శూన్య initTimer(శూన్య)
{
    sbi_సమితి_timer(riscv_read_సమయం() + RESET_చక్రాలు);
}
#అయితే_ముగింపు /* !CONFIG_KERNEL_MCS */

BOOT_సంకేతం శూన్య initLocalIRQController(శూన్య)
{
    రాయి("Init local IRQ\n");

    /* Init per-hart PLIC */
    plic_init_hart();

    /* Enable timer and external interrupt. If SMP is enabled, then enable the
     * software interrupt also, it is used as IPI between cores. */
    సమితి_sie_ముసుగు(బిట్టు(SIE_SEIE) | బిట్టు(SIE_STIE) | SMP_TERNARY(బిట్టు(SIE_SSIE), 0));
}

BOOT_సంకేతం శూన్య initIRQController(శూన్య)
{
    రాయి("Initializing PLIC...\n");

    /* Initialize active_irq[] properly to stick to the semantics and play safe.
     * Effectively this is not needed if irqInvalid is zero (which is currently
     * the case) and the array is in the BSS, that is filled with zeros (which
     * the a kernel loader is supposed to do and which the ELF-Loader does).
     */
    కొరకు (మాట_t i = 0; i < ARRAY_కొలత(చురుకైన_irq); i++) {
        చురుకైన_irq[i] = irqInvalid;
    }

    plic_init_controller();
}

స్థిర అంతఃవరుస శూన్య handleSpuriousIRQ(శూన్య)
{
    /* Do nothing */
}
