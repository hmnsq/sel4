/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_VTX

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

#परिभाषय VMX_EXIT_QUAL_TYPE_MOV_CR 0
#परिभाषय VMX_EXIT_QUAL_TYPE_CLTS 2
#परिभाषय VMX_EXIT_QUAL_TYPE_LMSW 3

ध्रुव vcpu_gp_पञ्जिकाकोशः_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#यदि_परिभाषितम् CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#यदि_अन्तः /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#परिभाषय MSR_BITMAP_आवरणम्(x) ((x) & 0x1fff)

प्रकारनिर्देश संरचना msr_bitmap {
    word_t bitmap[0x2000 / परिमाण(word_t) / 8];
} msr_bitmap_t;

प्रकारनिर्देश संरचना msr_bitmaps {
    msr_bitmap_t नीचः_msr_read;
    msr_bitmap_t उच्चः_msr_read;
    msr_bitmap_t नीचः_msr_write;
    msr_bitmap_t उच्चः_msr_write;
} msr_bitmaps_t;

स्थिर msr_bitmaps_t msr_bitmap_region ALIGN(BIT(se_L4_Page_द्वयङ्काः));

स्थिर वर्ण null_ept_space[BIT(se_L4_Page_द्वयङ्काः)] ALIGN(BIT(se_L4_Page_द्वयङ्काः));

/* Cached value of the hardware defined vmcs revision */
स्थिर uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
स्थिर vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
स्थिर uint32_t pin_control_उच्चः;
स्थिर uint32_t pin_control_नीचः;
स्थिर uint32_t primary_control_उच्चः;
स्थिर uint32_t primary_control_नीचः;
स्थिर uint32_t secondary_control_उच्चः;
स्थिर uint32_t secondary_control_नीचः;
स्थिर uint32_t प्रविष्टिः_control_उच्चः;
स्थिर uint32_t प्रविष्टिः_control_नीचः;
स्थिर uint32_t exit_control_उच्चः;
स्थिर uint32_t exit_control_नीचः;
स्थिर uint32_t cr0_उच्चः;
स्थिर uint32_t cr0_नीचः;
स्थिर uint32_t cr4_उच्चः;
स्थिर uint32_t cr4_नीचः;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
स्थिर bool_t vmx_feature_vpid;
स्थिर bool_t vmx_feature_आरोपय_perf_global_ctrl;
स्थिर bool_t vmx_feature_ack_on_exit;

स्थिर vcpu_t *x86KSVPIDTable[VPID_चरमः + 1];
स्थिर vpid_t x86KSNextVPID = VPID_FIRST;

स्थिर शून्य releaseVPID(vpid_t vpid);

स्थिर अन्तःस्थ bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm चञ्चल(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    प्रत्यागम !!error;
}

स्थिर शून्य vmclear(शून्य *vmcs_ptr)
{
    uint64_t physical_स्थानाङ्कः;
    physical_स्थानाङ्कः = pptr_गन्तव्यम्_paddr((शून्य *)vmcs_ptr);
    asm चञ्चल(
        "vmclear %0"
        :
        : "m"(physical_स्थानाङ्कः)
        : "cc"
    );
}

शून्य clearCurrentVCPU(शून्य)
{
    vcpu_t *vcpu = ARCH_ग्रन्थिः_STATE(x86KSCurrentVCPU);
    यदि (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_ग्रन्थिः_STATE(x86KSCurrentVCPU) = रिक्त;
        यदि (vcpu->fpu_सक्रिय && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_सक्रिय = false;
        }
    }
}

स्थिर शून्य vmptrld(शून्य *vmcs_ptr)
{
    uint64_t physical_स्थानाङ्कः;
    uint8_t error;
    physical_स्थानाङ्कः = pptr_गन्तव्यम्_paddr(vmcs_ptr);
    asm चञ्चल(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_स्थानाङ्कः)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    प्रतिज्ञा(!error);
}

#यदि_परिभाषितम् CONFIG_X86_64_VTX_64BIT_GUESTS
शून्य vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_तारकम्_MSR, vcpu->syscall_registers[VCPU_तारकम्]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_आवरणम्]);
}

शून्य vcpu_restore_host_msrs(शून्य)
{
    init_syscall_msrs();
}
#यदि_अन्तः

स्थिर शून्य switchVCPU(vcpu_t *vcpu)
{
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    यदि (vcpu->चरमः_cpu != getCurrentCPUIndex() && ARCH_ग्रन्थिः_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->चरमः_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->चरमः_cpu);
    }
#यदि_अन्तः
    clearCurrentVCPU();
    vmptrld(vcpu);
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    यदि (vcpu->चरमः_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_आधारः, (word_t)&x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KStss);
        vmwrite(VMX_HOST_GDTR_आधारः, (word_t)x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_आधारः, (word_t)x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((वर्ण *)&x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KStss.tss.शब्दाः[0] +
                                                          4));
    }
    vcpu->चरमः_cpu = getCurrentCPUIndex();
#यदि_अन्तः
    ARCH_ग्रन्थिः_STATE(x86KSCurrentVCPU) = vcpu;
}

स्थिर शून्य print_द्वयङ्काः(word_t द्वयङ्काः)
{
    bool_t first = true;
    यावत् (द्वयङ्काः) {
        पूर्णाङ्क सूचकांक = se_L4_Word_द्वयङ्काः - 1 - clzl(द्वयङ्काः);
        यदि (first) {
            प्रारूप_लिख("%d", सूचकांक);
            first = false;
        } अन्यथा {
            प्रारूप_लिख(",%d", सूचकांक);
        }
        द्वयङ्काः &= ~BIT(सूचकांक);
    }
}

स्थिर bool_t check_fixed_मानम्(word_t val, word_t नीचः, word_t उच्चः)
{
    word_t not_उच्चः;
    word_t not_नीचः;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_उच्चः = उच्चः & ~(उच्चः & val);
    यदि (not_उच्चः != 0) {
        प्रारूप_लिख("Failed to set bits: ");
        print_द्वयङ्काः(not_उच्चः);
        प्रत्यागम false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_नीचः = ~नीचः & ~(~नीचः & ~val);
    यदि (not_नीचः != 0) {
        प्रारूप_लिख("Incorrectly cleared bits: ");
        print_द्वयङ्काः(not_नीचः);
        प्रत्यागम false;
    }
    प्रत्यागम true;
}

स्थिर bool_t vtx_check_fixed_मानानि(word_t cr0, word_t cr4)
{
    यदि (!check_fixed_मानम्(cr0, cr0_नीचः, cr0_उच्चः)) {
        प्रारूप_लिख(" of CR0\n");
        प्रत्यागम false;
    }
    यदि (!check_fixed_मानम्(cr4, cr4_नीचः, cr4_उच्चः)) {
        प्रारूप_लिख(" of CR4\n");
        प्रत्यागम false;
    }
    प्रत्यागम true;
}

स्थिर bool_t BOOT_संहिता init_vtx_fixed_मानानि(bool_t useTrueMsrs)
{
    uint32_t pin_control_आवरणम् =
        BIT(0) |    //Extern interrupt exiting
        BIT(3) |    //NMI exiting
        BIT(5);     //virtual NMIs
    uint32_t primary_control_आवरणम् =
        BIT(25) |   //Use I/O bitmaps
        BIT(28) |   //Use MSR bitmaps
        BIT(31);    //Activate secondary controls
    uint32_t secondary_control_आवरणम् =
        BIT(1);     //Enable EPT
    uint32_t exit_control_आवरणम् =
        BIT(2)  |   //Save debug controls
        BIT(18) |   //Save guest IA32_PAT on exit
        BIT(19) |   //Load host IA32_PAT
        BIT(20) |   //Save guest IA32_EFER on exit
        BIT(21);    //Load host IA32_EFER
#यदि_परिभाषितम् CONFIG_ARCH_X86_64
    exit_control_आवरणम् |= BIT(9); //Host address-space size
#यदि_अन्तः /* CONFIG_ARCH_X86_64 */
    /* Read out the fixed high and low bits from the MSRs */
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t प्रविष्टिः_ctls;
    यदि (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        प्रविष्टिः_ctls = IA32_VMX_TRUE_प्रविष्टिः_CTLS_MSR;
    } अन्यथा {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        प्रविष्टिः_ctls = IA32_VMX_प्रविष्टिः_CTLS_MSR;
    }
    pin_control_उच्चः = x86_rdmsr_नीचः(pinbased_ctls);
    pin_control_नीचः = x86_rdmsr_उच्चः(pinbased_ctls);
    primary_control_उच्चः = x86_rdmsr_नीचः(procbased_ctls);
    primary_control_नीचः = x86_rdmsr_उच्चः(procbased_ctls);
    secondary_control_उच्चः = x86_rdmsr_नीचः(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_नीचः = x86_rdmsr_उच्चः(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_उच्चः = x86_rdmsr_नीचः(exit_ctls);
    exit_control_नीचः = x86_rdmsr_उच्चः(exit_ctls);
    प्रविष्टिः_control_उच्चः = x86_rdmsr_नीचः(प्रविष्टिः_ctls);
    प्रविष्टिः_control_नीचः = x86_rdmsr_उच्चः(प्रविष्टिः_ctls);

    cr0_उच्चः = x86_rdmsr_नीचः(IA32_VMX_CR0_FIXED0_MSR);
    cr0_नीचः = x86_rdmsr_नीचः(IA32_VMX_CR0_FIXED1_MSR);
    cr4_उच्चः = x86_rdmsr_नीचः(IA32_VMX_CR4_FIXED0_MSR);
    cr4_नीचः = x86_rdmsr_नीचः(IA32_VMX_CR4_FIXED1_MSR);

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
    यदि (secondary_control_नीचः & BIT(7)) {
        cr0_उच्चः &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    यदि (!(secondary_control_नीचः & BIT(5))) {
        vmx_feature_vpid = 0;
        प्रारूप_लिख("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } अन्यथा {
        vmx_feature_vpid = 1;
        secondary_control_आवरणम् |= BIT(5);
    }

    /* Check for load perf global control */
    यदि (!(exit_control_नीचः & BIT(12))) {
        vmx_feature_आरोपय_perf_global_ctrl = 0;
        प्रारूप_लिख("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } अन्यथा {
        vmx_feature_आरोपय_perf_global_ctrl = 1;
        exit_control_आवरणम् |= BIT(12);
    }

    /* Check for external interrupt exiting */
    यदि (!(exit_control_नीचः & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        प्रारूप_लिख("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } अन्यथा {
        vmx_feature_ack_on_exit = 1;
        exit_control_आवरणम् |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_नीचः) & pin_control_आवरणम्;
    यदि (missing) {
        प्रारूप_लिख("vt-x: Unsupported pin control features %lx\n", (दीर्घ)missing);
        प्रत्यागम false;
    }
    missing = (~primary_control_नीचः) & primary_control_आवरणम्;
    यदि (missing) {
        प्रारूप_लिख("vt-x: Unsupported primary control features %lx\n", (दीर्घ)missing);
        प्रत्यागम false;
    }
    missing = (~secondary_control_नीचः) & secondary_control_आवरणम्;
    यदि (missing) {
        प्रारूप_लिख("vt-x: Unsupported secondary control features %lx\n", (दीर्घ)missing);
        प्रत्यागम false;
    }
    missing = (~exit_control_नीचः) & exit_control_आवरणम्;
    यदि (missing) {
        प्रारूप_लिख("vt-x: Unsupported exit control features %lx\n", (दीर्घ)missing);
        प्रत्यागम false;
    }
#यदि_परिभाषितम् CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~प्रविष्टिः_control_नीचः) & BIT(9);
    यदि (missing) {
        प्रारूप_लिख("vt-x: CPU does not support 64-bit guests\n");
        प्रत्यागम false;
    }
#यदि_अन्तः /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_उच्चः |= pin_control_आवरणम्;
    primary_control_उच्चः |= primary_control_आवरणम्;
    secondary_control_उच्चः |= secondary_control_आवरणम्;
    exit_control_उच्चः |= exit_control_आवरणम्;

    प्रत्यागम true;
}

स्थिर bool_t BOOT_संहिता check_vtx_fixed_मानानि(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t प्रविष्टिः_ctls;
    यदि (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        प्रविष्टिः_ctls = IA32_VMX_TRUE_प्रविष्टिः_CTLS_MSR;
    } अन्यथा {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        प्रविष्टिः_ctls = IA32_VMX_प्रविष्टिः_CTLS_MSR;
    }
    uint32_t local_pin_control_उच्चः = x86_rdmsr_नीचः(pinbased_ctls);
    uint32_t local_pin_control_नीचः = x86_rdmsr_उच्चः(pinbased_ctls);
    uint32_t local_primary_control_उच्चः = x86_rdmsr_नीचः(procbased_ctls);
    uint32_t local_primary_control_नीचः = x86_rdmsr_उच्चः(procbased_ctls);
    uint32_t local_secondary_control_उच्चः = x86_rdmsr_नीचः(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_नीचः = x86_rdmsr_उच्चः(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_उच्चः = x86_rdmsr_नीचः(exit_ctls);
    uint32_t local_exit_control_नीचः = x86_rdmsr_उच्चः(exit_ctls);
    uint32_t local_प्रविष्टिः_control_उच्चः = x86_rdmsr_नीचः(प्रविष्टिः_ctls);
    uint32_t local_प्रविष्टिः_control_नीचः = x86_rdmsr_उच्चः(प्रविष्टिः_ctls);

    uint32_t local_cr0_उच्चः = x86_rdmsr_नीचः(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_नीचः = x86_rdmsr_नीचः(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_उच्चः = x86_rdmsr_नीचः(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_नीचः = x86_rdmsr_नीचः(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    यदि (local_secondary_control_नीचः & BIT(7)) {
        local_cr0_उच्चः &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    प्रत्यागम
        (local_pin_control_उच्चः & pin_control_उच्चः) == local_pin_control_उच्चः &&
        (~local_pin_control_नीचः & pin_control_उच्चः) == 0 &&
        (local_primary_control_उच्चः & primary_control_उच्चः) == local_primary_control_उच्चः &&
        (~local_primary_control_नीचः & primary_control_उच्चः) == 0 &&
        (local_secondary_control_उच्चः & secondary_control_उच्चः) == local_secondary_control_उच्चः &&
        (~local_secondary_control_नीचः & secondary_control_उच्चः) == 0 &&
        (local_exit_control_उच्चः & exit_control_उच्चः) == local_exit_control_उच्चः &&
        (~local_exit_control_नीचः & exit_control_उच्चः) == 0 &&
        (local_प्रविष्टिः_control_उच्चः & प्रविष्टिः_control_उच्चः) == local_प्रविष्टिः_control_उच्चः &&
        (~local_प्रविष्टिः_control_नीचः & प्रविष्टिः_control_उच्चः) == 0 &&
        local_cr0_उच्चः == cr0_उच्चः &&
        local_cr0_नीचः == cr0_नीचः &&
        local_cr4_उच्चः == cr4_उच्चः &&
        local_cr4_नीचः == cr4_नीचः;
}

स्थिर अन्तःस्थ uint32_t apply_Fixed_द्वयङ्काः(uint32_t original, uint32_t उच्चः, uint32_t नीचः)
{
    original |= उच्चः;
    original &= नीचः;
    प्रत्यागम original;
}

शून्य vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = रिक्त;
    vcpu->launched = false;
    vcpu->fpu_सक्रिय = false;
    vcpu->fpuState = x86KSnullFpuState;

    स्मृतिं_प्रतिलिख(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_उच्चः & cr0_नीचः;
    vcpu->cr0_shadow = 0;
    vcpu->cr0_आवरणम् = 0;
    vcpu->exception_bitmap = 0;
    vcpu->vpid = VPID_INVALID;
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    vcpu->चरमः_cpu = getCurrentCPUIndex();
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

    switchVCPU(vcpu);

    vmwrite(VMX_HOST_PAT, x86_rdmsr(IA32_PAT_MSR));
    vmwrite(VMX_HOST_EFER, x86_rdmsr(IA32_EFER_MSR));
    // By default we will disable performance counters when we come out
    // of a VM. When performance counters are supported this host state
    // needs to be updated on VM entry
    यदि (vmx_feature_आरोपय_perf_global_ctrl) {
        vmwrite(VMX_HOST_PERF_GLOBAL_CTRL, 0);
    }
    vmwrite(VMX_HOST_CR0, read_cr0());
    vmwrite(VMX_HOST_CR4, read_cr4());
    vmwrite(VMX_HOST_FS_आधारः, 0);
    vmwrite(VMX_HOST_GS_आधारः, 0);
    vmwrite(VMX_HOST_TR_आधारः, (word_t)&x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KStss);
    vmwrite(VMX_HOST_GDTR_आधारः, (word_t)x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KSgdt);
    vmwrite(VMX_HOST_IDTR_आधारः, (word_t)x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KSidt);
    vmwrite(VMX_HOST_SYSENTER_CS, (word_t)SEL_CS_0);
    vmwrite(VMX_HOST_SYSENTER_EIP, (word_t)&handle_syscall);
    यदि (!config_समुच्चयः(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((वर्ण *)&x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KStss.tss.शब्दाः[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_पञ्जिकाकोशः]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#यदि_परिभाषितम् CONFIG_KERNEL_SKIM_गवाक्षः
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_गन्तव्यम्_paddr(x64KSKernelPML4), 0).शब्दाः[0]);
#यदि_अन्तः /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_उच्चः & pin_control_नीचः);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_उच्चः & primary_control_नीचः);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_उच्चः & secondary_control_नीचः);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_उच्चः & exit_control_नीचः);

    uint64_t प्रविष्टिः_control = प्रविष्टिः_control_उच्चः & प्रविष्टिः_control_नीचः;
#यदि_परिभाषितम् CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    प्रविष्टिः_control |= BIT(9);
#यदि_अन्तः /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_प्रविष्टिः_CONTROLS, प्रविष्टिः_control);

    vmwrite(VMX_CONTROL_MSR_स्थानाङ्कः, (word_t)kpptr_गन्तव्यम्_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_उच्चः & cr4_नीचः);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_उच्चः, ~(word_t)0);

    स्मृतिं_पूरय(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_आकार);
    vmwrite(VMX_CONTROL_IOA_स्थानाङ्कः, pptr_गन्तव्यम्_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_स्थानाङ्कः, pptr_गन्तव्यम्_paddr((वर्ण *)vcpu->io + (VCPU_IOBITMAP_आकार / 2)));
}

स्थिर शून्य dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    प्रतिज्ञा(tcb->tcbArch.tcbVCPU == vcpu);
    प्रतिज्ञा(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_सक्रिय = false;
    tcb->tcbArch.tcbVCPU = रिक्त;
    vcpu->vcpuTCB = रिक्त;
}

शून्य vcpu_finalise(vcpu_t *vcpu)
{
    यदि (ARCH_ग्रन्थिः_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->चरमः_cpu) == vcpu) {
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
        यदि (vcpu->चरमः_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->चरमः_cpu);
        } अन्यथा
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    यदि (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    यदि (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

स्थिर शून्य associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    यदि (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    यदि (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

स्थिर exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *बफर)
{
    पूर्णाङ्क i;
    प्रति (i = 0; i < n_vcpu_gp_पञ्जिकाकोशः; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, बफर);
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeVCPUWriteRegisters(cap_t cap, word_t दीर्घता, word_t *बफर)
{
#यदि_परिभाषितम् CONFIG_X86_64_VTX_64BIT_GUESTS
    यदि (दीर्घता < 15) {
#अन्यथा
    यदि (दीर्घता < 7) {
#यदि_अन्तः /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    प्रत्यागम invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), बफर);
}

#यदि_परिभाषितम् CONFIG_X86_64_VTX_64BIT_GUESTS
स्थिर exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *बफर, word_t क्षेत्रम्, word_t मानम्)
{
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    यदि (ARCH_ग्रन्थिः_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    विकल्प (क्षेत्रम्) {
    पक्ष IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = मानम्;
        भङ्ग;
    पक्ष IA32_तारकम्_MSR:
        vcpu->syscall_registers[VCPU_तारकम्] = मानम्;
        भङ्ग;
    पक्ष IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = मानम्;
        भङ्ग;
    पक्ष IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_आवरणम्] = मानम्;
        भङ्ग;
    }
    समुच्चयः_MR(thread, बफर, 0, मानम्);
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeVCPUWriteMSR(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    word_t क्षेत्रम्;
    word_t मानम्;

    यदि (दीर्घता < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    क्षेत्रम् = getSyscallArg(0, बफर);
    मानम् = getSyscallArg(1, बफर);
    विकल्प (क्षेत्रम्) {
    पक्ष IA32_LSTAR_MSR:
    पक्ष IA32_तारकम्_MSR:
    पक्ष IA32_CSTAR_MSR:
    पक्ष IA32_FMASK_MSR:
        भङ्ग;
    स्वभाव:
        userError("VCPU WriteMSR: Invalid field %lx.", (दीर्घ)क्षेत्रम्);
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    प्रत्यागम invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), बफर, क्षेत्रम्, मानम्);
}

स्थिर exception_t invokeReadMSR(vcpu_t *vcpu, word_t क्षेत्रम्, word_t *बफर)
{
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);

    word_t मानम् = 0;

    विकल्प (क्षेत्रम्) {
    पक्ष IA32_LSTAR_MSR:
        मानम् = vcpu->syscall_registers[VCPU_LSTAR];
        भङ्ग;
    पक्ष IA32_तारकम्_MSR:
        मानम् = vcpu->syscall_registers[VCPU_तारकम्];
        भङ्ग;
    पक्ष IA32_CSTAR_MSR:
        मानम् = vcpu->syscall_registers[VCPU_CSTAR];
        भङ्ग;
    पक्ष IA32_FMASK_MSR:
        मानम् = vcpu->syscall_registers[VCPU_SYSCALL_आवरणम्];
        भङ्ग;
    पक्ष IA32_VMX_MISC_MSR:
        मानम् = x86_rdmsr(क्षेत्रम्);
        भङ्ग;
    }

    समुच्चयः_MR(thread, बफर, 0, मानम्);
    समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                    se_L4_सन्देशः_Info_new(0, 0, 0, 1)));
    समुच्चयः_Thread_State(thread, ThreadState_Restart);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeVCPUReadMSR(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    यदि (दीर्घता < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    word_t क्षेत्रम् = getSyscallArg(0, बफर);
    विकल्प (क्षेत्रम्) {
    पक्ष IA32_LSTAR_MSR:
    पक्ष IA32_तारकम्_MSR:
    पक्ष IA32_CSTAR_MSR:
    पक्ष IA32_FMASK_MSR:
    पक्ष IA32_VMX_MISC_MSR:
        भङ्ग;
    स्वभाव:
        userError("VCPU ReadMSR: Invalid field %lx.", (दीर्घ)क्षेत्रम्);
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    प्रत्यागम invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), क्षेत्रम्, बफर);
}
#यदि_अन्तः  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

स्थिर exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t नीचः, uint16_t उच्चः)
{
    /* remove any existing io ports from this cap */
    clearVPIDIOPortMappings(cap_io_पत्तनम्_cap_get_cap_IOPort_VPID(cap),
                            cap_io_पत्तनम्_cap_get_cap_IOPort_First_पत्तनम्(cap),
                            cap_io_पत्तनम्_cap_get_cap_IOPort_चरमः_पत्तनम्(cap));
    /* update the assigned vpid. If the vcpu does not have a valid vpid then
     * this is fine as whilst the cap will not point to the vcpu, the vcpu
     * will have its port mask cleared when it gets assigned a vpid */
    cap = cap_io_पत्तनम्_cap_समुच्चयः_cap_IOPort_VPID(cap, vcpu->vpid);
    slot->cap = cap;
    समुच्चयः_IOPort_आवरणम्(vcpu->io, नीचः, उच्चः, false);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeEnableIOPort(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t नीचः, उच्चः;

    यदि (दीर्घता < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    यदि (cap_get_capType(ioCap) != cap_io_पत्तनम्_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    नीचः = getSyscallArg(0, बफर);
    उच्चः = getSyscallArg(1, बफर);

    यदि (नीचः < cap_io_पत्तनम्_cap_get_cap_IOPort_First_पत्तनम्(ioCap) || उच्चः > cap_io_पत्तनम्_cap_get_cap_IOPort_चरमः_पत्तनम्(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeEnableIOPort(vcpu, ioSlot, ioCap, नीचः, उच्चः);
}

स्थिर exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t नीचः, uint16_t उच्चः)
{
    समुच्चयः_IOPort_आवरणम्(vcpu->io, नीचः, उच्चः, true);
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeDisableIOPort(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    vcpu_t *vcpu;
    uint16_t नीचः, उच्चः;

    यदि (दीर्घता < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    नीचः = getSyscallArg(0, बफर);
    उच्चः = getSyscallArg(1, बफर);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    प्रत्यागम invokeDisableIOPort(vcpu, नीचः, उच्चः);
}

स्थिर exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *बफर, word_t क्षेत्रम्, word_t मानम्)
{
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);

    यदि (ARCH_ग्रन्थिः_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    विकल्प (क्षेत्रम्) {
    पक्ष VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = मानम्;
        भङ्ग;
    पक्ष VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = मानम्;
        भङ्ग;
    पक्ष VMX_CONTROL_CR0_आवरणम्:
        vcpu->cr0_आवरणम् = vcpu->cached_cr0_आवरणम् = मानम्;
        भङ्ग;
    पक्ष VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = मानम्;
        भङ्ग;
    }
    vmwrite(क्षेत्रम्, मानम्);

    यदि (call) {
        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
        अचिह्नित पूर्णाङ्क दीर्घता = समुच्चयः_MR(thread, बफर, 0, मानम्);
        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeWriteVMCS(cap_t cap, word_t दीर्घता, bool_t call, word_t *बफर)
{
    word_t क्षेत्रम्;
    word_t मानम्;

    यदि (दीर्घता < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    क्षेत्रम् = getSyscallArg(0, बफर);
    मानम् = getSyscallArg(1, बफर);
    विकल्प (क्षेत्रम्) {
    पक्ष VMX_GUEST_RIP:
    पक्ष VMX_GUEST_RSP:
    पक्ष VMX_GUEST_ES_SELECTOR:
    पक्ष VMX_GUEST_CS_SELECTOR:
    पक्ष VMX_GUEST_SS_SELECTOR:
    पक्ष VMX_GUEST_DS_SELECTOR:
    पक्ष VMX_GUEST_FS_SELECTOR:
    पक्ष VMX_GUEST_GS_SELECTOR:
    पक्ष VMX_GUEST_LDTR_SELECTOR:
    पक्ष VMX_GUEST_TR_SELECTOR:
    पक्ष VMX_GUEST_DEBUGCTRL:
    पक्ष VMX_GUEST_PAT:
    पक्ष VMX_GUEST_EFER:
    पक्ष VMX_GUEST_PERF_GLOBAL_CTRL:
    पक्ष VMX_GUEST_PDPTE0:
    पक्ष VMX_GUEST_PDPTE1:
    पक्ष VMX_GUEST_PDPTE2:
    पक्ष VMX_GUEST_PDPTE3:
    पक्ष VMX_GUEST_ES_सीमा:
    पक्ष VMX_GUEST_CS_सीमा:
    पक्ष VMX_GUEST_SS_सीमा:
    पक्ष VMX_GUEST_DS_सीमा:
    पक्ष VMX_GUEST_FS_सीमा:
    पक्ष VMX_GUEST_GS_सीमा:
    पक्ष VMX_GUEST_LDTR_सीमा:
    पक्ष VMX_GUEST_TR_सीमा:
    पक्ष VMX_GUEST_GDTR_सीमा:
    पक्ष VMX_GUEST_IDTR_सीमा:
    पक्ष VMX_GUEST_ES_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_CS_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_SS_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_DS_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_FS_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_GS_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_LDTR_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_TR_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_INTERRUPTABILITY:
    पक्ष VMX_GUEST_ACTIVITY:
    पक्ष VMX_GUEST_SMBASE:
    पक्ष VMX_GUEST_SYSENTER_CS:
    पक्ष VMX_GUEST_PREEMPTION_TIMER_मानम्:
    पक्ष VMX_GUEST_ES_आधारः:
    पक्ष VMX_GUEST_CS_आधारः:
    पक्ष VMX_GUEST_SS_आधारः:
    पक्ष VMX_GUEST_DS_आधारः:
    पक्ष VMX_GUEST_FS_आधारः:
    पक्ष VMX_GUEST_GS_आधारः:
    पक्ष VMX_GUEST_LDTR_आधारः:
    पक्ष VMX_GUEST_TR_आधारः:
    पक्ष VMX_GUEST_GDTR_आधारः:
    पक्ष VMX_GUEST_IDTR_आधारः:
    पक्ष VMX_GUEST_DR7:
    पक्ष VMX_GUEST_RFLAGS:
    पक्ष VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    पक्ष VMX_GUEST_SYSENTER_ESP:
    पक्ष VMX_GUEST_SYSENTER_EIP:
    पक्ष VMX_CONTROL_CR0_आवरणम्:
    पक्ष VMX_CONTROL_CR4_आवरणम्:
    पक्ष VMX_CONTROL_CR0_READ_SHADOW:
    पक्ष VMX_CONTROL_CR4_READ_SHADOW:
    पक्ष VMX_GUEST_CR3:
    पक्ष VMX_CONTROL_EXCEPTION_BITMAP:
    पक्ष VMX_CONTROL_प्रविष्टिः_INTERRUPTION_INFO:
    पक्ष VMX_CONTROL_प्रविष्टिः_EXCEPTION_ERROR_संहिता:
        भङ्ग;
    पक्ष VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        मानम् = apply_Fixed_द्वयङ्काः(मानम्, pin_control_उच्चः, pin_control_नीचः);
        भङ्ग;
    पक्ष VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        मानम् = apply_Fixed_द्वयङ्काः(मानम्, primary_control_उच्चः, primary_control_नीचः);
        भङ्ग;
    पक्ष VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        मानम् = apply_Fixed_द्वयङ्काः(मानम्, secondary_control_उच्चः, secondary_control_नीचः);
        भङ्ग;
    पक्ष VMX_CONTROL_प्रविष्टिः_CONTROLS:
        मानम् = apply_Fixed_द्वयङ्काः(मानम्, प्रविष्टिः_control_उच्चः, प्रविष्टिः_control_नीचः);
        भङ्ग;
    पक्ष VMX_CONTROL_EXIT_CONTROLS:
        मानम् = apply_Fixed_द्वयङ्काः(मानम्, exit_control_उच्चः, exit_control_नीचः);
        भङ्ग;
    पक्ष VMX_GUEST_CR0:
        मानम् = apply_Fixed_द्वयङ्काः(मानम्, cr0_उच्चः, cr0_नीचः);
        भङ्ग;
    पक्ष VMX_GUEST_CR4:
        मानम् = apply_Fixed_द्वयङ्काः(मानम्, cr4_उच्चः, cr4_नीचः);
        भङ्ग;
    स्वभाव:
        userError("VCPU WriteVMCS: Invalid field %lx.", (दीर्घ)क्षेत्रम्);
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    प्रत्यागम invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, बफर, क्षेत्रम्, मानम्);
}

स्थिर word_t readVMCSField(vcpu_t *vcpu, word_t क्षेत्रम्)
{
    विकल्प (क्षेत्रम्) {
    पक्ष VMX_CONTROL_EXCEPTION_BITMAP:
        प्रत्यागम vcpu->exception_bitmap;
    पक्ष VMX_GUEST_CR0:
        प्रत्यागम vcpu->cr0;
    पक्ष VMX_CONTROL_CR0_आवरणम्:
        प्रत्यागम vcpu->cr0_आवरणम्;
    पक्ष VMX_CONTROL_CR0_READ_SHADOW:
        प्रत्यागम vcpu->cr0_shadow;
    }
    यदि (ARCH_ग्रन्थिः_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    प्रत्यागम vmread(क्षेत्रम्);
}

स्थिर exception_t invokeReadVMCS(vcpu_t *vcpu, word_t क्षेत्रम्, bool_t call, word_t *बफर)
{
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    word_t मानम् = readVMCSField(vcpu, क्षेत्रम्);
    यदि (call) {
        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
        अचिह्नित पूर्णाङ्क दीर्घता = समुच्चयः_MR(thread, बफर, 0, मानम्);
        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeReadVMCS(cap_t cap, word_t दीर्घता, bool_t call, word_t *बफर)
{
    यदि (दीर्घता < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    word_t क्षेत्रम् = getSyscallArg(0, बफर);
    विकल्प (क्षेत्रम्) {
    पक्ष VMX_GUEST_RIP:
    पक्ष VMX_GUEST_RSP:
    पक्ष VMX_GUEST_ES_SELECTOR:
    पक्ष VMX_GUEST_CS_SELECTOR:
    पक्ष VMX_GUEST_SS_SELECTOR:
    पक्ष VMX_GUEST_DS_SELECTOR:
    पक्ष VMX_GUEST_FS_SELECTOR:
    पक्ष VMX_GUEST_GS_SELECTOR:
    पक्ष VMX_GUEST_LDTR_SELECTOR:
    पक्ष VMX_GUEST_TR_SELECTOR:
    पक्ष VMX_GUEST_DEBUGCTRL:
    पक्ष VMX_GUEST_PAT:
    पक्ष VMX_GUEST_EFER:
    पक्ष VMX_GUEST_PERF_GLOBAL_CTRL:
    पक्ष VMX_GUEST_PDPTE0:
    पक्ष VMX_GUEST_PDPTE1:
    पक्ष VMX_GUEST_PDPTE2:
    पक्ष VMX_GUEST_PDPTE3:
    पक्ष VMX_GUEST_ES_सीमा:
    पक्ष VMX_GUEST_CS_सीमा:
    पक्ष VMX_GUEST_SS_सीमा:
    पक्ष VMX_GUEST_DS_सीमा:
    पक्ष VMX_GUEST_FS_सीमा:
    पक्ष VMX_GUEST_GS_सीमा:
    पक्ष VMX_GUEST_LDTR_सीमा:
    पक्ष VMX_GUEST_TR_सीमा:
    पक्ष VMX_GUEST_GDTR_सीमा:
    पक्ष VMX_GUEST_IDTR_सीमा:
    पक्ष VMX_GUEST_ES_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_CS_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_SS_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_DS_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_FS_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_GS_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_LDTR_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_TR_ACCESS_RIGHTS:
    पक्ष VMX_GUEST_INTERRUPTABILITY:
    पक्ष VMX_GUEST_ACTIVITY:
    पक्ष VMX_GUEST_SMBASE:
    पक्ष VMX_GUEST_SYSENTER_CS:
    पक्ष VMX_GUEST_PREEMPTION_TIMER_मानम्:
    पक्ष VMX_GUEST_ES_आधारः:
    पक्ष VMX_GUEST_CS_आधारः:
    पक्ष VMX_GUEST_SS_आधारः:
    पक्ष VMX_GUEST_DS_आधारः:
    पक्ष VMX_GUEST_FS_आधारः:
    पक्ष VMX_GUEST_GS_आधारः:
    पक्ष VMX_GUEST_LDTR_आधारः:
    पक्ष VMX_GUEST_TR_आधारः:
    पक्ष VMX_GUEST_GDTR_आधारः:
    पक्ष VMX_GUEST_IDTR_आधारः:
    पक्ष VMX_GUEST_DR7:
    पक्ष VMX_GUEST_RFLAGS:
    पक्ष VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    पक्ष VMX_GUEST_SYSENTER_ESP:
    पक्ष VMX_GUEST_SYSENTER_EIP:
    पक्ष VMX_CONTROL_CR0_आवरणम्:
    पक्ष VMX_CONTROL_CR4_आवरणम्:
    पक्ष VMX_CONTROL_CR0_READ_SHADOW:
    पक्ष VMX_CONTROL_CR4_READ_SHADOW:
    पक्ष VMX_दत्त_INSTRUCTION_ERROR:
    पक्ष VMX_दत्त_EXIT_व्यवधानम्_INFO:
    पक्ष VMX_दत्त_EXIT_व्यवधानम्_ERROR:
    पक्ष VMX_दत्त_IDT_सदिशम्_INFO:
    पक्ष VMX_दत्त_IDT_सदिशम्_ERROR:
    पक्ष VMX_दत्त_EXIT_INSTRUCTION_दीर्घता:
    पक्ष VMX_दत्त_EXIT_INSTRUCTION_INFO:
    पक्ष VMX_दत्त_GUEST_PHYSICAL:
    पक्ष VMX_दत्त_IO_RCX:
    पक्ष VMX_दत्त_IO_RSI:
    पक्ष VMX_दत्त_IO_RDI:
    पक्ष VMX_दत्त_IO_RIP:
    पक्ष VMX_दत्त_GUEST_रैखिकम्_स्थानाङ्कः:
    पक्ष VMX_CONTROL_प्रविष्टिः_INTERRUPTION_INFO:
    पक्ष VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    पक्ष VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    पक्ष VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    पक्ष VMX_CONTROL_EXCEPTION_BITMAP:
    पक्ष VMX_CONTROL_प्रविष्टिः_CONTROLS:
    पक्ष VMX_CONTROL_EXIT_CONTROLS:
    पक्ष VMX_GUEST_CR0:
    पक्ष VMX_GUEST_CR3:
    पक्ष VMX_GUEST_CR4:
        भङ्ग;
    स्वभाव:
        userError("VCPU ReadVMCS: Invalid field %lx.", (दीर्घ)क्षेत्रम्);
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    प्रत्यागम invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), क्षेत्रम्, call, बफर);
}

स्थिर exception_t invoke_समुच्चयः_TCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decode_समुच्चयः_TCB(cap_t cap, word_t दीर्घता, word_t *बफर)
{
    cap_t tcbCap;
    यदि (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    यदि (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    प्रत्यागम invoke_समुच्चयः_TCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

शून्य vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *बफर;
    यदि (ARCH_ग्रन्थिः_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    बफर = lookupIPCBuffer(false, ग्रन्थिः_STATE(ksCurThread));
    यदि (!बफर) {
        userError("No IPC buffer.");
        प्रत्यागम;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, बफर));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, apply_Fixed_द्वयङ्काः(getSyscallArg(1, बफर), primary_control_उच्चः,
                                                                   primary_control_नीचः));
    vmwrite(VMX_CONTROL_प्रविष्टिः_INTERRUPTION_INFO, getSyscallArg(2, बफर));
}

शून्य vcpu_sysvmenter_प्रत्युत्तरम्_गन्तव्यम्_user(tcb_t *tcb)
{
    word_t *बफर;
    vcpu_t *vcpu;

    बफर = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    प्रतिज्ञा(vcpu);

    यदि (ARCH_ग्रन्थिः_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    समुच्चयः_MR(tcb, बफर, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    समुच्चयः_MR(tcb, बफर, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    समुच्चयः_MR(tcb, बफर, SEL4_VMENTER_CALL_व्यवधानम्_INFO_MR,
          vmread(VMX_CONTROL_प्रविष्टिः_INTERRUPTION_INFO));
    समुच्चयः_पञ्जिकाकोशः(tcb, सन्देशः_Info_पञ्जिकाकोशः, 0);
}

exception_t decodeX86VCPUInvocation(
    word_t invLabel,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    विकल्प (invLabel) {
    पक्ष X86VCPUSetTCB:
        प्रत्यागम decode_समुच्चयः_TCB(cap, दीर्घता, बफर);
    पक्ष X86VCPUReadVMCS:
        प्रत्यागम decodeReadVMCS(cap, दीर्घता, call, बफर);
    पक्ष X86VCPUWriteVMCS:
        प्रत्यागम decodeWriteVMCS(cap, दीर्घता, call, बफर);
    पक्ष X86VCPUEnableIOPort:
        प्रत्यागम decodeEnableIOPort(cap, दीर्घता, बफर);
    पक्ष X86VCPUDisableIOPort:
        प्रत्यागम decodeDisableIOPort(cap, दीर्घता, बफर);
    पक्ष X86VCPUWriteRegisters:
        प्रत्यागम decodeVCPUWriteRegisters(cap, दीर्घता, बफर);
#यदि_परिभाषितम् CONFIG_X86_64_VTX_64BIT_GUESTS
    पक्ष X86VCPUWriteMSR:
        प्रत्यागम decodeVCPUWriteMSR(cap, दीर्घता, बफर);
    पक्ष X86VCPUReadMSR:
        प्रत्यागम decodeVCPUReadMSR(cap, दीर्घता, बफर);
#यदि_अन्तः /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    स्वभाव:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

स्थिर bool_t is_vtx_supported(शून्य)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    प्रत्यागम !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

स्थिर अन्तःस्थ शून्य clear_bit(word_t *bitmap, word_t bit)
{
    पूर्णाङ्क सूचकांक = bit / (परिमाण(word_t) * 8);
    पूर्णाङ्क विस्थापनम् = bit % (परिमाण(word_t) * 8);
    bitmap[सूचकांक] &= ~BIT(विस्थापनम्);
}

BOOT_संहिता bool_t vtx_init(शून्य)
{
    यदि (!is_vtx_supported()) {
        प्रारूप_लिख("vt-x: not supported\n");
        प्रत्यागम false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.शब्दाः[0] = x86_rdmsr_नीचः(IA32_VMX_BASIC_MSR);
    vmx_basic.शब्दाः[1] = x86_rdmsr_उच्चः(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.शब्दाः[0] = x86_rdmsr_नीचः(IA32_FEATURE_CONTROL_MSR);
    यदि (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        यदि (feature_control_msr_get_तालकम्(feature_control)) {
            प्रारूप_लिख("vt-x: feature locked\n");
            प्रत्यागम false;
        }
        feature_control = feature_control_msr_समुच्चयः_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_उच्चः(IA32_FEATURE_CONTROL_MSR), feature_control.शब्दाः[0]);
    }
    /* make sure the msr is locked */
    यदि (!feature_control_msr_get_तालकम्(feature_control)) {
        feature_control = feature_control_msr_समुच्चयः_तालकम्(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_उच्चः(IA32_FEATURE_CONTROL_MSR), feature_control.शब्दाः[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    यदि (CURRENT_CPU_सूचकांक() == 0) {
        यदि (!init_vtx_fixed_मानानि(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            प्रारूप_लिख("vt-x: lack of required features\n");
            प्रत्यागम false;
        }
    }
    यदि (!check_vtx_fixed_मानानि(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        प्रारूप_लिख("vt-x: cores have inconsistent features\n");
        प्रत्यागम false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    संरचना vmxon_region *vmxon_kptr = &ARCH_ग्रन्थिः_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    यदि (!vtx_check_fixed_मानानि(read_cr0(), read_cr4())) {
        प्रत्यागम false;
    }
    यदि (vmxon(kpptr_गन्तव्यम्_paddr(vmxon_kptr))) {
        प्रारूप_लिख("vt-x: vmxon failure\n");
        प्रत्यागम false;
    }
    स्मृतिं_पूरय(&msr_bitmap_region, ~0, परिमाण(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.नीचः_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.नीचः_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.नीचः_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.नीचः_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.नीचः_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.नीचः_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#यदि_परिभाषितम् CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.उच्चः_msr_read.bitmap, MSR_BITMAP_आवरणम्(IA32_FS_आधारः_MSR));
    clear_bit(msr_bitmap_region.उच्चः_msr_read.bitmap, MSR_BITMAP_आवरणम्(IA32_GS_आधारः_MSR));
    clear_bit(msr_bitmap_region.उच्चः_msr_read.bitmap, MSR_BITMAP_आवरणम्(IA32_KERNEL_GS_आधारः_MSR));
    clear_bit(msr_bitmap_region.उच्चः_msr_write.bitmap, MSR_BITMAP_आवरणम्(IA32_FS_आधारः_MSR));
    clear_bit(msr_bitmap_region.उच्चः_msr_write.bitmap, MSR_BITMAP_आवरणम्(IA32_GS_आधारः_MSR));
    clear_bit(msr_bitmap_region.उच्चः_msr_write.bitmap, MSR_BITMAP_आवरणम्(IA32_KERNEL_GS_आधारः_MSR));
#यदि_अन्तः
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.शब्दाः[0] = x86_rdmsr_नीचः(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.शब्दाः[1] = x86_rdmsr_उच्चः(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    यदि (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        प्रारूप_लिख("vt-x: Expected wb attribute for EPT paging structure\n");
        प्रत्यागम false;
    }
    यदि (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        प्रारूप_लिख("vt-x: Expected supported for 2m pages\n");
        प्रत्यागम false;
    }

    प्रत्यागम true;
}

स्थिर शून्य समुच्चयः_MRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *बफर;
    पूर्णाङ्क i;

    बफर = lookupIPCBuffer(true, ग्रन्थिः_STATE(ksCurThread));

    समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_CALL_व्यवधानम्_INFO_MR,
          vmread(VMX_CONTROL_प्रविष्टिः_INTERRUPTION_INFO));
    समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_FAULT_REASON_MR, reason);
    समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_दत्त_EXIT_INSTRUCTION_दीर्घता));
    समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_दत्त_GUEST_PHYSICAL));
    समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    प्रति (i = 0; i < n_vcpu_gp_पञ्जिकाकोशः; i++) {
        समुच्चयः_MR(ग्रन्थिः_STATE(ksCurThread), बफर, SEL4_VMENTER_FAULT_EAX + i,
              ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

स्थिर शून्य handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    समुच्चयः_पञ्जिकाकोशः(ग्रन्थिः_STATE(ksCurThread), सन्देशः_Info_पञ्जिकाकोशः, SEL4_VMENTER_परिणाम_FAULT);

    समुच्चयः_MRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

स्थिर अन्तःस्थ शून्य finishVmexitSaving(शून्य)
{
    vcpu_t *vcpu = ARCH_ग्रन्थिः_STATE(x86KSCurrentVCPU);
    प्रतिज्ञा(vcpu == ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(शून्य)
{
    uint32_t व्यवधानम्;
    /* qualification is host width, reason is defined as being 32 bit */
    word_t qualification;
    uint32_t reason;
    finishVmexitSaving();
#यदि_परिभाषितम् CONFIG_X86_64_VTX_64BIT_GUESTS
    vcpu_restore_host_msrs();
#यदि_अन्तः
    /* the basic exit reason is the bottom 16 bits of the exit reason field */
    reason = vmread(VMX_दत्त_EXIT_REASON) & आवरणम्(16);
    यदि (reason == EXTERNAL_व्यवधानम्) {
        ग्रन्थिः_तालकम्_IRQ;
        यदि (vmx_feature_ack_on_exit) {
            व्यवधानम् = vmread(VMX_दत्त_EXIT_व्यवधानम्_INFO) & 0xff;
            ARCH_ग्रन्थिः_STATE(x86_KScur_व्यवधानम्) = व्यवधानम्;
            handle_व्यवधानम्_प्रविष्टिः();
        } अन्यथा {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(ग्रन्थिः_STATE(ksCurThread));
#यदि_अन्तः
        प्रत्यागम EXCEPTION_NONE;
    }

    ग्रन्थिः_तालकम्_SYS;

    विकल्प (reason) {
    पक्ष EXCEPTION_OR_NMI:
    पक्ष MOV_DR:
    पक्ष TASK_SWITCH:
    पक्ष CONTROL_पञ्जिकाकोशः:
    पक्ष IO:
    पक्ष MWAIT:
    पक्ष SIPI:
    पक्ष INVLPG:
    पक्ष INVEPT:
    पक्ष INVVPID:
    पक्ष VMCLEAR:
    पक्ष VMPTRLD:
    पक्ष VMPTRST:
    पक्ष VMREAD:
    पक्ष VMWRITE:
    पक्ष VMXON:
    पक्ष EPT_VIOLATION:
    पक्ष GDTR_OR_IDTR:
    पक्ष LDTR_OR_TR:
    पक्ष TPR_अधः_देहली:
    पक्ष APIC_ACCESS:
        qualification = vmread(VMX_दत्त_EXIT_QUALIFICATION);
        भङ्ग;
    स्वभाव:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    प्रत्यागम EXCEPTION_NONE;
}

exception_t handle_Vm_प्रविष्टिः_Fail(शून्य)
{
    handleVmxFault(-1, -1);

    प्रत्यागम EXCEPTION_NONE;
}

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
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
    यदि (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_सक्रिय) {

        word_t badge = notification_ptr_get_ntfn_सन्देशः_Identifier(ntfnPtr);
        notification_ptr_समुच्चयः_state(ntfnPtr, NtfnState_Idle);
        समुच्चयः_Thread_State(tcb, ThreadState_Running);
        समुच्चयः_पञ्जिकाकोशः(tcb, badge_पञ्जिकाकोशः, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        यदि (tcb != ग्रन्थिः_STATE(ksCurThread)) {
            possible_Switch_गन्तव्यम्(tcb);
        }
    }
}
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

स्थिर शून्य invvpid_context(uint16_t vpid)
{
    संरचना {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t स्थानाङ्कः;
    } PACKED सङ्कार्यम् = {vpid, 0, 0};
    asm चञ्चल("invvpid %0, %1" :: "m"(सङ्कार्यम्), "r"((word_t)1) : "cc");
}

स्थिर शून्य समुच्चयः_EPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_मूलम्;
    यदि (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_मूलम् = kpptr_गन्तव्यम्_paddr(null_ept_space);
    } अन्यथा {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_आधारः_Ptr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        यदि (find_ret.स्थितिः != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_मूलम् = kpptr_गन्तव्यम्_paddr(null_ept_space);
        } अन्यथा {
            ept_मूलम् = pptr_गन्तव्यम्_paddr(pml4);
        }
    }
    यदि (ept_मूलम् != vcpu->चरमः_ept_मूलम्) {
        vcpu->चरमः_ept_मूलम् = ept_मूलम्;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_मूलम्,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.शब्दाः[0]);
        प्रतिज्ञा(vcpu->vpid != VPID_INVALID);
        यदि (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

शून्य clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t चरमः)
{
    यदि (vpid == VPID_INVALID) {
        प्रत्यागम;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    यदि (vcpu == रिक्त) {
        प्रत्यागम;
    }
    प्रतिज्ञा(vcpu->vpid == vpid);
    समुच्चयः_IOPort_आवरणम्(vcpu->io, first, चरमः, true);
}

स्थिर अन्तःस्थ vpid_t अग्र_VPID(vpid_t vpid)
{
    यदि (vpid == VPID_चरमः) {
        प्रत्यागम VPID_FIRST;
    } अन्यथा {
        प्रत्यागम vpid + 1;
    }
}

स्थिर शून्य invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    स्मृतिं_पूरय(vcpu->io, ~0, परिमाण(vcpu->io));
    /* invalidate the VPID context */
    यदि (vmx_feature_vpid) {
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
    कुरु {
        यदि (x86KSVPIDTable[vpid] == रिक्त) {
            प्रत्यागम vpid;
        }
        vpid = अग्र_VPID(vpid);
    } यावत् (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = अग्र_VPID(x86KSNextVPID);
    प्रत्यागम vpid;
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
शून्य vcpu_fpu_गन्तव्यम्_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    यदि (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        यदि (!vcpu->fpu_सक्रिय) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_सक्रिय = true;
            आरोपय_Fpu_State(tcb);
        }
    } अन्यथा {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_सक्रिय = true;
        switchLocalFpuOwner(tcb);
    }
}

शून्य vcpu_fpu_गन्तव्यम्_host(tcb_t *tcb, vcpu_t *vcpu)
{
    यदि (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } अन्यथा यदि (nativeThreadUsingFPU(tcb) && vcpu->fpu_सक्रिय) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_सक्रिय = false;
        आरोपय_Fpu_State(tcb);
    } अन्यथा यदि (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

शून्य restoreVMCS(शून्य)
{
    tcb_t *cur_thread = ग्रन्थिः_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    यदि (ARCH_ग्रन्थिः_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#यदि_अपरिभाषितम् CONFIG_KERNEL_SKIM_गवाक्षः
    यदि (getCurrentCR3().शब्दाः[0] != expected_vmcs->चरमः_host_cr3) {
        expected_vmcs->चरमः_host_cr3 = getCurrentCR3().शब्दाः[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().शब्दाः[0]);
    }
#यदि_अन्तः
    यदि (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        यदि (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    समुच्चयः_EPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

शून्य invept(ept_pml4e_t *ept_pml4)
{
    यदि (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        संरचना {
            uint64_t parts[2];
        } स्थानाङ्कः;
        word_t type;
        यदि (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } अन्यथा यदि (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } अन्यथा {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            प्रत्यागम;
        }

        स्थानाङ्कः.parts[0] = pptr_गन्तव्यम्_paddr((शून्य *)ept_pml4);
        स्थानाङ्कः.parts[1] = 0;
        asm चञ्चल(
            "invept %0, %1"
            :
            : "m"(स्थानाङ्कः),  "r"(type)
            : "memory"
        );
    }
}

#यदि_अन्तः
