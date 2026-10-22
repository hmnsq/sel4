/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/stack.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/cnode.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/untyped.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <armv/context_switch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/iospace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/vcpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/tlb.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>
#ਜੇ_ਅੰਤ

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SECTIONS_ਪ੍ਰਤੀ_SUPER_SECTION ਦੋਆਧਾਰੀ_ਅੰਕ(ARMSuper_Section_ਬਿੱਟ - ARMSection_ਬਿੱਟ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪੰਨੇ_ਪ੍ਰਤੀ_LARGE_ਪੰਨਾ ਦੋਆਧਾਰੀ_ਅੰਕ(ARMLarge_ਪੰਨਾ_ਬਿੱਟ - ARMSmall_ਪੰਨਾ_ਬਿੱਟ)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਆਖਰੀ_BYTE_PTE(PTE,ਲੰਮਾਈ_2) ((ਸ਼ਬਦ_t)&(PTE)[(ਲੰਮਾਈ_2)-1] + (ਦੋਆਧਾਰੀ_ਅੰਕ(PTE_ਮਾਪ_ਬਿੱਟ)-1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਆਖਰੀ_BYTE_PDE(PDE,ਲੰਮਾਈ_2) ((ਸ਼ਬਦ_t)&(PDE)[(ਲੰਮਾਈ_2)-1] + (ਦੋਆਧਾਰੀ_ਅੰਕ(PDE_ਮਾਪ_ਬਿੱਟ)-1))

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_Global_PD(ਖਾਲੀ);

ਬਣਤਰ resolve_ret {
    paddr_t ਚੌਖਟਾ_ਅਧਾਰ;
    vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ;
    bool_t valid;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ resolve_ret resolve_ret_t;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
ਸਥਿਰ bool_t ਸ਼ੁੱਧ pteCheckIfMapped(pte_t *pte);
ਸਥਿਰ bool_t ਸ਼ੁੱਧ pdeCheckIfMapped(pde_t *pde);

ਸਥਿਰ ਸ਼ਬਦ_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ਚੋਣ (vm_rights) {
    ਹਾਲਤ VMNoAccess:
        ਵਾਪਸ 0;

    ਹਾਲਤ VMKernelOnly:
        ਵਾਪਸ 1;

    ਹਾਲਤ VMReadOnly:
        ਵਾਪਸ 2;

    ਹਾਲਤ VMReadWrite:
        ਵਾਪਸ 3;

    ਮੂਲ:
        fail("Invalid VM rights");
    }
}

#ਨਹੀਂਤਾਂ
/* AP encoding slightly different. AP only used for kernel mappings which are fixed after boot time */
BOOT_ਕੋਡ
ਸਥਿਰ ਸ਼ਬਦ_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ਚੋਣ (vm_rights) {
    ਹਾਲਤ VMKernelOnly:
        ਵਾਪਸ 0;
    ਹਾਲਤ VMReadWrite:
        ਵਾਪਸ 1;
    ਹਾਲਤ VMNoAccess:
        /* RO at PL1 only */
        ਵਾਪਸ 2;
    ਹਾਲਤ VMReadOnly:
        ਵਾਪਸ 3;
    ਮੂਲ:
        fail("Invalid VM rights");
    }
}

ਸਥਿਰ ਸ਼ਬਦ_t CONST HAPFromVMRights(vm_rights_t vm_rights)
{
    ਚੋਣ (vm_rights) {
    ਹਾਲਤ VMKernelOnly:
    ਹਾਲਤ VMNoAccess:
        ਵਾਪਸ 0;
    ਹਾਲਤ VMReadOnly:
        ਵਾਪਸ 1;
    /*
    case VMWriteOnly:
        return 2;
    */
    ਹਾਲਤ VMReadWrite:
        ਵਾਪਸ 3;
    ਮੂਲ:
        fail("Invalid VM rights");
    }
}

#ਜੇ_ਅੰਤ

vm_rights_t CONST ਨਕਾਬ_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ਨਕਾਬ)
{
    ਜੇ (vm_rights == VMNoAccess) {
        ਵਾਪਸ VMNoAccess;
    }
    ਜੇ (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ)) {
        ਵਾਪਸ VMReadOnly;
    }
    ਜੇ (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ)) {
        ਜੇ (!seL4_CapRights_get_capAllowWrite(cap_rights_ਨਕਾਬ)) {
            ਵਾਪਸ VMReadOnly;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ VMReadWrite;
        }
    }
    ਜੇ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ਨਕਾਬ)) {
        userError("Attempted to make unsupported write only mapping");
    }
    ਵਾਪਸ VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_ਕੋਡ ਖਾਲੀ map_kernel_ਚੌਖਟਾ(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_ਗੁਣ_t ਗੁਣ)
{
    ਸ਼ਬਦ_t idx = (vaddr & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection))) >> ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSmall_ਪੰਨਾ);

    ਪ੍ਰਤਿਜ੍ਞਾ(vaddr >= PPTR_ਸਿਖਰ); /* vaddr lies in the region the global PT covers */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸ਼ਬਦ_t tex;
    ਜੇ (vm_ਗੁਣ_get_arm_ਪੰਨਾ_Cacheable(ਗੁਣ)) {
        tex = 5; /* TEX = 0b1(Cached)01(Outer Write Allocate) */
    } ਨਹੀਂਤਾਂ {
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
#ਨਹੀਂਤਾਂ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
            vm_ਗੁਣ_get_arm_ਪੰਨਾ_Cacheable(ਗੁਣ)
        );
#ਜੇ_ਅੰਤ /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_ਕੋਡ ਖਾਲੀ map_kernel_ਖਿੜਕੀ(ਖਾਲੀ)
{
    paddr_t  phys;
    ਸ਼ਬਦ_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = phys_ਅਧਾਰ();
    idx = PPTR_ਅਧਾਰ >> ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection);

    ਜਦੋਂ (idx < ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - SECTIONS_ਪ੍ਰਤੀ_SUPER_SECTION) {
        ਸ਼ਬਦ_t idx2;

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
        ਲਈ (idx2 = idx; idx2 < idx + SECTIONS_ਪ੍ਰਤੀ_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSuperSection));
        idx += SECTIONS_ਪ੍ਰਤੀ_SUPER_SECTION;
    }

    ਜਦੋਂ (idx < (PPTR_ਸਿਖਰ >> ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection))) {
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
        phys += ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    ਪ੍ਰਤਿਜ੍ਞਾ(phys == PADDR_ਸਿਖਰ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /*ਪਤਾ*/
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));

    phys += ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection));
    idx++;
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */

    /* map page table covering last 1M of virtual address space to page directory */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalPT), /*ਪਤਾ*/
            true,                        /* P       */
            0                            /* Domain  */
        );

    /* now start initialising the page table */
    memzero(armKSGlobalPT, 1 << se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ);

    /* map vector table */
    map_kernel_ਚੌਖਟਾ(
        addrFromKPPtr(arm_ਸਦਿਸ਼_ਸਾਰਣੀ),
        PPTR_ਸਦਿਸ਼_ਸਾਰਣੀ,
        VMKernelOnly,
        vm_ਗੁਣ_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#ਨਹੀਂਤਾਂ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_ਕੋਡ ਖਾਲੀ map_kernel_ਖਿੜਕੀ(ਖਾਲੀ)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t ਅਣਵਰਤਿਆ pteS2;

    /* Initialise PGD */
    ਲਈ (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    ਲਈ (idx = 0; idx < (USER_ਸਿਖਰ - 0xC0000000) >> (PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = phys_ਅਧਾਰ();
    ਲਈ (; idx < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1; idx++) {
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
        phys += ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ);
    ਲਈ (idx = 0; idx < 256; idx++) {
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
        phys += ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ);
    }
    /* map vector table */
    map_kernel_ਚੌਖਟਾ(
        addrFromKPPtr(arm_ਸਦਿਸ਼_ਸਾਰਣੀ),
        PPTR_ਸਦਿਸ਼_ਸਾਰਣੀ,
        VMKernelOnly,
        vm_ਗੁਣ_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#ਜੇ_ਅੰਤ /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

ਸਥਿਰ BOOT_ਕੋਡ ਖਾਲੀ map_it_ਚੌਖਟਾ_cap(cap_t pd_cap, cap_t ਚੌਖਟਾ_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *ਨਿਸ਼ਾਨਾ_ਥਾਂ;
    pde_t *pd    = PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    ਖਾਲੀ  *ਚੌਖਟਾ = (ਖਾਲੀ *)generic_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(ਚੌਖਟਾ_cap);
    vptr_t vptr  = generic_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(ਚੌਖਟਾ_cap);

    ਪ੍ਰਤਿਜ੍ਞਾ(generic_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(ਚੌਖਟਾ_cap) != 0);

    pd += (vptr >> ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ਪਤਾ(pd));
    ਨਿਸ਼ਾਨਾ_ਥਾਂ = pt + ((vptr & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection)))
                       >> ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSmall_ਪੰਨਾ));
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    *ਨਿਸ਼ਾਨਾ_ਥਾਂ = pte_pte_small_new(
                      addrFromPPtr(ਚੌਖਟਾ),
                      1, /* not global */
                      SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                      0, /* APX = 0, privileged full access */
                      5, /* TEX = 0b1(Cached)01(Outer Write Allocate) */
                      APFromVMRights(VMReadWrite),
                      0, /* C (Inner write allocate) */
                      1, /* B (Inner write allocate) */
                      !executable
                  );
#ਨਹੀਂਤਾਂ
    *ਨਿਸ਼ਾਨਾ_ਥਾਂ = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(ਚੌਖਟਾ),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#ਜੇ_ਅੰਤ
}

/* Create a frame cap for the initial thread. */

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_ਚੌਖਟਾ_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    ਜੇ (use_large)
        ਵਾਪਸ
            cap_ਚੌਖਟਾ_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_ਨੀਵਾਂ(asid),                /* capFMappedASIDLow  */
                ਸ਼ਬਦ_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_ਉੱਚਾ(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    ਨਹੀਂਤਾਂ
        ਵਾਪਸ
            cap_small_ਚੌਖਟਾ_cap_new(
                ASID_ਨੀਵਾਂ(asid),                /* capFMappedASIDLow  */
                ਸ਼ਬਦ_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#ਜੇ_ਅੰਤ
                ASID_ਉੱਚਾ(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

ਸਥਿਰ BOOT_ਕੋਡ ਖਾਲੀ map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(pt_cap);
    pde_t *ਨਿਸ਼ਾਨਾ_ਥਾਂ = pd + (vptr >> ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection));

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(pt_cap));

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    *ਨਿਸ਼ਾਨਾ_ਥਾਂ = pde_pde_coarse_new(
                      addrFromPPtr(pt), /*ਪਤਾ*/
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#ਨਹੀਂਤਾਂ
    *ਨਿਸ਼ਾਨਾ_ਥਾਂ = pde_pde_coarse_new(addrFromPPtr(pt));
#ਜੇ_ਅੰਤ
}

/* Create a page table for the initial thread */

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_ਪੰਨਾ_ਸਾਰਣੀ_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    ਜੇ (asid != asidInvalid) {
        map_it_pt_cap(pd, cap);
    }
    ਵਾਪਸ cap;
}

BOOT_ਕੋਡ ਸ਼ਬਦ_t arch_get_n_paging(v_region_t it_v_reg)
{
    ਵਾਪਸ get_n_paging(it_v_reg, PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_ਕੋਡ cap_t create_it_ਪਤਾ_space(cap_t ਜੜ੍ਹ_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_before;
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << se_L4_ਪੰਨਾ_Dir_ਬਿੱਟ) - 1,
                        addrFromPPtr((ਖਾਲੀ *)rootserver.vspace));
    cap_t pd_cap =
        cap_ਪੰਨਾ_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    ਥਾਂ_pos_before = ndks_boot.ਥਾਂ_pos_cur;
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    ਲਈ (pt_vptr = ਗੇੜਾ_ਥੱਲੇ(it_v_reg.ਸ਼ੁਰੂ, PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ);
         pt_vptr < it_v_reg.ਸਿਰਾ;
         pt_vptr += ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ)) {
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap,
                         create_it_ਪੰਨਾ_ਸਾਰਣੀ_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            ਵਾਪਸ cap_null_cap_new();
        }
    }

    ਥਾਂ_pos_after = ndks_boot.ਥਾਂ_pos_cur;
    ndks_boot.bi_ਚੌਖਟਾ->userImagePaging = (se_L4_ਥਾਂ_Region) {
        ਥਾਂ_pos_before, ਥਾਂ_pos_after
    };

    ਵਾਪਸ pd_cap;
}

BOOT_ਕੋਡ cap_t create_unmapped_it_ਚੌਖਟਾ_cap(pptr_t pptr, bool_t use_large)
{
    ਵਾਪਸ create_it_ਚੌਖਟਾ_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_ਕੋਡ cap_t create_mapped_it_ਚੌਖਟਾ_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_ਚੌਖਟਾ_cap(pptr, vptr, asid, use_large);
    map_it_ਚੌਖਟਾ_cap(pd_cap, cap, executable);
    ਵਾਪਸ cap;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_ਕੋਡ ਖਾਲੀ activate_kernel_vspace(ਖਾਲੀ)
{
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    ਸੈੱਟ_Global_PD();
    invalidateLocalTLB();
    ਤਾਲਾ_TLBEntry(PPTR_ਅਧਾਰ);
    ਤਾਲਾ_TLBEntry(PPTR_ਸਦਿਸ਼_ਸਾਰਣੀ);
}

#ਨਹੀਂਤਾਂ

BOOT_ਕੋਡ ਖਾਲੀ activate_kernel_vspace(ਖਾਲੀ)
{
    uint32_t r;
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    /* Setup the memory attributes: We use 2 indices (cachable/non-cacheable) */
    ਸੈੱਟ_HMAIR((ATTRINDX_NONCACHEABLE << 0) | (ATTRINDX_CACHEABLE << 8), 0);
    ਸੈੱਟ_Current_Hyp_PD(addrFromKPPtr(armHSGlobalPGD));
    invalidateHypTLB();
#ਜੇ 0 /* Can't lock entries on A15 */
    ਤਾਲਾ_TLBEntry(PPTR_ਅਧਾਰ);
    ਤਾਲਾ_TLBEntry(PPTR_ਸਦਿਸ਼_ਸਾਰਣੀ);
#ਜੇ_ਅੰਤ
    /* TODO find a better place to init the VMMU */
    r = 0;
    /* Translation range */
    r |= (0x0 << 0);     /* 2^(32 -(0)) input range. */
    r |= (r & 0x8) << 1; /* Sign bit */
    /* starting level */
    r |= (0x0 << 6);     /* Start at second level */
    /* Sharability of tables */
    r |= ਦੋਆਧਾਰੀ_ਅੰਕ(8);       /* Inner write-back, write-allocate */
    r |= ਦੋਆਧਾਰੀ_ਅੰਕ(10);      /* Outer write-back, write-allocate */
    /* Long descriptor format (not that we have a choice) */
    r |= ਦੋਆਧਾਰੀ_ਅੰਕ(31);
    ਸੈੱਟ_VTCR(r);
}

#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_ਕੋਡ ਖਾਲੀ write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_ਦਾ_cap(it_ap_cap));
    ap->array[ASID_ਨੀਵਾਂ(IT_ASID)] = PDE_PTR(pptr_ਦਾ_cap(it_pd_cap));
    armKSASIDTable[ASID_ਉੱਚਾ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_PDFor_ASID(asid_t asid)
{
    ਲੱਭੋ_PDFor_ASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਜੇ (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();

        ret.pd = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    pd = poolPtr->array[ASID_ਨੀਵਾਂ(asid)];
    ਜੇ (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();

        ret.pd = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ret.pd = pd;
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

ਸ਼ਬਦ_t *ਸ਼ੁੱਧ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    ਸ਼ਬਦ_t w_ਬਫਰ_Ptr;
    cap_t ਬਫਰ_Cap;
    vm_rights_t vm_rights;

    w_ਬਫਰ_Ptr = thread->tcbIPCBuffer;
    ਬਫਰ_Cap = TCB_PTR_CTE_PTR(thread, tcb_ਬਫਰ)->cap;

    ਜੇ (unlikely(cap_get_capType(ਬਫਰ_Cap) != cap_small_ਚੌਖਟਾ_cap &&
                 cap_get_capType(ਬਫਰ_Cap) != cap_ਚੌਖਟਾ_cap)) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (unlikely(generic_ਚੌਖਟਾ_cap_get_cap_FIs_Device(ਬਫਰ_Cap))) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    vm_rights = generic_ਚੌਖਟਾ_cap_get_cap_FVMRights(ਬਫਰ_Cap);
    ਜੇ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        ਸ਼ਬਦ_t ਅਧਾਰ_Ptr;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਪੰਨਾ_ਬਿੱਟ_2;

        ਅਧਾਰ_Ptr = generic_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(ਬਫਰ_Cap);
        ਪੰਨਾ_ਬਿੱਟ_2 = ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(generic_ਚੌਖਟਾ_cap_get_cap_FSize(ਬਫਰ_Cap));
        ਵਾਪਸ (ਸ਼ਬਦ_t *)(ਅਧਾਰ_Ptr + (w_ਬਫਰ_Ptr & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_2)));
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਨਿਰਾ;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ਜੇ (unlikely(cap_get_capType(cap) != cap_small_ਚੌਖਟਾ_cap &&
                 cap_get_capType(cap) != cap_ਚੌਖਟਾ_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (unlikely(generic_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(vptr & ਨਕਾਬ(se_L4_IPCBuffer_ਮਾਪ_ਬਿੱਟ))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (ਪੂਰਨ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pd_ਸੂਚਕਾਂਕ;

    pd_ਸੂਚਕਾਂਕ = vptr >> (ਪੰਨਾ_ਬਿੱਟ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ);
    ਵਾਪਸ pd + pd_ਸੂਚਕਾਂਕ;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pd_ਥਾਂ;

    pd_ਥਾਂ = lookupPDSlot(pd, vptr);

    ਜੇ (unlikely(pde_ptr_get_pdeType(pd_ਥਾਂ) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ);

        ret.pt_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    } ਨਹੀਂਤਾਂ {
        pte_t *pt, *pt_ਥਾਂ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pt_ਸੂਚਕਾਂਕ;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ਪਤਾ(pd_ਥਾਂ));
        pt_ਸੂਚਕਾਂਕ = (vptr >> ਪੰਨਾ_ਬਿੱਟ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ);
        pt_ਥਾਂ = pt + pt_ਸੂਚਕਾਂਕ;

        ret.pt_ਥਾਂ = pt_ਥਾਂ;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
    }
}

ਸਥਿਰ pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pt_ਸੂਚਕਾਂਕ;

    pt_ਸੂਚਕਾਂਕ = (vptr >> ਪੰਨਾ_ਬਿੱਟ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ);
    ਵਾਪਸ pt + pt_ਸੂਚਕਾਂਕ;
}

ਸਥਿਰ ਅਚਲ resolve_ret_t default_resolve_ret_t;

ਸਥਿਰ resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    ਚੋਣ (pde_ptr_get_pdeType(pde)) {
    ਹਾਲਤ pde_pde_section:
        ret.ਚੌਖਟਾ_ਅਧਾਰ = pde_pde_section_ptr_get_ਪਤਾ(pde);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਜੇ (pde_pde_section_ptr_get_ਮਾਪ(pde)) {
            ret.ਚੌਖਟਾ_ਮਾਪ = ARMSuperSection;
        } ਨਹੀਂਤਾਂ {
            ret.ਚੌਖਟਾ_ਮਾਪ = ARMSection;
        }
#ਨਹੀਂਤਾਂ
        ਜੇ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.ਚੌਖਟਾ_ਅਧਾਰ &= ~ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSuperSection));
            ret.ਚੌਖਟਾ_ਮਾਪ = ARMSuperSection;
        } ਨਹੀਂਤਾਂ {
            ret.ਚੌਖਟਾ_ਮਾਪ = ARMSection;
        }
#ਜੇ_ਅੰਤ
        ਵਾਪਸ ret;

    ਹਾਲਤ pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ਪਤਾ(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        ਚੋਣ (pte_ptr_get_pteType(pte)) {
        ਹਾਲਤ pte_pte_small:
            ret.ਚੌਖਟਾ_ਅਧਾਰ = pte_pte_small_ptr_get_ਪਤਾ(pte);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
            ਜੇ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.ਚੌਖਟਾ_ਅਧਾਰ &= ~ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMLarge_ਪੰਨਾ));
                ret.ਚੌਖਟਾ_ਮਾਪ = ARMLarge_ਪੰਨਾ;
            } ਨਹੀਂਤਾਂ {
                ret.ਚੌਖਟਾ_ਮਾਪ = ARMSmall_ਪੰਨਾ;
            }
#ਨਹੀਂਤਾਂ
            ret.ਚੌਖਟਾ_ਮਾਪ = ARMSmall_ਪੰਨਾ;
#ਜੇ_ਅੰਤ
            ਵਾਪਸ ret;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਹਾਲਤ pte_pte_large:
            ret.ਚੌਖਟਾ_ਅਧਾਰ = pte_pte_large_ptr_get_ਪਤਾ(pte);
            ret.ਚੌਖਟਾ_ਮਾਪ = ARMLarge_ਪੰਨਾ;
            ਵਾਪਸ ret;
#ਜੇ_ਅੰਤ
        ਮੂਲ:
            ਰੋਕੋ;
        }
        ਰੋਕੋ;
    }
    }

    ret.valid = false;
    ਵਾਪਸ ret;
}

ਸਥਿਰ pte_t CONST makeUserPTE(vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸ਼ਬਦ_t ap;

    ap = APFromVMRights(vm_rights);

    ਚੋਣ (ਪੰਨਾ_ਮਾਪ) {
    ਹਾਲਤ ARMSmall_ਪੰਨਾ: {
        ਜੇ (cacheable) {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    nonexecutable);
        } ਨਹੀਂਤਾਂ {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    0, /* TEX = 0b000, strongly-ordered. */
                                    ap,
                                    0, 0,
                                    nonexecutable);
        }
        ਰੋਕੋ;
    }

    ਹਾਲਤ ARMLarge_ਪੰਨਾ: {
        ਜੇ (cacheable) {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    1 /* reserved */);
        } ਨਹੀਂਤਾਂ {
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
        ਰੋਕੋ;
    }

    ਮੂਲ:
        fail("Invalid PTE frame type");
    }

#ਨਹੀਂਤਾਂ

    ਸ਼ਬਦ_t hap;

    hap = HAPFromVMRights(vm_rights);

    ਚੋਣ (ਪੰਨਾ_ਮਾਪ) {
    ਹਾਲਤ ARMSmall_ਪੰਨਾ: {
        ਜੇ (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_CACHEABLE /* Cacheable */);
        } ਨਹੀਂਤਾਂ {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        ਰੋਕੋ;
    }

    ਹਾਲਤ ARMLarge_ਪੰਨਾ: {
        ਜੇ (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_CACHEABLE  /* Cacheable */);
        } ਨਹੀਂਤਾਂ {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        ਰੋਕੋ;
    }
    ਮੂਲ:
        fail("Invalid PTE frame type");
    }
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    ਵਾਪਸ pte;
}

ਸਥਿਰ pde_t CONST makeUserPDE(vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, ਸ਼ਬਦ_t domain,
                               vm_rights_t vm_rights)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸ਼ਬਦ_t ap, size2;

    ap = APFromVMRights(vm_rights);
#ਨਹੀਂਤਾਂ
    ਸ਼ਬਦ_t hap, size2;

    (ਖਾਲੀ)domain;
    hap = HAPFromVMRights(vm_rights);
#ਜੇ_ਅੰਤ

    ਚੋਣ (ਪੰਨਾ_ਮਾਪ) {
    ਹਾਲਤ ARMSection:
        size2 = 0;
        ਰੋਕੋ;

    ਹਾਲਤ ARMSuperSection:
        size2 = 1;
        ਰੋਕੋ;

    ਮੂਲ:
        fail("Invalid PDE frame type");
    }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਜੇ (cacheable) {
        ਵਾਪਸ pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                   0, /* APX = 0, privileged full access */
                                   5, /* TEX = 0b101, outer write-back, write-allocate */
                                   ap, parity, domain, nonexecutable,
                                   0, 1 /* Inner write-back, write-allocate */);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   1, /* shared */
                                   0, /* APX = 0, privileged full access */
                                   0, /* TEX = 0b000, strongly-ordered */
                                   ap, parity, domain, nonexecutable,
                                   0, 0);
    }
#ਨਹੀਂਤਾਂ
    ਜੇ (cacheable) {
        ਵਾਪਸ pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_CACHEABLE /* Cacheable */);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_NONCACHEABLE /* Not cacheable */);
    }
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_ਜੜ੍ਹ(cap_t cap)
{
    ਵਾਪਸ cap_get_capType(cap) == cap_ਪੰਨਾ_directory_cap &&
           cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(cap);
}

bool_t CONST is_IOSpace_ਚੌਖਟਾ_Cap(cap_t cap)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਵਾਪਸ cap_get_capType(cap) == cap_small_ਚੌਖਟਾ_cap && cap_small_ਚੌਖਟਾ_cap_get_cap_FIs_IOSpace(cap);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ false;
#ਜੇ_ਅੰਤ
}

/** Switch to the empty PD on the reserved HW ASID */
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_Global_PD(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    writeContextIDAndPD(hwASIDReserved, addrFromKPPtr(armUSGlobalPD));
#ਨਹੀਂਤਾਂ
    /* Before changing the PD ensure all memory accesses have completed */
    dsb();
    /* First switch to global PD on old ASID. Stale TLB entries may exist
       under the old ASID, but no new stale mappings can be added any more. */
    writeTTBR0Ptr(addrFromKPPtr(armKSGlobalPD));
    /* Ensure the PD switch completes before we do anything else */
    isb();
    /* Switch to reserved HW ASID. Only empty/global kernel mappings are
       now available from the TLB. */
    ਸੈੱਟ_Hardware_ASID(hwASIDReserved);
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ ਸੈੱਟ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ਜੜ੍ਹ;
    asid_t asid;
    pde_t *pd;
    ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    ਜੇ (cap_get_capType(thread_ਜੜ੍ਹ) != cap_ਪੰਨਾ_directory_cap ||
        !cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(thread_ਜੜ੍ਹ)) {
        ਸੈੱਟ_Global_PD();
        ਵਾਪਸ;
    }

    pd = PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(thread_ਜੜ੍ਹ));
    asid = cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(thread_ਜੜ੍ਹ);
    ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(asid);
    ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE || ਲੱਭੋ_ret.pd != pd)) {
        ਸੈੱਟ_Global_PD();
        ਵਾਪਸ;
    }

    armv_contextSwitch(pd, asid);
}

ਸਥਿਰ bool_t ਸੈੱਟ_VMRoot_For_Flush(pde_t *pd, asid_t asid)
{
    cap_t thread_ਜੜ੍ਹ;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(ਗੰਢ_STATE(ksCurThread), tcbVTable)->cap;

    ਜੇ (cap_get_capType(thread_ਜੜ੍ਹ) == cap_ਪੰਨਾ_directory_cap &&
        cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(thread_ਜੜ੍ਹ) &&
        PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(thread_ਜੜ੍ਹ)) == pd) {
        ਵਾਪਸ false;
    }

    armv_contextSwitch(pd, asid);

    ਵਾਪਸ true;
}

pde_t *ਪੰਨਾ_ਸਾਰਣੀ_Mapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret;
    pde_t pde;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pd_ਸੂਚਕਾਂਕ;

    ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(asid);
    ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    pd_ਸੂਚਕਾਂਕ = vaddr >> (ਪੰਨਾ_ਬਿੱਟ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ);
    pde = ਲੱਭੋ_ret.pd[pd_ਸੂਚਕਾਂਕ];

    ਜੇ (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_ਪਤਾ(pde)) == pt)) {
        ਵਾਪਸ ਲੱਭੋ_ret.pd;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਨਿਰਾ;
    }
}

ਸਥਿਰ ਖਾਲੀ invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(asidPool);

    pd = asidPool->array[ASID_ਨੀਵਾਂ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(pd);

    pd[PD_ASID_ਥਾਂ] = pde_pde_invalid_new(0, false);
}

ਸਥਿਰ pde_t ਸ਼ੁੱਧ ਲੱਦੋ_HWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(asidPool);

    pd = asidPool->array[ASID_ਨੀਵਾਂ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(pd);

    ਵਾਪਸ pd[PD_ASID_ਥਾਂ];
}

ਸਥਿਰ ਖਾਲੀ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(asidPool);

    pd = asidPool->array[ASID_ਨੀਵਾਂ(asid)];
    ਪ੍ਰਤਿਜ੍ਞਾ(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_ਥਾਂ] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t ਲੱਭੋ_Free_HWASID(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t hw_asid_ਵਿੱਥ;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    ਲਈ (hw_asid_ਵਿੱਥ = 0;
         hw_asid_ਵਿੱਥ <= (ਸ਼ਬਦ_t)((hw_asid_t) - 1);
         hw_asid_ਵਿੱਥ ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_ਵਿੱਥ);
        ਜੇ (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            ਵਾਪਸ hw_asid;
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
    ਜੇ (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    ਵਾਪਸ hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ਲੱਦੋ_HWASID(asid);
    ਜੇ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ਵਾਪਸ pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } ਨਹੀਂਤਾਂ {
        hw_asid_t new_hw_asid;

        new_hw_asid = ਲੱਭੋ_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        ਵਾਪਸ new_hw_asid;
    }
}

ਸਥਿਰ ਖਾਲੀ invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ਲੱਦੋ_HWASID(asid);
    ਜੇ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

ਖਾਲੀ unmap_ਪੰਨਾ_ਸਾਰਣੀ(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pd_ਥਾਂ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pd_ਸੂਚਕਾਂਕ;

    pd = ਪੰਨਾ_ਸਾਰਣੀ_Mapped(asid, vaddr, pt);

    ਜੇ (likely(pd != ਨਿਰਾ)) {
        pd_ਸੂਚਕਾਂਕ = vaddr >> (PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ);
        pd_ਥਾਂ = pd + pd_ਸੂਚਕਾਂਕ;

        *pd_ਥਾਂ = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((ਸ਼ਬਦ_t)pd_ਥਾਂ, addrFromPPtr(pd_ਥਾਂ));
        flush_ਸਾਰਣੀ(pd, asid, vaddr, pt);
    }
}

ਖਾਲੀ copyGlobalMappings(pde_t *newPD)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸ਼ਬਦ_t i;
    pde_t *global_pd = armKSGlobalPD;

    ਲਈ (i = PPTR_ਅਧਾਰ >> ARMSection_ਬਿੱਟ; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        ਜੇ (i != PD_ASID_ਥਾਂ) {
            newPD[i] = global_pd[i];
        }
    }
#ਜੇ_ਅੰਤ
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    ਚੋਣ (vm_faultType) {
    ਹਾਲਤ ARMDataAbort: {
        ਸ਼ਬਦ_t addr, fault;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (ਪਤਾ_Translate_S1(addr) & ~ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ)) | (addr & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#ਨਹੀਂਤਾਂ
        addr = getFAR();
        fault = getDFSR();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
        /* Debug exceptions come in on the Prefetch and Data abort vectors.
         * We have to test the fault-status bits in the IFSR/DFSR to determine
         * if it's a debug exception when one occurs.
         *
         * If it is a debug exception, return early and don't fallthrough to the
         * normal VM Fault handling path.
         */
        ਜੇ (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(0);
            ਵਾਪਸ EXCEPTION_FAULT;
        }
#ਜੇ_ਅੰਤ
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        ਵਾਪਸ EXCEPTION_FAULT;
    }

    ਹਾਲਤ ARMPrefetchAbort: {
        ਸ਼ਬਦ_t pc, fault;

        pc = getRestartPC(thread);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (ਪਤਾ_Translate_S1(pc) & ~ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ)) | (pc & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#ਨਹੀਂਤਾਂ
        fault = getIFSR();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
        ਜੇ (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            ਜੇ (seL4_Fault_DebugException_get_exceptionReason(current_fault) == se_L4_Single_ਕਦਮ
                && !single_ਕਦਮ_Fault_Counter_Ready(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                ਵਾਪਸ EXCEPTION_NONE;
            }
            ਵਾਪਸ EXCEPTION_FAULT;
        }
#ਜੇ_ਅੰਤ
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        ਵਾਪਸ EXCEPTION_FAULT;
    }

    ਮੂਲ:
        fail("Invalid VM fault type");
    }
}

ਖਾਲੀ deleteASIDPool(asid_t asid_ਅਧਾਰ, asid_pool_t *pool)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਵਿੱਥ;

    /* Haskell error: "ASID pool's base must be aligned" */
    ਪ੍ਰਤਿਜ੍ਞਾ((asid_ਅਧਾਰ & ਨਕਾਬ(asid_ਨੀਵਾਂ_ਬਿੱਟ)) == 0);

    ਜੇ (armKSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] == pool) {
        ਲਈ (ਵਿੱਥ = 0; ਵਿੱਥ < ਦੋਆਧਾਰੀ_ਅੰਕ(asid_ਨੀਵਾਂ_ਬਿੱਟ); ਵਿੱਥ++) {
            ਜੇ (pool->array[ਵਿੱਥ]) {
                flushSpace(asid_ਅਧਾਰ + ਵਿੱਥ);
                invalidateASIDEntry(asid_ਅਧਾਰ + ਵਿੱਥ);
            }
        }
        armKSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] = ਨਿਰਾ;
        ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
    }
}

ਖਾਲੀ deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ਉੱਚਾ(asid)];

    ਜੇ (poolPtr != ਨਿਰਾ && poolPtr->array[ASID_ਨੀਵਾਂ(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_ਨੀਵਾਂ(asid)] = ਨਿਰਾ;
        ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
    }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
ਸਥਿਰ pte_t pte_pte_invalid_new(ਖਾਲੀ)
{
    /* Invalid as every PTE must have bit 0 set (large PTE) or bit 1 set (small
     * PTE). 0 == 'translation fault' in ARM parlance.
     */
    ਵਾਪਸ (pte_t) {
        {
            0
        }
    };
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ unmap_ਪੰਨਾ(vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, asid_t asid, vptr_t vptr, ਖਾਲੀ *pptr)
{
    ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret;
    paddr_t addr = addrFromPPtr(pptr);

    ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(asid);
    ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        ਵਾਪਸ;
    }

    ਚੋਣ (ਪੰਨਾ_ਮਾਪ) {
    ਹਾਲਤ ARMSmall_ਪੰਨਾ: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(ਲੱਭੋ_ret.pd, vptr);
        ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            ਵਾਪਸ;
        }

        ਜੇ (unlikely(pte_ptr_get_pteType(lu_ret.pt_ਥਾਂ) != pte_pte_small)) {
            ਵਾਪਸ;
        }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਜੇ (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_ਥਾਂ) != 0)) {
            ਵਾਪਸ;
        }
#ਜੇ_ਅੰਤ
        ਜੇ (unlikely(pte_pte_small_ptr_get_ਪਤਾ(lu_ret.pt_ਥਾਂ) != addr)) {
            ਵਾਪਸ;
        }

        *(lu_ret.pt_ਥਾਂ) = pte_pte_invalid_new();
        cleanByVA_PoU((ਸ਼ਬਦ_t)lu_ret.pt_ਥਾਂ, addrFromPPtr(lu_ret.pt_ਥਾਂ));

        ਰੋਕੋ;
    }

    ਹਾਲਤ ARMLarge_ਪੰਨਾ: {
        lookupPTSlot_ret_t lu_ret;
        ਸ਼ਬਦ_t i;

        lu_ret = lookupPTSlot(ਲੱਭੋ_ret.pd, vptr);
        ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            ਵਾਪਸ;
        }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਜੇ (unlikely(pte_ptr_get_pteType(lu_ret.pt_ਥਾਂ) != pte_pte_large)) {
            ਵਾਪਸ;
        }
        ਜੇ (unlikely(pte_pte_large_ptr_get_ਪਤਾ(lu_ret.pt_ਥਾਂ) != addr)) {
            ਵਾਪਸ;
        }
#ਨਹੀਂਤਾਂ
        ਜੇ (unlikely(pte_ptr_get_pteType(lu_ret.pt_ਥਾਂ) != pte_pte_small)) {
            ਵਾਪਸ;
        }
        ਜੇ (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_ਥਾਂ) != 1)) {
            ਵਾਪਸ;
        }
        ਜੇ (unlikely(pte_pte_small_ptr_get_ਪਤਾ(lu_ret.pt_ਥਾਂ) != addr)) {
            ਵਾਪਸ;
        }
#ਜੇ_ਅੰਤ

        ਲਈ (i = 0; i < ਪੰਨੇ_ਪ੍ਰਤੀ_LARGE_ਪੰਨਾ; i++) {
            lu_ret.pt_ਥਾਂ[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((ਸ਼ਬਦ_t)&lu_ret.pt_ਥਾਂ[0],
                            ਆਖਰੀ_BYTE_PTE(lu_ret.pt_ਥਾਂ, ਪੰਨੇ_ਪ੍ਰਤੀ_LARGE_ਪੰਨਾ),
                            addrFromPPtr(&lu_ret.pt_ਥਾਂ[0]));

        ਰੋਕੋ;
    }

    ਹਾਲਤ ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(ਲੱਭੋ_ret.pd, vptr);

        ਜੇ (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            ਵਾਪਸ;
        }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਜੇ (unlikely(pde_pde_section_ptr_get_ਮਾਪ(pd) != 0)) {
#ਨਹੀਂਤਾਂ
        ਜੇ (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#ਜੇ_ਅੰਤ
            ਵਾਪਸ;
        }
        ਜੇ (unlikely(pde_pde_section_ptr_get_ਪਤਾ(pd) != addr)) {
            ਵਾਪਸ;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((ਸ਼ਬਦ_t)pd, addrFromPPtr(pd));

        ਰੋਕੋ;
    }

    ਹਾਲਤ ARMSuperSection: {
        pde_t *pd;
        ਸ਼ਬਦ_t i;

        pd = lookupPDSlot(ਲੱਭੋ_ret.pd, vptr);

        ਜੇ (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            ਵਾਪਸ;
        }
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਜੇ (unlikely(pde_pde_section_ptr_get_ਮਾਪ(pd) != 1)) {
#ਨਹੀਂਤਾਂ
        ਜੇ (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#ਜੇ_ਅੰਤ
            ਵਾਪਸ;
        }
        ਜੇ (unlikely(pde_pde_section_ptr_get_ਪਤਾ(pd) != addr)) {
            ਵਾਪਸ;
        }

        ਲਈ (i = 0; i < SECTIONS_ਪ੍ਰਤੀ_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((ਸ਼ਬਦ_t)&pd[0], ਆਖਰੀ_BYTE_PDE(pd, SECTIONS_ਪ੍ਰਤੀ_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        ਰੋਕੋ;
    }

    ਮੂਲ:
        fail("Invalid ARM page type");
        ਰੋਕੋ;
    }

    /* Flush the page now that the mapping has been updated */
    flush_ਪੰਨਾ(ਪੰਨਾ_ਮਾਪ, ਲੱਭੋ_ret.pd, asid, vptr);
}

ਖਾਲੀ flush_ਪੰਨਾ(vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, pde_t *pd, asid_t asid, ਸ਼ਬਦ_t vptr)
{
    pde_t stored_hw_asid;
    ਸ਼ਬਦ_t ਅਧਾਰ_addr;
    bool_t ਜੜ੍ਹ_switched;

    ਪ੍ਰਤਿਜ੍ਞਾ((vptr & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਪੰਨਾ_ਮਾਪ))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ਜੜ੍ਹ_switched = ਸੈੱਟ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = ਲੱਦੋ_HWASID(asid);

    ਜੇ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ਅਧਾਰ_addr = vptr & ~ਨਕਾਬ(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(ਅਧਾਰ_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        ਜੇ (ਜੜ੍ਹ_switched) {
            ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
        }
    }
}

ਖਾਲੀ flush_ਸਾਰਣੀ(pde_t *pd, asid_t asid, ਸ਼ਬਦ_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t ਜੜ੍ਹ_switched;

    ਪ੍ਰਤਿਜ੍ਞਾ((vptr & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ARMSmall_ਪੰਨਾ_ਬਿੱਟ)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ਜੜ੍ਹ_switched = ਸੈੱਟ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = ਲੱਦੋ_HWASID(asid);

    ਜੇ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        ਜੇ (ਜੜ੍ਹ_switched) {
            ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
        }
    }
}

ਖਾਲੀ flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ਲੱਦੋ_HWASID(asid);

    /* Clean the entire data cache, to guarantee that any VAs mapped
     * in the deleted space are clean (because we can't clean by VA after
     * deleting the space) */
    cleanCaches_PoU();

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    ਜੇ (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ਵਾਪਸ;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ਖਾਲੀ invalidateTLBByASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ਲੱਦੋ_HWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    ਜੇ (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ਵਾਪਸ;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t CONST checkVPAlignment(vm_ਪੰਨਾ_ਮਾਪ_t sz, ਸ਼ਬਦ_t w)
{
    ਵਾਪਸ (w & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(sz))) == 0;
}

ਬਣਤਰ create_mappings_pte_return {
    exception_t ਸਥਿਤੀ;
    pte_t pte;
    pte_range_t pte_entries;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ create_mappings_pte_return create_mappings_pte_return_t;

ਬਣਤਰ create_mappings_pde_return {
    exception_t ਸਥਿਤੀ;
    pde_t pde;
    pde_range_t pde_entries;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ create_mappings_pde_return create_mappings_pde_return_t;

ਸਥਿਰ create_mappings_pte_return_t
create_ਸੁਰੱਖਿਅਤ_Mapping_Entries_PTE
(paddr_t ਅਧਾਰ, ਸ਼ਬਦ_t vaddr, vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ,
 vm_rights_t vmRights, vm_ਗੁਣ_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    ਸ਼ਬਦ_t i;

    ਚੋਣ (ਚੌਖਟਾ_ਮਾਪ) {

    ਹਾਲਤ ARMSmall_ਪੰਨਾ:

        ret.pte_entries.ਅਧਾਰ = ਨਿਰਾ; /* to avoid uninitialised warning */
        ret.pte_entries.ਲੰਮਾਈ = 1;

        ret.pte = makeUserPTE(ARMSmall_ਪੰਨਾ, ਅਧਾਰ,
                              vm_ਗੁਣ_get_arm_ਪੰਨਾ_Cacheable(attr),
                              vm_ਗੁਣ_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ =
                false;
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            ਵਾਪਸ ret;
        }

        ret.pte_entries.ਅਧਾਰ = lu_ret.pt_ਥਾਂ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਜੇ (unlikely(pte_ptr_get_pteType(ret.pte_entries.ਅਧਾਰ) ==
                     pte_pte_large)) {
#ਨਹੀਂਤਾਂ
        ਜੇ (unlikely(pte_ptr_get_pteType(ret.pte_entries.ਅਧਾਰ) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.ਅਧਾਰ))) {
#ਜੇ_ਅੰਤ
            current_syscall_error.type =
                se_L4_Delete_ਪਹਿਲਾ;

            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
            ਵਾਪਸ ret;
        }

        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

    ਹਾਲਤ ARMLarge_ਪੰਨਾ:

        ret.pte_entries.ਅਧਾਰ = ਨਿਰਾ; /* to avoid uninitialised warning */
        ret.pte_entries.ਲੰਮਾਈ = ਪੰਨੇ_ਪ੍ਰਤੀ_LARGE_ਪੰਨਾ;

        ret.pte = makeUserPTE(ARMLarge_ਪੰਨਾ, ਅਧਾਰ,
                              vm_ਗੁਣ_get_arm_ਪੰਨਾ_Cacheable(attr),
                              vm_ਗੁਣ_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ =
                false;
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            ਵਾਪਸ ret;
        }

        ret.pte_entries.ਅਧਾਰ = lu_ret.pt_ਥਾਂ;

        ਲਈ (i = 0; i < ਪੰਨੇ_ਪ੍ਰਤੀ_LARGE_ਪੰਨਾ; i++) {
            ਜੇ (unlikely(pte_get_pteType(ret.pte_entries.ਅਧਾਰ[i]) ==
                         pte_pte_small)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.ਅਧਾਰ[i])
#ਜੇ_ਅੰਤ
               ) {
                current_syscall_error.type =
                    se_L4_Delete_ਪਹਿਲਾ;

                ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
                ਵਾਪਸ ret;
            }
        }

        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

    ਮੂਲ:
        fail("Invalid or unexpected ARM page type.");

    }
}

ਸਥਿਰ create_mappings_pde_return_t
create_ਸੁਰੱਖਿਅਤ_Mapping_Entries_PDE
(paddr_t ਅਧਾਰ, ਸ਼ਬਦ_t vaddr, vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ,
 vm_rights_t vmRights, vm_ਗੁਣ_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_ਟੈਗ_t currentPDEType;
    ਸ਼ਬਦ_t i;

    ਚੋਣ (ਚੌਖਟਾ_ਮਾਪ) {

    /* PDE mappings */
    ਹਾਲਤ ARMSection:
        ret.pde_entries.ਅਧਾਰ = lookupPDSlot(pd, vaddr);
        ret.pde_entries.ਲੰਮਾਈ = 1;

        ret.pde = makeUserPDE(ARMSection, ਅਧਾਰ,
                              vm_ਗੁਣ_get_arm_Parity_Enabled(attr),
                              vm_ਗੁਣ_get_arm_ਪੰਨਾ_Cacheable(attr),
                              vm_ਗੁਣ_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.ਅਧਾਰ);
        ਜੇ (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_ਮਾਪ(ret.pde_entries.ਅਧਾਰ) != 0))) {
#ਨਹੀਂਤਾਂ
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.ਅਧਾਰ) != 0))) {
#ਜੇ_ਅੰਤ
            current_syscall_error.type =
                se_L4_Delete_ਪਹਿਲਾ;
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;

            ਵਾਪਸ ret;
        }

        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

    ਹਾਲਤ ARMSuperSection:
        ret.pde_entries.ਅਧਾਰ = lookupPDSlot(pd, vaddr);
        ret.pde_entries.ਲੰਮਾਈ = SECTIONS_ਪ੍ਰਤੀ_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, ਅਧਾਰ,
                              vm_ਗੁਣ_get_arm_Parity_Enabled(attr),
                              vm_ਗੁਣ_get_arm_ਪੰਨਾ_Cacheable(attr),
                              vm_ਗੁਣ_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        ਲਈ (i = 0; i < SECTIONS_ਪ੍ਰਤੀ_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.ਅਧਾਰ[i]);
            ਜੇ (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_ਮਾਪ(ret.pde_entries.ਅਧਾਰ[i]) != 1))) {
#ਨਹੀਂਤਾਂ
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.ਅਧਾਰ[i]) != 1))) {
#ਜੇ_ਅੰਤ
                current_syscall_error.type =
                    se_L4_Delete_ਪਹਿਲਾ;
                ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;

                ਵਾਪਸ ret;
            }
        }

        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

    ਮੂਲ:
        fail("Invalid or unexpected ARM page type.");

    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ vptr_t ਪੰਨਾ_ਅਧਾਰ(vptr_t vaddr, vm_ਪੰਨਾ_ਮਾਪ_t ਮਾਪ)
{
    ਵਾਪਸ vaddr & ~ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਮਾਪ));
}

ਸਥਿਰ bool_t ਸ਼ੁੱਧ pteCheckIfMapped(pte_t *pte)
{
    ਵਾਪਸ pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

ਸਥਿਰ bool_t ਸ਼ੁੱਧ pdeCheckIfMapped(pde_t *pde)
{
    ਵਾਪਸ pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

ਸਥਿਰ ਖਾਲੀ doFlush(ਪੂਰਨ invLabel, vptr_t ਸ਼ੁਰੂ, vptr_t ਸਿਰਾ, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        ਸਿਰਾ = (vptr_t)paddr_ਮੰਜ਼ਿਲ_pptr(pstart) + (ਸਿਰਾ - ਸ਼ੁਰੂ);
        ਸ਼ੁਰੂ = (vptr_t)paddr_ਮੰਜ਼ਿਲ_pptr(pstart);
    }
    ਚੋਣ (invLabel) {
    ਹਾਲਤ ARMPDClean_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPage_Clean_ਅੰਕੜੇ:
        cleanCacheRange_RAM(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        ਰੋਕੋ;
    ਹਾਲਤ ARMPDInvalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPage_Invalidate_ਅੰਕੜੇ:
        invalidateCacheRange_RAM(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        ਰੋਕੋ;
    ਹਾਲਤ ARMPDClean_Invalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPage_Clean_Invalidate_ਅੰਕੜੇ:
        cleanInvalidateCacheRange_RAM(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        ਰੋਕੋ;
    ਹਾਲਤ ARMPDUnify_Instruction:
    ਹਾਲਤ ARMPageUnify_Instruction:
        /* First clean data lines to point of unification
           (L2 cache)... */
        cleanCacheRange_PoU(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        /* ...then invalidate branch predictors... */
        branchFlushRange(ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);
        /* ... then wait for the completion of invalidations. */
        dsb();
        /* Ensure new instructions come from fresh cache lines. */
        isb();
        ਰੋਕੋ;
    ਮੂਲ:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

ਸਥਿਰ exception_t performPDFlush(ਪੂਰਨ invLabel, pde_t *pd, asid_t asid, vptr_t ਸ਼ੁਰੂ,
                                  vptr_t ਸਿਰਾ, paddr_t pstart)
{
    bool_t ਜੜ੍ਹ_switched;

    /* Flush if given a non zero range */
    ਜੇ (ਸ਼ੁਰੂ < ਸਿਰਾ) {
        ਜੜ੍ਹ_switched = ਸੈੱਟ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);

        ਜੇ (ਜੜ੍ਹ_switched) {
            ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
        }
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Map(cap_t cap, cte_t *ct_ਥਾਂ,
                                                 pde_t pde, pde_t *pd_ਥਾਂ)
{
    ct_ਥਾਂ->cap = cap;
    *pd_ਥਾਂ = pde;
    cleanByVA_PoU((ਸ਼ਬਦ_t)pd_ਥਾਂ, addrFromPPtr(pd_ਥਾਂ));

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Unmap(cap_t cap, cte_t *ct_ਥਾਂ)
{
    ਜੇ (cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap));
        unmap_ਪੰਨਾ_ਸਾਰਣੀ(
            cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap),
            cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap),
            pt);
        clear_ਸਮ੍ਰਿਤੀ_PT((ਖਾਲੀ *)pt, cap_get_cap_ਮਾਪ_ਬਿੱਟ(cap));
    }
    cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ptr_ਸੈੱਟ_cap_PTIs_Mapped(&(ct_ਥਾਂ->cap), 0);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Invocation_Map_PTE(asid_t asid, cap_t cap, cte_t *ct_ਥਾਂ, pte_t pte,
                                               pte_range_t pte_entries)
{
    ਸ਼ਬਦ_t i, j ਅਣਵਰਤਿਆ;
    bool_t tlbflush_required;

    ct_ਥਾਂ->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.ਅਧਾਰ);

    j = pte_entries.ਲੰਮਾਈ;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸ਼ਬਦ_t ਅਧਾਰ_ਪਤਾ = pte_pte_small_get_ਪਤਾ(pte);
#ਜੇ_ਅੰਤ
    ਲਈ (i = 0; i < pte_entries.ਲੰਮਾਈ; i++) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_ਸੈੱਟ_ਪਤਾ(pte, ਅਧਾਰ_ਪਤਾ + i * ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSmall_ਪੰਨਾ)));
#ਜੇ_ਅੰਤ
        pte_entries.ਅਧਾਰ[i] = pte;
    }
    cleanCacheRange_PoU((ਸ਼ਬਦ_t)pte_entries.ਅਧਾਰ,
                        ਆਖਰੀ_BYTE_PTE(pte_entries.ਅਧਾਰ, pte_entries.ਲੰਮਾਈ),
                        addrFromPPtr(pte_entries.ਅਧਾਰ));
    ਜੇ (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Invocation_Map_PDE(asid_t asid, cap_t cap, cte_t *ct_ਥਾਂ, pde_t pde,
                                               pde_range_t pde_entries)
{
    ਸ਼ਬਦ_t i, j ਅਣਵਰਤਿਆ;
    bool_t tlbflush_required;

    ct_ਥਾਂ->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.ਅਧਾਰ);

    j = pde_entries.ਲੰਮਾਈ;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸ਼ਬਦ_t ਅਧਾਰ_ਪਤਾ = pde_pde_section_get_ਪਤਾ(pde);
#ਜੇ_ਅੰਤ
    ਲਈ (i = 0; i < pde_entries.ਲੰਮਾਈ; i++) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_ਸੈੱਟ_ਪਤਾ(pde, ਅਧਾਰ_ਪਤਾ + i * ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection)));
#ਜੇ_ਅੰਤ
        pde_entries.ਅਧਾਰ[i] = pde;
    }
    cleanCacheRange_PoU((ਸ਼ਬਦ_t)pde_entries.ਅਧਾਰ,
                        ਆਖਰੀ_BYTE_PDE(pde_entries.ਅਧਾਰ, pde_entries.ਲੰਮਾਈ),
                        addrFromPPtr(pde_entries.ਅਧਾਰ));
    ਜੇ (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Invocation_Unmap(cap_t cap, cte_t *ct_ਥਾਂ)
{
    ਜੇ (generic_ਚੌਖਟਾ_cap_get_cap_FIs_Mapped(cap)) {
        unmap_ਪੰਨਾ(generic_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
                  generic_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                  generic_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                  (ਖਾਲੀ *)generic_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));
    }

    generic_ਚੌਖਟਾ_cap_ptr_ਸੈੱਟ_cap_FMapped_ਪਤਾ(&ct_ਥਾਂ->cap, asidInvalid, 0);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Flush(ਪੂਰਨ invLabel, pde_t *pd, asid_t asid, vptr_t ਸ਼ੁਰੂ,
                                    vptr_t ਸਿਰਾ, paddr_t pstart)
{
    bool_t ਜੜ੍ਹ_switched;

    /* now we can flush. But only if we were given a non zero range */
    ਜੇ (ਸ਼ੁਰੂ < ਸਿਰਾ) {
        ਜੜ੍ਹ_switched = ਸੈੱਟ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, ਸ਼ੁਰੂ, ਸਿਰਾ, pstart);

        ਜੇ (ਜੜ੍ਹ_switched) {
            ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
        }
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Get_ਪਤਾ(ਖਾਲੀ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);
    ਜੇ (call) {
        ਸ਼ਬਦ_t *ipc_ਬਫਰ = lookupIPCBuffer(true, thread);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ = ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, 0, capFBasePtr);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pd_Cap_ਥਾਂ)
{
    cap_ਪੰਨਾ_directory_cap_ptr_ਸੈੱਟ_cap_PDMapped_ASID(&pd_Cap_ਥਾਂ->cap, asid);
    cap_ਪੰਨਾ_directory_cap_ptr_ਸੈੱਟ_cap_PDIs_Mapped(&pd_Cap_ਥਾਂ->cap, 1);
    poolPtr->array[ASID_ਨੀਵਾਂ(asid)] =
        PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(pd_Cap_ਥਾਂ->cap));

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t performASIDControlInvocation(ਖਾਲੀ *ਚੌਖਟਾ, cte_t *ਥਾਂ,
                                                cte_t *ਮਾਪਾ, asid_t asid_ਅਧਾਰ)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_ਸੈੱਟ_cap_Free_ਸੂਚਕਾਂਕ(&(ਮਾਪਾ->cap),
                                         MAX_FREE_ਸੂਚਕਾਂਕ(cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(ਮਾਪਾ->cap)));

    memzero(ਚੌਖਟਾ, 1 << ARMSmall_ਪੰਨਾ_ਬਿੱਟ);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ਪਾਓ(cap_asid_pool_cap_new(asid_ਅਧਾਰ, ਸ਼ਬਦ_REF(ਚੌਖਟਾ)),
              ਮਾਪਾ, ਥਾਂ);;
    /* Haskell error: "ASID pool's base must be aligned" */
    ਪ੍ਰਤਿਜ੍ਞਾ((asid_ਅਧਾਰ & ਨਕਾਬ(asid_ਨੀਵਾਂ_ਬਿੱਟ)) == 0);
    armKSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] = (asid_pool_t *)ਚੌਖਟਾ;

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeARMPageDirectoryInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਚੋਣ (invLabel) {
    ਹਾਲਤ ARMPDClean_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPDInvalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPDClean_Invalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPDUnify_Instruction: {
        vptr_t ਸ਼ੁਰੂ, ਸਿਰਾ;
        paddr_t pstart;
        ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        ਜੇ (ਲੰਮਾਈ < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸ਼ੁਰੂ = getSyscallArg(0, ਬਫਰ);
        ਸਿਰਾ =   getSyscallArg(1, ਬਫਰ);

        /* Check sanity of arguments */
        ਜੇ (ਸਿਰਾ <= ਸ਼ੁਰੂ) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        ਜੇ (ਸ਼ੁਰੂ >= USER_ਸਿਖਰ || ਸਿਰਾ > USER_ਸਿਖਰ) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(cap_get_capType(cap) != cap_ਪੰਨਾ_directory_cap ||
                     !cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(cap));
        asid = cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(cap);

        ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(asid);
        ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(ਲੱਭੋ_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, ਸ਼ੁਰੂ);

        /* Check that there's actually something there. */
        ਜੇ (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
            ਵਾਪਸ EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        ਜੇ (ਪੰਨਾ_ਅਧਾਰ(ਸ਼ੁਰੂ, resolve_ret.ਚੌਖਟਾ_ਮਾਪ) !=
            ਪੰਨਾ_ਅਧਾਰ(ਸਿਰਾ - 1, resolve_ret.ਚੌਖਟਾ_ਮਾਪ)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = ਸ਼ੁਰੂ;
            current_syscall_error.rangeErrorMax =
                ਪੰਨਾ_ਅਧਾਰ(ਸ਼ੁਰੂ, resolve_ret.ਚੌਖਟਾ_ਮਾਪ) +
                ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(resolve_ret.ਚੌਖਟਾ_ਮਾਪ));
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.ਚੌਖਟਾ_ਅਧਾਰ
                 + (ਸ਼ੁਰੂ & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(resolve_ret.ਚੌਖਟਾ_ਮਾਪ)));

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        ਜੇ (pstart < phys_ਅਧਾਰ() || pstart > PADDR_ਸਿਖਰ || (ਸਿਰਾ - ਸ਼ੁਰੂ) > PADDR_ਸਿਖਰ - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਜੇ_ਅੰਤ

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performPDFlush(invLabel, pd, asid, ਸ਼ੁਰੂ, ਸਿਰਾ - 1, pstart);
    }

    ਮੂਲ:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

}

ਸਥਿਰ exception_t decode_ARMPage_ਸਾਰਣੀ_Invocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                                cte_t *cte, cap_t cap, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t vaddr, pd_ਸੂਚਕਾਂਕ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_ਗੁਣ_t attr;
#ਜੇ_ਅੰਤ
    cap_t pdCap;
    pde_t *pd, *pd_ਥਾਂ;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    ਜੇ (invLabel == ARMPage_ਸਾਰਣੀ_Unmap) {
        ਜੇ (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Unmap(cap, cte);
    }

    ਜੇ (unlikely(invLabel != ARMPage_ਸਾਰਣੀ_Map)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ਬਫਰ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vm_ਗੁਣ_From_ਸ਼ਬਦ(getSyscallArg(1, ਬਫਰ));
#ਜੇ_ਅੰਤ
    pdCap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (unlikely(cap_get_capType(pdCap) != cap_ਪੰਨਾ_directory_cap ||
                 !cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(pdCap));
    asid = cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(pdCap);

    ਜੇ (unlikely(vaddr >= USER_ਸਿਖਰ)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_ਸਿਖਰ);
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    {
        ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret;

        ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(asid);
        ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(ਲੱਭੋ_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_ਸੂਚਕਾਂਕ = vaddr >> (ਪੰਨਾ_ਬਿੱਟ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ);
    pd_ਥਾਂ = &pd[pd_ਸੂਚਕਾਂਕ];
    ਜੇ (unlikely(pde_ptr_get_pdeType(pd_ਥਾਂ) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap)));
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_ਗੁਣ_get_arm_Parity_Enabled(attr),
              0 /* Domain */
          );
#ਨਹੀਂਤਾਂ
    pde = pde_pde_coarse_new(paddr);
#ਜੇ_ਅੰਤ

    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTMapped_ਪਤਾ(cap, vaddr);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Map(cap, cte, pde, pd_ਥਾਂ);
}

ਸਥਿਰ exception_t decodeARMFrameInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                            cte_t *cte, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਚੋਣ (invLabel) {
    ਹਾਲਤ ARMPageMap: {
        ਸ਼ਬਦ_t vaddr, vtop, w_rights_ਨਕਾਬ;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ;
        vm_ਗੁਣ_t attr;

        ਜੇ (unlikely(ਲੰਮਾਈ < 3 || current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                se_L4_Truncated_ਸੁਨੇਹਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, ਬਫਰ);
        w_rights_ਨਕਾਬ = getSyscallArg(1, ਬਫਰ);
        attr = vm_ਗੁਣ_From_ਸ਼ਬਦ(getSyscallArg(2, ਬਫਰ));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        ਚੌਖਟਾ_ਮਾਪ = generic_ਚੌਖਟਾ_cap_get_cap_FSize(cap);
        capVMRights = generic_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap);

        ਜੇ (unlikely(cap_get_capType(pdCap) != cap_ਪੰਨਾ_directory_cap ||
                     !cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(
                         pdCap));
        asid = cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(pdCap);

        ਜੇ (generic_ਚੌਖਟਾ_cap_get_cap_FIs_Mapped(cap)) {
            ਜੇ (generic_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }

            ਜੇ (generic_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
                current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        } ਨਹੀਂਤਾਂ {
            vtop = vaddr + ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਚੌਖਟਾ_ਮਾਪ)) - 1;

            ਜੇ (unlikely(vtop >= USER_ਸਿਖਰ)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_ਸਿਖਰ);
                current_syscall_error.type =
                    se_L4_Invalid_ਦਲੀਲ;
                current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            ਲੱਭੋ_PDFor_ASID_ret_t ਲੱਭੋ_ret;

            ਲੱਭੋ_ret = ਲੱਭੋ_PDFor_ASID(asid);
            ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_ਸਰੋਤ =
                    false;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }

            ਜੇ (unlikely(ਲੱਭੋ_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            ਨਕਾਬ_VMRights(capVMRights, rights_From_ਸ਼ਬਦ(w_rights_ਨਕਾਬ));

        ਜੇ (unlikely(!checkVPAlignment(ਚੌਖਟਾ_ਮਾਪ, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((ਖਾਲੀ *)
                                   generic_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));

        cap = generic_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(cap, asid,
                                                      vaddr);
        ਜੇ (ਚੌਖਟਾ_ਮਾਪ == ARMSmall_ਪੰਨਾ || ਚੌਖਟਾ_ਮਾਪ == ARMLarge_ਪੰਨਾ) {
            create_mappings_pte_return_t map_ret;
            map_ret = create_ਸੁਰੱਖਿਅਤ_Mapping_Entries_PTE(capFBasePtr, vaddr,
                                                   ਚੌਖਟਾ_ਮਾਪ, vmRights,
                                                   attr, pd);
            ਜੇ (unlikely(map_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
                ਜੇ (current_syscall_error.type == se_L4_Delete_ਪਹਿਲਾ) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#ਜੇ_ਅੰਤ
                ਵਾਪਸ map_ret.ਸਥਿਤੀ;
            }

            ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
            ਵਾਪਸ perform_ਪੰਨਾ_Invocation_Map_PTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } ਨਹੀਂਤਾਂ {
            create_mappings_pde_return_t map_ret;
            map_ret = create_ਸੁਰੱਖਿਅਤ_Mapping_Entries_PDE(capFBasePtr, vaddr,
                                                   ਚੌਖਟਾ_ਮਾਪ, vmRights,
                                                   attr, pd);
            ਜੇ (unlikely(map_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
                ਜੇ (current_syscall_error.type == se_L4_Delete_ਪਹਿਲਾ) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#ਜੇ_ਅੰਤ
                ਵਾਪਸ map_ret.ਸਥਿਤੀ;
            }

            ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
            ਵਾਪਸ perform_ਪੰਨਾ_Invocation_Map_PDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    ਹਾਲਤ ARMPageUnmap: {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
        ਜੇ (is_IOSpace_ਚੌਖਟਾ_Cap(cap)) {
            ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
            ਵਾਪਸ perform_ਪੰਨਾ_Invocation_Unmap_IO(cap, cte);
        } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
        {
            ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
            ਵਾਪਸ perform_ਪੰਨਾ_Invocation_Unmap(cap, cte);
        }
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ ARMPageMapIO: {
        ਵਾਪਸ decodeARMIOMapInvocation(invLabel, ਲੰਮਾਈ, cte, cap, ਬਫਰ);
    }
#ਜੇ_ਅੰਤ

    ਹਾਲਤ ARMPage_Clean_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPage_Invalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPage_Clean_Invalidate_ਅੰਕੜੇ:
    ਹਾਲਤ ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        ਲੱਭੋ_PDFor_ASID_ret_t pd;
        vptr_t ਸ਼ੁਰੂ, ਸਿਰਾ;
        paddr_t pstart;
        ਸ਼ਬਦ_t ਪੰਨਾ_ਮਾਪ;
        ਸ਼ਬਦ_t ਪੰਨਾ_ਅਧਾਰ_2;

        ਜੇ (ਲੰਮਾਈ < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap);
#ਨਹੀਂਤਾਂ
        vaddr = generic_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap);
#ਜੇ_ਅੰਤ

        ਜੇ (unlikely(!generic_ਚੌਖਟਾ_cap_get_cap_FIs_Mapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        pd = ਲੱਭੋ_PDFor_ASID(asid);
        ਜੇ (unlikely(pd.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸ਼ੁਰੂ = getSyscallArg(0, ਬਫਰ);
        ਸਿਰਾ =   getSyscallArg(1, ਬਫਰ);

        /* check that the range is sane */
        ਜੇ (ਸਿਰਾ <= ਸ਼ੁਰੂ) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        ਪੰਨਾ_ਮਾਪ = 1 << ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(generic_ਚੌਖਟਾ_cap_get_cap_FSize(cap));
        ਪੰਨਾ_ਅਧਾਰ_2 = addrFromPPtr((ਖਾਲੀ *)generic_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        ਜੇ (unlikely(!resolve_ret.valid ||
                     resolve_ret.ਚੌਖਟਾ_ਮਾਪ != generic_ਚੌਖਟਾ_cap_get_cap_FSize(cap) ||
                     resolve_ret.ਚੌਖਟਾ_ਅਧਾਰ != ਪੰਨਾ_ਅਧਾਰ_2)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਜੇ_ਅੰਤ

        ਜੇ (ਸ਼ੁਰੂ >= ਪੰਨਾ_ਮਾਪ || ਸਿਰਾ > ਪੰਨਾ_ਮਾਪ) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = ਪੰਨਾ_ਅਧਾਰ_2 + ਸ਼ੁਰੂ;
        ਸ਼ੁਰੂ += vaddr;
        ਸਿਰਾ += vaddr;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        ਜੇ (pstart < phys_ਅਧਾਰ() || pstart > PADDR_ਸਿਖਰ || (ਸਿਰਾ - ਸ਼ੁਰੂ) > PADDR_ਸਿਖਰ - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
#ਜੇ_ਅੰਤ

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_Flush(invLabel, pd.pd, asid, ਸ਼ੁਰੂ, ਸਿਰਾ - 1, pstart);
    }

    ਹਾਲਤ ARMPage_Get_ਪਤਾ: {


        /* Check that there are enough message registers */
        ਪ੍ਰਤਿਜ੍ਞਾ(n_ਸੁਨੇਹਾ_Registers >= 1);

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_Get_ਪਤਾ((ਖਾਲੀ *)generic_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap), call);
    }

    ਮੂਲ:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
        ਵਾਪਸ decodeARMPageDirectoryInvocation(invLabel, ਲੰਮਾਈ, cptr, cte,
                                                cap, ਬਫਰ);

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਵਾਪਸ decode_ARMPage_ਸਾਰਣੀ_Invocation(invLabel, ਲੰਮਾਈ, cte, cap, ਬਫਰ);

    ਹਾਲਤ cap_small_ਚੌਖਟਾ_cap:
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਵਾਪਸ decodeARMFrameInvocation(invLabel, ਲੰਮਾਈ, cte, cap, call, ਬਫਰ);

    ਹਾਲਤ cap_asid_control_cap: {
        ਸ਼ਬਦ_t i;
        asid_t asid_ਅਧਾਰ;
        ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ;
        cap_t untyped, ਜੜ੍ਹ;
        cte_t *ਮਾਪਾ_ਥਾਂ, *dest_ਥਾਂ;
        lookup_ਥਾਂ_ret_t lu_ret;
        ਖਾਲੀ *ਚੌਖਟਾ;
        exception_t ਸਥਿਤੀ;

        ਜੇ (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ
                     || current_extra_caps.excaprefs[1] == ਨਿਰਾ)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੂਚਕਾਂਕ = getSyscallArg(0, ਬਫਰ);
        ਡੂੰਘਾਈ = getSyscallArg(1, ਬਫਰ);
        ਮਾਪਾ_ਥਾਂ = current_extra_caps.excaprefs[0];
        untyped = ਮਾਪਾ_ਥਾਂ->cap;
        ਜੜ੍ਹ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        ਲਈ (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        ਜੇ (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        asid_ਅਧਾਰ = i << asid_ਨੀਵਾਂ_ਬਿੱਟ;

        ਜੇ (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(untyped) !=
                     se_L4_ASIDPool_ਬਿੱਟ || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸਥਿਤੀ = ensureNoChildren(ਮਾਪਾ_ਥਾਂ);
        ਜੇ (unlikely(ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਚੌਖਟਾ = ਸ਼ਬਦ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(ਜੜ੍ਹ, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
        ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }
        dest_ਥਾਂ = lu_ret.ਥਾਂ;

        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (unlikely(ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performASIDControlInvocation(ਚੌਖਟਾ, dest_ਥਾਂ,
                                            ਮਾਪਾ_ਥਾਂ, asid_ਅਧਾਰ);
    }

    ਹਾਲਤ cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pd_Cap_ਥਾਂ;
        asid_pool_t *pool;
        ਸ਼ਬਦ_t i;
        asid_t asid;

        ਜੇ (unlikely(invLabel != ARMASIDPool_ਸੌਂਪੋ)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        pd_Cap_ਥਾਂ = current_extra_caps.excaprefs[0];
        pdCap = pd_Cap_ਥਾਂ->cap;

        ਜੇ (unlikely(
                cap_get_capType(pdCap) != cap_ਪੰਨਾ_directory_cap ||
                cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ਉੱਚਾ(cap_asid_pool_cap_get_capASIDBase(cap))];
        ਜੇ (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        ਲਈ (i = 0; i < (1 << asid_ਨੀਵਾਂ_ਬਿੱਟ) && (asid + i == 0 || pool->array[i]); i++);

        ਜੇ (unlikely(i == 1 << asid_ਨੀਵਾਂ_ਬਿੱਟ)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performASIDPoolInvocation(asid, pool, pd_Cap_ਥਾਂ);
    }

    ਮੂਲ:
        fail("Invalid ARM arch cap type");
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
exception_t benchmark_arch_map_log_ਬਫਰ(ਸ਼ਬਦ_t ਚੌਖਟਾ_cptr)
{
    lookup_Cap_And_ਥਾਂ_ret_t lu_ret;
    vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ;
    pptr_t  ਚੌਖਟਾ_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_ਥਾਂ(ਗੰਢ_STATE(ksCurThread), ਚੌਖਟਾ_cptr);

    ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", ਚੌਖਟਾ_cptr);
        current_fault = seL4_Fault_CapFault_new(ਚੌਖਟਾ_cptr, false);

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_get_capType(lu_ret.cap) != cap_ਚੌਖਟਾ_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(ਚੌਖਟਾ_cptr, false);

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਚੌਖਟਾ_ਮਾਪ = generic_ਚੌਖਟਾ_cap_get_cap_FSize(lu_ret.cap);

    ਜੇ (ਚੌਖਟਾ_ਮਾਪ != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(ਚੌਖਟਾ_cptr, false);

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਚੌਖਟਾ_pptr = generic_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_ਬਫਰ = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *) ਚੌਖਟਾ_pptr);

    ਲਈ (ਪੂਰਨ idx = 0; idx < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); idx++) {
        paddr_t physical_ਪਤਾ = ks_User_Log_ਬਫਰ + (idx << se_L4_ਪੰਨਾ_ਬਿੱਟ);

        armKSGlobalLogPT[idx] =
            pte_pte_small_new(
                physical_ਪਤਾ,
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
        invalidateTranslationSingle(KS_LOG_PPTR + (idx * ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਬਿੱਟ)));
    }

    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
ਖਾਲੀ kernelPrefetchAbort(ਸ਼ਬਦ_t pc, ਸ਼ਬਦ_t lr) VISIBLE;
ਖਾਲੀ kernel_ਅੰਕੜੇ_Abort(ਸ਼ਬਦ_t pc) VISIBLE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT

ਖਾਲੀ kernelUndefinedInstruction(ਸ਼ਬਦ_t pc) VISIBLE;

ਖਾਲੀ kernelPrefetchAbort(ਸ਼ਬਦ_t pc, ਸ਼ਬਦ_t lr)
{
    ਲਿਖੋ("\n\nKERNEL PREFETCH ABORT!\n");
    ਲਿਖੋ("Faulting instruction: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", pc);
    ਲਿਖੋ("HSR: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", getHSR());
    ਲਿਖੋ("LR: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", lr);
    halt();
}

ਖਾਲੀ kernel_ਅੰਕੜੇ_Abort(ਸ਼ਬਦ_t pc)
{
    ਲਿਖੋ("\n\nKERNEL DATA ABORT!\n");
    ਲਿਖੋ("Faulting instruction: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", pc);
    ਲਿਖੋ("HDFAR: 0x%"SEL4_PRIx_ਸ਼ਬਦ" HSR: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n",
           getHDFAR(), getHSR());
    halt();
}

ਖਾਲੀ kernelUndefinedInstruction(ਸ਼ਬਦ_t pc)
{
    ਲਿਖੋ("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    ਲਿਖੋ("Faulting instruction: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", pc);
    ਲਿਖੋ("HSR: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", getHSR());
    halt();
}

#ਨਹੀਂਤਾਂ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ਖਾਲੀ kernelPrefetchAbort(ਸ਼ਬਦ_t pc, ਸ਼ਬਦ_t lr)
{
    ਲਿਖੋ("\n\nKERNEL PREFETCH ABORT!\n");
    ਲਿਖੋ("Faulting instruction: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", pc);
    ਲਿਖੋ("IFSR: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", getIFSR());
    ਲਿਖੋ("LR: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", lr);
    halt();
}

ਖਾਲੀ kernel_ਅੰਕੜੇ_Abort(ਸ਼ਬਦ_t pc)
{
    ਲਿਖੋ("\n\nKERNEL DATA ABORT!\n");
    ਲਿਖੋ("Faulting instruction: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", pc);
    ਲਿਖੋ("FAR: 0x%"SEL4_PRIx_ਸ਼ਬਦ" DFSR: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n",
           getFAR(), getDFSR());
    halt();
}
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ read_ਸ਼ਬਦ_From_VSpace_ret {
    exception_t ਸਥਿਤੀ;
    ਸ਼ਬਦ_t ਮੁੱਲ;
} read_ਸ਼ਬਦ_From_VSpace_ret_t;

ਸਥਿਰ read_ਸ਼ਬਦ_From_VSpace_ret_t read_ਸ਼ਬਦ_From_VSpace(pde_t *pd, ਸ਼ਬਦ_t vaddr)
{
    read_ਸ਼ਬਦ_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_ਥਾਂ;
    pde_t *pd_ਥਾਂ;
    paddr_t paddr;
    ਸ਼ਬਦ_t ਵਿੱਥ;
    pptr_t kernel_vaddr;
    ਸ਼ਬਦ_t *ਮੁੱਲ;

    pd_ਥਾਂ = lookupPDSlot(pd, vaddr);
    ਜੇ (pde_ptr_get_pdeType(pd_ਥਾਂ) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_ਪਤਾ(pd_ਥਾਂ);
        ਵਿੱਥ = vaddr & ਨਕਾਬ(ARMSection_ਬਿੱਟ);
    } ਨਹੀਂਤਾਂ {
        pt_ਥਾਂ = lookupPTSlot(pd, vaddr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        ਜੇ (pt_ਥਾਂ.ਸਥਿਤੀ == EXCEPTION_NONE && pte_ptr_get_pteType(pt_ਥਾਂ.pt_ਥਾਂ) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_ਪਤਾ(pt_ਥਾਂ.pt_ਥਾਂ);
            ਜੇ (pte_pte_small_ptr_get_contiguous_hint(pt_ਥਾਂ.pt_ਥਾਂ)) {
                ਵਿੱਥ = vaddr & ਨਕਾਬ(ARMLarge_ਪੰਨਾ_ਬਿੱਟ);
            } ਨਹੀਂਤਾਂ {
                ਵਿੱਥ = vaddr & ਨਕਾਬ(ARMSmall_ਪੰਨਾ_ਬਿੱਟ);
            }
#ਨਹੀਂਤਾਂ
        ਜੇ (pt_ਥਾਂ.ਸਥਿਤੀ == EXCEPTION_NONE && pte_ptr_get_pteType(pt_ਥਾਂ.pt_ਥਾਂ) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_ਪਤਾ(pt_ਥਾਂ.pt_ਥਾਂ);
            ਵਿੱਥ = vaddr & ਨਕਾਬ(ARMSmall_ਪੰਨਾ_ਬਿੱਟ);
        } ਨਹੀਂਤਾਂ ਜੇ (pt_ਥਾਂ.ਸਥਿਤੀ == EXCEPTION_NONE && pte_ptr_get_pteType(pt_ਥਾਂ.pt_ਥਾਂ) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_ਪਤਾ(pt_ਥਾਂ.pt_ਥਾਂ);
            ਵਿੱਥ = vaddr & ਨਕਾਬ(ARMLarge_ਪੰਨਾ_ਬਿੱਟ);
#ਜੇ_ਅੰਤ
        } ਨਹੀਂਤਾਂ {
            ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
            ਵਾਪਸ ret;
        }
    }


    kernel_vaddr = (ਸ਼ਬਦ_t)paddr_ਮੰਜ਼ਿਲ_pptr(paddr);
    ਮੁੱਲ = (ਸ਼ਬਦ_t *)(kernel_vaddr + ਵਿੱਥ);
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ret.ਮੁੱਲ = *ਮੁੱਲ;
    ਵਾਪਸ ret;
}

ਖਾਲੀ Arch_user_ਢੇਰ_Trace(tcb_t *tptr)
{
    cap_t thread_ਜੜ੍ਹ;
    pde_t *pd;
    ਸ਼ਬਦ_t sp;
    ਪੂਰਨ i;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    ਜੇ (cap_get_capType(thread_ਜੜ੍ਹ) != cap_ਪੰਨਾ_directory_cap) {
        ਲਿਖੋ("Invalid vspace\n");
        ਵਾਪਸ;
    }

    pd = (pde_t *)pptr_ਦਾ_cap(thread_ਜੜ੍ਹ);

    sp = get_ਰਜਿਸਟਰ(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ਜੇ (!IS_ALIGNED(sp, se_L4_ਸ਼ਬਦ_ਮਾਪ_ਬਿੱਟ)) {
        ਲਿਖੋ("SP not aligned\n");
        ਵਾਪਸ;
    }

    ਲਈ (i = 0; i < CONFIG_USER_ਢੇਰ_TRACE_ਲੰਮਾਈ; i++) {
        ਸ਼ਬਦ_t ਪਤਾ = sp + (i * ਆਕਾਰ(ਸ਼ਬਦ_t));
        read_ਸ਼ਬਦ_From_VSpace_ret_t ਨਤੀਜਾ;
        ਨਤੀਜਾ = read_ਸ਼ਬਦ_From_VSpace(pd, ਪਤਾ);
        ਜੇ (ਨਤੀਜਾ.ਸਥਿਤੀ == EXCEPTION_NONE) {
            ਲਿਖੋ("0x%lx: 0x%lx\n", (ਲੰਮਾ)ਪਤਾ, (ਲੰਮਾ)ਨਤੀਜਾ.ਮੁੱਲ);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("0x%lx: INVALID\n", (ਲੰਮਾ)ਪਤਾ);
        }
    }
}
#ਜੇ_ਅੰਤ
