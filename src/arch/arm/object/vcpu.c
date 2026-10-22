/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#അന്തര്ഭാവ <arch/object/vcpu.h>
#അന്തര്ഭാവ <armv/vcpu.h>
#അന്തര്ഭാവ <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#അന്തര്ഭാവ <arch/machine/debug_conf.h>
#അന്തര്ഭാവ <drivers/timer/arm_generic.h>
#അന്തര്ഭാവ <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE ശൂന്യം vcpu_boot_init(ശൂന്യം)
{
    armv_vcpu_boot_init();
    gic_vcpu_അക്കം_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    എങ്കിൽ (gic_vcpu_അക്കം_list_regs > GIC_VCPU_MAX_അക്കം_LR) {
        printf("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_അക്കം_list_regs = GIC_VCPU_MAX_അക്കം_LR;
    }
    vcpu_disable(ശൂന്യ);
    ARCH_കണ്ണി_STATE(armHSCurVCPU) = ശൂന്യ;
    ARCH_കണ്ണി_STATE(armHSVCPUActive) = false;

}

സ്ഥിതം ശൂന്യം vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_അക്കം;

    പ്രതിജ്ഞാ(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    എങ്കിൽ (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_അക്കം = gic_vcpu_അക്കം_list_regs;
    വേണ്ടി (i = 0; i < lr_അക്കം; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


സ്ഥിതം word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    എങ്കിൽ (likely(ARCH_കണ്ണി_STATE(armHSCurVCPU) == vcpu)) {
        എങ്കിൽ (vcpu_reg_saved_when_disabled(field) && !ARCH_കണ്ണി_STATE(armHSVCPUActive)) {
            തിരികെ vcpu_read_reg(vcpu, field);
        } അല്ലെങ്കിൽ {
            തിരികെ vcpu_hw_read_reg(field);
        }
    } അല്ലെങ്കിൽ {
        തിരികെ vcpu_read_reg(vcpu, field);
    }
}

സ്ഥിതം ശൂന്യം writeVCPUReg(vcpu_t *vcpu, word_t field, word_t മൂല്യം)
{
    എങ്കിൽ (likely(ARCH_കണ്ണി_STATE(armHSCurVCPU) == vcpu)) {
        എങ്കിൽ (vcpu_reg_saved_when_disabled(field) && !ARCH_കണ്ണി_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, മൂല്യം);
        } അല്ലെങ്കിൽ {
            vcpu_hw_write_reg(field, മൂല്യം);
        }
    } അല്ലെങ്കിൽ {
        vcpu_write_reg(vcpu, field, മൂല്യം);
    }
}

ശൂന്യം vcpu_restore(vcpu_t *vcpu)
{
    പ്രതിജ്ഞാ(vcpu);
    word_t i;
    word_t lr_അക്കം;
    /* Turn off the VGIC */
    set_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    set_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    set_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_അക്കം = gic_vcpu_അക്കം_list_regs;
    വേണ്ടി (i = 0; i < lr_അക്കം; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

ശൂന്യം VPPIEvent(irq_t irq)
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
    എങ്കിൽ (thread_state_get_tcbQueued(കണ്ണി_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(കണ്ണി_STATE(ksCurThread))) {
        തിരികെ;
    }
#endif

    എങ്കിൽ (ARCH_കണ്ണി_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        പ്രതിജ്ഞാ(irqVPPIEventIndex(irq) != VPPIEventIRQ_invalid);
        ARCH_കണ്ണി_STATE(armHSCurVCPU)->vppi_masked[irqVPPIEventIndex(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        പ്രതിജ്ഞാ(isRunnable(കണ്ണി_STATE(ksCurThread)));
        എങ്കിൽ (isRunnable(കണ്ണി_STATE(ksCurThread))) {
            handleFault(കണ്ണി_STATE(ksCurThread));
        }
    }
}

ശൂന്യം VGICMaintenance(ശൂന്യം)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ifdef CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    എങ്കിൽ (thread_state_get_tcbQueued(കണ്ണി_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(കണ്ണി_STATE(ksCurThread))) {
        തിരികെ;
    }
#endif

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    എങ്കിൽ (!ARCH_കണ്ണി_STATE(armHSVCPUActive)) {
        printf("Received VGIC maintenance without active VCPU!\n");
        തിരികെ;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    എങ്കിൽ (flags & VGIC_MISR_EOI) {
        പൂർണ്ണസംഖ്യ irq_idx;
        എങ്കിൽ (eisr0) {
            irq_idx = ctzl(eisr0);
        } അല്ലെങ്കിൽ എങ്കിൽ (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } അല്ലെങ്കിൽ {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        എങ്കിൽ (irq_idx == -1  || irq_idx >= gic_vcpu_അക്കം_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } അല്ലെങ്കിൽ {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            തിരഞ്ഞെടുപ്പ് (virq_get_virqType(virq)) {
            അവസ്ഥ virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                നിർത്തുക;
            അവസ്ഥ virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                നിർത്തുക;
            അവസ്ഥ virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                നിർത്തുക;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            പ്രതിജ്ഞാ(ARCH_കണ്ണി_STATE(armHSCurVCPU) != ശൂന്യ && ARCH_കണ്ണി_STATE(armHSVCPUActive));
            എങ്കിൽ (ARCH_കണ്ണി_STATE(armHSCurVCPU) != ശൂന്യ && ARCH_കണ്ണി_STATE(armHSVCPUActive)) {
                ARCH_കണ്ണി_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } അല്ലെങ്കിൽ {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } അല്ലെങ്കിൽ {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    പ്രതിജ്ഞാ(isRunnable(കണ്ണി_STATE(ksCurThread)));
    എങ്കിൽ (isRunnable(കണ്ണി_STATE(ksCurThread))) {
        handleFault(കണ്ണി_STATE(ksCurThread));
    }
}

ശൂന്യം vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#endif
}

ശൂന്യം vcpu_switch(vcpu_t *new)
{
    എങ്കിൽ (likely(ARCH_കണ്ണി_STATE(armHSCurVCPU) != new)) {
        എങ്കിൽ (unlikely(new != ശൂന്യ)) {
            എങ്കിൽ (unlikely(ARCH_കണ്ണി_STATE(armHSCurVCPU) != ശൂന്യ)) {
                vcpu_save(ARCH_കണ്ണി_STATE(armHSCurVCPU), ARCH_കണ്ണി_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_കണ്ണി_STATE(armHSCurVCPU) = new;
            ARCH_കണ്ണി_STATE(armHSVCPUActive) = true;
        } അല്ലെങ്കിൽ എങ്കിൽ (unlikely(ARCH_കണ്ണി_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_കണ്ണി_STATE(armHSCurVCPU)->vcpuTCB);
#endif
            vcpu_disable(ARCH_കണ്ണി_STATE(armHSCurVCPU));
            ARCH_കണ്ണി_STATE(armHSVCPUActive) = false;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ (likely(!ARCH_കണ്ണി_STATE(armHSVCPUActive) && new != ശൂന്യ)) {
        isb();
        vcpu_enable(new);
        ARCH_കണ്ണി_STATE(armHSVCPUActive) = true;
    }
}

സ്ഥിതം ശൂന്യം vcpu_invalidate_active(ശൂന്യം)
{
    എങ്കിൽ (ARCH_കണ്ണി_STATE(armHSVCPUActive)) {
        vcpu_disable(ശൂന്യ);
        ARCH_കണ്ണി_STATE(armHSVCPUActive) = false;
    }
    ARCH_കണ്ണി_STATE(armHSCurVCPU) = ശൂന്യ;
}

ശൂന്യം vcpu_flush(ശൂന്യം)
{
    എങ്കിൽ (ARCH_കണ്ണി_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_കണ്ണി_STATE(armHSCurVCPU), ARCH_കണ്ണി_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

ശൂന്യം vcpu_flush_if_current(tcb_t *tptr)
{
    എങ്കിൽ (tptr->tcbArch.tcbVCPU == ARCH_കണ്ണി_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

ശൂന്യം vcpu_finalise(vcpu_t *vcpu)
{
    എങ്കിൽ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

ശൂന്യം associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    എങ്കിൽ (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    എങ്കിൽ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    എങ്കിൽ (tcb == കണ്ണി_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

ശൂന്യം dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    എങ്കിൽ (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    എങ്കിൽ (vcpu == ARCH_കണ്ണി_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = ശൂന്യ;
    vcpu->vcpuTCB = ശൂന്യ;
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#endif

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ifdef CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#അല്ലെങ്കിൽ
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#endif
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t മൂല്യം)
{
    writeVCPUReg(vcpu, field, മൂല്യം);
    തിരികെ EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t മൂല്യം;
    എങ്കിൽ (length < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, buffer);
    മൂല്യം = getSyscallArg(1, buffer);
    എങ്കിൽ (field >= se_L4_VCPUReg_അക്കം) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, മൂല്യം);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = കണ്ണി_STATE(ksCurThread);
    word_t മൂല്യം = readVCPUReg(vcpu, field);
    എങ്കിൽ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned പൂർണ്ണസംഖ്യ length = setMR(thread, ipcBuffer, 0, മൂല്യം);
        setRegister(thread, സന്ദേശം_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Running);
    തിരികെ EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    എങ്കിൽ (length < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);

    എങ്കിൽ (field >= se_L4_VCPUReg_അക്കം) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    എങ്കിൽ (likely(ARCH_കണ്ണി_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
#ifdef ENABLE_SMP_SUPPORT
    } അല്ലെങ്കിൽ എങ്കിൽ (vcpu->vcpuTCB != ശൂന്യ && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, index, virq.വാക്കുകൾ[0],
                       vcpu->vcpuTCB->tcbAffinity);
#endif /* CONFIG_ENABLE_SMP */
    } അല്ലെങ്കിൽ {
        vcpu->vgic.lr[index] = virq;
    }

    തിരികെ EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t length, word_t *buffer)
{
    word_t vid, priority, group, index;
    vcpu_t *vcpu;
#ifdef CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    എങ്കിൽ (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = (mr0 >> 32) & 0xff;
#അല്ലെങ്കിൽ
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    എങ്കിൽ (length < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    mr1 = getSyscallArg(1, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = mr1 & 0xff;
#endif

    /* Check IRQ parameters */
    എങ്കിൽ (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_Argument_സംഖ്യ = 1;
        current_syscall_error.type = seL4_RangeError;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    എങ്കിൽ (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_Argument_സംഖ്യ = 2;
        current_syscall_error.type = seL4_RangeError;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    എങ്കിൽ (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_Argument_സംഖ്യ = 3;
        current_syscall_error.type = seL4_RangeError;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    എങ്കിൽ (index >= gic_vcpu_അക്കം_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_അക്കം_list_regs - 1;
        current_syscall_error.invalid_Argument_സംഖ്യ = 4;
        current_syscall_error.type = seL4_RangeError;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    എങ്കിൽ (virq_get_virqType(vcpu->vgic.lr[index]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeVCPUInjectIRQ(vcpu, index, virq);
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
    തിരഞ്ഞെടുപ്പ് (label) {
    അവസ്ഥ ARMVCPUSetTCB:
        തിരികെ decodeVCPUSetTCB(cap);
    അവസ്ഥ ARMVCPUReadReg:
        തിരികെ decodeVCPUReadReg(cap, length, call, buffer);
    അവസ്ഥ ARMVCPUWriteReg:
        തിരികെ decodeVCPUWriteReg(cap, length, buffer);
    അവസ്ഥ ARMVCPUInjectIRQ:
        തിരികെ decodeVCPUInjectIRQ(cap, length, buffer);
    അവസ്ഥ ARMVCPUAckVPPI:
        തിരികെ decodeVCPUAckVPPI(cap, length, buffer);
    സ്ഥിരസ്ഥിതി:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    എങ്കിൽ (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, buffer);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    എങ്കിൽ (status != EXCEPTION_NONE) {
        തിരികെ status;
    }

    VPPIEventIRQ_t vppi = irqVPPIEventIndex(irq);
    എങ്കിൽ (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 0;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    തിരികെ EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    എങ്കിൽ (current_extra_caps.excaprefs[0] == ശൂന്യ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    എങ്കിൽ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    തിരികെ EXCEPTION_NONE;
}


ശൂന്യം handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        എങ്കിൽ (armv_handleVCPUFault(hsr))
        {
            തിരികെ;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(കണ്ണി_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#ifdef ENABLE_SMP_SUPPORT
ശൂന്യം handleVCPUInjectInterruptIPI(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    എങ്കിൽ (likely(ARCH_കണ്ണി_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
    } അല്ലെങ്കിൽ {
        vcpu->vgic.lr[index] = virq;
    }
}
#endif /* ENABLE_SMP_SUPPORT */

#endif
