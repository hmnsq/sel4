/*
 * Copyright 2020, DornerWorks
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <types.h>
#समावेश <benchmark/benchmark.h>
#समावेश <api/failures.h>
#समावेश <api/syscall.h>
#समावेश <kernel/boot.h>
#समावेश <kernel/cspace.h>
#समावेश <kernel/thread.h>
#समावेश <object/tcb.h>
#समावेश <machine/io.h>
#समावेश <model/preemption.h>
#समावेश <model/statedata.h>
#समावेश <object/cnode.h>
#समावेश <object/untyped.h>
#समावेश <arch/api/invocation.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <linker.h>
#समावेश <arch/machine.h>
#समावेश <plat/machine/hardware.h>
#समावेश <kernel/stack.h>
#समावेश <util.h>

काष्ठा resolve_ret {
    paddr_t frameBase;
    vm_page_आकार_t frame_आकार;
    bool_t valid;
};
प्रकार काष्ठा resolve_ret resolve_ret_t;

अटल exception_t performPageGetAddress(व्योम *vbase_ptr, bool_t call);

अटल word_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    वापस vm_rights == VMReadWrite;
}

अटल अंतरभूत word_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported.
     * Kernel-only conveys no user rights. */
    वापस vm_rights != VMKernelOnly;
}

अटल अंतरभूत bool_t isPTEPageTable(pte_t *pte)
{
    वापस pte_ptr_get_valid(pte) &&
           !(pte_ptr_get_read(pte) || pte_ptr_get_write(pte) || pte_ptr_get_execute(pte));
}

/** Helper function meant only to be used for mapping the kernel
 * window.
 *
 * Maps all pages with full RWX and supervisor perms by default.
 */
अटल pte_t pte_अग्र(word_t phys_addr, bool_t is_leaf)
{
    word_t ppn = (word_t)(phys_addr >> 12);

    uint8_t पढ़ो = is_leaf ? 1 : 0;
    uint8_t ग_लिखो = पढ़ो;
    uint8_t exec = पढ़ो;

    वापस pte_new(ppn,
                   0,     /* sw */
                   is_leaf ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_leaf ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_leaf ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   ग_लिखो, /* write */
                   पढ़ो,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_CODE व्योम map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#अगर __riscv_xlen == 32
    paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(0));
    निश्चित((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_मूल_page_Table[RISCV_GET_PT_सूचकांक(vaddr, 0)] = pte_अग्र(paddr, true);
#अन्यथा
    अगर (vaddr >= KDEV_BASE) {
        /* Map devices in level 1 page table */
        paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(1));
        निश्चित((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_सूचकांक(vaddr, 1)] = pte_अग्र(paddr, true);
    } अन्यथा {
        paddr = ROUND_DOWN(paddr, RISCV_GET_LVL_PGSIZE_BITS(0));
        निश्चित((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_मूल_page_Table[RISCV_GET_PT_सूचकांक(vaddr, 0)] = pte_अग्र(paddr, true);
    }
#पूर्ण_अगर
}

BOOT_CODE VISIBLE व्योम map_kernel_window(व्योम)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    निश्चित(CONFIG_PT_LEVELS > 1 && CONFIG_PT_LEVELS <= 4);
    /* Kernel image finishes before KDEV_BASE */
    निश्चित(KERNEL_ELF_शीर्ष <= KDEV_BASE);

    /* kernel window starts at PPTR_BASE */
    word_t pptr = PPTR_BASE;

    /* first we map in memory from PADDR_BASE */
    word_t paddr = PADDR_BASE;
    जबतक (pptr < PPTR_शीर्ष) {
        निश्चित(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_BITS(0)));
        निश्चित(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_BITS(0)));

        kernel_मूल_page_Table[RISCV_GET_PT_सूचकांक(pptr, 0)] = pte_अग्र(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    निश्चित(pptr == PPTR_शीर्ष);
    pptr = ROUND_DOWN(KERNEL_ELF_BASE, RISCV_GET_LVL_PGSIZE_BITS(0));
    paddr = ROUND_DOWN(KERNEL_ELF_PADDR_BASE, RISCV_GET_LVL_PGSIZE_BITS(0));

#अगर __riscv_xlen == 32
    kernel_मूल_page_Table[RISCV_GET_PT_सूचकांक(pptr, 0)] = pte_अग्र(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#अगर_घोषित CONFIG_KERNEL_LOG_बफर
    kernel_मूल_page_Table[RISCV_GET_PT_सूचकांक(KS_LOG_PPTR, 0)] =
        pte_अग्र(kpptr_to_paddr(kernel_image_level1_log_बफर_pt), false);
#पूर्ण_अगर
#अन्यथा
    word_t सूचकांक = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_मूल_page_Table[RISCV_GET_PT_सूचकांक(KERNEL_ELF_PADDR_BASE + PPTR_BASE_OFFSET, 0)] =
        pte_अग्र(kpptr_to_paddr(kernel_image_level1_pt), false);
    kernel_मूल_page_Table[RISCV_GET_PT_सूचकांक(pptr, 0)] =
        pte_अग्र(kpptr_to_paddr(kernel_image_level1_pt), false);
    जबतक (pptr < PPTR_शीर्ष + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[सूचकांक] = pte_अग्र(paddr, true);
        सूचकांक++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_मूल_page_Table[RISCV_GET_PT_सूचकांक(KDEV_BASE, 0)] =
        pte_अग्र(kpptr_to_paddr(kernel_image_level1_dev_pt), false);
#पूर्ण_अगर

    /* There should be free space where we put device mapping */
    निश्चित(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_CODE व्योम map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
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

BOOT_CODE व्योम map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_of_cap(vspace_cap));
    pte_t *frame_pptr   = PTE_PTR(pptr_of_cap(frame_cap));
    vptr_t frame_vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, frame_vptr);
    निश्चित(lu_ret.pt_Bits_बायाँ == seL4_PageBits);

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

    वापस cap;
}

/* Create a page table for the initial thread */
अटल BOOT_CODE cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    वापस cap;
}

BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    word_t n = 0;
    क्रम (पूर्णांक i = 0; i < CONFIG_PT_LEVELS - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_BITS(i));
    }
    वापस n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t create_it_address_space(cap_t मूल_cnode_cap, v_region_t it_v_reg)
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
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    क्रम (पूर्णांक i = 0; i < CONFIG_PT_LEVELS - 1; i++) {

        क्रम (pt_vptr = ROUND_DOWN(it_v_reg.start, RISCV_GET_LVL_PGSIZE_BITS(i));
             pt_vptr < it_v_reg.end;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            अगर (!provide_cap(मूल_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                वापस cap_null_cap_new();
            }
        }

    }

    seL4_SlotPos slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    वापस lvl1pt_cap;
}

BOOT_CODE व्योम activate_kernel_vspace(व्योम)
{
    set_VSpace_मूल(kpptr_to_paddr(&kernel_मूल_page_Table), 0);
}

BOOT_CODE व्योम write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[ASID_निचला(IT_ASID)] = PTE_PTR(pptr_of_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_ऊपरी(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

अटल findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_मूल;

    poolPtr = riscvKSASIDTable[ASID_ऊपरी(asid)];
    अगर (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_मूल_new();

        ret.vspace_मूल = शून्य;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        वापस ret;
    }

    vspace_मूल = poolPtr->array[ASID_निचला(asid)];
    अगर (!vspace_मूल) {
        current_lookup_fault = lookup_fault_invalid_मूल_new();

        ret.vspace_मूल = शून्य;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        वापस ret;
    }

    ret.vspace_मूल = vspace_मूल;
    ret.status = EXCEPTION_NONE;
    वापस ret;
}

व्योम copyGlobalMappings(pte_t *newLvl1pt)
{
    अचिन्हित दीर्घ i;
    pte_t *global_kernel_vspace = kernel_मूल_page_Table;

    क्रम (i = RISCV_GET_PT_सूचकांक(PPTR_BASE, 0); i < BIT(PT_सूचकांक_BITS); i++) {
        newLvl1pt[i] = global_kernel_vspace[i];
    }
}

word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_बफर_Ptr;
    cap_t बफर_Cap;
    vm_rights_t vm_rights;

    w_बफर_Ptr = thread->tcbIPCBuffer;
    बफर_Cap = TCB_PTR_CTE_PTR(thread, tcb_बफर)->cap;

    अगर (unlikely(cap_get_capType(बफर_Cap) != cap_frame_cap)) {
        वापस शून्य;
    }
    अगर (unlikely(cap_frame_cap_get_capFIsDevice(बफर_Cap))) {
        वापस शून्य;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(बफर_Cap);
    अगर (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t basePtr, pageBits;

        basePtr = cap_frame_cap_get_capFBasePtr(बफर_Cap);
        pageBits = page_Bits_For_आकार(cap_frame_cap_get_capFSize(बफर_Cap));
        वापस (word_t *)(basePtr + (w_बफर_Ptr & MASK(pageBits)));
    } अन्यथा {
        वापस शून्य;
    }
}

अटल अंतरभूत pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    वापस PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << seL4_PageTableBits));
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
    ret.pt_Bits_बायाँ = PT_सूचकांक_BITS * level + seL4_PageBits;
    ret.ptSlot = pt + ((vptr >> ret.pt_Bits_बायाँ) & MASK(PT_सूचकांक_BITS));

    जबतक (isPTEPageTable(ret.ptSlot) && likely(0 < level)) {
        level--;
        ret.pt_Bits_बायाँ -= PT_सूचकांक_BITS;
        pt = getPPtrFromHWPTE(ret.ptSlot);
        ret.ptSlot = pt + ((vptr >> ret.pt_Bits_बायाँ) & MASK(PT_सूचकांक_BITS));
    }

    वापस ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    चयन (vm_faultType) {
    हाल RISCVLoadPageFault:
    हाल RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        वापस EXCEPTION_FAULT;
    हाल RISCVStorePageFault:
    हाल RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        वापस EXCEPTION_FAULT;
    हाल RISCVInstructionPageFault:
    हाल RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        वापस EXCEPTION_FAULT;

    शेष:
        fail("Invalid VM fault type");
    }
}

व्योम deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    निश्चित(IS_ALIGNED(asid_base, asid_निचला_Bits));

    अगर (riscvKSASIDTable[ASID_ऊपरी(asid_base)] == pool) {
        riscvKSASIDTable[ASID_ऊपरी(asid_base)] = शून्य;
        setVMRoot(गाँठ_STATE(ksCurThread));
    }
}

अटल exception_t performASIDControlInvocation(व्योम *frame, cte_t *slot, cte_t *parent, asid_t asid_base)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_cap_Free_सूचकांक(&(parent->cap),
                                         MAX_FREE_सूचकांक(cap_untyped_cap_get_cap_Block_आकार(parent->cap)));

    memzero(frame, BIT(page_Bits_For_आकार(RISCV_4K_Page)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_जोड़ो(
        cap_asid_pool_cap_new(
            asid_base,          /* capASIDBase  */
            WORD_REF(frame)     /* capASIDPool  */
        ),
        parent,
        slot
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    निश्चित((asid_base & MASK(asid_निचला_Bits)) == 0);
    riscvKSASIDTable[ASID_ऊपरी(asid_base)] = (asid_pool_t *)frame;

    वापस EXCEPTION_NONE;
}

अटल exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspaceCapSlot)
{
    cap_t cap = vspaceCapSlot->cap;
    pte_t *regionBase = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, 0);
    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    vspaceCapSlot->cap = cap;

    copyGlobalMappings(regionBase);

    poolPtr->array[ASID_निचला(asid)] = regionBase;

    वापस EXCEPTION_NONE;
}

व्योम deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_ऊपरी(asid)];
    अगर (poolPtr != शून्य && poolPtr->array[ASID_निचला(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_निचला(asid)] = शून्य;
        setVMRoot(गाँठ_STATE(ksCurThread));
    }
}

व्योम unmapPageTable(asid_t asid, vptr_t vptr, pte_t *target_pt)
{
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
        /* nothing to do */
        वापस;
    }
    /* We won't ever unmap a top level page table */
    निश्चित(find_ret.vspace_मूल != target_pt);
    pte_t *ptSlot = शून्य;
    pte_t *pt = find_ret.vspace_मूल;

    क्रम (word_t i = 0; i < CONFIG_PT_LEVELS - 1 && pt != target_pt; i++) {
        ptSlot = pt + RISCV_GET_PT_सूचकांक(vptr, i);
        अगर (unlikely(!isPTEPageTable(ptSlot))) {
            /* couldn't find it */
            वापस;
        }
        pt = getPPtrFromHWPTE(ptSlot);
    }

    अगर (pt != target_pt) {
        /* didn't find it */
        वापस;
    }
    /* If we found a pt then ptSlot won't be null */
    निश्चित(ptSlot != शून्य);
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

अटल pte_t pte_pte_invalid_new(व्योम)
{
    वापस (pte_t) {
        0
    };
}

व्योम unmapPage(vm_page_आकार_t page_आकार, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;

    find_ret = findVSpaceForASID(asid);
    अगर (find_ret.status != EXCEPTION_NONE) {
        वापस;
    }

    lu_ret = lookupPTSlot(find_ret.vspace_मूल, vptr);
    अगर (unlikely(lu_ret.pt_Bits_बायाँ != page_Bits_For_आकार(page_आकार))) {
        वापस;
    }
    अगर (!pte_ptr_get_valid(lu_ret.ptSlot) || isPTEPageTable(lu_ret.ptSlot)
        || (pte_ptr_get_ppn(lu_ret.ptSlot) << seL4_PageBits) != pptr_to_paddr((व्योम *)pptr)) {
        वापस;
    }

    lu_ret.ptSlot[0] = pte_pte_invalid_new();
    sfence();
}

व्योम setVMRoot(tcb_t *tcb)
{
    cap_t thread_मूल;
    asid_t asid;
    pte_t *lvl1pt;
    findVSpaceForASID_ret_t  find_ret;

    thread_मूल = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    अगर (cap_get_capType(thread_मूल) != cap_page_table_cap) {
        set_VSpace_मूल(kpptr_to_paddr(&kernel_मूल_page_Table), 0);
        वापस;
    }

    lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(thread_मूल));

    asid = cap_page_table_cap_get_capPTMappedASID(thread_मूल);
    find_ret = findVSpaceForASID(asid);
    अगर (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.vspace_मूल != lvl1pt)) {
        set_VSpace_मूल(kpptr_to_paddr(&kernel_मूल_page_Table), 0);
        वापस;
    }

    set_VSpace_मूल(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_मूल(cap_t cap)
{
    वापस (cap_get_capType(cap) == cap_page_table_cap &&
            cap_page_table_cap_get_capPTIsMapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    अगर (unlikely(cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(!IS_ALIGNED(vptr, se_L4_IPCBuffer_आकार_Bits))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (पूर्णांक)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    वापस EXCEPTION_NONE;
}

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    अगर (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        वापस VMReadOnly;
    }
    अगर (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        अगर (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            वापस VMReadOnly;
        } अन्यथा {
            वापस VMReadWrite;
        }
    }
    अगर (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    वापस VMKernelOnly;
}

/* The rest of the file implements the RISCV object invocations */

अटल pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    word_t ग_लिखो = RISCVGetWriteFromVMRights(vm_rights);
    word_t पढ़ो = RISCVGetReadFromVMRights(vm_rights);
    अगर (unlikely(!पढ़ो && !ग_लिखो && !executable)) {
        वापस pte_pte_invalid_new();
    } अन्यथा {
        वापस pte_new(
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

अटल अंतरभूत bool_t CONST checkVPAlignment(vm_page_आकार_t sz, word_t w)
{
    वापस (w & MASK(page_Bits_For_आकार(sz))) == 0;
}

अटल exception_t decodeRISCVPageTableInvocation(word_t label, word_t लंबाई,
                                                  cte_t *cte, cap_t cap, word_t *बफर)
{
    अगर (label == RISCVPageTableUnmap) {
        अगर (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = seL4_RevokeFirst;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        अगर (likely(cap_page_table_cap_get_capPTIsMapped(cap))) {
            asid_t asid = cap_page_table_cap_get_capPTMappedASID(cap);
            findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
            pte_t *pte = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
            अगर (unlikely(find_ret.status == EXCEPTION_NONE &&
                         find_ret.vspace_मूल == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = seL4_RevokeFirst;
                वापस EXCEPTION_SYSCALL_ERROR;
            }
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageTableInvocationUnmap(cap, cte);
    }

    अगर (unlikely((label != RISCVPageTableMap))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(लंबाई < 2 || current_extra_caps.excaprefs[0] == शून्य)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (unlikely(cap_page_table_cap_get_capPTIsMapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    word_t vaddr = getSyscallArg(0, बफर);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    अगर (unlikely(cap_get_capType(lvl1ptCap) != cap_page_table_cap ||
                 cap_page_table_cap_get_capPTIsMapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(lvl1ptCap));
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(lvl1ptCap);

    अगर (unlikely(vaddr >= USER_शीर्ष)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 0;

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (unlikely(find_ret.vspace_मूल != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    अगर (lu_ret.pt_Bits_बायाँ == seL4_PageBits || pte_ptr_get_valid(lu_ret.ptSlot)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = seL4_DeleteFirst;
        वापस EXCEPTION_SYSCALL_ERROR;
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

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस performPageTableInvocationMap(cap, cte, pte, ptSlot);
}

अटल exception_t decodeRISCVFrameInvocation(word_t label, word_t लंबाई,
                                              cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    चयन (label) {
    हाल RISCVPageMap: {
        अगर (unlikely(लंबाई < 3 || current_extra_caps.excaprefs[0] == शून्य)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        word_t vaddr = getSyscallArg(0, बफर);
        word_t w_rightsMask = getSyscallArg(1, बफर);
        vm_attributes_t attr = vmAttributesFromWord(getSyscallArg(2, बफर));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_page_आकार_t frame_आकार = cap_frame_cap_get_capFSize(cap);
        vm_rights_t capVMRights = cap_frame_cap_get_capFVMRights(cap);

        अगर (unlikely(cap_get_capType(lvl1ptCap) != cap_page_table_cap ||
                     !cap_page_table_cap_get_capPTIsMapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(lvl1ptCap));
        asid_t asid = cap_page_table_cap_get_capPTMappedASID(lvl1ptCap);

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        अगर (unlikely(find_ret.status != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (unlikely(find_ret.vspace_मूल != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        word_t vtop = vaddr + BIT(page_Bits_For_आकार(frame_आकार)) - 1;
        अगर (unlikely(vtop >= USER_शीर्ष)) {
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अगर (unlikely(!checkVPAlignment(frame_आकार, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        अगर (unlikely(lu_ret.pt_Bits_बायाँ != page_Bits_For_आकार(frame_आकार))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_Bits_बायाँ);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        asid_t frame_asid = cap_frame_cap_get_capFMappedASID(cap);
        अगर (unlikely(frame_asid != asidInvalid)) {
            /* this frame is already mapped */
            अगर (frame_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_संख्या = 1;
                वापस EXCEPTION_SYSCALL_ERROR;
            }
            word_t mapped_vaddr = cap_frame_cap_get_capFMappedAddress(cap);
            अगर (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_संख्या = 0;
                वापस EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            अगर (unlikely(isPTEPageTable(lu_ret.ptSlot))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = seL4_DeleteFirst;
                वापस EXCEPTION_SYSCALL_ERROR;
            }
        } अन्यथा {
            /* check this vaddr isn't already mapped */
            अगर (unlikely(pte_ptr_get_valid(lu_ret.ptSlot))) {
                userError("Virtual address (0x%"SEL4_PRIx_word") already mapped", vaddr);
                current_syscall_error.type = seL4_DeleteFirst;
                वापस EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = maskVMRights(capVMRights, rightsFromWord(w_rightsMask));
        paddr_t frame_paddr = addrFromPPtr((व्योम *) cap_frame_cap_get_capFBasePtr(cap));
        cap = cap_frame_cap_set_capFMappedASID(cap, asid);
        cap = cap_frame_cap_set_capFMappedAddress(cap,  vaddr);

        bool_t executable = !vm_attributes_get_riscvExecuteNever(attr);
        pte_t pte = makeUserPTE(frame_paddr, executable, vmRights);
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageInvocationMapPTE(cap, cte, pte, lu_ret.ptSlot);
    }

    हाल RISCVPageUnmap: {
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageInvocationUnmap(cap, cte);
    }

    हाल RISCVPageGetAddress: {

        /* Check that there are enough message registers */
        निश्चित(n_संदेश_Registers >= 1);

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performPageGetAddress((व्योम *)cap_frame_cap_get_capFBasePtr(cap), call);
    }

    शेष:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        वापस EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(word_t label, word_t लंबाई, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    चयन (cap_get_capType(cap)) {

    हाल cap_page_table_cap:
        वापस decodeRISCVPageTableInvocation(label, लंबाई, cte, cap, बफर);

    हाल cap_frame_cap:
        वापस decodeRISCVFrameInvocation(label, लंबाई, cte, cap, call, बफर);

    हाल cap_asid_control_cap: {
        word_t     i;
        asid_t           asid_base;
        word_t           सूचकांक;
        word_t           depth;
        cap_t            untyped;
        cap_t            मूल;
        cte_t           *parentSlot;
        cte_t           *destSlot;
        lookupSlot_ret_t lu_ret;
        व्योम            *frame;
        exception_t      status;

        अगर (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (लंबाई < 2 || current_extra_caps.excaprefs[0] == शून्य
            || current_extra_caps.excaprefs[1] == शून्य) {
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        सूचकांक = getSyscallArg(0, बफर);
        depth = getSyscallArg(1, बफर);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        मूल = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        क्रम (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        अगर (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_निचला_Bits;

        अगर (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_Block_आकार(untyped) != seL4_ASIDPoolBits ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        अगर (status != EXCEPTION_NONE) {
            वापस status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(मूल, सूचकांक, depth);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            वापस lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        अगर (status != EXCEPTION_NONE) {
            वापस status;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performASIDControlInvocation(frame, destSlot, parentSlot, asid_base);
    }

    हाल cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t       asid;

        अगर (label != RISCVASIDPoolAssign) {
            current_syscall_error.type = seL4_IllegalOperation;

            वापस EXCEPTION_SYSCALL_ERROR;
        }
        अगर (current_extra_caps.excaprefs[0] == शून्य) {
            current_syscall_error.type = seL4_TruncatedMessage;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        अगर (unlikely(
                cap_get_capType(vspaceCap) != cap_page_table_cap ||
                cap_page_table_cap_get_capPTIsMapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 1;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_ऊपरी(cap_asid_pool_cap_get_capASIDBase(cap))];
        अगर (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_मूल_new();
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        क्रम (i = 0; i < BIT(asid_निचला_Bits) && (asid + i == 0 || pool->array[i]); i++);

        अगर (i == BIT(asid_निचला_Bits)) {
            current_syscall_error.type = seL4_DeleteFirst;

            वापस EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }
    शेष:
        fail("Invalid arch cap type");
    }
}

exception_t performPageTableInvocationMap(cap_t cap, cte_t *ctSlot,
                                          pte_t pte, pte_t *ptSlot)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    sfence();

    वापस EXCEPTION_NONE;
}

exception_t performPageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    अगर (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt
        );
        clearMemory((व्योम *)pt, seL4_PageTableBits);
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    वापस EXCEPTION_NONE;
}

अटल exception_t performPageGetAddress(व्योम *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = गाँठ_STATE(ksCurThread);
    अगर (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        अचिन्हित पूर्णांक लंबाई = setMR(thread, ipc_बफर, 0, capFBasePtr);
        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, लंबाई)));
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
    वापस EXCEPTION_NONE;
}

अटल exception_t updatePTE(pte_t pte, pte_t *base)
{
    *base = pte;
    sfence();
    वापस EXCEPTION_NONE;
}

exception_t performPageInvocationMapPTE(cap_t cap, cte_t *ctSlot,
                                        pte_t pte, pte_t *base)
{
    ctSlot->cap = cap;
    वापस updatePTE(pte, base);
}

exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    अगर (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
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

    वापस EXCEPTION_NONE;
}

#अगर_घोषित CONFIG_PRINTING
व्योम Arch_user_ढेर_Trace(tcb_t *tptr)
{
    cap_t thread_मूल = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    अगर (!is_Valid_VTable_मूल(thread_मूल)) {
        म_लिखो("Invalid vspace\n");
        वापस;
    }

    word_t sp = getRegister(tptr, SP);
    अगर (!IS_ALIGNED(sp, se_L4_Word_आकार_Bits)) {
        म_लिखो("SP %p not aligned", (व्योम *) sp);
        वापस;
    }

    pte_t *vspace_मूल = PTE_PTR(pptr_of_cap(thread_मूल));
    क्रम (पूर्णांक i = 0; i < CONFIG_USER_ढेर_TRACE_लंबाई; i++) {
        word_t address = sp + (i * माप(word_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_मूल, address);
        अगर (address < USER_शीर्ष && pte_ptr_get_valid(ret.ptSlot) && !isPTEPageTable(ret.ptSlot)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.ptSlot));
            word_t *मूल्य = (word_t *)((word_t)pptr + (address & MASK(ret.pt_Bits_बायाँ)));
            म_लिखो("0x%lx: 0x%lx\n", (दीर्घ) address, (दीर्घ) *मूल्य);
        } अन्यथा {
            म_लिखो("0x%lx: INVALID\n", (दीर्घ) address);
        }
    }
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_KERNEL_LOG_बफर
exception_t benchmark_arch_map_log_बफर(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_आकार_t frame_आकार;
    pptr_t  frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(गाँठ_STATE(ksCurThread), frame_cptr);

    अगर (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    frame_आकार = cap_frame_cap_get_capFSize(lu_ret.cap);

    अगर (frame_आकार != RISCV_Mega_Page) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        वापस EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ks_User_Log_बफर = pptr_to_paddr((व्योम *) frame_pptr);

#अगर __riscv_xlen == 32
    paddr_t physical_address = ks_User_Log_बफर;
    क्रम (word_t i = 0; i < BIT(PT_सूचकांक_BITS); i += 1) {
        kernel_image_level1_log_बफर_pt[i] = pte_अग्र(physical_address, true);
        physical_address += BIT(PAGE_BITS);
    }
    निश्चित(physical_address - ks_User_Log_बफर == BIT(seL4_LargePageBits));
#अन्यथा
    kernel_image_level1_dev_pt[RISCV_GET_PT_सूचकांक(KS_LOG_PPTR, 1)] = pte_अग्र(ks_User_Log_बफर, true);
#पूर्ण_अगर

    sfence();

    वापस EXCEPTION_NONE;
}
#पूर्ण_अगर /* CONFIG_KERNEL_LOG_BUFFER */
