/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT

#ضمن <arch/object/vcpu.h>
#ضمن <armv/vcpu.h>
#ضمن <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#ضمن <arch/machine/debug_conf.h>
#ضمن <drivers/timer/arm_generic.h>
#ضمن <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_ڪوڊ فراغ vcpu_boot_init(فراغ)
{
    armv_vcpu_boot_init();
    gic_vcpu_انگ_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    إذا (gic_vcpu_انگ_list_regs > GIC_VCPU_MAX_انگ_LR) {
        لکو("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_انگ_list_regs = GIC_VCPU_MAX_انگ_LR;
    }
    vcpu_disable(عدم);
    ARCH_ڳنڍ_STATE(armHSCurVCPU) = عدم;
    ARCH_ڳنڍ_STATE(armHSVCPUActive) = false;

}

ساكن فراغ vcpu_save(vcpu_t *vcpu, bool_t سرگرم)
{
    لفظ_t i;
    لفظ_t lr_انگ;

    أكد(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    إذا (سرگرم) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_انگ = gic_vcpu_انگ_list_regs;
    لكل (i = 0; i < lr_انگ; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, سرگرم);
}


ساكن لفظ_t readVCPUReg(vcpu_t *vcpu, لفظ_t ميدان)
{
    إذا (likely(ARCH_ڳنڍ_STATE(armHSCurVCPU) == vcpu)) {
        إذا (vcpu_reg_saved_when_disabled(ميدان) && !ARCH_ڳنڍ_STATE(armHSVCPUActive)) {
            أرجع vcpu_read_reg(vcpu, ميدان);
        } وإلا {
            أرجع vcpu_hw_read_reg(ميدان);
        }
    } وإلا {
        أرجع vcpu_read_reg(vcpu, ميدان);
    }
}

ساكن فراغ writeVCPUReg(vcpu_t *vcpu, لفظ_t ميدان, لفظ_t قدر)
{
    إذا (likely(ARCH_ڳنڍ_STATE(armHSCurVCPU) == vcpu)) {
        إذا (vcpu_reg_saved_when_disabled(ميدان) && !ARCH_ڳنڍ_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, ميدان, قدر);
        } وإلا {
            vcpu_hw_write_reg(ميدان, قدر);
        }
    } وإلا {
        vcpu_write_reg(vcpu, ميدان, قدر);
    }
}

فراغ vcpu_restore(vcpu_t *vcpu)
{
    أكد(vcpu);
    لفظ_t i;
    لفظ_t lr_انگ;
    /* Turn off the VGIC */
    سيٽ_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    سيٽ_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    سيٽ_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_انگ = gic_vcpu_انگ_list_regs;
    لكل (i = 0; i < lr_انگ; i++) {
        سيٽ_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, se_L4_VCPUReg_Save_Range_شروعات, se_L4_VCPUReg_Save_Range_پڇاڙي);
    vcpu_enable(vcpu);
}

فراغ VPPIEvent(irq_t irq)
{
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
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
    إذا (thread_state_get_tcbQueued(ڳنڍ_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ڳنڍ_STATE(ksCurThread))) {
        أرجع;
    }
#جيڪڏهن_پورو

    إذا (ARCH_ڳنڍ_STATE(armHSVCPUActive)) {
        نقاب_مداخلت(true, irq);
        أكد(irq_VPPIEvent_اشاريو(irq) != VPPIEventIRQ_invalid);
        ARCH_ڳنڍ_STATE(armHSCurVCPU)->vppi_masked[irq_VPPIEvent_اشاريو(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_منزل_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        أكد(isRunnable(ڳنڍ_STATE(ksCurThread)));
        إذا (isRunnable(ڳنڍ_STATE(ksCurThread))) {
            handleFault(ڳنڍ_STATE(ksCurThread));
        }
    }
}

فراغ VGICMaintenance(فراغ)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    إذا (thread_state_get_tcbQueued(ڳنڍ_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(ڳنڍ_STATE(ksCurThread))) {
        أرجع;
    }
#جيڪڏهن_پورو

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    إذا (!ARCH_ڳنڍ_STATE(armHSVCPUActive)) {
        لکو("Received VGIC maintenance without active VCPU!\n");
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
        إذا (irq_idx == -1  || irq_idx >= gic_vcpu_انگ_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } وإلا {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            بدل (virq_get_virqType(virq)) {
            حالة virq_virq_سرگرم:
                virq = virq_virq_سرگرم_سيٽ_virq_EOIIRQEN(virq, 0);
                اكسر;
            حالة virq_virq_pending:
                virq = virq_virq_pending_سيٽ_virq_EOIIRQEN(virq, 0);
                اكسر;
            حالة virq_virq_invalid:
                virq = virq_virq_invalid_سيٽ_virq_EOIIRQEN(virq, 0);
                اكسر;
            }
            سيٽ_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            أكد(ARCH_ڳنڍ_STATE(armHSCurVCPU) != عدم && ARCH_ڳنڍ_STATE(armHSVCPUActive));
            إذا (ARCH_ڳنڍ_STATE(armHSCurVCPU) != عدم && ARCH_ڳنڍ_STATE(armHSVCPUActive)) {
                ARCH_ڳنڍ_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
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
    أكد(isRunnable(ڳنڍ_STATE(ksCurThread)));
    إذا (isRunnable(ڳنڍ_STATE(ksCurThread))) {
        handleFault(ڳنڍ_STATE(ksCurThread));
    }
}

فراغ vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#جيڪڏهن_وصف_ٿيل CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.آخري_pcount = 0;
#جيڪڏهن_پورو
}

فراغ vcpu_switch(vcpu_t *new)
{
    إذا (likely(ARCH_ڳنڍ_STATE(armHSCurVCPU) != new)) {
        إذا (unlikely(new != عدم)) {
            إذا (unlikely(ARCH_ڳنڍ_STATE(armHSCurVCPU) != عدم)) {
                vcpu_save(ARCH_ڳنڍ_STATE(armHSCurVCPU), ARCH_ڳنڍ_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_ڳنڍ_STATE(armHSCurVCPU) = new;
            ARCH_ڳنڍ_STATE(armHSVCPUActive) = true;
        } وإلا إذا (unlikely(ARCH_ڳنڍ_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#جيڪڏهن_وصف_ٿيل ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_ڌاڳا
            saveAllBreakpointState(ARCH_ڳنڍ_STATE(armHSCurVCPU)->vcpuTCB);
#جيڪڏهن_پورو
            vcpu_disable(ARCH_ڳنڍ_STATE(armHSCurVCPU));
            ARCH_ڳنڍ_STATE(armHSVCPUActive) = false;
        }
    } وإلا إذا (likely(!ARCH_ڳنڍ_STATE(armHSVCPUActive) && new != عدم)) {
        isb();
        vcpu_enable(new);
        ARCH_ڳنڍ_STATE(armHSVCPUActive) = true;
    }
}

ساكن فراغ vcpu_invalidate_سرگرم(فراغ)
{
    إذا (ARCH_ڳنڍ_STATE(armHSVCPUActive)) {
        vcpu_disable(عدم);
        ARCH_ڳنڍ_STATE(armHSVCPUActive) = false;
    }
    ARCH_ڳنڍ_STATE(armHSCurVCPU) = عدم;
}

فراغ vcpu_flush(فراغ)
{
    إذا (ARCH_ڳنڍ_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_ڳنڍ_STATE(armHSCurVCPU), ARCH_ڳنڍ_STATE(armHSVCPUActive));
        vcpu_invalidate_سرگرم();
    }
}

فراغ vcpu_flush_if_current(tcb_t *tptr)
{
    إذا (tptr->tcbArch.tcbVCPU == ARCH_ڳنڍ_STATE(armHSCurVCPU)) {
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

    إذا (tcb == ڳنڍ_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

فراغ dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    إذا (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    إذا (vcpu == ARCH_ڳنڍ_STATE(armHSCurVCPU)) {
        vcpu_invalidate_سرگرم();
    }
    tcb->tcbArch.tcbVCPU = عدم;
    vcpu->vcpuTCB = عدم;
#جيڪڏهن_وصف_ٿيل ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_ڌاڳا
    Arch_debugDissociateVCPUTCB(tcb);
#جيڪڏهن_پورو

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    سيٽ_رجسٽر(tcb, SPSR_EL1, sanitise_رجسٽر(SPSR_EL1, get_رجسٽر(tcb, SPSR_EL1), false));
#وإلا
    سيٽ_رجسٽر(tcb, CPSR, sanitise_رجسٽر(CPSR, get_رجسٽر(tcb, CPSR), false));
#جيڪڏهن_پورو
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, لفظ_t ميدان, لفظ_t قدر)
{
    writeVCPUReg(vcpu, ميدان, قدر);
    أرجع EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    لفظ_t ميدان;
    لفظ_t قدر;
    إذا (ڊيگهائي < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    ميدان = getSyscallArg(0, بفر);
    قدر = getSyscallArg(1, بفر);
    إذا (ميدان >= se_L4_VCPUReg_انگ) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (ڊگهو)ميدان);
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ميدان, قدر);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, لفظ_t ميدان, bool_t call)
{
    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);
    لفظ_t قدر = readVCPUReg(vcpu, ميدان);
    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
        بنانشان صحيح ڊيگهائي = سيٽ_MR(thread, ipc_بفر, 0, قدر);
        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, ڊيگهائي)));
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, لفظ_t ڊيگهائي, bool_t call, لفظ_t *بفر)
{
    لفظ_t ميدان;
    إذا (ڊيگهائي < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ميدان = getSyscallArg(0, بفر);

    إذا (ميدان >= se_L4_VCPUReg_انگ) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (ڊگهو)ميدان);
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ميدان, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, بنانشان ڊگهو اشاريو, virq_t virq)
{
    إذا (likely(ARCH_ڳنڍ_STATE(armHSCurVCPU) == vcpu)) {
        سيٽ_gic_vcpu_ctrl_lr(اشاريو, virq);
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    } وإلا إذا (vcpu->vcpuTCB != عدم && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(Ipi_Remote_Call_VCPUInject_مداخلت,
                       (لفظ_t)vcpu, اشاريو, virq.لفظ[0],
                       vcpu->vcpuTCB->tcbAffinity);
#جيڪڏهن_پورو /* CONFIG_ENABLE_SMP */
    } وإلا {
        vcpu->vgic.lr[اشاريو] = virq;
    }

    أرجع EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    لفظ_t vid, ترجيح, گروهه, اشاريو;
    vcpu_t *vcpu;
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    لفظ_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    إذا (ڊيگهائي < 1) {
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, بفر);
    vid = mr0 & 0xffff;
    ترجيح = (mr0 >> 16) & 0xff;
    گروهه = (mr0 >> 24) & 0xff;
    اشاريو = (mr0 >> 32) & 0xff;
#وإلا
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    إذا (ڊيگهائي < 2) {
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, بفر);
    mr1 = getSyscallArg(1, بفر);
    vid = mr0 & 0xffff;
    ترجيح = (mr0 >> 16) & 0xff;
    گروهه = (mr0 >> 24) & 0xff;
    اشاريو = mr1 & 0xff;
#جيڪڏهن_پورو

    /* Check IRQ parameters */
    إذا (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_دليل_عدد = 1;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (ترجيح > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_دليل_عدد = 2;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (گروهه > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_دليل_عدد = 3;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    إذا (اشاريو >= gic_vcpu_انگ_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_انگ_list_regs - 1;
        current_syscall_error.invalid_دليل_عدد = 4;
        current_syscall_error.type = seL4_RangeError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    إذا (virq_get_virqType(vcpu->vgic.lr[اشاريو]) == virq_virq_سرگرم) {
        userError("VGIC List register in use.");
        current_syscall_error.type = se_L4_Delete_پهريون;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(گروهه, ترجيح, 1, vid);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUInjectIRQ(vcpu, اشاريو, virq);
}

exception_t decodeARMVCPUInvocation(
    لفظ_t label,
    لفظ_t ڊيگهائي,
    cptr_t cptr,
    cte_t *جڳهه,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (label) {
    حالة ARMVCPUSetTCB:
        أرجع decodeVCPUSetTCB(cap);
    حالة ARMVCPUReadReg:
        أرجع decodeVCPUReadReg(cap, ڊيگهائي, call, بفر);
    حالة ARMVCPUWriteReg:
        أرجع decodeVCPUWriteReg(cap, ڊيگهائي, بفر);
    حالة ARMVCPUInjectIRQ:
        أرجع decodeVCPUInjectIRQ(cap, ڊيگهائي, بفر);
    حالة ARMVCPUAckVPPI:
        أرجع decodeVCPUAckVPPI(cap, ڊيگهائي, بفر);
    افتراضي:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    إذا (ڊيگهائي < 1) {
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t irq_w = getSyscallArg(0, بفر);
    irq_t irq = (irq_t) CORE_IRQ_منزل_IRQT(CURRENT_CPU_اشاريو(), irq_w);
    exception_t حالت = Arch_checkIRQ(irq_w);
    إذا (حالت != EXCEPTION_NONE) {
        أرجع حالت;
    }

    VPPIEventIRQ_t vppi = irq_VPPIEvent_اشاريو(irq);
    إذا (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
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
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    أرجع EXCEPTION_NONE;
}


فراغ handleVCPUFault(لفظ_t hsr)
{
    MCS_DO_IF_BUDGET({
        إذا (armv_handleVCPUFault(hsr))
        {
            أرجع;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(ڳنڍ_STATE(ksCurThread));
    })
    وقت_جدول();
    activateThread();
}

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
فراغ handle_VCPUInject_مداخلت_IPI(vcpu_t *vcpu, بنانشان ڊگهو اشاريو, virq_t virq)
{
    إذا (likely(ARCH_ڳنڍ_STATE(armHSCurVCPU) == vcpu)) {
        سيٽ_gic_vcpu_ctrl_lr(اشاريو, virq);
    } وإلا {
        vcpu->vgic.lr[اشاريو] = virq;
    }
}
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

#جيڪڏهن_پورو
