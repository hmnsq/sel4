/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <benchmark/benchmark.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <kernel/boot.h>
#અન્તર્ભાવ <kernel/cspace.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <kernel/stack.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <machine/debug.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <object/cnode.h>
#અન્તર્ભાવ <object/untyped.h>
#અન્તર્ભાવ <arch/api/invocation.h>
#અન્તર્ભાવ <arch/kernel/vspace.h>
#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <plat/machine/hardware.h>
#અન્તર્ભાવ <armv/context_switch.h>
#અન્તર્ભાવ <arch/object/iospace.h>
#અન્તર્ભાવ <arch/object/vcpu.h>
#અન્તર્ભાવ <arch/machine/tlb.h>

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
ગણના mair_types {
    DEVICE_nGnRnE = 0,
    DEVICE_nGnRE = 1,
    DEVICE_GRE = 2,
    NORMAL_NC = 3,
    NORMAL = 4,
    NORMAL_WT = 5
};

/* Stage-2 translation memory attributes */
ગણના mair_s2_types {
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

સંરચના lookupPTSlot_ret {
    pte_t *ptSlot;
    word_t pt_Bits_ડાબું;
};
પ્રકારવ્યાખ્યા સંરચના lookupPTSlot_ret lookupPTSlot_ret_t;

સંરચના findVSpaceForASID_ret {
    exception_t status;
    vspace_મૂળ_t *vspace_મૂળ;
};
પ્રકારવ્યાખ્યા સંરચના findVSpaceForASID_ret findVSpaceForASID_ret_t;

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

સ્થિર word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    પસંદગી (vm_rights) {
    સ્થિતિ VMKernelOnly:
        જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            પાછા 0;
        } નહીંતર {
            પાછા 0;
        }

    સ્થિતિ VMReadWrite:
        જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            પાછા 3;
        } નહીંતર {
            પાછા 1;
        }

    સ્થિતિ VMReadOnly:
        જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            પાછા 1;
        } નહીંતર {
            પાછા 3;
        }

    મૂળભૂત:
        fail("Invalid VM rights");
    }
}

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
સ્થિર અંતઃરેખ CONST word_t pte_get_AP(pte_t pte)
{
    પ્રતિજ્ઞા(pte_is_page_type(pte));
    પસંદગી (pte_get_pte_type(pte)) {
    સ્થિતિ pte_pte_4k_page:
        પાછા pte_pte_4k_page_get_AP(pte);

    સ્થિતિ pte_pte_page:
        પાછા pte_pte_page_get_AP(pte);

    મૂળભૂત:
        પાછા 0;
    }
}

સ્થિર vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    word_t access_perms = pte_get_AP(pte);
    પસંદગી (access_perms) {
    સ્થિતિ 0:
    સ્થિતિ 2:
        પાછા VMKernelOnly;
    સ્થિતિ 1:
        પાછા VMReadWrite;
    સ્થિતિ 3:
        પાછા VMReadOnly;
    મૂળભૂત:
        fail("Invalid AP bit");
    }
}
#endif

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    જો (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        પાછા VMReadOnly;
    }
    જો (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        જો (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            પાછા VMReadOnly;
        } નહીંતર {
            પાછા VMReadWrite;
        }
    }
    જો (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    પાછા VMKernelOnly;
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
BOOT_CODE શૂન્ય map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_attributes_t attributes)
{
    પ્રતિજ્ઞા(vaddr >= PPTR_ટોચ);

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t uxn = vm_attributes_get_armExecuteNever(attributes);
#નહીંતર
    word_t uxn = 1; /* unprivileged execute never */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    word_t attr_index;
    word_t shareable;
    જો (vm_attributes_get_armPageCacheable(attributes)) {
        attr_index = NORMAL;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } નહીંતર {
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
BOOT_CODE શૂન્ય map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_BASE_RAW, seL4_LargePageBits));
    compile_assert(elf_base_aligned, IS_ALIGNED(KERNEL_ELF_BASE, seL4_LargePageBits));

    /* Please don't overlap with KDEV_BASE */
    પ્રતિજ્ઞા(KERNEL_ELF_ટોચ <= KDEV_BASE);
    /* Please don't overlap with KS_LOG_PPTR */
    પ્રતિજ્ઞા(KERNEL_ELF_ટોચ <= KS_LOG_PPTR);

    અચલ word_t pd_start = GET_KPT_INDEX(KERNEL_ELF_BASE, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    અચલ word_t pd_end   = GET_KPT_INDEX(KERNEL_ELF_ટોચ - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_BASE;
    UNUSED pptr_t vaddr = KERNEL_ELF_BASE;

    માટે (word_t pd = pd_start; pd < pd_end; pd++) {
        પ્રતિજ્ઞા(pd == GET_KPT_INDEX(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_page_new(
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#નહીંતર
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
#નહીંતર
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_window_pgd_location,
               GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(0)) == BIT(PT_INDEX_BITS) - 1);
#endif
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_window_base_aligned, IS_ALIGNED(PPTR_BASE, seL4_HugePageBits));
compile_assert(physical_window_ટોચ_aligned, IS_ALIGNED(PPTR_ટોચ, seL4_HugePageBits));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_window_bottom_kernel_region,
               GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_window_ટોચ_kernel_region,
               GET_KPT_INDEX(PPTR_ટોચ, KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_INDEX_BITS) - 1);

BOOT_CODE શૂન્ય map_kernel_physical_window(શૂન્ય)
{
    paddr_t paddr;
    pptr_t vaddr;
    word_t idx;

    /* place all PDs except the last one in PUD */
    માટે (idx = GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_INDEX(PPTR_ટોચ, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_table_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_BASE;
    માટે (paddr = PADDR_BASE; paddr < PADDR_ટોચ; paddr += BIT(seL4_LargePageBits)) {
        armKSGlobalKernelPDs[GET_KPT_INDEX(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_INDEX(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_page_new(
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#નહીંતર
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

BOOT_CODE શૂન્ય map_kernel_window(શૂન્ય)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_table_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_window();

    અચલ word_t elf_pud_idx = GET_KPT_INDEX(KERNEL_ELF_BASE, KLVL_FRM_ARM_PT_LVL(1));
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
સ્થિર BOOT_CODE શૂન્ય map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap, bool_t executable)
{
    vspace_મૂળ_t *vspace_મૂળ_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);
    શૂન્ય *pptr = (શૂન્ય *)cap_frame_cap_get_capFBasePtr(frame_cap);

    પ્રતિજ્ઞા(cap_frame_cap_get_capFMappedASID(frame_cap) != 0);

#ifdef AARCH64_VSPACE_S2_START_L1
    pud = vspace_મૂળ_2;
#નહીંતર
    vspace_મૂળ_2 += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0));
    પ્રતિજ્ઞા(pte_pte_table_ptr_get_present(vspace_મૂળ_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_મૂળ_2));
#endif
    pud += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
    પ્રતિજ્ઞા(pte_pte_table_ptr_get_present(pud));
    pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pud));
    pd += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
    પ્રતિજ્ઞા(pte_pte_table_ptr_get_present(pd));
    pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pd));
    *(pt + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_page_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_to_paddr(pptr),            /* page_base_address    */
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#નહીંતર
                                                              1,                              /* not global */
#endif
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_NORMAL
#નહીંતર
                                                              NORMAL
#endif
                                                          );
}

સ્થિર BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_page_size_t frame_size;
    જો (use_large) {
        frame_size = ARMLargePage;
    } નહીંતર {
        frame_size = ARMSmallPage;
    }
    પાછા
        cap_frame_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            frame_size,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            wordFromVMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

સ્થિર BOOT_CODE શૂન્ય map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_મૂળ_t *vspace_મૂળ_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);

    પ્રતિજ્ઞા(cap_page_table_cap_get_capPTIsMapped(pt_cap));

#ifdef AARCH64_VSPACE_S2_START_L1
    pud = vspace_મૂળ_2;
#નહીંતર
    vspace_મૂળ_2 += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0));
    પ્રતિજ્ઞા(pte_pte_table_ptr_get_present(vspace_મૂળ_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_મૂળ_2));
#endif
    pud += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
    પ્રતિજ્ઞા(pte_pte_table_ptr_get_present(pud));
    pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pud));
    *(pd + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_table_new(
                                                              pptr_to_paddr(pt)
                                                          );
}

સ્થિર BOOT_CODE cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    પાછા cap;
}

સ્થિર BOOT_CODE શૂન્ય map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_મૂળ_t *vspace_મૂળ_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pd_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pd_cap);

    પ્રતિજ્ઞા(cap_page_table_cap_get_capPTIsMapped(pd_cap));

#ifdef AARCH64_VSPACE_S2_START_L1
    pud = vspace_મૂળ_2;
#નહીંતર
    vspace_મૂળ_2 += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0));
    પ્રતિજ્ઞા(pte_pte_table_ptr_get_present(vspace_મૂળ_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_મૂળ_2));
#endif
    *(pud + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_table_new(
                                                               pptr_to_paddr(pd)
                                                           );
}

સ્થિર BOOT_CODE cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    પાછા cap;
}

#ifndef AARCH64_VSPACE_S2_START_L1
સ્થિર BOOT_CODE શૂન્ય map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pud_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pud_cap);

    પ્રતિજ્ઞા(cap_page_table_cap_get_capPTIsMapped(pud_cap));

    *(pgd + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_table_new(
                                                               pptr_to_paddr(pud));
}

સ્થિર BOOT_CODE cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    પાછા cap;
}
#endif /* AARCH64_VSPACE_S2_START_L1 */
BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    પાછા
#ifndef AARCH64_VSPACE_S2_START_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(0))) +
#endif
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_CODE cap_t create_it_address_space(cap_t મૂળ_cnode_cap, v_region_t it_v_reg)
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
    write_slot(SLOT_PTR(pptr_of_cap(મૂળ_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#ifndef AARCH64_VSPACE_S2_START_L1
    /* Create any PUDs needed for the user land image */
    માટે (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        જો (!provide_cap(મૂળ_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            પાછા cap_null_cap_new();
        }
    }
#endif
    /* Create any PDs needed for the user land image */
    માટે (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        જો (!provide_cap(મૂળ_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            પાછા cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    માટે (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        જો (!provide_cap(મૂળ_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            પાછા cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };
    પાછા vspace_cap;
}

BOOT_CODE cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    પાછા create_it_frame_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large);
    map_it_frame_cap(pd_cap, cap, executable);
    પાછા cap;
}

BOOT_CODE શૂન્ય activate_kernel_vspace(શૂન્ય)
{
    cleanInvalidateL1Caches();
    set_Current_Kernel_VSpace_મૂળ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    set_Current_User_VSpace_મૂળ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    lockTLBEntry(KERNEL_ELF_BASE);
}

BOOT_CODE શૂન્ય write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
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
    ap->array[ASID_નીચલું(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_ઉપલું(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t findMapForASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ઉપલું(asid)];
    જો (!poolPtr) {
        પાછા asid_map_asid_map_none_new();
    }

    પાછા poolPtr->array[ASID_નીચલું(asid)];
}

સ્થિર findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    જો (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_મૂળ_new();

        ret.vspace_મૂળ = ખાલી;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        પાછા ret;
    }

    ret.vspace_મૂળ = (vspace_મૂળ_t *)asid_map_asid_map_vspace_get_vspace_મૂળ(asid_map);
    ret.status = EXCEPTION_NONE;
    પાછા ret;
}

word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_bufferPtr;
    cap_t bufferCap;
    vm_rights_t vm_rights;

    w_bufferPtr = thread->tcbIPCBuffer;
    bufferCap = TCB_PTR_CTE_PTR(thread, tcbBuffer)->cap;

    જો (unlikely(cap_get_capType(bufferCap) != cap_frame_cap)) {
        પાછા ખાલી;
    }
    જો (unlikely(cap_frame_cap_get_capFIsDevice(bufferCap))) {
        પાછા ખાલી;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(bufferCap);
    જો (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr;
        unsigned પૂર્ણાંક pageBits;

        basePtr = cap_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(cap_frame_cap_get_capFSize(bufferCap));
        પાછા (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } નહીંતર {
        પાછા ખાલી;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    જો (cap_get_capType(cap) != cap_frame_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (!IS_ALIGNED(vptr, seL4_IPCBufferSizeBits)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (પૂર્ણાંક)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર lookupPTSlot_ret_t lookupPTSlot(vspace_મૂળ_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    word_t level = UPT_LEVELS - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_Bits_ડાબું = PT_INDEX_BITS * level + seL4_PageBits;
    ret.ptSlot = pt + ((vptr >> ret.pt_Bits_ડાબું) & MASK(seL4_VSpaceIndexBits));

    જ્યારે (pte_pte_table_ptr_get_present(ret.ptSlot) && likely(level > 0)) {
        level--;
        ret.pt_Bits_ડાબું -= PT_INDEX_BITS;
        pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ret.ptSlot));
        ret.ptSlot = pt + ((vptr >> ret.pt_Bits_ડાબું) & MASK(PT_INDEX_BITS));
    }

    પાછા ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
સ્થિર pte_t makeUserPagePTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_attributes_t attributes,
                             vm_page_size_t page_size)
{
    bool_t nonexecutable = vm_attributes_get_armExecuteNever(attributes) || !cap_read;
    word_t cacheable = vm_attributes_get_armPageCacheable(attributes);

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t nG = 0; /* not global */
    word_t attridx = cacheable ? S2_NORMAL : S2_DEVICE_nGnRnE;
#નહીંતર
    word_t nG = 1; /* not global */
    word_t attridx = cacheable ? NORMAL : DEVICE_nGnRnE;
#endif

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    word_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    જો (page_size == ARMSmallPage) {
        પાછા pte_pte_4k_page_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } નહીંતર {
        પાછા pte_pte_page_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    પસંદગી (vm_faultType) {
    સ્થિતિ ARMDataAbort: {
        word_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        જો (ARCH_ગાંઠ_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(addressTranslateS1(addr)) | (addr & MASK(PAGE_BITS));
        }
#endif
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        પાછા EXCEPTION_FAULT;
    }

    સ્થિતિ ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        જો (ARCH_ગાંઠ_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(addressTranslateS1(pc)) | (pc & MASK(PAGE_BITS));
        }
#endif
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        પાછા EXCEPTION_FAULT;
    }

    મૂળભૂત:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_મૂળ(cap_t cap)
{
    પાછા cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_મૂળ(cap_t cap)
{
    પાછા is_VTable_મૂળ(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_મૂળ(cap_t cap)
{
    પાછા is_Valid_Native_મૂળ(cap);
}

શૂન્ય setVMRoot(tcb_t *tcb)
{
    cap_t thread_મૂળ;
    asid_t asid;
    vspace_મૂળ_t *vspace_મૂળ_2;
    findVSpaceForASID_ret_t find_ret;

    thread_મૂળ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    જો (!is_Valid_Native_મૂળ(thread_મૂળ)) {
        set_Current_User_VSpace_મૂળ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        પાછા;
    }

    vspace_મૂળ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_મૂળ));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_મૂળ);
    find_ret = findVSpaceForASID(asid);
    જો (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.vspace_મૂળ != vspace_મૂળ_2)) {
        set_Current_User_VSpace_મૂળ(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        પાછા;
    }

    armv_contextSwitch(vspace_મૂળ_2, asid);
}

સ્થિર bool_t setVMRootForFlush(vspace_મૂળ_t *vspace, asid_t asid)
{
    cap_t thread_મૂળ;

    thread_મૂળ = TCB_PTR_CTE_PTR(ગાંઠ_STATE(ksCurThread), tcbVTable)->cap;

    જો (cap_get_capType(thread_મૂળ) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_મૂળ) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_મૂળ)) == vspace) {
        પાછા false;
    }

    armv_contextSwitch(vspace, asid);
    પાછા true;
}


#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

સ્થિર અંતઃરેખ asid_pool_t *getPoolPtr(asid_t asid)
{
    પાછા armKSASIDTable[ASID_ઉપલું(asid)];
}

સ્થિર અંતઃરેખ asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    પ્રતિજ્ઞા(poolPtr != ખાલી);
    પાછા poolPtr->array[ASID_નીચલું(asid)];
}

સ્થિર અંતઃરેખ શૂન્ય setASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    પ્રતિજ્ઞા(poolPtr != ખાલી);
    poolPtr->array[ASID_નીચલું(asid)] = asid_map;
}

સ્થિર શૂન્ય invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    પ્રતિજ્ઞા(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_set_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_set_stored_vmid_valid(asid_map, false);

    setASIDMap(poolPtr, asid, asid_map);
}

સ્થિર શૂન્ય storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    પ્રતિજ્ઞા(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_set_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_set_stored_vmid_valid(asid_map, true);

    setASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

સ્થિર hw_asid_t findFreeHWASID(શૂન્ય)
{
    word_t hw_asid_offset;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    માટે (hw_asid_offset = 0;
         hw_asid_offset <= (word_t)((hw_asid_t) - 1);
         hw_asid_offset++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_offset);
        જો (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            પાછા hw_asid;
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
    જો (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    પાછા hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    જો (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        પાછા asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } નહીંતર {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        પાછા new_hw_asid;
    }
}

સ્થિર શૂન્ય invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    જો (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#endif

#ifdef CONFIG_ARM_SMMU
સ્થિર word_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ઉપલું(asid)];
    પ્રતિજ્ઞા(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_નીચલું(asid)];
    પ્રતિજ્ઞા(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    પાછા asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

શૂન્ય increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ઉપલું(asid)];
    પ્રતિજ્ઞા(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_નીચલું(asid)];
    પ્રતિજ્ઞા(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_set_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

શૂન્ય decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_ઉપલું(asid)];
    પ્રતિજ્ઞા(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_નીચલું(asid)];
    પ્રતિજ્ઞા(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_set_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#endif

સ્થિર અંતઃરેખ શૂન્ય invalidateTLBByASID(asid_t asid)
{
#ifdef CONFIG_ARM_SMMU
    word_t bind_cb = getASIDBindCB(asid);
    જો (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    જો (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        પાછા;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#નહીંતર
    invalidateTranslationASID(asid);
#endif
}

સ્થિર અંતઃરેખ શૂન્ય invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#ifdef CONFIG_ARM_SMMU
    word_t bind_cb = getASIDBindCB(asid);
    જો (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    જો (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        પાછા;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> seL4_PageBits);
#નહીંતર
    invalidateTranslationSingle((asid << 48) | vaddr >> seL4_PageBits);
#endif
}


શૂન્ય unmapPageTable(asid_t asid, vptr_t vptr, pte_t *target_pt)
{
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
        /* nothing to do */
        પાછા;
    }
    pte_t *ptSlot = ખાલી;
    pte_t *pt = (pte_t *)find_ret.vspace_મૂળ;

    માટે (word_t i = 0; i < UPT_LEVELS - 1 && pt != target_pt; i++) {
        ptSlot = pt + GET_UPT_INDEX(vptr, i);
        જો (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
            /* couldn't find it */
            પાછા;
        }
        pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
    }

    જો (pt != target_pt) {
        /* didn't find it */
        પાછા;
    }
    /* If we found a pt then ptSlot won't be null */
    પ્રતિજ્ઞા(ptSlot != ખાલી);
    *ptSlot = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));
    invalidateTLBByASID(asid);
}

શૂન્ય unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    find_ret = findVSpaceForASID(asid);
    જો (find_ret.status != EXCEPTION_NONE) {
        પાછા;
    }

    lu_ret = lookupPTSlot(find_ret.vspace_મૂળ, vptr);
    જો (unlikely(lu_ret.pt_Bits_ડાબું != pageBitsForSize(page_size))) {
        /* Do nothing if the wrong size object was returned */
        પાછા;
    }

    pte = *(lu_ret.ptSlot);
    જો (!pte_is_page_type(pte)) {
        /* Do nothing if no page is present */
        પાછા;
    }

    જો (pte_get_page_base_address(pte) != pptr_to_paddr((શૂન્ય *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        પાછા;
    }

    *(lu_ret.ptSlot) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.ptSlot, pptr_to_paddr(lu_ret.ptSlot));
    પ્રતિજ્ઞા(asid < BIT(16));
    invalidateTLBByASIDVA(asid, vptr);
}

શૂન્ય deleteASID(asid_t asid, vspace_મૂળ_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ઉપલું(asid)];

    જો (poolPtr != ખાલી) {
        asid_map_t asid_map = poolPtr->array[ASID_નીચલું(asid)];
        જો (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_મૂળ_t *)asid_map_asid_map_vspace_get_vspace_મૂળ(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#endif
            poolPtr->array[ASID_નીચલું(asid)] = asid_map_asid_map_none_new();
            setVMRoot(ગાંઠ_STATE(ksCurThread));
        }
    }
}

શૂન્ય deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    word_t offset;

    પ્રતિજ્ઞા((asid_base & MASK(asid_નીચલું_Bits)) == 0);

    જો (armKSASIDTable[ASID_ઉપલું(asid_base)] == pool) {
        માટે (offset = 0; offset < BIT(asid_નીચલું_Bits); offset++) {
            asid_map_t asid_map = pool->array[offset];
            જો (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_base + offset);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_base + offset);
#endif
            }
        }
        armKSASIDTable[ASID_ઉપલું(asid_base)] = ખાલી;
        setVMRoot(ગાંઠ_STATE(ksCurThread));
    }
}

સ્થિર શૂન્ય doFlush(word_t invLabel, vptr_t start, vptr_t end, paddr_t pstart)
{
    પસંદગી (invLabel) {
    સ્થિતિ ARMVSpaceClean_Data:
    સ્થિતિ ARMPageClean_Data:
        cleanCacheRange_RAM(start, end, pstart);
        તોડો;

    સ્થિતિ ARMVSpaceInvalidate_Data:
    સ્થિતિ ARMPageInvalidate_Data:
        invalidateCacheRange_RAM(start, end, pstart);
        તોડો;

    સ્થિતિ ARMVSpaceCleanInvalidate_Data:
    સ્થિતિ ARMPageCleanInvalidate_Data:
        cleanInvalidateCacheRange_RAM(start, end, pstart);
        તોડો;

    સ્થિતિ ARMVSpaceUnify_Instruction:
    સ્થિતિ ARMPageUnify_Instruction:
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
        તોડો;
    મૂળભૂત:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

સ્થિર exception_t performVSpaceFlush(word_t invLabel, vspace_મૂળ_t *vspace_મૂળ_2, asid_t asid,
                                      vptr_t start, vptr_t end, paddr_t pstart)
{

    જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        word_t size = end - start;
        start = (vptr_t)paddr_to_pptr(pstart);
        end = start + size;
        જો (start < end) {
            doFlush(invLabel, start, end, pstart);
        }
    } નહીંતર {
        bool_t મૂળ_switched;

        /* Flush if given a non zero range */
        જો (start < end) {
            મૂળ_switched = setVMRootForFlush(vspace_મૂળ_2, asid);
            doFlush(invLabel, start, end, pstart);
            જો (મૂળ_switched) {
                setVMRoot(ગાંઠ_STATE(ksCurThread));
            }
        }
    }
    પાછા EXCEPTION_NONE;
}


સ્થિર exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot, pte_t pte, pte_t *ptSlot)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    જો (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                       cap_page_table_cap_get_capPTMappedAddress(cap), pt);
        clearMemory_PT((શૂન્ય *)pt, cap_get_capSizeBits(cap));
    }

    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageInvocationMap(asid_t asid, cap_t cap, cte_t *ctSlot,
                                            pte_t pte, pte_t *ptSlot)
{
    bool_t tlbflush_required = pte_ptr_get_valid(ptSlot);

    ctSlot->cap = cap;
    *ptSlot = pte;

    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));
    જો (unlikely(tlbflush_required)) {
        પ્રતિજ્ઞા(asid < BIT(16));
        invalidateTLBByASIDVA(asid, cap_frame_cap_get_capFMappedAddress(cap));
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    જો (cap_frame_cap_get_capFMappedASID(cap) != 0) {

        unmapPage(cap_frame_cap_get_capFSize(cap),
                  cap_frame_cap_get_capFMappedASID(cap),
                  cap_frame_cap_get_capFMappedAddress(cap),
                  cap_frame_cap_get_capFBasePtr(cap));
    }

    cap_t slotCap = ctSlot->cap;
    slotCap = cap_frame_cap_set_capFMappedAddress(slotCap, 0);
    slotCap = cap_frame_cap_set_capFMappedASID(slotCap, asidInvalid);
    ctSlot->cap = slotCap;


    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageFlush(word_t invLabel, vspace_મૂળ_t *vspace_મૂળ_2, asid_t asid,
                                    vptr_t start, vptr_t end, paddr_t pstart)
{
    જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        word_t size = end - start;
        start = (vptr_t)paddr_to_pptr(pstart);
        end = start + size;

        જો (start < end) {
            doFlush(invLabel, start, end, pstart);
        }
    } નહીંતર {
        bool_t મૂળ_switched;

        જો (start < end) {
            મૂળ_switched = setVMRootForFlush(vspace_મૂળ_2, asid);
            doFlush(invLabel, start, end, pstart);
            જો (મૂળ_switched) {
                setVMRoot(ગાંઠ_STATE(ksCurThread));
            }
        }
    }
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageGetAddress(pptr_t base_ptr, bool_t call)
{
    paddr_t base = pptr_to_paddr((શૂન્ય *)base_ptr);

    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);
    જો (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned પૂર્ણાંક length = setMR(thread, ipcBuffer, 0, base);
        setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performASIDControlInvocation(શૂન્ય *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_base)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, BIT(seL4_ASIDPoolBits));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ઉમેરો(
        cap_asid_pool_cap_new(
            asid_base,         /* capASIDBase  */
            WORD_REF(frame)    /* capASIDPool  */
        ), parent, slot);

    પ્રતિજ્ઞા((asid_base & MASK(asid_નીચલું_Bits)) == 0);
    armKSASIDTable[ASID_ઉપલું(asid_base)] = (asid_pool_t *)frame;

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decode_ARMVSpace_મૂળ_Invocation(word_t invLabel, word_t length,
                                                 cte_t *cte, cap_t cap, word_t *buffer)
{
    vptr_t start, end;
    paddr_t pstart;
    asid_t asid;
    vspace_મૂળ_t *vspace_મૂળ_2;
    lookupPTSlot_ret_t resolve_ret;
    findVSpaceForASID_ret_t find_ret;
    pte_t pte;

    પસંદગી (invLabel) {
    સ્થિતિ ARMVSpaceClean_Data:
    સ્થિતિ ARMVSpaceInvalidate_Data:
    સ્થિતિ ARMVSpaceCleanInvalidate_Data:
    સ્થિતિ ARMVSpaceUnify_Instruction:

        જો (length < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, buffer);
        end =   getSyscallArg(1, buffer);

        /* Check sanity of arguments */
        જો (end <= start) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 1;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        જો (end > USER_ટોચ) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(!is_Valid_Native_મૂળ(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_મૂળ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        find_ret = findVSpaceForASID(asid);
        જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(find_ret.vspace_મૂળ != vspace_મૂળ_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_મૂળ_2, start);
        pte = *resolve_ret.ptSlot;

        /* Check that the returned slot is a page. */
        જો (!pte_is_page_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
            પાછા EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        જો (ROUND_DOWN(start, resolve_ret.pt_Bits_ડાબું) != ROUND_DOWN(end - 1, resolve_ret.pt_Bits_ડાબું)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax = ROUND_DOWN(start, resolve_ret.pt_Bits_ડાબું) +
                                                  MASK(resolve_ret.pt_Bits_ડાબું);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        જો (invLabel == ARMVSpaceInvalidate_Data && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
#endif

        /* Calculate the physical start address. */
        paddr_t frame_base = pte_get_page_base_address(pte);

        pstart = frame_base + (start & MASK(resolve_ret.pt_Bits_ડાબું));

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        જો (pstart < PADDR_BASE || ((end - start) + pstart) > PADDR_ટોચ) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performVSpaceFlush(invLabel, vspace_મૂળ_2, asid, start, end - 1, pstart);

    મૂળભૂત:
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}


સ્થિર exception_t decodeARMPageTableInvocation(word_t invLabel, word_t length,
                                                cte_t *cte, cap_t cap, word_t *buffer)
{
    cap_t vspace_મૂળ_Cap;
    vspace_મૂળ_t *vspace_મૂળ_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t ptSlot;
    findVSpaceForASID_ret_t find_ret;

    જો (invLabel == ARMPageTableUnmap) {
        જો (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = seL4_RevokeFirst;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageTableInvocationUnmap(cap, cte);
    }

    જો (unlikely(invLabel != ARMPageTableMap)) {
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(length < 2 || current_extra_caps.excaprefs[0] == ખાલી)) {
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
    vspace_મૂળ_Cap = current_extra_caps.excaprefs[0]->cap;

    જો (unlikely(!is_Valid_Native_મૂળ(vspace_મૂળ_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    vspace_મૂળ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_મૂળ_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_મૂળ_Cap);

    જો (unlikely(vaddr > USER_ટોચ)) {
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    find_ret = findVSpaceForASID(asid);
    જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(find_ret.vspace_મૂળ != vspace_મૂળ_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    ptSlot = lookupPTSlot(vspace_મૂળ_2, vaddr);

    જો (unlikely(ptSlot.pt_Bits_ડાબું == seL4_PageBits || pte_ptr_get_valid(ptSlot.ptSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_table_new(pptr_to_paddr(PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap))));

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, (vaddr & ~MASK(ptSlot.pt_Bits_ડાબું)));

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા performPageTableInvocationMap(cap, cte, pte, ptSlot.ptSlot);
}

સ્થિર અંતઃરેખ bool_t CONST checkVPAlignment(vm_page_size_t sz, word_t w)
{
    પાછા (w & MASK(pageBitsForSize(sz))) == 0;
}

સ્થિર exception_t decodeARMFrameInvocation(word_t invLabel, word_t length,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    પસંદગી (invLabel) {
    સ્થિતિ ARMPageMap: {
        vptr_t vaddr;
        paddr_t base;
        cap_t vspace_મૂળ_Cap;
        vspace_મૂળ_t *vspace_મૂળ_2;
        asid_t asid, frame_asid;
        vm_rights_t vmRights, capRights;
        vm_page_size_t frameSize;
        vm_attributes_t attributes;
        findVSpaceForASID_ret_t find_ret;

        જો (unlikely(length < 3 || current_extra_caps.excaprefs[0] == ખાલી)) {
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, buffer);
        attributes = vmAttributesFromWord(getSyscallArg(2, buffer));
        vspace_મૂળ_Cap = current_extra_caps.excaprefs[0]->cap;

        frameSize = cap_frame_cap_get_capFSize(cap);
        capRights = cap_frame_cap_get_capFVMRights(cap);
        vmRights = maskVMRights(capRights, rightsFromWord(getSyscallArg(1, buffer)));

        જો (unlikely(!is_Valid_Native_મૂળ(vspace_મૂળ_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        vspace_મૂળ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_મૂળ_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_મૂળ_Cap);

        find_ret = findVSpaceForASID(asid);
        જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(find_ret.vspace_મૂળ != vspace_મૂળ_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(!checkVPAlignment(frameSize, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        frame_asid = cap_frame_cap_get_capFMappedASID(cap);

        જો (frame_asid != asidInvalid) {
            જો (frame_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_સંખ્યા = 1;
                પાછા EXCEPTION_SYSCALL_ERROR;

            } નહીંતર જો (cap_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_સંખ્યા = 0;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }
        } નહીંતર {
            જો (unlikely(vaddr + BIT(pageBitsForSize(frameSize)) - 1 > USER_ટોચ)) {
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_સંખ્યા = 0;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_frame_cap_set_capFMappedASID(cap, asid);
        cap = cap_frame_cap_set_capFMappedAddress(cap, vaddr);

        base = pptr_to_paddr((શૂન્ય *)cap_frame_cap_get_capFBasePtr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_મૂળ_2, vaddr);
        જો (unlikely(lu_ret.pt_Bits_ડાબું != pageBitsForSize(frameSize))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_Bits_ડાબું);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = makeUserPagePTE(base, vmRights, cap_read, attributes, frameSize);

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageInvocationMap(asid, cap, cte, pte, lu_ret.ptSlot);
    }

    સ્થિતિ ARMPageUnmap:
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageInvocationUnmap(cap, cte);

    સ્થિતિ ARMPageClean_Data:
    સ્થિતિ ARMPageInvalidate_Data:
    સ્થિતિ ARMPageCleanInvalidate_Data:
    સ્થિતિ ARMPageUnify_Instruction: {
        vptr_t start, end;
        vptr_t vaddr;
        asid_t asid;
        word_t page_size;
        findVSpaceForASID_ret_t find_ret;

        જો (length < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(cap_frame_cap_get_capFMappedASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_frame_cap_get_capFMappedAddress(cap);
        asid = cap_frame_cap_get_capFMappedASID(cap);

        find_ret = findVSpaceForASID(asid);
        જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, buffer);
        end =   getSyscallArg(1, buffer);

        /* check that the range is sane */
        જો (end <= start) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 1;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        page_size = BIT(pageBitsForSize(cap_frame_cap_get_capFSize(cap)));
        જો (start >= page_size || end > page_size) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        word_t pstart = pptr_to_paddr((શૂન્ય *)cap_frame_cap_get_capFBasePtr(cap)) + start;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        જો (pstart < PADDR_BASE || ((end - start) + pstart) > PADDR_ટોચ) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
#નહીંતર
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(find_ret.vspace_મૂળ, vaddr);
        pte_t pte = *lu_ret.ptSlot;
        શૂન્ય *base_ptr = (શૂન્ય *) cap_frame_cap_get_capFBasePtr(cap);
        જો (unlikely(lu_ret.pt_Bits_ડાબું != pageBitsForSize(cap_frame_cap_get_capFSize(cap)) ||
                     !pte_is_page_type(pte) ||
                     pte_get_page_base_address(pte) != pptr_to_paddr(base_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        જો (invLabel == ARMPageInvalidate_Data && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
#endif
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageFlush(invLabel, find_ret.vspace_મૂળ, asid, vaddr + start, vaddr + end - 1,
                                pstart);
    }

    સ્થિતિ ARMPageGetAddress:
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageGetAddress(cap_frame_cap_get_capFBasePtr(cap), call);

    મૂળભૂત:
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t length, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_vspace_cap:
        પાછા decode_ARMVSpace_મૂળ_Invocation(invLabel, length, cte, cap, buffer);
    સ્થિતિ cap_page_table_cap:
        પાછા decodeARMPageTableInvocation(invLabel, length, cte, cap, buffer);

    સ્થિતિ cap_frame_cap:
        પાછા decodeARMFrameInvocation(invLabel, length, cte, cap, call, buffer);

    સ્થિતિ cap_asid_control_cap: {
        word_t i;
        asid_t asid_base;
        word_t index, depth;
        cap_t untyped, મૂળ;
        cte_t *parentSlot, *destSlot;
        lookupSlot_ret_t lu_ret;
        શૂન્ય *frame;
        exception_t status;

        જો (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(length < 2 ||
                     current_extra_caps.excaprefs[0] == ખાલી ||
                     current_extra_caps.excaprefs[1] == ખાલી)) {
            current_syscall_error.type = seL4_TruncatedMessage;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        index = getSyscallArg(0, buffer);
        depth = getSyscallArg(1, buffer);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        મૂળ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        માટે (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        જો (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_નીચલું_Bits;

        જો (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_capBlockSize(untyped) != seL4_ASIDPoolBits ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        જો (unlikely(status != EXCEPTION_NONE)) {
            પાછા status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(મૂળ, index, depth);
        જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            પાછા lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        જો (unlikely(status != EXCEPTION_NONE)) {
            પાછા status;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performASIDControlInvocation(frame, destSlot, parentSlot, asid_base);
    }

    સ્થિતિ cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        જો (unlikely(invLabel != ARMASIDPoolAssign)) {
            current_syscall_error.type = seL4_IllegalOperation;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(current_extra_caps.excaprefs[0] == ખાલી)) {
            current_syscall_error.type = seL4_TruncatedMessage;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        જો (unlikely(!is_VTable_મૂળ(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ઉપલું(cap_asid_pool_cap_get_capASIDBase(cap))];

        જો (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_મૂળ_new();

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        માટે (i = 0; i < (1 << asid_નીચલું_Bits) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        જો (unlikely(i == 1 << asid_નીચલું_Bits)) {
            current_syscall_error.type = seL4_DeleteFirst;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }

    મૂળભૂત:
        fail("Invalid ARM arch cap type");
    }
}

#ifdef CONFIG_DEBUG_BUILD
શૂન્ય kernelPrefetchAbort(word_t pc) VISIBLE;
શૂન્ય kernelDataAbort(word_t pc) VISIBLE;

શૂન્ય kernelPrefetchAbort(word_t pc)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("ESR (IFSR): 0x%"SEL4_PRIx_word"\n", getIFSR());
    halt();
}

શૂન્ય kernelDataAbort(word_t pc)
{
    printf("\n\nKERNEL DATA ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("FAR: 0x%"SEL4_PRIx_word" ESR (DFSR): 0x%"SEL4_PRIx_word"\n",
           getFAR(), getDFSR());
    halt();
}
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_PRINTING
પ્રકારવ્યાખ્યા સંરચના readWordFromVSpace_ret {
    exception_t status;
    word_t કિંમત;
} readWordFromVSpace_ret_t;

સ્થિર readWordFromVSpace_ret_t readWordFromVSpace(vspace_મૂળ_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    word_t offset;
    pptr_t kernel_vaddr;
    word_t *કિંમત;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    જો (!pte_ptr_get_valid(lookup_ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(lookup_ret.ptSlot) && lookup_ret.pt_Bits_ડાબું > PAGE_BITS)) {
        ret.status = EXCEPTION_LOOKUP_FAULT;
        પાછા ret;
    }

    offset = vaddr & MASK(lookup_ret.pt_Bits_ડાબું);
    kernel_vaddr = (word_t)paddr_to_pptr(pte_page_ptr_get_page_base_address(lookup_ret.ptSlot));
    કિંમત = (word_t *)(kernel_vaddr + offset);

    ret.status = EXCEPTION_NONE;
    ret.કિંમત = *કિંમત;
    પાછા ret;
}

શૂન્ય Arch_user_ઢગલો_Trace(tcb_t *tptr)
{
    cap_t thread_મૂળ;
    vspace_મૂળ_t *vspace_મૂળ_2;
    word_t sp;

    thread_મૂળ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    જો (cap_get_capType(thread_મૂળ) != cap_vspace_cap) {
        printf("Invalid vspace\n");
        પાછા;
    }

    vspace_મૂળ_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_મૂળ));
    sp = getRegister(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    જો (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        printf("SP not aligned\n");
        પાછા;
    }

    માટે (unsigned પૂર્ણાંક i = 0; i < CONFIG_USER_ઢગલો_TRACE_LENGTH; i++) {
        word_t address = sp + (i * કદ(word_t));
        readWordFromVSpace_ret_t result = readWordFromVSpace(vspace_મૂળ_2,
                                                             address);
        જો (result.status == EXCEPTION_NONE) {
            printf("0x%"SEL4_PRIx_word": 0x%"SEL4_PRIx_word"\n",
                   address, result.કિંમત);
        } નહીંતર {
            printf("0x%"SEL4_PRIx_word": INVALID\n", address);
        }
    }
}
#endif /* CONFIG_PRINTING */

#જો defined(CONFIG_KERNEL_LOG_BUFFER)
exception_t benchmark_arch_map_logBuffer(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_size_t frameSize;
    pptr_t  frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(ગાંઠ_STATE(ksCurThread), frame_cptr);

    જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    frameSize = cap_frame_cap_get_capFSize(lu_ret.cap);

    જો (frameSize != ARMLargePage) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((શૂન્ય *) frame_pptr);

    *armKSGlobalLogPTE = pte_pte_page_new(
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#નહીંતર
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
    પાછા EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */
