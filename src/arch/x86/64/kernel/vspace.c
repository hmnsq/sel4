/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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
#అన్తర్భావ <mode/kernel/tlb.h>
#అన్తర్భావ <arch/kernel/tlb_bitmap.h>
#అన్తర్భావ <object/structures.h>

/* When using the SKIM window to isolate the kernel from the user we also need to
 * not use global mappings as having global mappings and entries in the TLB is
 * equivalent, for the purpose of exploitation, to having the mappings in the
 * kernel window */
#నిర్వచించు KERNEL_IS_GLOBAL() (config_సమితి(CONFIG_KERNEL_SKIM_కిటికీ) ? 0 : 1)

/* For the boot code we create two windows into the physical address space
 * One is at the same location as the kernel window, and is placed up high
 * The other is a 1-to-1 mapping of the first 512gb of memory. The purpose
 * of this is to have a 1-to-1 mapping for the low parts of memory, so that
 * when we switch paging on, and are still running at physical addresses,
 * we don't explode. Then we also want the high mappings so we can start
 * running at proper kernel virtual addresses */
pml4e_t boot_pml4[బిట్టు(PML4_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_పుట_బిట్లు)) VISIBLE PHYS_BSS;
pdpte_t boot_pdpt[బిట్టు(PDPT_సూచీ_బిట్లు)] ALIGN(బిట్టు(se_L4_పుట_బిట్లు)) VISIBLE PHYS_BSS;

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

BOOT_సంకేతం bool_t map_kernel_కిటికీ(
    uint32_t అంకె_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   అంకె_drhu,
    paddr_t   *drhu_list
)
{

    uint64_t paddr;
    uint64_t vaddr;

#నిర్వచితమైతే CONFIG_HUGE_పుట
    /* using 1 GiB page size */

    /* verify that the kernel window as at the last entry of the PML4 */
    ప్రతిజ్ఞా(GET_PML4_సూచీ(PPTR_పునాది) == బిట్టు(PML4_సూచీ_బిట్లు) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    ప్రతిజ్ఞా(GET_PML4_సూచీ(KERNEL_ELF_పునాది) == బిట్టు(PML4_సూచీ_బిట్లు) - 1);
    ప్రతిజ్ఞా(GET_PDPT_సూచీ(KERNEL_ELF_పునాది) == బిట్టు(PML4_సూచీ_బిట్లు) - 2);
    ప్రతిజ్ఞా(GET_PDPT_సూచీ(KDEV_పునాది) == బిట్టు(PML4_సూచీ_బిట్లు) - 1);
    ప్రతిజ్ఞా(IS_ALIGNED(KERNEL_ELF_పునాది - KERNEL_ELF_PADDR_పునాది, se_L4_Huge_పుట_బిట్లు));
    ప్రతిజ్ఞా(IS_ALIGNED(KDEV_పునాది, se_L4_Huge_పుట_బిట్లు));
    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_సూచీ(PPTR_పునాది)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_గమ్యం_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );
    /* put the 1GB kernel_base mapping into the PDPT */
    x64KSKernelPDPT[GET_PDPT_సూచీ(KERNEL_ELF_పునాది)] = pdpte_pdpte_1g_new(
                                                           0, /* xd */
                                                           PADDR_పునాది,
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
    vaddr = PPTR_పునాది;
    కొరకు (paddr = 0; paddr < PADDR_శిఖరం;
         paddr += బిట్టు(se_L4_Huge_పుట_బిట్లు)) {

        పూర్ణాంకం pdpte_సూచీ = GET_PDPT_సూచీ(vaddr);
        x64KSKernelPDPT[pdpte_సూచీ] = pdpte_pdpte_1g_new(
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

        vaddr += బిట్టు(se_L4_Huge_పుట_బిట్లు);
    }

    /* put the PD into the PDPT */
    x64KSKernelPDPT[GET_PDPT_సూచీ(KDEV_పునాది)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_గమ్యం_paddr(x64KSKernelPD),
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
                           kpptr_గమ్యం_paddr(x64KSKernelPT),
                           0, /* accessed */
                           0, /* cache_disabled */
                           0, /* write_through */
                           0, /* super_user */
                           1, /* read_write */
                           1  /* present */
                       );
#లేకపోతే

    పూర్ణాంకం pd_సూచీ_2 = 0;
    /* use 2 MiB page size */
    /* verify that the kernel window as at the last entry of the PML4 */
    ప్రతిజ్ఞా(GET_PML4_సూచీ(PPTR_పునాది) == బిట్టు(PML4_సూచీ_బిట్లు) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    ప్రతిజ్ఞా(GET_PML4_సూచీ(KERNEL_ELF_పునాది) == బిట్టు(PML4_సూచీ_బిట్లు) - 1);
    ప్రతిజ్ఞా(GET_PDPT_సూచీ(KERNEL_ELF_పునాది) == బిట్టు(PML4_సూచీ_బిట్లు) - 2);
    ప్రతిజ్ఞా(GET_PDPT_సూచీ(KDEV_పునాది) == బిట్టు(PML4_సూచీ_బిట్లు) - 1);
    ప్రతిజ్ఞా(IS_ALIGNED(KERNEL_ELF_పునాది - KERNEL_ELF_PADDR_పునాది, se_L4_Huge_పుట_బిట్లు));
    ప్రతిజ్ఞా(IS_ALIGNED(KDEV_పునాది, se_L4_Huge_పుట_బిట్లు));

    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_సూచీ(PPTR_పునాది)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_గమ్యం_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    కొరకు (pd_సూచీ_2 = 0; pd_సూచీ_2 < PADDR_శిఖరం >> se_L4_Huge_పుట_బిట్లు; pd_సూచీ_2++) {
        /* put the 1GB kernel_base mapping into the PDPT */
        x64KSKernelPDPT[GET_PDPT_సూచీ(PPTR_పునాది) + pd_సూచీ_2] = pdpte_pdpte_pd_new(
                                                                    0, /* xd */
                                                                    kpptr_గమ్యం_paddr(&x64KSKernelPDs[pd_సూచీ_2][0]),
                                                                    0, /* accessed */
                                                                    0, /* cache disabled */
                                                                    0, /* write through */
                                                                    0, /* super user */
                                                                    1, /* read write */
                                                                    1 /* present */
                                                                );
    }

    x64KSKernelPDPT[GET_PDPT_సూచీ(KERNEL_ELF_పునాది)] = pdpte_pdpte_pd_new(
                                                           0, /* xd */
                                                           kpptr_గమ్యం_paddr(&x64KSKernelPDs[0][0]),
                                                           0, /* accessed */
                                                           0, /* cache disable */
                                                           1, /* write through */
                                                           0, /* super user */
                                                           1, /* read write */
                                                           1  /* present */
                                                       );

    paddr = 0;
    vaddr = PPTR_పునాది;

    కొరకు (paddr = 0; paddr < PADDR_శిఖరం;
         paddr += 0x200000) {

        పూర్ణాంకం pd_సూచీ_2 = GET_PDPT_సూచీ(vaddr) - GET_PDPT_సూచీ(PPTR_పునాది);
        పూర్ణాంకం pde_సూచీ = GET_PD_సూచీ(vaddr);

        x64KSKernelPDs[pd_సూచీ_2][pde_సూచీ] = pde_pde_large_new(
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
    x64KSKernelPDPT[GET_PDPT_సూచీ(KDEV_పునాది)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_గమ్యం_paddr(&x64KSKernelPDs[బిట్టు(PDPT_సూచీ_బిట్లు) - 1][0]),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    /* put the PT into the PD */
    x64KSKernelPDs[బిట్టు(PDPT_సూచీ_బిట్లు) - 1][0] = pde_pde_pt_new(
                                                      0, /* xd */
                                                      kpptr_గమ్యం_paddr(x64KSKernelPT),
                                                      0, /* accessed */
                                                      0, /* cache_disabled */
                                                      0, /* write_through */
                                                      0, /* super_user */
                                                      1, /* read_write */
                                                      1  /* present */
                                                  );
#అయితే_ముగింపు

#ఒకవేళ CONFIG_MAX_అంకె_TRACE_POINTS > 0
    /* use the last PD entry as the benchmark log storage.
     * the actual backing physical memory will be filled
     * later by using alloc_region */
    ksLog = (ks_log_నమోదు_t *)(KDEV_పునాది + 0x200000 * (బిట్టు(PD_సూచీ_బిట్లు) - 1));
#అయితే_ముగింపు

    /* now map in the kernel devices */
    ఒకవేళ (!map_kernel_కిటికీ_devices(x64KSKernelPT, అంకె_ioapic, ioapic_paddrs, అంకె_drhu, drhu_list)) {
        తిరిగి false;
    }

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(x64KSKernelPML4);
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    /* In boot code, so fine to just trash everything here */
    invalidateLocalTranslationAll();
    రాయి("Mapping kernel window is done\n");
    తిరిగి true;
}

#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
BOOT_సంకేతం bool_t map_skim_కిటికీ(vptr_t skim_ప్రారంభం, vptr_t skim_చివర)
{
    /* place the PDPT into the PML4 */
    x64KSSKIMPML4[GET_PML4_సూచీ(PPTR_పునాది)] = pml4e_new(
                                                   0, /* xd */
                                                   kpptr_గమ్యం_paddr(x64KSSKIMPDPT),
                                                   0, /* accessed */
                                                   0, /* cache_disabled */
                                                   0, /* write_through */
                                                   0, /* super_user */
                                                   1, /* read_write */
                                                   1  /* present */
                                               );
    /* place the PD into the kernel_base slot of the PDPT */
    x64KSSKIMPDPT[GET_PDPT_సూచీ(KERNEL_ELF_పునాది)] = pdpte_pdpte_pd_new(
                                                         0, /* xd */
                                                         kpptr_గమ్యం_paddr(x64KSSKIMPD),
                                                         0, /* accessed */
                                                         0, /* cache_disabled */
                                                         0, /* write_through */
                                                         0, /* super_user */
                                                         1, /* read_write */
                                                         1  /* present */
                                                     );
    /* map the skim portion into the PD. we expect it to be 2M aligned */
    ప్రతిజ్ఞా((skim_ప్రారంభం % బిట్టు(se_L4_Large_పుట_బిట్లు)) == 0);
    ప్రతిజ్ఞా((skim_చివర % బిట్టు(se_L4_Large_పుట_బిట్లు)) == 0);
    uint64_t paddr = kpptr_గమ్యం_paddr((శూన్య *)skim_ప్రారంభం);
    కొరకు (పూర్ణాంకం i = GET_PD_సూచీ(skim_ప్రారంభం); i < GET_PD_సూచీ(skim_చివర); i++) {
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
        paddr += బిట్టు(se_L4_Large_పుట_బిట్లు);
    }
    తిరిగి true;
}
#అయితే_ముగింపు

BOOT_సంకేతం శూన్య init_tss(tss_t *tss)
{
    మాట_t పునాది = (మాట_t)&x64KSIRQStack[CURRENT_CPU_సూచీ()][IRQ_దొంతర_కొలత];
    *tss = tss_new(
               పరిమాణం(*tss),   /* io map base */
               0, 0,       /* ist 7 */
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               /* ist 1 is the stack frame we use for interrupts */
               పునాది >> 32, పునాది & 0xffffffff,  /* ist 1 */
               0, 0,       /* rsp 2 */
               0, 0,       /* rsp 1 */
               0, 0        /* rsp 0 */
           );
    /* set the IO map to all 1 to block user IN/OUT instructions */
    memset(&x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KStss.io_map[0], 0xff,
           పరిమాణం(x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KStss.io_map));
}

శూన్య init_syscall_msrs(శూన్య)
{
    x86_wrmsr(IA32_LSTAR_MSR, (uint64_t)&handle_fastsyscall);
    // mask bit 9 in the kernel (which is the interrupt enable bit)
    // also mask bit 8, which is the Trap Flag, to prevent the kernel
    // from single stepping
    x86_wrmsr(IA32_FMASK_MSR, FLAGS_TF | FLAGS_IF);
    x86_wrmsr(IA32_నక్షత్రం_MSR, ((uint64_t)SEL_CS_0 << 32) | ((uint64_t)SEL_CS_3 << 48));
}

BOOT_సంకేతం శూన్య init_gdt(gdt_నమోదు_t *gdt, tss_t *tss)
{

    uint64_t tss_పునాది = (uint64_t)tss;
    gdt_tss_t gdt_tss;

    gdt[GDT_NULL] = gdt_నమోదు_gdt_null_new();

    gdt[GDT_CS_0] = gdt_నమోదు_gdt_సంకేతం_new(
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

    gdt[GDT_DS_0] = gdt_నమోదు_gdt_దత్తాంశం_new(
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

    gdt[GDT_CS_3] = gdt_నమోదు_gdt_సంకేతం_new(
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

    gdt[GDT_DS_3] = gdt_నమోదు_gdt_దత్తాంశం_new(
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

    gdt[GDT_FS] = gdt_నమోదు_gdt_దత్తాంశం_new(
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

    gdt[GDT_GS] = gdt_నమోదు_gdt_దత్తాంశం_new(
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
                  tss_పునాది >> 32,                     /* base 63 - 32 */
                  (tss_పునాది & 0xff000000UL) >> 24,    /* base 31 - 24 */
                  1,                                  /* granularity */
                  0,                                  /* avl */
                  0,                                  /* limit high */
                  1,                                  /* present */
                  0,                                  /* dpl */
                  9,                                  /* desc type */
                  (tss_పునాది & 0xff0000UL) >> 16,      /* base 23-16 */
                  (tss_పునాది & 0xffffUL),              /* base 15 - 0 */
                  పరిమాణం(tss_io_t) - 1
              );

    gdt[GDT_TSS].పదాలు[0] = gdt_tss.పదాలు[0];
    gdt[GDT_TSS + 1].పదాలు[0] = gdt_tss.పదాలు[1];
}

BOOT_సంకేతం శూన్య init_idt_నమోదు(idt_నమోదు_t *idt, అంతరాయం_t అంతరాయం, శూన్య(*handler)(శూన్య))
{
    uint64_t handler_addr = (uint64_t)handler;
    uint64_t dpl = 3;

    ఒకవేళ (అంతరాయం < int_trap_min && అంతరాయం != int_software_break_request) {
        dpl = 0;
    }

    idt[అంతరాయం] = idt_నమోదు_అంతరాయం_gate_new(
                         handler_addr >> 32,                 /* offset 63 - 32 */
                         ((handler_addr >> 16) & 0xffff),
                         1,                                  /* present */
                         dpl,                                /* dpl */
                         1,                                  /* ist */
                         SEL_CS_0,                           /* segment selector */
                         (handler_addr & 0xffff)               /* offset 15 - 0 */
                     );
}

శూన్య సమితి_VMRoot(tcb_t *tcb)
{
    cap_t thread_మూలం;
    asid_t asid;
    pml4e_t *pml4;
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;
    cr3_t cr3;

    thread_మూలం = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    ఒకవేళ (cap_get_capType(thread_మూలం) != cap_pml4_cap ||
        !cap_pml4_cap_get_capPML4IsMapped(thread_మూలం)) {
        సమితి_Current_User_VSpace_మూలం(kpptr_గమ్యం_paddr(X86_GLOBAL_VSPACE_మూలం), 0);
        తిరిగి;
    }

    pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_పునాది_Ptr(thread_మూలం));
    asid = cap_pml4_cap_get_capPML4MappedASID(thread_మూలం);
    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (unlikely(కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE || కనుగొను_ret.vspace_మూలం != pml4)) {
        సమితి_Current_User_VSpace_మూలం(kpptr_గమ్యం_paddr(X86_GLOBAL_VSPACE_మూలం), 0);
        తిరిగి;
    }
    cr3 = makeCR3(pptr_గమ్యం_paddr(pml4), asid);
    ఒకవేళ (getCurrentUserCR3().పదాలు[0] != cr3.పదాలు[0]) {
        SMP_COND_STATEMENT(tlb_bitmap_సమితి(pml4, getCurrentCPUIndex());)
        సమితి_Current_User_CR3(cr3);
    }
}


BOOT_సంకేతం శూన్య init_dtrs(శూన్య)
{
    gdt_idt_ptr.హద్దు = (పరిమాణం(gdt_నమోదు_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.పునాది = (uint64_t)x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KSgdt;

    /* When we install the gdt it will clobber any value of gs that
     * we have. Since we might be using it for TLS we can stash
     * and unstash any gs value using swapgs
     */
    swapgs();
    x64_install_gdt(&gdt_idt_ptr);
    swapgs();

    gdt_idt_ptr.హద్దు = (పరిమాణం(idt_నమోదు_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.పునాది = (uint64_t)x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KSidt;
    x64_install_idt(&gdt_idt_ptr);

    x64_install_ldt(SEL_NULL);

    x64_install_tss(SEL_TSS);
}

BOOT_సంకేతం శూన్య map_it_చట్రం_cap(cap_t pd_cap, cap_t చట్రం_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_యొక్క_cap(pd_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt;
    vptr_t vptr = cap_చట్రం_cap_get_cap_FMapped_చిరునామా(చట్రం_cap);
    శూన్య *pptr = (శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(చట్రం_cap);

    ప్రతిజ్ఞా(cap_చట్రం_cap_get_cap_FMap_Type(చట్రం_cap) == X86_MappingVSpace);
    ప్రతిజ్ఞా(cap_చట్రం_cap_get_cap_FMapped_ASID(చట్రం_cap) != asidInvalid);
    pml4 += GET_PML4_సూచీ(vptr);
    ప్రతిజ్ఞా(pml4e_ptr_get_present(pml4));
    pdpt = paddr_గమ్యం_pptr(pml4e_ptr_get_pdpt_పునాది_చిరునామా(pml4));
    pdpt += GET_PDPT_సూచీ(vptr);
    ప్రతిజ్ఞా(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_గమ్యం_pptr(pdpte_pdpte_pd_ptr_get_pd_పునాది_చిరునామా(pdpt));
    pd += GET_PD_సూచీ(vptr);
    ప్రతిజ్ఞా(pde_pde_pt_ptr_get_present(pd));
    pt = paddr_గమ్యం_pptr(pde_pde_pt_ptr_get_pt_పునాది_చిరునామా(pd));
    *(pt + GET_PT_సూచీ(vptr)) = pte_new(
                                     0,                      /* xd                   */
                                     pptr_గమ్యం_paddr(pptr),    /* page_base_address    */
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

స్థిర BOOT_సంకేతం శూన్య map_it_pdpt_cap(cap_t vspace_cap, cap_t pdpt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_యొక్క_cap(vspace_cap));
    pdpte_t *pdpt = PDPT_PTR(cap_pdpt_cap_get_capPDPTBasePtr(pdpt_cap));
    vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_చిరునామా(pdpt_cap);

    ప్రతిజ్ఞా(cap_pdpt_cap_get_capPDPTIsMapped(pdpt_cap));
    *(pml4 + GET_PML4_సూచీ(vptr)) = pml4e_new(
                                         0,                      /* xd                   */
                                         pptr_గమ్యం_paddr(pdpt),    /* pdpt_base_address    */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_సంకేతం శూన్య map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_యొక్క_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd = PD_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    vptr_t vptr = cap_పుట_directory_cap_get_cap_PDMapped_చిరునామా(pd_cap);

    ప్రతిజ్ఞా(cap_పుట_directory_cap_get_cap_PDIs_Mapped(pd_cap));
    pml4 += GET_PML4_సూచీ(vptr);
    ప్రతిజ్ఞా(pml4e_ptr_get_present(pml4));
    pdpt = paddr_గమ్యం_pptr(pml4e_ptr_get_pdpt_పునాది_చిరునామా(pml4));
    *(pdpt + GET_PDPT_సూచీ(vptr)) = pdpte_pdpte_pd_new(
                                         0,                      /* xd                   */
                                         pptr_గమ్యం_paddr(pd),      /* pd_base_address      */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_సంకేతం శూన్య map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_యొక్క_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt = PT_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(pt_cap);

    ప్రతిజ్ఞా(cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(pt_cap));
    pml4 += GET_PML4_సూచీ(vptr);
    ప్రతిజ్ఞా(pml4e_ptr_get_present(pml4));
    pdpt = paddr_గమ్యం_pptr(pml4e_ptr_get_pdpt_పునాది_చిరునామా(pml4));
    pdpt += GET_PDPT_సూచీ(vptr);
    ప్రతిజ్ఞా(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_గమ్యం_pptr(pdpte_pdpte_pd_ptr_get_pd_పునాది_చిరునామా(pdpt));
    *(pd + GET_PD_సూచీ(vptr)) = pde_pde_pt_new(
                                     0,                      /* xd                   */
                                     pptr_గమ్యం_paddr(pt),      /* pt_base_address      */
                                     0,                      /* accessed             */
                                     0,                      /* cache_disabled       */
                                     0,                      /* write_through        */
                                     1,                      /* super_user           */
                                     1,                      /* read_write           */
                                     1                       /* present              */
                                 );
}

BOOT_సంకేతం శూన్య *map_తాత్కాలిక_boot_పుట(శూన్య *నమోదు_2, uint32_t large_పుటలు)
{
    /* this function is for legacy 32-bit systems where the ACPI tables might
     * collide with the kernel window. Here we just assert that the table is
     * in fact in the lower 4GiB region (which is already 1-to-1 mapped) and
     * continue */
    ప్రతిజ్ఞా((మాట_t)నమోదు_2 < బిట్టు(32));
    తిరిగి నమోదు_2;
}

స్థిర BOOT_సంకేతం cap_t create_it_pdpt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_pdpt_cap_new(
              asid,   /* capPDPTMappedASID    */
              pptr,   /* capPDPTBasePtr       */
              1,      /* capPDPTIsMapped      */
              vptr    /* capPDPTMappedAddress */
          );
    map_it_pdpt_cap(vspace_cap, cap);
    తిరిగి cap;
}

స్థిర BOOT_సంకేతం cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_పుట_directory_cap_new(
              asid,   /* capPDMappedASID      */
              pptr,   /* capPDBasePtr         */
              1,      /* capPDIsMapped        */
              vptr    /* capPDMappedAddress   */
          );
    map_it_pd_cap(vspace_cap, cap);
    తిరిగి cap;
}

స్థిర BOOT_సంకేతం cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_పుట_పట్టిక_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );
    map_it_pt_cap(vspace_cap, cap);
    తిరిగి cap;
}


BOOT_సంకేతం మాట_t arch_get_n_paging(v_region_t it_v_reg)
{
    మాట_t n = get_n_paging(it_v_reg, PD_సూచీ_స్థానభ్రంశం);
    n += get_n_paging(it_v_reg, PDPT_సూచీ_స్థానభ్రంశం);
    n += get_n_paging(it_v_reg, PML4_సూచీ_స్థానభ్రంశం);
#నిర్వచితమైతే CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#అయితే_ముగింపు
    తిరిగి n;
}

BOOT_సంకేతం cap_t create_it_చిరునామా_space(cap_t మూలం_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_స్థానం_Pos స్థానం_pos_before;
    se_L4_స్థానం_Pos స్థానం_pos_after;

    స్థానం_pos_before = ndks_boot.స్థానం_pos_cur;
    copyGlobalMappings(PML4_PTR(rootserver.vspace));
    vspace_cap = cap_pml4_cap_new(
                     IT_ASID,        /* capPML4MappedASID */
                     rootserver.vspace,           /* capPML4BasePtr   */
                     1               /* capPML4IsMapped   */
                 );


    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

    /* Create any PDPTs needed for the user land image */
    కొరకు (vptr = విడత_కిందికి(it_v_reg.ప్రారంభం, PML4_సూచీ_స్థానభ్రంశం);
         vptr < it_v_reg.చివర;
         vptr += బిట్టు(PML4_సూచీ_స్థానభ్రంశం)) {
        ఒకవేళ (!provide_cap(మూలం_cnode_cap,
                         create_it_pdpt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            తిరిగి cap_null_cap_new();
        }
    }

    /* Create any PDs needed for the user land image */
    కొరకు (vptr = విడత_కిందికి(it_v_reg.ప్రారంభం, PDPT_సూచీ_స్థానభ్రంశం);
         vptr < it_v_reg.చివర;
         vptr += బిట్టు(PDPT_సూచీ_స్థానభ్రంశం)) {
        ఒకవేళ (!provide_cap(మూలం_cnode_cap,
                         create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            తిరిగి cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    కొరకు (vptr = విడత_కిందికి(it_v_reg.ప్రారంభం, PD_సూచీ_స్థానభ్రంశం);
         vptr < it_v_reg.చివర;
         vptr += బిట్టు(PD_సూచీ_స్థానభ్రంశం)) {
        ఒకవేళ (!provide_cap(మూలం_cnode_cap,
                         create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
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

శూన్య copyGlobalMappings(vspace_మూలం_t *new_vspace)
{
    గుర్తులేని దీర్ఘ i;
    pml4e_t *vspace = (pml4e_t *)new_vspace;

    /* Copy from the tlbbitmap_pptr so that we copy the default entries of the
     * tlb bitmap (if it exists). If it doesn't exist then this loop
     * will be equivalent to copying from PPTR_BASE
     */
    కొరకు (i = GET_PML4_సూచీ(TLBBITMAP_PPTR); i < బిట్టు(PML4_సూచీ_బిట్లు); i++) {
        vspace[i] = X86_GLOBAL_VSPACE_మూలం[i];
    }
}

స్థిర BOOT_సంకేతం cap_t create_it_చట్రం_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large, se_L4_మాట map_type)
{
    vm_పుట_కొలత_t చట్రం_కొలత_2;

    ఒకవేళ (use_large) {
        చట్రం_కొలత_2 = X86_Large_పుట;
    } లేకపోతే {
        చట్రం_కొలత_2 = X86_Small_పుట;
    }

    తిరిగి
        cap_చట్రం_cap_new(
            asid,                          /* capFMappedASID     */
            pptr,                          /* capFBasePtr        */
            చట్రం_కొలత_2,                    /* capFSize           */
            map_type,                      /* capFMapType        */
            vptr,                          /* capFMappedAddress  */
            మాట_From_VMRights(VMReadWrite), /* capFVMRights       */
            0                              /* capFIsDevice       */
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

/* ====================== BOOT CODE FINISHES HERE ======================== */



exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_స్థానం)
{
    asid_map_t asid_map;
#నిర్వచితమైతే CONFIG_VTX
    ఒకవేళ (cap_get_capType(vspace_Cap_స్థానం->cap) == cap_ept_pml4_cap) {
        cap_ept_pml4_cap_ptr_సమితి_cap_PML4_Mapped_ASID(&vspace_Cap_స్థానం->cap, asid);
        cap_ept_pml4_cap_ptr_సమితి_cap_PML4_Is_Mapped(&vspace_Cap_స్థానం->cap, 1);
        asid_map = asid_map_asid_map_ept_new(cap_ept_pml4_cap_get_cap_PML4_పునాది_Ptr(vspace_Cap_స్థానం->cap));
    } లేకపోతే
#అయితే_ముగింపు
    {
        ప్రతిజ్ఞా(cap_get_capType(vspace_Cap_స్థానం->cap) == cap_pml4_cap);
        cap_pml4_cap_ptr_సమితి_cap_PML4_Mapped_ASID(&vspace_Cap_స్థానం->cap, asid);
        cap_pml4_cap_ptr_సమితి_cap_PML4_Is_Mapped(&vspace_Cap_స్థానం->cap, 1);
        asid_map = asid_map_asid_map_vspace_new(cap_pml4_cap_get_cap_PML4_పునాది_Ptr(vspace_Cap_స్థానం->cap));
    }
    poolPtr->array[ASID_దిగువ(asid)] = asid_map;
    తిరిగి EXCEPTION_NONE;
}

bool_t CONST is_VTable_మూలం(cap_t cap)
{
    తిరిగి cap_get_capType(cap) == cap_pml4_cap;
}

bool_t CONST is_Valid_Native_మూలం(cap_t cap)
{
    తిరిగి is_VTable_మూలం(cap) &&
           cap_pml4_cap_get_capPML4IsMapped(cap);
}

స్థిర pml4e_t CONST makeUserPML4E(paddr_t paddr, vm_లక్షణాలు_t vm_attr)
{
    తిరిగి pml4e_new(
               0,
               paddr,
               0,
               vm_లక్షణాలు_get_x86_PCDBit(vm_attr),
               vm_లక్షణాలు_get_x86_PWTBit(vm_attr),
               1,
               1,
               1
           );
}

స్థిర pml4e_t CONST makeUserPML4EInvalid(శూన్య)
{
    తిరిగి pml4e_new(
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

స్థిర pdpte_t CONST make_User_PDPTEHuge_పుట(paddr_t paddr, vm_లక్షణాలు_t vm_attr, vm_rights_t vm_rights)
{
    తిరిగి pdpte_pdpte_1g_new(
               0,          /* xd               */
               paddr,      /* physical address */
               0,          /* PAT              */
               0,          /* global           */
               0,          /* dirty            */
               0,          /* accessed         */
               vm_లక్షణాలు_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_లక్షణాలు_get_x86_PWTBit(vm_attr),  /* write through  */
               SuperUserFromVMRights(vm_rights),       /* super user     */
               WritableFromVMRights(vm_rights),        /* read write     */
               1                                       /* present        */
           );
}

స్థిర pdpte_t CONST makeUserPDPTEPageDirectory(paddr_t paddr, vm_లక్షణాలు_t vm_attr)
{
    తిరిగి pdpte_pdpte_pd_new(
               0,                      /* xd       */
               paddr,                  /* paddr    */
               0,                      /* accessed */
               vm_లక్షణాలు_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_లక్షణాలు_get_x86_PWTBit(vm_attr),  /* write through  */
               1,                      /* super user */
               1,                      /* read write */
               1                       /* present    */
           );
}

స్థిర pdpte_t CONST makeUserPDPTEInvalid(శూన్య)
{
    తిరిగి pdpte_pdpte_pd_new(
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

pde_t CONST make_User_PDELarge_పుట(paddr_t paddr, vm_లక్షణాలు_t vm_attr, vm_rights_t vm_rights)
{
    తిరిగి pde_pde_large_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               vm_లక్షణాలు_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_లక్షణాలు_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_లక్షణాలు_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_పట్టిక(paddr_t paddr, vm_లక్షణాలు_t vm_attr)
{

    తిరిగి  pde_pde_pt_new(
                0,                                  /* xd               */
                paddr,                              /* pt_base_address  */
                0,                                  /* accessed         */
                vm_లక్షణాలు_get_x86_PCDBit(vm_attr), /* cache_disabled   */
                vm_లక్షణాలు_get_x86_PWTBit(vm_attr), /* write_through    */
                1,                                  /* super_user       */
                1,                                  /* read_write       */
                1                                   /* present          */
            );
}

pde_t CONST makeUserPDEInvalid(శూన్య)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    తిరిగి pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_లక్షణాలు_t vm_attr, vm_rights_t vm_rights)
{
    తిరిగి pte_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               0,                                              /* global               */
               vm_లక్షణాలు_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_లక్షణాలు_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_లక్షణాలు_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pte_t CONST makeUserPTEInvalid(శూన్య)
{
    తిరిగి pte_new(
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


స్థిర pml4e_t *lookup_PML4_స్థానం(vspace_మూలం_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4e = PML4E_PTR(pml4);
    మాట_t pml4_సూచీ = GET_PML4_సూచీ(vptr);
    తిరిగి pml4e + pml4_సూచీ;
}

స్థిర lookupPDPTSlot_ret_t lookupPDPTSlot(vspace_మూలం_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4_స్థానం = lookup_PML4_స్థానం(pml4, vptr);
    lookupPDPTSlot_ret_t ret;

    ఒకవేళ (!pml4e_ptr_get_present(pml4_స్థానం)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PML4_సూచీ_స్థానభ్రంశం);

        ret.pdpt_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    } లేకపోతే {
        pdpte_t *pdpt;
        pdpte_t *pdpt_స్థానం;
        మాట_t pdpt_సూచీ = GET_PDPT_సూచీ(vptr);
        pdpt = paddr_గమ్యం_pptr(pml4e_ptr_get_pdpt_పునాది_చిరునామా(pml4_స్థానం));
        pdpt_స్థానం = pdpt + pdpt_సూచీ;

        ret.స్థితి_2 = EXCEPTION_NONE;
        ret.pdpt_స్థానం = pdpt_స్థానం;
        తిరిగి ret;
    }
}

lookupPDSlot_ret_t lookupPDSlot(vspace_మూలం_t *pml4, vptr_t vptr)
{
    lookupPDPTSlot_ret_t pdpt_స్థానం;
    lookupPDSlot_ret_t ret;

    pdpt_స్థానం = lookupPDPTSlot(pml4, vptr);

    ఒకవేళ (pdpt_స్థానం.స్థితి_2 != EXCEPTION_NONE) {
        ret.pd_స్థానం = ఖాళీ;
        ret.స్థితి_2 = pdpt_స్థానం.స్థితి_2;
        తిరిగి ret;
    }
    ఒకవేళ ((pdpte_ptr_get_పుట_కొలత(pdpt_స్థానం.pdpt_స్థానం) != pdpte_pdpte_pd) ||
        !pdpte_pdpte_pd_ptr_get_present(pdpt_స్థానం.pdpt_స్థానం)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PDPT_సూచీ_స్థానభ్రంశం);

        ret.pd_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    } లేకపోతే {
        pde_t *pd;
        pde_t *pd_స్థానం;
        మాట_t pd_సూచీ = GET_PD_సూచీ(vptr);
        pd = paddr_గమ్యం_pptr(pdpte_pdpte_pd_ptr_get_pd_పునాది_చిరునామా(pdpt_స్థానం.pdpt_స్థానం));
        pd_స్థానం = pd + pd_సూచీ;

        ret.స్థితి_2 = EXCEPTION_NONE;
        ret.pd_స్థానం = pd_స్థానం;
        తిరిగి ret;
    }
}

స్థిర శూన్య flushPD(vspace_మూలం_t *vspace, మాట_t vptr, pde_t *pd, asid_t asid)
{
    /* clearing the entire PCID vs flushing the virtual addresses
     * one by one using invplg.
     * choose the easy way, invalidate the PCID
     */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));

}

స్థిర శూన్య flushPDPT(vspace_మూలం_t *vspace, మాట_t vptr, pdpte_t *pdpt, asid_t asid)
{
    /* similar here */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    తిరిగి;
}

శూన్య hwASIDInvalidate(asid_t asid, vspace_మూలం_t *vspace)
{
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
}

శూన్య unmap_పుట_Directory(asid_t asid, vptr_t vaddr, pde_t *pd)
{
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;
    lookupPDPTSlot_ret_t    lu_ret;

    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి;
    }

    lu_ret = lookupPDPTSlot(కనుగొను_ret.vspace_మూలం, vaddr);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి;
    }

    /* check if the PDPT has the PD */
    ఒకవేళ (!(pdpte_ptr_get_పుట_కొలత(lu_ret.pdpt_స్థానం) == pdpte_pdpte_pd &&
          pdpte_pdpte_pd_ptr_get_present(lu_ret.pdpt_స్థానం) &&
          (pdpte_pdpte_pd_ptr_get_pd_పునాది_చిరునామా(lu_ret.pdpt_స్థానం) == pptr_గమ్యం_paddr(pd)))) {
        తిరిగి;
    }

    flushPD(కనుగొను_ret.vspace_మూలం, vaddr, pd, asid);

    *lu_ret.pdpt_స్థానం = makeUserPDPTEInvalid();

    invalidate_పుట_Structure_Cache_ASID(pptr_గమ్యం_paddr(కనుగొను_ret.vspace_మూలం), asid,
                                     SMP_TERNARY(tlb_bitmap_get(కనుగొను_ret.vspace_మూలం), 0));
}


స్థిర exception_t perform_X64_పుట_Directory_Invocation_Unmap(cap_t cap, cte_t *ct_స్థానం)
{

    ఒకవేళ (cap_పుట_directory_cap_get_cap_PDIs_Mapped(cap)) {
        pde_t *pd = PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(cap));
        unmap_పుట_Directory(
            cap_పుట_directory_cap_get_cap_PDMapped_ASID(cap),
            cap_పుట_directory_cap_get_cap_PDMapped_చిరునామా(cap),
            pd
        );
        clear_జ్ఞాపకం((శూన్య *)pd, cap_get_cap_కొలత_బిట్లు(cap));
    }

    cap_పుట_directory_cap_ptr_సమితి_cap_PDIs_Mapped(&(ct_స్థానం->cap), 0);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_X64_పుట_Directory_Invocation_Map(cap_t cap, cte_t *ct_స్థానం, pdpte_t pdpte, pdpte_t *pdpt_స్థానం,
                                                        vspace_మూలం_t *vspace)
{
    ct_స్థానం->cap = cap;
    *pdpt_స్థానం = pdpte;
    invalidate_పుట_Structure_Cache_ASID(pptr_గమ్యం_paddr(vspace), cap_పుట_directory_cap_get_cap_PDMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    తిరిగి EXCEPTION_NONE;
}


స్థిర exception_t decode_X64_పుట_Directory_Invocation(
    మాట_t label,
    మాట_t నిడివి,
    cte_t *cte,
    cap_t cap,
    మాట_t *బఫరు
)
{
    మాట_t              vaddr;
    vm_లక్షణాలు_t     vm_attr;
    cap_t               vspaceCap;
    vspace_మూలం_t      *vspace;
    pdpte_t             pdpte;
    paddr_t             paddr;
    asid_t              asid;
    lookupPDPTSlot_ret_t pdpt_స్థానం;

    ఒకవేళ (label == X86_పుట_Directory_Unmap) {
        ఒకవేళ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            userError("X86PageDirectory: Cannot unmap if more than one cap exist.");
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);

        తిరిగి perform_X64_పుట_Directory_Invocation_Unmap(cap, cte);
    }

    ఒకవేళ (label != X86_పుట_Directory_Map) {
        userError("X64Directory: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("X64PageDirectory: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_పుట_directory_cap_get_cap_PDIs_Mapped(cap)) {
        userError("X64PageDirectory: PD is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, బఫరు) & (~ముసుగు(PDPT_సూచీ_స్థానభ్రంశం));
    vm_attr = vm_లక్షణాలు_From_మాట(getSyscallArg(1, బఫరు));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (!is_Valid_Native_మూలం(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_మూలం_t *)pptr_యొక్క_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    ఒకవేళ (vaddr > USER_శిఖరం) {
        userError("X64PageDirectory: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;

    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (కనుగొను_ret.vspace_మూలం != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pdpt_స్థానం = lookupPDPTSlot(vspace, vaddr);
    ఒకవేళ (pdpt_స్థానం.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ ((pdpte_ptr_get_పుట_కొలత(pdpt_స్థానం.pdpt_స్థానం) == pdpte_pdpte_pd &&
         pdpte_pdpte_pd_ptr_get_present(pdpt_స్థానం.pdpt_స్థానం)) ||
        (pdpte_ptr_get_పుట_కొలత(pdpt_స్థానం.pdpt_స్థానం) == pdpte_pdpte_1g
         && pdpte_pdpte_1g_ptr_get_present(pdpt_స్థానం.pdpt_స్థానం))) {
        current_syscall_error.type = se_L4_Delete_మొదటి;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_గమ్యం_paddr(PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(cap)));
    pdpte = makeUserPDPTEPageDirectory(paddr, vm_attr);

    cap = cap_పుట_directory_cap_సమితి_cap_PDIs_Mapped(cap, 1);
    cap = cap_పుట_directory_cap_సమితి_cap_PDMapped_ASID(cap, asid);
    cap = cap_పుట_directory_cap_సమితి_cap_PDMapped_చిరునామా(cap, vaddr);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి perform_X64_పుట_Directory_Invocation_Map(cap, cte, pdpte, pdpt_స్థానం.pdpt_స్థానం, vspace);
}

స్థిర శూన్య unmapPDPT(asid_t asid, vptr_t vaddr, pdpte_t *pdpt)
{
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;
    pml4e_t *pml4_స్థానం;

    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి;
    }

    pml4_స్థానం = lookup_PML4_స్థానం(కనుగొను_ret.vspace_మూలం, vaddr);

    /* check if the PML4 has the PDPT */
    ఒకవేళ (!(pml4e_ptr_get_present(pml4_స్థానం) &&
          pml4e_ptr_get_pdpt_పునాది_చిరునామా(pml4_స్థానం) == pptr_గమ్యం_paddr(pdpt))) {
        తిరిగి;
    }

    flushPDPT(కనుగొను_ret.vspace_మూలం, vaddr, pdpt, asid);

    *pml4_స్థానం = makeUserPML4EInvalid();
}

స్థిర exception_t performX64PDPTInvocationUnmap(cap_t cap, cte_t *ct_స్థానం)
{
    ఒకవేళ (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        pdpte_t *pdpt = PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap));
        unmapPDPT(cap_pdpt_cap_get_capPDPTMappedASID(cap),
                  cap_pdpt_cap_get_cap_PDPTMapped_చిరునామా(cap),
                  pdpt);
        clear_జ్ఞాపకం((శూన్య *)pdpt, cap_get_cap_కొలత_బిట్లు(cap));
    }

    cap_pdpt_cap_ptr_సమితి_cap_PDPTIs_Mapped(&(ct_స్థానం->cap), 0);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t performX64PDPTInvocationMap(cap_t cap, cte_t *ct_స్థానం, pml4e_t pml4e, pml4e_t *pml4_స్థానం,
                                               vspace_మూలం_t *vspace)
{
    ct_స్థానం->cap = cap;
    *pml4_స్థానం = pml4e;
    invalidate_పుట_Structure_Cache_ASID(pptr_గమ్యం_paddr(vspace), cap_pdpt_cap_get_capPDPTMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeX64PDPTInvocation(
    మాట_t  label,
    మాట_t నిడివి,
    cte_t   *cte,
    cap_t   cap,
    మాట_t  *బఫరు)
{
    మాట_t                  vaddr;
    vm_లక్షణాలు_t         attr;
    pml4e_t                *pml4_స్థానం;
    cap_t                   vspaceCap;
    vspace_మూలం_t          *vspace;
    pml4e_t                 pml4e;
    paddr_t                 paddr;
    asid_t                  asid;

    ఒకవేళ (label == X86PDPTUnmap) {
        ఒకవేళ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            userError("X86PDPT: Cannot unmap if more than one cap exist.");
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);

        తిరిగి performX64PDPTInvocationUnmap(cap, cte);
    }

    ఒకవేళ (label != X86PDPTMap) {
        userError("X86PDPT: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("X64PDPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X64PDPT: PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, బఫరు) & (~ముసుగు(PML4_సూచీ_స్థానభ్రంశం));
    attr = vm_లక్షణాలు_From_మాట(getSyscallArg(1, బఫరు));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (!is_Valid_Native_మూలం(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_మూలం_t *)pptr_యొక్క_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    ఒకవేళ (vaddr > USER_శిఖరం) {
        userError("X64PDPT: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;

    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (కనుగొను_ret.vspace_మూలం != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    pml4_స్థానం = lookup_PML4_స్థానం(vspace, vaddr);

    ఒకవేళ (pml4e_ptr_get_present(pml4_స్థానం)) {
        current_syscall_error.type = se_L4_Delete_మొదటి;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_గమ్యం_paddr(PDPTE_PTR((cap_pdpt_cap_get_capPDPTBasePtr(cap))));
    pml4e = makeUserPML4E(paddr, attr);

    cap = cap_pdpt_cap_సమితి_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_pdpt_cap_సమితి_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_pdpt_cap_సమితి_cap_PDPTMapped_చిరునామా(cap, vaddr);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి performX64PDPTInvocationMap(cap, cte, pml4e, pml4_స్థానం, vspace);
}

exception_t decode_X86_బహుళకం_MMUInvocation(
    మాట_t label,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    మాట_t *బఫరు
)
{
    ఎంపిక (cap_get_capType(cap)) {

    స్థితి cap_pml4_cap:
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;

    స్థితి cap_pdpt_cap:
        తిరిగి decodeX64PDPTInvocation(label, నిడివి, cte, cap, బఫరు);

    స్థితి cap_పుట_directory_cap:
        తిరిగి decode_X64_పుట_Directory_Invocation(label, నిడివి, cte, cap, బఫరు);

    అప్రమేయ:
        fail("Invalid arch cap type");
    }
}

bool_t బహుళకం_Unmap_పుట(vm_పుట_కొలత_t పుట_కొలత, vspace_మూలం_t *vroot, vptr_t vaddr, శూన్య *pptr)
{
    ఒకవేళ (config_సమితి(CONFIG_HUGE_పుట) && పుట_కొలత == X64_Huge_పుట) {
        pdpte_t     *pdpte;
        lookupPDPTSlot_ret_t pdpt_ret = lookupPDPTSlot(vroot, vaddr);
        ఒకవేళ (pdpt_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి false;
        }
        pdpte = pdpt_ret.pdpt_స్థానం;


        ఒకవేళ (!(pdpte_ptr_get_పుట_కొలత(pdpte) == pdpte_pdpte_1g
              && pdpte_pdpte_1g_ptr_get_present(pdpte)
              && (pdpte_pdpte_1g_ptr_get_పుట_పునాది_చిరునామా(pdpte)
                  == pptr_గమ్యం_paddr(pptr)))) {
            తిరిగి false;
        }

        *pdpte = makeUserPDPTEInvalid();
        తిరిగి true;
    }
    fail("Invalid page type");
    తిరిగి false;
}

స్థిర exception_t updatePDPTE(asid_t asid, pdpte_t pdpte, pdpte_t *pdpt_స్థానం, vspace_మూలం_t *vspace)
{
    *pdpt_స్థానం = pdpte;
    invalidate_పుట_Structure_Cache_ASID(pptr_గమ్యం_paddr(vspace), asid,
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_X64_బహుళకం_Map(cap_t cap, cte_t *ct_స్థానం, pdpte_t pdpte, pdpte_t *pdpt_స్థానం, vspace_మూలం_t *vspace)
{
    ct_స్థానం->cap = cap;
    తిరిగి updatePDPTE(cap_చట్రం_cap_get_cap_FMapped_ASID(cap), pdpte, pdpt_స్థానం, vspace);
}

నిర్మాణం create_mapping_pdpte_return {
    exception_t స్థితి_2;
    pdpte_t pdpte;
    pdpte_t *pdpt_స్థానం;
};
రకనిర్వచనం నిర్మాణం create_mapping_pdpte_return create_mapping_pdpte_return_t;

స్థిర create_mapping_pdpte_return_t create_సురక్షిత_Mapping_Entries_PDPTE(paddr_t పునాది, మాట_t vaddr, vm_rights_t vmRights,
                                                                    vm_లక్షణాలు_t attr,
                                                                    vspace_మూలం_t *vspace)
{
    create_mapping_pdpte_return_t ret;
    lookupPDPTSlot_ret_t          lu_ret;

    lu_ret = lookupPDPTSlot(vspace, vaddr);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        తిరిగి ret;
    }
    ret.pdpt_స్థానం = lu_ret.pdpt_స్థానం;

    /* check for existing page directory */
    ఒకవేళ ((pdpte_ptr_get_పుట_కొలత(ret.pdpt_స్థానం) == pdpte_pdpte_pd) &&
        (pdpte_pdpte_pd_ptr_get_present(ret.pdpt_స్థానం))) {
        current_syscall_error.type = se_L4_Delete_మొదటి;
        ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        తిరిగి ret;
    }

    ret.pdpte = make_User_PDPTEHuge_పుట(పునాది, attr, vmRights);
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

exception_t decode_X86_బహుళకం_Map_పుట(మాట_t label, vm_పుట_కొలత_t పుట_కొలత, cte_t *cte, cap_t cap,
                                 vspace_మూలం_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_లక్షణాలు_t vm_attr)
{
    ఒకవేళ (config_సమితి(CONFIG_HUGE_పుట) && పుట_కొలత == X64_Huge_పుట) {
        create_mapping_pdpte_return_t map_ret;

        map_ret = create_సురక్షిత_Mapping_Entries_PDPTE(paddr, vaddr, vm_rights, vm_attr, vroot);
        ఒకవేళ (map_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి map_ret.స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);

        ఎంపిక (label) {
        స్థితి X86_పుట_Map:
            తిరిగి perform_X64_బహుళకం_Map(cap, cte, map_ret.pdpte, map_ret.pdpt_స్థానం, vroot);

        అప్రమేయ:
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
    }
    fail("Invalid Page type");
}

#నిర్వచితమైతే CONFIG_PRINTING
రకనిర్వచనం నిర్మాణం read_మాట_From_VSpace_ret {
    exception_t స్థితి_2;
    మాట_t విలువ;
} read_మాట_From_VSpace_ret_t;

స్థిర read_మాట_From_VSpace_ret_t read_మాట_From_VSpace(vspace_మూలం_t *vspace, మాట_t vaddr)
{
    read_మాట_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_స్థానం;
    lookupPDSlot_ret_t pd_స్థానం;
    lookupPDPTSlot_ret_t pdpt_స్థానం;
    paddr_t paddr;
    మాట_t స్థానభ్రంశం;
    pptr_t kernel_vaddr;
    మాట_t *విలువ;

    pdpt_స్థానం = lookupPDPTSlot(vspace, vaddr);
    ఒకవేళ (pdpt_స్థానం.స్థితి_2 == EXCEPTION_NONE &&
        pdpte_ptr_get_పుట_కొలత(pdpt_స్థానం.pdpt_స్థానం) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_స్థానం.pdpt_స్థానం)) {

        paddr = pdpte_pdpte_1g_ptr_get_పుట_పునాది_చిరునామా(pdpt_స్థానం.pdpt_స్థానం);
        స్థానభ్రంశం = vaddr & ముసుగు(se_L4_Huge_పుట_బిట్లు);
    } లేకపోతే {
        pd_స్థానం = lookupPDSlot(vspace, vaddr);
        ఒకవేళ (pd_స్థానం.స్థితి_2 == EXCEPTION_NONE &&
            ((pde_ptr_get_పుట_కొలత(pd_స్థానం.pd_స్థానం) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_స్థానం.pd_స్థానం))) {

            paddr = pde_pde_large_ptr_get_పుట_పునాది_చిరునామా(pd_స్థానం.pd_స్థానం);
            స్థానభ్రంశం = vaddr & ముసుగు(se_L4_Large_పుట_బిట్లు);
        } లేకపోతే {
            pt_స్థానం = lookupPTSlot(vspace, vaddr);
            ఒకవేళ (pt_స్థానం.స్థితి_2 == EXCEPTION_NONE && pte_ptr_get_present(pt_స్థానం.pt_స్థానం)) {
                paddr = pte_ptr_get_పుట_పునాది_చిరునామా(pt_స్థానం.pt_స్థానం);
                స్థానభ్రంశం = vaddr & ముసుగు(se_L4_పుట_బిట్లు);
            } లేకపోతే {
                ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
                తిరిగి ret;
            }
        }
    }


    kernel_vaddr = (మాట_t)paddr_గమ్యం_pptr(paddr);
    విలువ = (మాట_t *)(kernel_vaddr + స్థానభ్రంశం);
    ret.స్థితి_2 = EXCEPTION_NONE;
    ret.విలువ = *విలువ;
    తిరిగి ret;
}

శూన్య Arch_user_దొంతర_Trace(tcb_t *tptr)
{
    cap_t thread_మూలం;
    vspace_మూలం_t *vspace_మూలం;
    మాట_t sp;
    పూర్ణాంకం i;

    thread_మూలం = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    ఒకవేళ (cap_get_capType(thread_మూలం) != cap_pml4_cap) {
        రాయి("Invalid vspace\n");
        తిరిగి;
    }

    vspace_మూలం = (vspace_మూలం_t *)pptr_యొక్క_cap(thread_మూలం);

    sp = get_రిజిస్టరు(tptr, RSP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ఒకవేళ (!IS_ALIGNED(sp, se_L4_మాట_కొలత_బిట్లు)) {
        రాయి("RSP not aligned\n");
        తిరిగి;
    }

    కొరకు (i = 0; i < CONFIG_USER_దొంతర_TRACE_నిడివి; i++) {
        మాట_t చిరునామా = sp + (i * పరిమాణం(మాట_t));
        read_మాట_From_VSpace_ret_t ఫలితం;
        ఫలితం = read_మాట_From_VSpace(vspace_మూలం, చిరునామా);
        ఒకవేళ (ఫలితం.స్థితి_2 == EXCEPTION_NONE) {
            రాయి("0x%lx: 0x%lx\n", (దీర్ఘ)చిరునామా, (దీర్ఘ)ఫలితం.విలువ);
        } లేకపోతే {
            రాయి("0x%lx: INVALID\n", (దీర్ఘ)చిరునామా);
        }
    }
}
#అయితే_ముగింపు /* CONFIG_PRINTING */

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

    pde_t pde = pde_pde_large_new(
                    0,                 /* xd                   */
                    ks_User_Log_బఫరు,   /* page_base_address    */
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
#నిర్వచితమైతే CONFIG_HUGE_పుట
    x64KSKernelPD[1] = pde;
#లేకపోతే
    x64KSKernelPDs[బిట్టు(PDPT_సూచీ_బిట్లు) - 1][1] = pde;
#అయితే_ముగింపు
    invalidateTranslationAll(ముసుగు(CONFIG_MAX_అంకె_NODES));

    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు /* CONFIG_KERNEL_LOG_BUFFER */
