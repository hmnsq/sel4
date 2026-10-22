/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <machine/registerset.h>
#ضمن <machine/timer.h>
#ضمن <arch/machine.h>
#ضمن <arch/smp/ipi.h>

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
#تعریف_کړه RESET_دورې ((TIMER_CLOCK_HZ / MS_دننه_S) * CONFIG_TIMER_TICK_MS)
#د_که_پای /* !CONFIG_KERNEL_MCS */

#تعریف_کړه IS_IRQ_VALID(X) (((X)) <= maxIRQ && (X) != irqInvalid)

کلمه_t خالص getRestartPC(tcb_t *thread)
{
    أرجع get_ثبتوونکی(thread, FaultIP);
}

فراغ سېټ_بل_PC(tcb_t *thread, کلمه_t v)
{
    سېټ_ثبتوونکی(thread, بل_IP, v);
}

BOOT_کوډ فراغ map_kernel_devices(فراغ)
{
    /* If there are no kernel device frames at all, then kernel_device_frames is
     * NULL. Thus we can't use ARRAY_SIZE(kernel_device_frames) here directly,
     * but have to use NUM_KERNEL_DEVICE_FRAMES that is defined accordingly.
     */
    لكل (صحيح i = 0; i < عدد_KERNEL_DEVICE_چوکاټونه; i++) {
        ثابت kernel_چوکاټ_t *چوکاټ = &kernel_device_چوکاټونه[i];
        map_kernel_چوکاټ(چوکاټ->paddr, چوکاټ->pptr, VMKernelOnly);
        إذا (!چوکاټ->user_شته) {
            reserve_region((p_region_t) {
                .پیل = چوکاټ->paddr,
                .پای_ټکی   = چوکاټ->paddr + بټ(se_L4_Large_پاڼه_بټونه)
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

ساكن irq_t فعال_irq[CONFIG_MAX_عدد_NODES];


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
ساكن مضمن irq_t get_فعال_IRQ(فراغ)
{
    irq_t *فعال_irq_خونه = &فعال_irq[CURRENT_CPU_شاخص()];

    /* If an interrupt is currently active, then return it. */
    irq_t irq = *فعال_irq_خونه;
    إذا (IS_IRQ_VALID(irq)) {
        أرجع irq;
    }

    /* No interrupt currently active, find a new one from the sources. The
     * priorities are: external -> software -> timer.
     */
    کلمه_t sip = read_sip();
    إذا (sip & بټ(SIP_SEIP)) {
        /* Even if we say an external interrupt is pending, the PLIC may not
         * return any pending interrupt here in some corner cases. A level
         * triggered interrupt might have been deasserted again or another hard
         * has claimed it in a multicore system.
         */
        irq = plic_get_claim();
#که_تعریف_شوی CONFIG_PLAT_QEMU_RISCV_VIRT
        /* QEMU bug requires external interrupts to be immediately claimed. For
         * other platforms, the claim is done in invokeIRQHandler_AckIRQ.
         */
        plic_complete_claim(irq);
#د_که_پای
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    } وإلا إذا (sip & بټ(SIP_SSIP)) {
        sbi_clear_ipi();
        irq = ipi_get_irq();
#د_که_پای
    } وإلا إذا (sip & بټ(SIP_STIP)) {
        irq = KERNEL_TIMER_IRQ;
    } وإلا {
        /* Seems none of the known sources has a pending interrupt. This can
         * happen if e.g. if another hart context has claimed the interrupt
         * already.
         */
        irq = irqInvalid;
    }

    /* There is no guarantee that there is a new interrupt. */
    إذا (!IS_IRQ_VALID(irq)) {
        /* Sanity check: the slot can't hold an interrupt either. */
        أكد(!IS_IRQ_VALID(*فعال_irq_خونه));
        أرجع irqInvalid;
    }

    /* A new interrupt is active, remember it. */
    *فعال_irq_خونه = irq;
    أرجع irq;
}

#که_تعریف_شوی HAVE_سېټ_TRIGGER
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
فراغ سېټ_IRQTrigger(irq_t irq, bool_t څنډه_triggered)
{
    plic_irq_سېټ_trigger(irq, څنډه_triggered);
}
#د_که_پای

/* isIRQPending is used to determine whether to preempt long running
 * operations at various preemption points throughout the kernel. If this
 * returns true, it means that if the Kernel were to return to user mode, it
 * would then immediately take an interrupt. We check the SIP register for if
 * either a timer interrupt (STIP) or an external interrupt (SEIP) is pending.
 * We don't check software generated interrupts. These are used to perform cross
 * core signalling which isn't currently supported.
 * TODO: Add SSIP check when SMP support is added.
 */
ساكن مضمن bool_t isIRQPending(فراغ)
{
    کلمه_t sip = read_sip();
    أرجع (sip & (بټ(SIP_STIP) | بټ(SIP_SEIP)));
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
ساكن مضمن فراغ ماسک_مداخله(bool_t disable, irq_t irq)
{
    أكد(IS_IRQ_VALID(irq));
    إذا (irq == KERNEL_TIMER_IRQ) {
        إذا (disable) {
            clear_sie_ماسک(بټ(SIE_STIE));
        } وإلا {
            سېټ_sie_ماسک(بټ(SIE_STIE));
        }
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    } وإلا إذا (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        أرجع;
#د_که_پای
    } وإلا {
        plic_ماسک_irq(disable, irq);
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
ساكن مضمن فراغ ack_مداخله(irq_t irq)
{
    أكد(IS_IRQ_VALID(irq));
    فعال_irq[CURRENT_CPU_شاخص()] = irqInvalid;

    إذا (irq == KERNEL_TIMER_IRQ) {
        /* Reprogramming the timer has cleared the interrupt. */
        أرجع;
    }
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    إذا (irq == irq_reschedule_ipi || irq == irq_remote_call_ipi) {
        ipi_clear_irq(irq);
    }
#د_که_پای
}

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
فراغ resetTimer(فراغ)
{
    uint64_t موخه;
    // repeatedly try and set the timer in a loop as otherwise there is a race and we
    // may set a timeout in the past, resulting in it never getting triggered
    افعل {
        موخه = riscv_read_وخت() + RESET_دورې;
        sbi_سېټ_timer(موخه);
    } طالما (riscv_read_وخت() > موخه);
}

/**
   DONT_TRANSLATE
 */
BOOT_کوډ فراغ initTimer(فراغ)
{
    sbi_سېټ_timer(riscv_read_وخت() + RESET_دورې);
}
#د_که_پای /* !CONFIG_KERNEL_MCS */

BOOT_کوډ فراغ initLocalIRQController(فراغ)
{
    چاپ_کړه("Init local IRQ\n");

    /* Init per-hart PLIC */
    plic_init_hart();

    /* Enable timer and external interrupt. If SMP is enabled, then enable the
     * software interrupt also, it is used as IPI between cores. */
    سېټ_sie_ماسک(بټ(SIE_SEIE) | بټ(SIE_STIE) | SMP_TERNARY(بټ(SIE_SSIE), 0));
}

BOOT_کوډ فراغ initIRQController(فراغ)
{
    چاپ_کړه("Initializing PLIC...\n");

    /* Initialize active_irq[] properly to stick to the semantics and play safe.
     * Effectively this is not needed if irqInvalid is zero (which is currently
     * the case) and the array is in the BSS, that is filled with zeros (which
     * the a kernel loader is supposed to do and which the ELF-Loader does).
     */
    لكل (کلمه_t i = 0; i < ARRAY_اندازه(فعال_irq); i++) {
        فعال_irq[i] = irqInvalid;
    }

    plic_init_controller();
}

ساكن مضمن فراغ handleSpuriousIRQ(فراغ)
{
    /* Do nothing */
}
