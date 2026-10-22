/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <benchmark/benchmark.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <kernel/boot.h>
#અન્તર્ભાવ <kernel/cspace.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <object/tcb.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <model/preemption.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <object/cnode.h>
#અન્તર્ભાવ <object/untyped.h>
#અન્તર્ભાવ <arch/api/invocation.h>
#અન્તર્ભાવ <arch/kernel/vspace.h>
#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <plat/machine/hardware.h>
#અન્તર્ભાવ <kernel/stack.h>
#અન્તર્ભાવ <util.h>

સંરચના resolve_ret {
    paddr_t frameBase;
    vm_page_size_t frameSize;
    bool_t valid;
};
પ્રકારવ્યાખ્યા સંરચના resolve_ret resolve_ret_t;

સ્થિર exception_t performPageGetAddress(શૂન્ય *vbase_ptr, bool_t call);

સ્થિર word_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    પાછા vm_rights == VMReadWrite;
}

સ્થિર અંતઃરેખ word_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported.
     * Kernel-only conveys no user rights. */
    પાછા vm_rights != VMKernelOnly;
}

સ્થિર અંતઃરેખ bool_t isPTEPageTable(pte_t *pte)
{
    પાછા pte_ptr_get_valid(pte) &&
           !(pte_ptr_get_read(pte) || pte_ptr_get_write(pte) || pte_ptr_get_execute(pte));
}

/** Helper function meant only to be used for mapping the kernel
 * window.
 *
 * Maps all pages with full RWX and supervisor perms by default.
 */
સ્થિર pte_t pte_next(word_t phys_addr, bool_t is_leaf)
{
    word_t ppn = (word_t)(phys_addr >> 12);

    uint8_t પઠ = is_leaf ? 1 : 0;
    uint8_t લિખ = પઠ;
    uint8_t exec = પઠ;

    પાછા pte_new(ppn,
                   0,     /* sw */
                   is_leaf ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_leaf ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_leaf ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   લિખ, /* write */
                   પઠ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_CODE શૂન્ય map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#જો __riscv_xlen == 32
    paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(0));
    પ્રતિજ્ઞા((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_મૂળ_page_Table[RISCV_GET_PT_INDEX(vaddr, 0)] = pte_next(paddr, true);
#નહીંતર
    જો (vaddr >= KDEV_BASE) {
        /* Map devices in level 1 page table */
        paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(1));
        પ્રતિજ્ઞા((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_INDEX(vaddr, 1)] = pte_next(paddr, true);
    } નહીંતર {
        paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(0));
        પ્રતિજ્ઞા((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_મૂળ_page_Table[RISCV_GET_PT_INDEX(vaddr, 0)] = pte_next(paddr, true);
    }
#endif
}

BOOT_CODE VISIBLE શૂન્ય map_kernel_window(શૂન્ય)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    પ્રતિજ્ઞા(CONFIG_PT_LEVELS > 1 && CONFIG_PT_LEVELS <= 4);
    /* Kernel image finishes before KDEV_BASE */
    પ્રતિજ્ઞા(KERNEL_ELF_ટોચ <= KDEV_BASE);

    /* kernel window starts at PPTR_BASE */
    word_t pptr = PPTR_BASE;

    /* first we map in memory from PADDR_BASE */
    word_t paddr = PADDR_BASE;
    જ્યારે (pptr < PPTR_ટોચ) {
        પ્રતિજ્ઞા(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_BITS(0)));
        પ્રતિજ્ઞા(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_BITS(0)));

        kernel_મૂળ_page_Table[RISCV_GET_PT_INDEX(pptr, 0)] = pte_next(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    પ્રતિજ્ઞા(pptr == PPTR_ટોચ);
    pptr = ROUND_DOWN(KERNEL_ELF_BASE, RISCV_GET_LVL_PGSIZE_BITS(0));
    paddr = ROUND_DOWN(KERNEL_ELF_PADDR_BASE, RISCV_GET_LVL_PGSIZE_BITS(0));

#જો __riscv_xlen == 32
    kernel_મૂળ_page_Table[RISCV_GET_PT_INDEX(pptr, 0)] = pte_next(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#ifdef CONFIG_KERNEL_LOG_BUFFER
    kernel_મૂળ_page_Table[RISCV_GET_PT_INDEX(KS_LOG_PPTR, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_log_buffer_pt), false);
#endif
#નહીંતર
    word_t index = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_મૂળ_page_Table[RISCV_GET_PT_INDEX(KERNEL_ELF_PADDR_BASE + PPTR_BASE_OFFSET, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_pt), false);
    kernel_મૂળ_page_Table[RISCV_GET_PT_INDEX(pptr, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_pt), false);
    જ્યારે (pptr < PPTR_ટોચ + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[index] = pte_next(paddr, true);
        index++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_મૂળ_page_Table[RISCV_GET_PT_INDEX(KDEV_BASE, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_dev_pt), false);
#endif

    /* There should be free space where we put device mapping */
    પ્રતિજ્ઞા(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_CODE શૂન્ય map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
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

BOOT_CODE શૂન્ય map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_of_cap(vspace_cap));
    pte_t *frame_pptr   = PTE_PTR(pptr_of_cap(frame_cap));
    vptr_t frame_vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, frame_vptr);
    પ્રતિજ્ઞા(lu_ret.pt_Bits_ડાબું == seL4_PageBits);

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

    પાછા cap;
}

/* Create a page table for the initial thread */
સ્થિર BOOT_CODE cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    પાછા cap;
}

BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    word_t n = 0;
    માટે (પૂર્ણાંક i = 0; i < CONFIG_PT_LEVELS - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_BITS(i));
    }
    પાછા n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t create_it_address_space(cap_t મૂળ_cnode_cap, v_region_t it_v_reg)
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
    write_slot(SLOT_PTR(pptr_of_cap(મૂળ_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    માટે (પૂર્ણાંક i = 0; i < CONFIG_PT_LEVELS - 1; i++) {

        માટે (pt_vptr = ROUND_DOWN(it_v_reg.start, RISCV_GET_LVL_PGSIZE_BITS(i));
             pt_vptr < it_v_reg.end;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            જો (!provide_cap(મૂળ_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                પાછા cap_null_cap_new();
            }
        }

    }

    seL4_SlotPos slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    પાછા lvl1pt_cap;
}

BOOT_CODE શૂન્ય activate_kernel_vspace(શૂન્ય)
{
    set_VSpace_મૂળ(kpptr_to_paddr(&kernel_મૂળ_page_Table), 0);
}

BOOT_CODE શૂન્ય write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[ASID_નીચલું(IT_ASID)] = PTE_PTR(pptr_of_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_ઉપલું(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

સ્થિર findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_મૂળ;

    poolPtr = riscvKSASIDTable[ASID_ઉપલું(asid)];
    જો (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_મૂળ_new();

        ret.vspace_મૂળ = ખાલી;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        પાછા ret;
    }

    vspace_મૂળ = poolPtr->array[ASID_નીચલું(asid)];
    જો (!vspace_મૂળ) {
        current_lookup_fault = lookup_fault_invalid_મૂળ_new();

        ret.vspace_મૂળ = ખાલી;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        પાછા ret;
    }

    ret.vspace_મૂળ = vspace_મૂળ;
    ret.status = EXCEPTION_NONE;
    પાછા ret;
}

શૂન્ય copyGlobalMappings(pte_t *newLvl1pt)
{
    unsigned long i;
    pte_t *global_kernel_vspace = kernel_મૂળ_page_Table;

    માટે (i = RISCV_GET_PT_INDEX(PPTR_BASE, 0); i < BIT(PT_INDEX_BITS); i++) {
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

    જો (unlikely(cap_get_capType(bufferCap) != cap_frame_cap)) {
        પાછા ખાલી;
    }
    જો (unlikely(cap_frame_cap_get_capFIsDevice(bufferCap))) {
        પાછા ખાલી;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(bufferCap);
    જો (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr, pageBits;

        basePtr = cap_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(cap_frame_cap_get_capFSize(bufferCap));
        પાછા (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } નહીંતર {
        પાછા ખાલી;
    }
}

સ્થિર અંતઃરેખ pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    પાછા PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << seL4_PageTableBits));
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
    ret.pt_Bits_ડાબું = PT_INDEX_BITS * level + seL4_PageBits;
    ret.ptSlot = pt + ((vptr >> ret.pt_Bits_ડાબું) & MASK(PT_INDEX_BITS));

    જ્યારે (isPTEPageTable(ret.ptSlot) && likely(0 < level)) {
        level--;
        ret.pt_Bits_ડાબું -= PT_INDEX_BITS;
        pt = getPPtrFromHWPTE(ret.ptSlot);
        ret.ptSlot = pt + ((vptr >> ret.pt_Bits_ડાબું) & MASK(PT_INDEX_BITS));
    }

    પાછા ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    પસંદગી (vm_faultType) {
    સ્થિતિ RISCVLoadPageFault:
    સ્થિતિ RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        પાછા EXCEPTION_FAULT;
    સ્થિતિ RISCVStorePageFault:
    સ્થિતિ RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        પાછા EXCEPTION_FAULT;
    સ્થિતિ RISCVInstructionPageFault:
    સ્થિતિ RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        પાછા EXCEPTION_FAULT;

    મૂળભૂત:
        fail("Invalid VM fault type");
    }
}

શૂન્ય deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    પ્રતિજ્ઞા(IS_ALIGNED(asid_base, asid_નીચલું_Bits));

    જો (riscvKSASIDTable[ASID_ઉપલું(asid_base)] == pool) {
        riscvKSASIDTable[ASID_ઉપલું(asid_base)] = ખાલી;
        setVMRoot(ગાંઠ_STATE(ksCurThread));
    }
}

સ્થિર exception_t performASIDControlInvocation(શૂન્ય *frame, cte_t *slot, cte_t *parent, asid_t asid_base)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, BIT(pageBitsForSize(RISCV_4K_Page)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ઉમેરો(
        cap_asid_pool_cap_new(
            asid_base,          /* capASIDBase  */
            WORD_REF(frame)     /* capASIDPool  */
        ),
        parent,
        slot
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    પ્રતિજ્ઞા((asid_base & MASK(asid_નીચલું_Bits)) == 0);
    riscvKSASIDTable[ASID_ઉપલું(asid_base)] = (asid_pool_t *)frame;

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspaceCapSlot)
{
    cap_t cap = vspaceCapSlot->cap;
    pte_t *regionBase = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, 0);
    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    vspaceCapSlot->cap = cap;

    copyGlobalMappings(regionBase);

    poolPtr->array[ASID_નીચલું(asid)] = regionBase;

    પાછા EXCEPTION_NONE;
}

શૂન્ય deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_ઉપલું(asid)];
    જો (poolPtr != ખાલી && poolPtr->array[ASID_નીચલું(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_નીચલું(asid)] = ખાલી;
        setVMRoot(ગાંઠ_STATE(ksCurThread));
    }
}

શૂન્ય unmapPageTable(asid_t asid, vptr_t vptr, pte_t *target_pt)
{
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
        /* nothing to do */
        પાછા;
    }
    /* We won't ever unmap a top level page table */
    પ્રતિજ્ઞા(find_ret.vspace_મૂળ != target_pt);
    pte_t *ptSlot = ખાલી;
    pte_t *pt = find_ret.vspace_મૂળ;

    માટે (word_t i = 0; i < CONFIG_PT_LEVELS - 1 && pt != target_pt; i++) {
        ptSlot = pt + RISCV_GET_PT_INDEX(vptr, i);
        જો (unlikely(!isPTEPageTable(ptSlot))) {
            /* couldn't find it */
            પાછા;
        }
        pt = getPPtrFromHWPTE(ptSlot);
    }

    જો (pt != target_pt) {
        /* didn't find it */
        પાછા;
    }
    /* If we found a pt then ptSlot won't be null */
    પ્રતિજ્ઞા(ptSlot != ખાલી);
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

સ્થિર pte_t pte_pte_invalid_new(શૂન્ય)
{
    પાછા (pte_t) {
        0
    };
}

શૂન્ય unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;

    find_ret = findVSpaceForASID(asid);
    જો (find_ret.status != EXCEPTION_NONE) {
        પાછા;
    }

    lu_ret = lookupPTSlot(find_ret.vspace_મૂળ, vptr);
    જો (unlikely(lu_ret.pt_Bits_ડાબું != pageBitsForSize(page_size))) {
        પાછા;
    }
    જો (!pte_ptr_get_valid(lu_ret.ptSlot) || isPTEPageTable(lu_ret.ptSlot)
        || (pte_ptr_get_ppn(lu_ret.ptSlot) << seL4_PageBits) != pptr_to_paddr((શૂન્ય *)pptr)) {
        પાછા;
    }

    lu_ret.ptSlot[0] = pte_pte_invalid_new();
    sfence();
}

શૂન્ય setVMRoot(tcb_t *tcb)
{
    cap_t thread_મૂળ;
    asid_t asid;
    pte_t *lvl1pt;
    findVSpaceForASID_ret_t  find_ret;

    thread_મૂળ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    જો (cap_get_capType(thread_મૂળ) != cap_page_table_cap) {
        set_VSpace_મૂળ(kpptr_to_paddr(&kernel_મૂળ_page_Table), 0);
        પાછા;
    }

    lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(thread_મૂળ));

    asid = cap_page_table_cap_get_capPTMappedASID(thread_મૂળ);
    find_ret = findVSpaceForASID(asid);
    જો (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.vspace_મૂળ != lvl1pt)) {
        set_VSpace_મૂળ(kpptr_to_paddr(&kernel_મૂળ_page_Table), 0);
        પાછા;
    }

    set_VSpace_મૂળ(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_મૂળ(cap_t cap)
{
    પાછા (cap_get_capType(cap) == cap_page_table_cap &&
            cap_page_table_cap_get_capPTIsMapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    જો (unlikely(cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(!IS_ALIGNED(vptr, seL4_IPCBufferSizeBits))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (પૂર્ણાંક)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    પાછા EXCEPTION_NONE;
}

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    જો (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        પાછા VMReadOnly;
    }
    જો (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        જો (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            પાછા VMReadOnly;
        } નહીંતર {
            પાછા VMReadWrite;
        }
    }
    જો (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    પાછા VMKernelOnly;
}

/* The rest of the file implements the RISCV object invocations */

સ્થિર pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    word_t લિખ = RISCVGetWriteFromVMRights(vm_rights);
    word_t પઠ = RISCVGetReadFromVMRights(vm_rights);
    જો (unlikely(!પઠ && !લિખ && !executable)) {
        પાછા pte_pte_invalid_new();
    } નહીંતર {
        પાછા pte_new(
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

સ્થિર અંતઃરેખ bool_t CONST checkVPAlignment(vm_page_size_t sz, word_t w)
{
    પાછા (w & MASK(pageBitsForSize(sz))) == 0;
}

સ્થિર exception_t decodeRISCVPageTableInvocation(word_t label, word_t length,
                                                  cte_t *cte, cap_t cap, word_t *buffer)
{
    જો (label == RISCVPageTableUnmap) {
        જો (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = seL4_RevokeFirst;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        જો (likely(cap_page_table_cap_get_capPTIsMapped(cap))) {
            asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
            findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
            pte_t *pte = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
            જો (unlikely(find_ret.status == EXCEPTION_NONE &&
                         find_ret.vspace_મૂળ == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = seL4_RevokeFirst;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageTableInvocationUnmap(cap, cte);
    }

    જો (unlikely((label != RISCVPageTableMap))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(length < 2 || current_extra_caps.excaprefs[0] == ખાલી)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    word_t vaddr = getSyscallArg(0, buffer);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    જો (unlikely(cap_get_capType(lvl1ptCap) != cap_page_table_cap ||
                 cap_page_table_cap_get_capPTIsMapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(lvl1ptCap));
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(lvl1ptCap);

    જો (unlikely(vaddr >= USER_ટોચ)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 0;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (unlikely(find_ret.vspace_મૂળ != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    જો (lu_ret.pt_Bits_ડાબું == seL4_PageBits || pte_ptr_get_valid(lu_ret.ptSlot)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = seL4_DeleteFirst;
        પાછા EXCEPTION_SYSCALL_ERROR;
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
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, (vaddr & ~MASK(lu_ret.pt_Bits_ડાબું)));

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા performPageTableInvocationMap(cap, cte, pte, ptSlot);
}

સ્થિર exception_t decodeRISCVFrameInvocation(word_t label, word_t length,
                                              cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    પસંદગી (label) {
    સ્થિતિ RISCVPageMap: {
        જો (unlikely(length < 3 || current_extra_caps.excaprefs[0] == ખાલી)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        word_t vaddr = getSyscallArg(0, buffer);
        word_t w_rightsMask = getSyscallArg(1, buffer);
        vm_attributes_t attr = vmAttributesFromWord(getSyscallArg(2, buffer));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_page_size_t frameSize = cap_frame_cap_get_capFSize(cap);
        vm_rights_t capVMRights = cap_frame_cap_get_capFVMRights(cap);

        જો (unlikely(cap_get_capType(lvl1ptCap) != cap_page_table_cap ||
                     !cap_page_table_cap_get_capPTIsMapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(lvl1ptCap));
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(lvl1ptCap);

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        જો (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (unlikely(find_ret.vspace_મૂળ != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        word_t vtop = vaddr + BIT(pageBitsForSize(frameSize)) - 1;
        જો (unlikely(vtop >= USER_ટોચ)) {
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        જો (unlikely(!checkVPAlignment(frameSize, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        જો (unlikely(lu_ret.pt_Bits_ડાબું != pageBitsForSize(frameSize))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_Bits_ડાબું);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        asid_t frame_asid = cap_frame_cap_get_capFMappedASID(cap);
        જો (unlikely(frame_asid != asidInvalid)) {
            /* this frame is already mapped */
            જો (frame_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_સંખ્યા = 1;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }
            word_t mapped_vaddr = cap_frame_cap_get_capFMappedAddress(cap);
            જો (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_સંખ્યા = 0;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            જો (unlikely(isPTEPageTable(lu_ret.ptSlot))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = seL4_DeleteFirst;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }
        } નહીંતર {
            /* check this vaddr isn't already mapped */
            જો (unlikely(pte_ptr_get_valid(lu_ret.ptSlot))) {
                userError("Virtual address (0x%"SEL4_PRIx_word") already mapped", vaddr);
                current_syscall_error.type = seL4_DeleteFirst;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = maskVMRights(capVMRights, rightsFromWord(w_rightsMask));
        paddr_t frame_paddr = addrFromPPtr((શૂન્ય *) cap_frame_cap_get_capFBasePtr(cap));
        cap = cap_frame_cap_set_capFMappedASID(cap, asid);
        cap = cap_frame_cap_set_capFMappedAddress(cap,  vaddr);

        bool_t executable = !vm_attributes_get_riscvExecuteNever(attr);
        pte_t pte = makeUserPTE(frame_paddr, executable, vmRights);
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageInvocationMapPTE(cap, cte, pte, lu_ret.ptSlot);
    }

    સ્થિતિ RISCVPageUnmap: {
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageInvocationUnmap(cap, cte);
    }

    સ્થિતિ RISCVPageGetAddress: {

        /* Check that there are enough message registers */
        પ્રતિજ્ઞા(n_સંદેશ_Registers >= 1);

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performPageGetAddress((શૂન્ય *)cap_frame_cap_get_capFBasePtr(cap), call);
    }

    મૂળભૂત:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(word_t label, word_t length, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    પસંદગી (cap_get_capType(cap)) {

    સ્થિતિ cap_page_table_cap:
        પાછા decodeRISCVPageTableInvocation(label, length, cte, cap, buffer);

    સ્થિતિ cap_frame_cap:
        પાછા decodeRISCVFrameInvocation(label, length, cte, cap, call, buffer);

    સ્થિતિ cap_asid_control_cap: {
        word_t     i;
        asid_t           asid_base;
        word_t           index;
        word_t           depth;
        cap_t            untyped;
        cap_t            મૂળ;
        cte_t           *parentSlot;
        cte_t           *destSlot;
        lookupSlot_ret_t lu_ret;
        શૂન્ય            *frame;
        exception_t      status;

        જો (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (length < 2 || current_extra_caps.excaprefs[0] == ખાલી
            || current_extra_caps.excaprefs[1] == ખાલી) {
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        index = getSyscallArg(0, buffer);
        depth = getSyscallArg(1, buffer);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        મૂળ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        માટે (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        જો (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_નીચલું_Bits;

        જો (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_capBlockSize(untyped) != seL4_ASIDPoolBits ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        જો (status != EXCEPTION_NONE) {
            પાછા status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(મૂળ, index, depth);
        જો (lu_ret.status != EXCEPTION_NONE) {
            પાછા lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        જો (status != EXCEPTION_NONE) {
            પાછા status;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performASIDControlInvocation(frame, destSlot, parentSlot, asid_base);
    }

    સ્થિતિ cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t       asid;

        જો (label != RISCVASIDPoolAssign) {
            current_syscall_error.type = seL4_IllegalOperation;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        જો (current_extra_caps.excaprefs[0] == ખાલી) {
            current_syscall_error.type = seL4_TruncatedMessage;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        જો (unlikely(
                cap_get_capType(vspaceCap) != cap_page_table_cap ||
                cap_page_table_cap_get_capPTIsMapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 1;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_ઉપલું(cap_asid_pool_cap_get_capASIDBase(cap))];
        જો (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_મૂળ_new();
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        માટે (i = 0; i < BIT(asid_નીચલું_Bits) && (asid + i == 0 || pool->array[i]); i++);

        જો (i == BIT(asid_નીચલું_Bits)) {
            current_syscall_error.type = seL4_DeleteFirst;

            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }
    મૂળભૂત:
        fail("Invalid arch cap type");
    }
}

exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                          pte_t pte, pte_t *ptSlot)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    sfence();

    પાછા EXCEPTION_NONE;
}

exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    જો (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt
        );
        clearMemory((શૂન્ય *)pt, seL4_PageTableBits);
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performPageGetAddress(શૂન્ય *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);
    જો (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned પૂર્ણાંક length = setMR(thread, ipcBuffer, 0, capFBasePtr);
        setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t updatePTE(pte_t pte, pte_t *base)
{
    *base = pte;
    sfence();
    પાછા EXCEPTION_NONE;
}

exception_t performPageInvocationMapPTE(cap_t cap, cte_t *ctSlot,
                                        pte_t pte, pte_t *base)
{
    ctSlot->cap = cap;
    પાછા updatePTE(pte, base);
}

exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    જો (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
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

    પાછા EXCEPTION_NONE;
}

#ifdef CONFIG_PRINTING
શૂન્ય Arch_user_ઢગલો_Trace(tcb_t *tptr)
{
    cap_t thread_મૂળ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    જો (!is_Valid_VTable_મૂળ(thread_મૂળ)) {
        printf("Invalid vspace\n");
        પાછા;
    }

    word_t sp = getRegister(tptr, SP);
    જો (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        printf("SP %p not aligned", (શૂન્ય *) sp);
        પાછા;
    }

    pte_t *vspace_મૂળ = PTE_PTR(pptr_of_cap(thread_મૂળ));
    માટે (પૂર્ણાંક i = 0; i < CONFIG_USER_ઢગલો_TRACE_LENGTH; i++) {
        word_t address = sp + (i * કદ(word_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_મૂળ, address);
        જો (address < USER_ટોચ && pte_ptr_get_valid(ret.ptSlot) && !isPTEPageTable(ret.ptSlot)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.ptSlot));
            word_t *કિંમત = (word_t *)((word_t)pptr + (address & MASK(ret.pt_Bits_ડાબું)));
            printf("0x%lx: 0x%lx\n", (long) address, (long) *કિંમત);
        } નહીંતર {
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
    lu_ret = lookupCapAndSlot(ગાંઠ_STATE(ksCurThread), frame_cptr);

    જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    frameSize = cap_frame_cap_get_capFSize(lu_ret.cap);

    જો (frameSize != RISCV_Mega_Page) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((શૂન્ય *) frame_pptr);

#જો __riscv_xlen == 32
    paddr_t physical_address = ksUserLogBuffer;
    માટે (word_t i = 0; i < BIT(PT_INDEX_BITS); i += 1) {
        kernel_image_level1_log_buffer_pt[i] = pte_next(physical_address, true);
        physical_address += BIT(PAGE_BITS);
    }
    પ્રતિજ્ઞા(physical_address - ksUserLogBuffer == BIT(seL4_LargePageBits));
#નહીંતર
    kernel_image_level1_dev_pt[RISCV_GET_PT_INDEX(KS_LOG_PPTR, 1)] = pte_next(ksUserLogBuffer, true);
#endif

    sfence();

    પાછા EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */
