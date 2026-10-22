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
    paddr_t frame_आधारः;
    vm_page_आकार_t frame_आकार;
    bool_t valid;
};
प्रकारनिर्देश संरचना resolve_ret resolve_ret_t;

स्थिर exception_t perform_Page_Get_स्थानाङ्कः(शून्य *vbase_ptr, bool_t call);

स्थिर word_t CONST RISCVGetWriteFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported. */
    प्रत्यागम vm_rights == VMReadWrite;
}

स्थिर अन्तःस्थ word_t CONST RISCVGetReadFromVMRights(vm_rights_t vm_rights)
{
    /* Write-only frame cap rights not currently supported.
     * Kernel-only conveys no user rights. */
    प्रत्यागम vm_rights != VMKernelOnly;
}

स्थिर अन्तःस्थ bool_t is_PTEPage_सारणी(pte_t *pte)
{
    प्रत्यागम pte_ptr_get_valid(pte) &&
           !(pte_ptr_get_read(pte) || pte_ptr_get_write(pte) || pte_ptr_get_execute(pte));
}

/** Helper function meant only to be used for mapping the kernel
 * window.
 *
 * Maps all pages with full RWX and supervisor perms by default.
 */
स्थिर pte_t pte_अग्र(word_t phys_addr, bool_t is_पर्णम्)
{
    word_t ppn = (word_t)(phys_addr >> 12);

    uint8_t पठ = is_पर्णम् ? 1 : 0;
    uint8_t लिख = पठ;
    uint8_t exec = पठ;

    प्रत्यागम pte_new(ppn,
                   0,     /* sw */
                   is_पर्णम् ? 1 : 0,     /* dirty (leaf)/reserved (non-leaf) */
                   is_पर्णम् ? 1 : 0,     /* accessed (leaf)/reserved (non-leaf) */
                   1,     /* global */
                   is_पर्णम् ? 0 : 0,     /* user (leaf)/reserved (non-leaf) */
                   exec,  /* execute */
                   लिख, /* write */
                   पठ,  /* read */
                   1      /* valid */
                  );
}

/* ==================== BOOT CODE STARTS HERE ==================== */

BOOT_संहिता शून्य map_kernel_frame(paddr_t paddr, pptr_t vaddr, vm_rights_t vm_rights)
{
#यदि __riscv_xlen == 32
    paddr = आवर्तनम्_अधस्तात्(paddr, RISCV_GET_LVL_PGSIZE_द्वयङ्काः(0));
    प्रतिज्ञा((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
    kernel_मूलम्_page_सारणी[RISCV_GET_PT_सूचकांक(vaddr, 0)] = pte_अग्र(paddr, true);
#अन्यथा
    यदि (vaddr >= KDEV_आधारः) {
        /* Map devices in level 1 page table */
        paddr = आवर्तनम्_अधस्तात्(paddr, RISCV_GET_LVL_PGSIZE_द्वयङ्काः(1));
        प्रतिज्ञा((paddr % RISCV_GET_LVL_PGSIZE(1)) == 0);
        kernel_image_level1_dev_pt[RISCV_GET_PT_सूचकांक(vaddr, 1)] = pte_अग्र(paddr, true);
    } अन्यथा {
        paddr = आवर्तनम्_अधस्तात्(paddr, RISCV_GET_LVL_PGSIZE_द्वयङ्काः(0));
        प्रतिज्ञा((paddr % RISCV_GET_LVL_PGSIZE(0)) == 0);
        kernel_मूलम्_page_सारणी[RISCV_GET_PT_सूचकांक(vaddr, 0)] = pte_अग्र(paddr, true);
    }
#यदि_अन्तः
}

BOOT_संहिता VISIBLE शून्य map_kernel_गवाक्षः(शून्य)
{
    /* mapping of KERNEL_ELF_BASE (virtual address) to kernel's
     * KERNEL_ELF_PHYS_BASE  */
    प्रतिज्ञा(CONFIG_PT_स्तराः > 1 && CONFIG_PT_स्तराः <= 4);
    /* Kernel image finishes before KDEV_BASE */
    प्रतिज्ञा(KERNEL_ELF_शिखरम् <= KDEV_आधारः);

    /* kernel window starts at PPTR_BASE */
    word_t pptr = PPTR_आधारः;

    /* first we map in memory from PADDR_BASE */
    word_t paddr = PADDR_आधारः;
    यावत् (pptr < PPTR_शिखरम्) {
        प्रतिज्ञा(IS_ALIGNED(pptr, RISCV_GET_LVL_PGSIZE_द्वयङ्काः(0)));
        प्रतिज्ञा(IS_ALIGNED(paddr, RISCV_GET_LVL_PGSIZE_द्वयङ्काः(0)));

        kernel_मूलम्_page_सारणी[RISCV_GET_PT_सूचकांक(pptr, 0)] = pte_अग्र(paddr, true);

        pptr += RISCV_GET_LVL_PGSIZE(0);
        paddr += RISCV_GET_LVL_PGSIZE(0);
    }
    /* now map KERNEL_ELF_BASE to KERNEL_ELF_PADDR_BASE */
    प्रतिज्ञा(pptr == PPTR_शिखरम्);
    pptr = आवर्तनम्_अधस्तात्(KERNEL_ELF_आधारः, RISCV_GET_LVL_PGSIZE_द्वयङ्काः(0));
    paddr = आवर्तनम्_अधस्तात्(KERNEL_ELF_PADDR_आधारः, RISCV_GET_LVL_PGSIZE_द्वयङ्काः(0));

#यदि __riscv_xlen == 32
    kernel_मूलम्_page_सारणी[RISCV_GET_PT_सूचकांक(pptr, 0)] = pte_अग्र(paddr, true);
    pptr += RISCV_GET_LVL_PGSIZE(0);
    paddr += RISCV_GET_LVL_PGSIZE(0);
#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
    kernel_मूलम्_page_सारणी[RISCV_GET_PT_सूचकांक(KS_LOG_PPTR, 0)] =
        pte_अग्र(kpptr_गन्तव्यम्_paddr(kernel_image_level1_log_बफर_pt), false);
#यदि_अन्तः
#अन्यथा
    word_t सूचकांक = 0;
    /* The kernel image is mapped twice, locating the two indexes in the
     * root page table, pointing them to the same level 1 page table.
     */
    kernel_मूलम्_page_सारणी[RISCV_GET_PT_सूचकांक(KERNEL_ELF_PADDR_आधारः + PPTR_आधारः_विस्थापनम्, 0)] =
        pte_अग्र(kpptr_गन्तव्यम्_paddr(kernel_image_level1_pt), false);
    kernel_मूलम्_page_सारणी[RISCV_GET_PT_सूचकांक(pptr, 0)] =
        pte_अग्र(kpptr_गन्तव्यम्_paddr(kernel_image_level1_pt), false);
    यावत् (pptr < PPTR_शिखरम् + RISCV_GET_LVL_PGSIZE(0)) {
        kernel_image_level1_pt[सूचकांक] = pte_अग्र(paddr, true);
        सूचकांक++;
        pptr += RISCV_GET_LVL_PGSIZE(1);
        paddr += RISCV_GET_LVL_PGSIZE(1);
    }

    /* Map kernel device page table */
    kernel_मूलम्_page_सारणी[RISCV_GET_PT_सूचकांक(KDEV_आधारः, 0)] =
        pte_अग्र(kpptr_गन्तव्यम्_paddr(kernel_image_level1_dev_pt), false);
#यदि_अन्तः

    /* There should be free space where we put device mapping */
    प्रतिज्ञा(pptr == UINTPTR_MAX - RISCV_GET_LVL_PGSIZE(0) + 1);
    map_kernel_devices();
}

BOOT_संहिता शून्य map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    lookupPTSlot_ret_t pt_ret;
    pte_t *targetSlot;
    vptr_t vptr = cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(pt_cap);
    pte_t *lvl1pt = PTE_PTR(pptr_सम्बन्धि_cap(vspace_cap));

    /* pt to be mapped */
    pte_t *pt   = PTE_PTR(pptr_सम्बन्धि_cap(pt_cap));

    /* Get PT slot to install the address in */
    pt_ret = lookupPTSlot(lvl1pt, vptr);

    targetSlot = pt_ret.ptSlot;

    *targetSlot = pte_new(
                      (addrFromPPtr(pt) >> se_L4_Page_द्वयङ्काः),
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

BOOT_संहिता शून्य map_it_frame_cap(cap_t vspace_cap, cap_t frame_cap)
{
    pte_t *lvl1pt   = PTE_PTR(pptr_सम्बन्धि_cap(vspace_cap));
    pte_t *frame_pptr   = PTE_PTR(pptr_सम्बन्धि_cap(frame_cap));
    vptr_t frame_vptr = cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(frame_cap);

    /* We deal with a frame as 4KiB */
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, frame_vptr);
    प्रतिज्ञा(lu_ret.pt_द्वयङ्काः_वामः == se_L4_Page_द्वयङ्काः);

    pte_t *targetSlot = lu_ret.ptSlot;

    *targetSlot = pte_new(
                      (pptr_गन्तव्यम्_paddr(frame_pptr) >> se_L4_Page_द्वयङ्काः),
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

BOOT_संहिता cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    cap_t cap = cap_frame_cap_new(
                    asidInvalid,                     /* capFMappedASID       */
                    pptr,                            /* capFBasePtr          */
                    0,                               /* capFSize             */
                    0,                               /* capFVMRights         */
                    0,
                    0                                /* capFMappedAddress    */
                );

    प्रत्यागम cap;
}

/* Create a page table for the initial thread */
स्थिर BOOT_संहिता cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_सारणी_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );

    map_it_pt_cap(vspace_cap, cap);
    प्रत्यागम cap;
}

BOOT_संहिता word_t arch_get_n_paging(v_region_t it_v_reg)
{
    word_t n = 0;
    प्रति (पूर्णाङ्क i = 0; i < CONFIG_PT_स्तराः - 1; i++) {
        n += get_n_paging(it_v_reg, RISCV_GET_LVL_PGSIZE_द्वयङ्काः(i));
    }
    प्रत्यागम n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_संहिता cap_t create_it_स्थानाङ्कः_space(cap_t मूलम्_cnode_cap, v_region_t it_v_reg)
{
    cap_t      lvl1pt_cap;
    vptr_t     pt_vptr;

    copyGlobalMappings(PTE_PTR(rootserver.vspace));

    lvl1pt_cap =
        cap_page_सारणी_cap_new(
            IT_ASID,               /* capPTMappedASID    */
            (word_t) rootserver.vspace,  /* capPTBasePtr       */
            1,                     /* capPTIsMapped      */
            (word_t) rootserver.vspace   /* capPTMappedAddress */
        );

    seL4_SlotPos slot_pos_before = ndks_boot.slot_pos_cur;
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadVSpace), lvl1pt_cap);

    /* create all n level PT caps necessary to cover userland image in 4KiB pages */
    प्रति (पूर्णाङ्क i = 0; i < CONFIG_PT_स्तराः - 1; i++) {

        प्रति (pt_vptr = आवर्तनम्_अधस्तात्(it_v_reg.start, RISCV_GET_LVL_PGSIZE_द्वयङ्काः(i));
             pt_vptr < it_v_reg.अन्तः_2;
             pt_vptr += RISCV_GET_LVL_PGSIZE(i)) {
            यदि (!provide_cap(मूलम्_cnode_cap,
                             create_it_pt_cap(lvl1pt_cap, it_alloc_paging(), pt_vptr, IT_ASID))
               ) {
                प्रत्यागम cap_null_cap_new();
            }
        }

    }

    seL4_SlotPos slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    प्रत्यागम lvl1pt_cap;
}

BOOT_संहिता शून्य activate_kernel_vspace(शून्य)
{
    समुच्चयः_VSpace_मूलम्(kpptr_गन्तव्यम्_paddr(&kernel_मूलम्_page_सारणी), 0);
}

BOOT_संहिता शून्य write_it_asid_pool(cap_t it_ap_cap, cap_t it_lvl1pt_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_सम्बन्धि_cap(it_ap_cap));
    ap->array[ASID_नीचः(IT_ASID)] = PTE_PTR(pptr_सम्बन्धि_cap(it_lvl1pt_cap));
    riscvKSASIDTable[ASID_उच्चः(IT_ASID)] = ap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

स्थिर findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_pool_t        *poolPtr;
    pte_t     *vspace_मूलम्;

    poolPtr = riscvKSASIDTable[ASID_उच्चः(asid)];
    यदि (!poolPtr) {
        current_lookup_fault = lookup_fault_invalid_मूलम्_new();

        ret.vspace_मूलम् = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    vspace_मूलम् = poolPtr->array[ASID_नीचः(asid)];
    यदि (!vspace_मूलम्) {
        current_lookup_fault = lookup_fault_invalid_मूलम्_new();

        ret.vspace_मूलम् = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    ret.vspace_मूलम् = vspace_मूलम्;
    ret.स्थितिः = EXCEPTION_NONE;
    प्रत्यागम ret;
}

शून्य copyGlobalMappings(pte_t *newLvl1pt)
{
    अचिह्नित दीर्घ i;
    pte_t *global_kernel_vspace = kernel_मूलम्_page_सारणी;

    प्रति (i = RISCV_GET_PT_सूचकांक(PPTR_आधारः, 0); i < BIT(PT_सूचकांक_द्वयङ्काः); i++) {
        newLvl1pt[i] = global_kernel_vspace[i];
    }
}

word_t *विशुद्धम् lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t w_बफर_Ptr;
    cap_t बफर_Cap;
    vm_rights_t vm_rights;

    w_बफर_Ptr = thread->tcbIPCBuffer;
    बफर_Cap = TCB_PTR_CTE_PTR(thread, tcb_बफर)->cap;

    यदि (unlikely(cap_get_capType(बफर_Cap) != cap_frame_cap)) {
        प्रत्यागम रिक्त;
    }
    यदि (unlikely(cap_frame_cap_get_capFIsDevice(बफर_Cap))) {
        प्रत्यागम रिक्त;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(बफर_Cap);
    यदि (likely(vm_rights == VMReadWrite ||
               (!isReceiver && vm_rights == VMReadOnly))) {
        word_t आधारः_Ptr, page_द्वयङ्काः;

        आधारः_Ptr = cap_frame_cap_get_capFBasePtr(बफर_Cap);
        page_द्वयङ्काः = page_द्वयङ्काः_For_आकार(cap_frame_cap_get_capFSize(बफर_Cap));
        प्रत्यागम (word_t *)(आधारः_Ptr + (w_बफर_Ptr & आवरणम्(page_द्वयङ्काः)));
    } अन्यथा {
        प्रत्यागम रिक्त;
    }
}

स्थिर अन्तःस्थ pte_t *getPPtrFromHWPTE(pte_t *pte)
{
    प्रत्यागम PTE_PTR(ptrFromPAddr(pte_ptr_get_ppn(pte) << se_L4_Page_सारणी_द्वयङ्काः));
}

lookupPTSlot_ret_t lookupPTSlot(pte_t *lvl1pt, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;

    word_t level = CONFIG_PT_स्तराः - 1;
    pte_t *pt = lvl1pt;

    /* this is how many bits we potentially have left to decode. Initially we have the
     * full address space to decode, and every time we walk this will be reduced. The
     * final value of this after the walk is the size of the frame that can be inserted,
     * or already exists, in ret.ptSlot. The following formulation is an invariant of
     * the loop: */
    ret.pt_द्वयङ्काः_वामः = PT_सूचकांक_द्वयङ्काः * level + se_L4_Page_द्वयङ्काः;
    ret.ptSlot = pt + ((vptr >> ret.pt_द्वयङ्काः_वामः) & आवरणम्(PT_सूचकांक_द्वयङ्काः));

    यावत् (is_PTEPage_सारणी(ret.ptSlot) && likely(0 < level)) {
        level--;
        ret.pt_द्वयङ्काः_वामः -= PT_सूचकांक_द्वयङ्काः;
        pt = getPPtrFromHWPTE(ret.ptSlot);
        ret.ptSlot = pt + ((vptr >> ret.pt_द्वयङ्काः_वामः) & आवरणम्(PT_सूचकांक_द्वयङ्काः));
    }

    प्रत्यागम ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    uint64_t addr;

    addr = read_stval();

    विकल्प (vm_faultType) {
    पक्ष RISCVLoadPageFault:
    पक्ष RISCVLoadAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVLoadAccessFault, false);
        प्रत्यागम EXCEPTION_FAULT;
    पक्ष RISCVStorePageFault:
    पक्ष RISCVStoreAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVStoreAccessFault, false);
        प्रत्यागम EXCEPTION_FAULT;
    पक्ष RISCVInstructionPageFault:
    पक्ष RISCVInstructionAccessFault:
        current_fault = seL4_Fault_VMFault_new(addr, RISCVInstructionAccessFault, true);
        प्रत्यागम EXCEPTION_FAULT;

    स्वभाव:
        fail("Invalid VM fault type");
    }
}

शून्य deleteASIDPool(asid_t asid_आधारः, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    प्रतिज्ञा(IS_ALIGNED(asid_आधारः, asid_नीचः_द्वयङ्काः));

    यदि (riscvKSASIDTable[ASID_उच्चः(asid_आधारः)] == pool) {
        riscvKSASIDTable[ASID_उच्चः(asid_आधारः)] = रिक्त;
        समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
    }
}

स्थिर exception_t performASIDControlInvocation(शून्य *frame, cte_t *slot, cte_t *parent, asid_t asid_आधारः)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_समुच्चयः_cap_Free_सूचकांक(&(parent->cap),
                                         MAX_FREE_सूचकांक(cap_untyped_cap_get_cap_खण्डम्_आकार(parent->cap)));

    memzero(frame, BIT(page_द्वयङ्काः_For_आकार(RISCV_4K_Page)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_निवेशय(
        cap_asid_pool_cap_new(
            asid_आधारः,          /* capASIDBase  */
            WORD_REF(frame)     /* capASIDPool  */
        ),
        parent,
        slot
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    प्रतिज्ञा((asid_आधारः & आवरणम्(asid_नीचः_द्वयङ्काः)) == 0);
    riscvKSASIDTable[ASID_उच्चः(asid_आधारः)] = (asid_pool_t *)frame;

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspaceCapSlot)
{
    cap_t cap = vspaceCapSlot->cap;
    pte_t *region_आधारः = PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap));
    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTMapped_ASID(cap, asid);
    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTMapped_स्थानाङ्कः(cap, 0);
    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTIs_Mapped(cap, 1);
    vspaceCapSlot->cap = cap;

    copyGlobalMappings(region_आधारः);

    poolPtr->array[ASID_नीचः(asid)] = region_आधारः;

    प्रत्यागम EXCEPTION_NONE;
}

शून्य deleteASID(asid_t asid, pte_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = riscvKSASIDTable[ASID_उच्चः(asid)];
    यदि (poolPtr != रिक्त && poolPtr->array[ASID_नीचः(asid)] == vspace) {
        hwASIDFlush(asid);
        poolPtr->array[ASID_नीचः(asid)] = रिक्त;
        समुच्चयः_VMRoot(ग्रन्थिः_STATE(ksCurThread));
    }
}

शून्य unmap_Page_सारणी(asid_t asid, vptr_t vptr, pte_t *target_pt)
{
    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
        /* nothing to do */
        प्रत्यागम;
    }
    /* We won't ever unmap a top level page table */
    प्रतिज्ञा(find_ret.vspace_मूलम् != target_pt);
    pte_t *ptSlot = रिक्त;
    pte_t *pt = find_ret.vspace_मूलम्;

    प्रति (word_t i = 0; i < CONFIG_PT_स्तराः - 1 && pt != target_pt; i++) {
        ptSlot = pt + RISCV_GET_PT_सूचकांक(vptr, i);
        यदि (unlikely(!is_PTEPage_सारणी(ptSlot))) {
            /* couldn't find it */
            प्रत्यागम;
        }
        pt = getPPtrFromHWPTE(ptSlot);
    }

    यदि (pt != target_pt) {
        /* didn't find it */
        प्रत्यागम;
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
    प्रत्यागम (pte_t) {
        0
    };
}

शून्य unmapPage(vm_page_आकार_t page_आकार, asid_t asid, vptr_t vptr, pptr_t pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;

    find_ret = findVSpaceForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम;
    }

    lu_ret = lookupPTSlot(find_ret.vspace_मूलम्, vptr);
    यदि (unlikely(lu_ret.pt_द्वयङ्काः_वामः != page_द्वयङ्काः_For_आकार(page_आकार))) {
        प्रत्यागम;
    }
    यदि (!pte_ptr_get_valid(lu_ret.ptSlot) || is_PTEPage_सारणी(lu_ret.ptSlot)
        || (pte_ptr_get_ppn(lu_ret.ptSlot) << se_L4_Page_द्वयङ्काः) != pptr_गन्तव्यम्_paddr((शून्य *)pptr)) {
        प्रत्यागम;
    }

    lu_ret.ptSlot[0] = pte_pte_invalid_new();
    sfence();
}

शून्य समुच्चयः_VMRoot(tcb_t *tcb)
{
    cap_t thread_मूलम्;
    asid_t asid;
    pte_t *lvl1pt;
    findVSpaceForASID_ret_t  find_ret;

    thread_मूलम् = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    यदि (cap_get_capType(thread_मूलम्) != cap_page_सारणी_cap) {
        समुच्चयः_VSpace_मूलम्(kpptr_गन्तव्यम्_paddr(&kernel_मूलम्_page_सारणी), 0);
        प्रत्यागम;
    }

    lvl1pt = PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(thread_मूलम्));

    asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(thread_मूलम्);
    find_ret = findVSpaceForASID(asid);
    यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE || find_ret.vspace_मूलम् != lvl1pt)) {
        समुच्चयः_VSpace_मूलम्(kpptr_गन्तव्यम्_paddr(&kernel_मूलम्_page_सारणी), 0);
        प्रत्यागम;
    }

    समुच्चयः_VSpace_मूलम्(addrFromPPtr(lvl1pt), asid);
}

bool_t CONST is_Valid_VTable_मूलम्(cap_t cap)
{
    प्रत्यागम (cap_get_capType(cap) == cap_page_सारणी_cap &&
            cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap));
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    यदि (unlikely(cap_get_capType(cap) != cap_frame_cap)) {
        userError("Requested IPC Buffer is not a frame cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(!IS_ALIGNED(vptr, se_L4_IPCBuffer_आकार_द्वयङ्काः))) {
        userError("Requested IPC Buffer location 0x%x is not aligned.",
                  (पूर्णाङ्क)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    प्रत्यागम EXCEPTION_NONE;
}

vm_rights_t CONST आवरणम्_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_आवरणम्)
{
    यदि (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_आवरणम्)) {
        प्रत्यागम VMReadOnly;
    }
    यदि (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_आवरणम्)) {
        यदि (!seL4_CapRights_get_capAllowWrite(cap_rights_आवरणम्)) {
            प्रत्यागम VMReadOnly;
        } अन्यथा {
            प्रत्यागम VMReadWrite;
        }
    }
    यदि (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_आवरणम्) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_आवरणम्)) {
        userError("Attempted to make unsupported write only mapping");
    }
    प्रत्यागम VMKernelOnly;
}

/* The rest of the file implements the RISCV object invocations */

स्थिर pte_t CONST makeUserPTE(paddr_t paddr, bool_t executable, vm_rights_t vm_rights)
{
    word_t लिख = RISCVGetWriteFromVMRights(vm_rights);
    word_t पठ = RISCVGetReadFromVMRights(vm_rights);
    यदि (unlikely(!पठ && !लिख && !executable)) {
        प्रत्यागम pte_pte_invalid_new();
    } अन्यथा {
        प्रत्यागम pte_new(
                   paddr >> se_L4_Page_द्वयङ्काः,
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

स्थिर अन्तःस्थ bool_t CONST checkVPAlignment(vm_page_आकार_t sz, word_t w)
{
    प्रत्यागम (w & आवरणम्(page_द्वयङ्काः_For_आकार(sz))) == 0;
}

स्थिर exception_t decode_RISCVPage_सारणी_Invocation(word_t label, word_t दीर्घता,
                                                  cte_t *cte, cap_t cap, word_t *बफर)
{
    यदि (label == RISCVPage_सारणी_Unmap) {
        यदि (unlikely(!isFinalCapability(cte))) {
            userError("RISCVPageTableUnmap: cannot unmap if more than once cap exists");
            current_syscall_error.type = seL4_RevokeFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        /* Ensure that if the page table is mapped, it is not a top level table */
        यदि (likely(cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap))) {
            asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap);
            findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
            pte_t *pte = PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap));
            यदि (unlikely(find_ret.स्थितिः == EXCEPTION_NONE &&
                         find_ret.vspace_मूलम् == pte)) {
                userError("RISCVPageTableUnmap: cannot call unmap on top level PageTable");
                current_syscall_error.type = seL4_RevokeFirst;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम perform_Page_सारणी_Invocation_Unmap(cap, cte);
    }

    यदि (unlikely((label != RISCVPage_सारणी_Map))) {
        userError("RISCVPageTable: Illegal Operation");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त)) {
        userError("RISCVPageTable: truncated message");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (unlikely(cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap))) {
        userError("RISCVPageTable: PageTable is already mapped.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    word_t vaddr = getSyscallArg(0, बफर);
    cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

    यदि (unlikely(cap_get_capType(lvl1ptCap) != cap_page_सारणी_cap ||
                 cap_page_सारणी_cap_get_cap_PTIs_Mapped(lvl1ptCap) == asidInvalid)) {
        userError("RISCVPageTableMap: Invalid top-level PageTable.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pte_t *lvl1pt = PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(lvl1ptCap));
    asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(lvl1ptCap);

    यदि (unlikely(vaddr >= USER_शिखरम्)) {
        userError("RISCVPageTableMap: Virtual address cannot be in kernel window.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
    यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (unlikely(find_ret.vspace_मूलम् != lvl1pt)) {
        userError("RISCVPageTableMap: ASID lookup failed");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);

    /* if there is already something mapped (valid is set) or we have traversed far enough
     * that a page table is not valid to map then tell the user that they have to delete
     * something before they can put a PT here */
    यदि (lu_ret.pt_द्वयङ्काः_वामः == se_L4_Page_द्वयङ्काः || pte_ptr_get_valid(lu_ret.ptSlot)) {
        userError("RISCVPageTableMap: All objects mapped at this address");
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* Get the slot to install the PT in */
    pte_t *ptSlot = lu_ret.ptSlot;

    paddr_t paddr = addrFromPPtr(
                        PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap)));
    pte_t pte = pte_new((paddr >> se_L4_Page_द्वयङ्काः),
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

    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTIs_Mapped(cap, 1);
    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTMapped_ASID(cap, asid);
    cap = cap_page_सारणी_cap_समुच्चयः_cap_PTMapped_स्थानाङ्कः(cap, (vaddr & ~आवरणम्(lu_ret.pt_द्वयङ्काः_वामः)));

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम perform_Page_सारणी_Invocation_Map(cap, cte, pte, ptSlot);
}

स्थिर exception_t decodeRISCVFrameInvocation(word_t label, word_t दीर्घता,
                                              cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    विकल्प (label) {
    पक्ष RISCVPageMap: {
        यदि (unlikely(दीर्घता < 3 || current_extra_caps.excaprefs[0] == रिक्त)) {
            userError("RISCVPageMap: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        word_t vaddr = getSyscallArg(0, बफर);
        word_t w_rights_आवरणम् = getSyscallArg(1, बफर);
        vm_गुणाः_t attr = vm_गुणाः_From_Word(getSyscallArg(2, बफर));
        cap_t lvl1ptCap = current_extra_caps.excaprefs[0]->cap;

        vm_page_आकार_t frame_आकार = cap_frame_cap_get_capFSize(cap);
        vm_rights_t capVMRights = cap_frame_cap_get_capFVMRights(cap);

        यदि (unlikely(cap_get_capType(lvl1ptCap) != cap_page_सारणी_cap ||
                     !cap_page_सारणी_cap_get_cap_PTIs_Mapped(lvl1ptCap))) {
            userError("RISCVPageMap: Bad PageTable cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        pte_t *lvl1pt = PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(lvl1ptCap));
        asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(lvl1ptCap);

        findVSpaceForASID_ret_t find_ret = findVSpaceForASID(asid);
        यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE)) {
            userError("RISCVPageMap: No PageTable for ASID");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (unlikely(find_ret.vspace_मूलम् != lvl1pt)) {
            userError("RISCVPageMap: ASID lookup failed");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* check the vaddr is valid */
        word_t vtop = vaddr + BIT(page_द्वयङ्काः_For_आकार(frame_आकार)) - 1;
        यदि (unlikely(vtop >= USER_शिखरम्)) {
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        यदि (unlikely(!checkVPAlignment(frame_आकार, vaddr))) {
            current_syscall_error.type = seL4_AlignmentError;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* Check if this page is already mapped */
        lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vaddr);
        यदि (unlikely(lu_ret.pt_द्वयङ्काः_वामः != page_द्वयङ्काः_For_आकार(frame_आकार))) {
            current_lookup_fault = lookup_fault_missing_capability_new(lu_ret.pt_द्वयङ्काः_वामः);
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        asid_t frame_asid = cap_frame_cap_get_capFMappedASID(cap);
        यदि (unlikely(frame_asid != asidInvalid)) {
            /* this frame is already mapped */
            यदि (frame_asid != asid) {
                userError("RISCVPageMap: Attempting to remap a frame that does not belong to the passed address space");
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_सङ्ख्या = 1;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
            word_t mapped_vaddr = cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap);
            यदि (unlikely(mapped_vaddr != vaddr)) {
                userError("RISCVPageMap: attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_तर्कः;
                current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
            /* this check is redundant, as lookupPTSlot does not stop on a page
             * table PTE */
            यदि (unlikely(is_PTEPage_सारणी(lu_ret.ptSlot))) {
                userError("RISCVPageMap: no mapping to remap.");
                current_syscall_error.type = seL4_DeleteFirst;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
        } अन्यथा {
            /* check this vaddr isn't already mapped */
            यदि (unlikely(pte_ptr_get_valid(lu_ret.ptSlot))) {
                userError("Virtual address (0x%"SEL4_PRIx_word") already mapped", vaddr);
                current_syscall_error.type = seL4_DeleteFirst;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
        }

        vm_rights_t vmRights = आवरणम्_VMRights(capVMRights, rightsFromWord(w_rights_आवरणम्));
        paddr_t frame_paddr = addrFromPPtr((शून्य *) cap_frame_cap_get_capFBasePtr(cap));
        cap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(cap, asid);
        cap = cap_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(cap,  vaddr);

        bool_t executable = !vm_गुणाः_get_riscv_Execute_Never(attr);
        pte_t pte = makeUserPTE(frame_paddr, executable, vmRights);
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performPageInvocationMapPTE(cap, cte, pte, lu_ret.ptSlot);
    }

    पक्ष RISCVPageUnmap: {
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performPageInvocationUnmap(cap, cte);
    }

    पक्ष RISCVPage_Get_स्थानाङ्कः: {

        /* Check that there are enough message registers */
        प्रतिज्ञा(n_सन्देशः_Registers >= 1);

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम perform_Page_Get_स्थानाङ्कः((शून्य *)cap_frame_cap_get_capFBasePtr(cap), call);
    }

    स्वभाव:
        userError("RISCVPage: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

}

exception_t decodeRISCVMMUInvocation(word_t label, word_t दीर्घता, cptr_t cptr,
                                     cte_t *cte, cap_t cap, bool_t call, word_t *बफर)
{
    विकल्प (cap_get_capType(cap)) {

    पक्ष cap_page_सारणी_cap:
        प्रत्यागम decode_RISCVPage_सारणी_Invocation(label, दीर्घता, cte, cap, बफर);

    पक्ष cap_frame_cap:
        प्रत्यागम decodeRISCVFrameInvocation(label, दीर्घता, cte, cap, call, बफर);

    पक्ष cap_asid_control_cap: {
        word_t     i;
        asid_t           asid_आधारः;
        word_t           सूचकांक;
        word_t           गभीरता;
        cap_t            untyped;
        cap_t            मूलम्;
        cte_t           *parentSlot;
        cte_t           *destSlot;
        lookupSlot_ret_t lu_ret;
        शून्य            *frame;
        exception_t      स्थितिः;

        यदि (label != RISCVASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त
            || current_extra_caps.excaprefs[1] == रिक्त) {
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        सूचकांक = getSyscallArg(0, बफर);
        गभीरता = getSyscallArg(1, बफर);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        मूलम् = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        प्रति (i = 0; i < nASIDPools && riscvKSASIDTable[i]; i++);

        यदि (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        asid_आधारः = i << asid_नीचः_द्वयङ्काः;

        यदि (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_खण्डम्_आकार(untyped) != se_L4_ASIDPool_द्वयङ्काः ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        स्थितिः = ensureNoChildren(parentSlot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम स्थितिः;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(मूलम्, सूचकांक, गभीरता);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम lu_ret.स्थितिः;
        }
        destSlot = lu_ret.slot;

        स्थितिः = ensure_रिक्तानि_Slot(destSlot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम स्थितिः;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performASIDControlInvocation(frame, destSlot, parentSlot, asid_आधारः);
    }

    पक्ष cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t       asid;

        यदि (label != RISCVASIDPool_नियोजय) {
            current_syscall_error.type = seL4_IllegalOperation;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        यदि (current_extra_caps.excaprefs[0] == रिक्त) {
            current_syscall_error.type = se_L4_Truncated_सन्देशः;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        यदि (unlikely(
                cap_get_capType(vspaceCap) != cap_page_सारणी_cap ||
                cap_page_सारणी_cap_get_cap_PTIs_Mapped(vspaceCap))) {
            userError("RISCVASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        pool = riscvKSASIDTable[ASID_उच्चः(cap_asid_pool_cap_get_capASIDBase(cap))];
        यदि (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_मूलम्_new();
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        प्रति (i = 0; i < BIT(asid_नीचः_द्वयङ्काः) && (asid + i == 0 || pool->array[i]); i++);

        यदि (i == BIT(asid_नीचः_द्वयङ्काः)) {
            current_syscall_error.type = seL4_DeleteFirst;

            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }
    स्वभाव:
        fail("Invalid arch cap type");
    }
}

exception_t perform_Page_सारणी_Invocation_Map(cap_t cap, cte_t *ctSlot,
                                          pte_t pte, pte_t *ptSlot)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    sfence();

    प्रत्यागम EXCEPTION_NONE;
}

exception_t perform_Page_सारणी_Invocation_Unmap(cap_t cap, cte_t *ctSlot)
{
    यदि (cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap));
        unmap_Page_सारणी(
            cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap),
            cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap),
            pt
        );
        clear_स्मृतिः((शून्य *)pt, se_L4_Page_सारणी_द्वयङ्काः);
    }
    cap_page_सारणी_cap_ptr_समुच्चयः_cap_PTIs_Mapped(&(ctSlot->cap), 0);

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t perform_Page_Get_स्थानाङ्कः(शून्य *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);
    यदि (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);
        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
        अचिह्नित पूर्णाङ्क दीर्घता = समुच्चयः_MR(thread, ipc_बफर, 0, capFBasePtr);
        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t updatePTE(pte_t pte, pte_t *आधारः)
{
    *आधारः = pte;
    sfence();
    प्रत्यागम EXCEPTION_NONE;
}

exception_t performPageInvocationMapPTE(cap_t cap, cte_t *ctSlot,
                                        pte_t pte, pte_t *आधारः)
{
    ctSlot->cap = cap;
    प्रत्यागम updatePTE(pte, आधारः);
}

exception_t performPageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    यदि (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        unmapPage(cap_frame_cap_get_capFSize(cap),
                  cap_frame_cap_get_capFMappedASID(cap),
                  cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                  cap_frame_cap_get_capFBasePtr(cap)
                 );
    }

    cap_t slotCap = ctSlot->cap;
    slotCap = cap_frame_cap_समुच्चयः_cap_FMapped_स्थानाङ्कः(slotCap, 0);
    slotCap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(slotCap, asidInvalid);
    ctSlot->cap = slotCap;

    प्रत्यागम EXCEPTION_NONE;
}

#यदि_परिभाषितम् CONFIG_PRINTING
शून्य Arch_user_राशिः_Trace(tcb_t *tptr)
{
    cap_t thread_मूलम् = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;
    यदि (!is_Valid_VTable_मूलम्(thread_मूलम्)) {
        प्रारूप_लिख("Invalid vspace\n");
        प्रत्यागम;
    }

    word_t sp = get_पञ्जिकाकोशः(tptr, SP);
    यदि (!IS_ALIGNED(sp, se_L4_Word_आकार_द्वयङ्काः)) {
        प्रारूप_लिख("SP %p not aligned", (शून्य *) sp);
        प्रत्यागम;
    }

    pte_t *vspace_मूलम् = PTE_PTR(pptr_सम्बन्धि_cap(thread_मूलम्));
    प्रति (पूर्णाङ्क i = 0; i < CONFIG_USER_राशिः_TRACE_दीर्घता; i++) {
        word_t स्थानाङ्कः = sp + (i * परिमाण(word_t));
        lookupPTSlot_ret_t ret = lookupPTSlot(vspace_मूलम्, स्थानाङ्कः);
        यदि (स्थानाङ्कः < USER_शिखरम् && pte_ptr_get_valid(ret.ptSlot) && !is_PTEPage_सारणी(ret.ptSlot)) {
            pptr_t pptr = (pptr_t)(getPPtrFromHWPTE(ret.ptSlot));
            word_t *मानम् = (word_t *)((word_t)pptr + (स्थानाङ्कः & आवरणम्(ret.pt_द्वयङ्काः_वामः)));
            प्रारूप_लिख("0x%lx: 0x%lx\n", (दीर्घ) स्थानाङ्कः, (दीर्घ) *मानम्);
        } अन्यथा {
            प्रारूप_लिख("0x%lx: INVALID\n", (दीर्घ) स्थानाङ्कः);
        }
    }
}
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
exception_t benchmark_arch_map_log_बफर(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_आकार_t frame_आकार;
    pptr_t  frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(ग्रन्थिः_STATE(ksCurThread), frame_cptr);

    यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    frame_आकार = cap_frame_cap_get_capFSize(lu_ret.cap);

    यदि (frame_आकार != RISCV_Mega_Page) {
        userError("Invalid frame size. The kernel expects large page log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ks_User_Log_बफर = pptr_गन्तव्यम्_paddr((शून्य *) frame_pptr);

#यदि __riscv_xlen == 32
    paddr_t physical_स्थानाङ्कः = ks_User_Log_बफर;
    प्रति (word_t i = 0; i < BIT(PT_सूचकांक_द्वयङ्काः); i += 1) {
        kernel_image_level1_log_बफर_pt[i] = pte_अग्र(physical_स्थानाङ्कः, true);
        physical_स्थानाङ्कः += BIT(PAGE_द्वयङ्काः);
    }
    प्रतिज्ञा(physical_स्थानाङ्कः - ks_User_Log_बफर == BIT(se_L4_Large_Page_द्वयङ्काः));
#अन्यथा
    kernel_image_level1_dev_pt[RISCV_GET_PT_सूचकांक(KS_LOG_PPTR, 1)] = pte_अग्र(ks_User_Log_बफर, true);
#यदि_अन्तः

    sfence();

    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */
