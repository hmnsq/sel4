/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#include <arch/object/vcpu.h>
#include <armv/vcpu.h>
#include <arch/machine/debug.h> /* Arch_debug[A/Di]ssociateVCPUTCB() */
#include <arch/machine/debug_conf.h>
#include <drivers/timer/arm_generic.h>
#include <plat/platform_gen.h> /* Ensure correct GIC header is included */

BOOT_CODE void vcpu_boot_init(void)
{
    armv_vcpu_boot_init();
    gic_vcpu_num_list_regs = VGIC_VTR_NLISTREGS(get_gic_vcpu_ctrl_vtr());
    ꦪꦼꦤ꧀ (gic_vcpu_num_list_regs > GIC_VCPU_MAX_NUM_LR) {
        printf("Warning: VGIC is reporting more list registers than we support. Truncating\n");
        gic_vcpu_num_list_regs = GIC_VCPU_MAX_NUM_LR;
    }
    vcpu_disable(NULL);
    ARCH_NODE_STATE(armHSCurVCPU) = NULL;
    ARCH_NODE_STATE(armHSVCPUActive) = false;

}

static void vcpu_save(vcpu_t *vcpu, bool_t active)
{
    word_t i;
    word_t lr_num;

    assert(vcpu);
    dsb();
    /* If we aren't active then this state already got stored when
     * we were disabled */
    ꦪꦼꦤ꧀ (active) {
        vcpu_save_reg(vcpu, seL4_VCPUReg_SCTLR);
        vcpu->vgic.hcr = get_gic_vcpu_ctrl_hcr();
        save_virt_timer(vcpu);
    }

    /* Store GIC VCPU control state */
    vcpu->vgic.vmcr = get_gic_vcpu_ctrl_vmcr();
    vcpu->vgic.apr = get_gic_vcpu_ctrl_apr();
    lr_num = gic_vcpu_num_list_regs;
    ꦏꦁꦒꦺꦴ (i = 0; i < lr_num; i++) {
        vcpu->vgic.lr[i] = get_gic_vcpu_ctrl_lr(i);
    }
    armv_vcpu_save(vcpu, active);
}


static word_t readVCPUReg(vcpu_t *vcpu, word_t field)
{
    ꦪꦼꦤ꧀ (likely(ARCH_NODE_STATE(armHSCurVCPU) == vcpu)) {
        ꦪꦼꦤ꧀ (vcpu_reg_saved_when_disabled(field) && !ARCH_NODE_STATE(armHSVCPUActive)) {
            ꦮꦭꦶ vcpu_read_reg(vcpu, field);
        } ꦭꦶꦪ {
            ꦮꦭꦶ vcpu_hw_read_reg(field);
        }
    } ꦭꦶꦪ {
        ꦮꦭꦶ vcpu_read_reg(vcpu, field);
    }
}

static void writeVCPUReg(vcpu_t *vcpu, word_t field, word_t value)
{
    ꦪꦼꦤ꧀ (likely(ARCH_NODE_STATE(armHSCurVCPU) == vcpu)) {
        ꦪꦼꦤ꧀ (vcpu_reg_saved_when_disabled(field) && !ARCH_NODE_STATE(armHSVCPUActive)) {
            vcpu_write_reg(vcpu, field, value);
        } ꦭꦶꦪ {
            vcpu_hw_write_reg(field, value);
        }
    } ꦭꦶꦪ {
        vcpu_write_reg(vcpu, field, value);
    }
}

void vcpu_restore(vcpu_t *vcpu)
{
    assert(vcpu);
    word_t i;
    word_t lr_num;
    /* Turn off the VGIC */
    set_gic_vcpu_ctrl_hcr(0);
    isb();

    /* Restore GIC VCPU control state */
    set_gic_vcpu_ctrl_vmcr(vcpu->vgic.vmcr);
    set_gic_vcpu_ctrl_apr(vcpu->vgic.apr);
    lr_num = gic_vcpu_num_list_regs;
    ꦏꦁꦒꦺꦴ (i = 0; i < lr_num; i++) {
        set_gic_vcpu_ctrl_lr(i, vcpu->vgic.lr[i]);
    }

    /* restore registers */
    vcpu_restore_reg_range(vcpu, seL4_VCPURegSaveRange_start, seL4_VCPURegSaveRange_end);
    vcpu_enable(vcpu);
}

void VPPIEvent(irq_t irq)
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
    ꦪꦼꦤ꧀ (thread_state_get_tcbQueued(NODE_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(NODE_STATE(ksCurThread))) {
        ꦮꦭꦶ;
    }
#endif

    ꦪꦼꦤ꧀ (ARCH_NODE_STATE(armHSVCPUActive)) {
        maskInterrupt(true, irq);
        assert(irqVPPIEventIndex(irq) != VPPIEventIRQ_invalid);
        ARCH_NODE_STATE(armHSCurVCPU)->vppi_masked[irqVPPIEventIndex(irq)] = true;
        current_fault = seL4_Fault_VPPIEvent_new(IRQT_TO_IRQ(irq));
        /* Current VCPU being active should indicate that the current thread
         * is runnable. At present, verification cannot establish this so we
         * perform an extra check. */
        assert(isRunnable(NODE_STATE(ksCurThread)));
        ꦪꦼꦤ꧀ (isRunnable(NODE_STATE(ksCurThread))) {
            handleFault(NODE_STATE(ksCurThread));
        }
    }
}

void VGICMaintenance(void)
{
    uint32_t eisr0, eisr1;
    uint32_t flags;

#ifdef CONFIG_KERNEL_MCS
    /* See VPPIEvent for details on this check. */
    ꦪꦼꦤ꧀ (thread_state_get_tcbQueued(NODE_STATE(ksCurThread)->tcbState) ||
        !isSchedulable(NODE_STATE(ksCurThread))) {
        ꦮꦭꦶ;
    }
#endif

    /* We shouldn't get a VGICMaintenance interrupt while a VCPU isn't active,
     * but if one becomes pending before the VGIC is disabled we might get one
     * when returning to userlevel after disabling the current VCPU. In this
     * case we simply return and rely on the interrupt being raised again when
     * the VCPU is reenabled.
     */
    ꦪꦼꦤ꧀ (!ARCH_NODE_STATE(armHSVCPUActive)) {
        printf("Received VGIC maintenance without active VCPU!\n");
        ꦮꦭꦶ;
    }

    eisr0 = get_gic_vcpu_ctrl_eisr0();
    eisr1 = get_gic_vcpu_ctrl_eisr1();
    flags = get_gic_vcpu_ctrl_misr();

    ꦪꦼꦤ꧀ (flags & VGIC_MISR_EOI) {
        puur_naa_nka irq_idx;
        ꦪꦼꦤ꧀ (eisr0) {
            irq_idx = ctzl(eisr0);
        } ꦭꦶꦪ ꦪꦼꦤ꧀ (eisr1) {
            irq_idx = ctzl(eisr1) + 32;
        } ꦭꦶꦪ {
            irq_idx = -1;
        }

        /* the hardware should never give us an invalid index, but we don't
         * want to trust it that far */
        ꦪꦼꦤ꧀ (irq_idx == -1  || irq_idx >= gic_vcpu_num_list_regs) {
            current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
        } ꦭꦶꦪ {
            virq_t virq = get_gic_vcpu_ctrl_lr(irq_idx);
            switch (virq_get_virqType(virq)) {
            pak_sa virq_virq_active:
                virq = virq_virq_active_set_virqEOIIRQEN(virq, 0);
                bha_nga;
            pak_sa virq_virq_pending:
                virq = virq_virq_pending_set_virqEOIIRQEN(virq, 0);
                bha_nga;
            pak_sa virq_virq_invalid:
                virq = virq_virq_invalid_set_virqEOIIRQEN(virq, 0);
                bha_nga;
            }
            set_gic_vcpu_ctrl_lr(irq_idx, virq);
            /* decodeVCPUInjectIRQ below checks the vgic.lr register,
             * so we should also sync the shadow data structure as well */
            assert(ARCH_NODE_STATE(armHSCurVCPU) != NULL && ARCH_NODE_STATE(armHSVCPUActive));
            ꦪꦼꦤ꧀ (ARCH_NODE_STATE(armHSCurVCPU) != NULL && ARCH_NODE_STATE(armHSVCPUActive)) {
                ARCH_NODE_STATE(armHSCurVCPU)->vgic.lr[irq_idx] = virq;
            } ꦭꦶꦪ {
                /* FIXME This should not happen */
            }
            current_fault = seL4_Fault_VGICMaintenance_new(irq_idx, 1);
        }

    } ꦭꦶꦪ {
        /* Assume that it was an EOI for a LR that was not present */
        current_fault = seL4_Fault_VGICMaintenance_new(0, 0);
    }

    /* Current VCPU being active should indicate that the current thread
     * is runnable. At present, verification cannot establish this so we
     * perform an extra check. */
    assert(isRunnable(NODE_STATE(ksCurThread)));
    ꦪꦼꦤ꧀ (isRunnable(NODE_STATE(ksCurThread))) {
        handleFault(NODE_STATE(ksCurThread));
    }
}

void vcpu_init(vcpu_t *vcpu)
{
    armv_vcpu_init(vcpu);
    /* GICH VCPU interface control. Trap VCPU access to Group 0 registers on GICv3. */
    vcpu->vgic.hcr = VGIC_HCR_EN | VGIC_HCR_TALL0;
#ifdef CONFIG_VTIMER_UPDATE_VOFFSET
    /* Virtual Timer interface */
    vcpu->virtTimer.last_pcount = 0;
#endif
}

void vcpu_switch(vcpu_t *new)
{
    ꦪꦼꦤ꧀ (likely(ARCH_NODE_STATE(armHSCurVCPU) != new)) {
        ꦪꦼꦤ꧀ (unlikely(new != NULL)) {
            ꦪꦼꦤ꧀ (unlikely(ARCH_NODE_STATE(armHSCurVCPU) != NULL)) {
                vcpu_save(ARCH_NODE_STATE(armHSCurVCPU), ARCH_NODE_STATE(armHSVCPUActive));
            }
            vcpu_restore(new);
            ARCH_NODE_STATE(armHSCurVCPU) = new;
            ARCH_NODE_STATE(armHSVCPUActive) = true;
        } ꦭꦶꦪ ꦪꦼꦤ꧀ (unlikely(ARCH_NODE_STATE(armHSVCPUActive))) {
            /* leave the current VCPU state loaded, but disable vgic and mmu */
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
            saveAllBreakpointState(ARCH_NODE_STATE(armHSCurVCPU)->vcpuTCB);
#endif
            vcpu_disable(ARCH_NODE_STATE(armHSCurVCPU));
            ARCH_NODE_STATE(armHSVCPUActive) = false;
        }
    } ꦭꦶꦪ ꦪꦼꦤ꧀ (likely(!ARCH_NODE_STATE(armHSVCPUActive) && new != NULL)) {
        isb();
        vcpu_enable(new);
        ARCH_NODE_STATE(armHSVCPUActive) = true;
    }
}

static void vcpu_invalidate_active(void)
{
    ꦪꦼꦤ꧀ (ARCH_NODE_STATE(armHSVCPUActive)) {
        vcpu_disable(NULL);
        ARCH_NODE_STATE(armHSVCPUActive) = false;
    }
    ARCH_NODE_STATE(armHSCurVCPU) = NULL;
}

void vcpu_flush(void)
{
    ꦪꦼꦤ꧀ (ARCH_NODE_STATE(armHSCurVCPU)) {
        vcpu_save(ARCH_NODE_STATE(armHSCurVCPU), ARCH_NODE_STATE(armHSVCPUActive));
        vcpu_invalidate_active();
    }
}

void vcpu_flush_if_current(tcb_t *tptr)
{
    ꦪꦼꦤ꧀ (tptr->tcbArch.tcbVCPU == ARCH_NODE_STATE(armHSCurVCPU)) {
        vcpu_flush();
    }
}

void vcpu_finalise(vcpu_t *vcpu)
{
    ꦪꦼꦤ꧀ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
}

void associateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ꦪꦼꦤ꧀ (tcb->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(tcb->tcbArch.tcbVCPU, tcb);
    }
    ꦪꦼꦤ꧀ (vcpu->vcpuTCB) {
        dissociateVCPUTCB(vcpu, vcpu->vcpuTCB);
    }
    tcb->tcbArch.tcbVCPU = vcpu;
    vcpu->vcpuTCB = tcb;

    ꦪꦼꦤ꧀ (tcb == NODE_STATE(ksCurThread)) {
        vcpu_switch(vcpu);
    }
}

void dissociateVCPUTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    ꦪꦼꦤ꧀ (tcb->tcbArch.tcbVCPU != vcpu || vcpu->vcpuTCB != tcb) {
        fail("TCB and VCPU not associated.");
    }
    ꦪꦼꦤ꧀ (vcpu == ARCH_NODE_STATE(armHSCurVCPU)) {
        vcpu_invalidate_active();
    }
    tcb->tcbArch.tcbVCPU = NULL;
    vcpu->vcpuTCB = NULL;
#ifdef ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
    Arch_debugDissociateVCPUTCB(tcb);
#endif

    /* sanitize the CPSR as without a VCPU a thread should only be in user mode */
#ifdef CONFIG_ARCH_AARCH64
    setRegister(tcb, SPSR_EL1, sanitiseRegister(SPSR_EL1, getRegister(tcb, SPSR_EL1), false));
#ꦭꦶꦪ
    setRegister(tcb, CPSR, sanitiseRegister(CPSR, getRegister(tcb, CPSR), false));
#endif
}

exception_t invokeVCPUWriteReg(vcpu_t *vcpu, word_t field, word_t value)
{
    writeVCPUReg(vcpu, field, value);
    ꦮꦭꦶ EXCEPTION_NONE;
}

exception_t decodeVCPUWriteReg(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t value;
    ꦪꦼꦤ꧀ (length < 2) {
        userError("VCPUWriteReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }
    field = getSyscallArg(0, buffer);
    value = getSyscallArg(1, buffer);
    ꦪꦼꦤ꧀ (field >= seL4_VCPUReg_Num) {
        userError("VCPUWriteReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 1;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ꦮꦭꦶ invokeVCPUWriteReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, value);
}

exception_t invokeVCPUReadReg(vcpu_t *vcpu, word_t field, bool_t call)
{
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);
    word_t value = readVCPUReg(vcpu, field);
    ꦪꦼꦤ꧀ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned puur_naa_nka length = setMR(thread, ipcBuffer, 0, value);
        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    ꦮꦭꦶ EXCEPTION_NONE;
}

exception_t decodeVCPUReadReg(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    ꦪꦼꦤ꧀ (length < 1) {
        userError("VCPUReadReg: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);

    ꦪꦼꦤ꧀ (field >= seL4_VCPUReg_Num) {
        userError("VCPUReadReg: Invalid field 0x%lx.", (long)field);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 1;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ꦮꦭꦶ invokeVCPUReadReg(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call);
}

exception_t invokeVCPUInjectIRQ(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    ꦪꦼꦤ꧀ (likely(ARCH_NODE_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
#ifdef ENABLE_SMP_SUPPORT
    } ꦭꦶꦪ ꦪꦼꦤ꧀ (vcpu->vcpuTCB != NULL && vcpu->vcpuTCB->tcbAffinity != getCurrentCPUIndex()) {
        doRemoteOp3Arg(IpiRemoteCall_VCPUInjectInterrupt,
                       (word_t)vcpu, index, virq.words[0],
                       vcpu->vcpuTCB->tcbAffinity);
#endif /* CONFIG_ENABLE_SMP */
    } ꦭꦶꦪ {
        vcpu->vgic.lr[index] = virq;
    }

    ꦮꦭꦶ EXCEPTION_NONE;
}

exception_t decodeVCPUInjectIRQ(cap_t cap, word_t length, word_t *buffer)
{
    word_t vid, priority, group, index;
    vcpu_t *vcpu;
#ifdef CONFIG_ARCH_AARCH64
    word_t mr0;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ꦪꦼꦤ꧀ (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = (mr0 >> 32) & 0xff;
#ꦭꦶꦪ
    uint32_t mr0, mr1;

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ꦪꦼꦤ꧀ (length < 2) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }

    mr0 = getSyscallArg(0, buffer);
    mr1 = getSyscallArg(1, buffer);
    vid = mr0 & 0xffff;
    priority = (mr0 >> 16) & 0xff;
    group = (mr0 >> 24) & 0xff;
    index = mr1 & 0xff;
#endif

    /* Check IRQ parameters */
    ꦪꦼꦤ꧀ (vid > (1U << 10) - 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = (1U << 10) - 1;
        current_syscall_error.invalidArgumentNumber = 1;
        current_syscall_error.type = seL4_RangeError;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }
    ꦪꦼꦤ꧀ (priority > 31) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 31;
        current_syscall_error.invalidArgumentNumber = 2;
        current_syscall_error.type = seL4_RangeError;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }
    ꦪꦼꦤ꧀ (group > 1) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        current_syscall_error.invalidArgumentNumber = 3;
        current_syscall_error.type = seL4_RangeError;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index out of range */
    ꦪꦼꦤ꧀ (index >= gic_vcpu_num_list_regs) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = gic_vcpu_num_list_regs - 1;
        current_syscall_error.invalidArgumentNumber = 4;
        current_syscall_error.type = seL4_RangeError;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }
    /* LR index is in use */
    ꦪꦼꦤ꧀ (virq_get_virqType(vcpu->vgic.lr[index]) == virq_virq_active) {
        userError("VGIC List register in use.");
        current_syscall_error.type = seL4_DeleteFirst;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }
    virq_t virq = virq_virq_pending_new(group, priority, 1, vid);

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ꦮꦭꦶ invokeVCPUInjectIRQ(vcpu, index, virq);
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
    switch (label) {
    pak_sa ARMVCPUSetTCB:
        ꦮꦭꦶ decodeVCPUSetTCB(cap);
    pak_sa ARMVCPUReadReg:
        ꦮꦭꦶ decodeVCPUReadReg(cap, length, call, buffer);
    pak_sa ARMVCPUWriteReg:
        ꦮꦭꦶ decodeVCPUWriteReg(cap, length, buffer);
    pak_sa ARMVCPUInjectIRQ:
        ꦮꦭꦶ decodeVCPUInjectIRQ(cap, length, buffer);
    pak_sa ARMVCPUAckVPPI:
        ꦮꦭꦶ decodeVCPUAckVPPI(cap, length, buffer);
    default:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeVCPUAckVPPI(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ꦪꦼꦤ꧀ (length < 1) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }

    word_t irq_w = getSyscallArg(0, buffer);
    irq_t irq = (irq_t) CORE_IRQ_TO_IRQT(CURRENT_CPU_INDEX(), irq_w);
    exception_t status = Arch_checkIRQ(irq_w);
    ꦪꦼꦤ꧀ (status != EXCEPTION_NONE) {
        ꦮꦭꦶ status;
    }

    VPPIEventIRQ_t vppi = irqVPPIEventIndex(irq);
    ꦪꦼꦤ꧀ (vppi == VPPIEventIRQ_invalid) {
        userError("VCPUAckVPPI: Invalid irq number.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 0;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ꦮꦭꦶ invokeVCPUAckVPPI(vcpu, vppi);
}

exception_t invokeVCPUAckVPPI(vcpu_t *vcpu, VPPIEventIRQ_t vppi)
{
    vcpu->vppi_masked[vppi] = false;
    ꦮꦭꦶ EXCEPTION_NONE;
}

exception_t decodeVCPUSetTCB(cap_t cap)
{
    cap_t tcbCap;
    ꦪꦼꦤ꧀ (current_extra_caps.excaprefs[0] == NULL) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ꦪꦼꦤ꧀ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ꦮꦭꦶ EXCEPTION_SYSCALL_ERROR;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ꦮꦭꦶ invokeVCPUSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

exception_t invokeVCPUSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVCPUTCB(vcpu, tcb);

    ꦮꦭꦶ EXCEPTION_NONE;
}


void handleVCPUFault(word_t hsr)
{
    MCS_DO_IF_BUDGET({
        ꦪꦼꦤ꧀ (armv_handleVCPUFault(hsr))
        {
            ꦮꦭꦶ;
        }
        current_fault = seL4_Fault_VCPUFault_new(hsr);
        handleFault(NODE_STATE(ksCurThread));
    })
    schedule();
    activateThread();
}

#ifdef ENABLE_SMP_SUPPORT
void handleVCPUInjectInterruptIPI(vcpu_t *vcpu, unsigned long index, virq_t virq)
{
    ꦪꦼꦤ꧀ (likely(ARCH_NODE_STATE(armHSCurVCPU) == vcpu)) {
        set_gic_vcpu_ctrl_lr(index, virq);
    } ꦭꦶꦪ {
        vcpu->vgic.lr[index] = virq;
    }
}
#endif /* ENABLE_SMP_SUPPORT */

#endif
