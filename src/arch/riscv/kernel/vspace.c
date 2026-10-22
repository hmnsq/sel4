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
    paddr_t قاب_پایه;
    vm_صفحه_اندازه_t قاب_اندازه;
    bool_t valid;
};
تعريفنوع بنية resolve_ret resolve_ret_t;

ساكن exception_t perform_صفحه_Get_آدرس(فراغ *vbase_ptr, bool_t call);

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
ساكن pte_t pte_بعدی(کلمه_t phys_addr, bool_t is_برگ)
{
    کلمه_t ppn = (کلمه_t)(phys_addr >> 12);

    uint8_t اقرأ = is_برگ ? 1 : 0;
    uint8_t اكتب = اقرأ;
    uint8_t exec = اقرأ;

    أرجع pte_new(ppn,
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

BOOT_کد فراغ map_kernel_قاب(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#إذا __riscv_xlen == 32
    paddr = دور_پایین(paddr, RISCV_GET_LVL_PGSIZE_بیتها(0));
    أكد((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_ریشه_صفحه_جدول[RISCV_GET_PT_شاخص(vaddr, 0)] = pte_بعدی(paddr, true);
#وإلا
    إذا (vaddr >= KDEV_پایه) {
        /* Map devices in level 1 page table */
        paddr = دور_پایین(paddr, RISCV_GET_LVL_PGSIZE_بیتها(1));
        أكد((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_شاخص(vaddr, 1)] = pte_بعدی(paddr, true);
    } وإلا {
        paddr = دور_پایین(paddr, RISCV_GET_LVL_PGSIZE_بیتها(0));
        أكد((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_ریشه_صفحه_جدول[RISCV_GET_PT_شاخص(vaddr, 0)] = pte_بعدی(paddr, true);
    }
#پایان_اگر
}

BOOT_کد VISIBLE فراغ map_kernel_پنجره(فراغ)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    أكد(CONFIG_PT_سطوح > 1 && CONFIG_PT_سطوح <= 4);
    /* Kernel image finishes before KDEV_BASE */
    أكد(KERNEL_ELF_بالاترین <= KDEV_پایه);

    /* kernel window starts at PPTR_BASE */
    کلمه_t pptr = PPTR_پایه;

    /* first we map in memory from PADDR_BASE */
    کلمه_t paddr = PADDR_پایه;
    طالما (pptr < PPTR_بالاترین) {
        أكد(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_بیتها(0)));
        أكد(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_بیتها(0)));

        kernel_ریشه_صفحه_جدول[RISCV_GET_PT_شاخص(pptr, 0)] = pte_بعدی(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    أكد(pptr == PPTR_بالاترین);
    pptr = دور_پایین(KERNEL_ELF_پایه, RISCV_GET_LVL_PGSIZE_بیتها(0));
    paddr = دور_پایین(KERNEL_ELF_PADDR_پایه, RISCV_GET_LVL_PGSIZE_بیتها(0));

#إذا __riscv_xlen == 32
    kernel_ریشه_صفحه_جدول[RISCV_GET_PT_شاخص(pptr, 0)] = pte_بعدی(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
    kernel_ریشه_صفحه_جدول[RISCV_GET_PT_شاخص(KS_LOG_PPTR, 0)] =
        pte_بعدی(kpptr_مقصد_paddr(kernel_image_level1_log_میانگیر_pt), false);
#پایان_اگر
#وإلا
    کلمه_t شاخص = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_ریشه_صفحه_جدول[RISCV_GET_PT_شاخص(KERNEL_ELF_PADDR_پایه + PPTR_پایه_جابه_جایی, 0)] =
        pte_بعدی(kpptr_مقصد_paddr(kernel_image_level1_pt), false);
    kernel_ریشه_صفحه_جدول[RISCV_GET_PT_شاخص(pptr, 0)] =
        pte_بعدی(kpptr_مقصد_paddr(kernel_image_level1_pt), false);
    طالما (pptr < PPTR_بالاترین + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[شاخص] = pte_بعدی(paddr, true);
        شاخص++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_ریشه_صفحه_جدول[RISCV_GET_PT_شاخص(KDEV_پایه, 0)] =
        pte_بعدی(kpptr_مقصد_paddr(kernel_image_level1_dev_pt), false);
#پایان_اگر

    /* There should be free space where we put device mapping */
    أكد(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_کد فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    lookupPTSlot_ret_t pt_ret;
    pte_t *هدف_جایگاه;
    vptr_t vptr = cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(pt_cap);
    pte_t *lvl1pt = PTE_PTR(pptr_از_cap(vspace_cap));

    /* pt to be mapped */
    pte_t *pt   = PTE_PTR(pptr_از_cap(pt_cap));

    /* Get PT slot to install the address in */
    pt_ret = lookupPTSlot(lvl1pt, vptr);

    هدف_جایگاه = pt_ret.pt_جایگاه;

    *هدف_جایگاه = pte_new(
                      (addrFromPPtr(pt) >> se_L4_صفحه_بیتها),
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

BOOT_کد فراغ map_it_قاب_cap(cap_t vspace_cap, cap_t قاب_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_از_cap(vspace_cap));
    pte_t *قاب_pptr   = PTE_PTR(pptr_از_cap(قاب_cap));
    vptr_t قاب_vptr = cap_قاب_cap_get_cap_FMapped_آدرس(قاب_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, قاب_vptr);
    أكد(lu_ret.pt_بیتها_چپ == se_L4_صفحه_بیتها);

    pte_t *هدف_جایگاه = lu_ret.pt_جایگاه;

    *هدف_جایگاه = pte_new(
                      (pptr_مقصد_paddr(قاب_pptr) >> se_L4_صفحه_بیتها),
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

BOOT_کد cap_t create_unmapped_it_قاب_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_قاب_cap_new(
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
ساكن BOOT_کد cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحه_جدول_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}

BOOT_کد کلمه_t arch_get_n_paging(v_region_t it_v_reg)
{
    کلمه_t n = 0;
    لكل (صحيح i = 0; i < CONFIG_PT_سطوح - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_بیتها(i));
    }
    أرجع n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_کد cap_t create_it_آدرس_space(cap_t ریشه_cnode_cap, v_region_t it_v_reg)
{
    cap_t      lvl1pt_cap;
    vptr_t     pt_vptr;

    copyGlobalMappings(PTE_PTR(rootserver.vspace));

    lvl1pt_cap =
        cap_صفحه_جدول_cap_new(
            IT_ASID,               /* capPTMappedASID    */
            (کلمه_t) rootserver.vspace,  /* capPTBasePtr       */
            1,                     /* capPTIsMapped      */
            (کلمه_t) rootserver.vspace   /* capPTMappedAddress */
        );

    se_L4_جایگاه_Pos جایگاه_pos_before = ndks_boot.جایگاه_pos_cur;
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    لكل (صحيح i = 0; i < CONFIG_PT_سطوح - 1; i++) {

        لكل (pt_vptr = دور_پایین(it_v_reg.شروع, RISCV_GET_LVL_PGSIZE_بیتها(i));
             pt_vptr < it_v_reg.انتها;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            إذا (!provide_cap(ریشه_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                أرجع cap_null_cap_new();
            }
        }

    }

    se_L4_جایگاه_Pos جایگاه_pos_after = ndks_boot.جایگاه_pos_cur;
    ndks_boot.bi_قاب->userImagePaging = (se_L4_جایگاه_Region) {
        جایگاه_pos_before, جایگاه_pos_after
    };

    أرجع lvl1pt_cap;
}

BOOT_کد فراغ activate_kernel_vspace(فراغ)
{
    مجموعه_VSpace_ریشه(kpptr_مقصد_paddr(&kernel_ریشه_صفحه_جدول), 0);
}

BOOT_کد فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_از_cap(it_ap_cap));
    ap->array[ASID_کم(IT_ASID)] = PTE_PTR(pptr_از_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_بالا(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

ساكن پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_VSpace_For_ASID(asid_t asid)
{
    پیدا_کن_VSpace_For_ASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_ریشه;

    poolPtr = riscvKSASIDTable[ASID_بالا(asid)];
    إذا (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_ریشه_new();

        ret.vspace_ریشه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    vspace_ریشه = poolPtr->array[ASID_کم(asid)];
    إذا (!vspace_ریشه) {
        current_lookup_fault = lookup_fault_invalid_ریشه_new();

        ret.vspace_ریشه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_ریشه = vspace_ریشه;
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

فراغ copyGlobalMappings(pte_t *newLvl1pt)
{
    بیعلامت دراز i;
    pte_t *global_kernel_vspace = kernel_ریشه_صفحه_جدول;

    لكل (i = RISCV_GET_PT_شاخص(PPTR_پایه, 0); i < بیت(PT_شاخص_بیتها); i++) {
        newLvl1pt[i] = global_kernel_vspace[i];
    }
}

کلمه_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    کلمه_t w_میانگیر_Ptr;
    cap_t میانگیر_Cap;
    vm_rights_t vm_rights;

    w_میانگیر_Ptr = thread->tcbIPCBuffer;
    میانگیر_Cap = TCB_PTR_CTE_PTR(thread, tcb_میانگیر)->cap;

    إذا (unlikely(cap_get_capType(میانگیر_Cap) != cap_قاب_cap)) {
        أرجع عدم;
    }
    إذا (unlikely(cap_قاب_cap_get_cap_FIs_Device(میانگیر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_قاب_cap_get_cap_FVMRights(میانگیر_Cap);
    إذا (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        کلمه_t پایه_Ptr, صفحه_بیتها_2;

        پایه_Ptr = cap_قاب_cap_get_cap_FBase_Ptr(میانگیر_Cap);
        صفحه_بیتها_2 = صفحه_بیتها_For_اندازه(cap_قاب_cap_get_cap_FSize(میانگیر_Cap));
        أرجع (کلمه_t *)(پایه_Ptr + (w_میانگیر_Ptr & ماسک(صفحه_بیتها_2)));
    } وإلا {
        أرجع عدم;
    }
}

ساكن مضمن pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    أرجع PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << se_L4_صفحه_جدول_بیتها));
}

lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    کلمه_t level = CONFIG_PT_سطوح - 1;
    pte_t *pt = lvl1pt;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_بیتها_چپ = PT_شاخص_بیتها * level + se_L4_صفحه_بیتها;
    ret.pt_جایگاه = pt + ((vptr >> ret.pt_بیتها_چپ) & ماسک(PT_شاخص_بیتها));

    طالما (is_PTEPage_جدول(ret.pt_جایگاه) && likely(0 < level)) {
        level--;
        ret.pt_بیتها_چپ -= PT_شاخص_بیتها;
        pt = getPPtrFromHWPTE(ret.pt_جایگاه);
        ret.pt_جایگاه = pt + ((vptr >> ret.pt_بیتها_چپ) & ماسک(PT_شاخص_بیتها));
    }

    أرجع ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    بدل (vm_faultType) {
    حالة RISCVLoad_صفحه_Fault:
    حالة RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        أرجع EXCEPTION_FAULT;
    حالة RISCVStore_صفحه_Fault:
    حالة RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        أرجع EXCEPTION_FAULT;
    حالة RISCVInstruction_صفحه_Fault:
    حالة RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        أرجع EXCEPTION_FAULT;

    افتراضي:
        fail("Invalid VM fault type");
    }
}

فراغ deleteASIDPool(asid_t asid_پایه, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد(IS_ALIGNED(asid_پایه, asid_کم_بیتها));

    إذا (riscvKSASIDTable[ASID_بالا(asid_پایه)] == pool) {
        riscvKSASIDTable[ASID_بالا(asid_پایه)] = عدم;
        مجموعه_VMRoot(گره_STATE(ksCurThread));
    }
}

ساكن exception_t performASIDControlInvocation(فراغ *قاب, cte_t *جایگاه, cte_t *والد, asid_t asid_پایه)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_مجموعه_cap_Free_شاخص(&(والد->cap),
                                         MAX_FREE_شاخص(cap_untyped_cap_get_cap_بلوک_اندازه(والد->cap)));

    memzero(قاب, بیت(صفحه_بیتها_For_اندازه(RISCV_4_K_صفحه)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_درج(
        cap_asid_pool_cap_new(
            asid_پایه,          /* capASIDBase  */
            کلمه_REF(قاب)     /* capASIDPool  */
        ),
        والد,
        جایگاه
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_پایه & ماسک(asid_کم_بیتها)) == 0);
    riscvKSASIDTable[ASID_بالا(asid_پایه)] = (asid_pool_t *)قاب;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_جایگاه)
{
    cap_t cap = vspace_Cap_جایگاه->cap;
    pte_t *region_پایه = PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap));
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTMapped_آدرس(cap, 0);
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTIs_Mapped(cap, 1);
    vspace_Cap_جایگاه->cap = cap;

    copyGlobalMappings(region_پایه);

    poolPtr->array[ASID_کم(asid)] = region_پایه;

    أرجع EXCEPTION_NONE;
}

فراغ deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_بالا(asid)];
    إذا (poolPtr != عدم && poolPtr->array[ASID_کم(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_کم(asid)] = عدم;
        مجموعه_VMRoot(گره_STATE(ksCurThread));
    }
}

فراغ unmap_صفحه_جدول(asid_t asid, vptr_t vptr, pte_t *هدف_pt)
{
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
        /* nothing to do */
        أرجع;
    }
    /* We won't ever unmap a top level page table */
    أكد(پیدا_کن_ret.vspace_ریشه != هدف_pt);
    pte_t *pt_جایگاه = عدم;
    pte_t *pt = پیدا_کن_ret.vspace_ریشه;

    لكل (کلمه_t i = 0; i < CONFIG_PT_سطوح - 1 && pt != هدف_pt; i++) {
        pt_جایگاه = pt + RISCV_GET_PT_شاخص(vptr, i);
        إذا (unlikely(!is_PTEPage_جدول(pt_جایگاه))) {
            /* couldn't find it */
            أرجع;
        }
        pt = getPPtrFromHWPTE(pt_جایگاه);
    }

    إذا (pt != هدف_pt) {
        /* didn't find it */
        أرجع;
    }
    /* If we found a pt then ptSlot won't be null */
    أكد(pt_جایگاه != عدم);
    *pt_جایگاه = pte_new(
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

فراغ unmap_صفحه(vm_صفحه_اندازه_t صفحه_اندازه, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;
    lookupPTSlot_ret_t  lu_ret;

    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPTSlot(پیدا_کن_ret.vspace_ریشه, vptr);
    إذا (unlikely(lu_ret.pt_بیتها_چپ != صفحه_بیتها_For_اندازه(صفحه_اندازه))) {
        أرجع;
    }
    إذا (!pte_ptr_get_valid(lu_ret.pt_جایگاه) || is_PTEPage_جدول(lu_ret.pt_جایگاه)
        || (pte_ptr_get_ppn(lu_ret.pt_جایگاه) << se_L4_صفحه_بیتها) != pptr_مقصد_paddr((فراغ *)pptr)) {
        أرجع;
    }

    lu_ret.pt_جایگاه[0] = pte_pte_invalid_new();
    sfence();
}

فراغ مجموعه_VMRoot(tcb_t *tcb)
{
    cap_t thread_ریشه;
    asid_t asid;
    pte_t *lvl1pt;
    پیدا_کن_VSpace_For_ASID_ret_t  پیدا_کن_ret;

    thread_ریشه = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_ریشه) != cap_صفحه_جدول_cap) {
        مجموعه_VSpace_ریشه(kpptr_مقصد_paddr(&kernel_ریشه_صفحه_جدول), 0);
        أرجع;
    }

    lvl1pt = PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(thread_ریشه));

    asid = cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(thread_ریشه);
    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE || پیدا_کن_ret.vspace_ریشه != lvl1pt)) {
        مجموعه_VSpace_ریشه(kpptr_مقصد_paddr(&kernel_ریشه_صفحه_جدول), 0);
        أرجع;
    }

    مجموعه_VSpace_ریشه(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_ریشه(cap_t cap)
{
    أرجع (cap_get_capType(cap) == cap_صفحه_جدول_cap &&
            cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (unlikely(cap_get_capType(cap) != cap_قاب_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(cap_قاب_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(!IS_ALIGNED(vptr, se_L4_IPCBuffer_اندازه_بیتها))) {
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
                   paddr >> se_L4_صفحه_بیتها,
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

ساكن مضمن bool_t CONST checkVPAlignment(vm_صفحه_اندازه_t sz, کلمه_t w)
{
    أرجع (w & ماسک(صفحه_بیتها_For_اندازه(sz))) == 0;
}

ساكن exception_t decode_RISCVPage_جدول_Invocation(کلمه_t label, کلمه_t درازی,
                                                  cte_t *cte, cap_t cap, کلمه_t *میانگیر)
{
    إذا (label == RISCVPage_جدول_Unmap) {
        إذا (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = se_L4_Revoke_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        إذا (likely(cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap))) {
            asid_t asid = cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap);
            پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap));
            إذا (unlikely(پیدا_کن_ret.وضعیت == EXCEPTION_NONE &&
                         پیدا_کن_ret.vspace_ریشه == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = se_L4_Revoke_اولی;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (unlikely((label != RISCVPage_جدول_Map))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(درازی < 2 || current_extra_caps.excaprefs[0] == عدم)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کلمه_t vaddr = getSyscallArg(0, میانگیر);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    إذا (unlikely(cap_get_capType(lvl1ptCap) != cap_صفحه_جدول_cap ||
                 cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(lvl1ptCap));
    asid_t asid = cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(lvl1ptCap);

    إذا (unlikely(vaddr >= USER_بالاترین)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (unlikely(پیدا_کن_ret.vspace_ریشه != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    إذا (lu_ret.pt_بیتها_چپ == se_L4_صفحه_بیتها || pte_ptr_get_valid(lu_ret.pt_جایگاه)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = se_L4_Delete_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Get the slot to install the PT in */
    pte_t *pt_جایگاه = lu_ret.pt_جایگاه;

    paddr_t paddr = addrFromPPtr(
                        PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap)));
    pte_t pte = pte_new((paddr >> se_L4_صفحه_بیتها),
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

    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTMapped_آدرس(cap, (vaddr & ~ماسک(lu_ret.pt_بیتها_چپ)));

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_صفحه_جدول_Invocation_Map(cap, cte, pte, pt_جایگاه);
}

ساكن exception_t decodeRISCVFrameInvocation(کلمه_t label, کلمه_t درازی,
                                              cte_t *cte, cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    بدل (label) {
    حالة RISCVPageMap: {
        إذا (unlikely(درازی < 3 || current_extra_caps.excaprefs[0] == عدم)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        کلمه_t vaddr = getSyscallArg(0, میانگیر);
        کلمه_t w_rights_ماسک = getSyscallArg(1, میانگیر);
        vm_صفات_t attr = vm_صفات_From_کلمه(getSyscallArg(2, میانگیر));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_صفحه_اندازه_t قاب_اندازه = cap_قاب_cap_get_cap_FSize(cap);
        vm_rights_t capVMRights = cap_قاب_cap_get_cap_FVMRights(cap);

        إذا (unlikely(cap_get_capType(lvl1ptCap) != cap_صفحه_جدول_cap ||
                     !cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(lvl1ptCap));
        asid_t asid = cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(lvl1ptCap);

        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (unlikely(پیدا_کن_ret.vspace_ریشه != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        کلمه_t vtop = vaddr + بیت(صفحه_بیتها_For_اندازه(قاب_اندازه)) - 1;
        إذا (unlikely(vtop >= USER_بالاترین)) {
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (unlikely(!checkVPAlignment(قاب_اندازه, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        إذا (unlikely(lu_ret.pt_بیتها_چپ != صفحه_بیتها_For_اندازه(قاب_اندازه))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_بیتها_چپ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_t قاب_asid = cap_قاب_cap_get_cap_FMapped_ASID(cap);
        إذا (unlikely(قاب_asid != asidInvalid)) {
            /* this frame is already mapped */
            إذا (قاب_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
            کلمه_t mapped_vaddr = cap_قاب_cap_get_cap_FMapped_آدرس(cap);
            إذا (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_آرگومان;
                current_syscall_error.invalid_آرگومان_عدد = 0;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            إذا (unlikely(is_PTEPage_جدول(lu_ret.pt_جایگاه))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = se_L4_Delete_اولی;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            /* check this vaddr isn't already mapped */
            إذا (unlikely(pte_ptr_get_valid(lu_ret.pt_جایگاه))) {
                userError("Virtual address (0x%"SEL4_PRIx_کلمه") already mapped", vaddr);
                current_syscall_error.type = se_L4_Delete_اولی;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = ماسک_VMRights(capVMRights, rights_From_کلمه(w_rights_ماسک));
        paddr_t قاب_paddr = addrFromPPtr((فراغ *) cap_قاب_cap_get_cap_FBase_Ptr(cap));
        cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(cap, asid);
        cap = cap_قاب_cap_مجموعه_cap_FMapped_آدرس(cap,  vaddr);

        bool_t executable = !vm_صفات_get_riscv_Execute_Never(attr);
        pte_t pte = makeUserPTE(قاب_paddr, executable, vmRights);
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_Invocation_Map_PTE(cap, cte, pte, lu_ret.pt_جایگاه);
    }

    حالة RISCVPageUnmap: {
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_Invocation_Unmap(cap, cte);
    }

    حالة RISCVPage_Get_آدرس: {

        /* Check that there are enough message registers */
        أكد(n_پیام_Registers >= 1);

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_Get_آدرس((فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(کلمه_t label, کلمه_t درازی, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_صفحه_جدول_cap:
        أرجع decode_RISCVPage_جدول_Invocation(label, درازی, cte, cap, میانگیر);

    حالة cap_قاب_cap:
        أرجع decodeRISCVFrameInvocation(label, درازی, cte, cap, call, میانگیر);

    حالة cap_asid_control_cap: {
        کلمه_t     i;
        asid_t           asid_پایه;
        کلمه_t           شاخص;
        کلمه_t           عمق;
        cap_t            untyped;
        cap_t            ریشه;
        cte_t           *والد_جایگاه;
        cte_t           *dest_جایگاه;
        lookup_جایگاه_ret_t lu_ret;
        فراغ            *قاب;
        exception_t      وضعیت;

        إذا (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (درازی < 2 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شاخص = getSyscallArg(0, میانگیر);
        عمق = getSyscallArg(1, میانگیر);
        والد_جایگاه = current_extra_caps.excaprefs[0];
        untyped = والد_جایگاه->cap;
        ریشه = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        إذا (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_اولی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_پایه = i << asid_کم_بیتها;

        إذا (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_بلوک_اندازه(untyped) != se_L4_ASIDPool_بیتها ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        وضعیت = ensureNoChildren(والد_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

        قاب = کلمه_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_هدف_جایگاه(ریشه, شاخص, عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            أرجع lu_ret.وضعیت;
        }
        dest_جایگاه = lu_ret.جایگاه;

        وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(قاب, dest_جایگاه, والد_جایگاه, asid_پایه);
    }

    حالة cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_جایگاه;
        asid_pool_t *pool;
        کلمه_t i;
        asid_t       asid;

        إذا (label != RISCVASIDPool_تخصیص_بده) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_جایگاه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_جایگاه->cap;

        إذا (unlikely(
                cap_get_capType(vspaceCap) != cap_صفحه_جدول_cap ||
                cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_بالا(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            current_lookup_fault = lookup_fault_invalid_ریشه_new();
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < بیت(asid_کم_بیتها) && (asid + i == 0 || pool->array[i]); i++);

        إذا (i == بیت(asid_کم_بیتها)) {
            current_syscall_error.type = se_L4_Delete_اولی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_جایگاه);
    }
    افتراضي:
        fail("Invalid arch cap type");
    }
}

exception_t perform_صفحه_جدول_Invocation_Map(cap_t cap, cte_t *ct_جایگاه,
                                          pte_t pte, pte_t *pt_جایگاه)
{
    ct_جایگاه->cap = cap;
    *pt_جایگاه = pte;
    sfence();

    أرجع EXCEPTION_NONE;
}

exception_t perform_صفحه_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_جایگاه)
{
    إذا (cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحه_جدول(
            cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap),
            pt
        );
        clear_حافظه((فراغ *)pt, se_L4_صفحه_جدول_بیتها);
    }
    cap_صفحه_جدول_cap_ptr_مجموعه_cap_PTIs_Mapped(&(ct_جایگاه->cap), 0);

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

ساكن exception_t updatePTE(pte_t pte, pte_t *پایه)
{
    *پایه = pte;
    sfence();
    أرجع EXCEPTION_NONE;
}

exception_t perform_صفحه_Invocation_Map_PTE(cap_t cap, cte_t *ct_جایگاه,
                                        pte_t pte, pte_t *پایه)
{
    ct_جایگاه->cap = cap;
    أرجع updatePTE(pte, پایه);
}

exception_t perform_صفحه_Invocation_Unmap(cap_t cap, cte_t *ct_جایگاه)
{
    إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        unmap_صفحه(cap_قاب_cap_get_cap_FSize(cap),
                  cap_قاب_cap_get_cap_FMapped_ASID(cap),
                  cap_قاب_cap_get_cap_FMapped_آدرس(cap),
                  cap_قاب_cap_get_cap_FBase_Ptr(cap)
                 );
    }

    cap_t جایگاه_Cap = ct_جایگاه->cap;
    جایگاه_Cap = cap_قاب_cap_مجموعه_cap_FMapped_آدرس(جایگاه_Cap, 0);
    جایگاه_Cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(جایگاه_Cap, asidInvalid);
    ct_جایگاه->cap = جایگاه_Cap;

    أرجع EXCEPTION_NONE;
}

#اگر_تعریف_شده CONFIG_PRINTING
فراغ Arch_user_پشته_Trace(tcb_t *tptr)
{
    cap_t thread_ریشه = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    إذا (!is_Valid_VTable_ریشه(thread_ریشه)) {
        چاپ_کن("Invalid vspace\n");
        أرجع;
    }

    کلمه_t sp = get_ثبات(tptr, SP);
    إذا (!IS_ALIGNED(sp, se_L4_کلمه_اندازه_بیتها)) {
        چاپ_کن("SP %p not aligned", (فراغ *) sp);
        أرجع;
    }

    pte_t *vspace_ریشه = PTE_PTR(pptr_از_cap(thread_ریشه));
    لكل (صحيح i = 0; i < CONFIG_USER_پشته_TRACE_درازی; i++) {
        کلمه_t آدرس = sp + (i * حجم(کلمه_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_ریشه, آدرس);
        إذا (آدرس < USER_بالاترین && pte_ptr_get_valid(ret.pt_جایگاه) && !is_PTEPage_جدول(ret.pt_جایگاه)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.pt_جایگاه));
            کلمه_t *مقدار = (کلمه_t *)((کلمه_t)pptr + (آدرس & ماسک(ret.pt_بیتها_چپ)));
            چاپ_کن("0x%lx: 0x%lx\n", (دراز) آدرس, (دراز) *مقدار);
        } وإلا {
            چاپ_کن("0x%lx: INVALID\n", (دراز) آدرس);
        }
    }
}
#پایان_اگر

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

    قاب_اندازه = cap_قاب_cap_get_cap_FSize(lu_ret.cap);

    إذا (قاب_اندازه != RISCV_Mega_صفحه) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(قاب_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    قاب_pptr = cap_قاب_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_میانگیر = pptr_مقصد_paddr((فراغ *) قاب_pptr);

#إذا __riscv_xlen == 32
    paddr_t physical_آدرس = ks_User_Log_میانگیر;
    لكل (کلمه_t i = 0; i < بیت(PT_شاخص_بیتها); i += 1) {
        kernel_image_level1_log_میانگیر_pt[i] = pte_بعدی(physical_آدرس, true);
        physical_آدرس += بیت(صفحه_بیتها);
    }
    أكد(physical_آدرس - ks_User_Log_میانگیر == بیت(se_L4_Large_صفحه_بیتها));
#وإلا
    kernel_image_level1_dev_pt[RISCV_GET_PT_شاخص(KS_LOG_PPTR, 1)] = pte_بعدی(ks_User_Log_میانگیر, true);
#پایان_اگر

    sfence();

    أرجع EXCEPTION_NONE;
}
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */
