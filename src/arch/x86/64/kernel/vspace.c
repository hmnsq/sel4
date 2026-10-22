/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <kernel/boot.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <arch/kernel/vspace.h>
#અન્તર્ભાવ <arch/kernel/boot.h>
#અન્તર્ભાવ <arch/kernel/boot_sys.h>
#અન્તર્ભાવ <arch/api/invocation.h>
#અન્તર્ભાવ <mode/kernel/tlb.h>
#અન્તર્ભાવ <arch/kernel/tlb_bitmap.h>
#અન્તર્ભાવ <object/structures.h>

/* When using the SKIM window to isolate the kernel from the user we also need to
 * not use global mappings as having global mappings and entries in the TLB is
 * equivalent, for the purpose of exploitation, to having the mappings in the
 * kernel window */
#define KERNEL_IS_GLOBAL() (config_set(CONFIG_KERNEL_SKIM_WINDOW) ? 0 : 1)

/* For the boot code we create two windows into the physical address space
 * One is at the same location as the kernel window, and is placed up high
 * The other is a 1-to-1 mapping of the first 512gb of memory. The purpose
 * of this is to have a 1-to-1 mapping for the low parts of memory, so that
 * when we switch paging on, and are still running at physical addresses,
 * we don't explode. Then we also want the high mappings so we can start
 * running at proper kernel virtual addresses */
pml4e_t boot_pml4[BIT(PML4_INDEX_BITS)] ALIGN(BIT(seL4_PageBits)) VISIBLE PHYS_BSS;
pdpte_t boot_pdpt[BIT(PDPT_INDEX_BITS)] ALIGN(BIT(seL4_PageBits)) VISIBLE PHYS_BSS;

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

BOOT_CODE bool_t map_kernel_window(
    uint32_t અંક_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   અંક_drhu,
    paddr_t   *drhu_list
)
{

    uint64_t paddr;
    uint64_t vaddr;

#ifdef CONFIG_HUGE_PAGE
    /* using 1 GiB page size */

    /* verify that the kernel window as at the last entry of the PML4 */
    પ્રતિજ્ઞા(GET_PML4_INDEX(PPTR_BASE) == BIT(PML4_INDEX_BITS) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    પ્રતિજ્ઞા(GET_PML4_INDEX(KERNEL_ELF_BASE) == BIT(PML4_INDEX_BITS) - 1);
    પ્રતિજ્ઞા(GET_PDPT_INDEX(KERNEL_ELF_BASE) == BIT(PML4_INDEX_BITS) - 2);
    પ્રતિજ્ઞા(GET_PDPT_INDEX(KDEV_BASE) == BIT(PML4_INDEX_BITS) - 1);
    પ્રતિજ્ઞા(IS_ALIGNED(KERNEL_ELF_BASE - KERNEL_ELF_PADDR_BASE, seL4_HugePageBits));
    પ્રતિજ્ઞા(IS_ALIGNED(KDEV_BASE, seL4_HugePageBits));
    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_INDEX(PPTR_BASE)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_to_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );
    /* put the 1GB kernel_base mapping into the PDPT */
    x64KSKernelPDPT[GET_PDPT_INDEX(KERNEL_ELF_BASE)] = pdpte_pdpte_1g_new(
                                                           0, /* xd */
                                                           PADDR_BASE,
                                                           0, /* PAT */
                                                           KERNEL_IS_GLOBAL(), /* global */
                                                           0, /* dirty */
                                                           0, /* accessed */
                                                           0, /* cache_disabled */
                                                           0, /* write_through */
                                                           0, /* super_user */
                                                           1, /* read_write */
                                                           1  /* present */
                                                       );
    /* also map the physical memory into the big kernel window */
    paddr = 0;
    vaddr = PPTR_BASE;
    માટે (paddr = 0; paddr < PADDR_ટોચ;
         paddr += BIT(seL4_HugePageBits)) {

        પૂર્ણાંક pdpte_index = GET_PDPT_INDEX(vaddr);
        x64KSKernelPDPT[pdpte_index] = pdpte_pdpte_1g_new(
                                           0,          /* xd               */
                                           paddr,      /* physical address */
                                           0,          /* PAT              */
                                           KERNEL_IS_GLOBAL(), /* global   */
                                           0,          /* dirty            */
                                           0,          /* accessed         */
                                           0,          /* cache_disabled   */
                                           0,          /* write_through    */
                                           0,          /* super_user       */
                                           1,          /* read_write       */
                                           1           /* present          */
                                       );

        vaddr += BIT(seL4_HugePageBits);
    }

    /* put the PD into the PDPT */
    x64KSKernelPDPT[GET_PDPT_INDEX(KDEV_BASE)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_to_paddr(x64KSKernelPD),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );
    /* put the PT into the PD */
    x64KSKernelPD[0] = pde_pde_pt_new(
                           0, /* xd */
                           kpptr_to_paddr(x64KSKernelPT),
                           0, /* accessed */
                           0, /* cache_disabled */
                           0, /* write_through */
                           0, /* super_user */
                           1, /* read_write */
                           1  /* present */
                       );
#નહીંતર

    પૂર્ણાંક pd_index = 0;
    /* use 2 MiB page size */
    /* verify that the kernel window as at the last entry of the PML4 */
    પ્રતિજ્ઞા(GET_PML4_INDEX(PPTR_BASE) == BIT(PML4_INDEX_BITS) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    પ્રતિજ્ઞા(GET_PML4_INDEX(KERNEL_ELF_BASE) == BIT(PML4_INDEX_BITS) - 1);
    પ્રતિજ્ઞા(GET_PDPT_INDEX(KERNEL_ELF_BASE) == BIT(PML4_INDEX_BITS) - 2);
    પ્રતિજ્ઞા(GET_PDPT_INDEX(KDEV_BASE) == BIT(PML4_INDEX_BITS) - 1);
    પ્રતિજ્ઞા(IS_ALIGNED(KERNEL_ELF_BASE - KERNEL_ELF_PADDR_BASE, seL4_HugePageBits));
    પ્રતિજ્ઞા(IS_ALIGNED(KDEV_BASE, seL4_HugePageBits));

    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_INDEX(PPTR_BASE)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_to_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    માટે (pd_index = 0; pd_index < PADDR_ટોચ >> seL4_HugePageBits; pd_index++) {
        /* put the 1GB kernel_base mapping into the PDPT */
        x64KSKernelPDPT[GET_PDPT_INDEX(PPTR_BASE) + pd_index] = pdpte_pdpte_pd_new(
                                                                    0, /* xd */
                                                                    kpptr_to_paddr(&x64KSKernelPDs[pd_index][0]),
                                                                    0, /* accessed */
                                                                    0, /* cache disabled */
                                                                    0, /* write through */
                                                                    0, /* super user */
                                                                    1, /* read write */
                                                                    1 /* present */
                                                                );
    }

    x64KSKernelPDPT[GET_PDPT_INDEX(KERNEL_ELF_BASE)] = pdpte_pdpte_pd_new(
                                                           0, /* xd */
                                                           kpptr_to_paddr(&x64KSKernelPDs[0][0]),
                                                           0, /* accessed */
                                                           0, /* cache disable */
                                                           1, /* write through */
                                                           0, /* super user */
                                                           1, /* read write */
                                                           1  /* present */
                                                       );

    paddr = 0;
    vaddr = PPTR_BASE;

    માટે (paddr = 0; paddr < PADDR_ટોચ;
         paddr += 0x200000) {

        પૂર્ણાંક pd_index = GET_PDPT_INDEX(vaddr) - GET_PDPT_INDEX(PPTR_BASE);
        પૂર્ણાંક pde_index = GET_PD_INDEX(vaddr);

        x64KSKernelPDs[pd_index][pde_index] = pde_pde_large_new(
                                                  0, /* xd */
                                                  paddr,
                                                  0, /* pat */
                                                  KERNEL_IS_GLOBAL(), /* global */
                                                  0, /* dirty */
                                                  0, /* accessed */
                                                  0, /* cache disabled */
                                                  0, /* write through */
                                                  0, /* super user */
                                                  1, /* read write */
                                                  1  /* present */
                                              );
        vaddr += 0x200000;
    }

    /* put the PD into the PDPT */
    x64KSKernelPDPT[GET_PDPT_INDEX(KDEV_BASE)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_to_paddr(&x64KSKernelPDs[BIT(PDPT_INDEX_BITS) - 1][0]),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    /* put the PT into the PD */
    x64KSKernelPDs[BIT(PDPT_INDEX_BITS) - 1][0] = pde_pde_pt_new(
                                                      0, /* xd */
                                                      kpptr_to_paddr(x64KSKernelPT),
                                                      0, /* accessed */
                                                      0, /* cache_disabled */
                                                      0, /* write_through */
                                                      0, /* super_user */
                                                      1, /* read_write */
                                                      1  /* present */
                                                  );
#endif

#જો CONFIG_MAX_અંક_TRACE_POINTS > 0
    /* use the last PD entry as the benchmark log storage.
     * the actual backing physical memory will be filled
     * later by using alloc_region */
    ksLog = (ks_log_entry_t *)(KDEV_BASE + 0x200000 * (BIT(PD_INDEX_BITS) - 1));
#endif

    /* now map in the kernel devices */
    જો (!map_kernel_window_devices(x64KSKernelPT, અંક_ioapic, ioapic_paddrs, અંક_drhu, drhu_list)) {
        પાછા false;
    }

#ifdef ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(x64KSKernelPML4);
#endif /* ENABLE_SMP_SUPPORT */

    /* In boot code, so fine to just trash everything here */
    invalidateLocalTranslationAll();
    printf("Mapping kernel window is done\n");
    પાછા true;
}

#ifdef CONFIG_KERNEL_SKIM_WINDOW
BOOT_CODE bool_t map_skim_window(vptr_t skim_start, vptr_t skim_end)
{
    /* place the PDPT into the PML4 */
    x64KSSKIMPML4[GET_PML4_INDEX(PPTR_BASE)] = pml4e_new(
                                                   0, /* xd */
                                                   kpptr_to_paddr(x64KSSKIMPDPT),
                                                   0, /* accessed */
                                                   0, /* cache_disabled */
                                                   0, /* write_through */
                                                   0, /* super_user */
                                                   1, /* read_write */
                                                   1  /* present */
                                               );
    /* place the PD into the kernel_base slot of the PDPT */
    x64KSSKIMPDPT[GET_PDPT_INDEX(KERNEL_ELF_BASE)] = pdpte_pdpte_pd_new(
                                                         0, /* xd */
                                                         kpptr_to_paddr(x64KSSKIMPD),
                                                         0, /* accessed */
                                                         0, /* cache_disabled */
                                                         0, /* write_through */
                                                         0, /* super_user */
                                                         1, /* read_write */
                                                         1  /* present */
                                                     );
    /* map the skim portion into the PD. we expect it to be 2M aligned */
    પ્રતિજ્ઞા((skim_start % BIT(seL4_LargePageBits)) == 0);
    પ્રતિજ્ઞા((skim_end % BIT(seL4_LargePageBits)) == 0);
    uint64_t paddr = kpptr_to_paddr((શૂન્ય *)skim_start);
    માટે (પૂર્ણાંક i = GET_PD_INDEX(skim_start); i < GET_PD_INDEX(skim_end); i++) {
        x64KSSKIMPD[i] = pde_pde_large_new(
                             0, /* xd */
                             paddr,
                             0, /* pat */
                             KERNEL_IS_GLOBAL(), /* global */
                             0, /* dirty */
                             0, /* accessed */
                             0, /* cache_disabled */
                             0, /* write_through */
                             0, /* super_user */
                             1, /* read_write */
                             1  /* present */
                         );
        paddr += BIT(seL4_LargePageBits);
    }
    પાછા true;
}
#endif

BOOT_CODE શૂન્ય init_tss(tss_t *tss)
{
    word_t base = (word_t)&x64KSIRQStack[CURRENT_CPU_INDEX()][IRQ_ઢગલો_SIZE];
    *tss = tss_new(
               કદ(*tss),   /* io map base */
               0, 0,       /* ist 7 */
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               /* ist 1 is the stack frame we use for interrupts */
               base >> 32, base & 0xffffffff,  /* ist 1 */
               0, 0,       /* rsp 2 */
               0, 0,       /* rsp 1 */
               0, 0        /* rsp 0 */
           );
    /* set the IO map to all 1 to block user IN/OUT instructions */
    memset(&x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.io_map[0], 0xff,
           કદ(x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.io_map));
}

શૂન્ય init_syscall_msrs(શૂન્ય)
{
    x86_wrmsr(IA32_LSTAR_MSR, (uint64_t)&handle_fastsyscall);
    // mask bit 9 in the kernel (which is the interrupt enable bit)
    // also mask bit 8, which is the Trap Flag, to prevent the kernel
    // from single stepping
    x86_wrmsr(IA32_FMASK_MSR, FLAGS_TF | FLAGS_IF);
    x86_wrmsr(IA32_STAR_MSR, ((uint64_t)SEL_CS_0 << 32) | ((uint64_t)SEL_CS_3 << 48));
}

BOOT_CODE શૂન્ય init_gdt(gdt_entry_t *gdt, tss_t *tss)
{

    uint64_t tss_base = (uint64_t)tss;
    gdt_tss_t gdt_tss;

    gdt[GDT_NULL] = gdt_entry_gdt_null_new();

    gdt[GDT_CS_0] = gdt_entry_gdt_code_new(
                        0,                  /* base high */
                        1,                  /* granularity */
                        0,                  /* operation size, must be 0 when 64-bit is set */
                        1,                  /* long mode */
                        0,                  /* avl */
                        0xf,                /* limit high */
                        1,                  /* present */
                        0,                  /* dpl */
                        1,                  /* always 1 for segment */
                        0,                  /* base middle */
                        0,                  /* base low */
                        0xffff              /* limit low */
                    );

    gdt[GDT_DS_0] = gdt_entry_gdt_data_new(
                        0,                  /* base high */
                        1,                  /* granularity */
                        1,                  /* operation size */
                        0,                  /* avl */
                        0xf,                /* seg limit high */
                        1,                  /* present */
                        0,                  /* dpl */
                        1,                  /* always 1 */
                        0,                  /* base mid */
                        0,                  /* base low */
                        0xffff              /* seg limit low */
                    );

    gdt[GDT_CS_3] = gdt_entry_gdt_code_new(
                        0,                  /* base high */
                        1,                  /* granularity */
                        0,                  /* operation size, must be 0 when 64-bit is set */
                        1,                  /* long mode */
                        0,                  /* avl */
                        0xf,                /* limit high */
                        1,                  /* present */
                        3,                  /* dpl */
                        1,                  /* always 1 */
                        0,                  /* base middle */
                        0,                  /* base low */
                        0xffff              /* limit low */
                    );

    gdt[GDT_DS_3] = gdt_entry_gdt_data_new(
                        0,
                        1,
                        1,
                        0,
                        0xf,
                        1,
                        3,
                        1,
                        0,
                        0,
                        0xffff
                    );

    gdt[GDT_FS] = gdt_entry_gdt_data_new(
                      0,
                      1,
                      1,
                      0,
                      0xf,
                      1,
                      3,
                      1,
                      0,
                      0,
                      0xffff
                  );

    gdt[GDT_GS] = gdt_entry_gdt_data_new(
                      0,
                      1,
                      1,
                      0,
                      0xf,
                      1,
                      3,
                      1,
                      0,
                      0,
                      0xffff
                  );

    gdt_tss = gdt_tss_new(
                  tss_base >> 32,                     /* base 63 - 32 */
                  (tss_base & 0xff000000UL) >> 24,    /* base 31 - 24 */
                  1,                                  /* granularity */
                  0,                                  /* avl */
                  0,                                  /* limit high */
                  1,                                  /* present */
                  0,                                  /* dpl */
                  9,                                  /* desc type */
                  (tss_base & 0xff0000UL) >> 16,      /* base 23-16 */
                  (tss_base & 0xffffUL),              /* base 15 - 0 */
                  કદ(tss_io_t) - 1
              );

    gdt[GDT_TSS].શબ્દો[0] = gdt_tss.શબ્દો[0];
    gdt[GDT_TSS + 1].શબ્દો[0] = gdt_tss.શબ્દો[1];
}

BOOT_CODE શૂન્ય init_idt_entry(idt_entry_t *idt, interrupt_t interrupt, શૂન્ય(*handler)(શૂન્ય))
{
    uint64_t handler_addr = (uint64_t)handler;
    uint64_t dpl = 3;

    જો (interrupt < int_trap_min && interrupt != int_software_break_request) {
        dpl = 0;
    }

    idt[interrupt] = idt_entry_interrupt_gate_new(
                         handler_addr >> 32,                 /* offset 63 - 32 */
                         ((handler_addr >> 16) & 0xffff),
                         1,                                  /* present */
                         dpl,                                /* dpl */
                         1,                                  /* ist */
                         SEL_CS_0,                           /* segment selector */
                         (handler_addr & 0xffff)               /* offset 15 - 0 */
                     );
}

શૂન્ય setVMRoot(tcb_t *tcb)
{
    cap_t thread_મૂળ;
    asid_t asid;
    pml4e_t *pml4;
    findVSpaceForASID_ret_t find_ret;
    cr3_t cr3;

    thread_મૂળ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    જો (cap_get_capType(thread_મૂળ) != cap_pml4_cap ||
        !cap_pml4_cap_get_capPML4IsMapped(thread_મૂળ)) {
        set_Current_User_VSpace_મૂળ(kpptr_to_paddr(X86_GLOBAL_VSPACE_મૂળ), 0);
        પાછા;
    }

    pml4 = PML4E_PTR(cap_pml4_cap_get_capPML4BasePtr(thread_મૂળ));
    asid = cap_pml4_cap_get_capPML4MappedASID(thread_મૂળ);
    find_ret = findVSpaceForASID(asid);
    જો (unlikely(find_ret.status != EXCEPTION_NONE || find_ret.vspace_મૂળ != pml4)) {
        set_Current_User_VSpace_મૂળ(kpptr_to_paddr(X86_GLOBAL_VSPACE_મૂળ), 0);
        પાછા;
    }
    cr3 = makeCR3(pptr_to_paddr(pml4), asid);
    જો (getCurrentUserCR3().શબ્દો[0] != cr3.શબ્દો[0]) {
        SMP_COND_STATEMENT(tlb_bitmap_set(pml4, getCurrentCPUIndex());)
        setCurrentUserCR3(cr3);
    }
}


BOOT_CODE શૂન્ય init_dtrs(શૂન્ય)
{
    gdt_idt_ptr.limit = (કદ(gdt_entry_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.base = (uint64_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt;

    /* When we install the gdt it will clobber any value of gs that
     * we have. Since we might be using it for TLS we can stash
     * and unstash any gs value using swapgs
     */
    swapgs();
    x64_install_gdt(&gdt_idt_ptr);
    swapgs();

    gdt_idt_ptr.limit = (કદ(idt_entry_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.base = (uint64_t)x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt;
    x64_install_idt(&gdt_idt_ptr);

    x64_install_ldt(SEL_NULL);

    x64_install_tss(SEL_TSS);
}

BOOT_CODE શૂન્ય map_it_frame_cap(cap_t pd_cap, cap_t frame_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_of_cap(pd_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt;
    vptr_t vptr = cap_frame_cap_get_capFMappedAddress(frame_cap);
    શૂન્ય *pptr = (શૂન્ય *)cap_frame_cap_get_capFBasePtr(frame_cap);

    પ્રતિજ્ઞા(cap_frame_cap_get_capFMapType(frame_cap) == X86_MappingVSpace);
    પ્રતિજ્ઞા(cap_frame_cap_get_capFMappedASID(frame_cap) != asidInvalid);
    pml4 += GET_PML4_INDEX(vptr);
    પ્રતિજ્ઞા(pml4e_ptr_get_present(pml4));
    pdpt = paddr_to_pptr(pml4e_ptr_get_pdpt_base_address(pml4));
    pdpt += GET_PDPT_INDEX(vptr);
    પ્રતિજ્ઞા(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_to_pptr(pdpte_pdpte_pd_ptr_get_pd_base_address(pdpt));
    pd += GET_PD_INDEX(vptr);
    પ્રતિજ્ઞા(pde_pde_pt_ptr_get_present(pd));
    pt = paddr_to_pptr(pde_pde_pt_ptr_get_pt_base_address(pd));
    *(pt + GET_PT_INDEX(vptr)) = pte_new(
                                     0,                      /* xd                   */
                                     pptr_to_paddr(pptr),    /* page_base_address    */
                                     0,                      /* global               */
                                     0,                      /* pat                  */
                                     0,                      /* dirty                */
                                     0,                      /* accessed             */
                                     0,                      /* cache_disabled       */
                                     0,                      /* write_through        */
                                     1,                      /* super_user           */
                                     1,                      /* read_write           */
                                     1                       /* present              */
                                 );
}

સ્થિર BOOT_CODE શૂન્ય map_it_pdpt_cap(cap_t vspace_cap, cap_t pdpt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_of_cap(vspace_cap));
    pdpte_t *pdpt = PDPT_PTR(cap_pdpt_cap_get_capPDPTBasePtr(pdpt_cap));
    vptr_t vptr = cap_pdpt_cap_get_capPDPTMappedAddress(pdpt_cap);

    પ્રતિજ્ઞા(cap_pdpt_cap_get_capPDPTIsMapped(pdpt_cap));
    *(pml4 + GET_PML4_INDEX(vptr)) = pml4e_new(
                                         0,                      /* xd                   */
                                         pptr_to_paddr(pdpt),    /* pdpt_base_address    */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_CODE શૂન્ય map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_of_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd = PD_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    vptr_t vptr = cap_page_directory_cap_get_capPDMappedAddress(pd_cap);

    પ્રતિજ્ઞા(cap_page_directory_cap_get_capPDIsMapped(pd_cap));
    pml4 += GET_PML4_INDEX(vptr);
    પ્રતિજ્ઞા(pml4e_ptr_get_present(pml4));
    pdpt = paddr_to_pptr(pml4e_ptr_get_pdpt_base_address(pml4));
    *(pdpt + GET_PDPT_INDEX(vptr)) = pdpte_pdpte_pd_new(
                                         0,                      /* xd                   */
                                         pptr_to_paddr(pd),      /* pd_base_address      */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_CODE શૂન્ય map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_of_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt = PT_PTR(cap_page_table_cap_get_capPTBasePtr(pt_cap));
    vptr_t vptr = cap_page_table_cap_get_capPTMappedAddress(pt_cap);

    પ્રતિજ્ઞા(cap_page_table_cap_get_capPTIsMapped(pt_cap));
    pml4 += GET_PML4_INDEX(vptr);
    પ્રતિજ્ઞા(pml4e_ptr_get_present(pml4));
    pdpt = paddr_to_pptr(pml4e_ptr_get_pdpt_base_address(pml4));
    pdpt += GET_PDPT_INDEX(vptr);
    પ્રતિજ્ઞા(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_to_pptr(pdpte_pdpte_pd_ptr_get_pd_base_address(pdpt));
    *(pd + GET_PD_INDEX(vptr)) = pde_pde_pt_new(
                                     0,                      /* xd                   */
                                     pptr_to_paddr(pt),      /* pt_base_address      */
                                     0,                      /* accessed             */
                                     0,                      /* cache_disabled       */
                                     0,                      /* write_through        */
                                     1,                      /* super_user           */
                                     1,                      /* read_write           */
                                     1                       /* present              */
                                 );
}

BOOT_CODE શૂન્ય *map_temp_boot_page(શૂન્ય *entry, uint32_t large_pages)
{
    /* this function is for legacy 32-bit systems where the ACPI tables might
     * collide with the kernel window. Here we just assert that the table is
     * in fact in the lower 4GiB region (which is already 1-to-1 mapped) and
     * continue */
    પ્રતિજ્ઞા((word_t)entry < BIT(32));
    પાછા entry;
}

સ્થિર BOOT_CODE cap_t create_it_pdpt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_pdpt_cap_new(
              asid,   /* capPDPTMappedASID    */
              pptr,   /* capPDPTBasePtr       */
              1,      /* capPDPTIsMapped      */
              vptr    /* capPDPTMappedAddress */
          );
    map_it_pdpt_cap(vspace_cap, cap);
    પાછા cap;
}

સ્થિર BOOT_CODE cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_directory_cap_new(
              asid,   /* capPDMappedASID      */
              pptr,   /* capPDBasePtr         */
              1,      /* capPDIsMapped        */
              vptr    /* capPDMappedAddress   */
          );
    map_it_pd_cap(vspace_cap, cap);
    પાછા cap;
}

સ્થિર BOOT_CODE cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_table_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );
    map_it_pt_cap(vspace_cap, cap);
    પાછા cap;
}


BOOT_CODE word_t arch_get_n_paging(v_region_t it_v_reg)
{
    word_t n = get_n_paging(it_v_reg, PD_INDEX_OFFSET);
    n += get_n_paging(it_v_reg, PDPT_INDEX_OFFSET);
    n += get_n_paging(it_v_reg, PML4_INDEX_OFFSET);
#ifdef CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#endif
    પાછા n;
}

BOOT_CODE cap_t create_it_address_space(cap_t મૂળ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    slot_pos_before = ndks_boot.slot_pos_cur;
    copyGlobalMappings(PML4_PTR(rootserver.vspace));
    vspace_cap = cap_pml4_cap_new(
                     IT_ASID,        /* capPML4MappedASID */
                     rootserver.vspace,           /* capPML4BasePtr   */
                     1               /* capPML4IsMapped   */
                 );


    write_slot(SLOT_PTR(pptr_of_cap(મૂળ_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

    /* Create any PDPTs needed for the user land image */
    માટે (vptr = ROUND_DOWN(it_v_reg.start, PML4_INDEX_OFFSET);
         vptr < it_v_reg.end;
         vptr += BIT(PML4_INDEX_OFFSET)) {
        જો (!provide_cap(મૂળ_cnode_cap,
                         create_it_pdpt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            પાછા cap_null_cap_new();
        }
    }

    /* Create any PDs needed for the user land image */
    માટે (vptr = ROUND_DOWN(it_v_reg.start, PDPT_INDEX_OFFSET);
         vptr < it_v_reg.end;
         vptr += BIT(PDPT_INDEX_OFFSET)) {
        જો (!provide_cap(મૂળ_cnode_cap,
                         create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            પાછા cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    માટે (vptr = ROUND_DOWN(it_v_reg.start, PD_INDEX_OFFSET);
         vptr < it_v_reg.end;
         vptr += BIT(PD_INDEX_OFFSET)) {
        જો (!provide_cap(મૂળ_cnode_cap,
                         create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            પાછા cap_null_cap_new();
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;
    ndks_boot.bi_frame->userImagePaging = (seL4_SlotRegion) {
        slot_pos_before, slot_pos_after
    };
    પાછા vspace_cap;
}

શૂન્ય copyGlobalMappings(vspace_મૂળ_t *new_vspace)
{
    unsigned long i;
    pml4e_t *vspace = (pml4e_t *)new_vspace;

    /* Copy from the tlbbitmap_pptr so that we copy the default entries of the
     * tlb bitmap (if it exists). If it doesn't exist then this loop
     * will be equivalent to copying from PPTR_BASE
     */
    માટે (i = GET_PML4_INDEX(TLBBITMAP_PPTR); i < BIT(PML4_INDEX_BITS); i++) {
        vspace[i] = X86_GLOBAL_VSPACE_મૂળ[i];
    }
}

સ્થિર BOOT_CODE cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large, seL4_Word map_type)
{
    vm_page_size_t frame_size;

    જો (use_large) {
        frame_size = X86_LargePage;
    } નહીંતર {
        frame_size = X86_SmallPage;
    }

    પાછા
        cap_frame_cap_new(
            asid,                          /* capFMappedASID     */
            pptr,                          /* capFBasePtr        */
            frame_size,                    /* capFSize           */
            map_type,                      /* capFMapType        */
            vptr,                          /* capFMappedAddress  */
            wordFromVMRights(VMReadWrite), /* capFVMRights       */
            0                              /* capFIsDevice       */
        );
}

BOOT_CODE cap_t create_unmapped_it_frame_cap(pptr_t pptr, bool_t use_large)
{
    પાછા create_it_frame_cap(pptr, 0, asidInvalid, use_large, X86_MappingNone);
}

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable UNUSED)
{
    cap_t cap = create_it_frame_cap(pptr, vptr, asid, use_large, X86_MappingVSpace);
    map_it_frame_cap(vspace_cap, cap);
    પાછા cap;
}

/* ====================== BOOT CODE FINISHES HERE ======================== */



exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspaceCapSlot)
{
    asid_map_t asid_map;
#ifdef CONFIG_VTX
    જો (cap_get_capType(vspaceCapSlot->cap) == cap_ept_pml4_cap) {
        cap_ept_pml4_cap_ptr_set_capPML4MappedASID(&vspaceCapSlot->cap, asid);
        cap_ept_pml4_cap_ptr_set_capPML4IsMapped(&vspaceCapSlot->cap, 1);
        asid_map = asid_map_asid_map_ept_new(cap_ept_pml4_cap_get_capPML4BasePtr(vspaceCapSlot->cap));
    } નહીંતર
#endif
    {
        પ્રતિજ્ઞા(cap_get_capType(vspaceCapSlot->cap) == cap_pml4_cap);
        cap_pml4_cap_ptr_set_capPML4MappedASID(&vspaceCapSlot->cap, asid);
        cap_pml4_cap_ptr_set_capPML4IsMapped(&vspaceCapSlot->cap, 1);
        asid_map = asid_map_asid_map_vspace_new(cap_pml4_cap_get_capPML4BasePtr(vspaceCapSlot->cap));
    }
    poolPtr->array[ASID_નીચલું(asid)] = asid_map;
    પાછા EXCEPTION_NONE;
}

bool_t CONST is_VTable_મૂળ(cap_t cap)
{
    પાછા cap_get_capType(cap) == cap_pml4_cap;
}

bool_t CONST is_Valid_Native_મૂળ(cap_t cap)
{
    પાછા is_VTable_મૂળ(cap) &&
           cap_pml4_cap_get_capPML4IsMapped(cap);
}

સ્થિર pml4e_t CONST makeUserPML4E(paddr_t paddr, vm_attributes_t vm_attr)
{
    પાછા pml4e_new(
               0,
               paddr,
               0,
               vm_attributes_get_x86PCDBit(vm_attr),
               vm_attributes_get_x86PWTBit(vm_attr),
               1,
               1,
               1
           );
}

સ્થિર pml4e_t CONST makeUserPML4EInvalid(શૂન્ય)
{
    પાછા pml4e_new(
               0,                  /* xd               */
               0,                  /* pdpt_base_addr   */
               0,                  /* accessed         */
               0,                  /* cache_disabled   */
               0,                  /* write through    */
               0,                  /* super user       */
               0,                  /* read_write       */
               0                   /* present          */
           );
}

સ્થિર pdpte_t CONST makeUserPDPTEHugePage(paddr_t paddr, vm_attributes_t vm_attr, vm_rights_t vm_rights)
{
    પાછા pdpte_pdpte_1g_new(
               0,          /* xd               */
               paddr,      /* physical address */
               0,          /* PAT              */
               0,          /* global           */
               0,          /* dirty            */
               0,          /* accessed         */
               vm_attributes_get_x86PCDBit(vm_attr),  /* cache disabled */
               vm_attributes_get_x86PWTBit(vm_attr),  /* write through  */
               SuperUserFromVMRights(vm_rights),       /* super user     */
               WritableFromVMRights(vm_rights),        /* read write     */
               1                                       /* present        */
           );
}

સ્થિર pdpte_t CONST makeUserPDPTEPageDirectory(paddr_t paddr, vm_attributes_t vm_attr)
{
    પાછા pdpte_pdpte_pd_new(
               0,                      /* xd       */
               paddr,                  /* paddr    */
               0,                      /* accessed */
               vm_attributes_get_x86PCDBit(vm_attr),  /* cache disabled */
               vm_attributes_get_x86PWTBit(vm_attr),  /* write through  */
               1,                      /* super user */
               1,                      /* read write */
               1                       /* present    */
           );
}

સ્થિર pdpte_t CONST makeUserPDPTEInvalid(શૂન્ય)
{
    પાછા pdpte_pdpte_pd_new(
               0,          /* xd               */
               0,          /* physical address */
               0,          /* accessed         */
               0,          /* cache disabled */
               0,          /* write through  */
               0,          /* super user     */
               0,          /* read write     */
               0           /* present        */
           );
}

pde_t CONST makeUserPDELargePage(paddr_t paddr, vm_attributes_t vm_attr, vm_rights_t vm_rights)
{
    પાછા pde_pde_large_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               vm_attributes_get_x86PATBit(vm_attr),          /* pat                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_attributes_get_x86PCDBit(vm_attr),          /* cache_disabled       */
               vm_attributes_get_x86PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST makeUserPDEPageTable(paddr_t paddr, vm_attributes_t vm_attr)
{

    પાછા  pde_pde_pt_new(
                0,                                  /* xd               */
                paddr,                              /* pt_base_address  */
                0,                                  /* accessed         */
                vm_attributes_get_x86PCDBit(vm_attr), /* cache_disabled   */
                vm_attributes_get_x86PWTBit(vm_attr), /* write_through    */
                1,                                  /* super_user       */
                1,                                  /* read_write       */
                1                                   /* present          */
            );
}

pde_t CONST makeUserPDEInvalid(શૂન્ય)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    પાછા pde_pde_pt_new(
               0,      /* xd               */
               0,      /* pt_base_addr     */
               0,      /* accessed         */
               0,      /* cache_disabled   */
               0,      /* write_through    */
               0,      /* super_user       */
               0,      /* read_write       */
               0       /* present          */
           );
}

pte_t CONST makeUserPTE(paddr_t paddr, vm_attributes_t vm_attr, vm_rights_t vm_rights)
{
    પાછા pte_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               0,                                              /* global               */
               vm_attributes_get_x86PATBit(vm_attr),          /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_attributes_get_x86PCDBit(vm_attr),          /* cache_disabled       */
               vm_attributes_get_x86PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pte_t CONST makeUserPTEInvalid(શૂન્ય)
{
    પાછા pte_new(
               0,                   /* xd                   */
               0,                   /* page_base_address    */
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


સ્થિર pml4e_t *lookupPML4Slot(vspace_મૂળ_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4e = PML4E_PTR(pml4);
    word_t pml4Index = GET_PML4_INDEX(vptr);
    પાછા pml4e + pml4Index;
}

સ્થિર lookupPDPTSlot_ret_t lookupPDPTSlot(vspace_મૂળ_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4Slot = lookupPML4Slot(pml4, vptr);
    lookupPDPTSlot_ret_t ret;

    જો (!pml4e_ptr_get_present(pml4Slot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PML4_INDEX_OFFSET);

        ret.pdptSlot = ખાલી;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        પાછા ret;
    } નહીંતર {
        pdpte_t *pdpt;
        pdpte_t *pdptSlot;
        word_t pdptIndex = GET_PDPT_INDEX(vptr);
        pdpt = paddr_to_pptr(pml4e_ptr_get_pdpt_base_address(pml4Slot));
        pdptSlot = pdpt + pdptIndex;

        ret.status = EXCEPTION_NONE;
        ret.pdptSlot = pdptSlot;
        પાછા ret;
    }
}

lookupPDSlot_ret_t lookupPDSlot(vspace_મૂળ_t *pml4, vptr_t vptr)
{
    lookupPDPTSlot_ret_t pdptSlot;
    lookupPDSlot_ret_t ret;

    pdptSlot = lookupPDPTSlot(pml4, vptr);

    જો (pdptSlot.status != EXCEPTION_NONE) {
        ret.pdSlot = ખાલી;
        ret.status = pdptSlot.status;
        પાછા ret;
    }
    જો ((pdpte_ptr_get_page_size(pdptSlot.pdptSlot) != pdpte_pdpte_pd) ||
        !pdpte_pdpte_pd_ptr_get_present(pdptSlot.pdptSlot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PDPT_INDEX_OFFSET);

        ret.pdSlot = ખાલી;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        પાછા ret;
    } નહીંતર {
        pde_t *pd;
        pde_t *pdSlot;
        word_t pdIndex = GET_PD_INDEX(vptr);
        pd = paddr_to_pptr(pdpte_pdpte_pd_ptr_get_pd_base_address(pdptSlot.pdptSlot));
        pdSlot = pd + pdIndex;

        ret.status = EXCEPTION_NONE;
        ret.pdSlot = pdSlot;
        પાછા ret;
    }
}

સ્થિર શૂન્ય flushPD(vspace_મૂળ_t *vspace, word_t vptr, pde_t *pd, asid_t asid)
{
    /* clearing the entire PCID vs flushing the virtual addresses
     * one by one using invplg.
     * choose the easy way, invalidate the PCID
     */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));

}

સ્થિર શૂન્ય flushPDPT(vspace_મૂળ_t *vspace, word_t vptr, pdpte_t *pdpt, asid_t asid)
{
    /* similar here */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    પાછા;
}

શૂન્ય hwASIDInvalidate(asid_t asid, vspace_મૂળ_t *vspace)
{
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
}

શૂન્ય unmapPageDirectory(asid_t asid, vptr_t vaddr, pde_t *pd)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPDPTSlot_ret_t    lu_ret;

    find_ret = findVSpaceForASID(asid);
    જો (find_ret.status != EXCEPTION_NONE) {
        પાછા;
    }

    lu_ret = lookupPDPTSlot(find_ret.vspace_મૂળ, vaddr);
    જો (lu_ret.status != EXCEPTION_NONE) {
        પાછા;
    }

    /* check if the PDPT has the PD */
    જો (!(pdpte_ptr_get_page_size(lu_ret.pdptSlot) == pdpte_pdpte_pd &&
          pdpte_pdpte_pd_ptr_get_present(lu_ret.pdptSlot) &&
          (pdpte_pdpte_pd_ptr_get_pd_base_address(lu_ret.pdptSlot) == pptr_to_paddr(pd)))) {
        પાછા;
    }

    flushPD(find_ret.vspace_મૂળ, vaddr, pd, asid);

    *lu_ret.pdptSlot = makeUserPDPTEInvalid();

    invalidatePageStructureCacheASID(pptr_to_paddr(find_ret.vspace_મૂળ), asid,
                                     SMP_TERNARY(tlb_bitmap_get(find_ret.vspace_મૂળ), 0));
}


સ્થિર exception_t performX64PageDirectoryInvocationUnmap(cap_t cap, cte_t *ctSlot)
{

    જો (cap_page_directory_cap_get_capPDIsMapped(cap)) {
        pde_t *pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap));
        unmapPageDirectory(
            cap_page_directory_cap_get_capPDMappedASID(cap),
            cap_page_directory_cap_get_capPDMappedAddress(cap),
            pd
        );
        clearMemory((શૂન્ય *)pd, cap_get_capSizeBits(cap));
    }

    cap_page_directory_cap_ptr_set_capPDIsMapped(&(ctSlot->cap), 0);

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performX64PageDirectoryInvocationMap(cap_t cap, cte_t *ctSlot, pdpte_t pdpte, pdpte_t *pdptSlot,
                                                        vspace_મૂળ_t *vspace)
{
    ctSlot->cap = cap;
    *pdptSlot = pdpte;
    invalidatePageStructureCacheASID(pptr_to_paddr(vspace), cap_page_directory_cap_get_capPDMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    પાછા EXCEPTION_NONE;
}


સ્થિર exception_t decodeX64PageDirectoryInvocation(
    word_t label,
    word_t length,
    cte_t *cte,
    cap_t cap,
    word_t *buffer
)
{
    word_t              vaddr;
    vm_attributes_t     vm_attr;
    cap_t               vspaceCap;
    vspace_મૂળ_t      *vspace;
    pdpte_t             pdpte;
    paddr_t             paddr;
    asid_t              asid;
    lookupPDPTSlot_ret_t pdptSlot;

    જો (label == X86PageDirectoryUnmap) {
        જો (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("X86PageDirectory: Cannot unmap if more than one cap exist.");
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);

        પાછા performX64PageDirectoryInvocationUnmap(cap, cte);
    }

    જો (label != X86PageDirectoryMap) {
        userError("X64Directory: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (length < 2 || current_extra_caps.excaprefs[0] == ખાલી) {
        userError("X64PageDirectory: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (cap_page_directory_cap_get_capPDIsMapped(cap)) {
        userError("X64PageDirectory: PD is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer) & (~MASK(PDPT_INDEX_OFFSET));
    vm_attr = vmAttributesFromWord(getSyscallArg(1, buffer));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    જો (!is_Valid_Native_મૂળ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_મૂળ_t *)pptr_of_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    જો (vaddr > USER_ટોચ) {
        userError("X64PageDirectory: Mapping address too high.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 0;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    findVSpaceForASID_ret_t find_ret;

    find_ret = findVSpaceForASID(asid);
    જો (find_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (find_ret.vspace_મૂળ != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    pdptSlot = lookupPDPTSlot(vspace, vaddr);
    જો (pdptSlot.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો ((pdpte_ptr_get_page_size(pdptSlot.pdptSlot) == pdpte_pdpte_pd &&
         pdpte_pdpte_pd_ptr_get_present(pdptSlot.pdptSlot)) ||
        (pdpte_ptr_get_page_size(pdptSlot.pdptSlot) == pdpte_pdpte_1g
         && pdpte_pdpte_1g_ptr_get_present(pdptSlot.pdptSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr(PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
    pdpte = makeUserPDPTEPageDirectory(paddr, vm_attr);

    cap = cap_page_directory_cap_set_capPDIsMapped(cap, 1);
    cap = cap_page_directory_cap_set_capPDMappedASID(cap, asid);
    cap = cap_page_directory_cap_set_capPDMappedAddress(cap, vaddr);

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા performX64PageDirectoryInvocationMap(cap, cte, pdpte, pdptSlot.pdptSlot, vspace);
}

સ્થિર શૂન્ય unmapPDPT(asid_t asid, vptr_t vaddr, pdpte_t *pdpt)
{
    findVSpaceForASID_ret_t find_ret;
    pml4e_t *pml4Slot;

    find_ret = findVSpaceForASID(asid);
    જો (find_ret.status != EXCEPTION_NONE) {
        પાછા;
    }

    pml4Slot = lookupPML4Slot(find_ret.vspace_મૂળ, vaddr);

    /* check if the PML4 has the PDPT */
    જો (!(pml4e_ptr_get_present(pml4Slot) &&
          pml4e_ptr_get_pdpt_base_address(pml4Slot) == pptr_to_paddr(pdpt))) {
        પાછા;
    }

    flushPDPT(find_ret.vspace_મૂળ, vaddr, pdpt, asid);

    *pml4Slot = makeUserPML4EInvalid();
}

સ્થિર exception_t performX64PDPTInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    જો (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        pdpte_t *pdpt = PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap));
        unmapPDPT(cap_pdpt_cap_get_capPDPTMappedASID(cap),
                  cap_pdpt_cap_get_capPDPTMappedAddress(cap),
                  pdpt);
        clearMemory((શૂન્ય *)pdpt, cap_get_capSizeBits(cap));
    }

    cap_pdpt_cap_ptr_set_capPDPTIsMapped(&(ctSlot->cap), 0);

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performX64PDPTInvocationMap(cap_t cap, cte_t *ctSlot, pml4e_t pml4e, pml4e_t *pml4Slot,
                                               vspace_મૂળ_t *vspace)
{
    ctSlot->cap = cap;
    *pml4Slot = pml4e;
    invalidatePageStructureCacheASID(pptr_to_paddr(vspace), cap_pdpt_cap_get_capPDPTMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));

    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t decodeX64PDPTInvocation(
    word_t  label,
    word_t length,
    cte_t   *cte,
    cap_t   cap,
    word_t  *buffer)
{
    word_t                  vaddr;
    vm_attributes_t         attr;
    pml4e_t                *pml4Slot;
    cap_t                   vspaceCap;
    vspace_મૂળ_t          *vspace;
    pml4e_t                 pml4e;
    paddr_t                 paddr;
    asid_t                  asid;

    જો (label == X86PDPTUnmap) {
        જો (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("X86PDPT: Cannot unmap if more than one cap exist.");
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);

        પાછા performX64PDPTInvocationUnmap(cap, cte);
    }

    જો (label != X86PDPTMap) {
        userError("X86PDPT: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (length < 2 || current_extra_caps.excaprefs[0] == ખાલી) {
        userError("X64PDPT: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X64PDPT: PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 0;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer) & (~MASK(PML4_INDEX_OFFSET));
    attr = vmAttributesFromWord(getSyscallArg(1, buffer));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    જો (!is_Valid_Native_મૂળ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_મૂળ_t *)pptr_of_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    જો (vaddr > USER_ટોચ) {
        userError("X64PDPT: Mapping address too high.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 0;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    findVSpaceForASID_ret_t find_ret;

    find_ret = findVSpaceForASID(asid);
    જો (find_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (find_ret.vspace_મૂળ != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_સંખ્યા = 1;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    pml4Slot = lookupPML4Slot(vspace, vaddr);

    જો (pml4e_ptr_get_present(pml4Slot)) {
        current_syscall_error.type = seL4_DeleteFirst;

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr(PDPTE_PTR((cap_pdpt_cap_get_capPDPTBasePtr(cap))));
    pml4e = makeUserPML4E(paddr, attr);

    cap = cap_pdpt_cap_set_capPDPTIsMapped(cap, 1);
    cap = cap_pdpt_cap_set_capPDPTMappedASID(cap, asid);
    cap = cap_pdpt_cap_set_capPDPTMappedAddress(cap, vaddr);

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા performX64PDPTInvocationMap(cap, cte, pml4e, pml4Slot, vspace);
}

exception_t decodeX86ModeMMUInvocation(
    word_t label,
    word_t length,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    પસંદગી (cap_get_capType(cap)) {

    સ્થિતિ cap_pml4_cap:
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;

    સ્થિતિ cap_pdpt_cap:
        પાછા decodeX64PDPTInvocation(label, length, cte, cap, buffer);

    સ્થિતિ cap_page_directory_cap:
        પાછા decodeX64PageDirectoryInvocation(label, length, cte, cap, buffer);

    મૂળભૂત:
        fail("Invalid arch cap type");
    }
}

bool_t modeUnmapPage(vm_page_size_t page_size, vspace_મૂળ_t *vroot, vptr_t vaddr, શૂન્ય *pptr)
{
    જો (config_set(CONFIG_HUGE_PAGE) && page_size == X64_HugePage) {
        pdpte_t     *pdpte;
        lookupPDPTSlot_ret_t pdpt_ret = lookupPDPTSlot(vroot, vaddr);
        જો (pdpt_ret.status != EXCEPTION_NONE) {
            પાછા false;
        }
        pdpte = pdpt_ret.pdptSlot;


        જો (!(pdpte_ptr_get_page_size(pdpte) == pdpte_pdpte_1g
              && pdpte_pdpte_1g_ptr_get_present(pdpte)
              && (pdpte_pdpte_1g_ptr_get_page_base_address(pdpte)
                  == pptr_to_paddr(pptr)))) {
            પાછા false;
        }

        *pdpte = makeUserPDPTEInvalid();
        પાછા true;
    }
    fail("Invalid page type");
    પાછા false;
}

સ્થિર exception_t updatePDPTE(asid_t asid, pdpte_t pdpte, pdpte_t *pdptSlot, vspace_મૂળ_t *vspace)
{
    *pdptSlot = pdpte;
    invalidatePageStructureCacheASID(pptr_to_paddr(vspace), asid,
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t performX64ModeMap(cap_t cap, cte_t *ctSlot, pdpte_t pdpte, pdpte_t *pdptSlot, vspace_મૂળ_t *vspace)
{
    ctSlot->cap = cap;
    પાછા updatePDPTE(cap_frame_cap_get_capFMappedASID(cap), pdpte, pdptSlot, vspace);
}

સંરચના create_mapping_pdpte_return {
    exception_t status;
    pdpte_t pdpte;
    pdpte_t *pdptSlot;
};
પ્રકારવ્યાખ્યા સંરચના create_mapping_pdpte_return create_mapping_pdpte_return_t;

સ્થિર create_mapping_pdpte_return_t createSafeMappingEntries_PDPTE(paddr_t base, word_t vaddr, vm_rights_t vmRights,
                                                                    vm_attributes_t attr,
                                                                    vspace_મૂળ_t *vspace)
{
    create_mapping_pdpte_return_t ret;
    lookupPDPTSlot_ret_t          lu_ret;

    lu_ret = lookupPDPTSlot(vspace, vaddr);
    જો (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        પાછા ret;
    }
    ret.pdptSlot = lu_ret.pdptSlot;

    /* check for existing page directory */
    જો ((pdpte_ptr_get_page_size(ret.pdptSlot) == pdpte_pdpte_pd) &&
        (pdpte_pdpte_pd_ptr_get_present(ret.pdptSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        પાછા ret;
    }

    ret.pdpte = makeUserPDPTEHugePage(base, attr, vmRights);
    ret.status = EXCEPTION_NONE;
    પાછા ret;
}

exception_t decodeX86ModeMapPage(word_t label, vm_page_size_t page_size, cte_t *cte, cap_t cap,
                                 vspace_મૂળ_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_attributes_t vm_attr)
{
    જો (config_set(CONFIG_HUGE_PAGE) && page_size == X64_HugePage) {
        create_mapping_pdpte_return_t map_ret;

        map_ret = createSafeMappingEntries_PDPTE(paddr, vaddr, vm_rights, vm_attr, vroot);
        જો (map_ret.status != EXCEPTION_NONE) {
            પાછા map_ret.status;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);

        પસંદગી (label) {
        સ્થિતિ X86PageMap:
            પાછા performX64ModeMap(cap, cte, map_ret.pdpte, map_ret.pdptSlot, vroot);

        મૂળભૂત:
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
    }
    fail("Invalid Page type");
}

#ifdef CONFIG_PRINTING
પ્રકારવ્યાખ્યા સંરચના readWordFromVSpace_ret {
    exception_t status;
    word_t કિંમત;
} readWordFromVSpace_ret_t;

સ્થિર readWordFromVSpace_ret_t readWordFromVSpace(vspace_મૂળ_t *vspace, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    lookupPTSlot_ret_t ptSlot;
    lookupPDSlot_ret_t pdSlot;
    lookupPDPTSlot_ret_t pdptSlot;
    paddr_t paddr;
    word_t offset;
    pptr_t kernel_vaddr;
    word_t *કિંમત;

    pdptSlot = lookupPDPTSlot(vspace, vaddr);
    જો (pdptSlot.status == EXCEPTION_NONE &&
        pdpte_ptr_get_page_size(pdptSlot.pdptSlot) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdptSlot.pdptSlot)) {

        paddr = pdpte_pdpte_1g_ptr_get_page_base_address(pdptSlot.pdptSlot);
        offset = vaddr & MASK(seL4_HugePageBits);
    } નહીંતર {
        pdSlot = lookupPDSlot(vspace, vaddr);
        જો (pdSlot.status == EXCEPTION_NONE &&
            ((pde_ptr_get_page_size(pdSlot.pdSlot) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pdSlot.pdSlot))) {

            paddr = pde_pde_large_ptr_get_page_base_address(pdSlot.pdSlot);
            offset = vaddr & MASK(seL4_LargePageBits);
        } નહીંતર {
            ptSlot = lookupPTSlot(vspace, vaddr);
            જો (ptSlot.status == EXCEPTION_NONE && pte_ptr_get_present(ptSlot.ptSlot)) {
                paddr = pte_ptr_get_page_base_address(ptSlot.ptSlot);
                offset = vaddr & MASK(seL4_PageBits);
            } નહીંતર {
                ret.status = EXCEPTION_LOOKUP_FAULT;
                પાછા ret;
            }
        }
    }


    kernel_vaddr = (word_t)paddr_to_pptr(paddr);
    કિંમત = (word_t *)(kernel_vaddr + offset);
    ret.status = EXCEPTION_NONE;
    ret.કિંમત = *કિંમત;
    પાછા ret;
}

શૂન્ય Arch_user_ઢગલો_Trace(tcb_t *tptr)
{
    cap_t thread_મૂળ;
    vspace_મૂળ_t *vspace_મૂળ;
    word_t sp;
    પૂર્ણાંક i;

    thread_મૂળ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    જો (cap_get_capType(thread_મૂળ) != cap_pml4_cap) {
        printf("Invalid vspace\n");
        પાછા;
    }

    vspace_મૂળ = (vspace_મૂળ_t *)pptr_of_cap(thread_મૂળ);

    sp = getRegister(tptr, RSP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    જો (!IS_ALIGNED(sp, seL4_WordSizeBits)) {
        printf("RSP not aligned\n");
        પાછા;
    }

    માટે (i = 0; i < CONFIG_USER_ઢગલો_TRACE_LENGTH; i++) {
        word_t address = sp + (i * કદ(word_t));
        readWordFromVSpace_ret_t result;
        result = readWordFromVSpace(vspace_મૂળ, address);
        જો (result.status == EXCEPTION_NONE) {
            printf("0x%lx: 0x%lx\n", (long)address, (long)result.કિંમત);
        } નહીંતર {
            printf("0x%lx: INVALID\n", (long)address);
        }
    }
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_KERNEL_LOG_BUFFER
exception_t benchmark_arch_map_logBuffer(word_t frame_cptr)
{
    lookupCapAndSlot_ret_t lu_ret;
    vm_page_size_t frameSize;
    pptr_t frame_pptr;

    /* faulting section */
    lu_ret = lookupCapAndSlot(ગાંઠ_STATE(ksCurThread), frame_cptr);

    જો (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", frame_cptr);
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (cap_get_capType(lu_ret.cap) != cap_frame_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    frameSize = cap_frame_cap_get_capFSize(lu_ret.cap);

    જો (frameSize != X86_LargePage) {
        userError("Invalid size for log Buffer. The kernel expects at least 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(frame_cptr, false);

        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    frame_pptr = cap_frame_cap_get_capFBasePtr(lu_ret.cap);

    ksUserLogBuffer = pptr_to_paddr((શૂન્ય *) frame_pptr);

    pde_t pde = pde_pde_large_new(
                    0,                 /* xd                   */
                    ksUserLogBuffer,   /* page_base_address    */
                    VMKernelOnly,      /* pat                  */
                    1,                 /* global               */
                    0,                 /* dirty                */
                    0,                 /* accessed             */
                    0,                 /* cache_disabled       */
                    1,                 /* write_through        */
                    1,                 /* super_user           */
                    1,                 /* read_write           */
                    1                  /* present              */
                );

    /* Stored in the PD slot after the device page table */
#ifdef CONFIG_HUGE_PAGE
    x64KSKernelPD[1] = pde;
#નહીંતર
    x64KSKernelPDs[BIT(PDPT_INDEX_BITS) - 1][1] = pde;
#endif
    invalidateTranslationAll(MASK(CONFIG_MAX_અંક_NODES));

    પાછા EXCEPTION_NONE;
}
#endif /* CONFIG_KERNEL_LOG_BUFFER */
