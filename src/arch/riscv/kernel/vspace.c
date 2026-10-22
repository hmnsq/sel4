/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <benchmark/benchmark.h>
#ضمن <api/failures.h>
#ضمن <api/syscall.h>
#ضمن <kernel/boot.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <object/tcb.h>
#ضمن <machine/io.h>
#ضمن <model/preemption.h>
#ضمن <model/statedata.h>
#ضمن <object/cnode.h>
#ضمن <object/untyped.h>
#ضمن <arch/api/invocation.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <linker.h>
#ضمن <arch/machine.h>
#ضمن <plat/machine/hardware.h>
#ضمن <kernel/stack.h>
#ضمن <util.h>

بنية resolve_ret {
    paddr_t چوکٺو_بنياد;
    vm_صفحو_سائيز_t چوکٺو_سائيز;
    bool_t valid;
};
تعريفنوع بنية resolve_ret resolve_ret_t;

ساكن exception_t perform_صفحو_Get_پتو(فراغ *vbase_ptr, bool_t call);

ساكن لفظ_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    أرجع vm_rights == VMReadWrite;
}

ساكن مضمن لفظ_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported.
     * Kernel-only conveys no user rights. */
    أرجع vm_rights != VMKernelOnly;
}

ساكن مضمن bool_t is_PTEPage_جدول(pte_t *pte)
{
    أرجع pte_ptr_get_valid(pte) &&
           !(pte_ptr_get_read(pte) || pte_ptr_get_write(pte) || pte_ptr_get_execute(pte));
}

/** Helper function meant only to be used for mapping the kernel
 * window.
 *
 * Maps all pages with full RWX and supervisor perms by default.
 */
ساكن pte_t pte_ايندڙ(لفظ_t phys_addr, bool_t is_پن)
{
    لفظ_t ppn = (لفظ_t)(phys_addr >> 12);

    uint8_t اقرأ = is_پن ? 1 : 0;
    uint8_t اكتب = اقرأ;
    uint8_t exec = اقرأ;

    أرجع pte_new(ppn,
                   0,     /* sw */
                   is_پن ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_پن ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_پن ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   اكتب, /* write */
                   اقرأ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_ڪوڊ فراغ map_kernel_چوکٺو(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#إذا __riscv_xlen == 32
    paddr = ڦيرو_هيٺ_ڏانهن(paddr, RISCV_GET_LVL_PGSIZE_بٽ(0));
    أكد((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_پاڙ_صفحو_جدول[RISCV_GET_PT_اشاريو(vaddr, 0)] = pte_ايندڙ(paddr, true);
#وإلا
    إذا (vaddr >= KDEV_بنياد) {
        /* Map devices in level 1 page table */
        paddr = ڦيرو_هيٺ_ڏانهن(paddr, RISCV_GET_LVL_PGSIZE_بٽ(1));
        أكد((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_اشاريو(vaddr, 1)] = pte_ايندڙ(paddr, true);
    } وإلا {
        paddr = ڦيرو_هيٺ_ڏانهن(paddr, RISCV_GET_LVL_PGSIZE_بٽ(0));
        أكد((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_پاڙ_صفحو_جدول[RISCV_GET_PT_اشاريو(vaddr, 0)] = pte_ايندڙ(paddr, true);
    }
#جيڪڏهن_پورو
}

BOOT_ڪوڊ VISIBLE فراغ map_kernel_دري(فراغ)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    أكد(CONFIG_PT_سطحون > 1 && CONFIG_PT_سطحون <= 4);
    /* Kernel image finishes before KDEV_BASE */
    أكد(KERNEL_ELF_چوٽي <= KDEV_بنياد);

    /* kernel window starts at PPTR_BASE */
    لفظ_t pptr = PPTR_بنياد;

    /* first we map in memory from PADDR_BASE */
    لفظ_t paddr = PADDR_بنياد;
    طالما (pptr < PPTR_چوٽي) {
        أكد(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_بٽ(0)));
        أكد(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_بٽ(0)));

        kernel_پاڙ_صفحو_جدول[RISCV_GET_PT_اشاريو(pptr, 0)] = pte_ايندڙ(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    أكد(pptr == PPTR_چوٽي);
    pptr = ڦيرو_هيٺ_ڏانهن(KERNEL_ELF_بنياد, RISCV_GET_LVL_PGSIZE_بٽ(0));
    paddr = ڦيرو_هيٺ_ڏانهن(KERNEL_ELF_PADDR_بنياد, RISCV_GET_LVL_PGSIZE_بٽ(0));

#إذا __riscv_xlen == 32
    kernel_پاڙ_صفحو_جدول[RISCV_GET_PT_اشاريو(pptr, 0)] = pte_ايندڙ(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
    kernel_پاڙ_صفحو_جدول[RISCV_GET_PT_اشاريو(KS_LOG_PPTR, 0)] =
        pte_ايندڙ(kpptr_منزل_paddr(kernel_image_level1_log_بفر_pt), false);
#جيڪڏهن_پورو
#وإلا
    لفظ_t اشاريو = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_پاڙ_صفحو_جدول[RISCV_GET_PT_اشاريو(KERNEL_ELF_PADDR_بنياد + PPTR_بنياد_وٿي, 0)] =
        pte_ايندڙ(kpptr_منزل_paddr(kernel_image_level1_pt), false);
    kernel_پاڙ_صفحو_جدول[RISCV_GET_PT_اشاريو(pptr, 0)] =
        pte_ايندڙ(kpptr_منزل_paddr(kernel_image_level1_pt), false);
    طالما (pptr < PPTR_چوٽي + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[اشاريو] = pte_ايندڙ(paddr, true);
        اشاريو++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_پاڙ_صفحو_جدول[RISCV_GET_PT_اشاريو(KDEV_بنياد, 0)] =
        pte_ايندڙ(kpptr_منزل_paddr(kernel_image_level1_dev_pt), false);
#جيڪڏهن_پورو

    /* There should be free space where we put device mapping */
    أكد(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_ڪوڊ فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    lookupPTSlot_ret_t pt_ret;
    pte_t *هدف_جڳهه;
    vptr_t vptr = cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(pt_cap);
    pte_t *lvl1pt = PTE_PTR(pptr_جو_cap(vspace_cap));

    /* pt to be mapped */
    pte_t *pt   = PTE_PTR(pptr_جو_cap(pt_cap));

    /* Get PT slot to install the address in */
    pt_ret = lookupPTSlot(lvl1pt, vptr);

    هدف_جڳهه = pt_ret.pt_جڳهه;

    *هدف_جڳهه = pte_new(
                      (addrFromPPtr(pt) >> se_L4_صفحو_بٽ),
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

BOOT_ڪوڊ فراغ map_it_چوکٺو_cap(cap_t vspace_cap, cap_t چوکٺو_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_جو_cap(vspace_cap));
    pte_t *چوکٺو_pptr   = PTE_PTR(pptr_جو_cap(چوکٺو_cap));
    vptr_t چوکٺو_vptr = cap_چوکٺو_cap_get_cap_FMapped_پتو(چوکٺو_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, چوکٺو_vptr);
    أكد(lu_ret.pt_بٽ_کاٻو == se_L4_صفحو_بٽ);

    pte_t *هدف_جڳهه = lu_ret.pt_جڳهه;

    *هدف_جڳهه = pte_new(
                      (pptr_منزل_paddr(چوکٺو_pptr) >> se_L4_صفحو_بٽ),
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

BOOT_ڪوڊ cap_t create_unmapped_it_چوکٺو_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_چوکٺو_cap_new(
                    asidInvalid,                     /* capFMappedASID       */
                    pptr,                            /* capFBasePtr          */
                    0,                               /* capFSize             */
                    0,                               /* capFVMRights         */
                    0,
                    0                                /* capFMappedAddress    */
                );

    أرجع cap;
}

/* Create a page table for the initial thread */
ساكن BOOT_ڪوڊ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحو_جدول_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}

BOOT_ڪوڊ لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    لفظ_t n = 0;
    لكل (صحيح i = 0; i < CONFIG_PT_سطحون - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_بٽ(i));
    }
    أرجع n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_ڪوڊ cap_t create_it_پتو_space(cap_t پاڙ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      lvl1pt_cap;
    vptr_t     pt_vptr;

    copyGlobalMappings(PTE_PTR(rootserver.vspace));

    lvl1pt_cap =
        cap_صفحو_جدول_cap_new(
            IT_ASID,               /* capPTMappedASID    */
            (لفظ_t) rootserver.vspace,  /* capPTBasePtr       */
            1,                     /* capPTIsMapped      */
            (لفظ_t) rootserver.vspace   /* capPTMappedAddress */
        );

    se_L4_جڳهه_Pos جڳهه_pos_before = ndks_boot.جڳهه_pos_cur;
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    لكل (صحيح i = 0; i < CONFIG_PT_سطحون - 1; i++) {

        لكل (pt_vptr = ڦيرو_هيٺ_ڏانهن(it_v_reg.شروعات, RISCV_GET_LVL_PGSIZE_بٽ(i));
             pt_vptr < it_v_reg.پڇاڙي;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            إذا (!provide_cap(پاڙ_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                أرجع cap_null_cap_new();
            }
        }

    }

    se_L4_جڳهه_Pos جڳهه_pos_after = ndks_boot.جڳهه_pos_cur;
    ndks_boot.bi_چوکٺو->userImagePaging = (se_L4_جڳهه_Region) {
        جڳهه_pos_before, جڳهه_pos_after
    };

    أرجع lvl1pt_cap;
}

BOOT_ڪوڊ فراغ activate_kernel_vspace(فراغ)
{
    سيٽ_VSpace_پاڙ(kpptr_منزل_paddr(&kernel_پاڙ_صفحو_جدول), 0);
}

BOOT_ڪوڊ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_جو_cap(it_ap_cap));
    ap->array[ASID_هيٺيون(IT_ASID)] = PTE_PTR(pptr_جو_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_مٿاهون(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ساكن ڳوليو_VSpace_For_ASID_ret_t ڳوليو_VSpace_For_ASID(asid_t asid)
{
    ڳوليو_VSpace_For_ASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_پاڙ;

    poolPtr = riscvKSASIDTable[ASID_مٿاهون(asid)];
    إذا (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_پاڙ_new();

        ret.vspace_پاڙ = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    vspace_پاڙ = poolPtr->array[ASID_هيٺيون(asid)];
    إذا (!vspace_پاڙ) {
        current_lookup_fault = lookup_fault_invalid_پاڙ_new();

        ret.vspace_پاڙ = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_پاڙ = vspace_پاڙ;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

فراغ copyGlobalMappings(pte_t *newLvl1pt)
{
    بنانشان ڊگهو i;
    pte_t *global_kernel_vspace = kernel_پاڙ_صفحو_جدول;

    لكل (i = RISCV_GET_PT_اشاريو(PPTR_بنياد, 0); i < ٻٽو_انگ(PT_اشاريو_بٽ); i++) {
        newLvl1pt[i] = global_kernel_vspace[i];
    }
}

لفظ_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    لفظ_t w_بفر_Ptr;
    cap_t بفر_Cap;
    vm_rights_t vm_rights;

    w_بفر_Ptr = thread->tcbIPCBuffer;
    بفر_Cap = TCB_PTR_CTE_PTR(thread, tcb_بفر)->cap;

    إذا (unlikely(cap_get_capType(بفر_Cap) != cap_چوکٺو_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(cap_چوکٺو_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_چوکٺو_cap_get_cap_FVMRights(بفر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        لفظ_t بنياد_Ptr, صفحو_بٽ_2;

        بنياد_Ptr = cap_چوکٺو_cap_get_cap_FBase_Ptr(بفر_Cap);
        صفحو_بٽ_2 = صفحو_بٽ_For_سائيز(cap_چوکٺو_cap_get_cap_FSize(بفر_Cap));
        أرجع (لفظ_t *)(بنياد_Ptr + (w_بفر_Ptr & نقاب(صفحو_بٽ_2)));
    } وإلا {
        أرجع عدم;
    }
}

ساكن مضمن pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    أرجع PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << se_L4_صفحو_جدول_بٽ));
}

lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    لفظ_t level = CONFIG_PT_سطحون - 1;
    pte_t *pt = lvl1pt;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_بٽ_کاٻو = PT_اشاريو_بٽ * level + se_L4_صفحو_بٽ;
    ret.pt_جڳهه = pt + ((vptr >> ret.pt_بٽ_کاٻو) & نقاب(PT_اشاريو_بٽ));

    طالما (is_PTEPage_جدول(ret.pt_جڳهه) && likely(0 < level)) {
        level--;
        ret.pt_بٽ_کاٻو -= PT_اشاريو_بٽ;
        pt = getPPtrFromHWPTE(ret.pt_جڳهه);
        ret.pt_جڳهه = pt + ((vptr >> ret.pt_بٽ_کاٻو) & نقاب(PT_اشاريو_بٽ));
    }

    أرجع ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    بدل (vm_faultType) {
    حالة RISCVLoad_صفحو_Fault:
    حالة RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        أرجع EXCEPTION_FAULT;
    حالة RISCVStore_صفحو_Fault:
    حالة RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        أرجع EXCEPTION_FAULT;
    حالة RISCVInstruction_صفحو_Fault:
    حالة RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        أرجع EXCEPTION_FAULT;

    افتراضي:
        fail("Invalid VM fault type");
    }
}

فراغ deleteASIDPool(asid_t asid_بنياد, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد(IS_ALIGNED(asid_بنياد, asid_هيٺيون_بٽ));

    إذا (riscvKSASIDTable[ASID_مٿاهون(asid_بنياد)] == pool) {
        riscvKSASIDTable[ASID_مٿاهون(asid_بنياد)] = عدم;
        سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
    }
}

ساكن exception_t performASIDControlInvocation(فراغ *چوکٺو, cte_t *جڳهه, cte_t *والد, asid_t asid_بنياد)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سيٽ_cap_Free_اشاريو(&(والد->cap),
                                         MAX_FREE_اشاريو(cap_untyped_cap_get_cap_بلاڪ_سائيز(والد->cap)));

    memzero(چوکٺو, ٻٽو_انگ(صفحو_بٽ_For_سائيز(RISCV_4_K_صفحو)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_وجهو(
        cap_asid_pool_cap_new(
            asid_بنياد,          /* capASIDBase  */
            لفظ_REF(چوکٺو)     /* capASIDPool  */
        ),
        والد,
        جڳهه
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنياد & نقاب(asid_هيٺيون_بٽ)) == 0);
    riscvKSASIDTable[ASID_مٿاهون(asid_بنياد)] = (asid_pool_t *)چوکٺو;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_جڳهه)
{
    cap_t cap = vspace_Cap_جڳهه->cap;
    pte_t *region_بنياد = PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap));
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTMapped_پتو(cap, 0);
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTIs_Mapped(cap, 1);
    vspace_Cap_جڳهه->cap = cap;

    copyGlobalMappings(region_بنياد);

    poolPtr->array[ASID_هيٺيون(asid)] = region_بنياد;

    أرجع EXCEPTION_NONE;
}

فراغ deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_مٿاهون(asid)];
    إذا (poolPtr != عدم && poolPtr->array[ASID_هيٺيون(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_هيٺيون(asid)] = عدم;
        سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
    }
}

فراغ unmap_صفحو_جدول(asid_t asid, vptr_t vptr, pte_t *هدف_pt)
{
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
        /* nothing to do */
        أرجع;
    }
    /* We won't ever unmap a top level page table */
    أكد(ڳوليو_ret.vspace_پاڙ != هدف_pt);
    pte_t *pt_جڳهه = عدم;
    pte_t *pt = ڳوليو_ret.vspace_پاڙ;

    لكل (لفظ_t i = 0; i < CONFIG_PT_سطحون - 1 && pt != هدف_pt; i++) {
        pt_جڳهه = pt + RISCV_GET_PT_اشاريو(vptr, i);
        إذا (unlikely(!is_PTEPage_جدول(pt_جڳهه))) {
            /* couldn't find it */
            أرجع;
        }
        pt = getPPtrFromHWPTE(pt_جڳهه);
    }

    إذا (pt != هدف_pt) {
        /* didn't find it */
        أرجع;
    }
    /* If we found a pt then ptSlot won't be null */
    أكد(pt_جڳهه != عدم);
    *pt_جڳهه = pte_new(
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

ساكن pte_t pte_pte_invalid_new(فراغ)
{
    أرجع (pte_t) {
        0
    };
}

فراغ unmap_صفحو(vm_صفحو_سائيز_t صفحو_سائيز, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;
    lookupPTSlot_ret_t  lu_ret;

    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (ڳوليو_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPTSlot(ڳوليو_ret.vspace_پاڙ, vptr);
    إذا (unlikely(lu_ret.pt_بٽ_کاٻو != صفحو_بٽ_For_سائيز(صفحو_سائيز))) {
        أرجع;
    }
    إذا (!pte_ptr_get_valid(lu_ret.pt_جڳهه) || is_PTEPage_جدول(lu_ret.pt_جڳهه)
        || (pte_ptr_get_ppn(lu_ret.pt_جڳهه) << se_L4_صفحو_بٽ) != pptr_منزل_paddr((فراغ *)pptr)) {
        أرجع;
    }

    lu_ret.pt_جڳهه[0] = pte_pte_invalid_new();
    sfence();
}

فراغ سيٽ_VMRoot(tcb_t *tcb)
{
    cap_t thread_پاڙ;
    asid_t asid;
    pte_t *lvl1pt;
    ڳوليو_VSpace_For_ASID_ret_t  ڳوليو_ret;

    thread_پاڙ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_پاڙ) != cap_صفحو_جدول_cap) {
        سيٽ_VSpace_پاڙ(kpptr_منزل_paddr(&kernel_پاڙ_صفحو_جدول), 0);
        أرجع;
    }

    lvl1pt = PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(thread_پاڙ));

    asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(thread_پاڙ);
    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE || ڳوليو_ret.vspace_پاڙ != lvl1pt)) {
        سيٽ_VSpace_پاڙ(kpptr_منزل_paddr(&kernel_پاڙ_صفحو_جدول), 0);
        أرجع;
    }

    سيٽ_VSpace_پاڙ(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_پاڙ(cap_t cap)
{
    أرجع (cap_get_capType(cap) == cap_صفحو_جدول_cap &&
            cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (unlikely(cap_get_capType(cap) != cap_چوکٺو_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_چوکٺو_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(!IS_ALIGNED(vptr, se_L4_IPCBuffer_سائيز_بٽ))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

vm_rights_t CONST نقاب_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_نقاب)
{
    إذا (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        أرجع VMReadOnly;
    }
    إذا (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
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

/* The rest of the file implements the RISCV object invocations */

ساكن pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    لفظ_t اكتب = RISCVGetWriteFromVMRights(vm_rights);
    لفظ_t اقرأ = RISCVGetReadFromVMRights(vm_rights);
    إذا (unlikely(!اقرأ && !اكتب && !executable)) {
        أرجع pte_pte_invalid_new();
    } وإلا {
        أرجع pte_new(
                   paddr >> se_L4_صفحو_بٽ,
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

ساكن مضمن bool_t CONST checkVPAlignment(vm_صفحو_سائيز_t sz, لفظ_t w)
{
    أرجع (w & نقاب(صفحو_بٽ_For_سائيز(sz))) == 0;
}

ساكن exception_t decode_RISCVPage_جدول_Invocation(لفظ_t label, لفظ_t ڊيگهائي,
                                                  cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    إذا (label == RISCVPage_جدول_Unmap) {
        إذا (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = se_L4_Revoke_پهريون;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        إذا (likely(cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap))) {
            asid_t asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap);
            ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap));
            إذا (unlikely(ڳوليو_ret.حالت == EXCEPTION_NONE &&
                         ڳوليو_ret.vspace_پاڙ == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = se_L4_Revoke_پهريون;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely((label != RISCVPage_جدول_Map))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(ڊيگهائي < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t vaddr = getSyscallArg(0, بفر);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(cap_get_capType(lvl1ptCap) != cap_صفحو_جدول_cap ||
                 cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(lvl1ptCap));
    asid_t asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(lvl1ptCap);

    إذا (unlikely(vaddr >= USER_چوٽي)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(ڳوليو_ret.vspace_پاڙ != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    إذا (lu_ret.pt_بٽ_کاٻو == se_L4_صفحو_بٽ || pte_ptr_get_valid(lu_ret.pt_جڳهه)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = se_L4_Delete_پهريون;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Get the slot to install the PT in */
    pte_t *pt_جڳهه = lu_ret.pt_جڳهه;

    paddr_t paddr = addrFromPPtr(
                        PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap)));
    pte_t pte = pte_new((paddr >> se_L4_صفحو_بٽ),
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

    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTMapped_پتو(cap, (vaddr & ~نقاب(lu_ret.pt_بٽ_کاٻو)));

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_صفحو_جدول_Invocation_Map(cap, cte, pte, pt_جڳهه);
}

ساكن exception_t decodeRISCVFrameInvocation(لفظ_t label, لفظ_t ڊيگهائي,
                                              cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (label) {
    حالة RISCVPageMap: {
        إذا (unlikely(ڊيگهائي < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t vaddr = getSyscallArg(0, بفر);
        لفظ_t w_rights_نقاب = getSyscallArg(1, بفر);
        vm_خاصيتون_t attr = vm_خاصيتون_From_لفظ(getSyscallArg(2, بفر));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_صفحو_سائيز_t چوکٺو_سائيز = cap_چوکٺو_cap_get_cap_FSize(cap);
        vm_rights_t capVMRights = cap_چوکٺو_cap_get_cap_FVMRights(cap);

        إذا (unlikely(cap_get_capType(lvl1ptCap) != cap_صفحو_جدول_cap ||
                     !cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(lvl1ptCap));
        asid_t asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(lvl1ptCap);

        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(ڳوليو_ret.vspace_پاڙ != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        لفظ_t vtop = vaddr + ٻٽو_انگ(صفحو_بٽ_For_سائيز(چوکٺو_سائيز)) - 1;
        إذا (unlikely(vtop >= USER_چوٽي)) {
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (unlikely(!checkVPAlignment(چوکٺو_سائيز, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        إذا (unlikely(lu_ret.pt_بٽ_کاٻو != صفحو_بٽ_For_سائيز(چوکٺو_سائيز))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_بٽ_کاٻو);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_t چوکٺو_asid = cap_چوکٺو_cap_get_cap_FMapped_ASID(cap);
        إذا (unlikely(چوکٺو_asid != asidInvalid)) {
            /* this frame is already mapped */
            إذا (چوکٺو_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
            لفظ_t mapped_vaddr = cap_چوکٺو_cap_get_cap_FMapped_پتو(cap);
            إذا (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دليل;
                current_syscall_error.invalid_دليل_عدد = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            إذا (unlikely(is_PTEPage_جدول(lu_ret.pt_جڳهه))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = se_L4_Delete_پهريون;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            /* check this vaddr isn't already mapped */
            إذا (unlikely(pte_ptr_get_valid(lu_ret.pt_جڳهه))) {
                userError("Virtual address (0x%"SEL4_PRIx_لفظ") already mapped", vaddr);
                current_syscall_error.type = se_L4_Delete_پهريون;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = نقاب_VMRights(capVMRights, rights_From_لفظ(w_rights_نقاب));
        paddr_t چوکٺو_paddr = addrFromPPtr((فراغ *) cap_چوکٺو_cap_get_cap_FBase_Ptr(cap));
        cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_پتو(cap,  vaddr);

        bool_t executable = !vm_خاصيتون_get_riscv_Execute_Never(attr);
        pte_t pte = makeUserPTE(چوکٺو_paddr, executable, vmRights);
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_Invocation_Map_PTE(cap, cte, pte, lu_ret.pt_جڳهه);
    }

    حالة RISCVPageUnmap: {
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_Invocation_Unmap(cap, cte);
    }

    حالة RISCVPage_Get_پتو: {

        /* Check that there are enough message registers */
        أكد(n_نياپو_Registers >= 1);

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_Get_پتو((فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(لفظ_t label, لفظ_t ڊيگهائي, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_صفحو_جدول_cap:
        أرجع decode_RISCVPage_جدول_Invocation(label, ڊيگهائي, cte, cap, بفر);

    حالة cap_چوکٺو_cap:
        أرجع decodeRISCVFrameInvocation(label, ڊيگهائي, cte, cap, call, بفر);

    حالة cap_asid_control_cap: {
        لفظ_t     i;
        asid_t           asid_بنياد;
        لفظ_t           اشاريو;
        لفظ_t           اونهائي;
        cap_t            untyped;
        cap_t            پاڙ;
        cte_t           *والد_جڳهه;
        cte_t           *dest_جڳهه;
        lookup_جڳهه_ret_t lu_ret;
        فراغ            *چوکٺو;
        exception_t      حالت;

        إذا (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (ڊيگهائي < 2 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        اشاريو = getSyscallArg(0, بفر);
        اونهائي = getSyscallArg(1, بفر);
        والد_جڳهه = current_extra_caps.excaprefs[0];
        untyped = والد_جڳهه->cap;
        پاڙ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        إذا (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پهريون;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنياد = i << asid_هيٺيون_بٽ;

        إذا (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_بلاڪ_سائيز(untyped) != se_L4_ASIDPool_بٽ ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(والد_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        چوکٺو = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_هدف_جڳهه(پاڙ, اشاريو, اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        dest_جڳهه = lu_ret.جڳهه;

        حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکٺو, dest_جڳهه, والد_جڳهه, asid_بنياد);
    }

    حالة cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_جڳهه;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t       asid;

        إذا (label != RISCVASIDPool_سونپيو) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پيغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_جڳهه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_جڳهه->cap;

        إذا (unlikely(
                cap_get_capType(vspaceCap) != cap_صفحو_جدول_cap ||
                cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_مٿاهون(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            current_lookup_fault = lookup_fault_invalid_پاڙ_new();
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < ٻٽو_انگ(asid_هيٺيون_بٽ) && (asid + i == 0 || pool->array[i]); i++);

        إذا (i == ٻٽو_انگ(asid_هيٺيون_بٽ)) {
            current_syscall_error.type = se_L4_Delete_پهريون;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_جڳهه);
    }
    افتراضي:
        fail("Invalid arch cap type");
    }
}

exception_t perform_صفحو_جدول_Invocation_Map(cap_t cap, cte_t *ct_جڳهه,
                                          pte_t pte, pte_t *pt_جڳهه)
{
    ct_جڳهه->cap = cap;
    *pt_جڳهه = pte;
    sfence();

    أرجع EXCEPTION_NONE;
}

exception_t perform_صفحو_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_جڳهه)
{
    إذا (cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحو_جدول(
            cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(cap),
            pt
        );
        clear_يادگيري((فراغ *)pt, se_L4_صفحو_جدول_بٽ);
    }
    cap_صفحو_جدول_cap_ptr_سيٽ_cap_PTIs_Mapped(&(ct_جڳهه->cap), 0);

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

ساكن exception_t updatePTE(pte_t pte, pte_t *بنياد)
{
    *بنياد = pte;
    sfence();
    أرجع EXCEPTION_NONE;
}

exception_t perform_صفحو_Invocation_Map_PTE(cap_t cap, cte_t *ct_جڳهه,
                                        pte_t pte, pte_t *بنياد)
{
    ct_جڳهه->cap = cap;
    أرجع updatePTE(pte, بنياد);
}

exception_t perform_صفحو_Invocation_Unmap(cap_t cap, cte_t *ct_جڳهه)
{
    إذا (cap_چوکٺو_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        unmap_صفحو(cap_چوکٺو_cap_get_cap_FSize(cap),
                  cap_چوکٺو_cap_get_cap_FMapped_ASID(cap),
                  cap_چوکٺو_cap_get_cap_FMapped_پتو(cap),
                  cap_چوکٺو_cap_get_cap_FBase_Ptr(cap)
                 );
    }

    cap_t جڳهه_Cap = ct_جڳهه->cap;
    جڳهه_Cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_پتو(جڳهه_Cap, 0);
    جڳهه_Cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_ASID(جڳهه_Cap, asidInvalid);
    ct_جڳهه->cap = جڳهه_Cap;

    أرجع EXCEPTION_NONE;
}

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
فراغ Arch_user_ڍير_Trace(tcb_t *tptr)
{
    cap_t thread_پاڙ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    إذا (!is_Valid_VTable_پاڙ(thread_پاڙ)) {
        لکو("Invalid vspace\n");
        أرجع;
    }

    لفظ_t sp = get_رجسٽر(tptr, SP);
    إذا (!IS_ALIGNED(sp, se_L4_لفظ_سائيز_بٽ)) {
        لکو("SP %p not aligned", (فراغ *) sp);
        أرجع;
    }

    pte_t *vspace_پاڙ = PTE_PTR(pptr_جو_cap(thread_پاڙ));
    لكل (صحيح i = 0; i < CONFIG_USER_ڍير_TRACE_ڊيگهائي; i++) {
        لفظ_t پتو = sp + (i * حجم(لفظ_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_پاڙ, پتو);
        إذا (پتو < USER_چوٽي && pte_ptr_get_valid(ret.pt_جڳهه) && !is_PTEPage_جدول(ret.pt_جڳهه)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.pt_جڳهه));
            لفظ_t *قدر = (لفظ_t *)((لفظ_t)pptr + (پتو & نقاب(ret.pt_بٽ_کاٻو)));
            لکو("0x%lx: 0x%lx\n", (ڊگهو) پتو, (ڊگهو) *قدر);
        } وإلا {
            لکو("0x%lx: INVALID\n", (ڊگهو) پتو);
        }
    }
}
#جيڪڏهن_پورو

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

    چوکٺو_سائيز = cap_چوکٺو_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکٺو_سائيز != RISCV_Mega_صفحو) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکٺو_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکٺو_pptr = cap_چوکٺو_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکٺو_pptr);

#إذا __riscv_xlen == 32
    paddr_t physical_پتو = ks_User_Log_بفر;
    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PT_اشاريو_بٽ); i += 1) {
        kernel_image_level1_log_بفر_pt[i] = pte_ايندڙ(physical_پتو, true);
        physical_پتو += ٻٽو_انگ(صفحو_بٽ);
    }
    أكد(physical_پتو - ks_User_Log_بفر == ٻٽو_انگ(se_L4_Large_صفحو_بٽ));
#وإلا
    kernel_image_level1_dev_pt[RISCV_GET_PT_اشاريو(KS_LOG_PPTR, 1)] = pte_ايندڙ(ks_User_Log_بفر, true);
#جيڪڏهن_پورو

    sfence();

    أرجع EXCEPTION_NONE;
}
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */
