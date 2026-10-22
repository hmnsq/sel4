/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <benchmark/benchmark.h>
#شامل <api/failures.h>
#شامل <api/syscall.h>
#شامل <kernel/boot.h>
#شامل <kernel/cspace.h>
#شامل <kernel/thread.h>
#شامل <kernel/stack.h>
#شامل <machine/io.h>
#شامل <machine/debug.h>
#شامل <model/statedata.h>
#شامل <object/cnode.h>
#شامل <object/untyped.h>
#شامل <arch/api/invocation.h>
#شامل <arch/kernel/vspace.h>
#شامل <linker.h>
#شامل <plat/machine/hardware.h>
#شامل <armv/context_switch.h>
#شامل <arch/object/iospace.h>
#شامل <arch/object/vcpu.h>
#شامل <arch/machine/tlb.h>

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#شامل <benchmark/benchmark_track.h>
#اگر_ختم

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#تعریف_کرو SECTIONS_فی_SUPER_SECTION ثنائی_ہندسہ(ARMSuper_Section_بٹ - ARMSection_بٹ)
#تعریف_کرو صفحات_فی_LARGE_صفحہ ثنائی_ہندسہ(ARMLarge_صفحہ_بٹ - ARMSmall_صفحہ_بٹ)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#تعریف_کرو آخری_BYTE_PTE(PTE,لمبائی_2) ((لفظ_t)&(PTE)[(لمبائی_2)-1] + (ثنائی_ہندسہ(PTE_سائز_بٹ)-1))
#تعریف_کرو آخری_BYTE_PDE(PDE,لمبائی_2) ((لفظ_t)&(PDE)[(لمبائی_2)-1] + (ثنائی_ہندسہ(PDE_سائز_بٹ)-1))

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#تعریف_کرو MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#تعریف_کرو MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#تعریف_کرو ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#تعریف_کرو ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ساکن خالی سیٹ_Global_PD(خالی);

ساخت resolve_ret {
    paddr_t چوکھٹا_بنیاد;
    vm_صفحہ_سائز_t چوکھٹا_سائز;
    bool_t valid;
};
قسمتعریف ساخت resolve_ret resolve_ret_t;

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
ساکن bool_t خالص pteCheckIfMapped(pte_t *pte);
ساکن bool_t خالص pdeCheckIfMapped(pde_t *pde);

ساکن لفظ_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    انتخاب (vm_rights) {
    حالت VMNoAccess:
        واپس 0;

    حالت VMKernelOnly:
        واپس 1;

    حالت VMReadOnly:
        واپس 2;

    حالت VMReadWrite:
        واپس 3;

    طےشدہ:
        fail("Invalid VM rights");
    }
}

#ورنہ
/* AP encoding slightly different. AP only used for kernel mappings which are fixed after boot time */
BOOT_رمز
ساکن لفظ_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    انتخاب (vm_rights) {
    حالت VMKernelOnly:
        واپس 0;
    حالت VMReadWrite:
        واپس 1;
    حالت VMNoAccess:
        /* RO at PL1 only */
        واپس 2;
    حالت VMReadOnly:
        واپس 3;
    طےشدہ:
        fail("Invalid VM rights");
    }
}

ساکن لفظ_t CONST HAPFromVMRights(vm_rights_t vm_rights)
{
    انتخاب (vm_rights) {
    حالت VMKernelOnly:
    حالت VMNoAccess:
        واپس 0;
    حالت VMReadOnly:
        واپس 1;
    /*
    case VMWriteOnly:
        return 2;
    */
    حالت VMReadWrite:
        واپس 3;
    طےشدہ:
        fail("Invalid VM rights");
    }
}

#اگر_ختم

vm_rights_t CONST نقاب_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_نقاب)
{
    اگر (vm_rights == VMNoAccess) {
        واپس VMNoAccess;
    }
    اگر (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        واپس VMReadOnly;
    }
    اگر (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        اگر (!seL4_CapRights_get_capAllowWrite(cap_rights_نقاب)) {
            واپس VMReadOnly;
        } ورنہ {
            واپس VMReadWrite;
        }
    }
    اگر (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_نقاب) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_نقاب)) {
        userError("Attempted to make unsupported write only mapping");
    }
    واپس VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_رمز خالی map_kernel_چوکھٹا(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_اوصاف_t اوصاف)
{
    لفظ_t idx = (vaddr & نقاب(صفحہ_بٹ_For_سائز(ARMSection))) >> صفحہ_بٹ_For_سائز(ARMSmall_صفحہ);

    أكد(vaddr >= PPTR_چوٹی); /* vaddr lies in the region the global PT covers */
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t tex;
    اگر (vm_اوصاف_get_arm_صفحہ_Cacheable(اوصاف)) {
        tex = 5; /* TEX = 0b1(Cached)01(Outer Write Allocate) */
    } ورنہ {
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
#ورنہ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
            vm_اوصاف_get_arm_صفحہ_Cacheable(اوصاف)
        );
#اگر_ختم /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_رمز خالی map_kernel_کھڑکی(خالی)
{
    paddr_t  phys;
    لفظ_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = phys_بنیاد();
    idx = PPTR_بنیاد >> صفحہ_بٹ_For_سائز(ARMSection);

    جب (idx < ثنائی_ہندسہ(PD_اشاریہ_بٹ) - SECTIONS_فی_SUPER_SECTION) {
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
        برائے (idx2 = idx; idx2 < idx + SECTIONS_فی_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += ثنائی_ہندسہ(صفحہ_بٹ_For_سائز(ARMSuperSection));
        idx += SECTIONS_فی_SUPER_SECTION;
    }

    جب (idx < (PPTR_چوٹی >> صفحہ_بٹ_For_سائز(ARMSection))) {
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
        phys += ثنائی_ہندسہ(صفحہ_بٹ_For_سائز(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    أكد(phys == PADDR_چوٹی);

#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /*پتہ*/
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ));

    phys += ثنائی_ہندسہ(صفحہ_بٹ_For_سائز(ARMSection));
    idx++;
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */

    /* map page table covering last 1M of virtual address space to page directory */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalPT), /*پتہ*/
            true,                        /* P       */
            0                            /* Domain  */
        );

    /* now start initialising the page table */
    memzero(armKSGlobalPT, 1 << se_L4_صفحہ_جدول_بٹ);

    /* map vector table */
    map_kernel_چوکھٹا(
        addrFromKPPtr(arm_سمتیہ_جدول),
        PPTR_سمتیہ_جدول,
        VMKernelOnly,
        vm_اوصاف_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#ورنہ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_رمز خالی map_kernel_کھڑکی(خالی)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t غیر_مستعمل pteS2;

    /* Initialise PGD */
    برائے (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    برائے (idx = 0; idx < (USER_چوٹی - 0xC0000000) >> (PT_اشاریہ_بٹ + صفحہ_بٹ); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = phys_بنیاد();
    برائے (; idx < ثنائی_ہندسہ(PT_اشاریہ_بٹ) - 1; idx++) {
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
        phys += ثنائی_ہندسہ(PT_اشاریہ_بٹ + صفحہ_بٹ);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << se_L4_صفحہ_جدول_بٹ);
    برائے (idx = 0; idx < 256; idx++) {
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
        phys += ثنائی_ہندسہ(صفحہ_بٹ);
    }
    /* map vector table */
    map_kernel_چوکھٹا(
        addrFromKPPtr(arm_سمتیہ_جدول),
        PPTR_سمتیہ_جدول,
        VMKernelOnly,
        vm_اوصاف_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#اگر_ختم /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

ساکن BOOT_رمز خالی map_it_چوکھٹا_cap(cap_t pd_cap, cap_t چوکھٹا_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *ہدف_خانہ;
    pde_t *pd    = PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    خالی  *چوکھٹا = (خالی *)generic_چوکھٹا_cap_get_cap_FBase_Ptr(چوکھٹا_cap);
    vptr_t vptr  = generic_چوکھٹا_cap_get_cap_FMapped_پتہ(چوکھٹا_cap);

    أكد(generic_چوکھٹا_cap_get_cap_FMapped_ASID(چوکھٹا_cap) != 0);

    pd += (vptr >> صفحہ_بٹ_For_سائز(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pd));
    ہدف_خانہ = pt + ((vptr & نقاب(صفحہ_بٹ_For_سائز(ARMSection)))
                       >> صفحہ_بٹ_For_سائز(ARMSmall_صفحہ));
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    *ہدف_خانہ = pte_pte_small_new(
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
#ورنہ
    *ہدف_خانہ = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(چوکھٹا),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#اگر_ختم
}

/* Create a frame cap for the initial thread. */

ساکن BOOT_رمز cap_t create_it_چوکھٹا_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    اگر (use_large)
        واپس
            cap_چوکھٹا_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_کم(asid),                /* capFMappedASIDLow  */
                لفظ_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_بلند(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    ورنہ
        واپس
            cap_small_چوکھٹا_cap_new(
                ASID_کم(asid),                /* capFMappedASIDLow  */
                لفظ_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#اگر_ختم
                ASID_بلند(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

ساکن BOOT_رمز خالی map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(pt_cap);
    pde_t *ہدف_خانہ = pd + (vptr >> صفحہ_بٹ_For_سائز(ARMSection));

    أكد(cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(pt_cap));

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    *ہدف_خانہ = pde_pde_coarse_new(
                      addrFromPPtr(pt), /*پتہ*/
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#ورنہ
    *ہدف_خانہ = pde_pde_coarse_new(addrFromPPtr(pt));
#اگر_ختم
}

/* Create a page table for the initial thread */

ساکن BOOT_رمز cap_t create_it_صفحہ_جدول_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحہ_جدول_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    اگر (asid != asidInvalid) {
        map_it_pt_cap(pd, cap);
    }
    واپس cap;
}

BOOT_رمز لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    واپس get_n_paging(it_v_reg, PT_اشاریہ_بٹ + صفحہ_بٹ);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_رمز cap_t create_it_پتہ_space(cap_t جڑ_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    se_L4_خانہ_Pos خانہ_pos_before;
    se_L4_خانہ_Pos خانہ_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << se_L4_صفحہ_Dir_بٹ) - 1,
                        addrFromPPtr((خالی *)rootserver.vspace));
    cap_t pd_cap =
        cap_صفحہ_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    خانہ_pos_before = ndks_boot.خانہ_pos_cur;
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    برائے (pt_vptr = باری_نیچے_کو(it_v_reg.آغاز, PT_اشاریہ_بٹ + صفحہ_بٹ);
         pt_vptr < it_v_reg.انتہا;
         pt_vptr += ثنائی_ہندسہ(PT_اشاریہ_بٹ + صفحہ_بٹ)) {
        اگر (!provide_cap(جڑ_cnode_cap,
                         create_it_صفحہ_جدول_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            واپس cap_null_cap_new();
        }
    }

    خانہ_pos_after = ndks_boot.خانہ_pos_cur;
    ndks_boot.bi_چوکھٹا->userImagePaging = (se_L4_خانہ_Region) {
        خانہ_pos_before, خانہ_pos_after
    };

    واپس pd_cap;
}

BOOT_رمز cap_t create_unmapped_it_چوکھٹا_cap(pptr_t pptr, bool_t use_large)
{
    واپس create_it_چوکھٹا_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_رمز cap_t create_mapped_it_چوکھٹا_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_چوکھٹا_cap(pptr, vptr, asid, use_large);
    map_it_چوکھٹا_cap(pd_cap, cap, executable);
    واپس cap;
}

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_رمز خالی activate_kernel_vspace(خالی)
{
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    سیٹ_Global_PD();
    invalidateLocalTLB();
    تالا_TLBEntry(PPTR_بنیاد);
    تالا_TLBEntry(PPTR_سمتیہ_جدول);
}

#ورنہ

BOOT_رمز خالی activate_kernel_vspace(خالی)
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
#اگر 0 /* Can't lock entries on A15 */
    تالا_TLBEntry(PPTR_بنیاد);
    تالا_TLBEntry(PPTR_سمتیہ_جدول);
#اگر_ختم
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

#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_رمز خالی write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_کا_cap(it_ap_cap));
    ap->array[ASID_کم(IT_ASID)] = PDE_PTR(pptr_کا_cap(it_pd_cap));
    armKSASIDTable[ASID_بلند(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_PDFor_ASID(asid_t asid)
{
    ڈھونڈو_PDFor_ASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_بلند(asid)];
    اگر (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.pd = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    pd = poolPtr->array[ASID_کم(asid)];
    اگر (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.pd = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    ret.pd = pd;
    ret.حالت_2 = EXCEPTION_NONE;
    واپس ret;
}

لفظ_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    لفظ_t w_بفر_Ptr;
    cap_t بفر_Cap;
    vm_rights_t vm_rights;

    w_بفر_Ptr = thread->tcbIPCBuffer;
    بفر_Cap = TCB_PTR_CTE_PTR(thread, tcb_بفر)->cap;

    اگر (unlikely(cap_get_capType(بفر_Cap) != cap_small_چوکھٹا_cap &&
                 cap_get_capType(بفر_Cap) != cap_چوکھٹا_cap)) {
        واپس تہی;
    }
    اگر (unlikely(generic_چوکھٹا_cap_get_cap_FIs_Device(بفر_Cap))) {
        واپس تہی;
    }

    vm_rights = generic_چوکھٹا_cap_get_cap_FVMRights(بفر_Cap);
    اگر (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        لفظ_t بنیاد_Ptr;
        غیرعلامتی صحیح صفحہ_بٹ_2;

        بنیاد_Ptr = generic_چوکھٹا_cap_get_cap_FBase_Ptr(بفر_Cap);
        صفحہ_بٹ_2 = صفحہ_بٹ_For_سائز(generic_چوکھٹا_cap_get_cap_FSize(بفر_Cap));
        واپس (لفظ_t *)(بنیاد_Ptr + (w_بفر_Ptr & نقاب(صفحہ_بٹ_2)));
    } ورنہ {
        واپس تہی;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    اگر (unlikely(cap_get_capType(cap) != cap_small_چوکھٹا_cap &&
                 cap_get_capType(cap) != cap_چوکھٹا_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (unlikely(generic_چوکھٹا_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(vptr & نقاب(se_L4_IPCBuffer_سائز_بٹ))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (صحیح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    واپس EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    غیرعلامتی صحیح pd_اشاریہ;

    pd_اشاریہ = vptr >> (صفحہ_بٹ + PT_اشاریہ_بٹ);
    واپس pd + pd_اشاریہ;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pd_خانہ;

    pd_خانہ = lookupPDSlot(pd, vptr);

    اگر (unlikely(pde_ptr_get_pdeType(pd_خانہ) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_اشاریہ_بٹ + صفحہ_بٹ);

        ret.pt_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    } ورنہ {
        pte_t *pt, *pt_خانہ;
        غیرعلامتی صحیح pt_اشاریہ;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pd_خانہ));
        pt_اشاریہ = (vptr >> صفحہ_بٹ) & نقاب(PT_اشاریہ_بٹ);
        pt_خانہ = pt + pt_اشاریہ;

        ret.pt_خانہ = pt_خانہ;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
    }
}

ساکن pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    غیرعلامتی صحیح pt_اشاریہ;

    pt_اشاریہ = (vptr >> صفحہ_بٹ) & نقاب(PT_اشاریہ_بٹ);
    واپس pt + pt_اشاریہ;
}

ساکن ثابت resolve_ret_t default_resolve_ret_t;

ساکن resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    انتخاب (pde_ptr_get_pdeType(pde)) {
    حالت pde_pde_section:
        ret.چوکھٹا_بنیاد = pde_pde_section_ptr_get_پتہ(pde);
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        اگر (pde_pde_section_ptr_get_سائز(pde)) {
            ret.چوکھٹا_سائز = ARMSuperSection;
        } ورنہ {
            ret.چوکھٹا_سائز = ARMSection;
        }
#ورنہ
        اگر (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.چوکھٹا_بنیاد &= ~نقاب(صفحہ_بٹ_For_سائز(ARMSuperSection));
            ret.چوکھٹا_سائز = ARMSuperSection;
        } ورنہ {
            ret.چوکھٹا_سائز = ARMSection;
        }
#اگر_ختم
        واپس ret;

    حالت pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_پتہ(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        انتخاب (pte_ptr_get_pteType(pte)) {
        حالت pte_pte_small:
            ret.چوکھٹا_بنیاد = pte_pte_small_ptr_get_پتہ(pte);
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            اگر (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.چوکھٹا_بنیاد &= ~نقاب(صفحہ_بٹ_For_سائز(ARMLarge_صفحہ));
                ret.چوکھٹا_سائز = ARMLarge_صفحہ;
            } ورنہ {
                ret.چوکھٹا_سائز = ARMSmall_صفحہ;
            }
#ورنہ
            ret.چوکھٹا_سائز = ARMSmall_صفحہ;
#اگر_ختم
            واپس ret;

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        حالت pte_pte_large:
            ret.چوکھٹا_بنیاد = pte_pte_large_ptr_get_پتہ(pte);
            ret.چوکھٹا_سائز = ARMLarge_صفحہ;
            واپس ret;
#اگر_ختم
        طےشدہ:
            توڑو;
        }
        توڑو;
    }
    }

    ret.valid = false;
    واپس ret;
}

ساکن pte_t CONST makeUserPTE(vm_صفحہ_سائز_t صفحہ_سائز, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t ap;

    ap = APFromVMRights(vm_rights);

    انتخاب (صفحہ_سائز) {
    حالت ARMSmall_صفحہ: {
        اگر (cacheable) {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    nonexecutable);
        } ورنہ {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    0, /* TEX = 0b000, strongly-ordered. */
                                    ap,
                                    0, 0,
                                    nonexecutable);
        }
        توڑو;
    }

    حالت ARMLarge_صفحہ: {
        اگر (cacheable) {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    1 /* reserved */);
        } ورنہ {
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
        توڑو;
    }

    طےشدہ:
        fail("Invalid PTE frame type");
    }

#ورنہ

    لفظ_t hap;

    hap = HAPFromVMRights(vm_rights);

    انتخاب (صفحہ_سائز) {
    حالت ARMSmall_صفحہ: {
        اگر (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_CACHEABLE /* Cacheable */);
        } ورنہ {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        توڑو;
    }

    حالت ARMLarge_صفحہ: {
        اگر (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_CACHEABLE  /* Cacheable */);
        } ورنہ {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        توڑو;
    }
    طےشدہ:
        fail("Invalid PTE frame type");
    }
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    واپس pte;
}

ساکن pde_t CONST makeUserPDE(vm_صفحہ_سائز_t صفحہ_سائز, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, لفظ_t domain,
                               vm_rights_t vm_rights)
{
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t ap, size2;

    ap = APFromVMRights(vm_rights);
#ورنہ
    لفظ_t hap, size2;

    (خالی)domain;
    hap = HAPFromVMRights(vm_rights);
#اگر_ختم

    انتخاب (صفحہ_سائز) {
    حالت ARMSection:
        size2 = 0;
        توڑو;

    حالت ARMSuperSection:
        size2 = 1;
        توڑو;

    طےشدہ:
        fail("Invalid PDE frame type");
    }

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    اگر (cacheable) {
        واپس pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                   0, /* APX = 0, privileged full access */
                                   5, /* TEX = 0b101, outer write-back, write-allocate */
                                   ap, parity, domain, nonexecutable,
                                   0, 1 /* Inner write-back, write-allocate */);
    } ورنہ {
        واپس pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   1, /* shared */
                                   0, /* APX = 0, privileged full access */
                                   0, /* TEX = 0b000, strongly-ordered */
                                   ap, parity, domain, nonexecutable,
                                   0, 0);
    }
#ورنہ
    اگر (cacheable) {
        واپس pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_CACHEABLE /* Cacheable */);
    } ورنہ {
        واپس pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_NONCACHEABLE /* Not cacheable */);
    }
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_جڑ(cap_t cap)
{
    واپس cap_get_capType(cap) == cap_صفحہ_directory_cap &&
           cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(cap);
}

bool_t CONST is_IOSpace_چوکھٹا_Cap(cap_t cap)
{
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    واپس cap_get_capType(cap) == cap_small_چوکھٹا_cap && cap_small_چوکھٹا_cap_get_cap_FIs_IOSpace(cap);
#ورنہ
    واپس false;
#اگر_ختم
}

/** Switch to the empty PD on the reserved HW ASID */
ساکن خالی سیٹ_Global_PD(خالی)
{
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    writeContextIDAndPD(hwASIDReserved, addrFromKPPtr(armUSGlobalPD));
#ورنہ
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
#اگر_ختم
}

خالی سیٹ_VMRoot(tcb_t *tcb)
{
    cap_t thread_جڑ;
    asid_t asid;
    pde_t *pd;
    ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret;

    thread_جڑ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    اگر (cap_get_capType(thread_جڑ) != cap_صفحہ_directory_cap ||
        !cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(thread_جڑ)) {
        سیٹ_Global_PD();
        واپس;
    }

    pd = PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(thread_جڑ));
    asid = cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(thread_جڑ);
    ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(asid);
    اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE || ڈھونڈو_ret.pd != pd)) {
        سیٹ_Global_PD();
        واپس;
    }

    armv_contextSwitch(pd, asid);
}

ساکن bool_t سیٹ_VMRoot_For_Flush(pde_t *pd, asid_t asid)
{
    cap_t thread_جڑ;

    thread_جڑ = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbVTable)->cap;

    اگر (cap_get_capType(thread_جڑ) == cap_صفحہ_directory_cap &&
        cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(thread_جڑ) &&
        PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(thread_جڑ)) == pd) {
        واپس false;
    }

    armv_contextSwitch(pd, asid);

    واپس true;
}

pde_t *صفحہ_جدول_Mapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret;
    pde_t pde;
    غیرعلامتی صحیح pd_اشاریہ;

    ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(asid);
    اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
        واپس تہی;
    }

    pd_اشاریہ = vaddr >> (صفحہ_بٹ + PT_اشاریہ_بٹ);
    pde = ڈھونڈو_ret.pd[pd_اشاریہ];

    اگر (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_پتہ(pde)) == pt)) {
        واپس ڈھونڈو_ret.pd;
    } ورنہ {
        واپس تہی;
    }
}

ساکن خالی invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_کم(asid)];
    أكد(pd);

    pd[PD_ASID_خانہ] = pde_pde_invalid_new(0, false);
}

ساکن pde_t خالص بھرو_HWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_کم(asid)];
    أكد(pd);

    واپس pd[PD_ASID_خانہ];
}

ساکن خالی storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بلند(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_کم(asid)];
    أكد(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_خانہ] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t ڈھونڈو_Free_HWASID(خالی)
{
    لفظ_t hw_asid_انحراف;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    برائے (hw_asid_انحراف = 0;
         hw_asid_انحراف <= (لفظ_t)((hw_asid_t) - 1);
         hw_asid_انحراف ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_انحراف);
        اگر (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            واپس hw_asid;
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
    اگر (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    واپس hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = بھرو_HWASID(asid);
    اگر (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        واپس pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } ورنہ {
        hw_asid_t new_hw_asid;

        new_hw_asid = ڈھونڈو_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        واپس new_hw_asid;
    }
}

ساکن خالی invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = بھرو_HWASID(asid);
    اگر (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

خالی unmap_صفحہ_جدول(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pd_خانہ;
    غیرعلامتی صحیح pd_اشاریہ;

    pd = صفحہ_جدول_Mapped(asid, vaddr, pt);

    اگر (likely(pd != تہی)) {
        pd_اشاریہ = vaddr >> (PT_اشاریہ_بٹ + صفحہ_بٹ);
        pd_خانہ = pd + pd_اشاریہ;

        *pd_خانہ = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((لفظ_t)pd_خانہ, addrFromPPtr(pd_خانہ));
        flush_جدول(pd, asid, vaddr, pt);
    }
}

خالی copyGlobalMappings(pde_t *newPD)
{
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t i;
    pde_t *global_pd = armKSGlobalPD;

    برائے (i = PPTR_بنیاد >> ARMSection_بٹ; i < ثنائی_ہندسہ(PD_اشاریہ_بٹ); i++) {
        اگر (i != PD_ASID_خانہ) {
            newPD[i] = global_pd[i];
        }
    }
#اگر_ختم
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    انتخاب (vm_faultType) {
    حالت ARMDataAbort: {
        لفظ_t addr, fault;

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (پتہ_Translate_S1(addr) & ~نقاب(صفحہ_بٹ)) | (addr & نقاب(صفحہ_بٹ));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#ورنہ
        addr = getFAR();
        fault = getDFSR();
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
        /* Debug exceptions come in on the Prefetch and Data abort vectors.
         * We have to test the fault-status bits in the IFSR/DFSR to determine
         * if it's a debug exception when one occurs.
         *
         * If it is a debug exception, return early and don't fallthrough to the
         * normal VM Fault handling path.
         */
        اگر (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(0);
            واپس EXCEPTION_FAULT;
        }
#اگر_ختم
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        واپس EXCEPTION_FAULT;
    }

    حالت ARMPrefetchAbort: {
        لفظ_t pc, fault;

        pc = getRestartPC(thread);

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (پتہ_Translate_S1(pc) & ~نقاب(صفحہ_بٹ)) | (pc & نقاب(صفحہ_بٹ));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#ورنہ
        fault = getIFSR();
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
        اگر (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            اگر (seL4_Fault_DebugException_get_exceptionReason(current_fault) == se_L4_Single_قدم
                && !single_قدم_Fault_شمار_Ready(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                واپس EXCEPTION_NONE;
            }
            واپس EXCEPTION_FAULT;
        }
#اگر_ختم
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        واپس EXCEPTION_FAULT;
    }

    طےشدہ:
        fail("Invalid VM fault type");
    }
}

خالی deleteASIDPool(asid_t asid_بنیاد, asid_pool_t *pool)
{
    غیرعلامتی صحیح انحراف;

    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنیاد & نقاب(asid_کم_بٹ)) == 0);

    اگر (armKSASIDTable[ASID_بلند(asid_بنیاد)] == pool) {
        برائے (انحراف = 0; انحراف < ثنائی_ہندسہ(asid_کم_بٹ); انحراف++) {
            اگر (pool->array[انحراف]) {
                flushSpace(asid_بنیاد + انحراف);
                invalidateASIDEntry(asid_بنیاد + انحراف);
            }
        }
        armKSASIDTable[ASID_بلند(asid_بنیاد)] = تہی;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

خالی deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_بلند(asid)];

    اگر (poolPtr != تہی && poolPtr->array[ASID_کم(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_کم(asid)] = تہی;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
ساکن pte_t pte_pte_invalid_new(خالی)
{
    /* Invalid as every PTE must have bit 0 set (large PTE) or bit 1 set (small
     * PTE). 0 == 'translation fault' in ARM parlance.
     */
    واپس (pte_t) {
        {
            0
        }
    };
}
#اگر_ختم

خالی unmap_صفحہ(vm_صفحہ_سائز_t صفحہ_سائز, asid_t asid, vptr_t vptr, خالی *pptr)
{
    ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret;
    paddr_t addr = addrFromPPtr(pptr);

    ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(asid);
    اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
        واپس;
    }

    انتخاب (صفحہ_سائز) {
    حالت ARMSmall_صفحہ: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(ڈھونڈو_ret.pd, vptr);
        اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
            واپس;
        }

        اگر (unlikely(pte_ptr_get_pteType(lu_ret.pt_خانہ) != pte_pte_small)) {
            واپس;
        }
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        اگر (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_خانہ) != 0)) {
            واپس;
        }
#اگر_ختم
        اگر (unlikely(pte_pte_small_ptr_get_پتہ(lu_ret.pt_خانہ) != addr)) {
            واپس;
        }

        *(lu_ret.pt_خانہ) = pte_pte_invalid_new();
        cleanByVA_PoU((لفظ_t)lu_ret.pt_خانہ, addrFromPPtr(lu_ret.pt_خانہ));

        توڑو;
    }

    حالت ARMLarge_صفحہ: {
        lookupPTSlot_ret_t lu_ret;
        لفظ_t i;

        lu_ret = lookupPTSlot(ڈھونڈو_ret.pd, vptr);
        اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
            واپس;
        }
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        اگر (unlikely(pte_ptr_get_pteType(lu_ret.pt_خانہ) != pte_pte_large)) {
            واپس;
        }
        اگر (unlikely(pte_pte_large_ptr_get_پتہ(lu_ret.pt_خانہ) != addr)) {
            واپس;
        }
#ورنہ
        اگر (unlikely(pte_ptr_get_pteType(lu_ret.pt_خانہ) != pte_pte_small)) {
            واپس;
        }
        اگر (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_خانہ) != 1)) {
            واپس;
        }
        اگر (unlikely(pte_pte_small_ptr_get_پتہ(lu_ret.pt_خانہ) != addr)) {
            واپس;
        }
#اگر_ختم

        برائے (i = 0; i < صفحات_فی_LARGE_صفحہ; i++) {
            lu_ret.pt_خانہ[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((لفظ_t)&lu_ret.pt_خانہ[0],
                            آخری_BYTE_PTE(lu_ret.pt_خانہ, صفحات_فی_LARGE_صفحہ),
                            addrFromPPtr(&lu_ret.pt_خانہ[0]));

        توڑو;
    }

    حالت ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(ڈھونڈو_ret.pd, vptr);

        اگر (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            واپس;
        }
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        اگر (unlikely(pde_pde_section_ptr_get_سائز(pd) != 0)) {
#ورنہ
        اگر (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#اگر_ختم
            واپس;
        }
        اگر (unlikely(pde_pde_section_ptr_get_پتہ(pd) != addr)) {
            واپس;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((لفظ_t)pd, addrFromPPtr(pd));

        توڑو;
    }

    حالت ARMSuperSection: {
        pde_t *pd;
        لفظ_t i;

        pd = lookupPDSlot(ڈھونڈو_ret.pd, vptr);

        اگر (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            واپس;
        }
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        اگر (unlikely(pde_pde_section_ptr_get_سائز(pd) != 1)) {
#ورنہ
        اگر (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#اگر_ختم
            واپس;
        }
        اگر (unlikely(pde_pde_section_ptr_get_پتہ(pd) != addr)) {
            واپس;
        }

        برائے (i = 0; i < SECTIONS_فی_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((لفظ_t)&pd[0], آخری_BYTE_PDE(pd, SECTIONS_فی_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        توڑو;
    }

    طےشدہ:
        fail("Invalid ARM page type");
        توڑو;
    }

    /* Flush the page now that the mapping has been updated */
    flush_صفحہ(صفحہ_سائز, ڈھونڈو_ret.pd, asid, vptr);
}

خالی flush_صفحہ(vm_صفحہ_سائز_t صفحہ_سائز, pde_t *pd, asid_t asid, لفظ_t vptr)
{
    pde_t stored_hw_asid;
    لفظ_t بنیاد_addr;
    bool_t جڑ_switched;

    أكد((vptr & نقاب(صفحہ_بٹ_For_سائز(صفحہ_سائز))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    جڑ_switched = سیٹ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = بھرو_HWASID(asid);

    اگر (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        بنیاد_addr = vptr & ~نقاب(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(بنیاد_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        اگر (جڑ_switched) {
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }
}

خالی flush_جدول(pde_t *pd, asid_t asid, لفظ_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t جڑ_switched;

    أكد((vptr & نقاب(PT_اشاریہ_بٹ + ARMSmall_صفحہ_بٹ)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    جڑ_switched = سیٹ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = بھرو_HWASID(asid);

    اگر (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        اگر (جڑ_switched) {
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }
}

خالی flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = بھرو_HWASID(asid);

    /* Clean the entire data cache, to guarantee that any VAs mapped
     * in the deleted space are clean (because we can't clean by VA after
     * deleting the space) */
    cleanCaches_PoU();

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    اگر (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        واپس;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

خالی invalidateTLBByASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = بھرو_HWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    اگر (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        واپس;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ساکن اندرونی bool_t CONST checkVPAlignment(vm_صفحہ_سائز_t sz, لفظ_t w)
{
    واپس (w & نقاب(صفحہ_بٹ_For_سائز(sz))) == 0;
}

ساخت create_mappings_pte_return {
    exception_t حالت_2;
    pte_t pte;
    pte_range_t pte_entries;
};
قسمتعریف ساخت create_mappings_pte_return create_mappings_pte_return_t;

ساخت create_mappings_pde_return {
    exception_t حالت_2;
    pde_t pde;
    pde_range_t pde_entries;
};
قسمتعریف ساخت create_mappings_pde_return create_mappings_pde_return_t;

ساکن create_mappings_pte_return_t
create_محفوظ_Mapping_Entries_PTE
(paddr_t بنیاد, لفظ_t vaddr, vm_صفحہ_سائز_t چوکھٹا_سائز,
 vm_rights_t vmRights, vm_اوصاف_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    لفظ_t i;

    انتخاب (چوکھٹا_سائز) {

    حالت ARMSmall_صفحہ:

        ret.pte_entries.بنیاد = تہی; /* to avoid uninitialised warning */
        ret.pte_entries.لمبائی = 1;

        ret.pte = makeUserPTE(ARMSmall_صفحہ, بنیاد,
                              vm_اوصاف_get_arm_صفحہ_Cacheable(attr),
                              vm_اوصاف_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ =
                false;
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            واپس ret;
        }

        ret.pte_entries.بنیاد = lu_ret.pt_خانہ;
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        اگر (unlikely(pte_ptr_get_pteType(ret.pte_entries.بنیاد) ==
                     pte_pte_large)) {
#ورنہ
        اگر (unlikely(pte_ptr_get_pteType(ret.pte_entries.بنیاد) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.بنیاد))) {
#اگر_ختم
            current_syscall_error.type =
                se_L4_Delete_پہلا;

            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
            واپس ret;
        }

        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    حالت ARMLarge_صفحہ:

        ret.pte_entries.بنیاد = تہی; /* to avoid uninitialised warning */
        ret.pte_entries.لمبائی = صفحات_فی_LARGE_صفحہ;

        ret.pte = makeUserPTE(ARMLarge_صفحہ, بنیاد,
                              vm_اوصاف_get_arm_صفحہ_Cacheable(attr),
                              vm_اوصاف_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ =
                false;
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            واپس ret;
        }

        ret.pte_entries.بنیاد = lu_ret.pt_خانہ;

        برائے (i = 0; i < صفحات_فی_LARGE_صفحہ; i++) {
            اگر (unlikely(pte_get_pteType(ret.pte_entries.بنیاد[i]) ==
                         pte_pte_small)
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.بنیاد[i])
#اگر_ختم
               ) {
                current_syscall_error.type =
                    se_L4_Delete_پہلا;

                ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
                واپس ret;
            }
        }

        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    طےشدہ:
        fail("Invalid or unexpected ARM page type.");

    }
}

ساکن create_mappings_pde_return_t
create_محفوظ_Mapping_Entries_PDE
(paddr_t بنیاد, لفظ_t vaddr, vm_صفحہ_سائز_t چوکھٹا_سائز,
 vm_rights_t vmRights, vm_اوصاف_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_لیبل_t currentPDEType;
    لفظ_t i;

    انتخاب (چوکھٹا_سائز) {

    /* PDE mappings */
    حالت ARMSection:
        ret.pde_entries.بنیاد = lookupPDSlot(pd, vaddr);
        ret.pde_entries.لمبائی = 1;

        ret.pde = makeUserPDE(ARMSection, بنیاد,
                              vm_اوصاف_get_arm_Parity_Enabled(attr),
                              vm_اوصاف_get_arm_صفحہ_Cacheable(attr),
                              vm_اوصاف_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.بنیاد);
        اگر (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_سائز(ret.pde_entries.بنیاد) != 0))) {
#ورنہ
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.بنیاد) != 0))) {
#اگر_ختم
            current_syscall_error.type =
                se_L4_Delete_پہلا;
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;

            واپس ret;
        }

        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    حالت ARMSuperSection:
        ret.pde_entries.بنیاد = lookupPDSlot(pd, vaddr);
        ret.pde_entries.لمبائی = SECTIONS_فی_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, بنیاد,
                              vm_اوصاف_get_arm_Parity_Enabled(attr),
                              vm_اوصاف_get_arm_صفحہ_Cacheable(attr),
                              vm_اوصاف_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        برائے (i = 0; i < SECTIONS_فی_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.بنیاد[i]);
            اگر (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_سائز(ret.pde_entries.بنیاد[i]) != 1))) {
#ورنہ
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.بنیاد[i]) != 1))) {
#اگر_ختم
                current_syscall_error.type =
                    se_L4_Delete_پہلا;
                ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;

                واپس ret;
            }
        }

        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    طےشدہ:
        fail("Invalid or unexpected ARM page type.");

    }
}

ساکن اندرونی vptr_t صفحہ_بنیاد(vptr_t vaddr, vm_صفحہ_سائز_t سائز)
{
    واپس vaddr & ~نقاب(صفحہ_بٹ_For_سائز(سائز));
}

ساکن bool_t خالص pteCheckIfMapped(pte_t *pte)
{
    واپس pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

ساکن bool_t خالص pdeCheckIfMapped(pde_t *pde)
{
    واپس pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

ساکن خالی doFlush(صحیح invLabel, vptr_t آغاز, vptr_t انتہا, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        انتہا = (vptr_t)paddr_مقصد_pptr(pstart) + (انتہا - آغاز);
        آغاز = (vptr_t)paddr_مقصد_pptr(pstart);
    }
    انتخاب (invLabel) {
    حالت ARMPDClean_ڈیٹا:
    حالت ARMPage_Clean_ڈیٹا:
        cleanCacheRange_RAM(آغاز, انتہا, pstart);
        توڑو;
    حالت ARMPDInvalidate_ڈیٹا:
    حالت ARMPage_Invalidate_ڈیٹا:
        invalidateCacheRange_RAM(آغاز, انتہا, pstart);
        توڑو;
    حالت ARMPDClean_Invalidate_ڈیٹا:
    حالت ARMPage_Clean_Invalidate_ڈیٹا:
        cleanInvalidateCacheRange_RAM(آغاز, انتہا, pstart);
        توڑو;
    حالت ARMPDUnify_Instruction:
    حالت ARMPageUnify_Instruction:
        /* First clean data lines to point of unification
           (L2 cache)... */
        cleanCacheRange_PoU(آغاز, انتہا, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(آغاز, انتہا, pstart);
        /* ...then invalidate branch predictors... */
        branchFlushRange(آغاز, انتہا, pstart);
        /* ... then wait for the completion of invalidations. */
        dsb();
        /* Ensure new instructions come from fresh cache lines. */
        isb();
        توڑو;
    طےشدہ:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

ساکن exception_t performPDFlush(صحیح invLabel, pde_t *pd, asid_t asid, vptr_t آغاز,
                                  vptr_t انتہا, paddr_t pstart)
{
    bool_t جڑ_switched;

    /* Flush if given a non zero range */
    اگر (آغاز < انتہا) {
        جڑ_switched = سیٹ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, آغاز, انتہا, pstart);

        اگر (جڑ_switched) {
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_جدول_Invocation_Map(cap_t cap, cte_t *ct_خانہ,
                                                 pde_t pde, pde_t *pd_خانہ)
{
    ct_خانہ->cap = cap;
    *pd_خانہ = pde;
    cleanByVA_PoU((لفظ_t)pd_خانہ, addrFromPPtr(pd_خانہ));

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_خانہ)
{
    اگر (cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحہ_جدول(
            cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap),
            pt);
        clear_حافظہ_PT((خالی *)pt, cap_get_cap_سائز_بٹ(cap));
    }
    cap_صفحہ_جدول_cap_ptr_سیٹ_cap_PTIs_Mapped(&(ct_خانہ->cap), 0);

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_Invocation_Map_PTE(asid_t asid, cap_t cap, cte_t *ct_خانہ, pte_t pte,
                                               pte_range_t pte_entries)
{
    لفظ_t i, j غیر_مستعمل;
    bool_t tlbflush_required;

    ct_خانہ->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.بنیاد);

    j = pte_entries.لمبائی;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t بنیاد_پتہ = pte_pte_small_get_پتہ(pte);
#اگر_ختم
    برائے (i = 0; i < pte_entries.لمبائی; i++) {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_سیٹ_پتہ(pte, بنیاد_پتہ + i * ثنائی_ہندسہ(صفحہ_بٹ_For_سائز(ARMSmall_صفحہ)));
#اگر_ختم
        pte_entries.بنیاد[i] = pte;
    }
    cleanCacheRange_PoU((لفظ_t)pte_entries.بنیاد,
                        آخری_BYTE_PTE(pte_entries.بنیاد, pte_entries.لمبائی),
                        addrFromPPtr(pte_entries.بنیاد));
    اگر (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_Invocation_Map_PDE(asid_t asid, cap_t cap, cte_t *ct_خانہ, pde_t pde,
                                               pde_range_t pde_entries)
{
    لفظ_t i, j غیر_مستعمل;
    bool_t tlbflush_required;

    ct_خانہ->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.بنیاد);

    j = pde_entries.لمبائی;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    لفظ_t بنیاد_پتہ = pde_pde_section_get_پتہ(pde);
#اگر_ختم
    برائے (i = 0; i < pde_entries.لمبائی; i++) {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_سیٹ_پتہ(pde, بنیاد_پتہ + i * ثنائی_ہندسہ(صفحہ_بٹ_For_سائز(ARMSection)));
#اگر_ختم
        pde_entries.بنیاد[i] = pde;
    }
    cleanCacheRange_PoU((لفظ_t)pde_entries.بنیاد,
                        آخری_BYTE_PDE(pde_entries.بنیاد, pde_entries.لمبائی),
                        addrFromPPtr(pde_entries.بنیاد));
    اگر (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_Invocation_Unmap(cap_t cap, cte_t *ct_خانہ)
{
    اگر (generic_چوکھٹا_cap_get_cap_FIs_Mapped(cap)) {
        unmap_صفحہ(generic_چوکھٹا_cap_get_cap_FSize(cap),
                  generic_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                  generic_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                  (خالی *)generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
    }

    generic_چوکھٹا_cap_ptr_سیٹ_cap_FMapped_پتہ(&ct_خانہ->cap, asidInvalid, 0);

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_Flush(صحیح invLabel, pde_t *pd, asid_t asid, vptr_t آغاز,
                                    vptr_t انتہا, paddr_t pstart)
{
    bool_t جڑ_switched;

    /* now we can flush. But only if we were given a non zero range */
    اگر (آغاز < انتہا) {
        جڑ_switched = سیٹ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, آغاز, انتہا, pstart);

        اگر (جڑ_switched) {
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_صفحہ_Get_پتہ(خالی *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    اگر (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        غیرعلامتی صحیح لمبائی = سیٹ_MR(thread, ipc_بفر, 0, capFBasePtr);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, لمبائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    واپس EXCEPTION_NONE;
}

ساکن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pd_Cap_خانہ)
{
    cap_صفحہ_directory_cap_ptr_سیٹ_cap_PDMapped_ASID(&pd_Cap_خانہ->cap, asid);
    cap_صفحہ_directory_cap_ptr_سیٹ_cap_PDIs_Mapped(&pd_Cap_خانہ->cap, 1);
    poolPtr->array[ASID_کم(asid)] =
        PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(pd_Cap_خانہ->cap));

    واپس EXCEPTION_NONE;
}

ساکن exception_t performASIDControlInvocation(خالی *چوکھٹا, cte_t *خانہ,
                                                cte_t *والد, asid_t asid_بنیاد)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سیٹ_cap_Free_اشاریہ(&(والد->cap),
                                         MAX_FREE_اشاریہ(cap_untyped_cap_get_cap_بلاک_سائز(والد->cap)));

    memzero(چوکھٹا, 1 << ARMSmall_صفحہ_بٹ);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_داخل_کرو(cap_asid_pool_cap_new(asid_بنیاد, لفظ_REF(چوکھٹا)),
              والد, خانہ);;
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنیاد & نقاب(asid_کم_بٹ)) == 0);
    armKSASIDTable[ASID_بلند(asid_بنیاد)] = (asid_pool_t *)چوکھٹا;

    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeARMPageDirectoryInvocation(لفظ_t invLabel, لفظ_t لمبائی,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    لفظ_t *بفر)
{
    انتخاب (invLabel) {
    حالت ARMPDClean_ڈیٹا:
    حالت ARMPDInvalidate_ڈیٹا:
    حالت ARMPDClean_Invalidate_ڈیٹا:
    حالت ARMPDUnify_Instruction: {
        vptr_t آغاز, انتہا;
        paddr_t pstart;
        ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        اگر (لمبائی < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        آغاز = getSyscallArg(0, بفر);
        انتہا =   getSyscallArg(1, بفر);

        /* Check sanity of arguments */
        اگر (انتہا <= آغاز) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 1;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        اگر (آغاز >= USER_چوٹی || انتہا > USER_چوٹی) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(cap_get_capType(cap) != cap_صفحہ_directory_cap ||
                     !cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(cap));
        asid = cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(cap);

        ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(asid);
        اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(ڈھونڈو_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, آغاز);

        /* Check that there's actually something there. */
        اگر (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            واپس EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        اگر (صفحہ_بنیاد(آغاز, resolve_ret.چوکھٹا_سائز) !=
            صفحہ_بنیاد(انتہا - 1, resolve_ret.چوکھٹا_سائز)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = آغاز;
            current_syscall_error.rangeErrorMax =
                صفحہ_بنیاد(آغاز, resolve_ret.چوکھٹا_سائز) +
                نقاب(صفحہ_بٹ_For_سائز(resolve_ret.چوکھٹا_سائز));
            واپس EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.چوکھٹا_بنیاد
                 + (آغاز & نقاب(صفحہ_بٹ_For_سائز(resolve_ret.چوکھٹا_سائز)));

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        اگر (pstart < phys_بنیاد() || pstart > PADDR_چوٹی || (انتہا - آغاز) > PADDR_چوٹی - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#اگر_ختم

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performPDFlush(invLabel, pd, asid, آغاز, انتہا - 1, pstart);
    }

    طےشدہ:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

}

ساکن exception_t decode_ARMPage_جدول_Invocation(لفظ_t invLabel, لفظ_t لمبائی,
                                                cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    لفظ_t vaddr, pd_اشاریہ;

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_اوصاف_t attr;
#اگر_ختم
    cap_t pdCap;
    pde_t *pd, *pd_خانہ;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    اگر (invLabel == ARMPage_جدول_Unmap) {
        اگر (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_جدول_Invocation_Unmap(cap, cte);
    }

    اگر (unlikely(invLabel != ARMPage_جدول_Map)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر);
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vm_اوصاف_From_لفظ(getSyscallArg(1, بفر));
#اگر_ختم
    pdCap = current_extra_caps.excaprefs[0]->cap;

    اگر (unlikely(cap_get_capType(pdCap) != cap_صفحہ_directory_cap ||
                 !cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(pdCap));
    asid = cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(pdCap);

    اگر (unlikely(vaddr >= USER_چوٹی)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_چوٹی);
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    {
        ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret;

        ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(asid);
        اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(ڈھونڈو_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            واپس EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_اشاریہ = vaddr >> (صفحہ_بٹ + PT_اشاریہ_بٹ);
    pd_خانہ = &pd[pd_اشاریہ];
    اگر (unlikely(pde_ptr_get_pdeType(pd_خانہ) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = se_L4_Delete_پہلا;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap)));
#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_اوصاف_get_arm_Parity_Enabled(attr),
              0 /* Domain */
          );
#ورنہ
    pde = pde_pde_coarse_new(paddr);
#اگر_ختم

    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTMapped_پتہ(cap, vaddr);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس perform_صفحہ_جدول_Invocation_Map(cap, cte, pde, pd_خانہ);
}

ساکن exception_t decodeARMFrameInvocation(لفظ_t invLabel, لفظ_t لمبائی,
                                            cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    انتخاب (invLabel) {
    حالت ARMPageMap: {
        لفظ_t vaddr, vtop, w_rights_نقاب;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_صفحہ_سائز_t چوکھٹا_سائز;
        vm_اوصاف_t attr;

        اگر (unlikely(لمبائی < 3 || current_extra_caps.excaprefs[0] == تہی)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                se_L4_Truncated_پیغام;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, بفر);
        w_rights_نقاب = getSyscallArg(1, بفر);
        attr = vm_اوصاف_From_لفظ(getSyscallArg(2, بفر));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        چوکھٹا_سائز = generic_چوکھٹا_cap_get_cap_FSize(cap);
        capVMRights = generic_چوکھٹا_cap_get_cap_FVMRights(cap);

        اگر (unlikely(cap_get_capType(pdCap) != cap_صفحہ_directory_cap ||
                     !cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            واپس EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(
                         pdCap));
        asid = cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(pdCap);

        اگر (generic_چوکھٹا_cap_get_cap_FIs_Mapped(cap)) {
            اگر (generic_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_نمبر = 1;

                واپس EXCEPTION_SYSCALL_ERROR;
            }

            اگر (generic_چوکھٹا_cap_get_cap_FMapped_پتہ(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;

                واپس EXCEPTION_SYSCALL_ERROR;
            }
        } ورنہ {
            vtop = vaddr + ثنائی_ہندسہ(صفحہ_بٹ_For_سائز(چوکھٹا_سائز)) - 1;

            اگر (unlikely(vtop >= USER_چوٹی)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_چوٹی);
                current_syscall_error.type =
                    se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;

                واپس EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            ڈھونڈو_PDFor_ASID_ret_t ڈھونڈو_ret;

            ڈھونڈو_ret = ڈھونڈو_PDFor_ASID(asid);
            اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_ماخذ =
                    false;

                واپس EXCEPTION_SYSCALL_ERROR;
            }

            اگر (unlikely(ڈھونڈو_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_نمبر = 1;

                واپس EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            نقاب_VMRights(capVMRights, rights_From_لفظ(w_rights_نقاب));

        اگر (unlikely(!checkVPAlignment(چوکھٹا_سائز, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((خالی *)
                                   generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

        cap = generic_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, asid,
                                                      vaddr);
        اگر (چوکھٹا_سائز == ARMSmall_صفحہ || چوکھٹا_سائز == ARMLarge_صفحہ) {
            create_mappings_pte_return_t map_ret;
            map_ret = create_محفوظ_Mapping_Entries_PTE(capFBasePtr, vaddr,
                                                   چوکھٹا_سائز, vmRights,
                                                   attr, pd);
            اگر (unlikely(map_ret.حالت_2 != EXCEPTION_NONE)) {
#اگر_تعریف_شدہ CONFIG_PRINTING
                اگر (current_syscall_error.type == se_L4_Delete_پہلا) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#اگر_ختم
                واپس map_ret.حالت_2;
            }

            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            واپس perform_صفحہ_Invocation_Map_PTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } ورنہ {
            create_mappings_pde_return_t map_ret;
            map_ret = create_محفوظ_Mapping_Entries_PDE(capFBasePtr, vaddr,
                                                   چوکھٹا_سائز, vmRights,
                                                   attr, pd);
            اگر (unlikely(map_ret.حالت_2 != EXCEPTION_NONE)) {
#اگر_تعریف_شدہ CONFIG_PRINTING
                اگر (current_syscall_error.type == se_L4_Delete_پہلا) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#اگر_ختم
                واپس map_ret.حالت_2;
            }

            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            واپس perform_صفحہ_Invocation_Map_PDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    حالت ARMPageUnmap: {
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
        اگر (is_IOSpace_چوکھٹا_Cap(cap)) {
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            واپس perform_صفحہ_Invocation_Unmap_IO(cap, cte);
        } ورنہ
#اگر_ختم
        {
            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            واپس perform_صفحہ_Invocation_Unmap(cap, cte);
        }
    }

#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت ARMPageMapIO: {
        واپس decodeARMIOMapInvocation(invLabel, لمبائی, cte, cap, بفر);
    }
#اگر_ختم

    حالت ARMPage_Clean_ڈیٹا:
    حالت ARMPage_Invalidate_ڈیٹا:
    حالت ARMPage_Clean_Invalidate_ڈیٹا:
    حالت ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        ڈھونڈو_PDFor_ASID_ret_t pd;
        vptr_t آغاز, انتہا;
        paddr_t pstart;
        لفظ_t صفحہ_سائز;
        لفظ_t صفحہ_بنیاد_2;

        اگر (لمبائی < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_چوکھٹا_cap_get_cap_FMapped_ASID(cap);
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap);
#ورنہ
        vaddr = generic_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
#اگر_ختم

        اگر (unlikely(!generic_چوکھٹا_cap_get_cap_FIs_Mapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        pd = ڈھونڈو_PDFor_ASID(asid);
        اگر (unlikely(pd.حالت_2 != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        آغاز = getSyscallArg(0, بفر);
        انتہا =   getSyscallArg(1, بفر);

        /* check that the range is sane */
        اگر (انتہا <= آغاز) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 1;
            واپس EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        صفحہ_سائز = 1 << صفحہ_بٹ_For_سائز(generic_چوکھٹا_cap_get_cap_FSize(cap));
        صفحہ_بنیاد_2 = addrFromPPtr((خالی *)generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

#اگر_غیر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        اگر (unlikely(!resolve_ret.valid ||
                     resolve_ret.چوکھٹا_سائز != generic_چوکھٹا_cap_get_cap_FSize(cap) ||
                     resolve_ret.چوکھٹا_بنیاد != صفحہ_بنیاد_2)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#اگر_ختم

        اگر (آغاز >= صفحہ_سائز || انتہا > صفحہ_سائز) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = صفحہ_بنیاد_2 + آغاز;
        آغاز += vaddr;
        انتہا += vaddr;

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        اگر (pstart < phys_بنیاد() || pstart > PADDR_چوٹی || (انتہا - آغاز) > PADDR_چوٹی - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
#اگر_ختم

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_Flush(invLabel, pd.pd, asid, آغاز, انتہا - 1, pstart);
    }

    حالت ARMPage_Get_پتہ: {


        /* Check that there are enough message registers */
        أكد(n_پیغام_Registers >= 1);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_Get_پتہ((خالی *)generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap), call);
    }

    طےشدہ:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        واپس EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(لفظ_t invLabel, لفظ_t لمبائی, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_صفحہ_directory_cap:
        واپس decodeARMPageDirectoryInvocation(invLabel, لمبائی, cptr, cte,
                                                cap, بفر);

    حالت cap_صفحہ_جدول_cap:
        واپس decode_ARMPage_جدول_Invocation(invLabel, لمبائی, cte, cap, بفر);

    حالت cap_small_چوکھٹا_cap:
    حالت cap_چوکھٹا_cap:
        واپس decodeARMFrameInvocation(invLabel, لمبائی, cte, cap, call, بفر);

    حالت cap_asid_control_cap: {
        لفظ_t i;
        asid_t asid_بنیاد;
        لفظ_t اشاریہ, گہرائی;
        cap_t untyped, جڑ;
        cte_t *والد_خانہ, *dest_خانہ;
        lookup_خانہ_ret_t lu_ret;
        خالی *چوکھٹا;
        exception_t حالت_2;

        اگر (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی
                     || current_extra_caps.excaprefs[1] == تہی)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اشاریہ = getSyscallArg(0, بفر);
        گہرائی = getSyscallArg(1, بفر);
        والد_خانہ = current_extra_caps.excaprefs[0];
        untyped = والد_خانہ->cap;
        جڑ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        برائے (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        اگر (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پہلا;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنیاد = i << asid_کم_بٹ;

        اگر (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_بلاک_سائز(untyped) !=
                     se_L4_ASIDPool_بٹ || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        حالت_2 = ensureNoChildren(والد_خانہ);
        اگر (unlikely(حالت_2 != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            واپس حالت_2;
        }

        چوکھٹا = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ہدف_خانہ(جڑ, اشاریہ, گہرائی);
        اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            واپس lu_ret.حالت_2;
        }
        dest_خانہ = lu_ret.خانہ;

        حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
        اگر (unlikely(حالت_2 != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            واپس حالت_2;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performASIDControlInvocation(چوکھٹا, dest_خانہ,
                                            والد_خانہ, asid_بنیاد);
    }

    حالت cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pd_Cap_خانہ;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t asid;

        اگر (unlikely(invLabel != ARMASIDPool_تفویض)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(current_extra_caps.excaprefs[0] == تہی)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        pd_Cap_خانہ = current_extra_caps.excaprefs[0];
        pdCap = pd_Cap_خانہ->cap;

        اگر (unlikely(
                cap_get_capType(pdCap) != cap_صفحہ_directory_cap ||
                cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_بلند(cap_asid_pool_cap_get_capASIDBase(cap))];
        اگر (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            current_lookup_fault = lookup_fault_invalid_جڑ_new();

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        برائے (i = 0; i < (1 << asid_کم_بٹ) && (asid + i == 0 || pool->array[i]); i++);

        اگر (unlikely(i == 1 << asid_کم_بٹ)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = se_L4_Delete_پہلا;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performASIDPoolInvocation(asid, pool, pd_Cap_خانہ);
    }

    طےشدہ:
        fail("Invalid ARM arch cap type");
    }
}

#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
exception_t benchmark_arch_map_log_بفر(لفظ_t چوکھٹا_cptr)
{
    lookup_Cap_And_خانہ_ret_t lu_ret;
    vm_صفحہ_سائز_t چوکھٹا_سائز;
    pptr_t  چوکھٹا_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_خانہ(گرہ_STATE(ksCurThread), چوکھٹا_cptr);

    اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکھٹا_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_get_capType(lu_ret.cap) != cap_چوکھٹا_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_سائز = generic_چوکھٹا_cap_get_cap_FSize(lu_ret.cap);

    اگر (چوکھٹا_سائز != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_pptr = generic_چوکھٹا_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_مقصد_paddr((خالی *) چوکھٹا_pptr);

    برائے (صحیح idx = 0; idx < ثنائی_ہندسہ(PT_اشاریہ_بٹ); idx++) {
        paddr_t physical_پتہ = ks_User_Log_بفر + (idx << se_L4_صفحہ_بٹ);

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
        invalidateTranslationSingle(KS_LOG_PPTR + (idx * ثنائی_ہندسہ(se_L4_صفحہ_بٹ)));
    }

    واپس EXCEPTION_NONE;
}
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
خالی kernelPrefetchAbort(لفظ_t pc, لفظ_t lr) VISIBLE;
خالی kernel_ڈیٹا_Abort(لفظ_t pc) VISIBLE;

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT

خالی kernelUndefinedInstruction(لفظ_t pc) VISIBLE;

خالی kernelPrefetchAbort(لفظ_t pc, لفظ_t lr)
{
    لکھو("\n\nKERNEL PREFETCH ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("HSR: 0x%"SEL4_PRIx_لفظ"\n", getHSR());
    لکھو("LR: 0x%"SEL4_PRIx_لفظ"\n", lr);
    halt();
}

خالی kernel_ڈیٹا_Abort(لفظ_t pc)
{
    لکھو("\n\nKERNEL DATA ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("HDFAR: 0x%"SEL4_PRIx_لفظ" HSR: 0x%"SEL4_PRIx_لفظ"\n",
           getHDFAR(), getHSR());
    halt();
}

خالی kernelUndefinedInstruction(لفظ_t pc)
{
    لکھو("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("HSR: 0x%"SEL4_PRIx_لفظ"\n", getHSR());
    halt();
}

#ورنہ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

خالی kernelPrefetchAbort(لفظ_t pc, لفظ_t lr)
{
    لکھو("\n\nKERNEL PREFETCH ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("IFSR: 0x%"SEL4_PRIx_لفظ"\n", getIFSR());
    لکھو("LR: 0x%"SEL4_PRIx_لفظ"\n", lr);
    halt();
}

خالی kernel_ڈیٹا_Abort(لفظ_t pc)
{
    لکھو("\n\nKERNEL DATA ABORT!\n");
    لکھو("Faulting instruction: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("FAR: 0x%"SEL4_PRIx_لفظ" DFSR: 0x%"SEL4_PRIx_لفظ"\n",
           getFAR(), getDFSR());
    halt();
}
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#اگر_ختم

#اگر_تعریف_شدہ CONFIG_PRINTING
قسمتعریف ساخت read_لفظ_From_VSpace_ret {
    exception_t حالت_2;
    لفظ_t قدر;
} read_لفظ_From_VSpace_ret_t;

ساکن read_لفظ_From_VSpace_ret_t read_لفظ_From_VSpace(pde_t *pd, لفظ_t vaddr)
{
    read_لفظ_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_خانہ;
    pde_t *pd_خانہ;
    paddr_t paddr;
    لفظ_t انحراف;
    pptr_t kernel_vaddr;
    لفظ_t *قدر;

    pd_خانہ = lookupPDSlot(pd, vaddr);
    اگر (pde_ptr_get_pdeType(pd_خانہ) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_پتہ(pd_خانہ);
        انحراف = vaddr & نقاب(ARMSection_بٹ);
    } ورنہ {
        pt_خانہ = lookupPTSlot(pd, vaddr);
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        اگر (pt_خانہ.حالت_2 == EXCEPTION_NONE && pte_ptr_get_pteType(pt_خانہ.pt_خانہ) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_پتہ(pt_خانہ.pt_خانہ);
            اگر (pte_pte_small_ptr_get_contiguous_hint(pt_خانہ.pt_خانہ)) {
                انحراف = vaddr & نقاب(ARMLarge_صفحہ_بٹ);
            } ورنہ {
                انحراف = vaddr & نقاب(ARMSmall_صفحہ_بٹ);
            }
#ورنہ
        اگر (pt_خانہ.حالت_2 == EXCEPTION_NONE && pte_ptr_get_pteType(pt_خانہ.pt_خانہ) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_پتہ(pt_خانہ.pt_خانہ);
            انحراف = vaddr & نقاب(ARMSmall_صفحہ_بٹ);
        } ورنہ اگر (pt_خانہ.حالت_2 == EXCEPTION_NONE && pte_ptr_get_pteType(pt_خانہ.pt_خانہ) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_پتہ(pt_خانہ.pt_خانہ);
            انحراف = vaddr & نقاب(ARMLarge_صفحہ_بٹ);
#اگر_ختم
        } ورنہ {
            ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
            واپس ret;
        }
    }


    kernel_vaddr = (لفظ_t)paddr_مقصد_pptr(paddr);
    قدر = (لفظ_t *)(kernel_vaddr + انحراف);
    ret.حالت_2 = EXCEPTION_NONE;
    ret.قدر = *قدر;
    واپس ret;
}

خالی Arch_user_ڈھیر_Trace(tcb_t *tptr)
{
    cap_t thread_جڑ;
    pde_t *pd;
    لفظ_t sp;
    صحیح i;

    thread_جڑ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    اگر (cap_get_capType(thread_جڑ) != cap_صفحہ_directory_cap) {
        لکھو("Invalid vspace\n");
        واپس;
    }

    pd = (pde_t *)pptr_کا_cap(thread_جڑ);

    sp = get_رجسٹر(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    اگر (!IS_ALIGNED(sp, se_L4_لفظ_سائز_بٹ)) {
        لکھو("SP not aligned\n");
        واپس;
    }

    برائے (i = 0; i < CONFIG_USER_ڈھیر_TRACE_لمبائی; i++) {
        لفظ_t پتہ = sp + (i * حجم(لفظ_t));
        read_لفظ_From_VSpace_ret_t نتیجہ;
        نتیجہ = read_لفظ_From_VSpace(pd, پتہ);
        اگر (نتیجہ.حالت_2 == EXCEPTION_NONE) {
            لکھو("0x%lx: 0x%lx\n", (طویل)پتہ, (طویل)نتیجہ.قدر);
        } ورنہ {
            لکھو("0x%lx: INVALID\n", (طویل)پتہ);
        }
    }
}
#اگر_ختم
