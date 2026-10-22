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
    paddr_t چوکھٹا_ادھار;
    vm_ورقہ_ناپ_t چوکھٹا_ناپ;
    bool_t valid;
};
تعريفنوع بنية resolve_ret resolve_ret_t;

ساكن exception_t perform_ورقہ_Get_پتہ(فراغ *vbase_ptr, bool_t call);

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
ساكن pte_t pte_اگلا(لفظ_t phys_addr, bool_t is_پتی)
{
    لفظ_t ppn = (لفظ_t)(phys_addr >> 12);

    uint8_t اقرأ = is_پتی ? 1 : 0;
    uint8_t اكتب = اقرأ;
    uint8_t exec = اقرأ;

    أرجع pte_new(ppn,
                   0,     /* sw */
                   is_پتی ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_پتی ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_پتی ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   اكتب, /* write */
                   اقرأ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_کوڈ فراغ map_kernel_چوکھٹا(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#إذا __riscv_xlen == 32
    paddr = گیڑا_تھلے(paddr, RISCV_GET_LVL_PGSIZE_بٹ(0));
    أكد((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_جڑ_ورقہ_جدول[RISCV_GET_PT_اشاریہ(vaddr, 0)] = pte_اگلا(paddr, true);
#وإلا
    إذا (vaddr >= KDEV_ادھار) {
        /* Map devices in level 1 page table */
        paddr = گیڑا_تھلے(paddr, RISCV_GET_LVL_PGSIZE_بٹ(1));
        أكد((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_اشاریہ(vaddr, 1)] = pte_اگلا(paddr, true);
    } وإلا {
        paddr = گیڑا_تھلے(paddr, RISCV_GET_LVL_PGSIZE_بٹ(0));
        أكد((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_جڑ_ورقہ_جدول[RISCV_GET_PT_اشاریہ(vaddr, 0)] = pte_اگلا(paddr, true);
    }
#جے_انت
}

BOOT_کوڈ VISIBLE فراغ map_kernel_کھڑکی(فراغ)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    أكد(CONFIG_PT_پدھراں > 1 && CONFIG_PT_پدھراں <= 4);
    /* Kernel image finishes before KDEV_BASE */
    أكد(KERNEL_ELF_چوٹی <= KDEV_ادھار);

    /* kernel window starts at PPTR_BASE */
    لفظ_t pptr = PPTR_ادھار;

    /* first we map in memory from PADDR_BASE */
    لفظ_t paddr = PADDR_ادھار;
    طالما (pptr < PPTR_چوٹی) {
        أكد(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_بٹ(0)));
        أكد(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_بٹ(0)));

        kernel_جڑ_ورقہ_جدول[RISCV_GET_PT_اشاریہ(pptr, 0)] = pte_اگلا(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    أكد(pptr == PPTR_چوٹی);
    pptr = گیڑا_تھلے(KERNEL_ELF_ادھار, RISCV_GET_LVL_PGSIZE_بٹ(0));
    paddr = گیڑا_تھلے(KERNEL_ELF_PADDR_ادھار, RISCV_GET_LVL_PGSIZE_بٹ(0));

#إذا __riscv_xlen == 32
    kernel_جڑ_ورقہ_جدول[RISCV_GET_PT_اشاریہ(pptr, 0)] = pte_اگلا(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    kernel_جڑ_ورقہ_جدول[RISCV_GET_PT_اشاریہ(KS_LOG_PPTR, 0)] =
        pte_اگلا(kpptr_منزل_paddr(kernel_image_level1_log_بفر_pt), false);
#جے_انت
#وإلا
    لفظ_t اشاریہ = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_جڑ_ورقہ_جدول[RISCV_GET_PT_اشاریہ(KERNEL_ELF_PADDR_ادھار + PPTR_ادھار_وتھ, 0)] =
        pte_اگلا(kpptr_منزل_paddr(kernel_image_level1_pt), false);
    kernel_جڑ_ورقہ_جدول[RISCV_GET_PT_اشاریہ(pptr, 0)] =
        pte_اگلا(kpptr_منزل_paddr(kernel_image_level1_pt), false);
    طالما (pptr < PPTR_چوٹی + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[اشاریہ] = pte_اگلا(paddr, true);
        اشاریہ++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_جڑ_ورقہ_جدول[RISCV_GET_PT_اشاریہ(KDEV_ادھار, 0)] =
        pte_اگلا(kpptr_منزل_paddr(kernel_image_level1_dev_pt), false);
#جے_انت

    /* There should be free space where we put device mapping */
    أكد(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_کوڈ فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    lookupPTSlot_ret_t pt_ret;
    pte_t *نشانہ_تھاں;
    vptr_t vptr = cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(pt_cap);
    pte_t *lvl1pt = PTE_PTR(pptr_دا_cap(vspace_cap));

    /* pt to be mapped */
    pte_t *pt   = PTE_PTR(pptr_دا_cap(pt_cap));

    /* Get PT slot to install the address in */
    pt_ret = lookupPTSlot(lvl1pt, vptr);

    نشانہ_تھاں = pt_ret.pt_تھاں;

    *نشانہ_تھاں = pte_new(
                      (addrFromPPtr(pt) >> se_L4_ورقہ_بٹ),
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

BOOT_کوڈ فراغ map_it_چوکھٹا_cap(cap_t vspace_cap, cap_t چوکھٹا_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_دا_cap(vspace_cap));
    pte_t *چوکھٹا_pptr   = PTE_PTR(pptr_دا_cap(چوکھٹا_cap));
    vptr_t چوکھٹا_vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(چوکھٹا_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, چوکھٹا_vptr);
    أكد(lu_ret.pt_بٹ_بایاں == se_L4_ورقہ_بٹ);

    pte_t *نشانہ_تھاں = lu_ret.pt_تھاں;

    *نشانہ_تھاں = pte_new(
                      (pptr_منزل_paddr(چوکھٹا_pptr) >> se_L4_ورقہ_بٹ),
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

BOOT_کوڈ cap_t create_unmapped_it_چوکھٹا_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_چوکھٹا_cap_new(
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
ساكن BOOT_کوڈ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ورقہ_جدول_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}

BOOT_کوڈ لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    لفظ_t n = 0;
    لكل (صحيح i = 0; i < CONFIG_PT_پدھراں - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_بٹ(i));
    }
    أرجع n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_کوڈ cap_t create_it_پتہ_space(cap_t جڑ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      lvl1pt_cap;
    vptr_t     pt_vptr;

    copyGlobalMappings(PTE_PTR(rootserver.vspace));

    lvl1pt_cap =
        cap_ورقہ_جدول_cap_new(
            IT_ASID,               /* capPTMappedASID    */
            (لفظ_t) rootserver.vspace,  /* capPTBasePtr       */
            1,                     /* capPTIsMapped      */
            (لفظ_t) rootserver.vspace   /* capPTMappedAddress */
        );

    se_L4_تھاں_Pos تھاں_pos_before = ndks_boot.تھاں_pos_cur;
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    لكل (صحيح i = 0; i < CONFIG_PT_پدھراں - 1; i++) {

        لكل (pt_vptr = گیڑا_تھلے(it_v_reg.شروع, RISCV_GET_LVL_PGSIZE_بٹ(i));
             pt_vptr < it_v_reg.سرا;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            إذا (!provide_cap(جڑ_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                أرجع cap_null_cap_new();
            }
        }

    }

    se_L4_تھاں_Pos تھاں_pos_after = ndks_boot.تھاں_pos_cur;
    ndks_boot.bi_چوکھٹا->userImagePaging = (se_L4_تھاں_Region) {
        تھاں_pos_before, تھاں_pos_after
    };

    أرجع lvl1pt_cap;
}

BOOT_کوڈ فراغ activate_kernel_vspace(فراغ)
{
    سیٹ_VSpace_جڑ(kpptr_منزل_paddr(&kernel_جڑ_ورقہ_جدول), 0);
}

BOOT_کوڈ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_دا_cap(it_ap_cap));
    ap->array[ASID_کم(IT_ASID)] = PTE_PTR(pptr_دا_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_بلند(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ساكن لبھو_VSpace_For_ASID_ret_t لبھو_VSpace_For_ASID(asid_t asid)
{
    لبھو_VSpace_For_ASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_جڑ;

    poolPtr = riscvKSASIDTable[ASID_بلند(asid)];
    إذا (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.vspace_جڑ = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    vspace_جڑ = poolPtr->array[ASID_کم(asid)];
    إذا (!vspace_جڑ) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.vspace_جڑ = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_جڑ = vspace_جڑ;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

فراغ copyGlobalMappings(pte_t *newLvl1pt)
{
    بنانشانی لما i;
    pte_t *global_kernel_vspace = kernel_جڑ_ورقہ_جدول;

    لكل (i = RISCV_GET_PT_اشاریہ(PPTR_ادھار, 0); i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
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

    إذا (unlikely(cap_get_capType(بفر_Cap) != cap_چوکھٹا_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(cap_چوکھٹا_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_چوکھٹا_cap_get_cap_FVMRights(بفر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        لفظ_t ادھار_Ptr, ورقہ_بٹ_2;

        ادھار_Ptr = cap_چوکھٹا_cap_get_cap_FBase_Ptr(بفر_Cap);
        ورقہ_بٹ_2 = ورقہ_بٹ_For_ناپ(cap_چوکھٹا_cap_get_cap_FSize(بفر_Cap));
        أرجع (لفظ_t *)(ادھار_Ptr + (w_بفر_Ptr & نقاب(ورقہ_بٹ_2)));
    } وإلا {
        أرجع عدم;
    }
}

ساكن مضمن pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    أرجع PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << se_L4_ورقہ_جدول_بٹ));
}

lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    لفظ_t level = CONFIG_PT_پدھراں - 1;
    pte_t *pt = lvl1pt;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_بٹ_بایاں = PT_اشاریہ_بٹ * level + se_L4_ورقہ_بٹ;
    ret.pt_تھاں = pt + ((vptr >> ret.pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));

    طالما (is_PTEPage_جدول(ret.pt_تھاں) && likely(0 < level)) {
        level--;
        ret.pt_بٹ_بایاں -= PT_اشاریہ_بٹ;
        pt = getPPtrFromHWPTE(ret.pt_تھاں);
        ret.pt_تھاں = pt + ((vptr >> ret.pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
    }

    أرجع ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    بدل (vm_faultType) {
    حالة RISCVLoad_ورقہ_Fault:
    حالة RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        أرجع EXCEPTION_FAULT;
    حالة RISCVStore_ورقہ_Fault:
    حالة RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        أرجع EXCEPTION_FAULT;
    حالة RISCVInstruction_ورقہ_Fault:
    حالة RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        أرجع EXCEPTION_FAULT;

    افتراضي:
        fail("Invalid VM fault type");
    }
}

فراغ deleteASIDPool(asid_t asid_ادھار, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد(IS_ALIGNED(asid_ادھار, asid_کم_بٹ));

    إذا (riscvKSASIDTable[ASID_بلند(asid_ادھار)] == pool) {
        riscvKSASIDTable[ASID_بلند(asid_ادھار)] = عدم;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

ساكن exception_t performASIDControlInvocation(فراغ *چوکھٹا, cte_t *تھاں, cte_t *ماپا, asid_t asid_ادھار)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سیٹ_cap_Free_اشاریہ(&(ماپا->cap),
                                         MAX_FREE_اشاریہ(cap_untyped_cap_get_cap_بلاک_ناپ(ماپا->cap)));

    memzero(چوکھٹا, ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(RISCV_4_K_ورقہ)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_داخل_کرو(
        cap_asid_pool_cap_new(
            asid_ادھار,          /* capASIDBase  */
            لفظ_REF(چوکھٹا)     /* capASIDPool  */
        ),
        ماپا,
        تھاں
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_ادھار & نقاب(asid_کم_بٹ)) == 0);
    riscvKSASIDTable[ASID_بلند(asid_ادھار)] = (asid_pool_t *)چوکھٹا;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_تھاں)
{
    cap_t cap = vspace_Cap_تھاں->cap;
    pte_t *region_ادھار = PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap));
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTMapped_پتہ(cap, 0);
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTIs_Mapped(cap, 1);
    vspace_Cap_تھاں->cap = cap;

    copyGlobalMappings(region_ادھار);

    poolPtr->array[ASID_کم(asid)] = region_ادھار;

    أرجع EXCEPTION_NONE;
}

فراغ deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_بلند(asid)];
    إذا (poolPtr != عدم && poolPtr->array[ASID_کم(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_کم(asid)] = عدم;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

فراغ unmap_ورقہ_جدول(asid_t asid, vptr_t vptr, pte_t *نشانہ_pt)
{
    لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
    إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
        /* nothing to do */
        أرجع;
    }
    /* We won't ever unmap a top level page table */
    أكد(لبھو_ret.vspace_جڑ != نشانہ_pt);
    pte_t *pt_تھاں = عدم;
    pte_t *pt = لبھو_ret.vspace_جڑ;

    لكل (لفظ_t i = 0; i < CONFIG_PT_پدھراں - 1 && pt != نشانہ_pt; i++) {
        pt_تھاں = pt + RISCV_GET_PT_اشاریہ(vptr, i);
        إذا (unlikely(!is_PTEPage_جدول(pt_تھاں))) {
            /* couldn't find it */
            أرجع;
        }
        pt = getPPtrFromHWPTE(pt_تھاں);
    }

    إذا (pt != نشانہ_pt) {
        /* didn't find it */
        أرجع;
    }
    /* If we found a pt then ptSlot won't be null */
    أكد(pt_تھاں != عدم);
    *pt_تھاں = pte_new(
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

فراغ unmap_ورقہ(vm_ورقہ_ناپ_t ورقہ_ناپ, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    لبھو_VSpace_For_ASID_ret_t لبھو_ret;
    lookupPTSlot_ret_t  lu_ret;

    لبھو_ret = لبھو_VSpace_For_ASID(asid);
    إذا (لبھو_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPTSlot(لبھو_ret.vspace_جڑ, vptr);
    إذا (unlikely(lu_ret.pt_بٹ_بایاں != ورقہ_بٹ_For_ناپ(ورقہ_ناپ))) {
        أرجع;
    }
    إذا (!pte_ptr_get_valid(lu_ret.pt_تھاں) || is_PTEPage_جدول(lu_ret.pt_تھاں)
        || (pte_ptr_get_ppn(lu_ret.pt_تھاں) << se_L4_ورقہ_بٹ) != pptr_منزل_paddr((فراغ *)pptr)) {
        أرجع;
    }

    lu_ret.pt_تھاں[0] = pte_pte_invalid_new();
    sfence();
}

فراغ سیٹ_VMRoot(tcb_t *tcb)
{
    cap_t thread_جڑ;
    asid_t asid;
    pte_t *lvl1pt;
    لبھو_VSpace_For_ASID_ret_t  لبھو_ret;

    thread_جڑ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_جڑ) != cap_ورقہ_جدول_cap) {
        سیٹ_VSpace_جڑ(kpptr_منزل_paddr(&kernel_جڑ_ورقہ_جدول), 0);
        أرجع;
    }

    lvl1pt = PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(thread_جڑ));

    asid = cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(thread_جڑ);
    لبھو_ret = لبھو_VSpace_For_ASID(asid);
    إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE || لبھو_ret.vspace_جڑ != lvl1pt)) {
        سیٹ_VSpace_جڑ(kpptr_منزل_paddr(&kernel_جڑ_ورقہ_جدول), 0);
        أرجع;
    }

    سیٹ_VSpace_جڑ(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_جڑ(cap_t cap)
{
    أرجع (cap_get_capType(cap) == cap_ورقہ_جدول_cap &&
            cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (unlikely(cap_get_capType(cap) != cap_چوکھٹا_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_چوکھٹا_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(!IS_ALIGNED(vptr, se_L4_IPCBuffer_ناپ_بٹ))) {
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
                   paddr >> se_L4_ورقہ_بٹ,
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

ساكن مضمن bool_t CONST checkVPAlignment(vm_ورقہ_ناپ_t sz, لفظ_t w)
{
    أرجع (w & نقاب(ورقہ_بٹ_For_ناپ(sz))) == 0;
}

ساكن exception_t decode_RISCVPage_جدول_Invocation(لفظ_t label, لفظ_t لمائی,
                                                  cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    إذا (label == RISCVPage_جدول_Unmap) {
        إذا (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = se_L4_Revoke_پہلا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        إذا (likely(cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap))) {
            asid_t asid = cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap);
            لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap));
            إذا (unlikely(لبھو_ret.حالت == EXCEPTION_NONE &&
                         لبھو_ret.vspace_جڑ == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = se_L4_Revoke_پہلا;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely((label != RISCVPage_جدول_Map))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(لمائی < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t vaddr = getSyscallArg(0, بفر);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(cap_get_capType(lvl1ptCap) != cap_ورقہ_جدول_cap ||
                 cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(lvl1ptCap));
    asid_t asid = cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(lvl1ptCap);

    إذا (unlikely(vaddr >= USER_چوٹی)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
    إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(لبھو_ret.vspace_جڑ != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    إذا (lu_ret.pt_بٹ_بایاں == se_L4_ورقہ_بٹ || pte_ptr_get_valid(lu_ret.pt_تھاں)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = se_L4_Delete_پہلا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Get the slot to install the PT in */
    pte_t *pt_تھاں = lu_ret.pt_تھاں;

    paddr_t paddr = addrFromPPtr(
                        PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap)));
    pte_t pte = pte_new((paddr >> se_L4_ورقہ_بٹ),
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

    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTMapped_پتہ(cap, (vaddr & ~نقاب(lu_ret.pt_بٹ_بایاں)));

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_ورقہ_جدول_Invocation_Map(cap, cte, pte, pt_تھاں);
}

ساكن exception_t decodeRISCVFrameInvocation(لفظ_t label, لفظ_t لمائی,
                                              cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (label) {
    حالة RISCVPageMap: {
        إذا (unlikely(لمائی < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t vaddr = getSyscallArg(0, بفر);
        لفظ_t w_rights_نقاب = getSyscallArg(1, بفر);
        vm_گن_t attr = vm_گن_From_لفظ(getSyscallArg(2, بفر));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_ورقہ_ناپ_t چوکھٹا_ناپ = cap_چوکھٹا_cap_get_cap_FSize(cap);
        vm_rights_t capVMRights = cap_چوکھٹا_cap_get_cap_FVMRights(cap);

        إذا (unlikely(cap_get_capType(lvl1ptCap) != cap_ورقہ_جدول_cap ||
                     !cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(lvl1ptCap));
        asid_t asid = cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(lvl1ptCap);

        لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
        إذا (unlikely(لبھو_ret.حالت != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(لبھو_ret.vspace_جڑ != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        لفظ_t vtop = vaddr + ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(چوکھٹا_ناپ)) - 1;
        إذا (unlikely(vtop >= USER_چوٹی)) {
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (unlikely(!checkVPAlignment(چوکھٹا_ناپ, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        إذا (unlikely(lu_ret.pt_بٹ_بایاں != ورقہ_بٹ_For_ناپ(چوکھٹا_ناپ))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_بٹ_بایاں);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_t چوکھٹا_asid = cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap);
        إذا (unlikely(چوکھٹا_asid != asidInvalid)) {
            /* this frame is already mapped */
            إذا (چوکھٹا_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_نمبر = 1;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
            لفظ_t mapped_vaddr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
            إذا (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            إذا (unlikely(is_PTEPage_جدول(lu_ret.pt_تھاں))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = se_L4_Delete_پہلا;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            /* check this vaddr isn't already mapped */
            إذا (unlikely(pte_ptr_get_valid(lu_ret.pt_تھاں))) {
                userError("Virtual address (0x%"SEL4_PRIx_لفظ") already mapped", vaddr);
                current_syscall_error.type = se_L4_Delete_پہلا;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = نقاب_VMRights(capVMRights, rights_From_لفظ(w_rights_نقاب));
        paddr_t چوکھٹا_paddr = addrFromPPtr((فراغ *) cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap,  vaddr);

        bool_t executable = !vm_گن_get_riscv_Execute_Never(attr);
        pte_t pte = makeUserPTE(چوکھٹا_paddr, executable, vmRights);
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_Invocation_Map_PTE(cap, cte, pte, lu_ret.pt_تھاں);
    }

    حالة RISCVPageUnmap: {
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_Invocation_Unmap(cap, cte);
    }

    حالة RISCVPage_Get_پتہ: {

        /* Check that there are enough message registers */
        أكد(n_پیغام_Registers >= 1);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_Get_پتہ((فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(لفظ_t label, لفظ_t لمائی, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_ورقہ_جدول_cap:
        أرجع decode_RISCVPage_جدول_Invocation(label, لمائی, cte, cap, بفر);

    حالة cap_چوکھٹا_cap:
        أرجع decodeRISCVFrameInvocation(label, لمائی, cte, cap, call, بفر);

    حالة cap_asid_control_cap: {
        لفظ_t     i;
        asid_t           asid_ادھار;
        لفظ_t           اشاریہ;
        لفظ_t           ڈونگھائی;
        cap_t            untyped;
        cap_t            جڑ;
        cte_t           *ماپا_تھاں;
        cte_t           *dest_تھاں;
        lookup_تھاں_ret_t lu_ret;
        فراغ            *چوکھٹا;
        exception_t      حالت;

        إذا (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (لمائی < 2 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        اشاریہ = getSyscallArg(0, بفر);
        ڈونگھائی = getSyscallArg(1, بفر);
        ماپا_تھاں = current_extra_caps.excaprefs[0];
        untyped = ماپا_تھاں->cap;
        جڑ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        إذا (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پہلا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_ادھار = i << asid_کم_بٹ;

        إذا (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_بلاک_ناپ(untyped) != se_L4_ASIDPool_بٹ ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(ماپا_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        چوکھٹا = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_نشانہ_تھاں(جڑ, اشاریہ, ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        dest_تھاں = lu_ret.تھاں;

        حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکھٹا, dest_تھاں, ماپا_تھاں, asid_ادھار);
    }

    حالة cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_تھاں;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t       asid;

        إذا (label != RISCVASIDPool_سونپو) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_تھاں = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_تھاں->cap;

        إذا (unlikely(
                cap_get_capType(vspaceCap) != cap_ورقہ_جدول_cap ||
                cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_بلند(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            current_lookup_fault = lookup_fault_invalid_جڑ_new();
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < ثنائی_ہندسہ(asid_کم_بٹ) && (asid + i == 0 || pool->array[i]); i++);

        إذا (i == ثنائی_ہندسہ(asid_کم_بٹ)) {
            current_syscall_error.type = se_L4_Delete_پہلا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_تھاں);
    }
    افتراضي:
        fail("Invalid arch cap type");
    }
}

exception_t perform_ورقہ_جدول_Invocation_Map(cap_t cap, cte_t *ct_تھاں,
                                          pte_t pte, pte_t *pt_تھاں)
{
    ct_تھاں->cap = cap;
    *pt_تھاں = pte;
    sfence();

    أرجع EXCEPTION_NONE;
}

exception_t perform_ورقہ_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_تھاں)
{
    إذا (cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_ورقہ_جدول(
            cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(cap),
            pt
        );
        clear_حافظہ((فراغ *)pt, se_L4_ورقہ_جدول_بٹ);
    }
    cap_ورقہ_جدول_cap_ptr_سیٹ_cap_PTIs_Mapped(&(ct_تھاں->cap), 0);

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

ساكن exception_t updatePTE(pte_t pte, pte_t *ادھار)
{
    *ادھار = pte;
    sfence();
    أرجع EXCEPTION_NONE;
}

exception_t perform_ورقہ_Invocation_Map_PTE(cap_t cap, cte_t *ct_تھاں,
                                        pte_t pte, pte_t *ادھار)
{
    ct_تھاں->cap = cap;
    أرجع updatePTE(pte, ادھار);
}

exception_t perform_ورقہ_Invocation_Unmap(cap_t cap, cte_t *ct_تھاں)
{
    إذا (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        unmap_ورقہ(cap_چوکھٹا_cap_get_cap_FSize(cap),
                  cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                  cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                  cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
                 );
    }

    cap_t تھاں_Cap = ct_تھاں->cap;
    تھاں_Cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(تھاں_Cap, 0);
    تھاں_Cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(تھاں_Cap, asidInvalid);
    ct_تھاں->cap = تھاں_Cap;

    أرجع EXCEPTION_NONE;
}

#جے_تعریف_شدہ CONFIG_PRINTING
فراغ Arch_user_ڈھیر_Trace(tcb_t *tptr)
{
    cap_t thread_جڑ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    إذا (!is_Valid_VTable_جڑ(thread_جڑ)) {
        لکھو("Invalid vspace\n");
        أرجع;
    }

    لفظ_t sp = get_رجسٹر(tptr, SP);
    إذا (!IS_ALIGNED(sp, se_L4_لفظ_ناپ_بٹ)) {
        لکھو("SP %p not aligned", (فراغ *) sp);
        أرجع;
    }

    pte_t *vspace_جڑ = PTE_PTR(pptr_دا_cap(thread_جڑ));
    لكل (صحيح i = 0; i < CONFIG_USER_ڈھیر_TRACE_لمائی; i++) {
        لفظ_t پتہ = sp + (i * حجم(لفظ_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_جڑ, پتہ);
        إذا (پتہ < USER_چوٹی && pte_ptr_get_valid(ret.pt_تھاں) && !is_PTEPage_جدول(ret.pt_تھاں)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.pt_تھاں));
            لفظ_t *قدر = (لفظ_t *)((لفظ_t)pptr + (پتہ & نقاب(ret.pt_بٹ_بایاں)));
            لکھو("0x%lx: 0x%lx\n", (لما) پتہ, (لما) *قدر);
        } وإلا {
            لکھو("0x%lx: INVALID\n", (لما) پتہ);
        }
    }
}
#جے_انت

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

    چوکھٹا_ناپ = cap_چوکھٹا_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکھٹا_ناپ != RISCV_Mega_ورقہ) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_pptr = cap_چوکھٹا_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکھٹا_pptr);

#إذا __riscv_xlen == 32
    paddr_t physical_پتہ = ks_User_Log_بفر;
    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i += 1) {
        kernel_image_level1_log_بفر_pt[i] = pte_اگلا(physical_پتہ, true);
        physical_پتہ += ثنائی_ہندسہ(ورقہ_بٹ);
    }
    أكد(physical_پتہ - ks_User_Log_بفر == ثنائی_ہندسہ(se_L4_Large_ورقہ_بٹ));
#وإلا
    kernel_image_level1_dev_pt[RISCV_GET_PT_اشاریہ(KS_LOG_PPTR, 1)] = pte_اگلا(ks_User_Log_بفر, true);
#جے_انت

    sfence();

    أرجع EXCEPTION_NONE;
}
#جے_انت /* CONFIG_KERNEL_LOG_BUFFER */
