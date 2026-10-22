/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>

#ifdef CONFIG_VTX

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <object/objecttype.h>
#অন্তর্ভাব <arch/machine/cpu_registers.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <arch/object/vcpu.h>
#অন্তর্ভাব <arch/object/ioport.h>
#অন্তর্ভাব <util.h>
#অন্তর্ভাব <sel4/arch/vmenter.h>

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

ধ্রুবক vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

প্রকারলেখ কাঠামো msr_bitmap {
    word_t bitmap[0x2000 / আকার(word_t) / 8];
} msr_bitmap_t;

প্রকারলেখ কাঠামো msr_bitmaps {
    msr_bitmap_t নিচের_msr_read;
    msr_bitmap_t উপরের_msr_read;
    msr_bitmap_t নিচের_msr_write;
    msr_bitmap_t উপরের_msr_write;
} msr_bitmaps_t;

স্থির msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

স্থির অক্ষর null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
স্থির uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
স্থির vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
স্থির uint32_t pin_control_উপরের;
স্থির uint32_t pin_control_নিচের;
স্থির uint32_t primary_control_উপরের;
স্থির uint32_t primary_control_নিচের;
স্থির uint32_t secondary_control_উপরের;
স্থির uint32_t secondary_control_নিচের;
স্থির uint32_t entry_control_উপরের;
স্থির uint32_t entry_control_নিচের;
স্থির uint32_t exit_control_উপরের;
স্থির uint32_t exit_control_নিচের;
স্থির uint32_t cr0_উপরের;
স্থির uint32_t cr0_নিচের;
স্থির uint32_t cr4_উপরের;
স্থির uint32_t cr4_নিচের;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
স্থির bool_t vmx_feature_vpid;
স্থির bool_t vmx_feature_load_perf_global_ctrl;
স্থির bool_t vmx_feature_ack_on_exit;

স্থির vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
স্থির vpid_t x86KSNextVPID = VPID_FIRST;

স্থির শূন্য releaseVPID(vpid_t vpid);

স্থির অন্তঃরেখ bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm অস্থির(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    ফেরত !!error;
}

স্থির শূন্য vmclear(শূন্য *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((শূন্য *)vmcs_ptr);
    asm অস্থির(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

শূন্য clearCurrentVCPU(শূন্য)
{
    vcpu_t *vcpu = ARCH_গ্রন্থি_STATE(x86KSCurrentVCPU);
    যদি (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_গ্রন্থি_STATE(x86KSCurrentVCPU) = ফাঁকা;
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
    asm অস্থির(
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
    যদি (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_গ্রন্থি_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    যদি (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_সূচক()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_সূচক()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_সূচক()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((অক্ষর *)&x86KSGlobalState[CURRENT_CPU_সূচক()].x86KStss.tss.শব্দগুলি[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_গ্রন্থি_STATE(x86KSCurrentVCPU) = vcpu;
}

স্থির শূন্য print_bits(word_t bits)
{
    bool_t first = true;
    যখন (bits) {
        পূর্ণসংখ্যা সূচক = seL4_WordBits - 1 - clzl(bits);
        যদি (first) {
            printf("%d", সূচক);
            first = false;
        } অন্যথা {
            printf(",%d", সূচক);
        }
        bits &= ~BIT(সূচক);
    }
}

স্থির bool_t check_fixed_মান(word_t val, word_t নিচের, word_t উপরের)
{
    word_t not_উপরের;
    word_t not_নিচের;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_উপরের = উপরের & ~(উপরের & val);
    যদি (not_উপরের != 0) {
        printf("Failed to set bits: ");
        print_bits(not_উপরের);
        ফেরত false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_নিচের = ~নিচের & ~(~নিচের & ~val);
    যদি (not_নিচের != 0) {
        printf("Incorrectly cleared bits: ");
        print_bits(not_নিচের);
        ফেরত false;
    }
    ফেরত true;
}

স্থির bool_t vtx_check_fixed_মানসমূহ(word_t cr0, word_t cr4)
{
    যদি (!check_fixed_মান(cr0, cr0_নিচের, cr0_উপরের)) {
        printf(" of CR0\n");
        ফেরত false;
    }
    যদি (!check_fixed_মান(cr4, cr4_নিচের, cr4_উপরের)) {
        printf(" of CR4\n");
        ফেরত false;
    }
    ফেরত true;
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
    pin_control_উপরের = x86_rdmsr_নিচের(pinbased_ctls);
    pin_control_নিচের = x86_rdmsr_উপরের(pinbased_ctls);
    primary_control_উপরের = x86_rdmsr_নিচের(procbased_ctls);
    primary_control_নিচের = x86_rdmsr_উপরের(procbased_ctls);
    secondary_control_উপরের = x86_rdmsr_নিচের(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_নিচের = x86_rdmsr_উপরের(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_উপরের = x86_rdmsr_নিচের(exit_ctls);
    exit_control_নিচের = x86_rdmsr_উপরের(exit_ctls);
    entry_control_উপরের = x86_rdmsr_নিচের(entry_ctls);
    entry_control_নিচের = x86_rdmsr_উপরের(entry_ctls);

    cr0_উপরের = x86_rdmsr_নিচের(IA32_VMX_CR0_FIXED0_MSR);
    cr0_নিচের = x86_rdmsr_নিচের(IA32_VMX_CR0_FIXED1_MSR);
    cr4_উপরের = x86_rdmsr_নিচের(IA32_VMX_CR4_FIXED0_MSR);
    cr4_নিচের = x86_rdmsr_নিচের(IA32_VMX_CR4_FIXED1_MSR);

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
    যদি (secondary_control_নিচের & BIT(7)) {
        cr0_উপরের &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    যদি (!(secondary_control_নিচের & BIT(5))) {
        vmx_feature_vpid = 0;
        printf("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } অন্যথা {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    যদি (!(exit_control_নিচের & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        printf("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } অন্যথা {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    যদি (!(exit_control_নিচের & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        printf("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } অন্যথা {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_নিচের) & pin_control_mask;
    যদি (missing) {
        printf("vt-x: Unsupported pin control features %lx\n", (long)missing);
        ফেরত false;
    }
    missing = (~primary_control_নিচের) & primary_control_mask;
    যদি (missing) {
        printf("vt-x: Unsupported primary control features %lx\n", (long)missing);
        ফেরত false;
    }
    missing = (~secondary_control_নিচের) & secondary_control_mask;
    যদি (missing) {
        printf("vt-x: Unsupported secondary control features %lx\n", (long)missing);
        ফেরত false;
    }
    missing = (~exit_control_নিচের) & exit_control_mask;
    যদি (missing) {
        printf("vt-x: Unsupported exit control features %lx\n", (long)missing);
        ফেরত false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_নিচের) & BIT(9);
    যদি (missing) {
        printf("vt-x: CPU does not support 64-bit guests\n");
        ফেরত false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_উপরের |= pin_control_mask;
    primary_control_উপরের |= primary_control_mask;
    secondary_control_উপরের |= secondary_control_mask;
    exit_control_উপরের |= exit_control_mask;

    ফেরত true;
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
    uint32_t local_pin_control_উপরের = x86_rdmsr_নিচের(pinbased_ctls);
    uint32_t local_pin_control_নিচের = x86_rdmsr_উপরের(pinbased_ctls);
    uint32_t local_primary_control_উপরের = x86_rdmsr_নিচের(procbased_ctls);
    uint32_t local_primary_control_নিচের = x86_rdmsr_উপরের(procbased_ctls);
    uint32_t local_secondary_control_উপরের = x86_rdmsr_নিচের(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_নিচের = x86_rdmsr_উপরের(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_উপরের = x86_rdmsr_নিচের(exit_ctls);
    uint32_t local_exit_control_নিচের = x86_rdmsr_উপরের(exit_ctls);
    uint32_t local_entry_control_উপরের = x86_rdmsr_নিচের(entry_ctls);
    uint32_t local_entry_control_নিচের = x86_rdmsr_উপরের(entry_ctls);

    uint32_t local_cr0_উপরের = x86_rdmsr_নিচের(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_নিচের = x86_rdmsr_নিচের(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_উপরের = x86_rdmsr_নিচের(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_নিচের = x86_rdmsr_নিচের(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    যদি (local_secondary_control_নিচের & BIT(7)) {
        local_cr0_উপরের &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    ফেরত
        (local_pin_control_উপরের & pin_control_উপরের) == local_pin_control_উপরের &&
        (~local_pin_control_নিচের & pin_control_উপরের) == 0 &&
        (local_primary_control_উপরের & primary_control_উপরের) == local_primary_control_উপরের &&
        (~local_primary_control_নিচের & primary_control_উপরের) == 0 &&
        (local_secondary_control_উপরের & secondary_control_উপরের) == local_secondary_control_উপরের &&
        (~local_secondary_control_নিচের & secondary_control_উপরের) == 0 &&
        (local_exit_control_উপরের & exit_control_উপরের) == local_exit_control_উপরের &&
        (~local_exit_control_নিচের & exit_control_উপরের) == 0 &&
        (local_entry_control_উপরের & entry_control_উপরের) == local_entry_control_উপরের &&
        (~local_entry_control_নিচের & entry_control_উপরের) == 0 &&
        local_cr0_উপরের == cr0_উপরের &&
        local_cr0_নিচের == cr0_নিচের &&
        local_cr4_উপরের == cr4_উপরের &&
        local_cr4_নিচের == cr4_নিচের;
}

স্থির অন্তঃরেখ uint32_t applyFixedBits(uint32_t original, uint32_t উপরের, uint32_t নিচের)
{
    original |= উপরের;
    original &= নিচের;
    ফেরত original;
}

শূন্য vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = ফাঁকা;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    memcpy(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_উপরের & cr0_নিচের;
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
    vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_সূচক()].x86KStss);
    vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_সূচক()].x86KSgdt);
    vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_সূচক()].x86KSidt);
    vmwrite(VMX_HOST_SYSENTER_CS, (word_t)SEL_CS_0);
    vmwrite(VMX_HOST_SYSENTER_EIP, (word_t)&handle_syscall);
    যদি (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((অক্ষর *)&x86KSGlobalState[CURRENT_CPU_সূচক()].x86KStss.tss.শব্দগুলি[0] +
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
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_উপরের & pin_control_নিচের);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_উপরের & primary_control_নিচের);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_উপরের & secondary_control_নিচের);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_উপরের & exit_control_নিচের);

    uint64_t entry_control = entry_control_উপরের & entry_control_নিচের;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_উপরের & cr4_নিচের);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_উপরের, ~(word_t)0);

    memset(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_আকার);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((অক্ষর *)vcpu->io + (VCPU_IOBITMAP_আকার / 2)));
}

স্থির শূন্য dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    প্রতিজ্ঞা(tcb->tcbArch.tcbVCPU == vcpu);
    প্রতিজ্ঞা(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = ফাঁকা;
    vcpu->vcpuTCB = ফাঁকা;
}

শূন্য vcpu_finalise(vcpu_t *vcpu)
{
    যদি (ARCH_গ্রন্থি_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
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

স্থির exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *বাফার)
{
    পূর্ণসংখ্যা i;
    জন্য (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, বাফার);
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t decodeVCPUWriteRegisters(cap_t cap, word_t দৈর্ঘ্য, word_t *বাফার)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    যদি (দৈর্ঘ্য < 15) {
#অন্যথা
    যদি (দৈর্ঘ্য < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    ফেরত invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), বাফার);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
স্থির exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *বাফার, word_t field, word_t মান)
{
    tcb_t *thread;
    thread = গ্রন্থি_STATE(ksCurThread);
    যদি (ARCH_গ্রন্থি_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    নির্বাচন (field) {
    ক্ষেত্র IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = মান;
        বিরতি;
    ক্ষেত্র IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = মান;
        বিরতি;
    ক্ষেত্র IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = মান;
        বিরতি;
    ক্ষেত্র IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = মান;
        বিরতি;
    }
    setMR(thread, বাফার, 0, মান);
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t decodeVCPUWriteMSR(cap_t cap, word_t দৈর্ঘ্য, word_t *বাফার)
{
    word_t field;
    word_t মান;

    যদি (দৈর্ঘ্য < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, বাফার);
    মান = getSyscallArg(1, বাফার);
    নির্বাচন (field) {
    ক্ষেত্র IA32_LSTAR_MSR:
    ক্ষেত্র IA32_STAR_MSR:
    ক্ষেত্র IA32_CSTAR_MSR:
    ক্ষেত্র IA32_FMASK_MSR:
        বিরতি;
    পূর্বনির্ধারিত:
        userError("VCPU WriteMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    ফেরত invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), বাফার, field, মান);
}

স্থির exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *বাফার)
{
    tcb_t *thread;
    thread = গ্রন্থি_STATE(ksCurThread);

    word_t মান = 0;

    নির্বাচন (field) {
    ক্ষেত্র IA32_LSTAR_MSR:
        মান = vcpu->syscall_registers[VCPU_LSTAR];
        বিরতি;
    ক্ষেত্র IA32_STAR_MSR:
        মান = vcpu->syscall_registers[VCPU_STAR];
        বিরতি;
    ক্ষেত্র IA32_CSTAR_MSR:
        মান = vcpu->syscall_registers[VCPU_CSTAR];
        বিরতি;
    ক্ষেত্র IA32_FMASK_MSR:
        মান = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        বিরতি;
    ক্ষেত্র IA32_VMX_MISC_MSR:
        মান = x86_rdmsr(field);
        বিরতি;
    }

    setMR(thread, বাফার, 0, মান);
    setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t decodeVCPUReadMSR(cap_t cap, word_t দৈর্ঘ্য, word_t *বাফার)
{
    যদি (দৈর্ঘ্য < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, বাফার);
    নির্বাচন (field) {
    ক্ষেত্র IA32_LSTAR_MSR:
    ক্ষেত্র IA32_STAR_MSR:
    ক্ষেত্র IA32_CSTAR_MSR:
    ক্ষেত্র IA32_FMASK_MSR:
    ক্ষেত্র IA32_VMX_MISC_MSR:
        বিরতি;
    পূর্বনির্ধারিত:
        userError("VCPU ReadMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    ফেরত invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, বাফার);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

স্থির exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t নিচের, uint16_t উপরের)
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
    setIOPortMask(vcpu->io, নিচের, উপরের, false);
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t decodeEnableIOPort(cap_t cap, word_t দৈর্ঘ্য, word_t *বাফার)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t নিচের, উপরের;

    যদি (দৈর্ঘ্য < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (current_extra_caps.excaprefs[0] == ফাঁকা) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    নিচের = getSyscallArg(0, বাফার);
    উপরের = getSyscallArg(1, বাফার);

    যদি (নিচের < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || উপরের > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 0;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত invokeEnableIOPort(vcpu, ioSlot, ioCap, নিচের, উপরের);
}

স্থির exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t নিচের, uint16_t উপরের)
{
    setIOPortMask(vcpu->io, নিচের, উপরের, true);
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t decodeDisableIOPort(cap_t cap, word_t দৈর্ঘ্য, word_t *বাফার)
{
    vcpu_t *vcpu;
    uint16_t নিচের, উপরের;

    যদি (দৈর্ঘ্য < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    নিচের = getSyscallArg(0, বাফার);
    উপরের = getSyscallArg(1, বাফার);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ফেরত invokeDisableIOPort(vcpu, নিচের, উপরের);
}

স্থির exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *বাফার, word_t field, word_t মান)
{
    tcb_t *thread;
    thread = গ্রন্থি_STATE(ksCurThread);

    যদি (ARCH_গ্রন্থি_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    নির্বাচন (field) {
    ক্ষেত্র VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = মান;
        বিরতি;
    ক্ষেত্র VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = মান;
        বিরতি;
    ক্ষেত্র VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = মান;
        বিরতি;
    ক্ষেত্র VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = মান;
        বিরতি;
    }
    vmwrite(field, মান);

    যদি (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned পূর্ণসংখ্যা দৈর্ঘ্য = setMR(thread, বাফার, 0, মান);
        setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, দৈর্ঘ্য)));
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t decodeWriteVMCS(cap_t cap, word_t দৈর্ঘ্য, bool_t call, word_t *বাফার)
{
    word_t field;
    word_t মান;

    যদি (দৈর্ঘ্য < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, বাফার);
    মান = getSyscallArg(1, বাফার);
    নির্বাচন (field) {
    ক্ষেত্র VMX_GUEST_RIP:
    ক্ষেত্র VMX_GUEST_RSP:
    ক্ষেত্র VMX_GUEST_ES_SELECTOR:
    ক্ষেত্র VMX_GUEST_CS_SELECTOR:
    ক্ষেত্র VMX_GUEST_SS_SELECTOR:
    ক্ষেত্র VMX_GUEST_DS_SELECTOR:
    ক্ষেত্র VMX_GUEST_FS_SELECTOR:
    ক্ষেত্র VMX_GUEST_GS_SELECTOR:
    ক্ষেত্র VMX_GUEST_LDTR_SELECTOR:
    ক্ষেত্র VMX_GUEST_TR_SELECTOR:
    ক্ষেত্র VMX_GUEST_DEBUGCTRL:
    ক্ষেত্র VMX_GUEST_PAT:
    ক্ষেত্র VMX_GUEST_EFER:
    ক্ষেত্র VMX_GUEST_PERF_GLOBAL_CTRL:
    ক্ষেত্র VMX_GUEST_PDPTE0:
    ক্ষেত্র VMX_GUEST_PDPTE1:
    ক্ষেত্র VMX_GUEST_PDPTE2:
    ক্ষেত্র VMX_GUEST_PDPTE3:
    ক্ষেত্র VMX_GUEST_ES_LIMIT:
    ক্ষেত্র VMX_GUEST_CS_LIMIT:
    ক্ষেত্র VMX_GUEST_SS_LIMIT:
    ক্ষেত্র VMX_GUEST_DS_LIMIT:
    ক্ষেত্র VMX_GUEST_FS_LIMIT:
    ক্ষেত্র VMX_GUEST_GS_LIMIT:
    ক্ষেত্র VMX_GUEST_LDTR_LIMIT:
    ক্ষেত্র VMX_GUEST_TR_LIMIT:
    ক্ষেত্র VMX_GUEST_GDTR_LIMIT:
    ক্ষেত্র VMX_GUEST_IDTR_LIMIT:
    ক্ষেত্র VMX_GUEST_ES_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_CS_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_SS_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_DS_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_FS_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_GS_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_LDTR_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_TR_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_INTERRUPTABILITY:
    ক্ষেত্র VMX_GUEST_ACTIVITY:
    ক্ষেত্র VMX_GUEST_SMBASE:
    ক্ষেত্র VMX_GUEST_SYSENTER_CS:
    ক্ষেত্র VMX_GUEST_PREEMPTION_TIMER_মান:
    ক্ষেত্র VMX_GUEST_ES_BASE:
    ক্ষেত্র VMX_GUEST_CS_BASE:
    ক্ষেত্র VMX_GUEST_SS_BASE:
    ক্ষেত্র VMX_GUEST_DS_BASE:
    ক্ষেত্র VMX_GUEST_FS_BASE:
    ক্ষেত্র VMX_GUEST_GS_BASE:
    ক্ষেত্র VMX_GUEST_LDTR_BASE:
    ক্ষেত্র VMX_GUEST_TR_BASE:
    ক্ষেত্র VMX_GUEST_GDTR_BASE:
    ক্ষেত্র VMX_GUEST_IDTR_BASE:
    ক্ষেত্র VMX_GUEST_DR7:
    ক্ষেত্র VMX_GUEST_RFLAGS:
    ক্ষেত্র VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    ক্ষেত্র VMX_GUEST_SYSENTER_ESP:
    ক্ষেত্র VMX_GUEST_SYSENTER_EIP:
    ক্ষেত্র VMX_CONTROL_CR0_MASK:
    ক্ষেত্র VMX_CONTROL_CR4_MASK:
    ক্ষেত্র VMX_CONTROL_CR0_READ_SHADOW:
    ক্ষেত্র VMX_CONTROL_CR4_READ_SHADOW:
    ক্ষেত্র VMX_GUEST_CR3:
    ক্ষেত্র VMX_CONTROL_EXCEPTION_BITMAP:
    ক্ষেত্র VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    ক্ষেত্র VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        বিরতি;
    ক্ষেত্র VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        মান = applyFixedBits(মান, pin_control_উপরের, pin_control_নিচের);
        বিরতি;
    ক্ষেত্র VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        মান = applyFixedBits(মান, primary_control_উপরের, primary_control_নিচের);
        বিরতি;
    ক্ষেত্র VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        মান = applyFixedBits(মান, secondary_control_উপরের, secondary_control_নিচের);
        বিরতি;
    ক্ষেত্র VMX_CONTROL_ENTRY_CONTROLS:
        মান = applyFixedBits(মান, entry_control_উপরের, entry_control_নিচের);
        বিরতি;
    ক্ষেত্র VMX_CONTROL_EXIT_CONTROLS:
        মান = applyFixedBits(মান, exit_control_উপরের, exit_control_নিচের);
        বিরতি;
    ক্ষেত্র VMX_GUEST_CR0:
        মান = applyFixedBits(মান, cr0_উপরের, cr0_নিচের);
        বিরতি;
    ক্ষেত্র VMX_GUEST_CR4:
        মান = applyFixedBits(মান, cr4_উপরের, cr4_নিচের);
        বিরতি;
    পূর্বনির্ধারিত:
        userError("VCPU WriteVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    ফেরত invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, বাফার, field, মান);
}

স্থির word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    নির্বাচন (field) {
    ক্ষেত্র VMX_CONTROL_EXCEPTION_BITMAP:
        ফেরত vcpu->exception_bitmap;
    ক্ষেত্র VMX_GUEST_CR0:
        ফেরত vcpu->cr0;
    ক্ষেত্র VMX_CONTROL_CR0_MASK:
        ফেরত vcpu->cr0_mask;
    ক্ষেত্র VMX_CONTROL_CR0_READ_SHADOW:
        ফেরত vcpu->cr0_shadow;
    }
    যদি (ARCH_গ্রন্থি_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ফেরত vmread(field);
}

স্থির exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *বাফার)
{
    tcb_t *thread;
    thread = গ্রন্থি_STATE(ksCurThread);
    word_t মান = readVMCSField(vcpu, field);
    যদি (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned পূর্ণসংখ্যা দৈর্ঘ্য = setMR(thread, বাফার, 0, মান);
        setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, দৈর্ঘ্য)));
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t decodeReadVMCS(cap_t cap, word_t দৈর্ঘ্য, bool_t call, word_t *বাফার)
{
    যদি (দৈর্ঘ্য < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, বাফার);
    নির্বাচন (field) {
    ক্ষেত্র VMX_GUEST_RIP:
    ক্ষেত্র VMX_GUEST_RSP:
    ক্ষেত্র VMX_GUEST_ES_SELECTOR:
    ক্ষেত্র VMX_GUEST_CS_SELECTOR:
    ক্ষেত্র VMX_GUEST_SS_SELECTOR:
    ক্ষেত্র VMX_GUEST_DS_SELECTOR:
    ক্ষেত্র VMX_GUEST_FS_SELECTOR:
    ক্ষেত্র VMX_GUEST_GS_SELECTOR:
    ক্ষেত্র VMX_GUEST_LDTR_SELECTOR:
    ক্ষেত্র VMX_GUEST_TR_SELECTOR:
    ক্ষেত্র VMX_GUEST_DEBUGCTRL:
    ক্ষেত্র VMX_GUEST_PAT:
    ক্ষেত্র VMX_GUEST_EFER:
    ক্ষেত্র VMX_GUEST_PERF_GLOBAL_CTRL:
    ক্ষেত্র VMX_GUEST_PDPTE0:
    ক্ষেত্র VMX_GUEST_PDPTE1:
    ক্ষেত্র VMX_GUEST_PDPTE2:
    ক্ষেত্র VMX_GUEST_PDPTE3:
    ক্ষেত্র VMX_GUEST_ES_LIMIT:
    ক্ষেত্র VMX_GUEST_CS_LIMIT:
    ক্ষেত্র VMX_GUEST_SS_LIMIT:
    ক্ষেত্র VMX_GUEST_DS_LIMIT:
    ক্ষেত্র VMX_GUEST_FS_LIMIT:
    ক্ষেত্র VMX_GUEST_GS_LIMIT:
    ক্ষেত্র VMX_GUEST_LDTR_LIMIT:
    ক্ষেত্র VMX_GUEST_TR_LIMIT:
    ক্ষেত্র VMX_GUEST_GDTR_LIMIT:
    ক্ষেত্র VMX_GUEST_IDTR_LIMIT:
    ক্ষেত্র VMX_GUEST_ES_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_CS_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_SS_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_DS_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_FS_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_GS_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_LDTR_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_TR_ACCESS_RIGHTS:
    ক্ষেত্র VMX_GUEST_INTERRUPTABILITY:
    ক্ষেত্র VMX_GUEST_ACTIVITY:
    ক্ষেত্র VMX_GUEST_SMBASE:
    ক্ষেত্র VMX_GUEST_SYSENTER_CS:
    ক্ষেত্র VMX_GUEST_PREEMPTION_TIMER_মান:
    ক্ষেত্র VMX_GUEST_ES_BASE:
    ক্ষেত্র VMX_GUEST_CS_BASE:
    ক্ষেত্র VMX_GUEST_SS_BASE:
    ক্ষেত্র VMX_GUEST_DS_BASE:
    ক্ষেত্র VMX_GUEST_FS_BASE:
    ক্ষেত্র VMX_GUEST_GS_BASE:
    ক্ষেত্র VMX_GUEST_LDTR_BASE:
    ক্ষেত্র VMX_GUEST_TR_BASE:
    ক্ষেত্র VMX_GUEST_GDTR_BASE:
    ক্ষেত্র VMX_GUEST_IDTR_BASE:
    ক্ষেত্র VMX_GUEST_DR7:
    ক্ষেত্র VMX_GUEST_RFLAGS:
    ক্ষেত্র VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    ক্ষেত্র VMX_GUEST_SYSENTER_ESP:
    ক্ষেত্র VMX_GUEST_SYSENTER_EIP:
    ক্ষেত্র VMX_CONTROL_CR0_MASK:
    ক্ষেত্র VMX_CONTROL_CR4_MASK:
    ক্ষেত্র VMX_CONTROL_CR0_READ_SHADOW:
    ক্ষেত্র VMX_CONTROL_CR4_READ_SHADOW:
    ক্ষেত্র VMX_তথ্য_INSTRUCTION_ERROR:
    ক্ষেত্র VMX_তথ্য_EXIT_INTERRUPT_INFO:
    ক্ষেত্র VMX_তথ্য_EXIT_INTERRUPT_ERROR:
    ক্ষেত্র VMX_তথ্য_IDT_VECTOR_INFO:
    ক্ষেত্র VMX_তথ্য_IDT_VECTOR_ERROR:
    ক্ষেত্র VMX_তথ্য_EXIT_INSTRUCTION_দৈর্ঘ্য:
    ক্ষেত্র VMX_তথ্য_EXIT_INSTRUCTION_INFO:
    ক্ষেত্র VMX_তথ্য_GUEST_PHYSICAL:
    ক্ষেত্র VMX_তথ্য_IO_RCX:
    ক্ষেত্র VMX_তথ্য_IO_RSI:
    ক্ষেত্র VMX_তথ্য_IO_RDI:
    ক্ষেত্র VMX_তথ্য_IO_RIP:
    ক্ষেত্র VMX_তথ্য_GUEST_LINEAR_ADDRESS:
    ক্ষেত্র VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    ক্ষেত্র VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    ক্ষেত্র VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    ক্ষেত্র VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    ক্ষেত্র VMX_CONTROL_EXCEPTION_BITMAP:
    ক্ষেত্র VMX_CONTROL_ENTRY_CONTROLS:
    ক্ষেত্র VMX_CONTROL_EXIT_CONTROLS:
    ক্ষেত্র VMX_GUEST_CR0:
    ক্ষেত্র VMX_GUEST_CR3:
    ক্ষেত্র VMX_GUEST_CR4:
        বিরতি;
    পূর্বনির্ধারিত:
        userError("VCPU ReadVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    ফেরত invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, বাফার);
}

স্থির exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t decodeSetTCB(cap_t cap, word_t দৈর্ঘ্য, word_t *বাফার)
{
    cap_t tcbCap;
    যদি (current_extra_caps.excaprefs[0] == ফাঁকা) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    যদি (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    ফেরত invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

শূন্য vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *বাফার;
    যদি (ARCH_গ্রন্থি_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    বাফার = lookupIPCBuffer(false, গ্রন্থি_STATE(ksCurThread));
    যদি (!বাফার) {
        userError("No IPC buffer.");
        ফেরত;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, বাফার));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, বাফার), primary_control_উপরের,
                                                                   primary_control_নিচের));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, বাফার));
}

শূন্য vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *বাফার;
    vcpu_t *vcpu;

    বাফার = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    প্রতিজ্ঞা(vcpu);

    যদি (ARCH_গ্রন্থি_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, বাফার, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, বাফার, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, বাফার, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, বার্তা_Info_Register, 0);
}

exception_t decodeX86VCPUInvocation(
    word_t invLabel,
    word_t দৈর্ঘ্য,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *বাফার
)
{
    নির্বাচন (invLabel) {
    ক্ষেত্র X86VCPUSetTCB:
        ফেরত decodeSetTCB(cap, দৈর্ঘ্য, বাফার);
    ক্ষেত্র X86VCPUReadVMCS:
        ফেরত decodeReadVMCS(cap, দৈর্ঘ্য, call, বাফার);
    ক্ষেত্র X86VCPUWriteVMCS:
        ফেরত decodeWriteVMCS(cap, দৈর্ঘ্য, call, বাফার);
    ক্ষেত্র X86VCPUEnableIOPort:
        ফেরত decodeEnableIOPort(cap, দৈর্ঘ্য, বাফার);
    ক্ষেত্র X86VCPUDisableIOPort:
        ফেরত decodeDisableIOPort(cap, দৈর্ঘ্য, বাফার);
    ক্ষেত্র X86VCPUWriteRegisters:
        ফেরত decodeVCPUWriteRegisters(cap, দৈর্ঘ্য, বাফার);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    ক্ষেত্র X86VCPUWriteMSR:
        ফেরত decodeVCPUWriteMSR(cap, দৈর্ঘ্য, বাফার);
    ক্ষেত্র X86VCPUReadMSR:
        ফেরত decodeVCPUReadMSR(cap, দৈর্ঘ্য, বাফার);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    পূর্বনির্ধারিত:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
}

স্থির bool_t is_vtx_supported(শূন্য)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    ফেরত !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

স্থির অন্তঃরেখ শূন্য clear_bit(word_t *bitmap, word_t bit)
{
    পূর্ণসংখ্যা সূচক = bit / (আকার(word_t) * 8);
    পূর্ণসংখ্যা offset = bit % (আকার(word_t) * 8);
    bitmap[সূচক] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(শূন্য)
{
    যদি (!is_vtx_supported()) {
        printf("vt-x: not supported\n");
        ফেরত false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.শব্দগুলি[0] = x86_rdmsr_নিচের(IA32_VMX_BASIC_MSR);
    vmx_basic.শব্দগুলি[1] = x86_rdmsr_উপরের(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.শব্দগুলি[0] = x86_rdmsr_নিচের(IA32_FEATURE_CONTROL_MSR);
    যদি (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        যদি (feature_control_msr_get_lock(feature_control)) {
            printf("vt-x: feature locked\n");
            ফেরত false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_উপরের(IA32_FEATURE_CONTROL_MSR), feature_control.শব্দগুলি[0]);
    }
    /* make sure the msr is locked */
    যদি (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_উপরের(IA32_FEATURE_CONTROL_MSR), feature_control.শব্দগুলি[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    যদি (CURRENT_CPU_সূচক() == 0) {
        যদি (!init_vtx_fixed_মানসমূহ(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            printf("vt-x: lack of required features\n");
            ফেরত false;
        }
    }
    যদি (!check_vtx_fixed_মানসমূহ(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        printf("vt-x: cores have inconsistent features\n");
        ফেরত false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    কাঠামো vmxon_region *vmxon_kptr = &ARCH_গ্রন্থি_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    যদি (!vtx_check_fixed_মানসমূহ(read_cr0(), read_cr4())) {
        ফেরত false;
    }
    যদি (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        printf("vt-x: vmxon failure\n");
        ফেরত false;
    }
    memset(&msr_bitmap_region, ~0, আকার(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.নিচের_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.নিচের_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.নিচের_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.নিচের_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.নিচের_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.নিচের_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.উপরের_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.উপরের_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.উপরের_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.উপরের_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.উপরের_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.উপরের_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.শব্দগুলি[0] = x86_rdmsr_নিচের(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.শব্দগুলি[1] = x86_rdmsr_উপরের(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    যদি (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        printf("vt-x: Expected wb attribute for EPT paging structure\n");
        ফেরত false;
    }
    যদি (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        printf("vt-x: Expected supported for 2m pages\n");
        ফেরত false;
    }

    ফেরত true;
}

স্থির শূন্য setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *বাফার;
    পূর্ণসংখ্যা i;

    বাফার = lookupIPCBuffer(true, গ্রন্থি_STATE(ksCurThread));

    setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_তথ্য_EXIT_INSTRUCTION_দৈর্ঘ্য));
    setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_তথ্য_GUEST_PHYSICAL));
    setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    জন্য (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(গ্রন্থি_STATE(ksCurThread), বাফার, SEL4_VMENTER_FAULT_EAX + i,
              গ্রন্থি_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

স্থির শূন্য handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(গ্রন্থি_STATE(ksCurThread), বার্তা_Info_Register, SEL4_VMENTER_ফলাফল_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

স্থির অন্তঃরেখ শূন্য finishVmexitSaving(শূন্য)
{
    vcpu_t *vcpu = ARCH_গ্রন্থি_STATE(x86KSCurrentVCPU);
    প্রতিজ্ঞা(vcpu == গ্রন্থি_STATE(ksCurThread)->tcbArch.tcbVCPU);
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
    reason = vmread(VMX_তথ্য_EXIT_REASON) & MASK(16);
    যদি (reason == EXTERNAL_INTERRUPT) {
        গ্রন্থি_LOCK_IRQ;
        যদি (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_তথ্য_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_গ্রন্থি_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } অন্যথা {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(গ্রন্থি_STATE(ksCurThread));
#endif
        ফেরত EXCEPTION_NONE;
    }

    গ্রন্থি_LOCK_SYS;

    নির্বাচন (reason) {
    ক্ষেত্র EXCEPTION_OR_NMI:
    ক্ষেত্র MOV_DR:
    ক্ষেত্র TASK_SWITCH:
    ক্ষেত্র CONTROL_REGISTER:
    ক্ষেত্র IO:
    ক্ষেত্র MWAIT:
    ক্ষেত্র SIPI:
    ক্ষেত্র INVLPG:
    ক্ষেত্র INVEPT:
    ক্ষেত্র INVVPID:
    ক্ষেত্র VMCLEAR:
    ক্ষেত্র VMPTRLD:
    ক্ষেত্র VMPTRST:
    ক্ষেত্র VMREAD:
    ক্ষেত্র VMWRITE:
    ক্ষেত্র VMXON:
    ক্ষেত্র EPT_VIOLATION:
    ক্ষেত্র GDTR_OR_IDTR:
    ক্ষেত্র LDTR_OR_TR:
    ক্ষেত্র TPR_BELOW_THRESHOLD:
    ক্ষেত্র APIC_ACCESS:
        qualification = vmread(VMX_তথ্য_EXIT_QUALIFICATION);
        বিরতি;
    পূর্বনির্ধারিত:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    ফেরত EXCEPTION_NONE;
}

exception_t handleVmEntryFail(শূন্য)
{
    handleVmxFault(-1, -1);

    ফেরত EXCEPTION_NONE;
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

        word_t badge = notification_ptr_get_ntfn_বার্তা_Identifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        যদি (tcb != গ্রন্থি_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

স্থির শূন্য invvpid_context(uint16_t vpid)
{
    কাঠামো {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    asm অস্থির("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
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
        ফেরত;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    যদি (vcpu == ফাঁকা) {
        ফেরত;
    }
    প্রতিজ্ঞা(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

স্থির অন্তঃরেখ vpid_t পরবর্তী_VPID(vpid_t vpid)
{
    যদি (vpid == VPID_LAST) {
        ফেরত VPID_FIRST;
    } অন্যথা {
        ফেরত vpid + 1;
    }
}

স্থির শূন্য invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, আকার(vcpu->io));
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
    x86KSVPIDTable[vpid] = ফাঁকা;
}

স্থির vpid_t findFreeVPID(শূন্য)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    করো {
        যদি (x86KSVPIDTable[vpid] == ফাঁকা) {
            ফেরত vpid;
        }
        vpid = পরবর্তী_VPID(vpid);
    } যখন (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = পরবর্তী_VPID(x86KSNextVPID);
    ফেরত vpid;
}

স্থির শূন্য storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    প্রতিজ্ঞা(x86KSVPIDTable[vpid] == ফাঁকা);
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
    tcb_t *cur_thread = গ্রন্থি_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    যদি (ARCH_গ্রন্থি_STATE(x86KSCurrentVCPU) != expected_vmcs) {
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
        কাঠামো {
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
            ফেরত;
        }

        address.parts[0] = pptr_to_paddr((শূন্য *)ept_pml4);
        address.parts[1] = 0;
        asm অস্থির(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
