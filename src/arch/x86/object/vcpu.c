/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>

#ifdef CONFIG_VTX

#include <types.h>
#include <machine/io.h>
#include <api/failures.h>
#include <api/syscall.h>
#include <kernel/thread.h>
#include <object/objecttype.h>
#include <arch/machine/cpu_registers.h>
#include <arch/model/statedata.h>
#include <arch/object/vcpu.h>
#include <arch/object/ioport.h>
#include <util.h>
#include <sel4/arch/vmenter.h>

#define VMX_EXIT_QUAL_TYPE_MOV_CR 0
#define VMX_EXIT_QUAL_TYPE_CLTS 2
#define VMX_EXIT_QUAL_TYPE_LMSW 3

const vcpu_gp_register_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#define MSR_BITMAP_MASK(x) ((x) & 0x1fff)

typedef sa_nracanaa msr_bitmap {
    word_t bitmap[0x2000 / parimaa_na(word_t) / 8];
} msr_bitmap_t;

typedef sa_nracanaa msr_bitmaps {
    msr_bitmap_t low_msr_read;
    msr_bitmap_t high_msr_read;
    msr_bitmap_t low_msr_write;
    msr_bitmap_t high_msr_write;
} msr_bitmaps_t;

static msr_bitmaps_t msr_bitmap_region ALIGN(BIT(seL4_PageBits));

static var_na null_ept_space[BIT(seL4_PageBits)] ALIGN(BIT(seL4_PageBits));

/* Cached value of the hardware defined vmcs revision */
static uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
static vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
static uint32_t pin_control_high;
static uint32_t pin_control_low;
static uint32_t primary_control_high;
static uint32_t primary_control_low;
static uint32_t secondary_control_high;
static uint32_t secondary_control_low;
static uint32_t entry_control_high;
static uint32_t entry_control_low;
static uint32_t exit_control_high;
static uint32_t exit_control_low;
static uint32_t cr0_high;
static uint32_t cr0_low;
static uint32_t cr4_high;
static uint32_t cr4_low;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
static bool_t vmx_feature_vpid;
static bool_t vmx_feature_load_perf_global_ctrl;
static bool_t vmx_feature_ack_on_exit;

static vcpu_t *x86KSVPIDTable[VPID_LAST + 1];
static vpid_t x86KSNextVPID = VPID_FIRST;

static void releaseVPID(vpid_t vpid);

static anta_hstha bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm ca_ncala(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    ᰀᰧᰌ !!error;
}

static void vmclear(void *vmcs_ptr)
{
    uint64_t physical_address;
    physical_address = pptr_to_paddr((void *)vmcs_ptr);
    asm ca_ncala(
        "vmclear %0"
        :
        : "m"(physical_address)
        : "cc"
    );
}

void clearCurrentVCPU(void)
{
    vcpu_t *vcpu = ARCH_NODE_STATE(x86KSCurrentVCPU);
    ᰀᰧ (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_NODE_STATE(x86KSCurrentVCPU) = NULL;
        ᰀᰧ (vcpu->fpu_active && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_active = false;
        }
    }
}

static void vmptrld(void *vmcs_ptr)
{
    uint64_t physical_address;
    uint8_t error;
    physical_address = pptr_to_paddr(vmcs_ptr);
    asm ca_ncala(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_address)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    assert(!error);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
void vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_STAR_MSR, vcpu->syscall_registers[VCPU_STAR]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_MASK]);
}

void vcpu_restore_host_msrs(void)
{
    init_syscall_msrs();
}
#endif

static void switchVCPU(vcpu_t *vcpu)
{
#ifdef ENABLE_SMP_SUPPORT
    ᰀᰧ (vcpu->last_cpu != getCurrentCPUIndex() && ARCH_NODE_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->last_cpu);
    }
#endif
    clearCurrentVCPU();
    vmptrld(vcpu);
#ifdef ENABLE_SMP_SUPPORT
    ᰀᰧ (vcpu->last_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_BASE, (word_t)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss);
        vmwrite(VMX_HOST_GDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_BASE, (word_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((var_na *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.words[0] +
                                                          4));
    }
    vcpu->last_cpu = getCurrentCPUIndex();
#endif
    ARCH_NODE_STATE(x86KSCurrentVCPU) = vcpu;
}

static void print_bits(word_t bits)
{
    bool_t first = true;
    ᰀᰬ (bits) {
        puur_naa_nka index = seL4_WordBits - 1 - clzl(bits);
        ᰀᰧ (first) {
            printf("%d", index);
            first = false;
        } ᰀᰧᰀ {
            printf(",%d", index);
        }
        bits &= ~BIT(index);
    }
}

static bool_t check_fixed_value(word_t val, word_t low, word_t high)
{
    word_t not_high;
    word_t not_low;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_high = high & ~(high & val);
    ᰀᰧ (not_high != 0) {
        printf("Failed to set bits: ");
        print_bits(not_high);
        ᰀᰧᰌ false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_low = ~low & ~(~low & ~val);
    ᰀᰧ (not_low != 0) {
        printf("Incorrectly cleared bits: ");
        print_bits(not_low);
        ᰀᰧᰌ false;
    }
    ᰀᰧᰌ true;
}

static bool_t vtx_check_fixed_values(word_t cr0, word_t cr4)
{
    ᰀᰧ (!check_fixed_value(cr0, cr0_low, cr0_high)) {
        printf(" of CR0\n");
        ᰀᰧᰌ false;
    }
    ᰀᰧ (!check_fixed_value(cr4, cr4_low, cr4_high)) {
        printf(" of CR4\n");
        ᰀᰧᰌ false;
    }
    ᰀᰧᰌ true;
}

static bool_t BOOT_CODE init_vtx_fixed_values(bool_t useTrueMsrs)
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
    ᰀᰧ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } ᰀᰧᰀ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    pin_control_high = x86_rdmsr_low(pinbased_ctls);
    pin_control_low = x86_rdmsr_high(pinbased_ctls);
    primary_control_high = x86_rdmsr_low(procbased_ctls);
    primary_control_low = x86_rdmsr_high(procbased_ctls);
    secondary_control_high = x86_rdmsr_low(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_low = x86_rdmsr_high(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_high = x86_rdmsr_low(exit_ctls);
    exit_control_low = x86_rdmsr_high(exit_ctls);
    entry_control_high = x86_rdmsr_low(entry_ctls);
    entry_control_low = x86_rdmsr_high(entry_ctls);

    cr0_high = x86_rdmsr_low(IA32_VMX_CR0_FIXED0_MSR);
    cr0_low = x86_rdmsr_low(IA32_VMX_CR0_FIXED1_MSR);
    cr4_high = x86_rdmsr_low(IA32_VMX_CR4_FIXED0_MSR);
    cr4_low = x86_rdmsr_low(IA32_VMX_CR4_FIXED1_MSR);

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
    ᰀᰧ (secondary_control_low & BIT(7)) {
        cr0_high &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    ᰀᰧ (!(secondary_control_low & BIT(5))) {
        vmx_feature_vpid = 0;
        printf("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } ᰀᰧᰀ {
        vmx_feature_vpid = 1;
        secondary_control_mask |= BIT(5);
    }

    /* Check for load perf global control */
    ᰀᰧ (!(exit_control_low & BIT(12))) {
        vmx_feature_load_perf_global_ctrl = 0;
        printf("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } ᰀᰧᰀ {
        vmx_feature_load_perf_global_ctrl = 1;
        exit_control_mask |= BIT(12);
    }

    /* Check for external interrupt exiting */
    ᰀᰧ (!(exit_control_low & BIT(15))) {
        vmx_feature_ack_on_exit = 0;
        printf("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } ᰀᰧᰀ {
        vmx_feature_ack_on_exit = 1;
        exit_control_mask |= BIT(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_low) & pin_control_mask;
    ᰀᰧ (missing) {
        printf("vt-x: Unsupported pin control features %lx\n", (long)missing);
        ᰀᰧᰌ false;
    }
    missing = (~primary_control_low) & primary_control_mask;
    ᰀᰧ (missing) {
        printf("vt-x: Unsupported primary control features %lx\n", (long)missing);
        ᰀᰧᰌ false;
    }
    missing = (~secondary_control_low) & secondary_control_mask;
    ᰀᰧ (missing) {
        printf("vt-x: Unsupported secondary control features %lx\n", (long)missing);
        ᰀᰧᰌ false;
    }
    missing = (~exit_control_low) & exit_control_mask;
    ᰀᰧ (missing) {
        printf("vt-x: Unsupported exit control features %lx\n", (long)missing);
        ᰀᰧᰌ false;
    }
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    missing = (~entry_control_low) & BIT(9);
    ᰀᰧ (missing) {
        printf("vt-x: CPU does not support 64-bit guests\n");
        ᰀᰧᰌ false;
    }
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_high |= pin_control_mask;
    primary_control_high |= primary_control_mask;
    secondary_control_high |= secondary_control_mask;
    exit_control_high |= exit_control_mask;

    ᰀᰧᰌ true;
}

static bool_t BOOT_CODE check_vtx_fixed_values(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t entry_ctls;
    ᰀᰧ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_TRUE_ENTRY_CTLS_MSR;
    } ᰀᰧᰀ {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        entry_ctls = IA32_VMX_ENTRY_CTLS_MSR;
    }
    uint32_t local_pin_control_high = x86_rdmsr_low(pinbased_ctls);
    uint32_t local_pin_control_low = x86_rdmsr_high(pinbased_ctls);
    uint32_t local_primary_control_high = x86_rdmsr_low(procbased_ctls);
    uint32_t local_primary_control_low = x86_rdmsr_high(procbased_ctls);
    uint32_t local_secondary_control_high = x86_rdmsr_low(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_low = x86_rdmsr_high(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_high = x86_rdmsr_low(exit_ctls);
    uint32_t local_exit_control_low = x86_rdmsr_high(exit_ctls);
    uint32_t local_entry_control_high = x86_rdmsr_low(entry_ctls);
    uint32_t local_entry_control_low = x86_rdmsr_high(entry_ctls);

    uint32_t local_cr0_high = x86_rdmsr_low(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_low = x86_rdmsr_low(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_high = x86_rdmsr_low(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_low = x86_rdmsr_low(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    ᰀᰧ (local_secondary_control_low & BIT(7)) {
        local_cr0_high &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    ᰀᰧᰌ
        (local_pin_control_high & pin_control_high) == local_pin_control_high &&
        (~local_pin_control_low & pin_control_high) == 0 &&
        (local_primary_control_high & primary_control_high) == local_primary_control_high &&
        (~local_primary_control_low & primary_control_high) == 0 &&
        (local_secondary_control_high & secondary_control_high) == local_secondary_control_high &&
        (~local_secondary_control_low & secondary_control_high) == 0 &&
        (local_exit_control_high & exit_control_high) == local_exit_control_high &&
        (~local_exit_control_low & exit_control_high) == 0 &&
        (local_entry_control_high & entry_control_high) == local_entry_control_high &&
        (~local_entry_control_low & entry_control_high) == 0 &&
        local_cr0_high == cr0_high &&
        local_cr0_low == cr0_low &&
        local_cr4_high == cr4_high &&
        local_cr4_low == cr4_low;
}

static anta_hstha uint32_t applyFixedBits(uint32_t original, uint32_t high, uint32_t low)
{
    original |= high;
    original &= low;
    ᰀᰧᰌ original;
}

void vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = NULL;
    vcpu->launched = false;
    vcpu->fpu_active = false;
    vcpu->fpuState = x86KSnullFpuState;

    memcpy(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_high & cr0_low;
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
    ᰀᰧ (vmx_feature_load_perf_global_ctrl) {
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
    ᰀᰧ (!config_set(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(word_t)((var_na *)&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss.words[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (word_t)&vcpu->gp_registers[n_vcpu_gp_register]);
    vmwrite(VMX_HOST_RIP, (word_t)&handle_vmexit);
#ifdef CONFIG_KERNEL_SKIM_WINDOW
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_to_paddr(x64KSKernelPML4), 0).words[0]);
#endif /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_high & pin_control_low);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_high & primary_control_low);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_high & secondary_control_low);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_high & exit_control_low);

    uint64_t entry_control = entry_control_high & entry_control_low;
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    entry_control |= BIT(9);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_ENTRY_CONTROLS, entry_control);

    vmwrite(VMX_CONTROL_MSR_ADDRESS, (word_t)kpptr_to_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_high & cr4_low);

    vmwrite(VMX_GUEST_VMCS_LINK_POINTER, ~(word_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_POINTER_HIGH, ~(word_t)0);

    memset(vcpu->io, ~(word_t)0, VCPU_IOBITMAP_SIZE);
    vmwrite(VMX_CONTROL_IOA_ADDRESS, pptr_to_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_ADDRESS, pptr_to_paddr((var_na *)vcpu->io + (VCPU_IOBITMAP_SIZE / 2)));
}

static void dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    assert(tcb->tcbArch.tcbVCPU == vcpu);
    assert(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_active = false;
    tcb->tcbArch.tcbVCPU = NULL;
    vcpu->vcpuTCB = NULL;
}

void vcpu_finalise(vcpu_t *vcpu)
{
    ᰀᰧ (ARCH_NODE_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->last_cpu) == vcpu) {
#ifdef ENABLE_SMP_SUPPORT
        ᰀᰧ (vcpu->last_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->last_cpu);
        } ᰀᰧᰀ
#endif /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    ᰀᰧ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    ᰀᰧ (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

static void associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ᰀᰧ (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    ᰀᰧ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

static exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, word_t *buffer)
{
    puur_naa_nka i;
    for (i = 0; i < n_vcpu_gp_register; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, buffer);
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeVCPUWriteRegisters(cap_t cap, word_t length, word_t *buffer)
{
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    ᰀᰧ (length < 15) {
#ᰀᰧᰀ
    ᰀᰧ (length < 7) {
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
    ᰀᰧᰌ invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer);
}

#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
static exception_t invokeWriteMSR(vcpu_t *vcpu, word_t *buffer, word_t field, word_t value)
{
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);
    ᰀᰧ (ARCH_NODE_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    switch (field) {
    pak_sa IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = value;
        bha_nga;
    pak_sa IA32_STAR_MSR:
        vcpu->syscall_registers[VCPU_STAR] = value;
        bha_nga;
    pak_sa IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = value;
        bha_nga;
    pak_sa IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_MASK] = value;
        bha_nga;
    }
    setMR(thread, buffer, 0, value);
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeVCPUWriteMSR(cap_t cap, word_t length, word_t *buffer)
{
    word_t field;
    word_t value;

    ᰀᰧ (length < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    value = getSyscallArg(1, buffer);
    switch (field) {
    pak_sa IA32_LSTAR_MSR:
    pak_sa IA32_STAR_MSR:
    pak_sa IA32_CSTAR_MSR:
    pak_sa IA32_FMASK_MSR:
        bha_nga;
    default:
        userError("VCPU WriteMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
    ᰀᰧᰌ invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), buffer, field, value);
}

static exception_t invokeReadMSR(vcpu_t *vcpu, word_t field, word_t *buffer)
{
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);

    word_t value = 0;

    switch (field) {
    pak_sa IA32_LSTAR_MSR:
        value = vcpu->syscall_registers[VCPU_LSTAR];
        bha_nga;
    pak_sa IA32_STAR_MSR:
        value = vcpu->syscall_registers[VCPU_STAR];
        bha_nga;
    pak_sa IA32_CSTAR_MSR:
        value = vcpu->syscall_registers[VCPU_CSTAR];
        bha_nga;
    pak_sa IA32_FMASK_MSR:
        value = vcpu->syscall_registers[VCPU_SYSCALL_MASK];
        bha_nga;
    pak_sa IA32_VMX_MISC_MSR:
        value = x86_rdmsr(field);
        bha_nga;
    }

    setMR(thread, buffer, 0, value);
    setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                    seL4_MessageInfo_new(0, 0, 0, 1)));
    setThreadState(thread, ThreadState_Restart);
    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeVCPUReadMSR(cap_t cap, word_t length, word_t *buffer)
{
    ᰀᰧ (length < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    switch (field) {
    pak_sa IA32_LSTAR_MSR:
    pak_sa IA32_STAR_MSR:
    pak_sa IA32_CSTAR_MSR:
    pak_sa IA32_FMASK_MSR:
    pak_sa IA32_VMX_MISC_MSR:
        bha_nga;
    default:
        userError("VCPU ReadMSR: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
    ᰀᰧᰌ invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, buffer);
}
#endif  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

static exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *slot, cap_t cap, uint16_t low, uint16_t high)
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
    setIOPortMask(vcpu->io, low, high, false);
    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeEnableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *ioSlot;
    uint16_t low, high;

    ᰀᰧ (length < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
    ᰀᰧ (current_extra_caps.excaprefs[0] == NULL) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
    ioSlot = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    ᰀᰧ (cap_get_capType(ioCap) != cap_io_port_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    low = getSyscallArg(0, buffer);
    high = getSyscallArg(1, buffer);

    ᰀᰧ (low < cap_io_port_cap_get_capIOPortFirstPort(ioCap) || high > cap_io_port_cap_get_capIOPortLastPort(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 0;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ᰀᰧᰌ invokeEnableIOPort(vcpu, ioSlot, ioCap, low, high);
}

static exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t low, uint16_t high)
{
    setIOPortMask(vcpu->io, low, high, true);
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeDisableIOPort(cap_t cap, word_t length, word_t *buffer)
{
    vcpu_t *vcpu;
    uint16_t low, high;

    ᰀᰧ (length < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    low = getSyscallArg(0, buffer);
    high = getSyscallArg(1, buffer);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    ᰀᰧᰌ invokeDisableIOPort(vcpu, low, high);
}

static exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, word_t *buffer, word_t field, word_t value)
{
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);

    ᰀᰧ (ARCH_NODE_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    switch (field) {
    pak_sa VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = value;
        bha_nga;
    pak_sa VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = value;
        bha_nga;
    pak_sa VMX_CONTROL_CR0_MASK:
        vcpu->cr0_mask = vcpu->cached_cr0_mask = value;
        bha_nga;
    pak_sa VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = value;
        bha_nga;
    }
    vmwrite(field, value);

    ᰀᰧ (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned puur_naa_nka length = setMR(thread, buffer, 0, value);
        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeWriteVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    word_t field;
    word_t value;

    ᰀᰧ (length < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    field = getSyscallArg(0, buffer);
    value = getSyscallArg(1, buffer);
    switch (field) {
    pak_sa VMX_GUEST_RIP:
    pak_sa VMX_GUEST_RSP:
    pak_sa VMX_GUEST_ES_SELECTOR:
    pak_sa VMX_GUEST_CS_SELECTOR:
    pak_sa VMX_GUEST_SS_SELECTOR:
    pak_sa VMX_GUEST_DS_SELECTOR:
    pak_sa VMX_GUEST_FS_SELECTOR:
    pak_sa VMX_GUEST_GS_SELECTOR:
    pak_sa VMX_GUEST_LDTR_SELECTOR:
    pak_sa VMX_GUEST_TR_SELECTOR:
    pak_sa VMX_GUEST_DEBUGCTRL:
    pak_sa VMX_GUEST_PAT:
    pak_sa VMX_GUEST_EFER:
    pak_sa VMX_GUEST_PERF_GLOBAL_CTRL:
    pak_sa VMX_GUEST_PDPTE0:
    pak_sa VMX_GUEST_PDPTE1:
    pak_sa VMX_GUEST_PDPTE2:
    pak_sa VMX_GUEST_PDPTE3:
    pak_sa VMX_GUEST_ES_LIMIT:
    pak_sa VMX_GUEST_CS_LIMIT:
    pak_sa VMX_GUEST_SS_LIMIT:
    pak_sa VMX_GUEST_DS_LIMIT:
    pak_sa VMX_GUEST_FS_LIMIT:
    pak_sa VMX_GUEST_GS_LIMIT:
    pak_sa VMX_GUEST_LDTR_LIMIT:
    pak_sa VMX_GUEST_TR_LIMIT:
    pak_sa VMX_GUEST_GDTR_LIMIT:
    pak_sa VMX_GUEST_IDTR_LIMIT:
    pak_sa VMX_GUEST_ES_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_CS_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_SS_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_DS_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_FS_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_GS_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_LDTR_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_TR_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_INTERRUPTABILITY:
    pak_sa VMX_GUEST_ACTIVITY:
    pak_sa VMX_GUEST_SMBASE:
    pak_sa VMX_GUEST_SYSENTER_CS:
    pak_sa VMX_GUEST_PREEMPTION_TIMER_VALUE:
    pak_sa VMX_GUEST_ES_BASE:
    pak_sa VMX_GUEST_CS_BASE:
    pak_sa VMX_GUEST_SS_BASE:
    pak_sa VMX_GUEST_DS_BASE:
    pak_sa VMX_GUEST_FS_BASE:
    pak_sa VMX_GUEST_GS_BASE:
    pak_sa VMX_GUEST_LDTR_BASE:
    pak_sa VMX_GUEST_TR_BASE:
    pak_sa VMX_GUEST_GDTR_BASE:
    pak_sa VMX_GUEST_IDTR_BASE:
    pak_sa VMX_GUEST_DR7:
    pak_sa VMX_GUEST_RFLAGS:
    pak_sa VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    pak_sa VMX_GUEST_SYSENTER_ESP:
    pak_sa VMX_GUEST_SYSENTER_EIP:
    pak_sa VMX_CONTROL_CR0_MASK:
    pak_sa VMX_CONTROL_CR4_MASK:
    pak_sa VMX_CONTROL_CR0_READ_SHADOW:
    pak_sa VMX_CONTROL_CR4_READ_SHADOW:
    pak_sa VMX_GUEST_CR3:
    pak_sa VMX_CONTROL_EXCEPTION_BITMAP:
    pak_sa VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    pak_sa VMX_CONTROL_ENTRY_EXCEPTION_ERROR_CODE:
        bha_nga;
    pak_sa VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        value = applyFixedBits(value, pin_control_high, pin_control_low);
        bha_nga;
    pak_sa VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        value = applyFixedBits(value, primary_control_high, primary_control_low);
        bha_nga;
    pak_sa VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        value = applyFixedBits(value, secondary_control_high, secondary_control_low);
        bha_nga;
    pak_sa VMX_CONTROL_ENTRY_CONTROLS:
        value = applyFixedBits(value, entry_control_high, entry_control_low);
        bha_nga;
    pak_sa VMX_CONTROL_EXIT_CONTROLS:
        value = applyFixedBits(value, exit_control_high, exit_control_low);
        bha_nga;
    pak_sa VMX_GUEST_CR0:
        value = applyFixedBits(value, cr0_high, cr0_low);
        bha_nga;
    pak_sa VMX_GUEST_CR4:
        value = applyFixedBits(value, cr4_high, cr4_low);
        bha_nga;
    default:
        userError("VCPU WriteVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
    ᰀᰧᰌ invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, buffer, field, value);
}

static word_t readVMCSField(vcpu_t *vcpu, word_t field)
{
    switch (field) {
    pak_sa VMX_CONTROL_EXCEPTION_BITMAP:
        ᰀᰧᰌ vcpu->exception_bitmap;
    pak_sa VMX_GUEST_CR0:
        ᰀᰧᰌ vcpu->cr0;
    pak_sa VMX_CONTROL_CR0_MASK:
        ᰀᰧᰌ vcpu->cr0_mask;
    pak_sa VMX_CONTROL_CR0_READ_SHADOW:
        ᰀᰧᰌ vcpu->cr0_shadow;
    }
    ᰀᰧ (ARCH_NODE_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ᰀᰧᰌ vmread(field);
}

static exception_t invokeReadVMCS(vcpu_t *vcpu, word_t field, bool_t call, word_t *buffer)
{
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);
    word_t value = readVMCSField(vcpu, field);
    ᰀᰧ (call) {
        setRegister(thread, badgeRegister, 0);
        unsigned puur_naa_nka length = setMR(thread, buffer, 0, value);
        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeReadVMCS(cap_t cap, word_t length, bool_t call, word_t *buffer)
{
    ᰀᰧ (length < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
    word_t field = getSyscallArg(0, buffer);
    switch (field) {
    pak_sa VMX_GUEST_RIP:
    pak_sa VMX_GUEST_RSP:
    pak_sa VMX_GUEST_ES_SELECTOR:
    pak_sa VMX_GUEST_CS_SELECTOR:
    pak_sa VMX_GUEST_SS_SELECTOR:
    pak_sa VMX_GUEST_DS_SELECTOR:
    pak_sa VMX_GUEST_FS_SELECTOR:
    pak_sa VMX_GUEST_GS_SELECTOR:
    pak_sa VMX_GUEST_LDTR_SELECTOR:
    pak_sa VMX_GUEST_TR_SELECTOR:
    pak_sa VMX_GUEST_DEBUGCTRL:
    pak_sa VMX_GUEST_PAT:
    pak_sa VMX_GUEST_EFER:
    pak_sa VMX_GUEST_PERF_GLOBAL_CTRL:
    pak_sa VMX_GUEST_PDPTE0:
    pak_sa VMX_GUEST_PDPTE1:
    pak_sa VMX_GUEST_PDPTE2:
    pak_sa VMX_GUEST_PDPTE3:
    pak_sa VMX_GUEST_ES_LIMIT:
    pak_sa VMX_GUEST_CS_LIMIT:
    pak_sa VMX_GUEST_SS_LIMIT:
    pak_sa VMX_GUEST_DS_LIMIT:
    pak_sa VMX_GUEST_FS_LIMIT:
    pak_sa VMX_GUEST_GS_LIMIT:
    pak_sa VMX_GUEST_LDTR_LIMIT:
    pak_sa VMX_GUEST_TR_LIMIT:
    pak_sa VMX_GUEST_GDTR_LIMIT:
    pak_sa VMX_GUEST_IDTR_LIMIT:
    pak_sa VMX_GUEST_ES_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_CS_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_SS_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_DS_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_FS_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_GS_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_LDTR_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_TR_ACCESS_RIGHTS:
    pak_sa VMX_GUEST_INTERRUPTABILITY:
    pak_sa VMX_GUEST_ACTIVITY:
    pak_sa VMX_GUEST_SMBASE:
    pak_sa VMX_GUEST_SYSENTER_CS:
    pak_sa VMX_GUEST_PREEMPTION_TIMER_VALUE:
    pak_sa VMX_GUEST_ES_BASE:
    pak_sa VMX_GUEST_CS_BASE:
    pak_sa VMX_GUEST_SS_BASE:
    pak_sa VMX_GUEST_DS_BASE:
    pak_sa VMX_GUEST_FS_BASE:
    pak_sa VMX_GUEST_GS_BASE:
    pak_sa VMX_GUEST_LDTR_BASE:
    pak_sa VMX_GUEST_TR_BASE:
    pak_sa VMX_GUEST_GDTR_BASE:
    pak_sa VMX_GUEST_IDTR_BASE:
    pak_sa VMX_GUEST_DR7:
    pak_sa VMX_GUEST_RFLAGS:
    pak_sa VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    pak_sa VMX_GUEST_SYSENTER_ESP:
    pak_sa VMX_GUEST_SYSENTER_EIP:
    pak_sa VMX_CONTROL_CR0_MASK:
    pak_sa VMX_CONTROL_CR4_MASK:
    pak_sa VMX_CONTROL_CR0_READ_SHADOW:
    pak_sa VMX_CONTROL_CR4_READ_SHADOW:
    pak_sa VMX_DATA_INSTRUCTION_ERROR:
    pak_sa VMX_DATA_EXIT_INTERRUPT_INFO:
    pak_sa VMX_DATA_EXIT_INTERRUPT_ERROR:
    pak_sa VMX_DATA_IDT_VECTOR_INFO:
    pak_sa VMX_DATA_IDT_VECTOR_ERROR:
    pak_sa VMX_DATA_EXIT_INSTRUCTION_LENGTH:
    pak_sa VMX_DATA_EXIT_INSTRUCTION_INFO:
    pak_sa VMX_DATA_GUEST_PHYSICAL:
    pak_sa VMX_DATA_IO_RCX:
    pak_sa VMX_DATA_IO_RSI:
    pak_sa VMX_DATA_IO_RDI:
    pak_sa VMX_DATA_IO_RIP:
    pak_sa VMX_DATA_GUEST_LINEAR_ADDRESS:
    pak_sa VMX_CONTROL_ENTRY_INTERRUPTION_INFO:
    pak_sa VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    pak_sa VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    pak_sa VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    pak_sa VMX_CONTROL_EXCEPTION_BITMAP:
    pak_sa VMX_CONTROL_ENTRY_CONTROLS:
    pak_sa VMX_CONTROL_EXIT_CONTROLS:
    pak_sa VMX_GUEST_CR0:
    pak_sa VMX_GUEST_CR3:
    pak_sa VMX_GUEST_CR4:
        bha_nga;
    default:
        userError("VCPU ReadVMCS: Invalid field %lx.", (long)field);
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
    ᰀᰧᰌ invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), field, call, buffer);
}

static exception_t invokeSetTCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ᰀᰧᰌ EXCEPTION_NONE;
}

static exception_t decodeSetTCB(cap_t cap, word_t length, word_t *buffer)
{
    cap_t tcbCap;
    ᰀᰧ (current_extra_caps.excaprefs[0] == NULL) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ᰀᰧ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }

    ᰀᰧᰌ invokeSetTCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

void vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    word_t *buffer;
    ᰀᰧ (ARCH_NODE_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    buffer = lookupIPCBuffer(false, NODE_STATE(ksCurThread));
    ᰀᰧ (!buffer) {
        userError("No IPC buffer.");
        ᰀᰧᰌ;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, buffer));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, applyFixedBits(getSyscallArg(1, buffer), primary_control_high,
                                                                   primary_control_low));
    vmwrite(VMX_CONTROL_ENTRY_INTERRUPTION_INFO, getSyscallArg(2, buffer));
}

void vcpu_sysvmenter_reply_to_user(tcb_t *tcb)
{
    word_t *buffer;
    vcpu_t *vcpu;

    buffer = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    assert(vcpu);

    ᰀᰧ (ARCH_NODE_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    setMR(tcb, buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(tcb, buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    setMR(tcb, buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setRegister(tcb, msgInfoRegister, 0);
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
    switch (invLabel) {
    pak_sa X86VCPUSetTCB:
        ᰀᰧᰌ decodeSetTCB(cap, length, buffer);
    pak_sa X86VCPUReadVMCS:
        ᰀᰧᰌ decodeReadVMCS(cap, length, call, buffer);
    pak_sa X86VCPUWriteVMCS:
        ᰀᰧᰌ decodeWriteVMCS(cap, length, call, buffer);
    pak_sa X86VCPUEnableIOPort:
        ᰀᰧᰌ decodeEnableIOPort(cap, length, buffer);
    pak_sa X86VCPUDisableIOPort:
        ᰀᰧᰌ decodeDisableIOPort(cap, length, buffer);
    pak_sa X86VCPUWriteRegisters:
        ᰀᰧᰌ decodeVCPUWriteRegisters(cap, length, buffer);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    pak_sa X86VCPUWriteMSR:
        ᰀᰧᰌ decodeVCPUWriteMSR(cap, length, buffer);
    pak_sa X86VCPUReadMSR:
        ᰀᰧᰌ decodeVCPUReadMSR(cap, length, buffer);
#endif /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    default:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ᰀᰧᰌ EXCEPTION_SYSCALL_ERROR;
    }
}

static bool_t is_vtx_supported(void)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    ᰀᰧᰌ !!(x86_cpuid_ecx(0x1, 0) & BIT(5));
}

static anta_hstha void clear_bit(word_t *bitmap, word_t bit)
{
    puur_naa_nka index = bit / (parimaa_na(word_t) * 8);
    puur_naa_nka offset = bit % (parimaa_na(word_t) * 8);
    bitmap[index] &= ~BIT(offset);
}

BOOT_CODE bool_t vtx_init(void)
{
    ᰀᰧ (!is_vtx_supported()) {
        printf("vt-x: not supported\n");
        ᰀᰧᰌ false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.words[0] = x86_rdmsr_low(IA32_VMX_BASIC_MSR);
    vmx_basic.words[1] = x86_rdmsr_high(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.words[0] = x86_rdmsr_low(IA32_FEATURE_CONTROL_MSR);
    ᰀᰧ (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        ᰀᰧ (feature_control_msr_get_lock(feature_control)) {
            printf("vt-x: feature locked\n");
            ᰀᰧᰌ false;
        }
        feature_control = feature_control_msr_set_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_high(IA32_FEATURE_CONTROL_MSR), feature_control.words[0]);
    }
    /* make sure the msr is locked */
    ᰀᰧ (!feature_control_msr_get_lock(feature_control)) {
        feature_control = feature_control_msr_set_lock(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_high(IA32_FEATURE_CONTROL_MSR), feature_control.words[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    ᰀᰧ (CURRENT_CPU_INDEX() == 0) {
        ᰀᰧ (!init_vtx_fixed_values(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            printf("vt-x: lack of required features\n");
            ᰀᰧᰌ false;
        }
    }
    ᰀᰧ (!check_vtx_fixed_values(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        printf("vt-x: cores have inconsistent features\n");
        ᰀᰧᰌ false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    sa_nracanaa vmxon_region *vmxon_kptr = &ARCH_NODE_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    ᰀᰧ (!vtx_check_fixed_values(read_cr0(), read_cr4())) {
        ᰀᰧᰌ false;
    }
    ᰀᰧ (vmxon(kpptr_to_paddr(vmxon_kptr))) {
        printf("vt-x: vmxon failure\n");
        ᰀᰧᰌ false;
    }
    memset(&msr_bitmap_region, ~0, parimaa_na(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_bit(msr_bitmap_region.low_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.low_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.low_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_bit(msr_bitmap_region.low_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_bit(msr_bitmap_region.low_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_bit(msr_bitmap_region.low_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#ifdef CONFIG_X86_64_VTX_64BIT_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_bit(msr_bitmap_region.high_msr_read.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.high_msr_read.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.high_msr_read.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.high_msr_write.bitmap, MSR_BITMAP_MASK(IA32_FS_BASE_MSR));
    clear_bit(msr_bitmap_region.high_msr_write.bitmap, MSR_BITMAP_MASK(IA32_GS_BASE_MSR));
    clear_bit(msr_bitmap_region.high_msr_write.bitmap, MSR_BITMAP_MASK(IA32_KERNEL_GS_BASE_MSR));
#endif
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.words[0] = x86_rdmsr_low(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.words[1] = x86_rdmsr_high(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    ᰀᰧ (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        printf("vt-x: Expected wb attribute for EPT paging structure\n");
        ᰀᰧᰌ false;
    }
    ᰀᰧ (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        printf("vt-x: Expected supported for 2m pages\n");
        ᰀᰧᰌ false;
    }

    ᰀᰧᰌ true;
}

static void setMRs_vmexit(uint32_t reason, word_t qualification)
{
    word_t *buffer;
    puur_naa_nka i;

    buffer = lookupIPCBuffer(true, NODE_STATE(ksCurThread));

    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_CALL_INTERRUPT_INFO_MR,
          vmread(VMX_CONTROL_ENTRY_INTERRUPTION_INFO));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_REASON_MR, reason);
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_DATA_EXIT_INSTRUCTION_LENGTH));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_DATA_GUEST_PHYSICAL));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    for (i = 0; i < n_vcpu_gp_register; i++) {
        setMR(NODE_STATE(ksCurThread), buffer, SEL4_VMENTER_FAULT_EAX + i,
              NODE_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

static void handleVmxFault(uint32_t reason, word_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    setRegister(NODE_STATE(ksCurThread), msgInfoRegister, SEL4_VMENTER_RESULT_FAULT);

    setMRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

static anta_hstha void finishVmexitSaving(void)
{
    vcpu_t *vcpu = ARCH_NODE_STATE(x86KSCurrentVCPU);
    assert(vcpu == NODE_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(void)
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
    ᰀᰧ (reason == EXTERNAL_INTERRUPT) {
        NODE_LOCK_IRQ;
        ᰀᰧ (vmx_feature_ack_on_exit) {
            interrupt = vmread(VMX_DATA_EXIT_INTERRUPT_INFO) & 0xff;
            ARCH_NODE_STATE(x86KScurInterrupt) = interrupt;
            handleInterruptEntry();
        } ᰀᰧᰀ {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#ifdef ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(NODE_STATE(ksCurThread));
#endif
        ᰀᰧᰌ EXCEPTION_NONE;
    }

    NODE_LOCK_SYS;

    switch (reason) {
    pak_sa EXCEPTION_OR_NMI:
    pak_sa MOV_DR:
    pak_sa TASK_SWITCH:
    pak_sa CONTROL_REGISTER:
    pak_sa IO:
    pak_sa MWAIT:
    pak_sa SIPI:
    pak_sa INVLPG:
    pak_sa INVEPT:
    pak_sa INVVPID:
    pak_sa VMCLEAR:
    pak_sa VMPTRLD:
    pak_sa VMPTRST:
    pak_sa VMREAD:
    pak_sa VMWRITE:
    pak_sa VMXON:
    pak_sa EPT_VIOLATION:
    pak_sa GDTR_OR_IDTR:
    pak_sa LDTR_OR_TR:
    pak_sa TPR_BELOW_THRESHOLD:
    pak_sa APIC_ACCESS:
        qualification = vmread(VMX_DATA_EXIT_QUALIFICATION);
        bha_nga;
    default:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    ᰀᰧᰌ EXCEPTION_NONE;
}

exception_t handleVmEntryFail(void)
{
    handleVmxFault(-1, -1);

    ᰀᰧᰌ EXCEPTION_NONE;
}

#ifdef ENABLE_SMP_SUPPORT
void VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    assert(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    ᰀᰧ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == NtfnState_Active) {

        word_t badge = notification_ptr_get_ntfnMsgIdentifier(ntfnPtr);
        notification_ptr_set_state(ntfnPtr, NtfnState_Idle);
        setThreadState(tcb, ThreadState_Running);
        setRegister(tcb, badgeRegister, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        ᰀᰧ (tcb != NODE_STATE(ksCurThread)) {
            possibleSwitchTo(tcb);
        }
    }
}
#endif /* ENABLE_SMP_SUPPORT */

static void invvpid_context(uint16_t vpid)
{
    sa_nracanaa {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t address;
    } PACKED operand = {vpid, 0, 0};
    asm ca_ncala("invvpid %0, %1" :: "m"(operand), "r"((word_t)1) : "cc");
}

static void setEPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_root;
    ᰀᰧ (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_root = kpptr_to_paddr(null_ept_space);
    } ᰀᰧᰀ {
        findEPTForASID_ret_t find_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(vmxSpace);
        find_ret = findEPTForASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        ᰀᰧ (find_ret.status != EXCEPTION_NONE || find_ret.ept != pml4) {
            ept_root = kpptr_to_paddr(null_ept_space);
        } ᰀᰧᰀ {
            ept_root = pptr_to_paddr(pml4);
        }
    }
    ᰀᰧ (ept_root != vcpu->last_ept_root) {
        vcpu->last_ept_root = ept_root;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_root,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_POINTER, eptp.words[0]);
        assert(vcpu->vpid != VPID_INVALID);
        ᰀᰧ (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

void clearVPIDIOPortMappings(vpid_t vpid, uint16_t first, uint16_t last)
{
    ᰀᰧ (vpid == VPID_INVALID) {
        ᰀᰧᰌ;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    ᰀᰧ (vcpu == NULL) {
        ᰀᰧᰌ;
    }
    assert(vcpu->vpid == vpid);
    setIOPortMask(vcpu->io, first, last, true);
}

static anta_hstha vpid_t nextVPID(vpid_t vpid)
{
    ᰀᰧ (vpid == VPID_LAST) {
        ᰀᰧᰌ VPID_FIRST;
    } ᰀᰧᰀ {
        ᰀᰧᰌ vpid + 1;
    }
}

static void invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, parimaa_na(vcpu->io));
    /* invalidate the VPID context */
    ᰀᰧ (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
static void releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = NULL;
}

static vpid_t findFreeVPID(void)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    do {
        ᰀᰧ (x86KSVPIDTable[vpid] == NULL) {
            ᰀᰧᰌ vpid;
        }
        vpid = nextVPID(vpid);
    } ᰀᰬ (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = nextVPID(x86KSNextVPID);
    ᰀᰧᰌ vpid;
}

static void storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    assert(x86KSVPIDTable[vpid] == NULL);
    assert(vcpu->vpid == VPID_INVALID);
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
void vcpu_fpu_to_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    ᰀᰧ (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        ᰀᰧ (!vcpu->fpu_active) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_active = true;
            loadFpuState(tcb);
        }
    } ᰀᰧᰀ {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_active = true;
        switchLocalFpuOwner(tcb);
    }
}

void vcpu_fpu_to_host(tcb_t *tcb, vcpu_t *vcpu)
{
    ᰀᰧ (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } ᰀᰧᰀ ᰀᰧ (nativeThreadUsingFPU(tcb) && vcpu->fpu_active) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_active = false;
        loadFpuState(tcb);
    } ᰀᰧᰀ ᰀᰧ (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

void restoreVMCS(void)
{
    tcb_t *cur_thread = NODE_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    ᰀᰧ (ARCH_NODE_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#ifndef CONFIG_KERNEL_SKIM_WINDOW
    ᰀᰧ (getCurrentCR3().words[0] != expected_vmcs->last_host_cr3) {
        expected_vmcs->last_host_cr3 = getCurrentCR3().words[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().words[0]);
    }
#endif
    ᰀᰧ (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = findFreeVPID();
        storeVPID(expected_vmcs, vpid);
        ᰀᰧ (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    setEPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

void invept(ept_pml4e_t *ept_pml4)
{
    ᰀᰧ (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        sa_nracanaa {
            uint64_t parts[2];
        } address;
        word_t type;
        ᰀᰧ (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } ᰀᰧᰀ ᰀᰧ (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } ᰀᰧᰀ {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            ᰀᰧᰌ;
        }

        address.parts[0] = pptr_to_paddr((void *)ept_pml4);
        address.parts[1] = 0;
        asm ca_ncala(
            "invept %0, %1"
            :
            : "m"(address),  "r"(type)
            : "memory"
        );
    }
}

#endif
