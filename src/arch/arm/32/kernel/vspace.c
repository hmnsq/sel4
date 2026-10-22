/*
 * Copyright 2014, General Dynamics C4 Systems
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

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#અન્તર્ભાવ <benchmark/benchmark_track.h>
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

સ્થિર શૂન્ય setGlobalPD(શૂન્ય);

સંરચના resolve_ret {
    paddr_t frameBase;
    vm_page_size_t frameSize;
    bool_t valid;
};
પ્રકારવ્યાખ્યા સંરચના resolve_ret resolve_ret_t;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
સ્થિર bool_t PURE pteCheckIfMapped(pte_t *pte);
સ્થિર bool_t PURE pdeCheckIfMapped(pde_t *pde);

સ્થિર word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    પસંદગી (vm_rights) {
    સ્થિતિ VMNoAccess:
        પાછા 0;

    સ્થિતિ VMKernelOnly:
        પાછા 1;

    સ્થિતિ VMReadOnly:
        પાછા 2;

    સ્થિતિ VMReadWrite:
        પાછા 3;

    મૂળભૂત:
        fail("Invalid VM rights");
    }
}

#નહીંતર
/* AP encoding slightly different. AP only used for kernel mappings which are fixed after boot time */
BOOT_CODE
સ્થિર word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    પસંદગી (vm_rights) {
    સ્થિતિ VMKernelOnly:
        પાછા 0;
    સ્થિતિ VMReadWrite:
        પાછા 1;
    સ્થિતિ VMNoAccess:
        /* RO at PL1 only */
        પાછા 2;
    સ્થિતિ VMReadOnly:
        પાછા 3;
    મૂળભૂત:
        fail("Invalid VM rights");
    }
}

સ્થિર word_t CONST HAPFromVMRights(vm_rights_t vm_rights)
{
    પસંદગી (vm_rights) {
    સ્થિતિ VMKernelOnly:
    સ્થિતિ VMNoAccess:
        પાછા 0;
    સ્થિતિ VMReadOnly:
        પાછા 1;
    /*
    case VMWriteOnly:
        return 2;
    */
    સ્થિતિ VMReadWrite:
        પાછા 3;
    મૂળભૂત:
        fail("Invalid VM rights");
    }
}

#endif

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    જો (vm_rights == VMNoAccess) {
        પાછા VMNoAccess;
    }
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

BOOT_CODE શૂન્ય map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_attributes_t attributes)
{
    word_t idx = (vaddr & MASK(pageBitsForSize(ARMSection))) >> pageBitsForSize(ARMSmallPage);

    પ્રતિજ્ઞા(vaddr >= PPTR_ટોચ); /* vaddr lies in the region the global PT covers */
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t tex;
    જો (vm_attributes_get_armPageCacheable(attributes)) {
        tex = 5; /* TEX = 0b1(Cached)01(Outer Write Allocate) */
    } નહીંતર {
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
#નહીંતર /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
BOOT_CODE શૂન્ય map_kernel_window(શૂન્ય)
{
    paddr_t  phys;
    word_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = physBase();
    idx = PPTR_BASE >> pageBitsForSize(ARMSection);

    જ્યારે (idx < BIT(PD_INDEX_BITS) - SECTIONS_PER_SUPER_SECTION) {
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
        માટે (idx2 = idx; idx2 < idx + SECTIONS_PER_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += BIT(pageBitsForSize(ARMSuperSection));
        idx += SECTIONS_PER_SUPER_SECTION;
    }

    જ્યારે (idx < (PPTR_ટોચ >> pageBitsForSize(ARMSection))) {
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
    પ્રતિજ્ઞા(phys == PADDR_ટોચ);

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

#નહીંતર /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_CODE શૂન્ય map_kernel_window(શૂન્ય)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t UNUSED pteS2;

    /* Initialise PGD */
    માટે (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    માટે (idx = 0; idx < (USER_ટોચ - 0xC0000000) >> (PT_INDEX_BITS + PAGE_BITS); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = physBase();
    માટે (; idx < BIT(PT_INDEX_BITS) - 1; idx++) {
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
    માટે (idx = 0; idx < 256; idx++) {
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

સ્થિર BOOT_CODE શૂન્ય map_it_frame_cap(cap_t pd_cap, cap_t frame_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *targetSlot;
    pde_t *pd    = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    શૂન્ય  *frame = (શૂન્ય *)generic_frame_cap_get_capFBasePtr(frame_cap);
    vptr_t vptr  = generic_frame_cap_get_capFMappedAddress(frame_cap);

    પ્રતિજ્ઞા(generic_frame_cap_get_capFMappedASID(frame_cap) != 0);

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
#નહીંતર
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

સ્થિર BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    જો (use_large)
        પાછા
            cap_frame_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_નીચલું(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_ઉપલું(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    નહીંતર
        પાછા
            cap_small_frame_cap_new(
                ASID_નીચલું(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#ifdef CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#endif
                ASID_ઉપલું(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

સ્થિર BOOT_CODE શૂન્ય map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);
    pde_t *targetSlot = pd + (vptr >> pageBitsForSize(ARMSection));

    પ્રતિજ્ઞા(cap_page_table_cap_get_capPTIsMapped(pt_cap));

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    *targetSlot = pde_pde_coarse_new(
                      addrFromPPtr(pt), /* address */
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#નહીંતર
    *targetSlot = pde_pde_coarse_new(addrFromPPtr(pt));
#endif
}

/* Create a page table for the initial thread */

સ્થિર BOOT_CODE cap_t create_it_page_table_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    જો (asid != asidInvalid) {
        map_it_pt_cap(pd, cap);
    }
    પાછા cap;
}

BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    પાછા get_n_paging(it_v_reg, PT_INDEX_BITS + PAGE_BITS);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t create_it_address_space(cap_t મૂળ_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << seL4_PageDirBits) - 1,
                        addrFromPPtr((શૂન્ય *)rootserver.vspace));
    cap_t pd_cap =
        cap_page_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_of_cap(મૂળ_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    માટે (pt_vptr = ROUND_DOWN(it_v_reg.start, PT_INDEX_BITS + PAGE_BITS);
         pt_vptr < it_v_reg.end;
         pt_vptr += BIT(PT_INDEX_BITS + PAGE_BITS)) {
        જો (!provide_cap(મૂળ_cnode_cap,
                         create_it_page_table_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            પાછા cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    પાછા pd_cap;
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

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_CODE શૂન્ય activate_kernel_vspace(શૂન્ય)
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

#નહીંતર

BOOT_CODE શૂન્ય activate_kernel_vspace(શૂન્ય)
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
#જો 0 /* Can't lock entries on A15 */
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

BOOT_CODE શૂન્ય write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[ASID_નીચલું(IT_ASID)] = PDE_PTR(pptr_of_cap(it_pd_cap));
    armKSASIDTable[ASID_ઉપલું(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

findPDForASID_ret_t findPDForASID(asid_t asid)
{
    findPDForASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_ઉપલું(asid)];
    જો (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_મૂળ_new();

        ret.pd = ખાલી;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        પાછા ret;
    }

    pd = poolPtr->array[ASID_નીચલું(asid)];
    જો (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_મૂળ_new();

        ret.pd = ખાલી;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        પાછા ret;
    }

    ret.pd = pd;
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

    જો (unlikely(cap_get_capType(bufferCap) != cap_small_frame_cap &&
                 cap_get_capType(bufferCap) != cap_frame_cap)) {
        પાછા ખાલી;
    }
    જો (unlikely(generic_frame_cap_get_capFIsDevice(bufferCap))) {
        પાછા ખાલી;
    }

    vm_rights = generic_frame_cap_get_capFVMRights(bufferCap);
    જો (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr;
        unsigned પૂર્ણાંક pageBits;

        basePtr = generic_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(generic_frame_cap_get_capFSize(bufferCap));
        પાછા (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } નહીંતર {
        પાછા ખાલી;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    જો (unlikely(cap_get_capType(cap) != cap_small_frame_cap &&
                 cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (unlikely(generic_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(vptr & MASK(seL4_IPCBufferSizeBits))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (પૂર્ણાંક)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    પાછા EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    unsigned પૂર્ણાંક pdIndex;

    pdIndex = vptr >> (PAGE_BITS + PT_INDEX_BITS);
    પાછા pd + pdIndex;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pdSlot;

    pdSlot = lookupPDSlot(pd, vptr);

    જો (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_INDEX_BITS + PAGE_BITS);

        ret.ptSlot = ખાલી;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        પાછા ret;
    } નહીંતર {
        pte_t *pt, *ptSlot;
        unsigned પૂર્ણાંક ptIndex;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pdSlot));
        ptIndex = (vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS);
        ptSlot = pt + ptIndex;

        ret.ptSlot = ptSlot;
        ret.status = EXCEPTION_NONE;
        પાછા ret;
    }
}

સ્થિર pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    unsigned પૂર્ણાંક ptIndex;

    ptIndex = (vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS);
    પાછા pt + ptIndex;
}

સ્થિર અચલ resolve_ret_t default_resolve_ret_t;

સ્થિર resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    પસંદગી (pde_ptr_get_pdeType(pde)) {
    સ્થિતિ pde_pde_section:
        ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        જો (pde_pde_section_ptr_get_size(pde)) {
            ret.frameSize = ARMSuperSection;
        } નહીંતર {
            ret.frameSize = ARMSection;
        }
#નહીંતર
        જો (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
            ret.frameSize = ARMSuperSection;
        } નહીંતર {
            ret.frameSize = ARMSection;
        }
#endif
        પાછા ret;

    સ્થિતિ pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        પસંદગી (pte_ptr_get_pteType(pte)) {
        સ્થિતિ pte_pte_small:
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            જો (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } નહીંતર {
                ret.frameSize = ARMSmallPage;
            }
#નહીંતર
            ret.frameSize = ARMSmallPage;
#endif
            પાછા ret;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        સ્થિતિ pte_pte_large:
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            પાછા ret;
#endif
        મૂળભૂત:
            તોડો;
        }
        તોડો;
    }
    }

    ret.valid = false;
    પાછા ret;
}

સ્થિર pte_t CONST makeUserPTE(vm_page_size_t page_size, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap;

    ap = APFromVMRights(vm_rights);

    પસંદગી (page_size) {
    સ્થિતિ ARMSmallPage: {
        જો (cacheable) {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    nonexecutable);
        } નહીંતર {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    0, /* TEX = 0b000, strongly-ordered. */
                                    ap,
                                    0, 0,
                                    nonexecutable);
        }
        તોડો;
    }

    સ્થિતિ ARMLargePage: {
        જો (cacheable) {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    1 /* reserved */);
        } નહીંતર {
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
        તોડો;
    }

    મૂળભૂત:
        fail("Invalid PTE frame type");
    }

#નહીંતર

    word_t hap;

    hap = HAPFromVMRights(vm_rights);

    પસંદગી (page_size) {
    સ્થિતિ ARMSmallPage: {
        જો (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_CACHEABLE /* Cacheable */);
        } નહીંતર {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        તોડો;
    }

    સ્થિતિ ARMLargePage: {
        જો (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_CACHEABLE  /* Cacheable */);
        } નહીંતર {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        તોડો;
    }
    મૂળભૂત:
        fail("Invalid PTE frame type");
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    પાછા pte;
}

સ્થિર pde_t CONST makeUserPDE(vm_page_size_t page_size, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, word_t domain,
                               vm_rights_t vm_rights)
{
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap, size2;

    ap = APFromVMRights(vm_rights);
#નહીંતર
    word_t hap, size2;

    (શૂન્ય)domain;
    hap = HAPFromVMRights(vm_rights);
#endif

    પસંદગી (page_size) {
    સ્થિતિ ARMSection:
        size2 = 0;
        તોડો;

    સ્થિતિ ARMSuperSection:
        size2 = 1;
        તોડો;

    મૂળભૂત:
        fail("Invalid PDE frame type");
    }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    જો (cacheable) {
        પાછા pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                   0, /* APX = 0, privileged full access */
                                   5, /* TEX = 0b101, outer write-back, write-allocate */
                                   ap, parity, domain, nonexecutable,
                                   0, 1 /* Inner write-back, write-allocate */);
    } નહીંતર {
        પાછા pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   1, /* shared */
                                   0, /* APX = 0, privileged full access */
                                   0, /* TEX = 0b000, strongly-ordered */
                                   ap, parity, domain, nonexecutable,
                                   0, 0);
    }
#નહીંતર
    જો (cacheable) {
        પાછા pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_CACHEABLE /* Cacheable */);
    } નહીંતર {
        પાછા pde_pde_section_new(
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

bool_t CONST is_Valid_VTable_મૂળ(cap_t cap)
{
    પાછા cap_get_capType(cap) == cap_page_directory_cap &&
           cap_page_directory_cap_get_capPDIsMapped(cap);
}

bool_t CONST isIOSpaceFrameCap(cap_t cap)
{
#ifdef CONFIG_TK1_SMMU
    પાછા cap_get_capType(cap) == cap_small_frame_cap && cap_small_frame_cap_get_capFIsIOSpace(cap);
#નહીંતર
    પાછા false;
#endif
}

/** Switch to the empty PD on the reserved HW ASID */
સ્થિર શૂન્ય setGlobalPD(શૂન્ય)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    writeContextIDAndPD(hwASIDReserved, addrFromKPPtr(armUSGlobalPD));
#નહીંતર
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

શૂન્ય setVMRoot(tcb_t *tcb)
{
    cap_t thread_મૂળ;
    asid_t asid;
    pde_t *pd;
    findPDForASID_ret_t find_ret;

    thread_મૂળ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    જો (cap_get_capType(thread_મૂળ) != cap_page_directory_cap ||
        !cap_page_directory_cap_get_capPDIsMapped(thread_મૂળ)) {
        setGlobalPD();
        પાછા;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_મૂળ));
    asid = cap_page_directory_cap_get_capPDMappedASID(thread_મૂળ);
    find_ret = findPDForASID(asid);
    જો (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.pd != pd)) {
        setGlobalPD();
        પાછા;
    }

    armv_contextSwitch(pd, asid);
}

સ્થિર bool_t setVMRootForFlush(pde_t *pd, asid_t asid)
{
    cap_t thread_મૂળ;

    thread_મૂળ = TCB_PTR_CTE_PTR(ગાંઠ_STATE(ksCurThread), tcbVTable)->cap;

    જો (cap_get_capType(thread_મૂળ) == cap_page_directory_cap &&
        cap_page_directory_cap_get_capPDIsMapped(thread_મૂળ) &&
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_મૂળ)) == pd) {
        પાછા false;
    }

    armv_contextSwitch(pd, asid);

    પાછા true;
}

pde_t *pageTableMapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    findPDForASID_ret_t find_ret;
    pde_t pde;
    unsigned પૂર્ણાંક pdIndex;

    find_ret = findPDForASID(asid);
    જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
        પાછા ખાલી;
    }

    pdIndex = vaddr >> (PAGE_BITS + PT_INDEX_BITS);
    pde = find_ret.pd[pdIndex];

    જો (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_address(pde)) == pt)) {
        પાછા find_ret.pd;
    } નહીંતર {
        પાછા ખાલી;
    }
}

સ્થિર શૂન્ય invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ઉપલું(asid)];
    પ્રતિજ્ઞા(asidPool);

    pd = asidPool->array[ASID_નીચલું(asid)];
    પ્રતિજ્ઞા(pd);

    pd[PD_ASID_SLOT] = pde_pde_invalid_new(0, false);
}

સ્થિર pde_t PURE loadHWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ઉપલું(asid)];
    પ્રતિજ્ઞા(asidPool);

    pd = asidPool->array[ASID_નીચલું(asid)];
    પ્રતિજ્ઞા(pd);

    પાછા pd[PD_ASID_SLOT];
}

સ્થિર શૂન્ય storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ઉપલું(asid)];
    પ્રતિજ્ઞા(asidPool);

    pd = asidPool->array[ASID_નીચલું(asid)];
    પ્રતિજ્ઞા(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_SLOT] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t findFreeHWASID(શૂન્ય)
{
    word_t hw_asid_offset;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    માટે (hw_asid_offset = 0;
         hw_asid_offset <= (word_t)((hw_asid_t) - 1);
         hw_asid_offset ++) {
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

    /* Increment the NextASID index, skipping the reserved ASID 0 on wrap */
    armKSNextASID++;
    જો (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    પાછા hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);
    જો (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        પાછા pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } નહીંતર {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        પાછા new_hw_asid;
    }
}

સ્થિર શૂન્ય invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);
    જો (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

શૂન્ય unmapPageTable(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pdSlot;
    unsigned પૂર્ણાંક pdIndex;

    pd = pageTableMapped(asid, vaddr, pt);

    જો (likely(pd != ખાલી)) {
        pdIndex = vaddr >> (PT_INDEX_BITS + PAGE_BITS);
        pdSlot = pd + pdIndex;

        *pdSlot = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));
        flushTable(pd, asid, vaddr, pt);
    }
}

શૂન્ય copyGlobalMappings(pde_t *newPD)
{
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t i;
    pde_t *global_pd = armKSGlobalPD;

    માટે (i = PPTR_BASE >> ARMSectionBits; i < BIT(PD_INDEX_BITS); i++) {
        જો (i != PD_ASID_SLOT) {
            newPD[i] = global_pd[i];
        }
    }
#endif
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    પસંદગી (vm_faultType) {
    સ્થિતિ ARMDataAbort: {
        word_t addr, fault;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (addressTranslateS1(addr) & ~MASK(PAGE_BITS)) | (addr & MASK(PAGE_BITS));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#નહીંતર
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
        જો (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(0);
            પાછા EXCEPTION_FAULT;
        }
#endif
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        પાછા EXCEPTION_FAULT;
    }

    સ્થિતિ ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (addressTranslateS1(pc) & ~MASK(PAGE_BITS)) | (pc & MASK(PAGE_BITS));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#નહીંતર
        fault = getIFSR();
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
        જો (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            જો (seL4_Fault_DebugException_get_exceptionReason(current_fault) == seL4_SingleStep
                && !singleStepFaultCounterReady(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                પાછા EXCEPTION_NONE;
            }
            પાછા EXCEPTION_FAULT;
        }
#endif
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        પાછા EXCEPTION_FAULT;
    }

    મૂળભૂત:
        fail("Invalid VM fault type");
    }
}

શૂન્ય deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    unsigned પૂર્ણાંક offset;

    /* Haskell error: "ASID pool's base must be aligned" */
    પ્રતિજ્ઞા((asid_base & MASK(asid_નીચલું_Bits)) == 0);

    જો (armKSASIDTable[ASID_ઉપલું(asid_base)] == pool) {
        માટે (offset = 0; offset < BIT(asid_નીચલું_Bits); offset++) {
            જો (pool->array[offset]) {
                flushSpace(asid_base + offset);
                invalidateASIDEntry(asid_base + offset);
            }
        }
        armKSASIDTable[ASID_ઉપલું(asid_base)] = ખાલી;
        setVMRoot(ગાંઠ_STATE(ksCurThread));
    }
}

શૂન્ય deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ઉપલું(asid)];

    જો (poolPtr != ખાલી && poolPtr->array[ASID_નીચલું(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_નીચલું(asid)] = ખાલી;
        setVMRoot(ગાંઠ_STATE(ksCurThread));
    }
}

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
સ્થિર pte_t pte_pte_invalid_new(શૂન્ય)
{
    /* Invalid as every PTE must have bit 0 set (large PTE) or bit 1 set (small
     * PTE). 0 == 'translation fault' in ARM parlance.
     */
    પાછા (pte_t) {
        {
            0
        }
    };
}
#endif

શૂન્ય unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, શૂન્ય *pptr)
{
    findPDForASID_ret_t find_ret;
    paddr_t addr = addrFromPPtr(pptr);

    find_ret = findPDForASID(asid);
    જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
        પાછા;
    }

    પસંદગી (page_size) {
    સ્થિતિ ARMSmallPage: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            પાછા;
        }

        જો (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            પાછા;
        }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        જો (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 0)) {
            પાછા;
        }
#endif
        જો (unlikely(pte_pte_small_ptr_get_address(lu_ret.ptSlot) != addr)) {
            પાછા;
        }

        *(lu_ret.ptSlot) = pte_pte_invalid_new();
        cleanByVA_PoU((word_t)lu_ret.ptSlot, addrFromPPtr(lu_ret.ptSlot));

        તોડો;
    }

    સ્થિતિ ARMLargePage: {
        lookupPTSlot_ret_t lu_ret;
        word_t i;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            પાછા;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        જો (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_large)) {
            પાછા;
        }
        જો (unlikely(pte_pte_large_ptr_get_address(lu_ret.ptSlot) != addr)) {
            પાછા;
        }
#નહીંતર
        જો (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            પાછા;
        }
        જો (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 1)) {
            પાછા;
        }
        જો (unlikely(pte_pte_small_ptr_get_address(lu_ret.ptSlot) != addr)) {
            પાછા;
        }
#endif

        માટે (i = 0; i < PAGES_PER_LARGE_PAGE; i++) {
            lu_ret.ptSlot[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((word_t)&lu_ret.ptSlot[0],
                            LAST_BYTE_PTE(lu_ret.ptSlot, PAGES_PER_LARGE_PAGE),
                            addrFromPPtr(&lu_ret.ptSlot[0]));

        તોડો;
    }

    સ્થિતિ ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(find_ret.pd, vptr);

        જો (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            પાછા;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        જો (unlikely(pde_pde_section_ptr_get_size(pd) != 0)) {
#નહીંતર
        જો (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#endif
            પાછા;
        }
        જો (unlikely(pde_pde_section_ptr_get_address(pd) != addr)) {
            પાછા;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pd, addrFromPPtr(pd));

        તોડો;
    }

    સ્થિતિ ARMSuperSection: {
        pde_t *pd;
        word_t i;

        pd = lookupPDSlot(find_ret.pd, vptr);

        જો (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            પાછા;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        જો (unlikely(pde_pde_section_ptr_get_size(pd) != 1)) {
#નહીંતર
        જો (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#endif
            પાછા;
        }
        જો (unlikely(pde_pde_section_ptr_get_address(pd) != addr)) {
            પાછા;
        }

        માટે (i = 0; i < SECTIONS_PER_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((word_t)&pd[0], LAST_BYTE_PDE(pd, SECTIONS_PER_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        તોડો;
    }

    મૂળભૂત:
        fail("Invalid ARM page type");
        તોડો;
    }

    /* Flush the page now that the mapping has been updated */
    flushPage(page_size, find_ret.pd, asid, vptr);
}

શૂન્ય flushPage(vm_page_size_t page_size, pde_t *pd, asid_t asid, word_t vptr)
{
    pde_t stored_hw_asid;
    word_t base_addr;
    bool_t મૂળ_switched;

    પ્રતિજ્ઞા((vptr & MASK(pageBitsForSize(page_size))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    મૂળ_switched = setVMRootForFlush(pd, asid);
    stored_hw_asid = loadHWASID(asid);

    જો (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        base_addr = vptr & ~MASK(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(base_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        જો (મૂળ_switched) {
            setVMRoot(ગાંઠ_STATE(ksCurThread));
        }
    }
}

શૂન્ય flushTable(pde_t *pd, asid_t asid, word_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t મૂળ_switched;

    પ્રતિજ્ઞા((vptr & MASK(PT_INDEX_BITS + ARMSmallPageBits)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    મૂળ_switched = setVMRootForFlush(pd, asid);
    stored_hw_asid = loadHWASID(asid);

    જો (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        જો (મૂળ_switched) {
            setVMRoot(ગાંઠ_STATE(ksCurThread));
        }
    }
}

શૂન્ય flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);

    /* Clean the entire data cache, to guarantee that any VAs mapped
     * in the deleted space are clean (because we can't clean by VA after
     * deleting the space) */
    cleanCaches_PoU();

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    જો (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        પાછા;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

શૂન્ય invalidateTLBByASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    જો (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        પાછા;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

સ્થિર અંતઃરેખ bool_t CONST checkVPAlignment(vm_page_size_t sz, word_t w)
{
    પાછા (w & MASK(pageBitsForSize(sz))) == 0;
}

સંરચના create_mappings_pte_return {
    exception_t status;
    pte_t pte;
    pte_range_t pte_entries;
};
પ્રકારવ્યાખ્યા સંરચના create_mappings_pte_return create_mappings_pte_return_t;

સંરચના create_mappings_pde_return {
    exception_t status;
    pde_t pde;
    pde_range_t pde_entries;
};
પ્રકારવ્યાખ્યા સંરચના create_mappings_pde_return create_mappings_pde_return_t;

સ્થિર create_mappings_pte_return_t
createSafeMappingEntries_PTE
(paddr_t base, word_t vaddr, vm_page_size_t frameSize,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    word_t i;

    પસંદગી (frameSize) {

    સ્થિતિ ARMSmallPage:

        ret.pte_entries.base = ખાલી; /* to avoid uninitialised warning */
        ret.pte_entries.length = 1;

        ret.pte = makeUserPTE(ARMSmallPage, base,
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            પાછા ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        જો (unlikely(pte_ptr_get_pteType(ret.pte_entries.base) ==
                     pte_pte_large)) {
#નહીંતર
        જો (unlikely(pte_ptr_get_pteType(ret.pte_entries.base) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.base))) {
#endif
            current_syscall_error.type =
                seL4_DeleteFirst;

            ret.status = EXCEPTION_SYSCALL_ERROR;
            પાછા ret;
        }

        ret.status = EXCEPTION_NONE;
        પાછા ret;

    સ્થિતિ ARMLargePage:

        ret.pte_entries.base = ખાલી; /* to avoid uninitialised warning */
        ret.pte_entries.length = PAGES_PER_LARGE_PAGE;

        ret.pte = makeUserPTE(ARMLargePage, base,
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            પાછા ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;

        માટે (i = 0; i < PAGES_PER_LARGE_PAGE; i++) {
            જો (unlikely(pte_get_pteType(ret.pte_entries.base[i]) ==
                         pte_pte_small)
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.base[i])
#endif
               ) {
                current_syscall_error.type =
                    seL4_DeleteFirst;

                ret.status = EXCEPTION_SYSCALL_ERROR;
                પાછા ret;
            }
        }

        ret.status = EXCEPTION_NONE;
        પાછા ret;

    મૂળભૂત:
        fail("Invalid or unexpected ARM page type.");

    }
}

સ્થિર create_mappings_pde_return_t
createSafeMappingEntries_PDE
(paddr_t base, word_t vaddr, vm_page_size_t frameSize,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_tag_t currentPDEType;
    word_t i;

    પસંદગી (frameSize) {

    /* PDE mappings */
    સ્થિતિ ARMSection:
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
        જો (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_size(ret.pde_entries.base) != 0))) {
#નહીંતર
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.base) != 0))) {
#endif
            current_syscall_error.type =
                seL4_DeleteFirst;
            ret.status = EXCEPTION_SYSCALL_ERROR;

            પાછા ret;
        }

        ret.status = EXCEPTION_NONE;
        પાછા ret;

    સ્થિતિ ARMSuperSection:
        ret.pde_entries.base = lookupPDSlot(pd, vaddr);
        ret.pde_entries.length = SECTIONS_PER_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, base,
                              vm_attributes_get_armParityEnabled(attr),
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              0,
                              vmRights);

        માટે (i = 0; i < SECTIONS_PER_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.base[i]);
            જો (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_size(ret.pde_entries.base[i]) != 1))) {
#નહીંતર
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.base[i]) != 1))) {
#endif
                current_syscall_error.type =
                    seL4_DeleteFirst;
                ret.status = EXCEPTION_SYSCALL_ERROR;

                પાછા ret;
            }
        }

        ret.status = EXCEPTION_NONE;
        પાછા ret;

    મૂળભૂત:
        fail("Invalid or unexpected ARM page type.");

    }
}

સ્થિર અંતઃરેખ vptr_t pageBase(vptr_t vaddr, vm_page_size_t size)
{
    પાછા vaddr & ~MASK(pageBitsForSize(size));
}

સ્થિર bool_t PURE pteCheckIfMapped(pte_t *pte)
{
    પાછા pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

સ્થિર bool_t PURE pdeCheckIfMapped(pde_t *pde)
{
    પાછા pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

સ્થિર શૂન્ય doFlush(પૂર્ણાંક invLabel, vptr_t start, vptr_t end, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    જો (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        end = (vptr_t)paddr_to_pptr(pstart) + (end - start);
        start = (vptr_t)paddr_to_pptr(pstart);
    }
    પસંદગી (invLabel) {
    સ્થિતિ ARMPDClean_Data:
    સ્થિતિ ARMPageClean_Data:
        cleanCacheRange_RAM(start, end, pstart);
        તોડો;
    સ્થિતિ ARMPDInvalidate_Data:
    સ્થિતિ ARMPageInvalidate_Data:
        invalidateCacheRange_RAM(start, end, pstart);
        તોડો;
    સ્થિતિ ARMPDCleanInvalidate_Data:
    સ્થિતિ ARMPageCleanInvalidate_Data:
        cleanInvalidateCacheRange_RAM(start, end, pstart);
        તોડો;
    સ્થિતિ ARMPDUnify_Instruction:
    સ્થિતિ ARMPageUnify_Instruction:
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
        તોડો;
    મૂળભૂત:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

સ્થિર exception_t performPDFlush(પૂર્ણાંક invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                  vptr_t end, paddr_t pstart)
{
    bool_t મૂળ_switched;

    /* Flush if given a non zero range */
    જો (start < end) {
        મૂળ_switched = setVMRootForFlush(pd, asid);

        doFlush(invLabel, start, end, pstart);

        જો (મૂળ_switched) {
            setVMRoot(ગાંઠ_STATE(ksCurThread));
        }
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                                 pde_t pde, pde_t *pdSlot)
{
    ctSlot->cap = cap;
    *pdSlot = pde;
    cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    જો (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt);
        clearMemory_PT((શૂન્ય *)pt, cap_get_capSizeBits(cap));
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageInvocationMapPTE(asid_t asid, cap_t cap, cte_t *ctSlot, pte_t pte,
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
    માટે (i = 0; i < pte_entries.length; i++) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_set_address(pte, base_address + i * BIT(pageBitsForSize(ARMSmallPage)));
#endif
        pte_entries.base[i] = pte;
    }
    cleanCacheRange_PoU((word_t)pte_entries.base,
                        LAST_BYTE_PTE(pte_entries.base, pte_entries.length),
                        addrFromPPtr(pte_entries.base));
    જો (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageInvocationMapPDE(asid_t asid, cap_t cap, cte_t *ctSlot, pde_t pde,
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
    માટે (i = 0; i < pde_entries.length; i++) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_set_address(pde, base_address + i * BIT(pageBitsForSize(ARMSection)));
#endif
        pde_entries.base[i] = pde;
    }
    cleanCacheRange_PoU((word_t)pde_entries.base,
                        LAST_BYTE_PDE(pde_entries.base, pde_entries.length),
                        addrFromPPtr(pde_entries.base));
    જો (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    જો (generic_frame_cap_get_capFIsMapped(cap)) {
        unmapPage(generic_frame_cap_get_capFSize(cap),
                  generic_frame_cap_get_capFMappedASID(cap),
                  generic_frame_cap_get_capFMappedAddress(cap),
                  (શૂન્ય *)generic_frame_cap_get_capFBasePtr(cap));
    }

    generic_frame_cap_ptr_set_capFMappedAddress(&ctSlot->cap, asidInvalid, 0);

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageFlush(પૂર્ણાંક invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                    vptr_t end, paddr_t pstart)
{
    bool_t મૂળ_switched;

    /* now we can flush. But only if we were given a non zero range */
    જો (start < end) {
        મૂળ_switched = setVMRootForFlush(pd, asid);

        doFlush(invLabel, start, end, pstart);

        જો (મૂળ_switched) {
            setVMRoot(ગાંઠ_STATE(ksCurThread));
        }
    }

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageGetAddress(શૂન્ય *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);
    જો (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned પૂર્ણાંક length = setMR(thread, ipcBuffer, 0, capFBasePtr);
        setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pdCapSlot)
{
    cap_page_directory_cap_ptr_set_capPDMappedASID(&pdCapSlot->cap, asid);
    cap_page_directory_cap_ptr_set_capPDIsMapped(&pdCapSlot->cap, 1);
    poolPtr->array[ASID_નીચલું(asid)] =
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCapSlot->cap));

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performASIDControlInvocation(શૂન્ય *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_base)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, 1 << ARMSmallPageBits);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ઉમેરો(cap_asid_pool_cap_new(asid_base, WORD_REF(frame)),
              parent, slot);;
    /* Haskell error: "ASID pool's base must be aligned" */
    પ્રતિજ્ઞા((asid_base & MASK(asid_નીચલું_Bits)) == 0);
    armKSASIDTable[ASID_ઉપલું(asid_base)] = (asid_pool_t *)frame;

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeARMPageDirectoryInvocation(word_t invLabel, word_t length,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    word_t *buffer)
{
    પસંદગી (invLabel) {
    સ્થિતિ ARMPDClean_Data:
    સ્થિતિ ARMPDInvalidate_Data:
    સ્થિતિ ARMPDCleanInvalidate_Data:
    સ્થિતિ ARMPDUnify_Instruction: {
        vptr_t start, end;
        paddr_t pstart;
        findPDForASID_ret_t find_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        જો (length < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, buffer);
        end =   getSyscallArg(1, buffer);

        /* Check sanity of arguments */
        જો (end <= start) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 1;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        જો (start >= USER_ટોચ || end > USER_ટોચ) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(cap_get_capType(cap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap));
        asid = cap_page_directory_cap_get_capPDMappedASID(cap);

        find_ret = findPDForASID(asid);
        જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(find_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, start);

        /* Check that there's actually something there. */
        જો (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
            પાછા EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        જો (pageBase(start, resolve_ret.frameSize) !=
            pageBase(end - 1, resolve_ret.frameSize)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax =
                pageBase(start, resolve_ret.frameSize) +
                MASK(pageBitsForSize(resolve_ret.frameSize));
            પાછા EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.frameBase
                 + (start & MASK(pageBitsForSize(resolve_ret.frameSize)));

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        જો (pstart < physBase() || pstart > PADDR_ટોચ || (end - start) > PADDR_ટોચ - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPDFlush(invLabel, pd, asid, start, end - 1, pstart);
    }

    મૂળભૂત:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

}

સ્થિર exception_t decodeARMPageTableInvocation(word_t invLabel, word_t length,
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

    જો (invLabel == ARMPageTableUnmap) {
        જો (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageTableInvocationUnmap(cap, cte);
    }

    જો (unlikely(invLabel != ARMPageTableMap)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(length < 2 || current_extra_caps.excaprefs[0] == ખાલી)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vmAttributesFromWord(getSyscallArg(1, buffer));
#endif
    pdCap = current_extra_caps.excaprefs[0]->cap;

    જો (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                 !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCap));
    asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

    જો (unlikely(vaddr >= USER_ટોચ)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_ટોચ);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 0;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    {
        findPDForASID_ret_t find_ret;

        find_ret = findPDForASID(asid);
        જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(find_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }
    }

    pdIndex = vaddr >> (PAGE_BITS + PT_INDEX_BITS);
    pdSlot = &pd[pdIndex];
    જો (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = seL4_DeleteFirst;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_attributes_get_armParityEnabled(attr),
              0 /* Domain */
          );
#નહીંતર
    pde = pde_pde_coarse_new(paddr);
#endif

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, vaddr);

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા performPageTableInvocationMap(cap, cte, pde, pdSlot);
}

સ્થિર exception_t decodeARMFrameInvocation(word_t invLabel, word_t length,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    પસંદગી (invLabel) {
    સ્થિતિ ARMPageMap: {
        word_t vaddr, vtop, w_rightsMask;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_page_size_t frameSize;
        vm_attributes_t attr;

        જો (unlikely(length < 3 || current_extra_caps.excaprefs[0] == ખાલી)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                seL4_TruncatedMessage;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, buffer);
        w_rightsMask = getSyscallArg(1, buffer);
        attr = vmAttributesFromWord(getSyscallArg(2, buffer));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        frameSize = generic_frame_cap_get_capFSize(cap);
        capVMRights = generic_frame_cap_get_capFVMRights(cap);

        જો (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(
                         pdCap));
        asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

        જો (generic_frame_cap_get_capFIsMapped(cap)) {
            જો (generic_frame_cap_get_capFMappedASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_સંખ્યા = 1;

                પાછા EXCEPTION_SYSCALL_ERROR;
            }

            જો (generic_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_સંખ્યા = 0;

                પાછા EXCEPTION_SYSCALL_ERROR;
            }
        } નહીંતર {
            vtop = vaddr + BIT(pageBitsForSize(frameSize)) - 1;

            જો (unlikely(vtop >= USER_ટોચ)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_ટોચ);
                current_syscall_error.type =
                    seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_સંખ્યા = 0;

                પાછા EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            findPDForASID_ret_t find_ret;

            find_ret = findPDForASID(asid);
            જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource =
                    false;

                પાછા EXCEPTION_SYSCALL_ERROR;
            }

            જો (unlikely(find_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_સંખ્યા = 1;

                પાછા EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            maskVMRights(capVMRights, rightsFromWord(w_rightsMask));

        જો (unlikely(!checkVPAlignment(frameSize, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((શૂન્ય *)
                                   generic_frame_cap_get_capFBasePtr(cap));

        cap = generic_frame_cap_set_capFMappedAddress(cap, asid,
                                                      vaddr);
        જો (frameSize == ARMSmallPage || frameSize == ARMLargePage) {
            create_mappings_pte_return_t map_ret;
            map_ret = createSafeMappingEntries_PTE(capFBasePtr, vaddr,
                                                   frameSize, vmRights,
                                                   attr, pd);
            જો (unlikely(map_ret.status != EXCEPTION_NONE)) {
#ifdef CONFIG_PRINTING
                જો (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#endif
                પાછા map_ret.status;
            }

            setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
            પાછા performPageInvocationMapPTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } નહીંતર {
            create_mappings_pde_return_t map_ret;
            map_ret = createSafeMappingEntries_PDE(capFBasePtr, vaddr,
                                                   frameSize, vmRights,
                                                   attr, pd);
            જો (unlikely(map_ret.status != EXCEPTION_NONE)) {
#ifdef CONFIG_PRINTING
                જો (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#endif
                પાછા map_ret.status;
            }

            setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
            પાછા performPageInvocationMapPDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    સ્થિતિ ARMPageUnmap: {
#ifdef CONFIG_TK1_SMMU
        જો (isIOSpaceFrameCap(cap)) {
            setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
            પાછા performPageInvocationUnmapIO(cap, cte);
        } નહીંતર
#endif
        {
            setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
            પાછા performPageInvocationUnmap(cap, cte);
        }
    }

#ifdef CONFIG_TK1_SMMU
    સ્થિતિ ARMPageMapIO: {
        પાછા decodeARMIOMapInvocation(invLabel, length, cte, cap, buffer);
    }
#endif

    સ્થિતિ ARMPageClean_Data:
    સ્થિતિ ARMPageInvalidate_Data:
    સ્થિતિ ARMPageCleanInvalidate_Data:
    સ્થિતિ ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        findPDForASID_ret_t pd;
        vptr_t start, end;
        paddr_t pstart;
        word_t page_size;
        word_t page_base;

        જો (length < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_frame_cap_get_capFMappedASID(cap);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_frame_cap_get_capFBasePtr(cap);
#નહીંતર
        vaddr = generic_frame_cap_get_capFMappedAddress(cap);
#endif

        જો (unlikely(!generic_frame_cap_get_capFIsMapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        pd = findPDForASID(asid);
        જો (unlikely(pd.status != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
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
        page_size = 1 << pageBitsForSize(generic_frame_cap_get_capFSize(cap));
        page_base = addrFromPPtr((શૂન્ય *)generic_frame_cap_get_capFBasePtr(cap));

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        જો (unlikely(!resolve_ret.valid ||
                     resolve_ret.frameSize != generic_frame_cap_get_capFSize(cap) ||
                     resolve_ret.frameBase != page_base)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
#endif

        જો (start >= page_size || end > page_size) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = page_base + start;
        start += vaddr;
        end += vaddr;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        જો (pstart < physBase() || pstart > PADDR_ટોચ || (end - start) > PADDR_ટોચ - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageFlush(invLabel, pd.pd, asid, start, end - 1, pstart);
    }

    સ્થિતિ ARMPageGetAddress: {


        /* Check that there are enough message registers */
        પ્રતિજ્ઞા(n_સંદેશ_Registers >= 1);

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageGetAddress((શૂન્ય *)generic_frame_cap_get_capFBasePtr(cap), call);
    }

    મૂળભૂત:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t length, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_page_directory_cap:
        પાછા decodeARMPageDirectoryInvocation(invLabel, length, cptr, cte,
                                                cap, buffer);

    સ્થિતિ cap_page_table_cap:
        પાછા decodeARMPageTableInvocation(invLabel, length, cte, cap, buffer);

    સ્થિતિ cap_small_frame_cap:
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
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(length < 2 || current_extra_caps.excaprefs[0] == ખાલી
                     || current_extra_caps.excaprefs[1] == ખાલી)) {
            userError("ASIDControlMakePool: Truncated message.");
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
                     cap_untyped_cap_get_capBlockSize(untyped) !=
                     seL4_ASIDPoolBits || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        જો (unlikely(status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            પાછા status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(મૂળ, index, depth);
        જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            પાછા lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        જો (unlikely(status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            પાછા status;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performASIDControlInvocation(frame, destSlot,
                                            parentSlot, asid_base);
    }

    સ્થિતિ cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pdCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        જો (unlikely(invLabel != ARMASIDPoolAssign)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(current_extra_caps.excaprefs[0] == ખાલી)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        pdCapSlot = current_extra_caps.excaprefs[0];
        pdCap = pdCapSlot->cap;

        જો (unlikely(
                cap_get_capType(pdCap) != cap_page_directory_cap ||
                cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ઉપલું(cap_asid_pool_cap_get_capASIDBase(cap))];
        જો (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_મૂળ_new();

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        માટે (i = 0; i < (1 << asid_નીચલું_Bits) && (asid + i == 0 || pool->array[i]); i++);

        જો (unlikely(i == 1 << asid_નીચલું_Bits)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = seL4_DeleteFirst;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performASIDPoolInvocation(asid, pool, pdCapSlot);
    }

    મૂળભૂત:
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

    frameSize = generic_frame_cap_get_capFSize(lu_ret.cap);

    જો (frameSize != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = generic_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((શૂન્ય *) frame_pptr);

    માટે (પૂર્ણાંક idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
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

    પાછા EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_DEBUG_BUILD
શૂન્ય kernelPrefetchAbort(word_t pc, word_t lr) VISIBLE;
શૂન્ય kernelDataAbort(word_t pc) VISIBLE;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

શૂન્ય kernelUndefinedInstruction(word_t pc) VISIBLE;

શૂન્ય kernelPrefetchAbort(word_t pc, word_t lr)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    printf("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

શૂન્ય kernelDataAbort(word_t pc)
{
    printf("\n\nKERNEL DATA ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HDFAR: 0x%"SEL4_PRIx_word" HSR: 0x%"SEL4_PRIx_word"\n",
           getHDFAR(), getHSR());
    halt();
}

શૂન્ય kernelUndefinedInstruction(word_t pc)
{
    printf("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    halt();
}

#નહીંતર /* CONFIG_ARM_HYPERVISOR_SUPPORT */

શૂન્ય kernelPrefetchAbort(word_t pc, word_t lr)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("IFSR: 0x%"SEL4_PRIx_word"\n", getIFSR());
    printf("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

શૂન્ય kernelDataAbort(word_t pc)
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
પ્રકારવ્યાખ્યા સંરચના readWordFromVSpace_ret {
    exception_t status;
    word_t કિંમત;
} readWordFromVSpace_ret_t;

સ્થિર readWordFromVSpace_ret_t readWordFromVSpace(pde_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    lookupPTSlot_ret_t ptSlot;
    pde_t *pdSlot;
    paddr_t paddr;
    word_t offset;
    pptr_t kernel_vaddr;
    word_t *કિંમત;

    pdSlot = lookupPDSlot(pd, vaddr);
    જો (pde_ptr_get_pdeType(pdSlot) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_address(pdSlot);
        offset = vaddr & MASK(ARMSectionBits);
    } નહીંતર {
        ptSlot = lookupPTSlot(pd, vaddr);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        જો (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_address(ptSlot.ptSlot);
            જો (pte_pte_small_ptr_get_contiguous_hint(ptSlot.ptSlot)) {
                offset = vaddr & MASK(ARMLargePageBits);
            } નહીંતર {
                offset = vaddr & MASK(ARMSmallPageBits);
            }
#નહીંતર
        જો (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_address(ptSlot.ptSlot);
            offset = vaddr & MASK(ARMSmallPageBits);
        } નહીંતર જો (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_address(ptSlot.ptSlot);
            offset = vaddr & MASK(ARMLargePageBits);
#endif
        } નહીંતર {
            ret.status = EXCEPTION_LOOKUP_FAULT;
            પાછા ret;
        }
    }


    kernel_vaddr = (word_t)paddr_to_pptr(paddr);
    કિંમત = (word_t *)(kernel_vaddr + offset);
    ret.status = EXCEPTION_NONE;
    ret.કિંમત = *કિંમત;
    પાછા ret;
}

શૂન્ય Arch_user_ઢગલો_Trace(tcb_t *tptr)
{
    cap_t thread_મૂળ;
    pde_t *pd;
    word_t sp;
    પૂર્ણાંક i;

    thread_મૂળ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    જો (cap_get_capType(thread_મૂળ) != cap_page_directory_cap) {
        printf("Invalid vspace\n");
        પાછા;
    }

    pd = (pde_t *)pptr_of_cap(thread_મૂળ);

    sp = getRegister(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    જો (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        printf("SP not aligned\n");
        પાછા;
    }

    માટે (i = 0; i < CONFIG_USER_ઢગલો_TRACE_LENGTH; i++) {
        word_t address = sp + (i * કદ(word_t));
        readWordFromVSpace_ret_t result;
        result = readWordFromVSpace(pd, address);
        જો (result.status == EXCEPTION_NONE) {
            printf("0x%lx: 0x%lx\n", (long)address, (long)result.કિંમત);
        } નહીંતર {
            printf("0x%lx: INVALID\n", (long)address);
        }
    }
}
#endif
