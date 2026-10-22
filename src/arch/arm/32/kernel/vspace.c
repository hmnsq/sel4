/*
 * Copyright 2014, General Dynamics C4 Systems
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

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#endif

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#define SECTIONS_PER_SUPER_SECTION BIT(ARMSuperSectionBits - ARMSectionBits)
#define PAGES_PER_LARGE_PAGE BIT(ARMLargePageBits - ARMSmallPageBits)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#define LAST_BYTE_PTE(PTE,LENGTH) ((word_t)&(PTE)[(LENGTH)-1] + (BIT(PTE_SIZE_BITS)-1))
#define LAST_BYTE_PDE(PDE,LENGTH) ((word_t)&(PDE)[(LENGTH)-1] + (BIT(PDE_SIZE_BITS)-1))

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#define MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#define MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#define ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#define ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ಸ್ಥಿರ ಶೂನ್ಯ setGlobalPD(ಶೂನ್ಯ);

ಸಂರಚನಾ resolve_ret {
    paddr_t frameBase;
    vm_page_size_t frameSize;
    bool_t valid;
};
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ resolve_ret resolve_ret_t;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
ಸ್ಥಿರ bool_t PURE pteCheckIfMapped(pte_t *pte);
ಸ್ಥಿರ bool_t PURE pdeCheckIfMapped(pde_t *pde);

ಸ್ಥಿರ word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ವಿಕಲ್ಪ (vm_rights) {
    ಪಕ್ಷ VMNoAccess:
        ಪ್ರತ್ಯಾಗಮ 0;

    ಪಕ್ಷ VMKernelOnly:
        ಪ್ರತ್ಯಾಗಮ 1;

    ಪಕ್ಷ VMReadOnly:
        ಪ್ರತ್ಯಾಗಮ 2;

    ಪಕ್ಷ VMReadWrite:
        ಪ್ರತ್ಯಾಗಮ 3;

    ಸ್ವಭಾವ:
        fail("Invalid VM rights");
    }
}

#ಅನ್ಯಥಾ
/* AP encoding slightly different. AP only used for kernel mappings which are fixed after boot time */
BOOT_CODE
ಸ್ಥಿರ word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ವಿಕಲ್ಪ (vm_rights) {
    ಪಕ್ಷ VMKernelOnly:
        ಪ್ರತ್ಯಾಗಮ 0;
    ಪಕ್ಷ VMReadWrite:
        ಪ್ರತ್ಯಾಗಮ 1;
    ಪಕ್ಷ VMNoAccess:
        /* RO at PL1 only */
        ಪ್ರತ್ಯಾಗಮ 2;
    ಪಕ್ಷ VMReadOnly:
        ಪ್ರತ್ಯಾಗಮ 3;
    ಸ್ವಭಾವ:
        fail("Invalid VM rights");
    }
}

ಸ್ಥಿರ word_t CONST HAPFromVMRights(vm_rights_t vm_rights)
{
    ವಿಕಲ್ಪ (vm_rights) {
    ಪಕ್ಷ VMKernelOnly:
    ಪಕ್ಷ VMNoAccess:
        ಪ್ರತ್ಯಾಗಮ 0;
    ಪಕ್ಷ VMReadOnly:
        ಪ್ರತ್ಯಾಗಮ 1;
    /*
    case VMWriteOnly:
        return 2;
    */
    ಪಕ್ಷ VMReadWrite:
        ಪ್ರತ್ಯಾಗಮ 3;
    ಸ್ವಭಾವ:
        fail("Invalid VM rights");
    }
}

#endif

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    ಯದಿ (vm_rights == VMNoAccess) {
        ಪ್ರತ್ಯಾಗಮ VMNoAccess;
    }
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

BOOT_CODE ಶೂನ್ಯ map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_attributes_t attributes)
{
    word_t idx = (vaddr & MASK(pageBitsForSize(ARMSection))) >> pageBitsForSize(ARMSmallPage);

    ಪ್ರತಿಜ್ಞಾ(vaddr >= PPTR_ತುದಿ); /* vaddr lies in the region the global PT covers */
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t tex;
    ಯದಿ (vm_attributes_get_armPageCacheable(attributes)) {
        tex = 5; /* TEX = 0b1(Cached)01(Outer Write Allocate) */
    } ಅನ್ಯಥಾ {
        tex = 0;
    }
    armKSGlobalPT[idx] =
        pte_pte_small_new(
            paddr,
            0, /* global */
            SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
            0, /* APX = 0, privileged full access */
            tex,
            APFromVMRights(vm_rights),
            0, /* C (Inner write allocate) */
            1, /* B (Inner write allocate) */
            0  /* executable */
        );
#ಅನ್ಯಥಾ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    armHSGlobalPT[idx] =
        pteS1_pteS1_small_new(
            0, /* Executable */
            0, /* Executable at PL1 */
            0, /* Not contiguous */
            paddr,
            0, /* global */
            1, /* AF -- always set */
            0, /* Not shared */
            APFromVMRights(vm_rights),
            0, /* non secure */
            vm_attributes_get_armPageCacheable(attributes)
        );
#endif /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_CODE ಶೂನ್ಯ map_kernel_window(ಶೂನ್ಯ)
{
    paddr_t  phys;
    word_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = physBase();
    idx = PPTR_BASE >> pageBitsForSize(ARMSection);

    ಯಾವತ್ (idx < BIT(PD_INDEX_BITS) - SECTIONS_PER_SUPER_SECTION) {
        word_t idx2;

        pde = pde_pde_section_new(
                  phys,
                  1, /* SuperSection */
                  0, /* global */
                  SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                  0, /* APX = 0, privileged full access */
                  5, /* TEX = 0b1(Cached)01(Outer Write Allocate) */
                  1, /* VMKernelOnly */
                  1, /* Parity enabled */
                  0, /* Domain 0 */
                  0, /* XN not set */
                  0, /* C (Inner write allocate) */
                  1  /* B (Inner write allocate) */
              );
        ಪ್ರತಿ (idx2 = idx; idx2 < idx + SECTIONS_PER_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += BIT(pageBitsForSize(ARMSuperSection));
        idx += SECTIONS_PER_SUPER_SECTION;
    }

    ಯಾವತ್ (idx < (PPTR_ತುದಿ >> pageBitsForSize(ARMSection))) {
        pde = pde_pde_section_new(
                  phys,
                  0, /* Section */
                  0, /* global */
                  SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                  0, /* APX = 0, privileged full access */
                  5, /* TEX = 0b1(Cached)01(Outer Write Allocate) */
                  1, /* VMKernelOnly */
                  1, /* Parity enabled */
                  0, /* Domain 0 */
                  0, /* XN not set */
                  0, /* C (Inner write allocate) */
                  1  /* B (Inner write allocate) */
              );
        armKSGlobalPD[idx] = pde;
        phys += BIT(pageBitsForSize(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    ಪ್ರತಿಜ್ಞಾ(phys == PADDR_ತುದಿ);

#ifdef CONFIG_KERNEL_LOG_BUFFER
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /* address */
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, BIT(seL4_PageTableBits));

    phys += BIT(pageBitsForSize(ARMSection));
    idx++;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

    /* map page table covering last 1M of virtual address space to page directory */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalPT), /* address */
            true,                        /* P       */
            0                            /* Domain  */
        );

    /* now start initialising the page table */
    memzero(armKSGlobalPT, 1 << seL4_PageTableBits);

    /* map vector table */
    map_kernel_frame(
        addrFromKPPtr(arm_vector_table),
        PPTR_VECTOR_TABLE,
        VMKernelOnly,
        vm_attributes_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#ಅನ್ಯಥಾ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_CODE ಶೂನ್ಯ map_kernel_window(ಶೂನ್ಯ)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t UNUSED pteS2;

    /* Initialise PGD */
    ಪ್ರತಿ (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    ಪ್ರತಿ (idx = 0; idx < (USER_ತುದಿ - 0xC0000000) >> (PT_INDEX_BITS + PAGE_BITS); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = physBase();
    ಪ್ರತಿ (; idx < BIT(PT_INDEX_BITS) - 1; idx++) {
        pde = pdeS1_pdeS1_section_new(
                  0, /* Executable */
                  0, /* Executable in PL1 */
                  0, /* Not contiguous */
                  phys, /* Address */
                  0, /* global */
                  1, /* AF -- always set to 1 */
                  0, /* Not Shareable */
                  0, /* AP: WR at PL1 only */
                  0, /* Not secure */
                  1  /* outer write-back Cacheable */
              );
        armHSGlobalPD[idx] = pde;
        phys += BIT(PT_INDEX_BITS + PAGE_BITS);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << seL4_PageTableBits);
    ಪ್ರತಿ (idx = 0; idx < 256; idx++) {
        pteS1_t pte;
        pte = pteS1_pteS1_small_new(
                  0, /* Executable */
                  0, /* Executable in PL1 */
                  0, /* Not contiguous */
                  phys, /* Address */
                  0, /* global */
                  1, /* AF -- always set to 1 */
                  0, /* Not Shareable */
                  0, /* AP: WR at PL1 only */
                  0, /* Not secure */
                  1  /* outer write-back Cacheable */
              );
        armHSGlobalPT[idx] = pte;
        phys += BIT(PAGE_BITS);
    }
    /* map vector table */
    map_kernel_frame(
        addrFromKPPtr(arm_vector_table),
        PPTR_VECTOR_TABLE,
        VMKernelOnly,
        vm_attributes_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#endif /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

ಸ್ಥಿರ BOOT_CODE ಶೂನ್ಯ map_it_frame_cap(cap_t pd_cap, cap_t frame_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *targetSlot;
    pde_t *pd    = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    ಶೂನ್ಯ  *frame = (ಶೂನ್ಯ *)generic_frame_cap_get_capFBasePtr(frame_cap);
    vptr_t vptr  = generic_frame_cap_get_capFMappedAddress(frame_cap);

    ಪ್ರತಿಜ್ಞಾ(generic_frame_cap_get_capFMappedASID(frame_cap) != 0);

    pd += (vptr >> pageBitsForSize(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pd));
    targetSlot = pt + ((vptr & MASK(pageBitsForSize(ARMSection)))
                       >> pageBitsForSize(ARMSmallPage));
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    *targetSlot = pte_pte_small_new(
                      addrFromPPtr(frame),
                      1, /* not global */
                      SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                      0, /* APX = 0, privileged full access */
                      5, /* TEX = 0b1(Cached)01(Outer Write Allocate) */
                      APFromVMRights(VMReadWrite),
                      0, /* C (Inner write allocate) */
                      1, /* B (Inner write allocate) */
                      !executable
                  );
#ಅನ್ಯಥಾ
    *targetSlot = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(frame),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#endif
}

/* Create a frame cap for the initial thread. */

ಸ್ಥಿರ BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    ಯದಿ (use_large)
        ಪ್ರತ್ಯಾಗಮ
            cap_frame_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_ಕೆಳಗಿನ(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_ಮೇಲಿನ(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ
            cap_small_frame_cap_new(
                ASID_ಕೆಳಗಿನ(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#ifdef CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#endif
                ASID_ಮೇಲಿನ(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

ಸ್ಥಿರ BOOT_CODE ಶೂನ್ಯ map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);
    pde_t *targetSlot = pd + (vptr >> pageBitsForSize(ARMSection));

    ಪ್ರತಿಜ್ಞಾ(cap_page_table_cap_get_capPTIsMapped(pt_cap));

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    *targetSlot = pde_pde_coarse_new(
                      addrFromPPtr(pt), /* address */
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#ಅನ್ಯಥಾ
    *targetSlot = pde_pde_coarse_new(addrFromPPtr(pt));
#endif
}

/* Create a page table for the initial thread */

ಸ್ಥಿರ BOOT_CODE cap_t create_it_page_table_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    ಯದಿ (asid != asidInvalid) {
        map_it_pt_cap(pd, cap);
    }
    ಪ್ರತ್ಯಾಗಮ cap;
}

BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    ಪ್ರತ್ಯಾಗಮ get_n_paging(it_v_reg, PT_INDEX_BITS + PAGE_BITS);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t create_it_address_space(cap_t ಬೇರು_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << seL4_PageDirBits) - 1,
                        addrFromPPtr((ಶೂನ್ಯ *)rootserver.vspace));
    cap_t pd_cap =
        cap_page_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_of_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    ಪ್ರತಿ (pt_vptr = ROUND_DOWN(it_v_reg.start, PT_INDEX_BITS + PAGE_BITS);
         pt_vptr < it_v_reg.end;
         pt_vptr += BIT(PT_INDEX_BITS + PAGE_BITS)) {
        ಯದಿ (!provide_cap(ಬೇರು_cnode_cap,
                         create_it_page_table_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            ಪ್ರತ್ಯಾಗಮ cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    ಪ್ರತ್ಯಾಗಮ pd_cap;
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

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_CODE ಶೂನ್ಯ activate_kernel_vspace(ಶೂನ್ಯ)
{
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    setGlobalPD();
    invalidateLocalTLB();
    lockTLBEntry(PPTR_BASE);
    lockTLBEntry(PPTR_VECTOR_TABLE);
}

#ಅನ್ಯಥಾ

BOOT_CODE ಶೂನ್ಯ activate_kernel_vspace(ಶೂನ್ಯ)
{
    uint32_t r;
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    /* Setup the memory attributes: We use 2 indices (cachable/non-cacheable) */
    setHMAIR((ATTRINDX_NONCACHEABLE << 0) | (ATTRINDX_CACHEABLE << 8), 0);
    setCurrentHypPD(addrFromKPPtr(armHSGlobalPGD));
    invalidateHypTLB();
#ಯದಿ 0 /* Can't lock entries on A15 */
    lockTLBEntry(PPTR_BASE);
    lockTLBEntry(PPTR_VECTOR_TABLE);
#endif
    /* TODO find a better place to init the VMMU */
    r = 0;
    /* Translation range */
    r |= (0x0 << 0);     /* 2^(32 -(0)) input range. */
    r |= (r & 0x8) << 1; /* Sign bit */
    /* starting level */
    r |= (0x0 << 6);     /* Start at second level */
    /* Sharability of tables */
    r |= BIT(8);       /* Inner write-back, write-allocate */
    r |= BIT(10);      /* Outer write-back, write-allocate */
    /* Long descriptor format (not that we have a choice) */
    r |= BIT(31);
    setVTCR(r);
}

#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_CODE ಶೂನ್ಯ write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[ASID_ಕೆಳಗಿನ(IT_ASID)] = PDE_PTR(pptr_of_cap(it_pd_cap));
    armKSASIDTable[ASID_ಮೇಲಿನ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

findPDForASID_ret_t findPDForASID(asid_t asid)
{
    findPDForASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಯದಿ (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

        ret.pd = ರಿಕ್ತ;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    pd = poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
    ಯದಿ (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

        ret.pd = ರಿಕ್ತ;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    ret.pd = pd;
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

    ಯದಿ (unlikely(cap_get_capType(bufferCap) != cap_small_frame_cap &&
                 cap_get_capType(bufferCap) != cap_frame_cap)) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (unlikely(generic_frame_cap_get_capFIsDevice(bufferCap))) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    vm_rights = generic_frame_cap_get_capFVMRights(bufferCap);
    ಯದಿ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr;
        unsigned ಪೂರ್ಣಾಙ್ಕ pageBits;

        basePtr = generic_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(generic_frame_cap_get_capFSize(bufferCap));
        ಪ್ರತ್ಯಾಗಮ (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ಯದಿ (unlikely(cap_get_capType(cap) != cap_small_frame_cap &&
                 cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (unlikely(generic_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (unlikely(vptr & MASK(seL4_IPCBufferSizeBits))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (ಪೂರ್ಣಾಙ್ಕ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    unsigned ಪೂರ್ಣಾಙ್ಕ pdIndex;

    pdIndex = vptr >> (PAGE_BITS + PT_INDEX_BITS);
    ಪ್ರತ್ಯಾಗಮ pd + pdIndex;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pdSlot;

    pdSlot = lookupPDSlot(pd, vptr);

    ಯದಿ (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_INDEX_BITS + PAGE_BITS);

        ret.ptSlot = ರಿಕ್ತ;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ಪ್ರತ್ಯಾಗಮ ret;
    } ಅನ್ಯಥಾ {
        pte_t *pt, *ptSlot;
        unsigned ಪೂರ್ಣಾಙ್ಕ ptIndex;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pdSlot));
        ptIndex = (vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS);
        ptSlot = pt + ptIndex;

        ret.ptSlot = ptSlot;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;
    }
}

ಸ್ಥಿರ pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    unsigned ಪೂರ್ಣಾಙ್ಕ ptIndex;

    ptIndex = (vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS);
    ಪ್ರತ್ಯಾಗಮ pt + ptIndex;
}

ಸ್ಥಿರ ಧ್ರುವ resolve_ret_t default_resolve_ret_t;

ಸ್ಥಿರ resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    ವಿಕಲ್ಪ (pde_ptr_get_pdeType(pde)) {
    ಪಕ್ಷ pde_pde_section:
        ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಯದಿ (pde_pde_section_ptr_get_size(pde)) {
            ret.frameSize = ARMSuperSection;
        } ಅನ್ಯಥಾ {
            ret.frameSize = ARMSection;
        }
#ಅನ್ಯಥಾ
        ಯದಿ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
            ret.frameSize = ARMSuperSection;
        } ಅನ್ಯಥಾ {
            ret.frameSize = ARMSection;
        }
#endif
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        ವಿಕಲ್ಪ (pte_ptr_get_pteType(pte)) {
        ಪಕ್ಷ pte_pte_small:
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            ಯದಿ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } ಅನ್ಯಥಾ {
                ret.frameSize = ARMSmallPage;
            }
#ಅನ್ಯಥಾ
            ret.frameSize = ARMSmallPage;
#endif
            ಪ್ರತ್ಯಾಗಮ ret;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಪಕ್ಷ pte_pte_large:
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            ಪ್ರತ್ಯಾಗಮ ret;
#endif
        ಸ್ವಭಾವ:
            ಭಙ್ಗ;
        }
        ಭಙ್ಗ;
    }
    }

    ret.valid = false;
    ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ pte_t CONST makeUserPTE(vm_page_size_t page_size, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap;

    ap = APFromVMRights(vm_rights);

    ವಿಕಲ್ಪ (page_size) {
    ಪಕ್ಷ ARMSmallPage: {
        ಯದಿ (cacheable) {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    nonexecutable);
        } ಅನ್ಯಥಾ {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    0, /* TEX = 0b000, strongly-ordered. */
                                    ap,
                                    0, 0,
                                    nonexecutable);
        }
        ಭಙ್ಗ;
    }

    ಪಕ್ಷ ARMLargePage: {
        ಯದಿ (cacheable) {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    1 /* reserved */);
        } ಅನ್ಯಥಾ {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    0, /* TEX = 0b000, strongly-ordered */
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 0,
                                    1 /* reserved */);
        }
        ಭಙ್ಗ;
    }

    ಸ್ವಭಾವ:
        fail("Invalid PTE frame type");
    }

#ಅನ್ಯಥಾ

    word_t hap;

    hap = HAPFromVMRights(vm_rights);

    ವಿಕಲ್ಪ (page_size) {
    ಪಕ್ಷ ARMSmallPage: {
        ಯದಿ (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_CACHEABLE /* Cacheable */);
        } ಅನ್ಯಥಾ {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        ಭಙ್ಗ;
    }

    ಪಕ್ಷ ARMLargePage: {
        ಯದಿ (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_CACHEABLE  /* Cacheable */);
        } ಅನ್ಯಥಾ {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        fail("Invalid PTE frame type");
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    ಪ್ರತ್ಯಾಗಮ pte;
}

ಸ್ಥಿರ pde_t CONST makeUserPDE(vm_page_size_t page_size, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, word_t domain,
                               vm_rights_t vm_rights)
{
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap, size2;

    ap = APFromVMRights(vm_rights);
#ಅನ್ಯಥಾ
    word_t hap, size2;

    (ಶೂನ್ಯ)domain;
    hap = HAPFromVMRights(vm_rights);
#endif

    ವಿಕಲ್ಪ (page_size) {
    ಪಕ್ಷ ARMSection:
        size2 = 0;
        ಭಙ್ಗ;

    ಪಕ್ಷ ARMSuperSection:
        size2 = 1;
        ಭಙ್ಗ;

    ಸ್ವಭಾವ:
        fail("Invalid PDE frame type");
    }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಯದಿ (cacheable) {
        ಪ್ರತ್ಯಾಗಮ pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                   0, /* APX = 0, privileged full access */
                                   5, /* TEX = 0b101, outer write-back, write-allocate */
                                   ap, parity, domain, nonexecutable,
                                   0, 1 /* Inner write-back, write-allocate */);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   1, /* shared */
                                   0, /* APX = 0, privileged full access */
                                   0, /* TEX = 0b000, strongly-ordered */
                                   ap, parity, domain, nonexecutable,
                                   0, 0);
    }
#ಅನ್ಯಥಾ
    ಯದಿ (cacheable) {
        ಪ್ರತ್ಯಾಗಮ pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_CACHEABLE /* Cacheable */);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_NONCACHEABLE /* Not cacheable */);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_ಬೇರು(cap_t cap)
{
    ಪ್ರತ್ಯಾಗಮ cap_get_capType(cap) == cap_page_directory_cap &&
           cap_page_directory_cap_get_capPDIsMapped(cap);
}

bool_t CONST isIOSpaceFrameCap(cap_t cap)
{
#ifdef CONFIG_TK1_SMMU
    ಪ್ರತ್ಯಾಗಮ cap_get_capType(cap) == cap_small_frame_cap && cap_small_frame_cap_get_capFIsIOSpace(cap);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ false;
#endif
}

/** Switch to the empty PD on the reserved HW ASID */
ಸ್ಥಿರ ಶೂನ್ಯ setGlobalPD(ಶೂನ್ಯ)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    writeContextIDAndPD(hwASIDReserved, addrFromKPPtr(armUSGlobalPD));
#ಅನ್ಯಥಾ
    /* Before changing the PD ensure all memory accesses have completed */
    dsb();
    /* First switch to global PD on old ASID. Stale TLB entries may exist
       under the old ASID, but no new stale mappings can be added any more. */
    writeTTBR0Ptr(addrFromKPPtr(armKSGlobalPD));
    /* Ensure the PD switch completes before we do anything else */
    isb();
    /* Switch to reserved HW ASID. Only empty/global kernel mappings are
       now available from the TLB. */
    setHardwareASID(hwASIDReserved);
#endif
}

ಶೂನ್ಯ setVMRoot(tcb_t *tcb)
{
    cap_t thread_ಬೇರು;
    asid_t asid;
    pde_t *pd;
    findPDForASID_ret_t find_ret;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    ಯದಿ (cap_get_capType(thread_ಬೇರು) != cap_page_directory_cap ||
        !cap_page_directory_cap_get_capPDIsMapped(thread_ಬೇರು)) {
        setGlobalPD();
        ಪ್ರತ್ಯಾಗಮ;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_ಬೇರು));
    asid = cap_page_directory_cap_get_capPDMappedASID(thread_ಬೇರು);
    find_ret = findPDForASID(asid);
    ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.pd != pd)) {
        setGlobalPD();
        ಪ್ರತ್ಯಾಗಮ;
    }

    armv_contextSwitch(pd, asid);
}

ಸ್ಥಿರ bool_t setVMRootForFlush(pde_t *pd, asid_t asid)
{
    cap_t thread_ಬೇರು;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbVTable)->cap;

    ಯದಿ (cap_get_capType(thread_ಬೇರು) == cap_page_directory_cap &&
        cap_page_directory_cap_get_capPDIsMapped(thread_ಬೇರು) &&
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_ಬೇರು)) == pd) {
        ಪ್ರತ್ಯಾಗಮ false;
    }

    armv_contextSwitch(pd, asid);

    ಪ್ರತ್ಯಾಗಮ true;
}

pde_t *pageTableMapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    findPDForASID_ret_t find_ret;
    pde_t pde;
    unsigned ಪೂರ್ಣಾಙ್ಕ pdIndex;

    find_ret = findPDForASID(asid);
    ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE)) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    pdIndex = vaddr >> (PAGE_BITS + PT_INDEX_BITS);
    pde = find_ret.pd[pdIndex];

    ಯದಿ (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_address(pde)) == pt)) {
        ಪ್ರತ್ಯಾಗಮ find_ret.pd;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    pd = asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(pd);

    pd[PD_ASID_SLOT] = pde_pde_invalid_new(0, false);
}

ಸ್ಥಿರ pde_t PURE loadHWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    pd = asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(pd);

    ಪ್ರತ್ಯಾಗಮ pd[PD_ASID_SLOT];
}

ಸ್ಥಿರ ಶೂನ್ಯ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    pd = asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_SLOT] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t findFreeHWASID(ಶೂನ್ಯ)
{
    word_t hw_asid_offset;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    ಪ್ರತಿ (hw_asid_offset = 0;
         hw_asid_offset <= (word_t)((hw_asid_t) - 1);
         hw_asid_offset ++) {
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

    /* Increment the NextASID index, skipping the reserved ASID 0 on wrap */
    armKSNextASID++;
    ಯದಿ (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    ಪ್ರತ್ಯಾಗಮ hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);
    ಯದಿ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ಪ್ರತ್ಯಾಗಮ pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } ಅನ್ಯಥಾ {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        ಪ್ರತ್ಯಾಗಮ new_hw_asid;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);
    ಯದಿ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

ಶೂನ್ಯ unmapPageTable(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pdSlot;
    unsigned ಪೂರ್ಣಾಙ್ಕ pdIndex;

    pd = pageTableMapped(asid, vaddr, pt);

    ಯದಿ (likely(pd != ರಿಕ್ತ)) {
        pdIndex = vaddr >> (PT_INDEX_BITS + PAGE_BITS);
        pdSlot = pd + pdIndex;

        *pdSlot = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));
        flushTable(pd, asid, vaddr, pt);
    }
}

ಶೂನ್ಯ copyGlobalMappings(pde_t *newPD)
{
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t i;
    pde_t *global_pd = armKSGlobalPD;

    ಪ್ರತಿ (i = PPTR_BASE >> ARMSectionBits; i < BIT(PD_INDEX_BITS); i++) {
        ಯದಿ (i != PD_ASID_SLOT) {
            newPD[i] = global_pd[i];
        }
    }
#endif
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    ವಿಕಲ್ಪ (vm_faultType) {
    ಪಕ್ಷ ARMDataAbort: {
        word_t addr, fault;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (addressTranslateS1(addr) & ~MASK(PAGE_BITS)) | (addr & MASK(PAGE_BITS));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#ಅನ್ಯಥಾ
        addr = getFAR();
        fault = getDFSR();
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
        /* Debug exceptions come in on the Prefetch and Data abort vectors.
         * We have to test the fault-status bits in the IFSR/DFSR to determine
         * if it's a debug exception when one occurs.
         *
         * If it is a debug exception, return early and don't fallthrough to the
         * normal VM Fault handling path.
         */
        ಯದಿ (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(0);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_FAULT;
        }
#endif
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_FAULT;
    }

    ಪಕ್ಷ ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (addressTranslateS1(pc) & ~MASK(PAGE_BITS)) | (pc & MASK(PAGE_BITS));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#ಅನ್ಯಥಾ
        fault = getIFSR();
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
        ಯದಿ (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            ಯದಿ (seL4_Fault_DebugException_get_exceptionReason(current_fault) == seL4_SingleStep
                && !singleStepFaultCounterReady(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
            }
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_FAULT;
        }
#endif
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_FAULT;
    }

    ಸ್ವಭಾವ:
        fail("Invalid VM fault type");
    }
}

ಶೂನ್ಯ deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    unsigned ಪೂರ್ಣಾಙ್ಕ offset;

    /* Haskell error: "ASID pool's base must be aligned" */
    ಪ್ರತಿಜ್ಞಾ((asid_base & MASK(asid_ಕೆಳಗಿನ_Bits)) == 0);

    ಯದಿ (armKSASIDTable[ASID_ಮೇಲಿನ(asid_base)] == pool) {
        ಪ್ರತಿ (offset = 0; offset < BIT(asid_ಕೆಳಗಿನ_Bits); offset++) {
            ಯದಿ (pool->array[offset]) {
                flushSpace(asid_base + offset);
                invalidateASIDEntry(asid_base + offset);
            }
        }
        armKSASIDTable[ASID_ಮೇಲಿನ(asid_base)] = ರಿಕ್ತ;
        setVMRoot(ಗಂಟು_STATE(ksCurThread));
    }
}

ಶೂನ್ಯ deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];

    ಯದಿ (poolPtr != ರಿಕ್ತ && poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = ರಿಕ್ತ;
        setVMRoot(ಗಂಟು_STATE(ksCurThread));
    }
}

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
ಸ್ಥಿರ pte_t pte_pte_invalid_new(ಶೂನ್ಯ)
{
    /* Invalid as every PTE must have bit 0 set (large PTE) or bit 1 set (small
     * PTE). 0 == 'translation fault' in ARM parlance.
     */
    ಪ್ರತ್ಯಾಗಮ (pte_t) {
        {
            0
        }
    };
}
#endif

ಶೂನ್ಯ unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, ಶೂನ್ಯ *pptr)
{
    findPDForASID_ret_t find_ret;
    paddr_t addr = addrFromPPtr(pptr);

    find_ret = findPDForASID(asid);
    ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ವಿಕಲ್ಪ (page_size) {
    ಪಕ್ಷ ARMSmallPage: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            ಪ್ರತ್ಯಾಗಮ;
        }

        ಯದಿ (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಯದಿ (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 0)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
#endif
        ಯದಿ (unlikely(pte_pte_small_ptr_get_address(lu_ret.ptSlot) != addr)) {
            ಪ್ರತ್ಯಾಗಮ;
        }

        *(lu_ret.ptSlot) = pte_pte_invalid_new();
        cleanByVA_PoU((word_t)lu_ret.ptSlot, addrFromPPtr(lu_ret.ptSlot));

        ಭಙ್ಗ;
    }

    ಪಕ್ಷ ARMLargePage: {
        lookupPTSlot_ret_t lu_ret;
        word_t i;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಯದಿ (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_large)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (unlikely(pte_pte_large_ptr_get_address(lu_ret.ptSlot) != addr)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
#ಅನ್ಯಥಾ
        ಯದಿ (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 1)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (unlikely(pte_pte_small_ptr_get_address(lu_ret.ptSlot) != addr)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
#endif

        ಪ್ರತಿ (i = 0; i < PAGES_PER_LARGE_PAGE; i++) {
            lu_ret.ptSlot[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((word_t)&lu_ret.ptSlot[0],
                            LAST_BYTE_PTE(lu_ret.ptSlot, PAGES_PER_LARGE_PAGE),
                            addrFromPPtr(&lu_ret.ptSlot[0]));

        ಭಙ್ಗ;
    }

    ಪಕ್ಷ ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(find_ret.pd, vptr);

        ಯದಿ (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಯದಿ (unlikely(pde_pde_section_ptr_get_size(pd) != 0)) {
#ಅನ್ಯಥಾ
        ಯದಿ (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#endif
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (unlikely(pde_pde_section_ptr_get_address(pd) != addr)) {
            ಪ್ರತ್ಯಾಗಮ;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pd, addrFromPPtr(pd));

        ಭಙ್ಗ;
    }

    ಪಕ್ಷ ARMSuperSection: {
        pde_t *pd;
        word_t i;

        pd = lookupPDSlot(find_ret.pd, vptr);

        ಯದಿ (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            ಪ್ರತ್ಯಾಗಮ;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಯದಿ (unlikely(pde_pde_section_ptr_get_size(pd) != 1)) {
#ಅನ್ಯಥಾ
        ಯದಿ (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#endif
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ (unlikely(pde_pde_section_ptr_get_address(pd) != addr)) {
            ಪ್ರತ್ಯಾಗಮ;
        }

        ಪ್ರತಿ (i = 0; i < SECTIONS_PER_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((word_t)&pd[0], LAST_BYTE_PDE(pd, SECTIONS_PER_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        ಭಙ್ಗ;
    }

    ಸ್ವಭಾವ:
        fail("Invalid ARM page type");
        ಭಙ್ಗ;
    }

    /* Flush the page now that the mapping has been updated */
    flushPage(page_size, find_ret.pd, asid, vptr);
}

ಶೂನ್ಯ flushPage(vm_page_size_t page_size, pde_t *pd, asid_t asid, word_t vptr)
{
    pde_t stored_hw_asid;
    word_t base_addr;
    bool_t ಬೇರು_switched;

    ಪ್ರತಿಜ್ಞಾ((vptr & MASK(pageBitsForSize(page_size))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ಬೇರು_switched = setVMRootForFlush(pd, asid);
    stored_hw_asid = loadHWASID(asid);

    ಯದಿ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        base_addr = vptr & ~MASK(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(base_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        ಯದಿ (ಬೇರು_switched) {
            setVMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }
}

ಶೂನ್ಯ flushTable(pde_t *pd, asid_t asid, word_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t ಬೇರು_switched;

    ಪ್ರತಿಜ್ಞಾ((vptr & MASK(PT_INDEX_BITS + ARMSmallPageBits)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ಬೇರು_switched = setVMRootForFlush(pd, asid);
    stored_hw_asid = loadHWASID(asid);

    ಯದಿ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        ಯದಿ (ಬೇರು_switched) {
            setVMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }
}

ಶೂನ್ಯ flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);

    /* Clean the entire data cache, to guarantee that any VAs mapped
     * in the deleted space are clean (because we can't clean by VA after
     * deleting the space) */
    cleanCaches_PoU();

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    ಯದಿ (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ಶೂನ್ಯ invalidateTLBByASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    ಯದಿ (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t CONST checkVPAlignment(vm_page_size_t sz, word_t w)
{
    ಪ್ರತ್ಯಾಗಮ (w & MASK(pageBitsForSize(sz))) == 0;
}

ಸಂರಚನಾ create_mappings_pte_return {
    exception_t status;
    pte_t pte;
    pte_range_t pte_entries;
};
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ create_mappings_pte_return create_mappings_pte_return_t;

ಸಂರಚನಾ create_mappings_pde_return {
    exception_t status;
    pde_t pde;
    pde_range_t pde_entries;
};
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ create_mappings_pde_return create_mappings_pde_return_t;

ಸ್ಥಿರ create_mappings_pte_return_t
createSafeMappingEntries_PTE
(paddr_t base, word_t vaddr, vm_page_size_t frameSize,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    word_t i;

    ವಿಕಲ್ಪ (frameSize) {

    ಪಕ್ಷ ARMSmallPage:

        ret.pte_entries.base = ರಿಕ್ತ; /* to avoid uninitialised warning */
        ret.pte_entries.length = 1;

        ret.pte = makeUserPTE(ARMSmallPage, base,
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            ಪ್ರತ್ಯಾಗಮ ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಯದಿ (unlikely(pte_ptr_get_pteType(ret.pte_entries.base) ==
                     pte_pte_large)) {
#ಅನ್ಯಥಾ
        ಯದಿ (unlikely(pte_ptr_get_pteType(ret.pte_entries.base) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.base))) {
#endif
            current_syscall_error.type =
                seL4_DeleteFirst;

            ret.status = EXCEPTION_SYSCALL_ERROR;
            ಪ್ರತ್ಯಾಗಮ ret;
        }

        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ ARMLargePage:

        ret.pte_entries.base = ರಿಕ್ತ; /* to avoid uninitialised warning */
        ret.pte_entries.length = PAGES_PER_LARGE_PAGE;

        ret.pte = makeUserPTE(ARMLargePage, base,
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            ಪ್ರತ್ಯಾಗಮ ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;

        ಪ್ರತಿ (i = 0; i < PAGES_PER_LARGE_PAGE; i++) {
            ಯದಿ (unlikely(pte_get_pteType(ret.pte_entries.base[i]) ==
                         pte_pte_small)
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.base[i])
#endif
               ) {
                current_syscall_error.type =
                    seL4_DeleteFirst;

                ret.status = EXCEPTION_SYSCALL_ERROR;
                ಪ್ರತ್ಯಾಗಮ ret;
            }
        }

        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಸ್ವಭಾವ:
        fail("Invalid or unexpected ARM page type.");

    }
}

ಸ್ಥಿರ create_mappings_pde_return_t
createSafeMappingEntries_PDE
(paddr_t base, word_t vaddr, vm_page_size_t frameSize,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_tag_t currentPDEType;
    word_t i;

    ವಿಕಲ್ಪ (frameSize) {

    /* PDE mappings */
    ಪಕ್ಷ ARMSection:
        ret.pde_entries.base = lookupPDSlot(pd, vaddr);
        ret.pde_entries.length = 1;

        ret.pde = makeUserPDE(ARMSection, base,
                              vm_attributes_get_armParityEnabled(attr),
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.base);
        ಯದಿ (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_size(ret.pde_entries.base) != 0))) {
#ಅನ್ಯಥಾ
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.base) != 0))) {
#endif
            current_syscall_error.type =
                seL4_DeleteFirst;
            ret.status = EXCEPTION_SYSCALL_ERROR;

            ಪ್ರತ್ಯಾಗಮ ret;
        }

        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ ARMSuperSection:
        ret.pde_entries.base = lookupPDSlot(pd, vaddr);
        ret.pde_entries.length = SECTIONS_PER_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, base,
                              vm_attributes_get_armParityEnabled(attr),
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              0,
                              vmRights);

        ಪ್ರತಿ (i = 0; i < SECTIONS_PER_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.base[i]);
            ಯದಿ (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_size(ret.pde_entries.base[i]) != 1))) {
#ಅನ್ಯಥಾ
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.base[i]) != 1))) {
#endif
                current_syscall_error.type =
                    seL4_DeleteFirst;
                ret.status = EXCEPTION_SYSCALL_ERROR;

                ಪ್ರತ್ಯಾಗಮ ret;
            }
        }

        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಸ್ವಭಾವ:
        fail("Invalid or unexpected ARM page type.");

    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ vptr_t pageBase(vptr_t vaddr, vm_page_size_t size)
{
    ಪ್ರತ್ಯಾಗಮ vaddr & ~MASK(pageBitsForSize(size));
}

ಸ್ಥಿರ bool_t PURE pteCheckIfMapped(pte_t *pte)
{
    ಪ್ರತ್ಯಾಗಮ pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

ಸ್ಥಿರ bool_t PURE pdeCheckIfMapped(pde_t *pde)
{
    ಪ್ರತ್ಯಾಗಮ pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

ಸ್ಥಿರ ಶೂನ್ಯ doFlush(ಪೂರ್ಣಾಙ್ಕ invLabel, vptr_t start, vptr_t end, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    ಯದಿ (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        end = (vptr_t)paddr_to_pptr(pstart) + (end - start);
        start = (vptr_t)paddr_to_pptr(pstart);
    }
    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ ARMPDClean_Data:
    ಪಕ್ಷ ARMPageClean_Data:
        cleanCacheRange_RAM(start, end, pstart);
        ಭಙ್ಗ;
    ಪಕ್ಷ ARMPDInvalidate_Data:
    ಪಕ್ಷ ARMPageInvalidate_Data:
        invalidateCacheRange_RAM(start, end, pstart);
        ಭಙ್ಗ;
    ಪಕ್ಷ ARMPDCleanInvalidate_Data:
    ಪಕ್ಷ ARMPageCleanInvalidate_Data:
        cleanInvalidateCacheRange_RAM(start, end, pstart);
        ಭಙ್ಗ;
    ಪಕ್ಷ ARMPDUnify_Instruction:
    ಪಕ್ಷ ARMPageUnify_Instruction:
        /* First clean data lines to point of unification
           (L2 cache)... */
        cleanCacheRange_PoU(start, end, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(start, end, pstart);
        /* ...then invalidate branch predictors... */
        branchFlushRange(start, end, pstart);
        /* ... then wait for the completion of invalidations. */
        dsb();
        /* Ensure new instructions come from fresh cache lines. */
        isb();
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

ಸ್ಥಿರ exception_t performPDFlush(ಪೂರ್ಣಾಙ್ಕ invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                  vptr_t end, paddr_t pstart)
{
    bool_t ಬೇರು_switched;

    /* Flush if given a non zero range */
    ಯದಿ (start < end) {
        ಬೇರು_switched = setVMRootForFlush(pd, asid);

        doFlush(invLabel, start, end, pstart);

        ಯದಿ (ಬೇರು_switched) {
            setVMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                                 pde_t pde, pde_t *pdSlot)
{
    ctSlot->cap = cap;
    *pdSlot = pde;
    cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    ಯದಿ (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt);
        clearMemory_PT((ಶೂನ್ಯ *)pt, cap_get_capSizeBits(cap));
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageInvocationMapPTE(asid_t asid, cap_t cap, cte_t *ctSlot, pte_t pte,
                                               pte_range_t pte_entries)
{
    word_t i, j UNUSED;
    bool_t tlbflush_required;

    ctSlot->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.base);

    j = pte_entries.length;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t base_address = pte_pte_small_get_address(pte);
#endif
    ಪ್ರತಿ (i = 0; i < pte_entries.length; i++) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_set_address(pte, base_address + i * BIT(pageBitsForSize(ARMSmallPage)));
#endif
        pte_entries.base[i] = pte;
    }
    cleanCacheRange_PoU((word_t)pte_entries.base,
                        LAST_BYTE_PTE(pte_entries.base, pte_entries.length),
                        addrFromPPtr(pte_entries.base));
    ಯದಿ (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageInvocationMapPDE(asid_t asid, cap_t cap, cte_t *ctSlot, pde_t pde,
                                               pde_range_t pde_entries)
{
    word_t i, j UNUSED;
    bool_t tlbflush_required;

    ctSlot->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.base);

    j = pde_entries.length;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t base_address = pde_pde_section_get_address(pde);
#endif
    ಪ್ರತಿ (i = 0; i < pde_entries.length; i++) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_set_address(pde, base_address + i * BIT(pageBitsForSize(ARMSection)));
#endif
        pde_entries.base[i] = pde;
    }
    cleanCacheRange_PoU((word_t)pde_entries.base,
                        LAST_BYTE_PDE(pde_entries.base, pde_entries.length),
                        addrFromPPtr(pde_entries.base));
    ಯದಿ (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    ಯದಿ (generic_frame_cap_get_capFIsMapped(cap)) {
        unmapPage(generic_frame_cap_get_capFSize(cap),
                  generic_frame_cap_get_capFMappedASID(cap),
                  generic_frame_cap_get_capFMappedAddress(cap),
                  (ಶೂನ್ಯ *)generic_frame_cap_get_capFBasePtr(cap));
    }

    generic_frame_cap_ptr_set_capFMappedAddress(&ctSlot->cap, asidInvalid, 0);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageFlush(ಪೂರ್ಣಾಙ್ಕ invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                    vptr_t end, paddr_t pstart)
{
    bool_t ಬೇರು_switched;

    /* now we can flush. But only if we were given a non zero range */
    ಯದಿ (start < end) {
        ಬೇರು_switched = setVMRootForFlush(pd, asid);

        doFlush(invLabel, start, end, pstart);

        ಯದಿ (ಬೇರು_switched) {
            setVMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performPageGetAddress(ಶೂನ್ಯ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    ಯದಿ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned ಪೂರ್ಣಾಙ್ಕ length = setMR(thread, ipcBuffer, 0, capFBasePtr);
        setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pdCapSlot)
{
    cap_page_directory_cap_ptr_set_capPDMappedASID(&pdCapSlot->cap, asid);
    cap_page_directory_cap_ptr_set_capPDIsMapped(&pdCapSlot->cap, 1);
    poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] =
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCapSlot->cap));

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performASIDControlInvocation(ಶೂನ್ಯ *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_base)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, 1 << ARMSmallPageBits);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ಸೇರಿಸು(cap_asid_pool_cap_new(asid_base, WORD_REF(frame)),
              parent, slot);;
    /* Haskell error: "ASID pool's base must be aligned" */
    ಪ್ರತಿಜ್ಞಾ((asid_base & MASK(asid_ಕೆಳಗಿನ_Bits)) == 0);
    armKSASIDTable[ASID_ಮೇಲಿನ(asid_base)] = (asid_pool_t *)frame;

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeARMPageDirectoryInvocation(word_t invLabel, word_t length,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    word_t *buffer)
{
    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ ARMPDClean_Data:
    ಪಕ್ಷ ARMPDInvalidate_Data:
    ಪಕ್ಷ ARMPDCleanInvalidate_Data:
    ಪಕ್ಷ ARMPDUnify_Instruction: {
        vptr_t start, end;
        paddr_t pstart;
        findPDForASID_ret_t find_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        ಯದಿ (length < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, buffer);
        end =   getSyscallArg(1, buffer);

        /* Check sanity of arguments */
        ಯದಿ (end <= start) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        ಯದಿ (start >= USER_ತುದಿ || end > USER_ತುದಿ) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(cap_get_capType(cap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap));
        asid = cap_page_directory_cap_get_capPDMappedASID(cap);

        find_ret = findPDForASID(asid);
        ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(find_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, start);

        /* Check that there's actually something there. */
        ಯದಿ (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        ಯದಿ (pageBase(start, resolve_ret.frameSize) !=
            pageBase(end - 1, resolve_ret.frameSize)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax =
                pageBase(start, resolve_ret.frameSize) +
                MASK(pageBitsForSize(resolve_ret.frameSize));
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.frameBase
                 + (start & MASK(pageBitsForSize(resolve_ret.frameSize)));

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        ಯದಿ (pstart < physBase() || pstart > PADDR_ತುದಿ || (end - start) > PADDR_ತುದಿ - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performPDFlush(invLabel, pd, asid, start, end - 1, pstart);
    }

    ಸ್ವಭಾವ:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

}

ಸ್ಥಿರ exception_t decodeARMPageTableInvocation(word_t invLabel, word_t length,
                                                cte_t *cte, cap_t cap, word_t *buffer)
{
    word_t vaddr, pdIndex;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_attributes_t attr;
#endif
    cap_t pdCap;
    pde_t *pd, *pdSlot;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    ಯದಿ (invLabel == ARMPageTableUnmap) {
        ಯದಿ (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performPageTableInvocationUnmap(cap, cte);
    }

    ಯದಿ (unlikely(invLabel != ARMPageTableMap)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (unlikely(length < 2 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vmAttributesFromWord(getSyscallArg(1, buffer));
#endif
    pdCap = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                 !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCap));
    asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

    ಯದಿ (unlikely(vaddr >= USER_ತುದಿ)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_ತುದಿ);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    {
        findPDForASID_ret_t find_ret;

        find_ret = findPDForASID(asid);
        ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(find_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
    }

    pdIndex = vaddr >> (PAGE_BITS + PT_INDEX_BITS);
    pdSlot = &pd[pdIndex];
    ಯದಿ (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = seL4_DeleteFirst;

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_attributes_get_armParityEnabled(attr),
              0 /* Domain */
          );
#ಅನ್ಯಥಾ
    pde = pde_pde_coarse_new(paddr);
#endif

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, vaddr);

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ performPageTableInvocationMap(cap, cte, pde, pdSlot);
}

ಸ್ಥಿರ exception_t decodeARMFrameInvocation(word_t invLabel, word_t length,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ ARMPageMap: {
        word_t vaddr, vtop, w_rightsMask;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_page_size_t frameSize;
        vm_attributes_t attr;

        ಯದಿ (unlikely(length < 3 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                seL4_TruncatedMessage;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, buffer);
        w_rightsMask = getSyscallArg(1, buffer);
        attr = vmAttributesFromWord(getSyscallArg(2, buffer));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        frameSize = generic_frame_cap_get_capFSize(cap);
        capVMRights = generic_frame_cap_get_capFVMRights(cap);

        ಯದಿ (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(
                         pdCap));
        asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

        ಯದಿ (generic_frame_cap_get_capFIsMapped(cap)) {
            ಯದಿ (generic_frame_cap_get_capFMappedASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }

            ಯದಿ (generic_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;

                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }
        } ಅನ್ಯಥಾ {
            vtop = vaddr + BIT(pageBitsForSize(frameSize)) - 1;

            ಯದಿ (unlikely(vtop >= USER_ತುದಿ)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_ತುದಿ);
                current_syscall_error.type =
                    seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;

                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            findPDForASID_ret_t find_ret;

            find_ret = findPDForASID(asid);
            ಯದಿ (unlikely(find_ret.status != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource =
                    false;

                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }

            ಯದಿ (unlikely(find_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            maskVMRights(capVMRights, rightsFromWord(w_rightsMask));

        ಯದಿ (unlikely(!checkVPAlignment(frameSize, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((ಶೂನ್ಯ *)
                                   generic_frame_cap_get_capFBasePtr(cap));

        cap = generic_frame_cap_set_capFMappedAddress(cap, asid,
                                                      vaddr);
        ಯದಿ (frameSize == ARMSmallPage || frameSize == ARMLargePage) {
            create_mappings_pte_return_t map_ret;
            map_ret = createSafeMappingEntries_PTE(capFBasePtr, vaddr,
                                                   frameSize, vmRights,
                                                   attr, pd);
            ಯದಿ (unlikely(map_ret.status != EXCEPTION_NONE)) {
#ifdef CONFIG_PRINTING
                ಯದಿ (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#endif
                ಪ್ರತ್ಯಾಗಮ map_ret.status;
            }

            setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಪ್ರತ್ಯಾಗಮ performPageInvocationMapPTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } ಅನ್ಯಥಾ {
            create_mappings_pde_return_t map_ret;
            map_ret = createSafeMappingEntries_PDE(capFBasePtr, vaddr,
                                                   frameSize, vmRights,
                                                   attr, pd);
            ಯದಿ (unlikely(map_ret.status != EXCEPTION_NONE)) {
#ifdef CONFIG_PRINTING
                ಯದಿ (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#endif
                ಪ್ರತ್ಯಾಗಮ map_ret.status;
            }

            setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಪ್ರತ್ಯಾಗಮ performPageInvocationMapPDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    ಪಕ್ಷ ARMPageUnmap: {
#ifdef CONFIG_TK1_SMMU
        ಯದಿ (isIOSpaceFrameCap(cap)) {
            setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಪ್ರತ್ಯಾಗಮ performPageInvocationUnmapIO(cap, cte);
        } ಅನ್ಯಥಾ
#endif
        {
            setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಪ್ರತ್ಯಾಗಮ performPageInvocationUnmap(cap, cte);
        }
    }

#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ ARMPageMapIO: {
        ಪ್ರತ್ಯಾಗಮ decodeARMIOMapInvocation(invLabel, length, cte, cap, buffer);
    }
#endif

    ಪಕ್ಷ ARMPageClean_Data:
    ಪಕ್ಷ ARMPageInvalidate_Data:
    ಪಕ್ಷ ARMPageCleanInvalidate_Data:
    ಪಕ್ಷ ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        findPDForASID_ret_t pd;
        vptr_t start, end;
        paddr_t pstart;
        word_t page_size;
        word_t page_base;

        ಯದಿ (length < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_frame_cap_get_capFMappedASID(cap);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_frame_cap_get_capFBasePtr(cap);
#ಅನ್ಯಥಾ
        vaddr = generic_frame_cap_get_capFMappedAddress(cap);
#endif

        ಯದಿ (unlikely(!generic_frame_cap_get_capFIsMapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        pd = findPDForASID(asid);
        ಯದಿ (unlikely(pd.status != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
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
        page_size = 1 << pageBitsForSize(generic_frame_cap_get_capFSize(cap));
        page_base = addrFromPPtr((ಶೂನ್ಯ *)generic_frame_cap_get_capFBasePtr(cap));

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        ಯದಿ (unlikely(!resolve_ret.valid ||
                     resolve_ret.frameSize != generic_frame_cap_get_capFSize(cap) ||
                     resolve_ret.frameBase != page_base)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#endif

        ಯದಿ (start >= page_size || end > page_size) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = page_base + start;
        start += vaddr;
        end += vaddr;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        ಯದಿ (pstart < physBase() || pstart > PADDR_ತುದಿ || (end - start) > PADDR_ತುದಿ - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performPageFlush(invLabel, pd.pd, asid, start, end - 1, pstart);
    }

    ಪಕ್ಷ ARMPageGetAddress: {


        /* Check that there are enough message registers */
        ಪ್ರತಿಜ್ಞಾ(n_ಸಂದೇಶ_Registers >= 1);

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performPageGetAddress((ಶೂನ್ಯ *)generic_frame_cap_get_capFBasePtr(cap), call);
    }

    ಸ್ವಭಾವ:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t length, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_page_directory_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMPageDirectoryInvocation(invLabel, length, cptr, cte,
                                                cap, buffer);

    ಪಕ್ಷ cap_page_table_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMPageTableInvocation(invLabel, length, cte, cap, buffer);

    ಪಕ್ಷ cap_small_frame_cap:
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
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(length < 2 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ
                     || current_extra_caps.excaprefs[1] == ರಿಕ್ತ)) {
            userError("ASIDControlMakePool: Truncated message.");
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
                     cap_untyped_cap_get_capBlockSize(untyped) !=
                     seL4_ASIDPoolBits || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        ಯದಿ (unlikely(status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            ಪ್ರತ್ಯಾಗಮ status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(ಬೇರು, index, depth);
        ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        ಯದಿ (unlikely(status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            ಪ್ರತ್ಯಾಗಮ status;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performASIDControlInvocation(frame, destSlot,
                                            parentSlot, asid_base);
    }

    ಪಕ್ಷ cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pdCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        ಯದಿ (unlikely(invLabel != ARMASIDPoolAssign)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(current_extra_caps.excaprefs[0] == ರಿಕ್ತ)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        pdCapSlot = current_extra_caps.excaprefs[0];
        pdCap = pdCapSlot->cap;

        ಯದಿ (unlikely(
                cap_get_capType(pdCap) != cap_page_directory_cap ||
                cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ಮೇಲಿನ(cap_asid_pool_cap_get_capASIDBase(cap))];
        ಯದಿ (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        ಪ್ರತಿ (i = 0; i < (1 << asid_ಕೆಳಗಿನ_Bits) && (asid + i == 0 || pool->array[i]); i++);

        ಯದಿ (unlikely(i == 1 << asid_ಕೆಳಗಿನ_Bits)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = seL4_DeleteFirst;

            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ performASIDPoolInvocation(asid, pool, pdCapSlot);
    }

    ಸ್ವಭಾವ:
        fail("Invalid ARM arch cap type");
    }
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
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

    frameSize = generic_frame_cap_get_capFSize(lu_ret.cap);

    ಯದಿ (frameSize != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = generic_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((ಶೂನ್ಯ *) frame_pptr);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
        paddr_t physical_address = ksUserLogBuffer + (idx << seL4_PageBits);

        armKSGlobalLogPT[idx] =
            pte_pte_small_new(
                physical_address,
                0, /* global */
                0, /* Not shared */
                0, /* APX = 0, privileged full access */
                0, /* TEX = 0 */
                1, /* VMKernelOnly */
                1, /* Cacheable */
                0, /* Write-through to minimise perf hit */
                0  /* executable */
            );

        cleanByVA_PoU((vptr_t)&armKSGlobalLogPT[idx], addrFromKPPtr(&armKSGlobalLogPT[idx]));
        invalidateTranslationSingle(KS_LOG_PPTR + (idx * BIT(seL4_PageBits)));
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_DEBUG_BUILD
ಶೂನ್ಯ kernelPrefetchAbort(word_t pc, word_t lr) VISIBLE;
ಶೂನ್ಯ kernelDataAbort(word_t pc) VISIBLE;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

ಶೂನ್ಯ kernelUndefinedInstruction(word_t pc) VISIBLE;

ಶೂನ್ಯ kernelPrefetchAbort(word_t pc, word_t lr)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    printf("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

ಶೂನ್ಯ kernelDataAbort(word_t pc)
{
    printf("\n\nKERNEL DATA ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HDFAR: 0x%"SEL4_PRIx_word" HSR: 0x%"SEL4_PRIx_word"\n",
           getHDFAR(), getHSR());
    halt();
}

ಶೂನ್ಯ kernelUndefinedInstruction(word_t pc)
{
    printf("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    halt();
}

#ಅನ್ಯಥಾ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ಶೂನ್ಯ kernelPrefetchAbort(word_t pc, word_t lr)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("IFSR: 0x%"SEL4_PRIx_word"\n", getIFSR());
    printf("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

ಶೂನ್ಯ kernelDataAbort(word_t pc)
{
    printf("\n\nKERNEL DATA ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("FAR: 0x%"SEL4_PRIx_word" DFSR: 0x%"SEL4_PRIx_word"\n",
           getFAR(), getDFSR());
    halt();
}
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#endif

#ifdef CONFIG_PRINTING
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ readWordFromVSpace_ret {
    exception_t status;
    word_t ಮೌಲ್ಯ;
} readWordFromVSpace_ret_t;

ಸ್ಥಿರ readWordFromVSpace_ret_t readWordFromVSpace(pde_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    lookupPTSlot_ret_t ptSlot;
    pde_t *pdSlot;
    paddr_t paddr;
    word_t offset;
    pptr_t kernel_vaddr;
    word_t *ಮೌಲ್ಯ;

    pdSlot = lookupPDSlot(pd, vaddr);
    ಯದಿ (pde_ptr_get_pdeType(pdSlot) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_address(pdSlot);
        offset = vaddr & MASK(ARMSectionBits);
    } ಅನ್ಯಥಾ {
        ptSlot = lookupPTSlot(pd, vaddr);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        ಯದಿ (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_address(ptSlot.ptSlot);
            ಯದಿ (pte_pte_small_ptr_get_contiguous_hint(ptSlot.ptSlot)) {
                offset = vaddr & MASK(ARMLargePageBits);
            } ಅನ್ಯಥಾ {
                offset = vaddr & MASK(ARMSmallPageBits);
            }
#ಅನ್ಯಥಾ
        ಯದಿ (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_address(ptSlot.ptSlot);
            offset = vaddr & MASK(ARMSmallPageBits);
        } ಅನ್ಯಥಾ ಯದಿ (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_address(ptSlot.ptSlot);
            offset = vaddr & MASK(ARMLargePageBits);
#endif
        } ಅನ್ಯಥಾ {
            ret.status = EXCEPTION_LOOKUP_FAULT;
            ಪ್ರತ್ಯಾಗಮ ret;
        }
    }


    kernel_vaddr = (word_t)paddr_to_pptr(paddr);
    ಮೌಲ್ಯ = (word_t *)(kernel_vaddr + offset);
    ret.status = EXCEPTION_NONE;
    ret.ಮೌಲ್ಯ = *ಮೌಲ್ಯ;
    ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ Arch_user_ರಾಶಿ_Trace(tcb_t *tptr)
{
    cap_t thread_ಬೇರು;
    pde_t *pd;
    word_t sp;
    ಪೂರ್ಣಾಙ್ಕ i;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    ಯದಿ (cap_get_capType(thread_ಬೇರು) != cap_page_directory_cap) {
        printf("Invalid vspace\n");
        ಪ್ರತ್ಯಾಗಮ;
    }

    pd = (pde_t *)pptr_of_cap(thread_ಬೇರು);

    sp = getRegister(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ಯದಿ (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        printf("SP not aligned\n");
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (i = 0; i < CONFIG_USER_ರಾಶಿ_TRACE_LENGTH; i++) {
        word_t address = sp + (i * ಪರಿಮಾಣ(word_t));
        readWordFromVSpace_ret_t result;
        result = readWordFromVSpace(pd, address);
        ಯದಿ (result.status == EXCEPTION_NONE) {
            printf("0x%lx: 0x%lx\n", (long)address, (long)result.ಮೌಲ್ಯ);
        } ಅನ್ಯಥಾ {
            printf("0x%lx: INVALID\n", (long)address);
        }
    }
}
#endif
