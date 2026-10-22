/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT

#شامل <arch/object/vcpu.h>
#شامل <armv/vcpu.h>
#شامل <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#شامل <arch/machine/debug_conf.h>
#شامل <drivers/timer/arm_generic.h>
#شامل <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_رمز خالی vcpu_boot_init(خالی)
{
    armv_vcpu_boot_init();
    gic_vcpu_عدد_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    اگر (gic_vcpu_عدد_list_regs > GIC_VCPU_MAX_عدد_LR) {
        لکھو("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_عدد_list_regs = GIC_VCPU_MAX_عدد_LR;
    }
    vcpu_disable(تہی);
    ARCH_گرہ_STATE(armHSCurVCPU) = تہی;
    ARCH_گرہ_STATE(armHSVCPUActive) = false;

}

ساکن خالی vcpu_save(vcpu_t *vcpu, bool_t فعال)
{
    لفظ_t i;
    لفظ_t lr_عدد;

    أكد(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    اگر (فعال) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_عدد = gic_vcpu_عدد_list_regs;
    برائے (i = 0; i < lr_عدد; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, فعال);
}


ساکن لفظ_t readVCPUReg(vcpu_t *vcpu, لفظ_t میدان)
{
    اگر (likely(ARCH_گرہ_STATE(armHSCurVCPU) == vcpu)) {
        اگر (vcpu_reg_saved_when_disabled(میدان) && !ARCH_گرہ_STATE(armHSVCPUActive)) {
            واپس vcpu_read_reg(vcpu, میدان);
        } ورنہ {
            واپس vcpu_hw_read_reg(میدان);
        }
    } ورنہ {
        واپس vcpu_read_reg(vcpu, میدان);
    }
}

ساکن خالی writeVCPUReg(vcpu_t *vcpu, لفظ_t میدان, لفظ_t قدر)
{
    اگر (likely(ARCH_گرہ_STATE(armHSCurVCPU) == vcpu)) {
        اگر (vcpu_reg_saved_when_disabled(میدان) && !ARCH_گرہ_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, میدان, قدر);
        } ورنہ {
            vcpu_hw_write_reg(میدان, قدر);
        }
    } ورنہ {
        vcpu_write_reg(vcpu, میدان, قدر);
    }
}

خالی vcpu_restore(vcpu_t *vcpu)
{
    أكد(vcpu);
    لفظ_t i;
    لفظ_t lr_عدد;
    /* Turn off the VGIC */
    سیٹ_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    سیٹ_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    سیٹ_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_عدد = gic_vcpu_عدد_list_regs;
    برائے (i = 0; i < lr_عدد; i++) {
        سیٹ_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, se_L4_VCPUReg_Save_Range_آغاز, se_L4_VCPUReg_Save_Range_انتہا);
    vcpu_enable(vcpu);
}

خالی VPPIEvent(irq_t irq)
{
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
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
    اگر (thread_state_get_tcbQueued(گرہ_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(گرہ_STATE(ksCurThread))) {
        واپس;
    }
#اگر_ختم

    اگر (ARCH_گرہ_STATE(armHSVCPUActive)) {
        نقاب_مداخلت(true, irq);
        أكد(irq_VPPIEvent_اشاریہ(irq) != VPPIEventIRQ_invalid);
        ARCH_گرہ_STATE(armHSCurVCPU)->vppi_masked[irq_VPPIEvent_اشاریہ(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_مقصد_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        أكد(isRunnable(گرہ_STATE(ksCurThread)));
        اگر (isRunnable(گرہ_STATE(ksCurThread))) {
            handleFault(گرہ_STATE(ksCurThread));
        }
    }
}

خالی VGICMaintenance(خالی)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    اگر (thread_state_get_tcbQueued(گرہ_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(گرہ_STATE(ksCurThread))) {
        واپس;
    }
#اگر_ختم

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    اگر (!ARCH_گرہ_STATE(armHSVCPUActive)) {
        لکھو("Received VGIC maintenance without active VCPU!\n");
        واپس;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    اگر (flags & VGIC_MISR_EOI) {
        صحیح irq_idx;
        اگر (eisr0) {
            irq_idx = ctzl(eisr0);
        } ورنہ اگر (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } ورنہ {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        اگر (irq_idx == -1  || irq_idx >= gic_vcpu_عدد_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } ورنہ {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            انتخاب (virq_get_virqType(virq)) {
            حالت virq_virq_فعال:
                virq = virq_virq_فعال_سیٹ_virq_EOIIRQEN(virq, 0);
                توڑو;
            حالت virq_virq_pending:
                virq = virq_virq_pending_سیٹ_virq_EOIIRQEN(virq, 0);
                توڑو;
            حالت virq_virq_invalid:
                virq = virq_virq_invalid_سیٹ_virq_EOIIRQEN(virq, 0);
                توڑو;
            }
            سیٹ_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            أكد(ARCH_گرہ_STATE(armHSCurVCPU) != تہی && ARCH_گرہ_STATE(armHSVCPUActive));
            اگر (ARCH_گرہ_STATE(armHSCurVCPU) != تہی && ARCH_گرہ_STATE(armHSVCPUActive)) {
                ARCH_گرہ_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } ورنہ {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } ورنہ {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    أكد(isRunnable(گرہ_STATE(ksCurThread)));
    اگر (isRunnable(گرہ_STATE(ksCurThread))) {
        handleFault(گرہ_STATE(ksCurThread));
    }
}

خالی vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#اگر_تعریف_شدہ CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.آخری_pcount = 0;
#اگر_ختم
}

خالی vcpu_switch(vcpu_t *new)
{
    اگر (likely(ARCH_گرہ_STATE(armHSCurVCPU) != new)) {
        اگر (unlikely(new != تہی)) {
            اگر (unlikely(ARCH_گرہ_STATE(armHSCurVCPU) != تہی)) {
                vcpu_save(ARCH_گرہ_STATE(armHSCurVCPU), ARCH_گرہ_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_گرہ_STATE(armHSCurVCPU) = new;
            ARCH_گرہ_STATE(armHSVCPUActive) = true;
        } ورنہ اگر (unlikely(ARCH_گرہ_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#اگر_تعریف_شدہ ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_دھاگے
            saveAllBreakpointState(ARCH_گرہ_STATE(armHSCurVCPU)->vcpuTCB);
#اگر_ختم
            vcpu_disable(ARCH_گرہ_STATE(armHSCurVCPU));
            ARCH_گرہ_STATE(armHSVCPUActive) = false;
        }
    } ورنہ اگر (likely(!ARCH_گرہ_STATE(armHSVCPUActive) && new != تہی)) {
        isb();
        vcpu_enable(new);
        ARCH_گرہ_STATE(armHSVCPUActive) = true;
    }
}

ساکن خالی vcpu_invalidate_فعال(خالی)
{
    اگر (ARCH_گرہ_STATE(armHSVCPUActive)) {
        vcpu_disable(تہی);
        ARCH_گرہ_STATE(armHSVCPUActive) = false;
    }
    ARCH_گرہ_STATE(armHSCurVCPU) = تہی;
}

خالی vcpu_flush(خالی)
{
    اگر (ARCH_گرہ_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_گرہ_STATE(armHSCurVCPU), ARCH_گرہ_STATE(armHSVCPUActive));
        vcpu_invalidate_فعال();
    }
}

خالی vcpu_flush_if_current(tcb_t *tptr)
{
    اگر (tptr->tcbArch.tcbVCPU == ARCH_گرہ_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

خالی vcpu_finalise(vcpu_t *vcpu)
{
    اگر (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

خالی associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    اگر (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    اگر (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    اگر (tcb == گرہ_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

خالی dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    اگر (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    اگر (vcpu == ARCH_گرہ_STATE(armHSCurVCPU)) {
        vcpu_invalidate_فعال();
    }
    tcb->tcbArch.tcbVCPU = تہی;
    vcpu->vcpuTCB = تہی;
#اگر_تعریف_شدہ ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_دھاگے
    Arch_debugDissociateVCPUTCB(tcb);
#اگر_ختم

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    سیٹ_رجسٹر(tcb, SPSR_EL1, sanitise_رجسٹر(SPSR_EL1, get_رجسٹر(tcb, SPSR_EL1), false));
#ورنہ
    سیٹ_رجسٹر(tcb, CPSR, sanitise_رجسٹر(CPSR, get_رجسٹر(tcb, CPSR), false));
#اگر_ختم
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, لفظ_t میدان, لفظ_t قدر)
{
    writeVCPUReg(vcpu, میدان, قدر);
    واپس EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    لفظ_t میدان;
    لفظ_t قدر;
    اگر (لمبائی < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    میدان = getSyscallArg(0, بفر);
    قدر = getSyscallArg(1, بفر);
    اگر (میدان >= se_L4_VCPUReg_عدد) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (طویل)میدان);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), میدان, قدر);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, لفظ_t میدان, bool_t call)
{
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    لفظ_t قدر = readVCPUReg(vcpu, میدان);
    اگر (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        غیرعلامتی صحیح لمبائی = سیٹ_MR(thread, ipc_بفر, 0, قدر);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, لمبائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    واپس EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, لفظ_t لمبائی, bool_t call, لفظ_t *بفر)
{
    لفظ_t میدان;
    اگر (لمبائی < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    میدان = getSyscallArg(0, بفر);

    اگر (میدان >= se_L4_VCPUReg_عدد) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (طویل)میدان);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), میدان, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, غیرعلامتی طویل اشاریہ, virq_t virq)
{
    اگر (likely(ARCH_گرہ_STATE(armHSCurVCPU) == vcpu)) {
        سیٹ_gic_vcpu_ctrl_lr(اشاریہ, virq);
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    } ورنہ اگر (vcpu->vcpuTCB != تہی && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(Ipi_Remote_Call_VCPUInject_مداخلت,
                       (لفظ_t)vcpu, اشاریہ, virq.الفاظ[0],
                       vcpu->vcpuTCB->tcbAffinity);
#اگر_ختم /* CONFIG_ENABLE_SMP */
    } ورنہ {
        vcpu->vgic.lr[اشاریہ] = virq;
    }

    واپس EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    لفظ_t vid, ترجیح, گروہ, اشاریہ;
    vcpu_t *vcpu;
#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    لفظ_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    اگر (لمبائی < 1) {
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, بفر);
    vid = mr0 & 0xffff;
    ترجیح = (mr0 >> 16) & 0xff;
    گروہ = (mr0 >> 24) & 0xff;
    اشاریہ = (mr0 >> 32) & 0xff;
#ورنہ
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    اگر (لمبائی < 2) {
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, بفر);
    mr1 = getSyscallArg(1, بفر);
    vid = mr0 & 0xffff;
    ترجیح = (mr0 >> 16) & 0xff;
    گروہ = (mr0 >> 24) & 0xff;
    اشاریہ = mr1 & 0xff;
#اگر_ختم

    /* Check IRQ parameters */
    اگر (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        current_syscall_error.type = seL4_RangeError;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (ترجیح > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalid_دلیل_نمبر = 2;
        current_syscall_error.type = seL4_RangeError;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (گروہ > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalid_دلیل_نمبر = 3;
        current_syscall_error.type = seL4_RangeError;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    اگر (اشاریہ >= gic_vcpu_عدد_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_عدد_list_regs - 1;
        current_syscall_error.invalid_دلیل_نمبر = 4;
        current_syscall_error.type = seL4_RangeError;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    اگر (virq_get_virqType(vcpu->vgic.lr[اشاریہ]) == virq_virq_فعال) {
        userError("VGIC List register in use.");
        current_syscall_error.type = se_L4_Delete_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(گروہ, ترجیح, 1, vid);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeVCPUInjectIRQ(vcpu, اشاریہ, virq);
}

exception_t decodeARMVCPUInvocation(
    لفظ_t label,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *خانہ,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    انتخاب (label) {
    حالت ARMVCPUSetTCB:
        واپس decodeVCPUSetTCB(cap);
    حالت ARMVCPUReadReg:
        واپس decodeVCPUReadReg(cap, لمبائی, call, بفر);
    حالت ARMVCPUWriteReg:
        واپس decodeVCPUWriteReg(cap, لمبائی, بفر);
    حالت ARMVCPUInjectIRQ:
        واپس decodeVCPUInjectIRQ(cap, لمبائی, بفر);
    حالت ARMVCPUAckVPPI:
        واپس decodeVCPUAckVPPI(cap, لمبائی, بفر);
    طےشدہ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    اگر (لمبائی < 1) {
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t irq_w = getSyscallArg(0, بفر);
    irq_t irq = (irq_t) CORE_IRQ_مقصد_IRQT(CURRENT_CPU_اشاریہ(), irq_w);
    exception_t حالت_2 = Arch_checkIRQ(irq_w);
    اگر (حالت_2 != EXCEPTION_NONE) {
        واپس حالت_2;
    }

    VPPIEventIRQ_t vppi = irq_VPPIEvent_اشاریہ(irq);
    اگر (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    واپس EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    اگر (current_extra_caps.excaprefs[0] == تہی) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    واپس EXCEPTION_NONE;
}


خالی handleVCPUFault(لفظ_t hsr)
{
    MCS_DO_IF_BUDGET({
        اگر (armv_handleVCPUFault(hsr))
        {
            واپس;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(گرہ_STATE(ksCurThread));
    })
    جدول_وقت();
    activateThread();
}

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
خالی handle_VCPUInject_مداخلت_IPI(vcpu_t *vcpu, غیرعلامتی طویل اشاریہ, virq_t virq)
{
    اگر (likely(ARCH_گرہ_STATE(armHSCurVCPU) == vcpu)) {
        سیٹ_gic_vcpu_ctrl_lr(اشاریہ, virq);
    } ورنہ {
        vcpu->vgic.lr[اشاریہ] = virq;
    }
}
#اگر_ختم /* ENABLE_SMP_SUPPORT */

#اگر_ختم
