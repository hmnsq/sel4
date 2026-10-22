/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>

#ifdef CONFIG_VTX

#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <object/objecttype.h>
#അന്തര്ഭാവ <arch/machine/cpu_registers.h>
#അന്തര്ഭാവ <arch/model/statedata.h>
#അന്തര്ഭാവ <arch/object/vcpu.h>
#അന്തര്ഭാവ <arch/object/ioport.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <sel4/arch/vmenter.h>

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

സ്ഥിരം vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

തരനിർവചനം ഘടന msr_bitmap {
    word_t bitmap[0x2000 / വലുപ്പം(word_t) / 8];
} msr_bitmap_t;

തരനിർവചനം ഘടന msr_bitmaps {
    msr_bitmap_t താഴെയുള്ള_msr_read;
    msr_bitmap_t മുകളിലെ_msr_read;
    msr_bitmap_t താഴെയുള്ള_msr_write;
    msr_bitmap_t മുകളിലെ_msr_write;
} msr_bitmaps_t;

സ്ഥിതം msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

സ്ഥിതം അക്ഷരം null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
സ്ഥിതം uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
സ്ഥിതം vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
സ്ഥിതം uint32_t pin_control_മുകളിലെ;
സ്ഥിതം uint32_t pin_control_താഴെയുള്ള;
സ്ഥിതം uint32_t primary_control_മുകളിലെ;
സ്ഥിതം uint32_t primary_control_താഴെയുള്ള;
സ്ഥിതം uint32_t secondary_control_മുകളിലെ;
സ്ഥിതം uint32_t secondary_control_താഴെയുള്ള;
സ്ഥിതം uint32_t entry_control_മുകളിലെ;
സ്ഥിതം uint32_t entry_control_താഴെയുള്ള;
സ്ഥിതം uint32_t exit_control_മുകളിലെ;
സ്ഥിതം uint32_t exit_control_താഴെയുള്ള;
സ്ഥിതം uint32_t cr0_മുകളിലെ;
സ്ഥിതം uint32_t cr0_താഴെയുള്ള;
സ്ഥിതം uint32_t cr4_മുകളിലെ;
സ്ഥിതം uint32_t cr4_താഴെയുള്ള;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
സ്ഥിതം bool_t vmx_feature_vpid;
സ്ഥിതം bool_t vmx_feature_load_perf_global_ctrl;
സ്ഥിതം bool_t vmx_feature_ack_on_exit;

സ്ഥിതം vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
സ്ഥിതം vpid_t x86KSNextVPID = VPID_FIRST;

സ്ഥിതം ശൂന്യം releaseVPID(vpid_t vpid);

സ്ഥിതം വരിയിൽ bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm അസ്ഥിര(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    തിരികെ !!error;
}

സ്ഥിതം ശൂന്യം vmclear(ശൂന്യം *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((ശൂന്യം *)vmcs_ptr);
    asm അസ്ഥിര(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

ശൂന്യം clearCurrentVCPU(ശൂന്യം)
{
    vcpu_t *vcpu = ARCH_കണ്ണി_STATE(x86KSCurrentVCPU);
    എങ്കിൽ (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_കണ്ണി_STATE(x86KSCurrentVCPU) = ശൂന്യ;
        എങ്കിൽ (vcpu->fpu_active && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_active = false;
        }
    }
}

സ്ഥിതം ശൂന്യം vmptrld(ശൂന്യം *vmcs_ptr)
{
    uint64_t physical_address;
    uint8_t error;
    physical_address = pptr_to_paddr(vmcs_ptr);
    asm അസ്ഥിര(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_address)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    പ്രതിജ്ഞാ(!error);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
ശൂന്യം vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_STAR_MSR, vcpu->syscall_registers[VCPU_STAR]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_MASK]);
}

ശൂന്യം vcpu_restore_host_msrs(ശൂന്യം)
{
    init_syscall_msrs();
}
#endif

സ്ഥിതം ശൂന്യം switchVCPU(vcpu_t *vcpu)
{
#ifdef ENABLE_SMP_SUPPORT
    എങ്കിൽ (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_കണ്ണി_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    എങ്കിൽ (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((അക്ഷരം *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.വാക്കുകൾ[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_കണ്ണി_STATE(x86KSCurrentVCPU) = vcpu;
}

സ്ഥിതം ശൂന്യം print_bits(word_t bits)
{
    bool_t first = true;
    ആകുമ്പോൾ (bits) {
        പൂർണ്ണസംഖ്യ index = seL4_WordBits - 1 - clzl(bits);
        എങ്കിൽ (first) {
            printf("%d", index);
            first = false;
        } അല്ലെങ്കിൽ {
            printf(",%d", index);
        }
        bits &= ~BIT(index);
    }
}

സ്ഥിതം bool_t check_fixed_മൂല്യം(word_t val, word_t താഴെയുള്ള, word_t മുകളിലെ)
{
    word_t not_മുകളിലെ;
    word_t not_താഴെയുള്ള;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_മുകളിലെ = മുകളിലെ & ~(മുകളിലെ & val);
    എങ്കിൽ (not_മുകളിലെ != 0) {
        printf("Failed to set bits: ");
        print_bits(not_മുകളിലെ);
        തിരികെ false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_താഴെയുള്ള = ~താഴെയുള്ള & ~(~താഴെയുള്ള & ~val);
    എങ്കിൽ (not_താഴെയുള്ള != 0) {
        printf("Incorrectly cleared bits: ");
        print_bits(not_താഴെയുള്ള);
        തിരികെ false;
    }
    തിരികെ true;
}

സ്ഥിതം bool_t vtx_check_fixed_മൂല്യങ്ങൾ(word_t cr0, word_t cr4)
{
    എങ്കിൽ (!check_fixed_മൂല്യം(cr0, cr0_താഴെയുള്ള, cr0_മുകളിലെ)) {
        printf(" of CR0\n");
        തിരികെ false;
    }
    എങ്കിൽ (!check_fixed_മൂല്യം(cr4, cr4_താഴെയുള്ള, cr4_മുകളിലെ)) {
        printf(" of CR4\n");
        തിരികെ false;
    }
    തിരികെ true;
}

സ്ഥിതം bool_t BOOT_CODE init_vtx_fixed_മൂല്യങ്ങൾ(bool_t useTrueMsrs)
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
    എങ്കിൽ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } അല്ലെങ്കിൽ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    pin_control_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(pinbased_ctls);
    pin_control_താഴെയുള്ള = x86_rdmsr_മുകളിലെ(pinbased_ctls);
    primary_control_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(procbased_ctls);
    primary_control_താഴെയുള്ള = x86_rdmsr_മുകളിലെ(procbased_ctls);
    secondary_control_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_താഴെയുള്ള = x86_rdmsr_മുകളിലെ(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(exit_ctls);
    exit_control_താഴെയുള്ള = x86_rdmsr_മുകളിലെ(exit_ctls);
    entry_control_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(entry_ctls);
    entry_control_താഴെയുള്ള = x86_rdmsr_മുകളിലെ(entry_ctls);

    cr0_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(IA32_VMX_CR0_FIXED0_MSR);
    cr0_താഴെയുള്ള = x86_rdmsr_താഴെയുള്ള(IA32_VMX_CR0_FIXED1_MSR);
    cr4_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(IA32_VMX_CR4_FIXED0_MSR);
    cr4_താഴെയുള്ള = x86_rdmsr_താഴെയുള്ള(IA32_VMX_CR4_FIXED1_MSR);

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
    എങ്കിൽ (secondary_control_താഴെയുള്ള & BIT(7)) {
        cr0_മുകളിലെ &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    എങ്കിൽ (!(secondary_control_താഴെയുള്ള & BIT(5))) {
        vmx_feature_vpid = 0;
        printf("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } അല്ലെങ്കിൽ {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    എങ്കിൽ (!(exit_control_താഴെയുള്ള & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        printf("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } അല്ലെങ്കിൽ {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    എങ്കിൽ (!(exit_control_താഴെയുള്ള & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        printf("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } അല്ലെങ്കിൽ {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_താഴെയുള്ള) & pin_control_mask;
    എങ്കിൽ (missing) {
        printf("vt-x: Unsupported pin control features %lx\n", (long)missing);
        തിരികെ false;
    }
    missing = (~primary_control_താഴെയുള്ള) & primary_control_mask;
    എങ്കിൽ (missing) {
        printf("vt-x: Unsupported primary control features %lx\n", (long)missing);
        തിരികെ false;
    }
    missing = (~secondary_control_താഴെയുള്ള) & secondary_control_mask;
    എങ്കിൽ (missing) {
        printf("vt-x: Unsupported secondary control features %lx\n", (long)missing);
        തിരികെ false;
    }
    missing = (~exit_control_താഴെയുള്ള) & exit_control_mask;
    എങ്കിൽ (missing) {
        printf("vt-x: Unsupported exit control features %lx\n", (long)missing);
        തിരികെ false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_താഴെയുള്ള) & BIT(9);
    എങ്കിൽ (missing) {
        printf("vt-x: CPU does not support 64-bit guests\n");
        തിരികെ false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_മുകളിലെ |= pin_control_mask;
    primary_control_മുകളിലെ |= primary_control_mask;
    secondary_control_മുകളിലെ |= secondary_control_mask;
    exit_control_മുകളിലെ |= exit_control_mask;

    തിരികെ true;
}

സ്ഥിതം bool_t BOOT_CODE check_vtx_fixed_മൂല്യങ്ങൾ(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    എങ്കിൽ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } അല്ലെങ്കിൽ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    uint32_t local_pin_control_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(pinbased_ctls);
    uint32_t local_pin_control_താഴെയുള്ള = x86_rdmsr_മുകളിലെ(pinbased_ctls);
    uint32_t local_primary_control_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(procbased_ctls);
    uint32_t local_primary_control_താഴെയുള്ള = x86_rdmsr_മുകളിലെ(procbased_ctls);
    uint32_t local_secondary_control_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_താഴെയുള്ള = x86_rdmsr_മുകളിലെ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(exit_ctls);
    uint32_t local_exit_control_താഴെയുള്ള = x86_rdmsr_മുകളിലെ(exit_ctls);
    uint32_t local_entry_control_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(entry_ctls);
    uint32_t local_entry_control_താഴെയുള്ള = x86_rdmsr_മുകളിലെ(entry_ctls);

    uint32_t local_cr0_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_താഴെയുള്ള = x86_rdmsr_താഴെയുള്ള(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_മുകളിലെ = x86_rdmsr_താഴെയുള്ള(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_താഴെയുള്ള = x86_rdmsr_താഴെയുള്ള(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    എങ്കിൽ (local_secondary_control_താഴെയുള്ള & BIT(7)) {
        local_cr0_മുകളിലെ &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    തിരികെ
        (local_pin_control_മുകളിലെ & pin_control_മുകളിലെ) == local_pin_control_മുകളിലെ &&
        (~local_pin_control_താഴെയുള്ള & pin_control_മുകളിലെ) == 0 &&
        (local_primary_control_മുകളിലെ & primary_control_മുകളിലെ) == local_primary_control_മുകളിലെ &&
        (~local_primary_control_താഴെയുള്ള & primary_control_മുകളിലെ) == 0 &&
        (local_secondary_control_മുകളിലെ & secondary_control_മുകളിലെ) == local_secondary_control_മുകളിലെ &&
        (~local_secondary_control_താഴെയുള്ള & secondary_control_മുകളിലെ) == 0 &&
        (local_exit_control_മുകളിലെ & exit_control_മുകളിലെ) == local_exit_control_മുകളിലെ &&
        (~local_exit_control_താഴെയുള്ള & exit_control_മുകളിലെ) == 0 &&
        (local_entry_control_മുകളിലെ & entry_control_മുകളിലെ) == local_entry_control_മുകളിലെ &&
        (~local_entry_control_താഴെയുള്ള & entry_control_മുകളിലെ) == 0 &&
        local_cr0_മുകളിലെ == cr0_മുകളിലെ &&
        local_cr0_താഴെയുള്ള == cr0_താഴെയുള്ള &&
        local_cr4_മുകളിലെ == cr4_മുകളിലെ &&
        local_cr4_താഴെയുള്ള == cr4_താഴെയുള്ള;
}

സ്ഥിതം വരിയിൽ uint32_t applyFixedBits(uint32_t original, uint32_t മുകളിലെ, uint32_t താഴെയുള്ള)
{
    original |= മുകളിലെ;
    original &= താഴെയുള്ള;
    തിരികെ original;
}

ശൂന്യം vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = ശൂന്യ;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    memcpy(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_മുകളിലെ & cr0_താഴെയുള്ള;
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
    എങ്കിൽ (vmx_feature_load_perf_global_ctrl) {
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
    എങ്കിൽ (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((അക്ഷരം *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.വാക്കുകൾ[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_register]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_to_paddr(x64KSKernelPML4), 0).വാക്കുകൾ[0]);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_മുകളിലെ & pin_control_താഴെയുള്ള);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_മുകളിലെ & primary_control_താഴെയുള്ള);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_മുകളിലെ & secondary_control_താഴെയുള്ള);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_മുകളിലെ & exit_control_താഴെയുള്ള);

    uint64_t entry_control = entry_control_മുകളിലെ & entry_control_താഴെയുള്ള;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_മുകളിലെ & cr4_താഴെയുള്ള);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_മുകളിലെ, ~(word_t)0);

    memset(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_SIZE);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((അക്ഷരം *)vcpu->io + (VCPU_IOBITMAP_SIZE / 2)));
}

സ്ഥിതം ശൂന്യം dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    പ്രതിജ്ഞാ(tcb->tcbArch.tcbVCPU == vcpu);
    പ്രതിജ്ഞാ(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = ശൂന്യ;
    vcpu->vcpuTCB = ശൂന്യ;
}

ശൂന്യം vcpu_finalise(vcpu_t *vcpu)
{
    എങ്കിൽ (ARCH_കണ്ണി_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
#ifdef ENABLE_SMP_SUPPORT
        എങ്കിൽ (vcpu->last_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->last_cpu);
        } അല്ലെങ്കിൽ
#endif /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    എങ്കിൽ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    എങ്കിൽ (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

സ്ഥിതം ശൂന്യം associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    എങ്കിൽ (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    എങ്കിൽ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

സ്ഥിതം exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *buffer)
{
    പൂർണ്ണസംഖ്യ i;
    വേണ്ടി (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, buffer);
    }
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeVCPUWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    എങ്കിൽ (length < 15) {
#അല്ലെങ്കിൽ
    എങ്കിൽ (length < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    തിരികെ invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
സ്ഥിതം exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *buffer, word_t field, word_t മൂല്യം)
{
    tcb_t *thread;
    thread = കണ്ണി_STATE(ksCurThread);
    എങ്കിൽ (ARCH_കണ്ണി_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    തിരഞ്ഞെടുപ്പ് (field) {
    അവസ്ഥ IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = മൂല്യം;
        നിർത്തുക;
    അവസ്ഥ IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = മൂല്യം;
        നിർത്തുക;
    അവസ്ഥ IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = മൂല്യം;
        നിർത്തുക;
    അവസ്ഥ IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = മൂല്യം;
        നിർത്തുക;
    }
    setMR(thread, buffer, 0, മൂല്യം);
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeVCPUWriteMSR(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t മൂല്യം;

    എങ്കിൽ (length < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    മൂല്യം = getSyscallArg(1, buffer);
    തിരഞ്ഞെടുപ്പ് (field) {
    അവസ്ഥ IA32_LSTAR_MSR:
    അവസ്ഥ IA32_STAR_MSR:
    അവസ്ഥ IA32_CSTAR_MSR:
    അവസ്ഥ IA32_FMASK_MSR:
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        userError("VCPU WriteMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    തിരികെ invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer, field, മൂല്യം);
}

സ്ഥിതം exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *buffer)
{
    tcb_t *thread;
    thread = കണ്ണി_STATE(ksCurThread);

    word_t മൂല്യം = 0;

    തിരഞ്ഞെടുപ്പ് (field) {
    അവസ്ഥ IA32_LSTAR_MSR:
        മൂല്യം = vcpu->syscall_registers[VCPU_LSTAR];
        നിർത്തുക;
    അവസ്ഥ IA32_STAR_MSR:
        മൂല്യം = vcpu->syscall_registers[VCPU_STAR];
        നിർത്തുക;
    അവസ്ഥ IA32_CSTAR_MSR:
        മൂല്യം = vcpu->syscall_registers[VCPU_CSTAR];
        നിർത്തുക;
    അവസ്ഥ IA32_FMASK_MSR:
        മൂല്യം = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        നിർത്തുക;
    അവസ്ഥ IA32_VMX_MISC_MSR:
        മൂല്യം = x86_rdmsr(field);
        നിർത്തുക;
    }

    setMR(thread, buffer, 0, മൂല്യം);
    setRegister(thread, സന്ദേശം_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeVCPUReadMSR(cap_t cap, word_t length, word_t *buffer)
{
    എങ്കിൽ (length < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    തിരഞ്ഞെടുപ്പ് (field) {
    അവസ്ഥ IA32_LSTAR_MSR:
    അവസ്ഥ IA32_STAR_MSR:
    അവസ്ഥ IA32_CSTAR_MSR:
    അവസ്ഥ IA32_FMASK_MSR:
    അവസ്ഥ IA32_VMX_MISC_MSR:
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        userError("VCPU ReadMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    തിരികെ invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, buffer);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

സ്ഥിതം exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t താഴെയുള്ള, uint16_t മുകളിലെ)
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
    setIOPortMask(vcpu->io, താഴെയുള്ള, മുകളിലെ, false);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeEnableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t താഴെയുള്ള, മുകളിലെ;

    എങ്കിൽ (length < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    എങ്കിൽ (current_extra_caps.excaprefs[0] == ശൂന്യ) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    എങ്കിൽ (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    താഴെയുള്ള = getSyscallArg(0, buffer);
    മുകളിലെ = getSyscallArg(1, buffer);

    എങ്കിൽ (താഴെയുള്ള < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || മുകളിലെ > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 0;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeEnableIOPort(vcpu, ioSlot, ioCap, താഴെയുള്ള, മുകളിലെ);
}

സ്ഥിതം exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t താഴെയുള്ള, uint16_t മുകളിലെ)
{
    setIOPortMask(vcpu->io, താഴെയുള്ള, മുകളിലെ, true);
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeDisableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    uint16_t താഴെയുള്ള, മുകളിലെ;

    എങ്കിൽ (length < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    താഴെയുള്ള = getSyscallArg(0, buffer);
    മുകളിലെ = getSyscallArg(1, buffer);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    തിരികെ invokeDisableIOPort(vcpu, താഴെയുള്ള, മുകളിലെ);
}

സ്ഥിതം exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *buffer, word_t field, word_t മൂല്യം)
{
    tcb_t *thread;
    thread = കണ്ണി_STATE(ksCurThread);

    എങ്കിൽ (ARCH_കണ്ണി_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    തിരഞ്ഞെടുപ്പ് (field) {
    അവസ്ഥ VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = മൂല്യം;
        നിർത്തുക;
    അവസ്ഥ VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = മൂല്യം;
        നിർത്തുക;
    അവസ്ഥ VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = മൂല്യം;
        നിർത്തുക;
    അവസ്ഥ VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = മൂല്യം;
        നിർത്തുക;
    }
    vmwrite(field, മൂല്യം);

    എങ്കിൽ (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned പൂർണ്ണസംഖ്യ length = setMR(thread, buffer, 0, മൂല്യം);
        setRegister(thread, സന്ദേശം_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Running);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeWriteVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    word_t മൂല്യം;

    എങ്കിൽ (length < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    മൂല്യം = getSyscallArg(1, buffer);
    തിരഞ്ഞെടുപ്പ് (field) {
    അവസ്ഥ VMX_GUEST_RIP:
    അവസ്ഥ VMX_GUEST_RSP:
    അവസ്ഥ VMX_GUEST_ES_SELECTOR:
    അവസ്ഥ VMX_GUEST_CS_SELECTOR:
    അവസ്ഥ VMX_GUEST_SS_SELECTOR:
    അവസ്ഥ VMX_GUEST_DS_SELECTOR:
    അവസ്ഥ VMX_GUEST_FS_SELECTOR:
    അവസ്ഥ VMX_GUEST_GS_SELECTOR:
    അവസ്ഥ VMX_GUEST_LDTR_SELECTOR:
    അവസ്ഥ VMX_GUEST_TR_SELECTOR:
    അവസ്ഥ VMX_GUEST_DEBUGCTRL:
    അവസ്ഥ VMX_GUEST_PAT:
    അവസ്ഥ VMX_GUEST_EFER:
    അവസ്ഥ VMX_GUEST_PERF_GLOBAL_CTRL:
    അവസ്ഥ VMX_GUEST_PDPTE0:
    അവസ്ഥ VMX_GUEST_PDPTE1:
    അവസ്ഥ VMX_GUEST_PDPTE2:
    അവസ്ഥ VMX_GUEST_PDPTE3:
    അവസ്ഥ VMX_GUEST_ES_LIMIT:
    അവസ്ഥ VMX_GUEST_CS_LIMIT:
    അവസ്ഥ VMX_GUEST_SS_LIMIT:
    അവസ്ഥ VMX_GUEST_DS_LIMIT:
    അവസ്ഥ VMX_GUEST_FS_LIMIT:
    അവസ്ഥ VMX_GUEST_GS_LIMIT:
    അവസ്ഥ VMX_GUEST_LDTR_LIMIT:
    അവസ്ഥ VMX_GUEST_TR_LIMIT:
    അവസ്ഥ VMX_GUEST_GDTR_LIMIT:
    അവസ്ഥ VMX_GUEST_IDTR_LIMIT:
    അവസ്ഥ VMX_GUEST_ES_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_CS_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_SS_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_DS_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_FS_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_GS_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_TR_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_INTERRUPTABILITY:
    അവസ്ഥ VMX_GUEST_ACTIVITY:
    അവസ്ഥ VMX_GUEST_SMBASE:
    അവസ്ഥ VMX_GUEST_SYSENTER_CS:
    അവസ്ഥ VMX_GUEST_PREEMPTION_TIMER_മൂല്യം:
    അവസ്ഥ VMX_GUEST_ES_BASE:
    അവസ്ഥ VMX_GUEST_CS_BASE:
    അവസ്ഥ VMX_GUEST_SS_BASE:
    അവസ്ഥ VMX_GUEST_DS_BASE:
    അവസ്ഥ VMX_GUEST_FS_BASE:
    അവസ്ഥ VMX_GUEST_GS_BASE:
    അവസ്ഥ VMX_GUEST_LDTR_BASE:
    അവസ്ഥ VMX_GUEST_TR_BASE:
    അവസ്ഥ VMX_GUEST_GDTR_BASE:
    അവസ്ഥ VMX_GUEST_IDTR_BASE:
    അവസ്ഥ VMX_GUEST_DR7:
    അവസ്ഥ VMX_GUEST_RFLAGS:
    അവസ്ഥ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    അവസ്ഥ VMX_GUEST_SYSENTER_ESP:
    അവസ്ഥ VMX_GUEST_SYSENTER_EIP:
    അവസ്ഥ VMX_CONTROL_CR0_MASK:
    അവസ്ഥ VMX_CONTROL_CR4_MASK:
    അവസ്ഥ VMX_CONTROL_CR0_READ_SHADOW:
    അവസ്ഥ VMX_CONTROL_CR4_READ_SHADOW:
    അവസ്ഥ VMX_GUEST_CR3:
    അവസ്ഥ VMX_CONTROL_EXCEPTION_BITMAP:
    അവസ്ഥ VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    അവസ്ഥ VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        നിർത്തുക;
    അവസ്ഥ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        മൂല്യം = applyFixedBits(മൂല്യം, pin_control_മുകളിലെ, pin_control_താഴെയുള്ള);
        നിർത്തുക;
    അവസ്ഥ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        മൂല്യം = applyFixedBits(മൂല്യം, primary_control_മുകളിലെ, primary_control_താഴെയുള്ള);
        നിർത്തുക;
    അവസ്ഥ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        മൂല്യം = applyFixedBits(മൂല്യം, secondary_control_മുകളിലെ, secondary_control_താഴെയുള്ള);
        നിർത്തുക;
    അവസ്ഥ VMX_CONTROL_ENTRY_CONTROLS:
        മൂല്യം = applyFixedBits(മൂല്യം, entry_control_മുകളിലെ, entry_control_താഴെയുള്ള);
        നിർത്തുക;
    അവസ്ഥ VMX_CONTROL_EXIT_CONTROLS:
        മൂല്യം = applyFixedBits(മൂല്യം, exit_control_മുകളിലെ, exit_control_താഴെയുള്ള);
        നിർത്തുക;
    അവസ്ഥ VMX_GUEST_CR0:
        മൂല്യം = applyFixedBits(മൂല്യം, cr0_മുകളിലെ, cr0_താഴെയുള്ള);
        നിർത്തുക;
    അവസ്ഥ VMX_GUEST_CR4:
        മൂല്യം = applyFixedBits(മൂല്യം, cr4_മുകളിലെ, cr4_താഴെയുള്ള);
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        userError("VCPU WriteVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    തിരികെ invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, buffer, field, മൂല്യം);
}

സ്ഥിതം word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    തിരഞ്ഞെടുപ്പ് (field) {
    അവസ്ഥ VMX_CONTROL_EXCEPTION_BITMAP:
        തിരികെ vcpu->exception_bitmap;
    അവസ്ഥ VMX_GUEST_CR0:
        തിരികെ vcpu->cr0;
    അവസ്ഥ VMX_CONTROL_CR0_MASK:
        തിരികെ vcpu->cr0_mask;
    അവസ്ഥ VMX_CONTROL_CR0_READ_SHADOW:
        തിരികെ vcpu->cr0_shadow;
    }
    എങ്കിൽ (ARCH_കണ്ണി_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    തിരികെ vmread(field);
}

സ്ഥിതം exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *buffer)
{
    tcb_t *thread;
    thread = കണ്ണി_STATE(ksCurThread);
    word_t മൂല്യം = readVMCSField(vcpu, field);
    എങ്കിൽ (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned പൂർണ്ണസംഖ്യ length = setMR(thread, buffer, 0, മൂല്യം);
        setRegister(thread, സന്ദേശം_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Running);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeReadVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    എങ്കിൽ (length < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    തിരഞ്ഞെടുപ്പ് (field) {
    അവസ്ഥ VMX_GUEST_RIP:
    അവസ്ഥ VMX_GUEST_RSP:
    അവസ്ഥ VMX_GUEST_ES_SELECTOR:
    അവസ്ഥ VMX_GUEST_CS_SELECTOR:
    അവസ്ഥ VMX_GUEST_SS_SELECTOR:
    അവസ്ഥ VMX_GUEST_DS_SELECTOR:
    അവസ്ഥ VMX_GUEST_FS_SELECTOR:
    അവസ്ഥ VMX_GUEST_GS_SELECTOR:
    അവസ്ഥ VMX_GUEST_LDTR_SELECTOR:
    അവസ്ഥ VMX_GUEST_TR_SELECTOR:
    അവസ്ഥ VMX_GUEST_DEBUGCTRL:
    അവസ്ഥ VMX_GUEST_PAT:
    അവസ്ഥ VMX_GUEST_EFER:
    അവസ്ഥ VMX_GUEST_PERF_GLOBAL_CTRL:
    അവസ്ഥ VMX_GUEST_PDPTE0:
    അവസ്ഥ VMX_GUEST_PDPTE1:
    അവസ്ഥ VMX_GUEST_PDPTE2:
    അവസ്ഥ VMX_GUEST_PDPTE3:
    അവസ്ഥ VMX_GUEST_ES_LIMIT:
    അവസ്ഥ VMX_GUEST_CS_LIMIT:
    അവസ്ഥ VMX_GUEST_SS_LIMIT:
    അവസ്ഥ VMX_GUEST_DS_LIMIT:
    അവസ്ഥ VMX_GUEST_FS_LIMIT:
    അവസ്ഥ VMX_GUEST_GS_LIMIT:
    അവസ്ഥ VMX_GUEST_LDTR_LIMIT:
    അവസ്ഥ VMX_GUEST_TR_LIMIT:
    അവസ്ഥ VMX_GUEST_GDTR_LIMIT:
    അവസ്ഥ VMX_GUEST_IDTR_LIMIT:
    അവസ്ഥ VMX_GUEST_ES_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_CS_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_SS_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_DS_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_FS_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_GS_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_TR_ACCESS_RIGHTS:
    അവസ്ഥ VMX_GUEST_INTERRUPTABILITY:
    അവസ്ഥ VMX_GUEST_ACTIVITY:
    അവസ്ഥ VMX_GUEST_SMBASE:
    അവസ്ഥ VMX_GUEST_SYSENTER_CS:
    അവസ്ഥ VMX_GUEST_PREEMPTION_TIMER_മൂല്യം:
    അവസ്ഥ VMX_GUEST_ES_BASE:
    അവസ്ഥ VMX_GUEST_CS_BASE:
    അവസ്ഥ VMX_GUEST_SS_BASE:
    അവസ്ഥ VMX_GUEST_DS_BASE:
    അവസ്ഥ VMX_GUEST_FS_BASE:
    അവസ്ഥ VMX_GUEST_GS_BASE:
    അവസ്ഥ VMX_GUEST_LDTR_BASE:
    അവസ്ഥ VMX_GUEST_TR_BASE:
    അവസ്ഥ VMX_GUEST_GDTR_BASE:
    അവസ്ഥ VMX_GUEST_IDTR_BASE:
    അവസ്ഥ VMX_GUEST_DR7:
    അവസ്ഥ VMX_GUEST_RFLAGS:
    അവസ്ഥ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    അവസ്ഥ VMX_GUEST_SYSENTER_ESP:
    അവസ്ഥ VMX_GUEST_SYSENTER_EIP:
    അവസ്ഥ VMX_CONTROL_CR0_MASK:
    അവസ്ഥ VMX_CONTROL_CR4_MASK:
    അവസ്ഥ VMX_CONTROL_CR0_READ_SHADOW:
    അവസ്ഥ VMX_CONTROL_CR4_READ_SHADOW:
    അവസ്ഥ VMX_DATA_INSTRUCTION_ERROR:
    അവസ്ഥ VMX_DATA_EXIT_INTERRUPT_INFO:
    അവസ്ഥ VMX_DATA_EXIT_INTERRUPT_ERROR:
    അവസ്ഥ VMX_DATA_IDT_VECTOR_INFO:
    അവസ്ഥ VMX_DATA_IDT_VECTOR_ERROR:
    അവസ്ഥ VMX_DATA_EXIT_INSTRUCTION_LENGTH:
    അവസ്ഥ VMX_DATA_EXIT_INSTRUCTION_INFO:
    അവസ്ഥ VMX_DATA_GUEST_PHYSICAL:
    അവസ്ഥ VMX_DATA_IO_RCX:
    അവസ്ഥ VMX_DATA_IO_RSI:
    അവസ്ഥ VMX_DATA_IO_RDI:
    അവസ്ഥ VMX_DATA_IO_RIP:
    അവസ്ഥ VMX_DATA_GUEST_LINEAR_ADDRESS:
    അവസ്ഥ VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    അവസ്ഥ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    അവസ്ഥ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    അവസ്ഥ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    അവസ്ഥ VMX_CONTROL_EXCEPTION_BITMAP:
    അവസ്ഥ VMX_CONTROL_ENTRY_CONTROLS:
    അവസ്ഥ VMX_CONTROL_EXIT_CONTROLS:
    അവസ്ഥ VMX_GUEST_CR0:
    അവസ്ഥ VMX_GUEST_CR3:
    അവസ്ഥ VMX_GUEST_CR4:
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        userError("VCPU ReadVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    തിരികെ invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, buffer);
}

സ്ഥിതം exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decodeSetTCB(cap_t cap, word_t length, word_t *buffer)
{
    cap_t tcbCap;
    എങ്കിൽ (current_extra_caps.excaprefs[0] == ശൂന്യ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    എങ്കിൽ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    തിരികെ invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

ശൂന്യം vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *buffer;
    എങ്കിൽ (ARCH_കണ്ണി_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    buffer = lookupIPCBuffer(false, കണ്ണി_STATE(ksCurThread));
    എങ്കിൽ (!buffer) {
        userError("No IPC buffer.");
        തിരികെ;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, buffer));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, buffer), primary_control_മുകളിലെ,
                                                                   primary_control_താഴെയുള്ള));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, buffer));
}

ശൂന്യം vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *buffer;
    vcpu_t *vcpu;

    buffer = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    പ്രതിജ്ഞാ(vcpu);

    എങ്കിൽ (ARCH_കണ്ണി_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, സന്ദേശം_Info_Register, 0);
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
    തിരഞ്ഞെടുപ്പ് (invLabel) {
    അവസ്ഥ X86VCPUSetTCB:
        തിരികെ decodeSetTCB(cap, length, buffer);
    അവസ്ഥ X86VCPUReadVMCS:
        തിരികെ decodeReadVMCS(cap, length, call, buffer);
    അവസ്ഥ X86VCPUWriteVMCS:
        തിരികെ decodeWriteVMCS(cap, length, call, buffer);
    അവസ്ഥ X86VCPUEnableIOPort:
        തിരികെ decodeEnableIOPort(cap, length, buffer);
    അവസ്ഥ X86VCPUDisableIOPort:
        തിരികെ decodeDisableIOPort(cap, length, buffer);
    അവസ്ഥ X86VCPUWriteRegisters:
        തിരികെ decodeVCPUWriteRegisters(cap, length, buffer);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    അവസ്ഥ X86VCPUWriteMSR:
        തിരികെ decodeVCPUWriteMSR(cap, length, buffer);
    അവസ്ഥ X86VCPUReadMSR:
        തിരികെ decodeVCPUReadMSR(cap, length, buffer);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    സ്ഥിരസ്ഥിതി:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
}

സ്ഥിതം bool_t is_vtx_supported(ശൂന്യം)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    തിരികെ !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

സ്ഥിതം വരിയിൽ ശൂന്യം clear_bit(word_t *bitmap, word_t bit)
{
    പൂർണ്ണസംഖ്യ index = bit / (വലുപ്പം(word_t) * 8);
    പൂർണ്ണസംഖ്യ offset = bit % (വലുപ്പം(word_t) * 8);
    bitmap[index] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(ശൂന്യം)
{
    എങ്കിൽ (!is_vtx_supported()) {
        printf("vt-x: not supported\n");
        തിരികെ false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.വാക്കുകൾ[0] = x86_rdmsr_താഴെയുള്ള(IA32_VMX_BASIC_MSR);
    vmx_basic.വാക്കുകൾ[1] = x86_rdmsr_മുകളിലെ(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.വാക്കുകൾ[0] = x86_rdmsr_താഴെയുള്ള(IA32_FEATURE_CONTROL_MSR);
    എങ്കിൽ (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        എങ്കിൽ (feature_control_msr_get_lock(feature_control)) {
            printf("vt-x: feature locked\n");
            തിരികെ false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_മുകളിലെ(IA32_FEATURE_CONTROL_MSR), feature_control.വാക്കുകൾ[0]);
    }
    /* make sure the msr is locked */
    എങ്കിൽ (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_മുകളിലെ(IA32_FEATURE_CONTROL_MSR), feature_control.വാക്കുകൾ[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    എങ്കിൽ (CURRENT_CPU_INDEX() == 0) {
        എങ്കിൽ (!init_vtx_fixed_മൂല്യങ്ങൾ(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            printf("vt-x: lack of required features\n");
            തിരികെ false;
        }
    }
    എങ്കിൽ (!check_vtx_fixed_മൂല്യങ്ങൾ(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        printf("vt-x: cores have inconsistent features\n");
        തിരികെ false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    ഘടന vmxon_region *vmxon_kptr = &ARCH_കണ്ണി_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    എങ്കിൽ (!vtx_check_fixed_മൂല്യങ്ങൾ(read_cr0(), read_cr4())) {
        തിരികെ false;
    }
    എങ്കിൽ (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        printf("vt-x: vmxon failure\n");
        തിരികെ false;
    }
    memset(&msr_bitmap_region, ~0, വലുപ്പം(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.താഴെയുള്ള_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.താഴെയുള്ള_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.താഴെയുള്ള_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.താഴെയുള്ള_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.താഴെയുള്ള_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.താഴെയുള്ള_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.മുകളിലെ_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.മുകളിലെ_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.മുകളിലെ_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.മുകളിലെ_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.മുകളിലെ_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.മുകളിലെ_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.വാക്കുകൾ[0] = x86_rdmsr_താഴെയുള്ള(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.വാക്കുകൾ[1] = x86_rdmsr_മുകളിലെ(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    എങ്കിൽ (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        printf("vt-x: Expected wb attribute for EPT paging structure\n");
        തിരികെ false;
    }
    എങ്കിൽ (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        printf("vt-x: Expected supported for 2m pages\n");
        തിരികെ false;
    }

    തിരികെ true;
}

സ്ഥിതം ശൂന്യം setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *buffer;
    പൂർണ്ണസംഖ്യ i;

    buffer = lookupIPCBuffer(true, കണ്ണി_STATE(ksCurThread));

    setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_DATA_EXIT_INSTRUCTION_LENGTH));
    setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_DATA_GUEST_PHYSICAL));
    setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    വേണ്ടി (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(കണ്ണി_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_EAX + i,
              കണ്ണി_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

സ്ഥിതം ശൂന്യം handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(കണ്ണി_STATE(ksCurThread), സന്ദേശം_Info_Register, SEL4_VMENTER_RESULT_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

സ്ഥിതം വരിയിൽ ശൂന്യം finishVmexitSaving(ശൂന്യം)
{
    vcpu_t *vcpu = ARCH_കണ്ണി_STATE(x86KSCurrentVCPU);
    പ്രതിജ്ഞാ(vcpu == കണ്ണി_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(ശൂന്യം)
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
    എങ്കിൽ (reason == EXTERNAL_INTERRUPT) {
        കണ്ണി_LOCK_IRQ;
        എങ്കിൽ (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_DATA_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_കണ്ണി_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } അല്ലെങ്കിൽ {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(കണ്ണി_STATE(ksCurThread));
#endif
        തിരികെ EXCEPTION_NONE;
    }

    കണ്ണി_LOCK_SYS;

    തിരഞ്ഞെടുപ്പ് (reason) {
    അവസ്ഥ EXCEPTION_OR_NMI:
    അവസ്ഥ MOV_DR:
    അവസ്ഥ TASK_SWITCH:
    അവസ്ഥ CONTROL_REGISTER:
    അവസ്ഥ IO:
    അവസ്ഥ MWAIT:
    അവസ്ഥ SIPI:
    അവസ്ഥ INVLPG:
    അവസ്ഥ INVEPT:
    അവസ്ഥ INVVPID:
    അവസ്ഥ VMCLEAR:
    അവസ്ഥ VMPTRLD:
    അവസ്ഥ VMPTRST:
    അവസ്ഥ VMREAD:
    അവസ്ഥ VMWRITE:
    അവസ്ഥ VMXON:
    അവസ്ഥ EPT_VIOLATION:
    അവസ്ഥ GDTR_OR_IDTR:
    അവസ്ഥ LDTR_OR_TR:
    അവസ്ഥ TPR_BELOW_THRESHOLD:
    അവസ്ഥ APIC_ACCESS:
        qualification = vmread(VMX_DATA_EXIT_QUALIFICATION);
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    തിരികെ EXCEPTION_NONE;
}

exception_t handleVmEntryFail(ശൂന്യം)
{
    handleVmxFault(-1, -1);

    തിരികെ EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
ശൂന്യം VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    പ്രതിജ്ഞാ(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    എങ്കിൽ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {

        word_t badge = notification_ptr_get_ntfn_സന്ദേശം_Identifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        എങ്കിൽ (tcb != കണ്ണി_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

സ്ഥിതം ശൂന്യം invvpid_context(uint16_t vpid)
{
    ഘടന {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    asm അസ്ഥിര("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
}

സ്ഥിതം ശൂന്യം setEPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_വേര്;
    എങ്കിൽ (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_വേര് = kpptr_to_paddr(null_ept_space);
    } അല്ലെങ്കിൽ {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        എങ്കിൽ (find_ret.status != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_വേര് = kpptr_to_paddr(null_ept_space);
        } അല്ലെങ്കിൽ {
            ept_വേര് = pptr_to_paddr(pml4);
        }
    }
    എങ്കിൽ (ept_വേര് != vcpu->last_ept_വേര്) {
        vcpu->last_ept_വേര് = ept_വേര്;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_വേര്,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.വാക്കുകൾ[0]);
        പ്രതിജ്ഞാ(vcpu->vpid != VPID_INVALID);
        എങ്കിൽ (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

ശൂന്യം clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t last)
{
    എങ്കിൽ (vpid == VPID_INVALID) {
        തിരികെ;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    എങ്കിൽ (vcpu == ശൂന്യ) {
        തിരികെ;
    }
    പ്രതിജ്ഞാ(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

സ്ഥിതം വരിയിൽ vpid_t nextVPID(vpid_t vpid)
{
    എങ്കിൽ (vpid == VPID_LAST) {
        തിരികെ VPID_FIRST;
    } അല്ലെങ്കിൽ {
        തിരികെ vpid + 1;
    }
}

സ്ഥിതം ശൂന്യം invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, വലുപ്പം(vcpu->io));
    /* invalidate the VPID context */
    എങ്കിൽ (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
സ്ഥിതം ശൂന്യം releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = ശൂന്യ;
}

സ്ഥിതം vpid_t findFreeVPID(ശൂന്യം)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    ചെയ്യുക {
        എങ്കിൽ (x86KSVPIDTable[vpid] == ശൂന്യ) {
            തിരികെ vpid;
        }
        vpid = nextVPID(vpid);
    } ആകുമ്പോൾ (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = nextVPID(x86KSNextVPID);
    തിരികെ vpid;
}

സ്ഥിതം ശൂന്യം storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    പ്രതിജ്ഞാ(x86KSVPIDTable[vpid] == ശൂന്യ);
    പ്രതിജ്ഞാ(vcpu->vpid == VPID_INVALID);
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
ശൂന്യം vcpu_fpu_to_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    എങ്കിൽ (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        എങ്കിൽ (!vcpu->fpu_active) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_active = true;
            loadFpuState(tcb);
        }
    } അല്ലെങ്കിൽ {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_active = true;
        switchLocalFpuOwner(tcb);
    }
}

ശൂന്യം vcpu_fpu_to_host(tcb_t *tcb, vcpu_t *vcpu)
{
    എങ്കിൽ (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } അല്ലെങ്കിൽ എങ്കിൽ (nativeThreadUsingFPU(tcb) && vcpu->fpu_active) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_active = false;
        loadFpuState(tcb);
    } അല്ലെങ്കിൽ എങ്കിൽ (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

ശൂന്യം restoreVMCS(ശൂന്യം)
{
    tcb_t *cur_thread = കണ്ണി_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    എങ്കിൽ (ARCH_കണ്ണി_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ifndef CONFIG_KERNEL_SKIM_WINDOW
    എങ്കിൽ (getCurrentCR3().വാക്കുകൾ[0] != expected_vmcs->last_host_cr3) {
        expected_vmcs->last_host_cr3 = getCurrentCR3().വാക്കുകൾ[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().വാക്കുകൾ[0]);
    }
#endif
    എങ്കിൽ (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        എങ്കിൽ (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    setEPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

ശൂന്യം invept(ept_pml4e_t *ept_pml4)
{
    എങ്കിൽ (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        ഘടന {
            uint64_t parts[2];
        } address;
        word_t type;
        എങ്കിൽ (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } അല്ലെങ്കിൽ എങ്കിൽ (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } അല്ലെങ്കിൽ {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            തിരികെ;
        }

        address.parts[0] = pptr_to_paddr((ശൂന്യം *)ept_pml4);
        address.parts[1] = 0;
        asm അസ്ഥിര(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
