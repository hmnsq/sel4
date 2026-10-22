/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <benchmark/benchmark.h>
#شامل <api/failures.h>
#شامل <api/syscall.h>
#شامل <kernel/boot.h>
#شامل <kernel/cspace.h>
#شامل <kernel/thread.h>
#شامل <object/tcb.h>
#شامل <machine/io.h>
#شامل <model/preemption.h>
#شامل <model/statedata.h>
#شامل <object/cnode.h>
#شامل <object/untyped.h>
#شامل <arch/api/invocation.h>
#شامل <arch/kernel/vspace.h>
#شامل <linker.h>
#شامل <arch/machine.h>
#شامل <plat/machine/hardware.h>
#شامل <kernel/stack.h>
#شامل <util.h>

ساخت resolve_ret {
    paddr_t چوکھٹا_بنیاد;
    vm_صفحہ_سائز_t چوکھٹا_سائز;
    bool_t valid;
};
قسمتعریف ساخت resolve_ret resolve_ret_t;

ساکن exception_t perform_صفحہ_Get_پتہ(خالی *vbase_ptr, bool_t call);

ساکن لفظ_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    واپس vm_rights == VMReadWrite;
}

ساکن اندرونی لفظ_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported.
     * Kernel-only conveys no user rights. */
    واپس vm_rights != VMKernelOnly;
}

ساکن اندرونی bool_t is_PTEPage_جدول(pte_t *pte)
{
    واپس pte_ptr_get_valid(pte) &&
           !(pte_ptr_get_read(pte) || pte_ptr_get_write(pte) || pte_ptr_get_execute(pte));
}

/** Helper function meant only to be used for mapping the kernel
 * window.
 *
 * Maps all pages with full RWX and supervisor perms by default.
 */
ساکن pte_t pte_اگلا(لفظ_t phys_addr, bool_t is_برگ)
{
    لفظ_t ppn = (لفظ_t)(phys_addr >> 12);

    uint8_t اقرأ = is_برگ ? 1 : 0;
    uint8_t اكتب = اقرأ;
    uint8_t exec = اقرأ;

    واپس pte_new(ppn,
                   0,     /* sw */
                   is_برگ ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_برگ ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_برگ ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   اكتب, /* write */
                   اقرأ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_رمز خالی map_kernel_چوکھٹا(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#اگر __riscv_xlen == 32
    paddr = باری_نیچے_کو(paddr, RISCV_GET_LVL_PGSIZE_بٹ(0));
    أكد((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_جڑ_صفحہ_جدول[RISCV_GET_PT_اشاریہ(vaddr, 0)] = pte_اگلا(paddr, true);
#ورنہ
    اگر (vaddr >= KDEV_بنیاد) {
        /* Map devices in level 1 page table */
        paddr = باری_نیچے_کو(paddr, RISCV_GET_LVL_PGSIZE_بٹ(1));
        أكد((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_اشاریہ(vaddr, 1)] = pte_اگلا(paddr, true);
    } ورنہ {
        paddr = باری_نیچے_کو(paddr, RISCV_GET_LVL_PGSIZE_بٹ(0));
        أكد((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_جڑ_صفحہ_جدول[RISCV_GET_PT_اشاریہ(vaddr, 0)] = pte_اگلا(paddr, true);
    }
#اگر_ختم
}

BOOT_رمز VISIBLE خالی map_kernel_کھڑکی(خالی)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    أكد(CONFIG_PT_سطحیں > 1 && CONFIG_PT_سطحیں <= 4);
    /* Kernel image finishes before KDEV_BASE */
    أكد(KERNEL_ELF_چوٹی <= KDEV_بنیاد);

    /* kernel window starts at PPTR_BASE */
    لفظ_t pptr = PPTR_بنیاد;

    /* first we map in memory from PADDR_BASE */
    لفظ_t paddr = PADDR_بنیاد;
    جب (pptr < PPTR_چوٹی) {
        أكد(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_بٹ(0)));
        أكد(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_بٹ(0)));

        kernel_جڑ_صفحہ_جدول[RISCV_GET_PT_اشاریہ(pptr, 0)] = pte_اگلا(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    أكد(pptr == PPTR_چوٹی);
    pptr = باری_نیچے_کو(KERNEL_ELF_بنیاد, RISCV_GET_LVL_PGSIZE_بٹ(0));
    paddr = باری_نیچے_کو(KERNEL_ELF_PADDR_بنیاد, RISCV_GET_LVL_PGSIZE_بٹ(0));

#اگر __riscv_xlen == 32
    kernel_جڑ_صفحہ_جدول[RISCV_GET_PT_اشاریہ(pptr, 0)] = pte_اگلا(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    kernel_جڑ_صفحہ_جدول[RISCV_GET_PT_اشاریہ(KS_LOG_PPTR, 0)] =
        pte_اگلا(kpptr_مقصد_paddr(kernel_image_level1_log_بفر_pt), false);
#اگر_ختم
#ورنہ
    لفظ_t اشاریہ = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_جڑ_صفحہ_جدول[RISCV_GET_PT_اشاریہ(KERNEL_ELF_PADDR_بنیاد + PPTR_بنیاد_انحراف, 0)] =
        pte_اگلا(kpptr_مقصد_paddr(kernel_image_level1_pt), false);
    kernel_جڑ_صفحہ_جدول[RISCV_GET_PT_اشاریہ(pptr, 0)] =
        pte_اگلا(kpptr_مقصد_paddr(kernel_image_level1_pt), false);
    جب (pptr < PPTR_چوٹی + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[اشاریہ] = pte_اگلا(paddr, true);
        اشاریہ++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_جڑ_صفحہ_جدول[RISCV_GET_PT_اشاریہ(KDEV_بنیاد, 0)] =
        pte_اگلا(kpptr_مقصد_paddr(kernel_image_level1_dev_pt), false);
#اگر_ختم

    /* There should be free space where we put device mapping */
    أكد(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_رمز خالی map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    lookupPTSlot_ret_t pt_ret;
    pte_t *ہدف_خانہ;
    vptr_t vptr = cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(pt_cap);
    pte_t *lvl1pt = PTE_PTR(pptr_کا_cap(vspace_cap));

    /* pt to be mapped */
    pte_t *pt   = PTE_PTR(pptr_کا_cap(pt_cap));

    /* Get PT slot to install the address in */
    pt_ret = lookupPTSlot(lvl1pt, vptr);

    ہدف_خانہ = pt_ret.pt_خانہ;

    *ہدف_خانہ = pte_new(
                      (addrFromPPtr(pt) >> se_L4_صفحہ_بٹ),
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

BOOT_رمز خالی map_it_چوکھٹا_cap(cap_t vspace_cap, cap_t چوکھٹا_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_کا_cap(vspace_cap));
    pte_t *چوکھٹا_pptr   = PTE_PTR(pptr_کا_cap(چوکھٹا_cap));
    vptr_t چوکھٹا_vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(چوکھٹا_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, چوکھٹا_vptr);
    أكد(lu_ret.pt_بٹ_بایاں == se_L4_صفحہ_بٹ);

    pte_t *ہدف_خانہ = lu_ret.pt_خانہ;

    *ہدف_خانہ = pte_new(
                      (pptr_مقصد_paddr(چوکھٹا_pptr) >> se_L4_صفحہ_بٹ),
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

BOOT_رمز cap_t create_unmapped_it_چوکھٹا_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_چوکھٹا_cap_new(
                    asidInvalid,                     /* capFMappedASID       */
                    pptr,                            /* capFBasePtr          */
                    0,                               /* capFSize             */
                    0,                               /* capFVMRights         */
                    0,
                    0                                /* capFMappedAddress    */
                );

    واپس cap;
}

/* Create a page table for the initial thread */
ساکن BOOT_رمز cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحہ_جدول_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    واپس cap;
}

BOOT_رمز لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    لفظ_t n = 0;
    برائے (صحیح i = 0; i < CONFIG_PT_سطحیں - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_بٹ(i));
    }
    واپس n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_رمز cap_t create_it_پتہ_space(cap_t جڑ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      lvl1pt_cap;
    vptr_t     pt_vptr;

    copyGlobalMappings(PTE_PTR(rootserver.vspace));

    lvl1pt_cap =
        cap_صفحہ_جدول_cap_new(
            IT_ASID,               /* capPTMappedASID    */
            (لفظ_t) rootserver.vspace,  /* capPTBasePtr       */
            1,                     /* capPTIsMapped      */
            (لفظ_t) rootserver.vspace   /* capPTMappedAddress */
        );

    se_L4_خانہ_Pos خانہ_pos_before = ndks_boot.خانہ_pos_cur;
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    برائے (صحیح i = 0; i < CONFIG_PT_سطحیں - 1; i++) {

        برائے (pt_vptr = باری_نیچے_کو(it_v_reg.آغاز, RISCV_GET_LVL_PGSIZE_بٹ(i));
             pt_vptr < it_v_reg.انتہا;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            اگر (!provide_cap(جڑ_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                واپس cap_null_cap_new();
            }
        }

    }

    se_L4_خانہ_Pos خانہ_pos_after = ndks_boot.خانہ_pos_cur;
    ndks_boot.bi_چوکھٹا->userImagePaging = (se_L4_خانہ_Region) {
        خانہ_pos_before, خانہ_pos_after
    };

    واپس lvl1pt_cap;
}

BOOT_رمز خالی activate_kernel_vspace(خالی)
{
    سیٹ_VSpace_جڑ(kpptr_مقصد_paddr(&kernel_جڑ_صفحہ_جدول), 0);
}

BOOT_رمز خالی write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_کا_cap(it_ap_cap));
    ap->array[ASID_کم(IT_ASID)] = PTE_PTR(pptr_کا_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_بلند(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ساکن ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_VSpace_For_ASID(asid_t asid)
{
    ڈھونڈو_VSpace_For_ASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_جڑ;

    poolPtr = riscvKSASIDTable[ASID_بلند(asid)];
    اگر (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.vspace_جڑ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    vspace_جڑ = poolPtr->array[ASID_کم(asid)];
    اگر (!vspace_جڑ) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.vspace_جڑ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    ret.vspace_جڑ = vspace_جڑ;
    ret.حالت_2 = EXCEPTION_NONE;
    واپس ret;
}

خالی copyGlobalMappings(pte_t *newLvl1pt)
{
    غیرعلامتی طویل i;
    pte_t *global_kernel_vspace = kernel_جڑ_صفحہ_جدول;

    برائے (i = RISCV_GET_PT_اشاریہ(PPTR_بنیاد, 0); i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
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

    اگر (unlikely(cap_get_capType(بفر_Cap) != cap_چوکھٹا_cap)) {
        واپس تہی;
    }
    اگر (unlikely(cap_چوکھٹا_cap_get_cap_FIs_Device(بفر_Cap))) {
        واپس تہی;
    }

    vm_rights = cap_چوکھٹا_cap_get_cap_FVMRights(بفر_Cap);
    اگر (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        لفظ_t بنیاد_Ptr, صفحہ_بٹ_2;

        بنیاد_Ptr = cap_چوکھٹا_cap_get_cap_FBase_Ptr(بفر_Cap);
        صفحہ_بٹ_2 = صفحہ_بٹ_For_سائز(cap_چوکھٹا_cap_get_cap_FSize(بفر_Cap));
        واپس (لفظ_t *)(بنیاد_Ptr + (w_بفر_Ptr & نقاب(صفحہ_بٹ_2)));
    } ورنہ {
        واپس تہی;
    }
}

ساکن اندرونی pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    واپس PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << se_L4_صفحہ_جدول_بٹ));
}

lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    لفظ_t level = CONFIG_PT_سطحیں - 1;
    pte_t *pt = lvl1pt;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_بٹ_بایاں = PT_اشاریہ_بٹ * level + se_L4_صفحہ_بٹ;
    ret.pt_خانہ = pt + ((vptr >> ret.pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));

    جب (is_PTEPage_جدول(ret.pt_خانہ) && likely(0 < level)) {
        level--;
        ret.pt_بٹ_بایاں -= PT_اشاریہ_بٹ;
        pt = getPPtrFromHWPTE(ret.pt_خانہ);
        ret.pt_خانہ = pt + ((vptr >> ret.pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
    }

    واپس ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    انتخاب (vm_faultType) {
    حالت RISCVLoad_صفحہ_Fault:
    حالت RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        واپس EXCEPTION_FAULT;
    حالت RISCVStore_صفحہ_Fault:
    حالت RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        واپس EXCEPTION_FAULT;
    حالت RISCVInstruction_صفحہ_Fault:
    حالت RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        واپس EXCEPTION_FAULT;

    طےشدہ:
        fail("Invalid VM fault type");
    }
}

خالی deleteASIDPool(asid_t asid_بنیاد, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد(IS_ALIGNED(asid_بنیاد, asid_کم_بٹ));

    اگر (riscvKSASIDTable[ASID_بلند(asid_بنیاد)] == pool) {
        riscvKSASIDTable[ASID_بلند(asid_بنیاد)] = تہی;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

ساکن exception_t performASIDControlInvocation(خالی *چوکھٹا, cte_t *خانہ, cte_t *والد, asid_t asid_بنیاد)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سیٹ_cap_Free_اشاریہ(&(والد->cap),
                                         MAX_FREE_اشاریہ(cap_untyped_cap_get_cap_بلاک_سائز(والد->cap)));

    memzero(چوکھٹا, ثنائی_ہندسہ(صفحہ_بٹ_For_سائز(RISCV_4_K_صفحہ)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_داخل_کرو(
        cap_asid_pool_cap_new(
            asid_بنیاد,          /* capASIDBase  */
            لفظ_REF(چوکھٹا)     /* capASIDPool  */
        ),
        والد,
        خانہ
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنیاد & نقاب(asid_کم_بٹ)) == 0);
    riscvKSASIDTable[ASID_بلند(asid_بنیاد)] = (asid_pool_t *)چوکھٹا;

    واپس EXCEPTION_NONE;
}

ساکن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_خانہ)
{
    cap_t cap = vspace_Cap_خانہ->cap;
    pte_t *region_بنیاد = PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap));
    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTMapped_پتہ(cap, 0);
    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTIs_Mapped(cap, 1);
    vspace_Cap_خانہ->cap = cap;

    copyGlobalMappings(region_بنیاد);

    poolPtr->array[ASID_کم(asid)] = region_بنیاد;

    واپس EXCEPTION_NONE;
}

خالی deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_بلند(asid)];
    اگر (poolPtr != تہی && poolPtr->array[ASID_کم(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_کم(asid)] = تہی;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

خالی unmap_صفحہ_جدول(asid_t asid, vptr_t vptr, pte_t *ہدف_pt)
{
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
        /* nothing to do */
        واپس;
    }
    /* We won't ever unmap a top level page table */
    أكد(ڈھونڈو_ret.vspace_جڑ != ہدف_pt);
    pte_t *pt_خانہ = تہی;
    pte_t *pt = ڈھونڈو_ret.vspace_جڑ;

    برائے (لفظ_t i = 0; i < CONFIG_PT_سطحیں - 1 && pt != ہدف_pt; i++) {
        pt_خانہ = pt + RISCV_GET_PT_اشاریہ(vptr, i);
        اگر (unlikely(!is_PTEPage_جدول(pt_خانہ))) {
            /* couldn't find it */
            واپس;
        }
        pt = getPPtrFromHWPTE(pt_خانہ);
    }

    اگر (pt != ہدف_pt) {
        /* didn't find it */
        واپس;
    }
    /* If we found a pt then ptSlot won't be null */
    أكد(pt_خانہ != تہی);
    *pt_خانہ = pte_new(
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

ساکن pte_t pte_pte_invalid_new(خالی)
{
    واپس (pte_t) {
        0
    };
}

خالی unmap_صفحہ(vm_صفحہ_سائز_t صفحہ_سائز, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;
    lookupPTSlot_ret_t  lu_ret;

    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        واپس;
    }

    lu_ret = lookupPTSlot(ڈھونڈو_ret.vspace_جڑ, vptr);
    اگر (unlikely(lu_ret.pt_بٹ_بایاں != صفحہ_بٹ_For_سائز(صفحہ_سائز))) {
        واپس;
    }
    اگر (!pte_ptr_get_valid(lu_ret.pt_خانہ) || is_PTEPage_جدول(lu_ret.pt_خانہ)
        || (pte_ptr_get_ppn(lu_ret.pt_خانہ) << se_L4_صفحہ_بٹ) != pptr_مقصد_paddr((خالی *)pptr)) {
        واپس;
    }

    lu_ret.pt_خانہ[0] = pte_pte_invalid_new();
    sfence();
}

خالی سیٹ_VMRoot(tcb_t *tcb)
{
    cap_t thread_جڑ;
    asid_t asid;
    pte_t *lvl1pt;
    ڈھونڈو_VSpace_For_ASID_ret_t  ڈھونڈو_ret;

    thread_جڑ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    اگر (cap_get_capType(thread_جڑ) != cap_صفحہ_جدول_cap) {
        سیٹ_VSpace_جڑ(kpptr_مقصد_paddr(&kernel_جڑ_صفحہ_جدول), 0);
        واپس;
    }

    lvl1pt = PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(thread_جڑ));

    asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(thread_جڑ);
    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE || ڈھونڈو_ret.vspace_جڑ != lvl1pt)) {
        سیٹ_VSpace_جڑ(kpptr_مقصد_paddr(&kernel_جڑ_صفحہ_جدول), 0);
        واپس;
    }

    سیٹ_VSpace_جڑ(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_جڑ(cap_t cap)
{
    واپس (cap_get_capType(cap) == cap_صفحہ_جدول_cap &&
            cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    اگر (unlikely(cap_get_capType(cap) != cap_چوکھٹا_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(cap_چوکھٹا_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(!IS_ALIGNED(vptr, se_L4_IPCBuffer_سائز_بٹ))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (صحیح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    واپس EXCEPTION_NONE;
}

vm_rights_t CONST نقاب_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_نقاب)
{
    اگر (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        واپس VMReadOnly;
    }
    اگر (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
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

/* The rest of the file implements the RISCV object invocations */

ساکن pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    لفظ_t اكتب = RISCVGetWriteFromVMRights(vm_rights);
    لفظ_t اقرأ = RISCVGetReadFromVMRights(vm_rights);
    اگر (unlikely(!اقرأ && !اكتب && !executable)) {
        واپس pte_pte_invalid_new();
    } ورنہ {
        واپس pte_new(
                   paddr >> se_L4_صفحہ_بٹ,
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

ساکن اندرونی bool_t CONST checkVPAlignment(vm_صفحہ_سائز_t sz, لفظ_t w)
{
    واپس (w & نقاب(صفحہ_بٹ_For_سائز(sz))) == 0;
}

ساکن exception_t decode_RISCVPage_جدول_Invocation(لفظ_t label, لفظ_t لمبائی,
                                                  cte_t *cte, cap_t cap, لفظ_t *بفر)
{
    اگر (label == RISCVPage_جدول_Unmap) {
        اگر (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = se_L4_Revoke_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        اگر (likely(cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap))) {
            asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap);
            ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap));
            اگر (unlikely(ڈھونڈو_ret.حالت_2 == EXCEPTION_NONE &&
                         ڈھونڈو_ret.vspace_جڑ == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = se_L4_Revoke_پہلا;
                واپس EXCEPTION_SYSCALL_ERROR;
            }
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_جدول_Invocation_Unmap(cap, cte);
    }

    اگر (unlikely((label != RISCVPage_جدول_Map))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (unlikely(cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t vaddr = getSyscallArg(0, بفر);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    اگر (unlikely(cap_get_capType(lvl1ptCap) != cap_صفحہ_جدول_cap ||
                 cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(lvl1ptCap));
    asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(lvl1ptCap);

    اگر (unlikely(vaddr >= USER_چوٹی)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (unlikely(ڈھونڈو_ret.vspace_جڑ != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    اگر (lu_ret.pt_بٹ_بایاں == se_L4_صفحہ_بٹ || pte_ptr_get_valid(lu_ret.pt_خانہ)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = se_L4_Delete_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* Get the slot to install the PT in */
    pte_t *pt_خانہ = lu_ret.pt_خانہ;

    paddr_t paddr = addrFromPPtr(
                        PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap)));
    pte_t pte = pte_new((paddr >> se_L4_صفحہ_بٹ),
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

    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحہ_جدول_cap_سیٹ_cap_PTMapped_پتہ(cap, (vaddr & ~نقاب(lu_ret.pt_بٹ_بایاں)));

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس perform_صفحہ_جدول_Invocation_Map(cap, cte, pte, pt_خانہ);
}

ساکن exception_t decodeRISCVFrameInvocation(لفظ_t label, لفظ_t لمبائی,
                                              cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    انتخاب (label) {
    حالت RISCVPageMap: {
        اگر (unlikely(لمبائی < 3 || current_extra_caps.excaprefs[0] == تہی)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        لفظ_t vaddr = getSyscallArg(0, بفر);
        لفظ_t w_rights_نقاب = getSyscallArg(1, بفر);
        vm_اوصاف_t attr = vm_اوصاف_From_لفظ(getSyscallArg(2, بفر));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_صفحہ_سائز_t چوکھٹا_سائز = cap_چوکھٹا_cap_get_cap_FSize(cap);
        vm_rights_t capVMRights = cap_چوکھٹا_cap_get_cap_FVMRights(cap);

        اگر (unlikely(cap_get_capType(lvl1ptCap) != cap_صفحہ_جدول_cap ||
                     !cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(lvl1ptCap));
        asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(lvl1ptCap);

        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
        اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (unlikely(ڈھونڈو_ret.vspace_جڑ != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        لفظ_t vtop = vaddr + ثنائی_ہندسہ(صفحہ_بٹ_For_سائز(چوکھٹا_سائز)) - 1;
        اگر (unlikely(vtop >= USER_چوٹی)) {
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        اگر (unlikely(!checkVPAlignment(چوکھٹا_سائز, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        اگر (unlikely(lu_ret.pt_بٹ_بایاں != صفحہ_بٹ_For_سائز(چوکھٹا_سائز))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_بٹ_بایاں);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        asid_t چوکھٹا_asid = cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap);
        اگر (unlikely(چوکھٹا_asid != asidInvalid)) {
            /* this frame is already mapped */
            اگر (چوکھٹا_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_نمبر = 1;
                واپس EXCEPTION_SYSCALL_ERROR;
            }
            لفظ_t mapped_vaddr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
            اگر (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;
                واپس EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            اگر (unlikely(is_PTEPage_جدول(lu_ret.pt_خانہ))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = se_L4_Delete_پہلا;
                واپس EXCEPTION_SYSCALL_ERROR;
            }
        } ورنہ {
            /* check this vaddr isn't already mapped */
            اگر (unlikely(pte_ptr_get_valid(lu_ret.pt_خانہ))) {
                userError("Virtual address (0x%"SEL4_PRIx_لفظ") already mapped", vaddr);
                current_syscall_error.type = se_L4_Delete_پہلا;
                واپس EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = نقاب_VMRights(capVMRights, rights_From_لفظ(w_rights_نقاب));
        paddr_t چوکھٹا_paddr = addrFromPPtr((خالی *) cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap,  vaddr);

        bool_t executable = !vm_اوصاف_get_riscv_Execute_Never(attr);
        pte_t pte = makeUserPTE(چوکھٹا_paddr, executable, vmRights);
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_Invocation_Map_PTE(cap, cte, pte, lu_ret.pt_خانہ);
    }

    حالت RISCVPageUnmap: {
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_Invocation_Unmap(cap, cte);
    }

    حالت RISCVPage_Get_پتہ: {

        /* Check that there are enough message registers */
        أكد(n_پیغام_Registers >= 1);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس perform_صفحہ_Get_پتہ((خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap), call);
    }

    طےشدہ:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(لفظ_t label, لفظ_t لمبائی, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, لفظ_t *بفر)
{
    انتخاب (cap_get_capType(cap)) {

    حالت cap_صفحہ_جدول_cap:
        واپس decode_RISCVPage_جدول_Invocation(label, لمبائی, cte, cap, بفر);

    حالت cap_چوکھٹا_cap:
        واپس decodeRISCVFrameInvocation(label, لمبائی, cte, cap, call, بفر);

    حالت cap_asid_control_cap: {
        لفظ_t     i;
        asid_t           asid_بنیاد;
        لفظ_t           اشاریہ;
        لفظ_t           گہرائی;
        cap_t            untyped;
        cap_t            جڑ;
        cte_t           *والد_خانہ;
        cte_t           *dest_خانہ;
        lookup_خانہ_ret_t lu_ret;
        خالی            *چوکھٹا;
        exception_t      حالت_2;

        اگر (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی
            || current_extra_caps.excaprefs[1] == تہی) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اشاریہ = getSyscallArg(0, بفر);
        گہرائی = getSyscallArg(1, بفر);
        والد_خانہ = current_extra_caps.excaprefs[0];
        untyped = والد_خانہ->cap;
        جڑ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        برائے (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        اگر (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پہلا;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنیاد = i << asid_کم_بٹ;

        اگر (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_بلاک_سائز(untyped) != se_L4_ASIDPool_بٹ ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        حالت_2 = ensureNoChildren(والد_خانہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }

        چوکھٹا = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ہدف_خانہ(جڑ, اشاریہ, گہرائی);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            واپس lu_ret.حالت_2;
        }
        dest_خانہ = lu_ret.خانہ;

        حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performASIDControlInvocation(چوکھٹا, dest_خانہ, والد_خانہ, asid_بنیاد);
    }

    حالت cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_خانہ;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t       asid;

        اگر (label != RISCVASIDPool_تفویض) {
            current_syscall_error.type = seL4_IllegalOperation;

            واپس EXCEPTION_SYSCALL_ERROR;
        }
        اگر (current_extra_caps.excaprefs[0] == تہی) {
            current_syscall_error.type = se_L4_Truncated_پیغام;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_خانہ = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_خانہ->cap;

        اگر (unlikely(
                cap_get_capType(vspaceCap) != cap_صفحہ_جدول_cap ||
                cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_بلند(cap_asid_pool_cap_get_capASIDBase(cap))];
        اگر (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = false;
            current_lookup_fault = lookup_fault_invalid_جڑ_new();
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        برائے (i = 0; i < ثنائی_ہندسہ(asid_کم_بٹ) && (asid + i == 0 || pool->array[i]); i++);

        اگر (i == ثنائی_ہندسہ(asid_کم_بٹ)) {
            current_syscall_error.type = se_L4_Delete_پہلا;

            واپس EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس performASIDPoolInvocation(asid, pool, vspace_Cap_خانہ);
    }
    طےشدہ:
        fail("Invalid arch cap type");
    }
}

exception_t perform_صفحہ_جدول_Invocation_Map(cap_t cap, cte_t *ct_خانہ,
                                          pte_t pte, pte_t *pt_خانہ)
{
    ct_خانہ->cap = cap;
    *pt_خانہ = pte;
    sfence();

    واپس EXCEPTION_NONE;
}

exception_t perform_صفحہ_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_خانہ)
{
    اگر (cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحہ_جدول(
            cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap),
            pt
        );
        clear_حافظہ((خالی *)pt, se_L4_صفحہ_جدول_بٹ);
    }
    cap_صفحہ_جدول_cap_ptr_سیٹ_cap_PTIs_Mapped(&(ct_خانہ->cap), 0);

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

ساکن exception_t updatePTE(pte_t pte, pte_t *بنیاد)
{
    *بنیاد = pte;
    sfence();
    واپس EXCEPTION_NONE;
}

exception_t perform_صفحہ_Invocation_Map_PTE(cap_t cap, cte_t *ct_خانہ,
                                        pte_t pte, pte_t *بنیاد)
{
    ct_خانہ->cap = cap;
    واپس updatePTE(pte, بنیاد);
}

exception_t perform_صفحہ_Invocation_Unmap(cap_t cap, cte_t *ct_خانہ)
{
    اگر (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        unmap_صفحہ(cap_چوکھٹا_cap_get_cap_FSize(cap),
                  cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                  cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                  cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
                 );
    }

    cap_t خانہ_Cap = ct_خانہ->cap;
    خانہ_Cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(خانہ_Cap, 0);
    خانہ_Cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(خانہ_Cap, asidInvalid);
    ct_خانہ->cap = خانہ_Cap;

    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ CONFIG_PRINTING
خالی Arch_user_ڈھیر_Trace(tcb_t *tptr)
{
    cap_t thread_جڑ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    اگر (!is_Valid_VTable_جڑ(thread_جڑ)) {
        لکھو("Invalid vspace\n");
        واپس;
    }

    لفظ_t sp = get_رجسٹر(tptr, SP);
    اگر (!IS_ALIGNED(sp, se_L4_لفظ_سائز_بٹ)) {
        لکھو("SP %p not aligned", (خالی *) sp);
        واپس;
    }

    pte_t *vspace_جڑ = PTE_PTR(pptr_کا_cap(thread_جڑ));
    برائے (صحیح i = 0; i < CONFIG_USER_ڈھیر_TRACE_لمبائی; i++) {
        لفظ_t پتہ = sp + (i * حجم(لفظ_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_جڑ, پتہ);
        اگر (پتہ < USER_چوٹی && pte_ptr_get_valid(ret.pt_خانہ) && !is_PTEPage_جدول(ret.pt_خانہ)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.pt_خانہ));
            لفظ_t *قدر = (لفظ_t *)((لفظ_t)pptr + (پتہ & نقاب(ret.pt_بٹ_بایاں)));
            لکھو("0x%lx: 0x%lx\n", (طویل) پتہ, (طویل) *قدر);
        } ورنہ {
            لکھو("0x%lx: INVALID\n", (طویل) پتہ);
        }
    }
}
#اگر_ختم

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

    چوکھٹا_سائز = cap_چوکھٹا_cap_get_cap_FSize(lu_ret.cap);

    اگر (چوکھٹا_سائز != RISCV_Mega_صفحہ) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_pptr = cap_چوکھٹا_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_مقصد_paddr((خالی *) چوکھٹا_pptr);

#اگر __riscv_xlen == 32
    paddr_t physical_پتہ = ks_User_Log_بفر;
    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i += 1) {
        kernel_image_level1_log_بفر_pt[i] = pte_اگلا(physical_پتہ, true);
        physical_پتہ += ثنائی_ہندسہ(صفحہ_بٹ);
    }
    أكد(physical_پتہ - ks_User_Log_بفر == ثنائی_ہندسہ(se_L4_Large_صفحہ_بٹ));
#ورنہ
    kernel_image_level1_dev_pt[RISCV_GET_PT_اشاریہ(KS_LOG_PPTR, 1)] = pte_اگلا(ks_User_Log_بفر, true);
#اگر_ختم

    sfence();

    واپس EXCEPTION_NONE;
}
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */
