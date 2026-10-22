/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <kernel/boot.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <arch/kernel/boot.h>
#अन्तर्भाव <arch/kernel/boot_sys.h>
#अन्तर्भाव <arch/api/invocation.h>
#अन्तर्भाव <benchmark/benchmark_track.h>
#अन्तर्भाव <arch/kernel/tlb_bitmap.h>
#अन्तर्भाव <mode/kernel/tlb.h>

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

/* initialise the Task State Segment (TSS) */

BOOT_संहिता शून्य init_tss(tss_t *tss)
{
    *tss = tss_new(
               परिमाण(*tss),   /* io_map_base  */
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
    स्मृतिं_पूरय(&x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KStss.io_map[0], 0xff,
           परिमाण(x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KStss.io_map));
}
/* initialise Global Descriptor Table (GDT) */

BOOT_संहिता शून्य init_gdt(gdt_प्रविष्टिः_t *gdt, tss_t *tss)
{
    uint32_t tss_addr = (uint32_t)tss;

    /* Set the NULL descriptor */
    gdt[GDT_NULL] = gdt_प्रविष्टिः_gdt_null_new();

    /* 4GB flat kernel code segment on ring 0 descriptor */
    gdt[GDT_CS_0] = gdt_प्रविष्टिः_gdt_संहिता_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*उपलब्धम्*/
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
    gdt[GDT_DS_0] = gdt_प्रविष्टिः_gdt_दत्त_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*उपलब्धम्*/
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
    gdt[GDT_CS_3] = gdt_प्रविष्टिः_gdt_संहिता_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*उपलब्धम्*/
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
    gdt[GDT_DS_3] = gdt_प्रविष्टिः_gdt_दत्त_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*उपलब्धम्*/
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
    gdt[GDT_TSS] = gdt_प्रविष्टिः_gdt_tss_new(
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
                       परिमाण(tss_io_t) - 1         /* limit_low 16 bits    */
                   );

    gdt[GDT_FS] = gdt_प्रविष्टिः_gdt_दत्त_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*उपलब्धम्*/
                      0xf,    /* Segment limit high 4 bits    */
                      1,      /* Present                      */
                      3,      /* Descriptor privilege level   */
                      1,      /* writable                     */
                      1,      /* accessed                     */
                      0,      /* Base middle 8 bits           */
                      0,      /* Base low 16 bits             */
                      0xffff  /* Segment limit low 16 bits    */
                  );

    gdt[GDT_GS] = gdt_प्रविष्टिः_gdt_दत्त_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*उपलब्धम्*/
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

BOOT_संहिता शून्य init_idt_प्रविष्टिः(idt_प्रविष्टिः_t *idt, व्यवधानम्_t व्यवधानम्, शून्य(*handler)(शून्य))
{
    uint32_t handler_addr = (uint32_t)handler;
    uint32_t dpl = 3;

    यदि (व्यवधानम् < int_trap_min && व्यवधानम् != int_software_break_request) {
        dpl = 0;
    }

    idt[व्यवधानम्] = idt_प्रविष्टिः_व्यवधानम्_gate_new(
                         handler_addr >> 16,   /* offset_high  */
                         1,                    /* present      */
                         dpl,                  /* dpl          */
                         1,                    /* gate_size    */
                         SEL_CS_0,             /* seg_selector */
                         handler_addr & 0xffff /* offset_low   */
                     );
}

BOOT_संहिता bool_t map_kernel_गवाक्षः(
    uint32_t अङ्कः_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   अङ्कः_drhu,
    paddr_t   *drhu_list
)
{
    paddr_t  phys;
    uint32_t idx;
    pde_t    pde;
    pte_t    pte;
    अचिह्नित पूर्णाङ्क अप्रयुक्तम् i;

    /* Mapping of PPTR_BASE (virtual address) to kernel's PADDR_BASE
     * up to end of virtual address space except for the last large page.
     */
    phys = PADDR_आधारः;
    idx = PPTR_आधारः >> LARGE_PAGE_द्वयङ्काः;

    /* PPTR_TOP differs whether CONFIG_KERNEL_LOG_BUFFER
     * is enabled or not.
     */
    यावत् (idx < (PPTR_शिखरम् >> LARGE_PAGE_द्वयङ्काः)) {
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
        phys += BIT(LARGE_PAGE_द्वयङ्काः);
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    प्रतिज्ञा(phys == PADDR_शिखरम्);

#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
    /* Map global page table for the log buffer */
    pde = pde_pde_pt_new(
              kpptr_गन्तव्यम्_paddr(ia32KSGlobalLogPT), /* pt_base_address  */
              0,                 /* avl              */
              0,                 /* accessed         */
              0,                 /* cache_disabled   */
              0,                 /* write_through    */
              0,                 /* super_user       */
              1,                 /* read_write       */
              1                  /* present          */
          );

    ia32KSGlobalPD[idx] = pde;
    phys += BIT(LARGE_PAGE_द्वयङ्काः);
    प्रतिज्ञा(idx == (KS_LOG_PPTR >> LARGE_PAGE_द्वयङ्काः));
    idx++;
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(ia32KSGlobalPD);

    phys += TLBBITMAP_PD_RESERVED;
    idx += TLBBITMAP_मूलम्_ENTRIES;
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

    /* map page table of last 4M of virtual address space to page directory */
    pde = pde_pde_pt_new(
              kpptr_गन्तव्यम्_paddr(ia32KSGlobalPT), /* pt_base_address  */
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

    यावत् (idx < (KDEV_आधारः &आवरणम्(LARGE_PAGE_द्वयङ्काः)) >> PAGE_द्वयङ्काः) {
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
    यदि (!map_kernel_गवाक्षः_devices(ia32KSGlobalPT, अङ्कः_ioapic, ioapic_paddrs, अङ्कः_drhu, drhu_list)) {
        प्रत्यागम false;
    }

    invalidateLocalPageStructureCache();
    प्रत्यागम true;
}

/* Note: this function will invalidate any pointers previously returned from this function */
BOOT_संहिता शून्य *map_अस्थायी_boot_page(शून्य *प्रविष्टिः, uint32_t large_पृष्ठानि)
{
    शून्य *replacement_vaddr;
    अचिह्नित पूर्णाङ्क i;
    अचिह्नित पूर्णाङ्क विस्थापनम्_अन्तर्_page;

    अचिह्नित पूर्णाङ्क phys_pg_start = (अचिह्नित पूर्णाङ्क)(प्रविष्टिः) & ~आवरणम्(LARGE_PAGE_द्वयङ्काः);
    अचिह्नित पूर्णाङ्क virt_pd_start = (PPTR_आधारः >> LARGE_PAGE_द्वयङ्काः) - large_पृष्ठानि;
    अचिह्नित पूर्णाङ्क virt_pg_start = PPTR_आधारः - (large_पृष्ठानि << LARGE_PAGE_द्वयङ्काः);

    प्रति (i = 0; i < large_पृष्ठानि; i++) {
        अचिह्नित पूर्णाङ्क pg_विस्थापनम् = i << LARGE_PAGE_द्वयङ्काः; // num pages since start * page size

        *(get_boot_pd() + virt_pd_start + i) = pde_pde_large_new(
                                                   phys_pg_start + pg_विस्थापनम्, /* physical address */
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
        invalidateLocalTranslationSingle(virt_pg_start + pg_विस्थापनम्);
    }

    // assign replacement virtual addresses page
    विस्थापनम्_अन्तर्_page = (अचिह्नित पूर्णाङ्क)(प्रविष्टिः) & आवरणम्(LARGE_PAGE_द्वयङ्काः);
    replacement_vaddr = (शून्य *)(virt_pg_start + विस्थापनम्_अन्तर्_page);

    invalidateLocalPageStructureCache();

    प्रत्यागम replacement_vaddr;
}

/* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */

BOOT_संहिता शून्य init_dtrs(शून्य)
{
    /* setup the GDT pointer and limit and load into GDTR */
    gdt_idt_ptr.सीमा = (परिमाण(gdt_प्रविष्टिः_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.आधारः = (uint32_t)x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KSgdt;
    ia32_install_gdt(&gdt_idt_ptr);

    /* setup the IDT pointer and limit and load into IDTR */
    gdt_idt_ptr.सीमा = (परिमाण(idt_प्रविष्टिः_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.आधारः = (uint32_t)x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KSidt;
    ia32_install_idt(&gdt_idt_ptr);

    /* load NULL LDT selector into LDTR */
    ia32_install_ldt(SEL_NULL);

    /* load TSS selector into Task Register (TR) */
    ia32_install_tss(SEL_TSS);
}

स्थिर BOOT_संहिता cap_t create_it_page_सारणी_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_सारणी_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    यदि (asid != asidInvalid) {
        map_it_pt_cap(vspace_cap, cap);
    }
    प्रत्यागम cap;
}

स्थिर BOOT_संहिता cap_t create_it_page_directory_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_directory_cap_new(
              true,    /* capPDIsMapped   */
              IT_ASID, /* capPDMappedASID */
              vptr,    /* capPDMappedAddress */
              pptr  /* capPDBasePtr    */
          );
    यदि (asid != asidInvalid && cap_get_capType(vspace_cap) != cap_null_cap) {
        map_it_pd_cap(vspace_cap, cap);
    }
    प्रत्यागम cap;
}

BOOT_संहिता word_t arch_get_n_paging(v_region_t it_v_reg)
{
    word_t n = get_n_paging(it_v_reg, PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः);
#यदि_परिभाषितम् CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#यदि_अन्तः
    प्रत्यागम n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_संहिता cap_t create_it_स्थानाङ्कः_space(cap_t मूलम्_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    slot_pos_before = ndks_boot.slot_pos_cur;
    copyGlobalMappings((vspace_मूलम्_t *)rootserver.vspace);
    cap_t pd_cap = create_it_page_directory_cap(cap_null_cap_new(), rootserver.vspace, 0, IT_ASID);
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);
    vspace_cap = pd_cap;

    /* create all PT objs and caps necessary to cover userland image */

    प्रति (vptr = आवर्तनम्_अधस्तात्(it_v_reg.start, PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः);
         vptr < it_v_reg.अन्तः_2;
         vptr += BIT(PT_सूचकांक_द्वयङ्काः + PAGE_द्वयङ्काः)) {
        यदि (!provide_cap(मूलम्_cnode_cap,
                         create_it_page_सारणी_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            प्रत्यागम cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };

    प्रत्यागम vspace_cap;
}

स्थिर BOOT_संहिता cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           vm_page_map_type_t map_type)
{
    vm_page_आकार_t frame_आकार_2;

    यदि (use_large) {
        frame_आकार_2 = X86_LargePage;
    } अन्यथा {
        frame_आकार_2 = X86_SmallPage;
    }

    प्रत्यागम
        cap_frame_cap_new(
            frame_आकार_2,                    /* capFSize           */
            ASID_नीचः(asid),                /* capFMappedASIDLow  */
            vptr,                          /* capFMappedAddress  */
            map_type,                      /* capFMapType        */
            false,                         /* capFIsDevice       */
            ASID_उच्चः(asid),               /* capFMappedASIDHigh */
            wordFromVMRights(VMReadWrite), /* capFVMRights       */
            pptr                           /* capFBasePtr        */
        );
}

BOOT_संहिता cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    प्रत्यागम create_it_frame_cap(pptr, 0, asidInvalid, use_large, X86_MappingNone);
}

BOOT_संहिता cap_t create_mapped_it_frame_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable अप्रयुक्तम्)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large, X86_MappingVSpace);
    map_it_frame_cap(vspace_cap, cap);
    प्रत्यागम cap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

pde_t CONST makeUserPDELargePage(paddr_t paddr, vm_गुणाः_t vm_attr, vm_rights_t vm_rights)
{
    प्रत्यागम pde_pde_large_new(
               paddr,                                          /* page_base_address    */
               vm_गुणाः_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* avl                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_गुणाः_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_गुणाः_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_सारणी(paddr_t paddr, vm_गुणाः_t vm_attr)
{
    प्रत्यागम pde_pde_pt_new(
               paddr,                                      /* pt_base_address  */
               0,                                          /* avl              */
               0,                                          /* accessed         */
               vm_गुणाः_get_x86_PCDBit(vm_attr),       /* cache_disabled   */
               vm_गुणाः_get_x86_PWTBit(vm_attr),       /* write_through    */
               1,                                          /* super_user       */
               1,                                          /* read_write       */
               1                                           /* present          */
           );
}

pde_t CONST makeUserPDEInvalid(शून्य)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    प्रत्यागम pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_गुणाः_t vm_attr, vm_rights_t vm_rights)
{
    प्रत्यागम pte_new(
               paddr,                                          /* page_base_address    */
               0,                                              /* avl                  */
               0,                                              /* global               */
               vm_गुणाः_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_गुणाः_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_गुणाः_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}


pte_t CONST makeUserPTEInvalid(शून्य)
{
    प्रत्यागम pte_new(
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

शून्य समुच्चयः_VMRoot(tcb_t *tcb)
{
    cap_t               thread_मूलम्;
    vspace_मूलम्_t *vspace_मूलम्;
    asid_t              asid;
    findVSpaceForASID_ret_t find_ret;

    thread_मूलम् = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    vspace_मूलम् = get_Valid_Native_मूलम्(thread_मूलम्);
    यदि (!vspace_मूलम्) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_गन्तव्यम्_pptr(getCurrentPD()), getCurrentCPUIndex());)
        समुच्चयः_Current_PD(kpptr_गन्तव्यम्_paddr(ia32KSGlobalPD));
        प्रत्यागम;
    }

    asid = cap_get_capMappedASID(thread_मूलम्);
    find_ret = findVSpaceForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE || find_ret.vspace_मूलम् != vspace_मूलम्) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_गन्तव्यम्_pptr(getCurrentPD()), getCurrentCPUIndex());)
        समुच्चयः_Current_PD(kpptr_गन्तव्यम्_paddr(ia32KSGlobalPD));
        प्रत्यागम;
    }

    /* only set PD if we change it, otherwise we flush the TLB needlessly */
    यदि (getCurrentPD() != pptr_गन्तव्यम्_paddr(vspace_मूलम्)) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_गन्तव्यम्_pptr(getCurrentPD()), getCurrentCPUIndex());)
        SMP_COND_STATEMENT(tlb_bitmap_समुच्चयः(vspace_मूलम्, getCurrentCPUIndex());)

        समुच्चयः_Current_PD(pptr_गन्तव्यम्_paddr(vspace_मूलम्));
    }
}

शून्य hwASIDInvalidate(asid_t asid, vspace_मूलम्_t *vspace)
{
    /* 32-bit does not have PCID */
    प्रत्यागम;
}

exception_t decode_X86_बहुलकम्_MMUInvocation(
    word_t invLabel,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_directory_cap:
        प्रत्यागम decodeIA32PageDirectoryInvocation(invLabel, दीर्घता, cte, cap, call, बफर);

    स्वभाव:
        fail("Invalid arch cap type");
    }
}


bool_t बहुलकम्_Unmap_Page(vm_page_आकार_t page_आकार, vspace_मूलम्_t *vroot, vptr_t vaddr, शून्य *pptr)
{
    fail("Invalid page type");
    प्रत्यागम false;
}

exception_t decode_X86_बहुलकम्_Map_Page(word_t invLabel, vm_page_आकार_t page_आकार, cte_t *cte, cap_t cap,
                                 vspace_मूलम्_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_गुणाः_t vm_attr)
{
    fail("Invalid Page type");
}

#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
exception_t benchmark_arch_map_log_बफर(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_आकार_t frame_आकार;
    pptr_t frame_pptr;

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

    यदि (frame_आकार != X86_LargePage) {
        userError("Invalid size for log Buffer. The kernel expects at least 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ks_User_Log_बफर = pptr_गन्तव्यम्_paddr((शून्य *) frame_pptr);

    /* fill global log page table with mappings */
    प्रति (पूर्णाङ्क idx = 0; idx < BIT(PT_सूचकांक_द्वयङ्काः); idx++) {
        paddr_t physical_स्थानाङ्कः = ks_User_Log_बफर + (idx << se_L4_Page_द्वयङ्काः);

        pte_t pte = pte_new(
                        physical_स्थानाङ्कः,   /* page_base_address    */
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
        invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_Page_द्वयङ्काः), आवरणम्(ks_अङ्कः_CPUs));
    }

    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */
