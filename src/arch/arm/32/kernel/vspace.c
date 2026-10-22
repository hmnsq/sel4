/*
 * Copyright 2014, General Dynamics C4 Systems
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

#अगर_घोषित CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#समावेश <benchmark/benchmark_track.h>
#पूर्ण_अगर

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#घोषणा SECTIONS_PER_SUPER_SECTION BIT(ARMSuperSectionBits - ARMSectionBits)
#घोषणा PAGES_PER_LARGE_PAGE BIT(ARMLargePageBits - ARMSmallPageBits)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#घोषणा LAST_BYTE_PTE(PTE,लंबाई_2) ((word_t)&(PTE)[(लंबाई_2)-1] + (BIT(PTE_आकार_BITS)-1))
#घोषणा LAST_BYTE_PDE(PDE,लंबाई_2) ((word_t)&(PDE)[(लंबाई_2)-1] + (BIT(PDE_आकार_BITS)-1))

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#घोषणा MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#घोषणा MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#घोषणा ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#घोषणा ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */

अटल व्योम setGlobalPD(व्योम);

काष्ठा resolve_ret {
    paddr_t frameBase;
    vm_page_आकार_t frame_आकार;
    bool_t valid;
};
प्रकार काष्ठा resolve_ret resolve_ret_t;

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
अटल bool_t PURE pteCheckIfMapped(pte_t *pte);
अटल bool_t PURE pdeCheckIfMapped(pde_t *pde);

अटल word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    चयन (vm_rights) {
    हाल VMNoAccess:
        वापस 0;

    हाल VMKernelOnly:
        वापस 1;

    हाल VMReadOnly:
        वापस 2;

    हाल VMReadWrite:
        वापस 3;

    शेष:
        fail("Invalid VM rights");
    }
}

#अन्यथा
/* AP encoding slightly different. AP only used for kernel mappings which are fixed after boot time */
BOOT_CODE
अटल word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    चयन (vm_rights) {
    हाल VMKernelOnly:
        वापस 0;
    हाल VMReadWrite:
        वापस 1;
    हाल VMNoAccess:
        /* RO at PL1 only */
        वापस 2;
    हाल VMReadOnly:
        वापस 3;
    शेष:
        fail("Invalid VM rights");
    }
}

अटल word_t CONST HAPFromVMRights(vm_rights_t vm_rights)
{
    चयन (vm_rights) {
    हाल VMKernelOnly:
    हाल VMNoAccess:
        वापस 0;
    हाल VMReadOnly:
        वापस 1;
    /*
    case VMWriteOnly:
        return 2;
    */
    हाल VMReadWrite:
        वापस 3;
    शेष:
        fail("Invalid VM rights");
    }
}

#पूर्ण_अगर

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    अगर (vm_rights == VMNoAccess) {
        वापस VMNoAccess;
    }
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

BOOT_CODE व्योम map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_attributes_t attributes)
{
    word_t idx = (vaddr & MASK(page_Bits_For_आकार(ARMSection))) >> page_Bits_For_आकार(ARMSmallPage);

    निश्चित(vaddr >= PPTR_शीर्ष); /* vaddr lies in the region the global PT covers */
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t tex;
    अगर (vm_attributes_get_armPageCacheable(attributes)) {
        tex = 5; /* TEX = 0b1(Cached)01(Outer Write Allocate) */
    } अन्यथा {
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
#अन्यथा /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
#पूर्ण_अगर /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_CODE व्योम map_kernel_window(व्योम)
{
    paddr_t  phys;
    word_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = physBase();
    idx = PPTR_BASE >> page_Bits_For_आकार(ARMSection);

    जबतक (idx < BIT(PD_सूचकांक_BITS) - SECTIONS_PER_SUPER_SECTION) {
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
        क्रम (idx2 = idx; idx2 < idx + SECTIONS_PER_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += BIT(page_Bits_For_आकार(ARMSuperSection));
        idx += SECTIONS_PER_SUPER_SECTION;
    }

    जबतक (idx < (PPTR_शीर्ष >> page_Bits_For_आकार(ARMSection))) {
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
        phys += BIT(page_Bits_For_आकार(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    निश्चित(phys == PADDR_शीर्ष);

#अगर_घोषित CONFIG_KERNEL_LOG_बफर
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /* address */
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, BIT(seL4_PageTableBits));

    phys += BIT(page_Bits_For_आकार(ARMSection));
    idx++;
#पूर्ण_अगर /* CONFIG_KERNEL_LOG_BUFFER */

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

#अन्यथा /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_CODE व्योम map_kernel_window(व्योम)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t UNUSED pteS2;

    /* Initialise PGD */
    क्रम (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    क्रम (idx = 0; idx < (USER_शीर्ष - 0xC0000000) >> (PT_सूचकांक_BITS + PAGE_BITS); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = physBase();
    क्रम (; idx < BIT(PT_सूचकांक_BITS) - 1; idx++) {
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
        phys += BIT(PT_सूचकांक_BITS + PAGE_BITS);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << seL4_PageTableBits);
    क्रम (idx = 0; idx < 256; idx++) {
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

#पूर्ण_अगर /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

अटल BOOT_CODE व्योम map_it_frame_cap(cap_t pd_cap, cap_t frame_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *targetSlot;
    pde_t *pd    = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    व्योम  *frame = (व्योम *)generic_frame_cap_get_capFBasePtr(frame_cap);
    vptr_t vptr  = generic_frame_cap_get_capFMappedAddress(frame_cap);

    निश्चित(generic_frame_cap_get_capFMappedASID(frame_cap) != 0);

    pd += (vptr >> page_Bits_For_आकार(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pd));
    targetSlot = pt + ((vptr & MASK(page_Bits_For_आकार(ARMSection)))
                       >> page_Bits_For_आकार(ARMSmallPage));
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
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
#अन्यथा
    *targetSlot = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(frame),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#पूर्ण_अगर
}

/* Create a frame cap for the initial thread. */

अटल BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    अगर (use_large)
        वापस
            cap_frame_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_निचला(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_ऊपरी(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    अन्यथा
        वापस
            cap_small_frame_cap_new(
                ASID_निचला(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#अगर_घोषित CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#पूर्ण_अगर
                ASID_ऊपरी(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

अटल BOOT_CODE व्योम map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);
    pde_t *targetSlot = pd + (vptr >> page_Bits_For_आकार(ARMSection));

    निश्चित(cap_page_table_cap_get_capPTIsMapped(pt_cap));

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    *targetSlot = pde_pde_coarse_new(
                      addrFromPPtr(pt), /* address */
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#अन्यथा
    *targetSlot = pde_pde_coarse_new(addrFromPPtr(pt));
#पूर्ण_अगर
}

/* Create a page table for the initial thread */

अटल BOOT_CODE cap_t create_it_page_table_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    अगर (asid != asidInvalid) {
        map_it_pt_cap(pd, cap);
    }
    वापस cap;
}

BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    वापस get_n_paging(it_v_reg, PT_सूचकांक_BITS + PAGE_BITS);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t create_it_address_space(cap_t मूल_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << seL4_PageDirBits) - 1,
                        addrFromPPtr((व्योम *)rootserver.vspace));
    cap_t pd_cap =
        cap_page_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    क्रम (pt_vptr = ROUND_DOWN(it_v_reg.start, PT_सूचकांक_BITS + PAGE_BITS);
         pt_vptr < it_v_reg.end;
         pt_vptr += BIT(PT_सूचकांक_BITS + PAGE_BITS)) {
        अगर (!provide_cap(मूल_cnode_cap,
                         create_it_page_table_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            वापस cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    वापस pd_cap;
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

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_CODE व्योम activate_kernel_vspace(व्योम)
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

#अन्यथा

BOOT_CODE व्योम activate_kernel_vspace(व्योम)
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
#अगर 0 /* Can't lock entries on A15 */
    lockTLBEntry(PPTR_BASE);
    lockTLBEntry(PPTR_VECTOR_TABLE);
#पूर्ण_अगर
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

#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_CODE व्योम write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[ASID_निचला(IT_ASID)] = PDE_PTR(pptr_of_cap(it_pd_cap));
    armKSASIDTable[ASID_ऊपरी(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

findPDForASID_ret_t findPDForASID(asid_t asid)
{
    findPDForASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_ऊपरी(asid)];
    अगर (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_मूल_new();

        ret.pd = शून्य;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        वापस ret;
    }

    pd = poolPtr->array[ASID_निचला(asid)];
    अगर (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_मूल_new();

        ret.pd = शून्य;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        वापस ret;
    }

    ret.pd = pd;
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

    अगर (unlikely(cap_get_capType(बफर_Cap) != cap_small_frame_cap &&
                 cap_get_capType(बफर_Cap) != cap_frame_cap)) {
        वापस शून्य;
    }
    अगर (unlikely(generic_frame_cap_get_capFIsDevice(बफर_Cap))) {
        वापस शून्य;
    }

    vm_rights = generic_frame_cap_get_capFVMRights(बफर_Cap);
    अगर (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr;
        अचिन्हित पूर्णांक pageBits;

        basePtr = generic_frame_cap_get_capFBasePtr(बफर_Cap);
        pageBits = page_Bits_For_आकार(generic_frame_cap_get_capFSize(बफर_Cap));
        वापस (word_t *)(basePtr + (w_बफर_Ptr & MASK(pageBits)));
    } अन्यथा {
        वापस शून्य;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    अगर (unlikely(cap_get_capType(cap) != cap_small_frame_cap &&
                 cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (unlikely(generic_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(vptr & MASK(se_L4_IPCBuffer_आकार_Bits))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (पूर्णांक)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    वापस EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    अचिन्हित पूर्णांक pd_सूचकांक;

    pd_सूचकांक = vptr >> (PAGE_BITS + PT_सूचकांक_BITS);
    वापस pd + pd_सूचकांक;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pdSlot;

    pdSlot = lookupPDSlot(pd, vptr);

    अगर (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_सूचकांक_BITS + PAGE_BITS);

        ret.ptSlot = शून्य;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        वापस ret;
    } अन्यथा {
        pte_t *pt, *ptSlot;
        अचिन्हित पूर्णांक pt_सूचकांक;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pdSlot));
        pt_सूचकांक = (vptr >> PAGE_BITS) & MASK(PT_सूचकांक_BITS);
        ptSlot = pt + pt_सूचकांक;

        ret.ptSlot = ptSlot;
        ret.status = EXCEPTION_NONE;
        वापस ret;
    }
}

अटल pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    अचिन्हित पूर्णांक pt_सूचकांक;

    pt_सूचकांक = (vptr >> PAGE_BITS) & MASK(PT_सूचकांक_BITS);
    वापस pt + pt_सूचकांक;
}

अटल स्थिर resolve_ret_t default_resolve_ret_t;

अटल resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    चयन (pde_ptr_get_pdeType(pde)) {
    हाल pde_pde_section:
        ret.frameBase = pde_pde_section_ptr_get_address(pde);
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        अगर (pde_pde_section_ptr_get_आकार(pde)) {
            ret.frame_आकार = ARMSuperSection;
        } अन्यथा {
            ret.frame_आकार = ARMSection;
        }
#अन्यथा
        अगर (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.frameBase &= ~MASK(page_Bits_For_आकार(ARMSuperSection));
            ret.frame_आकार = ARMSuperSection;
        } अन्यथा {
            ret.frame_आकार = ARMSection;
        }
#पूर्ण_अगर
        वापस ret;

    हाल pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        चयन (pte_ptr_get_pteType(pte)) {
        हाल pte_pte_small:
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
            अगर (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(page_Bits_For_आकार(ARMLargePage));
                ret.frame_आकार = ARMLargePage;
            } अन्यथा {
                ret.frame_आकार = ARMSmallPage;
            }
#अन्यथा
            ret.frame_आकार = ARMSmallPage;
#पूर्ण_अगर
            वापस ret;

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        हाल pte_pte_large:
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frame_आकार = ARMLargePage;
            वापस ret;
#पूर्ण_अगर
        शेष:
            अवरोध;
        }
        अवरोध;
    }
    }

    ret.valid = false;
    वापस ret;
}

अटल pte_t CONST makeUserPTE(vm_page_आकार_t page_आकार, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap;

    ap = APFromVMRights(vm_rights);

    चयन (page_आकार) {
    हाल ARMSmallPage: {
        अगर (cacheable) {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    nonexecutable);
        } अन्यथा {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    0, /* TEX = 0b000, strongly-ordered. */
                                    ap,
                                    0, 0,
                                    nonexecutable);
        }
        अवरोध;
    }

    हाल ARMLargePage: {
        अगर (cacheable) {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    1 /* reserved */);
        } अन्यथा {
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
        अवरोध;
    }

    शेष:
        fail("Invalid PTE frame type");
    }

#अन्यथा

    word_t hap;

    hap = HAPFromVMRights(vm_rights);

    चयन (page_आकार) {
    हाल ARMSmallPage: {
        अगर (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_CACHEABLE /* Cacheable */);
        } अन्यथा {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        अवरोध;
    }

    हाल ARMLargePage: {
        अगर (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_CACHEABLE  /* Cacheable */);
        } अन्यथा {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        अवरोध;
    }
    शेष:
        fail("Invalid PTE frame type");
    }
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    वापस pte;
}

अटल pde_t CONST makeUserPDE(vm_page_आकार_t page_आकार, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, word_t domain,
                               vm_rights_t vm_rights)
{
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap, size2;

    ap = APFromVMRights(vm_rights);
#अन्यथा
    word_t hap, size2;

    (व्योम)domain;
    hap = HAPFromVMRights(vm_rights);
#पूर्ण_अगर

    चयन (page_आकार) {
    हाल ARMSection:
        size2 = 0;
        अवरोध;

    हाल ARMSuperSection:
        size2 = 1;
        अवरोध;

    शेष:
        fail("Invalid PDE frame type");
    }

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    अगर (cacheable) {
        वापस pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                   0, /* APX = 0, privileged full access */
                                   5, /* TEX = 0b101, outer write-back, write-allocate */
                                   ap, parity, domain, nonexecutable,
                                   0, 1 /* Inner write-back, write-allocate */);
    } अन्यथा {
        वापस pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   1, /* shared */
                                   0, /* APX = 0, privileged full access */
                                   0, /* TEX = 0b000, strongly-ordered */
                                   ap, parity, domain, nonexecutable,
                                   0, 0);
    }
#अन्यथा
    अगर (cacheable) {
        वापस pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_CACHEABLE /* Cacheable */);
    } अन्यथा {
        वापस pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_NONCACHEABLE /* Not cacheable */);
    }
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_मूल(cap_t cap)
{
    वापस cap_get_capType(cap) == cap_page_directory_cap &&
           cap_page_directory_cap_get_capPDIsMapped(cap);
}

bool_t CONST isIOSpaceFrameCap(cap_t cap)
{
#अगर_घोषित CONFIG_TK1_SMMU
    वापस cap_get_capType(cap) == cap_small_frame_cap && cap_small_frame_cap_get_capFIsIOSpace(cap);
#अन्यथा
    वापस false;
#पूर्ण_अगर
}

/** Switch to the empty PD on the reserved HW ASID */
अटल व्योम setGlobalPD(व्योम)
{
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    writeContextIDAndPD(hwASIDReserved, addrFromKPPtr(armUSGlobalPD));
#अन्यथा
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
#पूर्ण_अगर
}

व्योम setVMRoot(tcb_t *tcb)
{
    cap_t thread_मूल;
    asid_t asid;
    pde_t *pd;
    findPDForASID_ret_t find_ret;

    thread_मूल = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    अगर (cap_get_capType(thread_मूल) != cap_page_directory_cap ||
        !cap_page_directory_cap_get_capPDIsMapped(thread_मूल)) {
        setGlobalPD();
        वापस;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_मूल));
    asid = cap_page_directory_cap_get_capPDMappedASID(thread_मूल);
    find_ret = findPDForASID(asid);
    अगर (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.pd != pd)) {
        setGlobalPD();
        वापस;
    }

    armv_contextSwitch(pd, asid);
}

अटल bool_t setVMRootForFlush(pde_t *pd, asid_t asid)
{
    cap_t thread_मूल;

    thread_मूल = TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbVTable)->cap;

    अगर (cap_get_capType(thread_मूल) == cap_page_directory_cap &&
        cap_page_directory_cap_get_capPDIsMapped(thread_मूल) &&
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_मूल)) == pd) {
        वापस false;
    }

    armv_contextSwitch(pd, asid);

    वापस true;
}

pde_t *pageTableMapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    findPDForASID_ret_t find_ret;
    pde_t pde;
    अचिन्हित पूर्णांक pd_सूचकांक;

    find_ret = findPDForASID(asid);
    अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
        वापस शून्य;
    }

    pd_सूचकांक = vaddr >> (PAGE_BITS + PT_सूचकांक_BITS);
    pde = find_ret.pd[pd_सूचकांक];

    अगर (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_address(pde)) == pt)) {
        वापस find_ret.pd;
    } अन्यथा {
        वापस शून्य;
    }
}

अटल व्योम invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ऊपरी(asid)];
    निश्चित(asidPool);

    pd = asidPool->array[ASID_निचला(asid)];
    निश्चित(pd);

    pd[PD_ASID_SLOT] = pde_pde_invalid_new(0, false);
}

अटल pde_t PURE loadHWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ऊपरी(asid)];
    निश्चित(asidPool);

    pd = asidPool->array[ASID_निचला(asid)];
    निश्चित(pd);

    वापस pd[PD_ASID_SLOT];
}

अटल व्योम storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ऊपरी(asid)];
    निश्चित(asidPool);

    pd = asidPool->array[ASID_निचला(asid)];
    निश्चित(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_SLOT] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t findFreeHWASID(व्योम)
{
    word_t hw_asid_offset;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    क्रम (hw_asid_offset = 0;
         hw_asid_offset <= (word_t)((hw_asid_t) - 1);
         hw_asid_offset ++) {
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

    /* Increment the NextASID index, skipping the reserved ASID 0 on wrap */
    armKSNextASID++;
    अगर (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    वापस hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);
    अगर (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        वापस pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } अन्यथा {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        वापस new_hw_asid;
    }
}

अटल व्योम invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);
    अगर (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

व्योम unmapPageTable(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pdSlot;
    अचिन्हित पूर्णांक pd_सूचकांक;

    pd = pageTableMapped(asid, vaddr, pt);

    अगर (likely(pd != शून्य)) {
        pd_सूचकांक = vaddr >> (PT_सूचकांक_BITS + PAGE_BITS);
        pdSlot = pd + pd_सूचकांक;

        *pdSlot = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));
        flushTable(pd, asid, vaddr, pt);
    }
}

व्योम copyGlobalMappings(pde_t *newPD)
{
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t i;
    pde_t *global_pd = armKSGlobalPD;

    क्रम (i = PPTR_BASE >> ARMSectionBits; i < BIT(PD_सूचकांक_BITS); i++) {
        अगर (i != PD_ASID_SLOT) {
            newPD[i] = global_pd[i];
        }
    }
#पूर्ण_अगर
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    चयन (vm_faultType) {
    हाल ARMDataAbort: {
        word_t addr, fault;

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (addressTranslateS1(addr) & ~MASK(PAGE_BITS)) | (addr & MASK(PAGE_BITS));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#अन्यथा
        addr = getFAR();
        fault = getDFSR();
#पूर्ण_अगर

#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
        /* Debug exceptions come in on the Prefetch and Data abort vectors.
         * We have to test the fault-status bits in the IFSR/DFSR to determine
         * if it's a debug exception when one occurs.
         *
         * If it is a debug exception, return early and don't fallthrough to the
         * normal VM Fault handling path.
         */
        अगर (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(0);
            वापस EXCEPTION_FAULT;
        }
#पूर्ण_अगर
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        वापस EXCEPTION_FAULT;
    }

    हाल ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (addressTranslateS1(pc) & ~MASK(PAGE_BITS)) | (pc & MASK(PAGE_BITS));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#अन्यथा
        fault = getIFSR();
#पूर्ण_अगर

#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
        अगर (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            अगर (seL4_Fault_DebugException_get_exceptionReason(current_fault) == seL4_SingleStep
                && !single_Step_Fault_गणक_Ready(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                वापस EXCEPTION_NONE;
            }
            वापस EXCEPTION_FAULT;
        }
#पूर्ण_अगर
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        वापस EXCEPTION_FAULT;
    }

    शेष:
        fail("Invalid VM fault type");
    }
}

व्योम deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    अचिन्हित पूर्णांक offset;

    /* Haskell error: "ASID pool's base must be aligned" */
    निश्चित((asid_base & MASK(asid_निचला_Bits)) == 0);

    अगर (armKSASIDTable[ASID_ऊपरी(asid_base)] == pool) {
        क्रम (offset = 0; offset < BIT(asid_निचला_Bits); offset++) {
            अगर (pool->array[offset]) {
                flushSpace(asid_base + offset);
                invalidateASIDEntry(asid_base + offset);
            }
        }
        armKSASIDTable[ASID_ऊपरी(asid_base)] = शून्य;
        setVMRoot(गाँठ_STATE(ksCurThread));
    }
}

व्योम deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ऊपरी(asid)];

    अगर (poolPtr != शून्य && poolPtr->array[ASID_निचला(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_निचला(asid)] = शून्य;
        setVMRoot(गाँठ_STATE(ksCurThread));
    }
}

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
अटल pte_t pte_pte_invalid_new(व्योम)
{
    /* Invalid as every PTE must have bit 0 set (large PTE) or bit 1 set (small
     * PTE). 0 == 'translation fault' in ARM parlance.
     */
    वापस (pte_t) {
        {
            0
        }
    };
}
#पूर्ण_अगर

व्योम unmapPage(vm_page_आकार_t page_आकार, asid_t asid, vptr_t vptr, व्योम *pptr)
{
    findPDForASID_ret_t find_ret;
    paddr_t addr = addrFromPPtr(pptr);

    find_ret = findPDForASID(asid);
    अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
        वापस;
    }

    चयन (page_आकार) {
    हाल ARMSmallPage: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            वापस;
        }

        अगर (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            वापस;
        }
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        अगर (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 0)) {
            वापस;
        }
#पूर्ण_अगर
        अगर (unlikely(pte_pte_small_ptr_get_address(lu_ret.ptSlot) != addr)) {
            वापस;
        }

        *(lu_ret.ptSlot) = pte_pte_invalid_new();
        cleanByVA_PoU((word_t)lu_ret.ptSlot, addrFromPPtr(lu_ret.ptSlot));

        अवरोध;
    }

    हाल ARMLargePage: {
        lookupPTSlot_ret_t lu_ret;
        word_t i;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            वापस;
        }
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        अगर (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_large)) {
            वापस;
        }
        अगर (unlikely(pte_pte_large_ptr_get_address(lu_ret.ptSlot) != addr)) {
            वापस;
        }
#अन्यथा
        अगर (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            वापस;
        }
        अगर (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 1)) {
            वापस;
        }
        अगर (unlikely(pte_pte_small_ptr_get_address(lu_ret.ptSlot) != addr)) {
            वापस;
        }
#पूर्ण_अगर

        क्रम (i = 0; i < PAGES_PER_LARGE_PAGE; i++) {
            lu_ret.ptSlot[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((word_t)&lu_ret.ptSlot[0],
                            LAST_BYTE_PTE(lu_ret.ptSlot, PAGES_PER_LARGE_PAGE),
                            addrFromPPtr(&lu_ret.ptSlot[0]));

        अवरोध;
    }

    हाल ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(find_ret.pd, vptr);

        अगर (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            वापस;
        }
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        अगर (unlikely(pde_pde_section_ptr_get_आकार(pd) != 0)) {
#अन्यथा
        अगर (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#पूर्ण_अगर
            वापस;
        }
        अगर (unlikely(pde_pde_section_ptr_get_address(pd) != addr)) {
            वापस;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pd, addrFromPPtr(pd));

        अवरोध;
    }

    हाल ARMSuperSection: {
        pde_t *pd;
        word_t i;

        pd = lookupPDSlot(find_ret.pd, vptr);

        अगर (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            वापस;
        }
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        अगर (unlikely(pde_pde_section_ptr_get_आकार(pd) != 1)) {
#अन्यथा
        अगर (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#पूर्ण_अगर
            वापस;
        }
        अगर (unlikely(pde_pde_section_ptr_get_address(pd) != addr)) {
            वापस;
        }

        क्रम (i = 0; i < SECTIONS_PER_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((word_t)&pd[0], LAST_BYTE_PDE(pd, SECTIONS_PER_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        अवरोध;
    }

    शेष:
        fail("Invalid ARM page type");
        अवरोध;
    }

    /* Flush the page now that the mapping has been updated */
    flushPage(page_आकार, find_ret.pd, asid, vptr);
}

व्योम flushPage(vm_page_आकार_t page_आकार, pde_t *pd, asid_t asid, word_t vptr)
{
    pde_t stored_hw_asid;
    word_t base_addr;
    bool_t मूल_switched;

    निश्चित((vptr & MASK(page_Bits_For_आकार(page_आकार))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    मूल_switched = setVMRootForFlush(pd, asid);
    stored_hw_asid = loadHWASID(asid);

    अगर (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        base_addr = vptr & ~MASK(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(base_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        अगर (मूल_switched) {
            setVMRoot(गाँठ_STATE(ksCurThread));
        }
    }
}

व्योम flushTable(pde_t *pd, asid_t asid, word_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t मूल_switched;

    निश्चित((vptr & MASK(PT_सूचकांक_BITS + ARMSmallPageBits)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    मूल_switched = setVMRootForFlush(pd, asid);
    stored_hw_asid = loadHWASID(asid);

    अगर (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        अगर (मूल_switched) {
            setVMRoot(गाँठ_STATE(ksCurThread));
        }
    }
}

व्योम flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);

    /* Clean the entire data cache, to guarantee that any VAs mapped
     * in the deleted space are clean (because we can't clean by VA after
     * deleting the space) */
    cleanCaches_PoU();

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    अगर (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        वापस;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

व्योम invalidateTLBByASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    अगर (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        वापस;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

अटल अंतरभूत bool_t CONST checkVPAlignment(vm_page_आकार_t sz, word_t w)
{
    वापस (w & MASK(page_Bits_For_आकार(sz))) == 0;
}

काष्ठा create_mappings_pte_return {
    exception_t status;
    pte_t pte;
    pte_range_t pte_entries;
};
प्रकार काष्ठा create_mappings_pte_return create_mappings_pte_return_t;

काष्ठा create_mappings_pde_return {
    exception_t status;
    pde_t pde;
    pde_range_t pde_entries;
};
प्रकार काष्ठा create_mappings_pde_return create_mappings_pde_return_t;

अटल create_mappings_pte_return_t
createSafeMappingEntries_PTE
(paddr_t base, word_t vaddr, vm_page_आकार_t frame_आकार,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    word_t i;

    चयन (frame_आकार) {

    हाल ARMSmallPage:

        ret.pte_entries.base = शून्य; /* to avoid uninitialised warning */
        ret.pte_entries.लंबाई = 1;

        ret.pte = makeUserPTE(ARMSmallPage, base,
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            वापस ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        अगर (unlikely(pte_ptr_get_pteType(ret.pte_entries.base) ==
                     pte_pte_large)) {
#अन्यथा
        अगर (unlikely(pte_ptr_get_pteType(ret.pte_entries.base) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.base))) {
#पूर्ण_अगर
            current_syscall_error.type =
                seL4_DeleteFirst;

            ret.status = EXCEPTION_SYSCALL_ERROR;
            वापस ret;
        }

        ret.status = EXCEPTION_NONE;
        वापस ret;

    हाल ARMLargePage:

        ret.pte_entries.base = शून्य; /* to avoid uninitialised warning */
        ret.pte_entries.लंबाई = PAGES_PER_LARGE_PAGE;

        ret.pte = makeUserPTE(ARMLargePage, base,
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            वापस ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;

        क्रम (i = 0; i < PAGES_PER_LARGE_PAGE; i++) {
            अगर (unlikely(pte_get_pteType(ret.pte_entries.base[i]) ==
                         pte_pte_small)
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.base[i])
#पूर्ण_अगर
               ) {
                current_syscall_error.type =
                    seL4_DeleteFirst;

                ret.status = EXCEPTION_SYSCALL_ERROR;
                वापस ret;
            }
        }

        ret.status = EXCEPTION_NONE;
        वापस ret;

    शेष:
        fail("Invalid or unexpected ARM page type.");

    }
}

अटल create_mappings_pde_return_t
createSafeMappingEntries_PDE
(paddr_t base, word_t vaddr, vm_page_आकार_t frame_आकार,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_tag_t currentPDEType;
    word_t i;

    चयन (frame_आकार) {

    /* PDE mappings */
    हाल ARMSection:
        ret.pde_entries.base = lookupPDSlot(pd, vaddr);
        ret.pde_entries.लंबाई = 1;

        ret.pde = makeUserPDE(ARMSection, base,
                              vm_attributes_get_armParityEnabled(attr),
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.base);
        अगर (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_आकार(ret.pde_entries.base) != 0))) {
#अन्यथा
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.base) != 0))) {
#पूर्ण_अगर
            current_syscall_error.type =
                seL4_DeleteFirst;
            ret.status = EXCEPTION_SYSCALL_ERROR;

            वापस ret;
        }

        ret.status = EXCEPTION_NONE;
        वापस ret;

    हाल ARMSuperSection:
        ret.pde_entries.base = lookupPDSlot(pd, vaddr);
        ret.pde_entries.लंबाई = SECTIONS_PER_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, base,
                              vm_attributes_get_armParityEnabled(attr),
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              0,
                              vmRights);

        क्रम (i = 0; i < SECTIONS_PER_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.base[i]);
            अगर (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_आकार(ret.pde_entries.base[i]) != 1))) {
#अन्यथा
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.base[i]) != 1))) {
#पूर्ण_अगर
                current_syscall_error.type =
                    seL4_DeleteFirst;
                ret.status = EXCEPTION_SYSCALL_ERROR;

                वापस ret;
            }
        }

        ret.status = EXCEPTION_NONE;
        वापस ret;

    शेष:
        fail("Invalid or unexpected ARM page type.");

    }
}

अटल अंतरभूत vptr_t pageBase(vptr_t vaddr, vm_page_आकार_t आकार)
{
    वापस vaddr & ~MASK(page_Bits_For_आकार(आकार));
}

अटल bool_t PURE pteCheckIfMapped(pte_t *pte)
{
    वापस pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

अटल bool_t PURE pdeCheckIfMapped(pde_t *pde)
{
    वापस pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

अटल व्योम doFlush(पूर्णांक invLabel, vptr_t start, vptr_t end, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        end = (vptr_t)paddr_to_pptr(pstart) + (end - start);
        start = (vptr_t)paddr_to_pptr(pstart);
    }
    चयन (invLabel) {
    हाल ARMPDClean_तथ्य:
    हाल ARMPage_Clean_तथ्य:
        cleanCacheRange_RAM(start, end, pstart);
        अवरोध;
    हाल ARMPDInvalidate_तथ्य:
    हाल ARMPage_Invalidate_तथ्य:
        invalidateCacheRange_RAM(start, end, pstart);
        अवरोध;
    हाल ARMPDClean_Invalidate_तथ्य:
    हाल ARMPage_Clean_Invalidate_तथ्य:
        cleanInvalidateCacheRange_RAM(start, end, pstart);
        अवरोध;
    हाल ARMPDUnify_Instruction:
    हाल ARMPageUnify_Instruction:
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
        अवरोध;
    शेष:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

अटल exception_t performPDFlush(पूर्णांक invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                  vptr_t end, paddr_t pstart)
{
    bool_t मूल_switched;

    /* Flush if given a non zero range */
    अगर (start < end) {
        मूल_switched = setVMRootForFlush(pd, asid);

        doFlush(invLabel, start, end, pstart);

        अगर (मूल_switched) {
            setVMRoot(गाँठ_STATE(ksCurThread));
        }
    }

    वापस EXCEPTION_NONE;
}

अटल exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                                 pde_t pde, pde_t *pdSlot)
{
    ctSlot->cap = cap;
    *pdSlot = pde;
    cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));

    वापस EXCEPTION_NONE;
}

अटल exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    अगर (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt);
        clearMemory_PT((व्योम *)pt, cap_get_cap_आकार_Bits(cap));
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    वापस EXCEPTION_NONE;
}

अटल exception_t performPageInvocationMapPTE(asid_t asid, cap_t cap, cte_t *ctSlot, pte_t pte,
                                               pte_range_t pte_entries)
{
    word_t i, j UNUSED;
    bool_t tlbflush_required;

    ctSlot->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.base);

    j = pte_entries.लंबाई;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t base_address = pte_pte_small_get_address(pte);
#पूर्ण_अगर
    क्रम (i = 0; i < pte_entries.लंबाई; i++) {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_set_address(pte, base_address + i * BIT(page_Bits_For_आकार(ARMSmallPage)));
#पूर्ण_अगर
        pte_entries.base[i] = pte;
    }
    cleanCacheRange_PoU((word_t)pte_entries.base,
                        LAST_BYTE_PTE(pte_entries.base, pte_entries.लंबाई),
                        addrFromPPtr(pte_entries.base));
    अगर (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    वापस EXCEPTION_NONE;
}

अटल exception_t performPageInvocationMapPDE(asid_t asid, cap_t cap, cte_t *ctSlot, pde_t pde,
                                               pde_range_t pde_entries)
{
    word_t i, j UNUSED;
    bool_t tlbflush_required;

    ctSlot->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.base);

    j = pde_entries.लंबाई;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t base_address = pde_pde_section_get_address(pde);
#पूर्ण_अगर
    क्रम (i = 0; i < pde_entries.लंबाई; i++) {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_set_address(pde, base_address + i * BIT(page_Bits_For_आकार(ARMSection)));
#पूर्ण_अगर
        pde_entries.base[i] = pde;
    }
    cleanCacheRange_PoU((word_t)pde_entries.base,
                        LAST_BYTE_PDE(pde_entries.base, pde_entries.लंबाई),
                        addrFromPPtr(pde_entries.base));
    अगर (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    वापस EXCEPTION_NONE;
}

अटल exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    अगर (generic_frame_cap_get_capFIsMapped(cap)) {
        unmapPage(generic_frame_cap_get_capFSize(cap),
                  generic_frame_cap_get_capFMappedASID(cap),
                  generic_frame_cap_get_capFMappedAddress(cap),
                  (व्योम *)generic_frame_cap_get_capFBasePtr(cap));
    }

    generic_frame_cap_ptr_set_capFMappedAddress(&ctSlot->cap, asidInvalid, 0);

    वापस EXCEPTION_NONE;
}

अटल exception_t performPageFlush(पूर्णांक invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                    vptr_t end, paddr_t pstart)
{
    bool_t मूल_switched;

    /* now we can flush. But only if we were given a non zero range */
    अगर (start < end) {
        मूल_switched = setVMRootForFlush(pd, asid);

        doFlush(invLabel, start, end, pstart);

        अगर (मूल_switched) {
            setVMRoot(गाँठ_STATE(ksCurThread));
        }
    }

    वापस EXCEPTION_NONE;
}

अटल exception_t performPageGetAddress(व्योम *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = गाँठ_STATE(ksCurThread);
    अगर (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        अचिन्हित पूर्णांक लंबाई = setMR(thread, ipc_बफर, 0, capFBasePtr);
        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, लंबाई)));
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
    वापस EXCEPTION_NONE;
}

अटल exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pdCapSlot)
{
    cap_page_directory_cap_ptr_set_capPDMappedASID(&pdCapSlot->cap, asid);
    cap_page_directory_cap_ptr_set_capPDIsMapped(&pdCapSlot->cap, 1);
    poolPtr->array[ASID_निचला(asid)] =
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCapSlot->cap));

    वापस EXCEPTION_NONE;
}

अटल exception_t performASIDControlInvocation(व्योम *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_base)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_cap_Free_सूचकांक(&(parent->cap),
                                         MAX_FREE_सूचकांक(cap_untyped_cap_get_cap_Block_आकार(parent->cap)));

    memzero(frame, 1 << ARMSmallPageBits);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_जोड़ो(cap_asid_pool_cap_new(asid_base, WORD_REF(frame)),
              parent, slot);;
    /* Haskell error: "ASID pool's base must be aligned" */
    निश्चित((asid_base & MASK(asid_निचला_Bits)) == 0);
    armKSASIDTable[ASID_ऊपरी(asid_base)] = (asid_pool_t *)frame;

    वापस EXCEPTION_NONE;
}

अटल exception_t decodeARMPageDirectoryInvocation(word_t invLabel, word_t लंबाई,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    word_t *बफर)
{
    चयन (invLabel) {
    हाल ARMPDClean_तथ्य:
    हाल ARMPDInvalidate_तथ्य:
    हाल ARMPDClean_Invalidate_तथ्य:
    हाल ARMPDUnify_Instruction: {
        vptr_t start, end;
        paddr_t pstart;
        findPDForASID_ret_t find_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        अगर (लंबाई < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, बफर);
        end =   getSyscallArg(1, बफर);

        /* Check sanity of arguments */
        अगर (end <= start) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 1;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        अगर (start >= USER_शीर्ष || end > USER_शीर्ष) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(cap_get_capType(cap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap));
        asid = cap_page_directory_cap_get_capPDMappedASID(cap);

        find_ret = findPDForASID(asid);
        अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(find_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, start);

        /* Check that there's actually something there. */
        अगर (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
            वापस EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        अगर (pageBase(start, resolve_ret.frame_आकार) !=
            pageBase(end - 1, resolve_ret.frame_आकार)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax =
                pageBase(start, resolve_ret.frame_आकार) +
                MASK(page_Bits_For_आकार(resolve_ret.frame_आकार));
            वापस EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.frameBase
                 + (start & MASK(page_Bits_For_आकार(resolve_ret.frame_आकार)));

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        अगर (pstart < physBase() || pstart > PADDR_शीर्ष || (end - start) > PADDR_शीर्ष - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#पूर्ण_अगर

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPDFlush(invLabel, pd, asid, start, end - 1, pstart);
    }

    शेष:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

}

अटल exception_t decodeARMPageTableInvocation(word_t invLabel, word_t लंबाई,
                                                cte_t *cte, cap_t cap, word_t *बफर)
{
    word_t vaddr, pd_सूचकांक;

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_attributes_t attr;
#पूर्ण_अगर
    cap_t pdCap;
    pde_t *pd, *pdSlot;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    अगर (invLabel == ARMPageTableUnmap) {
        अगर (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageTableInvocationUnmap(cap, cte);
    }

    अगर (unlikely(invLabel != ARMPageTableMap)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(लंबाई < 2 || current_extra_caps.excaprefs[0] == शून्य)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, बफर);
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vmAttributesFromWord(getSyscallArg(1, बफर));
#पूर्ण_अगर
    pdCap = current_extra_caps.excaprefs[0]->cap;

    अगर (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                 !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCap));
    asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

    अगर (unlikely(vaddr >= USER_शीर्ष)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_शीर्ष);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 0;

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    {
        findPDForASID_ret_t find_ret;

        find_ret = findPDForASID(asid);
        अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(find_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;

            वापस EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_सूचकांक = vaddr >> (PAGE_BITS + PT_सूचकांक_BITS);
    pdSlot = &pd[pd_सूचकांक];
    अगर (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = seL4_DeleteFirst;

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_attributes_get_armParityEnabled(attr),
              0 /* Domain */
          );
#अन्यथा
    pde = pde_pde_coarse_new(paddr);
#पूर्ण_अगर

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, vaddr);

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस performPageTableInvocationMap(cap, cte, pde, pdSlot);
}

अटल exception_t decodeARMFrameInvocation(word_t invLabel, word_t लंबाई,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    चयन (invLabel) {
    हाल ARMPageMap: {
        word_t vaddr, vtop, w_rightsMask;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_page_आकार_t frame_आकार;
        vm_attributes_t attr;

        अगर (unlikely(लंबाई < 3 || current_extra_caps.excaprefs[0] == शून्य)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                seL4_TruncatedMessage;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, बफर);
        w_rightsMask = getSyscallArg(1, बफर);
        attr = vmAttributesFromWord(getSyscallArg(2, बफर));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        frame_आकार = generic_frame_cap_get_capFSize(cap);
        capVMRights = generic_frame_cap_get_capFVMRights(cap);

        अगर (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;

            वापस EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(
                         pdCap));
        asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

        अगर (generic_frame_cap_get_capFIsMapped(cap)) {
            अगर (generic_frame_cap_get_capFMappedASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_संख्या = 1;

                वापस EXCEPTION_SYSCALL_ERROR;
            }

            अगर (generic_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_संख्या = 0;

                वापस EXCEPTION_SYSCALL_ERROR;
            }
        } अन्यथा {
            vtop = vaddr + BIT(page_Bits_For_आकार(frame_आकार)) - 1;

            अगर (unlikely(vtop >= USER_शीर्ष)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_शीर्ष);
                current_syscall_error.type =
                    seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_संख्या = 0;

                वापस EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            findPDForASID_ret_t find_ret;

            find_ret = findPDForASID(asid);
            अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource =
                    false;

                वापस EXCEPTION_SYSCALL_ERROR;
            }

            अगर (unlikely(find_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_संख्या = 1;

                वापस EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            maskVMRights(capVMRights, rightsFromWord(w_rightsMask));

        अगर (unlikely(!checkVPAlignment(frame_आकार, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((व्योम *)
                                   generic_frame_cap_get_capFBasePtr(cap));

        cap = generic_frame_cap_set_capFMappedAddress(cap, asid,
                                                      vaddr);
        अगर (frame_आकार == ARMSmallPage || frame_आकार == ARMLargePage) {
            create_mappings_pte_return_t map_ret;
            map_ret = createSafeMappingEntries_PTE(capFBasePtr, vaddr,
                                                   frame_आकार, vmRights,
                                                   attr, pd);
            अगर (unlikely(map_ret.status != EXCEPTION_NONE)) {
#अगर_घोषित CONFIG_PRINTING
                अगर (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#पूर्ण_अगर
                वापस map_ret.status;
            }

            setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
            वापस performPageInvocationMapPTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } अन्यथा {
            create_mappings_pde_return_t map_ret;
            map_ret = createSafeMappingEntries_PDE(capFBasePtr, vaddr,
                                                   frame_आकार, vmRights,
                                                   attr, pd);
            अगर (unlikely(map_ret.status != EXCEPTION_NONE)) {
#अगर_घोषित CONFIG_PRINTING
                अगर (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#पूर्ण_अगर
                वापस map_ret.status;
            }

            setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
            वापस performPageInvocationMapPDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    हाल ARMPageUnmap: {
#अगर_घोषित CONFIG_TK1_SMMU
        अगर (isIOSpaceFrameCap(cap)) {
            setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
            वापस performPageInvocationUnmapIO(cap, cte);
        } अन्यथा
#पूर्ण_अगर
        {
            setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
            वापस performPageInvocationUnmap(cap, cte);
        }
    }

#अगर_घोषित CONFIG_TK1_SMMU
    हाल ARMPageMapIO: {
        वापस decodeARMIOMapInvocation(invLabel, लंबाई, cte, cap, बफर);
    }
#पूर्ण_अगर

    हाल ARMPage_Clean_तथ्य:
    हाल ARMPage_Invalidate_तथ्य:
    हाल ARMPage_Clean_Invalidate_तथ्य:
    हाल ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        findPDForASID_ret_t pd;
        vptr_t start, end;
        paddr_t pstart;
        word_t page_आकार;
        word_t page_base;

        अगर (लंबाई < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_frame_cap_get_capFMappedASID(cap);
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_frame_cap_get_capFBasePtr(cap);
#अन्यथा
        vaddr = generic_frame_cap_get_capFMappedAddress(cap);
#पूर्ण_अगर

        अगर (unlikely(!generic_frame_cap_get_capFIsMapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        pd = findPDForASID(asid);
        अगर (unlikely(pd.status != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
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
        page_आकार = 1 << page_Bits_For_आकार(generic_frame_cap_get_capFSize(cap));
        page_base = addrFromPPtr((व्योम *)generic_frame_cap_get_capFBasePtr(cap));

#अगर_अघोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        अगर (unlikely(!resolve_ret.valid ||
                     resolve_ret.frame_आकार != generic_frame_cap_get_capFSize(cap) ||
                     resolve_ret.frameBase != page_base)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#पूर्ण_अगर

        अगर (start >= page_आकार || end > page_आकार) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = page_base + start;
        start += vaddr;
        end += vaddr;

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        अगर (pstart < physBase() || pstart > PADDR_शीर्ष || (end - start) > PADDR_शीर्ष - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
#पूर्ण_अगर

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageFlush(invLabel, pd.pd, asid, start, end - 1, pstart);
    }

    हाल ARMPageGetAddress: {


        /* Check that there are enough message registers */
        निश्चित(n_संदेश_Registers >= 1);

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageGetAddress((व्योम *)generic_frame_cap_get_capFBasePtr(cap), call);
    }

    शेष:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        वापस EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t लंबाई, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_page_directory_cap:
        वापस decodeARMPageDirectoryInvocation(invLabel, लंबाई, cptr, cte,
                                                cap, बफर);

    हाल cap_page_table_cap:
        वापस decodeARMPageTableInvocation(invLabel, लंबाई, cte, cap, बफर);

    हाल cap_small_frame_cap:
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
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(लंबाई < 2 || current_extra_caps.excaprefs[0] == शून्य
                     || current_extra_caps.excaprefs[1] == शून्य)) {
            userError("ASIDControlMakePool: Truncated message.");
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
                     cap_untyped_cap_get_cap_Block_आकार(untyped) !=
                     seL4_ASIDPoolBits || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        अगर (unlikely(status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            वापस status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(मूल, सूचकांक, depth);
        अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            वापस lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        अगर (unlikely(status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            वापस status;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performASIDControlInvocation(frame, destSlot,
                                            parentSlot, asid_base);
    }

    हाल cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pdCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        अगर (unlikely(invLabel != ARMASIDPoolAssign)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(current_extra_caps.excaprefs[0] == शून्य)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        pdCapSlot = current_extra_caps.excaprefs[0];
        pdCap = pdCapSlot->cap;

        अगर (unlikely(
                cap_get_capType(pdCap) != cap_page_directory_cap ||
                cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ऊपरी(cap_asid_pool_cap_get_capASIDBase(cap))];
        अगर (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_मूल_new();

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        क्रम (i = 0; i < (1 << asid_निचला_Bits) && (asid + i == 0 || pool->array[i]); i++);

        अगर (unlikely(i == 1 << asid_निचला_Bits)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = seL4_DeleteFirst;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performASIDPoolInvocation(asid, pool, pdCapSlot);
    }

    शेष:
        fail("Invalid ARM arch cap type");
    }
}

#अगर_घोषित CONFIG_KERNEL_LOG_बफर
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

    frame_आकार = generic_frame_cap_get_capFSize(lu_ret.cap);

    अगर (frame_आकार != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = generic_frame_cap_get_capFBasePtr(lu_ret.cap);

    ks_User_Log_बफर = pptr_to_paddr((व्योम *) frame_pptr);

    क्रम (पूर्णांक idx = 0; idx < BIT(PT_सूचकांक_BITS); idx++) {
        paddr_t physical_address = ks_User_Log_बफर + (idx << seL4_PageBits);

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

    वापस EXCEPTION_NONE;
}
#पूर्ण_अगर /* CONFIG_KERNEL_LOG_BUFFER */

#अगर_घोषित CONFIG_DEBUG_BUILD
व्योम kernelPrefetchAbort(word_t pc, word_t lr) VISIBLE;
व्योम kernel_तथ्य_Abort(word_t pc) VISIBLE;

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT

व्योम kernelUndefinedInstruction(word_t pc) VISIBLE;

व्योम kernelPrefetchAbort(word_t pc, word_t lr)
{
    म_लिखो("\n\nKERNEL PREFETCH ABORT!\n");
    म_लिखो("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    म_लिखो("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    म_लिखो("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

व्योम kernel_तथ्य_Abort(word_t pc)
{
    म_लिखो("\n\nKERNEL DATA ABORT!\n");
    म_लिखो("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    म_लिखो("HDFAR: 0x%"SEL4_PRIx_word" HSR: 0x%"SEL4_PRIx_word"\n",
           getHDFAR(), getHSR());
    halt();
}

व्योम kernelUndefinedInstruction(word_t pc)
{
    म_लिखो("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    म_लिखो("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    म_लिखो("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    halt();
}

#अन्यथा /* CONFIG_ARM_HYPERVISOR_SUPPORT */

व्योम kernelPrefetchAbort(word_t pc, word_t lr)
{
    म_लिखो("\n\nKERNEL PREFETCH ABORT!\n");
    म_लिखो("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    म_लिखो("IFSR: 0x%"SEL4_PRIx_word"\n", getIFSR());
    म_लिखो("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

व्योम kernel_तथ्य_Abort(word_t pc)
{
    म_लिखो("\n\nKERNEL DATA ABORT!\n");
    म_लिखो("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    म_लिखो("FAR: 0x%"SEL4_PRIx_word" DFSR: 0x%"SEL4_PRIx_word"\n",
           getFAR(), getDFSR());
    halt();
}
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#पूर्ण_अगर

#अगर_घोषित CONFIG_PRINTING
प्रकार काष्ठा readWordFromVSpace_ret {
    exception_t status;
    word_t मूल्य;
} readWordFromVSpace_ret_t;

अटल readWordFromVSpace_ret_t readWordFromVSpace(pde_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    lookupPTSlot_ret_t ptSlot;
    pde_t *pdSlot;
    paddr_t paddr;
    word_t offset;
    pptr_t kernel_vaddr;
    word_t *मूल्य;

    pdSlot = lookupPDSlot(pd, vaddr);
    अगर (pde_ptr_get_pdeType(pdSlot) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_address(pdSlot);
        offset = vaddr & MASK(ARMSectionBits);
    } अन्यथा {
        ptSlot = lookupPTSlot(pd, vaddr);
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        अगर (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_address(ptSlot.ptSlot);
            अगर (pte_pte_small_ptr_get_contiguous_hint(ptSlot.ptSlot)) {
                offset = vaddr & MASK(ARMLargePageBits);
            } अन्यथा {
                offset = vaddr & MASK(ARMSmallPageBits);
            }
#अन्यथा
        अगर (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_address(ptSlot.ptSlot);
            offset = vaddr & MASK(ARMSmallPageBits);
        } अन्यथा अगर (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_address(ptSlot.ptSlot);
            offset = vaddr & MASK(ARMLargePageBits);
#पूर्ण_अगर
        } अन्यथा {
            ret.status = EXCEPTION_LOOKUP_FAULT;
            वापस ret;
        }
    }


    kernel_vaddr = (word_t)paddr_to_pptr(paddr);
    मूल्य = (word_t *)(kernel_vaddr + offset);
    ret.status = EXCEPTION_NONE;
    ret.मूल्य = *मूल्य;
    वापस ret;
}

व्योम Arch_user_ढेर_Trace(tcb_t *tptr)
{
    cap_t thread_मूल;
    pde_t *pd;
    word_t sp;
    पूर्णांक i;

    thread_मूल = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    अगर (cap_get_capType(thread_मूल) != cap_page_directory_cap) {
        म_लिखो("Invalid vspace\n");
        वापस;
    }

    pd = (pde_t *)pptr_of_cap(thread_मूल);

    sp = getRegister(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    अगर (!IS_ALIGNED(sp, se_L4_Word_आकार_Bits)) {
        म_लिखो("SP not aligned\n");
        वापस;
    }

    क्रम (i = 0; i < CONFIG_USER_ढेर_TRACE_लंबाई; i++) {
        word_t address = sp + (i * माप(word_t));
        readWordFromVSpace_ret_t परिणाम;
        परिणाम = readWordFromVSpace(pd, address);
        अगर (परिणाम.status == EXCEPTION_NONE) {
            म_लिखो("0x%lx: 0x%lx\n", (दीर्घ)address, (दीर्घ)परिणाम.मूल्य);
        } अन्यथा {
            म_लिखो("0x%lx: INVALID\n", (दीर्घ)address);
        }
    }
}
#पूर्ण_अगर
