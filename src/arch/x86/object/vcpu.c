/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_VTX

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

#وصف_ڪريو VMX_EXIT_QUAL_TYPE_MOV_CR 0
#وصف_ڪريو VMX_EXIT_QUAL_TYPE_CLTS 2
#وصف_ڪريو VMX_EXIT_QUAL_TYPE_LMSW 3

ثابت vcpu_gp_رجسٽر_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#جيڪڏهن_وصف_ٿيل CONFIG_X86_64_VTX_64_ٻٽو_انگ_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#جيڪڏهن_پورو /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#وصف_ڪريو MSR_BITMAP_نقاب(x) ((x) & 0x1fff)

تعريفنوع بنية msr_bitmap {
    لفظ_t bitmap[0x2000 / حجم(لفظ_t) / 8];
} msr_bitmap_t;

تعريفنوع بنية msr_bitmaps {
    msr_bitmap_t هيٺيون_msr_read;
    msr_bitmap_t مٿاهون_msr_read;
    msr_bitmap_t هيٺيون_msr_write;
    msr_bitmap_t مٿاهون_msr_write;
} msr_bitmaps_t;

ساكن msr_bitmaps_t msr_bitmap_region ALIGN(ٻٽو_انگ(se_L4_صفحو_بٽ));

ساكن حرف null_ept_space[ٻٽو_انگ(se_L4_صفحو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_صفحو_بٽ));

/* Cached value of the hardware defined vmcs revision */
ساكن uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
ساكن vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
ساكن uint32_t pin_control_مٿاهون;
ساكن uint32_t pin_control_هيٺيون;
ساكن uint32_t primary_control_مٿاهون;
ساكن uint32_t primary_control_هيٺيون;
ساكن uint32_t secondary_control_مٿاهون;
ساكن uint32_t secondary_control_هيٺيون;
ساكن uint32_t داخلا_control_مٿاهون;
ساكن uint32_t داخلا_control_هيٺيون;
ساكن uint32_t exit_control_مٿاهون;
ساكن uint32_t exit_control_هيٺيون;
ساكن uint32_t cr0_مٿاهون;
ساكن uint32_t cr0_هيٺيون;
ساكن uint32_t cr4_مٿاهون;
ساكن uint32_t cr4_هيٺيون;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
ساكن bool_t vmx_feature_vpid;
ساكن bool_t vmx_feature_ڀريو_perf_global_ctrl;
ساكن bool_t vmx_feature_ack_on_exit;

ساكن vcpu_t *x86KSVPIDTable[VPID_آخري + 1];
ساكن vpid_t x86KSNextVPID = VPID_پهريون;

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
    uint64_t physical_پتو;
    physical_پتو = pptr_منزل_paddr((فراغ *)vmcs_ptr);
    asm متطاير(
        "vmclear %0"
        :
        : "m"(physical_پتو)
        : "cc"
    );
}

فراغ clearCurrentVCPU(فراغ)
{
    vcpu_t *vcpu = ARCH_ڳنڍ_STATE(x86KSCurrentVCPU);
    إذا (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_ڳنڍ_STATE(x86KSCurrentVCPU) = عدم;
        إذا (vcpu->fpu_سرگرم && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_سرگرم = false;
        }
    }
}

ساكن فراغ vmptrld(فراغ *vmcs_ptr)
{
    uint64_t physical_پتو;
    uint8_t error;
    physical_پتو = pptr_منزل_paddr(vmcs_ptr);
    asm متطاير(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_پتو)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    أكد(!error);
}

#جيڪڏهن_وصف_ٿيل CONFIG_X86_64_VTX_64_ٻٽو_انگ_GUESTS
فراغ vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_تارو_MSR, vcpu->syscall_registers[VCPU_تارو]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_نقاب]);
}

فراغ vcpu_restore_host_msrs(فراغ)
{
    init_syscall_msrs();
}
#جيڪڏهن_پورو

ساكن فراغ switchVCPU(vcpu_t *vcpu)
{
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    إذا (vcpu->آخري_cpu != getCurrentCPUIndex() && ARCH_ڳنڍ_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->آخري_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->آخري_cpu);
    }
#جيڪڏهن_پورو
    clearCurrentVCPU();
    vmptrld(vcpu);
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    إذا (vcpu->آخري_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_بنياد, (لفظ_t)&x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KStss);
        vmwrite(VMX_HOST_GDTR_بنياد, (لفظ_t)x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_بنياد, (لفظ_t)x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(لفظ_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KStss.tss.لفظ[0] +
                                                          4));
    }
    vcpu->آخري_cpu = getCurrentCPUIndex();
#جيڪڏهن_پورو
    ARCH_ڳنڍ_STATE(x86KSCurrentVCPU) = vcpu;
}

ساكن فراغ print_بٽ(لفظ_t بٽ)
{
    bool_t پهريون = true;
    طالما (بٽ) {
        صحيح اشاريو = se_L4_لفظ_بٽ - 1 - clzl(بٽ);
        إذا (پهريون) {
            لکو("%d", اشاريو);
            پهريون = false;
        } وإلا {
            لکو(",%d", اشاريو);
        }
        بٽ &= ~ٻٽو_انگ(اشاريو);
    }
}

ساكن bool_t check_fixed_قدر(لفظ_t val, لفظ_t هيٺيون, لفظ_t مٿاهون)
{
    لفظ_t not_مٿاهون;
    لفظ_t not_هيٺيون;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_مٿاهون = مٿاهون & ~(مٿاهون & val);
    إذا (not_مٿاهون != 0) {
        لکو("Failed to set bits: ");
        print_بٽ(not_مٿاهون);
        أرجع false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_هيٺيون = ~هيٺيون & ~(~هيٺيون & ~val);
    إذا (not_هيٺيون != 0) {
        لکو("Incorrectly cleared bits: ");
        print_بٽ(not_هيٺيون);
        أرجع false;
    }
    أرجع true;
}

ساكن bool_t vtx_check_fixed_قدرون(لفظ_t cr0, لفظ_t cr4)
{
    إذا (!check_fixed_قدر(cr0, cr0_هيٺيون, cr0_مٿاهون)) {
        لکو(" of CR0\n");
        أرجع false;
    }
    إذا (!check_fixed_قدر(cr4, cr4_هيٺيون, cr4_مٿاهون)) {
        لکو(" of CR4\n");
        أرجع false;
    }
    أرجع true;
}

ساكن bool_t BOOT_ڪوڊ init_vtx_fixed_قدرون(bool_t useTrueMsrs)
{
    uint32_t pin_control_نقاب =
        ٻٽو_انگ(0) |    //Extern interrupt exiting
        ٻٽو_انگ(3) |    //NMI exiting
        ٻٽو_انگ(5);     //virtual NMIs
    uint32_t primary_control_نقاب =
        ٻٽو_انگ(25) |   //Use I/O bitmaps
        ٻٽو_انگ(28) |   //Use MSR bitmaps
        ٻٽو_انگ(31);    //Activate secondary controls
    uint32_t secondary_control_نقاب =
        ٻٽو_انگ(1);     //Enable EPT
    uint32_t exit_control_نقاب =
        ٻٽو_انگ(2)  |   //Save debug controls
        ٻٽو_انگ(18) |   //Save guest IA32_PAT on exit
        ٻٽو_انگ(19) |   //Load host IA32_PAT
        ٻٽو_انگ(20) |   //Save guest IA32_EFER on exit
        ٻٽو_انگ(21);    //Load host IA32_EFER
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_X86_64
    exit_control_نقاب |= ٻٽو_انگ(9); //Host address-space size
#جيڪڏهن_پورو /* CONFIG_ARCH_X86_64 */
    /* Read out the fixed high and low bits from the MSRs */
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t داخلا_ctls;
    إذا (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        داخلا_ctls = IA32_VMX_TRUE_داخلا_CTLS_MSR;
    } وإلا {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        داخلا_ctls = IA32_VMX_داخلا_CTLS_MSR;
    }
    pin_control_مٿاهون = x86_rdmsr_هيٺيون(pinbased_ctls);
    pin_control_هيٺيون = x86_rdmsr_مٿاهون(pinbased_ctls);
    primary_control_مٿاهون = x86_rdmsr_هيٺيون(procbased_ctls);
    primary_control_هيٺيون = x86_rdmsr_مٿاهون(procbased_ctls);
    secondary_control_مٿاهون = x86_rdmsr_هيٺيون(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_هيٺيون = x86_rdmsr_مٿاهون(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_مٿاهون = x86_rdmsr_هيٺيون(exit_ctls);
    exit_control_هيٺيون = x86_rdmsr_مٿاهون(exit_ctls);
    داخلا_control_مٿاهون = x86_rdmsr_هيٺيون(داخلا_ctls);
    داخلا_control_هيٺيون = x86_rdmsr_مٿاهون(داخلا_ctls);

    cr0_مٿاهون = x86_rdmsr_هيٺيون(IA32_VMX_CR0_FIXED0_MSR);
    cr0_هيٺيون = x86_rdmsr_هيٺيون(IA32_VMX_CR0_FIXED1_MSR);
    cr4_مٿاهون = x86_rdmsr_هيٺيون(IA32_VMX_CR4_FIXED0_MSR);
    cr4_هيٺيون = x86_rdmsr_هيٺيون(IA32_VMX_CR4_FIXED1_MSR);

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
    إذا (secondary_control_هيٺيون & ٻٽو_انگ(7)) {
        cr0_مٿاهون &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    إذا (!(secondary_control_هيٺيون & ٻٽو_انگ(5))) {
        vmx_feature_vpid = 0;
        لکو("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } وإلا {
        vmx_feature_vpid = 1;
        secondary_control_نقاب |= ٻٽو_انگ(5);
    }

    /* Check for load perf global control */
    إذا (!(exit_control_هيٺيون & ٻٽو_انگ(12))) {
        vmx_feature_ڀريو_perf_global_ctrl = 0;
        لکو("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } وإلا {
        vmx_feature_ڀريو_perf_global_ctrl = 1;
        exit_control_نقاب |= ٻٽو_انگ(12);
    }

    /* Check for external interrupt exiting */
    إذا (!(exit_control_هيٺيون & ٻٽو_انگ(15))) {
        vmx_feature_ack_on_exit = 0;
        لکو("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } وإلا {
        vmx_feature_ack_on_exit = 1;
        exit_control_نقاب |= ٻٽو_انگ(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_هيٺيون) & pin_control_نقاب;
    إذا (missing) {
        لکو("vt-x: Unsupported pin control features %lx\n", (ڊگهو)missing);
        أرجع false;
    }
    missing = (~primary_control_هيٺيون) & primary_control_نقاب;
    إذا (missing) {
        لکو("vt-x: Unsupported primary control features %lx\n", (ڊگهو)missing);
        أرجع false;
    }
    missing = (~secondary_control_هيٺيون) & secondary_control_نقاب;
    إذا (missing) {
        لکو("vt-x: Unsupported secondary control features %lx\n", (ڊگهو)missing);
        أرجع false;
    }
    missing = (~exit_control_هيٺيون) & exit_control_نقاب;
    إذا (missing) {
        لکو("vt-x: Unsupported exit control features %lx\n", (ڊگهو)missing);
        أرجع false;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_X86_64_VTX_64_ٻٽو_انگ_GUESTS
    missing = (~داخلا_control_هيٺيون) & ٻٽو_انگ(9);
    إذا (missing) {
        لکو("vt-x: CPU does not support 64-bit guests\n");
        أرجع false;
    }
#جيڪڏهن_پورو /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_مٿاهون |= pin_control_نقاب;
    primary_control_مٿاهون |= primary_control_نقاب;
    secondary_control_مٿاهون |= secondary_control_نقاب;
    exit_control_مٿاهون |= exit_control_نقاب;

    أرجع true;
}

ساكن bool_t BOOT_ڪوڊ check_vtx_fixed_قدرون(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t داخلا_ctls;
    إذا (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        داخلا_ctls = IA32_VMX_TRUE_داخلا_CTLS_MSR;
    } وإلا {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        داخلا_ctls = IA32_VMX_داخلا_CTLS_MSR;
    }
    uint32_t local_pin_control_مٿاهون = x86_rdmsr_هيٺيون(pinbased_ctls);
    uint32_t local_pin_control_هيٺيون = x86_rdmsr_مٿاهون(pinbased_ctls);
    uint32_t local_primary_control_مٿاهون = x86_rdmsr_هيٺيون(procbased_ctls);
    uint32_t local_primary_control_هيٺيون = x86_rdmsr_مٿاهون(procbased_ctls);
    uint32_t local_secondary_control_مٿاهون = x86_rdmsr_هيٺيون(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_هيٺيون = x86_rdmsr_مٿاهون(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_مٿاهون = x86_rdmsr_هيٺيون(exit_ctls);
    uint32_t local_exit_control_هيٺيون = x86_rdmsr_مٿاهون(exit_ctls);
    uint32_t local_داخلا_control_مٿاهون = x86_rdmsr_هيٺيون(داخلا_ctls);
    uint32_t local_داخلا_control_هيٺيون = x86_rdmsr_مٿاهون(داخلا_ctls);

    uint32_t local_cr0_مٿاهون = x86_rdmsr_هيٺيون(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_هيٺيون = x86_rdmsr_هيٺيون(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_مٿاهون = x86_rdmsr_هيٺيون(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_هيٺيون = x86_rdmsr_هيٺيون(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    إذا (local_secondary_control_هيٺيون & ٻٽو_انگ(7)) {
        local_cr0_مٿاهون &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    أرجع
        (local_pin_control_مٿاهون & pin_control_مٿاهون) == local_pin_control_مٿاهون &&
        (~local_pin_control_هيٺيون & pin_control_مٿاهون) == 0 &&
        (local_primary_control_مٿاهون & primary_control_مٿاهون) == local_primary_control_مٿاهون &&
        (~local_primary_control_هيٺيون & primary_control_مٿاهون) == 0 &&
        (local_secondary_control_مٿاهون & secondary_control_مٿاهون) == local_secondary_control_مٿاهون &&
        (~local_secondary_control_هيٺيون & secondary_control_مٿاهون) == 0 &&
        (local_exit_control_مٿاهون & exit_control_مٿاهون) == local_exit_control_مٿاهون &&
        (~local_exit_control_هيٺيون & exit_control_مٿاهون) == 0 &&
        (local_داخلا_control_مٿاهون & داخلا_control_مٿاهون) == local_داخلا_control_مٿاهون &&
        (~local_داخلا_control_هيٺيون & داخلا_control_مٿاهون) == 0 &&
        local_cr0_مٿاهون == cr0_مٿاهون &&
        local_cr0_هيٺيون == cr0_هيٺيون &&
        local_cr4_مٿاهون == cr4_مٿاهون &&
        local_cr4_هيٺيون == cr4_هيٺيون;
}

ساكن مضمن uint32_t apply_Fixed_بٽ(uint32_t original, uint32_t مٿاهون, uint32_t هيٺيون)
{
    original |= مٿاهون;
    original &= هيٺيون;
    أرجع original;
}

فراغ vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = عدم;
    vcpu->launched = false;
    vcpu->fpu_سرگرم = false;
    vcpu->fpuState = x86KSnullFpuState;

    يادگيري_نقل(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_مٿاهون & cr0_هيٺيون;
    vcpu->cr0_shadow = 0;
    vcpu->cr0_نقاب = 0;
    vcpu->exception_bitmap = 0;
    vcpu->vpid = VPID_INVALID;
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    vcpu->آخري_cpu = getCurrentCPUIndex();
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    switchVCPU(vcpu);

    vmwrite(VMX_HOST_PAT, x86_rdmsr(IA32_PAT_MSR));
    vmwrite(VMX_HOST_EFER, x86_rdmsr(IA32_EFER_MSR));
    // By default we will disable performance counters when we come out
    // of a VM. When performance counters are supported this host state
    // needs to be updated on VM entry
    إذا (vmx_feature_ڀريو_perf_global_ctrl) {
        vmwrite(VMX_HOST_PERF_GLOBAL_CTRL, 0);
    }
    vmwrite(VMX_HOST_CR0, read_cr0());
    vmwrite(VMX_HOST_CR4, read_cr4());
    vmwrite(VMX_HOST_FS_بنياد, 0);
    vmwrite(VMX_HOST_GS_بنياد, 0);
    vmwrite(VMX_HOST_TR_بنياد, (لفظ_t)&x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KStss);
    vmwrite(VMX_HOST_GDTR_بنياد, (لفظ_t)x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KSgdt);
    vmwrite(VMX_HOST_IDTR_بنياد, (لفظ_t)x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KSidt);
    vmwrite(VMX_HOST_SYSENTER_CS, (لفظ_t)SEL_CS_0);
    vmwrite(VMX_HOST_SYSENTER_EIP, (لفظ_t)&handle_syscall);
    إذا (!config_سيٽ(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(لفظ_t)((حرف *)&x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KStss.tss.لفظ[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (لفظ_t)&vcpu->gp_registers[n_vcpu_gp_رجسٽر]);
    vmwrite(VMX_HOST_RIP, (لفظ_t)&handle_vmexit);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_SKIM_دري
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_منزل_paddr(x64KSKernelPML4), 0).لفظ[0]);
#جيڪڏهن_پورو /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_مٿاهون & pin_control_هيٺيون);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_مٿاهون & primary_control_هيٺيون);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_مٿاهون & secondary_control_هيٺيون);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_مٿاهون & exit_control_هيٺيون);

    uint64_t داخلا_control = داخلا_control_مٿاهون & داخلا_control_هيٺيون;
#جيڪڏهن_وصف_ٿيل CONFIG_X86_64_VTX_64_ٻٽو_انگ_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    داخلا_control |= ٻٽو_انگ(9);
#جيڪڏهن_پورو /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_داخلا_CONTROLS, داخلا_control);

    vmwrite(VMX_CONTROL_MSR_پتو, (لفظ_t)kpptr_منزل_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_مٿاهون & cr4_هيٺيون);

    vmwrite(VMX_GUEST_VMCS_LINK_اشارو, ~(لفظ_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_اشارو_مٿاهون, ~(لفظ_t)0);

    memset(vcpu->io, ~(لفظ_t)0, VCPU_IOBITMAP_سائيز);
    vmwrite(VMX_CONTROL_IOA_پتو, pptr_منزل_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_پتو, pptr_منزل_paddr((حرف *)vcpu->io + (VCPU_IOBITMAP_سائيز / 2)));
}

ساكن فراغ dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    أكد(tcb->tcbArch.tcbVCPU == vcpu);
    أكد(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_سرگرم = false;
    tcb->tcbArch.tcbVCPU = عدم;
    vcpu->vcpuTCB = عدم;
}

فراغ vcpu_finalise(vcpu_t *vcpu)
{
    إذا (ARCH_ڳنڍ_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->آخري_cpu) == vcpu) {
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
        إذا (vcpu->آخري_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->آخري_cpu);
        } وإلا
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
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

ساكن exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, لفظ_t *بفر)
{
    صحيح i;
    لكل (i = 0; i < n_vcpu_gp_رجسٽر; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, بفر);
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUWriteRegisters(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
#جيڪڏهن_وصف_ٿيل CONFIG_X86_64_VTX_64_ٻٽو_انگ_GUESTS
    إذا (ڊيگهائي < 15) {
#وإلا
    إذا (ڊيگهائي < 7) {
#جيڪڏهن_پورو /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), بفر);
}

#جيڪڏهن_وصف_ٿيل CONFIG_X86_64_VTX_64_ٻٽو_انگ_GUESTS
ساكن exception_t invokeWriteMSR(vcpu_t *vcpu, لفظ_t *بفر, لفظ_t ميدان, لفظ_t قدر)
{
    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);
    إذا (ARCH_ڳنڍ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بدل (ميدان) {
    حالة IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = قدر;
        اكسر;
    حالة IA32_تارو_MSR:
        vcpu->syscall_registers[VCPU_تارو] = قدر;
        اكسر;
    حالة IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = قدر;
        اكسر;
    حالة IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_نقاب] = قدر;
        اكسر;
    }
    سيٽ_MR(thread, بفر, 0, قدر);
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUWriteMSR(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    لفظ_t ميدان;
    لفظ_t قدر;

    إذا (ڊيگهائي < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ميدان = getSyscallArg(0, بفر);
    قدر = getSyscallArg(1, بفر);
    بدل (ميدان) {
    حالة IA32_LSTAR_MSR:
    حالة IA32_تارو_MSR:
    حالة IA32_CSTAR_MSR:
    حالة IA32_FMASK_MSR:
        اكسر;
    افتراضي:
        userError("VCPU WriteMSR: Invalid field %lx.", (ڊگهو)ميدان);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), بفر, ميدان, قدر);
}

ساكن exception_t invokeReadMSR(vcpu_t *vcpu, لفظ_t ميدان, لفظ_t *بفر)
{
    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);

    لفظ_t قدر = 0;

    بدل (ميدان) {
    حالة IA32_LSTAR_MSR:
        قدر = vcpu->syscall_registers[VCPU_LSTAR];
        اكسر;
    حالة IA32_تارو_MSR:
        قدر = vcpu->syscall_registers[VCPU_تارو];
        اكسر;
    حالة IA32_CSTAR_MSR:
        قدر = vcpu->syscall_registers[VCPU_CSTAR];
        اكسر;
    حالة IA32_FMASK_MSR:
        قدر = vcpu->syscall_registers[VCPU_SYSCALL_نقاب];
        اكسر;
    حالة IA32_VMX_MISC_MSR:
        قدر = x86_rdmsr(ميدان);
        اكسر;
    }

    سيٽ_MR(thread, بفر, 0, قدر);
    سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                    se_L4_پيغام_Info_new(0, 0, 0, 1)));
    سيٽ_Thread_State(thread, ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeVCPUReadMSR(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    إذا (ڊيگهائي < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    لفظ_t ميدان = getSyscallArg(0, بفر);
    بدل (ميدان) {
    حالة IA32_LSTAR_MSR:
    حالة IA32_تارو_MSR:
    حالة IA32_CSTAR_MSR:
    حالة IA32_FMASK_MSR:
    حالة IA32_VMX_MISC_MSR:
        اكسر;
    افتراضي:
        userError("VCPU ReadMSR: Invalid field %lx.", (ڊگهو)ميدان);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ميدان, بفر);
}
#جيڪڏهن_پورو  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

ساكن exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *جڳهه, cap_t cap, uint16_t هيٺيون, uint16_t مٿاهون)
{
    /* remove any existing io ports from this cap */
    clearVPIDIOPortMappings(cap_io_پورٽ_cap_get_cap_IOPort_VPID(cap),
                            cap_io_پورٽ_cap_get_cap_IOPort_پهريون_پورٽ(cap),
                            cap_io_پورٽ_cap_get_cap_IOPort_آخري_پورٽ(cap));
    /* update the assigned vpid. If the vcpu does not have a valid vpid then
     * this is fine as whilst the cap will not point to the vcpu, the vcpu
     * will have its port mask cleared when it gets assigned a vpid */
    cap = cap_io_پورٽ_cap_سيٽ_cap_IOPort_VPID(cap, vcpu->vpid);
    جڳهه->cap = cap;
    سيٽ_IOPort_نقاب(vcpu->io, هيٺيون, مٿاهون, false);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeEnableIOPort(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *io_جڳهه;
    uint16_t هيٺيون, مٿاهون;

    إذا (ڊيگهائي < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    io_جڳهه = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(ioCap) != cap_io_پورٽ_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    هيٺيون = getSyscallArg(0, بفر);
    مٿاهون = getSyscallArg(1, بفر);

    إذا (هيٺيون < cap_io_پورٽ_cap_get_cap_IOPort_پهريون_پورٽ(ioCap) || مٿاهون > cap_io_پورٽ_cap_get_cap_IOPort_آخري_پورٽ(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeEnableIOPort(vcpu, io_جڳهه, ioCap, هيٺيون, مٿاهون);
}

ساكن exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t هيٺيون, uint16_t مٿاهون)
{
    سيٽ_IOPort_نقاب(vcpu->io, هيٺيون, مٿاهون, true);
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeDisableIOPort(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    vcpu_t *vcpu;
    uint16_t هيٺيون, مٿاهون;

    إذا (ڊيگهائي < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    هيٺيون = getSyscallArg(0, بفر);
    مٿاهون = getSyscallArg(1, بفر);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    أرجع invokeDisableIOPort(vcpu, هيٺيون, مٿاهون);
}

ساكن exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, لفظ_t *بفر, لفظ_t ميدان, لفظ_t قدر)
{
    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);

    إذا (ARCH_ڳنڍ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بدل (ميدان) {
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = قدر;
        اكسر;
    حالة VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = قدر;
        اكسر;
    حالة VMX_CONTROL_CR0_نقاب:
        vcpu->cr0_نقاب = vcpu->cached_cr0_نقاب = قدر;
        اكسر;
    حالة VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = قدر;
        اكسر;
    }
    vmwrite(ميدان, قدر);

    إذا (call) {
        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
        بنانشان صحيح ڊيگهائي = سيٽ_MR(thread, بفر, 0, قدر);
        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, ڊيگهائي)));
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeWriteVMCS(cap_t cap, لفظ_t ڊيگهائي, bool_t call, لفظ_t *بفر)
{
    لفظ_t ميدان;
    لفظ_t قدر;

    إذا (ڊيگهائي < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ميدان = getSyscallArg(0, بفر);
    قدر = getSyscallArg(1, بفر);
    بدل (ميدان) {
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
    حالة VMX_GUEST_ES_حد_بندي:
    حالة VMX_GUEST_CS_حد_بندي:
    حالة VMX_GUEST_SS_حد_بندي:
    حالة VMX_GUEST_DS_حد_بندي:
    حالة VMX_GUEST_FS_حد_بندي:
    حالة VMX_GUEST_GS_حد_بندي:
    حالة VMX_GUEST_LDTR_حد_بندي:
    حالة VMX_GUEST_TR_حد_بندي:
    حالة VMX_GUEST_GDTR_حد_بندي:
    حالة VMX_GUEST_IDTR_حد_بندي:
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
    حالة VMX_GUEST_PREEMPTION_TIMER_قدر:
    حالة VMX_GUEST_ES_بنياد:
    حالة VMX_GUEST_CS_بنياد:
    حالة VMX_GUEST_SS_بنياد:
    حالة VMX_GUEST_DS_بنياد:
    حالة VMX_GUEST_FS_بنياد:
    حالة VMX_GUEST_GS_بنياد:
    حالة VMX_GUEST_LDTR_بنياد:
    حالة VMX_GUEST_TR_بنياد:
    حالة VMX_GUEST_GDTR_بنياد:
    حالة VMX_GUEST_IDTR_بنياد:
    حالة VMX_GUEST_DR7:
    حالة VMX_GUEST_RFLAGS:
    حالة VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالة VMX_GUEST_SYSENTER_ESP:
    حالة VMX_GUEST_SYSENTER_EIP:
    حالة VMX_CONTROL_CR0_نقاب:
    حالة VMX_CONTROL_CR4_نقاب:
    حالة VMX_CONTROL_CR0_READ_SHADOW:
    حالة VMX_CONTROL_CR4_READ_SHADOW:
    حالة VMX_GUEST_CR3:
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
    حالة VMX_CONTROL_داخلا_INTERRUPTION_INFO:
    حالة VMX_CONTROL_داخلا_EXCEPTION_ERROR_ڪوڊ:
        اكسر;
    حالة VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        قدر = apply_Fixed_بٽ(قدر, pin_control_مٿاهون, pin_control_هيٺيون);
        اكسر;
    حالة VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        قدر = apply_Fixed_بٽ(قدر, primary_control_مٿاهون, primary_control_هيٺيون);
        اكسر;
    حالة VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        قدر = apply_Fixed_بٽ(قدر, secondary_control_مٿاهون, secondary_control_هيٺيون);
        اكسر;
    حالة VMX_CONTROL_داخلا_CONTROLS:
        قدر = apply_Fixed_بٽ(قدر, داخلا_control_مٿاهون, داخلا_control_هيٺيون);
        اكسر;
    حالة VMX_CONTROL_EXIT_CONTROLS:
        قدر = apply_Fixed_بٽ(قدر, exit_control_مٿاهون, exit_control_هيٺيون);
        اكسر;
    حالة VMX_GUEST_CR0:
        قدر = apply_Fixed_بٽ(قدر, cr0_مٿاهون, cr0_هيٺيون);
        اكسر;
    حالة VMX_GUEST_CR4:
        قدر = apply_Fixed_بٽ(قدر, cr4_مٿاهون, cr4_هيٺيون);
        اكسر;
    افتراضي:
        userError("VCPU WriteVMCS: Invalid field %lx.", (ڊگهو)ميدان);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, بفر, ميدان, قدر);
}

ساكن لفظ_t readVMCSField(vcpu_t *vcpu, لفظ_t ميدان)
{
    بدل (ميدان) {
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
        أرجع vcpu->exception_bitmap;
    حالة VMX_GUEST_CR0:
        أرجع vcpu->cr0;
    حالة VMX_CONTROL_CR0_نقاب:
        أرجع vcpu->cr0_نقاب;
    حالة VMX_CONTROL_CR0_READ_SHADOW:
        أرجع vcpu->cr0_shadow;
    }
    إذا (ARCH_ڳنڍ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    أرجع vmread(ميدان);
}

ساكن exception_t invokeReadVMCS(vcpu_t *vcpu, لفظ_t ميدان, bool_t call, لفظ_t *بفر)
{
    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);
    لفظ_t قدر = readVMCSField(vcpu, ميدان);
    إذا (call) {
        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
        بنانشان صحيح ڊيگهائي = سيٽ_MR(thread, بفر, 0, قدر);
        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, ڊيگهائي)));
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeReadVMCS(cap_t cap, لفظ_t ڊيگهائي, bool_t call, لفظ_t *بفر)
{
    إذا (ڊيگهائي < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    لفظ_t ميدان = getSyscallArg(0, بفر);
    بدل (ميدان) {
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
    حالة VMX_GUEST_ES_حد_بندي:
    حالة VMX_GUEST_CS_حد_بندي:
    حالة VMX_GUEST_SS_حد_بندي:
    حالة VMX_GUEST_DS_حد_بندي:
    حالة VMX_GUEST_FS_حد_بندي:
    حالة VMX_GUEST_GS_حد_بندي:
    حالة VMX_GUEST_LDTR_حد_بندي:
    حالة VMX_GUEST_TR_حد_بندي:
    حالة VMX_GUEST_GDTR_حد_بندي:
    حالة VMX_GUEST_IDTR_حد_بندي:
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
    حالة VMX_GUEST_PREEMPTION_TIMER_قدر:
    حالة VMX_GUEST_ES_بنياد:
    حالة VMX_GUEST_CS_بنياد:
    حالة VMX_GUEST_SS_بنياد:
    حالة VMX_GUEST_DS_بنياد:
    حالة VMX_GUEST_FS_بنياد:
    حالة VMX_GUEST_GS_بنياد:
    حالة VMX_GUEST_LDTR_بنياد:
    حالة VMX_GUEST_TR_بنياد:
    حالة VMX_GUEST_GDTR_بنياد:
    حالة VMX_GUEST_IDTR_بنياد:
    حالة VMX_GUEST_DR7:
    حالة VMX_GUEST_RFLAGS:
    حالة VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالة VMX_GUEST_SYSENTER_ESP:
    حالة VMX_GUEST_SYSENTER_EIP:
    حالة VMX_CONTROL_CR0_نقاب:
    حالة VMX_CONTROL_CR4_نقاب:
    حالة VMX_CONTROL_CR0_READ_SHADOW:
    حالة VMX_CONTROL_CR4_READ_SHADOW:
    حالة VMX_معلومات_INSTRUCTION_ERROR:
    حالة VMX_معلومات_EXIT_مداخلت_INFO:
    حالة VMX_معلومات_EXIT_مداخلت_ERROR:
    حالة VMX_معلومات_IDT_ويڪٽر_INFO:
    حالة VMX_معلومات_IDT_ويڪٽر_ERROR:
    حالة VMX_معلومات_EXIT_INSTRUCTION_ڊيگهائي:
    حالة VMX_معلومات_EXIT_INSTRUCTION_INFO:
    حالة VMX_معلومات_GUEST_PHYSICAL:
    حالة VMX_معلومات_IO_RCX:
    حالة VMX_معلومات_IO_RSI:
    حالة VMX_معلومات_IO_RDI:
    حالة VMX_معلومات_IO_RIP:
    حالة VMX_معلومات_GUEST_لڪيري_پتو:
    حالة VMX_CONTROL_داخلا_INTERRUPTION_INFO:
    حالة VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    حالة VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    حالة VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    حالة VMX_CONTROL_EXCEPTION_BITMAP:
    حالة VMX_CONTROL_داخلا_CONTROLS:
    حالة VMX_CONTROL_EXIT_CONTROLS:
    حالة VMX_GUEST_CR0:
    حالة VMX_GUEST_CR3:
    حالة VMX_GUEST_CR4:
        اكسر;
    افتراضي:
        userError("VCPU ReadVMCS: Invalid field %lx.", (ڊگهو)ميدان);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), ميدان, call, بفر);
}

ساكن exception_t invoke_سيٽ_TCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_سيٽ_TCB(cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    cap_t tcbCap;
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    إذا (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع invoke_سيٽ_TCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

فراغ vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    لفظ_t *بفر;
    إذا (ARCH_ڳنڍ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بفر = lookupIPCBuffer(false, ڳنڍ_STATE(ksCurThread));
    إذا (!بفر) {
        userError("No IPC buffer.");
        أرجع;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, بفر));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, apply_Fixed_بٽ(getSyscallArg(1, بفر), primary_control_مٿاهون,
                                                                   primary_control_هيٺيون));
    vmwrite(VMX_CONTROL_داخلا_INTERRUPTION_INFO, getSyscallArg(2, بفر));
}

فراغ vcpu_sysvmenter_جواب_منزل_user(tcb_t *tcb)
{
    لفظ_t *بفر;
    vcpu_t *vcpu;

    بفر = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    أكد(vcpu);

    إذا (ARCH_ڳنڍ_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    سيٽ_MR(tcb, بفر, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    سيٽ_MR(tcb, بفر, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    سيٽ_MR(tcb, بفر, SEL4_VMENTER_CALL_مداخلت_INFO_MR,
          vmread(VMX_CONTROL_داخلا_INTERRUPTION_INFO));
    سيٽ_رجسٽر(tcb, نياپو_Info_رجسٽر, 0);
}

exception_t decodeX86VCPUInvocation(
    لفظ_t invLabel,
    لفظ_t ڊيگهائي,
    cptr_t cptr,
    cte_t *جڳهه,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (invLabel) {
    حالة X86VCPUSetTCB:
        أرجع decode_سيٽ_TCB(cap, ڊيگهائي, بفر);
    حالة X86VCPUReadVMCS:
        أرجع decodeReadVMCS(cap, ڊيگهائي, call, بفر);
    حالة X86VCPUWriteVMCS:
        أرجع decodeWriteVMCS(cap, ڊيگهائي, call, بفر);
    حالة X86VCPUEnableIOPort:
        أرجع decodeEnableIOPort(cap, ڊيگهائي, بفر);
    حالة X86VCPUDisableIOPort:
        أرجع decodeDisableIOPort(cap, ڊيگهائي, بفر);
    حالة X86VCPUWriteRegisters:
        أرجع decodeVCPUWriteRegisters(cap, ڊيگهائي, بفر);
#جيڪڏهن_وصف_ٿيل CONFIG_X86_64_VTX_64_ٻٽو_انگ_GUESTS
    حالة X86VCPUWriteMSR:
        أرجع decodeVCPUWriteMSR(cap, ڊيگهائي, بفر);
    حالة X86VCPUReadMSR:
        أرجع decodeVCPUReadMSR(cap, ڊيگهائي, بفر);
#جيڪڏهن_پورو /* CONFIG_X86_64_VTX_64BIT_GUESTS */
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
    أرجع !!(x86_cpuid_ecx(0x1, 0) & ٻٽو_انگ(5));
}

ساكن مضمن فراغ clear_ٻٽو_انگ(لفظ_t *bitmap, لفظ_t ٻٽو_انگ_2)
{
    صحيح اشاريو = ٻٽو_انگ_2 / (حجم(لفظ_t) * 8);
    صحيح وٿي = ٻٽو_انگ_2 % (حجم(لفظ_t) * 8);
    bitmap[اشاريو] &= ~ٻٽو_انگ(وٿي);
}

BOOT_ڪوڊ bool_t vtx_init(فراغ)
{
    إذا (!is_vtx_supported()) {
        لکو("vt-x: not supported\n");
        أرجع false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.لفظ[0] = x86_rdmsr_هيٺيون(IA32_VMX_BASIC_MSR);
    vmx_basic.لفظ[1] = x86_rdmsr_مٿاهون(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.لفظ[0] = x86_rdmsr_هيٺيون(IA32_FEATURE_CONTROL_MSR);
    إذا (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        إذا (feature_control_msr_get_تالو(feature_control)) {
            لکو("vt-x: feature locked\n");
            أرجع false;
        }
        feature_control = feature_control_msr_سيٽ_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_مٿاهون(IA32_FEATURE_CONTROL_MSR), feature_control.لفظ[0]);
    }
    /* make sure the msr is locked */
    إذا (!feature_control_msr_get_تالو(feature_control)) {
        feature_control = feature_control_msr_سيٽ_تالو(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_مٿاهون(IA32_FEATURE_CONTROL_MSR), feature_control.لفظ[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    إذا (CURRENT_CPU_اشاريو() == 0) {
        إذا (!init_vtx_fixed_قدرون(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            لکو("vt-x: lack of required features\n");
            أرجع false;
        }
    }
    إذا (!check_vtx_fixed_قدرون(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        لکو("vt-x: cores have inconsistent features\n");
        أرجع false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    بنية vmxon_region *vmxon_kptr = &ARCH_ڳنڍ_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    إذا (!vtx_check_fixed_قدرون(read_cr0(), read_cr4())) {
        أرجع false;
    }
    إذا (vmxon(kpptr_منزل_paddr(vmxon_kptr))) {
        لکو("vt-x: vmxon failure\n");
        أرجع false;
    }
    memset(&msr_bitmap_region, ~0, حجم(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_ٻٽو_انگ(msr_bitmap_region.هيٺيون_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_ٻٽو_انگ(msr_bitmap_region.هيٺيون_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_ٻٽو_انگ(msr_bitmap_region.هيٺيون_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_ٻٽو_انگ(msr_bitmap_region.هيٺيون_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_ٻٽو_انگ(msr_bitmap_region.هيٺيون_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_ٻٽو_انگ(msr_bitmap_region.هيٺيون_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#جيڪڏهن_وصف_ٿيل CONFIG_X86_64_VTX_64_ٻٽو_انگ_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_ٻٽو_انگ(msr_bitmap_region.مٿاهون_msr_read.bitmap, MSR_BITMAP_نقاب(IA32_FS_بنياد_MSR));
    clear_ٻٽو_انگ(msr_bitmap_region.مٿاهون_msr_read.bitmap, MSR_BITMAP_نقاب(IA32_GS_بنياد_MSR));
    clear_ٻٽو_انگ(msr_bitmap_region.مٿاهون_msr_read.bitmap, MSR_BITMAP_نقاب(IA32_KERNEL_GS_بنياد_MSR));
    clear_ٻٽو_انگ(msr_bitmap_region.مٿاهون_msr_write.bitmap, MSR_BITMAP_نقاب(IA32_FS_بنياد_MSR));
    clear_ٻٽو_انگ(msr_bitmap_region.مٿاهون_msr_write.bitmap, MSR_BITMAP_نقاب(IA32_GS_بنياد_MSR));
    clear_ٻٽو_انگ(msr_bitmap_region.مٿاهون_msr_write.bitmap, MSR_BITMAP_نقاب(IA32_KERNEL_GS_بنياد_MSR));
#جيڪڏهن_پورو
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.لفظ[0] = x86_rdmsr_هيٺيون(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.لفظ[1] = x86_rdmsr_مٿاهون(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    إذا (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        لکو("vt-x: Expected wb attribute for EPT paging structure\n");
        أرجع false;
    }
    إذا (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        لکو("vt-x: Expected supported for 2m pages\n");
        أرجع false;
    }

    أرجع true;
}

ساكن فراغ سيٽ_MRs_vmexit(uint32_t reason, لفظ_t qualification)
{
    لفظ_t *بفر;
    صحيح i;

    بفر = lookupIPCBuffer(true, ڳنڍ_STATE(ksCurThread));

    سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_CALL_مداخلت_INFO_MR,
          vmread(VMX_CONTROL_داخلا_INTERRUPTION_INFO));
    سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_REASON_MR, reason);
    سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_معلومات_EXIT_INSTRUCTION_ڊيگهائي));
    سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_معلومات_GUEST_PHYSICAL));
    سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    لكل (i = 0; i < n_vcpu_gp_رجسٽر; i++) {
        سيٽ_MR(ڳنڍ_STATE(ksCurThread), بفر, SEL4_VMENTER_FAULT_EAX + i,
              ڳنڍ_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

ساكن فراغ handleVmxFault(uint32_t reason, لفظ_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    سيٽ_رجسٽر(ڳنڍ_STATE(ksCurThread), نياپو_Info_رجسٽر, SEL4_VMENTER_نتيجو_FAULT);

    سيٽ_MRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

ساكن مضمن فراغ پڄاڻي_Vmexit_Saving(فراغ)
{
    vcpu_t *vcpu = ARCH_ڳنڍ_STATE(x86KSCurrentVCPU);
    أكد(vcpu == ڳنڍ_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(فراغ)
{
    uint32_t مداخلت;
    /* qualification is host width, reason is defined as being 32 bit */
    لفظ_t qualification;
    uint32_t reason;
    پڄاڻي_Vmexit_Saving();
#جيڪڏهن_وصف_ٿيل CONFIG_X86_64_VTX_64_ٻٽو_انگ_GUESTS
    vcpu_restore_host_msrs();
#جيڪڏهن_پورو
    /* the basic exit reason is the bottom 16 bits of the exit reason field */
    reason = vmread(VMX_معلومات_EXIT_REASON) & نقاب(16);
    إذا (reason == EXTERNAL_مداخلت) {
        ڳنڍ_تالو_IRQ;
        إذا (vmx_feature_ack_on_exit) {
            مداخلت = vmread(VMX_معلومات_EXIT_مداخلت_INFO) & 0xff;
            ARCH_ڳنڍ_STATE(x86_KScur_مداخلت) = مداخلت;
            handle_مداخلت_داخلا();
        } وإلا {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(ڳنڍ_STATE(ksCurThread));
#جيڪڏهن_پورو
        أرجع EXCEPTION_NONE;
    }

    ڳنڍ_تالو_SYS;

    بدل (reason) {
    حالة EXCEPTION_OR_NMI:
    حالة MOV_DR:
    حالة TASK_SWITCH:
    حالة CONTROL_رجسٽر:
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
    حالة TPR_هيٺ_حد:
    حالة APIC_ACCESS:
        qualification = vmread(VMX_معلومات_EXIT_QUALIFICATION);
        اكسر;
    افتراضي:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    أرجع EXCEPTION_NONE;
}

exception_t handle_Vm_داخلا_Fail(فراغ)
{
    handleVmxFault(-1, -1);

    أرجع EXCEPTION_NONE;
}

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
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
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_سرگرم) {

        لفظ_t badge = notification_ptr_get_ntfn_نياپو_Identifier(ntfnPtr);
        notification_ptr_سيٽ_state(ntfnPtr, NtfnState_Idle);
        سيٽ_Thread_State(tcb, ThreadState_Running);
        سيٽ_رجسٽر(tcb, badge_رجسٽر, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        إذا (tcb != ڳنڍ_STATE(ksCurThread)) {
            possible_Switch_منزل(tcb);
        }
    }
}
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

ساكن فراغ invvpid_context(uint16_t vpid)
{
    بنية {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t پتو;
    } PACKED عمليو = {vpid, 0, 0};
    asm متطاير("invvpid %0, %1" :: "m"(عمليو), "r"((لفظ_t)1) : "cc");
}

ساكن فراغ سيٽ_EPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_پاڙ;
    إذا (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_پاڙ = kpptr_منزل_paddr(null_ept_space);
    } وإلا {
        ڳوليو_EPTFor_ASID_ret_t ڳوليو_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_بنياد_Ptr(vmxSpace);
        ڳوليو_ret = ڳوليو_EPTFor_ASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        إذا (ڳوليو_ret.حالت != EXCEPTION_NONE || ڳوليو_ret.ept != pml4) {
            ept_پاڙ = kpptr_منزل_paddr(null_ept_space);
        } وإلا {
            ept_پاڙ = pptr_منزل_paddr(pml4);
        }
    }
    إذا (ept_پاڙ != vcpu->آخري_ept_پاڙ) {
        vcpu->آخري_ept_پاڙ = ept_پاڙ;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_پاڙ,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_اشارو, eptp.لفظ[0]);
        أكد(vcpu->vpid != VPID_INVALID);
        إذا (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

فراغ clearVPIDIOPortMappings(vpid_t vpid, uint16_t پهريون, uint16_t آخري)
{
    إذا (vpid == VPID_INVALID) {
        أرجع;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    إذا (vcpu == عدم) {
        أرجع;
    }
    أكد(vcpu->vpid == vpid);
    سيٽ_IOPort_نقاب(vcpu->io, پهريون, آخري, true);
}

ساكن مضمن vpid_t ايندڙ_VPID(vpid_t vpid)
{
    إذا (vpid == VPID_آخري) {
        أرجع VPID_پهريون;
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

ساكن vpid_t ڳوليو_Free_VPID(فراغ)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    افعل {
        إذا (x86KSVPIDTable[vpid] == عدم) {
            أرجع vpid;
        }
        vpid = ايندڙ_VPID(vpid);
    } طالما (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = ايندڙ_VPID(x86KSNextVPID);
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
        إذا (!vcpu->fpu_سرگرم) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_سرگرم = true;
            ڀريو_Fpu_State(tcb);
        }
    } وإلا {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_سرگرم = true;
        switchLocalFpuOwner(tcb);
    }
}

فراغ vcpu_fpu_منزل_host(tcb_t *tcb, vcpu_t *vcpu)
{
    إذا (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } وإلا إذا (nativeThreadUsingFPU(tcb) && vcpu->fpu_سرگرم) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_سرگرم = false;
        ڀريو_Fpu_State(tcb);
    } وإلا إذا (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

فراغ restoreVMCS(فراغ)
{
    tcb_t *cur_thread = ڳنڍ_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    إذا (ARCH_ڳنڍ_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_SKIM_دري
    إذا (getCurrentCR3().لفظ[0] != expected_vmcs->آخري_host_cr3) {
        expected_vmcs->آخري_host_cr3 = getCurrentCR3().لفظ[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().لفظ[0]);
    }
#جيڪڏهن_پورو
    إذا (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = ڳوليو_Free_VPID();
        storeVPID(expected_vmcs, vpid);
        إذا (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    سيٽ_EPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

فراغ invept(ept_pml4e_t *ept_pml4)
{
    إذا (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        بنية {
            uint64_t parts[2];
        } پتو;
        لفظ_t type;
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

        پتو.parts[0] = pptr_منزل_paddr((فراغ *)ept_pml4);
        پتو.parts[1] = 0;
        asm متطاير(
            "invept %0, %1"
            :
            : "m"(پتو),  "r"(type)
            : "memory"
        );
    }
}

#جيڪڏهن_پورو
