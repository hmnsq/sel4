/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX

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

#ವ್ಯಾಖ್ಯಾನಿಸು VMX_EXIT_QUAL_TYPE_MOV_CR 0
#ವ್ಯಾಖ್ಯಾನಿಸು VMX_EXIT_QUAL_TYPE_CLTS 2
#ವ್ಯಾಖ್ಯಾನಿಸು VMX_EXIT_QUAL_TYPE_LMSW 3

ಸ್ಥಿರಾಂಕ vcpu_gp_ರಿಜಿಸ್ಟರ್_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#ವ್ಯಾಖ್ಯಾನಿಸು MSR_BITMAP_ಮುಖವಾಡ(x) ((x) & 0x1fff)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ msr_bitmap {
    ಶಬ್ದ_t bitmap[0x2000 / ಗಾತ್ರ(ಶಬ್ದ_t) / 8];
} msr_bitmap_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ msr_bitmaps {
    msr_bitmap_t ಕೆಳಗಿನ_msr_read;
    msr_bitmap_t ಮೇಲಿನ_msr_read;
    msr_bitmap_t ಕೆಳಗಿನ_msr_write;
    msr_bitmap_t ಮೇಲಿನ_msr_write;
} msr_bitmaps_t;

ಸ್ಥಿರ msr_bitmaps_t msr_bitmap_region ALIGN(ಬಿಟ್(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು));

ಸ್ಥಿರ ಅಕ್ಷರ null_ept_space[ಬಿಟ್(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು));

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
ಸ್ಥಿರ uint32_t ನಮೂದು_control_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t ನಮೂದು_control_ಕೆಳಗಿನ;
ಸ್ಥಿರ uint32_t exit_control_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t exit_control_ಕೆಳಗಿನ;
ಸ್ಥಿರ uint32_t cr0_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t cr0_ಕೆಳಗಿನ;
ಸ್ಥಿರ uint32_t cr4_ಮೇಲಿನ;
ಸ್ಥಿರ uint32_t cr4_ಕೆಳಗಿನ;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
ಸ್ಥಿರ bool_t vmx_feature_vpid;
ಸ್ಥಿರ bool_t vmx_feature_ತುಂಬು_perf_global_ctrl;
ಸ್ಥಿರ bool_t vmx_feature_ack_on_exit;

ಸ್ಥಿರ vcpu_t *x86KSVPIDTable[VPID_ಕೊನೆಯ + 1];
ಸ್ಥಿರ vpid_t x86KSNextVPID = VPID_ಮೊದಲ;

ಸ್ಥಿರ ಶೂನ್ಯ releaseVPID(vpid_t vpid);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm ಅಸ್ಥಿರ(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    ಹಿಂತಿರುಗಿ !!error;
}

ಸ್ಥಿರ ಶೂನ್ಯ vmclear(ಶೂನ್ಯ *vmcs_ptr)
{
    uint64_t physical_ವಿಳಾಸ;
    physical_ವಿಳಾಸ = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)vmcs_ptr);
    asm ಅಸ್ಥಿರ(
        "vmclear %0"
        :
        : "m"(physical_ವಿಳಾಸ)
        : "cc"
    );
}

ಶೂನ್ಯ clearCurrentVCPU(ಶೂನ್ಯ)
{
    vcpu_t *vcpu = ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU);
    ಆದರೆ (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) = ಖಾಲಿ;
        ಆದರೆ (vcpu->fpu_ಸಕ್ರಿಯ && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_ಸಕ್ರಿಯ = false;
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ vmptrld(ಶೂನ್ಯ *vmcs_ptr)
{
    uint64_t physical_ವಿಳಾಸ;
    uint8_t error;
    physical_ವಿಳಾಸ = pptr_ಗಮ್ಯ_paddr(vmcs_ptr);
    asm ಅಸ್ಥಿರ(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_ವಿಳಾಸ)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    ಪ್ರತಿಜ್ಞಾ(!error);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
ಶೂನ್ಯ vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_ನಕ್ಷತ್ರ_MSR, vcpu->syscall_registers[VCPU_ನಕ್ಷತ್ರ]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_ಮುಖವಾಡ]);
}

ಶೂನ್ಯ vcpu_restore_host_msrs(ಶೂನ್ಯ)
{
    init_syscall_msrs();
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ switchVCPU(vcpu_t *vcpu)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಆದರೆ (vcpu->ಕೊನೆಯ_cpu != getCurrentCPUIndex() && ARCH_ಗಂಟು_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->ಕೊನೆಯ_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->ಕೊನೆಯ_cpu);
    }
#ಆದರೆ_ಅಂತ್ಯ
    clearCurrentVCPU();
    vmptrld(vcpu);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಆದರೆ (vcpu->ಕೊನೆಯ_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_ತಳ, (ಶಬ್ದ_t)&x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KStss);
        vmwrite(VMX_HOST_GDTR_ತಳ, (ಶಬ್ದ_t)x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_ತಳ, (ಶಬ್ದ_t)x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(ಶಬ್ದ_t)((ಅಕ್ಷರ *)&x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KStss.tss.ಪದಗಳು[0] +
                                                          4));
    }
    vcpu->ಕೊನೆಯ_cpu = getCurrentCPUIndex();
#ಆದರೆ_ಅಂತ್ಯ
    ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) = vcpu;
}

ಸ್ಥಿರ ಶೂನ್ಯ print_ಬಿಟ್ಟುಗಳು(ಶಬ್ದ_t ಬಿಟ್ಟುಗಳು)
{
    bool_t ಮೊದಲ = true;
    ಆಗ (ಬಿಟ್ಟುಗಳು) {
        ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = se_L4_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1 - clzl(ಬಿಟ್ಟುಗಳು);
        ಆದರೆ (ಮೊದಲ) {
            ಬರೆ("%d", ಸೂಚ್ಯಂಕ);
            ಮೊದಲ = false;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ(",%d", ಸೂಚ್ಯಂಕ);
        }
        ಬಿಟ್ಟುಗಳು &= ~ಬಿಟ್(ಸೂಚ್ಯಂಕ);
    }
}

ಸ್ಥಿರ bool_t check_fixed_ಮೌಲ್ಯ(ಶಬ್ದ_t val, ಶಬ್ದ_t ಕೆಳಗಿನ, ಶಬ್ದ_t ಮೇಲಿನ)
{
    ಶಬ್ದ_t not_ಮೇಲಿನ;
    ಶಬ್ದ_t not_ಕೆಳಗಿನ;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_ಮೇಲಿನ = ಮೇಲಿನ & ~(ಮೇಲಿನ & val);
    ಆದರೆ (not_ಮೇಲಿನ != 0) {
        ಬರೆ("Failed to set bits: ");
        print_ಬಿಟ್ಟುಗಳು(not_ಮೇಲಿನ);
        ಹಿಂತಿರುಗಿ false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_ಕೆಳಗಿನ = ~ಕೆಳಗಿನ & ~(~ಕೆಳಗಿನ & ~val);
    ಆದರೆ (not_ಕೆಳಗಿನ != 0) {
        ಬರೆ("Incorrectly cleared bits: ");
        print_ಬಿಟ್ಟುಗಳು(not_ಕೆಳಗಿನ);
        ಹಿಂತಿರುಗಿ false;
    }
    ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ bool_t vtx_check_fixed_ಮೌಲ್ಯಗಳು(ಶಬ್ದ_t cr0, ಶಬ್ದ_t cr4)
{
    ಆದರೆ (!check_fixed_ಮೌಲ್ಯ(cr0, cr0_ಕೆಳಗಿನ, cr0_ಮೇಲಿನ)) {
        ಬರೆ(" of CR0\n");
        ಹಿಂತಿರುಗಿ false;
    }
    ಆದರೆ (!check_fixed_ಮೌಲ್ಯ(cr4, cr4_ಕೆಳಗಿನ, cr4_ಮೇಲಿನ)) {
        ಬರೆ(" of CR4\n");
        ಹಿಂತಿರುಗಿ false;
    }
    ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ bool_t BOOT_ಸಂಕೇತ init_vtx_fixed_ಮೌಲ್ಯಗಳು(bool_t useTrueMsrs)
{
    uint32_t pin_control_ಮುಖವಾಡ =
        ಬಿಟ್(0) |    //Extern interrupt exiting
        ಬಿಟ್(3) |    //NMI exiting
        ಬಿಟ್(5);     //virtual NMIs
    uint32_t primary_control_ಮುಖವಾಡ =
        ಬಿಟ್(25) |   //Use I/O bitmaps
        ಬಿಟ್(28) |   //Use MSR bitmaps
        ಬಿಟ್(31);    //Activate secondary controls
    uint32_t secondary_control_ಮುಖವಾಡ =
        ಬಿಟ್(1);     //Enable EPT
    uint32_t exit_control_ಮುಖವಾಡ =
        ಬಿಟ್(2)  |   //Save debug controls
        ಬಿಟ್(18) |   //Save guest IA32_PAT on exit
        ಬಿಟ್(19) |   //Load host IA32_PAT
        ಬಿಟ್(20) |   //Save guest IA32_EFER on exit
        ಬಿಟ್(21);    //Load host IA32_EFER
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_X86_64
    exit_control_ಮುಖವಾಡ |= ಬಿಟ್(9); //Host address-space size
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARCH_X86_64 */
    /* Read out the fixed high and low bits from the MSRs */
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t ನಮೂದು_ctls;
    ಆದರೆ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        ನಮೂದು_ctls = IA32_VMX_TRUE_ನಮೂದು_CTLS_MSR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        ನಮೂದು_ctls = IA32_VMX_ನಮೂದು_CTLS_MSR;
    }
    pin_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(pinbased_ctls);
    pin_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(pinbased_ctls);
    primary_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(procbased_ctls);
    primary_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(procbased_ctls);
    secondary_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(exit_ctls);
    exit_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(exit_ctls);
    ನಮೂದು_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(ನಮೂದು_ctls);
    ನಮೂದು_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(ನಮೂದು_ctls);

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
    ಆದರೆ (secondary_control_ಕೆಳಗಿನ & ಬಿಟ್(7)) {
        cr0_ಮೇಲಿನ &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    ಆದರೆ (!(secondary_control_ಕೆಳಗಿನ & ಬಿಟ್(5))) {
        vmx_feature_vpid = 0;
        ಬರೆ("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } ಇಲ್ಲದಿದ್ದರೆ {
        vmx_feature_vpid = 1;
        secondary_control_ಮುಖವಾಡ |= ಬಿಟ್(5);
    }

    /* Check for load perf global control */
    ಆದರೆ (!(exit_control_ಕೆಳಗಿನ & ಬಿಟ್(12))) {
        vmx_feature_ತುಂಬು_perf_global_ctrl = 0;
        ಬರೆ("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } ಇಲ್ಲದಿದ್ದರೆ {
        vmx_feature_ತುಂಬು_perf_global_ctrl = 1;
        exit_control_ಮುಖವಾಡ |= ಬಿಟ್(12);
    }

    /* Check for external interrupt exiting */
    ಆದರೆ (!(exit_control_ಕೆಳಗಿನ & ಬಿಟ್(15))) {
        vmx_feature_ack_on_exit = 0;
        ಬರೆ("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } ಇಲ್ಲದಿದ್ದರೆ {
        vmx_feature_ack_on_exit = 1;
        exit_control_ಮುಖವಾಡ |= ಬಿಟ್(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_ಕೆಳಗಿನ) & pin_control_ಮುಖವಾಡ;
    ಆದರೆ (missing) {
        ಬರೆ("vt-x: Unsupported pin control features %lx\n", (ದೀರ್ಘ)missing);
        ಹಿಂತಿರುಗಿ false;
    }
    missing = (~primary_control_ಕೆಳಗಿನ) & primary_control_ಮುಖವಾಡ;
    ಆದರೆ (missing) {
        ಬರೆ("vt-x: Unsupported primary control features %lx\n", (ದೀರ್ಘ)missing);
        ಹಿಂತಿರುಗಿ false;
    }
    missing = (~secondary_control_ಕೆಳಗಿನ) & secondary_control_ಮುಖವಾಡ;
    ಆದರೆ (missing) {
        ಬರೆ("vt-x: Unsupported secondary control features %lx\n", (ದೀರ್ಘ)missing);
        ಹಿಂತಿರುಗಿ false;
    }
    missing = (~exit_control_ಕೆಳಗಿನ) & exit_control_ಮುಖವಾಡ;
    ಆದರೆ (missing) {
        ಬರೆ("vt-x: Unsupported exit control features %lx\n", (ದೀರ್ಘ)missing);
        ಹಿಂತಿರುಗಿ false;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
    missing = (~ನಮೂದು_control_ಕೆಳಗಿನ) & ಬಿಟ್(9);
    ಆದರೆ (missing) {
        ಬರೆ("vt-x: CPU does not support 64-bit guests\n");
        ಹಿಂತಿರುಗಿ false;
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_ಮೇಲಿನ |= pin_control_ಮುಖವಾಡ;
    primary_control_ಮೇಲಿನ |= primary_control_ಮುಖವಾಡ;
    secondary_control_ಮೇಲಿನ |= secondary_control_ಮುಖವಾಡ;
    exit_control_ಮೇಲಿನ |= exit_control_ಮುಖವಾಡ;

    ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ bool_t BOOT_ಸಂಕೇತ check_vtx_fixed_ಮೌಲ್ಯಗಳು(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t ನಮೂದು_ctls;
    ಆದರೆ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        ನಮೂದು_ctls = IA32_VMX_TRUE_ನಮೂದು_CTLS_MSR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        ನಮೂದು_ctls = IA32_VMX_ನಮೂದು_CTLS_MSR;
    }
    uint32_t local_pin_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(pinbased_ctls);
    uint32_t local_pin_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(pinbased_ctls);
    uint32_t local_primary_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(procbased_ctls);
    uint32_t local_primary_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(procbased_ctls);
    uint32_t local_secondary_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(exit_ctls);
    uint32_t local_exit_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(exit_ctls);
    uint32_t local_ನಮೂದು_control_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(ನಮೂದು_ctls);
    uint32_t local_ನಮೂದು_control_ಕೆಳಗಿನ = x86_rdmsr_ಮೇಲಿನ(ನಮೂದು_ctls);

    uint32_t local_cr0_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_ಕೆಳಗಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_ಮೇಲಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_ಕೆಳಗಿನ = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    ಆದರೆ (local_secondary_control_ಕೆಳಗಿನ & ಬಿಟ್(7)) {
        local_cr0_ಮೇಲಿನ &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    ಹಿಂತಿರುಗಿ
        (local_pin_control_ಮೇಲಿನ & pin_control_ಮೇಲಿನ) == local_pin_control_ಮೇಲಿನ &&
        (~local_pin_control_ಕೆಳಗಿನ & pin_control_ಮೇಲಿನ) == 0 &&
        (local_primary_control_ಮೇಲಿನ & primary_control_ಮೇಲಿನ) == local_primary_control_ಮೇಲಿನ &&
        (~local_primary_control_ಕೆಳಗಿನ & primary_control_ಮೇಲಿನ) == 0 &&
        (local_secondary_control_ಮೇಲಿನ & secondary_control_ಮೇಲಿನ) == local_secondary_control_ಮೇಲಿನ &&
        (~local_secondary_control_ಕೆಳಗಿನ & secondary_control_ಮೇಲಿನ) == 0 &&
        (local_exit_control_ಮೇಲಿನ & exit_control_ಮೇಲಿನ) == local_exit_control_ಮೇಲಿನ &&
        (~local_exit_control_ಕೆಳಗಿನ & exit_control_ಮೇಲಿನ) == 0 &&
        (local_ನಮೂದು_control_ಮೇಲಿನ & ನಮೂದು_control_ಮೇಲಿನ) == local_ನಮೂದು_control_ಮೇಲಿನ &&
        (~local_ನಮೂದು_control_ಕೆಳಗಿನ & ನಮೂದು_control_ಮೇಲಿನ) == 0 &&
        local_cr0_ಮೇಲಿನ == cr0_ಮೇಲಿನ &&
        local_cr0_ಕೆಳಗಿನ == cr0_ಕೆಳಗಿನ &&
        local_cr4_ಮೇಲಿನ == cr4_ಮೇಲಿನ &&
        local_cr4_ಕೆಳಗಿನ == cr4_ಕೆಳಗಿನ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t apply_Fixed_ಬಿಟ್ಟುಗಳು(uint32_t original, uint32_t ಮೇಲಿನ, uint32_t ಕೆಳಗಿನ)
{
    original |= ಮೇಲಿನ;
    original &= ಕೆಳಗಿನ;
    ಹಿಂತಿರುಗಿ original;
}

ಶೂನ್ಯ vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = ಖಾಲಿ;
    vcpu->launched = false;
    vcpu->fpu_ಸಕ್ರಿಯ = false;
    vcpu->fpuState = x86KSnullFpuState;

    ಸ್ಮರಣೆ_ನಕಲು(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_ಮೇಲಿನ & cr0_ಕೆಳಗಿನ;
    vcpu->cr0_shadow = 0;
    vcpu->cr0_ಮುಖವಾಡ = 0;
    vcpu->exception_bitmap = 0;
    vcpu->vpid = VPID_INVALID;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    vcpu->ಕೊನೆಯ_cpu = getCurrentCPUIndex();
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    switchVCPU(vcpu);

    vmwrite(VMX_HOST_PAT, x86_rdmsr(IA32_PAT_MSR));
    vmwrite(VMX_HOST_EFER, x86_rdmsr(IA32_EFER_MSR));
    // By default we will disable performance counters when we come out
    // of a VM. When performance counters are supported this host state
    // needs to be updated on VM entry
    ಆದರೆ (vmx_feature_ತುಂಬು_perf_global_ctrl) {
        vmwrite(VMX_HOST_PERF_GLOBAL_CTRL, 0);
    }
    vmwrite(VMX_HOST_CR0, read_cr0());
    vmwrite(VMX_HOST_CR4, read_cr4());
    vmwrite(VMX_HOST_FS_ತಳ, 0);
    vmwrite(VMX_HOST_GS_ತಳ, 0);
    vmwrite(VMX_HOST_TR_ತಳ, (ಶಬ್ದ_t)&x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KStss);
    vmwrite(VMX_HOST_GDTR_ತಳ, (ಶಬ್ದ_t)x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KSgdt);
    vmwrite(VMX_HOST_IDTR_ತಳ, (ಶಬ್ದ_t)x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KSidt);
    vmwrite(VMX_HOST_SYSENTER_CS, (ಶಬ್ದ_t)SEL_CS_0);
    vmwrite(VMX_HOST_SYSENTER_EIP, (ಶಬ್ದ_t)&handle_syscall);
    ಆದರೆ (!config_ಗಣ(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(ಶಬ್ದ_t)((ಅಕ್ಷರ *)&x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KStss.tss.ಪದಗಳು[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (ಶಬ್ದ_t)&vcpu->gp_registers[n_vcpu_gp_ರಿಜಿಸ್ಟರ್]);
    vmwrite(VMX_HOST_RIP, (ಶಬ್ದ_t)&handle_vmexit);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_ಗಮ್ಯ_paddr(x64KSKernelPML4), 0).ಪದಗಳು[0]);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_SKIM_WINDOW */

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

    uint64_t ನಮೂದು_control = ನಮೂದು_control_ಮೇಲಿನ & ನಮೂದು_control_ಕೆಳಗಿನ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    ನಮೂದು_control |= ಬಿಟ್(9);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ನಮೂದು_CONTROLS, ನಮೂದು_control);

    vmwrite(VMX_CONTROL_MSR_ವಿಳಾಸ, (ಶಬ್ದ_t)kpptr_ಗಮ್ಯ_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_ಮೇಲಿನ & cr4_ಕೆಳಗಿನ);

    vmwrite(VMX_GUEST_VMCS_LINK_ಸೂಚಕ, ~(ಶಬ್ದ_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_ಸೂಚಕ_ಮೇಲಿನ, ~(ಶಬ್ದ_t)0);

    memset(vcpu->io, ~(ಶಬ್ದ_t)0, VCPU_IOBITMAP_ಗಾತ್ರ);
    vmwrite(VMX_CONTROL_IOA_ವಿಳಾಸ, pptr_ಗಮ್ಯ_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ವಿಳಾಸ, pptr_ಗಮ್ಯ_paddr((ಅಕ್ಷರ *)vcpu->io + (VCPU_IOBITMAP_ಗಾತ್ರ / 2)));
}

ಸ್ಥಿರ ಶೂನ್ಯ dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ಪ್ರತಿಜ್ಞಾ(tcb->tcbArch.tcbVCPU == vcpu);
    ಪ್ರತಿಜ್ಞಾ(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_ಸಕ್ರಿಯ = false;
    tcb->tcbArch.tcbVCPU = ಖಾಲಿ;
    vcpu->vcpuTCB = ಖಾಲಿ;
}

ಶೂನ್ಯ vcpu_finalise(vcpu_t *vcpu)
{
    ಆದರೆ (ARCH_ಗಂಟು_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->ಕೊನೆಯ_cpu) == vcpu) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
        ಆದರೆ (vcpu->ಕೊನೆಯ_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->ಕೊನೆಯ_cpu);
        } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    ಆದರೆ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    ಆದರೆ (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ಆದರೆ (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    ಆದರೆ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

ಸ್ಥಿರ exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, ಶಬ್ದ_t *ಬಫರ್)
{
    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ (i = 0; i < n_vcpu_gp_ರಿಜಿಸ್ಟರ್; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, ಬಫರ್);
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeVCPUWriteRegisters(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
    ಆದರೆ (ಉದ್ದಳತೆ < 15) {
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (ಉದ್ದಳತೆ < 7) {
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಹಿಂತಿರುಗಿ invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ಬಫರ್);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
ಸ್ಥಿರ exception_t invokeWriteMSR(vcpu_t *vcpu, ಶಬ್ದ_t *ಬಫರ್, ಶಬ್ದ_t ಕ್ಷೇತ್ರ, ಶಬ್ದ_t ಮೌಲ್ಯ)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    ಆದರೆ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ಆಯ್ಕೆ (ಕ್ಷೇತ್ರ) {
    ಸ್ಥಿತಿ IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = ಮೌಲ್ಯ;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ IA32_ನಕ್ಷತ್ರ_MSR:
        vcpu->syscall_registers[VCPU_ನಕ್ಷತ್ರ] = ಮೌಲ್ಯ;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = ಮೌಲ್ಯ;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_ಮುಖವಾಡ] = ಮೌಲ್ಯ;
        ನಿಲ್ಲಿಸು;
    }
    ಗಣ_MR(thread, ಬಫರ್, 0, ಮೌಲ್ಯ);
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeVCPUWriteMSR(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t ಕ್ಷೇತ್ರ;
    ಶಬ್ದ_t ಮೌಲ್ಯ;

    ಆದರೆ (ಉದ್ದಳತೆ < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಕ್ಷೇತ್ರ = getSyscallArg(0, ಬಫರ್);
    ಮೌಲ್ಯ = getSyscallArg(1, ಬಫರ್);
    ಆಯ್ಕೆ (ಕ್ಷೇತ್ರ) {
    ಸ್ಥಿತಿ IA32_LSTAR_MSR:
    ಸ್ಥಿತಿ IA32_ನಕ್ಷತ್ರ_MSR:
    ಸ್ಥಿತಿ IA32_CSTAR_MSR:
    ಸ್ಥಿತಿ IA32_FMASK_MSR:
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("VCPU WriteMSR: Invalid field %lx.", (ದೀರ್ಘ)ಕ್ಷೇತ್ರ);
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಹಿಂತಿರುಗಿ invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ಬಫರ್, ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ exception_t invokeReadMSR(vcpu_t *vcpu, ಶಬ್ದ_t ಕ್ಷೇತ್ರ, ಶಬ್ದ_t *ಬಫರ್)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);

    ಶಬ್ದ_t ಮೌಲ್ಯ = 0;

    ಆಯ್ಕೆ (ಕ್ಷೇತ್ರ) {
    ಸ್ಥಿತಿ IA32_LSTAR_MSR:
        ಮೌಲ್ಯ = vcpu->syscall_registers[VCPU_LSTAR];
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ IA32_ನಕ್ಷತ್ರ_MSR:
        ಮೌಲ್ಯ = vcpu->syscall_registers[VCPU_ನಕ್ಷತ್ರ];
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ IA32_CSTAR_MSR:
        ಮೌಲ್ಯ = vcpu->syscall_registers[VCPU_CSTAR];
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ IA32_FMASK_MSR:
        ಮೌಲ್ಯ = vcpu->syscall_registers[VCPU_SYSCALL_ಮುಖವಾಡ];
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ IA32_VMX_MISC_MSR:
        ಮೌಲ್ಯ = x86_rdmsr(ಕ್ಷೇತ್ರ);
        ನಿಲ್ಲಿಸು;
    }

    ಗಣ_MR(thread, ಬಫರ್, 0, ಮೌಲ್ಯ);
    ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                    se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, 1)));
    ಗಣ_Thread_State(thread, ThreadState_Restart);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeVCPUReadMSR(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆದರೆ (ಉದ್ದಳತೆ < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಶಬ್ದ_t ಕ್ಷೇತ್ರ = getSyscallArg(0, ಬಫರ್);
    ಆಯ್ಕೆ (ಕ್ಷೇತ್ರ) {
    ಸ್ಥಿತಿ IA32_LSTAR_MSR:
    ಸ್ಥಿತಿ IA32_ನಕ್ಷತ್ರ_MSR:
    ಸ್ಥಿತಿ IA32_CSTAR_MSR:
    ಸ್ಥಿತಿ IA32_FMASK_MSR:
    ಸ್ಥಿತಿ IA32_VMX_MISC_MSR:
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("VCPU ReadMSR: Invalid field %lx.", (ದೀರ್ಘ)ಕ್ಷೇತ್ರ);
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಹಿಂತಿರುಗಿ invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ಕ್ಷೇತ್ರ, ಬಫರ್);
}
#ಆದರೆ_ಅಂತ್ಯ  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

ಸ್ಥಿರ exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *ಸ್ಥಾನ, cap_t cap, uint16_t ಕೆಳಗಿನ, uint16_t ಮೇಲಿನ)
{
    /* remove any existing io ports from this cap */
    clearVPIDIOPortMappings(cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_VPID(cap),
                            cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಮೊದಲ_ಪೋರ್ಟ್(cap),
                            cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಕೊನೆಯ_ಪೋರ್ಟ್(cap));
    /* update the assigned vpid. If the vcpu does not have a valid vpid then
     * this is fine as whilst the cap will not point to the vcpu, the vcpu
     * will have its port mask cleared when it gets assigned a vpid */
    cap = cap_io_ಪೋರ್ಟ್_cap_ಗಣ_cap_IOPort_VPID(cap, vcpu->vpid);
    ಸ್ಥಾನ->cap = cap;
    ಗಣ_IOPort_ಮುಖವಾಡ(vcpu->io, ಕೆಳಗಿನ, ಮೇಲಿನ, false);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeEnableIOPort(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *io_ಸ್ಥಾನ;
    uint16_t ಕೆಳಗಿನ, ಮೇಲಿನ;

    ಆದರೆ (ಉದ್ದಳತೆ < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    io_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cap_get_capType(ioCap) != cap_io_ಪೋರ್ಟ್_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಕೆಳಗಿನ = getSyscallArg(0, ಬಫರ್);
    ಮೇಲಿನ = getSyscallArg(1, ಬಫರ್);

    ಆದರೆ (ಕೆಳಗಿನ < cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಮೊದಲ_ಪೋರ್ಟ್(ioCap) || ಮೇಲಿನ > cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಕೊನೆಯ_ಪೋರ್ಟ್(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeEnableIOPort(vcpu, io_ಸ್ಥಾನ, ioCap, ಕೆಳಗಿನ, ಮೇಲಿನ);
}

ಸ್ಥಿರ exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t ಕೆಳಗಿನ, uint16_t ಮೇಲಿನ)
{
    ಗಣ_IOPort_ಮುಖವಾಡ(vcpu->io, ಕೆಳಗಿನ, ಮೇಲಿನ, true);
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeDisableIOPort(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    vcpu_t *vcpu;
    uint16_t ಕೆಳಗಿನ, ಮೇಲಿನ;

    ಆದರೆ (ಉದ್ದಳತೆ < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಕೆಳಗಿನ = getSyscallArg(0, ಬಫರ್);
    ಮೇಲಿನ = getSyscallArg(1, ಬಫರ್);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ಹಿಂತಿರುಗಿ invokeDisableIOPort(vcpu, ಕೆಳಗಿನ, ಮೇಲಿನ);
}

ಸ್ಥಿರ exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, ಶಬ್ದ_t *ಬಫರ್, ಶಬ್ದ_t ಕ್ಷೇತ್ರ, ಶಬ್ದ_t ಮೌಲ್ಯ)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);

    ಆದರೆ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ಆಯ್ಕೆ (ಕ್ಷೇತ್ರ) {
    ಸ್ಥಿತಿ VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = ಮೌಲ್ಯ;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = ಮೌಲ್ಯ;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VMX_CONTROL_CR0_ಮುಖವಾಡ:
        vcpu->cr0_ಮುಖವಾಡ = vcpu->cached_cr0_ಮುಖವಾಡ = ಮೌಲ್ಯ;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = ಮೌಲ್ಯ;
        ನಿಲ್ಲಿಸು;
    }
    vmwrite(ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);

    ಆದರೆ (call) {
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ಗಣ_MR(thread, ಬಫರ್, 0, ಮೌಲ್ಯ);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeWriteVMCS(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t ಕ್ಷೇತ್ರ;
    ಶಬ್ದ_t ಮೌಲ್ಯ;

    ಆದರೆ (ಉದ್ದಳತೆ < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಕ್ಷೇತ್ರ = getSyscallArg(0, ಬಫರ್);
    ಮೌಲ್ಯ = getSyscallArg(1, ಬಫರ್);
    ಆಯ್ಕೆ (ಕ್ಷೇತ್ರ) {
    ಸ್ಥಿತಿ VMX_GUEST_RIP:
    ಸ್ಥಿತಿ VMX_GUEST_RSP:
    ಸ್ಥಿತಿ VMX_GUEST_ES_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_CS_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_SS_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_DS_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_FS_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_GS_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_LDTR_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_TR_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_DEBUGCTRL:
    ಸ್ಥಿತಿ VMX_GUEST_PAT:
    ಸ್ಥಿತಿ VMX_GUEST_EFER:
    ಸ್ಥಿತಿ VMX_GUEST_PERF_GLOBAL_CTRL:
    ಸ್ಥಿತಿ VMX_GUEST_PDPTE0:
    ಸ್ಥಿತಿ VMX_GUEST_PDPTE1:
    ಸ್ಥಿತಿ VMX_GUEST_PDPTE2:
    ಸ್ಥಿತಿ VMX_GUEST_PDPTE3:
    ಸ್ಥಿತಿ VMX_GUEST_ES_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_CS_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_SS_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_DS_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_FS_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_GS_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_LDTR_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_TR_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_GDTR_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_IDTR_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_ES_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_CS_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_SS_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_DS_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_FS_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_GS_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_TR_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_INTERRUPTABILITY:
    ಸ್ಥಿತಿ VMX_GUEST_ACTIVITY:
    ಸ್ಥಿತಿ VMX_GUEST_SMBASE:
    ಸ್ಥಿತಿ VMX_GUEST_SYSENTER_CS:
    ಸ್ಥಿತಿ VMX_GUEST_PREEMPTION_TIMER_ಮೌಲ್ಯ:
    ಸ್ಥಿತಿ VMX_GUEST_ES_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_CS_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_SS_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_DS_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_FS_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_GS_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_LDTR_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_TR_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_GDTR_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_IDTR_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_DR7:
    ಸ್ಥಿತಿ VMX_GUEST_RFLAGS:
    ಸ್ಥಿತಿ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    ಸ್ಥಿತಿ VMX_GUEST_SYSENTER_ESP:
    ಸ್ಥಿತಿ VMX_GUEST_SYSENTER_EIP:
    ಸ್ಥಿತಿ VMX_CONTROL_CR0_ಮುಖವಾಡ:
    ಸ್ಥಿತಿ VMX_CONTROL_CR4_ಮುಖವಾಡ:
    ಸ್ಥಿತಿ VMX_CONTROL_CR0_READ_SHADOW:
    ಸ್ಥಿತಿ VMX_CONTROL_CR4_READ_SHADOW:
    ಸ್ಥಿತಿ VMX_GUEST_CR3:
    ಸ್ಥಿತಿ VMX_CONTROL_EXCEPTION_BITMAP:
    ಸ್ಥಿತಿ VMX_CONTROL_ನಮೂದು_INTERRUPTION_INFO:
    ಸ್ಥಿತಿ VMX_CONTROL_ನಮೂದು_EXCEPTION_ERROR_ಸಂಕೇತ:
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        ಮೌಲ್ಯ = apply_Fixed_ಬಿಟ್ಟುಗಳು(ಮೌಲ್ಯ, pin_control_ಮೇಲಿನ, pin_control_ಕೆಳಗಿನ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        ಮೌಲ್ಯ = apply_Fixed_ಬಿಟ್ಟುಗಳು(ಮೌಲ್ಯ, primary_control_ಮೇಲಿನ, primary_control_ಕೆಳಗಿನ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        ಮೌಲ್ಯ = apply_Fixed_ಬಿಟ್ಟುಗಳು(ಮೌಲ್ಯ, secondary_control_ಮೇಲಿನ, secondary_control_ಕೆಳಗಿನ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VMX_CONTROL_ನಮೂದು_CONTROLS:
        ಮೌಲ್ಯ = apply_Fixed_ಬಿಟ್ಟುಗಳು(ಮೌಲ್ಯ, ನಮೂದು_control_ಮೇಲಿನ, ನಮೂದು_control_ಕೆಳಗಿನ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VMX_CONTROL_EXIT_CONTROLS:
        ಮೌಲ್ಯ = apply_Fixed_ಬಿಟ್ಟುಗಳು(ಮೌಲ್ಯ, exit_control_ಮೇಲಿನ, exit_control_ಕೆಳಗಿನ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VMX_GUEST_CR0:
        ಮೌಲ್ಯ = apply_Fixed_ಬಿಟ್ಟುಗಳು(ಮೌಲ್ಯ, cr0_ಮೇಲಿನ, cr0_ಕೆಳಗಿನ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ VMX_GUEST_CR4:
        ಮೌಲ್ಯ = apply_Fixed_ಬಿಟ್ಟುಗಳು(ಮೌಲ್ಯ, cr4_ಮೇಲಿನ, cr4_ಕೆಳಗಿನ);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("VCPU WriteVMCS: Invalid field %lx.", (ದೀರ್ಘ)ಕ್ಷೇತ್ರ);
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಹಿಂತಿರುಗಿ invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, ಬಫರ್, ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ ಶಬ್ದ_t readVMCSField(vcpu_t *vcpu, ಶಬ್ದ_t ಕ್ಷೇತ್ರ)
{
    ಆಯ್ಕೆ (ಕ್ಷೇತ್ರ) {
    ಸ್ಥಿತಿ VMX_CONTROL_EXCEPTION_BITMAP:
        ಹಿಂತಿರುಗಿ vcpu->exception_bitmap;
    ಸ್ಥಿತಿ VMX_GUEST_CR0:
        ಹಿಂತಿರುಗಿ vcpu->cr0;
    ಸ್ಥಿತಿ VMX_CONTROL_CR0_ಮುಖವಾಡ:
        ಹಿಂತಿರುಗಿ vcpu->cr0_ಮುಖವಾಡ;
    ಸ್ಥಿತಿ VMX_CONTROL_CR0_READ_SHADOW:
        ಹಿಂತಿರುಗಿ vcpu->cr0_shadow;
    }
    ಆದರೆ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ಹಿಂತಿರುಗಿ vmread(ಕ್ಷೇತ್ರ);
}

ಸ್ಥಿರ exception_t invokeReadVMCS(vcpu_t *vcpu, ಶಬ್ದ_t ಕ್ಷೇತ್ರ, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    ಶಬ್ದ_t ಮೌಲ್ಯ = readVMCSField(vcpu, ಕ್ಷೇತ್ರ);
    ಆದರೆ (call) {
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ಗಣ_MR(thread, ಬಫರ್, 0, ಮೌಲ್ಯ);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeReadVMCS(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆದರೆ (ಉದ್ದಳತೆ < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಶಬ್ದ_t ಕ್ಷೇತ್ರ = getSyscallArg(0, ಬಫರ್);
    ಆಯ್ಕೆ (ಕ್ಷೇತ್ರ) {
    ಸ್ಥಿತಿ VMX_GUEST_RIP:
    ಸ್ಥಿತಿ VMX_GUEST_RSP:
    ಸ್ಥಿತಿ VMX_GUEST_ES_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_CS_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_SS_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_DS_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_FS_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_GS_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_LDTR_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_TR_SELECTOR:
    ಸ್ಥಿತಿ VMX_GUEST_DEBUGCTRL:
    ಸ್ಥಿತಿ VMX_GUEST_PAT:
    ಸ್ಥಿತಿ VMX_GUEST_EFER:
    ಸ್ಥಿತಿ VMX_GUEST_PERF_GLOBAL_CTRL:
    ಸ್ಥಿತಿ VMX_GUEST_PDPTE0:
    ಸ್ಥಿತಿ VMX_GUEST_PDPTE1:
    ಸ್ಥಿತಿ VMX_GUEST_PDPTE2:
    ಸ್ಥಿತಿ VMX_GUEST_PDPTE3:
    ಸ್ಥಿತಿ VMX_GUEST_ES_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_CS_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_SS_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_DS_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_FS_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_GS_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_LDTR_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_TR_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_GDTR_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_IDTR_ಎಲ್ಲೆ:
    ಸ್ಥಿತಿ VMX_GUEST_ES_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_CS_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_SS_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_DS_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_FS_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_GS_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_TR_ACCESS_RIGHTS:
    ಸ್ಥಿತಿ VMX_GUEST_INTERRUPTABILITY:
    ಸ್ಥಿತಿ VMX_GUEST_ACTIVITY:
    ಸ್ಥಿತಿ VMX_GUEST_SMBASE:
    ಸ್ಥಿತಿ VMX_GUEST_SYSENTER_CS:
    ಸ್ಥಿತಿ VMX_GUEST_PREEMPTION_TIMER_ಮೌಲ್ಯ:
    ಸ್ಥಿತಿ VMX_GUEST_ES_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_CS_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_SS_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_DS_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_FS_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_GS_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_LDTR_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_TR_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_GDTR_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_IDTR_ತಳ:
    ಸ್ಥಿತಿ VMX_GUEST_DR7:
    ಸ್ಥಿತಿ VMX_GUEST_RFLAGS:
    ಸ್ಥಿತಿ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    ಸ್ಥಿತಿ VMX_GUEST_SYSENTER_ESP:
    ಸ್ಥಿತಿ VMX_GUEST_SYSENTER_EIP:
    ಸ್ಥಿತಿ VMX_CONTROL_CR0_ಮುಖವಾಡ:
    ಸ್ಥಿತಿ VMX_CONTROL_CR4_ಮುಖವಾಡ:
    ಸ್ಥಿತಿ VMX_CONTROL_CR0_READ_SHADOW:
    ಸ್ಥಿತಿ VMX_CONTROL_CR4_READ_SHADOW:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_INSTRUCTION_ERROR:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_EXIT_ಅಡಚಣೆ_INFO:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_EXIT_ಅಡಚಣೆ_ERROR:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_IDT_ಸದಿಶ_INFO:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_IDT_ಸದಿಶ_ERROR:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_EXIT_INSTRUCTION_ಉದ್ದಳತೆ:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_EXIT_INSTRUCTION_INFO:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_GUEST_PHYSICAL:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_IO_RCX:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_IO_RSI:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_IO_RDI:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_IO_RIP:
    ಸ್ಥಿತಿ VMX_ದತ್ತಾಂಶ_GUEST_ರೇಖೀಯ_ವಿಳಾಸ:
    ಸ್ಥಿತಿ VMX_CONTROL_ನಮೂದು_INTERRUPTION_INFO:
    ಸ್ಥಿತಿ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    ಸ್ಥಿತಿ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    ಸ್ಥಿತಿ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    ಸ್ಥಿತಿ VMX_CONTROL_EXCEPTION_BITMAP:
    ಸ್ಥಿತಿ VMX_CONTROL_ನಮೂದು_CONTROLS:
    ಸ್ಥಿತಿ VMX_CONTROL_EXIT_CONTROLS:
    ಸ್ಥಿತಿ VMX_GUEST_CR0:
    ಸ್ಥಿತಿ VMX_GUEST_CR3:
    ಸ್ಥಿತಿ VMX_GUEST_CR4:
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("VCPU ReadVMCS: Invalid field %lx.", (ದೀರ್ಘ)ಕ್ಷೇತ್ರ);
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಹಿಂತಿರುಗಿ invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ಕ್ಷೇತ್ರ, call, ಬಫರ್);
}

ಸ್ಥಿರ exception_t invoke_ಗಣ_TCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decode_ಗಣ_TCB(cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    cap_t tcbCap;
    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಹಿಂತಿರುಗಿ invoke_ಗಣ_TCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

ಶೂನ್ಯ vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    ಶಬ್ದ_t *ಬಫರ್;
    ಆದರೆ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ಬಫರ್ = lookupIPCBuffer(false, ಗಂಟು_STATE(ksCurThread));
    ಆದರೆ (!ಬಫರ್) {
        userError("No IPC buffer.");
        ಹಿಂತಿರುಗಿ;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, ಬಫರ್));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, apply_Fixed_ಬಿಟ್ಟುಗಳು(getSyscallArg(1, ಬಫರ್), primary_control_ಮೇಲಿನ,
                                                                   primary_control_ಕೆಳಗಿನ));
    vmwrite(VMX_CONTROL_ನಮೂದು_INTERRUPTION_INFO, getSyscallArg(2, ಬಫರ್));
}

ಶೂನ್ಯ vcpu_sysvmenter_ಉತ್ತರ_ಗಮ್ಯ_user(tcb_t *tcb)
{
    ಶಬ್ದ_t *ಬಫರ್;
    vcpu_t *vcpu;

    ಬಫರ್ = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    ಪ್ರತಿಜ್ಞಾ(vcpu);

    ಆದರೆ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    ಗಣ_MR(tcb, ಬಫರ್, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    ಗಣ_MR(tcb, ಬಫರ್, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    ಗಣ_MR(tcb, ಬಫರ್, SEL4_VMENTER_CALL_ಅಡಚಣೆ_INFO_MR,
          vmread(VMX_CONTROL_ನಮೂದು_INTERRUPTION_INFO));
    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, 0);
}

exception_t decodeX86VCPUInvocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *ಸ್ಥಾನ,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ X86VCPUSetTCB:
        ಹಿಂತಿರುಗಿ decode_ಗಣ_TCB(cap, ಉದ್ದಳತೆ, ಬಫರ್);
    ಸ್ಥಿತಿ X86VCPUReadVMCS:
        ಹಿಂತಿರುಗಿ decodeReadVMCS(cap, ಉದ್ದಳತೆ, call, ಬಫರ್);
    ಸ್ಥಿತಿ X86VCPUWriteVMCS:
        ಹಿಂತಿರುಗಿ decodeWriteVMCS(cap, ಉದ್ದಳತೆ, call, ಬಫರ್);
    ಸ್ಥಿತಿ X86VCPUEnableIOPort:
        ಹಿಂತಿರುಗಿ decodeEnableIOPort(cap, ಉದ್ದಳತೆ, ಬಫರ್);
    ಸ್ಥಿತಿ X86VCPUDisableIOPort:
        ಹಿಂತಿರುಗಿ decodeDisableIOPort(cap, ಉದ್ದಳತೆ, ಬಫರ್);
    ಸ್ಥಿತಿ X86VCPUWriteRegisters:
        ಹಿಂತಿರುಗಿ decodeVCPUWriteRegisters(cap, ಉದ್ದಳತೆ, ಬಫರ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
    ಸ್ಥಿತಿ X86VCPUWriteMSR:
        ಹಿಂತಿರುಗಿ decodeVCPUWriteMSR(cap, ಉದ್ದಳತೆ, ಬಫರ್);
    ಸ್ಥಿತಿ X86VCPUReadMSR:
        ಹಿಂತಿರುಗಿ decodeVCPUReadMSR(cap, ಉದ್ದಳತೆ, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

ಸ್ಥಿರ bool_t is_vtx_supported(ಶೂನ್ಯ)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    ಹಿಂತಿರುಗಿ !!(x86_cpuid_ecx(0x1, 0) & ಬಿಟ್(5));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ clear_ಬಿಟ್(ಶಬ್ದ_t *bitmap, ಶಬ್ದ_t ಬಿಟ್_2)
{
    ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = ಬಿಟ್_2 / (ಗಾತ್ರ(ಶಬ್ದ_t) * 8);
    ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = ಬಿಟ್_2 % (ಗಾತ್ರ(ಶಬ್ದ_t) * 8);
    bitmap[ಸೂಚ್ಯಂಕ] &= ~ಬಿಟ್(ಸ್ಥಾನಾಂತರ);
}

BOOT_ಸಂಕೇತ bool_t vtx_init(ಶೂನ್ಯ)
{
    ಆದರೆ (!is_vtx_supported()) {
        ಬರೆ("vt-x: not supported\n");
        ಹಿಂತಿರುಗಿ false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_BASIC_MSR);
    vmx_basic.ಪದಗಳು[1] = x86_rdmsr_ಮೇಲಿನ(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_FEATURE_CONTROL_MSR);
    ಆದರೆ (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        ಆದರೆ (feature_control_msr_get_ಬೀಗ(feature_control)) {
            ಬರೆ("vt-x: feature locked\n");
            ಹಿಂತಿರುಗಿ false;
        }
        feature_control = feature_control_msr_ಗಣ_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ಮೇಲಿನ(IA32_FEATURE_CONTROL_MSR), feature_control.ಪದಗಳು[0]);
    }
    /* make sure the msr is locked */
    ಆದರೆ (!feature_control_msr_get_ಬೀಗ(feature_control)) {
        feature_control = feature_control_msr_ಗಣ_ಬೀಗ(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ಮೇಲಿನ(IA32_FEATURE_CONTROL_MSR), feature_control.ಪದಗಳು[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    ಆದರೆ (CURRENT_CPU_ಸೂಚ್ಯಂಕ() == 0) {
        ಆದರೆ (!init_vtx_fixed_ಮೌಲ್ಯಗಳು(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            ಬರೆ("vt-x: lack of required features\n");
            ಹಿಂತಿರುಗಿ false;
        }
    }
    ಆದರೆ (!check_vtx_fixed_ಮೌಲ್ಯಗಳು(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        ಬರೆ("vt-x: cores have inconsistent features\n");
        ಹಿಂತಿರುಗಿ false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    ರಚನೆ vmxon_region *vmxon_kptr = &ARCH_ಗಂಟು_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    ಆದರೆ (!vtx_check_fixed_ಮೌಲ್ಯಗಳು(read_cr0(), read_cr4())) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಆದರೆ (vmxon(kpptr_ಗಮ್ಯ_paddr(vmxon_kptr))) {
        ಬರೆ("vt-x: vmxon failure\n");
        ಹಿಂತಿರುಗಿ false;
    }
    memset(&msr_bitmap_region, ~0, ಗಾತ್ರ(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_ಬಿಟ್(msr_bitmap_region.ಕೆಳಗಿನ_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_ಬಿಟ್(msr_bitmap_region.ಕೆಳಗಿನ_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_ಬಿಟ್(msr_bitmap_region.ಕೆಳಗಿನ_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_ಬಿಟ್(msr_bitmap_region.ಕೆಳಗಿನ_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_ಬಿಟ್(msr_bitmap_region.ಕೆಳಗಿನ_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_ಬಿಟ್(msr_bitmap_region.ಕೆಳಗಿನ_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_ಬಿಟ್(msr_bitmap_region.ಮೇಲಿನ_msr_read.bitmap, MSR_BITMAP_ಮುಖವಾಡ(IA32_FS_ತಳ_MSR));
    clear_ಬಿಟ್(msr_bitmap_region.ಮೇಲಿನ_msr_read.bitmap, MSR_BITMAP_ಮುಖವಾಡ(IA32_GS_ತಳ_MSR));
    clear_ಬಿಟ್(msr_bitmap_region.ಮೇಲಿನ_msr_read.bitmap, MSR_BITMAP_ಮುಖವಾಡ(IA32_KERNEL_GS_ತಳ_MSR));
    clear_ಬಿಟ್(msr_bitmap_region.ಮೇಲಿನ_msr_write.bitmap, MSR_BITMAP_ಮುಖವಾಡ(IA32_FS_ತಳ_MSR));
    clear_ಬಿಟ್(msr_bitmap_region.ಮೇಲಿನ_msr_write.bitmap, MSR_BITMAP_ಮುಖವಾಡ(IA32_GS_ತಳ_MSR));
    clear_ಬಿಟ್(msr_bitmap_region.ಮೇಲಿನ_msr_write.bitmap, MSR_BITMAP_ಮುಖವಾಡ(IA32_KERNEL_GS_ತಳ_MSR));
#ಆದರೆ_ಅಂತ್ಯ
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.ಪದಗಳು[1] = x86_rdmsr_ಮೇಲಿನ(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    ಆದರೆ (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        ಬರೆ("vt-x: Expected wb attribute for EPT paging structure\n");
        ಹಿಂತಿರುಗಿ false;
    }
    ಆದರೆ (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        ಬರೆ("vt-x: Expected supported for 2m pages\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_MRs_vmexit(uint32_t reason, ಶಬ್ದ_t qualification)
{
    ಶಬ್ದ_t *ಬಫರ್;
    ಪೂರ್ಣಾಂಕ i;

    ಬಫರ್ = lookupIPCBuffer(true, ಗಂಟು_STATE(ksCurThread));

    ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_CALL_ಅಡಚಣೆ_INFO_MR,
          vmread(VMX_CONTROL_ನಮೂದು_INTERRUPTION_INFO));
    ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_FAULT_REASON_MR, reason);
    ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_ದತ್ತಾಂಶ_EXIT_INSTRUCTION_ಉದ್ದಳತೆ));
    ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_ದತ್ತಾಂಶ_GUEST_PHYSICAL));
    ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    ಗಾಗಿ (i = 0; i < n_vcpu_gp_ರಿಜಿಸ್ಟರ್; i++) {
        ಗಣ_MR(ಗಂಟು_STATE(ksCurThread), ಬಫರ್, SEL4_VMENTER_FAULT_EAX + i,
              ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ handleVmxFault(uint32_t reason, ಶಬ್ದ_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    ಗಣ_ರಿಜಿಸ್ಟರ್(ಗಂಟು_STATE(ksCurThread), ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, SEL4_VMENTER_ಫಲಿತಾಂಶ_FAULT);

    ಗಣ_MRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಮುಕ್ತಾಯ_Vmexit_Saving(ಶೂನ್ಯ)
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
    uint32_t ಅಡಚಣೆ;
    /* qualification is host width, reason is defined as being 32 bit */
    ಶಬ್ದ_t qualification;
    uint32_t reason;
    ಮುಕ್ತಾಯ_Vmexit_Saving();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X86_64_VTX_64_ಬಿಟ್_GUESTS
    vcpu_restore_host_msrs();
#ಆದರೆ_ಅಂತ್ಯ
    /* the basic exit reason is the bottom 16 bits of the exit reason field */
    reason = vmread(VMX_ದತ್ತಾಂಶ_EXIT_REASON) & ಮುಖವಾಡ(16);
    ಆದರೆ (reason == EXTERNAL_ಅಡಚಣೆ) {
        ಗಂಟು_ಬೀಗ_IRQ;
        ಆದರೆ (vmx_feature_ack_on_exit) {
            ಅಡಚಣೆ = vmread(VMX_ದತ್ತಾಂಶ_EXIT_ಅಡಚಣೆ_INFO) & 0xff;
            ARCH_ಗಂಟು_STATE(x86_KScur_ಅಡಚಣೆ) = ಅಡಚಣೆ;
            handle_ಅಡಚಣೆ_ನಮೂದು();
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(ಗಂಟು_STATE(ksCurThread));
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    ಗಂಟು_ಬೀಗ_SYS;

    ಆಯ್ಕೆ (reason) {
    ಸ್ಥಿತಿ EXCEPTION_OR_NMI:
    ಸ್ಥಿತಿ MOV_DR:
    ಸ್ಥಿತಿ TASK_SWITCH:
    ಸ್ಥಿತಿ CONTROL_ರಿಜಿಸ್ಟರ್:
    ಸ್ಥಿತಿ IO:
    ಸ್ಥಿತಿ MWAIT:
    ಸ್ಥಿತಿ SIPI:
    ಸ್ಥಿತಿ INVLPG:
    ಸ್ಥಿತಿ INVEPT:
    ಸ್ಥಿತಿ INVVPID:
    ಸ್ಥಿತಿ VMCLEAR:
    ಸ್ಥಿತಿ VMPTRLD:
    ಸ್ಥಿತಿ VMPTRST:
    ಸ್ಥಿತಿ VMREAD:
    ಸ್ಥಿತಿ VMWRITE:
    ಸ್ಥಿತಿ VMXON:
    ಸ್ಥಿತಿ EPT_VIOLATION:
    ಸ್ಥಿತಿ GDTR_OR_IDTR:
    ಸ್ಥಿತಿ LDTR_OR_TR:
    ಸ್ಥಿತಿ TPR_ಕೆಳಗೆ_ಮಿತಿ:
    ಸ್ಥಿತಿ APIC_ACCESS:
        qualification = vmread(VMX_ದತ್ತಾಂಶ_EXIT_QUALIFICATION);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t handle_Vm_ನಮೂದು_Fail(ಶೂನ್ಯ)
{
    handleVmxFault(-1, -1);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
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
    ಆದರೆ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_ಸಕ್ರಿಯ) {

        ಶಬ್ದ_t badge = notification_ptr_get_ntfn_ಸಂದೇಶ_Identifier(ntfnPtr);
        notification_ptr_ಗಣ_state(ntfnPtr, NtfnState_Idle);
        ಗಣ_Thread_State(tcb, ThreadState_Running);
        ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, badge_ರಿಜಿಸ್ಟರ್, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        ಆದರೆ (tcb != ಗಂಟು_STATE(ksCurThread)) {
            possible_Switch_ಗಮ್ಯ(tcb);
        }
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

ಸ್ಥಿರ ಶೂನ್ಯ invvpid_context(uint16_t vpid)
{
    ರಚನೆ {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t ವಿಳಾಸ;
    } PACKED ಕಾರ್ಯಾಂಶ = {vpid, 0, 0};
    asm ಅಸ್ಥಿರ("invvpid %0, %1" :: "m"(ಕಾರ್ಯಾಂಶ), "r"((ಶಬ್ದ_t)1) : "cc");
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_EPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_ಬೇರು;
    ಆದರೆ (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_ಬೇರು = kpptr_ಗಮ್ಯ_paddr(null_ept_space);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಕಂಡುಹಿಡಿ_EPTFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_ತಳ_Ptr(vmxSpace);
        ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_EPTFor_ASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE || ಕಂಡುಹಿಡಿ_ret.ept != pml4) {
            ept_ಬೇರು = kpptr_ಗಮ್ಯ_paddr(null_ept_space);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ept_ಬೇರು = pptr_ಗಮ್ಯ_paddr(pml4);
        }
    }
    ಆದರೆ (ept_ಬೇರು != vcpu->ಕೊನೆಯ_ept_ಬೇರು) {
        vcpu->ಕೊನೆಯ_ept_ಬೇರು = ept_ಬೇರು;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_ಬೇರು,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_ಸೂಚಕ, eptp.ಪದಗಳು[0]);
        ಪ್ರತಿಜ್ಞಾ(vcpu->vpid != VPID_INVALID);
        ಆದರೆ (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

ಶೂನ್ಯ clearVPIDIOPortMappings(vpid_t vpid, uint16_t ಮೊದಲ, uint16_t ಕೊನೆಯ)
{
    ಆದರೆ (vpid == VPID_INVALID) {
        ಹಿಂತಿರುಗಿ;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    ಆದರೆ (vcpu == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಪ್ರತಿಜ್ಞಾ(vcpu->vpid == vpid);
    ಗಣ_IOPort_ಮುಖವಾಡ(vcpu->io, ಮೊದಲ, ಕೊನೆಯ, true);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ vpid_t ಮುಂದಿನ_VPID(vpid_t vpid)
{
    ಆದರೆ (vpid == VPID_ಕೊನೆಯ) {
        ಹಿಂತಿರುಗಿ VPID_ಮೊದಲ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ vpid + 1;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, ಗಾತ್ರ(vcpu->io));
    /* invalidate the VPID context */
    ಆದರೆ (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
ಸ್ಥಿರ ಶೂನ್ಯ releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = ಖಾಲಿ;
}

ಸ್ಥಿರ vpid_t ಕಂಡುಹಿಡಿ_Free_VPID(ಶೂನ್ಯ)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    ಮಾಡು {
        ಆದರೆ (x86KSVPIDTable[vpid] == ಖಾಲಿ) {
            ಹಿಂತಿರುಗಿ vpid;
        }
        vpid = ಮುಂದಿನ_VPID(vpid);
    } ಆಗ (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = ಮುಂದಿನ_VPID(x86KSNextVPID);
    ಹಿಂತಿರುಗಿ vpid;
}

ಸ್ಥಿರ ಶೂನ್ಯ storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    ಪ್ರತಿಜ್ಞಾ(x86KSVPIDTable[vpid] == ಖಾಲಿ);
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
ಶೂನ್ಯ vcpu_fpu_ಗಮ್ಯ_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    ಆದರೆ (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        ಆದರೆ (!vcpu->fpu_ಸಕ್ರಿಯ) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_ಸಕ್ರಿಯ = true;
            ತುಂಬು_Fpu_State(tcb);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_ಸಕ್ರಿಯ = true;
        switchLocalFpuOwner(tcb);
    }
}

ಶೂನ್ಯ vcpu_fpu_ಗಮ್ಯ_host(tcb_t *tcb, vcpu_t *vcpu)
{
    ಆದರೆ (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nativeThreadUsingFPU(tcb) && vcpu->fpu_ಸಕ್ರಿಯ) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_ಸಕ್ರಿಯ = false;
        ತುಂಬು_Fpu_State(tcb);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

ಶೂನ್ಯ restoreVMCS(ಶೂನ್ಯ)
{
    tcb_t *cur_thread = ಗಂಟು_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    ಆದರೆ (ARCH_ಗಂಟು_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
    ಆದರೆ (getCurrentCR3().ಪದಗಳು[0] != expected_vmcs->ಕೊನೆಯ_host_cr3) {
        expected_vmcs->ಕೊನೆಯ_host_cr3 = getCurrentCR3().ಪದಗಳು[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().ಪದಗಳು[0]);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = ಕಂಡುಹಿಡಿ_Free_VPID();
        storeVPID(expected_vmcs, vpid);
        ಆದರೆ (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    ಗಣ_EPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

ಶೂನ್ಯ invept(ept_pml4e_t *ept_pml4)
{
    ಆದರೆ (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        ರಚನೆ {
            uint64_t parts[2];
        } ವಿಳಾಸ;
        ಶಬ್ದ_t type;
        ಆದರೆ (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            ಹಿಂತಿರುಗಿ;
        }

        ವಿಳಾಸ.parts[0] = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)ept_pml4);
        ವಿಳಾಸ.parts[1] = 0;
        asm ಅಸ್ಥಿರ(
            "invept %0, %1"
            :
            : "m"(ವಿಳಾಸ),  "r"(type)
            : "memory"
        );
    }
}

#ಆದರೆ_ಅಂತ್ಯ
