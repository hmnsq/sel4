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

BOOT_کوډ فراغ init_tss(tss_t *tss)
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
    memset(&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss.io_map[0], 0xff,
           حجم(x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss.io_map));
}
/* initialise Global Descriptor Table (GDT) */

BOOT_کوډ فراغ init_gdt(gdt_ثبت_t *gdt, tss_t *tss)
{
    uint32_t tss_addr = (uint32_t)tss;

    /* Set the NULL descriptor */
    gdt[GDT_NULL] = gdt_ثبت_gdt_null_new();

    /* 4GB flat kernel code segment on ring 0 descriptor */
    gdt[GDT_CS_0] = gdt_ثبت_gdt_کوډ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*شته*/
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
    gdt[GDT_DS_0] = gdt_ثبت_gdt_معلومات_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*شته*/
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
    gdt[GDT_CS_3] = gdt_ثبت_gdt_کوډ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*شته*/
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
    gdt[GDT_DS_3] = gdt_ثبت_gdt_معلومات_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*شته*/
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
    gdt[GDT_TSS] = gdt_ثبت_gdt_tss_new(
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

    gdt[GDT_FS] = gdt_ثبت_gdt_معلومات_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*شته*/
                      0xf,    /* Segment limit high 4 bits    */
                      1,      /* Present                      */
                      3,      /* Descriptor privilege level   */
                      1,      /* writable                     */
                      1,      /* accessed                     */
                      0,      /* Base middle 8 bits           */
                      0,      /* Base low 16 bits             */
                      0xffff  /* Segment limit low 16 bits    */
                  );

    gdt[GDT_GS] = gdt_ثبت_gdt_معلومات_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*شته*/
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

BOOT_کوډ فراغ init_idt_ثبت(idt_ثبت_t *idt, مداخله_t مداخله, فراغ(*handler)(فراغ))
{
    uint32_t handler_addr = (uint32_t)handler;
    uint32_t dpl = 3;

    إذا (مداخله < int_trap_min && مداخله != int_software_break_request) {
        dpl = 0;
    }

    idt[مداخله] = idt_ثبت_مداخله_gate_new(
                         handler_addr >> 16,   /* offset_high  */
                         1,                    /* present      */
                         dpl,                  /* dpl          */
                         1,                    /* gate_size    */
                         SEL_CS_0,             /* seg_selector */
                         handler_addr & 0xffff /* offset_low   */
                     );
}

BOOT_کوډ bool_t map_kernel_کړکۍ(
    uint32_t عدد_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   عدد_drhu,
    paddr_t   *drhu_list
)
{
    paddr_t  phys;
    uint32_t idx;
    pde_t    pde;
    pte_t    pte;
    بینښه صحيح ناکارول_شوی i;

    /* Mapping of PPTR_BASE (virtual address) to kernel's PADDR_BASE
     * up to end of virtual address space except for the last large page.
     */
    phys = PADDR_بنسټ;
    idx = PPTR_بنسټ >> LARGE_پاڼه_بټونه;

    /* PPTR_TOP differs whether CONFIG_KERNEL_LOG_BUFFER
     * is enabled or not.
     */
    طالما (idx < (PPTR_سر >> LARGE_پاڼه_بټونه)) {
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
        phys += بټ(LARGE_پاڼه_بټونه);
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    أكد(phys == PADDR_سر);

#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
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
    phys += بټ(LARGE_پاڼه_بټونه);
    أكد(idx == (KS_LOG_PPTR >> LARGE_پاڼه_بټونه));
    idx++;
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(ia32KSGlobalPD);

    phys += TLBBITMAP_PD_RESERVED;
    idx += TLBBITMAP_ریښه_ENTRIES;
#د_که_پای /* ENABLE_SMP_SUPPORT */

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

    طالما (idx < (KDEV_بنسټ &ماسک(LARGE_پاڼه_بټونه)) >> پاڼه_بټونه) {
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
    إذا (!map_kernel_کړکۍ_devices(ia32KSGlobalPT, عدد_ioapic, ioapic_paddrs, عدد_drhu, drhu_list)) {
        أرجع false;
    }

    invalidate_Local_پاڼه_Structure_Cache();
    أرجع true;
}

/* Note: this function will invalidate any pointers previously returned from this function */
BOOT_کوډ فراغ *map_لنډمهاله_boot_پاڼه(فراغ *ثبت, uint32_t large_پاڼې)
{
    فراغ *replacement_vaddr;
    بینښه صحيح i;
    بینښه صحيح دننه_واټن_دننه_پاڼه;

    بینښه صحيح phys_pg_پیل = (بینښه صحيح)(ثبت) & ~ماسک(LARGE_پاڼه_بټونه);
    بینښه صحيح virt_pd_پیل = (PPTR_بنسټ >> LARGE_پاڼه_بټونه) - large_پاڼې;
    بینښه صحيح virt_pg_پیل = PPTR_بنسټ - (large_پاڼې << LARGE_پاڼه_بټونه);

    لكل (i = 0; i < large_پاڼې; i++) {
        بینښه صحيح pg_دننه_واټن = i << LARGE_پاڼه_بټونه; // num pages since start * page size

        *(get_boot_pd() + virt_pd_پیل + i) = pde_pde_large_new(
                                                   phys_pg_پیل + pg_دننه_واټن, /* physical address */
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
        invalidateLocalTranslationSingle(virt_pg_پیل + pg_دننه_واټن);
    }

    // assign replacement virtual addresses page
    دننه_واټن_دننه_پاڼه = (بینښه صحيح)(ثبت) & ماسک(LARGE_پاڼه_بټونه);
    replacement_vaddr = (فراغ *)(virt_pg_پیل + دننه_واټن_دننه_پاڼه);

    invalidate_Local_پاڼه_Structure_Cache();

    أرجع replacement_vaddr;
}

/* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */

BOOT_کوډ فراغ init_dtrs(فراغ)
{
    /* setup the GDT pointer and limit and load into GDTR */
    gdt_idt_ptr.حد = (حجم(gdt_ثبت_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.بنسټ = (uint32_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSgdt;
    ia32_install_gdt(&gdt_idt_ptr);

    /* setup the IDT pointer and limit and load into IDTR */
    gdt_idt_ptr.حد = (حجم(idt_ثبت_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.بنسټ = (uint32_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSidt;
    ia32_install_idt(&gdt_idt_ptr);

    /* load NULL LDT selector into LDTR */
    ia32_install_ldt(SEL_NULL);

    /* load TSS selector into Task Register (TR) */
    ia32_install_tss(SEL_TSS);
}

ساكن BOOT_کوډ cap_t create_it_پاڼه_جدول_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_پاڼه_جدول_cap_new(
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

ساكن BOOT_کوډ cap_t create_it_پاڼه_directory_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_پاڼه_directory_cap_new(
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

BOOT_کوډ کلمه_t arch_get_n_paging(v_region_t it_v_reg)
{
    کلمه_t n = get_n_paging(it_v_reg, PT_شاخص_بټونه + پاڼه_بټونه);
#که_تعریف_شوی CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#د_که_پای
    أرجع n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_کوډ cap_t create_it_پته_space(cap_t ریښه_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_خونه_Pos خونه_pos_before;
    se_L4_خونه_Pos خونه_pos_after;

    خونه_pos_before = ndks_boot.خونه_pos_cur;
    copyGlobalMappings((vspace_ریښه_t *)rootserver.vspace);
    cap_t pd_cap = create_it_پاڼه_directory_cap(cap_null_cap_new(), rootserver.vspace, 0, IT_ASID);
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);
    vspace_cap = pd_cap;

    /* create all PT objs and caps necessary to cover userland image */

    لكل (vptr = پړاو_ښکته(it_v_reg.پیل, PT_شاخص_بټونه + پاڼه_بټونه);
         vptr < it_v_reg.پای_ټکی;
         vptr += بټ(PT_شاخص_بټونه + پاڼه_بټونه)) {
        إذا (!provide_cap(ریښه_cnode_cap,
                         create_it_پاڼه_جدول_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    خونه_pos_after = ndks_boot.خونه_pos_cur;
    ndks_boot.bi_چوکاټ->userImagePaging = (se_L4_خونه_Region) {
        خونه_pos_before, خونه_pos_after
    };

    أرجع vspace_cap;
}

ساكن BOOT_کوډ cap_t create_it_چوکاټ_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           vm_پاڼه_map_type_t map_type)
{
    vm_پاڼه_اندازه_t چوکاټ_اندازه_2;

    إذا (use_large) {
        چوکاټ_اندازه_2 = X86_Large_پاڼه;
    } وإلا {
        چوکاټ_اندازه_2 = X86_Small_پاڼه;
    }

    أرجع
        cap_چوکاټ_cap_new(
            چوکاټ_اندازه_2,                    /* capFSize           */
            ASID_ټیټ(asid),                /* capFMappedASIDLow  */
            vptr,                          /* capFMappedAddress  */
            map_type,                      /* capFMapType        */
            false,                         /* capFIsDevice       */
            ASID_لوړ(asid),               /* capFMappedASIDHigh */
            کلمه_From_VMRights(VMReadWrite), /* capFVMRights       */
            pptr                           /* capFBasePtr        */
        );
}

BOOT_کوډ cap_t create_unmapped_it_چوکاټ_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_چوکاټ_cap(pptr, 0, asidInvalid, use_large, X86_MappingNone);
}

BOOT_کوډ cap_t create_mapped_it_چوکاټ_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable ناکارول_شوی)
{
    cap_t cap = create_it_چوکاټ_cap(pptr, vptr, asid, use_large, X86_MappingVSpace);
    map_it_چوکاټ_cap(vspace_cap, cap);
    أرجع cap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

pde_t CONST make_User_PDELarge_پاڼه(paddr_t paddr, vm_ځانګړتیاوې_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pde_pde_large_new(
               paddr,                                          /* page_base_address    */
               vm_ځانګړتیاوې_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* avl                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ځانګړتیاوې_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_ځانګړتیاوې_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_جدول(paddr_t paddr, vm_ځانګړتیاوې_t vm_attr)
{
    أرجع pde_pde_pt_new(
               paddr,                                      /* pt_base_address  */
               0,                                          /* avl              */
               0,                                          /* accessed         */
               vm_ځانګړتیاوې_get_x86_PCDBit(vm_attr),       /* cache_disabled   */
               vm_ځانګړتیاوې_get_x86_PWTBit(vm_attr),       /* write_through    */
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_ځانګړتیاوې_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pte_new(
               paddr,                                          /* page_base_address    */
               0,                                              /* avl                  */
               0,                                              /* global               */
               vm_ځانګړتیاوې_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ځانګړتیاوې_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_ځانګړتیاوې_get_x86_PWTBit(vm_attr),           /* write_through        */
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

فراغ سېټ_VMRoot(tcb_t *tcb)
{
    cap_t               thread_ریښه;
    vspace_ریښه_t *vspace_ریښه;
    asid_t              asid;
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;

    thread_ریښه = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    vspace_ریښه = get_Valid_Native_ریښه(thread_ریښه);
    إذا (!vspace_ریښه) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_منزل_pptr(getCurrentPD()), getCurrentCPUIndex());)
        سېټ_Current_PD(kpptr_منزل_paddr(ia32KSGlobalPD));
        أرجع;
    }

    asid = cap_get_capMappedASID(thread_ریښه);
    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE || پیدا_کړه_ret.vspace_ریښه != vspace_ریښه) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_منزل_pptr(getCurrentPD()), getCurrentCPUIndex());)
        سېټ_Current_PD(kpptr_منزل_paddr(ia32KSGlobalPD));
        أرجع;
    }

    /* only set PD if we change it, otherwise we flush the TLB needlessly */
    إذا (getCurrentPD() != pptr_منزل_paddr(vspace_ریښه)) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_منزل_pptr(getCurrentPD()), getCurrentCPUIndex());)
        SMP_COND_STATEMENT(tlb_bitmap_سېټ(vspace_ریښه, getCurrentCPUIndex());)

        سېټ_Current_PD(pptr_منزل_paddr(vspace_ریښه));
    }
}

فراغ hwASIDInvalidate(asid_t asid, vspace_ریښه_t *vspace)
{
    /* 32-bit does not have PCID */
    أرجع;
}

exception_t decode_X86_ډېرپېښ_MMUInvocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    کلمه_t *بفر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_پاڼه_directory_cap:
        أرجع decode_IA32_پاڼه_Directory_Invocation(invLabel, اوږدتیا, cte, cap, call, بفر);

    افتراضي:
        fail("Invalid arch cap type");
    }
}


bool_t ډېرپېښ_Unmap_پاڼه(vm_پاڼه_اندازه_t پاڼه_اندازه, vspace_ریښه_t *vroot, vptr_t vaddr, فراغ *pptr)
{
    fail("Invalid page type");
    أرجع false;
}

exception_t decode_X86_ډېرپېښ_Map_پاڼه(کلمه_t invLabel, vm_پاڼه_اندازه_t پاڼه_اندازه, cte_t *cte, cap_t cap,
                                 vspace_ریښه_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_ځانګړتیاوې_t vm_attr)
{
    fail("Invalid Page type");
}

#که_تعریف_شوی CONFIG_KERNEL_LOG_بفر
exception_t benchmark_arch_map_log_بفر(کلمه_t چوکاټ_cptr)
{
    lookup_Cap_And_خونه_ret_t lu_ret;
    vm_پاڼه_اندازه_t چوکاټ_اندازه;
    pptr_t چوکاټ_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_خونه(غوټه_STATE(ksCurThread), چوکاټ_cptr);

    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکاټ_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکاټ_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_چوکاټ_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکاټ_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکاټ_اندازه = cap_چوکاټ_cap_get_cap_FSize(lu_ret.cap);

    إذا (چوکاټ_اندازه != X86_Large_پاڼه) {
        userError("Invalid size for log Buffer. The kernel expects at least 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکاټ_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    چوکاټ_pptr = cap_چوکاټ_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_منزل_paddr((فراغ *) چوکاټ_pptr);

    /* fill global log page table with mappings */
    لكل (صحيح idx = 0; idx < بټ(PT_شاخص_بټونه); idx++) {
        paddr_t physical_پته = ks_User_Log_بفر + (idx << se_L4_پاڼه_بټونه);

        pte_t pte = pte_new(
                        physical_پته,   /* page_base_address    */
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
        invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_پاڼه_بټونه), ماسک(ks_عدد_CPUs));
    }

    أرجع EXCEPTION_NONE;
}
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */
