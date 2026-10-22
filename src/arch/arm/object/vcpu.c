/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#ଅନ୍ତର୍ଭାଵ <arch/object/vcpu.h>
#ଅନ୍ତର୍ଭାଵ <armv/vcpu.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#ଅନ୍ତର୍ଭାଵ <arch/machine/debug_conf.h>
#ଅନ୍ତର୍ଭାଵ <drivers/timer/arm_generic.h>
#ଅନ୍ତର୍ଭାଵ <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE ଶୂନ୍ୟ vcpu_boot_init(ଶୂନ୍ୟ)
{
    armv_vcpu_boot_init();
    gic_vcpu_ଅଙ୍କ_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    ଯଦି (gic_vcpu_ଅଙ୍କ_list_regs > GIC_VCPU_MAX_ଅଙ୍କ_LR) {
        printf("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_ଅଙ୍କ_list_regs = GIC_VCPU_MAX_ଅଙ୍କ_LR;
    }
    vcpu_disable(ଖାଲି);
    ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) = ଖାଲି;
    ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive) = false;

}

ସ୍ଥିର ଶୂନ୍ୟ vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_ଅଙ୍କ;

    ପ୍ରତିଜ୍ଞା(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    ଯଦି (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_ଅଙ୍କ = gic_vcpu_ଅଙ୍କ_list_regs;
    ପାଇଁ (i = 0; i < lr_ଅଙ୍କ; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


ସ୍ଥିର word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    ଯଦି (likely(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) == vcpu)) {
        ଯଦି (vcpu_reg_saved_when_disabled(field) && !ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive)) {
            ଫେରନ୍ତୁ vcpu_read_reg(vcpu, field);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ vcpu_hw_read_reg(field);
        }
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ vcpu_read_reg(vcpu, field);
    }
}

ସ୍ଥିର ଶୂନ୍ୟ writeVCPUReg(vcpu_t *vcpu, word_t field, word_t ମୂଲ୍ୟ)
{
    ଯଦି (likely(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) == vcpu)) {
        ଯଦି (vcpu_reg_saved_when_disabled(field) && !ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, ମୂଲ୍ୟ);
        } ଅନ୍ୟଥା {
            vcpu_hw_write_reg(field, ମୂଲ୍ୟ);
        }
    } ଅନ୍ୟଥା {
        vcpu_write_reg(vcpu, field, ମୂଲ୍ୟ);
    }
}

ଶୂନ୍ୟ vcpu_restore(vcpu_t *vcpu)
{
    ପ୍ରତିଜ୍ଞା(vcpu);
    word_t i;
    word_t lr_ଅଙ୍କ;
    /* Turn off the VGIC */
    set_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    set_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    set_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_ଅଙ୍କ = gic_vcpu_ଅଙ୍କ_list_regs;
    ପାଇଁ (i = 0; i < lr_ଅଙ୍କ; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

ଶୂନ୍ୟ VPPIEvent(irq_t irq)
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
    ଯଦି (thread_state_get_tcbQueued(ଗଣ୍ଠି_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ଗଣ୍ଠି_STATE(ksCurThread))) {
        ଫେରନ୍ତୁ;
    }
#endif

    ଯଦି (ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        ପ୍ରତିଜ୍ଞା(irqVPPIEventIndex(irq) != VPPIEventIRQ_invalid);
        ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU)->vppi_masked[irqVPPIEventIndex(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        ପ୍ରତିଜ୍ଞା(isRunnable(ଗଣ୍ଠି_STATE(ksCurThread)));
        ଯଦି (isRunnable(ଗଣ୍ଠି_STATE(ksCurThread))) {
            handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
        }
    }
}

ଶୂନ୍ୟ VGICMaintenance(ଶୂନ୍ୟ)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ifdef CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    ଯଦି (thread_state_get_tcbQueued(ଗଣ୍ଠି_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ଗଣ୍ଠି_STATE(ksCurThread))) {
        ଫେରନ୍ତୁ;
    }
#endif

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    ଯଦି (!ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive)) {
        printf("Received VGIC maintenance without active VCPU!\n");
        ଫେରନ୍ତୁ;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    ଯଦି (flags & VGIC_MISR_EOI) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq_idx;
        ଯଦି (eisr0) {
            irq_idx = ctzl(eisr0);
        } ଅନ୍ୟଥା ଯଦି (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } ଅନ୍ୟଥା {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        ଯଦି (irq_idx == -1  || irq_idx >= gic_vcpu_ଅଙ୍କ_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } ଅନ୍ୟଥା {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            ବାଛ (virq_get_virqType(virq)) {
            ସ୍ଥିତି virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                ଭାଙ୍ଗ;
            ସ୍ଥିତି virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                ଭାଙ୍ଗ;
            ସ୍ଥିତି virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                ଭାଙ୍ଗ;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            ପ୍ରତିଜ୍ଞା(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) != ଖାଲି && ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive));
            ଯଦି (ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) != ଖାଲି && ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive)) {
                ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } ଅନ୍ୟଥା {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } ଅନ୍ୟଥା {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    ପ୍ରତିଜ୍ଞା(isRunnable(ଗଣ୍ଠି_STATE(ksCurThread)));
    ଯଦି (isRunnable(ଗଣ୍ଠି_STATE(ksCurThread))) {
        handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
    }
}

ଶୂନ୍ୟ vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#endif
}

ଶୂନ୍ୟ vcpu_switch(vcpu_t *new)
{
    ଯଦି (likely(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) != new)) {
        ଯଦି (unlikely(new != ଖାଲି)) {
            ଯଦି (unlikely(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) != ଖାଲି)) {
                vcpu_save(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU), ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) = new;
            ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive) = true;
        } ଅନ୍ୟଥା ଯଦି (unlikely(ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU)->vcpuTCB);
#endif
            vcpu_disable(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU));
            ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive) = false;
        }
    } ଅନ୍ୟଥା ଯଦି (likely(!ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive) && new != ଖାଲି)) {
        isb();
        vcpu_enable(new);
        ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive) = true;
    }
}

ସ୍ଥିର ଶୂନ୍ୟ vcpu_invalidate_active(ଶୂନ୍ୟ)
{
    ଯଦି (ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive)) {
        vcpu_disable(ଖାଲି);
        ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive) = false;
    }
    ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) = ଖାଲି;
}

ଶୂନ୍ୟ vcpu_flush(ଶୂନ୍ୟ)
{
    ଯଦି (ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU), ARCH_ଗଣ୍ଠି_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

ଶୂନ୍ୟ vcpu_flush_if_current(tcb_t *tptr)
{
    ଯଦି (tptr->tcbArch.tcbVCPU == ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

ଶୂନ୍ୟ vcpu_finalise(vcpu_t *vcpu)
{
    ଯଦି (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

ଶୂନ୍ୟ associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ଯଦି (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    ଯଦି (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    ଯଦି (tcb == ଗଣ୍ଠି_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

ଶୂନ୍ୟ dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ଯଦି (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    ଯଦି (vcpu == ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = ଖାଲି;
    vcpu->vcpuTCB = ଖାଲି;
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#endif

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ifdef CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#ଅନ୍ୟଥା
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#endif
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t ମୂଲ୍ୟ)
{
    writeVCPUReg(vcpu, field, ମୂଲ୍ୟ);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t ମୂଲ୍ୟ;
    ଯଦି (length < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, buffer);
    ମୂଲ୍ୟ = getSyscallArg(1, buffer);
    ଯଦି (field >= se_L4_VCPUReg_ଅଙ୍କ) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, ମୂଲ୍ୟ);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = ଗଣ୍ଠି_STATE(ksCurThread);
    word_t ମୂଲ୍ୟ = readVCPUReg(vcpu, field);
    ଯଦି (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length = setMR(thread, ipcBuffer, 0, ମୂଲ୍ୟ);
        setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Running);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    ଯଦି (length < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);

    ଯଦି (field >= se_L4_VCPUReg_ଅଙ୍କ) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    ଯଦି (likely(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
#ifdef ENABLE_SMP_SUPPORT
    } ଅନ୍ୟଥା ଯଦି (vcpu->vcpuTCB != ଖାଲି && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, index, virq.ଶବ୍ଦଗୁଡ଼ିକ[0],
                       vcpu->vcpuTCB->tcbAffinity);
#endif /* CONFIG_ENABLE_SMP */
    } ଅନ୍ୟଥା {
        vcpu->vgic.lr[index] = virq;
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t length, word_t *buffer)
{
    word_t vid, priority, group, index;
    vcpu_t *vcpu;
#ifdef CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ଯଦି (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = (mr0 >> 32) & 0xff;
#ଅନ୍ୟଥା
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ଯଦି (length < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    mr1 = getSyscallArg(1, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = mr1 & 0xff;
#endif

    /* Check IRQ parameters */
    ଯଦି (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        current_syscall_error.type = seL4_RangeError;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 2;
        current_syscall_error.type = seL4_RangeError;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 3;
        current_syscall_error.type = seL4_RangeError;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    ଯଦି (index >= gic_vcpu_ଅଙ୍କ_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_ଅଙ୍କ_list_regs - 1;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 4;
        current_syscall_error.type = seL4_RangeError;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    ଯଦି (virq_get_virqType(vcpu->vgic.lr[index]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeVCPUInjectIRQ(vcpu, index, virq);
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
    ବାଛ (label) {
    ସ୍ଥିତି ARMVCPUSetTCB:
        ଫେରନ୍ତୁ decodeVCPUSetTCB(cap);
    ସ୍ଥିତି ARMVCPUReadReg:
        ଫେରନ୍ତୁ decodeVCPUReadReg(cap, length, call, buffer);
    ସ୍ଥିତି ARMVCPUWriteReg:
        ଫେରନ୍ତୁ decodeVCPUWriteReg(cap, length, buffer);
    ସ୍ଥିତି ARMVCPUInjectIRQ:
        ଫେରନ୍ତୁ decodeVCPUInjectIRQ(cap, length, buffer);
    ସ୍ଥିତି ARMVCPUAckVPPI:
        ଫେରନ୍ତୁ decodeVCPUAckVPPI(cap, length, buffer);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ଯଦି (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, buffer);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    ଯଦି (status != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ status;
    }

    VPPIEventIRQ_t vppi = irqVPPIEventIndex(irq);
    ଯଦି (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 0;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    ଯଦି (current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ଯଦି (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}


ଶୂନ୍ୟ handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        ଯଦି (armv_handleVCPUFault(hsr))
        {
            ଫେରନ୍ତୁ;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#ifdef ENABLE_SMP_SUPPORT
ଶୂନ୍ୟ handleVCPUInjectInterruptIPI(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    ଯଦି (likely(ARCH_ଗଣ୍ଠି_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
    } ଅନ୍ୟଥା {
        vcpu->vgic.lr[index] = virq;
    }
}
#endif /* ENABLE_SMP_SUPPORT */

#endif
