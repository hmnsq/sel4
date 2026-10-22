/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>

#ifdef CONFIG_VTX

#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <api/failures.h>
#অন্তর্ভাৱ <api/syscall.h>
#অন্তর্ভাৱ <kernel/thread.h>
#অন্তর্ভাৱ <object/objecttype.h>
#অন্তর্ভাৱ <arch/machine/cpu_registers.h>
#অন্তর্ভাৱ <arch/model/statedata.h>
#অন্তর্ভাৱ <arch/object/vcpu.h>
#অন্তর্ভাৱ <arch/object/ioport.h>
#অন্তর্ভাৱ <util.h>
#অন্তর্ভাৱ <sel4/arch/vmenter.h>

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

ধ্রুৱ vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

প্রকারনির্দেশ সংরচনা msr_bitmap {
    word_t bitmap[0x2000 / পরিমাণ(word_t) / 8];
} msr_bitmap_t;

প্রকারনির্দেশ সংরচনা msr_bitmaps {
    msr_bitmap_t নিচেৰ_msr_read;
    msr_bitmap_t উপৰেৰ_msr_read;
    msr_bitmap_t নিচেৰ_msr_write;
    msr_bitmap_t উপৰেৰ_msr_write;
} msr_bitmaps_t;

স্থির msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

স্থির ৱর্ণ null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
স্থির uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
স্থির vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
স্থির uint32_t pin_control_উপৰেৰ;
স্থির uint32_t pin_control_নিচেৰ;
স্থির uint32_t primary_control_উপৰেৰ;
স্থির uint32_t primary_control_নিচেৰ;
স্থির uint32_t secondary_control_উপৰেৰ;
স্থির uint32_t secondary_control_নিচেৰ;
স্থির uint32_t entry_control_উপৰেৰ;
স্থির uint32_t entry_control_নিচেৰ;
স্থির uint32_t exit_control_উপৰেৰ;
স্থির uint32_t exit_control_নিচেৰ;
স্থির uint32_t cr0_উপৰেৰ;
স্থির uint32_t cr0_নিচেৰ;
স্থির uint32_t cr4_উপৰেৰ;
স্থির uint32_t cr4_নিচেৰ;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
স্থির bool_t vmx_feature_vpid;
স্থির bool_t vmx_feature_load_perf_global_ctrl;
স্থির bool_t vmx_feature_ack_on_exit;

স্থির vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
স্থির vpid_t x86KSNextVPID = VPID_FIRST;

স্থির শূন্য releaseVPID(vpid_t vpid);

স্থির অন্তঃস্থ bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm চঞ্চল(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    ঘূৰাই !!error;
}

স্থির শূন্য vmclear(শূন্য *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((শূন্য *)vmcs_ptr);
    asm চঞ্চল(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

শূন্য clearCurrentVCPU(শূন্য)
{
    vcpu_t *vcpu = ARCH_গ্ৰন্থি_STATE(x86KSCurrentVCPU);
    যদি (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_গ্ৰন্থি_STATE(x86KSCurrentVCPU) = রিক্ত;
        যদি (vcpu->fpu_active && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_active = false;
        }
    }
}

স্থির শূন্য vmptrld(শূন্য *vmcs_ptr)
{
    uint64_t physical_address;
    uint8_t error;
    physical_address = pptr_to_paddr(vmcs_ptr);
    asm চঞ্চল(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_address)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    প্রতিজ্ঞা(!error);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
শূন্য vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_STAR_MSR, vcpu->syscall_registers[VCPU_STAR]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_MASK]);
}

শূন্য vcpu_restore_host_msrs(শূন্য)
{
    init_syscall_msrs();
}
#endif

স্থির শূন্য switchVCPU(vcpu_t *vcpu)
{
#ifdef ENABLE_SMP_SUPPORT
    যদি (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_গ্ৰন্থি_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    যদি (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((ৱর্ণ *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.শব্দগুলি[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_গ্ৰন্থি_STATE(x86KSCurrentVCPU) = vcpu;
}

স্থির শূন্য print_bits(word_t bits)
{
    bool_t first = true;
    যেতিয়ালৈ (bits) {
        পূর্ণাঙ্ক index = seL4_WordBits - 1 - clzl(bits);
        যদি (first) {
            printf("%d", index);
            first = false;
        } অন্যথা {
            printf(",%d", index);
        }
        bits &= ~BIT(index);
    }
}

স্থির bool_t check_fixed_মান(word_t val, word_t নিচেৰ, word_t উপৰেৰ)
{
    word_t not_উপৰেৰ;
    word_t not_নিচেৰ;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_উপৰেৰ = উপৰেৰ & ~(উপৰেৰ & val);
    যদি (not_উপৰেৰ != 0) {
        printf("Failed to set bits: ");
        print_bits(not_উপৰেৰ);
        ঘূৰাই false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_নিচেৰ = ~নিচেৰ & ~(~নিচেৰ & ~val);
    যদি (not_নিচেৰ != 0) {
        printf("Incorrectly cleared bits: ");
        print_bits(not_নিচেৰ);
        ঘূৰাই false;
    }
    ঘূৰাই true;
}

স্থির bool_t vtx_check_fixed_মানসমূহ(word_t cr0, word_t cr4)
{
    যদি (!check_fixed_মান(cr0, cr0_নিচেৰ, cr0_উপৰেৰ)) {
        printf(" of CR0\n");
        ঘূৰাই false;
    }
    যদি (!check_fixed_মান(cr4, cr4_নিচেৰ, cr4_উপৰেৰ)) {
        printf(" of CR4\n");
        ঘূৰাই false;
    }
    ঘূৰাই true;
}

স্থির bool_t BOOT_CODE init_vtx_fixed_মানসমূহ(bool_t useTrueMsrs)
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
    যদি (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } অন্যথা {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    pin_control_উপৰেৰ = x86_rdmsr_নিচেৰ(pinbased_ctls);
    pin_control_নিচেৰ = x86_rdmsr_উপৰেৰ(pinbased_ctls);
    primary_control_উপৰেৰ = x86_rdmsr_নিচেৰ(procbased_ctls);
    primary_control_নিচেৰ = x86_rdmsr_উপৰেৰ(procbased_ctls);
    secondary_control_উপৰেৰ = x86_rdmsr_নিচেৰ(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_নিচেৰ = x86_rdmsr_উপৰেৰ(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_উপৰেৰ = x86_rdmsr_নিচেৰ(exit_ctls);
    exit_control_নিচেৰ = x86_rdmsr_উপৰেৰ(exit_ctls);
    entry_control_উপৰেৰ = x86_rdmsr_নিচেৰ(entry_ctls);
    entry_control_নিচেৰ = x86_rdmsr_উপৰেৰ(entry_ctls);

    cr0_উপৰেৰ = x86_rdmsr_নিচেৰ(IA32_VMX_CR0_FIXED0_MSR);
    cr0_নিচেৰ = x86_rdmsr_নিচেৰ(IA32_VMX_CR0_FIXED1_MSR);
    cr4_উপৰেৰ = x86_rdmsr_নিচেৰ(IA32_VMX_CR4_FIXED0_MSR);
    cr4_নিচেৰ = x86_rdmsr_নিচেৰ(IA32_VMX_CR4_FIXED1_MSR);

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
    যদি (secondary_control_নিচেৰ & BIT(7)) {
        cr0_উপৰেৰ &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    যদি (!(secondary_control_নিচেৰ & BIT(5))) {
        vmx_feature_vpid = 0;
        printf("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } অন্যথা {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    যদি (!(exit_control_নিচেৰ & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        printf("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } অন্যথা {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    যদি (!(exit_control_নিচেৰ & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        printf("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } অন্যথা {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_নিচেৰ) & pin_control_mask;
    যদি (missing) {
        printf("vt-x: Unsupported pin control features %lx\n", (long)missing);
        ঘূৰাই false;
    }
    missing = (~primary_control_নিচেৰ) & primary_control_mask;
    যদি (missing) {
        printf("vt-x: Unsupported primary control features %lx\n", (long)missing);
        ঘূৰাই false;
    }
    missing = (~secondary_control_নিচেৰ) & secondary_control_mask;
    যদি (missing) {
        printf("vt-x: Unsupported secondary control features %lx\n", (long)missing);
        ঘূৰাই false;
    }
    missing = (~exit_control_নিচেৰ) & exit_control_mask;
    যদি (missing) {
        printf("vt-x: Unsupported exit control features %lx\n", (long)missing);
        ঘূৰাই false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_নিচেৰ) & BIT(9);
    যদি (missing) {
        printf("vt-x: CPU does not support 64-bit guests\n");
        ঘূৰাই false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_উপৰেৰ |= pin_control_mask;
    primary_control_উপৰেৰ |= primary_control_mask;
    secondary_control_উপৰেৰ |= secondary_control_mask;
    exit_control_উপৰেৰ |= exit_control_mask;

    ঘূৰাই true;
}

স্থির bool_t BOOT_CODE check_vtx_fixed_মানসমূহ(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    যদি (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } অন্যথা {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    uint32_t local_pin_control_উপৰেৰ = x86_rdmsr_নিচেৰ(pinbased_ctls);
    uint32_t local_pin_control_নিচেৰ = x86_rdmsr_উপৰেৰ(pinbased_ctls);
    uint32_t local_primary_control_উপৰেৰ = x86_rdmsr_নিচেৰ(procbased_ctls);
    uint32_t local_primary_control_নিচেৰ = x86_rdmsr_উপৰেৰ(procbased_ctls);
    uint32_t local_secondary_control_উপৰেৰ = x86_rdmsr_নিচেৰ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_নিচেৰ = x86_rdmsr_উপৰেৰ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_উপৰেৰ = x86_rdmsr_নিচেৰ(exit_ctls);
    uint32_t local_exit_control_নিচেৰ = x86_rdmsr_উপৰেৰ(exit_ctls);
    uint32_t local_entry_control_উপৰেৰ = x86_rdmsr_নিচেৰ(entry_ctls);
    uint32_t local_entry_control_নিচেৰ = x86_rdmsr_উপৰেৰ(entry_ctls);

    uint32_t local_cr0_উপৰেৰ = x86_rdmsr_নিচেৰ(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_নিচেৰ = x86_rdmsr_নিচেৰ(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_উপৰেৰ = x86_rdmsr_নিচেৰ(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_নিচেৰ = x86_rdmsr_নিচেৰ(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    যদি (local_secondary_control_নিচেৰ & BIT(7)) {
        local_cr0_উপৰেৰ &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    ঘূৰাই
        (local_pin_control_উপৰেৰ & pin_control_উপৰেৰ) == local_pin_control_উপৰেৰ &&
        (~local_pin_control_নিচেৰ & pin_control_উপৰেৰ) == 0 &&
        (local_primary_control_উপৰেৰ & primary_control_উপৰেৰ) == local_primary_control_উপৰেৰ &&
        (~local_primary_control_নিচেৰ & primary_control_উপৰেৰ) == 0 &&
        (local_secondary_control_উপৰেৰ & secondary_control_উপৰেৰ) == local_secondary_control_উপৰেৰ &&
        (~local_secondary_control_নিচেৰ & secondary_control_উপৰেৰ) == 0 &&
        (local_exit_control_উপৰেৰ & exit_control_উপৰেৰ) == local_exit_control_উপৰেৰ &&
        (~local_exit_control_নিচেৰ & exit_control_উপৰেৰ) == 0 &&
        (local_entry_control_উপৰেৰ & entry_control_উপৰেৰ) == local_entry_control_উপৰেৰ &&
        (~local_entry_control_নিচেৰ & entry_control_উপৰেৰ) == 0 &&
        local_cr0_উপৰেৰ == cr0_উপৰেৰ &&
        local_cr0_নিচেৰ == cr0_নিচেৰ &&
        local_cr4_উপৰেৰ == cr4_উপৰেৰ &&
        local_cr4_নিচেৰ == cr4_নিচেৰ;
}

স্থির অন্তঃস্থ uint32_t applyFixedBits(uint32_t original, uint32_t উপৰেৰ, uint32_t নিচেৰ)
{
    original |= উপৰেৰ;
    original &= নিচেৰ;
    ঘূৰাই original;
}

শূন্য vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = রিক্ত;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    memcpy(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_উপৰেৰ & cr0_নিচেৰ;
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
    যদি (vmx_feature_load_perf_global_ctrl) {
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
    যদি (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((ৱর্ণ *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.শব্দগুলি[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_register]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_to_paddr(x64KSKernelPML4), 0).শব্দগুলি[0]);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_উপৰেৰ & pin_control_নিচেৰ);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_উপৰেৰ & primary_control_নিচেৰ);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_উপৰেৰ & secondary_control_নিচেৰ);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_উপৰেৰ & exit_control_নিচেৰ);

    uint64_t entry_control = entry_control_উপৰেৰ & entry_control_নিচেৰ;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_উপৰেৰ & cr4_নিচেৰ);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_উপৰেৰ, ~(word_t)0);

    memset(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_SIZE);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((ৱর্ণ *)vcpu->io + (VCPU_IOBITMAP_SIZE / 2)));
}

স্থির শূন্য dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    প্রতিজ্ঞা(tcb->tcbArch.tcbVCPU == vcpu);
    প্রতিজ্ঞা(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = রিক্ত;
    vcpu->vcpuTCB = রিক্ত;
}

শূন্য vcpu_finalise(vcpu_t *vcpu)
{
    যদি (ARCH_গ্ৰন্থি_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
#ifdef ENABLE_SMP_SUPPORT
        যদি (vcpu->last_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->last_cpu);
        } অন্যথা
#endif /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    যদি (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    যদি (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

স্থির শূন্য associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    যদি (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    যদি (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

স্থির exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *buffer)
{
    পূর্ণাঙ্ক i;
    বাবে (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, buffer);
    }
    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t decodeVCPUWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    যদি (length < 15) {
#অন্যথা
    যদি (length < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    ঘূৰাই invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
স্থির exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *buffer, word_t field, word_t মান)
{
    tcb_t *thread;
    thread = গ্ৰন্থি_STATE(ksCurThread);
    যদি (ARCH_গ্ৰন্থি_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ৱিকল্প (field) {
    পক্ষ IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = মান;
        ভঙ্গ;
    পক্ষ IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = মান;
        ভঙ্গ;
    পক্ষ IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = মান;
        ভঙ্গ;
    পক্ষ IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = মান;
        ভঙ্গ;
    }
    setMR(thread, buffer, 0, মান);
    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t decodeVCPUWriteMSR(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t মান;

    যদি (length < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    মান = getSyscallArg(1, buffer);
    ৱিকল্প (field) {
    পক্ষ IA32_LSTAR_MSR:
    পক্ষ IA32_STAR_MSR:
    পক্ষ IA32_CSTAR_MSR:
    পক্ষ IA32_FMASK_MSR:
        ভঙ্গ;
    স্ৱভাৱ:
        userError("VCPU WriteMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    ঘূৰাই invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer, field, মান);
}

স্থির exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *buffer)
{
    tcb_t *thread;
    thread = গ্ৰন্থি_STATE(ksCurThread);

    word_t মান = 0;

    ৱিকল্প (field) {
    পক্ষ IA32_LSTAR_MSR:
        মান = vcpu->syscall_registers[VCPU_LSTAR];
        ভঙ্গ;
    পক্ষ IA32_STAR_MSR:
        মান = vcpu->syscall_registers[VCPU_STAR];
        ভঙ্গ;
    পক্ষ IA32_CSTAR_MSR:
        মান = vcpu->syscall_registers[VCPU_CSTAR];
        ভঙ্গ;
    পক্ষ IA32_FMASK_MSR:
        মান = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        ভঙ্গ;
    পক্ষ IA32_VMX_MISC_MSR:
        মান = x86_rdmsr(field);
        ভঙ্গ;
    }

    setMR(thread, buffer, 0, মান);
    setRegister(thread, বাৰ্তা_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t decodeVCPUReadMSR(cap_t cap, word_t length, word_t *buffer)
{
    যদি (length < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    ৱিকল্প (field) {
    পক্ষ IA32_LSTAR_MSR:
    পক্ষ IA32_STAR_MSR:
    পক্ষ IA32_CSTAR_MSR:
    পক্ষ IA32_FMASK_MSR:
    পক্ষ IA32_VMX_MISC_MSR:
        ভঙ্গ;
    স্ৱভাৱ:
        userError("VCPU ReadMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    ঘূৰাই invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, buffer);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

স্থির exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t নিচেৰ, uint16_t উপৰেৰ)
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
    setIOPortMask(vcpu->io, নিচেৰ, উপৰেৰ, false);
    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t decodeEnableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t নিচেৰ, উপৰেৰ;

    যদি (length < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    যদি (current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    নিচেৰ = getSyscallArg(0, buffer);
    উপৰেৰ = getSyscallArg(1, buffer);

    যদি (নিচেৰ < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || উপৰেৰ > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 0;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
    ঘূৰাই invokeEnableIOPort(vcpu, ioSlot, ioCap, নিচেৰ, উপৰেৰ);
}

স্থির exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t নিচেৰ, uint16_t উপৰেৰ)
{
    setIOPortMask(vcpu->io, নিচেৰ, উপৰেৰ, true);
    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t decodeDisableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    uint16_t নিচেৰ, উপৰেৰ;

    যদি (length < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    নিচেৰ = getSyscallArg(0, buffer);
    উপৰেৰ = getSyscallArg(1, buffer);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ঘূৰাই invokeDisableIOPort(vcpu, নিচেৰ, উপৰেৰ);
}

স্থির exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *buffer, word_t field, word_t মান)
{
    tcb_t *thread;
    thread = গ্ৰন্থি_STATE(ksCurThread);

    যদি (ARCH_গ্ৰন্থি_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ৱিকল্প (field) {
    পক্ষ VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = মান;
        ভঙ্গ;
    পক্ষ VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = মান;
        ভঙ্গ;
    পক্ষ VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = মান;
        ভঙ্গ;
    পক্ষ VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = মান;
        ভঙ্গ;
    }
    vmwrite(field, মান);

    যদি (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned পূর্ণাঙ্ক length = setMR(thread, buffer, 0, মান);
        setRegister(thread, বাৰ্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Running);
    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t decodeWriteVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    word_t মান;

    যদি (length < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    মান = getSyscallArg(1, buffer);
    ৱিকল্প (field) {
    পক্ষ VMX_GUEST_RIP:
    পক্ষ VMX_GUEST_RSP:
    পক্ষ VMX_GUEST_ES_SELECTOR:
    পক্ষ VMX_GUEST_CS_SELECTOR:
    পক্ষ VMX_GUEST_SS_SELECTOR:
    পক্ষ VMX_GUEST_DS_SELECTOR:
    পক্ষ VMX_GUEST_FS_SELECTOR:
    পক্ষ VMX_GUEST_GS_SELECTOR:
    পক্ষ VMX_GUEST_LDTR_SELECTOR:
    পক্ষ VMX_GUEST_TR_SELECTOR:
    পক্ষ VMX_GUEST_DEBUGCTRL:
    পক্ষ VMX_GUEST_PAT:
    পক্ষ VMX_GUEST_EFER:
    পক্ষ VMX_GUEST_PERF_GLOBAL_CTRL:
    পক্ষ VMX_GUEST_PDPTE0:
    পক্ষ VMX_GUEST_PDPTE1:
    পক্ষ VMX_GUEST_PDPTE2:
    পক্ষ VMX_GUEST_PDPTE3:
    পক্ষ VMX_GUEST_ES_LIMIT:
    পক্ষ VMX_GUEST_CS_LIMIT:
    পক্ষ VMX_GUEST_SS_LIMIT:
    পক্ষ VMX_GUEST_DS_LIMIT:
    পক্ষ VMX_GUEST_FS_LIMIT:
    পক্ষ VMX_GUEST_GS_LIMIT:
    পক্ষ VMX_GUEST_LDTR_LIMIT:
    পক্ষ VMX_GUEST_TR_LIMIT:
    পক্ষ VMX_GUEST_GDTR_LIMIT:
    পক্ষ VMX_GUEST_IDTR_LIMIT:
    পক্ষ VMX_GUEST_ES_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_CS_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_SS_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_DS_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_FS_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_GS_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_TR_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_INTERRUPTABILITY:
    পক্ষ VMX_GUEST_ACTIVITY:
    পক্ষ VMX_GUEST_SMBASE:
    পক্ষ VMX_GUEST_SYSENTER_CS:
    পক্ষ VMX_GUEST_PREEMPTION_TIMER_মান:
    পক্ষ VMX_GUEST_ES_BASE:
    পক্ষ VMX_GUEST_CS_BASE:
    পক্ষ VMX_GUEST_SS_BASE:
    পক্ষ VMX_GUEST_DS_BASE:
    পক্ষ VMX_GUEST_FS_BASE:
    পক্ষ VMX_GUEST_GS_BASE:
    পক্ষ VMX_GUEST_LDTR_BASE:
    পক্ষ VMX_GUEST_TR_BASE:
    পক্ষ VMX_GUEST_GDTR_BASE:
    পক্ষ VMX_GUEST_IDTR_BASE:
    পক্ষ VMX_GUEST_DR7:
    পক্ষ VMX_GUEST_RFLAGS:
    পক্ষ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    পক্ষ VMX_GUEST_SYSENTER_ESP:
    পক্ষ VMX_GUEST_SYSENTER_EIP:
    পক্ষ VMX_CONTROL_CR0_MASK:
    পক্ষ VMX_CONTROL_CR4_MASK:
    পক্ষ VMX_CONTROL_CR0_READ_SHADOW:
    পক্ষ VMX_CONTROL_CR4_READ_SHADOW:
    পক্ষ VMX_GUEST_CR3:
    পক্ষ VMX_CONTROL_EXCEPTION_BITMAP:
    পক্ষ VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    পক্ষ VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        ভঙ্গ;
    পক্ষ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        মান = applyFixedBits(মান, pin_control_উপৰেৰ, pin_control_নিচেৰ);
        ভঙ্গ;
    পক্ষ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        মান = applyFixedBits(মান, primary_control_উপৰেৰ, primary_control_নিচেৰ);
        ভঙ্গ;
    পক্ষ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        মান = applyFixedBits(মান, secondary_control_উপৰেৰ, secondary_control_নিচেৰ);
        ভঙ্গ;
    পক্ষ VMX_CONTROL_ENTRY_CONTROLS:
        মান = applyFixedBits(মান, entry_control_উপৰেৰ, entry_control_নিচেৰ);
        ভঙ্গ;
    পক্ষ VMX_CONTROL_EXIT_CONTROLS:
        মান = applyFixedBits(মান, exit_control_উপৰেৰ, exit_control_নিচেৰ);
        ভঙ্গ;
    পক্ষ VMX_GUEST_CR0:
        মান = applyFixedBits(মান, cr0_উপৰেৰ, cr0_নিচেৰ);
        ভঙ্গ;
    পক্ষ VMX_GUEST_CR4:
        মান = applyFixedBits(মান, cr4_উপৰেৰ, cr4_নিচেৰ);
        ভঙ্গ;
    স্ৱভাৱ:
        userError("VCPU WriteVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    ঘূৰাই invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, buffer, field, মান);
}

স্থির word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    ৱিকল্প (field) {
    পক্ষ VMX_CONTROL_EXCEPTION_BITMAP:
        ঘূৰাই vcpu->exception_bitmap;
    পক্ষ VMX_GUEST_CR0:
        ঘূৰাই vcpu->cr0;
    পক্ষ VMX_CONTROL_CR0_MASK:
        ঘূৰাই vcpu->cr0_mask;
    পক্ষ VMX_CONTROL_CR0_READ_SHADOW:
        ঘূৰাই vcpu->cr0_shadow;
    }
    যদি (ARCH_গ্ৰন্থি_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ঘূৰাই vmread(field);
}

স্থির exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *buffer)
{
    tcb_t *thread;
    thread = গ্ৰন্থি_STATE(ksCurThread);
    word_t মান = readVMCSField(vcpu, field);
    যদি (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned পূর্ণাঙ্ক length = setMR(thread, buffer, 0, মান);
        setRegister(thread, বাৰ্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Running);
    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t decodeReadVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    যদি (length < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    ৱিকল্প (field) {
    পক্ষ VMX_GUEST_RIP:
    পক্ষ VMX_GUEST_RSP:
    পক্ষ VMX_GUEST_ES_SELECTOR:
    পক্ষ VMX_GUEST_CS_SELECTOR:
    পক্ষ VMX_GUEST_SS_SELECTOR:
    পক্ষ VMX_GUEST_DS_SELECTOR:
    পক্ষ VMX_GUEST_FS_SELECTOR:
    পক্ষ VMX_GUEST_GS_SELECTOR:
    পক্ষ VMX_GUEST_LDTR_SELECTOR:
    পক্ষ VMX_GUEST_TR_SELECTOR:
    পক্ষ VMX_GUEST_DEBUGCTRL:
    পক্ষ VMX_GUEST_PAT:
    পক্ষ VMX_GUEST_EFER:
    পক্ষ VMX_GUEST_PERF_GLOBAL_CTRL:
    পক্ষ VMX_GUEST_PDPTE0:
    পক্ষ VMX_GUEST_PDPTE1:
    পক্ষ VMX_GUEST_PDPTE2:
    পক্ষ VMX_GUEST_PDPTE3:
    পক্ষ VMX_GUEST_ES_LIMIT:
    পক্ষ VMX_GUEST_CS_LIMIT:
    পক্ষ VMX_GUEST_SS_LIMIT:
    পক্ষ VMX_GUEST_DS_LIMIT:
    পক্ষ VMX_GUEST_FS_LIMIT:
    পক্ষ VMX_GUEST_GS_LIMIT:
    পক্ষ VMX_GUEST_LDTR_LIMIT:
    পক্ষ VMX_GUEST_TR_LIMIT:
    পক্ষ VMX_GUEST_GDTR_LIMIT:
    পক্ষ VMX_GUEST_IDTR_LIMIT:
    পক্ষ VMX_GUEST_ES_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_CS_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_SS_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_DS_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_FS_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_GS_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_TR_ACCESS_RIGHTS:
    পক্ষ VMX_GUEST_INTERRUPTABILITY:
    পক্ষ VMX_GUEST_ACTIVITY:
    পক্ষ VMX_GUEST_SMBASE:
    পক্ষ VMX_GUEST_SYSENTER_CS:
    পক্ষ VMX_GUEST_PREEMPTION_TIMER_মান:
    পক্ষ VMX_GUEST_ES_BASE:
    পক্ষ VMX_GUEST_CS_BASE:
    পক্ষ VMX_GUEST_SS_BASE:
    পক্ষ VMX_GUEST_DS_BASE:
    পক্ষ VMX_GUEST_FS_BASE:
    পক্ষ VMX_GUEST_GS_BASE:
    পক্ষ VMX_GUEST_LDTR_BASE:
    পক্ষ VMX_GUEST_TR_BASE:
    পক্ষ VMX_GUEST_GDTR_BASE:
    পক্ষ VMX_GUEST_IDTR_BASE:
    পক্ষ VMX_GUEST_DR7:
    পক্ষ VMX_GUEST_RFLAGS:
    পক্ষ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    পক্ষ VMX_GUEST_SYSENTER_ESP:
    পক্ষ VMX_GUEST_SYSENTER_EIP:
    পক্ষ VMX_CONTROL_CR0_MASK:
    পক্ষ VMX_CONTROL_CR4_MASK:
    পক্ষ VMX_CONTROL_CR0_READ_SHADOW:
    পক্ষ VMX_CONTROL_CR4_READ_SHADOW:
    পক্ষ VMX_DATA_INSTRUCTION_ERROR:
    পক্ষ VMX_DATA_EXIT_INTERRUPT_INFO:
    পক্ষ VMX_DATA_EXIT_INTERRUPT_ERROR:
    পক্ষ VMX_DATA_IDT_VECTOR_INFO:
    পক্ষ VMX_DATA_IDT_VECTOR_ERROR:
    পক্ষ VMX_DATA_EXIT_INSTRUCTION_LENGTH:
    পক্ষ VMX_DATA_EXIT_INSTRUCTION_INFO:
    পক্ষ VMX_DATA_GUEST_PHYSICAL:
    পক্ষ VMX_DATA_IO_RCX:
    পক্ষ VMX_DATA_IO_RSI:
    পক্ষ VMX_DATA_IO_RDI:
    পক্ষ VMX_DATA_IO_RIP:
    পক্ষ VMX_DATA_GUEST_LINEAR_ADDRESS:
    পক্ষ VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    পক্ষ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    পক্ষ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    পক্ষ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    পক্ষ VMX_CONTROL_EXCEPTION_BITMAP:
    পক্ষ VMX_CONTROL_ENTRY_CONTROLS:
    পক্ষ VMX_CONTROL_EXIT_CONTROLS:
    পক্ষ VMX_GUEST_CR0:
    পক্ষ VMX_GUEST_CR3:
    পক্ষ VMX_GUEST_CR4:
        ভঙ্গ;
    স্ৱভাৱ:
        userError("VCPU ReadVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    ঘূৰাই invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, buffer);
}

স্থির exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t decodeSetTCB(cap_t cap, word_t length, word_t *buffer)
{
    cap_t tcbCap;
    যদি (current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    ঘূৰাই invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

শূন্য vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *buffer;
    যদি (ARCH_গ্ৰন্থি_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    buffer = lookupIPCBuffer(false, গ্ৰন্থি_STATE(ksCurThread));
    যদি (!buffer) {
        userError("No IPC buffer.");
        ঘূৰাই;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, buffer));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, buffer), primary_control_উপৰেৰ,
                                                                   primary_control_নিচেৰ));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, buffer));
}

শূন্য vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *buffer;
    vcpu_t *vcpu;

    buffer = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    প্রতিজ্ঞা(vcpu);

    যদি (ARCH_গ্ৰন্থি_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, বাৰ্তা_Info_Register, 0);
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
    ৱিকল্প (invLabel) {
    পক্ষ X86VCPUSetTCB:
        ঘূৰাই decodeSetTCB(cap, length, buffer);
    পক্ষ X86VCPUReadVMCS:
        ঘূৰাই decodeReadVMCS(cap, length, call, buffer);
    পক্ষ X86VCPUWriteVMCS:
        ঘূৰাই decodeWriteVMCS(cap, length, call, buffer);
    পক্ষ X86VCPUEnableIOPort:
        ঘূৰাই decodeEnableIOPort(cap, length, buffer);
    পক্ষ X86VCPUDisableIOPort:
        ঘূৰাই decodeDisableIOPort(cap, length, buffer);
    পক্ষ X86VCPUWriteRegisters:
        ঘূৰাই decodeVCPUWriteRegisters(cap, length, buffer);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    পক্ষ X86VCPUWriteMSR:
        ঘূৰাই decodeVCPUWriteMSR(cap, length, buffer);
    পক্ষ X86VCPUReadMSR:
        ঘূৰাই decodeVCPUReadMSR(cap, length, buffer);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    স্ৱভাৱ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
}

স্থির bool_t is_vtx_supported(শূন্য)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    ঘূৰাই !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

স্থির অন্তঃস্থ শূন্য clear_bit(word_t *bitmap, word_t bit)
{
    পূর্ণাঙ্ক index = bit / (পরিমাণ(word_t) * 8);
    পূর্ণাঙ্ক offset = bit % (পরিমাণ(word_t) * 8);
    bitmap[index] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(শূন্য)
{
    যদি (!is_vtx_supported()) {
        printf("vt-x: not supported\n");
        ঘূৰাই false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.শব্দগুলি[0] = x86_rdmsr_নিচেৰ(IA32_VMX_BASIC_MSR);
    vmx_basic.শব্দগুলি[1] = x86_rdmsr_উপৰেৰ(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.শব্দগুলি[0] = x86_rdmsr_নিচেৰ(IA32_FEATURE_CONTROL_MSR);
    যদি (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        যদি (feature_control_msr_get_lock(feature_control)) {
            printf("vt-x: feature locked\n");
            ঘূৰাই false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_উপৰেৰ(IA32_FEATURE_CONTROL_MSR), feature_control.শব্দগুলি[0]);
    }
    /* make sure the msr is locked */
    যদি (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_উপৰেৰ(IA32_FEATURE_CONTROL_MSR), feature_control.শব্দগুলি[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    যদি (CURRENT_CPU_INDEX() == 0) {
        যদি (!init_vtx_fixed_মানসমূহ(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            printf("vt-x: lack of required features\n");
            ঘূৰাই false;
        }
    }
    যদি (!check_vtx_fixed_মানসমূহ(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        printf("vt-x: cores have inconsistent features\n");
        ঘূৰাই false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    সংরচনা vmxon_region *vmxon_kptr = &ARCH_গ্ৰন্থি_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    যদি (!vtx_check_fixed_মানসমূহ(read_cr0(), read_cr4())) {
        ঘূৰাই false;
    }
    যদি (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        printf("vt-x: vmxon failure\n");
        ঘূৰাই false;
    }
    memset(&msr_bitmap_region, ~0, পরিমাণ(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.নিচেৰ_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.নিচেৰ_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.নিচেৰ_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.নিচেৰ_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.নিচেৰ_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.নিচেৰ_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.উপৰেৰ_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.উপৰেৰ_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.উপৰেৰ_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.উপৰেৰ_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.উপৰেৰ_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.উপৰেৰ_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.শব্দগুলি[0] = x86_rdmsr_নিচেৰ(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.শব্দগুলি[1] = x86_rdmsr_উপৰেৰ(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    যদি (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        printf("vt-x: Expected wb attribute for EPT paging structure\n");
        ঘূৰাই false;
    }
    যদি (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        printf("vt-x: Expected supported for 2m pages\n");
        ঘূৰাই false;
    }

    ঘূৰাই true;
}

স্থির শূন্য setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *buffer;
    পূর্ণাঙ্ক i;

    buffer = lookupIPCBuffer(true, গ্ৰন্থি_STATE(ksCurThread));

    setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_DATA_EXIT_INSTRUCTION_LENGTH));
    setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_DATA_GUEST_PHYSICAL));
    setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    বাবে (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(গ্ৰন্থি_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_EAX + i,
              গ্ৰন্থি_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

স্থির শূন্য handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(গ্ৰন্থি_STATE(ksCurThread), বাৰ্তা_Info_Register, SEL4_VMENTER_RESULT_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

স্থির অন্তঃস্থ শূন্য finishVmexitSaving(শূন্য)
{
    vcpu_t *vcpu = ARCH_গ্ৰন্থি_STATE(x86KSCurrentVCPU);
    প্রতিজ্ঞা(vcpu == গ্ৰন্থি_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(শূন্য)
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
    যদি (reason == EXTERNAL_INTERRUPT) {
        গ্ৰন্থি_LOCK_IRQ;
        যদি (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_DATA_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_গ্ৰন্থি_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } অন্যথা {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(গ্ৰন্থি_STATE(ksCurThread));
#endif
        ঘূৰাই EXCEPTION_NONE;
    }

    গ্ৰন্থি_LOCK_SYS;

    ৱিকল্প (reason) {
    পক্ষ EXCEPTION_OR_NMI:
    পক্ষ MOV_DR:
    পক্ষ TASK_SWITCH:
    পক্ষ CONTROL_REGISTER:
    পক্ষ IO:
    পক্ষ MWAIT:
    পক্ষ SIPI:
    পক্ষ INVLPG:
    পক্ষ INVEPT:
    পক্ষ INVVPID:
    পক্ষ VMCLEAR:
    পক্ষ VMPTRLD:
    পক্ষ VMPTRST:
    পক্ষ VMREAD:
    পক্ষ VMWRITE:
    পক্ষ VMXON:
    পক্ষ EPT_VIOLATION:
    পক্ষ GDTR_OR_IDTR:
    পক্ষ LDTR_OR_TR:
    পক্ষ TPR_BELOW_THRESHOLD:
    পক্ষ APIC_ACCESS:
        qualification = vmread(VMX_DATA_EXIT_QUALIFICATION);
        ভঙ্গ;
    স্ৱভাৱ:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    ঘূৰাই EXCEPTION_NONE;
}

exception_t handleVmEntryFail(শূন্য)
{
    handleVmxFault(-1, -1);

    ঘূৰাই EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
শূন্য VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    প্রতিজ্ঞা(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    যদি (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {

        word_t badge = notification_ptr_get_ntfn_বাৰ্তা_Identifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        যদি (tcb != গ্ৰন্থি_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

স্থির শূন্য invvpid_context(uint16_t vpid)
{
    সংরচনা {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    asm চঞ্চল("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
}

স্থির শূন্য setEPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_মূল;
    যদি (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_মূল = kpptr_to_paddr(null_ept_space);
    } অন্যথা {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        যদি (find_ret.status != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_মূল = kpptr_to_paddr(null_ept_space);
        } অন্যথা {
            ept_মূল = pptr_to_paddr(pml4);
        }
    }
    যদি (ept_মূল != vcpu->last_ept_মূল) {
        vcpu->last_ept_মূল = ept_মূল;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_মূল,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.শব্দগুলি[0]);
        প্রতিজ্ঞা(vcpu->vpid != VPID_INVALID);
        যদি (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

শূন্য clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t last)
{
    যদি (vpid == VPID_INVALID) {
        ঘূৰাই;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    যদি (vcpu == রিক্ত) {
        ঘূৰাই;
    }
    প্রতিজ্ঞা(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

স্থির অন্তঃস্থ vpid_t nextVPID(vpid_t vpid)
{
    যদি (vpid == VPID_LAST) {
        ঘূৰাই VPID_FIRST;
    } অন্যথা {
        ঘূৰাই vpid + 1;
    }
}

স্থির শূন্য invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, পরিমাণ(vcpu->io));
    /* invalidate the VPID context */
    যদি (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
স্থির শূন্য releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = রিক্ত;
}

স্থির vpid_t findFreeVPID(শূন্য)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    কৰা {
        যদি (x86KSVPIDTable[vpid] == রিক্ত) {
            ঘূৰাই vpid;
        }
        vpid = nextVPID(vpid);
    } যেতিয়ালৈ (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = nextVPID(x86KSNextVPID);
    ঘূৰাই vpid;
}

স্থির শূন্য storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    প্রতিজ্ঞা(x86KSVPIDTable[vpid] == রিক্ত);
    প্রতিজ্ঞা(vcpu->vpid == VPID_INVALID);
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
শূন্য vcpu_fpu_to_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    যদি (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        যদি (!vcpu->fpu_active) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_active = true;
            loadFpuState(tcb);
        }
    } অন্যথা {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_active = true;
        switchLocalFpuOwner(tcb);
    }
}

শূন্য vcpu_fpu_to_host(tcb_t *tcb, vcpu_t *vcpu)
{
    যদি (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } অন্যথা যদি (nativeThreadUsingFPU(tcb) && vcpu->fpu_active) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_active = false;
        loadFpuState(tcb);
    } অন্যথা যদি (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

শূন্য restoreVMCS(শূন্য)
{
    tcb_t *cur_thread = গ্ৰন্থি_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    যদি (ARCH_গ্ৰন্থি_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ifndef CONFIG_KERNEL_SKIM_WINDOW
    যদি (getCurrentCR3().শব্দগুলি[0] != expected_vmcs->last_host_cr3) {
        expected_vmcs->last_host_cr3 = getCurrentCR3().শব্দগুলি[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().শব্দগুলি[0]);
    }
#endif
    যদি (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        যদি (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    setEPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

শূন্য invept(ept_pml4e_t *ept_pml4)
{
    যদি (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        সংরচনা {
            uint64_t parts[2];
        } address;
        word_t type;
        যদি (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } অন্যথা যদি (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } অন্যথা {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            ঘূৰাই;
        }

        address.parts[0] = pptr_to_paddr((শূন্য *)ept_pml4);
        address.parts[1] = 0;
        asm চঞ্চল(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
