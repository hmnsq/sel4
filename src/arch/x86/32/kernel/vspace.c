/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <api/syscall.h>
#شامل <machine/io.h>
#شامل <kernel/boot.h>
#شامل <model/statedata.h>
#شامل <arch/kernel/vspace.h>
#شامل <arch/kernel/boot.h>
#شامل <arch/kernel/boot_sys.h>
#شامل <arch/api/invocation.h>
#شامل <benchmark/benchmark_track.h>
#شامل <arch/kernel/tlb_bitmap.h>
#شامل <mode/kernel/tlb.h>

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

/* initialise the Task State Segment (TSS) */

BOOT_رمز خالی init_tss(tss_t *tss)
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
    memset(&x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KStss.io_map[0], 0xff,
           حجم(x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KStss.io_map));
}
/* initialise Global Descriptor Table (GDT) */

BOOT_رمز خالی init_gdt(gdt_اندراج_t *gdt, tss_t *tss)
{
    uint32_t tss_addr = (uint32_t)tss;

    /* Set the NULL descriptor */
    gdt[GDT_NULL] = gdt_اندراج_gdt_null_new();

    /* 4GB flat kernel code segment on ring 0 descriptor */
    gdt[GDT_CS_0] = gdt_اندراج_gdt_رمز_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*دستیاب*/
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
    gdt[GDT_DS_0] = gdt_اندراج_gdt_ڈیٹا_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*دستیاب*/
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
    gdt[GDT_CS_3] = gdt_اندراج_gdt_رمز_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*دستیاب*/
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
    gdt[GDT_DS_3] = gdt_اندراج_gdt_ڈیٹا_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*دستیاب*/
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
    gdt[GDT_TSS] = gdt_اندراج_gdt_tss_new(
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

    gdt[GDT_FS] = gdt_اندراج_gdt_ڈیٹا_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*دستیاب*/
                      0xf,    /* Segment limit high 4 bits    */
                      1,      /* Present                      */
                      3,      /* Descriptor privilege level   */
                      1,      /* writable                     */
                      1,      /* accessed                     */
                      0,      /* Base middle 8 bits           */
                      0,      /* Base low 16 bits             */
                      0xffff  /* Segment limit low 16 bits    */
                  );

    gdt[GDT_GS] = gdt_اندراج_gdt_ڈیٹا_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*دستیاب*/
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

BOOT_رمز خالی init_idt_اندراج(idt_اندراج_t *idt, مداخلت_t مداخلت, خالی(*handler)(خالی))
{
    uint32_t handler_addr = (uint32_t)handler;
    uint32_t dpl = 3;

    اگر (مداخلت < int_trap_min && مداخلت != int_software_break_request) {
        dpl = 0;
    }

    idt[مداخلت] = idt_اندراج_مداخلت_gate_new(
                         handler_addr >> 16,   /* offset_high  */
                         1,                    /* present      */
                         dpl,                  /* dpl          */
                         1,                    /* gate_size    */
                         SEL_CS_0,             /* seg_selector */
                         handler_addr & 0xffff /* offset_low   */
                     );
}

BOOT_رمز bool_t map_kernel_کھڑکی(
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
    غیرعلامتی صحیح غیر_مستعمل i;

    /* Mapping of PPTR_BASE (virtual address) to kernel's PADDR_BASE
     * up to end of virtual address space except for the last large page.
     */
    phys = PADDR_بنیاد;
    idx = PPTR_بنیاد >> LARGE_صفحہ_بٹ;

    /* PPTR_TOP differs whether CONFIG_KERNEL_LOG_BUFFER
     * is enabled or not.
     */
    جب (idx < (PPTR_چوٹی >> LARGE_صفحہ_بٹ)) {
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
        phys += ثنائی_ہندسہ(LARGE_صفحہ_بٹ);
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    أكد(phys == PADDR_چوٹی);

#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
    /* Map global page table for the log buffer */
    pde = pde_pde_pt_new(
              kpptr_مقصد_paddr(ia32KSGlobalLogPT), /* pt_base_address  */
              0,                 /* avl              */
              0,                 /* accessed         */
              0,                 /* cache_disabled   */
              0,                 /* write_through    */
              0,                 /* super_user       */
              1,                 /* read_write       */
              1                  /* present          */
          );

    ia32KSGlobalPD[idx] = pde;
    phys += ثنائی_ہندسہ(LARGE_صفحہ_بٹ);
    أكد(idx == (KS_LOG_PPTR >> LARGE_صفحہ_بٹ));
    idx++;
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(ia32KSGlobalPD);

    phys += TLBBITMAP_PD_RESERVED;
    idx += TLBBITMAP_جڑ_ENTRIES;
#اگر_ختم /* ENABLE_SMP_SUPPORT */

    /* map page table of last 4M of virtual address space to page directory */
    pde = pde_pde_pt_new(
              kpptr_مقصد_paddr(ia32KSGlobalPT), /* pt_base_address  */
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

    جب (idx < (KDEV_بنیاد &نقاب(LARGE_صفحہ_بٹ)) >> صفحہ_بٹ) {
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
    اگر (!map_kernel_کھڑکی_devices(ia32KSGlobalPT, عدد_ioapic, ioapic_paddrs, عدد_drhu, drhu_list)) {
        واپس false;
    }

    invalidate_Local_صفحہ_Structure_Cache();
    واپس true;
}

/* Note: this function will invalidate any pointers previously returned from this function */
BOOT_رمز خالی *map_عارضی_boot_صفحہ(خالی *اندراج_2, uint32_t large_صفحات)
{
    خالی *replacement_vaddr;
    غیرعلامتی صحیح i;
    غیرعلامتی صحیح انحراف_اندر_صفحہ;

    غیرعلامتی صحیح phys_pg_آغاز = (غیرعلامتی صحیح)(اندراج_2) & ~نقاب(LARGE_صفحہ_بٹ);
    غیرعلامتی صحیح virt_pd_آغاز = (PPTR_بنیاد >> LARGE_صفحہ_بٹ) - large_صفحات;
    غیرعلامتی صحیح virt_pg_آغاز = PPTR_بنیاد - (large_صفحات << LARGE_صفحہ_بٹ);

    برائے (i = 0; i < large_صفحات; i++) {
        غیرعلامتی صحیح pg_انحراف = i << LARGE_صفحہ_بٹ; // num pages since start * page size

        *(get_boot_pd() + virt_pd_آغاز + i) = pde_pde_large_new(
                                                   phys_pg_آغاز + pg_انحراف, /* physical address */
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
        invalidateLocalTranslationSingle(virt_pg_آغاز + pg_انحراف);
    }

    // assign replacement virtual addresses page
    انحراف_اندر_صفحہ = (غیرعلامتی صحیح)(اندراج_2) & نقاب(LARGE_صفحہ_بٹ);
    replacement_vaddr = (خالی *)(virt_pg_آغاز + انحراف_اندر_صفحہ);

    invalidate_Local_صفحہ_Structure_Cache();

    واپس replacement_vaddr;
}

/* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */

BOOT_رمز خالی init_dtrs(خالی)
{
    /* setup the GDT pointer and limit and load into GDTR */
    gdt_idt_ptr.حد_بندی = (حجم(gdt_اندراج_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.بنیاد = (uint32_t)x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KSgdt;
    ia32_install_gdt(&gdt_idt_ptr);

    /* setup the IDT pointer and limit and load into IDTR */
    gdt_idt_ptr.حد_بندی = (حجم(idt_اندراج_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.بنیاد = (uint32_t)x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KSidt;
    ia32_install_idt(&gdt_idt_ptr);

    /* load NULL LDT selector into LDTR */
    ia32_install_ldt(SEL_NULL);

    /* load TSS selector into Task Register (TR) */
    ia32_install_tss(SEL_TSS);
}

ساکن BOOT_رمز cap_t create_it_صفحہ_جدول_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحہ_جدول_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    اگر (asid != asidInvalid) {
        map_it_pt_cap(vspace_cap, cap);
    }
    واپس cap;
}

ساکن BOOT_رمز cap_t create_it_صفحہ_directory_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحہ_directory_cap_new(
              true,    /* capPDIsMapped   */
              IT_ASID, /* capPDMappedASID */
              vptr,    /* capPDMappedAddress */
              pptr  /* capPDBasePtr    */
          );
    اگر (asid != asidInvalid && cap_get_capType(vspace_cap) != cap_null_cap) {
        map_it_pd_cap(vspace_cap, cap);
    }
    واپس cap;
}

BOOT_رمز لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    لفظ_t n = get_n_paging(it_v_reg, PT_اشاریہ_بٹ + صفحہ_بٹ);
#اگر_تعریف_شدہ CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#اگر_ختم
    واپس n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_رمز cap_t create_it_پتہ_space(cap_t جڑ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_خانہ_Pos خانہ_pos_before;
    se_L4_خانہ_Pos خانہ_pos_after;

    خانہ_pos_before = ndks_boot.خانہ_pos_cur;
    copyGlobalMappings((vspace_جڑ_t *)rootserver.vspace);
    cap_t pd_cap = create_it_صفحہ_directory_cap(cap_null_cap_new(), rootserver.vspace, 0, IT_ASID);
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);
    vspace_cap = pd_cap;

    /* create all PT objs and caps necessary to cover userland image */

    برائے (vptr = باری_نیچے_کو(it_v_reg.آغاز, PT_اشاریہ_بٹ + صفحہ_بٹ);
         vptr < it_v_reg.انتہا;
         vptr += ثنائی_ہندسہ(PT_اشاریہ_بٹ + صفحہ_بٹ)) {
        اگر (!provide_cap(جڑ_cnode_cap,
                         create_it_صفحہ_جدول_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            واپس cap_null_cap_new();
        }
    }

    خانہ_pos_after = ndks_boot.خانہ_pos_cur;
    ndks_boot.bi_چوکھٹا->userImagePaging = (se_L4_خانہ_Region) {
        خانہ_pos_before, خانہ_pos_after
    };

    واپس vspace_cap;
}

ساکن BOOT_رمز cap_t create_it_چوکھٹا_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           vm_صفحہ_map_type_t map_type)
{
    vm_صفحہ_سائز_t چوکھٹا_سائز_2;

    اگر (use_large) {
        چوکھٹا_سائز_2 = X86_Large_صفحہ;
    } ورنہ {
        چوکھٹا_سائز_2 = X86_Small_صفحہ;
    }

    واپس
        cap_چوکھٹا_cap_new(
            چوکھٹا_سائز_2,                    /* capFSize           */
            ASID_کم(asid),                /* capFMappedASIDLow  */
            vptr,                          /* capFMappedAddress  */
            map_type,                      /* capFMapType        */
            false,                         /* capFIsDevice       */
            ASID_بلند(asid),               /* capFMappedASIDHigh */
            لفظ_From_VMRights(VMReadWrite), /* capFVMRights       */
            pptr                           /* capFBasePtr        */
        );
}

BOOT_رمز cap_t create_unmapped_it_چوکھٹا_cap(pptr_t pptr, bool_t use_large)
{
    واپس create_it_چوکھٹا_cap(pptr, 0, asidInvalid, use_large, X86_MappingNone);
}

BOOT_رمز cap_t create_mapped_it_چوکھٹا_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable غیر_مستعمل)
{
    cap_t cap = create_it_چوکھٹا_cap(pptr, vptr, asid, use_large, X86_MappingVSpace);
    map_it_چوکھٹا_cap(vspace_cap, cap);
    واپس cap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

pde_t CONST make_User_PDELarge_صفحہ(paddr_t paddr, vm_اوصاف_t vm_attr, vm_rights_t vm_rights)
{
    واپس pde_pde_large_new(
               paddr,                                          /* page_base_address    */
               vm_اوصاف_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* avl                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_اوصاف_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_اوصاف_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_جدول(paddr_t paddr, vm_اوصاف_t vm_attr)
{
    واپس pde_pde_pt_new(
               paddr,                                      /* pt_base_address  */
               0,                                          /* avl              */
               0,                                          /* accessed         */
               vm_اوصاف_get_x86_PCDBit(vm_attr),       /* cache_disabled   */
               vm_اوصاف_get_x86_PWTBit(vm_attr),       /* write_through    */
               1,                                          /* super_user       */
               1,                                          /* read_write       */
               1                                           /* present          */
           );
}

pde_t CONST makeUserPDEInvalid(خالی)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    واپس pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_اوصاف_t vm_attr, vm_rights_t vm_rights)
{
    واپس pte_new(
               paddr,                                          /* page_base_address    */
               0,                                              /* avl                  */
               0,                                              /* global               */
               vm_اوصاف_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_اوصاف_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_اوصاف_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}


pte_t CONST makeUserPTEInvalid(خالی)
{
    واپس pte_new(
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

خالی سیٹ_VMRoot(tcb_t *tcb)
{
    cap_t               thread_جڑ;
    vspace_جڑ_t *vspace_جڑ;
    asid_t              asid;
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;

    thread_جڑ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    vspace_جڑ = get_Valid_Native_جڑ(thread_جڑ);
    اگر (!vspace_جڑ) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_مقصد_pptr(getCurrentPD()), getCurrentCPUIndex());)
        سیٹ_Current_PD(kpptr_مقصد_paddr(ia32KSGlobalPD));
        واپس;
    }

    asid = cap_get_capMappedASID(thread_جڑ);
    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE || ڈھونڈو_ret.vspace_جڑ != vspace_جڑ) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_مقصد_pptr(getCurrentPD()), getCurrentCPUIndex());)
        سیٹ_Current_PD(kpptr_مقصد_paddr(ia32KSGlobalPD));
        واپس;
    }

    /* only set PD if we change it, otherwise we flush the TLB needlessly */
    اگر (getCurrentPD() != pptr_مقصد_paddr(vspace_جڑ)) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_مقصد_pptr(getCurrentPD()), getCurrentCPUIndex());)
        SMP_COND_STATEMENT(tlb_bitmap_سیٹ(vspace_جڑ, getCurrentCPUIndex());)

        سیٹ_Current_PD(pptr_مقصد_paddr(vspace_جڑ));
    }
}

خالی hwASIDInvalidate(asid_t asid, vspace_جڑ_t *vspace)
{
    /* 32-bit does not have PCID */
    واپس;
}

exception_t decode_X86_منوال_MMUInvocation(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_صفحہ_directory_cap:
        واپس decode_IA32_صفحہ_Directory_Invocation(invLabel, لمبائی, cte, cap, call, بفر);

    طےشدہ:
        fail("Invalid arch cap type");
    }
}


bool_t منوال_Unmap_صفحہ(vm_صفحہ_سائز_t صفحہ_سائز, vspace_جڑ_t *vroot, vptr_t vaddr, خالی *pptr)
{
    fail("Invalid page type");
    واپس false;
}

exception_t decode_X86_منوال_Map_صفحہ(لفظ_t invLabel, vm_صفحہ_سائز_t صفحہ_سائز, cte_t *cte, cap_t cap,
                                 vspace_جڑ_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_اوصاف_t vm_attr)
{
    fail("Invalid Page type");
}

#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
exception_t benchmark_arch_map_log_بفر(لفظ_t چوکھٹا_cptr)
{
    lookup_Cap_And_خانہ_ret_t lu_ret;
    vm_صفحہ_سائز_t چوکھٹا_سائز;
    pptr_t چوکھٹا_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_خانہ(گرہ_STATE(ksCurThread), چوکھٹا_cptr);

    اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", چوکھٹا_cptr);
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_get_capType(lu_ret.cap) != cap_چوکھٹا_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_سائز = cap_چوکھٹا_cap_get_cap_FSize(lu_ret.cap);

    اگر (چوکھٹا_سائز != X86_Large_صفحہ) {
        userError("Invalid size for log Buffer. The kernel expects at least 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(چوکھٹا_cptr, false);

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    چوکھٹا_pptr = cap_چوکھٹا_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_بفر = pptr_مقصد_paddr((خالی *) چوکھٹا_pptr);

    /* fill global log page table with mappings */
    برائے (صحیح idx = 0; idx < ثنائی_ہندسہ(PT_اشاریہ_بٹ); idx++) {
        paddr_t physical_پتہ = ks_User_Log_بفر + (idx << se_L4_صفحہ_بٹ);

        pte_t pte = pte_new(
                        physical_پتہ,   /* page_base_address    */
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
        invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_صفحہ_بٹ), نقاب(ks_عدد_CPUs));
    }

    واپس EXCEPTION_NONE;
}
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */
