/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <benchmark/benchmark.h>
#অন্তর্ভাৱ <api/failures.h>
#অন্তর্ভাৱ <api/syscall.h>
#অন্তর্ভাৱ <kernel/boot.h>
#অন্তর্ভাৱ <kernel/cspace.h>
#অন্তর্ভাৱ <kernel/thread.h>
#অন্তর্ভাৱ <object/tcb.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <model/preemption.h>
#অন্তর্ভাৱ <model/statedata.h>
#অন্তর্ভাৱ <object/cnode.h>
#অন্তর্ভাৱ <object/untyped.h>
#অন্তর্ভাৱ <arch/api/invocation.h>
#অন্তর্ভাৱ <arch/kernel/vspace.h>
#অন্তর্ভাৱ <linker.h>
#অন্তর্ভাৱ <arch/machine.h>
#অন্তর্ভাৱ <plat/machine/hardware.h>
#অন্তর্ভাৱ <kernel/stack.h>
#অন্তর্ভাৱ <util.h>

সংরচনা resolve_ret {
    paddr_t frameBase;
    vm_page_size_t frameSize;
    bool_t valid;
};
প্রকারনির্দেশ সংরচনা resolve_ret resolve_ret_t;

স্থির exception_t performPageGetAddress(শূন্য *vbase_ptr, bool_t call);

স্থির word_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    ঘূৰাই vm_rights == VMReadWrite;
}

স্থির অন্তঃস্থ word_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported.
     * Kernel-only conveys no user rights. */
    ঘূৰাই vm_rights != VMKernelOnly;
}

স্থির অন্তঃস্থ bool_t isPTEPageTable(pte_t *pte)
{
    ঘূৰাই pte_ptr_get_valid(pte) &&
           !(pte_ptr_get_read(pte) || pte_ptr_get_write(pte) || pte_ptr_get_execute(pte));
}

/** Helper function meant only to be used for mapping the kernel
 * window.
 *
 * Maps all pages with full RWX and supervisor perms by default.
 */
স্থির pte_t pte_next(word_t phys_addr, bool_t is_leaf)
{
    word_t ppn = (word_t)(phys_addr >> 12);

    uint8_t পঠ = is_leaf ? 1 : 0;
    uint8_t লিখ = পঠ;
    uint8_t exec = পঠ;

    ঘূৰাই pte_new(ppn,
                   0,     /* sw */
                   is_leaf ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_leaf ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_leaf ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   লিখ, /* write */
                   পঠ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_CODE শূন্য map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#যদি __riscv_xlen == 32
    paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(0));
    প্রতিজ্ঞা((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_মূল_page_Table[RISCV_GET_PT_INDEX(vaddr, 0)] = pte_next(paddr, true);
#অন্যথা
    যদি (vaddr >= KDEV_BASE) {
        /* Map devices in level 1 page table */
        paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(1));
        প্রতিজ্ঞা((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_INDEX(vaddr, 1)] = pte_next(paddr, true);
    } অন্যথা {
        paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(0));
        প্রতিজ্ঞা((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_মূল_page_Table[RISCV_GET_PT_INDEX(vaddr, 0)] = pte_next(paddr, true);
    }
#endif
}

BOOT_CODE VISIBLE শূন্য map_kernel_window(শূন্য)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    প্রতিজ্ঞা(CONFIG_PT_LEVELS > 1 && CONFIG_PT_LEVELS <= 4);
    /* Kernel image finishes before KDEV_BASE */
    প্রতিজ্ঞা(KERNEL_ELF_শীৰ্ষ <= KDEV_BASE);

    /* kernel window starts at PPTR_BASE */
    word_t pptr = PPTR_BASE;

    /* first we map in memory from PADDR_BASE */
    word_t paddr = PADDR_BASE;
    যেতিয়ালৈ (pptr < PPTR_শীৰ্ষ) {
        প্রতিজ্ঞা(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_BITS(0)));
        প্রতিজ্ঞা(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_BITS(0)));

        kernel_মূল_page_Table[RISCV_GET_PT_INDEX(pptr, 0)] = pte_next(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    প্রতিজ্ঞা(pptr == PPTR_শীৰ্ষ);
    pptr = ROUND_DOWN(KERNEL_ELF_BASE, RISCV_GET_LVL_PGSIZE_BITS(0));
    paddr = ROUND_DOWN(KERNEL_ELF_PADDR_BASE, RISCV_GET_LVL_PGSIZE_BITS(0));

#যদি __riscv_xlen == 32
    kernel_মূল_page_Table[RISCV_GET_PT_INDEX(pptr, 0)] = pte_next(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#ifdef CONFIG_KERNEL_LOG_BUFFER
    kernel_মূল_page_Table[RISCV_GET_PT_INDEX(KS_LOG_PPTR, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_log_buffer_pt), false);
#endif
#অন্যথা
    word_t index = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_মূল_page_Table[RISCV_GET_PT_INDEX(KERNEL_ELF_PADDR_BASE + PPTR_BASE_OFFSET, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_pt), false);
    kernel_মূল_page_Table[RISCV_GET_PT_INDEX(pptr, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_pt), false);
    যেতিয়ালৈ (pptr < PPTR_শীৰ্ষ + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[index] = pte_next(paddr, true);
        index++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_মূল_page_Table[RISCV_GET_PT_INDEX(KDEV_BASE, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_dev_pt), false);
#endif

    /* There should be free space where we put device mapping */
    প্রতিজ্ঞা(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_CODE শূন্য map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    lookupPTSlot_ret_t pt_ret;
    pte_t *targetSlot;
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);
    pte_t *lvl1pt = PTE_PTR(pptr_of_cap(vspace_cap));

    /* pt to be mapped */
    pte_t *pt   = PTE_PTR(pptr_of_cap(pt_cap));

    /* Get PT slot to install the address in */
    pt_ret = lookupPTSlot(lvl1pt, vptr);

    targetSlot = pt_ret.ptSlot;

    *targetSlot = pte_new(
                      (addrFromPPtr(pt) >> seL4_PageBits),
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

BOOT_CODE শূন্য map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_of_cap(vspace_cap));
    pte_t *frame_pptr   = PTE_PTR(pptr_of_cap(frame_cap));
    vptr_t frame_vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, frame_vptr);
    প্রতিজ্ঞা(lu_ret.pt_Bits_বাম == seL4_PageBits);

    pte_t *targetSlot = lu_ret.ptSlot;

    *targetSlot = pte_new(
                      (pptr_to_paddr(frame_pptr) >> seL4_PageBits),
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

BOOT_CODE cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_frame_cap_new(
                    asidInvalid,                     /* capFMappedASID       */
                    pptr,                            /* capFBasePtr          */
                    0,                               /* capFSize             */
                    0,                               /* capFVMRights         */
                    0,
                    0                                /* capFMappedAddress    */
                );

    ঘূৰাই cap;
}

/* Create a page table for the initial thread */
স্থির BOOT_CODE cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    ঘূৰাই cap;
}

BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    word_t n = 0;
    বাবে (পূর্ণাঙ্ক i = 0; i < CONFIG_PT_LEVELS - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_BITS(i));
    }
    ঘূৰাই n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t create_it_address_space(cap_t মূল_cnode_cap, v_region_t it_v_reg)
{
    cap_t      lvl1pt_cap;
    vptr_t     pt_vptr;

    copyGlobalMappings(PTE_PTR(rootserver.vspace));

    lvl1pt_cap =
        cap_page_table_cap_new(
            IT_ASID,               /* capPTMappedASID    */
            (word_t) rootserver.vspace,  /* capPTBasePtr       */
            1,                     /* capPTIsMapped      */
            (word_t) rootserver.vspace   /* capPTMappedAddress */
        );

    seL4_SlotPos slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_of_cap(মূল_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    বাবে (পূর্ণাঙ্ক i = 0; i < CONFIG_PT_LEVELS - 1; i++) {

        বাবে (pt_vptr = ROUND_DOWN(it_v_reg.start, RISCV_GET_LVL_PGSIZE_BITS(i));
             pt_vptr < it_v_reg.end;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            যদি (!provide_cap(মূল_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                ঘূৰাই cap_null_cap_new();
            }
        }

    }

    seL4_SlotPos slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    ঘূৰাই lvl1pt_cap;
}

BOOT_CODE শূন্য activate_kernel_vspace(শূন্য)
{
    set_VSpace_মূল(kpptr_to_paddr(&kernel_মূল_page_Table), 0);
}

BOOT_CODE শূন্য write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[ASID_নিচেৰ(IT_ASID)] = PTE_PTR(pptr_of_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_উপৰেৰ(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

স্থির findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_মূল;

    poolPtr = riscvKSASIDTable[ASID_উপৰেৰ(asid)];
    যদি (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_মূল_new();

        ret.vspace_মূল = রিক্ত;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ঘূৰাই ret;
    }

    vspace_মূল = poolPtr->array[ASID_নিচেৰ(asid)];
    যদি (!vspace_মূল) {
        current_lookup_fault = lookup_fault_invalid_মূল_new();

        ret.vspace_মূল = রিক্ত;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ঘূৰাই ret;
    }

    ret.vspace_মূল = vspace_মূল;
    ret.status = EXCEPTION_NONE;
    ঘূৰাই ret;
}

শূন্য copyGlobalMappings(pte_t *newLvl1pt)
{
    unsigned long i;
    pte_t *global_kernel_vspace = kernel_মূল_page_Table;

    বাবে (i = RISCV_GET_PT_INDEX(PPTR_BASE, 0); i < BIT(PT_INDEX_BITS); i++) {
        newLvl1pt[i] = global_kernel_vspace[i];
    }
}

word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_bufferPtr;
    cap_t bufferCap;
    vm_rights_t vm_rights;

    w_bufferPtr = thread->tcbIPCBuffer;
    bufferCap = TCB_PTR_CTE_PTR(thread, tcbBuffer)->cap;

    যদি (unlikely(cap_get_capType(bufferCap) != cap_frame_cap)) {
        ঘূৰাই রিক্ত;
    }
    যদি (unlikely(cap_frame_cap_get_capFIsDevice(bufferCap))) {
        ঘূৰাই রিক্ত;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(bufferCap);
    যদি (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr, pageBits;

        basePtr = cap_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(cap_frame_cap_get_capFSize(bufferCap));
        ঘূৰাই (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } অন্যথা {
        ঘূৰাই রিক্ত;
    }
}

স্থির অন্তঃস্থ pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    ঘূৰাই PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << seL4_PageTableBits));
}

lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    word_t level = CONFIG_PT_LEVELS - 1;
    pte_t *pt = lvl1pt;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_Bits_বাম = PT_INDEX_BITS * level + seL4_PageBits;
    ret.ptSlot = pt + ((vptr >> ret.pt_Bits_বাম) & MASK(PT_INDEX_BITS));

    যেতিয়ালৈ (isPTEPageTable(ret.ptSlot) && likely(0 < level)) {
        level--;
        ret.pt_Bits_বাম -= PT_INDEX_BITS;
        pt = getPPtrFromHWPTE(ret.ptSlot);
        ret.ptSlot = pt + ((vptr >> ret.pt_Bits_বাম) & MASK(PT_INDEX_BITS));
    }

    ঘূৰাই ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    ৱিকল্প (vm_faultType) {
    পক্ষ RISCVLoadPageFault:
    পক্ষ RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        ঘূৰাই EXCEPTION_FAULT;
    পক্ষ RISCVStorePageFault:
    পক্ষ RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        ঘূৰাই EXCEPTION_FAULT;
    পক্ষ RISCVInstructionPageFault:
    পক্ষ RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        ঘূৰাই EXCEPTION_FAULT;

    স্ৱভাৱ:
        fail("Invalid VM fault type");
    }
}

শূন্য deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    প্রতিজ্ঞা(IS_ALIGNED(asid_base, asid_নিচেৰ_Bits));

    যদি (riscvKSASIDTable[ASID_উপৰেৰ(asid_base)] == pool) {
        riscvKSASIDTable[ASID_উপৰেৰ(asid_base)] = রিক্ত;
        setVMRoot(গ্ৰন্থি_STATE(ksCurThread));
    }
}

স্থির exception_t performASIDControlInvocation(শূন্য *frame, cte_t *slot, cte_t *parent, asid_t asid_base)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, BIT(pageBitsForSize(RISCV_4K_Page)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ঢোকাও(
        cap_asid_pool_cap_new(
            asid_base,          /* capASIDBase  */
            WORD_REF(frame)     /* capASIDPool  */
        ),
        parent,
        slot
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    প্রতিজ্ঞা((asid_base & MASK(asid_নিচেৰ_Bits)) == 0);
    riscvKSASIDTable[ASID_উপৰেৰ(asid_base)] = (asid_pool_t *)frame;

    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspaceCapSlot)
{
    cap_t cap = vspaceCapSlot->cap;
    pte_t *regionBase = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, 0);
    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    vspaceCapSlot->cap = cap;

    copyGlobalMappings(regionBase);

    poolPtr->array[ASID_নিচেৰ(asid)] = regionBase;

    ঘূৰাই EXCEPTION_NONE;
}

শূন্য deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_উপৰেৰ(asid)];
    যদি (poolPtr != রিক্ত && poolPtr->array[ASID_নিচেৰ(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_নিচেৰ(asid)] = রিক্ত;
        setVMRoot(গ্ৰন্থি_STATE(ksCurThread));
    }
}

শূন্য unmapPageTable(asid_t asid, vptr_t vptr, pte_t *target_pt)
{
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    যদি (unlikely(find_ret.status != EXCEPTION_NONE)) {
        /* nothing to do */
        ঘূৰাই;
    }
    /* We won't ever unmap a top level page table */
    প্রতিজ্ঞা(find_ret.vspace_মূল != target_pt);
    pte_t *ptSlot = রিক্ত;
    pte_t *pt = find_ret.vspace_মূল;

    বাবে (word_t i = 0; i < CONFIG_PT_LEVELS - 1 && pt != target_pt; i++) {
        ptSlot = pt + RISCV_GET_PT_INDEX(vptr, i);
        যদি (unlikely(!isPTEPageTable(ptSlot))) {
            /* couldn't find it */
            ঘূৰাই;
        }
        pt = getPPtrFromHWPTE(ptSlot);
    }

    যদি (pt != target_pt) {
        /* didn't find it */
        ঘূৰাই;
    }
    /* If we found a pt then ptSlot won't be null */
    প্রতিজ্ঞা(ptSlot != রিক্ত);
    *ptSlot = pte_new(
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

স্থির pte_t pte_pte_invalid_new(শূন্য)
{
    ঘূৰাই (pte_t) {
        0
    };
}

শূন্য unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;

    find_ret = findVSpaceForASID(asid);
    যদি (find_ret.status != EXCEPTION_NONE) {
        ঘূৰাই;
    }

    lu_ret = lookupPTSlot(find_ret.vspace_মূল, vptr);
    যদি (unlikely(lu_ret.pt_Bits_বাম != pageBitsForSize(page_size))) {
        ঘূৰাই;
    }
    যদি (!pte_ptr_get_valid(lu_ret.ptSlot) || isPTEPageTable(lu_ret.ptSlot)
        || (pte_ptr_get_ppn(lu_ret.ptSlot) << seL4_PageBits) != pptr_to_paddr((শূন্য *)pptr)) {
        ঘূৰাই;
    }

    lu_ret.ptSlot[0] = pte_pte_invalid_new();
    sfence();
}

শূন্য setVMRoot(tcb_t *tcb)
{
    cap_t thread_মূল;
    asid_t asid;
    pte_t *lvl1pt;
    findVSpaceForASID_ret_t  find_ret;

    thread_মূল = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    যদি (cap_get_capType(thread_মূল) != cap_page_table_cap) {
        set_VSpace_মূল(kpptr_to_paddr(&kernel_মূল_page_Table), 0);
        ঘূৰাই;
    }

    lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(thread_মূল));

    asid = cap_page_table_cap_get_capPTMappedASID(thread_মূল);
    find_ret = findVSpaceForASID(asid);
    যদি (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.vspace_মূল != lvl1pt)) {
        set_VSpace_মূল(kpptr_to_paddr(&kernel_মূল_page_Table), 0);
        ঘূৰাই;
    }

    set_VSpace_মূল(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_মূল(cap_t cap)
{
    ঘূৰাই (cap_get_capType(cap) == cap_page_table_cap &&
            cap_page_table_cap_get_capPTIsMapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    যদি (unlikely(cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    যদি (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    যদি (unlikely(!IS_ALIGNED(vptr, seL4_IPCBufferSizeBits))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (পূর্ণাঙ্ক)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    ঘূৰাই EXCEPTION_NONE;
}

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    যদি (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        ঘূৰাই VMReadOnly;
    }
    যদি (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        যদি (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            ঘূৰাই VMReadOnly;
        } অন্যথা {
            ঘূৰাই VMReadWrite;
        }
    }
    যদি (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    ঘূৰাই VMKernelOnly;
}

/* The rest of the file implements the RISCV object invocations */

স্থির pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    word_t লিখ = RISCVGetWriteFromVMRights(vm_rights);
    word_t পঠ = RISCVGetReadFromVMRights(vm_rights);
    যদি (unlikely(!পঠ && !লিখ && !executable)) {
        ঘূৰাই pte_pte_invalid_new();
    } অন্যথা {
        ঘূৰাই pte_new(
                   paddr >> seL4_PageBits,
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

স্থির অন্তঃস্থ bool_t CONST checkVPAlignment(vm_page_size_t sz, word_t w)
{
    ঘূৰাই (w & MASK(pageBitsForSize(sz))) == 0;
}

স্থির exception_t decodeRISCVPageTableInvocation(word_t label, word_t length,
                                                  cte_t *cte, cap_t cap, word_t *buffer)
{
    যদি (label == RISCVPageTableUnmap) {
        যদি (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = seL4_RevokeFirst;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        যদি (likely(cap_page_table_cap_get_capPTIsMapped(cap))) {
            asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
            findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
            pte_t *pte = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
            যদি (unlikely(find_ret.status == EXCEPTION_NONE &&
                         find_ret.vspace_মূল == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = seL4_RevokeFirst;
                ঘূৰাই EXCEPTION_SYSCALL_ERROR;
            }
        }

        setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
        ঘূৰাই performPageTableInvocationUnmap(cap, cte);
    }

    যদি (unlikely((label != RISCVPageTableMap))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    যদি (unlikely(length < 2 || current_extra_caps.excaprefs[0] == রিক্ত)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = seL4_TruncatedMessage;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    যদি (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 0;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    word_t vaddr = getSyscallArg(0, buffer);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    যদি (unlikely(cap_get_capType(lvl1ptCap) != cap_page_table_cap ||
                 cap_page_table_cap_get_capPTIsMapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;

        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(lvl1ptCap));
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(lvl1ptCap);

    যদি (unlikely(vaddr >= USER_শীৰ্ষ)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 0;

        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    যদি (unlikely(find_ret.status != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    যদি (unlikely(find_ret.vspace_মূল != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_সংখ্যা = 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    যদি (lu_ret.pt_Bits_বাম == seL4_PageBits || pte_ptr_get_valid(lu_ret.ptSlot)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = seL4_DeleteFirst;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    /* Get the slot to install the PT in */
    pte_t *ptSlot = lu_ret.ptSlot;

    paddr_t paddr = addrFromPPtr(
                        PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
    pte_t pte = pte_new((paddr >> seL4_PageBits),
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

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, (vaddr & ~MASK(lu_ret.pt_Bits_বাম)));

    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
    ঘূৰাই performPageTableInvocationMap(cap, cte, pte, ptSlot);
}

স্থির exception_t decodeRISCVFrameInvocation(word_t label, word_t length,
                                              cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    ৱিকল্প (label) {
    পক্ষ RISCVPageMap: {
        যদি (unlikely(length < 3 || current_extra_caps.excaprefs[0] == রিক্ত)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        word_t vaddr = getSyscallArg(0, buffer);
        word_t w_rightsMask = getSyscallArg(1, buffer);
        vm_attributes_t attr = vmAttributesFromWord(getSyscallArg(2, buffer));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_page_size_t frameSize = cap_frame_cap_get_capFSize(cap);
        vm_rights_t capVMRights = cap_frame_cap_get_capFVMRights(cap);

        যদি (unlikely(cap_get_capType(lvl1ptCap) != cap_page_table_cap ||
                     !cap_page_table_cap_get_capPTIsMapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 1;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(lvl1ptCap));
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(lvl1ptCap);

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        যদি (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        যদি (unlikely(find_ret.vspace_মূল != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 1;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        word_t vtop = vaddr + BIT(pageBitsForSize(frameSize)) - 1;
        যদি (unlikely(vtop >= USER_শীৰ্ষ)) {
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 0;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }
        যদি (unlikely(!checkVPAlignment(frameSize, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        যদি (unlikely(lu_ret.pt_Bits_বাম != pageBitsForSize(frameSize))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_Bits_বাম);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        asid_t frame_asid = cap_frame_cap_get_capFMappedASID(cap);
        যদি (unlikely(frame_asid != asidInvalid)) {
            /* this frame is already mapped */
            যদি (frame_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_সংখ্যা = 1;
                ঘূৰাই EXCEPTION_SYSCALL_ERROR;
            }
            word_t mapped_vaddr = cap_frame_cap_get_capFMappedAddress(cap);
            যদি (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_সংখ্যা = 0;
                ঘূৰাই EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            যদি (unlikely(isPTEPageTable(lu_ret.ptSlot))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = seL4_DeleteFirst;
                ঘূৰাই EXCEPTION_SYSCALL_ERROR;
            }
        } অন্যথা {
            /* check this vaddr isn't already mapped */
            যদি (unlikely(pte_ptr_get_valid(lu_ret.ptSlot))) {
                userError("Virtual address (0x%"SEL4_PRIx_word") already mapped", vaddr);
                current_syscall_error.type = seL4_DeleteFirst;
                ঘূৰাই EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = maskVMRights(capVMRights, rightsFromWord(w_rightsMask));
        paddr_t frame_paddr = addrFromPPtr((শূন্য *) cap_frame_cap_get_capFBasePtr(cap));
        cap = cap_frame_cap_set_capFMappedASID(cap, asid);
        cap = cap_frame_cap_set_capFMappedAddress(cap,  vaddr);

        bool_t executable = !vm_attributes_get_riscvExecuteNever(attr);
        pte_t pte = makeUserPTE(frame_paddr, executable, vmRights);
        setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
        ঘূৰাই performPageInvocationMapPTE(cap, cte, pte, lu_ret.ptSlot);
    }

    পক্ষ RISCVPageUnmap: {
        setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
        ঘূৰাই performPageInvocationUnmap(cap, cte);
    }

    পক্ষ RISCVPageGetAddress: {

        /* Check that there are enough message registers */
        প্রতিজ্ঞা(n_বাৰ্তা_Registers >= 1);

        setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
        ঘূৰাই performPageGetAddress((শূন্য *)cap_frame_cap_get_capFBasePtr(cap), call);
    }

    স্ৱভাৱ:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(word_t label, word_t length, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    ৱিকল্প (cap_get_capType(cap)) {

    পক্ষ cap_page_table_cap:
        ঘূৰাই decodeRISCVPageTableInvocation(label, length, cte, cap, buffer);

    পক্ষ cap_frame_cap:
        ঘূৰাই decodeRISCVFrameInvocation(label, length, cte, cap, call, buffer);

    পক্ষ cap_asid_control_cap: {
        word_t     i;
        asid_t           asid_base;
        word_t           index;
        word_t           depth;
        cap_t            untyped;
        cap_t            মূল;
        cte_t           *parentSlot;
        cte_t           *destSlot;
        lookupSlot_ret_t lu_ret;
        শূন্য            *frame;
        exception_t      status;

        যদি (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        যদি (length < 2 || current_extra_caps.excaprefs[0] == রিক্ত
            || current_extra_caps.excaprefs[1] == রিক্ত) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        index = getSyscallArg(0, buffer);
        depth = getSyscallArg(1, buffer);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        মূল = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        বাবে (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        যদি (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_নিচেৰ_Bits;

        যদি (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_capBlockSize(untyped) != seL4_ASIDPoolBits ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 1;

            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        যদি (status != EXCEPTION_NONE) {
            ঘূৰাই status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(মূল, index, depth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            ঘূৰাই lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        যদি (status != EXCEPTION_NONE) {
            ঘূৰাই status;
        }

        setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
        ঘূৰাই performASIDControlInvocation(frame, destSlot, parentSlot, asid_base);
    }

    পক্ষ cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t       asid;

        যদি (label != RISCVASIDPoolAssign) {
            current_syscall_error.type = seL4_IllegalOperation;

            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }
        যদি (current_extra_caps.excaprefs[0] == রিক্ত) {
            current_syscall_error.type = seL4_TruncatedMessage;

            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        যদি (unlikely(
                cap_get_capType(vspaceCap) != cap_page_table_cap ||
                cap_page_table_cap_get_capPTIsMapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 1;

            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_উপৰেৰ(cap_asid_pool_cap_get_capASIDBase(cap))];
        যদি (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_মূল_new();
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        যদি (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_সংখ্যা = 0;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        বাবে (i = 0; i < BIT(asid_নিচেৰ_Bits) && (asid + i == 0 || pool->array[i]); i++);

        যদি (i == BIT(asid_নিচেৰ_Bits)) {
            current_syscall_error.type = seL4_DeleteFirst;

            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
        ঘূৰাই performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }
    স্ৱভাৱ:
        fail("Invalid arch cap type");
    }
}

exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                          pte_t pte, pte_t *ptSlot)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    sfence();

    ঘূৰাই EXCEPTION_NONE;
}

exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    যদি (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt
        );
        clearMemory((শূন্য *)pt, seL4_PageTableBits);
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t performPageGetAddress(শূন্য *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = গ্ৰন্থি_STATE(ksCurThread);
    যদি (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned পূর্ণাঙ্ক length = setMR(thread, ipcBuffer, 0, capFBasePtr);
        setRegister(thread, বাৰ্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Running);
    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t updatePTE(pte_t pte, pte_t *base)
{
    *base = pte;
    sfence();
    ঘূৰাই EXCEPTION_NONE;
}

exception_t performPageInvocationMapPTE(cap_t cap, cte_t *ctSlot,
                                        pte_t pte, pte_t *base)
{
    ctSlot->cap = cap;
    ঘূৰাই updatePTE(pte, base);
}

exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    যদি (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        unmapPage(cap_frame_cap_get_capFSize(cap),
                  cap_frame_cap_get_capFMappedASID(cap),
                  cap_frame_cap_get_capFMappedAddress(cap),
                  cap_frame_cap_get_capFBasePtr(cap)
                 );
    }

    cap_t slotCap = ctSlot->cap;
    slotCap = cap_frame_cap_set_capFMappedAddress(slotCap, 0);
    slotCap = cap_frame_cap_set_capFMappedASID(slotCap, asidInvalid);
    ctSlot->cap = slotCap;

    ঘূৰাই EXCEPTION_NONE;
}

#ifdef CONFIG_PRINTING
শূন্য Arch_user_স্তূপ_Trace(tcb_t *tptr)
{
    cap_t thread_মূল = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    যদি (!is_Valid_VTable_মূল(thread_মূল)) {
        printf("Invalid vspace\n");
        ঘূৰাই;
    }

    word_t sp = getRegister(tptr, SP);
    যদি (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        printf("SP %p not aligned", (শূন্য *) sp);
        ঘূৰাই;
    }

    pte_t *vspace_মূল = PTE_PTR(pptr_of_cap(thread_মূল));
    বাবে (পূর্ণাঙ্ক i = 0; i < CONFIG_USER_স্তূপ_TRACE_LENGTH; i++) {
        word_t address = sp + (i * পরিমাণ(word_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_মূল, address);
        যদি (address < USER_শীৰ্ষ && pte_ptr_get_valid(ret.ptSlot) && !isPTEPageTable(ret.ptSlot)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.ptSlot));
            word_t *মান = (word_t *)((word_t)pptr + (address & MASK(ret.pt_Bits_বাম)));
            printf("0x%lx: 0x%lx\n", (long) address, (long) *মান);
        } অন্যথা {
            printf("0x%lx: INVALID\n", (long) address);
        }
    }
}
#endif

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t benchmark_arch_map_logBuffer(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_size_t frameSize;
    pptr_t  frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(গ্ৰন্থি_STATE(ksCurThread), frame_cptr);

    যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    যদি (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    frameSize = cap_frame_cap_get_capFSize(lu_ret.cap);

    যদি (frameSize != RISCV_Mega_Page) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((শূন্য *) frame_pptr);

#যদি __riscv_xlen == 32
    paddr_t physical_address = ksUserLogBuffer;
    বাবে (word_t i = 0; i < BIT(PT_INDEX_BITS); i += 1) {
        kernel_image_level1_log_buffer_pt[i] = pte_next(physical_address, true);
        physical_address += BIT(PAGE_BITS);
    }
    প্রতিজ্ঞা(physical_address - ksUserLogBuffer == BIT(seL4_LargePageBits));
#অন্যথা
    kernel_image_level1_dev_pt[RISCV_GET_PT_INDEX(KS_LOG_PPTR, 1)] = pte_next(ksUserLogBuffer, true);
#endif

    sfence();

    ঘূৰাই EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */
