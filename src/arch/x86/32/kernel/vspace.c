/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <arch/kernel/boot.h>
#అన్తర్భావ <arch/kernel/boot_sys.h>
#అన్తర్భావ <arch/api/invocation.h>
#అన్తర్భావ <benchmark/benchmark_track.h>
#అన్తర్భావ <arch/kernel/tlb_bitmap.h>
#అన్తర్భావ <mode/kernel/tlb.h>

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

/* initialise the Task State Segment (TSS) */

BOOT_సంకేతం శూన్య init_tss(tss_t *tss)
{
    *tss = tss_new(
               పరిమాణం(*tss),   /* io_map_base  */
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
    memset(&x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KStss.io_map[0], 0xff,
           పరిమాణం(x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KStss.io_map));
}
/* initialise Global Descriptor Table (GDT) */

BOOT_సంకేతం శూన్య init_gdt(gdt_నమోదు_t *gdt, tss_t *tss)
{
    uint32_t tss_addr = (uint32_t)tss;

    /* Set the NULL descriptor */
    gdt[GDT_NULL] = gdt_నమోదు_gdt_null_new();

    /* 4GB flat kernel code segment on ring 0 descriptor */
    gdt[GDT_CS_0] = gdt_నమోదు_gdt_సంకేతం_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*అందుబాటు*/
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
    gdt[GDT_DS_0] = gdt_నమోదు_gdt_దత్తాంశం_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*అందుబాటు*/
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
    gdt[GDT_CS_3] = gdt_నమోదు_gdt_సంకేతం_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*అందుబాటు*/
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
    gdt[GDT_DS_3] = gdt_నమోదు_gdt_దత్తాంశం_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*అందుబాటు*/
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
    gdt[GDT_TSS] = gdt_నమోదు_gdt_tss_new(
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
                       పరిమాణం(tss_io_t) - 1         /* limit_low 16 bits    */
                   );

    gdt[GDT_FS] = gdt_నమోదు_gdt_దత్తాంశం_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*అందుబాటు*/
                      0xf,    /* Segment limit high 4 bits    */
                      1,      /* Present                      */
                      3,      /* Descriptor privilege level   */
                      1,      /* writable                     */
                      1,      /* accessed                     */
                      0,      /* Base middle 8 bits           */
                      0,      /* Base low 16 bits             */
                      0xffff  /* Segment limit low 16 bits    */
                  );

    gdt[GDT_GS] = gdt_నమోదు_gdt_దత్తాంశం_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*అందుబాటు*/
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

BOOT_సంకేతం శూన్య init_idt_నమోదు(idt_నమోదు_t *idt, అంతరాయం_t అంతరాయం, శూన్య(*handler)(శూన్య))
{
    uint32_t handler_addr = (uint32_t)handler;
    uint32_t dpl = 3;

    ఒకవేళ (అంతరాయం < int_trap_min && అంతరాయం != int_software_break_request) {
        dpl = 0;
    }

    idt[అంతరాయం] = idt_నమోదు_అంతరాయం_gate_new(
                         handler_addr >> 16,   /* offset_high  */
                         1,                    /* present      */
                         dpl,                  /* dpl          */
                         1,                    /* gate_size    */
                         SEL_CS_0,             /* seg_selector */
                         handler_addr & 0xffff /* offset_low   */
                     );
}

BOOT_సంకేతం bool_t map_kernel_కిటికీ(
    uint32_t అంకె_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   అంకె_drhu,
    paddr_t   *drhu_list
)
{
    paddr_t  phys;
    uint32_t idx;
    pde_t    pde;
    pte_t    pte;
    గుర్తులేని పూర్ణాంకం వాడని i;

    /* Mapping of PPTR_BASE (virtual address) to kernel's PADDR_BASE
     * up to end of virtual address space except for the last large page.
     */
    phys = PADDR_పునాది;
    idx = PPTR_పునాది >> LARGE_పుట_బిట్లు;

    /* PPTR_TOP differs whether CONFIG_KERNEL_LOG_BUFFER
     * is enabled or not.
     */
    అయితే (idx < (PPTR_శిఖరం >> LARGE_పుట_బిట్లు)) {
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
        phys += బిట్టు(LARGE_పుట_బిట్లు);
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    ప్రతిజ్ఞా(phys == PADDR_శిఖరం);

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
    /* Map global page table for the log buffer */
    pde = pde_pde_pt_new(
              kpptr_గమ్యం_paddr(ia32KSGlobalLogPT), /* pt_base_address  */
              0,                 /* avl              */
              0,                 /* accessed         */
              0,                 /* cache_disabled   */
              0,                 /* write_through    */
              0,                 /* super_user       */
              1,                 /* read_write       */
              1                  /* present          */
          );

    ia32KSGlobalPD[idx] = pde;
    phys += బిట్టు(LARGE_పుట_బిట్లు);
    ప్రతిజ్ఞా(idx == (KS_LOG_PPTR >> LARGE_పుట_బిట్లు));
    idx++;
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(ia32KSGlobalPD);

    phys += TLBBITMAP_PD_RESERVED;
    idx += TLBBITMAP_మూలం_ENTRIES;
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    /* map page table of last 4M of virtual address space to page directory */
    pde = pde_pde_pt_new(
              kpptr_గమ్యం_paddr(ia32KSGlobalPT), /* pt_base_address  */
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

    అయితే (idx < (KDEV_పునాది &ముసుగు(LARGE_పుట_బిట్లు)) >> పుట_బిట్లు) {
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
    ఒకవేళ (!map_kernel_కిటికీ_devices(ia32KSGlobalPT, అంకె_ioapic, ioapic_paddrs, అంకె_drhu, drhu_list)) {
        తిరిగి false;
    }

    invalidate_Local_పుట_Structure_Cache();
    తిరిగి true;
}

/* Note: this function will invalidate any pointers previously returned from this function */
BOOT_సంకేతం శూన్య *map_తాత్కాలిక_boot_పుట(శూన్య *నమోదు_2, uint32_t large_పుటలు)
{
    శూన్య *replacement_vaddr;
    గుర్తులేని పూర్ణాంకం i;
    గుర్తులేని పూర్ణాంకం స్థానభ్రంశం_లోపల_పుట;

    గుర్తులేని పూర్ణాంకం phys_pg_ప్రారంభం = (గుర్తులేని పూర్ణాంకం)(నమోదు_2) & ~ముసుగు(LARGE_పుట_బిట్లు);
    గుర్తులేని పూర్ణాంకం virt_pd_ప్రారంభం = (PPTR_పునాది >> LARGE_పుట_బిట్లు) - large_పుటలు;
    గుర్తులేని పూర్ణాంకం virt_pg_ప్రారంభం = PPTR_పునాది - (large_పుటలు << LARGE_పుట_బిట్లు);

    కొరకు (i = 0; i < large_పుటలు; i++) {
        గుర్తులేని పూర్ణాంకం pg_స్థానభ్రంశం = i << LARGE_పుట_బిట్లు; // num pages since start * page size

        *(get_boot_pd() + virt_pd_ప్రారంభం + i) = pde_pde_large_new(
                                                   phys_pg_ప్రారంభం + pg_స్థానభ్రంశం, /* physical address */
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
        invalidateLocalTranslationSingle(virt_pg_ప్రారంభం + pg_స్థానభ్రంశం);
    }

    // assign replacement virtual addresses page
    స్థానభ్రంశం_లోపల_పుట = (గుర్తులేని పూర్ణాంకం)(నమోదు_2) & ముసుగు(LARGE_పుట_బిట్లు);
    replacement_vaddr = (శూన్య *)(virt_pg_ప్రారంభం + స్థానభ్రంశం_లోపల_పుట);

    invalidate_Local_పుట_Structure_Cache();

    తిరిగి replacement_vaddr;
}

/* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */

BOOT_సంకేతం శూన్య init_dtrs(శూన్య)
{
    /* setup the GDT pointer and limit and load into GDTR */
    gdt_idt_ptr.హద్దు = (పరిమాణం(gdt_నమోదు_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.పునాది = (uint32_t)x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KSgdt;
    ia32_install_gdt(&gdt_idt_ptr);

    /* setup the IDT pointer and limit and load into IDTR */
    gdt_idt_ptr.హద్దు = (పరిమాణం(idt_నమోదు_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.పునాది = (uint32_t)x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KSidt;
    ia32_install_idt(&gdt_idt_ptr);

    /* load NULL LDT selector into LDTR */
    ia32_install_ldt(SEL_NULL);

    /* load TSS selector into Task Register (TR) */
    ia32_install_tss(SEL_TSS);
}

స్థిర BOOT_సంకేతం cap_t create_it_పుట_పట్టిక_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_పుట_పట్టిక_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    ఒకవేళ (asid != asidInvalid) {
        map_it_pt_cap(vspace_cap, cap);
    }
    తిరిగి cap;
}

స్థిర BOOT_సంకేతం cap_t create_it_పుట_directory_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_పుట_directory_cap_new(
              true,    /* capPDIsMapped   */
              IT_ASID, /* capPDMappedASID */
              vptr,    /* capPDMappedAddress */
              pptr  /* capPDBasePtr    */
          );
    ఒకవేళ (asid != asidInvalid && cap_get_capType(vspace_cap) != cap_null_cap) {
        map_it_pd_cap(vspace_cap, cap);
    }
    తిరిగి cap;
}

BOOT_సంకేతం మాట_t arch_get_n_paging(v_region_t it_v_reg)
{
    మాట_t n = get_n_paging(it_v_reg, PT_సూచీ_బిట్లు + పుట_బిట్లు);
#నిర్వచితమైతే CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#అయితే_ముగింపు
    తిరిగి n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_సంకేతం cap_t create_it_చిరునామా_space(cap_t మూలం_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_స్థానం_Pos స్థానం_pos_before;
    se_L4_స్థానం_Pos స్థానం_pos_after;

    స్థానం_pos_before = ndks_boot.స్థానం_pos_cur;
    copyGlobalMappings((vspace_మూలం_t *)rootserver.vspace);
    cap_t pd_cap = create_it_పుట_directory_cap(cap_null_cap_new(), rootserver.vspace, 0, IT_ASID);
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);
    vspace_cap = pd_cap;

    /* create all PT objs and caps necessary to cover userland image */

    కొరకు (vptr = విడత_కిందికి(it_v_reg.ప్రారంభం, PT_సూచీ_బిట్లు + పుట_బిట్లు);
         vptr < it_v_reg.చివర;
         vptr += బిట్టు(PT_సూచీ_బిట్లు + పుట_బిట్లు)) {
        ఒకవేళ (!provide_cap(మూలం_cnode_cap,
                         create_it_పుట_పట్టిక_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            తిరిగి cap_null_cap_new();
        }
    }

    స్థానం_pos_after = ndks_boot.స్థానం_pos_cur;
    ndks_boot.bi_చట్రం->userImagePaging = (se_L4_స్థానం_Region) {
        స్థానం_pos_before, స్థానం_pos_after
    };

    తిరిగి vspace_cap;
}

స్థిర BOOT_సంకేతం cap_t create_it_చట్రం_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           vm_పుట_map_type_t map_type)
{
    vm_పుట_కొలత_t చట్రం_కొలత_2;

    ఒకవేళ (use_large) {
        చట్రం_కొలత_2 = X86_Large_పుట;
    } లేకపోతే {
        చట్రం_కొలత_2 = X86_Small_పుట;
    }

    తిరిగి
        cap_చట్రం_cap_new(
            చట్రం_కొలత_2,                    /* capFSize           */
            ASID_దిగువ(asid),                /* capFMappedASIDLow  */
            vptr,                          /* capFMappedAddress  */
            map_type,                      /* capFMapType        */
            false,                         /* capFIsDevice       */
            ASID_ఎగువ(asid),               /* capFMappedASIDHigh */
            మాట_From_VMRights(VMReadWrite), /* capFVMRights       */
            pptr                           /* capFBasePtr        */
        );
}

BOOT_సంకేతం cap_t create_unmapped_it_చట్రం_cap(pptr_t pptr, bool_t use_large)
{
    తిరిగి create_it_చట్రం_cap(pptr, 0, asidInvalid, use_large, X86_MappingNone);
}

BOOT_సంకేతం cap_t create_mapped_it_చట్రం_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable వాడని)
{
    cap_t cap = create_it_చట్రం_cap(pptr, vptr, asid, use_large, X86_MappingVSpace);
    map_it_చట్రం_cap(vspace_cap, cap);
    తిరిగి cap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

pde_t CONST make_User_PDELarge_పుట(paddr_t paddr, vm_లక్షణాలు_t vm_attr, vm_rights_t vm_rights)
{
    తిరిగి pde_pde_large_new(
               paddr,                                          /* page_base_address    */
               vm_లక్షణాలు_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* avl                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_లక్షణాలు_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_లక్షణాలు_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_పట్టిక(paddr_t paddr, vm_లక్షణాలు_t vm_attr)
{
    తిరిగి pde_pde_pt_new(
               paddr,                                      /* pt_base_address  */
               0,                                          /* avl              */
               0,                                          /* accessed         */
               vm_లక్షణాలు_get_x86_PCDBit(vm_attr),       /* cache_disabled   */
               vm_లక్షణాలు_get_x86_PWTBit(vm_attr),       /* write_through    */
               1,                                          /* super_user       */
               1,                                          /* read_write       */
               1                                           /* present          */
           );
}

pde_t CONST makeUserPDEInvalid(శూన్య)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    తిరిగి pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_లక్షణాలు_t vm_attr, vm_rights_t vm_rights)
{
    తిరిగి pte_new(
               paddr,                                          /* page_base_address    */
               0,                                              /* avl                  */
               0,                                              /* global               */
               vm_లక్షణాలు_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_లక్షణాలు_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_లక్షణాలు_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}


pte_t CONST makeUserPTEInvalid(శూన్య)
{
    తిరిగి pte_new(
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

శూన్య సమితి_VMRoot(tcb_t *tcb)
{
    cap_t               thread_మూలం;
    vspace_మూలం_t *vspace_మూలం;
    asid_t              asid;
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;

    thread_మూలం = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    vspace_మూలం = get_Valid_Native_మూలం(thread_మూలం);
    ఒకవేళ (!vspace_మూలం) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_గమ్యం_pptr(getCurrentPD()), getCurrentCPUIndex());)
        సమితి_Current_PD(kpptr_గమ్యం_paddr(ia32KSGlobalPD));
        తిరిగి;
    }

    asid = cap_get_capMappedASID(thread_మూలం);
    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE || కనుగొను_ret.vspace_మూలం != vspace_మూలం) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_గమ్యం_pptr(getCurrentPD()), getCurrentCPUIndex());)
        సమితి_Current_PD(kpptr_గమ్యం_paddr(ia32KSGlobalPD));
        తిరిగి;
    }

    /* only set PD if we change it, otherwise we flush the TLB needlessly */
    ఒకవేళ (getCurrentPD() != pptr_గమ్యం_paddr(vspace_మూలం)) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_గమ్యం_pptr(getCurrentPD()), getCurrentCPUIndex());)
        SMP_COND_STATEMENT(tlb_bitmap_సమితి(vspace_మూలం, getCurrentCPUIndex());)

        సమితి_Current_PD(pptr_గమ్యం_paddr(vspace_మూలం));
    }
}

శూన్య hwASIDInvalidate(asid_t asid, vspace_మూలం_t *vspace)
{
    /* 32-bit does not have PCID */
    తిరిగి;
}

exception_t decode_X86_బహుళకం_MMUInvocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    మాట_t *బఫరు
)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_పుట_directory_cap:
        తిరిగి decode_IA32_పుట_Directory_Invocation(invLabel, నిడివి, cte, cap, call, బఫరు);

    అప్రమేయ:
        fail("Invalid arch cap type");
    }
}


bool_t బహుళకం_Unmap_పుట(vm_పుట_కొలత_t పుట_కొలత, vspace_మూలం_t *vroot, vptr_t vaddr, శూన్య *pptr)
{
    fail("Invalid page type");
    తిరిగి false;
}

exception_t decode_X86_బహుళకం_Map_పుట(మాట_t invLabel, vm_పుట_కొలత_t పుట_కొలత, cte_t *cte, cap_t cap,
                                 vspace_మూలం_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_లక్షణాలు_t vm_attr)
{
    fail("Invalid Page type");
}

#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
exception_t benchmark_arch_map_log_బఫరు(మాట_t చట్రం_cptr)
{
    lookup_Cap_And_స్థానం_ret_t lu_ret;
    vm_పుట_కొలత_t చట్రం_కొలత;
    pptr_t చట్రం_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_స్థానం(కణుపు_STATE(ksCurThread), చట్రం_cptr);

    ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", చట్రం_cptr);
        current_fault = seL4_Fault_CapFault_new(చట్రం_cptr, false);

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_get_capType(lu_ret.cap) != cap_చట్రం_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(చట్రం_cptr, false);

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    చట్రం_కొలత = cap_చట్రం_cap_get_cap_FSize(lu_ret.cap);

    ఒకవేళ (చట్రం_కొలత != X86_Large_పుట) {
        userError("Invalid size for log Buffer. The kernel expects at least 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(చట్రం_cptr, false);

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    చట్రం_pptr = cap_చట్రం_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_బఫరు = pptr_గమ్యం_paddr((శూన్య *) చట్రం_pptr);

    /* fill global log page table with mappings */
    కొరకు (పూర్ణాంకం idx = 0; idx < బిట్టు(PT_సూచీ_బిట్లు); idx++) {
        paddr_t physical_చిరునామా = ks_User_Log_బఫరు + (idx << se_L4_పుట_బిట్లు);

        pte_t pte = pte_new(
                        physical_చిరునామా,   /* page_base_address    */
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
        invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_పుట_బిట్లు), ముసుగు(ks_అంకె_CPUs));
    }

    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */
