/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#شامل <arch/object/vcpu.h>
#شامل <armv/vcpu.h>
#شامل <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#شامل <arch/machine/debug_conf.h>
#شامل <drivers/timer/arm_generic.h>
#شامل <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE تهی vcpu_boot_init(تهی)
{
    armv_vcpu_boot_init();
    gic_vcpu_رقم_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    اگر (gic_vcpu_رقم_list_regs > GIC_VCPU_MAX_رقم_LR) {
        printf("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_رقم_list_regs = GIC_VCPU_MAX_رقم_LR;
    }
    vcpu_disable(پوچ);
    ARCH_گره_STATE(armHSCurVCPU) = پوچ;
    ARCH_گره_STATE(armHSVCPUActive) = false;

}

ایستا تهی vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_رقم;

    أكد(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    اگر (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_رقم = gic_vcpu_رقم_list_regs;
    برای (i = 0; i < lr_رقم; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


ایستا word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    اگر (likely(ARCH_گره_STATE(armHSCurVCPU) == vcpu)) {
        اگر (vcpu_reg_saved_when_disabled(field) && !ARCH_گره_STATE(armHSVCPUActive)) {
            بازگشت vcpu_read_reg(vcpu, field);
        } وگرنه {
            بازگشت vcpu_hw_read_reg(field);
        }
    } وگرنه {
        بازگشت vcpu_read_reg(vcpu, field);
    }
}

ایستا تهی writeVCPUReg(vcpu_t *vcpu, word_t field, word_t مقدار)
{
    اگر (likely(ARCH_گره_STATE(armHSCurVCPU) == vcpu)) {
        اگر (vcpu_reg_saved_when_disabled(field) && !ARCH_گره_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, مقدار);
        } وگرنه {
            vcpu_hw_write_reg(field, مقدار);
        }
    } وگرنه {
        vcpu_write_reg(vcpu, field, مقدار);
    }
}

تهی vcpu_restore(vcpu_t *vcpu)
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
    برای (i = 0; i < lr_رقم; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

تهی VPPIEvent(irq_t irq)
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
    اگر (thread_state_get_tcbQueued(گره_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(گره_STATE(ksCurThread))) {
        بازگشت;
    }
#endif

    اگر (ARCH_گره_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        أكد(irqVPPIEventIndex(irq) != VPPIEventIRQ_invalid);
        ARCH_گره_STATE(armHSCurVCPU)->vppi_masked[irqVPPIEventIndex(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        أكد(isRunnable(گره_STATE(ksCurThread)));
        اگر (isRunnable(گره_STATE(ksCurThread))) {
            handleFault(گره_STATE(ksCurThread));
        }
    }
}

تهی VGICMaintenance(تهی)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ifdef CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    اگر (thread_state_get_tcbQueued(گره_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(گره_STATE(ksCurThread))) {
        بازگشت;
    }
#endif

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    اگر (!ARCH_گره_STATE(armHSVCPUActive)) {
        printf("Received VGIC maintenance without active VCPU!\n");
        بازگشت;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    اگر (flags & VGIC_MISR_EOI) {
        صحیح irq_idx;
        اگر (eisr0) {
            irq_idx = ctzl(eisr0);
        } وگرنه اگر (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } وگرنه {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        اگر (irq_idx == -1  || irq_idx >= gic_vcpu_رقم_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } وگرنه {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            انتخاب (virq_get_virqType(virq)) {
            حالت virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                بشکن;
            حالت virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                بشکن;
            حالت virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                بشکن;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            أكد(ARCH_گره_STATE(armHSCurVCPU) != پوچ && ARCH_گره_STATE(armHSVCPUActive));
            اگر (ARCH_گره_STATE(armHSCurVCPU) != پوچ && ARCH_گره_STATE(armHSVCPUActive)) {
                ARCH_گره_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } وگرنه {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } وگرنه {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    أكد(isRunnable(گره_STATE(ksCurThread)));
    اگر (isRunnable(گره_STATE(ksCurThread))) {
        handleFault(گره_STATE(ksCurThread));
    }
}

تهی vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#endif
}

تهی vcpu_switch(vcpu_t *new)
{
    اگر (likely(ARCH_گره_STATE(armHSCurVCPU) != new)) {
        اگر (unlikely(new != پوچ)) {
            اگر (unlikely(ARCH_گره_STATE(armHSCurVCPU) != پوچ)) {
                vcpu_save(ARCH_گره_STATE(armHSCurVCPU), ARCH_گره_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_گره_STATE(armHSCurVCPU) = new;
            ARCH_گره_STATE(armHSVCPUActive) = true;
        } وگرنه اگر (unlikely(ARCH_گره_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_گره_STATE(armHSCurVCPU)->vcpuTCB);
#endif
            vcpu_disable(ARCH_گره_STATE(armHSCurVCPU));
            ARCH_گره_STATE(armHSVCPUActive) = false;
        }
    } وگرنه اگر (likely(!ARCH_گره_STATE(armHSVCPUActive) && new != پوچ)) {
        isb();
        vcpu_enable(new);
        ARCH_گره_STATE(armHSVCPUActive) = true;
    }
}

ایستا تهی vcpu_invalidate_active(تهی)
{
    اگر (ARCH_گره_STATE(armHSVCPUActive)) {
        vcpu_disable(پوچ);
        ARCH_گره_STATE(armHSVCPUActive) = false;
    }
    ARCH_گره_STATE(armHSCurVCPU) = پوچ;
}

تهی vcpu_flush(تهی)
{
    اگر (ARCH_گره_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_گره_STATE(armHSCurVCPU), ARCH_گره_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

تهی vcpu_flush_if_current(tcb_t *tptr)
{
    اگر (tptr->tcbArch.tcbVCPU == ARCH_گره_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

تهی vcpu_finalise(vcpu_t *vcpu)
{
    اگر (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

تهی associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    اگر (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    اگر (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    اگر (tcb == گره_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

تهی dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    اگر (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    اگر (vcpu == ARCH_گره_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = پوچ;
    vcpu->vcpuTCB = پوچ;
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#endif

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ifdef CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#وگرنه
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#endif
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t مقدار)
{
    writeVCPUReg(vcpu, field, مقدار);
    بازگشت EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t مقدار;
    اگر (length < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, buffer);
    مقدار = getSyscallArg(1, buffer);
    اگر (field >= se_L4_VCPUReg_رقم) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, مقدار);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    word_t مقدار = readVCPUReg(vcpu, field);
    اگر (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned صحیح length = setMR(thread, ipcBuffer, 0, مقدار);
        setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
    بازگشت EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    اگر (length < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);

    اگر (field >= se_L4_VCPUReg_رقم) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    اگر (likely(ARCH_گره_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
#ifdef ENABLE_SMP_SUPPORT
    } وگرنه اگر (vcpu->vcpuTCB != پوچ && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, index, virq.واژگان[0],
                       vcpu->vcpuTCB->tcbAffinity);
#endif /* CONFIG_ENABLE_SMP */
    } وگرنه {
        vcpu->vgic.lr[index] = virq;
    }

    بازگشت EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t length, word_t *buffer)
{
    word_t vid, priority, group, index;
    vcpu_t *vcpu;
#ifdef CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    اگر (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = (mr0 >> 32) & 0xff;
#وگرنه
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    اگر (length < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    mr1 = getSyscallArg(1, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = mr1 & 0xff;
#endif

    /* Check IRQ parameters */
    اگر (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_Argument_عدد = 1;
        current_syscall_error.type = seL4_RangeError;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_Argument_عدد = 2;
        current_syscall_error.type = seL4_RangeError;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_Argument_عدد = 3;
        current_syscall_error.type = seL4_RangeError;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    اگر (index >= gic_vcpu_رقم_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_رقم_list_regs - 1;
        current_syscall_error.invalid_Argument_عدد = 4;
        current_syscall_error.type = seL4_RangeError;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    اگر (virq_get_virqType(vcpu->vgic.lr[index]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeVCPUInjectIRQ(vcpu, index, virq);
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
    انتخاب (label) {
    حالت ARMVCPUSetTCB:
        بازگشت decodeVCPUSetTCB(cap);
    حالت ARMVCPUReadReg:
        بازگشت decodeVCPUReadReg(cap, length, call, buffer);
    حالت ARMVCPUWriteReg:
        بازگشت decodeVCPUWriteReg(cap, length, buffer);
    حالت ARMVCPUInjectIRQ:
        بازگشت decodeVCPUInjectIRQ(cap, length, buffer);
    حالت ARMVCPUAckVPPI:
        بازگشت decodeVCPUAckVPPI(cap, length, buffer);
    پیشفرض:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    اگر (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, buffer);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    اگر (status != EXCEPTION_NONE) {
        بازگشت status;
    }

    VPPIEventIRQ_t vppi = irqVPPIEventIndex(irq);
    اگر (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 0;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    بازگشت EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    اگر (current_extra_caps.excaprefs[0] == پوچ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    بازگشت EXCEPTION_NONE;
}


تهی handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        اگر (armv_handleVCPUFault(hsr))
        {
            بازگشت;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(گره_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#ifdef ENABLE_SMP_SUPPORT
تهی handleVCPUInjectInterruptIPI(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    اگر (likely(ARCH_گره_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
    } وگرنه {
        vcpu->vgic.lr[index] = virq;
    }
}
#endif /* ENABLE_SMP_SUPPORT */

#endif
