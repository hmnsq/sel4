/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#ifdef CONFIG_VTX

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <object/objecttype.h>
#અન્તર્ભાવ <arch/machine/cpu_registers.h>
#અન્તર્ભાવ <arch/model/statedata.h>
#અન્તર્ભાવ <arch/object/vcpu.h>
#અન્તર્ભાવ <arch/object/ioport.h>
#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <sel4/arch/vmenter.h>

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

અચલ vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

પ્રકારવ્યાખ્યા સંરચના msr_bitmap {
    word_t bitmap[0x2000 / કદ(word_t) / 8];
} msr_bitmap_t;

પ્રકારવ્યાખ્યા સંરચના msr_bitmaps {
    msr_bitmap_t નીચલું_msr_read;
    msr_bitmap_t ઉપલું_msr_read;
    msr_bitmap_t નીચલું_msr_write;
    msr_bitmap_t ઉપલું_msr_write;
} msr_bitmaps_t;

સ્થિર msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

સ્થિર અક્ષર null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
સ્થિર uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
સ્થિર vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
સ્થિર uint32_t pin_control_ઉપલું;
સ્થિર uint32_t pin_control_નીચલું;
સ્થિર uint32_t primary_control_ઉપલું;
સ્થિર uint32_t primary_control_નીચલું;
સ્થિર uint32_t secondary_control_ઉપલું;
સ્થિર uint32_t secondary_control_નીચલું;
સ્થિર uint32_t entry_control_ઉપલું;
સ્થિર uint32_t entry_control_નીચલું;
સ્થિર uint32_t exit_control_ઉપલું;
સ્થિર uint32_t exit_control_નીચલું;
સ્થિર uint32_t cr0_ઉપલું;
સ્થિર uint32_t cr0_નીચલું;
સ્થિર uint32_t cr4_ઉપલું;
સ્થિર uint32_t cr4_નીચલું;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
સ્થિર bool_t vmx_feature_vpid;
સ્થિર bool_t vmx_feature_load_perf_global_ctrl;
સ્થિર bool_t vmx_feature_ack_on_exit;

સ્થિર vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
સ્થિર vpid_t x86KSNextVPID = VPID_FIRST;

સ્થિર શૂન્ય releaseVPID(vpid_t vpid);

સ્થિર અંતઃરેખ bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm અસ્થિર(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    પાછા !!error;
}

સ્થિર શૂન્ય vmclear(શૂન્ય *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((શૂન્ય *)vmcs_ptr);
    asm અસ્થિર(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

શૂન્ય clearCurrentVCPU(શૂન્ય)
{
    vcpu_t *vcpu = ARCH_ગાંઠ_STATE(x86KSCurrentVCPU);
    જો (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_ગાંઠ_STATE(x86KSCurrentVCPU) = ખાલી;
        જો (vcpu->fpu_active && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_active = false;
        }
    }
}

સ્થિર શૂન્ય vmptrld(શૂન્ય *vmcs_ptr)
{
    uint64_t physical_address;
    uint8_t error;
    physical_address = pptr_to_paddr(vmcs_ptr);
    asm અસ્થિર(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_address)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    પ્રતિજ્ઞા(!error);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
શૂન્ય vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_STAR_MSR, vcpu->syscall_registers[VCPU_STAR]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_MASK]);
}

શૂન્ય vcpu_restore_host_msrs(શૂન્ય)
{
    init_syscall_msrs();
}
#endif

સ્થિર શૂન્ય switchVCPU(vcpu_t *vcpu)
{
#ifdef ENABLE_SMP_SUPPORT
    જો (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_ગાંઠ_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    જો (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((અક્ષર *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.શબ્દો[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_ગાંઠ_STATE(x86KSCurrentVCPU) = vcpu;
}

સ્થિર શૂન્ય print_bits(word_t bits)
{
    bool_t first = true;
    જ્યારે (bits) {
        પૂર્ણાંક index = seL4_WordBits - 1 - clzl(bits);
        જો (first) {
            printf("%d", index);
            first = false;
        } નહીંતર {
            printf(",%d", index);
        }
        bits &= ~BIT(index);
    }
}

સ્થિર bool_t check_fixed_કિંમત(word_t val, word_t નીચલું, word_t ઉપલું)
{
    word_t not_ઉપલું;
    word_t not_નીચલું;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_ઉપલું = ઉપલું & ~(ઉપલું & val);
    જો (not_ઉપલું != 0) {
        printf("Failed to set bits: ");
        print_bits(not_ઉપલું);
        પાછા false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_નીચલું = ~નીચલું & ~(~નીચલું & ~val);
    જો (not_નીચલું != 0) {
        printf("Incorrectly cleared bits: ");
        print_bits(not_નીચલું);
        પાછા false;
    }
    પાછા true;
}

સ્થિર bool_t vtx_check_fixed_કિંમતો(word_t cr0, word_t cr4)
{
    જો (!check_fixed_કિંમત(cr0, cr0_નીચલું, cr0_ઉપલું)) {
        printf(" of CR0\n");
        પાછા false;
    }
    જો (!check_fixed_કિંમત(cr4, cr4_નીચલું, cr4_ઉપલું)) {
        printf(" of CR4\n");
        પાછા false;
    }
    પાછા true;
}

સ્થિર bool_t BOOT_CODE init_vtx_fixed_કિંમતો(bool_t useTrueMsrs)
{
    uint32_t pin_control_mask =
        BIT(0) |    //Extern interrupt exiting
        BIT(3) |    //NMI exiting
        BIT(5);     //virtual NMIs
    uint32_t primary_control_mask =
        BIT(25) |   //Use I/O bitmaps
        BIT(28) |   //Use MSR bitmaps
        BIT(31);    //Activate secondary controls
    uint32_t secondary_control_mask =
        BIT(1);     //Enable EPT
    uint32_t exit_control_mask =
        BIT(2)  |   //Save debug controls
        BIT(18) |   //Save guest IA32_PAT on exit
        BIT(19) |   //Load host IA32_PAT
        BIT(20) |   //Save guest IA32_EFER on exit
        BIT(21);    //Load host IA32_EFER
#ifdef CONFIG_ARCH_X86_64
    exit_control_mask |= BIT(9); //Host address-space size
#endif /* CONFIG_ARCH_X86_64 */
    /* Read out the fixed high and low bits from the MSRs */
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    જો (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } નહીંતર {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    pin_control_ઉપલું = x86_rdmsr_નીચલું(pinbased_ctls);
    pin_control_નીચલું = x86_rdmsr_ઉપલું(pinbased_ctls);
    primary_control_ઉપલું = x86_rdmsr_નીચલું(procbased_ctls);
    primary_control_નીચલું = x86_rdmsr_ઉપલું(procbased_ctls);
    secondary_control_ઉપલું = x86_rdmsr_નીચલું(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_નીચલું = x86_rdmsr_ઉપલું(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_ઉપલું = x86_rdmsr_નીચલું(exit_ctls);
    exit_control_નીચલું = x86_rdmsr_ઉપલું(exit_ctls);
    entry_control_ઉપલું = x86_rdmsr_નીચલું(entry_ctls);
    entry_control_નીચલું = x86_rdmsr_ઉપલું(entry_ctls);

    cr0_ઉપલું = x86_rdmsr_નીચલું(IA32_VMX_CR0_FIXED0_MSR);
    cr0_નીચલું = x86_rdmsr_નીચલું(IA32_VMX_CR0_FIXED1_MSR);
    cr4_ઉપલું = x86_rdmsr_નીચલું(IA32_VMX_CR4_FIXED0_MSR);
    cr4_નીચલું = x86_rdmsr_નીચલું(IA32_VMX_CR4_FIXED1_MSR);

    /* "Unrestricted guest" is a userspace controlled setting that determines whether the VCPU
     * may run in unpaged protected mode or in real-address mode. If the host CPU supports this feature, then
     * CR0.PE and CR0.PG may be 0 while the VCPU is running: */

    /* Intel SDM Combined Volumes
     * Order Number: 325462-090US February 2026
     * Chapter 26.8 "RESTRICTIONS ON VMX OPERATION"
     * > Later processors support a VM-execution control called “unrestricted guest” (see Section 27.6.2).
     * > If this control is 1, CR0.PE and CR0.PG may be 0 in VMX non-root operation (even if the capability
     * > MSR IA32_VMX_CR0_FIXED0 reports otherwise). Such processors allow guest software to run in
     * > unpaged protected mode or in real-address mode. */
    જો (secondary_control_નીચલું & BIT(7)) {
        cr0_ઉપલું &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    જો (!(secondary_control_નીચલું & BIT(5))) {
        vmx_feature_vpid = 0;
        printf("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } નહીંતર {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    જો (!(exit_control_નીચલું & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        printf("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } નહીંતર {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    જો (!(exit_control_નીચલું & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        printf("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } નહીંતર {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_નીચલું) & pin_control_mask;
    જો (missing) {
        printf("vt-x: Unsupported pin control features %lx\n", (long)missing);
        પાછા false;
    }
    missing = (~primary_control_નીચલું) & primary_control_mask;
    જો (missing) {
        printf("vt-x: Unsupported primary control features %lx\n", (long)missing);
        પાછા false;
    }
    missing = (~secondary_control_નીચલું) & secondary_control_mask;
    જો (missing) {
        printf("vt-x: Unsupported secondary control features %lx\n", (long)missing);
        પાછા false;
    }
    missing = (~exit_control_નીચલું) & exit_control_mask;
    જો (missing) {
        printf("vt-x: Unsupported exit control features %lx\n", (long)missing);
        પાછા false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_નીચલું) & BIT(9);
    જો (missing) {
        printf("vt-x: CPU does not support 64-bit guests\n");
        પાછા false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_ઉપલું |= pin_control_mask;
    primary_control_ઉપલું |= primary_control_mask;
    secondary_control_ઉપલું |= secondary_control_mask;
    exit_control_ઉપલું |= exit_control_mask;

    પાછા true;
}

સ્થિર bool_t BOOT_CODE check_vtx_fixed_કિંમતો(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    જો (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } નહીંતર {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    uint32_t local_pin_control_ઉપલું = x86_rdmsr_નીચલું(pinbased_ctls);
    uint32_t local_pin_control_નીચલું = x86_rdmsr_ઉપલું(pinbased_ctls);
    uint32_t local_primary_control_ઉપલું = x86_rdmsr_નીચલું(procbased_ctls);
    uint32_t local_primary_control_નીચલું = x86_rdmsr_ઉપલું(procbased_ctls);
    uint32_t local_secondary_control_ઉપલું = x86_rdmsr_નીચલું(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_નીચલું = x86_rdmsr_ઉપલું(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_ઉપલું = x86_rdmsr_નીચલું(exit_ctls);
    uint32_t local_exit_control_નીચલું = x86_rdmsr_ઉપલું(exit_ctls);
    uint32_t local_entry_control_ઉપલું = x86_rdmsr_નીચલું(entry_ctls);
    uint32_t local_entry_control_નીચલું = x86_rdmsr_ઉપલું(entry_ctls);

    uint32_t local_cr0_ઉપલું = x86_rdmsr_નીચલું(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_નીચલું = x86_rdmsr_નીચલું(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_ઉપલું = x86_rdmsr_નીચલું(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_નીચલું = x86_rdmsr_નીચલું(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    જો (local_secondary_control_નીચલું & BIT(7)) {
        local_cr0_ઉપલું &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    પાછા
        (local_pin_control_ઉપલું & pin_control_ઉપલું) == local_pin_control_ઉપલું &&
        (~local_pin_control_નીચલું & pin_control_ઉપલું) == 0 &&
        (local_primary_control_ઉપલું & primary_control_ઉપલું) == local_primary_control_ઉપલું &&
        (~local_primary_control_નીચલું & primary_control_ઉપલું) == 0 &&
        (local_secondary_control_ઉપલું & secondary_control_ઉપલું) == local_secondary_control_ઉપલું &&
        (~local_secondary_control_નીચલું & secondary_control_ઉપલું) == 0 &&
        (local_exit_control_ઉપલું & exit_control_ઉપલું) == local_exit_control_ઉપલું &&
        (~local_exit_control_નીચલું & exit_control_ઉપલું) == 0 &&
        (local_entry_control_ઉપલું & entry_control_ઉપલું) == local_entry_control_ઉપલું &&
        (~local_entry_control_નીચલું & entry_control_ઉપલું) == 0 &&
        local_cr0_ઉપલું == cr0_ઉપલું &&
        local_cr0_નીચલું == cr0_નીચલું &&
        local_cr4_ઉપલું == cr4_ઉપલું &&
        local_cr4_નીચલું == cr4_નીચલું;
}

સ્થિર અંતઃરેખ uint32_t applyFixedBits(uint32_t original, uint32_t ઉપલું, uint32_t નીચલું)
{
    original |= ઉપલું;
    original &= નીચલું;
    પાછા original;
}

શૂન્ય vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = ખાલી;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    memcpy(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_ઉપલું & cr0_નીચલું;
    vcpu->cr0_shadow = 0;
    vcpu->cr0_mask = 0;
    vcpu->exception_bitmap = 0;
    vcpu->vpid = VPID_INVALID;
#ifdef ENABLE_SMP_SUPPORT
    vcpu->last_cpu = getCurrentCPUIndex();
#endif /* ENABLE_SMP_SUPPORT */

    switchVCPU(vcpu);

    vmwrite(VMX_HOST_PAT, x86_rdmsr(IA32_PAT_MSR));
    vmwrite(VMX_HOST_EFER, x86_rdmsr(IA32_EFER_MSR));
    // By default we will disable performance counters when we come out
    // of a VM. When performance counters are supported this host state
    // needs to be updated on VM entry
    જો (vmx_feature_load_perf_global_ctrl) {
        vmwrite(VMX_HOST_PERF_GLOBAL_CTRL, 0);
    }
    vmwrite(VMX_HOST_CR0, read_cr0());
    vmwrite(VMX_HOST_CR4, read_cr4());
    vmwrite(VMX_HOST_FS_BASE, 0);
    vmwrite(VMX_HOST_GS_BASE, 0);
    vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
    vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
    vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
    vmwrite(VMX_HOST_SYSENTER_CS, (word_t)SEL_CS_0);
    vmwrite(VMX_HOST_SYSENTER_EIP, (word_t)&handle_syscall);
    જો (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((અક્ષર *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.શબ્દો[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_register]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_to_paddr(x64KSKernelPML4), 0).શબ્દો[0]);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_ઉપલું & pin_control_નીચલું);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_ઉપલું & primary_control_નીચલું);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_ઉપલું & secondary_control_નીચલું);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_ઉપલું & exit_control_નીચલું);

    uint64_t entry_control = entry_control_ઉપલું & entry_control_નીચલું;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_ઉપલું & cr4_નીચલું);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_ઉપલું, ~(word_t)0);

    memset(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_SIZE);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((અક્ષર *)vcpu->io + (VCPU_IOBITMAP_SIZE / 2)));
}

સ્થિર શૂન્ય dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    પ્રતિજ્ઞા(tcb->tcbArch.tcbVCPU == vcpu);
    પ્રતિજ્ઞા(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = ખાલી;
    vcpu->vcpuTCB = ખાલી;
}

શૂન્ય vcpu_finalise(vcpu_t *vcpu)
{
    જો (ARCH_ગાંઠ_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
#ifdef ENABLE_SMP_SUPPORT
        જો (vcpu->last_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->last_cpu);
        } નહીંતર
#endif /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    જો (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    જો (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

સ્થિર શૂન્ય associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    જો (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    જો (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

સ્થિર exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *buffer)
{
    પૂર્ણાંક i;
    માટે (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, buffer);
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeVCPUWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    જો (length < 15) {
#નહીંતર
    જો (length < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    પાછા invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
સ્થિર exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *buffer, word_t field, word_t કિંમત)
{
    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);
    જો (ARCH_ગાંઠ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    પસંદગી (field) {
    સ્થિતિ IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = કિંમત;
        તોડો;
    સ્થિતિ IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = કિંમત;
        તોડો;
    સ્થિતિ IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = કિંમત;
        તોડો;
    સ્થિતિ IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = કિંમત;
        તોડો;
    }
    setMR(thread, buffer, 0, કિંમત);
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeVCPUWriteMSR(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t કિંમત;

    જો (length < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    કિંમત = getSyscallArg(1, buffer);
    પસંદગી (field) {
    સ્થિતિ IA32_LSTAR_MSR:
    સ્થિતિ IA32_STAR_MSR:
    સ્થિતિ IA32_CSTAR_MSR:
    સ્થિતિ IA32_FMASK_MSR:
        તોડો;
    મૂળભૂત:
        userError("VCPU WriteMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    પાછા invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer, field, કિંમત);
}

સ્થિર exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *buffer)
{
    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);

    word_t કિંમત = 0;

    પસંદગી (field) {
    સ્થિતિ IA32_LSTAR_MSR:
        કિંમત = vcpu->syscall_registers[VCPU_LSTAR];
        તોડો;
    સ્થિતિ IA32_STAR_MSR:
        કિંમત = vcpu->syscall_registers[VCPU_STAR];
        તોડો;
    સ્થિતિ IA32_CSTAR_MSR:
        કિંમત = vcpu->syscall_registers[VCPU_CSTAR];
        તોડો;
    સ્થિતિ IA32_FMASK_MSR:
        કિંમત = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        તોડો;
    સ્થિતિ IA32_VMX_MISC_MSR:
        કિંમત = x86_rdmsr(field);
        તોડો;
    }

    setMR(thread, buffer, 0, કિંમત);
    setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeVCPUReadMSR(cap_t cap, word_t length, word_t *buffer)
{
    જો (length < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    પસંદગી (field) {
    સ્થિતિ IA32_LSTAR_MSR:
    સ્થિતિ IA32_STAR_MSR:
    સ્થિતિ IA32_CSTAR_MSR:
    સ્થિતિ IA32_FMASK_MSR:
    સ્થિતિ IA32_VMX_MISC_MSR:
        તોડો;
    મૂળભૂત:
        userError("VCPU ReadMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    પાછા invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, buffer);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

સ્થિર exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t નીચલું, uint16_t ઉપલું)
{
    /* remove any existing io ports from this cap */
    clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                            cap_io_port_cap_get_capIOPortFirstPort(cap),
                            cap_io_port_cap_get_capIOPortLastPort(cap));
    /* update the assigned vpid. If the vcpu does not have a valid vpid then
     * this is fine as whilst the cap will not point to the vcpu, the vcpu
     * will have its port mask cleared when it gets assigned a vpid */
    cap = cap_io_port_cap_set_capIOPortVPID(cap, vcpu->vpid);
    slot->cap = cap;
    setIOPortMask(vcpu->io, નીચલું, ઉપલું, false);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeEnableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t નીચલું, ઉપલું;

    જો (length < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (current_extra_caps.excaprefs[0] == ખાલી) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    જો (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    નીચલું = getSyscallArg(0, buffer);
    ઉપલું = getSyscallArg(1, buffer);

    જો (નીચલું < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || ઉપલું > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeEnableIOPort(vcpu, ioSlot, ioCap, નીચલું, ઉપલું);
}

સ્થિર exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t નીચલું, uint16_t ઉપલું)
{
    setIOPortMask(vcpu->io, નીચલું, ઉપલું, true);
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeDisableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    uint16_t નીચલું, ઉપલું;

    જો (length < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    નીચલું = getSyscallArg(0, buffer);
    ઉપલું = getSyscallArg(1, buffer);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    પાછા invokeDisableIOPort(vcpu, નીચલું, ઉપલું);
}

સ્થિર exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *buffer, word_t field, word_t કિંમત)
{
    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);

    જો (ARCH_ગાંઠ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    પસંદગી (field) {
    સ્થિતિ VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = કિંમત;
        તોડો;
    સ્થિતિ VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = કિંમત;
        તોડો;
    સ્થિતિ VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = કિંમત;
        તોડો;
    સ્થિતિ VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = કિંમત;
        તોડો;
    }
    vmwrite(field, કિંમત);

    જો (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned પૂર્ણાંક length = setMR(thread, buffer, 0, કિંમત);
        setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeWriteVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    word_t કિંમત;

    જો (length < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    કિંમત = getSyscallArg(1, buffer);
    પસંદગી (field) {
    સ્થિતિ VMX_GUEST_RIP:
    સ્થિતિ VMX_GUEST_RSP:
    સ્થિતિ VMX_GUEST_ES_SELECTOR:
    સ્થિતિ VMX_GUEST_CS_SELECTOR:
    સ્થિતિ VMX_GUEST_SS_SELECTOR:
    સ્થિતિ VMX_GUEST_DS_SELECTOR:
    સ્થિતિ VMX_GUEST_FS_SELECTOR:
    સ્થિતિ VMX_GUEST_GS_SELECTOR:
    સ્થિતિ VMX_GUEST_LDTR_SELECTOR:
    સ્થિતિ VMX_GUEST_TR_SELECTOR:
    સ્થિતિ VMX_GUEST_DEBUGCTRL:
    સ્થિતિ VMX_GUEST_PAT:
    સ્થિતિ VMX_GUEST_EFER:
    સ્થિતિ VMX_GUEST_PERF_GLOBAL_CTRL:
    સ્થિતિ VMX_GUEST_PDPTE0:
    સ્થિતિ VMX_GUEST_PDPTE1:
    સ્થિતિ VMX_GUEST_PDPTE2:
    સ્થિતિ VMX_GUEST_PDPTE3:
    સ્થિતિ VMX_GUEST_ES_LIMIT:
    સ્થિતિ VMX_GUEST_CS_LIMIT:
    સ્થિતિ VMX_GUEST_SS_LIMIT:
    સ્થિતિ VMX_GUEST_DS_LIMIT:
    સ્થિતિ VMX_GUEST_FS_LIMIT:
    સ્થિતિ VMX_GUEST_GS_LIMIT:
    સ્થિતિ VMX_GUEST_LDTR_LIMIT:
    સ્થિતિ VMX_GUEST_TR_LIMIT:
    સ્થિતિ VMX_GUEST_GDTR_LIMIT:
    સ્થિતિ VMX_GUEST_IDTR_LIMIT:
    સ્થિતિ VMX_GUEST_ES_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_CS_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_SS_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_DS_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_FS_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_GS_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_TR_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_INTERRUPTABILITY:
    સ્થિતિ VMX_GUEST_ACTIVITY:
    સ્થિતિ VMX_GUEST_SMBASE:
    સ્થિતિ VMX_GUEST_SYSENTER_CS:
    સ્થિતિ VMX_GUEST_PREEMPTION_TIMER_કિંમત:
    સ્થિતિ VMX_GUEST_ES_BASE:
    સ્થિતિ VMX_GUEST_CS_BASE:
    સ્થિતિ VMX_GUEST_SS_BASE:
    સ્થિતિ VMX_GUEST_DS_BASE:
    સ્થિતિ VMX_GUEST_FS_BASE:
    સ્થિતિ VMX_GUEST_GS_BASE:
    સ્થિતિ VMX_GUEST_LDTR_BASE:
    સ્થિતિ VMX_GUEST_TR_BASE:
    સ્થિતિ VMX_GUEST_GDTR_BASE:
    સ્થિતિ VMX_GUEST_IDTR_BASE:
    સ્થિતિ VMX_GUEST_DR7:
    સ્થિતિ VMX_GUEST_RFLAGS:
    સ્થિતિ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    સ્થિતિ VMX_GUEST_SYSENTER_ESP:
    સ્થિતિ VMX_GUEST_SYSENTER_EIP:
    સ્થિતિ VMX_CONTROL_CR0_MASK:
    સ્થિતિ VMX_CONTROL_CR4_MASK:
    સ્થિતિ VMX_CONTROL_CR0_READ_SHADOW:
    સ્થિતિ VMX_CONTROL_CR4_READ_SHADOW:
    સ્થિતિ VMX_GUEST_CR3:
    સ્થિતિ VMX_CONTROL_EXCEPTION_BITMAP:
    સ્થિતિ VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    સ્થિતિ VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        તોડો;
    સ્થિતિ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        કિંમત = applyFixedBits(કિંમત, pin_control_ઉપલું, pin_control_નીચલું);
        તોડો;
    સ્થિતિ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        કિંમત = applyFixedBits(કિંમત, primary_control_ઉપલું, primary_control_નીચલું);
        તોડો;
    સ્થિતિ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        કિંમત = applyFixedBits(કિંમત, secondary_control_ઉપલું, secondary_control_નીચલું);
        તોડો;
    સ્થિતિ VMX_CONTROL_ENTRY_CONTROLS:
        કિંમત = applyFixedBits(કિંમત, entry_control_ઉપલું, entry_control_નીચલું);
        તોડો;
    સ્થિતિ VMX_CONTROL_EXIT_CONTROLS:
        કિંમત = applyFixedBits(કિંમત, exit_control_ઉપલું, exit_control_નીચલું);
        તોડો;
    સ્થિતિ VMX_GUEST_CR0:
        કિંમત = applyFixedBits(કિંમત, cr0_ઉપલું, cr0_નીચલું);
        તોડો;
    સ્થિતિ VMX_GUEST_CR4:
        કિંમત = applyFixedBits(કિંમત, cr4_ઉપલું, cr4_નીચલું);
        તોડો;
    મૂળભૂત:
        userError("VCPU WriteVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    પાછા invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, buffer, field, કિંમત);
}

સ્થિર word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    પસંદગી (field) {
    સ્થિતિ VMX_CONTROL_EXCEPTION_BITMAP:
        પાછા vcpu->exception_bitmap;
    સ્થિતિ VMX_GUEST_CR0:
        પાછા vcpu->cr0;
    સ્થિતિ VMX_CONTROL_CR0_MASK:
        પાછા vcpu->cr0_mask;
    સ્થિતિ VMX_CONTROL_CR0_READ_SHADOW:
        પાછા vcpu->cr0_shadow;
    }
    જો (ARCH_ગાંઠ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    પાછા vmread(field);
}

સ્થિર exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *buffer)
{
    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);
    word_t કિંમત = readVMCSField(vcpu, field);
    જો (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned પૂર્ણાંક length = setMR(thread, buffer, 0, કિંમત);
        setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeReadVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    જો (length < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    પસંદગી (field) {
    સ્થિતિ VMX_GUEST_RIP:
    સ્થિતિ VMX_GUEST_RSP:
    સ્થિતિ VMX_GUEST_ES_SELECTOR:
    સ્થિતિ VMX_GUEST_CS_SELECTOR:
    સ્થિતિ VMX_GUEST_SS_SELECTOR:
    સ્થિતિ VMX_GUEST_DS_SELECTOR:
    સ્થિતિ VMX_GUEST_FS_SELECTOR:
    સ્થિતિ VMX_GUEST_GS_SELECTOR:
    સ્થિતિ VMX_GUEST_LDTR_SELECTOR:
    સ્થિતિ VMX_GUEST_TR_SELECTOR:
    સ્થિતિ VMX_GUEST_DEBUGCTRL:
    સ્થિતિ VMX_GUEST_PAT:
    સ્થિતિ VMX_GUEST_EFER:
    સ્થિતિ VMX_GUEST_PERF_GLOBAL_CTRL:
    સ્થિતિ VMX_GUEST_PDPTE0:
    સ્થિતિ VMX_GUEST_PDPTE1:
    સ્થિતિ VMX_GUEST_PDPTE2:
    સ્થિતિ VMX_GUEST_PDPTE3:
    સ્થિતિ VMX_GUEST_ES_LIMIT:
    સ્થિતિ VMX_GUEST_CS_LIMIT:
    સ્થિતિ VMX_GUEST_SS_LIMIT:
    સ્થિતિ VMX_GUEST_DS_LIMIT:
    સ્થિતિ VMX_GUEST_FS_LIMIT:
    સ્થિતિ VMX_GUEST_GS_LIMIT:
    સ્થિતિ VMX_GUEST_LDTR_LIMIT:
    સ્થિતિ VMX_GUEST_TR_LIMIT:
    સ્થિતિ VMX_GUEST_GDTR_LIMIT:
    સ્થિતિ VMX_GUEST_IDTR_LIMIT:
    સ્થિતિ VMX_GUEST_ES_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_CS_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_SS_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_DS_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_FS_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_GS_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_TR_ACCESS_RIGHTS:
    સ્થિતિ VMX_GUEST_INTERRUPTABILITY:
    સ્થિતિ VMX_GUEST_ACTIVITY:
    સ્થિતિ VMX_GUEST_SMBASE:
    સ્થિતિ VMX_GUEST_SYSENTER_CS:
    સ્થિતિ VMX_GUEST_PREEMPTION_TIMER_કિંમત:
    સ્થિતિ VMX_GUEST_ES_BASE:
    સ્થિતિ VMX_GUEST_CS_BASE:
    સ્થિતિ VMX_GUEST_SS_BASE:
    સ્થિતિ VMX_GUEST_DS_BASE:
    સ્થિતિ VMX_GUEST_FS_BASE:
    સ્થિતિ VMX_GUEST_GS_BASE:
    સ્થિતિ VMX_GUEST_LDTR_BASE:
    સ્થિતિ VMX_GUEST_TR_BASE:
    સ્થિતિ VMX_GUEST_GDTR_BASE:
    સ્થિતિ VMX_GUEST_IDTR_BASE:
    સ્થિતિ VMX_GUEST_DR7:
    સ્થિતિ VMX_GUEST_RFLAGS:
    સ્થિતિ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    સ્થિતિ VMX_GUEST_SYSENTER_ESP:
    સ્થિતિ VMX_GUEST_SYSENTER_EIP:
    સ્થિતિ VMX_CONTROL_CR0_MASK:
    સ્થિતિ VMX_CONTROL_CR4_MASK:
    સ્થિતિ VMX_CONTROL_CR0_READ_SHADOW:
    સ્થિતિ VMX_CONTROL_CR4_READ_SHADOW:
    સ્થિતિ VMX_DATA_INSTRUCTION_ERROR:
    સ્થિતિ VMX_DATA_EXIT_INTERRUPT_INFO:
    સ્થિતિ VMX_DATA_EXIT_INTERRUPT_ERROR:
    સ્થિતિ VMX_DATA_IDT_VECTOR_INFO:
    સ્થિતિ VMX_DATA_IDT_VECTOR_ERROR:
    સ્થિતિ VMX_DATA_EXIT_INSTRUCTION_LENGTH:
    સ્થિતિ VMX_DATA_EXIT_INSTRUCTION_INFO:
    સ્થિતિ VMX_DATA_GUEST_PHYSICAL:
    સ્થિતિ VMX_DATA_IO_RCX:
    સ્થિતિ VMX_DATA_IO_RSI:
    સ્થિતિ VMX_DATA_IO_RDI:
    સ્થિતિ VMX_DATA_IO_RIP:
    સ્થિતિ VMX_DATA_GUEST_LINEAR_ADDRESS:
    સ્થિતિ VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    સ્થિતિ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    સ્થિતિ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    સ્થિતિ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    સ્થિતિ VMX_CONTROL_EXCEPTION_BITMAP:
    સ્થિતિ VMX_CONTROL_ENTRY_CONTROLS:
    સ્થિતિ VMX_CONTROL_EXIT_CONTROLS:
    સ્થિતિ VMX_GUEST_CR0:
    સ્થિતિ VMX_GUEST_CR3:
    સ્થિતિ VMX_GUEST_CR4:
        તોડો;
    મૂળભૂત:
        userError("VCPU ReadVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    પાછા invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, buffer);
}

સ્થિર exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeSetTCB(cap_t cap, word_t length, word_t *buffer)
{
    cap_t tcbCap;
    જો (current_extra_caps.excaprefs[0] == ખાલી) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    જો (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    પાછા invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

શૂન્ય vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *buffer;
    જો (ARCH_ગાંઠ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    buffer = lookupIPCBuffer(false, ગાંઠ_STATE(ksCurThread));
    જો (!buffer) {
        userError("No IPC buffer.");
        પાછા;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, buffer));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, buffer), primary_control_ઉપલું,
                                                                   primary_control_નીચલું));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, buffer));
}

શૂન્ય vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *buffer;
    vcpu_t *vcpu;

    buffer = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    પ્રતિજ્ઞા(vcpu);

    જો (ARCH_ગાંઠ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, સંદેશ_Info_Register, 0);
}

exception_t decodeX86VCPUInvocation(
    word_t invLabel,
    word_t length,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    પસંદગી (invLabel) {
    સ્થિતિ X86VCPUSetTCB:
        પાછા decodeSetTCB(cap, length, buffer);
    સ્થિતિ X86VCPUReadVMCS:
        પાછા decodeReadVMCS(cap, length, call, buffer);
    સ્થિતિ X86VCPUWriteVMCS:
        પાછા decodeWriteVMCS(cap, length, call, buffer);
    સ્થિતિ X86VCPUEnableIOPort:
        પાછા decodeEnableIOPort(cap, length, buffer);
    સ્થિતિ X86VCPUDisableIOPort:
        પાછા decodeDisableIOPort(cap, length, buffer);
    સ્થિતિ X86VCPUWriteRegisters:
        પાછા decodeVCPUWriteRegisters(cap, length, buffer);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    સ્થિતિ X86VCPUWriteMSR:
        પાછા decodeVCPUWriteMSR(cap, length, buffer);
    સ્થિતિ X86VCPUReadMSR:
        પાછા decodeVCPUReadMSR(cap, length, buffer);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    મૂળભૂત:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}

સ્થિર bool_t is_vtx_supported(શૂન્ય)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    પાછા !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

સ્થિર અંતઃરેખ શૂન્ય clear_bit(word_t *bitmap, word_t bit)
{
    પૂર્ણાંક index = bit / (કદ(word_t) * 8);
    પૂર્ણાંક offset = bit % (કદ(word_t) * 8);
    bitmap[index] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(શૂન્ય)
{
    જો (!is_vtx_supported()) {
        printf("vt-x: not supported\n");
        પાછા false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.શબ્દો[0] = x86_rdmsr_નીચલું(IA32_VMX_BASIC_MSR);
    vmx_basic.શબ્દો[1] = x86_rdmsr_ઉપલું(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.શબ્દો[0] = x86_rdmsr_નીચલું(IA32_FEATURE_CONTROL_MSR);
    જો (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        જો (feature_control_msr_get_lock(feature_control)) {
            printf("vt-x: feature locked\n");
            પાછા false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ઉપલું(IA32_FEATURE_CONTROL_MSR), feature_control.શબ્દો[0]);
    }
    /* make sure the msr is locked */
    જો (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ઉપલું(IA32_FEATURE_CONTROL_MSR), feature_control.શબ્દો[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    જો (CURRENT_CPU_INDEX() == 0) {
        જો (!init_vtx_fixed_કિંમતો(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            printf("vt-x: lack of required features\n");
            પાછા false;
        }
    }
    જો (!check_vtx_fixed_કિંમતો(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        printf("vt-x: cores have inconsistent features\n");
        પાછા false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    સંરચના vmxon_region *vmxon_kptr = &ARCH_ગાંઠ_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    જો (!vtx_check_fixed_કિંમતો(read_cr0(), read_cr4())) {
        પાછા false;
    }
    જો (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        printf("vt-x: vmxon failure\n");
        પાછા false;
    }
    memset(&msr_bitmap_region, ~0, કદ(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.નીચલું_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.નીચલું_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.નીચલું_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.નીચલું_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.નીચલું_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.નીચલું_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.ઉપલું_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.ઉપલું_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.ઉપલું_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.ઉપલું_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.ઉપલું_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.ઉપલું_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.શબ્દો[0] = x86_rdmsr_નીચલું(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.શબ્દો[1] = x86_rdmsr_ઉપલું(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    જો (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        printf("vt-x: Expected wb attribute for EPT paging structure\n");
        પાછા false;
    }
    જો (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        printf("vt-x: Expected supported for 2m pages\n");
        પાછા false;
    }

    પાછા true;
}

સ્થિર શૂન્ય setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *buffer;
    પૂર્ણાંક i;

    buffer = lookupIPCBuffer(true, ગાંઠ_STATE(ksCurThread));

    setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_DATA_EXIT_INSTRUCTION_LENGTH));
    setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_DATA_GUEST_PHYSICAL));
    setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    માટે (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(ગાંઠ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_EAX + i,
              ગાંઠ_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

સ્થિર શૂન્ય handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(ગાંઠ_STATE(ksCurThread), સંદેશ_Info_Register, SEL4_VMENTER_RESULT_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

સ્થિર અંતઃરેખ શૂન્ય finishVmexitSaving(શૂન્ય)
{
    vcpu_t *vcpu = ARCH_ગાંઠ_STATE(x86KSCurrentVCPU);
    પ્રતિજ્ઞા(vcpu == ગાંઠ_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(શૂન્ય)
{
    uint32_t interrupt;
    /* qualification is host width, reason is defined as being 32 bit */
    word_t qualification;
    uint32_t reason;
    finishVmexitSaving();
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    vcpu_restore_host_msrs();
#endif
    /* the basic exit reason is the bottom 16 bits of the exit reason field */
    reason = vmread(VMX_DATA_EXIT_REASON) & MASK(16);
    જો (reason == EXTERNAL_INTERRUPT) {
        ગાંઠ_LOCK_IRQ;
        જો (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_DATA_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_ગાંઠ_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } નહીંતર {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(ગાંઠ_STATE(ksCurThread));
#endif
        પાછા EXCEPTION_NONE;
    }

    ગાંઠ_LOCK_SYS;

    પસંદગી (reason) {
    સ્થિતિ EXCEPTION_OR_NMI:
    સ્થિતિ MOV_DR:
    સ્થિતિ TASK_SWITCH:
    સ્થિતિ CONTROL_REGISTER:
    સ્થિતિ IO:
    સ્થિતિ MWAIT:
    સ્થિતિ SIPI:
    સ્થિતિ INVLPG:
    સ્થિતિ INVEPT:
    સ્થિતિ INVVPID:
    સ્થિતિ VMCLEAR:
    સ્થિતિ VMPTRLD:
    સ્થિતિ VMPTRST:
    સ્થિતિ VMREAD:
    સ્થિતિ VMWRITE:
    સ્થિતિ VMXON:
    સ્થિતિ EPT_VIOLATION:
    સ્થિતિ GDTR_OR_IDTR:
    સ્થિતિ LDTR_OR_TR:
    સ્થિતિ TPR_BELOW_THRESHOLD:
    સ્થિતિ APIC_ACCESS:
        qualification = vmread(VMX_DATA_EXIT_QUALIFICATION);
        તોડો;
    મૂળભૂત:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    પાછા EXCEPTION_NONE;
}

exception_t handleVmEntryFail(શૂન્ય)
{
    handleVmxFault(-1, -1);

    પાછા EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
શૂન્ય VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    પ્રતિજ્ઞા(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    જો (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {

        word_t badge = notification_ptr_get_ntfn_સંદેશ_Identifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        જો (tcb != ગાંઠ_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

સ્થિર શૂન્ય invvpid_context(uint16_t vpid)
{
    સંરચના {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    asm અસ્થિર("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
}

સ્થિર શૂન્ય setEPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_મૂળ;
    જો (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_મૂળ = kpptr_to_paddr(null_ept_space);
    } નહીંતર {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        જો (find_ret.status != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_મૂળ = kpptr_to_paddr(null_ept_space);
        } નહીંતર {
            ept_મૂળ = pptr_to_paddr(pml4);
        }
    }
    જો (ept_મૂળ != vcpu->last_ept_મૂળ) {
        vcpu->last_ept_મૂળ = ept_મૂળ;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_મૂળ,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.શબ્દો[0]);
        પ્રતિજ્ઞા(vcpu->vpid != VPID_INVALID);
        જો (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

શૂન્ય clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t last)
{
    જો (vpid == VPID_INVALID) {
        પાછા;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    જો (vcpu == ખાલી) {
        પાછા;
    }
    પ્રતિજ્ઞા(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

સ્થિર અંતઃરેખ vpid_t nextVPID(vpid_t vpid)
{
    જો (vpid == VPID_LAST) {
        પાછા VPID_FIRST;
    } નહીંતર {
        પાછા vpid + 1;
    }
}

સ્થિર શૂન્ય invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, કદ(vcpu->io));
    /* invalidate the VPID context */
    જો (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
સ્થિર શૂન્ય releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = ખાલી;
}

સ્થિર vpid_t findFreeVPID(શૂન્ય)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    કરો {
        જો (x86KSVPIDTable[vpid] == ખાલી) {
            પાછા vpid;
        }
        vpid = nextVPID(vpid);
    } જ્યારે (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = nextVPID(x86KSNextVPID);
    પાછા vpid;
}

સ્થિર શૂન્ય storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    પ્રતિજ્ઞા(x86KSVPIDTable[vpid] == ખાલી);
    પ્રતિજ્ઞા(vcpu->vpid == VPID_INVALID);
    x86KSVPIDTable[vpid] = vcpu;
    vcpu->vpid = vpid;
}

/* Normally the seL4_TCBFlag_fpuDisabled TCB flag is used to decide whether to
 * enable or disable the FPU. However, with x86 virtualisation there is only
 * one TCB. Use the task flag for the host and always enable FPU for the guest.
 *
 * When either the host or the guest's FPU is loaded, ksCurFPUOwner will
 * point to our TCB. saveFpuState and loadFpuState check fpu_active and
 * do the right thing depending on the current FPU user.
 */
શૂન્ય vcpu_fpu_to_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    જો (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        જો (!vcpu->fpu_active) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_active = true;
            loadFpuState(tcb);
        }
    } નહીંતર {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_active = true;
        switchLocalFpuOwner(tcb);
    }
}

શૂન્ય vcpu_fpu_to_host(tcb_t *tcb, vcpu_t *vcpu)
{
    જો (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } નહીંતર જો (nativeThreadUsingFPU(tcb) && vcpu->fpu_active) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_active = false;
        loadFpuState(tcb);
    } નહીંતર જો (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

શૂન્ય restoreVMCS(શૂન્ય)
{
    tcb_t *cur_thread = ગાંઠ_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    જો (ARCH_ગાંઠ_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ifndef CONFIG_KERNEL_SKIM_WINDOW
    જો (getCurrentCR3().શબ્દો[0] != expected_vmcs->last_host_cr3) {
        expected_vmcs->last_host_cr3 = getCurrentCR3().શબ્દો[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().શબ્દો[0]);
    }
#endif
    જો (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        જો (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    setEPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

શૂન્ય invept(ept_pml4e_t *ept_pml4)
{
    જો (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        સંરચના {
            uint64_t parts[2];
        } address;
        word_t type;
        જો (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } નહીંતર જો (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } નહીંતર {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            પાછા;
        }

        address.parts[0] = pptr_to_paddr((શૂન્ય *)ept_pml4);
        address.parts[1] = 0;
        asm અસ્થિર(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
