/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <benchmark/benchmark.h>
#समावेश <api/failures.h>
#समावेश <api/syscall.h>
#समावेश <kernel/boot.h>
#समावेश <kernel/cspace.h>
#समावेश <kernel/thread.h>
#समावेश <kernel/stack.h>
#समावेश <machine/io.h>
#समावेश <machine/debug.h>
#समावेश <model/statedata.h>
#समावेश <object/cnode.h>
#समावेश <object/untyped.h>
#समावेश <arch/api/invocation.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <linker.h>
#समावेश <plat/machine/hardware.h>
#समावेश <armv/context_switch.h>
#समावेश <arch/object/iospace.h>
#समावेश <arch/object/vcpu.h>
#समावेश <arch/machine/tlb.h>

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
क्रमागत mair_types {
    DEVICE_nGnRnE = 0,
    DEVICE_nGnRE = 1,
    DEVICE_GRE = 2,
    NORMAL_NC = 3,
    NORMAL = 4,
    NORMAL_WT = 5
};

/* Stage-2 translation memory attributes */
क्रमागत mair_s2_types {
    S2_DEVICE_nGnRnE = 0,
    S2_DEVICE_nGnRE = 1,
    S2_DEVICE_nGRE  = 2,
    S2_DEVICE_GRE = 3,

    S2_NORMAL_INNER_NC_OUTER_NC = 5,
    S2_NORMAL_INNER_WTC_OUTER_NC = 6,
    S2_NORMAL_INNER_WBC_OUTER_NC = 7,

    S2_NORMAL_INNER_NC_OUTER_WTC = 9,
    S2_NORMAL_INNER_WTC_OUTER_WTC = 10,
    S2_NORMAL_INNER_WBC_OUTER_WTC = 11,

    S2_NORMAL_INNER_NC_OUTER_WBC = 13,
    S2_NORMAL_INNER_WTC_OUTER_WBC = 14,
    S2_NORMAL_INNER_WBC_OUTER_WBC = 15,

    S2_NORMAL = S2_NORMAL_INNER_WBC_OUTER_WBC
};

/* Leif from Linaro said the big.LITTLE clusters should be treated as
 * inner shareable, and we believe so, although the Example B2-1 given in
 * ARM ARM DDI 0487B.b (ID092517) says otherwise.
 */

#घोषणा SMP_SHARE   3

काष्ठा lookupPTSlot_ret {
    pte_t *ptSlot;
    word_t pt_Bits_बायाँ;
};
प्रकार काष्ठा lookupPTSlot_ret lookupPTSlot_ret_t;

काष्ठा findVSpaceForASID_ret {
    exception_t status;
    vspace_मूल_t *vspace_मूल;
};
प्रकार काष्ठा findVSpaceForASID_ret findVSpaceForASID_ret_t;

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

अटल word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    चयन (vm_rights) {
    हाल VMKernelOnly:
        अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            वापस 0;
        } अन्यथा {
            वापस 0;
        }

    हाल VMReadWrite:
        अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            वापस 3;
        } अन्यथा {
            वापस 1;
        }

    हाल VMReadOnly:
        अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            वापस 1;
        } अन्यथा {
            वापस 3;
        }

    शेष:
        fail("Invalid VM rights");
    }
}

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
अटल अंतरभूत CONST word_t pte_get_AP(pte_t pte)
{
    निश्चित(pte_is_page_type(pte));
    चयन (pte_get_pte_type(pte)) {
    हाल pte_pte_4k_page:
        वापस pte_pte_4k_page_get_AP(pte);

    हाल pte_pte_page:
        वापस pte_pte_page_get_AP(pte);

    शेष:
        वापस 0;
    }
}

अटल vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    word_t access_perms = pte_get_AP(pte);
    चयन (access_perms) {
    हाल 0:
    हाल 2:
        वापस VMKernelOnly;
    हाल 1:
        वापस VMReadWrite;
    हाल 3:
        वापस VMReadOnly;
    शेष:
        fail("Invalid AP bit");
    }
}
#पूर्ण_अगर

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    अगर (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        वापस VMReadOnly;
    }
    अगर (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        अगर (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            वापस VMReadOnly;
        } अन्यथा {
            वापस VMReadWrite;
        }
    }
    अगर (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    वापस VMKernelOnly;
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
BOOT_CODE व्योम map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_attributes_t attributes)
{
    निश्चित(vaddr >= PPTR_शीर्ष);

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t uxn = vm_attributes_get_armExecuteNever(attributes);
#अन्यथा
    word_t uxn = 1; /* unprivileged execute never */
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    word_t attr_सूचकांक;
    word_t shareable;
    अगर (vm_attributes_get_armPageCacheable(attributes)) {
        attr_सूचकांक = NORMAL;
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
compile_assert(elf_in_kernel_mappings, GET_KPT_सूचकांक(KERNEL_ELF_BASE,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_सूचकांक_BITS) - 1);
compile_assert(kdev_in_kernel_mappings, GET_KPT_सूचकांक(KDEV_BASE,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_सूचकांक_BITS) - 1);
compile_assert(log_buf_in_kernel_mappings, GET_KPT_सूचकांक(KS_LOG_BASE,
                                                         KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_सूचकांक_BITS) - 1);

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
BOOT_CODE व्योम map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_BASE_RAW, seL4_LargePageBits));
    compile_assert(elf_base_aligned, IS_ALIGNED(KERNEL_ELF_BASE, seL4_LargePageBits));

    /* Please don't overlap with KDEV_BASE */
    निश्चित(KERNEL_ELF_शीर्ष <= KDEV_BASE);
    /* Please don't overlap with KS_LOG_PPTR */
    निश्चित(KERNEL_ELF_शीर्ष <= KS_LOG_PPTR);

    स्थिर word_t pd_start = GET_KPT_सूचकांक(KERNEL_ELF_BASE, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    स्थिर word_t pd_end   = GET_KPT_सूचकांक(KERNEL_ELF_शीर्ष - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_BASE;
    UNUSED pptr_t vaddr = KERNEL_ELF_BASE;

    क्रम (word_t pd = pd_start; pd < pd_end; pd++) {
        निश्चित(pd == GET_KPT_सूचकांक(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_page_new(
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#अन्यथा
                                     /* UXN */ 1,
#पूर्ण_अगर
                                     /* page_base_address */ paddr,
                                     /* global (nG) */ 0,
                                     /* access flag (AF) */ 1,
                                     /* shareability (SH) */ SMP_TERNARY(SMP_SHARE, 0),
                                     /* AP */ APFromVMRights(VMKernelOnly),
                                     /* AttrIndx */ NORMAL
                                 );

        paddr += BIT(seL4_LargePageBits);
        vaddr += BIT(seL4_LargePageBits);
    }
}

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_window_pgd_location,
               GET_KPT_सूचकांक(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#अन्यथा
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_window_pgd_location,
               GET_KPT_सूचकांक(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(0)) == BIT(PT_सूचकांक_BITS) - 1);
#पूर्ण_अगर
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_window_base_aligned, IS_ALIGNED(PPTR_BASE, seL4_HugePageBits));
compile_assert(physical_window_शीर्ष_aligned, IS_ALIGNED(PPTR_शीर्ष, seL4_HugePageBits));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_window_bottom_kernel_region,
               GET_KPT_सूचकांक(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_window_शीर्ष_kernel_region,
               GET_KPT_सूचकांक(PPTR_शीर्ष, KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_सूचकांक_BITS) - 1);

BOOT_CODE व्योम map_kernel_physical_window(व्योम)
{
    paddr_t paddr;
    pptr_t vaddr;
    word_t idx;

    /* place all PDs except the last one in PUD */
    क्रम (idx = GET_KPT_सूचकांक(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_सूचकांक(PPTR_शीर्ष, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_table_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_BASE;
    क्रम (paddr = PADDR_BASE; paddr < PADDR_शीर्ष; paddr += BIT(seL4_LargePageBits)) {
        armKSGlobalKernelPDs[GET_KPT_सूचकांक(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_सूचकांक(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_page_new(
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#अन्यथा
                                                                                                                        1, // UXN
#पूर्ण_अगर
                                                                                                                        paddr,
                                                                                                                        0,                        /* global */
                                                                                                                        1,                        /* access flag */
                                                                                                                        SMP_TERNARY(SMP_SHARE, 0),        /* Inner-shareable if SMP enabled, otherwise unshared */
                                                                                                                        0,                        /* VMKernelOnly */
                                                                                                                        NORMAL
                                                                                                                    );
        vaddr += BIT(seL4_LargePageBits);
    }
}

BOOT_CODE व्योम map_kernel_window(व्योम)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_सूचकांक(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_table_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_window();

    स्थिर word_t elf_pud_idx = GET_KPT_सूचकांक(KERNEL_ELF_BASE, KLVL_FRM_ARM_PT_LVL(1));
    pte_t *kernel_mapping_pds = armKSGlobalKernelPDs[elf_pud_idx];

    /* place the kernel mapping PDs in the kernel mapping PUD */
    armKSGlobalKernelPUD[elf_pud_idx] = pte_pte_table_new(addrFromKPPtr(kernel_mapping_pds));

    /* map the kernel ELF */
    map_kernel_elf_image(kernel_mapping_pds);

    /* put the PT into the PD for device window. It would be nice if
       we could pass kernel_mapping_pds[511] to map_kernel_devices(), but it is
       shared between AArch64 and AArch32, so... */
    kernel_mapping_pds[BIT(PT_सूचकांक_BITS) - 1]
        = pte_pte_table_new(addrFromKPPtr(armKSGlobalKernelPT));

    map_kernel_devices();
}

/* When the hypervisor support is enabled, the stage-2 translation table format
 * is used for applications.
 * The global bit is always 0.
 * The memory attributes use the S2 translation values.
 */
अटल BOOT_CODE व्योम map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap, bool_t executable)
{
    vspace_मूल_t *vspace_मूल_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);
    व्योम *pptr = (व्योम *)cap_frame_cap_get_capFBasePtr(frame_cap);

    निश्चित(cap_frame_cap_get_capFMappedASID(frame_cap) != 0);

#अगर_घोषित AARCH64_VSPACE_S2_START_L1
    pud = vspace_मूल_2;
#अन्यथा
    vspace_मूल_2 += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(0));
    निश्चित(pte_pte_table_ptr_get_present(vspace_मूल_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_मूल_2));
#पूर्ण_अगर
    pud += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(1));
    निश्चित(pte_pte_table_ptr_get_present(pud));
    pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pud));
    pd += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(2));
    निश्चित(pte_pte_table_ptr_get_present(pd));
    pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pd));
    *(pt + GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_page_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_to_paddr(pptr),            /* page_base_address    */
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#अन्यथा
                                                              1,                              /* not global */
#पूर्ण_अगर
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_NORMAL
#अन्यथा
                                                              NORMAL
#पूर्ण_अगर
                                                          );
}

अटल BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_page_आकार_t frame_आकार_2;
    अगर (use_large) {
        frame_आकार_2 = ARMLargePage;
    } अन्यथा {
        frame_आकार_2 = ARMSmallPage;
    }
    वापस
        cap_frame_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            frame_आकार_2,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            wordFromVMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

अटल BOOT_CODE व्योम map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_मूल_t *vspace_मूल_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);

    निश्चित(cap_page_table_cap_get_capPTIsMapped(pt_cap));

#अगर_घोषित AARCH64_VSPACE_S2_START_L1
    pud = vspace_मूल_2;
#अन्यथा
    vspace_मूल_2 += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(0));
    निश्चित(pte_pte_table_ptr_get_present(vspace_मूल_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_मूल_2));
#पूर्ण_अगर
    pud += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(1));
    निश्चित(pte_pte_table_ptr_get_present(pud));
    pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pud));
    *(pd + GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_table_new(
                                                              pptr_to_paddr(pt)
                                                          );
}

अटल BOOT_CODE cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    वापस cap;
}

अटल BOOT_CODE व्योम map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_मूल_t *vspace_मूल_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pd_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pd_cap);

    निश्चित(cap_page_table_cap_get_capPTIsMapped(pd_cap));

#अगर_घोषित AARCH64_VSPACE_S2_START_L1
    pud = vspace_मूल_2;
#अन्यथा
    vspace_मूल_2 += GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(0));
    निश्चित(pte_pte_table_ptr_get_present(vspace_मूल_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_मूल_2));
#पूर्ण_अगर
    *(pud + GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_table_new(
                                                               pptr_to_paddr(pd)
                                                           );
}

अटल BOOT_CODE cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    वापस cap;
}

#अगर_अघोषित AARCH64_VSPACE_S2_START_L1
अटल BOOT_CODE व्योम map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pud_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pud_cap);

    निश्चित(cap_page_table_cap_get_capPTIsMapped(pud_cap));

    *(pgd + GET_UPT_सूचकांक(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_table_new(
                                                               pptr_to_paddr(pud));
}

अटल BOOT_CODE cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    वापस cap;
}
#पूर्ण_अगर /* AARCH64_VSPACE_S2_START_L1 */
BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    वापस
#अगर_अघोषित AARCH64_VSPACE_S2_START_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(0))) +
#पूर्ण_अगर
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_CODE cap_t create_it_address_space(cap_t मूल_cnode_cap, v_region_t it_v_reg)
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
#अगर_घोषित CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#पूर्ण_अगर
                 );
    slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#अगर_अघोषित AARCH64_VSPACE_S2_START_L1
    /* Create any PUDs needed for the user land image */
    क्रम (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        अगर (!provide_cap(मूल_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            वापस cap_null_cap_new();
        }
    }
#पूर्ण_अगर
    /* Create any PDs needed for the user land image */
    क्रम (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        अगर (!provide_cap(मूल_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            वापस cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    क्रम (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        अगर (!provide_cap(मूल_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            वापस cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };
    वापस vspace_cap;
}

BOOT_CODE cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    वापस create_it_frame_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large);
    map_it_frame_cap(pd_cap, cap, executable);
    वापस cap;
}

BOOT_CODE व्योम activate_kernel_vspace(व्योम)
{
    cleanInvalidateL1Caches();
    set_Current_Kernel_VSpace_मूल(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    set_Current_User_VSpace_मूल(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    lockTLBEntry(KERNEL_ELF_BASE);
}

BOOT_CODE व्योम write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#अगर_घोषित CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#पूर्ण_अगर
                              /* vspace_root: reference to vspace root page table object */
                              (word_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#पूर्ण_अगर
                          );
    ap->array[ASID_निचला(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_ऊपरी(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t findMapForASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ऊपरी(asid)];
    अगर (!poolPtr) {
        वापस asid_map_asid_map_none_new();
    }

    वापस poolPtr->array[ASID_निचला(asid)];
}

अटल findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    अगर (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_मूल_new();

        ret.vspace_मूल = शून्य;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        वापस ret;
    }

    ret.vspace_मूल = (vspace_मूल_t *)asid_map_asid_map_vspace_get_vspace_मूल(asid_map);
    ret.status = EXCEPTION_NONE;
    वापस ret;
}

word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_बफर_Ptr;
    cap_t बफर_Cap;
    vm_rights_t vm_rights;

    w_बफर_Ptr = thread->tcbIPCBuffer;
    बफर_Cap = TCB_PTR_CTE_PTR(thread, tcb_बफर)->cap;

    अगर (unlikely(cap_get_capType(बफर_Cap) != cap_frame_cap)) {
        वापस शून्य;
    }
    अगर (unlikely(cap_frame_cap_get_capFIsDevice(बफर_Cap))) {
        वापस शून्य;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(बफर_Cap);
    अगर (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr;
        अचिन्हित पूर्णांक pageBits;

        basePtr = cap_frame_cap_get_capFBasePtr(बफर_Cap);
        pageBits = page_Bits_For_आकार(cap_frame_cap_get_capFSize(बफर_Cap));
        वापस (word_t *)(basePtr + (w_बफर_Ptr & MASK(pageBits)));
    } अन्यथा {
        वापस शून्य;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    अगर (cap_get_capType(cap) != cap_frame_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (!IS_ALIGNED(vptr, se_L4_IPCBuffer_आकार_Bits)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (पूर्णांक)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    वापस EXCEPTION_NONE;
}

अटल lookupPTSlot_ret_t lookupPTSlot(vspace_मूल_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    word_t level = UPT_LEVELS - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_Bits_बायाँ = PT_सूचकांक_BITS * level + seL4_PageBits;
    ret.ptSlot = pt + ((vptr >> ret.pt_Bits_बायाँ) & MASK(se_L4_VSpace_सूचकांक_Bits));

    जबतक (pte_pte_table_ptr_get_present(ret.ptSlot) && likely(level > 0)) {
        level--;
        ret.pt_Bits_बायाँ -= PT_सूचकांक_BITS;
        pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ret.ptSlot));
        ret.ptSlot = pt + ((vptr >> ret.pt_Bits_बायाँ) & MASK(PT_सूचकांक_BITS));
    }

    वापस ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
अटल pte_t makeUserPagePTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_attributes_t attributes,
                             vm_page_आकार_t page_आकार)
{
    bool_t nonexecutable = vm_attributes_get_armExecuteNever(attributes) || !cap_read;
    word_t cacheable = vm_attributes_get_armPageCacheable(attributes);

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t nG = 0; /* not global */
    word_t attridx = cacheable ? S2_NORMAL : S2_DEVICE_nGnRnE;
#अन्यथा
    word_t nG = 1; /* not global */
    word_t attridx = cacheable ? NORMAL : DEVICE_nGnRnE;
#पूर्ण_अगर

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    word_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    अगर (page_आकार == ARMSmallPage) {
        वापस pte_pte_4k_page_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } अन्यथा {
        वापस pte_pte_page_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    चयन (vm_faultType) {
    हाल ARMDataAbort: {
        word_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        अगर (ARCH_गाँठ_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(addressTranslateS1(addr)) | (addr & MASK(PAGE_BITS));
        }
#पूर्ण_अगर
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        वापस EXCEPTION_FAULT;
    }

    हाल ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        अगर (ARCH_गाँठ_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(addressTranslateS1(pc)) | (pc & MASK(PAGE_BITS));
        }
#पूर्ण_अगर
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        वापस EXCEPTION_FAULT;
    }

    शेष:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_मूल(cap_t cap)
{
    वापस cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_मूल(cap_t cap)
{
    वापस is_VTable_मूल(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_मूल(cap_t cap)
{
    वापस is_Valid_Native_मूल(cap);
}

व्योम setVMRoot(tcb_t *tcb)
{
    cap_t thread_मूल;
    asid_t asid;
    vspace_मूल_t *vspace_मूल_2;
    findVSpaceForASID_ret_t find_ret;

    thread_मूल = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    अगर (!is_Valid_Native_मूल(thread_मूल)) {
        set_Current_User_VSpace_मूल(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        वापस;
    }

    vspace_मूल_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_मूल));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_मूल);
    find_ret = findVSpaceForASID(asid);
    अगर (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.vspace_मूल != vspace_मूल_2)) {
        set_Current_User_VSpace_मूल(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        वापस;
    }

    armv_contextSwitch(vspace_मूल_2, asid);
}

अटल bool_t setVMRootForFlush(vspace_मूल_t *vspace, asid_t asid)
{
    cap_t thread_मूल;

    thread_मूल = TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbVTable)->cap;

    अगर (cap_get_capType(thread_मूल) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_मूल) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_मूल)) == vspace) {
        वापस false;
    }

    armv_contextSwitch(vspace, asid);
    वापस true;
}


#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT

अटल अंतरभूत asid_pool_t *getPoolPtr(asid_t asid)
{
    वापस armKSASIDTable[ASID_ऊपरी(asid)];
}

अटल अंतरभूत asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    निश्चित(poolPtr != शून्य);
    वापस poolPtr->array[ASID_निचला(asid)];
}

अटल अंतरभूत व्योम setASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    निश्चित(poolPtr != शून्य);
    poolPtr->array[ASID_निचला(asid)] = asid_map;
}

अटल व्योम invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    निश्चित(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_set_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_set_stored_vmid_valid(asid_map, false);

    setASIDMap(poolPtr, asid, asid_map);
}

अटल व्योम storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    निश्चित(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_set_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_set_stored_vmid_valid(asid_map, true);

    setASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

अटल hw_asid_t findFreeHWASID(व्योम)
{
    word_t hw_asid_offset;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    क्रम (hw_asid_offset = 0;
         hw_asid_offset <= (word_t)((hw_asid_t) - 1);
         hw_asid_offset++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_offset);
        अगर (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            वापस hw_asid;
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
    अगर (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    वापस hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    अगर (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        वापस asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } अन्यथा {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        वापस new_hw_asid;
    }
}

अटल व्योम invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    अगर (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#पूर्ण_अगर

#अगर_घोषित CONFIG_ARM_SMMU
अटल word_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ऊपरी(asid)];
    निश्चित(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_निचला(asid)];
    निश्चित(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    वापस asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

व्योम increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ऊपरी(asid)];
    निश्चित(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_निचला(asid)];
    निश्चित(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_set_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

व्योम decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ऊपरी(asid)];
    निश्चित(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_निचला(asid)];
    निश्चित(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_set_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#पूर्ण_अगर

अटल अंतरभूत व्योम invalidateTLBByASID(asid_t asid)
{
#अगर_घोषित CONFIG_ARM_SMMU
    word_t bind_cb = getASIDBindCB(asid);
    अगर (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#पूर्ण_अगर
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    अगर (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        वापस;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#अन्यथा
    invalidateTranslationASID(asid);
#पूर्ण_अगर
}

अटल अंतरभूत व्योम invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#अगर_घोषित CONFIG_ARM_SMMU
    word_t bind_cb = getASIDBindCB(asid);
    अगर (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#पूर्ण_अगर
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    अगर (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        वापस;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> seL4_PageBits);
#अन्यथा
    invalidateTranslationSingle((asid << 48) | vaddr >> seL4_PageBits);
#पूर्ण_अगर
}


व्योम unmapPageTable(asid_t asid, vptr_t vptr, pte_t *target_pt)
{
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
        /* nothing to do */
        वापस;
    }
    pte_t *ptSlot = शून्य;
    pte_t *pt = (pte_t *)find_ret.vspace_मूल;

    क्रम (word_t i = 0; i < UPT_LEVELS - 1 && pt != target_pt; i++) {
        ptSlot = pt + GET_UPT_सूचकांक(vptr, i);
        अगर (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
            /* couldn't find it */
            वापस;
        }
        pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
    }

    अगर (pt != target_pt) {
        /* didn't find it */
        वापस;
    }
    /* If we found a pt then ptSlot won't be null */
    निश्चित(ptSlot != शून्य);
    *ptSlot = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));
    invalidateTLBByASID(asid);
}

व्योम unmapPage(vm_page_आकार_t page_आकार, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    find_ret = findVSpaceForASID(asid);
    अगर (find_ret.status != EXCEPTION_NONE) {
        वापस;
    }

    lu_ret = lookupPTSlot(find_ret.vspace_मूल, vptr);
    अगर (unlikely(lu_ret.pt_Bits_बायाँ != page_Bits_For_आकार(page_आकार))) {
        /* Do nothing if the wrong size object was returned */
        वापस;
    }

    pte = *(lu_ret.ptSlot);
    अगर (!pte_is_page_type(pte)) {
        /* Do nothing if no page is present */
        वापस;
    }

    अगर (pte_get_page_base_address(pte) != pptr_to_paddr((व्योम *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        वापस;
    }

    *(lu_ret.ptSlot) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.ptSlot, pptr_to_paddr(lu_ret.ptSlot));
    निश्चित(asid < BIT(16));
    invalidateTLBByASIDVA(asid, vptr);
}

व्योम deleteASID(asid_t asid, vspace_मूल_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ऊपरी(asid)];

    अगर (poolPtr != शून्य) {
        asid_map_t asid_map = poolPtr->array[ASID_निचला(asid)];
        अगर (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_मूल_t *)asid_map_asid_map_vspace_get_vspace_मूल(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#पूर्ण_अगर
            poolPtr->array[ASID_निचला(asid)] = asid_map_asid_map_none_new();
            setVMRoot(गाँठ_STATE(ksCurThread));
        }
    }
}

व्योम deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    word_t offset;

    निश्चित((asid_base & MASK(asid_निचला_Bits)) == 0);

    अगर (armKSASIDTable[ASID_ऊपरी(asid_base)] == pool) {
        क्रम (offset = 0; offset < BIT(asid_निचला_Bits); offset++) {
            asid_map_t asid_map = pool->array[offset];
            अगर (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_base + offset);
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_base + offset);
#पूर्ण_अगर
            }
        }
        armKSASIDTable[ASID_ऊपरी(asid_base)] = शून्य;
        setVMRoot(गाँठ_STATE(ksCurThread));
    }
}

अटल व्योम doFlush(word_t invLabel, vptr_t start, vptr_t end, paddr_t pstart)
{
    चयन (invLabel) {
    हाल ARMVSpace_Clean_तथ्य:
    हाल ARMPage_Clean_तथ्य:
        cleanCacheRange_RAM(start, end, pstart);
        अवरोध;

    हाल ARMVSpace_Invalidate_तथ्य:
    हाल ARMPage_Invalidate_तथ्य:
        invalidateCacheRange_RAM(start, end, pstart);
        अवरोध;

    हाल ARMVSpace_Clean_Invalidate_तथ्य:
    हाल ARMPage_Clean_Invalidate_तथ्य:
        cleanInvalidateCacheRange_RAM(start, end, pstart);
        अवरोध;

    हाल ARMVSpaceUnify_Instruction:
    हाल ARMPageUnify_Instruction:
        /* First clean data lines to point of unification... */
        cleanCacheRange_PoU(start, end, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(start, end, pstart);
        /* ... then wait for the completion of invalidations... */
        dsb();
        /* ... and ensure new instructions come from fresh cache lines. */
        isb();
        अवरोध;
    शेष:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

अटल exception_t performVSpaceFlush(word_t invLabel, vspace_मूल_t *vspace_मूल_2, asid_t asid,
                                      vptr_t start, vptr_t end, paddr_t pstart)
{

    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        word_t आकार = end - start;
        start = (vptr_t)paddr_to_pptr(pstart);
        end = start + आकार;
        अगर (start < end) {
            doFlush(invLabel, start, end, pstart);
        }
    } अन्यथा {
        bool_t मूल_switched;

        /* Flush if given a non zero range */
        अगर (start < end) {
            मूल_switched = setVMRootForFlush(vspace_मूल_2, asid);
            doFlush(invLabel, start, end, pstart);
            अगर (मूल_switched) {
                setVMRoot(गाँठ_STATE(ksCurThread));
            }
        }
    }
    वापस EXCEPTION_NONE;
}


अटल exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot, pte_t pte, pte_t *ptSlot)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));

    वापस EXCEPTION_NONE;
}

अटल exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    अगर (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                       cap_page_table_cap_get_capPTMappedAddress(cap), pt);
        clearMemory_PT((व्योम *)pt, cap_get_cap_आकार_Bits(cap));
    }

    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);
    वापस EXCEPTION_NONE;
}

अटल exception_t performPageInvocationMap(asid_t asid, cap_t cap, cte_t *ctSlot,
                                            pte_t pte, pte_t *ptSlot)
{
    bool_t tlbflush_required = pte_ptr_get_valid(ptSlot);

    ctSlot->cap = cap;
    *ptSlot = pte;

    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));
    अगर (unlikely(tlbflush_required)) {
        निश्चित(asid < BIT(16));
        invalidateTLBByASIDVA(asid, cap_frame_cap_get_capFMappedAddress(cap));
    }

    वापस EXCEPTION_NONE;
}

अटल exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    अगर (cap_frame_cap_get_capFMappedASID(cap) != 0) {

        unmapPage(cap_frame_cap_get_capFSize(cap),
                  cap_frame_cap_get_capFMappedASID(cap),
                  cap_frame_cap_get_capFMappedAddress(cap),
                  cap_frame_cap_get_capFBasePtr(cap));
    }

    cap_t slotCap = ctSlot->cap;
    slotCap = cap_frame_cap_set_capFMappedAddress(slotCap, 0);
    slotCap = cap_frame_cap_set_capFMappedASID(slotCap, asidInvalid);
    ctSlot->cap = slotCap;


    वापस EXCEPTION_NONE;
}

अटल exception_t performPageFlush(word_t invLabel, vspace_मूल_t *vspace_मूल_2, asid_t asid,
                                    vptr_t start, vptr_t end, paddr_t pstart)
{
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        word_t आकार = end - start;
        start = (vptr_t)paddr_to_pptr(pstart);
        end = start + आकार;

        अगर (start < end) {
            doFlush(invLabel, start, end, pstart);
        }
    } अन्यथा {
        bool_t मूल_switched;

        अगर (start < end) {
            मूल_switched = setVMRootForFlush(vspace_मूल_2, asid);
            doFlush(invLabel, start, end, pstart);
            अगर (मूल_switched) {
                setVMRoot(गाँठ_STATE(ksCurThread));
            }
        }
    }
    वापस EXCEPTION_NONE;
}

अटल exception_t performPageGetAddress(pptr_t base_ptr, bool_t call)
{
    paddr_t base = pptr_to_paddr((व्योम *)base_ptr);

    tcb_t *thread;
    thread = गाँठ_STATE(ksCurThread);
    अगर (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        अचिन्हित पूर्णांक लंबाई = setMR(thread, ipc_बफर, 0, base);
        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, लंबाई)));
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
    वापस EXCEPTION_NONE;
}

अटल exception_t performASIDControlInvocation(व्योम *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_base)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_cap_Free_सूचकांक(&(parent->cap),
                                         MAX_FREE_सूचकांक(cap_untyped_cap_get_cap_Block_आकार(parent->cap)));

    memzero(frame, BIT(seL4_ASIDPoolBits));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_जोड़ो(
        cap_asid_pool_cap_new(
            asid_base,         /* capASIDBase  */
            WORD_REF(frame)    /* capASIDPool  */
        ), parent, slot);

    निश्चित((asid_base & MASK(asid_निचला_Bits)) == 0);
    armKSASIDTable[ASID_ऊपरी(asid_base)] = (asid_pool_t *)frame;

    वापस EXCEPTION_NONE;
}

अटल exception_t decode_ARMVSpace_मूल_Invocation(word_t invLabel, word_t लंबाई,
                                                 cte_t *cte, cap_t cap, word_t *बफर)
{
    vptr_t start, end;
    paddr_t pstart;
    asid_t asid;
    vspace_मूल_t *vspace_मूल_2;
    lookupPTSlot_ret_t resolve_ret;
    findVSpaceForASID_ret_t find_ret;
    pte_t pte;

    चयन (invLabel) {
    हाल ARMVSpace_Clean_तथ्य:
    हाल ARMVSpace_Invalidate_तथ्य:
    हाल ARMVSpace_Clean_Invalidate_तथ्य:
    हाल ARMVSpaceUnify_Instruction:

        अगर (लंबाई < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, बफर);
        end =   getSyscallArg(1, बफर);

        /* Check sanity of arguments */
        अगर (end <= start) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 1;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        अगर (end > USER_शीर्ष) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(!is_Valid_Native_मूल(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_मूल_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        find_ret = findVSpaceForASID(asid);
        अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(find_ret.vspace_मूल != vspace_मूल_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_मूल_2, start);
        pte = *resolve_ret.ptSlot;

        /* Check that the returned slot is a page. */
        अगर (!pte_is_page_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
            वापस EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        अगर (ROUND_DOWN(start, resolve_ret.pt_Bits_बायाँ) != ROUND_DOWN(end - 1, resolve_ret.pt_Bits_बायाँ)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax = ROUND_DOWN(start, resolve_ret.pt_Bits_बायाँ) +
                                                  MASK(resolve_ret.pt_Bits_बायाँ);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            वापस EXCEPTION_SYSCALL_ERROR;
        }

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        अगर (invLabel == ARMVSpace_Invalidate_तथ्य && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#पूर्ण_अगर

        /* Calculate the physical start address. */
        paddr_t frame_base = pte_get_page_base_address(pte);

        pstart = frame_base + (start & MASK(resolve_ret.pt_Bits_बायाँ));

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        अगर (pstart < PADDR_BASE || ((end - start) + pstart) > PADDR_शीर्ष) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#पूर्ण_अगर

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performVSpaceFlush(invLabel, vspace_मूल_2, asid, start, end - 1, pstart);

    शेष:
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
}


अटल exception_t decodeARMPageTableInvocation(word_t invLabel, word_t लंबाई,
                                                cte_t *cte, cap_t cap, word_t *बफर)
{
    cap_t vspace_मूल_Cap;
    vspace_मूल_t *vspace_मूल_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t ptSlot;
    findVSpaceForASID_ret_t find_ret;

    अगर (invLabel == ARMPageTableUnmap) {
        अगर (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = seL4_RevokeFirst;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageTableInvocationUnmap(cap, cte);
    }

    अगर (unlikely(invLabel != ARMPageTableMap)) {
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(लंबाई < 2 || current_extra_caps.excaprefs[0] == शून्य)) {
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, बफर);
    vspace_मूल_Cap = current_extra_caps.excaprefs[0]->cap;

    अगर (unlikely(!is_Valid_Native_मूल(vspace_मूल_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    vspace_मूल_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_मूल_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_मूल_Cap);

    अगर (unlikely(vaddr > USER_शीर्ष)) {
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    find_ret = findVSpaceForASID(asid);
    अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(find_ret.vspace_मूल != vspace_मूल_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    ptSlot = lookupPTSlot(vspace_मूल_2, vaddr);

    अगर (unlikely(ptSlot.pt_Bits_बायाँ == seL4_PageBits || pte_ptr_get_valid(ptSlot.ptSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_table_new(pptr_to_paddr(PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap))));

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, (vaddr & ~MASK(ptSlot.pt_Bits_बायाँ)));

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस performPageTableInvocationMap(cap, cte, pte, ptSlot.ptSlot);
}

अटल अंतरभूत bool_t CONST checkVPAlignment(vm_page_आकार_t sz, word_t w)
{
    वापस (w & MASK(page_Bits_For_आकार(sz))) == 0;
}

अटल exception_t decodeARMFrameInvocation(word_t invLabel, word_t लंबाई,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    चयन (invLabel) {
    हाल ARMPageMap: {
        vptr_t vaddr;
        paddr_t base;
        cap_t vspace_मूल_Cap;
        vspace_मूल_t *vspace_मूल_2;
        asid_t asid, frame_asid;
        vm_rights_t vmRights, capRights;
        vm_page_आकार_t frame_आकार;
        vm_attributes_t attributes;
        findVSpaceForASID_ret_t find_ret;

        अगर (unlikely(लंबाई < 3 || current_extra_caps.excaprefs[0] == शून्य)) {
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, बफर);
        attributes = vmAttributesFromWord(getSyscallArg(2, बफर));
        vspace_मूल_Cap = current_extra_caps.excaprefs[0]->cap;

        frame_आकार = cap_frame_cap_get_capFSize(cap);
        capRights = cap_frame_cap_get_capFVMRights(cap);
        vmRights = maskVMRights(capRights, rightsFromWord(getSyscallArg(1, बफर)));

        अगर (unlikely(!is_Valid_Native_मूल(vspace_मूल_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        vspace_मूल_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_मूल_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_मूल_Cap);

        find_ret = findVSpaceForASID(asid);
        अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(find_ret.vspace_मूल != vspace_मूल_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(!checkVPAlignment(frame_आकार, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        frame_asid = cap_frame_cap_get_capFMappedASID(cap);

        अगर (frame_asid != asidInvalid) {
            अगर (frame_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_संख्या = 1;
                वापस EXCEPTION_SYSCALL_ERROR;

            } अन्यथा अगर (cap_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_संख्या = 0;
                वापस EXCEPTION_SYSCALL_ERROR;
            }
        } अन्यथा {
            अगर (unlikely(vaddr + BIT(page_Bits_For_आकार(frame_आकार)) - 1 > USER_शीर्ष)) {
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_संख्या = 0;
                वापस EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_frame_cap_set_capFMappedASID(cap, asid);
        cap = cap_frame_cap_set_capFMappedAddress(cap, vaddr);

        base = pptr_to_paddr((व्योम *)cap_frame_cap_get_capFBasePtr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_मूल_2, vaddr);
        अगर (unlikely(lu_ret.pt_Bits_बायाँ != page_Bits_For_आकार(frame_आकार))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_Bits_बायाँ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = makeUserPagePTE(base, vmRights, cap_read, attributes, frame_आकार);

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageInvocationMap(asid, cap, cte, pte, lu_ret.ptSlot);
    }

    हाल ARMPageUnmap:
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageInvocationUnmap(cap, cte);

    हाल ARMPage_Clean_तथ्य:
    हाल ARMPage_Invalidate_तथ्य:
    हाल ARMPage_Clean_Invalidate_तथ्य:
    हाल ARMPageUnify_Instruction: {
        vptr_t start, end;
        vptr_t vaddr;
        asid_t asid;
        word_t page_आकार;
        findVSpaceForASID_ret_t find_ret;

        अगर (लंबाई < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(cap_frame_cap_get_capFMappedASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_frame_cap_get_capFMappedAddress(cap);
        asid = cap_frame_cap_get_capFMappedASID(cap);

        find_ret = findVSpaceForASID(asid);
        अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, बफर);
        end =   getSyscallArg(1, बफर);

        /* check that the range is sane */
        अगर (end <= start) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 1;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        page_आकार = BIT(page_Bits_For_आकार(cap_frame_cap_get_capFSize(cap)));
        अगर (start >= page_आकार || end > page_आकार) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        word_t pstart = pptr_to_paddr((व्योम *)cap_frame_cap_get_capFBasePtr(cap)) + start;
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        अगर (pstart < PADDR_BASE || ((end - start) + pstart) > PADDR_शीर्ष) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#अन्यथा
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(find_ret.vspace_मूल, vaddr);
        pte_t pte = *lu_ret.ptSlot;
        व्योम *base_ptr = (व्योम *) cap_frame_cap_get_capFBasePtr(cap);
        अगर (unlikely(lu_ret.pt_Bits_बायाँ != page_Bits_For_आकार(cap_frame_cap_get_capFSize(cap)) ||
                     !pte_is_page_type(pte) ||
                     pte_get_page_base_address(pte) != pptr_to_paddr(base_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        अगर (invLabel == ARMPage_Invalidate_तथ्य && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#पूर्ण_अगर
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageFlush(invLabel, find_ret.vspace_मूल, asid, vaddr + start, vaddr + end - 1,
                                pstart);
    }

    हाल ARMPageGetAddress:
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageGetAddress(cap_frame_cap_get_capFBasePtr(cap), call);

    शेष:
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t लंबाई, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_vspace_cap:
        वापस decode_ARMVSpace_मूल_Invocation(invLabel, लंबाई, cte, cap, बफर);
    हाल cap_page_table_cap:
        वापस decodeARMPageTableInvocation(invLabel, लंबाई, cte, cap, बफर);

    हाल cap_frame_cap:
        वापस decodeARMFrameInvocation(invLabel, लंबाई, cte, cap, call, बफर);

    हाल cap_asid_control_cap: {
        word_t i;
        asid_t asid_base;
        word_t सूचकांक, depth;
        cap_t untyped, मूल;
        cte_t *parentSlot, *destSlot;
        lookupSlot_ret_t lu_ret;
        व्योम *frame;
        exception_t status;

        अगर (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(लंबाई < 2 ||
                     current_extra_caps.excaprefs[0] == शून्य ||
                     current_extra_caps.excaprefs[1] == शून्य)) {
            current_syscall_error.type = seL4_TruncatedMessage;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        सूचकांक = getSyscallArg(0, बफर);
        depth = getSyscallArg(1, बफर);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        मूल = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        क्रम (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        अगर (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_निचला_Bits;

        अगर (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_Block_आकार(untyped) != seL4_ASIDPoolBits ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        अगर (unlikely(status != EXCEPTION_NONE)) {
            वापस status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(मूल, सूचकांक, depth);
        अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            वापस lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        अगर (unlikely(status != EXCEPTION_NONE)) {
            वापस status;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performASIDControlInvocation(frame, destSlot, parentSlot, asid_base);
    }

    हाल cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        अगर (unlikely(invLabel != ARMASIDPoolAssign)) {
            current_syscall_error.type = seL4_IllegalOperation;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(current_extra_caps.excaprefs[0] == शून्य)) {
            current_syscall_error.type = seL4_TruncatedMessage;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        अगर (unlikely(!is_VTable_मूल(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ऊपरी(cap_asid_pool_cap_get_capASIDBase(cap))];

        अगर (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_मूल_new();

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        क्रम (i = 0; i < (1 << asid_निचला_Bits) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        अगर (unlikely(i == 1 << asid_निचला_Bits)) {
            current_syscall_error.type = seL4_DeleteFirst;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }

    शेष:
        fail("Invalid ARM arch cap type");
    }
}

#अगर_घोषित CONFIG_DEBUG_BUILD
व्योम kernelPrefetchAbort(word_t pc) VISIBLE;
व्योम kernel_तथ्य_Abort(word_t pc) VISIBLE;

व्योम kernelPrefetchAbort(word_t pc)
{
    म_लिखो("\n\nKERNEL PREFETCH ABORT!\n");
    म_लिखो("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    म_लिखो("ESR (IFSR): 0x%"SEL4_PRIx_word"\n", getIFSR());
    halt();
}

व्योम kernel_तथ्य_Abort(word_t pc)
{
    म_लिखो("\n\nKERNEL DATA ABORT!\n");
    म_लिखो("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    म_लिखो("FAR: 0x%"SEL4_PRIx_word" ESR (DFSR): 0x%"SEL4_PRIx_word"\n",
           getFAR(), getDFSR());
    halt();
}
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */

#अगर_घोषित CONFIG_PRINTING
प्रकार काष्ठा readWordFromVSpace_ret {
    exception_t status;
    word_t मूल्य;
} readWordFromVSpace_ret_t;

अटल readWordFromVSpace_ret_t readWordFromVSpace(vspace_मूल_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    word_t offset;
    pptr_t kernel_vaddr;
    word_t *मूल्य;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    अगर (!pte_ptr_get_valid(lookup_ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(lookup_ret.ptSlot) && lookup_ret.pt_Bits_बायाँ > PAGE_BITS)) {
        ret.status = EXCEPTION_LOOKUP_FAULT;
        वापस ret;
    }

    offset = vaddr & MASK(lookup_ret.pt_Bits_बायाँ);
    kernel_vaddr = (word_t)paddr_to_pptr(pte_page_ptr_get_page_base_address(lookup_ret.ptSlot));
    मूल्य = (word_t *)(kernel_vaddr + offset);

    ret.status = EXCEPTION_NONE;
    ret.मूल्य = *मूल्य;
    वापस ret;
}

व्योम Arch_user_ढेर_Trace(tcb_t *tptr)
{
    cap_t thread_मूल;
    vspace_मूल_t *vspace_मूल_2;
    word_t sp;

    thread_मूल = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    अगर (cap_get_capType(thread_मूल) != cap_vspace_cap) {
        म_लिखो("Invalid vspace\n");
        वापस;
    }

    vspace_मूल_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_मूल));
    sp = getRegister(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    अगर (!IS_ALIGNED(sp, se_L4_Word_आकार_Bits)) {
        म_लिखो("SP not aligned\n");
        वापस;
    }

    क्रम (अचिन्हित पूर्णांक i = 0; i < CONFIG_USER_ढेर_TRACE_लंबाई; i++) {
        word_t address = sp + (i * माप(word_t));
        readWordFromVSpace_ret_t परिणाम = readWordFromVSpace(vspace_मूल_2,
                                                             address);
        अगर (परिणाम.status == EXCEPTION_NONE) {
            म_लिखो("0x%"SEL4_PRIx_word": 0x%"SEL4_PRIx_word"\n",
                   address, परिणाम.मूल्य);
        } अन्यथा {
            म_लिखो("0x%"SEL4_PRIx_word": INVALID\n", address);
        }
    }
}
#पूर्ण_अगर /* CONFIG_PRINTING */

#अगर defined(CONFIG_KERNEL_LOG_बफर)
exception_t benchmark_arch_map_log_बफर(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_आकार_t frame_आकार;
    pptr_t  frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(गाँठ_STATE(ksCurThread), frame_cptr);

    अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    frame_आकार = cap_frame_cap_get_capFSize(lu_ret.cap);

    अगर (frame_आकार != ARMLargePage) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ks_User_Log_बफर = pptr_to_paddr((व्योम *) frame_pptr);

    *armKSGlobalLogPTE = pte_pte_page_new(
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#अन्यथा
                             1, // UXN
#पूर्ण_अगर
                             ks_User_Log_बफर,
                             0,                         /* global */
                             1,                         /* access flag */
                             SMP_TERNARY(SMP_SHARE, 0), /* Inner-shareable if SMP enabled, otherwise unshared */
                             0,                         /* VMKernelOnly */
                             NORMAL_WT);

    cleanByVA_PoU((vptr_t)armKSGlobalLogPTE, addrFromKPPtr(armKSGlobalLogPTE));
    invalidateTranslationSingle(KS_LOG_PPTR);
    वापस EXCEPTION_NONE;
}
#पूर्ण_अगर /* CONFIG_KERNEL_LOG_BUFFER */
