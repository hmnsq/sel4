/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#ifdef CONFIG_VTX

#अन्तर्भाव <types.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <object/objecttype.h>
#अन्तर्भाव <arch/machine/cpu_registers.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/object/vcpu.h>
#अन्तर्भाव <arch/object/ioport.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <sel4/arch/vmenter.h>

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

स्थिरांक vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

प्रकारव्याख्या संरचना msr_bitmap {
    word_t bitmap[0x2000 / आकार(word_t) / 8];
} msr_bitmap_t;

प्रकारव्याख्या संरचना msr_bitmaps {
    msr_bitmap_t खालचा_msr_read;
    msr_bitmap_t वरचा_msr_read;
    msr_bitmap_t खालचा_msr_write;
    msr_bitmap_t वरचा_msr_write;
} msr_bitmaps_t;

स्थिर msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

स्थिर अक्षर null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
स्थिर uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
स्थिर vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
स्थिर uint32_t pin_control_वरचा;
स्थिर uint32_t pin_control_खालचा;
स्थिर uint32_t primary_control_वरचा;
स्थिर uint32_t primary_control_खालचा;
स्थिर uint32_t secondary_control_वरचा;
स्थिर uint32_t secondary_control_खालचा;
स्थिर uint32_t entry_control_वरचा;
स्थिर uint32_t entry_control_खालचा;
स्थिर uint32_t exit_control_वरचा;
स्थिर uint32_t exit_control_खालचा;
स्थिर uint32_t cr0_वरचा;
स्थिर uint32_t cr0_खालचा;
स्थिर uint32_t cr4_वरचा;
स्थिर uint32_t cr4_खालचा;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
स्थिर bool_t vmx_feature_vpid;
स्थिर bool_t vmx_feature_load_perf_global_ctrl;
स्थिर bool_t vmx_feature_ack_on_exit;

स्थिर vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
स्थिर vpid_t x86KSNextVPID = VPID_FIRST;

स्थिर शून्य releaseVPID(vpid_t vpid);

स्थिर ओळीत bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    यंत्र अस्थिर(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    परत !!error;
}

स्थिर शून्य vmclear(शून्य *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((शून्य *)vmcs_ptr);
    यंत्र अस्थिर(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

शून्य clearCurrentVCPU(शून्य)
{
    vcpu_t *vcpu = ARCH_गाठ_STATE(x86KSCurrentVCPU);
    जर (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_गाठ_STATE(x86KSCurrentVCPU) = रिक्त;
        जर (vcpu->fpu_active && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_active = false;
        }
    }
}

स्थिर शून्य vmptrld(शून्य *vmcs_ptr)
{
    uint64_t physical_address;
    uint8_t error;
    physical_address = pptr_to_paddr(vmcs_ptr);
    यंत्र अस्थिर(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_address)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    प्रतिज्ञा(!error);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
शून्य vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_STAR_MSR, vcpu->syscall_registers[VCPU_STAR]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_MASK]);
}

शून्य vcpu_restore_host_msrs(शून्य)
{
    init_syscall_msrs();
}
#endif

स्थिर शून्य switchVCPU(vcpu_t *vcpu)
{
#ifdef ENABLE_SMP_SUPPORT
    जर (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_गाठ_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    जर (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((अक्षर *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.शब्द[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_गाठ_STATE(x86KSCurrentVCPU) = vcpu;
}

स्थिर शून्य print_bits(word_t bits)
{
    bool_t first = true;
    पर्यंत (bits) {
        पूर्णांक index = seL4_WordBits - 1 - clzl(bits);
        जर (first) {
            म_लिखो("%d", index);
            first = false;
        } अन्यथा {
            म_लिखो(",%d", index);
        }
        bits &= ~BIT(index);
    }
}

स्थिर bool_t check_fixed_मूल्य(word_t val, word_t खालचा, word_t वरचा)
{
    word_t not_वरचा;
    word_t not_खालचा;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_वरचा = वरचा & ~(वरचा & val);
    जर (not_वरचा != 0) {
        म_लिखो("Failed to set bits: ");
        print_bits(not_वरचा);
        परत false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_खालचा = ~खालचा & ~(~खालचा & ~val);
    जर (not_खालचा != 0) {
        म_लिखो("Incorrectly cleared bits: ");
        print_bits(not_खालचा);
        परत false;
    }
    परत true;
}

स्थिर bool_t vtx_check_fixed_मूल्ये(word_t cr0, word_t cr4)
{
    जर (!check_fixed_मूल्य(cr0, cr0_खालचा, cr0_वरचा)) {
        म_लिखो(" of CR0\n");
        परत false;
    }
    जर (!check_fixed_मूल्य(cr4, cr4_खालचा, cr4_वरचा)) {
        म_लिखो(" of CR4\n");
        परत false;
    }
    परत true;
}

स्थिर bool_t BOOT_CODE init_vtx_fixed_मूल्ये(bool_t useTrueMsrs)
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
    जर (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } अन्यथा {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    pin_control_वरचा = x86_rdmsr_खालचा(pinbased_ctls);
    pin_control_खालचा = x86_rdmsr_वरचा(pinbased_ctls);
    primary_control_वरचा = x86_rdmsr_खालचा(procbased_ctls);
    primary_control_खालचा = x86_rdmsr_वरचा(procbased_ctls);
    secondary_control_वरचा = x86_rdmsr_खालचा(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_खालचा = x86_rdmsr_वरचा(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_वरचा = x86_rdmsr_खालचा(exit_ctls);
    exit_control_खालचा = x86_rdmsr_वरचा(exit_ctls);
    entry_control_वरचा = x86_rdmsr_खालचा(entry_ctls);
    entry_control_खालचा = x86_rdmsr_वरचा(entry_ctls);

    cr0_वरचा = x86_rdmsr_खालचा(IA32_VMX_CR0_FIXED0_MSR);
    cr0_खालचा = x86_rdmsr_खालचा(IA32_VMX_CR0_FIXED1_MSR);
    cr4_वरचा = x86_rdmsr_खालचा(IA32_VMX_CR4_FIXED0_MSR);
    cr4_खालचा = x86_rdmsr_खालचा(IA32_VMX_CR4_FIXED1_MSR);

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
    जर (secondary_control_खालचा & BIT(7)) {
        cr0_वरचा &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    जर (!(secondary_control_खालचा & BIT(5))) {
        vmx_feature_vpid = 0;
        म_लिखो("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } अन्यथा {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    जर (!(exit_control_खालचा & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        म_लिखो("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } अन्यथा {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    जर (!(exit_control_खालचा & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        म_लिखो("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } अन्यथा {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_खालचा) & pin_control_mask;
    जर (missing) {
        म_लिखो("vt-x: Unsupported pin control features %lx\n", (दीर्घ)missing);
        परत false;
    }
    missing = (~primary_control_खालचा) & primary_control_mask;
    जर (missing) {
        म_लिखो("vt-x: Unsupported primary control features %lx\n", (दीर्घ)missing);
        परत false;
    }
    missing = (~secondary_control_खालचा) & secondary_control_mask;
    जर (missing) {
        म_लिखो("vt-x: Unsupported secondary control features %lx\n", (दीर्घ)missing);
        परत false;
    }
    missing = (~exit_control_खालचा) & exit_control_mask;
    जर (missing) {
        म_लिखो("vt-x: Unsupported exit control features %lx\n", (दीर्घ)missing);
        परत false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_खालचा) & BIT(9);
    जर (missing) {
        म_लिखो("vt-x: CPU does not support 64-bit guests\n");
        परत false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_वरचा |= pin_control_mask;
    primary_control_वरचा |= primary_control_mask;
    secondary_control_वरचा |= secondary_control_mask;
    exit_control_वरचा |= exit_control_mask;

    परत true;
}

स्थिर bool_t BOOT_CODE check_vtx_fixed_मूल्ये(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    जर (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } अन्यथा {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    uint32_t local_pin_control_वरचा = x86_rdmsr_खालचा(pinbased_ctls);
    uint32_t local_pin_control_खालचा = x86_rdmsr_वरचा(pinbased_ctls);
    uint32_t local_primary_control_वरचा = x86_rdmsr_खालचा(procbased_ctls);
    uint32_t local_primary_control_खालचा = x86_rdmsr_वरचा(procbased_ctls);
    uint32_t local_secondary_control_वरचा = x86_rdmsr_खालचा(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_खालचा = x86_rdmsr_वरचा(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_वरचा = x86_rdmsr_खालचा(exit_ctls);
    uint32_t local_exit_control_खालचा = x86_rdmsr_वरचा(exit_ctls);
    uint32_t local_entry_control_वरचा = x86_rdmsr_खालचा(entry_ctls);
    uint32_t local_entry_control_खालचा = x86_rdmsr_वरचा(entry_ctls);

    uint32_t local_cr0_वरचा = x86_rdmsr_खालचा(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_खालचा = x86_rdmsr_खालचा(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_वरचा = x86_rdmsr_खालचा(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_खालचा = x86_rdmsr_खालचा(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    जर (local_secondary_control_खालचा & BIT(7)) {
        local_cr0_वरचा &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    परत
        (local_pin_control_वरचा & pin_control_वरचा) == local_pin_control_वरचा &&
        (~local_pin_control_खालचा & pin_control_वरचा) == 0 &&
        (local_primary_control_वरचा & primary_control_वरचा) == local_primary_control_वरचा &&
        (~local_primary_control_खालचा & primary_control_वरचा) == 0 &&
        (local_secondary_control_वरचा & secondary_control_वरचा) == local_secondary_control_वरचा &&
        (~local_secondary_control_खालचा & secondary_control_वरचा) == 0 &&
        (local_exit_control_वरचा & exit_control_वरचा) == local_exit_control_वरचा &&
        (~local_exit_control_खालचा & exit_control_वरचा) == 0 &&
        (local_entry_control_वरचा & entry_control_वरचा) == local_entry_control_वरचा &&
        (~local_entry_control_खालचा & entry_control_वरचा) == 0 &&
        local_cr0_वरचा == cr0_वरचा &&
        local_cr0_खालचा == cr0_खालचा &&
        local_cr4_वरचा == cr4_वरचा &&
        local_cr4_खालचा == cr4_खालचा;
}

स्थिर ओळीत uint32_t applyFixedBits(uint32_t original, uint32_t वरचा, uint32_t खालचा)
{
    original |= वरचा;
    original &= खालचा;
    परत original;
}

शून्य vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = रिक्त;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    स_नकल(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_वरचा & cr0_खालचा;
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
    जर (vmx_feature_load_perf_global_ctrl) {
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
    जर (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((अक्षर *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.शब्द[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_register]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_to_paddr(x64KSKernelPML4), 0).शब्द[0]);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_वरचा & pin_control_खालचा);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_वरचा & primary_control_खालचा);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_वरचा & secondary_control_खालचा);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_वरचा & exit_control_खालचा);

    uint64_t entry_control = entry_control_वरचा & entry_control_खालचा;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_वरचा & cr4_खालचा);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_वरचा, ~(word_t)0);

    स_रखो(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_SIZE);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((अक्षर *)vcpu->io + (VCPU_IOBITMAP_SIZE / 2)));
}

स्थिर शून्य dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    प्रतिज्ञा(tcb->tcbArch.tcbVCPU == vcpu);
    प्रतिज्ञा(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = रिक्त;
    vcpu->vcpuTCB = रिक्त;
}

शून्य vcpu_finalise(vcpu_t *vcpu)
{
    जर (ARCH_गाठ_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
#ifdef ENABLE_SMP_SUPPORT
        जर (vcpu->last_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->last_cpu);
        } अन्यथा
#endif /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    जर (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    जर (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

स्थिर शून्य associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    जर (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    जर (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

स्थिर exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *buffer)
{
    पूर्णांक i;
    साठी (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, buffer);
    }
    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    परत EXCEPTION_NONE;
}

स्थिर exception_t decodeVCPUWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    जर (length < 15) {
#अन्यथा
    जर (length < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    परत invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
स्थिर exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *buffer, word_t field, word_t मूल्य)
{
    tcb_t *thread;
    thread = गाठ_STATE(ksCurThread);
    जर (ARCH_गाठ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    निवड (field) {
    स्थिती IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = मूल्य;
        थांबा;
    स्थिती IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = मूल्य;
        थांबा;
    स्थिती IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = मूल्य;
        थांबा;
    स्थिती IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = मूल्य;
        थांबा;
    }
    setMR(thread, buffer, 0, मूल्य);
    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    परत EXCEPTION_NONE;
}

स्थिर exception_t decodeVCPUWriteMSR(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t मूल्य;

    जर (length < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    मूल्य = getSyscallArg(1, buffer);
    निवड (field) {
    स्थिती IA32_LSTAR_MSR:
    स्थिती IA32_STAR_MSR:
    स्थिती IA32_CSTAR_MSR:
    स्थिती IA32_FMASK_MSR:
        थांबा;
    पूर्वनिर्धारित:
        userError("VCPU WriteMSR: Invalid field %lx.", (दीर्घ)field);
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    परत invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer, field, मूल्य);
}

स्थिर exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *buffer)
{
    tcb_t *thread;
    thread = गाठ_STATE(ksCurThread);

    word_t मूल्य = 0;

    निवड (field) {
    स्थिती IA32_LSTAR_MSR:
        मूल्य = vcpu->syscall_registers[VCPU_LSTAR];
        थांबा;
    स्थिती IA32_STAR_MSR:
        मूल्य = vcpu->syscall_registers[VCPU_STAR];
        थांबा;
    स्थिती IA32_CSTAR_MSR:
        मूल्य = vcpu->syscall_registers[VCPU_CSTAR];
        थांबा;
    स्थिती IA32_FMASK_MSR:
        मूल्य = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        थांबा;
    स्थिती IA32_VMX_MISC_MSR:
        मूल्य = x86_rdmsr(field);
        थांबा;
    }

    setMR(thread, buffer, 0, मूल्य);
    setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    परत EXCEPTION_NONE;
}

स्थिर exception_t decodeVCPUReadMSR(cap_t cap, word_t length, word_t *buffer)
{
    जर (length < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    निवड (field) {
    स्थिती IA32_LSTAR_MSR:
    स्थिती IA32_STAR_MSR:
    स्थिती IA32_CSTAR_MSR:
    स्थिती IA32_FMASK_MSR:
    स्थिती IA32_VMX_MISC_MSR:
        थांबा;
    पूर्वनिर्धारित:
        userError("VCPU ReadMSR: Invalid field %lx.", (दीर्घ)field);
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    परत invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, buffer);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

स्थिर exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t खालचा, uint16_t वरचा)
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
    setIOPortMask(vcpu->io, खालचा, वरचा, false);
    परत EXCEPTION_NONE;
}

स्थिर exception_t decodeEnableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t खालचा, वरचा;

    जर (length < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    जर (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    जर (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }

    खालचा = getSyscallArg(0, buffer);
    वरचा = getSyscallArg(1, buffer);

    जर (खालचा < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || वरचा > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 0;
        परत EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    परत invokeEnableIOPort(vcpu, ioSlot, ioCap, खालचा, वरचा);
}

स्थिर exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t खालचा, uint16_t वरचा)
{
    setIOPortMask(vcpu->io, खालचा, वरचा, true);
    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    परत EXCEPTION_NONE;
}

स्थिर exception_t decodeDisableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    uint16_t खालचा, वरचा;

    जर (length < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        परत EXCEPTION_SYSCALL_ERROR;
    }

    खालचा = getSyscallArg(0, buffer);
    वरचा = getSyscallArg(1, buffer);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    परत invokeDisableIOPort(vcpu, खालचा, वरचा);
}

स्थिर exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *buffer, word_t field, word_t मूल्य)
{
    tcb_t *thread;
    thread = गाठ_STATE(ksCurThread);

    जर (ARCH_गाठ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    निवड (field) {
    स्थिती VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = मूल्य;
        थांबा;
    स्थिती VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = मूल्य;
        थांबा;
    स्थिती VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = मूल्य;
        थांबा;
    स्थिती VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = मूल्य;
        थांबा;
    }
    vmwrite(field, मूल्य);

    जर (call) {
        setRegister(thread, badgeRegister, 0);
        अचिन्हित पूर्णांक length = setMR(thread, buffer, 0, मूल्य);
        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Running);
    परत EXCEPTION_NONE;
}

स्थिर exception_t decodeWriteVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    word_t मूल्य;

    जर (length < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    मूल्य = getSyscallArg(1, buffer);
    निवड (field) {
    स्थिती VMX_GUEST_RIP:
    स्थिती VMX_GUEST_RSP:
    स्थिती VMX_GUEST_ES_SELECTOR:
    स्थिती VMX_GUEST_CS_SELECTOR:
    स्थिती VMX_GUEST_SS_SELECTOR:
    स्थिती VMX_GUEST_DS_SELECTOR:
    स्थिती VMX_GUEST_FS_SELECTOR:
    स्थिती VMX_GUEST_GS_SELECTOR:
    स्थिती VMX_GUEST_LDTR_SELECTOR:
    स्थिती VMX_GUEST_TR_SELECTOR:
    स्थिती VMX_GUEST_DEBUGCTRL:
    स्थिती VMX_GUEST_PAT:
    स्थिती VMX_GUEST_EFER:
    स्थिती VMX_GUEST_PERF_GLOBAL_CTRL:
    स्थिती VMX_GUEST_PDPTE0:
    स्थिती VMX_GUEST_PDPTE1:
    स्थिती VMX_GUEST_PDPTE2:
    स्थिती VMX_GUEST_PDPTE3:
    स्थिती VMX_GUEST_ES_LIMIT:
    स्थिती VMX_GUEST_CS_LIMIT:
    स्थिती VMX_GUEST_SS_LIMIT:
    स्थिती VMX_GUEST_DS_LIMIT:
    स्थिती VMX_GUEST_FS_LIMIT:
    स्थिती VMX_GUEST_GS_LIMIT:
    स्थिती VMX_GUEST_LDTR_LIMIT:
    स्थिती VMX_GUEST_TR_LIMIT:
    स्थिती VMX_GUEST_GDTR_LIMIT:
    स्थिती VMX_GUEST_IDTR_LIMIT:
    स्थिती VMX_GUEST_ES_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_CS_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_SS_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_DS_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_FS_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_GS_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_LDTR_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_TR_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_INTERRUPTABILITY:
    स्थिती VMX_GUEST_ACTIVITY:
    स्थिती VMX_GUEST_SMBASE:
    स्थिती VMX_GUEST_SYSENTER_CS:
    स्थिती VMX_GUEST_PREEMPTION_TIMER_मूल्य:
    स्थिती VMX_GUEST_ES_BASE:
    स्थिती VMX_GUEST_CS_BASE:
    स्थिती VMX_GUEST_SS_BASE:
    स्थिती VMX_GUEST_DS_BASE:
    स्थिती VMX_GUEST_FS_BASE:
    स्थिती VMX_GUEST_GS_BASE:
    स्थिती VMX_GUEST_LDTR_BASE:
    स्थिती VMX_GUEST_TR_BASE:
    स्थिती VMX_GUEST_GDTR_BASE:
    स्थिती VMX_GUEST_IDTR_BASE:
    स्थिती VMX_GUEST_DR7:
    स्थिती VMX_GUEST_RFLAGS:
    स्थिती VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    स्थिती VMX_GUEST_SYSENTER_ESP:
    स्थिती VMX_GUEST_SYSENTER_EIP:
    स्थिती VMX_CONTROL_CR0_MASK:
    स्थिती VMX_CONTROL_CR4_MASK:
    स्थिती VMX_CONTROL_CR0_READ_SHADOW:
    स्थिती VMX_CONTROL_CR4_READ_SHADOW:
    स्थिती VMX_GUEST_CR3:
    स्थिती VMX_CONTROL_EXCEPTION_BITMAP:
    स्थिती VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    स्थिती VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        थांबा;
    स्थिती VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        मूल्य = applyFixedBits(मूल्य, pin_control_वरचा, pin_control_खालचा);
        थांबा;
    स्थिती VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        मूल्य = applyFixedBits(मूल्य, primary_control_वरचा, primary_control_खालचा);
        थांबा;
    स्थिती VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        मूल्य = applyFixedBits(मूल्य, secondary_control_वरचा, secondary_control_खालचा);
        थांबा;
    स्थिती VMX_CONTROL_ENTRY_CONTROLS:
        मूल्य = applyFixedBits(मूल्य, entry_control_वरचा, entry_control_खालचा);
        थांबा;
    स्थिती VMX_CONTROL_EXIT_CONTROLS:
        मूल्य = applyFixedBits(मूल्य, exit_control_वरचा, exit_control_खालचा);
        थांबा;
    स्थिती VMX_GUEST_CR0:
        मूल्य = applyFixedBits(मूल्य, cr0_वरचा, cr0_खालचा);
        थांबा;
    स्थिती VMX_GUEST_CR4:
        मूल्य = applyFixedBits(मूल्य, cr4_वरचा, cr4_खालचा);
        थांबा;
    पूर्वनिर्धारित:
        userError("VCPU WriteVMCS: Invalid field %lx.", (दीर्घ)field);
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    परत invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, buffer, field, मूल्य);
}

स्थिर word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    निवड (field) {
    स्थिती VMX_CONTROL_EXCEPTION_BITMAP:
        परत vcpu->exception_bitmap;
    स्थिती VMX_GUEST_CR0:
        परत vcpu->cr0;
    स्थिती VMX_CONTROL_CR0_MASK:
        परत vcpu->cr0_mask;
    स्थिती VMX_CONTROL_CR0_READ_SHADOW:
        परत vcpu->cr0_shadow;
    }
    जर (ARCH_गाठ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    परत vmread(field);
}

स्थिर exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *buffer)
{
    tcb_t *thread;
    thread = गाठ_STATE(ksCurThread);
    word_t मूल्य = readVMCSField(vcpu, field);
    जर (call) {
        setRegister(thread, badgeRegister, 0);
        अचिन्हित पूर्णांक length = setMR(thread, buffer, 0, मूल्य);
        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Running);
    परत EXCEPTION_NONE;
}

स्थिर exception_t decodeReadVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    जर (length < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    निवड (field) {
    स्थिती VMX_GUEST_RIP:
    स्थिती VMX_GUEST_RSP:
    स्थिती VMX_GUEST_ES_SELECTOR:
    स्थिती VMX_GUEST_CS_SELECTOR:
    स्थिती VMX_GUEST_SS_SELECTOR:
    स्थिती VMX_GUEST_DS_SELECTOR:
    स्थिती VMX_GUEST_FS_SELECTOR:
    स्थिती VMX_GUEST_GS_SELECTOR:
    स्थिती VMX_GUEST_LDTR_SELECTOR:
    स्थिती VMX_GUEST_TR_SELECTOR:
    स्थिती VMX_GUEST_DEBUGCTRL:
    स्थिती VMX_GUEST_PAT:
    स्थिती VMX_GUEST_EFER:
    स्थिती VMX_GUEST_PERF_GLOBAL_CTRL:
    स्थिती VMX_GUEST_PDPTE0:
    स्थिती VMX_GUEST_PDPTE1:
    स्थिती VMX_GUEST_PDPTE2:
    स्थिती VMX_GUEST_PDPTE3:
    स्थिती VMX_GUEST_ES_LIMIT:
    स्थिती VMX_GUEST_CS_LIMIT:
    स्थिती VMX_GUEST_SS_LIMIT:
    स्थिती VMX_GUEST_DS_LIMIT:
    स्थिती VMX_GUEST_FS_LIMIT:
    स्थिती VMX_GUEST_GS_LIMIT:
    स्थिती VMX_GUEST_LDTR_LIMIT:
    स्थिती VMX_GUEST_TR_LIMIT:
    स्थिती VMX_GUEST_GDTR_LIMIT:
    स्थिती VMX_GUEST_IDTR_LIMIT:
    स्थिती VMX_GUEST_ES_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_CS_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_SS_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_DS_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_FS_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_GS_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_LDTR_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_TR_ACCESS_RIGHTS:
    स्थिती VMX_GUEST_INTERRUPTABILITY:
    स्थिती VMX_GUEST_ACTIVITY:
    स्थिती VMX_GUEST_SMBASE:
    स्थिती VMX_GUEST_SYSENTER_CS:
    स्थिती VMX_GUEST_PREEMPTION_TIMER_मूल्य:
    स्थिती VMX_GUEST_ES_BASE:
    स्थिती VMX_GUEST_CS_BASE:
    स्थिती VMX_GUEST_SS_BASE:
    स्थिती VMX_GUEST_DS_BASE:
    स्थिती VMX_GUEST_FS_BASE:
    स्थिती VMX_GUEST_GS_BASE:
    स्थिती VMX_GUEST_LDTR_BASE:
    स्थिती VMX_GUEST_TR_BASE:
    स्थिती VMX_GUEST_GDTR_BASE:
    स्थिती VMX_GUEST_IDTR_BASE:
    स्थिती VMX_GUEST_DR7:
    स्थिती VMX_GUEST_RFLAGS:
    स्थिती VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    स्थिती VMX_GUEST_SYSENTER_ESP:
    स्थिती VMX_GUEST_SYSENTER_EIP:
    स्थिती VMX_CONTROL_CR0_MASK:
    स्थिती VMX_CONTROL_CR4_MASK:
    स्थिती VMX_CONTROL_CR0_READ_SHADOW:
    स्थिती VMX_CONTROL_CR4_READ_SHADOW:
    स्थिती VMX_DATA_INSTRUCTION_ERROR:
    स्थिती VMX_DATA_EXIT_INTERRUPT_INFO:
    स्थिती VMX_DATA_EXIT_INTERRUPT_ERROR:
    स्थिती VMX_DATA_IDT_VECTOR_INFO:
    स्थिती VMX_DATA_IDT_VECTOR_ERROR:
    स्थिती VMX_DATA_EXIT_INSTRUCTION_LENGTH:
    स्थिती VMX_DATA_EXIT_INSTRUCTION_INFO:
    स्थिती VMX_DATA_GUEST_PHYSICAL:
    स्थिती VMX_DATA_IO_RCX:
    स्थिती VMX_DATA_IO_RSI:
    स्थिती VMX_DATA_IO_RDI:
    स्थिती VMX_DATA_IO_RIP:
    स्थिती VMX_DATA_GUEST_LINEAR_ADDRESS:
    स्थिती VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    स्थिती VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    स्थिती VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    स्थिती VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    स्थिती VMX_CONTROL_EXCEPTION_BITMAP:
    स्थिती VMX_CONTROL_ENTRY_CONTROLS:
    स्थिती VMX_CONTROL_EXIT_CONTROLS:
    स्थिती VMX_GUEST_CR0:
    स्थिती VMX_GUEST_CR3:
    स्थिती VMX_GUEST_CR4:
        थांबा;
    पूर्वनिर्धारित:
        userError("VCPU ReadVMCS: Invalid field %lx.", (दीर्घ)field);
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    परत invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, buffer);
}

स्थिर exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    परत EXCEPTION_NONE;
}

स्थिर exception_t decodeSetTCB(cap_t cap, word_t length, word_t *buffer)
{
    cap_t tcbCap;
    जर (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    जर (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }

    परत invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

शून्य vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *buffer;
    जर (ARCH_गाठ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    buffer = lookupIPCBuffer(false, गाठ_STATE(ksCurThread));
    जर (!buffer) {
        userError("No IPC buffer.");
        परत;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, buffer));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, buffer), primary_control_वरचा,
                                                                   primary_control_खालचा));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, buffer));
}

शून्य vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *buffer;
    vcpu_t *vcpu;

    buffer = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    प्रतिज्ञा(vcpu);

    जर (ARCH_गाठ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, संदेश_Info_Register, 0);
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
    निवड (invLabel) {
    स्थिती X86VCPUSetTCB:
        परत decodeSetTCB(cap, length, buffer);
    स्थिती X86VCPUReadVMCS:
        परत decodeReadVMCS(cap, length, call, buffer);
    स्थिती X86VCPUWriteVMCS:
        परत decodeWriteVMCS(cap, length, call, buffer);
    स्थिती X86VCPUEnableIOPort:
        परत decodeEnableIOPort(cap, length, buffer);
    स्थिती X86VCPUDisableIOPort:
        परत decodeDisableIOPort(cap, length, buffer);
    स्थिती X86VCPUWriteRegisters:
        परत decodeVCPUWriteRegisters(cap, length, buffer);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    स्थिती X86VCPUWriteMSR:
        परत decodeVCPUWriteMSR(cap, length, buffer);
    स्थिती X86VCPUReadMSR:
        परत decodeVCPUReadMSR(cap, length, buffer);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    पूर्वनिर्धारित:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }
}

स्थिर bool_t is_vtx_supported(शून्य)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    परत !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

स्थिर ओळीत शून्य clear_bit(word_t *bitmap, word_t bit)
{
    पूर्णांक index = bit / (आकार(word_t) * 8);
    पूर्णांक offset = bit % (आकार(word_t) * 8);
    bitmap[index] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(शून्य)
{
    जर (!is_vtx_supported()) {
        म_लिखो("vt-x: not supported\n");
        परत false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.शब्द[0] = x86_rdmsr_खालचा(IA32_VMX_BASIC_MSR);
    vmx_basic.शब्द[1] = x86_rdmsr_वरचा(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.शब्द[0] = x86_rdmsr_खालचा(IA32_FEATURE_CONTROL_MSR);
    जर (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        जर (feature_control_msr_get_lock(feature_control)) {
            म_लिखो("vt-x: feature locked\n");
            परत false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_वरचा(IA32_FEATURE_CONTROL_MSR), feature_control.शब्द[0]);
    }
    /* make sure the msr is locked */
    जर (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_वरचा(IA32_FEATURE_CONTROL_MSR), feature_control.शब्द[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    जर (CURRENT_CPU_INDEX() == 0) {
        जर (!init_vtx_fixed_मूल्ये(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            म_लिखो("vt-x: lack of required features\n");
            परत false;
        }
    }
    जर (!check_vtx_fixed_मूल्ये(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        म_लिखो("vt-x: cores have inconsistent features\n");
        परत false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    संरचना vmxon_region *vmxon_kptr = &ARCH_गाठ_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    जर (!vtx_check_fixed_मूल्ये(read_cr0(), read_cr4())) {
        परत false;
    }
    जर (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        म_लिखो("vt-x: vmxon failure\n");
        परत false;
    }
    स_रखो(&msr_bitmap_region, ~0, आकार(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.खालचा_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.खालचा_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.खालचा_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.खालचा_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.खालचा_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.खालचा_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.वरचा_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.वरचा_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.वरचा_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.वरचा_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.वरचा_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.वरचा_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.शब्द[0] = x86_rdmsr_खालचा(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.शब्द[1] = x86_rdmsr_वरचा(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    जर (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        म_लिखो("vt-x: Expected wb attribute for EPT paging structure\n");
        परत false;
    }
    जर (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        म_लिखो("vt-x: Expected supported for 2m pages\n");
        परत false;
    }

    परत true;
}

स्थिर शून्य setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *buffer;
    पूर्णांक i;

    buffer = lookupIPCBuffer(true, गाठ_STATE(ksCurThread));

    setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_DATA_EXIT_INSTRUCTION_LENGTH));
    setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_DATA_GUEST_PHYSICAL));
    setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    साठी (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(गाठ_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_EAX + i,
              गाठ_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

स्थिर शून्य handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(गाठ_STATE(ksCurThread), संदेश_Info_Register, SEL4_VMENTER_RESULT_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

स्थिर ओळीत शून्य finishVmexitSaving(शून्य)
{
    vcpu_t *vcpu = ARCH_गाठ_STATE(x86KSCurrentVCPU);
    प्रतिज्ञा(vcpu == गाठ_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(शून्य)
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
    जर (reason == EXTERNAL_INTERRUPT) {
        गाठ_LOCK_IRQ;
        जर (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_DATA_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_गाठ_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } अन्यथा {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(गाठ_STATE(ksCurThread));
#endif
        परत EXCEPTION_NONE;
    }

    गाठ_LOCK_SYS;

    निवड (reason) {
    स्थिती EXCEPTION_OR_NMI:
    स्थिती MOV_DR:
    स्थिती TASK_SWITCH:
    स्थिती CONTROL_REGISTER:
    स्थिती IO:
    स्थिती MWAIT:
    स्थिती SIPI:
    स्थिती INVLPG:
    स्थिती INVEPT:
    स्थिती INVVPID:
    स्थिती VMCLEAR:
    स्थिती VMPTRLD:
    स्थिती VMPTRST:
    स्थिती VMREAD:
    स्थिती VMWRITE:
    स्थिती VMXON:
    स्थिती EPT_VIOLATION:
    स्थिती GDTR_OR_IDTR:
    स्थिती LDTR_OR_TR:
    स्थिती TPR_BELOW_THRESHOLD:
    स्थिती APIC_ACCESS:
        qualification = vmread(VMX_DATA_EXIT_QUALIFICATION);
        थांबा;
    पूर्वनिर्धारित:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    परत EXCEPTION_NONE;
}

exception_t handleVmEntryFail(शून्य)
{
    handleVmxFault(-1, -1);

    परत EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
शून्य VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    प्रतिज्ञा(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    जर (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {

        word_t badge = notification_ptr_get_ntfn_संदेश_Identifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        जर (tcb != गाठ_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

स्थिर शून्य invvpid_context(uint16_t vpid)
{
    संरचना {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    यंत्र अस्थिर("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
}

स्थिर शून्य setEPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_मूळ;
    जर (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_मूळ = kpptr_to_paddr(null_ept_space);
    } अन्यथा {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        जर (find_ret.status != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_मूळ = kpptr_to_paddr(null_ept_space);
        } अन्यथा {
            ept_मूळ = pptr_to_paddr(pml4);
        }
    }
    जर (ept_मूळ != vcpu->last_ept_मूळ) {
        vcpu->last_ept_मूळ = ept_मूळ;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_मूळ,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.शब्द[0]);
        प्रतिज्ञा(vcpu->vpid != VPID_INVALID);
        जर (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

शून्य clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t last)
{
    जर (vpid == VPID_INVALID) {
        परत;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    जर (vcpu == रिक्त) {
        परत;
    }
    प्रतिज्ञा(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

स्थिर ओळीत vpid_t nextVPID(vpid_t vpid)
{
    जर (vpid == VPID_LAST) {
        परत VPID_FIRST;
    } अन्यथा {
        परत vpid + 1;
    }
}

स्थिर शून्य invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    स_रखो(vcpu->io, ~0, आकार(vcpu->io));
    /* invalidate the VPID context */
    जर (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
स्थिर शून्य releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = रिक्त;
}

स्थिर vpid_t findFreeVPID(शून्य)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    करा {
        जर (x86KSVPIDTable[vpid] == रिक्त) {
            परत vpid;
        }
        vpid = nextVPID(vpid);
    } पर्यंत (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = nextVPID(x86KSNextVPID);
    परत vpid;
}

स्थिर शून्य storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    प्रतिज्ञा(x86KSVPIDTable[vpid] == रिक्त);
    प्रतिज्ञा(vcpu->vpid == VPID_INVALID);
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
शून्य vcpu_fpu_to_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    जर (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        जर (!vcpu->fpu_active) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_active = true;
            loadFpuState(tcb);
        }
    } अन्यथा {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_active = true;
        switchLocalFpuOwner(tcb);
    }
}

शून्य vcpu_fpu_to_host(tcb_t *tcb, vcpu_t *vcpu)
{
    जर (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } अन्यथा जर (nativeThreadUsingFPU(tcb) && vcpu->fpu_active) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_active = false;
        loadFpuState(tcb);
    } अन्यथा जर (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

शून्य restoreVMCS(शून्य)
{
    tcb_t *cur_thread = गाठ_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    जर (ARCH_गाठ_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ifndef CONFIG_KERNEL_SKIM_WINDOW
    जर (getCurrentCR3().शब्द[0] != expected_vmcs->last_host_cr3) {
        expected_vmcs->last_host_cr3 = getCurrentCR3().शब्द[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().शब्द[0]);
    }
#endif
    जर (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        जर (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    setEPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

शून्य invept(ept_pml4e_t *ept_pml4)
{
    जर (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        संरचना {
            uint64_t parts[2];
        } address;
        word_t type;
        जर (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } अन्यथा जर (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } अन्यथा {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            परत;
        }

        address.parts[0] = pptr_to_paddr((शून्य *)ept_pml4);
        address.parts[1] = 0;
        यंत्र अस्थिर(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
