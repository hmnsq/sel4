/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_VTX

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

#تعریف_کړه VMX_EXIT_QUAL_TYPE_MOV_CR 0
#تعریف_کړه VMX_EXIT_QUAL_TYPE_CLTS 2
#تعریف_کړه VMX_EXIT_QUAL_TYPE_LMSW 3

ثابت vcpu_gp_ثبتوونکی_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#که_تعریف_شوی CONFIG_X86_64_VTX_64_بټ_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#د_که_پای /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#تعریف_کړه MSR_BITMAP_ماسک(x) ((x) & 0x1fff)

تعريفنوع بنية msr_bitmap {
    کلمه_t bitmap[0x2000 / حجم(کلمه_t) / 8];
} msr_bitmap_t;

تعريفنوع بنية msr_bitmaps {
    msr_bitmap_t ټیټ_msr_read;
    msr_bitmap_t لوړ_msr_read;
    msr_bitmap_t ټیټ_msr_write;
    msr_bitmap_t لوړ_msr_write;
} msr_bitmaps_t;

ساكن msr_bitmaps_t msr_bitmap_region ALIGN(بټ(se_L4_پاڼه_بټونه));

ساكن حرف null_ept_space[بټ(se_L4_پاڼه_بټونه)] ALIGN(بټ(se_L4_پاڼه_بټونه));

/* Cached value of the hardware defined vmcs revision */
ساكن uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
ساكن vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
ساكن uint32_t pin_control_لوړ;
ساكن uint32_t pin_control_ټیټ;
ساكن uint32_t primary_control_لوړ;
ساكن uint32_t primary_control_ټیټ;
ساكن uint32_t secondary_control_لوړ;
ساكن uint32_t secondary_control_ټیټ;
ساكن uint32_t ثبت_control_لوړ;
ساكن uint32_t ثبت_control_ټیټ;
ساكن uint32_t exit_control_لوړ;
ساكن uint32_t exit_control_ټیټ;
ساكن uint32_t cr0_لوړ;
ساكن uint32_t cr0_ټیټ;
ساكن uint32_t cr4_لوړ;
ساكن uint32_t cr4_ټیټ;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
ساكن bool_t vmx_feature_vpid;
ساكن bool_t vmx_feature_بارول_perf_global_ctrl;
ساكن bool_t vmx_feature_ack_on_exit;

ساكن vcpu_t *x86KSVPIDTable[VPID_وروستی + 1];
ساكن vpid_t x86KSNextVPID = VPID_لومړی;

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
    uint64_t physical_پته;
    physical_پته = pptr_منزل_paddr((فراغ *)vmcs_ptr);
    asm متطاير(
        "vmclear %0"
        :
        : "m"(physical_پته)
        : "cc"
    );
}

فراغ clearCurrentVCPU(فراغ)
{
    vcpu_t *vcpu = ARCH_غوټه_STATE(x86KSCurrentVCPU);
    إذا (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_غوټه_STATE(x86KSCurrentVCPU) = عدم;
        إذا (vcpu->fpu_فعال && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_فعال = false;
        }
    }
}

ساكن فراغ vmptrld(فراغ *vmcs_ptr)
{
    uint64_t physical_پته;
    uint8_t error;
    physical_پته = pptr_منزل_paddr(vmcs_ptr);
    asm متطاير(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_پته)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    أكد(!error);
}

#که_تعریف_شوی CONFIG_X86_64_VTX_64_بټ_GUESTS
فراغ vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_ستوری_MSR, vcpu->syscall_registers[VCPU_ستوری]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_ماسک]);
}

فراغ vcpu_restore_host_msrs(فراغ)
{
    init_syscall_msrs();
}
#د_که_پای

ساكن فراغ switchVCPU(vcpu_t *vcpu)
{
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    إذا (vcpu->وروستی_cpu != getCurrentCPUIndex() && ARCH_غوټه_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->وروستی_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->وروستی_cpu);
    }
#د_که_پای
    clearCurrentVCPU();
    vmptrld(vcpu);
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    إذا (vcpu->وروستی_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_بنسټ, (کلمه_t)&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss);
        vmwrite(VMX_HOST_GDTR_بنسټ, (کلمه_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_بنسټ, (کلمه_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(کلمه_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss.tss.ټکي[0] +
                                                          4));
    }
    vcpu->وروستی_cpu = getCurrentCPUIndex();
#د_که_پای
    ARCH_غوټه_STATE(x86KSCurrentVCPU) = vcpu;
}

ساكن فراغ print_بټونه(کلمه_t بټونه)
{
    bool_t لومړی = true;
    طالما (بټونه) {
        صحيح شاخص = se_L4_کلمه_بټونه - 1 - clzl(بټونه);
        إذا (لومړی) {
            چاپ_کړه("%d", شاخص);
            لومړی = false;
        } وإلا {
            چاپ_کړه(",%d", شاخص);
        }
        بټونه &= ~بټ(شاخص);
    }
}

ساكن bool_t check_fixed_ارزښت(کلمه_t val, کلمه_t ټیټ, کلمه_t لوړ)
{
    کلمه_t not_لوړ;
    کلمه_t not_ټیټ;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_لوړ = لوړ & ~(لوړ & val);
    إذا (not_لوړ != 0) {
        چاپ_کړه("Failed to set bits: ");
        print_بټونه(not_لوړ);
        أرجع false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_ټیټ = ~ټیټ & ~(~ټیټ & ~val);
    إذا (not_ټیټ != 0) {
        چاپ_کړه("Incorrectly cleared bits: ");
        print_بټونه(not_ټیټ);
        أرجع false;
    }
    أرجع true;
}

ساكن bool_t vtx_check_fixed_ارزښتونه(کلمه_t cr0, کلمه_t cr4)
{
    إذا (!check_fixed_ارزښت(cr0, cr0_ټیټ, cr0_لوړ)) {
        چاپ_کړه(" of CR0\n");
        أرجع false;
    }
    إذا (!check_fixed_ارزښت(cr4, cr4_ټیټ, cr4_لوړ)) {
        چاپ_کړه(" of CR4\n");
        أرجع false;
    }
    أرجع true;
}

ساكن bool_t BOOT_کوډ init_vtx_fixed_ارزښتونه(bool_t useTrueMsrs)
{
    uint32_t pin_control_ماسک =
        بټ(0) |    //Extern interrupt exiting
        بټ(3) |    //NMI exiting
        بټ(5);     //virtual NMIs
    uint32_t primary_control_ماسک =
        بټ(25) |   //Use I/O bitmaps
        بټ(28) |   //Use MSR bitmaps
        بټ(31);    //Activate secondary controls
    uint32_t secondary_control_ماسک =
        بټ(1);     //Enable EPT
    uint32_t exit_control_ماسک =
        بټ(2)  |   //Save debug controls
        بټ(18) |   //Save guest IA32_PAT on exit
        بټ(19) |   //Load host IA32_PAT
        بټ(20) |   //Save guest IA32_EFER on exit
        بټ(21);    //Load host IA32_EFER
#که_تعریف_شوی CONFIG_ARCH_X86_64
    exit_control_ماسک |= بټ(9); //Host address-space size
#د_که_پای /* CONFIG_ARCH_X86_64 */
    /* Read out the fixed high and low bits from the MSRs */
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t ثبت_ctls;
    إذا (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        ثبت_ctls = IA32_VMX_TRUE_ثبت_CTLS_MSR;
    } وإلا {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        ثبت_ctls = IA32_VMX_ثبت_CTLS_MSR;
    }
    pin_control_لوړ = x86_rdmsr_ټیټ(pinbased_ctls);
    pin_control_ټیټ = x86_rdmsr_لوړ(pinbased_ctls);
    primary_control_لوړ = x86_rdmsr_ټیټ(procbased_ctls);
    primary_control_ټیټ = x86_rdmsr_لوړ(procbased_ctls);
    secondary_control_لوړ = x86_rdmsr_ټیټ(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_ټیټ = x86_rdmsr_لوړ(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_لوړ = x86_rdmsr_ټیټ(exit_ctls);
    exit_control_ټیټ = x86_rdmsr_لوړ(exit_ctls);
    ثبت_control_لوړ = x86_rdmsr_ټیټ(ثبت_ctls);
    ثبت_control_ټیټ = x86_rdmsr_لوړ(ثبت_ctls);

    cr0_لوړ = x86_rdmsr_ټیټ(IA32_VMX_CR0_FIXED0_MSR);
    cr0_ټیټ = x86_rdmsr_ټیټ(IA32_VMX_CR0_FIXED1_MSR);
    cr4_لوړ = x86_rdmsr_ټیټ(IA32_VMX_CR4_FIXED0_MSR);
    cr4_ټیټ = x86_rdmsr_ټیټ(IA32_VMX_CR4_FIXED1_MSR);

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
    إذا (secondary_control_ټیټ & بټ(7)) {
        cr0_لوړ &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    إذا (!(secondary_control_ټیټ & بټ(5))) {
        vmx_feature_vpid = 0;
        چاپ_کړه("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } وإلا {
        vmx_feature_vpid = 1;
        secondary_control_ماسک |= بټ(5);
    }

    /* Check for load perf global control */
    إذا (!(exit_control_ټیټ & بټ(12))) {
        vmx_feature_بارول_perf_global_ctrl = 0;
        چاپ_کړه("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } وإلا {
        vmx_feature_بارول_perf_global_ctrl = 1;
        exit_control_ماسک |= بټ(12);
    }

    /* Check for external interrupt exiting */
    إذا (!(exit_control_ټیټ & بټ(15))) {
        vmx_feature_ack_on_exit = 0;
        چاپ_کړه("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } وإلا {
        vmx_feature_ack_on_exit = 1;
        exit_control_ماسک |= بټ(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_ټیټ) & pin_control_ماسک;
    إذا (missing) {
        چاپ_کړه("vt-x: Unsupported pin control features %lx\n", (اوږد)missing);
        أرجع false;
    }
    missing = (~primary_control_ټیټ) & primary_control_ماسک;
    إذا (missing) {
        چاپ_کړه("vt-x: Unsupported primary control features %lx\n", (اوږد)missing);
        أرجع false;
    }
    missing = (~secondary_control_ټیټ) & secondary_control_ماسک;
    إذا (missing) {
        چاپ_کړه("vt-x: Unsupported secondary control features %lx\n", (اوږد)missing);
        أرجع false;
    }
    missing = (~exit_control_ټیټ) & exit_control_ماسک;
    إذا (missing) {
        چاپ_کړه("vt-x: Unsupported exit control features %lx\n", (اوږد)missing);
        أرجع false;
    }
#که_تعریف_شوی CONFIG_X86_64_VTX_64_بټ_GUESTS
    missing = (~ثبت_control_ټیټ) & بټ(9);
    إذا (missing) {
        چاپ_کړه("vt-x: CPU does not support 64-bit guests\n");
        أرجع false;
    }
#د_که_پای /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_لوړ |= pin_control_ماسک;
    primary_control_لوړ |= primary_control_ماسک;
    secondary_control_لوړ |= secondary_control_ماسک;
    exit_control_لوړ |= exit_control_ماسک;

    أرجع true;
}

ساكن bool_t BOOT_کوډ check_vtx_fixed_ارزښتونه(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t ثبت_ctls;
    إذا (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        ثبت_ctls = IA32_VMX_TRUE_ثبت_CTLS_MSR;
    } وإلا {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        ثبت_ctls = IA32_VMX_ثبت_CTLS_MSR;
    }
    uint32_t local_pin_control_لوړ = x86_rdmsr_ټیټ(pinbased_ctls);
    uint32_t local_pin_control_ټیټ = x86_rdmsr_لوړ(pinbased_ctls);
    uint32_t local_primary_control_لوړ = x86_rdmsr_ټیټ(procbased_ctls);
    uint32_t local_primary_control_ټیټ = x86_rdmsr_لوړ(procbased_ctls);
    uint32_t local_secondary_control_لوړ = x86_rdmsr_ټیټ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_ټیټ = x86_rdmsr_لوړ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_لوړ = x86_rdmsr_ټیټ(exit_ctls);
    uint32_t local_exit_control_ټیټ = x86_rdmsr_لوړ(exit_ctls);
    uint32_t local_ثبت_control_لوړ = x86_rdmsr_ټیټ(ثبت_ctls);
    uint32_t local_ثبت_control_ټیټ = x86_rdmsr_لوړ(ثبت_ctls);

    uint32_t local_cr0_لوړ = x86_rdmsr_ټیټ(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_ټیټ = x86_rdmsr_ټیټ(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_لوړ = x86_rdmsr_ټیټ(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_ټیټ = x86_rdmsr_ټیټ(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    إذا (local_secondary_control_ټیټ & بټ(7)) {
        local_cr0_لوړ &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    أرجع
        (local_pin_control_لوړ & pin_control_لوړ) == local_pin_control_لوړ &&
        (~local_pin_control_ټیټ & pin_control_لوړ) == 0 &&
        (local_primary_control_لوړ & primary_control_لوړ) == local_primary_control_لوړ &&
        (~local_primary_control_ټیټ & primary_control_لوړ) == 0 &&
        (local_secondary_control_لوړ & secondary_control_لوړ) == local_secondary_control_لوړ &&
        (~local_secondary_control_ټیټ & secondary_control_لوړ) == 0 &&
        (local_exit_control_لوړ & exit_control_لوړ) == local_exit_control_لوړ &&
        (~local_exit_control_ټیټ & exit_control_لوړ) == 0 &&
        (local_ثبت_control_لوړ & ثبت_control_لوړ) == local_ثبت_control_لوړ &&
        (~local_ثبت_control_ټیټ & ثبت_control_لوړ) == 0 &&
        local_cr0_لوړ == cr0_لوړ &&
        local_cr0_ټیټ == cr0_ټیټ &&
        local_cr4_لوړ == cr4_لوړ &&
        local_cr4_ټیټ == cr4_ټیټ;
}

ساكن مضمن uint32_t apply_Fixed_بټونه(uint32_t original, uint32_t لوړ, uint32_t ټیټ)
{
    original |= لوړ;
    original &= ټیټ;
    أرجع original;
}

فراغ vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = عدم;
    vcpu->launched = false;
    vcpu->fpu_فعال = false;
    vcpu->fpuState = x86KSnullFpuState;

    حافظه_نقل(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_لوړ & cr0_ټیټ;
    vcpu->cr0_shadow = 0;
    vcpu->cr0_ماسک = 0;
    vcpu->exception_bitmap = 0;
    vcpu->vpid = VPID_INVALID;
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    vcpu->وروستی_cpu = getCurrentCPUIndex();
#د_که_پای /* ENABLE_SMP_SUPPORT */

    switchVCPU(vcpu);

    vmwrite(VMX_HOST_PAT, x86_rdmsr(IA32_PAT_MSR));
    vmwrite(VMX_HOST_EFER, x86_rdmsr(IA32_EFER_MSR));
    // By default we will disable performance counters when we come out
    // of a VM. When performance counters are supported this host state
    // needs to be updated on VM entry
    إذا (vmx_feature_بارول_perf_global_ctrl) {
        vmwrite(VMX_HOST_PERF_GLOBAL_CTRL, 0);
    }
    vmwrite(VMX_HOST_CR0, read_cr0());
    vmwrite(VMX_HOST_CR4, read_cr4());
    vmwrite(VMX_HOST_FS_بنسټ, 0);
    vmwrite(VMX_HOST_GS_بنسټ, 0);
    vmwrite(VMX_HOST_TR_بنسټ, (کلمه_t)&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss);
    vmwrite(VMX_HOST_GDTR_بنسټ, (کلمه_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSgdt);
    vmwrite(VMX_HOST_IDTR_بنسټ, (کلمه_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSidt);
    vmwrite(VMX_HOST_SYSENTER_CS, (کلمه_t)SEL_CS_0);
    vmwrite(VMX_HOST_SYSENTER_EIP, (کلمه_t)&handle_syscall);
    إذا (!config_سېټ(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(کلمه_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss.tss.ټکي[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (کلمه_t)&vcpu->gp_registers[n_vcpu_gp_ثبتوونکی]);
    vmwrite(VMX_HOST_RIP, (کلمه_t)&handle_vmexit);
#که_تعریف_شوی CONFIG_KERNEL_SKIM_کړکۍ
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_منزل_paddr(x64KSKernelPML4), 0).ټکي[0]);
#د_که_پای /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_لوړ & pin_control_ټیټ);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_لوړ & primary_control_ټیټ);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_لوړ & secondary_control_ټیټ);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_لوړ & exit_control_ټیټ);

    uint64_t ثبت_control = ثبت_control_لوړ & ثبت_control_ټیټ;
#که_تعریف_شوی CONFIG_X86_64_VTX_64_بټ_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    ثبت_control |= بټ(9);
#د_که_پای /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ثبت_CONTROLS, ثبت_control);

    vmwrite(VMX_CONTROL_MSR_پته, (کلمه_t)kpptr_منزل_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_لوړ & cr4_ټیټ);

    vmwrite(VMX_GUEST_VMCS_LINK_اشاره_ګر, ~(کلمه_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_اشاره_ګر_لوړ, ~(کلمه_t)0);

    memset(vcpu->io, ~(کلمه_t)0, VCPU_IOBITMAP_اندازه);
    vmwrite(VMX_CONTROL_IOA_پته, pptr_منزل_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_پته, pptr_منزل_paddr((حرف *)vcpu->io + (VCPU_IOBITMAP_اندازه / 2)));
}

ساكن فراغ dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    أكد(tcb->tcbArch.tcbVCPU == vcpu);
    أكد(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_فعال = false;
    tcb->tcbArch.tcbVCPU = عدم;
    vcpu->vcpuTCB = عدم;
}

فراغ vcpu_finalise(vcpu_t *vcpu)
{
    إذا (ARCH_غوټه_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->وروستی_cpu) == vcpu) {
#که_تعریف_شوی ENABLE_SMP_SUPPORT
        إذا (vcpu->وروستی_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->وروستی_cpu);
        } وإلا
#د_که_پای /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    إذا (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    إذا (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

ساكن فراغ associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    إذا (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    إذا (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

ساكن exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, کلمه_t *بفر)
{
    صحيح i;
    لكل (i = 0; i < n_vcpu_gp_ثبتوونکی; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, بفر);
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUWriteRegisters(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
#که_تعریف_شوی CONFIG_X86_64_VTX_64_بټ_GUESTS
    إذا (اوږدتیا < 15) {
#وإلا
    إذا (اوږدتیا < 7) {
#د_که_پای /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), بفر);
}

#که_تعریف_شوی CONFIG_X86_64_VTX_64_بټ_GUESTS
ساكن exception_t invokeWriteMSR(vcpu_t *vcpu, کلمه_t *بفر, کلمه_t ډګر, کلمه_t ارزښت)
{
    tcb_t *thread;
    thread = غوټه_STATE(ksCurThread);
    إذا (ARCH_غوټه_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بدل (ډګر) {
    حالة IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = ارزښت;
        اكسر;
    حالة IA32_ستوری_MSR:
        vcpu->syscall_registers[VCPU_ستوری] = ارزښت;
        اكسر;
    حالة IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = ارزښت;
        اكسر;
    حالة IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_ماسک] = ارزښت;
        اكسر;
    }
    سېټ_MR(thread, بفر, 0, ارزښت);
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUWriteMSR(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    کلمه_t ډګر;
    کلمه_t ارزښت;

    إذا (اوږدتیا < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ډګر = getSyscallArg(0, بفر);
    ارزښت = getSyscallArg(1, بفر);
    بدل (ډګر) {
    حالة IA32_LSTAR_MSR:
    حالة IA32_ستوری_MSR:
    حالة IA32_CSTAR_MSR:
    حالة IA32_FMASK_MSR:
        اكسر;
    افتراضي:
        userError("VCPU WriteMSR: Invalid field %lx.", (اوږد)ډګر);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), بفر, ډګر, ارزښت);
}

ساكن exception_t invokeReadMSR(vcpu_t *vcpu, کلمه_t ډګر, کلمه_t *بفر)
{
    tcb_t *thread;
    thread = غوټه_STATE(ksCurThread);

    کلمه_t ارزښت = 0;

    بدل (ډګر) {
    حالة IA32_LSTAR_MSR:
        ارزښت = vcpu->syscall_registers[VCPU_LSTAR];
        اكسر;
    حالة IA32_ستوری_MSR:
        ارزښت = vcpu->syscall_registers[VCPU_ستوری];
        اكسر;
    حالة IA32_CSTAR_MSR:
        ارزښت = vcpu->syscall_registers[VCPU_CSTAR];
        اكسر;
    حالة IA32_FMASK_MSR:
        ارزښت = vcpu->syscall_registers[VCPU_SYSCALL_ماسک];
        اكسر;
    حالة IA32_VMX_MISC_MSR:
        ارزښت = x86_rdmsr(ډګر);
        اكسر;
    }

    سېټ_MR(thread, بفر, 0, ارزښت);
    سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                    se_L4_پیغام_Info_new(0, 0, 0, 1)));
    سېټ_Thread_State(thread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUReadMSR(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    إذا (اوږدتیا < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    کلمه_t ډګر = getSyscallArg(0, بفر);
    بدل (ډګر) {
    حالة IA32_LSTAR_MSR:
    حالة IA32_ستوری_MSR:
    حالة IA32_CSTAR_MSR:
    حالة IA32_FMASK_MSR:
    حالة IA32_VMX_MISC_MSR:
        اكسر;
    افتراضي:
        userError("VCPU ReadMSR: Invalid field %lx.", (اوږد)ډګر);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ډګر, بفر);
}
#د_که_پای  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

ساكن exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *خونه, cap_t cap, uint16_t ټیټ, uint16_t لوړ)
{
    /* remove any existing io ports from this cap */
    clearVPIDIOPortMappings(cap_io_پورټ_cap_get_cap_IOPort_VPID(cap),
                            cap_io_پورټ_cap_get_cap_IOPort_لومړی_پورټ(cap),
                            cap_io_پورټ_cap_get_cap_IOPort_وروستی_پورټ(cap));
    /* update the assigned vpid. If the vcpu does not have a valid vpid then
     * this is fine as whilst the cap will not point to the vcpu, the vcpu
     * will have its port mask cleared when it gets assigned a vpid */
    cap = cap_io_پورټ_cap_سېټ_cap_IOPort_VPID(cap, vcpu->vpid);
    خونه->cap = cap;
    سېټ_IOPort_ماسک(vcpu->io, ټیټ, لوړ, false);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeEnableIOPort(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *io_خونه;
    uint16_t ټیټ, لوړ;

    إذا (اوږدتیا < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    io_خونه = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(ioCap) != cap_io_پورټ_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ټیټ = getSyscallArg(0, بفر);
    لوړ = getSyscallArg(1, بفر);

    إذا (ټیټ < cap_io_پورټ_cap_get_cap_IOPort_لومړی_پورټ(ioCap) || لوړ > cap_io_پورټ_cap_get_cap_IOPort_وروستی_پورټ(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeEnableIOPort(vcpu, io_خونه, ioCap, ټیټ, لوړ);
}

ساكن exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t ټیټ, uint16_t لوړ)
{
    سېټ_IOPort_ماسک(vcpu->io, ټیټ, لوړ, true);
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeDisableIOPort(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    vcpu_t *vcpu;
    uint16_t ټیټ, لوړ;

    إذا (اوږدتیا < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ټیټ = getSyscallArg(0, بفر);
    لوړ = getSyscallArg(1, بفر);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    أرجع invokeDisableIOPort(vcpu, ټیټ, لوړ);
}

ساكن exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, کلمه_t *بفر, کلمه_t ډګر, کلمه_t ارزښت)
{
    tcb_t *thread;
    thread = غوټه_STATE(ksCurThread);

    إذا (ARCH_غوټه_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بدل (ډګر) {
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = ارزښت;
        اكسر;
    حالة VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = ارزښت;
        اكسر;
    حالة VMX_CONTROL_CR0_ماسک:
        vcpu->cr0_ماسک = vcpu->cached_cr0_ماسک = ارزښت;
        اكسر;
    حالة VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = ارزښت;
        اكسر;
    }
    vmwrite(ډګر, ارزښت);

    إذا (call) {
        سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
        بینښه صحيح اوږدتیا = سېټ_MR(thread, بفر, 0, ارزښت);
        سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, اوږدتیا)));
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeWriteVMCS(cap_t cap, کلمه_t اوږدتیا, bool_t call, کلمه_t *بفر)
{
    کلمه_t ډګر;
    کلمه_t ارزښت;

    إذا (اوږدتیا < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ډګر = getSyscallArg(0, بفر);
    ارزښت = getSyscallArg(1, بفر);
    بدل (ډګر) {
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
    حالة VMX_GUEST_ES_حد:
    حالة VMX_GUEST_CS_حد:
    حالة VMX_GUEST_SS_حد:
    حالة VMX_GUEST_DS_حد:
    حالة VMX_GUEST_FS_حد:
    حالة VMX_GUEST_GS_حد:
    حالة VMX_GUEST_LDTR_حد:
    حالة VMX_GUEST_TR_حد:
    حالة VMX_GUEST_GDTR_حد:
    حالة VMX_GUEST_IDTR_حد:
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
    حالة VMX_GUEST_PREEMPTION_TIMER_ارزښت:
    حالة VMX_GUEST_ES_بنسټ:
    حالة VMX_GUEST_CS_بنسټ:
    حالة VMX_GUEST_SS_بنسټ:
    حالة VMX_GUEST_DS_بنسټ:
    حالة VMX_GUEST_FS_بنسټ:
    حالة VMX_GUEST_GS_بنسټ:
    حالة VMX_GUEST_LDTR_بنسټ:
    حالة VMX_GUEST_TR_بنسټ:
    حالة VMX_GUEST_GDTR_بنسټ:
    حالة VMX_GUEST_IDTR_بنسټ:
    حالة VMX_GUEST_DR7:
    حالة VMX_GUEST_RFLAGS:
    حالة VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالة VMX_GUEST_SYSENTER_ESP:
    حالة VMX_GUEST_SYSENTER_EIP:
    حالة VMX_CONTROL_CR0_ماسک:
    حالة VMX_CONTROL_CR4_ماسک:
    حالة VMX_CONTROL_CR0_READ_SHADOW:
    حالة VMX_CONTROL_CR4_READ_SHADOW:
    حالة VMX_GUEST_CR3:
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
    حالة VMX_CONTROL_ثبت_INTERRUPTION_INFO:
    حالة VMX_CONTROL_ثبت_EXCEPTION_ERROR_کوډ:
        اكسر;
    حالة VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        ارزښت = apply_Fixed_بټونه(ارزښت, pin_control_لوړ, pin_control_ټیټ);
        اكسر;
    حالة VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        ارزښت = apply_Fixed_بټونه(ارزښت, primary_control_لوړ, primary_control_ټیټ);
        اكسر;
    حالة VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        ارزښت = apply_Fixed_بټونه(ارزښت, secondary_control_لوړ, secondary_control_ټیټ);
        اكسر;
    حالة VMX_CONTROL_ثبت_CONTROLS:
        ارزښت = apply_Fixed_بټونه(ارزښت, ثبت_control_لوړ, ثبت_control_ټیټ);
        اكسر;
    حالة VMX_CONTROL_EXIT_CONTROLS:
        ارزښت = apply_Fixed_بټونه(ارزښت, exit_control_لوړ, exit_control_ټیټ);
        اكسر;
    حالة VMX_GUEST_CR0:
        ارزښت = apply_Fixed_بټونه(ارزښت, cr0_لوړ, cr0_ټیټ);
        اكسر;
    حالة VMX_GUEST_CR4:
        ارزښت = apply_Fixed_بټونه(ارزښت, cr4_لوړ, cr4_ټیټ);
        اكسر;
    افتراضي:
        userError("VCPU WriteVMCS: Invalid field %lx.", (اوږد)ډګر);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, بفر, ډګر, ارزښت);
}

ساكن کلمه_t readVMCSField(vcpu_t *vcpu, کلمه_t ډګر)
{
    بدل (ډګر) {
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
        أرجع vcpu->exception_bitmap;
    حالة VMX_GUEST_CR0:
        أرجع vcpu->cr0;
    حالة VMX_CONTROL_CR0_ماسک:
        أرجع vcpu->cr0_ماسک;
    حالة VMX_CONTROL_CR0_READ_SHADOW:
        أرجع vcpu->cr0_shadow;
    }
    إذا (ARCH_غوټه_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    أرجع vmread(ډګر);
}

ساكن exception_t invokeReadVMCS(vcpu_t *vcpu, کلمه_t ډګر, bool_t call, کلمه_t *بفر)
{
    tcb_t *thread;
    thread = غوټه_STATE(ksCurThread);
    کلمه_t ارزښت = readVMCSField(vcpu, ډګر);
    إذا (call) {
        سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
        بینښه صحيح اوږدتیا = سېټ_MR(thread, بفر, 0, ارزښت);
        سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, اوږدتیا)));
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeReadVMCS(cap_t cap, کلمه_t اوږدتیا, bool_t call, کلمه_t *بفر)
{
    إذا (اوږدتیا < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    کلمه_t ډګر = getSyscallArg(0, بفر);
    بدل (ډګر) {
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
    حالة VMX_GUEST_ES_حد:
    حالة VMX_GUEST_CS_حد:
    حالة VMX_GUEST_SS_حد:
    حالة VMX_GUEST_DS_حد:
    حالة VMX_GUEST_FS_حد:
    حالة VMX_GUEST_GS_حد:
    حالة VMX_GUEST_LDTR_حد:
    حالة VMX_GUEST_TR_حد:
    حالة VMX_GUEST_GDTR_حد:
    حالة VMX_GUEST_IDTR_حد:
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
    حالة VMX_GUEST_PREEMPTION_TIMER_ارزښت:
    حالة VMX_GUEST_ES_بنسټ:
    حالة VMX_GUEST_CS_بنسټ:
    حالة VMX_GUEST_SS_بنسټ:
    حالة VMX_GUEST_DS_بنسټ:
    حالة VMX_GUEST_FS_بنسټ:
    حالة VMX_GUEST_GS_بنسټ:
    حالة VMX_GUEST_LDTR_بنسټ:
    حالة VMX_GUEST_TR_بنسټ:
    حالة VMX_GUEST_GDTR_بنسټ:
    حالة VMX_GUEST_IDTR_بنسټ:
    حالة VMX_GUEST_DR7:
    حالة VMX_GUEST_RFLAGS:
    حالة VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالة VMX_GUEST_SYSENTER_ESP:
    حالة VMX_GUEST_SYSENTER_EIP:
    حالة VMX_CONTROL_CR0_ماسک:
    حالة VMX_CONTROL_CR4_ماسک:
    حالة VMX_CONTROL_CR0_READ_SHADOW:
    حالة VMX_CONTROL_CR4_READ_SHADOW:
    حالة VMX_معلومات_INSTRUCTION_ERROR:
    حالة VMX_معلومات_EXIT_مداخله_INFO:
    حالة VMX_معلومات_EXIT_مداخله_ERROR:
    حالة VMX_معلومات_IDT_ویکټور_INFO:
    حالة VMX_معلومات_IDT_ویکټور_ERROR:
    حالة VMX_معلومات_EXIT_INSTRUCTION_اوږدتیا:
    حالة VMX_معلومات_EXIT_INSTRUCTION_INFO:
    حالة VMX_معلومات_GUEST_PHYSICAL:
    حالة VMX_معلومات_IO_RCX:
    حالة VMX_معلومات_IO_RSI:
    حالة VMX_معلومات_IO_RDI:
    حالة VMX_معلومات_IO_RIP:
    حالة VMX_معلومات_GUEST_خطي_پته:
    حالة VMX_CONTROL_ثبت_INTERRUPTION_INFO:
    حالة VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    حالة VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    حالة VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
    حالة VMX_CONTROL_ثبت_CONTROLS:
    حالة VMX_CONTROL_EXIT_CONTROLS:
    حالة VMX_GUEST_CR0:
    حالة VMX_GUEST_CR3:
    حالة VMX_GUEST_CR4:
        اكسر;
    افتراضي:
        userError("VCPU ReadVMCS: Invalid field %lx.", (اوږد)ډګر);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ډګر, call, بفر);
}

ساكن exception_t invoke_سېټ_TCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سېټ_TCB(cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    cap_t tcbCap;
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع invoke_سېټ_TCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

فراغ vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    کلمه_t *بفر;
    إذا (ARCH_غوټه_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بفر = lookupIPCBuffer(false, غوټه_STATE(ksCurThread));
    إذا (!بفر) {
        userError("No IPC buffer.");
        أرجع;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, بفر));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, apply_Fixed_بټونه(getSyscallArg(1, بفر), primary_control_لوړ,
                                                                   primary_control_ټیټ));
    vmwrite(VMX_CONTROL_ثبت_INTERRUPTION_INFO, getSyscallArg(2, بفر));
}

فراغ vcpu_sysvmenter_ځواب_منزل_user(tcb_t *tcb)
{
    کلمه_t *بفر;
    vcpu_t *vcpu;

    بفر = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    أكد(vcpu);

    إذا (ARCH_غوټه_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    سېټ_MR(tcb, بفر, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    سېټ_MR(tcb, بفر, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    سېټ_MR(tcb, بفر, SEL4_VMENTER_CALL_مداخله_INFO_MR,
          vmread(VMX_CONTROL_ثبت_INTERRUPTION_INFO));
    سېټ_ثبتوونکی(tcb, پیغام_Info_ثبتوونکی, 0);
}

exception_t decodeX86VCPUInvocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cptr_t cptr,
    cte_t *خونه,
    cap_t cap,
    bool_t call,
    کلمه_t *بفر
)
{
    بدل (invLabel) {
    حالة X86VCPUSetTCB:
        أرجع decode_سېټ_TCB(cap, اوږدتیا, بفر);
    حالة X86VCPUReadVMCS:
        أرجع decodeReadVMCS(cap, اوږدتیا, call, بفر);
    حالة X86VCPUWriteVMCS:
        أرجع decodeWriteVMCS(cap, اوږدتیا, call, بفر);
    حالة X86VCPUEnableIOPort:
        أرجع decodeEnableIOPort(cap, اوږدتیا, بفر);
    حالة X86VCPUDisableIOPort:
        أرجع decodeDisableIOPort(cap, اوږدتیا, بفر);
    حالة X86VCPUWriteRegisters:
        أرجع decodeVCPUWriteRegisters(cap, اوږدتیا, بفر);
#که_تعریف_شوی CONFIG_X86_64_VTX_64_بټ_GUESTS
    حالة X86VCPUWriteMSR:
        أرجع decodeVCPUWriteMSR(cap, اوږدتیا, بفر);
    حالة X86VCPUReadMSR:
        أرجع decodeVCPUReadMSR(cap, اوږدتیا, بفر);
#د_که_پای /* CONFIG_X86_64_VTX_64BIT_GUESTS */
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
    أرجع !!(x86_cpuid_ecx(0x1, 0) & بټ(5));
}

ساكن مضمن فراغ clear_بټ(کلمه_t *bitmap, کلمه_t بټ_2)
{
    صحيح شاخص = بټ_2 / (حجم(کلمه_t) * 8);
    صحيح دننه_واټن = بټ_2 % (حجم(کلمه_t) * 8);
    bitmap[شاخص] &= ~بټ(دننه_واټن);
}

BOOT_کوډ bool_t vtx_init(فراغ)
{
    إذا (!is_vtx_supported()) {
        چاپ_کړه("vt-x: not supported\n");
        أرجع false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.ټکي[0] = x86_rdmsr_ټیټ(IA32_VMX_BASIC_MSR);
    vmx_basic.ټکي[1] = x86_rdmsr_لوړ(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.ټکي[0] = x86_rdmsr_ټیټ(IA32_FEATURE_CONTROL_MSR);
    إذا (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        إذا (feature_control_msr_get_قلف(feature_control)) {
            چاپ_کړه("vt-x: feature locked\n");
            أرجع false;
        }
        feature_control = feature_control_msr_سېټ_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_لوړ(IA32_FEATURE_CONTROL_MSR), feature_control.ټکي[0]);
    }
    /* make sure the msr is locked */
    إذا (!feature_control_msr_get_قلف(feature_control)) {
        feature_control = feature_control_msr_سېټ_قلف(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_لوړ(IA32_FEATURE_CONTROL_MSR), feature_control.ټکي[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    إذا (CURRENT_CPU_شاخص() == 0) {
        إذا (!init_vtx_fixed_ارزښتونه(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            چاپ_کړه("vt-x: lack of required features\n");
            أرجع false;
        }
    }
    إذا (!check_vtx_fixed_ارزښتونه(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        چاپ_کړه("vt-x: cores have inconsistent features\n");
        أرجع false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    بنية vmxon_region *vmxon_kptr = &ARCH_غوټه_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    إذا (!vtx_check_fixed_ارزښتونه(read_cr0(), read_cr4())) {
        أرجع false;
    }
    إذا (vmxon(kpptr_منزل_paddr(vmxon_kptr))) {
        چاپ_کړه("vt-x: vmxon failure\n");
        أرجع false;
    }
    memset(&msr_bitmap_region, ~0, حجم(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_بټ(msr_bitmap_region.ټیټ_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_بټ(msr_bitmap_region.ټیټ_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_بټ(msr_bitmap_region.ټیټ_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_بټ(msr_bitmap_region.ټیټ_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_بټ(msr_bitmap_region.ټیټ_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_بټ(msr_bitmap_region.ټیټ_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#که_تعریف_شوی CONFIG_X86_64_VTX_64_بټ_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_بټ(msr_bitmap_region.لوړ_msr_read.bitmap, MSR_BITMAP_ماسک(IA32_FS_بنسټ_MSR));
    clear_بټ(msr_bitmap_region.لوړ_msr_read.bitmap, MSR_BITMAP_ماسک(IA32_GS_بنسټ_MSR));
    clear_بټ(msr_bitmap_region.لوړ_msr_read.bitmap, MSR_BITMAP_ماسک(IA32_KERNEL_GS_بنسټ_MSR));
    clear_بټ(msr_bitmap_region.لوړ_msr_write.bitmap, MSR_BITMAP_ماسک(IA32_FS_بنسټ_MSR));
    clear_بټ(msr_bitmap_region.لوړ_msr_write.bitmap, MSR_BITMAP_ماسک(IA32_GS_بنسټ_MSR));
    clear_بټ(msr_bitmap_region.لوړ_msr_write.bitmap, MSR_BITMAP_ماسک(IA32_KERNEL_GS_بنسټ_MSR));
#د_که_پای
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.ټکي[0] = x86_rdmsr_ټیټ(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.ټکي[1] = x86_rdmsr_لوړ(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    إذا (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        چاپ_کړه("vt-x: Expected wb attribute for EPT paging structure\n");
        أرجع false;
    }
    إذا (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        چاپ_کړه("vt-x: Expected supported for 2m pages\n");
        أرجع false;
    }

    أرجع true;
}

ساكن فراغ سېټ_MRs_vmexit(uint32_t reason, کلمه_t qualification)
{
    کلمه_t *بفر;
    صحيح i;

    بفر = lookupIPCBuffer(true, غوټه_STATE(ksCurThread));

    سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_CALL_مداخله_INFO_MR,
          vmread(VMX_CONTROL_ثبت_INTERRUPTION_INFO));
    سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_REASON_MR, reason);
    سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_معلومات_EXIT_INSTRUCTION_اوږدتیا));
    سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_معلومات_GUEST_PHYSICAL));
    سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    لكل (i = 0; i < n_vcpu_gp_ثبتوونکی; i++) {
        سېټ_MR(غوټه_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_EAX + i,
              غوټه_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

ساكن فراغ handleVmxFault(uint32_t reason, کلمه_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    سېټ_ثبتوونکی(غوټه_STATE(ksCurThread), پیغام_Info_ثبتوونکی, SEL4_VMENTER_پایله_FAULT);

    سېټ_MRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

ساكن مضمن فراغ پای_Vmexit_Saving(فراغ)
{
    vcpu_t *vcpu = ARCH_غوټه_STATE(x86KSCurrentVCPU);
    أكد(vcpu == غوټه_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(فراغ)
{
    uint32_t مداخله;
    /* qualification is host width, reason is defined as being 32 bit */
    کلمه_t qualification;
    uint32_t reason;
    پای_Vmexit_Saving();
#که_تعریف_شوی CONFIG_X86_64_VTX_64_بټ_GUESTS
    vcpu_restore_host_msrs();
#د_که_پای
    /* the basic exit reason is the bottom 16 bits of the exit reason field */
    reason = vmread(VMX_معلومات_EXIT_REASON) & ماسک(16);
    إذا (reason == EXTERNAL_مداخله) {
        غوټه_قلف_IRQ;
        إذا (vmx_feature_ack_on_exit) {
            مداخله = vmread(VMX_معلومات_EXIT_مداخله_INFO) & 0xff;
            ARCH_غوټه_STATE(x86_KScur_مداخله) = مداخله;
            handle_مداخله_ثبت();
        } وإلا {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#که_تعریف_شوی ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(غوټه_STATE(ksCurThread));
#د_که_پای
        أرجع EXCEPTION_NONE;
    }

    غوټه_قلف_SYS;

    بدل (reason) {
    حالة EXCEPTION_OR_NMI:
    حالة MOV_DR:
    حالة TASK_SWITCH:
    حالة CONTROL_ثبتوونکی:
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
    حالة TPR_لاندې_پوله:
    حالة APIC_ACCESS:
        qualification = vmread(VMX_معلومات_EXIT_QUALIFICATION);
        اكسر;
    افتراضي:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    أرجع EXCEPTION_NONE;
}

exception_t handle_Vm_ثبت_Fail(فراغ)
{
    handleVmxFault(-1, -1);

    أرجع EXCEPTION_NONE;
}

#که_تعریف_شوی ENABLE_SMP_SUPPORT
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
    إذا (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_فعال) {

        کلمه_t badge = notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr);
        notification_ptr_سېټ_state(ntfnPtr, NtfnState_Idle);
        سېټ_Thread_State(tcb, ThreadState_Running);
        سېټ_ثبتوونکی(tcb, badge_ثبتوونکی, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        إذا (tcb != غوټه_STATE(ksCurThread)) {
            possible_Switch_منزل(tcb);
        }
    }
}
#د_که_پای /* ENABLE_SMP_SUPPORT */

ساكن فراغ invvpid_context(uint16_t vpid)
{
    بنية {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t پته;
    } PACKED عملوند = {vpid, 0, 0};
    asm متطاير("invvpid %0, %1" :: "m"(عملوند), "r"((کلمه_t)1) : "cc");
}

ساكن فراغ سېټ_EPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_ریښه;
    إذا (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_ریښه = kpptr_منزل_paddr(null_ept_space);
    } وإلا {
        پیدا_کړه_EPTFor_ASID_ret_t پیدا_کړه_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_بنسټ_Ptr(vmxSpace);
        پیدا_کړه_ret = پیدا_کړه_EPTFor_ASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE || پیدا_کړه_ret.ept != pml4) {
            ept_ریښه = kpptr_منزل_paddr(null_ept_space);
        } وإلا {
            ept_ریښه = pptr_منزل_paddr(pml4);
        }
    }
    إذا (ept_ریښه != vcpu->وروستی_ept_ریښه) {
        vcpu->وروستی_ept_ریښه = ept_ریښه;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_ریښه,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_اشاره_ګر, eptp.ټکي[0]);
        أكد(vcpu->vpid != VPID_INVALID);
        إذا (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

فراغ clearVPIDIOPortMappings(vpid_t vpid, uint16_t لومړی, uint16_t وروستی)
{
    إذا (vpid == VPID_INVALID) {
        أرجع;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    إذا (vcpu == عدم) {
        أرجع;
    }
    أكد(vcpu->vpid == vpid);
    سېټ_IOPort_ماسک(vcpu->io, لومړی, وروستی, true);
}

ساكن مضمن vpid_t بل_VPID(vpid_t vpid)
{
    إذا (vpid == VPID_وروستی) {
        أرجع VPID_لومړی;
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
    إذا (vmx_feature_vpid) {
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

ساكن vpid_t پیدا_کړه_Free_VPID(فراغ)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    افعل {
        إذا (x86KSVPIDTable[vpid] == عدم) {
            أرجع vpid;
        }
        vpid = بل_VPID(vpid);
    } طالما (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = بل_VPID(x86KSNextVPID);
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
فراغ vcpu_fpu_منزل_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    إذا (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        إذا (!vcpu->fpu_فعال) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_فعال = true;
            بارول_Fpu_State(tcb);
        }
    } وإلا {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_فعال = true;
        switchLocalFpuOwner(tcb);
    }
}

فراغ vcpu_fpu_منزل_host(tcb_t *tcb, vcpu_t *vcpu)
{
    إذا (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } وإلا إذا (nativeThreadUsingFPU(tcb) && vcpu->fpu_فعال) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_فعال = false;
        بارول_Fpu_State(tcb);
    } وإلا إذا (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

فراغ restoreVMCS(فراغ)
{
    tcb_t *cur_thread = غوټه_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    إذا (ARCH_غوټه_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#که_نه_تعریف_شوی CONFIG_KERNEL_SKIM_کړکۍ
    إذا (getCurrentCR3().ټکي[0] != expected_vmcs->وروستی_host_cr3) {
        expected_vmcs->وروستی_host_cr3 = getCurrentCR3().ټکي[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().ټکي[0]);
    }
#د_که_پای
    إذا (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = پیدا_کړه_Free_VPID();
        storeVPID(expected_vmcs, vpid);
        إذا (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    سېټ_EPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

فراغ invept(ept_pml4e_t *ept_pml4)
{
    إذا (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        بنية {
            uint64_t parts[2];
        } پته;
        کلمه_t type;
        إذا (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } وإلا إذا (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } وإلا {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            أرجع;
        }

        پته.parts[0] = pptr_منزل_paddr((فراغ *)ept_pml4);
        پته.parts[1] = 0;
        asm متطاير(
            "invept %0, %1"
            :
            : "m"(پته),  "r"(type)
            : "memory"
        );
    }
}

#د_که_پای
