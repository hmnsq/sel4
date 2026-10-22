/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <benchmark/benchmark.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <kernel/boot.h>
#অন্তর্ভাব <kernel/cspace.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <kernel/stack.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <machine/debug.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <object/cnode.h>
#অন্তর্ভাব <object/untyped.h>
#অন্তর্ভাব <arch/api/invocation.h>
#অন্তর্ভাব <arch/kernel/vspace.h>
#অন্তর্ভাব <linker.h>
#অন্তর্ভাব <plat/machine/hardware.h>
#অন্তর্ভাব <armv/context_switch.h>
#অন্তর্ভাব <arch/object/iospace.h>
#অন্তর্ভাব <arch/object/vcpu.h>
#অন্তর্ভাব <arch/machine/tlb.h>

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#অন্তর্ভাব <benchmark/benchmark_track.h>
#endif

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#define SECTIONS_PER_SUPER_SECTION BIT(ARMSuperSectionBits - ARMSectionBits)
#define PAGES_PER_LARGE_PAGE BIT(ARMLargePageBits - ARMSmallPageBits)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#define LAST_BYTE_PTE(PTE,দৈর্ঘ্য_2) ((word_t)&(PTE)[(দৈর্ঘ্য_2)-1] + (BIT(PTE_আকার_BITS)-1))
#define LAST_BYTE_PDE(PDE,দৈর্ঘ্য_2) ((word_t)&(PDE)[(দৈর্ঘ্য_2)-1] + (BIT(PDE_আকার_BITS)-1))

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#define MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#define MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#define ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#define ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

স্থির শূন্য setGlobalPD(শূন্য);

কাঠামো resolve_ret {
    paddr_t frameBase;
    vm_page_আকার_t frame_আকার;
    bool_t valid;
};
প্রকারলেখ কাঠামো resolve_ret resolve_ret_t;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
স্থির bool_t PURE pteCheckIfMapped(pte_t *pte);
স্থির bool_t PURE pdeCheckIfMapped(pde_t *pde);

স্থির word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    নির্বাচন (vm_rights) {
    ক্ষেত্র VMNoAccess:
        ফেরত 0;

    ক্ষেত্র VMKernelOnly:
        ফেরত 1;

    ক্ষেত্র VMReadOnly:
        ফেরত 2;

    ক্ষেত্র VMReadWrite:
        ফেরত 3;

    পূর্বনির্ধারিত:
        fail("Invalid VM rights");
    }
}

#অন্যথা
/* AP encoding slightly different. AP only used for kernel mappings which are fixed after boot time */
BOOT_CODE
স্থির word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    নির্বাচন (vm_rights) {
    ক্ষেত্র VMKernelOnly:
        ফেরত 0;
    ক্ষেত্র VMReadWrite:
        ফেরত 1;
    ক্ষেত্র VMNoAccess:
        /* RO at PL1 only */
        ফেরত 2;
    ক্ষেত্র VMReadOnly:
        ফেরত 3;
    পূর্বনির্ধারিত:
        fail("Invalid VM rights");
    }
}

স্থির word_t CONST HAPFromVMRights(vm_rights_t vm_rights)
{
    নির্বাচন (vm_rights) {
    ক্ষেত্র VMKernelOnly:
    ক্ষেত্র VMNoAccess:
        ফেরত 0;
    ক্ষেত্র VMReadOnly:
        ফেরত 1;
    /*
    case VMWriteOnly:
        return 2;
    */
    ক্ষেত্র VMReadWrite:
        ফেরত 3;
    পূর্বনির্ধারিত:
        fail("Invalid VM rights");
    }
}

#endif

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    যদি (vm_rights == VMNoAccess) {
        ফেরত VMNoAccess;
    }
    যদি (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        ফেরত VMReadOnly;
    }
    যদি (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        যদি (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            ফেরত VMReadOnly;
        } অন্যথা {
            ফেরত VMReadWrite;
        }
    }
    যদি (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    ফেরত VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_CODE শূন্য map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_attributes_t attributes)
{
    word_t idx = (vaddr & MASK(page_Bits_For_আকার(ARMSection))) >> page_Bits_For_আকার(ARMSmallPage);

    প্রতিজ্ঞা(vaddr >= PPTR_শীর্ষ); /* vaddr lies in the region the global PT covers */
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t tex;
    যদি (vm_attributes_get_armPageCacheable(attributes)) {
        tex = 5; /* TEX = 0b1(Cached)01(Outer Write Allocate) */
    } অন্যথা {
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
#অন্যথা /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
BOOT_CODE শূন্য map_kernel_window(শূন্য)
{
    paddr_t  phys;
    word_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = physBase();
    idx = PPTR_BASE >> page_Bits_For_আকার(ARMSection);

    যখন (idx < BIT(PD_সূচক_BITS) - SECTIONS_PER_SUPER_SECTION) {
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
        জন্য (idx2 = idx; idx2 < idx + SECTIONS_PER_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += BIT(page_Bits_For_আকার(ARMSuperSection));
        idx += SECTIONS_PER_SUPER_SECTION;
    }

    যখন (idx < (PPTR_শীর্ষ >> page_Bits_For_আকার(ARMSection))) {
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
        phys += BIT(page_Bits_For_আকার(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    প্রতিজ্ঞা(phys == PADDR_শীর্ষ);

#ifdef CONFIG_KERNEL_LOG_বাফার
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /* address */
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, BIT(seL4_PageTableBits));

    phys += BIT(page_Bits_For_আকার(ARMSection));
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

#অন্যথা /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_CODE শূন্য map_kernel_window(শূন্য)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t UNUSED pteS2;

    /* Initialise PGD */
    জন্য (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    জন্য (idx = 0; idx < (USER_শীর্ষ - 0xC0000000) >> (PT_সূচক_BITS + PAGE_BITS); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = physBase();
    জন্য (; idx < BIT(PT_সূচক_BITS) - 1; idx++) {
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
        phys += BIT(PT_সূচক_BITS + PAGE_BITS);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << seL4_PageTableBits);
    জন্য (idx = 0; idx < 256; idx++) {
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

স্থির BOOT_CODE শূন্য map_it_frame_cap(cap_t pd_cap, cap_t frame_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *targetSlot;
    pde_t *pd    = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    শূন্য  *frame = (শূন্য *)generic_frame_cap_get_capFBasePtr(frame_cap);
    vptr_t vptr  = generic_frame_cap_get_capFMappedAddress(frame_cap);

    প্রতিজ্ঞা(generic_frame_cap_get_capFMappedASID(frame_cap) != 0);

    pd += (vptr >> page_Bits_For_আকার(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pd));
    targetSlot = pt + ((vptr & MASK(page_Bits_For_আকার(ARMSection)))
                       >> page_Bits_For_আকার(ARMSmallPage));
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
#অন্যথা
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

স্থির BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    যদি (use_large)
        ফেরত
            cap_frame_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_নিচের(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_উপরের(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    অন্যথা
        ফেরত
            cap_small_frame_cap_new(
                ASID_নিচের(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#ifdef CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#endif
                ASID_উপরের(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

স্থির BOOT_CODE শূন্য map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);
    pde_t *targetSlot = pd + (vptr >> page_Bits_For_আকার(ARMSection));

    প্রতিজ্ঞা(cap_page_table_cap_get_capPTIsMapped(pt_cap));

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    *targetSlot = pde_pde_coarse_new(
                      addrFromPPtr(pt), /* address */
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#অন্যথা
    *targetSlot = pde_pde_coarse_new(addrFromPPtr(pt));
#endif
}

/* Create a page table for the initial thread */

স্থির BOOT_CODE cap_t create_it_page_table_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    যদি (asid != asidInvalid) {
        map_it_pt_cap(pd, cap);
    }
    ফেরত cap;
}

BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    ফেরত get_n_paging(it_v_reg, PT_সূচক_BITS + PAGE_BITS);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t create_it_address_space(cap_t মূল_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << seL4_PageDirBits) - 1,
                        addrFromPPtr((শূন্য *)rootserver.vspace));
    cap_t pd_cap =
        cap_page_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_of_cap(মূল_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    জন্য (pt_vptr = ROUND_DOWN(it_v_reg.start, PT_সূচক_BITS + PAGE_BITS);
         pt_vptr < it_v_reg.end;
         pt_vptr += BIT(PT_সূচক_BITS + PAGE_BITS)) {
        যদি (!provide_cap(মূল_cnode_cap,
                         create_it_page_table_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            ফেরত cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    ফেরত pd_cap;
}

BOOT_CODE cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    ফেরত create_it_frame_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large);
    map_it_frame_cap(pd_cap, cap, executable);
    ফেরত cap;
}

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_CODE শূন্য activate_kernel_vspace(শূন্য)
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

#অন্যথা

BOOT_CODE শূন্য activate_kernel_vspace(শূন্য)
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
#যদি 0 /* Can't lock entries on A15 */
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

BOOT_CODE শূন্য write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[ASID_নিচের(IT_ASID)] = PDE_PTR(pptr_of_cap(it_pd_cap));
    armKSASIDTable[ASID_উপরের(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

findPDForASID_ret_t findPDForASID(asid_t asid)
{
    findPDForASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_উপরের(asid)];
    যদি (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_মূল_new();

        ret.pd = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }

    pd = poolPtr->array[ASID_নিচের(asid)];
    যদি (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_মূল_new();

        ret.pd = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    }

    ret.pd = pd;
    ret.status = EXCEPTION_NONE;
    ফেরত ret;
}

word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_বাফার_Ptr;
    cap_t বাফার_Cap;
    vm_rights_t vm_rights;

    w_বাফার_Ptr = thread->tcbIPCBuffer;
    বাফার_Cap = TCB_PTR_CTE_PTR(thread, tcb_বাফার)->cap;

    যদি (unlikely(cap_get_capType(বাফার_Cap) != cap_small_frame_cap &&
                 cap_get_capType(বাফার_Cap) != cap_frame_cap)) {
        ফেরত ফাঁকা;
    }
    যদি (unlikely(generic_frame_cap_get_capFIsDevice(বাফার_Cap))) {
        ফেরত ফাঁকা;
    }

    vm_rights = generic_frame_cap_get_capFVMRights(বাফার_Cap);
    যদি (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr;
        unsigned পূর্ণসংখ্যা pageBits;

        basePtr = generic_frame_cap_get_capFBasePtr(বাফার_Cap);
        pageBits = page_Bits_For_আকার(generic_frame_cap_get_capFSize(বাফার_Cap));
        ফেরত (word_t *)(basePtr + (w_বাফার_Ptr & MASK(pageBits)));
    } অন্যথা {
        ফেরত ফাঁকা;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    যদি (unlikely(cap_get_capType(cap) != cap_small_frame_cap &&
                 cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (unlikely(generic_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (unlikely(vptr & MASK(se_L4_IPCBuffer_আকার_Bits))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (পূর্ণসংখ্যা)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    ফেরত EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    unsigned পূর্ণসংখ্যা pd_সূচক;

    pd_সূচক = vptr >> (PAGE_BITS + PT_সূচক_BITS);
    ফেরত pd + pd_সূচক;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pdSlot;

    pdSlot = lookupPDSlot(pd, vptr);

    যদি (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_সূচক_BITS + PAGE_BITS);

        ret.ptSlot = ফাঁকা;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ফেরত ret;
    } অন্যথা {
        pte_t *pt, *ptSlot;
        unsigned পূর্ণসংখ্যা pt_সূচক;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pdSlot));
        pt_সূচক = (vptr >> PAGE_BITS) & MASK(PT_সূচক_BITS);
        ptSlot = pt + pt_সূচক;

        ret.ptSlot = ptSlot;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
    }
}

স্থির pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    unsigned পূর্ণসংখ্যা pt_সূচক;

    pt_সূচক = (vptr >> PAGE_BITS) & MASK(PT_সূচক_BITS);
    ফেরত pt + pt_সূচক;
}

স্থির ধ্রুবক resolve_ret_t default_resolve_ret_t;

স্থির resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    নির্বাচন (pde_ptr_get_pdeType(pde)) {
    ক্ষেত্র pde_pde_section:
        ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        যদি (pde_pde_section_ptr_get_আকার(pde)) {
            ret.frame_আকার = ARMSuperSection;
        } অন্যথা {
            ret.frame_আকার = ARMSection;
        }
#অন্যথা
        যদি (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.frameBase &= ~MASK(page_Bits_For_আকার(ARMSuperSection));
            ret.frame_আকার = ARMSuperSection;
        } অন্যথা {
            ret.frame_আকার = ARMSection;
        }
#endif
        ফেরত ret;

    ক্ষেত্র pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        নির্বাচন (pte_ptr_get_pteType(pte)) {
        ক্ষেত্র pte_pte_small:
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            যদি (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(page_Bits_For_আকার(ARMLargePage));
                ret.frame_আকার = ARMLargePage;
            } অন্যথা {
                ret.frame_আকার = ARMSmallPage;
            }
#অন্যথা
            ret.frame_আকার = ARMSmallPage;
#endif
            ফেরত ret;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        ক্ষেত্র pte_pte_large:
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frame_আকার = ARMLargePage;
            ফেরত ret;
#endif
        পূর্বনির্ধারিত:
            বিরতি;
        }
        বিরতি;
    }
    }

    ret.valid = false;
    ফেরত ret;
}

স্থির pte_t CONST makeUserPTE(vm_page_আকার_t page_আকার, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap;

    ap = APFromVMRights(vm_rights);

    নির্বাচন (page_আকার) {
    ক্ষেত্র ARMSmallPage: {
        যদি (cacheable) {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    nonexecutable);
        } অন্যথা {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    0, /* TEX = 0b000, strongly-ordered. */
                                    ap,
                                    0, 0,
                                    nonexecutable);
        }
        বিরতি;
    }

    ক্ষেত্র ARMLargePage: {
        যদি (cacheable) {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    1 /* reserved */);
        } অন্যথা {
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
        বিরতি;
    }

    পূর্বনির্ধারিত:
        fail("Invalid PTE frame type");
    }

#অন্যথা

    word_t hap;

    hap = HAPFromVMRights(vm_rights);

    নির্বাচন (page_আকার) {
    ক্ষেত্র ARMSmallPage: {
        যদি (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_CACHEABLE /* Cacheable */);
        } অন্যথা {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        বিরতি;
    }

    ক্ষেত্র ARMLargePage: {
        যদি (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_CACHEABLE  /* Cacheable */);
        } অন্যথা {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        বিরতি;
    }
    পূর্বনির্ধারিত:
        fail("Invalid PTE frame type");
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    ফেরত pte;
}

স্থির pde_t CONST makeUserPDE(vm_page_আকার_t page_আকার, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, word_t domain,
                               vm_rights_t vm_rights)
{
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap, size2;

    ap = APFromVMRights(vm_rights);
#অন্যথা
    word_t hap, size2;

    (শূন্য)domain;
    hap = HAPFromVMRights(vm_rights);
#endif

    নির্বাচন (page_আকার) {
    ক্ষেত্র ARMSection:
        size2 = 0;
        বিরতি;

    ক্ষেত্র ARMSuperSection:
        size2 = 1;
        বিরতি;

    পূর্বনির্ধারিত:
        fail("Invalid PDE frame type");
    }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    যদি (cacheable) {
        ফেরত pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                   0, /* APX = 0, privileged full access */
                                   5, /* TEX = 0b101, outer write-back, write-allocate */
                                   ap, parity, domain, nonexecutable,
                                   0, 1 /* Inner write-back, write-allocate */);
    } অন্যথা {
        ফেরত pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   1, /* shared */
                                   0, /* APX = 0, privileged full access */
                                   0, /* TEX = 0b000, strongly-ordered */
                                   ap, parity, domain, nonexecutable,
                                   0, 0);
    }
#অন্যথা
    যদি (cacheable) {
        ফেরত pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_CACHEABLE /* Cacheable */);
    } অন্যথা {
        ফেরত pde_pde_section_new(
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

bool_t CONST is_Valid_VTable_মূল(cap_t cap)
{
    ফেরত cap_get_capType(cap) == cap_page_directory_cap &&
           cap_page_directory_cap_get_capPDIsMapped(cap);
}

bool_t CONST isIOSpaceFrameCap(cap_t cap)
{
#ifdef CONFIG_TK1_SMMU
    ফেরত cap_get_capType(cap) == cap_small_frame_cap && cap_small_frame_cap_get_capFIsIOSpace(cap);
#অন্যথা
    ফেরত false;
#endif
}

/** Switch to the empty PD on the reserved HW ASID */
স্থির শূন্য setGlobalPD(শূন্য)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    writeContextIDAndPD(hwASIDReserved, addrFromKPPtr(armUSGlobalPD));
#অন্যথা
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

শূন্য setVMRoot(tcb_t *tcb)
{
    cap_t thread_মূল;
    asid_t asid;
    pde_t *pd;
    findPDForASID_ret_t find_ret;

    thread_মূল = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    যদি (cap_get_capType(thread_মূল) != cap_page_directory_cap ||
        !cap_page_directory_cap_get_capPDIsMapped(thread_মূল)) {
        setGlobalPD();
        ফেরত;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_মূল));
    asid = cap_page_directory_cap_get_capPDMappedASID(thread_মূল);
    find_ret = findPDForASID(asid);
    যদি (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.pd != pd)) {
        setGlobalPD();
        ফেরত;
    }

    armv_contextSwitch(pd, asid);
}

স্থির bool_t setVMRootForFlush(pde_t *pd, asid_t asid)
{
    cap_t thread_মূল;

    thread_মূল = TCB_PTR_CTE_PTR(গ্রন্থি_STATE(ksCurThread), tcbVTable)->cap;

    যদি (cap_get_capType(thread_মূল) == cap_page_directory_cap &&
        cap_page_directory_cap_get_capPDIsMapped(thread_মূল) &&
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_মূল)) == pd) {
        ফেরত false;
    }

    armv_contextSwitch(pd, asid);

    ফেরত true;
}

pde_t *pageTableMapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    findPDForASID_ret_t find_ret;
    pde_t pde;
    unsigned পূর্ণসংখ্যা pd_সূচক;

    find_ret = findPDForASID(asid);
    যদি (unlikely(find_ret.status != EXCEPTION_NONE)) {
        ফেরত ফাঁকা;
    }

    pd_সূচক = vaddr >> (PAGE_BITS + PT_সূচক_BITS);
    pde = find_ret.pd[pd_সূচক];

    যদি (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_address(pde)) == pt)) {
        ফেরত find_ret.pd;
    } অন্যথা {
        ফেরত ফাঁকা;
    }
}

স্থির শূন্য invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_উপরের(asid)];
    প্রতিজ্ঞা(asidPool);

    pd = asidPool->array[ASID_নিচের(asid)];
    প্রতিজ্ঞা(pd);

    pd[PD_ASID_SLOT] = pde_pde_invalid_new(0, false);
}

স্থির pde_t PURE loadHWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_উপরের(asid)];
    প্রতিজ্ঞা(asidPool);

    pd = asidPool->array[ASID_নিচের(asid)];
    প্রতিজ্ঞা(pd);

    ফেরত pd[PD_ASID_SLOT];
}

স্থির শূন্য storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_উপরের(asid)];
    প্রতিজ্ঞা(asidPool);

    pd = asidPool->array[ASID_নিচের(asid)];
    প্রতিজ্ঞা(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_SLOT] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t findFreeHWASID(শূন্য)
{
    word_t hw_asid_offset;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    জন্য (hw_asid_offset = 0;
         hw_asid_offset <= (word_t)((hw_asid_t) - 1);
         hw_asid_offset ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_offset);
        যদি (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            ফেরত hw_asid;
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
    যদি (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    ফেরত hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);
    যদি (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ফেরত pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } অন্যথা {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        ফেরত new_hw_asid;
    }
}

স্থির শূন্য invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);
    যদি (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

শূন্য unmapPageTable(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pdSlot;
    unsigned পূর্ণসংখ্যা pd_সূচক;

    pd = pageTableMapped(asid, vaddr, pt);

    যদি (likely(pd != ফাঁকা)) {
        pd_সূচক = vaddr >> (PT_সূচক_BITS + PAGE_BITS);
        pdSlot = pd + pd_সূচক;

        *pdSlot = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));
        flushTable(pd, asid, vaddr, pt);
    }
}

শূন্য copyGlobalMappings(pde_t *newPD)
{
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t i;
    pde_t *global_pd = armKSGlobalPD;

    জন্য (i = PPTR_BASE >> ARMSectionBits; i < BIT(PD_সূচক_BITS); i++) {
        যদি (i != PD_ASID_SLOT) {
            newPD[i] = global_pd[i];
        }
    }
#endif
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    নির্বাচন (vm_faultType) {
    ক্ষেত্র ARMDataAbort: {
        word_t addr, fault;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (addressTranslateS1(addr) & ~MASK(PAGE_BITS)) | (addr & MASK(PAGE_BITS));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#অন্যথা
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
        যদি (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(0);
            ফেরত EXCEPTION_FAULT;
        }
#endif
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        ফেরত EXCEPTION_FAULT;
    }

    ক্ষেত্র ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (addressTranslateS1(pc) & ~MASK(PAGE_BITS)) | (pc & MASK(PAGE_BITS));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#অন্যথা
        fault = getIFSR();
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
        যদি (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            যদি (seL4_Fault_DebugException_get_exceptionReason(current_fault) == seL4_SingleStep
                && !single_Step_Fault_গণক_Ready(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                ফেরত EXCEPTION_NONE;
            }
            ফেরত EXCEPTION_FAULT;
        }
#endif
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        ফেরত EXCEPTION_FAULT;
    }

    পূর্বনির্ধারিত:
        fail("Invalid VM fault type");
    }
}

শূন্য deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    unsigned পূর্ণসংখ্যা offset;

    /* Haskell error: "ASID pool's base must be aligned" */
    প্রতিজ্ঞা((asid_base & MASK(asid_নিচের_Bits)) == 0);

    যদি (armKSASIDTable[ASID_উপরের(asid_base)] == pool) {
        জন্য (offset = 0; offset < BIT(asid_নিচের_Bits); offset++) {
            যদি (pool->array[offset]) {
                flushSpace(asid_base + offset);
                invalidateASIDEntry(asid_base + offset);
            }
        }
        armKSASIDTable[ASID_উপরের(asid_base)] = ফাঁকা;
        setVMRoot(গ্রন্থি_STATE(ksCurThread));
    }
}

শূন্য deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_উপরের(asid)];

    যদি (poolPtr != ফাঁকা && poolPtr->array[ASID_নিচের(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_নিচের(asid)] = ফাঁকা;
        setVMRoot(গ্রন্থি_STATE(ksCurThread));
    }
}

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
স্থির pte_t pte_pte_invalid_new(শূন্য)
{
    /* Invalid as every PTE must have bit 0 set (large PTE) or bit 1 set (small
     * PTE). 0 == 'translation fault' in ARM parlance.
     */
    ফেরত (pte_t) {
        {
            0
        }
    };
}
#endif

শূন্য unmapPage(vm_page_আকার_t page_আকার, asid_t asid, vptr_t vptr, শূন্য *pptr)
{
    findPDForASID_ret_t find_ret;
    paddr_t addr = addrFromPPtr(pptr);

    find_ret = findPDForASID(asid);
    যদি (unlikely(find_ret.status != EXCEPTION_NONE)) {
        ফেরত;
    }

    নির্বাচন (page_আকার) {
    ক্ষেত্র ARMSmallPage: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            ফেরত;
        }

        যদি (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            ফেরত;
        }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        যদি (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 0)) {
            ফেরত;
        }
#endif
        যদি (unlikely(pte_pte_small_ptr_get_address(lu_ret.ptSlot) != addr)) {
            ফেরত;
        }

        *(lu_ret.ptSlot) = pte_pte_invalid_new();
        cleanByVA_PoU((word_t)lu_ret.ptSlot, addrFromPPtr(lu_ret.ptSlot));

        বিরতি;
    }

    ক্ষেত্র ARMLargePage: {
        lookupPTSlot_ret_t lu_ret;
        word_t i;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            ফেরত;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        যদি (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_large)) {
            ফেরত;
        }
        যদি (unlikely(pte_pte_large_ptr_get_address(lu_ret.ptSlot) != addr)) {
            ফেরত;
        }
#অন্যথা
        যদি (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            ফেরত;
        }
        যদি (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 1)) {
            ফেরত;
        }
        যদি (unlikely(pte_pte_small_ptr_get_address(lu_ret.ptSlot) != addr)) {
            ফেরত;
        }
#endif

        জন্য (i = 0; i < PAGES_PER_LARGE_PAGE; i++) {
            lu_ret.ptSlot[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((word_t)&lu_ret.ptSlot[0],
                            LAST_BYTE_PTE(lu_ret.ptSlot, PAGES_PER_LARGE_PAGE),
                            addrFromPPtr(&lu_ret.ptSlot[0]));

        বিরতি;
    }

    ক্ষেত্র ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(find_ret.pd, vptr);

        যদি (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            ফেরত;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        যদি (unlikely(pde_pde_section_ptr_get_আকার(pd) != 0)) {
#অন্যথা
        যদি (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#endif
            ফেরত;
        }
        যদি (unlikely(pde_pde_section_ptr_get_address(pd) != addr)) {
            ফেরত;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pd, addrFromPPtr(pd));

        বিরতি;
    }

    ক্ষেত্র ARMSuperSection: {
        pde_t *pd;
        word_t i;

        pd = lookupPDSlot(find_ret.pd, vptr);

        যদি (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            ফেরত;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        যদি (unlikely(pde_pde_section_ptr_get_আকার(pd) != 1)) {
#অন্যথা
        যদি (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#endif
            ফেরত;
        }
        যদি (unlikely(pde_pde_section_ptr_get_address(pd) != addr)) {
            ফেরত;
        }

        জন্য (i = 0; i < SECTIONS_PER_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((word_t)&pd[0], LAST_BYTE_PDE(pd, SECTIONS_PER_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        বিরতি;
    }

    পূর্বনির্ধারিত:
        fail("Invalid ARM page type");
        বিরতি;
    }

    /* Flush the page now that the mapping has been updated */
    flushPage(page_আকার, find_ret.pd, asid, vptr);
}

শূন্য flushPage(vm_page_আকার_t page_আকার, pde_t *pd, asid_t asid, word_t vptr)
{
    pde_t stored_hw_asid;
    word_t base_addr;
    bool_t মূল_switched;

    প্রতিজ্ঞা((vptr & MASK(page_Bits_For_আকার(page_আকার))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    মূল_switched = setVMRootForFlush(pd, asid);
    stored_hw_asid = loadHWASID(asid);

    যদি (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        base_addr = vptr & ~MASK(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(base_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        যদি (মূল_switched) {
            setVMRoot(গ্রন্থি_STATE(ksCurThread));
        }
    }
}

শূন্য flushTable(pde_t *pd, asid_t asid, word_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t মূল_switched;

    প্রতিজ্ঞা((vptr & MASK(PT_সূচক_BITS + ARMSmallPageBits)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    মূল_switched = setVMRootForFlush(pd, asid);
    stored_hw_asid = loadHWASID(asid);

    যদি (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        যদি (মূল_switched) {
            setVMRoot(গ্রন্থি_STATE(ksCurThread));
        }
    }
}

শূন্য flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);

    /* Clean the entire data cache, to guarantee that any VAs mapped
     * in the deleted space are clean (because we can't clean by VA after
     * deleting the space) */
    cleanCaches_PoU();

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    যদি (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ফেরত;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

শূন্য invalidateTLBByASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    যদি (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ফেরত;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

স্থির অন্তঃরেখ bool_t CONST checkVPAlignment(vm_page_আকার_t sz, word_t w)
{
    ফেরত (w & MASK(page_Bits_For_আকার(sz))) == 0;
}

কাঠামো create_mappings_pte_return {
    exception_t status;
    pte_t pte;
    pte_range_t pte_entries;
};
প্রকারলেখ কাঠামো create_mappings_pte_return create_mappings_pte_return_t;

কাঠামো create_mappings_pde_return {
    exception_t status;
    pde_t pde;
    pde_range_t pde_entries;
};
প্রকারলেখ কাঠামো create_mappings_pde_return create_mappings_pde_return_t;

স্থির create_mappings_pte_return_t
createSafeMappingEntries_PTE
(paddr_t base, word_t vaddr, vm_page_আকার_t frame_আকার,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    word_t i;

    নির্বাচন (frame_আকার) {

    ক্ষেত্র ARMSmallPage:

        ret.pte_entries.base = ফাঁকা; /* to avoid uninitialised warning */
        ret.pte_entries.দৈর্ঘ্য = 1;

        ret.pte = makeUserPTE(ARMSmallPage, base,
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            ফেরত ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        যদি (unlikely(pte_ptr_get_pteType(ret.pte_entries.base) ==
                     pte_pte_large)) {
#অন্যথা
        যদি (unlikely(pte_ptr_get_pteType(ret.pte_entries.base) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.base))) {
#endif
            current_syscall_error.type =
                seL4_DeleteFirst;

            ret.status = EXCEPTION_SYSCALL_ERROR;
            ফেরত ret;
        }

        ret.status = EXCEPTION_NONE;
        ফেরত ret;

    ক্ষেত্র ARMLargePage:

        ret.pte_entries.base = ফাঁকা; /* to avoid uninitialised warning */
        ret.pte_entries.দৈর্ঘ্য = PAGES_PER_LARGE_PAGE;

        ret.pte = makeUserPTE(ARMLargePage, base,
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            ফেরত ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;

        জন্য (i = 0; i < PAGES_PER_LARGE_PAGE; i++) {
            যদি (unlikely(pte_get_pteType(ret.pte_entries.base[i]) ==
                         pte_pte_small)
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.base[i])
#endif
               ) {
                current_syscall_error.type =
                    seL4_DeleteFirst;

                ret.status = EXCEPTION_SYSCALL_ERROR;
                ফেরত ret;
            }
        }

        ret.status = EXCEPTION_NONE;
        ফেরত ret;

    পূর্বনির্ধারিত:
        fail("Invalid or unexpected ARM page type.");

    }
}

স্থির create_mappings_pde_return_t
createSafeMappingEntries_PDE
(paddr_t base, word_t vaddr, vm_page_আকার_t frame_আকার,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_tag_t currentPDEType;
    word_t i;

    নির্বাচন (frame_আকার) {

    /* PDE mappings */
    ক্ষেত্র ARMSection:
        ret.pde_entries.base = lookupPDSlot(pd, vaddr);
        ret.pde_entries.দৈর্ঘ্য = 1;

        ret.pde = makeUserPDE(ARMSection, base,
                              vm_attributes_get_armParityEnabled(attr),
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.base);
        যদি (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_আকার(ret.pde_entries.base) != 0))) {
#অন্যথা
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.base) != 0))) {
#endif
            current_syscall_error.type =
                seL4_DeleteFirst;
            ret.status = EXCEPTION_SYSCALL_ERROR;

            ফেরত ret;
        }

        ret.status = EXCEPTION_NONE;
        ফেরত ret;

    ক্ষেত্র ARMSuperSection:
        ret.pde_entries.base = lookupPDSlot(pd, vaddr);
        ret.pde_entries.দৈর্ঘ্য = SECTIONS_PER_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, base,
                              vm_attributes_get_armParityEnabled(attr),
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              0,
                              vmRights);

        জন্য (i = 0; i < SECTIONS_PER_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.base[i]);
            যদি (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_আকার(ret.pde_entries.base[i]) != 1))) {
#অন্যথা
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.base[i]) != 1))) {
#endif
                current_syscall_error.type =
                    seL4_DeleteFirst;
                ret.status = EXCEPTION_SYSCALL_ERROR;

                ফেরত ret;
            }
        }

        ret.status = EXCEPTION_NONE;
        ফেরত ret;

    পূর্বনির্ধারিত:
        fail("Invalid or unexpected ARM page type.");

    }
}

স্থির অন্তঃরেখ vptr_t pageBase(vptr_t vaddr, vm_page_আকার_t আকার_2)
{
    ফেরত vaddr & ~MASK(page_Bits_For_আকার(আকার_2));
}

স্থির bool_t PURE pteCheckIfMapped(pte_t *pte)
{
    ফেরত pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

স্থির bool_t PURE pdeCheckIfMapped(pde_t *pde)
{
    ফেরত pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

স্থির শূন্য doFlush(পূর্ণসংখ্যা invLabel, vptr_t start, vptr_t end, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        end = (vptr_t)paddr_to_pptr(pstart) + (end - start);
        start = (vptr_t)paddr_to_pptr(pstart);
    }
    নির্বাচন (invLabel) {
    ক্ষেত্র ARMPDClean_তথ্য:
    ক্ষেত্র ARMPage_Clean_তথ্য:
        cleanCacheRange_RAM(start, end, pstart);
        বিরতি;
    ক্ষেত্র ARMPDInvalidate_তথ্য:
    ক্ষেত্র ARMPage_Invalidate_তথ্য:
        invalidateCacheRange_RAM(start, end, pstart);
        বিরতি;
    ক্ষেত্র ARMPDClean_Invalidate_তথ্য:
    ক্ষেত্র ARMPage_Clean_Invalidate_তথ্য:
        cleanInvalidateCacheRange_RAM(start, end, pstart);
        বিরতি;
    ক্ষেত্র ARMPDUnify_Instruction:
    ক্ষেত্র ARMPageUnify_Instruction:
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
        বিরতি;
    পূর্বনির্ধারিত:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

স্থির exception_t performPDFlush(পূর্ণসংখ্যা invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                  vptr_t end, paddr_t pstart)
{
    bool_t মূল_switched;

    /* Flush if given a non zero range */
    যদি (start < end) {
        মূল_switched = setVMRootForFlush(pd, asid);

        doFlush(invLabel, start, end, pstart);

        যদি (মূল_switched) {
            setVMRoot(গ্রন্থি_STATE(ksCurThread));
        }
    }

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                                 pde_t pde, pde_t *pdSlot)
{
    ctSlot->cap = cap;
    *pdSlot = pde;
    cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    যদি (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt);
        clearMemory_PT((শূন্য *)pt, cap_get_cap_আকার_Bits(cap));
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performPageInvocationMapPTE(asid_t asid, cap_t cap, cte_t *ctSlot, pte_t pte,
                                               pte_range_t pte_entries)
{
    word_t i, j UNUSED;
    bool_t tlbflush_required;

    ctSlot->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.base);

    j = pte_entries.দৈর্ঘ্য;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t base_address = pte_pte_small_get_address(pte);
#endif
    জন্য (i = 0; i < pte_entries.দৈর্ঘ্য; i++) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_set_address(pte, base_address + i * BIT(page_Bits_For_আকার(ARMSmallPage)));
#endif
        pte_entries.base[i] = pte;
    }
    cleanCacheRange_PoU((word_t)pte_entries.base,
                        LAST_BYTE_PTE(pte_entries.base, pte_entries.দৈর্ঘ্য),
                        addrFromPPtr(pte_entries.base));
    যদি (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performPageInvocationMapPDE(asid_t asid, cap_t cap, cte_t *ctSlot, pde_t pde,
                                               pde_range_t pde_entries)
{
    word_t i, j UNUSED;
    bool_t tlbflush_required;

    ctSlot->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.base);

    j = pde_entries.দৈর্ঘ্য;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t base_address = pde_pde_section_get_address(pde);
#endif
    জন্য (i = 0; i < pde_entries.দৈর্ঘ্য; i++) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_set_address(pde, base_address + i * BIT(page_Bits_For_আকার(ARMSection)));
#endif
        pde_entries.base[i] = pde;
    }
    cleanCacheRange_PoU((word_t)pde_entries.base,
                        LAST_BYTE_PDE(pde_entries.base, pde_entries.দৈর্ঘ্য),
                        addrFromPPtr(pde_entries.base));
    যদি (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    যদি (generic_frame_cap_get_capFIsMapped(cap)) {
        unmapPage(generic_frame_cap_get_capFSize(cap),
                  generic_frame_cap_get_capFMappedASID(cap),
                  generic_frame_cap_get_capFMappedAddress(cap),
                  (শূন্য *)generic_frame_cap_get_capFBasePtr(cap));
    }

    generic_frame_cap_ptr_set_capFMappedAddress(&ctSlot->cap, asidInvalid, 0);

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performPageFlush(পূর্ণসংখ্যা invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                    vptr_t end, paddr_t pstart)
{
    bool_t মূল_switched;

    /* now we can flush. But only if we were given a non zero range */
    যদি (start < end) {
        মূল_switched = setVMRootForFlush(pd, asid);

        doFlush(invLabel, start, end, pstart);

        যদি (মূল_switched) {
            setVMRoot(গ্রন্থি_STATE(ksCurThread));
        }
    }

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performPageGetAddress(শূন্য *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = গ্রন্থি_STATE(ksCurThread);
    যদি (call) {
        word_t *ipc_বাফার = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned পূর্ণসংখ্যা দৈর্ঘ্য = setMR(thread, ipc_বাফার, 0, capFBasePtr);
        setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, দৈর্ঘ্য)));
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pdCapSlot)
{
    cap_page_directory_cap_ptr_set_capPDMappedASID(&pdCapSlot->cap, asid);
    cap_page_directory_cap_ptr_set_capPDIsMapped(&pdCapSlot->cap, 1);
    poolPtr->array[ASID_নিচের(asid)] =
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCapSlot->cap));

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t performASIDControlInvocation(শূন্য *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_base)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_cap_Free_সূচক(&(parent->cap),
                                         MAX_FREE_সূচক(cap_untyped_cap_get_cap_Block_আকার(parent->cap)));

    memzero(frame, 1 << ARMSmallPageBits);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ঢোকাও(cap_asid_pool_cap_new(asid_base, WORD_REF(frame)),
              parent, slot);;
    /* Haskell error: "ASID pool's base must be aligned" */
    প্রতিজ্ঞা((asid_base & MASK(asid_নিচের_Bits)) == 0);
    armKSASIDTable[ASID_উপরের(asid_base)] = (asid_pool_t *)frame;

    ফেরত EXCEPTION_NONE;
}

স্থির exception_t decodeARMPageDirectoryInvocation(word_t invLabel, word_t দৈর্ঘ্য,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    word_t *বাফার)
{
    নির্বাচন (invLabel) {
    ক্ষেত্র ARMPDClean_তথ্য:
    ক্ষেত্র ARMPDInvalidate_তথ্য:
    ক্ষেত্র ARMPDClean_Invalidate_তথ্য:
    ক্ষেত্র ARMPDUnify_Instruction: {
        vptr_t start, end;
        paddr_t pstart;
        findPDForASID_ret_t find_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        যদি (দৈর্ঘ্য < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, বাফার);
        end =   getSyscallArg(1, বাফার);

        /* Check sanity of arguments */
        যদি (end <= start) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 1;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        যদি (start >= USER_শীর্ষ || end > USER_শীর্ষ) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (unlikely(cap_get_capType(cap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap));
        asid = cap_page_directory_cap_get_capPDMappedASID(cap);

        find_ret = findPDForASID(asid);
        যদি (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (unlikely(find_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, start);

        /* Check that there's actually something there. */
        যদি (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
            ফেরত EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        যদি (pageBase(start, resolve_ret.frame_আকার) !=
            pageBase(end - 1, resolve_ret.frame_আকার)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax =
                pageBase(start, resolve_ret.frame_আকার) +
                MASK(page_Bits_For_আকার(resolve_ret.frame_আকার));
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.frameBase
                 + (start & MASK(page_Bits_For_আকার(resolve_ret.frame_আকার)));

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        যদি (pstart < physBase() || pstart > PADDR_শীর্ষ || (end - start) > PADDR_শীর্ষ - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performPDFlush(invLabel, pd, asid, start, end - 1, pstart);
    }

    পূর্বনির্ধারিত:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

}

স্থির exception_t decodeARMPageTableInvocation(word_t invLabel, word_t দৈর্ঘ্য,
                                                cte_t *cte, cap_t cap, word_t *বাফার)
{
    word_t vaddr, pd_সূচক;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_attributes_t attr;
#endif
    cap_t pdCap;
    pde_t *pd, *pdSlot;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    যদি (invLabel == ARMPageTableUnmap) {
        যদি (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performPageTableInvocationUnmap(cap, cte);
    }

    যদি (unlikely(invLabel != ARMPageTableMap)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (unlikely(দৈর্ঘ্য < 2 || current_extra_caps.excaprefs[0] == ফাঁকা)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, বাফার);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vmAttributesFromWord(getSyscallArg(1, বাফার));
#endif
    pdCap = current_extra_caps.excaprefs[0]->cap;

    যদি (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                 !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCap));
    asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

    যদি (unlikely(vaddr >= USER_শীর্ষ)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_শীর্ষ);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 0;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    {
        findPDForASID_ret_t find_ret;

        find_ret = findPDForASID(asid);
        যদি (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (unlikely(find_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 1;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_সূচক = vaddr >> (PAGE_BITS + PT_সূচক_BITS);
    pdSlot = &pd[pd_সূচক];
    যদি (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = seL4_DeleteFirst;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_attributes_get_armParityEnabled(attr),
              0 /* Domain */
          );
#অন্যথা
    pde = pde_pde_coarse_new(paddr);
#endif

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, vaddr);

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত performPageTableInvocationMap(cap, cte, pde, pdSlot);
}

স্থির exception_t decodeARMFrameInvocation(word_t invLabel, word_t দৈর্ঘ্য,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *বাফার)
{
    নির্বাচন (invLabel) {
    ক্ষেত্র ARMPageMap: {
        word_t vaddr, vtop, w_rightsMask;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_page_আকার_t frame_আকার;
        vm_attributes_t attr;

        যদি (unlikely(দৈর্ঘ্য < 3 || current_extra_caps.excaprefs[0] == ফাঁকা)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                seL4_TruncatedMessage;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, বাফার);
        w_rightsMask = getSyscallArg(1, বাফার);
        attr = vmAttributesFromWord(getSyscallArg(2, বাফার));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        frame_আকার = generic_frame_cap_get_capFSize(cap);
        capVMRights = generic_frame_cap_get_capFVMRights(cap);

        যদি (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 1;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(
                         pdCap));
        asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

        যদি (generic_frame_cap_get_capFIsMapped(cap)) {
            যদি (generic_frame_cap_get_capFMappedASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_সংখ্যা = 1;

                ফেরত EXCEPTION_SYSCALL_ERROR;
            }

            যদি (generic_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_সংখ্যা = 0;

                ফেরত EXCEPTION_SYSCALL_ERROR;
            }
        } অন্যথা {
            vtop = vaddr + BIT(page_Bits_For_আকার(frame_আকার)) - 1;

            যদি (unlikely(vtop >= USER_শীর্ষ)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_শীর্ষ);
                current_syscall_error.type =
                    seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_সংখ্যা = 0;

                ফেরত EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            findPDForASID_ret_t find_ret;

            find_ret = findPDForASID(asid);
            যদি (unlikely(find_ret.status != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource =
                    false;

                ফেরত EXCEPTION_SYSCALL_ERROR;
            }

            যদি (unlikely(find_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_সংখ্যা = 1;

                ফেরত EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            maskVMRights(capVMRights, rightsFromWord(w_rightsMask));

        যদি (unlikely(!checkVPAlignment(frame_আকার, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((শূন্য *)
                                   generic_frame_cap_get_capFBasePtr(cap));

        cap = generic_frame_cap_set_capFMappedAddress(cap, asid,
                                                      vaddr);
        যদি (frame_আকার == ARMSmallPage || frame_আকার == ARMLargePage) {
            create_mappings_pte_return_t map_ret;
            map_ret = createSafeMappingEntries_PTE(capFBasePtr, vaddr,
                                                   frame_আকার, vmRights,
                                                   attr, pd);
            যদি (unlikely(map_ret.status != EXCEPTION_NONE)) {
#ifdef CONFIG_PRINTING
                যদি (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#endif
                ফেরত map_ret.status;
            }

            setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
            ফেরত performPageInvocationMapPTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } অন্যথা {
            create_mappings_pde_return_t map_ret;
            map_ret = createSafeMappingEntries_PDE(capFBasePtr, vaddr,
                                                   frame_আকার, vmRights,
                                                   attr, pd);
            যদি (unlikely(map_ret.status != EXCEPTION_NONE)) {
#ifdef CONFIG_PRINTING
                যদি (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#endif
                ফেরত map_ret.status;
            }

            setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
            ফেরত performPageInvocationMapPDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    ক্ষেত্র ARMPageUnmap: {
#ifdef CONFIG_TK1_SMMU
        যদি (isIOSpaceFrameCap(cap)) {
            setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
            ফেরত performPageInvocationUnmapIO(cap, cte);
        } অন্যথা
#endif
        {
            setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
            ফেরত performPageInvocationUnmap(cap, cte);
        }
    }

#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র ARMPageMapIO: {
        ফেরত decodeARMIOMapInvocation(invLabel, দৈর্ঘ্য, cte, cap, বাফার);
    }
#endif

    ক্ষেত্র ARMPage_Clean_তথ্য:
    ক্ষেত্র ARMPage_Invalidate_তথ্য:
    ক্ষেত্র ARMPage_Clean_Invalidate_তথ্য:
    ক্ষেত্র ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        findPDForASID_ret_t pd;
        vptr_t start, end;
        paddr_t pstart;
        word_t page_আকার;
        word_t page_base;

        যদি (দৈর্ঘ্য < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_frame_cap_get_capFMappedASID(cap);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_frame_cap_get_capFBasePtr(cap);
#অন্যথা
        vaddr = generic_frame_cap_get_capFMappedAddress(cap);
#endif

        যদি (unlikely(!generic_frame_cap_get_capFIsMapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        pd = findPDForASID(asid);
        যদি (unlikely(pd.status != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, বাফার);
        end =   getSyscallArg(1, বাফার);

        /* check that the range is sane */
        যদি (end <= start) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 1;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        page_আকার = 1 << page_Bits_For_আকার(generic_frame_cap_get_capFSize(cap));
        page_base = addrFromPPtr((শূন্য *)generic_frame_cap_get_capFBasePtr(cap));

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        যদি (unlikely(!resolve_ret.valid ||
                     resolve_ret.frame_আকার != generic_frame_cap_get_capFSize(cap) ||
                     resolve_ret.frameBase != page_base)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
#endif

        যদি (start >= page_আকার || end > page_আকার) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = page_base + start;
        start += vaddr;
        end += vaddr;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        যদি (pstart < physBase() || pstart > PADDR_শীর্ষ || (end - start) > PADDR_শীর্ষ - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performPageFlush(invLabel, pd.pd, asid, start, end - 1, pstart);
    }

    ক্ষেত্র ARMPageGetAddress: {


        /* Check that there are enough message registers */
        প্রতিজ্ঞা(n_বার্তা_Registers >= 1);

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performPageGetAddress((শূন্য *)generic_frame_cap_get_capFBasePtr(cap), call);
    }

    পূর্বনির্ধারিত:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t দৈর্ঘ্য, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *বাফার)
{
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_page_directory_cap:
        ফেরত decodeARMPageDirectoryInvocation(invLabel, দৈর্ঘ্য, cptr, cte,
                                                cap, বাফার);

    ক্ষেত্র cap_page_table_cap:
        ফেরত decodeARMPageTableInvocation(invLabel, দৈর্ঘ্য, cte, cap, বাফার);

    ক্ষেত্র cap_small_frame_cap:
    ক্ষেত্র cap_frame_cap:
        ফেরত decodeARMFrameInvocation(invLabel, দৈর্ঘ্য, cte, cap, call, বাফার);

    ক্ষেত্র cap_asid_control_cap: {
        word_t i;
        asid_t asid_base;
        word_t সূচক, depth;
        cap_t untyped, মূল;
        cte_t *parentSlot, *destSlot;
        lookupSlot_ret_t lu_ret;
        শূন্য *frame;
        exception_t status;

        যদি (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (unlikely(দৈর্ঘ্য < 2 || current_extra_caps.excaprefs[0] == ফাঁকা
                     || current_extra_caps.excaprefs[1] == ফাঁকা)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        সূচক = getSyscallArg(0, বাফার);
        depth = getSyscallArg(1, বাফার);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        মূল = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        জন্য (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        যদি (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_নিচের_Bits;

        যদি (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_Block_আকার(untyped) !=
                     seL4_ASIDPoolBits || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 1;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        যদি (unlikely(status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            ফেরত status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(মূল, সূচক, depth);
        যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            ফেরত lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        যদি (unlikely(status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            ফেরত status;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performASIDControlInvocation(frame, destSlot,
                                            parentSlot, asid_base);
    }

    ক্ষেত্র cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pdCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        যদি (unlikely(invLabel != ARMASIDPoolAssign)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (unlikely(current_extra_caps.excaprefs[0] == ফাঁকা)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        pdCapSlot = current_extra_caps.excaprefs[0];
        pdCap = pdCapSlot->cap;

        যদি (unlikely(
                cap_get_capType(pdCap) != cap_page_directory_cap ||
                cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 1;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_উপরের(cap_asid_pool_cap_get_capASIDBase(cap))];
        যদি (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_মূল_new();

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        জন্য (i = 0; i < (1 << asid_নিচের_Bits) && (asid + i == 0 || pool->array[i]); i++);

        যদি (unlikely(i == 1 << asid_নিচের_Bits)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = seL4_DeleteFirst;

            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত performASIDPoolInvocation(asid, pool, pdCapSlot);
    }

    পূর্বনির্ধারিত:
        fail("Invalid ARM arch cap type");
    }
}

#ifdef CONFIG_KERNEL_LOG_বাফার
exception_t benchmark_arch_map_log_বাফার(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_আকার_t frame_আকার;
    pptr_t  frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(গ্রন্থি_STATE(ksCurThread), frame_cptr);

    যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    frame_আকার = generic_frame_cap_get_capFSize(lu_ret.cap);

    যদি (frame_আকার != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = generic_frame_cap_get_capFBasePtr(lu_ret.cap);

    ks_User_Log_বাফার = pptr_to_paddr((শূন্য *) frame_pptr);

    জন্য (পূর্ণসংখ্যা idx = 0; idx < BIT(PT_সূচক_BITS); idx++) {
        paddr_t physical_address = ks_User_Log_বাফার + (idx << seL4_PageBits);

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

    ফেরত EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_DEBUG_BUILD
শূন্য kernelPrefetchAbort(word_t pc, word_t lr) VISIBLE;
শূন্য kernel_তথ্য_Abort(word_t pc) VISIBLE;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

শূন্য kernelUndefinedInstruction(word_t pc) VISIBLE;

শূন্য kernelPrefetchAbort(word_t pc, word_t lr)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    printf("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

শূন্য kernel_তথ্য_Abort(word_t pc)
{
    printf("\n\nKERNEL DATA ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HDFAR: 0x%"SEL4_PRIx_word" HSR: 0x%"SEL4_PRIx_word"\n",
           getHDFAR(), getHSR());
    halt();
}

শূন্য kernelUndefinedInstruction(word_t pc)
{
    printf("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    halt();
}

#অন্যথা /* CONFIG_ARM_HYPERVISOR_SUPPORT */

শূন্য kernelPrefetchAbort(word_t pc, word_t lr)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("IFSR: 0x%"SEL4_PRIx_word"\n", getIFSR());
    printf("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

শূন্য kernel_তথ্য_Abort(word_t pc)
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
প্রকারলেখ কাঠামো readWordFromVSpace_ret {
    exception_t status;
    word_t মান;
} readWordFromVSpace_ret_t;

স্থির readWordFromVSpace_ret_t readWordFromVSpace(pde_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    lookupPTSlot_ret_t ptSlot;
    pde_t *pdSlot;
    paddr_t paddr;
    word_t offset;
    pptr_t kernel_vaddr;
    word_t *মান;

    pdSlot = lookupPDSlot(pd, vaddr);
    যদি (pde_ptr_get_pdeType(pdSlot) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_address(pdSlot);
        offset = vaddr & MASK(ARMSectionBits);
    } অন্যথা {
        ptSlot = lookupPTSlot(pd, vaddr);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        যদি (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_address(ptSlot.ptSlot);
            যদি (pte_pte_small_ptr_get_contiguous_hint(ptSlot.ptSlot)) {
                offset = vaddr & MASK(ARMLargePageBits);
            } অন্যথা {
                offset = vaddr & MASK(ARMSmallPageBits);
            }
#অন্যথা
        যদি (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_address(ptSlot.ptSlot);
            offset = vaddr & MASK(ARMSmallPageBits);
        } অন্যথা যদি (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_address(ptSlot.ptSlot);
            offset = vaddr & MASK(ARMLargePageBits);
#endif
        } অন্যথা {
            ret.status = EXCEPTION_LOOKUP_FAULT;
            ফেরত ret;
        }
    }


    kernel_vaddr = (word_t)paddr_to_pptr(paddr);
    মান = (word_t *)(kernel_vaddr + offset);
    ret.status = EXCEPTION_NONE;
    ret.মান = *মান;
    ফেরত ret;
}

শূন্য Arch_user_স্তূপ_Trace(tcb_t *tptr)
{
    cap_t thread_মূল;
    pde_t *pd;
    word_t sp;
    পূর্ণসংখ্যা i;

    thread_মূল = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    যদি (cap_get_capType(thread_মূল) != cap_page_directory_cap) {
        printf("Invalid vspace\n");
        ফেরত;
    }

    pd = (pde_t *)pptr_of_cap(thread_মূল);

    sp = getRegister(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    যদি (!IS_ALIGNED(sp, se_L4_Word_আকার_Bits)) {
        printf("SP not aligned\n");
        ফেরত;
    }

    জন্য (i = 0; i < CONFIG_USER_স্তূপ_TRACE_দৈর্ঘ্য; i++) {
        word_t address = sp + (i * আকার(word_t));
        readWordFromVSpace_ret_t ফলাফল;
        ফলাফল = readWordFromVSpace(pd, address);
        যদি (ফলাফল.status == EXCEPTION_NONE) {
            printf("0x%lx: 0x%lx\n", (long)address, (long)ফলাফল.মান);
        } অন্যথা {
            printf("0x%lx: INVALID\n", (long)address);
        }
    }
}
#endif
