/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <machine/timer.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/smp/ipi.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
#ವ್ಯಾಖ್ಯಾನಿಸು RESET_ಚಕ್ರಗಳು ((TIMER_CLOCK_HZ / MS_ಒಳ_S) * CONFIG_TIMER_TICK_MS)
#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_KERNEL_MCS */

#ವ್ಯಾಖ್ಯಾನಿಸು IS_IRQ_VALID(X) (((X)) <= maxIRQ && (X) != irqInvalid)

ಶಬ್ದ_t ಶುದ್ಧ getRestartPC(tcb_t *thread)
{
    ಹಿಂತಿರುಗಿ get_ರಿಜಿಸ್ಟರ್(thread, FaultIP);
}

ಶೂನ್ಯ ಗಣ_ಮುಂದಿನ_PC(tcb_t *thread, ಶಬ್ದ_t v)
{
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಮುಂದಿನ_IP, v);
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_kernel_devices(ಶೂನ್ಯ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_KERNEL_DEVICE_ಚೌಕಟ್ಟುಗಳು; i++) {
        ಸ್ಥಿರಾಂಕ kernel_ಚೌಕಟ್ಟು_t *ಚೌಕಟ್ಟು = &kernel_device_ಚೌಕಟ್ಟುಗಳು[i];
        map_kernel_ಚೌಕಟ್ಟು(ಚೌಕಟ್ಟು->paddr, ಚೌಕಟ್ಟು->pptr, VMKernelOnly);
        ಆದರೆ (!ಚೌಕಟ್ಟು->user_ಲಭ್ಯ) {
            reserve_region((p_region_t) {
                .ಆರಂಭ = ಚೌಕಟ್ಟು->paddr,
                .ಕೊನೆ   = ಚೌಕಟ್ಟು->paddr + ಬಿಟ್(se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು)
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

ಸ್ಥಿರ irq_t ಸಕ್ರಿಯ_irq[CONFIG_MAX_ಅಂಕೆ_NODES];


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
ಸ್ಥಿರ ಸಾಲೊಳಗೆ irq_t get_ಸಕ್ರಿಯ_IRQ(ಶೂನ್ಯ)
{
    irq_t *ಸಕ್ರಿಯ_irq_ಸ್ಥಾನ = &ಸಕ್ರಿಯ_irq[CURRENT_CPU_ಸೂಚ್ಯಂಕ()];

    /* If an interrupt is currently active, then return it. */
    irq_t irq = *ಸಕ್ರಿಯ_irq_ಸ್ಥಾನ;
    ಆದರೆ (IS_IRQ_VALID(irq)) {
        ಹಿಂತಿರುಗಿ irq;
    }

    /* No interrupt currently active, find a new one from the sources. The
     * priorities are: external -> software -> timer.
     */
    ಶಬ್ದ_t sip = read_sip();
    ಆದರೆ (sip & ಬಿಟ್(SIP_SEIP)) {
        /* Even if we say an external interrupt is pending, the PLIC may not
         * return any pending interrupt here in some corner cases. A level
         * triggered interrupt might have been deasserted again or another hard
         * has claimed it in a multicore system.
         */
        irq = plic_get_claim();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PLAT_QEMU_RISCV_VIRT
        /* QEMU bug requires external interrupts to be immediately claimed. For
         * other platforms, the claim is done in invokeIRQHandler_AckIRQ.
         */
        plic_complete_claim(irq);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sip & ಬಿಟ್(SIP_SSIP)) {
        sbi_clear_ipi();
        irq = ipi_get_irq();
#ಆದರೆ_ಅಂತ್ಯ
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sip & ಬಿಟ್(SIP_STIP)) {
        irq = KERNEL_TIMER_IRQ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Seems none of the known sources has a pending interrupt. This can
         * happen if e.g. if another hart context has claimed the interrupt
         * already.
         */
        irq = irqInvalid;
    }

    /* There is no guarantee that there is a new interrupt. */
    ಆದರೆ (!IS_IRQ_VALID(irq)) {
        /* Sanity check: the slot can't hold an interrupt either. */
        ಪ್ರತಿಜ್ಞಾ(!IS_IRQ_VALID(*ಸಕ್ರಿಯ_irq_ಸ್ಥಾನ));
        ಹಿಂತಿರುಗಿ irqInvalid;
    }

    /* A new interrupt is active, remember it. */
    *ಸಕ್ರಿಯ_irq_ಸ್ಥಾನ = irq;
    ಹಿಂತಿರುಗಿ irq;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ಗಣ_TRIGGER
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
ಶೂನ್ಯ ಗಣ_IRQTrigger(irq_t irq, bool_t ಅಂಚು_triggered)
{
    plic_irq_ಗಣ_trigger(irq, ಅಂಚು_triggered);
}
#ಆದರೆ_ಅಂತ್ಯ

/* isIRQPending is used to determine whether to preempt long running
 * operations at various preemption points throughout the kernel. If this
 * returns true, it means that if the Kernel were to return to user mode, it
 * would then immediately take an interrupt. We check the SIP register for if
 * either a timer interrupt (STIP) or an external interrupt (SEIP) is pending.
 * We don't check software generated interrupts. These are used to perform cross
 * core signalling which isn't currently supported.
 * TODO: Add SSIP check when SMP support is added.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t isIRQPending(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t sip = read_sip();
    ಹಿಂತಿರುಗಿ (sip & (ಬಿಟ್(SIP_STIP) | ಬಿಟ್(SIP_SEIP)));
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
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಮುಖವಾಡ_ಅಡಚಣೆ(bool_t disable, irq_t irq)
{
    ಪ್ರತಿಜ್ಞಾ(IS_IRQ_VALID(irq));
    ಆದರೆ (irq == KERNEL_TIMER_IRQ) {
        ಆದರೆ (disable) {
            clear_sie_ಮುಖವಾಡ(ಬಿಟ್(SIE_STIE));
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಣ_sie_ಮುಖವಾಡ(ಬಿಟ್(SIE_STIE));
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
    } ಇಲ್ಲದಿದ್ದರೆ {
        plic_ಮುಖವಾಡ_irq(disable, irq);
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
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ack_ಅಡಚಣೆ(irq_t irq)
{
    ಪ್ರತಿಜ್ಞಾ(IS_IRQ_VALID(irq));
    ಸಕ್ರಿಯ_irq[CURRENT_CPU_ಸೂಚ್ಯಂಕ()] = irqInvalid;

    ಆದರೆ (irq == KERNEL_TIMER_IRQ) {
        /* Reprogramming the timer has cleared the interrupt. */
        ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಆದರೆ (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        ipi_clear_irq(irq);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ resetTimer(ಶೂನ್ಯ)
{
    uint64_t ಗುರಿ;
    // repeatedly try and set the timer in a loop as otherwise there is a race and we
    // may set a timeout in the past, resulting in it never getting triggered
    ಮಾಡು {
        ಗುರಿ = riscv_read_ಸಮಯ() + RESET_ಚಕ್ರಗಳು;
        sbi_ಗಣ_timer(ಗುರಿ);
    } ಆಗ (riscv_read_ಸಮಯ() > ಗುರಿ);
}

/**
   DONT_TRANSLATE
 */
BOOT_ಸಂಕೇತ ಶೂನ್ಯ initTimer(ಶೂನ್ಯ)
{
    sbi_ಗಣ_timer(riscv_read_ಸಮಯ() + RESET_ಚಕ್ರಗಳು);
}
#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_KERNEL_MCS */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initLocalIRQController(ಶೂನ್ಯ)
{
    ಬರೆ("Init local IRQ\n");

    /* Init per-hart PLIC */
    plic_init_hart();

    /* Enable timer and external interrupt. If SMP is enabled, then enable the
     * software interrupt also, it is used as IPI between cores. */
    ಗಣ_sie_ಮುಖವಾಡ(ಬಿಟ್(SIE_SEIE) | ಬಿಟ್(SIE_STIE) | SMP_TERNARY(ಬಿಟ್(SIE_SSIE), 0));
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initIRQController(ಶೂನ್ಯ)
{
    ಬರೆ("Initializing PLIC...\n");

    /* Initialize active_irq[] properly to stick to the semantics and play safe.
     * Effectively this is not needed if irqInvalid is zero (which is currently
     * the case) and the array is in the BSS, that is filled with zeros (which
     * the a kernel loader is supposed to do and which the ELF-Loader does).
     */
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ARRAY_ಗಾತ್ರ(ಸಕ್ರಿಯ_irq); i++) {
        ಸಕ್ರಿಯ_irq[i] = irqInvalid;
    }

    plic_init_controller();
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ handleSpuriousIRQ(ಶೂನ್ಯ)
{
    /* Do nothing */
}
