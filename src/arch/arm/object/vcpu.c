/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#अन्तर्भाव <arch/object/vcpu.h>
#अन्तर्भाव <armv/vcpu.h>
#अन्तर्भाव <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#अन्तर्भाव <arch/machine/debug_conf.h>
#अन्तर्भाव <drivers/timer/arm_generic.h>
#अन्तर्भाव <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE शून्य vcpu_boot_init(शून्य)
{
    armv_vcpu_boot_init();
    gic_vcpu_अङ्कः_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    जइ (gic_vcpu_अङ्कः_list_regs > GIC_VCPU_MAX_अङ्कः_LR) {
        printf("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_अङ्कः_list_regs = GIC_VCPU_MAX_अङ्कः_LR;
    }
    vcpu_disable(रिक्त);
    ARCH_ग्रन्थिः_STATE(armHSCurVCPU) = रिक्त;
    ARCH_ग्रन्थिः_STATE(armHSVCPUActive) = false;

}

स्थिर शून्य vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_अङ्कः;

    प्रतिज्ञा(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    जइ (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_अङ्कः = gic_vcpu_अङ्कः_list_regs;
    प्रति (i = 0; i < lr_अङ्कः; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


स्थिर word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    जइ (likely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) == vcpu)) {
        जइ (vcpu_reg_saved_when_disabled(field) && !ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
            प्रत्यागम vcpu_read_reg(vcpu, field);
        } अण्णहा {
            प्रत्यागम vcpu_hw_read_reg(field);
        }
    } अण्णहा {
        प्रत्यागम vcpu_read_reg(vcpu, field);
    }
}

स्थिर शून्य writeVCPUReg(vcpu_t *vcpu, word_t field, word_t मानम्)
{
    जइ (likely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) == vcpu)) {
        जइ (vcpu_reg_saved_when_disabled(field) && !ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, मानम्);
        } अण्णहा {
            vcpu_hw_write_reg(field, मानम्);
        }
    } अण्णहा {
        vcpu_write_reg(vcpu, field, मानम्);
    }
}

शून्य vcpu_restore(vcpu_t *vcpu)
{
    प्रतिज्ञा(vcpu);
    word_t i;
    word_t lr_अङ्कः;
    /* Turn off the VGIC */
    set_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    set_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    set_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_अङ्कः = gic_vcpu_अङ्कः_list_regs;
    प्रति (i = 0; i < lr_अङ्कः; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

शून्य VPPIEvent(irq_t irq)
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
    जइ (thread_state_get_tcbQueued(ग्रन्थिः_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ग्रन्थिः_STATE(ksCurThread))) {
        प्रत्यागम;
    }
#endif

    जइ (ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        प्रतिज्ञा(irqVPPIEventIndex(irq) != VPPIEventIRQ_invalid);
        ARCH_ग्रन्थिः_STATE(armHSCurVCPU)->vppi_masked[irqVPPIEventIndex(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        प्रतिज्ञा(isRunnable(ग्रन्थिः_STATE(ksCurThread)));
        जइ (isRunnable(ग्रन्थिः_STATE(ksCurThread))) {
            handleFault(ग्रन्थिः_STATE(ksCurThread));
        }
    }
}

शून्य VGICMaintenance(शून्य)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ifdef CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    जइ (thread_state_get_tcbQueued(ग्रन्थिः_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ग्रन्थिः_STATE(ksCurThread))) {
        प्रत्यागम;
    }
#endif

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    जइ (!ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
        printf("Received VGIC maintenance without active VCPU!\n");
        प्रत्यागम;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    जइ (flags & VGIC_MISR_EOI) {
        पूर्णाङ्क irq_idx;
        जइ (eisr0) {
            irq_idx = ctzl(eisr0);
        } अण्णहा जइ (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } अण्णहा {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        जइ (irq_idx == -1  || irq_idx >= gic_vcpu_अङ्कः_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } अण्णहा {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            विकल्प (virq_get_virqType(virq)) {
            पक्ष virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                भङ्ग;
            पक्ष virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                भङ्ग;
            पक्ष virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                भङ्ग;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            प्रतिज्ञा(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) != रिक्त && ARCH_ग्रन्थिः_STATE(armHSVCPUActive));
            जइ (ARCH_ग्रन्थिः_STATE(armHSCurVCPU) != रिक्त && ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
                ARCH_ग्रन्थिः_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } अण्णहा {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } अण्णहा {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    प्रतिज्ञा(isRunnable(ग्रन्थिः_STATE(ksCurThread)));
    जइ (isRunnable(ग्रन्थिः_STATE(ksCurThread))) {
        handleFault(ग्रन्थिः_STATE(ksCurThread));
    }
}

शून्य vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#endif
}

शून्य vcpu_switch(vcpu_t *new)
{
    जइ (likely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) != new)) {
        जइ (unlikely(new != रिक्त)) {
            जइ (unlikely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) != रिक्त)) {
                vcpu_save(ARCH_ग्रन्थिः_STATE(armHSCurVCPU), ARCH_ग्रन्थिः_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_ग्रन्थिः_STATE(armHSCurVCPU) = new;
            ARCH_ग्रन्थिः_STATE(armHSVCPUActive) = true;
        } अण्णहा जइ (unlikely(ARCH_ग्रन्थिः_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_ग्रन्थिः_STATE(armHSCurVCPU)->vcpuTCB);
#endif
            vcpu_disable(ARCH_ग्रन्थिः_STATE(armHSCurVCPU));
            ARCH_ग्रन्थिः_STATE(armHSVCPUActive) = false;
        }
    } अण्णहा जइ (likely(!ARCH_ग्रन्थिः_STATE(armHSVCPUActive) && new != रिक्त)) {
        isb();
        vcpu_enable(new);
        ARCH_ग्रन्थिः_STATE(armHSVCPUActive) = true;
    }
}

स्थिर शून्य vcpu_invalidate_active(शून्य)
{
    जइ (ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
        vcpu_disable(रिक्त);
        ARCH_ग्रन्थिः_STATE(armHSVCPUActive) = false;
    }
    ARCH_ग्रन्थिः_STATE(armHSCurVCPU) = रिक्त;
}

शून्य vcpu_flush(शून्य)
{
    जइ (ARCH_ग्रन्थिः_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_ग्रन्थिः_STATE(armHSCurVCPU), ARCH_ग्रन्थिः_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

शून्य vcpu_flush_if_current(tcb_t *tptr)
{
    जइ (tptr->tcbArch.tcbVCPU == ARCH_ग्रन्थिः_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

शून्य vcpu_finalise(vcpu_t *vcpu)
{
    जइ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

शून्य associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    जइ (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    जइ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    जइ (tcb == ग्रन्थिः_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

शून्य dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    जइ (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    जइ (vcpu == ARCH_ग्रन्थिः_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = रिक्त;
    vcpu->vcpuTCB = रिक्त;
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#endif

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ifdef CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#अण्णहा
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#endif
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t मानम्)
{
    writeVCPUReg(vcpu, field, मानम्);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t मानम्;
    जइ (length < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, buffer);
    मानम् = getSyscallArg(1, buffer);
    जइ (field >= se_L4_VCPUReg_अङ्कः) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, मानम्);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    word_t मानम् = readVCPUReg(vcpu, field);
    जइ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned पूर्णाङ्क length = setMR(thread, ipcBuffer, 0, मानम्);
        setRegister(thread, सन्देशः_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    जइ (length < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);

    जइ (field >= se_L4_VCPUReg_अङ्कः) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    जइ (likely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
#ifdef ENABLE_SMP_SUPPORT
    } अण्णहा जइ (vcpu->vcpuTCB != रिक्त && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, index, virq.शब्दाः[0],
                       vcpu->vcpuTCB->tcbAffinity);
#endif /* CONFIG_ENABLE_SMP */
    } अण्णहा {
        vcpu->vgic.lr[index] = virq;
    }

    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t length, word_t *buffer)
{
    word_t vid, priority, group, index;
    vcpu_t *vcpu;
#ifdef CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    जइ (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = (mr0 >> 32) & 0xff;
#अण्णहा
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    जइ (length < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    mr1 = getSyscallArg(1, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = mr1 & 0xff;
#endif

    /* Check IRQ parameters */
    जइ (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        current_syscall_error.type = seL4_RangeError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    जइ (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_Argument_सङ्ख्या = 2;
        current_syscall_error.type = seL4_RangeError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    जइ (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_Argument_सङ्ख्या = 3;
        current_syscall_error.type = seL4_RangeError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    जइ (index >= gic_vcpu_अङ्कः_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_अङ्कः_list_regs - 1;
        current_syscall_error.invalid_Argument_सङ्ख्या = 4;
        current_syscall_error.type = seL4_RangeError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    जइ (virq_get_virqType(vcpu->vgic.lr[index]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeVCPUInjectIRQ(vcpu, index, virq);
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
    विकल्प (label) {
    पक्ष ARMVCPUSetTCB:
        प्रत्यागम decodeVCPUSetTCB(cap);
    पक्ष ARMVCPUReadReg:
        प्रत्यागम decodeVCPUReadReg(cap, length, call, buffer);
    पक्ष ARMVCPUWriteReg:
        प्रत्यागम decodeVCPUWriteReg(cap, length, buffer);
    पक्ष ARMVCPUInjectIRQ:
        प्रत्यागम decodeVCPUInjectIRQ(cap, length, buffer);
    पक्ष ARMVCPUAckVPPI:
        प्रत्यागम decodeVCPUAckVPPI(cap, length, buffer);
    स्वभाव:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    जइ (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, buffer);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    जइ (status != EXCEPTION_NONE) {
        प्रत्यागम status;
    }

    VPPIEventIRQ_t vppi = irqVPPIEventIndex(irq);
    जइ (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    जइ (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    जइ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    प्रत्यागम EXCEPTION_NONE;
}


शून्य handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        जइ (armv_handleVCPUFault(hsr))
        {
            प्रत्यागम;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(ग्रन्थिः_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#ifdef ENABLE_SMP_SUPPORT
शून्य handleVCPUInjectInterruptIPI(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    जइ (likely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
    } अण्णहा {
        vcpu->vgic.lr[index] = virq;
    }
}
#endif /* ENABLE_SMP_SUPPORT */

#endif
