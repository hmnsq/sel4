/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_VTX

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

#تعریف_کن VMX_EXIT_QUAL_TYPE_MOV_CR 0
#تعریف_کن VMX_EXIT_QUAL_TYPE_CLTS 2
#تعریف_کن VMX_EXIT_QUAL_TYPE_LMSW 3

ثابت vcpu_gp_ثبات_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#تعریف_کن MSR_BITMAP_ماسک(x) ((x) & 0x1fff)

تعريفنوع بنية msr_bitmap {
    کلمه_t bitmap[0x2000 / حجم(کلمه_t) / 8];
} msr_bitmap_t;

تعريفنوع بنية msr_bitmaps {
    msr_bitmap_t کم_msr_read;
    msr_bitmap_t بالا_msr_read;
    msr_bitmap_t کم_msr_write;
    msr_bitmap_t بالا_msr_write;
} msr_bitmaps_t;

ساكن msr_bitmaps_t msr_bitmap_region ALIGN(بیت(se_L4_صفحه_بیتها));

ساكن حرف null_ept_space[بیت(se_L4_صفحه_بیتها)] ALIGN(بیت(se_L4_صفحه_بیتها));

/* Cached value of the hardware defined vmcs revision */
ساكن uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
ساكن vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
ساكن uint32_t pin_control_بالا;
ساكن uint32_t pin_control_کم;
ساكن uint32_t primary_control_بالا;
ساكن uint32_t primary_control_کم;
ساكن uint32_t secondary_control_بالا;
ساكن uint32_t secondary_control_کم;
ساكن uint32_t مدخل_control_بالا;
ساكن uint32_t مدخل_control_کم;
ساكن uint32_t exit_control_بالا;
ساكن uint32_t exit_control_کم;
ساكن uint32_t cr0_بالا;
ساكن uint32_t cr0_کم;
ساكن uint32_t cr4_بالا;
ساكن uint32_t cr4_کم;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
ساكن bool_t vmx_feature_vpid;
ساكن bool_t vmx_feature_بارگیری_perf_global_ctrl;
ساكن bool_t vmx_feature_ack_on_exit;

ساكن vcpu_t *x86KSVPIDTable[VPID_آخرین + 1];
ساكن vpid_t x86KSNextVPID = VPID_اولی;

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
    uint64_t physical_آدرس;
    physical_آدرس = pptr_مقصد_paddr((فراغ *)vmcs_ptr);
    asm متطاير(
        "vmclear %0"
        :
        : "m"(physical_آدرس)
        : "cc"
    );
}

فراغ clearCurrentVCPU(فراغ)
{
    vcpu_t *vcpu = ARCH_گره_STATE(x86KSCurrentVCPU);
    إذا (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_گره_STATE(x86KSCurrentVCPU) = عدم;
        إذا (vcpu->fpu_فعال && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_فعال = false;
        }
    }
}

ساكن فراغ vmptrld(فراغ *vmcs_ptr)
{
    uint64_t physical_آدرس;
    uint8_t error;
    physical_آدرس = pptr_مقصد_paddr(vmcs_ptr);
    asm متطاير(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_آدرس)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    أكد(!error);
}

#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
فراغ vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_ستاره_MSR, vcpu->syscall_registers[VCPU_ستاره]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_ماسک]);
}

فراغ vcpu_restore_host_msrs(فراغ)
{
    init_syscall_msrs();
}
#پایان_اگر

ساكن فراغ switchVCPU(vcpu_t *vcpu)
{
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    إذا (vcpu->آخرین_cpu != getCurrentCPUIndex() && ARCH_گره_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->آخرین_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->آخرین_cpu);
    }
#پایان_اگر
    clearCurrentVCPU();
    vmptrld(vcpu);
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    إذا (vcpu->آخرین_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_پایه, (کلمه_t)&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss);
        vmwrite(VMX_HOST_GDTR_پایه, (کلمه_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_پایه, (کلمه_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(کلمه_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss.tss.واژگان[0] +
                                                          4));
    }
    vcpu->آخرین_cpu = getCurrentCPUIndex();
#پایان_اگر
    ARCH_گره_STATE(x86KSCurrentVCPU) = vcpu;
}

ساكن فراغ print_بیتها(کلمه_t بیتها)
{
    bool_t اولی = true;
    طالما (بیتها) {
        صحيح شاخص = se_L4_کلمه_بیتها - 1 - clzl(بیتها);
        إذا (اولی) {
            چاپ_کن("%d", شاخص);
            اولی = false;
        } وإلا {
            چاپ_کن(",%d", شاخص);
        }
        بیتها &= ~بیت(شاخص);
    }
}

ساكن bool_t check_fixed_مقدار(کلمه_t val, کلمه_t کم, کلمه_t بالا)
{
    کلمه_t not_بالا;
    کلمه_t not_کم;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_بالا = بالا & ~(بالا & val);
    إذا (not_بالا != 0) {
        چاپ_کن("Failed to set bits: ");
        print_بیتها(not_بالا);
        أرجع false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_کم = ~کم & ~(~کم & ~val);
    إذا (not_کم != 0) {
        چاپ_کن("Incorrectly cleared bits: ");
        print_بیتها(not_کم);
        أرجع false;
    }
    أرجع true;
}

ساكن bool_t vtx_check_fixed_مقادیر(کلمه_t cr0, کلمه_t cr4)
{
    إذا (!check_fixed_مقدار(cr0, cr0_کم, cr0_بالا)) {
        چاپ_کن(" of CR0\n");
        أرجع false;
    }
    إذا (!check_fixed_مقدار(cr4, cr4_کم, cr4_بالا)) {
        چاپ_کن(" of CR4\n");
        أرجع false;
    }
    أرجع true;
}

ساكن bool_t BOOT_کد init_vtx_fixed_مقادیر(bool_t useTrueMsrs)
{
    uint32_t pin_control_ماسک =
        بیت(0) |    //Extern interrupt exiting
        بیت(3) |    //NMI exiting
        بیت(5);     //virtual NMIs
    uint32_t primary_control_ماسک =
        بیت(25) |   //Use I/O bitmaps
        بیت(28) |   //Use MSR bitmaps
        بیت(31);    //Activate secondary controls
    uint32_t secondary_control_ماسک =
        بیت(1);     //Enable EPT
    uint32_t exit_control_ماسک =
        بیت(2)  |   //Save debug controls
        بیت(18) |   //Save guest IA32_PAT on exit
        بیت(19) |   //Load host IA32_PAT
        بیت(20) |   //Save guest IA32_EFER on exit
        بیت(21);    //Load host IA32_EFER
#اگر_تعریف_شده CONFIG_ARCH_X86_64
    exit_control_ماسک |= بیت(9); //Host address-space size
#پایان_اگر /* CONFIG_ARCH_X86_64 */
    /* Read out the fixed high and low bits from the MSRs */
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t مدخل_ctls;
    إذا (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        مدخل_ctls = IA32_VMX_TRUE_مدخل_CTLS_MSR;
    } وإلا {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        مدخل_ctls = IA32_VMX_مدخل_CTLS_MSR;
    }
    pin_control_بالا = x86_rdmsr_کم(pinbased_ctls);
    pin_control_کم = x86_rdmsr_بالا(pinbased_ctls);
    primary_control_بالا = x86_rdmsr_کم(procbased_ctls);
    primary_control_کم = x86_rdmsr_بالا(procbased_ctls);
    secondary_control_بالا = x86_rdmsr_کم(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_کم = x86_rdmsr_بالا(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_بالا = x86_rdmsr_کم(exit_ctls);
    exit_control_کم = x86_rdmsr_بالا(exit_ctls);
    مدخل_control_بالا = x86_rdmsr_کم(مدخل_ctls);
    مدخل_control_کم = x86_rdmsr_بالا(مدخل_ctls);

    cr0_بالا = x86_rdmsr_کم(IA32_VMX_CR0_FIXED0_MSR);
    cr0_کم = x86_rdmsr_کم(IA32_VMX_CR0_FIXED1_MSR);
    cr4_بالا = x86_rdmsr_کم(IA32_VMX_CR4_FIXED0_MSR);
    cr4_کم = x86_rdmsr_کم(IA32_VMX_CR4_FIXED1_MSR);

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
    إذا (secondary_control_کم & بیت(7)) {
        cr0_بالا &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    إذا (!(secondary_control_کم & بیت(5))) {
        vmx_feature_vpid = 0;
        چاپ_کن("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } وإلا {
        vmx_feature_vpid = 1;
        secondary_control_ماسک |= بیت(5);
    }

    /* Check for load perf global control */
    إذا (!(exit_control_کم & بیت(12))) {
        vmx_feature_بارگیری_perf_global_ctrl = 0;
        چاپ_کن("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } وإلا {
        vmx_feature_بارگیری_perf_global_ctrl = 1;
        exit_control_ماسک |= بیت(12);
    }

    /* Check for external interrupt exiting */
    إذا (!(exit_control_کم & بیت(15))) {
        vmx_feature_ack_on_exit = 0;
        چاپ_کن("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } وإلا {
        vmx_feature_ack_on_exit = 1;
        exit_control_ماسک |= بیت(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_کم) & pin_control_ماسک;
    إذا (missing) {
        چاپ_کن("vt-x: Unsupported pin control features %lx\n", (دراز)missing);
        أرجع false;
    }
    missing = (~primary_control_کم) & primary_control_ماسک;
    إذا (missing) {
        چاپ_کن("vt-x: Unsupported primary control features %lx\n", (دراز)missing);
        أرجع false;
    }
    missing = (~secondary_control_کم) & secondary_control_ماسک;
    إذا (missing) {
        چاپ_کن("vt-x: Unsupported secondary control features %lx\n", (دراز)missing);
        أرجع false;
    }
    missing = (~exit_control_کم) & exit_control_ماسک;
    إذا (missing) {
        چاپ_کن("vt-x: Unsupported exit control features %lx\n", (دراز)missing);
        أرجع false;
    }
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
    missing = (~مدخل_control_کم) & بیت(9);
    إذا (missing) {
        چاپ_کن("vt-x: CPU does not support 64-bit guests\n");
        أرجع false;
    }
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_بالا |= pin_control_ماسک;
    primary_control_بالا |= primary_control_ماسک;
    secondary_control_بالا |= secondary_control_ماسک;
    exit_control_بالا |= exit_control_ماسک;

    أرجع true;
}

ساكن bool_t BOOT_کد check_vtx_fixed_مقادیر(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t مدخل_ctls;
    إذا (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        مدخل_ctls = IA32_VMX_TRUE_مدخل_CTLS_MSR;
    } وإلا {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        مدخل_ctls = IA32_VMX_مدخل_CTLS_MSR;
    }
    uint32_t local_pin_control_بالا = x86_rdmsr_کم(pinbased_ctls);
    uint32_t local_pin_control_کم = x86_rdmsr_بالا(pinbased_ctls);
    uint32_t local_primary_control_بالا = x86_rdmsr_کم(procbased_ctls);
    uint32_t local_primary_control_کم = x86_rdmsr_بالا(procbased_ctls);
    uint32_t local_secondary_control_بالا = x86_rdmsr_کم(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_کم = x86_rdmsr_بالا(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_بالا = x86_rdmsr_کم(exit_ctls);
    uint32_t local_exit_control_کم = x86_rdmsr_بالا(exit_ctls);
    uint32_t local_مدخل_control_بالا = x86_rdmsr_کم(مدخل_ctls);
    uint32_t local_مدخل_control_کم = x86_rdmsr_بالا(مدخل_ctls);

    uint32_t local_cr0_بالا = x86_rdmsr_کم(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_کم = x86_rdmsr_کم(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_بالا = x86_rdmsr_کم(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_کم = x86_rdmsr_کم(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    إذا (local_secondary_control_کم & بیت(7)) {
        local_cr0_بالا &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    أرجع
        (local_pin_control_بالا & pin_control_بالا) == local_pin_control_بالا &&
        (~local_pin_control_کم & pin_control_بالا) == 0 &&
        (local_primary_control_بالا & primary_control_بالا) == local_primary_control_بالا &&
        (~local_primary_control_کم & primary_control_بالا) == 0 &&
        (local_secondary_control_بالا & secondary_control_بالا) == local_secondary_control_بالا &&
        (~local_secondary_control_کم & secondary_control_بالا) == 0 &&
        (local_exit_control_بالا & exit_control_بالا) == local_exit_control_بالا &&
        (~local_exit_control_کم & exit_control_بالا) == 0 &&
        (local_مدخل_control_بالا & مدخل_control_بالا) == local_مدخل_control_بالا &&
        (~local_مدخل_control_کم & مدخل_control_بالا) == 0 &&
        local_cr0_بالا == cr0_بالا &&
        local_cr0_کم == cr0_کم &&
        local_cr4_بالا == cr4_بالا &&
        local_cr4_کم == cr4_کم;
}

ساكن مضمن uint32_t apply_Fixed_بیتها(uint32_t original, uint32_t بالا, uint32_t کم)
{
    original |= بالا;
    original &= کم;
    أرجع original;
}

فراغ vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = عدم;
    vcpu->launched = false;
    vcpu->fpu_فعال = false;
    vcpu->fpuState = x86KSnullFpuState;

    کپی_حافظه(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_بالا & cr0_کم;
    vcpu->cr0_shadow = 0;
    vcpu->cr0_ماسک = 0;
    vcpu->exception_bitmap = 0;
    vcpu->vpid = VPID_INVALID;
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    vcpu->آخرین_cpu = getCurrentCPUIndex();
#پایان_اگر /* ENABLE_SMP_SUPPORT */

    switchVCPU(vcpu);

    vmwrite(VMX_HOST_PAT, x86_rdmsr(IA32_PAT_MSR));
    vmwrite(VMX_HOST_EFER, x86_rdmsr(IA32_EFER_MSR));
    // By default we will disable performance counters when we come out
    // of a VM. When performance counters are supported this host state
    // needs to be updated on VM entry
    إذا (vmx_feature_بارگیری_perf_global_ctrl) {
        vmwrite(VMX_HOST_PERF_GLOBAL_CTRL, 0);
    }
    vmwrite(VMX_HOST_CR0, read_cr0());
    vmwrite(VMX_HOST_CR4, read_cr4());
    vmwrite(VMX_HOST_FS_پایه, 0);
    vmwrite(VMX_HOST_GS_پایه, 0);
    vmwrite(VMX_HOST_TR_پایه, (کلمه_t)&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss);
    vmwrite(VMX_HOST_GDTR_پایه, (کلمه_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSgdt);
    vmwrite(VMX_HOST_IDTR_پایه, (کلمه_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSidt);
    vmwrite(VMX_HOST_SYSENTER_CS, (کلمه_t)SEL_CS_0);
    vmwrite(VMX_HOST_SYSENTER_EIP, (کلمه_t)&handle_syscall);
    إذا (!config_مجموعه(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(کلمه_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss.tss.واژگان[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (کلمه_t)&vcpu->gp_registers[n_vcpu_gp_ثبات]);
    vmwrite(VMX_HOST_RIP, (کلمه_t)&handle_vmexit);
#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_مقصد_paddr(x64KSKernelPML4), 0).واژگان[0]);
#پایان_اگر /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_بالا & pin_control_کم);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_بالا & primary_control_کم);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_بالا & secondary_control_کم);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_بالا & exit_control_کم);

    uint64_t مدخل_control = مدخل_control_بالا & مدخل_control_کم;
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    مدخل_control |= بیت(9);
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_مدخل_CONTROLS, مدخل_control);

    vmwrite(VMX_CONTROL_MSR_آدرس, (کلمه_t)kpptr_مقصد_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_بالا & cr4_کم);

    vmwrite(VMX_GUEST_VMCS_LINK_اشاره_گر, ~(کلمه_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_اشاره_گر_بالا, ~(کلمه_t)0);

    memset(vcpu->io, ~(کلمه_t)0, VCPU_IOBITMAP_اندازه);
    vmwrite(VMX_CONTROL_IOA_آدرس, pptr_مقصد_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_آدرس, pptr_مقصد_paddr((حرف *)vcpu->io + (VCPU_IOBITMAP_اندازه / 2)));
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
    إذا (ARCH_گره_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->آخرین_cpu) == vcpu) {
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
        إذا (vcpu->آخرین_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->آخرین_cpu);
        } وإلا
#پایان_اگر /* ENABLE_SMP_SUPPORT */
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

ساكن exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, کلمه_t *میانگیر)
{
    صحيح i;
    لكل (i = 0; i < n_vcpu_gp_ثبات; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, میانگیر);
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUWriteRegisters(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
    إذا (درازی < 15) {
#وإلا
    إذا (درازی < 7) {
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), میانگیر);
}

#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
ساكن exception_t invokeWriteMSR(vcpu_t *vcpu, کلمه_t *میانگیر, کلمه_t فیلد, کلمه_t مقدار)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    إذا (ARCH_گره_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بدل (فیلد) {
    حالة IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = مقدار;
        اكسر;
    حالة IA32_ستاره_MSR:
        vcpu->syscall_registers[VCPU_ستاره] = مقدار;
        اكسر;
    حالة IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = مقدار;
        اكسر;
    حالة IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_ماسک] = مقدار;
        اكسر;
    }
    مجموعه_MR(thread, میانگیر, 0, مقدار);
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUWriteMSR(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    کلمه_t فیلد;
    کلمه_t مقدار;

    إذا (درازی < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    فیلد = getSyscallArg(0, میانگیر);
    مقدار = getSyscallArg(1, میانگیر);
    بدل (فیلد) {
    حالة IA32_LSTAR_MSR:
    حالة IA32_ستاره_MSR:
    حالة IA32_CSTAR_MSR:
    حالة IA32_FMASK_MSR:
        اكسر;
    افتراضي:
        userError("VCPU WriteMSR: Invalid field %lx.", (دراز)فیلد);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), میانگیر, فیلد, مقدار);
}

ساكن exception_t invokeReadMSR(vcpu_t *vcpu, کلمه_t فیلد, کلمه_t *میانگیر)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);

    کلمه_t مقدار = 0;

    بدل (فیلد) {
    حالة IA32_LSTAR_MSR:
        مقدار = vcpu->syscall_registers[VCPU_LSTAR];
        اكسر;
    حالة IA32_ستاره_MSR:
        مقدار = vcpu->syscall_registers[VCPU_ستاره];
        اكسر;
    حالة IA32_CSTAR_MSR:
        مقدار = vcpu->syscall_registers[VCPU_CSTAR];
        اكسر;
    حالة IA32_FMASK_MSR:
        مقدار = vcpu->syscall_registers[VCPU_SYSCALL_ماسک];
        اكسر;
    حالة IA32_VMX_MISC_MSR:
        مقدار = x86_rdmsr(فیلد);
        اكسر;
    }

    مجموعه_MR(thread, میانگیر, 0, مقدار);
    مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                    se_L4_پیام_Info_new(0, 0, 0, 1)));
    مجموعه_Thread_State(thread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUReadMSR(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    إذا (درازی < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    کلمه_t فیلد = getSyscallArg(0, میانگیر);
    بدل (فیلد) {
    حالة IA32_LSTAR_MSR:
    حالة IA32_ستاره_MSR:
    حالة IA32_CSTAR_MSR:
    حالة IA32_FMASK_MSR:
    حالة IA32_VMX_MISC_MSR:
        اكسر;
    افتراضي:
        userError("VCPU ReadMSR: Invalid field %lx.", (دراز)فیلد);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), فیلد, میانگیر);
}
#پایان_اگر  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

ساكن exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *جایگاه, cap_t cap, uint16_t کم, uint16_t بالا)
{
    /* remove any existing io ports from this cap */
    clearVPIDIOPortMappings(cap_io_پورت_cap_get_cap_IOPort_VPID(cap),
                            cap_io_پورت_cap_get_cap_IOPort_اولی_پورت(cap),
                            cap_io_پورت_cap_get_cap_IOPort_آخرین_پورت(cap));
    /* update the assigned vpid. If the vcpu does not have a valid vpid then
     * this is fine as whilst the cap will not point to the vcpu, the vcpu
     * will have its port mask cleared when it gets assigned a vpid */
    cap = cap_io_پورت_cap_مجموعه_cap_IOPort_VPID(cap, vcpu->vpid);
    جایگاه->cap = cap;
    مجموعه_IOPort_ماسک(vcpu->io, کم, بالا, false);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeEnableIOPort(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *io_جایگاه;
    uint16_t کم, بالا;

    إذا (درازی < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    io_جایگاه = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(ioCap) != cap_io_پورت_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کم = getSyscallArg(0, میانگیر);
    بالا = getSyscallArg(1, میانگیر);

    إذا (کم < cap_io_پورت_cap_get_cap_IOPort_اولی_پورت(ioCap) || بالا > cap_io_پورت_cap_get_cap_IOPort_آخرین_پورت(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeEnableIOPort(vcpu, io_جایگاه, ioCap, کم, بالا);
}

ساكن exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t کم, uint16_t بالا)
{
    مجموعه_IOPort_ماسک(vcpu->io, کم, بالا, true);
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeDisableIOPort(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    vcpu_t *vcpu;
    uint16_t کم, بالا;

    إذا (درازی < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کم = getSyscallArg(0, میانگیر);
    بالا = getSyscallArg(1, میانگیر);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    أرجع invokeDisableIOPort(vcpu, کم, بالا);
}

ساكن exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, کلمه_t *میانگیر, کلمه_t فیلد, کلمه_t مقدار)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);

    إذا (ARCH_گره_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بدل (فیلد) {
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = مقدار;
        اكسر;
    حالة VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = مقدار;
        اكسر;
    حالة VMX_CONTROL_CR0_ماسک:
        vcpu->cr0_ماسک = vcpu->cached_cr0_ماسک = مقدار;
        اكسر;
    حالة VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = مقدار;
        اكسر;
    }
    vmwrite(فیلد, مقدار);

    إذا (call) {
        مجموعه_ثبات(thread, badge_ثبات, 0);
        بیعلامت صحيح درازی = مجموعه_MR(thread, میانگیر, 0, مقدار);
        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, درازی)));
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeWriteVMCS(cap_t cap, کلمه_t درازی, bool_t call, کلمه_t *میانگیر)
{
    کلمه_t فیلد;
    کلمه_t مقدار;

    إذا (درازی < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    فیلد = getSyscallArg(0, میانگیر);
    مقدار = getSyscallArg(1, میانگیر);
    بدل (فیلد) {
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
    حالة VMX_GUEST_PREEMPTION_TIMER_مقدار:
    حالة VMX_GUEST_ES_پایه:
    حالة VMX_GUEST_CS_پایه:
    حالة VMX_GUEST_SS_پایه:
    حالة VMX_GUEST_DS_پایه:
    حالة VMX_GUEST_FS_پایه:
    حالة VMX_GUEST_GS_پایه:
    حالة VMX_GUEST_LDTR_پایه:
    حالة VMX_GUEST_TR_پایه:
    حالة VMX_GUEST_GDTR_پایه:
    حالة VMX_GUEST_IDTR_پایه:
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
    حالة VMX_CONTROL_مدخل_INTERRUPTION_INFO:
    حالة VMX_CONTROL_مدخل_EXCEPTION_ERROR_کد:
        اكسر;
    حالة VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        مقدار = apply_Fixed_بیتها(مقدار, pin_control_بالا, pin_control_کم);
        اكسر;
    حالة VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        مقدار = apply_Fixed_بیتها(مقدار, primary_control_بالا, primary_control_کم);
        اكسر;
    حالة VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        مقدار = apply_Fixed_بیتها(مقدار, secondary_control_بالا, secondary_control_کم);
        اكسر;
    حالة VMX_CONTROL_مدخل_CONTROLS:
        مقدار = apply_Fixed_بیتها(مقدار, مدخل_control_بالا, مدخل_control_کم);
        اكسر;
    حالة VMX_CONTROL_EXIT_CONTROLS:
        مقدار = apply_Fixed_بیتها(مقدار, exit_control_بالا, exit_control_کم);
        اكسر;
    حالة VMX_GUEST_CR0:
        مقدار = apply_Fixed_بیتها(مقدار, cr0_بالا, cr0_کم);
        اكسر;
    حالة VMX_GUEST_CR4:
        مقدار = apply_Fixed_بیتها(مقدار, cr4_بالا, cr4_کم);
        اكسر;
    افتراضي:
        userError("VCPU WriteVMCS: Invalid field %lx.", (دراز)فیلد);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, میانگیر, فیلد, مقدار);
}

ساكن کلمه_t readVMCSField(vcpu_t *vcpu, کلمه_t فیلد)
{
    بدل (فیلد) {
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
        أرجع vcpu->exception_bitmap;
    حالة VMX_GUEST_CR0:
        أرجع vcpu->cr0;
    حالة VMX_CONTROL_CR0_ماسک:
        أرجع vcpu->cr0_ماسک;
    حالة VMX_CONTROL_CR0_READ_SHADOW:
        أرجع vcpu->cr0_shadow;
    }
    إذا (ARCH_گره_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    أرجع vmread(فیلد);
}

ساكن exception_t invokeReadVMCS(vcpu_t *vcpu, کلمه_t فیلد, bool_t call, کلمه_t *میانگیر)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    کلمه_t مقدار = readVMCSField(vcpu, فیلد);
    إذا (call) {
        مجموعه_ثبات(thread, badge_ثبات, 0);
        بیعلامت صحيح درازی = مجموعه_MR(thread, میانگیر, 0, مقدار);
        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, درازی)));
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeReadVMCS(cap_t cap, کلمه_t درازی, bool_t call, کلمه_t *میانگیر)
{
    إذا (درازی < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    کلمه_t فیلد = getSyscallArg(0, میانگیر);
    بدل (فیلد) {
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
    حالة VMX_GUEST_PREEMPTION_TIMER_مقدار:
    حالة VMX_GUEST_ES_پایه:
    حالة VMX_GUEST_CS_پایه:
    حالة VMX_GUEST_SS_پایه:
    حالة VMX_GUEST_DS_پایه:
    حالة VMX_GUEST_FS_پایه:
    حالة VMX_GUEST_GS_پایه:
    حالة VMX_GUEST_LDTR_پایه:
    حالة VMX_GUEST_TR_پایه:
    حالة VMX_GUEST_GDTR_پایه:
    حالة VMX_GUEST_IDTR_پایه:
    حالة VMX_GUEST_DR7:
    حالة VMX_GUEST_RFLAGS:
    حالة VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالة VMX_GUEST_SYSENTER_ESP:
    حالة VMX_GUEST_SYSENTER_EIP:
    حالة VMX_CONTROL_CR0_ماسک:
    حالة VMX_CONTROL_CR4_ماسک:
    حالة VMX_CONTROL_CR0_READ_SHADOW:
    حالة VMX_CONTROL_CR4_READ_SHADOW:
    حالة VMX_داده_INSTRUCTION_ERROR:
    حالة VMX_داده_EXIT_وقفه_INFO:
    حالة VMX_داده_EXIT_وقفه_ERROR:
    حالة VMX_داده_IDT_وکتور_INFO:
    حالة VMX_داده_IDT_وکتور_ERROR:
    حالة VMX_داده_EXIT_INSTRUCTION_درازی:
    حالة VMX_داده_EXIT_INSTRUCTION_INFO:
    حالة VMX_داده_GUEST_PHYSICAL:
    حالة VMX_داده_IO_RCX:
    حالة VMX_داده_IO_RSI:
    حالة VMX_داده_IO_RDI:
    حالة VMX_داده_IO_RIP:
    حالة VMX_داده_GUEST_خطی_آدرس:
    حالة VMX_CONTROL_مدخل_INTERRUPTION_INFO:
    حالة VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    حالة VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    حالة VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
    حالة VMX_CONTROL_مدخل_CONTROLS:
    حالة VMX_CONTROL_EXIT_CONTROLS:
    حالة VMX_GUEST_CR0:
    حالة VMX_GUEST_CR3:
    حالة VMX_GUEST_CR4:
        اكسر;
    افتراضي:
        userError("VCPU ReadVMCS: Invalid field %lx.", (دراز)فیلد);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), فیلد, call, میانگیر);
}

ساكن exception_t invoke_مجموعه_TCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_مجموعه_TCB(cap_t cap, کلمه_t درازی, کلمه_t *میانگیر)
{
    cap_t tcbCap;
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع invoke_مجموعه_TCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

فراغ vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    کلمه_t *میانگیر;
    إذا (ARCH_گره_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    میانگیر = lookupIPCBuffer(false, گره_STATE(ksCurThread));
    إذا (!میانگیر) {
        userError("No IPC buffer.");
        أرجع;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, میانگیر));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, apply_Fixed_بیتها(getSyscallArg(1, میانگیر), primary_control_بالا,
                                                                   primary_control_کم));
    vmwrite(VMX_CONTROL_مدخل_INTERRUPTION_INFO, getSyscallArg(2, میانگیر));
}

فراغ vcpu_sysvmenter_پاسخ_مقصد_user(tcb_t *tcb)
{
    کلمه_t *میانگیر;
    vcpu_t *vcpu;

    میانگیر = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    أكد(vcpu);

    إذا (ARCH_گره_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    مجموعه_MR(tcb, میانگیر, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    مجموعه_MR(tcb, میانگیر, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    مجموعه_MR(tcb, میانگیر, SEL4_VMENTER_CALL_وقفه_INFO_MR,
          vmread(VMX_CONTROL_مدخل_INTERRUPTION_INFO));
    مجموعه_ثبات(tcb, پیام_Info_ثبات, 0);
}

exception_t decodeX86VCPUInvocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *جایگاه,
    cap_t cap,
    bool_t call,
    کلمه_t *میانگیر
)
{
    بدل (invLabel) {
    حالة X86VCPUSetTCB:
        أرجع decode_مجموعه_TCB(cap, درازی, میانگیر);
    حالة X86VCPUReadVMCS:
        أرجع decodeReadVMCS(cap, درازی, call, میانگیر);
    حالة X86VCPUWriteVMCS:
        أرجع decodeWriteVMCS(cap, درازی, call, میانگیر);
    حالة X86VCPUEnableIOPort:
        أرجع decodeEnableIOPort(cap, درازی, میانگیر);
    حالة X86VCPUDisableIOPort:
        أرجع decodeDisableIOPort(cap, درازی, میانگیر);
    حالة X86VCPUWriteRegisters:
        أرجع decodeVCPUWriteRegisters(cap, درازی, میانگیر);
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
    حالة X86VCPUWriteMSR:
        أرجع decodeVCPUWriteMSR(cap, درازی, میانگیر);
    حالة X86VCPUReadMSR:
        أرجع decodeVCPUReadMSR(cap, درازی, میانگیر);
#پایان_اگر /* CONFIG_X86_64_VTX_64BIT_GUESTS */
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
    أرجع !!(x86_cpuid_ecx(0x1, 0) & بیت(5));
}

ساكن مضمن فراغ clear_بیت(کلمه_t *bitmap, کلمه_t بیت_2)
{
    صحيح شاخص = بیت_2 / (حجم(کلمه_t) * 8);
    صحيح جابه_جایی = بیت_2 % (حجم(کلمه_t) * 8);
    bitmap[شاخص] &= ~بیت(جابه_جایی);
}

BOOT_کد bool_t vtx_init(فراغ)
{
    إذا (!is_vtx_supported()) {
        چاپ_کن("vt-x: not supported\n");
        أرجع false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.واژگان[0] = x86_rdmsr_کم(IA32_VMX_BASIC_MSR);
    vmx_basic.واژگان[1] = x86_rdmsr_بالا(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.واژگان[0] = x86_rdmsr_کم(IA32_FEATURE_CONTROL_MSR);
    إذا (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        إذا (feature_control_msr_get_قفل(feature_control)) {
            چاپ_کن("vt-x: feature locked\n");
            أرجع false;
        }
        feature_control = feature_control_msr_مجموعه_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_بالا(IA32_FEATURE_CONTROL_MSR), feature_control.واژگان[0]);
    }
    /* make sure the msr is locked */
    إذا (!feature_control_msr_get_قفل(feature_control)) {
        feature_control = feature_control_msr_مجموعه_قفل(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_بالا(IA32_FEATURE_CONTROL_MSR), feature_control.واژگان[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    إذا (CURRENT_CPU_شاخص() == 0) {
        إذا (!init_vtx_fixed_مقادیر(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            چاپ_کن("vt-x: lack of required features\n");
            أرجع false;
        }
    }
    إذا (!check_vtx_fixed_مقادیر(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        چاپ_کن("vt-x: cores have inconsistent features\n");
        أرجع false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    بنية vmxon_region *vmxon_kptr = &ARCH_گره_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    إذا (!vtx_check_fixed_مقادیر(read_cr0(), read_cr4())) {
        أرجع false;
    }
    إذا (vmxon(kpptr_مقصد_paddr(vmxon_kptr))) {
        چاپ_کن("vt-x: vmxon failure\n");
        أرجع false;
    }
    memset(&msr_bitmap_region, ~0, حجم(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_بیت(msr_bitmap_region.کم_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_بیت(msr_bitmap_region.کم_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_بیت(msr_bitmap_region.کم_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_بیت(msr_bitmap_region.کم_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_بیت(msr_bitmap_region.کم_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_بیت(msr_bitmap_region.کم_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_بیت(msr_bitmap_region.بالا_msr_read.bitmap, MSR_BITMAP_ماسک(IA32_FS_پایه_MSR));
    clear_بیت(msr_bitmap_region.بالا_msr_read.bitmap, MSR_BITMAP_ماسک(IA32_GS_پایه_MSR));
    clear_بیت(msr_bitmap_region.بالا_msr_read.bitmap, MSR_BITMAP_ماسک(IA32_KERNEL_GS_پایه_MSR));
    clear_بیت(msr_bitmap_region.بالا_msr_write.bitmap, MSR_BITMAP_ماسک(IA32_FS_پایه_MSR));
    clear_بیت(msr_bitmap_region.بالا_msr_write.bitmap, MSR_BITMAP_ماسک(IA32_GS_پایه_MSR));
    clear_بیت(msr_bitmap_region.بالا_msr_write.bitmap, MSR_BITMAP_ماسک(IA32_KERNEL_GS_پایه_MSR));
#پایان_اگر
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.واژگان[0] = x86_rdmsr_کم(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.واژگان[1] = x86_rdmsr_بالا(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    إذا (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        چاپ_کن("vt-x: Expected wb attribute for EPT paging structure\n");
        أرجع false;
    }
    إذا (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        چاپ_کن("vt-x: Expected supported for 2m pages\n");
        أرجع false;
    }

    أرجع true;
}

ساكن فراغ مجموعه_MRs_vmexit(uint32_t reason, کلمه_t qualification)
{
    کلمه_t *میانگیر;
    صحيح i;

    میانگیر = lookupIPCBuffer(true, گره_STATE(ksCurThread));

    مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_CALL_وقفه_INFO_MR,
          vmread(VMX_CONTROL_مدخل_INTERRUPTION_INFO));
    مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_FAULT_REASON_MR, reason);
    مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_داده_EXIT_INSTRUCTION_درازی));
    مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_داده_GUEST_PHYSICAL));
    مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    لكل (i = 0; i < n_vcpu_gp_ثبات; i++) {
        مجموعه_MR(گره_STATE(ksCurThread), میانگیر, SEL4_VMENTER_FAULT_EAX + i,
              گره_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

ساكن فراغ handleVmxFault(uint32_t reason, کلمه_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    مجموعه_ثبات(گره_STATE(ksCurThread), پیام_Info_ثبات, SEL4_VMENTER_نتیجه_FAULT);

    مجموعه_MRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

ساكن مضمن فراغ پایان_Vmexit_Saving(فراغ)
{
    vcpu_t *vcpu = ARCH_گره_STATE(x86KSCurrentVCPU);
    أكد(vcpu == گره_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(فراغ)
{
    uint32_t وقفه;
    /* qualification is host width, reason is defined as being 32 bit */
    کلمه_t qualification;
    uint32_t reason;
    پایان_Vmexit_Saving();
#اگر_تعریف_شده CONFIG_X86_64_VTX_64_بیت_GUESTS
    vcpu_restore_host_msrs();
#پایان_اگر
    /* the basic exit reason is the bottom 16 bits of the exit reason field */
    reason = vmread(VMX_داده_EXIT_REASON) & ماسک(16);
    إذا (reason == EXTERNAL_وقفه) {
        گره_قفل_IRQ;
        إذا (vmx_feature_ack_on_exit) {
            وقفه = vmread(VMX_داده_EXIT_وقفه_INFO) & 0xff;
            ARCH_گره_STATE(x86_KScur_وقفه) = وقفه;
            handle_وقفه_مدخل();
        } وإلا {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(گره_STATE(ksCurThread));
#پایان_اگر
        أرجع EXCEPTION_NONE;
    }

    گره_قفل_SYS;

    بدل (reason) {
    حالة EXCEPTION_OR_NMI:
    حالة MOV_DR:
    حالة TASK_SWITCH:
    حالة CONTROL_ثبات:
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
    حالة TPR_زیر_آستانه:
    حالة APIC_ACCESS:
        qualification = vmread(VMX_داده_EXIT_QUALIFICATION);
        اكسر;
    افتراضي:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    أرجع EXCEPTION_NONE;
}

exception_t handle_Vm_مدخل_Fail(فراغ)
{
    handleVmxFault(-1, -1);

    أرجع EXCEPTION_NONE;
}

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
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

        کلمه_t badge = notification_ptr_get_ntfn_پیام_Identifier(ntfnPtr);
        notification_ptr_مجموعه_state(ntfnPtr, NtfnState_Idle);
        مجموعه_Thread_State(tcb, ThreadState_Running);
        مجموعه_ثبات(tcb, badge_ثبات, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        إذا (tcb != گره_STATE(ksCurThread)) {
            possible_Switch_مقصد(tcb);
        }
    }
}
#پایان_اگر /* ENABLE_SMP_SUPPORT */

ساكن فراغ invvpid_context(uint16_t vpid)
{
    بنية {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t آدرس;
    } PACKED عملوند = {vpid, 0, 0};
    asm متطاير("invvpid %0, %1" :: "m"(عملوند), "r"((کلمه_t)1) : "cc");
}

ساكن فراغ مجموعه_EPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_ریشه;
    إذا (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_ریشه = kpptr_مقصد_paddr(null_ept_space);
    } وإلا {
        پیدا_کن_EPTFor_ASID_ret_t پیدا_کن_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_پایه_Ptr(vmxSpace);
        پیدا_کن_ret = پیدا_کن_EPTFor_ASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE || پیدا_کن_ret.ept != pml4) {
            ept_ریشه = kpptr_مقصد_paddr(null_ept_space);
        } وإلا {
            ept_ریشه = pptr_مقصد_paddr(pml4);
        }
    }
    إذا (ept_ریشه != vcpu->آخرین_ept_ریشه) {
        vcpu->آخرین_ept_ریشه = ept_ریشه;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_ریشه,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_اشاره_گر, eptp.واژگان[0]);
        أكد(vcpu->vpid != VPID_INVALID);
        إذا (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

فراغ clearVPIDIOPortMappings(vpid_t vpid, uint16_t اولی, uint16_t آخرین)
{
    إذا (vpid == VPID_INVALID) {
        أرجع;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    إذا (vcpu == عدم) {
        أرجع;
    }
    أكد(vcpu->vpid == vpid);
    مجموعه_IOPort_ماسک(vcpu->io, اولی, آخرین, true);
}

ساكن مضمن vpid_t بعدی_VPID(vpid_t vpid)
{
    إذا (vpid == VPID_آخرین) {
        أرجع VPID_اولی;
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

ساكن vpid_t پیدا_کن_Free_VPID(فراغ)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    افعل {
        إذا (x86KSVPIDTable[vpid] == عدم) {
            أرجع vpid;
        }
        vpid = بعدی_VPID(vpid);
    } طالما (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = بعدی_VPID(x86KSNextVPID);
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
فراغ vcpu_fpu_مقصد_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    إذا (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        إذا (!vcpu->fpu_فعال) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_فعال = true;
            بارگیری_Fpu_State(tcb);
        }
    } وإلا {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_فعال = true;
        switchLocalFpuOwner(tcb);
    }
}

فراغ vcpu_fpu_مقصد_host(tcb_t *tcb, vcpu_t *vcpu)
{
    إذا (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } وإلا إذا (nativeThreadUsingFPU(tcb) && vcpu->fpu_فعال) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_فعال = false;
        بارگیری_Fpu_State(tcb);
    } وإلا إذا (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

فراغ restoreVMCS(فراغ)
{
    tcb_t *cur_thread = گره_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    إذا (ARCH_گره_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#اگر_تعریف_نشده CONFIG_KERNEL_SKIM_پنجره
    إذا (getCurrentCR3().واژگان[0] != expected_vmcs->آخرین_host_cr3) {
        expected_vmcs->آخرین_host_cr3 = getCurrentCR3().واژگان[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().واژگان[0]);
    }
#پایان_اگر
    إذا (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = پیدا_کن_Free_VPID();
        storeVPID(expected_vmcs, vpid);
        إذا (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    مجموعه_EPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

فراغ invept(ept_pml4e_t *ept_pml4)
{
    إذا (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        بنية {
            uint64_t parts[2];
        } آدرس;
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

        آدرس.parts[0] = pptr_مقصد_paddr((فراغ *)ept_pml4);
        آدرس.parts[1] = 0;
        asm متطاير(
            "invept %0, %1"
            :
            : "m"(آدرس),  "r"(type)
            : "memory"
        );
    }
}

#پایان_اگر
