/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>

#ifdef CONFIG_VTX

#அந்தர்பாவ <types.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <object/objecttype.h>
#அந்தர்பாவ <arch/machine/cpu_registers.h>
#அந்தர்பாவ <arch/model/statedata.h>
#அந்தர்பாவ <arch/object/vcpu.h>
#அந்தர்பாவ <arch/object/ioport.h>
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <sel4/arch/vmenter.h>

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

மாறிலி vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

வகைவரை கட்டமைப்பு msr_bitmap {
    word_t bitmap[0x2000 / அளவு(word_t) / 8];
} msr_bitmap_t;

வகைவரை கட்டமைப்பு msr_bitmaps {
    msr_bitmap_t கீழ்_msr_read;
    msr_bitmap_t உயர்வு_msr_read;
    msr_bitmap_t கீழ்_msr_write;
    msr_bitmap_t உயர்வு_msr_write;
} msr_bitmaps_t;

நிலையான msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

நிலையான எழுத்து null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
நிலையான uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
நிலையான vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
நிலையான uint32_t pin_control_உயர்வு;
நிலையான uint32_t pin_control_கீழ்;
நிலையான uint32_t primary_control_உயர்வு;
நிலையான uint32_t primary_control_கீழ்;
நிலையான uint32_t secondary_control_உயர்வு;
நிலையான uint32_t secondary_control_கீழ்;
நிலையான uint32_t entry_control_உயர்வு;
நிலையான uint32_t entry_control_கீழ்;
நிலையான uint32_t exit_control_உயர்வு;
நிலையான uint32_t exit_control_கீழ்;
நிலையான uint32_t cr0_உயர்வு;
நிலையான uint32_t cr0_கீழ்;
நிலையான uint32_t cr4_உயர்வு;
நிலையான uint32_t cr4_கீழ்;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
நிலையான bool_t vmx_feature_vpid;
நிலையான bool_t vmx_feature_load_perf_global_ctrl;
நிலையான bool_t vmx_feature_ack_on_exit;

நிலையான vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
நிலையான vpid_t x86KSNextVPID = VPID_FIRST;

நிலையான வெறுமை releaseVPID(vpid_t vpid);

நிலையான உள்வரி bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm நிலையற்ற(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    திரும்பு !!error;
}

நிலையான வெறுமை vmclear(வெறுமை *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((வெறுமை *)vmcs_ptr);
    asm நிலையற்ற(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

வெறுமை clearCurrentVCPU(வெறுமை)
{
    vcpu_t *vcpu = ARCH_முனை_STATE(x86KSCurrentVCPU);
    எனில் (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_முனை_STATE(x86KSCurrentVCPU) = வெற்று;
        எனில் (vcpu->fpu_active && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_active = false;
        }
    }
}

நிலையான வெறுமை vmptrld(வெறுமை *vmcs_ptr)
{
    uint64_t physical_address;
    uint8_t error;
    physical_address = pptr_to_paddr(vmcs_ptr);
    asm நிலையற்ற(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_address)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    ப்ரதிஜ்ஞா(!error);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
வெறுமை vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_STAR_MSR, vcpu->syscall_registers[VCPU_STAR]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_MASK]);
}

வெறுமை vcpu_restore_host_msrs(வெறுமை)
{
    init_syscall_msrs();
}
#endif

நிலையான வெறுமை switchVCPU(vcpu_t *vcpu)
{
#ifdef ENABLE_SMP_SUPPORT
    எனில் (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_முனை_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    எனில் (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((எழுத்து *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.சொற்கள்[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_முனை_STATE(x86KSCurrentVCPU) = vcpu;
}

நிலையான வெறுமை print_bits(word_t bits)
{
    bool_t first = true;
    போது (bits) {
        முழு index = seL4_WordBits - 1 - clzl(bits);
        எனில் (first) {
            printf("%d", index);
            first = false;
        } இல்லையெனில் {
            printf(",%d", index);
        }
        bits &= ~BIT(index);
    }
}

நிலையான bool_t check_fixed_மதிப்பு(word_t val, word_t கீழ், word_t உயர்வு)
{
    word_t not_உயர்வு;
    word_t not_கீழ்;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_உயர்வு = உயர்வு & ~(உயர்வு & val);
    எனில் (not_உயர்வு != 0) {
        printf("Failed to set bits: ");
        print_bits(not_உயர்வு);
        திரும்பு false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_கீழ் = ~கீழ் & ~(~கீழ் & ~val);
    எனில் (not_கீழ் != 0) {
        printf("Incorrectly cleared bits: ");
        print_bits(not_கீழ்);
        திரும்பு false;
    }
    திரும்பு true;
}

நிலையான bool_t vtx_check_fixed_மதிப்புகள்(word_t cr0, word_t cr4)
{
    எனில் (!check_fixed_மதிப்பு(cr0, cr0_கீழ், cr0_உயர்வு)) {
        printf(" of CR0\n");
        திரும்பு false;
    }
    எனில் (!check_fixed_மதிப்பு(cr4, cr4_கீழ், cr4_உயர்வு)) {
        printf(" of CR4\n");
        திரும்பு false;
    }
    திரும்பு true;
}

நிலையான bool_t BOOT_CODE init_vtx_fixed_மதிப்புகள்(bool_t useTrueMsrs)
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
    எனில் (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } இல்லையெனில் {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    pin_control_உயர்வு = x86_rdmsr_கீழ்(pinbased_ctls);
    pin_control_கீழ் = x86_rdmsr_உயர்வு(pinbased_ctls);
    primary_control_உயர்வு = x86_rdmsr_கீழ்(procbased_ctls);
    primary_control_கீழ் = x86_rdmsr_உயர்வு(procbased_ctls);
    secondary_control_உயர்வு = x86_rdmsr_கீழ்(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_கீழ் = x86_rdmsr_உயர்வு(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_உயர்வு = x86_rdmsr_கீழ்(exit_ctls);
    exit_control_கீழ் = x86_rdmsr_உயர்வு(exit_ctls);
    entry_control_உயர்வு = x86_rdmsr_கீழ்(entry_ctls);
    entry_control_கீழ் = x86_rdmsr_உயர்வு(entry_ctls);

    cr0_உயர்வு = x86_rdmsr_கீழ்(IA32_VMX_CR0_FIXED0_MSR);
    cr0_கீழ் = x86_rdmsr_கீழ்(IA32_VMX_CR0_FIXED1_MSR);
    cr4_உயர்வு = x86_rdmsr_கீழ்(IA32_VMX_CR4_FIXED0_MSR);
    cr4_கீழ் = x86_rdmsr_கீழ்(IA32_VMX_CR4_FIXED1_MSR);

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
    எனில் (secondary_control_கீழ் & BIT(7)) {
        cr0_உயர்வு &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    எனில் (!(secondary_control_கீழ் & BIT(5))) {
        vmx_feature_vpid = 0;
        printf("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } இல்லையெனில் {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    எனில் (!(exit_control_கீழ் & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        printf("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } இல்லையெனில் {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    எனில் (!(exit_control_கீழ் & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        printf("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } இல்லையெனில் {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_கீழ்) & pin_control_mask;
    எனில் (missing) {
        printf("vt-x: Unsupported pin control features %lx\n", (long)missing);
        திரும்பு false;
    }
    missing = (~primary_control_கீழ்) & primary_control_mask;
    எனில் (missing) {
        printf("vt-x: Unsupported primary control features %lx\n", (long)missing);
        திரும்பு false;
    }
    missing = (~secondary_control_கீழ்) & secondary_control_mask;
    எனில் (missing) {
        printf("vt-x: Unsupported secondary control features %lx\n", (long)missing);
        திரும்பு false;
    }
    missing = (~exit_control_கீழ்) & exit_control_mask;
    எனில் (missing) {
        printf("vt-x: Unsupported exit control features %lx\n", (long)missing);
        திரும்பு false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_கீழ்) & BIT(9);
    எனில் (missing) {
        printf("vt-x: CPU does not support 64-bit guests\n");
        திரும்பு false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_உயர்வு |= pin_control_mask;
    primary_control_உயர்வு |= primary_control_mask;
    secondary_control_உயர்வு |= secondary_control_mask;
    exit_control_உயர்வு |= exit_control_mask;

    திரும்பு true;
}

நிலையான bool_t BOOT_CODE check_vtx_fixed_மதிப்புகள்(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    எனில் (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } இல்லையெனில் {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    uint32_t local_pin_control_உயர்வு = x86_rdmsr_கீழ்(pinbased_ctls);
    uint32_t local_pin_control_கீழ் = x86_rdmsr_உயர்வு(pinbased_ctls);
    uint32_t local_primary_control_உயர்வு = x86_rdmsr_கீழ்(procbased_ctls);
    uint32_t local_primary_control_கீழ் = x86_rdmsr_உயர்வு(procbased_ctls);
    uint32_t local_secondary_control_உயர்வு = x86_rdmsr_கீழ்(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_கீழ் = x86_rdmsr_உயர்வு(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_உயர்வு = x86_rdmsr_கீழ்(exit_ctls);
    uint32_t local_exit_control_கீழ் = x86_rdmsr_உயர்வு(exit_ctls);
    uint32_t local_entry_control_உயர்வு = x86_rdmsr_கீழ்(entry_ctls);
    uint32_t local_entry_control_கீழ் = x86_rdmsr_உயர்வு(entry_ctls);

    uint32_t local_cr0_உயர்வு = x86_rdmsr_கீழ்(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_கீழ் = x86_rdmsr_கீழ்(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_உயர்வு = x86_rdmsr_கீழ்(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_கீழ் = x86_rdmsr_கீழ்(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    எனில் (local_secondary_control_கீழ் & BIT(7)) {
        local_cr0_உயர்வு &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    திரும்பு
        (local_pin_control_உயர்வு & pin_control_உயர்வு) == local_pin_control_உயர்வு &&
        (~local_pin_control_கீழ் & pin_control_உயர்வு) == 0 &&
        (local_primary_control_உயர்வு & primary_control_உயர்வு) == local_primary_control_உயர்வு &&
        (~local_primary_control_கீழ் & primary_control_உயர்வு) == 0 &&
        (local_secondary_control_உயர்வு & secondary_control_உயர்வு) == local_secondary_control_உயர்வு &&
        (~local_secondary_control_கீழ் & secondary_control_உயர்வு) == 0 &&
        (local_exit_control_உயர்வு & exit_control_உயர்வு) == local_exit_control_உயர்வு &&
        (~local_exit_control_கீழ் & exit_control_உயர்வு) == 0 &&
        (local_entry_control_உயர்வு & entry_control_உயர்வு) == local_entry_control_உயர்வு &&
        (~local_entry_control_கீழ் & entry_control_உயர்வு) == 0 &&
        local_cr0_உயர்வு == cr0_உயர்வு &&
        local_cr0_கீழ் == cr0_கீழ் &&
        local_cr4_உயர்வு == cr4_உயர்வு &&
        local_cr4_கீழ் == cr4_கீழ்;
}

நிலையான உள்வரி uint32_t applyFixedBits(uint32_t original, uint32_t உயர்வு, uint32_t கீழ்)
{
    original |= உயர்வு;
    original &= கீழ்;
    திரும்பு original;
}

வெறுமை vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = வெற்று;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    memcpy(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_உயர்வு & cr0_கீழ்;
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
    எனில் (vmx_feature_load_perf_global_ctrl) {
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
    எனில் (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((எழுத்து *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.சொற்கள்[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_register]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_to_paddr(x64KSKernelPML4), 0).சொற்கள்[0]);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_உயர்வு & pin_control_கீழ்);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_உயர்வு & primary_control_கீழ்);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_உயர்வு & secondary_control_கீழ்);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_உயர்வு & exit_control_கீழ்);

    uint64_t entry_control = entry_control_உயர்வு & entry_control_கீழ்;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_உயர்வு & cr4_கீழ்);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_உயர்வு, ~(word_t)0);

    memset(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_SIZE);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((எழுத்து *)vcpu->io + (VCPU_IOBITMAP_SIZE / 2)));
}

நிலையான வெறுமை dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ப்ரதிஜ்ஞா(tcb->tcbArch.tcbVCPU == vcpu);
    ப்ரதிஜ்ஞா(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = வெற்று;
    vcpu->vcpuTCB = வெற்று;
}

வெறுமை vcpu_finalise(vcpu_t *vcpu)
{
    எனில் (ARCH_முனை_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
#ifdef ENABLE_SMP_SUPPORT
        எனில் (vcpu->last_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->last_cpu);
        } இல்லையெனில்
#endif /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    எனில் (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    எனில் (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

நிலையான வெறுமை associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    எனில் (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    எனில் (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

நிலையான exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *buffer)
{
    முழு i;
    க்கு (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, buffer);
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeVCPUWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    எனில் (length < 15) {
#இல்லையெனில்
    எனில் (length < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    திரும்பு invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
நிலையான exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *buffer, word_t field, word_t மதிப்பு)
{
    tcb_t *thread;
    thread = முனை_STATE(ksCurThread);
    எனில் (ARCH_முனை_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    தேர்வு (field) {
    நிலை IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = மதிப்பு;
        நிறுத்து;
    நிலை IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = மதிப்பு;
        நிறுத்து;
    நிலை IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = மதிப்பு;
        நிறுத்து;
    நிலை IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = மதிப்பு;
        நிறுத்து;
    }
    setMR(thread, buffer, 0, மதிப்பு);
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeVCPUWriteMSR(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t மதிப்பு;

    எனில் (length < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    மதிப்பு = getSyscallArg(1, buffer);
    தேர்வு (field) {
    நிலை IA32_LSTAR_MSR:
    நிலை IA32_STAR_MSR:
    நிலை IA32_CSTAR_MSR:
    நிலை IA32_FMASK_MSR:
        நிறுத்து;
    இயல்பு:
        userError("VCPU WriteMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    திரும்பு invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer, field, மதிப்பு);
}

நிலையான exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *buffer)
{
    tcb_t *thread;
    thread = முனை_STATE(ksCurThread);

    word_t மதிப்பு = 0;

    தேர்வு (field) {
    நிலை IA32_LSTAR_MSR:
        மதிப்பு = vcpu->syscall_registers[VCPU_LSTAR];
        நிறுத்து;
    நிலை IA32_STAR_MSR:
        மதிப்பு = vcpu->syscall_registers[VCPU_STAR];
        நிறுத்து;
    நிலை IA32_CSTAR_MSR:
        மதிப்பு = vcpu->syscall_registers[VCPU_CSTAR];
        நிறுத்து;
    நிலை IA32_FMASK_MSR:
        மதிப்பு = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        நிறுத்து;
    நிலை IA32_VMX_MISC_MSR:
        மதிப்பு = x86_rdmsr(field);
        நிறுத்து;
    }

    setMR(thread, buffer, 0, மதிப்பு);
    setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeVCPUReadMSR(cap_t cap, word_t length, word_t *buffer)
{
    எனில் (length < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    தேர்வு (field) {
    நிலை IA32_LSTAR_MSR:
    நிலை IA32_STAR_MSR:
    நிலை IA32_CSTAR_MSR:
    நிலை IA32_FMASK_MSR:
    நிலை IA32_VMX_MISC_MSR:
        நிறுத்து;
    இயல்பு:
        userError("VCPU ReadMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    திரும்பு invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, buffer);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

நிலையான exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t கீழ், uint16_t உயர்வு)
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
    setIOPortMask(vcpu->io, கீழ், உயர்வு, false);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeEnableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t கீழ், உயர்வு;

    எனில் (length < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (current_extra_caps.excaprefs[0] == வெற்று) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    எனில் (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    கீழ் = getSyscallArg(0, buffer);
    உயர்வு = getSyscallArg(1, buffer);

    எனில் (கீழ் < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || உயர்வு > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 0;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeEnableIOPort(vcpu, ioSlot, ioCap, கீழ், உயர்வு);
}

நிலையான exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t கீழ், uint16_t உயர்வு)
{
    setIOPortMask(vcpu->io, கீழ், உயர்வு, true);
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeDisableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    uint16_t கீழ், உயர்வு;

    எனில் (length < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    கீழ் = getSyscallArg(0, buffer);
    உயர்வு = getSyscallArg(1, buffer);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    திரும்பு invokeDisableIOPort(vcpu, கீழ், உயர்வு);
}

நிலையான exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *buffer, word_t field, word_t மதிப்பு)
{
    tcb_t *thread;
    thread = முனை_STATE(ksCurThread);

    எனில் (ARCH_முனை_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    தேர்வு (field) {
    நிலை VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = மதிப்பு;
        நிறுத்து;
    நிலை VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = மதிப்பு;
        நிறுத்து;
    நிலை VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = மதிப்பு;
        நிறுத்து;
    நிலை VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = மதிப்பு;
        நிறுத்து;
    }
    vmwrite(field, மதிப்பு);

    எனில் (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned முழு length = setMR(thread, buffer, 0, மதிப்பு);
        setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeWriteVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    word_t மதிப்பு;

    எனில் (length < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    மதிப்பு = getSyscallArg(1, buffer);
    தேர்வு (field) {
    நிலை VMX_GUEST_RIP:
    நிலை VMX_GUEST_RSP:
    நிலை VMX_GUEST_ES_SELECTOR:
    நிலை VMX_GUEST_CS_SELECTOR:
    நிலை VMX_GUEST_SS_SELECTOR:
    நிலை VMX_GUEST_DS_SELECTOR:
    நிலை VMX_GUEST_FS_SELECTOR:
    நிலை VMX_GUEST_GS_SELECTOR:
    நிலை VMX_GUEST_LDTR_SELECTOR:
    நிலை VMX_GUEST_TR_SELECTOR:
    நிலை VMX_GUEST_DEBUGCTRL:
    நிலை VMX_GUEST_PAT:
    நிலை VMX_GUEST_EFER:
    நிலை VMX_GUEST_PERF_GLOBAL_CTRL:
    நிலை VMX_GUEST_PDPTE0:
    நிலை VMX_GUEST_PDPTE1:
    நிலை VMX_GUEST_PDPTE2:
    நிலை VMX_GUEST_PDPTE3:
    நிலை VMX_GUEST_ES_LIMIT:
    நிலை VMX_GUEST_CS_LIMIT:
    நிலை VMX_GUEST_SS_LIMIT:
    நிலை VMX_GUEST_DS_LIMIT:
    நிலை VMX_GUEST_FS_LIMIT:
    நிலை VMX_GUEST_GS_LIMIT:
    நிலை VMX_GUEST_LDTR_LIMIT:
    நிலை VMX_GUEST_TR_LIMIT:
    நிலை VMX_GUEST_GDTR_LIMIT:
    நிலை VMX_GUEST_IDTR_LIMIT:
    நிலை VMX_GUEST_ES_ACCESS_RIGHTS:
    நிலை VMX_GUEST_CS_ACCESS_RIGHTS:
    நிலை VMX_GUEST_SS_ACCESS_RIGHTS:
    நிலை VMX_GUEST_DS_ACCESS_RIGHTS:
    நிலை VMX_GUEST_FS_ACCESS_RIGHTS:
    நிலை VMX_GUEST_GS_ACCESS_RIGHTS:
    நிலை VMX_GUEST_LDTR_ACCESS_RIGHTS:
    நிலை VMX_GUEST_TR_ACCESS_RIGHTS:
    நிலை VMX_GUEST_INTERRUPTABILITY:
    நிலை VMX_GUEST_ACTIVITY:
    நிலை VMX_GUEST_SMBASE:
    நிலை VMX_GUEST_SYSENTER_CS:
    நிலை VMX_GUEST_PREEMPTION_TIMER_மதிப்பு:
    நிலை VMX_GUEST_ES_BASE:
    நிலை VMX_GUEST_CS_BASE:
    நிலை VMX_GUEST_SS_BASE:
    நிலை VMX_GUEST_DS_BASE:
    நிலை VMX_GUEST_FS_BASE:
    நிலை VMX_GUEST_GS_BASE:
    நிலை VMX_GUEST_LDTR_BASE:
    நிலை VMX_GUEST_TR_BASE:
    நிலை VMX_GUEST_GDTR_BASE:
    நிலை VMX_GUEST_IDTR_BASE:
    நிலை VMX_GUEST_DR7:
    நிலை VMX_GUEST_RFLAGS:
    நிலை VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    நிலை VMX_GUEST_SYSENTER_ESP:
    நிலை VMX_GUEST_SYSENTER_EIP:
    நிலை VMX_CONTROL_CR0_MASK:
    நிலை VMX_CONTROL_CR4_MASK:
    நிலை VMX_CONTROL_CR0_READ_SHADOW:
    நிலை VMX_CONTROL_CR4_READ_SHADOW:
    நிலை VMX_GUEST_CR3:
    நிலை VMX_CONTROL_EXCEPTION_BITMAP:
    நிலை VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    நிலை VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        நிறுத்து;
    நிலை VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        மதிப்பு = applyFixedBits(மதிப்பு, pin_control_உயர்வு, pin_control_கீழ்);
        நிறுத்து;
    நிலை VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        மதிப்பு = applyFixedBits(மதிப்பு, primary_control_உயர்வு, primary_control_கீழ்);
        நிறுத்து;
    நிலை VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        மதிப்பு = applyFixedBits(மதிப்பு, secondary_control_உயர்வு, secondary_control_கீழ்);
        நிறுத்து;
    நிலை VMX_CONTROL_ENTRY_CONTROLS:
        மதிப்பு = applyFixedBits(மதிப்பு, entry_control_உயர்வு, entry_control_கீழ்);
        நிறுத்து;
    நிலை VMX_CONTROL_EXIT_CONTROLS:
        மதிப்பு = applyFixedBits(மதிப்பு, exit_control_உயர்வு, exit_control_கீழ்);
        நிறுத்து;
    நிலை VMX_GUEST_CR0:
        மதிப்பு = applyFixedBits(மதிப்பு, cr0_உயர்வு, cr0_கீழ்);
        நிறுத்து;
    நிலை VMX_GUEST_CR4:
        மதிப்பு = applyFixedBits(மதிப்பு, cr4_உயர்வு, cr4_கீழ்);
        நிறுத்து;
    இயல்பு:
        userError("VCPU WriteVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    திரும்பு invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, buffer, field, மதிப்பு);
}

நிலையான word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    தேர்வு (field) {
    நிலை VMX_CONTROL_EXCEPTION_BITMAP:
        திரும்பு vcpu->exception_bitmap;
    நிலை VMX_GUEST_CR0:
        திரும்பு vcpu->cr0;
    நிலை VMX_CONTROL_CR0_MASK:
        திரும்பு vcpu->cr0_mask;
    நிலை VMX_CONTROL_CR0_READ_SHADOW:
        திரும்பு vcpu->cr0_shadow;
    }
    எனில் (ARCH_முனை_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    திரும்பு vmread(field);
}

நிலையான exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *buffer)
{
    tcb_t *thread;
    thread = முனை_STATE(ksCurThread);
    word_t மதிப்பு = readVMCSField(vcpu, field);
    எனில் (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned முழு length = setMR(thread, buffer, 0, மதிப்பு);
        setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeReadVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    எனில் (length < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    தேர்வு (field) {
    நிலை VMX_GUEST_RIP:
    நிலை VMX_GUEST_RSP:
    நிலை VMX_GUEST_ES_SELECTOR:
    நிலை VMX_GUEST_CS_SELECTOR:
    நிலை VMX_GUEST_SS_SELECTOR:
    நிலை VMX_GUEST_DS_SELECTOR:
    நிலை VMX_GUEST_FS_SELECTOR:
    நிலை VMX_GUEST_GS_SELECTOR:
    நிலை VMX_GUEST_LDTR_SELECTOR:
    நிலை VMX_GUEST_TR_SELECTOR:
    நிலை VMX_GUEST_DEBUGCTRL:
    நிலை VMX_GUEST_PAT:
    நிலை VMX_GUEST_EFER:
    நிலை VMX_GUEST_PERF_GLOBAL_CTRL:
    நிலை VMX_GUEST_PDPTE0:
    நிலை VMX_GUEST_PDPTE1:
    நிலை VMX_GUEST_PDPTE2:
    நிலை VMX_GUEST_PDPTE3:
    நிலை VMX_GUEST_ES_LIMIT:
    நிலை VMX_GUEST_CS_LIMIT:
    நிலை VMX_GUEST_SS_LIMIT:
    நிலை VMX_GUEST_DS_LIMIT:
    நிலை VMX_GUEST_FS_LIMIT:
    நிலை VMX_GUEST_GS_LIMIT:
    நிலை VMX_GUEST_LDTR_LIMIT:
    நிலை VMX_GUEST_TR_LIMIT:
    நிலை VMX_GUEST_GDTR_LIMIT:
    நிலை VMX_GUEST_IDTR_LIMIT:
    நிலை VMX_GUEST_ES_ACCESS_RIGHTS:
    நிலை VMX_GUEST_CS_ACCESS_RIGHTS:
    நிலை VMX_GUEST_SS_ACCESS_RIGHTS:
    நிலை VMX_GUEST_DS_ACCESS_RIGHTS:
    நிலை VMX_GUEST_FS_ACCESS_RIGHTS:
    நிலை VMX_GUEST_GS_ACCESS_RIGHTS:
    நிலை VMX_GUEST_LDTR_ACCESS_RIGHTS:
    நிலை VMX_GUEST_TR_ACCESS_RIGHTS:
    நிலை VMX_GUEST_INTERRUPTABILITY:
    நிலை VMX_GUEST_ACTIVITY:
    நிலை VMX_GUEST_SMBASE:
    நிலை VMX_GUEST_SYSENTER_CS:
    நிலை VMX_GUEST_PREEMPTION_TIMER_மதிப்பு:
    நிலை VMX_GUEST_ES_BASE:
    நிலை VMX_GUEST_CS_BASE:
    நிலை VMX_GUEST_SS_BASE:
    நிலை VMX_GUEST_DS_BASE:
    நிலை VMX_GUEST_FS_BASE:
    நிலை VMX_GUEST_GS_BASE:
    நிலை VMX_GUEST_LDTR_BASE:
    நிலை VMX_GUEST_TR_BASE:
    நிலை VMX_GUEST_GDTR_BASE:
    நிலை VMX_GUEST_IDTR_BASE:
    நிலை VMX_GUEST_DR7:
    நிலை VMX_GUEST_RFLAGS:
    நிலை VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    நிலை VMX_GUEST_SYSENTER_ESP:
    நிலை VMX_GUEST_SYSENTER_EIP:
    நிலை VMX_CONTROL_CR0_MASK:
    நிலை VMX_CONTROL_CR4_MASK:
    நிலை VMX_CONTROL_CR0_READ_SHADOW:
    நிலை VMX_CONTROL_CR4_READ_SHADOW:
    நிலை VMX_DATA_INSTRUCTION_ERROR:
    நிலை VMX_DATA_EXIT_INTERRUPT_INFO:
    நிலை VMX_DATA_EXIT_INTERRUPT_ERROR:
    நிலை VMX_DATA_IDT_VECTOR_INFO:
    நிலை VMX_DATA_IDT_VECTOR_ERROR:
    நிலை VMX_DATA_EXIT_INSTRUCTION_LENGTH:
    நிலை VMX_DATA_EXIT_INSTRUCTION_INFO:
    நிலை VMX_DATA_GUEST_PHYSICAL:
    நிலை VMX_DATA_IO_RCX:
    நிலை VMX_DATA_IO_RSI:
    நிலை VMX_DATA_IO_RDI:
    நிலை VMX_DATA_IO_RIP:
    நிலை VMX_DATA_GUEST_LINEAR_ADDRESS:
    நிலை VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    நிலை VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    நிலை VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    நிலை VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    நிலை VMX_CONTROL_EXCEPTION_BITMAP:
    நிலை VMX_CONTROL_ENTRY_CONTROLS:
    நிலை VMX_CONTROL_EXIT_CONTROLS:
    நிலை VMX_GUEST_CR0:
    நிலை VMX_GUEST_CR3:
    நிலை VMX_GUEST_CR4:
        நிறுத்து;
    இயல்பு:
        userError("VCPU ReadVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    திரும்பு invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, buffer);
}

நிலையான exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeSetTCB(cap_t cap, word_t length, word_t *buffer)
{
    cap_t tcbCap;
    எனில் (current_extra_caps.excaprefs[0] == வெற்று) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    எனில் (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    திரும்பு invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

வெறுமை vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *buffer;
    எனில் (ARCH_முனை_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    buffer = lookupIPCBuffer(false, முனை_STATE(ksCurThread));
    எனில் (!buffer) {
        userError("No IPC buffer.");
        திரும்பு;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, buffer));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, buffer), primary_control_உயர்வு,
                                                                   primary_control_கீழ்));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, buffer));
}

வெறுமை vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *buffer;
    vcpu_t *vcpu;

    buffer = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    ப்ரதிஜ்ஞா(vcpu);

    எனில் (ARCH_முனை_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, செய்தி_Info_Register, 0);
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
    தேர்வு (invLabel) {
    நிலை X86VCPUSetTCB:
        திரும்பு decodeSetTCB(cap, length, buffer);
    நிலை X86VCPUReadVMCS:
        திரும்பு decodeReadVMCS(cap, length, call, buffer);
    நிலை X86VCPUWriteVMCS:
        திரும்பு decodeWriteVMCS(cap, length, call, buffer);
    நிலை X86VCPUEnableIOPort:
        திரும்பு decodeEnableIOPort(cap, length, buffer);
    நிலை X86VCPUDisableIOPort:
        திரும்பு decodeDisableIOPort(cap, length, buffer);
    நிலை X86VCPUWriteRegisters:
        திரும்பு decodeVCPUWriteRegisters(cap, length, buffer);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    நிலை X86VCPUWriteMSR:
        திரும்பு decodeVCPUWriteMSR(cap, length, buffer);
    நிலை X86VCPUReadMSR:
        திரும்பு decodeVCPUReadMSR(cap, length, buffer);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    இயல்பு:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
}

நிலையான bool_t is_vtx_supported(வெறுமை)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    திரும்பு !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

நிலையான உள்வரி வெறுமை clear_bit(word_t *bitmap, word_t bit)
{
    முழு index = bit / (அளவு(word_t) * 8);
    முழு offset = bit % (அளவு(word_t) * 8);
    bitmap[index] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(வெறுமை)
{
    எனில் (!is_vtx_supported()) {
        printf("vt-x: not supported\n");
        திரும்பு false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.சொற்கள்[0] = x86_rdmsr_கீழ்(IA32_VMX_BASIC_MSR);
    vmx_basic.சொற்கள்[1] = x86_rdmsr_உயர்வு(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.சொற்கள்[0] = x86_rdmsr_கீழ்(IA32_FEATURE_CONTROL_MSR);
    எனில் (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        எனில் (feature_control_msr_get_lock(feature_control)) {
            printf("vt-x: feature locked\n");
            திரும்பு false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_உயர்வு(IA32_FEATURE_CONTROL_MSR), feature_control.சொற்கள்[0]);
    }
    /* make sure the msr is locked */
    எனில் (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_உயர்வு(IA32_FEATURE_CONTROL_MSR), feature_control.சொற்கள்[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    எனில் (CURRENT_CPU_INDEX() == 0) {
        எனில் (!init_vtx_fixed_மதிப்புகள்(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            printf("vt-x: lack of required features\n");
            திரும்பு false;
        }
    }
    எனில் (!check_vtx_fixed_மதிப்புகள்(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        printf("vt-x: cores have inconsistent features\n");
        திரும்பு false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    கட்டமைப்பு vmxon_region *vmxon_kptr = &ARCH_முனை_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    எனில் (!vtx_check_fixed_மதிப்புகள்(read_cr0(), read_cr4())) {
        திரும்பு false;
    }
    எனில் (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        printf("vt-x: vmxon failure\n");
        திரும்பு false;
    }
    memset(&msr_bitmap_region, ~0, அளவு(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.கீழ்_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.கீழ்_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.கீழ்_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.கீழ்_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.கீழ்_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.கீழ்_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.உயர்வு_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.உயர்வு_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.உயர்வு_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.உயர்வு_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.உயர்வு_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.உயர்வு_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.சொற்கள்[0] = x86_rdmsr_கீழ்(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.சொற்கள்[1] = x86_rdmsr_உயர்வு(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    எனில் (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        printf("vt-x: Expected wb attribute for EPT paging structure\n");
        திரும்பு false;
    }
    எனில் (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        printf("vt-x: Expected supported for 2m pages\n");
        திரும்பு false;
    }

    திரும்பு true;
}

நிலையான வெறுமை setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *buffer;
    முழு i;

    buffer = lookupIPCBuffer(true, முனை_STATE(ksCurThread));

    setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_DATA_EXIT_INSTRUCTION_LENGTH));
    setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_DATA_GUEST_PHYSICAL));
    setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    க்கு (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(முனை_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_EAX + i,
              முனை_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

நிலையான வெறுமை handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(முனை_STATE(ksCurThread), செய்தி_Info_Register, SEL4_VMENTER_RESULT_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

நிலையான உள்வரி வெறுமை finishVmexitSaving(வெறுமை)
{
    vcpu_t *vcpu = ARCH_முனை_STATE(x86KSCurrentVCPU);
    ப்ரதிஜ்ஞா(vcpu == முனை_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(வெறுமை)
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
    எனில் (reason == EXTERNAL_INTERRUPT) {
        முனை_LOCK_IRQ;
        எனில் (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_DATA_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_முனை_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } இல்லையெனில் {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(முனை_STATE(ksCurThread));
#endif
        திரும்பு EXCEPTION_NONE;
    }

    முனை_LOCK_SYS;

    தேர்வு (reason) {
    நிலை EXCEPTION_OR_NMI:
    நிலை MOV_DR:
    நிலை TASK_SWITCH:
    நிலை CONTROL_REGISTER:
    நிலை IO:
    நிலை MWAIT:
    நிலை SIPI:
    நிலை INVLPG:
    நிலை INVEPT:
    நிலை INVVPID:
    நிலை VMCLEAR:
    நிலை VMPTRLD:
    நிலை VMPTRST:
    நிலை VMREAD:
    நிலை VMWRITE:
    நிலை VMXON:
    நிலை EPT_VIOLATION:
    நிலை GDTR_OR_IDTR:
    நிலை LDTR_OR_TR:
    நிலை TPR_BELOW_THRESHOLD:
    நிலை APIC_ACCESS:
        qualification = vmread(VMX_DATA_EXIT_QUALIFICATION);
        நிறுத்து;
    இயல்பு:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    திரும்பு EXCEPTION_NONE;
}

exception_t handleVmEntryFail(வெறுமை)
{
    handleVmxFault(-1, -1);

    திரும்பு EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
வெறுமை VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    ப்ரதிஜ்ஞா(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    எனில் (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {

        word_t badge = notification_ptr_get_ntfn_செய்தி_Identifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        எனில் (tcb != முனை_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

நிலையான வெறுமை invvpid_context(uint16_t vpid)
{
    கட்டமைப்பு {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    asm நிலையற்ற("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
}

நிலையான வெறுமை setEPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_வேர்;
    எனில் (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_வேர் = kpptr_to_paddr(null_ept_space);
    } இல்லையெனில் {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        எனில் (find_ret.status != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_வேர் = kpptr_to_paddr(null_ept_space);
        } இல்லையெனில் {
            ept_வேர் = pptr_to_paddr(pml4);
        }
    }
    எனில் (ept_வேர் != vcpu->last_ept_வேர்) {
        vcpu->last_ept_வேர் = ept_வேர்;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_வேர்,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.சொற்கள்[0]);
        ப்ரதிஜ்ஞா(vcpu->vpid != VPID_INVALID);
        எனில் (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

வெறுமை clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t last)
{
    எனில் (vpid == VPID_INVALID) {
        திரும்பு;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    எனில் (vcpu == வெற்று) {
        திரும்பு;
    }
    ப்ரதிஜ்ஞா(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

நிலையான உள்வரி vpid_t nextVPID(vpid_t vpid)
{
    எனில் (vpid == VPID_LAST) {
        திரும்பு VPID_FIRST;
    } இல்லையெனில் {
        திரும்பு vpid + 1;
    }
}

நிலையான வெறுமை invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, அளவு(vcpu->io));
    /* invalidate the VPID context */
    எனில் (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
நிலையான வெறுமை releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = வெற்று;
}

நிலையான vpid_t findFreeVPID(வெறுமை)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    செய் {
        எனில் (x86KSVPIDTable[vpid] == வெற்று) {
            திரும்பு vpid;
        }
        vpid = nextVPID(vpid);
    } போது (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = nextVPID(x86KSNextVPID);
    திரும்பு vpid;
}

நிலையான வெறுமை storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    ப்ரதிஜ்ஞா(x86KSVPIDTable[vpid] == வெற்று);
    ப்ரதிஜ்ஞா(vcpu->vpid == VPID_INVALID);
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
வெறுமை vcpu_fpu_to_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    எனில் (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        எனில் (!vcpu->fpu_active) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_active = true;
            loadFpuState(tcb);
        }
    } இல்லையெனில் {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_active = true;
        switchLocalFpuOwner(tcb);
    }
}

வெறுமை vcpu_fpu_to_host(tcb_t *tcb, vcpu_t *vcpu)
{
    எனில் (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } இல்லையெனில் எனில் (nativeThreadUsingFPU(tcb) && vcpu->fpu_active) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_active = false;
        loadFpuState(tcb);
    } இல்லையெனில் எனில் (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

வெறுமை restoreVMCS(வெறுமை)
{
    tcb_t *cur_thread = முனை_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    எனில் (ARCH_முனை_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ifndef CONFIG_KERNEL_SKIM_WINDOW
    எனில் (getCurrentCR3().சொற்கள்[0] != expected_vmcs->last_host_cr3) {
        expected_vmcs->last_host_cr3 = getCurrentCR3().சொற்கள்[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().சொற்கள்[0]);
    }
#endif
    எனில் (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        எனில் (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    setEPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

வெறுமை invept(ept_pml4e_t *ept_pml4)
{
    எனில் (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        கட்டமைப்பு {
            uint64_t parts[2];
        } address;
        word_t type;
        எனில் (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } இல்லையெனில் எனில் (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } இல்லையெனில் {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            திரும்பு;
        }

        address.parts[0] = pptr_to_paddr((வெறுமை *)ept_pml4);
        address.parts[1] = 0;
        asm நிலையற்ற(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
