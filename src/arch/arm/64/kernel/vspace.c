/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <benchmark/benchmark.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <kernel/boot.h>
#അന്തര്ഭാവ <kernel/cspace.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <kernel/stack.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <machine/debug.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <object/cnode.h>
#അന്തര്ഭാവ <object/untyped.h>
#അന്തര്ഭാവ <arch/api/invocation.h>
#അന്തര്ഭാവ <arch/kernel/vspace.h>
#അന്തര്ഭാവ <linker.h>
#അന്തര്ഭാവ <plat/machine/hardware.h>
#അന്തര്ഭാവ <armv/context_switch.h>
#അന്തര്ഭാവ <arch/object/iospace.h>
#അന്തര്ഭാവ <arch/object/vcpu.h>
#അന്തര്ഭാവ <arch/machine/tlb.h>

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
എണ്ണൽ mair_types {
    DEVICE_nGnRnE = 0,
    DEVICE_nGnRE = 1,
    DEVICE_GRE = 2,
    NORMAL_NC = 3,
    NORMAL = 4,
    NORMAL_WT = 5
};

/* Stage-2 translation memory attributes */
എണ്ണൽ mair_s2_types {
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

ഘടന lookupPTSlot_ret {
    pte_t *ptSlot;
    word_t pt_Bits_ഇടത്;
};
തരനിർവചനം ഘടന lookupPTSlot_ret lookupPTSlot_ret_t;

ഘടന findVSpaceForASID_ret {
    exception_t status;
    vspace_വേര്_t *vspace_വേര്;
};
തരനിർവചനം ഘടന findVSpaceForASID_ret findVSpaceForASID_ret_t;

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

സ്ഥിതം word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    തിരഞ്ഞെടുപ്പ് (vm_rights) {
    അവസ്ഥ VMKernelOnly:
        എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            തിരികെ 0;
        } അല്ലെങ്കിൽ {
            തിരികെ 0;
        }

    അവസ്ഥ VMReadWrite:
        എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            തിരികെ 3;
        } അല്ലെങ്കിൽ {
            തിരികെ 1;
        }

    അവസ്ഥ VMReadOnly:
        എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            തിരികെ 1;
        } അല്ലെങ്കിൽ {
            തിരികെ 3;
        }

    സ്ഥിരസ്ഥിതി:
        fail("Invalid VM rights");
    }
}

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
സ്ഥിതം വരിയിൽ CONST word_t pte_get_AP(pte_t pte)
{
    പ്രതിജ്ഞാ(pte_is_page_type(pte));
    തിരഞ്ഞെടുപ്പ് (pte_get_pte_type(pte)) {
    അവസ്ഥ pte_pte_4k_page:
        തിരികെ pte_pte_4k_page_get_AP(pte);

    അവസ്ഥ pte_pte_page:
        തിരികെ pte_pte_page_get_AP(pte);

    സ്ഥിരസ്ഥിതി:
        തിരികെ 0;
    }
}

സ്ഥിതം vm_rights_t CONST vmRightsFromPTE(pte_t pte)
{
    word_t access_perms = pte_get_AP(pte);
    തിരഞ്ഞെടുപ്പ് (access_perms) {
    അവസ്ഥ 0:
    അവസ്ഥ 2:
        തിരികെ VMKernelOnly;
    അവസ്ഥ 1:
        തിരികെ VMReadWrite;
    അവസ്ഥ 3:
        തിരികെ VMReadOnly;
    സ്ഥിരസ്ഥിതി:
        fail("Invalid AP bit");
    }
}
#endif

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    എങ്കിൽ (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        തിരികെ VMReadOnly;
    }
    എങ്കിൽ (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        എങ്കിൽ (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            തിരികെ VMReadOnly;
        } അല്ലെങ്കിൽ {
            തിരികെ VMReadWrite;
        }
    }
    എങ്കിൽ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    തിരികെ VMKernelOnly;
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
BOOT_CODE ശൂന്യം map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_attributes_t attributes)
{
    പ്രതിജ്ഞാ(vaddr >= PPTR_മുകൾ);

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t uxn = vm_attributes_get_armExecuteNever(attributes);
#അല്ലെങ്കിൽ
    word_t uxn = 1; /* unprivileged execute never */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    word_t attr_index;
    word_t shareable;
    എങ്കിൽ (vm_attributes_get_armPageCacheable(attributes)) {
        attr_index = NORMAL;
        shareable = SMP_TERNARY(SMP_SHARE, 0);
    } അല്ലെങ്കിൽ {
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
BOOT_CODE ശൂന്യം map_kernel_elf_image(pte_t kernel_mapping_pds[])
{
    /** This is not strictly necessary, but to handle a non-aligned KERNEL_ELF_PADDR_BASE
     *  we'd need to do a bit more logic and probably do what riscv64 does
     *  where the *actual* KERNEL_ELF_BASE is rounded up to the alignment.
     *  This is simpler and memory is cheap.
     **/
    compile_assert(elf_paddr_aligned, IS_ALIGNED(KERNEL_ELF_PADDR_BASE_RAW, seL4_LargePageBits));
    compile_assert(elf_base_aligned, IS_ALIGNED(KERNEL_ELF_BASE, seL4_LargePageBits));

    /* Please don't overlap with KDEV_BASE */
    പ്രതിജ്ഞാ(KERNEL_ELF_മുകൾ <= KDEV_BASE);
    /* Please don't overlap with KS_LOG_PPTR */
    പ്രതിജ്ഞാ(KERNEL_ELF_മുകൾ <= KS_LOG_PPTR);

    സ്ഥിരം word_t pd_start = GET_KPT_INDEX(KERNEL_ELF_BASE, KLVL_FRM_ARM_PT_LVL(2));
    /* pd_end and ELF_TOP are both exclusive, so handle that correctly */
    സ്ഥിരം word_t pd_end   = GET_KPT_INDEX(KERNEL_ELF_മുകൾ - 1, KLVL_FRM_ARM_PT_LVL(2)) + 1;

    paddr_t paddr = KERNEL_ELF_PADDR_BASE;
    UNUSED pptr_t vaddr = KERNEL_ELF_BASE;

    വേണ്ടി (word_t pd = pd_start; pd < pd_end; pd++) {
        പ്രതിജ്ഞാ(pd == GET_KPT_INDEX(vaddr, KLVL_FRM_ARM_PT_LVL(2)));

        kernel_mapping_pds[pd] = pte_pte_page_new(
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                     /* XN */ 0,
#അല്ലെങ്കിൽ
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
#അല്ലെങ്കിൽ
/* verify that the physical memory window as at the last entry of the PGD */
compile_assert(physical_window_pgd_location,
               GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(0)) == BIT(PT_INDEX_BITS) - 1);
#endif
/* make sure that the PPTR_BASE and PPTR_TOP are huge paged aligned */
compile_assert(physical_window_base_aligned, IS_ALIGNED(PPTR_BASE, seL4_HugePageBits));
compile_assert(physical_window_മുകൾ_aligned, IS_ALIGNED(PPTR_മുകൾ, seL4_HugePageBits));

/* the PPTR_BASE starts from the bottom of the kernel PUD */
compile_assert(physical_window_bottom_kernel_region,
               GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(1)) == 0);
/* the PPTR_TOP then ends at the second-to-last entry of the PUD
    note: because this is the exclusive value the assert is one higher than
          you would expect.
*/
compile_assert(physical_window_മുകൾ_kernel_region,
               GET_KPT_INDEX(PPTR_മുകൾ, KLVL_FRM_ARM_PT_LVL(1)) == BIT(PT_INDEX_BITS) - 1);

BOOT_CODE ശൂന്യം map_kernel_physical_window(ശൂന്യം)
{
    paddr_t paddr;
    pptr_t vaddr;
    word_t idx;

    /* place all PDs except the last one in PUD */
    വേണ്ടി (idx = GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(1)); idx < GET_KPT_INDEX(PPTR_മുകൾ, KLVL_FRM_ARM_PT_LVL(1));
         idx++) {
        armKSGlobalKernelPUD[idx] = pte_pte_table_new(
                                        addrFromKPPtr(&armKSGlobalKernelPDs[idx][0])
                                    );
    }

    /* map the kernel window using large pages */
    vaddr = PPTR_BASE;
    വേണ്ടി (paddr = PADDR_BASE; paddr < PADDR_മുകൾ; paddr += BIT(seL4_LargePageBits)) {
        armKSGlobalKernelPDs[GET_KPT_INDEX(vaddr, KLVL_FRM_ARM_PT_LVL(1))][GET_KPT_INDEX(vaddr,
                                                                                         KLVL_FRM_ARM_PT_LVL(2))] = pte_pte_page_new(
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                                                                                                        1, // XN
#അല്ലെങ്കിൽ
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

BOOT_CODE ശൂന്യം map_kernel_window(ശൂന്യം)
{
    /* place the PUD into the PGD */
    armKSGlobalKernelPGD[GET_KPT_INDEX(PPTR_BASE, KLVL_FRM_ARM_PT_LVL(0))] =
        pte_pte_table_new(addrFromKPPtr(armKSGlobalKernelPUD));

    map_kernel_physical_window();

    സ്ഥിരം word_t elf_pud_idx = GET_KPT_INDEX(KERNEL_ELF_BASE, KLVL_FRM_ARM_PT_LVL(1));
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
സ്ഥിതം BOOT_CODE ശൂന്യം map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap, bool_t executable)
{
    vspace_വേര്_t *vspace_വേര്_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt;

    vptr_t vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);
    ശൂന്യം *pptr = (ശൂന്യം *)cap_frame_cap_get_capFBasePtr(frame_cap);

    പ്രതിജ്ഞാ(cap_frame_cap_get_capFMappedASID(frame_cap) != 0);

#ifdef AARCH64_VSPACE_S2_START_L1
    pud = vspace_വേര്_2;
#അല്ലെങ്കിൽ
    vspace_വേര്_2 += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0));
    പ്രതിജ്ഞാ(pte_pte_table_ptr_get_present(vspace_വേര്_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_വേര്_2));
#endif
    pud += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
    പ്രതിജ്ഞാ(pte_pte_table_ptr_get_present(pud));
    pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pud));
    pd += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2));
    പ്രതിജ്ഞാ(pte_pte_table_ptr_get_present(pd));
    pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pd));
    *(pt + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(3))) = pte_pte_4k_page_new(
                                                              !executable,                    /* unprivileged execute never */
                                                              pptr_to_paddr(pptr),            /* page_base_address    */
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              0,
#അല്ലെങ്കിൽ
                                                              1,                              /* not global */
#endif
                                                              1,                              /* access flag */
                                                              SMP_TERNARY(SMP_SHARE, 0),              /* Inner-shareable if SMP enabled, otherwise unshared */
                                                              APFromVMRights(VMReadWrite),
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                                                              S2_NORMAL
#അല്ലെങ്കിൽ
                                                              NORMAL
#endif
                                                          );
}

സ്ഥിതം BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    vm_page_size_t frame_size;
    എങ്കിൽ (use_large) {
        frame_size = ARMLargePage;
    } അല്ലെങ്കിൽ {
        frame_size = ARMSmallPage;
    }
    തിരികെ
        cap_frame_cap_new(
            asid,                          /* capFMappedASID */
            pptr,                          /* capFBasePtr */
            frame_size,                    /* capFSize */
            vptr,                          /* capFMappedAddress */
            wordFromVMRights(VMReadWrite), /* capFVMRights */
            false                          /* capFIsDevice */
        );
}

സ്ഥിതം BOOT_CODE ശൂന്യം map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    vspace_വേര്_t *vspace_വേര്_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd;
    pte_t *pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);

    പ്രതിജ്ഞാ(cap_page_table_cap_get_capPTIsMapped(pt_cap));

#ifdef AARCH64_VSPACE_S2_START_L1
    pud = vspace_വേര്_2;
#അല്ലെങ്കിൽ
    vspace_വേര്_2 += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0));
    പ്രതിജ്ഞാ(pte_pte_table_ptr_get_present(vspace_വേര്_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_വേര്_2));
#endif
    pud += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1));
    പ്രതിജ്ഞാ(pte_pte_table_ptr_get_present(pud));
    pd = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(pud));
    *(pd + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(2))) = pte_pte_table_new(
                                                              pptr_to_paddr(pt)
                                                          );
}

സ്ഥിതം BOOT_CODE cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pt_cap(vspace_cap, cap);
    തിരികെ cap;
}

സ്ഥിതം BOOT_CODE ശൂന്യം map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    vspace_വേര്_t *vspace_വേര്_2 = VSPACE_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud;
    pte_t *pd = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pd_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pd_cap);

    പ്രതിജ്ഞാ(cap_page_table_cap_get_capPTIsMapped(pd_cap));

#ifdef AARCH64_VSPACE_S2_START_L1
    pud = vspace_വേര്_2;
#അല്ലെങ്കിൽ
    vspace_വേര്_2 += GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0));
    പ്രതിജ്ഞാ(pte_pte_table_ptr_get_present(vspace_വേര്_2));
    pud = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(vspace_വേര്_2));
#endif
    *(pud + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(1))) = pte_pte_table_new(
                                                               pptr_to_paddr(pd)
                                                           );
}

സ്ഥിതം BOOT_CODE cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,                   /* capPTMappedASID */
              pptr,                   /* capPTBasePtr */
              1,                      /* capPTIsMapped */
              vptr                    /* capPTMappedAddress */
          );
    map_it_pd_cap(vspace_cap, cap);
    തിരികെ cap;
}

#ifndef AARCH64_VSPACE_S2_START_L1
സ്ഥിതം BOOT_CODE ശൂന്യം map_it_pud_cap(cap_t vspace_cap, cap_t pud_cap)
{
    pte_t *pgd = PT_PTR(pptr_of_cap(vspace_cap));
    pte_t *pud = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pud_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pud_cap);

    പ്രതിജ്ഞാ(cap_page_table_cap_get_capPTIsMapped(pud_cap));

    *(pgd + GET_UPT_INDEX(vptr, ULVL_FRM_ARM_PT_LVL(0))) = pte_pte_table_new(
                                                               pptr_to_paddr(pud));
}

സ്ഥിതം BOOT_CODE cap_t create_it_pud_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,               /* capPTMappedASID */
              pptr,               /* capPTBasePtr */
              1,                  /* capPTIsMapped */
              vptr                /* capPTMappedAddress */
          );
    map_it_pud_cap(vspace_cap, cap);
    തിരികെ cap;
}
#endif /* AARCH64_VSPACE_S2_START_L1 */
BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    തിരികെ
#ifndef AARCH64_VSPACE_S2_START_L1
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(0))) +
#endif
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(1))) +
        get_n_paging(it_v_reg, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(2)));
}

BOOT_CODE cap_t create_it_address_space(cap_t വേര്_cnode_cap, v_region_t it_v_reg)
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
    write_slot(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

#ifndef AARCH64_VSPACE_S2_START_L1
    /* Create any PUDs needed for the user land image */
    വേണ്ടി (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(0)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(0))) {
        എങ്കിൽ (!provide_cap(വേര്_cnode_cap, create_it_pud_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            തിരികെ cap_null_cap_new();
        }
    }
#endif
    /* Create any PDs needed for the user land image */
    വേണ്ടി (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(1)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))) {
        എങ്കിൽ (!provide_cap(വേര്_cnode_cap, create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            തിരികെ cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    വേണ്ടി (vptr = ROUND_DOWN(it_v_reg.start, GET_ULVL_PGSIZE_BITS(ULVL_FRM_ARM_PT_LVL(2)));
         vptr < it_v_reg.end;
         vptr += GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))) {
        എങ്കിൽ (!provide_cap(വേര്_cnode_cap, create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))) {
            തിരികെ cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };
    തിരികെ vspace_cap;
}

BOOT_CODE cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    തിരികെ create_it_frame_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large);
    map_it_frame_cap(pd_cap, cap, executable);
    തിരികെ cap;
}

BOOT_CODE ശൂന്യം activate_kernel_vspace(ശൂന്യം)
{
    cleanInvalidateL1Caches();
    set_Current_Kernel_VSpace_വേര്(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalKernelPGD)));

    /* Prevent elf-loader address translation to fill up TLB */
    set_Current_User_VSpace_വേര്(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));

    invalidateLocalTLB();
    lockTLBEntry(KERNEL_ELF_BASE);
}

BOOT_CODE ശൂന്യം write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
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
    ap->array[ASID_താഴെയുള്ള(IT_ASID)] = asid_map;
    armKSASIDTable[ASID_മുകളിലെ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

asid_map_t findMapForASID(asid_t asid)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_മുകളിലെ(asid)];
    എങ്കിൽ (!poolPtr) {
        തിരികെ asid_map_asid_map_none_new();
    }

    തിരികെ poolPtr->array[ASID_താഴെയുള്ള(asid)];
}

സ്ഥിതം findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    എങ്കിൽ (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_വേര്_new();

        ret.vspace_വേര് = ശൂന്യ;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        തിരികെ ret;
    }

    ret.vspace_വേര് = (vspace_വേര്_t *)asid_map_asid_map_vspace_get_vspace_വേര്(asid_map);
    ret.status = EXCEPTION_NONE;
    തിരികെ ret;
}

word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_bufferPtr;
    cap_t bufferCap;
    vm_rights_t vm_rights;

    w_bufferPtr = thread->tcbIPCBuffer;
    bufferCap = TCB_PTR_CTE_PTR(thread, tcbBuffer)->cap;

    എങ്കിൽ (unlikely(cap_get_capType(bufferCap) != cap_frame_cap)) {
        തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (unlikely(cap_frame_cap_get_capFIsDevice(bufferCap))) {
        തിരികെ ശൂന്യ;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(bufferCap);
    എങ്കിൽ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr;
        unsigned പൂർണ്ണസംഖ്യ pageBits;

        basePtr = cap_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(cap_frame_cap_get_capFSize(bufferCap));
        തിരികെ (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } അല്ലെങ്കിൽ {
        തിരികെ ശൂന്യ;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    എങ്കിൽ (cap_get_capType(cap) != cap_frame_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    എങ്കിൽ (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    എങ്കിൽ (!IS_ALIGNED(vptr, seL4_IPCBufferSizeBits)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (പൂർണ്ണസംഖ്യ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം lookupPTSlot_ret_t lookupPTSlot(vspace_വേര്_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    word_t level = UPT_LEVELS - 1;
    pte_t *pt = vspace;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_Bits_ഇടത് = PT_INDEX_BITS * level + seL4_PageBits;
    ret.ptSlot = pt + ((vptr >> ret.pt_Bits_ഇടത്) & MASK(seL4_VSpaceIndexBits));

    ആകുമ്പോൾ (pte_pte_table_ptr_get_present(ret.ptSlot) && likely(level > 0)) {
        level--;
        ret.pt_Bits_ഇടത് -= PT_INDEX_BITS;
        pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ret.ptSlot));
        ret.ptSlot = pt + ((vptr >> ret.pt_Bits_ഇടത്) & MASK(PT_INDEX_BITS));
    }

    തിരികെ ret;
}

/* Note that if the hypervisor support is enabled, the user page tables use
 * stage-2 translation format. Otherwise, they follow the stage-1 translation format.
 */
സ്ഥിതം pte_t makeUserPagePTE(paddr_t paddr, vm_rights_t vm_rights, bool_t cap_read, vm_attributes_t attributes,
                             vm_page_size_t page_size)
{
    bool_t nonexecutable = vm_attributes_get_armExecuteNever(attributes) || !cap_read;
    word_t cacheable = vm_attributes_get_armPageCacheable(attributes);

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t nG = 0; /* not global */
    word_t attridx = cacheable ? S2_NORMAL : S2_DEVICE_nGnRnE;
#അല്ലെങ്കിൽ
    word_t nG = 1; /* not global */
    word_t attridx = cacheable ? NORMAL : DEVICE_nGnRnE;
#endif

    /* Inner-shareable if SMP enabled, otherwise unshared (ignored for devices) */
    word_t shareable = cacheable ? SMP_TERNARY(SMP_SHARE, 0) : 0;

    എങ്കിൽ (page_size == ARMSmallPage) {
        തിരികെ pte_pte_4k_page_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                   shareable, APFromVMRights(vm_rights), attridx);
    } അല്ലെങ്കിൽ {
        തിരികെ pte_pte_page_new(nonexecutable, paddr, nG, 1 /* access flag */,
                                shareable, APFromVMRights(vm_rights), attridx);
    }
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    തിരഞ്ഞെടുപ്പ് (vm_faultType) {
    അവസ്ഥ ARMDataAbort: {
        word_t addr, fault;

        addr = getFAR();
        fault = getDFSR();

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* use the IPA */
        എങ്കിൽ (ARCH_കണ്ണി_STATE(armHSVCPUActive)) {
            addr = GET_PAR_ADDR(addressTranslateS1(addr)) | (addr & MASK(PAGE_BITS));
        }
#endif
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        തിരികെ EXCEPTION_FAULT;
    }

    അവസ്ഥ ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);
        fault = getIFSR();

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        എങ്കിൽ (ARCH_കണ്ണി_STATE(armHSVCPUActive)) {
            pc = GET_PAR_ADDR(addressTranslateS1(pc)) | (pc & MASK(PAGE_BITS));
        }
#endif
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        തിരികെ EXCEPTION_FAULT;
    }

    സ്ഥിരസ്ഥിതി:
        fail("Invalid VM fault type");
    }
}

bool_t CONST is_VTable_വേര്(cap_t cap)
{
    തിരികെ cap_get_capType(cap) == cap_vspace_cap;
}

bool_t CONST is_Valid_Native_വേര്(cap_t cap)
{
    തിരികെ is_VTable_വേര്(cap) &&
           cap_vspace_cap_get_capVSIsMapped(cap);
}

bool_t CONST is_Valid_VTable_വേര്(cap_t cap)
{
    തിരികെ is_Valid_Native_വേര്(cap);
}

ശൂന്യം setVMRoot(tcb_t *tcb)
{
    cap_t thread_വേര്;
    asid_t asid;
    vspace_വേര്_t *vspace_വേര്_2;
    findVSpaceForASID_ret_t find_ret;

    thread_വേര് = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    /* ASID/VMID 0 is never allocated to a real VSpace (see findFreeHWASID()
     * for the hypervisor case), so the empty global VSpace can be installed
     * under it without flushing the TLB. */
    എങ്കിൽ (!is_Valid_Native_വേര്(thread_വേര്)) {
        set_Current_User_VSpace_വേര്(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        തിരികെ;
    }

    vspace_വേര്_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_വേര്));
    asid = cap_vspace_cap_get_capVSMappedASID(thread_വേര്);
    find_ret = findVSpaceForASID(asid);
    എങ്കിൽ (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.vspace_വേര് != vspace_വേര്_2)) {
        set_Current_User_VSpace_വേര്(ttbr_new(hwASIDReserved, addrFromKPPtr(armKSGlobalUserVSpace)));
        തിരികെ;
    }

    armv_contextSwitch(vspace_വേര്_2, asid);
}

സ്ഥിതം bool_t setVMRootForFlush(vspace_വേര്_t *vspace, asid_t asid)
{
    cap_t thread_വേര്;

    thread_വേര് = TCB_PTR_CTE_PTR(കണ്ണി_STATE(ksCurThread), tcbVTable)->cap;

    എങ്കിൽ (cap_get_capType(thread_വേര്) == cap_vspace_cap &&
        cap_vspace_cap_get_capVSIsMapped(thread_വേര്) &&
        VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_വേര്)) == vspace) {
        തിരികെ false;
    }

    armv_contextSwitch(vspace, asid);
    തിരികെ true;
}


#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

സ്ഥിതം വരിയിൽ asid_pool_t *getPoolPtr(asid_t asid)
{
    തിരികെ armKSASIDTable[ASID_മുകളിലെ(asid)];
}

സ്ഥിതം വരിയിൽ asid_map_t getASIDMap(asid_pool_t *poolPtr, asid_t asid)
{
    പ്രതിജ്ഞാ(poolPtr != ശൂന്യ);
    തിരികെ poolPtr->array[ASID_താഴെയുള്ള(asid)];
}

സ്ഥിതം വരിയിൽ ശൂന്യം setASIDMap(asid_pool_t *poolPtr, asid_t asid, asid_map_t asid_map)
{
    പ്രതിജ്ഞാ(poolPtr != ശൂന്യ);
    poolPtr->array[ASID_താഴെയുള്ള(asid)] = asid_map;
}

സ്ഥിതം ശൂന്യം invalidateASID(asid_t asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    പ്രതിജ്ഞാ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_set_stored_hw_vmid(asid_map, 0);
    asid_map = asid_map_asid_map_vspace_set_stored_vmid_valid(asid_map, false);

    setASIDMap(poolPtr, asid, asid_map);
}

സ്ഥിതം ശൂന്യം storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *poolPtr;
    asid_map_t asid_map;

    poolPtr = getPoolPtr(asid);
    asid_map = getASIDMap(poolPtr, asid);
    പ്രതിജ്ഞാ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map = asid_map_asid_map_vspace_set_stored_hw_vmid(asid_map, hw_asid);
    asid_map = asid_map_asid_map_vspace_set_stored_vmid_valid(asid_map, true);

    setASIDMap(poolPtr, asid, asid_map);
    armKSHWASIDTable[hw_asid] = asid;
}

സ്ഥിതം hw_asid_t findFreeHWASID(ശൂന്യം)
{
    word_t hw_asid_offset;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    വേണ്ടി (hw_asid_offset = 0;
         hw_asid_offset <= (word_t)((hw_asid_t) - 1);
         hw_asid_offset++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_offset);
        എങ്കിൽ (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            തിരികെ hw_asid;
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
    എങ്കിൽ (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    തിരികെ hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    എങ്കിൽ (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        തിരികെ asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    } അല്ലെങ്കിൽ {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        തിരികെ new_hw_asid;
    }
}

സ്ഥിതം ശൂന്യം invalidateASIDEntry(asid_t asid)
{
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    എങ്കിൽ (asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        armKSHWASIDTable[asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

#endif

#ifdef CONFIG_ARM_SMMU
സ്ഥിതം word_t getASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_മുകളിലെ(asid)];
    പ്രതിജ്ഞാ(asidPool);

    asid_map_t asid_map = asidPool->array[ASID_താഴെയുള്ള(asid)];
    പ്രതിജ്ഞാ(asid_map_get_type(asid_map) == asid_map_asid_map_vspace);

    തിരികെ asid_map_asid_map_vspace_get_bind_cb(asid_map);
}

ശൂന്യം increaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_മുകളിലെ(asid)];
    പ്രതിജ്ഞാ(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_താഴെയുള്ള(asid)];
    പ്രതിജ്ഞാ(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_set_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) + 1);
}

ശൂന്യം decreaseASIDBindCB(asid_t asid)
{
    asid_pool_t *asidPool;

    asidPool = armKSASIDTable[ASID_മുകളിലെ(asid)];
    പ്രതിജ്ഞാ(asidPool);

    asid_map_t *asid_map = &asidPool->array[ASID_താഴെയുള്ള(asid)];
    പ്രതിജ്ഞാ(asid_map_ptr_get_type(asid_map) == asid_map_asid_map_vspace);

    asid_map_asid_map_vspace_ptr_set_bind_cb(asid_map, asid_map_asid_map_vspace_ptr_get_bind_cb(asid_map) - 1);
}
#endif

സ്ഥിതം വരിയിൽ ശൂന്യം invalidateTLBByASID(asid_t asid)
{
#ifdef CONFIG_ARM_SMMU
    word_t bind_cb = getASIDBindCB(asid);
    എങ്കിൽ (unlikely(bind_cb)) {
        invalidateSMMUTLBByASID(asid, bind_cb);
    }
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    എങ്കിൽ (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        തിരികെ;
    }
    invalidateTranslationASID(asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map));
#അല്ലെങ്കിൽ
    invalidateTranslationASID(asid);
#endif
}

സ്ഥിതം വരിയിൽ ശൂന്യം invalidateTLBByASIDVA(asid_t asid, vptr_t vaddr)
{
#ifdef CONFIG_ARM_SMMU
    word_t bind_cb = getASIDBindCB(asid);
    എങ്കിൽ (unlikely(bind_cb)) {
        invalidateSMMUTLBByASIDVA(asid, vaddr, bind_cb);
    }
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    എങ്കിൽ (!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map)) {
        തിരികെ;
    }
    uint64_t hw_asid = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
    invalidateTranslationSingle((hw_asid << 48) | vaddr >> seL4_PageBits);
#അല്ലെങ്കിൽ
    invalidateTranslationSingle((asid << 48) | vaddr >> seL4_PageBits);
#endif
}


ശൂന്യം unmapPageTable(asid_t asid, vptr_t vptr, pte_t *target_pt)
{
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    എങ്കിൽ (unlikely(find_ret.status != EXCEPTION_NONE)) {
        /* nothing to do */
        തിരികെ;
    }
    pte_t *ptSlot = ശൂന്യ;
    pte_t *pt = (pte_t *)find_ret.vspace_വേര്;

    വേണ്ടി (word_t i = 0; i < UPT_LEVELS - 1 && pt != target_pt; i++) {
        ptSlot = pt + GET_UPT_INDEX(vptr, i);
        എങ്കിൽ (unlikely(!pte_pte_table_ptr_get_present(ptSlot))) {
            /* couldn't find it */
            തിരികെ;
        }
        pt = paddr_to_pptr(pte_pte_table_ptr_get_pt_base_address(ptSlot));
    }

    എങ്കിൽ (pt != target_pt) {
        /* didn't find it */
        തിരികെ;
    }
    /* If we found a pt then ptSlot won't be null */
    പ്രതിജ്ഞാ(ptSlot != ശൂന്യ);
    *ptSlot = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));
    invalidateTLBByASID(asid);
}

ശൂന്യം unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;
    pte_t pte;

    find_ret = findVSpaceForASID(asid);
    എങ്കിൽ (find_ret.status != EXCEPTION_NONE) {
        തിരികെ;
    }

    lu_ret = lookupPTSlot(find_ret.vspace_വേര്, vptr);
    എങ്കിൽ (unlikely(lu_ret.pt_Bits_ഇടത് != pageBitsForSize(page_size))) {
        /* Do nothing if the wrong size object was returned */
        തിരികെ;
    }

    pte = *(lu_ret.ptSlot);
    എങ്കിൽ (!pte_is_page_type(pte)) {
        /* Do nothing if no page is present */
        തിരികെ;
    }

    എങ്കിൽ (pte_get_page_base_address(pte) != pptr_to_paddr((ശൂന്യം *)pptr)) {
        /* Do nothing if the mapped page is not the same physical frame */
        തിരികെ;
    }

    *(lu_ret.ptSlot) = pte_pte_invalid_new();
    cleanByVA_PoU((vptr_t)lu_ret.ptSlot, pptr_to_paddr(lu_ret.ptSlot));
    പ്രതിജ്ഞാ(asid < BIT(16));
    invalidateTLBByASIDVA(asid, vptr);
}

ശൂന്യം deleteASID(asid_t asid, vspace_വേര്_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_മുകളിലെ(asid)];

    എങ്കിൽ (poolPtr != ശൂന്യ) {
        asid_map_t asid_map = poolPtr->array[ASID_താഴെയുള്ള(asid)];
        എങ്കിൽ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_വേര്_t *)asid_map_asid_map_vspace_get_vspace_വേര്(asid_map) == vspace) {
            invalidateTLBByASID(asid);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            invalidateASIDEntry(asid);
#endif
            poolPtr->array[ASID_താഴെയുള്ള(asid)] = asid_map_asid_map_none_new();
            setVMRoot(കണ്ണി_STATE(ksCurThread));
        }
    }
}

ശൂന്യം deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    word_t offset;

    പ്രതിജ്ഞാ((asid_base & MASK(asid_താഴെയുള്ള_Bits)) == 0);

    എങ്കിൽ (armKSASIDTable[ASID_മുകളിലെ(asid_base)] == pool) {
        വേണ്ടി (offset = 0; offset < BIT(asid_താഴെയുള്ള_Bits); offset++) {
            asid_map_t asid_map = pool->array[offset];
            എങ്കിൽ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                invalidateTLBByASID(asid_base + offset);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                invalidateASIDEntry(asid_base + offset);
#endif
            }
        }
        armKSASIDTable[ASID_മുകളിലെ(asid_base)] = ശൂന്യ;
        setVMRoot(കണ്ണി_STATE(ksCurThread));
    }
}

സ്ഥിതം ശൂന്യം doFlush(word_t invLabel, vptr_t start, vptr_t end, paddr_t pstart)
{
    തിരഞ്ഞെടുപ്പ് (invLabel) {
    അവസ്ഥ ARMVSpaceClean_Data:
    അവസ്ഥ ARMPageClean_Data:
        cleanCacheRange_RAM(start, end, pstart);
        നിർത്തുക;

    അവസ്ഥ ARMVSpaceInvalidate_Data:
    അവസ്ഥ ARMPageInvalidate_Data:
        invalidateCacheRange_RAM(start, end, pstart);
        നിർത്തുക;

    അവസ്ഥ ARMVSpaceCleanInvalidate_Data:
    അവസ്ഥ ARMPageCleanInvalidate_Data:
        cleanInvalidateCacheRange_RAM(start, end, pstart);
        നിർത്തുക;

    അവസ്ഥ ARMVSpaceUnify_Instruction:
    അവസ്ഥ ARMPageUnify_Instruction:
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
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

സ്ഥിതം exception_t performVSpaceFlush(word_t invLabel, vspace_വേര്_t *vspace_വേര്_2, asid_t asid,
                                      vptr_t start, vptr_t end, paddr_t pstart)
{

    എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        word_t size = end - start;
        start = (vptr_t)paddr_to_pptr(pstart);
        end = start + size;
        എങ്കിൽ (start < end) {
            doFlush(invLabel, start, end, pstart);
        }
    } അല്ലെങ്കിൽ {
        bool_t വേര്_switched;

        /* Flush if given a non zero range */
        എങ്കിൽ (start < end) {
            വേര്_switched = setVMRootForFlush(vspace_വേര്_2, asid);
            doFlush(invLabel, start, end, pstart);
            എങ്കിൽ (വേര്_switched) {
                setVMRoot(കണ്ണി_STATE(ksCurThread));
            }
        }
    }
    തിരികെ EXCEPTION_NONE;
}


സ്ഥിതം exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot, pte_t pte, pte_t *ptSlot)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));

    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    എങ്കിൽ (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                       cap_page_table_cap_get_capPTMappedAddress(cap), pt);
        clearMemory_PT((ശൂന്യം *)pt, cap_get_capSizeBits(cap));
    }

    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t performPageInvocationMap(asid_t asid, cap_t cap, cte_t *ctSlot,
                                            pte_t pte, pte_t *ptSlot)
{
    bool_t tlbflush_required = pte_ptr_get_valid(ptSlot);

    ctSlot->cap = cap;
    *ptSlot = pte;

    cleanByVA_PoU((vptr_t)ptSlot, pptr_to_paddr(ptSlot));
    എങ്കിൽ (unlikely(tlbflush_required)) {
        പ്രതിജ്ഞാ(asid < BIT(16));
        invalidateTLBByASIDVA(asid, cap_frame_cap_get_capFMappedAddress(cap));
    }

    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    എങ്കിൽ (cap_frame_cap_get_capFMappedASID(cap) != 0) {

        unmapPage(cap_frame_cap_get_capFSize(cap),
                  cap_frame_cap_get_capFMappedASID(cap),
                  cap_frame_cap_get_capFMappedAddress(cap),
                  cap_frame_cap_get_capFBasePtr(cap));
    }

    cap_t slotCap = ctSlot->cap;
    slotCap = cap_frame_cap_set_capFMappedAddress(slotCap, 0);
    slotCap = cap_frame_cap_set_capFMappedASID(slotCap, asidInvalid);
    ctSlot->cap = slotCap;


    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t performPageFlush(word_t invLabel, vspace_വേര്_t *vspace_വേര്_2, asid_t asid,
                                    vptr_t start, vptr_t end, paddr_t pstart)
{
    എങ്കിൽ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* We flush the cache with kernel virtual addresses since
         * the user virtual addresses are not valid in EL2.
         * Switching VMRoot is not required.
         */
        word_t size = end - start;
        start = (vptr_t)paddr_to_pptr(pstart);
        end = start + size;

        എങ്കിൽ (start < end) {
            doFlush(invLabel, start, end, pstart);
        }
    } അല്ലെങ്കിൽ {
        bool_t വേര്_switched;

        എങ്കിൽ (start < end) {
            വേര്_switched = setVMRootForFlush(vspace_വേര്_2, asid);
            doFlush(invLabel, start, end, pstart);
            എങ്കിൽ (വേര്_switched) {
                setVMRoot(കണ്ണി_STATE(ksCurThread));
            }
        }
    }
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t performPageGetAddress(pptr_t base_ptr, bool_t call)
{
    paddr_t base = pptr_to_paddr((ശൂന്യം *)base_ptr);

    tcb_t *thread;
    thread = കണ്ണി_STATE(ksCurThread);
    എങ്കിൽ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned പൂർണ്ണസംഖ്യ length = setMR(thread, ipcBuffer, 0, base);
        setRegister(thread, സന്ദേശം_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Running);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t performASIDControlInvocation(ശൂന്യം *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_base)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, BIT(seL4_ASIDPoolBits));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_തിരുകുക(
        cap_asid_pool_cap_new(
            asid_base,         /* capASIDBase  */
            WORD_REF(frame)    /* capASIDPool  */
        ), parent, slot);

    പ്രതിജ്ഞാ((asid_base & MASK(asid_താഴെയുള്ള_Bits)) == 0);
    armKSASIDTable[ASID_മുകളിലെ(asid_base)] = (asid_pool_t *)frame;

    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t decode_ARMVSpace_വേര്_Invocation(word_t invLabel, word_t length,
                                                 cte_t *cte, cap_t cap, word_t *buffer)
{
    vptr_t start, end;
    paddr_t pstart;
    asid_t asid;
    vspace_വേര്_t *vspace_വേര്_2;
    lookupPTSlot_ret_t resolve_ret;
    findVSpaceForASID_ret_t find_ret;
    pte_t pte;

    തിരഞ്ഞെടുപ്പ് (invLabel) {
    അവസ്ഥ ARMVSpaceClean_Data:
    അവസ്ഥ ARMVSpaceInvalidate_Data:
    അവസ്ഥ ARMVSpaceCleanInvalidate_Data:
    അവസ്ഥ ARMVSpaceUnify_Instruction:

        എങ്കിൽ (length < 2) {
            userError("VSpaceRoot Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, buffer);
        end =   getSyscallArg(1, buffer);

        /* Check sanity of arguments */
        എങ്കിൽ (end <= start) {
            userError("VSpaceRoot Flush: Invalid range.");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_സംഖ്യ = 1;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        എങ്കിൽ (end > USER_മുകൾ) {
            userError("VSpaceRoot Flush: Exceed the user addressable region.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (unlikely(!is_Valid_Native_വേര്(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        /* Make sure that the supplied pgd is ok */
        vspace_വേര്_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap));
        asid = cap_vspace_cap_get_capVSMappedASID(cap);

        find_ret = findVSpaceForASID(asid);
        എങ്കിൽ (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("VSpaceRoot Flush: No VSpace for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (unlikely(find_ret.vspace_വേര് != vspace_വേര്_2)) {
            userError("VSpaceRoot Flush: Invalid VSpace Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = lookupPTSlot(vspace_വേര്_2, start);
        pte = *resolve_ret.ptSlot;

        /* Check that the returned slot is a page. */
        എങ്കിൽ (!pte_is_page_type(pte)) {

            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
            തിരികെ EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        എങ്കിൽ (ROUND_DOWN(start, resolve_ret.pt_Bits_ഇടത്) != ROUND_DOWN(end - 1, resolve_ret.pt_Bits_ഇടത്)) {
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax = ROUND_DOWN(start, resolve_ret.pt_Bits_ഇടത്) +
                                                  MASK(resolve_ret.pt_Bits_ഇടത്);
            userError("VSpaceRoot Flush: cannot cross page boundary, valid range is [0x%lx..0x%lx)",
                      current_syscall_error.rangeErrorMin, current_syscall_error.rangeErrorMax);
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When in EL1, the mapping must be write-able for ARMVSpaceInvalidate_Data */
        എങ്കിൽ (invLabel == ARMVSpaceInvalidate_Data && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMVSpaceInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
#endif

        /* Calculate the physical start address. */
        paddr_t frame_base = pte_get_page_base_address(pte);

        pstart = frame_base + (start & MASK(resolve_ret.pt_Bits_ഇടത്));

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        എങ്കിൽ (pstart < PADDR_BASE || ((end - start) + pstart) > PADDR_മുകൾ) {
            userError("VSpaceRoot Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performVSpaceFlush(invLabel, vspace_വേര്_2, asid, start, end - 1, pstart);

    സ്ഥിരസ്ഥിതി:
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
}


സ്ഥിതം exception_t decodeARMPageTableInvocation(word_t invLabel, word_t length,
                                                cte_t *cte, cap_t cap, word_t *buffer)
{
    cap_t vspace_വേര്_Cap;
    vspace_വേര്_t *vspace_വേര്_2;
    pte_t pte;
    asid_t asid;
    vptr_t vaddr;
    lookupPTSlot_ret_t ptSlot;
    findVSpaceForASID_ret_t find_ret;

    എങ്കിൽ (invLabel == ARMPageTableUnmap) {
        എങ്കിൽ (unlikely(!isFinalCapability(cte))) {
            current_syscall_error.type = seL4_RevokeFirst;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performPageTableInvocationUnmap(cap, cte);
    }

    എങ്കിൽ (unlikely(invLabel != ARMPageTableMap)) {
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    എങ്കിൽ (unlikely(length < 2 || current_extra_caps.excaprefs[0] == ശൂന്യ)) {
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    എങ്കിൽ (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_സംഖ്യ = 0;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
    vspace_വേര്_Cap = current_extra_caps.excaprefs[0]->cap;

    എങ്കിൽ (unlikely(!is_Valid_Native_വേര്(vspace_വേര്_Cap))) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    vspace_വേര്_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_വേര്_Cap));
    asid = cap_vspace_cap_get_capVSMappedASID(vspace_വേര്_Cap);

    എങ്കിൽ (unlikely(vaddr > USER_മുകൾ)) {
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 0;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    find_ret = findVSpaceForASID(asid);
    എങ്കിൽ (unlikely(find_ret.status != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    എങ്കിൽ (unlikely(find_ret.vspace_വേര് != vspace_വേര്_2)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    ptSlot = lookupPTSlot(vspace_വേര്_2, vaddr);

    എങ്കിൽ (unlikely(ptSlot.pt_Bits_ഇടത് == seL4_PageBits || pte_ptr_get_valid(ptSlot.ptSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    pte = pte_pte_table_new(pptr_to_paddr(PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap))));

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, (vaddr & ~MASK(ptSlot.pt_Bits_ഇടത്)));

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ performPageTableInvocationMap(cap, cte, pte, ptSlot.ptSlot);
}

സ്ഥിതം വരിയിൽ bool_t CONST checkVPAlignment(vm_page_size_t sz, word_t w)
{
    തിരികെ (w & MASK(pageBitsForSize(sz))) == 0;
}

സ്ഥിതം exception_t decodeARMFrameInvocation(word_t invLabel, word_t length,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    തിരഞ്ഞെടുപ്പ് (invLabel) {
    അവസ്ഥ ARMPageMap: {
        vptr_t vaddr;
        paddr_t base;
        cap_t vspace_വേര്_Cap;
        vspace_വേര്_t *vspace_വേര്_2;
        asid_t asid, frame_asid;
        vm_rights_t vmRights, capRights;
        vm_page_size_t frameSize;
        vm_attributes_t attributes;
        findVSpaceForASID_ret_t find_ret;

        എങ്കിൽ (unlikely(length < 3 || current_extra_caps.excaprefs[0] == ശൂന്യ)) {
            current_syscall_error.type = seL4_TruncatedMessage;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, buffer);
        attributes = vmAttributesFromWord(getSyscallArg(2, buffer));
        vspace_വേര്_Cap = current_extra_caps.excaprefs[0]->cap;

        frameSize = cap_frame_cap_get_capFSize(cap);
        capRights = cap_frame_cap_get_capFVMRights(cap);
        vmRights = maskVMRights(capRights, rightsFromWord(getSyscallArg(1, buffer)));

        എങ്കിൽ (unlikely(!is_Valid_Native_വേര്(vspace_വേര്_Cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 1;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_വേര്_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspace_വേര്_Cap));
        asid = cap_vspace_cap_get_capVSMappedASID(vspace_വേര്_Cap);

        find_ret = findVSpaceForASID(asid);
        എങ്കിൽ (unlikely(find_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (unlikely(find_ret.vspace_വേര് != vspace_വേര്_2)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 1;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (unlikely(!checkVPAlignment(frameSize, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        /* In the case of remap, the cap should have a valid asid */
        frame_asid = cap_frame_cap_get_capFMappedASID(cap);

        എങ്കിൽ (frame_asid != asidInvalid) {
            എങ്കിൽ (frame_asid != asid) {
                userError("ARMPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_സംഖ്യ = 1;
                തിരികെ EXCEPTION_SYSCALL_ERROR;

            } അല്ലെങ്കിൽ എങ്കിൽ (cap_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_സംഖ്യ = 0;
                തിരികെ EXCEPTION_SYSCALL_ERROR;
            }
        } അല്ലെങ്കിൽ {
            എങ്കിൽ (unlikely(vaddr + BIT(pageBitsForSize(frameSize)) - 1 > USER_മുകൾ)) {
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_സംഖ്യ = 0;
                തിരികെ EXCEPTION_SYSCALL_ERROR;
            }
        }

        cap = cap_frame_cap_set_capFMappedASID(cap, asid);
        cap = cap_frame_cap_set_capFMappedAddress(cap, vaddr);

        base = pptr_to_paddr((ശൂന്യം *)cap_frame_cap_get_capFBasePtr(cap));

        lookupPTSlot_ret_t lu_ret = lookupPTSlot(vspace_വേര്_2, vaddr);
        എങ്കിൽ (unlikely(lu_ret.pt_Bits_ഇടത് != pageBitsForSize(frameSize))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_Bits_ഇടത്);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        bool_t cap_read = (capRights == VMReadOnly || capRights == VMReadWrite);
        pte_t pte = makeUserPagePTE(base, vmRights, cap_read, attributes, frameSize);

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performPageInvocationMap(asid, cap, cte, pte, lu_ret.ptSlot);
    }

    അവസ്ഥ ARMPageUnmap:
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performPageInvocationUnmap(cap, cte);

    അവസ്ഥ ARMPageClean_Data:
    അവസ്ഥ ARMPageInvalidate_Data:
    അവസ്ഥ ARMPageCleanInvalidate_Data:
    അവസ്ഥ ARMPageUnify_Instruction: {
        vptr_t start, end;
        vptr_t vaddr;
        asid_t asid;
        word_t page_size;
        findVSpaceForASID_ret_t find_ret;

        എങ്കിൽ (length < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (unlikely(cap_frame_cap_get_capFMappedASID(cap) == 0)) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = cap_frame_cap_get_capFMappedAddress(cap);
        asid = cap_frame_cap_get_capFMappedASID(cap);

        find_ret = findVSpaceForASID(asid);
        എങ്കിൽ (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("Page Flush: No PGD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, buffer);
        end =   getSyscallArg(1, buffer);

        /* check that the range is sane */
        എങ്കിൽ (end <= start) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_സംഖ്യ = 1;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        /* start and end are currently relative inside this page */
        page_size = BIT(pageBitsForSize(cap_frame_cap_get_capFSize(cap)));
        എങ്കിൽ (start >= page_size || end > page_size) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        word_t pstart = pptr_to_paddr((ശൂന്യം *)cap_frame_cap_get_capFBasePtr(cap)) + start;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window */
        എങ്കിൽ (pstart < PADDR_BASE || ((end - start) + pstart) > PADDR_മുകൾ) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
#അല്ലെങ്കിൽ
        /* When in EL1, we are using the user page table for flushing and need to make sure
           the mapping info in the cap is not stale. */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(find_ret.vspace_വേര്, vaddr);
        pte_t pte = *lu_ret.ptSlot;
        ശൂന്യം *base_ptr = (ശൂന്യം *) cap_frame_cap_get_capFBasePtr(cap);
        എങ്കിൽ (unlikely(lu_ret.pt_Bits_ഇടത് != pageBitsForSize(cap_frame_cap_get_capFSize(cap)) ||
                     !pte_is_page_type(pte) ||
                     pte_get_page_base_address(pte) != pptr_to_paddr(base_ptr))) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        /* When in EL1, the mapping must be writeable for DC IVAC */
        എങ്കിൽ (invLabel == ARMPageInvalidate_Data && vmRightsFromPTE(pte) != VMReadWrite) {
            userError("ARMPageInvalidate_Data: Cannot call on mapping without write rights.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
#endif
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performPageFlush(invLabel, find_ret.vspace_വേര്, asid, vaddr + start, vaddr + end - 1,
                                pstart);
    }

    അവസ്ഥ ARMPageGetAddress:
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performPageGetAddress(cap_frame_cap_get_capFBasePtr(cap), call);

    സ്ഥിരസ്ഥിതി:
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t length, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_vspace_cap:
        തിരികെ decode_ARMVSpace_വേര്_Invocation(invLabel, length, cte, cap, buffer);
    അവസ്ഥ cap_page_table_cap:
        തിരികെ decodeARMPageTableInvocation(invLabel, length, cte, cap, buffer);

    അവസ്ഥ cap_frame_cap:
        തിരികെ decodeARMFrameInvocation(invLabel, length, cte, cap, call, buffer);

    അവസ്ഥ cap_asid_control_cap: {
        word_t i;
        asid_t asid_base;
        word_t index, depth;
        cap_t untyped, വേര്;
        cte_t *parentSlot, *destSlot;
        lookupSlot_ret_t lu_ret;
        ശൂന്യം *frame;
        exception_t status;

        എങ്കിൽ (unlikely(invLabel != ARMASIDControlMakePool)) {
            current_syscall_error.type = seL4_IllegalOperation;

            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (unlikely(length < 2 ||
                     current_extra_caps.excaprefs[0] == ശൂന്യ ||
                     current_extra_caps.excaprefs[1] == ശൂന്യ)) {
            current_syscall_error.type = seL4_TruncatedMessage;

            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        index = getSyscallArg(0, buffer);
        depth = getSyscallArg(1, buffer);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        വേര് = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        വേണ്ടി (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        എങ്കിൽ (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_താഴെയുള്ള_Bits;

        എങ്കിൽ (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_capBlockSize(untyped) != seL4_ASIDPoolBits ||
                     cap_untyped_cap_get_capIsDevice(untyped))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 1;

            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        എങ്കിൽ (unlikely(status != EXCEPTION_NONE)) {
            തിരികെ status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(വേര്, index, depth);
        എങ്കിൽ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            തിരികെ lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        എങ്കിൽ (unlikely(status != EXCEPTION_NONE)) {
            തിരികെ status;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performASIDControlInvocation(frame, destSlot, parentSlot, asid_base);
    }

    അവസ്ഥ cap_asid_pool_cap: {
        cap_t vspaceCap;
        cte_t *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        എങ്കിൽ (unlikely(invLabel != ARMASIDPoolAssign)) {
            current_syscall_error.type = seL4_IllegalOperation;

            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (unlikely(current_extra_caps.excaprefs[0] == ശൂന്യ)) {
            current_syscall_error.type = seL4_TruncatedMessage;

            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        എങ്കിൽ (unlikely(!is_VTable_വേര്(vspaceCap) || cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 1;

            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_മുകളിലെ(cap_asid_pool_cap_get_capASIDBase(cap))];

        എങ്കിൽ (unlikely(!pool)) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_വേര്_new();

            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_സംഖ്യ = 0;

            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        വേണ്ടി (i = 0; i < (1 << asid_താഴെയുള്ള_Bits) && (asid + i == 0
                                               || (asid_map_get_type(pool->array[i]) != asid_map_asid_map_none)); i++);

        എങ്കിൽ (unlikely(i == 1 << asid_താഴെയുള്ള_Bits)) {
            current_syscall_error.type = seL4_DeleteFirst;

            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }

    സ്ഥിരസ്ഥിതി:
        fail("Invalid ARM arch cap type");
    }
}

#ifdef CONFIG_DEBUG_BUILD
ശൂന്യം kernelPrefetchAbort(word_t pc) VISIBLE;
ശൂന്യം kernelDataAbort(word_t pc) VISIBLE;

ശൂന്യം kernelPrefetchAbort(word_t pc)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("ESR (IFSR): 0x%"SEL4_PRIx_word"\n", getIFSR());
    halt();
}

ശൂന്യം kernelDataAbort(word_t pc)
{
    printf("\n\nKERNEL DATA ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("FAR: 0x%"SEL4_PRIx_word" ESR (DFSR): 0x%"SEL4_PRIx_word"\n",
           getFAR(), getDFSR());
    halt();
}
#endif /* CONFIG_DEBUG_BUILD */

#ifdef CONFIG_PRINTING
തരനിർവചനം ഘടന readWordFromVSpace_ret {
    exception_t status;
    word_t മൂല്യം;
} readWordFromVSpace_ret_t;

സ്ഥിതം readWordFromVSpace_ret_t readWordFromVSpace(vspace_വേര്_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    word_t offset;
    pptr_t kernel_vaddr;
    word_t *മൂല്യം;

    lookupPTSlot_ret_t lookup_ret = lookupPTSlot(pd, vaddr);

    /* Check that the returned slot is a page. */
    എങ്കിൽ (!pte_ptr_get_valid(lookup_ret.ptSlot) ||
        (pte_pte_table_ptr_get_present(lookup_ret.ptSlot) && lookup_ret.pt_Bits_ഇടത് > PAGE_BITS)) {
        ret.status = EXCEPTION_LOOKUP_FAULT;
        തിരികെ ret;
    }

    offset = vaddr & MASK(lookup_ret.pt_Bits_ഇടത്);
    kernel_vaddr = (word_t)paddr_to_pptr(pte_page_ptr_get_page_base_address(lookup_ret.ptSlot));
    മൂല്യം = (word_t *)(kernel_vaddr + offset);

    ret.status = EXCEPTION_NONE;
    ret.മൂല്യം = *മൂല്യം;
    തിരികെ ret;
}

ശൂന്യം Arch_user_അട്ടി_Trace(tcb_t *tptr)
{
    cap_t thread_വേര്;
    vspace_വേര്_t *vspace_വേര്_2;
    word_t sp;

    thread_വേര് = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the vspace root */
    എങ്കിൽ (cap_get_capType(thread_വേര്) != cap_vspace_cap) {
        printf("Invalid vspace\n");
        തിരികെ;
    }

    vspace_വേര്_2 = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(thread_വേര്));
    sp = getRegister(tptr, SP_EL0);

    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    എങ്കിൽ (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        printf("SP not aligned\n");
        തിരികെ;
    }

    വേണ്ടി (unsigned പൂർണ്ണസംഖ്യ i = 0; i < CONFIG_USER_അട്ടി_TRACE_LENGTH; i++) {
        word_t address = sp + (i * വലുപ്പം(word_t));
        readWordFromVSpace_ret_t result = readWordFromVSpace(vspace_വേര്_2,
                                                             address);
        എങ്കിൽ (result.status == EXCEPTION_NONE) {
            printf("0x%"SEL4_PRIx_word": 0x%"SEL4_PRIx_word"\n",
                   address, result.മൂല്യം);
        } അല്ലെങ്കിൽ {
            printf("0x%"SEL4_PRIx_word": INVALID\n", address);
        }
    }
}
#endif /* CONFIG_PRINTING */

#എങ്കിൽ defined(CONFIG_KERNEL_LOG_BUFFER)
exception_t benchmark_arch_map_logBuffer(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_size_t frameSize;
    pptr_t  frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(കണ്ണി_STATE(ksCurThread), frame_cptr);

    എങ്കിൽ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    എങ്കിൽ (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    frameSize = cap_frame_cap_get_capFSize(lu_ret.cap);

    എങ്കിൽ (frameSize != ARMLargePage) {
        userError("Invalid frame size. The kernel expects 2M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((ശൂന്യം *) frame_pptr);

    *armKSGlobalLogPTE = pte_pte_page_new(
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                             0, // XN
#അല്ലെങ്കിൽ
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
    തിരികെ EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */
