/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/stack.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <machine/debug.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <object/untyped.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <armv/context_switch.h>
#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/tlb.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಆದರೆ_ಅಂತ್ಯ

/* ARM uses multiple identical mappings in a page table / page directory to construct
 * large mappings. In both cases it happens to be 16 entries, which can be calculated by
 * looking at the size difference of the mappings, and is done this way to remove magic
 * numbers littering this code and make it clear what is going on */
#ವ್ಯಾಖ್ಯಾನಿಸು SECTIONS_ಪ್ರತಿ_SUPER_SECTION ಬಿಟ್(ARMSuper_Section_ಬಿಟ್ಟುಗಳು - ARMSection_ಬಿಟ್ಟುಗಳು)
#ವ್ಯಾಖ್ಯಾನಿಸು ಪುಟಗಳು_ಪ್ರತಿ_LARGE_ಪುಟ ಬಿಟ್(ARMLarge_ಪುಟ_ಬಿಟ್ಟುಗಳು - ARMSmall_ಪುಟ_ಬಿಟ್ಟುಗಳು)

/* helper stuff to avoid fencepost errors when
 * getting the last byte of a PTE or PDE */
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆಯ_BYTE_PTE(PTE,ಉದ್ದಳತೆ_2) ((ಶಬ್ದ_t)&(PTE)[(ಉದ್ದಳತೆ_2)-1] + (ಬಿಟ್(PTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)-1))
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆಯ_BYTE_PDE(PDE,ಉದ್ದಳತೆ_2) ((ಶಬ್ದ_t)&(PDE)[(ಉದ್ದಳತೆ_2)-1] + (ಬಿಟ್(PDE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)-1))

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
/* Stage 2 */
#ವ್ಯಾಖ್ಯಾನಿಸು MEMATTR_CACHEABLE    0xf /* Inner and Outer write-back */
#ವ್ಯಾಖ್ಯಾನಿಸು MEMATTR_NONCACHEABLE 0x0 /* Strongly ordered or device memory */

/* STage 1 hyp */
#ವ್ಯಾಖ್ಯಾನಿಸು ATTRINDX_CACHEABLE    0xff /* Inner and Outer RW write-back non-transient */
#ವ್ಯಾಖ್ಯಾನಿಸು ATTRINDX_NONCACHEABLE 0x0  /* strongly ordered or device memory */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_Global_PD(ಶೂನ್ಯ);

ರಚನೆ resolve_ret {
    paddr_t ಚೌಕಟ್ಟು_ತಳ;
    vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ;
    bool_t valid;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ resolve_ret resolve_ret_t;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
ಸ್ಥಿರ bool_t ಶುದ್ಧ pteCheckIfMapped(pte_t *pte);
ಸ್ಥಿರ bool_t ಶುದ್ಧ pdeCheckIfMapped(pde_t *pde);

ಸ್ಥಿರ ಶಬ್ದ_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ಆಯ್ಕೆ (vm_rights) {
    ಸ್ಥಿತಿ VMNoAccess:
        ಹಿಂತಿರುಗಿ 0;

    ಸ್ಥಿತಿ VMKernelOnly:
        ಹಿಂತಿರುಗಿ 1;

    ಸ್ಥಿತಿ VMReadOnly:
        ಹಿಂತಿರುಗಿ 2;

    ಸ್ಥಿತಿ VMReadWrite:
        ಹಿಂತಿರುಗಿ 3;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid VM rights");
    }
}

#ಇಲ್ಲದಿದ್ದರೆ
/* AP encoding slightly different. AP only used for kernel mappings which are fixed after boot time */
BOOT_ಸಂಕೇತ
ಸ್ಥಿರ ಶಬ್ದ_t CONST APFromVMRights(vm_rights_t vm_rights)
{
    ಆಯ್ಕೆ (vm_rights) {
    ಸ್ಥಿತಿ VMKernelOnly:
        ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಿತಿ VMReadWrite:
        ಹಿಂತಿರುಗಿ 1;
    ಸ್ಥಿತಿ VMNoAccess:
        /* RO at PL1 only */
        ಹಿಂತಿರುಗಿ 2;
    ಸ್ಥಿತಿ VMReadOnly:
        ಹಿಂತಿರುಗಿ 3;
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid VM rights");
    }
}

ಸ್ಥಿರ ಶಬ್ದ_t CONST HAPFromVMRights(vm_rights_t vm_rights)
{
    ಆಯ್ಕೆ (vm_rights) {
    ಸ್ಥಿತಿ VMKernelOnly:
    ಸ್ಥಿತಿ VMNoAccess:
        ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಿತಿ VMReadOnly:
        ಹಿಂತಿರುಗಿ 1;
    /*
    case VMWriteOnly:
        return 2;
    */
    ಸ್ಥಿತಿ VMReadWrite:
        ಹಿಂತಿರುಗಿ 3;
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid VM rights");
    }
}

#ಆದರೆ_ಅಂತ್ಯ

vm_rights_t CONST ಮುಖವಾಡ_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ಮುಖವಾಡ)
{
    ಆದರೆ (vm_rights == VMNoAccess) {
        ಹಿಂತಿರುಗಿ VMNoAccess;
    }
    ಆದರೆ (vm_rights == VMReadOnly &&
        seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ)) {
        ಹಿಂತಿರುಗಿ VMReadOnly;
    }
    ಆದರೆ (vm_rights == VMReadWrite &&
        seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ)) {
        ಆದರೆ (!seL4_CapRights_get_capAllowWrite(cap_rights_ಮುಖವಾಡ)) {
            ಹಿಂತಿರುಗಿ VMReadOnly;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ VMReadWrite;
        }
    }
    ಆದರೆ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ಮುಖವಾಡ)) {
        userError("Attempted to make unsupported write only mapping");
    }
    ಹಿಂತಿರುಗಿ VMKernelOnly;
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_kernel_ಚೌಕಟ್ಟು(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights, vm_ಗುಣಲಕ್ಷಣಗಳು_t ಗುಣಲಕ್ಷಣಗಳು)
{
    ಶಬ್ದ_t idx = (vaddr & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection))) >> ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSmall_ಪುಟ);

    ಪ್ರತಿಜ್ಞಾ(vaddr >= PPTR_ತುದಿ); /* vaddr lies in the region the global PT covers */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಶಬ್ದ_t tex;
    ಆದರೆ (vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_ಪುಟ_Cacheable(ಗುಣಲಕ್ಷಣಗಳು)) {
        tex = 5; /* TEX = 0b1(Cached)01(Outer Write Allocate) */
    } ಇಲ್ಲದಿದ್ದರೆ {
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
#ಇಲ್ಲದಿದ್ದರೆ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
            vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_ಪುಟ_Cacheable(ಗುಣಲಕ್ಷಣಗಳು)
        );
#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_ARM_HYPERVISOR_SUPPORT */
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_kernel_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    paddr_t  phys;
    ಶಬ್ದ_t idx;
    pde_t    pde;

    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    /* up to end of virtual address space minus 16M using 16M frames */
    phys = phys_ತಳ();
    idx = PPTR_ತಳ >> ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection);

    ಆಗ (idx < ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - SECTIONS_ಪ್ರತಿ_SUPER_SECTION) {
        ಶಬ್ದ_t idx2;

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
        ಗಾಗಿ (idx2 = idx; idx2 < idx + SECTIONS_ಪ್ರತಿ_SUPER_SECTION; idx2++) {
            armKSGlobalPD[idx2] = pde;
        }
        phys += ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSuperSection));
        idx += SECTIONS_ಪ್ರತಿ_SUPER_SECTION;
    }

    ಆಗ (idx < (PPTR_ತುದಿ >> ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection))) {
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
        phys += ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection));
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    ಪ್ರತಿಜ್ಞಾ(phys == PADDR_ತುದಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
    /* map log buffer page table. PTEs to be filled by user later by calling seL4_BenchmarkSetLogBuffer() */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalLogPT), /*ವಿಳಾಸ*/
            true,                           /* P       */
            0                               /* Domain  */
        );

    memzero(armKSGlobalLogPT, ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));

    phys += ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection));
    idx++;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */

    /* map page table covering last 1M of virtual address space to page directory */
    armKSGlobalPD[idx] =
        pde_pde_coarse_new(
            addrFromKPPtr(armKSGlobalPT), /*ವಿಳಾಸ*/
            true,                        /* P       */
            0                            /* Domain  */
        );

    /* now start initialising the page table */
    memzero(armKSGlobalPT, 1 << se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು);

    /* map vector table */
    map_kernel_ಚೌಕಟ್ಟು(
        addrFromKPPtr(arm_ಸದಿಶ_ಕೋಷ್ಟಕ),
        PPTR_ಸದಿಶ_ಕೋಷ್ಟಕ,
        VMKernelOnly,
        vm_ಗುಣಲಕ್ಷಣಗಳು_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#ಇಲ್ಲದಿದ್ದರೆ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_kernel_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    paddr_t    phys;
    uint32_t   idx;
    pdeS1_t pde;
    pte_t ಬಳಸದ pteS2;

    /* Initialise PGD */
    ಗಾಗಿ (idx = 0; idx < 3; idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPGD[idx] = pde;
    }
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPD));
    armHSGlobalPGD[3] = pde;

    /* Initialise PMD */
    /* Invalidate up until USER_TOP */
    ಗಾಗಿ (idx = 0; idx < (USER_ತುದಿ - 0xC0000000) >> (PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು); idx++) {
        pde = pdeS1_pdeS1_invalid_new();
        armHSGlobalPD[idx] = pde;
    }
    /* mapping of PPTR_BASE (virtual address) to kernel's physBase  */
    /* up to end of virtual address space minus 2M using 2M frames */
    phys = phys_ತಳ();
    ಗಾಗಿ (; idx < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1; idx++) {
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
        phys += ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು);
    }
    /* map page table covering last 2M of virtual address space */
    pde = pdeS1_pdeS1_coarse_new(0, 0, 0, 0, addrFromKPPtr(armHSGlobalPT));
    armHSGlobalPD[idx] = pde;

    /* now start initialising the page table */
    memzero(armHSGlobalPT, 1 << se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು);
    ಗಾಗಿ (idx = 0; idx < 256; idx++) {
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
        phys += ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು);
    }
    /* map vector table */
    map_kernel_ಚೌಕಟ್ಟು(
        addrFromKPPtr(arm_ಸದಿಶ_ಕೋಷ್ಟಕ),
        PPTR_ಸದಿಶ_ಕೋಷ್ಟಕ,
        VMKernelOnly,
        vm_ಗುಣಲಕ್ಷಣಗಳು_new(
            false, /* armExecuteNever */
            true,  /* armParityEnabled */
            true   /* armPageCacheable */
        )
    );

    map_kernel_devices();
}

#ಆದರೆ_ಅಂತ್ಯ /* !CONFIG_ARM_HYPERVISOR_SUPPORT */

ಸ್ಥಿರ BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_ಚೌಕಟ್ಟು_cap(cap_t pd_cap, cap_t ಚೌಕಟ್ಟು_cap, bool_t executable)
{
    pte_t *pt;
    pte_t *ಗುರಿ_ಸ್ಥಾನ;
    pde_t *pd    = PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    ಶೂನ್ಯ  *ಚೌಕಟ್ಟು = (ಶೂನ್ಯ *)generic_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(ಚೌಕಟ್ಟು_cap);
    vptr_t vptr  = generic_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(ಚೌಕಟ್ಟು_cap);

    ಪ್ರತಿಜ್ಞಾ(generic_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(ಚೌಕಟ್ಟು_cap) != 0);

    pd += (vptr >> ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection));
    pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ವಿಳಾಸ(pd));
    ಗುರಿ_ಸ್ಥಾನ = pt + ((vptr & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection)))
                       >> ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSmall_ಪುಟ));
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    *ಗುರಿ_ಸ್ಥಾನ = pte_pte_small_new(
                      addrFromPPtr(ಚೌಕಟ್ಟು),
                      1, /* not global */
                      SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                      0, /* APX = 0, privileged full access */
                      5, /* TEX = 0b1(Cached)01(Outer Write Allocate) */
                      APFromVMRights(VMReadWrite),
                      0, /* C (Inner write allocate) */
                      1, /* B (Inner write allocate) */
                      !executable
                  );
#ಇಲ್ಲದಿದ್ದರೆ
    *ಗುರಿ_ಸ್ಥಾನ = pte_pte_small_new(
                      0, /* Executable */
                      0, /* Not contiguous */
                      addrFromPPtr(ಚೌಕಟ್ಟು),
                      1, /* AF -- always set */
                      0, /* Not shared */
                      HAPFromVMRights(VMReadWrite),
                      MEMATTR_CACHEABLE  /* Cacheable */
                  );
#ಆದರೆ_ಅಂತ್ಯ
}

/* Create a frame cap for the initial thread. */

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_ಚೌಕಟ್ಟು_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large)
{
    ಆದರೆ (use_large)
        ಹಿಂತಿರುಗಿ
            cap_ಚೌಕಟ್ಟು_cap_new(
                ARMSection,                    /* capFSize           */
                ASID_ಕೆಳಗಿನ(asid),                /* capFMappedASIDLow  */
                ಶಬ್ದ_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
                ASID_ಮೇಲಿನ(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
    ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ
            cap_small_ಚೌಕಟ್ಟು_cap_new(
                ASID_ಕೆಳಗಿನ(asid),                /* capFMappedASIDLow  */
                ಶಬ್ದ_From_VMRights(VMReadWrite), /* capFVMRights       */
                vptr,                          /* capFMappedAddress  */
                false,                         /* capFIsDevice       */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
                0,                             /* IOSpace            */
#ಆದರೆ_ಅಂತ್ಯ
                ASID_ಮೇಲಿನ(asid),               /* capFMappedASIDHigh */
                pptr                           /* capFBasePtr        */
            );
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_pt_cap(cap_t pd_cap, cap_t pt_cap)
{
    pde_t *pd   = PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    pte_t *pt   = PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(pt_cap);
    pde_t *ಗುರಿ_ಸ್ಥಾನ = pd + (vptr >> ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection));

    ಪ್ರತಿಜ್ಞಾ(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(pt_cap));

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    *ಗುರಿ_ಸ್ಥಾನ = pde_pde_coarse_new(
                      addrFromPPtr(pt), /*ವಿಳಾಸ*/
                      true,             /* P       */
                      0                 /* Domain  */
                  );
#ಇಲ್ಲದಿದ್ದರೆ
    *ಗುರಿ_ಸ್ಥಾನ = pde_pde_coarse_new(addrFromPPtr(pt));
#ಆದರೆ_ಅಂತ್ಯ
}

/* Create a page table for the initial thread */

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_ಪುಟ_ಕೋಷ್ಟಕ_cap(cap_t pd, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    ಆದರೆ (asid != asidInvalid) {
        map_it_pt_cap(pd, cap);
    }
    ಹಿಂತಿರುಗಿ cap;
}

BOOT_ಸಂಕೇತ ಶಬ್ದ_t arch_get_n_paging(v_region_t it_v_reg)
{
    ಹಿಂತಿರುಗಿ get_n_paging(it_v_reg, PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು);
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_ಸಂಕೇತ cap_t create_it_ವಿಳಾಸ_space(cap_t ಬೇರು_cnode_cap, v_region_t it_v_reg)
{
    vptr_t     pt_vptr;
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_before;
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_after;

    /* create PD cap */
    copyGlobalMappings(PDE_PTR(rootserver.vspace));
    cleanCacheRange_PoU(rootserver.vspace, rootserver.vspace + (1 << se_L4_ಪುಟ_Dir_ಬಿಟ್ಟುಗಳು) - 1,
                        addrFromPPtr((ಶೂನ್ಯ *)rootserver.vspace));
    cap_t pd_cap =
        cap_ಪುಟ_directory_cap_new(
            true,    /* capPDIsMapped   */
            IT_ASID, /* capPDMappedASID */
            rootserver.vspace  /* capPDBasePtr    */
        );
    ಸ್ಥಾನ_pos_before = ndks_boot.ಸ್ಥಾನ_pos_cur;
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);

    /* create all PT caps necessary to cover userland image */
    ಗಾಗಿ (pt_vptr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(it_v_reg.ಆರಂಭ, PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು);
         pt_vptr < it_v_reg.ಕೊನೆ;
         pt_vptr += ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap,
                         create_it_ಪುಟ_ಕೋಷ್ಟಕ_cap(pd_cap, it_alloc_paging(), pt_vptr, IT_ASID))
           ) {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }
    }

    ಸ್ಥಾನ_pos_after = ndks_boot.ಸ್ಥಾನ_pos_cur;
    ndks_boot.bi_ಚೌಕಟ್ಟು->userImagePaging = (se_L4_ಸ್ಥಾನ_Region) {
        ಸ್ಥಾನ_pos_before, ಸ್ಥಾನ_pos_after
    };

    ಹಿಂತಿರುಗಿ pd_cap;
}

BOOT_ಸಂಕೇತ cap_t create_unmapped_it_ಚೌಕಟ್ಟು_cap(pptr_t pptr, bool_t use_large)
{
    ಹಿಂತಿರುಗಿ create_it_ಚೌಕಟ್ಟು_cap(pptr, 0, asidInvalid, use_large);
}

BOOT_ಸಂಕೇತ cap_t create_mapped_it_ಚೌಕಟ್ಟು_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable)
{
    cap_t cap = create_it_ಚೌಕಟ್ಟು_cap(pptr, vptr, asid, use_large);
    map_it_ಚೌಕಟ್ಟು_cap(pd_cap, cap, executable);
    ಹಿಂತಿರುಗಿ cap;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT

BOOT_ಸಂಕೇತ ಶೂನ್ಯ activate_kernel_vspace(ಶೂನ್ಯ)
{
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    ಗಣ_Global_PD();
    invalidateLocalTLB();
    ಬೀಗ_TLBEntry(PPTR_ತಳ);
    ಬೀಗ_TLBEntry(PPTR_ಸದಿಶ_ಕೋಷ್ಟಕ);
}

#ಇಲ್ಲದಿದ್ದರೆ

BOOT_ಸಂಕೇತ ಶೂನ್ಯ activate_kernel_vspace(ಶೂನ್ಯ)
{
    uint32_t r;
    /* Ensure that there's nothing stale in newly-mapped regions, and
       that everything we've written (particularly the kernel page tables)
       is committed. */
    cleanInvalidateL1Caches();
    /* Setup the memory attributes: We use 2 indices (cachable/non-cacheable) */
    ಗಣ_HMAIR((ATTRINDX_NONCACHEABLE << 0) | (ATTRINDX_CACHEABLE << 8), 0);
    ಗಣ_Current_Hyp_PD(addrFromKPPtr(armHSGlobalPGD));
    invalidateHypTLB();
#ಆದರೆ 0 /* Can't lock entries on A15 */
    ಬೀಗ_TLBEntry(PPTR_ತಳ);
    ಬೀಗ_TLBEntry(PPTR_ಸದಿಶ_ಕೋಷ್ಟಕ);
#ಆದರೆ_ಅಂತ್ಯ
    /* TODO find a better place to init the VMMU */
    r = 0;
    /* Translation range */
    r |= (0x0 << 0);     /* 2^(32 -(0)) input range. */
    r |= (r & 0x8) << 1; /* Sign bit */
    /* starting level */
    r |= (0x0 << 6);     /* Start at second level */
    /* Sharability of tables */
    r |= ಬಿಟ್(8);       /* Inner write-back, write-allocate */
    r |= ಬಿಟ್(10);      /* Outer write-back, write-allocate */
    /* Long descriptor format (not that we have a choice) */
    r |= ಬಿಟ್(31);
    ಗಣ_VTCR(r);
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ write_it_asid_pool(cap_t it_ap_cap, cap_t it_pd_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_ದ_cap(it_ap_cap));
    ap->array[ASID_ಕೆಳಗಿನ(IT_ASID)] = PDE_PTR(pptr_ದ_cap(it_pd_cap));
    armKSASIDTable[ASID_ಮೇಲಿನ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_PDFor_ASID(asid_t asid)
{
    ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ret;
    asid_pool_t *poolPtr;
    pde_t *pd;

    poolPtr = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಆದರೆ (unlikely(!poolPtr)) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

        ret.pd = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    pd = poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
    ಆದರೆ (unlikely(!pd)) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

        ret.pd = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.pd = pd;
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

ಶಬ್ದ_t *ಶುದ್ಧ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    ಶಬ್ದ_t w_ಬಫರ್_Ptr;
    cap_t ಬಫರ್_Cap;
    vm_rights_t vm_rights;

    w_ಬಫರ್_Ptr = thread->tcbIPCBuffer;
    ಬಫರ್_Cap = TCB_PTR_CTE_PTR(thread, tcb_ಬಫರ್)->cap;

    ಆದರೆ (unlikely(cap_get_capType(ಬಫರ್_Cap) != cap_small_ಚೌಕಟ್ಟು_cap &&
                 cap_get_capType(ಬಫರ್_Cap) != cap_ಚೌಕಟ್ಟು_cap)) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (unlikely(generic_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(ಬಫರ್_Cap))) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    vm_rights = generic_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(ಬಫರ್_Cap);
    ಆದರೆ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        ಶಬ್ದ_t ತಳ_Ptr;
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಪುಟ_ಬಿಟ್ಟುಗಳು_2;

        ತಳ_Ptr = generic_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(ಬಫರ್_Cap);
        ಪುಟ_ಬಿಟ್ಟುಗಳು_2 = ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(generic_ಚೌಕಟ್ಟು_cap_get_cap_FSize(ಬಫರ್_Cap));
        ಹಿಂತಿರುಗಿ (ಶಬ್ದ_t *)(ತಳ_Ptr + (w_ಬಫರ್_Ptr & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_2)));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ಆದರೆ (unlikely(cap_get_capType(cap) != cap_small_ಚೌಕಟ್ಟು_cap &&
                 cap_get_capType(cap) != cap_ಚೌಕಟ್ಟು_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (unlikely(generic_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(vptr & ಮುಖವಾಡ(se_L4_IPCBuffer_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (ಪೂರ್ಣಾಂಕ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

pde_t *CONST lookupPDSlot(pde_t *pd, vptr_t vptr)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pd_ಸೂಚ್ಯಂಕ;

    pd_ಸೂಚ್ಯಂಕ = vptr >> (ಪುಟ_ಬಿಟ್ಟುಗಳು + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು);
    ಹಿಂತಿರುಗಿ pd + pd_ಸೂಚ್ಯಂಕ;
}

lookupPTSlot_ret_t lookupPTSlot(pde_t *pd, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    pde_t *pd_ಸ್ಥಾನ;

    pd_ಸ್ಥಾನ = lookupPDSlot(pd, vptr);

    ಆದರೆ (unlikely(pde_ptr_get_pdeType(pd_ಸ್ಥಾನ) != pde_pde_coarse)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು);

        ret.pt_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    } ಇಲ್ಲದಿದ್ದರೆ {
        pte_t *pt, *pt_ಸ್ಥಾನ;
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pt_ಸೂಚ್ಯಂಕ;

        pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ವಿಳಾಸ(pd_ಸ್ಥಾನ));
        pt_ಸೂಚ್ಯಂಕ = (vptr >> ಪುಟ_ಬಿಟ್ಟುಗಳು) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು);
        pt_ಸ್ಥಾನ = pt + pt_ಸೂಚ್ಯಂಕ;

        ret.pt_ಸ್ಥಾನ = pt_ಸ್ಥಾನ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
    }
}

ಸ್ಥಿರ pte_t *lookupPTSlot_nofail(pte_t *pt, vptr_t vptr)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pt_ಸೂಚ್ಯಂಕ;

    pt_ಸೂಚ್ಯಂಕ = (vptr >> ಪುಟ_ಬಿಟ್ಟುಗಳು) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು);
    ಹಿಂತಿರುಗಿ pt + pt_ಸೂಚ್ಯಂಕ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ resolve_ret_t default_resolve_ret_t;

ಸ್ಥಿರ resolve_ret_t resolveVAddr(pde_t *pd, vptr_t vaddr)
{
    pde_t *pde = lookupPDSlot(pd, vaddr);
    resolve_ret_t ret = default_resolve_ret_t;

    ret.valid = true;

    ಆಯ್ಕೆ (pde_ptr_get_pdeType(pde)) {
    ಸ್ಥಿತಿ pde_pde_section:
        ret.ಚೌಕಟ್ಟು_ತಳ = pde_pde_section_ptr_get_ವಿಳಾಸ(pde);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಆದರೆ (pde_pde_section_ptr_get_ಗಾತ್ರ(pde)) {
            ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSuperSection;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSection;
        }
#ಇಲ್ಲದಿದ್ದರೆ
        ಆದರೆ (pde_pde_section_ptr_get_contiguous_hint(pde)) {
            /* Entries are represented as 16 contiguous sections. We need to mask
               to get the super section frame base */
            ret.ಚೌಕಟ್ಟು_ತಳ &= ~ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSuperSection));
            ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSuperSection;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSection;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ pde_pde_coarse: {
        pte_t *pt = ptrFromPAddr(pde_pde_coarse_ptr_get_ವಿಳಾಸ(pde));
        pte_t *pte = lookupPTSlot_nofail(pt, vaddr);
        ಆಯ್ಕೆ (pte_ptr_get_pteType(pte)) {
        ಸ್ಥಿತಿ pte_pte_small:
            ret.ಚೌಕಟ್ಟು_ತಳ = pte_pte_small_ptr_get_ವಿಳಾಸ(pte);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
            ಆದರೆ (pte_pte_small_ptr_get_contiguous_hint(pte)) {
                /* Entries are represented as 16 contiguous small frames. We need to mask
                   to get the large frame base */
                ret.ಚೌಕಟ್ಟು_ತಳ &= ~ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMLarge_ಪುಟ));
                ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMLarge_ಪುಟ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSmall_ಪುಟ;
            }
#ಇಲ್ಲದಿದ್ದರೆ
            ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMSmall_ಪುಟ;
#ಆದರೆ_ಅಂತ್ಯ
            ಹಿಂತಿರುಗಿ ret;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಸ್ಥಿತಿ pte_pte_large:
            ret.ಚೌಕಟ್ಟು_ತಳ = pte_pte_large_ptr_get_ವಿಳಾಸ(pte);
            ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ = ARMLarge_ಪುಟ;
            ಹಿಂತಿರುಗಿ ret;
#ಆದರೆ_ಅಂತ್ಯ
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು;
        }
        ನಿಲ್ಲಿಸು;
    }
    }

    ret.valid = false;
    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ pte_t CONST makeUserPTE(vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, paddr_t paddr,
                               bool_t cacheable, bool_t nonexecutable, vm_rights_t vm_rights)
{
    pte_t pte;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಶಬ್ದ_t ap;

    ap = APFromVMRights(vm_rights);

    ಆಯ್ಕೆ (ಪುಟ_ಗಾತ್ರ) {
    ಸ್ಥಿತಿ ARMSmall_ಪುಟ: {
        ಆದರೆ (cacheable) {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    nonexecutable);
        } ಇಲ್ಲದಿದ್ದರೆ {
            pte = pte_pte_small_new(paddr,
                                    1, /* not global */
                                    1, /* shared */
                                    0, /* APX = 0, privileged full access */
                                    0, /* TEX = 0b000, strongly-ordered. */
                                    ap,
                                    0, 0,
                                    nonexecutable);
        }
        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ ARMLarge_ಪುಟ: {
        ಆದರೆ (cacheable) {
            pte = pte_pte_large_new(paddr,
                                    nonexecutable,
                                    5, /* TEX = 0b101, outer write-back, write-allocate */
                                    1, /* not global */
                                    SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                    0, /* APX = 0, privileged full access */
                                    ap,
                                    0, 1, /* Inner write-back, write-allocate */
                                    1 /* reserved */);
        } ಇಲ್ಲದಿದ್ದರೆ {
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
        ನಿಲ್ಲಿಸು;
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid PTE frame type");
    }

#ಇಲ್ಲದಿದ್ದರೆ

    ಶಬ್ದ_t hap;

    hap = HAPFromVMRights(vm_rights);

    ಆಯ್ಕೆ (ಪುಟ_ಗಾತ್ರ) {
    ಸ್ಥಿತಿ ARMSmall_ಪುಟ: {
        ಆದರೆ (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_CACHEABLE /* Cacheable */);
        } ಇಲ್ಲದಿದ್ದರೆ {
            pte = pte_pte_small_new(
                      nonexecutable, /* Executable */
                      0,      /* Not contiguous */
                      paddr,
                      1,      /* AF - Always set */
                      0,      /* not shared */
                      hap,    /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ ARMLarge_ಪುಟ: {
        ಆದರೆ (cacheable) {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_CACHEABLE  /* Cacheable */);
        } ಇಲ್ಲದಿದ್ದರೆ {
            pte = pte_pte_small_new(
                      nonexecutable,   /* Executable */
                      1,   /* 16 contiguous */
                      paddr,
                      1,   /* AF - Always set */
                      0,   /* not shared */
                      hap, /* HAP - access */
                      MEMATTR_NONCACHEABLE /* Not cacheable */);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid PTE frame type");
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

    ಹಿಂತಿರುಗಿ pte;
}

ಸ್ಥಿರ pde_t CONST makeUserPDE(vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, paddr_t paddr, bool_t parity,
                               bool_t cacheable, bool_t nonexecutable, ಶಬ್ದ_t domain,
                               vm_rights_t vm_rights)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಶಬ್ದ_t ap, size2;

    ap = APFromVMRights(vm_rights);
#ಇಲ್ಲದಿದ್ದರೆ
    ಶಬ್ದ_t hap, size2;

    (ಶೂನ್ಯ)domain;
    hap = HAPFromVMRights(vm_rights);
#ಆದರೆ_ಅಂತ್ಯ

    ಆಯ್ಕೆ (ಪುಟ_ಗಾತ್ರ) {
    ಸ್ಥಿತಿ ARMSection:
        size2 = 0;
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ ARMSuperSection:
        size2 = 1;
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid PDE frame type");
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಆದರೆ (cacheable) {
        ಹಿಂತಿರುಗಿ pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   SMP_TERNARY(1, 0), /* shareable if SMP enabled, otherwise unshared */
                                   0, /* APX = 0, privileged full access */
                                   5, /* TEX = 0b101, outer write-back, write-allocate */
                                   ap, parity, domain, nonexecutable,
                                   0, 1 /* Inner write-back, write-allocate */);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ pde_pde_section_new(paddr, size2,
                                   1, /* not global */
                                   1, /* shared */
                                   0, /* APX = 0, privileged full access */
                                   0, /* TEX = 0b000, strongly-ordered */
                                   ap, parity, domain, nonexecutable,
                                   0, 0);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (cacheable) {
        ಹಿಂತಿರುಗಿ pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_CACHEABLE /* Cacheable */);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ pde_pde_section_new(
                   nonexecutable, /* Executable */
                   size2, /* contiguous */
                   paddr,
                   1, /* AF - Always set */
                   0, /* not shared */
                   hap,
                   MEMATTR_NONCACHEABLE /* Not cacheable */);
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
}

bool_t CONST is_Valid_VTable_ಬೇರು(cap_t cap)
{
    ಹಿಂತಿರುಗಿ cap_get_capType(cap) == cap_ಪುಟ_directory_cap &&
           cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(cap);
}

bool_t CONST is_IOSpace_ಚೌಕಟ್ಟು_Cap(cap_t cap)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಹಿಂತಿರುಗಿ cap_get_capType(cap) == cap_small_ಚೌಕಟ್ಟು_cap && cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FIs_IOSpace(cap);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ false;
#ಆದರೆ_ಅಂತ್ಯ
}

/** Switch to the empty PD on the reserved HW ASID */
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_Global_PD(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    writeContextIDAndPD(hwASIDReserved, addrFromKPPtr(armUSGlobalPD));
#ಇಲ್ಲದಿದ್ದರೆ
    /* Before changing the PD ensure all memory accesses have completed */
    dsb();
    /* First switch to global PD on old ASID. Stale TLB entries may exist
       under the old ASID, but no new stale mappings can be added any more. */
    writeTTBR0Ptr(addrFromKPPtr(armKSGlobalPD));
    /* Ensure the PD switch completes before we do anything else */
    isb();
    /* Switch to reserved HW ASID. Only empty/global kernel mappings are
       now available from the TLB. */
    ಗಣ_Hardware_ASID(hwASIDReserved);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ ಗಣ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ಬೇರು;
    asid_t asid;
    pde_t *pd;
    ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    ಆದರೆ (cap_get_capType(thread_ಬೇರು) != cap_ಪುಟ_directory_cap ||
        !cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(thread_ಬೇರು)) {
        ಗಣ_Global_PD();
        ಹಿಂತಿರುಗಿ;
    }

    pd = PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(thread_ಬೇರು));
    asid = cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(thread_ಬೇರು);
    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(asid);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE || ಕಂಡುಹಿಡಿ_ret.pd != pd)) {
        ಗಣ_Global_PD();
        ಹಿಂತಿರುಗಿ;
    }

    armv_contextSwitch(pd, asid);
}

ಸ್ಥಿರ bool_t ಗಣ_VMRoot_For_Flush(pde_t *pd, asid_t asid)
{
    cap_t thread_ಬೇರು;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbVTable)->cap;

    ಆದರೆ (cap_get_capType(thread_ಬೇರು) == cap_ಪುಟ_directory_cap &&
        cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(thread_ಬೇರು) &&
        PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(thread_ಬೇರು)) == pd) {
        ಹಿಂತಿರುಗಿ false;
    }

    armv_contextSwitch(pd, asid);

    ಹಿಂತಿರುಗಿ true;
}

pde_t *ಪುಟ_ಕೋಷ್ಟಕ_Mapped(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    pde_t pde;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pd_ಸೂಚ್ಯಂಕ;

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(asid);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    pd_ಸೂಚ್ಯಂಕ = vaddr >> (ಪುಟ_ಬಿಟ್ಟುಗಳು + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು);
    pde = ಕಂಡುಹಿಡಿ_ret.pd[pd_ಸೂಚ್ಯಂಕ];

    ಆದರೆ (likely(pde_get_pdeType(pde) == pde_pde_coarse
               && ptrFromPAddr(pde_pde_coarse_get_ವಿಳಾಸ(pde)) == pt)) {
        ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ret.pd;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ invalidateASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    pd = asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(pd);

    pd[PD_ASID_ಸ್ಥಾನ] = pde_pde_invalid_new(0, false);
}

ಸ್ಥಿರ pde_t ಶುದ್ಧ ತುಂಬು_HWASID(asid_t asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    pd = asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(pd);

    ಹಿಂತಿರುಗಿ pd[PD_ASID_ಸ್ಥಾನ];
}

ಸ್ಥಿರ ಶೂನ್ಯ storeHWASID(asid_t asid, hw_asid_t hw_asid)
{
    asid_pool_t *asidPool;
    pde_t *pd;

    asidPool = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(asidPool);

    pd = asidPool->array[ASID_ಕೆಳಗಿನ(asid)];
    ಪ್ರತಿಜ್ಞಾ(pd);

    /* Store HW ASID in the last entry
       Masquerade as an invalid PDE */
    pd[PD_ASID_ಸ್ಥಾನ] = pde_pde_invalid_new(hw_asid, true);

    armKSHWASIDTable[hw_asid] = asid;
}

hw_asid_t ಕಂಡುಹಿಡಿ_Free_HWASID(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t hw_asid_ಸ್ಥಾನಾಂತರ;
    hw_asid_t hw_asid;

    /* Find a free hardware ASID */
    ಗಾಗಿ (hw_asid_ಸ್ಥಾನಾಂತರ = 0;
         hw_asid_ಸ್ಥಾನಾಂತರ <= (ಶಬ್ದ_t)((hw_asid_t) - 1);
         hw_asid_ಸ್ಥಾನಾಂತರ ++) {
        hw_asid = armKSNextASID + ((hw_asid_t)hw_asid_ಸ್ಥಾನಾಂತರ);
        ಆದರೆ (hw_asid != hwASIDReserved && armKSHWASIDTable[hw_asid] == asidInvalid) {
            ಹಿಂತಿರುಗಿ hw_asid;
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
    ಆದರೆ (armKSNextASID == hwASIDReserved) {
        armKSNextASID = hwASIDMin;
    }
    ಹಿಂತಿರುಗಿ hw_asid;
}

hw_asid_t getHWASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ತುಂಬು_HWASID(asid);
    ಆದರೆ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ಹಿಂತಿರುಗಿ pde_pde_invalid_get_stored_hw_asid(stored_hw_asid);
    } ಇಲ್ಲದಿದ್ದರೆ {
        hw_asid_t new_hw_asid;

        new_hw_asid = ಕಂಡುಹಿಡಿ_Free_HWASID();
        storeHWASID(asid, new_hw_asid);
        ಹಿಂತಿರುಗಿ new_hw_asid;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ invalidateASIDEntry(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ತುಂಬು_HWASID(asid);
    ಆದರೆ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        armKSHWASIDTable[pde_pde_invalid_get_stored_hw_asid(stored_hw_asid)] =
            asidInvalid;
    }
    invalidateASID(asid);
}

ಶೂನ್ಯ unmap_ಪುಟ_ಕೋಷ್ಟಕ(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    pde_t *pd, *pd_ಸ್ಥಾನ;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pd_ಸೂಚ್ಯಂಕ;

    pd = ಪುಟ_ಕೋಷ್ಟಕ_Mapped(asid, vaddr, pt);

    ಆದರೆ (likely(pd != ಖಾಲಿ)) {
        pd_ಸೂಚ್ಯಂಕ = vaddr >> (PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು);
        pd_ಸ್ಥಾನ = pd + pd_ಸೂಚ್ಯಂಕ;

        *pd_ಸ್ಥಾನ = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((ಶಬ್ದ_t)pd_ಸ್ಥಾನ, addrFromPPtr(pd_ಸ್ಥಾನ));
        flush_ಕೋಷ್ಟಕ(pd, asid, vaddr, pt);
    }
}

ಶೂನ್ಯ copyGlobalMappings(pde_t *newPD)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಶಬ್ದ_t i;
    pde_t *global_pd = armKSGlobalPD;

    ಗಾಗಿ (i = PPTR_ತಳ >> ARMSection_ಬಿಟ್ಟುಗಳು; i < ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        ಆದರೆ (i != PD_ASID_ಸ್ಥಾನ) {
            newPD[i] = global_pd[i];
        }
    }
#ಆದರೆ_ಅಂತ್ಯ
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    ಆಯ್ಕೆ (vm_faultType) {
    ಸ್ಥಿತಿ ARMDataAbort: {
        ಶಬ್ದ_t addr, fault;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        addr = getHDFAR();
        addr = (ವಿಳಾಸ_Translate_S1(addr) & ~ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು)) | (addr & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು));
        /* MSBs tell us that this was a DataAbort */
        fault = getHSR() & 0x3ffffff;
#ಇಲ್ಲದಿದ್ದರೆ
        addr = getFAR();
        fault = getDFSR();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
        /* Debug exceptions come in on the Prefetch and Data abort vectors.
         * We have to test the fault-status bits in the IFSR/DFSR to determine
         * if it's a debug exception when one occurs.
         *
         * If it is a debug exception, return early and don't fallthrough to the
         * normal VM Fault handling path.
         */
        ಆದರೆ (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(0);
            ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;
        }
#ಆದರೆ_ಅಂತ್ಯ
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;
    }

    ಸ್ಥಿತಿ ARMPrefetchAbort: {
        ಶಬ್ದ_t pc, fault;

        pc = getRestartPC(thread);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        pc = (ವಿಳಾಸ_Translate_S1(pc) & ~ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು)) | (pc & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು));
        /* MSBs tell us that this was a PrefetchAbort */
        fault = getHSR() & 0x3ffffff;
#ಇಲ್ಲದಿದ್ದರೆ
        fault = getIFSR();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
        ಆದರೆ (isDebugFault(fault)) {
            current_fault = handleUserLevelDebugException(pc);

            ಆದರೆ (seL4_Fault_DebugException_get_exceptionReason(current_fault) == se_L4_Single_ಹೆಜ್ಜೆ
                && !single_ಹೆಜ್ಜೆ_Fault_Counter_Ready(thread)) {
                /* Don't send a fault message to the thread yet if we were asked
                 * to step through N instructions and the counter isn't depleted
                 * yet.
                 */
                ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
            }
            ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;
        }
#ಆದರೆ_ಅಂತ್ಯ
        current_fault = seL4_Fault_VMFault_new(pc, fault, true);
        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid VM fault type");
    }
}

ಶೂನ್ಯ deleteASIDPool(asid_t asid_ತಳ, asid_pool_t *pool)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;

    /* Haskell error: "ASID pool's base must be aligned" */
    ಪ್ರತಿಜ್ಞಾ((asid_ತಳ & ಮುಖವಾಡ(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು)) == 0);

    ಆದರೆ (armKSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] == pool) {
        ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = 0; ಸ್ಥಾನಾಂತರ < ಬಿಟ್(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು); ಸ್ಥಾನಾಂತರ++) {
            ಆದರೆ (pool->array[ಸ್ಥಾನಾಂತರ]) {
                flushSpace(asid_ತಳ + ಸ್ಥಾನಾಂತರ);
                invalidateASIDEntry(asid_ತಳ + ಸ್ಥಾನಾಂತರ);
            }
        }
        armKSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] = ಖಾಲಿ;
        ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
    }
}

ಶೂನ್ಯ deleteASID(asid_t asid, pde_t *pd)
{
    asid_pool_t *poolPtr;

    poolPtr = armKSASIDTable[ASID_ಮೇಲಿನ(asid)];

    ಆದರೆ (poolPtr != ಖಾಲಿ && poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] == pd) {
        flushSpace(asid);
        invalidateASIDEntry(asid);
        poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = ಖಾಲಿ;
        ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
ಸ್ಥಿರ pte_t pte_pte_invalid_new(ಶೂನ್ಯ)
{
    /* Invalid as every PTE must have bit 0 set (large PTE) or bit 1 set (small
     * PTE). 0 == 'translation fault' in ARM parlance.
     */
    ಹಿಂತಿರುಗಿ (pte_t) {
        {
            0
        }
    };
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ unmap_ಪುಟ(vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, asid_t asid, vptr_t vptr, ಶೂನ್ಯ *pptr)
{
    ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    paddr_t addr = addrFromPPtr(pptr);

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(asid);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆಯ್ಕೆ (ಪುಟ_ಗಾತ್ರ) {
    ಸ್ಥಿತಿ ARMSmall_ಪುಟ: {
        lookupPTSlot_ret_t lu_ret;

        lu_ret = lookupPTSlot(ಕಂಡುಹಿಡಿ_ret.pd, vptr);
        ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            ಹಿಂತಿರುಗಿ;
        }

        ಆದರೆ (unlikely(pte_ptr_get_pteType(lu_ret.pt_ಸ್ಥಾನ) != pte_pte_small)) {
            ಹಿಂತಿರುಗಿ;
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಆದರೆ (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_ಸ್ಥಾನ) != 0)) {
            ಹಿಂತಿರುಗಿ;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ (unlikely(pte_pte_small_ptr_get_ವಿಳಾಸ(lu_ret.pt_ಸ್ಥಾನ) != addr)) {
            ಹಿಂತಿರುಗಿ;
        }

        *(lu_ret.pt_ಸ್ಥಾನ) = pte_pte_invalid_new();
        cleanByVA_PoU((ಶಬ್ದ_t)lu_ret.pt_ಸ್ಥಾನ, addrFromPPtr(lu_ret.pt_ಸ್ಥಾನ));

        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ ARMLarge_ಪುಟ: {
        lookupPTSlot_ret_t lu_ret;
        ಶಬ್ದ_t i;

        lu_ret = lookupPTSlot(ಕಂಡುಹಿಡಿ_ret.pd, vptr);
        ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            ಹಿಂತಿರುಗಿ;
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಆದರೆ (unlikely(pte_ptr_get_pteType(lu_ret.pt_ಸ್ಥಾನ) != pte_pte_large)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (unlikely(pte_pte_large_ptr_get_ವಿಳಾಸ(lu_ret.pt_ಸ್ಥಾನ) != addr)) {
            ಹಿಂತಿರುಗಿ;
        }
#ಇಲ್ಲದಿದ್ದರೆ
        ಆದರೆ (unlikely(pte_ptr_get_pteType(lu_ret.pt_ಸ್ಥಾನ) != pte_pte_small)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (unlikely(pte_pte_small_ptr_get_contiguous_hint(lu_ret.pt_ಸ್ಥಾನ) != 1)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (unlikely(pte_pte_small_ptr_get_ವಿಳಾಸ(lu_ret.pt_ಸ್ಥಾನ) != addr)) {
            ಹಿಂತಿರುಗಿ;
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಗಾಗಿ (i = 0; i < ಪುಟಗಳು_ಪ್ರತಿ_LARGE_ಪುಟ; i++) {
            lu_ret.pt_ಸ್ಥಾನ[i] = pte_pte_invalid_new();
        }
        cleanCacheRange_PoU((ಶಬ್ದ_t)&lu_ret.pt_ಸ್ಥಾನ[0],
                            ಕೊನೆಯ_BYTE_PTE(lu_ret.pt_ಸ್ಥಾನ, ಪುಟಗಳು_ಪ್ರತಿ_LARGE_ಪುಟ),
                            addrFromPPtr(&lu_ret.pt_ಸ್ಥಾನ[0]));

        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ ARMSection: {
        pde_t *pd;

        pd = lookupPDSlot(ಕಂಡುಹಿಡಿ_ret.pd, vptr);

        ಆದರೆ (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            ಹಿಂತಿರುಗಿ;
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಆದರೆ (unlikely(pde_pde_section_ptr_get_ಗಾತ್ರ(pd) != 0)) {
#ಇಲ್ಲದಿದ್ದರೆ
        ಆದರೆ (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 0)) {
#ಆದರೆ_ಅಂತ್ಯ
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (unlikely(pde_pde_section_ptr_get_ವಿಳಾಸ(pd) != addr)) {
            ಹಿಂತಿರುಗಿ;
        }

        *pd = pde_pde_invalid_new(0, 0);
        cleanByVA_PoU((ಶಬ್ದ_t)pd, addrFromPPtr(pd));

        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ ARMSuperSection: {
        pde_t *pd;
        ಶಬ್ದ_t i;

        pd = lookupPDSlot(ಕಂಡುಹಿಡಿ_ret.pd, vptr);

        ಆದರೆ (unlikely(pde_ptr_get_pdeType(pd) != pde_pde_section)) {
            ಹಿಂತಿರುಗಿ;
        }
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಆದರೆ (unlikely(pde_pde_section_ptr_get_ಗಾತ್ರ(pd) != 1)) {
#ಇಲ್ಲದಿದ್ದರೆ
        ಆದರೆ (unlikely(pde_pde_section_ptr_get_contiguous_hint(pd) != 1)) {
#ಆದರೆ_ಅಂತ್ಯ
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (unlikely(pde_pde_section_ptr_get_ವಿಳಾಸ(pd) != addr)) {
            ಹಿಂತಿರುಗಿ;
        }

        ಗಾಗಿ (i = 0; i < SECTIONS_ಪ್ರತಿ_SUPER_SECTION; i++) {
            pd[i] = pde_pde_invalid_new(0, 0);
        }
        cleanCacheRange_PoU((ಶಬ್ದ_t)&pd[0], ಕೊನೆಯ_BYTE_PDE(pd, SECTIONS_ಪ್ರತಿ_SUPER_SECTION),
                            addrFromPPtr(&pd[0]));

        ನಿಲ್ಲಿಸು;
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid ARM page type");
        ನಿಲ್ಲಿಸು;
    }

    /* Flush the page now that the mapping has been updated */
    flush_ಪುಟ(ಪುಟ_ಗಾತ್ರ, ಕಂಡುಹಿಡಿ_ret.pd, asid, vptr);
}

ಶೂನ್ಯ flush_ಪುಟ(vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, pde_t *pd, asid_t asid, ಶಬ್ದ_t vptr)
{
    pde_t stored_hw_asid;
    ಶಬ್ದ_t ತಳ_addr;
    bool_t ಬೇರು_switched;

    ಪ್ರತಿಜ್ಞಾ((vptr & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಪುಟ_ಗಾತ್ರ))) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ಬೇರು_switched = ಗಣ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = ತುಂಬು_HWASID(asid);

    ಆದರೆ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ತಳ_addr = vptr & ~ಮುಖವಾಡ(12);

        /* Do the TLB flush */
        invalidateTranslationSingle(ತಳ_addr | pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));

        ಆದರೆ (ಬೇರು_switched) {
            ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }
}

ಶೂನ್ಯ flush_ಕೋಷ್ಟಕ(pde_t *pd, asid_t asid, ಶಬ್ದ_t vptr, pte_t *pt)
{
    pde_t stored_hw_asid;
    bool_t ಬೇರು_switched;

    ಪ್ರತಿಜ್ಞಾ((vptr & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ARMSmall_ಪುಟ_ಬಿಟ್ಟುಗಳು)) == 0);

    /* Switch to the address space to allow a cache clean by VA */
    ಬೇರು_switched = ಗಣ_VMRoot_For_Flush(pd, asid);
    stored_hw_asid = ತುಂಬು_HWASID(asid);

    ಆದರೆ (pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
        ಆದರೆ (ಬೇರು_switched) {
            ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }
}

ಶೂನ್ಯ flushSpace(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ತುಂಬು_HWASID(asid);

    /* Clean the entire data cache, to guarantee that any VAs mapped
     * in the deleted space are clean (because we can't clean by VA after
     * deleting the space) */
    cleanCaches_PoU();

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    ಆದರೆ (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ಶೂನ್ಯ invalidateTLBByASID(asid_t asid)
{
    pde_t stored_hw_asid;

    stored_hw_asid = ತುಂಬು_HWASID(asid);

    /* If the given ASID doesn't have a hardware ASID
     * assigned, then it can't have any mappings in the TLB */
    ಆದರೆ (!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* Do the TLB flush */
    invalidateTranslationASID(pde_pde_invalid_get_stored_hw_asid(stored_hw_asid));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t CONST checkVPAlignment(vm_ಪುಟ_ಗಾತ್ರ_t sz, ಶಬ್ದ_t w)
{
    ಹಿಂತಿರುಗಿ (w & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(sz))) == 0;
}

ರಚನೆ create_mappings_pte_return {
    exception_t ಸ್ಥಿತಿ_2;
    pte_t pte;
    pte_range_t pte_entries;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ create_mappings_pte_return create_mappings_pte_return_t;

ರಚನೆ create_mappings_pde_return {
    exception_t ಸ್ಥಿತಿ_2;
    pde_t pde;
    pde_range_t pde_entries;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ create_mappings_pde_return create_mappings_pde_return_t;

ಸ್ಥಿರ create_mappings_pte_return_t
create_ಸುರಕ್ಷಿತ_Mapping_Entries_PTE
(paddr_t ತಳ, ಶಬ್ದ_t vaddr, vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ,
 vm_rights_t vmRights, vm_ಗುಣಲಕ್ಷಣಗಳು_t attr, pde_t *pd)
{

    create_mappings_pte_return_t ret;
    lookupPTSlot_ret_t lu_ret;
    ಶಬ್ದ_t i;

    ಆಯ್ಕೆ (ಚೌಕಟ್ಟು_ಗಾತ್ರ) {

    ಸ್ಥಿತಿ ARMSmall_ಪುಟ:

        ret.pte_entries.ತಳ = ಖಾಲಿ; /* to avoid uninitialised warning */
        ret.pte_entries.ಉದ್ದಳತೆ = 1;

        ret.pte = makeUserPTE(ARMSmall_ಪುಟ, ತಳ,
                              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_ಪುಟ_Cacheable(attr),
                              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ =
                false;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            ಹಿಂತಿರುಗಿ ret;
        }

        ret.pte_entries.ತಳ = lu_ret.pt_ಸ್ಥಾನ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಆದರೆ (unlikely(pte_ptr_get_pteType(ret.pte_entries.ತಳ) ==
                     pte_pte_large)) {
#ಇಲ್ಲದಿದ್ದರೆ
        ಆದರೆ (unlikely(pte_ptr_get_pteType(ret.pte_entries.ತಳ) == pte_pte_small
                     && pte_pte_small_ptr_get_contiguous_hint(ret.pte_entries.ತಳ))) {
#ಆದರೆ_ಅಂತ್ಯ
            current_syscall_error.type =
                se_L4_Delete_ಮೊದಲ;

            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
            ಹಿಂತಿರುಗಿ ret;
        }

        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ ARMLarge_ಪುಟ:

        ret.pte_entries.ತಳ = ಖಾಲಿ; /* to avoid uninitialised warning */
        ret.pte_entries.ಉದ್ದಳತೆ = ಪುಟಗಳು_ಪ್ರತಿ_LARGE_ಪುಟ;

        ret.pte = makeUserPTE(ARMLarge_ಪುಟ, ತಳ,
                              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_ಪುಟ_Cacheable(attr),
                              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_Execute_Never(attr),
                              vmRights);

        lu_ret = lookupPTSlot(pd, vaddr);
        ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ =
                false;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
            /* current_lookup_fault will have been set by
             * lookupPTSlot */
            ಹಿಂತಿರುಗಿ ret;
        }

        ret.pte_entries.ತಳ = lu_ret.pt_ಸ್ಥಾನ;

        ಗಾಗಿ (i = 0; i < ಪುಟಗಳು_ಪ್ರತಿ_LARGE_ಪುಟ; i++) {
            ಆದರೆ (unlikely(pte_get_pteType(ret.pte_entries.ತಳ[i]) ==
                         pte_pte_small)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                && !pte_pte_small_get_contiguous_hint(ret.pte_entries.ತಳ[i])
#ಆದರೆ_ಅಂತ್ಯ
               ) {
                current_syscall_error.type =
                    se_L4_Delete_ಮೊದಲ;

                ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
                ಹಿಂತಿರುಗಿ ret;
            }
        }

        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid or unexpected ARM page type.");

    }
}

ಸ್ಥಿರ create_mappings_pde_return_t
create_ಸುರಕ್ಷಿತ_Mapping_Entries_PDE
(paddr_t ತಳ, ಶಬ್ದ_t vaddr, vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ,
 vm_rights_t vmRights, vm_ಗುಣಲಕ್ಷಣಗಳು_t attr, pde_t *pd)
{

    create_mappings_pde_return_t ret;
    pde_ಟ್ಯಾಗ್_t currentPDEType;
    ಶಬ್ದ_t i;

    ಆಯ್ಕೆ (ಚೌಕಟ್ಟು_ಗಾತ್ರ) {

    /* PDE mappings */
    ಸ್ಥಿತಿ ARMSection:
        ret.pde_entries.ತಳ = lookupPDSlot(pd, vaddr);
        ret.pde_entries.ಉದ್ದಳತೆ = 1;

        ret.pde = makeUserPDE(ARMSection, ತಳ,
                              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_Parity_Enabled(attr),
                              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_ಪುಟ_Cacheable(attr),
                              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        currentPDEType =
            pde_ptr_get_pdeType(ret.pde_entries.ತಳ);
        ಆದರೆ (unlikely(currentPDEType != pde_pde_invalid &&
                     (currentPDEType != pde_pde_section ||
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                      pde_pde_section_ptr_get_ಗಾತ್ರ(ret.pde_entries.ತಳ) != 0))) {
#ಇಲ್ಲದಿದ್ದರೆ
                      pde_pde_section_ptr_get_contiguous_hint(ret.pde_entries.ತಳ) != 0))) {
#ಆದರೆ_ಅಂತ್ಯ
            current_syscall_error.type =
                se_L4_Delete_ಮೊದಲ;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;

            ಹಿಂತಿರುಗಿ ret;
        }

        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ ARMSuperSection:
        ret.pde_entries.ತಳ = lookupPDSlot(pd, vaddr);
        ret.pde_entries.ಉದ್ದಳತೆ = SECTIONS_ಪ್ರತಿ_SUPER_SECTION;

        ret.pde = makeUserPDE(ARMSuperSection, ತಳ,
                              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_Parity_Enabled(attr),
                              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_ಪುಟ_Cacheable(attr),
                              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_Execute_Never(attr),
                              0,
                              vmRights);

        ಗಾಗಿ (i = 0; i < SECTIONS_ಪ್ರತಿ_SUPER_SECTION; i++) {
            currentPDEType =
                pde_get_pdeType(ret.pde_entries.ತಳ[i]);
            ಆದರೆ (unlikely(currentPDEType != pde_pde_invalid &&
                         (currentPDEType != pde_pde_section ||
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
                          pde_pde_section_get_ಗಾತ್ರ(ret.pde_entries.ತಳ[i]) != 1))) {
#ಇಲ್ಲದಿದ್ದರೆ
                          pde_pde_section_get_contiguous_hint(ret.pde_entries.ತಳ[i]) != 1))) {
#ಆದರೆ_ಅಂತ್ಯ
                current_syscall_error.type =
                    se_L4_Delete_ಮೊದಲ;
                ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;

                ಹಿಂತಿರುಗಿ ret;
            }
        }

        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid or unexpected ARM page type.");

    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ vptr_t ಪುಟ_ತಳ(vptr_t vaddr, vm_ಪುಟ_ಗಾತ್ರ_t ಗಾತ್ರ_2)
{
    ಹಿಂತಿರುಗಿ vaddr & ~ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಗಾತ್ರ_2));
}

ಸ್ಥಿರ bool_t ಶುದ್ಧ pteCheckIfMapped(pte_t *pte)
{
    ಹಿಂತಿರುಗಿ pte_ptr_get_pteType(pte) != pte_pte_invalid;
}

ಸ್ಥಿರ bool_t ಶುದ್ಧ pdeCheckIfMapped(pde_t *pde)
{
    ಹಿಂತಿರುಗಿ pde_ptr_get_pdeType(pde) != pde_pde_invalid;
}

ಸ್ಥಿರ ಶೂನ್ಯ doFlush(ಪೂರ್ಣಾಂಕ invLabel, vptr_t ಆರಂಭ, vptr_t ಕೊನೆ, paddr_t pstart)
{
    /** GHOSTUPD: "((gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state = 0
            \<or> \<acute>end - \<acute>start <= gs_get_assn cap_get_capSizeBits_'proc \<acute>ghost'state)
        \<and> \<acute>start <= \<acute>end, id)" */
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* The hypervisor does not share an AS with userspace so we must flush
         * by kernel MVA instead. ARMv7 caches are PIPT so it makes no difference */
        ಕೊನೆ = (vptr_t)paddr_ಗಮ್ಯ_pptr(pstart) + (ಕೊನೆ - ಆರಂಭ);
        ಆರಂಭ = (vptr_t)paddr_ಗಮ್ಯ_pptr(pstart);
    }
    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ ARMPDClean_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPage_Clean_ದತ್ತಾಂಶ:
        cleanCacheRange_RAM(ಆರಂಭ, ಕೊನೆ, pstart);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ARMPDInvalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPage_Invalidate_ದತ್ತಾಂಶ:
        invalidateCacheRange_RAM(ಆರಂಭ, ಕೊನೆ, pstart);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ARMPDClean_Invalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPage_Clean_Invalidate_ದತ್ತಾಂಶ:
        cleanInvalidateCacheRange_RAM(ಆರಂಭ, ಕೊನೆ, pstart);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ARMPDUnify_Instruction:
    ಸ್ಥಿತಿ ARMPageUnify_Instruction:
        /* First clean data lines to point of unification
           (L2 cache)... */
        cleanCacheRange_PoU(ಆರಂಭ, ಕೊನೆ, pstart);
        /* Ensure it's been written. */
        dsb();
        /* ...then invalidate the corresponding instruction lines
           to point of unification... */
        invalidateCacheRange_I(ಆರಂಭ, ಕೊನೆ, pstart);
        /* ...then invalidate branch predictors... */
        branchFlushRange(ಆರಂಭ, ಕೊನೆ, pstart);
        /* ... then wait for the completion of invalidations. */
        dsb();
        /* Ensure new instructions come from fresh cache lines. */
        isb();
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid operation, shouldn't get here.\n");
    }
}

/* ================= INVOCATION HANDLING STARTS HERE ================== */

ಸ್ಥಿರ exception_t performPDFlush(ಪೂರ್ಣಾಂಕ invLabel, pde_t *pd, asid_t asid, vptr_t ಆರಂಭ,
                                  vptr_t ಕೊನೆ, paddr_t pstart)
{
    bool_t ಬೇರು_switched;

    /* Flush if given a non zero range */
    ಆದರೆ (ಆರಂಭ < ಕೊನೆ) {
        ಬೇರು_switched = ಗಣ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, ಆರಂಭ, ಕೊನೆ, pstart);

        ಆದರೆ (ಬೇರು_switched) {
            ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Map(cap_t cap, cte_t *ct_ಸ್ಥಾನ,
                                                 pde_t pde, pde_t *pd_ಸ್ಥಾನ)
{
    ct_ಸ್ಥಾನ->cap = cap;
    *pd_ಸ್ಥಾನ = pde;
    cleanByVA_PoU((ಶಬ್ದ_t)pd_ಸ್ಥಾನ, addrFromPPtr(pd_ಸ್ಥಾನ));

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Unmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    ಆದರೆ (cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap));
        unmap_ಪುಟ_ಕೋಷ್ಟಕ(
            cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap),
            cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap),
            pt);
        clear_ಸ್ಮರಣೆ_PT((ಶೂನ್ಯ *)pt, cap_get_cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap));
    }
    cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ptr_ಗಣ_cap_PTIs_Mapped(&(ct_ಸ್ಥಾನ->cap), 0);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_Invocation_Map_PTE(asid_t asid, cap_t cap, cte_t *ct_ಸ್ಥಾನ, pte_t pte,
                                               pte_range_t pte_entries)
{
    ಶಬ್ದ_t i, j ಬಳಸದ;
    bool_t tlbflush_required;

    ct_ಸ್ಥಾನ->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pteCheckIfMapped(pte_entries.ತಳ);

    j = pte_entries.ಉದ್ದಳತೆ;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಶಬ್ದ_t ತಳ_ವಿಳಾಸ = pte_pte_small_get_ವಿಳಾಸ(pte);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ (i = 0; i < pte_entries.ಉದ್ದಳತೆ; i++) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        pte = pte_pte_small_ಗಣ_ವಿಳಾಸ(pte, ತಳ_ವಿಳಾಸ + i * ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSmall_ಪುಟ)));
#ಆದರೆ_ಅಂತ್ಯ
        pte_entries.ತಳ[i] = pte;
    }
    cleanCacheRange_PoU((ಶಬ್ದ_t)pte_entries.ತಳ,
                        ಕೊನೆಯ_BYTE_PTE(pte_entries.ತಳ, pte_entries.ಉದ್ದಳತೆ),
                        addrFromPPtr(pte_entries.ತಳ));
    ಆದರೆ (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_Invocation_Map_PDE(asid_t asid, cap_t cap, cte_t *ct_ಸ್ಥಾನ, pde_t pde,
                                               pde_range_t pde_entries)
{
    ಶಬ್ದ_t i, j ಬಳಸದ;
    bool_t tlbflush_required;

    ct_ಸ್ಥಾನ->cap = cap;

    /* we only need to check the first entries because of how createSafeMappingEntries
     * works to preserve the consistency of tables */
    tlbflush_required = pdeCheckIfMapped(pde_entries.ತಳ);

    j = pde_entries.ಉದ್ದಳತೆ;
    /** GHOSTUPD: "(\<acute>j <= 16, id)" */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಶಬ್ದ_t ತಳ_ವಿಳಾಸ = pde_pde_section_get_ವಿಳಾಸ(pde);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ (i = 0; i < pde_entries.ಉದ್ದಳತೆ; i++) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        pde = pde_pde_section_ಗಣ_ವಿಳಾಸ(pde, ತಳ_ವಿಳಾಸ + i * ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection)));
#ಆದರೆ_ಅಂತ್ಯ
        pde_entries.ತಳ[i] = pde;
    }
    cleanCacheRange_PoU((ಶಬ್ದ_t)pde_entries.ತಳ,
                        ಕೊನೆಯ_BYTE_PDE(pde_entries.ತಳ, pde_entries.ಉದ್ದಳತೆ),
                        addrFromPPtr(pde_entries.ತಳ));
    ಆದರೆ (unlikely(tlbflush_required)) {
        invalidateTLBByASID(asid);
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_Invocation_Unmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    ಆದರೆ (generic_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Mapped(cap)) {
        unmap_ಪುಟ(generic_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
                  generic_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                  generic_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                  (ಶೂನ್ಯ *)generic_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));
    }

    generic_ಚೌಕಟ್ಟು_cap_ptr_ಗಣ_cap_FMapped_ವಿಳಾಸ(&ct_ಸ್ಥಾನ->cap, asidInvalid, 0);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_Flush(ಪೂರ್ಣಾಂಕ invLabel, pde_t *pd, asid_t asid, vptr_t ಆರಂಭ,
                                    vptr_t ಕೊನೆ, paddr_t pstart)
{
    bool_t ಬೇರು_switched;

    /* now we can flush. But only if we were given a non zero range */
    ಆದರೆ (ಆರಂಭ < ಕೊನೆ) {
        ಬೇರು_switched = ಗಣ_VMRoot_For_Flush(pd, asid);

        doFlush(invLabel, ಆರಂಭ, ಕೊನೆ, pstart);

        ಆದರೆ (ಬೇರು_switched) {
            ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_ಪುಟ_Get_ವಿಳಾಸ(ಶೂನ್ಯ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    ಆದರೆ (call) {
        ಶಬ್ದ_t *ipc_ಬಫರ್ = lookupIPCBuffer(true, thread);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ಗಣ_MR(thread, ipc_ಬಫರ್, 0, capFBasePtr);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr,
                                             cte_t *pd_Cap_ಸ್ಥಾನ)
{
    cap_ಪುಟ_directory_cap_ptr_ಗಣ_cap_PDMapped_ASID(&pd_Cap_ಸ್ಥಾನ->cap, asid);
    cap_ಪುಟ_directory_cap_ptr_ಗಣ_cap_PDIs_Mapped(&pd_Cap_ಸ್ಥಾನ->cap, 1);
    poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] =
        PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(pd_Cap_ಸ್ಥಾನ->cap));

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performASIDControlInvocation(ಶೂನ್ಯ *ಚೌಕಟ್ಟು, cte_t *ಸ್ಥಾನ,
                                                cte_t *ಪೋಷಕ, asid_t asid_ತಳ)
{

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_ಗಣ_cap_Free_ಸೂಚ್ಯಂಕ(&(ಪೋಷಕ->cap),
                                         MAX_FREE_ಸೂಚ್ಯಂಕ(cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(ಪೋಷಕ->cap)));

    memzero(ಚೌಕಟ್ಟು, 1 << ARMSmall_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ಸೇರಿಸು(cap_asid_pool_cap_new(asid_ತಳ, ಶಬ್ದ_REF(ಚೌಕಟ್ಟು)),
              ಪೋಷಕ, ಸ್ಥಾನ);;
    /* Haskell error: "ASID pool's base must be aligned" */
    ಪ್ರತಿಜ್ಞಾ((asid_ತಳ & ಮುಖವಾಡ(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು)) == 0);
    armKSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] = (asid_pool_t *)ಚೌಕಟ್ಟು;

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeARMPageDirectoryInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                                    cptr_t cptr, cte_t *cte, cap_t cap,
                                                    ಶಬ್ದ_t *ಬಫರ್)
{
    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ ARMPDClean_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPDInvalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPDClean_Invalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPDUnify_Instruction: {
        vptr_t ಆರಂಭ, ಕೊನೆ;
        paddr_t pstart;
        ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
        asid_t asid;
        pde_t *pd;
        resolve_ret_t resolve_ret;

        ಆದರೆ (ಉದ್ದಳತೆ < 2) {
            userError("PD Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆರಂಭ = getSyscallArg(0, ಬಫರ್);
        ಕೊನೆ =   getSyscallArg(1, ಬಫರ್);

        /* Check sanity of arguments */
        ಆದರೆ (ಕೊನೆ <= ಆರಂಭ) {
            userError("PD Flush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* Don't let applications flush kernel regions. */
        ಆದರೆ (ಆರಂಭ >= USER_ತುದಿ || ಕೊನೆ > USER_ತುದಿ) {
            userError("PD Flush: Overlaps kernel region.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(cap_get_capType(cap) != cap_ಪುಟ_directory_cap ||
                     !cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(cap))) {
            userError("PD Flush: Invalid cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }


        /* Make sure that the supplied pd is ok */
        pd = PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(cap));
        asid = cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(cap);

        ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(asid);
        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("PD Flush: No PD for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.pd != pd)) {
            userError("PD Flush: Invalid PD Cap");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* Look up the frame containing 'start'. */
        resolve_ret = resolveVAddr(pd, ಆರಂಭ);

        /* Check that there's actually something there. */
        ಆದರೆ (!resolve_ret.valid) {
            /* Fail silently, as there can't be any stale cached data (for the
             * given address space), and getting a syscall error because the
             * relevant page is non-resident would be 'astonishing'. */
            ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
        }

        /* Refuse to cross a page boundary. */
        ಆದರೆ (ಪುಟ_ತಳ(ಆರಂಭ, resolve_ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ) !=
            ಪುಟ_ತಳ(ಕೊನೆ - 1, resolve_ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ)) {
            userError("PD Flush: Range is across page boundary.");
            current_syscall_error.type = seL4_RangeError;
            current_syscall_error.rangeErrorMin = ಆರಂಭ;
            current_syscall_error.rangeErrorMax =
                ಪುಟ_ತಳ(ಆರಂಭ, resolve_ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ) +
                ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(resolve_ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ));
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }


        /* Calculate the physical start address. */
        pstart = resolve_ret.ಚೌಕಟ್ಟು_ತಳ
                 + (ಆರಂಭ & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(resolve_ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ)));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        ಆದರೆ (pstart < phys_ತಳ() || pstart > PADDR_ತುದಿ || (ಕೊನೆ - ಆರಂಭ) > PADDR_ತುದಿ - pstart) {
            userError("PD Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performPDFlush(invLabel, pd, asid, ಆರಂಭ, ಕೊನೆ - 1, pstart);
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("PD: Invalid invocation number");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

}

ಸ್ಥಿರ exception_t decode_ARMPage_ಕೋಷ್ಟಕ_Invocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                                cte_t *cte, cap_t cap, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t vaddr, pd_ಸೂಚ್ಯಂಕ;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    vm_ಗುಣಲಕ್ಷಣಗಳು_t attr;
#ಆದರೆ_ಅಂತ್ಯ
    cap_t pdCap;
    pde_t *pd, *pd_ಸ್ಥಾನ;
    pde_t pde;
    asid_t asid;
    paddr_t paddr;

    ಆದರೆ (invLabel == ARMPage_ಕೋಷ್ಟಕ_Unmap) {
        ಆದರೆ (unlikely(! isFinalCapability(cte))) {
            userError("ARMPageTableUnmap: Cannot unmap if more than one cap exists.");
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Unmap(cap, cte);
    }

    ಆದರೆ (unlikely(invLabel != ARMPage_ಕೋಷ್ಟಕ_Map)) {
        userError("ARMPageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
        userError("ARMPageTableMap: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap))) {
        userError("ARMPageTableMap: Page table is already mapped to page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ಬಫರ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    attr = vm_ಗುಣಲಕ್ಷಣಗಳು_From_ಶಬ್ದ(getSyscallArg(1, ಬಫರ್));
#ಆದರೆ_ಅಂತ್ಯ
    pdCap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (unlikely(cap_get_capType(pdCap) != cap_ಪುಟ_directory_cap ||
                 !cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
        userError("ARMPageTableMap: Invalid PD cap.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pd = PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(pdCap));
    asid = cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(pdCap);

    ಆದರೆ (unlikely(vaddr >= USER_ತುದಿ)) {
        userError("ARMPageTableMap: Virtual address cannot be in kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr,
                  USER_ತುದಿ);
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    {
        ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

        ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(asid);
        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.pd != pd)) {
            userError("ARMPageTableMap: ASID lookup failed.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_ಸೂಚ್ಯಂಕ = vaddr >> (ಪುಟ_ಬಿಟ್ಟುಗಳು + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು);
    pd_ಸ್ಥಾನ = &pd[pd_ಸೂಚ್ಯಂಕ];
    ಆದರೆ (unlikely(pde_ptr_get_pdeType(pd_ಸ್ಥಾನ) != pde_pde_invalid)) {
        userError("ARMPageTableMap: Page directory already has entry for supplied address.");
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = addrFromPPtr(
                PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap)));
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    pde = pde_pde_coarse_new(
              paddr,
              vm_ಗುಣಲಕ್ಷಣಗಳು_get_arm_Parity_Enabled(attr),
              0 /* Domain */
          );
#ಇಲ್ಲದಿದ್ದರೆ
    pde = pde_pde_coarse_new(paddr);
#ಆದರೆ_ಅಂತ್ಯ

    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTMapped_ವಿಳಾಸ(cap, vaddr);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Map(cap, cte, pde, pd_ಸ್ಥಾನ);
}

ಸ್ಥಿರ exception_t decodeARMFrameInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                            cte_t *cte, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ ARMPageMap: {
        ಶಬ್ದ_t vaddr, vtop, w_rights_ಮುಖವಾಡ;
        paddr_t capFBasePtr;
        cap_t pdCap;
        pde_t *pd;
        asid_t asid;
        vm_rights_t capVMRights, vmRights;
        vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ;
        vm_ಗುಣಲಕ್ಷಣಗಳು_t attr;

        ಆದರೆ (unlikely(ಉದ್ದಳತೆ < 3 || current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
            userError("ARMPageMap: Truncated message.");
            current_syscall_error.type =
                se_L4_Truncated_ಸಂದೇಶ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        vaddr = getSyscallArg(0, ಬಫರ್);
        w_rights_ಮುಖವಾಡ = getSyscallArg(1, ಬಫರ್);
        attr = vm_ಗುಣಲಕ್ಷಣಗಳು_From_ಶಬ್ದ(getSyscallArg(2, ಬಫರ್));
        pdCap = current_extra_caps.excaprefs[0]->cap;

        ಚೌಕಟ್ಟು_ಗಾತ್ರ = generic_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap);
        capVMRights = generic_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap);

        ಆದರೆ (unlikely(cap_get_capType(pdCap) != cap_ಪುಟ_directory_cap ||
                     !cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ARMPageMap: Bad PageDirectory cap.");
            current_syscall_error.type =
                seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        pd = PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(
                         pdCap));
        asid = cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(pdCap);

        ಆದರೆ (generic_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Mapped(cap)) {
            ಆದರೆ (generic_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }

            ಆದರೆ (generic_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap) != vaddr) {
                userError("ARMPageMap: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_ವಾದ;
                current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            vtop = vaddr + ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಚೌಕಟ್ಟು_ಗಾತ್ರ)) - 1;

            ಆದರೆ (unlikely(vtop >= USER_ತುದಿ)) {
                userError("ARMPageMap: Cannot map frame over kernel window. vaddr: 0x%08lx, USER_TOP: 0x%08x", vaddr, USER_ತುದಿ);
                current_syscall_error.type =
                    se_L4_Invalid_ವಾದ;
                current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

            ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_PDFor_ASID(asid);
            ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
                userError("ARMPageMap: No PD for ASID");
                current_syscall_error.type =
                    seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_ಮೂಲ =
                    false;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }

            ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.pd != pd)) {
                userError("ARMPageMap: ASID lookup failed.");
                current_syscall_error.type =
                    seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights =
            ಮುಖವಾಡ_VMRights(capVMRights, rights_From_ಶಬ್ದ(w_rights_ಮುಖವಾಡ));

        ಆದರೆ (unlikely(!checkVPAlignment(ಚೌಕಟ್ಟು_ಗಾತ್ರ, vaddr))) {
            userError("ARMPageMap: Virtual address has incorrect alignment.");
            current_syscall_error.type =
                seL4_AlignmentError;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        capFBasePtr = addrFromPPtr((ಶೂನ್ಯ *)
                                   generic_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));

        cap = generic_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(cap, asid,
                                                      vaddr);
        ಆದರೆ (ಚೌಕಟ್ಟು_ಗಾತ್ರ == ARMSmall_ಪುಟ || ಚೌಕಟ್ಟು_ಗಾತ್ರ == ARMLarge_ಪುಟ) {
            create_mappings_pte_return_t map_ret;
            map_ret = create_ಸುರಕ್ಷಿತ_Mapping_Entries_PTE(capFBasePtr, vaddr,
                                                   ಚೌಕಟ್ಟು_ಗಾತ್ರ, vmRights,
                                                   attr, pd);
            ಆದರೆ (unlikely(map_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
                ಆದರೆ (current_syscall_error.type == se_L4_Delete_ಮೊದಲ) {
                    userError("ARMPageMap: Page table entry was not free.");
                }
#ಆದರೆ_ಅಂತ್ಯ
                ಹಿಂತಿರುಗಿ map_ret.ಸ್ಥಿತಿ_2;
            }

            ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಹಿಂತಿರುಗಿ perform_ಪುಟ_Invocation_Map_PTE(asid, cap, cte,
                                               map_ret.pte,
                                               map_ret.pte_entries);
        } ಇಲ್ಲದಿದ್ದರೆ {
            create_mappings_pde_return_t map_ret;
            map_ret = create_ಸುರಕ್ಷಿತ_Mapping_Entries_PDE(capFBasePtr, vaddr,
                                                   ಚೌಕಟ್ಟು_ಗಾತ್ರ, vmRights,
                                                   attr, pd);
            ಆದರೆ (unlikely(map_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
                ಆದರೆ (current_syscall_error.type == se_L4_Delete_ಮೊದಲ) {
                    userError("ARMPageMap: Page directory entry was not free.");
                }
#ಆದರೆ_ಅಂತ್ಯ
                ಹಿಂತಿರುಗಿ map_ret.ಸ್ಥಿತಿ_2;
            }

            ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಹಿಂತಿರುಗಿ perform_ಪುಟ_Invocation_Map_PDE(asid, cap, cte,
                                               map_ret.pde,
                                               map_ret.pde_entries);
        }
    }

    ಸ್ಥಿತಿ ARMPageUnmap: {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
        ಆದರೆ (is_IOSpace_ಚೌಕಟ್ಟು_Cap(cap)) {
            ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಹಿಂತಿರುಗಿ perform_ಪುಟ_Invocation_Unmap_IO(cap, cte);
        } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
        {
            ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಹಿಂತಿರುಗಿ perform_ಪುಟ_Invocation_Unmap(cap, cte);
        }
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಸ್ಥಿತಿ ARMPageMapIO: {
        ಹಿಂತಿರುಗಿ decodeARMIOMapInvocation(invLabel, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ ARMPage_Clean_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPage_Invalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPage_Clean_Invalidate_ದತ್ತಾಂಶ:
    ಸ್ಥಿತಿ ARMPageUnify_Instruction: {
        asid_t asid;
        vptr_t vaddr;
        ಕಂಡುಹಿಡಿ_PDFor_ASID_ret_t pd;
        vptr_t ಆರಂಭ, ಕೊನೆ;
        paddr_t pstart;
        ಶಬ್ದ_t ಪುಟ_ಗಾತ್ರ;
        ಶಬ್ದ_t ಪುಟ_ತಳ_2;

        ಆದರೆ (ಉದ್ದಳತೆ < 2) {
            userError("Page Flush: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        asid = generic_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Must use kernel vaddr in hyp mode. */
        vaddr = generic_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap);
#ಇಲ್ಲದಿದ್ದರೆ
        vaddr = generic_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap);
#ಆದರೆ_ಅಂತ್ಯ

        ಆದರೆ (unlikely(!generic_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Mapped(cap))) {
            userError("Page Flush: Frame is not mapped.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        pd = ಕಂಡುಹಿಡಿ_PDFor_ASID(asid);
        ಆದರೆ (unlikely(pd.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("Page Flush: No PD for ASID");
            current_syscall_error.type =
                seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆರಂಭ = getSyscallArg(0, ಬಫರ್);
        ಕೊನೆ =   getSyscallArg(1, ಬಫರ್);

        /* check that the range is sane */
        ಆದರೆ (ಕೊನೆ <= ಆರಂಭ) {
            userError("PageFlush: Invalid range");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }


        /* start and end are currently relative inside this page */
        ಪುಟ_ಗಾತ್ರ = 1 << ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(generic_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap));
        ಪುಟ_ತಳ_2 = addrFromPPtr((ಶೂನ್ಯ *)generic_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* When not in hypervisor mode, we flush via user virtual addresses and
           need to make sure the mapping info in the cap is not stale. */
        resolve_ret_t resolve_ret = resolveVAddr(pd.pd, vaddr);
        ಆದರೆ (unlikely(!resolve_ret.valid ||
                     resolve_ret.ಚೌಕಟ್ಟು_ಗಾತ್ರ != generic_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap) ||
                     resolve_ret.ಚೌಕಟ್ಟು_ತಳ != ಪುಟ_ತಳ_2)) {
            userError("Page Flush: Attempting to use cap with stale mapping information.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಆದರೆ (ಆರಂಭ >= ಪುಟ_ಗಾತ್ರ || ಕೊನೆ > ಪುಟ_ಗಾತ್ರ) {
            userError("Page Flush: Requested range not inside page");
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* turn start and end into absolute addresses */
        pstart = ಪುಟ_ತಳ_2 + ಆರಂಭ;
        ಆರಂಭ += vaddr;
        ಕೊನೆ += vaddr;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Don't let applications flush outside of the kernel window.
           pstart + (end - start) can overflow at the top of the address space. */
        ಆದರೆ (pstart < phys_ತಳ() || pstart > PADDR_ತುದಿ || (ಕೊನೆ - ಆರಂಭ) > PADDR_ತುದಿ - pstart) {
            userError("Page Flush: Outside of kernel physical memory window");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_Flush(invLabel, pd.pd, asid, ಆರಂಭ, ಕೊನೆ - 1, pstart);
    }

    ಸ್ಥಿತಿ ARMPage_Get_ವಿಳಾಸ: {


        /* Check that there are enough message registers */
        ಪ್ರತಿಜ್ಞಾ(n_ಸಂದೇಶ_Registers >= 1);

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_Get_ವಿಳಾಸ((ಶೂನ್ಯ *)generic_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap), call);
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("ARMPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t decodeARMMMUInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ, cptr_t cptr,
                                   cte_t *cte, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
        ಹಿಂತಿರುಗಿ decodeARMPageDirectoryInvocation(invLabel, ಉದ್ದಳತೆ, cptr, cte,
                                                cap, ಬಫರ್);

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಹಿಂತಿರುಗಿ decode_ARMPage_ಕೋಷ್ಟಕ_Invocation(invLabel, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);

    ಸ್ಥಿತಿ cap_small_ಚೌಕಟ್ಟು_cap:
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಹಿಂತಿರುಗಿ decodeARMFrameInvocation(invLabel, ಉದ್ದಳತೆ, cte, cap, call, ಬಫರ್);

    ಸ್ಥಿತಿ cap_asid_control_cap: {
        ಶಬ್ದ_t i;
        asid_t asid_ತಳ;
        ಶಬ್ದ_t ಸೂಚ್ಯಂಕ, ಆಳ;
        cap_t untyped, ಬೇರು;
        cte_t *ಪೋಷಕ_ಸ್ಥಾನ, *dest_ಸ್ಥಾನ;
        lookup_ಸ್ಥಾನ_ret_t lu_ret;
        ಶೂನ್ಯ *ಚೌಕಟ್ಟು;
        exception_t ಸ್ಥಿತಿ_2;

        ಆದರೆ (unlikely(invLabel != ARMASIDControlMakePool)) {
            userError("ASIDControl: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ
                     || current_extra_caps.excaprefs[1] == ಖಾಲಿ)) {
            userError("ASIDControlMakePool: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಸೂಚ್ಯಂಕ = getSyscallArg(0, ಬಫರ್);
        ಆಳ = getSyscallArg(1, ಬಫರ್);
        ಪೋಷಕ_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
        untyped = ಪೋಷಕ_ಸ್ಥಾನ->cap;
        ಬೇರು = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        ಗಾಗಿ (i = 0; i < nASIDPools && armKSASIDTable[i]; i++);

        ಆದರೆ (unlikely(i == nASIDPools)) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        asid_ತಳ = i << asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು;

        ಆದರೆ (unlikely(cap_get_capType(untyped) != cap_untyped_cap ||
                     cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(untyped) !=
                     se_L4_ASIDPool_ಬಿಟ್ಟುಗಳು || cap_untyped_cap_get_capIsDevice(untyped))) {
            userError("ASIDControlMakePool: Invalid untyped cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಸ್ಥಿತಿ_2 = ensureNoChildren(ಪೋಷಕ_ಸ್ಥಾನ);
        ಆದರೆ (unlikely(ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Untyped has children. Revoke first.");
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಚೌಕಟ್ಟು = ಶಬ್ದ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(ಬೇರು, ಸೂಚ್ಯಂಕ, ಆಳ);
        ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Failed to lookup destination slot.");
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }
        dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (unlikely(ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("ASIDControlMakePool: Destination slot not empty.");
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performASIDControlInvocation(ಚೌಕಟ್ಟು, dest_ಸ್ಥಾನ,
                                            ಪೋಷಕ_ಸ್ಥಾನ, asid_ತಳ);
    }

    ಸ್ಥಿತಿ cap_asid_pool_cap: {
        cap_t pdCap;
        cte_t *pd_Cap_ಸ್ಥಾನ;
        asid_pool_t *pool;
        ಶಬ್ದ_t i;
        asid_t asid;

        ಆದರೆ (unlikely(invLabel != ARMASIDPool_ನಿಯೋಜಿಸು)) {
            userError("ASIDPool: Illegal operation.");
            current_syscall_error.type = seL4_IllegalOperation;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
            userError("ASIDPoolAssign: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        pd_Cap_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
        pdCap = pd_Cap_ಸ್ಥಾನ->cap;

        ಆದರೆ (unlikely(
                cap_get_capType(pdCap) != cap_ಪುಟ_directory_cap ||
                cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(pdCap))) {
            userError("ASIDPoolAssign: Invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        pool = armKSASIDTable[ASID_ಮೇಲಿನ(cap_asid_pool_cap_get_capASIDBase(cap))];
        ಆದರೆ (unlikely(!pool)) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)))) {
            userError("ASIDPoolAssign: Failed to lookup pool.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        ಗಾಗಿ (i = 0; i < (1 << asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು) && (asid + i == 0 || pool->array[i]); i++);

        ಆದರೆ (unlikely(i == 1 << asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು)) {
            userError("ASIDPoolAssign: No free ASID.");
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performASIDPoolInvocation(asid, pool, pd_Cap_ಸ್ಥಾನ);
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid ARM arch cap type");
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
exception_t benchmark_arch_map_log_ಬಫರ್(ಶಬ್ದ_t ಚೌಕಟ್ಟು_cptr)
{
    lookup_Cap_And_ಸ್ಥಾನ_ret_t lu_ret;
    vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ;
    pptr_t  ಚೌಕಟ್ಟು_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_ಸ್ಥಾನ(ಗಂಟು_STATE(ksCurThread), ಚೌಕಟ್ಟು_cptr);

    ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", ಚೌಕಟ್ಟು_cptr);
        current_fault = seL4_Fault_CapFault_new(ಚೌಕಟ್ಟು_cptr, false);

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_get_capType(lu_ret.cap) != cap_ಚೌಕಟ್ಟು_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(ಚೌಕಟ್ಟು_cptr, false);

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಚೌಕಟ್ಟು_ಗಾತ್ರ = generic_ಚೌಕಟ್ಟು_cap_get_cap_FSize(lu_ret.cap);

    ಆದರೆ (ಚೌಕಟ್ಟು_ಗಾತ್ರ != ARMSection) {
        userError("Invalid frame size. The kernel expects 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(ಚೌಕಟ್ಟು_cptr, false);

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಚೌಕಟ್ಟು_pptr = generic_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_ಬಫರ್ = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *) ಚೌಕಟ್ಟು_pptr);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); idx++) {
        paddr_t physical_ವಿಳಾಸ = ks_User_Log_ಬಫರ್ + (idx << se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು);

        armKSGlobalLogPT[idx] =
            pte_pte_small_new(
                physical_ವಿಳಾಸ,
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
        invalidateTranslationSingle(KS_LOG_PPTR + (idx * ಬಿಟ್(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು)));
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
ಶೂನ್ಯ kernelPrefetchAbort(ಶಬ್ದ_t pc, ಶಬ್ದ_t lr) VISIBLE;
ಶೂನ್ಯ kernel_ದತ್ತಾಂಶ_Abort(ಶಬ್ದ_t pc) VISIBLE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT

ಶೂನ್ಯ kernelUndefinedInstruction(ಶಬ್ದ_t pc) VISIBLE;

ಶೂನ್ಯ kernelPrefetchAbort(ಶಬ್ದ_t pc, ಶಬ್ದ_t lr)
{
    ಬರೆ("\n\nKERNEL PREFETCH ABORT!\n");
    ಬರೆ("Faulting instruction: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", pc);
    ಬರೆ("HSR: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", getHSR());
    ಬರೆ("LR: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", lr);
    halt();
}

ಶೂನ್ಯ kernel_ದತ್ತಾಂಶ_Abort(ಶಬ್ದ_t pc)
{
    ಬರೆ("\n\nKERNEL DATA ABORT!\n");
    ಬರೆ("Faulting instruction: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", pc);
    ಬರೆ("HDFAR: 0x%"SEL4_PRIx_ಶಬ್ದ" HSR: 0x%"SEL4_PRIx_ಶಬ್ದ"\n",
           getHDFAR(), getHSR());
    halt();
}

ಶೂನ್ಯ kernelUndefinedInstruction(ಶಬ್ದ_t pc)
{
    ಬರೆ("\n\nKERNEL UNDEFINED INSTRUCTION!\n");
    ಬರೆ("Faulting instruction: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", pc);
    ಬರೆ("HSR: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", getHSR());
    halt();
}

#ಇಲ್ಲದಿದ್ದರೆ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

ಶೂನ್ಯ kernelPrefetchAbort(ಶಬ್ದ_t pc, ಶಬ್ದ_t lr)
{
    ಬರೆ("\n\nKERNEL PREFETCH ABORT!\n");
    ಬರೆ("Faulting instruction: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", pc);
    ಬರೆ("IFSR: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", getIFSR());
    ಬರೆ("LR: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", lr);
    halt();
}

ಶೂನ್ಯ kernel_ದತ್ತಾಂಶ_Abort(ಶಬ್ದ_t pc)
{
    ಬರೆ("\n\nKERNEL DATA ABORT!\n");
    ಬರೆ("Faulting instruction: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", pc);
    ಬರೆ("FAR: 0x%"SEL4_PRIx_ಶಬ್ದ" DFSR: 0x%"SEL4_PRIx_ಶಬ್ದ"\n",
           getFAR(), getDFSR());
    halt();
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ read_ಶಬ್ದ_From_VSpace_ret {
    exception_t ಸ್ಥಿತಿ_2;
    ಶಬ್ದ_t ಮೌಲ್ಯ;
} read_ಶಬ್ದ_From_VSpace_ret_t;

ಸ್ಥಿರ read_ಶಬ್ದ_From_VSpace_ret_t read_ಶಬ್ದ_From_VSpace(pde_t *pd, ಶಬ್ದ_t vaddr)
{
    read_ಶಬ್ದ_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_ಸ್ಥಾನ;
    pde_t *pd_ಸ್ಥಾನ;
    paddr_t paddr;
    ಶಬ್ದ_t ಸ್ಥಾನಾಂತರ;
    pptr_t kernel_vaddr;
    ಶಬ್ದ_t *ಮೌಲ್ಯ;

    pd_ಸ್ಥಾನ = lookupPDSlot(pd, vaddr);
    ಆದರೆ (pde_ptr_get_pdeType(pd_ಸ್ಥಾನ) == pde_pde_section) {
        paddr = pde_pde_section_ptr_get_ವಿಳಾಸ(pd_ಸ್ಥಾನ);
        ಸ್ಥಾನಾಂತರ = vaddr & ಮುಖವಾಡ(ARMSection_ಬಿಟ್ಟುಗಳು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        pt_ಸ್ಥಾನ = lookupPTSlot(pd, vaddr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        ಆದರೆ (pt_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE && pte_ptr_get_pteType(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_ವಿಳಾಸ(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ);
            ಆದರೆ (pte_pte_small_ptr_get_contiguous_hint(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ)) {
                ಸ್ಥಾನಾಂತರ = vaddr & ಮುಖವಾಡ(ARMLarge_ಪುಟ_ಬಿಟ್ಟುಗಳು);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಸ್ಥಾನಾಂತರ = vaddr & ಮುಖವಾಡ(ARMSmall_ಪುಟ_ಬಿಟ್ಟುಗಳು);
            }
#ಇಲ್ಲದಿದ್ದರೆ
        ಆದರೆ (pt_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE && pte_ptr_get_pteType(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ) == pte_pte_small) {
            paddr = pte_pte_small_ptr_get_ವಿಳಾಸ(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ);
            ಸ್ಥಾನಾಂತರ = vaddr & ಮುಖವಾಡ(ARMSmall_ಪುಟ_ಬಿಟ್ಟುಗಳು);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pt_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE && pte_ptr_get_pteType(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ) == pte_pte_large) {
            paddr = pte_pte_large_ptr_get_ವಿಳಾಸ(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ);
            ಸ್ಥಾನಾಂತರ = vaddr & ಮುಖವಾಡ(ARMLarge_ಪುಟ_ಬಿಟ್ಟುಗಳು);
#ಆದರೆ_ಅಂತ್ಯ
        } ಇಲ್ಲದಿದ್ದರೆ {
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
            ಹಿಂತಿರುಗಿ ret;
        }
    }


    kernel_vaddr = (ಶಬ್ದ_t)paddr_ಗಮ್ಯ_pptr(paddr);
    ಮೌಲ್ಯ = (ಶಬ್ದ_t *)(kernel_vaddr + ಸ್ಥಾನಾಂತರ);
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ret.ಮೌಲ್ಯ = *ಮೌಲ್ಯ;
    ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ Arch_user_ರಾಶಿ_Trace(tcb_t *tptr)
{
    cap_t thread_ಬೇರು;
    pde_t *pd;
    ಶಬ್ದ_t sp;
    ಪೂರ್ಣಾಂಕ i;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    ಆದರೆ (cap_get_capType(thread_ಬೇರು) != cap_ಪುಟ_directory_cap) {
        ಬರೆ("Invalid vspace\n");
        ಹಿಂತಿರುಗಿ;
    }

    pd = (pde_t *)pptr_ದ_cap(thread_ಬೇರು);

    sp = get_ರಿಜಿಸ್ಟರ್(tptr, SP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ಆದರೆ (!IS_ALIGNED(sp, se_L4_ಶಬ್ದ_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        ಬರೆ("SP not aligned\n");
        ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (i = 0; i < CONFIG_USER_ರಾಶಿ_TRACE_ಉದ್ದಳತೆ; i++) {
        ಶಬ್ದ_t ವಿಳಾಸ = sp + (i * ಗಾತ್ರ(ಶಬ್ದ_t));
        read_ಶಬ್ದ_From_VSpace_ret_t ಫಲಿತಾಂಶ;
        ಫಲಿತಾಂಶ = read_ಶಬ್ದ_From_VSpace(pd, ವಿಳಾಸ);
        ಆದರೆ (ಫಲಿತಾಂಶ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE) {
            ಬರೆ("0x%lx: 0x%lx\n", (ದೀರ್ಘ)ವಿಳಾಸ, (ದೀರ್ಘ)ಫಲಿತಾಂಶ.ಮೌಲ್ಯ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("0x%lx: INVALID\n", (ದೀರ್ಘ)ವಿಳಾಸ);
        }
    }
}
#ಆದರೆ_ಅಂತ್ಯ
