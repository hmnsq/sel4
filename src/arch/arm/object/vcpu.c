/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#ضمن <arch/object/vcpu.h>
#ضمن <armv/vcpu.h>
#ضمن <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#ضمن <arch/machine/debug_conf.h>
#ضمن <drivers/timer/arm_generic.h>
#ضمن <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE فراغ vcpu_boot_init(فراغ)
{
    armv_vcpu_boot_init();
    gic_vcpu_رقم_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    إذا (gic_vcpu_رقم_list_regs > GIC_VCPU_MAX_رقم_LR) {
        printf("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_رقم_list_regs = GIC_VCPU_MAX_رقم_LR;
    }
    vcpu_disable(عدم);
    ARCH_عقدة_STATE(armHSCurVCPU) = عدم;
    ARCH_عقدة_STATE(armHSVCPUActive) = false;

}

ساكن فراغ vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_رقم;

    أكد(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    إذا (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_رقم = gic_vcpu_رقم_list_regs;
    لكل (i = 0; i < lr_رقم; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


ساكن word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    إذا (likely(ARCH_عقدة_STATE(armHSCurVCPU) == vcpu)) {
        إذا (vcpu_reg_saved_when_disabled(field) && !ARCH_عقدة_STATE(armHSVCPUActive)) {
            أرجع vcpu_read_reg(vcpu, field);
        } وإلا {
            أرجع vcpu_hw_read_reg(field);
        }
    } وإلا {
        أرجع vcpu_read_reg(vcpu, field);
    }
}

ساكن فراغ writeVCPUReg(vcpu_t *vcpu, word_t field, word_t قيمة)
{
    إذا (likely(ARCH_عقدة_STATE(armHSCurVCPU) == vcpu)) {
        إذا (vcpu_reg_saved_when_disabled(field) && !ARCH_عقدة_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, قيمة);
        } وإلا {
            vcpu_hw_write_reg(field, قيمة);
        }
    } وإلا {
        vcpu_write_reg(vcpu, field, قيمة);
    }
}

فراغ vcpu_restore(vcpu_t *vcpu)
{
    أكد(vcpu);
    word_t i;
    word_t lr_رقم;
    /* Turn off the VGIC */
    set_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    set_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    set_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_رقم = gic_vcpu_رقم_list_regs;
    لكل (i = 0; i < lr_رقم; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

فراغ VPPIEvent(irq_t irq)
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
    إذا (thread_state_get_tcbQueued(عقدة_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(عقدة_STATE(ksCurThread))) {
        أرجع;
    }
#endif

    إذا (ARCH_عقدة_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        أكد(irqVPPIEventIndex(irq) != VPPIEventIRQ_invalid);
        ARCH_عقدة_STATE(armHSCurVCPU)->vppi_masked[irqVPPIEventIndex(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        أكد(isRunnable(عقدة_STATE(ksCurThread)));
        إذا (isRunnable(عقدة_STATE(ksCurThread))) {
            handleFault(عقدة_STATE(ksCurThread));
        }
    }
}

فراغ VGICMaintenance(فراغ)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ifdef CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    إذا (thread_state_get_tcbQueued(عقدة_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(عقدة_STATE(ksCurThread))) {
        أرجع;
    }
#endif

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    إذا (!ARCH_عقدة_STATE(armHSVCPUActive)) {
        printf("Received VGIC maintenance without active VCPU!\n");
        أرجع;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    إذا (flags & VGIC_MISR_EOI) {
        صحيح irq_idx;
        إذا (eisr0) {
            irq_idx = ctzl(eisr0);
        } وإلا إذا (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } وإلا {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        إذا (irq_idx == -1  || irq_idx >= gic_vcpu_رقم_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } وإلا {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            بدل (virq_get_virqType(virq)) {
            حالة virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                اكسر;
            حالة virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                اكسر;
            حالة virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                اكسر;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            أكد(ARCH_عقدة_STATE(armHSCurVCPU) != عدم && ARCH_عقدة_STATE(armHSVCPUActive));
            إذا (ARCH_عقدة_STATE(armHSCurVCPU) != عدم && ARCH_عقدة_STATE(armHSVCPUActive)) {
                ARCH_عقدة_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } وإلا {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } وإلا {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    أكد(isRunnable(عقدة_STATE(ksCurThread)));
    إذا (isRunnable(عقدة_STATE(ksCurThread))) {
        handleFault(عقدة_STATE(ksCurThread));
    }
}

فراغ vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#endif
}

فراغ vcpu_switch(vcpu_t *new)
{
    إذا (likely(ARCH_عقدة_STATE(armHSCurVCPU) != new)) {
        إذا (unlikely(new != عدم)) {
            إذا (unlikely(ARCH_عقدة_STATE(armHSCurVCPU) != عدم)) {
                vcpu_save(ARCH_عقدة_STATE(armHSCurVCPU), ARCH_عقدة_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_عقدة_STATE(armHSCurVCPU) = new;
            ARCH_عقدة_STATE(armHSVCPUActive) = true;
        } وإلا إذا (unlikely(ARCH_عقدة_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_عقدة_STATE(armHSCurVCPU)->vcpuTCB);
#endif
            vcpu_disable(ARCH_عقدة_STATE(armHSCurVCPU));
            ARCH_عقدة_STATE(armHSVCPUActive) = false;
        }
    } وإلا إذا (likely(!ARCH_عقدة_STATE(armHSVCPUActive) && new != عدم)) {
        isb();
        vcpu_enable(new);
        ARCH_عقدة_STATE(armHSVCPUActive) = true;
    }
}

ساكن فراغ vcpu_invalidate_active(فراغ)
{
    إذا (ARCH_عقدة_STATE(armHSVCPUActive)) {
        vcpu_disable(عدم);
        ARCH_عقدة_STATE(armHSVCPUActive) = false;
    }
    ARCH_عقدة_STATE(armHSCurVCPU) = عدم;
}

فراغ vcpu_flush(فراغ)
{
    إذا (ARCH_عقدة_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_عقدة_STATE(armHSCurVCPU), ARCH_عقدة_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

فراغ vcpu_flush_if_current(tcb_t *tptr)
{
    إذا (tptr->tcbArch.tcbVCPU == ARCH_عقدة_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

فراغ vcpu_finalise(vcpu_t *vcpu)
{
    إذا (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

فراغ associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    إذا (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    إذا (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    إذا (tcb == عقدة_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

فراغ dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    إذا (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    إذا (vcpu == ARCH_عقدة_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = عدم;
    vcpu->vcpuTCB = عدم;
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#endif

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ifdef CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#وإلا
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#endif
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t قيمة)
{
    writeVCPUReg(vcpu, field, قيمة);
    أرجع EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t قيمة;
    إذا (length < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, buffer);
    قيمة = getSyscallArg(1, buffer);
    إذا (field >= se_L4_VCPUReg_رقم) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, قيمة);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = عقدة_STATE(ksCurThread);
    word_t قيمة = readVCPUReg(vcpu, field);
    إذا (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned صحيح length = setMR(thread, ipcBuffer, 0, قيمة);
        setRegister(thread, رسالة_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    إذا (length < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);

    إذا (field >= se_L4_VCPUReg_رقم) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_رقم = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    إذا (likely(ARCH_عقدة_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
#ifdef ENABLE_SMP_SUPPORT
    } وإلا إذا (vcpu->vcpuTCB != عدم && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, index, virq.كلمات[0],
                       vcpu->vcpuTCB->tcbAffinity);
#endif /* CONFIG_ENABLE_SMP */
    } وإلا {
        vcpu->vgic.lr[index] = virq;
    }

    أرجع EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t length, word_t *buffer)
{
    word_t vid, priority, group, index;
    vcpu_t *vcpu;
#ifdef CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    إذا (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = (mr0 >> 32) & 0xff;
#وإلا
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    إذا (length < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    mr1 = getSyscallArg(1, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = mr1 & 0xff;
#endif

    /* Check IRQ parameters */
    إذا (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_Argument_رقم = 1;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_Argument_رقم = 2;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_Argument_رقم = 3;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    إذا (index >= gic_vcpu_رقم_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_رقم_list_regs - 1;
        current_syscall_error.invalid_Argument_رقم = 4;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    إذا (virq_get_virqType(vcpu->vgic.lr[index]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUInjectIRQ(vcpu, index, virq);
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
    بدل (label) {
    حالة ARMVCPUSetTCB:
        أرجع decodeVCPUSetTCB(cap);
    حالة ARMVCPUReadReg:
        أرجع decodeVCPUReadReg(cap, length, call, buffer);
    حالة ARMVCPUWriteReg:
        أرجع decodeVCPUWriteReg(cap, length, buffer);
    حالة ARMVCPUInjectIRQ:
        أرجع decodeVCPUInjectIRQ(cap, length, buffer);
    حالة ARMVCPUAckVPPI:
        أرجع decodeVCPUAckVPPI(cap, length, buffer);
    افتراضي:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    إذا (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, buffer);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    إذا (status != EXCEPTION_NONE) {
        أرجع status;
    }

    VPPIEventIRQ_t vppi = irqVPPIEventIndex(irq);
    إذا (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_رقم = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    أرجع EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    أرجع EXCEPTION_NONE;
}


فراغ handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        إذا (armv_handleVCPUFault(hsr))
        {
            أرجع;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(عقدة_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#ifdef ENABLE_SMP_SUPPORT
فراغ handleVCPUInjectInterruptIPI(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    إذا (likely(ARCH_عقدة_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
    } وإلا {
        vcpu->vgic.lr[index] = virq;
    }
}
#endif /* ENABLE_SMP_SUPPORT */

#endif
