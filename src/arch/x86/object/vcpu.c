/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ifdef CONFIG_VTX

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/cpu_registers.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <arch/object/ioport.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <sel4/arch/vmenter.h>

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

ಧ್ರುವ vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ msr_bitmap {
    word_t bitmap[0x2000 / ಪರಿಮಾಣ(word_t) / 8];
} msr_bitmap_t;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ msr_bitmaps {
    msr_bitmap_t ಕೆಳಗಿನ_msr_read;
    msr_bitmap_t ಮೇಲಿನ_msr_read;
    msr_bitmap_t ಕೆಳಗಿನ_msr_write;
    msr_bitmap_t ಮೇಲಿನ_msr_write;
} msr_bitmaps_t;

ಸ್ಥಿರ msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

ಸ್ಥಿರ ವರ್ಣ null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
ಸ್ಥಿರ uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
ಸ್ಥಿರ vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
ಸ್ಥಿರ uint32_t pin_control_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t pin_control_ಕೆಳಗಿನ;
ಸ್ಥಿರ uint32_t primary_control_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t primary_control_ಕೆಳಗಿನ;
ಸ್ಥಿರ uint32_t secondary_control_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t secondary_control_ಕೆಳಗಿನ;
ಸ್ಥಿರ uint32_t entry_control_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t entry_control_ಕೆಳಗಿನ;
ಸ್ಥಿರ uint32_t exit_control_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t exit_control_ಕೆಳಗಿನ;
ಸ್ಥಿರ uint32_t cr0_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t cr0_ಕೆಳಗಿನ;
ಸ್ಥಿರ uint32_t cr4_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t cr4_ಕೆಳಗಿನ;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
ಸ್ಥಿರ bool_t vmx_feature_vpid;
ಸ್ಥಿರ bool_t vmx_feature_load_perf_global_ctrl;
ಸ್ಥಿರ bool_t vmx_feature_ack_on_exit;

ಸ್ಥಿರ vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
ಸ್ಥಿರ vpid_t x86KSNextVPID = VPID_FIRST;

ಸ್ಥಿರ ಶೂನ್ಯ releaseVPID(vpid_t vpid);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm ಚಞ್ಚಲ(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    ಪ್ರತ್ಯಾಗಮ !!error;
}

ಸ್ಥಿರ ಶೂನ್ಯ vmclear(ಶೂನ್ಯ *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((ಶೂನ್ಯ *)vmcs_ptr);
    asm ಚಞ್ಚಲ(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

ಶೂನ್ಯ clearCurrentVCPU(ಶೂನ್ಯ)
{
    vcpu_t *vcpu = ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU);
    ಯದಿ (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) = ರಿಕ್ತ;
        ಯದಿ (vcpu->fpu_active && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_active = false;
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ vmptrld(ಶೂನ್ಯ *vmcs_ptr)
{
    uint64_t physical_address;
    uint8_t error;
    physical_address = pptr_to_paddr(vmcs_ptr);
    asm ಚಞ್ಚಲ(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_address)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    ಪ್ರತಿಜ್ಞಾ(!error);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
ಶೂನ್ಯ vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_STAR_MSR, vcpu->syscall_registers[VCPU_STAR]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_MASK]);
}

ಶೂನ್ಯ vcpu_restore_host_msrs(ಶೂನ್ಯ)
{
    init_syscall_msrs();
}
#endif

ಸ್ಥಿರ ಶೂನ್ಯ switchVCPU(vcpu_t *vcpu)
{
#ifdef ENABLE_SMP_SUPPORT
    ಯದಿ (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_ಗಂಟು_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    ಯದಿ (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((ವರ್ಣ *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.ಪದಗಳು[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) = vcpu;
}

ಸ್ಥಿರ ಶೂನ್ಯ print_bits(word_t bits)
{
    bool_t first = true;
    ಯಾವತ್ (bits) {
        ಪೂರ್ಣಾಙ್ಕ index = seL4_WordBits - 1 - clzl(bits);
        ಯದಿ (first) {
            printf("%d", index);
            first = false;
        } ಅನ್ಯಥಾ {
            printf(",%d", index);
        }
        bits &= ~BIT(index);
    }
}

ಸ್ಥಿರ bool_t check_fixed_ಮೌಲ್ಯ(word_t val, word_t ಕೆಳಗಿನ, word_t ಮೇಲಿನ)
{
    word_t not_ಮೇಲಿನ;
    word_t not_ಕೆಳಗಿನ;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_ಮೇಲಿನ = ಮೇಲಿನ & ~(ಮೇಲಿನ & val);
    ಯದಿ (not_ಮೇಲಿನ != 0) {
        printf("Failed to set bits: ");
        print_bits(not_ಮೇಲಿನ);
        ಪ್ರತ್ಯಾಗಮ false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_ಕೆಳಗಿನ = ~ಕೆಳಗಿನ & ~(~ಕೆಳಗಿನ & ~val);
    ಯದಿ (not_ಕೆಳಗಿನ != 0) {
        printf("Incorrectly cleared bits: ");
        print_bits(not_ಕೆಳಗಿನ);
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ bool_t vtx_check_fixed_ಮೌಲ್ಯಗಳು(word_t cr0, word_t cr4)
{
    ಯದಿ (!check_fixed_ಮೌಲ್ಯ(cr0, cr0_ಕೆಳಗಿನ, cr0_ಮೇಲಿನ)) {
        printf(" of CR0\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಯದಿ (!check_fixed_ಮೌಲ್ಯ(cr4, cr4_ಕೆಳಗಿನ, cr4_ಮೇಲಿನ)) {
        printf(" of CR4\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ bool_t BOOT_CODE init_vtx_fixed_ಮೌಲ್ಯಗಳು(bool_t useTrueMsrs)
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
    ಯದಿ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } ಅನ್ಯಥಾ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    pin_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(pinbased_ctls);
    pin_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(pinbased_ctls);
    primary_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(procbased_ctls);
    primary_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(procbased_ctls);
    secondary_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(exit_ctls);
    exit_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(exit_ctls);
    entry_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(entry_ctls);
    entry_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(entry_ctls);

    cr0_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR0_FIXED0_MSR);
    cr0_ಕೆಳಗಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR0_FIXED1_MSR);
    cr4_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR4_FIXED0_MSR);
    cr4_ಕೆಳಗಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR4_FIXED1_MSR);

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
    ಯದಿ (secondary_control_ಕೆಳಗಿನ & BIT(7)) {
        cr0_ಮೇಲಿನ &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    ಯದಿ (!(secondary_control_ಕೆಳಗಿನ & BIT(5))) {
        vmx_feature_vpid = 0;
        printf("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } ಅನ್ಯಥಾ {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    ಯದಿ (!(exit_control_ಕೆಳಗಿನ & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        printf("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } ಅನ್ಯಥಾ {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    ಯದಿ (!(exit_control_ಕೆಳಗಿನ & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        printf("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } ಅನ್ಯಥಾ {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_ಕೆಳಗಿನ) & pin_control_mask;
    ಯದಿ (missing) {
        printf("vt-x: Unsupported pin control features %lx\n", (long)missing);
        ಪ್ರತ್ಯಾಗಮ false;
    }
    missing = (~primary_control_ಕೆಳಗಿನ) & primary_control_mask;
    ಯದಿ (missing) {
        printf("vt-x: Unsupported primary control features %lx\n", (long)missing);
        ಪ್ರತ್ಯಾಗಮ false;
    }
    missing = (~secondary_control_ಕೆಳಗಿನ) & secondary_control_mask;
    ಯದಿ (missing) {
        printf("vt-x: Unsupported secondary control features %lx\n", (long)missing);
        ಪ್ರತ್ಯಾಗಮ false;
    }
    missing = (~exit_control_ಕೆಳಗಿನ) & exit_control_mask;
    ಯದಿ (missing) {
        printf("vt-x: Unsupported exit control features %lx\n", (long)missing);
        ಪ್ರತ್ಯಾಗಮ false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_ಕೆಳಗಿನ) & BIT(9);
    ಯದಿ (missing) {
        printf("vt-x: CPU does not support 64-bit guests\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_ಮೇಲಿನ |= pin_control_mask;
    primary_control_ಮೇಲಿನ |= primary_control_mask;
    secondary_control_ಮೇಲಿನ |= secondary_control_mask;
    exit_control_ಮೇಲಿನ |= exit_control_mask;

    ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ bool_t BOOT_CODE check_vtx_fixed_ಮೌಲ್ಯಗಳು(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    ಯದಿ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } ಅನ್ಯಥಾ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    uint32_t local_pin_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(pinbased_ctls);
    uint32_t local_pin_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(pinbased_ctls);
    uint32_t local_primary_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(procbased_ctls);
    uint32_t local_primary_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(procbased_ctls);
    uint32_t local_secondary_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(exit_ctls);
    uint32_t local_exit_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(exit_ctls);
    uint32_t local_entry_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(entry_ctls);
    uint32_t local_entry_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(entry_ctls);

    uint32_t local_cr0_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_ಕೆಳಗಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_ಕೆಳಗಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    ಯದಿ (local_secondary_control_ಕೆಳಗಿನ & BIT(7)) {
        local_cr0_ಮೇಲಿನ &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    ಪ್ರತ್ಯಾಗಮ
        (local_pin_control_ಮೇಲಿನ & pin_control_ಮೇಲಿನ) == local_pin_control_ಮೇಲಿನ &&
        (~local_pin_control_ಕೆಳಗಿನ & pin_control_ಮೇಲಿನ) == 0 &&
        (local_primary_control_ಮೇಲಿನ & primary_control_ಮೇಲಿನ) == local_primary_control_ಮೇಲಿನ &&
        (~local_primary_control_ಕೆಳಗಿನ & primary_control_ಮೇಲಿನ) == 0 &&
        (local_secondary_control_ಮೇಲಿನ & secondary_control_ಮೇಲಿನ) == local_secondary_control_ಮೇಲಿನ &&
        (~local_secondary_control_ಕೆಳಗಿನ & secondary_control_ಮೇಲಿನ) == 0 &&
        (local_exit_control_ಮೇಲಿನ & exit_control_ಮೇಲಿನ) == local_exit_control_ಮೇಲಿನ &&
        (~local_exit_control_ಕೆಳಗಿನ & exit_control_ಮೇಲಿನ) == 0 &&
        (local_entry_control_ಮೇಲಿನ & entry_control_ಮೇಲಿನ) == local_entry_control_ಮೇಲಿನ &&
        (~local_entry_control_ಕೆಳಗಿನ & entry_control_ಮೇಲಿನ) == 0 &&
        local_cr0_ಮೇಲಿನ == cr0_ಮೇಲಿನ &&
        local_cr0_ಕೆಳಗಿನ == cr0_ಕೆಳಗಿನ &&
        local_cr4_ಮೇಲಿನ == cr4_ಮೇಲಿನ &&
        local_cr4_ಕೆಳಗಿನ == cr4_ಕೆಳಗಿನ;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint32_t applyFixedBits(uint32_t original, uint32_t ಮೇಲಿನ, uint32_t ಕೆಳಗಿನ)
{
    original |= ಮೇಲಿನ;
    original &= ಕೆಳಗಿನ;
    ಪ್ರತ್ಯಾಗಮ original;
}

ಶೂನ್ಯ vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = ರಿಕ್ತ;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    memcpy(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_ಮೇಲಿನ & cr0_ಕೆಳಗಿನ;
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
    ಯದಿ (vmx_feature_load_perf_global_ctrl) {
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
    ಯದಿ (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((ವರ್ಣ *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.ಪದಗಳು[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_register]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_to_paddr(x64KSKernelPML4), 0).ಪದಗಳು[0]);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_ಮೇಲಿನ & pin_control_ಕೆಳಗಿನ);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_ಮೇಲಿನ & primary_control_ಕೆಳಗಿನ);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_ಮೇಲಿನ & secondary_control_ಕೆಳಗಿನ);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_ಮೇಲಿನ & exit_control_ಕೆಳಗಿನ);

    uint64_t entry_control = entry_control_ಮೇಲಿನ & entry_control_ಕೆಳಗಿನ;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_ಮೇಲಿನ & cr4_ಕೆಳಗಿನ);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_ಮೇಲಿನ, ~(word_t)0);

    memset(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_SIZE);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((ವರ್ಣ *)vcpu->io + (VCPU_IOBITMAP_SIZE / 2)));
}

ಸ್ಥಿರ ಶೂನ್ಯ dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ಪ್ರತಿಜ್ಞಾ(tcb->tcbArch.tcbVCPU == vcpu);
    ಪ್ರತಿಜ್ಞಾ(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = ರಿಕ್ತ;
    vcpu->vcpuTCB = ರಿಕ್ತ;
}

ಶೂನ್ಯ vcpu_finalise(vcpu_t *vcpu)
{
    ಯದಿ (ARCH_ಗಂಟು_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
#ifdef ENABLE_SMP_SUPPORT
        ಯದಿ (vcpu->last_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->last_cpu);
        } ಅನ್ಯಥಾ
#endif /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    ಯದಿ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    ಯದಿ (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ಯದಿ (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    ಯದಿ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

ಸ್ಥಿರ exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *buffer)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪ್ರತಿ (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, buffer);
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeVCPUWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    ಯದಿ (length < 15) {
#ಅನ್ಯಥಾ
    ಯದಿ (length < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
ಸ್ಥಿರ exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *buffer, word_t field, word_t ಮೌಲ್ಯ)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    ಯದಿ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ವಿಕಲ್ಪ (field) {
    ಪಕ್ಷ IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = ಮೌಲ್ಯ;
        ಭಙ್ಗ;
    ಪಕ್ಷ IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = ಮೌಲ್ಯ;
        ಭಙ್ಗ;
    ಪಕ್ಷ IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = ಮೌಲ್ಯ;
        ಭಙ್ಗ;
    ಪಕ್ಷ IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = ಮೌಲ್ಯ;
        ಭಙ್ಗ;
    }
    setMR(thread, buffer, 0, ಮೌಲ್ಯ);
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeVCPUWriteMSR(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t ಮೌಲ್ಯ;

    ಯದಿ (length < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    ಮೌಲ್ಯ = getSyscallArg(1, buffer);
    ವಿಕಲ್ಪ (field) {
    ಪಕ್ಷ IA32_LSTAR_MSR:
    ಪಕ್ಷ IA32_STAR_MSR:
    ಪಕ್ಷ IA32_CSTAR_MSR:
    ಪಕ್ಷ IA32_FMASK_MSR:
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        userError("VCPU WriteMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer, field, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *buffer)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);

    word_t ಮೌಲ್ಯ = 0;

    ವಿಕಲ್ಪ (field) {
    ಪಕ್ಷ IA32_LSTAR_MSR:
        ಮೌಲ್ಯ = vcpu->syscall_registers[VCPU_LSTAR];
        ಭಙ್ಗ;
    ಪಕ್ಷ IA32_STAR_MSR:
        ಮೌಲ್ಯ = vcpu->syscall_registers[VCPU_STAR];
        ಭಙ್ಗ;
    ಪಕ್ಷ IA32_CSTAR_MSR:
        ಮೌಲ್ಯ = vcpu->syscall_registers[VCPU_CSTAR];
        ಭಙ್ಗ;
    ಪಕ್ಷ IA32_FMASK_MSR:
        ಮೌಲ್ಯ = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        ಭಙ್ಗ;
    ಪಕ್ಷ IA32_VMX_MISC_MSR:
        ಮೌಲ್ಯ = x86_rdmsr(field);
        ಭಙ್ಗ;
    }

    setMR(thread, buffer, 0, ಮೌಲ್ಯ);
    setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeVCPUReadMSR(cap_t cap, word_t length, word_t *buffer)
{
    ಯದಿ (length < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    ವಿಕಲ್ಪ (field) {
    ಪಕ್ಷ IA32_LSTAR_MSR:
    ಪಕ್ಷ IA32_STAR_MSR:
    ಪಕ್ಷ IA32_CSTAR_MSR:
    ಪಕ್ಷ IA32_FMASK_MSR:
    ಪಕ್ಷ IA32_VMX_MISC_MSR:
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        userError("VCPU ReadMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, buffer);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

ಸ್ಥಿರ exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t ಕೆಳಗಿನ, uint16_t ಮೇಲಿನ)
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
    setIOPortMask(vcpu->io, ಕೆಳಗಿನ, ಮೇಲಿನ, false);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeEnableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t ಕೆಳಗಿನ, ಮೇಲಿನ;

    ಯದಿ (length < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಕೆಳಗಿನ = getSyscallArg(0, buffer);
    ಮೇಲಿನ = getSyscallArg(1, buffer);

    ಯದಿ (ಕೆಳಗಿನ < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || ಮೇಲಿನ > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeEnableIOPort(vcpu, ioSlot, ioCap, ಕೆಳಗಿನ, ಮೇಲಿನ);
}

ಸ್ಥಿರ exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t ಕೆಳಗಿನ, uint16_t ಮೇಲಿನ)
{
    setIOPortMask(vcpu->io, ಕೆಳಗಿನ, ಮೇಲಿನ, true);
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeDisableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    uint16_t ಕೆಳಗಿನ, ಮೇಲಿನ;

    ಯದಿ (length < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಕೆಳಗಿನ = getSyscallArg(0, buffer);
    ಮೇಲಿನ = getSyscallArg(1, buffer);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ಪ್ರತ್ಯಾಗಮ invokeDisableIOPort(vcpu, ಕೆಳಗಿನ, ಮೇಲಿನ);
}

ಸ್ಥಿರ exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *buffer, word_t field, word_t ಮೌಲ್ಯ)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);

    ಯದಿ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ವಿಕಲ್ಪ (field) {
    ಪಕ್ಷ VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = ಮೌಲ್ಯ;
        ಭಙ್ಗ;
    ಪಕ್ಷ VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = ಮೌಲ್ಯ;
        ಭಙ್ಗ;
    ಪಕ್ಷ VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = ಮೌಲ್ಯ;
        ಭಙ್ಗ;
    ಪಕ್ಷ VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = ಮೌಲ್ಯ;
        ಭಙ್ಗ;
    }
    vmwrite(field, ಮೌಲ್ಯ);

    ಯದಿ (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned ಪೂರ್ಣಾಙ್ಕ length = setMR(thread, buffer, 0, ಮೌಲ್ಯ);
        setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeWriteVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    word_t ಮೌಲ್ಯ;

    ಯದಿ (length < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    ಮೌಲ್ಯ = getSyscallArg(1, buffer);
    ವಿಕಲ್ಪ (field) {
    ಪಕ್ಷ VMX_GUEST_RIP:
    ಪಕ್ಷ VMX_GUEST_RSP:
    ಪಕ್ಷ VMX_GUEST_ES_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_CS_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_SS_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_DS_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_FS_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_GS_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_LDTR_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_TR_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_DEBUGCTRL:
    ಪಕ್ಷ VMX_GUEST_PAT:
    ಪಕ್ಷ VMX_GUEST_EFER:
    ಪಕ್ಷ VMX_GUEST_PERF_GLOBAL_CTRL:
    ಪಕ್ಷ VMX_GUEST_PDPTE0:
    ಪಕ್ಷ VMX_GUEST_PDPTE1:
    ಪಕ್ಷ VMX_GUEST_PDPTE2:
    ಪಕ್ಷ VMX_GUEST_PDPTE3:
    ಪಕ್ಷ VMX_GUEST_ES_LIMIT:
    ಪಕ್ಷ VMX_GUEST_CS_LIMIT:
    ಪಕ್ಷ VMX_GUEST_SS_LIMIT:
    ಪಕ್ಷ VMX_GUEST_DS_LIMIT:
    ಪಕ್ಷ VMX_GUEST_FS_LIMIT:
    ಪಕ್ಷ VMX_GUEST_GS_LIMIT:
    ಪಕ್ಷ VMX_GUEST_LDTR_LIMIT:
    ಪಕ್ಷ VMX_GUEST_TR_LIMIT:
    ಪಕ್ಷ VMX_GUEST_GDTR_LIMIT:
    ಪಕ್ಷ VMX_GUEST_IDTR_LIMIT:
    ಪಕ್ಷ VMX_GUEST_ES_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_CS_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_SS_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_DS_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_FS_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_GS_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_TR_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_INTERRUPTABILITY:
    ಪಕ್ಷ VMX_GUEST_ACTIVITY:
    ಪಕ್ಷ VMX_GUEST_SMBASE:
    ಪಕ್ಷ VMX_GUEST_SYSENTER_CS:
    ಪಕ್ಷ VMX_GUEST_PREEMPTION_TIMER_ಮೌಲ್ಯ:
    ಪಕ್ಷ VMX_GUEST_ES_BASE:
    ಪಕ್ಷ VMX_GUEST_CS_BASE:
    ಪಕ್ಷ VMX_GUEST_SS_BASE:
    ಪಕ್ಷ VMX_GUEST_DS_BASE:
    ಪಕ್ಷ VMX_GUEST_FS_BASE:
    ಪಕ್ಷ VMX_GUEST_GS_BASE:
    ಪಕ್ಷ VMX_GUEST_LDTR_BASE:
    ಪಕ್ಷ VMX_GUEST_TR_BASE:
    ಪಕ್ಷ VMX_GUEST_GDTR_BASE:
    ಪಕ್ಷ VMX_GUEST_IDTR_BASE:
    ಪಕ್ಷ VMX_GUEST_DR7:
    ಪಕ್ಷ VMX_GUEST_RFLAGS:
    ಪಕ್ಷ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    ಪಕ್ಷ VMX_GUEST_SYSENTER_ESP:
    ಪಕ್ಷ VMX_GUEST_SYSENTER_EIP:
    ಪಕ್ಷ VMX_CONTROL_CR0_MASK:
    ಪಕ್ಷ VMX_CONTROL_CR4_MASK:
    ಪಕ್ಷ VMX_CONTROL_CR0_READ_SHADOW:
    ಪಕ್ಷ VMX_CONTROL_CR4_READ_SHADOW:
    ಪಕ್ಷ VMX_GUEST_CR3:
    ಪಕ್ಷ VMX_CONTROL_EXCEPTION_BITMAP:
    ಪಕ್ಷ VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    ಪಕ್ಷ VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        ಭಙ್ಗ;
    ಪಕ್ಷ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        ಮೌಲ್ಯ = applyFixedBits(ಮೌಲ್ಯ, pin_control_ಮೇಲಿನ, pin_control_ಕೆಳಗಿನ);
        ಭಙ್ಗ;
    ಪಕ್ಷ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        ಮೌಲ್ಯ = applyFixedBits(ಮೌಲ್ಯ, primary_control_ಮೇಲಿನ, primary_control_ಕೆಳಗಿನ);
        ಭಙ್ಗ;
    ಪಕ್ಷ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        ಮೌಲ್ಯ = applyFixedBits(ಮೌಲ್ಯ, secondary_control_ಮೇಲಿನ, secondary_control_ಕೆಳಗಿನ);
        ಭಙ್ಗ;
    ಪಕ್ಷ VMX_CONTROL_ENTRY_CONTROLS:
        ಮೌಲ್ಯ = applyFixedBits(ಮೌಲ್ಯ, entry_control_ಮೇಲಿನ, entry_control_ಕೆಳಗಿನ);
        ಭಙ್ಗ;
    ಪಕ್ಷ VMX_CONTROL_EXIT_CONTROLS:
        ಮೌಲ್ಯ = applyFixedBits(ಮೌಲ್ಯ, exit_control_ಮೇಲಿನ, exit_control_ಕೆಳಗಿನ);
        ಭಙ್ಗ;
    ಪಕ್ಷ VMX_GUEST_CR0:
        ಮೌಲ್ಯ = applyFixedBits(ಮೌಲ್ಯ, cr0_ಮೇಲಿನ, cr0_ಕೆಳಗಿನ);
        ಭಙ್ಗ;
    ಪಕ್ಷ VMX_GUEST_CR4:
        ಮೌಲ್ಯ = applyFixedBits(ಮೌಲ್ಯ, cr4_ಮೇಲಿನ, cr4_ಕೆಳಗಿನ);
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        userError("VCPU WriteVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, buffer, field, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    ವಿಕಲ್ಪ (field) {
    ಪಕ್ಷ VMX_CONTROL_EXCEPTION_BITMAP:
        ಪ್ರತ್ಯಾಗಮ vcpu->exception_bitmap;
    ಪಕ್ಷ VMX_GUEST_CR0:
        ಪ್ರತ್ಯಾಗಮ vcpu->cr0;
    ಪಕ್ಷ VMX_CONTROL_CR0_MASK:
        ಪ್ರತ್ಯಾಗಮ vcpu->cr0_mask;
    ಪಕ್ಷ VMX_CONTROL_CR0_READ_SHADOW:
        ಪ್ರತ್ಯಾಗಮ vcpu->cr0_shadow;
    }
    ಯದಿ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ಪ್ರತ್ಯಾಗಮ vmread(field);
}

ಸ್ಥಿರ exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *buffer)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    word_t ಮೌಲ್ಯ = readVMCSField(vcpu, field);
    ಯದಿ (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned ಪೂರ್ಣಾಙ್ಕ length = setMR(thread, buffer, 0, ಮೌಲ್ಯ);
        setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeReadVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    ಯದಿ (length < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    ವಿಕಲ್ಪ (field) {
    ಪಕ್ಷ VMX_GUEST_RIP:
    ಪಕ್ಷ VMX_GUEST_RSP:
    ಪಕ್ಷ VMX_GUEST_ES_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_CS_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_SS_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_DS_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_FS_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_GS_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_LDTR_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_TR_SELECTOR:
    ಪಕ್ಷ VMX_GUEST_DEBUGCTRL:
    ಪಕ್ಷ VMX_GUEST_PAT:
    ಪಕ್ಷ VMX_GUEST_EFER:
    ಪಕ್ಷ VMX_GUEST_PERF_GLOBAL_CTRL:
    ಪಕ್ಷ VMX_GUEST_PDPTE0:
    ಪಕ್ಷ VMX_GUEST_PDPTE1:
    ಪಕ್ಷ VMX_GUEST_PDPTE2:
    ಪಕ್ಷ VMX_GUEST_PDPTE3:
    ಪಕ್ಷ VMX_GUEST_ES_LIMIT:
    ಪಕ್ಷ VMX_GUEST_CS_LIMIT:
    ಪಕ್ಷ VMX_GUEST_SS_LIMIT:
    ಪಕ್ಷ VMX_GUEST_DS_LIMIT:
    ಪಕ್ಷ VMX_GUEST_FS_LIMIT:
    ಪಕ್ಷ VMX_GUEST_GS_LIMIT:
    ಪಕ್ಷ VMX_GUEST_LDTR_LIMIT:
    ಪಕ್ಷ VMX_GUEST_TR_LIMIT:
    ಪಕ್ಷ VMX_GUEST_GDTR_LIMIT:
    ಪಕ್ಷ VMX_GUEST_IDTR_LIMIT:
    ಪಕ್ಷ VMX_GUEST_ES_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_CS_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_SS_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_DS_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_FS_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_GS_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_TR_ACCESS_RIGHTS:
    ಪಕ್ಷ VMX_GUEST_INTERRUPTABILITY:
    ಪಕ್ಷ VMX_GUEST_ACTIVITY:
    ಪಕ್ಷ VMX_GUEST_SMBASE:
    ಪಕ್ಷ VMX_GUEST_SYSENTER_CS:
    ಪಕ್ಷ VMX_GUEST_PREEMPTION_TIMER_ಮೌಲ್ಯ:
    ಪಕ್ಷ VMX_GUEST_ES_BASE:
    ಪಕ್ಷ VMX_GUEST_CS_BASE:
    ಪಕ್ಷ VMX_GUEST_SS_BASE:
    ಪಕ್ಷ VMX_GUEST_DS_BASE:
    ಪಕ್ಷ VMX_GUEST_FS_BASE:
    ಪಕ್ಷ VMX_GUEST_GS_BASE:
    ಪಕ್ಷ VMX_GUEST_LDTR_BASE:
    ಪಕ್ಷ VMX_GUEST_TR_BASE:
    ಪಕ್ಷ VMX_GUEST_GDTR_BASE:
    ಪಕ್ಷ VMX_GUEST_IDTR_BASE:
    ಪಕ್ಷ VMX_GUEST_DR7:
    ಪಕ್ಷ VMX_GUEST_RFLAGS:
    ಪಕ್ಷ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    ಪಕ್ಷ VMX_GUEST_SYSENTER_ESP:
    ಪಕ್ಷ VMX_GUEST_SYSENTER_EIP:
    ಪಕ್ಷ VMX_CONTROL_CR0_MASK:
    ಪಕ್ಷ VMX_CONTROL_CR4_MASK:
    ಪಕ್ಷ VMX_CONTROL_CR0_READ_SHADOW:
    ಪಕ್ಷ VMX_CONTROL_CR4_READ_SHADOW:
    ಪಕ್ಷ VMX_DATA_INSTRUCTION_ERROR:
    ಪಕ್ಷ VMX_DATA_EXIT_INTERRUPT_INFO:
    ಪಕ್ಷ VMX_DATA_EXIT_INTERRUPT_ERROR:
    ಪಕ್ಷ VMX_DATA_IDT_VECTOR_INFO:
    ಪಕ್ಷ VMX_DATA_IDT_VECTOR_ERROR:
    ಪಕ್ಷ VMX_DATA_EXIT_INSTRUCTION_LENGTH:
    ಪಕ್ಷ VMX_DATA_EXIT_INSTRUCTION_INFO:
    ಪಕ್ಷ VMX_DATA_GUEST_PHYSICAL:
    ಪಕ್ಷ VMX_DATA_IO_RCX:
    ಪಕ್ಷ VMX_DATA_IO_RSI:
    ಪಕ್ಷ VMX_DATA_IO_RDI:
    ಪಕ್ಷ VMX_DATA_IO_RIP:
    ಪಕ್ಷ VMX_DATA_GUEST_LINEAR_ADDRESS:
    ಪಕ್ಷ VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    ಪಕ್ಷ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    ಪಕ್ಷ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    ಪಕ್ಷ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    ಪಕ್ಷ VMX_CONTROL_EXCEPTION_BITMAP:
    ಪಕ್ಷ VMX_CONTROL_ENTRY_CONTROLS:
    ಪಕ್ಷ VMX_CONTROL_EXIT_CONTROLS:
    ಪಕ್ಷ VMX_GUEST_CR0:
    ಪಕ್ಷ VMX_GUEST_CR3:
    ಪಕ್ಷ VMX_GUEST_CR4:
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        userError("VCPU ReadVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, buffer);
}

ಸ್ಥಿರ exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeSetTCB(cap_t cap, word_t length, word_t *buffer)
{
    cap_t tcbCap;
    ಯದಿ (current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಪ್ರತ್ಯಾಗಮ invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

ಶೂನ್ಯ vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *buffer;
    ಯದಿ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    buffer = lookupIPCBuffer(false, ಗಂಟು_STATE(ksCurThread));
    ಯದಿ (!buffer) {
        userError("No IPC buffer.");
        ಪ್ರತ್ಯಾಗಮ;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, buffer));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, buffer), primary_control_ಮೇಲಿನ,
                                                                   primary_control_ಕೆಳಗಿನ));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, buffer));
}

ಶೂನ್ಯ vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *buffer;
    vcpu_t *vcpu;

    buffer = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    ಪ್ರತಿಜ್ಞಾ(vcpu);

    ಯದಿ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, ಸಂದೇಶ_Info_Register, 0);
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
    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ X86VCPUSetTCB:
        ಪ್ರತ್ಯಾಗಮ decodeSetTCB(cap, length, buffer);
    ಪಕ್ಷ X86VCPUReadVMCS:
        ಪ್ರತ್ಯಾಗಮ decodeReadVMCS(cap, length, call, buffer);
    ಪಕ್ಷ X86VCPUWriteVMCS:
        ಪ್ರತ್ಯಾಗಮ decodeWriteVMCS(cap, length, call, buffer);
    ಪಕ್ಷ X86VCPUEnableIOPort:
        ಪ್ರತ್ಯಾಗಮ decodeEnableIOPort(cap, length, buffer);
    ಪಕ್ಷ X86VCPUDisableIOPort:
        ಪ್ರತ್ಯಾಗಮ decodeDisableIOPort(cap, length, buffer);
    ಪಕ್ಷ X86VCPUWriteRegisters:
        ಪ್ರತ್ಯಾಗಮ decodeVCPUWriteRegisters(cap, length, buffer);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    ಪಕ್ಷ X86VCPUWriteMSR:
        ಪ್ರತ್ಯಾಗಮ decodeVCPUWriteMSR(cap, length, buffer);
    ಪಕ್ಷ X86VCPUReadMSR:
        ಪ್ರತ್ಯಾಗಮ decodeVCPUReadMSR(cap, length, buffer);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    ಸ್ವಭಾವ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}

ಸ್ಥಿರ bool_t is_vtx_supported(ಶೂನ್ಯ)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    ಪ್ರತ್ಯಾಗಮ !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ clear_bit(word_t *bitmap, word_t bit)
{
    ಪೂರ್ಣಾಙ್ಕ index = bit / (ಪರಿಮಾಣ(word_t) * 8);
    ಪೂರ್ಣಾಙ್ಕ offset = bit % (ಪರಿಮಾಣ(word_t) * 8);
    bitmap[index] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(ಶೂನ್ಯ)
{
    ಯದಿ (!is_vtx_supported()) {
        printf("vt-x: not supported\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_BASIC_MSR);
    vmx_basic.ಪದಗಳು[1] = x86_rdmsr_ಮೇಲಿನ(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_FEATURE_CONTROL_MSR);
    ಯದಿ (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        ಯದಿ (feature_control_msr_get_lock(feature_control)) {
            printf("vt-x: feature locked\n");
            ಪ್ರತ್ಯಾಗಮ false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ಮೇಲಿನ(IA32_FEATURE_CONTROL_MSR), feature_control.ಪದಗಳು[0]);
    }
    /* make sure the msr is locked */
    ಯದಿ (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ಮೇಲಿನ(IA32_FEATURE_CONTROL_MSR), feature_control.ಪದಗಳು[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    ಯದಿ (CURRENT_CPU_INDEX() == 0) {
        ಯದಿ (!init_vtx_fixed_ಮೌಲ್ಯಗಳು(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            printf("vt-x: lack of required features\n");
            ಪ್ರತ್ಯಾಗಮ false;
        }
    }
    ಯದಿ (!check_vtx_fixed_ಮೌಲ್ಯಗಳು(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        printf("vt-x: cores have inconsistent features\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    ಸಂರಚನಾ vmxon_region *vmxon_kptr = &ARCH_ಗಂಟು_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    ಯದಿ (!vtx_check_fixed_ಮೌಲ್ಯಗಳು(read_cr0(), read_cr4())) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಯದಿ (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        printf("vt-x: vmxon failure\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }
    memset(&msr_bitmap_region, ~0, ಪರಿಮಾಣ(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.ಕೆಳಗಿನ_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.ಕೆಳಗಿನ_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.ಕೆಳಗಿನ_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.ಕೆಳಗಿನ_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.ಕೆಳಗಿನ_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.ಕೆಳಗಿನ_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.ಮೇಲಿನ_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.ಮೇಲಿನ_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.ಮೇಲಿನ_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.ಮೇಲಿನ_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.ಮೇಲಿನ_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.ಮೇಲಿನ_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.ಪದಗಳು[1] = x86_rdmsr_ಮೇಲಿನ(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    ಯದಿ (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        printf("vt-x: Expected wb attribute for EPT paging structure\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಯದಿ (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        printf("vt-x: Expected supported for 2m pages\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }

    ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ ಶೂನ್ಯ setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *buffer;
    ಪೂರ್ಣಾಙ್ಕ i;

    buffer = lookupIPCBuffer(true, ಗಂಟು_STATE(ksCurThread));

    setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_DATA_EXIT_INSTRUCTION_LENGTH));
    setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_DATA_GUEST_PHYSICAL));
    setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    ಪ್ರತಿ (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(ಗಂಟು_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_EAX + i,
              ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(ಗಂಟು_STATE(ksCurThread), ಸಂದೇಶ_Info_Register, SEL4_VMENTER_RESULT_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ finishVmexitSaving(ಶೂನ್ಯ)
{
    vcpu_t *vcpu = ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU);
    ಪ್ರತಿಜ್ಞಾ(vcpu == ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(ಶೂನ್ಯ)
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
    ಯದಿ (reason == EXTERNAL_INTERRUPT) {
        ಗಂಟು_LOCK_IRQ;
        ಯದಿ (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_DATA_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_ಗಂಟು_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } ಅನ್ಯಥಾ {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(ಗಂಟು_STATE(ksCurThread));
#endif
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }

    ಗಂಟು_LOCK_SYS;

    ವಿಕಲ್ಪ (reason) {
    ಪಕ್ಷ EXCEPTION_OR_NMI:
    ಪಕ್ಷ MOV_DR:
    ಪಕ್ಷ TASK_SWITCH:
    ಪಕ್ಷ CONTROL_REGISTER:
    ಪಕ್ಷ IO:
    ಪಕ್ಷ MWAIT:
    ಪಕ್ಷ SIPI:
    ಪಕ್ಷ INVLPG:
    ಪಕ್ಷ INVEPT:
    ಪಕ್ಷ INVVPID:
    ಪಕ್ಷ VMCLEAR:
    ಪಕ್ಷ VMPTRLD:
    ಪಕ್ಷ VMPTRST:
    ಪಕ್ಷ VMREAD:
    ಪಕ್ಷ VMWRITE:
    ಪಕ್ಷ VMXON:
    ಪಕ್ಷ EPT_VIOLATION:
    ಪಕ್ಷ GDTR_OR_IDTR:
    ಪಕ್ಷ LDTR_OR_TR:
    ಪಕ್ಷ TPR_BELOW_THRESHOLD:
    ಪಕ್ಷ APIC_ACCESS:
        qualification = vmread(VMX_DATA_EXIT_QUALIFICATION);
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t handleVmEntryFail(ಶೂನ್ಯ)
{
    handleVmxFault(-1, -1);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
ಶೂನ್ಯ VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    ಪ್ರತಿಜ್ಞಾ(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    ಯದಿ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {

        word_t badge = notification_ptr_get_ntfn_ಸಂದೇಶ_Identifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        ಯದಿ (tcb != ಗಂಟು_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

ಸ್ಥಿರ ಶೂನ್ಯ invvpid_context(uint16_t vpid)
{
    ಸಂರಚನಾ {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    asm ಚಞ್ಚಲ("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
}

ಸ್ಥಿರ ಶೂನ್ಯ setEPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_ಬೇರು;
    ಯದಿ (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_ಬೇರು = kpptr_to_paddr(null_ept_space);
    } ಅನ್ಯಥಾ {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        ಯದಿ (find_ret.status != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_ಬೇರು = kpptr_to_paddr(null_ept_space);
        } ಅನ್ಯಥಾ {
            ept_ಬೇರು = pptr_to_paddr(pml4);
        }
    }
    ಯದಿ (ept_ಬೇರು != vcpu->last_ept_ಬೇರು) {
        vcpu->last_ept_ಬೇರು = ept_ಬೇರು;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_ಬೇರು,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.ಪದಗಳು[0]);
        ಪ್ರತಿಜ್ಞಾ(vcpu->vpid != VPID_INVALID);
        ಯದಿ (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

ಶೂನ್ಯ clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t last)
{
    ಯದಿ (vpid == VPID_INVALID) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    ಯದಿ (vcpu == ರಿಕ್ತ) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿಜ್ಞಾ(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ vpid_t nextVPID(vpid_t vpid)
{
    ಯದಿ (vpid == VPID_LAST) {
        ಪ್ರತ್ಯಾಗಮ VPID_FIRST;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ vpid + 1;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, ಪರಿಮಾಣ(vcpu->io));
    /* invalidate the VPID context */
    ಯದಿ (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
ಸ್ಥಿರ ಶೂನ್ಯ releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = ರಿಕ್ತ;
}

ಸ್ಥಿರ vpid_t findFreeVPID(ಶೂನ್ಯ)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    ಕುರು {
        ಯದಿ (x86KSVPIDTable[vpid] == ರಿಕ್ತ) {
            ಪ್ರತ್ಯಾಗಮ vpid;
        }
        vpid = nextVPID(vpid);
    } ಯಾವತ್ (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = nextVPID(x86KSNextVPID);
    ಪ್ರತ್ಯಾಗಮ vpid;
}

ಸ್ಥಿರ ಶೂನ್ಯ storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    ಪ್ರತಿಜ್ಞಾ(x86KSVPIDTable[vpid] == ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(vcpu->vpid == VPID_INVALID);
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
ಶೂನ್ಯ vcpu_fpu_to_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    ಯದಿ (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        ಯದಿ (!vcpu->fpu_active) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_active = true;
            loadFpuState(tcb);
        }
    } ಅನ್ಯಥಾ {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_active = true;
        switchLocalFpuOwner(tcb);
    }
}

ಶೂನ್ಯ vcpu_fpu_to_host(tcb_t *tcb, vcpu_t *vcpu)
{
    ಯದಿ (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } ಅನ್ಯಥಾ ಯದಿ (nativeThreadUsingFPU(tcb) && vcpu->fpu_active) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_active = false;
        loadFpuState(tcb);
    } ಅನ್ಯಥಾ ಯದಿ (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

ಶೂನ್ಯ restoreVMCS(ಶೂನ್ಯ)
{
    tcb_t *cur_thread = ಗಂಟು_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    ಯದಿ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ifndef CONFIG_KERNEL_SKIM_WINDOW
    ಯದಿ (getCurrentCR3().ಪದಗಳು[0] != expected_vmcs->last_host_cr3) {
        expected_vmcs->last_host_cr3 = getCurrentCR3().ಪದಗಳು[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().ಪದಗಳು[0]);
    }
#endif
    ಯದಿ (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        ಯದಿ (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    setEPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

ಶೂನ್ಯ invept(ept_pml4e_t *ept_pml4)
{
    ಯದಿ (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        ಸಂರಚನಾ {
            uint64_t parts[2];
        } address;
        word_t type;
        ಯದಿ (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } ಅನ್ಯಥಾ ಯದಿ (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } ಅನ್ಯಥಾ {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            ಪ್ರತ್ಯಾಗಮ;
        }

        address.parts[0] = pptr_to_paddr((ಶೂನ್ಯ *)ept_pml4);
        address.parts[1] = 0;
        asm ಚಞ್ಚಲ(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
