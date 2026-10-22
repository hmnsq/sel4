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

#جے_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ضمن <benchmark/benchmark_track.h>
#جے_انت

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#تعریف_کرو SECTIONS_فی_SUPER_SECTION ثنائی_ہندسہ(ARMSuper_Section_بٹ - ARMSection_بٹ)
#تعریف_کرو ورقے_فی_LARGE_ورقہ ثنائی_ہندسہ(ARMLarge_ورقہ_بٹ - ARMSmall_ورقہ_بٹ)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#تعریف_کرو آخری_BYTE_PTE(PTE,لمائی_2) ((لفظ_t)&(PTE)[(لمائی_2)-1] + (ثنائی_ہندسہ(PTE_ناپ_بٹ)-1))
#تعریف_کرو آخری_BYTE_PDE(PDE,لمائی_2) ((لفظ_t)&(PDE)[(لمائی_2)-1] + (ثنائی_ہندسہ(PDE_ناپ_بٹ)-1))

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#تعریف_کرو MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#تعریف_کرو MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#تعریف_کرو ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#تعریف_کرو ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ساكن فراغ سیٹ_Global_PD(فراغ);

بنية resolve_ret {
    paddr_t چوکھٹا_ادھار;
    vm_ورقہ_ناپ_t چوکھٹا_ناپ;
    bool_t valid;
};
تعريفنوع بنية resolve_ret resolve_ret_t;

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
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
BOOT_کوڈ
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

#جے_انت

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

BOOT_کوڈ فراغ map_kernel_چوکھٹا(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_گن_t گن)
{
    لفظ_t idx = (vaddr & نقاب(ورقہ_بٹ_For_ناپ(ARMSection))) >> ورقہ_بٹ_For_ناپ(ARMSmall_ورقہ);

    أكد(vaddr >= PPTR_چوٹی); /* vaddr lies in the region the global PT covers */
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t tex;
    إذا (vm_گن_get_arm_ورقہ_Cacheable(گن)) {
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
            vm_گن_get_arm_ورقہ_Cacheable(گن)
        );
#جے_انت /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_کوڈ فراغ map_kernel_کھڑکی(فراغ)
{
    paddr_t  phys;
    لفظ_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = phys_ادھار();
    idx = PPTR_ادھار >> ورقہ_بٹ_For_ناپ(ARMSection);

    طالما (idx < ثنائی_ہندسہ(PD_اشاریہ_بٹ) - SECTIONS_فی_SUPER_SECTION) {
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
        لكل (idx2 = idx; idx2 < idx + SECTIONS_فی_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(ARMSuperSection));
        idx += SECTIONS_فی_SUPER_SECTION;
    }

    طالما (idx < (PPTR_چوٹی >> ورقہ_بٹ_For_ناپ(ARMSection))) {
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
        phys += ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    أكد(phys == PADDR_چوٹی);

#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /*پتہ*/
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, ثنائی_ہندسہ(se_L4_ورقہ_جدول_بٹ));

    phys += ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(ARMSection));
    idx++;
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */

    /* map page table covering last 1M of virtual address space to page directory */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalPT), /*پتہ*/
            true,                        /* P       */
            0                            /* Domain  */
        );

    /* now start initialising the page table */
    memzero(armKSGlobalPT, 1 << se_L4_ورقہ_جدول_بٹ);

    /* map vector table */
    map_kernel_چوکھٹا(
        addrFromKPPtr(arm_سمتیہ_جدول),
        PPTR_سمتیہ_جدول,
        VMKernelOnly,
        vm_گن_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_کوڈ فراغ map_kernel_کھڑکی(فراغ)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t بن_ورتیا pteS2;

    /* Initialise PGD */
    لكل (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    لكل (idx = 0; idx < (USER_چوٹی - 0xC0000000) >> (PT_اشاریہ_بٹ + ورقہ_بٹ); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = phys_ادھار();
    لكل (; idx < ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1; idx++) {
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
        phys += ثنائی_ہندسہ(PT_اشاریہ_بٹ + ورقہ_بٹ);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << se_L4_ورقہ_جدول_بٹ);
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
        phys += ثنائی_ہندسہ(ورقہ_بٹ);
    }
    /* map vector table */
    map_kernel_چوکھٹا(
        addrFromKPPtr(arm_سمتیہ_جدول),
        PPTR_سمتیہ_جدول,
        VMKernelOnly,
        vm_گن_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#جے_انت /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

ساكن BOOT_کوڈ فراغ map_it_چوکھٹا_cap(cap_t pd_cap, cap_t چوکھٹا_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *نشانہ_تھاں;
    pde_t *pd    = PDE_PTR(cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    فراغ  *چوکھٹا = (فراغ *)generic_چوکھٹا_cap_get_cap_FBase_Ptr(چوکھٹا_cap);
    vptr_t vptr  = generic_چوکھٹا_cap_get_cap_FMapped_پتہ(چوکھٹا_cap);

    أكد(generic_چوکھٹا_cap_get_cap_FMapped_ASID(چوکھٹا_cap) != 0);

    pd += (vptr >> ورقہ_بٹ_For_ناپ(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pd));
    نشانہ_تھاں = pt + ((vptr & نقاب(ورقہ_بٹ_For_ناپ(ARMSection)))
                       >> ورقہ_بٹ_For_ناپ(ARMSmall_ورقہ));
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
    *نشانہ_تھاں = pte_pte_small_new(
                      addrFromPPtr(چوکھٹا),
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
    *نشانہ_تھاں = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(چوکھٹا),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#جے_انت
}

/* Create a frame cap for the initial thread. */

ساكن BOOT_کوڈ cap_t create_it_چوکھٹا_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    إذا (use_large)
        أرجع
            cap_چوکھٹا_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_کم(asid),                /* capFMappedASIDLow  */
                لفظ_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_بلند(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    وإلا
        أرجع
            cap_small_چوکھٹا_cap_new(
                ASID_کم(asid),                /* capFMappedASIDLow  */
                لفظ_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#جے_تعریف_شدہ CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#جے_انت
                ASID_بلند(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

ساكن BOOT_کوڈ فراغ map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(pt_cap);
    pde_t *نشانہ_تھاں = pd + (vptr >> ورقہ_بٹ_For_ناپ(ARMSection));

    أكد(cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(pt_cap));

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
    *نشانہ_تھاں = pde_pde_coarse_new(
                      addrFromPPtr(pt), /*پتہ*/
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#وإلا
    *نشانہ_تھاں = pde_pde_coarse_new(addrFromPPtr(pt));
#جے_انت
}

/* Create a page table for the initial thread */

ساكن BOOT_کوڈ cap_t create_it_ورقہ_جدول_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ورقہ_جدول_cap_new(
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

BOOT_کوڈ لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    أرجع get_n_paging(it_v_reg, PT_اشاریہ_بٹ + ورقہ_بٹ);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_کوڈ cap_t create_it_پتہ_space(cap_t جڑ_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    se_L4_تھاں_Pos تھاں_pos_before;
    se_L4_تھاں_Pos تھاں_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << se_L4_ورقہ_Dir_بٹ) - 1,
                        addrFromPPtr((فراغ *)rootserver.vspace));
    cap_t pd_cap =
        cap_ورقہ_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    تھاں_pos_before = ndks_boot.تھاں_pos_cur;
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    لكل (pt_vptr = گیڑا_تھلے(it_v_reg.شروع, PT_اشاریہ_بٹ + ورقہ_بٹ);
         pt_vptr < it_v_reg.سرا;
         pt_vptr += ثنائی_ہندسہ(PT_اشاریہ_بٹ + ورقہ_بٹ)) {
        إذا (!provide_cap(جڑ_cnode_cap,
                         create_it_ورقہ_جدول_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    تھاں_pos_after = ndks_boot.تھاں_pos_cur;
    ndks_boot.bi_چوکھٹا->userImagePaging = (se_L4_تھاں_Region) {
        تھاں_pos_before, تھاں_pos_after
    };

    أرجع pd_cap;
}

BOOT_کوڈ cap_t create_unmapped_it_چوکھٹا_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_چوکھٹا_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_کوڈ cap_t create_mapped_it_چوکھٹا_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_چوکھٹا_cap(pptr, vptr, asid, use_large);
    map_it_چوکھٹا_cap(pd_cap, cap, executable);
    أرجع cap;
}

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_کوڈ فراغ activate_kernel_vspace(فراغ)
{
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    سیٹ_Global_PD();
    invalidateLocalTLB();
    جندرا_TLBEntry(PPTR_ادھار);
    جندرا_TLBEntry(PPTR_سمتیہ_جدول);
}

#وإلا

BOOT_کوڈ فراغ activate_kernel_vspace(فراغ)
{
    uint32_t r;
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    /* Setup the memory attributes: We use 2 indices (cachable/non-cacheable) */
    سیٹ_HMAIR((ATTRINDX_NONCACHEABLE << 0) | (ATTRINDX_CACHEABLE << 8), 0);
    سیٹ_Current_Hyp_PD(addrFromKPPtr(armHSGlobalPGD));
    invalidateHypTLB();
#إذا 0 /* Can't lock entries on A15 */
    جندرا_TLBEntry(PPTR_ادھار);
    جندرا_TLBEntry(PPTR_سمتیہ_جدول);
#جے_انت
    /* TODO find a better place to init the VMMU */
    r = 0;
    /* Translation range */
    r |= (0x0 << 0);     /* 2^(32 -(0)) input range. */
    r |= (r & 0x8) << 1; /* Sign bit */
    /* starting level */
    r |= (0x0 << 6);     /* Start at second level */
    /* Sharability of tables */
    r |= ثنائی_ہندسہ(8);       /* Inner write-back, write-allocate */
    r |= ثنائی_ہندسہ(10);      /* Outer write-back, write-allocate */
    /* Long descriptor format (not that we have a choice) */
    r |= ثنائی_ہندسہ(31);
    سیٹ_VTCR(r);
}

#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_کوڈ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_دا_cap(it_ap_cap));
    ap->array[ASID_کم(IT_ASID)] = PDE_PTR(pptr_دا_cap(it_pd_cap));
    armKSASIDTable[ASID_بلند(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

لبھو_PDFor_ASID_ret_t لبھو_PDFor_ASID(asid_t asid)
{
    لبھو_PDFor_ASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_بلند(asid)];
    إذا (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.pd = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    pd = poolPtr->array[ASID_کم(asid)];
    إذا (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

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

    إذا (unlikely(cap_get_capType(بفر_Cap) != cap_small_چوکھٹا_cap &&
                 cap_get_capType(بفر_Cap) != cap_چوکھٹا_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(generic_چوکھٹا_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = generic_چوکھٹا_cap_get_cap_FVMRights(بفر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        لفظ_t ادھار_Ptr;
        بنانشانی صحيح ورقہ_بٹ_2;

        ادھار_Ptr = generic_چوکھٹا_cap_get_cap_FBase_Ptr(بفر_Cap);
        ورقہ_بٹ_2 = ورقہ_بٹ_For_ناپ(generic_چوکھٹا_cap_get_cap_FSize(بفر_Cap));
        أرجع (لفظ_t *)(ادھار_Ptr + (w_بفر_Ptr & نقاب(ورقہ_بٹ_2)));
    } وإلا {
        أرجع عدم;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (unlikely(cap_get_capType(cap) != cap_small_چوکھٹا_cap &&
                 cap_get_capType(cap) != cap_چوکھٹا_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(generic_چوکھٹا_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(vptr & نقاب(se_L4_IPCBuffer_ناپ_بٹ))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    بنانشانی صحيح pd_اشاریہ;

    pd_اشاریہ = vptr >> (ورقہ_بٹ + PT_اشاریہ_بٹ);
    أرجع pd + pd_اشاریہ;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pd_تھاں;

    pd_تھاں = lookupPDSlot(pd, vptr);

    إذا (unlikely(pde_ptr_get_pdeType(pd_تھاں) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_اشاریہ_بٹ + ورقہ_بٹ);

        ret.pt_تھاں = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pte_t *pt, *pt_تھاں;
        بنانشانی صحيح pt_اشاریہ;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pd_تھاں));
        pt_اشاریہ = (vptr >> ورقہ_بٹ) & نقاب(PT_اشاریہ_بٹ);
        pt_تھاں = pt + pt_اشاریہ;

        ret.pt_تھاں = pt_تھاں;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    }
}

ساكن pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    بنانشانی صحيح pt_اشاریہ;

    pt_اشاریہ = (vptr >> ورقہ_بٹ) & نقاب(PT_اشاریہ_بٹ);
    أرجع pt + pt_اشاریہ;
}

ساكن ثابت resolve_ret_t default_resolve_ret_t;

ساكن resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    بدل (pde_ptr_get_pdeType(pde)) {
    حالة pde_pde_section:
        ret.چوکھٹا_ادھار = pde_pde_section_ptr_get_پتہ(pde);
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (pde_pde_section_ptr_get_ناپ(pde)) {
            ret.چوکھٹا_ناپ = ARMSuperSection;
        } وإلا {
            ret.چوکھٹا_ناپ = ARMSection;
        }
#وإلا
        إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.چوکھٹا_ادھار &= ~نقاب(ورقہ_بٹ_For_ناپ(ARMSuperSection));
            ret.چوکھٹا_ناپ = ARMSuperSection;
        } وإلا {
            ret.چوکھٹا_ناپ = ARMSection;
        }
#جے_انت
        أرجع ret;

    حالة pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small:
            ret.چوکھٹا_ادھار = pte_pte_small_ptr_get_پتہ(pte);
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.چوکھٹا_ادھار &= ~نقاب(ورقہ_بٹ_For_ناپ(ARMLarge_ورقہ));
                ret.چوکھٹا_ناپ = ARMLarge_ورقہ;
            } وإلا {
                ret.چوکھٹا_ناپ = ARMSmall_ورقہ;
            }
#وإلا
            ret.چوکھٹا_ناپ = ARMSmall_ورقہ;
#جے_انت
            أرجع ret;

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large:
            ret.چوکھٹا_ادھار = pte_pte_large_ptr_get_پتہ(pte);
            ret.چوکھٹا_ناپ = ARMLarge_ورقہ;
            أرجع ret;
#جے_انت
        افتراضي:
            اكسر;
        }
        اكسر;
    }
    }

    ret.valid = false;
    أرجع ret;
}

ساكن pte_t CONST makeUserPTE(vm_ورقہ_ناپ_t ورقہ_ناپ, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t ap;

    ap = APFromVMRights(vm_rights);

    بدل (ورقہ_ناپ) {
    حالة ARMSmall_ورقہ: {
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

    حالة ARMLarge_ورقہ: {
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

    بدل (ورقہ_ناپ) {
    حالة ARMSmall_ورقہ: {
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

    حالة ARMLarge_ورقہ: {
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
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    أرجع pte;
}

ساكن pde_t CONST makeUserPDE(vm_ورقہ_ناپ_t ورقہ_ناپ, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, لفظ_t domain,
                               vm_rights_t vm_rights)
{
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t ap, size2;

    ap = APFromVMRights(vm_rights);
#وإلا
    لفظ_t hap, size2;

    (فراغ)domain;
    hap = HAPFromVMRights(vm_rights);
#جے_انت

    بدل (ورقہ_ناپ) {
    حالة ARMSection:
        size2 = 0;
        اكسر;

    حالة ARMSuperSection:
        size2 = 1;
        اكسر;

    افتراضي:
        fail("Invalid PDE frame type");
    }

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
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
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_جڑ(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_ورقہ_directory_cap &&
           cap_ورقہ_directory_cap_get_cap_PDIs_Mapped(cap);
}

bool_t CONST is_IOSpace_چوکھٹا_Cap(cap_t cap)
{
#جے_تعریف_شدہ CONFIG_TK1_SMMU
    أرجع cap_get_capType(cap) == cap_small_چوکھٹا_cap && cap_small_چوکھٹا_cap_get_cap_FIs_IOSpace(cap);
#وإلا
    أرجع false;
#جے_انت
}

/** Switch to the empty PD on the reserved HW ASID */
ساكن فراغ سیٹ_Global_PD(فراغ)
{
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
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
    سیٹ_Hardware_ASID(hwASIDReserved);
#جے_انت
}

فراغ سیٹ_VMRoot(tcb_t *tcb)
{
    cap_t thread_جڑ;
    asid_t asid;
    pde_t *pd;
    لبھو_PDFor_ASID_ret_t لبھو_ret;

    thread_جڑ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_جڑ) != cap_ورقہ_directory_cap ||
        !cap_ورقہ_directory_cap_get_cap_PDIs_Mapped(thread_جڑ)) {
        سیٹ_Global_PD();
        أرجع;
    }

    pd = PDE_PTR(cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(thread_جڑ));
    asid = cap_ورقہ_directory_cap_get_cap_PDMapped_ASID(thread_جڑ);
    لبھو_ret = لبھو_PDFor_ASID(asid);
    إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE || لبھو_ret.pd != pd)) {
        سیٹ_Global_PD();
        أرجع;
    }

    armv_contextSwitch(pd, asid);
}

ساكن bool_t سیٹ_VMRoot_For_Flush(pde_t *pd, asid_t asid)
{
    cap_t thread_جڑ;

    thread_جڑ = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbVTable)->cap;

    إذا (cap_get_capType(thread_جڑ) == cap_ورقہ_directory_cap &&
        cap_ورقہ_directory_cap_get_cap_PDIs_Mapped(thread_جڑ) &&
        PDE_PTR(cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(thread_جڑ)) == pd) {
        أرجع false;
    }

    armv_contextSwitch(pd, asid);

    أرجع true;
}

pde_t *ورقہ_جدول_Mapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    لبھو_PDFor_ASID_ret_t لبھو_ret;
    pde_t pde;
    بنانشانی صحيح pd_اشاریہ;

    لبھو_ret = لبھو_PDFor_ASID(asid);
    إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
        أرجع عدم;
    }

    pd_اشاریہ = vaddr >> (ورقہ_بٹ + PT_اشاریہ_بٹ);
    pde = لبھو_ret.pd[pd_اشاریہ];

    إذا (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_پتہ(pde)) == pt)) {
        أرجع لبھو_ret.pd;
    } وإلا {
        أرجع عدم;
    }
}

ساكن فراغ invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_کم(asid)];
    أكد(pd);

    pd[PD_ASID_تھاں] = pde_pde_invalid_new(0, false);
}

ساكن pde_t خالص لدو_HWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_کم(asid)];
    أكد(pd);

    أرجع pd[PD_ASID_تھاں];
}

ساكن فراغ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_کم(asid)];
    أكد(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_تھاں] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t لبھو_Free_HWASID(فراغ)
{
    لفظ_t hw_asid_وتھ;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    لكل (hw_asid_وتھ = 0;
         hw_asid_وتھ <= (لفظ_t)((hw_asid_t) - 1);
         hw_asid_وتھ ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_وتھ);
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

    stored_hw_asid = لدو_HWASID(asid);
    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } وإلا {
        hw_asid_t new_hw_asid;

        new_hw_asid = لبھو_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        أرجع new_hw_asid;
    }
}

ساكن فراغ invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = لدو_HWASID(asid);
    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

فراغ unmap_ورقہ_جدول(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pd_تھاں;
    بنانشانی صحيح pd_اشاریہ;

    pd = ورقہ_جدول_Mapped(asid, vaddr, pt);

    إذا (likely(pd != عدم)) {
        pd_اشاریہ = vaddr >> (PT_اشاریہ_بٹ + ورقہ_بٹ);
        pd_تھاں = pd + pd_اشاریہ;

        *pd_تھاں = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((لفظ_t)pd_تھاں, addrFromPPtr(pd_تھاں));
        flush_جدول(pd, asid, vaddr, pt);
    }
}

فراغ copyGlobalMappings(pde_t *newPD)
{
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t i;
    pde_t *global_pd = armKSGlobalPD;

    لكل (i = PPTR_ادھار >> ARMSection_بٹ; i < ثنائی_ہندسہ(PD_اشاریہ_بٹ); i++) {
        إذا (i != PD_ASID_تھاں) {
            newPD[i] = global_pd[i];
        }
    }
#جے_انت
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    بدل (vm_faultType) {
    حالة ARMDataAbort: {
        لفظ_t addr, fault;

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (پتہ_Translate_S1(addr) & ~نقاب(ورقہ_بٹ)) | (addr & نقاب(ورقہ_بٹ));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#وإلا
        addr = getFAR();
        fault = getDFSR();
#جے_انت

#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
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
#جے_انت
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        أرجع EXCEPTION_FAULT;
    }

    حالة ARMPrefetchAbort: {
        لفظ_t pc, fault;

        pc = getRestartPC(thread);

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (پتہ_Translate_S1(pc) & ~نقاب(ورقہ_بٹ)) | (pc & نقاب(ورقہ_بٹ));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#وإلا
        fault = getIFSR();
#جے_انت

#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
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
#جے_انت
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        أرجع EXCEPTION_FAULT;
    }

    افتراضي:
        fail("Invalid VM fault type");
    }
}

فراغ deleteASIDPool(asid_t asid_ادھار, asid_pool_t *pool)
{
    بنانشانی صحيح وتھ;

    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_ادھار & نقاب(asid_کم_بٹ)) == 0);

    إذا (armKSASIDTable[ASID_بلند(asid_ادھار)] == pool) {
        لكل (وتھ = 0; وتھ < ثنائی_ہندسہ(asid_کم_بٹ); وتھ++) {
            إذا (pool->array[وتھ]) {
                flushSpace(asid_ادھار + وتھ);
                invalidateASIDEntry(asid_ادھار + وتھ);
            }
        }
        armKSASIDTable[ASID_بلند(asid_ادھار)] = عدم;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

فراغ deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_بلند(asid)];

    إذا (poolPtr != عدم && poolPtr->array[ASID_کم(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_کم(asid)] = عدم;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
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
#جے_انت

فراغ unmap_ورقہ(vm_ورقہ_ناپ_t ورقہ_ناپ, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    لبھو_PDFor_ASID_ret_t لبھو_ret;
    paddr_t addr = addrFromPPtr(pptr);

    لبھو_ret = لبھو_PDFor_ASID(asid);
    إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
        أرجع;
    }

    بدل (ورقہ_ناپ) {
    حالة ARMSmall_ورقہ: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(لبھو_ret.pd, vptr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            أرجع;
        }

        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_تھاں) != pte_pte_small)) {
            أرجع;
        }
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_تھاں) != 0)) {
            أرجع;
        }
#جے_انت
        إذا (unlikely(pte_pte_small_ptr_get_پتہ(lu_ret.pt_تھاں) != addr)) {
            أرجع;
        }

        *(lu_ret.pt_تھاں) = pte_pte_invalid_new();
        cleanByVA_PoU((لفظ_t)lu_ret.pt_تھاں, addrFromPPtr(lu_ret.pt_تھاں));

        اكسر;
    }

    حالة ARMLarge_ورقہ: {
        lookupPTSlot_ret_t lu_ret;
        لفظ_t i;

        lu_ret = lookupPTSlot(لبھو_ret.pd, vptr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            أرجع;
        }
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_تھاں) != pte_pte_large)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_large_ptr_get_پتہ(lu_ret.pt_تھاں) != addr)) {
            أرجع;
        }
#وإلا
        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_تھاں) != pte_pte_small)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_تھاں) != 1)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_small_ptr_get_پتہ(lu_ret.pt_تھاں) != addr)) {
            أرجع;
        }
#جے_انت

        لكل (i = 0; i < ورقے_فی_LARGE_ورقہ; i++) {
            lu_ret.pt_تھاں[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((لفظ_t)&lu_ret.pt_تھاں[0],
                            آخری_BYTE_PTE(lu_ret.pt_تھاں, ورقے_فی_LARGE_ورقہ),
                            addrFromPPtr(&lu_ret.pt_تھاں[0]));

        اكسر;
    }

    حالة ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(لبھو_ret.pd, vptr);

        إذا (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            أرجع;
        }
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pde_pde_section_ptr_get_ناپ(pd) != 0)) {
#وإلا
        إذا (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#جے_انت
            أرجع;
        }
        إذا (unlikely(pde_pde_section_ptr_get_پتہ(pd) != addr)) {
            أرجع;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((لفظ_t)pd, addrFromPPtr(pd));

        اكسر;
    }

    حالة ARMSuperSection: {
        pde_t *pd;
        لفظ_t i;

        pd = lookupPDSlot(لبھو_ret.pd, vptr);

        إذا (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            أرجع;
        }
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pde_pde_section_ptr_get_ناپ(pd) != 1)) {
#وإلا
        إذا (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#جے_انت
            أرجع;
        }
        إذا (unlikely(pde_pde_section_ptr_get_پتہ(pd) != addr)) {
            أرجع;
        }

        لكل (i = 0; i < SECTIONS_فی_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((لفظ_t)&pd[0], آخری_BYTE_PDE(pd, SECTIONS_فی_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        اكسر;
    }

    افتراضي:
        fail("Invalid ARM page type");
        اكسر;
    }

    /* Flush the page now that the mapping has been updated */
    flush_ورقہ(ورقہ_ناپ, لبھو_ret.pd, asid, vptr);
}

فراغ flush_ورقہ(vm_ورقہ_ناپ_t ورقہ_ناپ, pde_t *pd, asid_t asid, لفظ_t vptr)
{
    pde_t stored_hw_asid;
    لفظ_t ادھار_addr;
    bool_t جڑ_switched;

    أكد((vptr & نقاب(ورقہ_بٹ_For_ناپ(ورقہ_ناپ))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    جڑ_switched = سیٹ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = لدو_HWASID(asid);

    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ادھار_addr = vptr & ~نقاب(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(ادھار_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        إذا (جڑ_switched) {
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }
}

فراغ flush_جدول(pde_t *pd, asid_t asid, لفظ_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t جڑ_switched;

    أكد((vptr & نقاب(PT_اشاریہ_بٹ + ARMSmall_ورقہ_بٹ)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    جڑ_switched = سیٹ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = لدو_HWASID(asid);

    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        إذا (جڑ_switched) {
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }
}

فراغ flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = لدو_HWASID(asid);

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

    stored_hw_asid = لدو_HWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    إذا (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ساكن مضمن bool_t CONST checkVPAlignment(vm_ورقہ_ناپ_t sz, لفظ_t w)
{
    أرجع (w & نقاب(ورقہ_بٹ_For_ناپ(sz))) == 0;
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
(paddr_t ادھار, لفظ_t vaddr, vm_ورقہ_ناپ_t چوکھٹا_ناپ,
 vm_rights_t vmRights, vm_گن_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    لفظ_t i;

    بدل (چوکھٹا_ناپ) {

    حالة ARMSmall_ورقہ:

        ret.pte_entries.ادھار = عدم; /* to avoid uninitialised warning */
        ret.pte_entries.لمائی = 1;

        ret.pte = makeUserPTE(ARMSmall_ورقہ, ادھار,
                              vm_گن_get_arm_ورقہ_Cacheable(attr),
                              vm_گن_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت =
                false;
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            أرجع ret;
        }

        ret.pte_entries.ادھار = lu_ret.pt_تھاں;
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_ptr_get_pteType(ret.pte_entries.ادھار) ==
                     pte_pte_large)) {
#وإلا
        إذا (unlikely(pte_ptr_get_pteType(ret.pte_entries.ادھار) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.ادھار))) {
#جے_انت
            current_syscall_error.type =
                se_L4_Delete_پہلا;

            ret.حالت = EXCEPTION_SYSCALL_ERROR;
            أرجع ret;
        }

        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة ARMLarge_ورقہ:

        ret.pte_entries.ادھار = عدم; /* to avoid uninitialised warning */
        ret.pte_entries.لمائی = ورقے_فی_LARGE_ورقہ;

        ret.pte = makeUserPTE(ARMLarge_ورقہ, ادھار,
                              vm_گن_get_arm_ورقہ_Cacheable(attr),
                              vm_گن_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت =
                false;
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            أرجع ret;
        }

        ret.pte_entries.ادھار = lu_ret.pt_تھاں;

        لكل (i = 0; i < ورقے_فی_LARGE_ورقہ; i++) {
            إذا (unlikely(pte_get_pteType(ret.pte_entries.ادھار[i]) ==
                         pte_pte_small)
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.ادھار[i])
#جے_انت
               ) {
                current_syscall_error.type =
                    se_L4_Delete_پہلا;

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
(paddr_t ادھار, لفظ_t vaddr, vm_ورقہ_ناپ_t چوکھٹا_ناپ,
 vm_rights_t vmRights, vm_گن_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_ٹیگ_t currentPDEType;
    لفظ_t i;

    بدل (چوکھٹا_ناپ) {

    /* PDE mappings */
    حالة ARMSection:
        ret.pde_entries.ادھار = lookupPDSlot(pd, vaddr);
        ret.pde_entries.لمائی = 1;

        ret.pde = makeUserPDE(ARMSection, ادھار,
                              vm_گن_get_arm_Parity_Enabled(attr),
                              vm_گن_get_arm_ورقہ_Cacheable(attr),
                              vm_گن_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.ادھار);
        إذا (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_ناپ(ret.pde_entries.ادھار) != 0))) {
#وإلا
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.ادھار) != 0))) {
#جے_انت
            current_syscall_error.type =
                se_L4_Delete_پہلا;
            ret.حالت = EXCEPTION_SYSCALL_ERROR;

            أرجع ret;
        }

        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة ARMSuperSection:
        ret.pde_entries.ادھار = lookupPDSlot(pd, vaddr);
        ret.pde_entries.لمائی = SECTIONS_فی_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, ادھار,
                              vm_گن_get_arm_Parity_Enabled(attr),
                              vm_گن_get_arm_ورقہ_Cacheable(attr),
                              vm_گن_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        لكل (i = 0; i < SECTIONS_فی_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.ادھار[i]);
            إذا (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_ناپ(ret.pde_entries.ادھار[i]) != 1))) {
#وإلا
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.ادھار[i]) != 1))) {
#جے_انت
                current_syscall_error.type =
                    se_L4_Delete_پہلا;
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

ساكن مضمن vptr_t ورقہ_ادھار(vptr_t vaddr, vm_ورقہ_ناپ_t ناپ)
{
    أرجع vaddr & ~نقاب(ورقہ_بٹ_For_ناپ(ناپ));
}

ساكن bool_t خالص pteCheckIfMapped(pte_t *pte)
{
    أرجع pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

ساكن bool_t خالص pdeCheckIfMapped(pde_t *pde)
{
    أرجع pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

ساكن فراغ doFlush(صحيح invLabel, vptr_t شروع, vptr_t سرا, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        سرا = (vptr_t)paddr_منزل_pptr(pstart) + (سرا - شروع);
        شروع = (vptr_t)paddr_منزل_pptr(pstart);
    }
    بدل (invLabel) {
    حالة ARMPDClean_انکڑے:
    حالة ARMPage_Clean_انکڑے:
        cleanCacheRange_RAM(شروع, سرا, pstart);
        اكسر;
    حالة ARMPDInvalidate_انکڑے:
    حالة ARMPage_Invalidate_انکڑے:
        invalidateCacheRange_RAM(شروع, سرا, pstart);
        اكسر;
    حالة ARMPDClean_Invalidate_انکڑے:
    حالة ARMPage_Clean_Invalidate_انکڑے:
        cleanInvalidateCacheRange_RAM(شروع, سرا, pstart);
        اكسر;
    حالة ARMPDUnify_Instruction:
    حالة ARMPageUnify_Instruction:
        /* First clean data lines to point of unification
           (L2 cache)... */
        cleanCacheRange_PoU(شروع, سرا, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(شروع, سرا, pstart);
        /* ...then invalidate branch predictors... */
        branchFlushRange(شروع, سرا, pstart);
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

ساكن exception_t performPDFlush(صحيح invLabel, pde_t *pd, asid_t asid, vptr_t شروع,
                                  vptr_t سرا, paddr_t pstart)
{
    bool_t جڑ_switched;

    /* Flush if given a non zero range */
    إذا (شروع < سرا) {
        جڑ_switched = سیٹ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, شروع, سرا, pstart);

        إذا (جڑ_switched) {
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_جدول_Invocation_Map(cap_t cap, cte_t *ct_تھاں,
                                                 pde_t pde, pde_t *pd_تھاں)
{
    ct_تھاں->cap = cap;
    *pd_تھاں = pde;
    cleanByVA_PoU((لفظ_t)pd_تھاں, addrFromPPtr(pd_تھاں));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_تھاں)
{
    إذا (cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_ورقہ_جدول(
            cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(cap),
            pt);
        clear_حافظہ_PT((فراغ *)pt, cap_get_cap_ناپ_بٹ(cap));
    }
    cap_ورقہ_جدول_cap_ptr_سیٹ_cap_PTIs_Mapped(&(ct_تھاں->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_Invocation_Map_PTE(asid_t asid, cap_t cap, cte_t *ct_تھاں, pte_t pte,
                                               pte_range_t pte_entries)
{
    لفظ_t i, j بن_ورتیا;
    bool_t tlbflush_required;

    ct_تھاں->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.ادھار);

    j = pte_entries.لمائی;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t ادھار_پتہ = pte_pte_small_get_پتہ(pte);
#جے_انت
    لكل (i = 0; i < pte_entries.لمائی; i++) {
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_سیٹ_پتہ(pte, ادھار_پتہ + i * ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(ARMSmall_ورقہ)));
#جے_انت
        pte_entries.ادھار[i] = pte;
    }
    cleanCacheRange_PoU((لفظ_t)pte_entries.ادھار,
                        آخری_BYTE_PTE(pte_entries.ادھار, pte_entries.لمائی),
                        addrFromPPtr(pte_entries.ادھار));
    إذا (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_Invocation_Map_PDE(asid_t asid, cap_t cap, cte_t *ct_تھاں, pde_t pde,
                                               pde_range_t pde_entries)
{
    لفظ_t i, j بن_ورتیا;
    bool_t tlbflush_required;

    ct_تھاں->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.ادھار);

    j = pde_entries.لمائی;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t ادھار_پتہ = pde_pde_section_get_پتہ(pde);
#جے_انت
    لكل (i = 0; i < pde_entries.لمائی; i++) {
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_سیٹ_پتہ(pde, ادھار_پتہ + i * ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(ARMSection)));
#جے_انت
        pde_entries.ادھار[i] = pde;
    }
    cleanCacheRange_PoU((لفظ_t)pde_entries.ادھار,
                        آخری_BYTE_PDE(pde_entries.ادھار, pde_entries.لمائی),
                        addrFromPPtr(pde_entries.ادھار));
    إذا (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_Invocation_Unmap(cap_t cap, cte_t *ct_تھاں)
{
    إذا (generic_چوکھٹا_cap_get_cap_FIs_Mapped(cap)) {
        unmap_ورقہ(generic_چوکھٹا_cap_get_cap_FSize(cap),
                  generic_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                  generic_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                  (فراغ *)generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
    }

    generic_چوکھٹا_cap_ptr_سیٹ_cap_FMapped_پتہ(&ct_تھاں->cap, asidInvalid, 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_Flush(صحيح invLabel, pde_t *pd, asid_t asid, vptr_t شروع,
                                    vptr_t سرا, paddr_t pstart)
{
    bool_t جڑ_switched;

    /* now we can flush. But only if we were given a non zero range */
    إذا (شروع < سرا) {
        جڑ_switched = سیٹ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, شروع, سرا, pstart);

        إذا (جڑ_switched) {
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_ورقہ_Get_پتہ(فراغ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        بنانشانی صحيح لمائی = سیٹ_MR(thread, ipc_بفر, 0, capFBasePtr);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                        se_L4_سنیہا_Info_new(0, 0, 0, لمائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pd_Cap_تھاں)
{
    cap_ورقہ_directory_cap_ptr_سیٹ_cap_PDMapped_ASID(&pd_Cap_تھاں->cap, asid);
    cap_ورقہ_directory_cap_ptr_سیٹ_cap_PDIs_Mapped(&pd_Cap_تھاں->cap, 1);
    poolPtr->array[ASID_کم(asid)] =
        PDE_PTR(cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(pd_Cap_تھاں->cap));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDControlInvocation(فراغ *چوکھٹا, cte_t *تھاں,
                                                cte_t *ماپا, asid_t asid_ادھار)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سیٹ_cap_Free_اشاریہ(&(ماپا->cap),
                                         MAX_FREE_اشاریہ(cap_untyped_cap_get_cap_بلاک_ناپ(ماپا->cap)));

    memzero(چوکھٹا, 1 << ARMSmall_ورقہ_بٹ);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_داخل_کرو(cap_asid_pool_cap_new(asid_ادھار, لفظ_REF(چوکھٹا)),
              ماپا, تھاں);;
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_ادھار & نقاب(asid_کم_بٹ)) == 0);
    armKSASIDTable[ASID_بلند(asid_ادھار)] = (asid_pool_t *)چوکھٹا;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeARMPageDirectoryInvocation(لفظ_t invLabel, لفظ_t لمائی,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    لفظ_t *بفر)
{
    بدل (invLabel) {
    حالة ARMPDClean_انکڑے:
    حالة ARMPDInvalidate_انکڑے:
    حالة ARMPDClean_Invalidate_انکڑے:
    حالة ARMPDUnify_Instruction: {
        vptr_t شروع, سرا;
        paddr_t pstart;
        لبھو_PDFor_ASID_ret_t لبھو_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        إذا (لمائی < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروع = getSyscallArg(0, بفر);
        سرا =   getSyscallArg(1, بفر);

        /* Check sanity of arguments */
        إذا (سرا <= شروع) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        إذا (شروع >= USER_چوٹی || سرا > USER_چوٹی) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(cap_get_capType(cap) != cap_ورقہ_directory_cap ||
                     !cap_ورقہ_directory_cap_get_cap_PDIs_Mapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(cap));
        asid = cap_ورقہ_directory_cap_get_cap_PDMapped_ASID(cap);

        لبھو_ret = لبھو_PDFor_ASID(asid);
        إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(لبھو_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, شروع);

        /* Check that there's actually something there. */
        إذا (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            أرجع EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        إذا (ورقہ_ادھار(شروع, resolve_ret.چوکھٹا_ناپ) !=
            ورقہ_ادھار(سرا - 1, resolve_ret.چوکھٹا_ناپ)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = شروع;
            current_syscall_error.rangeErrorMax =
                ورقہ_ادھار(شروع, resolve_ret.چوکھٹا_ناپ) +
                نقاب(ورقہ_بٹ_For_ناپ(resolve_ret.چوکھٹا_ناپ));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.چوکھٹا_ادھار
                 + (شروع & نقاب(ورقہ_بٹ_For_ناپ(resolve_ret.چوکھٹا_ناپ)));

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        إذا (pstart < phys_ادھار() || pstart > PADDR_چوٹی || (سرا - شروع) > PADDR_چوٹی - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جے_انت

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performPDFlush(invLabel, pd, asid, شروع, سرا - 1, pstart);
    }

    افتراضي:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

}

ساكن exception_t decode_ARMPage_جدول_Invocation(لفظ_t invLabel, لفظ_t لمائی,
                                                cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    لفظ_t vaddr, pd_اشاریہ;

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_گن_t attr;
#جے_انت
    cap_t pdCap;
    pde_t *pd, *pd_تھاں;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    إذا (invLabel == ARMPage_جدول_Unmap) {
        إذا (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_پہلا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely(invLabel != ARMPage_جدول_Map)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(لمائی < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vm_گن_From_لفظ(getSyscallArg(1, بفر));
#جے_انت
    pdCap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(cap_get_capType(pdCap) != cap_ورقہ_directory_cap ||
                 !cap_ورقہ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(pdCap));
    asid = cap_ورقہ_directory_cap_get_cap_PDMapped_ASID(pdCap);

    إذا (unlikely(vaddr >= USER_چوٹی)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_چوٹی);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    {
        لبھو_PDFor_ASID_ret_t لبھو_ret;

        لبھو_ret = لبھو_PDFor_ASID(asid);
        إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(لبھو_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_اشاریہ = vaddr >> (ورقہ_بٹ + PT_اشاریہ_بٹ);
    pd_تھاں = &pd[pd_اشاریہ];
    إذا (unlikely(pde_ptr_get_pdeType(pd_تھاں) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = se_L4_Delete_پہلا;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap)));
#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_گن_get_arm_Parity_Enabled(attr),
              0 /* Domain */
          );
#وإلا
    pde = pde_pde_coarse_new(paddr);
#جے_انت

    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTMapped_پتہ(cap, vaddr);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_ورقہ_جدول_Invocation_Map(cap, cte, pde, pd_تھاں);
}

ساكن exception_t decodeARMFrameInvocation(لفظ_t invLabel, لفظ_t لمائی,
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
        vm_ورقہ_ناپ_t چوکھٹا_ناپ;
        vm_گن_t attr;

        إذا (unlikely(لمائی < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                se_L4_Truncated_سنیہا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, بفر);
        w_rights_نقاب = getSyscallArg(1, بفر);
        attr = vm_گن_From_لفظ(getSyscallArg(2, بفر));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        چوکھٹا_ناپ = generic_چوکھٹا_cap_get_cap_FSize(cap);
        capVMRights = generic_چوکھٹا_cap_get_cap_FVMRights(cap);

        إذا (unlikely(cap_get_capType(pdCap) != cap_ورقہ_directory_cap ||
                     !cap_ورقہ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(
                         pdCap));
        asid = cap_ورقہ_directory_cap_get_cap_PDMapped_ASID(pdCap);

        إذا (generic_چوکھٹا_cap_get_cap_FIs_Mapped(cap)) {
            إذا (generic_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_نمبر = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (generic_چوکھٹا_cap_get_cap_FMapped_پتہ(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            vtop = vaddr + ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(چوکھٹا_ناپ)) - 1;

            إذا (unlikely(vtop >= USER_چوٹی)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_چوٹی);
                current_syscall_error.type =
                    se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            لبھو_PDFor_ASID_ret_t لبھو_ret;

            لبھو_ret = لبھو_PDFor_ASID(asid);
            إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_سروت =
                    false;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (unlikely(لبھو_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_نمبر = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            نقاب_VMRights(capVMRights, rights_From_لفظ(w_rights_نقاب));

        إذا (unlikely(!checkVPAlignment(چوکھٹا_ناپ, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((فراغ *)
                                   generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

        cap = generic_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, asid,
                                                      vaddr);
        إذا (چوکھٹا_ناپ == ARMSmall_ورقہ || چوکھٹا_ناپ == ARMLarge_ورقہ) {
            create_mappings_pte_return_t map_ret;
            map_ret = create_محفوظ_Mapping_Entries_PTE(capFBasePtr, vaddr,
                                                   چوکھٹا_ناپ, vmRights,
                                                   attr, pd);
            إذا (unlikely(map_ret.حالت != EXCEPTION_NONE)) {
#جے_تعریف_شدہ CONFIG_PRINTING
                إذا (current_syscall_error.type == se_L4_Delete_پہلا) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#جے_انت
                أرجع map_ret.حالت;
            }

            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_ورقہ_Invocation_Map_PTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } وإلا {
            create_mappings_pde_return_t map_ret;
            map_ret = create_محفوظ_Mapping_Entries_PDE(capFBasePtr, vaddr,
                                                   چوکھٹا_ناپ, vmRights,
                                                   attr, pd);
            إذا (unlikely(map_ret.حالت != EXCEPTION_NONE)) {
#جے_تعریف_شدہ CONFIG_PRINTING
                إذا (current_syscall_error.type == se_L4_Delete_پہلا) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#جے_انت
                أرجع map_ret.حالت;
            }

            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_ورقہ_Invocation_Map_PDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    حالة ARMPageUnmap: {
#جے_تعریف_شدہ CONFIG_TK1_SMMU
        إذا (is_IOSpace_چوکھٹا_Cap(cap)) {
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_ورقہ_Invocation_Unmap_IO(cap, cte);
        } وإلا
#جے_انت
        {
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_ورقہ_Invocation_Unmap(cap, cte);
        }
    }

#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة ARMPageMapIO: {
        أرجع decodeARMIOMapInvocation(invLabel, لمائی, cte, cap, بفر);
    }
#جے_انت

    حالة ARMPage_Clean_انکڑے:
    حالة ARMPage_Invalidate_انکڑے:
    حالة ARMPage_Clean_Invalidate_انکڑے:
    حالة ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        لبھو_PDFor_ASID_ret_t pd;
        vptr_t شروع, سرا;
        paddr_t pstart;
        لفظ_t ورقہ_ناپ;
        لفظ_t ورقہ_ادھار_2;

        إذا (لمائی < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_چوکھٹا_cap_get_cap_FMapped_ASID(cap);
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap);
#وإلا
        vaddr = generic_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
#جے_انت

        إذا (unlikely(!generic_چوکھٹا_cap_get_cap_FIs_Mapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pd = لبھو_PDFor_ASID(asid);
        إذا (unlikely(pd.حالت != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروع = getSyscallArg(0, بفر);
        سرا =   getSyscallArg(1, بفر);

        /* check that the range is sane */
        إذا (سرا <= شروع) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        ورقہ_ناپ = 1 << ورقہ_بٹ_For_ناپ(generic_چوکھٹا_cap_get_cap_FSize(cap));
        ورقہ_ادھار_2 = addrFromPPtr((فراغ *)generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

#جے_بنا_تعریف CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        إذا (unlikely(!resolve_ret.valid ||
                     resolve_ret.چوکھٹا_ناپ != generic_چوکھٹا_cap_get_cap_FSize(cap) ||
                     resolve_ret.چوکھٹا_ادھار != ورقہ_ادھار_2)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جے_انت

        إذا (شروع >= ورقہ_ناپ || سرا > ورقہ_ناپ) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = ورقہ_ادھار_2 + شروع;
        شروع += vaddr;
        سرا += vaddr;

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        إذا (pstart < phys_ادھار() || pstart > PADDR_چوٹی || (سرا - شروع) > PADDR_چوٹی - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#جے_انت

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_Flush(invLabel, pd.pd, asid, شروع, سرا - 1, pstart);
    }

    حالة ARMPage_Get_پتہ: {


        /* Check that there are enough message registers */
        أكد(n_پیغام_Registers >= 1);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_Get_پتہ((فراغ *)generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(لفظ_t invLabel, لفظ_t لمائی, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_ورقہ_directory_cap:
        أرجع decodeARMPageDirectoryInvocation(invLabel, لمائی, cptr, cte,
                                                cap, بفر);

    حالة cap_ورقہ_جدول_cap:
        أرجع decode_ARMPage_جدول_Invocation(invLabel, لمائی, cte, cap, بفر);

    حالة cap_small_چوکھٹا_cap:
    حالة cap_چوکھٹا_cap:
        أرجع decodeARMFrameInvocation(invLabel, لمائی, cte, cap, call, بفر);

    حالة cap_asid_control_cap: {
        لفظ_t i;
        asid_t asid_ادھار;
        لفظ_t اشاریہ, ڈونگھائی;
        cap_t untyped, جڑ;
        cte_t *ماپا_تھاں, *dest_تھاں;
        lookup_تھاں_ret_t lu_ret;
        فراغ *چوکھٹا;
        exception_t حالت;

        إذا (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(لمائی < 2 || current_extra_caps.excaprefs[0] == عدم
                     || current_extra_caps.excaprefs[1] == عدم)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        اشاریہ = getSyscallArg(0, بفر);
        ڈونگھائی = getSyscallArg(1, بفر);
        ماپا_تھاں = current_extra_caps.excaprefs[0];
        untyped = ماپا_تھاں->cap;
        جڑ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        إذا (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پہلا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_ادھار = i << asid_کم_بٹ;

        إذا (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_بلاک_ناپ(untyped) !=
                     se_L4_ASIDPool_بٹ || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(ماپا_تھاں);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            أرجع حالت;
        }

        چوکھٹا = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_نشانہ_تھاں(جڑ, اشاریہ, ڈونگھائی);
        إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            أرجع lu_ret.حالت;
        }
        dest_تھاں = lu_ret.تھاں;

        حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکھٹا, dest_تھاں,
                                            ماپا_تھاں, asid_ادھار);
    }

    حالة cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pd_Cap_تھاں;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t asid;

        إذا (unlikely(invLabel != ARMASIDPool_سونپو)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pd_Cap_تھاں = current_extra_caps.excaprefs[0];
        pdCap = pd_Cap_تھاں->cap;

        إذا (unlikely(
                cap_get_capType(pdCap) != cap_ورقہ_directory_cap ||
                cap_ورقہ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_بلند(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            current_lookup_fault = lookup_fault_invalid_جڑ_new();

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < (1 << asid_کم_بٹ) && (asid + i == 0 || pool->array[i]); i++);

        إذا (unlikely(i == 1 << asid_کم_بٹ)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = se_L4_Delete_پہلا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, pd_Cap_تھاں);
    }

    افتراضي:
        fail("Invalid ARM arch cap type");
    }
}

#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
exception_t benchmark_arch_map_log_بفر(لفظ_t چوکھٹا_cptr)
{
    lookup_Cap_And_تھاں_ret_t lu_ret;
    vm_ورقہ_ناپ_t چوکھٹا_ناپ;
    pptr_t  چوکھٹا_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_تھاں(گرہ_STATE(ksCurThread), چوکھٹا_cptr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکھٹا_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_چوکھٹا_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_ناپ = generic_چوکھٹا_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکھٹا_ناپ != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_pptr = generic_چوکھٹا_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکھٹا_pptr);

    لكل (صحيح idx = 0; idx < ثنائی_ہندسہ(PT_اشاریہ_بٹ); idx++) {
        paddr_t physical_پتہ = ks_User_Log_بفر + (idx << se_L4_ورقہ_بٹ);

        armKSGlobalLogPT[idx] =
            pte_pte_small_new(
                physical_پتہ,
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
        invalidateTranslationSingle(KS_LOG_PPTR + (idx * ثنائی_ہندسہ(se_L4_ورقہ_بٹ)));
    }

    أرجع EXCEPTION_NONE;
}
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
فراغ kernelPrefetchAbort(لفظ_t pc, لفظ_t lr) VISIBLE;
فراغ kernel_انکڑے_Abort(لفظ_t pc) VISIBLE;

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT

فراغ kernelUndefinedInstruction(لفظ_t pc) VISIBLE;

فراغ kernelPrefetchAbort(لفظ_t pc, لفظ_t lr)
{
    لکھو("\n\nKERNEL PREFETCH ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("HSR: 0x%"SEL4_PRIx_لفظ"\n", getHSR());
    لکھو("LR: 0x%"SEL4_PRIx_لفظ"\n", lr);
    halt();
}

فراغ kernel_انکڑے_Abort(لفظ_t pc)
{
    لکھو("\n\nKERNEL DATA ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("HDFAR: 0x%"SEL4_PRIx_لفظ" HSR: 0x%"SEL4_PRIx_لفظ"\n",
           getHDFAR(), getHSR());
    halt();
}

فراغ kernelUndefinedInstruction(لفظ_t pc)
{
    لکھو("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("HSR: 0x%"SEL4_PRIx_لفظ"\n", getHSR());
    halt();
}

#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */

فراغ kernelPrefetchAbort(لفظ_t pc, لفظ_t lr)
{
    لکھو("\n\nKERNEL PREFETCH ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("IFSR: 0x%"SEL4_PRIx_لفظ"\n", getIFSR());
    لکھو("LR: 0x%"SEL4_PRIx_لفظ"\n", lr);
    halt();
}

فراغ kernel_انکڑے_Abort(لفظ_t pc)
{
    لکھو("\n\nKERNEL DATA ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("FAR: 0x%"SEL4_PRIx_لفظ" DFSR: 0x%"SEL4_PRIx_لفظ"\n",
           getFAR(), getDFSR());
    halt();
}
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#جے_انت

#جے_تعریف_شدہ CONFIG_PRINTING
تعريفنوع بنية read_لفظ_From_VSpace_ret {
    exception_t حالت;
    لفظ_t قدر;
} read_لفظ_From_VSpace_ret_t;

ساكن read_لفظ_From_VSpace_ret_t read_لفظ_From_VSpace(pde_t *pd, لفظ_t vaddr)
{
    read_لفظ_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_تھاں;
    pde_t *pd_تھاں;
    paddr_t paddr;
    لفظ_t وتھ;
    pptr_t kernel_vaddr;
    لفظ_t *قدر;

    pd_تھاں = lookupPDSlot(pd, vaddr);
    إذا (pde_ptr_get_pdeType(pd_تھاں) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_پتہ(pd_تھاں);
        وتھ = vaddr & نقاب(ARMSection_بٹ);
    } وإلا {
        pt_تھاں = lookupPTSlot(pd, vaddr);
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (pt_تھاں.حالت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_تھاں.pt_تھاں) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_پتہ(pt_تھاں.pt_تھاں);
            إذا (pte_pte_small_ptr_get_contiguous_hint(pt_تھاں.pt_تھاں)) {
                وتھ = vaddr & نقاب(ARMLarge_ورقہ_بٹ);
            } وإلا {
                وتھ = vaddr & نقاب(ARMSmall_ورقہ_بٹ);
            }
#وإلا
        إذا (pt_تھاں.حالت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_تھاں.pt_تھاں) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_پتہ(pt_تھاں.pt_تھاں);
            وتھ = vaddr & نقاب(ARMSmall_ورقہ_بٹ);
        } وإلا إذا (pt_تھاں.حالت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_تھاں.pt_تھاں) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_پتہ(pt_تھاں.pt_تھاں);
            وتھ = vaddr & نقاب(ARMLarge_ورقہ_بٹ);
#جے_انت
        } وإلا {
            ret.حالت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }
    }


    kernel_vaddr = (لفظ_t)paddr_منزل_pptr(paddr);
    قدر = (لفظ_t *)(kernel_vaddr + وتھ);
    ret.حالت = EXCEPTION_NONE;
    ret.قدر = *قدر;
    أرجع ret;
}

فراغ Arch_user_ڈھیر_Trace(tcb_t *tptr)
{
    cap_t thread_جڑ;
    pde_t *pd;
    لفظ_t sp;
    صحيح i;

    thread_جڑ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    إذا (cap_get_capType(thread_جڑ) != cap_ورقہ_directory_cap) {
        لکھو("Invalid vspace\n");
        أرجع;
    }

    pd = (pde_t *)pptr_دا_cap(thread_جڑ);

    sp = get_رجسٹر(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_لفظ_ناپ_بٹ)) {
        لکھو("SP not aligned\n");
        أرجع;
    }

    لكل (i = 0; i < CONFIG_USER_ڈھیر_TRACE_لمائی; i++) {
        لفظ_t پتہ = sp + (i * حجم(لفظ_t));
        read_لفظ_From_VSpace_ret_t نتیجہ;
        نتیجہ = read_لفظ_From_VSpace(pd, پتہ);
        إذا (نتیجہ.حالت == EXCEPTION_NONE) {
            لکھو("0x%lx: 0x%lx\n", (لما)پتہ, (لما)نتیجہ.قدر);
        } وإلا {
            لکھو("0x%lx: INVALID\n", (لما)پتہ);
        }
    }
}
#جے_انت
