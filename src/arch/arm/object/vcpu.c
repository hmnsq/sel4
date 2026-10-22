/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT

#అన్తర్భావ <arch/object/vcpu.h>
#అన్తర్భావ <armv/vcpu.h>
#అన్తర్భావ <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#అన్తర్భావ <arch/machine/debug_conf.h>
#అన్తర్భావ <drivers/timer/arm_generic.h>
#అన్తర్భావ <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_సంకేతం శూన్య vcpu_boot_init(శూన్య)
{
    armv_vcpu_boot_init();
    gic_vcpu_అంకె_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    ఒకవేళ (gic_vcpu_అంకె_list_regs > GIC_VCPU_MAX_అంకె_LR) {
        రాయి("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_అంకె_list_regs = GIC_VCPU_MAX_అంకె_LR;
    }
    vcpu_disable(ఖాళీ);
    ARCH_కణుపు_STATE(armHSCurVCPU) = ఖాళీ;
    ARCH_కణుపు_STATE(armHSVCPUActive) = false;

}

స్థిర శూన్య vcpu_save(vcpu_t *vcpu, bool_t చురుకైన)
{
    మాట_t i;
    మాట_t lr_అంకె;

    ప్రతిజ్ఞా(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    ఒకవేళ (చురుకైన) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_అంకె = gic_vcpu_అంకె_list_regs;
    కొరకు (i = 0; i < lr_అంకె; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, చురుకైన);
}


స్థిర మాట_t readVCPUReg(vcpu_t *vcpu, మాట_t క్షేత్రం)
{
    ఒకవేళ (likely(ARCH_కణుపు_STATE(armHSCurVCPU) == vcpu)) {
        ఒకవేళ (vcpu_reg_saved_when_disabled(క్షేత్రం) && !ARCH_కణుపు_STATE(armHSVCPUActive)) {
            తిరిగి vcpu_read_reg(vcpu, క్షేత్రం);
        } లేకపోతే {
            తిరిగి vcpu_hw_read_reg(క్షేత్రం);
        }
    } లేకపోతే {
        తిరిగి vcpu_read_reg(vcpu, క్షేత్రం);
    }
}

స్థిర శూన్య writeVCPUReg(vcpu_t *vcpu, మాట_t క్షేత్రం, మాట_t విలువ)
{
    ఒకవేళ (likely(ARCH_కణుపు_STATE(armHSCurVCPU) == vcpu)) {
        ఒకవేళ (vcpu_reg_saved_when_disabled(క్షేత్రం) && !ARCH_కణుపు_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, క్షేత్రం, విలువ);
        } లేకపోతే {
            vcpu_hw_write_reg(క్షేత్రం, విలువ);
        }
    } లేకపోతే {
        vcpu_write_reg(vcpu, క్షేత్రం, విలువ);
    }
}

శూన్య vcpu_restore(vcpu_t *vcpu)
{
    ప్రతిజ్ఞా(vcpu);
    మాట_t i;
    మాట_t lr_అంకె;
    /* Turn off the VGIC */
    సమితి_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    సమితి_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    సమితి_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_అంకె = gic_vcpu_అంకె_list_regs;
    కొరకు (i = 0; i < lr_అంకె; i++) {
        సమితి_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, se_L4_VCPUReg_Save_Range_ప్రారంభం, se_L4_VCPUReg_Save_Range_చివర);
    vcpu_enable(vcpu);
}

శూన్య VPPIEvent(irq_t irq)
{
#నిర్వచితమైతే CONFIG_KERNEL_MCS
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
    ఒకవేళ (thread_state_get_tcbQueued(కణుపు_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(కణుపు_STATE(ksCurThread))) {
        తిరిగి;
    }
#అయితే_ముగింపు

    ఒకవేళ (ARCH_కణుపు_STATE(armHSVCPUActive)) {
        ముసుగు_అంతరాయం(true, irq);
        ప్రతిజ్ఞా(irq_VPPIEvent_సూచీ(irq) != VPPIEventIRQ_invalid);
        ARCH_కణుపు_STATE(armHSCurVCPU)->vppi_masked[irq_VPPIEvent_సూచీ(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_గమ్యం_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        ప్రతిజ్ఞా(isRunnable(కణుపు_STATE(ksCurThread)));
        ఒకవేళ (isRunnable(కణుపు_STATE(ksCurThread))) {
            handleFault(కణుపు_STATE(ksCurThread));
        }
    }
}

శూన్య VGICMaintenance(శూన్య)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    ఒకవేళ (thread_state_get_tcbQueued(కణుపు_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(కణుపు_STATE(ksCurThread))) {
        తిరిగి;
    }
#అయితే_ముగింపు

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    ఒకవేళ (!ARCH_కణుపు_STATE(armHSVCPUActive)) {
        రాయి("Received VGIC maintenance without active VCPU!\n");
        తిరిగి;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    ఒకవేళ (flags & VGIC_MISR_EOI) {
        పూర్ణాంకం irq_idx;
        ఒకవేళ (eisr0) {
            irq_idx = ctzl(eisr0);
        } లేకపోతే ఒకవేళ (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } లేకపోతే {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        ఒకవేళ (irq_idx == -1  || irq_idx >= gic_vcpu_అంకె_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } లేకపోతే {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            ఎంపిక (virq_get_virqType(virq)) {
            స్థితి virq_virq_చురుకైన:
                virq = virq_virq_చురుకైన_సమితి_virq_EOIIRQEN(virq, 0);
                ఆపు;
            స్థితి virq_virq_pending:
                virq = virq_virq_pending_సమితి_virq_EOIIRQEN(virq, 0);
                ఆపు;
            స్థితి virq_virq_invalid:
                virq = virq_virq_invalid_సమితి_virq_EOIIRQEN(virq, 0);
                ఆపు;
            }
            సమితి_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            ప్రతిజ్ఞా(ARCH_కణుపు_STATE(armHSCurVCPU) != ఖాళీ && ARCH_కణుపు_STATE(armHSVCPUActive));
            ఒకవేళ (ARCH_కణుపు_STATE(armHSCurVCPU) != ఖాళీ && ARCH_కణుపు_STATE(armHSVCPUActive)) {
                ARCH_కణుపు_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } లేకపోతే {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } లేకపోతే {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    ప్రతిజ్ఞా(isRunnable(కణుపు_STATE(ksCurThread)));
    ఒకవేళ (isRunnable(కణుపు_STATE(ksCurThread))) {
        handleFault(కణుపు_STATE(ksCurThread));
    }
}

శూన్య vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#నిర్వచితమైతే CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.చివరి_pcount = 0;
#అయితే_ముగింపు
}

శూన్య vcpu_switch(vcpu_t *new)
{
    ఒకవేళ (likely(ARCH_కణుపు_STATE(armHSCurVCPU) != new)) {
        ఒకవేళ (unlikely(new != ఖాళీ)) {
            ఒకవేళ (unlikely(ARCH_కణుపు_STATE(armHSCurVCPU) != ఖాళీ)) {
                vcpu_save(ARCH_కణుపు_STATE(armHSCurVCPU), ARCH_కణుపు_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_కణుపు_STATE(armHSCurVCPU) = new;
            ARCH_కణుపు_STATE(armHSVCPUActive) = true;
        } లేకపోతే ఒకవేళ (unlikely(ARCH_కణుపు_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#నిర్వచితమైతే ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_దారాలు
            saveAllBreakpointState(ARCH_కణుపు_STATE(armHSCurVCPU)->vcpuTCB);
#అయితే_ముగింపు
            vcpu_disable(ARCH_కణుపు_STATE(armHSCurVCPU));
            ARCH_కణుపు_STATE(armHSVCPUActive) = false;
        }
    } లేకపోతే ఒకవేళ (likely(!ARCH_కణుపు_STATE(armHSVCPUActive) && new != ఖాళీ)) {
        isb();
        vcpu_enable(new);
        ARCH_కణుపు_STATE(armHSVCPUActive) = true;
    }
}

స్థిర శూన్య vcpu_invalidate_చురుకైన(శూన్య)
{
    ఒకవేళ (ARCH_కణుపు_STATE(armHSVCPUActive)) {
        vcpu_disable(ఖాళీ);
        ARCH_కణుపు_STATE(armHSVCPUActive) = false;
    }
    ARCH_కణుపు_STATE(armHSCurVCPU) = ఖాళీ;
}

శూన్య vcpu_flush(శూన్య)
{
    ఒకవేళ (ARCH_కణుపు_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_కణుపు_STATE(armHSCurVCPU), ARCH_కణుపు_STATE(armHSVCPUActive));
        vcpu_invalidate_చురుకైన();
    }
}

శూన్య vcpu_flush_if_current(tcb_t *tptr)
{
    ఒకవేళ (tptr->tcbArch.tcbVCPU == ARCH_కణుపు_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

శూన్య vcpu_finalise(vcpu_t *vcpu)
{
    ఒకవేళ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

శూన్య associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ఒకవేళ (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    ఒకవేళ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    ఒకవేళ (tcb == కణుపు_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

శూన్య dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ఒకవేళ (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    ఒకవేళ (vcpu == ARCH_కణుపు_STATE(armHSCurVCPU)) {
        vcpu_invalidate_చురుకైన();
    }
    tcb->tcbArch.tcbVCPU = ఖాళీ;
    vcpu->vcpuTCB = ఖాళీ;
#నిర్వచితమైతే ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_దారాలు
    Arch_debugDissociateVCPUTCB(tcb);
#అయితే_ముగింపు

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    సమితి_రిజిస్టరు(tcb, SPSR_EL1, sanitise_రిజిస్టరు(SPSR_EL1, get_రిజిస్టరు(tcb, SPSR_EL1), false));
#లేకపోతే
    సమితి_రిజిస్టరు(tcb, CPSR, sanitise_రిజిస్టరు(CPSR, get_రిజిస్టరు(tcb, CPSR), false));
#అయితే_ముగింపు
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, మాట_t క్షేత్రం, మాట_t విలువ)
{
    writeVCPUReg(vcpu, క్షేత్రం, విలువ);
    తిరిగి EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    మాట_t క్షేత్రం;
    మాట_t విలువ;
    ఒకవేళ (నిడివి < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    క్షేత్రం = getSyscallArg(0, బఫరు);
    విలువ = getSyscallArg(1, బఫరు);
    ఒకవేళ (క్షేత్రం >= se_L4_VCPUReg_అంకె) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (దీర్ఘ)క్షేత్రం);
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), క్షేత్రం, విలువ);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, మాట_t క్షేత్రం, bool_t call)
{
    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);
    మాట_t విలువ = readVCPUReg(vcpu, క్షేత్రం);
    ఒకవేళ (call) {
        మాట_t *ipc_బఫరు = lookupIPCBuffer(true, thread);
        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        గుర్తులేని పూర్ణాంకం నిడివి = సమితి_MR(thread, ipc_బఫరు, 0, విలువ);
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, మాట_t నిడివి, bool_t call, మాట_t *బఫరు)
{
    మాట_t క్షేత్రం;
    ఒకవేళ (నిడివి < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    క్షేత్రం = getSyscallArg(0, బఫరు);

    ఒకవేళ (క్షేత్రం >= se_L4_VCPUReg_అంకె) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (దీర్ఘ)క్షేత్రం);
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), క్షేత్రం, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, గుర్తులేని దీర్ఘ సూచీ, virq_t virq)
{
    ఒకవేళ (likely(ARCH_కణుపు_STATE(armHSCurVCPU) == vcpu)) {
        సమితి_gic_vcpu_ctrl_lr(సూచీ, virq);
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    } లేకపోతే ఒకవేళ (vcpu->vcpuTCB != ఖాళీ && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(Ipi_Remote_Call_VCPUInject_అంతరాయం,
                       (మాట_t)vcpu, సూచీ, virq.పదాలు[0],
                       vcpu->vcpuTCB->tcbAffinity);
#అయితే_ముగింపు /* CONFIG_ENABLE_SMP */
    } లేకపోతే {
        vcpu->vgic.lr[సూచీ] = virq;
    }

    తిరిగి EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    మాట_t vid, ప్రాధాన్యత, సమూహం, సూచీ;
    vcpu_t *vcpu;
#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    మాట_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ఒకవేళ (నిడివి < 1) {
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, బఫరు);
    vid = mr0 & 0xffff;
    ప్రాధాన్యత = (mr0 >> 16) & 0xff;
    సమూహం = (mr0 >> 24) & 0xff;
    సూచీ = (mr0 >> 32) & 0xff;
#లేకపోతే
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ఒకవేళ (నిడివి < 2) {
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, బఫరు);
    mr1 = getSyscallArg(1, బఫరు);
    vid = mr0 & 0xffff;
    ప్రాధాన్యత = (mr0 >> 16) & 0xff;
    సమూహం = (mr0 >> 24) & 0xff;
    సూచీ = mr1 & 0xff;
#అయితే_ముగింపు

    /* Check IRQ parameters */
    ఒకవేళ (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        current_syscall_error.type = seL4_RangeError;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (ప్రాధాన్యత > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_వాదం_సంఖ్య = 2;
        current_syscall_error.type = seL4_RangeError;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (సమూహం > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_వాదం_సంఖ్య = 3;
        current_syscall_error.type = seL4_RangeError;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    ఒకవేళ (సూచీ >= gic_vcpu_అంకె_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_అంకె_list_regs - 1;
        current_syscall_error.invalid_వాదం_సంఖ్య = 4;
        current_syscall_error.type = seL4_RangeError;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    ఒకవేళ (virq_get_virqType(vcpu->vgic.lr[సూచీ]) == virq_virq_చురుకైన) {
        userError("VGIC List register in use.");
        current_syscall_error.type = se_L4_Delete_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(సమూహం, ప్రాధాన్యత, 1, vid);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeVCPUInjectIRQ(vcpu, సూచీ, virq);
}

exception_t decodeARMVCPUInvocation(
    మాట_t label,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *స్థానం,
    cap_t cap,
    bool_t call,
    మాట_t *బఫరు
)
{
    ఎంపిక (label) {
    స్థితి ARMVCPUSetTCB:
        తిరిగి decodeVCPUSetTCB(cap);
    స్థితి ARMVCPUReadReg:
        తిరిగి decodeVCPUReadReg(cap, నిడివి, call, బఫరు);
    స్థితి ARMVCPUWriteReg:
        తిరిగి decodeVCPUWriteReg(cap, నిడివి, బఫరు);
    స్థితి ARMVCPUInjectIRQ:
        తిరిగి decodeVCPUInjectIRQ(cap, నిడివి, బఫరు);
    స్థితి ARMVCPUAckVPPI:
        తిరిగి decodeVCPUAckVPPI(cap, నిడివి, బఫరు);
    అప్రమేయ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ఒకవేళ (నిడివి < 1) {
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    మాట_t irq_w = getSyscallArg(0, బఫరు);
    irq_t irq = (irq_t) CORE_IRQ_గమ్యం_IRQT(CURRENT_CPU_సూచీ(), irq_w);
    exception_t స్థితి_2 = Arch_checkIRQ(irq_w);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి స్థితి_2;
    }

    VPPIEventIRQ_t vppi = irq_VPPIEvent_సూచీ(irq);
    ఒకవేళ (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    తిరిగి EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    తిరిగి EXCEPTION_NONE;
}


శూన్య handleVCPUFault(మాట_t hsr)
{
    MCS_DO_IF_BUDGET({
        ఒకవేళ (armv_handleVCPUFault(hsr))
        {
            తిరిగి;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(కణుపు_STATE(ksCurThread));
    })
    కాలపట్టిక();
    activateThread();
}

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
శూన్య handle_VCPUInject_అంతరాయం_IPI(vcpu_t *vcpu, గుర్తులేని దీర్ఘ సూచీ, virq_t virq)
{
    ఒకవేళ (likely(ARCH_కణుపు_STATE(armHSCurVCPU) == vcpu)) {
        సమితి_gic_vcpu_ctrl_lr(సూచీ, virq);
    } లేకపోతే {
        vcpu->vgic.lr[సూచీ] = virq;
    }
}
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

#అయితే_ముగింపు
