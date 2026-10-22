/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <object/tcb.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <model/preemption.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <object/untyped.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <kernel/stack.h>
#ಅನ್ತರ್ಭಾವ <util.h>

ರಚನೆ resolve_ret {
    paddr_t ಚೌಕಟ್ಟು_ತಳ;
    vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ;
    bool_t valid;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ resolve_ret resolve_ret_t;

ಸ್ಥಿರ exception_t perform_ಪುಟ_Get_ವಿಳಾಸ(ಶೂನ್ಯ *vbase_ptr, bool_t call);

ಸ್ಥಿರ ಶಬ್ದ_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    ಹಿಂತಿರುಗಿ vm_rights == VMReadWrite;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported.
     * Kernel-only conveys no user rights. */
    ಹಿಂತಿರುಗಿ vm_rights != VMKernelOnly;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t is_PTEPage_ಕೋಷ್ಟಕ(pte_t *pte)
{
    ಹಿಂತಿರುಗಿ pte_ptr_get_valid(pte) &&
           !(pte_ptr_get_read(pte) || pte_ptr_get_write(pte) || pte_ptr_get_execute(pte));
}

/** Helper function meant only to be used for mapping the kernel
 * window.
 *
 * Maps all pages with full RWX and supervisor perms by default.
 */
ಸ್ಥಿರ pte_t pte_ಮುಂದಿನ(ಶಬ್ದ_t phys_addr, bool_t is_ಎಲೆ)
{
    ಶಬ್ದ_t ppn = (ಶಬ್ದ_t)(phys_addr >> 12);

    uint8_t ಪಠ = is_ಎಲೆ ? 1 : 0;
    uint8_t ಲಿಖ = ಪಠ;
    uint8_t exec = ಪಠ;

    ಹಿಂತಿರುಗಿ pte_new(ppn,
                   0,     /* sw */
                   is_ಎಲೆ ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_ಎಲೆ ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_ಎಲೆ ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   ಲಿಖ, /* write */
                   ಪಠ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_kernel_ಚೌಕಟ್ಟು(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#ಆದರೆ __riscv_xlen == 32
    paddr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(paddr, RISCV_GET_LVL_PGSIZE_ಬಿಟ್ಟುಗಳು(0));
    ಪ್ರತಿಜ್ಞಾ((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ[RISCV_GET_PT_ಸೂಚ್ಯಂಕ(vaddr, 0)] = pte_ಮುಂದಿನ(paddr, true);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (vaddr >= KDEV_ತಳ) {
        /* Map devices in level 1 page table */
        paddr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(paddr, RISCV_GET_LVL_PGSIZE_ಬಿಟ್ಟುಗಳು(1));
        ಪ್ರತಿಜ್ಞಾ((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_ಸೂಚ್ಯಂಕ(vaddr, 1)] = pte_ಮುಂದಿನ(paddr, true);
    } ಇಲ್ಲದಿದ್ದರೆ {
        paddr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(paddr, RISCV_GET_LVL_PGSIZE_ಬಿಟ್ಟುಗಳು(0));
        ಪ್ರತಿಜ್ಞಾ((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ[RISCV_GET_PT_ಸೂಚ್ಯಂಕ(vaddr, 0)] = pte_ಮುಂದಿನ(paddr, true);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

BOOT_ಸಂಕೇತ VISIBLE ಶೂನ್ಯ map_kernel_ಕಿಟಕಿ(ಶೂನ್ಯ)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    ಪ್ರತಿಜ್ಞಾ(CONFIG_PT_ಮಟ್ಟಗಳು > 1 && CONFIG_PT_ಮಟ್ಟಗಳು <= 4);
    /* Kernel image finishes before KDEV_BASE */
    ಪ್ರತಿಜ್ಞಾ(KERNEL_ELF_ತುದಿ <= KDEV_ತಳ);

    /* kernel window starts at PPTR_BASE */
    ಶಬ್ದ_t pptr = PPTR_ತಳ;

    /* first we map in memory from PADDR_BASE */
    ಶಬ್ದ_t paddr = PADDR_ತಳ;
    ಆಗ (pptr < PPTR_ತುದಿ) {
        ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_ಬಿಟ್ಟುಗಳು(0)));
        ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_ಬಿಟ್ಟುಗಳು(0)));

        kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ[RISCV_GET_PT_ಸೂಚ್ಯಂಕ(pptr, 0)] = pte_ಮುಂದಿನ(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    ಪ್ರತಿಜ್ಞಾ(pptr == PPTR_ತುದಿ);
    pptr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(KERNEL_ELF_ತಳ, RISCV_GET_LVL_PGSIZE_ಬಿಟ್ಟುಗಳು(0));
    paddr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(KERNEL_ELF_PADDR_ತಳ, RISCV_GET_LVL_PGSIZE_ಬಿಟ್ಟುಗಳು(0));

#ಆದರೆ __riscv_xlen == 32
    kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ[RISCV_GET_PT_ಸೂಚ್ಯಂಕ(pptr, 0)] = pte_ಮುಂದಿನ(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
    kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ[RISCV_GET_PT_ಸೂಚ್ಯಂಕ(KS_LOG_PPTR, 0)] =
        pte_ಮುಂದಿನ(kpptr_ಗಮ್ಯ_paddr(kernel_image_level1_log_ಬಫರ್_pt), false);
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ[RISCV_GET_PT_ಸೂಚ್ಯಂಕ(KERNEL_ELF_PADDR_ತಳ + PPTR_ತಳ_ಸ್ಥಾನಾಂತರ, 0)] =
        pte_ಮುಂದಿನ(kpptr_ಗಮ್ಯ_paddr(kernel_image_level1_pt), false);
    kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ[RISCV_GET_PT_ಸೂಚ್ಯಂಕ(pptr, 0)] =
        pte_ಮುಂದಿನ(kpptr_ಗಮ್ಯ_paddr(kernel_image_level1_pt), false);
    ಆಗ (pptr < PPTR_ತುದಿ + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[ಸೂಚ್ಯಂಕ] = pte_ಮುಂದಿನ(paddr, true);
        ಸೂಚ್ಯಂಕ++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ[RISCV_GET_PT_ಸೂಚ್ಯಂಕ(KDEV_ತಳ, 0)] =
        pte_ಮುಂದಿನ(kpptr_ಗಮ್ಯ_paddr(kernel_image_level1_dev_pt), false);
#ಆದರೆ_ಅಂತ್ಯ

    /* There should be free space where we put device mapping */
    ಪ್ರತಿಜ್ಞಾ(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    lookupPTSlot_ret_t pt_ret;
    pte_t *ಗುರಿ_ಸ್ಥಾನ;
    vptr_t vptr = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(pt_cap);
    pte_t *lvl1pt = PTE_PTR(pptr_ದ_cap(vspace_cap));

    /* pt to be mapped */
    pte_t *pt   = PTE_PTR(pptr_ದ_cap(pt_cap));

    /* Get PT slot to install the address in */
    pt_ret = lookupPTSlot(lvl1pt, vptr);

    ಗುರಿ_ಸ್ಥಾನ = pt_ret.pt_ಸ್ಥಾನ;

    *ಗುರಿ_ಸ್ಥಾನ = pte_new(
                      (addrFromPPtr(pt) >> se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು),
                      0, /* sw */
                      0, /* dirty (reserved non-leaf) */
                      0, /* accessed (reserved non-leaf) */
                      0,  /* global */
                      0,  /* user (reserved non-leaf) */
                      0,  /* execute */
                      0,  /* write */
                      0,  /* read */
                      1 /* valid */
                  );
    sfence();
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_ಚೌಕಟ್ಟು_cap(cap_t vspace_cap, cap_t ಚೌಕಟ್ಟು_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_ದ_cap(vspace_cap));
    pte_t *ಚೌಕಟ್ಟು_pptr   = PTE_PTR(pptr_ದ_cap(ಚೌಕಟ್ಟು_cap));
    vptr_t ಚೌಕಟ್ಟು_vptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(ಚೌಕಟ್ಟು_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, ಚೌಕಟ್ಟು_vptr);
    ಪ್ರತಿಜ್ಞಾ(lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ == se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು);

    pte_t *ಗುರಿ_ಸ್ಥಾನ = lu_ret.pt_ಸ್ಥಾನ;

    *ಗುರಿ_ಸ್ಥಾನ = pte_new(
                      (pptr_ಗಮ್ಯ_paddr(ಚೌಕಟ್ಟು_pptr) >> se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು),
                      0, /* sw */
                      1, /* dirty (leaf) */
                      1, /* accessed (leaf) */
                      0,  /* global */
                      1,  /* user (leaf) */
                      1,  /* execute */
                      1,  /* write */
                      1,  /* read */
                      1   /* valid */
                  );
    sfence();
}

BOOT_ಸಂಕೇತ cap_t create_unmapped_it_ಚೌಕಟ್ಟು_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_ಚೌಕಟ್ಟು_cap_new(
                    asidInvalid,                     /* capFMappedASID       */
                    pptr,                            /* capFBasePtr          */
                    0,                               /* capFSize             */
                    0,                               /* capFVMRights         */
                    0,
                    0                                /* capFMappedAddress    */
                );

    ಹಿಂತಿರುಗಿ cap;
}

/* Create a page table for the initial thread */
ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    ಹಿಂತಿರುಗಿ cap;
}

BOOT_ಸಂಕೇತ ಶಬ್ದ_t arch_get_n_paging(v_region_t it_v_reg)
{
    ಶಬ್ದ_t n = 0;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < CONFIG_PT_ಮಟ್ಟಗಳು - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_ಬಿಟ್ಟುಗಳು(i));
    }
    ಹಿಂತಿರುಗಿ n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_ಸಂಕೇತ cap_t create_it_ವಿಳಾಸ_space(cap_t ಬೇರು_cnode_cap, v_region_t it_v_reg)
{
    cap_t      lvl1pt_cap;
    vptr_t     pt_vptr;

    copyGlobalMappings(PTE_PTR(rootserver.vspace));

    lvl1pt_cap =
        cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
            IT_ASID,               /* capPTMappedASID    */
            (ಶಬ್ದ_t) rootserver.vspace,  /* capPTBasePtr       */
            1,                     /* capPTIsMapped      */
            (ಶಬ್ದ_t) rootserver.vspace   /* capPTMappedAddress */
        );

    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_before = ndks_boot.ಸ್ಥಾನ_pos_cur;
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < CONFIG_PT_ಮಟ್ಟಗಳು - 1; i++) {

        ಗಾಗಿ (pt_vptr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(it_v_reg.ಆರಂಭ, RISCV_GET_LVL_PGSIZE_ಬಿಟ್ಟುಗಳು(i));
             pt_vptr < it_v_reg.ಕೊನೆ;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                ಹಿಂತಿರುಗಿ cap_null_cap_new();
            }
        }

    }

    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_after = ndks_boot.ಸ್ಥಾನ_pos_cur;
    ndks_boot.bi_ಚೌಕಟ್ಟು->userImagePaging = (se_L4_ಸ್ಥಾನ_Region) {
        ಸ್ಥಾನ_pos_before, ಸ್ಥಾನ_pos_after
    };

    ಹಿಂತಿರುಗಿ lvl1pt_cap;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ activate_kernel_vspace(ಶೂನ್ಯ)
{
    ಗಣ_VSpace_ಬೇರು(kpptr_ಗಮ್ಯ_paddr(&kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ), 0);
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_ದ_cap(it_ap_cap));
    ap->array[ASID_ಕೆಳಗಿನ(IT_ASID)] = PTE_PTR(pptr_ದ_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_ಮೇಲಿನ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ಸ್ಥಿರ ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid_t asid)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_ಬೇರು;

    poolPtr = riscvKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಆದರೆ (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

        ret.vspace_ಬೇರು = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    vspace_ಬೇರು = poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
    ಆದರೆ (!vspace_ಬೇರು) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

        ret.vspace_ಬೇರು = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.vspace_ಬೇರು = vspace_ಬೇರು;
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ copyGlobalMappings(pte_t *newLvl1pt)
{
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;
    pte_t *global_kernel_vspace = kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ;

    ಗಾಗಿ (i = RISCV_GET_PT_ಸೂಚ್ಯಂಕ(PPTR_ತಳ, 0); i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        newLvl1pt[i] = global_kernel_vspace[i];
    }
}

ಶಬ್ದ_t *ಶುದ್ಧ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    ಶಬ್ದ_t w_ಬಫರ್_Ptr;
    cap_t ಬಫರ್_Cap;
    vm_rights_t vm_rights;

    w_ಬಫರ್_Ptr = thread->tcbIPCBuffer;
    ಬಫರ್_Cap = TCB_PTR_CTE_PTR(thread, tcb_ಬಫರ್)->cap;

    ಆದರೆ (unlikely(cap_get_capType(ಬಫರ್_Cap) != cap_ಚೌಕಟ್ಟು_cap)) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (unlikely(cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(ಬಫರ್_Cap))) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    vm_rights = cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(ಬಫರ್_Cap);
    ಆದರೆ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        ಶಬ್ದ_t ತಳ_Ptr, ಪುಟ_ಬಿಟ್ಟುಗಳು_2;

        ತಳ_Ptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(ಬಫರ್_Cap);
        ಪುಟ_ಬಿಟ್ಟುಗಳು_2 = ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(ಬಫರ್_Cap));
        ಹಿಂತಿರುಗಿ (ಶಬ್ದ_t *)(ತಳ_Ptr + (w_ಬಫರ್_Ptr & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_2)));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    ಹಿಂತಿರುಗಿ PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು));
}

lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    ಶಬ್ದ_t level = CONFIG_PT_ಮಟ್ಟಗಳು - 1;
    pte_t *pt = lvl1pt;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ = PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು * level + se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು;
    ret.pt_ಸ್ಥಾನ = pt + ((vptr >> ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));

    ಆಗ (is_PTEPage_ಕೋಷ್ಟಕ(ret.pt_ಸ್ಥಾನ) && likely(0 < level)) {
        level--;
        ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ -= PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
        pt = getPPtrFromHWPTE(ret.pt_ಸ್ಥಾನ);
        ret.pt_ಸ್ಥಾನ = pt + ((vptr >> ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));
    }

    ಹಿಂತಿರುಗಿ ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    ಆಯ್ಕೆ (vm_faultType) {
    ಸ್ಥಿತಿ RISCVLoad_ಪುಟ_Fault:
    ಸ್ಥಿತಿ RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;
    ಸ್ಥಿತಿ RISCVStore_ಪುಟ_Fault:
    ಸ್ಥಿತಿ RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;
    ಸ್ಥಿತಿ RISCVInstruction_ಪುಟ_Fault:
    ಸ್ಥಿತಿ RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid VM fault type");
    }
}

ಶೂನ್ಯ deleteASIDPool(asid_t asid_ತಳ, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED(asid_ತಳ, asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು));

    ಆದರೆ (riscvKSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] == pool) {
        riscvKSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] = ಖಾಲಿ;
        ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
    }
}

ಸ್ಥಿರ exception_t performASIDControlInvocation(ಶೂನ್ಯ *ಚೌಕಟ್ಟು, cte_t *ಸ್ಥಾನ, cte_t *ಪೋಷಕ, asid_t asid_ತಳ)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_ಗಣ_cap_Free_ಸೂಚ್ಯಂಕ(&(ಪೋಷಕ->cap),
                                         MAX_FREE_ಸೂಚ್ಯಂಕ(cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(ಪೋಷಕ->cap)));

    memzero(ಚೌಕಟ್ಟು, ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(RISCV_4_K_ಪುಟ)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ಸೇರಿಸು(
        cap_asid_pool_cap_new(
            asid_ತಳ,          /* capASIDBase  */
            ಶಬ್ದ_REF(ಚೌಕಟ್ಟು)     /* capASIDPool  */
        ),
        ಪೋಷಕ,
        ಸ್ಥಾನ
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    ಪ್ರತಿಜ್ಞಾ((asid_ತಳ & ಮುಖವಾಡ(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು)) == 0);
    riscvKSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] = (asid_pool_t *)ಚೌಕಟ್ಟು;

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_ಸ್ಥಾನ)
{
    cap_t cap = vspace_Cap_ಸ್ಥಾನ->cap;
    pte_t *region_ತಳ = PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap));
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTMapped_ವಿಳಾಸ(cap, 0);
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTIs_Mapped(cap, 1);
    vspace_Cap_ಸ್ಥಾನ->cap = cap;

    copyGlobalMappings(region_ತಳ);

    poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = region_ತಳ;

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಶೂನ್ಯ deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಆದರೆ (poolPtr != ಖಾಲಿ && poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = ಖಾಲಿ;
        ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
    }
}

ಶೂನ್ಯ unmap_ಪುಟ_ಕೋಷ್ಟಕ(asid_t asid, vptr_t vptr, pte_t *ಗುರಿ_pt)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        /* nothing to do */
        ಹಿಂತಿರುಗಿ;
    }
    /* We won't ever unmap a top level page table */
    ಪ್ರತಿಜ್ಞಾ(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != ಗುರಿ_pt);
    pte_t *pt_ಸ್ಥಾನ = ಖಾಲಿ;
    pte_t *pt = ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು;

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < CONFIG_PT_ಮಟ್ಟಗಳು - 1 && pt != ಗುರಿ_pt; i++) {
        pt_ಸ್ಥಾನ = pt + RISCV_GET_PT_ಸೂಚ್ಯಂಕ(vptr, i);
        ಆದರೆ (unlikely(!is_PTEPage_ಕೋಷ್ಟಕ(pt_ಸ್ಥಾನ))) {
            /* couldn't find it */
            ಹಿಂತಿರುಗಿ;
        }
        pt = getPPtrFromHWPTE(pt_ಸ್ಥಾನ);
    }

    ಆದರೆ (pt != ಗುರಿ_pt) {
        /* didn't find it */
        ಹಿಂತಿರುಗಿ;
    }
    /* If we found a pt then ptSlot won't be null */
    ಪ್ರತಿಜ್ಞಾ(pt_ಸ್ಥಾನ != ಖಾಲಿ);
    *pt_ಸ್ಥಾನ = pte_new(
                  0,  /* phy_address */
                  0,  /* sw */
                  0,  /* dirty (reserved non-leaf) */
                  0,  /* accessed (reserved non-leaf) */
                  0,  /* global */
                  0,  /* user (reserved non-leaf) */
                  0,  /* execute */
                  0,  /* write */
                  0,  /* read */
                  0  /* valid */
              );
    sfence();
}

ಸ್ಥಿರ pte_t pte_pte_invalid_new(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (pte_t) {
        0
    };
}

ಶೂನ್ಯ unmap_ಪುಟ(vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    lookupPTSlot_ret_t  lu_ret;

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ;
    }

    lu_ret = lookupPTSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr);
    ಆದರೆ (unlikely(lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ != ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಪುಟ_ಗಾತ್ರ))) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!pte_ptr_get_valid(lu_ret.pt_ಸ್ಥಾನ) || is_PTEPage_ಕೋಷ್ಟಕ(lu_ret.pt_ಸ್ಥಾನ)
        || (pte_ptr_get_ppn(lu_ret.pt_ಸ್ಥಾನ) << se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು) != pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)pptr)) {
        ಹಿಂತಿರುಗಿ;
    }

    lu_ret.pt_ಸ್ಥಾನ[0] = pte_pte_invalid_new();
    sfence();
}

ಶೂನ್ಯ ಗಣ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ಬೇರು;
    asid_t asid;
    pte_t *lvl1pt;
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t  ಕಂಡುಹಿಡಿ_ret;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    ಆದರೆ (cap_get_capType(thread_ಬೇರು) != cap_ಪುಟ_ಕೋಷ್ಟಕ_cap) {
        ಗಣ_VSpace_ಬೇರು(kpptr_ಗಮ್ಯ_paddr(&kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ), 0);
        ಹಿಂತಿರುಗಿ;
    }

    lvl1pt = PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(thread_ಬೇರು));

    asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(thread_ಬೇರು);
    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE || ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != lvl1pt)) {
        ಗಣ_VSpace_ಬೇರು(kpptr_ಗಮ್ಯ_paddr(&kernel_ಬೇರು_ಪುಟ_ಕೋಷ್ಟಕ), 0);
        ಹಿಂತಿರುಗಿ;
    }

    ಗಣ_VSpace_ಬೇರು(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_ಬೇರು(cap_t cap)
{
    ಹಿಂತಿರುಗಿ (cap_get_capType(cap) == cap_ಪುಟ_ಕೋಷ್ಟಕ_cap &&
            cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ಆದರೆ (unlikely(cap_get_capType(cap) != cap_ಚೌಕಟ್ಟು_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(!IS_ALIGNED(vptr, se_L4_IPCBuffer_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (ಪೂರ್ಣಾಂಕ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

vm_rights_t CONST ಮುಖವಾಡ_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ಮುಖವಾಡ)
{
    ಆದರೆ (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ)) {
        ಹಿಂತಿರುಗಿ VMReadOnly;
    }
    ಆದರೆ (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ)) {
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

/* The rest of the file implements the RISCV object invocations */

ಸ್ಥಿರ pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    ಶಬ್ದ_t ಲಿಖ = RISCVGetWriteFromVMRights(vm_rights);
    ಶಬ್ದ_t ಪಠ = RISCVGetReadFromVMRights(vm_rights);
    ಆದರೆ (unlikely(!ಪಠ && !ಲಿಖ && !executable)) {
        ಹಿಂತಿರುಗಿ pte_pte_invalid_new();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ pte_new(
                   paddr >> se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು,
                   0, /* sw */
                   1, /* dirty (leaf) */
                   1, /* accessed (leaf) */
                   0, /* global */
                   1, /* user (leaf) */
                   executable, /* execute */
                   RISCVGetWriteFromVMRights(vm_rights), /* write */
                   RISCVGetReadFromVMRights(vm_rights), /* read */
                   1 /* valid */
               );
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t CONST checkVPAlignment(vm_ಪುಟ_ಗಾತ್ರ_t sz, ಶಬ್ದ_t w)
{
    ಹಿಂತಿರುಗಿ (w & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(sz))) == 0;
}

ಸ್ಥಿರ exception_t decode_RISCVPage_ಕೋಷ್ಟಕ_Invocation(ಶಬ್ದ_t label, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                                  cte_t *cte, cap_t cap, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆದರೆ (label == RISCVPage_ಕೋಷ್ಟಕ_Unmap) {
        ಆದರೆ (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        ಆದರೆ (likely(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap))) {
            asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap);
            ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap));
            ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE &&
                         ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Unmap(cap, cte);
    }

    ಆದರೆ (unlikely((label != RISCVPage_ಕೋಷ್ಟಕ_Map))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (unlikely(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಶಬ್ದ_t vaddr = getSyscallArg(0, ಬಫರ್);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (unlikely(cap_get_capType(lvl1ptCap) != cap_ಪುಟ_ಕೋಷ್ಟಕ_cap ||
                 cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(lvl1ptCap));
    asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(lvl1ptCap);

    ಆದರೆ (unlikely(vaddr >= USER_ತುದಿ)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    ಆದರೆ (lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ == se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು || pte_ptr_get_valid(lu_ret.pt_ಸ್ಥಾನ)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* Get the slot to install the PT in */
    pte_t *pt_ಸ್ಥಾನ = lu_ret.pt_ಸ್ಥಾನ;

    paddr_t paddr = addrFromPPtr(
                        PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap)));
    pte_t pte = pte_new((paddr >> se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು),
                        0, /* sw */
                        0, /* dirty (reserved non-leaf) */
                        0, /* accessed (reserved non-leaf) */
                        0,  /* global */
                        0,  /* user (reserved non-leaf) */
                        0,  /* execute */
                        0,  /* write */
                        0,  /* read */
                        1 /* valid */
                       );

    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTMapped_ವಿಳಾಸ(cap, (vaddr & ~ಮುಖವಾಡ(lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ)));

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Map(cap, cte, pte, pt_ಸ್ಥಾನ);
}

ಸ್ಥಿರ exception_t decodeRISCVFrameInvocation(ಶಬ್ದ_t label, ಶಬ್ದ_t ಉದ್ದಳತೆ,
                                              cte_t *cte, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆಯ್ಕೆ (label) {
    ಸ್ಥಿತಿ RISCVPageMap: {
        ಆದರೆ (unlikely(ಉದ್ದಳತೆ < 3 || current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಶಬ್ದ_t vaddr = getSyscallArg(0, ಬಫರ್);
        ಶಬ್ದ_t w_rights_ಮುಖವಾಡ = getSyscallArg(1, ಬಫರ್);
        vm_ಗುಣಲಕ್ಷಣಗಳು_t attr = vm_ಗುಣಲಕ್ಷಣಗಳು_From_ಶಬ್ದ(getSyscallArg(2, ಬಫರ್));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ = cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap);
        vm_rights_t capVMRights = cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap);

        ಆದರೆ (unlikely(cap_get_capType(lvl1ptCap) != cap_ಪುಟ_ಕೋಷ್ಟಕ_cap ||
                     !cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(lvl1ptCap));
        asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(lvl1ptCap);

        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        ಶಬ್ದ_t vtop = vaddr + ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಚೌಕಟ್ಟು_ಗಾತ್ರ)) - 1;
        ಆದರೆ (unlikely(vtop >= USER_ತುದಿ)) {
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಆದರೆ (unlikely(!checkVPAlignment(ಚೌಕಟ್ಟು_ಗಾತ್ರ, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        ಆದರೆ (unlikely(lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ != ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಚೌಕಟ್ಟು_ಗಾತ್ರ))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        asid_t ಚೌಕಟ್ಟು_asid = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap);
        ಆದರೆ (unlikely(ಚೌಕಟ್ಟು_asid != asidInvalid)) {
            /* this frame is already mapped */
            ಆದರೆ (ಚೌಕಟ್ಟು_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
            ಶಬ್ದ_t mapped_vaddr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap);
            ಆದರೆ (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_ವಾದ;
                current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            ಆದರೆ (unlikely(is_PTEPage_ಕೋಷ್ಟಕ(lu_ret.pt_ಸ್ಥಾನ))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* check this vaddr isn't already mapped */
            ಆದರೆ (unlikely(pte_ptr_get_valid(lu_ret.pt_ಸ್ಥಾನ))) {
                userError("Virtual address (0x%"SEL4_PRIx_ಶಬ್ದ") already mapped", vaddr);
                current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = ಮುಖವಾಡ_VMRights(capVMRights, rights_From_ಶಬ್ದ(w_rights_ಮುಖವಾಡ));
        paddr_t ಚೌಕಟ್ಟು_paddr = addrFromPPtr((ಶೂನ್ಯ *) cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));
        cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, asid);
        cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(cap,  vaddr);

        bool_t executable = !vm_ಗುಣಲಕ್ಷಣಗಳು_get_riscv_Execute_Never(attr);
        pte_t pte = makeUserPTE(ಚೌಕಟ್ಟು_paddr, executable, vmRights);
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_Invocation_Map_PTE(cap, cte, pte, lu_ret.pt_ಸ್ಥಾನ);
    }

    ಸ್ಥಿತಿ RISCVPageUnmap: {
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_Invocation_Unmap(cap, cte);
    }

    ಸ್ಥಿತಿ RISCVPage_Get_ವಿಳಾಸ: {

        /* Check that there are enough message registers */
        ಪ್ರತಿಜ್ಞಾ(n_ಸಂದೇಶ_Registers >= 1);

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_Get_ವಿಳಾಸ((ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap), call);
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(ಶಬ್ದ_t label, ಶಬ್ದ_t ಉದ್ದಳತೆ, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಹಿಂತಿರುಗಿ decode_RISCVPage_ಕೋಷ್ಟಕ_Invocation(label, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಹಿಂತಿರುಗಿ decodeRISCVFrameInvocation(label, ಉದ್ದಳತೆ, cte, cap, call, ಬಫರ್);

    ಸ್ಥಿತಿ cap_asid_control_cap: {
        ಶಬ್ದ_t     i;
        asid_t           asid_ತಳ;
        ಶಬ್ದ_t           ಸೂಚ್ಯಂಕ;
        ಶಬ್ದ_t           ಆಳ;
        cap_t            untyped;
        cap_t            ಬೇರು;
        cte_t           *ಪೋಷಕ_ಸ್ಥಾನ;
        cte_t           *dest_ಸ್ಥಾನ;
        lookup_ಸ್ಥಾನ_ret_t lu_ret;
        ಶೂನ್ಯ            *ಚೌಕಟ್ಟು;
        exception_t      ಸ್ಥಿತಿ_2;

        ಆದರೆ (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ
            || current_extra_caps.excaprefs[1] == ಖಾಲಿ) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಸೂಚ್ಯಂಕ = getSyscallArg(0, ಬಫರ್);
        ಆಳ = getSyscallArg(1, ಬಫರ್);
        ಪೋಷಕ_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
        untyped = ಪೋಷಕ_ಸ್ಥಾನ->cap;
        ಬೇರು = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        ಗಾಗಿ (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        ಆದರೆ (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        asid_ತಳ = i << asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು;

        ಆದರೆ (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(untyped) != se_L4_ASIDPool_ಬಿಟ್ಟುಗಳು ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಸ್ಥಿತಿ_2 = ensureNoChildren(ಪೋಷಕ_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಚೌಕಟ್ಟು = ಶಬ್ದ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(ಬೇರು, ಸೂಚ್ಯಂಕ, ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }
        dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performASIDControlInvocation(ಚೌಕಟ್ಟು, dest_ಸ್ಥಾನ, ಪೋಷಕ_ಸ್ಥಾನ, asid_ತಳ);
    }

    ಸ್ಥಿತಿ cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_ಸ್ಥಾನ;
        asid_pool_t *pool;
        ಶಬ್ದ_t i;
        asid_t       asid;

        ಆದರೆ (label != RISCVASIDPool_ನಿಯೋಜಿಸು) {
            current_syscall_error.type = seL4_IllegalOperation;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_ಸ್ಥಾನ->cap;

        ಆದರೆ (unlikely(
                cap_get_capType(vspaceCap) != cap_ಪುಟ_ಕೋಷ್ಟಕ_cap ||
                cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_ಮೇಲಿನ(cap_asid_pool_cap_get_capASIDBase(cap))];
        ಆದರೆ (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        ಗಾಗಿ (i = 0; i < ಬಿಟ್(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು) && (asid + i == 0 || pool->array[i]); i++);

        ಆದರೆ (i == ಬಿಟ್(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು)) {
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performASIDPoolInvocation(asid, pool, vspace_Cap_ಸ್ಥಾನ);
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid arch cap type");
    }
}

exception_t perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Map(cap_t cap, cte_t *ct_ಸ್ಥಾನ,
                                          pte_t pte, pte_t *pt_ಸ್ಥಾನ)
{
    ct_ಸ್ಥಾನ->cap = cap;
    *pt_ಸ್ಥಾನ = pte;
    sfence();

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t perform_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Unmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    ಆದರೆ (cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap));
        unmap_ಪುಟ_ಕೋಷ್ಟಕ(
            cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap),
            cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap),
            pt
        );
        clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *)pt, se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು);
    }
    cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ptr_ಗಣ_cap_PTIs_Mapped(&(ct_ಸ್ಥಾನ->cap), 0);

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

ಸ್ಥಿರ exception_t updatePTE(pte_t pte, pte_t *ತಳ)
{
    *ತಳ = pte;
    sfence();
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t perform_ಪುಟ_Invocation_Map_PTE(cap_t cap, cte_t *ct_ಸ್ಥಾನ,
                                        pte_t pte, pte_t *ತಳ)
{
    ct_ಸ್ಥಾನ->cap = cap;
    ಹಿಂತಿರುಗಿ updatePTE(pte, ತಳ);
}

exception_t perform_ಪುಟ_Invocation_Unmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        unmap_ಪುಟ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
                  cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                  cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                  cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap)
                 );
    }

    cap_t ಸ್ಥಾನ_Cap = ct_ಸ್ಥಾನ->cap;
    ಸ್ಥಾನ_Cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(ಸ್ಥಾನ_Cap, 0);
    ಸ್ಥಾನ_Cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(ಸ್ಥಾನ_Cap, asidInvalid);
    ct_ಸ್ಥಾನ->cap = ಸ್ಥಾನ_Cap;

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಶೂನ್ಯ Arch_user_ರಾಶಿ_Trace(tcb_t *tptr)
{
    cap_t thread_ಬೇರು = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    ಆದರೆ (!is_Valid_VTable_ಬೇರು(thread_ಬೇರು)) {
        ಬರೆ("Invalid vspace\n");
        ಹಿಂತಿರುಗಿ;
    }

    ಶಬ್ದ_t sp = get_ರಿಜಿಸ್ಟರ್(tptr, SP);
    ಆದರೆ (!IS_ALIGNED(sp, se_L4_ಶಬ್ದ_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        ಬರೆ("SP %p not aligned", (ಶೂನ್ಯ *) sp);
        ಹಿಂತಿರುಗಿ;
    }

    pte_t *vspace_ಬೇರು = PTE_PTR(pptr_ದ_cap(thread_ಬೇರು));
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < CONFIG_USER_ರಾಶಿ_TRACE_ಉದ್ದಳತೆ; i++) {
        ಶಬ್ದ_t ವಿಳಾಸ = sp + (i * ಗಾತ್ರ(ಶಬ್ದ_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_ಬೇರು, ವಿಳಾಸ);
        ಆದರೆ (ವಿಳಾಸ < USER_ತುದಿ && pte_ptr_get_valid(ret.pt_ಸ್ಥಾನ) && !is_PTEPage_ಕೋಷ್ಟಕ(ret.pt_ಸ್ಥಾನ)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.pt_ಸ್ಥಾನ));
            ಶಬ್ದ_t *ಮೌಲ್ಯ = (ಶಬ್ದ_t *)((ಶಬ್ದ_t)pptr + (ವಿಳಾಸ & ಮುಖವಾಡ(ret.pt_ಬಿಟ್ಟುಗಳು_ಎಡ)));
            ಬರೆ("0x%lx: 0x%lx\n", (ದೀರ್ಘ) ವಿಳಾಸ, (ದೀರ್ಘ) *ಮೌಲ್ಯ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("0x%lx: INVALID\n", (ದೀರ್ಘ) ವಿಳಾಸ);
        }
    }
}
#ಆದರೆ_ಅಂತ್ಯ

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

    ಚೌಕಟ್ಟು_ಗಾತ್ರ = cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(lu_ret.cap);

    ಆದರೆ (ಚೌಕಟ್ಟು_ಗಾತ್ರ != RISCV_Mega_ಪುಟ) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(ಚೌಕಟ್ಟು_cptr, false);

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಚೌಕಟ್ಟು_pptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_ಬಫರ್ = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *) ಚೌಕಟ್ಟು_pptr);

#ಆದರೆ __riscv_xlen == 32
    paddr_t physical_ವಿಳಾಸ = ks_User_Log_ಬಫರ್;
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i += 1) {
        kernel_image_level1_log_ಬಫರ್_pt[i] = pte_ಮುಂದಿನ(physical_ವಿಳಾಸ, true);
        physical_ವಿಳಾಸ += ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು);
    }
    ಪ್ರತಿಜ್ಞಾ(physical_ವಿಳಾಸ - ks_User_Log_ಬಫರ್ == ಬಿಟ್(se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು));
#ಇಲ್ಲದಿದ್ದರೆ
    kernel_image_level1_dev_pt[RISCV_GET_PT_ಸೂಚ್ಯಂಕ(KS_LOG_PPTR, 1)] = pte_ಮುಂದಿನ(ks_User_Log_ಬಫರ್, true);
#ಆದರೆ_ಅಂತ್ಯ

    sfence();

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */
