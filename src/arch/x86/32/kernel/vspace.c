/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot_sys.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/tlb_bitmap.h>
#ಅನ್ತರ್ಭಾವ <mode/kernel/tlb.h>

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

/* initialise the Task State Segment (TSS) */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ init_tss(tss_t *tss)
{
    *tss = tss_new(
               ಗಾತ್ರ(*tss),   /* io_map_base  */
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
    memset(&x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KStss.io_map[0], 0xff,
           ಗಾತ್ರ(x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KStss.io_map));
}
/* initialise Global Descriptor Table (GDT) */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ init_gdt(gdt_ನಮೂದು_t *gdt, tss_t *tss)
{
    uint32_t tss_addr = (uint32_t)tss;

    /* Set the NULL descriptor */
    gdt[GDT_NULL] = gdt_ನಮೂದು_gdt_null_new();

    /* 4GB flat kernel code segment on ring 0 descriptor */
    gdt[GDT_CS_0] = gdt_ನಮೂದು_gdt_ಸಂಕೇತ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*ಲಭ್ಯ*/
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
    gdt[GDT_DS_0] = gdt_ನಮೂದು_gdt_ದತ್ತಾಂಶ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*ಲಭ್ಯ*/
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
    gdt[GDT_CS_3] = gdt_ನಮೂದು_gdt_ಸಂಕೇತ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*ಲಭ್ಯ*/
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
    gdt[GDT_DS_3] = gdt_ನಮೂದು_gdt_ದತ್ತಾಂಶ_new(
                        0,      /* Base high 8 bits             */
                        1,      /* Granularity                  */
                        1,      /* Operation size               */
                        0,      /*ಲಭ್ಯ*/
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
    gdt[GDT_TSS] = gdt_ನಮೂದು_gdt_tss_new(
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
                       ಗಾತ್ರ(tss_io_t) - 1         /* limit_low 16 bits    */
                   );

    gdt[GDT_FS] = gdt_ನಮೂದು_gdt_ದತ್ತಾಂಶ_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*ಲಭ್ಯ*/
                      0xf,    /* Segment limit high 4 bits    */
                      1,      /* Present                      */
                      3,      /* Descriptor privilege level   */
                      1,      /* writable                     */
                      1,      /* accessed                     */
                      0,      /* Base middle 8 bits           */
                      0,      /* Base low 16 bits             */
                      0xffff  /* Segment limit low 16 bits    */
                  );

    gdt[GDT_GS] = gdt_ನಮೂದು_gdt_ದತ್ತಾಂಶ_new(
                      0,      /* Base high 8 bits             */
                      1,      /* Granularity                  */
                      1,      /* Operation size               */
                      0,      /*ಲಭ್ಯ*/
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

BOOT_ಸಂಕೇತ ಶೂನ್ಯ init_idt_ನಮೂದು(idt_ನಮೂದು_t *idt, ಅಡಚಣೆ_t ಅಡಚಣೆ, ಶೂನ್ಯ(*handler)(ಶೂನ್ಯ))
{
    uint32_t handler_addr = (uint32_t)handler;
    uint32_t dpl = 3;

    ಆದರೆ (ಅಡಚಣೆ < int_trap_min && ಅಡಚಣೆ != int_software_break_request) {
        dpl = 0;
    }

    idt[ಅಡಚಣೆ] = idt_ನಮೂದು_ಅಡಚಣೆ_gate_new(
                         handler_addr >> 16,   /* offset_high  */
                         1,                    /* present      */
                         dpl,                  /* dpl          */
                         1,                    /* gate_size    */
                         SEL_CS_0,             /* seg_selector */
                         handler_addr & 0xffff /* offset_low   */
                     );
}

BOOT_ಸಂಕೇತ bool_t map_kernel_ಕಿಟಕಿ(
    uint32_t ಅಂಕೆ_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   ಅಂಕೆ_drhu,
    paddr_t   *drhu_list
)
{
    paddr_t  phys;
    uint32_t idx;
    pde_t    pde;
    pte_t    pte;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಬಳಸದ i;

    /* Mapping of PPTR_BASE (virtual address) to kernel's PADDR_BASE
     * up to end of virtual address space except for the last large page.
     */
    phys = PADDR_ತಳ;
    idx = PPTR_ತಳ >> LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು;

    /* PPTR_TOP differs whether CONFIG_KERNEL_LOG_BUFFER
     * is enabled or not.
     */
    ಆಗ (idx < (PPTR_ತುದಿ >> LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
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
        phys += ಬಿಟ್(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು);
        idx++;
    }

    /* crosscheck whether we have mapped correctly so far */
    ಪ್ರತಿಜ್ಞಾ(phys == PADDR_ತುದಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
    /* Map global page table for the log buffer */
    pde = pde_pde_pt_new(
              kpptr_ಗಮ್ಯ_paddr(ia32KSGlobalLogPT), /* pt_base_address  */
              0,                 /* avl              */
              0,                 /* accessed         */
              0,                 /* cache_disabled   */
              0,                 /* write_through    */
              0,                 /* super_user       */
              1,                 /* read_write       */
              1                  /* present          */
          );

    ia32KSGlobalPD[idx] = pde;
    phys += ಬಿಟ್(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    ಪ್ರತಿಜ್ಞಾ(idx == (KS_LOG_PPTR >> LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು));
    idx++;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(ia32KSGlobalPD);

    phys += TLBBITMAP_PD_RESERVED;
    idx += TLBBITMAP_ಬೇರು_ENTRIES;
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    /* map page table of last 4M of virtual address space to page directory */
    pde = pde_pde_pt_new(
              kpptr_ಗಮ್ಯ_paddr(ia32KSGlobalPT), /* pt_base_address  */
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

    ಆಗ (idx < (KDEV_ತಳ &ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು)) >> ಪುಟ_ಬಿಟ್ಟುಗಳು) {
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
    ಆದರೆ (!map_kernel_ಕಿಟಕಿ_devices(ia32KSGlobalPT, ಅಂಕೆ_ioapic, ioapic_paddrs, ಅಂಕೆ_drhu, drhu_list)) {
        ಹಿಂತಿರುಗಿ false;
    }

    invalidate_Local_ಪುಟ_Structure_Cache();
    ಹಿಂತಿರುಗಿ true;
}

/* Note: this function will invalidate any pointers previously returned from this function */
BOOT_ಸಂಕೇತ ಶೂನ್ಯ *map_ತಾತ್ಕಾಲಿಕ_boot_ಪುಟ(ಶೂನ್ಯ *ನಮೂದು, uint32_t large_ಪುಟಗಳು)
{
    ಶೂನ್ಯ *replacement_vaddr;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ_ಒಳ_ಪುಟ;

    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ phys_pg_ಆರಂಭ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(ನಮೂದು) & ~ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ virt_pd_ಆರಂಭ = (PPTR_ತಳ >> LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು) - large_ಪುಟಗಳು;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ virt_pg_ಆರಂಭ = PPTR_ತಳ - (large_ಪುಟಗಳು << LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು);

    ಗಾಗಿ (i = 0; i < large_ಪುಟಗಳು; i++) {
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ pg_ಸ್ಥಾನಾಂತರ = i << LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು; // num pages since start * page size

        *(get_boot_pd() + virt_pd_ಆರಂಭ + i) = pde_pde_large_new(
                                                   phys_pg_ಆರಂಭ + pg_ಸ್ಥಾನಾಂತರ, /* physical address */
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
        invalidateLocalTranslationSingle(virt_pg_ಆರಂಭ + pg_ಸ್ಥಾನಾಂತರ);
    }

    // assign replacement virtual addresses page
    ಸ್ಥಾನಾಂತರ_ಒಳ_ಪುಟ = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(ನಮೂದು) & ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    replacement_vaddr = (ಶೂನ್ಯ *)(virt_pg_ಆರಂಭ + ಸ್ಥಾನಾಂತರ_ಒಳ_ಪುಟ);

    invalidate_Local_ಪುಟ_Structure_Cache();

    ಹಿಂತಿರುಗಿ replacement_vaddr;
}

/* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */

BOOT_ಸಂಕೇತ ಶೂನ್ಯ init_dtrs(ಶೂನ್ಯ)
{
    /* setup the GDT pointer and limit and load into GDTR */
    gdt_idt_ptr.ಎಲ್ಲೆ = (ಗಾತ್ರ(gdt_ನಮೂದು_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.ತಳ = (uint32_t)x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KSgdt;
    ia32_install_gdt(&gdt_idt_ptr);

    /* setup the IDT pointer and limit and load into IDTR */
    gdt_idt_ptr.ಎಲ್ಲೆ = (ಗಾತ್ರ(idt_ನಮೂದು_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.ತಳ = (uint32_t)x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KSidt;
    ia32_install_idt(&gdt_idt_ptr);

    /* load NULL LDT selector into LDTR */
    ia32_install_ldt(SEL_NULL);

    /* load TSS selector into Task Register (TR) */
    ia32_install_tss(SEL_TSS);
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_ಪುಟ_ಕೋಷ್ಟಕ_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
              1,    /* capPTIsMapped      */
              asid, /* capPTMappedASID    */
              vptr, /* capPTMappedAddress */
              pptr  /* capPTBasePtr       */
          );
    ಆದರೆ (asid != asidInvalid) {
        map_it_pt_cap(vspace_cap, cap);
    }
    ಹಿಂತಿರುಗಿ cap;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_ಪುಟ_directory_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ಪುಟ_directory_cap_new(
              true,    /* capPDIsMapped   */
              IT_ASID, /* capPDMappedASID */
              vptr,    /* capPDMappedAddress */
              pptr  /* capPDBasePtr    */
          );
    ಆದರೆ (asid != asidInvalid && cap_get_capType(vspace_cap) != cap_null_cap) {
        map_it_pd_cap(vspace_cap, cap);
    }
    ಹಿಂತಿರುಗಿ cap;
}

BOOT_ಸಂಕೇತ ಶಬ್ದ_t arch_get_n_paging(v_region_t it_v_reg)
{
    ಶಬ್ದ_t n = get_n_paging(it_v_reg, PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ n;
}

/* Create an address space for the initial thread.
 * This includes page directory and page tables */
BOOT_ಸಂಕೇತ cap_t create_it_ವಿಳಾಸ_space(cap_t ಬೇರು_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_before;
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_after;

    ಸ್ಥಾನ_pos_before = ndks_boot.ಸ್ಥಾನ_pos_cur;
    copyGlobalMappings((vspace_ಬೇರು_t *)rootserver.vspace);
    cap_t pd_cap = create_it_ಪುಟ_directory_cap(cap_null_cap_new(), rootserver.vspace, 0, IT_ASID);
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadVSpace), pd_cap);
    vspace_cap = pd_cap;

    /* create all PT objs and caps necessary to cover userland image */

    ಗಾಗಿ (vptr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(it_v_reg.ಆರಂಭ, PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು);
         vptr < it_v_reg.ಕೊನೆ;
         vptr += ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap,
                         create_it_ಪುಟ_ಕೋಷ್ಟಕ_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }
    }

    ಸ್ಥಾನ_pos_after = ndks_boot.ಸ್ಥಾನ_pos_cur;
    ndks_boot.bi_ಚೌಕಟ್ಟು->userImagePaging = (se_L4_ಸ್ಥಾನ_Region) {
        ಸ್ಥಾನ_pos_before, ಸ್ಥಾನ_pos_after
    };

    ಹಿಂತಿರುಗಿ vspace_cap;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_ಚೌಕಟ್ಟು_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           vm_ಪುಟ_map_type_t map_type)
{
    vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ_2;

    ಆದರೆ (use_large) {
        ಚೌಕಟ್ಟು_ಗಾತ್ರ_2 = X86_Large_ಪುಟ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಚೌಕಟ್ಟು_ಗಾತ್ರ_2 = X86_Small_ಪುಟ;
    }

    ಹಿಂತಿರುಗಿ
        cap_ಚೌಕಟ್ಟು_cap_new(
            ಚೌಕಟ್ಟು_ಗಾತ್ರ_2,                    /* capFSize           */
            ASID_ಕೆಳಗಿನ(asid),                /* capFMappedASIDLow  */
            vptr,                          /* capFMappedAddress  */
            map_type,                      /* capFMapType        */
            false,                         /* capFIsDevice       */
            ASID_ಮೇಲಿನ(asid),               /* capFMappedASIDHigh */
            ಶಬ್ದ_From_VMRights(VMReadWrite), /* capFVMRights       */
            pptr                           /* capFBasePtr        */
        );
}

BOOT_ಸಂಕೇತ cap_t create_unmapped_it_ಚೌಕಟ್ಟು_cap(pptr_t pptr, bool_t use_large)
{
    ಹಿಂತಿರುಗಿ create_it_ಚೌಕಟ್ಟು_cap(pptr, 0, asidInvalid, use_large, X86_MappingNone);
}

BOOT_ಸಂಕೇತ cap_t create_mapped_it_ಚೌಕಟ್ಟು_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable ಬಳಸದ)
{
    cap_t cap = create_it_ಚೌಕಟ್ಟು_cap(pptr, vptr, asid, use_large, X86_MappingVSpace);
    map_it_ಚೌಕಟ್ಟು_cap(vspace_cap, cap);
    ಹಿಂತಿರುಗಿ cap;
}

/* ==================== BOOT CODE FINISHES HERE ==================== */

pde_t CONST make_User_PDELarge_ಪುಟ(paddr_t paddr, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr, vm_rights_t vm_rights)
{
    ಹಿಂತಿರುಗಿ pde_pde_large_new(
               paddr,                                          /* page_base_address    */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* avl                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_ಕೋಷ್ಟಕ(paddr_t paddr, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr)
{
    ಹಿಂತಿರುಗಿ pde_pde_pt_new(
               paddr,                                      /* pt_base_address  */
               0,                                          /* avl              */
               0,                                          /* accessed         */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PCDBit(vm_attr),       /* cache_disabled   */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PWTBit(vm_attr),       /* write_through    */
               1,                                          /* super_user       */
               1,                                          /* read_write       */
               1                                           /* present          */
           );
}

pde_t CONST makeUserPDEInvalid(ಶೂನ್ಯ)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    ಹಿಂತಿರುಗಿ pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr, vm_rights_t vm_rights)
{
    ಹಿಂತಿರುಗಿ pte_new(
               paddr,                                          /* page_base_address    */
               0,                                              /* avl                  */
               0,                                              /* global               */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PATBit(vm_attr),           /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PCDBit(vm_attr),           /* cache_disabled       */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PWTBit(vm_attr),           /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}


pte_t CONST makeUserPTEInvalid(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ pte_new(
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

ಶೂನ್ಯ ಗಣ_VMRoot(tcb_t *tcb)
{
    cap_t               thread_ಬೇರು;
    vspace_ಬೇರು_t *vspace_ಬೇರು;
    asid_t              asid;
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    vspace_ಬೇರು = get_Valid_Native_ಬೇರು(thread_ಬೇರು);
    ಆದರೆ (!vspace_ಬೇರು) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_ಗಮ್ಯ_pptr(getCurrentPD()), getCurrentCPUIndex());)
        ಗಣ_Current_PD(kpptr_ಗಮ್ಯ_paddr(ia32KSGlobalPD));
        ಹಿಂತಿರುಗಿ;
    }

    asid = cap_get_capMappedASID(thread_ಬೇರು);
    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE || ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != vspace_ಬೇರು) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_ಗಮ್ಯ_pptr(getCurrentPD()), getCurrentCPUIndex());)
        ಗಣ_Current_PD(kpptr_ಗಮ್ಯ_paddr(ia32KSGlobalPD));
        ಹಿಂತಿರುಗಿ;
    }

    /* only set PD if we change it, otherwise we flush the TLB needlessly */
    ಆದರೆ (getCurrentPD() != pptr_ಗಮ್ಯ_paddr(vspace_ಬೇರು)) {
        SMP_COND_STATEMENT(tlb_bitmap_unset(paddr_ಗಮ್ಯ_pptr(getCurrentPD()), getCurrentCPUIndex());)
        SMP_COND_STATEMENT(tlb_bitmap_ಗಣ(vspace_ಬೇರು, getCurrentCPUIndex());)

        ಗಣ_Current_PD(pptr_ಗಮ್ಯ_paddr(vspace_ಬೇರು));
    }
}

ಶೂನ್ಯ hwASIDInvalidate(asid_t asid, vspace_ಬೇರು_t *vspace)
{
    /* 32-bit does not have PCID */
    ಹಿಂತಿರುಗಿ;
}

exception_t decode_X86_ಬಹುಲಕ_MMUInvocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
        ಹಿಂತಿರುಗಿ decode_IA32_ಪುಟ_Directory_Invocation(invLabel, ಉದ್ದಳತೆ, cte, cap, call, ಬಫರ್);

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid arch cap type");
    }
}


bool_t ಬಹುಲಕ_Unmap_ಪುಟ(vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, vspace_ಬೇರು_t *vroot, vptr_t vaddr, ಶೂನ್ಯ *pptr)
{
    fail("Invalid page type");
    ಹಿಂತಿರುಗಿ false;
}

exception_t decode_X86_ಬಹುಲಕ_Map_ಪುಟ(ಶಬ್ದ_t invLabel, vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, cte_t *cte, cap_t cap,
                                 vspace_ಬೇರು_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr)
{
    fail("Invalid Page type");
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
exception_t benchmark_arch_map_log_ಬಫರ್(ಶಬ್ದ_t ಚೌಕಟ್ಟು_cptr)
{
    lookup_Cap_And_ಸ್ಥಾನ_ret_t lu_ret;
    vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ;
    pptr_t ಚೌಕಟ್ಟು_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_ಸ್ಥಾನ(ಗಂಟು_STATE(ksCurThread), ಚೌಕಟ್ಟು_cptr);

    ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", ಚೌಕಟ್ಟು_cptr);
        current_fault = seL4_Fault_CapFault_new(ಚೌಕಟ್ಟು_cptr, false);

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_get_capType(lu_ret.cap) != cap_ಚೌಕಟ್ಟು_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(ಚೌಕಟ್ಟು_cptr, false);

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಚೌಕಟ್ಟು_ಗಾತ್ರ = cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(lu_ret.cap);

    ಆದರೆ (ಚೌಕಟ್ಟು_ಗಾತ್ರ != X86_Large_ಪುಟ) {
        userError("Invalid size for log Buffer. The kernel expects at least 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(ಚೌಕಟ್ಟು_cptr, false);

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಚೌಕಟ್ಟು_pptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_ಬಫರ್ = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *) ಚೌಕಟ್ಟು_pptr);

    /* fill global log page table with mappings */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); idx++) {
        paddr_t physical_ವಿಳಾಸ = ks_User_Log_ಬಫರ್ + (idx << se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು);

        pte_t pte = pte_new(
                        physical_ವಿಳಾಸ,   /* page_base_address    */
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
        invalidateTLBEntry(KS_LOG_PPTR + (idx << se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು), ಮುಖವಾಡ(ks_ಅಂಕೆ_CPUs));
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */
