/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/stack.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <machine/debug.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <object/untyped.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <armv/context_switch.h>
#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/tlb.h>

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
ಗಣನಾ mair_types {
    DEVICE_nGnRnE = 0,
    DEVICE_nGnRE = 1,
    DEVICE_GRE = 2,
    NORMAL_NC = 3,
    NORMAL = 4,
    NORMAL_WT = 5
};

/* Stage-2 translation memory attributes */
ಗಣನಾ mair_s2_types {
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

#define SMP_SHARE   3

ಸಂರಚನಾ lookupPTSlot_ret {
    pte_t *ptSlot;
    word_t pt_Bits_ಎಡ;
};
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ lookupPTSlot_ret lookupPTSlot_ret_t;

ಸಂರಚನಾ findVSpaceForASID_ret {
    exception_t status;
    vspace_ಬೇರು_t *vspace_ಬೇರು;
};
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ findVSpaceForASID_ret findVSpaceForASID_ret_t;

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

ಸ್ಥಿರ word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ವಿಕಲ್ಪ (vm_rights) {
    ಪಕ್ಷ VMKernelOnly:
        ಯದಿ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ 0;
        }

    ಪಕ್ಷ VMReadWrite:
        ಯದಿ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            ಪ್ರತ್ಯಾಗಮ 3;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ 1;
        }

    ಪಕ್ಷ VMReadOnly:
        ಯದಿ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            ಪ್ರತ್ಯಾಗಮ 1;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ 3;
        }

    ಸ್ವಭಾವ:
        fail("Invalid VM rights");
    }
}

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ CONST word_t pte_get_AP(pte_t pte)
{
    ಪ್ರತಿಜ್ಞಾ(pte_is_page_type(pte));
    ವಿಕಲ್ಪ (pte_get_pte_type(pte)) {
    ಪಕ್ಷ pte_pte_4k_page:
        ಪ್ರತ್ಯಾಗಮ pte_pte_4k_page_get_AP(pte);

    ಪಕ್ಷ pte_pte_page:
        ಪ್ರತ್ಯಾಗಮ pte_pte_page_get_AP(pte);

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}

ಸ್ಥಿರ vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    word_t access_perms = pte_get_AP(pte);
    ವಿಕಲ್ಪ (access_perms) {
    ಪಕ್ಷ 0:
    ಪಕ್ಷ 2:
        ಪ್ರತ್ಯಾಗಮ VMKernelOnly;
    ಪಕ್ಷ 1:
        ಪ್ರತ್ಯಾಗಮ VMReadWrite;
    ಪಕ್ಷ 3:
        ಪ್ರತ್ಯಾಗಮ VMReadOnly;
    ಸ್ವಭಾವ:
        fail("Invalid AP bit");
    }
}
#endif

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    ಯದಿ (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        ಪ್ರತ್ಯಾಗಮ VMReadOnly;
    }
    ಯದಿ (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        ಯದಿ (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            ಪ್ರತ್ಯಾಗಮ VMReadOnly;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ VMReadWrite;
        }
    }
    ಯದಿ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    ಪ್ರತ್ಯಾಗಮ VMKernelOnly;
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
BOOT_CODE ಶೂನ್ಯ map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_attributes_t attributes)
{
    ಪ್ರತಿಜ್ಞಾ(vaddr >= PPTR_ತುದಿ);

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t uxn = vm_attributes_get_armExecuteNever(attributes);
#ಅನ್ಯಥಾ
    word_t uxn = 1; /* unprivileged execute never */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    word_t attr_index;
    word_t shareable;
    ಯದಿ (vm_attributes_get_armPageCacheable(attributes)) {
        attr_index = NORMAL;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } ಅನ್ಯಥಾ {
        attr_index = DEVICE_nGnRnE;
        shareable = 0;
    }
    armKSGlobalKernelPT[GET_KPT_INDEX(vaddr, KLVL_FRM_ARM_PT_LVL(3))] = pte_pte_4k_page_new(uxn, paddr,
                                                                                            0, /* global */
                                                                                            1, /* access flag */
                                                                                            shareable,
                                                                                            APFromVMRights(vm_rights),
                                                                                            attr_index);
}

/* Essentially, assert that the ELF, kernel devices, and log buffer are all in the same PUD.
   In the current code this is the top index. This is arbitrary, the important part
   is that they are all the same.
 */
compile_assert(elf_in_kernel_mappings, GET_KPT_INDEX(KERNEL_ELF_BASE,
                                                     KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_INDEX_BITS) - 1);
compile_assert(kdev_in_kernel_mappings, GET_KPT_INDEX(KDEV_BASE,
                                                      KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_INDEX_BITS) - 1);
compile_assert(log_buf_in_kernel_mappings, GET_KPT_INDEX(KS_LOG_BASE,
                                                         KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_INDEX_BITS) - 1);

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
BOOT_CODE ಶೂನ್ಯ map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_BASE_RAW, seL4_LargePageBits));
    compile_assert(elf_base_aligned, IS_ALIGNED(KERNEL_ELF_BASE, seL4_LargePageBits));

    /* Please don't overlap with KDEV_BASE */
    ಪ್ರತಿಜ್ಞಾ(KERNEL_ELF_ತುದಿ <= KDEV_BASE);
    /* Please don't overlap with KS_LOG_PPTR */
    ಪ್ರತಿಜ್ಞಾ(KERNEL_ELF_ತುದಿ <= KS_LOG_PPTR);

    ಧ್ರುವ word_t pd_start = GET_KPT_INDEX(KERNEL_ELF_BASE, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    ಧ್ರುವ word_t pd_end   = GET_KPT_INDEX(KERNEL_ELF_ತುದಿ - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_BASE;
    UNUSED pptr_t vaddr = KERNEL_ELF_BASE;

    ಪ್ರತಿ (word_t pd = pd_start; pd < pd_end; pd++) {
        ಪ್ರತಿಜ್ಞಾ(pd == GET_KPT_INDEX(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_page_new(
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#ಅನ್ಯಥಾ
                                     /* UXN */ 1,
#endif
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

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
/* verify that the physical memory window as at the second entry of the PGD */
compile_assert(physical_window_pgd_location,
               GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(0)) == 1);
#ಅನ್ಯಥಾ
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_window_pgd_location,
               GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(0)) == BIT(PT_INDEX_BITS) - 1);
#endif
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_window_base_aligned, IS_ALIGNED(PPTR_BASE, seL4_HugePageBits));
compile_assert(physical_window_ತುದಿ_aligned, IS_ALIGNED(PPTR_ತುದಿ, seL4_HugePageBits));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_window_bottom_kernel_region,
               GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_window_ತುದಿ_kernel_region,
               GET_KPT_INDEX(PPTR_ತುದಿ, KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_INDEX_BITS) - 1);

BOOT_CODE ಶೂನ್ಯ map_kernel_physical_window(ಶೂನ್ಯ)
{
    paddr_t paddr;
    pptr_t vaddr;
    word_t idx;

    /* place all PDs except the last one in PUD */
    ಪ್ರತಿ (idx = GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_INDEX(PPTR_ತುದಿ, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_table_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_BASE;
    ಪ್ರತಿ (paddr = PADDR_BASE; paddr < PADDR_ತುದಿ; paddr += BIT(seL4_LargePageBits)) {
        armKSGlobalKernelPDs[GET_KPT_INDEX(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_INDEX(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_page_new(
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#ಅನ್ಯಥಾ
                                                                                                                        1, // UXN
#endif
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

BOOT_CODE ಶೂನ್ಯ map_kernel_window(ಶೂನ್ಯ)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_table_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_window();

    ಧ್ರುವ word_t elf_pud_idx = GET_KPT_INDEX(KERNEL_ELF_BASE, KLVL_FRM_ARM_PT_LVL(1));
    pte_t *kernel_mapping_pds = armKSGlobalKernelPDs[elf_pud_idx];

    /* place the kernel mapping PDs in the kernel mapping PUD */
    armKSGlobalKernelPUD[elf_pud_idx] = pte_pte_table_new(addrFromKPPtr(kernel_mapping_pds));

    /* map the kernel ELF */
    map_kernel_elf_image(kernel_mapping_pds);

    /* put the PT into the PD for device window. It would be nice if
       we could pass kernel_mapping_pds[511] to map_kernel_devices(), but it is
       shared between AArch64 and AArch32, so... */
    kernel_mapping_pds[BIT(PT_INDEX_BITS) - 1]
        = pte_pte_table_new(addrFromKPPtr(armKSGlobalKernelPT));

    map_kernel_devices();
}

/* When the hypervisor support is enabled, the stage-2 translation table format
 * is used for applications.
 * The global bit is always 0.
 * The memory attributes use the S2 translation values.
 */
ಸ್ಥಿರ BOOT_CODE ಶೂನ್ಯ map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap, bool_t executable)
{
    vspace_ಬೇರು_t *vspace_ಬೇರು_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);
    ಶೂನ್ಯ *pptr = (ಶೂನ್ಯ *)cap_frame_cap_get_capFBasePtr(frame_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_frame_cap_get_capFMappedASID(frame_cap) != 0);

#ifdef AARCH64_VSPACE_S2_START_L1
    pud = vspace_ಬೇರು_2;
#ಅನ್ಯಥಾ
    vspace_ಬೇರು_2 += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_table_ptr_get_present(vspace_ಬೇರು_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_ಬೇರು_2));
#endif
    pud += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_table_ptr_get_present(pud));
    pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pud));
    pd += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_table_ptr_get_present(pd));
    pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pd));
    *(pt + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_page_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_to_paddr(pptr),            /* page_base_address    */
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#ಅನ್ಯಥಾ
                                                              1,                              /* not global */
#endif
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_NORMAL
#ಅನ್ಯಥಾ
                                                              NORMAL
#endif
                                                          );
}

ಸ್ಥಿರ BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_page_size_t frame_size;
    ಯದಿ (use_large) {
        frame_size = ARMLargePage;
    } ಅನ್ಯಥಾ {
        frame_size = ARMSmallPage;
    }
    ಪ್ರತ್ಯಾಗಮ
        cap_frame_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            frame_size,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            wordFromVMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

ಸ್ಥಿರ BOOT_CODE ಶೂನ್ಯ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_ಬೇರು_t *vspace_ಬೇರು_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_page_table_cap_get_capPTIsMapped(pt_cap));

#ifdef AARCH64_VSPACE_S2_START_L1
    pud = vspace_ಬೇರು_2;
#ಅನ್ಯಥಾ
    vspace_ಬೇರು_2 += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_table_ptr_get_present(vspace_ಬೇರು_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_ಬೇರು_2));
#endif
    pud += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_table_ptr_get_present(pud));
    pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pud));
    *(pd + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_table_new(
                                                              pptr_to_paddr(pt)
                                                          );
}

ಸ್ಥಿರ BOOT_CODE cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    ಪ್ರತ್ಯಾಗಮ cap;
}

ಸ್ಥಿರ BOOT_CODE ಶೂನ್ಯ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_ಬೇರು_t *vspace_ಬೇರು_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pd_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pd_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_page_table_cap_get_capPTIsMapped(pd_cap));

#ifdef AARCH64_VSPACE_S2_START_L1
    pud = vspace_ಬೇರು_2;
#ಅನ್ಯಥಾ
    vspace_ಬೇರು_2 += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0));
    ಪ್ರತಿಜ್ಞಾ(pte_pte_table_ptr_get_present(vspace_ಬೇರು_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_ಬೇರು_2));
#endif
    *(pud + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_table_new(
                                                               pptr_to_paddr(pd)
                                                           );
}

ಸ್ಥಿರ BOOT_CODE cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    ಪ್ರತ್ಯಾಗಮ cap;
}

#ifndef AARCH64_VSPACE_S2_START_L1
ಸ್ಥಿರ BOOT_CODE ಶೂನ್ಯ map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pud_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pud_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_page_table_cap_get_capPTIsMapped(pud_cap));

    *(pgd + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_table_new(
                                                               pptr_to_paddr(pud));
}

ಸ್ಥಿರ BOOT_CODE cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    ಪ್ರತ್ಯಾಗಮ cap;
}
#endif /* AARCH64_VSPACE_S2_START_L1 */
BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    ಪ್ರತ್ಯಾಗಮ
#ifndef AARCH64_VSPACE_S2_START_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(0))) +
#endif
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_CODE cap_t create_it_address_space(cap_t ಬೇರು_cnode_cap, v_region_t it_v_reg)
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
#ifdef CONFIG_ARM_SMMU
                     , CB_INVALID       /* capVSMappedCB   */
#endif
                 );
    slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_of_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#ifndef AARCH64_VSPACE_S2_START_L1
    /* Create any PUDs needed for the user land image */
    ಪ್ರತಿ (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        ಯದಿ (!provide_cap(ಬೇರು_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            ಪ್ರತ್ಯಾಗಮ cap_null_cap_new();
        }
    }
#endif
    /* Create any PDs needed for the user land image */
    ಪ್ರತಿ (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        ಯದಿ (!provide_cap(ಬೇರು_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            ಪ್ರತ್ಯಾಗಮ cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    ಪ್ರತಿ (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        ಯದಿ (!provide_cap(ಬೇರು_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            ಪ್ರತ್ಯಾಗಮ cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };
    ಪ್ರತ್ಯಾಗಮ vspace_cap;
}

BOOT_CODE cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    ಪ್ರತ್ಯಾಗಮ create_it_frame_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large);
    map_it_frame_cap(pd_cap, cap, executable);
    ಪ್ರತ್ಯಾಗಮ cap;
}

BOOT_CODE ಶೂನ್ಯ activate_kernel_vspace(ಶೂನ್ಯ)
{
    cleanInvalidateL1Caches();
    set_Current_Kernel_VSpace_ಬೇರು(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    set_Current_User_VSpace_ಬೇರು(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    lockTLBEntry(KERNEL_ELF_BASE);
}

BOOT_CODE ಶೂನ್ಯ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    asid_map_t asid_map = asid_map_asid_map_vspace_new(
#ifdef CONFIG_ARM_SMMU
                              /* bind_cb: Number of bound context banks */
                              0,
#endif
                              /* vspace_root: reference to vspace root page table object */
                              (word_t)cap_vspace_cap_get_capVSBasePtr(it_vspace_cap)
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                              /* stored_hw_vmid, stored_vmid_valid: Assigned hardware VMID for TLB. */
                              , 0, false
#endif
                          );
    ap->array[ASID_ಕೆಳಗಿನ(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_ಮೇಲಿನ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t findMapForASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಯದಿ (!poolPtr) {
        ಪ್ರತ್ಯಾಗಮ asid_map_asid_map_none_new();
    }

    ಪ್ರತ್ಯಾಗಮ poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
}

ಸ್ಥಿರ findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    ಯದಿ (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

        ret.vspace_ಬೇರು = ರಿಕ್ತ;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    ret.vspace_ಬೇರು = (vspace_ಬೇರು_t *)asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map);
    ret.status = EXCEPTION_NONE;
    ಪ್ರತ್ಯಾಗಮ ret;
}

word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_bufferPtr;
    cap_t bufferCap;
    vm_rights_t vm_rights;

    w_bufferPtr = thread->tcbIPCBuffer;
    bufferCap = TCB_PTR_CTE_PTR(thread, tcbBuffer)->cap;

    ಯದಿ (unlikely(cap_get_capType(bufferCap) != cap_frame_cap)) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (unlikely(cap_frame_cap_get_capFIsDevice(bufferCap))) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(bufferCap);
    ಯದಿ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr;
        unsigned ಪೂರ್ಣಾಙ್ಕ pageBits;

        basePtr = cap_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(cap_frame_cap_get_capFSize(bufferCap));
        ಪ್ರತ್ಯಾಗಮ (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ಯದಿ (cap_get_capType(cap) != cap_frame_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (!IS_ALIGNED(vptr, seL4_IPCBufferSizeBits)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (ಪೂರ್ಣಾಙ್ಕ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ lookupPTSlot_ret_t lookupPTSlot(vspace_ಬೇರು_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    word_t level = UPT_LEVELS - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_Bits_ಎಡ = PT_INDEX_BITS * level + seL4_PageBits;
    ret.ptSlot = pt + ((vptr >> ret.pt_Bits_ಎಡ) & MASK(seL4_VSpaceIndexBits));

    ಯಾವತ್ (pte_pte_table_ptr_get_present(ret.ptSlot) && likely(level > 0)) {
        level--;
        ret.pt_Bits_ಎಡ -= PT_INDEX_BITS;
        pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ret.ptSlot));
        ret.ptSlot = pt + ((vptr >> ret.pt_Bits_ಎಡ) & MASK(PT_INDEX_BITS));
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
ಸ್ಥಿರ pte_t makeUserPagePTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_attributes_t attributes,
                             vm_page_size_t page_size)
{
    bool_t nonexecutable = vm_attributes_get_armExecuteNever(attributes) || !cap_read;
    word_t cacheable = vm_attributes_get_armPageCacheable(attributes);

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t nG = 0; /* not global */
    word_t attridx = cacheable ? S2_NORMAL : S2_DEVICE_nGnRnE;
#ಅನ್ಯಥಾ
    word_t nG = 1; /* not global */
    word_t attridx = cacheable ? NORMAL : DEVICE_nGnRnE;
#endif

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    word_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    ಯದಿ (page_size == ARMSmallPage) {
        ಪ್ರತ್ಯಾಗಮ pte_pte_4k_page_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ pte_pte_page_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    ವಿಕಲ್ಪ (vm_faultType) {
    ಪಕ್ಷ ARMDataAbort: {
        word_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        ಯದಿ (ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(addressTranslateS1(addr)) | (addr & MASK(PAGE_BITS));
        }
#endif
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_FAULT;
    }

    ಪಕ್ಷ ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಯದಿ (ARCH_ಗಂಟು_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(addressTranslateS1(pc)) | (pc & MASK(PAGE_BITS));
        }
#endif
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_FAULT;
    }

    ಸ್ವಭಾವ:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_ಬೇರು(cap_t cap)
{
    ಪ್ರತ್ಯಾಗಮ cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_ಬೇರು(cap_t cap)
{
    ಪ್ರತ್ಯಾಗಮ is_VTable_ಬೇರು(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_ಬೇರು(cap_t cap)
{
    ಪ್ರತ್ಯಾಗಮ is_Valid_Native_ಬೇರು(cap);
}

ಶೂನ್ಯ setVMRoot(tcb_t *tcb)
{
    cap_t thread_ಬೇರು;
    asid_t asid;
    vspace_ಬೇರು_t *vspace_ಬೇರು_2;
    findVSpaceForASID_ret_t find_ret;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    ಯದಿ (!is_Valid_Native_ಬೇರು(thread_ಬೇರು)) {
        set_Current_User_VSpace_ಬೇರು(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        ಪ್ರತ್ಯಾಗಮ;
    }

    vspace_ಬೇರು_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ಬೇರು));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_ಬೇರು);
    find_ret = findVSpaceForASID(asid);
    ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.vspace_ಬೇರು != vspace_ಬೇರು_2)) {
        set_Current_User_VSpace_ಬೇರು(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        ಪ್ರತ್ಯಾಗಮ;
    }

    armv_contextSwitch(vspace_ಬೇರು_2, asid);
}

ಸ್ಥಿರ bool_t setVMRootForFlush(vspace_ಬೇರು_t *vspace, asid_t asid)
{
    cap_t thread_ಬೇರು;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbVTable)->cap;

    ಯದಿ (cap_get_capType(thread_ಬೇರು) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_ಬೇರು) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ಬೇರು)) == vspace) {
        ಪ್ರತ್ಯಾಗಮ false;
    }

    armv_contextSwitch(vspace, asid);
    ಪ್ರತ್ಯಾಗಮ true;
}


#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ asid_pool_t *getPoolPtr(asid_t asid)
{
    ಪ್ರತ್ಯಾಗಮ armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    ಪ್ರತಿಜ್ಞಾ(poolPtr != ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ setASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    ಪ್ರತಿಜ್ಞಾ(poolPtr != ರಿಕ್ತ);
    poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = asid_map;
}

ಸ್ಥಿರ ಶೂನ್ಯ invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    ಪ್ರತಿಜ್ಞಾ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_set_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_set_stored_vmid_valid(asid_map, false);

    setASIDMap(poolPtr, asid, asid_map);
}

ಸ್ಥಿರ ಶೂನ್ಯ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    ಪ್ರತಿಜ್ಞಾ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_set_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_set_stored_vmid_valid(asid_map, true);

    setASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

ಸ್ಥಿರ hw_asid_t findFreeHWASID(ಶೂನ್ಯ)
{
    word_t hw_asid_offset;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    ಪ್ರತಿ (hw_asid_offset = 0;
         hw_asid_offset <= (word_t)((hw_asid_t) - 1);
         hw_asid_offset++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_offset);
        ಯದಿ (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            ಪ್ರತ್ಯಾಗಮ hw_asid;
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
    ಯದಿ (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    ಪ್ರತ್ಯಾಗಮ hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    ಯದಿ (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        ಪ್ರತ್ಯಾಗಮ asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } ಅನ್ಯಥಾ {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        ಪ್ರತ್ಯಾಗಮ new_hw_asid;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    ಯದಿ (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#endif

#ifdef CONFIG_ARM_SMMU
ಸ್ಥಿರ word_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    ಪ್ರತ್ಯಾಗಮ asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

ಶೂನ್ಯ increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_set_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

ಶೂನ್ಯ decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_set_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#endif

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ invalidateTLBByASID(asid_t asid)
{
#ifdef CONFIG_ARM_SMMU
    word_t bind_cb = getASIDBindCB(asid);
    ಯದಿ (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    ಯದಿ (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#ಅನ್ಯಥಾ
    invalidateTranslationASID(asid);
#endif
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#ifdef CONFIG_ARM_SMMU
    word_t bind_cb = getASIDBindCB(asid);
    ಯದಿ (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    ಯದಿ (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> seL4_PageBits);
#ಅನ್ಯಥಾ
    invalidateTranslationSingle((asid << 48) | vaddr >> seL4_PageBits);
#endif
}


ಶೂನ್ಯ unmapPageTable(asid_t asid, vptr_t vptr, pte_t *target_pt)
{
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE)) {
        /* nothing to do */
        ಪ್ರತ್ಯಾಗಮ;
    }
    pte_t *ptSlot = ರಿಕ್ತ;
    pte_t *pt = (pte_t *)find_ret.vspace_ಬೇರು;

    ಪ್ರತಿ (word_t i = 0; i < UPT_LEVELS - 1 && pt != target_pt; i++) {
        ptSlot = pt + GET_UPT_INDEX(vptr, i);
        ಯದಿ (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
            /* couldn't find it */
            ಪ್ರತ್ಯಾಗಮ;
        }
        pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
    }

    ಯದಿ (pt != target_pt) {
        /* didn't find it */
        ಪ್ರತ್ಯಾಗಮ;
    }
    /* If we found a pt then ptSlot won't be null */
    ಪ್ರತಿಜ್ಞಾ(ptSlot != ರಿಕ್ತ);
    *ptSlot = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));
    invalidateTLBByASID(asid);
}

ಶೂನ್ಯ unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    find_ret = findVSpaceForASID(asid);
    ಯದಿ (find_ret.status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    lu_ret = lookupPTSlot(find_ret.vspace_ಬೇರು, vptr);
    ಯದಿ (unlikely(lu_ret.pt_Bits_ಎಡ != pageBitsForSize(page_size))) {
        /* Do nothing if the wrong size object was returned */
        ಪ್ರತ್ಯಾಗಮ;
    }

    pte = *(lu_ret.ptSlot);
    ಯದಿ (!pte_is_page_type(pte)) {
        /* Do nothing if no page is present */
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (pte_get_page_base_address(pte) != pptr_to_paddr((ಶೂನ್ಯ *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        ಪ್ರತ್ಯಾಗಮ;
    }

    *(lu_ret.ptSlot) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.ptSlot, pptr_to_paddr(lu_ret.ptSlot));
    ಪ್ರತಿಜ್ಞಾ(asid < BIT(16));
    invalidateTLBByASIDVA(asid, vptr);
}

ಶೂನ್ಯ deleteASID(asid_t asid, vspace_ಬೇರು_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];

    ಯದಿ (poolPtr != ರಿಕ್ತ) {
        asid_map_t asid_map = poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
        ಯದಿ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_ಬೇರು_t *)asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#endif
            poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = asid_map_asid_map_none_new();
            setVMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }
}

ಶೂನ್ಯ deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    word_t offset;

    ಪ್ರತಿಜ್ಞಾ((asid_base & MASK(asid_ಕೆಳಗಿನ_Bits)) == 0);

    ಯದಿ (armKSASIDTable[ASID_ಮೇಲಿನ(asid_base)] == pool) {
        ಪ್ರತಿ (offset = 0; offset < BIT(asid_ಕೆಳಗಿನ_Bits); offset++) {
            asid_map_t asid_map = pool->array[offset];
            ಯದಿ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_base + offset);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_base + offset);
#endif
            }
        }
        armKSASIDTable[ASID_ಮೇಲಿನ(asid_base)] = ರಿಕ್ತ;
        setVMRoot(ಗಂಟು_STATE(ksCurThread));
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ doFlush(word_t invLabel, vptr_t start, vptr_t end, paddr_t pstart)
{
    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ ARMVSpaceClean_Data:
    ಪಕ್ಷ ARMPageClean_Data:
        cleanCacheRange_RAM(start, end, pstart);
        ಭಙ್ಗ;

    ಪಕ್ಷ ARMVSpaceInvalidate_Data:
    ಪಕ್ಷ ARMPageInvalidate_Data:
        invalidateCacheRange_RAM(start, end, pstart);
        ಭಙ್ಗ;

    ಪಕ್ಷ ARMVSpaceCleanInvalidate_Data:
    ಪಕ್ಷ ARMPageCleanInvalidate_Data:
        cleanInvalidateCacheRange_RAM(start, end, pstart);
        ಭಙ್ಗ;

    ಪಕ್ಷ ARMVSpaceUnify_Instruction:
    ಪಕ್ಷ ARMPageUnify_Instruction:
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
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

ಸ್ಥಿರ exception_t performVSpaceFlush(word_t invLabel, vspace_ಬೇರು_t *vspace_ಬೇರು_2, asid_t asid,
                                      vptr_t start, vptr_t end, paddr_t pstart)
{

    ಯದಿ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        word_t size = end - start;
        start = (vptr_t)paddr_to_pptr(pstart);
        end = start + size;
        ಯದಿ (start < end) {
            doFlush(invLabel, start, end, pstart);
        }
    } ಅನ್ಯಥಾ {
        bool_t ಬೇರು_switched;

        /* Flush if given a non zero range */
        ಯದಿ (start < end) {
            ಬೇರು_switched = setVMRootForFlush(vspace_ಬೇರು_2, asid);
            doFlush(invLabel, start, end, pstart);
            ಯದಿ (ಬೇರು_switched) {
                setVMRoot(ಗಂಟು_STATE(ksCurThread));
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}


ಸ್ಥಿರ exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot, pte_t pte, pte_t *ptSlot)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    ಯದಿ (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                       cap_page_table_cap_get_capPTMappedAddress(cap), pt);
        clearMemory_PT((ಶೂನ್ಯ *)pt, cap_get_capSizeBits(cap));
    }

    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageInvocationMap(asid_t asid, cap_t cap, cte_t *ctSlot,
                                            pte_t pte, pte_t *ptSlot)
{
    bool_t tlbflush_required = pte_ptr_get_valid(ptSlot);

    ctSlot->cap = cap;
    *ptSlot = pte;

    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));
    ಯದಿ (unlikely(tlbflush_required)) {
        ಪ್ರತಿಜ್ಞಾ(asid < BIT(16));
        invalidateTLBByASIDVA(asid, cap_frame_cap_get_capFMappedAddress(cap));
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    ಯದಿ (cap_frame_cap_get_capFMappedASID(cap) != 0) {

        unmapPage(cap_frame_cap_get_capFSize(cap),
                  cap_frame_cap_get_capFMappedASID(cap),
                  cap_frame_cap_get_capFMappedAddress(cap),
                  cap_frame_cap_get_capFBasePtr(cap));
    }

    cap_t slotCap = ctSlot->cap;
    slotCap = cap_frame_cap_set_capFMappedAddress(slotCap, 0);
    slotCap = cap_frame_cap_set_capFMappedASID(slotCap, asidInvalid);
    ctSlot->cap = slotCap;


    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageFlush(word_t invLabel, vspace_ಬೇರು_t *vspace_ಬೇರು_2, asid_t asid,
                                    vptr_t start, vptr_t end, paddr_t pstart)
{
    ಯದಿ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        word_t size = end - start;
        start = (vptr_t)paddr_to_pptr(pstart);
        end = start + size;

        ಯದಿ (start < end) {
            doFlush(invLabel, start, end, pstart);
        }
    } ಅನ್ಯಥಾ {
        bool_t ಬೇರು_switched;

        ಯದಿ (start < end) {
            ಬೇರು_switched = setVMRootForFlush(vspace_ಬೇರು_2, asid);
            doFlush(invLabel, start, end, pstart);
            ಯದಿ (ಬೇರು_switched) {
                setVMRoot(ಗಂಟು_STATE(ksCurThread));
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageGetAddress(pptr_t base_ptr, bool_t call)
{
    paddr_t base = pptr_to_paddr((ಶೂನ್ಯ *)base_ptr);

    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    ಯದಿ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned ಪೂರ್ಣಾಙ್ಕ length = setMR(thread, ipcBuffer, 0, base);
        setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performASIDControlInvocation(ಶೂನ್ಯ *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_base)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, BIT(seL4_ASIDPoolBits));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ಸೇರಿಸು(
        cap_asid_pool_cap_new(
            asid_base,         /* capASIDBase  */
            WORD_REF(frame)    /* capASIDPool  */
        ), parent, slot);

    ಪ್ರತಿಜ್ಞಾ((asid_base & MASK(asid_ಕೆಳಗಿನ_Bits)) == 0);
    armKSASIDTable[ASID_ಮೇಲಿನ(asid_base)] = (asid_pool_t *)frame;

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decode_ARMVSpace_ಬೇರು_Invocation(word_t invLabel, word_t length,
                                                 cte_t *cte, cap_t cap, word_t *buffer)
{
    vptr_t start, end;
    paddr_t pstart;
    asid_t asid;
    vspace_ಬೇರು_t *vspace_ಬೇರು_2;
    lookupPTSlot_ret_t resolve_ret;
    findVSpaceForASID_ret_t find_ret;
    pte_t pte;

    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ ARMVSpaceClean_Data:
    ಪಕ್ಷ ARMVSpaceInvalidate_Data:
    ಪಕ್ಷ ARMVSpaceCleanInvalidate_Data:
    ಪಕ್ಷ ARMVSpaceUnify_Instruction:

        ಯದಿ (length < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, buffer);
        end =   getSyscallArg(1, buffer);

        /* Check sanity of arguments */
        ಯದಿ (end <= start) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        ಯದಿ (end > USER_ತುದಿ) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(!is_Valid_Native_ಬೇರು(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_ಬೇರು_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        find_ret = findVSpaceForASID(asid);
        ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(find_ret.vspace_ಬೇರು != vspace_ಬೇರು_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_ಬೇರು_2, start);
        pte = *resolve_ret.ptSlot;

        /* Check that the returned slot is a page. */
        ಯದಿ (!pte_is_page_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        ಯದಿ (ROUND_DOWN(start, resolve_ret.pt_Bits_ಎಡ) != ROUND_DOWN(end - 1, resolve_ret.pt_Bits_ಎಡ)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax = ROUND_DOWN(start, resolve_ret.pt_Bits_ಎಡ) +
                                                  MASK(resolve_ret.pt_Bits_ಎಡ);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        ಯದಿ (invLabel == ARMVSpaceInvalidate_Data && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#endif

        /* Calculate the physical start address. */
        paddr_t frame_base = pte_get_page_base_address(pte);

        pstart = frame_base + (start & MASK(resolve_ret.pt_Bits_ಎಡ));

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        ಯದಿ (pstart < PADDR_BASE || ((end - start) + pstart) > PADDR_ತುದಿ) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performVSpaceFlush(invLabel, vspace_ಬೇರು_2, asid, start, end - 1, pstart);

    ಸ್ವಭಾವ:
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}


ಸ್ಥಿರ exception_t decodeARMPageTableInvocation(word_t invLabel, word_t length,
                                                cte_t *cte, cap_t cap, word_t *buffer)
{
    cap_t vspace_ಬೇರು_Cap;
    vspace_ಬೇರು_t *vspace_ಬೇರು_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t ptSlot;
    findVSpaceForASID_ret_t find_ret;

    ಯದಿ (invLabel == ARMPageTableUnmap) {
        ಯದಿ (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = seL4_RevokeFirst;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performPageTableInvocationUnmap(cap, cte);
    }

    ಯದಿ (unlikely(invLabel != ARMPageTableMap)) {
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (unlikely(length < 2 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ)) {
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
    vspace_ಬೇರು_Cap = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (unlikely(!is_Valid_Native_ಬೇರು(vspace_ಬೇರು_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    vspace_ಬೇರು_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_ಬೇರು_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_ಬೇರು_Cap);

    ಯದಿ (unlikely(vaddr > USER_ತುದಿ)) {
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    find_ret = findVSpaceForASID(asid);
    ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (unlikely(find_ret.vspace_ಬೇರು != vspace_ಬೇರು_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ptSlot = lookupPTSlot(vspace_ಬೇರು_2, vaddr);

    ಯದಿ (unlikely(ptSlot.pt_Bits_ಎಡ == seL4_PageBits || pte_ptr_get_valid(ptSlot.ptSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_table_new(pptr_to_paddr(PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap))));

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, (vaddr & ~MASK(ptSlot.pt_Bits_ಎಡ)));

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ performPageTableInvocationMap(cap, cte, pte, ptSlot.ptSlot);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t CONST checkVPAlignment(vm_page_size_t sz, word_t w)
{
    ಪ್ರತ್ಯಾಗಮ (w & MASK(pageBitsForSize(sz))) == 0;
}

ಸ್ಥಿರ exception_t decodeARMFrameInvocation(word_t invLabel, word_t length,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ ARMPageMap: {
        vptr_t vaddr;
        paddr_t base;
        cap_t vspace_ಬೇರು_Cap;
        vspace_ಬೇರು_t *vspace_ಬೇರು_2;
        asid_t asid, frame_asid;
        vm_rights_t vmRights, capRights;
        vm_page_size_t frameSize;
        vm_attributes_t attributes;
        findVSpaceForASID_ret_t find_ret;

        ಯದಿ (unlikely(length < 3 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ)) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, buffer);
        attributes = vmAttributesFromWord(getSyscallArg(2, buffer));
        vspace_ಬೇರು_Cap = current_extra_caps.excaprefs[0]->cap;

        frameSize = cap_frame_cap_get_capFSize(cap);
        capRights = cap_frame_cap_get_capFVMRights(cap);
        vmRights = maskVMRights(capRights, rightsFromWord(getSyscallArg(1, buffer)));

        ಯದಿ (unlikely(!is_Valid_Native_ಬೇರು(vspace_ಬೇರು_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_ಬೇರು_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_ಬೇರು_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_ಬೇರು_Cap);

        find_ret = findVSpaceForASID(asid);
        ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(find_ret.vspace_ಬೇರು != vspace_ಬೇರು_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(!checkVPAlignment(frameSize, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        frame_asid = cap_frame_cap_get_capFMappedASID(cap);

        ಯದಿ (frame_asid != asidInvalid) {
            ಯದಿ (frame_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;

            } ಅನ್ಯಥಾ ಯದಿ (cap_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;
                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }
        } ಅನ್ಯಥಾ {
            ಯದಿ (unlikely(vaddr + BIT(pageBitsForSize(frameSize)) - 1 > USER_ತುದಿ)) {
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;
                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_frame_cap_set_capFMappedASID(cap, asid);
        cap = cap_frame_cap_set_capFMappedAddress(cap, vaddr);

        base = pptr_to_paddr((ಶೂನ್ಯ *)cap_frame_cap_get_capFBasePtr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_ಬೇರು_2, vaddr);
        ಯದಿ (unlikely(lu_ret.pt_Bits_ಎಡ != pageBitsForSize(frameSize))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_Bits_ಎಡ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = makeUserPagePTE(base, vmRights, cap_read, attributes, frameSize);

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performPageInvocationMap(asid, cap, cte, pte, lu_ret.ptSlot);
    }

    ಪಕ್ಷ ARMPageUnmap:
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performPageInvocationUnmap(cap, cte);

    ಪಕ್ಷ ARMPageClean_Data:
    ಪಕ್ಷ ARMPageInvalidate_Data:
    ಪಕ್ಷ ARMPageCleanInvalidate_Data:
    ಪಕ್ಷ ARMPageUnify_Instruction: {
        vptr_t start, end;
        vptr_t vaddr;
        asid_t asid;
        word_t page_size;
        findVSpaceForASID_ret_t find_ret;

        ಯದಿ (length < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(cap_frame_cap_get_capFMappedASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_frame_cap_get_capFMappedAddress(cap);
        asid = cap_frame_cap_get_capFMappedASID(cap);

        find_ret = findVSpaceForASID(asid);
        ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, buffer);
        end =   getSyscallArg(1, buffer);

        /* check that the range is sane */
        ಯದಿ (end <= start) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        page_size = BIT(pageBitsForSize(cap_frame_cap_get_capFSize(cap)));
        ಯದಿ (start >= page_size || end > page_size) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        word_t pstart = pptr_to_paddr((ಶೂನ್ಯ *)cap_frame_cap_get_capFBasePtr(cap)) + start;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        ಯದಿ (pstart < PADDR_BASE || ((end - start) + pstart) > PADDR_ತುದಿ) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#ಅನ್ಯಥಾ
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(find_ret.vspace_ಬೇರು, vaddr);
        pte_t pte = *lu_ret.ptSlot;
        ಶೂನ್ಯ *base_ptr = (ಶೂನ್ಯ *) cap_frame_cap_get_capFBasePtr(cap);
        ಯದಿ (unlikely(lu_ret.pt_Bits_ಎಡ != pageBitsForSize(cap_frame_cap_get_capFSize(cap)) ||
                     !pte_is_page_type(pte) ||
                     pte_get_page_base_address(pte) != pptr_to_paddr(base_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        ಯದಿ (invLabel == ARMPageInvalidate_Data && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performPageFlush(invLabel, find_ret.vspace_ಬೇರು, asid, vaddr + start, vaddr + end - 1,
                                pstart);
    }

    ಪಕ್ಷ ARMPageGetAddress:
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performPageGetAddress(cap_frame_cap_get_capFBasePtr(cap), call);

    ಸ್ವಭಾವ:
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t length, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_vspace_cap:
        ಪ್ರತ್ಯಾಗಮ decode_ARMVSpace_ಬೇರು_Invocation(invLabel, length, cte, cap, buffer);
    ಪಕ್ಷ cap_page_table_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMPageTableInvocation(invLabel, length, cte, cap, buffer);

    ಪಕ್ಷ cap_frame_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMFrameInvocation(invLabel, length, cte, cap, call, buffer);

    ಪಕ್ಷ cap_asid_control_cap: {
        word_t i;
        asid_t asid_base;
        word_t index, depth;
        cap_t untyped, ಬೇರು;
        cte_t *parentSlot, *destSlot;
        lookupSlot_ret_t lu_ret;
        ಶೂನ್ಯ *frame;
        exception_t status;

        ಯದಿ (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(length < 2 ||
                     current_extra_caps.excaprefs[0] == ರಿಕ್ತ ||
                     current_extra_caps.excaprefs[1] == ರಿಕ್ತ)) {
            current_syscall_error.type = seL4_TruncatedMessage;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        index = getSyscallArg(0, buffer);
        depth = getSyscallArg(1, buffer);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        ಬೇರು = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        ಪ್ರತಿ (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        ಯದಿ (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_ಕೆಳಗಿನ_Bits;

        ಯದಿ (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_capBlockSize(untyped) != seL4_ASIDPoolBits ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        ಯದಿ (unlikely(status != EXCEPTION_NONE)) {
            ಪ್ರತ್ಯಾಗಮ status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(ಬೇರು, index, depth);
        ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        ಯದಿ (unlikely(status != EXCEPTION_NONE)) {
            ಪ್ರತ್ಯಾಗಮ status;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performASIDControlInvocation(frame, destSlot, parentSlot, asid_base);
    }

    ಪಕ್ಷ cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        ಯದಿ (unlikely(invLabel != ARMASIDPoolAssign)) {
            current_syscall_error.type = seL4_IllegalOperation;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(current_extra_caps.excaprefs[0] == ರಿಕ್ತ)) {
            current_syscall_error.type = seL4_TruncatedMessage;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        ಯದಿ (unlikely(!is_VTable_ಬೇರು(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ಮೇಲಿನ(cap_asid_pool_cap_get_capASIDBase(cap))];

        ಯದಿ (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        ಪ್ರತಿ (i = 0; i < (1 << asid_ಕೆಳಗಿನ_Bits) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        ಯದಿ (unlikely(i == 1 << asid_ಕೆಳಗಿನ_Bits)) {
            current_syscall_error.type = seL4_DeleteFirst;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }

    ಸ್ವಭಾವ:
        fail("Invalid ARM arch cap type");
    }
}

#ifdef CONFIG_DEBUG_BUILD
ಶೂನ್ಯ kernelPrefetchAbort(word_t pc) VISIBLE;
ಶೂನ್ಯ kernelDataAbort(word_t pc) VISIBLE;

ಶೂನ್ಯ kernelPrefetchAbort(word_t pc)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("ESR (IFSR): 0x%"SEL4_PRIx_word"\n", getIFSR());
    halt();
}

ಶೂನ್ಯ kernelDataAbort(word_t pc)
{
    printf("\n\nKERNEL DATA ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("FAR: 0x%"SEL4_PRIx_word" ESR (DFSR): 0x%"SEL4_PRIx_word"\n",
           getFAR(), getDFSR());
    halt();
}
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_PRINTING
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ readWordFromVSpace_ret {
    exception_t status;
    word_t ಮೌಲ್ಯ;
} readWordFromVSpace_ret_t;

ಸ್ಥಿರ readWordFromVSpace_ret_t readWordFromVSpace(vspace_ಬೇರು_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    word_t offset;
    pptr_t kernel_vaddr;
    word_t *ಮೌಲ್ಯ;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    ಯದಿ (!pte_ptr_get_valid(lookup_ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(lookup_ret.ptSlot) && lookup_ret.pt_Bits_ಎಡ > PAGE_BITS)) {
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    offset = vaddr & MASK(lookup_ret.pt_Bits_ಎಡ);
    kernel_vaddr = (word_t)paddr_to_pptr(pte_page_ptr_get_page_base_address(lookup_ret.ptSlot));
    ಮೌಲ್ಯ = (word_t *)(kernel_vaddr + offset);

    ret.status = EXCEPTION_NONE;
    ret.ಮೌಲ್ಯ = *ಮೌಲ್ಯ;
    ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ Arch_user_ರಾಶಿ_Trace(tcb_t *tptr)
{
    cap_t thread_ಬೇರು;
    vspace_ಬೇರು_t *vspace_ಬೇರು_2;
    word_t sp;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    ಯದಿ (cap_get_capType(thread_ಬೇರು) != cap_vspace_cap) {
        printf("Invalid vspace\n");
        ಪ್ರತ್ಯಾಗಮ;
    }

    vspace_ಬೇರು_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_ಬೇರು));
    sp = getRegister(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ಯದಿ (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        printf("SP not aligned\n");
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (unsigned ಪೂರ್ಣಾಙ್ಕ i = 0; i < CONFIG_USER_ರಾಶಿ_TRACE_LENGTH; i++) {
        word_t address = sp + (i * ಪರಿಮಾಣ(word_t));
        readWordFromVSpace_ret_t result = readWordFromVSpace(vspace_ಬೇರು_2,
                                                             address);
        ಯದಿ (result.status == EXCEPTION_NONE) {
            printf("0x%"SEL4_PRIx_word": 0x%"SEL4_PRIx_word"\n",
                   address, result.ಮೌಲ್ಯ);
        } ಅನ್ಯಥಾ {
            printf("0x%"SEL4_PRIx_word": INVALID\n", address);
        }
    }
}
#endif /* CONFIG_PRINTING */

#ಯದಿ defined(CONFIG_KERNEL_LOG_BUFFER)
exception_t benchmark_arch_map_logBuffer(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_size_t frameSize;
    pptr_t  frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(ಗಂಟು_STATE(ksCurThread), frame_cptr);

    ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    frameSize = cap_frame_cap_get_capFSize(lu_ret.cap);

    ಯದಿ (frameSize != ARMLargePage) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((ಶೂನ್ಯ *) frame_pptr);

    *armKSGlobalLogPTE = pte_pte_page_new(
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#ಅನ್ಯಥಾ
                             1, // UXN
#endif
                             ksUserLogBuffer,
                             0,                         /* global */
                             1,                         /* access flag */
                             SMP_TERNARY(SMP_SHARE, 0), /* Inner-shareable if SMP enabled, otherwise unshared */
                             0,                         /* VMKernelOnly */
                             NORMAL_WT);

    cleanByVA_PoU((vptr_t)armKSGlobalLogPTE, addrFromKPPtr(armKSGlobalLogPTE));
    invalidateTranslationSingle(KS_LOG_PPTR);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */
