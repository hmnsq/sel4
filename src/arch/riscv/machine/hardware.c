/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <machine/timer.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/smp/ipi.h>

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
#परिभाषय RESET_आवृत्तयः ((TIMER_CLOCK_HZ / MS_अन्तर्_S) * CONFIG_TIMER_TICK_MS)
#यदि_अन्तः /* !CONFIG_KERNEL_MCS */

#परिभाषय IS_IRQ_VALID(X) (((X)) <= maxIRQ && (X) != irqInvalid)

word_t विशुद्धम् getRestartPC(tcb_t *thread)
{
    प्रत्यागम get_पञ्जिकाकोशः(thread, FaultIP);
}

शून्य समुच्चयः_अग्र_PC(tcb_t *thread, word_t v)
{
    समुच्चयः_पञ्जिकाकोशः(thread, अग्र_IP, v);
}

BOOT_संहिता शून्य map_kernel_devices(शून्य)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    प्रति (पूर्णाङ्क i = 0; i < अङ्कः_KERNEL_DEVICE_चौकटाः; i++) {
        ध्रुव kernel_frame_t *frame = &kernel_device_चौकटाः[i];
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly);
        यदि (!frame->user_उपलब्धम्) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .अन्तः_2   = frame->paddr + BIT(se_L4_Large_Page_द्वयङ्काः)
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

स्थिर irq_t सक्रिय_irq[CONFIG_MAX_अङ्कः_NODES];


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
स्थिर अन्तःस्थ irq_t get_सक्रिय_IRQ(शून्य)
{
    irq_t *सक्रिय_irq_slot = &सक्रिय_irq[CURRENT_CPU_सूचकांक()];

    /* If an interrupt is currently active, then return it. */
    irq_t irq = *सक्रिय_irq_slot;
    यदि (IS_IRQ_VALID(irq)) {
        प्रत्यागम irq;
    }

    /* No interrupt currently active, find a new one from the sources. The
     * priorities are: external -> software -> timer.
     */
    word_t sip = read_sip();
    यदि (sip & BIT(SIP_SEIP)) {
        /* Even if we say an external interrupt is pending, the PLIC may not
         * return any pending interrupt here in some corner cases. A level
         * triggered interrupt might have been deasserted again or another hard
         * has claimed it in a multicore system.
         */
        irq = plic_get_claim();
#यदि_परिभाषितम् CONFIG_PLAT_QEMU_RISCV_VIRT
        /* QEMU bug requires external interrupts to be immediately claimed. For
         * other platforms, the claim is done in invokeIRQHandler_AckIRQ.
         */
        plic_complete_claim(irq);
#यदि_अन्तः
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    } अन्यथा यदि (sip & BIT(SIP_SSIP)) {
        sbi_clear_ipi();
        irq = ipi_get_irq();
#यदि_अन्तः
    } अन्यथा यदि (sip & BIT(SIP_STIP)) {
        irq = KERNEL_TIMER_IRQ;
    } अन्यथा {
        /* Seems none of the known sources has a pending interrupt. This can
         * happen if e.g. if another hart context has claimed the interrupt
         * already.
         */
        irq = irqInvalid;
    }

    /* There is no guarantee that there is a new interrupt. */
    यदि (!IS_IRQ_VALID(irq)) {
        /* Sanity check: the slot can't hold an interrupt either. */
        प्रतिज्ञा(!IS_IRQ_VALID(*सक्रिय_irq_slot));
        प्रत्यागम irqInvalid;
    }

    /* A new interrupt is active, remember it. */
    *सक्रिय_irq_slot = irq;
    प्रत्यागम irq;
}

#यदि_परिभाषितम् HAVE_समुच्चयः_TRIGGER
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
शून्य समुच्चयः_IRQTrigger(irq_t irq, bool_t edge_triggered)
{
    plic_irq_समुच्चयः_trigger(irq, edge_triggered);
}
#यदि_अन्तः

/* isIRQPending is used to determine whether to preempt long running
 * operations at various preemption points throughout the kernel. If this
 * returns true, it means that if the Kernel were to return to user mode, it
 * would then immediately take an interrupt. We check the SIP register for if
 * either a timer interrupt (STIP) or an external interrupt (SEIP) is pending.
 * We don't check software generated interrupts. These are used to perform cross
 * core signalling which isn't currently supported.
 * TODO: Add SSIP check when SMP support is added.
 */
स्थिर अन्तःस्थ bool_t isIRQPending(शून्य)
{
    word_t sip = read_sip();
    प्रत्यागम (sip & (BIT(SIP_STIP) | BIT(SIP_SEIP)));
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
स्थिर अन्तःस्थ शून्य आवरणम्_व्यवधानम्(bool_t disable, irq_t irq)
{
    प्रतिज्ञा(IS_IRQ_VALID(irq));
    यदि (irq == KERNEL_TIMER_IRQ) {
        यदि (disable) {
            clear_sie_आवरणम्(BIT(SIE_STIE));
        } अन्यथा {
            समुच्चयः_sie_आवरणम्(BIT(SIE_STIE));
        }
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    } अन्यथा यदि (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        प्रत्यागम;
#यदि_अन्तः
    } अन्यथा {
        plic_आवरणम्_irq(disable, irq);
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
स्थिर अन्तःस्थ शून्य ack_व्यवधानम्(irq_t irq)
{
    प्रतिज्ञा(IS_IRQ_VALID(irq));
    सक्रिय_irq[CURRENT_CPU_सूचकांक()] = irqInvalid;

    यदि (irq == KERNEL_TIMER_IRQ) {
        /* Reprogramming the timer has cleared the interrupt. */
        प्रत्यागम;
    }
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    यदि (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        ipi_clear_irq(irq);
    }
#यदि_अन्तः
}

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
शून्य resetTimer(शून्य)
{
    uint64_t target;
    // repeatedly try and set the timer in a loop as otherwise there is a race and we
    // may set a timeout in the past, resulting in it never getting triggered
    कुरु {
        target = riscv_read_time() + RESET_आवृत्तयः;
        sbi_समुच्चयः_timer(target);
    } यावत् (riscv_read_time() > target);
}

/**
   DONT_TRANSLATE
 */
BOOT_संहिता शून्य initTimer(शून्य)
{
    sbi_समुच्चयः_timer(riscv_read_time() + RESET_आवृत्तयः);
}
#यदि_अन्तः /* !CONFIG_KERNEL_MCS */

BOOT_संहिता शून्य initLocalIRQController(शून्य)
{
    प्रारूप_लिख("Init local IRQ\n");

    /* Init per-hart PLIC */
    plic_init_hart();

    /* Enable timer and external interrupt. If SMP is enabled, then enable the
     * software interrupt also, it is used as IPI between cores. */
    समुच्चयः_sie_आवरणम्(BIT(SIE_SEIE) | BIT(SIE_STIE) | SMP_TERNARY(BIT(SIE_SSIE), 0));
}

BOOT_संहिता शून्य initIRQController(शून्य)
{
    प्रारूप_लिख("Initializing PLIC...\n");

    /* Initialize active_irq[] properly to stick to the semantics and play safe.
     * Effectively this is not needed if irqInvalid is zero (which is currently
     * the case) and the array is in the BSS, that is filled with zeros (which
     * the a kernel loader is supposed to do and which the ELF-Loader does).
     */
    प्रति (word_t i = 0; i < ARRAY_आकार(सक्रिय_irq); i++) {
        सक्रिय_irq[i] = irqInvalid;
    }

    plic_init_controller();
}

स्थिर अन्तःस्थ शून्य handleSpuriousIRQ(शून्य)
{
    /* Do nothing */
}
