/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#அந்தர்பாவ <arch/object/vcpu.h>
#அந்தர்பாவ <armv/vcpu.h>
#அந்தர்பாவ <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#அந்தர்பாவ <arch/machine/debug_conf.h>
#அந்தர்பாவ <drivers/timer/arm_generic.h>
#அந்தர்பாவ <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE வெறுமை vcpu_boot_init(வெறுமை)
{
    armv_vcpu_boot_init();
    gic_vcpu_இலக்கம்_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    எனில் (gic_vcpu_இலக்கம்_list_regs > GIC_VCPU_MAX_இலக்கம்_LR) {
        printf("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_இலக்கம்_list_regs = GIC_VCPU_MAX_இலக்கம்_LR;
    }
    vcpu_disable(வெற்று);
    ARCH_முனை_STATE(armHSCurVCPU) = வெற்று;
    ARCH_முனை_STATE(armHSVCPUActive) = false;

}

நிலையான வெறுமை vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_இலக்கம்;

    ப்ரதிஜ்ஞா(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    எனில் (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_இலக்கம் = gic_vcpu_இலக்கம்_list_regs;
    க்கு (i = 0; i < lr_இலக்கம்; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


நிலையான word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    எனில் (likely(ARCH_முனை_STATE(armHSCurVCPU) == vcpu)) {
        எனில் (vcpu_reg_saved_when_disabled(field) && !ARCH_முனை_STATE(armHSVCPUActive)) {
            திரும்பு vcpu_read_reg(vcpu, field);
        } இல்லையெனில் {
            திரும்பு vcpu_hw_read_reg(field);
        }
    } இல்லையெனில் {
        திரும்பு vcpu_read_reg(vcpu, field);
    }
}

நிலையான வெறுமை writeVCPUReg(vcpu_t *vcpu, word_t field, word_t மதிப்பு)
{
    எனில் (likely(ARCH_முனை_STATE(armHSCurVCPU) == vcpu)) {
        எனில் (vcpu_reg_saved_when_disabled(field) && !ARCH_முனை_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, மதிப்பு);
        } இல்லையெனில் {
            vcpu_hw_write_reg(field, மதிப்பு);
        }
    } இல்லையெனில் {
        vcpu_write_reg(vcpu, field, மதிப்பு);
    }
}

வெறுமை vcpu_restore(vcpu_t *vcpu)
{
    ப்ரதிஜ்ஞா(vcpu);
    word_t i;
    word_t lr_இலக்கம்;
    /* Turn off the VGIC */
    set_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    set_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    set_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_இலக்கம் = gic_vcpu_இலக்கம்_list_regs;
    க்கு (i = 0; i < lr_இலக்கம்; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

வெறுமை VPPIEvent(irq_t irq)
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
    எனில் (thread_state_get_tcbQueued(முனை_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(முனை_STATE(ksCurThread))) {
        திரும்பு;
    }
#endif

    எனில் (ARCH_முனை_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        ப்ரதிஜ்ஞா(irqVPPIEventIndex(irq) != VPPIEventIRQ_invalid);
        ARCH_முனை_STATE(armHSCurVCPU)->vppi_masked[irqVPPIEventIndex(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        ப்ரதிஜ்ஞா(isRunnable(முனை_STATE(ksCurThread)));
        எனில் (isRunnable(முனை_STATE(ksCurThread))) {
            handleFault(முனை_STATE(ksCurThread));
        }
    }
}

வெறுமை VGICMaintenance(வெறுமை)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ifdef CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    எனில் (thread_state_get_tcbQueued(முனை_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(முனை_STATE(ksCurThread))) {
        திரும்பு;
    }
#endif

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    எனில் (!ARCH_முனை_STATE(armHSVCPUActive)) {
        printf("Received VGIC maintenance without active VCPU!\n");
        திரும்பு;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    எனில் (flags & VGIC_MISR_EOI) {
        முழு irq_idx;
        எனில் (eisr0) {
            irq_idx = ctzl(eisr0);
        } இல்லையெனில் எனில் (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } இல்லையெனில் {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        எனில் (irq_idx == -1  || irq_idx >= gic_vcpu_இலக்கம்_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } இல்லையெனில் {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            தேர்வு (virq_get_virqType(virq)) {
            நிலை virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                நிறுத்து;
            நிலை virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                நிறுத்து;
            நிலை virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                நிறுத்து;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            ப்ரதிஜ்ஞா(ARCH_முனை_STATE(armHSCurVCPU) != வெற்று && ARCH_முனை_STATE(armHSVCPUActive));
            எனில் (ARCH_முனை_STATE(armHSCurVCPU) != வெற்று && ARCH_முனை_STATE(armHSVCPUActive)) {
                ARCH_முனை_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } இல்லையெனில் {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } இல்லையெனில் {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    ப்ரதிஜ்ஞா(isRunnable(முனை_STATE(ksCurThread)));
    எனில் (isRunnable(முனை_STATE(ksCurThread))) {
        handleFault(முனை_STATE(ksCurThread));
    }
}

வெறுமை vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#endif
}

வெறுமை vcpu_switch(vcpu_t *new)
{
    எனில் (likely(ARCH_முனை_STATE(armHSCurVCPU) != new)) {
        எனில் (unlikely(new != வெற்று)) {
            எனில் (unlikely(ARCH_முனை_STATE(armHSCurVCPU) != வெற்று)) {
                vcpu_save(ARCH_முனை_STATE(armHSCurVCPU), ARCH_முனை_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_முனை_STATE(armHSCurVCPU) = new;
            ARCH_முனை_STATE(armHSVCPUActive) = true;
        } இல்லையெனில் எனில் (unlikely(ARCH_முனை_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_முனை_STATE(armHSCurVCPU)->vcpuTCB);
#endif
            vcpu_disable(ARCH_முனை_STATE(armHSCurVCPU));
            ARCH_முனை_STATE(armHSVCPUActive) = false;
        }
    } இல்லையெனில் எனில் (likely(!ARCH_முனை_STATE(armHSVCPUActive) && new != வெற்று)) {
        isb();
        vcpu_enable(new);
        ARCH_முனை_STATE(armHSVCPUActive) = true;
    }
}

நிலையான வெறுமை vcpu_invalidate_active(வெறுமை)
{
    எனில் (ARCH_முனை_STATE(armHSVCPUActive)) {
        vcpu_disable(வெற்று);
        ARCH_முனை_STATE(armHSVCPUActive) = false;
    }
    ARCH_முனை_STATE(armHSCurVCPU) = வெற்று;
}

வெறுமை vcpu_flush(வெறுமை)
{
    எனில் (ARCH_முனை_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_முனை_STATE(armHSCurVCPU), ARCH_முனை_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

வெறுமை vcpu_flush_if_current(tcb_t *tptr)
{
    எனில் (tptr->tcbArch.tcbVCPU == ARCH_முனை_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

வெறுமை vcpu_finalise(vcpu_t *vcpu)
{
    எனில் (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

வெறுமை associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    எனில் (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    எனில் (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    எனில் (tcb == முனை_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

வெறுமை dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    எனில் (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    எனில் (vcpu == ARCH_முனை_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = வெற்று;
    vcpu->vcpuTCB = வெற்று;
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#endif

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ifdef CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#இல்லையெனில்
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#endif
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t மதிப்பு)
{
    writeVCPUReg(vcpu, field, மதிப்பு);
    திரும்பு EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t மதிப்பு;
    எனில் (length < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, buffer);
    மதிப்பு = getSyscallArg(1, buffer);
    எனில் (field >= se_L4_VCPUReg_இலக்கம்) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, மதிப்பு);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = முனை_STATE(ksCurThread);
    word_t மதிப்பு = readVCPUReg(vcpu, field);
    எனில் (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned முழு length = setMR(thread, ipcBuffer, 0, மதிப்பு);
        setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
    திரும்பு EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    எனில் (length < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);

    எனில் (field >= se_L4_VCPUReg_இலக்கம்) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    எனில் (likely(ARCH_முனை_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
#ifdef ENABLE_SMP_SUPPORT
    } இல்லையெனில் எனில் (vcpu->vcpuTCB != வெற்று && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, index, virq.சொற்கள்[0],
                       vcpu->vcpuTCB->tcbAffinity);
#endif /* CONFIG_ENABLE_SMP */
    } இல்லையெனில் {
        vcpu->vgic.lr[index] = virq;
    }

    திரும்பு EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t length, word_t *buffer)
{
    word_t vid, priority, group, index;
    vcpu_t *vcpu;
#ifdef CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    எனில் (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = (mr0 >> 32) & 0xff;
#இல்லையெனில்
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    எனில் (length < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    mr1 = getSyscallArg(1, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = mr1 & 0xff;
#endif

    /* Check IRQ parameters */
    எனில் (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_Argument_எண் = 1;
        current_syscall_error.type = seL4_RangeError;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_Argument_எண் = 2;
        current_syscall_error.type = seL4_RangeError;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_Argument_எண் = 3;
        current_syscall_error.type = seL4_RangeError;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    எனில் (index >= gic_vcpu_இலக்கம்_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_இலக்கம்_list_regs - 1;
        current_syscall_error.invalid_Argument_எண் = 4;
        current_syscall_error.type = seL4_RangeError;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    எனில் (virq_get_virqType(vcpu->vgic.lr[index]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeVCPUInjectIRQ(vcpu, index, virq);
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
    தேர்வு (label) {
    நிலை ARMVCPUSetTCB:
        திரும்பு decodeVCPUSetTCB(cap);
    நிலை ARMVCPUReadReg:
        திரும்பு decodeVCPUReadReg(cap, length, call, buffer);
    நிலை ARMVCPUWriteReg:
        திரும்பு decodeVCPUWriteReg(cap, length, buffer);
    நிலை ARMVCPUInjectIRQ:
        திரும்பு decodeVCPUInjectIRQ(cap, length, buffer);
    நிலை ARMVCPUAckVPPI:
        திரும்பு decodeVCPUAckVPPI(cap, length, buffer);
    இயல்பு:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    எனில் (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, buffer);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    எனில் (status != EXCEPTION_NONE) {
        திரும்பு status;
    }

    VPPIEventIRQ_t vppi = irqVPPIEventIndex(irq);
    எனில் (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 0;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    திரும்பு EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    எனில் (current_extra_caps.excaprefs[0] == வெற்று) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    எனில் (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    திரும்பு EXCEPTION_NONE;
}


வெறுமை handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        எனில் (armv_handleVCPUFault(hsr))
        {
            திரும்பு;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(முனை_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#ifdef ENABLE_SMP_SUPPORT
வெறுமை handleVCPUInjectInterruptIPI(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    எனில் (likely(ARCH_முனை_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
    } இல்லையெனில் {
        vcpu->vgic.lr[index] = virq;
    }
}
#endif /* ENABLE_SMP_SUPPORT */

#endif
