/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <kernel/boot.h>
#ضمن <model/statedata.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/kernel/boot.h>
#ضمن <arch/kernel/boot_sys.h>
#ضمن <arch/api/invocation.h>
#ضمن <benchmark/benchmark_track.h>
#ضمن <arch/kernel/tlb_bitmap.h>
#ضمن <mode/kernel/tlb.h>

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

/* initialise the Task State Segment (TSS) */

BOOT_CODE فراغ init_tss(tss_t *tss)
{
    *tss = tss_new(
               حجم(*tss),   /* io_map_base  */
               0,              /* trap         */
               SEL_NULL,       /* sel_ldt      */
               SEL_NULL,       /* gs           */
               SEL_NULL,       /* fs           */
               SEL_NULL,       /* ds           */
               SEL_NULL,       /* ss           */
               SEL_NULL,       /* cs           */
               SEL_NULL,       /* es           */
               0,              /* edi          */
               0,              /* esi          */
               0,              /* ebp          */
               0,              /* esp          */
               0,              /* ebx          */
               0,              /* edx          */
               0,              /* ecx          */
               0,              /* eax          */
               0,              /* eflags       */
               0,              /* eip          */
               0,              /* cr3          */
               SEL_NULL,       /* ss2          */
               0,              /* esp2         */
               SEL_NULL,       /* ss1          */
               0,              /* esp1         */
               SEL_DS_0,       /* ss0          */
               0,              /* esp0         */
               0               /* prev_task    */
           );
    memset(&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.io_map[0], 0xff,
           حجم(x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.io_map));
}
/* initialise Global Descriptor Table (GDT) */

BOOT_CODE فراغ init_gdt(gdt_entry_t *gdt, tss_t *tss)
{
    uint32_t tss_addr = (uint32_t)tss;

    /* Set the NULL descriptor */
    gdt[GDT_NULL] = gdt_entry_gdt_null_new();

    /* 4GB flat kernel code segment on ring 0 descriptor */
    gdt[GDT_CS_0] = gdt_entry_gdt_code_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /* Available                    */
                        0xf,    /* Segment limit high 4 bits    */
                        1,      /* Present                      */
                        0,      /* Descriptor privilege level   */
                        1,      /* readable                     */
                        1,      /* accessed                     */
                        0,      /* Base middle 8 bits           */
                        0,      /* Base low 16 bits             */
                        0xffff  /* Segment limit low 16 bits    */
                    );

    /* 4GB flat kernel data segment on ring 0 descriptor */
    gdt[GDT_DS_0] = gdt_entry_gdt_data_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /* Available                    */
                        0xf,    /* Segment limit high 4 bits    */
                        1,      /* Present                      */
                        0,      /* Descriptor privilege level   */
                        1,      /* writable                     */
                        1,      /* accessed                     */
                        0,      /* Base middle 8 bits           */
                        0,      /* Base low 16 bits             */
                        0xffff  /* Segment limit low 16 bits    */
                    );

    /* 4GB flat userland code segment on ring 3 descriptor */
    gdt[GDT_CS_3] = gdt_entry_gdt_code_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /* Available                    */
                        0xf,    /* Segment limit high 4 bits    */
                        1,      /* Present                      */
                        3,      /* Descriptor privilege level   */
                        1,      /* readable                     */
                        1,      /* accessed                     */
                        0,      /* Base middle 8 bits           */
                        0,      /* Base low 16 bits             */
                        0xffff  /* Segment limit low 16 bits    */
                    );

    /* 4GB flat userland data segment on ring 3 descriptor */
    gdt[GDT_DS_3] = gdt_entry_gdt_data_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /* Available                    */
                        0xf,    /* Segment limit high 4 bits    */
                        1,      /* Present                      */
                        3,      /* Descriptor privilege level   */
                        1,      /* writable                     */
                        1,      /* accessed                     */
                        0,      /* Base middle 8 bits           */
                        0,      /* Base low 16 bits             */
                        0xffff  /* Segment limit low 16 bits    */
                    );

    /* Task State Segment (TSS) descriptor */
    gdt[GDT_TSS] = gdt_entry_gdt_tss_new(
                       tss_addr >> 24,              /* base_high 8 bits     */
                       0,                           /* granularity          */
                       0,                           /* avl                  */
                       0,                           /* limit_high 4 bits    */
                       1,                           /* present              */
                       0,                           /* dpl                  */
                       0,                           /* busy                 */
                       1,                           /* always_true          */
                       (tss_addr >> 16) & 0xff,     /* base_mid 8 bits      */
                       (tss_addr & 0xffff),         /* base_low 16 bits     */
                       حجم(tss_io_t) - 1         /* limit_low 16 bits    */
                   );

    gdt[GDT_FS] = gdt_entry_gdt_data_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /* Available                    */
                      0xf,    /* Segment limit high 4 bits    */
                      1,      /* Present                      */
                      3,      /* Descriptor privilege level   */
                      1,      /* writable                     */
                      1,      /* accessed                     */
                      0,      /* Base middle 8 bits           */
                      0,      /* Base low 16 bits             */
                      0xffff  /* Segment limit low 16 bits    */
                  );

    gdt[GDT_GS] = gdt_entry_gdt_data_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /* Available                    */
                      0xf,    /* Segment limit high 4 bits    */
                      1,      /* Present                      */
                      3,      /* Descriptor privilege level   */
                      1,      /* writable                     */
                      1,      /* accessed                     */
                      0,      /* Base middle 8 bits           */
                      0,      /* Base low 16 bits             */
                      0xffff  /* Segment limit low 16 bits    */
                  );
}

/* initialise the Interrupt Descriptor Table (IDT) */

BOOT_CODE فراغ init_idt_entry(idt_entry_t *idt, interrupt_t interrupt, فراغ(*handler)(فراغ))
{
    uint32_t handler_addr = (uint32_t)handler;
    uint32_t dpl = 3;

    إذا (interrupt < int_trap_min && interrupt != int_software_break_request) {
        dpl = 0;
    }

    idt[interrupt] = idt_entry_interrupt_gate_new(
                         handler_addr >> 16,   /* offset_high  */
                         1,                    /* present      */
                         dpl,                  /* dpl          */
                         1,                    /* gate_size    */
                         SEL_CS_0,             /* seg_selector */
                         handler_addr & 0xffff /* offset_low   */
                     );
}

BOOT_CODE bool_t map_kernel_window(
    uint32_t رەقەم_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   رەقەم_drhu,
    paddr_t   *drhu_list
)
{
    paddr_t  phys;
    uint32_t idx;
    pde_t    pde;
    pte_t    pte;
    unsigned صحيح UNUSED i;

    /* Mapping of PPTR_BASE (virtual address) to kernel's PADDR_BASE
     * up to end of virtual address space except for the last large page.
     */
    phys = PADDR_BASE;
    idx = PPTR_BASE >> LARGE_PAGE_BITS;

    /* PPTR_TOP differs whether CONFIG_KERNEL_LOG_BUFFER
     * is enabled or not.
     */
    طالما (idx < (PPTR_چوققا >> LARGE_PAGE_BITS)) {
        pde = pde_pde_large_new(
                  phys,   /* page_base_address    */
                  0,      /* pat                  */
                  0,      /* avl                  */
                  1,      /* global               */
                  0,      /* dirty                */
                  0,      /* accessed             */
                  0,      /* cache_disabled       */
                  0,      /* write_through        */
                  0,      /* super_user           */
                  1,      /* read_write           */
                  1       /* present              */
              );
        ia32KSGlobalPD[idx] = pde;
        phys += BIT(LARGE_PAGE_BITS);
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    أكد(phys == PADDR_چوققا);

#ifdef CONFIG_KERNEL_LOG_BUFFER
    /* Map global page table for the log buffer */
    pde = pde_pde_pt_new(
              kpptr_to_paddr(ia32KSGlobalLogPT), /* pt_base_address  */
              0,                 /* avl              */
              0,                 /* accessed         */
              0,                 /* cache_disabled   */
              0,                 /* write_through    */
              0,                 /* super_user       */
              1,                 /* read_write       */
              1                  /* present          */
          );

    ia32KSGlobalPD[idx] = pde;
    phys += BIT(LARGE_PAGE_BITS);
    أكد(idx == (KS_LOG_PPTR >> LARGE_PAGE_BITS));
    idx++;
#endif /* CONFIG_KERNEL_LOG_BUFFER */

#ifdef ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(ia32KSGlobalPD);

    phys += TLBBITMAP_PD_RESERVED;
    idx += TLBBITMAP_يىلتىز_ENTRIES;
#endif /* ENABLE_SMP_SUPPORT */

    /* map page table of last 4M of virtual address space to page directory */
    pde = pde_pde_pt_new(
              kpptr_to_paddr(ia32KSGlobalPT), /* pt_base_address  */
              0,                 /* avl              */
              0,                 /* accessed         */
              0,                 /* cache_disabled   */
              0,                 /* write_through    */
              0,                 /* super_user       */
              1,                 /* read_write       */
              1                  /* present          */
          );
    ia32KSGlobalPD[idx] = pde;

    /* Start with an empty guard page preceding the stack. */
    idx = 0;
    pte = pte_new(
              0,      /* page_base_address    */
              0,      /* avl                  */
              0,      /* global               */
              0,      /* pat                  */
              0,      /* dirty                */
              0,      /* accessed             */
              0,      /* cache_disabled       */
              0,      /* write_through        */
              0,      /* super_user           */
              0,      /* read_write           */
              0       /* present              */
          );
    ia32KSGlobalPT[idx] = pte;
    idx++;

    /* null mappings up to KDEV_BASE */

    طالما (idx < (KDEV_BASE &MASK(LARGE_PAGE_BITS)) >> PAGE_BITS) {
        pte = pte_new(
                  0,      /* page_base_address    */
                  0,      /* avl                  */
                  0,      /* global               */
                  0,      /* pat                  */
                  0,      /* dirty                */
                  0,      /* accessed             */
                  0,      /* cache_disabled       */
                  0,      /* write_through        */
                  0,      /* super_user           */
                  0,      /* read_write           */
                  0       /* present              */
              );
        ia32KSGlobalPT[idx] = pte;
        idx++;
    }

    /* map kernel devices (devices only used by the kernel) */
    إذا (!map_kernel_window_devices(ia32KSGlobalPT, رەقەم_ioapic, ioapic_paddrs, رەقەم_drhu, drhu_list)) {
        أرجع false;
    }

    invalidateLocalPageStructureCache();
    أرجع true;
}

/* Note: this function will invalidate any pointers previously returned from this function */
BOOT_CODE فراغ *map_temp_boot_page(فراغ *entry, uint32_t large_pages)
{
    فراغ *replacement_vaddr;
    unsigned صحيح i;
    unsigned صحيح offset_in_page;

    unsigned صحيح phys_pg_start = (unsigned صحيح)(entry) & ~MASK(LARGE_PAGE_BITS);
    unsigned صحيح virt_pd_start = (PPTR_BASE >> LARGE_PAGE_BITS) - large_pages;
    unsigned صحيح virt_pg_start = PPTR_BASE - (large_pages << LARGE_PAGE_BITS);

    لكل (i = 0; i < large_pages; i++) {
        unsigned صحيح pg_offset = i << LARGE_PAGE_BITS; // num pages since start * page size

        *(get_boot_pd() + virt_pd_start + i) = pde_pde_large_new(
                                                   phys_pg_start + pg_offset, /* physical address */
                                                   0, /* pat            */
                                                   0, /* avl            */
                                                   1, /* global         */
                                                   0, /* dirty          */
                                                   0, /* accessed       */
                                                   0, /* cache_disabled */
                                                   0, /* write_through  */
                                                   0, /* super_user     */
                                                   1, /* read_write     */
                                                   1  /* present        */
                                               );
        invalidateLocalTranslationSingle(virt_pg_start + pg_offset);
    }

    // assign replacement virtual addresses page
    offset_in_page = (unsigned صحيح)(entry) & MASK(LARGE_PAGE_BITS);
    replacement_vaddr = (فراغ *)(virt_pg_start + offset_in_page);

    invalidateLocalPageStructureCache();

    أرجع replacement_vaddr;
}

/* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */

BOOT_CODE فراغ init_dtrs(فراغ)
{
    /* setup the GDT pointer and limit and load into GDTR */
    gdt_idt_ptr.limit = (حجم(gdt_entry_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.base = (uint32_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt;
    ia32_install_gdt(&gdt_idt_ptr);

    /* setup the IDT pointer and limit and load into IDTR */
    gdt_idt_ptr.limit = (حجم(idt_entry_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.base = (uint32_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt;
    ia32_install_idt(&gdt_idt_ptr);

    /* load NULL LDT selector into LDTR */
    ia32_install_ldt(SEL_NULL);

    /* load TSS selector into Task Register (TR) */
    ia32_install_tss(SEL_TSS);
}

ساكن BOOT_CODE cap_t create_it_page_table_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    إذا (asid != asidInvalid) {
        map_it_pt_cap(vspace_cap, cap);
    }
    أرجع cap;
}

ساكن BOOT_CODE cap_t create_it_page_directory_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_directory_cap_new(
              true,    /* capPDIsMapped   */
              IT_ASID, /* capPDMappedASID */
              vptr,    /* capPDMappedAddress */
              pptr  /* capPDBasePtr    */
          );
    إذا (asid != asidInvalid && cap_get_capType(vspace_cap) != cap_null_cap) {
        map_it_pd_cap(vspace_cap, cap);
    }
    أرجع cap;
}

BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    word_t n = get_n_paging(it_v_reg, PT_INDEX_BITS + PAGE_BITS);
#ifdef CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#endif
    أرجع n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_CODE cap_t create_it_address_space(cap_t يىلتىز_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    slot_pos_before = ndks_boot.slot_pos_cur;
    copyGlobalMappings((vspace_يىلتىز_t *)rootserver.vspace);
    cap_t pd_cap = create_it_page_directory_cap(cap_null_cap_new(), rootserver.vspace, 0, IT_ASID);
    write_slot(SLOT_PTR(pptr_of_cap(يىلتىز_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);
    vspace_cap = pd_cap;

    /* create all PT objs and caps necessary to cover userland image */

    لكل (vptr = ROUND_DOWN(it_v_reg.start, PT_INDEX_BITS + PAGE_BITS);
         vptr < it_v_reg.end;
         vptr += BIT(PT_INDEX_BITS + PAGE_BITS)) {
        إذا (!provide_cap(يىلتىز_cnode_cap,
                         create_it_page_table_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    أرجع vspace_cap;
}

ساكن BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           vm_page_map_type_t map_type)
{
    vm_page_size_t frame_size;

    إذا (use_large) {
        frame_size = X86_LargePage;
    } وإلا {
        frame_size = X86_SmallPage;
    }

    أرجع
        cap_frame_cap_new(
            frame_size,                    /* capFSize           */
            ASID_تۆۋەن(asid),                /* capFMappedASIDLow  */
            vptr,                          /* capFMappedAddress  */
            map_type,                      /* capFMapType        */
            false,                         /* capFIsDevice       */
            ASID_يۇقىرى(asid),               /* capFMappedASIDHigh */
            wordFromVMRights(VMReadWrite), /* capFVMRights       */
            pptr                           /* capFBasePtr        */
        );
}

BOOT_CODE cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_frame_cap(pptr, 0, asidInvalid, use_large, X86_MappingNone);
}

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable UNUSED)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large, X86_MappingVSpace);
    map_it_frame_cap(vspace_cap, cap);
    أرجع cap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

pde_t CONST makeUserPDELargePage(paddr_t paddr, vm_attributes_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pde_pde_large_new(
               paddr,                                          /* page_base_address    */
               vm_attributes_get_x86PATBit(vm_attr),           /* pat                  */
               0,                                              /* avl                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_attributes_get_x86PCDBit(vm_attr),           /* cache_disabled       */
               vm_attributes_get_x86PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST makeUserPDEPageTable(paddr_t paddr, vm_attributes_t vm_attr)
{
    أرجع pde_pde_pt_new(
               paddr,                                      /* pt_base_address  */
               0,                                          /* avl              */
               0,                                          /* accessed         */
               vm_attributes_get_x86PCDBit(vm_attr),       /* cache_disabled   */
               vm_attributes_get_x86PWTBit(vm_attr),       /* write_through    */
               1,                                          /* super_user       */
               1,                                          /* read_write       */
               1                                           /* present          */
           );
}

pde_t CONST makeUserPDEInvalid(فراغ)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    أرجع pde_pde_pt_new(
               0,  /* pt_base_address  */
               0,  /* avl              */
               0,  /* accessed         */
               0,  /* cache_disabled   */
               0,  /* write_through    */
               0,  /* super_user       */
               0,  /* read_write       */
               0   /* present          */
           );
}

pte_t CONST makeUserPTE(paddr_t paddr, vm_attributes_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pte_new(
               paddr,                                          /* page_base_address    */
               0,                                              /* avl                  */
               0,                                              /* global               */
               vm_attributes_get_x86PATBit(vm_attr),           /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_attributes_get_x86PCDBit(vm_attr),           /* cache_disabled       */
               vm_attributes_get_x86PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}


pte_t CONST makeUserPTEInvalid(فراغ)
{
    أرجع pte_new(
               0,                   /* page_base_address    */
               0,                   /* avl                  */
               0,                   /* global               */
               0,                   /* pat                  */
               0,                   /* dirty                */
               0,                   /* accessed             */
               0,                   /* cache_disabled       */
               0,                   /* write_through        */
               0,                   /* super_user           */
               0,                   /* read_write           */
               0                    /* present              */
           );
}

فراغ setVMRoot(tcb_t *tcb)
{
    cap_t               thread_يىلتىز;
    vspace_يىلتىز_t *vspace_يىلتىز;
    asid_t              asid;
    findVSpaceForASID_ret_t find_ret;

    thread_يىلتىز = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    vspace_يىلتىز = get_Valid_Native_يىلتىز(thread_يىلتىز);
    إذا (!vspace_يىلتىز) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_to_pptr(getCurrentPD()), getCurrentCPUIndex());)
        setCurrentPD(kpptr_to_paddr(ia32KSGlobalPD));
        أرجع;
    }

    asid = cap_get_capMappedASID(thread_يىلتىز);
    find_ret = findVSpaceForASID(asid);
    إذا (find_ret.status != EXCEPTION_NONE || find_ret.vspace_يىلتىز != vspace_يىلتىز) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_to_pptr(getCurrentPD()), getCurrentCPUIndex());)
        setCurrentPD(kpptr_to_paddr(ia32KSGlobalPD));
        أرجع;
    }

    /* only set PD if we change it, otherwise we flush the TLB needlessly */
    إذا (getCurrentPD() != pptr_to_paddr(vspace_يىلتىز)) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_to_pptr(getCurrentPD()), getCurrentCPUIndex());)
        SMP_COND_STATEMENT(tlb_bitmap_set(vspace_يىلتىز, getCurrentCPUIndex());)

        setCurrentPD(pptr_to_paddr(vspace_يىلتىز));
    }
}

فراغ hwASIDInvalidate(asid_t asid, vspace_يىلتىز_t *vspace)
{
    /* 32-bit does not have PCID */
    أرجع;
}

exception_t decodeX86ModeMMUInvocation(
    word_t invLabel,
    word_t length,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_page_directory_cap:
        أرجع decodeIA32PageDirectoryInvocation(invLabel, length, cte, cap, call, buffer);

    افتراضي:
        fail("Invalid arch cap type");
    }
}


bool_t modeUnmapPage(vm_page_size_t page_size, vspace_يىلتىز_t *vroot, vptr_t vaddr, فراغ *pptr)
{
    fail("Invalid page type");
    أرجع false;
}

exception_t decodeX86ModeMapPage(word_t invLabel, vm_page_size_t page_size, cte_t *cte, cap_t cap,
                                 vspace_يىلتىز_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_attributes_t vm_attr)
{
    fail("Invalid Page type");
}

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t benchmark_arch_map_logBuffer(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_size_t frameSize;
    pptr_t frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(تۈگۈن_STATE(ksCurThread), frame_cptr);

    إذا (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    frameSize = cap_frame_cap_get_capFSize(lu_ret.cap);

    إذا (frameSize != X86_LargePage) {
        userError("Invalid size for log Buffer. The kernel expects at least 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((فراغ *) frame_pptr);

    /* fill global log page table with mappings */
    لكل (صحيح idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
        paddr_t physical_address = ksUserLogBuffer + (idx << seL4_PageBits);

        pte_t pte = pte_new(
                        physical_address,   /* page_base_address    */
                        0,                  /* avl                  */
                        1,                  /* global               */
                        VMKernelOnly,       /* pat                  */
                        0,                  /* dirty                */
                        0,                  /* accessed             */
                        0,                  /* cache_disabled       */
                        1,                  /* write_through        */
                        1,                  /* super_user           */
                        1,                  /* read_write           */
                        1                   /* present              */
                    );

        ia32KSGlobalLogPT[idx] = pte;
        invalidateTLBEntry(KS_LOG_PPTR + (idx << seL4_PageBits), MASK(ks_رەقەم_CPUs));
    }

    أرجع EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */
