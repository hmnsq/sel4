/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#અન્તર્ભાવ <arch/object/vcpu.h>
#અન્તર્ભાવ <armv/vcpu.h>
#અન્તર્ભાવ <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#અન્તર્ભાવ <arch/machine/debug_conf.h>
#અન્તર્ભાવ <drivers/timer/arm_generic.h>
#અન્તર્ભાવ <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE શૂન્ય vcpu_boot_init(શૂન્ય)
{
    armv_vcpu_boot_init();
    gic_vcpu_અંક_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    જો (gic_vcpu_અંક_list_regs > GIC_VCPU_MAX_અંક_LR) {
        printf("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_અંક_list_regs = GIC_VCPU_MAX_અંક_LR;
    }
    vcpu_disable(ખાલી);
    ARCH_ગાંઠ_STATE(armHSCurVCPU) = ખાલી;
    ARCH_ગાંઠ_STATE(armHSVCPUActive) = false;

}

સ્થિર શૂન્ય vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_અંક;

    પ્રતિજ્ઞા(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    જો (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_અંક = gic_vcpu_અંક_list_regs;
    માટે (i = 0; i < lr_અંક; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


સ્થિર word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    જો (likely(ARCH_ગાંઠ_STATE(armHSCurVCPU) == vcpu)) {
        જો (vcpu_reg_saved_when_disabled(field) && !ARCH_ગાંઠ_STATE(armHSVCPUActive)) {
            પાછા vcpu_read_reg(vcpu, field);
        } નહીંતર {
            પાછા vcpu_hw_read_reg(field);
        }
    } નહીંતર {
        પાછા vcpu_read_reg(vcpu, field);
    }
}

સ્થિર શૂન્ય writeVCPUReg(vcpu_t *vcpu, word_t field, word_t કિંમત)
{
    જો (likely(ARCH_ગાંઠ_STATE(armHSCurVCPU) == vcpu)) {
        જો (vcpu_reg_saved_when_disabled(field) && !ARCH_ગાંઠ_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, કિંમત);
        } નહીંતર {
            vcpu_hw_write_reg(field, કિંમત);
        }
    } નહીંતર {
        vcpu_write_reg(vcpu, field, કિંમત);
    }
}

શૂન્ય vcpu_restore(vcpu_t *vcpu)
{
    પ્રતિજ્ઞા(vcpu);
    word_t i;
    word_t lr_અંક;
    /* Turn off the VGIC */
    set_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    set_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    set_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_અંક = gic_vcpu_અંક_list_regs;
    માટે (i = 0; i < lr_અંક; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

શૂન્ય VPPIEvent(irq_t irq)
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
    જો (thread_state_get_tcbQueued(ગાંઠ_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ગાંઠ_STATE(ksCurThread))) {
        પાછા;
    }
#endif

    જો (ARCH_ગાંઠ_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        પ્રતિજ્ઞા(irqVPPIEventIndex(irq) != VPPIEventIRQ_invalid);
        ARCH_ગાંઠ_STATE(armHSCurVCPU)->vppi_masked[irqVPPIEventIndex(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        પ્રતિજ્ઞા(isRunnable(ગાંઠ_STATE(ksCurThread)));
        જો (isRunnable(ગાંઠ_STATE(ksCurThread))) {
            handleFault(ગાંઠ_STATE(ksCurThread));
        }
    }
}

શૂન્ય VGICMaintenance(શૂન્ય)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ifdef CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    જો (thread_state_get_tcbQueued(ગાંઠ_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ગાંઠ_STATE(ksCurThread))) {
        પાછા;
    }
#endif

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    જો (!ARCH_ગાંઠ_STATE(armHSVCPUActive)) {
        printf("Received VGIC maintenance without active VCPU!\n");
        પાછા;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    જો (flags & VGIC_MISR_EOI) {
        પૂર્ણાંક irq_idx;
        જો (eisr0) {
            irq_idx = ctzl(eisr0);
        } નહીંતર જો (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } નહીંતર {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        જો (irq_idx == -1  || irq_idx >= gic_vcpu_અંક_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } નહીંતર {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            પસંદગી (virq_get_virqType(virq)) {
            સ્થિતિ virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                તોડો;
            સ્થિતિ virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                તોડો;
            સ્થિતિ virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                તોડો;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            પ્રતિજ્ઞા(ARCH_ગાંઠ_STATE(armHSCurVCPU) != ખાલી && ARCH_ગાંઠ_STATE(armHSVCPUActive));
            જો (ARCH_ગાંઠ_STATE(armHSCurVCPU) != ખાલી && ARCH_ગાંઠ_STATE(armHSVCPUActive)) {
                ARCH_ગાંઠ_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } નહીંતર {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } નહીંતર {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    પ્રતિજ્ઞા(isRunnable(ગાંઠ_STATE(ksCurThread)));
    જો (isRunnable(ગાંઠ_STATE(ksCurThread))) {
        handleFault(ગાંઠ_STATE(ksCurThread));
    }
}

શૂન્ય vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#endif
}

શૂન્ય vcpu_switch(vcpu_t *new)
{
    જો (likely(ARCH_ગાંઠ_STATE(armHSCurVCPU) != new)) {
        જો (unlikely(new != ખાલી)) {
            જો (unlikely(ARCH_ગાંઠ_STATE(armHSCurVCPU) != ખાલી)) {
                vcpu_save(ARCH_ગાંઠ_STATE(armHSCurVCPU), ARCH_ગાંઠ_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_ગાંઠ_STATE(armHSCurVCPU) = new;
            ARCH_ગાંઠ_STATE(armHSVCPUActive) = true;
        } નહીંતર જો (unlikely(ARCH_ગાંઠ_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_ગાંઠ_STATE(armHSCurVCPU)->vcpuTCB);
#endif
            vcpu_disable(ARCH_ગાંઠ_STATE(armHSCurVCPU));
            ARCH_ગાંઠ_STATE(armHSVCPUActive) = false;
        }
    } નહીંતર જો (likely(!ARCH_ગાંઠ_STATE(armHSVCPUActive) && new != ખાલી)) {
        isb();
        vcpu_enable(new);
        ARCH_ગાંઠ_STATE(armHSVCPUActive) = true;
    }
}

સ્થિર શૂન્ય vcpu_invalidate_active(શૂન્ય)
{
    જો (ARCH_ગાંઠ_STATE(armHSVCPUActive)) {
        vcpu_disable(ખાલી);
        ARCH_ગાંઠ_STATE(armHSVCPUActive) = false;
    }
    ARCH_ગાંઠ_STATE(armHSCurVCPU) = ખાલી;
}

શૂન્ય vcpu_flush(શૂન્ય)
{
    જો (ARCH_ગાંઠ_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_ગાંઠ_STATE(armHSCurVCPU), ARCH_ગાંઠ_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

શૂન્ય vcpu_flush_if_current(tcb_t *tptr)
{
    જો (tptr->tcbArch.tcbVCPU == ARCH_ગાંઠ_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

શૂન્ય vcpu_finalise(vcpu_t *vcpu)
{
    જો (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

શૂન્ય associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    જો (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    જો (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    જો (tcb == ગાંઠ_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

શૂન્ય dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    જો (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    જો (vcpu == ARCH_ગાંઠ_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = ખાલી;
    vcpu->vcpuTCB = ખાલી;
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#endif

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ifdef CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#નહીંતર
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#endif
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t કિંમત)
{
    writeVCPUReg(vcpu, field, કિંમત);
    પાછા EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t કિંમત;
    જો (length < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, buffer);
    કિંમત = getSyscallArg(1, buffer);
    જો (field >= se_L4_VCPUReg_અંક) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, કિંમત);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);
    word_t કિંમત = readVCPUReg(vcpu, field);
    જો (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned પૂર્ણાંક length = setMR(thread, ipcBuffer, 0, કિંમત);
        setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    પાછા EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    જો (length < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);

    જો (field >= se_L4_VCPUReg_અંક) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    જો (likely(ARCH_ગાંઠ_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
#ifdef ENABLE_SMP_SUPPORT
    } નહીંતર જો (vcpu->vcpuTCB != ખાલી && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, index, virq.શબ્દો[0],
                       vcpu->vcpuTCB->tcbAffinity);
#endif /* CONFIG_ENABLE_SMP */
    } નહીંતર {
        vcpu->vgic.lr[index] = virq;
    }

    પાછા EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t length, word_t *buffer)
{
    word_t vid, priority, group, index;
    vcpu_t *vcpu;
#ifdef CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    જો (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = (mr0 >> 32) & 0xff;
#નહીંતર
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    જો (length < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    mr1 = getSyscallArg(1, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = mr1 & 0xff;
#endif

    /* Check IRQ parameters */
    જો (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_Argument_સંખ્યા = 1;
        current_syscall_error.type = seL4_RangeError;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_Argument_સંખ્યા = 2;
        current_syscall_error.type = seL4_RangeError;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_Argument_સંખ્યા = 3;
        current_syscall_error.type = seL4_RangeError;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    જો (index >= gic_vcpu_અંક_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_અંક_list_regs - 1;
        current_syscall_error.invalid_Argument_સંખ્યા = 4;
        current_syscall_error.type = seL4_RangeError;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    જો (virq_get_virqType(vcpu->vgic.lr[index]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeVCPUInjectIRQ(vcpu, index, virq);
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
    પસંદગી (label) {
    સ્થિતિ ARMVCPUSetTCB:
        પાછા decodeVCPUSetTCB(cap);
    સ્થિતિ ARMVCPUReadReg:
        પાછા decodeVCPUReadReg(cap, length, call, buffer);
    સ્થિતિ ARMVCPUWriteReg:
        પાછા decodeVCPUWriteReg(cap, length, buffer);
    સ્થિતિ ARMVCPUInjectIRQ:
        પાછા decodeVCPUInjectIRQ(cap, length, buffer);
    સ્થિતિ ARMVCPUAckVPPI:
        પાછા decodeVCPUAckVPPI(cap, length, buffer);
    મૂળભૂત:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    જો (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, buffer);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    જો (status != EXCEPTION_NONE) {
        પાછા status;
    }

    VPPIEventIRQ_t vppi = irqVPPIEventIndex(irq);
    જો (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    પાછા EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    જો (current_extra_caps.excaprefs[0] == ખાલી) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    જો (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    પાછા EXCEPTION_NONE;
}


શૂન્ય handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        જો (armv_handleVCPUFault(hsr))
        {
            પાછા;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(ગાંઠ_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#ifdef ENABLE_SMP_SUPPORT
શૂન્ય handleVCPUInjectInterruptIPI(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    જો (likely(ARCH_ગાંઠ_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
    } નહીંતર {
        vcpu->vgic.lr[index] = virq;
    }
}
#endif /* ENABLE_SMP_SUPPORT */

#endif
