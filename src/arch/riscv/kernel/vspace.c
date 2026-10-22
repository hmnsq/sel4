/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/tcb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/preemption.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/cnode.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/untyped.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/stack.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

ਬਣਤਰ resolve_ret {
    paddr_t ਚੌਖਟਾ_ਅਧਾਰ;
    vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ;
    bool_t valid;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ resolve_ret resolve_ret_t;

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Get_ਪਤਾ(ਖਾਲੀ *vbase_ptr, bool_t call);

ਸਥਿਰ ਸ਼ਬਦ_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    ਵਾਪਸ vm_rights == VMReadWrite;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported.
     * Kernel-only conveys no user rights. */
    ਵਾਪਸ vm_rights != VMKernelOnly;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t is_PTEPage_ਸਾਰਣੀ(pte_t *pte)
{
    ਵਾਪਸ pte_ptr_get_valid(pte) &&
           !(pte_ptr_get_read(pte) || pte_ptr_get_write(pte) || pte_ptr_get_execute(pte));
}

/** Helper function meant only to be used for mapping the kernel
 * window.
 *
 * Maps all pages with full RWX and supervisor perms by default.
 */
ਸਥਿਰ pte_t pte_ਅਗਲਾ(ਸ਼ਬਦ_t phys_addr, bool_t is_ਪੱਤੀ)
{
    ਸ਼ਬਦ_t ppn = (ਸ਼ਬਦ_t)(phys_addr >> 12);

    uint8_t ਪਠ = is_ਪੱਤੀ ? 1 : 0;
    uint8_t ਲਿਖ = ਪਠ;
    uint8_t exec = ਪਠ;

    ਵਾਪਸ pte_new(ppn,
                   0,     /* sw */
                   is_ਪੱਤੀ ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_ਪੱਤੀ ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_ਪੱਤੀ ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   ਲਿਖ, /* write */
                   ਪਠ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_ਕੋਡ ਖਾਲੀ map_kernel_ਚੌਖਟਾ(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#ਜੇ __riscv_xlen == 32
    paddr = ਗੇੜਾ_ਥੱਲੇ(paddr, RISCV_GET_LVL_PGSIZE_ਬਿੱਟ(0));
    ਪ੍ਰਤਿਜ੍ਞਾ((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ[RISCV_GET_PT_ਸੂਚਕਾਂਕ(vaddr, 0)] = pte_ਅਗਲਾ(paddr, true);
#ਨਹੀਂਤਾਂ
    ਜੇ (vaddr >= KDEV_ਅਧਾਰ) {
        /* Map devices in level 1 page table */
        paddr = ਗੇੜਾ_ਥੱਲੇ(paddr, RISCV_GET_LVL_PGSIZE_ਬਿੱਟ(1));
        ਪ੍ਰਤਿਜ੍ਞਾ((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_ਸੂਚਕਾਂਕ(vaddr, 1)] = pte_ਅਗਲਾ(paddr, true);
    } ਨਹੀਂਤਾਂ {
        paddr = ਗੇੜਾ_ਥੱਲੇ(paddr, RISCV_GET_LVL_PGSIZE_ਬਿੱਟ(0));
        ਪ੍ਰਤਿਜ੍ਞਾ((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ[RISCV_GET_PT_ਸੂਚਕਾਂਕ(vaddr, 0)] = pte_ਅਗਲਾ(paddr, true);
    }
#ਜੇ_ਅੰਤ
}

BOOT_ਕੋਡ VISIBLE ਖਾਲੀ map_kernel_ਖਿੜਕੀ(ਖਾਲੀ)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    ਪ੍ਰਤਿਜ੍ਞਾ(CONFIG_PT_ਪੱਧਰ > 1 && CONFIG_PT_ਪੱਧਰ <= 4);
    /* Kernel image finishes before KDEV_BASE */
    ਪ੍ਰਤਿਜ੍ਞਾ(KERNEL_ELF_ਸਿਖਰ <= KDEV_ਅਧਾਰ);

    /* kernel window starts at PPTR_BASE */
    ਸ਼ਬਦ_t pptr = PPTR_ਅਧਾਰ;

    /* first we map in memory from PADDR_BASE */
    ਸ਼ਬਦ_t paddr = PADDR_ਅਧਾਰ;
    ਜਦੋਂ (pptr < PPTR_ਸਿਖਰ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_ਬਿੱਟ(0)));
        ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_ਬਿੱਟ(0)));

        kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ[RISCV_GET_PT_ਸੂਚਕਾਂਕ(pptr, 0)] = pte_ਅਗਲਾ(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    ਪ੍ਰਤਿਜ੍ਞਾ(pptr == PPTR_ਸਿਖਰ);
    pptr = ਗੇੜਾ_ਥੱਲੇ(KERNEL_ELF_ਅਧਾਰ, RISCV_GET_LVL_PGSIZE_ਬਿੱਟ(0));
    paddr = ਗੇੜਾ_ਥੱਲੇ(KERNEL_ELF_PADDR_ਅਧਾਰ, RISCV_GET_LVL_PGSIZE_ਬਿੱਟ(0));

#ਜੇ __riscv_xlen == 32
    kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ[RISCV_GET_PT_ਸੂਚਕਾਂਕ(pptr, 0)] = pte_ਅਗਲਾ(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
    kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ[RISCV_GET_PT_ਸੂਚਕਾਂਕ(KS_LOG_PPTR, 0)] =
        pte_ਅਗਲਾ(kpptr_ਮੰਜ਼ਿਲ_paddr(kernel_image_level1_log_ਬਫਰ_pt), false);
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ[RISCV_GET_PT_ਸੂਚਕਾਂਕ(KERNEL_ELF_PADDR_ਅਧਾਰ + PPTR_ਅਧਾਰ_ਵਿੱਥ, 0)] =
        pte_ਅਗਲਾ(kpptr_ਮੰਜ਼ਿਲ_paddr(kernel_image_level1_pt), false);
    kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ[RISCV_GET_PT_ਸੂਚਕਾਂਕ(pptr, 0)] =
        pte_ਅਗਲਾ(kpptr_ਮੰਜ਼ਿਲ_paddr(kernel_image_level1_pt), false);
    ਜਦੋਂ (pptr < PPTR_ਸਿਖਰ + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[ਸੂਚਕਾਂਕ] = pte_ਅਗਲਾ(paddr, true);
        ਸੂਚਕਾਂਕ++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ[RISCV_GET_PT_ਸੂਚਕਾਂਕ(KDEV_ਅਧਾਰ, 0)] =
        pte_ਅਗਲਾ(kpptr_ਮੰਜ਼ਿਲ_paddr(kernel_image_level1_dev_pt), false);
#ਜੇ_ਅੰਤ

    /* There should be free space where we put device mapping */
    ਪ੍ਰਤਿਜ੍ਞਾ(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_ਕੋਡ ਖਾਲੀ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    lookupPTSlot_ret_t pt_ret;
    pte_t *ਨਿਸ਼ਾਨਾ_ਥਾਂ;
    vptr_t vptr = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(pt_cap);
    pte_t *lvl1pt = PTE_PTR(pptr_ਦਾ_cap(vspace_cap));

    /* pt to be mapped */
    pte_t *pt   = PTE_PTR(pptr_ਦਾ_cap(pt_cap));

    /* Get PT slot to install the address in */
    pt_ret = lookupPTSlot(lvl1pt, vptr);

    ਨਿਸ਼ਾਨਾ_ਥਾਂ = pt_ret.pt_ਥਾਂ;

    *ਨਿਸ਼ਾਨਾ_ਥਾਂ = pte_new(
                      (addrFromPPtr(pt) >> se_L4_ਪੰਨਾ_ਬਿੱਟ),
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

BOOT_ਕੋਡ ਖਾਲੀ map_it_ਚੌਖਟਾ_cap(cap_t vspace_cap, cap_t ਚੌਖਟਾ_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_ਦਾ_cap(vspace_cap));
    pte_t *ਚੌਖਟਾ_pptr   = PTE_PTR(pptr_ਦਾ_cap(ਚੌਖਟਾ_cap));
    vptr_t ਚੌਖਟਾ_vptr = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(ਚੌਖਟਾ_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, ਚੌਖਟਾ_vptr);
    ਪ੍ਰਤਿਜ੍ਞਾ(lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ == se_L4_ਪੰਨਾ_ਬਿੱਟ);

    pte_t *ਨਿਸ਼ਾਨਾ_ਥਾਂ = lu_ret.pt_ਥਾਂ;

    *ਨਿਸ਼ਾਨਾ_ਥਾਂ = pte_new(
                      (pptr_ਮੰਜ਼ਿਲ_paddr(ਚੌਖਟਾ_pptr) >> se_L4_ਪੰਨਾ_ਬਿੱਟ),
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

BOOT_ਕੋਡ cap_t create_unmapped_it_ਚੌਖਟਾ_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_ਚੌਖਟਾ_cap_new(
                    asidInvalid,                     /* capFMappedASID       */
                    pptr,                            /* capFBasePtr          */
                    0,                               /* capFSize             */
                    0,                               /* capFVMRights         */
                    0,
                    0                                /* capFMappedAddress    */
                );

    ਵਾਪਸ cap;
}

/* Create a page table for the initial thread */
ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    ਵਾਪਸ cap;
}

BOOT_ਕੋਡ ਸ਼ਬਦ_t arch_get_n_paging(v_region_t it_v_reg)
{
    ਸ਼ਬਦ_t n = 0;
    ਲਈ (ਪੂਰਨ i = 0; i < CONFIG_PT_ਪੱਧਰ - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_ਬਿੱਟ(i));
    }
    ਵਾਪਸ n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_ਕੋਡ cap_t create_it_ਪਤਾ_space(cap_t ਜੜ੍ਹ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      lvl1pt_cap;
    vptr_t     pt_vptr;

    copyGlobalMappings(PTE_PTR(rootserver.vspace));

    lvl1pt_cap =
        cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
            IT_ASID,               /* capPTMappedASID    */
            (ਸ਼ਬਦ_t) rootserver.vspace,  /* capPTBasePtr       */
            1,                     /* capPTIsMapped      */
            (ਸ਼ਬਦ_t) rootserver.vspace   /* capPTMappedAddress */
        );

    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_before = ndks_boot.ਥਾਂ_pos_cur;
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    ਲਈ (ਪੂਰਨ i = 0; i < CONFIG_PT_ਪੱਧਰ - 1; i++) {

        ਲਈ (pt_vptr = ਗੇੜਾ_ਥੱਲੇ(it_v_reg.ਸ਼ੁਰੂ, RISCV_GET_LVL_PGSIZE_ਬਿੱਟ(i));
             pt_vptr < it_v_reg.ਸਿਰਾ;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                ਵਾਪਸ cap_null_cap_new();
            }
        }

    }

    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_after = ndks_boot.ਥਾਂ_pos_cur;
    ndks_boot.bi_ਚੌਖਟਾ->userImagePaging = (se_L4_ਥਾਂ_Region) {
        ਥਾਂ_pos_before, ਥਾਂ_pos_after
    };

    ਵਾਪਸ lvl1pt_cap;
}

BOOT_ਕੋਡ ਖਾਲੀ activate_kernel_vspace(ਖਾਲੀ)
{
    ਸੈੱਟ_VSpace_ਜੜ੍ਹ(kpptr_ਮੰਜ਼ਿਲ_paddr(&kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ), 0);
}

BOOT_ਕੋਡ ਖਾਲੀ write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_ਦਾ_cap(it_ap_cap));
    ap->array[ASID_ਨੀਵਾਂ(IT_ASID)] = PTE_PTR(pptr_ਦਾ_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_ਉੱਚਾ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ਸਥਿਰ ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_VSpace_For_ASID(asid_t asid)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_ਜੜ੍ਹ;

    poolPtr = riscvKSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਜੇ (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();

        ret.vspace_ਜੜ੍ਹ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    vspace_ਜੜ੍ਹ = poolPtr->array[ASID_ਨੀਵਾਂ(asid)];
    ਜੇ (!vspace_ਜੜ੍ਹ) {
        current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();

        ret.vspace_ਜੜ੍ਹ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ret.vspace_ਜੜ੍ਹ = vspace_ਜੜ੍ਹ;
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

ਖਾਲੀ copyGlobalMappings(pte_t *newLvl1pt)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;
    pte_t *global_kernel_vspace = kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ;

    ਲਈ (i = RISCV_GET_PT_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ, 0); i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        newLvl1pt[i] = global_kernel_vspace[i];
    }
}

ਸ਼ਬਦ_t *ਸ਼ੁੱਧ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    ਸ਼ਬਦ_t w_ਬਫਰ_Ptr;
    cap_t ਬਫਰ_Cap;
    vm_rights_t vm_rights;

    w_ਬਫਰ_Ptr = thread->tcbIPCBuffer;
    ਬਫਰ_Cap = TCB_PTR_CTE_PTR(thread, tcb_ਬਫਰ)->cap;

    ਜੇ (unlikely(cap_get_capType(ਬਫਰ_Cap) != cap_ਚੌਖਟਾ_cap)) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (unlikely(cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(ਬਫਰ_Cap))) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    vm_rights = cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(ਬਫਰ_Cap);
    ਜੇ (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        ਸ਼ਬਦ_t ਅਧਾਰ_Ptr, ਪੰਨਾ_ਬਿੱਟ_2;

        ਅਧਾਰ_Ptr = cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(ਬਫਰ_Cap);
        ਪੰਨਾ_ਬਿੱਟ_2 = ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(ਬਫਰ_Cap));
        ਵਾਪਸ (ਸ਼ਬਦ_t *)(ਅਧਾਰ_Ptr + (w_ਬਫਰ_Ptr & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_2)));
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਨਿਰਾ;
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    ਵਾਪਸ PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ));
}

lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    ਸ਼ਬਦ_t level = CONFIG_PT_ਪੱਧਰ - 1;
    pte_t *pt = lvl1pt;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_ਬਿੱਟ_ਖੱਬਾ = PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ * level + se_L4_ਪੰਨਾ_ਬਿੱਟ;
    ret.pt_ਥਾਂ = pt + ((vptr >> ret.pt_ਬਿੱਟ_ਖੱਬਾ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ));

    ਜਦੋਂ (is_PTEPage_ਸਾਰਣੀ(ret.pt_ਥਾਂ) && likely(0 < level)) {
        level--;
        ret.pt_ਬਿੱਟ_ਖੱਬਾ -= PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
        pt = getPPtrFromHWPTE(ret.pt_ਥਾਂ);
        ret.pt_ਥਾਂ = pt + ((vptr >> ret.pt_ਬਿੱਟ_ਖੱਬਾ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ));
    }

    ਵਾਪਸ ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    ਚੋਣ (vm_faultType) {
    ਹਾਲਤ RISCVLoad_ਪੰਨਾ_Fault:
    ਹਾਲਤ RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        ਵਾਪਸ EXCEPTION_FAULT;
    ਹਾਲਤ RISCVStore_ਪੰਨਾ_Fault:
    ਹਾਲਤ RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        ਵਾਪਸ EXCEPTION_FAULT;
    ਹਾਲਤ RISCVInstruction_ਪੰਨਾ_Fault:
    ਹਾਲਤ RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        ਵਾਪਸ EXCEPTION_FAULT;

    ਮੂਲ:
        fail("Invalid VM fault type");
    }
}

ਖਾਲੀ deleteASIDPool(asid_t asid_ਅਧਾਰ, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(asid_ਅਧਾਰ, asid_ਨੀਵਾਂ_ਬਿੱਟ));

    ਜੇ (riscvKSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] == pool) {
        riscvKSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] = ਨਿਰਾ;
        ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
    }
}

ਸਥਿਰ exception_t performASIDControlInvocation(ਖਾਲੀ *ਚੌਖਟਾ, cte_t *ਥਾਂ, cte_t *ਮਾਪਾ, asid_t asid_ਅਧਾਰ)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_ਸੈੱਟ_cap_Free_ਸੂਚਕਾਂਕ(&(ਮਾਪਾ->cap),
                                         MAX_FREE_ਸੂਚਕਾਂਕ(cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(ਮਾਪਾ->cap)));

    memzero(ਚੌਖਟਾ, ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(RISCV_4_K_ਪੰਨਾ)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ਪਾਓ(
        cap_asid_pool_cap_new(
            asid_ਅਧਾਰ,          /* capASIDBase  */
            ਸ਼ਬਦ_REF(ਚੌਖਟਾ)     /* capASIDPool  */
        ),
        ਮਾਪਾ,
        ਥਾਂ
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    ਪ੍ਰਤਿਜ੍ਞਾ((asid_ਅਧਾਰ & ਨਕਾਬ(asid_ਨੀਵਾਂ_ਬਿੱਟ)) == 0);
    riscvKSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] = (asid_pool_t *)ਚੌਖਟਾ;

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_ਥਾਂ)
{
    cap_t cap = vspace_Cap_ਥਾਂ->cap;
    pte_t *region_ਅਧਾਰ = PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap));
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTMapped_ਪਤਾ(cap, 0);
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTIs_Mapped(cap, 1);
    vspace_Cap_ਥਾਂ->cap = cap;

    copyGlobalMappings(region_ਅਧਾਰ);

    poolPtr->array[ASID_ਨੀਵਾਂ(asid)] = region_ਅਧਾਰ;

    ਵਾਪਸ EXCEPTION_NONE;
}

ਖਾਲੀ deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਜੇ (poolPtr != ਨਿਰਾ && poolPtr->array[ASID_ਨੀਵਾਂ(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_ਨੀਵਾਂ(asid)] = ਨਿਰਾ;
        ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
    }
}

ਖਾਲੀ unmap_ਪੰਨਾ_ਸਾਰਣੀ(asid_t asid, vptr_t vptr, pte_t *ਨਿਸ਼ਾਨਾ_pt)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        /* nothing to do */
        ਵਾਪਸ;
    }
    /* We won't ever unmap a top level page table */
    ਪ੍ਰਤਿਜ੍ਞਾ(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != ਨਿਸ਼ਾਨਾ_pt);
    pte_t *pt_ਥਾਂ = ਨਿਰਾ;
    pte_t *pt = ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ;

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < CONFIG_PT_ਪੱਧਰ - 1 && pt != ਨਿਸ਼ਾਨਾ_pt; i++) {
        pt_ਥਾਂ = pt + RISCV_GET_PT_ਸੂਚਕਾਂਕ(vptr, i);
        ਜੇ (unlikely(!is_PTEPage_ਸਾਰਣੀ(pt_ਥਾਂ))) {
            /* couldn't find it */
            ਵਾਪਸ;
        }
        pt = getPPtrFromHWPTE(pt_ਥਾਂ);
    }

    ਜੇ (pt != ਨਿਸ਼ਾਨਾ_pt) {
        /* didn't find it */
        ਵਾਪਸ;
    }
    /* If we found a pt then ptSlot won't be null */
    ਪ੍ਰਤਿਜ੍ਞਾ(pt_ਥਾਂ != ਨਿਰਾ);
    *pt_ਥਾਂ = pte_new(
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

ਸਥਿਰ pte_t pte_pte_invalid_new(ਖਾਲੀ)
{
    ਵਾਪਸ (pte_t) {
        0
    };
}

ਖਾਲੀ unmap_ਪੰਨਾ(vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;
    lookupPTSlot_ret_t  lu_ret;

    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ;
    }

    lu_ret = lookupPTSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr);
    ਜੇ (unlikely(lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ != ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਪੰਨਾ_ਮਾਪ))) {
        ਵਾਪਸ;
    }
    ਜੇ (!pte_ptr_get_valid(lu_ret.pt_ਥਾਂ) || is_PTEPage_ਸਾਰਣੀ(lu_ret.pt_ਥਾਂ)
        || (pte_ptr_get_ppn(lu_ret.pt_ਥਾਂ) << se_L4_ਪੰਨਾ_ਬਿੱਟ) != pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)pptr)) {
        ਵਾਪਸ;
    }

    lu_ret.pt_ਥਾਂ[0] = pte_pte_invalid_new();
    sfence();
}

ਖਾਲੀ ਸੈੱਟ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ਜੜ੍ਹ;
    asid_t asid;
    pte_t *lvl1pt;
    ਲੱਭੋ_VSpace_For_ASID_ret_t  ਲੱਭੋ_ret;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    ਜੇ (cap_get_capType(thread_ਜੜ੍ਹ) != cap_ਪੰਨਾ_ਸਾਰਣੀ_cap) {
        ਸੈੱਟ_VSpace_ਜੜ੍ਹ(kpptr_ਮੰਜ਼ਿਲ_paddr(&kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ), 0);
        ਵਾਪਸ;
    }

    lvl1pt = PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(thread_ਜੜ੍ਹ));

    asid = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(thread_ਜੜ੍ਹ);
    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE || ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != lvl1pt)) {
        ਸੈੱਟ_VSpace_ਜੜ੍ਹ(kpptr_ਮੰਜ਼ਿਲ_paddr(&kernel_ਜੜ੍ਹ_ਪੰਨਾ_ਸਾਰਣੀ), 0);
        ਵਾਪਸ;
    }

    ਸੈੱਟ_VSpace_ਜੜ੍ਹ(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_ਜੜ੍ਹ(cap_t cap)
{
    ਵਾਪਸ (cap_get_capType(cap) == cap_ਪੰਨਾ_ਸਾਰਣੀ_cap &&
            cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ਜੇ (unlikely(cap_get_capType(cap) != cap_ਚੌਖਟਾ_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(!IS_ALIGNED(vptr, se_L4_IPCBuffer_ਮਾਪ_ਬਿੱਟ))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (ਪੂਰਨ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

vm_rights_t CONST ਨਕਾਬ_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ਨਕਾਬ)
{
    ਜੇ (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ)) {
        ਵਾਪਸ VMReadOnly;
    }
    ਜੇ (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ)) {
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

/* The rest of the file implements the RISCV object invocations */

ਸਥਿਰ pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    ਸ਼ਬਦ_t ਲਿਖ = RISCVGetWriteFromVMRights(vm_rights);
    ਸ਼ਬਦ_t ਪਠ = RISCVGetReadFromVMRights(vm_rights);
    ਜੇ (unlikely(!ਪਠ && !ਲਿਖ && !executable)) {
        ਵਾਪਸ pte_pte_invalid_new();
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ pte_new(
                   paddr >> se_L4_ਪੰਨਾ_ਬਿੱਟ,
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

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t CONST checkVPAlignment(vm_ਪੰਨਾ_ਮਾਪ_t sz, ਸ਼ਬਦ_t w)
{
    ਵਾਪਸ (w & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(sz))) == 0;
}

ਸਥਿਰ exception_t decode_RISCVPage_ਸਾਰਣੀ_Invocation(ਸ਼ਬਦ_t label, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                                  cte_t *cte, cap_t cap, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਜੇ (label == RISCVPage_ਸਾਰਣੀ_Unmap) {
        ਜੇ (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        ਜੇ (likely(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap))) {
            asid_t asid = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap);
            ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap));
            ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ == EXCEPTION_NONE &&
                         ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Unmap(cap, cte);
    }

    ਜੇ (unlikely((label != RISCVPage_ਸਾਰਣੀ_Map))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (unlikely(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸ਼ਬਦ_t vaddr = getSyscallArg(0, ਬਫਰ);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (unlikely(cap_get_capType(lvl1ptCap) != cap_ਪੰਨਾ_ਸਾਰਣੀ_cap ||
                 cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(lvl1ptCap));
    asid_t asid = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(lvl1ptCap);

    ਜੇ (unlikely(vaddr >= USER_ਸਿਖਰ)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (unlikely(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    ਜੇ (lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ == se_L4_ਪੰਨਾ_ਬਿੱਟ || pte_ptr_get_valid(lu_ret.pt_ਥਾਂ)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* Get the slot to install the PT in */
    pte_t *pt_ਥਾਂ = lu_ret.pt_ਥਾਂ;

    paddr_t paddr = addrFromPPtr(
                        PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap)));
    pte_t pte = pte_new((paddr >> se_L4_ਪੰਨਾ_ਬਿੱਟ),
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

    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTMapped_ਪਤਾ(cap, (vaddr & ~ਨਕਾਬ(lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ)));

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Map(cap, cte, pte, pt_ਥਾਂ);
}

ਸਥਿਰ exception_t decodeRISCVFrameInvocation(ਸ਼ਬਦ_t label, ਸ਼ਬਦ_t ਲੰਮਾਈ,
                                              cte_t *cte, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਚੋਣ (label) {
    ਹਾਲਤ RISCVPageMap: {
        ਜੇ (unlikely(ਲੰਮਾਈ < 3 || current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸ਼ਬਦ_t vaddr = getSyscallArg(0, ਬਫਰ);
        ਸ਼ਬਦ_t w_rights_ਨਕਾਬ = getSyscallArg(1, ਬਫਰ);
        vm_ਗੁਣ_t attr = vm_ਗੁਣ_From_ਸ਼ਬਦ(getSyscallArg(2, ਬਫਰ));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ = cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap);
        vm_rights_t capVMRights = cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap);

        ਜੇ (unlikely(cap_get_capType(lvl1ptCap) != cap_ਪੰਨਾ_ਸਾਰਣੀ_cap ||
                     !cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(lvl1ptCap));
        asid_t asid = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(lvl1ptCap);

        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (unlikely(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        ਸ਼ਬਦ_t vtop = vaddr + ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਚੌਖਟਾ_ਮਾਪ)) - 1;
        ਜੇ (unlikely(vtop >= USER_ਸਿਖਰ)) {
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਜੇ (unlikely(!checkVPAlignment(ਚੌਖਟਾ_ਮਾਪ, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        ਜੇ (unlikely(lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ != ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਚੌਖਟਾ_ਮਾਪ))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        asid_t ਚੌਖਟਾ_asid = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap);
        ਜੇ (unlikely(ਚੌਖਟਾ_asid != asidInvalid)) {
            /* this frame is already mapped */
            ਜੇ (ਚੌਖਟਾ_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
            ਸ਼ਬਦ_t mapped_vaddr = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap);
            ਜੇ (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
                current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            ਜੇ (unlikely(is_PTEPage_ਸਾਰਣੀ(lu_ret.pt_ਥਾਂ))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        } ਨਹੀਂਤਾਂ {
            /* check this vaddr isn't already mapped */
            ਜੇ (unlikely(pte_ptr_get_valid(lu_ret.pt_ਥਾਂ))) {
                userError("Virtual address (0x%"SEL4_PRIx_ਸ਼ਬਦ") already mapped", vaddr);
                current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = ਨਕਾਬ_VMRights(capVMRights, rights_From_ਸ਼ਬਦ(w_rights_ਨਕਾਬ));
        paddr_t ਚੌਖਟਾ_paddr = addrFromPPtr((ਖਾਲੀ *) cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));
        cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, asid);
        cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(cap,  vaddr);

        bool_t executable = !vm_ਗੁਣ_get_riscv_Execute_Never(attr);
        pte_t pte = makeUserPTE(ਚੌਖਟਾ_paddr, executable, vmRights);
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_Invocation_Map_PTE(cap, cte, pte, lu_ret.pt_ਥਾਂ);
    }

    ਹਾਲਤ RISCVPageUnmap: {
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_Invocation_Unmap(cap, cte);
    }

    ਹਾਲਤ RISCVPage_Get_ਪਤਾ: {

        /* Check that there are enough message registers */
        ਪ੍ਰਤਿਜ੍ਞਾ(n_ਸੁਨੇਹਾ_Registers >= 1);

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_Get_ਪਤਾ((ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap), call);
    }

    ਮੂਲ:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(ਸ਼ਬਦ_t label, ਸ਼ਬਦ_t ਲੰਮਾਈ, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਚੋਣ (cap_get_capType(cap)) {

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਵਾਪਸ decode_RISCVPage_ਸਾਰਣੀ_Invocation(label, ਲੰਮਾਈ, cte, cap, ਬਫਰ);

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਵਾਪਸ decodeRISCVFrameInvocation(label, ਲੰਮਾਈ, cte, cap, call, ਬਫਰ);

    ਹਾਲਤ cap_asid_control_cap: {
        ਸ਼ਬਦ_t     i;
        asid_t           asid_ਅਧਾਰ;
        ਸ਼ਬਦ_t           ਸੂਚਕਾਂਕ;
        ਸ਼ਬਦ_t           ਡੂੰਘਾਈ;
        cap_t            untyped;
        cap_t            ਜੜ੍ਹ;
        cte_t           *ਮਾਪਾ_ਥਾਂ;
        cte_t           *dest_ਥਾਂ;
        lookup_ਥਾਂ_ret_t lu_ret;
        ਖਾਲੀ            *ਚੌਖਟਾ;
        exception_t      ਸਥਿਤੀ;

        ਜੇ (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ
            || current_extra_caps.excaprefs[1] == ਨਿਰਾ) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੂਚਕਾਂਕ = getSyscallArg(0, ਬਫਰ);
        ਡੂੰਘਾਈ = getSyscallArg(1, ਬਫਰ);
        ਮਾਪਾ_ਥਾਂ = current_extra_caps.excaprefs[0];
        untyped = ਮਾਪਾ_ਥਾਂ->cap;
        ਜੜ੍ਹ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        ਲਈ (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        ਜੇ (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        asid_ਅਧਾਰ = i << asid_ਨੀਵਾਂ_ਬਿੱਟ;

        ਜੇ (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(untyped) != se_L4_ASIDPool_ਬਿੱਟ ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸਥਿਤੀ = ensureNoChildren(ਮਾਪਾ_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਚੌਖਟਾ = ਸ਼ਬਦ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(ਜੜ੍ਹ, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }
        dest_ਥਾਂ = lu_ret.ਥਾਂ;

        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performASIDControlInvocation(ਚੌਖਟਾ, dest_ਥਾਂ, ਮਾਪਾ_ਥਾਂ, asid_ਅਧਾਰ);
    }

    ਹਾਲਤ cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_ਥਾਂ;
        asid_pool_t *pool;
        ਸ਼ਬਦ_t i;
        asid_t       asid;

        ਜੇ (label != RISCVASIDPool_ਸੌਂਪੋ) {
            current_syscall_error.type = seL4_IllegalOperation;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_ਥਾਂ = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_ਥਾਂ->cap;

        ਜੇ (unlikely(
                cap_get_capType(vspaceCap) != cap_ਪੰਨਾ_ਸਾਰਣੀ_cap ||
                cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_ਉੱਚਾ(cap_asid_pool_cap_get_capASIDBase(cap))];
        ਜੇ (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        ਲਈ (i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(asid_ਨੀਵਾਂ_ਬਿੱਟ) && (asid + i == 0 || pool->array[i]); i++);

        ਜੇ (i == ਦੋਆਧਾਰੀ_ਅੰਕ(asid_ਨੀਵਾਂ_ਬਿੱਟ)) {
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performASIDPoolInvocation(asid, pool, vspace_Cap_ਥਾਂ);
    }
    ਮੂਲ:
        fail("Invalid arch cap type");
    }
}

exception_t perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Map(cap_t cap, cte_t *ct_ਥਾਂ,
                                          pte_t pte, pte_t *pt_ਥਾਂ)
{
    ct_ਥਾਂ->cap = cap;
    *pt_ਥਾਂ = pte;
    sfence();

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t perform_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Unmap(cap_t cap, cte_t *ct_ਥਾਂ)
{
    ਜੇ (cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap));
        unmap_ਪੰਨਾ_ਸਾਰਣੀ(
            cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap),
            cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap),
            pt
        );
        clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *)pt, se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ);
    }
    cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ptr_ਸੈੱਟ_cap_PTIs_Mapped(&(ct_ਥਾਂ->cap), 0);

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

ਸਥਿਰ exception_t updatePTE(pte_t pte, pte_t *ਅਧਾਰ)
{
    *ਅਧਾਰ = pte;
    sfence();
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t perform_ਪੰਨਾ_Invocation_Map_PTE(cap_t cap, cte_t *ct_ਥਾਂ,
                                        pte_t pte, pte_t *ਅਧਾਰ)
{
    ct_ਥਾਂ->cap = cap;
    ਵਾਪਸ updatePTE(pte, ਅਧਾਰ);
}

exception_t perform_ਪੰਨਾ_Invocation_Unmap(cap_t cap, cte_t *ct_ਥਾਂ)
{
    ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        unmap_ਪੰਨਾ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
                  cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                  cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                  cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap)
                 );
    }

    cap_t ਥਾਂ_Cap = ct_ਥਾਂ->cap;
    ਥਾਂ_Cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(ਥਾਂ_Cap, 0);
    ਥਾਂ_Cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(ਥਾਂ_Cap, asidInvalid);
    ct_ਥਾਂ->cap = ਥਾਂ_Cap;

    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਖਾਲੀ Arch_user_ਢੇਰ_Trace(tcb_t *tptr)
{
    cap_t thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    ਜੇ (!is_Valid_VTable_ਜੜ੍ਹ(thread_ਜੜ੍ਹ)) {
        ਲਿਖੋ("Invalid vspace\n");
        ਵਾਪਸ;
    }

    ਸ਼ਬਦ_t sp = get_ਰਜਿਸਟਰ(tptr, SP);
    ਜੇ (!IS_ALIGNED(sp, se_L4_ਸ਼ਬਦ_ਮਾਪ_ਬਿੱਟ)) {
        ਲਿਖੋ("SP %p not aligned", (ਖਾਲੀ *) sp);
        ਵਾਪਸ;
    }

    pte_t *vspace_ਜੜ੍ਹ = PTE_PTR(pptr_ਦਾ_cap(thread_ਜੜ੍ਹ));
    ਲਈ (ਪੂਰਨ i = 0; i < CONFIG_USER_ਢੇਰ_TRACE_ਲੰਮਾਈ; i++) {
        ਸ਼ਬਦ_t ਪਤਾ = sp + (i * ਆਕਾਰ(ਸ਼ਬਦ_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_ਜੜ੍ਹ, ਪਤਾ);
        ਜੇ (ਪਤਾ < USER_ਸਿਖਰ && pte_ptr_get_valid(ret.pt_ਥਾਂ) && !is_PTEPage_ਸਾਰਣੀ(ret.pt_ਥਾਂ)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.pt_ਥਾਂ));
            ਸ਼ਬਦ_t *ਮੁੱਲ = (ਸ਼ਬਦ_t *)((ਸ਼ਬਦ_t)pptr + (ਪਤਾ & ਨਕਾਬ(ret.pt_ਬਿੱਟ_ਖੱਬਾ)));
            ਲਿਖੋ("0x%lx: 0x%lx\n", (ਲੰਮਾ) ਪਤਾ, (ਲੰਮਾ) *ਮੁੱਲ);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("0x%lx: INVALID\n", (ਲੰਮਾ) ਪਤਾ);
        }
    }
}
#ਜੇ_ਅੰਤ

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

    ਚੌਖਟਾ_ਮਾਪ = cap_ਚੌਖਟਾ_cap_get_cap_FSize(lu_ret.cap);

    ਜੇ (ਚੌਖਟਾ_ਮਾਪ != RISCV_Mega_ਪੰਨਾ) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(ਚੌਖਟਾ_cptr, false);

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਚੌਖਟਾ_pptr = cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_ਬਫਰ = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *) ਚੌਖਟਾ_pptr);

#ਜੇ __riscv_xlen == 32
    paddr_t physical_ਪਤਾ = ks_User_Log_ਬਫਰ;
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += 1) {
        kernel_image_level1_log_ਬਫਰ_pt[i] = pte_ਅਗਲਾ(physical_ਪਤਾ, true);
        physical_ਪਤਾ += ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ);
    }
    ਪ੍ਰਤਿਜ੍ਞਾ(physical_ਪਤਾ - ks_User_Log_ਬਫਰ == ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Large_ਪੰਨਾ_ਬਿੱਟ));
#ਨਹੀਂਤਾਂ
    kernel_image_level1_dev_pt[RISCV_GET_PT_ਸੂਚਕਾਂਕ(KS_LOG_PPTR, 1)] = pte_ਅਗਲਾ(ks_User_Log_ਬਫਰ, true);
#ਜੇ_ਅੰਤ

    sfence();

    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */
