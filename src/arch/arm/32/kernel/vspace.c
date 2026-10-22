/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <benchmark/benchmark.h>
#ضمن <api/failures.h>
#ضمن <api/syscall.h>
#ضمن <kernel/boot.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/stack.h>
#ضمن <machine/io.h>
#ضمن <machine/debug.h>
#ضمن <model/statedata.h>
#ضمن <object/cnode.h>
#ضمن <object/untyped.h>
#ضمن <arch/api/invocation.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <linker.h>
#ضمن <plat/machine/hardware.h>
#ضمن <armv/context_switch.h>
#ضمن <arch/object/iospace.h>
#ضمن <arch/object/vcpu.h>
#ضمن <arch/machine/tlb.h>

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ضمن <benchmark/benchmark_track.h>
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

ساكن فراغ setGlobalPD(فراغ);

بنية resolve_ret {
    paddr_t frameBase;
    vm_page_size_t frameSize;
    bool_t valid;
};
تعريفنوع بنية resolve_ret resolve_ret_t;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
ساكن bool_t PURE pteCheckIfMapped(pte_t *pte);
ساكن bool_t PURE pdeCheckIfMapped(pde_t *pde);

ساكن word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    بدل (vm_rights) {
    حالة VMNoAccess:
        أرجع 0;

    حالة VMKernelOnly:
        أرجع 1;

    حالة VMReadOnly:
        أرجع 2;

    حالة VMReadWrite:
        أرجع 3;

    افتراضي:
        fail("Invalid VM rights");
    }
}

#وإلا
/* AP encoding slightly different. AP only used for kernel mappings which are fixed after boot time */
BOOT_CODE
ساكن word_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    بدل (vm_rights) {
    حالة VMKernelOnly:
        أرجع 0;
    حالة VMReadWrite:
        أرجع 1;
    حالة VMNoAccess:
        /* RO at PL1 only */
        أرجع 2;
    حالة VMReadOnly:
        أرجع 3;
    افتراضي:
        fail("Invalid VM rights");
    }
}

ساكن word_t CONST HAPFromVMRights(vm_rights_t vm_rights)
{
    بدل (vm_rights) {
    حالة VMKernelOnly:
    حالة VMNoAccess:
        أرجع 0;
    حالة VMReadOnly:
        أرجع 1;
    /*
    case VMWriteOnly:
        return 2;
    */
    حالة VMReadWrite:
        أرجع 3;
    افتراضي:
        fail("Invalid VM rights");
    }
}

#endif

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    إذا (vm_rights == VMNoAccess) {
        أرجع VMNoAccess;
    }
    إذا (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        أرجع VMReadOnly;
    }
    إذا (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        إذا (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            أرجع VMReadOnly;
        } وإلا {
            أرجع VMReadWrite;
        }
    }
    إذا (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    أرجع VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_CODE فراغ map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_attributes_t attributes)
{
    word_t idx = (vaddr & MASK(pageBitsForSize(ARMSection))) >> pageBitsForSize(ARMSmallPage);

    أكد(vaddr >= PPTR_لووتکە); /* vaddr lies in the region the global PT covers */
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t tex;
    إذا (vm_attributes_get_armPageCacheable(attributes)) {
        tex = 5; /* TEX = 0b1(Cached)01(Outer Write Allocate) */
    } وإلا {
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
#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
BOOT_CODE فراغ map_kernel_window(فراغ)
{
    paddr_t  phys;
    word_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = physBase();
    idx = PPTR_BASE >> pageBitsForSize(ARMSection);

    طالما (idx < BIT(PD_INDEX_BITS) - SECTIONS_PER_SUPER_SECTION) {
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
        لكل (idx2 = idx; idx2 < idx + SECTIONS_PER_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += BIT(pageBitsForSize(ARMSuperSection));
        idx += SECTIONS_PER_SUPER_SECTION;
    }

    طالما (idx < (PPTR_لووتکە >> pageBitsForSize(ARMSection))) {
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
    أكد(phys == PADDR_لووتکە);

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

#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_CODE فراغ map_kernel_window(فراغ)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t UNUSED pteS2;

    /* Initialise PGD */
    لكل (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    لكل (idx = 0; idx < (USER_لووتکە - 0xC0000000) >> (PT_INDEX_BITS + PAGE_BITS); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = physBase();
    لكل (; idx < BIT(PT_INDEX_BITS) - 1; idx++) {
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
    لكل (idx = 0; idx < 256; idx++) {
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

ساكن BOOT_CODE فراغ map_it_frame_cap(cap_t pd_cap, cap_t frame_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *targetSlot;
    pde_t *pd    = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    فراغ  *frame = (فراغ *)generic_frame_cap_get_capFBasePtr(frame_cap);
    vptr_t vptr  = generic_frame_cap_get_capFMappedAddress(frame_cap);

    أكد(generic_frame_cap_get_capFMappedASID(frame_cap) != 0);

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
#وإلا
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

ساكن BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    إذا (use_large)
        أرجع
            cap_frame_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_نزم(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_بەرز(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    وإلا
        أرجع
            cap_small_frame_cap_new(
                ASID_نزم(asid),                /* capFMappedASIDLow  */
                wordFromVMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#ifdef CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#endif
                ASID_بەرز(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

ساكن BOOT_CODE فراغ map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);
    pde_t *targetSlot = pd + (vptr >> pageBitsForSize(ARMSection));

    أكد(cap_page_table_cap_get_capPTIsMapped(pt_cap));

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    *targetSlot = pde_pde_coarse_new(
                      addrFromPPtr(pt), /* address */
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#وإلا
    *targetSlot = pde_pde_coarse_new(addrFromPPtr(pt));
#endif
}

/* Create a page table for the initial thread */

ساكن BOOT_CODE cap_t create_it_page_table_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    إذا (asid != asidInvalid) {
        map_it_pt_cap(pd, cap);
    }
    أرجع cap;
}

BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    أرجع get_n_paging(it_v_reg, PT_INDEX_BITS + PAGE_BITS);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t create_it_address_space(cap_t ڕەگ_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << seL4_PageDirBits) - 1,
                        addrFromPPtr((فراغ *)rootserver.vspace));
    cap_t pd_cap =
        cap_page_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_of_cap(ڕەگ_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    لكل (pt_vptr = ROUND_DOWN(it_v_reg.start, PT_INDEX_BITS + PAGE_BITS);
         pt_vptr < it_v_reg.end;
         pt_vptr += BIT(PT_INDEX_BITS + PAGE_BITS)) {
        إذا (!provide_cap(ڕەگ_cnode_cap,
                         create_it_page_table_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    أرجع pd_cap;
}

BOOT_CODE cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_frame_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large);
    map_it_frame_cap(pd_cap, cap, executable);
    أرجع cap;
}

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_CODE فراغ activate_kernel_vspace(فراغ)
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

#وإلا

BOOT_CODE فراغ activate_kernel_vspace(فراغ)
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
#إذا 0 /* Can't lock entries on A15 */
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

BOOT_CODE فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[ASID_نزم(IT_ASID)] = PDE_PTR(pptr_of_cap(it_pd_cap));
    armKSASIDTable[ASID_بەرز(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

findPDForASID_ret_t findPDForASID(asid_t asid)
{
    findPDForASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_بەرز(asid)];
    إذا (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_ڕەگ_new();

        ret.pd = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    pd = poolPtr->array[ASID_نزم(asid)];
    إذا (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_ڕەگ_new();

        ret.pd = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.pd = pd;
    ret.status = EXCEPTION_NONE;
    أرجع ret;
}

word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_bufferPtr;
    cap_t bufferCap;
    vm_rights_t vm_rights;

    w_bufferPtr = thread->tcbIPCBuffer;
    bufferCap = TCB_PTR_CTE_PTR(thread, tcbBuffer)->cap;

    إذا (unlikely(cap_get_capType(bufferCap) != cap_small_frame_cap &&
                 cap_get_capType(bufferCap) != cap_frame_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(generic_frame_cap_get_capFIsDevice(bufferCap))) {
        أرجع عدم;
    }

    vm_rights = generic_frame_cap_get_capFVMRights(bufferCap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr;
        unsigned صحيح pageBits;

        basePtr = generic_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(generic_frame_cap_get_capFSize(bufferCap));
        أرجع (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } وإلا {
        أرجع عدم;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (unlikely(cap_get_capType(cap) != cap_small_frame_cap &&
                 cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(generic_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(vptr & MASK(seL4_IPCBufferSizeBits))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    unsigned صحيح pdIndex;

    pdIndex = vptr >> (PAGE_BITS + PT_INDEX_BITS);
    أرجع pd + pdIndex;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pdSlot;

    pdSlot = lookupPDSlot(pd, vptr);

    إذا (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_INDEX_BITS + PAGE_BITS);

        ret.ptSlot = عدم;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pte_t *pt, *ptSlot;
        unsigned صحيح ptIndex;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pdSlot));
        ptIndex = (vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS);
        ptSlot = pt + ptIndex;

        ret.ptSlot = ptSlot;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
    }
}

ساكن pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    unsigned صحيح ptIndex;

    ptIndex = (vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS);
    أرجع pt + ptIndex;
}

ساكن ثابت resolve_ret_t default_resolve_ret_t;

ساكن resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    بدل (pde_ptr_get_pdeType(pde)) {
    حالة pde_pde_section:
        ret.frameBase = pde_pde_section_ptr_get_address(pde);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (pde_pde_section_ptr_get_size(pde)) {
            ret.frameSize = ARMSuperSection;
        } وإلا {
            ret.frameSize = ARMSection;
        }
#وإلا
        إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.frameBase &= ~MASK(pageBitsForSize(ARMSuperSection));
            ret.frameSize = ARMSuperSection;
        } وإلا {
            ret.frameSize = ARMSection;
        }
#endif
        أرجع ret;

    حالة pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_address(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small:
            ret.frameBase = pte_pte_small_ptr_get_address(pte);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.frameBase &= ~MASK(pageBitsForSize(ARMLargePage));
                ret.frameSize = ARMLargePage;
            } وإلا {
                ret.frameSize = ARMSmallPage;
            }
#وإلا
            ret.frameSize = ARMSmallPage;
#endif
            أرجع ret;

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large:
            ret.frameBase = pte_pte_large_ptr_get_address(pte);
            ret.frameSize = ARMLargePage;
            أرجع ret;
#endif
        افتراضي:
            اكسر;
        }
        اكسر;
    }
    }

    ret.valid = false;
    أرجع ret;
}

ساكن pte_t CONST makeUserPTE(vm_page_size_t page_size, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap;

    ap = APFromVMRights(vm_rights);

    بدل (page_size) {
    حالة ARMSmallPage: {
        إذا (cacheable) {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    nonexecutable);
        } وإلا {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    0, /* TEX = 0b000, strongly-ordered. */
                                    ap,
                                    0, 0,
                                    nonexecutable);
        }
        اكسر;
    }

    حالة ARMLargePage: {
        إذا (cacheable) {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    1 /* reserved */);
        } وإلا {
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
        اكسر;
    }

    افتراضي:
        fail("Invalid PTE frame type");
    }

#وإلا

    word_t hap;

    hap = HAPFromVMRights(vm_rights);

    بدل (page_size) {
    حالة ARMSmallPage: {
        إذا (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_CACHEABLE /* Cacheable */);
        } وإلا {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        اكسر;
    }

    حالة ARMLargePage: {
        إذا (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_CACHEABLE  /* Cacheable */);
        } وإلا {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        اكسر;
    }
    افتراضي:
        fail("Invalid PTE frame type");
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    أرجع pte;
}

ساكن pde_t CONST makeUserPDE(vm_page_size_t page_size, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, word_t domain,
                               vm_rights_t vm_rights)
{
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t ap, size2;

    ap = APFromVMRights(vm_rights);
#وإلا
    word_t hap, size2;

    (فراغ)domain;
    hap = HAPFromVMRights(vm_rights);
#endif

    بدل (page_size) {
    حالة ARMSection:
        size2 = 0;
        اكسر;

    حالة ARMSuperSection:
        size2 = 1;
        اكسر;

    افتراضي:
        fail("Invalid PDE frame type");
    }

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    إذا (cacheable) {
        أرجع pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                   0, /* APX = 0, privileged full access */
                                   5, /* TEX = 0b101, outer write-back, write-allocate */
                                   ap, parity, domain, nonexecutable,
                                   0, 1 /* Inner write-back, write-allocate */);
    } وإلا {
        أرجع pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   1, /* shared */
                                   0, /* APX = 0, privileged full access */
                                   0, /* TEX = 0b000, strongly-ordered */
                                   ap, parity, domain, nonexecutable,
                                   0, 0);
    }
#وإلا
    إذا (cacheable) {
        أرجع pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_CACHEABLE /* Cacheable */);
    } وإلا {
        أرجع pde_pde_section_new(
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

bool_t CONST is_Valid_VTable_ڕەگ(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_page_directory_cap &&
           cap_page_directory_cap_get_capPDIsMapped(cap);
}

bool_t CONST isIOSpaceFrameCap(cap_t cap)
{
#ifdef CONFIG_TK1_SMMU
    أرجع cap_get_capType(cap) == cap_small_frame_cap && cap_small_frame_cap_get_capFIsIOSpace(cap);
#وإلا
    أرجع false;
#endif
}

/** Switch to the empty PD on the reserved HW ASID */
ساكن فراغ setGlobalPD(فراغ)
{
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    writeContextIDAndPD(hwASIDReserved, addrFromKPPtr(armUSGlobalPD));
#وإلا
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

فراغ setVMRoot(tcb_t *tcb)
{
    cap_t thread_ڕەگ;
    asid_t asid;
    pde_t *pd;
    findPDForASID_ret_t find_ret;

    thread_ڕەگ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_ڕەگ) != cap_page_directory_cap ||
        !cap_page_directory_cap_get_capPDIsMapped(thread_ڕەگ)) {
        setGlobalPD();
        أرجع;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_ڕەگ));
    asid = cap_page_directory_cap_get_capPDMappedASID(thread_ڕەگ);
    find_ret = findPDForASID(asid);
    إذا (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.pd != pd)) {
        setGlobalPD();
        أرجع;
    }

    armv_contextSwitch(pd, asid);
}

ساكن bool_t setVMRootForFlush(pde_t *pd, asid_t asid)
{
    cap_t thread_ڕەگ;

    thread_ڕەگ = TCB_PTR_CTE_PTR(گرێ_STATE(ksCurThread), tcbVTable)->cap;

    إذا (cap_get_capType(thread_ڕەگ) == cap_page_directory_cap &&
        cap_page_directory_cap_get_capPDIsMapped(thread_ڕەگ) &&
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(thread_ڕەگ)) == pd) {
        أرجع false;
    }

    armv_contextSwitch(pd, asid);

    أرجع true;
}

pde_t *pageTableMapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    findPDForASID_ret_t find_ret;
    pde_t pde;
    unsigned صحيح pdIndex;

    find_ret = findPDForASID(asid);
    إذا (unlikely(find_ret.status != EXCEPTION_NONE)) {
        أرجع عدم;
    }

    pdIndex = vaddr >> (PAGE_BITS + PT_INDEX_BITS);
    pde = find_ret.pd[pdIndex];

    إذا (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_address(pde)) == pt)) {
        أرجع find_ret.pd;
    } وإلا {
        أرجع عدم;
    }
}

ساكن فراغ invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بەرز(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_نزم(asid)];
    أكد(pd);

    pd[PD_ASID_SLOT] = pde_pde_invalid_new(0, false);
}

ساكن pde_t PURE loadHWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بەرز(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_نزم(asid)];
    أكد(pd);

    أرجع pd[PD_ASID_SLOT];
}

ساكن فراغ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بەرز(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_نزم(asid)];
    أكد(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_SLOT] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t findFreeHWASID(فراغ)
{
    word_t hw_asid_offset;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    لكل (hw_asid_offset = 0;
         hw_asid_offset <= (word_t)((hw_asid_t) - 1);
         hw_asid_offset ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_offset);
        إذا (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            أرجع hw_asid;
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
    إذا (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    أرجع hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);
    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } وإلا {
        hw_asid_t new_hw_asid;

        new_hw_asid = findFreeHWASID();
        storeHWASID(asid, new_hw_asid);
        أرجع new_hw_asid;
    }
}

ساكن فراغ invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);
    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

فراغ unmapPageTable(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pdSlot;
    unsigned صحيح pdIndex;

    pd = pageTableMapped(asid, vaddr, pt);

    إذا (likely(pd != عدم)) {
        pdIndex = vaddr >> (PT_INDEX_BITS + PAGE_BITS);
        pdSlot = pd + pdIndex;

        *pdSlot = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));
        flushTable(pd, asid, vaddr, pt);
    }
}

فراغ copyGlobalMappings(pde_t *newPD)
{
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    word_t i;
    pde_t *global_pd = armKSGlobalPD;

    لكل (i = PPTR_BASE >> ARMSectionBits; i < BIT(PD_INDEX_BITS); i++) {
        إذا (i != PD_ASID_SLOT) {
            newPD[i] = global_pd[i];
        }
    }
#endif
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    بدل (vm_faultType) {
    حالة ARMDataAbort: {
        word_t addr, fault;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (addressTranslateS1(addr) & ~MASK(PAGE_BITS)) | (addr & MASK(PAGE_BITS));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#وإلا
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
        إذا (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(0);
            أرجع EXCEPTION_FAULT;
        }
#endif
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        أرجع EXCEPTION_FAULT;
    }

    حالة ARMPrefetchAbort: {
        word_t pc, fault;

        pc = getRestartPC(thread);

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (addressTranslateS1(pc) & ~MASK(PAGE_BITS)) | (pc & MASK(PAGE_BITS));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#وإلا
        fault = getIFSR();
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
        إذا (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            إذا (seL4_Fault_DebugException_get_exceptionReason(current_fault) == seL4_SingleStep
                && !singleStepFaultCounterReady(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                أرجع EXCEPTION_NONE;
            }
            أرجع EXCEPTION_FAULT;
        }
#endif
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        أرجع EXCEPTION_FAULT;
    }

    افتراضي:
        fail("Invalid VM fault type");
    }
}

فراغ deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    unsigned صحيح offset;

    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_base & MASK(asid_نزم_Bits)) == 0);

    إذا (armKSASIDTable[ASID_بەرز(asid_base)] == pool) {
        لكل (offset = 0; offset < BIT(asid_نزم_Bits); offset++) {
            إذا (pool->array[offset]) {
                flushSpace(asid_base + offset);
                invalidateASIDEntry(asid_base + offset);
            }
        }
        armKSASIDTable[ASID_بەرز(asid_base)] = عدم;
        setVMRoot(گرێ_STATE(ksCurThread));
    }
}

فراغ deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_بەرز(asid)];

    إذا (poolPtr != عدم && poolPtr->array[ASID_نزم(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_نزم(asid)] = عدم;
        setVMRoot(گرێ_STATE(ksCurThread));
    }
}

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
ساكن pte_t pte_pte_invalid_new(فراغ)
{
    /* Invalid as every PTE must have bit 0 set (large PTE) or bit 1 set (small
     * PTE). 0 == 'translation fault' in ARM parlance.
     */
    أرجع (pte_t) {
        {
            0
        }
    };
}
#endif

فراغ unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    findPDForASID_ret_t find_ret;
    paddr_t addr = addrFromPPtr(pptr);

    find_ret = findPDForASID(asid);
    إذا (unlikely(find_ret.status != EXCEPTION_NONE)) {
        أرجع;
    }

    بدل (page_size) {
    حالة ARMSmallPage: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        إذا (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            أرجع;
        }

        إذا (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            أرجع;
        }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 0)) {
            أرجع;
        }
#endif
        إذا (unlikely(pte_pte_small_ptr_get_address(lu_ret.ptSlot) != addr)) {
            أرجع;
        }

        *(lu_ret.ptSlot) = pte_pte_invalid_new();
        cleanByVA_PoU((word_t)lu_ret.ptSlot, addrFromPPtr(lu_ret.ptSlot));

        اكسر;
    }

    حالة ARMLargePage: {
        lookupPTSlot_ret_t lu_ret;
        word_t i;

        lu_ret = lookupPTSlot(find_ret.pd, vptr);
        إذا (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            أرجع;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_large)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_large_ptr_get_address(lu_ret.ptSlot) != addr)) {
            أرجع;
        }
#وإلا
        إذا (unlikely(pte_ptr_get_pteType(lu_ret.ptSlot) != pte_pte_small)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.ptSlot) != 1)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_small_ptr_get_address(lu_ret.ptSlot) != addr)) {
            أرجع;
        }
#endif

        لكل (i = 0; i < PAGES_PER_LARGE_PAGE; i++) {
            lu_ret.ptSlot[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((word_t)&lu_ret.ptSlot[0],
                            LAST_BYTE_PTE(lu_ret.ptSlot, PAGES_PER_LARGE_PAGE),
                            addrFromPPtr(&lu_ret.ptSlot[0]));

        اكسر;
    }

    حالة ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(find_ret.pd, vptr);

        إذا (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            أرجع;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pde_pde_section_ptr_get_size(pd) != 0)) {
#وإلا
        إذا (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#endif
            أرجع;
        }
        إذا (unlikely(pde_pde_section_ptr_get_address(pd) != addr)) {
            أرجع;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((word_t)pd, addrFromPPtr(pd));

        اكسر;
    }

    حالة ARMSuperSection: {
        pde_t *pd;
        word_t i;

        pd = lookupPDSlot(find_ret.pd, vptr);

        إذا (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            أرجع;
        }
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pde_pde_section_ptr_get_size(pd) != 1)) {
#وإلا
        إذا (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#endif
            أرجع;
        }
        إذا (unlikely(pde_pde_section_ptr_get_address(pd) != addr)) {
            أرجع;
        }

        لكل (i = 0; i < SECTIONS_PER_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((word_t)&pd[0], LAST_BYTE_PDE(pd, SECTIONS_PER_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        اكسر;
    }

    افتراضي:
        fail("Invalid ARM page type");
        اكسر;
    }

    /* Flush the page now that the mapping has been updated */
    flushPage(page_size, find_ret.pd, asid, vptr);
}

فراغ flushPage(vm_page_size_t page_size, pde_t *pd, asid_t asid, word_t vptr)
{
    pde_t stored_hw_asid;
    word_t base_addr;
    bool_t ڕەگ_switched;

    أكد((vptr & MASK(pageBitsForSize(page_size))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ڕەگ_switched = setVMRootForFlush(pd, asid);
    stored_hw_asid = loadHWASID(asid);

    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        base_addr = vptr & ~MASK(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(base_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        إذا (ڕەگ_switched) {
            setVMRoot(گرێ_STATE(ksCurThread));
        }
    }
}

فراغ flushTable(pde_t *pd, asid_t asid, word_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t ڕەگ_switched;

    أكد((vptr & MASK(PT_INDEX_BITS + ARMSmallPageBits)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ڕەگ_switched = setVMRootForFlush(pd, asid);
    stored_hw_asid = loadHWASID(asid);

    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        إذا (ڕەگ_switched) {
            setVMRoot(گرێ_STATE(ksCurThread));
        }
    }
}

فراغ flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);

    /* Clean the entire data cache, to guarantee that any VAs mapped
     * in the deleted space are clean (because we can't clean by VA after
     * deleting the space) */
    cleanCaches_PoU();

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    إذا (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

فراغ invalidateTLBByASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = loadHWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    إذا (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ساكن مضمن bool_t CONST checkVPAlignment(vm_page_size_t sz, word_t w)
{
    أرجع (w & MASK(pageBitsForSize(sz))) == 0;
}

بنية create_mappings_pte_return {
    exception_t status;
    pte_t pte;
    pte_range_t pte_entries;
};
تعريفنوع بنية create_mappings_pte_return create_mappings_pte_return_t;

بنية create_mappings_pde_return {
    exception_t status;
    pde_t pde;
    pde_range_t pde_entries;
};
تعريفنوع بنية create_mappings_pde_return create_mappings_pde_return_t;

ساكن create_mappings_pte_return_t
createSafeMappingEntries_PTE
(paddr_t base, word_t vaddr, vm_page_size_t frameSize,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    word_t i;

    بدل (frameSize) {

    حالة ARMSmallPage:

        ret.pte_entries.base = عدم; /* to avoid uninitialised warning */
        ret.pte_entries.length = 1;

        ret.pte = makeUserPTE(ARMSmallPage, base,
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        إذا (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            أرجع ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_ptr_get_pteType(ret.pte_entries.base) ==
                     pte_pte_large)) {
#وإلا
        إذا (unlikely(pte_ptr_get_pteType(ret.pte_entries.base) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.base))) {
#endif
            current_syscall_error.type =
                seL4_DeleteFirst;

            ret.status = EXCEPTION_SYSCALL_ERROR;
            أرجع ret;
        }

        ret.status = EXCEPTION_NONE;
        أرجع ret;

    حالة ARMLargePage:

        ret.pte_entries.base = عدم; /* to avoid uninitialised warning */
        ret.pte_entries.length = PAGES_PER_LARGE_PAGE;

        ret.pte = makeUserPTE(ARMLargePage, base,
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        إذا (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource =
                false;
            ret.status = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            أرجع ret;
        }

        ret.pte_entries.base = lu_ret.ptSlot;

        لكل (i = 0; i < PAGES_PER_LARGE_PAGE; i++) {
            إذا (unlikely(pte_get_pteType(ret.pte_entries.base[i]) ==
                         pte_pte_small)
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.base[i])
#endif
               ) {
                current_syscall_error.type =
                    seL4_DeleteFirst;

                ret.status = EXCEPTION_SYSCALL_ERROR;
                أرجع ret;
            }
        }

        ret.status = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        fail("Invalid or unexpected ARM page type.");

    }
}

ساكن create_mappings_pde_return_t
createSafeMappingEntries_PDE
(paddr_t base, word_t vaddr, vm_page_size_t frameSize,
 vm_rights_t vmRights, vm_attributes_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_tag_t currentPDEType;
    word_t i;

    بدل (frameSize) {

    /* PDE mappings */
    حالة ARMSection:
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
        إذا (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_size(ret.pde_entries.base) != 0))) {
#وإلا
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.base) != 0))) {
#endif
            current_syscall_error.type =
                seL4_DeleteFirst;
            ret.status = EXCEPTION_SYSCALL_ERROR;

            أرجع ret;
        }

        ret.status = EXCEPTION_NONE;
        أرجع ret;

    حالة ARMSuperSection:
        ret.pde_entries.base = lookupPDSlot(pd, vaddr);
        ret.pde_entries.length = SECTIONS_PER_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, base,
                              vm_attributes_get_armParityEnabled(attr),
                              vm_attributes_get_armPageCacheable(attr),
                              vm_attributes_get_armExecuteNever(attr),
                              0,
                              vmRights);

        لكل (i = 0; i < SECTIONS_PER_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.base[i]);
            إذا (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_size(ret.pde_entries.base[i]) != 1))) {
#وإلا
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.base[i]) != 1))) {
#endif
                current_syscall_error.type =
                    seL4_DeleteFirst;
                ret.status = EXCEPTION_SYSCALL_ERROR;

                أرجع ret;
            }
        }

        ret.status = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        fail("Invalid or unexpected ARM page type.");

    }
}

ساكن مضمن vptr_t pageBase(vptr_t vaddr, vm_page_size_t size)
{
    أرجع vaddr & ~MASK(pageBitsForSize(size));
}

ساكن bool_t PURE pteCheckIfMapped(pte_t *pte)
{
    أرجع pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

ساكن bool_t PURE pdeCheckIfMapped(pde_t *pde)
{
    أرجع pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

ساكن فراغ doFlush(صحيح invLabel, vptr_t start, vptr_t end, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    إذا (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        end = (vptr_t)paddr_to_pptr(pstart) + (end - start);
        start = (vptr_t)paddr_to_pptr(pstart);
    }
    بدل (invLabel) {
    حالة ARMPDClean_Data:
    حالة ARMPageClean_Data:
        cleanCacheRange_RAM(start, end, pstart);
        اكسر;
    حالة ARMPDInvalidate_Data:
    حالة ARMPageInvalidate_Data:
        invalidateCacheRange_RAM(start, end, pstart);
        اكسر;
    حالة ARMPDCleanInvalidate_Data:
    حالة ARMPageCleanInvalidate_Data:
        cleanInvalidateCacheRange_RAM(start, end, pstart);
        اكسر;
    حالة ARMPDUnify_Instruction:
    حالة ARMPageUnify_Instruction:
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
        اكسر;
    افتراضي:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

ساكن exception_t performPDFlush(صحيح invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                  vptr_t end, paddr_t pstart)
{
    bool_t ڕەگ_switched;

    /* Flush if given a non zero range */
    إذا (start < end) {
        ڕەگ_switched = setVMRootForFlush(pd, asid);

        doFlush(invLabel, start, end, pstart);

        إذا (ڕەگ_switched) {
            setVMRoot(گرێ_STATE(ksCurThread));
        }
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                                 pde_t pde, pde_t *pdSlot)
{
    ctSlot->cap = cap;
    *pdSlot = pde;
    cleanByVA_PoU((word_t)pdSlot, addrFromPPtr(pdSlot));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    إذا (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt);
        clearMemory_PT((فراغ *)pt, cap_get_capSizeBits(cap));
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performPageInvocationMapPTE(asid_t asid, cap_t cap, cte_t *ctSlot, pte_t pte,
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
    لكل (i = 0; i < pte_entries.length; i++) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_set_address(pte, base_address + i * BIT(pageBitsForSize(ARMSmallPage)));
#endif
        pte_entries.base[i] = pte;
    }
    cleanCacheRange_PoU((word_t)pte_entries.base,
                        LAST_BYTE_PTE(pte_entries.base, pte_entries.length),
                        addrFromPPtr(pte_entries.base));
    إذا (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performPageInvocationMapPDE(asid_t asid, cap_t cap, cte_t *ctSlot, pde_t pde,
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
    لكل (i = 0; i < pde_entries.length; i++) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_set_address(pde, base_address + i * BIT(pageBitsForSize(ARMSection)));
#endif
        pde_entries.base[i] = pde;
    }
    cleanCacheRange_PoU((word_t)pde_entries.base,
                        LAST_BYTE_PDE(pde_entries.base, pde_entries.length),
                        addrFromPPtr(pde_entries.base));
    إذا (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    إذا (generic_frame_cap_get_capFIsMapped(cap)) {
        unmapPage(generic_frame_cap_get_capFSize(cap),
                  generic_frame_cap_get_capFMappedASID(cap),
                  generic_frame_cap_get_capFMappedAddress(cap),
                  (فراغ *)generic_frame_cap_get_capFBasePtr(cap));
    }

    generic_frame_cap_ptr_set_capFMappedAddress(&ctSlot->cap, asidInvalid, 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performPageFlush(صحيح invLabel, pde_t *pd, asid_t asid, vptr_t start,
                                    vptr_t end, paddr_t pstart)
{
    bool_t ڕەگ_switched;

    /* now we can flush. But only if we were given a non zero range */
    إذا (start < end) {
        ڕەگ_switched = setVMRootForFlush(pd, asid);

        doFlush(invLabel, start, end, pstart);

        إذا (ڕەگ_switched) {
            setVMRoot(گرێ_STATE(ksCurThread));
        }
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performPageGetAddress(فراغ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = گرێ_STATE(ksCurThread);
    إذا (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned صحيح length = setMR(thread, ipcBuffer, 0, capFBasePtr);
        setRegister(thread, پەیام_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(گرێ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pdCapSlot)
{
    cap_page_directory_cap_ptr_set_capPDMappedASID(&pdCapSlot->cap, asid);
    cap_page_directory_cap_ptr_set_capPDIsMapped(&pdCapSlot->cap, 1);
    poolPtr->array[ASID_نزم(asid)] =
        PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCapSlot->cap));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDControlInvocation(فراغ *frame, cte_t *slot,
                                                cte_t *parent, asid_t asid_base)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, 1 << ARMSmallPageBits);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_زیادکردن(cap_asid_pool_cap_new(asid_base, WORD_REF(frame)),
              parent, slot);;
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_base & MASK(asid_نزم_Bits)) == 0);
    armKSASIDTable[ASID_بەرز(asid_base)] = (asid_pool_t *)frame;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeARMPageDirectoryInvocation(word_t invLabel, word_t length,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    word_t *buffer)
{
    بدل (invLabel) {
    حالة ARMPDClean_Data:
    حالة ARMPDInvalidate_Data:
    حالة ARMPDCleanInvalidate_Data:
    حالة ARMPDUnify_Instruction: {
        vptr_t start, end;
        paddr_t pstart;
        findPDForASID_ret_t find_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        إذا (length < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, buffer);
        end =   getSyscallArg(1, buffer);

        /* Check sanity of arguments */
        إذا (end <= start) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ژمار = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        إذا (start >= USER_لووتکە || end > USER_لووتکە) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(cap_get_capType(cap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ژمار = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap));
        asid = cap_page_directory_cap_get_capPDMappedASID(cap);

        find_ret = findPDForASID(asid);
        إذا (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(find_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ژمار = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, start);

        /* Check that there's actually something there. */
        إذا (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
            أرجع EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        إذا (pageBase(start, resolve_ret.frameSize) !=
            pageBase(end - 1, resolve_ret.frameSize)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = start;
            current_syscall_error.rangeErrorMax =
                pageBase(start, resolve_ret.frameSize) +
                MASK(pageBitsForSize(resolve_ret.frameSize));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.frameBase
                 + (start & MASK(pageBitsForSize(resolve_ret.frameSize)));

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        إذا (pstart < physBase() || pstart > PADDR_لووتکە || (end - start) > PADDR_لووتکە - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performPDFlush(invLabel, pd, asid, start, end - 1, pstart);
    }

    افتراضي:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

}

ساكن exception_t decodeARMPageTableInvocation(word_t invLabel, word_t length,
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

    إذا (invLabel == ARMPageTableUnmap) {
        إذا (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = seL4_RevokeFirst;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performPageTableInvocationUnmap(cap, cte);
    }

    إذا (unlikely(invLabel != ARMPageTableMap)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(length < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ژمار = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer);
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vmAttributesFromWord(getSyscallArg(1, buffer));
#endif
    pdCap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                 !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ژمار = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(pdCap));
    asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

    إذا (unlikely(vaddr >= USER_لووتکە)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_لووتکە);
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ژمار = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    {
        findPDForASID_ret_t find_ret;

        find_ret = findPDForASID(asid);
        إذا (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(find_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ژمار = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    pdIndex = vaddr >> (PAGE_BITS + PT_INDEX_BITS);
    pdSlot = &pd[pdIndex];
    إذا (unlikely(pde_ptr_get_pdeType(pdSlot) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = seL4_DeleteFirst;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_attributes_get_armParityEnabled(attr),
              0 /* Domain */
          );
#وإلا
    pde = pde_pde_coarse_new(paddr);
#endif

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, vaddr);

    setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
    أرجع performPageTableInvocationMap(cap, cte, pde, pdSlot);
}

ساكن exception_t decodeARMFrameInvocation(word_t invLabel, word_t length,
                                            cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    بدل (invLabel) {
    حالة ARMPageMap: {
        word_t vaddr, vtop, w_rightsMask;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_page_size_t frameSize;
        vm_attributes_t attr;

        إذا (unlikely(length < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                seL4_TruncatedMessage;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, buffer);
        w_rightsMask = getSyscallArg(1, buffer);
        attr = vmAttributesFromWord(getSyscallArg(2, buffer));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        frameSize = generic_frame_cap_get_capFSize(cap);
        capVMRights = generic_frame_cap_get_capFVMRights(cap);

        إذا (unlikely(cap_get_capType(pdCap) != cap_page_directory_cap ||
                     !cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ژمار = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(
                         pdCap));
        asid = cap_page_directory_cap_get_capPDMappedASID(pdCap);

        إذا (generic_frame_cap_get_capFIsMapped(cap)) {
            إذا (generic_frame_cap_get_capFMappedASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ژمار = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (generic_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_ژمار = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            vtop = vaddr + BIT(pageBitsForSize(frameSize)) - 1;

            إذا (unlikely(vtop >= USER_لووتکە)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_لووتکە);
                current_syscall_error.type =
                    seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_ژمار = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            findPDForASID_ret_t find_ret;

            find_ret = findPDForASID(asid);
            إذا (unlikely(find_ret.status != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource =
                    false;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (unlikely(find_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ژمار = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            maskVMRights(capVMRights, rightsFromWord(w_rightsMask));

        إذا (unlikely(!checkVPAlignment(frameSize, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((فراغ *)
                                   generic_frame_cap_get_capFBasePtr(cap));

        cap = generic_frame_cap_set_capFMappedAddress(cap, asid,
                                                      vaddr);
        إذا (frameSize == ARMSmallPage || frameSize == ARMLargePage) {
            create_mappings_pte_return_t map_ret;
            map_ret = createSafeMappingEntries_PTE(capFBasePtr, vaddr,
                                                   frameSize, vmRights,
                                                   attr, pd);
            إذا (unlikely(map_ret.status != EXCEPTION_NONE)) {
#ifdef CONFIG_PRINTING
                إذا (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#endif
                أرجع map_ret.status;
            }

            setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
            أرجع performPageInvocationMapPTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } وإلا {
            create_mappings_pde_return_t map_ret;
            map_ret = createSafeMappingEntries_PDE(capFBasePtr, vaddr,
                                                   frameSize, vmRights,
                                                   attr, pd);
            إذا (unlikely(map_ret.status != EXCEPTION_NONE)) {
#ifdef CONFIG_PRINTING
                إذا (current_syscall_error.type == seL4_DeleteFirst) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#endif
                أرجع map_ret.status;
            }

            setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
            أرجع performPageInvocationMapPDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    حالة ARMPageUnmap: {
#ifdef CONFIG_TK1_SMMU
        إذا (isIOSpaceFrameCap(cap)) {
            setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
            أرجع performPageInvocationUnmapIO(cap, cte);
        } وإلا
#endif
        {
            setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
            أرجع performPageInvocationUnmap(cap, cte);
        }
    }

#ifdef CONFIG_TK1_SMMU
    حالة ARMPageMapIO: {
        أرجع decodeARMIOMapInvocation(invLabel, length, cte, cap, buffer);
    }
#endif

    حالة ARMPageClean_Data:
    حالة ARMPageInvalidate_Data:
    حالة ARMPageCleanInvalidate_Data:
    حالة ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        findPDForASID_ret_t pd;
        vptr_t start, end;
        paddr_t pstart;
        word_t page_size;
        word_t page_base;

        إذا (length < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_frame_cap_get_capFMappedASID(cap);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_frame_cap_get_capFBasePtr(cap);
#وإلا
        vaddr = generic_frame_cap_get_capFMappedAddress(cap);
#endif

        إذا (unlikely(!generic_frame_cap_get_capFIsMapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pd = findPDForASID(asid);
        إذا (unlikely(pd.status != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        start = getSyscallArg(0, buffer);
        end =   getSyscallArg(1, buffer);

        /* check that the range is sane */
        إذا (end <= start) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ژمار = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        page_size = 1 << pageBitsForSize(generic_frame_cap_get_capFSize(cap));
        page_base = addrFromPPtr((فراغ *)generic_frame_cap_get_capFBasePtr(cap));

#ifndef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        إذا (unlikely(!resolve_ret.valid ||
                     resolve_ret.frameSize != generic_frame_cap_get_capFSize(cap) ||
                     resolve_ret.frameBase != page_base)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ژمار = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#endif

        إذا (start >= page_size || end > page_size) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ژمار = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = page_base + start;
        start += vaddr;
        end += vaddr;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        إذا (pstart < physBase() || pstart > PADDR_لووتکە || (end - start) > PADDR_لووتکە - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#endif

        setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performPageFlush(invLabel, pd.pd, asid, start, end - 1, pstart);
    }

    حالة ARMPageGetAddress: {


        /* Check that there are enough message registers */
        أكد(n_پەیام_Registers >= 1);

        setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performPageGetAddress((فراغ *)generic_frame_cap_get_capFBasePtr(cap), call);
    }

    افتراضي:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(word_t invLabel, word_t length, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_page_directory_cap:
        أرجع decodeARMPageDirectoryInvocation(invLabel, length, cptr, cte,
                                                cap, buffer);

    حالة cap_page_table_cap:
        أرجع decodeARMPageTableInvocation(invLabel, length, cte, cap, buffer);

    حالة cap_small_frame_cap:
    حالة cap_frame_cap:
        أرجع decodeARMFrameInvocation(invLabel, length, cte, cap, call, buffer);

    حالة cap_asid_control_cap: {
        word_t i;
        asid_t asid_base;
        word_t index, depth;
        cap_t untyped, ڕەگ;
        cte_t *parentSlot, *destSlot;
        lookupSlot_ret_t lu_ret;
        فراغ *frame;
        exception_t status;

        إذا (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(length < 2 || current_extra_caps.excaprefs[0] == عدم
                     || current_extra_caps.excaprefs[1] == عدم)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        index = getSyscallArg(0, buffer);
        depth = getSyscallArg(1, buffer);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        ڕەگ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        إذا (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_نزم_Bits;

        إذا (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_capBlockSize(untyped) !=
                     seL4_ASIDPoolBits || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ژمار = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        إذا (unlikely(status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            أرجع status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(ڕەگ, index, depth);
        إذا (unlikely(lu_ret.status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            أرجع lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        إذا (unlikely(status != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            أرجع status;
        }

        setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(frame, destSlot,
                                            parentSlot, asid_base);
    }

    حالة cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pdCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t asid;

        إذا (unlikely(invLabel != ARMASIDPoolAssign)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pdCapSlot = current_extra_caps.excaprefs[0];
        pdCap = pdCapSlot->cap;

        إذا (unlikely(
                cap_get_capType(pdCap) != cap_page_directory_cap ||
                cap_page_directory_cap_get_capPDIsMapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ژمار = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_بەرز(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_ڕەگ_new();

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ژمار = 0;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < (1 << asid_نزم_Bits) && (asid + i == 0 || pool->array[i]); i++);

        إذا (unlikely(i == 1 << asid_نزم_Bits)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = seL4_DeleteFirst;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(گرێ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, pdCapSlot);
    }

    افتراضي:
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
    lu_ret = lookupCapAndSlot(گرێ_STATE(ksCurThread), frame_cptr);

    إذا (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    frameSize = generic_frame_cap_get_capFSize(lu_ret.cap);

    إذا (frameSize != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = generic_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((فراغ *) frame_pptr);

    لكل (صحيح idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
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

    أرجع EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef CONFIG_DEBUG_BUILD
فراغ kernelPrefetchAbort(word_t pc, word_t lr) VISIBLE;
فراغ kernelDataAbort(word_t pc) VISIBLE;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

فراغ kernelUndefinedInstruction(word_t pc) VISIBLE;

فراغ kernelPrefetchAbort(word_t pc, word_t lr)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    printf("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

فراغ kernelDataAbort(word_t pc)
{
    printf("\n\nKERNEL DATA ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HDFAR: 0x%"SEL4_PRIx_word" HSR: 0x%"SEL4_PRIx_word"\n",
           getHDFAR(), getHSR());
    halt();
}

فراغ kernelUndefinedInstruction(word_t pc)
{
    printf("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("HSR: 0x%"SEL4_PRIx_word"\n", getHSR());
    halt();
}

#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */

فراغ kernelPrefetchAbort(word_t pc, word_t lr)
{
    printf("\n\nKERNEL PREFETCH ABORT!\n");
    printf("Faulting instruction: 0x%"SEL4_PRIx_word"\n", pc);
    printf("IFSR: 0x%"SEL4_PRIx_word"\n", getIFSR());
    printf("LR: 0x%"SEL4_PRIx_word"\n", lr);
    halt();
}

فراغ kernelDataAbort(word_t pc)
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
تعريفنوع بنية readWordFromVSpace_ret {
    exception_t status;
    word_t بەها;
} readWordFromVSpace_ret_t;

ساكن readWordFromVSpace_ret_t readWordFromVSpace(pde_t *pd, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    lookupPTSlot_ret_t ptSlot;
    pde_t *pdSlot;
    paddr_t paddr;
    word_t offset;
    pptr_t kernel_vaddr;
    word_t *بەها;

    pdSlot = lookupPDSlot(pd, vaddr);
    إذا (pde_ptr_get_pdeType(pdSlot) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_address(pdSlot);
        offset = vaddr & MASK(ARMSectionBits);
    } وإلا {
        ptSlot = lookupPTSlot(pd, vaddr);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_address(ptSlot.ptSlot);
            إذا (pte_pte_small_ptr_get_contiguous_hint(ptSlot.ptSlot)) {
                offset = vaddr & MASK(ARMLargePageBits);
            } وإلا {
                offset = vaddr & MASK(ARMSmallPageBits);
            }
#وإلا
        إذا (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_address(ptSlot.ptSlot);
            offset = vaddr & MASK(ARMSmallPageBits);
        } وإلا إذا (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_pteType(ptSlot.ptSlot) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_address(ptSlot.ptSlot);
            offset = vaddr & MASK(ARMLargePageBits);
#endif
        } وإلا {
            ret.status = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }
    }


    kernel_vaddr = (word_t)paddr_to_pptr(paddr);
    بەها = (word_t *)(kernel_vaddr + offset);
    ret.status = EXCEPTION_NONE;
    ret.بەها = *بەها;
    أرجع ret;
}

فراغ Arch_user_کەڵەکە_Trace(tcb_t *tptr)
{
    cap_t thread_ڕەگ;
    pde_t *pd;
    word_t sp;
    صحيح i;

    thread_ڕەگ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    إذا (cap_get_capType(thread_ڕەگ) != cap_page_directory_cap) {
        printf("Invalid vspace\n");
        أرجع;
    }

    pd = (pde_t *)pptr_of_cap(thread_ڕەگ);

    sp = getRegister(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        printf("SP not aligned\n");
        أرجع;
    }

    لكل (i = 0; i < CONFIG_USER_کەڵەکە_TRACE_LENGTH; i++) {
        word_t address = sp + (i * حجم(word_t));
        readWordFromVSpace_ret_t result;
        result = readWordFromVSpace(pd, address);
        إذا (result.status == EXCEPTION_NONE) {
            printf("0x%lx: 0x%lx\n", (long)address, (long)result.بەها);
        } وإلا {
            printf("0x%lx: INVALID\n", (long)address);
        }
    }
}
#endif
