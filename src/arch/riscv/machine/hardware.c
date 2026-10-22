/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <machine/registerset.h>
#समावेश <machine/timer.h>
#समावेश <arch/machine.h>
#समावेश <arch/smp/ipi.h>

#अगर_अघोषित CONFIG_KERNEL_MCS
#घोषणा RESET_CYCLES ((TIMER_CLOCK_HZ / MS_IN_S) * CONFIG_TIMER_TICK_MS)
#पूर्ण_अगर /* !CONFIG_KERNEL_MCS */

#घोषणा IS_IRQ_VALID(X) (((X)) <= maxIRQ && (X) != irqInvalid)

word_t PURE getRestartPC(tcb_t *thread)
{
    वापस getRegister(thread, FaultIP);
}

व्योम set_अग्र_PC(tcb_t *thread, word_t v)
{
    setRegister(thread, अग्र_IP, v);
}

BOOT_CODE व्योम map_kernel_devices(व्योम)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    क्रम (पूर्णांक i = 0; i < अंक_KERNEL_DEVICE_FRAMES; i++) {
        स्थिर kernel_frame_t *frame = &kernel_device_frames[i];
        map_kernel_frame(frame->paddr, frame->pptr, VMKernelOnly);
        अगर (!frame->userAvailable) {
            reserve_region((p_region_t) {
                .start = frame->paddr,
                .end   = frame->paddr + BIT(seL4_LargePageBits)
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

अटल irq_t active_irq[CONFIG_MAX_अंक_NODES];


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
अटल अंतरभूत irq_t getActiveIRQ(व्योम)
{
    irq_t *active_irq_slot = &active_irq[CURRENT_CPU_सूचकांक()];

    /* If an interrupt is currently active, then return it. */
    irq_t irq = *active_irq_slot;
    अगर (IS_IRQ_VALID(irq)) {
        वापस irq;
    }

    /* No interrupt currently active, find a new one from the sources. The
     * priorities are: external -> software -> timer.
     */
    word_t sip = read_sip();
    अगर (sip & BIT(SIP_SEIP)) {
        /* Even if we say an external interrupt is pending, the PLIC may not
         * return any pending interrupt here in some corner cases. A level
         * triggered interrupt might have been deasserted again or another hard
         * has claimed it in a multicore system.
         */
        irq = plic_get_claim();
#अगर_घोषित CONFIG_PLAT_QEMU_RISCV_VIRT
        /* QEMU bug requires external interrupts to be immediately claimed. For
         * other platforms, the claim is done in invokeIRQHandler_AckIRQ.
         */
        plic_complete_claim(irq);
#पूर्ण_अगर
#अगर_घोषित ENABLE_SMP_SUPPORT
    } अन्यथा अगर (sip & BIT(SIP_SSIP)) {
        sbi_clear_ipi();
        irq = ipi_get_irq();
#पूर्ण_अगर
    } अन्यथा अगर (sip & BIT(SIP_STIP)) {
        irq = KERNEL_TIMER_IRQ;
    } अन्यथा {
        /* Seems none of the known sources has a pending interrupt. This can
         * happen if e.g. if another hart context has claimed the interrupt
         * already.
         */
        irq = irqInvalid;
    }

    /* There is no guarantee that there is a new interrupt. */
    अगर (!IS_IRQ_VALID(irq)) {
        /* Sanity check: the slot can't hold an interrupt either. */
        निश्चित(!IS_IRQ_VALID(*active_irq_slot));
        वापस irqInvalid;
    }

    /* A new interrupt is active, remember it. */
    *active_irq_slot = irq;
    वापस irq;
}

#अगर_घोषित HAVE_SET_TRIGGER
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
व्योम setIRQTrigger(irq_t irq, bool_t edge_triggered)
{
    plic_irq_set_trigger(irq, edge_triggered);
}
#पूर्ण_अगर

/* isIRQPending is used to determine whether to preempt long running
 * operations at various preemption points throughout the kernel. If this
 * returns true, it means that if the Kernel were to return to user mode, it
 * would then immediately take an interrupt. We check the SIP register for if
 * either a timer interrupt (STIP) or an external interrupt (SEIP) is pending.
 * We don't check software generated interrupts. These are used to perform cross
 * core signalling which isn't currently supported.
 * TODO: Add SSIP check when SMP support is added.
 */
अटल अंतरभूत bool_t isIRQPending(व्योम)
{
    word_t sip = read_sip();
    वापस (sip & (BIT(SIP_STIP) | BIT(SIP_SEIP)));
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
अटल अंतरभूत व्योम maskInterrupt(bool_t disable, irq_t irq)
{
    निश्चित(IS_IRQ_VALID(irq));
    अगर (irq == KERNEL_TIMER_IRQ) {
        अगर (disable) {
            clear_sie_mask(BIT(SIE_STIE));
        } अन्यथा {
            set_sie_mask(BIT(SIE_STIE));
        }
#अगर_घोषित ENABLE_SMP_SUPPORT
    } अन्यथा अगर (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        वापस;
#पूर्ण_अगर
    } अन्यथा {
        plic_mask_irq(disable, irq);
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
अटल अंतरभूत व्योम ackInterrupt(irq_t irq)
{
    निश्चित(IS_IRQ_VALID(irq));
    active_irq[CURRENT_CPU_सूचकांक()] = irqInvalid;

    अगर (irq == KERNEL_TIMER_IRQ) {
        /* Reprogramming the timer has cleared the interrupt. */
        वापस;
    }
#अगर_घोषित ENABLE_SMP_SUPPORT
    अगर (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        ipi_clear_irq(irq);
    }
#पूर्ण_अगर
}

#अगर_अघोषित CONFIG_KERNEL_MCS
व्योम resetTimer(व्योम)
{
    uint64_t target;
    // repeatedly try and set the timer in a loop as otherwise there is a race and we
    // may set a timeout in the past, resulting in it never getting triggered
    करो {
        target = riscv_read_time() + RESET_CYCLES;
        sbi_set_timer(target);
    } जबतक (riscv_read_time() > target);
}

/**
   DONT_TRANSLATE
 */
BOOT_CODE व्योम initTimer(व्योम)
{
    sbi_set_timer(riscv_read_time() + RESET_CYCLES);
}
#पूर्ण_अगर /* !CONFIG_KERNEL_MCS */

BOOT_CODE व्योम initLocalIRQController(व्योम)
{
    म_लिखो("Init local IRQ\n");

    /* Init per-hart PLIC */
    plic_init_hart();

    /* Enable timer and external interrupt. If SMP is enabled, then enable the
     * software interrupt also, it is used as IPI between cores. */
    set_sie_mask(BIT(SIE_SEIE) | BIT(SIE_STIE) | SMP_TERNARY(BIT(SIE_SSIE), 0));
}

BOOT_CODE व्योम initIRQController(व्योम)
{
    म_लिखो("Initializing PLIC...\n");

    /* Initialize active_irq[] properly to stick to the semantics and play safe.
     * Effectively this is not needed if irqInvalid is zero (which is currently
     * the case) and the array is in the BSS, that is filled with zeros (which
     * the a kernel loader is supposed to do and which the ELF-Loader does).
     */
    क्रम (word_t i = 0; i < ARRAY_आकार(active_irq); i++) {
        active_irq[i] = irqInvalid;
    }

    plic_init_controller();
}

अटल अंतरभूत व्योम handleSpuriousIRQ(व्योम)
{
    /* Do nothing */
}
