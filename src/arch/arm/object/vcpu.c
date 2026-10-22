/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT

#समावेश <arch/object/vcpu.h>
#समावेश <armv/vcpu.h>
#समावेश <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#समावेश <arch/machine/debug_conf.h>
#समावेश <drivers/timer/arm_generic.h>
#समावेश <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE व्योम vcpu_boot_init(व्योम)
{
    armv_vcpu_boot_init();
    gic_vcpu_अंक_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    अगर (gic_vcpu_अंक_list_regs > GIC_VCPU_MAX_अंक_LR) {
        म_लिखो("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_अंक_list_regs = GIC_VCPU_MAX_अंक_LR;
    }
    vcpu_disable(शून्य);
    ARCH_गाँठ_STATE(armHSCurVCPU) = शून्य;
    ARCH_गाँठ_STATE(armHSVCPUActive) = false;

}

अटल व्योम vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_अंक;

    निश्चित(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    अगर (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_अंक = gic_vcpu_अंक_list_regs;
    क्रम (i = 0; i < lr_अंक; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


अटल word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    अगर (likely(ARCH_गाँठ_STATE(armHSCurVCPU) == vcpu)) {
        अगर (vcpu_reg_saved_when_disabled(field) && !ARCH_गाँठ_STATE(armHSVCPUActive)) {
            वापस vcpu_read_reg(vcpu, field);
        } अन्यथा {
            वापस vcpu_hw_read_reg(field);
        }
    } अन्यथा {
        वापस vcpu_read_reg(vcpu, field);
    }
}

अटल व्योम writeVCPUReg(vcpu_t *vcpu, word_t field, word_t मूल्य)
{
    अगर (likely(ARCH_गाँठ_STATE(armHSCurVCPU) == vcpu)) {
        अगर (vcpu_reg_saved_when_disabled(field) && !ARCH_गाँठ_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, मूल्य);
        } अन्यथा {
            vcpu_hw_write_reg(field, मूल्य);
        }
    } अन्यथा {
        vcpu_write_reg(vcpu, field, मूल्य);
    }
}

व्योम vcpu_restore(vcpu_t *vcpu)
{
    निश्चित(vcpu);
    word_t i;
    word_t lr_अंक;
    /* Turn off the VGIC */
    set_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    set_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    set_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_अंक = gic_vcpu_अंक_list_regs;
    क्रम (i = 0; i < lr_अंक; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

व्योम VPPIEvent(irq_t irq)
{
#अगर_घोषित CONFIG_KERNEL_MCS
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
    अगर (thread_state_get_tcbQueued(गाँठ_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(गाँठ_STATE(ksCurThread))) {
        वापस;
    }
#पूर्ण_अगर

    अगर (ARCH_गाँठ_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        निश्चित(irq_VPPIEvent_सूचकांक(irq) != VPPIEventIRQ_invalid);
        ARCH_गाँठ_STATE(armHSCurVCPU)->vppi_masked[irq_VPPIEvent_सूचकांक(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        निश्चित(isRunnable(गाँठ_STATE(ksCurThread)));
        अगर (isRunnable(गाँठ_STATE(ksCurThread))) {
            handleFault(गाँठ_STATE(ksCurThread));
        }
    }
}

व्योम VGICMaintenance(व्योम)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#अगर_घोषित CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    अगर (thread_state_get_tcbQueued(गाँठ_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(गाँठ_STATE(ksCurThread))) {
        वापस;
    }
#पूर्ण_अगर

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    अगर (!ARCH_गाँठ_STATE(armHSVCPUActive)) {
        म_लिखो("Received VGIC maintenance without active VCPU!\n");
        वापस;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    अगर (flags & VGIC_MISR_EOI) {
        पूर्णांक irq_idx;
        अगर (eisr0) {
            irq_idx = ctzl(eisr0);
        } अन्यथा अगर (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } अन्यथा {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        अगर (irq_idx == -1  || irq_idx >= gic_vcpu_अंक_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } अन्यथा {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            चयन (virq_get_virqType(virq)) {
            हाल virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                अवरोध;
            हाल virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                अवरोध;
            हाल virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                अवरोध;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            निश्चित(ARCH_गाँठ_STATE(armHSCurVCPU) != शून्य && ARCH_गाँठ_STATE(armHSVCPUActive));
            अगर (ARCH_गाँठ_STATE(armHSCurVCPU) != शून्य && ARCH_गाँठ_STATE(armHSVCPUActive)) {
                ARCH_गाँठ_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } अन्यथा {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } अन्यथा {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    निश्चित(isRunnable(गाँठ_STATE(ksCurThread)));
    अगर (isRunnable(गाँठ_STATE(ksCurThread))) {
        handleFault(गाँठ_STATE(ksCurThread));
    }
}

व्योम vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#अगर_घोषित CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#पूर्ण_अगर
}

व्योम vcpu_switch(vcpu_t *new)
{
    अगर (likely(ARCH_गाँठ_STATE(armHSCurVCPU) != new)) {
        अगर (unlikely(new != शून्य)) {
            अगर (unlikely(ARCH_गाँठ_STATE(armHSCurVCPU) != शून्य)) {
                vcpu_save(ARCH_गाँठ_STATE(armHSCurVCPU), ARCH_गाँठ_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_गाँठ_STATE(armHSCurVCPU) = new;
            ARCH_गाँठ_STATE(armHSVCPUActive) = true;
        } अन्यथा अगर (unlikely(ARCH_गाँठ_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#अगर_घोषित ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_गाँठ_STATE(armHSCurVCPU)->vcpuTCB);
#पूर्ण_अगर
            vcpu_disable(ARCH_गाँठ_STATE(armHSCurVCPU));
            ARCH_गाँठ_STATE(armHSVCPUActive) = false;
        }
    } अन्यथा अगर (likely(!ARCH_गाँठ_STATE(armHSVCPUActive) && new != शून्य)) {
        isb();
        vcpu_enable(new);
        ARCH_गाँठ_STATE(armHSVCPUActive) = true;
    }
}

अटल व्योम vcpu_invalidate_active(व्योम)
{
    अगर (ARCH_गाँठ_STATE(armHSVCPUActive)) {
        vcpu_disable(शून्य);
        ARCH_गाँठ_STATE(armHSVCPUActive) = false;
    }
    ARCH_गाँठ_STATE(armHSCurVCPU) = शून्य;
}

व्योम vcpu_flush(व्योम)
{
    अगर (ARCH_गाँठ_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_गाँठ_STATE(armHSCurVCPU), ARCH_गाँठ_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

व्योम vcpu_flush_if_current(tcb_t *tptr)
{
    अगर (tptr->tcbArch.tcbVCPU == ARCH_गाँठ_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

व्योम vcpu_finalise(vcpu_t *vcpu)
{
    अगर (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

व्योम associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    अगर (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    अगर (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    अगर (tcb == गाँठ_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

व्योम dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    अगर (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    अगर (vcpu == ARCH_गाँठ_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = शून्य;
    vcpu->vcpuTCB = शून्य;
#अगर_घोषित ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#पूर्ण_अगर

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#अगर_घोषित CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#अन्यथा
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#पूर्ण_अगर
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t मूल्य)
{
    writeVCPUReg(vcpu, field, मूल्य);
    वापस EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t लंबाई, word_t *बफर)
{
    word_t field;
    word_t मूल्य;
    अगर (लंबाई < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, बफर);
    मूल्य = getSyscallArg(1, बफर);
    अगर (field >= se_L4_VCPUReg_अंक) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (दीर्घ)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, मूल्य);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = गाँठ_STATE(ksCurThread);
    word_t मूल्य = readVCPUReg(vcpu, field);
    अगर (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        अचिन्हित पूर्णांक लंबाई = setMR(thread, ipc_बफर, 0, मूल्य);
        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, लंबाई)));
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
    वापस EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t लंबाई, bool_t call, word_t *बफर)
{
    word_t field;
    अगर (लंबाई < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, बफर);

    अगर (field >= se_L4_VCPUReg_अंक) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (दीर्घ)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, अचिन्हित दीर्घ सूचकांक, virq_t virq)
{
    अगर (likely(ARCH_गाँठ_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(सूचकांक, virq);
#अगर_घोषित ENABLE_SMP_SUPPORT
    } अन्यथा अगर (vcpu->vcpuTCB != शून्य && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, सूचकांक, virq.शब्दगण[0],
                       vcpu->vcpuTCB->tcbAffinity);
#पूर्ण_अगर /* CONFIG_ENABLE_SMP */
    } अन्यथा {
        vcpu->vgic.lr[सूचकांक] = virq;
    }

    वापस EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t लंबाई, word_t *बफर)
{
    word_t vid, priority, group, सूचकांक;
    vcpu_t *vcpu;
#अगर_घोषित CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    अगर (लंबाई < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, बफर);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    सूचकांक = (mr0 >> 32) & 0xff;
#अन्यथा
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    अगर (लंबाई < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, बफर);
    mr1 = getSyscallArg(1, बफर);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    सूचकांक = mr1 & 0xff;
#पूर्ण_अगर

    /* Check IRQ parameters */
    अगर (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_Argument_संख्या = 1;
        current_syscall_error.type = seL4_RangeError;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_Argument_संख्या = 2;
        current_syscall_error.type = seL4_RangeError;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_Argument_संख्या = 3;
        current_syscall_error.type = seL4_RangeError;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    अगर (सूचकांक >= gic_vcpu_अंक_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_अंक_list_regs - 1;
        current_syscall_error.invalid_Argument_संख्या = 4;
        current_syscall_error.type = seL4_RangeError;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    अगर (virq_get_virqType(vcpu->vgic.lr[सूचकांक]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeVCPUInjectIRQ(vcpu, सूचकांक, virq);
}

exception_t decodeARMVCPUInvocation(
    word_t label,
    word_t लंबाई,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    चयन (label) {
    हाल ARMVCPUSetTCB:
        वापस decodeVCPUSetTCB(cap);
    हाल ARMVCPUReadReg:
        वापस decodeVCPUReadReg(cap, लंबाई, call, बफर);
    हाल ARMVCPUWriteReg:
        वापस decodeVCPUWriteReg(cap, लंबाई, बफर);
    हाल ARMVCPUInjectIRQ:
        वापस decodeVCPUInjectIRQ(cap, लंबाई, बफर);
    हाल ARMVCPUAckVPPI:
        वापस decodeVCPUAckVPPI(cap, लंबाई, बफर);
    शेष:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t लंबाई, word_t *बफर)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    अगर (लंबाई < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, बफर);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_सूचकांक(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    अगर (status != EXCEPTION_NONE) {
        वापस status;
    }

    VPPIEventIRQ_t vppi = irq_VPPIEvent_सूचकांक(irq);
    अगर (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    वापस EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    अगर (current_extra_caps.excaprefs[0] == शून्य) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    अगर (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    वापस EXCEPTION_NONE;
}


व्योम handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        अगर (armv_handleVCPUFault(hsr))
        {
            वापस;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(गाँठ_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#अगर_घोषित ENABLE_SMP_SUPPORT
व्योम handleVCPUInjectInterruptIPI(vcpu_t *vcpu, अचिन्हित दीर्घ सूचकांक, virq_t virq)
{
    अगर (likely(ARCH_गाँठ_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(सूचकांक, virq);
    } अन्यथा {
        vcpu->vgic.lr[सूचकांक] = virq;
    }
}
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

#पूर्ण_अगर
