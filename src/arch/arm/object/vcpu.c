/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT

#ضمن <arch/object/vcpu.h>
#ضمن <armv/vcpu.h>
#ضمن <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#ضمن <arch/machine/debug_conf.h>
#ضمن <drivers/timer/arm_generic.h>
#ضمن <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_کد فراغ vcpu_boot_init(فراغ)
{
    armv_vcpu_boot_init();
    gic_vcpu_رقم_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    إذا (gic_vcpu_رقم_list_regs > GIC_VCPU_MAX_رقم_LR) {
        چاپ_کن("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_رقم_list_regs = GIC_VCPU_MAX_رقم_LR;
    }
    vcpu_disable(عدم);
    ARCH_گره_STATE(armHSCurVCPU) = عدم;
    ARCH_گره_STATE(armHSVCPUActive) = false;

}

ساكن فراغ vcpu_save(vcpu_t *vcpu, bool_t فعال)
{
    کلمه_t i;
    کلمه_t lr_رقم;

    أكد(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    إذا (فعال) {
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
    armv_vcpu_save(vcpu, فعال);
}


ساكن کلمه_t readVCPUReg(vcpu_t *vcpu, کلمه_t فیلد)
{
    إذا (likely(ARCH_گره_STATE(armHSCurVCPU) == vcpu)) {
        إذا (vcpu_reg_saved_when_disabled(فیلد) && !ARCH_گره_STATE(armHSVCPUActive)) {
            أرجع vcpu_read_reg(vcpu, فیلد);
        } وإلا {
            أرجع vcpu_hw_read_reg(فیلد);
        }
    } وإلا {
        أرجع vcpu_read_reg(vcpu, فیلد);
    }
}

ساكن فراغ writeVCPUReg(vcpu_t *vcpu, کلمه_t فیلد, کلمه_t مقدار)
{
    إذا (likely(ARCH_گره_STATE(armHSCurVCPU) == vcpu)) {
        إذا (vcpu_reg_saved_when_disabled(فیلد) && !ARCH_گره_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, فیلد, مقدار);
        } وإلا {
            vcpu_hw_write_reg(فیلد, مقدار);
        }
    } وإلا {
        vcpu_write_reg(vcpu, فیلد, مقدار);
    }
}

فراغ vcpu_restore(vcpu_t *vcpu)
{
    أكد(vcpu);
    کلمه_t i;
    کلمه_t lr_رقم;
    /* Turn off the VGIC */
    مجموعه_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    مجموعه_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    مجموعه_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_رقم = gic_vcpu_رقم_list_regs;
    لكل (i = 0; i < lr_رقم; i++) {
        مجموعه_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, se_L4_VCPUReg_Save_Range_شروع, se_L4_VCPUReg_Save_Range_انتها);
    vcpu_enable(vcpu);
}

فراغ VPPIEvent(irq_t irq)
{
#اگر_تعریف_شده CONFIG_KERNEL_MCS
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
    إذا (thread_state_get_tcbQueued(گره_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(گره_STATE(ksCurThread))) {
        أرجع;
    }
#پایان_اگر

    إذا (ARCH_گره_STATE(armHSVCPUActive)) {
        ماسک_وقفه(true, irq);
        أكد(irq_VPPIEvent_شاخص(irq) != VPPIEventIRQ_invalid);
        ARCH_گره_STATE(armHSCurVCPU)->vppi_masked[irq_VPPIEvent_شاخص(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_مقصد_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        أكد(isRunnable(گره_STATE(ksCurThread)));
        إذا (isRunnable(گره_STATE(ksCurThread))) {
            handleFault(گره_STATE(ksCurThread));
        }
    }
}

فراغ VGICMaintenance(فراغ)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    إذا (thread_state_get_tcbQueued(گره_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(گره_STATE(ksCurThread))) {
        أرجع;
    }
#پایان_اگر

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    إذا (!ARCH_گره_STATE(armHSVCPUActive)) {
        چاپ_کن("Received VGIC maintenance without active VCPU!\n");
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
            حالة virq_virq_فعال:
                virq = virq_virq_فعال_مجموعه_virq_EOIIRQEN(virq, 0);
                اكسر;
            حالة virq_virq_pending:
                virq = virq_virq_pending_مجموعه_virq_EOIIRQEN(virq, 0);
                اكسر;
            حالة virq_virq_invalid:
                virq = virq_virq_invalid_مجموعه_virq_EOIIRQEN(virq, 0);
                اكسر;
            }
            مجموعه_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            أكد(ARCH_گره_STATE(armHSCurVCPU) != عدم && ARCH_گره_STATE(armHSVCPUActive));
            إذا (ARCH_گره_STATE(armHSCurVCPU) != عدم && ARCH_گره_STATE(armHSVCPUActive)) {
                ARCH_گره_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
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
    أكد(isRunnable(گره_STATE(ksCurThread)));
    إذا (isRunnable(گره_STATE(ksCurThread))) {
        handleFault(گره_STATE(ksCurThread));
    }
}

فراغ vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#اگر_تعریف_شده CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.آخرین_pcount = 0;
#پایان_اگر
}

فراغ vcpu_switch(vcpu_t *new)
{
    إذا (likely(ARCH_گره_STATE(armHSCurVCPU) != new)) {
        إذا (unlikely(new != عدم)) {
            إذا (unlikely(ARCH_گره_STATE(armHSCurVCPU) != عدم)) {
                vcpu_save(ARCH_گره_STATE(armHSCurVCPU), ARCH_گره_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_گره_STATE(armHSCurVCPU) = new;
            ARCH_گره_STATE(armHSVCPUActive) = true;
        } وإلا إذا (unlikely(ARCH_گره_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#اگر_تعریف_شده ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_نخها
            saveAllBreakpointState(ARCH_گره_STATE(armHSCurVCPU)->vcpuTCB);
#پایان_اگر
            vcpu_disable(ARCH_گره_STATE(armHSCurVCPU));
            ARCH_گره_STATE(armHSVCPUActive) = false;
        }
    } وإلا إذا (likely(!ARCH_گره_STATE(armHSVCPUActive) && new != عدم)) {
        isb();
        vcpu_enable(new);
        ARCH_گره_STATE(armHSVCPUActive) = true;
    }
}

ساكن فراغ vcpu_invalidate_فعال(فراغ)
{
    إذا (ARCH_گره_STATE(armHSVCPUActive)) {
        vcpu_disable(عدم);
        ARCH_گره_STATE(armHSVCPUActive) = false;
    }
    ARCH_گره_STATE(armHSCurVCPU) = عدم;
}

فراغ vcpu_flush(فراغ)
{
    إذا (ARCH_گره_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_گره_STATE(armHSCurVCPU), ARCH_گره_STATE(armHSVCPUActive));
        vcpu_invalidate_فعال();
    }
}

فراغ vcpu_flush_if_current(tcb_t *tptr)
{
    إذا (tptr->tcbArch.tcbVCPU == ARCH_گره_STATE(armHSCurVCPU)) {
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

    إذا (tcb == گره_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

فراغ dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    إذا (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    إذا (vcpu == ARCH_گره_STATE(armHSCurVCPU)) {
        vcpu_invalidate_فعال();
    }
    tcb->tcbArch.tcbVCPU = عدم;
    vcpu->vcpuTCB = عدم;
#اگر_تعریف_شده ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_نخها
    Arch_debugDissociateVCPUTCB(tcb);
#پایان_اگر

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#اگر_تعریف_شده CONFIG_ARCH_AARCH64
    مجموعه_ثبات(tcb, SPSR_EL1, sanitise_ثبات(SPSR_EL1, get_ثبات(tcb, SPSR_EL1), false));
#وإلا
    مجموعه_ثبات(tcb, CPSR, sanitise_ثبات(CPSR, get_ثبات(tcb, CPSR), false));
#پایان_اگر
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, کلمه_t فیلد, کلمه_t مقدار)
{
    writeVCPUReg(vcpu, فیلد, مقدار);
    أرجع EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    کلمه_t فیلد;
    کلمه_t مقدار;
    إذا (درازی < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    فیلد = getSyscallArg(0, میانگیر);
    مقدار = getSyscallArg(1, میانگیر);
    إذا (فیلد >= se_L4_VCPUReg_رقم) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (دراز)فیلد);
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), فیلد, مقدار);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, کلمه_t فیلد, bool_t call)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    کلمه_t مقدار = readVCPUReg(vcpu, فیلد);
    إذا (call) {
        کلمه_t *ipc_میانگیر = lookupIPCBuffer(true, thread);
        مجموعه_ثبات(thread, badge_ثبات, 0);
        بیعلامت صحيح درازی = مجموعه_MR(thread, ipc_میانگیر, 0, مقدار);
        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, درازی)));
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, کلمه_t درازی, bool_t call, کلمه_t *میانگیر)
{
    کلمه_t فیلد;
    إذا (درازی < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    فیلد = getSyscallArg(0, میانگیر);

    إذا (فیلد >= se_L4_VCPUReg_رقم) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (دراز)فیلد);
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), فیلد, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, بیعلامت دراز شاخص, virq_t virq)
{
    إذا (likely(ARCH_گره_STATE(armHSCurVCPU) == vcpu)) {
        مجموعه_gic_vcpu_ctrl_lr(شاخص, virq);
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    } وإلا إذا (vcpu->vcpuTCB != عدم && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(Ipi_Remote_Call_VCPUInject_وقفه,
                       (کلمه_t)vcpu, شاخص, virq.واژگان[0],
                       vcpu->vcpuTCB->tcbAffinity);
#پایان_اگر /* CONFIG_ENABLE_SMP */
    } وإلا {
        vcpu->vgic.lr[شاخص] = virq;
    }

    أرجع EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    کلمه_t vid, اولویت, گروه, شاخص;
    vcpu_t *vcpu;
#اگر_تعریف_شده CONFIG_ARCH_AARCH64
    کلمه_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    إذا (درازی < 1) {
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, میانگیر);
    vid = mr0 & 0xffff;
    اولویت = (mr0 >> 16) & 0xff;
    گروه = (mr0 >> 24) & 0xff;
    شاخص = (mr0 >> 32) & 0xff;
#وإلا
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    إذا (درازی < 2) {
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, میانگیر);
    mr1 = getSyscallArg(1, میانگیر);
    vid = mr0 & 0xffff;
    اولویت = (mr0 >> 16) & 0xff;
    گروه = (mr0 >> 24) & 0xff;
    شاخص = mr1 & 0xff;
#پایان_اگر

    /* Check IRQ parameters */
    إذا (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_آرگومان_عدد = 1;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (اولویت > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_آرگومان_عدد = 2;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (گروه > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_آرگومان_عدد = 3;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    إذا (شاخص >= gic_vcpu_رقم_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_رقم_list_regs - 1;
        current_syscall_error.invalid_آرگومان_عدد = 4;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    إذا (virq_get_virqType(vcpu->vgic.lr[شاخص]) == virq_virq_فعال) {
        userError("VGIC List register in use.");
        current_syscall_error.type = se_L4_Delete_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(گروه, اولویت, 1, vid);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUInjectIRQ(vcpu, شاخص, virq);
}

exception_t decodeARMVCPUInvocation(
    کلمه_t label,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *جایگاه,
    cap_t cap,
    bool_t call,
    کلمه_t *میانگیر
)
{
    بدل (label) {
    حالة ARMVCPUSetTCB:
        أرجع decodeVCPUSetTCB(cap);
    حالة ARMVCPUReadReg:
        أرجع decodeVCPUReadReg(cap, درازی, call, میانگیر);
    حالة ARMVCPUWriteReg:
        أرجع decodeVCPUWriteReg(cap, درازی, میانگیر);
    حالة ARMVCPUInjectIRQ:
        أرجع decodeVCPUInjectIRQ(cap, درازی, میانگیر);
    حالة ARMVCPUAckVPPI:
        أرجع decodeVCPUAckVPPI(cap, درازی, میانگیر);
    افتراضي:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    إذا (درازی < 1) {
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کلمه_t irq_w = getSyscallArg(0, میانگیر);
    irq_t irq = (irq_t) CORE_IRQ_مقصد_IRQT(CURRENT_CPU_شاخص(), irq_w);
    exception_t وضعیت = Arch_checkIRQ(irq_w);
    إذا (وضعیت != EXCEPTION_NONE) {
        أرجع وضعیت;
    }

    VPPIEventIRQ_t vppi = irq_VPPIEvent_شاخص(irq);
    إذا (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
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
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    أرجع EXCEPTION_NONE;
}


فراغ handleVCPUFault(کلمه_t hsr)
{
    MCS_DO_IF_BUDGET({
        إذا (armv_handleVCPUFault(hsr))
        {
            أرجع;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(گره_STATE(ksCurThread));
    })
    زمانبندی();
    activateThread();
}

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
فراغ handle_VCPUInject_وقفه_IPI(vcpu_t *vcpu, بیعلامت دراز شاخص, virq_t virq)
{
    إذا (likely(ARCH_گره_STATE(armHSCurVCPU) == vcpu)) {
        مجموعه_gic_vcpu_ctrl_lr(شاخص, virq);
    } وإلا {
        vcpu->vgic.lr[شاخص] = virq;
    }
}
#پایان_اگر /* ENABLE_SMP_SUPPORT */

#پایان_اگر
