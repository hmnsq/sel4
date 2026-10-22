/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#অন্তর্ভাব <arch/object/vcpu.h>
#অন্তর্ভাব <armv/vcpu.h>
#অন্তর্ভাব <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#অন্তর্ভাব <arch/machine/debug_conf.h>
#অন্তর্ভাব <drivers/timer/arm_generic.h>
#অন্তর্ভাব <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE শূন্য vcpu_boot_init(শূন্য)
{
    armv_vcpu_boot_init();
    gic_vcpu_অঙ্ক_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    যদি (gic_vcpu_অঙ্ক_list_regs > GIC_VCPU_MAX_অঙ্ক_LR) {
        printf("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_অঙ্ক_list_regs = GIC_VCPU_MAX_অঙ্ক_LR;
    }
    vcpu_disable(ফাঁকা);
    ARCH_গ্রন্থি_STATE(armHSCurVCPU) = ফাঁকা;
    ARCH_গ্রন্থি_STATE(armHSVCPUActive) = false;

}

স্থির শূন্য vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_অঙ্ক;

    প্রতিজ্ঞা(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    যদি (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_অঙ্ক = gic_vcpu_অঙ্ক_list_regs;
    জন্য (i = 0; i < lr_অঙ্ক; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


স্থির word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    যদি (likely(ARCH_গ্রন্থি_STATE(armHSCurVCPU) == vcpu)) {
        যদি (vcpu_reg_saved_when_disabled(field) && !ARCH_গ্রন্থি_STATE(armHSVCPUActive)) {
            ফেরত vcpu_read_reg(vcpu, field);
        } অন্যথা {
            ফেরত vcpu_hw_read_reg(field);
        }
    } অন্যথা {
        ফেরত vcpu_read_reg(vcpu, field);
    }
}

স্থির শূন্য writeVCPUReg(vcpu_t *vcpu, word_t field, word_t মান)
{
    যদি (likely(ARCH_গ্রন্থি_STATE(armHSCurVCPU) == vcpu)) {
        যদি (vcpu_reg_saved_when_disabled(field) && !ARCH_গ্রন্থি_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, মান);
        } অন্যথা {
            vcpu_hw_write_reg(field, মান);
        }
    } অন্যথা {
        vcpu_write_reg(vcpu, field, মান);
    }
}

শূন্য vcpu_restore(vcpu_t *vcpu)
{
    প্রতিজ্ঞা(vcpu);
    word_t i;
    word_t lr_অঙ্ক;
    /* Turn off the VGIC */
    set_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    set_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    set_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_অঙ্ক = gic_vcpu_অঙ্ক_list_regs;
    জন্য (i = 0; i < lr_অঙ্ক; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

শূন্য VPPIEvent(irq_t irq)
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
    যদি (thread_state_get_tcbQueued(গ্রন্থি_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(গ্রন্থি_STATE(ksCurThread))) {
        ফেরত;
    }
#endif

    যদি (ARCH_গ্রন্থি_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        প্রতিজ্ঞা(irq_VPPIEvent_সূচক(irq) != VPPIEventIRQ_invalid);
        ARCH_গ্রন্থি_STATE(armHSCurVCPU)->vppi_masked[irq_VPPIEvent_সূচক(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        প্রতিজ্ঞা(isRunnable(গ্রন্থি_STATE(ksCurThread)));
        যদি (isRunnable(গ্রন্থি_STATE(ksCurThread))) {
            handleFault(গ্রন্থি_STATE(ksCurThread));
        }
    }
}

শূন্য VGICMaintenance(শূন্য)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ifdef CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    যদি (thread_state_get_tcbQueued(গ্রন্থি_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(গ্রন্থি_STATE(ksCurThread))) {
        ফেরত;
    }
#endif

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    যদি (!ARCH_গ্রন্থি_STATE(armHSVCPUActive)) {
        printf("Received VGIC maintenance without active VCPU!\n");
        ফেরত;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    যদি (flags & VGIC_MISR_EOI) {
        পূর্ণসংখ্যা irq_idx;
        যদি (eisr0) {
            irq_idx = ctzl(eisr0);
        } অন্যথা যদি (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } অন্যথা {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        যদি (irq_idx == -1  || irq_idx >= gic_vcpu_অঙ্ক_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } অন্যথা {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            নির্বাচন (virq_get_virqType(virq)) {
            ক্ষেত্র virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                বিরতি;
            ক্ষেত্র virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                বিরতি;
            ক্ষেত্র virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                বিরতি;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            প্রতিজ্ঞা(ARCH_গ্রন্থি_STATE(armHSCurVCPU) != ফাঁকা && ARCH_গ্রন্থি_STATE(armHSVCPUActive));
            যদি (ARCH_গ্রন্থি_STATE(armHSCurVCPU) != ফাঁকা && ARCH_গ্রন্থি_STATE(armHSVCPUActive)) {
                ARCH_গ্রন্থি_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } অন্যথা {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } অন্যথা {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    প্রতিজ্ঞা(isRunnable(গ্রন্থি_STATE(ksCurThread)));
    যদি (isRunnable(গ্রন্থি_STATE(ksCurThread))) {
        handleFault(গ্রন্থি_STATE(ksCurThread));
    }
}

শূন্য vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#endif
}

শূন্য vcpu_switch(vcpu_t *new)
{
    যদি (likely(ARCH_গ্রন্থি_STATE(armHSCurVCPU) != new)) {
        যদি (unlikely(new != ফাঁকা)) {
            যদি (unlikely(ARCH_গ্রন্থি_STATE(armHSCurVCPU) != ফাঁকা)) {
                vcpu_save(ARCH_গ্রন্থি_STATE(armHSCurVCPU), ARCH_গ্রন্থি_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_গ্রন্থি_STATE(armHSCurVCPU) = new;
            ARCH_গ্রন্থি_STATE(armHSVCPUActive) = true;
        } অন্যথা যদি (unlikely(ARCH_গ্রন্থি_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_গ্রন্থি_STATE(armHSCurVCPU)->vcpuTCB);
#endif
            vcpu_disable(ARCH_গ্রন্থি_STATE(armHSCurVCPU));
            ARCH_গ্রন্থি_STATE(armHSVCPUActive) = false;
        }
    } অন্যথা যদি (likely(!ARCH_গ্রন্থি_STATE(armHSVCPUActive) && new != ফাঁকা)) {
        isb();
        vcpu_enable(new);
        ARCH_গ্রন্থি_STATE(armHSVCPUActive) = true;
    }
}

স্থির শূন্য vcpu_invalidate_active(শূন্য)
{
    যদি (ARCH_গ্রন্থি_STATE(armHSVCPUActive)) {
        vcpu_disable(ফাঁকা);
        ARCH_গ্রন্থি_STATE(armHSVCPUActive) = false;
    }
    ARCH_গ্রন্থি_STATE(armHSCurVCPU) = ফাঁকা;
}

শূন্য vcpu_flush(শূন্য)
{
    যদি (ARCH_গ্রন্থি_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_গ্রন্থি_STATE(armHSCurVCPU), ARCH_গ্রন্থি_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

শূন্য vcpu_flush_if_current(tcb_t *tptr)
{
    যদি (tptr->tcbArch.tcbVCPU == ARCH_গ্রন্থি_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

শূন্য vcpu_finalise(vcpu_t *vcpu)
{
    যদি (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

শূন্য associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    যদি (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    যদি (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    যদি (tcb == গ্রন্থি_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

শূন্য dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    যদি (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    যদি (vcpu == ARCH_গ্রন্থি_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = ফাঁকা;
    vcpu->vcpuTCB = ফাঁকা;
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#endif

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ifdef CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#অন্যথা
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#endif
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t মান)
{
    writeVCPUReg(vcpu, field, মান);
    ফেরত EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t দৈর্ঘ্য, word_t *বাফার)
{
    word_t field;
    word_t মান;
    যদি (দৈর্ঘ্য < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, বাফার);
    মান = getSyscallArg(1, বাফার);
    যদি (field >= se_L4_VCPUReg_অঙ্ক) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, মান);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = গ্রন্থি_STATE(ksCurThread);
    word_t মান = readVCPUReg(vcpu, field);
    যদি (call) {
        word_t *ipc_বাফার = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned পূর্ণসংখ্যা দৈর্ঘ্য = setMR(thread, ipc_বাফার, 0, মান);
        setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, দৈর্ঘ্য)));
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);
    ফেরত EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t দৈর্ঘ্য, bool_t call, word_t *বাফার)
{
    word_t field;
    যদি (দৈর্ঘ্য < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, বাফার);

    যদি (field >= se_L4_VCPUReg_অঙ্ক) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, unsigned long সূচক, virq_t virq)
{
    যদি (likely(ARCH_গ্রন্থি_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(সূচক, virq);
#ifdef ENABLE_SMP_SUPPORT
    } অন্যথা যদি (vcpu->vcpuTCB != ফাঁকা && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, সূচক, virq.শব্দগুলি[0],
                       vcpu->vcpuTCB->tcbAffinity);
#endif /* CONFIG_ENABLE_SMP */
    } অন্যথা {
        vcpu->vgic.lr[সূচক] = virq;
    }

    ফেরত EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t দৈর্ঘ্য, word_t *বাফার)
{
    word_t vid, priority, group, সূচক;
    vcpu_t *vcpu;
#ifdef CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    যদি (দৈর্ঘ্য < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, বাফার);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    সূচক = (mr0 >> 32) & 0xff;
#অন্যথা
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    যদি (দৈর্ঘ্য < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, বাফার);
    mr1 = getSyscallArg(1, বাফার);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    সূচক = mr1 & 0xff;
#endif

    /* Check IRQ parameters */
    যদি (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        current_syscall_error.type = seL4_RangeError;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_Argument_সংখ্যা = 2;
        current_syscall_error.type = seL4_RangeError;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_Argument_সংখ্যা = 3;
        current_syscall_error.type = seL4_RangeError;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    যদি (সূচক >= gic_vcpu_অঙ্ক_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_অঙ্ক_list_regs - 1;
        current_syscall_error.invalid_Argument_সংখ্যা = 4;
        current_syscall_error.type = seL4_RangeError;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    যদি (virq_get_virqType(vcpu->vgic.lr[সূচক]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত invokeVCPUInjectIRQ(vcpu, সূচক, virq);
}

exception_t decodeARMVCPUInvocation(
    word_t label,
    word_t দৈর্ঘ্য,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *বাফার
)
{
    নির্বাচন (label) {
    ক্ষেত্র ARMVCPUSetTCB:
        ফেরত decodeVCPUSetTCB(cap);
    ক্ষেত্র ARMVCPUReadReg:
        ফেরত decodeVCPUReadReg(cap, দৈর্ঘ্য, call, বাফার);
    ক্ষেত্র ARMVCPUWriteReg:
        ফেরত decodeVCPUWriteReg(cap, দৈর্ঘ্য, বাফার);
    ক্ষেত্র ARMVCPUInjectIRQ:
        ফেরত decodeVCPUInjectIRQ(cap, দৈর্ঘ্য, বাফার);
    ক্ষেত্র ARMVCPUAckVPPI:
        ফেরত decodeVCPUAckVPPI(cap, দৈর্ঘ্য, বাফার);
    পূর্বনির্ধারিত:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t দৈর্ঘ্য, word_t *বাফার)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    যদি (দৈর্ঘ্য < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, বাফার);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_সূচক(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    যদি (status != EXCEPTION_NONE) {
        ফেরত status;
    }

    VPPIEventIRQ_t vppi = irq_VPPIEvent_সূচক(irq);
    যদি (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 0;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    ফেরত EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    যদি (current_extra_caps.excaprefs[0] == ফাঁকা) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    ফেরত EXCEPTION_NONE;
}


শূন্য handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        যদি (armv_handleVCPUFault(hsr))
        {
            ফেরত;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(গ্রন্থি_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#ifdef ENABLE_SMP_SUPPORT
শূন্য handleVCPUInjectInterruptIPI(vcpu_t *vcpu, unsigned long সূচক, virq_t virq)
{
    যদি (likely(ARCH_গ্রন্থি_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(সূচক, virq);
    } অন্যথা {
        vcpu->vgic.lr[সূচক] = virq;
    }
}
#endif /* ENABLE_SMP_SUPPORT */

#endif
