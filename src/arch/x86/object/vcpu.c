/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_VTX

#شامل <types.h>
#شامل <machine/io.h>
#شامل <api/failures.h>
#شامل <api/syscall.h>
#شامل <kernel/thread.h>
#شامل <object/objecttype.h>
#شامل <arch/machine/cpu_registers.h>
#شامل <arch/model/statedata.h>
#شامل <arch/object/vcpu.h>
#شامل <arch/object/ioport.h>
#شامل <util.h>
#شامل <sel4/arch/vmenter.h>

#تعریف_کرو VMX_EXIT_QUAL_TYPE_MOV_CR 0
#تعریف_کرو VMX_EXIT_QUAL_TYPE_CLTS 2
#تعریف_کرو VMX_EXIT_QUAL_TYPE_LMSW 3

ثابت vcpu_gp_رجسٹر_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#اگر_تعریف_شدہ CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#اگر_ختم /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#تعریف_کرو MSR_BITMAP_نقاب(x) ((x) & 0x1fff)

قسمتعریف ساخت msr_bitmap {
    لفظ_t bitmap[0x2000 / حجم(لفظ_t) / 8];
} msr_bitmap_t;

قسمتعریف ساخت msr_bitmaps {
    msr_bitmap_t کم_msr_read;
    msr_bitmap_t بلند_msr_read;
    msr_bitmap_t کم_msr_write;
    msr_bitmap_t بلند_msr_write;
} msr_bitmaps_t;

ساکن msr_bitmaps_t msr_bitmap_region ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_بٹ));

ساکن حرف null_ept_space[ثنائی_ہندسہ(se_L4_صفحہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_بٹ));

/* Cached value of the hardware defined vmcs revision */
ساکن uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
ساکن vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
ساکن uint32_t pin_control_بلند;
ساکن uint32_t pin_control_کم;
ساکن uint32_t primary_control_بلند;
ساکن uint32_t primary_control_کم;
ساکن uint32_t secondary_control_بلند;
ساکن uint32_t secondary_control_کم;
ساکن uint32_t اندراج_control_بلند;
ساکن uint32_t اندراج_control_کم;
ساکن uint32_t exit_control_بلند;
ساکن uint32_t exit_control_کم;
ساکن uint32_t cr0_بلند;
ساکن uint32_t cr0_کم;
ساکن uint32_t cr4_بلند;
ساکن uint32_t cr4_کم;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
ساکن bool_t vmx_feature_vpid;
ساکن bool_t vmx_feature_بھرو_perf_global_ctrl;
ساکن bool_t vmx_feature_ack_on_exit;

ساکن vcpu_t *x86KSVPIDTable[VPID_آخری + 1];
ساکن vpid_t x86KSNextVPID = VPID_پہلا;

ساکن خالی releaseVPID(vpid_t vpid);

ساکن اندرونی bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm متلون(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    واپس !!error;
}

ساکن خالی vmclear(خالی *vmcs_ptr)
{
    uint64_t physical_پتہ;
    physical_پتہ = pptr_مقصد_paddr((خالی *)vmcs_ptr);
    asm متلون(
        "vmclear %0"
        :
        : "m"(physical_پتہ)
        : "cc"
    );
}

خالی clearCurrentVCPU(خالی)
{
    vcpu_t *vcpu = ARCH_گرہ_STATE(x86KSCurrentVCPU);
    اگر (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_گرہ_STATE(x86KSCurrentVCPU) = تہی;
        اگر (vcpu->fpu_فعال && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_فعال = false;
        }
    }
}

ساکن خالی vmptrld(خالی *vmcs_ptr)
{
    uint64_t physical_پتہ;
    uint8_t error;
    physical_پتہ = pptr_مقصد_paddr(vmcs_ptr);
    asm متلون(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_پتہ)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    أكد(!error);
}

#اگر_تعریف_شدہ CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
خالی vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_ستارہ_MSR, vcpu->syscall_registers[VCPU_ستارہ]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_نقاب]);
}

خالی vcpu_restore_host_msrs(خالی)
{
    init_syscall_msrs();
}
#اگر_ختم

ساکن خالی switchVCPU(vcpu_t *vcpu)
{
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    اگر (vcpu->آخری_cpu != getCurrentCPUIndex() && ARCH_گرہ_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->آخری_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->آخری_cpu);
    }
#اگر_ختم
    clearCurrentVCPU();
    vmptrld(vcpu);
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    اگر (vcpu->آخری_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_بنیاد, (لفظ_t)&x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KStss);
        vmwrite(VMX_HOST_GDTR_بنیاد, (لفظ_t)x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_بنیاد, (لفظ_t)x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(لفظ_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KStss.tss.الفاظ[0] +
                                                          4));
    }
    vcpu->آخری_cpu = getCurrentCPUIndex();
#اگر_ختم
    ARCH_گرہ_STATE(x86KSCurrentVCPU) = vcpu;
}

ساکن خالی print_بٹ(لفظ_t بٹ)
{
    bool_t پہلا = true;
    جب (بٹ) {
        صحیح اشاریہ = se_L4_لفظ_بٹ - 1 - clzl(بٹ);
        اگر (پہلا) {
            لکھو("%d", اشاریہ);
            پہلا = false;
        } ورنہ {
            لکھو(",%d", اشاریہ);
        }
        بٹ &= ~ثنائی_ہندسہ(اشاریہ);
    }
}

ساکن bool_t check_fixed_قدر(لفظ_t val, لفظ_t کم, لفظ_t بلند)
{
    لفظ_t not_بلند;
    لفظ_t not_کم;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_بلند = بلند & ~(بلند & val);
    اگر (not_بلند != 0) {
        لکھو("Failed to set bits: ");
        print_بٹ(not_بلند);
        واپس false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_کم = ~کم & ~(~کم & ~val);
    اگر (not_کم != 0) {
        لکھو("Incorrectly cleared bits: ");
        print_بٹ(not_کم);
        واپس false;
    }
    واپس true;
}

ساکن bool_t vtx_check_fixed_اقدار(لفظ_t cr0, لفظ_t cr4)
{
    اگر (!check_fixed_قدر(cr0, cr0_کم, cr0_بلند)) {
        لکھو(" of CR0\n");
        واپس false;
    }
    اگر (!check_fixed_قدر(cr4, cr4_کم, cr4_بلند)) {
        لکھو(" of CR4\n");
        واپس false;
    }
    واپس true;
}

ساکن bool_t BOOT_رمز init_vtx_fixed_اقدار(bool_t useTrueMsrs)
{
    uint32_t pin_control_نقاب =
        ثنائی_ہندسہ(0) |    //Extern interrupt exiting
        ثنائی_ہندسہ(3) |    //NMI exiting
        ثنائی_ہندسہ(5);     //virtual NMIs
    uint32_t primary_control_نقاب =
        ثنائی_ہندسہ(25) |   //Use I/O bitmaps
        ثنائی_ہندسہ(28) |   //Use MSR bitmaps
        ثنائی_ہندسہ(31);    //Activate secondary controls
    uint32_t secondary_control_نقاب =
        ثنائی_ہندسہ(1);     //Enable EPT
    uint32_t exit_control_نقاب =
        ثنائی_ہندسہ(2)  |   //Save debug controls
        ثنائی_ہندسہ(18) |   //Save guest IA32_PAT on exit
        ثنائی_ہندسہ(19) |   //Load host IA32_PAT
        ثنائی_ہندسہ(20) |   //Save guest IA32_EFER on exit
        ثنائی_ہندسہ(21);    //Load host IA32_EFER
#اگر_تعریف_شدہ CONFIG_ARCH_X86_64
    exit_control_نقاب |= ثنائی_ہندسہ(9); //Host address-space size
#اگر_ختم /* CONFIG_ARCH_X86_64 */
    /* Read out the fixed high and low bits from the MSRs */
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t اندراج_ctls;
    اگر (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        اندراج_ctls = IA32_VMX_TRUE_اندراج_CTLS_MSR;
    } ورنہ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        اندراج_ctls = IA32_VMX_اندراج_CTLS_MSR;
    }
    pin_control_بلند = x86_rdmsr_کم(pinbased_ctls);
    pin_control_کم = x86_rdmsr_بلند(pinbased_ctls);
    primary_control_بلند = x86_rdmsr_کم(procbased_ctls);
    primary_control_کم = x86_rdmsr_بلند(procbased_ctls);
    secondary_control_بلند = x86_rdmsr_کم(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_کم = x86_rdmsr_بلند(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_بلند = x86_rdmsr_کم(exit_ctls);
    exit_control_کم = x86_rdmsr_بلند(exit_ctls);
    اندراج_control_بلند = x86_rdmsr_کم(اندراج_ctls);
    اندراج_control_کم = x86_rdmsr_بلند(اندراج_ctls);

    cr0_بلند = x86_rdmsr_کم(IA32_VMX_CR0_FIXED0_MSR);
    cr0_کم = x86_rdmsr_کم(IA32_VMX_CR0_FIXED1_MSR);
    cr4_بلند = x86_rdmsr_کم(IA32_VMX_CR4_FIXED0_MSR);
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
    اگر (secondary_control_کم & ثنائی_ہندسہ(7)) {
        cr0_بلند &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    اگر (!(secondary_control_کم & ثنائی_ہندسہ(5))) {
        vmx_feature_vpid = 0;
        لکھو("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } ورنہ {
        vmx_feature_vpid = 1;
        secondary_control_نقاب |= ثنائی_ہندسہ(5);
    }

    /* Check for load perf global control */
    اگر (!(exit_control_کم & ثنائی_ہندسہ(12))) {
        vmx_feature_بھرو_perf_global_ctrl = 0;
        لکھو("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } ورنہ {
        vmx_feature_بھرو_perf_global_ctrl = 1;
        exit_control_نقاب |= ثنائی_ہندسہ(12);
    }

    /* Check for external interrupt exiting */
    اگر (!(exit_control_کم & ثنائی_ہندسہ(15))) {
        vmx_feature_ack_on_exit = 0;
        لکھو("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } ورنہ {
        vmx_feature_ack_on_exit = 1;
        exit_control_نقاب |= ثنائی_ہندسہ(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_کم) & pin_control_نقاب;
    اگر (missing) {
        لکھو("vt-x: Unsupported pin control features %lx\n", (طویل)missing);
        واپس false;
    }
    missing = (~primary_control_کم) & primary_control_نقاب;
    اگر (missing) {
        لکھو("vt-x: Unsupported primary control features %lx\n", (طویل)missing);
        واپس false;
    }
    missing = (~secondary_control_کم) & secondary_control_نقاب;
    اگر (missing) {
        لکھو("vt-x: Unsupported secondary control features %lx\n", (طویل)missing);
        واپس false;
    }
    missing = (~exit_control_کم) & exit_control_نقاب;
    اگر (missing) {
        لکھو("vt-x: Unsupported exit control features %lx\n", (طویل)missing);
        واپس false;
    }
#اگر_تعریف_شدہ CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
    missing = (~اندراج_control_کم) & ثنائی_ہندسہ(9);
    اگر (missing) {
        لکھو("vt-x: CPU does not support 64-bit guests\n");
        واپس false;
    }
#اگر_ختم /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_بلند |= pin_control_نقاب;
    primary_control_بلند |= primary_control_نقاب;
    secondary_control_بلند |= secondary_control_نقاب;
    exit_control_بلند |= exit_control_نقاب;

    واپس true;
}

ساکن bool_t BOOT_رمز check_vtx_fixed_اقدار(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t اندراج_ctls;
    اگر (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        اندراج_ctls = IA32_VMX_TRUE_اندراج_CTLS_MSR;
    } ورنہ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        اندراج_ctls = IA32_VMX_اندراج_CTLS_MSR;
    }
    uint32_t local_pin_control_بلند = x86_rdmsr_کم(pinbased_ctls);
    uint32_t local_pin_control_کم = x86_rdmsr_بلند(pinbased_ctls);
    uint32_t local_primary_control_بلند = x86_rdmsr_کم(procbased_ctls);
    uint32_t local_primary_control_کم = x86_rdmsr_بلند(procbased_ctls);
    uint32_t local_secondary_control_بلند = x86_rdmsr_کم(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_کم = x86_rdmsr_بلند(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_بلند = x86_rdmsr_کم(exit_ctls);
    uint32_t local_exit_control_کم = x86_rdmsr_بلند(exit_ctls);
    uint32_t local_اندراج_control_بلند = x86_rdmsr_کم(اندراج_ctls);
    uint32_t local_اندراج_control_کم = x86_rdmsr_بلند(اندراج_ctls);

    uint32_t local_cr0_بلند = x86_rdmsr_کم(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_کم = x86_rdmsr_کم(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_بلند = x86_rdmsr_کم(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_کم = x86_rdmsr_کم(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    اگر (local_secondary_control_کم & ثنائی_ہندسہ(7)) {
        local_cr0_بلند &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    واپس
        (local_pin_control_بلند & pin_control_بلند) == local_pin_control_بلند &&
        (~local_pin_control_کم & pin_control_بلند) == 0 &&
        (local_primary_control_بلند & primary_control_بلند) == local_primary_control_بلند &&
        (~local_primary_control_کم & primary_control_بلند) == 0 &&
        (local_secondary_control_بلند & secondary_control_بلند) == local_secondary_control_بلند &&
        (~local_secondary_control_کم & secondary_control_بلند) == 0 &&
        (local_exit_control_بلند & exit_control_بلند) == local_exit_control_بلند &&
        (~local_exit_control_کم & exit_control_بلند) == 0 &&
        (local_اندراج_control_بلند & اندراج_control_بلند) == local_اندراج_control_بلند &&
        (~local_اندراج_control_کم & اندراج_control_بلند) == 0 &&
        local_cr0_بلند == cr0_بلند &&
        local_cr0_کم == cr0_کم &&
        local_cr4_بلند == cr4_بلند &&
        local_cr4_کم == cr4_کم;
}

ساکن اندرونی uint32_t apply_Fixed_بٹ(uint32_t original, uint32_t بلند, uint32_t کم)
{
    original |= بلند;
    original &= کم;
    واپس original;
}

خالی vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = تہی;
    vcpu->launched = false;
    vcpu->fpu_فعال = false;
    vcpu->fpuState = x86KSnullFpuState;

    حافظہ_نقل(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_بلند & cr0_کم;
    vcpu->cr0_shadow = 0;
    vcpu->cr0_نقاب = 0;
    vcpu->exception_bitmap = 0;
    vcpu->vpid = VPID_INVALID;
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    vcpu->آخری_cpu = getCurrentCPUIndex();
#اگر_ختم /* ENABLE_SMP_SUPPORT */

    switchVCPU(vcpu);

    vmwrite(VMX_HOST_PAT, x86_rdmsr(IA32_PAT_MSR));
    vmwrite(VMX_HOST_EFER, x86_rdmsr(IA32_EFER_MSR));
    // By default we will disable performance counters when we come out
    // of a VM. When performance counters are supported this host state
    // needs to be updated on VM entry
    اگر (vmx_feature_بھرو_perf_global_ctrl) {
        vmwrite(VMX_HOST_PERF_GLOBAL_CTRL, 0);
    }
    vmwrite(VMX_HOST_CR0, read_cr0());
    vmwrite(VMX_HOST_CR4, read_cr4());
    vmwrite(VMX_HOST_FS_بنیاد, 0);
    vmwrite(VMX_HOST_GS_بنیاد, 0);
    vmwrite(VMX_HOST_TR_بنیاد, (لفظ_t)&x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KStss);
    vmwrite(VMX_HOST_GDTR_بنیاد, (لفظ_t)x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KSgdt);
    vmwrite(VMX_HOST_IDTR_بنیاد, (لفظ_t)x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KSidt);
    vmwrite(VMX_HOST_SYSENTER_CS, (لفظ_t)SEL_CS_0);
    vmwrite(VMX_HOST_SYSENTER_EIP, (لفظ_t)&handle_syscall);
    اگر (!config_سیٹ(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(لفظ_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KStss.tss.الفاظ[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (لفظ_t)&vcpu->gp_registers[n_vcpu_gp_رجسٹر]);
    vmwrite(VMX_HOST_RIP, (لفظ_t)&handle_vmexit);
#اگر_تعریف_شدہ CONFIG_KERNEL_SKIM_کھڑکی
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_مقصد_paddr(x64KSKernelPML4), 0).الفاظ[0]);
#اگر_ختم /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_بلند & pin_control_کم);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_بلند & primary_control_کم);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_بلند & secondary_control_کم);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_بلند & exit_control_کم);

    uint64_t اندراج_control = اندراج_control_بلند & اندراج_control_کم;
#اگر_تعریف_شدہ CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    اندراج_control |= ثنائی_ہندسہ(9);
#اگر_ختم /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_اندراج_CONTROLS, اندراج_control);

    vmwrite(VMX_CONTROL_MSR_پتہ, (لفظ_t)kpptr_مقصد_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_بلند & cr4_کم);

    vmwrite(VMX_GUEST_VMCS_LINK_اشارہ_گر, ~(لفظ_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_اشارہ_گر_بلند, ~(لفظ_t)0);

    memset(vcpu->io, ~(لفظ_t)0, VCPU_IOBITMAP_سائز);
    vmwrite(VMX_CONTROL_IOA_پتہ, pptr_مقصد_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_پتہ, pptr_مقصد_paddr((حرف *)vcpu->io + (VCPU_IOBITMAP_سائز / 2)));
}

ساکن خالی dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    أكد(tcb->tcbArch.tcbVCPU == vcpu);
    أكد(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_فعال = false;
    tcb->tcbArch.tcbVCPU = تہی;
    vcpu->vcpuTCB = تہی;
}

خالی vcpu_finalise(vcpu_t *vcpu)
{
    اگر (ARCH_گرہ_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->آخری_cpu) == vcpu) {
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
        اگر (vcpu->آخری_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->آخری_cpu);
        } ورنہ
#اگر_ختم /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    اگر (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    اگر (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

ساکن خالی associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    اگر (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    اگر (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

ساکن exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, لفظ_t *بفر)
{
    صحیح i;
    برائے (i = 0; i < n_vcpu_gp_رجسٹر; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, بفر);
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeVCPUWriteRegisters(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
#اگر_تعریف_شدہ CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
    اگر (لمبائی < 15) {
#ورنہ
    اگر (لمبائی < 7) {
#اگر_ختم /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    واپس invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), بفر);
}

#اگر_تعریف_شدہ CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
ساکن exception_t invokeWriteMSR(vcpu_t *vcpu, لفظ_t *بفر, لفظ_t میدان, لفظ_t قدر)
{
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    اگر (ARCH_گرہ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    انتخاب (میدان) {
    حالت IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = قدر;
        توڑو;
    حالت IA32_ستارہ_MSR:
        vcpu->syscall_registers[VCPU_ستارہ] = قدر;
        توڑو;
    حالت IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = قدر;
        توڑو;
    حالت IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_نقاب] = قدر;
        توڑو;
    }
    سیٹ_MR(thread, بفر, 0, قدر);
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeVCPUWriteMSR(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    لفظ_t میدان;
    لفظ_t قدر;

    اگر (لمبائی < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    میدان = getSyscallArg(0, بفر);
    قدر = getSyscallArg(1, بفر);
    انتخاب (میدان) {
    حالت IA32_LSTAR_MSR:
    حالت IA32_ستارہ_MSR:
    حالت IA32_CSTAR_MSR:
    حالت IA32_FMASK_MSR:
        توڑو;
    طےشدہ:
        userError("VCPU WriteMSR: Invalid field %lx.", (طویل)میدان);
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    واپس invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), بفر, میدان, قدر);
}

ساکن exception_t invokeReadMSR(vcpu_t *vcpu, لفظ_t میدان, لفظ_t *بفر)
{
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);

    لفظ_t قدر = 0;

    انتخاب (میدان) {
    حالت IA32_LSTAR_MSR:
        قدر = vcpu->syscall_registers[VCPU_LSTAR];
        توڑو;
    حالت IA32_ستارہ_MSR:
        قدر = vcpu->syscall_registers[VCPU_ستارہ];
        توڑو;
    حالت IA32_CSTAR_MSR:
        قدر = vcpu->syscall_registers[VCPU_CSTAR];
        توڑو;
    حالت IA32_FMASK_MSR:
        قدر = vcpu->syscall_registers[VCPU_SYSCALL_نقاب];
        توڑو;
    حالت IA32_VMX_MISC_MSR:
        قدر = x86_rdmsr(میدان);
        توڑو;
    }

    سیٹ_MR(thread, بفر, 0, قدر);
    سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                    se_L4_پیغام_Info_new(0, 0, 0, 1)));
    سیٹ_Thread_State(thread, ThreadState_Restart);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeVCPUReadMSR(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    اگر (لمبائی < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    لفظ_t میدان = getSyscallArg(0, بفر);
    انتخاب (میدان) {
    حالت IA32_LSTAR_MSR:
    حالت IA32_ستارہ_MSR:
    حالت IA32_CSTAR_MSR:
    حالت IA32_FMASK_MSR:
    حالت IA32_VMX_MISC_MSR:
        توڑو;
    طےشدہ:
        userError("VCPU ReadMSR: Invalid field %lx.", (طویل)میدان);
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    واپس invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), میدان, بفر);
}
#اگر_ختم  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

ساکن exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *خانہ, cap_t cap, uint16_t کم, uint16_t بلند)
{
    /* remove any existing io ports from this cap */
    clearVPIDIOPortMappings(cap_io_پورٹ_cap_get_cap_IOPort_VPID(cap),
                            cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap),
                            cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap));
    /* update the assigned vpid. If the vcpu does not have a valid vpid then
     * this is fine as whilst the cap will not point to the vcpu, the vcpu
     * will have its port mask cleared when it gets assigned a vpid */
    cap = cap_io_پورٹ_cap_سیٹ_cap_IOPort_VPID(cap, vcpu->vpid);
    خانہ->cap = cap;
    سیٹ_IOPort_نقاب(vcpu->io, کم, بلند, false);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeEnableIOPort(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *io_خانہ;
    uint16_t کم, بلند;

    اگر (لمبائی < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (current_extra_caps.excaprefs[0] == تہی) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    io_خانہ = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(ioCap) != cap_io_پورٹ_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    کم = getSyscallArg(0, بفر);
    بلند = getSyscallArg(1, بفر);

    اگر (کم < cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(ioCap) || بلند > cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeEnableIOPort(vcpu, io_خانہ, ioCap, کم, بلند);
}

ساکن exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t کم, uint16_t بلند)
{
    سیٹ_IOPort_نقاب(vcpu->io, کم, بلند, true);
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeDisableIOPort(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    vcpu_t *vcpu;
    uint16_t کم, بلند;

    اگر (لمبائی < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    کم = getSyscallArg(0, بفر);
    بلند = getSyscallArg(1, بفر);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    واپس invokeDisableIOPort(vcpu, کم, بلند);
}

ساکن exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, لفظ_t *بفر, لفظ_t میدان, لفظ_t قدر)
{
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);

    اگر (ARCH_گرہ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    انتخاب (میدان) {
    حالت VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = قدر;
        توڑو;
    حالت VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = قدر;
        توڑو;
    حالت VMX_CONTROL_CR0_نقاب:
        vcpu->cr0_نقاب = vcpu->cached_cr0_نقاب = قدر;
        توڑو;
    حالت VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = قدر;
        توڑو;
    }
    vmwrite(میدان, قدر);

    اگر (call) {
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        غیرعلامتی صحیح لمبائی = سیٹ_MR(thread, بفر, 0, قدر);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, لمبائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeWriteVMCS(cap_t cap, لفظ_t لمبائی, bool_t call, لفظ_t *بفر)
{
    لفظ_t میدان;
    لفظ_t قدر;

    اگر (لمبائی < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    میدان = getSyscallArg(0, بفر);
    قدر = getSyscallArg(1, بفر);
    انتخاب (میدان) {
    حالت VMX_GUEST_RIP:
    حالت VMX_GUEST_RSP:
    حالت VMX_GUEST_ES_SELECTOR:
    حالت VMX_GUEST_CS_SELECTOR:
    حالت VMX_GUEST_SS_SELECTOR:
    حالت VMX_GUEST_DS_SELECTOR:
    حالت VMX_GUEST_FS_SELECTOR:
    حالت VMX_GUEST_GS_SELECTOR:
    حالت VMX_GUEST_LDTR_SELECTOR:
    حالت VMX_GUEST_TR_SELECTOR:
    حالت VMX_GUEST_DEBUGCTRL:
    حالت VMX_GUEST_PAT:
    حالت VMX_GUEST_EFER:
    حالت VMX_GUEST_PERF_GLOBAL_CTRL:
    حالت VMX_GUEST_PDPTE0:
    حالت VMX_GUEST_PDPTE1:
    حالت VMX_GUEST_PDPTE2:
    حالت VMX_GUEST_PDPTE3:
    حالت VMX_GUEST_ES_حد_بندی:
    حالت VMX_GUEST_CS_حد_بندی:
    حالت VMX_GUEST_SS_حد_بندی:
    حالت VMX_GUEST_DS_حد_بندی:
    حالت VMX_GUEST_FS_حد_بندی:
    حالت VMX_GUEST_GS_حد_بندی:
    حالت VMX_GUEST_LDTR_حد_بندی:
    حالت VMX_GUEST_TR_حد_بندی:
    حالت VMX_GUEST_GDTR_حد_بندی:
    حالت VMX_GUEST_IDTR_حد_بندی:
    حالت VMX_GUEST_ES_ACCESS_RIGHTS:
    حالت VMX_GUEST_CS_ACCESS_RIGHTS:
    حالت VMX_GUEST_SS_ACCESS_RIGHTS:
    حالت VMX_GUEST_DS_ACCESS_RIGHTS:
    حالت VMX_GUEST_FS_ACCESS_RIGHTS:
    حالت VMX_GUEST_GS_ACCESS_RIGHTS:
    حالت VMX_GUEST_LDTR_ACCESS_RIGHTS:
    حالت VMX_GUEST_TR_ACCESS_RIGHTS:
    حالت VMX_GUEST_INTERRUPTABILITY:
    حالت VMX_GUEST_ACTIVITY:
    حالت VMX_GUEST_SMBASE:
    حالت VMX_GUEST_SYSENTER_CS:
    حالت VMX_GUEST_PREEMPTION_TIMER_قدر:
    حالت VMX_GUEST_ES_بنیاد:
    حالت VMX_GUEST_CS_بنیاد:
    حالت VMX_GUEST_SS_بنیاد:
    حالت VMX_GUEST_DS_بنیاد:
    حالت VMX_GUEST_FS_بنیاد:
    حالت VMX_GUEST_GS_بنیاد:
    حالت VMX_GUEST_LDTR_بنیاد:
    حالت VMX_GUEST_TR_بنیاد:
    حالت VMX_GUEST_GDTR_بنیاد:
    حالت VMX_GUEST_IDTR_بنیاد:
    حالت VMX_GUEST_DR7:
    حالت VMX_GUEST_RFLAGS:
    حالت VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالت VMX_GUEST_SYSENTER_ESP:
    حالت VMX_GUEST_SYSENTER_EIP:
    حالت VMX_CONTROL_CR0_نقاب:
    حالت VMX_CONTROL_CR4_نقاب:
    حالت VMX_CONTROL_CR0_READ_SHADOW:
    حالت VMX_CONTROL_CR4_READ_SHADOW:
    حالت VMX_GUEST_CR3:
    حالت VMX_CONTROL_EXCEPTION_BITMAP:
    حالت VMX_CONTROL_اندراج_INTERRUPTION_INFO:
    حالت VMX_CONTROL_اندراج_EXCEPTION_ERROR_رمز:
        توڑو;
    حالت VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        قدر = apply_Fixed_بٹ(قدر, pin_control_بلند, pin_control_کم);
        توڑو;
    حالت VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        قدر = apply_Fixed_بٹ(قدر, primary_control_بلند, primary_control_کم);
        توڑو;
    حالت VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        قدر = apply_Fixed_بٹ(قدر, secondary_control_بلند, secondary_control_کم);
        توڑو;
    حالت VMX_CONTROL_اندراج_CONTROLS:
        قدر = apply_Fixed_بٹ(قدر, اندراج_control_بلند, اندراج_control_کم);
        توڑو;
    حالت VMX_CONTROL_EXIT_CONTROLS:
        قدر = apply_Fixed_بٹ(قدر, exit_control_بلند, exit_control_کم);
        توڑو;
    حالت VMX_GUEST_CR0:
        قدر = apply_Fixed_بٹ(قدر, cr0_بلند, cr0_کم);
        توڑو;
    حالت VMX_GUEST_CR4:
        قدر = apply_Fixed_بٹ(قدر, cr4_بلند, cr4_کم);
        توڑو;
    طےشدہ:
        userError("VCPU WriteVMCS: Invalid field %lx.", (طویل)میدان);
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    واپس invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, بفر, میدان, قدر);
}

ساکن لفظ_t readVMCSField(vcpu_t *vcpu, لفظ_t میدان)
{
    انتخاب (میدان) {
    حالت VMX_CONTROL_EXCEPTION_BITMAP:
        واپس vcpu->exception_bitmap;
    حالت VMX_GUEST_CR0:
        واپس vcpu->cr0;
    حالت VMX_CONTROL_CR0_نقاب:
        واپس vcpu->cr0_نقاب;
    حالت VMX_CONTROL_CR0_READ_SHADOW:
        واپس vcpu->cr0_shadow;
    }
    اگر (ARCH_گرہ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    واپس vmread(میدان);
}

ساکن exception_t invokeReadVMCS(vcpu_t *vcpu, لفظ_t میدان, bool_t call, لفظ_t *بفر)
{
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    لفظ_t قدر = readVMCSField(vcpu, میدان);
    اگر (call) {
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        غیرعلامتی صحیح لمبائی = سیٹ_MR(thread, بفر, 0, قدر);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, لمبائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeReadVMCS(cap_t cap, لفظ_t لمبائی, bool_t call, لفظ_t *بفر)
{
    اگر (لمبائی < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    لفظ_t میدان = getSyscallArg(0, بفر);
    انتخاب (میدان) {
    حالت VMX_GUEST_RIP:
    حالت VMX_GUEST_RSP:
    حالت VMX_GUEST_ES_SELECTOR:
    حالت VMX_GUEST_CS_SELECTOR:
    حالت VMX_GUEST_SS_SELECTOR:
    حالت VMX_GUEST_DS_SELECTOR:
    حالت VMX_GUEST_FS_SELECTOR:
    حالت VMX_GUEST_GS_SELECTOR:
    حالت VMX_GUEST_LDTR_SELECTOR:
    حالت VMX_GUEST_TR_SELECTOR:
    حالت VMX_GUEST_DEBUGCTRL:
    حالت VMX_GUEST_PAT:
    حالت VMX_GUEST_EFER:
    حالت VMX_GUEST_PERF_GLOBAL_CTRL:
    حالت VMX_GUEST_PDPTE0:
    حالت VMX_GUEST_PDPTE1:
    حالت VMX_GUEST_PDPTE2:
    حالت VMX_GUEST_PDPTE3:
    حالت VMX_GUEST_ES_حد_بندی:
    حالت VMX_GUEST_CS_حد_بندی:
    حالت VMX_GUEST_SS_حد_بندی:
    حالت VMX_GUEST_DS_حد_بندی:
    حالت VMX_GUEST_FS_حد_بندی:
    حالت VMX_GUEST_GS_حد_بندی:
    حالت VMX_GUEST_LDTR_حد_بندی:
    حالت VMX_GUEST_TR_حد_بندی:
    حالت VMX_GUEST_GDTR_حد_بندی:
    حالت VMX_GUEST_IDTR_حد_بندی:
    حالت VMX_GUEST_ES_ACCESS_RIGHTS:
    حالت VMX_GUEST_CS_ACCESS_RIGHTS:
    حالت VMX_GUEST_SS_ACCESS_RIGHTS:
    حالت VMX_GUEST_DS_ACCESS_RIGHTS:
    حالت VMX_GUEST_FS_ACCESS_RIGHTS:
    حالت VMX_GUEST_GS_ACCESS_RIGHTS:
    حالت VMX_GUEST_LDTR_ACCESS_RIGHTS:
    حالت VMX_GUEST_TR_ACCESS_RIGHTS:
    حالت VMX_GUEST_INTERRUPTABILITY:
    حالت VMX_GUEST_ACTIVITY:
    حالت VMX_GUEST_SMBASE:
    حالت VMX_GUEST_SYSENTER_CS:
    حالت VMX_GUEST_PREEMPTION_TIMER_قدر:
    حالت VMX_GUEST_ES_بنیاد:
    حالت VMX_GUEST_CS_بنیاد:
    حالت VMX_GUEST_SS_بنیاد:
    حالت VMX_GUEST_DS_بنیاد:
    حالت VMX_GUEST_FS_بنیاد:
    حالت VMX_GUEST_GS_بنیاد:
    حالت VMX_GUEST_LDTR_بنیاد:
    حالت VMX_GUEST_TR_بنیاد:
    حالت VMX_GUEST_GDTR_بنیاد:
    حالت VMX_GUEST_IDTR_بنیاد:
    حالت VMX_GUEST_DR7:
    حالت VMX_GUEST_RFLAGS:
    حالت VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالت VMX_GUEST_SYSENTER_ESP:
    حالت VMX_GUEST_SYSENTER_EIP:
    حالت VMX_CONTROL_CR0_نقاب:
    حالت VMX_CONTROL_CR4_نقاب:
    حالت VMX_CONTROL_CR0_READ_SHADOW:
    حالت VMX_CONTROL_CR4_READ_SHADOW:
    حالت VMX_ڈیٹا_INSTRUCTION_ERROR:
    حالت VMX_ڈیٹا_EXIT_مداخلت_INFO:
    حالت VMX_ڈیٹا_EXIT_مداخلت_ERROR:
    حالت VMX_ڈیٹا_IDT_سمتیہ_INFO:
    حالت VMX_ڈیٹا_IDT_سمتیہ_ERROR:
    حالت VMX_ڈیٹا_EXIT_INSTRUCTION_لمبائی:
    حالت VMX_ڈیٹا_EXIT_INSTRUCTION_INFO:
    حالت VMX_ڈیٹا_GUEST_PHYSICAL:
    حالت VMX_ڈیٹا_IO_RCX:
    حالت VMX_ڈیٹا_IO_RSI:
    حالت VMX_ڈیٹا_IO_RDI:
    حالت VMX_ڈیٹا_IO_RIP:
    حالت VMX_ڈیٹا_GUEST_خطی_پتہ:
    حالت VMX_CONTROL_اندراج_INTERRUPTION_INFO:
    حالت VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    حالت VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    حالت VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    حالت VMX_CONTROL_EXCEPTION_BITMAP:
    حالت VMX_CONTROL_اندراج_CONTROLS:
    حالت VMX_CONTROL_EXIT_CONTROLS:
    حالت VMX_GUEST_CR0:
    حالت VMX_GUEST_CR3:
    حالت VMX_GUEST_CR4:
        توڑو;
    طےشدہ:
        userError("VCPU ReadVMCS: Invalid field %lx.", (طویل)میدان);
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    واپس invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), میدان, call, بفر);
}

ساکن exception_t invoke_سیٹ_TCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس EXCEPTION_NONE;
}

ساکن exception_t decode_سیٹ_TCB(cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    cap_t tcbCap;
    اگر (current_extra_caps.excaprefs[0] == تہی) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    واپس invoke_سیٹ_TCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

خالی vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    لفظ_t *بفر;
    اگر (ARCH_گرہ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بفر = lookupIPCBuffer(false, گرہ_STATE(ksCurThread));
    اگر (!بفر) {
        userError("No IPC buffer.");
        واپس;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, بفر));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, apply_Fixed_بٹ(getSyscallArg(1, بفر), primary_control_بلند,
                                                                   primary_control_کم));
    vmwrite(VMX_CONTROL_اندراج_INTERRUPTION_INFO, getSyscallArg(2, بفر));
}

خالی vcpu_sysvmenter_جواب_مقصد_user(tcb_t *tcb)
{
    لفظ_t *بفر;
    vcpu_t *vcpu;

    بفر = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    أكد(vcpu);

    اگر (ARCH_گرہ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    سیٹ_MR(tcb, بفر, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    سیٹ_MR(tcb, بفر, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    سیٹ_MR(tcb, بفر, SEL4_VMENTER_CALL_مداخلت_INFO_MR,
          vmread(VMX_CONTROL_اندراج_INTERRUPTION_INFO));
    سیٹ_رجسٹر(tcb, پیغام_Info_رجسٹر, 0);
}

exception_t decodeX86VCPUInvocation(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *خانہ,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    انتخاب (invLabel) {
    حالت X86VCPUSetTCB:
        واپس decode_سیٹ_TCB(cap, لمبائی, بفر);
    حالت X86VCPUReadVMCS:
        واپس decodeReadVMCS(cap, لمبائی, call, بفر);
    حالت X86VCPUWriteVMCS:
        واپس decodeWriteVMCS(cap, لمبائی, call, بفر);
    حالت X86VCPUEnableIOPort:
        واپس decodeEnableIOPort(cap, لمبائی, بفر);
    حالت X86VCPUDisableIOPort:
        واپس decodeDisableIOPort(cap, لمبائی, بفر);
    حالت X86VCPUWriteRegisters:
        واپس decodeVCPUWriteRegisters(cap, لمبائی, بفر);
#اگر_تعریف_شدہ CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
    حالت X86VCPUWriteMSR:
        واپس decodeVCPUWriteMSR(cap, لمبائی, بفر);
    حالت X86VCPUReadMSR:
        واپس decodeVCPUReadMSR(cap, لمبائی, بفر);
#اگر_ختم /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    طےشدہ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}

ساکن bool_t is_vtx_supported(خالی)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    واپس !!(x86_cpuid_ecx(0x1, 0) & ثنائی_ہندسہ(5));
}

ساکن اندرونی خالی clear_ثنائی_ہندسہ(لفظ_t *bitmap, لفظ_t ثنائی_ہندسہ_2)
{
    صحیح اشاریہ = ثنائی_ہندسہ_2 / (حجم(لفظ_t) * 8);
    صحیح انحراف = ثنائی_ہندسہ_2 % (حجم(لفظ_t) * 8);
    bitmap[اشاریہ] &= ~ثنائی_ہندسہ(انحراف);
}

BOOT_رمز bool_t vtx_init(خالی)
{
    اگر (!is_vtx_supported()) {
        لکھو("vt-x: not supported\n");
        واپس false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.الفاظ[0] = x86_rdmsr_کم(IA32_VMX_BASIC_MSR);
    vmx_basic.الفاظ[1] = x86_rdmsr_بلند(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.الفاظ[0] = x86_rdmsr_کم(IA32_FEATURE_CONTROL_MSR);
    اگر (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        اگر (feature_control_msr_get_تالا(feature_control)) {
            لکھو("vt-x: feature locked\n");
            واپس false;
        }
        feature_control = feature_control_msr_سیٹ_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_بلند(IA32_FEATURE_CONTROL_MSR), feature_control.الفاظ[0]);
    }
    /* make sure the msr is locked */
    اگر (!feature_control_msr_get_تالا(feature_control)) {
        feature_control = feature_control_msr_سیٹ_تالا(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_بلند(IA32_FEATURE_CONTROL_MSR), feature_control.الفاظ[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    اگر (CURRENT_CPU_اشاریہ() == 0) {
        اگر (!init_vtx_fixed_اقدار(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            لکھو("vt-x: lack of required features\n");
            واپس false;
        }
    }
    اگر (!check_vtx_fixed_اقدار(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        لکھو("vt-x: cores have inconsistent features\n");
        واپس false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    ساخت vmxon_region *vmxon_kptr = &ARCH_گرہ_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    اگر (!vtx_check_fixed_اقدار(read_cr0(), read_cr4())) {
        واپس false;
    }
    اگر (vmxon(kpptr_مقصد_paddr(vmxon_kptr))) {
        لکھو("vt-x: vmxon failure\n");
        واپس false;
    }
    memset(&msr_bitmap_region, ~0, حجم(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_ثنائی_ہندسہ(msr_bitmap_region.کم_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_ثنائی_ہندسہ(msr_bitmap_region.کم_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_ثنائی_ہندسہ(msr_bitmap_region.کم_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_ثنائی_ہندسہ(msr_bitmap_region.کم_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_ثنائی_ہندسہ(msr_bitmap_region.کم_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_ثنائی_ہندسہ(msr_bitmap_region.کم_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#اگر_تعریف_شدہ CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_ثنائی_ہندسہ(msr_bitmap_region.بلند_msr_read.bitmap, MSR_BITMAP_نقاب(IA32_FS_بنیاد_MSR));
    clear_ثنائی_ہندسہ(msr_bitmap_region.بلند_msr_read.bitmap, MSR_BITMAP_نقاب(IA32_GS_بنیاد_MSR));
    clear_ثنائی_ہندسہ(msr_bitmap_region.بلند_msr_read.bitmap, MSR_BITMAP_نقاب(IA32_KERNEL_GS_بنیاد_MSR));
    clear_ثنائی_ہندسہ(msr_bitmap_region.بلند_msr_write.bitmap, MSR_BITMAP_نقاب(IA32_FS_بنیاد_MSR));
    clear_ثنائی_ہندسہ(msr_bitmap_region.بلند_msr_write.bitmap, MSR_BITMAP_نقاب(IA32_GS_بنیاد_MSR));
    clear_ثنائی_ہندسہ(msr_bitmap_region.بلند_msr_write.bitmap, MSR_BITMAP_نقاب(IA32_KERNEL_GS_بنیاد_MSR));
#اگر_ختم
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.الفاظ[0] = x86_rdmsr_کم(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.الفاظ[1] = x86_rdmsr_بلند(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    اگر (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        لکھو("vt-x: Expected wb attribute for EPT paging structure\n");
        واپس false;
    }
    اگر (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        لکھو("vt-x: Expected supported for 2m pages\n");
        واپس false;
    }

    واپس true;
}

ساکن خالی سیٹ_MRs_vmexit(uint32_t reason, لفظ_t qualification)
{
    لفظ_t *بفر;
    صحیح i;

    بفر = lookupIPCBuffer(true, گرہ_STATE(ksCurThread));

    سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_CALL_مداخلت_INFO_MR,
          vmread(VMX_CONTROL_اندراج_INTERRUPTION_INFO));
    سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_REASON_MR, reason);
    سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_ڈیٹا_EXIT_INSTRUCTION_لمبائی));
    سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_ڈیٹا_GUEST_PHYSICAL));
    سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    برائے (i = 0; i < n_vcpu_gp_رجسٹر; i++) {
        سیٹ_MR(گرہ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_EAX + i,
              گرہ_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

ساکن خالی handleVmxFault(uint32_t reason, لفظ_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    سیٹ_رجسٹر(گرہ_STATE(ksCurThread), پیغام_Info_رجسٹر, SEL4_VMENTER_نتیجہ_FAULT);

    سیٹ_MRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

ساکن اندرونی خالی اختتام_Vmexit_Saving(خالی)
{
    vcpu_t *vcpu = ARCH_گرہ_STATE(x86KSCurrentVCPU);
    أكد(vcpu == گرہ_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(خالی)
{
    uint32_t مداخلت;
    /* qualification is host width, reason is defined as being 32 bit */
    لفظ_t qualification;
    uint32_t reason;
    اختتام_Vmexit_Saving();
#اگر_تعریف_شدہ CONFIG_X86_64_VTX_64_ثنائی_ہندسہ_GUESTS
    vcpu_restore_host_msrs();
#اگر_ختم
    /* the basic exit reason is the bottom 16 bits of the exit reason field */
    reason = vmread(VMX_ڈیٹا_EXIT_REASON) & نقاب(16);
    اگر (reason == EXTERNAL_مداخلت) {
        گرہ_تالا_IRQ;
        اگر (vmx_feature_ack_on_exit) {
            مداخلت = vmread(VMX_ڈیٹا_EXIT_مداخلت_INFO) & 0xff;
            ARCH_گرہ_STATE(x86_KScur_مداخلت) = مداخلت;
            handle_مداخلت_اندراج();
        } ورنہ {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(گرہ_STATE(ksCurThread));
#اگر_ختم
        واپس EXCEPTION_NONE;
    }

    گرہ_تالا_SYS;

    انتخاب (reason) {
    حالت EXCEPTION_OR_NMI:
    حالت MOV_DR:
    حالت TASK_SWITCH:
    حالت CONTROL_رجسٹر:
    حالت IO:
    حالت MWAIT:
    حالت SIPI:
    حالت INVLPG:
    حالت INVEPT:
    حالت INVVPID:
    حالت VMCLEAR:
    حالت VMPTRLD:
    حالت VMPTRST:
    حالت VMREAD:
    حالت VMWRITE:
    حالت VMXON:
    حالت EPT_VIOLATION:
    حالت GDTR_OR_IDTR:
    حالت LDTR_OR_TR:
    حالت TPR_نیچے_حد:
    حالت APIC_ACCESS:
        qualification = vmread(VMX_ڈیٹا_EXIT_QUALIFICATION);
        توڑو;
    طےشدہ:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    واپس EXCEPTION_NONE;
}

exception_t handle_Vm_اندراج_Fail(خالی)
{
    handleVmxFault(-1, -1);

    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
خالی VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    أكد(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    اگر (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_فعال) {

        لفظ_t badge = notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr);
        notification_ptr_سیٹ_state(ntfnPtr, NtfnState_Idle);
        سیٹ_Thread_State(tcb, ThreadState_Running);
        سیٹ_رجسٹر(tcb, badge_رجسٹر, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        اگر (tcb != گرہ_STATE(ksCurThread)) {
            possible_Switch_مقصد(tcb);
        }
    }
}
#اگر_ختم /* ENABLE_SMP_SUPPORT */

ساکن خالی invvpid_context(uint16_t vpid)
{
    ساخت {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t پتہ;
    } PACKED عملیہ = {vpid, 0, 0};
    asm متلون("invvpid %0, %1" :: "m"(عملیہ), "r"((لفظ_t)1) : "cc");
}

ساکن خالی سیٹ_EPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_جڑ;
    اگر (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_جڑ = kpptr_مقصد_paddr(null_ept_space);
    } ورنہ {
        ڈھونڈو_EPTFor_ASID_ret_t ڈھونڈو_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_بنیاد_Ptr(vmxSpace);
        ڈھونڈو_ret = ڈھونڈو_EPTFor_ASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE || ڈھونڈو_ret.ept != pml4) {
            ept_جڑ = kpptr_مقصد_paddr(null_ept_space);
        } ورنہ {
            ept_جڑ = pptr_مقصد_paddr(pml4);
        }
    }
    اگر (ept_جڑ != vcpu->آخری_ept_جڑ) {
        vcpu->آخری_ept_جڑ = ept_جڑ;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_جڑ,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_اشارہ_گر, eptp.الفاظ[0]);
        أكد(vcpu->vpid != VPID_INVALID);
        اگر (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

خالی clearVPIDIOPortMappings(vpid_t vpid, uint16_t پہلا, uint16_t آخری)
{
    اگر (vpid == VPID_INVALID) {
        واپس;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    اگر (vcpu == تہی) {
        واپس;
    }
    أكد(vcpu->vpid == vpid);
    سیٹ_IOPort_نقاب(vcpu->io, پہلا, آخری, true);
}

ساکن اندرونی vpid_t اگلا_VPID(vpid_t vpid)
{
    اگر (vpid == VPID_آخری) {
        واپس VPID_پہلا;
    } ورنہ {
        واپس vpid + 1;
    }
}

ساکن خالی invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, حجم(vcpu->io));
    /* invalidate the VPID context */
    اگر (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
ساکن خالی releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = تہی;
}

ساکن vpid_t ڈھونڈو_Free_VPID(خالی)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    کرو {
        اگر (x86KSVPIDTable[vpid] == تہی) {
            واپس vpid;
        }
        vpid = اگلا_VPID(vpid);
    } جب (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = اگلا_VPID(x86KSNextVPID);
    واپس vpid;
}

ساکن خالی storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    أكد(x86KSVPIDTable[vpid] == تہی);
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
خالی vcpu_fpu_مقصد_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    اگر (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        اگر (!vcpu->fpu_فعال) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_فعال = true;
            بھرو_Fpu_State(tcb);
        }
    } ورنہ {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_فعال = true;
        switchLocalFpuOwner(tcb);
    }
}

خالی vcpu_fpu_مقصد_host(tcb_t *tcb, vcpu_t *vcpu)
{
    اگر (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } ورنہ اگر (nativeThreadUsingFPU(tcb) && vcpu->fpu_فعال) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_فعال = false;
        بھرو_Fpu_State(tcb);
    } ورنہ اگر (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

خالی restoreVMCS(خالی)
{
    tcb_t *cur_thread = گرہ_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    اگر (ARCH_گرہ_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_SKIM_کھڑکی
    اگر (getCurrentCR3().الفاظ[0] != expected_vmcs->آخری_host_cr3) {
        expected_vmcs->آخری_host_cr3 = getCurrentCR3().الفاظ[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().الفاظ[0]);
    }
#اگر_ختم
    اگر (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = ڈھونڈو_Free_VPID();
        storeVPID(expected_vmcs, vpid);
        اگر (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    سیٹ_EPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

خالی invept(ept_pml4e_t *ept_pml4)
{
    اگر (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        ساخت {
            uint64_t parts[2];
        } پتہ;
        لفظ_t type;
        اگر (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } ورنہ اگر (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } ورنہ {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            واپس;
        }

        پتہ.parts[0] = pptr_مقصد_paddr((خالی *)ept_pml4);
        پتہ.parts[1] = 0;
        asm متلون(
            "invept %0, %1"
            :
            : "m"(پتہ),  "r"(type)
            : "memory"
        );
    }
}

#اگر_ختم
