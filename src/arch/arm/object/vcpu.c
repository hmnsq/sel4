/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/vcpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <armv/vcpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug_conf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/timer/arm_generic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_ਕੋਡ ਖਾਲੀ vcpu_boot_init(ਖਾਲੀ)
{
    armv_vcpu_boot_init();
    gic_vcpu_ਅੰਕ_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    ਜੇ (gic_vcpu_ਅੰਕ_list_regs > GIC_VCPU_MAX_ਅੰਕ_LR) {
        ਲਿਖੋ("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_ਅੰਕ_list_regs = GIC_VCPU_MAX_ਅੰਕ_LR;
    }
    vcpu_disable(ਨਿਰਾ);
    ARCH_ਗੰਢ_STATE(armHSCurVCPU) = ਨਿਰਾ;
    ARCH_ਗੰਢ_STATE(armHSVCPUActive) = false;

}

ਸਥਿਰ ਖਾਲੀ vcpu_save(vcpu_t *vcpu, bool_t ਸਰਗਰਮ)
{
    ਸ਼ਬਦ_t i;
    ਸ਼ਬਦ_t lr_ਅੰਕ;

    ਪ੍ਰਤਿਜ੍ਞਾ(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    ਜੇ (ਸਰਗਰਮ) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_ਅੰਕ = gic_vcpu_ਅੰਕ_list_regs;
    ਲਈ (i = 0; i < lr_ਅੰਕ; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, ਸਰਗਰਮ);
}


ਸਥਿਰ ਸ਼ਬਦ_t readVCPUReg(vcpu_t *vcpu, ਸ਼ਬਦ_t ਖੇਤਰ)
{
    ਜੇ (likely(ARCH_ਗੰਢ_STATE(armHSCurVCPU) == vcpu)) {
        ਜੇ (vcpu_reg_saved_when_disabled(ਖੇਤਰ) && !ARCH_ਗੰਢ_STATE(armHSVCPUActive)) {
            ਵਾਪਸ vcpu_read_reg(vcpu, ਖੇਤਰ);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ vcpu_hw_read_reg(ਖੇਤਰ);
        }
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ vcpu_read_reg(vcpu, ਖੇਤਰ);
    }
}

ਸਥਿਰ ਖਾਲੀ writeVCPUReg(vcpu_t *vcpu, ਸ਼ਬਦ_t ਖੇਤਰ, ਸ਼ਬਦ_t ਮੁੱਲ)
{
    ਜੇ (likely(ARCH_ਗੰਢ_STATE(armHSCurVCPU) == vcpu)) {
        ਜੇ (vcpu_reg_saved_when_disabled(ਖੇਤਰ) && !ARCH_ਗੰਢ_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, ਖੇਤਰ, ਮੁੱਲ);
        } ਨਹੀਂਤਾਂ {
            vcpu_hw_write_reg(ਖੇਤਰ, ਮੁੱਲ);
        }
    } ਨਹੀਂਤਾਂ {
        vcpu_write_reg(vcpu, ਖੇਤਰ, ਮੁੱਲ);
    }
}

ਖਾਲੀ vcpu_restore(vcpu_t *vcpu)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(vcpu);
    ਸ਼ਬਦ_t i;
    ਸ਼ਬਦ_t lr_ਅੰਕ;
    /* Turn off the VGIC */
    ਸੈੱਟ_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    ਸੈੱਟ_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    ਸੈੱਟ_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_ਅੰਕ = gic_vcpu_ਅੰਕ_list_regs;
    ਲਈ (i = 0; i < lr_ਅੰਕ; i++) {
        ਸੈੱਟ_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, se_L4_VCPUReg_Save_Range_ਸ਼ੁਰੂ, se_L4_VCPUReg_Save_Range_ਸਿਰਾ);
    vcpu_enable(vcpu);
}

ਖਾਲੀ VPPIEvent(irq_t irq)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    /* If the current task is currently enqueued it will not be able to
     * correctly receive a fault IPC message. This may occur due to the
     * budget check that happens early in the handleInterruptEntry.
     *
     * If the current thread does *not* have budget, as indicated by its
     * presence in either the scheduling or release queue, this interrupt
     * is ignored for now.
     * As it is a level-triggered interrupt it shall be re-raised
     * (and not lost).
     *
     * Additionally, if we have already received a timeout fault due to
     * our timeslice having ended, and we have a timeout fault handler for
     * this thread, we do not want to overwrite that with our VCPU fault.
     */
    ਜੇ (thread_state_get_tcbQueued(ਗੰਢ_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ਗੰਢ_STATE(ksCurThread))) {
        ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (ARCH_ਗੰਢ_STATE(armHSVCPUActive)) {
        ਨਕਾਬ_ਰੁਕਾਵਟ(true, irq);
        ਪ੍ਰਤਿਜ੍ਞਾ(irq_VPPIEvent_ਸੂਚਕਾਂਕ(irq) != VPPIEventIRQ_invalid);
        ARCH_ਗੰਢ_STATE(armHSCurVCPU)->vppi_masked[irq_VPPIEvent_ਸੂਚਕਾਂਕ(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_ਮੰਜ਼ਿਲ_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        ਪ੍ਰਤਿਜ੍ਞਾ(isRunnable(ਗੰਢ_STATE(ksCurThread)));
        ਜੇ (isRunnable(ਗੰਢ_STATE(ksCurThread))) {
            handleFault(ਗੰਢ_STATE(ksCurThread));
        }
    }
}

ਖਾਲੀ VGICMaintenance(ਖਾਲੀ)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    ਜੇ (thread_state_get_tcbQueued(ਗੰਢ_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ਗੰਢ_STATE(ksCurThread))) {
        ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    ਜੇ (!ARCH_ਗੰਢ_STATE(armHSVCPUActive)) {
        ਲਿਖੋ("Received VGIC maintenance without active VCPU!\n");
        ਵਾਪਸ;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    ਜੇ (flags & VGIC_MISR_EOI) {
        ਪੂਰਨ irq_idx;
        ਜੇ (eisr0) {
            irq_idx = ctzl(eisr0);
        } ਨਹੀਂਤਾਂ ਜੇ (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } ਨਹੀਂਤਾਂ {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        ਜੇ (irq_idx == -1  || irq_idx >= gic_vcpu_ਅੰਕ_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } ਨਹੀਂਤਾਂ {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            ਚੋਣ (virq_get_virqType(virq)) {
            ਹਾਲਤ virq_virq_ਸਰਗਰਮ:
                virq = virq_virq_ਸਰਗਰਮ_ਸੈੱਟ_virq_EOIIRQEN(virq, 0);
                ਰੋਕੋ;
            ਹਾਲਤ virq_virq_pending:
                virq = virq_virq_pending_ਸੈੱਟ_virq_EOIIRQEN(virq, 0);
                ਰੋਕੋ;
            ਹਾਲਤ virq_virq_invalid:
                virq = virq_virq_invalid_ਸੈੱਟ_virq_EOIIRQEN(virq, 0);
                ਰੋਕੋ;
            }
            ਸੈੱਟ_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            ਪ੍ਰਤਿਜ੍ਞਾ(ARCH_ਗੰਢ_STATE(armHSCurVCPU) != ਨਿਰਾ && ARCH_ਗੰਢ_STATE(armHSVCPUActive));
            ਜੇ (ARCH_ਗੰਢ_STATE(armHSCurVCPU) != ਨਿਰਾ && ARCH_ਗੰਢ_STATE(armHSVCPUActive)) {
                ARCH_ਗੰਢ_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } ਨਹੀਂਤਾਂ {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } ਨਹੀਂਤਾਂ {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    ਪ੍ਰਤਿਜ੍ਞਾ(isRunnable(ਗੰਢ_STATE(ksCurThread)));
    ਜੇ (isRunnable(ਗੰਢ_STATE(ksCurThread))) {
        handleFault(ਗੰਢ_STATE(ksCurThread));
    }
}

ਖਾਲੀ vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.ਆਖਰੀ_pcount = 0;
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ vcpu_switch(vcpu_t *new)
{
    ਜੇ (likely(ARCH_ਗੰਢ_STATE(armHSCurVCPU) != new)) {
        ਜੇ (unlikely(new != ਨਿਰਾ)) {
            ਜੇ (unlikely(ARCH_ਗੰਢ_STATE(armHSCurVCPU) != ਨਿਰਾ)) {
                vcpu_save(ARCH_ਗੰਢ_STATE(armHSCurVCPU), ARCH_ਗੰਢ_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_ਗੰਢ_STATE(armHSCurVCPU) = new;
            ARCH_ਗੰਢ_STATE(armHSVCPUActive) = true;
        } ਨਹੀਂਤਾਂ ਜੇ (unlikely(ARCH_ਗੰਢ_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_ਧਾਗੇ
            saveAllBreakpointState(ARCH_ਗੰਢ_STATE(armHSCurVCPU)->vcpuTCB);
#ਜੇ_ਅੰਤ
            vcpu_disable(ARCH_ਗੰਢ_STATE(armHSCurVCPU));
            ARCH_ਗੰਢ_STATE(armHSVCPUActive) = false;
        }
    } ਨਹੀਂਤਾਂ ਜੇ (likely(!ARCH_ਗੰਢ_STATE(armHSVCPUActive) && new != ਨਿਰਾ)) {
        isb();
        vcpu_enable(new);
        ARCH_ਗੰਢ_STATE(armHSVCPUActive) = true;
    }
}

ਸਥਿਰ ਖਾਲੀ vcpu_invalidate_ਸਰਗਰਮ(ਖਾਲੀ)
{
    ਜੇ (ARCH_ਗੰਢ_STATE(armHSVCPUActive)) {
        vcpu_disable(ਨਿਰਾ);
        ARCH_ਗੰਢ_STATE(armHSVCPUActive) = false;
    }
    ARCH_ਗੰਢ_STATE(armHSCurVCPU) = ਨਿਰਾ;
}

ਖਾਲੀ vcpu_flush(ਖਾਲੀ)
{
    ਜੇ (ARCH_ਗੰਢ_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_ਗੰਢ_STATE(armHSCurVCPU), ARCH_ਗੰਢ_STATE(armHSVCPUActive));
        vcpu_invalidate_ਸਰਗਰਮ();
    }
}

ਖਾਲੀ vcpu_flush_if_current(tcb_t *tptr)
{
    ਜੇ (tptr->tcbArch.tcbVCPU == ARCH_ਗੰਢ_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

ਖਾਲੀ vcpu_finalise(vcpu_t *vcpu)
{
    ਜੇ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

ਖਾਲੀ associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ਜੇ (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    ਜੇ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    ਜੇ (tcb == ਗੰਢ_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

ਖਾਲੀ dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ਜੇ (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    ਜੇ (vcpu == ARCH_ਗੰਢ_STATE(armHSCurVCPU)) {
        vcpu_invalidate_ਸਰਗਰਮ();
    }
    tcb->tcbArch.tcbVCPU = ਨਿਰਾ;
    vcpu->vcpuTCB = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_ਧਾਗੇ
    Arch_debugDissociateVCPUTCB(tcb);
#ਜੇ_ਅੰਤ

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, SPSR_EL1, sanitise_ਰਜਿਸਟਰ(SPSR_EL1, get_ਰਜਿਸਟਰ(tcb, SPSR_EL1), false));
#ਨਹੀਂਤਾਂ
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, CPSR, sanitise_ਰਜਿਸਟਰ(CPSR, get_ਰਜਿਸਟਰ(tcb, CPSR), false));
#ਜੇ_ਅੰਤ
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, ਸ਼ਬਦ_t ਖੇਤਰ, ਸ਼ਬਦ_t ਮੁੱਲ)
{
    writeVCPUReg(vcpu, ਖੇਤਰ, ਮੁੱਲ);
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t ਖੇਤਰ;
    ਸ਼ਬਦ_t ਮੁੱਲ;
    ਜੇ (ਲੰਮਾਈ < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਖੇਤਰ = getSyscallArg(0, ਬਫਰ);
    ਮੁੱਲ = getSyscallArg(1, ਬਫਰ);
    ਜੇ (ਖੇਤਰ >= se_L4_VCPUReg_ਅੰਕ) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (ਲੰਮਾ)ਖੇਤਰ);
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ਖੇਤਰ, ਮੁੱਲ);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, ਸ਼ਬਦ_t ਖੇਤਰ, bool_t call)
{
    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);
    ਸ਼ਬਦ_t ਮੁੱਲ = readVCPUReg(vcpu, ਖੇਤਰ);
    ਜੇ (call) {
        ਸ਼ਬਦ_t *ipc_ਬਫਰ = lookupIPCBuffer(true, thread);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ = ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, 0, ਮੁੱਲ);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t ਖੇਤਰ;
    ਜੇ (ਲੰਮਾਈ < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਖੇਤਰ = getSyscallArg(0, ਬਫਰ);

    ਜੇ (ਖੇਤਰ >= se_L4_VCPUReg_ਅੰਕ) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (ਲੰਮਾ)ਖੇਤਰ);
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ਖੇਤਰ, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸੂਚਕਾਂਕ, virq_t virq)
{
    ਜੇ (likely(ARCH_ਗੰਢ_STATE(armHSCurVCPU) == vcpu)) {
        ਸੈੱਟ_gic_vcpu_ctrl_lr(ਸੂਚਕਾਂਕ, virq);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    } ਨਹੀਂਤਾਂ ਜੇ (vcpu->vcpuTCB != ਨਿਰਾ && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(Ipi_Remote_Call_VCPUInject_ਰੁਕਾਵਟ,
                       (ਸ਼ਬਦ_t)vcpu, ਸੂਚਕਾਂਕ, virq.ਸ਼ਬਦ[0],
                       vcpu->vcpuTCB->tcbAffinity);
#ਜੇ_ਅੰਤ /* CONFIG_ENABLE_SMP */
    } ਨਹੀਂਤਾਂ {
        vcpu->vgic.lr[ਸੂਚਕਾਂਕ] = virq;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t vid, ਤਰਜੀਹ, ਸਮੂਹ, ਸੂਚਕਾਂਕ;
    vcpu_t *vcpu;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
    ਸ਼ਬਦ_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ਜੇ (ਲੰਮਾਈ < 1) {
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, ਬਫਰ);
    vid = mr0 & 0xffff;
    ਤਰਜੀਹ = (mr0 >> 16) & 0xff;
    ਸਮੂਹ = (mr0 >> 24) & 0xff;
    ਸੂਚਕਾਂਕ = (mr0 >> 32) & 0xff;
#ਨਹੀਂਤਾਂ
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ਜੇ (ਲੰਮਾਈ < 2) {
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, ਬਫਰ);
    mr1 = getSyscallArg(1, ਬਫਰ);
    vid = mr0 & 0xffff;
    ਤਰਜੀਹ = (mr0 >> 16) & 0xff;
    ਸਮੂਹ = (mr0 >> 24) & 0xff;
    ਸੂਚਕਾਂਕ = mr1 & 0xff;
#ਜੇ_ਅੰਤ

    /* Check IRQ parameters */
    ਜੇ (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        current_syscall_error.type = seL4_RangeError;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (ਤਰਜੀਹ > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 2;
        current_syscall_error.type = seL4_RangeError;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (ਸਮੂਹ > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 3;
        current_syscall_error.type = seL4_RangeError;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    ਜੇ (ਸੂਚਕਾਂਕ >= gic_vcpu_ਅੰਕ_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_ਅੰਕ_list_regs - 1;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 4;
        current_syscall_error.type = seL4_RangeError;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    ਜੇ (virq_get_virqType(vcpu->vgic.lr[ਸੂਚਕਾਂਕ]) == virq_virq_ਸਰਗਰਮ) {
        userError("VGIC List register in use.");
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(ਸਮੂਹ, ਤਰਜੀਹ, 1, vid);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeVCPUInjectIRQ(vcpu, ਸੂਚਕਾਂਕ, virq);
}

exception_t decodeARMVCPUInvocation(
    ਸ਼ਬਦ_t label,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *ਥਾਂ,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਚੋਣ (label) {
    ਹਾਲਤ ARMVCPUSetTCB:
        ਵਾਪਸ decodeVCPUSetTCB(cap);
    ਹਾਲਤ ARMVCPUReadReg:
        ਵਾਪਸ decodeVCPUReadReg(cap, ਲੰਮਾਈ, call, ਬਫਰ);
    ਹਾਲਤ ARMVCPUWriteReg:
        ਵਾਪਸ decodeVCPUWriteReg(cap, ਲੰਮਾਈ, ਬਫਰ);
    ਹਾਲਤ ARMVCPUInjectIRQ:
        ਵਾਪਸ decodeVCPUInjectIRQ(cap, ਲੰਮਾਈ, ਬਫਰ);
    ਹਾਲਤ ARMVCPUAckVPPI:
        ਵਾਪਸ decodeVCPUAckVPPI(cap, ਲੰਮਾਈ, ਬਫਰ);
    ਮੂਲ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ਜੇ (ਲੰਮਾਈ < 1) {
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸ਼ਬਦ_t irq_w = getSyscallArg(0, ਬਫਰ);
    irq_t irq = (irq_t) CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(CURRENT_CPU_ਸੂਚਕਾਂਕ(), irq_w);
    exception_t ਸਥਿਤੀ = Arch_checkIRQ(irq_w);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ ਸਥਿਤੀ;
    }

    VPPIEventIRQ_t vppi = irq_VPPIEvent_ਸੂਚਕਾਂਕ(irq);
    ਜੇ (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    ਵਾਪਸ EXCEPTION_NONE;
}


ਖਾਲੀ handleVCPUFault(ਸ਼ਬਦ_t hsr)
{
    MCS_DO_IF_BUDGET({
        ਜੇ (armv_handleVCPUFault(hsr))
        {
            ਵਾਪਸ;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(ਗੰਢ_STATE(ksCurThread));
    })
    ਸਮਾਂ_ਸਾਰਣੀ();
    activateThread();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
ਖਾਲੀ handle_VCPUInject_ਰੁਕਾਵਟ_IPI(vcpu_t *vcpu, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸੂਚਕਾਂਕ, virq_t virq)
{
    ਜੇ (likely(ARCH_ਗੰਢ_STATE(armHSCurVCPU) == vcpu)) {
        ਸੈੱਟ_gic_vcpu_ctrl_lr(ਸੂਚਕਾਂਕ, virq);
    } ਨਹੀਂਤਾਂ {
        vcpu->vgic.lr[ਸੂਚਕਾਂਕ] = virq;
    }
}
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

#ਜੇ_ਅੰਤ
