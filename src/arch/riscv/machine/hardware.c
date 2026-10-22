/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/smp/ipi.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RESET_ਚੱਕਰਾਂ ((TIMER_CLOCK_HZ / MS_ਵਿੱਚ_S) * CONFIG_TIMER_TICK_MS)
#ਜੇ_ਅੰਤ /* !CONFIG_KERNEL_MCS */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_IRQ_VALID(X) (((X)) <= maxIRQ && (X) != irqInvalid)

ਸ਼ਬਦ_t ਸ਼ੁੱਧ getRestartPC(tcb_t *thread)
{
    ਵਾਪਸ get_ਰਜਿਸਟਰ(thread, FaultIP);
}

ਖਾਲੀ ਸੈੱਟ_ਅਗਲਾ_PC(tcb_t *thread, ਸ਼ਬਦ_t v)
{
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਅਗਲਾ_IP, v);
}

BOOT_ਕੋਡ ਖਾਲੀ map_kernel_devices(ਖਾਲੀ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_KERNEL_DEVICE_ਚੌਖਟੇ; i++) {
        ਅਚਲ kernel_ਚੌਖਟਾ_t *ਚੌਖਟਾ = &kernel_device_ਚੌਖਟੇ[i];
        map_kernel_ਚੌਖਟਾ(ਚੌਖਟਾ->paddr, ਚੌਖਟਾ->pptr, VMKernelOnly);
        ਜੇ (!ਚੌਖਟਾ->user_ਉਪਲਬਧ) {
            reserve_region((p_region_t) {
                .ਸ਼ੁਰੂ = ਚੌਖਟਾ->paddr,
                .ਸਿਰਾ   = ਚੌਖਟਾ->paddr + ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Large_ਪੰਨਾ_ਬਿੱਟ)
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

ਸਥਿਰ irq_t ਸਰਗਰਮ_irq[CONFIG_MAX_ਅੰਕ_NODES];


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
ਸਥਿਰ ਸਤਰਅੰਦਰ irq_t get_ਸਰਗਰਮ_IRQ(ਖਾਲੀ)
{
    irq_t *ਸਰਗਰਮ_irq_ਥਾਂ = &ਸਰਗਰਮ_irq[CURRENT_CPU_ਸੂਚਕਾਂਕ()];

    /* If an interrupt is currently active, then return it. */
    irq_t irq = *ਸਰਗਰਮ_irq_ਥਾਂ;
    ਜੇ (IS_IRQ_VALID(irq)) {
        ਵਾਪਸ irq;
    }

    /* No interrupt currently active, find a new one from the sources. The
     * priorities are: external -> software -> timer.
     */
    ਸ਼ਬਦ_t sip = read_sip();
    ਜੇ (sip & ਦੋਆਧਾਰੀ_ਅੰਕ(SIP_SEIP)) {
        /* Even if we say an external interrupt is pending, the PLIC may not
         * return any pending interrupt here in some corner cases. A level
         * triggered interrupt might have been deasserted again or another hard
         * has claimed it in a multicore system.
         */
        irq = plic_get_claim();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PLAT_QEMU_RISCV_VIRT
        /* QEMU bug requires external interrupts to be immediately claimed. For
         * other platforms, the claim is done in invokeIRQHandler_AckIRQ.
         */
        plic_complete_claim(irq);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    } ਨਹੀਂਤਾਂ ਜੇ (sip & ਦੋਆਧਾਰੀ_ਅੰਕ(SIP_SSIP)) {
        sbi_clear_ipi();
        irq = ipi_get_irq();
#ਜੇ_ਅੰਤ
    } ਨਹੀਂਤਾਂ ਜੇ (sip & ਦੋਆਧਾਰੀ_ਅੰਕ(SIP_STIP)) {
        irq = KERNEL_TIMER_IRQ;
    } ਨਹੀਂਤਾਂ {
        /* Seems none of the known sources has a pending interrupt. This can
         * happen if e.g. if another hart context has claimed the interrupt
         * already.
         */
        irq = irqInvalid;
    }

    /* There is no guarantee that there is a new interrupt. */
    ਜੇ (!IS_IRQ_VALID(irq)) {
        /* Sanity check: the slot can't hold an interrupt either. */
        ਪ੍ਰਤਿਜ੍ਞਾ(!IS_IRQ_VALID(*ਸਰਗਰਮ_irq_ਥਾਂ));
        ਵਾਪਸ irqInvalid;
    }

    /* A new interrupt is active, remember it. */
    *ਸਰਗਰਮ_irq_ਥਾਂ = irq;
    ਵਾਪਸ irq;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਸੈੱਟ_TRIGGER
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
ਖਾਲੀ ਸੈੱਟ_IRQTrigger(irq_t irq, bool_t ਕਿਨਾਰਾ_triggered)
{
    plic_irq_ਸੈੱਟ_trigger(irq, ਕਿਨਾਰਾ_triggered);
}
#ਜੇ_ਅੰਤ

/* isIRQPending is used to determine whether to preempt long running
 * operations at various preemption points throughout the kernel. If this
 * returns true, it means that if the Kernel were to return to user mode, it
 * would then immediately take an interrupt. We check the SIP register for if
 * either a timer interrupt (STIP) or an external interrupt (SEIP) is pending.
 * We don't check software generated interrupts. These are used to perform cross
 * core signalling which isn't currently supported.
 * TODO: Add SSIP check when SMP support is added.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t isIRQPending(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t sip = read_sip();
    ਵਾਪਸ (sip & (ਦੋਆਧਾਰੀ_ਅੰਕ(SIP_STIP) | ਦੋਆਧਾਰੀ_ਅੰਕ(SIP_SEIP)));
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
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਨਕਾਬ_ਰੁਕਾਵਟ(bool_t disable, irq_t irq)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_IRQ_VALID(irq));
    ਜੇ (irq == KERNEL_TIMER_IRQ) {
        ਜੇ (disable) {
            clear_sie_ਨਕਾਬ(ਦੋਆਧਾਰੀ_ਅੰਕ(SIE_STIE));
        } ਨਹੀਂਤਾਂ {
            ਸੈੱਟ_sie_ਨਕਾਬ(ਦੋਆਧਾਰੀ_ਅੰਕ(SIE_STIE));
        }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    } ਨਹੀਂਤਾਂ ਜੇ (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        ਵਾਪਸ;
#ਜੇ_ਅੰਤ
    } ਨਹੀਂਤਾਂ {
        plic_ਨਕਾਬ_irq(disable, irq);
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
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ack_ਰੁਕਾਵਟ(irq_t irq)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_IRQ_VALID(irq));
    ਸਰਗਰਮ_irq[CURRENT_CPU_ਸੂਚਕਾਂਕ()] = irqInvalid;

    ਜੇ (irq == KERNEL_TIMER_IRQ) {
        /* Reprogramming the timer has cleared the interrupt. */
        ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਜੇ (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        ipi_clear_irq(irq);
    }
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ resetTimer(ਖਾਲੀ)
{
    uint64_t ਨਿਸ਼ਾਨਾ;
    // repeatedly try and set the timer in a loop as otherwise there is a race and we
    // may set a timeout in the past, resulting in it never getting triggered
    ਕਰੋ {
        ਨਿਸ਼ਾਨਾ = riscv_read_ਸਮਾਂ() + RESET_ਚੱਕਰਾਂ;
        sbi_ਸੈੱਟ_timer(ਨਿਸ਼ਾਨਾ);
    } ਜਦੋਂ (riscv_read_ਸਮਾਂ() > ਨਿਸ਼ਾਨਾ);
}

/**
   DONT_TRANSLATE
 */
BOOT_ਕੋਡ ਖਾਲੀ initTimer(ਖਾਲੀ)
{
    sbi_ਸੈੱਟ_timer(riscv_read_ਸਮਾਂ() + RESET_ਚੱਕਰਾਂ);
}
#ਜੇ_ਅੰਤ /* !CONFIG_KERNEL_MCS */

BOOT_ਕੋਡ ਖਾਲੀ initLocalIRQController(ਖਾਲੀ)
{
    ਲਿਖੋ("Init local IRQ\n");

    /* Init per-hart PLIC */
    plic_init_hart();

    /* Enable timer and external interrupt. If SMP is enabled, then enable the
     * software interrupt also, it is used as IPI between cores. */
    ਸੈੱਟ_sie_ਨਕਾਬ(ਦੋਆਧਾਰੀ_ਅੰਕ(SIE_SEIE) | ਦੋਆਧਾਰੀ_ਅੰਕ(SIE_STIE) | SMP_TERNARY(ਦੋਆਧਾਰੀ_ਅੰਕ(SIE_SSIE), 0));
}

BOOT_ਕੋਡ ਖਾਲੀ initIRQController(ਖਾਲੀ)
{
    ਲਿਖੋ("Initializing PLIC...\n");

    /* Initialize active_irq[] properly to stick to the semantics and play safe.
     * Effectively this is not needed if irqInvalid is zero (which is currently
     * the case) and the array is in the BSS, that is filled with zeros (which
     * the a kernel loader is supposed to do and which the ELF-Loader does).
     */
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ARRAY_ਮਾਪ(ਸਰਗਰਮ_irq); i++) {
        ਸਰਗਰਮ_irq[i] = irqInvalid;
    }

    plic_init_controller();
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ handleSpuriousIRQ(ਖਾਲੀ)
{
    /* Do nothing */
}
