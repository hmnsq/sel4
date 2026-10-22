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

BOOT_ڪوڊ فراغ init_tss(tss_t *tss)
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
    memset(&x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KStss.io_map[0], 0xff,
           حجم(x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KStss.io_map));
}
/* initialise Global Descriptor Table (GDT) */

BOOT_ڪوڊ فراغ init_gdt(gdt_داخلا_t *gdt, tss_t *tss)
{
    uint32_t tss_addr = (uint32_t)tss;

    /* Set the NULL descriptor */
    gdt[GDT_NULL] = gdt_داخلا_gdt_null_new();

    /* 4GB flat kernel code segment on ring 0 descriptor */
    gdt[GDT_CS_0] = gdt_داخلا_gdt_ڪوڊ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*موجود*/
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
    gdt[GDT_DS_0] = gdt_داخلا_gdt_معلومات_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*موجود*/
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
    gdt[GDT_CS_3] = gdt_داخلا_gdt_ڪوڊ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*موجود*/
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
    gdt[GDT_DS_3] = gdt_داخلا_gdt_معلومات_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*موجود*/
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
    gdt[GDT_TSS] = gdt_داخلا_gdt_tss_new(
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

    gdt[GDT_FS] = gdt_داخلا_gdt_معلومات_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*موجود*/
                      0xf,    /* Segment limit high 4 bits    */
                      1,      /* Present                      */
                      3,      /* Descriptor privilege level   */
                      1,      /* writable                     */
                      1,      /* accessed                     */
                      0,      /* Base middle 8 bits           */
                      0,      /* Base low 16 bits             */
                      0xffff  /* Segment limit low 16 bits    */
                  );

    gdt[GDT_GS] = gdt_داخلا_gdt_معلومات_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*موجود*/
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

BOOT_ڪوڊ فراغ init_idt_داخلا(idt_داخلا_t *idt, مداخلت_t مداخلت, فراغ(*handler)(فراغ))
{
    uint32_t handler_addr = (uint32_t)handler;
    uint32_t dpl = 3;

    إذا (مداخلت < int_trap_min && مداخلت != int_software_break_request) {
        dpl = 0;
    }

    idt[مداخلت] = idt_داخلا_مداخلت_gate_new(
                         handler_addr >> 16,   /* offset_high  */
                         1,                    /* present      */
                         dpl,                  /* dpl          */
                         1,                    /* gate_size    */
                         SEL_CS_0,             /* seg_selector */
                         handler_addr & 0xffff /* offset_low   */
                     );
}

BOOT_ڪوڊ bool_t map_kernel_دري(
    uint32_t انگ_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   انگ_drhu,
    paddr_t   *drhu_list
)
{
    paddr_t  phys;
    uint32_t idx;
    pde_t    pde;
    pte_t    pte;
    بنانشان صحيح اڻ_استعمال_ٿيل i;

    /* Mapping of PPTR_BASE (virtual address) to kernel's PADDR_BASE
     * up to end of virtual address space except for the last large page.
     */
    phys = PADDR_بنياد;
    idx = PPTR_بنياد >> LARGE_صفحو_بٽ;

    /* PPTR_TOP differs whether CONFIG_KERNEL_LOG_BUFFER
     * is enabled or not.
     */
    طالما (idx < (PPTR_چوٽي >> LARGE_صفحو_بٽ)) {
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
        phys += ٻٽو_انگ(LARGE_صفحو_بٽ);
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    أكد(phys == PADDR_چوٽي);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
    /* Map global page table for the log buffer */
    pde = pde_pde_pt_new(
              kpptr_منزل_paddr(ia32KSGlobalLogPT), /* pt_base_address  */
              0,                 /* avl              */
              0,                 /* accessed         */
              0,                 /* cache_disabled   */
              0,                 /* write_through    */
              0,                 /* super_user       */
              1,                 /* read_write       */
              1                  /* present          */
          );

    ia32KSGlobalPD[idx] = pde;
    phys += ٻٽو_انگ(LARGE_صفحو_بٽ);
    أكد(idx == (KS_LOG_PPTR >> LARGE_صفحو_بٽ));
    idx++;
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(ia32KSGlobalPD);

    phys += TLBBITMAP_PD_RESERVED;
    idx += TLBBITMAP_پاڙ_ENTRIES;
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    /* map page table of last 4M of virtual address space to page directory */
    pde = pde_pde_pt_new(
              kpptr_منزل_paddr(ia32KSGlobalPT), /* pt_base_address  */
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

    طالما (idx < (KDEV_بنياد &نقاب(LARGE_صفحو_بٽ)) >> صفحو_بٽ) {
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
    إذا (!map_kernel_دري_devices(ia32KSGlobalPT, انگ_ioapic, ioapic_paddrs, انگ_drhu, drhu_list)) {
        أرجع false;
    }

    invalidate_Local_صفحو_Structure_Cache();
    أرجع true;
}

/* Note: this function will invalidate any pointers previously returned from this function */
BOOT_ڪوڊ فراغ *map_عارضي_boot_صفحو(فراغ *داخلا, uint32_t large_صفحا)
{
    فراغ *replacement_vaddr;
    بنانشان صحيح i;
    بنانشان صحيح وٿي_اندريون_صفحو;

    بنانشان صحيح phys_pg_شروعات = (بنانشان صحيح)(داخلا) & ~نقاب(LARGE_صفحو_بٽ);
    بنانشان صحيح virt_pd_شروعات = (PPTR_بنياد >> LARGE_صفحو_بٽ) - large_صفحا;
    بنانشان صحيح virt_pg_شروعات = PPTR_بنياد - (large_صفحا << LARGE_صفحو_بٽ);

    لكل (i = 0; i < large_صفحا; i++) {
        بنانشان صحيح pg_وٿي = i << LARGE_صفحو_بٽ; // num pages since start * page size

        *(get_boot_pd() + virt_pd_شروعات + i) = pde_pde_large_new(
                                                   phys_pg_شروعات + pg_وٿي, /* physical address */
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
        invalidateLocalTranslationSingle(virt_pg_شروعات + pg_وٿي);
    }

    // assign replacement virtual addresses page
    وٿي_اندريون_صفحو = (بنانشان صحيح)(داخلا) & نقاب(LARGE_صفحو_بٽ);
    replacement_vaddr = (فراغ *)(virt_pg_شروعات + وٿي_اندريون_صفحو);

    invalidate_Local_صفحو_Structure_Cache();

    أرجع replacement_vaddr;
}

/* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */

BOOT_ڪوڊ فراغ init_dtrs(فراغ)
{
    /* setup the GDT pointer and limit and load into GDTR */
    gdt_idt_ptr.حد_بندي = (حجم(gdt_داخلا_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.بنياد = (uint32_t)x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KSgdt;
    ia32_install_gdt(&gdt_idt_ptr);

    /* setup the IDT pointer and limit and load into IDTR */
    gdt_idt_ptr.حد_بندي = (حجم(idt_داخلا_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.بنياد = (uint32_t)x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KSidt;
    ia32_install_idt(&gdt_idt_ptr);

    /* load NULL LDT selector into LDTR */
    ia32_install_ldt(SEL_NULL);

    /* load TSS selector into Task Register (TR) */
    ia32_install_tss(SEL_TSS);
}

ساكن BOOT_ڪوڊ cap_t create_it_صفحو_جدول_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحو_جدول_cap_new(
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

ساكن BOOT_ڪوڊ cap_t create_it_صفحو_directory_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحو_directory_cap_new(
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

BOOT_ڪوڊ لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    لفظ_t n = get_n_paging(it_v_reg, PT_اشاريو_بٽ + صفحو_بٽ);
#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#جيڪڏهن_پورو
    أرجع n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_ڪوڊ cap_t create_it_پتو_space(cap_t پاڙ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_جڳهه_Pos جڳهه_pos_before;
    se_L4_جڳهه_Pos جڳهه_pos_after;

    جڳهه_pos_before = ndks_boot.جڳهه_pos_cur;
    copyGlobalMappings((vspace_پاڙ_t *)rootserver.vspace);
    cap_t pd_cap = create_it_صفحو_directory_cap(cap_null_cap_new(), rootserver.vspace, 0, IT_ASID);
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);
    vspace_cap = pd_cap;

    /* create all PT objs and caps necessary to cover userland image */

    لكل (vptr = ڦيرو_هيٺ_ڏانهن(it_v_reg.شروعات, PT_اشاريو_بٽ + صفحو_بٽ);
         vptr < it_v_reg.پڇاڙي;
         vptr += ٻٽو_انگ(PT_اشاريو_بٽ + صفحو_بٽ)) {
        إذا (!provide_cap(پاڙ_cnode_cap,
                         create_it_صفحو_جدول_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    جڳهه_pos_after = ndks_boot.جڳهه_pos_cur;
    ndks_boot.bi_چوکٺو->userImagePaging = (se_L4_جڳهه_Region) {
        جڳهه_pos_before, جڳهه_pos_after
    };

    أرجع vspace_cap;
}

ساكن BOOT_ڪوڊ cap_t create_it_چوکٺو_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           vm_صفحو_map_type_t map_type)
{
    vm_صفحو_سائيز_t چوکٺو_سائيز_2;

    إذا (use_large) {
        چوکٺو_سائيز_2 = X86_Large_صفحو;
    } وإلا {
        چوکٺو_سائيز_2 = X86_Small_صفحو;
    }

    أرجع
        cap_چوکٺو_cap_new(
            چوکٺو_سائيز_2,                    /* capFSize           */
            ASID_هيٺيون(asid),                /* capFMappedASIDLow  */
            vptr,                          /* capFMappedAddress  */
            map_type,                      /* capFMapType        */
            false,                         /* capFIsDevice       */
            ASID_مٿاهون(asid),               /* capFMappedASIDHigh */
            لفظ_From_VMRights(VMReadWrite), /* capFVMRights       */
            pptr                           /* capFBasePtr        */
        );
}

BOOT_ڪوڊ cap_t create_unmapped_it_چوکٺو_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_چوکٺو_cap(pptr, 0, asidInvalid, use_large, X86_MappingNone);
}

BOOT_ڪوڊ cap_t create_mapped_it_چوکٺو_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable اڻ_استعمال_ٿيل)
{
    cap_t cap = create_it_چوکٺو_cap(pptr, vptr, asid, use_large, X86_MappingVSpace);
    map_it_چوکٺو_cap(vspace_cap, cap);
    أرجع cap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

pde_t CONST make_User_PDELarge_صفحو(paddr_t paddr, vm_خاصيتون_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pde_pde_large_new(
               paddr,                                          /* page_base_address    */
               vm_خاصيتون_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* avl                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_خاصيتون_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_خاصيتون_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_جدول(paddr_t paddr, vm_خاصيتون_t vm_attr)
{
    أرجع pde_pde_pt_new(
               paddr,                                      /* pt_base_address  */
               0,                                          /* avl              */
               0,                                          /* accessed         */
               vm_خاصيتون_get_x86_PCDBit(vm_attr),       /* cache_disabled   */
               vm_خاصيتون_get_x86_PWTBit(vm_attr),       /* write_through    */
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_خاصيتون_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pte_new(
               paddr,                                          /* page_base_address    */
               0,                                              /* avl                  */
               0,                                              /* global               */
               vm_خاصيتون_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_خاصيتون_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_خاصيتون_get_x86_PWTBit(vm_attr),           /* write_through        */
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

فراغ سيٽ_VMRoot(tcb_t *tcb)
{
    cap_t               thread_پاڙ;
    vspace_پاڙ_t *vspace_پاڙ;
    asid_t              asid;
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;

    thread_پاڙ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    vspace_پاڙ = get_Valid_Native_پاڙ(thread_پاڙ);
    إذا (!vspace_پاڙ) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_منزل_pptr(getCurrentPD()), getCurrentCPUIndex());)
        سيٽ_Current_PD(kpptr_منزل_paddr(ia32KSGlobalPD));
        أرجع;
    }

    asid = cap_get_capMappedASID(thread_پاڙ);
    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (ڳوليو_ret.حالت != EXCEPTION_NONE || ڳوليو_ret.vspace_پاڙ != vspace_پاڙ) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_منزل_pptr(getCurrentPD()), getCurrentCPUIndex());)
        سيٽ_Current_PD(kpptr_منزل_paddr(ia32KSGlobalPD));
        أرجع;
    }

    /* only set PD if we change it, otherwise we flush the TLB needlessly */
    إذا (getCurrentPD() != pptr_منزل_paddr(vspace_پاڙ)) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_منزل_pptr(getCurrentPD()), getCurrentCPUIndex());)
        SMP_COND_STATEMENT(tlb_bitmap_سيٽ(vspace_پاڙ, getCurrentCPUIndex());)

        سيٽ_Current_PD(pptr_منزل_paddr(vspace_پاڙ));
    }
}

فراغ hwASIDInvalidate(asid_t asid, vspace_پاڙ_t *vspace)
{
    /* 32-bit does not have PCID */
    أرجع;
}

exception_t decode_X86_گهڻائي_MMUInvocation(
    لفظ_t invLabel,
    لفظ_t ڊيگهائي,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_صفحو_directory_cap:
        أرجع decode_IA32_صفحو_Directory_Invocation(invLabel, ڊيگهائي, cte, cap, call, بفر);

    افتراضي:
        fail("Invalid arch cap type");
    }
}


bool_t گهڻائي_Unmap_صفحو(vm_صفحو_سائيز_t صفحو_سائيز, vspace_پاڙ_t *vroot, vptr_t vaddr, فراغ *pptr)
{
    fail("Invalid page type");
    أرجع false;
}

exception_t decode_X86_گهڻائي_Map_صفحو(لفظ_t invLabel, vm_صفحو_سائيز_t صفحو_سائيز, cte_t *cte, cap_t cap,
                                 vspace_پاڙ_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_خاصيتون_t vm_attr)
{
    fail("Invalid Page type");
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
exception_t benchmark_arch_map_log_بفر(لفظ_t چوکٺو_cptr)
{
    lookup_Cap_And_جڳهه_ret_t lu_ret;
    vm_صفحو_سائيز_t چوکٺو_سائيز;
    pptr_t چوکٺو_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_جڳهه(ڳنڍ_STATE(ksCurThread), چوکٺو_cptr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکٺو_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکٺو_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_چوکٺو_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکٺو_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکٺو_سائيز = cap_چوکٺو_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکٺو_سائيز != X86_Large_صفحو) {
        userError("Invalid size for log Buffer. The kernel expects at least 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکٺو_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکٺو_pptr = cap_چوکٺو_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکٺو_pptr);

    /* fill global log page table with mappings */
    لكل (صحيح idx = 0; idx < ٻٽو_انگ(PT_اشاريو_بٽ); idx++) {
        paddr_t physical_پتو = ks_User_Log_بفر + (idx << se_L4_صفحو_بٽ);

        pte_t pte = pte_new(
                        physical_پتو,   /* page_base_address    */
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
        invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_صفحو_بٽ), نقاب(ks_انگ_CPUs));
    }

    أرجع EXCEPTION_NONE;
}
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */
