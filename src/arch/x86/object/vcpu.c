/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_VTX

#అన్తర్భావ <types.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <object/objecttype.h>
#అన్తర్భావ <arch/machine/cpu_registers.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/object/vcpu.h>
#అన్తర్భావ <arch/object/ioport.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <sel4/arch/vmenter.h>

#నిర్వచించు VMX_EXIT_QUAL_TYPE_MOV_CR 0
#నిర్వచించు VMX_EXIT_QUAL_TYPE_CLTS 2
#నిర్వచించు VMX_EXIT_QUAL_TYPE_LMSW 3

స్థిరరాశి vcpu_gp_రిజిస్టరు_t crExitRegs[] = {
    VCPU_EAX, VCPU_ECX, VCPU_EDX, VCPU_EBX, VCPU_ESP, VCPU_EBP, VCPU_ESI, VCPU_EDI,
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
    VCPU_R8, VCPU_R9, VCPU_R10, VCPU_R11, VCPU_R12, VCPU_R13, VCPU_R14, VCPU_R15,
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */
};

#నిర్వచించు MSR_BITMAP_ముసుగు(x) ((x) & 0x1fff)

రకనిర్వచనం నిర్మాణం msr_bitmap {
    మాట_t bitmap[0x2000 / పరిమాణం(మాట_t) / 8];
} msr_bitmap_t;

రకనిర్వచనం నిర్మాణం msr_bitmaps {
    msr_bitmap_t దిగువ_msr_read;
    msr_bitmap_t ఎగువ_msr_read;
    msr_bitmap_t దిగువ_msr_write;
    msr_bitmap_t ఎగువ_msr_write;
} msr_bitmaps_t;

స్థిర msr_bitmaps_t msr_bitmap_region ALIGN(బిట్టు(se_L4_పుట_బిట్లు));

స్థిర అక్షరం null_ept_space[బిట్టు(se_L4_పుట_బిట్లు)] ALIGN(బిట్టు(se_L4_పుట_బిట్లు));

/* Cached value of the hardware defined vmcs revision */
స్థిర uint32_t vmcs_revision;

/* Cached value of the VPID capability MSR */
స్థిర vmx_ept_vpid_cap_msr_t vpid_capability;

/* Cache the values that we calculated for bits that need to be set high
 * and low in various vmcs fields */
స్థిర uint32_t pin_control_ఎగువ;
స్థిర uint32_t pin_control_దిగువ;
స్థిర uint32_t primary_control_ఎగువ;
స్థిర uint32_t primary_control_దిగువ;
స్థిర uint32_t secondary_control_ఎగువ;
స్థిర uint32_t secondary_control_దిగువ;
స్థిర uint32_t నమోదు_control_ఎగువ;
స్థిర uint32_t నమోదు_control_దిగువ;
స్థిర uint32_t exit_control_ఎగువ;
స్థిర uint32_t exit_control_దిగువ;
స్థిర uint32_t cr0_ఎగువ;
స్థిర uint32_t cr0_దిగువ;
స్థిర uint32_t cr4_ఎగువ;
స్థిర uint32_t cr4_దిగువ;

/* these flags indicate the presence of specific VT-x features. These features
 * are checked for at boot time and are constant from then on */
స్థిర bool_t vmx_feature_vpid;
స్థిర bool_t vmx_feature_ఎక్కించు_perf_global_ctrl;
స్థిర bool_t vmx_feature_ack_on_exit;

స్థిర vcpu_t *x86KSVPIDTable[VPID_చివరి + 1];
స్థిర vpid_t x86KSNextVPID = VPID_మొదటి;

స్థిర శూన్య releaseVPID(vpid_t vpid);

స్థిర అంతఃవరుస bool_t vmxon(paddr_t vmxon_region)
{
    uint8_t error;
    /* vmxon requires a 64bit memory address, so perform a
     * cast here to guarantee this on 32-bit platforms */
    uint64_t vmxonreg = vmxon_region;
    asm అస్థిర(
        "vmxon %1; setnae %0"
        : "=q"(error)
        : "m"(vmxonreg)
        : "memory", "cc"
    );
    తిరిగి !!error;
}

స్థిర శూన్య vmclear(శూన్య *vmcs_ptr)
{
    uint64_t physical_చిరునామా;
    physical_చిరునామా = pptr_గమ్యం_paddr((శూన్య *)vmcs_ptr);
    asm అస్థిర(
        "vmclear %0"
        :
        : "m"(physical_చిరునామా)
        : "cc"
    );
}

శూన్య clearCurrentVCPU(శూన్య)
{
    vcpu_t *vcpu = ARCH_కణుపు_STATE(x86KSCurrentVCPU);
    ఒకవేళ (vcpu) {
        vmclear(vcpu);
        vcpu->launched = false;
        ARCH_కణుపు_STATE(x86KSCurrentVCPU) = ఖాళీ;
        ఒకవేళ (vcpu->fpu_చురుకైన && vcpu->vcpuTCB) {
            fpuRelease(vcpu->vcpuTCB);
            vcpu->fpu_చురుకైన = false;
        }
    }
}

స్థిర శూన్య vmptrld(శూన్య *vmcs_ptr)
{
    uint64_t physical_చిరునామా;
    uint8_t error;
    physical_చిరునామా = pptr_గమ్యం_paddr(vmcs_ptr);
    asm అస్థిర(
        "vmptrld %1; setna %0"
        : "=q"(error)
        : "m"(physical_చిరునామా)
        : "cc"
    );
    /* The usage of vmptrld should be correct by construction. As there is no
     * capacity to propagate errors where vmptrld is used we will do our best
     * to detect bugs in debug builds by asserting */
    ప్రతిజ్ఞా(!error);
}

#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
శూన్య vcpu_restore_guest_msrs(vcpu_t *vcpu)
{
    x86_wrmsr(IA32_నక్షత్రం_MSR, vcpu->syscall_registers[VCPU_నక్షత్రం]);
    x86_wrmsr(IA32_LSTAR_MSR, vcpu->syscall_registers[VCPU_LSTAR]);
    x86_wrmsr(IA32_CSTAR_MSR, vcpu->syscall_registers[VCPU_CSTAR]);
    x86_wrmsr(IA32_FMASK_MSR, vcpu->syscall_registers[VCPU_SYSCALL_ముసుగు]);
}

శూన్య vcpu_restore_host_msrs(శూన్య)
{
    init_syscall_msrs();
}
#అయితే_ముగింపు

స్థిర శూన్య switchVCPU(vcpu_t *vcpu)
{
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    ఒకవేళ (vcpu->చివరి_cpu != getCurrentCPUIndex() && ARCH_కణుపు_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->చివరి_cpu) == vcpu) {
        /* vcpu is currently loaded on another core, need to do vmclear on that core */
        doRemoteClearCurrentVCPU(vcpu->చివరి_cpu);
    }
#అయితే_ముగింపు
    clearCurrentVCPU();
    vmptrld(vcpu);
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    ఒకవేళ (vcpu->చివరి_cpu != getCurrentCPUIndex()) {
        /* migrate host state */
        vmwrite(VMX_HOST_TR_పునాది, (మాట_t)&x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KStss);
        vmwrite(VMX_HOST_GDTR_పునాది, (మాట_t)x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KSgdt);
        vmwrite(VMX_HOST_IDTR_పునాది, (మాట_t)x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KSidt);
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(మాట_t)((అక్షరం *)&x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KStss.tss.పదాలు[0] +
                                                          4));
    }
    vcpu->చివరి_cpu = getCurrentCPUIndex();
#అయితే_ముగింపు
    ARCH_కణుపు_STATE(x86KSCurrentVCPU) = vcpu;
}

స్థిర శూన్య print_బిట్లు(మాట_t బిట్లు)
{
    bool_t మొదటి = true;
    అయితే (బిట్లు) {
        పూర్ణాంకం సూచీ = se_L4_మాట_బిట్లు - 1 - clzl(బిట్లు);
        ఒకవేళ (మొదటి) {
            రాయి("%d", సూచీ);
            మొదటి = false;
        } లేకపోతే {
            రాయి(",%d", సూచీ);
        }
        బిట్లు &= ~బిట్టు(సూచీ);
    }
}

స్థిర bool_t check_fixed_విలువ(మాట_t val, మాట_t దిగువ, మాట_t ఎగువ)
{
    మాట_t not_ఎగువ;
    మాట_t not_దిగువ;
    /* check if any bits that should be high, are not
     * high & val represents the set of bits that are
     * correctly set to high. if this is equal to high
     * then everything is good, to detect exactly which
     * bits were not high we can invert and mask with
     * high again. Now if this is 0 everythins is fine,
     * and if not each bit set indicates a bit we had
     * failed to set */
    not_ఎగువ = ఎగువ & ~(ఎగువ & val);
    ఒకవేళ (not_ఎగువ != 0) {
        రాయి("Failed to set bits: ");
        print_బిట్లు(not_ఎగువ);
        తిరిగి false;
    }
    /* we can do the same thing for finding things
     * that should be low by first inverting */
    not_దిగువ = ~దిగువ & ~(~దిగువ & ~val);
    ఒకవేళ (not_దిగువ != 0) {
        రాయి("Incorrectly cleared bits: ");
        print_బిట్లు(not_దిగువ);
        తిరిగి false;
    }
    తిరిగి true;
}

స్థిర bool_t vtx_check_fixed_విలువలు(మాట_t cr0, మాట_t cr4)
{
    ఒకవేళ (!check_fixed_విలువ(cr0, cr0_దిగువ, cr0_ఎగువ)) {
        రాయి(" of CR0\n");
        తిరిగి false;
    }
    ఒకవేళ (!check_fixed_విలువ(cr4, cr4_దిగువ, cr4_ఎగువ)) {
        రాయి(" of CR4\n");
        తిరిగి false;
    }
    తిరిగి true;
}

స్థిర bool_t BOOT_సంకేతం init_vtx_fixed_విలువలు(bool_t useTrueMsrs)
{
    uint32_t pin_control_ముసుగు =
        బిట్టు(0) |    //Extern interrupt exiting
        బిట్టు(3) |    //NMI exiting
        బిట్టు(5);     //virtual NMIs
    uint32_t primary_control_ముసుగు =
        బిట్టు(25) |   //Use I/O bitmaps
        బిట్టు(28) |   //Use MSR bitmaps
        బిట్టు(31);    //Activate secondary controls
    uint32_t secondary_control_ముసుగు =
        బిట్టు(1);     //Enable EPT
    uint32_t exit_control_ముసుగు =
        బిట్టు(2)  |   //Save debug controls
        బిట్టు(18) |   //Save guest IA32_PAT on exit
        బిట్టు(19) |   //Load host IA32_PAT
        బిట్టు(20) |   //Save guest IA32_EFER on exit
        బిట్టు(21);    //Load host IA32_EFER
#నిర్వచితమైతే CONFIG_ARCH_X86_64
    exit_control_ముసుగు |= బిట్టు(9); //Host address-space size
#అయితే_ముగింపు /* CONFIG_ARCH_X86_64 */
    /* Read out the fixed high and low bits from the MSRs */
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t నమోదు_ctls;
    ఒకవేళ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        నమోదు_ctls = IA32_VMX_TRUE_నమోదు_CTLS_MSR;
    } లేకపోతే {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        నమోదు_ctls = IA32_VMX_నమోదు_CTLS_MSR;
    }
    pin_control_ఎగువ = x86_rdmsr_దిగువ(pinbased_ctls);
    pin_control_దిగువ = x86_rdmsr_ఎగువ(pinbased_ctls);
    primary_control_ఎగువ = x86_rdmsr_దిగువ(procbased_ctls);
    primary_control_దిగువ = x86_rdmsr_ఎగువ(procbased_ctls);
    secondary_control_ఎగువ = x86_rdmsr_దిగువ(IA32_VMX_PROCBASED_CTLS2_MSR);
    secondary_control_దిగువ = x86_rdmsr_ఎగువ(IA32_VMX_PROCBASED_CTLS2_MSR);
    exit_control_ఎగువ = x86_rdmsr_దిగువ(exit_ctls);
    exit_control_దిగువ = x86_rdmsr_ఎగువ(exit_ctls);
    నమోదు_control_ఎగువ = x86_rdmsr_దిగువ(నమోదు_ctls);
    నమోదు_control_దిగువ = x86_rdmsr_ఎగువ(నమోదు_ctls);

    cr0_ఎగువ = x86_rdmsr_దిగువ(IA32_VMX_CR0_FIXED0_MSR);
    cr0_దిగువ = x86_rdmsr_దిగువ(IA32_VMX_CR0_FIXED1_MSR);
    cr4_ఎగువ = x86_rdmsr_దిగువ(IA32_VMX_CR4_FIXED0_MSR);
    cr4_దిగువ = x86_rdmsr_దిగువ(IA32_VMX_CR4_FIXED1_MSR);

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
    ఒకవేళ (secondary_control_దిగువ & బిట్టు(7)) {
        cr0_ఎగువ &= ~(CR0_PE | CR0_PG);
    }

    /* Check for VPID support */
    ఒకవేళ (!(secondary_control_దిగువ & బిట్టు(5))) {
        vmx_feature_vpid = 0;
        రాయి("vt-x: VPIDs are not supported. Expect performance degradation\n");
    } లేకపోతే {
        vmx_feature_vpid = 1;
        secondary_control_ముసుగు |= బిట్టు(5);
    }

    /* Check for load perf global control */
    ఒకవేళ (!(exit_control_దిగువ & బిట్టు(12))) {
        vmx_feature_ఎక్కించు_perf_global_ctrl = 0;
        రాయి("vt-x: Load IA32_PERF_GLOBAL_CONTROL not supported. Hardware debugging may not work\n");
    } లేకపోతే {
        vmx_feature_ఎక్కించు_perf_global_ctrl = 1;
        exit_control_ముసుగు |= బిట్టు(12);
    }

    /* Check for external interrupt exiting */
    ఒకవేళ (!(exit_control_దిగువ & బిట్టు(15))) {
        vmx_feature_ack_on_exit = 0;
        రాయి("vt-x: Interrupt ack on exit not supported. Expect performance degradation\n");
    } లేకపోతే {
        vmx_feature_ack_on_exit = 1;
        exit_control_ముసుగు |= బిట్టు(15);
    }

    /* See if the hardware requires bits that require to be high to be low */
    uint32_t missing;
    missing = (~pin_control_దిగువ) & pin_control_ముసుగు;
    ఒకవేళ (missing) {
        రాయి("vt-x: Unsupported pin control features %lx\n", (దీర్ఘ)missing);
        తిరిగి false;
    }
    missing = (~primary_control_దిగువ) & primary_control_ముసుగు;
    ఒకవేళ (missing) {
        రాయి("vt-x: Unsupported primary control features %lx\n", (దీర్ఘ)missing);
        తిరిగి false;
    }
    missing = (~secondary_control_దిగువ) & secondary_control_ముసుగు;
    ఒకవేళ (missing) {
        రాయి("vt-x: Unsupported secondary control features %lx\n", (దీర్ఘ)missing);
        తిరిగి false;
    }
    missing = (~exit_control_దిగువ) & exit_control_ముసుగు;
    ఒకవేళ (missing) {
        రాయి("vt-x: Unsupported exit control features %lx\n", (దీర్ఘ)missing);
        తిరిగి false;
    }
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
    missing = (~నమోదు_control_దిగువ) & బిట్టు(9);
    ఒకవేళ (missing) {
        రాయి("vt-x: CPU does not support 64-bit guests\n");
        తిరిగి false;
    }
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */

    /* Force the bits we require to be high */
    pin_control_ఎగువ |= pin_control_ముసుగు;
    primary_control_ఎగువ |= primary_control_ముసుగు;
    secondary_control_ఎగువ |= secondary_control_ముసుగు;
    exit_control_ఎగువ |= exit_control_ముసుగు;

    తిరిగి true;
}

స్థిర bool_t BOOT_సంకేతం check_vtx_fixed_విలువలు(bool_t useTrueMsrs)
{
    uint32_t pinbased_ctls;
    uint32_t procbased_ctls;
    uint32_t exit_ctls;
    uint32_t నమోదు_ctls;
    ఒకవేళ (useTrueMsrs) {
        pinbased_ctls = IA32_VMX_TRUE_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_TRUE_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_TRUE_EXIT_CTLS_MSR;
        నమోదు_ctls = IA32_VMX_TRUE_నమోదు_CTLS_MSR;
    } లేకపోతే {
        pinbased_ctls = IA32_VMX_PINBASED_CTLS_MSR;
        procbased_ctls = IA32_VMX_PROCBASED_CTLS_MSR;
        exit_ctls = IA32_VMX_EXIT_CTLS_MSR;
        నమోదు_ctls = IA32_VMX_నమోదు_CTLS_MSR;
    }
    uint32_t local_pin_control_ఎగువ = x86_rdmsr_దిగువ(pinbased_ctls);
    uint32_t local_pin_control_దిగువ = x86_rdmsr_ఎగువ(pinbased_ctls);
    uint32_t local_primary_control_ఎగువ = x86_rdmsr_దిగువ(procbased_ctls);
    uint32_t local_primary_control_దిగువ = x86_rdmsr_ఎగువ(procbased_ctls);
    uint32_t local_secondary_control_ఎగువ = x86_rdmsr_దిగువ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_secondary_control_దిగువ = x86_rdmsr_ఎగువ(IA32_VMX_PROCBASED_CTLS2_MSR);
    uint32_t local_exit_control_ఎగువ = x86_rdmsr_దిగువ(exit_ctls);
    uint32_t local_exit_control_దిగువ = x86_rdmsr_ఎగువ(exit_ctls);
    uint32_t local_నమోదు_control_ఎగువ = x86_rdmsr_దిగువ(నమోదు_ctls);
    uint32_t local_నమోదు_control_దిగువ = x86_rdmsr_ఎగువ(నమోదు_ctls);

    uint32_t local_cr0_ఎగువ = x86_rdmsr_దిగువ(IA32_VMX_CR0_FIXED0_MSR);
    uint32_t local_cr0_దిగువ = x86_rdmsr_దిగువ(IA32_VMX_CR0_FIXED1_MSR);
    uint32_t local_cr4_ఎగువ = x86_rdmsr_దిగువ(IA32_VMX_CR4_FIXED0_MSR);
    uint32_t local_cr4_దిగువ = x86_rdmsr_దిగువ(IA32_VMX_CR4_FIXED1_MSR);

    /* See "init_vtx_fixed_values" */
    ఒకవేళ (local_secondary_control_దిగువ & బిట్టు(7)) {
        local_cr0_ఎగువ &= ~(CR0_PE | CR0_PG);
    }

    /* We want to check that any bits that there are no bits that this core
     * requires to be high, that the BSP did not require to be high. This can
     * be checked with 'local_high & high == local_high'.
     * Also need to make sure that the BSP has not determined that any bits should
     * be high that this core requires to be low. This can be checked with
     * '~local_low & high == 0'
     */
    తిరిగి
        (local_pin_control_ఎగువ & pin_control_ఎగువ) == local_pin_control_ఎగువ &&
        (~local_pin_control_దిగువ & pin_control_ఎగువ) == 0 &&
        (local_primary_control_ఎగువ & primary_control_ఎగువ) == local_primary_control_ఎగువ &&
        (~local_primary_control_దిగువ & primary_control_ఎగువ) == 0 &&
        (local_secondary_control_ఎగువ & secondary_control_ఎగువ) == local_secondary_control_ఎగువ &&
        (~local_secondary_control_దిగువ & secondary_control_ఎగువ) == 0 &&
        (local_exit_control_ఎగువ & exit_control_ఎగువ) == local_exit_control_ఎగువ &&
        (~local_exit_control_దిగువ & exit_control_ఎగువ) == 0 &&
        (local_నమోదు_control_ఎగువ & నమోదు_control_ఎగువ) == local_నమోదు_control_ఎగువ &&
        (~local_నమోదు_control_దిగువ & నమోదు_control_ఎగువ) == 0 &&
        local_cr0_ఎగువ == cr0_ఎగువ &&
        local_cr0_దిగువ == cr0_దిగువ &&
        local_cr4_ఎగువ == cr4_ఎగువ &&
        local_cr4_దిగువ == cr4_దిగువ;
}

స్థిర అంతఃవరుస uint32_t apply_Fixed_బిట్లు(uint32_t original, uint32_t ఎగువ, uint32_t దిగువ)
{
    original |= ఎగువ;
    original &= దిగువ;
    తిరిగి original;
}

శూన్య vcpu_init(vcpu_t *vcpu)
{
    vcpu->vcpuTCB = ఖాళీ;
    vcpu->launched = false;
    vcpu->fpu_చురుకైన = false;
    vcpu->fpuState = x86KSnullFpuState;

    జ్ఞాపకం_నకలు(vcpu->vmcs, &vmcs_revision, 4);

    vcpu->cr0 = cr0_ఎగువ & cr0_దిగువ;
    vcpu->cr0_shadow = 0;
    vcpu->cr0_ముసుగు = 0;
    vcpu->exception_bitmap = 0;
    vcpu->vpid = VPID_INVALID;
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    vcpu->చివరి_cpu = getCurrentCPUIndex();
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    switchVCPU(vcpu);

    vmwrite(VMX_HOST_PAT, x86_rdmsr(IA32_PAT_MSR));
    vmwrite(VMX_HOST_EFER, x86_rdmsr(IA32_EFER_MSR));
    // By default we will disable performance counters when we come out
    // of a VM. When performance counters are supported this host state
    // needs to be updated on VM entry
    ఒకవేళ (vmx_feature_ఎక్కించు_perf_global_ctrl) {
        vmwrite(VMX_HOST_PERF_GLOBAL_CTRL, 0);
    }
    vmwrite(VMX_HOST_CR0, read_cr0());
    vmwrite(VMX_HOST_CR4, read_cr4());
    vmwrite(VMX_HOST_FS_పునాది, 0);
    vmwrite(VMX_HOST_GS_పునాది, 0);
    vmwrite(VMX_HOST_TR_పునాది, (మాట_t)&x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KStss);
    vmwrite(VMX_HOST_GDTR_పునాది, (మాట_t)x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KSgdt);
    vmwrite(VMX_HOST_IDTR_పునాది, (మాట_t)x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KSidt);
    vmwrite(VMX_HOST_SYSENTER_CS, (మాట_t)SEL_CS_0);
    vmwrite(VMX_HOST_SYSENTER_EIP, (మాట_t)&handle_syscall);
    ఒకవేళ (!config_సమితి(CONFIG_HARDWARE_DEBUG_API)) {
        vmwrite(VMX_HOST_SYSENTER_ESP, (uint64_t)(మాట_t)((అక్షరం *)&x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KStss.tss.పదాలు[0] +
                                                          4));
    }
    /* Set host SP to point just beyond the first field to be stored on exit. */
    vmwrite(VMX_HOST_RSP, (మాట_t)&vcpu->gp_registers[n_vcpu_gp_రిజిస్టరు]);
    vmwrite(VMX_HOST_RIP, (మాట_t)&handle_vmexit);
#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
    /* if we have a skim window then our host cr3 is a constant and is always the
     * the kernel address space, so we set it here instead of lazily in restoreVMCS */
    vmwrite(VMX_HOST_CR3, makeCR3(kpptr_గమ్యం_paddr(x64KSKernelPML4), 0).పదాలు[0]);
#అయితే_ముగింపు /* CONFIG_KERNEL_SKIM_WINDOW */

    vmwrite(VMX_HOST_ES_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_CS_SELECTOR, SEL_CS_0);
    vmwrite(VMX_HOST_SS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_DS_SELECTOR, SEL_DS_0);
    vmwrite(VMX_HOST_FS_SELECTOR, 0);
    vmwrite(VMX_HOST_GS_SELECTOR, 0);
    vmwrite(VMX_HOST_TR_SELECTOR, SEL_TSS);

    /* Set fixed VMCS control fields. */
    vmwrite(VMX_CONTROL_PIN_EXECUTION_CONTROLS, pin_control_ఎగువ & pin_control_దిగువ);
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, primary_control_ఎగువ & primary_control_దిగువ);
    vmwrite(VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS, secondary_control_ఎగువ & secondary_control_దిగువ);
    vmwrite(VMX_CONTROL_EXIT_CONTROLS, exit_control_ఎగువ & exit_control_దిగువ);

    uint64_t నమోదు_control = నమోదు_control_ఎగువ & నమోదు_control_దిగువ;
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
    /* Put VCPU in IA-32e mode after VM entry for backward compatibility with previous versions of seL4.
     * This bit can be zero'ed by userspace to enter in other modes. */
    నమోదు_control |= బిట్టు(9);
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    vmwrite(VMX_CONTROL_నమోదు_CONTROLS, నమోదు_control);

    vmwrite(VMX_CONTROL_MSR_చిరునామా, (మాట_t)kpptr_గమ్యం_paddr(&msr_bitmap_region));
    vmwrite(VMX_GUEST_CR0, vcpu->cr0);
    vmwrite(VMX_GUEST_CR4, cr4_ఎగువ & cr4_దిగువ);

    vmwrite(VMX_GUEST_VMCS_LINK_సూచిక, ~(మాట_t)0);
    vmwrite(VMX_GUEST_VMCS_LINK_సూచిక_ఎగువ, ~(మాట_t)0);

    memset(vcpu->io, ~(మాట_t)0, VCPU_IOBITMAP_కొలత);
    vmwrite(VMX_CONTROL_IOA_చిరునామా, pptr_గమ్యం_paddr(vcpu->io));
    vmwrite(VMX_CONTROL_IOB_చిరునామా, pptr_గమ్యం_paddr((అక్షరం *)vcpu->io + (VCPU_IOBITMAP_కొలత / 2)));
}

స్థిర శూన్య dissociateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ప్రతిజ్ఞా(tcb->tcbArch.tcbVCPU == vcpu);
    ప్రతిజ్ఞా(vcpu->vcpuTCB == tcb);
    fpuRelease(tcb);
    vcpu->fpu_చురుకైన = false;
    tcb->tcbArch.tcbVCPU = ఖాళీ;
    vcpu->vcpuTCB = ఖాళీ;
}

శూన్య vcpu_finalise(vcpu_t *vcpu)
{
    ఒకవేళ (ARCH_కణుపు_STATE_ON_CORE(x86KSCurrentVCPU, vcpu->చివరి_cpu) == vcpu) {
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
        ఒకవేళ (vcpu->చివరి_cpu != getCurrentCPUIndex()) {
            doRemoteClearCurrentVCPU(vcpu->చివరి_cpu);
        } లేకపోతే
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
        {
            clearCurrentVCPU();
        }
    }
    ఒకవేళ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    ఒకవేళ (vcpu->vpid != VPID_INVALID) {
        releaseVPID(vcpu->vpid);
    }
}

స్థిర శూన్య associateVcpuTcb(tcb_t *tcb, vcpu_t *vcpu)
{
    ఒకవేళ (tcb->tcbArch.tcbVCPU) {
        dissociateVcpuTcb(tcb, tcb->tcbArch.tcbVCPU);
    }
    ఒకవేళ (vcpu->vcpuTCB) {
        dissociateVcpuTcb(vcpu->vcpuTCB, vcpu);
    }
    vcpu->vcpuTCB = tcb;
    tcb->tcbArch.tcbVCPU = vcpu;
}

స్థిర exception_t invokeVCPUWriteRegisters(vcpu_t *vcpu, మాట_t *బఫరు)
{
    పూర్ణాంకం i;
    కొరకు (i = 0; i < n_vcpu_gp_రిజిస్టరు; i++) {
        vcpu->gp_registers[i] = getSyscallArg(i, బఫరు);
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeVCPUWriteRegisters(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
    ఒకవేళ (నిడివి < 15) {
#లేకపోతే
    ఒకవేళ (నిడివి < 7) {
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */
        userError("VCPU WriteRegisters: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    తిరిగి invokeVCPUWriteRegisters(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), బఫరు);
}

#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
స్థిర exception_t invokeWriteMSR(vcpu_t *vcpu, మాట_t *బఫరు, మాట_t క్షేత్రం, మాట_t విలువ)
{
    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);
    ఒకవేళ (ARCH_కణుపు_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ఎంపిక (క్షేత్రం) {
    స్థితి IA32_LSTAR_MSR:
        vcpu->syscall_registers[VCPU_LSTAR] = విలువ;
        ఆపు;
    స్థితి IA32_నక్షత్రం_MSR:
        vcpu->syscall_registers[VCPU_నక్షత్రం] = విలువ;
        ఆపు;
    స్థితి IA32_CSTAR_MSR:
        vcpu->syscall_registers[VCPU_CSTAR] = విలువ;
        ఆపు;
    స్థితి IA32_FMASK_MSR:
        vcpu->syscall_registers[VCPU_SYSCALL_ముసుగు] = విలువ;
        ఆపు;
    }
    సమితి_MR(thread, బఫరు, 0, విలువ);
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeVCPUWriteMSR(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    మాట_t క్షేత్రం;
    మాట_t విలువ;

    ఒకవేళ (నిడివి < 2) {
        userError("VCPU WriteMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    క్షేత్రం = getSyscallArg(0, బఫరు);
    విలువ = getSyscallArg(1, బఫరు);
    ఎంపిక (క్షేత్రం) {
    స్థితి IA32_LSTAR_MSR:
    స్థితి IA32_నక్షత్రం_MSR:
    స్థితి IA32_CSTAR_MSR:
    స్థితి IA32_FMASK_MSR:
        ఆపు;
    అప్రమేయ:
        userError("VCPU WriteMSR: Invalid field %lx.", (దీర్ఘ)క్షేత్రం);
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    తిరిగి invokeWriteMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), బఫరు, క్షేత్రం, విలువ);
}

స్థిర exception_t invokeReadMSR(vcpu_t *vcpu, మాట_t క్షేత్రం, మాట_t *బఫరు)
{
    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);

    మాట_t విలువ = 0;

    ఎంపిక (క్షేత్రం) {
    స్థితి IA32_LSTAR_MSR:
        విలువ = vcpu->syscall_registers[VCPU_LSTAR];
        ఆపు;
    స్థితి IA32_నక్షత్రం_MSR:
        విలువ = vcpu->syscall_registers[VCPU_నక్షత్రం];
        ఆపు;
    స్థితి IA32_CSTAR_MSR:
        విలువ = vcpu->syscall_registers[VCPU_CSTAR];
        ఆపు;
    స్థితి IA32_FMASK_MSR:
        విలువ = vcpu->syscall_registers[VCPU_SYSCALL_ముసుగు];
        ఆపు;
    స్థితి IA32_VMX_MISC_MSR:
        విలువ = x86_rdmsr(క్షేత్రం);
        ఆపు;
    }

    సమితి_MR(thread, బఫరు, 0, విలువ);
    సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                    se_L4_సందేశం_Info_new(0, 0, 0, 1)));
    సమితి_Thread_State(thread, ThreadState_Restart);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeVCPUReadMSR(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    ఒకవేళ (నిడివి < 1) {
        userError("VCPU ReadMSR: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    మాట_t క్షేత్రం = getSyscallArg(0, బఫరు);
    ఎంపిక (క్షేత్రం) {
    స్థితి IA32_LSTAR_MSR:
    స్థితి IA32_నక్షత్రం_MSR:
    స్థితి IA32_CSTAR_MSR:
    స్థితి IA32_FMASK_MSR:
    స్థితి IA32_VMX_MISC_MSR:
        ఆపు;
    అప్రమేయ:
        userError("VCPU ReadMSR: Invalid field %lx.", (దీర్ఘ)క్షేత్రం);
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    తిరిగి invokeReadMSR(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), క్షేత్రం, బఫరు);
}
#అయితే_ముగింపు  /* CONFIG_X86_64_VTX_64BIT_GUESTS */

స్థిర exception_t invokeEnableIOPort(vcpu_t *vcpu, cte_t *స్థానం, cap_t cap, uint16_t దిగువ, uint16_t ఎగువ)
{
    /* remove any existing io ports from this cap */
    clearVPIDIOPortMappings(cap_io_పోర్టు_cap_get_cap_IOPort_VPID(cap),
                            cap_io_పోర్టు_cap_get_cap_IOPort_మొదటి_పోర్టు(cap),
                            cap_io_పోర్టు_cap_get_cap_IOPort_చివరి_పోర్టు(cap));
    /* update the assigned vpid. If the vcpu does not have a valid vpid then
     * this is fine as whilst the cap will not point to the vcpu, the vcpu
     * will have its port mask cleared when it gets assigned a vpid */
    cap = cap_io_పోర్టు_cap_సమితి_cap_IOPort_VPID(cap, vcpu->vpid);
    స్థానం->cap = cap;
    సమితి_IOPort_ముసుగు(vcpu->io, దిగువ, ఎగువ, false);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeEnableIOPort(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    vcpu_t *vcpu;
    cap_t ioCap;
    cte_t *io_స్థానం;
    uint16_t దిగువ, ఎగువ;

    ఒకవేళ (నిడివి < 2) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("VCPU EnableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    io_స్థానం = current_extra_caps.excaprefs[0];
    ioCap  = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cap_get_capType(ioCap) != cap_io_పోర్టు_cap) {
        userError("VCPU EnableIOPort: IOPort cap is not a IOPort cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    దిగువ = getSyscallArg(0, బఫరు);
    ఎగువ = getSyscallArg(1, బఫరు);

    ఒకవేళ (దిగువ < cap_io_పోర్టు_cap_get_cap_IOPort_మొదటి_పోర్టు(ioCap) || ఎగువ > cap_io_పోర్టు_cap_get_cap_IOPort_చివరి_పోర్టు(ioCap)) {
        userError("VCPU EnableIOPort: Requested range not valid for given IOPort cap");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeEnableIOPort(vcpu, io_స్థానం, ioCap, దిగువ, ఎగువ);
}

స్థిర exception_t invokeDisableIOPort(vcpu_t *vcpu, uint16_t దిగువ, uint16_t ఎగువ)
{
    సమితి_IOPort_ముసుగు(vcpu->io, దిగువ, ఎగువ, true);
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeDisableIOPort(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    vcpu_t *vcpu;
    uint16_t దిగువ, ఎగువ;

    ఒకవేళ (నిడివి < 2) {
        userError("VCPU DisableIOPort: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    దిగువ = getSyscallArg(0, బఫరు);
    ఎగువ = getSyscallArg(1, బఫరు);

    vcpu = VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap));

    తిరిగి invokeDisableIOPort(vcpu, దిగువ, ఎగువ);
}

స్థిర exception_t invokeWriteVMCS(vcpu_t *vcpu, bool_t call, మాట_t *బఫరు, మాట_t క్షేత్రం, మాట_t విలువ)
{
    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);

    ఒకవేళ (ARCH_కణుపు_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    ఎంపిక (క్షేత్రం) {
    స్థితి VMX_CONTROL_EXCEPTION_BITMAP:
        vcpu->exception_bitmap = vcpu->cached_exception_bitmap = విలువ;
        ఆపు;
    స్థితి VMX_GUEST_CR0:
        vcpu->cr0 = vcpu->cached_cr0 = విలువ;
        ఆపు;
    స్థితి VMX_CONTROL_CR0_ముసుగు:
        vcpu->cr0_ముసుగు = vcpu->cached_cr0_ముసుగు = విలువ;
        ఆపు;
    స్థితి VMX_CONTROL_CR0_READ_SHADOW:
        vcpu->cr0_shadow = vcpu->cached_cr0_shadow = విలువ;
        ఆపు;
    }
    vmwrite(క్షేత్రం, విలువ);

    ఒకవేళ (call) {
        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        గుర్తులేని పూర్ణాంకం నిడివి = సమితి_MR(thread, బఫరు, 0, విలువ);
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeWriteVMCS(cap_t cap, మాట_t నిడివి, bool_t call, మాట_t *బఫరు)
{
    మాట_t క్షేత్రం;
    మాట_t విలువ;

    ఒకవేళ (నిడివి < 2) {
        userError("VCPU WriteVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    క్షేత్రం = getSyscallArg(0, బఫరు);
    విలువ = getSyscallArg(1, బఫరు);
    ఎంపిక (క్షేత్రం) {
    స్థితి VMX_GUEST_RIP:
    స్థితి VMX_GUEST_RSP:
    స్థితి VMX_GUEST_ES_SELECTOR:
    స్థితి VMX_GUEST_CS_SELECTOR:
    స్థితి VMX_GUEST_SS_SELECTOR:
    స్థితి VMX_GUEST_DS_SELECTOR:
    స్థితి VMX_GUEST_FS_SELECTOR:
    స్థితి VMX_GUEST_GS_SELECTOR:
    స్థితి VMX_GUEST_LDTR_SELECTOR:
    స్థితి VMX_GUEST_TR_SELECTOR:
    స్థితి VMX_GUEST_DEBUGCTRL:
    స్థితి VMX_GUEST_PAT:
    స్థితి VMX_GUEST_EFER:
    స్థితి VMX_GUEST_PERF_GLOBAL_CTRL:
    స్థితి VMX_GUEST_PDPTE0:
    స్థితి VMX_GUEST_PDPTE1:
    స్థితి VMX_GUEST_PDPTE2:
    స్థితి VMX_GUEST_PDPTE3:
    స్థితి VMX_GUEST_ES_హద్దు:
    స్థితి VMX_GUEST_CS_హద్దు:
    స్థితి VMX_GUEST_SS_హద్దు:
    స్థితి VMX_GUEST_DS_హద్దు:
    స్థితి VMX_GUEST_FS_హద్దు:
    స్థితి VMX_GUEST_GS_హద్దు:
    స్థితి VMX_GUEST_LDTR_హద్దు:
    స్థితి VMX_GUEST_TR_హద్దు:
    స్థితి VMX_GUEST_GDTR_హద్దు:
    స్థితి VMX_GUEST_IDTR_హద్దు:
    స్థితి VMX_GUEST_ES_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_CS_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_SS_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_DS_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_FS_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_GS_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_LDTR_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_TR_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_INTERRUPTABILITY:
    స్థితి VMX_GUEST_ACTIVITY:
    స్థితి VMX_GUEST_SMBASE:
    స్థితి VMX_GUEST_SYSENTER_CS:
    స్థితి VMX_GUEST_PREEMPTION_TIMER_విలువ:
    స్థితి VMX_GUEST_ES_పునాది:
    స్థితి VMX_GUEST_CS_పునాది:
    స్థితి VMX_GUEST_SS_పునాది:
    స్థితి VMX_GUEST_DS_పునాది:
    స్థితి VMX_GUEST_FS_పునాది:
    స్థితి VMX_GUEST_GS_పునాది:
    స్థితి VMX_GUEST_LDTR_పునాది:
    స్థితి VMX_GUEST_TR_పునాది:
    స్థితి VMX_GUEST_GDTR_పునాది:
    స్థితి VMX_GUEST_IDTR_పునాది:
    స్థితి VMX_GUEST_DR7:
    స్థితి VMX_GUEST_RFLAGS:
    స్థితి VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    స్థితి VMX_GUEST_SYSENTER_ESP:
    స్థితి VMX_GUEST_SYSENTER_EIP:
    స్థితి VMX_CONTROL_CR0_ముసుగు:
    స్థితి VMX_CONTROL_CR4_ముసుగు:
    స్థితి VMX_CONTROL_CR0_READ_SHADOW:
    స్థితి VMX_CONTROL_CR4_READ_SHADOW:
    స్థితి VMX_GUEST_CR3:
    స్థితి VMX_CONTROL_EXCEPTION_BITMAP:
    స్థితి VMX_CONTROL_నమోదు_INTERRUPTION_INFO:
    స్థితి VMX_CONTROL_నమోదు_EXCEPTION_ERROR_సంకేతం:
        ఆపు;
    స్థితి VMX_CONTROL_PIN_EXECUTION_CONTROLS:
        విలువ = apply_Fixed_బిట్లు(విలువ, pin_control_ఎగువ, pin_control_దిగువ);
        ఆపు;
    స్థితి VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
        విలువ = apply_Fixed_బిట్లు(విలువ, primary_control_ఎగువ, primary_control_దిగువ);
        ఆపు;
    స్థితి VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
        విలువ = apply_Fixed_బిట్లు(విలువ, secondary_control_ఎగువ, secondary_control_దిగువ);
        ఆపు;
    స్థితి VMX_CONTROL_నమోదు_CONTROLS:
        విలువ = apply_Fixed_బిట్లు(విలువ, నమోదు_control_ఎగువ, నమోదు_control_దిగువ);
        ఆపు;
    స్థితి VMX_CONTROL_EXIT_CONTROLS:
        విలువ = apply_Fixed_బిట్లు(విలువ, exit_control_ఎగువ, exit_control_దిగువ);
        ఆపు;
    స్థితి VMX_GUEST_CR0:
        విలువ = apply_Fixed_బిట్లు(విలువ, cr0_ఎగువ, cr0_దిగువ);
        ఆపు;
    స్థితి VMX_GUEST_CR4:
        విలువ = apply_Fixed_బిట్లు(విలువ, cr4_ఎగువ, cr4_దిగువ);
        ఆపు;
    అప్రమేయ:
        userError("VCPU WriteVMCS: Invalid field %lx.", (దీర్ఘ)క్షేత్రం);
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    తిరిగి invokeWriteVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), call, బఫరు, క్షేత్రం, విలువ);
}

స్థిర మాట_t readVMCSField(vcpu_t *vcpu, మాట_t క్షేత్రం)
{
    ఎంపిక (క్షేత్రం) {
    స్థితి VMX_CONTROL_EXCEPTION_BITMAP:
        తిరిగి vcpu->exception_bitmap;
    స్థితి VMX_GUEST_CR0:
        తిరిగి vcpu->cr0;
    స్థితి VMX_CONTROL_CR0_ముసుగు:
        తిరిగి vcpu->cr0_ముసుగు;
    స్థితి VMX_CONTROL_CR0_READ_SHADOW:
        తిరిగి vcpu->cr0_shadow;
    }
    ఒకవేళ (ARCH_కణుపు_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    తిరిగి vmread(క్షేత్రం);
}

స్థిర exception_t invokeReadVMCS(vcpu_t *vcpu, మాట_t క్షేత్రం, bool_t call, మాట_t *బఫరు)
{
    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);
    మాట_t విలువ = readVMCSField(vcpu, క్షేత్రం);
    ఒకవేళ (call) {
        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        గుర్తులేని పూర్ణాంకం నిడివి = సమితి_MR(thread, బఫరు, 0, విలువ);
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeReadVMCS(cap_t cap, మాట_t నిడివి, bool_t call, మాట_t *బఫరు)
{
    ఒకవేళ (నిడివి < 1) {
        userError("VCPU ReadVMCS: Not enough arguments.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    మాట_t క్షేత్రం = getSyscallArg(0, బఫరు);
    ఎంపిక (క్షేత్రం) {
    స్థితి VMX_GUEST_RIP:
    స్థితి VMX_GUEST_RSP:
    స్థితి VMX_GUEST_ES_SELECTOR:
    స్థితి VMX_GUEST_CS_SELECTOR:
    స్థితి VMX_GUEST_SS_SELECTOR:
    స్థితి VMX_GUEST_DS_SELECTOR:
    స్థితి VMX_GUEST_FS_SELECTOR:
    స్థితి VMX_GUEST_GS_SELECTOR:
    స్థితి VMX_GUEST_LDTR_SELECTOR:
    స్థితి VMX_GUEST_TR_SELECTOR:
    స్థితి VMX_GUEST_DEBUGCTRL:
    స్థితి VMX_GUEST_PAT:
    స్థితి VMX_GUEST_EFER:
    స్థితి VMX_GUEST_PERF_GLOBAL_CTRL:
    స్థితి VMX_GUEST_PDPTE0:
    స్థితి VMX_GUEST_PDPTE1:
    స్థితి VMX_GUEST_PDPTE2:
    స్థితి VMX_GUEST_PDPTE3:
    స్థితి VMX_GUEST_ES_హద్దు:
    స్థితి VMX_GUEST_CS_హద్దు:
    స్థితి VMX_GUEST_SS_హద్దు:
    స్థితి VMX_GUEST_DS_హద్దు:
    స్థితి VMX_GUEST_FS_హద్దు:
    స్థితి VMX_GUEST_GS_హద్దు:
    స్థితి VMX_GUEST_LDTR_హద్దు:
    స్థితి VMX_GUEST_TR_హద్దు:
    స్థితి VMX_GUEST_GDTR_హద్దు:
    స్థితి VMX_GUEST_IDTR_హద్దు:
    స్థితి VMX_GUEST_ES_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_CS_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_SS_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_DS_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_FS_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_GS_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_LDTR_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_TR_ACCESS_RIGHTS:
    స్థితి VMX_GUEST_INTERRUPTABILITY:
    స్థితి VMX_GUEST_ACTIVITY:
    స్థితి VMX_GUEST_SMBASE:
    స్థితి VMX_GUEST_SYSENTER_CS:
    స్థితి VMX_GUEST_PREEMPTION_TIMER_విలువ:
    స్థితి VMX_GUEST_ES_పునాది:
    స్థితి VMX_GUEST_CS_పునాది:
    స్థితి VMX_GUEST_SS_పునాది:
    స్థితి VMX_GUEST_DS_పునాది:
    స్థితి VMX_GUEST_FS_పునాది:
    స్థితి VMX_GUEST_GS_పునాది:
    స్థితి VMX_GUEST_LDTR_పునాది:
    స్థితి VMX_GUEST_TR_పునాది:
    స్థితి VMX_GUEST_GDTR_పునాది:
    స్థితి VMX_GUEST_IDTR_పునాది:
    స్థితి VMX_GUEST_DR7:
    స్థితి VMX_GUEST_RFLAGS:
    స్థితి VMX_GUEST_PENDING_DEBUG_EXCEPTIONS:
    స్థితి VMX_GUEST_SYSENTER_ESP:
    స్థితి VMX_GUEST_SYSENTER_EIP:
    స్థితి VMX_CONTROL_CR0_ముసుగు:
    స్థితి VMX_CONTROL_CR4_ముసుగు:
    స్థితి VMX_CONTROL_CR0_READ_SHADOW:
    స్థితి VMX_CONTROL_CR4_READ_SHADOW:
    స్థితి VMX_దత్తాంశం_INSTRUCTION_ERROR:
    స్థితి VMX_దత్తాంశం_EXIT_అంతరాయం_INFO:
    స్థితి VMX_దత్తాంశం_EXIT_అంతరాయం_ERROR:
    స్థితి VMX_దత్తాంశం_IDT_సదిశ_INFO:
    స్థితి VMX_దత్తాంశం_IDT_సదిశ_ERROR:
    స్థితి VMX_దత్తాంశం_EXIT_INSTRUCTION_నిడివి:
    స్థితి VMX_దత్తాంశం_EXIT_INSTRUCTION_INFO:
    స్థితి VMX_దత్తాంశం_GUEST_PHYSICAL:
    స్థితి VMX_దత్తాంశం_IO_RCX:
    స్థితి VMX_దత్తాంశం_IO_RSI:
    స్థితి VMX_దత్తాంశం_IO_RDI:
    స్థితి VMX_దత్తాంశం_IO_RIP:
    స్థితి VMX_దత్తాంశం_GUEST_రేఖీయ_చిరునామా:
    స్థితి VMX_CONTROL_నమోదు_INTERRUPTION_INFO:
    స్థితి VMX_CONTROL_PIN_EXECUTION_CONTROLS:
    స్థితి VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS:
    స్థితి VMX_CONTROL_SECONDARY_PROCESSOR_CONTROLS:
    స్థితి VMX_CONTROL_EXCEPTION_BITMAP:
    స్థితి VMX_CONTROL_నమోదు_CONTROLS:
    స్థితి VMX_CONTROL_EXIT_CONTROLS:
    స్థితి VMX_GUEST_CR0:
    స్థితి VMX_GUEST_CR3:
    స్థితి VMX_GUEST_CR4:
        ఆపు;
    అప్రమేయ:
        userError("VCPU ReadVMCS: Invalid field %lx.", (దీర్ఘ)క్షేత్రం);
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    తిరిగి invokeReadVMCS(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), క్షేత్రం, call, బఫరు);
}

స్థిర exception_t invoke_సమితి_TCB(vcpu_t *vcpu, tcb_t *tcb)
{
    associateVcpuTcb(tcb, vcpu);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decode_సమితి_TCB(cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    cap_t tcbCap;
    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("VCPU SetTCB: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    tcbCap  = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cap_get_capType(tcbCap) != cap_thread_cap) {
        userError("TCB cap is not a TCB cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    తిరిగి invoke_సమితి_TCB(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)), TCB_PTR(cap_thread_cap_get_capTCBPtr(tcbCap)));
}

శూన్య vcpu_update_state_sysvmenter(vcpu_t *vcpu)
{
    మాట_t *బఫరు;
    ఒకవేళ (ARCH_కణుపు_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }
    బఫరు = lookupIPCBuffer(false, కణుపు_STATE(ksCurThread));
    ఒకవేళ (!బఫరు) {
        userError("No IPC buffer.");
        తిరిగి;
    }
    vmwrite(VMX_GUEST_RIP, getSyscallArg(0, బఫరు));
    vmwrite(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS, apply_Fixed_బిట్లు(getSyscallArg(1, బఫరు), primary_control_ఎగువ,
                                                                   primary_control_దిగువ));
    vmwrite(VMX_CONTROL_నమోదు_INTERRUPTION_INFO, getSyscallArg(2, బఫరు));
}

శూన్య vcpu_sysvmenter_జవాబు_గమ్యం_user(tcb_t *tcb)
{
    మాట_t *బఫరు;
    vcpu_t *vcpu;

    బఫరు = lookupIPCBuffer(true, tcb);
    vcpu = tcb->tcbArch.tcbVCPU;

    ప్రతిజ్ఞా(vcpu);

    ఒకవేళ (ARCH_కణుపు_STATE(x86KSCurrentVCPU) != vcpu) {
        switchVCPU(vcpu);
    }

    సమితి_MR(tcb, బఫరు, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    సమితి_MR(tcb, బఫరు, SEL4_VMENTER_CALL_CONTROL_PPC_MR, vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));

    సమితి_MR(tcb, బఫరు, SEL4_VMENTER_CALL_అంతరాయం_INFO_MR,
          vmread(VMX_CONTROL_నమోదు_INTERRUPTION_INFO));
    సమితి_రిజిస్టరు(tcb, సందేశం_Info_రిజిస్టరు, 0);
}

exception_t decodeX86VCPUInvocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *స్థానం,
    cap_t cap,
    bool_t call,
    మాట_t *బఫరు
)
{
    ఎంపిక (invLabel) {
    స్థితి X86VCPUSetTCB:
        తిరిగి decode_సమితి_TCB(cap, నిడివి, బఫరు);
    స్థితి X86VCPUReadVMCS:
        తిరిగి decodeReadVMCS(cap, నిడివి, call, బఫరు);
    స్థితి X86VCPUWriteVMCS:
        తిరిగి decodeWriteVMCS(cap, నిడివి, call, బఫరు);
    స్థితి X86VCPUEnableIOPort:
        తిరిగి decodeEnableIOPort(cap, నిడివి, బఫరు);
    స్థితి X86VCPUDisableIOPort:
        తిరిగి decodeDisableIOPort(cap, నిడివి, బఫరు);
    స్థితి X86VCPUWriteRegisters:
        తిరిగి decodeVCPUWriteRegisters(cap, నిడివి, బఫరు);
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
    స్థితి X86VCPUWriteMSR:
        తిరిగి decodeVCPUWriteMSR(cap, నిడివి, బఫరు);
    స్థితి X86VCPUReadMSR:
        తిరిగి decodeVCPUReadMSR(cap, నిడివి, బఫరు);
#అయితే_ముగింపు /* CONFIG_X86_64_VTX_64BIT_GUESTS */
    అప్రమేయ:
        userError("VCPU: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

స్థిర bool_t is_vtx_supported(శూన్య)
{
    /* check for VMX support in CPUID
     * see section 23.7 of Volume 3 of the Intel manual */
    తిరిగి !!(x86_cpuid_ecx(0x1, 0) & బిట్టు(5));
}

స్థిర అంతఃవరుస శూన్య clear_బిట్టు(మాట_t *bitmap, మాట_t బిట్టు_2)
{
    పూర్ణాంకం సూచీ = బిట్టు_2 / (పరిమాణం(మాట_t) * 8);
    పూర్ణాంకం స్థానభ్రంశం = బిట్టు_2 % (పరిమాణం(మాట_t) * 8);
    bitmap[సూచీ] &= ~బిట్టు(స్థానభ్రంశం);
}

BOOT_సంకేతం bool_t vtx_init(శూన్య)
{
    ఒకవేళ (!is_vtx_supported()) {
        రాయి("vt-x: not supported\n");
        తిరిగి false;
    }
    vmx_basic_msr_t vmx_basic;
    feature_control_msr_t feature_control;
    vmx_basic.పదాలు[0] = x86_rdmsr_దిగువ(IA32_VMX_BASIC_MSR);
    vmx_basic.పదాలు[1] = x86_rdmsr_ఎగువ(IA32_VMX_BASIC_MSR);
    vmcs_revision = vmx_basic_msr_get_vmcs_revision(vmx_basic);
    feature_control.పదాలు[0] = x86_rdmsr_దిగువ(IA32_FEATURE_CONTROL_MSR);
    ఒకవేళ (!feature_control_msr_get_vmx_outside_smx(feature_control)) {
        /* enable if the MSR is not locked */
        ఒకవేళ (feature_control_msr_get_తాళం(feature_control)) {
            రాయి("vt-x: feature locked\n");
            తిరిగి false;
        }
        feature_control = feature_control_msr_సమితి_vmx_outside_smx(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ఎగువ(IA32_FEATURE_CONTROL_MSR), feature_control.పదాలు[0]);
    }
    /* make sure the msr is locked */
    ఒకవేళ (!feature_control_msr_get_తాళం(feature_control)) {
        feature_control = feature_control_msr_సమితి_తాళం(feature_control, 1);
        x86_wrmsr_parts(IA32_FEATURE_CONTROL_MSR, x86_rdmsr_ఎగువ(IA32_FEATURE_CONTROL_MSR), feature_control.పదాలు[0]);
    }
    /* Initialize the fixed values only on the boot core. All other cores
     * will just check that the fixed values are valid */
    ఒకవేళ (CURRENT_CPU_సూచీ() == 0) {
        ఒకవేళ (!init_vtx_fixed_విలువలు(vmx_basic_msr_get_true_msrs(vmx_basic))) {
            రాయి("vt-x: lack of required features\n");
            తిరిగి false;
        }
    }
    ఒకవేళ (!check_vtx_fixed_విలువలు(vmx_basic_msr_get_true_msrs(vmx_basic))) {
        రాయి("vt-x: cores have inconsistent features\n");
        తిరిగి false;
    }
    write_cr4(read_cr4() | CR4_VMXE);
    /* we are required to set the VMCS region in the VMXON region */
    నిర్మాణం vmxon_region *vmxon_kptr = &ARCH_కణుపు_STATE(x86KSVMXOnRegion);
    vmxon_kptr->revision = vmcs_revision;
    /* Before calling vmxon, we must check that CR0 and CR4 are not set to values
     * that are unsupported by vt-x */
    ఒకవేళ (!vtx_check_fixed_విలువలు(read_cr0(), read_cr4())) {
        తిరిగి false;
    }
    ఒకవేళ (vmxon(kpptr_గమ్యం_paddr(vmxon_kptr))) {
        రాయి("vt-x: vmxon failure\n");
        తిరిగి false;
    }
    memset(&msr_bitmap_region, ~0, పరిమాణం(msr_bitmap_region));
    /* Set sysenter MSRs to writeable and readable. These are all low msrs */
    clear_బిట్టు(msr_bitmap_region.దిగువ_msr_read.bitmap, IA32_SYSENTER_CS_MSR);
    clear_బిట్టు(msr_bitmap_region.దిగువ_msr_read.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_బిట్టు(msr_bitmap_region.దిగువ_msr_read.bitmap, IA32_SYSENTER_EIP_MSR);
    clear_బిట్టు(msr_bitmap_region.దిగువ_msr_write.bitmap, IA32_SYSENTER_CS_MSR);
    clear_బిట్టు(msr_bitmap_region.దిగువ_msr_write.bitmap, IA32_SYSENTER_ESP_MSR);
    clear_బిట్టు(msr_bitmap_region.దిగువ_msr_write.bitmap, IA32_SYSENTER_EIP_MSR);
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
    /* Allow guest access to FS and both GS MSRs */
    clear_బిట్టు(msr_bitmap_region.ఎగువ_msr_read.bitmap, MSR_BITMAP_ముసుగు(IA32_FS_పునాది_MSR));
    clear_బిట్టు(msr_bitmap_region.ఎగువ_msr_read.bitmap, MSR_BITMAP_ముసుగు(IA32_GS_పునాది_MSR));
    clear_బిట్టు(msr_bitmap_region.ఎగువ_msr_read.bitmap, MSR_BITMAP_ముసుగు(IA32_KERNEL_GS_పునాది_MSR));
    clear_బిట్టు(msr_bitmap_region.ఎగువ_msr_write.bitmap, MSR_BITMAP_ముసుగు(IA32_FS_పునాది_MSR));
    clear_బిట్టు(msr_bitmap_region.ఎగువ_msr_write.bitmap, MSR_BITMAP_ముసుగు(IA32_GS_పునాది_MSR));
    clear_బిట్టు(msr_bitmap_region.ఎగువ_msr_write.bitmap, MSR_BITMAP_ముసుగు(IA32_KERNEL_GS_పునాది_MSR));
#అయితే_ముగింపు
    /* The VMX_EPT_VPID_CAP MSR exists if VMX supports EPT or VPIDs. Whilst
     * VPID support is optional, EPT support is not and is already checked for,
     * so we know that this MSR is safe to read */
    vpid_capability.పదాలు[0] = x86_rdmsr_దిగువ(IA32_VMX_EPT_VPID_CAP_MSR);
    vpid_capability.పదాలు[1] = x86_rdmsr_ఎగువ(IA32_VMX_EPT_VPID_CAP_MSR);

    /* check for supported EPT features */
    ఒకవేళ (!vmx_ept_vpid_cap_msr_get_ept_wb(vpid_capability)) {
        రాయి("vt-x: Expected wb attribute for EPT paging structure\n");
        తిరిగి false;
    }
    ఒకవేళ (!vmx_ept_vpid_cap_msr_get_ept_2m(vpid_capability)) {
        రాయి("vt-x: Expected supported for 2m pages\n");
        తిరిగి false;
    }

    తిరిగి true;
}

స్థిర శూన్య సమితి_MRs_vmexit(uint32_t reason, మాట_t qualification)
{
    మాట_t *బఫరు;
    పూర్ణాంకం i;

    బఫరు = lookupIPCBuffer(true, కణుపు_STATE(ksCurThread));

    సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_CALL_EIP_MR, vmread(VMX_GUEST_RIP));
    సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_CALL_CONTROL_PPC_MR,
          vmread(VMX_CONTROL_PRIMARY_PROCESSOR_CONTROLS));
    సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_CALL_అంతరాయం_INFO_MR,
          vmread(VMX_CONTROL_నమోదు_INTERRUPTION_INFO));
    సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_FAULT_REASON_MR, reason);
    సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_FAULT_QUALIFICATION_MR, qualification);

    సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_FAULT_INSTRUCTION_LEN_MR, vmread(VMX_దత్తాంశం_EXIT_INSTRUCTION_నిడివి));
    సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_FAULT_GUEST_PHYSICAL_MR, vmread(VMX_దత్తాంశం_GUEST_PHYSICAL));
    సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_FAULT_RFLAGS_MR, vmread(VMX_GUEST_RFLAGS));
    సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_FAULT_GUEST_INT_MR, vmread(VMX_GUEST_INTERRUPTABILITY));
    సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_FAULT_CR3_MR, vmread(VMX_GUEST_CR3));

    కొరకు (i = 0; i < n_vcpu_gp_రిజిస్టరు; i++) {
        సమితి_MR(కణుపు_STATE(ksCurThread), బఫరు, SEL4_VMENTER_FAULT_EAX + i,
              కణుపు_STATE(ksCurThread)->tcbArch.tcbVCPU->gp_registers[i]);
    }
}

స్థిర శూన్య handleVmxFault(uint32_t reason, మాట_t qualification)
{
    /* Indicate that we are returning the from VMEnter with a fault */
    సమితి_రిజిస్టరు(కణుపు_STATE(ksCurThread), సందేశం_Info_రిజిస్టరు, SEL4_VMENTER_ఫలితం_FAULT);

    సమితి_MRs_vmexit(reason, qualification);

    /* Set the thread back to running */
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);

    /* No need to schedule because this wasn't an interrupt and
     * we run at the same priority */
    activateThread();
}

స్థిర అంతఃవరుస శూన్య ముగింపు_Vmexit_Saving(శూన్య)
{
    vcpu_t *vcpu = ARCH_కణుపు_STATE(x86KSCurrentVCPU);
    ప్రతిజ్ఞా(vcpu == కణుపు_STATE(ksCurThread)->tcbArch.tcbVCPU);
    vcpu->launched = true;
    /* Update our cache of what is in the vmcs. This is the only value
     * that we cache that can be modified by the guest during execution */
    vcpu->cached_cr0 = vmread(VMX_GUEST_CR0);
    vcpu->cr0 = vcpu->cached_cr0;
}

exception_t handleVmexit(శూన్య)
{
    uint32_t అంతరాయం;
    /* qualification is host width, reason is defined as being 32 bit */
    మాట_t qualification;
    uint32_t reason;
    ముగింపు_Vmexit_Saving();
#నిర్వచితమైతే CONFIG_X86_64_VTX_64_బిట్టు_GUESTS
    vcpu_restore_host_msrs();
#అయితే_ముగింపు
    /* the basic exit reason is the bottom 16 bits of the exit reason field */
    reason = vmread(VMX_దత్తాంశం_EXIT_REASON) & ముసుగు(16);
    ఒకవేళ (reason == EXTERNAL_అంతరాయం) {
        కణుపు_తాళం_IRQ;
        ఒకవేళ (vmx_feature_ack_on_exit) {
            అంతరాయం = vmread(VMX_దత్తాంశం_EXIT_అంతరాయం_INFO) & 0xff;
            ARCH_కణుపు_STATE(x86_KScur_అంతరాయం) = అంతరాయం;
            handle_అంతరాయం_నమోదు();
        } లేకపోతే {
            /* poll for the pending irq. We will then handle it once we return back
             * up to restore_user_context */
            receivePendingIRQ();
        }
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
        VMCheckBoundNotification(కణుపు_STATE(ksCurThread));
#అయితే_ముగింపు
        తిరిగి EXCEPTION_NONE;
    }

    కణుపు_తాళం_SYS;

    ఎంపిక (reason) {
    స్థితి EXCEPTION_OR_NMI:
    స్థితి MOV_DR:
    స్థితి TASK_SWITCH:
    స్థితి CONTROL_రిజిస్టరు:
    స్థితి IO:
    స్థితి MWAIT:
    స్థితి SIPI:
    స్థితి INVLPG:
    స్థితి INVEPT:
    స్థితి INVVPID:
    స్థితి VMCLEAR:
    స్థితి VMPTRLD:
    స్థితి VMPTRST:
    స్థితి VMREAD:
    స్థితి VMWRITE:
    స్థితి VMXON:
    స్థితి EPT_VIOLATION:
    స్థితి GDTR_OR_IDTR:
    స్థితి LDTR_OR_TR:
    స్థితి TPR_క్రింద_పరిమితి:
    స్థితి APIC_ACCESS:
        qualification = vmread(VMX_దత్తాంశం_EXIT_QUALIFICATION);
        ఆపు;
    అప్రమేయ:
        qualification = 0;
    }

    handleVmxFault(reason, qualification);

    తిరిగి EXCEPTION_NONE;
}

exception_t handle_Vm_నమోదు_Fail(శూన్య)
{
    handleVmxFault(-1, -1);

    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
శూన్య VMCheckBoundNotification(tcb_t *tcb)
{
    /* We want to check if the VM we are currently running has received
     * a message on its bound notification object. This check is done
     * in c_traps when we first perform a SysVMEnter, but we could presently
     * be running a VM and another core may have placed a message on the
     * endpoint
     */
    ప్రతిజ్ఞా(tcb->tcbAffinity == getCurrentCPUIndex());
    notification_t *ntfnPtr = tcb->tcbBoundNotification;
    ఒకవేళ (thread_state_ptr_get_tsType(&tcb->tcbState) == ThreadState_RunningVM
        && ntfnPtr && notification_ptr_get_state(ntfnPtr) == Ntfn_State_చురుకైన) {

        మాట_t badge = notification_ptr_get_ntfn_సందేశం_Identifier(ntfnPtr);
        notification_ptr_సమితి_state(ntfnPtr, NtfnState_Idle);
        సమితి_Thread_State(tcb, ThreadState_Running);
        సమితి_రిజిస్టరు(tcb, badge_రిజిస్టరు, badge);
        Arch_leaveVMAsyncTransfer(tcb);
        /* In the process of performing Arch_leavVMAsyncTransfer we will have
         * had to switch the active VMCS. As a result we might as well try and
         * run this tcb if it is permitted instead of switching VMCS contexts
         * back and forth */
        ఒకవేళ (tcb != కణుపు_STATE(ksCurThread)) {
            possible_Switch_గమ్యం(tcb);
        }
    }
}
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

స్థిర శూన్య invvpid_context(uint16_t vpid)
{
    నిర్మాణం {
        uint64_t vpid : 16;
        uint64_t rsvd : 48;
        uint64_t చిరునామా;
    } PACKED నిర్వాహ్యం = {vpid, 0, 0};
    asm అస్థిర("invvpid %0, %1" :: "m"(నిర్వాహ్యం), "r"((మాట_t)1) : "cc");
}

స్థిర శూన్య సమితి_EPTRoot(cap_t vmxSpace, vcpu_t *vcpu)
{
    paddr_t ept_మూలం;
    ఒకవేళ (cap_get_capType(vmxSpace) != cap_ept_pml4_cap ||
        !cap_ept_pml4_cap_get_capPML4IsMapped(vmxSpace)) {
        ept_మూలం = kpptr_గమ్యం_paddr(null_ept_space);
    } లేకపోతే {
        కనుగొను_EPTFor_ASID_ret_t కనుగొను_ret;
        ept_pml4e_t *pml4;

        pml4 = (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_పునాది_Ptr(vmxSpace);
        కనుగొను_ret = కనుగొను_EPTFor_ASID(cap_ept_pml4_cap_get_capPML4MappedASID(vmxSpace));
        ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE || కనుగొను_ret.ept != pml4) {
            ept_మూలం = kpptr_గమ్యం_paddr(null_ept_space);
        } లేకపోతే {
            ept_మూలం = pptr_గమ్యం_paddr(pml4);
        }
    }
    ఒకవేళ (ept_మూలం != vcpu->చివరి_ept_మూలం) {
        vcpu->చివరి_ept_మూలం = ept_మూలం;
        vmx_eptp_t eptp = vmx_eptp_new(
                              ept_మూలం,       /* paddr of ept */
                              0,              /* do not use accessed and dirty flags */
                              3,              /* depth (4) minus 1 of desired table walking */
                              6               /* write back memory type */
                          );
        vmwrite(VMX_CONTROL_EPT_సూచిక, eptp.పదాలు[0]);
        ప్రతిజ్ఞా(vcpu->vpid != VPID_INVALID);
        ఒకవేళ (vmx_feature_vpid) {
            invvpid_context(vcpu->vpid);
        }
    }
}

శూన్య clearVPIDIOPortMappings(vpid_t vpid, uint16_t మొదటి, uint16_t చివరి)
{
    ఒకవేళ (vpid == VPID_INVALID) {
        తిరిగి;
    }
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    ఒకవేళ (vcpu == ఖాళీ) {
        తిరిగి;
    }
    ప్రతిజ్ఞా(vcpu->vpid == vpid);
    సమితి_IOPort_ముసుగు(vcpu->io, మొదటి, చివరి, true);
}

స్థిర అంతఃవరుస vpid_t తదుపరి_VPID(vpid_t vpid)
{
    ఒకవేళ (vpid == VPID_చివరి) {
        తిరిగి VPID_మొదటి;
    } లేకపోతే {
        తిరిగి vpid + 1;
    }
}

స్థిర శూన్య invalidateVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];
    /* clear the IO bitmap as when we sever the VPID assignment we lose
     * the ability for the references in IO port capabilities to invalidate */
    memset(vcpu->io, ~0, పరిమాణం(vcpu->io));
    /* invalidate the VPID context */
    ఒకవేళ (vmx_feature_vpid) {
        invvpid_context(vpid);
    }
}

/** Disassociate a VPID from its VCPU */
స్థిర శూన్య releaseVPID(vpid_t vpid)
{
    vcpu_t *vcpu = x86KSVPIDTable[vpid];

    invalidateVPID(vpid);
    vcpu->vpid = VPID_INVALID;
    x86KSVPIDTable[vpid] = ఖాళీ;
}

స్థిర vpid_t కనుగొను_Free_VPID(శూన్య)
{
    vpid_t vpid;

    vpid = x86KSNextVPID;
    చేయి {
        ఒకవేళ (x86KSVPIDTable[vpid] == ఖాళీ) {
            తిరిగి vpid;
        }
        vpid = తదుపరి_VPID(vpid);
    } అయితే (vpid != x86KSNextVPID);

    /* Forcibly take the next VPID */
    vpid = x86KSNextVPID;
    releaseVPID(vpid);

    x86KSNextVPID = తదుపరి_VPID(x86KSNextVPID);
    తిరిగి vpid;
}

స్థిర శూన్య storeVPID(vcpu_t *vcpu, vpid_t vpid)
{
    ప్రతిజ్ఞా(x86KSVPIDTable[vpid] == ఖాళీ);
    ప్రతిజ్ఞా(vcpu->vpid == VPID_INVALID);
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
శూన్య vcpu_fpu_గమ్యం_guest(tcb_t *tcb, vcpu_t *vcpu)
{
    ఒకవేళ (nativeThreadUsingFPU(tcb)) {
        /* Make sure FPU is enabled */
        enableFpu();
        ఒకవేళ (!vcpu->fpu_చురుకైన) {
            /* Host was using the FPU, switch to guest FPU state */
            saveFpuState(tcb);
            vcpu->fpu_చురుకైన = true;
            ఎక్కించు_Fpu_State(tcb);
        }
    } లేకపోతే {
        /* Someone else used the FPU, load guest's FPU state */
        vcpu->fpu_చురుకైన = true;
        switchLocalFpuOwner(tcb);
    }
}

శూన్య vcpu_fpu_గమ్యం_host(tcb_t *tcb, vcpu_t *vcpu)
{
    ఒకవేళ (tcb->tcbFlags & seL4_TCBFlag_fpuDisabled) {
        /* FPU may have been enabled for the guest, but host isn't allowed to use it */
        disableFpu();
    } లేకపోతే ఒకవేళ (nativeThreadUsingFPU(tcb) && vcpu->fpu_చురుకైన) {
        /* Guest was using the FPU, switch to host FPU state */
        saveFpuState(tcb);
        vcpu->fpu_చురుకైన = false;
        ఎక్కించు_Fpu_State(tcb);
    } లేకపోతే ఒకవేళ (!nativeThreadUsingFPU(tcb)) {
        /* Handle corner cases like dissociated vcpu */
        switchLocalFpuOwner(tcb);
    }
}

శూన్య restoreVMCS(శూన్య)
{
    tcb_t *cur_thread = కణుపు_STATE(ksCurThread);
    vcpu_t *expected_vmcs = cur_thread->tcbArch.tcbVCPU;

    /* Check that the right VMCS is active and current. */
    ఒకవేళ (ARCH_కణుపు_STATE(x86KSCurrentVCPU) != expected_vmcs) {
        switchVCPU(expected_vmcs);
    }

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_SKIM_కిటికీ
    ఒకవేళ (getCurrentCR3().పదాలు[0] != expected_vmcs->చివరి_host_cr3) {
        expected_vmcs->చివరి_host_cr3 = getCurrentCR3().పదాలు[0];
        vmwrite(VMX_HOST_CR3, getCurrentCR3().పదాలు[0]);
    }
#అయితే_ముగింపు
    ఒకవేళ (expected_vmcs->vpid == VPID_INVALID) {
        vpid_t vpid = కనుగొను_Free_VPID();
        storeVPID(expected_vmcs, vpid);
        ఒకవేళ (vmx_feature_vpid) {
            vmwrite(VMX_CONTROL_VPID, vpid);
        }
    }
    సమితి_EPTRoot(TCB_PTR_CTE_PTR(cur_thread, tcbArchEPTRoot)->cap, expected_vmcs);
}

శూన్య invept(ept_pml4e_t *ept_pml4)
{
    ఒకవేళ (vmx_ept_vpid_cap_msr_get_invept(vpid_capability)) {
        నిర్మాణం {
            uint64_t parts[2];
        } చిరునామా;
        మాట_t type;
        ఒకవేళ (vmx_ept_vpid_cap_msr_get_invept_single_context(vpid_capability)) {
            type = 1;
        } లేకపోతే ఒకవేళ (vmx_ept_vpid_cap_msr_get_invept_all_context(vpid_capability)) {
            type = 2;
        } లేకపోతే {
            /* hardware claims to support invept yet provides us with no actual
             * invept mechanism. This is probably impossible, but just silently
             * ignore if it happens */
            userError("Hardware claimed to support invept, yet provided no mechanism");
            తిరిగి;
        }

        చిరునామా.parts[0] = pptr_గమ్యం_paddr((శూన్య *)ept_pml4);
        చిరునామా.parts[1] = 0;
        asm అస్థిర(
            "invept %0, %1"
            :
            : "m"(చిరునామా),  "r"(type)
            : "memory"
        );
    }
}

#అయితే_ముగింపు
