/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT

#अन्तर्भाव <arch/object/vcpu.h>
#अन्तर्भाव <armv/vcpu.h>
#अन्तर्भाव <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#अन्तर्भाव <arch/machine/debug_conf.h>
#अन्तर्भाव <drivers/timer/arm_generic.h>
#अन्तर्भाव <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_संहिता शून्य vcpu_boot_init(शून्य)
{
    armv_vcpu_boot_init();
    gic_vcpu_अङ्कः_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    यदि (gic_vcpu_अङ्कः_list_regs > GIC_VCPU_MAX_अङ्कः_LR) {
        प्रारूप_लिख("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_अङ्कः_list_regs = GIC_VCPU_MAX_अङ्कः_LR;
    }
    vcpu_disable(रिक्त);
    ARCH_ग्रन्थिः_STATE(armHSCurVCPU) = रिक्त;
    ARCH_ग्रन्थिः_STATE(armHSVCPUActive) = false;

}

स्थिर शून्य vcpu_save(vcpu_t *vcpu, bool_t सक्रिय)
{
    word_t i;
    word_t lr_अङ्कः;

    प्रतिज्ञा(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    यदि (सक्रिय) {
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
    armv_vcpu_save(vcpu, सक्रिय);
}


स्थिर word_t readVCPUReg(vcpu_t *vcpu, word_t क्षेत्रम्)
{
    यदि (likely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) == vcpu)) {
        यदि (vcpu_reg_saved_when_disabled(क्षेत्रम्) && !ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
            प्रत्यागम vcpu_read_reg(vcpu, क्षेत्रम्);
        } अन्यथा {
            प्रत्यागम vcpu_hw_read_reg(क्षेत्रम्);
        }
    } अन्यथा {
        प्रत्यागम vcpu_read_reg(vcpu, क्षेत्रम्);
    }
}

स्थिर शून्य writeVCPUReg(vcpu_t *vcpu, word_t क्षेत्रम्, word_t मानम्)
{
    यदि (likely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) == vcpu)) {
        यदि (vcpu_reg_saved_when_disabled(क्षेत्रम्) && !ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, क्षेत्रम्, मानम्);
        } अन्यथा {
            vcpu_hw_write_reg(क्षेत्रम्, मानम्);
        }
    } अन्यथा {
        vcpu_write_reg(vcpu, क्षेत्रम्, मानम्);
    }
}

शून्य vcpu_restore(vcpu_t *vcpu)
{
    प्रतिज्ञा(vcpu);
    word_t i;
    word_t lr_अङ्कः;
    /* Turn off the VGIC */
    समुच्चयः_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    समुच्चयः_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    समुच्चयः_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_अङ्कः = gic_vcpu_अङ्कः_list_regs;
    प्रति (i = 0; i < lr_अङ्कः; i++) {
        समुच्चयः_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, se_L4_VCPUReg_Save_Range_अन्तः);
    vcpu_enable(vcpu);
}

शून्य VPPIEvent(irq_t irq)
{
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
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
    यदि (thread_state_get_tcbQueued(ग्रन्थिः_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ग्रन्थिः_STATE(ksCurThread))) {
        प्रत्यागम;
    }
#यदि_अन्तः

    यदि (ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
        आवरणम्_व्यवधानम्(true, irq);
        प्रतिज्ञा(irq_VPPIEvent_सूचकांक(irq) != VPPIEventIRQ_invalid);
        ARCH_ग्रन्थिः_STATE(armHSCurVCPU)->vppi_masked[irq_VPPIEvent_सूचकांक(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_गन्तव्यम्_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        प्रतिज्ञा(isRunnable(ग्रन्थिः_STATE(ksCurThread)));
        यदि (isRunnable(ग्रन्थिः_STATE(ksCurThread))) {
            handleFault(ग्रन्थिः_STATE(ksCurThread));
        }
    }
}

शून्य VGICMaintenance(शून्य)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    यदि (thread_state_get_tcbQueued(ग्रन्थिः_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ग्रन्थिः_STATE(ksCurThread))) {
        प्रत्यागम;
    }
#यदि_अन्तः

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    यदि (!ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
        प्रारूप_लिख("Received VGIC maintenance without active VCPU!\n");
        प्रत्यागम;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    यदि (flags & VGIC_MISR_EOI) {
        पूर्णाङ्क irq_idx;
        यदि (eisr0) {
            irq_idx = ctzl(eisr0);
        } अन्यथा यदि (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } अन्यथा {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        यदि (irq_idx == -1  || irq_idx >= gic_vcpu_अङ्कः_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } अन्यथा {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            विकल्प (virq_get_virqType(virq)) {
            पक्ष virq_virq_सक्रिय:
                virq = virq_virq_सक्रिय_समुच्चयः_virq_EOIIRQEN(virq, 0);
                भङ्ग;
            पक्ष virq_virq_pending:
                virq = virq_virq_pending_समुच्चयः_virq_EOIIRQEN(virq, 0);
                भङ्ग;
            पक्ष virq_virq_invalid:
                virq = virq_virq_invalid_समुच्चयः_virq_EOIIRQEN(virq, 0);
                भङ्ग;
            }
            समुच्चयः_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            प्रतिज्ञा(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) != रिक्त && ARCH_ग्रन्थिः_STATE(armHSVCPUActive));
            यदि (ARCH_ग्रन्थिः_STATE(armHSCurVCPU) != रिक्त && ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
                ARCH_ग्रन्थिः_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
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
    प्रतिज्ञा(isRunnable(ग्रन्थिः_STATE(ksCurThread)));
    यदि (isRunnable(ग्रन्थिः_STATE(ksCurThread))) {
        handleFault(ग्रन्थिः_STATE(ksCurThread));
    }
}

शून्य vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#यदि_परिभाषितम् CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.चरमः_pcount = 0;
#यदि_अन्तः
}

शून्य vcpu_switch(vcpu_t *new)
{
    यदि (likely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) != new)) {
        यदि (unlikely(new != रिक्त)) {
            यदि (unlikely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) != रिक्त)) {
                vcpu_save(ARCH_ग्रन्थिः_STATE(armHSCurVCPU), ARCH_ग्रन्थिः_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_ग्रन्थिः_STATE(armHSCurVCPU) = new;
            ARCH_ग्रन्थिः_STATE(armHSVCPUActive) = true;
        } अन्यथा यदि (unlikely(ARCH_ग्रन्थिः_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#यदि_परिभाषितम् ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_तन्तवः
            saveAllBreakpointState(ARCH_ग्रन्थिः_STATE(armHSCurVCPU)->vcpuTCB);
#यदि_अन्तः
            vcpu_disable(ARCH_ग्रन्थिः_STATE(armHSCurVCPU));
            ARCH_ग्रन्थिः_STATE(armHSVCPUActive) = false;
        }
    } अन्यथा यदि (likely(!ARCH_ग्रन्थिः_STATE(armHSVCPUActive) && new != रिक्त)) {
        isb();
        vcpu_enable(new);
        ARCH_ग्रन्थिः_STATE(armHSVCPUActive) = true;
    }
}

स्थिर शून्य vcpu_invalidate_सक्रिय(शून्य)
{
    यदि (ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
        vcpu_disable(रिक्त);
        ARCH_ग्रन्थिः_STATE(armHSVCPUActive) = false;
    }
    ARCH_ग्रन्थिः_STATE(armHSCurVCPU) = रिक्त;
}

शून्य vcpu_flush(शून्य)
{
    यदि (ARCH_ग्रन्थिः_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_ग्रन्थिः_STATE(armHSCurVCPU), ARCH_ग्रन्थिः_STATE(armHSVCPUActive));
        vcpu_invalidate_सक्रिय();
    }
}

शून्य vcpu_flush_if_current(tcb_t *tptr)
{
    यदि (tptr->tcbArch.tcbVCPU == ARCH_ग्रन्थिः_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

शून्य vcpu_finalise(vcpu_t *vcpu)
{
    यदि (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

शून्य associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    यदि (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    यदि (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    यदि (tcb == ग्रन्थिः_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

शून्य dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    यदि (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    यदि (vcpu == ARCH_ग्रन्थिः_STATE(armHSCurVCPU)) {
        vcpu_invalidate_सक्रिय();
    }
    tcb->tcbArch.tcbVCPU = रिक्त;
    vcpu->vcpuTCB = रिक्त;
#यदि_परिभाषितम् ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_तन्तवः
    Arch_debugDissociateVCPUTCB(tcb);
#यदि_अन्तः

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    समुच्चयः_पञ्जिकाकोशः(tcb, SPSR_EL1, sanitise_पञ्जिकाकोशः(SPSR_EL1, get_पञ्जिकाकोशः(tcb, SPSR_EL1), false));
#अन्यथा
    समुच्चयः_पञ्जिकाकोशः(tcb, CPSR, sanitise_पञ्जिकाकोशः(CPSR, get_पञ्जिकाकोशः(tcb, CPSR), false));
#यदि_अन्तः
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t क्षेत्रम्, word_t मानम्)
{
    writeVCPUReg(vcpu, क्षेत्रम्, मानम्);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    word_t क्षेत्रम्;
    word_t मानम्;
    यदि (दीर्घता < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    क्षेत्रम् = getSyscallArg(0, बफर);
    मानम् = getSyscallArg(1, बफर);
    यदि (क्षेत्रम् >= se_L4_VCPUReg_अङ्कः) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (दीर्घ)क्षेत्रम्);
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), क्षेत्रम्, मानम्);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t क्षेत्रम्, bool_t call)
{
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    word_t मानम् = readVCPUReg(vcpu, क्षेत्रम्);
    यदि (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);
        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
        अचिह्नित पूर्णाङ्क दीर्घता = समुच्चयः_MR(thread, ipc_बफर, 0, मानम्);
        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t दीर्घता, bool_t call, word_t *बफर)
{
    word_t क्षेत्रम्;
    यदि (दीर्घता < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    क्षेत्रम् = getSyscallArg(0, बफर);

    यदि (क्षेत्रम् >= se_L4_VCPUReg_अङ्कः) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (दीर्घ)क्षेत्रम्);
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), क्षेत्रम्, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, अचिह्नित दीर्घ सूचकांक, virq_t virq)
{
    यदि (likely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) == vcpu)) {
        समुच्चयः_gic_vcpu_ctrl_lr(सूचकांक, virq);
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    } अन्यथा यदि (vcpu->vcpuTCB != रिक्त && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(Ipi_Remote_Call_VCPUInject_व्यवधानम्,
                       (word_t)vcpu, सूचकांक, virq.शब्दाः[0],
                       vcpu->vcpuTCB->tcbAffinity);
#यदि_अन्तः /* CONFIG_ENABLE_SMP */
    } अन्यथा {
        vcpu->vgic.lr[सूचकांक] = virq;
    }

    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    word_t vid, प्राथमिकता, गणः, सूचकांक;
    vcpu_t *vcpu;
#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    यदि (दीर्घता < 1) {
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, बफर);
    vid = mr0 & 0xffff;
    प्राथमिकता = (mr0 >> 16) & 0xff;
    गणः = (mr0 >> 24) & 0xff;
    सूचकांक = (mr0 >> 32) & 0xff;
#अन्यथा
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    यदि (दीर्घता < 2) {
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, बफर);
    mr1 = getSyscallArg(1, बफर);
    vid = mr0 & 0xffff;
    प्राथमिकता = (mr0 >> 16) & 0xff;
    गणः = (mr0 >> 24) & 0xff;
    सूचकांक = mr1 & 0xff;
#यदि_अन्तः

    /* Check IRQ parameters */
    यदि (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        current_syscall_error.type = seL4_RangeError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (प्राथमिकता > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 2;
        current_syscall_error.type = seL4_RangeError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (गणः > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 3;
        current_syscall_error.type = seL4_RangeError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    यदि (सूचकांक >= gic_vcpu_अङ्कः_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_अङ्कः_list_regs - 1;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 4;
        current_syscall_error.type = seL4_RangeError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    यदि (virq_get_virqType(vcpu->vgic.lr[सूचकांक]) == virq_virq_सक्रिय) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(गणः, प्राथमिकता, 1, vid);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeVCPUInjectIRQ(vcpu, सूचकांक, virq);
}

exception_t decodeARMVCPUInvocation(
    word_t label,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    विकल्प (label) {
    पक्ष ARMVCPUSetTCB:
        प्रत्यागम decodeVCPUSetTCB(cap);
    पक्ष ARMVCPUReadReg:
        प्रत्यागम decodeVCPUReadReg(cap, दीर्घता, call, बफर);
    पक्ष ARMVCPUWriteReg:
        प्रत्यागम decodeVCPUWriteReg(cap, दीर्घता, बफर);
    पक्ष ARMVCPUInjectIRQ:
        प्रत्यागम decodeVCPUInjectIRQ(cap, दीर्घता, बफर);
    पक्ष ARMVCPUAckVPPI:
        प्रत्यागम decodeVCPUAckVPPI(cap, दीर्घता, बफर);
    स्वभाव:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    यदि (दीर्घता < 1) {
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, बफर);
    irq_t irq = (irq_t) CORE_IRQ_गन्तव्यम्_IRQT(CURRENT_CPU_सूचकांक(), irq_w);
    exception_t स्थितिः = Arch_checkIRQ(irq_w);
    यदि (स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम स्थितिः;
    }

    VPPIEventIRQ_t vppi = irq_VPPIEvent_सूचकांक(irq);
    यदि (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
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
    यदि (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    यदि (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
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
        यदि (armv_handleVCPUFault(hsr))
        {
            प्रत्यागम;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(ग्रन्थिः_STATE(ksCurThread));
    })
    कालसारणी();
    activateThread();
}

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
शून्य handle_VCPUInject_व्यवधानम्_IPI(vcpu_t *vcpu, अचिह्नित दीर्घ सूचकांक, virq_t virq)
{
    यदि (likely(ARCH_ग्रन्थिः_STATE(armHSCurVCPU) == vcpu)) {
        समुच्चयः_gic_vcpu_ctrl_lr(सूचकांक, virq);
    } अन्यथा {
        vcpu->vgic.lr[सूचकांक] = virq;
    }
}
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

#यदि_अन्तः
