/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/objecttype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/cpu_registers.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/vcpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/ioport.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sel4/arch/vmenter.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VMX_EXIT_QUAL_TYPE_MOV_CR 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VMX_EXIT_QUAL_TYPE_CLTS 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VMX_EXIT_QUAL_TYPE_LMSW 3

ਅਚਲ vcpu_gp_ਰਜਿਸਟਰ_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSR_BITMAP_ਨਕਾਬ(x) ((x) & 0x1fff)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ msr_bitmap {
    ਸ਼ਬਦ_t bitmap[0x2000 / ਆਕਾਰ(ਸ਼ਬਦ_t) / 8];
} msr_bitmap_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ msr_bitmaps {
    msr_bitmap_t ਨੀਵਾਂ_msr_read;
    msr_bitmap_t ਉੱਚਾ_msr_read;
    msr_bitmap_t ਨੀਵਾਂ_msr_write;
    msr_bitmap_t ਉੱਚਾ_msr_write;
} msr_bitmaps_t;

ਸਥਿਰ msr_bitmaps_t msr_bitmap_region ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਬਿੱਟ));

ਸਥਿਰ ਅੱਖਰ null_ept_space[ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਬਿੱਟ));

/* Cached value of the hardware defined vmcs revision */
ਸਥਿਰ uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
ਸਥਿਰ vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
ਸਥਿਰ uint32_t pin_control_ਉੱਚਾ;
ਸਥਿਰ uint32_t pin_control_ਨੀਵਾਂ;
ਸਥਿਰ uint32_t primary_control_ਉੱਚਾ;
ਸਥਿਰ uint32_t primary_control_ਨੀਵਾਂ;
ਸਥਿਰ uint32_t secondary_control_ਉੱਚਾ;
ਸਥਿਰ uint32_t secondary_control_ਨੀਵਾਂ;
ਸਥਿਰ uint32_t ਇੰਦਰਾਜ_control_ਉੱਚਾ;
ਸਥਿਰ uint32_t ਇੰਦਰਾਜ_control_ਨੀਵਾਂ;
ਸਥਿਰ uint32_t exit_control_ਉੱਚਾ;
ਸਥਿਰ uint32_t exit_control_ਨੀਵਾਂ;
ਸਥਿਰ uint32_t cr0_ਉੱਚਾ;
ਸਥਿਰ uint32_t cr0_ਨੀਵਾਂ;
ਸਥਿਰ uint32_t cr4_ਉੱਚਾ;
ਸਥਿਰ uint32_t cr4_ਨੀਵਾਂ;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
ਸਥਿਰ bool_t vmx_feature_vpid;
ਸਥਿਰ bool_t vmx_feature_ਲੱਦੋ_perf_global_ctrl;
ਸਥਿਰ bool_t vmx_feature_ack_on_exit;

ਸਥਿਰ vcpu_t *x86KSVPIDTable[VPID_ਆਖਰੀ + 1];
ਸਥਿਰ vpid_t x86KSNextVPID = VPID_ਪਹਿਲਾ;

ਸਥਿਰ ਖਾਲੀ releaseVPID(vpid_t vpid);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm ਅਸਥਿਰ(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    ਵਾਪਸ !!error;
}

ਸਥਿਰ ਖਾਲੀ vmclear(ਖਾਲੀ *vmcs_ptr)
{
    uint64_t physical_ਪਤਾ;
    physical_ਪਤਾ = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)vmcs_ptr);
    asm ਅਸਥਿਰ(
        "vmclear %0"
        :
        : "m"(physical_ਪਤਾ)
        : "cc"
    );
}

ਖਾਲੀ clearCurrentVCPU(ਖਾਲੀ)
{
    vcpu_t *vcpu = ARCH_ਗੰਢ_STATE(x86KSCurrentVCPU);
    ਜੇ (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_ਗੰਢ_STATE(x86KSCurrentVCPU) = ਨਿਰਾ;
        ਜੇ (vcpu->fpu_ਸਰਗਰਮ && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_ਸਰਗਰਮ = false;
        }
    }
}

ਸਥਿਰ ਖਾਲੀ vmptrld(ਖਾਲੀ *vmcs_ptr)
{
    uint64_t physical_ਪਤਾ;
    uint8_t error;
    physical_ਪਤਾ = pptr_ਮੰਜ਼ਿਲ_paddr(vmcs_ptr);
    asm ਅਸਥਿਰ(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_ਪਤਾ)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    ਪ੍ਰਤਿਜ੍ਞਾ(!error);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
ਖਾਲੀ vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_ਤਾਰਾ_MSR, vcpu->syscall_registers[VCPU_ਤਾਰਾ]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_ਨਕਾਬ]);
}

ਖਾਲੀ vcpu_restore_host_msrs(ਖਾਲੀ)
{
    init_syscall_msrs();
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ switchVCPU(vcpu_t *vcpu)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਜੇ (vcpu->ਆਖਰੀ_cpu != getCurrentCPUIndex() && ARCH_ਗੰਢ_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->ਆਖਰੀ_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->ਆਖਰੀ_cpu);
    }
#ਜੇ_ਅੰਤ
    clearCurrentVCPU();
    vmptrld(vcpu);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਜੇ (vcpu->ਆਖਰੀ_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_ਅਧਾਰ, (ਸ਼ਬਦ_t)&x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KStss);
        vmwrite(VMX_HOST_GDTR_ਅਧਾਰ, (ਸ਼ਬਦ_t)x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_ਅਧਾਰ, (ਸ਼ਬਦ_t)x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(ਸ਼ਬਦ_t)((ਅੱਖਰ *)&x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KStss.tss.ਸ਼ਬਦ[0] +
                                                          4));
    }
    vcpu->ਆਖਰੀ_cpu = getCurrentCPUIndex();
#ਜੇ_ਅੰਤ
    ARCH_ਗੰਢ_STATE(x86KSCurrentVCPU) = vcpu;
}

ਸਥਿਰ ਖਾਲੀ print_ਬਿੱਟ(ਸ਼ਬਦ_t ਬਿੱਟ)
{
    bool_t ਪਹਿਲਾ = true;
    ਜਦੋਂ (ਬਿੱਟ) {
        ਪੂਰਨ ਸੂਚਕਾਂਕ = se_L4_ਸ਼ਬਦ_ਬਿੱਟ - 1 - clzl(ਬਿੱਟ);
        ਜੇ (ਪਹਿਲਾ) {
            ਲਿਖੋ("%d", ਸੂਚਕਾਂਕ);
            ਪਹਿਲਾ = false;
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ(",%d", ਸੂਚਕਾਂਕ);
        }
        ਬਿੱਟ &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(ਸੂਚਕਾਂਕ);
    }
}

ਸਥਿਰ bool_t check_fixed_ਮੁੱਲ(ਸ਼ਬਦ_t val, ਸ਼ਬਦ_t ਨੀਵਾਂ, ਸ਼ਬਦ_t ਉੱਚਾ)
{
    ਸ਼ਬਦ_t not_ਉੱਚਾ;
    ਸ਼ਬਦ_t not_ਨੀਵਾਂ;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_ਉੱਚਾ = ਉੱਚਾ & ~(ਉੱਚਾ & val);
    ਜੇ (not_ਉੱਚਾ != 0) {
        ਲਿਖੋ("Failed to set bits: ");
        print_ਬਿੱਟ(not_ਉੱਚਾ);
        ਵਾਪਸ false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_ਨੀਵਾਂ = ~ਨੀਵਾਂ & ~(~ਨੀਵਾਂ & ~val);
    ਜੇ (not_ਨੀਵਾਂ != 0) {
        ਲਿਖੋ("Incorrectly cleared bits: ");
        print_ਬਿੱਟ(not_ਨੀਵਾਂ);
        ਵਾਪਸ false;
    }
    ਵਾਪਸ true;
}

ਸਥਿਰ bool_t vtx_check_fixed_ਮੁੱਲਾਂ(ਸ਼ਬਦ_t cr0, ਸ਼ਬਦ_t cr4)
{
    ਜੇ (!check_fixed_ਮੁੱਲ(cr0, cr0_ਨੀਵਾਂ, cr0_ਉੱਚਾ)) {
        ਲਿਖੋ(" of CR0\n");
        ਵਾਪਸ false;
    }
    ਜੇ (!check_fixed_ਮੁੱਲ(cr4, cr4_ਨੀਵਾਂ, cr4_ਉੱਚਾ)) {
        ਲਿਖੋ(" of CR4\n");
        ਵਾਪਸ false;
    }
    ਵਾਪਸ true;
}

ਸਥਿਰ bool_t BOOT_ਕੋਡ init_vtx_fixed_ਮੁੱਲਾਂ(bool_t useTrueMsrs)
{
    uint32_t pin_control_ਨਕਾਬ =
        ਦੋਆਧਾਰੀ_ਅੰਕ(0) |    //Extern interrupt exiting
        ਦੋਆਧਾਰੀ_ਅੰਕ(3) |    //NMI exiting
        ਦੋਆਧਾਰੀ_ਅੰਕ(5);     //virtual NMIs
    uint32_t primary_control_ਨਕਾਬ =
        ਦੋਆਧਾਰੀ_ਅੰਕ(25) |   //Use I/O bitmaps
        ਦੋਆਧਾਰੀ_ਅੰਕ(28) |   //Use MSR bitmaps
        ਦੋਆਧਾਰੀ_ਅੰਕ(31);    //Activate secondary controls
    uint32_t secondary_control_ਨਕਾਬ =
        ਦੋਆਧਾਰੀ_ਅੰਕ(1);     //Enable EPT
    uint32_t exit_control_ਨਕਾਬ =
        ਦੋਆਧਾਰੀ_ਅੰਕ(2)  |   //Save debug controls
        ਦੋਆਧਾਰੀ_ਅੰਕ(18) |   //Save guest IA32_PAT on exit
        ਦੋਆਧਾਰੀ_ਅੰਕ(19) |   //Load host IA32_PAT
        ਦੋਆਧਾਰੀ_ਅੰਕ(20) |   //Save guest IA32_EFER on exit
        ਦੋਆਧਾਰੀ_ਅੰਕ(21);    //Load host IA32_EFER
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_X86_64
    exit_control_ਨਕਾਬ |= ਦੋਆਧਾਰੀ_ਅੰਕ(9); //Host address-space size
#ਜੇ_ਅੰਤ /* CONFIG_ARCH_X86_64 */
    /* Read out the fixed high and low bits from the MSRs */
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t ਇੰਦਰਾਜ_ctls;
    ਜੇ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        ਇੰਦਰਾਜ_ctls = IA32_VMX_TRUE_ਇੰਦਰਾਜ_CTLS_MSR;
    } ਨਹੀਂਤਾਂ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        ਇੰਦਰਾਜ_ctls = IA32_VMX_ਇੰਦਰਾਜ_CTLS_MSR;
    }
    pin_control_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(pinbased_ctls);
    pin_control_ਨੀਵਾਂ = x86_rdmsr_ਉੱਚਾ(pinbased_ctls);
    primary_control_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(procbased_ctls);
    primary_control_ਨੀਵਾਂ = x86_rdmsr_ਉੱਚਾ(procbased_ctls);
    secondary_control_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_ਨੀਵਾਂ = x86_rdmsr_ਉੱਚਾ(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(exit_ctls);
    exit_control_ਨੀਵਾਂ = x86_rdmsr_ਉੱਚਾ(exit_ctls);
    ਇੰਦਰਾਜ_control_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(ਇੰਦਰਾਜ_ctls);
    ਇੰਦਰਾਜ_control_ਨੀਵਾਂ = x86_rdmsr_ਉੱਚਾ(ਇੰਦਰਾਜ_ctls);

    cr0_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_CR0_FIXED0_MSR);
    cr0_ਨੀਵਾਂ = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_CR0_FIXED1_MSR);
    cr4_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_CR4_FIXED0_MSR);
    cr4_ਨੀਵਾਂ = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_CR4_FIXED1_MSR);

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
    ਜੇ (secondary_control_ਨੀਵਾਂ & ਦੋਆਧਾਰੀ_ਅੰਕ(7)) {
        cr0_ਉੱਚਾ &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    ਜੇ (!(secondary_control_ਨੀਵਾਂ & ਦੋਆਧਾਰੀ_ਅੰਕ(5))) {
        vmx_feature_vpid = 0;
        ਲਿਖੋ("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } ਨਹੀਂਤਾਂ {
        vmx_feature_vpid = 1;
        secondary_control_ਨਕਾਬ |= ਦੋਆਧਾਰੀ_ਅੰਕ(5);
    }

    /* Check for load perf global control */
    ਜੇ (!(exit_control_ਨੀਵਾਂ & ਦੋਆਧਾਰੀ_ਅੰਕ(12))) {
        vmx_feature_ਲੱਦੋ_perf_global_ctrl = 0;
        ਲਿਖੋ("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } ਨਹੀਂਤਾਂ {
        vmx_feature_ਲੱਦੋ_perf_global_ctrl = 1;
        exit_control_ਨਕਾਬ |= ਦੋਆਧਾਰੀ_ਅੰਕ(12);
    }

    /* Check for external interrupt exiting */
    ਜੇ (!(exit_control_ਨੀਵਾਂ & ਦੋਆਧਾਰੀ_ਅੰਕ(15))) {
        vmx_feature_ack_on_exit = 0;
        ਲਿਖੋ("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } ਨਹੀਂਤਾਂ {
        vmx_feature_ack_on_exit = 1;
        exit_control_ਨਕਾਬ |= ਦੋਆਧਾਰੀ_ਅੰਕ(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_ਨੀਵਾਂ) & pin_control_ਨਕਾਬ;
    ਜੇ (missing) {
        ਲਿਖੋ("vt-x: Unsupported pin control features %lx\n", (ਲੰਮਾ)missing);
        ਵਾਪਸ false;
    }
    missing = (~primary_control_ਨੀਵਾਂ) & primary_control_ਨਕਾਬ;
    ਜੇ (missing) {
        ਲਿਖੋ("vt-x: Unsupported primary control features %lx\n", (ਲੰਮਾ)missing);
        ਵਾਪਸ false;
    }
    missing = (~secondary_control_ਨੀਵਾਂ) & secondary_control_ਨਕਾਬ;
    ਜੇ (missing) {
        ਲਿਖੋ("vt-x: Unsupported secondary control features %lx\n", (ਲੰਮਾ)missing);
        ਵਾਪਸ false;
    }
    missing = (~exit_control_ਨੀਵਾਂ) & exit_control_ਨਕਾਬ;
    ਜੇ (missing) {
        ਲਿਖੋ("vt-x: Unsupported exit control features %lx\n", (ਲੰਮਾ)missing);
        ਵਾਪਸ false;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
    missing = (~ਇੰਦਰਾਜ_control_ਨੀਵਾਂ) & ਦੋਆਧਾਰੀ_ਅੰਕ(9);
    ਜੇ (missing) {
        ਲਿਖੋ("vt-x: CPU does not support 64-bit guests\n");
        ਵਾਪਸ false;
    }
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_ਉੱਚਾ |= pin_control_ਨਕਾਬ;
    primary_control_ਉੱਚਾ |= primary_control_ਨਕਾਬ;
    secondary_control_ਉੱਚਾ |= secondary_control_ਨਕਾਬ;
    exit_control_ਉੱਚਾ |= exit_control_ਨਕਾਬ;

    ਵਾਪਸ true;
}

ਸਥਿਰ bool_t BOOT_ਕੋਡ check_vtx_fixed_ਮੁੱਲਾਂ(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t ਇੰਦਰਾਜ_ctls;
    ਜੇ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        ਇੰਦਰਾਜ_ctls = IA32_VMX_TRUE_ਇੰਦਰਾਜ_CTLS_MSR;
    } ਨਹੀਂਤਾਂ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        ਇੰਦਰਾਜ_ctls = IA32_VMX_ਇੰਦਰਾਜ_CTLS_MSR;
    }
    uint32_t local_pin_control_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(pinbased_ctls);
    uint32_t local_pin_control_ਨੀਵਾਂ = x86_rdmsr_ਉੱਚਾ(pinbased_ctls);
    uint32_t local_primary_control_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(procbased_ctls);
    uint32_t local_primary_control_ਨੀਵਾਂ = x86_rdmsr_ਉੱਚਾ(procbased_ctls);
    uint32_t local_secondary_control_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_ਨੀਵਾਂ = x86_rdmsr_ਉੱਚਾ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(exit_ctls);
    uint32_t local_exit_control_ਨੀਵਾਂ = x86_rdmsr_ਉੱਚਾ(exit_ctls);
    uint32_t local_ਇੰਦਰਾਜ_control_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(ਇੰਦਰਾਜ_ctls);
    uint32_t local_ਇੰਦਰਾਜ_control_ਨੀਵਾਂ = x86_rdmsr_ਉੱਚਾ(ਇੰਦਰਾਜ_ctls);

    uint32_t local_cr0_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_ਨੀਵਾਂ = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_ਉੱਚਾ = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_ਨੀਵਾਂ = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    ਜੇ (local_secondary_control_ਨੀਵਾਂ & ਦੋਆਧਾਰੀ_ਅੰਕ(7)) {
        local_cr0_ਉੱਚਾ &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    ਵਾਪਸ
        (local_pin_control_ਉੱਚਾ & pin_control_ਉੱਚਾ) == local_pin_control_ਉੱਚਾ &&
        (~local_pin_control_ਨੀਵਾਂ & pin_control_ਉੱਚਾ) == 0 &&
        (local_primary_control_ਉੱਚਾ & primary_control_ਉੱਚਾ) == local_primary_control_ਉੱਚਾ &&
        (~local_primary_control_ਨੀਵਾਂ & primary_control_ਉੱਚਾ) == 0 &&
        (local_secondary_control_ਉੱਚਾ & secondary_control_ਉੱਚਾ) == local_secondary_control_ਉੱਚਾ &&
        (~local_secondary_control_ਨੀਵਾਂ & secondary_control_ਉੱਚਾ) == 0 &&
        (local_exit_control_ਉੱਚਾ & exit_control_ਉੱਚਾ) == local_exit_control_ਉੱਚਾ &&
        (~local_exit_control_ਨੀਵਾਂ & exit_control_ਉੱਚਾ) == 0 &&
        (local_ਇੰਦਰਾਜ_control_ਉੱਚਾ & ਇੰਦਰਾਜ_control_ਉੱਚਾ) == local_ਇੰਦਰਾਜ_control_ਉੱਚਾ &&
        (~local_ਇੰਦਰਾਜ_control_ਨੀਵਾਂ & ਇੰਦਰਾਜ_control_ਉੱਚਾ) == 0 &&
        local_cr0_ਉੱਚਾ == cr0_ਉੱਚਾ &&
        local_cr0_ਨੀਵਾਂ == cr0_ਨੀਵਾਂ &&
        local_cr4_ਉੱਚਾ == cr4_ਉੱਚਾ &&
        local_cr4_ਨੀਵਾਂ == cr4_ਨੀਵਾਂ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t apply_Fixed_ਬਿੱਟ(uint32_t original, uint32_t ਉੱਚਾ, uint32_t ਨੀਵਾਂ)
{
    original |= ਉੱਚਾ;
    original &= ਨੀਵਾਂ;
    ਵਾਪਸ original;
}

ਖਾਲੀ vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = ਨਿਰਾ;
    vcpu->launched = false;
    vcpu->fpu_ਸਰਗਰਮ = false;
    vcpu->fpuState = x86KSnullFpuState;

    ਸਮ੍ਰਿਤੀ_ਨਕਲ(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_ਉੱਚਾ & cr0_ਨੀਵਾਂ;
    vcpu->cr0_shadow = 0;
    vcpu->cr0_ਨਕਾਬ = 0;
    vcpu->exception_bitmap = 0;
    vcpu->vpid = VPID_INVALID;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    vcpu->ਆਖਰੀ_cpu = getCurrentCPUIndex();
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

    switchVCPU(vcpu);

    vmwrite(VMX_HOST_PAT, x86_rdmsr(IA32_PAT_MSR));
    vmwrite(VMX_HOST_EFER, x86_rdmsr(IA32_EFER_MSR));
    // By default we will disable performance counters when we come out
    // of a VM. When performance counters are supported this host state
    // needs to be updated on VM entry
    ਜੇ (vmx_feature_ਲੱਦੋ_perf_global_ctrl) {
        vmwrite(VMX_HOST_PERF_GLOBAL_CTRL, 0);
    }
    vmwrite(VMX_HOST_CR0, read_cr0());
    vmwrite(VMX_HOST_CR4, read_cr4());
    vmwrite(VMX_HOST_FS_ਅਧਾਰ, 0);
    vmwrite(VMX_HOST_GS_ਅਧਾਰ, 0);
    vmwrite(VMX_HOST_TR_ਅਧਾਰ, (ਸ਼ਬਦ_t)&x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KStss);
    vmwrite(VMX_HOST_GDTR_ਅਧਾਰ, (ਸ਼ਬਦ_t)x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KSgdt);
    vmwrite(VMX_HOST_IDTR_ਅਧਾਰ, (ਸ਼ਬਦ_t)x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KSidt);
    vmwrite(VMX_HOST_SYSENTER_CS, (ਸ਼ਬਦ_t)SEL_CS_0);
    vmwrite(VMX_HOST_SYSENTER_EIP, (ਸ਼ਬਦ_t)&handle_syscall);
    ਜੇ (!config_ਸੈੱਟ(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(ਸ਼ਬਦ_t)((ਅੱਖਰ *)&x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KStss.tss.ਸ਼ਬਦ[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (ਸ਼ਬਦ_t)&vcpu->gp_registers[n_vcpu_gp_ਰਜਿਸਟਰ]);
    vmwrite(VMX_HOST_RIP, (ਸ਼ਬਦ_t)&handle_vmexit);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_ਮੰਜ਼ਿਲ_paddr(x64KSKernelPML4), 0).ਸ਼ਬਦ[0]);
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_ਉੱਚਾ & pin_control_ਨੀਵਾਂ);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_ਉੱਚਾ & primary_control_ਨੀਵਾਂ);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_ਉੱਚਾ & secondary_control_ਨੀਵਾਂ);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_ਉੱਚਾ & exit_control_ਨੀਵਾਂ);

    uint64_t ਇੰਦਰਾਜ_control = ਇੰਦਰਾਜ_control_ਉੱਚਾ & ਇੰਦਰਾਜ_control_ਨੀਵਾਂ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    ਇੰਦਰਾਜ_control |= ਦੋਆਧਾਰੀ_ਅੰਕ(9);
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ਇੰਦਰਾਜ_CONTROLS, ਇੰਦਰਾਜ_control);

    vmwrite(VMX_CONTROL_MSR_ਪਤਾ, (ਸ਼ਬਦ_t)kpptr_ਮੰਜ਼ਿਲ_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_ਉੱਚਾ & cr4_ਨੀਵਾਂ);

    vmwrite(VMX_GUEST_VMCS_LINK_ਸੂਚਕ, ~(ਸ਼ਬਦ_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_ਸੂਚਕ_ਉੱਚਾ, ~(ਸ਼ਬਦ_t)0);

    memset(vcpu->io, ~(ਸ਼ਬਦ_t)0, VCPU_IOBITMAP_ਮਾਪ);
    vmwrite(VMX_CONTROL_IOA_ਪਤਾ, pptr_ਮੰਜ਼ਿਲ_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ਪਤਾ, pptr_ਮੰਜ਼ਿਲ_paddr((ਅੱਖਰ *)vcpu->io + (VCPU_IOBITMAP_ਮਾਪ / 2)));
}

ਸਥਿਰ ਖਾਲੀ dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(tcb->tcbArch.tcbVCPU == vcpu);
    ਪ੍ਰਤਿਜ੍ਞਾ(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_ਸਰਗਰਮ = false;
    tcb->tcbArch.tcbVCPU = ਨਿਰਾ;
    vcpu->vcpuTCB = ਨਿਰਾ;
}

ਖਾਲੀ vcpu_finalise(vcpu_t *vcpu)
{
    ਜੇ (ARCH_ਗੰਢ_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->ਆਖਰੀ_cpu) == vcpu) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
        ਜੇ (vcpu->ਆਖਰੀ_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->ਆਖਰੀ_cpu);
        } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    ਜੇ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    ਜੇ (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

ਸਥਿਰ ਖਾਲੀ associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ਜੇ (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    ਜੇ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

ਸਥਿਰ exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਪੂਰਨ i;
    ਲਈ (i = 0; i < n_vcpu_gp_ਰਜਿਸਟਰ; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, ਬਫਰ);
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeVCPUWriteRegisters(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
    ਜੇ (ਲੰਮਾਈ < 15) {
#ਨਹੀਂਤਾਂ
    ਜੇ (ਲੰਮਾਈ < 7) {
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਵਾਪਸ invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ਬਫਰ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
ਸਥਿਰ exception_t invokeWriteMSR(vcpu_t *vcpu, ਸ਼ਬਦ_t *ਬਫਰ, ਸ਼ਬਦ_t ਖੇਤਰ, ਸ਼ਬਦ_t ਮੁੱਲ)
{
    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);
    ਜੇ (ARCH_ਗੰਢ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ਚੋਣ (ਖੇਤਰ) {
    ਹਾਲਤ IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = ਮੁੱਲ;
        ਰੋਕੋ;
    ਹਾਲਤ IA32_ਤਾਰਾ_MSR:
        vcpu->syscall_registers[VCPU_ਤਾਰਾ] = ਮੁੱਲ;
        ਰੋਕੋ;
    ਹਾਲਤ IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = ਮੁੱਲ;
        ਰੋਕੋ;
    ਹਾਲਤ IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_ਨਕਾਬ] = ਮੁੱਲ;
        ਰੋਕੋ;
    }
    ਸੈੱਟ_MR(thread, ਬਫਰ, 0, ਮੁੱਲ);
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeVCPUWriteMSR(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t ਖੇਤਰ;
    ਸ਼ਬਦ_t ਮੁੱਲ;

    ਜੇ (ਲੰਮਾਈ < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਖੇਤਰ = getSyscallArg(0, ਬਫਰ);
    ਮੁੱਲ = getSyscallArg(1, ਬਫਰ);
    ਚੋਣ (ਖੇਤਰ) {
    ਹਾਲਤ IA32_LSTAR_MSR:
    ਹਾਲਤ IA32_ਤਾਰਾ_MSR:
    ਹਾਲਤ IA32_CSTAR_MSR:
    ਹਾਲਤ IA32_FMASK_MSR:
        ਰੋਕੋ;
    ਮੂਲ:
        userError("VCPU WriteMSR: Invalid field %lx.", (ਲੰਮਾ)ਖੇਤਰ);
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਵਾਪਸ invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ਬਫਰ, ਖੇਤਰ, ਮੁੱਲ);
}

ਸਥਿਰ exception_t invokeReadMSR(vcpu_t *vcpu, ਸ਼ਬਦ_t ਖੇਤਰ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);

    ਸ਼ਬਦ_t ਮੁੱਲ = 0;

    ਚੋਣ (ਖੇਤਰ) {
    ਹਾਲਤ IA32_LSTAR_MSR:
        ਮੁੱਲ = vcpu->syscall_registers[VCPU_LSTAR];
        ਰੋਕੋ;
    ਹਾਲਤ IA32_ਤਾਰਾ_MSR:
        ਮੁੱਲ = vcpu->syscall_registers[VCPU_ਤਾਰਾ];
        ਰੋਕੋ;
    ਹਾਲਤ IA32_CSTAR_MSR:
        ਮੁੱਲ = vcpu->syscall_registers[VCPU_CSTAR];
        ਰੋਕੋ;
    ਹਾਲਤ IA32_FMASK_MSR:
        ਮੁੱਲ = vcpu->syscall_registers[VCPU_SYSCALL_ਨਕਾਬ];
        ਰੋਕੋ;
    ਹਾਲਤ IA32_VMX_MISC_MSR:
        ਮੁੱਲ = x86_rdmsr(ਖੇਤਰ);
        ਰੋਕੋ;
    }

    ਸੈੱਟ_MR(thread, ਬਫਰ, 0, ਮੁੱਲ);
    ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                    se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, 1)));
    ਸੈੱਟ_Thread_State(thread, ThreadState_Restart);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeVCPUReadMSR(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਜੇ (ਲੰਮਾਈ < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਸ਼ਬਦ_t ਖੇਤਰ = getSyscallArg(0, ਬਫਰ);
    ਚੋਣ (ਖੇਤਰ) {
    ਹਾਲਤ IA32_LSTAR_MSR:
    ਹਾਲਤ IA32_ਤਾਰਾ_MSR:
    ਹਾਲਤ IA32_CSTAR_MSR:
    ਹਾਲਤ IA32_FMASK_MSR:
    ਹਾਲਤ IA32_VMX_MISC_MSR:
        ਰੋਕੋ;
    ਮੂਲ:
        userError("VCPU ReadMSR: Invalid field %lx.", (ਲੰਮਾ)ਖੇਤਰ);
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਵਾਪਸ invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ਖੇਤਰ, ਬਫਰ);
}
#ਜੇ_ਅੰਤ  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

ਸਥਿਰ exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *ਥਾਂ, cap_t cap, uint16_t ਨੀਵਾਂ, uint16_t ਉੱਚਾ)
{
    /* remove any existing io ports from this cap */
    clearVPIDIOPortMappings(cap_io_ਪੋਰਟ_cap_get_cap_IOPort_VPID(cap),
                            cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਪਹਿਲਾ_ਪੋਰਟ(cap),
                            cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਆਖਰੀ_ਪੋਰਟ(cap));
    /* update the assigned vpid. If the vcpu does not have a valid vpid then
     * this is fine as whilst the cap will not point to the vcpu, the vcpu
     * will have its port mask cleared when it gets assigned a vpid */
    cap = cap_io_ਪੋਰਟ_cap_ਸੈੱਟ_cap_IOPort_VPID(cap, vcpu->vpid);
    ਥਾਂ->cap = cap;
    ਸੈੱਟ_IOPort_ਨਕਾਬ(vcpu->io, ਨੀਵਾਂ, ਉੱਚਾ, false);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeEnableIOPort(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *io_ਥਾਂ;
    uint16_t ਨੀਵਾਂ, ਉੱਚਾ;

    ਜੇ (ਲੰਮਾਈ < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    io_ਥਾਂ = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cap_get_capType(ioCap) != cap_io_ਪੋਰਟ_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਨੀਵਾਂ = getSyscallArg(0, ਬਫਰ);
    ਉੱਚਾ = getSyscallArg(1, ਬਫਰ);

    ਜੇ (ਨੀਵਾਂ < cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਪਹਿਲਾ_ਪੋਰਟ(ioCap) || ਉੱਚਾ > cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਆਖਰੀ_ਪੋਰਟ(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeEnableIOPort(vcpu, io_ਥਾਂ, ioCap, ਨੀਵਾਂ, ਉੱਚਾ);
}

ਸਥਿਰ exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t ਨੀਵਾਂ, uint16_t ਉੱਚਾ)
{
    ਸੈੱਟ_IOPort_ਨਕਾਬ(vcpu->io, ਨੀਵਾਂ, ਉੱਚਾ, true);
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeDisableIOPort(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    vcpu_t *vcpu;
    uint16_t ਨੀਵਾਂ, ਉੱਚਾ;

    ਜੇ (ਲੰਮਾਈ < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਨੀਵਾਂ = getSyscallArg(0, ਬਫਰ);
    ਉੱਚਾ = getSyscallArg(1, ਬਫਰ);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ਵਾਪਸ invokeDisableIOPort(vcpu, ਨੀਵਾਂ, ਉੱਚਾ);
}

ਸਥਿਰ exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ, ਸ਼ਬਦ_t ਖੇਤਰ, ਸ਼ਬਦ_t ਮੁੱਲ)
{
    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);

    ਜੇ (ARCH_ਗੰਢ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ਚੋਣ (ਖੇਤਰ) {
    ਹਾਲਤ VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = ਮੁੱਲ;
        ਰੋਕੋ;
    ਹਾਲਤ VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = ਮੁੱਲ;
        ਰੋਕੋ;
    ਹਾਲਤ VMX_CONTROL_CR0_ਨਕਾਬ:
        vcpu->cr0_ਨਕਾਬ = vcpu->cached_cr0_ਨਕਾਬ = ਮੁੱਲ;
        ਰੋਕੋ;
    ਹਾਲਤ VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = ਮੁੱਲ;
        ਰੋਕੋ;
    }
    vmwrite(ਖੇਤਰ, ਮੁੱਲ);

    ਜੇ (call) {
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ = ਸੈੱਟ_MR(thread, ਬਫਰ, 0, ਮੁੱਲ);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeWriteVMCS(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t ਖੇਤਰ;
    ਸ਼ਬਦ_t ਮੁੱਲ;

    ਜੇ (ਲੰਮਾਈ < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਖੇਤਰ = getSyscallArg(0, ਬਫਰ);
    ਮੁੱਲ = getSyscallArg(1, ਬਫਰ);
    ਚੋਣ (ਖੇਤਰ) {
    ਹਾਲਤ VMX_GUEST_RIP:
    ਹਾਲਤ VMX_GUEST_RSP:
    ਹਾਲਤ VMX_GUEST_ES_SELECTOR:
    ਹਾਲਤ VMX_GUEST_CS_SELECTOR:
    ਹਾਲਤ VMX_GUEST_SS_SELECTOR:
    ਹਾਲਤ VMX_GUEST_DS_SELECTOR:
    ਹਾਲਤ VMX_GUEST_FS_SELECTOR:
    ਹਾਲਤ VMX_GUEST_GS_SELECTOR:
    ਹਾਲਤ VMX_GUEST_LDTR_SELECTOR:
    ਹਾਲਤ VMX_GUEST_TR_SELECTOR:
    ਹਾਲਤ VMX_GUEST_DEBUGCTRL:
    ਹਾਲਤ VMX_GUEST_PAT:
    ਹਾਲਤ VMX_GUEST_EFER:
    ਹਾਲਤ VMX_GUEST_PERF_GLOBAL_CTRL:
    ਹਾਲਤ VMX_GUEST_PDPTE0:
    ਹਾਲਤ VMX_GUEST_PDPTE1:
    ਹਾਲਤ VMX_GUEST_PDPTE2:
    ਹਾਲਤ VMX_GUEST_PDPTE3:
    ਹਾਲਤ VMX_GUEST_ES_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_CS_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_SS_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_DS_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_FS_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_GS_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_LDTR_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_TR_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_GDTR_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_IDTR_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_ES_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_CS_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_SS_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_DS_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_FS_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_GS_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_TR_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_INTERRUPTABILITY:
    ਹਾਲਤ VMX_GUEST_ACTIVITY:
    ਹਾਲਤ VMX_GUEST_SMBASE:
    ਹਾਲਤ VMX_GUEST_SYSENTER_CS:
    ਹਾਲਤ VMX_GUEST_PREEMPTION_TIMER_ਮੁੱਲ:
    ਹਾਲਤ VMX_GUEST_ES_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_CS_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_SS_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_DS_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_FS_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_GS_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_LDTR_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_TR_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_GDTR_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_IDTR_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_DR7:
    ਹਾਲਤ VMX_GUEST_RFLAGS:
    ਹਾਲਤ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    ਹਾਲਤ VMX_GUEST_SYSENTER_ESP:
    ਹਾਲਤ VMX_GUEST_SYSENTER_EIP:
    ਹਾਲਤ VMX_CONTROL_CR0_ਨਕਾਬ:
    ਹਾਲਤ VMX_CONTROL_CR4_ਨਕਾਬ:
    ਹਾਲਤ VMX_CONTROL_CR0_READ_SHADOW:
    ਹਾਲਤ VMX_CONTROL_CR4_READ_SHADOW:
    ਹਾਲਤ VMX_GUEST_CR3:
    ਹਾਲਤ VMX_CONTROL_EXCEPTION_BITMAP:
    ਹਾਲਤ VMX_CONTROL_ਇੰਦਰਾਜ_INTERRUPTION_INFO:
    ਹਾਲਤ VMX_CONTROL_ਇੰਦਰਾਜ_EXCEPTION_ERROR_ਕੋਡ:
        ਰੋਕੋ;
    ਹਾਲਤ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        ਮੁੱਲ = apply_Fixed_ਬਿੱਟ(ਮੁੱਲ, pin_control_ਉੱਚਾ, pin_control_ਨੀਵਾਂ);
        ਰੋਕੋ;
    ਹਾਲਤ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        ਮੁੱਲ = apply_Fixed_ਬਿੱਟ(ਮੁੱਲ, primary_control_ਉੱਚਾ, primary_control_ਨੀਵਾਂ);
        ਰੋਕੋ;
    ਹਾਲਤ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        ਮੁੱਲ = apply_Fixed_ਬਿੱਟ(ਮੁੱਲ, secondary_control_ਉੱਚਾ, secondary_control_ਨੀਵਾਂ);
        ਰੋਕੋ;
    ਹਾਲਤ VMX_CONTROL_ਇੰਦਰਾਜ_CONTROLS:
        ਮੁੱਲ = apply_Fixed_ਬਿੱਟ(ਮੁੱਲ, ਇੰਦਰਾਜ_control_ਉੱਚਾ, ਇੰਦਰਾਜ_control_ਨੀਵਾਂ);
        ਰੋਕੋ;
    ਹਾਲਤ VMX_CONTROL_EXIT_CONTROLS:
        ਮੁੱਲ = apply_Fixed_ਬਿੱਟ(ਮੁੱਲ, exit_control_ਉੱਚਾ, exit_control_ਨੀਵਾਂ);
        ਰੋਕੋ;
    ਹਾਲਤ VMX_GUEST_CR0:
        ਮੁੱਲ = apply_Fixed_ਬਿੱਟ(ਮੁੱਲ, cr0_ਉੱਚਾ, cr0_ਨੀਵਾਂ);
        ਰੋਕੋ;
    ਹਾਲਤ VMX_GUEST_CR4:
        ਮੁੱਲ = apply_Fixed_ਬਿੱਟ(ਮੁੱਲ, cr4_ਉੱਚਾ, cr4_ਨੀਵਾਂ);
        ਰੋਕੋ;
    ਮੂਲ:
        userError("VCPU WriteVMCS: Invalid field %lx.", (ਲੰਮਾ)ਖੇਤਰ);
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਵਾਪਸ invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, ਬਫਰ, ਖੇਤਰ, ਮੁੱਲ);
}

ਸਥਿਰ ਸ਼ਬਦ_t readVMCSField(vcpu_t *vcpu, ਸ਼ਬਦ_t ਖੇਤਰ)
{
    ਚੋਣ (ਖੇਤਰ) {
    ਹਾਲਤ VMX_CONTROL_EXCEPTION_BITMAP:
        ਵਾਪਸ vcpu->exception_bitmap;
    ਹਾਲਤ VMX_GUEST_CR0:
        ਵਾਪਸ vcpu->cr0;
    ਹਾਲਤ VMX_CONTROL_CR0_ਨਕਾਬ:
        ਵਾਪਸ vcpu->cr0_ਨਕਾਬ;
    ਹਾਲਤ VMX_CONTROL_CR0_READ_SHADOW:
        ਵਾਪਸ vcpu->cr0_shadow;
    }
    ਜੇ (ARCH_ਗੰਢ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ਵਾਪਸ vmread(ਖੇਤਰ);
}

ਸਥਿਰ exception_t invokeReadVMCS(vcpu_t *vcpu, ਸ਼ਬਦ_t ਖੇਤਰ, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);
    ਸ਼ਬਦ_t ਮੁੱਲ = readVMCSField(vcpu, ਖੇਤਰ);
    ਜੇ (call) {
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ = ਸੈੱਟ_MR(thread, ਬਫਰ, 0, ਮੁੱਲ);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeReadVMCS(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਜੇ (ਲੰਮਾਈ < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਸ਼ਬਦ_t ਖੇਤਰ = getSyscallArg(0, ਬਫਰ);
    ਚੋਣ (ਖੇਤਰ) {
    ਹਾਲਤ VMX_GUEST_RIP:
    ਹਾਲਤ VMX_GUEST_RSP:
    ਹਾਲਤ VMX_GUEST_ES_SELECTOR:
    ਹਾਲਤ VMX_GUEST_CS_SELECTOR:
    ਹਾਲਤ VMX_GUEST_SS_SELECTOR:
    ਹਾਲਤ VMX_GUEST_DS_SELECTOR:
    ਹਾਲਤ VMX_GUEST_FS_SELECTOR:
    ਹਾਲਤ VMX_GUEST_GS_SELECTOR:
    ਹਾਲਤ VMX_GUEST_LDTR_SELECTOR:
    ਹਾਲਤ VMX_GUEST_TR_SELECTOR:
    ਹਾਲਤ VMX_GUEST_DEBUGCTRL:
    ਹਾਲਤ VMX_GUEST_PAT:
    ਹਾਲਤ VMX_GUEST_EFER:
    ਹਾਲਤ VMX_GUEST_PERF_GLOBAL_CTRL:
    ਹਾਲਤ VMX_GUEST_PDPTE0:
    ਹਾਲਤ VMX_GUEST_PDPTE1:
    ਹਾਲਤ VMX_GUEST_PDPTE2:
    ਹਾਲਤ VMX_GUEST_PDPTE3:
    ਹਾਲਤ VMX_GUEST_ES_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_CS_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_SS_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_DS_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_FS_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_GS_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_LDTR_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_TR_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_GDTR_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_IDTR_ਹੱਦਬੰਦੀ:
    ਹਾਲਤ VMX_GUEST_ES_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_CS_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_SS_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_DS_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_FS_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_GS_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_LDTR_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_TR_ACCESS_RIGHTS:
    ਹਾਲਤ VMX_GUEST_INTERRUPTABILITY:
    ਹਾਲਤ VMX_GUEST_ACTIVITY:
    ਹਾਲਤ VMX_GUEST_SMBASE:
    ਹਾਲਤ VMX_GUEST_SYSENTER_CS:
    ਹਾਲਤ VMX_GUEST_PREEMPTION_TIMER_ਮੁੱਲ:
    ਹਾਲਤ VMX_GUEST_ES_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_CS_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_SS_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_DS_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_FS_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_GS_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_LDTR_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_TR_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_GDTR_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_IDTR_ਅਧਾਰ:
    ਹਾਲਤ VMX_GUEST_DR7:
    ਹਾਲਤ VMX_GUEST_RFLAGS:
    ਹਾਲਤ VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    ਹਾਲਤ VMX_GUEST_SYSENTER_ESP:
    ਹਾਲਤ VMX_GUEST_SYSENTER_EIP:
    ਹਾਲਤ VMX_CONTROL_CR0_ਨਕਾਬ:
    ਹਾਲਤ VMX_CONTROL_CR4_ਨਕਾਬ:
    ਹਾਲਤ VMX_CONTROL_CR0_READ_SHADOW:
    ਹਾਲਤ VMX_CONTROL_CR4_READ_SHADOW:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_INSTRUCTION_ERROR:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_EXIT_ਰੁਕਾਵਟ_INFO:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_EXIT_ਰੁਕਾਵਟ_ERROR:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_IDT_ਸਦਿਸ਼_INFO:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_IDT_ਸਦਿਸ਼_ERROR:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_EXIT_INSTRUCTION_ਲੰਮਾਈ:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_EXIT_INSTRUCTION_INFO:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_GUEST_PHYSICAL:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_IO_RCX:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_IO_RSI:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_IO_RDI:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_IO_RIP:
    ਹਾਲਤ VMX_ਅੰਕੜੇ_GUEST_ਰੇਖਿਕ_ਪਤਾ:
    ਹਾਲਤ VMX_CONTROL_ਇੰਦਰਾਜ_INTERRUPTION_INFO:
    ਹਾਲਤ VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    ਹਾਲਤ VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    ਹਾਲਤ VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    ਹਾਲਤ VMX_CONTROL_EXCEPTION_BITMAP:
    ਹਾਲਤ VMX_CONTROL_ਇੰਦਰਾਜ_CONTROLS:
    ਹਾਲਤ VMX_CONTROL_EXIT_CONTROLS:
    ਹਾਲਤ VMX_GUEST_CR0:
    ਹਾਲਤ VMX_GUEST_CR3:
    ਹਾਲਤ VMX_GUEST_CR4:
        ਰੋਕੋ;
    ਮੂਲ:
        userError("VCPU ReadVMCS: Invalid field %lx.", (ਲੰਮਾ)ਖੇਤਰ);
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਵਾਪਸ invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ਖੇਤਰ, call, ਬਫਰ);
}

ਸਥਿਰ exception_t invoke_ਸੈੱਟ_TCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decode_ਸੈੱਟ_TCB(cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    cap_t tcbCap;
    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਵਾਪਸ invoke_ਸੈੱਟ_TCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

ਖਾਲੀ vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    ਸ਼ਬਦ_t *ਬਫਰ;
    ਜੇ (ARCH_ਗੰਢ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ਬਫਰ = lookupIPCBuffer(false, ਗੰਢ_STATE(ksCurThread));
    ਜੇ (!ਬਫਰ) {
        userError("No IPC buffer.");
        ਵਾਪਸ;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, ਬਫਰ));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, apply_Fixed_ਬਿੱਟ(getSyscallArg(1, ਬਫਰ), primary_control_ਉੱਚਾ,
                                                                   primary_control_ਨੀਵਾਂ));
    vmwrite(VMX_CONTROL_ਇੰਦਰਾਜ_INTERRUPTION_INFO, getSyscallArg(2, ਬਫਰ));
}

ਖਾਲੀ vcpu_sysvmenter_ਜਵਾਬ_ਮੰਜ਼ਿਲ_user(tcb_t *tcb)
{
    ਸ਼ਬਦ_t *ਬਫਰ;
    vcpu_t *vcpu;

    ਬਫਰ = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    ਪ੍ਰਤਿਜ੍ਞਾ(vcpu);

    ਜੇ (ARCH_ਗੰਢ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    ਸੈੱਟ_MR(tcb, ਬਫਰ, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    ਸੈੱਟ_MR(tcb, ਬਫਰ, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    ਸੈੱਟ_MR(tcb, ਬਫਰ, SEL4_VMENTER_CALL_ਰੁਕਾਵਟ_INFO_MR,
          vmread(VMX_CONTROL_ਇੰਦਰਾਜ_INTERRUPTION_INFO));
    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, 0);
}

exception_t decodeX86VCPUInvocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *ਥਾਂ,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਚੋਣ (invLabel) {
    ਹਾਲਤ X86VCPUSetTCB:
        ਵਾਪਸ decode_ਸੈੱਟ_TCB(cap, ਲੰਮਾਈ, ਬਫਰ);
    ਹਾਲਤ X86VCPUReadVMCS:
        ਵਾਪਸ decodeReadVMCS(cap, ਲੰਮਾਈ, call, ਬਫਰ);
    ਹਾਲਤ X86VCPUWriteVMCS:
        ਵਾਪਸ decodeWriteVMCS(cap, ਲੰਮਾਈ, call, ਬਫਰ);
    ਹਾਲਤ X86VCPUEnableIOPort:
        ਵਾਪਸ decodeEnableIOPort(cap, ਲੰਮਾਈ, ਬਫਰ);
    ਹਾਲਤ X86VCPUDisableIOPort:
        ਵਾਪਸ decodeDisableIOPort(cap, ਲੰਮਾਈ, ਬਫਰ);
    ਹਾਲਤ X86VCPUWriteRegisters:
        ਵਾਪਸ decodeVCPUWriteRegisters(cap, ਲੰਮਾਈ, ਬਫਰ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
    ਹਾਲਤ X86VCPUWriteMSR:
        ਵਾਪਸ decodeVCPUWriteMSR(cap, ਲੰਮਾਈ, ਬਫਰ);
    ਹਾਲਤ X86VCPUReadMSR:
        ਵਾਪਸ decodeVCPUReadMSR(cap, ਲੰਮਾਈ, ਬਫਰ);
#ਜੇ_ਅੰਤ /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    ਮੂਲ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

ਸਥਿਰ bool_t is_vtx_supported(ਖਾਲੀ)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    ਵਾਪਸ !!(x86_cpuid_ecx(0x1, 0) & ਦੋਆਧਾਰੀ_ਅੰਕ(5));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ clear_ਦੋਆਧਾਰੀ_ਅੰਕ(ਸ਼ਬਦ_t *bitmap, ਸ਼ਬਦ_t ਦੋਆਧਾਰੀ_ਅੰਕ_2)
{
    ਪੂਰਨ ਸੂਚਕਾਂਕ = ਦੋਆਧਾਰੀ_ਅੰਕ_2 / (ਆਕਾਰ(ਸ਼ਬਦ_t) * 8);
    ਪੂਰਨ ਵਿੱਥ = ਦੋਆਧਾਰੀ_ਅੰਕ_2 % (ਆਕਾਰ(ਸ਼ਬਦ_t) * 8);
    bitmap[ਸੂਚਕਾਂਕ] &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(ਵਿੱਥ);
}

BOOT_ਕੋਡ bool_t vtx_init(ਖਾਲੀ)
{
    ਜੇ (!is_vtx_supported()) {
        ਲਿਖੋ("vt-x: not supported\n");
        ਵਾਪਸ false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.ਸ਼ਬਦ[0] = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_BASIC_MSR);
    vmx_basic.ਸ਼ਬਦ[1] = x86_rdmsr_ਉੱਚਾ(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.ਸ਼ਬਦ[0] = x86_rdmsr_ਨੀਵਾਂ(IA32_FEATURE_CONTROL_MSR);
    ਜੇ (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        ਜੇ (feature_control_msr_get_ਤਾਲਾ(feature_control)) {
            ਲਿਖੋ("vt-x: feature locked\n");
            ਵਾਪਸ false;
        }
        feature_control = feature_control_msr_ਸੈੱਟ_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ਉੱਚਾ(IA32_FEATURE_CONTROL_MSR), feature_control.ਸ਼ਬਦ[0]);
    }
    /* make sure the msr is locked */
    ਜੇ (!feature_control_msr_get_ਤਾਲਾ(feature_control)) {
        feature_control = feature_control_msr_ਸੈੱਟ_ਤਾਲਾ(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ਉੱਚਾ(IA32_FEATURE_CONTROL_MSR), feature_control.ਸ਼ਬਦ[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    ਜੇ (CURRENT_CPU_ਸੂਚਕਾਂਕ() == 0) {
        ਜੇ (!init_vtx_fixed_ਮੁੱਲਾਂ(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            ਲਿਖੋ("vt-x: lack of required features\n");
            ਵਾਪਸ false;
        }
    }
    ਜੇ (!check_vtx_fixed_ਮੁੱਲਾਂ(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        ਲਿਖੋ("vt-x: cores have inconsistent features\n");
        ਵਾਪਸ false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    ਬਣਤਰ vmxon_region *vmxon_kptr = &ARCH_ਗੰਢ_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    ਜੇ (!vtx_check_fixed_ਮੁੱਲਾਂ(read_cr0(), read_cr4())) {
        ਵਾਪਸ false;
    }
    ਜੇ (vmxon(kpptr_ਮੰਜ਼ਿਲ_paddr(vmxon_kptr))) {
        ਲਿਖੋ("vt-x: vmxon failure\n");
        ਵਾਪਸ false;
    }
    memset(&msr_bitmap_region, ~0, ਆਕਾਰ(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਨੀਵਾਂ_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਨੀਵਾਂ_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਨੀਵਾਂ_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਨੀਵਾਂ_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਨੀਵਾਂ_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਨੀਵਾਂ_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਉੱਚਾ_msr_read.bitmap, MSR_BITMAP_ਨਕਾਬ(IA32_FS_ਅਧਾਰ_MSR));
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਉੱਚਾ_msr_read.bitmap, MSR_BITMAP_ਨਕਾਬ(IA32_GS_ਅਧਾਰ_MSR));
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਉੱਚਾ_msr_read.bitmap, MSR_BITMAP_ਨਕਾਬ(IA32_KERNEL_GS_ਅਧਾਰ_MSR));
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਉੱਚਾ_msr_write.bitmap, MSR_BITMAP_ਨਕਾਬ(IA32_FS_ਅਧਾਰ_MSR));
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਉੱਚਾ_msr_write.bitmap, MSR_BITMAP_ਨਕਾਬ(IA32_GS_ਅਧਾਰ_MSR));
    clear_ਦੋਆਧਾਰੀ_ਅੰਕ(msr_bitmap_region.ਉੱਚਾ_msr_write.bitmap, MSR_BITMAP_ਨਕਾਬ(IA32_KERNEL_GS_ਅਧਾਰ_MSR));
#ਜੇ_ਅੰਤ
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.ਸ਼ਬਦ[0] = x86_rdmsr_ਨੀਵਾਂ(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.ਸ਼ਬਦ[1] = x86_rdmsr_ਉੱਚਾ(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    ਜੇ (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        ਲਿਖੋ("vt-x: Expected wb attribute for EPT paging structure\n");
        ਵਾਪਸ false;
    }
    ਜੇ (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        ਲਿਖੋ("vt-x: Expected supported for 2m pages\n");
        ਵਾਪਸ false;
    }

    ਵਾਪਸ true;
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_MRs_vmexit(uint32_t reason, ਸ਼ਬਦ_t qualification)
{
    ਸ਼ਬਦ_t *ਬਫਰ;
    ਪੂਰਨ i;

    ਬਫਰ = lookupIPCBuffer(true, ਗੰਢ_STATE(ksCurThread));

    ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_CALL_ਰੁਕਾਵਟ_INFO_MR,
          vmread(VMX_CONTROL_ਇੰਦਰਾਜ_INTERRUPTION_INFO));
    ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_FAULT_REASON_MR, reason);
    ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_ਅੰਕੜੇ_EXIT_INSTRUCTION_ਲੰਮਾਈ));
    ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_ਅੰਕੜੇ_GUEST_PHYSICAL));
    ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    ਲਈ (i = 0; i < n_vcpu_gp_ਰਜਿਸਟਰ; i++) {
        ਸੈੱਟ_MR(ਗੰਢ_STATE(ksCurThread), ਬਫਰ, SEL4_VMENTER_FAULT_EAX + i,
              ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

ਸਥਿਰ ਖਾਲੀ handleVmxFault(uint32_t reason, ਸ਼ਬਦ_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    ਸੈੱਟ_ਰਜਿਸਟਰ(ਗੰਢ_STATE(ksCurThread), ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, SEL4_VMENTER_ਨਤੀਜਾ_FAULT);

    ਸੈੱਟ_MRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਅੰਤ_Vmexit_Saving(ਖਾਲੀ)
{
    vcpu_t *vcpu = ARCH_ਗੰਢ_STATE(x86KSCurrentVCPU);
    ਪ੍ਰਤਿਜ੍ਞਾ(vcpu == ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(ਖਾਲੀ)
{
    uint32_t ਰੁਕਾਵਟ;
    /* qualification is host width, reason is defined as being 32 bit */
    ਸ਼ਬਦ_t qualification;
    uint32_t reason;
    ਅੰਤ_Vmexit_Saving();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_X86_64_VTX_64_ਦੋਆਧਾਰੀ_ਅੰਕ_GUESTS
    vcpu_restore_host_msrs();
#ਜੇ_ਅੰਤ
    /* the basic exit reason is the bottom 16 bits of the exit reason field */
    reason = vmread(VMX_ਅੰਕੜੇ_EXIT_REASON) & ਨਕਾਬ(16);
    ਜੇ (reason == EXTERNAL_ਰੁਕਾਵਟ) {
        ਗੰਢ_ਤਾਲਾ_IRQ;
        ਜੇ (vmx_feature_ack_on_exit) {
            ਰੁਕਾਵਟ = vmread(VMX_ਅੰਕੜੇ_EXIT_ਰੁਕਾਵਟ_INFO) & 0xff;
            ARCH_ਗੰਢ_STATE(x86_KScur_ਰੁਕਾਵਟ) = ਰੁਕਾਵਟ;
            handle_ਰੁਕਾਵਟ_ਇੰਦਰਾਜ();
        } ਨਹੀਂਤਾਂ {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(ਗੰਢ_STATE(ksCurThread));
#ਜੇ_ਅੰਤ
        ਵਾਪਸ EXCEPTION_NONE;
    }

    ਗੰਢ_ਤਾਲਾ_SYS;

    ਚੋਣ (reason) {
    ਹਾਲਤ EXCEPTION_OR_NMI:
    ਹਾਲਤ MOV_DR:
    ਹਾਲਤ TASK_SWITCH:
    ਹਾਲਤ CONTROL_ਰਜਿਸਟਰ:
    ਹਾਲਤ IO:
    ਹਾਲਤ MWAIT:
    ਹਾਲਤ SIPI:
    ਹਾਲਤ INVLPG:
    ਹਾਲਤ INVEPT:
    ਹਾਲਤ INVVPID:
    ਹਾਲਤ VMCLEAR:
    ਹਾਲਤ VMPTRLD:
    ਹਾਲਤ VMPTRST:
    ਹਾਲਤ VMREAD:
    ਹਾਲਤ VMWRITE:
    ਹਾਲਤ VMXON:
    ਹਾਲਤ EPT_VIOLATION:
    ਹਾਲਤ GDTR_OR_IDTR:
    ਹਾਲਤ LDTR_OR_TR:
    ਹਾਲਤ TPR_ਹੇਠਾਂ_ਹੱਦ:
    ਹਾਲਤ APIC_ACCESS:
        qualification = vmread(VMX_ਅੰਕੜੇ_EXIT_QUALIFICATION);
        ਰੋਕੋ;
    ਮੂਲ:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t handle_Vm_ਇੰਦਰਾਜ_Fail(ਖਾਲੀ)
{
    handleVmxFault(-1, -1);

    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
ਖਾਲੀ VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    ਪ੍ਰਤਿਜ੍ਞਾ(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    ਜੇ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_ਸਰਗਰਮ) {

        ਸ਼ਬਦ_t badge = notification_ptr_get_ntfn_ਸੁਨੇਹਾ_Identifier(ntfnPtr);
        notification_ptr_ਸੈੱਟ_state(ntfnPtr, NtfnState_Idle);
        ਸੈੱਟ_Thread_State(tcb, ThreadState_Running);
        ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, badge_ਰਜਿਸਟਰ, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        ਜੇ (tcb != ਗੰਢ_STATE(ksCurThread)) {
            possible_Switch_ਮੰਜ਼ਿਲ(tcb);
        }
    }
}
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

ਸਥਿਰ ਖਾਲੀ invvpid_context(uint16_t vpid)
{
    ਬਣਤਰ {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t ਪਤਾ;
    } PACKED ਸੰਕਾਰਜ = {vpid, 0, 0};
    asm ਅਸਥਿਰ("invvpid %0, %1" :: "m"(ਸੰਕਾਰਜ), "r"((ਸ਼ਬਦ_t)1) : "cc");
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_EPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_ਜੜ੍ਹ;
    ਜੇ (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_ਜੜ੍ਹ = kpptr_ਮੰਜ਼ਿਲ_paddr(null_ept_space);
    } ਨਹੀਂਤਾਂ {
        ਲੱਭੋ_EPTFor_ASID_ret_t ਲੱਭੋ_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(vmxSpace);
        ਲੱਭੋ_ret = ਲੱਭੋ_EPTFor_ASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE || ਲੱਭੋ_ret.ept != pml4) {
            ept_ਜੜ੍ਹ = kpptr_ਮੰਜ਼ਿਲ_paddr(null_ept_space);
        } ਨਹੀਂਤਾਂ {
            ept_ਜੜ੍ਹ = pptr_ਮੰਜ਼ਿਲ_paddr(pml4);
        }
    }
    ਜੇ (ept_ਜੜ੍ਹ != vcpu->ਆਖਰੀ_ept_ਜੜ੍ਹ) {
        vcpu->ਆਖਰੀ_ept_ਜੜ੍ਹ = ept_ਜੜ੍ਹ;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_ਜੜ੍ਹ,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_ਸੂਚਕ, eptp.ਸ਼ਬਦ[0]);
        ਪ੍ਰਤਿਜ੍ਞਾ(vcpu->vpid != VPID_INVALID);
        ਜੇ (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

ਖਾਲੀ clearVPIDIOPortMappings(vpid_t vpid, uint16_t ਪਹਿਲਾ, uint16_t ਆਖਰੀ)
{
    ਜੇ (vpid == VPID_INVALID) {
        ਵਾਪਸ;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    ਜੇ (vcpu == ਨਿਰਾ) {
        ਵਾਪਸ;
    }
    ਪ੍ਰਤਿਜ੍ਞਾ(vcpu->vpid == vpid);
    ਸੈੱਟ_IOPort_ਨਕਾਬ(vcpu->io, ਪਹਿਲਾ, ਆਖਰੀ, true);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ vpid_t ਅਗਲਾ_VPID(vpid_t vpid)
{
    ਜੇ (vpid == VPID_ਆਖਰੀ) {
        ਵਾਪਸ VPID_ਪਹਿਲਾ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ vpid + 1;
    }
}

ਸਥਿਰ ਖਾਲੀ invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, ਆਕਾਰ(vcpu->io));
    /* invalidate the VPID context */
    ਜੇ (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
ਸਥਿਰ ਖਾਲੀ releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = ਨਿਰਾ;
}

ਸਥਿਰ vpid_t ਲੱਭੋ_Free_VPID(ਖਾਲੀ)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    ਕਰੋ {
        ਜੇ (x86KSVPIDTable[vpid] == ਨਿਰਾ) {
            ਵਾਪਸ vpid;
        }
        vpid = ਅਗਲਾ_VPID(vpid);
    } ਜਦੋਂ (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = ਅਗਲਾ_VPID(x86KSNextVPID);
    ਵਾਪਸ vpid;
}

ਸਥਿਰ ਖਾਲੀ storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(x86KSVPIDTable[vpid] == ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(vcpu->vpid == VPID_INVALID);
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
ਖਾਲੀ vcpu_fpu_ਮੰਜ਼ਿਲ_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    ਜੇ (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        ਜੇ (!vcpu->fpu_ਸਰਗਰਮ) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_ਸਰਗਰਮ = true;
            ਲੱਦੋ_Fpu_State(tcb);
        }
    } ਨਹੀਂਤਾਂ {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_ਸਰਗਰਮ = true;
        switchLocalFpuOwner(tcb);
    }
}

ਖਾਲੀ vcpu_fpu_ਮੰਜ਼ਿਲ_host(tcb_t *tcb, vcpu_t *vcpu)
{
    ਜੇ (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } ਨਹੀਂਤਾਂ ਜੇ (nativeThreadUsingFPU(tcb) && vcpu->fpu_ਸਰਗਰਮ) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_ਸਰਗਰਮ = false;
        ਲੱਦੋ_Fpu_State(tcb);
    } ਨਹੀਂਤਾਂ ਜੇ (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

ਖਾਲੀ restoreVMCS(ਖਾਲੀ)
{
    tcb_t *cur_thread = ਗੰਢ_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    ਜੇ (ARCH_ਗੰਢ_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
    ਜੇ (getCurrentCR3().ਸ਼ਬਦ[0] != expected_vmcs->ਆਖਰੀ_host_cr3) {
        expected_vmcs->ਆਖਰੀ_host_cr3 = getCurrentCR3().ਸ਼ਬਦ[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().ਸ਼ਬਦ[0]);
    }
#ਜੇ_ਅੰਤ
    ਜੇ (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = ਲੱਭੋ_Free_VPID();
        storeVPID(expected_vmcs, vpid);
        ਜੇ (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    ਸੈੱਟ_EPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

ਖਾਲੀ invept(ept_pml4e_t *ept_pml4)
{
    ਜੇ (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        ਬਣਤਰ {
            uint64_t parts[2];
        } ਪਤਾ;
        ਸ਼ਬਦ_t type;
        ਜੇ (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } ਨਹੀਂਤਾਂ ਜੇ (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } ਨਹੀਂਤਾਂ {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            ਵਾਪਸ;
        }

        ਪਤਾ.parts[0] = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)ept_pml4);
        ਪਤਾ.parts[1] = 0;
        asm ਅਸਥਿਰ(
            "invept %0, %1"
            :
            : "m"(ਪਤਾ),  "r"(type)
            : "memory"
        );
    }
}

#ਜੇ_ਅੰਤ
