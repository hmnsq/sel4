/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <armv/vcpu.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#ಅನ್ತರ್ಭಾವ <arch/machine/debug_conf.h>
#ಅನ್ತರ್ಭಾವ <drivers/timer/arm_generic.h>
#ಅನ್ತರ್ಭಾವ <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE ಶೂನ್ಯ vcpu_boot_init(ಶೂನ್ಯ)
{
    armv_vcpu_boot_init();
    gic_vcpu_ಅಂಕೆ_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    ಯದಿ (gic_vcpu_ಅಂಕೆ_list_regs > GIC_VCPU_MAX_ಅಂಕೆ_LR) {
        printf("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_ಅಂಕೆ_list_regs = GIC_VCPU_MAX_ಅಂಕೆ_LR;
    }
    vcpu_disable(ರಿಕ್ತ);
    ARCH_ಗಂಟು_STATE(armHSCurVCPU) = ರಿಕ್ತ;
    ARCH_ಗಂಟು_STATE(armHSVCPUActive) = false;

}

ಸ್ಥಿರ ಶೂನ್ಯ vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_ಅಂಕೆ;

    ಪ್ರತಿಜ್ಞಾ(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    ಯದಿ (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_ಅಂಕೆ = gic_vcpu_ಅಂಕೆ_list_regs;
    ಪ್ರತಿ (i = 0; i < lr_ಅಂಕೆ; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


ಸ್ಥಿರ word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    ಯದಿ (likely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) == vcpu)) {
        ಯದಿ (vcpu_reg_saved_when_disabled(field) && !ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
            ಪ್ರತ್ಯಾಗಮ vcpu_read_reg(vcpu, field);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ vcpu_hw_read_reg(field);
        }
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ vcpu_read_reg(vcpu, field);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ writeVCPUReg(vcpu_t *vcpu, word_t field, word_t ಮೌಲ್ಯ)
{
    ಯದಿ (likely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) == vcpu)) {
        ಯದಿ (vcpu_reg_saved_when_disabled(field) && !ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, ಮೌಲ್ಯ);
        } ಅನ್ಯಥಾ {
            vcpu_hw_write_reg(field, ಮೌಲ್ಯ);
        }
    } ಅನ್ಯಥಾ {
        vcpu_write_reg(vcpu, field, ಮೌಲ್ಯ);
    }
}

ಶೂನ್ಯ vcpu_restore(vcpu_t *vcpu)
{
    ಪ್ರತಿಜ್ಞಾ(vcpu);
    word_t i;
    word_t lr_ಅಂಕೆ;
    /* Turn off the VGIC */
    set_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    set_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    set_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_ಅಂಕೆ = gic_vcpu_ಅಂಕೆ_list_regs;
    ಪ್ರತಿ (i = 0; i < lr_ಅಂಕೆ; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

ಶೂನ್ಯ VPPIEvent(irq_t irq)
{
#ifdef CONFIG_KERNEL_MCS
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
    ಯದಿ (thread_state_get_tcbQueued(ಗಂಟು_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ಗಂಟು_STATE(ksCurThread))) {
        ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    ಯದಿ (ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        ಪ್ರತಿಜ್ಞಾ(irqVPPIEventIndex(irq) != VPPIEventIRQ_invalid);
        ARCH_ಗಂಟು_STATE(armHSCurVCPU)->vppi_masked[irqVPPIEventIndex(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        ಪ್ರತಿಜ್ಞಾ(isRunnable(ಗಂಟು_STATE(ksCurThread)));
        ಯದಿ (isRunnable(ಗಂಟು_STATE(ksCurThread))) {
            handleFault(ಗಂಟು_STATE(ksCurThread));
        }
    }
}

ಶೂನ್ಯ VGICMaintenance(ಶೂನ್ಯ)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ifdef CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    ಯದಿ (thread_state_get_tcbQueued(ಗಂಟು_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ಗಂಟು_STATE(ksCurThread))) {
        ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    ಯದಿ (!ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
        printf("Received VGIC maintenance without active VCPU!\n");
        ಪ್ರತ್ಯಾಗಮ;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    ಯದಿ (flags & VGIC_MISR_EOI) {
        ಪೂರ್ಣಾಙ್ಕ irq_idx;
        ಯದಿ (eisr0) {
            irq_idx = ctzl(eisr0);
        } ಅನ್ಯಥಾ ಯದಿ (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } ಅನ್ಯಥಾ {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        ಯದಿ (irq_idx == -1  || irq_idx >= gic_vcpu_ಅಂಕೆ_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } ಅನ್ಯಥಾ {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            ವಿಕಲ್ಪ (virq_get_virqType(virq)) {
            ಪಕ್ಷ virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                ಭಙ್ಗ;
            ಪಕ್ಷ virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                ಭಙ್ಗ;
            ಪಕ್ಷ virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                ಭಙ್ಗ;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            ಪ್ರತಿಜ್ಞಾ(ARCH_ಗಂಟು_STATE(armHSCurVCPU) != ರಿಕ್ತ && ARCH_ಗಂಟು_STATE(armHSVCPUActive));
            ಯದಿ (ARCH_ಗಂಟು_STATE(armHSCurVCPU) != ರಿಕ್ತ && ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
                ARCH_ಗಂಟು_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } ಅನ್ಯಥಾ {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } ಅನ್ಯಥಾ {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    ಪ್ರತಿಜ್ಞಾ(isRunnable(ಗಂಟು_STATE(ksCurThread)));
    ಯದಿ (isRunnable(ಗಂಟು_STATE(ksCurThread))) {
        handleFault(ಗಂಟು_STATE(ksCurThread));
    }
}

ಶೂನ್ಯ vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#endif
}

ಶೂನ್ಯ vcpu_switch(vcpu_t *new)
{
    ಯದಿ (likely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) != new)) {
        ಯದಿ (unlikely(new != ರಿಕ್ತ)) {
            ಯದಿ (unlikely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) != ರಿಕ್ತ)) {
                vcpu_save(ARCH_ಗಂಟು_STATE(armHSCurVCPU), ARCH_ಗಂಟು_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_ಗಂಟು_STATE(armHSCurVCPU) = new;
            ARCH_ಗಂಟು_STATE(armHSVCPUActive) = true;
        } ಅನ್ಯಥಾ ಯದಿ (unlikely(ARCH_ಗಂಟು_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_ಗಂಟು_STATE(armHSCurVCPU)->vcpuTCB);
#endif
            vcpu_disable(ARCH_ಗಂಟು_STATE(armHSCurVCPU));
            ARCH_ಗಂಟು_STATE(armHSVCPUActive) = false;
        }
    } ಅನ್ಯಥಾ ಯದಿ (likely(!ARCH_ಗಂಟು_STATE(armHSVCPUActive) && new != ರಿಕ್ತ)) {
        isb();
        vcpu_enable(new);
        ARCH_ಗಂಟು_STATE(armHSVCPUActive) = true;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ vcpu_invalidate_active(ಶೂನ್ಯ)
{
    ಯದಿ (ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
        vcpu_disable(ರಿಕ್ತ);
        ARCH_ಗಂಟು_STATE(armHSVCPUActive) = false;
    }
    ARCH_ಗಂಟು_STATE(armHSCurVCPU) = ರಿಕ್ತ;
}

ಶೂನ್ಯ vcpu_flush(ಶೂನ್ಯ)
{
    ಯದಿ (ARCH_ಗಂಟು_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_ಗಂಟು_STATE(armHSCurVCPU), ARCH_ಗಂಟು_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

ಶೂನ್ಯ vcpu_flush_if_current(tcb_t *tptr)
{
    ಯದಿ (tptr->tcbArch.tcbVCPU == ARCH_ಗಂಟು_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

ಶೂನ್ಯ vcpu_finalise(vcpu_t *vcpu)
{
    ಯದಿ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

ಶೂನ್ಯ associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ಯದಿ (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    ಯದಿ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    ಯದಿ (tcb == ಗಂಟು_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

ಶೂನ್ಯ dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ಯದಿ (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    ಯದಿ (vcpu == ARCH_ಗಂಟು_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = ರಿಕ್ತ;
    vcpu->vcpuTCB = ರಿಕ್ತ;
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#endif

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ifdef CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#ಅನ್ಯಥಾ
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#endif
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t ಮೌಲ್ಯ)
{
    writeVCPUReg(vcpu, field, ಮೌಲ್ಯ);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t ಮೌಲ್ಯ;
    ಯದಿ (length < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, buffer);
    ಮೌಲ್ಯ = getSyscallArg(1, buffer);
    ಯದಿ (field >= se_L4_VCPUReg_ಅಂಕೆ) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, ಮೌಲ್ಯ);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    word_t ಮೌಲ್ಯ = readVCPUReg(vcpu, field);
    ಯದಿ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned ಪೂರ್ಣಾಙ್ಕ length = setMR(thread, ipcBuffer, 0, ಮೌಲ್ಯ);
        setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    ಯದಿ (length < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);

    ಯದಿ (field >= se_L4_VCPUReg_ಅಂಕೆ) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    ಯದಿ (likely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
#ifdef ENABLE_SMP_SUPPORT
    } ಅನ್ಯಥಾ ಯದಿ (vcpu->vcpuTCB != ರಿಕ್ತ && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, index, virq.ಪದಗಳು[0],
                       vcpu->vcpuTCB->tcbAffinity);
#endif /* CONFIG_ENABLE_SMP */
    } ಅನ್ಯಥಾ {
        vcpu->vgic.lr[index] = virq;
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t length, word_t *buffer)
{
    word_t vid, priority, group, index;
    vcpu_t *vcpu;
#ifdef CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ಯದಿ (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = (mr0 >> 32) & 0xff;
#ಅನ್ಯಥಾ
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ಯದಿ (length < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    mr1 = getSyscallArg(1, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = mr1 & 0xff;
#endif

    /* Check IRQ parameters */
    ಯದಿ (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        current_syscall_error.type = seL4_RangeError;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 2;
        current_syscall_error.type = seL4_RangeError;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 3;
        current_syscall_error.type = seL4_RangeError;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    ಯದಿ (index >= gic_vcpu_ಅಂಕೆ_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_ಅಂಕೆ_list_regs - 1;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 4;
        current_syscall_error.type = seL4_RangeError;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    ಯದಿ (virq_get_virqType(vcpu->vgic.lr[index]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeVCPUInjectIRQ(vcpu, index, virq);
}

exception_t decodeARMVCPUInvocation(
    word_t label,
    word_t length,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    ವಿಕಲ್ಪ (label) {
    ಪಕ್ಷ ARMVCPUSetTCB:
        ಪ್ರತ್ಯಾಗಮ decodeVCPUSetTCB(cap);
    ಪಕ್ಷ ARMVCPUReadReg:
        ಪ್ರತ್ಯಾಗಮ decodeVCPUReadReg(cap, length, call, buffer);
    ಪಕ್ಷ ARMVCPUWriteReg:
        ಪ್ರತ್ಯಾಗಮ decodeVCPUWriteReg(cap, length, buffer);
    ಪಕ್ಷ ARMVCPUInjectIRQ:
        ಪ್ರತ್ಯಾಗಮ decodeVCPUInjectIRQ(cap, length, buffer);
    ಪಕ್ಷ ARMVCPUAckVPPI:
        ಪ್ರತ್ಯಾಗಮ decodeVCPUAckVPPI(cap, length, buffer);
    ಸ್ವಭಾವ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ಯದಿ (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, buffer);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    ಯದಿ (status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ status;
    }

    VPPIEventIRQ_t vppi = irqVPPIEventIndex(irq);
    ಯದಿ (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    ಯದಿ (current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}


ಶೂನ್ಯ handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        ಯದಿ (armv_handleVCPUFault(hsr))
        {
            ಪ್ರತ್ಯಾಗಮ;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(ಗಂಟು_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#ifdef ENABLE_SMP_SUPPORT
ಶೂನ್ಯ handleVCPUInjectInterruptIPI(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    ಯದಿ (likely(ARCH_ಗಂಟು_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
    } ಅನ್ಯಥಾ {
        vcpu->vgic.lr[index] = virq;
    }
}
#endif /* ENABLE_SMP_SUPPORT */

#endif
