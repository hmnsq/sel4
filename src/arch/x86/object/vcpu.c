/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ifdef CONFIG_VTX

#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <object/objecttype.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/cpu_registers.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/vcpu.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/ioport.h>
#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <sel4/arch/vmenter.h>

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

ଅଚଳ vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା msr_bitmap {
    word_t bitmap[0x2000 / ଆକାର(word_t) / 8];
} msr_bitmap_t;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା msr_bitmaps {
    msr_bitmap_t ତଳ_msr_read;
    msr_bitmap_t ଉପର_msr_read;
    msr_bitmap_t ତଳ_msr_write;
    msr_bitmap_t ଉପର_msr_write;
} msr_bitmaps_t;

ସ୍ଥିର msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

ସ୍ଥିର ଅକ୍ଷର null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
ସ୍ଥିର uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
ସ୍ଥିର vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
ସ୍ଥିର uint32_t pin_control_ଉପର;
ସ୍ଥିର uint32_t pin_control_ତଳ;
ସ୍ଥିର uint32_t primary_control_ଉପର;
ସ୍ଥିର uint32_t primary_control_ତଳ;
ସ୍ଥିର uint32_t secondary_control_ଉପର;
ସ୍ଥିର uint32_t secondary_control_ତଳ;
ସ୍ଥିର uint32_t entry_control_ଉପର;
ସ୍ଥିର uint32_t entry_control_ତଳ;
ସ୍ଥିର uint32_t exit_control_ଉପର;
ସ୍ଥିର uint32_t exit_control_ତଳ;
ସ୍ଥିର uint32_t cr0_ଉପର;
ସ୍ଥିର uint32_t cr0_ତଳ;
ସ୍ଥିର uint32_t cr4_ଉପର;
ସ୍ଥିର uint32_t cr4_ତଳ;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
ସ୍ଥିର bool_t vmx_feature_vpid;
ସ୍ଥିର bool_t vmx_feature_load_perf_global_ctrl;
ସ୍ଥିର bool_t vmx_feature_ack_on_exit;

ସ୍ଥିର vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
ସ୍ଥିର vpid_t x86KSNextVPID = VPID_FIRST;

ସ୍ଥିର ଶୂନ୍ୟ releaseVPID(vpid_t vpid);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm ଅସ୍ଥିର(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    ଫେରନ୍ତୁ !!error;
}

ସ୍ଥିର ଶୂନ୍ୟ vmclear(ଶୂନ୍ୟ *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((ଶୂନ୍ୟ *)vmcs_ptr);
    asm ଅସ୍ଥିର(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

ଶୂନ୍ୟ clearCurrentVCPU(ଶୂନ୍ୟ)
{
    vcpu_t *vcpu = ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentVCPU);
    ଯଦି (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentVCPU) = ଖାଲି;
        ଯଦି (vcpu->fpu_active && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_active = false;
        }
    }
}

ସ୍ଥିର ଶୂନ୍ୟ vmptrld(ଶୂନ୍ୟ *vmcs_ptr)
{
    uint64_t physical_address;
    uint8_t error;
    physical_address = pptr_to_paddr(vmcs_ptr);
    asm ଅସ୍ଥିର(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_address)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    ପ୍ରତିଜ୍ଞା(!error);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
ଶୂନ୍ୟ vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_STAR_MSR, vcpu->syscall_registers[VCPU_STAR]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_MASK]);
}

ଶୂନ୍ୟ vcpu_restore_host_msrs(ଶୂନ୍ୟ)
{
    init_syscall_msrs();
}
#endif

ସ୍ଥିର ଶୂନ୍ୟ switchVCPU(vcpu_t *vcpu)
{
#ifdef ENABLE_SMP_SUPPORT
    ଯଦି (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_ଗଣ୍ଠି_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    ଯଦି (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((ଅକ୍ଷର *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.ଶବ୍ଦଗୁଡ଼ିକ[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentVCPU) = vcpu;
}

ସ୍ଥିର ଶୂନ୍ୟ print_bits(word_t bits)
{
    bool_t first = true;
    ଯେତେବେଳେ (bits) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = seL4_WordBits - 1 - clzl(bits);
        ଯଦି (first) {
            printf("%d", index);
            first = false;
        } ଅନ୍ୟଥା {
            printf(",%d", index);
        }
        bits &= ~BIT(index);
    }
}

ସ୍ଥିର bool_t check_fixed_ମୂଲ୍ୟ(word_t val, word_t ତଳ, word_t ଉପର)
{
    word_t not_ଉପର;
    word_t not_ତଳ;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_ଉପର = ଉପର & ~(ଉପର & val);
    ଯଦି (not_ଉପର != 0) {
        printf("Failed to set bits: ");
        print_bits(not_ଉପର);
        ଫେରନ୍ତୁ false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_ତଳ = ~ତଳ & ~(~ତଳ & ~val);
    ଯଦି (not_ତଳ != 0) {
        printf("Incorrectly cleared bits: ");
        print_bits(not_ତଳ);
        ଫେରନ୍ତୁ false;
    }
    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର bool_t vtx_check_fixed_ମୂଲ୍ୟଗୁଡ଼ିକ(word_t cr0, word_t cr4)
{
    ଯଦି (!check_fixed_ମୂଲ୍ୟ(cr0, cr0_ତଳ, cr0_ଉପର)) {
        printf(" of CR0\n");
        ଫେରନ୍ତୁ false;
    }
    ଯଦି (!check_fixed_ମୂଲ୍ୟ(cr4, cr4_ତଳ, cr4_ଉପର)) {
        printf(" of CR4\n");
        ଫେରନ୍ତୁ false;
    }
    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର bool_t BOOT_CODE init_vtx_fixed_ମୂଲ୍ୟଗୁଡ଼ିକ(bool_t useTrueMsrs)
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
    ଯଦି (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } ଅନ୍ୟଥା {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    pin_control_ଉପର = x86_rdmsr_ତଳ(pinbased_ctls);
    pin_control_ତଳ = x86_rdmsr_ଉପର(pinbased_ctls);
    primary_control_ଉପର = x86_rdmsr_ତଳ(procbased_ctls);
    primary_control_ତଳ = x86_rdmsr_ଉପର(procbased_ctls);
    secondary_control_ଉପର = x86_rdmsr_ତଳ(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_ତଳ = x86_rdmsr_ଉପର(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_ଉପର = x86_rdmsr_ତଳ(exit_ctls);
    exit_control_ତଳ = x86_rdmsr_ଉପର(exit_ctls);
    entry_control_ଉପର = x86_rdmsr_ତଳ(entry_ctls);
    entry_control_ତଳ = x86_rdmsr_ଉପର(entry_ctls);

    cr0_ଉପର = x86_rdmsr_ତଳ(IA32_VMX_CR0_FIXED0_MSR);
    cr0_ତଳ = x86_rdmsr_ତଳ(IA32_VMX_CR0_FIXED1_MSR);
    cr4_ଉପର = x86_rdmsr_ତଳ(IA32_VMX_CR4_FIXED0_MSR);
    cr4_ତଳ = x86_rdmsr_ତଳ(IA32_VMX_CR4_FIXED1_MSR);

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
    ଯଦି (secondary_control_ତଳ & BIT(7)) {
        cr0_ଉପର &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    ଯଦି (!(secondary_control_ତଳ & BIT(5))) {
        vmx_feature_vpid = 0;
        printf("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } ଅନ୍ୟଥା {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    ଯଦି (!(exit_control_ତଳ & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        printf("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } ଅନ୍ୟଥା {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    ଯଦି (!(exit_control_ତଳ & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        printf("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } ଅନ୍ୟଥା {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_ତଳ) & pin_control_mask;
    ଯଦି (missing) {
        printf("vt-x: Unsupported pin control features %lx\n", (long)missing);
        ଫେରନ୍ତୁ false;
    }
    missing = (~primary_control_ତଳ) & primary_control_mask;
    ଯଦି (missing) {
        printf("vt-x: Unsupported primary control features %lx\n", (long)missing);
        ଫେରନ୍ତୁ false;
    }
    missing = (~secondary_control_ତଳ) & secondary_control_mask;
    ଯଦି (missing) {
        printf("vt-x: Unsupported secondary control features %lx\n", (long)missing);
        ଫେରନ୍ତୁ false;
    }
    missing = (~exit_control_ତଳ) & exit_control_mask;
    ଯଦି (missing) {
        printf("vt-x: Unsupported exit control features %lx\n", (long)missing);
        ଫେରନ୍ତୁ false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_ତଳ) & BIT(9);
    ଯଦି (missing) {
        printf("vt-x: CPU does not support 64-bit guests\n");
        ଫେରନ୍ତୁ false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_ଉପର |= pin_control_mask;
    primary_control_ଉପର |= primary_control_mask;
    secondary_control_ଉପର |= secondary_control_mask;
    exit_control_ଉପର |= exit_control_mask;

    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର bool_t BOOT_CODE check_vtx_fixed_ମୂଲ୍ୟଗୁଡ଼ିକ(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    ଯଦି (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } ଅନ୍ୟଥା {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    uint32_t local_pin_control_ଉପର = x86_rdmsr_ତଳ(pinbased_ctls);
    uint32_t local_pin_control_ତଳ = x86_rdmsr_ଉପର(pinbased_ctls);
    uint32_t local_primary_control_ଉପର = x86_rdmsr_ତଳ(procbased_ctls);
    uint32_t local_primary_control_ତଳ = x86_rdmsr_ଉପର(procbased_ctls);
    uint32_t local_secondary_control_ଉପର = x86_rdmsr_ତଳ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_ତଳ = x86_rdmsr_ଉପର(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_ଉପର = x86_rdmsr_ତଳ(exit_ctls);
    uint32_t local_exit_control_ତଳ = x86_rdmsr_ଉପର(exit_ctls);
    uint32_t local_entry_control_ଉପର = x86_rdmsr_ତଳ(entry_ctls);
    uint32_t local_entry_control_ତଳ = x86_rdmsr_ଉପର(entry_ctls);

    uint32_t local_cr0_ଉପର = x86_rdmsr_ତଳ(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_ତଳ = x86_rdmsr_ତଳ(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_ଉପର = x86_rdmsr_ତଳ(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_ତଳ = x86_rdmsr_ତଳ(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    ଯଦି (local_secondary_control_ତଳ & BIT(7)) {
        local_cr0_ଉପର &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    ଫେରନ୍ତୁ
        (local_pin_control_ଉପର & pin_control_ଉପର) == local_pin_control_ଉପର &&
        (~local_pin_control_ତଳ & pin_control_ଉପର) == 0 &&
        (local_primary_control_ଉପର & primary_control_ଉପର) == local_primary_control_ଉପର &&
        (~local_primary_control_ତଳ & primary_control_ଉପର) == 0 &&
        (local_secondary_control_ଉପର & secondary_control_ଉପର) == local_secondary_control_ଉପର &&
        (~local_secondary_control_ତଳ & secondary_control_ଉପର) == 0 &&
        (local_exit_control_ଉପର & exit_control_ଉପର) == local_exit_control_ଉପର &&
        (~local_exit_control_ତଳ & exit_control_ଉପର) == 0 &&
        (local_entry_control_ଉପର & entry_control_ଉପର) == local_entry_control_ଉପର &&
        (~local_entry_control_ତଳ & entry_control_ଉପର) == 0 &&
        local_cr0_ଉପର == cr0_ଉପର &&
        local_cr0_ତଳ == cr0_ତଳ &&
        local_cr4_ଉପର == cr4_ଉପର &&
        local_cr4_ତଳ == cr4_ତଳ;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t applyFixedBits(uint32_t original, uint32_t ଉପର, uint32_t ତଳ)
{
    original |= ଉପର;
    original &= ତଳ;
    ଫେରନ୍ତୁ original;
}

ଶୂନ୍ୟ vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = ଖାଲି;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    memcpy(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_ଉପର & cr0_ତଳ;
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
    ଯଦି (vmx_feature_load_perf_global_ctrl) {
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
    ଯଦି (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((ଅକ୍ଷର *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.ଶବ୍ଦଗୁଡ଼ିକ[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_register]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_to_paddr(x64KSKernelPML4), 0).ଶବ୍ଦଗୁଡ଼ିକ[0]);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_ଉପର & pin_control_ତଳ);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_ଉପର & primary_control_ତଳ);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_ଉପର & secondary_control_ତଳ);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_ଉପର & exit_control_ତଳ);

    uint64_t entry_control = entry_control_ଉପର & entry_control_ତଳ;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_ଉପର & cr4_ତଳ);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_ଉପର, ~(word_t)0);

    memset(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_SIZE);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((ଅକ୍ଷର *)vcpu->io + (VCPU_IOBITMAP_SIZE / 2)));
}

ସ୍ଥିର ଶୂନ୍ୟ dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ପ୍ରତିଜ୍ଞା(tcb->tcbArch.tcbVCPU == vcpu);
    ପ୍ରତିଜ୍ଞା(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = ଖାଲି;
    vcpu->vcpuTCB = ଖାଲି;
}

ଶୂନ୍ୟ vcpu_finalise(vcpu_t *vcpu)
{
    ଯଦି (ARCH_ଗଣ୍ଠି_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
#ifdef ENABLE_SMP_SUPPORT
        ଯଦି (vcpu->last_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->last_cpu);
        } ଅନ୍ୟଥା
#endif /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    ଯଦି (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    ଯଦି (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

ସ୍ଥିର ଶୂନ୍ୟ associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ଯଦି (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    ଯଦି (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

ସ୍ଥିର exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *buffer)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପାଇଁ (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, buffer);
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeVCPUWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    ଯଦି (length < 15) {
#ଅନ୍ୟଥା
    ଯଦି (length < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଫେରନ୍ତୁ invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
ସ୍ଥିର exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *buffer, word_t field, word_t ମୂଲ୍ୟ)
{
    tcb_t *thread;
    thread = ଗଣ୍ଠି_STATE(ksCurThread);
    ଯଦି (ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ବାଛ (field) {
    ସ୍ଥିତି IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = ମୂଲ୍ୟ;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = ମୂଲ୍ୟ;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = ମୂଲ୍ୟ;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = ମୂଲ୍ୟ;
        ଭାଙ୍ଗ;
    }
    setMR(thread, buffer, 0, ମୂଲ୍ୟ);
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeVCPUWriteMSR(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t ମୂଲ୍ୟ;

    ଯଦି (length < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    ମୂଲ୍ୟ = getSyscallArg(1, buffer);
    ବାଛ (field) {
    ସ୍ଥିତି IA32_LSTAR_MSR:
    ସ୍ଥିତି IA32_STAR_MSR:
    ସ୍ଥିତି IA32_CSTAR_MSR:
    ସ୍ଥିତି IA32_FMASK_MSR:
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("VCPU WriteMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଫେରନ୍ତୁ invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer, field, ମୂଲ୍ୟ);
}

ସ୍ଥିର exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *buffer)
{
    tcb_t *thread;
    thread = ଗଣ୍ଠି_STATE(ksCurThread);

    word_t ମୂଲ୍ୟ = 0;

    ବାଛ (field) {
    ସ୍ଥିତି IA32_LSTAR_MSR:
        ମୂଲ୍ୟ = vcpu->syscall_registers[VCPU_LSTAR];
        ଭାଙ୍ଗ;
    ସ୍ଥିତି IA32_STAR_MSR:
        ମୂଲ୍ୟ = vcpu->syscall_registers[VCPU_STAR];
        ଭାଙ୍ଗ;
    ସ୍ଥିତି IA32_CSTAR_MSR:
        ମୂଲ୍ୟ = vcpu->syscall_registers[VCPU_CSTAR];
        ଭାଙ୍ଗ;
    ସ୍ଥିତି IA32_FMASK_MSR:
        ମୂଲ୍ୟ = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        ଭାଙ୍ଗ;
    ସ୍ଥିତି IA32_VMX_MISC_MSR:
        ମୂଲ୍ୟ = x86_rdmsr(field);
        ଭାଙ୍ଗ;
    }

    setMR(thread, buffer, 0, ମୂଲ୍ୟ);
    setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeVCPUReadMSR(cap_t cap, word_t length, word_t *buffer)
{
    ଯଦି (length < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    ବାଛ (field) {
    ସ୍ଥିତି IA32_LSTAR_MSR:
    ସ୍ଥିତି IA32_STAR_MSR:
    ସ୍ଥିତି IA32_CSTAR_MSR:
    ସ୍ଥିତି IA32_FMASK_MSR:
    ସ୍ଥିତି IA32_VMX_MISC_MSR:
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("VCPU ReadMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଫେରନ୍ତୁ invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, buffer);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

ସ୍ଥିର exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t ତଳ, uint16_t ଉପର)
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
    setIOPortMask(vcpu->io, ତଳ, ଉପର, false);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeEnableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t ତଳ, ଉପର;

    ଯଦି (length < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    ଯଦି (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ତଳ = getSyscallArg(0, buffer);
    ଉପର = getSyscallArg(1, buffer);

    ଯଦି (ତଳ < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || ଉପର > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 0;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeEnableIOPort(vcpu, ioSlot, ioCap, ତଳ, ଉପର);
}

ସ୍ଥିର exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t ତଳ, uint16_t ଉପର)
{
    setIOPortMask(vcpu->io, ତଳ, ଉପର, true);
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeDisableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    uint16_t ତଳ, ଉପର;

    ଯଦି (length < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ତଳ = getSyscallArg(0, buffer);
    ଉପର = getSyscallArg(1, buffer);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ଫେରନ୍ତୁ invokeDisableIOPort(vcpu, ତଳ, ଉପର);
}

ସ୍ଥିର exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *buffer, word_t field, word_t ମୂଲ୍ୟ)
{
    tcb_t *thread;
    thread = ଗଣ୍ଠି_STATE(ksCurThread);

    ଯଦି (ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ବାଛ (field) {
    ସ୍ଥିତି VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = ମୂଲ୍ୟ;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = ମୂଲ୍ୟ;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = ମୂଲ୍ୟ;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = ମୂଲ୍ୟ;
        ଭାଙ୍ଗ;
    }
    vmwrite(field, ମୂଲ୍ୟ);

    ଯଦି (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length = setMR(thread, buffer, 0, ମୂଲ୍ୟ);
        setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Running);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeWriteVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    word_t ମୂଲ୍ୟ;

    ଯଦି (length < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    ମୂଲ୍ୟ = getSyscallArg(1, buffer);
    ବାଛ (field) {
    ସ୍ଥିତି VMX_GUEST_RIP:
    ସ୍ଥିତି VMX_GUEST_RSP:
    ସ୍ଥିତି VMX_GUEST_ES_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_CS_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_SS_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_DS_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_FS_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_GS_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_LDTR_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_TR_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_DEBUGCTRL:
    ସ୍ଥିତି VMX_GUEST_PAT:
    ସ୍ଥିତି VMX_GUEST_EFER:
    ସ୍ଥିତି VMX_GUEST_PERF_GLOBAL_CTRL:
    ସ୍ଥିତି VMX_GUEST_PDPTE0:
    ସ୍ଥିତି VMX_GUEST_PDPTE1:
    ସ୍ଥିତି VMX_GUEST_PDPTE2:
    ସ୍ଥିତି VMX_GUEST_PDPTE3:
    ସ୍ଥିତି VMX_GUEST_ES_LIMIT:
    ସ୍ଥିତି VMX_GUEST_CS_LIMIT:
    ସ୍ଥିତି VMX_GUEST_SS_LIMIT:
    ସ୍ଥିତି VMX_GUEST_DS_LIMIT:
    ସ୍ଥିତି VMX_GUEST_FS_LIMIT:
    ସ୍ଥିତି VMX_GUEST_GS_LIMIT:
    ସ୍ଥିତି VMX_GUEST_LDTR_LIMIT:
    ସ୍ଥିତି VMX_GUEST_TR_LIMIT:
    ସ୍ଥିତି VMX_GUEST_GDTR_LIMIT:
    ସ୍ଥିତି VMX_GUEST_IDTR_LIMIT:
    ସ୍ଥିତି VMX_GUEST_ES_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_CS_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_SS_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_DS_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_FS_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_GS_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_LDTR_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_TR_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_INTERRUPTABILITY:
    ସ୍ଥିତି VMX_GUEST_ACTIVITY:
    ସ୍ଥିତି VMX_GUEST_SMBASE:
    ସ୍ଥିତି VMX_GUEST_SYSENTER_CS:
    ସ୍ଥିତି VMX_GUEST_PREEMPTION_TIMER_ମୂଲ୍ୟ:
    ସ୍ଥିତି VMX_GUEST_ES_BASE:
    ସ୍ଥିତି VMX_GUEST_CS_BASE:
    ସ୍ଥିତି VMX_GUEST_SS_BASE:
    ସ୍ଥିତି VMX_GUEST_DS_BASE:
    ସ୍ଥିତି VMX_GUEST_FS_BASE:
    ସ୍ଥିତି VMX_GUEST_GS_BASE:
    ସ୍ଥିତି VMX_GUEST_LDTR_BASE:
    ସ୍ଥିତି VMX_GUEST_TR_BASE:
    ସ୍ଥିତି VMX_GUEST_GDTR_BASE:
    ସ୍ଥିତି VMX_GUEST_IDTR_BASE:
    ସ୍ଥିତି VMX_GUEST_DR7:
    ସ୍ଥିତି VMX_GUEST_RFLAGS:
    ସ୍ଥିତି VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    ସ୍ଥିତି VMX_GUEST_SYSENTER_ESP:
    ସ୍ଥିତି VMX_GUEST_SYSENTER_EIP:
    ସ୍ଥିତି VMX_CONTROL_CR0_MASK:
    ସ୍ଥିତି VMX_CONTROL_CR4_MASK:
    ସ୍ଥିତି VMX_CONTROL_CR0_READ_SHADOW:
    ସ୍ଥିତି VMX_CONTROL_CR4_READ_SHADOW:
    ସ୍ଥିତି VMX_GUEST_CR3:
    ସ୍ଥିତି VMX_CONTROL_EXCEPTION_BITMAP:
    ସ୍ଥିତି VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    ସ୍ଥିତି VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        ଭାଙ୍ଗ;
    ସ୍ଥିତି VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        ମୂଲ୍ୟ = applyFixedBits(ମୂଲ୍ୟ, pin_control_ଉପର, pin_control_ତଳ);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        ମୂଲ୍ୟ = applyFixedBits(ମୂଲ୍ୟ, primary_control_ଉପର, primary_control_ତଳ);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        ମୂଲ୍ୟ = applyFixedBits(ମୂଲ୍ୟ, secondary_control_ଉପର, secondary_control_ତଳ);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି VMX_CONTROL_ENTRY_CONTROLS:
        ମୂଲ୍ୟ = applyFixedBits(ମୂଲ୍ୟ, entry_control_ଉପର, entry_control_ତଳ);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି VMX_CONTROL_EXIT_CONTROLS:
        ମୂଲ୍ୟ = applyFixedBits(ମୂଲ୍ୟ, exit_control_ଉପର, exit_control_ତଳ);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି VMX_GUEST_CR0:
        ମୂଲ୍ୟ = applyFixedBits(ମୂଲ୍ୟ, cr0_ଉପର, cr0_ତଳ);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି VMX_GUEST_CR4:
        ମୂଲ୍ୟ = applyFixedBits(ମୂଲ୍ୟ, cr4_ଉପର, cr4_ତଳ);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("VCPU WriteVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଫେରନ୍ତୁ invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, buffer, field, ମୂଲ୍ୟ);
}

ସ୍ଥିର word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    ବାଛ (field) {
    ସ୍ଥିତି VMX_CONTROL_EXCEPTION_BITMAP:
        ଫେରନ୍ତୁ vcpu->exception_bitmap;
    ସ୍ଥିତି VMX_GUEST_CR0:
        ଫେରନ୍ତୁ vcpu->cr0;
    ସ୍ଥିତି VMX_CONTROL_CR0_MASK:
        ଫେରନ୍ତୁ vcpu->cr0_mask;
    ସ୍ଥିତି VMX_CONTROL_CR0_READ_SHADOW:
        ଫେରନ୍ତୁ vcpu->cr0_shadow;
    }
    ଯଦି (ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ଫେରନ୍ତୁ vmread(field);
}

ସ୍ଥିର exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *buffer)
{
    tcb_t *thread;
    thread = ଗଣ୍ଠି_STATE(ksCurThread);
    word_t ମୂଲ୍ୟ = readVMCSField(vcpu, field);
    ଯଦି (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length = setMR(thread, buffer, 0, ମୂଲ୍ୟ);
        setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Running);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeReadVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    ଯଦି (length < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    ବାଛ (field) {
    ସ୍ଥିତି VMX_GUEST_RIP:
    ସ୍ଥିତି VMX_GUEST_RSP:
    ସ୍ଥିତି VMX_GUEST_ES_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_CS_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_SS_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_DS_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_FS_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_GS_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_LDTR_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_TR_SELECTOR:
    ସ୍ଥିତି VMX_GUEST_DEBUGCTRL:
    ସ୍ଥିତି VMX_GUEST_PAT:
    ସ୍ଥିତି VMX_GUEST_EFER:
    ସ୍ଥିତି VMX_GUEST_PERF_GLOBAL_CTRL:
    ସ୍ଥିତି VMX_GUEST_PDPTE0:
    ସ୍ଥିତି VMX_GUEST_PDPTE1:
    ସ୍ଥିତି VMX_GUEST_PDPTE2:
    ସ୍ଥିତି VMX_GUEST_PDPTE3:
    ସ୍ଥିତି VMX_GUEST_ES_LIMIT:
    ସ୍ଥିତି VMX_GUEST_CS_LIMIT:
    ସ୍ଥିତି VMX_GUEST_SS_LIMIT:
    ସ୍ଥିତି VMX_GUEST_DS_LIMIT:
    ସ୍ଥିତି VMX_GUEST_FS_LIMIT:
    ସ୍ଥିତି VMX_GUEST_GS_LIMIT:
    ସ୍ଥିତି VMX_GUEST_LDTR_LIMIT:
    ସ୍ଥିତି VMX_GUEST_TR_LIMIT:
    ସ୍ଥିତି VMX_GUEST_GDTR_LIMIT:
    ସ୍ଥିତି VMX_GUEST_IDTR_LIMIT:
    ସ୍ଥିତି VMX_GUEST_ES_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_CS_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_SS_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_DS_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_FS_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_GS_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_LDTR_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_TR_ACCESS_RIGHTS:
    ସ୍ଥିତି VMX_GUEST_INTERRUPTABILITY:
    ସ୍ଥିତି VMX_GUEST_ACTIVITY:
    ସ୍ଥିତି VMX_GUEST_SMBASE:
    ସ୍ଥିତି VMX_GUEST_SYSENTER_CS:
    ସ୍ଥିତି VMX_GUEST_PREEMPTION_TIMER_ମୂଲ୍ୟ:
    ସ୍ଥିତି VMX_GUEST_ES_BASE:
    ସ୍ଥିତି VMX_GUEST_CS_BASE:
    ସ୍ଥିତି VMX_GUEST_SS_BASE:
    ସ୍ଥିତି VMX_GUEST_DS_BASE:
    ସ୍ଥିତି VMX_GUEST_FS_BASE:
    ସ୍ଥିତି VMX_GUEST_GS_BASE:
    ସ୍ଥିତି VMX_GUEST_LDTR_BASE:
    ସ୍ଥିତି VMX_GUEST_TR_BASE:
    ସ୍ଥିତି VMX_GUEST_GDTR_BASE:
    ସ୍ଥିତି VMX_GUEST_IDTR_BASE:
    ସ୍ଥିତି VMX_GUEST_DR7:
    ସ୍ଥିତି VMX_GUEST_RFLAGS:
    ସ୍ଥିତି VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    ସ୍ଥିତି VMX_GUEST_SYSENTER_ESP:
    ସ୍ଥିତି VMX_GUEST_SYSENTER_EIP:
    ସ୍ଥିତି VMX_CONTROL_CR0_MASK:
    ସ୍ଥିତି VMX_CONTROL_CR4_MASK:
    ସ୍ଥିତି VMX_CONTROL_CR0_READ_SHADOW:
    ସ୍ଥିତି VMX_CONTROL_CR4_READ_SHADOW:
    ସ୍ଥିତି VMX_DATA_INSTRUCTION_ERROR:
    ସ୍ଥିତି VMX_DATA_EXIT_INTERRUPT_INFO:
    ସ୍ଥିତି VMX_DATA_EXIT_INTERRUPT_ERROR:
    ସ୍ଥିତି VMX_DATA_IDT_VECTOR_INFO:
    ସ୍ଥିତି VMX_DATA_IDT_VECTOR_ERROR:
    ସ୍ଥିତି VMX_DATA_EXIT_INSTRUCTION_LENGTH:
    ସ୍ଥିତି VMX_DATA_EXIT_INSTRUCTION_INFO:
    ସ୍ଥିତି VMX_DATA_GUEST_PHYSICAL:
    ସ୍ଥିତି VMX_DATA_IO_RCX:
    ସ୍ଥିତି VMX_DATA_IO_RSI:
    ସ୍ଥିତି VMX_DATA_IO_RDI:
    ସ୍ଥିତି VMX_DATA_IO_RIP:
    ସ୍ଥିତି VMX_DATA_GUEST_LINEAR_ADDRESS:
    ସ୍ଥିତି VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    ସ୍ଥିତି VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    ସ୍ଥିତି VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    ସ୍ଥିତି VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    ସ୍ଥିତି VMX_CONTROL_EXCEPTION_BITMAP:
    ସ୍ଥିତି VMX_CONTROL_ENTRY_CONTROLS:
    ସ୍ଥିତି VMX_CONTROL_EXIT_CONTROLS:
    ସ୍ଥିତି VMX_GUEST_CR0:
    ସ୍ଥିତି VMX_GUEST_CR3:
    ସ୍ଥିତି VMX_GUEST_CR4:
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("VCPU ReadVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଫେରନ୍ତୁ invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, buffer);
}

ସ୍ଥିର exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t decodeSetTCB(cap_t cap, word_t length, word_t *buffer)
{
    cap_t tcbCap;
    ଯଦି (current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ଯଦି (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଫେରନ୍ତୁ invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

ଶୂନ୍ୟ vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *buffer;
    ଯଦି (ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    buffer = lookupIPCBuffer(false, ଗଣ୍ଠି_STATE(ksCurThread));
    ଯଦି (!buffer) {
        userError("No IPC buffer.");
        ଫେରନ୍ତୁ;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, buffer));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, buffer), primary_control_ଉପର,
                                                                   primary_control_ତଳ));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, buffer));
}

ଶୂନ୍ୟ vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *buffer;
    vcpu_t *vcpu;

    buffer = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    ପ୍ରତିଜ୍ଞା(vcpu);

    ଯଦି (ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, ସନ୍ଦେଶ_Info_Register, 0);
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
    ବାଛ (invLabel) {
    ସ୍ଥିତି X86VCPUSetTCB:
        ଫେରନ୍ତୁ decodeSetTCB(cap, length, buffer);
    ସ୍ଥିତି X86VCPUReadVMCS:
        ଫେରନ୍ତୁ decodeReadVMCS(cap, length, call, buffer);
    ସ୍ଥିତି X86VCPUWriteVMCS:
        ଫେରନ୍ତୁ decodeWriteVMCS(cap, length, call, buffer);
    ସ୍ଥିତି X86VCPUEnableIOPort:
        ଫେରନ୍ତୁ decodeEnableIOPort(cap, length, buffer);
    ସ୍ଥିତି X86VCPUDisableIOPort:
        ଫେରନ୍ତୁ decodeDisableIOPort(cap, length, buffer);
    ସ୍ଥିତି X86VCPUWriteRegisters:
        ଫେରନ୍ତୁ decodeVCPUWriteRegisters(cap, length, buffer);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    ସ୍ଥିତି X86VCPUWriteMSR:
        ଫେରନ୍ତୁ decodeVCPUWriteMSR(cap, length, buffer);
    ସ୍ଥିତି X86VCPUReadMSR:
        ଫେରନ୍ତୁ decodeVCPUReadMSR(cap, length, buffer);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}

ସ୍ଥିର bool_t is_vtx_supported(ଶୂନ୍ୟ)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    ଫେରନ୍ତୁ !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ clear_bit(word_t *bitmap, word_t bit)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = bit / (ଆକାର(word_t) * 8);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = bit % (ଆକାର(word_t) * 8);
    bitmap[index] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(ଶୂନ୍ୟ)
{
    ଯଦି (!is_vtx_supported()) {
        printf("vt-x: not supported\n");
        ଫେରନ୍ତୁ false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.ଶବ୍ଦଗୁଡ଼ିକ[0] = x86_rdmsr_ତଳ(IA32_VMX_BASIC_MSR);
    vmx_basic.ଶବ୍ଦଗୁଡ଼ିକ[1] = x86_rdmsr_ଉପର(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.ଶବ୍ଦଗୁଡ଼ିକ[0] = x86_rdmsr_ତଳ(IA32_FEATURE_CONTROL_MSR);
    ଯଦି (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        ଯଦି (feature_control_msr_get_lock(feature_control)) {
            printf("vt-x: feature locked\n");
            ଫେରନ୍ତୁ false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ଉପର(IA32_FEATURE_CONTROL_MSR), feature_control.ଶବ୍ଦଗୁଡ଼ିକ[0]);
    }
    /* make sure the msr is locked */
    ଯଦି (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ଉପର(IA32_FEATURE_CONTROL_MSR), feature_control.ଶବ୍ଦଗୁଡ଼ିକ[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    ଯଦି (CURRENT_CPU_INDEX() == 0) {
        ଯଦି (!init_vtx_fixed_ମୂଲ୍ୟଗୁଡ଼ିକ(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            printf("vt-x: lack of required features\n");
            ଫେରନ୍ତୁ false;
        }
    }
    ଯଦି (!check_vtx_fixed_ମୂଲ୍ୟଗୁଡ଼ିକ(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        printf("vt-x: cores have inconsistent features\n");
        ଫେରନ୍ତୁ false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    ସଂରଚନା vmxon_region *vmxon_kptr = &ARCH_ଗଣ୍ଠି_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    ଯଦି (!vtx_check_fixed_ମୂଲ୍ୟଗୁଡ଼ିକ(read_cr0(), read_cr4())) {
        ଫେରନ୍ତୁ false;
    }
    ଯଦି (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        printf("vt-x: vmxon failure\n");
        ଫେରନ୍ତୁ false;
    }
    memset(&msr_bitmap_region, ~0, ଆକାର(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.ତଳ_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.ତଳ_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.ତଳ_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.ତଳ_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.ତଳ_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.ତଳ_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.ଉପର_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.ଉପର_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.ଉପର_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.ଉପର_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.ଉପର_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.ଉପର_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.ଶବ୍ଦଗୁଡ଼ିକ[0] = x86_rdmsr_ତଳ(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.ଶବ୍ଦଗୁଡ଼ିକ[1] = x86_rdmsr_ଉପର(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    ଯଦି (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        printf("vt-x: Expected wb attribute for EPT paging structure\n");
        ଫେରନ୍ତୁ false;
    }
    ଯଦି (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        printf("vt-x: Expected supported for 2m pages\n");
        ଫେରନ୍ତୁ false;
    }

    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର ଶୂନ୍ୟ setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *buffer;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    buffer = lookupIPCBuffer(true, ଗଣ୍ଠି_STATE(ksCurThread));

    setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_DATA_EXIT_INSTRUCTION_LENGTH));
    setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_DATA_GUEST_PHYSICAL));
    setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    ପାଇଁ (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(ଗଣ୍ଠି_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_EAX + i,
              ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

ସ୍ଥିର ଶୂନ୍ୟ handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(ଗଣ୍ଠି_STATE(ksCurThread), ସନ୍ଦେଶ_Info_Register, SEL4_VMENTER_RESULT_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ finishVmexitSaving(ଶୂନ୍ୟ)
{
    vcpu_t *vcpu = ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentVCPU);
    ପ୍ରତିଜ୍ଞା(vcpu == ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(ଶୂନ୍ୟ)
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
    ଯଦି (reason == EXTERNAL_INTERRUPT) {
        ଗଣ୍ଠି_LOCK_IRQ;
        ଯଦି (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_DATA_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_ଗଣ୍ଠି_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } ଅନ୍ୟଥା {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(ଗଣ୍ଠି_STATE(ksCurThread));
#endif
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }

    ଗଣ୍ଠି_LOCK_SYS;

    ବାଛ (reason) {
    ସ୍ଥିତି EXCEPTION_OR_NMI:
    ସ୍ଥିତି MOV_DR:
    ସ୍ଥିତି TASK_SWITCH:
    ସ୍ଥିତି CONTROL_REGISTER:
    ସ୍ଥିତି IO:
    ସ୍ଥିତି MWAIT:
    ସ୍ଥିତି SIPI:
    ସ୍ଥିତି INVLPG:
    ସ୍ଥିତି INVEPT:
    ସ୍ଥିତି INVVPID:
    ସ୍ଥିତି VMCLEAR:
    ସ୍ଥିତି VMPTRLD:
    ସ୍ଥିତି VMPTRST:
    ସ୍ଥିତି VMREAD:
    ସ୍ଥିତି VMWRITE:
    ସ୍ଥିତି VMXON:
    ସ୍ଥିତି EPT_VIOLATION:
    ସ୍ଥିତି GDTR_OR_IDTR:
    ସ୍ଥିତି LDTR_OR_TR:
    ସ୍ଥିତି TPR_BELOW_THRESHOLD:
    ସ୍ଥିତି APIC_ACCESS:
        qualification = vmread(VMX_DATA_EXIT_QUALIFICATION);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t handleVmEntryFail(ଶୂନ୍ୟ)
{
    handleVmxFault(-1, -1);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
ଶୂନ୍ୟ VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    ପ୍ରତିଜ୍ଞା(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    ଯଦି (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {

        word_t badge = notification_ptr_get_ntfn_ସନ୍ଦେଶ_Identifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        ଯଦି (tcb != ଗଣ୍ଠି_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

ସ୍ଥିର ଶୂନ୍ୟ invvpid_context(uint16_t vpid)
{
    ସଂରଚନା {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    asm ଅସ୍ଥିର("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
}

ସ୍ଥିର ଶୂନ୍ୟ setEPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_ମୂଳ;
    ଯଦି (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_ମୂଳ = kpptr_to_paddr(null_ept_space);
    } ଅନ୍ୟଥା {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        ଯଦି (find_ret.status != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_ମୂଳ = kpptr_to_paddr(null_ept_space);
        } ଅନ୍ୟଥା {
            ept_ମୂଳ = pptr_to_paddr(pml4);
        }
    }
    ଯଦି (ept_ମୂଳ != vcpu->last_ept_ମୂଳ) {
        vcpu->last_ept_ମୂଳ = ept_ମୂଳ;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_ମୂଳ,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.ଶବ୍ଦଗୁଡ଼ିକ[0]);
        ପ୍ରତିଜ୍ଞା(vcpu->vpid != VPID_INVALID);
        ଯଦି (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

ଶୂନ୍ୟ clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t last)
{
    ଯଦି (vpid == VPID_INVALID) {
        ଫେରନ୍ତୁ;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    ଯଦି (vcpu == ଖାଲି) {
        ଫେରନ୍ତୁ;
    }
    ପ୍ରତିଜ୍ଞା(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ vpid_t nextVPID(vpid_t vpid)
{
    ଯଦି (vpid == VPID_LAST) {
        ଫେରନ୍ତୁ VPID_FIRST;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ vpid + 1;
    }
}

ସ୍ଥିର ଶୂନ୍ୟ invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, ଆକାର(vcpu->io));
    /* invalidate the VPID context */
    ଯଦି (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
ସ୍ଥିର ଶୂନ୍ୟ releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = ଖାଲି;
}

ସ୍ଥିର vpid_t findFreeVPID(ଶୂନ୍ୟ)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    କର {
        ଯଦି (x86KSVPIDTable[vpid] == ଖାଲି) {
            ଫେରନ୍ତୁ vpid;
        }
        vpid = nextVPID(vpid);
    } ଯେତେବେଳେ (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = nextVPID(x86KSNextVPID);
    ଫେରନ୍ତୁ vpid;
}

ସ୍ଥିର ଶୂନ୍ୟ storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    ପ୍ରତିଜ୍ଞା(x86KSVPIDTable[vpid] == ଖାଲି);
    ପ୍ରତିଜ୍ଞା(vcpu->vpid == VPID_INVALID);
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
ଶୂନ୍ୟ vcpu_fpu_to_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    ଯଦି (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        ଯଦି (!vcpu->fpu_active) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_active = true;
            loadFpuState(tcb);
        }
    } ଅନ୍ୟଥା {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_active = true;
        switchLocalFpuOwner(tcb);
    }
}

ଶୂନ୍ୟ vcpu_fpu_to_host(tcb_t *tcb, vcpu_t *vcpu)
{
    ଯଦି (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } ଅନ୍ୟଥା ଯଦି (nativeThreadUsingFPU(tcb) && vcpu->fpu_active) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_active = false;
        loadFpuState(tcb);
    } ଅନ୍ୟଥା ଯଦି (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

ଶୂନ୍ୟ restoreVMCS(ଶୂନ୍ୟ)
{
    tcb_t *cur_thread = ଗଣ୍ଠି_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    ଯଦି (ARCH_ଗଣ୍ଠି_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ifndef CONFIG_KERNEL_SKIM_WINDOW
    ଯଦି (getCurrentCR3().ଶବ୍ଦଗୁଡ଼ିକ[0] != expected_vmcs->last_host_cr3) {
        expected_vmcs->last_host_cr3 = getCurrentCR3().ଶବ୍ଦଗୁଡ଼ିକ[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().ଶବ୍ଦଗୁଡ଼ିକ[0]);
    }
#endif
    ଯଦି (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        ଯଦି (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    setEPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

ଶୂନ୍ୟ invept(ept_pml4e_t *ept_pml4)
{
    ଯଦି (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        ସଂରଚନା {
            uint64_t parts[2];
        } address;
        word_t type;
        ଯଦି (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } ଅନ୍ୟଥା ଯଦି (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } ଅନ୍ୟଥା {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            ଫେରନ୍ତୁ;
        }

        address.parts[0] = pptr_to_paddr((ଶୂନ୍ୟ *)ept_pml4);
        address.parts[1] = 0;
        asm ଅସ୍ଥିର(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
