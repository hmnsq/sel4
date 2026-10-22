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
    paddr_t چوکاټ_بنسټ;
    vm_پاڼه_اندازه_t چوکاټ_اندازه;
    bool_t valid;
};
تعريفنوع بنية resolve_ret resolve_ret_t;

ساكن exception_t perform_پاڼه_Get_پته(فراغ *vbase_ptr, bool_t call);

ساكن کلمه_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    أرجع vm_rights == VMReadWrite;
}

ساكن مضمن کلمه_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
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
ساكن pte_t pte_بل(کلمه_t phys_addr, bool_t is_ونې_پاڼه)
{
    کلمه_t ppn = (کلمه_t)(phys_addr >> 12);

    uint8_t اقرأ = is_ونې_پاڼه ? 1 : 0;
    uint8_t اكتب = اقرأ;
    uint8_t exec = اقرأ;

    أرجع pte_new(ppn,
                   0,     /* sw */
                   is_ونې_پاڼه ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_ونې_پاڼه ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_ونې_پاڼه ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   اكتب, /* write */
                   اقرأ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_کوډ فراغ map_kernel_چوکاټ(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#إذا __riscv_xlen == 32
    paddr = پړاو_ښکته(paddr, RISCV_GET_LVL_PGSIZE_بټونه(0));
    أكد((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_ریښه_پاڼه_جدول[RISCV_GET_PT_شاخص(vaddr, 0)] = pte_بل(paddr, true);
#وإلا
    إذا (vaddr >= KDEV_بنسټ) {
        /* Map devices in level 1 page table */
        paddr = پړاو_ښکته(paddr, RISCV_GET_LVL_PGSIZE_بټونه(1));
        أكد((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_شاخص(vaddr, 1)] = pte_بل(paddr, true);
    } وإلا {
        paddr = پړاو_ښکته(paddr, RISCV_GET_LVL_PGSIZE_بټونه(0));
        أكد((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_ریښه_پاڼه_جدول[RISCV_GET_PT_شاخص(vaddr, 0)] = pte_بل(paddr, true);
    }
#د_که_پای
}

BOOT_کوډ VISIBLE فراغ map_kernel_کړکۍ(فراغ)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    أكد(CONFIG_PT_پوړونه > 1 && CONFIG_PT_پوړونه <= 4);
    /* Kernel image finishes before KDEV_BASE */
    أكد(KERNEL_ELF_سر <= KDEV_بنسټ);

    /* kernel window starts at PPTR_BASE */
    کلمه_t pptr = PPTR_بنسټ;

    /* first we map in memory from PADDR_BASE */
    کلمه_t paddr = PADDR_بنسټ;
    طالما (pptr < PPTR_سر) {
        أكد(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_بټونه(0)));
        أكد(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_بټونه(0)));

        kernel_ریښه_پاڼه_جدول[RISCV_GET_PT_شاخص(pptr, 0)] = pte_بل(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    أكد(pptr == PPTR_سر);
    pptr = پړاو_ښکته(KERNEL_ELF_بنسټ, RISCV_GET_LVL_PGSIZE_بټونه(0));
    paddr = پړاو_ښکته(KERNEL_ELF_PADDR_بنسټ, RISCV_GET_LVL_PGSIZE_بټونه(0));

#إذا __riscv_xlen == 32
    kernel_ریښه_پاڼه_جدول[RISCV_GET_PT_شاخص(pptr, 0)] = pte_بل(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
    kernel_ریښه_پاڼه_جدول[RISCV_GET_PT_شاخص(KS_LOG_PPTR, 0)] =
        pte_بل(kpptr_منزل_paddr(kernel_image_level1_log_بفر_pt), false);
#د_که_پای
#وإلا
    کلمه_t شاخص = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_ریښه_پاڼه_جدول[RISCV_GET_PT_شاخص(KERNEL_ELF_PADDR_بنسټ + PPTR_بنسټ_دننه_واټن, 0)] =
        pte_بل(kpptr_منزل_paddr(kernel_image_level1_pt), false);
    kernel_ریښه_پاڼه_جدول[RISCV_GET_PT_شاخص(pptr, 0)] =
        pte_بل(kpptr_منزل_paddr(kernel_image_level1_pt), false);
    طالما (pptr < PPTR_سر + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[شاخص] = pte_بل(paddr, true);
        شاخص++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_ریښه_پاڼه_جدول[RISCV_GET_PT_شاخص(KDEV_بنسټ, 0)] =
        pte_بل(kpptr_منزل_paddr(kernel_image_level1_dev_pt), false);
#د_که_پای

    /* There should be free space where we put device mapping */
    أكد(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_کوډ فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    lookupPTSlot_ret_t pt_ret;
    pte_t *موخه_خونه;
    vptr_t vptr = cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(pt_cap);
    pte_t *lvl1pt = PTE_PTR(pptr_د_cap(vspace_cap));

    /* pt to be mapped */
    pte_t *pt   = PTE_PTR(pptr_د_cap(pt_cap));

    /* Get PT slot to install the address in */
    pt_ret = lookupPTSlot(lvl1pt, vptr);

    موخه_خونه = pt_ret.pt_خونه;

    *موخه_خونه = pte_new(
                      (addrFromPPtr(pt) >> se_L4_پاڼه_بټونه),
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

BOOT_کوډ فراغ map_it_چوکاټ_cap(cap_t vspace_cap, cap_t چوکاټ_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_د_cap(vspace_cap));
    pte_t *چوکاټ_pptr   = PTE_PTR(pptr_د_cap(چوکاټ_cap));
    vptr_t چوکاټ_vptr = cap_چوکاټ_cap_get_cap_FMapped_پته(چوکاټ_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, چوکاټ_vptr);
    أكد(lu_ret.pt_بټونه_کیڼ == se_L4_پاڼه_بټونه);

    pte_t *موخه_خونه = lu_ret.pt_خونه;

    *موخه_خونه = pte_new(
                      (pptr_منزل_paddr(چوکاټ_pptr) >> se_L4_پاڼه_بټونه),
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

BOOT_کوډ cap_t create_unmapped_it_چوکاټ_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_چوکاټ_cap_new(
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
ساكن BOOT_کوډ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_پاڼه_جدول_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}

BOOT_کوډ کلمه_t arch_get_n_paging(v_region_t it_v_reg)
{
    کلمه_t n = 0;
    لكل (صحيح i = 0; i < CONFIG_PT_پوړونه - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_بټونه(i));
    }
    أرجع n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_کوډ cap_t create_it_پته_space(cap_t ریښه_cnode_cap, v_region_t it_v_reg)
{
    cap_t      lvl1pt_cap;
    vptr_t     pt_vptr;

    copyGlobalMappings(PTE_PTR(rootserver.vspace));

    lvl1pt_cap =
        cap_پاڼه_جدول_cap_new(
            IT_ASID,               /* capPTMappedASID    */
            (کلمه_t) rootserver.vspace,  /* capPTBasePtr       */
            1,                     /* capPTIsMapped      */
            (کلمه_t) rootserver.vspace   /* capPTMappedAddress */
        );

    se_L4_خونه_Pos خونه_pos_before = ndks_boot.خونه_pos_cur;
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    لكل (صحيح i = 0; i < CONFIG_PT_پوړونه - 1; i++) {

        لكل (pt_vptr = پړاو_ښکته(it_v_reg.پیل, RISCV_GET_LVL_PGSIZE_بټونه(i));
             pt_vptr < it_v_reg.پای_ټکی;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            إذا (!provide_cap(ریښه_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                أرجع cap_null_cap_new();
            }
        }

    }

    se_L4_خونه_Pos خونه_pos_after = ndks_boot.خونه_pos_cur;
    ndks_boot.bi_چوکاټ->userImagePaging = (se_L4_خونه_Region) {
        خونه_pos_before, خونه_pos_after
    };

    أرجع lvl1pt_cap;
}

BOOT_کوډ فراغ activate_kernel_vspace(فراغ)
{
    سېټ_VSpace_ریښه(kpptr_منزل_paddr(&kernel_ریښه_پاڼه_جدول), 0);
}

BOOT_کوډ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_د_cap(it_ap_cap));
    ap->array[ASID_ټیټ(IT_ASID)] = PTE_PTR(pptr_د_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_لوړ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ساكن پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_VSpace_For_ASID(asid_t asid)
{
    پیدا_کړه_VSpace_For_ASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_ریښه;

    poolPtr = riscvKSASIDTable[ASID_لوړ(asid)];
    إذا (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_ریښه_new();

        ret.vspace_ریښه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    vspace_ریښه = poolPtr->array[ASID_ټیټ(asid)];
    إذا (!vspace_ریښه) {
        current_lookup_fault = lookup_fault_invalid_ریښه_new();

        ret.vspace_ریښه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_ریښه = vspace_ریښه;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

فراغ copyGlobalMappings(pte_t *newLvl1pt)
{
    بینښه اوږد i;
    pte_t *global_kernel_vspace = kernel_ریښه_پاڼه_جدول;

    لكل (i = RISCV_GET_PT_شاخص(PPTR_بنسټ, 0); i < بټ(PT_شاخص_بټونه); i++) {
        newLvl1pt[i] = global_kernel_vspace[i];
    }
}

کلمه_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    کلمه_t w_بفر_Ptr;
    cap_t بفر_Cap;
    vm_rights_t vm_rights;

    w_بفر_Ptr = thread->tcbIPCBuffer;
    بفر_Cap = TCB_PTR_CTE_PTR(thread, tcb_بفر)->cap;

    إذا (unlikely(cap_get_capType(بفر_Cap) != cap_چوکاټ_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(cap_چوکاټ_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_چوکاټ_cap_get_cap_FVMRights(بفر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        کلمه_t بنسټ_Ptr, پاڼه_بټونه_2;

        بنسټ_Ptr = cap_چوکاټ_cap_get_cap_FBase_Ptr(بفر_Cap);
        پاڼه_بټونه_2 = پاڼه_بټونه_For_اندازه(cap_چوکاټ_cap_get_cap_FSize(بفر_Cap));
        أرجع (کلمه_t *)(بنسټ_Ptr + (w_بفر_Ptr & ماسک(پاڼه_بټونه_2)));
    } وإلا {
        أرجع عدم;
    }
}

ساكن مضمن pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    أرجع PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << se_L4_پاڼه_جدول_بټونه));
}

lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    کلمه_t level = CONFIG_PT_پوړونه - 1;
    pte_t *pt = lvl1pt;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_بټونه_کیڼ = PT_شاخص_بټونه * level + se_L4_پاڼه_بټونه;
    ret.pt_خونه = pt + ((vptr >> ret.pt_بټونه_کیڼ) & ماسک(PT_شاخص_بټونه));

    طالما (is_PTEPage_جدول(ret.pt_خونه) && likely(0 < level)) {
        level--;
        ret.pt_بټونه_کیڼ -= PT_شاخص_بټونه;
        pt = getPPtrFromHWPTE(ret.pt_خونه);
        ret.pt_خونه = pt + ((vptr >> ret.pt_بټونه_کیڼ) & ماسک(PT_شاخص_بټونه));
    }

    أرجع ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    بدل (vm_faultType) {
    حالة RISCVLoad_پاڼه_Fault:
    حالة RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        أرجع EXCEPTION_FAULT;
    حالة RISCVStore_پاڼه_Fault:
    حالة RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        أرجع EXCEPTION_FAULT;
    حالة RISCVInstruction_پاڼه_Fault:
    حالة RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        أرجع EXCEPTION_FAULT;

    افتراضي:
        fail("Invalid VM fault type");
    }
}

فراغ deleteASIDPool(asid_t asid_بنسټ, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد(IS_ALIGNED(asid_بنسټ, asid_ټیټ_بټونه));

    إذا (riscvKSASIDTable[ASID_لوړ(asid_بنسټ)] == pool) {
        riscvKSASIDTable[ASID_لوړ(asid_بنسټ)] = عدم;
        سېټ_VMRoot(غوټه_STATE(ksCurThread));
    }
}

ساكن exception_t performASIDControlInvocation(فراغ *چوکاټ, cte_t *خونه, cte_t *پلار, asid_t asid_بنسټ)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سېټ_cap_Free_شاخص(&(پلار->cap),
                                         MAX_FREE_شاخص(cap_untyped_cap_get_cap_بلاک_اندازه(پلار->cap)));

    memzero(چوکاټ, بټ(پاڼه_بټونه_For_اندازه(RISCV_4_K_پاڼه)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ورننباسه(
        cap_asid_pool_cap_new(
            asid_بنسټ,          /* capASIDBase  */
            کلمه_REF(چوکاټ)     /* capASIDPool  */
        ),
        پلار,
        خونه
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنسټ & ماسک(asid_ټیټ_بټونه)) == 0);
    riscvKSASIDTable[ASID_لوړ(asid_بنسټ)] = (asid_pool_t *)چوکاټ;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_خونه)
{
    cap_t cap = vspace_Cap_خونه->cap;
    pte_t *region_بنسټ = PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap));
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTMapped_ASID(cap, asid);
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTMapped_پته(cap, 0);
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTIs_Mapped(cap, 1);
    vspace_Cap_خونه->cap = cap;

    copyGlobalMappings(region_بنسټ);

    poolPtr->array[ASID_ټیټ(asid)] = region_بنسټ;

    أرجع EXCEPTION_NONE;
}

فراغ deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_لوړ(asid)];
    إذا (poolPtr != عدم && poolPtr->array[ASID_ټیټ(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_ټیټ(asid)] = عدم;
        سېټ_VMRoot(غوټه_STATE(ksCurThread));
    }
}

فراغ unmap_پاڼه_جدول(asid_t asid, vptr_t vptr, pte_t *موخه_pt)
{
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
        /* nothing to do */
        أرجع;
    }
    /* We won't ever unmap a top level page table */
    أكد(پیدا_کړه_ret.vspace_ریښه != موخه_pt);
    pte_t *pt_خونه = عدم;
    pte_t *pt = پیدا_کړه_ret.vspace_ریښه;

    لكل (کلمه_t i = 0; i < CONFIG_PT_پوړونه - 1 && pt != موخه_pt; i++) {
        pt_خونه = pt + RISCV_GET_PT_شاخص(vptr, i);
        إذا (unlikely(!is_PTEPage_جدول(pt_خونه))) {
            /* couldn't find it */
            أرجع;
        }
        pt = getPPtrFromHWPTE(pt_خونه);
    }

    إذا (pt != موخه_pt) {
        /* didn't find it */
        أرجع;
    }
    /* If we found a pt then ptSlot won't be null */
    أكد(pt_خونه != عدم);
    *pt_خونه = pte_new(
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

فراغ unmap_پاڼه(vm_پاڼه_اندازه_t پاڼه_اندازه, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;
    lookupPTSlot_ret_t  lu_ret;

    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPTSlot(پیدا_کړه_ret.vspace_ریښه, vptr);
    إذا (unlikely(lu_ret.pt_بټونه_کیڼ != پاڼه_بټونه_For_اندازه(پاڼه_اندازه))) {
        أرجع;
    }
    إذا (!pte_ptr_get_valid(lu_ret.pt_خونه) || is_PTEPage_جدول(lu_ret.pt_خونه)
        || (pte_ptr_get_ppn(lu_ret.pt_خونه) << se_L4_پاڼه_بټونه) != pptr_منزل_paddr((فراغ *)pptr)) {
        أرجع;
    }

    lu_ret.pt_خونه[0] = pte_pte_invalid_new();
    sfence();
}

فراغ سېټ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ریښه;
    asid_t asid;
    pte_t *lvl1pt;
    پیدا_کړه_VSpace_For_ASID_ret_t  پیدا_کړه_ret;

    thread_ریښه = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_ریښه) != cap_پاڼه_جدول_cap) {
        سېټ_VSpace_ریښه(kpptr_منزل_paddr(&kernel_ریښه_پاڼه_جدول), 0);
        أرجع;
    }

    lvl1pt = PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(thread_ریښه));

    asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(thread_ریښه);
    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE || پیدا_کړه_ret.vspace_ریښه != lvl1pt)) {
        سېټ_VSpace_ریښه(kpptr_منزل_paddr(&kernel_ریښه_پاڼه_جدول), 0);
        أرجع;
    }

    سېټ_VSpace_ریښه(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_ریښه(cap_t cap)
{
    أرجع (cap_get_capType(cap) == cap_پاڼه_جدول_cap &&
            cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (unlikely(cap_get_capType(cap) != cap_چوکاټ_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_چوکاټ_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(!IS_ALIGNED(vptr, se_L4_IPCBuffer_اندازه_بټونه))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

vm_rights_t CONST ماسک_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ماسک)
{
    إذا (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_ماسک)) {
        أرجع VMReadOnly;
    }
    إذا (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_ماسک)) {
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

/* The rest of the file implements the RISCV object invocations */

ساكن pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    کلمه_t اكتب = RISCVGetWriteFromVMRights(vm_rights);
    کلمه_t اقرأ = RISCVGetReadFromVMRights(vm_rights);
    إذا (unlikely(!اقرأ && !اكتب && !executable)) {
        أرجع pte_pte_invalid_new();
    } وإلا {
        أرجع pte_new(
                   paddr >> se_L4_پاڼه_بټونه,
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

ساكن مضمن bool_t CONST checkVPAlignment(vm_پاڼه_اندازه_t sz, کلمه_t w)
{
    أرجع (w & ماسک(پاڼه_بټونه_For_اندازه(sz))) == 0;
}

ساكن exception_t decode_RISCVPage_جدول_Invocation(کلمه_t label, کلمه_t اوږدتیا,
                                                  cte_t *cte, cap_t cap, کلمه_t *بفر)
{
    إذا (label == RISCVPage_جدول_Unmap) {
        إذا (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = se_L4_Revoke_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        إذا (likely(cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap))) {
            asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap);
            پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap));
            إذا (unlikely(پیدا_کړه_ret.حالت == EXCEPTION_NONE &&
                         پیدا_کړه_ret.vspace_ریښه == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = se_L4_Revoke_لومړی;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely((label != RISCVPage_جدول_Map))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کلمه_t vaddr = getSyscallArg(0, بفر);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(cap_get_capType(lvl1ptCap) != cap_پاڼه_جدول_cap ||
                 cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(lvl1ptCap));
    asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(lvl1ptCap);

    إذا (unlikely(vaddr >= USER_سر)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(پیدا_کړه_ret.vspace_ریښه != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    إذا (lu_ret.pt_بټونه_کیڼ == se_L4_پاڼه_بټونه || pte_ptr_get_valid(lu_ret.pt_خونه)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = se_L4_Delete_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Get the slot to install the PT in */
    pte_t *pt_خونه = lu_ret.pt_خونه;

    paddr_t paddr = addrFromPPtr(
                        PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap)));
    pte_t pte = pte_new((paddr >> se_L4_پاڼه_بټونه),
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

    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTIs_Mapped(cap, 1);
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTMapped_ASID(cap, asid);
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTMapped_پته(cap, (vaddr & ~ماسک(lu_ret.pt_بټونه_کیڼ)));

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_پاڼه_جدول_Invocation_Map(cap, cte, pte, pt_خونه);
}

ساكن exception_t decodeRISCVFrameInvocation(کلمه_t label, کلمه_t اوږدتیا,
                                              cte_t *cte, cap_t cap, bool_t call, کلمه_t *بفر)
{
    بدل (label) {
    حالة RISCVPageMap: {
        إذا (unlikely(اوږدتیا < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        کلمه_t vaddr = getSyscallArg(0, بفر);
        کلمه_t w_rights_ماسک = getSyscallArg(1, بفر);
        vm_ځانګړتیاوې_t attr = vm_ځانګړتیاوې_From_کلمه(getSyscallArg(2, بفر));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_پاڼه_اندازه_t چوکاټ_اندازه = cap_چوکاټ_cap_get_cap_FSize(cap);
        vm_rights_t capVMRights = cap_چوکاټ_cap_get_cap_FVMRights(cap);

        إذا (unlikely(cap_get_capType(lvl1ptCap) != cap_پاڼه_جدول_cap ||
                     !cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(lvl1ptCap));
        asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(lvl1ptCap);

        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(پیدا_کړه_ret.vspace_ریښه != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        کلمه_t vtop = vaddr + بټ(پاڼه_بټونه_For_اندازه(چوکاټ_اندازه)) - 1;
        إذا (unlikely(vtop >= USER_سر)) {
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (unlikely(!checkVPAlignment(چوکاټ_اندازه, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        إذا (unlikely(lu_ret.pt_بټونه_کیڼ != پاڼه_بټونه_For_اندازه(چوکاټ_اندازه))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_بټونه_کیڼ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_t چوکاټ_asid = cap_چوکاټ_cap_get_cap_FMapped_ASID(cap);
        إذا (unlikely(چوکاټ_asid != asidInvalid)) {
            /* this frame is already mapped */
            إذا (چوکاټ_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_شمېره = 1;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
            کلمه_t mapped_vaddr = cap_چوکاټ_cap_get_cap_FMapped_پته(cap);
            إذا (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_شمېره = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            إذا (unlikely(is_PTEPage_جدول(lu_ret.pt_خونه))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = se_L4_Delete_لومړی;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            /* check this vaddr isn't already mapped */
            إذا (unlikely(pte_ptr_get_valid(lu_ret.pt_خونه))) {
                userError("Virtual address (0x%"SEL4_PRIx_کلمه") already mapped", vaddr);
                current_syscall_error.type = se_L4_Delete_لومړی;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = ماسک_VMRights(capVMRights, rights_From_کلمه(w_rights_ماسک));
        paddr_t چوکاټ_paddr = addrFromPPtr((فراغ *) cap_چوکاټ_cap_get_cap_FBase_Ptr(cap));
        cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکاټ_cap_سېټ_cap_FMapped_پته(cap,  vaddr);

        bool_t executable = !vm_ځانګړتیاوې_get_riscv_Execute_Never(attr);
        pte_t pte = makeUserPTE(چوکاټ_paddr, executable, vmRights);
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_Invocation_Map_PTE(cap, cte, pte, lu_ret.pt_خونه);
    }

    حالة RISCVPageUnmap: {
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_Invocation_Unmap(cap, cte);
    }

    حالة RISCVPage_Get_پته: {

        /* Check that there are enough message registers */
        أكد(n_پیغام_Registers >= 1);

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_Get_پته((فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(کلمه_t label, کلمه_t اوږدتیا, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, کلمه_t *بفر)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_پاڼه_جدول_cap:
        أرجع decode_RISCVPage_جدول_Invocation(label, اوږدتیا, cte, cap, بفر);

    حالة cap_چوکاټ_cap:
        أرجع decodeRISCVFrameInvocation(label, اوږدتیا, cte, cap, call, بفر);

    حالة cap_asid_control_cap: {
        کلمه_t     i;
        asid_t           asid_بنسټ;
        کلمه_t           شاخص;
        کلمه_t           ژوروالی;
        cap_t            untyped;
        cap_t            ریښه;
        cte_t           *پلار_خونه;
        cte_t           *dest_خونه;
        lookup_خونه_ret_t lu_ret;
        فراغ            *چوکاټ;
        exception_t      حالت;

        إذا (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شاخص = getSyscallArg(0, بفر);
        ژوروالی = getSyscallArg(1, بفر);
        پلار_خونه = current_extra_caps.excaprefs[0];
        untyped = پلار_خونه->cap;
        ریښه = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        إذا (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_لومړی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنسټ = i << asid_ټیټ_بټونه;

        إذا (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_بلاک_اندازه(untyped) != se_L4_ASIDPool_بټونه ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(پلار_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        چوکاټ = کلمه_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_موخه_خونه(ریښه, شاخص, ژوروالی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        dest_خونه = lu_ret.خونه;

        حالت = ensure_تشه_خونه(dest_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکاټ, dest_خونه, پلار_خونه, asid_بنسټ);
    }

    حالة cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_خونه;
        asid_pool_t *pool;
        کلمه_t i;
        asid_t       asid;

        إذا (label != RISCVASIDPool_ټاکل) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_خونه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_خونه->cap;

        إذا (unlikely(
                cap_get_capType(vspaceCap) != cap_پاڼه_جدول_cap ||
                cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_لوړ(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            current_lookup_fault = lookup_fault_invalid_ریښه_new();
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < بټ(asid_ټیټ_بټونه) && (asid + i == 0 || pool->array[i]); i++);

        إذا (i == بټ(asid_ټیټ_بټونه)) {
            current_syscall_error.type = se_L4_Delete_لومړی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_خونه);
    }
    افتراضي:
        fail("Invalid arch cap type");
    }
}

exception_t perform_پاڼه_جدول_Invocation_Map(cap_t cap, cte_t *ct_خونه,
                                          pte_t pte, pte_t *pt_خونه)
{
    ct_خونه->cap = cap;
    *pt_خونه = pte;
    sfence();

    أرجع EXCEPTION_NONE;
}

exception_t perform_پاڼه_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_خونه)
{
    إذا (cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_پاڼه_جدول(
            cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(cap),
            pt
        );
        clear_حافظه((فراغ *)pt, se_L4_پاڼه_جدول_بټونه);
    }
    cap_پاڼه_جدول_cap_ptr_سېټ_cap_PTIs_Mapped(&(ct_خونه->cap), 0);

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

ساكن exception_t updatePTE(pte_t pte, pte_t *بنسټ)
{
    *بنسټ = pte;
    sfence();
    أرجع EXCEPTION_NONE;
}

exception_t perform_پاڼه_Invocation_Map_PTE(cap_t cap, cte_t *ct_خونه,
                                        pte_t pte, pte_t *بنسټ)
{
    ct_خونه->cap = cap;
    أرجع updatePTE(pte, بنسټ);
}

exception_t perform_پاڼه_Invocation_Unmap(cap_t cap, cte_t *ct_خونه)
{
    إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        unmap_پاڼه(cap_چوکاټ_cap_get_cap_FSize(cap),
                  cap_چوکاټ_cap_get_cap_FMapped_ASID(cap),
                  cap_چوکاټ_cap_get_cap_FMapped_پته(cap),
                  cap_چوکاټ_cap_get_cap_FBase_Ptr(cap)
                 );
    }

    cap_t خونه_Cap = ct_خونه->cap;
    خونه_Cap = cap_چوکاټ_cap_سېټ_cap_FMapped_پته(خونه_Cap, 0);
    خونه_Cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(خونه_Cap, asidInvalid);
    ct_خونه->cap = خونه_Cap;

    أرجع EXCEPTION_NONE;
}

#که_تعریف_شوی CONFIG_PRINTING
فراغ Arch_user_ډېری_Trace(tcb_t *tptr)
{
    cap_t thread_ریښه = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    إذا (!is_Valid_VTable_ریښه(thread_ریښه)) {
        چاپ_کړه("Invalid vspace\n");
        أرجع;
    }

    کلمه_t sp = get_ثبتوونکی(tptr, SP);
    إذا (!IS_ALIGNED(sp, se_L4_کلمه_اندازه_بټونه)) {
        چاپ_کړه("SP %p not aligned", (فراغ *) sp);
        أرجع;
    }

    pte_t *vspace_ریښه = PTE_PTR(pptr_د_cap(thread_ریښه));
    لكل (صحيح i = 0; i < CONFIG_USER_ډېری_TRACE_اوږدتیا; i++) {
        کلمه_t پته = sp + (i * حجم(کلمه_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_ریښه, پته);
        إذا (پته < USER_سر && pte_ptr_get_valid(ret.pt_خونه) && !is_PTEPage_جدول(ret.pt_خونه)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.pt_خونه));
            کلمه_t *ارزښت = (کلمه_t *)((کلمه_t)pptr + (پته & ماسک(ret.pt_بټونه_کیڼ)));
            چاپ_کړه("0x%lx: 0x%lx\n", (اوږد) پته, (اوږد) *ارزښت);
        } وإلا {
            چاپ_کړه("0x%lx: INVALID\n", (اوږد) پته);
        }
    }
}
#د_که_پای

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

    چوکاټ_اندازه = cap_چوکاټ_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکاټ_اندازه != RISCV_Mega_پاڼه) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکاټ_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکاټ_pptr = cap_چوکاټ_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکاټ_pptr);

#إذا __riscv_xlen == 32
    paddr_t physical_پته = ks_User_Log_بفر;
    لكل (کلمه_t i = 0; i < بټ(PT_شاخص_بټونه); i += 1) {
        kernel_image_level1_log_بفر_pt[i] = pte_بل(physical_پته, true);
        physical_پته += بټ(پاڼه_بټونه);
    }
    أكد(physical_پته - ks_User_Log_بفر == بټ(se_L4_Large_پاڼه_بټونه));
#وإلا
    kernel_image_level1_dev_pt[RISCV_GET_PT_شاخص(KS_LOG_PPTR, 1)] = pte_بل(ks_User_Log_بفر, true);
#د_که_پای

    sfence();

    أرجع EXCEPTION_NONE;
}
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */
