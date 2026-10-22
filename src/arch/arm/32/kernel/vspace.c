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

#اگر_تعریف_شده CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ضمن <benchmark/benchmark_track.h>
#پایان_اگر

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#تعریف_کن SECTIONS_در_هر_SUPER_SECTION بیت(ARMSuper_Section_بیتها - ARMSection_بیتها)
#تعریف_کن صفحات_در_هر_LARGE_صفحه بیت(ARMLarge_صفحه_بیتها - ARMSmall_صفحه_بیتها)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#تعریف_کن آخرین_BYTE_PTE(PTE,درازی_2) ((کلمه_t)&(PTE)[(درازی_2)-1] + (بیت(PTE_اندازه_بیتها)-1))
#تعریف_کن آخرین_BYTE_PDE(PDE,درازی_2) ((کلمه_t)&(PDE)[(درازی_2)-1] + (بیت(PDE_اندازه_بیتها)-1))

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#تعریف_کن MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#تعریف_کن MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#تعریف_کن ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#تعریف_کن ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ساكن فراغ مجموعه_Global_PD(فراغ);

بنية resolve_ret {
    paddr_t قاب_پایه;
    vm_صفحه_اندازه_t قاب_اندازه;
    bool_t valid;
};
تعريفنوع بنية resolve_ret resolve_ret_t;

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
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
BOOT_کد
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

#پایان_اگر

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

BOOT_کد فراغ map_kernel_قاب(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_صفات_t صفات)
{
    کلمه_t idx = (vaddr & ماسک(صفحه_بیتها_For_اندازه(ARMSection))) >> صفحه_بیتها_For_اندازه(ARMSmall_صفحه);

    أكد(vaddr >= PPTR_بالاترین); /* vaddr lies in the region the global PT covers */
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t tex;
    إذا (vm_صفات_get_arm_صفحه_Cacheable(صفات)) {
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
            vm_صفات_get_arm_صفحه_Cacheable(صفات)
        );
#پایان_اگر /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_کد فراغ map_kernel_پنجره(فراغ)
{
    paddr_t  phys;
    کلمه_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = phys_پایه();
    idx = PPTR_پایه >> صفحه_بیتها_For_اندازه(ARMSection);

    طالما (idx < بیت(PD_شاخص_بیتها) - SECTIONS_در_هر_SUPER_SECTION) {
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
        لكل (idx2 = idx; idx2 < idx + SECTIONS_در_هر_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += بیت(صفحه_بیتها_For_اندازه(ARMSuperSection));
        idx += SECTIONS_در_هر_SUPER_SECTION;
    }

    طالما (idx < (PPTR_بالاترین >> صفحه_بیتها_For_اندازه(ARMSection))) {
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
        phys += بیت(صفحه_بیتها_For_اندازه(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    أكد(phys == PADDR_بالاترین);

#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /*آدرس*/
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, بیت(se_L4_صفحه_جدول_بیتها));

    phys += بیت(صفحه_بیتها_For_اندازه(ARMSection));
    idx++;
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */

    /* map page table covering last 1M of virtual address space to page directory */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalPT), /*آدرس*/
            true,                        /* P       */
            0                            /* Domain  */
        );

    /* now start initialising the page table */
    memzero(armKSGlobalPT, 1 << se_L4_صفحه_جدول_بیتها);

    /* map vector table */
    map_kernel_قاب(
        addrFromKPPtr(arm_وکتور_جدول),
        PPTR_وکتور_جدول,
        VMKernelOnly,
        vm_صفات_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_کد فراغ map_kernel_پنجره(فراغ)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t استفادهنشده pteS2;

    /* Initialise PGD */
    لكل (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    لكل (idx = 0; idx < (USER_بالاترین - 0xC0000000) >> (PT_شاخص_بیتها + صفحه_بیتها); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = phys_پایه();
    لكل (; idx < بیت(PT_شاخص_بیتها) - 1; idx++) {
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
        phys += بیت(PT_شاخص_بیتها + صفحه_بیتها);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << se_L4_صفحه_جدول_بیتها);
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
        phys += بیت(صفحه_بیتها);
    }
    /* map vector table */
    map_kernel_قاب(
        addrFromKPPtr(arm_وکتور_جدول),
        PPTR_وکتور_جدول,
        VMKernelOnly,
        vm_صفات_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#پایان_اگر /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

ساكن BOOT_کد فراغ map_it_قاب_cap(cap_t pd_cap, cap_t قاب_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *هدف_جایگاه;
    pde_t *pd    = PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    فراغ  *قاب = (فراغ *)generic_قاب_cap_get_cap_FBase_Ptr(قاب_cap);
    vptr_t vptr  = generic_قاب_cap_get_cap_FMapped_آدرس(قاب_cap);

    أكد(generic_قاب_cap_get_cap_FMapped_ASID(قاب_cap) != 0);

    pd += (vptr >> صفحه_بیتها_For_اندازه(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_آدرس(pd));
    هدف_جایگاه = pt + ((vptr & ماسک(صفحه_بیتها_For_اندازه(ARMSection)))
                       >> صفحه_بیتها_For_اندازه(ARMSmall_صفحه));
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    *هدف_جایگاه = pte_pte_small_new(
                      addrFromPPtr(قاب),
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
    *هدف_جایگاه = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(قاب),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#پایان_اگر
}

/* Create a frame cap for the initial thread. */

ساكن BOOT_کد cap_t create_it_قاب_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    إذا (use_large)
        أرجع
            cap_قاب_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_کم(asid),                /* capFMappedASIDLow  */
                کلمه_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_بالا(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    وإلا
        أرجع
            cap_small_قاب_cap_new(
                ASID_کم(asid),                /* capFMappedASIDLow  */
                کلمه_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#اگر_تعریف_شده CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#پایان_اگر
                ASID_بالا(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

ساكن BOOT_کد فراغ map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(pt_cap);
    pde_t *هدف_جایگاه = pd + (vptr >> صفحه_بیتها_For_اندازه(ARMSection));

    أكد(cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(pt_cap));

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    *هدف_جایگاه = pde_pde_coarse_new(
                      addrFromPPtr(pt), /*آدرس*/
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#وإلا
    *هدف_جایگاه = pde_pde_coarse_new(addrFromPPtr(pt));
#پایان_اگر
}

/* Create a page table for the initial thread */

ساكن BOOT_کد cap_t create_it_صفحه_جدول_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحه_جدول_cap_new(
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

BOOT_کد کلمه_t arch_get_n_paging(v_region_t it_v_reg)
{
    أرجع get_n_paging(it_v_reg, PT_شاخص_بیتها + صفحه_بیتها);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_کد cap_t create_it_آدرس_space(cap_t ریشه_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    se_L4_جایگاه_Pos جایگاه_pos_before;
    se_L4_جایگاه_Pos جایگاه_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << se_L4_صفحه_Dir_بیتها) - 1,
                        addrFromPPtr((فراغ *)rootserver.vspace));
    cap_t pd_cap =
        cap_صفحه_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    جایگاه_pos_before = ndks_boot.جایگاه_pos_cur;
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    لكل (pt_vptr = دور_پایین(it_v_reg.شروع, PT_شاخص_بیتها + صفحه_بیتها);
         pt_vptr < it_v_reg.انتها;
         pt_vptr += بیت(PT_شاخص_بیتها + صفحه_بیتها)) {
        إذا (!provide_cap(ریشه_cnode_cap,
                         create_it_صفحه_جدول_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    جایگاه_pos_after = ndks_boot.جایگاه_pos_cur;
    ndks_boot.bi_قاب->userImagePaging = (se_L4_جایگاه_Region) {
        جایگاه_pos_before, جایگاه_pos_after
    };

    أرجع pd_cap;
}

BOOT_کد cap_t create_unmapped_it_قاب_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_قاب_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_کد cap_t create_mapped_it_قاب_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_قاب_cap(pptr, vptr, asid, use_large);
    map_it_قاب_cap(pd_cap, cap, executable);
    أرجع cap;
}

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_کد فراغ activate_kernel_vspace(فراغ)
{
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    مجموعه_Global_PD();
    invalidateLocalTLB();
    قفل_TLBEntry(PPTR_پایه);
    قفل_TLBEntry(PPTR_وکتور_جدول);
}

#وإلا

BOOT_کد فراغ activate_kernel_vspace(فراغ)
{
    uint32_t r;
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    /* Setup the memory attributes: We use 2 indices (cachable/non-cacheable) */
    مجموعه_HMAIR((ATTRINDX_NONCACHEABLE << 0) | (ATTRINDX_CACHEABLE << 8), 0);
    مجموعه_Current_Hyp_PD(addrFromKPPtr(armHSGlobalPGD));
    invalidateHypTLB();
#إذا 0 /* Can't lock entries on A15 */
    قفل_TLBEntry(PPTR_پایه);
    قفل_TLBEntry(PPTR_وکتور_جدول);
#پایان_اگر
    /* TODO find a better place to init the VMMU */
    r = 0;
    /* Translation range */
    r |= (0x0 << 0);     /* 2^(32 -(0)) input range. */
    r |= (r & 0x8) << 1; /* Sign bit */
    /* starting level */
    r |= (0x0 << 6);     /* Start at second level */
    /* Sharability of tables */
    r |= بیت(8);       /* Inner write-back, write-allocate */
    r |= بیت(10);      /* Outer write-back, write-allocate */
    /* Long descriptor format (not that we have a choice) */
    r |= بیت(31);
    مجموعه_VTCR(r);
}

#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_کد فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_از_cap(it_ap_cap));
    ap->array[ASID_کم(IT_ASID)] = PDE_PTR(pptr_از_cap(it_pd_cap));
    armKSASIDTable[ASID_بالا(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

پیدا_کن_PDFor_ASID_ret_t پیدا_کن_PDFor_ASID(asid_t asid)
{
    پیدا_کن_PDFor_ASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_بالا(asid)];
    إذا (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_ریشه_new();

        ret.pd = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    pd = poolPtr->array[ASID_کم(asid)];
    إذا (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_ریشه_new();

        ret.pd = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.pd = pd;
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

کلمه_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    کلمه_t w_میانگیر_Ptr;
    cap_t میانگیر_Cap;
    vm_rights_t vm_rights;

    w_میانگیر_Ptr = thread->tcbIPCBuffer;
    میانگیر_Cap = TCB_PTR_CTE_PTR(thread, tcb_میانگیر)->cap;

    إذا (unlikely(cap_get_capType(میانگیر_Cap) != cap_small_قاب_cap &&
                 cap_get_capType(میانگیر_Cap) != cap_قاب_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(generic_قاب_cap_get_cap_FIs_Device(میانگیر_Cap))) {
        أرجع عدم;
    }

    vm_rights = generic_قاب_cap_get_cap_FVMRights(میانگیر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        کلمه_t پایه_Ptr;
        بیعلامت صحيح صفحه_بیتها_2;

        پایه_Ptr = generic_قاب_cap_get_cap_FBase_Ptr(میانگیر_Cap);
        صفحه_بیتها_2 = صفحه_بیتها_For_اندازه(generic_قاب_cap_get_cap_FSize(میانگیر_Cap));
        أرجع (کلمه_t *)(پایه_Ptr + (w_میانگیر_Ptr & ماسک(صفحه_بیتها_2)));
    } وإلا {
        أرجع عدم;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (unlikely(cap_get_capType(cap) != cap_small_قاب_cap &&
                 cap_get_capType(cap) != cap_قاب_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(generic_قاب_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(vptr & ماسک(se_L4_IPCBuffer_اندازه_بیتها))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    بیعلامت صحيح pd_شاخص;

    pd_شاخص = vptr >> (صفحه_بیتها + PT_شاخص_بیتها);
    أرجع pd + pd_شاخص;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pd_جایگاه;

    pd_جایگاه = lookupPDSlot(pd, vptr);

    إذا (unlikely(pde_ptr_get_pdeType(pd_جایگاه) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_شاخص_بیتها + صفحه_بیتها);

        ret.pt_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pte_t *pt, *pt_جایگاه;
        بیعلامت صحيح pt_شاخص;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_آدرس(pd_جایگاه));
        pt_شاخص = (vptr >> صفحه_بیتها) & ماسک(PT_شاخص_بیتها);
        pt_جایگاه = pt + pt_شاخص;

        ret.pt_جایگاه = pt_جایگاه;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
    }
}

ساكن pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    بیعلامت صحيح pt_شاخص;

    pt_شاخص = (vptr >> صفحه_بیتها) & ماسک(PT_شاخص_بیتها);
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
        ret.قاب_پایه = pde_pde_section_ptr_get_آدرس(pde);
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (pde_pde_section_ptr_get_اندازه(pde)) {
            ret.قاب_اندازه = ARMSuperSection;
        } وإلا {
            ret.قاب_اندازه = ARMSection;
        }
#وإلا
        إذا (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.قاب_پایه &= ~ماسک(صفحه_بیتها_For_اندازه(ARMSuperSection));
            ret.قاب_اندازه = ARMSuperSection;
        } وإلا {
            ret.قاب_اندازه = ARMSection;
        }
#پایان_اگر
        أرجع ret;

    حالة pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_آدرس(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        بدل (pte_ptr_get_pteType(pte)) {
        حالة pte_pte_small:
            ret.قاب_پایه = pte_pte_small_ptr_get_آدرس(pte);
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
            إذا (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.قاب_پایه &= ~ماسک(صفحه_بیتها_For_اندازه(ARMLarge_صفحه));
                ret.قاب_اندازه = ARMLarge_صفحه;
            } وإلا {
                ret.قاب_اندازه = ARMSmall_صفحه;
            }
#وإلا
            ret.قاب_اندازه = ARMSmall_صفحه;
#پایان_اگر
            أرجع ret;

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        حالة pte_pte_large:
            ret.قاب_پایه = pte_pte_large_ptr_get_آدرس(pte);
            ret.قاب_اندازه = ARMLarge_صفحه;
            أرجع ret;
#پایان_اگر
        افتراضي:
            اكسر;
        }
        اكسر;
    }
    }

    ret.valid = false;
    أرجع ret;
}

ساكن pte_t CONST makeUserPTE(vm_صفحه_اندازه_t صفحه_اندازه, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t ap;

    ap = APFromVMRights(vm_rights);

    بدل (صفحه_اندازه) {
    حالة ARMSmall_صفحه: {
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

    حالة ARMLarge_صفحه: {
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

    بدل (صفحه_اندازه) {
    حالة ARMSmall_صفحه: {
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

    حالة ARMLarge_صفحه: {
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
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    أرجع pte;
}

ساكن pde_t CONST makeUserPDE(vm_صفحه_اندازه_t صفحه_اندازه, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, کلمه_t domain,
                               vm_rights_t vm_rights)
{
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t ap, size2;

    ap = APFromVMRights(vm_rights);
#وإلا
    کلمه_t hap, size2;

    (فراغ)domain;
    hap = HAPFromVMRights(vm_rights);
#پایان_اگر

    بدل (صفحه_اندازه) {
    حالة ARMSection:
        size2 = 0;
        اكسر;

    حالة ARMSuperSection:
        size2 = 1;
        اكسر;

    افتراضي:
        fail("Invalid PDE frame type");
    }

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
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
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_ریشه(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_صفحه_directory_cap &&
           cap_صفحه_directory_cap_get_cap_PDIs_Mapped(cap);
}

bool_t CONST is_IOSpace_قاب_Cap(cap_t cap)
{
#اگر_تعریف_شده CONFIG_TK1_SMMU
    أرجع cap_get_capType(cap) == cap_small_قاب_cap && cap_small_قاب_cap_get_cap_FIs_IOSpace(cap);
#وإلا
    أرجع false;
#پایان_اگر
}

/** Switch to the empty PD on the reserved HW ASID */
ساكن فراغ مجموعه_Global_PD(فراغ)
{
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
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
    مجموعه_Hardware_ASID(hwASIDReserved);
#پایان_اگر
}

فراغ مجموعه_VMRoot(tcb_t *tcb)
{
    cap_t thread_ریشه;
    asid_t asid;
    pde_t *pd;
    پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret;

    thread_ریشه = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_ریشه) != cap_صفحه_directory_cap ||
        !cap_صفحه_directory_cap_get_cap_PDIs_Mapped(thread_ریشه)) {
        مجموعه_Global_PD();
        أرجع;
    }

    pd = PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(thread_ریشه));
    asid = cap_صفحه_directory_cap_get_cap_PDMapped_ASID(thread_ریشه);
    پیدا_کن_ret = پیدا_کن_PDFor_ASID(asid);
    إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE || پیدا_کن_ret.pd != pd)) {
        مجموعه_Global_PD();
        أرجع;
    }

    armv_contextSwitch(pd, asid);
}

ساكن bool_t مجموعه_VMRoot_For_Flush(pde_t *pd, asid_t asid)
{
    cap_t thread_ریشه;

    thread_ریشه = TCB_PTR_CTE_PTR(گره_STATE(ksCurThread), tcbVTable)->cap;

    إذا (cap_get_capType(thread_ریشه) == cap_صفحه_directory_cap &&
        cap_صفحه_directory_cap_get_cap_PDIs_Mapped(thread_ریشه) &&
        PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(thread_ریشه)) == pd) {
        أرجع false;
    }

    armv_contextSwitch(pd, asid);

    أرجع true;
}

pde_t *صفحه_جدول_Mapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret;
    pde_t pde;
    بیعلامت صحيح pd_شاخص;

    پیدا_کن_ret = پیدا_کن_PDFor_ASID(asid);
    إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
        أرجع عدم;
    }

    pd_شاخص = vaddr >> (صفحه_بیتها + PT_شاخص_بیتها);
    pde = پیدا_کن_ret.pd[pd_شاخص];

    إذا (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_آدرس(pde)) == pt)) {
        أرجع پیدا_کن_ret.pd;
    } وإلا {
        أرجع عدم;
    }
}

ساكن فراغ invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بالا(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_کم(asid)];
    أكد(pd);

    pd[PD_ASID_جایگاه] = pde_pde_invalid_new(0, false);
}

ساكن pde_t خالص بارگیری_HWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بالا(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_کم(asid)];
    أكد(pd);

    أرجع pd[PD_ASID_جایگاه];
}

ساكن فراغ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_بالا(asid)];
    أكد(asidPool);

    pd = asidPool->array[ASID_کم(asid)];
    أكد(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_جایگاه] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t پیدا_کن_Free_HWASID(فراغ)
{
    کلمه_t hw_asid_جابه_جایی;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    لكل (hw_asid_جابه_جایی = 0;
         hw_asid_جابه_جایی <= (کلمه_t)((hw_asid_t) - 1);
         hw_asid_جابه_جایی ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_جابه_جایی);
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

    stored_hw_asid = بارگیری_HWASID(asid);
    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } وإلا {
        hw_asid_t new_hw_asid;

        new_hw_asid = پیدا_کن_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        أرجع new_hw_asid;
    }
}

ساكن فراغ invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = بارگیری_HWASID(asid);
    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

فراغ unmap_صفحه_جدول(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pd_جایگاه;
    بیعلامت صحيح pd_شاخص;

    pd = صفحه_جدول_Mapped(asid, vaddr, pt);

    إذا (likely(pd != عدم)) {
        pd_شاخص = vaddr >> (PT_شاخص_بیتها + صفحه_بیتها);
        pd_جایگاه = pd + pd_شاخص;

        *pd_جایگاه = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((کلمه_t)pd_جایگاه, addrFromPPtr(pd_جایگاه));
        flush_جدول(pd, asid, vaddr, pt);
    }
}

فراغ copyGlobalMappings(pde_t *newPD)
{
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t i;
    pde_t *global_pd = armKSGlobalPD;

    لكل (i = PPTR_پایه >> ARMSection_بیتها; i < بیت(PD_شاخص_بیتها); i++) {
        إذا (i != PD_ASID_جایگاه) {
            newPD[i] = global_pd[i];
        }
    }
#پایان_اگر
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    بدل (vm_faultType) {
    حالة ARMDataAbort: {
        کلمه_t addr, fault;

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (آدرس_Translate_S1(addr) & ~ماسک(صفحه_بیتها)) | (addr & ماسک(صفحه_بیتها));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#وإلا
        addr = getFAR();
        fault = getDFSR();
#پایان_اگر

#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
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
#پایان_اگر
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        أرجع EXCEPTION_FAULT;
    }

    حالة ARMPrefetchAbort: {
        کلمه_t pc, fault;

        pc = getRestartPC(thread);

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (آدرس_Translate_S1(pc) & ~ماسک(صفحه_بیتها)) | (pc & ماسک(صفحه_بیتها));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#وإلا
        fault = getIFSR();
#پایان_اگر

#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
        إذا (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            إذا (seL4_Fault_DebugException_get_exceptionReason(current_fault) == se_L4_Single_گام
                && !single_گام_Fault_Counter_Ready(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                أرجع EXCEPTION_NONE;
            }
            أرجع EXCEPTION_FAULT;
        }
#پایان_اگر
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        أرجع EXCEPTION_FAULT;
    }

    افتراضي:
        fail("Invalid VM fault type");
    }
}

فراغ deleteASIDPool(asid_t asid_پایه, asid_pool_t *pool)
{
    بیعلامت صحيح جابه_جایی;

    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_پایه & ماسک(asid_کم_بیتها)) == 0);

    إذا (armKSASIDTable[ASID_بالا(asid_پایه)] == pool) {
        لكل (جابه_جایی = 0; جابه_جایی < بیت(asid_کم_بیتها); جابه_جایی++) {
            إذا (pool->array[جابه_جایی]) {
                flushSpace(asid_پایه + جابه_جایی);
                invalidateASIDEntry(asid_پایه + جابه_جایی);
            }
        }
        armKSASIDTable[ASID_بالا(asid_پایه)] = عدم;
        مجموعه_VMRoot(گره_STATE(ksCurThread));
    }
}

فراغ deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_بالا(asid)];

    إذا (poolPtr != عدم && poolPtr->array[ASID_کم(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_کم(asid)] = عدم;
        مجموعه_VMRoot(گره_STATE(ksCurThread));
    }
}

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
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
#پایان_اگر

فراغ unmap_صفحه(vm_صفحه_اندازه_t صفحه_اندازه, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret;
    paddr_t addr = addrFromPPtr(pptr);

    پیدا_کن_ret = پیدا_کن_PDFor_ASID(asid);
    إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
        أرجع;
    }

    بدل (صفحه_اندازه) {
    حالة ARMSmall_صفحه: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(پیدا_کن_ret.pd, vptr);
        إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
            أرجع;
        }

        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_جایگاه) != pte_pte_small)) {
            أرجع;
        }
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_جایگاه) != 0)) {
            أرجع;
        }
#پایان_اگر
        إذا (unlikely(pte_pte_small_ptr_get_آدرس(lu_ret.pt_جایگاه) != addr)) {
            أرجع;
        }

        *(lu_ret.pt_جایگاه) = pte_pte_invalid_new();
        cleanByVA_PoU((کلمه_t)lu_ret.pt_جایگاه, addrFromPPtr(lu_ret.pt_جایگاه));

        اكسر;
    }

    حالة ARMLarge_صفحه: {
        lookupPTSlot_ret_t lu_ret;
        کلمه_t i;

        lu_ret = lookupPTSlot(پیدا_کن_ret.pd, vptr);
        إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
            أرجع;
        }
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_جایگاه) != pte_pte_large)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_large_ptr_get_آدرس(lu_ret.pt_جایگاه) != addr)) {
            أرجع;
        }
#وإلا
        إذا (unlikely(pte_ptr_get_pteType(lu_ret.pt_جایگاه) != pte_pte_small)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_جایگاه) != 1)) {
            أرجع;
        }
        إذا (unlikely(pte_pte_small_ptr_get_آدرس(lu_ret.pt_جایگاه) != addr)) {
            أرجع;
        }
#پایان_اگر

        لكل (i = 0; i < صفحات_در_هر_LARGE_صفحه; i++) {
            lu_ret.pt_جایگاه[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((کلمه_t)&lu_ret.pt_جایگاه[0],
                            آخرین_BYTE_PTE(lu_ret.pt_جایگاه, صفحات_در_هر_LARGE_صفحه),
                            addrFromPPtr(&lu_ret.pt_جایگاه[0]));

        اكسر;
    }

    حالة ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(پیدا_کن_ret.pd, vptr);

        إذا (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            أرجع;
        }
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pde_pde_section_ptr_get_اندازه(pd) != 0)) {
#وإلا
        إذا (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#پایان_اگر
            أرجع;
        }
        إذا (unlikely(pde_pde_section_ptr_get_آدرس(pd) != addr)) {
            أرجع;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((کلمه_t)pd, addrFromPPtr(pd));

        اكسر;
    }

    حالة ARMSuperSection: {
        pde_t *pd;
        کلمه_t i;

        pd = lookupPDSlot(پیدا_کن_ret.pd, vptr);

        إذا (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            أرجع;
        }
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pde_pde_section_ptr_get_اندازه(pd) != 1)) {
#وإلا
        إذا (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#پایان_اگر
            أرجع;
        }
        إذا (unlikely(pde_pde_section_ptr_get_آدرس(pd) != addr)) {
            أرجع;
        }

        لكل (i = 0; i < SECTIONS_در_هر_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((کلمه_t)&pd[0], آخرین_BYTE_PDE(pd, SECTIONS_در_هر_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        اكسر;
    }

    افتراضي:
        fail("Invalid ARM page type");
        اكسر;
    }

    /* Flush the page now that the mapping has been updated */
    flush_صفحه(صفحه_اندازه, پیدا_کن_ret.pd, asid, vptr);
}

فراغ flush_صفحه(vm_صفحه_اندازه_t صفحه_اندازه, pde_t *pd, asid_t asid, کلمه_t vptr)
{
    pde_t stored_hw_asid;
    کلمه_t پایه_addr;
    bool_t ریشه_switched;

    أكد((vptr & ماسک(صفحه_بیتها_For_اندازه(صفحه_اندازه))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ریشه_switched = مجموعه_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = بارگیری_HWASID(asid);

    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        پایه_addr = vptr & ~ماسک(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(پایه_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        إذا (ریشه_switched) {
            مجموعه_VMRoot(گره_STATE(ksCurThread));
        }
    }
}

فراغ flush_جدول(pde_t *pd, asid_t asid, کلمه_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t ریشه_switched;

    أكد((vptr & ماسک(PT_شاخص_بیتها + ARMSmall_صفحه_بیتها)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ریشه_switched = مجموعه_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = بارگیری_HWASID(asid);

    إذا (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        إذا (ریشه_switched) {
            مجموعه_VMRoot(گره_STATE(ksCurThread));
        }
    }
}

فراغ flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = بارگیری_HWASID(asid);

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

    stored_hw_asid = بارگیری_HWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    إذا (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        أرجع;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ساكن مضمن bool_t CONST checkVPAlignment(vm_صفحه_اندازه_t sz, کلمه_t w)
{
    أرجع (w & ماسک(صفحه_بیتها_For_اندازه(sz))) == 0;
}

بنية create_mappings_pte_return {
    exception_t وضعیت;
    pte_t pte;
    pte_range_t pte_entries;
};
تعريفنوع بنية create_mappings_pte_return create_mappings_pte_return_t;

بنية create_mappings_pde_return {
    exception_t وضعیت;
    pde_t pde;
    pde_range_t pde_entries;
};
تعريفنوع بنية create_mappings_pde_return create_mappings_pde_return_t;

ساكن create_mappings_pte_return_t
create_امن_Mapping_Entries_PTE
(paddr_t پایه, کلمه_t vaddr, vm_صفحه_اندازه_t قاب_اندازه,
 vm_rights_t vmRights, vm_صفات_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    کلمه_t i;

    بدل (قاب_اندازه) {

    حالة ARMSmall_صفحه:

        ret.pte_entries.پایه = عدم; /* to avoid uninitialised warning */
        ret.pte_entries.درازی = 1;

        ret.pte = makeUserPTE(ARMSmall_صفحه, پایه,
                              vm_صفات_get_arm_صفحه_Cacheable(attr),
                              vm_صفات_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع =
                false;
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            أرجع ret;
        }

        ret.pte_entries.پایه = lu_ret.pt_جایگاه;
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (unlikely(pte_ptr_get_pteType(ret.pte_entries.پایه) ==
                     pte_pte_large)) {
#وإلا
        إذا (unlikely(pte_ptr_get_pteType(ret.pte_entries.پایه) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.پایه))) {
#پایان_اگر
            current_syscall_error.type =
                se_L4_Delete_اولی;

            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
            أرجع ret;
        }

        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    حالة ARMLarge_صفحه:

        ret.pte_entries.پایه = عدم; /* to avoid uninitialised warning */
        ret.pte_entries.درازی = صفحات_در_هر_LARGE_صفحه;

        ret.pte = makeUserPTE(ARMLarge_صفحه, پایه,
                              vm_صفات_get_arm_صفحه_Cacheable(attr),
                              vm_صفات_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع =
                false;
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            أرجع ret;
        }

        ret.pte_entries.پایه = lu_ret.pt_جایگاه;

        لكل (i = 0; i < صفحات_در_هر_LARGE_صفحه; i++) {
            إذا (unlikely(pte_get_pteType(ret.pte_entries.پایه[i]) ==
                         pte_pte_small)
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.پایه[i])
#پایان_اگر
               ) {
                current_syscall_error.type =
                    se_L4_Delete_اولی;

                ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
                أرجع ret;
            }
        }

        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        fail("Invalid or unexpected ARM page type.");

    }
}

ساكن create_mappings_pde_return_t
create_امن_Mapping_Entries_PDE
(paddr_t پایه, کلمه_t vaddr, vm_صفحه_اندازه_t قاب_اندازه,
 vm_rights_t vmRights, vm_صفات_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_برچسب_t currentPDEType;
    کلمه_t i;

    بدل (قاب_اندازه) {

    /* PDE mappings */
    حالة ARMSection:
        ret.pde_entries.پایه = lookupPDSlot(pd, vaddr);
        ret.pde_entries.درازی = 1;

        ret.pde = makeUserPDE(ARMSection, پایه,
                              vm_صفات_get_arm_Parity_Enabled(attr),
                              vm_صفات_get_arm_صفحه_Cacheable(attr),
                              vm_صفات_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.پایه);
        إذا (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_اندازه(ret.pde_entries.پایه) != 0))) {
#وإلا
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.پایه) != 0))) {
#پایان_اگر
            current_syscall_error.type =
                se_L4_Delete_اولی;
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;

            أرجع ret;
        }

        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    حالة ARMSuperSection:
        ret.pde_entries.پایه = lookupPDSlot(pd, vaddr);
        ret.pde_entries.درازی = SECTIONS_در_هر_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, پایه,
                              vm_صفات_get_arm_Parity_Enabled(attr),
                              vm_صفات_get_arm_صفحه_Cacheable(attr),
                              vm_صفات_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        لكل (i = 0; i < SECTIONS_در_هر_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.پایه[i]);
            إذا (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_اندازه(ret.pde_entries.پایه[i]) != 1))) {
#وإلا
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.پایه[i]) != 1))) {
#پایان_اگر
                current_syscall_error.type =
                    se_L4_Delete_اولی;
                ret.وضعیت = EXCEPTION_SYSCALL_ERROR;

                أرجع ret;
            }
        }

        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        fail("Invalid or unexpected ARM page type.");

    }
}

ساكن مضمن vptr_t صفحه_پایه(vptr_t vaddr, vm_صفحه_اندازه_t اندازه)
{
    أرجع vaddr & ~ماسک(صفحه_بیتها_For_اندازه(اندازه));
}

ساكن bool_t خالص pteCheckIfMapped(pte_t *pte)
{
    أرجع pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

ساكن bool_t خالص pdeCheckIfMapped(pde_t *pde)
{
    أرجع pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

ساكن فراغ doFlush(صحيح invLabel, vptr_t شروع, vptr_t انتها, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        انتها = (vptr_t)paddr_مقصد_pptr(pstart) + (انتها - شروع);
        شروع = (vptr_t)paddr_مقصد_pptr(pstart);
    }
    بدل (invLabel) {
    حالة ARMPDClean_داده:
    حالة ARMPage_Clean_داده:
        cleanCacheRange_RAM(شروع, انتها, pstart);
        اكسر;
    حالة ARMPDInvalidate_داده:
    حالة ARMPage_Invalidate_داده:
        invalidateCacheRange_RAM(شروع, انتها, pstart);
        اكسر;
    حالة ARMPDClean_Invalidate_داده:
    حالة ARMPage_Clean_Invalidate_داده:
        cleanInvalidateCacheRange_RAM(شروع, انتها, pstart);
        اكسر;
    حالة ARMPDUnify_Instruction:
    حالة ARMPageUnify_Instruction:
        /* First clean data lines to point of unification
           (L2 cache)... */
        cleanCacheRange_PoU(شروع, انتها, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(شروع, انتها, pstart);
        /* ...then invalidate branch predictors... */
        branchFlushRange(شروع, انتها, pstart);
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
                                  vptr_t انتها, paddr_t pstart)
{
    bool_t ریشه_switched;

    /* Flush if given a non zero range */
    إذا (شروع < انتها) {
        ریشه_switched = مجموعه_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, شروع, انتها, pstart);

        إذا (ریشه_switched) {
            مجموعه_VMRoot(گره_STATE(ksCurThread));
        }
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_جدول_Invocation_Map(cap_t cap, cte_t *ct_جایگاه,
                                                 pde_t pde, pde_t *pd_جایگاه)
{
    ct_جایگاه->cap = cap;
    *pd_جایگاه = pde;
    cleanByVA_PoU((کلمه_t)pd_جایگاه, addrFromPPtr(pd_جایگاه));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_جایگاه)
{
    إذا (cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحه_جدول(
            cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap),
            pt);
        clear_حافظه_PT((فراغ *)pt, cap_get_cap_اندازه_بیتها(cap));
    }
    cap_صفحه_جدول_cap_ptr_مجموعه_cap_PTIs_Mapped(&(ct_جایگاه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_Invocation_Map_PTE(asid_t asid, cap_t cap, cte_t *ct_جایگاه, pte_t pte,
                                               pte_range_t pte_entries)
{
    کلمه_t i, j استفادهنشده;
    bool_t tlbflush_required;

    ct_جایگاه->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.پایه);

    j = pte_entries.درازی;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t پایه_آدرس = pte_pte_small_get_آدرس(pte);
#پایان_اگر
    لكل (i = 0; i < pte_entries.درازی; i++) {
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_مجموعه_آدرس(pte, پایه_آدرس + i * بیت(صفحه_بیتها_For_اندازه(ARMSmall_صفحه)));
#پایان_اگر
        pte_entries.پایه[i] = pte;
    }
    cleanCacheRange_PoU((کلمه_t)pte_entries.پایه,
                        آخرین_BYTE_PTE(pte_entries.پایه, pte_entries.درازی),
                        addrFromPPtr(pte_entries.پایه));
    إذا (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_Invocation_Map_PDE(asid_t asid, cap_t cap, cte_t *ct_جایگاه, pde_t pde,
                                               pde_range_t pde_entries)
{
    کلمه_t i, j استفادهنشده;
    bool_t tlbflush_required;

    ct_جایگاه->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.پایه);

    j = pde_entries.درازی;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    کلمه_t پایه_آدرس = pde_pde_section_get_آدرس(pde);
#پایان_اگر
    لكل (i = 0; i < pde_entries.درازی; i++) {
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_مجموعه_آدرس(pde, پایه_آدرس + i * بیت(صفحه_بیتها_For_اندازه(ARMSection)));
#پایان_اگر
        pde_entries.پایه[i] = pde;
    }
    cleanCacheRange_PoU((کلمه_t)pde_entries.پایه,
                        آخرین_BYTE_PDE(pde_entries.پایه, pde_entries.درازی),
                        addrFromPPtr(pde_entries.پایه));
    إذا (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_Invocation_Unmap(cap_t cap, cte_t *ct_جایگاه)
{
    إذا (generic_قاب_cap_get_cap_FIs_Mapped(cap)) {
        unmap_صفحه(generic_قاب_cap_get_cap_FSize(cap),
                  generic_قاب_cap_get_cap_FMapped_ASID(cap),
                  generic_قاب_cap_get_cap_FMapped_آدرس(cap),
                  (فراغ *)generic_قاب_cap_get_cap_FBase_Ptr(cap));
    }

    generic_قاب_cap_ptr_مجموعه_cap_FMapped_آدرس(&ct_جایگاه->cap, asidInvalid, 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_Flush(صحيح invLabel, pde_t *pd, asid_t asid, vptr_t شروع,
                                    vptr_t انتها, paddr_t pstart)
{
    bool_t ریشه_switched;

    /* now we can flush. But only if we were given a non zero range */
    إذا (شروع < انتها) {
        ریشه_switched = مجموعه_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, شروع, انتها, pstart);

        إذا (ریشه_switched) {
            مجموعه_VMRoot(گره_STATE(ksCurThread));
        }
    }

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_صفحه_Get_آدرس(فراغ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    إذا (call) {
        کلمه_t *ipc_میانگیر = lookupIPCBuffer(true, thread);
        مجموعه_ثبات(thread, badge_ثبات, 0);
        بیعلامت صحيح درازی = مجموعه_MR(thread, ipc_میانگیر, 0, capFBasePtr);
        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, درازی)));
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pd_Cap_جایگاه)
{
    cap_صفحه_directory_cap_ptr_مجموعه_cap_PDMapped_ASID(&pd_Cap_جایگاه->cap, asid);
    cap_صفحه_directory_cap_ptr_مجموعه_cap_PDIs_Mapped(&pd_Cap_جایگاه->cap, 1);
    poolPtr->array[ASID_کم(asid)] =
        PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(pd_Cap_جایگاه->cap));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDControlInvocation(فراغ *قاب, cte_t *جایگاه,
                                                cte_t *والد, asid_t asid_پایه)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_مجموعه_cap_Free_شاخص(&(والد->cap),
                                         MAX_FREE_شاخص(cap_untyped_cap_get_cap_بلوک_اندازه(والد->cap)));

    memzero(قاب, 1 << ARMSmall_صفحه_بیتها);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_درج(cap_asid_pool_cap_new(asid_پایه, کلمه_REF(قاب)),
              والد, جایگاه);;
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_پایه & ماسک(asid_کم_بیتها)) == 0);
    armKSASIDTable[ASID_بالا(asid_پایه)] = (asid_pool_t *)قاب;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeARMPageDirectoryInvocation(کلمه_t invLabel, کلمه_t درازی,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    کلمه_t *میانگیر)
{
    بدل (invLabel) {
    حالة ARMPDClean_داده:
    حالة ARMPDInvalidate_داده:
    حالة ARMPDClean_Invalidate_داده:
    حالة ARMPDUnify_Instruction: {
        vptr_t شروع, انتها;
        paddr_t pstart;
        پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        إذا (درازی < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروع = getSyscallArg(0, میانگیر);
        انتها =   getSyscallArg(1, میانگیر);

        /* Check sanity of arguments */
        إذا (انتها <= شروع) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        إذا (شروع >= USER_بالاترین || انتها > USER_بالاترین) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(cap_get_capType(cap) != cap_صفحه_directory_cap ||
                     !cap_صفحه_directory_cap_get_cap_PDIs_Mapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(cap));
        asid = cap_صفحه_directory_cap_get_cap_PDMapped_ASID(cap);

        پیدا_کن_ret = پیدا_کن_PDFor_ASID(asid);
        إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(پیدا_کن_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, شروع);

        /* Check that there's actually something there. */
        إذا (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
            أرجع EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        إذا (صفحه_پایه(شروع, resolve_ret.قاب_اندازه) !=
            صفحه_پایه(انتها - 1, resolve_ret.قاب_اندازه)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = شروع;
            current_syscall_error.rangeErrorMax =
                صفحه_پایه(شروع, resolve_ret.قاب_اندازه) +
                ماسک(صفحه_بیتها_For_اندازه(resolve_ret.قاب_اندازه));
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.قاب_پایه
                 + (شروع & ماسک(صفحه_بیتها_For_اندازه(resolve_ret.قاب_اندازه)));

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        إذا (pstart < phys_پایه() || pstart > PADDR_بالاترین || (انتها - شروع) > PADDR_بالاترین - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#پایان_اگر

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performPDFlush(invLabel, pd, asid, شروع, انتها - 1, pstart);
    }

    افتراضي:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

}

ساكن exception_t decode_ARMPage_جدول_Invocation(کلمه_t invLabel, کلمه_t درازی,
                                                cte_t *cte, cap_t cap, کلمه_t *میانگیر)
{
    کلمه_t vaddr, pd_شاخص;

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_صفات_t attr;
#پایان_اگر
    cap_t pdCap;
    pde_t *pd, *pd_جایگاه;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    إذا (invLabel == ARMPage_جدول_Unmap) {
        إذا (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely(invLabel != ARMPage_جدول_Map)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(درازی < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, میانگیر);
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vm_صفات_From_کلمه(getSyscallArg(1, میانگیر));
#پایان_اگر
    pdCap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(cap_get_capType(pdCap) != cap_صفحه_directory_cap ||
                 !cap_صفحه_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(pdCap));
    asid = cap_صفحه_directory_cap_get_cap_PDMapped_ASID(pdCap);

    إذا (unlikely(vaddr >= USER_بالاترین)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_بالاترین);
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    {
        پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret;

        پیدا_کن_ret = پیدا_کن_PDFor_ASID(asid);
        إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(پیدا_کن_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_شاخص = vaddr >> (صفحه_بیتها + PT_شاخص_بیتها);
    pd_جایگاه = &pd[pd_شاخص];
    إذا (unlikely(pde_ptr_get_pdeType(pd_جایگاه) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = se_L4_Delete_اولی;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap)));
#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_صفات_get_arm_Parity_Enabled(attr),
              0 /* Domain */
          );
#وإلا
    pde = pde_pde_coarse_new(paddr);
#پایان_اگر

    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTMapped_آدرس(cap, vaddr);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_صفحه_جدول_Invocation_Map(cap, cte, pde, pd_جایگاه);
}

ساكن exception_t decodeARMFrameInvocation(کلمه_t invLabel, کلمه_t درازی,
                                            cte_t *cte, cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    بدل (invLabel) {
    حالة ARMPageMap: {
        کلمه_t vaddr, vtop, w_rights_ماسک;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_صفحه_اندازه_t قاب_اندازه;
        vm_صفات_t attr;

        إذا (unlikely(درازی < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                se_L4_Truncated_پیام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, میانگیر);
        w_rights_ماسک = getSyscallArg(1, میانگیر);
        attr = vm_صفات_From_کلمه(getSyscallArg(2, میانگیر));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        قاب_اندازه = generic_قاب_cap_get_cap_FSize(cap);
        capVMRights = generic_قاب_cap_get_cap_FVMRights(cap);

        إذا (unlikely(cap_get_capType(pdCap) != cap_صفحه_directory_cap ||
                     !cap_صفحه_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(
                         pdCap));
        asid = cap_صفحه_directory_cap_get_cap_PDMapped_ASID(pdCap);

        إذا (generic_قاب_cap_get_cap_FIs_Mapped(cap)) {
            إذا (generic_قاب_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (generic_قاب_cap_get_cap_FMapped_آدرس(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_آرگومان;
                current_syscall_error.invalid_آرگومان_عدد = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            vtop = vaddr + بیت(صفحه_بیتها_For_اندازه(قاب_اندازه)) - 1;

            إذا (unlikely(vtop >= USER_بالاترین)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_بالاترین);
                current_syscall_error.type =
                    se_L4_Invalid_آرگومان;
                current_syscall_error.invalid_آرگومان_عدد = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            پیدا_کن_PDFor_ASID_ret_t پیدا_کن_ret;

            پیدا_کن_ret = پیدا_کن_PDFor_ASID(asid);
            إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_منبع =
                    false;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (unlikely(پیدا_کن_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            ماسک_VMRights(capVMRights, rights_From_کلمه(w_rights_ماسک));

        إذا (unlikely(!checkVPAlignment(قاب_اندازه, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((فراغ *)
                                   generic_قاب_cap_get_cap_FBase_Ptr(cap));

        cap = generic_قاب_cap_مجموعه_cap_FMapped_آدرس(cap, asid,
                                                      vaddr);
        إذا (قاب_اندازه == ARMSmall_صفحه || قاب_اندازه == ARMLarge_صفحه) {
            create_mappings_pte_return_t map_ret;
            map_ret = create_امن_Mapping_Entries_PTE(capFBasePtr, vaddr,
                                                   قاب_اندازه, vmRights,
                                                   attr, pd);
            إذا (unlikely(map_ret.وضعیت != EXCEPTION_NONE)) {
#اگر_تعریف_شده CONFIG_PRINTING
                إذا (current_syscall_error.type == se_L4_Delete_اولی) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#پایان_اگر
                أرجع map_ret.وضعیت;
            }

            مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_صفحه_Invocation_Map_PTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } وإلا {
            create_mappings_pde_return_t map_ret;
            map_ret = create_امن_Mapping_Entries_PDE(capFBasePtr, vaddr,
                                                   قاب_اندازه, vmRights,
                                                   attr, pd);
            إذا (unlikely(map_ret.وضعیت != EXCEPTION_NONE)) {
#اگر_تعریف_شده CONFIG_PRINTING
                إذا (current_syscall_error.type == se_L4_Delete_اولی) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#پایان_اگر
                أرجع map_ret.وضعیت;
            }

            مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_صفحه_Invocation_Map_PDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    حالة ARMPageUnmap: {
#اگر_تعریف_شده CONFIG_TK1_SMMU
        إذا (is_IOSpace_قاب_Cap(cap)) {
            مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_صفحه_Invocation_Unmap_IO(cap, cte);
        } وإلا
#پایان_اگر
        {
            مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_صفحه_Invocation_Unmap(cap, cte);
        }
    }

#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة ARMPageMapIO: {
        أرجع decodeARMIOMapInvocation(invLabel, درازی, cte, cap, میانگیر);
    }
#پایان_اگر

    حالة ARMPage_Clean_داده:
    حالة ARMPage_Invalidate_داده:
    حالة ARMPage_Clean_Invalidate_داده:
    حالة ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        پیدا_کن_PDFor_ASID_ret_t pd;
        vptr_t شروع, انتها;
        paddr_t pstart;
        کلمه_t صفحه_اندازه;
        کلمه_t صفحه_پایه_2;

        إذا (درازی < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_قاب_cap_get_cap_FMapped_ASID(cap);
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_قاب_cap_get_cap_FBase_Ptr(cap);
#وإلا
        vaddr = generic_قاب_cap_get_cap_FMapped_آدرس(cap);
#پایان_اگر

        إذا (unlikely(!generic_قاب_cap_get_cap_FIs_Mapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pd = پیدا_کن_PDFor_ASID(asid);
        إذا (unlikely(pd.وضعیت != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شروع = getSyscallArg(0, میانگیر);
        انتها =   getSyscallArg(1, میانگیر);

        /* check that the range is sane */
        إذا (انتها <= شروع) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        صفحه_اندازه = 1 << صفحه_بیتها_For_اندازه(generic_قاب_cap_get_cap_FSize(cap));
        صفحه_پایه_2 = addrFromPPtr((فراغ *)generic_قاب_cap_get_cap_FBase_Ptr(cap));

#اگر_تعریف_نشده CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        إذا (unlikely(!resolve_ret.valid ||
                     resolve_ret.قاب_اندازه != generic_قاب_cap_get_cap_FSize(cap) ||
                     resolve_ret.قاب_پایه != صفحه_پایه_2)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#پایان_اگر

        إذا (شروع >= صفحه_اندازه || انتها > صفحه_اندازه) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = صفحه_پایه_2 + شروع;
        شروع += vaddr;
        انتها += vaddr;

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        إذا (pstart < phys_پایه() || pstart > PADDR_بالاترین || (انتها - شروع) > PADDR_بالاترین - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
#پایان_اگر

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_Flush(invLabel, pd.pd, asid, شروع, انتها - 1, pstart);
    }

    حالة ARMPage_Get_آدرس: {


        /* Check that there are enough message registers */
        أكد(n_پیام_Registers >= 1);

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_Get_آدرس((فراغ *)generic_قاب_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(کلمه_t invLabel, کلمه_t درازی, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_صفحه_directory_cap:
        أرجع decodeARMPageDirectoryInvocation(invLabel, درازی, cptr, cte,
                                                cap, میانگیر);

    حالة cap_صفحه_جدول_cap:
        أرجع decode_ARMPage_جدول_Invocation(invLabel, درازی, cte, cap, میانگیر);

    حالة cap_small_قاب_cap:
    حالة cap_قاب_cap:
        أرجع decodeARMFrameInvocation(invLabel, درازی, cte, cap, call, میانگیر);

    حالة cap_asid_control_cap: {
        کلمه_t i;
        asid_t asid_پایه;
        کلمه_t شاخص, عمق;
        cap_t untyped, ریشه;
        cte_t *والد_جایگاه, *dest_جایگاه;
        lookup_جایگاه_ret_t lu_ret;
        فراغ *قاب;
        exception_t وضعیت;

        إذا (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(درازی < 2 || current_extra_caps.excaprefs[0] == عدم
                     || current_extra_caps.excaprefs[1] == عدم)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شاخص = getSyscallArg(0, میانگیر);
        عمق = getSyscallArg(1, میانگیر);
        والد_جایگاه = current_extra_caps.excaprefs[0];
        untyped = والد_جایگاه->cap;
        ریشه = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        إذا (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_اولی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_پایه = i << asid_کم_بیتها;

        إذا (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_بلوک_اندازه(untyped) !=
                     se_L4_ASIDPool_بیتها || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        وضعیت = ensureNoChildren(والد_جایگاه);
        إذا (unlikely(وضعیت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            أرجع وضعیت;
        }

        قاب = کلمه_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_هدف_جایگاه(ریشه, شاخص, عمق);
        إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            أرجع lu_ret.وضعیت;
        }
        dest_جایگاه = lu_ret.جایگاه;

        وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (unlikely(وضعیت != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(قاب, dest_جایگاه,
                                            والد_جایگاه, asid_پایه);
    }

    حالة cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pd_Cap_جایگاه;
        asid_pool_t *pool;
        کلمه_t i;
        asid_t asid;

        إذا (unlikely(invLabel != ARMASIDPool_تخصیص_بده)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pd_Cap_جایگاه = current_extra_caps.excaprefs[0];
        pdCap = pd_Cap_جایگاه->cap;

        إذا (unlikely(
                cap_get_capType(pdCap) != cap_صفحه_directory_cap ||
                cap_صفحه_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_بالا(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            current_lookup_fault = lookup_fault_invalid_ریشه_new();

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
        لكل (i = 0; i < (1 << asid_کم_بیتها) && (asid + i == 0 || pool->array[i]); i++);

        إذا (unlikely(i == 1 << asid_کم_بیتها)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = se_L4_Delete_اولی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, pd_Cap_جایگاه);
    }

    افتراضي:
        fail("Invalid ARM arch cap type");
    }
}

#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
exception_t benchmark_arch_map_log_میانگیر(کلمه_t قاب_cptr)
{
    lookup_Cap_And_جایگاه_ret_t lu_ret;
    vm_صفحه_اندازه_t قاب_اندازه;
    pptr_t  قاب_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_جایگاه(گره_STATE(ksCurThread), قاب_cptr);

    إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", قاب_cptr);
        current_fault = seL4_Fault_CapFault_new(قاب_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_قاب_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(قاب_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    قاب_اندازه = generic_قاب_cap_get_cap_FSize(lu_ret.cap);

    إذا (قاب_اندازه != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(قاب_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    قاب_pptr = generic_قاب_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_میانگیر = pptr_مقصد_paddr((فراغ *) قاب_pptr);

    لكل (صحيح idx = 0; idx < بیت(PT_شاخص_بیتها); idx++) {
        paddr_t physical_آدرس = ks_User_Log_میانگیر + (idx << se_L4_صفحه_بیتها);

        armKSGlobalLogPT[idx] =
            pte_pte_small_new(
                physical_آدرس,
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
        invalidateTranslationSingle(KS_LOG_PPTR + (idx * بیت(se_L4_صفحه_بیتها)));
    }

    أرجع EXCEPTION_NONE;
}
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */

#اگر_تعریف_شده CONFIG_DEBUG_BUILD
فراغ kernelPrefetchAbort(کلمه_t pc, کلمه_t lr) VISIBLE;
فراغ kernel_داده_Abort(کلمه_t pc) VISIBLE;

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT

فراغ kernelUndefinedInstruction(کلمه_t pc) VISIBLE;

فراغ kernelPrefetchAbort(کلمه_t pc, کلمه_t lr)
{
    چاپ_کن("\n\nKERNEL PREFETCH ABORT!\n");
    چاپ_کن("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کن("HSR: 0x%"SEL4_PRIx_کلمه"\n", getHSR());
    چاپ_کن("LR: 0x%"SEL4_PRIx_کلمه"\n", lr);
    halt();
}

فراغ kernel_داده_Abort(کلمه_t pc)
{
    چاپ_کن("\n\nKERNEL DATA ABORT!\n");
    چاپ_کن("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کن("HDFAR: 0x%"SEL4_PRIx_کلمه" HSR: 0x%"SEL4_PRIx_کلمه"\n",
           getHDFAR(), getHSR());
    halt();
}

فراغ kernelUndefinedInstruction(کلمه_t pc)
{
    چاپ_کن("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    چاپ_کن("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کن("HSR: 0x%"SEL4_PRIx_کلمه"\n", getHSR());
    halt();
}

#وإلا /* CONFIG_ARM_HYPERVISOR_SUPPORT */

فراغ kernelPrefetchAbort(کلمه_t pc, کلمه_t lr)
{
    چاپ_کن("\n\nKERNEL PREFETCH ABORT!\n");
    چاپ_کن("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کن("IFSR: 0x%"SEL4_PRIx_کلمه"\n", getIFSR());
    چاپ_کن("LR: 0x%"SEL4_PRIx_کلمه"\n", lr);
    halt();
}

فراغ kernel_داده_Abort(کلمه_t pc)
{
    چاپ_کن("\n\nKERNEL DATA ABORT!\n");
    چاپ_کن("Faulting instruction: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کن("FAR: 0x%"SEL4_PRIx_کلمه" DFSR: 0x%"SEL4_PRIx_کلمه"\n",
           getFAR(), getDFSR());
    halt();
}
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#پایان_اگر

#اگر_تعریف_شده CONFIG_PRINTING
تعريفنوع بنية read_کلمه_From_VSpace_ret {
    exception_t وضعیت;
    کلمه_t مقدار;
} read_کلمه_From_VSpace_ret_t;

ساكن read_کلمه_From_VSpace_ret_t read_کلمه_From_VSpace(pde_t *pd, کلمه_t vaddr)
{
    read_کلمه_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_جایگاه;
    pde_t *pd_جایگاه;
    paddr_t paddr;
    کلمه_t جابه_جایی;
    pptr_t kernel_vaddr;
    کلمه_t *مقدار;

    pd_جایگاه = lookupPDSlot(pd, vaddr);
    إذا (pde_ptr_get_pdeType(pd_جایگاه) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_آدرس(pd_جایگاه);
        جابه_جایی = vaddr & ماسک(ARMSection_بیتها);
    } وإلا {
        pt_جایگاه = lookupPTSlot(pd, vaddr);
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        إذا (pt_جایگاه.وضعیت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_جایگاه.pt_جایگاه) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_آدرس(pt_جایگاه.pt_جایگاه);
            إذا (pte_pte_small_ptr_get_contiguous_hint(pt_جایگاه.pt_جایگاه)) {
                جابه_جایی = vaddr & ماسک(ARMLarge_صفحه_بیتها);
            } وإلا {
                جابه_جایی = vaddr & ماسک(ARMSmall_صفحه_بیتها);
            }
#وإلا
        إذا (pt_جایگاه.وضعیت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_جایگاه.pt_جایگاه) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_آدرس(pt_جایگاه.pt_جایگاه);
            جابه_جایی = vaddr & ماسک(ARMSmall_صفحه_بیتها);
        } وإلا إذا (pt_جایگاه.وضعیت == EXCEPTION_NONE && pte_ptr_get_pteType(pt_جایگاه.pt_جایگاه) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_آدرس(pt_جایگاه.pt_جایگاه);
            جابه_جایی = vaddr & ماسک(ARMLarge_صفحه_بیتها);
#پایان_اگر
        } وإلا {
            ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }
    }


    kernel_vaddr = (کلمه_t)paddr_مقصد_pptr(paddr);
    مقدار = (کلمه_t *)(kernel_vaddr + جابه_جایی);
    ret.وضعیت = EXCEPTION_NONE;
    ret.مقدار = *مقدار;
    أرجع ret;
}

فراغ Arch_user_پشته_Trace(tcb_t *tptr)
{
    cap_t thread_ریشه;
    pde_t *pd;
    کلمه_t sp;
    صحيح i;

    thread_ریشه = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    إذا (cap_get_capType(thread_ریشه) != cap_صفحه_directory_cap) {
        چاپ_کن("Invalid vspace\n");
        أرجع;
    }

    pd = (pde_t *)pptr_از_cap(thread_ریشه);

    sp = get_ثبات(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_کلمه_اندازه_بیتها)) {
        چاپ_کن("SP not aligned\n");
        أرجع;
    }

    لكل (i = 0; i < CONFIG_USER_پشته_TRACE_درازی; i++) {
        کلمه_t آدرس = sp + (i * حجم(کلمه_t));
        read_کلمه_From_VSpace_ret_t نتیجه;
        نتیجه = read_کلمه_From_VSpace(pd, آدرس);
        إذا (نتیجه.وضعیت == EXCEPTION_NONE) {
            چاپ_کن("0x%lx: 0x%lx\n", (دراز)آدرس, (دراز)نتیجه.مقدار);
        } وإلا {
            چاپ_کن("0x%lx: INVALID\n", (دراز)آدرس);
        }
    }
}
#پایان_اگر
