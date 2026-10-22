/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#ifdef CONFIG_VTX

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

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

ثابت vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

تعریفنوع ساختار msr_bitmap {
    word_t bitmap[0x2000 / اندازه(word_t) / 8];
} msr_bitmap_t;

تعریفنوع ساختار msr_bitmaps {
    msr_bitmap_t کم_msr_read;
    msr_bitmap_t بالا_msr_read;
    msr_bitmap_t کم_msr_write;
    msr_bitmap_t بالا_msr_write;
} msr_bitmaps_t;

ایستا msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

ایستا نویسه null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
ایستا uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
ایستا vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
ایستا uint32_t pin_control_بالا;
ایستا uint32_t pin_control_کم;
ایستا uint32_t primary_control_بالا;
ایستا uint32_t primary_control_کم;
ایستا uint32_t secondary_control_بالا;
ایستا uint32_t secondary_control_کم;
ایستا uint32_t entry_control_بالا;
ایستا uint32_t entry_control_کم;
ایستا uint32_t exit_control_بالا;
ایستا uint32_t exit_control_کم;
ایستا uint32_t cr0_بالا;
ایستا uint32_t cr0_کم;
ایستا uint32_t cr4_بالا;
ایستا uint32_t cr4_کم;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
ایستا bool_t vmx_feature_vpid;
ایستا bool_t vmx_feature_load_perf_global_ctrl;
ایستا bool_t vmx_feature_ack_on_exit;

ایستا vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
ایستا vpid_t x86KSNextVPID = VPID_FIRST;

ایستا تهی releaseVPID(vpid_t vpid);

ایستا درونخطی bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm فرار(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    بازگشت !!error;
}

ایستا تهی vmclear(تهی *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((تهی *)vmcs_ptr);
    asm فرار(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

تهی clearCurrentVCPU(تهی)
{
    vcpu_t *vcpu = ARCH_گره_STATE(x86KSCurrentVCPU);
    اگر (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_گره_STATE(x86KSCurrentVCPU) = پوچ;
        اگر (vcpu->fpu_active && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_active = false;
        }
    }
}

ایستا تهی vmptrld(تهی *vmcs_ptr)
{
    uint64_t physical_address;
    uint8_t error;
    physical_address = pptr_to_paddr(vmcs_ptr);
    asm فرار(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_address)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    أكد(!error);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
تهی vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_STAR_MSR, vcpu->syscall_registers[VCPU_STAR]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_MASK]);
}

تهی vcpu_restore_host_msrs(تهی)
{
    init_syscall_msrs();
}
#endif

ایستا تهی switchVCPU(vcpu_t *vcpu)
{
#ifdef ENABLE_SMP_SUPPORT
    اگر (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_گره_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    اگر (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((نویسه *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.واژگان[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_گره_STATE(x86KSCurrentVCPU) = vcpu;
}

ایستا تهی print_bits(word_t bits)
{
    bool_t first = true;
    وقتی (bits) {
        صحیح index = seL4_WordBits - 1 - clzl(bits);
        اگر (first) {
            printf("%d", index);
            first = false;
        } وگرنه {
            printf(",%d", index);
        }
        bits &= ~BIT(index);
    }
}

ایستا bool_t check_fixed_مقدار(word_t val, word_t کم, word_t بالا)
{
    word_t not_بالا;
    word_t not_کم;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_بالا = بالا & ~(بالا & val);
    اگر (not_بالا != 0) {
        printf("Failed to set bits: ");
        print_bits(not_بالا);
        بازگشت false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_کم = ~کم & ~(~کم & ~val);
    اگر (not_کم != 0) {
        printf("Incorrectly cleared bits: ");
        print_bits(not_کم);
        بازگشت false;
    }
    بازگشت true;
}

ایستا bool_t vtx_check_fixed_مقادیر(word_t cr0, word_t cr4)
{
    اگر (!check_fixed_مقدار(cr0, cr0_کم, cr0_بالا)) {
        printf(" of CR0\n");
        بازگشت false;
    }
    اگر (!check_fixed_مقدار(cr4, cr4_کم, cr4_بالا)) {
        printf(" of CR4\n");
        بازگشت false;
    }
    بازگشت true;
}

ایستا bool_t BOOT_CODE init_vtx_fixed_مقادیر(bool_t useTrueMsrs)
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
    اگر (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } وگرنه {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    pin_control_بالا = x86_rdmsr_کم(pinbased_ctls);
    pin_control_کم = x86_rdmsr_بالا(pinbased_ctls);
    primary_control_بالا = x86_rdmsr_کم(procbased_ctls);
    primary_control_کم = x86_rdmsr_بالا(procbased_ctls);
    secondary_control_بالا = x86_rdmsr_کم(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_کم = x86_rdmsr_بالا(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_بالا = x86_rdmsr_کم(exit_ctls);
    exit_control_کم = x86_rdmsr_بالا(exit_ctls);
    entry_control_بالا = x86_rdmsr_کم(entry_ctls);
    entry_control_کم = x86_rdmsr_بالا(entry_ctls);

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
    اگر (secondary_control_کم & BIT(7)) {
        cr0_بالا &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    اگر (!(secondary_control_کم & BIT(5))) {
        vmx_feature_vpid = 0;
        printf("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } وگرنه {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    اگر (!(exit_control_کم & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        printf("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } وگرنه {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    اگر (!(exit_control_کم & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        printf("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } وگرنه {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_کم) & pin_control_mask;
    اگر (missing) {
        printf("vt-x: Unsupported pin control features %lx\n", (long)missing);
        بازگشت false;
    }
    missing = (~primary_control_کم) & primary_control_mask;
    اگر (missing) {
        printf("vt-x: Unsupported primary control features %lx\n", (long)missing);
        بازگشت false;
    }
    missing = (~secondary_control_کم) & secondary_control_mask;
    اگر (missing) {
        printf("vt-x: Unsupported secondary control features %lx\n", (long)missing);
        بازگشت false;
    }
    missing = (~exit_control_کم) & exit_control_mask;
    اگر (missing) {
        printf("vt-x: Unsupported exit control features %lx\n", (long)missing);
        بازگشت false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_کم) & BIT(9);
    اگر (missing) {
        printf("vt-x: CPU does not support 64-bit guests\n");
        بازگشت false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_بالا |= pin_control_mask;
    primary_control_بالا |= primary_control_mask;
    secondary_control_بالا |= secondary_control_mask;
    exit_control_بالا |= exit_control_mask;

    بازگشت true;
}

ایستا bool_t BOOT_CODE check_vtx_fixed_مقادیر(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    اگر (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } وگرنه {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    uint32_t local_pin_control_بالا = x86_rdmsr_کم(pinbased_ctls);
    uint32_t local_pin_control_کم = x86_rdmsr_بالا(pinbased_ctls);
    uint32_t local_primary_control_بالا = x86_rdmsr_کم(procbased_ctls);
    uint32_t local_primary_control_کم = x86_rdmsr_بالا(procbased_ctls);
    uint32_t local_secondary_control_بالا = x86_rdmsr_کم(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_کم = x86_rdmsr_بالا(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_بالا = x86_rdmsr_کم(exit_ctls);
    uint32_t local_exit_control_کم = x86_rdmsr_بالا(exit_ctls);
    uint32_t local_entry_control_بالا = x86_rdmsr_کم(entry_ctls);
    uint32_t local_entry_control_کم = x86_rdmsr_بالا(entry_ctls);

    uint32_t local_cr0_بالا = x86_rdmsr_کم(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_کم = x86_rdmsr_کم(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_بالا = x86_rdmsr_کم(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_کم = x86_rdmsr_کم(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    اگر (local_secondary_control_کم & BIT(7)) {
        local_cr0_بالا &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    بازگشت
        (local_pin_control_بالا & pin_control_بالا) == local_pin_control_بالا &&
        (~local_pin_control_کم & pin_control_بالا) == 0 &&
        (local_primary_control_بالا & primary_control_بالا) == local_primary_control_بالا &&
        (~local_primary_control_کم & primary_control_بالا) == 0 &&
        (local_secondary_control_بالا & secondary_control_بالا) == local_secondary_control_بالا &&
        (~local_secondary_control_کم & secondary_control_بالا) == 0 &&
        (local_exit_control_بالا & exit_control_بالا) == local_exit_control_بالا &&
        (~local_exit_control_کم & exit_control_بالا) == 0 &&
        (local_entry_control_بالا & entry_control_بالا) == local_entry_control_بالا &&
        (~local_entry_control_کم & entry_control_بالا) == 0 &&
        local_cr0_بالا == cr0_بالا &&
        local_cr0_کم == cr0_کم &&
        local_cr4_بالا == cr4_بالا &&
        local_cr4_کم == cr4_کم;
}

ایستا درونخطی uint32_t applyFixedBits(uint32_t original, uint32_t بالا, uint32_t کم)
{
    original |= بالا;
    original &= کم;
    بازگشت original;
}

تهی vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = پوچ;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    memcpy(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_بالا & cr0_کم;
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
    اگر (vmx_feature_load_perf_global_ctrl) {
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
    اگر (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((نویسه *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.واژگان[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_register]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_to_paddr(x64KSKernelPML4), 0).واژگان[0]);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */

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

    uint64_t entry_control = entry_control_بالا & entry_control_کم;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_بالا & cr4_کم);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_بالا, ~(word_t)0);

    memset(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_SIZE);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((نویسه *)vcpu->io + (VCPU_IOBITMAP_SIZE / 2)));
}

ایستا تهی dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    أكد(tcb->tcbArch.tcbVCPU == vcpu);
    أكد(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = پوچ;
    vcpu->vcpuTCB = پوچ;
}

تهی vcpu_finalise(vcpu_t *vcpu)
{
    اگر (ARCH_گره_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
#ifdef ENABLE_SMP_SUPPORT
        اگر (vcpu->last_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->last_cpu);
        } وگرنه
#endif /* ENABLE_SMP_SUPPORT */
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

ایستا تهی associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
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

ایستا exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *buffer)
{
    صحیح i;
    برای (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, buffer);
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeVCPUWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    اگر (length < 15) {
#وگرنه
    اگر (length < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    بازگشت invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
ایستا exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *buffer, word_t field, word_t مقدار)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    اگر (ARCH_گره_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    انتخاب (field) {
    حالت IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = مقدار;
        بشکن;
    حالت IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = مقدار;
        بشکن;
    حالت IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = مقدار;
        بشکن;
    حالت IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = مقدار;
        بشکن;
    }
    setMR(thread, buffer, 0, مقدار);
    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeVCPUWriteMSR(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t مقدار;

    اگر (length < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    مقدار = getSyscallArg(1, buffer);
    انتخاب (field) {
    حالت IA32_LSTAR_MSR:
    حالت IA32_STAR_MSR:
    حالت IA32_CSTAR_MSR:
    حالت IA32_FMASK_MSR:
        بشکن;
    پیشفرض:
        userError("VCPU WriteMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    بازگشت invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer, field, مقدار);
}

ایستا exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *buffer)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);

    word_t مقدار = 0;

    انتخاب (field) {
    حالت IA32_LSTAR_MSR:
        مقدار = vcpu->syscall_registers[VCPU_LSTAR];
        بشکن;
    حالت IA32_STAR_MSR:
        مقدار = vcpu->syscall_registers[VCPU_STAR];
        بشکن;
    حالت IA32_CSTAR_MSR:
        مقدار = vcpu->syscall_registers[VCPU_CSTAR];
        بشکن;
    حالت IA32_FMASK_MSR:
        مقدار = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        بشکن;
    حالت IA32_VMX_MISC_MSR:
        مقدار = x86_rdmsr(field);
        بشکن;
    }

    setMR(thread, buffer, 0, مقدار);
    setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeVCPUReadMSR(cap_t cap, word_t length, word_t *buffer)
{
    اگر (length < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    انتخاب (field) {
    حالت IA32_LSTAR_MSR:
    حالت IA32_STAR_MSR:
    حالت IA32_CSTAR_MSR:
    حالت IA32_FMASK_MSR:
    حالت IA32_VMX_MISC_MSR:
        بشکن;
    پیشفرض:
        userError("VCPU ReadMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    بازگشت invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, buffer);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

ایستا exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t کم, uint16_t بالا)
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
    setIOPortMask(vcpu->io, کم, بالا, false);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeEnableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t کم, بالا;

    اگر (length < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (current_extra_caps.excaprefs[0] == پوچ) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    کم = getSyscallArg(0, buffer);
    بالا = getSyscallArg(1, buffer);

    اگر (کم < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || بالا > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 0;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeEnableIOPort(vcpu, ioSlot, ioCap, کم, بالا);
}

ایستا exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t کم, uint16_t بالا)
{
    setIOPortMask(vcpu->io, کم, بالا, true);
    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeDisableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    uint16_t کم, بالا;

    اگر (length < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    کم = getSyscallArg(0, buffer);
    بالا = getSyscallArg(1, buffer);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    بازگشت invokeDisableIOPort(vcpu, کم, بالا);
}

ایستا exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *buffer, word_t field, word_t مقدار)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);

    اگر (ARCH_گره_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    انتخاب (field) {
    حالت VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = مقدار;
        بشکن;
    حالت VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = مقدار;
        بشکن;
    حالت VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = مقدار;
        بشکن;
    حالت VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = مقدار;
        بشکن;
    }
    vmwrite(field, مقدار);

    اگر (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned صحیح length = setMR(thread, buffer, 0, مقدار);
        setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeWriteVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    word_t مقدار;

    اگر (length < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    مقدار = getSyscallArg(1, buffer);
    انتخاب (field) {
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
    حالت VMX_GUEST_ES_LIMIT:
    حالت VMX_GUEST_CS_LIMIT:
    حالت VMX_GUEST_SS_LIMIT:
    حالت VMX_GUEST_DS_LIMIT:
    حالت VMX_GUEST_FS_LIMIT:
    حالت VMX_GUEST_GS_LIMIT:
    حالت VMX_GUEST_LDTR_LIMIT:
    حالت VMX_GUEST_TR_LIMIT:
    حالت VMX_GUEST_GDTR_LIMIT:
    حالت VMX_GUEST_IDTR_LIMIT:
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
    حالت VMX_GUEST_PREEMPTION_TIMER_مقدار:
    حالت VMX_GUEST_ES_BASE:
    حالت VMX_GUEST_CS_BASE:
    حالت VMX_GUEST_SS_BASE:
    حالت VMX_GUEST_DS_BASE:
    حالت VMX_GUEST_FS_BASE:
    حالت VMX_GUEST_GS_BASE:
    حالت VMX_GUEST_LDTR_BASE:
    حالت VMX_GUEST_TR_BASE:
    حالت VMX_GUEST_GDTR_BASE:
    حالت VMX_GUEST_IDTR_BASE:
    حالت VMX_GUEST_DR7:
    حالت VMX_GUEST_RFLAGS:
    حالت VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالت VMX_GUEST_SYSENTER_ESP:
    حالت VMX_GUEST_SYSENTER_EIP:
    حالت VMX_CONTROL_CR0_MASK:
    حالت VMX_CONTROL_CR4_MASK:
    حالت VMX_CONTROL_CR0_READ_SHADOW:
    حالت VMX_CONTROL_CR4_READ_SHADOW:
    حالت VMX_GUEST_CR3:
    حالت VMX_CONTROL_EXCEPTION_BITMAP:
    حالت VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    حالت VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        بشکن;
    حالت VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        مقدار = applyFixedBits(مقدار, pin_control_بالا, pin_control_کم);
        بشکن;
    حالت VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        مقدار = applyFixedBits(مقدار, primary_control_بالا, primary_control_کم);
        بشکن;
    حالت VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        مقدار = applyFixedBits(مقدار, secondary_control_بالا, secondary_control_کم);
        بشکن;
    حالت VMX_CONTROL_ENTRY_CONTROLS:
        مقدار = applyFixedBits(مقدار, entry_control_بالا, entry_control_کم);
        بشکن;
    حالت VMX_CONTROL_EXIT_CONTROLS:
        مقدار = applyFixedBits(مقدار, exit_control_بالا, exit_control_کم);
        بشکن;
    حالت VMX_GUEST_CR0:
        مقدار = applyFixedBits(مقدار, cr0_بالا, cr0_کم);
        بشکن;
    حالت VMX_GUEST_CR4:
        مقدار = applyFixedBits(مقدار, cr4_بالا, cr4_کم);
        بشکن;
    پیشفرض:
        userError("VCPU WriteVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    بازگشت invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, buffer, field, مقدار);
}

ایستا word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    انتخاب (field) {
    حالت VMX_CONTROL_EXCEPTION_BITMAP:
        بازگشت vcpu->exception_bitmap;
    حالت VMX_GUEST_CR0:
        بازگشت vcpu->cr0;
    حالت VMX_CONTROL_CR0_MASK:
        بازگشت vcpu->cr0_mask;
    حالت VMX_CONTROL_CR0_READ_SHADOW:
        بازگشت vcpu->cr0_shadow;
    }
    اگر (ARCH_گره_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    بازگشت vmread(field);
}

ایستا exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *buffer)
{
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    word_t مقدار = readVMCSField(vcpu, field);
    اگر (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned صحیح length = setMR(thread, buffer, 0, مقدار);
        setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeReadVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    اگر (length < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    انتخاب (field) {
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
    حالت VMX_GUEST_ES_LIMIT:
    حالت VMX_GUEST_CS_LIMIT:
    حالت VMX_GUEST_SS_LIMIT:
    حالت VMX_GUEST_DS_LIMIT:
    حالت VMX_GUEST_FS_LIMIT:
    حالت VMX_GUEST_GS_LIMIT:
    حالت VMX_GUEST_LDTR_LIMIT:
    حالت VMX_GUEST_TR_LIMIT:
    حالت VMX_GUEST_GDTR_LIMIT:
    حالت VMX_GUEST_IDTR_LIMIT:
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
    حالت VMX_GUEST_PREEMPTION_TIMER_مقدار:
    حالت VMX_GUEST_ES_BASE:
    حالت VMX_GUEST_CS_BASE:
    حالت VMX_GUEST_SS_BASE:
    حالت VMX_GUEST_DS_BASE:
    حالت VMX_GUEST_FS_BASE:
    حالت VMX_GUEST_GS_BASE:
    حالت VMX_GUEST_LDTR_BASE:
    حالت VMX_GUEST_TR_BASE:
    حالت VMX_GUEST_GDTR_BASE:
    حالت VMX_GUEST_IDTR_BASE:
    حالت VMX_GUEST_DR7:
    حالت VMX_GUEST_RFLAGS:
    حالت VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    حالت VMX_GUEST_SYSENTER_ESP:
    حالت VMX_GUEST_SYSENTER_EIP:
    حالت VMX_CONTROL_CR0_MASK:
    حالت VMX_CONTROL_CR4_MASK:
    حالت VMX_CONTROL_CR0_READ_SHADOW:
    حالت VMX_CONTROL_CR4_READ_SHADOW:
    حالت VMX_DATA_INSTRUCTION_ERROR:
    حالت VMX_DATA_EXIT_INTERRUPT_INFO:
    حالت VMX_DATA_EXIT_INTERRUPT_ERROR:
    حالت VMX_DATA_IDT_VECTOR_INFO:
    حالت VMX_DATA_IDT_VECTOR_ERROR:
    حالت VMX_DATA_EXIT_INSTRUCTION_LENGTH:
    حالت VMX_DATA_EXIT_INSTRUCTION_INFO:
    حالت VMX_DATA_GUEST_PHYSICAL:
    حالت VMX_DATA_IO_RCX:
    حالت VMX_DATA_IO_RSI:
    حالت VMX_DATA_IO_RDI:
    حالت VMX_DATA_IO_RIP:
    حالت VMX_DATA_GUEST_LINEAR_ADDRESS:
    حالت VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    حالت VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    حالت VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    حالت VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    حالت VMX_CONTROL_EXCEPTION_BITMAP:
    حالت VMX_CONTROL_ENTRY_CONTROLS:
    حالت VMX_CONTROL_EXIT_CONTROLS:
    حالت VMX_GUEST_CR0:
    حالت VMX_GUEST_CR3:
    حالت VMX_GUEST_CR4:
        بشکن;
    پیشفرض:
        userError("VCPU ReadVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    بازگشت invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, buffer);
}

ایستا exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeSetTCB(cap_t cap, word_t length, word_t *buffer)
{
    cap_t tcbCap;
    اگر (current_extra_caps.excaprefs[0] == پوچ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    اگر (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    بازگشت invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

تهی vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *buffer;
    اگر (ARCH_گره_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    buffer = lookupIPCBuffer(false, گره_STATE(ksCurThread));
    اگر (!buffer) {
        userError("No IPC buffer.");
        بازگشت;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, buffer));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, buffer), primary_control_بالا,
                                                                   primary_control_کم));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, buffer));
}

تهی vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *buffer;
    vcpu_t *vcpu;

    buffer = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    أكد(vcpu);

    اگر (ARCH_گره_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, پیام_Info_Register, 0);
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
    انتخاب (invLabel) {
    حالت X86VCPUSetTCB:
        بازگشت decodeSetTCB(cap, length, buffer);
    حالت X86VCPUReadVMCS:
        بازگشت decodeReadVMCS(cap, length, call, buffer);
    حالت X86VCPUWriteVMCS:
        بازگشت decodeWriteVMCS(cap, length, call, buffer);
    حالت X86VCPUEnableIOPort:
        بازگشت decodeEnableIOPort(cap, length, buffer);
    حالت X86VCPUDisableIOPort:
        بازگشت decodeDisableIOPort(cap, length, buffer);
    حالت X86VCPUWriteRegisters:
        بازگشت decodeVCPUWriteRegisters(cap, length, buffer);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    حالت X86VCPUWriteMSR:
        بازگشت decodeVCPUWriteMSR(cap, length, buffer);
    حالت X86VCPUReadMSR:
        بازگشت decodeVCPUReadMSR(cap, length, buffer);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    پیشفرض:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}

ایستا bool_t is_vtx_supported(تهی)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    بازگشت !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

ایستا درونخطی تهی clear_bit(word_t *bitmap, word_t bit)
{
    صحیح index = bit / (اندازه(word_t) * 8);
    صحیح offset = bit % (اندازه(word_t) * 8);
    bitmap[index] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(تهی)
{
    اگر (!is_vtx_supported()) {
        printf("vt-x: not supported\n");
        بازگشت false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.واژگان[0] = x86_rdmsr_کم(IA32_VMX_BASIC_MSR);
    vmx_basic.واژگان[1] = x86_rdmsr_بالا(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.واژگان[0] = x86_rdmsr_کم(IA32_FEATURE_CONTROL_MSR);
    اگر (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        اگر (feature_control_msr_get_lock(feature_control)) {
            printf("vt-x: feature locked\n");
            بازگشت false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_بالا(IA32_FEATURE_CONTROL_MSR), feature_control.واژگان[0]);
    }
    /* make sure the msr is locked */
    اگر (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_بالا(IA32_FEATURE_CONTROL_MSR), feature_control.واژگان[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    اگر (CURRENT_CPU_INDEX() == 0) {
        اگر (!init_vtx_fixed_مقادیر(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            printf("vt-x: lack of required features\n");
            بازگشت false;
        }
    }
    اگر (!check_vtx_fixed_مقادیر(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        printf("vt-x: cores have inconsistent features\n");
        بازگشت false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    ساختار vmxon_region *vmxon_kptr = &ARCH_گره_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    اگر (!vtx_check_fixed_مقادیر(read_cr0(), read_cr4())) {
        بازگشت false;
    }
    اگر (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        printf("vt-x: vmxon failure\n");
        بازگشت false;
    }
    memset(&msr_bitmap_region, ~0, اندازه(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.کم_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.کم_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.کم_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.کم_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.کم_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.کم_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.بالا_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.بالا_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.بالا_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.بالا_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.بالا_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.بالا_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.واژگان[0] = x86_rdmsr_کم(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.واژگان[1] = x86_rdmsr_بالا(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    اگر (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        printf("vt-x: Expected wb attribute for EPT paging structure\n");
        بازگشت false;
    }
    اگر (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        printf("vt-x: Expected supported for 2m pages\n");
        بازگشت false;
    }

    بازگشت true;
}

ایستا تهی setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *buffer;
    صحیح i;

    buffer = lookupIPCBuffer(true, گره_STATE(ksCurThread));

    setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_DATA_EXIT_INSTRUCTION_LENGTH));
    setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_DATA_GUEST_PHYSICAL));
    setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    برای (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(گره_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_EAX + i,
              گره_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

ایستا تهی handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(گره_STATE(ksCurThread), پیام_Info_Register, SEL4_VMENTER_RESULT_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(گره_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

ایستا درونخطی تهی finishVmexitSaving(تهی)
{
    vcpu_t *vcpu = ARCH_گره_STATE(x86KSCurrentVCPU);
    أكد(vcpu == گره_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(تهی)
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
    اگر (reason == EXTERNAL_INTERRUPT) {
        گره_LOCK_IRQ;
        اگر (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_DATA_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_گره_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } وگرنه {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(گره_STATE(ksCurThread));
#endif
        بازگشت EXCEPTION_NONE;
    }

    گره_LOCK_SYS;

    انتخاب (reason) {
    حالت EXCEPTION_OR_NMI:
    حالت MOV_DR:
    حالت TASK_SWITCH:
    حالت CONTROL_REGISTER:
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
    حالت TPR_BELOW_THRESHOLD:
    حالت APIC_ACCESS:
        qualification = vmread(VMX_DATA_EXIT_QUALIFICATION);
        بشکن;
    پیشفرض:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    بازگشت EXCEPTION_NONE;
}

exception_t handleVmEntryFail(تهی)
{
    handleVmxFault(-1, -1);

    بازگشت EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
تهی VMCheckBoundNotification(tcb_t *tcb)
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
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {

        word_t badge = notification_ptr_get_ntfn_پیام_Identifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        اگر (tcb != گره_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

ایستا تهی invvpid_context(uint16_t vpid)
{
    ساختار {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    asm فرار("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
}

ایستا تهی setEPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_ریشه;
    اگر (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_ریشه = kpptr_to_paddr(null_ept_space);
    } وگرنه {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        اگر (find_ret.status != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_ریشه = kpptr_to_paddr(null_ept_space);
        } وگرنه {
            ept_ریشه = pptr_to_paddr(pml4);
        }
    }
    اگر (ept_ریشه != vcpu->last_ept_ریشه) {
        vcpu->last_ept_ریشه = ept_ریشه;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_ریشه,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.واژگان[0]);
        أكد(vcpu->vpid != VPID_INVALID);
        اگر (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

تهی clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t last)
{
    اگر (vpid == VPID_INVALID) {
        بازگشت;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    اگر (vcpu == پوچ) {
        بازگشت;
    }
    أكد(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

ایستا درونخطی vpid_t nextVPID(vpid_t vpid)
{
    اگر (vpid == VPID_LAST) {
        بازگشت VPID_FIRST;
    } وگرنه {
        بازگشت vpid + 1;
    }
}

ایستا تهی invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, اندازه(vcpu->io));
    /* invalidate the VPID context */
    اگر (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
ایستا تهی releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = پوچ;
}

ایستا vpid_t findFreeVPID(تهی)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    انجامبده {
        اگر (x86KSVPIDTable[vpid] == پوچ) {
            بازگشت vpid;
        }
        vpid = nextVPID(vpid);
    } وقتی (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = nextVPID(x86KSNextVPID);
    بازگشت vpid;
}

ایستا تهی storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    أكد(x86KSVPIDTable[vpid] == پوچ);
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
تهی vcpu_fpu_to_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    اگر (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        اگر (!vcpu->fpu_active) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_active = true;
            loadFpuState(tcb);
        }
    } وگرنه {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_active = true;
        switchLocalFpuOwner(tcb);
    }
}

تهی vcpu_fpu_to_host(tcb_t *tcb, vcpu_t *vcpu)
{
    اگر (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } وگرنه اگر (nativeThreadUsingFPU(tcb) && vcpu->fpu_active) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_active = false;
        loadFpuState(tcb);
    } وگرنه اگر (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

تهی restoreVMCS(تهی)
{
    tcb_t *cur_thread = گره_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    اگر (ARCH_گره_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ifndef CONFIG_KERNEL_SKIM_WINDOW
    اگر (getCurrentCR3().واژگان[0] != expected_vmcs->last_host_cr3) {
        expected_vmcs->last_host_cr3 = getCurrentCR3().واژگان[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().واژگان[0]);
    }
#endif
    اگر (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        اگر (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    setEPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

تهی invept(ept_pml4e_t *ept_pml4)
{
    اگر (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        ساختار {
            uint64_t parts[2];
        } address;
        word_t type;
        اگر (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } وگرنه اگر (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } وگرنه {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            بازگشت;
        }

        address.parts[0] = pptr_to_paddr((تهی *)ept_pml4);
        address.parts[1] = 0;
        asm فرار(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
