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

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ضمن <benchmark/benchmark_track.h>
#د_که_پای

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#تعریف_کړه SECTIONS_فی_SUPER_SECTION بټ(ARMSuper_Section_بټونه - ARMSection_بټونه)
#تعریف_کړه پاڼې_فی_LARGE_پاڼه بټ(ARMLarge_پاڼه_بټونه - ARMSmall_پاڼه_بټونه)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#تعریف_کړه وروستی_BYTE_PTE(PTE,اوږدتیا_2) ((کلمه_t)&(PTE)[(اوږدتیا_2)-1] + (بټ(PTE_اندازه_بټونه)-1))
#تعریف_کړه وروستی_BYTE_PDE(PDE,اوږدتیا_2) ((کلمه_t)&(PDE)[(اوږدتیا_2)-1] + (بټ(PDE_اندازه_بټونه)-1))

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#تعریف_کړه MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#تعریف_کړه MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#تعریف_کړه ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#تعریف_کړه ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ساكن فراغ سېټ_Global_PD(فراغ);

بنية resolve_ret {
    paddr_t چوکاټ_بنسټ;
    vm_پاڼه_اندازه_t چوکاټ_اندازه;
    bool_t valid;
};
تعريفنوع بنية resolve_ret resolve_ret_t;

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
ساكن bool_t خالص pteCheckIfMapped(pte_t *pte);
ساكن bool_t خالص pdeCheckIfMapped(pde_t *pde);

ساكن کلمه_t CONST APFromVMRights(vm_rights_t vm_rights)
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
BOOT_کوډ
ساكن کلمه_t CONST APFromVMRights(vm_rights_t vm_rights)
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

ساكن کلمه_t CONST HAPFromVMRights(vm_rights_t vm_rights)
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

#د_که_پای

vm_rights_t CONST ماسک_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ماسک)
{
    إذا (vm_rights == VMNoAccess) {
        أرجع VMNoAccess;
    }
    إذا (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_ماسک)) {
        أرجع VMReadOnly;
    }
    إذا (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_ماسک)) {
        إذا (!seL4_CapRights_get_capAllowWrite(cap_rights_ماسک)) {
            أرجع VMReadOnly;
        } وإلا {
            أرجع VMReadWrite;
        }
    }
    إذا (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ماسک) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ماسک)) {
        userError("Attempted to make unsupported write only mapping");
    }
    أرجع VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_کوډ فراغ map_kernel_چوکاټ(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_ځانګړتیاوې_t ځانګړتیاوې)
{
    کلمه_t idx = (vaddr & ماسک(پاڼه_بټونه_For_اندازه(ARMSection))) >> پاڼه_بټونه_For_اندازه(ARMSmall_پاڼه);

    أكد(vaddr >= PPTR_سر); /* vaddr lies in the region the global PT covers */
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t tex;
    إذا (vm_ځانګړتیاوې_get_arm_پاڼه_Cacheable(ځانګړتیاوې)) {
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
            vm_ځانګړتیاوې_get_arm_پاڼه_Cacheable(ځانګړتیاوې)
        );
#د_که_پای /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_کوډ فراغ map_kernel_کړکۍ(فراغ)
{
    paddr_t  phys;
    کلمه_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = phys_بنسټ();
    idx = PPTR_بنسټ >> پاڼه_بټونه_For_اندازه(ARMSection);

    طالما (idx < بټ(PD_شاخص_بټونه) - SECTIONS_فی_SUPER_SECTION) {
        کلمه_t idx2;

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
        لكل (idx2 = idx; idx2 < idx + SECTIONS_فی_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += بټ(پاڼه_بټونه_For_اندازه(ARMSuperSection));
        idx += SECTIONS_فی_SUPER_SECTION;
    }

    طالما (idx < (PPTR_سر >> پاڼه_بټونه_For_اندازه(ARMSection))) {
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
        phys += بټ(پاڼه_بټونه_For_اندازه(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    أكد(phys == PADDR_سر);

#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /*پته*/
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, بټ(se_L4_پاڼه_جدول_بټونه));

    phys += بټ(پاڼه_بټونه_For_اندازه(ARMSection));
    idx++;
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */

    /* map page table covering last 1M of virtual address space to page directory */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalPT), /*پته*/
            true,                        /* P       */
            0                            /* Domain  */
        );

    /* now start initialising the page table */
    memzero(armKSGlobalPT, 1 << se_L4_پاڼه_جدول_بټونه);

    /* map vector table */
    map_kernel_چوکاټ(
        addrFromKPPtr(arm_ویکټور_جدول),
        PPTR_ویکټور_جدول,
        VMKernelOnly,
        vm_ځانګړتیاوې_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_کوډ فراغ map_kernel_کړکۍ(فراغ)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t ناکارول_شوی pteS2;

    /* Initialise PGD */
    لكل (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    لكل (idx = 0; idx < (USER_سر - 0xC0000000) >> (PT_شاخص_بټونه + پاڼه_بټونه); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = phys_بنسټ();
    لكل (; idx < بټ(PT_شاخص_بټونه) - 1; idx++) {
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
        phys += بټ(PT_شاخص_بټونه + پاڼه_بټونه);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << se_L4_پاڼه_جدول_بټونه);
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
        phys += بټ(پاڼه_بټونه);
    }
    /* map vector table */
    map_kernel_چوکاټ(
        addrFromKPPtr(arm_ویکټور_جدول),
        PPTR_ویکټور_جدول,
        VMKernelOnly,
        vm_ځانګړتیاوې_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#د_که_پای /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

ساكن BOOT_کوډ فراغ map_it_چوکاټ_cap(cap_t pd_cap, cap_t چوکاټ_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *موخه_خونه;
    pde_t *pd    = PDE_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    فراغ  *چوکاټ = (فراغ *)generic_چوکاټ_cap_get_cap_FBase_Ptr(چوکاټ_cap);
    vptr_t vptr  = generic_چوکاټ_cap_get_cap_FMapped_پته(چوکاټ_cap);

    أكد(generic_چوکاټ_cap_get_cap_FMapped_ASID(چوکاټ_cap) != 0);

    pd += (vptr >> پاڼه_بټونه_For_اندازه(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پته(pd));
    موخه_خونه = pt + ((vptr & ماسک(پاڼه_بټونه_For_اندازه(ARMSection)))
                       >> پاڼه_بټونه_For_اندازه(ARMSmall_پاڼه));
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    *موخه_خونه = pte_pte_small_new(
                      addrFromPPtr(چوکاټ),
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
    *موخه_خونه = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(چوکاټ),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#د_که_پای
}

/* Create a frame cap for the initial thread. */

ساكن BOOT_کوډ cap_t create_it_چوکاټ_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    إذا (use_large)
        أرجع
            cap_چوکاټ_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_ټیټ(asid),                /* capFMappedASIDLow  */
                کلمه_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_لوړ(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    وإلا
        أرجع
            cap_small_چوکاټ_cap_new(
                ASID_ټیټ(asid),                /* capFMappedASIDLow  */
                کلمه_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#که_تعریف_شوی CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#د_که_پای
                ASID_لوړ(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

ساكن BOOT_کوډ فراغ map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(pt_cap);
    pde_t *موخه_خونه = pd + (vptr >> پاڼه_بټونه_For_اندازه(ARMSection));

    أكد(cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(pt_cap));

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    *موخه_خونه = pde_pde_coarse_new(
                      addrFromPPtr(pt), /*پته*/
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#وإلا
    *موخه_خونه = pde_pde_coarse_new(addrFromPPtr(pt));
#د_که_پای
}

/* Create a page table for the initial thread */

ساكن BOOT_کوډ cap_t create_it_پاڼه_جدول_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_پاڼه_جدول_cap_new(
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

BOOT_کوډ کلمه_t arch_get_n_paging(v_region_t it_v_reg)
{
    أرجع get_n_paging(it_v_reg, PT_شاخص_بټونه + پاڼه_بټونه);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_کوډ cap_t create_it_پته_space(cap_t ریښه_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    se_L4_خونه_Pos خونه_pos_before;
    se_L4_خونه_Pos خونه_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << se_L4_پاڼه_Dir_بټونه) - 1,
                        addrFromPPtr((فراغ *)rootserver.vspace));
    cap_t pd_cap =
        cap_پاڼه_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    خونه_pos_before = ndks_boot.خونه_pos_cur;
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    لكل (pt_vptr = پړاو_ښکته(it_v_reg.پیل, PT_شاخص_بټونه + پاڼه_بټونه);
         pt_vptr < it_v_reg.پای_ټکی;
         pt_vptr += بټ(PT_شاخص_بټونه + پاڼه_بټونه)) {
        إذا (!provide_cap(ریښه_cnode_cap,
                         create_it_پاڼه_جدول_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    خونه_pos_after = ndks_boot.خونه_pos_cur;
    ndks_boot.bi_چوکاټ->userImagePaging = (se_L4_خونه_Region) {
        خونه_pos_before, خونه_pos_after
    };

    أرجع pd_cap;
}

BOOT_کوډ cap_t create_unmapped_it_چوکاټ_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_چوکاټ_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_کوډ cap_t create_mapped_it_چوکاټ_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_چوکاټ_cap(pptr, vptr, asid, use_large);
    map_it_چوکاټ_cap(pd_cap, cap, executable);
    أرجع cap;
}

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_کوډ فراغ activate_kernel_vspace(فراغ)
{
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    سېټ_Global_PD();
    invalidateLocalTLB();
    قلف_TLBEntry(PPTR_بنسټ);
    قلف_TLBEntry(PPTR_ویکټور_جدول);
}

#وإلا

BOOT_کوډ فراغ activate_kernel_vspace(فراغ)
{
    uint32_t r;
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    /* Setup the memory attributes: We use 2 indices (cachable/non-cacheable) */
    سېټ_HMAIR((ATTRINDX_NONCACHEABLE << 0) | (ATTRINDX_CACHEABLE << 8), 0);
    سېټ_Current_Hyp_PD(addrFromKPPtr(armHSGlobalPGD));
    invalidateHypTLB();
#إذا 0 /* Can't lock entries on A15 */
    قلف_TLBEntry(PPTR_بنسټ);
    قلف_TLBEntry(PPTR_ویکټور_جدول);
#د_که_پای
    /* TODO find a better place to init the VMMU */
    r = 0;
    /* Translation range */
    r |= (0x0 << 0);     /* 2^(32 -(0)) input range. */
    r |= (r & 0x8) << 1; /* Sign bit */
    /* starting level */
    r |= (0x0 << 6);     /* Start at second level */
    /* Sharability of tables */
    r |= بټ(8);       /* Inner write-back, write-allocate */
    r |= بټ(10);      /* Outer write-back, write-allocate */
    /* Long descriptor format (not that we have a choice) */
    r |= بټ(31);
    سېټ_VTCR(r);
}

#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_کوډ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_د_cap(it_ap_cap));
    ap->array[ASID_ټیټ(IT_ASID)] = PDE_PTR(pptr_د_cap(it_pd_cap));
    armKSASIDTable[ASID_لوړ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_PDFor_ASID(asid_t asid)
{
    پیدا_کړه_PDFor_ASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_لوړ(asid)];
    إذا (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_ریښه_new();

        ret.pd = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    pd = poolPtr->array[ASID_ټیټ(asid)];
    إذا (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_ریښه_new();

        ret.pd = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.pd = pd;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

کلمه_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    کلمه_t w_بفر_Ptr;
    cap_t بفر_Cap;
    vm_rights_t vm_rights;

    w_بفر_Ptr = thread->tcbIPCBuffer;
    بفر_Cap = TCB_PTR_CTE_PTR(thread, tcb_بفر)->cap;

    إذا (unlikely(cap_get_capType(بفر_Cap) != cap_small_چوکاټ_cap &&
                 cap_get_capType(بفر_Cap) != cap_چوکاټ_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(generic_چوکاټ_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = generic_چوکاټ_cap_get_cap_FVMRights(بفر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        کلمه_t بنسټ_Ptr;
        بینښه صحيح پاڼه_بټونه_2;

        بنسټ_Ptr = generic_چوکاټ_cap_get_cap_FBase_Ptr(بفر_Cap);
        پاڼه_بټونه_2 = پاڼه_بټونه_For_اندازه(generic_چوکاټ_cap_get_cap_FSize(بفر_Cap));
        أرجع (کلمه_t *)(بنسټ_Ptr + (w_بفر_Ptr & ماسک(پاڼه_بټونه_2)));
    } وإلا {
        أرجع عدم;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (unlikely(cap_get_capType(cap) != cap_small_چوکاټ_cap &&
                 cap_get_capType(cap) != cap_چوکاټ_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(generic_چوکاټ_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(vptr & ماسک(se_L4_IPCBuffer_اندازه_بټونه))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    بینښه صحيح pd_شاخص;

    pd_شاخص = vptr >> (پاڼه_بټونه + PT_شاخص_بټونه);
    أرجع pd + pd_شاخص;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pd_خونه;

    pd_خونه = lookupPDSlot(pd, vptr);

    إذا (unlikely(pde_ptr_get_pdeType(pd_خونه) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_شاخص_بټونه + پاڼه_بټونه);

        ret.pt_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pte_t *pt, *pt_خونه;
        بینښه صحيح pt_شاخص;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پته(pd_خونه));
        pt_شاخص = (vptr >> پاڼه_بټونه) & ماسک(PT_شاخص_بټونه);
        pt_خونه = pt + pt_شاخص;

        ret.pt_خونه = pt_خونه;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    }
}

ساكن pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    بینښه صحيح pt_شاخص;

    pt_شاخص = (vptr >> پاڼه_بټونه) & ماسک(PT_شاخص_بټونه);
    أرجع pt + pt_شاخص;
}

ساكن ثابت resolve_ret_t default_resolve_ret_t;

ساكن resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    بدل (pde_ptr_get_pdeType(pde)) {
    حالة pde_pde_section:
        ret.چوکاټ_بنسټ = pde_pde_section_ptr_get_پته(pde);
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (pde_pde_section_ptr_get_اندازه(pde)) {
            ret.چوکاټ_اندازه = ARMSuperSection;
        } وإلا {
            ret.چوکاټ_اندازه = ARMSection;
        }
#وإلا
        إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.چوکاټ_بنسټ &= ~ماسک(پاڼه_بټونه_For_اندازه(ARMSuperSection));
            ret.چوکاټ_اندازه = ARMSuperSection;
        } وإلا {
            ret.چوکاټ_اندازه = ARMSection;
        }
#د_که_پای
        أرجع ret;

    حالة pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پته(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small:
            ret.چوکاټ_بنسټ = pte_pte_small_ptr_get_پته(pte);
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.چوکاټ_بنسټ &= ~ماسک(پاڼه_بټونه_For_اندازه(ARMLarge_پاڼه));
                ret.چوکاټ_اندازه = ARMLarge_پاڼه;
            } وإلا {
                ret.چوکاټ_اندازه = ARMSmall_پاڼه;
            }
#وإلا
            ret.چوکاټ_اندازه = ARMSmall_پاڼه;
#د_که_پای
            أرجع ret;

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large:
            ret.چوکاټ_بنسټ = pte_pte_large_ptr_get_پته(pte);
            ret.چوکاټ_اندازه = ARMLarge_پاڼه;
            أرجع ret;
#د_که_پای
        افتراضي:
            اكسر;
        }
        اكسر;
    }
    }

    ret.valid = false;
    أرجع ret;
}

ساكن pte_t CONST makeUserPTE(vm_پاڼه_اندازه_t پاڼه_اندازه, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t ap;

    ap = APFromVMRights(vm_rights);

    بدل (پاڼه_اندازه) {
    حالة ARMSmall_پاڼه: {
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

    حالة ARMLarge_پاڼه: {
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

    کلمه_t hap;

    hap = HAPFromVMRights(vm_rights);

    بدل (پاڼه_اندازه) {
    حالة ARMSmall_پاڼه: {
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

    حالة ARMLarge_پاڼه: {
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
#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    أرجع pte;
}

ساكن pde_t CONST makeUserPDE(vm_پاڼه_اندازه_t پاڼه_اندازه, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, کلمه_t domain,
                               vm_rights_t vm_rights)
{
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t ap, size2;

    ap = APFromVMRights(vm_rights);
#وإلا
    کلمه_t hap, size2;

    (فراغ)domain;
    hap = HAPFromVMRights(vm_rights);
#د_که_پای

    بدل (پاڼه_اندازه) {
    حالة ARMSection:
        size2 = 0;
        اكسر;

    حالة ARMSuperSection:
        size2 = 1;
        اكسر;

    افتراضي:
        fail("Invalid PDE frame type");
    }

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
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
#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_ریښه(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_پاڼه_directory_cap &&
           cap_پاڼه_directory_cap_get_cap_PDIs_Mapped(cap);
}

bool_t CONST is_IOSpace_چوکاټ_Cap(cap_t cap)
{
#که_تعریف_شوی CONFIG_TK1_SMMU
    أرجع cap_get_capType(cap) == cap_small_چوکاټ_cap && cap_small_چوکاټ_cap_get_cap_FIs_IOSpace(cap);
#وإلا
    أرجع false;
#د_که_پای
}

/** Switch to the empty PD on the reserved HW ASID */
ساكن فراغ سېټ_Global_PD(فراغ)
{
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
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
    سېټ_Hardware_ASID(hwASIDReserved);
#د_که_پای
}

فراغ سېټ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ریښه;
    asid_t asid;
    pde_t *pd;
    پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret;

    thread_ریښه = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_ریښه) != cap_پاڼه_directory_cap ||
        !cap_پاڼه_directory_cap_get_cap_PDIs_Mapped(thread_ریښه)) {
        سېټ_Global_PD();
        أرجع;
    }

    pd = PDE_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(thread_ریښه));
    asid = cap_پاڼه_directory_cap_get_cap_PDMapped_ASID(thread_ریښه);
    پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(asid);
    إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE || پیدا_کړه_ret.pd != pd)) {
        سېټ_Global_PD();
        أرجع;
    }

    armv_contextSwitch(pd, asid);
}

ساكن bool_t سېټ_VMRoot_For_Flush(pde_t *pd, asid_t asid)
{
    cap_t thread_ریښه;

    thread_ریښه = TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbVTable)->cap;

    إذا (cap_get_capType(thread_ریښه) == cap_پاڼه_directory_cap &&
        cap_پاڼه_directory_cap_get_cap_PDIs_Mapped(thread_ریښه) &&
        PDE_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(thread_ریښه)) == pd) {
        أرجع false;
    }

    armv_contextSwitch(pd, asid);

    أرجع true;
}

pde_t *پاڼه_جدول_Mapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret;
    pde_t pde;
    بینښه صحيح pd_شاخص;

    پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(asid);
    إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
        أرجع عدم;
    }

    pd_شاخص = vaddr >> (پاڼه_بټونه + PT_شاخص_بټونه);
    pde = پیدا_کړه_ret.pd[pd_شاخص];

    إذا (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_پته(pde)) == pt)) {
        أرجع پیدا_کړه_ret.pd;
    } وإلا {
        أرجع عدم;
    }
}

ساكن فراغ invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_لوړ(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_ټیټ(asid)];
    أكد(pd);

    pd[PD_ASID_خونه] = pde_pde_invalid_new(0, false);
}

ساكن pde_t خالص بارول_HWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_لوړ(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_ټیټ(asid)];
    أكد(pd);

    أرجع pd[PD_ASID_خونه];
}

ساكن فراغ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_لوړ(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_ټیټ(asid)];
    أكد(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_خونه] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t پیدا_کړه_Free_HWASID(فراغ)
{
    کلمه_t hw_asid_دننه_واټن;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    لكل (hw_asid_دننه_واټن = 0;
         hw_asid_دننه_واټن <= (کلمه_t)((hw_asid_t) - 1);
         hw_asid_دننه_واټن ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_دننه_واټن);
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

    stored_hw_asid = بارول_HWASID(asid);
    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } وإلا {
        hw_asid_t new_hw_asid;

        new_hw_asid = پیدا_کړه_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        أرجع new_hw_asid;
    }
}

ساكن فراغ invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = بارول_HWASID(asid);
    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

فراغ unmap_پاڼه_جدول(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pd_خونه;
    بینښه صحيح pd_شاخص;

    pd = پاڼه_جدول_Mapped(asid, vaddr, pt);

    إذا (likely(pd != عدم)) {
        pd_شاخص = vaddr >> (PT_شاخص_بټونه + پاڼه_بټونه);
        pd_خونه = pd + pd_شاخص;

        *pd_خونه = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((کلمه_t)pd_خونه, addrFromPPtr(pd_خونه));
        flush_جدول(pd, asid, vaddr, pt);
    }
}

فراغ copyGlobalMappings(pde_t *newPD)
{
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t i;
    pde_t *global_pd = armKSGlobalPD;

    لكل (i = PPTR_بنسټ >> ARMSection_بټونه; i < بټ(PD_شاخص_بټونه); i++) {
        إذا (i != PD_ASID_خونه) {
            newPD[i] = global_pd[i];
        }
    }
#د_که_پای
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    بدل (vm_faultType) {
    حالة ARMDataAbort: {
        کلمه_t addr, fault;

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (پته_Translate_S1(addr) & ~ماسک(پاڼه_بټونه)) | (addr & ماسک(پاڼه_بټونه));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#وإلا
        addr = getFAR();
        fault = getDFSR();
#د_که_پای

#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
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
#د_که_پای
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        أرجع EXCEPTION_FAULT;
    }

    حالة ARMPrefetchAbort: {
        کلمه_t pc, fault;

        pc = getRestartPC(thread);

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (پته_Translate_S1(pc) & ~ماسک(پاڼه_بټونه)) | (pc & ماسک(پاڼه_بټونه));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#وإلا
        fault = getIFSR();
#د_که_پای

#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
        إذا (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            إذا (seL4_Fault_DebugException_get_exceptionReason(current_fault) == se_L4_Single_ګام
                && !single_ګام_Fault_Counter_Ready(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                أرجع EXCEPTION_NONE;
            }
            أرجع EXCEPTION_FAULT;
        }
#د_که_پای
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        أرجع EXCEPTION_FAULT;
    }

    افتراضي:
        fail("Invalid VM fault type");
    }
}

فراغ deleteASIDPool(asid_t asid_بنسټ, asid_pool_t *pool)
{
    بینښه صحيح دننه_واټن;

    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنسټ & ماسک(asid_ټیټ_بټونه)) == 0);

    إذا (armKSASIDTable[ASID_لوړ(asid_بنسټ)] == pool) {
        لكل (دننه_واټن = 0; دننه_واټن < بټ(asid_ټیټ_بټونه); دننه_واټن++) {
            إذا (pool->array[دننه_واټن]) {
                flushSpace(asid_بنسټ + دننه_واټن);
                invalidateASIDEntry(asid_بنسټ + دننه_واټن);
            }
        }
        armKSASIDTable[ASID_لوړ(asid_بنسټ)] = عدم;
        سېټ_VMRoot(غوټه_STATE(ksCurThread));
    }
}

فراغ deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_لوړ(asid)];

    إذا (poolPtr != عدم && poolPtr->array[ASID_ټیټ(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_ټیټ(asid)] = عدم;
        سېټ_VMRoot(غوټه_STATE(ksCurThread));
    }
}

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
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
#د_که_پای

فراغ unmap_پاڼه(vm_پاڼه_اندازه_t پاڼه_اندازه, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret;
    paddr_t addr = addrFromPPtr(pptr);

    پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(asid);
    إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
        أرجع;
    }

    بدل (پاڼه_اندازه) {
    حالة ARMSmall_پاڼه: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(پیدا_کړه_ret.pd, vptr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            أرجع;
        }

        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_خونه) != pte_pte_small)) {
            أرجع;
        }
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_خونه) != 0)) {
            أرجع;
        }
#د_که_پای
        إذا (unlikely(pte_pte_small_ptr_get_پته(lu_ret.pt_خونه) != addr)) {
            أرجع;
        }

        *(lu_ret.pt_خونه) = pte_pte_invalid_new();
        cleanByVA_PoU((کلمه_t)lu_ret.pt_خونه, addrFromPPtr(lu_ret.pt_خونه));

        اكسر;
    }

    حالة ARMLarge_پاڼه: {
        lookupPTSlot_ret_t lu_ret;
        کلمه_t i;

        lu_ret = lookupPTSlot(پیدا_کړه_ret.pd, vptr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            أرجع;
        }
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_خونه) != pte_pte_large)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_large_ptr_get_پته(lu_ret.pt_خونه) != addr)) {
            أرجع;
        }
#وإلا
        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_خونه) != pte_pte_small)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_خونه) != 1)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_small_ptr_get_پته(lu_ret.pt_خونه) != addr)) {
            أرجع;
        }
#د_که_پای

        لكل (i = 0; i < پاڼې_فی_LARGE_پاڼه; i++) {
            lu_ret.pt_خونه[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((کلمه_t)&lu_ret.pt_خونه[0],
                            وروستی_BYTE_PTE(lu_ret.pt_خونه, پاڼې_فی_LARGE_پاڼه),
                            addrFromPPtr(&lu_ret.pt_خونه[0]));

        اكسر;
    }

    حالة ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(پیدا_کړه_ret.pd, vptr);

        إذا (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            أرجع;
        }
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pde_pde_section_ptr_get_اندازه(pd) != 0)) {
#وإلا
        إذا (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#د_که_پای
            أرجع;
        }
        إذا (unlikely(pde_pde_section_ptr_get_پته(pd) != addr)) {
            أرجع;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((کلمه_t)pd, addrFromPPtr(pd));

        اكسر;
    }

    حالة ARMSuperSection: {
        pde_t *pd;
        کلمه_t i;

        pd = lookupPDSlot(پیدا_کړه_ret.pd, vptr);

        إذا (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            أرجع;
        }
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pde_pde_section_ptr_get_اندازه(pd) != 1)) {
#وإلا
        إذا (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#د_که_پای
            أرجع;
        }
        إذا (unlikely(pde_pde_section_ptr_get_پته(pd) != addr)) {
            أرجع;
        }

        لكل (i = 0; i < SECTIONS_فی_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((کلمه_t)&pd[0], وروستی_BYTE_PDE(pd, SECTIONS_فی_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        اكسر;
    }

    افتراضي:
        fail("Invalid ARM page type");
        اكسر;
    }

    /* Flush the page now that the mapping has been updated */
    flush_پاڼه(پاڼه_اندازه, پیدا_کړه_ret.pd, asid, vptr);
}

فراغ flush_پاڼه(vm_پاڼه_اندازه_t پاڼه_اندازه, pde_t *pd, asid_t asid, کلمه_t vptr)
{
    pde_t stored_hw_asid;
    کلمه_t بنسټ_addr;
    bool_t ریښه_switched;

    أكد((vptr & ماسک(پاڼه_بټونه_For_اندازه(پاڼه_اندازه))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ریښه_switched = سېټ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = بارول_HWASID(asid);

    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        بنسټ_addr = vptr & ~ماسک(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(بنسټ_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        إذا (ریښه_switched) {
            سېټ_VMRoot(غوټه_STATE(ksCurThread));
        }
    }
}

فراغ flush_جدول(pde_t *pd, asid_t asid, کلمه_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t ریښه_switched;

    أكد((vptr & ماسک(PT_شاخص_بټونه + ARMSmall_پاڼه_بټونه)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ریښه_switched = سېټ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = بارول_HWASID(asid);

    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        إذا (ریښه_switched) {
            سېټ_VMRoot(غوټه_STATE(ksCurThread));
        }
    }
}

فراغ flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = بارول_HWASID(asid);

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

    stored_hw_asid = بارول_HWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    إذا (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ساكن مضمن bool_t CONST checkVPAlignment(vm_پاڼه_اندازه_t sz, کلمه_t w)
{
    أرجع (w & ماسک(پاڼه_بټونه_For_اندازه(sz))) == 0;
}

بنية create_mappings_pte_return {
    exception_t حالت;
    pte_t pte;
    pte_range_t pte_entries;
};
تعريفنوع بنية create_mappings_pte_return create_mappings_pte_return_t;

بنية create_mappings_pde_return {
    exception_t حالت;
    pde_t pde;
    pde_range_t pde_entries;
};
تعريفنوع بنية create_mappings_pde_return create_mappings_pde_return_t;

ساكن create_mappings_pte_return_t
create_خوندي_Mapping_Entries_PTE
(paddr_t بنسټ, کلمه_t vaddr, vm_پاڼه_اندازه_t چوکاټ_اندازه,
 vm_rights_t vmRights, vm_ځانګړتیاوې_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    کلمه_t i;

    بدل (چوکاټ_اندازه) {

    حالة ARMSmall_پاڼه:

        ret.pte_entries.بنسټ = عدم; /* to avoid uninitialised warning */
        ret.pte_entries.اوږدتیا = 1;

        ret.pte = makeUserPTE(ARMSmall_پاڼه, بنسټ,
                              vm_ځانګړتیاوې_get_arm_پاڼه_Cacheable(attr),
                              vm_ځانګړتیاوې_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه =
                false;
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            أرجع ret;
        }

        ret.pte_entries.بنسټ = lu_ret.pt_خونه;
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_ptr_get_pteType(ret.pte_entries.بنسټ) ==
                     pte_pte_large)) {
#وإلا
        إذا (unlikely(pte_ptr_get_pteType(ret.pte_entries.بنسټ) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.بنسټ))) {
#د_که_پای
            current_syscall_error.type =
                se_L4_Delete_لومړی;

            ret.حالت = EXCEPTION_SYSCALL_ERROR;
            أرجع ret;
        }

        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة ARMLarge_پاڼه:

        ret.pte_entries.بنسټ = عدم; /* to avoid uninitialised warning */
        ret.pte_entries.اوږدتیا = پاڼې_فی_LARGE_پاڼه;

        ret.pte = makeUserPTE(ARMLarge_پاڼه, بنسټ,
                              vm_ځانګړتیاوې_get_arm_پاڼه_Cacheable(attr),
                              vm_ځانګړتیاوې_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه =
                false;
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            أرجع ret;
        }

        ret.pte_entries.بنسټ = lu_ret.pt_خونه;

        لكل (i = 0; i < پاڼې_فی_LARGE_پاڼه; i++) {
            إذا (unlikely(pte_get_pteType(ret.pte_entries.بنسټ[i]) ==
                         pte_pte_small)
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.بنسټ[i])
#د_که_پای
               ) {
                current_syscall_error.type =
                    se_L4_Delete_لومړی;

                ret.حالت = EXCEPTION_SYSCALL_ERROR;
                أرجع ret;
            }
        }

        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        fail("Invalid or unexpected ARM page type.");

    }
}

ساكن create_mappings_pde_return_t
create_خوندي_Mapping_Entries_PDE
(paddr_t بنسټ, کلمه_t vaddr, vm_پاڼه_اندازه_t چوکاټ_اندازه,
 vm_rights_t vmRights, vm_ځانګړتیاوې_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_نښان_t currentPDEType;
    کلمه_t i;

    بدل (چوکاټ_اندازه) {

    /* PDE mappings */
    حالة ARMSection:
        ret.pde_entries.بنسټ = lookupPDSlot(pd, vaddr);
        ret.pde_entries.اوږدتیا = 1;

        ret.pde = makeUserPDE(ARMSection, بنسټ,
                              vm_ځانګړتیاوې_get_arm_Parity_Enabled(attr),
                              vm_ځانګړتیاوې_get_arm_پاڼه_Cacheable(attr),
                              vm_ځانګړتیاوې_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.بنسټ);
        إذا (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_اندازه(ret.pde_entries.بنسټ) != 0))) {
#وإلا
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.بنسټ) != 0))) {
#د_که_پای
            current_syscall_error.type =
                se_L4_Delete_لومړی;
            ret.حالت = EXCEPTION_SYSCALL_ERROR;

            أرجع ret;
        }

        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة ARMSuperSection:
        ret.pde_entries.بنسټ = lookupPDSlot(pd, vaddr);
        ret.pde_entries.اوږدتیا = SECTIONS_فی_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, بنسټ,
                              vm_ځانګړتیاوې_get_arm_Parity_Enabled(attr),
                              vm_ځانګړتیاوې_get_arm_پاڼه_Cacheable(attr),
                              vm_ځانګړتیاوې_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        لكل (i = 0; i < SECTIONS_فی_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.بنسټ[i]);
            إذا (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_اندازه(ret.pde_entries.بنسټ[i]) != 1))) {
#وإلا
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.بنسټ[i]) != 1))) {
#د_که_پای
                current_syscall_error.type =
                    se_L4_Delete_لومړی;
                ret.حالت = EXCEPTION_SYSCALL_ERROR;

                أرجع ret;
            }
        }

        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        fail("Invalid or unexpected ARM page type.");

    }
}

ساكن مضمن vptr_t پاڼه_بنسټ(vptr_t vaddr, vm_پاڼه_اندازه_t اندازه)
{
    أرجع vaddr & ~ماسک(پاڼه_بټونه_For_اندازه(اندازه));
}

ساكن bool_t خالص pteCheckIfMapped(pte_t *pte)
{
    أرجع pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

ساكن bool_t خالص pdeCheckIfMapped(pde_t *pde)
{
    أرجع pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

ساكن فراغ doFlush(صحيح invLabel, vptr_t پیل, vptr_t پای_ټکی, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        پای_ټکی = (vptr_t)paddr_منزل_pptr(pstart) + (پای_ټکی - پیل);
        پیل = (vptr_t)paddr_منزل_pptr(pstart);
    }
    بدل (invLabel) {
    حالة ARMPDClean_معلومات:
    حالة ARMPage_Clean_معلومات:
        cleanCacheRange_RAM(پیل, پای_ټکی, pstart);
        اكسر;
    حالة ARMPDInvalidate_معلومات:
    حالة ARMPage_Invalidate_معلومات:
        invalidateCacheRange_RAM(پیل, پای_ټکی, pstart);
        اكسر;
    حالة ARMPDClean_Invalidate_معلومات:
    حالة ARMPage_Clean_Invalidate_معلومات:
        cleanInvalidateCacheRange_RAM(پیل, پای_ټکی, pstart);
        اكسر;
    حالة ARMPDUnify_Instruction:
    حالة ARMPageUnify_Instruction:
        /* First clean data lines to point of unification
           (L2 cache)... */
        cleanCacheRange_PoU(پیل, پای_ټکی, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(پیل, پای_ټکی, pstart);
        /* ...then invalidate branch predictors... */
        branchFlushRange(پیل, پای_ټکی, pstart);
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

ساكن exception_t performPDFlush(صحيح invLabel, pde_t *pd, asid_t asid, vptr_t پیل,
                                  vptr_t پای_ټکی, paddr_t pstart)
{
    bool_t ریښه_switched;

    /* Flush if given a non zero range */
    إذا (پیل < پای_ټکی) {
        ریښه_switched = سېټ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, پیل, پای_ټکی, pstart);

        إذا (ریښه_switched) {
            سېټ_VMRoot(غوټه_STATE(ksCurThread));
        }
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_جدول_Invocation_Map(cap_t cap, cte_t *ct_خونه,
                                                 pde_t pde, pde_t *pd_خونه)
{
    ct_خونه->cap = cap;
    *pd_خونه = pde;
    cleanByVA_PoU((کلمه_t)pd_خونه, addrFromPPtr(pd_خونه));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_خونه)
{
    إذا (cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_پاڼه_جدول(
            cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(cap),
            pt);
        clear_حافظه_PT((فراغ *)pt, cap_get_cap_اندازه_بټونه(cap));
    }
    cap_پاڼه_جدول_cap_ptr_سېټ_cap_PTIs_Mapped(&(ct_خونه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_Invocation_Map_PTE(asid_t asid, cap_t cap, cte_t *ct_خونه, pte_t pte,
                                               pte_range_t pte_entries)
{
    کلمه_t i, j ناکارول_شوی;
    bool_t tlbflush_required;

    ct_خونه->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.بنسټ);

    j = pte_entries.اوږدتیا;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t بنسټ_پته = pte_pte_small_get_پته(pte);
#د_که_پای
    لكل (i = 0; i < pte_entries.اوږدتیا; i++) {
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_سېټ_پته(pte, بنسټ_پته + i * بټ(پاڼه_بټونه_For_اندازه(ARMSmall_پاڼه)));
#د_که_پای
        pte_entries.بنسټ[i] = pte;
    }
    cleanCacheRange_PoU((کلمه_t)pte_entries.بنسټ,
                        وروستی_BYTE_PTE(pte_entries.بنسټ, pte_entries.اوږدتیا),
                        addrFromPPtr(pte_entries.بنسټ));
    إذا (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_Invocation_Map_PDE(asid_t asid, cap_t cap, cte_t *ct_خونه, pde_t pde,
                                               pde_range_t pde_entries)
{
    کلمه_t i, j ناکارول_شوی;
    bool_t tlbflush_required;

    ct_خونه->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.بنسټ);

    j = pde_entries.اوږدتیا;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t بنسټ_پته = pde_pde_section_get_پته(pde);
#د_که_پای
    لكل (i = 0; i < pde_entries.اوږدتیا; i++) {
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_سېټ_پته(pde, بنسټ_پته + i * بټ(پاڼه_بټونه_For_اندازه(ARMSection)));
#د_که_پای
        pde_entries.بنسټ[i] = pde;
    }
    cleanCacheRange_PoU((کلمه_t)pde_entries.بنسټ,
                        وروستی_BYTE_PDE(pde_entries.بنسټ, pde_entries.اوږدتیا),
                        addrFromPPtr(pde_entries.بنسټ));
    إذا (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_Invocation_Unmap(cap_t cap, cte_t *ct_خونه)
{
    إذا (generic_چوکاټ_cap_get_cap_FIs_Mapped(cap)) {
        unmap_پاڼه(generic_چوکاټ_cap_get_cap_FSize(cap),
                  generic_چوکاټ_cap_get_cap_FMapped_ASID(cap),
                  generic_چوکاټ_cap_get_cap_FMapped_پته(cap),
                  (فراغ *)generic_چوکاټ_cap_get_cap_FBase_Ptr(cap));
    }

    generic_چوکاټ_cap_ptr_سېټ_cap_FMapped_پته(&ct_خونه->cap, asidInvalid, 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_Flush(صحيح invLabel, pde_t *pd, asid_t asid, vptr_t پیل,
                                    vptr_t پای_ټکی, paddr_t pstart)
{
    bool_t ریښه_switched;

    /* now we can flush. But only if we were given a non zero range */
    إذا (پیل < پای_ټکی) {
        ریښه_switched = سېټ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, پیل, پای_ټکی, pstart);

        إذا (ریښه_switched) {
            سېټ_VMRoot(غوټه_STATE(ksCurThread));
        }
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_پاڼه_Get_پته(فراغ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = غوټه_STATE(ksCurThread);
    إذا (call) {
        کلمه_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
        بینښه صحيح اوږدتیا = سېټ_MR(thread, ipc_بفر, 0, capFBasePtr);
        سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, اوږدتیا)));
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pd_Cap_خونه)
{
    cap_پاڼه_directory_cap_ptr_سېټ_cap_PDMapped_ASID(&pd_Cap_خونه->cap, asid);
    cap_پاڼه_directory_cap_ptr_سېټ_cap_PDIs_Mapped(&pd_Cap_خونه->cap, 1);
    poolPtr->array[ASID_ټیټ(asid)] =
        PDE_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(pd_Cap_خونه->cap));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDControlInvocation(فراغ *چوکاټ, cte_t *خونه,
                                                cte_t *پلار, asid_t asid_بنسټ)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سېټ_cap_Free_شاخص(&(پلار->cap),
                                         MAX_FREE_شاخص(cap_untyped_cap_get_cap_بلاک_اندازه(پلار->cap)));

    memzero(چوکاټ, 1 << ARMSmall_پاڼه_بټونه);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ورننباسه(cap_asid_pool_cap_new(asid_بنسټ, کلمه_REF(چوکاټ)),
              پلار, خونه);;
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنسټ & ماسک(asid_ټیټ_بټونه)) == 0);
    armKSASIDTable[ASID_لوړ(asid_بنسټ)] = (asid_pool_t *)چوکاټ;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeARMPageDirectoryInvocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    کلمه_t *بفر)
{
    بدل (invLabel) {
    حالة ARMPDClean_معلومات:
    حالة ARMPDInvalidate_معلومات:
    حالة ARMPDClean_Invalidate_معلومات:
    حالة ARMPDUnify_Instruction: {
        vptr_t پیل, پای_ټکی;
        paddr_t pstart;
        پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        إذا (اوږدتیا < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        پیل = getSyscallArg(0, بفر);
        پای_ټکی =   getSyscallArg(1, بفر);

        /* Check sanity of arguments */
        إذا (پای_ټکی <= پیل) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        إذا (پیل >= USER_سر || پای_ټکی > USER_سر) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(cap_get_capType(cap) != cap_پاڼه_directory_cap ||
                     !cap_پاڼه_directory_cap_get_cap_PDIs_Mapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(cap));
        asid = cap_پاڼه_directory_cap_get_cap_PDMapped_ASID(cap);

        پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(asid);
        إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(پیدا_کړه_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, پیل);

        /* Check that there's actually something there. */
        إذا (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
            أرجع EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        إذا (پاڼه_بنسټ(پیل, resolve_ret.چوکاټ_اندازه) !=
            پاڼه_بنسټ(پای_ټکی - 1, resolve_ret.چوکاټ_اندازه)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = پیل;
            current_syscall_error.rangeErrorMax =
                پاڼه_بنسټ(پیل, resolve_ret.چوکاټ_اندازه) +
                ماسک(پاڼه_بټونه_For_اندازه(resolve_ret.چوکاټ_اندازه));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.چوکاټ_بنسټ
                 + (پیل & ماسک(پاڼه_بټونه_For_اندازه(resolve_ret.چوکاټ_اندازه)));

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        إذا (pstart < phys_بنسټ() || pstart > PADDR_سر || (پای_ټکی - پیل) > PADDR_سر - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#د_که_پای

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performPDFlush(invLabel, pd, asid, پیل, پای_ټکی - 1, pstart);
    }

    افتراضي:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

}

ساكن exception_t decode_ARMPage_جدول_Invocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                                                cte_t *cte, cap_t cap, کلمه_t *بفر)
{
    کلمه_t vaddr, pd_شاخص;

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_ځانګړتیاوې_t attr;
#د_که_پای
    cap_t pdCap;
    pde_t *pd, *pd_خونه;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    إذا (invLabel == ARMPage_جدول_Unmap) {
        إذا (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely(invLabel != ARMPage_جدول_Map)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vm_ځانګړتیاوې_From_کلمه(getSyscallArg(1, بفر));
#د_که_پای
    pdCap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(cap_get_capType(pdCap) != cap_پاڼه_directory_cap ||
                 !cap_پاڼه_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(pdCap));
    asid = cap_پاڼه_directory_cap_get_cap_PDMapped_ASID(pdCap);

    إذا (unlikely(vaddr >= USER_سر)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_سر);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    {
        پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret;

        پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(asid);
        إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(پیدا_کړه_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_شاخص = vaddr >> (پاڼه_بټونه + PT_شاخص_بټونه);
    pd_خونه = &pd[pd_شاخص];
    إذا (unlikely(pde_ptr_get_pdeType(pd_خونه) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = se_L4_Delete_لومړی;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap)));
#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_ځانګړتیاوې_get_arm_Parity_Enabled(attr),
              0 /* Domain */
          );
#وإلا
    pde = pde_pde_coarse_new(paddr);
#د_که_پای

    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTIs_Mapped(cap, 1);
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTMapped_ASID(cap, asid);
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTMapped_پته(cap, vaddr);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_پاڼه_جدول_Invocation_Map(cap, cte, pde, pd_خونه);
}

ساكن exception_t decodeARMFrameInvocation(کلمه_t invLabel, کلمه_t اوږدتیا,
                                            cte_t *cte, cap_t cap, bool_t call, کلمه_t *بفر)
{
    بدل (invLabel) {
    حالة ARMPageMap: {
        کلمه_t vaddr, vtop, w_rights_ماسک;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_پاڼه_اندازه_t چوکاټ_اندازه;
        vm_ځانګړتیاوې_t attr;

        إذا (unlikely(اوږدتیا < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                se_L4_Truncated_پیغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, بفر);
        w_rights_ماسک = getSyscallArg(1, بفر);
        attr = vm_ځانګړتیاوې_From_کلمه(getSyscallArg(2, بفر));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        چوکاټ_اندازه = generic_چوکاټ_cap_get_cap_FSize(cap);
        capVMRights = generic_چوکاټ_cap_get_cap_FVMRights(cap);

        إذا (unlikely(cap_get_capType(pdCap) != cap_پاڼه_directory_cap ||
                     !cap_پاڼه_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(
                         pdCap));
        asid = cap_پاڼه_directory_cap_get_cap_PDMapped_ASID(pdCap);

        إذا (generic_چوکاټ_cap_get_cap_FIs_Mapped(cap)) {
            إذا (generic_چوکاټ_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_شمېره = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (generic_چوکاټ_cap_get_cap_FMapped_پته(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_شمېره = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            vtop = vaddr + بټ(پاڼه_بټونه_For_اندازه(چوکاټ_اندازه)) - 1;

            إذا (unlikely(vtop >= USER_سر)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_سر);
                current_syscall_error.type =
                    se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_شمېره = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            پیدا_کړه_PDFor_ASID_ret_t پیدا_کړه_ret;

            پیدا_کړه_ret = پیدا_کړه_PDFor_ASID(asid);
            إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_سرچینه =
                    false;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (unlikely(پیدا_کړه_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_شمېره = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            ماسک_VMRights(capVMRights, rights_From_کلمه(w_rights_ماسک));

        إذا (unlikely(!checkVPAlignment(چوکاټ_اندازه, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((فراغ *)
                                   generic_چوکاټ_cap_get_cap_FBase_Ptr(cap));

        cap = generic_چوکاټ_cap_سېټ_cap_FMapped_پته(cap, asid,
                                                      vaddr);
        إذا (چوکاټ_اندازه == ARMSmall_پاڼه || چوکاټ_اندازه == ARMLarge_پاڼه) {
            create_mappings_pte_return_t map_ret;
            map_ret = create_خوندي_Mapping_Entries_PTE(capFBasePtr, vaddr,
                                                   چوکاټ_اندازه, vmRights,
                                                   attr, pd);
            إذا (unlikely(map_ret.حالت != EXCEPTION_NONE)) {
#که_تعریف_شوی CONFIG_PRINTING
                إذا (current_syscall_error.type == se_L4_Delete_لومړی) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#د_که_پای
                أرجع map_ret.حالت;
            }

            سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_پاڼه_Invocation_Map_PTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } وإلا {
            create_mappings_pde_return_t map_ret;
            map_ret = create_خوندي_Mapping_Entries_PDE(capFBasePtr, vaddr,
                                                   چوکاټ_اندازه, vmRights,
                                                   attr, pd);
            إذا (unlikely(map_ret.حالت != EXCEPTION_NONE)) {
#که_تعریف_شوی CONFIG_PRINTING
                إذا (current_syscall_error.type == se_L4_Delete_لومړی) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#د_که_پای
                أرجع map_ret.حالت;
            }

            سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_پاڼه_Invocation_Map_PDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    حالة ARMPageUnmap: {
#که_تعریف_شوی CONFIG_TK1_SMMU
        إذا (is_IOSpace_چوکاټ_Cap(cap)) {
            سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_پاڼه_Invocation_Unmap_IO(cap, cte);
        } وإلا
#د_که_پای
        {
            سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_پاڼه_Invocation_Unmap(cap, cte);
        }
    }

#که_تعریف_شوی CONFIG_TK1_SMMU
    حالة ARMPageMapIO: {
        أرجع decodeARMIOMapInvocation(invLabel, اوږدتیا, cte, cap, بفر);
    }
#د_که_پای

    حالة ARMPage_Clean_معلومات:
    حالة ARMPage_Invalidate_معلومات:
    حالة ARMPage_Clean_Invalidate_معلومات:
    حالة ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        پیدا_کړه_PDFor_ASID_ret_t pd;
        vptr_t پیل, پای_ټکی;
        paddr_t pstart;
        کلمه_t پاڼه_اندازه;
        کلمه_t پاڼه_بنسټ_2;

        إذا (اوږدتیا < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_چوکاټ_cap_get_cap_FMapped_ASID(cap);
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_چوکاټ_cap_get_cap_FBase_Ptr(cap);
#وإلا
        vaddr = generic_چوکاټ_cap_get_cap_FMapped_پته(cap);
#د_که_پای

        إذا (unlikely(!generic_چوکاټ_cap_get_cap_FIs_Mapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pd = پیدا_کړه_PDFor_ASID(asid);
        إذا (unlikely(pd.حالت != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        پیل = getSyscallArg(0, بفر);
        پای_ټکی =   getSyscallArg(1, بفر);

        /* check that the range is sane */
        إذا (پای_ټکی <= پیل) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        پاڼه_اندازه = 1 << پاڼه_بټونه_For_اندازه(generic_چوکاټ_cap_get_cap_FSize(cap));
        پاڼه_بنسټ_2 = addrFromPPtr((فراغ *)generic_چوکاټ_cap_get_cap_FBase_Ptr(cap));

#که_نه_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        إذا (unlikely(!resolve_ret.valid ||
                     resolve_ret.چوکاټ_اندازه != generic_چوکاټ_cap_get_cap_FSize(cap) ||
                     resolve_ret.چوکاټ_بنسټ != پاڼه_بنسټ_2)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#د_که_پای

        إذا (پیل >= پاڼه_اندازه || پای_ټکی > پاڼه_اندازه) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = پاڼه_بنسټ_2 + پیل;
        پیل += vaddr;
        پای_ټکی += vaddr;

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        إذا (pstart < phys_بنسټ() || pstart > PADDR_سر || (پای_ټکی - پیل) > PADDR_سر - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#د_که_پای

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_Flush(invLabel, pd.pd, asid, پیل, پای_ټکی - 1, pstart);
    }

    حالة ARMPage_Get_پته: {


        /* Check that there are enough message registers */
        أكد(n_پیغام_Registers >= 1);

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_Get_پته((فراغ *)generic_چوکاټ_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(کلمه_t invLabel, کلمه_t اوږدتیا, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, کلمه_t *بفر)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_پاڼه_directory_cap:
        أرجع decodeARMPageDirectoryInvocation(invLabel, اوږدتیا, cptr, cte,
                                                cap, بفر);

    حالة cap_پاڼه_جدول_cap:
        أرجع decode_ARMPage_جدول_Invocation(invLabel, اوږدتیا, cte, cap, بفر);

    حالة cap_small_چوکاټ_cap:
    حالة cap_چوکاټ_cap:
        أرجع decodeARMFrameInvocation(invLabel, اوږدتیا, cte, cap, call, بفر);

    حالة cap_asid_control_cap: {
        کلمه_t i;
        asid_t asid_بنسټ;
        کلمه_t شاخص, ژوروالی;
        cap_t untyped, ریښه;
        cte_t *پلار_خونه, *dest_خونه;
        lookup_خونه_ret_t lu_ret;
        فراغ *چوکاټ;
        exception_t حالت;

        إذا (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم
                     || current_extra_caps.excaprefs[1] == عدم)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شاخص = getSyscallArg(0, بفر);
        ژوروالی = getSyscallArg(1, بفر);
        پلار_خونه = current_extra_caps.excaprefs[0];
        untyped = پلار_خونه->cap;
        ریښه = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        إذا (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_لومړی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنسټ = i << asid_ټیټ_بټونه;

        إذا (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_بلاک_اندازه(untyped) !=
                     se_L4_ASIDPool_بټونه || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(پلار_خونه);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            أرجع حالت;
        }

        چوکاټ = کلمه_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_موخه_خونه(ریښه, شاخص, ژوروالی);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            أرجع lu_ret.حالت;
        }
        dest_خونه = lu_ret.خونه;

        حالت = ensure_تشه_خونه(dest_خونه);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            أرجع حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکاټ, dest_خونه,
                                            پلار_خونه, asid_بنسټ);
    }

    حالة cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pd_Cap_خونه;
        asid_pool_t *pool;
        کلمه_t i;
        asid_t asid;

        إذا (unlikely(invLabel != ARMASIDPool_ټاکل)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pd_Cap_خونه = current_extra_caps.excaprefs[0];
        pdCap = pd_Cap_خونه->cap;

        إذا (unlikely(
                cap_get_capType(pdCap) != cap_پاڼه_directory_cap ||
                cap_پاڼه_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_لوړ(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            current_lookup_fault = lookup_fault_invalid_ریښه_new();

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < (1 << asid_ټیټ_بټونه) && (asid + i == 0 || pool->array[i]); i++);

        إذا (unlikely(i == 1 << asid_ټیټ_بټونه)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = se_L4_Delete_لومړی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, pd_Cap_خونه);
    }

    افتراضي:
        fail("Invalid ARM arch cap type");
    }
}

#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
exception_t benchmark_arch_map_log_بفر(کلمه_t چوکاټ_cptr)
{
    lookup_Cap_And_خونه_ret_t lu_ret;
    vm_پاڼه_اندازه_t چوکاټ_اندازه;
    pptr_t  چوکاټ_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_خونه(غوټه_STATE(ksCurThread), چوکاټ_cptr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکاټ_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکاټ_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_چوکاټ_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکاټ_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکاټ_اندازه = generic_چوکاټ_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکاټ_اندازه != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکاټ_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکاټ_pptr = generic_چوکاټ_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکاټ_pptr);

    لكل (صحيح idx = 0; idx < بټ(PT_شاخص_بټونه); idx++) {
        paddr_t physical_پته = ks_User_Log_بفر + (idx << se_L4_پاڼه_بټونه);

        armKSGlobalLogPT[idx] =
            pte_pte_small_new(
                physical_پته,
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
        invalidateTranslationSingle(KS_LOG_PPTR + (idx * بټ(se_L4_پاڼه_بټونه)));
    }

    أرجع EXCEPTION_NONE;
}
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */

#که_تعریف_شوی CONFIG_DEBUG_BUILD
فراغ kernelPrefetchAbort(کلمه_t pc, کلمه_t lr) VISIBLE;
فراغ kernel_معلومات_Abort(کلمه_t pc) VISIBLE;

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT

فراغ kernelUndefinedInstruction(کلمه_t pc) VISIBLE;

فراغ kernelPrefetchAbort(کلمه_t pc, کلمه_t lr)
{
    چاپ_کړه("\n\nKERNEL PREFETCH ABORT!\n");
    چاپ_کړه("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کړه("HSR: 0x%"SEL4_PRIx_کلمه"\n", getHSR());
    چاپ_کړه("LR: 0x%"SEL4_PRIx_کلمه"\n", lr);
    halt();
}

فراغ kernel_معلومات_Abort(کلمه_t pc)
{
    چاپ_کړه("\n\nKERNEL DATA ABORT!\n");
    چاپ_کړه("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کړه("HDFAR: 0x%"SEL4_PRIx_کلمه" HSR: 0x%"SEL4_PRIx_کلمه"\n",
           getHDFAR(), getHSR());
    halt();
}

فراغ kernelUndefinedInstruction(کلمه_t pc)
{
    چاپ_کړه("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    چاپ_کړه("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کړه("HSR: 0x%"SEL4_PRIx_کلمه"\n", getHSR());
    halt();
}

#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */

فراغ kernelPrefetchAbort(کلمه_t pc, کلمه_t lr)
{
    چاپ_کړه("\n\nKERNEL PREFETCH ABORT!\n");
    چاپ_کړه("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کړه("IFSR: 0x%"SEL4_PRIx_کلمه"\n", getIFSR());
    چاپ_کړه("LR: 0x%"SEL4_PRIx_کلمه"\n", lr);
    halt();
}

فراغ kernel_معلومات_Abort(کلمه_t pc)
{
    چاپ_کړه("\n\nKERNEL DATA ABORT!\n");
    چاپ_کړه("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کړه("FAR: 0x%"SEL4_PRIx_کلمه" DFSR: 0x%"SEL4_PRIx_کلمه"\n",
           getFAR(), getDFSR());
    halt();
}
#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#د_که_پای

#که_تعریف_شوی CONFIG_PRINTING
تعريفنوع بنية read_کلمه_From_VSpace_ret {
    exception_t حالت;
    کلمه_t ارزښت;
} read_کلمه_From_VSpace_ret_t;

ساكن read_کلمه_From_VSpace_ret_t read_کلمه_From_VSpace(pde_t *pd, کلمه_t vaddr)
{
    read_کلمه_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_خونه;
    pde_t *pd_خونه;
    paddr_t paddr;
    کلمه_t دننه_واټن;
    pptr_t kernel_vaddr;
    کلمه_t *ارزښت;

    pd_خونه = lookupPDSlot(pd, vaddr);
    إذا (pde_ptr_get_pdeType(pd_خونه) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_پته(pd_خونه);
        دننه_واټن = vaddr & ماسک(ARMSection_بټونه);
    } وإلا {
        pt_خونه = lookupPTSlot(pd, vaddr);
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (pt_خونه.حالت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_خونه.pt_خونه) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_پته(pt_خونه.pt_خونه);
            إذا (pte_pte_small_ptr_get_contiguous_hint(pt_خونه.pt_خونه)) {
                دننه_واټن = vaddr & ماسک(ARMLarge_پاڼه_بټونه);
            } وإلا {
                دننه_واټن = vaddr & ماسک(ARMSmall_پاڼه_بټونه);
            }
#وإلا
        إذا (pt_خونه.حالت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_خونه.pt_خونه) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_پته(pt_خونه.pt_خونه);
            دننه_واټن = vaddr & ماسک(ARMSmall_پاڼه_بټونه);
        } وإلا إذا (pt_خونه.حالت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_خونه.pt_خونه) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_پته(pt_خونه.pt_خونه);
            دننه_واټن = vaddr & ماسک(ARMLarge_پاڼه_بټونه);
#د_که_پای
        } وإلا {
            ret.حالت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }
    }


    kernel_vaddr = (کلمه_t)paddr_منزل_pptr(paddr);
    ارزښت = (کلمه_t *)(kernel_vaddr + دننه_واټن);
    ret.حالت = EXCEPTION_NONE;
    ret.ارزښت = *ارزښت;
    أرجع ret;
}

فراغ Arch_user_ډېری_Trace(tcb_t *tptr)
{
    cap_t thread_ریښه;
    pde_t *pd;
    کلمه_t sp;
    صحيح i;

    thread_ریښه = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    إذا (cap_get_capType(thread_ریښه) != cap_پاڼه_directory_cap) {
        چاپ_کړه("Invalid vspace\n");
        أرجع;
    }

    pd = (pde_t *)pptr_د_cap(thread_ریښه);

    sp = get_ثبتوونکی(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_کلمه_اندازه_بټونه)) {
        چاپ_کړه("SP not aligned\n");
        أرجع;
    }

    لكل (i = 0; i < CONFIG_USER_ډېری_TRACE_اوږدتیا; i++) {
        کلمه_t پته = sp + (i * حجم(کلمه_t));
        read_کلمه_From_VSpace_ret_t پایله;
        پایله = read_کلمه_From_VSpace(pd, پته);
        إذا (پایله.حالت == EXCEPTION_NONE) {
            چاپ_کړه("0x%lx: 0x%lx\n", (اوږد)پته, (اوږد)پایله.ارزښت);
        } وإلا {
            چاپ_کړه("0x%lx: INVALID\n", (اوږد)پته);
        }
    }
}
#د_که_پای
