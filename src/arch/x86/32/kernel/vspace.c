/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot_sys.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/tlb_bitmap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/kernel/tlb.h>

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

/* initialise the Task State Segment (TSS) */

BOOT_ਕੋਡ ਖਾਲੀ init_tss(tss_t *tss)
{
    *tss = tss_new(
               ਆਕਾਰ(*tss),   /* io_map_base  */
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
    memset(&x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KStss.io_map[0], 0xff,
           ਆਕਾਰ(x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KStss.io_map));
}
/* initialise Global Descriptor Table (GDT) */

BOOT_ਕੋਡ ਖਾਲੀ init_gdt(gdt_ਇੰਦਰਾਜ_t *gdt, tss_t *tss)
{
    uint32_t tss_addr = (uint32_t)tss;

    /* Set the NULL descriptor */
    gdt[GDT_NULL] = gdt_ਇੰਦਰਾਜ_gdt_null_new();

    /* 4GB flat kernel code segment on ring 0 descriptor */
    gdt[GDT_CS_0] = gdt_ਇੰਦਰਾਜ_gdt_ਕੋਡ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*ਉਪਲਬਧ*/
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
    gdt[GDT_DS_0] = gdt_ਇੰਦਰਾਜ_gdt_ਅੰਕੜੇ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*ਉਪਲਬਧ*/
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
    gdt[GDT_CS_3] = gdt_ਇੰਦਰਾਜ_gdt_ਕੋਡ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*ਉਪਲਬਧ*/
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
    gdt[GDT_DS_3] = gdt_ਇੰਦਰਾਜ_gdt_ਅੰਕੜੇ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*ਉਪਲਬਧ*/
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
    gdt[GDT_TSS] = gdt_ਇੰਦਰਾਜ_gdt_tss_new(
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
                       ਆਕਾਰ(tss_io_t) - 1         /* limit_low 16 bits    */
                   );

    gdt[GDT_FS] = gdt_ਇੰਦਰਾਜ_gdt_ਅੰਕੜੇ_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*ਉਪਲਬਧ*/
                      0xf,    /* Segment limit high 4 bits    */
                      1,      /* Present                      */
                      3,      /* Descriptor privilege level   */
                      1,      /* writable                     */
                      1,      /* accessed                     */
                      0,      /* Base middle 8 bits           */
                      0,      /* Base low 16 bits             */
                      0xffff  /* Segment limit low 16 bits    */
                  );

    gdt[GDT_GS] = gdt_ਇੰਦਰਾਜ_gdt_ਅੰਕੜੇ_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*ਉਪਲਬਧ*/
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

BOOT_ਕੋਡ ਖਾਲੀ init_idt_ਇੰਦਰਾਜ(idt_ਇੰਦਰਾਜ_t *idt, ਰੁਕਾਵਟ_t ਰੁਕਾਵਟ, ਖਾਲੀ(*handler)(ਖਾਲੀ))
{
    uint32_t handler_addr = (uint32_t)handler;
    uint32_t dpl = 3;

    ਜੇ (ਰੁਕਾਵਟ < int_trap_min && ਰੁਕਾਵਟ != int_software_break_request) {
        dpl = 0;
    }

    idt[ਰੁਕਾਵਟ] = idt_ਇੰਦਰਾਜ_ਰੁਕਾਵਟ_gate_new(
                         handler_addr >> 16,   /* offset_high  */
                         1,                    /* present      */
                         dpl,                  /* dpl          */
                         1,                    /* gate_size    */
                         SEL_CS_0,             /* seg_selector */
                         handler_addr & 0xffff /* offset_low   */
                     );
}

BOOT_ਕੋਡ bool_t map_kernel_ਖਿੜਕੀ(
    uint32_t ਅੰਕ_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   ਅੰਕ_drhu,
    paddr_t   *drhu_list
)
{
    paddr_t  phys;
    uint32_t idx;
    pde_t    pde;
    pte_t    pte;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਅਣਵਰਤਿਆ i;

    /* Mapping of PPTR_BASE (virtual address) to kernel's PADDR_BASE
     * up to end of virtual address space except for the last large page.
     */
    phys = PADDR_ਅਧਾਰ;
    idx = PPTR_ਅਧਾਰ >> LARGE_ਪੰਨਾ_ਬਿੱਟ;

    /* PPTR_TOP differs whether CONFIG_KERNEL_LOG_BUFFER
     * is enabled or not.
     */
    ਜਦੋਂ (idx < (PPTR_ਸਿਖਰ >> LARGE_ਪੰਨਾ_ਬਿੱਟ)) {
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
        phys += ਦੋਆਧਾਰੀ_ਅੰਕ(LARGE_ਪੰਨਾ_ਬਿੱਟ);
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    ਪ੍ਰਤਿਜ੍ਞਾ(phys == PADDR_ਸਿਖਰ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
    /* Map global page table for the log buffer */
    pde = pde_pde_pt_new(
              kpptr_ਮੰਜ਼ਿਲ_paddr(ia32KSGlobalLogPT), /* pt_base_address  */
              0,                 /* avl              */
              0,                 /* accessed         */
              0,                 /* cache_disabled   */
              0,                 /* write_through    */
              0,                 /* super_user       */
              1,                 /* read_write       */
              1                  /* present          */
          );

    ia32KSGlobalPD[idx] = pde;
    phys += ਦੋਆਧਾਰੀ_ਅੰਕ(LARGE_ਪੰਨਾ_ਬਿੱਟ);
    ਪ੍ਰਤਿਜ੍ਞਾ(idx == (KS_LOG_PPTR >> LARGE_ਪੰਨਾ_ਬਿੱਟ));
    idx++;
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(ia32KSGlobalPD);

    phys += TLBBITMAP_PD_RESERVED;
    idx += TLBBITMAP_ਜੜ੍ਹ_ENTRIES;
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

    /* map page table of last 4M of virtual address space to page directory */
    pde = pde_pde_pt_new(
              kpptr_ਮੰਜ਼ਿਲ_paddr(ia32KSGlobalPT), /* pt_base_address  */
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

    ਜਦੋਂ (idx < (KDEV_ਅਧਾਰ &ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ)) >> ਪੰਨਾ_ਬਿੱਟ) {
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
    ਜੇ (!map_kernel_ਖਿੜਕੀ_devices(ia32KSGlobalPT, ਅੰਕ_ioapic, ioapic_paddrs, ਅੰਕ_drhu, drhu_list)) {
        ਵਾਪਸ false;
    }

    invalidate_Local_ਪੰਨਾ_Structure_Cache();
    ਵਾਪਸ true;
}

/* Note: this function will invalidate any pointers previously returned from this function */
BOOT_ਕੋਡ ਖਾਲੀ *map_ਆਰਜ਼ੀ_boot_ਪੰਨਾ(ਖਾਲੀ *ਇੰਦਰਾਜ, uint32_t large_ਪੰਨੇ)
{
    ਖਾਲੀ *replacement_vaddr;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਵਿੱਥ_ਵਿੱਚ_ਪੰਨਾ;

    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ phys_pg_ਸ਼ੁਰੂ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(ਇੰਦਰਾਜ) & ~ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ virt_pd_ਸ਼ੁਰੂ = (PPTR_ਅਧਾਰ >> LARGE_ਪੰਨਾ_ਬਿੱਟ) - large_ਪੰਨੇ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ virt_pg_ਸ਼ੁਰੂ = PPTR_ਅਧਾਰ - (large_ਪੰਨੇ << LARGE_ਪੰਨਾ_ਬਿੱਟ);

    ਲਈ (i = 0; i < large_ਪੰਨੇ; i++) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ pg_ਵਿੱਥ = i << LARGE_ਪੰਨਾ_ਬਿੱਟ; // num pages since start * page size

        *(get_boot_pd() + virt_pd_ਸ਼ੁਰੂ + i) = pde_pde_large_new(
                                                   phys_pg_ਸ਼ੁਰੂ + pg_ਵਿੱਥ, /* physical address */
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
        invalidateLocalTranslationSingle(virt_pg_ਸ਼ੁਰੂ + pg_ਵਿੱਥ);
    }

    // assign replacement virtual addresses page
    ਵਿੱਥ_ਵਿੱਚ_ਪੰਨਾ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(ਇੰਦਰਾਜ) & ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ);
    replacement_vaddr = (ਖਾਲੀ *)(virt_pg_ਸ਼ੁਰੂ + ਵਿੱਥ_ਵਿੱਚ_ਪੰਨਾ);

    invalidate_Local_ਪੰਨਾ_Structure_Cache();

    ਵਾਪਸ replacement_vaddr;
}

/* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */

BOOT_ਕੋਡ ਖਾਲੀ init_dtrs(ਖਾਲੀ)
{
    /* setup the GDT pointer and limit and load into GDTR */
    gdt_idt_ptr.ਹੱਦਬੰਦੀ = (ਆਕਾਰ(gdt_ਇੰਦਰਾਜ_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.ਅਧਾਰ = (uint32_t)x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KSgdt;
    ia32_install_gdt(&gdt_idt_ptr);

    /* setup the IDT pointer and limit and load into IDTR */
    gdt_idt_ptr.ਹੱਦਬੰਦੀ = (ਆਕਾਰ(idt_ਇੰਦਰਾਜ_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.ਅਧਾਰ = (uint32_t)x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KSidt;
    ia32_install_idt(&gdt_idt_ptr);

    /* load NULL LDT selector into LDTR */
    ia32_install_ldt(SEL_NULL);

    /* load TSS selector into Task Register (TR) */
    ia32_install_tss(SEL_TSS);
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_ਪੰਨਾ_ਸਾਰਣੀ_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    ਜੇ (asid != asidInvalid) {
        map_it_pt_cap(vspace_cap, cap);
    }
    ਵਾਪਸ cap;
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_ਪੰਨਾ_directory_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ਪੰਨਾ_directory_cap_new(
              true,    /* capPDIsMapped   */
              IT_ASID, /* capPDMappedASID */
              vptr,    /* capPDMappedAddress */
              pptr  /* capPDBasePtr    */
          );
    ਜੇ (asid != asidInvalid && cap_get_capType(vspace_cap) != cap_null_cap) {
        map_it_pd_cap(vspace_cap, cap);
    }
    ਵਾਪਸ cap;
}

BOOT_ਕੋਡ ਸ਼ਬਦ_t arch_get_n_paging(v_region_t it_v_reg)
{
    ਸ਼ਬਦ_t n = get_n_paging(it_v_reg, PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_ਕੋਡ cap_t create_it_ਪਤਾ_space(cap_t ਜੜ੍ਹ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_before;
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_after;

    ਥਾਂ_pos_before = ndks_boot.ਥਾਂ_pos_cur;
    copyGlobalMappings((vspace_ਜੜ੍ਹ_t *)rootserver.vspace);
    cap_t pd_cap = create_it_ਪੰਨਾ_directory_cap(cap_null_cap_new(), rootserver.vspace, 0, IT_ASID);
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);
    vspace_cap = pd_cap;

    /* create all PT objs and caps necessary to cover userland image */

    ਲਈ (vptr = ਗੇੜਾ_ਥੱਲੇ(it_v_reg.ਸ਼ੁਰੂ, PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ);
         vptr < it_v_reg.ਸਿਰਾ;
         vptr += ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ)) {
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap,
                         create_it_ਪੰਨਾ_ਸਾਰਣੀ_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            ਵਾਪਸ cap_null_cap_new();
        }
    }

    ਥਾਂ_pos_after = ndks_boot.ਥਾਂ_pos_cur;
    ndks_boot.bi_ਚੌਖਟਾ->userImagePaging = (se_L4_ਥਾਂ_Region) {
        ਥਾਂ_pos_before, ਥਾਂ_pos_after
    };

    ਵਾਪਸ vspace_cap;
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_ਚੌਖਟਾ_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           vm_ਪੰਨਾ_map_type_t map_type)
{
    vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ_2;

    ਜੇ (use_large) {
        ਚੌਖਟਾ_ਮਾਪ_2 = X86_Large_ਪੰਨਾ;
    } ਨਹੀਂਤਾਂ {
        ਚੌਖਟਾ_ਮਾਪ_2 = X86_Small_ਪੰਨਾ;
    }

    ਵਾਪਸ
        cap_ਚੌਖਟਾ_cap_new(
            ਚੌਖਟਾ_ਮਾਪ_2,                    /* capFSize           */
            ASID_ਨੀਵਾਂ(asid),                /* capFMappedASIDLow  */
            vptr,                          /* capFMappedAddress  */
            map_type,                      /* capFMapType        */
            false,                         /* capFIsDevice       */
            ASID_ਉੱਚਾ(asid),               /* capFMappedASIDHigh */
            ਸ਼ਬਦ_From_VMRights(VMReadWrite), /* capFVMRights       */
            pptr                           /* capFBasePtr        */
        );
}

BOOT_ਕੋਡ cap_t create_unmapped_it_ਚੌਖਟਾ_cap(pptr_t pptr, bool_t use_large)
{
    ਵਾਪਸ create_it_ਚੌਖਟਾ_cap(pptr, 0, asidInvalid, use_large, X86_MappingNone);
}

BOOT_ਕੋਡ cap_t create_mapped_it_ਚੌਖਟਾ_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable ਅਣਵਰਤਿਆ)
{
    cap_t cap = create_it_ਚੌਖਟਾ_cap(pptr, vptr, asid, use_large, X86_MappingVSpace);
    map_it_ਚੌਖਟਾ_cap(vspace_cap, cap);
    ਵਾਪਸ cap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

pde_t CONST make_User_PDELarge_ਪੰਨਾ(paddr_t paddr, vm_ਗੁਣ_t vm_attr, vm_rights_t vm_rights)
{
    ਵਾਪਸ pde_pde_large_new(
               paddr,                                          /* page_base_address    */
               vm_ਗੁਣ_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* avl                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ਗੁਣ_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_ਗੁਣ_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_ਸਾਰਣੀ(paddr_t paddr, vm_ਗੁਣ_t vm_attr)
{
    ਵਾਪਸ pde_pde_pt_new(
               paddr,                                      /* pt_base_address  */
               0,                                          /* avl              */
               0,                                          /* accessed         */
               vm_ਗੁਣ_get_x86_PCDBit(vm_attr),       /* cache_disabled   */
               vm_ਗੁਣ_get_x86_PWTBit(vm_attr),       /* write_through    */
               1,                                          /* super_user       */
               1,                                          /* read_write       */
               1                                           /* present          */
           );
}

pde_t CONST makeUserPDEInvalid(ਖਾਲੀ)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    ਵਾਪਸ pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_ਗੁਣ_t vm_attr, vm_rights_t vm_rights)
{
    ਵਾਪਸ pte_new(
               paddr,                                          /* page_base_address    */
               0,                                              /* avl                  */
               0,                                              /* global               */
               vm_ਗੁਣ_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ਗੁਣ_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_ਗੁਣ_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}


pte_t CONST makeUserPTEInvalid(ਖਾਲੀ)
{
    ਵਾਪਸ pte_new(
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

ਖਾਲੀ ਸੈੱਟ_VMRoot(tcb_t *tcb)
{
    cap_t               thread_ਜੜ੍ਹ;
    vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ;
    asid_t              asid;
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    vspace_ਜੜ੍ਹ = get_Valid_Native_ਜੜ੍ਹ(thread_ਜੜ੍ਹ);
    ਜੇ (!vspace_ਜੜ੍ਹ) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_ਮੰਜ਼ਿਲ_pptr(getCurrentPD()), getCurrentCPUIndex());)
        ਸੈੱਟ_Current_PD(kpptr_ਮੰਜ਼ਿਲ_paddr(ia32KSGlobalPD));
        ਵਾਪਸ;
    }

    asid = cap_get_capMappedASID(thread_ਜੜ੍ਹ);
    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE || ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != vspace_ਜੜ੍ਹ) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_ਮੰਜ਼ਿਲ_pptr(getCurrentPD()), getCurrentCPUIndex());)
        ਸੈੱਟ_Current_PD(kpptr_ਮੰਜ਼ਿਲ_paddr(ia32KSGlobalPD));
        ਵਾਪਸ;
    }

    /* only set PD if we change it, otherwise we flush the TLB needlessly */
    ਜੇ (getCurrentPD() != pptr_ਮੰਜ਼ਿਲ_paddr(vspace_ਜੜ੍ਹ)) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_ਮੰਜ਼ਿਲ_pptr(getCurrentPD()), getCurrentCPUIndex());)
        SMP_COND_STATEMENT(tlb_bitmap_ਸੈੱਟ(vspace_ਜੜ੍ਹ, getCurrentCPUIndex());)

        ਸੈੱਟ_Current_PD(pptr_ਮੰਜ਼ਿਲ_paddr(vspace_ਜੜ੍ਹ));
    }
}

ਖਾਲੀ hwASIDInvalidate(asid_t asid, vspace_ਜੜ੍ਹ_t *vspace)
{
    /* 32-bit does not have PCID */
    ਵਾਪਸ;
}

exception_t decode_X86_ਬਹੁਲਕ_MMUInvocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
        ਵਾਪਸ decode_IA32_ਪੰਨਾ_Directory_Invocation(invLabel, ਲੰਮਾਈ, cte, cap, call, ਬਫਰ);

    ਮੂਲ:
        fail("Invalid arch cap type");
    }
}


bool_t ਬਹੁਲਕ_Unmap_ਪੰਨਾ(vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, vspace_ਜੜ੍ਹ_t *vroot, vptr_t vaddr, ਖਾਲੀ *pptr)
{
    fail("Invalid page type");
    ਵਾਪਸ false;
}

exception_t decode_X86_ਬਹੁਲਕ_Map_ਪੰਨਾ(ਸ਼ਬਦ_t invLabel, vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, cte_t *cte, cap_t cap,
                                 vspace_ਜੜ੍ਹ_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_ਗੁਣ_t vm_attr)
{
    fail("Invalid Page type");
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
exception_t benchmark_arch_map_log_ਬਫਰ(ਸ਼ਬਦ_t ਚੌਖਟਾ_cptr)
{
    lookup_Cap_And_ਥਾਂ_ret_t lu_ret;
    vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ;
    pptr_t ਚੌਖਟਾ_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_ਥਾਂ(ਗੰਢ_STATE(ksCurThread), ਚੌਖਟਾ_cptr);

    ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", ਚੌਖਟਾ_cptr);
        current_fault = seL4_Fault_CapFault_new(ਚੌਖਟਾ_cptr, false);

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_get_capType(lu_ret.cap) != cap_ਚੌਖਟਾ_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(ਚੌਖਟਾ_cptr, false);

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਚੌਖਟਾ_ਮਾਪ = cap_ਚੌਖਟਾ_cap_get_cap_FSize(lu_ret.cap);

    ਜੇ (ਚੌਖਟਾ_ਮਾਪ != X86_Large_ਪੰਨਾ) {
        userError("Invalid size for log Buffer. The kernel expects at least 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(ਚੌਖਟਾ_cptr, false);

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਚੌਖਟਾ_pptr = cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_ਬਫਰ = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *) ਚੌਖਟਾ_pptr);

    /* fill global log page table with mappings */
    ਲਈ (ਪੂਰਨ idx = 0; idx < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); idx++) {
        paddr_t physical_ਪਤਾ = ks_User_Log_ਬਫਰ + (idx << se_L4_ਪੰਨਾ_ਬਿੱਟ);

        pte_t pte = pte_new(
                        physical_ਪਤਾ,   /* page_base_address    */
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
        invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_ਪੰਨਾ_ਬਿੱਟ), ਨਕਾਬ(ks_ਅੰਕ_CPUs));
    }

    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */
