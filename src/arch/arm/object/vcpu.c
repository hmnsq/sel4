/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT

#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <armv/vcpu.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#ಅನ್ತರ್ಭಾವ <arch/machine/debug_conf.h>
#ಅನ್ತರ್ಭಾವ <drivers/timer/arm_generic.h>
#ಅನ್ತರ್ಭಾವ <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ vcpu_boot_init(ಶೂನ್ಯ)
{
    armv_vcpu_boot_init();
    gic_vcpu_ಅಂಕೆ_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    ಆದರೆ (gic_vcpu_ಅಂಕೆ_list_regs > GIC_VCPU_MAX_ಅಂಕೆ_LR) {
        ಬರೆ("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_ಅಂಕೆ_list_regs = GIC_VCPU_MAX_ಅಂಕೆ_LR;
    }
    vcpu_disable(ಖಾಲಿ);
    ARCH_ಗಂಟು_STATE(armHSCurVCPU) = ಖಾಲಿ;
    ARCH_ಗಂಟು_STATE(armHSVCPUActive) = false;

}

ಸ್ಥಿರ ಶೂನ್ಯ vcpu_save(vcpu_t *vcpu, bool_t ಸಕ್ರಿಯ)
{
    ಶಬ್ದ_t i;
    ಶಬ್ದ_t lr_ಅಂಕೆ;

    ಪ್ರತಿಜ್ಞಾ(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    ಆದರೆ (ಸಕ್ರಿಯ) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_ಅಂಕೆ = gic_vcpu_ಅಂಕೆ_list_regs;
    ಗಾಗಿ (i = 0; i < lr_ಅಂಕೆ; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, ಸಕ್ರಿಯ);
}


ಸ್ಥಿರ ಶಬ್ದ_t readVCPUReg(vcpu_t *vcpu, ಶಬ್ದ_t ಕ್ಷೇತ್ರ)
{
    ಆದರೆ (likely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) == vcpu)) {
        ಆದರೆ (vcpu_reg_saved_when_disabled(ಕ್ಷೇತ್ರ) && !ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
            ಹಿಂತಿರುಗಿ vcpu_read_reg(vcpu, ಕ್ಷೇತ್ರ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ vcpu_hw_read_reg(ಕ್ಷೇತ್ರ);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ vcpu_read_reg(vcpu, ಕ್ಷೇತ್ರ);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ writeVCPUReg(vcpu_t *vcpu, ಶಬ್ದ_t ಕ್ಷೇತ್ರ, ಶಬ್ದ_t ಮೌಲ್ಯ)
{
    ಆದರೆ (likely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) == vcpu)) {
        ಆದರೆ (vcpu_reg_saved_when_disabled(ಕ್ಷೇತ್ರ) && !ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            vcpu_hw_write_reg(ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        vcpu_write_reg(vcpu, ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
    }
}

ಶೂನ್ಯ vcpu_restore(vcpu_t *vcpu)
{
    ಪ್ರತಿಜ್ಞಾ(vcpu);
    ಶಬ್ದ_t i;
    ಶಬ್ದ_t lr_ಅಂಕೆ;
    /* Turn off the VGIC */
    ಗಣ_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    ಗಣ_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    ಗಣ_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_ಅಂಕೆ = gic_vcpu_ಅಂಕೆ_list_regs;
    ಗಾಗಿ (i = 0; i < lr_ಅಂಕೆ; i++) {
        ಗಣ_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, se_L4_VCPUReg_Save_Range_ಆರಂಭ, se_L4_VCPUReg_Save_Range_ಕೊನೆ);
    vcpu_enable(vcpu);
}

ಶೂನ್ಯ VPPIEvent(irq_t irq)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
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
    ಆದರೆ (thread_state_get_tcbQueued(ಗಂಟು_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ಗಂಟು_STATE(ksCurThread))) {
        ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
        ಮುಖವಾಡ_ಅಡಚಣೆ(true, irq);
        ಪ್ರತಿಜ್ಞಾ(irq_VPPIEvent_ಸೂಚ್ಯಂಕ(irq) != VPPIEventIRQ_invalid);
        ARCH_ಗಂಟು_STATE(armHSCurVCPU)->vppi_masked[irq_VPPIEvent_ಸೂಚ್ಯಂಕ(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_ಗಮ್ಯ_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        ಪ್ರತಿಜ್ಞಾ(isRunnable(ಗಂಟು_STATE(ksCurThread)));
        ಆದರೆ (isRunnable(ಗಂಟು_STATE(ksCurThread))) {
            handleFault(ಗಂಟು_STATE(ksCurThread));
        }
    }
}

ಶೂನ್ಯ VGICMaintenance(ಶೂನ್ಯ)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    ಆದರೆ (thread_state_get_tcbQueued(ಗಂಟು_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ಗಂಟು_STATE(ksCurThread))) {
        ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    ಆದರೆ (!ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
        ಬರೆ("Received VGIC maintenance without active VCPU!\n");
        ಹಿಂತಿರುಗಿ;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    ಆದರೆ (flags & VGIC_MISR_EOI) {
        ಪೂರ್ಣಾಂಕ irq_idx;
        ಆದರೆ (eisr0) {
            irq_idx = ctzl(eisr0);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } ಇಲ್ಲದಿದ್ದರೆ {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        ಆದರೆ (irq_idx == -1  || irq_idx >= gic_vcpu_ಅಂಕೆ_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } ಇಲ್ಲದಿದ್ದರೆ {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            ಆಯ್ಕೆ (virq_get_virqType(virq)) {
            ಸ್ಥಿತಿ virq_virq_ಸಕ್ರಿಯ:
                virq = virq_virq_ಸಕ್ರಿಯ_ಗಣ_virq_EOIIRQEN(virq, 0);
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ virq_virq_pending:
                virq = virq_virq_pending_ಗಣ_virq_EOIIRQEN(virq, 0);
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ virq_virq_invalid:
                virq = virq_virq_invalid_ಗಣ_virq_EOIIRQEN(virq, 0);
                ನಿಲ್ಲಿಸು;
            }
            ಗಣ_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            ಪ್ರತಿಜ್ಞಾ(ARCH_ಗಂಟು_STATE(armHSCurVCPU) != ಖಾಲಿ && ARCH_ಗಂಟು_STATE(armHSVCPUActive));
            ಆದರೆ (ARCH_ಗಂಟು_STATE(armHSCurVCPU) != ಖಾಲಿ && ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
                ARCH_ಗಂಟು_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    ಪ್ರತಿಜ್ಞಾ(isRunnable(ಗಂಟು_STATE(ksCurThread)));
    ಆದರೆ (isRunnable(ಗಂಟು_STATE(ksCurThread))) {
        handleFault(ಗಂಟು_STATE(ksCurThread));
    }
}

ಶೂನ್ಯ vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.ಕೊನೆಯ_pcount = 0;
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ vcpu_switch(vcpu_t *new)
{
    ಆದರೆ (likely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) != new)) {
        ಆದರೆ (unlikely(new != ಖಾಲಿ)) {
            ಆದರೆ (unlikely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) != ಖಾಲಿ)) {
                vcpu_save(ARCH_ಗಂಟು_STATE(armHSCurVCPU), ARCH_ಗಂಟು_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_ಗಂಟು_STATE(armHSCurVCPU) = new;
            ARCH_ಗಂಟು_STATE(armHSVCPUActive) = true;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (unlikely(ARCH_ಗಂಟು_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_ಎಳೆಗಳು
            saveAllBreakpointState(ARCH_ಗಂಟು_STATE(armHSCurVCPU)->vcpuTCB);
#ಆದರೆ_ಅಂತ್ಯ
            vcpu_disable(ARCH_ಗಂಟು_STATE(armHSCurVCPU));
            ARCH_ಗಂಟು_STATE(armHSVCPUActive) = false;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (likely(!ARCH_ಗಂಟು_STATE(armHSVCPUActive) && new != ಖಾಲಿ)) {
        isb();
        vcpu_enable(new);
        ARCH_ಗಂಟು_STATE(armHSVCPUActive) = true;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ vcpu_invalidate_ಸಕ್ರಿಯ(ಶೂನ್ಯ)
{
    ಆದರೆ (ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
        vcpu_disable(ಖಾಲಿ);
        ARCH_ಗಂಟು_STATE(armHSVCPUActive) = false;
    }
    ARCH_ಗಂಟು_STATE(armHSCurVCPU) = ಖಾಲಿ;
}

ಶೂನ್ಯ vcpu_flush(ಶೂನ್ಯ)
{
    ಆದರೆ (ARCH_ಗಂಟು_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_ಗಂಟು_STATE(armHSCurVCPU), ARCH_ಗಂಟು_STATE(armHSVCPUActive));
        vcpu_invalidate_ಸಕ್ರಿಯ();
    }
}

ಶೂನ್ಯ vcpu_flush_if_current(tcb_t *tptr)
{
    ಆದರೆ (tptr->tcbArch.tcbVCPU == ARCH_ಗಂಟು_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

ಶೂನ್ಯ vcpu_finalise(vcpu_t *vcpu)
{
    ಆದರೆ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

ಶೂನ್ಯ associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ಆದರೆ (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    ಆದರೆ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    ಆದರೆ (tcb == ಗಂಟು_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

ಶೂನ್ಯ dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ಆದರೆ (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    ಆದರೆ (vcpu == ARCH_ಗಂಟು_STATE(armHSCurVCPU)) {
        vcpu_invalidate_ಸಕ್ರಿಯ();
    }
    tcb->tcbArch.tcbVCPU = ಖಾಲಿ;
    vcpu->vcpuTCB = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_ಎಳೆಗಳು
    Arch_debugDissociateVCPUTCB(tcb);
#ಆದರೆ_ಅಂತ್ಯ

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, SPSR_EL1, sanitise_ರಿಜಿಸ್ಟರ್(SPSR_EL1, get_ರಿಜಿಸ್ಟರ್(tcb, SPSR_EL1), false));
#ಇಲ್ಲದಿದ್ದರೆ
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, CPSR, sanitise_ರಿಜಿಸ್ಟರ್(CPSR, get_ರಿಜಿಸ್ಟರ್(tcb, CPSR), false));
#ಆದರೆ_ಅಂತ್ಯ
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, ಶಬ್ದ_t ಕ್ಷೇತ್ರ, ಶಬ್ದ_t ಮೌಲ್ಯ)
{
    writeVCPUReg(vcpu, ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t ಕ್ಷೇತ್ರ;
    ಶಬ್ದ_t ಮೌಲ್ಯ;
    ಆದರೆ (ಉದ್ದಳತೆ < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಕ್ಷೇತ್ರ = getSyscallArg(0, ಬಫರ್);
    ಮೌಲ್ಯ = getSyscallArg(1, ಬಫರ್);
    ಆದರೆ (ಕ್ಷೇತ್ರ >= se_L4_VCPUReg_ಅಂಕೆ) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (ದೀರ್ಘ)ಕ್ಷೇತ್ರ);
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, ಶಬ್ದ_t ಕ್ಷೇತ್ರ, bool_t call)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    ಶಬ್ದ_t ಮೌಲ್ಯ = readVCPUReg(vcpu, ಕ್ಷೇತ್ರ);
    ಆದರೆ (call) {
        ಶಬ್ದ_t *ipc_ಬಫರ್ = lookupIPCBuffer(true, thread);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ಗಣ_MR(thread, ipc_ಬಫರ್, 0, ಮೌಲ್ಯ);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t ಕ್ಷೇತ್ರ;
    ಆದರೆ (ಉದ್ದಳತೆ < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಕ್ಷೇತ್ರ = getSyscallArg(0, ಬಫರ್);

    ಆದರೆ (ಕ್ಷೇತ್ರ >= se_L4_VCPUReg_ಅಂಕೆ) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (ದೀರ್ಘ)ಕ್ಷೇತ್ರ);
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ಕ್ಷೇತ್ರ, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸೂಚ್ಯಂಕ, virq_t virq)
{
    ಆದರೆ (likely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) == vcpu)) {
        ಗಣ_gic_vcpu_ctrl_lr(ಸೂಚ್ಯಂಕ, virq);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vcpu->vcpuTCB != ಖಾಲಿ && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(Ipi_Remote_Call_VCPUInject_ಅಡಚಣೆ,
                       (ಶಬ್ದ_t)vcpu, ಸೂಚ್ಯಂಕ, virq.ಪದಗಳು[0],
                       vcpu->vcpuTCB->tcbAffinity);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ENABLE_SMP */
    } ಇಲ್ಲದಿದ್ದರೆ {
        vcpu->vgic.lr[ಸೂಚ್ಯಂಕ] = virq;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t vid, ಆದ್ಯತೆ, ಗುಂಪು, ಸೂಚ್ಯಂಕ;
    vcpu_t *vcpu;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    ಶಬ್ದ_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ಆದರೆ (ಉದ್ದಳತೆ < 1) {
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, ಬಫರ್);
    vid = mr0 & 0xffff;
    ಆದ್ಯತೆ = (mr0 >> 16) & 0xff;
    ಗುಂಪು = (mr0 >> 24) & 0xff;
    ಸೂಚ್ಯಂಕ = (mr0 >> 32) & 0xff;
#ಇಲ್ಲದಿದ್ದರೆ
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ಆದರೆ (ಉದ್ದಳತೆ < 2) {
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, ಬಫರ್);
    mr1 = getSyscallArg(1, ಬಫರ್);
    vid = mr0 & 0xffff;
    ಆದ್ಯತೆ = (mr0 >> 16) & 0xff;
    ಗುಂಪು = (mr0 >> 24) & 0xff;
    ಸೂಚ್ಯಂಕ = mr1 & 0xff;
#ಆದರೆ_ಅಂತ್ಯ

    /* Check IRQ parameters */
    ಆದರೆ (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        current_syscall_error.type = seL4_RangeError;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (ಆದ್ಯತೆ > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 2;
        current_syscall_error.type = seL4_RangeError;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (ಗುಂಪು > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 3;
        current_syscall_error.type = seL4_RangeError;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    ಆದರೆ (ಸೂಚ್ಯಂಕ >= gic_vcpu_ಅಂಕೆ_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_ಅಂಕೆ_list_regs - 1;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 4;
        current_syscall_error.type = seL4_RangeError;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    ಆದರೆ (virq_get_virqType(vcpu->vgic.lr[ಸೂಚ್ಯಂಕ]) == virq_virq_ಸಕ್ರಿಯ) {
        userError("VGIC List register in use.");
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(ಗುಂಪು, ಆದ್ಯತೆ, 1, vid);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeVCPUInjectIRQ(vcpu, ಸೂಚ್ಯಂಕ, virq);
}

exception_t decodeARMVCPUInvocation(
    ಶಬ್ದ_t label,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *ಸ್ಥಾನ,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಆಯ್ಕೆ (label) {
    ಸ್ಥಿತಿ ARMVCPUSetTCB:
        ಹಿಂತಿರುಗಿ decodeVCPUSetTCB(cap);
    ಸ್ಥಿತಿ ARMVCPUReadReg:
        ಹಿಂತಿರುಗಿ decodeVCPUReadReg(cap, ಉದ್ದಳತೆ, call, ಬಫರ್);
    ಸ್ಥಿತಿ ARMVCPUWriteReg:
        ಹಿಂತಿರುಗಿ decodeVCPUWriteReg(cap, ಉದ್ದಳತೆ, ಬಫರ್);
    ಸ್ಥಿತಿ ARMVCPUInjectIRQ:
        ಹಿಂತಿರುಗಿ decodeVCPUInjectIRQ(cap, ಉದ್ದಳತೆ, ಬಫರ್);
    ಸ್ಥಿತಿ ARMVCPUAckVPPI:
        ಹಿಂತಿರುಗಿ decodeVCPUAckVPPI(cap, ಉದ್ದಳತೆ, ಬಫರ್);
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ಆದರೆ (ಉದ್ದಳತೆ < 1) {
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಶಬ್ದ_t irq_w = getSyscallArg(0, ಬಫರ್);
    irq_t irq = (irq_t) CORE_IRQ_ಗಮ್ಯ_IRQT(CURRENT_CPU_ಸೂಚ್ಯಂಕ(), irq_w);
    exception_t ಸ್ಥಿತಿ_2 = Arch_checkIRQ(irq_w);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }

    VPPIEventIRQ_t vppi = irq_VPPIEvent_ಸೂಚ್ಯಂಕ(irq);
    ಆದರೆ (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}


ಶೂನ್ಯ handleVCPUFault(ಶಬ್ದ_t hsr)
{
    MCS_DO_IF_BUDGET({
        ಆದರೆ (armv_handleVCPUFault(hsr))
        {
            ಹಿಂತಿರುಗಿ;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(ಗಂಟು_STATE(ksCurThread));
    })
    ವೇಳಾಪಟ್ಟಿ();
    activateThread();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
ಶೂನ್ಯ handle_VCPUInject_ಅಡಚಣೆ_IPI(vcpu_t *vcpu, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸೂಚ್ಯಂಕ, virq_t virq)
{
    ಆದರೆ (likely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) == vcpu)) {
        ಗಣ_gic_vcpu_ctrl_lr(ಸೂಚ್ಯಂಕ, virq);
    } ಇಲ್ಲದಿದ್ದರೆ {
        vcpu->vgic.lr[ಸೂಚ್ಯಂಕ] = virq;
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

#ಆದರೆ_ಅಂತ್ಯ
