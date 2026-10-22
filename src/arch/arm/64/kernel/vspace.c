/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <benchmark/benchmark.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <kernel/boot.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <kernel/stack.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <machine/debug.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <object/cnode.h>
#अन्तर्भाव <object/untyped.h>
#अन्तर्भाव <arch/api/invocation.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <armv/context_switch.h>
#अन्तर्भाव <arch/object/iospace.h>
#अन्तर्भाव <arch/object/vcpu.h>
#अन्तर्भाव <arch/machine/tlb.h>

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
गणना mair_types {
    DEVICE_nGnRnE = 0,
    DEVICE_nGnRE = 1,
    DEVICE_GRE = 2,
    प्रसामान्यम्_NC = 3,
    प्रसामान्यम् = 4,
    प्रसामान्यम्_WT = 5
};

/* Stage-2 translation memory attributes */
गणना mair_s2_types {
    S2_DEVICE_nGnRnE = 0,
    S2_DEVICE_nGnRE = 1,
    S2_DEVICE_nGRE  = 2,
    S2_DEVICE_GRE = 3,

    S2_प्रसामान्यम्_INNER_NC_OUTER_NC = 5,
    S2_प्रसामान्यम्_INNER_WTC_OUTER_NC = 6,
    S2_प्रसामान्यम्_INNER_WBC_OUTER_NC = 7,

    S2_प्रसामान्यम्_INNER_NC_OUTER_WTC = 9,
    S2_प्रसामान्यम्_INNER_WTC_OUTER_WTC = 10,
    S2_प्रसामान्यम्_INNER_WBC_OUTER_WTC = 11,

    S2_प्रसामान्यम्_INNER_NC_OUTER_WBC = 13,
    S2_प्रसामान्यम्_INNER_WTC_OUTER_WBC = 14,
    S2_प्रसामान्यम्_INNER_WBC_OUTER_WBC = 15,

    S2_प्रसामान्यम् = S2_प्रसामान्यम्_INNER_WBC_OUTER_WBC
};

/* Leif from Linaro said the big.LITTLE clusters should be treated as
 * inner shareable, and we believe so, although the Example B2-1 given in
 * ARM ARM DDI 0487B.b (ID092517) says otherwise.
 */

#परिभाषय SMP_SHARE   3

संरचना lookupPTSlot_ret {
    pte_t *ptSlot;
    word_t pt_द्वयङ्काः_वामः;
};
प्रकारनिर्देश संरचना lookupPTSlot_ret lookupPTSlot_ret_t;

संरचना findVSpaceForASID_ret {
    exception_t स्थितिः;
    vspace_मूलम्_t *vspace_मूलम्;
};
प्रकारनिर्देश संरचना findVSpaceForASID_ret findVSpaceForASID_ret_t;

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

स्थिर word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    विकल्प (vm_rights) {
    पक्ष VMKernelOnly:
        यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            प्रत्यागम 0;
        } अन्यथा {
            प्रत्यागम 0;
        }

    पक्ष VMReadWrite:
        यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            प्रत्यागम 3;
        } अन्यथा {
            प्रत्यागम 1;
        }

    पक्ष VMReadOnly:
        यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            प्रत्यागम 1;
        } अन्यथा {
            प्रत्यागम 3;
        }

    स्वभाव:
        fail("Invalid VM rights");
    }
}

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
स्थिर अन्तःस्थ CONST word_t pte_get_AP(pte_t pte)
{
    प्रतिज्ञा(pte_is_page_type(pte));
    विकल्प (pte_get_pte_type(pte)) {
    पक्ष pte_pte_4k_page:
        प्रत्यागम pte_pte_4k_page_get_AP(pte);

    पक्ष pte_pte_page:
        प्रत्यागम pte_pte_page_get_AP(pte);

    स्वभाव:
        प्रत्यागम 0;
    }
}

स्थिर vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    word_t access_perms = pte_get_AP(pte);
    विकल्प (access_perms) {
    पक्ष 0:
    पक्ष 2:
        प्रत्यागम VMKernelOnly;
    पक्ष 1:
        प्रत्यागम VMReadWrite;
    पक्ष 3:
        प्रत्यागम VMReadOnly;
    स्वभाव:
        fail("Invalid AP bit");
    }
}
#यदि_अन्तः

vm_rights_t CONST आवरणम्_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_आवरणम्)
{
    यदि (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_आवरणम्)) {
        प्रत्यागम VMReadOnly;
    }
    यदि (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_आवरणम्)) {
        यदि (!seL4_CapRights_get_capAllowWrite(cap_rights_आवरणम्)) {
            प्रत्यागम VMReadOnly;
        } अन्यथा {
            प्रत्यागम VMReadWrite;
        }
    }
    यदि (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_आवरणम्) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_आवरणम्)) {
        userError("Attempted to make unsupported write only mapping");
    }
    प्रत्यागम VMKernelOnly;
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
BOOT_संहिता शून्य map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_गुणाः_t गुणाः)
{
    प्रतिज्ञा(vaddr >= PPTR_शिखरम्);

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t uxn = vm_गुणाः_get_arm_Execute_Never(गुणाः);
#अन्यथा
    word_t uxn = 1; /* unprivileged execute never */
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    word_t attr_सूचकांक;
    word_t shareable;
    यदि (vm_गुणाः_get_arm_Page_Cacheable(गुणाः)) {
        attr_सूचकांक = प्रसामान्यम्;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } अन्यथा {
        attr_सूचकांक = DEVICE_nGnRnE;
        shareable = 0;
    }
    armKSGlobalKernelPT[GET_KPT_सूचकांक(vaddr, KLVL_FRM_ARM_PT_LVL(3))] = pte_pte_4k_page_new(uxn, paddr,
                                                                                            0, /* global */
                                                                                            1, /* access flag */
                                                                                            shareable,
                                                                                            APFromVMRights(vm_rights),
                                                                                            attr_सूचकांक);
}

/* Essentially, assert that the ELF, kernel devices, and log buffer are all in the same PUD.
   In the current code this is the top index. This is arbitrary, the important part
   is that they are all the same.
 */
compile_assert(elf_अन्तर्_kernel_mappings, GET_KPT_सूचकांक(KERNEL_ELF_आधारः,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_सूचकांक_द्वयङ्काः) - 1);
compile_assert(kdev_अन्तर्_kernel_mappings, GET_KPT_सूचकांक(KDEV_आधारः,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_सूचकांक_द्वयङ्काः) - 1);
compile_assert(log_buf_अन्तर्_kernel_mappings, GET_KPT_सूचकांक(KS_LOG_आधारः,
                                                         KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_सूचकांक_द्वयङ्काः) - 1);

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
BOOT_संहिता शून्य map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_आधारः_RAW, se_L4_Large_Page_द्वयङ्काः));
    compile_assert(elf_आधारः_aligned, IS_ALIGNED(KERNEL_ELF_आधारः, se_L4_Large_Page_द्वयङ्काः));

    /* Please don't overlap with KDEV_BASE */
    प्रतिज्ञा(KERNEL_ELF_शिखरम् <= KDEV_आधारः);
    /* Please don't overlap with KS_LOG_PPTR */
    प्रतिज्ञा(KERNEL_ELF_शिखरम् <= KS_LOG_PPTR);

    ध्रुव word_t pd_start = GET_KPT_सूचकांक(KERNEL_ELF_आधारः, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    ध्रुव word_t pd_अन्तः   = GET_KPT_सूचकांक(KERNEL_ELF_शिखरम् - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_आधारः;
    अप्रयुक्तम् pptr_t vaddr = KERNEL_ELF_आधारः;

    प्रति (word_t pd = pd_start; pd < pd_अन्तः; pd++) {
        प्रतिज्ञा(pd == GET_KPT_सूचकांक(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_page_new(
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#अन्यथा
                                     /* UXN */ 1,
#यदि_अन्तः
                                     /* page_base_address */ paddr,
                                     /* global (nG) */ 0,
                                     /* access flag (AF) */ 1,
                                     /* shareability (SH) */ SMP_TERNARY(SMP_SHARE, 0),
                                     /* AP */ APFromVMRights(VMKernelOnly),
                                     /* AttrIndx */ प्रसामान्यम्
                                 );

        paddr += BIT(se_L4_Large_Page_द्वयङ्काः);
        vaddr += BIT(se_L4_Large_Page_द्वयङ्काः);
    }
}

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_गवाक्षः_pgd_location,
               GET_KPT_सूचकांक(PPTR_आधारः, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#अन्यथा
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_गवाक्षः_pgd_location,
               GET_KPT_सूचकांक(PPTR_आधारः, KLVL_FRM_ARM_PT_LVL(0)) == BIT(PT_सूचकांक_द्वयङ्काः) - 1);
#यदि_अन्तः
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_गवाक्षः_आधारः_aligned, IS_ALIGNED(PPTR_आधारः, se_L4_Huge_Page_द्वयङ्काः));
compile_assert(physical_गवाक्षः_शिखरम्_aligned, IS_ALIGNED(PPTR_शिखरम्, se_L4_Huge_Page_द्वयङ्काः));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_गवाक्षः_bottom_kernel_region,
               GET_KPT_सूचकांक(PPTR_आधारः, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_गवाक्षः_शिखरम्_kernel_region,
               GET_KPT_सूचकांक(PPTR_शिखरम्, KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_सूचकांक_द्वयङ्काः) - 1);

BOOT_संहिता शून्य map_kernel_physical_गवाक्षः(शून्य)
{
    paddr_t paddr;
    pptr_t vaddr;
    word_t idx;

    /* place all PDs except the last one in PUD */
    प्रति (idx = GET_KPT_सूचकांक(PPTR_आधारः, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_सूचकांक(PPTR_शिखरम्, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_सारणी_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_आधारः;
    प्रति (paddr = PADDR_आधारः; paddr < PADDR_शिखरम्; paddr += BIT(se_L4_Large_Page_द्वयङ्काः)) {
        armKSGlobalKernelPDs[GET_KPT_सूचकांक(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_सूचकांक(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_page_new(
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#अन्यथा
                                                                                                                        1, // UXN
#यदि_अन्तः
                                                                                                                        paddr,
                                                                                                                        0,                        /* global */
                                                                                                                        1,                        /* access flag */
                                                                                                                        SMP_TERNARY(SMP_SHARE, 0),        /* Inner-shareable if SMP enabled, otherwise unshared */
                                                                                                                        0,                        /* VMKernelOnly */
                                                                                                                        प्रसामान्यम्
                                                                                                                    );
        vaddr += BIT(se_L4_Large_Page_द्वयङ्काः);
    }
}

BOOT_संहिता शून्य map_kernel_गवाक्षः(शून्य)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_सूचकांक(PPTR_आधारः, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_सारणी_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_गवाक्षः();

    ध्रुव word_t elf_pud_idx = GET_KPT_सूचकांक(KERNEL_ELF_आधारः, KLVL_FRM_ARM_PT_LVL(1));
    pte_t *kernel_mapping_pds = armKSGlobalKernelPDs[elf_pud_idx];

    /* place the kernel mapping PDs in the kernel mapping PUD */
    armKSGlobalKernelPUD[elf_pud_idx] = pte_pte_सारणी_new(addrFromKPPtr(kernel_mapping_pds));

    /* map the kernel ELF */
    map_kernel_elf_image(kernel_mapping_pds);

    /* put the PT into the PD for device window. It would be nice if
       we could pass kernel_mapping_pds[511] to map_kernel_devices(), but it is
       shared between AArch64 and AArch32, so... */
    kernel_mapping_pds[BIT(PT_सूचकांक_द्वयङ्काः) - 1]
        = pte_pte_सारणी_new(addrFromKPPtr(armKSGlobalKernelPT));

    map_kernel_devices();
}

/* When the hypervisor support is enabled, the stage-2 translation table format
 * is used for applications.
 * The global bit is always 0.
 * The memory attributes use the S2 translation values.
 */
स्थिर BOOT_संहिता शून्य map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap, bool_t executable)
{
    vspace_मूलम्_t *vspace_मूलम्_2 = VSPACE_PTR(pptr_सम्बन्धि_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(frame_cap);
    शून्य *pptr = (शून्य *)cap_frame_cap_get_capFBasePtr(frame_cap);

    प्रतिज्ञा(cap_frame_cap_get_capFMappedASID(frame_cap) != 0);

#यदि_परिभाषितम् AARCH64_VSPACE_S2_START_L1
    pud = vspace_मूलम्_2;
#अन्यथा
    vspace_मूलम्_2 += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(0));
    प्रतिज्ञा(pte_pte_सारणी_ptr_get_present(vspace_मूलम्_2));
    pud = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(vspace_मूलम्_2));
#यदि_अन्तः
    pud += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(1));
    प्रतिज्ञा(pte_pte_सारणी_ptr_get_present(pud));
    pd = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(pud));
    pd += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(2));
    प्रतिज्ञा(pte_pte_सारणी_ptr_get_present(pd));
    pt = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(pd));
    *(pt + GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_page_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_गन्तव्यम्_paddr(pptr),            /* page_base_address    */
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#अन्यथा
                                                              1,                              /* not global */
#यदि_अन्तः
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_प्रसामान्यम्
#अन्यथा
                                                              प्रसामान्यम्
#यदि_अन्तः
                                                          );
}

स्थिर BOOT_संहिता cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_page_आकार_t frame_आकार_2;
    यदि (use_large) {
        frame_आकार_2 = ARMLargePage;
    } अन्यथा {
        frame_आकार_2 = ARMSmallPage;
    }
    प्रत्यागम
        cap_frame_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            frame_आकार_2,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            wordFromVMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

स्थिर BOOT_संहिता शून्य map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_मूलम्_t *vspace_मूलम्_2 = VSPACE_PTR(pptr_सम्बन्धि_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(pt_cap);

    प्रतिज्ञा(cap_page_सारणी_cap_get_cap_PTIs_Mapped(pt_cap));

#यदि_परिभाषितम् AARCH64_VSPACE_S2_START_L1
    pud = vspace_मूलम्_2;
#अन्यथा
    vspace_मूलम्_2 += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(0));
    प्रतिज्ञा(pte_pte_सारणी_ptr_get_present(vspace_मूलम्_2));
    pud = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(vspace_मूलम्_2));
#यदि_अन्तः
    pud += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(1));
    प्रतिज्ञा(pte_pte_सारणी_ptr_get_present(pud));
    pd = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(pud));
    *(pd + GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_सारणी_new(
                                                              pptr_गन्तव्यम्_paddr(pt)
                                                          );
}

स्थिर BOOT_संहिता cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_सारणी_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    प्रत्यागम cap;
}

स्थिर BOOT_संहिता शून्य map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_मूलम्_t *vspace_मूलम्_2 = VSPACE_PTR(pptr_सम्बन्धि_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(pd_cap));
    vptr_t vptr = cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(pd_cap);

    प्रतिज्ञा(cap_page_सारणी_cap_get_cap_PTIs_Mapped(pd_cap));

#यदि_परिभाषितम् AARCH64_VSPACE_S2_START_L1
    pud = vspace_मूलम्_2;
#अन्यथा
    vspace_मूलम्_2 += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(0));
    प्रतिज्ञा(pte_pte_सारणी_ptr_get_present(vspace_मूलम्_2));
    pud = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(vspace_मूलम्_2));
#यदि_अन्तः
    *(pud + GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_सारणी_new(
                                                               pptr_गन्तव्यम्_paddr(pd)
                                                           );
}

स्थिर BOOT_संहिता cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_सारणी_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    प्रत्यागम cap;
}

#यदि_अपरिभाषितम् AARCH64_VSPACE_S2_START_L1
स्थिर BOOT_संहिता शून्य map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_सम्बन्धि_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(pud_cap));
    vptr_t vptr = cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(pud_cap);

    प्रतिज्ञा(cap_page_सारणी_cap_get_cap_PTIs_Mapped(pud_cap));

    *(pgd + GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_सारणी_new(
                                                               pptr_गन्तव्यम्_paddr(pud));
}

स्थिर BOOT_संहिता cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_सारणी_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    प्रत्यागम cap;
}
#यदि_अन्तः /* AARCH64_VSPACE_S2_START_L1 */
BOOT_संहिता word_t arch_get_n_paging(v_region_t it_v_reg)
{
    प्रत्यागम
#यदि_अपरिभाषितम् AARCH64_VSPACE_S2_START_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_द्वयङ्काः(ULVL_FRM_ARM_PT_LVL(0))) +
#यदि_अन्तः
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_द्वयङ्काः(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_द्वयङ्काः(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_संहिता cap_t create_it_स्थानाङ्कः_space(cap_t मूलम्_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    /* create the PGD */
    vspace_cap = cap_vspace_cap_new(
                     IT_ASID,           /* capVSMappedASID */
                     rootserver.vspace, /* capVSBasePtr    */
                     1                  /* capVSIsMapped   */
#यदि_परिभाषितम् CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#यदि_अन्तः
                 );
    slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#यदि_अपरिभाषितम् AARCH64_VSPACE_S2_START_L1
    /* Create any PUDs needed for the user land image */
    प्रति (vptr = आवर्तनम्_अधस्तात्(it_v_reg.start, GET_ULVL_PGSIZE_द्वयङ्काः(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.अन्तः_2;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        यदि (!provide_cap(मूलम्_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            प्रत्यागम cap_null_cap_new();
        }
    }
#यदि_अन्तः
    /* Create any PDs needed for the user land image */
    प्रति (vptr = आवर्तनम्_अधस्तात्(it_v_reg.start, GET_ULVL_PGSIZE_द्वयङ्काः(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.अन्तः_2;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        यदि (!provide_cap(मूलम्_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            प्रत्यागम cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    प्रति (vptr = आवर्तनम्_अधस्तात्(it_v_reg.start, GET_ULVL_PGSIZE_द्वयङ्काः(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.अन्तः_2;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        यदि (!provide_cap(मूलम्_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            प्रत्यागम cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };
    प्रत्यागम vspace_cap;
}

BOOT_संहिता cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    प्रत्यागम create_it_frame_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_संहिता cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large);
    map_it_frame_cap(pd_cap, cap, executable);
    प्रत्यागम cap;
}

BOOT_संहिता शून्य activate_kernel_vspace(शून्य)
{
    cleanInvalidateL1Caches();
    समुच्चयः_Current_Kernel_VSpace_मूलम्(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    समुच्चयः_Current_User_VSpace_मूलम्(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    तालकम्_TLBEntry(KERNEL_ELF_आधारः);
}

BOOT_संहिता शून्य write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_सम्बन्धि_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#यदि_परिभाषितम् CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#यदि_अन्तः
                              /* vspace_root: reference to vspace root page table object */
                              (word_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#यदि_अन्तः
                          );
    ap->array[ASID_नीचः(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_उच्चः(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t findMapForASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_उच्चः(asid)];
    यदि (!poolPtr) {
        प्रत्यागम asid_map_asid_map_none_new();
    }

    प्रत्यागम poolPtr->array[ASID_नीचः(asid)];
}

स्थिर findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    यदि (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_मूलम्_new();

        ret.vspace_मूलम् = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    ret.vspace_मूलम् = (vspace_मूलम्_t *)asid_map_asid_map_vspace_get_vspace_मूलम्(asid_map);
    ret.स्थितिः = EXCEPTION_NONE;
    प्रत्यागम ret;
}

word_t *विशुद्धम् lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_बफर_Ptr;
    cap_t बफर_Cap;
    vm_rights_t vm_rights;

    w_बफर_Ptr = thread->tcbIPCBuffer;
    बफर_Cap = TCB_PTR_CTE_PTR(thread, tcb_बफर)->cap;

    यदि (unlikely(cap_get_capType(बफर_Cap) != cap_frame_cap)) {
        प्रत्यागम रिक्त;
    }
    यदि (unlikely(cap_frame_cap_get_capFIsDevice(बफर_Cap))) {
        प्रत्यागम रिक्त;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(बफर_Cap);
    यदि (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t आधारः_Ptr;
        अचिह्नित पूर्णाङ्क page_द्वयङ्काः;

        आधारः_Ptr = cap_frame_cap_get_capFBasePtr(बफर_Cap);
        page_द्वयङ्काः = page_द्वयङ्काः_For_आकार(cap_frame_cap_get_capFSize(बफर_Cap));
        प्रत्यागम (word_t *)(आधारः_Ptr + (w_बफर_Ptr & आवरणम्(page_द्वयङ्काः)));
    } अन्यथा {
        प्रत्यागम रिक्त;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    यदि (cap_get_capType(cap) != cap_frame_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (!IS_ALIGNED(vptr, se_L4_IPCBuffer_आकार_द्वयङ्काः)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (पूर्णाङ्क)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर lookupPTSlot_ret_t lookupPTSlot(vspace_मूलम्_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    word_t level = UPT_स्तराः - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_द्वयङ्काः_वामः = PT_सूचकांक_द्वयङ्काः * level + se_L4_Page_द्वयङ्काः;
    ret.ptSlot = pt + ((vptr >> ret.pt_द्वयङ्काः_वामः) & आवरणम्(se_L4_VSpace_सूचकांक_द्वयङ्काः));

    यावत् (pte_pte_सारणी_ptr_get_present(ret.ptSlot) && likely(level > 0)) {
        level--;
        ret.pt_द्वयङ्काः_वामः -= PT_सूचकांक_द्वयङ्काः;
        pt = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(ret.ptSlot));
        ret.ptSlot = pt + ((vptr >> ret.pt_द्वयङ्काः_वामः) & आवरणम्(PT_सूचकांक_द्वयङ्काः));
    }

    प्रत्यागम ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
स्थिर pte_t makeUserPagePTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_गुणाः_t गुणाः,
                             vm_page_आकार_t page_आकार)
{
    bool_t nonexecutable = vm_गुणाः_get_arm_Execute_Never(गुणाः) || !cap_read;
    word_t cacheable = vm_गुणाः_get_arm_Page_Cacheable(गुणाः);

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t nG = 0; /* not global */
    word_t attridx = cacheable ? S2_प्रसामान्यम् : S2_DEVICE_nGnRnE;
#अन्यथा
    word_t nG = 1; /* not global */
    word_t attridx = cacheable ? प्रसामान्यम् : DEVICE_nGnRnE;
#यदि_अन्तः

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    word_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    यदि (page_आकार == ARMSmallPage) {
        प्रत्यागम pte_pte_4k_page_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } अन्यथा {
        प्रत्यागम pte_pte_page_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    विकल्प (vm_faultType) {
    पक्ष ARMDataAbort: {
        word_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        यदि (ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(स्थानाङ्कः_Translate_S1(addr)) | (addr & आवरणम्(PAGE_द्वयङ्काः));
        }
#यदि_अन्तः
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        प्रत्यागम EXCEPTION_FAULT;
    }

    पक्ष ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        यदि (ARCH_ग्रन्थिः_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(स्थानाङ्कः_Translate_S1(pc)) | (pc & आवरणम्(PAGE_द्वयङ्काः));
        }
#यदि_अन्तः
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        प्रत्यागम EXCEPTION_FAULT;
    }

    स्वभाव:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_मूलम्(cap_t cap)
{
    प्रत्यागम cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_मूलम्(cap_t cap)
{
    प्रत्यागम is_VTable_मूलम्(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_मूलम्(cap_t cap)
{
    प्रत्यागम is_Valid_Native_मूलम्(cap);
}

शून्य समुच्चयः_VMRoot(tcb_t *tcb)
{
    cap_t thread_मूलम्;
    asid_t asid;
    vspace_मूलम्_t *vspace_मूलम्_2;
    findVSpaceForASID_ret_t find_ret;

    thread_मूलम् = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    यदि (!is_Valid_Native_मूलम्(thread_मूलम्)) {
        समुच्चयः_Current_User_VSpace_मूलम्(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        प्रत्यागम;
    }

    vspace_मूलम्_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_मूलम्));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_मूलम्);
    find_ret = findVSpaceForASID(asid);
    यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE || find_ret.vspace_मूलम् != vspace_मूलम्_2)) {
        समुच्चयः_Current_User_VSpace_मूलम्(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        प्रत्यागम;
    }

    armv_contextSwitch(vspace_मूलम्_2, asid);
}

स्थिर bool_t समुच्चयः_VMRoot_For_Flush(vspace_मूलम्_t *vspace, asid_t asid)
{
    cap_t thread_मूलम्;

    thread_मूलम् = TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbVTable)->cap;

    यदि (cap_get_capType(thread_मूलम्) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_मूलम्) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_मूलम्)) == vspace) {
        प्रत्यागम false;
    }

    armv_contextSwitch(vspace, asid);
    प्रत्यागम true;
}


#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT

स्थिर अन्तःस्थ asid_pool_t *getPoolPtr(asid_t asid)
{
    प्रत्यागम armKSASIDTable[ASID_उच्चः(asid)];
}

स्थिर अन्तःस्थ asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    प्रतिज्ञा(poolPtr != रिक्त);
    प्रत्यागम poolPtr->array[ASID_नीचः(asid)];
}

स्थिर अन्तःस्थ शून्य समुच्चयः_ASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    प्रतिज्ञा(poolPtr != रिक्त);
    poolPtr->array[ASID_नीचः(asid)] = asid_map;
}

स्थिर शून्य invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    प्रतिज्ञा(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_समुच्चयः_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_समुच्चयः_stored_vmid_valid(asid_map, false);

    समुच्चयः_ASIDMap(poolPtr, asid, asid_map);
}

स्थिर शून्य storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    प्रतिज्ञा(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_समुच्चयः_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_समुच्चयः_stored_vmid_valid(asid_map, true);

    समुच्चयः_ASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

स्थिर hw_asid_t findFreeHWASID(शून्य)
{
    word_t hw_asid_विस्थापनम्;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    प्रति (hw_asid_विस्थापनम् = 0;
         hw_asid_विस्थापनम् <= (word_t)((hw_asid_t) - 1);
         hw_asid_विस्थापनम्++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_विस्थापनम्);
        यदि (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            प्रत्यागम hw_asid;
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
    यदि (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    प्रत्यागम hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    यदि (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        प्रत्यागम asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } अन्यथा {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        प्रत्यागम new_hw_asid;
    }
}

स्थिर शून्य invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    यदि (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARM_SMMU
स्थिर word_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_उच्चः(asid)];
    प्रतिज्ञा(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_नीचः(asid)];
    प्रतिज्ञा(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    प्रत्यागम asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

शून्य increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_उच्चः(asid)];
    प्रतिज्ञा(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_नीचः(asid)];
    प्रतिज्ञा(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_समुच्चयः_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

शून्य decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_उच्चः(asid)];
    प्रतिज्ञा(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_नीचः(asid)];
    प्रतिज्ञा(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_समुच्चयः_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#यदि_अन्तः

स्थिर अन्तःस्थ शून्य invalidateTLBByASID(asid_t asid)
{
#यदि_परिभाषितम् CONFIG_ARM_SMMU
    word_t bind_cb = getASIDBindCB(asid);
    यदि (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    यदि (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        प्रत्यागम;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#अन्यथा
    invalidateTranslationASID(asid);
#यदि_अन्तः
}

स्थिर अन्तःस्थ शून्य invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#यदि_परिभाषितम् CONFIG_ARM_SMMU
    word_t bind_cb = getASIDBindCB(asid);
    यदि (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    यदि (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        प्रत्यागम;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> se_L4_Page_द्वयङ्काः);
#अन्यथा
    invalidateTranslationSingle((asid << 48) | vaddr >> se_L4_Page_द्वयङ्काः);
#यदि_अन्तः
}


शून्य unmap_Page_सारणी(asid_t asid, vptr_t vptr, pte_t *target_pt)
{
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
        /* nothing to do */
        प्रत्यागम;
    }
    pte_t *ptSlot = रिक्त;
    pte_t *pt = (pte_t *)find_ret.vspace_मूलम्;

    प्रति (word_t i = 0; i < UPT_स्तराः - 1 && pt != target_pt; i++) {
        ptSlot = pt + GET_UPT_सूचकांक(vptr, i);
        यदि (unlikely(!pte_pte_सारणी_ptr_get_present(ptSlot))) {
            /* couldn't find it */
            प्रत्यागम;
        }
        pt = paddr_गन्तव्यम्_pptr(pte_pte_सारणी_ptr_get_pt_आधारः_स्थानाङ्कः(ptSlot));
    }

    यदि (pt != target_pt) {
        /* didn't find it */
        प्रत्यागम;
    }
    /* If we found a pt then ptSlot won't be null */
    प्रतिज्ञा(ptSlot != रिक्त);
    *ptSlot = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)ptSlot, pptr_गन्तव्यम्_paddr(ptSlot));
    invalidateTLBByASID(asid);
}

शून्य unmapPage(vm_page_आकार_t page_आकार, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    find_ret = findVSpaceForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम;
    }

    lu_ret = lookupPTSlot(find_ret.vspace_मूलम्, vptr);
    यदि (unlikely(lu_ret.pt_द्वयङ्काः_वामः != page_द्वयङ्काः_For_आकार(page_आकार))) {
        /* Do nothing if the wrong size object was returned */
        प्रत्यागम;
    }

    pte = *(lu_ret.ptSlot);
    यदि (!pte_is_page_type(pte)) {
        /* Do nothing if no page is present */
        प्रत्यागम;
    }

    यदि (pte_get_page_आधारः_स्थानाङ्कः(pte) != pptr_गन्तव्यम्_paddr((शून्य *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        प्रत्यागम;
    }

    *(lu_ret.ptSlot) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.ptSlot, pptr_गन्तव्यम्_paddr(lu_ret.ptSlot));
    प्रतिज्ञा(asid < BIT(16));
    invalidateTLBByASIDVA(asid, vptr);
}

शून्य deleteASID(asid_t asid, vspace_मूलम्_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_उच्चः(asid)];

    यदि (poolPtr != रिक्त) {
        asid_map_t asid_map = poolPtr->array[ASID_नीचः(asid)];
        यदि (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_मूलम्_t *)asid_map_asid_map_vspace_get_vspace_मूलम्(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#यदि_अन्तः
            poolPtr->array[ASID_नीचः(asid)] = asid_map_asid_map_none_new();
            समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
        }
    }
}

शून्य deleteASIDPool(asid_t asid_आधारः, asid_pool_t *pool)
{
    word_t विस्थापनम्;

    प्रतिज्ञा((asid_आधारः & आवरणम्(asid_नीचः_द्वयङ्काः)) == 0);

    यदि (armKSASIDTable[ASID_उच्चः(asid_आधारः)] == pool) {
        प्रति (विस्थापनम् = 0; विस्थापनम् < BIT(asid_नीचः_द्वयङ्काः); विस्थापनम्++) {
            asid_map_t asid_map = pool->array[विस्थापनम्];
            यदि (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_आधारः + विस्थापनम्);
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_आधारः + विस्थापनम्);
#यदि_अन्तः
            }
        }
        armKSASIDTable[ASID_उच्चः(asid_आधारः)] = रिक्त;
        समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
    }
}

स्थिर शून्य doFlush(word_t invLabel, vptr_t start, vptr_t अन्तः_2, paddr_t pstart)
{
    विकल्प (invLabel) {
    पक्ष ARMVSpace_Clean_दत्त:
    पक्ष ARMPage_Clean_दत्त:
        cleanCacheRange_RAM(start, अन्तः_2, pstart);
        भङ्ग;

    पक्ष ARMVSpace_Invalidate_दत्त:
    पक्ष ARMPage_Invalidate_दत्त:
        invalidateCacheRange_RAM(start, अन्तः_2, pstart);
        भङ्ग;

    पक्ष ARMVSpace_Clean_Invalidate_दत्त:
    पक्ष ARMPage_Clean_Invalidate_दत्त:
        cleanInvalidateCacheRange_RAM(start, अन्तः_2, pstart);
        भङ्ग;

    पक्ष ARMVSpaceUnify_Instruction:
    पक्ष ARMPageUnify_Instruction:
        /* First clean data lines to point of unification... */
        cleanCacheRange_PoU(start, अन्तः_2, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(start, अन्तः_2, pstart);
        /* ... then wait for the completion of invalidations... */
        dsb();
        /* ... and ensure new instructions come from fresh cache lines. */
        isb();
        भङ्ग;
    स्वभाव:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

स्थिर exception_t performVSpaceFlush(word_t invLabel, vspace_मूलम्_t *vspace_मूलम्_2, asid_t asid,
                                      vptr_t start, vptr_t अन्तः_2, paddr_t pstart)
{

    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        word_t आकार = अन्तः_2 - start;
        start = (vptr_t)paddr_गन्तव्यम्_pptr(pstart);
        अन्तः_2 = start + आकार;
        यदि (start < अन्तः_2) {
            doFlush(invLabel, start, अन्तः_2, pstart);
        }
    } अन्यथा {
        bool_t मूलम्_switched;

        /* Flush if given a non zero range */
        यदि (start < अन्तः_2) {
            मूलम्_switched = समुच्चयः_VMRoot_For_Flush(vspace_मूलम्_2, asid);
            doFlush(invLabel, start, अन्तः_2, pstart);
            यदि (मूलम्_switched) {
                समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
            }
        }
    }
    प्रत्यागम EXCEPTION_NONE;
}


स्थिर exception_t perform_Page_सारणी_Invocation_Map(cap_t cap, cte_t *ctSlot, pte_t pte, pte_t *ptSlot)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    cleanByVA_PoU((vptr_t)ptSlot, pptr_गन्तव्यम्_paddr(ptSlot));

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t perform_Page_सारणी_Invocation_Unmap(cap_t cap, cte_t *ctSlot)
{
    यदि (cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PT_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap));
        unmap_Page_सारणी(cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap),
                       cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap), pt);
        clear_स्मृतिः_PT((शून्य *)pt, cap_get_cap_आकार_द्वयङ्काः(cap));
    }

    cap_page_सारणी_cap_ptr_समुच्चयः_cap_PTIs_Mapped(&(ctSlot->cap), 0);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performPageInvocationMap(asid_t asid, cap_t cap, cte_t *ctSlot,
                                            pte_t pte, pte_t *ptSlot)
{
    bool_t tlbflush_required = pte_ptr_get_valid(ptSlot);

    ctSlot->cap = cap;
    *ptSlot = pte;

    cleanByVA_PoU((vptr_t)ptSlot, pptr_गन्तव्यम्_paddr(ptSlot));
    यदि (unlikely(tlbflush_required)) {
        प्रतिज्ञा(asid < BIT(16));
        invalidateTLBByASIDVA(asid, cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap));
    }

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    यदि (cap_frame_cap_get_capFMappedASID(cap) != 0) {

        unmapPage(cap_frame_cap_get_capFSize(cap),
                  cap_frame_cap_get_capFMappedASID(cap),
                  cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                  cap_frame_cap_get_capFBasePtr(cap));
    }

    cap_t slotCap = ctSlot->cap;
    slotCap = cap_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(slotCap, 0);
    slotCap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(slotCap, asidInvalid);
    ctSlot->cap = slotCap;


    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performPageFlush(word_t invLabel, vspace_मूलम्_t *vspace_मूलम्_2, asid_t asid,
                                    vptr_t start, vptr_t अन्तः_2, paddr_t pstart)
{
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        word_t आकार = अन्तः_2 - start;
        start = (vptr_t)paddr_गन्तव्यम्_pptr(pstart);
        अन्तः_2 = start + आकार;

        यदि (start < अन्तः_2) {
            doFlush(invLabel, start, अन्तः_2, pstart);
        }
    } अन्यथा {
        bool_t मूलम्_switched;

        यदि (start < अन्तः_2) {
            मूलम्_switched = समुच्चयः_VMRoot_For_Flush(vspace_मूलम्_2, asid);
            doFlush(invLabel, start, अन्तः_2, pstart);
            यदि (मूलम्_switched) {
                समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
            }
        }
    }
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t perform_Page_Get_स्थानाङ्कः(pptr_t आधारः_ptr, bool_t call)
{
    paddr_t आधारः = pptr_गन्तव्यम्_paddr((शून्य *)आधारः_ptr);

    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    यदि (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);
        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
        अचिह्नित पूर्णाङ्क दीर्घता = समुच्चयः_MR(thread, ipc_बफर, 0, आधारः);
        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performASIDControlInvocation(शून्य *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_आधारः)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_समुच्चयः_cap_Free_सूचकांक(&(parent->cap),
                                         MAX_FREE_सूचकांक(cap_untyped_cap_get_cap_खण्डम्_आकार(parent->cap)));

    memzero(frame, BIT(se_L4_ASIDPool_द्वयङ्काः));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_निवेशय(
        cap_asid_pool_cap_new(
            asid_आधारः,         /* capASIDBase  */
            WORD_REF(frame)    /* capASIDPool  */
        ), parent, slot);

    प्रतिज्ञा((asid_आधारः & आवरणम्(asid_नीचः_द्वयङ्काः)) == 0);
    armKSASIDTable[ASID_उच्चः(asid_आधारः)] = (asid_pool_t *)frame;

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decode_ARMVSpace_मूलम्_Invocation(word_t invLabel, word_t दीर्घता,
                                                 cte_t *cte, cap_t cap, word_t *बफर)
{
    vptr_t start, अन्तः_2;
    paddr_t pstart;
    asid_t asid;
    vspace_मूलम्_t *vspace_मूलम्_2;
    lookupPTSlot_ret_t resolve_ret;
    findVSpaceForASID_ret_t find_ret;
    pte_t pte;

    विकल्प (invLabel) {
    पक्ष ARMVSpace_Clean_दत्त:
    पक्ष ARMVSpace_Invalidate_दत्त:
    पक्ष ARMVSpace_Clean_Invalidate_दत्त:
    पक्ष ARMVSpaceUnify_Instruction:

        यदि (दीर्घता < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, बफर);
        अन्तः_2 =   getSyscallArg(1, बफर);

        /* Check sanity of arguments */
        यदि (अन्तः_2 <= start) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        यदि (अन्तः_2 > USER_शिखरम्) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(!is_Valid_Native_मूलम्(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_मूलम्_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        find_ret = findVSpaceForASID(asid);
        यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(find_ret.vspace_मूलम् != vspace_मूलम्_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_मूलम्_2, start);
        pte = *resolve_ret.ptSlot;

        /* Check that the returned slot is a page. */
        यदि (!pte_is_page_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
            प्रत्यागम EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        यदि (आवर्तनम्_अधस्तात्(start, resolve_ret.pt_द्वयङ्काः_वामः) != आवर्तनम्_अधस्तात्(अन्तः_2 - 1, resolve_ret.pt_द्वयङ्काः_वामः)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax = आवर्तनम्_अधस्तात्(start, resolve_ret.pt_द्वयङ्काः_वामः) +
                                                  आवरणम्(resolve_ret.pt_द्वयङ्काः_वामः);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

#यदि_अपरिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        यदि (invLabel == ARMVSpace_Invalidate_दत्त && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#यदि_अन्तः

        /* Calculate the physical start address. */
        paddr_t frame_आधारः_2 = pte_get_page_आधारः_स्थानाङ्कः(pte);

        pstart = frame_आधारः_2 + (start & आवरणम्(resolve_ret.pt_द्वयङ्काः_वामः));

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        यदि (pstart < PADDR_आधारः || ((अन्तः_2 - start) + pstart) > PADDR_शिखरम्) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#यदि_अन्तः

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performVSpaceFlush(invLabel, vspace_मूलम्_2, asid, start, अन्तः_2 - 1, pstart);

    स्वभाव:
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}


स्थिर exception_t decode_ARMPage_सारणी_Invocation(word_t invLabel, word_t दीर्घता,
                                                cte_t *cte, cap_t cap, word_t *बफर)
{
    cap_t vspace_मूलम्_Cap;
    vspace_मूलम्_t *vspace_मूलम्_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t ptSlot;
    findVSpaceForASID_ret_t find_ret;

    यदि (invLabel == ARMPage_सारणी_Unmap) {
        यदि (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = seL4_RevokeFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम perform_Page_सारणी_Invocation_Unmap(cap, cte);
    }

    यदि (unlikely(invLabel != ARMPage_सारणी_Map)) {
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त)) {
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, बफर);
    vspace_मूलम्_Cap = current_extra_caps.excaprefs[0]->cap;

    यदि (unlikely(!is_Valid_Native_मूलम्(vspace_मूलम्_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vspace_मूलम्_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_मूलम्_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_मूलम्_Cap);

    यदि (unlikely(vaddr > USER_शिखरम्)) {
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    find_ret = findVSpaceForASID(asid);
    यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(find_ret.vspace_मूलम् != vspace_मूलम्_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    ptSlot = lookupPTSlot(vspace_मूलम्_2, vaddr);

    यदि (unlikely(ptSlot.pt_द्वयङ्काः_वामः == se_L4_Page_द्वयङ्काः || pte_ptr_get_valid(ptSlot.ptSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_सारणी_new(pptr_गन्तव्यम्_paddr(PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap))));

    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTIs_Mapped(cap, 1);
    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTMapped_ASID(cap, asid);
    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTMapped_स्थानाङ्कः(cap, (vaddr & ~आवरणम्(ptSlot.pt_द्वयङ्काः_वामः)));

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम perform_Page_सारणी_Invocation_Map(cap, cte, pte, ptSlot.ptSlot);
}

स्थिर अन्तःस्थ bool_t CONST checkVPAlignment(vm_page_आकार_t sz, word_t w)
{
    प्रत्यागम (w & आवरणम्(page_द्वयङ्काः_For_आकार(sz))) == 0;
}

स्थिर exception_t decodeARMFrameInvocation(word_t invLabel, word_t दीर्घता,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    विकल्प (invLabel) {
    पक्ष ARMPageMap: {
        vptr_t vaddr;
        paddr_t आधारः;
        cap_t vspace_मूलम्_Cap;
        vspace_मूलम्_t *vspace_मूलम्_2;
        asid_t asid, frame_asid;
        vm_rights_t vmRights, capRights;
        vm_page_आकार_t frame_आकार;
        vm_गुणाः_t गुणाः;
        findVSpaceForASID_ret_t find_ret;

        यदि (unlikely(दीर्घता < 3 || current_extra_caps.excaprefs[0] == रिक्त)) {
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, बफर);
        गुणाः = vm_गुणाः_From_Word(getSyscallArg(2, बफर));
        vspace_मूलम्_Cap = current_extra_caps.excaprefs[0]->cap;

        frame_आकार = cap_frame_cap_get_capFSize(cap);
        capRights = cap_frame_cap_get_capFVMRights(cap);
        vmRights = आवरणम्_VMRights(capRights, rightsFromWord(getSyscallArg(1, बफर)));

        यदि (unlikely(!is_Valid_Native_मूलम्(vspace_मूलम्_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        vspace_मूलम्_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_मूलम्_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_मूलम्_Cap);

        find_ret = findVSpaceForASID(asid);
        यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(find_ret.vspace_मूलम् != vspace_मूलम्_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(!checkVPAlignment(frame_आकार, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        frame_asid = cap_frame_cap_get_capFMappedASID(cap);

        यदि (frame_asid != asidInvalid) {
            यदि (frame_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_सङ्ख्या = 1;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;

            } अन्यथा यदि (cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_तर्कः;
                current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
        } अन्यथा {
            यदि (unlikely(vaddr + BIT(page_द्वयङ्काः_For_आकार(frame_आकार)) - 1 > USER_शिखरम्)) {
                current_syscall_error.type = se_L4_Invalid_तर्कः;
                current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(cap, asid);
        cap = cap_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(cap, vaddr);

        आधारः = pptr_गन्तव्यम्_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_मूलम्_2, vaddr);
        यदि (unlikely(lu_ret.pt_द्वयङ्काः_वामः != page_द्वयङ्काः_For_आकार(frame_आकार))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_द्वयङ्काः_वामः);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = makeUserPagePTE(आधारः, vmRights, cap_read, गुणाः, frame_आकार);

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performPageInvocationMap(asid, cap, cte, pte, lu_ret.ptSlot);
    }

    पक्ष ARMPageUnmap:
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performPageInvocationUnmap(cap, cte);

    पक्ष ARMPage_Clean_दत्त:
    पक्ष ARMPage_Invalidate_दत्त:
    पक्ष ARMPage_Clean_Invalidate_दत्त:
    पक्ष ARMPageUnify_Instruction: {
        vptr_t start, अन्तः_2;
        vptr_t vaddr;
        asid_t asid;
        word_t page_आकार;
        findVSpaceForASID_ret_t find_ret;

        यदि (दीर्घता < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(cap_frame_cap_get_capFMappedASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap);
        asid = cap_frame_cap_get_capFMappedASID(cap);

        find_ret = findVSpaceForASID(asid);
        यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, बफर);
        अन्तः_2 =   getSyscallArg(1, बफर);

        /* check that the range is sane */
        यदि (अन्तः_2 <= start) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        page_आकार = BIT(page_द्वयङ्काः_For_आकार(cap_frame_cap_get_capFSize(cap)));
        यदि (start >= page_आकार || अन्तः_2 > page_आकार) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        word_t pstart = pptr_गन्तव्यम्_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap)) + start;
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        यदि (pstart < PADDR_आधारः || ((अन्तः_2 - start) + pstart) > PADDR_शिखरम्) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#अन्यथा
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(find_ret.vspace_मूलम्, vaddr);
        pte_t pte = *lu_ret.ptSlot;
        शून्य *आधारः_ptr = (शून्य *) cap_frame_cap_get_capFBasePtr(cap);
        यदि (unlikely(lu_ret.pt_द्वयङ्काः_वामः != page_द्वयङ्काः_For_आकार(cap_frame_cap_get_capFSize(cap)) ||
                     !pte_is_page_type(pte) ||
                     pte_get_page_आधारः_स्थानाङ्कः(pte) != pptr_गन्तव्यम्_paddr(आधारः_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        यदि (invLabel == ARMPage_Invalidate_दत्त && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
#यदि_अन्तः
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performPageFlush(invLabel, find_ret.vspace_मूलम्, asid, vaddr + start, vaddr + अन्तः_2 - 1,
                                pstart);
    }

    पक्ष ARMPage_Get_स्थानाङ्कः:
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम perform_Page_Get_स्थानाङ्कः(cap_frame_cap_get_capFBasePtr(cap), call);

    स्वभाव:
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t दीर्घता, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_vspace_cap:
        प्रत्यागम decode_ARMVSpace_मूलम्_Invocation(invLabel, दीर्घता, cte, cap, बफर);
    पक्ष cap_page_सारणी_cap:
        प्रत्यागम decode_ARMPage_सारणी_Invocation(invLabel, दीर्घता, cte, cap, बफर);

    पक्ष cap_frame_cap:
        प्रत्यागम decodeARMFrameInvocation(invLabel, दीर्घता, cte, cap, call, बफर);

    पक्ष cap_asid_control_cap: {
        word_t i;
        asid_t asid_आधारः;
        word_t सूचकांक, गभीरता;
        cap_t untyped, मूलम्;
        cte_t *parentSlot, *destSlot;
        lookupSlot_ret_t lu_ret;
        शून्य *frame;
        exception_t स्थितिः;

        यदि (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(दीर्घता < 2 ||
                     current_extra_caps.excaprefs[0] == रिक्त ||
                     current_extra_caps.excaprefs[1] == रिक्त)) {
            current_syscall_error.type = se_L4_Truncated_सन्देशः;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        सूचकांक = getSyscallArg(0, बफर);
        गभीरता = getSyscallArg(1, बफर);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        मूलम् = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        प्रति (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        यदि (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        asid_आधारः = i << asid_नीचः_द्वयङ्काः;

        यदि (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_खण्डम्_आकार(untyped) != se_L4_ASIDPool_द्वयङ्काः ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        स्थितिः = ensureNoChildren(parentSlot);
        यदि (unlikely(स्थितिः != EXCEPTION_NONE)) {
            प्रत्यागम स्थितिः;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(मूलम्, सूचकांक, गभीरता);
        यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
            प्रत्यागम lu_ret.स्थितिः;
        }
        destSlot = lu_ret.slot;

        स्थितिः = ensure_रिक्तानि_Slot(destSlot);
        यदि (unlikely(स्थितिः != EXCEPTION_NONE)) {
            प्रत्यागम स्थितिः;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performASIDControlInvocation(frame, destSlot, parentSlot, asid_आधारः);
    }

    पक्ष cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        यदि (unlikely(invLabel != ARMASIDPool_नियोजय)) {
            current_syscall_error.type = seL4_IllegalOperation;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(current_extra_caps.excaprefs[0] == रिक्त)) {
            current_syscall_error.type = se_L4_Truncated_सन्देशः;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        यदि (unlikely(!is_VTable_मूलम्(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_उच्चः(cap_asid_pool_cap_get_capASIDBase(cap))];

        यदि (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_मूलम्_new();

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        प्रति (i = 0; i < (1 << asid_नीचः_द्वयङ्काः) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        यदि (unlikely(i == 1 << asid_नीचः_द्वयङ्काः)) {
            current_syscall_error.type = seL4_DeleteFirst;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }

    स्वभाव:
        fail("Invalid ARM arch cap type");
    }
}

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
शून्य kernelPrefetchAbort(word_t pc) VISIBLE;
शून्य kernel_दत्त_Abort(word_t pc) VISIBLE;

शून्य kernelPrefetchAbort(word_t pc)
{
    प्रारूप_लिख("\n\nKERNEL PREFETCH ABORT!\n");
    प्रारूप_लिख("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    प्रारूप_लिख("ESR (IFSR): 0x%"SEL4_PRIx_word"\n", getIFSR());
    halt();
}

शून्य kernel_दत्त_Abort(word_t pc)
{
    प्रारूप_लिख("\n\nKERNEL DATA ABORT!\n");
    प्रारूप_लिख("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    प्रारूप_लिख("FAR: 0x%"SEL4_PRIx_word" ESR (DFSR): 0x%"SEL4_PRIx_word"\n",
           getFAR(), getDFSR());
    halt();
}
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */

#यदि_परिभाषितम् CONFIG_PRINTING
प्रकारनिर्देश संरचना readWordFromVSpace_ret {
    exception_t स्थितिः;
    word_t मानम्;
} readWordFromVSpace_ret_t;

स्थिर readWordFromVSpace_ret_t readWordFromVSpace(vspace_मूलम्_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    word_t विस्थापनम्;
    pptr_t kernel_vaddr;
    word_t *मानम्;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    यदि (!pte_ptr_get_valid(lookup_ret.ptSlot) ||
        (pte_pte_सारणी_ptr_get_present(lookup_ret.ptSlot) && lookup_ret.pt_द्वयङ्काः_वामः > PAGE_द्वयङ्काः)) {
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    विस्थापनम् = vaddr & आवरणम्(lookup_ret.pt_द्वयङ्काः_वामः);
    kernel_vaddr = (word_t)paddr_गन्तव्यम्_pptr(pte_page_ptr_get_page_आधारः_स्थानाङ्कः(lookup_ret.ptSlot));
    मानम् = (word_t *)(kernel_vaddr + विस्थापनम्);

    ret.स्थितिः = EXCEPTION_NONE;
    ret.मानम् = *मानम्;
    प्रत्यागम ret;
}

शून्य Arch_user_राशिः_Trace(tcb_t *tptr)
{
    cap_t thread_मूलम्;
    vspace_मूलम्_t *vspace_मूलम्_2;
    word_t sp;

    thread_मूलम् = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    यदि (cap_get_capType(thread_मूलम्) != cap_vspace_cap) {
        प्रारूप_लिख("Invalid vspace\n");
        प्रत्यागम;
    }

    vspace_मूलम्_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_मूलम्));
    sp = get_पञ्जिकाकोशः(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    यदि (!IS_ALIGNED(sp, se_L4_Word_आकार_द्वयङ्काः)) {
        प्रारूप_लिख("SP not aligned\n");
        प्रत्यागम;
    }

    प्रति (अचिह्नित पूर्णाङ्क i = 0; i < CONFIG_USER_राशिः_TRACE_दीर्घता; i++) {
        word_t स्थानाङ्कः = sp + (i * परिमाण(word_t));
        readWordFromVSpace_ret_t परिणाम = readWordFromVSpace(vspace_मूलम्_2,
                                                             स्थानाङ्कः);
        यदि (परिणाम.स्थितिः == EXCEPTION_NONE) {
            प्रारूप_लिख("0x%"SEL4_PRIx_word": 0x%"SEL4_PRIx_word"\n",
                   स्थानाङ्कः, परिणाम.मानम्);
        } अन्यथा {
            प्रारूप_लिख("0x%"SEL4_PRIx_word": INVALID\n", स्थानाङ्कः);
        }
    }
}
#यदि_अन्तः /* CONFIG_PRINTING */

#यदि defined(CONFIG_KERNEL_LOG_बफर)
exception_t benchmark_arch_map_log_बफर(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_आकार_t frame_आकार;
    pptr_t  frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(ग्रन्थिः_STATE(ksCurThread), frame_cptr);

    यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    frame_आकार = cap_frame_cap_get_capFSize(lu_ret.cap);

    यदि (frame_आकार != ARMLargePage) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ks_User_Log_बफर = pptr_गन्तव्यम्_paddr((शून्य *) frame_pptr);

    *armKSGlobalLogPTE = pte_pte_page_new(
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#अन्यथा
                             1, // UXN
#यदि_अन्तः
                             ks_User_Log_बफर,
                             0,                         /* global */
                             1,                         /* access flag */
                             SMP_TERNARY(SMP_SHARE, 0), /* Inner-shareable if SMP enabled, otherwise unshared */
                             0,                         /* VMKernelOnly */
                             प्रसामान्यम्_WT);

    cleanByVA_PoU((vptr_t)armKSGlobalLogPTE, addrFromKPPtr(armKSGlobalLogPTE));
    invalidateTranslationSingle(KS_LOG_PPTR);
    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */
