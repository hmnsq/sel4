/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#ifdef CONFIG_VTX

#ضمن <types.h>
#ضمن <machine/io.h>
#ضمن <api/failures.h>
#ضمن <api/syscall.h>
#ضمن <kernel/thread.h>
#ضمن <object/objecttype.h>
#ضمن <arch/machine/cpu_registers.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/object/vcpu.h>
#ضمن <arch/object/ioport.h>
#ضمن <util.h>
#ضمن <sel4/arch/vmenter.h>

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

ثابت vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

تعريفنوع بنية msr_bitmap {
    word_t bitmap[0x2000 / حجم(word_t) / 8];
} msr_bitmap_t;

تعريفنوع بنية msr_bitmaps {
    msr_bitmap_t low_msr_read;
    msr_bitmap_t high_msr_read;
    msr_bitmap_t low_msr_write;
    msr_bitmap_t high_msr_write;
} msr_bitmaps_t;

ساكن msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

ساكن حرف null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
ساكن uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
ساكن vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
ساكن uint32_t pin_control_high;
ساكن uint32_t pin_control_low;
ساكن uint32_t primary_control_high;
ساكن uint32_t primary_control_low;
ساكن uint32_t secondary_control_high;
ساكن uint32_t secondary_control_low;
ساكن uint32_t entry_control_high;
ساكن uint32_t entry_control_low;
ساكن uint32_t exit_control_high;
ساكن uint32_t exit_control_low;
ساكن uint32_t cr0_high;
ساكن uint32_t cr0_low;
ساكن uint32_t cr4_high;
ساكن uint32_t cr4_low;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
ساكن bool_t vmx_feature_vpid;
ساكن bool_t vmx_feature_load_perf_global_ctrl;
ساكن bool_t vmx_feature_ack_on_exit;

ساكن vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
ساكن vpid_t x86KSNextVPID = VPID_FIRST;

ساكن فراغ releaseVPID(vpid_t vpid);

ساكن مضمن bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm متطاير(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    أرجع !!error;
}

ساكن فراغ vmclear(فراغ *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((فراغ *)vmcs_ptr);
    asm متطاير(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

فراغ clearCurrentVCPU(فراغ)
{
    vcpu_t *vcpu = ARCH_NODE_STATE(x86KSCurrentVCPU);
    𐡄𐡍 (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_NODE_STATE(x86KSCurrentVCPU) = عدم;
        𐡄𐡍 (vcpu->fpu_active && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_active = false;
        }
    }
}

ساكن فراغ vmptrld(فراغ *vmcs_ptr)
{
    uint64_t physical_address;
    uint8_t error;
    physical_address = pptr_to_paddr(vmcs_ptr);
    asm متطاير(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_address)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    أكد(!error);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
فراغ vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_STAR_MSR, vcpu->syscall_registers[VCPU_STAR]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_MASK]);
}

فراغ vcpu_restore_host_msrs(فراغ)
{
    init_syscall_msrs();
}
#endif

ساكن فراغ switchVCPU(vcpu_t *vcpu)
{
#ifdef ENABLE_SMP_SUPPORT
    𐡄𐡍 (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_NODE_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    𐡄𐡍 (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.words[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_NODE_STATE(x86KSCurrentVCPU) = vcpu;
}

ساكن فراغ print_bits(word_t bits)
{
    bool_t first = true;
    طالما (bits) {
        صحيح index = seL4_WordBits - 1 - clzl(bits);
        𐡄𐡍 (first) {
            printf("%d", index);
            first = false;
        } وإلا {
            printf(",%d", index);
        }
        bits &= ~BIT(index);
    }
}

ساكن bool_t check_fixed_value(word_t val, word_t low, word_t high)
{
    word_t not_high;
    word_t not_low;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_high = high & ~(high & val);
    𐡄𐡍 (not_high != 0) {
        printf("Failed to set bits: ");
        print_bits(not_high);
        أرجع false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_low = ~low & ~(~low & ~val);
    𐡄𐡍 (not_low != 0) {
        printf("Incorrectly cleared bits: ");
        print_bits(not_low);
        أرجع false;
    }
    أرجع true;
}

ساكن bool_t vtx_check_fixed_values(word_t cr0, word_t cr4)
{
    𐡄𐡍 (!check_fixed_value(cr0, cr0_low, cr0_high)) {
        printf(" of CR0\n");
        أرجع false;
    }
    𐡄𐡍 (!check_fixed_value(cr4, cr4_low, cr4_high)) {
        printf(" of CR4\n");
        أرجع false;
    }
    أرجع true;
}

ساكن bool_t BOOT_CODE init_vtx_fixed_values(bool_t useTrueMsrs)
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
    𐡄𐡍 (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } وإلا {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    pin_control_high = x86_rdmsr_low(pinbased_ctls);
    pin_control_low = x86_rdmsr_high(pinbased_ctls);
    primary_control_high = x86_rdmsr_low(procbased_ctls);
    primary_control_low = x86_rdmsr_high(procbased_ctls);
    secondary_control_high = x86_rdmsr_low(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_low = x86_rdmsr_high(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_high = x86_rdmsr_low(exit_ctls);
    exit_control_low = x86_rdmsr_high(exit_ctls);
    entry_control_high = x86_rdmsr_low(entry_ctls);
    entry_control_low = x86_rdmsr_high(entry_ctls);

    cr0_high = x86_rdmsr_low(IA32_VMX_CR0_FIXED0_MSR);
    cr0_low = x86_rdmsr_low(IA32_VMX_CR0_FIXED1_MSR);
    cr4_high = x86_rdmsr_low(IA32_VMX_CR4_FIXED0_MSR);
    cr4_low = x86_rdmsr_low(IA32_VMX_CR4_FIXED1_MSR);

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
    𐡄𐡍 (secondary_control_low & BIT(7)) {
        cr0_high &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    𐡄𐡍 (!(secondary_control_low & BIT(5))) {
        vmx_feature_vpid = 0;
        printf("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } وإلا {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    𐡄𐡍 (!(exit_control_low & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        printf("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } وإلا {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    𐡄𐡍 (!(exit_control_low & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        printf("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } وإلا {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_low) & pin_control_mask;
    𐡄𐡍 (missing) {
        printf("vt-x: Unsupported pin control features %lx\n", (long)missing);
        أرجع false;
    }
    missing = (~primary_control_low) & primary_control_mask;
    𐡄𐡍 (missing) {
        printf("vt-x: Unsupported primary control features %lx\n", (long)missing);
        أرجع false;
    }
    missing = (~secondary_control_low) & secondary_control_mask;
    𐡄𐡍 (missing) {
        printf("vt-x: Unsupported secondary control features %lx\n", (long)missing);
        أرجع false;
    }
    missing = (~exit_control_low) & exit_control_mask;
    𐡄𐡍 (missing) {
        printf("vt-x: Unsupported exit control features %lx\n", (long)missing);
        أرجع false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_low) & BIT(9);
    𐡄𐡍 (missing) {
        printf("vt-x: CPU does not support 64-bit guests\n");
        أرجع false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_high |= pin_control_mask;
    primary_control_high |= primary_control_mask;
    secondary_control_high |= secondary_control_mask;
    exit_control_high |= exit_control_mask;

    أرجع true;
}

ساكن bool_t BOOT_CODE check_vtx_fixed_values(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    𐡄𐡍 (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } وإلا {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    uint32_t local_pin_control_high = x86_rdmsr_low(pinbased_ctls);
    uint32_t local_pin_control_low = x86_rdmsr_high(pinbased_ctls);
    uint32_t local_primary_control_high = x86_rdmsr_low(procbased_ctls);
    uint32_t local_primary_control_low = x86_rdmsr_high(procbased_ctls);
    uint32_t local_secondary_control_high = x86_rdmsr_low(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_low = x86_rdmsr_high(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_high = x86_rdmsr_low(exit_ctls);
    uint32_t local_exit_control_low = x86_rdmsr_high(exit_ctls);
    uint32_t local_entry_control_high = x86_rdmsr_low(entry_ctls);
    uint32_t local_entry_control_low = x86_rdmsr_high(entry_ctls);

    uint32_t local_cr0_high = x86_rdmsr_low(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_low = x86_rdmsr_low(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_high = x86_rdmsr_low(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_low = x86_rdmsr_low(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    𐡄𐡍 (local_secondary_control_low & BIT(7)) {
        local_cr0_high &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    أرجع
        (local_pin_control_high & pin_control_high) == local_pin_control_high &&
        (~local_pin_control_low & pin_control_high) == 0 &&
        (local_primary_control_high & primary_control_high) == local_primary_control_high &&
        (~local_primary_control_low & primary_control_high) == 0 &&
        (local_secondary_control_high & secondary_control_high) == local_secondary_control_high &&
        (~local_secondary_control_low & secondary_control_high) == 0 &&
        (local_exit_control_high & exit_control_high) == local_exit_control_high &&
        (~local_exit_control_low & exit_control_high) == 0 &&
        (local_entry_control_high & entry_control_high) == local_entry_control_high &&
        (~local_entry_control_low & entry_control_high) == 0 &&
        local_cr0_high == cr0_high &&
        local_cr0_low == cr0_low &&
        local_cr4_high == cr4_high &&
        local_cr4_low == cr4_low;
}

ساكن مضمن uint32_t applyFixedBits(uint32_t original, uint32_t high, uint32_t low)
{
    original |= high;
    original &= low;
    أرجع original;
}

فراغ vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = عدم;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    memcpy(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_high & cr0_low;
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
    𐡄𐡍 (vmx_feature_load_perf_global_ctrl) {
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
    𐡄𐡍 (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.words[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_register]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_to_paddr(x64KSKernelPML4), 0).words[0]);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_high & pin_control_low);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_high & primary_control_low);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_high & secondary_control_low);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_high & exit_control_low);

    uint64_t entry_control = entry_control_high & entry_control_low;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_high & cr4_low);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_HIGH, ~(word_t)0);

    memset(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_SIZE);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((حرف *)vcpu->io + (VCPU_IOBITMAP_SIZE / 2)));
}

ساكن فراغ dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    أكد(tcb->tcbArch.tcbVCPU == vcpu);
    أكد(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = عدم;
    vcpu->vcpuTCB = عدم;
}

فراغ vcpu_finalise(vcpu_t *vcpu)
{
    𐡄𐡍 (ARCH_NODE_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
#ifdef ENABLE_SMP_SUPPORT
        𐡄𐡍 (vcpu->last_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->last_cpu);
        } وإلا
#endif /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    𐡄𐡍 (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    𐡄𐡍 (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

ساكن فراغ associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    𐡄𐡍 (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    𐡄𐡍 (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

ساكن exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *buffer)
{
    صحيح i;
    لكل (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, buffer);
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    𐡄𐡍 (length < 15) {
#وإلا
    𐡄𐡍 (length < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
ساكن exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *buffer, word_t field, word_t value)
{
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);
    𐡄𐡍 (ARCH_NODE_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بدل (field) {
    حالة IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = value;
        اكسر;
    حالة IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = value;
        اكسر;
    حالة IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = value;
        اكسر;
    حالة IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = value;
        اكسر;
    }
    setMR(thread, buffer, 0, value);
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUWriteMSR(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t value;

    𐡄𐡍 (length < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    value = getSyscallArg(1, buffer);
    بدل (field) {
    حالة IA32_LSTAR_MSR:
    حالة IA32_STAR_MSR:
    حالة IA32_CSTAR_MSR:
    حالة IA32_FMASK_MSR:
        اكسر;
    افتراضي:
        userError("VCPU WriteMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer, field, value);
}

ساكن exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *buffer)
{
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);

    word_t value = 0;

    بدل (field) {
    حالة IA32_LSTAR_MSR:
        value = vcpu->syscall_registers[VCPU_LSTAR];
        اكسر;
    حالة IA32_STAR_MSR:
        value = vcpu->syscall_registers[VCPU_STAR];
        اكسر;
    حالة IA32_CSTAR_MSR:
        value = vcpu->syscall_registers[VCPU_CSTAR];
        اكسر;
    حالة IA32_FMASK_MSR:
        value = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        اكسر;
    حالة IA32_VMX_MISC_MSR:
        value = x86_rdmsr(field);
        اكسر;
    }

    setMR(thread, buffer, 0, value);
    setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUReadMSR(cap_t cap, word_t length, word_t *buffer)
{
    𐡄𐡍 (length < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    بدل (field) {
    حالة IA32_LSTAR_MSR:
    حالة IA32_STAR_MSR:
    حالة IA32_CSTAR_MSR:
    حالة IA32_FMASK_MSR:
    حالة IA32_VMX_MISC_MSR:
        اكسر;
    افتراضي:
        userError("VCPU ReadMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, buffer);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

ساكن exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t low, uint16_t high)
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
    setIOPortMask(vcpu->io, low, high, false);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeEnableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t low, high;

    𐡄𐡍 (length < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    𐡄𐡍 (current_extra_caps.excaprefs[0] == عدم) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    𐡄𐡍 (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    low = getSyscallArg(0, buffer);
    high = getSyscallArg(1, buffer);

    𐡄𐡍 (low < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || high > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeEnableIOPort(vcpu, ioSlot, ioCap, low, high);
}

ساكن exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t low, uint16_t high)
{
    setIOPortMask(vcpu->io, low, high, true);
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeDisableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    uint16_t low, high;

    𐡄𐡍 (length < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    low = getSyscallArg(0, buffer);
    high = getSyscallArg(1, buffer);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    أرجع invokeDisableIOPort(vcpu, low, high);
}

ساكن exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *buffer, word_t field, word_t value)
{
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);

    𐡄𐡍 (ARCH_NODE_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بدل (field) {
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = value;
        اكسر;
    حالة VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = value;
        اكسر;
    حالة VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = value;
        اكسر;
    حالة VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = value;
        اكسر;
    }
    vmwrite(field, value);

    𐡄𐡍 (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned صحيح length = setMR(thread, buffer, 0, value);
        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeWriteVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    word_t value;

    𐡄𐡍 (length < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    value = getSyscallArg(1, buffer);
    بدل (field) {
    حالة VMX_GUEST_RIP:
    حالة VMX_GUEST_RSP:
    حالة VMX_GUEST_ES_SELECTOR:
    حالة VMX_GUEST_CS_SELECTOR:
    حالة VMX_GUEST_SS_SELECTOR:
    حالة VMX_GUEST_DS_SELECTOR:
    حالة VMX_GUEST_FS_SELECTOR:
    حالة VMX_GUEST_GS_SELECTOR:
    حالة VMX_GUEST_LDTR_SELECTOR:
    حالة VMX_GUEST_TR_SELECTOR:
    حالة VMX_GUEST_DEBUGCTRL:
    حالة VMX_GUEST_PAT:
    حالة VMX_GUEST_EFER:
    حالة VMX_GUEST_PERF_GLOBAL_CTRL:
    حالة VMX_GUEST_PDPTE0:
    حالة VMX_GUEST_PDPTE1:
    حالة VMX_GUEST_PDPTE2:
    حالة VMX_GUEST_PDPTE3:
    حالة VMX_GUEST_ES_LIMIT:
    حالة VMX_GUEST_CS_LIMIT:
    حالة VMX_GUEST_SS_LIMIT:
    حالة VMX_GUEST_DS_LIMIT:
    حالة VMX_GUEST_FS_LIMIT:
    حالة VMX_GUEST_GS_LIMIT:
    حالة VMX_GUEST_LDTR_LIMIT:
    حالة VMX_GUEST_TR_LIMIT:
    حالة VMX_GUEST_GDTR_LIMIT:
    حالة VMX_GUEST_IDTR_LIMIT:
    حالة VMX_GUEST_ES_ACCESS_RIGHTS:
    حالة VMX_GUEST_CS_ACCESS_RIGHTS:
    حالة VMX_GUEST_SS_ACCESS_RIGHTS:
    حالة VMX_GUEST_DS_ACCESS_RIGHTS:
    حالة VMX_GUEST_FS_ACCESS_RIGHTS:
    حالة VMX_GUEST_GS_ACCESS_RIGHTS:
    حالة VMX_GUEST_LDTR_ACCESS_RIGHTS:
    حالة VMX_GUEST_TR_ACCESS_RIGHTS:
    حالة VMX_GUEST_INTERRUPTABILITY:
    حالة VMX_GUEST_ACTIVITY:
    حالة VMX_GUEST_SMBASE:
    حالة VMX_GUEST_SYSENTER_CS:
    حالة VMX_GUEST_PREEMPTION_TIMER_VALUE:
    حالة VMX_GUEST_ES_BASE:
    حالة VMX_GUEST_CS_BASE:
    حالة VMX_GUEST_SS_BASE:
    حالة VMX_GUEST_DS_BASE:
    حالة VMX_GUEST_FS_BASE:
    حالة VMX_GUEST_GS_BASE:
    حالة VMX_GUEST_LDTR_BASE:
    حالة VMX_GUEST_TR_BASE:
    حالة VMX_GUEST_GDTR_BASE:
    حالة VMX_GUEST_IDTR_BASE:
    حالة VMX_GUEST_DR7:
    حالة VMX_GUEST_RFLAGS:
    حالة VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالة VMX_GUEST_SYSENTER_ESP:
    حالة VMX_GUEST_SYSENTER_EIP:
    حالة VMX_CONTROL_CR0_MASK:
    حالة VMX_CONTROL_CR4_MASK:
    حالة VMX_CONTROL_CR0_READ_SHADOW:
    حالة VMX_CONTROL_CR4_READ_SHADOW:
    حالة VMX_GUEST_CR3:
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
    حالة VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    حالة VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        اكسر;
    حالة VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        value = applyFixedBits(value, pin_control_high, pin_control_low);
        اكسر;
    حالة VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        value = applyFixedBits(value, primary_control_high, primary_control_low);
        اكسر;
    حالة VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        value = applyFixedBits(value, secondary_control_high, secondary_control_low);
        اكسر;
    حالة VMX_CONTROL_ENTRY_CONTROLS:
        value = applyFixedBits(value, entry_control_high, entry_control_low);
        اكسر;
    حالة VMX_CONTROL_EXIT_CONTROLS:
        value = applyFixedBits(value, exit_control_high, exit_control_low);
        اكسر;
    حالة VMX_GUEST_CR0:
        value = applyFixedBits(value, cr0_high, cr0_low);
        اكسر;
    حالة VMX_GUEST_CR4:
        value = applyFixedBits(value, cr4_high, cr4_low);
        اكسر;
    افتراضي:
        userError("VCPU WriteVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, buffer, field, value);
}

ساكن word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    بدل (field) {
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
        أرجع vcpu->exception_bitmap;
    حالة VMX_GUEST_CR0:
        أرجع vcpu->cr0;
    حالة VMX_CONTROL_CR0_MASK:
        أرجع vcpu->cr0_mask;
    حالة VMX_CONTROL_CR0_READ_SHADOW:
        أرجع vcpu->cr0_shadow;
    }
    𐡄𐡍 (ARCH_NODE_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    أرجع vmread(field);
}

ساكن exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *buffer)
{
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);
    word_t value = readVMCSField(vcpu, field);
    𐡄𐡍 (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned صحيح length = setMR(thread, buffer, 0, value);
        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeReadVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    𐡄𐡍 (length < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    بدل (field) {
    حالة VMX_GUEST_RIP:
    حالة VMX_GUEST_RSP:
    حالة VMX_GUEST_ES_SELECTOR:
    حالة VMX_GUEST_CS_SELECTOR:
    حالة VMX_GUEST_SS_SELECTOR:
    حالة VMX_GUEST_DS_SELECTOR:
    حالة VMX_GUEST_FS_SELECTOR:
    حالة VMX_GUEST_GS_SELECTOR:
    حالة VMX_GUEST_LDTR_SELECTOR:
    حالة VMX_GUEST_TR_SELECTOR:
    حالة VMX_GUEST_DEBUGCTRL:
    حالة VMX_GUEST_PAT:
    حالة VMX_GUEST_EFER:
    حالة VMX_GUEST_PERF_GLOBAL_CTRL:
    حالة VMX_GUEST_PDPTE0:
    حالة VMX_GUEST_PDPTE1:
    حالة VMX_GUEST_PDPTE2:
    حالة VMX_GUEST_PDPTE3:
    حالة VMX_GUEST_ES_LIMIT:
    حالة VMX_GUEST_CS_LIMIT:
    حالة VMX_GUEST_SS_LIMIT:
    حالة VMX_GUEST_DS_LIMIT:
    حالة VMX_GUEST_FS_LIMIT:
    حالة VMX_GUEST_GS_LIMIT:
    حالة VMX_GUEST_LDTR_LIMIT:
    حالة VMX_GUEST_TR_LIMIT:
    حالة VMX_GUEST_GDTR_LIMIT:
    حالة VMX_GUEST_IDTR_LIMIT:
    حالة VMX_GUEST_ES_ACCESS_RIGHTS:
    حالة VMX_GUEST_CS_ACCESS_RIGHTS:
    حالة VMX_GUEST_SS_ACCESS_RIGHTS:
    حالة VMX_GUEST_DS_ACCESS_RIGHTS:
    حالة VMX_GUEST_FS_ACCESS_RIGHTS:
    حالة VMX_GUEST_GS_ACCESS_RIGHTS:
    حالة VMX_GUEST_LDTR_ACCESS_RIGHTS:
    حالة VMX_GUEST_TR_ACCESS_RIGHTS:
    حالة VMX_GUEST_INTERRUPTABILITY:
    حالة VMX_GUEST_ACTIVITY:
    حالة VMX_GUEST_SMBASE:
    حالة VMX_GUEST_SYSENTER_CS:
    حالة VMX_GUEST_PREEMPTION_TIMER_VALUE:
    حالة VMX_GUEST_ES_BASE:
    حالة VMX_GUEST_CS_BASE:
    حالة VMX_GUEST_SS_BASE:
    حالة VMX_GUEST_DS_BASE:
    حالة VMX_GUEST_FS_BASE:
    حالة VMX_GUEST_GS_BASE:
    حالة VMX_GUEST_LDTR_BASE:
    حالة VMX_GUEST_TR_BASE:
    حالة VMX_GUEST_GDTR_BASE:
    حالة VMX_GUEST_IDTR_BASE:
    حالة VMX_GUEST_DR7:
    حالة VMX_GUEST_RFLAGS:
    حالة VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالة VMX_GUEST_SYSENTER_ESP:
    حالة VMX_GUEST_SYSENTER_EIP:
    حالة VMX_CONTROL_CR0_MASK:
    حالة VMX_CONTROL_CR4_MASK:
    حالة VMX_CONTROL_CR0_READ_SHADOW:
    حالة VMX_CONTROL_CR4_READ_SHADOW:
    حالة VMX_DATA_INSTRUCTION_ERROR:
    حالة VMX_DATA_EXIT_INTERRUPT_INFO:
    حالة VMX_DATA_EXIT_INTERRUPT_ERROR:
    حالة VMX_DATA_IDT_VECTOR_INFO:
    حالة VMX_DATA_IDT_VECTOR_ERROR:
    حالة VMX_DATA_EXIT_INSTRUCTION_LENGTH:
    حالة VMX_DATA_EXIT_INSTRUCTION_INFO:
    حالة VMX_DATA_GUEST_PHYSICAL:
    حالة VMX_DATA_IO_RCX:
    حالة VMX_DATA_IO_RSI:
    حالة VMX_DATA_IO_RDI:
    حالة VMX_DATA_IO_RIP:
    حالة VMX_DATA_GUEST_LINEAR_ADDRESS:
    حالة VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    حالة VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    حالة VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    حالة VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
    حالة VMX_CONTROL_ENTRY_CONTROLS:
    حالة VMX_CONTROL_EXIT_CONTROLS:
    حالة VMX_GUEST_CR0:
    حالة VMX_GUEST_CR3:
    حالة VMX_GUEST_CR4:
        اكسر;
    افتراضي:
        userError("VCPU ReadVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, buffer);
}

ساكن exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeSetTCB(cap_t cap, word_t length, word_t *buffer)
{
    cap_t tcbCap;
    𐡄𐡍 (current_extra_caps.excaprefs[0] == عدم) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    𐡄𐡍 (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

فراغ vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *buffer;
    𐡄𐡍 (ARCH_NODE_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    buffer = lookupIPCBuffer(false, NODE_STATE(ksCurThread));
    𐡄𐡍 (!buffer) {
        userError("No IPC buffer.");
        أرجع;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, buffer));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, buffer), primary_control_high,
                                                                   primary_control_low));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, buffer));
}

فراغ vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *buffer;
    vcpu_t *vcpu;

    buffer = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    أكد(vcpu);

    𐡄𐡍 (ARCH_NODE_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, msgInfoRegister, 0);
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
    بدل (invLabel) {
    حالة X86VCPUSetTCB:
        أرجع decodeSetTCB(cap, length, buffer);
    حالة X86VCPUReadVMCS:
        أرجع decodeReadVMCS(cap, length, call, buffer);
    حالة X86VCPUWriteVMCS:
        أرجع decodeWriteVMCS(cap, length, call, buffer);
    حالة X86VCPUEnableIOPort:
        أرجع decodeEnableIOPort(cap, length, buffer);
    حالة X86VCPUDisableIOPort:
        أرجع decodeDisableIOPort(cap, length, buffer);
    حالة X86VCPUWriteRegisters:
        أرجع decodeVCPUWriteRegisters(cap, length, buffer);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    حالة X86VCPUWriteMSR:
        أرجع decodeVCPUWriteMSR(cap, length, buffer);
    حالة X86VCPUReadMSR:
        أرجع decodeVCPUReadMSR(cap, length, buffer);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    افتراضي:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

ساكن bool_t is_vtx_supported(فراغ)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    أرجع !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

ساكن مضمن فراغ clear_bit(word_t *bitmap, word_t bit)
{
    صحيح index = bit / (حجم(word_t) * 8);
    صحيح offset = bit % (حجم(word_t) * 8);
    bitmap[index] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(فراغ)
{
    𐡄𐡍 (!is_vtx_supported()) {
        printf("vt-x: not supported\n");
        أرجع false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.words[0] = x86_rdmsr_low(IA32_VMX_BASIC_MSR);
    vmx_basic.words[1] = x86_rdmsr_high(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.words[0] = x86_rdmsr_low(IA32_FEATURE_CONTROL_MSR);
    𐡄𐡍 (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        𐡄𐡍 (feature_control_msr_get_lock(feature_control)) {
            printf("vt-x: feature locked\n");
            أرجع false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_high(IA32_FEATURE_CONTROL_MSR), feature_control.words[0]);
    }
    /* make sure the msr is locked */
    𐡄𐡍 (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_high(IA32_FEATURE_CONTROL_MSR), feature_control.words[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    𐡄𐡍 (CURRENT_CPU_INDEX() == 0) {
        𐡄𐡍 (!init_vtx_fixed_values(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            printf("vt-x: lack of required features\n");
            أرجع false;
        }
    }
    𐡄𐡍 (!check_vtx_fixed_values(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        printf("vt-x: cores have inconsistent features\n");
        أرجع false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    بنية vmxon_region *vmxon_kptr = &ARCH_NODE_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    𐡄𐡍 (!vtx_check_fixed_values(read_cr0(), read_cr4())) {
        أرجع false;
    }
    𐡄𐡍 (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        printf("vt-x: vmxon failure\n");
        أرجع false;
    }
    memset(&msr_bitmap_region, ~0, حجم(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.low_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.low_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.low_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.low_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.low_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.low_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.high_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.high_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.high_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.high_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.high_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.high_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.words[0] = x86_rdmsr_low(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.words[1] = x86_rdmsr_high(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    𐡄𐡍 (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        printf("vt-x: Expected wb attribute for EPT paging structure\n");
        أرجع false;
    }
    𐡄𐡍 (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        printf("vt-x: Expected supported for 2m pages\n");
        أرجع false;
    }

    أرجع true;
}

ساكن فراغ setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *buffer;
    صحيح i;

    buffer = lookupIPCBuffer(true, NODE_STATE(ksCurThread));

    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_DATA_EXIT_INSTRUCTION_LENGTH));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_DATA_GUEST_PHYSICAL));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    لكل (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_EAX + i,
              NODE_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

ساكن فراغ handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(NODE_STATE(ksCurThread), msgInfoRegister, SEL4_VMENTER_RESULT_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

ساكن مضمن فراغ finishVmexitSaving(فراغ)
{
    vcpu_t *vcpu = ARCH_NODE_STATE(x86KSCurrentVCPU);
    أكد(vcpu == NODE_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(فراغ)
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
    𐡄𐡍 (reason == EXTERNAL_INTERRUPT) {
        NODE_LOCK_IRQ;
        𐡄𐡍 (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_DATA_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_NODE_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } وإلا {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(NODE_STATE(ksCurThread));
#endif
        أرجع EXCEPTION_NONE;
    }

    NODE_LOCK_SYS;

    بدل (reason) {
    حالة EXCEPTION_OR_NMI:
    حالة MOV_DR:
    حالة TASK_SWITCH:
    حالة CONTROL_REGISTER:
    حالة IO:
    حالة MWAIT:
    حالة SIPI:
    حالة INVLPG:
    حالة INVEPT:
    حالة INVVPID:
    حالة VMCLEAR:
    حالة VMPTRLD:
    حالة VMPTRST:
    حالة VMREAD:
    حالة VMWRITE:
    حالة VMXON:
    حالة EPT_VIOLATION:
    حالة GDTR_OR_IDTR:
    حالة LDTR_OR_TR:
    حالة TPR_BELOW_THRESHOLD:
    حالة APIC_ACCESS:
        qualification = vmread(VMX_DATA_EXIT_QUALIFICATION);
        اكسر;
    افتراضي:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    أرجع EXCEPTION_NONE;
}

exception_t handleVmEntryFail(فراغ)
{
    handleVmxFault(-1, -1);

    أرجع EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
فراغ VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    أكد(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    𐡄𐡍 (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {

        word_t badge = notification_ptr_get_ntfnMsgIdentifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        𐡄𐡍 (tcb != NODE_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

ساكن فراغ invvpid_context(uint16_t vpid)
{
    بنية {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    asm متطاير("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
}

ساكن فراغ setEPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_root;
    𐡄𐡍 (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_root = kpptr_to_paddr(null_ept_space);
    } وإلا {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        𐡄𐡍 (find_ret.status != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_root = kpptr_to_paddr(null_ept_space);
        } وإلا {
            ept_root = pptr_to_paddr(pml4);
        }
    }
    𐡄𐡍 (ept_root != vcpu->last_ept_root) {
        vcpu->last_ept_root = ept_root;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_root,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.words[0]);
        أكد(vcpu->vpid != VPID_INVALID);
        𐡄𐡍 (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

فراغ clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t last)
{
    𐡄𐡍 (vpid == VPID_INVALID) {
        أرجع;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    𐡄𐡍 (vcpu == عدم) {
        أرجع;
    }
    أكد(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

ساكن مضمن vpid_t nextVPID(vpid_t vpid)
{
    𐡄𐡍 (vpid == VPID_LAST) {
        أرجع VPID_FIRST;
    } وإلا {
        أرجع vpid + 1;
    }
}

ساكن فراغ invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, حجم(vcpu->io));
    /* invalidate the VPID context */
    𐡄𐡍 (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
ساكن فراغ releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = عدم;
}

ساكن vpid_t findFreeVPID(فراغ)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    𐡏𐡁𐡃 {
        𐡄𐡍 (x86KSVPIDTable[vpid] == عدم) {
            أرجع vpid;
        }
        vpid = nextVPID(vpid);
    } طالما (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = nextVPID(x86KSNextVPID);
    أرجع vpid;
}

ساكن فراغ storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    أكد(x86KSVPIDTable[vpid] == عدم);
    أكد(vcpu->vpid == VPID_INVALID);
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
فراغ vcpu_fpu_to_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    𐡄𐡍 (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        𐡄𐡍 (!vcpu->fpu_active) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_active = true;
            loadFpuState(tcb);
        }
    } وإلا {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_active = true;
        switchLocalFpuOwner(tcb);
    }
}

فراغ vcpu_fpu_to_host(tcb_t *tcb, vcpu_t *vcpu)
{
    𐡄𐡍 (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } وإلا 𐡄𐡍 (nativeThreadUsingFPU(tcb) && vcpu->fpu_active) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_active = false;
        loadFpuState(tcb);
    } وإلا 𐡄𐡍 (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

فراغ restoreVMCS(فراغ)
{
    tcb_t *cur_thread = NODE_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    𐡄𐡍 (ARCH_NODE_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ifndef CONFIG_KERNEL_SKIM_WINDOW
    𐡄𐡍 (getCurrentCR3().words[0] != expected_vmcs->last_host_cr3) {
        expected_vmcs->last_host_cr3 = getCurrentCR3().words[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().words[0]);
    }
#endif
    𐡄𐡍 (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        𐡄𐡍 (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    setEPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

فراغ invept(ept_pml4e_t *ept_pml4)
{
    𐡄𐡍 (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        بنية {
            uint64_t parts[2];
        } address;
        word_t type;
        𐡄𐡍 (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } وإلا 𐡄𐡍 (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } وإلا {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            أرجع;
        }

        address.parts[0] = pptr_to_paddr((فراغ *)ept_pml4);
        address.parts[1] = 0;
        asm متطاير(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
