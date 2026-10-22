/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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
#अन्तर्भाव <mode/kernel/tlb.h>
#अन्तर्भाव <arch/kernel/tlb_bitmap.h>
#अन्तर्भाव <object/structures.h>

/* When using the SKIM window to isolate the kernel from the user we also need to
 * not use global mappings as having global mappings and entries in the TLB is
 * equivalent, for the purpose of exploitation, to having the mappings in the
 * kernel window */
#परिभाषय KERNEL_IS_GLOBAL() (config_समुच्चयः(CONFIG_KERNEL_SKIM_गवाक्षः) ? 0 : 1)

/* For the boot code we create two windows into the physical address space
 * One is at the same location as the kernel window, and is placed up high
 * The other is a 1-to-1 mapping of the first 512gb of memory. The purpose
 * of this is to have a 1-to-1 mapping for the low parts of memory, so that
 * when we switch paging on, and are still running at physical addresses,
 * we don't explode. Then we also want the high mappings so we can start
 * running at proper kernel virtual addresses */
pml4e_t boot_pml4[BIT(PML4_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_Page_द्वयङ्काः)) VISIBLE PHYS_BSS;
pdpte_t boot_pdpt[BIT(PDPT_सूचकांक_द्वयङ्काः)] ALIGN(BIT(se_L4_Page_द्वयङ्काः)) VISIBLE PHYS_BSS;

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

BOOT_संहिता bool_t map_kernel_गवाक्षः(
    uint32_t अङ्कः_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   अङ्कः_drhu,
    paddr_t   *drhu_list
)
{

    uint64_t paddr;
    uint64_t vaddr;

#यदि_परिभाषितम् CONFIG_HUGE_PAGE
    /* using 1 GiB page size */

    /* verify that the kernel window as at the last entry of the PML4 */
    प्रतिज्ञा(GET_PML4_सूचकांक(PPTR_आधारः) == BIT(PML4_सूचकांक_द्वयङ्काः) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    प्रतिज्ञा(GET_PML4_सूचकांक(KERNEL_ELF_आधारः) == BIT(PML4_सूचकांक_द्वयङ्काः) - 1);
    प्रतिज्ञा(GET_PDPT_सूचकांक(KERNEL_ELF_आधारः) == BIT(PML4_सूचकांक_द्वयङ्काः) - 2);
    प्रतिज्ञा(GET_PDPT_सूचकांक(KDEV_आधारः) == BIT(PML4_सूचकांक_द्वयङ्काः) - 1);
    प्रतिज्ञा(IS_ALIGNED(KERNEL_ELF_आधारः - KERNEL_ELF_PADDR_आधारः, se_L4_Huge_Page_द्वयङ्काः));
    प्रतिज्ञा(IS_ALIGNED(KDEV_आधारः, se_L4_Huge_Page_द्वयङ्काः));
    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_सूचकांक(PPTR_आधारः)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_गन्तव्यम्_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );
    /* put the 1GB kernel_base mapping into the PDPT */
    x64KSKernelPDPT[GET_PDPT_सूचकांक(KERNEL_ELF_आधारः)] = pdpte_pdpte_1g_new(
                                                           0, /* xd */
                                                           PADDR_आधारः,
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
    vaddr = PPTR_आधारः;
    प्रति (paddr = 0; paddr < PADDR_शिखरम्;
         paddr += BIT(se_L4_Huge_Page_द्वयङ्काः)) {

        पूर्णाङ्क pdpte_सूचकांक = GET_PDPT_सूचकांक(vaddr);
        x64KSKernelPDPT[pdpte_सूचकांक] = pdpte_pdpte_1g_new(
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

        vaddr += BIT(se_L4_Huge_Page_द्वयङ्काः);
    }

    /* put the PD into the PDPT */
    x64KSKernelPDPT[GET_PDPT_सूचकांक(KDEV_आधारः)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_गन्तव्यम्_paddr(x64KSKernelPD),
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
                           kpptr_गन्तव्यम्_paddr(x64KSKernelPT),
                           0, /* accessed */
                           0, /* cache_disabled */
                           0, /* write_through */
                           0, /* super_user */
                           1, /* read_write */
                           1  /* present */
                       );
#अन्यथा

    पूर्णाङ्क pd_सूचकांक_2 = 0;
    /* use 2 MiB page size */
    /* verify that the kernel window as at the last entry of the PML4 */
    प्रतिज्ञा(GET_PML4_सूचकांक(PPTR_आधारः) == BIT(PML4_सूचकांक_द्वयङ्काः) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    प्रतिज्ञा(GET_PML4_सूचकांक(KERNEL_ELF_आधारः) == BIT(PML4_सूचकांक_द्वयङ्काः) - 1);
    प्रतिज्ञा(GET_PDPT_सूचकांक(KERNEL_ELF_आधारः) == BIT(PML4_सूचकांक_द्वयङ्काः) - 2);
    प्रतिज्ञा(GET_PDPT_सूचकांक(KDEV_आधारः) == BIT(PML4_सूचकांक_द्वयङ्काः) - 1);
    प्रतिज्ञा(IS_ALIGNED(KERNEL_ELF_आधारः - KERNEL_ELF_PADDR_आधारः, se_L4_Huge_Page_द्वयङ्काः));
    प्रतिज्ञा(IS_ALIGNED(KDEV_आधारः, se_L4_Huge_Page_द्वयङ्काः));

    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_सूचकांक(PPTR_आधारः)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_गन्तव्यम्_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    प्रति (pd_सूचकांक_2 = 0; pd_सूचकांक_2 < PADDR_शिखरम् >> se_L4_Huge_Page_द्वयङ्काः; pd_सूचकांक_2++) {
        /* put the 1GB kernel_base mapping into the PDPT */
        x64KSKernelPDPT[GET_PDPT_सूचकांक(PPTR_आधारः) + pd_सूचकांक_2] = pdpte_pdpte_pd_new(
                                                                    0, /* xd */
                                                                    kpptr_गन्तव्यम्_paddr(&x64KSKernelPDs[pd_सूचकांक_2][0]),
                                                                    0, /* accessed */
                                                                    0, /* cache disabled */
                                                                    0, /* write through */
                                                                    0, /* super user */
                                                                    1, /* read write */
                                                                    1 /* present */
                                                                );
    }

    x64KSKernelPDPT[GET_PDPT_सूचकांक(KERNEL_ELF_आधारः)] = pdpte_pdpte_pd_new(
                                                           0, /* xd */
                                                           kpptr_गन्तव्यम्_paddr(&x64KSKernelPDs[0][0]),
                                                           0, /* accessed */
                                                           0, /* cache disable */
                                                           1, /* write through */
                                                           0, /* super user */
                                                           1, /* read write */
                                                           1  /* present */
                                                       );

    paddr = 0;
    vaddr = PPTR_आधारः;

    प्रति (paddr = 0; paddr < PADDR_शिखरम्;
         paddr += 0x200000) {

        पूर्णाङ्क pd_सूचकांक_2 = GET_PDPT_सूचकांक(vaddr) - GET_PDPT_सूचकांक(PPTR_आधारः);
        पूर्णाङ्क pde_सूचकांक = GET_PD_सूचकांक(vaddr);

        x64KSKernelPDs[pd_सूचकांक_2][pde_सूचकांक] = pde_pde_large_new(
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
    x64KSKernelPDPT[GET_PDPT_सूचकांक(KDEV_आधारः)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_गन्तव्यम्_paddr(&x64KSKernelPDs[BIT(PDPT_सूचकांक_द्वयङ्काः) - 1][0]),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    /* put the PT into the PD */
    x64KSKernelPDs[BIT(PDPT_सूचकांक_द्वयङ्काः) - 1][0] = pde_pde_pt_new(
                                                      0, /* xd */
                                                      kpptr_गन्तव्यम्_paddr(x64KSKernelPT),
                                                      0, /* accessed */
                                                      0, /* cache_disabled */
                                                      0, /* write_through */
                                                      0, /* super_user */
                                                      1, /* read_write */
                                                      1  /* present */
                                                  );
#यदि_अन्तः

#यदि CONFIG_MAX_अङ्कः_TRACE_POINTS > 0
    /* use the last PD entry as the benchmark log storage.
     * the actual backing physical memory will be filled
     * later by using alloc_region */
    ksLog = (ks_log_प्रविष्टिः_t *)(KDEV_आधारः + 0x200000 * (BIT(PD_सूचकांक_द्वयङ्काः) - 1));
#यदि_अन्तः

    /* now map in the kernel devices */
    यदि (!map_kernel_गवाक्षः_devices(x64KSKernelPT, अङ्कः_ioapic, ioapic_paddrs, अङ्कः_drhu, drhu_list)) {
        प्रत्यागम false;
    }

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(x64KSKernelPML4);
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

    /* In boot code, so fine to just trash everything here */
    invalidateLocalTranslationAll();
    प्रारूप_लिख("Mapping kernel window is done\n");
    प्रत्यागम true;
}

#यदि_परिभाषितम् CONFIG_KERNEL_SKIM_गवाक्षः
BOOT_संहिता bool_t map_skim_गवाक्षः(vptr_t skim_start, vptr_t skim_अन्तः)
{
    /* place the PDPT into the PML4 */
    x64KSSKIMPML4[GET_PML4_सूचकांक(PPTR_आधारः)] = pml4e_new(
                                                   0, /* xd */
                                                   kpptr_गन्तव्यम्_paddr(x64KSSKIMPDPT),
                                                   0, /* accessed */
                                                   0, /* cache_disabled */
                                                   0, /* write_through */
                                                   0, /* super_user */
                                                   1, /* read_write */
                                                   1  /* present */
                                               );
    /* place the PD into the kernel_base slot of the PDPT */
    x64KSSKIMPDPT[GET_PDPT_सूचकांक(KERNEL_ELF_आधारः)] = pdpte_pdpte_pd_new(
                                                         0, /* xd */
                                                         kpptr_गन्तव्यम्_paddr(x64KSSKIMPD),
                                                         0, /* accessed */
                                                         0, /* cache_disabled */
                                                         0, /* write_through */
                                                         0, /* super_user */
                                                         1, /* read_write */
                                                         1  /* present */
                                                     );
    /* map the skim portion into the PD. we expect it to be 2M aligned */
    प्रतिज्ञा((skim_start % BIT(se_L4_Large_Page_द्वयङ्काः)) == 0);
    प्रतिज्ञा((skim_अन्तः % BIT(se_L4_Large_Page_द्वयङ्काः)) == 0);
    uint64_t paddr = kpptr_गन्तव्यम्_paddr((शून्य *)skim_start);
    प्रति (पूर्णाङ्क i = GET_PD_सूचकांक(skim_start); i < GET_PD_सूचकांक(skim_अन्तः); i++) {
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
        paddr += BIT(se_L4_Large_Page_द्वयङ्काः);
    }
    प्रत्यागम true;
}
#यदि_अन्तः

BOOT_संहिता शून्य init_tss(tss_t *tss)
{
    word_t आधारः = (word_t)&x64KSIRQStack[CURRENT_CPU_सूचकांक()][IRQ_राशिः_आकार];
    *tss = tss_new(
               परिमाण(*tss),   /* io map base */
               0, 0,       /* ist 7 */
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               /* ist 1 is the stack frame we use for interrupts */
               आधारः >> 32, आधारः & 0xffffffff,  /* ist 1 */
               0, 0,       /* rsp 2 */
               0, 0,       /* rsp 1 */
               0, 0        /* rsp 0 */
           );
    /* set the IO map to all 1 to block user IN/OUT instructions */
    स्मृतिं_पूरय(&x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KStss.io_map[0], 0xff,
           परिमाण(x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KStss.io_map));
}

शून्य init_syscall_msrs(शून्य)
{
    x86_wrmsr(IA32_LSTAR_MSR, (uint64_t)&handle_fastsyscall);
    // mask bit 9 in the kernel (which is the interrupt enable bit)
    // also mask bit 8, which is the Trap Flag, to prevent the kernel
    // from single stepping
    x86_wrmsr(IA32_FMASK_MSR, FLAGS_TF | FLAGS_IF);
    x86_wrmsr(IA32_तारकम्_MSR, ((uint64_t)SEL_CS_0 << 32) | ((uint64_t)SEL_CS_3 << 48));
}

BOOT_संहिता शून्य init_gdt(gdt_प्रविष्टिः_t *gdt, tss_t *tss)
{

    uint64_t tss_आधारः = (uint64_t)tss;
    gdt_tss_t gdt_tss;

    gdt[GDT_NULL] = gdt_प्रविष्टिः_gdt_null_new();

    gdt[GDT_CS_0] = gdt_प्रविष्टिः_gdt_संहिता_new(
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

    gdt[GDT_DS_0] = gdt_प्रविष्टिः_gdt_दत्त_new(
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

    gdt[GDT_CS_3] = gdt_प्रविष्टिः_gdt_संहिता_new(
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

    gdt[GDT_DS_3] = gdt_प्रविष्टिः_gdt_दत्त_new(
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

    gdt[GDT_FS] = gdt_प्रविष्टिः_gdt_दत्त_new(
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

    gdt[GDT_GS] = gdt_प्रविष्टिः_gdt_दत्त_new(
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
                  tss_आधारः >> 32,                     /* base 63 - 32 */
                  (tss_आधारः & 0xff000000UL) >> 24,    /* base 31 - 24 */
                  1,                                  /* granularity */
                  0,                                  /* avl */
                  0,                                  /* limit high */
                  1,                                  /* present */
                  0,                                  /* dpl */
                  9,                                  /* desc type */
                  (tss_आधारः & 0xff0000UL) >> 16,      /* base 23-16 */
                  (tss_आधारः & 0xffffUL),              /* base 15 - 0 */
                  परिमाण(tss_io_t) - 1
              );

    gdt[GDT_TSS].शब्दाः[0] = gdt_tss.शब्दाः[0];
    gdt[GDT_TSS + 1].शब्दाः[0] = gdt_tss.शब्दाः[1];
}

BOOT_संहिता शून्य init_idt_प्रविष्टिः(idt_प्रविष्टिः_t *idt, व्यवधानम्_t व्यवधानम्, शून्य(*handler)(शून्य))
{
    uint64_t handler_addr = (uint64_t)handler;
    uint64_t dpl = 3;

    यदि (व्यवधानम् < int_trap_min && व्यवधानम् != int_software_break_request) {
        dpl = 0;
    }

    idt[व्यवधानम्] = idt_प्रविष्टिः_व्यवधानम्_gate_new(
                         handler_addr >> 32,                 /* offset 63 - 32 */
                         ((handler_addr >> 16) & 0xffff),
                         1,                                  /* present */
                         dpl,                                /* dpl */
                         1,                                  /* ist */
                         SEL_CS_0,                           /* segment selector */
                         (handler_addr & 0xffff)               /* offset 15 - 0 */
                     );
}

शून्य समुच्चयः_VMRoot(tcb_t *tcb)
{
    cap_t thread_मूलम्;
    asid_t asid;
    pml4e_t *pml4;
    findVSpaceForASID_ret_t find_ret;
    cr3_t cr3;

    thread_मूलम् = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    यदि (cap_get_capType(thread_मूलम्) != cap_pml4_cap ||
        !cap_pml4_cap_get_capPML4IsMapped(thread_मूलम्)) {
        समुच्चयः_Current_User_VSpace_मूलम्(kpptr_गन्तव्यम्_paddr(X86_GLOBAL_VSPACE_मूलम्), 0);
        प्रत्यागम;
    }

    pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_आधारः_Ptr(thread_मूलम्));
    asid = cap_pml4_cap_get_capPML4MappedASID(thread_मूलम्);
    find_ret = findVSpaceForASID(asid);
    यदि (unlikely(find_ret.स्थितिः != EXCEPTION_NONE || find_ret.vspace_मूलम् != pml4)) {
        समुच्चयः_Current_User_VSpace_मूलम्(kpptr_गन्तव्यम्_paddr(X86_GLOBAL_VSPACE_मूलम्), 0);
        प्रत्यागम;
    }
    cr3 = makeCR3(pptr_गन्तव्यम्_paddr(pml4), asid);
    यदि (getCurrentUserCR3().शब्दाः[0] != cr3.शब्दाः[0]) {
        SMP_COND_STATEMENT(tlb_bitmap_समुच्चयः(pml4, getCurrentCPUIndex());)
        समुच्चयः_Current_User_CR3(cr3);
    }
}


BOOT_संहिता शून्य init_dtrs(शून्य)
{
    gdt_idt_ptr.सीमा = (परिमाण(gdt_प्रविष्टिः_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.आधारः = (uint64_t)x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KSgdt;

    /* When we install the gdt it will clobber any value of gs that
     * we have. Since we might be using it for TLS we can stash
     * and unstash any gs value using swapgs
     */
    swapgs();
    x64_install_gdt(&gdt_idt_ptr);
    swapgs();

    gdt_idt_ptr.सीमा = (परिमाण(idt_प्रविष्टिः_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.आधारः = (uint64_t)x86KSGlobalState[CURRENT_CPU_सूचकांक()].x86KSidt;
    x64_install_idt(&gdt_idt_ptr);

    x64_install_ldt(SEL_NULL);

    x64_install_tss(SEL_TSS);
}

BOOT_संहिता शून्य map_it_frame_cap(cap_t pd_cap, cap_t frame_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_सम्बन्धि_cap(pd_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt;
    vptr_t vptr = cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(frame_cap);
    शून्य *pptr = (शून्य *)cap_frame_cap_get_capFBasePtr(frame_cap);

    प्रतिज्ञा(cap_frame_cap_get_capFMapType(frame_cap) == X86_MappingVSpace);
    प्रतिज्ञा(cap_frame_cap_get_capFMappedASID(frame_cap) != asidInvalid);
    pml4 += GET_PML4_सूचकांक(vptr);
    प्रतिज्ञा(pml4e_ptr_get_present(pml4));
    pdpt = paddr_गन्तव्यम्_pptr(pml4e_ptr_get_pdpt_आधारः_स्थानाङ्कः(pml4));
    pdpt += GET_PDPT_सूचकांक(vptr);
    प्रतिज्ञा(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_गन्तव्यम्_pptr(pdpte_pdpte_pd_ptr_get_pd_आधारः_स्थानाङ्कः(pdpt));
    pd += GET_PD_सूचकांक(vptr);
    प्रतिज्ञा(pde_pde_pt_ptr_get_present(pd));
    pt = paddr_गन्तव्यम्_pptr(pde_pde_pt_ptr_get_pt_आधारः_स्थानाङ्कः(pd));
    *(pt + GET_PT_सूचकांक(vptr)) = pte_new(
                                     0,                      /* xd                   */
                                     pptr_गन्तव्यम्_paddr(pptr),    /* page_base_address    */
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

स्थिर BOOT_संहिता शून्य map_it_pdpt_cap(cap_t vspace_cap, cap_t pdpt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_सम्बन्धि_cap(vspace_cap));
    pdpte_t *pdpt = PDPT_PTR(cap_pdpt_cap_get_capPDPTBasePtr(pdpt_cap));
    vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_स्थानाङ्कः(pdpt_cap);

    प्रतिज्ञा(cap_pdpt_cap_get_capPDPTIsMapped(pdpt_cap));
    *(pml4 + GET_PML4_सूचकांक(vptr)) = pml4e_new(
                                         0,                      /* xd                   */
                                         pptr_गन्तव्यम्_paddr(pdpt),    /* pdpt_base_address    */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_संहिता शून्य map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_सम्बन्धि_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd = PD_PTR(cap_page_directory_cap_get_capPDBasePtr(pd_cap));
    vptr_t vptr = cap_page_directory_cap_get_cap_PDMapped_स्थानाङ्कः(pd_cap);

    प्रतिज्ञा(cap_page_directory_cap_get_capPDIsMapped(pd_cap));
    pml4 += GET_PML4_सूचकांक(vptr);
    प्रतिज्ञा(pml4e_ptr_get_present(pml4));
    pdpt = paddr_गन्तव्यम्_pptr(pml4e_ptr_get_pdpt_आधारः_स्थानाङ्कः(pml4));
    *(pdpt + GET_PDPT_सूचकांक(vptr)) = pdpte_pdpte_pd_new(
                                         0,                      /* xd                   */
                                         pptr_गन्तव्यम्_paddr(pd),      /* pd_base_address      */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_संहिता शून्य map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_सम्बन्धि_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt = PT_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(pt_cap);

    प्रतिज्ञा(cap_page_सारणी_cap_get_cap_PTIs_Mapped(pt_cap));
    pml4 += GET_PML4_सूचकांक(vptr);
    प्रतिज्ञा(pml4e_ptr_get_present(pml4));
    pdpt = paddr_गन्तव्यम्_pptr(pml4e_ptr_get_pdpt_आधारः_स्थानाङ्कः(pml4));
    pdpt += GET_PDPT_सूचकांक(vptr);
    प्रतिज्ञा(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_गन्तव्यम्_pptr(pdpte_pdpte_pd_ptr_get_pd_आधारः_स्थानाङ्कः(pdpt));
    *(pd + GET_PD_सूचकांक(vptr)) = pde_pde_pt_new(
                                     0,                      /* xd                   */
                                     pptr_गन्तव्यम्_paddr(pt),      /* pt_base_address      */
                                     0,                      /* accessed             */
                                     0,                      /* cache_disabled       */
                                     0,                      /* write_through        */
                                     1,                      /* super_user           */
                                     1,                      /* read_write           */
                                     1                       /* present              */
                                 );
}

BOOT_संहिता शून्य *map_अस्थायी_boot_page(शून्य *प्रविष्टिः, uint32_t large_पृष्ठानि)
{
    /* this function is for legacy 32-bit systems where the ACPI tables might
     * collide with the kernel window. Here we just assert that the table is
     * in fact in the lower 4GiB region (which is already 1-to-1 mapped) and
     * continue */
    प्रतिज्ञा((word_t)प्रविष्टिः < BIT(32));
    प्रत्यागम प्रविष्टिः;
}

स्थिर BOOT_संहिता cap_t create_it_pdpt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_pdpt_cap_new(
              asid,   /* capPDPTMappedASID    */
              pptr,   /* capPDPTBasePtr       */
              1,      /* capPDPTIsMapped      */
              vptr    /* capPDPTMappedAddress */
          );
    map_it_pdpt_cap(vspace_cap, cap);
    प्रत्यागम cap;
}

स्थिर BOOT_संहिता cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_page_directory_cap_new(
              asid,   /* capPDMappedASID      */
              pptr,   /* capPDBasePtr         */
              1,      /* capPDIsMapped        */
              vptr    /* capPDMappedAddress   */
          );
    map_it_pd_cap(vspace_cap, cap);
    प्रत्यागम cap;
}

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
    word_t n = get_n_paging(it_v_reg, PD_सूचकांक_विस्थापनम्);
    n += get_n_paging(it_v_reg, PDPT_सूचकांक_विस्थापनम्);
    n += get_n_paging(it_v_reg, PML4_सूचकांक_विस्थापनम्);
#यदि_परिभाषितम् CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#यदि_अन्तः
    प्रत्यागम n;
}

BOOT_संहिता cap_t create_it_स्थानाङ्कः_space(cap_t मूलम्_cnode_cap, v_region_t it_v_reg)
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


    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

    /* Create any PDPTs needed for the user land image */
    प्रति (vptr = आवर्तनम्_अधस्तात्(it_v_reg.start, PML4_सूचकांक_विस्थापनम्);
         vptr < it_v_reg.अन्तः_2;
         vptr += BIT(PML4_सूचकांक_विस्थापनम्)) {
        यदि (!provide_cap(मूलम्_cnode_cap,
                         create_it_pdpt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            प्रत्यागम cap_null_cap_new();
        }
    }

    /* Create any PDs needed for the user land image */
    प्रति (vptr = आवर्तनम्_अधस्तात्(it_v_reg.start, PDPT_सूचकांक_विस्थापनम्);
         vptr < it_v_reg.अन्तः_2;
         vptr += BIT(PDPT_सूचकांक_विस्थापनम्)) {
        यदि (!provide_cap(मूलम्_cnode_cap,
                         create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            प्रत्यागम cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    प्रति (vptr = आवर्तनम्_अधस्तात्(it_v_reg.start, PD_सूचकांक_विस्थापनम्);
         vptr < it_v_reg.अन्तः_2;
         vptr += BIT(PD_सूचकांक_विस्थापनम्)) {
        यदि (!provide_cap(मूलम्_cnode_cap,
                         create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
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

शून्य copyGlobalMappings(vspace_मूलम्_t *new_vspace)
{
    अचिह्नित दीर्घ i;
    pml4e_t *vspace = (pml4e_t *)new_vspace;

    /* Copy from the tlbbitmap_pptr so that we copy the default entries of the
     * tlb bitmap (if it exists). If it doesn't exist then this loop
     * will be equivalent to copying from PPTR_BASE
     */
    प्रति (i = GET_PML4_सूचकांक(TLBBITMAP_PPTR); i < BIT(PML4_सूचकांक_द्वयङ्काः); i++) {
        vspace[i] = X86_GLOBAL_VSPACE_मूलम्[i];
    }
}

स्थिर BOOT_संहिता cap_t create_it_frame_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large, seL4_Word map_type)
{
    vm_page_आकार_t frame_आकार_2;

    यदि (use_large) {
        frame_आकार_2 = X86_LargePage;
    } अन्यथा {
        frame_आकार_2 = X86_SmallPage;
    }

    प्रत्यागम
        cap_frame_cap_new(
            asid,                          /* capFMappedASID     */
            pptr,                          /* capFBasePtr        */
            frame_आकार_2,                    /* capFSize           */
            map_type,                      /* capFMapType        */
            vptr,                          /* capFMappedAddress  */
            wordFromVMRights(VMReadWrite), /* capFVMRights       */
            0                              /* capFIsDevice       */
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

/* ====================== BOOT CODE FINISHES HERE ======================== */



exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspaceCapSlot)
{
    asid_map_t asid_map;
#यदि_परिभाषितम् CONFIG_VTX
    यदि (cap_get_capType(vspaceCapSlot->cap) == cap_ept_pml4_cap) {
        cap_ept_pml4_cap_ptr_समुच्चयः_cap_PML4_Mapped_ASID(&vspaceCapSlot->cap, asid);
        cap_ept_pml4_cap_ptr_समुच्चयः_cap_PML4_Is_Mapped(&vspaceCapSlot->cap, 1);
        asid_map = asid_map_asid_map_ept_new(cap_ept_pml4_cap_get_cap_PML4_आधारः_Ptr(vspaceCapSlot->cap));
    } अन्यथा
#यदि_अन्तः
    {
        प्रतिज्ञा(cap_get_capType(vspaceCapSlot->cap) == cap_pml4_cap);
        cap_pml4_cap_ptr_समुच्चयः_cap_PML4_Mapped_ASID(&vspaceCapSlot->cap, asid);
        cap_pml4_cap_ptr_समुच्चयः_cap_PML4_Is_Mapped(&vspaceCapSlot->cap, 1);
        asid_map = asid_map_asid_map_vspace_new(cap_pml4_cap_get_cap_PML4_आधारः_Ptr(vspaceCapSlot->cap));
    }
    poolPtr->array[ASID_नीचः(asid)] = asid_map;
    प्रत्यागम EXCEPTION_NONE;
}

bool_t CONST is_VTable_मूलम्(cap_t cap)
{
    प्रत्यागम cap_get_capType(cap) == cap_pml4_cap;
}

bool_t CONST is_Valid_Native_मूलम्(cap_t cap)
{
    प्रत्यागम is_VTable_मूलम्(cap) &&
           cap_pml4_cap_get_capPML4IsMapped(cap);
}

स्थिर pml4e_t CONST makeUserPML4E(paddr_t paddr, vm_गुणाः_t vm_attr)
{
    प्रत्यागम pml4e_new(
               0,
               paddr,
               0,
               vm_गुणाः_get_x86_PCDBit(vm_attr),
               vm_गुणाः_get_x86_PWTBit(vm_attr),
               1,
               1,
               1
           );
}

स्थिर pml4e_t CONST makeUserPML4EInvalid(शून्य)
{
    प्रत्यागम pml4e_new(
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

स्थिर pdpte_t CONST makeUserPDPTEHugePage(paddr_t paddr, vm_गुणाः_t vm_attr, vm_rights_t vm_rights)
{
    प्रत्यागम pdpte_pdpte_1g_new(
               0,          /* xd               */
               paddr,      /* physical address */
               0,          /* PAT              */
               0,          /* global           */
               0,          /* dirty            */
               0,          /* accessed         */
               vm_गुणाः_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_गुणाः_get_x86_PWTBit(vm_attr),  /* write through  */
               SuperUserFromVMRights(vm_rights),       /* super user     */
               WritableFromVMRights(vm_rights),        /* read write     */
               1                                       /* present        */
           );
}

स्थिर pdpte_t CONST makeUserPDPTEPageDirectory(paddr_t paddr, vm_गुणाः_t vm_attr)
{
    प्रत्यागम pdpte_pdpte_pd_new(
               0,                      /* xd       */
               paddr,                  /* paddr    */
               0,                      /* accessed */
               vm_गुणाः_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_गुणाः_get_x86_PWTBit(vm_attr),  /* write through  */
               1,                      /* super user */
               1,                      /* read write */
               1                       /* present    */
           );
}

स्थिर pdpte_t CONST makeUserPDPTEInvalid(शून्य)
{
    प्रत्यागम pdpte_pdpte_pd_new(
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

pde_t CONST makeUserPDELargePage(paddr_t paddr, vm_गुणाः_t vm_attr, vm_rights_t vm_rights)
{
    प्रत्यागम pde_pde_large_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               vm_गुणाः_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_गुणाः_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_गुणाः_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_सारणी(paddr_t paddr, vm_गुणाः_t vm_attr)
{

    प्रत्यागम  pde_pde_pt_new(
                0,                                  /* xd               */
                paddr,                              /* pt_base_address  */
                0,                                  /* accessed         */
                vm_गुणाः_get_x86_PCDBit(vm_attr), /* cache_disabled   */
                vm_गुणाः_get_x86_PWTBit(vm_attr), /* write_through    */
                1,                                  /* super_user       */
                1,                                  /* read_write       */
                1                                   /* present          */
            );
}

pde_t CONST makeUserPDEInvalid(शून्य)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    प्रत्यागम pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_गुणाः_t vm_attr, vm_rights_t vm_rights)
{
    प्रत्यागम pte_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               0,                                              /* global               */
               vm_गुणाः_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_गुणाः_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_गुणाः_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pte_t CONST makeUserPTEInvalid(शून्य)
{
    प्रत्यागम pte_new(
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


स्थिर pml4e_t *lookupPML4Slot(vspace_मूलम्_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4e = PML4E_PTR(pml4);
    word_t pml4_सूचकांक = GET_PML4_सूचकांक(vptr);
    प्रत्यागम pml4e + pml4_सूचकांक;
}

स्थिर lookupPDPTSlot_ret_t lookupPDPTSlot(vspace_मूलम्_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4Slot = lookupPML4Slot(pml4, vptr);
    lookupPDPTSlot_ret_t ret;

    यदि (!pml4e_ptr_get_present(pml4Slot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PML4_सूचकांक_विस्थापनम्);

        ret.pdptSlot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    } अन्यथा {
        pdpte_t *pdpt;
        pdpte_t *pdptSlot;
        word_t pdpt_सूचकांक = GET_PDPT_सूचकांक(vptr);
        pdpt = paddr_गन्तव्यम्_pptr(pml4e_ptr_get_pdpt_आधारः_स्थानाङ्कः(pml4Slot));
        pdptSlot = pdpt + pdpt_सूचकांक;

        ret.स्थितिः = EXCEPTION_NONE;
        ret.pdptSlot = pdptSlot;
        प्रत्यागम ret;
    }
}

lookupPDSlot_ret_t lookupPDSlot(vspace_मूलम्_t *pml4, vptr_t vptr)
{
    lookupPDPTSlot_ret_t pdptSlot;
    lookupPDSlot_ret_t ret;

    pdptSlot = lookupPDPTSlot(pml4, vptr);

    यदि (pdptSlot.स्थितिः != EXCEPTION_NONE) {
        ret.pdSlot = रिक्त;
        ret.स्थितिः = pdptSlot.स्थितिः;
        प्रत्यागम ret;
    }
    यदि ((pdpte_ptr_get_page_आकार(pdptSlot.pdptSlot) != pdpte_pdpte_pd) ||
        !pdpte_pdpte_pd_ptr_get_present(pdptSlot.pdptSlot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PDPT_सूचकांक_विस्थापनम्);

        ret.pdSlot = रिक्त;
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    } अन्यथा {
        pde_t *pd;
        pde_t *pdSlot;
        word_t pd_सूचकांक = GET_PD_सूचकांक(vptr);
        pd = paddr_गन्तव्यम्_pptr(pdpte_pdpte_pd_ptr_get_pd_आधारः_स्थानाङ्कः(pdptSlot.pdptSlot));
        pdSlot = pd + pd_सूचकांक;

        ret.स्थितिः = EXCEPTION_NONE;
        ret.pdSlot = pdSlot;
        प्रत्यागम ret;
    }
}

स्थिर शून्य flushPD(vspace_मूलम्_t *vspace, word_t vptr, pde_t *pd, asid_t asid)
{
    /* clearing the entire PCID vs flushing the virtual addresses
     * one by one using invplg.
     * choose the easy way, invalidate the PCID
     */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));

}

स्थिर शून्य flushPDPT(vspace_मूलम्_t *vspace, word_t vptr, pdpte_t *pdpt, asid_t asid)
{
    /* similar here */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    प्रत्यागम;
}

शून्य hwASIDInvalidate(asid_t asid, vspace_मूलम्_t *vspace)
{
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
}

शून्य unmapPageDirectory(asid_t asid, vptr_t vaddr, pde_t *pd)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPDPTSlot_ret_t    lu_ret;

    find_ret = findVSpaceForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम;
    }

    lu_ret = lookupPDPTSlot(find_ret.vspace_मूलम्, vaddr);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम;
    }

    /* check if the PDPT has the PD */
    यदि (!(pdpte_ptr_get_page_आकार(lu_ret.pdptSlot) == pdpte_pdpte_pd &&
          pdpte_pdpte_pd_ptr_get_present(lu_ret.pdptSlot) &&
          (pdpte_pdpte_pd_ptr_get_pd_आधारः_स्थानाङ्कः(lu_ret.pdptSlot) == pptr_गन्तव्यम्_paddr(pd)))) {
        प्रत्यागम;
    }

    flushPD(find_ret.vspace_मूलम्, vaddr, pd, asid);

    *lu_ret.pdptSlot = makeUserPDPTEInvalid();

    invalidatePageStructureCacheASID(pptr_गन्तव्यम्_paddr(find_ret.vspace_मूलम्), asid,
                                     SMP_TERNARY(tlb_bitmap_get(find_ret.vspace_मूलम्), 0));
}


स्थिर exception_t performX64PageDirectoryInvocationUnmap(cap_t cap, cte_t *ctSlot)
{

    यदि (cap_page_directory_cap_get_capPDIsMapped(cap)) {
        pde_t *pd = PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap));
        unmapPageDirectory(
            cap_page_directory_cap_get_capPDMappedASID(cap),
            cap_page_directory_cap_get_cap_PDMapped_स्थानाङ्कः(cap),
            pd
        );
        clear_स्मृतिः((शून्य *)pd, cap_get_cap_आकार_द्वयङ्काः(cap));
    }

    cap_page_directory_cap_ptr_समुच्चयः_cap_PDIs_Mapped(&(ctSlot->cap), 0);

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performX64PageDirectoryInvocationMap(cap_t cap, cte_t *ctSlot, pdpte_t pdpte, pdpte_t *pdptSlot,
                                                        vspace_मूलम्_t *vspace)
{
    ctSlot->cap = cap;
    *pdptSlot = pdpte;
    invalidatePageStructureCacheASID(pptr_गन्तव्यम्_paddr(vspace), cap_page_directory_cap_get_capPDMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    प्रत्यागम EXCEPTION_NONE;
}


स्थिर exception_t decodeX64PageDirectoryInvocation(
    word_t label,
    word_t दीर्घता,
    cte_t *cte,
    cap_t cap,
    word_t *बफर
)
{
    word_t              vaddr;
    vm_गुणाः_t     vm_attr;
    cap_t               vspaceCap;
    vspace_मूलम्_t      *vspace;
    pdpte_t             pdpte;
    paddr_t             paddr;
    asid_t              asid;
    lookupPDPTSlot_ret_t pdptSlot;

    यदि (label == X86PageDirectoryUnmap) {
        यदि (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("X86PageDirectory: Cannot unmap if more than one cap exist.");
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);

        प्रत्यागम performX64PageDirectoryInvocationUnmap(cap, cte);
    }

    यदि (label != X86PageDirectoryMap) {
        userError("X64Directory: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("X64PageDirectory: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_page_directory_cap_get_capPDIsMapped(cap)) {
        userError("X64PageDirectory: PD is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, बफर) & (~आवरणम्(PDPT_सूचकांक_विस्थापनम्));
    vm_attr = vm_गुणाः_From_Word(getSyscallArg(1, बफर));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    यदि (!is_Valid_Native_मूलम्(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_मूलम्_t *)pptr_सम्बन्धि_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    यदि (vaddr > USER_शिखरम्) {
        userError("X64PageDirectory: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    findVSpaceForASID_ret_t find_ret;

    find_ret = findVSpaceForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (find_ret.vspace_मूलम् != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pdptSlot = lookupPDPTSlot(vspace, vaddr);
    यदि (pdptSlot.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि ((pdpte_ptr_get_page_आकार(pdptSlot.pdptSlot) == pdpte_pdpte_pd &&
         pdpte_pdpte_pd_ptr_get_present(pdptSlot.pdptSlot)) ||
        (pdpte_ptr_get_page_आकार(pdptSlot.pdptSlot) == pdpte_pdpte_1g
         && pdpte_pdpte_1g_ptr_get_present(pdptSlot.pdptSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_गन्तव्यम्_paddr(PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
    pdpte = makeUserPDPTEPageDirectory(paddr, vm_attr);

    cap = cap_page_directory_cap_समुच्चयः_cap_PDIs_Mapped(cap, 1);
    cap = cap_page_directory_cap_समुच्चयः_cap_PDMapped_ASID(cap, asid);
    cap = cap_page_directory_cap_समुच्चयः_cap_PDMapped_स्थानाङ्कः(cap, vaddr);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम performX64PageDirectoryInvocationMap(cap, cte, pdpte, pdptSlot.pdptSlot, vspace);
}

स्थिर शून्य unmapPDPT(asid_t asid, vptr_t vaddr, pdpte_t *pdpt)
{
    findVSpaceForASID_ret_t find_ret;
    pml4e_t *pml4Slot;

    find_ret = findVSpaceForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम;
    }

    pml4Slot = lookupPML4Slot(find_ret.vspace_मूलम्, vaddr);

    /* check if the PML4 has the PDPT */
    यदि (!(pml4e_ptr_get_present(pml4Slot) &&
          pml4e_ptr_get_pdpt_आधारः_स्थानाङ्कः(pml4Slot) == pptr_गन्तव्यम्_paddr(pdpt))) {
        प्रत्यागम;
    }

    flushPDPT(find_ret.vspace_मूलम्, vaddr, pdpt, asid);

    *pml4Slot = makeUserPML4EInvalid();
}

स्थिर exception_t performX64PDPTInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    यदि (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        pdpte_t *pdpt = PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap));
        unmapPDPT(cap_pdpt_cap_get_capPDPTMappedASID(cap),
                  cap_pdpt_cap_get_cap_PDPTMapped_स्थानाङ्कः(cap),
                  pdpt);
        clear_स्मृतिः((शून्य *)pdpt, cap_get_cap_आकार_द्वयङ्काः(cap));
    }

    cap_pdpt_cap_ptr_समुच्चयः_cap_PDPTIs_Mapped(&(ctSlot->cap), 0);

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t performX64PDPTInvocationMap(cap_t cap, cte_t *ctSlot, pml4e_t pml4e, pml4e_t *pml4Slot,
                                               vspace_मूलम्_t *vspace)
{
    ctSlot->cap = cap;
    *pml4Slot = pml4e;
    invalidatePageStructureCacheASID(pptr_गन्तव्यम्_paddr(vspace), cap_pdpt_cap_get_capPDPTMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));

    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t decodeX64PDPTInvocation(
    word_t  label,
    word_t दीर्घता,
    cte_t   *cte,
    cap_t   cap,
    word_t  *बफर)
{
    word_t                  vaddr;
    vm_गुणाः_t         attr;
    pml4e_t                *pml4Slot;
    cap_t                   vspaceCap;
    vspace_मूलम्_t          *vspace;
    pml4e_t                 pml4e;
    paddr_t                 paddr;
    asid_t                  asid;

    यदि (label == X86PDPTUnmap) {
        यदि (!isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("X86PDPT: Cannot unmap if more than one cap exist.");
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);

        प्रत्यागम performX64PDPTInvocationUnmap(cap, cte);
    }

    यदि (label != X86PDPTMap) {
        userError("X86PDPT: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (दीर्घता < 2 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("X64PDPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X64PDPT: PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, बफर) & (~आवरणम्(PML4_सूचकांक_विस्थापनम्));
    attr = vm_गुणाः_From_Word(getSyscallArg(1, बफर));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    यदि (!is_Valid_Native_मूलम्(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_मूलम्_t *)pptr_सम्बन्धि_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    यदि (vaddr > USER_शिखरम्) {
        userError("X64PDPT: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    findVSpaceForASID_ret_t find_ret;

    find_ret = findVSpaceForASID(asid);
    यदि (find_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (find_ret.vspace_मूलम् != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    pml4Slot = lookupPML4Slot(vspace, vaddr);

    यदि (pml4e_ptr_get_present(pml4Slot)) {
        current_syscall_error.type = seL4_DeleteFirst;

        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_गन्तव्यम्_paddr(PDPTE_PTR((cap_pdpt_cap_get_capPDPTBasePtr(cap))));
    pml4e = makeUserPML4E(paddr, attr);

    cap = cap_pdpt_cap_समुच्चयः_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_pdpt_cap_समुच्चयः_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_pdpt_cap_समुच्चयः_cap_PDPTMapped_स्थानाङ्कः(cap, vaddr);

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम performX64PDPTInvocationMap(cap, cte, pml4e, pml4Slot, vspace);
}

exception_t decode_X86_बहुलकम्_MMUInvocation(
    word_t label,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    विकल्प (cap_get_capType(cap)) {

    पक्ष cap_pml4_cap:
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;

    पक्ष cap_pdpt_cap:
        प्रत्यागम decodeX64PDPTInvocation(label, दीर्घता, cte, cap, बफर);

    पक्ष cap_page_directory_cap:
        प्रत्यागम decodeX64PageDirectoryInvocation(label, दीर्घता, cte, cap, बफर);

    स्वभाव:
        fail("Invalid arch cap type");
    }
}

bool_t बहुलकम्_Unmap_Page(vm_page_आकार_t page_आकार, vspace_मूलम्_t *vroot, vptr_t vaddr, शून्य *pptr)
{
    यदि (config_समुच्चयः(CONFIG_HUGE_PAGE) && page_आकार == X64_HugePage) {
        pdpte_t     *pdpte;
        lookupPDPTSlot_ret_t pdpt_ret = lookupPDPTSlot(vroot, vaddr);
        यदि (pdpt_ret.स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम false;
        }
        pdpte = pdpt_ret.pdptSlot;


        यदि (!(pdpte_ptr_get_page_आकार(pdpte) == pdpte_pdpte_1g
              && pdpte_pdpte_1g_ptr_get_present(pdpte)
              && (pdpte_pdpte_1g_ptr_get_page_आधारः_स्थानाङ्कः(pdpte)
                  == pptr_गन्तव्यम्_paddr(pptr)))) {
            प्रत्यागम false;
        }

        *pdpte = makeUserPDPTEInvalid();
        प्रत्यागम true;
    }
    fail("Invalid page type");
    प्रत्यागम false;
}

स्थिर exception_t updatePDPTE(asid_t asid, pdpte_t pdpte, pdpte_t *pdptSlot, vspace_मूलम्_t *vspace)
{
    *pdptSlot = pdpte;
    invalidatePageStructureCacheASID(pptr_गन्तव्यम्_paddr(vspace), asid,
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t perform_X64_बहुलकम्_Map(cap_t cap, cte_t *ctSlot, pdpte_t pdpte, pdpte_t *pdptSlot, vspace_मूलम्_t *vspace)
{
    ctSlot->cap = cap;
    प्रत्यागम updatePDPTE(cap_frame_cap_get_capFMappedASID(cap), pdpte, pdptSlot, vspace);
}

संरचना create_mapping_pdpte_return {
    exception_t स्थितिः;
    pdpte_t pdpte;
    pdpte_t *pdptSlot;
};
प्रकारनिर्देश संरचना create_mapping_pdpte_return create_mapping_pdpte_return_t;

स्थिर create_mapping_pdpte_return_t createSafeMappingEntries_PDPTE(paddr_t आधारः, word_t vaddr, vm_rights_t vmRights,
                                                                    vm_गुणाः_t attr,
                                                                    vspace_मूलम्_t *vspace)
{
    create_mapping_pdpte_return_t ret;
    lookupPDPTSlot_ret_t          lu_ret;

    lu_ret = lookupPDPTSlot(vspace, vaddr);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        प्रत्यागम ret;
    }
    ret.pdptSlot = lu_ret.pdptSlot;

    /* check for existing page directory */
    यदि ((pdpte_ptr_get_page_आकार(ret.pdptSlot) == pdpte_pdpte_pd) &&
        (pdpte_pdpte_pd_ptr_get_present(ret.pdptSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;
        ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        प्रत्यागम ret;
    }

    ret.pdpte = makeUserPDPTEHugePage(आधारः, attr, vmRights);
    ret.स्थितिः = EXCEPTION_NONE;
    प्रत्यागम ret;
}

exception_t decode_X86_बहुलकम्_Map_Page(word_t label, vm_page_आकार_t page_आकार, cte_t *cte, cap_t cap,
                                 vspace_मूलम्_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_गुणाः_t vm_attr)
{
    यदि (config_समुच्चयः(CONFIG_HUGE_PAGE) && page_आकार == X64_HugePage) {
        create_mapping_pdpte_return_t map_ret;

        map_ret = createSafeMappingEntries_PDPTE(paddr, vaddr, vm_rights, vm_attr, vroot);
        यदि (map_ret.स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम map_ret.स्थितिः;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);

        विकल्प (label) {
        पक्ष X86PageMap:
            प्रत्यागम perform_X64_बहुलकम्_Map(cap, cte, map_ret.pdpte, map_ret.pdptSlot, vroot);

        स्वभाव:
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
    }
    fail("Invalid Page type");
}

#यदि_परिभाषितम् CONFIG_PRINTING
प्रकारनिर्देश संरचना readWordFromVSpace_ret {
    exception_t स्थितिः;
    word_t मानम्;
} readWordFromVSpace_ret_t;

स्थिर readWordFromVSpace_ret_t readWordFromVSpace(vspace_मूलम्_t *vspace, word_t vaddr)
{
    readWordFromVSpace_ret_t ret;
    lookupPTSlot_ret_t ptSlot;
    lookupPDSlot_ret_t pdSlot;
    lookupPDPTSlot_ret_t pdptSlot;
    paddr_t paddr;
    word_t विस्थापनम्;
    pptr_t kernel_vaddr;
    word_t *मानम्;

    pdptSlot = lookupPDPTSlot(vspace, vaddr);
    यदि (pdptSlot.स्थितिः == EXCEPTION_NONE &&
        pdpte_ptr_get_page_आकार(pdptSlot.pdptSlot) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdptSlot.pdptSlot)) {

        paddr = pdpte_pdpte_1g_ptr_get_page_आधारः_स्थानाङ्कः(pdptSlot.pdptSlot);
        विस्थापनम् = vaddr & आवरणम्(se_L4_Huge_Page_द्वयङ्काः);
    } अन्यथा {
        pdSlot = lookupPDSlot(vspace, vaddr);
        यदि (pdSlot.स्थितिः == EXCEPTION_NONE &&
            ((pde_ptr_get_page_आकार(pdSlot.pdSlot) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pdSlot.pdSlot))) {

            paddr = pde_pde_large_ptr_get_page_आधारः_स्थानाङ्कः(pdSlot.pdSlot);
            विस्थापनम् = vaddr & आवरणम्(se_L4_Large_Page_द्वयङ्काः);
        } अन्यथा {
            ptSlot = lookupPTSlot(vspace, vaddr);
            यदि (ptSlot.स्थितिः == EXCEPTION_NONE && pte_ptr_get_present(ptSlot.ptSlot)) {
                paddr = pte_ptr_get_page_आधारः_स्थानाङ्कः(ptSlot.ptSlot);
                विस्थापनम् = vaddr & आवरणम्(se_L4_Page_द्वयङ्काः);
            } अन्यथा {
                ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
                प्रत्यागम ret;
            }
        }
    }


    kernel_vaddr = (word_t)paddr_गन्तव्यम्_pptr(paddr);
    मानम् = (word_t *)(kernel_vaddr + विस्थापनम्);
    ret.स्थितिः = EXCEPTION_NONE;
    ret.मानम् = *मानम्;
    प्रत्यागम ret;
}

शून्य Arch_user_राशिः_Trace(tcb_t *tptr)
{
    cap_t thread_मूलम्;
    vspace_मूलम्_t *vspace_मूलम्;
    word_t sp;
    पूर्णाङ्क i;

    thread_मूलम् = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    यदि (cap_get_capType(thread_मूलम्) != cap_pml4_cap) {
        प्रारूप_लिख("Invalid vspace\n");
        प्रत्यागम;
    }

    vspace_मूलम् = (vspace_मूलम्_t *)pptr_सम्बन्धि_cap(thread_मूलम्);

    sp = get_पञ्जिकाकोशः(tptr, RSP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    यदि (!IS_ALIGNED(sp, se_L4_Word_आकार_द्वयङ्काः)) {
        प्रारूप_लिख("RSP not aligned\n");
        प्रत्यागम;
    }

    प्रति (i = 0; i < CONFIG_USER_राशिः_TRACE_दीर्घता; i++) {
        word_t स्थानाङ्कः = sp + (i * परिमाण(word_t));
        readWordFromVSpace_ret_t परिणाम;
        परिणाम = readWordFromVSpace(vspace_मूलम्, स्थानाङ्कः);
        यदि (परिणाम.स्थितिः == EXCEPTION_NONE) {
            प्रारूप_लिख("0x%lx: 0x%lx\n", (दीर्घ)स्थानाङ्कः, (दीर्घ)परिणाम.मानम्);
        } अन्यथा {
            प्रारूप_लिख("0x%lx: INVALID\n", (दीर्घ)स्थानाङ्कः);
        }
    }
}
#यदि_अन्तः /* CONFIG_PRINTING */

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

    pde_t pde = pde_pde_large_new(
                    0,                 /* xd                   */
                    ks_User_Log_बफर,   /* page_base_address    */
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
#यदि_परिभाषितम् CONFIG_HUGE_PAGE
    x64KSKernelPD[1] = pde;
#अन्यथा
    x64KSKernelPDs[BIT(PDPT_सूचकांक_द्वयङ्काः) - 1][1] = pde;
#यदि_अन्तः
    invalidateTranslationAll(आवरणम्(CONFIG_MAX_अङ्कः_NODES));

    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः /* CONFIG_KERNEL_LOG_BUFFER */
