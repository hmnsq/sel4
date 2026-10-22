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

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ضمن <benchmark/benchmark_track.h>
#جيڪڏهن_پورو

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#وصف_ڪريو SECTIONS_في_SUPER_SECTION ٻٽو_انگ(ARMSuper_Section_بٽ - ARMSection_بٽ)
#وصف_ڪريو صفحا_في_LARGE_صفحو ٻٽو_انگ(ARMLarge_صفحو_بٽ - ARMSmall_صفحو_بٽ)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#وصف_ڪريو آخري_BYTE_PTE(PTE,ڊيگهائي_2) ((لفظ_t)&(PTE)[(ڊيگهائي_2)-1] + (ٻٽو_انگ(PTE_سائيز_بٽ)-1))
#وصف_ڪريو آخري_BYTE_PDE(PDE,ڊيگهائي_2) ((لفظ_t)&(PDE)[(ڊيگهائي_2)-1] + (ٻٽو_انگ(PDE_سائيز_بٽ)-1))

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#وصف_ڪريو MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#وصف_ڪريو MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#وصف_ڪريو ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#وصف_ڪريو ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ساكن فراغ سيٽ_Global_PD(فراغ);

بنية resolve_ret {
    paddr_t چوکٺو_بنياد;
    vm_صفحو_سائيز_t چوکٺو_سائيز;
    bool_t valid;
};
تعريفنوع بنية resolve_ret resolve_ret_t;

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
ساكن bool_t خالص pteCheckIfMapped(pte_t *pte);
ساكن bool_t خالص pdeCheckIfMapped(pde_t *pde);

ساكن لفظ_t CONST APFromVMRights(vm_rights_t vm_rights)
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
BOOT_ڪوڊ
ساكن لفظ_t CONST APFromVMRights(vm_rights_t vm_rights)
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

ساكن لفظ_t CONST HAPFromVMRights(vm_rights_t vm_rights)
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

#جيڪڏهن_پورو

vm_rights_t CONST نقاب_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_نقاب)
{
    إذا (vm_rights == VMNoAccess) {
        أرجع VMNoAccess;
    }
    إذا (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        أرجع VMReadOnly;
    }
    إذا (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        إذا (!seL4_CapRights_get_capAllowWrite(cap_rights_نقاب)) {
            أرجع VMReadOnly;
        } وإلا {
            أرجع VMReadWrite;
        }
    }
    إذا (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_نقاب) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_نقاب)) {
        userError("Attempted to make unsupported write only mapping");
    }
    أرجع VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_ڪوڊ فراغ map_kernel_چوکٺو(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_خاصيتون_t خاصيتون)
{
    لفظ_t idx = (vaddr & نقاب(صفحو_بٽ_For_سائيز(ARMSection))) >> صفحو_بٽ_For_سائيز(ARMSmall_صفحو);

    أكد(vaddr >= PPTR_چوٽي); /* vaddr lies in the region the global PT covers */
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t tex;
    إذا (vm_خاصيتون_get_arm_صفحو_Cacheable(خاصيتون)) {
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
            vm_خاصيتون_get_arm_صفحو_Cacheable(خاصيتون)
        );
#جيڪڏهن_پورو /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_ڪوڊ فراغ map_kernel_دري(فراغ)
{
    paddr_t  phys;
    لفظ_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = phys_بنياد();
    idx = PPTR_بنياد >> صفحو_بٽ_For_سائيز(ARMSection);

    طالما (idx < ٻٽو_انگ(PD_اشاريو_بٽ) - SECTIONS_في_SUPER_SECTION) {
        لفظ_t idx2;

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
        لكل (idx2 = idx; idx2 < idx + SECTIONS_في_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += ٻٽو_انگ(صفحو_بٽ_For_سائيز(ARMSuperSection));
        idx += SECTIONS_في_SUPER_SECTION;
    }

    طالما (idx < (PPTR_چوٽي >> صفحو_بٽ_For_سائيز(ARMSection))) {
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
        phys += ٻٽو_انگ(صفحو_بٽ_For_سائيز(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    أكد(phys == PADDR_چوٽي);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /*پتو*/
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, ٻٽو_انگ(se_L4_صفحو_جدول_بٽ));

    phys += ٻٽو_انگ(صفحو_بٽ_For_سائيز(ARMSection));
    idx++;
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */

    /* map page table covering last 1M of virtual address space to page directory */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalPT), /*پتو*/
            true,                        /* P       */
            0                            /* Domain  */
        );

    /* now start initialising the page table */
    memzero(armKSGlobalPT, 1 << se_L4_صفحو_جدول_بٽ);

    /* map vector table */
    map_kernel_چوکٺو(
        addrFromKPPtr(arm_ويڪٽر_جدول),
        PPTR_ويڪٽر_جدول,
        VMKernelOnly,
        vm_خاصيتون_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_ڪوڊ فراغ map_kernel_دري(فراغ)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t اڻ_استعمال_ٿيل pteS2;

    /* Initialise PGD */
    لكل (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    لكل (idx = 0; idx < (USER_چوٽي - 0xC0000000) >> (PT_اشاريو_بٽ + صفحو_بٽ); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = phys_بنياد();
    لكل (; idx < ٻٽو_انگ(PT_اشاريو_بٽ) - 1; idx++) {
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
        phys += ٻٽو_انگ(PT_اشاريو_بٽ + صفحو_بٽ);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << se_L4_صفحو_جدول_بٽ);
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
        phys += ٻٽو_انگ(صفحو_بٽ);
    }
    /* map vector table */
    map_kernel_چوکٺو(
        addrFromKPPtr(arm_ويڪٽر_جدول),
        PPTR_ويڪٽر_جدول,
        VMKernelOnly,
        vm_خاصيتون_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#جيڪڏهن_پورو /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

ساكن BOOT_ڪوڊ فراغ map_it_چوکٺو_cap(cap_t pd_cap, cap_t چوکٺو_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *هدف_جڳهه;
    pde_t *pd    = PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    فراغ  *چوکٺو = (فراغ *)generic_چوکٺو_cap_get_cap_FBase_Ptr(چوکٺو_cap);
    vptr_t vptr  = generic_چوکٺو_cap_get_cap_FMapped_پتو(چوکٺو_cap);

    أكد(generic_چوکٺو_cap_get_cap_FMapped_ASID(چوکٺو_cap) != 0);

    pd += (vptr >> صفحو_بٽ_For_سائيز(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتو(pd));
    هدف_جڳهه = pt + ((vptr & نقاب(صفحو_بٽ_For_سائيز(ARMSection)))
                       >> صفحو_بٽ_For_سائيز(ARMSmall_صفحو));
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    *هدف_جڳهه = pte_pte_small_new(
                      addrFromPPtr(چوکٺو),
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
    *هدف_جڳهه = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(چوکٺو),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#جيڪڏهن_پورو
}

/* Create a frame cap for the initial thread. */

ساكن BOOT_ڪوڊ cap_t create_it_چوکٺو_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    إذا (use_large)
        أرجع
            cap_چوکٺو_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_هيٺيون(asid),                /* capFMappedASIDLow  */
                لفظ_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_مٿاهون(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    وإلا
        أرجع
            cap_small_چوکٺو_cap_new(
                ASID_هيٺيون(asid),                /* capFMappedASIDLow  */
                لفظ_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#جيڪڏهن_پورو
                ASID_مٿاهون(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

ساكن BOOT_ڪوڊ فراغ map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(pt_cap);
    pde_t *هدف_جڳهه = pd + (vptr >> صفحو_بٽ_For_سائيز(ARMSection));

    أكد(cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(pt_cap));

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    *هدف_جڳهه = pde_pde_coarse_new(
                      addrFromPPtr(pt), /*پتو*/
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#وإلا
    *هدف_جڳهه = pde_pde_coarse_new(addrFromPPtr(pt));
#جيڪڏهن_پورو
}

/* Create a page table for the initial thread */

ساكن BOOT_ڪوڊ cap_t create_it_صفحو_جدول_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحو_جدول_cap_new(
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

BOOT_ڪوڊ لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    أرجع get_n_paging(it_v_reg, PT_اشاريو_بٽ + صفحو_بٽ);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_ڪوڊ cap_t create_it_پتو_space(cap_t پاڙ_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    se_L4_جڳهه_Pos جڳهه_pos_before;
    se_L4_جڳهه_Pos جڳهه_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << se_L4_صفحو_Dir_بٽ) - 1,
                        addrFromPPtr((فراغ *)rootserver.vspace));
    cap_t pd_cap =
        cap_صفحو_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    جڳهه_pos_before = ndks_boot.جڳهه_pos_cur;
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    لكل (pt_vptr = ڦيرو_هيٺ_ڏانهن(it_v_reg.شروعات, PT_اشاريو_بٽ + صفحو_بٽ);
         pt_vptr < it_v_reg.پڇاڙي;
         pt_vptr += ٻٽو_انگ(PT_اشاريو_بٽ + صفحو_بٽ)) {
        إذا (!provide_cap(پاڙ_cnode_cap,
                         create_it_صفحو_جدول_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    جڳهه_pos_after = ndks_boot.جڳهه_pos_cur;
    ndks_boot.bi_چوکٺو->userImagePaging = (se_L4_جڳهه_Region) {
        جڳهه_pos_before, جڳهه_pos_after
    };

    أرجع pd_cap;
}

BOOT_ڪوڊ cap_t create_unmapped_it_چوکٺو_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_چوکٺو_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_ڪوڊ cap_t create_mapped_it_چوکٺو_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_چوکٺو_cap(pptr, vptr, asid, use_large);
    map_it_چوکٺو_cap(pd_cap, cap, executable);
    أرجع cap;
}

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_ڪوڊ فراغ activate_kernel_vspace(فراغ)
{
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    سيٽ_Global_PD();
    invalidateLocalTLB();
    تالو_TLBEntry(PPTR_بنياد);
    تالو_TLBEntry(PPTR_ويڪٽر_جدول);
}

#وإلا

BOOT_ڪوڊ فراغ activate_kernel_vspace(فراغ)
{
    uint32_t r;
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    /* Setup the memory attributes: We use 2 indices (cachable/non-cacheable) */
    سيٽ_HMAIR((ATTRINDX_NONCACHEABLE << 0) | (ATTRINDX_CACHEABLE << 8), 0);
    سيٽ_Current_Hyp_PD(addrFromKPPtr(armHSGlobalPGD));
    invalidateHypTLB();
#إذا 0 /* Can't lock entries on A15 */
    تالو_TLBEntry(PPTR_بنياد);
    تالو_TLBEntry(PPTR_ويڪٽر_جدول);
#جيڪڏهن_پورو
    /* TODO find a better place to init the VMMU */
    r = 0;
    /* Translation range */
    r |= (0x0 << 0);     /* 2^(32 -(0)) input range. */
    r |= (r & 0x8) << 1; /* Sign bit */
    /* starting level */
    r |= (0x0 << 6);     /* Start at second level */
    /* Sharability of tables */
    r |= ٻٽو_انگ(8);       /* Inner write-back, write-allocate */
    r |= ٻٽو_انگ(10);      /* Outer write-back, write-allocate */
    /* Long descriptor format (not that we have a choice) */
    r |= ٻٽو_انگ(31);
    سيٽ_VTCR(r);
}

#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_ڪوڊ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_جو_cap(it_ap_cap));
    ap->array[ASID_هيٺيون(IT_ASID)] = PDE_PTR(pptr_جو_cap(it_pd_cap));
    armKSASIDTable[ASID_مٿاهون(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ڳوليو_PDFor_ASID_ret_t ڳوليو_PDFor_ASID(asid_t asid)
{
    ڳوليو_PDFor_ASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_مٿاهون(asid)];
    إذا (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_پاڙ_new();

        ret.pd = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    pd = poolPtr->array[ASID_هيٺيون(asid)];
    إذا (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_پاڙ_new();

        ret.pd = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.pd = pd;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

لفظ_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    لفظ_t w_بفر_Ptr;
    cap_t بفر_Cap;
    vm_rights_t vm_rights;

    w_بفر_Ptr = thread->tcbIPCBuffer;
    بفر_Cap = TCB_PTR_CTE_PTR(thread, tcb_بفر)->cap;

    إذا (unlikely(cap_get_capType(بفر_Cap) != cap_small_چوکٺو_cap &&
                 cap_get_capType(بفر_Cap) != cap_چوکٺو_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(generic_چوکٺو_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = generic_چوکٺو_cap_get_cap_FVMRights(بفر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        لفظ_t بنياد_Ptr;
        بنانشان صحيح صفحو_بٽ_2;

        بنياد_Ptr = generic_چوکٺو_cap_get_cap_FBase_Ptr(بفر_Cap);
        صفحو_بٽ_2 = صفحو_بٽ_For_سائيز(generic_چوکٺو_cap_get_cap_FSize(بفر_Cap));
        أرجع (لفظ_t *)(بنياد_Ptr + (w_بفر_Ptr & نقاب(صفحو_بٽ_2)));
    } وإلا {
        أرجع عدم;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (unlikely(cap_get_capType(cap) != cap_small_چوکٺو_cap &&
                 cap_get_capType(cap) != cap_چوکٺو_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(generic_چوکٺو_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(vptr & نقاب(se_L4_IPCBuffer_سائيز_بٽ))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    بنانشان صحيح pd_اشاريو;

    pd_اشاريو = vptr >> (صفحو_بٽ + PT_اشاريو_بٽ);
    أرجع pd + pd_اشاريو;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pd_جڳهه;

    pd_جڳهه = lookupPDSlot(pd, vptr);

    إذا (unlikely(pde_ptr_get_pdeType(pd_جڳهه) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_اشاريو_بٽ + صفحو_بٽ);

        ret.pt_جڳهه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pte_t *pt, *pt_جڳهه;
        بنانشان صحيح pt_اشاريو;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتو(pd_جڳهه));
        pt_اشاريو = (vptr >> صفحو_بٽ) & نقاب(PT_اشاريو_بٽ);
        pt_جڳهه = pt + pt_اشاريو;

        ret.pt_جڳهه = pt_جڳهه;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    }
}

ساكن pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    بنانشان صحيح pt_اشاريو;

    pt_اشاريو = (vptr >> صفحو_بٽ) & نقاب(PT_اشاريو_بٽ);
    أرجع pt + pt_اشاريو;
}

ساكن ثابت resolve_ret_t default_resolve_ret_t;

ساكن resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    بدل (pde_ptr_get_pdeType(pde)) {
    حالة pde_pde_section:
        ret.چوکٺو_بنياد = pde_pde_section_ptr_get_پتو(pde);
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (pde_pde_section_ptr_get_سائيز(pde)) {
            ret.چوکٺو_سائيز = ARMSuperSection;
        } وإلا {
            ret.چوکٺو_سائيز = ARMSection;
        }
#وإلا
        إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.چوکٺو_بنياد &= ~نقاب(صفحو_بٽ_For_سائيز(ARMSuperSection));
            ret.چوکٺو_سائيز = ARMSuperSection;
        } وإلا {
            ret.چوکٺو_سائيز = ARMSection;
        }
#جيڪڏهن_پورو
        أرجع ret;

    حالة pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتو(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small:
            ret.چوکٺو_بنياد = pte_pte_small_ptr_get_پتو(pte);
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.چوکٺو_بنياد &= ~نقاب(صفحو_بٽ_For_سائيز(ARMLarge_صفحو));
                ret.چوکٺو_سائيز = ARMLarge_صفحو;
            } وإلا {
                ret.چوکٺو_سائيز = ARMSmall_صفحو;
            }
#وإلا
            ret.چوکٺو_سائيز = ARMSmall_صفحو;
#جيڪڏهن_پورو
            أرجع ret;

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large:
            ret.چوکٺو_بنياد = pte_pte_large_ptr_get_پتو(pte);
            ret.چوکٺو_سائيز = ARMLarge_صفحو;
            أرجع ret;
#جيڪڏهن_پورو
        افتراضي:
            اكسر;
        }
        اكسر;
    }
    }

    ret.valid = false;
    أرجع ret;
}

ساكن pte_t CONST makeUserPTE(vm_صفحو_سائيز_t صفحو_سائيز, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t ap;

    ap = APFromVMRights(vm_rights);

    بدل (صفحو_سائيز) {
    حالة ARMSmall_صفحو: {
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

    حالة ARMLarge_صفحو: {
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

    لفظ_t hap;

    hap = HAPFromVMRights(vm_rights);

    بدل (صفحو_سائيز) {
    حالة ARMSmall_صفحو: {
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

    حالة ARMLarge_صفحو: {
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
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    أرجع pte;
}

ساكن pde_t CONST makeUserPDE(vm_صفحو_سائيز_t صفحو_سائيز, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, لفظ_t domain,
                               vm_rights_t vm_rights)
{
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t ap, size2;

    ap = APFromVMRights(vm_rights);
#وإلا
    لفظ_t hap, size2;

    (فراغ)domain;
    hap = HAPFromVMRights(vm_rights);
#جيڪڏهن_پورو

    بدل (صفحو_سائيز) {
    حالة ARMSection:
        size2 = 0;
        اكسر;

    حالة ARMSuperSection:
        size2 = 1;
        اكسر;

    افتراضي:
        fail("Invalid PDE frame type");
    }

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
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
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_پاڙ(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_صفحو_directory_cap &&
           cap_صفحو_directory_cap_get_cap_PDIs_Mapped(cap);
}

bool_t CONST is_IOSpace_چوکٺو_Cap(cap_t cap)
{
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    أرجع cap_get_capType(cap) == cap_small_چوکٺو_cap && cap_small_چوکٺو_cap_get_cap_FIs_IOSpace(cap);
#وإلا
    أرجع false;
#جيڪڏهن_پورو
}

/** Switch to the empty PD on the reserved HW ASID */
ساكن فراغ سيٽ_Global_PD(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
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
    سيٽ_Hardware_ASID(hwASIDReserved);
#جيڪڏهن_پورو
}

فراغ سيٽ_VMRoot(tcb_t *tcb)
{
    cap_t thread_پاڙ;
    asid_t asid;
    pde_t *pd;
    ڳوليو_PDFor_ASID_ret_t ڳوليو_ret;

    thread_پاڙ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_پاڙ) != cap_صفحو_directory_cap ||
        !cap_صفحو_directory_cap_get_cap_PDIs_Mapped(thread_پاڙ)) {
        سيٽ_Global_PD();
        أرجع;
    }

    pd = PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(thread_پاڙ));
    asid = cap_صفحو_directory_cap_get_cap_PDMapped_ASID(thread_پاڙ);
    ڳوليو_ret = ڳوليو_PDFor_ASID(asid);
    إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE || ڳوليو_ret.pd != pd)) {
        سيٽ_Global_PD();
        أرجع;
    }

    armv_contextSwitch(pd, asid);
}

ساكن bool_t سيٽ_VMRoot_For_Flush(pde_t *pd, asid_t asid)
{
    cap_t thread_پاڙ;

    thread_پاڙ = TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbVTable)->cap;

    إذا (cap_get_capType(thread_پاڙ) == cap_صفحو_directory_cap &&
        cap_صفحو_directory_cap_get_cap_PDIs_Mapped(thread_پاڙ) &&
        PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(thread_پاڙ)) == pd) {
        أرجع false;
    }

    armv_contextSwitch(pd, asid);

    أرجع true;
}

pde_t *صفحو_جدول_Mapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    ڳوليو_PDFor_ASID_ret_t ڳوليو_ret;
    pde_t pde;
    بنانشان صحيح pd_اشاريو;

    ڳوليو_ret = ڳوليو_PDFor_ASID(asid);
    إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
        أرجع عدم;
    }

    pd_اشاريو = vaddr >> (صفحو_بٽ + PT_اشاريو_بٽ);
    pde = ڳوليو_ret.pd[pd_اشاريو];

    إذا (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_پتو(pde)) == pt)) {
        أرجع ڳوليو_ret.pd;
    } وإلا {
        أرجع عدم;
    }
}

ساكن فراغ invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_مٿاهون(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_هيٺيون(asid)];
    أكد(pd);

    pd[PD_ASID_جڳهه] = pde_pde_invalid_new(0, false);
}

ساكن pde_t خالص ڀريو_HWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_مٿاهون(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_هيٺيون(asid)];
    أكد(pd);

    أرجع pd[PD_ASID_جڳهه];
}

ساكن فراغ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_مٿاهون(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_هيٺيون(asid)];
    أكد(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_جڳهه] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t ڳوليو_Free_HWASID(فراغ)
{
    لفظ_t hw_asid_وٿي;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    لكل (hw_asid_وٿي = 0;
         hw_asid_وٿي <= (لفظ_t)((hw_asid_t) - 1);
         hw_asid_وٿي ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_وٿي);
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

    stored_hw_asid = ڀريو_HWASID(asid);
    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } وإلا {
        hw_asid_t new_hw_asid;

        new_hw_asid = ڳوليو_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        أرجع new_hw_asid;
    }
}

ساكن فراغ invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ڀريو_HWASID(asid);
    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

فراغ unmap_صفحو_جدول(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pd_جڳهه;
    بنانشان صحيح pd_اشاريو;

    pd = صفحو_جدول_Mapped(asid, vaddr, pt);

    إذا (likely(pd != عدم)) {
        pd_اشاريو = vaddr >> (PT_اشاريو_بٽ + صفحو_بٽ);
        pd_جڳهه = pd + pd_اشاريو;

        *pd_جڳهه = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((لفظ_t)pd_جڳهه, addrFromPPtr(pd_جڳهه));
        flush_جدول(pd, asid, vaddr, pt);
    }
}

فراغ copyGlobalMappings(pde_t *newPD)
{
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t i;
    pde_t *global_pd = armKSGlobalPD;

    لكل (i = PPTR_بنياد >> ARMSection_بٽ; i < ٻٽو_انگ(PD_اشاريو_بٽ); i++) {
        إذا (i != PD_ASID_جڳهه) {
            newPD[i] = global_pd[i];
        }
    }
#جيڪڏهن_پورو
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    بدل (vm_faultType) {
    حالة ARMDataAbort: {
        لفظ_t addr, fault;

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (پتو_Translate_S1(addr) & ~نقاب(صفحو_بٽ)) | (addr & نقاب(صفحو_بٽ));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#وإلا
        addr = getFAR();
        fault = getDFSR();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
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
#جيڪڏهن_پورو
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        أرجع EXCEPTION_FAULT;
    }

    حالة ARMPrefetchAbort: {
        لفظ_t pc, fault;

        pc = getRestartPC(thread);

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (پتو_Translate_S1(pc) & ~نقاب(صفحو_بٽ)) | (pc & نقاب(صفحو_بٽ));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#وإلا
        fault = getIFSR();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
        إذا (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            إذا (seL4_Fault_DebugException_get_exceptionReason(current_fault) == se_L4_Single_قدم
                && !single_قدم_Fault_Counter_Ready(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                أرجع EXCEPTION_NONE;
            }
            أرجع EXCEPTION_FAULT;
        }
#جيڪڏهن_پورو
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        أرجع EXCEPTION_FAULT;
    }

    افتراضي:
        fail("Invalid VM fault type");
    }
}

فراغ deleteASIDPool(asid_t asid_بنياد, asid_pool_t *pool)
{
    بنانشان صحيح وٿي;

    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنياد & نقاب(asid_هيٺيون_بٽ)) == 0);

    إذا (armKSASIDTable[ASID_مٿاهون(asid_بنياد)] == pool) {
        لكل (وٿي = 0; وٿي < ٻٽو_انگ(asid_هيٺيون_بٽ); وٿي++) {
            إذا (pool->array[وٿي]) {
                flushSpace(asid_بنياد + وٿي);
                invalidateASIDEntry(asid_بنياد + وٿي);
            }
        }
        armKSASIDTable[ASID_مٿاهون(asid_بنياد)] = عدم;
        سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
    }
}

فراغ deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_مٿاهون(asid)];

    إذا (poolPtr != عدم && poolPtr->array[ASID_هيٺيون(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_هيٺيون(asid)] = عدم;
        سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
    }
}

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
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
#جيڪڏهن_پورو

فراغ unmap_صفحو(vm_صفحو_سائيز_t صفحو_سائيز, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    ڳوليو_PDFor_ASID_ret_t ڳوليو_ret;
    paddr_t addr = addrFromPPtr(pptr);

    ڳوليو_ret = ڳوليو_PDFor_ASID(asid);
    إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
        أرجع;
    }

    بدل (صفحو_سائيز) {
    حالة ARMSmall_صفحو: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(ڳوليو_ret.pd, vptr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            أرجع;
        }

        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_جڳهه) != pte_pte_small)) {
            أرجع;
        }
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_جڳهه) != 0)) {
            أرجع;
        }
#جيڪڏهن_پورو
        إذا (unlikely(pte_pte_small_ptr_get_پتو(lu_ret.pt_جڳهه) != addr)) {
            أرجع;
        }

        *(lu_ret.pt_جڳهه) = pte_pte_invalid_new();
        cleanByVA_PoU((لفظ_t)lu_ret.pt_جڳهه, addrFromPPtr(lu_ret.pt_جڳهه));

        اكسر;
    }

    حالة ARMLarge_صفحو: {
        lookupPTSlot_ret_t lu_ret;
        لفظ_t i;

        lu_ret = lookupPTSlot(ڳوليو_ret.pd, vptr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            أرجع;
        }
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_جڳهه) != pte_pte_large)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_large_ptr_get_پتو(lu_ret.pt_جڳهه) != addr)) {
            أرجع;
        }
#وإلا
        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_جڳهه) != pte_pte_small)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_جڳهه) != 1)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_small_ptr_get_پتو(lu_ret.pt_جڳهه) != addr)) {
            أرجع;
        }
#جيڪڏهن_پورو

        لكل (i = 0; i < صفحا_في_LARGE_صفحو; i++) {
            lu_ret.pt_جڳهه[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((لفظ_t)&lu_ret.pt_جڳهه[0],
                            آخري_BYTE_PTE(lu_ret.pt_جڳهه, صفحا_في_LARGE_صفحو),
                            addrFromPPtr(&lu_ret.pt_جڳهه[0]));

        اكسر;
    }

    حالة ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(ڳوليو_ret.pd, vptr);

        إذا (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            أرجع;
        }
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pde_pde_section_ptr_get_سائيز(pd) != 0)) {
#وإلا
        إذا (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#جيڪڏهن_پورو
            أرجع;
        }
        إذا (unlikely(pde_pde_section_ptr_get_پتو(pd) != addr)) {
            أرجع;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((لفظ_t)pd, addrFromPPtr(pd));

        اكسر;
    }

    حالة ARMSuperSection: {
        pde_t *pd;
        لفظ_t i;

        pd = lookupPDSlot(ڳوليو_ret.pd, vptr);

        إذا (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            أرجع;
        }
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pde_pde_section_ptr_get_سائيز(pd) != 1)) {
#وإلا
        إذا (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#جيڪڏهن_پورو
            أرجع;
        }
        إذا (unlikely(pde_pde_section_ptr_get_پتو(pd) != addr)) {
            أرجع;
        }

        لكل (i = 0; i < SECTIONS_في_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((لفظ_t)&pd[0], آخري_BYTE_PDE(pd, SECTIONS_في_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        اكسر;
    }

    افتراضي:
        fail("Invalid ARM page type");
        اكسر;
    }

    /* Flush the page now that the mapping has been updated */
    flush_صفحو(صفحو_سائيز, ڳوليو_ret.pd, asid, vptr);
}

فراغ flush_صفحو(vm_صفحو_سائيز_t صفحو_سائيز, pde_t *pd, asid_t asid, لفظ_t vptr)
{
    pde_t stored_hw_asid;
    لفظ_t بنياد_addr;
    bool_t پاڙ_switched;

    أكد((vptr & نقاب(صفحو_بٽ_For_سائيز(صفحو_سائيز))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    پاڙ_switched = سيٽ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = ڀريو_HWASID(asid);

    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        بنياد_addr = vptr & ~نقاب(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(بنياد_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        إذا (پاڙ_switched) {
            سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
        }
    }
}

فراغ flush_جدول(pde_t *pd, asid_t asid, لفظ_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t پاڙ_switched;

    أكد((vptr & نقاب(PT_اشاريو_بٽ + ARMSmall_صفحو_بٽ)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    پاڙ_switched = سيٽ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = ڀريو_HWASID(asid);

    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        إذا (پاڙ_switched) {
            سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
        }
    }
}

فراغ flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ڀريو_HWASID(asid);

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

    stored_hw_asid = ڀريو_HWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    إذا (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ساكن مضمن bool_t CONST checkVPAlignment(vm_صفحو_سائيز_t sz, لفظ_t w)
{
    أرجع (w & نقاب(صفحو_بٽ_For_سائيز(sz))) == 0;
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
create_محفوظ_Mapping_Entries_PTE
(paddr_t بنياد, لفظ_t vaddr, vm_صفحو_سائيز_t چوکٺو_سائيز,
 vm_rights_t vmRights, vm_خاصيتون_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    لفظ_t i;

    بدل (چوکٺو_سائيز) {

    حالة ARMSmall_صفحو:

        ret.pte_entries.بنياد = عدم; /* to avoid uninitialised warning */
        ret.pte_entries.ڊيگهائي = 1;

        ret.pte = makeUserPTE(ARMSmall_صفحو, بنياد,
                              vm_خاصيتون_get_arm_صفحو_Cacheable(attr),
                              vm_خاصيتون_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو =
                false;
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            أرجع ret;
        }

        ret.pte_entries.بنياد = lu_ret.pt_جڳهه;
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_ptr_get_pteType(ret.pte_entries.بنياد) ==
                     pte_pte_large)) {
#وإلا
        إذا (unlikely(pte_ptr_get_pteType(ret.pte_entries.بنياد) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.بنياد))) {
#جيڪڏهن_پورو
            current_syscall_error.type =
                se_L4_Delete_پهريون;

            ret.حالت = EXCEPTION_SYSCALL_ERROR;
            أرجع ret;
        }

        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة ARMLarge_صفحو:

        ret.pte_entries.بنياد = عدم; /* to avoid uninitialised warning */
        ret.pte_entries.ڊيگهائي = صفحا_في_LARGE_صفحو;

        ret.pte = makeUserPTE(ARMLarge_صفحو, بنياد,
                              vm_خاصيتون_get_arm_صفحو_Cacheable(attr),
                              vm_خاصيتون_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو =
                false;
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            أرجع ret;
        }

        ret.pte_entries.بنياد = lu_ret.pt_جڳهه;

        لكل (i = 0; i < صفحا_في_LARGE_صفحو; i++) {
            إذا (unlikely(pte_get_pteType(ret.pte_entries.بنياد[i]) ==
                         pte_pte_small)
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.بنياد[i])
#جيڪڏهن_پورو
               ) {
                current_syscall_error.type =
                    se_L4_Delete_پهريون;

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
create_محفوظ_Mapping_Entries_PDE
(paddr_t بنياد, لفظ_t vaddr, vm_صفحو_سائيز_t چوکٺو_سائيز,
 vm_rights_t vmRights, vm_خاصيتون_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_ٽيگ_t currentPDEType;
    لفظ_t i;

    بدل (چوکٺو_سائيز) {

    /* PDE mappings */
    حالة ARMSection:
        ret.pde_entries.بنياد = lookupPDSlot(pd, vaddr);
        ret.pde_entries.ڊيگهائي = 1;

        ret.pde = makeUserPDE(ARMSection, بنياد,
                              vm_خاصيتون_get_arm_Parity_Enabled(attr),
                              vm_خاصيتون_get_arm_صفحو_Cacheable(attr),
                              vm_خاصيتون_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.بنياد);
        إذا (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_سائيز(ret.pde_entries.بنياد) != 0))) {
#وإلا
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.بنياد) != 0))) {
#جيڪڏهن_پورو
            current_syscall_error.type =
                se_L4_Delete_پهريون;
            ret.حالت = EXCEPTION_SYSCALL_ERROR;

            أرجع ret;
        }

        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة ARMSuperSection:
        ret.pde_entries.بنياد = lookupPDSlot(pd, vaddr);
        ret.pde_entries.ڊيگهائي = SECTIONS_في_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, بنياد,
                              vm_خاصيتون_get_arm_Parity_Enabled(attr),
                              vm_خاصيتون_get_arm_صفحو_Cacheable(attr),
                              vm_خاصيتون_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        لكل (i = 0; i < SECTIONS_في_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.بنياد[i]);
            إذا (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_سائيز(ret.pde_entries.بنياد[i]) != 1))) {
#وإلا
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.بنياد[i]) != 1))) {
#جيڪڏهن_پورو
                current_syscall_error.type =
                    se_L4_Delete_پهريون;
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

ساكن مضمن vptr_t صفحو_بنياد(vptr_t vaddr, vm_صفحو_سائيز_t سائيز)
{
    أرجع vaddr & ~نقاب(صفحو_بٽ_For_سائيز(سائيز));
}

ساكن bool_t خالص pteCheckIfMapped(pte_t *pte)
{
    أرجع pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

ساكن bool_t خالص pdeCheckIfMapped(pde_t *pde)
{
    أرجع pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

ساكن فراغ doFlush(صحيح invLabel, vptr_t شروعات, vptr_t پڇاڙي, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    إذا (config_سيٽ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        پڇاڙي = (vptr_t)paddr_منزل_pptr(pstart) + (پڇاڙي - شروعات);
        شروعات = (vptr_t)paddr_منزل_pptr(pstart);
    }
    بدل (invLabel) {
    حالة ARMPDClean_معلومات:
    حالة ARMPage_Clean_معلومات:
        cleanCacheRange_RAM(شروعات, پڇاڙي, pstart);
        اكسر;
    حالة ARMPDInvalidate_معلومات:
    حالة ARMPage_Invalidate_معلومات:
        invalidateCacheRange_RAM(شروعات, پڇاڙي, pstart);
        اكسر;
    حالة ARMPDClean_Invalidate_معلومات:
    حالة ARMPage_Clean_Invalidate_معلومات:
        cleanInvalidateCacheRange_RAM(شروعات, پڇاڙي, pstart);
        اكسر;
    حالة ARMPDUnify_Instruction:
    حالة ARMPageUnify_Instruction:
        /* First clean data lines to point of unification
           (L2 cache)... */
        cleanCacheRange_PoU(شروعات, پڇاڙي, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(شروعات, پڇاڙي, pstart);
        /* ...then invalidate branch predictors... */
        branchFlushRange(شروعات, پڇاڙي, pstart);
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

ساكن exception_t performPDFlush(صحيح invLabel, pde_t *pd, asid_t asid, vptr_t شروعات,
                                  vptr_t پڇاڙي, paddr_t pstart)
{
    bool_t پاڙ_switched;

    /* Flush if given a non zero range */
    إذا (شروعات < پڇاڙي) {
        پاڙ_switched = سيٽ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, شروعات, پڇاڙي, pstart);

        إذا (پاڙ_switched) {
            سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
        }
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_جدول_Invocation_Map(cap_t cap, cte_t *ct_جڳهه,
                                                 pde_t pde, pde_t *pd_جڳهه)
{
    ct_جڳهه->cap = cap;
    *pd_جڳهه = pde;
    cleanByVA_PoU((لفظ_t)pd_جڳهه, addrFromPPtr(pd_جڳهه));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_جڳهه)
{
    إذا (cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحو_جدول(
            cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(cap),
            pt);
        clear_يادگيري_PT((فراغ *)pt, cap_get_cap_سائيز_بٽ(cap));
    }
    cap_صفحو_جدول_cap_ptr_سيٽ_cap_PTIs_Mapped(&(ct_جڳهه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_Invocation_Map_PTE(asid_t asid, cap_t cap, cte_t *ct_جڳهه, pte_t pte,
                                               pte_range_t pte_entries)
{
    لفظ_t i, j اڻ_استعمال_ٿيل;
    bool_t tlbflush_required;

    ct_جڳهه->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.بنياد);

    j = pte_entries.ڊيگهائي;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t بنياد_پتو = pte_pte_small_get_پتو(pte);
#جيڪڏهن_پورو
    لكل (i = 0; i < pte_entries.ڊيگهائي; i++) {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_سيٽ_پتو(pte, بنياد_پتو + i * ٻٽو_انگ(صفحو_بٽ_For_سائيز(ARMSmall_صفحو)));
#جيڪڏهن_پورو
        pte_entries.بنياد[i] = pte;
    }
    cleanCacheRange_PoU((لفظ_t)pte_entries.بنياد,
                        آخري_BYTE_PTE(pte_entries.بنياد, pte_entries.ڊيگهائي),
                        addrFromPPtr(pte_entries.بنياد));
    إذا (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_Invocation_Map_PDE(asid_t asid, cap_t cap, cte_t *ct_جڳهه, pde_t pde,
                                               pde_range_t pde_entries)
{
    لفظ_t i, j اڻ_استعمال_ٿيل;
    bool_t tlbflush_required;

    ct_جڳهه->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.بنياد);

    j = pde_entries.ڊيگهائي;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t بنياد_پتو = pde_pde_section_get_پتو(pde);
#جيڪڏهن_پورو
    لكل (i = 0; i < pde_entries.ڊيگهائي; i++) {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_سيٽ_پتو(pde, بنياد_پتو + i * ٻٽو_انگ(صفحو_بٽ_For_سائيز(ARMSection)));
#جيڪڏهن_پورو
        pde_entries.بنياد[i] = pde;
    }
    cleanCacheRange_PoU((لفظ_t)pde_entries.بنياد,
                        آخري_BYTE_PDE(pde_entries.بنياد, pde_entries.ڊيگهائي),
                        addrFromPPtr(pde_entries.بنياد));
    إذا (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_Invocation_Unmap(cap_t cap, cte_t *ct_جڳهه)
{
    إذا (generic_چوکٺو_cap_get_cap_FIs_Mapped(cap)) {
        unmap_صفحو(generic_چوکٺو_cap_get_cap_FSize(cap),
                  generic_چوکٺو_cap_get_cap_FMapped_ASID(cap),
                  generic_چوکٺو_cap_get_cap_FMapped_پتو(cap),
                  (فراغ *)generic_چوکٺو_cap_get_cap_FBase_Ptr(cap));
    }

    generic_چوکٺو_cap_ptr_سيٽ_cap_FMapped_پتو(&ct_جڳهه->cap, asidInvalid, 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_Flush(صحيح invLabel, pde_t *pd, asid_t asid, vptr_t شروعات,
                                    vptr_t پڇاڙي, paddr_t pstart)
{
    bool_t پاڙ_switched;

    /* now we can flush. But only if we were given a non zero range */
    إذا (شروعات < پڇاڙي) {
        پاڙ_switched = سيٽ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, شروعات, پڇاڙي, pstart);

        إذا (پاڙ_switched) {
            سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
        }
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحو_Get_پتو(فراغ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);
    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
        بنانشان صحيح ڊيگهائي = سيٽ_MR(thread, ipc_بفر, 0, capFBasePtr);
        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, ڊيگهائي)));
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pd_Cap_جڳهه)
{
    cap_صفحو_directory_cap_ptr_سيٽ_cap_PDMapped_ASID(&pd_Cap_جڳهه->cap, asid);
    cap_صفحو_directory_cap_ptr_سيٽ_cap_PDIs_Mapped(&pd_Cap_جڳهه->cap, 1);
    poolPtr->array[ASID_هيٺيون(asid)] =
        PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(pd_Cap_جڳهه->cap));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDControlInvocation(فراغ *چوکٺو, cte_t *جڳهه,
                                                cte_t *والد, asid_t asid_بنياد)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سيٽ_cap_Free_اشاريو(&(والد->cap),
                                         MAX_FREE_اشاريو(cap_untyped_cap_get_cap_بلاڪ_سائيز(والد->cap)));

    memzero(چوکٺو, 1 << ARMSmall_صفحو_بٽ);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_وجهو(cap_asid_pool_cap_new(asid_بنياد, لفظ_REF(چوکٺو)),
              والد, جڳهه);;
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنياد & نقاب(asid_هيٺيون_بٽ)) == 0);
    armKSASIDTable[ASID_مٿاهون(asid_بنياد)] = (asid_pool_t *)چوکٺو;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeARMPageDirectoryInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    لفظ_t *بفر)
{
    بدل (invLabel) {
    حالة ARMPDClean_معلومات:
    حالة ARMPDInvalidate_معلومات:
    حالة ARMPDClean_Invalidate_معلومات:
    حالة ARMPDUnify_Instruction: {
        vptr_t شروعات, پڇاڙي;
        paddr_t pstart;
        ڳوليو_PDFor_ASID_ret_t ڳوليو_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        إذا (ڊيگهائي < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروعات = getSyscallArg(0, بفر);
        پڇاڙي =   getSyscallArg(1, بفر);

        /* Check sanity of arguments */
        إذا (پڇاڙي <= شروعات) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        إذا (شروعات >= USER_چوٽي || پڇاڙي > USER_چوٽي) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(cap_get_capType(cap) != cap_صفحو_directory_cap ||
                     !cap_صفحو_directory_cap_get_cap_PDIs_Mapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(cap));
        asid = cap_صفحو_directory_cap_get_cap_PDMapped_ASID(cap);

        ڳوليو_ret = ڳوليو_PDFor_ASID(asid);
        إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(ڳوليو_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, شروعات);

        /* Check that there's actually something there. */
        إذا (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
            أرجع EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        إذا (صفحو_بنياد(شروعات, resolve_ret.چوکٺو_سائيز) !=
            صفحو_بنياد(پڇاڙي - 1, resolve_ret.چوکٺو_سائيز)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = شروعات;
            current_syscall_error.rangeErrorMax =
                صفحو_بنياد(شروعات, resolve_ret.چوکٺو_سائيز) +
                نقاب(صفحو_بٽ_For_سائيز(resolve_ret.چوکٺو_سائيز));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.چوکٺو_بنياد
                 + (شروعات & نقاب(صفحو_بٽ_For_سائيز(resolve_ret.چوکٺو_سائيز)));

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        إذا (pstart < phys_بنياد() || pstart > PADDR_چوٽي || (پڇاڙي - شروعات) > PADDR_چوٽي - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جيڪڏهن_پورو

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performPDFlush(invLabel, pd, asid, شروعات, پڇاڙي - 1, pstart);
    }

    افتراضي:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

}

ساكن exception_t decode_ARMPage_جدول_Invocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                                                cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    لفظ_t vaddr, pd_اشاريو;

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_خاصيتون_t attr;
#جيڪڏهن_پورو
    cap_t pdCap;
    pde_t *pd, *pd_جڳهه;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    إذا (invLabel == ARMPage_جدول_Unmap) {
        إذا (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_پهريون;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely(invLabel != ARMPage_جدول_Map)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(ڊيگهائي < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vm_خاصيتون_From_لفظ(getSyscallArg(1, بفر));
#جيڪڏهن_پورو
    pdCap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(cap_get_capType(pdCap) != cap_صفحو_directory_cap ||
                 !cap_صفحو_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(pdCap));
    asid = cap_صفحو_directory_cap_get_cap_PDMapped_ASID(pdCap);

    إذا (unlikely(vaddr >= USER_چوٽي)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_چوٽي);
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    {
        ڳوليو_PDFor_ASID_ret_t ڳوليو_ret;

        ڳوليو_ret = ڳوليو_PDFor_ASID(asid);
        إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(ڳوليو_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_اشاريو = vaddr >> (صفحو_بٽ + PT_اشاريو_بٽ);
    pd_جڳهه = &pd[pd_اشاريو];
    إذا (unlikely(pde_ptr_get_pdeType(pd_جڳهه) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = se_L4_Delete_پهريون;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap)));
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_خاصيتون_get_arm_Parity_Enabled(attr),
              0 /* Domain */
          );
#وإلا
    pde = pde_pde_coarse_new(paddr);
#جيڪڏهن_پورو

    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTMapped_پتو(cap, vaddr);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_صفحو_جدول_Invocation_Map(cap, cte, pde, pd_جڳهه);
}

ساكن exception_t decodeARMFrameInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي,
                                            cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (invLabel) {
    حالة ARMPageMap: {
        لفظ_t vaddr, vtop, w_rights_نقاب;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_صفحو_سائيز_t چوکٺو_سائيز;
        vm_خاصيتون_t attr;

        إذا (unlikely(ڊيگهائي < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                se_L4_Truncated_پيغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, بفر);
        w_rights_نقاب = getSyscallArg(1, بفر);
        attr = vm_خاصيتون_From_لفظ(getSyscallArg(2, بفر));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        چوکٺو_سائيز = generic_چوکٺو_cap_get_cap_FSize(cap);
        capVMRights = generic_چوکٺو_cap_get_cap_FVMRights(cap);

        إذا (unlikely(cap_get_capType(pdCap) != cap_صفحو_directory_cap ||
                     !cap_صفحو_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(
                         pdCap));
        asid = cap_صفحو_directory_cap_get_cap_PDMapped_ASID(pdCap);

        إذا (generic_چوکٺو_cap_get_cap_FIs_Mapped(cap)) {
            إذا (generic_چوکٺو_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (generic_چوکٺو_cap_get_cap_FMapped_پتو(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دليل;
                current_syscall_error.invalid_دليل_عدد = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            vtop = vaddr + ٻٽو_انگ(صفحو_بٽ_For_سائيز(چوکٺو_سائيز)) - 1;

            إذا (unlikely(vtop >= USER_چوٽي)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_چوٽي);
                current_syscall_error.type =
                    se_L4_Invalid_دليل;
                current_syscall_error.invalid_دليل_عدد = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            ڳوليو_PDFor_ASID_ret_t ڳوليو_ret;

            ڳوليو_ret = ڳوليو_PDFor_ASID(asid);
            إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_ذريعو =
                    false;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (unlikely(ڳوليو_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            نقاب_VMRights(capVMRights, rights_From_لفظ(w_rights_نقاب));

        إذا (unlikely(!checkVPAlignment(چوکٺو_سائيز, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((فراغ *)
                                   generic_چوکٺو_cap_get_cap_FBase_Ptr(cap));

        cap = generic_چوکٺو_cap_سيٽ_cap_FMapped_پتو(cap, asid,
                                                      vaddr);
        إذا (چوکٺو_سائيز == ARMSmall_صفحو || چوکٺو_سائيز == ARMLarge_صفحو) {
            create_mappings_pte_return_t map_ret;
            map_ret = create_محفوظ_Mapping_Entries_PTE(capFBasePtr, vaddr,
                                                   چوکٺو_سائيز, vmRights,
                                                   attr, pd);
            إذا (unlikely(map_ret.حالت != EXCEPTION_NONE)) {
#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
                إذا (current_syscall_error.type == se_L4_Delete_پهريون) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#جيڪڏهن_پورو
                أرجع map_ret.حالت;
            }

            سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_صفحو_Invocation_Map_PTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } وإلا {
            create_mappings_pde_return_t map_ret;
            map_ret = create_محفوظ_Mapping_Entries_PDE(capFBasePtr, vaddr,
                                                   چوکٺو_سائيز, vmRights,
                                                   attr, pd);
            إذا (unlikely(map_ret.حالت != EXCEPTION_NONE)) {
#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
                إذا (current_syscall_error.type == se_L4_Delete_پهريون) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#جيڪڏهن_پورو
                أرجع map_ret.حالت;
            }

            سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_صفحو_Invocation_Map_PDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    حالة ARMPageUnmap: {
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
        إذا (is_IOSpace_چوکٺو_Cap(cap)) {
            سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_صفحو_Invocation_Unmap_IO(cap, cte);
        } وإلا
#جيڪڏهن_پورو
        {
            سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_صفحو_Invocation_Unmap(cap, cte);
        }
    }

#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة ARMPageMapIO: {
        أرجع decodeARMIOMapInvocation(invLabel, ڊيگهائي, cte, cap, بفر);
    }
#جيڪڏهن_پورو

    حالة ARMPage_Clean_معلومات:
    حالة ARMPage_Invalidate_معلومات:
    حالة ARMPage_Clean_Invalidate_معلومات:
    حالة ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        ڳوليو_PDFor_ASID_ret_t pd;
        vptr_t شروعات, پڇاڙي;
        paddr_t pstart;
        لفظ_t صفحو_سائيز;
        لفظ_t صفحو_بنياد_2;

        إذا (ڊيگهائي < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_چوکٺو_cap_get_cap_FMapped_ASID(cap);
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_چوکٺو_cap_get_cap_FBase_Ptr(cap);
#وإلا
        vaddr = generic_چوکٺو_cap_get_cap_FMapped_پتو(cap);
#جيڪڏهن_پورو

        إذا (unlikely(!generic_چوکٺو_cap_get_cap_FIs_Mapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pd = ڳوليو_PDFor_ASID(asid);
        إذا (unlikely(pd.حالت != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروعات = getSyscallArg(0, بفر);
        پڇاڙي =   getSyscallArg(1, بفر);

        /* check that the range is sane */
        إذا (پڇاڙي <= شروعات) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        صفحو_سائيز = 1 << صفحو_بٽ_For_سائيز(generic_چوکٺو_cap_get_cap_FSize(cap));
        صفحو_بنياد_2 = addrFromPPtr((فراغ *)generic_چوکٺو_cap_get_cap_FBase_Ptr(cap));

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        إذا (unlikely(!resolve_ret.valid ||
                     resolve_ret.چوکٺو_سائيز != generic_چوکٺو_cap_get_cap_FSize(cap) ||
                     resolve_ret.چوکٺو_بنياد != صفحو_بنياد_2)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جيڪڏهن_پورو

        إذا (شروعات >= صفحو_سائيز || پڇاڙي > صفحو_سائيز) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = صفحو_بنياد_2 + شروعات;
        شروعات += vaddr;
        پڇاڙي += vaddr;

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        إذا (pstart < phys_بنياد() || pstart > PADDR_چوٽي || (پڇاڙي - شروعات) > PADDR_چوٽي - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جيڪڏهن_پورو

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_Flush(invLabel, pd.pd, asid, شروعات, پڇاڙي - 1, pstart);
    }

    حالة ARMPage_Get_پتو: {


        /* Check that there are enough message registers */
        أكد(n_نياپو_Registers >= 1);

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_Get_پتو((فراغ *)generic_چوکٺو_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_صفحو_directory_cap:
        أرجع decodeARMPageDirectoryInvocation(invLabel, ڊيگهائي, cptr, cte,
                                                cap, بفر);

    حالة cap_صفحو_جدول_cap:
        أرجع decode_ARMPage_جدول_Invocation(invLabel, ڊيگهائي, cte, cap, بفر);

    حالة cap_small_چوکٺو_cap:
    حالة cap_چوکٺو_cap:
        أرجع decodeARMFrameInvocation(invLabel, ڊيگهائي, cte, cap, call, بفر);

    حالة cap_asid_control_cap: {
        لفظ_t i;
        asid_t asid_بنياد;
        لفظ_t اشاريو, اونهائي;
        cap_t untyped, پاڙ;
        cte_t *والد_جڳهه, *dest_جڳهه;
        lookup_جڳهه_ret_t lu_ret;
        فراغ *چوکٺو;
        exception_t حالت;

        إذا (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(ڊيگهائي < 2 || current_extra_caps.excaprefs[0] == عدم
                     || current_extra_caps.excaprefs[1] == عدم)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        اشاريو = getSyscallArg(0, بفر);
        اونهائي = getSyscallArg(1, بفر);
        والد_جڳهه = current_extra_caps.excaprefs[0];
        untyped = والد_جڳهه->cap;
        پاڙ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        إذا (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پهريون;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنياد = i << asid_هيٺيون_بٽ;

        إذا (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_بلاڪ_سائيز(untyped) !=
                     se_L4_ASIDPool_بٽ || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(والد_جڳهه);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            أرجع حالت;
        }

        چوکٺو = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_هدف_جڳهه(پاڙ, اشاريو, اونهائي);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            أرجع lu_ret.حالت;
        }
        dest_جڳهه = lu_ret.جڳهه;

        حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            أرجع حالت;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکٺو, dest_جڳهه,
                                            والد_جڳهه, asid_بنياد);
    }

    حالة cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pd_Cap_جڳهه;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t asid;

        إذا (unlikely(invLabel != ARMASIDPool_سونپيو)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pd_Cap_جڳهه = current_extra_caps.excaprefs[0];
        pdCap = pd_Cap_جڳهه->cap;

        إذا (unlikely(
                cap_get_capType(pdCap) != cap_صفحو_directory_cap ||
                cap_صفحو_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_مٿاهون(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            current_lookup_fault = lookup_fault_invalid_پاڙ_new();

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < (1 << asid_هيٺيون_بٽ) && (asid + i == 0 || pool->array[i]); i++);

        إذا (unlikely(i == 1 << asid_هيٺيون_بٽ)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = se_L4_Delete_پهريون;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, pd_Cap_جڳهه);
    }

    افتراضي:
        fail("Invalid ARM arch cap type");
    }
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
exception_t benchmark_arch_map_log_بفر(لفظ_t چوکٺو_cptr)
{
    lookup_Cap_And_جڳهه_ret_t lu_ret;
    vm_صفحو_سائيز_t چوکٺو_سائيز;
    pptr_t  چوکٺو_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_جڳهه(ڳنڍ_STATE(ksCurThread), چوکٺو_cptr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکٺو_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکٺو_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_چوکٺو_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکٺو_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکٺو_سائيز = generic_چوکٺو_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکٺو_سائيز != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکٺو_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکٺو_pptr = generic_چوکٺو_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکٺو_pptr);

    لكل (صحيح idx = 0; idx < ٻٽو_انگ(PT_اشاريو_بٽ); idx++) {
        paddr_t physical_پتو = ks_User_Log_بفر + (idx << se_L4_صفحو_بٽ);

        armKSGlobalLogPT[idx] =
            pte_pte_small_new(
                physical_پتو,
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
        invalidateTranslationSingle(KS_LOG_PPTR + (idx * ٻٽو_انگ(se_L4_صفحو_بٽ)));
    }

    أرجع EXCEPTION_NONE;
}
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
فراغ kernelPrefetchAbort(لفظ_t pc, لفظ_t lr) VISIBLE;
فراغ kernel_معلومات_Abort(لفظ_t pc) VISIBLE;

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT

فراغ kernelUndefinedInstruction(لفظ_t pc) VISIBLE;

فراغ kernelPrefetchAbort(لفظ_t pc, لفظ_t lr)
{
    لکو("\n\nKERNEL PREFETCH ABORT!\n");
    لکو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکو("HSR: 0x%"SEL4_PRIx_لفظ"\n", getHSR());
    لکو("LR: 0x%"SEL4_PRIx_لفظ"\n", lr);
    halt();
}

فراغ kernel_معلومات_Abort(لفظ_t pc)
{
    لکو("\n\nKERNEL DATA ABORT!\n");
    لکو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکو("HDFAR: 0x%"SEL4_PRIx_لفظ" HSR: 0x%"SEL4_PRIx_لفظ"\n",
           getHDFAR(), getHSR());
    halt();
}

فراغ kernelUndefinedInstruction(لفظ_t pc)
{
    لکو("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    لکو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکو("HSR: 0x%"SEL4_PRIx_لفظ"\n", getHSR());
    halt();
}

#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */

فراغ kernelPrefetchAbort(لفظ_t pc, لفظ_t lr)
{
    لکو("\n\nKERNEL PREFETCH ABORT!\n");
    لکو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکو("IFSR: 0x%"SEL4_PRIx_لفظ"\n", getIFSR());
    لکو("LR: 0x%"SEL4_PRIx_لفظ"\n", lr);
    halt();
}

فراغ kernel_معلومات_Abort(لفظ_t pc)
{
    لکو("\n\nKERNEL DATA ABORT!\n");
    لکو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکو("FAR: 0x%"SEL4_PRIx_لفظ" DFSR: 0x%"SEL4_PRIx_لفظ"\n",
           getFAR(), getDFSR());
    halt();
}
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
تعريفنوع بنية read_لفظ_From_VSpace_ret {
    exception_t حالت;
    لفظ_t قدر;
} read_لفظ_From_VSpace_ret_t;

ساكن read_لفظ_From_VSpace_ret_t read_لفظ_From_VSpace(pde_t *pd, لفظ_t vaddr)
{
    read_لفظ_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_جڳهه;
    pde_t *pd_جڳهه;
    paddr_t paddr;
    لفظ_t وٿي;
    pptr_t kernel_vaddr;
    لفظ_t *قدر;

    pd_جڳهه = lookupPDSlot(pd, vaddr);
    إذا (pde_ptr_get_pdeType(pd_جڳهه) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_پتو(pd_جڳهه);
        وٿي = vaddr & نقاب(ARMSection_بٽ);
    } وإلا {
        pt_جڳهه = lookupPTSlot(pd, vaddr);
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (pt_جڳهه.حالت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_جڳهه.pt_جڳهه) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_پتو(pt_جڳهه.pt_جڳهه);
            إذا (pte_pte_small_ptr_get_contiguous_hint(pt_جڳهه.pt_جڳهه)) {
                وٿي = vaddr & نقاب(ARMLarge_صفحو_بٽ);
            } وإلا {
                وٿي = vaddr & نقاب(ARMSmall_صفحو_بٽ);
            }
#وإلا
        إذا (pt_جڳهه.حالت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_جڳهه.pt_جڳهه) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_پتو(pt_جڳهه.pt_جڳهه);
            وٿي = vaddr & نقاب(ARMSmall_صفحو_بٽ);
        } وإلا إذا (pt_جڳهه.حالت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_جڳهه.pt_جڳهه) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_پتو(pt_جڳهه.pt_جڳهه);
            وٿي = vaddr & نقاب(ARMLarge_صفحو_بٽ);
#جيڪڏهن_پورو
        } وإلا {
            ret.حالت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }
    }


    kernel_vaddr = (لفظ_t)paddr_منزل_pptr(paddr);
    قدر = (لفظ_t *)(kernel_vaddr + وٿي);
    ret.حالت = EXCEPTION_NONE;
    ret.قدر = *قدر;
    أرجع ret;
}

فراغ Arch_user_ڍير_Trace(tcb_t *tptr)
{
    cap_t thread_پاڙ;
    pde_t *pd;
    لفظ_t sp;
    صحيح i;

    thread_پاڙ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    إذا (cap_get_capType(thread_پاڙ) != cap_صفحو_directory_cap) {
        لکو("Invalid vspace\n");
        أرجع;
    }

    pd = (pde_t *)pptr_جو_cap(thread_پاڙ);

    sp = get_رجسٽر(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_لفظ_سائيز_بٽ)) {
        لکو("SP not aligned\n");
        أرجع;
    }

    لكل (i = 0; i < CONFIG_USER_ڍير_TRACE_ڊيگهائي; i++) {
        لفظ_t پتو = sp + (i * حجم(لفظ_t));
        read_لفظ_From_VSpace_ret_t نتيجو;
        نتيجو = read_لفظ_From_VSpace(pd, پتو);
        إذا (نتيجو.حالت == EXCEPTION_NONE) {
            لکو("0x%lx: 0x%lx\n", (ڊگهو)پتو, (ڊگهو)نتيجو.قدر);
        } وإلا {
            لکو("0x%lx: INVALID\n", (ڊگهو)پتو);
        }
    }
}
#جيڪڏهن_پورو
