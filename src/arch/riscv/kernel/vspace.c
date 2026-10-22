/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <benchmark/benchmark.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <kernel/boot.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <object/tcb.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <model/preemption.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <object/cnode.h>
#अन्तर्भाव <object/untyped.h>
#अन्तर्भाव <arch/api/invocation.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <kernel/stack.h>
#अन्तर्भाव <util.h>

संरचना resolve_ret {
    paddr_t frameBase;
    vm_page_size_t frameSize;
    bool_t valid;
};
प्रकारनिर्देश संरचना resolve_ret resolve_ret_t;

स्थिर exception_t performPageGetAddress(शून्य *vbase_ptr, bool_t call);

स्थिर word_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    फर्काऊ vm_rights == VMReadWrite;
}

स्थिर अन्तःस्थ word_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported.
     * Kernel-only conveys no user rights. */
    फर्काऊ vm_rights != VMKernelOnly;
}

स्थिर अन्तःस्थ bool_t isPTEPageTable(pte_t *pte)
{
    फर्काऊ pte_ptr_get_valid(pte) &&
           !(pte_ptr_get_read(pte) || pte_ptr_get_write(pte) || pte_ptr_get_execute(pte));
}

/** Helper function meant only to be used for mapping the kernel
 * window.
 *
 * Maps all pages with full RWX and supervisor perms by default.
 */
स्थिर pte_t pte_next(word_t phys_addr, bool_t is_leaf)
{
    word_t ppn = (word_t)(phys_addr >> 12);

    uint8_t पठ = is_leaf ? 1 : 0;
    uint8_t लिख = पठ;
    uint8_t exec = पठ;

    फर्काऊ pte_new(ppn,
                   0,     /* sw */
                   is_leaf ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_leaf ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_leaf ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   लिख, /* write */
                   पठ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_CODE शून्य map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#यदि __riscv_xlen == 32
    paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(0));
    प्रतिज्ञा((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_जरा_page_Table[RISCV_GET_PT_INDEX(vaddr, 0)] = pte_next(paddr, true);
#अन्यथा
    यदि (vaddr >= KDEV_BASE) {
        /* Map devices in level 1 page table */
        paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(1));
        प्रतिज्ञा((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_INDEX(vaddr, 1)] = pte_next(paddr, true);
    } अन्यथा {
        paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(0));
        प्रतिज्ञा((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_जरा_page_Table[RISCV_GET_PT_INDEX(vaddr, 0)] = pte_next(paddr, true);
    }
#endif
}

BOOT_CODE VISIBLE शून्य map_kernel_window(शून्य)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    प्रतिज्ञा(CONFIG_PT_LEVELS > 1 && CONFIG_PT_LEVELS <= 4);
    /* Kernel image finishes before KDEV_BASE */
    प्रतिज्ञा(KERNEL_ELF_शीर्ष <= KDEV_BASE);

    /* kernel window starts at PPTR_BASE */
    word_t pptr = PPTR_BASE;

    /* first we map in memory from PADDR_BASE */
    word_t paddr = PADDR_BASE;
    जबसम्म (pptr < PPTR_शीर्ष) {
        प्रतिज्ञा(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_BITS(0)));
        प्रतिज्ञा(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_BITS(0)));

        kernel_जरा_page_Table[RISCV_GET_PT_INDEX(pptr, 0)] = pte_next(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    प्रतिज्ञा(pptr == PPTR_शीर्ष);
    pptr = ROUND_DOWN(KERNEL_ELF_BASE, RISCV_GET_LVL_PGSIZE_BITS(0));
    paddr = ROUND_DOWN(KERNEL_ELF_PADDR_BASE, RISCV_GET_LVL_PGSIZE_BITS(0));

#यदि __riscv_xlen == 32
    kernel_जरा_page_Table[RISCV_GET_PT_INDEX(pptr, 0)] = pte_next(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#ifdef CONFIG_KERNEL_LOG_BUFFER
    kernel_जरा_page_Table[RISCV_GET_PT_INDEX(KS_LOG_PPTR, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_log_buffer_pt), false);
#endif
#अन्यथा
    word_t index = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_जरा_page_Table[RISCV_GET_PT_INDEX(KERNEL_ELF_PADDR_BASE + PPTR_BASE_OFFSET, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_pt), false);
    kernel_जरा_page_Table[RISCV_GET_PT_INDEX(pptr, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_pt), false);
    जबसम्म (pptr < PPTR_शीर्ष + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[index] = pte_next(paddr, true);
        index++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_जरा_page_Table[RISCV_GET_PT_INDEX(KDEV_BASE, 0)] =
        pte_next(kpptr_to_paddr(kernel_image_level1_dev_pt), false);
#endif

    /* There should be free space where we put device mapping */
    प्रतिज्ञा(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_CODE शून्य map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
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

BOOT_CODE शून्य map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_of_cap(vspace_cap));
    pte_t *frame_pptr   = PTE_PTR(pptr_of_cap(frame_cap));
    vptr_t frame_vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, frame_vptr);
    प्रतिज्ञा(lu_ret.pt_Bits_बायाँ == seL4_PageBits);

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

    फर्काऊ cap;
}

/* Create a page table for the initial thread */
स्थिर BOOT_CODE cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    फर्काऊ cap;
}

BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    word_t n = 0;
    लागि (पूर्णाङ्क i = 0; i < CONFIG_PT_LEVELS - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_BITS(i));
    }
    फर्काऊ n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t create_it_address_space(cap_t जरा_cnode_cap, v_region_t it_v_reg)
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
    write_slot(SLOT_PTR(pptr_of_cap(जरा_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    लागि (पूर्णाङ्क i = 0; i < CONFIG_PT_LEVELS - 1; i++) {

        लागि (pt_vptr = ROUND_DOWN(it_v_reg.start, RISCV_GET_LVL_PGSIZE_BITS(i));
             pt_vptr < it_v_reg.end;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            यदि (!provide_cap(जरा_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                फर्काऊ cap_null_cap_new();
            }
        }

    }

    seL4_SlotPos slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    फर्काऊ lvl1pt_cap;
}

BOOT_CODE शून्य activate_kernel_vspace(शून्य)
{
    set_VSpace_जरा(kpptr_to_paddr(&kernel_जरा_page_Table), 0);
}

BOOT_CODE शून्य write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[ASID_तल्लो(IT_ASID)] = PTE_PTR(pptr_of_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_माथिल्लो(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

स्थिर findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_जरा;

    poolPtr = riscvKSASIDTable[ASID_माथिल्लो(asid)];
    यदि (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_जरा_new();

        ret.vspace_जरा = रिक्त;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        फर्काऊ ret;
    }

    vspace_जरा = poolPtr->array[ASID_तल्लो(asid)];
    यदि (!vspace_जरा) {
        current_lookup_fault = lookup_fault_invalid_जरा_new();

        ret.vspace_जरा = रिक्त;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        फर्काऊ ret;
    }

    ret.vspace_जरा = vspace_जरा;
    ret.status = EXCEPTION_NONE;
    फर्काऊ ret;
}

शून्य copyGlobalMappings(pte_t *newLvl1pt)
{
    अचिन्हित दीर्घ i;
    pte_t *global_kernel_vspace = kernel_जरा_page_Table;

    लागि (i = RISCV_GET_PT_INDEX(PPTR_BASE, 0); i < BIT(PT_INDEX_BITS); i++) {
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

    यदि (unlikely(cap_get_capType(bufferCap) != cap_frame_cap)) {
        फर्काऊ रिक्त;
    }
    यदि (unlikely(cap_frame_cap_get_capFIsDevice(bufferCap))) {
        फर्काऊ रिक्त;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(bufferCap);
    यदि (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr, pageBits;

        basePtr = cap_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(cap_frame_cap_get_capFSize(bufferCap));
        फर्काऊ (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } अन्यथा {
        फर्काऊ रिक्त;
    }
}

स्थिर अन्तःस्थ pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    फर्काऊ PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << seL4_PageTableBits));
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
    ret.pt_Bits_बायाँ = PT_INDEX_BITS * level + seL4_PageBits;
    ret.ptSlot = pt + ((vptr >> ret.pt_Bits_बायाँ) & MASK(PT_INDEX_BITS));

    जबसम्म (isPTEPageTable(ret.ptSlot) && likely(0 < level)) {
        level--;
        ret.pt_Bits_बायाँ -= PT_INDEX_BITS;
        pt = getPPtrFromHWPTE(ret.ptSlot);
        ret.ptSlot = pt + ((vptr >> ret.pt_Bits_बायाँ) & MASK(PT_INDEX_BITS));
    }

    फर्काऊ ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    विकल्प (vm_faultType) {
    पक्ष RISCVLoadPageFault:
    पक्ष RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        फर्काऊ EXCEPTION_FAULT;
    पक्ष RISCVStorePageFault:
    पक्ष RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        फर्काऊ EXCEPTION_FAULT;
    पक्ष RISCVInstructionPageFault:
    पक्ष RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        फर्काऊ EXCEPTION_FAULT;

    स्वभाव:
        fail("Invalid VM fault type");
    }
}

शून्य deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    प्रतिज्ञा(IS_ALIGNED(asid_base, asid_तल्लो_Bits));

    यदि (riscvKSASIDTable[ASID_माथिल्लो(asid_base)] == pool) {
        riscvKSASIDTable[ASID_माथिल्लो(asid_base)] = रिक्त;
        setVMRoot(गाँठो_STATE(ksCurThread));
    }
}

स्थिर exception_t performASIDControlInvocation(शून्य *frame, cte_t *slot, cte_t *parent, asid_t asid_base)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, BIT(pageBitsForSize(RISCV_4K_Page)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_घुसाउ(
        cap_asid_pool_cap_new(
            asid_base,          /* capASIDBase  */
            WORD_REF(frame)     /* capASIDPool  */
        ),
        parent,
        slot
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    प्रतिज्ञा((asid_base & MASK(asid_तल्लो_Bits)) == 0);
    riscvKSASIDTable[ASID_माथिल्लो(asid_base)] = (asid_pool_t *)frame;

    फर्काऊ EXCEPTION_NONE;
}

स्थिर exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspaceCapSlot)
{
    cap_t cap = vspaceCapSlot->cap;
    pte_t *regionBase = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, 0);
    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    vspaceCapSlot->cap = cap;

    copyGlobalMappings(regionBase);

    poolPtr->array[ASID_तल्लो(asid)] = regionBase;

    फर्काऊ EXCEPTION_NONE;
}

शून्य deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_माथिल्लो(asid)];
    यदि (poolPtr != रिक्त && poolPtr->array[ASID_तल्लो(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_तल्लो(asid)] = रिक्त;
        setVMRoot(गाँठो_STATE(ksCurThread));
    }
}

शून्य unmapPageTable(asid_t asid, vptr_t vptr, pte_t *target_pt)
{
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    यदि (unlikely(find_ret.status != EXCEPTION_NONE)) {
        /* nothing to do */
        फर्काऊ;
    }
    /* We won't ever unmap a top level page table */
    प्रतिज्ञा(find_ret.vspace_जरा != target_pt);
    pte_t *ptSlot = रिक्त;
    pte_t *pt = find_ret.vspace_जरा;

    लागि (word_t i = 0; i < CONFIG_PT_LEVELS - 1 && pt != target_pt; i++) {
        ptSlot = pt + RISCV_GET_PT_INDEX(vptr, i);
        यदि (unlikely(!isPTEPageTable(ptSlot))) {
            /* couldn't find it */
            फर्काऊ;
        }
        pt = getPPtrFromHWPTE(ptSlot);
    }

    यदि (pt != target_pt) {
        /* didn't find it */
        फर्काऊ;
    }
    /* If we found a pt then ptSlot won't be null */
    प्रतिज्ञा(ptSlot != रिक्त);
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

स्थिर pte_t pte_pte_invalid_new(शून्य)
{
    फर्काऊ (pte_t) {
        0
    };
}

शून्य unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;

    find_ret = findVSpaceForASID(asid);
    यदि (find_ret.status != EXCEPTION_NONE) {
        फर्काऊ;
    }

    lu_ret = lookupPTSlot(find_ret.vspace_जरा, vptr);
    यदि (unlikely(lu_ret.pt_Bits_बायाँ != pageBitsForSize(page_size))) {
        फर्काऊ;
    }
    यदि (!pte_ptr_get_valid(lu_ret.ptSlot) || isPTEPageTable(lu_ret.ptSlot)
        || (pte_ptr_get_ppn(lu_ret.ptSlot) << seL4_PageBits) != pptr_to_paddr((शून्य *)pptr)) {
        फर्काऊ;
    }

    lu_ret.ptSlot[0] = pte_pte_invalid_new();
    sfence();
}

शून्य setVMRoot(tcb_t *tcb)
{
    cap_t thread_जरा;
    asid_t asid;
    pte_t *lvl1pt;
    findVSpaceForASID_ret_t  find_ret;

    thread_जरा = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    यदि (cap_get_capType(thread_जरा) != cap_page_table_cap) {
        set_VSpace_जरा(kpptr_to_paddr(&kernel_जरा_page_Table), 0);
        फर्काऊ;
    }

    lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(thread_जरा));

    asid = cap_page_table_cap_get_capPTMappedASID(thread_जरा);
    find_ret = findVSpaceForASID(asid);
    यदि (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.vspace_जरा != lvl1pt)) {
        set_VSpace_जरा(kpptr_to_paddr(&kernel_जरा_page_Table), 0);
        फर्काऊ;
    }

    set_VSpace_जरा(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_जरा(cap_t cap)
{
    फर्काऊ (cap_get_capType(cap) == cap_page_table_cap &&
            cap_page_table_cap_get_capPTIsMapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    यदि (unlikely(cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(!IS_ALIGNED(vptr, seL4_IPCBufferSizeBits))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (पूर्णाङ्क)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    फर्काऊ EXCEPTION_NONE;
}

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    यदि (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        फर्काऊ VMReadOnly;
    }
    यदि (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        यदि (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            फर्काऊ VMReadOnly;
        } अन्यथा {
            फर्काऊ VMReadWrite;
        }
    }
    यदि (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    फर्काऊ VMKernelOnly;
}

/* The rest of the file implements the RISCV object invocations */

स्थिर pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    word_t लिख = RISCVGetWriteFromVMRights(vm_rights);
    word_t पठ = RISCVGetReadFromVMRights(vm_rights);
    यदि (unlikely(!पठ && !लिख && !executable)) {
        फर्काऊ pte_pte_invalid_new();
    } अन्यथा {
        फर्काऊ pte_new(
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

स्थिर अन्तःस्थ bool_t CONST checkVPAlignment(vm_page_size_t sz, word_t w)
{
    फर्काऊ (w & MASK(pageBitsForSize(sz))) == 0;
}

स्थिर exception_t decodeRISCVPageTableInvocation(word_t label, word_t length,
                                                  cte_t *cte, cap_t cap, word_t *buffer)
{
    यदि (label == RISCVPageTableUnmap) {
        यदि (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = seL4_RevokeFirst;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        यदि (likely(cap_page_table_cap_get_capPTIsMapped(cap))) {
            asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
            findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
            pte_t *pte = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
            यदि (unlikely(find_ret.status == EXCEPTION_NONE &&
                         find_ret.vspace_जरा == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = seL4_RevokeFirst;
                फर्काऊ EXCEPTION_SYSCALL_ERROR;
            }
        }

        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ performPageTableInvocationUnmap(cap, cte);
    }

    यदि (unlikely((label != RISCVPageTableMap))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(length < 2 || current_extra_caps.excaprefs[0] == रिक्त)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = seL4_TruncatedMessage;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }
    यदि (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    word_t vaddr = getSyscallArg(0, buffer);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    यदि (unlikely(cap_get_capType(lvl1ptCap) != cap_page_table_cap ||
                 cap_page_table_cap_get_capPTIsMapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(lvl1ptCap));
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(lvl1ptCap);

    यदि (unlikely(vaddr >= USER_शीर्ष)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 0;

        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    यदि (unlikely(find_ret.status != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(find_ret.vspace_जरा != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    यदि (lu_ret.pt_Bits_बायाँ == seL4_PageBits || pte_ptr_get_valid(lu_ret.ptSlot)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = seL4_DeleteFirst;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
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
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, (vaddr & ~MASK(lu_ret.pt_Bits_बायाँ)));

    setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
    फर्काऊ performPageTableInvocationMap(cap, cte, pte, ptSlot);
}

स्थिर exception_t decodeRISCVFrameInvocation(word_t label, word_t length,
                                              cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    विकल्प (label) {
    पक्ष RISCVPageMap: {
        यदि (unlikely(length < 3 || current_extra_caps.excaprefs[0] == रिक्त)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        word_t vaddr = getSyscallArg(0, buffer);
        word_t w_rightsMask = getSyscallArg(1, buffer);
        vm_attributes_t attr = vmAttributesFromWord(getSyscallArg(2, buffer));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_page_size_t frameSize = cap_frame_cap_get_capFSize(cap);
        vm_rights_t capVMRights = cap_frame_cap_get_capFVMRights(cap);

        यदि (unlikely(cap_get_capType(lvl1ptCap) != cap_page_table_cap ||
                     !cap_page_table_cap_get_capPTIsMapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(lvl1ptCap));
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(lvl1ptCap);

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        यदि (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(find_ret.vspace_जरा != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        word_t vtop = vaddr + BIT(pageBitsForSize(frameSize)) - 1;
        यदि (unlikely(vtop >= USER_शीर्ष)) {
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_सङ्ख्या = 0;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }
        यदि (unlikely(!checkVPAlignment(frameSize, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        यदि (unlikely(lu_ret.pt_Bits_बायाँ != pageBitsForSize(frameSize))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_Bits_बायाँ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        asid_t frame_asid = cap_frame_cap_get_capFMappedASID(cap);
        यदि (unlikely(frame_asid != asidInvalid)) {
            /* this frame is already mapped */
            यदि (frame_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_सङ्ख्या = 1;
                फर्काऊ EXCEPTION_SYSCALL_ERROR;
            }
            word_t mapped_vaddr = cap_frame_cap_get_capFMappedAddress(cap);
            यदि (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_सङ्ख्या = 0;
                फर्काऊ EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            यदि (unlikely(isPTEPageTable(lu_ret.ptSlot))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = seL4_DeleteFirst;
                फर्काऊ EXCEPTION_SYSCALL_ERROR;
            }
        } अन्यथा {
            /* check this vaddr isn't already mapped */
            यदि (unlikely(pte_ptr_get_valid(lu_ret.ptSlot))) {
                userError("Virtual address (0x%"SEL4_PRIx_word") already mapped", vaddr);
                current_syscall_error.type = seL4_DeleteFirst;
                फर्काऊ EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = maskVMRights(capVMRights, rightsFromWord(w_rightsMask));
        paddr_t frame_paddr = addrFromPPtr((शून्य *) cap_frame_cap_get_capFBasePtr(cap));
        cap = cap_frame_cap_set_capFMappedASID(cap, asid);
        cap = cap_frame_cap_set_capFMappedAddress(cap,  vaddr);

        bool_t executable = !vm_attributes_get_riscvExecuteNever(attr);
        pte_t pte = makeUserPTE(frame_paddr, executable, vmRights);
        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ performPageInvocationMapPTE(cap, cte, pte, lu_ret.ptSlot);
    }

    पक्ष RISCVPageUnmap: {
        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ performPageInvocationUnmap(cap, cte);
    }

    पक्ष RISCVPageGetAddress: {

        /* Check that there are enough message registers */
        प्रतिज्ञा(n_सन्देश_Registers >= 1);

        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ performPageGetAddress((शून्य *)cap_frame_cap_get_capFBasePtr(cap), call);
    }

    स्वभाव:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(word_t label, word_t length, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, word_t *buffer)
{
    विकल्प (cap_get_capType(cap)) {

    पक्ष cap_page_table_cap:
        फर्काऊ decodeRISCVPageTableInvocation(label, length, cte, cap, buffer);

    पक्ष cap_frame_cap:
        फर्काऊ decodeRISCVFrameInvocation(label, length, cte, cap, call, buffer);

    पक्ष cap_asid_control_cap: {
        word_t     i;
        asid_t           asid_base;
        word_t           index;
        word_t           depth;
        cap_t            untyped;
        cap_t            जरा;
        cte_t           *parentSlot;
        cte_t           *destSlot;
        lookupSlot_ret_t lu_ret;
        शून्य            *frame;
        exception_t      status;

        यदि (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        यदि (length < 2 || current_extra_caps.excaprefs[0] == रिक्त
            || current_extra_caps.excaprefs[1] == रिक्त) {
            current_syscall_error.type = seL4_TruncatedMessage;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        index = getSyscallArg(0, buffer);
        depth = getSyscallArg(1, buffer);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        जरा = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        लागि (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        यदि (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_तल्लो_Bits;

        यदि (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_capBlockSize(untyped) != seL4_ASIDPoolBits ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;

            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        यदि (status != EXCEPTION_NONE) {
            फर्काऊ status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(जरा, index, depth);
        यदि (lu_ret.status != EXCEPTION_NONE) {
            फर्काऊ lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        यदि (status != EXCEPTION_NONE) {
            फर्काऊ status;
        }

        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ performASIDControlInvocation(frame, destSlot, parentSlot, asid_base);
    }

    पक्ष cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t       asid;

        यदि (label != RISCVASIDPoolAssign) {
            current_syscall_error.type = seL4_IllegalOperation;

            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }
        यदि (current_extra_caps.excaprefs[0] == रिक्त) {
            current_syscall_error.type = seL4_TruncatedMessage;

            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        यदि (unlikely(
                cap_get_capType(vspaceCap) != cap_page_table_cap ||
                cap_page_table_cap_get_capPTIsMapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;

            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_माथिल्लो(cap_asid_pool_cap_get_capASIDBase(cap))];
        यदि (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_जरा_new();
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        यदि (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        लागि (i = 0; i < BIT(asid_तल्लो_Bits) && (asid + i == 0 || pool->array[i]); i++);

        यदि (i == BIT(asid_तल्लो_Bits)) {
            current_syscall_error.type = seL4_DeleteFirst;

            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }
    स्वभाव:
        fail("Invalid arch cap type");
    }
}

exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                          pte_t pte, pte_t *ptSlot)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    sfence();

    फर्काऊ EXCEPTION_NONE;
}

exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    यदि (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt
        );
        clearMemory((शून्य *)pt, seL4_PageTableBits);
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    फर्काऊ EXCEPTION_NONE;
}

स्थिर exception_t performPageGetAddress(शून्य *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = गाँठो_STATE(ksCurThread);
    यदि (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        अचिन्हित पूर्णाङ्क length = setMR(thread, ipcBuffer, 0, capFBasePtr);
        setRegister(thread, सन्देश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Running);
    फर्काऊ EXCEPTION_NONE;
}

स्थिर exception_t updatePTE(pte_t pte, pte_t *base)
{
    *base = pte;
    sfence();
    फर्काऊ EXCEPTION_NONE;
}

exception_t performPageInvocationMapPTE(cap_t cap, cte_t *ctSlot,
                                        pte_t pte, pte_t *base)
{
    ctSlot->cap = cap;
    फर्काऊ updatePTE(pte, base);
}

exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    यदि (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
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

    फर्काऊ EXCEPTION_NONE;
}

#ifdef CONFIG_PRINTING
शून्य Arch_user_थाक_Trace(tcb_t *tptr)
{
    cap_t thread_जरा = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    यदि (!is_Valid_VTable_जरा(thread_जरा)) {
        म_लिखो("Invalid vspace\n");
        फर्काऊ;
    }

    word_t sp = getRegister(tptr, SP);
    यदि (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        म_लिखो("SP %p not aligned", (शून्य *) sp);
        फर्काऊ;
    }

    pte_t *vspace_जरा = PTE_PTR(pptr_of_cap(thread_जरा));
    लागि (पूर्णाङ्क i = 0; i < CONFIG_USER_थाक_TRACE_LENGTH; i++) {
        word_t address = sp + (i * परिमाण(word_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_जरा, address);
        यदि (address < USER_शीर्ष && pte_ptr_get_valid(ret.ptSlot) && !isPTEPageTable(ret.ptSlot)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.ptSlot));
            word_t *मान = (word_t *)((word_t)pptr + (address & MASK(ret.pt_Bits_बायाँ)));
            म_लिखो("0x%lx: 0x%lx\n", (दीर्घ) address, (दीर्घ) *मान);
        } अन्यथा {
            म_लिखो("0x%lx: INVALID\n", (दीर्घ) address);
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
    lu_ret = lookupCapAndSlot(गाँठो_STATE(ksCurThread), frame_cptr);

    यदि (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    frameSize = cap_frame_cap_get_capFSize(lu_ret.cap);

    यदि (frameSize != RISCV_Mega_Page) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((शून्य *) frame_pptr);

#यदि __riscv_xlen == 32
    paddr_t physical_address = ksUserLogBuffer;
    लागि (word_t i = 0; i < BIT(PT_INDEX_BITS); i += 1) {
        kernel_image_level1_log_buffer_pt[i] = pte_next(physical_address, true);
        physical_address += BIT(PAGE_BITS);
    }
    प्रतिज्ञा(physical_address - ksUserLogBuffer == BIT(seL4_LargePageBits));
#अन्यथा
    kernel_image_level1_dev_pt[RISCV_GET_PT_INDEX(KS_LOG_PPTR, 1)] = pte_next(ksUserLogBuffer, true);
#endif

    sfence();

    फर्काऊ EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */
