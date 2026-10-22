/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <benchmark/benchmark.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <kernel/stack.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <machine/debug.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <object/cnode.h>
#అన్తర్భావ <object/untyped.h>
#అన్తర్భావ <arch/api/invocation.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <armv/context_switch.h>
#అన్తర్భావ <arch/object/iospace.h>
#అన్తర్భావ <arch/object/vcpu.h>
#అన్తర్భావ <arch/machine/tlb.h>

/*
 * Memory types are defined in Memory Attribute Indirection Register.
 *  - nGnRnE Device non-Gathering, non-Reordering, No Early write acknowledgement
 *  - nGnRE Unused Device non-Gathering, non-Reordering, Early write acknowledgement
 *  - GRE Unused Device Gathering, Reordering, Early write acknowledgement
 *  - NORMAL_NC Normal Memory, Inner/Outer non-cacheable
 *  - NORMAL Normal Memory, Inner/Outer Write-back non-transient, Write-allocate, Read-allocate
 *  - NORMAL_WT Normal Memory, Inner/Outer Write-through non-transient, No-Write-allocate, Read-allocate
 * Note: These should match with contents of MAIR_EL1 register!
 */
గణన mair_types {
    DEVICE_nGnRnE = 0,
    DEVICE_nGnRE = 1,
    DEVICE_GRE = 2,
    సాధారణ_NC = 3,
    సాధారణ = 4,
    సాధారణ_WT = 5
};

/* Stage-2 translation memory attributes */
గణన mair_s2_types {
    S2_DEVICE_nGnRnE = 0,
    S2_DEVICE_nGnRE = 1,
    S2_DEVICE_nGRE  = 2,
    S2_DEVICE_GRE = 3,

    S2_సాధారణ_INNER_NC_OUTER_NC = 5,
    S2_సాధారణ_INNER_WTC_OUTER_NC = 6,
    S2_సాధారణ_INNER_WBC_OUTER_NC = 7,

    S2_సాధారణ_INNER_NC_OUTER_WTC = 9,
    S2_సాధారణ_INNER_WTC_OUTER_WTC = 10,
    S2_సాధారణ_INNER_WBC_OUTER_WTC = 11,

    S2_సాధారణ_INNER_NC_OUTER_WBC = 13,
    S2_సాధారణ_INNER_WTC_OUTER_WBC = 14,
    S2_సాధారణ_INNER_WBC_OUTER_WBC = 15,

    S2_సాధారణ = S2_సాధారణ_INNER_WBC_OUTER_WBC
};

/* Leif from Linaro said the big.LITTLE clusters should be treated as
 * inner shareable, and we believe so, although the Example B2-1 given in
 * ARM ARM DDI 0487B.b (ID092517) says otherwise.
 */

#నిర్వచించు SMP_SHARE   3

నిర్మాణం lookupPTSlot_ret {
    pte_t *pt_స్థానం;
    మాట_t pt_బిట్లు_ఎడమ;
};
రకనిర్వచనం నిర్మాణం lookupPTSlot_ret lookupPTSlot_ret_t;

నిర్మాణం కనుగొను_VSpace_For_ASID_ret {
    exception_t స్థితి_2;
    vspace_మూలం_t *vspace_మూలం;
};
రకనిర్వచనం నిర్మాణం కనుగొను_VSpace_For_ASID_ret కనుగొను_VSpace_For_ASID_ret_t;

/* Stage-1 access permissions:
 * AP[2:1]  higher EL        EL0
 *   00       rw            None
 *   01       rw            rw
 *   10       r             None
 *   11       r             r
 *
 * Stage-2 access permissions:
 * S2AP    Access from Nonsecure EL1 or Non-secure EL0
 *  00                      None
 *  01                      r
 *  10                      w
 *  11                      rw
 *
 *  For VMs or native seL4 applications, if hypervisor support
 *  is enabled, we use the S2AP. The kernel itself running in
 *  EL2 still uses the Stage-1 AP format.
 */

స్థిర మాట_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ఎంపిక (vm_rights) {
    స్థితి VMKernelOnly:
        ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            తిరిగి 0;
        } లేకపోతే {
            తిరిగి 0;
        }

    స్థితి VMReadWrite:
        ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            తిరిగి 3;
        } లేకపోతే {
            తిరిగి 1;
        }

    స్థితి VMReadOnly:
        ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            తిరిగి 1;
        } లేకపోతే {
            తిరిగి 3;
        }

    అప్రమేయ:
        fail("Invalid VM rights");
    }
}

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
స్థిర అంతఃవరుస CONST మాట_t pte_get_AP(pte_t pte)
{
    ప్రతిజ్ఞా(pte_is_పుట_type(pte));
    ఎంపిక (pte_get_pte_type(pte)) {
    స్థితి pte_pte_4k_పుట:
        తిరిగి pte_pte_4k_పుట_get_AP(pte);

    స్థితి pte_pte_పుట:
        తిరిగి pte_pte_పుట_get_AP(pte);

    అప్రమేయ:
        తిరిగి 0;
    }
}

స్థిర vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    మాట_t access_perms = pte_get_AP(pte);
    ఎంపిక (access_perms) {
    స్థితి 0:
    స్థితి 2:
        తిరిగి VMKernelOnly;
    స్థితి 1:
        తిరిగి VMReadWrite;
    స్థితి 3:
        తిరిగి VMReadOnly;
    అప్రమేయ:
        fail("Invalid AP bit");
    }
}
#అయితే_ముగింపు

vm_rights_t CONST ముసుగు_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ముసుగు)
{
    ఒకవేళ (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు)) {
        తిరిగి VMReadOnly;
    }
    ఒకవేళ (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు)) {
        ఒకవేళ (!seL4_CapRights_get_capAllowWrite(cap_rights_ముసుగు)) {
            తిరిగి VMReadOnly;
        } లేకపోతే {
            తిరిగి VMReadWrite;
        }
    }
    ఒకవేళ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ముసుగు)) {
        userError("Attempted to make unsupported write only mapping");
    }
    తిరిగి VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

/* The 54th bit is defined as UXN (unprivileged execute-never) for stage 1
 * of any translation regime for which stage 1 translation can support
 * two VA ranges. This field applies only to execution at EL0. A value
 * of 0 indicates that this control permits execution.
 *
 * The 54th bit is defined as XN (execute-never) for stage 1 of any translation
 * regime for which the stage 1 translation can support only a singe VA range or
 * stage 2 translation when ARMVv8.2-TTS2UXN is not implemented.
 * This field applies to execution at any exception level to which the stage of
 * translation applies. A value of 0 indicates that this control permits execution.
 *
 * When the kernel is running in EL2, the stage-1 translation only supports one
 * VA range so that the 54th bit is XN. Setting the bit to 0 allows execution.
 *
 */
BOOT_సంకేతం శూన్య map_kernel_చట్రం(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_లక్షణాలు_t లక్షణాలు)
{
    ప్రతిజ్ఞా(vaddr >= PPTR_శిఖరం);

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    మాట_t uxn = vm_లక్షణాలు_get_arm_Execute_Never(లక్షణాలు);
#లేకపోతే
    మాట_t uxn = 1; /* unprivileged execute never */
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    మాట_t attr_సూచీ;
    మాట_t shareable;
    ఒకవేళ (vm_లక్షణాలు_get_arm_పుట_Cacheable(లక్షణాలు)) {
        attr_సూచీ = సాధారణ;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } లేకపోతే {
        attr_సూచీ = DEVICE_nGnRnE;
        shareable = 0;
    }
    armKSGlobalKernelPT[GET_KPT_సూచీ(vaddr, KLVL_FRM_ARM_PT_LVL(3))] = pte_pte_4k_పుట_new(uxn, paddr,
                                                                                            0, /* global */
                                                                                            1, /* access flag */
                                                                                            shareable,
                                                                                            APFromVMRights(vm_rights),
                                                                                            attr_సూచీ);
}

/* Essentially, assert that the ELF, kernel devices, and log buffer are all in the same PUD.
   In the current code this is the top index. This is arbitrary, the important part
   is that they are all the same.
 */
compile_assert(elf_లోపల_kernel_mappings, GET_KPT_సూచీ(KERNEL_ELF_పునాది,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == బిట్టు(PT_సూచీ_బిట్లు) - 1);
compile_assert(kdev_లోపల_kernel_mappings, GET_KPT_సూచీ(KDEV_పునాది,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == బిట్టు(PT_సూచీ_బిట్లు) - 1);
compile_assert(log_buf_లోపల_kernel_mappings, GET_KPT_సూచీ(KS_LOG_పునాది,
                                                         KLVL_FRM_ARM_PT_LVL(1)) == బిట్టు(PT_సూచీ_బిట్లు) - 1);

/**
 *  Per ARM ARM DDI 0487 (version L.b), in B2.11 "Mismatched memory attributes",
 *  when physical memory locations are accessed with mismatched attributes
 *  (caching, memory type (device/normal), shareability), the coherency of these
 *  memory locations may be lost, as well as various violations of read and
 *  write orderings or atomic operations. Other ARM documents refer to the
 *  situation in which two virtual addresses map to the same physical address as
 *  "Memory aliasing" (e.g. Document ID 102376 0200_01_en).
 *
 *  Currently, the memory attributes used in the physical memory window does
 *  match that of the kernel ELF mapping, so this is OK. At the same time,
 *  the current physical window mapping does use NORMAL memory covering memory
 *  that is not DRAM, which is questionable for newer processors with more
 *  speculative execution. (FIXME)
 *
 *  Hence, the kernel should never access kernel ELF memory through the physical
 *  memory window, i.e., never modify the result of `ptrFromPAddr()` when the
 *  physical address lies within the kernel ELF. (Reads are fine if they
 *  have been flushed appropriately per B2.11; this should be the case for
 *  any paging structures).
 *
 *  As an addendum, LWN Article "ARM's multiply-mapped memory mess" specifies
 *  that in ARMv6 specifications this kind of aliasing would be UNPREDICTABLE;
 *  the current ARMv8 A-Profile specifications no longer contain this wording,
 *  and instead specify the loss of coherency and other behaviours in B2.11.
 */
BOOT_సంకేతం శూన్య map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_పునాది_RAW, se_L4_Large_పుట_బిట్లు));
    compile_assert(elf_పునాది_aligned, IS_ALIGNED(KERNEL_ELF_పునాది, se_L4_Large_పుట_బిట్లు));

    /* Please don't overlap with KDEV_BASE */
    ప్రతిజ్ఞా(KERNEL_ELF_శిఖరం <= KDEV_పునాది);
    /* Please don't overlap with KS_LOG_PPTR */
    ప్రతిజ్ఞా(KERNEL_ELF_శిఖరం <= KS_LOG_PPTR);

    స్థిరరాశి మాట_t pd_ప్రారంభం = GET_KPT_సూచీ(KERNEL_ELF_పునాది, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    స్థిరరాశి మాట_t pd_చివర   = GET_KPT_సూచీ(KERNEL_ELF_శిఖరం - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_పునాది;
    వాడని pptr_t vaddr = KERNEL_ELF_పునాది;

    కొరకు (మాట_t pd = pd_ప్రారంభం; pd < pd_చివర; pd++) {
        ప్రతిజ్ఞా(pd == GET_KPT_సూచీ(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_పుట_new(
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#లేకపోతే
                                     /* UXN */ 1,
#అయితే_ముగింపు
                                     /* page_base_address */ paddr,
                                     /* global (nG) */ 0,
                                     /* access flag (AF) */ 1,
                                     /* shareability (SH) */ SMP_TERNARY(SMP_SHARE, 0),
                                     /* AP */ APFromVMRights(VMKernelOnly),
                                     /* AttrIndx */ సాధారణ
                                 );

        paddr += బిట్టు(se_L4_Large_పుట_బిట్లు);
        vaddr += బిట్టు(se_L4_Large_పుట_బిట్లు);
    }
}

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_కిటికీ_pgd_location,
               GET_KPT_సూచీ(PPTR_పునాది, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#లేకపోతే
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_కిటికీ_pgd_location,
               GET_KPT_సూచీ(PPTR_పునాది, KLVL_FRM_ARM_PT_LVL(0)) == బిట్టు(PT_సూచీ_బిట్లు) - 1);
#అయితే_ముగింపు
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_కిటికీ_పునాది_aligned, IS_ALIGNED(PPTR_పునాది, se_L4_Huge_పుట_బిట్లు));
compile_assert(physical_కిటికీ_శిఖరం_aligned, IS_ALIGNED(PPTR_శిఖరం, se_L4_Huge_పుట_బిట్లు));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_కిటికీ_bottom_kernel_region,
               GET_KPT_సూచీ(PPTR_పునాది, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_కిటికీ_శిఖరం_kernel_region,
               GET_KPT_సూచీ(PPTR_శిఖరం, KLVL_FRM_ARM_PT_LVL(1)) == బిట్టు(PT_సూచీ_బిట్లు) - 1);

BOOT_సంకేతం శూన్య map_kernel_physical_కిటికీ(శూన్య)
{
    paddr_t paddr;
    pptr_t vaddr;
    మాట_t idx;

    /* place all PDs except the last one in PUD */
    కొరకు (idx = GET_KPT_సూచీ(PPTR_పునాది, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_సూచీ(PPTR_శిఖరం, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_పట్టిక_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_పునాది;
    కొరకు (paddr = PADDR_పునాది; paddr < PADDR_శిఖరం; paddr += బిట్టు(se_L4_Large_పుట_బిట్లు)) {
        armKSGlobalKernelPDs[GET_KPT_సూచీ(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_సూచీ(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_పుట_new(
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#లేకపోతే
                                                                                                                        1, // UXN
#అయితే_ముగింపు
                                                                                                                        paddr,
                                                                                                                        0,                        /* global */
                                                                                                                        1,                        /* access flag */
                                                                                                                        SMP_TERNARY(SMP_SHARE, 0),        /* Inner-shareable if SMP enabled, otherwise unshared */
                                                                                                                        0,                        /* VMKernelOnly */
                                                                                                                        సాధారణ
                                                                                                                    );
        vaddr += బిట్టు(se_L4_Large_పుట_బిట్లు);
    }
}

BOOT_సంకేతం శూన్య map_kernel_కిటికీ(శూన్య)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_సూచీ(PPTR_పునాది, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_పట్టిక_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_కిటికీ();

    స్థిరరాశి మాట_t elf_pud_idx = GET_KPT_సూచీ(KERNEL_ELF_పునాది, KLVL_FRM_ARM_PT_LVL(1));
    pte_t *kernel_mapping_pds = armKSGlobalKernelPDs[elf_pud_idx];

    /* place the kernel mapping PDs in the kernel mapping PUD */
    armKSGlobalKernelPUD[elf_pud_idx] = pte_pte_పట్టిక_new(addrFromKPPtr(kernel_mapping_pds));

    /* map the kernel ELF */
    map_kernel_elf_image(kernel_mapping_pds);

    /* put the PT into the PD for device window. It would be nice if
       we could pass kernel_mapping_pds[511] to map_kernel_devices(), but it is
       shared between AArch64 and AArch32, so... */
    kernel_mapping_pds[బిట్టు(PT_సూచీ_బిట్లు) - 1]
        = pte_pte_పట్టిక_new(addrFromKPPtr(armKSGlobalKernelPT));

    map_kernel_devices();
}

/* When the hypervisor support is enabled, the stage-2 translation table format
 * is used for applications.
 * The global bit is always 0.
 * The memory attributes use the S2 translation values.
 */
స్థిర BOOT_సంకేతం శూన్య map_it_చట్రం_cap(cap_t vspace_cap, cap_t చట్రం_cap, bool_t executable)
{
    vspace_మూలం_t *vspace_మూలం_2 = VSPACE_PTR(pptr_యొక్క_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_చట్రం_cap_get_cap_FMapped_చిరునామా(చట్రం_cap);
    శూన్య *pptr = (శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(చట్రం_cap);

    ప్రతిజ్ఞా(cap_చట్రం_cap_get_cap_FMapped_ASID(చట్రం_cap) != 0);

#నిర్వచితమైతే AARCH64_VSPACE_S2_ప్రారంభం_L1
    pud = vspace_మూలం_2;
#లేకపోతే
    vspace_మూలం_2 += GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ప్రతిజ్ఞా(pte_pte_పట్టిక_ptr_get_present(vspace_మూలం_2));
    pud = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(vspace_మూలం_2));
#అయితే_ముగింపు
    pud += GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(1));
    ప్రతిజ్ఞా(pte_pte_పట్టిక_ptr_get_present(pud));
    pd = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pud));
    pd += GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(2));
    ప్రతిజ్ఞా(pte_pte_పట్టిక_ptr_get_present(pd));
    pt = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pd));
    *(pt + GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_పుట_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_గమ్యం_paddr(pptr),            /* page_base_address    */
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#లేకపోతే
                                                              1,                              /* not global */
#అయితే_ముగింపు
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_సాధారణ
#లేకపోతే
                                                              సాధారణ
#అయితే_ముగింపు
                                                          );
}

స్థిర BOOT_సంకేతం cap_t create_it_చట్రం_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_పుట_కొలత_t చట్రం_కొలత_2;
    ఒకవేళ (use_large) {
        చట్రం_కొలత_2 = ARMLarge_పుట;
    } లేకపోతే {
        చట్రం_కొలత_2 = ARMSmall_పుట;
    }
    తిరిగి
        cap_చట్రం_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            చట్రం_కొలత_2,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            మాట_From_VMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

స్థిర BOOT_సంకేతం శూన్య map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_మూలం_t *vspace_మూలం_2 = VSPACE_PTR(pptr_యొక్క_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(pt_cap);

    ప్రతిజ్ఞా(cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(pt_cap));

#నిర్వచితమైతే AARCH64_VSPACE_S2_ప్రారంభం_L1
    pud = vspace_మూలం_2;
#లేకపోతే
    vspace_మూలం_2 += GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ప్రతిజ్ఞా(pte_pte_పట్టిక_ptr_get_present(vspace_మూలం_2));
    pud = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(vspace_మూలం_2));
#అయితే_ముగింపు
    pud += GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(1));
    ప్రతిజ్ఞా(pte_pte_పట్టిక_ptr_get_present(pud));
    pd = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pud));
    *(pd + GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_పట్టిక_new(
                                                              pptr_గమ్యం_paddr(pt)
                                                          );
}

స్థిర BOOT_సంకేతం cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_పుట_పట్టిక_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    తిరిగి cap;
}

స్థిర BOOT_సంకేతం శూన్య map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_మూలం_t *vspace_మూలం_2 = VSPACE_PTR(pptr_యొక్క_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(pd_cap));
    vptr_t vptr = cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(pd_cap);

    ప్రతిజ్ఞా(cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(pd_cap));

#నిర్వచితమైతే AARCH64_VSPACE_S2_ప్రారంభం_L1
    pud = vspace_మూలం_2;
#లేకపోతే
    vspace_మూలం_2 += GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ప్రతిజ్ఞా(pte_pte_పట్టిక_ptr_get_present(vspace_మూలం_2));
    pud = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(vspace_మూలం_2));
#అయితే_ముగింపు
    *(pud + GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_పట్టిక_new(
                                                               pptr_గమ్యం_paddr(pd)
                                                           );
}

స్థిర BOOT_సంకేతం cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_పుట_పట్టిక_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    తిరిగి cap;
}

#నిర్వచితం_కాకపోతే AARCH64_VSPACE_S2_ప్రారంభం_L1
స్థిర BOOT_సంకేతం శూన్య map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_యొక్క_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(pud_cap));
    vptr_t vptr = cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(pud_cap);

    ప్రతిజ్ఞా(cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(pud_cap));

    *(pgd + GET_UPT_సూచీ(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_పట్టిక_new(
                                                               pptr_గమ్యం_paddr(pud));
}

స్థిర BOOT_సంకేతం cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_పుట_పట్టిక_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    తిరిగి cap;
}
#అయితే_ముగింపు /* AARCH64_VSPACE_S2_START_L1 */
BOOT_సంకేతం మాట_t arch_get_n_paging(v_region_t it_v_reg)
{
    తిరిగి
#నిర్వచితం_కాకపోతే AARCH64_VSPACE_S2_ప్రారంభం_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_బిట్లు(ULVL_FRM_ARM_PT_LVL(0))) +
#అయితే_ముగింపు
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_బిట్లు(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_బిట్లు(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_సంకేతం cap_t create_it_చిరునామా_space(cap_t మూలం_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_స్థానం_Pos స్థానం_pos_before;
    se_L4_స్థానం_Pos స్థానం_pos_after;

    /* create the PGD */
    vspace_cap = cap_vspace_cap_new(
                     IT_ASID,           /* capVSMappedASID */
                     rootserver.vspace, /* capVSBasePtr    */
                     1                  /* capVSIsMapped   */
#నిర్వచితమైతే CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#అయితే_ముగింపు
                 );
    స్థానం_pos_before = ndks_boot.స్థానం_pos_cur;
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#నిర్వచితం_కాకపోతే AARCH64_VSPACE_S2_ప్రారంభం_L1
    /* Create any PUDs needed for the user land image */
    కొరకు (vptr = విడత_కిందికి(it_v_reg.ప్రారంభం, GET_ULVL_PGSIZE_బిట్లు(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.చివర;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        ఒకవేళ (!provide_cap(మూలం_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            తిరిగి cap_null_cap_new();
        }
    }
#అయితే_ముగింపు
    /* Create any PDs needed for the user land image */
    కొరకు (vptr = విడత_కిందికి(it_v_reg.ప్రారంభం, GET_ULVL_PGSIZE_బిట్లు(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.చివర;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        ఒకవేళ (!provide_cap(మూలం_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            తిరిగి cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    కొరకు (vptr = విడత_కిందికి(it_v_reg.ప్రారంభం, GET_ULVL_PGSIZE_బిట్లు(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.చివర;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        ఒకవేళ (!provide_cap(మూలం_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            తిరిగి cap_null_cap_new();
        }
    }

    స్థానం_pos_after = ndks_boot.స్థానం_pos_cur;
    ndks_boot.bi_చట్రం->userImagePaging = (se_L4_స్థానం_Region) {
        స్థానం_pos_before, స్థానం_pos_after
    };
    తిరిగి vspace_cap;
}

BOOT_సంకేతం cap_t create_unmapped_it_చట్రం_cap(pptr_t pptr, bool_t use_large)
{
    తిరిగి create_it_చట్రం_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_సంకేతం cap_t create_mapped_it_చట్రం_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_చట్రం_cap(pptr, vptr, asid, use_large);
    map_it_చట్రం_cap(pd_cap, cap, executable);
    తిరిగి cap;
}

BOOT_సంకేతం శూన్య activate_kernel_vspace(శూన్య)
{
    cleanInvalidateL1Caches();
    సమితి_Current_Kernel_VSpace_మూలం(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    సమితి_Current_User_VSpace_మూలం(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    తాళం_TLBEntry(KERNEL_ELF_పునాది);
}

BOOT_సంకేతం శూన్య write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_యొక్క_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#నిర్వచితమైతే CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#అయితే_ముగింపు
                              /* vspace_root: reference to vspace root page table object */
                              (మాట_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#అయితే_ముగింపు
                          );
    ap->array[ASID_దిగువ(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_ఎగువ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t కనుగొను_Map_For_ASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ఎగువ(asid)];
    ఒకవేళ (!poolPtr) {
        తిరిగి asid_map_asid_map_none_new();
    }

    తిరిగి poolPtr->array[ASID_దిగువ(asid)];
}

స్థిర కనుగొను_VSpace_For_ASID_ret_t కనుగొను_VSpace_For_ASID(asid_t asid)
{
    కనుగొను_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_మూలం_new();

        ret.vspace_మూలం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    ret.vspace_మూలం = (vspace_మూలం_t *)asid_map_asid_map_vspace_get_vspace_మూలం(asid_map);
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

మాట_t *శుద్ధ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    మాట_t w_బఫరు_Ptr;
    cap_t బఫరు_Cap;
    vm_rights_t vm_rights;

    w_బఫరు_Ptr = thread->tcbIPCBuffer;
    బఫరు_Cap = TCB_PTR_CTE_PTR(thread, tcb_బఫరు)->cap;

    ఒకవేళ (unlikely(cap_get_capType(బఫరు_Cap) != cap_చట్రం_cap)) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (unlikely(cap_చట్రం_cap_get_cap_FIs_Device(బఫరు_Cap))) {
        తిరిగి ఖాళీ;
    }

    vm_rights = cap_చట్రం_cap_get_cap_FVMRights(బఫరు_Cap);
    ఒకవేళ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        మాట_t పునాది_Ptr;
        గుర్తులేని పూర్ణాంకం పుట_బిట్లు_2;

        పునాది_Ptr = cap_చట్రం_cap_get_cap_FBase_Ptr(బఫరు_Cap);
        పుట_బిట్లు_2 = పుట_బిట్లు_For_కొలత(cap_చట్రం_cap_get_cap_FSize(బఫరు_Cap));
        తిరిగి (మాట_t *)(పునాది_Ptr + (w_బఫరు_Ptr & ముసుగు(పుట_బిట్లు_2)));
    } లేకపోతే {
        తిరిగి ఖాళీ;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ఒకవేళ (cap_get_capType(cap) != cap_చట్రం_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(cap_చట్రం_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (!IS_ALIGNED(vptr, se_L4_IPCBuffer_కొలత_బిట్లు)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (పూర్ణాంకం)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర lookupPTSlot_ret_t lookupPTSlot(vspace_మూలం_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    మాట_t level = UPT_స్థాయిలు - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_బిట్లు_ఎడమ = PT_సూచీ_బిట్లు * level + se_L4_పుట_బిట్లు;
    ret.pt_స్థానం = pt + ((vptr >> ret.pt_బిట్లు_ఎడమ) & ముసుగు(se_L4_VSpace_సూచీ_బిట్లు));

    అయితే (pte_pte_పట్టిక_ptr_get_present(ret.pt_స్థానం) && likely(level > 0)) {
        level--;
        ret.pt_బిట్లు_ఎడమ -= PT_సూచీ_బిట్లు;
        pt = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(ret.pt_స్థానం));
        ret.pt_స్థానం = pt + ((vptr >> ret.pt_బిట్లు_ఎడమ) & ముసుగు(PT_సూచీ_బిట్లు));
    }

    తిరిగి ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
స్థిర pte_t make_User_పుట_PTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_లక్షణాలు_t లక్షణాలు,
                             vm_పుట_కొలత_t పుట_కొలత)
{
    bool_t nonexecutable = vm_లక్షణాలు_get_arm_Execute_Never(లక్షణాలు) || !cap_read;
    మాట_t cacheable = vm_లక్షణాలు_get_arm_పుట_Cacheable(లక్షణాలు);

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    మాట_t nG = 0; /* not global */
    మాట_t attridx = cacheable ? S2_సాధారణ : S2_DEVICE_nGnRnE;
#లేకపోతే
    మాట_t nG = 1; /* not global */
    మాట_t attridx = cacheable ? సాధారణ : DEVICE_nGnRnE;
#అయితే_ముగింపు

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    మాట_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    ఒకవేళ (పుట_కొలత == ARMSmall_పుట) {
        తిరిగి pte_pte_4k_పుట_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } లేకపోతే {
        తిరిగి pte_pte_పుట_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    ఎంపిక (vm_faultType) {
    స్థితి ARMDataAbort: {
        మాట_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        ఒకవేళ (ARCH_కణుపు_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(చిరునామా_Translate_S1(addr)) | (addr & ముసుగు(పుట_బిట్లు));
        }
#అయితే_ముగింపు
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        తిరిగి EXCEPTION_FAULT;
    }

    స్థితి ARMPrefetchAbort: {
        మాట_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        ఒకవేళ (ARCH_కణుపు_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(చిరునామా_Translate_S1(pc)) | (pc & ముసుగు(పుట_బిట్లు));
        }
#అయితే_ముగింపు
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        తిరిగి EXCEPTION_FAULT;
    }

    అప్రమేయ:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_మూలం(cap_t cap)
{
    తిరిగి cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_మూలం(cap_t cap)
{
    తిరిగి is_VTable_మూలం(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_మూలం(cap_t cap)
{
    తిరిగి is_Valid_Native_మూలం(cap);
}

శూన్య సమితి_VMRoot(tcb_t *tcb)
{
    cap_t thread_మూలం;
    asid_t asid;
    vspace_మూలం_t *vspace_మూలం_2;
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;

    thread_మూలం = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    ఒకవేళ (!is_Valid_Native_మూలం(thread_మూలం)) {
        సమితి_Current_User_VSpace_మూలం(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        తిరిగి;
    }

    vspace_మూలం_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_మూలం));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_మూలం);
    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE || కనుగొను_ret.vspace_మూలం != vspace_మూలం_2)) {
        సమితి_Current_User_VSpace_మూలం(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        తిరిగి;
    }

    armv_contextSwitch(vspace_మూలం_2, asid);
}

స్థిర bool_t సమితి_VMRoot_For_Flush(vspace_మూలం_t *vspace, asid_t asid)
{
    cap_t thread_మూలం;

    thread_మూలం = TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbVTable)->cap;

    ఒకవేళ (cap_get_capType(thread_మూలం) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_మూలం) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_మూలం)) == vspace) {
        తిరిగి false;
    }

    armv_contextSwitch(vspace, asid);
    తిరిగి true;
}


#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT

స్థిర అంతఃవరుస asid_pool_t *getPoolPtr(asid_t asid)
{
    తిరిగి armKSASIDTable[ASID_ఎగువ(asid)];
}

స్థిర అంతఃవరుస asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    ప్రతిజ్ఞా(poolPtr != ఖాళీ);
    తిరిగి poolPtr->array[ASID_దిగువ(asid)];
}

స్థిర అంతఃవరుస శూన్య సమితి_ASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    ప్రతిజ్ఞా(poolPtr != ఖాళీ);
    poolPtr->array[ASID_దిగువ(asid)] = asid_map;
}

స్థిర శూన్య invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    ప్రతిజ్ఞా(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_సమితి_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_సమితి_stored_vmid_valid(asid_map, false);

    సమితి_ASIDMap(poolPtr, asid, asid_map);
}

స్థిర శూన్య storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    ప్రతిజ్ఞా(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_సమితి_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_సమితి_stored_vmid_valid(asid_map, true);

    సమితి_ASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

స్థిర hw_asid_t కనుగొను_Free_HWASID(శూన్య)
{
    మాట_t hw_asid_స్థానభ్రంశం;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    కొరకు (hw_asid_స్థానభ్రంశం = 0;
         hw_asid_స్థానభ్రంశం <= (మాట_t)((hw_asid_t) - 1);
         hw_asid_స్థానభ్రంశం++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_స్థానభ్రంశం);
        ఒకవేళ (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            తిరిగి hw_asid;
        }
    }
    /* If we've scanned the table without finding a free ASID */
    hw_asid = armKSNextASID;
    invalidateASID(armKSHWASIDTable[hw_asid]);

    /* Flush TLB */
    invalidateTranslationASID(hw_asid);
    armKSHWASIDTable[hw_asid] = asidInvalid;

    /* Increment the NextASID index, skipping the reserved VMID 0 on wrap */
    armKSNextASID++;
    ఒకవేళ (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    తిరిగి hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        తిరిగి asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } లేకపోతే {
        hw_asid_t new_hw_asid;

        new_hw_asid = కనుగొను_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        తిరిగి new_hw_asid;
    }
}

స్థిర శూన్య invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARM_SMMU
స్థిర మాట_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ఎగువ(asid)];
    ప్రతిజ్ఞా(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_దిగువ(asid)];
    ప్రతిజ్ఞా(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    తిరిగి asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

శూన్య increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ఎగువ(asid)];
    ప్రతిజ్ఞా(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_దిగువ(asid)];
    ప్రతిజ్ఞా(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_సమితి_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

శూన్య decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ఎగువ(asid)];
    ప్రతిజ్ఞా(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_దిగువ(asid)];
    ప్రతిజ్ఞా(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_సమితి_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#అయితే_ముగింపు

స్థిర అంతఃవరుస శూన్య invalidateTLBByASID(asid_t asid)
{
#నిర్వచితమైతే CONFIG_ARM_SMMU
    మాట_t bind_cb = getASIDBindCB(asid);
    ఒకవేళ (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        తిరిగి;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#లేకపోతే
    invalidateTranslationASID(asid);
#అయితే_ముగింపు
}

స్థిర అంతఃవరుస శూన్య invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#నిర్వచితమైతే CONFIG_ARM_SMMU
    మాట_t bind_cb = getASIDBindCB(asid);
    ఒకవేళ (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        తిరిగి;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> se_L4_పుట_బిట్లు);
#లేకపోతే
    invalidateTranslationSingle((asid << 48) | vaddr >> se_L4_పుట_బిట్లు);
#అయితే_ముగింపు
}


శూన్య unmap_పుట_పట్టిక(asid_t asid, vptr_t vptr, pte_t *లక్ష్యం_pt)
{
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
        /* nothing to do */
        తిరిగి;
    }
    pte_t *pt_స్థానం = ఖాళీ;
    pte_t *pt = (pte_t *)కనుగొను_ret.vspace_మూలం;

    కొరకు (మాట_t i = 0; i < UPT_స్థాయిలు - 1 && pt != లక్ష్యం_pt; i++) {
        pt_స్థానం = pt + GET_UPT_సూచీ(vptr, i);
        ఒకవేళ (unlikely(!pte_pte_పట్టిక_ptr_get_present(pt_స్థానం))) {
            /* couldn't find it */
            తిరిగి;
        }
        pt = paddr_గమ్యం_pptr(pte_pte_పట్టిక_ptr_get_pt_పునాది_చిరునామా(pt_స్థానం));
    }

    ఒకవేళ (pt != లక్ష్యం_pt) {
        /* didn't find it */
        తిరిగి;
    }
    /* If we found a pt then ptSlot won't be null */
    ప్రతిజ్ఞా(pt_స్థానం != ఖాళీ);
    *pt_స్థానం = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)pt_స్థానం, pptr_గమ్యం_paddr(pt_స్థానం));
    invalidateTLBByASID(asid);
}

శూన్య unmap_పుట(vm_పుట_కొలత_t పుట_కొలత, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి;
    }

    lu_ret = lookupPTSlot(కనుగొను_ret.vspace_మూలం, vptr);
    ఒకవేళ (unlikely(lu_ret.pt_బిట్లు_ఎడమ != పుట_బిట్లు_For_కొలత(పుట_కొలత))) {
        /* Do nothing if the wrong size object was returned */
        తిరిగి;
    }

    pte = *(lu_ret.pt_స్థానం);
    ఒకవేళ (!pte_is_పుట_type(pte)) {
        /* Do nothing if no page is present */
        తిరిగి;
    }

    ఒకవేళ (pte_get_పుట_పునాది_చిరునామా(pte) != pptr_గమ్యం_paddr((శూన్య *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        తిరిగి;
    }

    *(lu_ret.pt_స్థానం) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.pt_స్థానం, pptr_గమ్యం_paddr(lu_ret.pt_స్థానం));
    ప్రతిజ్ఞా(asid < బిట్టు(16));
    invalidateTLBByASIDVA(asid, vptr);
}

శూన్య deleteASID(asid_t asid, vspace_మూలం_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ఎగువ(asid)];

    ఒకవేళ (poolPtr != ఖాళీ) {
        asid_map_t asid_map = poolPtr->array[ASID_దిగువ(asid)];
        ఒకవేళ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_మూలం_t *)asid_map_asid_map_vspace_get_vspace_మూలం(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#అయితే_ముగింపు
            poolPtr->array[ASID_దిగువ(asid)] = asid_map_asid_map_none_new();
            సమితి_VMRoot(కణుపు_STATE(ksCurThread));
        }
    }
}

శూన్య deleteASIDPool(asid_t asid_పునాది, asid_pool_t *pool)
{
    మాట_t స్థానభ్రంశం;

    ప్రతిజ్ఞా((asid_పునాది & ముసుగు(asid_దిగువ_బిట్లు)) == 0);

    ఒకవేళ (armKSASIDTable[ASID_ఎగువ(asid_పునాది)] == pool) {
        కొరకు (స్థానభ్రంశం = 0; స్థానభ్రంశం < బిట్టు(asid_దిగువ_బిట్లు); స్థానభ్రంశం++) {
            asid_map_t asid_map = pool->array[స్థానభ్రంశం];
            ఒకవేళ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_పునాది + స్థానభ్రంశం);
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_పునాది + స్థానభ్రంశం);
#అయితే_ముగింపు
            }
        }
        armKSASIDTable[ASID_ఎగువ(asid_పునాది)] = ఖాళీ;
        సమితి_VMRoot(కణుపు_STATE(ksCurThread));
    }
}

స్థిర శూన్య doFlush(మాట_t invLabel, vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{
    ఎంపిక (invLabel) {
    స్థితి ARMVSpace_Clean_దత్తాంశం:
    స్థితి ARMPage_Clean_దత్తాంశం:
        cleanCacheRange_RAM(ప్రారంభం, చివర, pstart);
        ఆపు;

    స్థితి ARMVSpace_Invalidate_దత్తాంశం:
    స్థితి ARMPage_Invalidate_దత్తాంశం:
        invalidateCacheRange_RAM(ప్రారంభం, చివర, pstart);
        ఆపు;

    స్థితి ARMVSpace_Clean_Invalidate_దత్తాంశం:
    స్థితి ARMPage_Clean_Invalidate_దత్తాంశం:
        cleanInvalidateCacheRange_RAM(ప్రారంభం, చివర, pstart);
        ఆపు;

    స్థితి ARMVSpaceUnify_Instruction:
    స్థితి ARMPageUnify_Instruction:
        /* First clean data lines to point of unification... */
        cleanCacheRange_PoU(ప్రారంభం, చివర, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(ప్రారంభం, చివర, pstart);
        /* ... then wait for the completion of invalidations... */
        dsb();
        /* ... and ensure new instructions come from fresh cache lines. */
        isb();
        ఆపు;
    అప్రమేయ:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

స్థిర exception_t performVSpaceFlush(మాట_t invLabel, vspace_మూలం_t *vspace_మూలం_2, asid_t asid,
                                      vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{

    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        మాట_t కొలత = చివర - ప్రారంభం;
        ప్రారంభం = (vptr_t)paddr_గమ్యం_pptr(pstart);
        చివర = ప్రారంభం + కొలత;
        ఒకవేళ (ప్రారంభం < చివర) {
            doFlush(invLabel, ప్రారంభం, చివర, pstart);
        }
    } లేకపోతే {
        bool_t మూలం_switched;

        /* Flush if given a non zero range */
        ఒకవేళ (ప్రారంభం < చివర) {
            మూలం_switched = సమితి_VMRoot_For_Flush(vspace_మూలం_2, asid);
            doFlush(invLabel, ప్రారంభం, చివర, pstart);
            ఒకవేళ (మూలం_switched) {
                సమితి_VMRoot(కణుపు_STATE(ksCurThread));
            }
        }
    }
    తిరిగి EXCEPTION_NONE;
}


స్థిర exception_t perform_పుట_పట్టిక_Invocation_Map(cap_t cap, cte_t *ct_స్థానం, pte_t pte, pte_t *pt_స్థానం)
{
    ct_స్థానం->cap = cap;
    *pt_స్థానం = pte;
    cleanByVA_PoU((vptr_t)pt_స్థానం, pptr_గమ్యం_paddr(pt_స్థానం));

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_పట్టిక_Invocation_Unmap(cap_t cap, cte_t *ct_స్థానం)
{
    ఒకవేళ (cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PT_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap));
        unmap_పుట_పట్టిక(cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap),
                       cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap), pt);
        clear_జ్ఞాపకం_PT((శూన్య *)pt, cap_get_cap_కొలత_బిట్లు(cap));
    }

    cap_పుట_పట్టిక_cap_ptr_సమితి_cap_PTIs_Mapped(&(ct_స్థానం->cap), 0);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_Invocation_Map(asid_t asid, cap_t cap, cte_t *ct_స్థానం,
                                            pte_t pte, pte_t *pt_స్థానం)
{
    bool_t tlbflush_required = pte_ptr_get_valid(pt_స్థానం);

    ct_స్థానం->cap = cap;
    *pt_స్థానం = pte;

    cleanByVA_PoU((vptr_t)pt_స్థానం, pptr_గమ్యం_paddr(pt_స్థానం));
    ఒకవేళ (unlikely(tlbflush_required)) {
        ప్రతిజ్ఞా(asid < బిట్టు(16));
        invalidateTLBByASIDVA(asid, cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap));
    }

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_Invocation_Unmap(cap_t cap, cte_t *ct_స్థానం)
{
    ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap) != 0) {

        unmap_పుట(cap_చట్రం_cap_get_cap_FSize(cap),
                  cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                  cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                  cap_చట్రం_cap_get_cap_FBase_Ptr(cap));
    }

    cap_t స్థానం_Cap = ct_స్థానం->cap;
    స్థానం_Cap = cap_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(స్థానం_Cap, 0);
    స్థానం_Cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(స్థానం_Cap, asidInvalid);
    ct_స్థానం->cap = స్థానం_Cap;


    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_Flush(మాట_t invLabel, vspace_మూలం_t *vspace_మూలం_2, asid_t asid,
                                    vptr_t ప్రారంభం, vptr_t చివర, paddr_t pstart)
{
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        మాట_t కొలత = చివర - ప్రారంభం;
        ప్రారంభం = (vptr_t)paddr_గమ్యం_pptr(pstart);
        చివర = ప్రారంభం + కొలత;

        ఒకవేళ (ప్రారంభం < చివర) {
            doFlush(invLabel, ప్రారంభం, చివర, pstart);
        }
    } లేకపోతే {
        bool_t మూలం_switched;

        ఒకవేళ (ప్రారంభం < చివర) {
            మూలం_switched = సమితి_VMRoot_For_Flush(vspace_మూలం_2, asid);
            doFlush(invLabel, ప్రారంభం, చివర, pstart);
            ఒకవేళ (మూలం_switched) {
                సమితి_VMRoot(కణుపు_STATE(ksCurThread));
            }
        }
    }
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_పుట_Get_చిరునామా(pptr_t పునాది_ptr, bool_t call)
{
    paddr_t పునాది = pptr_గమ్యం_paddr((శూన్య *)పునాది_ptr);

    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);
    ఒకవేళ (call) {
        మాట_t *ipc_బఫరు = lookupIPCBuffer(true, thread);
        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        గుర్తులేని పూర్ణాంకం నిడివి = సమితి_MR(thread, ipc_బఫరు, 0, పునాది);
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t performASIDControlInvocation(శూన్య *చట్రం, cte_t *స్థానం,
                                                cte_t *మాతృ, asid_t asid_పునాది)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_సమితి_cap_Free_సూచీ(&(మాతృ->cap),
                                         MAX_FREE_సూచీ(cap_untyped_cap_get_cap_ఖండం_కొలత(మాతృ->cap)));

    memzero(చట్రం, బిట్టు(se_L4_ASIDPool_బిట్లు));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_చొప్పించు(
        cap_asid_pool_cap_new(
            asid_పునాది,         /* capASIDBase  */
            మాట_REF(చట్రం)    /* capASIDPool  */
        ), మాతృ, స్థానం);

    ప్రతిజ్ఞా((asid_పునాది & ముసుగు(asid_దిగువ_బిట్లు)) == 0);
    armKSASIDTable[ASID_ఎగువ(asid_పునాది)] = (asid_pool_t *)చట్రం;

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decode_ARMVSpace_మూలం_Invocation(మాట_t invLabel, మాట_t నిడివి,
                                                 cte_t *cte, cap_t cap, మాట_t *బఫరు)
{
    vptr_t ప్రారంభం, చివర;
    paddr_t pstart;
    asid_t asid;
    vspace_మూలం_t *vspace_మూలం_2;
    lookupPTSlot_ret_t resolve_ret;
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;
    pte_t pte;

    ఎంపిక (invLabel) {
    స్థితి ARMVSpace_Clean_దత్తాంశం:
    స్థితి ARMVSpace_Invalidate_దత్తాంశం:
    స్థితి ARMVSpace_Clean_Invalidate_దత్తాంశం:
    స్థితి ARMVSpaceUnify_Instruction:

        ఒకవేళ (నిడివి < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ప్రారంభం = getSyscallArg(0, బఫరు);
        చివర =   getSyscallArg(1, బఫరు);

        /* Check sanity of arguments */
        ఒకవేళ (చివర <= ప్రారంభం) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        ఒకవేళ (చివర > USER_శిఖరం) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(!is_Valid_Native_మూలం(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_మూలం_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(కనుగొను_ret.vspace_మూలం != vspace_మూలం_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_మూలం_2, ప్రారంభం);
        pte = *resolve_ret.pt_స్థానం;

        /* Check that the returned slot is a page. */
        ఒకవేళ (!pte_is_పుట_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
            తిరిగి EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        ఒకవేళ (విడత_కిందికి(ప్రారంభం, resolve_ret.pt_బిట్లు_ఎడమ) != విడత_కిందికి(చివర - 1, resolve_ret.pt_బిట్లు_ఎడమ)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = ప్రారంభం;
            current_syscall_error.rangeErrorMax = విడత_కిందికి(ప్రారంభం, resolve_ret.pt_బిట్లు_ఎడమ) +
                                                  ముసుగు(resolve_ret.pt_బిట్లు_ఎడమ);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

#నిర్వచితం_కాకపోతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        ఒకవేళ (invLabel == ARMVSpace_Invalidate_దత్తాంశం && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#అయితే_ముగింపు

        /* Calculate the physical start address. */
        paddr_t చట్రం_పునాది_2 = pte_get_పుట_పునాది_చిరునామా(pte);

        pstart = చట్రం_పునాది_2 + (ప్రారంభం & ముసుగు(resolve_ret.pt_బిట్లు_ఎడమ));

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        ఒకవేళ (pstart < PADDR_పునాది || ((చివర - ప్రారంభం) + pstart) > PADDR_శిఖరం) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#అయితే_ముగింపు

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performVSpaceFlush(invLabel, vspace_మూలం_2, asid, ప్రారంభం, చివర - 1, pstart);

    అప్రమేయ:
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}


స్థిర exception_t decode_ARMPage_పట్టిక_Invocation(మాట_t invLabel, మాట_t నిడివి,
                                                cte_t *cte, cap_t cap, మాట_t *బఫరు)
{
    cap_t vspace_మూలం_Cap;
    vspace_మూలం_t *vspace_మూలం_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t pt_స్థానం;
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;

    ఒకవేళ (invLabel == ARMPage_పట్టిక_Unmap) {
        ఒకవేళ (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_పట్టిక_Invocation_Unmap(cap, cte);
    }

    ఒకవేళ (unlikely(invLabel != ARMPage_పట్టిక_Map)) {
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ)) {
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, బఫరు);
    vspace_మూలం_Cap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (unlikely(!is_Valid_Native_మూలం(vspace_మూలం_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vspace_మూలం_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_మూలం_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_మూలం_Cap);

    ఒకవేళ (unlikely(vaddr > USER_శిఖరం)) {
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (unlikely(కనుగొను_ret.vspace_మూలం != vspace_మూలం_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pt_స్థానం = lookupPTSlot(vspace_మూలం_2, vaddr);

    ఒకవేళ (unlikely(pt_స్థానం.pt_బిట్లు_ఎడమ == se_L4_పుట_బిట్లు || pte_ptr_get_valid(pt_స్థానం.pt_స్థానం))) {
        current_syscall_error.type = se_L4_Delete_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_పట్టిక_new(pptr_గమ్యం_paddr(PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap))));

    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTIs_Mapped(cap, 1);
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTMapped_ASID(cap, asid);
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTMapped_చిరునామా(cap, (vaddr & ~ముసుగు(pt_స్థానం.pt_బిట్లు_ఎడమ)));

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి perform_పుట_పట్టిక_Invocation_Map(cap, cte, pte, pt_స్థానం.pt_స్థానం);
}

స్థిర అంతఃవరుస bool_t CONST checkVPAlignment(vm_పుట_కొలత_t sz, మాట_t w)
{
    తిరిగి (w & ముసుగు(పుట_బిట్లు_For_కొలత(sz))) == 0;
}

స్థిర exception_t decodeARMFrameInvocation(మాట_t invLabel, మాట_t నిడివి,
                                            cte_t *cte, cap_t cap, bool_t call, మాట_t *బఫరు)
{
    ఎంపిక (invLabel) {
    స్థితి ARMPageMap: {
        vptr_t vaddr;
        paddr_t పునాది;
        cap_t vspace_మూలం_Cap;
        vspace_మూలం_t *vspace_మూలం_2;
        asid_t asid, చట్రం_asid;
        vm_rights_t vmRights, capRights;
        vm_పుట_కొలత_t చట్రం_కొలత;
        vm_లక్షణాలు_t లక్షణాలు;
        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;

        ఒకవేళ (unlikely(నిడివి < 3 || current_extra_caps.excaprefs[0] == ఖాళీ)) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, బఫరు);
        లక్షణాలు = vm_లక్షణాలు_From_మాట(getSyscallArg(2, బఫరు));
        vspace_మూలం_Cap = current_extra_caps.excaprefs[0]->cap;

        చట్రం_కొలత = cap_చట్రం_cap_get_cap_FSize(cap);
        capRights = cap_చట్రం_cap_get_cap_FVMRights(cap);
        vmRights = ముసుగు_VMRights(capRights, rights_From_మాట(getSyscallArg(1, బఫరు)));

        ఒకవేళ (unlikely(!is_Valid_Native_మూలం(vspace_మూలం_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        vspace_మూలం_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_మూలం_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_మూలం_Cap);

        కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(కనుగొను_ret.vspace_మూలం != vspace_మూలం_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(!checkVPAlignment(చట్రం_కొలత, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        చట్రం_asid = cap_చట్రం_cap_get_cap_FMapped_ASID(cap);

        ఒకవేళ (చట్రం_asid != asidInvalid) {
            ఒకవేళ (చట్రం_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_సంఖ్య = 1;
                తిరిగి EXCEPTION_SYSCALL_ERROR;

            } లేకపోతే ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_వాదం;
                current_syscall_error.invalid_వాదం_సంఖ్య = 0;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        } లేకపోతే {
            ఒకవేళ (unlikely(vaddr + బిట్టు(పుట_బిట్లు_For_కొలత(చట్రం_కొలత)) - 1 > USER_శిఖరం)) {
                current_syscall_error.type = se_L4_Invalid_వాదం;
                current_syscall_error.invalid_వాదం_సంఖ్య = 0;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, asid);
        cap = cap_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(cap, vaddr);

        పునాది = pptr_గమ్యం_paddr((శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_మూలం_2, vaddr);
        ఒకవేళ (unlikely(lu_ret.pt_బిట్లు_ఎడమ != పుట_బిట్లు_For_కొలత(చట్రం_కొలత))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_బిట్లు_ఎడమ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = make_User_పుట_PTE(పునాది, vmRights, cap_read, లక్షణాలు, చట్రం_కొలత);

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_Invocation_Map(asid, cap, cte, pte, lu_ret.pt_స్థానం);
    }

    స్థితి ARMPageUnmap:
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_Invocation_Unmap(cap, cte);

    స్థితి ARMPage_Clean_దత్తాంశం:
    స్థితి ARMPage_Invalidate_దత్తాంశం:
    స్థితి ARMPage_Clean_Invalidate_దత్తాంశం:
    స్థితి ARMPageUnify_Instruction: {
        vptr_t ప్రారంభం, చివర;
        vptr_t vaddr;
        asid_t asid;
        మాట_t పుట_కొలత;
        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;

        ఒకవేళ (నిడివి < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(cap_చట్రం_cap_get_cap_FMapped_ASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap);
        asid = cap_చట్రం_cap_get_cap_FMapped_ASID(cap);

        కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ప్రారంభం = getSyscallArg(0, బఫరు);
        చివర =   getSyscallArg(1, బఫరు);

        /* check that the range is sane */
        ఒకవేళ (చివర <= ప్రారంభం) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        పుట_కొలత = బిట్టు(పుట_బిట్లు_For_కొలత(cap_చట్రం_cap_get_cap_FSize(cap)));
        ఒకవేళ (ప్రారంభం >= పుట_కొలత || చివర > పుట_కొలత) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        మాట_t pstart = pptr_గమ్యం_paddr((శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap)) + ప్రారంభం;
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        ఒకవేళ (pstart < PADDR_పునాది || ((చివర - ప్రారంభం) + pstart) > PADDR_శిఖరం) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#లేకపోతే
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(కనుగొను_ret.vspace_మూలం, vaddr);
        pte_t pte = *lu_ret.pt_స్థానం;
        శూన్య *పునాది_ptr = (శూన్య *) cap_చట్రం_cap_get_cap_FBase_Ptr(cap);
        ఒకవేళ (unlikely(lu_ret.pt_బిట్లు_ఎడమ != పుట_బిట్లు_For_కొలత(cap_చట్రం_cap_get_cap_FSize(cap)) ||
                     !pte_is_పుట_type(pte) ||
                     pte_get_పుట_పునాది_చిరునామా(pte) != pptr_గమ్యం_paddr(పునాది_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        ఒకవేళ (invLabel == ARMPage_Invalidate_దత్తాంశం && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
#అయితే_ముగింపు
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_Flush(invLabel, కనుగొను_ret.vspace_మూలం, asid, vaddr + ప్రారంభం, vaddr + చివర - 1,
                                pstart);
    }

    స్థితి ARMPage_Get_చిరునామా:
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_Get_చిరునామా(cap_చట్రం_cap_get_cap_FBase_Ptr(cap), call);

    అప్రమేయ:
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(మాట_t invLabel, మాట_t నిడివి, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, మాట_t *బఫరు)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_vspace_cap:
        తిరిగి decode_ARMVSpace_మూలం_Invocation(invLabel, నిడివి, cte, cap, బఫరు);
    స్థితి cap_పుట_పట్టిక_cap:
        తిరిగి decode_ARMPage_పట్టిక_Invocation(invLabel, నిడివి, cte, cap, బఫరు);

    స్థితి cap_చట్రం_cap:
        తిరిగి decodeARMFrameInvocation(invLabel, నిడివి, cte, cap, call, బఫరు);

    స్థితి cap_asid_control_cap: {
        మాట_t i;
        asid_t asid_పునాది;
        మాట_t సూచీ, లోతు;
        cap_t untyped, మూలం;
        cte_t *మాతృ_స్థానం, *dest_స్థానం;
        lookup_స్థానం_ret_t lu_ret;
        శూన్య *చట్రం;
        exception_t స్థితి_2;

        ఒకవేళ (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(నిడివి < 2 ||
                     current_extra_caps.excaprefs[0] == ఖాళీ ||
                     current_extra_caps.excaprefs[1] == ఖాళీ)) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సూచీ = getSyscallArg(0, బఫరు);
        లోతు = getSyscallArg(1, బఫరు);
        మాతృ_స్థానం = current_extra_caps.excaprefs[0];
        untyped = మాతృ_స్థానం->cap;
        మూలం = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        కొరకు (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        ఒకవేళ (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_మొదటి;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        asid_పునాది = i << asid_దిగువ_బిట్లు;

        ఒకవేళ (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_ఖండం_కొలత(untyped) != se_L4_ASIDPool_బిట్లు ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        స్థితి_2 = ensureNoChildren(మాతృ_స్థానం);
        ఒకవేళ (unlikely(స్థితి_2 != EXCEPTION_NONE)) {
            తిరిగి స్థితి_2;
        }

        చట్రం = మాట_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_లక్ష్యం_స్థానం(మూలం, సూచీ, లోతు);
        ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
            తిరిగి lu_ret.స్థితి_2;
        }
        dest_స్థానం = lu_ret.స్థానం;

        స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (unlikely(స్థితి_2 != EXCEPTION_NONE)) {
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performASIDControlInvocation(చట్రం, dest_స్థానం, మాతృ_స్థానం, asid_పునాది);
    }

    స్థితి cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspace_Cap_స్థానం;
        asid_pool_t *pool;
        మాట_t i;
        asid_t asid;

        ఒకవేళ (unlikely(invLabel != ARMASIDPool_కేటాయించు)) {
            current_syscall_error.type = seL4_IllegalOperation;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(current_extra_caps.excaprefs[0] == ఖాళీ)) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_స్థానం = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_స్థానం->cap;

        ఒకవేళ (unlikely(!is_VTable_మూలం(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ఎగువ(cap_asid_pool_cap_get_capASIDBase(cap))];

        ఒకవేళ (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            current_lookup_fault = lookup_fault_invalid_మూలం_new();

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        కొరకు (i = 0; i < (1 << asid_దిగువ_బిట్లు) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        ఒకవేళ (unlikely(i == 1 << asid_దిగువ_బిట్లు)) {
            current_syscall_error.type = se_L4_Delete_మొదటి;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performASIDPoolInvocation(asid, pool, vspace_Cap_స్థానం);
    }

    అప్రమేయ:
        fail("Invalid ARM arch cap type");
    }
}

#నిర్వచితమైతే CONFIG_DEBUG_BUILD
శూన్య kernelPrefetchAbort(మాట_t pc) VISIBLE;
శూన్య kernel_దత్తాంశం_Abort(మాట_t pc) VISIBLE;

శూన్య kernelPrefetchAbort(మాట_t pc)
{
    రాయి("\n\nKERNEL PREFETCH ABORT!\n");
    రాయి("Faulting instruction: 0x%"SEL4_PRIx_మాట"\n", pc);
    రాయి("ESR (IFSR): 0x%"SEL4_PRIx_మాట"\n", getIFSR());
    halt();
}

శూన్య kernel_దత్తాంశం_Abort(మాట_t pc)
{
    రాయి("\n\nKERNEL DATA ABORT!\n");
    రాయి("Faulting instruction: 0x%"SEL4_PRIx_మాట"\n", pc);
    రాయి("FAR: 0x%"SEL4_PRIx_మాట" ESR (DFSR): 0x%"SEL4_PRIx_మాట"\n",
           getFAR(), getDFSR());
    halt();
}
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */

#నిర్వచితమైతే CONFIG_PRINTING
రకనిర్వచనం నిర్మాణం read_మాట_From_VSpace_ret {
    exception_t స్థితి_2;
    మాట_t విలువ;
} read_మాట_From_VSpace_ret_t;

స్థిర read_మాట_From_VSpace_ret_t read_మాట_From_VSpace(vspace_మూలం_t *pd, మాట_t vaddr)
{
    read_మాట_From_VSpace_ret_t ret;
    మాట_t స్థానభ్రంశం;
    pptr_t kernel_vaddr;
    మాట_t *విలువ;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    ఒకవేళ (!pte_ptr_get_valid(lookup_ret.pt_స్థానం) ||
        (pte_pte_పట్టిక_ptr_get_present(lookup_ret.pt_స్థానం) && lookup_ret.pt_బిట్లు_ఎడమ > పుట_బిట్లు)) {
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    స్థానభ్రంశం = vaddr & ముసుగు(lookup_ret.pt_బిట్లు_ఎడమ);
    kernel_vaddr = (మాట_t)paddr_గమ్యం_pptr(pte_పుట_ptr_get_పుట_పునాది_చిరునామా(lookup_ret.pt_స్థానం));
    విలువ = (మాట_t *)(kernel_vaddr + స్థానభ్రంశం);

    ret.స్థితి_2 = EXCEPTION_NONE;
    ret.విలువ = *విలువ;
    తిరిగి ret;
}

శూన్య Arch_user_దొంతర_Trace(tcb_t *tptr)
{
    cap_t thread_మూలం;
    vspace_మూలం_t *vspace_మూలం_2;
    మాట_t sp;

    thread_మూలం = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    ఒకవేళ (cap_get_capType(thread_మూలం) != cap_vspace_cap) {
        రాయి("Invalid vspace\n");
        తిరిగి;
    }

    vspace_మూలం_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_మూలం));
    sp = get_రిజిస్టరు(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ఒకవేళ (!IS_ALIGNED(sp, se_L4_మాట_కొలత_బిట్లు)) {
        రాయి("SP not aligned\n");
        తిరిగి;
    }

    కొరకు (గుర్తులేని పూర్ణాంకం i = 0; i < CONFIG_USER_దొంతర_TRACE_నిడివి; i++) {
        మాట_t చిరునామా = sp + (i * పరిమాణం(మాట_t));
        read_మాట_From_VSpace_ret_t ఫలితం = read_మాట_From_VSpace(vspace_మూలం_2,
                                                             చిరునామా);
        ఒకవేళ (ఫలితం.స్థితి_2 == EXCEPTION_NONE) {
            రాయి("0x%"SEL4_PRIx_మాట": 0x%"SEL4_PRIx_మాట"\n",
                   చిరునామా, ఫలితం.విలువ);
        } లేకపోతే {
            రాయి("0x%"SEL4_PRIx_మాట": INVALID\n", చిరునామా);
        }
    }
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

#ఒకవేళ defined(CONFIG_KERNEL_LOG_బఫరు)
exception_t benchmark_arch_map_log_బఫరు(మాట_t చట్రం_cptr)
{
    lookup_Cap_And_స్థానం_ret_t lu_ret;
    vm_పుట_కొలత_t చట్రం_కొలత;
    pptr_t  చట్రం_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_స్థానం(కణుపు_STATE(ksCurThread), చట్రం_cptr);

    ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", చట్రం_cptr);
        current_fault = seL4_Fault_CapFault_new(చట్రం_cptr, false);

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_get_capType(lu_ret.cap) != cap_చట్రం_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(చట్రం_cptr, false);

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    చట్రం_కొలత = cap_చట్రం_cap_get_cap_FSize(lu_ret.cap);

    ఒకవేళ (చట్రం_కొలత != ARMLarge_పుట) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(చట్రం_cptr, false);

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    చట్రం_pptr = cap_చట్రం_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_బఫరు = pptr_గమ్యం_paddr((శూన్య *) చట్రం_pptr);

    *armKSGlobalLogPTE = pte_pte_పుట_new(
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#లేకపోతే
                             1, // UXN
#అయితే_ముగింపు
                             ks_User_Log_బఫరు,
                             0,                         /* global */
                             1,                         /* access flag */
                             SMP_TERNARY(SMP_SHARE, 0), /* Inner-shareable if SMP enabled, otherwise unshared */
                             0,                         /* VMKernelOnly */
                             సాధారణ_WT);

    cleanByVA_PoU((vptr_t)armKSGlobalLogPTE, addrFromKPPtr(armKSGlobalLogPTE));
    invalidateTranslationSingle(KS_LOG_PPTR);
    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */
