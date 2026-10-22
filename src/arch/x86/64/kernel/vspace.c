/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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
#ضمن <mode/kernel/tlb.h>
#ضمن <arch/kernel/tlb_bitmap.h>
#ضمن <object/structures.h>

/* When using the SKIM window to isolate the kernel from the user we also need to
 * not use global mappings as having global mappings and entries in the TLB is
 * equivalent, for the purpose of exploitation, to having the mappings in the
 * kernel window */
#تعریف_کړه KERNEL_IS_GLOBAL() (config_سېټ(CONFIG_KERNEL_SKIM_کړکۍ) ? 0 : 1)

/* For the boot code we create two windows into the physical address space
 * One is at the same location as the kernel window, and is placed up high
 * The other is a 1-to-1 mapping of the first 512gb of memory. The purpose
 * of this is to have a 1-to-1 mapping for the low parts of memory, so that
 * when we switch paging on, and are still running at physical addresses,
 * we don't explode. Then we also want the high mappings so we can start
 * running at proper kernel virtual addresses */
pml4e_t boot_pml4[بټ(PML4_شاخص_بټونه)] ALIGN(بټ(se_L4_پاڼه_بټونه)) VISIBLE PHYS_BSS;
pdpte_t boot_pdpt[بټ(PDPT_شاخص_بټونه)] ALIGN(بټ(se_L4_پاڼه_بټونه)) VISIBLE PHYS_BSS;

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

BOOT_کوډ bool_t map_kernel_کړکۍ(
    uint32_t عدد_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   عدد_drhu,
    paddr_t   *drhu_list
)
{

    uint64_t paddr;
    uint64_t vaddr;

#که_تعریف_شوی CONFIG_HUGE_پاڼه
    /* using 1 GiB page size */

    /* verify that the kernel window as at the last entry of the PML4 */
    أكد(GET_PML4_شاخص(PPTR_بنسټ) == بټ(PML4_شاخص_بټونه) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    أكد(GET_PML4_شاخص(KERNEL_ELF_بنسټ) == بټ(PML4_شاخص_بټونه) - 1);
    أكد(GET_PDPT_شاخص(KERNEL_ELF_بنسټ) == بټ(PML4_شاخص_بټونه) - 2);
    أكد(GET_PDPT_شاخص(KDEV_بنسټ) == بټ(PML4_شاخص_بټونه) - 1);
    أكد(IS_ALIGNED(KERNEL_ELF_بنسټ - KERNEL_ELF_PADDR_بنسټ, se_L4_Huge_پاڼه_بټونه));
    أكد(IS_ALIGNED(KDEV_بنسټ, se_L4_Huge_پاڼه_بټونه));
    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_شاخص(PPTR_بنسټ)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_منزل_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );
    /* put the 1GB kernel_base mapping into the PDPT */
    x64KSKernelPDPT[GET_PDPT_شاخص(KERNEL_ELF_بنسټ)] = pdpte_pdpte_1g_new(
                                                           0, /* xd */
                                                           PADDR_بنسټ,
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
    vaddr = PPTR_بنسټ;
    لكل (paddr = 0; paddr < PADDR_سر;
         paddr += بټ(se_L4_Huge_پاڼه_بټونه)) {

        صحيح pdpte_شاخص = GET_PDPT_شاخص(vaddr);
        x64KSKernelPDPT[pdpte_شاخص] = pdpte_pdpte_1g_new(
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

        vaddr += بټ(se_L4_Huge_پاڼه_بټونه);
    }

    /* put the PD into the PDPT */
    x64KSKernelPDPT[GET_PDPT_شاخص(KDEV_بنسټ)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_منزل_paddr(x64KSKernelPD),
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
                           kpptr_منزل_paddr(x64KSKernelPT),
                           0, /* accessed */
                           0, /* cache_disabled */
                           0, /* write_through */
                           0, /* super_user */
                           1, /* read_write */
                           1  /* present */
                       );
#وإلا

    صحيح pd_شاخص_2 = 0;
    /* use 2 MiB page size */
    /* verify that the kernel window as at the last entry of the PML4 */
    أكد(GET_PML4_شاخص(PPTR_بنسټ) == بټ(PML4_شاخص_بټونه) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    أكد(GET_PML4_شاخص(KERNEL_ELF_بنسټ) == بټ(PML4_شاخص_بټونه) - 1);
    أكد(GET_PDPT_شاخص(KERNEL_ELF_بنسټ) == بټ(PML4_شاخص_بټونه) - 2);
    أكد(GET_PDPT_شاخص(KDEV_بنسټ) == بټ(PML4_شاخص_بټونه) - 1);
    أكد(IS_ALIGNED(KERNEL_ELF_بنسټ - KERNEL_ELF_PADDR_بنسټ, se_L4_Huge_پاڼه_بټونه));
    أكد(IS_ALIGNED(KDEV_بنسټ, se_L4_Huge_پاڼه_بټونه));

    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_شاخص(PPTR_بنسټ)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_منزل_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    لكل (pd_شاخص_2 = 0; pd_شاخص_2 < PADDR_سر >> se_L4_Huge_پاڼه_بټونه; pd_شاخص_2++) {
        /* put the 1GB kernel_base mapping into the PDPT */
        x64KSKernelPDPT[GET_PDPT_شاخص(PPTR_بنسټ) + pd_شاخص_2] = pdpte_pdpte_pd_new(
                                                                    0, /* xd */
                                                                    kpptr_منزل_paddr(&x64KSKernelPDs[pd_شاخص_2][0]),
                                                                    0, /* accessed */
                                                                    0, /* cache disabled */
                                                                    0, /* write through */
                                                                    0, /* super user */
                                                                    1, /* read write */
                                                                    1 /* present */
                                                                );
    }

    x64KSKernelPDPT[GET_PDPT_شاخص(KERNEL_ELF_بنسټ)] = pdpte_pdpte_pd_new(
                                                           0, /* xd */
                                                           kpptr_منزل_paddr(&x64KSKernelPDs[0][0]),
                                                           0, /* accessed */
                                                           0, /* cache disable */
                                                           1, /* write through */
                                                           0, /* super user */
                                                           1, /* read write */
                                                           1  /* present */
                                                       );

    paddr = 0;
    vaddr = PPTR_بنسټ;

    لكل (paddr = 0; paddr < PADDR_سر;
         paddr += 0x200000) {

        صحيح pd_شاخص_2 = GET_PDPT_شاخص(vaddr) - GET_PDPT_شاخص(PPTR_بنسټ);
        صحيح pde_شاخص = GET_PD_شاخص(vaddr);

        x64KSKernelPDs[pd_شاخص_2][pde_شاخص] = pde_pde_large_new(
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
    x64KSKernelPDPT[GET_PDPT_شاخص(KDEV_بنسټ)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_منزل_paddr(&x64KSKernelPDs[بټ(PDPT_شاخص_بټونه) - 1][0]),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    /* put the PT into the PD */
    x64KSKernelPDs[بټ(PDPT_شاخص_بټونه) - 1][0] = pde_pde_pt_new(
                                                      0, /* xd */
                                                      kpptr_منزل_paddr(x64KSKernelPT),
                                                      0, /* accessed */
                                                      0, /* cache_disabled */
                                                      0, /* write_through */
                                                      0, /* super_user */
                                                      1, /* read_write */
                                                      1  /* present */
                                                  );
#د_که_پای

#إذا CONFIG_MAX_عدد_TRACE_POINTS > 0
    /* use the last PD entry as the benchmark log storage.
     * the actual backing physical memory will be filled
     * later by using alloc_region */
    ksLog = (ks_log_ثبت_t *)(KDEV_بنسټ + 0x200000 * (بټ(PD_شاخص_بټونه) - 1));
#د_که_پای

    /* now map in the kernel devices */
    إذا (!map_kernel_کړکۍ_devices(x64KSKernelPT, عدد_ioapic, ioapic_paddrs, عدد_drhu, drhu_list)) {
        أرجع false;
    }

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(x64KSKernelPML4);
#د_که_پای /* ENABLE_SMP_SUPPORT */

    /* In boot code, so fine to just trash everything here */
    invalidateLocalTranslationAll();
    چاپ_کړه("Mapping kernel window is done\n");
    أرجع true;
}

#که_تعریف_شوی CONFIG_KERNEL_SKIM_کړکۍ
BOOT_کوډ bool_t map_skim_کړکۍ(vptr_t skim_پیل, vptr_t skim_پای_ټکی)
{
    /* place the PDPT into the PML4 */
    x64KSSKIMPML4[GET_PML4_شاخص(PPTR_بنسټ)] = pml4e_new(
                                                   0, /* xd */
                                                   kpptr_منزل_paddr(x64KSSKIMPDPT),
                                                   0, /* accessed */
                                                   0, /* cache_disabled */
                                                   0, /* write_through */
                                                   0, /* super_user */
                                                   1, /* read_write */
                                                   1  /* present */
                                               );
    /* place the PD into the kernel_base slot of the PDPT */
    x64KSSKIMPDPT[GET_PDPT_شاخص(KERNEL_ELF_بنسټ)] = pdpte_pdpte_pd_new(
                                                         0, /* xd */
                                                         kpptr_منزل_paddr(x64KSSKIMPD),
                                                         0, /* accessed */
                                                         0, /* cache_disabled */
                                                         0, /* write_through */
                                                         0, /* super_user */
                                                         1, /* read_write */
                                                         1  /* present */
                                                     );
    /* map the skim portion into the PD. we expect it to be 2M aligned */
    أكد((skim_پیل % بټ(se_L4_Large_پاڼه_بټونه)) == 0);
    أكد((skim_پای_ټکی % بټ(se_L4_Large_پاڼه_بټونه)) == 0);
    uint64_t paddr = kpptr_منزل_paddr((فراغ *)skim_پیل);
    لكل (صحيح i = GET_PD_شاخص(skim_پیل); i < GET_PD_شاخص(skim_پای_ټکی); i++) {
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
        paddr += بټ(se_L4_Large_پاڼه_بټونه);
    }
    أرجع true;
}
#د_که_پای

BOOT_کوډ فراغ init_tss(tss_t *tss)
{
    کلمه_t بنسټ = (کلمه_t)&x64KSIRQStack[CURRENT_CPU_شاخص()][IRQ_ډېری_اندازه];
    *tss = tss_new(
               حجم(*tss),   /* io map base */
               0, 0,       /* ist 7 */
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               /* ist 1 is the stack frame we use for interrupts */
               بنسټ >> 32, بنسټ & 0xffffffff,  /* ist 1 */
               0, 0,       /* rsp 2 */
               0, 0,       /* rsp 1 */
               0, 0        /* rsp 0 */
           );
    /* set the IO map to all 1 to block user IN/OUT instructions */
    memset(&x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss.io_map[0], 0xff,
           حجم(x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss.io_map));
}

فراغ init_syscall_msrs(فراغ)
{
    x86_wrmsr(IA32_LSTAR_MSR, (uint64_t)&handle_fastsyscall);
    // mask bit 9 in the kernel (which is the interrupt enable bit)
    // also mask bit 8, which is the Trap Flag, to prevent the kernel
    // from single stepping
    x86_wrmsr(IA32_FMASK_MSR, FLAGS_TF | FLAGS_IF);
    x86_wrmsr(IA32_ستوری_MSR, ((uint64_t)SEL_CS_0 << 32) | ((uint64_t)SEL_CS_3 << 48));
}

BOOT_کوډ فراغ init_gdt(gdt_ثبت_t *gdt, tss_t *tss)
{

    uint64_t tss_بنسټ = (uint64_t)tss;
    gdt_tss_t gdt_tss;

    gdt[GDT_NULL] = gdt_ثبت_gdt_null_new();

    gdt[GDT_CS_0] = gdt_ثبت_gdt_کوډ_new(
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

    gdt[GDT_DS_0] = gdt_ثبت_gdt_معلومات_new(
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

    gdt[GDT_CS_3] = gdt_ثبت_gdt_کوډ_new(
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

    gdt[GDT_DS_3] = gdt_ثبت_gdt_معلومات_new(
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

    gdt[GDT_FS] = gdt_ثبت_gdt_معلومات_new(
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

    gdt[GDT_GS] = gdt_ثبت_gdt_معلومات_new(
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
                  tss_بنسټ >> 32,                     /* base 63 - 32 */
                  (tss_بنسټ & 0xff000000UL) >> 24,    /* base 31 - 24 */
                  1,                                  /* granularity */
                  0,                                  /* avl */
                  0,                                  /* limit high */
                  1,                                  /* present */
                  0,                                  /* dpl */
                  9,                                  /* desc type */
                  (tss_بنسټ & 0xff0000UL) >> 16,      /* base 23-16 */
                  (tss_بنسټ & 0xffffUL),              /* base 15 - 0 */
                  حجم(tss_io_t) - 1
              );

    gdt[GDT_TSS].ټکي[0] = gdt_tss.ټکي[0];
    gdt[GDT_TSS + 1].ټکي[0] = gdt_tss.ټکي[1];
}

BOOT_کوډ فراغ init_idt_ثبت(idt_ثبت_t *idt, مداخله_t مداخله, فراغ(*handler)(فراغ))
{
    uint64_t handler_addr = (uint64_t)handler;
    uint64_t dpl = 3;

    إذا (مداخله < int_trap_min && مداخله != int_software_break_request) {
        dpl = 0;
    }

    idt[مداخله] = idt_ثبت_مداخله_gate_new(
                         handler_addr >> 32,                 /* offset 63 - 32 */
                         ((handler_addr >> 16) & 0xffff),
                         1,                                  /* present */
                         dpl,                                /* dpl */
                         1,                                  /* ist */
                         SEL_CS_0,                           /* segment selector */
                         (handler_addr & 0xffff)               /* offset 15 - 0 */
                     );
}

فراغ سېټ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ریښه;
    asid_t asid;
    pml4e_t *pml4;
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;
    cr3_t cr3;

    thread_ریښه = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_ریښه) != cap_pml4_cap ||
        !cap_pml4_cap_get_capPML4IsMapped(thread_ریښه)) {
        سېټ_Current_User_VSpace_ریښه(kpptr_منزل_paddr(X86_GLOBAL_VSPACE_ریښه), 0);
        أرجع;
    }

    pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنسټ_Ptr(thread_ریښه));
    asid = cap_pml4_cap_get_capPML4MappedASID(thread_ریښه);
    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کړه_ret.حالت != EXCEPTION_NONE || پیدا_کړه_ret.vspace_ریښه != pml4)) {
        سېټ_Current_User_VSpace_ریښه(kpptr_منزل_paddr(X86_GLOBAL_VSPACE_ریښه), 0);
        أرجع;
    }
    cr3 = makeCR3(pptr_منزل_paddr(pml4), asid);
    إذا (getCurrentUserCR3().ټکي[0] != cr3.ټکي[0]) {
        SMP_COND_STATEMENT(tlb_bitmap_سېټ(pml4, getCurrentCPUIndex());)
        سېټ_Current_User_CR3(cr3);
    }
}


BOOT_کوډ فراغ init_dtrs(فراغ)
{
    gdt_idt_ptr.حد = (حجم(gdt_ثبت_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.بنسټ = (uint64_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSgdt;

    /* When we install the gdt it will clobber any value of gs that
     * we have. Since we might be using it for TLS we can stash
     * and unstash any gs value using swapgs
     */
    swapgs();
    x64_install_gdt(&gdt_idt_ptr);
    swapgs();

    gdt_idt_ptr.حد = (حجم(idt_ثبت_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.بنسټ = (uint64_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSidt;
    x64_install_idt(&gdt_idt_ptr);

    x64_install_ldt(SEL_NULL);

    x64_install_tss(SEL_TSS);
}

BOOT_کوډ فراغ map_it_چوکاټ_cap(cap_t pd_cap, cap_t چوکاټ_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_د_cap(pd_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt;
    vptr_t vptr = cap_چوکاټ_cap_get_cap_FMapped_پته(چوکاټ_cap);
    فراغ *pptr = (فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(چوکاټ_cap);

    أكد(cap_چوکاټ_cap_get_cap_FMap_Type(چوکاټ_cap) == X86_MappingVSpace);
    أكد(cap_چوکاټ_cap_get_cap_FMapped_ASID(چوکاټ_cap) != asidInvalid);
    pml4 += GET_PML4_شاخص(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنسټ_پته(pml4));
    pdpt += GET_PDPT_شاخص(vptr);
    أكد(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_بنسټ_پته(pdpt));
    pd += GET_PD_شاخص(vptr);
    أكد(pde_pde_pt_ptr_get_present(pd));
    pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_بنسټ_پته(pd));
    *(pt + GET_PT_شاخص(vptr)) = pte_new(
                                     0,                      /* xd                   */
                                     pptr_منزل_paddr(pptr),    /* page_base_address    */
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

ساكن BOOT_کوډ فراغ map_it_pdpt_cap(cap_t vspace_cap, cap_t pdpt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_د_cap(vspace_cap));
    pdpte_t *pdpt = PDPT_PTR(cap_pdpt_cap_get_capPDPTBasePtr(pdpt_cap));
    vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_پته(pdpt_cap);

    أكد(cap_pdpt_cap_get_capPDPTIsMapped(pdpt_cap));
    *(pml4 + GET_PML4_شاخص(vptr)) = pml4e_new(
                                         0,                      /* xd                   */
                                         pptr_منزل_paddr(pdpt),    /* pdpt_base_address    */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_کوډ فراغ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_د_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd = PD_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    vptr_t vptr = cap_پاڼه_directory_cap_get_cap_PDMapped_پته(pd_cap);

    أكد(cap_پاڼه_directory_cap_get_cap_PDIs_Mapped(pd_cap));
    pml4 += GET_PML4_شاخص(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنسټ_پته(pml4));
    *(pdpt + GET_PDPT_شاخص(vptr)) = pdpte_pdpte_pd_new(
                                         0,                      /* xd                   */
                                         pptr_منزل_paddr(pd),      /* pd_base_address      */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_کوډ فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_د_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt = PT_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(pt_cap);

    أكد(cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(pt_cap));
    pml4 += GET_PML4_شاخص(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنسټ_پته(pml4));
    pdpt += GET_PDPT_شاخص(vptr);
    أكد(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_بنسټ_پته(pdpt));
    *(pd + GET_PD_شاخص(vptr)) = pde_pde_pt_new(
                                     0,                      /* xd                   */
                                     pptr_منزل_paddr(pt),      /* pt_base_address      */
                                     0,                      /* accessed             */
                                     0,                      /* cache_disabled       */
                                     0,                      /* write_through        */
                                     1,                      /* super_user           */
                                     1,                      /* read_write           */
                                     1                       /* present              */
                                 );
}

BOOT_کوډ فراغ *map_لنډمهاله_boot_پاڼه(فراغ *ثبت, uint32_t large_پاڼې)
{
    /* this function is for legacy 32-bit systems where the ACPI tables might
     * collide with the kernel window. Here we just assert that the table is
     * in fact in the lower 4GiB region (which is already 1-to-1 mapped) and
     * continue */
    أكد((کلمه_t)ثبت < بټ(32));
    أرجع ثبت;
}

ساكن BOOT_کوډ cap_t create_it_pdpt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_pdpt_cap_new(
              asid,   /* capPDPTMappedASID    */
              pptr,   /* capPDPTBasePtr       */
              1,      /* capPDPTIsMapped      */
              vptr    /* capPDPTMappedAddress */
          );
    map_it_pdpt_cap(vspace_cap, cap);
    أرجع cap;
}

ساكن BOOT_کوډ cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_پاڼه_directory_cap_new(
              asid,   /* capPDMappedASID      */
              pptr,   /* capPDBasePtr         */
              1,      /* capPDIsMapped        */
              vptr    /* capPDMappedAddress   */
          );
    map_it_pd_cap(vspace_cap, cap);
    أرجع cap;
}

ساكن BOOT_کوډ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_پاڼه_جدول_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );
    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}


BOOT_کوډ کلمه_t arch_get_n_paging(v_region_t it_v_reg)
{
    کلمه_t n = get_n_paging(it_v_reg, PD_شاخص_دننه_واټن);
    n += get_n_paging(it_v_reg, PDPT_شاخص_دننه_واټن);
    n += get_n_paging(it_v_reg, PML4_شاخص_دننه_واټن);
#که_تعریف_شوی CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#د_که_پای
    أرجع n;
}

BOOT_کوډ cap_t create_it_پته_space(cap_t ریښه_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_خونه_Pos خونه_pos_before;
    se_L4_خونه_Pos خونه_pos_after;

    خونه_pos_before = ndks_boot.خونه_pos_cur;
    copyGlobalMappings(PML4_PTR(rootserver.vspace));
    vspace_cap = cap_pml4_cap_new(
                     IT_ASID,        /* capPML4MappedASID */
                     rootserver.vspace,           /* capPML4BasePtr   */
                     1               /* capPML4IsMapped   */
                 );


    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

    /* Create any PDPTs needed for the user land image */
    لكل (vptr = پړاو_ښکته(it_v_reg.پیل, PML4_شاخص_دننه_واټن);
         vptr < it_v_reg.پای_ټکی;
         vptr += بټ(PML4_شاخص_دننه_واټن)) {
        إذا (!provide_cap(ریښه_cnode_cap,
                         create_it_pdpt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    /* Create any PDs needed for the user land image */
    لكل (vptr = پړاو_ښکته(it_v_reg.پیل, PDPT_شاخص_دننه_واټن);
         vptr < it_v_reg.پای_ټکی;
         vptr += بټ(PDPT_شاخص_دننه_واټن)) {
        إذا (!provide_cap(ریښه_cnode_cap,
                         create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    لكل (vptr = پړاو_ښکته(it_v_reg.پیل, PD_شاخص_دننه_واټن);
         vptr < it_v_reg.پای_ټکی;
         vptr += بټ(PD_شاخص_دننه_واټن)) {
        إذا (!provide_cap(ریښه_cnode_cap,
                         create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
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

فراغ copyGlobalMappings(vspace_ریښه_t *new_vspace)
{
    بینښه اوږد i;
    pml4e_t *vspace = (pml4e_t *)new_vspace;

    /* Copy from the tlbbitmap_pptr so that we copy the default entries of the
     * tlb bitmap (if it exists). If it doesn't exist then this loop
     * will be equivalent to copying from PPTR_BASE
     */
    لكل (i = GET_PML4_شاخص(TLBBITMAP_PPTR); i < بټ(PML4_شاخص_بټونه); i++) {
        vspace[i] = X86_GLOBAL_VSPACE_ریښه[i];
    }
}

ساكن BOOT_کوډ cap_t create_it_چوکاټ_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large, se_L4_کلمه map_type)
{
    vm_پاڼه_اندازه_t چوکاټ_اندازه_2;

    إذا (use_large) {
        چوکاټ_اندازه_2 = X86_Large_پاڼه;
    } وإلا {
        چوکاټ_اندازه_2 = X86_Small_پاڼه;
    }

    أرجع
        cap_چوکاټ_cap_new(
            asid,                          /* capFMappedASID     */
            pptr,                          /* capFBasePtr        */
            چوکاټ_اندازه_2,                    /* capFSize           */
            map_type,                      /* capFMapType        */
            vptr,                          /* capFMappedAddress  */
            کلمه_From_VMRights(VMReadWrite), /* capFVMRights       */
            0                              /* capFIsDevice       */
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

/* ====================== BOOT CODE FINISHES HERE ======================== */



exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_خونه)
{
    asid_map_t asid_map;
#که_تعریف_شوی CONFIG_VTX
    إذا (cap_get_capType(vspace_Cap_خونه->cap) == cap_ept_pml4_cap) {
        cap_ept_pml4_cap_ptr_سېټ_cap_PML4_Mapped_ASID(&vspace_Cap_خونه->cap, asid);
        cap_ept_pml4_cap_ptr_سېټ_cap_PML4_Is_Mapped(&vspace_Cap_خونه->cap, 1);
        asid_map = asid_map_asid_map_ept_new(cap_ept_pml4_cap_get_cap_PML4_بنسټ_Ptr(vspace_Cap_خونه->cap));
    } وإلا
#د_که_پای
    {
        أكد(cap_get_capType(vspace_Cap_خونه->cap) == cap_pml4_cap);
        cap_pml4_cap_ptr_سېټ_cap_PML4_Mapped_ASID(&vspace_Cap_خونه->cap, asid);
        cap_pml4_cap_ptr_سېټ_cap_PML4_Is_Mapped(&vspace_Cap_خونه->cap, 1);
        asid_map = asid_map_asid_map_vspace_new(cap_pml4_cap_get_cap_PML4_بنسټ_Ptr(vspace_Cap_خونه->cap));
    }
    poolPtr->array[ASID_ټیټ(asid)] = asid_map;
    أرجع EXCEPTION_NONE;
}

bool_t CONST is_VTable_ریښه(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_pml4_cap;
}

bool_t CONST is_Valid_Native_ریښه(cap_t cap)
{
    أرجع is_VTable_ریښه(cap) &&
           cap_pml4_cap_get_capPML4IsMapped(cap);
}

ساكن pml4e_t CONST makeUserPML4E(paddr_t paddr, vm_ځانګړتیاوې_t vm_attr)
{
    أرجع pml4e_new(
               0,
               paddr,
               0,
               vm_ځانګړتیاوې_get_x86_PCDBit(vm_attr),
               vm_ځانګړتیاوې_get_x86_PWTBit(vm_attr),
               1,
               1,
               1
           );
}

ساكن pml4e_t CONST makeUserPML4EInvalid(فراغ)
{
    أرجع pml4e_new(
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

ساكن pdpte_t CONST make_User_PDPTEHuge_پاڼه(paddr_t paddr, vm_ځانګړتیاوې_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pdpte_pdpte_1g_new(
               0,          /* xd               */
               paddr,      /* physical address */
               0,          /* PAT              */
               0,          /* global           */
               0,          /* dirty            */
               0,          /* accessed         */
               vm_ځانګړتیاوې_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_ځانګړتیاوې_get_x86_PWTBit(vm_attr),  /* write through  */
               SuperUserFromVMRights(vm_rights),       /* super user     */
               WritableFromVMRights(vm_rights),        /* read write     */
               1                                       /* present        */
           );
}

ساكن pdpte_t CONST makeUserPDPTEPageDirectory(paddr_t paddr, vm_ځانګړتیاوې_t vm_attr)
{
    أرجع pdpte_pdpte_pd_new(
               0,                      /* xd       */
               paddr,                  /* paddr    */
               0,                      /* accessed */
               vm_ځانګړتیاوې_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_ځانګړتیاوې_get_x86_PWTBit(vm_attr),  /* write through  */
               1,                      /* super user */
               1,                      /* read write */
               1                       /* present    */
           );
}

ساكن pdpte_t CONST makeUserPDPTEInvalid(فراغ)
{
    أرجع pdpte_pdpte_pd_new(
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

pde_t CONST make_User_PDELarge_پاڼه(paddr_t paddr, vm_ځانګړتیاوې_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pde_pde_large_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               vm_ځانګړتیاوې_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ځانګړتیاوې_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_ځانګړتیاوې_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_جدول(paddr_t paddr, vm_ځانګړتیاوې_t vm_attr)
{

    أرجع  pde_pde_pt_new(
                0,                                  /* xd               */
                paddr,                              /* pt_base_address  */
                0,                                  /* accessed         */
                vm_ځانګړتیاوې_get_x86_PCDBit(vm_attr), /* cache_disabled   */
                vm_ځانګړتیاوې_get_x86_PWTBit(vm_attr), /* write_through    */
                1,                                  /* super_user       */
                1,                                  /* read_write       */
                1                                   /* present          */
            );
}

pde_t CONST makeUserPDEInvalid(فراغ)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    أرجع pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_ځانګړتیاوې_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pte_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               0,                                              /* global               */
               vm_ځانګړتیاوې_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ځانګړتیاوې_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_ځانګړتیاوې_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pte_t CONST makeUserPTEInvalid(فراغ)
{
    أرجع pte_new(
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


ساكن pml4e_t *lookup_PML4_خونه(vspace_ریښه_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4e = PML4E_PTR(pml4);
    کلمه_t pml4_شاخص = GET_PML4_شاخص(vptr);
    أرجع pml4e + pml4_شاخص;
}

ساكن lookupPDPTSlot_ret_t lookupPDPTSlot(vspace_ریښه_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4_خونه = lookup_PML4_خونه(pml4, vptr);
    lookupPDPTSlot_ret_t ret;

    إذا (!pml4e_ptr_get_present(pml4_خونه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PML4_شاخص_دننه_واټن);

        ret.pdpt_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pdpte_t *pdpt;
        pdpte_t *pdpt_خونه;
        کلمه_t pdpt_شاخص = GET_PDPT_شاخص(vptr);
        pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنسټ_پته(pml4_خونه));
        pdpt_خونه = pdpt + pdpt_شاخص;

        ret.حالت = EXCEPTION_NONE;
        ret.pdpt_خونه = pdpt_خونه;
        أرجع ret;
    }
}

lookupPDSlot_ret_t lookupPDSlot(vspace_ریښه_t *pml4, vptr_t vptr)
{
    lookupPDPTSlot_ret_t pdpt_خونه;
    lookupPDSlot_ret_t ret;

    pdpt_خونه = lookupPDPTSlot(pml4, vptr);

    إذا (pdpt_خونه.حالت != EXCEPTION_NONE) {
        ret.pd_خونه = عدم;
        ret.حالت = pdpt_خونه.حالت;
        أرجع ret;
    }
    إذا ((pdpte_ptr_get_پاڼه_اندازه(pdpt_خونه.pdpt_خونه) != pdpte_pdpte_pd) ||
        !pdpte_pdpte_pd_ptr_get_present(pdpt_خونه.pdpt_خونه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PDPT_شاخص_دننه_واټن);

        ret.pd_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pde_t *pd;
        pde_t *pd_خونه;
        کلمه_t pd_شاخص = GET_PD_شاخص(vptr);
        pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_بنسټ_پته(pdpt_خونه.pdpt_خونه));
        pd_خونه = pd + pd_شاخص;

        ret.حالت = EXCEPTION_NONE;
        ret.pd_خونه = pd_خونه;
        أرجع ret;
    }
}

ساكن فراغ flushPD(vspace_ریښه_t *vspace, کلمه_t vptr, pde_t *pd, asid_t asid)
{
    /* clearing the entire PCID vs flushing the virtual addresses
     * one by one using invplg.
     * choose the easy way, invalidate the PCID
     */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));

}

ساكن فراغ flushPDPT(vspace_ریښه_t *vspace, کلمه_t vptr, pdpte_t *pdpt, asid_t asid)
{
    /* similar here */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع;
}

فراغ hwASIDInvalidate(asid_t asid, vspace_ریښه_t *vspace)
{
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
}

فراغ unmap_پاڼه_Directory(asid_t asid, vptr_t vaddr, pde_t *pd)
{
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;
    lookupPDPTSlot_ret_t    lu_ret;

    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPDPTSlot(پیدا_کړه_ret.vspace_ریښه, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    /* check if the PDPT has the PD */
    إذا (!(pdpte_ptr_get_پاڼه_اندازه(lu_ret.pdpt_خونه) == pdpte_pdpte_pd &&
          pdpte_pdpte_pd_ptr_get_present(lu_ret.pdpt_خونه) &&
          (pdpte_pdpte_pd_ptr_get_pd_بنسټ_پته(lu_ret.pdpt_خونه) == pptr_منزل_paddr(pd)))) {
        أرجع;
    }

    flushPD(پیدا_کړه_ret.vspace_ریښه, vaddr, pd, asid);

    *lu_ret.pdpt_خونه = makeUserPDPTEInvalid();

    invalidate_پاڼه_Structure_Cache_ASID(pptr_منزل_paddr(پیدا_کړه_ret.vspace_ریښه), asid,
                                     SMP_TERNARY(tlb_bitmap_get(پیدا_کړه_ret.vspace_ریښه), 0));
}


ساكن exception_t perform_X64_پاڼه_Directory_Invocation_Unmap(cap_t cap, cte_t *ct_خونه)
{

    إذا (cap_پاڼه_directory_cap_get_cap_PDIs_Mapped(cap)) {
        pde_t *pd = PDE_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(cap));
        unmap_پاڼه_Directory(
            cap_پاڼه_directory_cap_get_cap_PDMapped_ASID(cap),
            cap_پاڼه_directory_cap_get_cap_PDMapped_پته(cap),
            pd
        );
        clear_حافظه((فراغ *)pd, cap_get_cap_اندازه_بټونه(cap));
    }

    cap_پاڼه_directory_cap_ptr_سېټ_cap_PDIs_Mapped(&(ct_خونه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X64_پاڼه_Directory_Invocation_Map(cap_t cap, cte_t *ct_خونه, pdpte_t pdpte, pdpte_t *pdpt_خونه,
                                                        vspace_ریښه_t *vspace)
{
    ct_خونه->cap = cap;
    *pdpt_خونه = pdpte;
    invalidate_پاڼه_Structure_Cache_ASID(pptr_منزل_paddr(vspace), cap_پاڼه_directory_cap_get_cap_PDMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}


ساكن exception_t decode_X64_پاڼه_Directory_Invocation(
    کلمه_t label,
    کلمه_t اوږدتیا,
    cte_t *cte,
    cap_t cap,
    کلمه_t *بفر
)
{
    کلمه_t              vaddr;
    vm_ځانګړتیاوې_t     vm_attr;
    cap_t               vspaceCap;
    vspace_ریښه_t      *vspace;
    pdpte_t             pdpte;
    paddr_t             paddr;
    asid_t              asid;
    lookupPDPTSlot_ret_t pdpt_خونه;

    إذا (label == X86_پاڼه_Directory_Unmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            userError("X86PageDirectory: Cannot unmap if more than one cap exist.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);

        أرجع perform_X64_پاڼه_Directory_Invocation_Unmap(cap, cte);
    }

    إذا (label != X86_پاڼه_Directory_Map) {
        userError("X64Directory: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X64PageDirectory: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_پاڼه_directory_cap_get_cap_PDIs_Mapped(cap)) {
        userError("X64PageDirectory: PD is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر) & (~ماسک(PDPT_شاخص_دننه_واټن));
    vm_attr = vm_ځانګړتیاوې_From_کلمه(getSyscallArg(1, بفر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    إذا (!is_Valid_Native_ریښه(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ریښه_t *)pptr_د_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    إذا (vaddr > USER_سر) {
        userError("X64PageDirectory: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;

    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کړه_ret.vspace_ریښه != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pdpt_خونه = lookupPDPTSlot(vspace, vaddr);
    إذا (pdpt_خونه.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا ((pdpte_ptr_get_پاڼه_اندازه(pdpt_خونه.pdpt_خونه) == pdpte_pdpte_pd &&
         pdpte_pdpte_pd_ptr_get_present(pdpt_خونه.pdpt_خونه)) ||
        (pdpte_ptr_get_پاڼه_اندازه(pdpt_خونه.pdpt_خونه) == pdpte_pdpte_1g
         && pdpte_pdpte_1g_ptr_get_present(pdpt_خونه.pdpt_خونه))) {
        current_syscall_error.type = se_L4_Delete_لومړی;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr(PDE_PTR(cap_پاڼه_directory_cap_get_cap_PDBase_Ptr(cap)));
    pdpte = makeUserPDPTEPageDirectory(paddr, vm_attr);

    cap = cap_پاڼه_directory_cap_سېټ_cap_PDIs_Mapped(cap, 1);
    cap = cap_پاڼه_directory_cap_سېټ_cap_PDMapped_ASID(cap, asid);
    cap = cap_پاڼه_directory_cap_سېټ_cap_PDMapped_پته(cap, vaddr);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_X64_پاڼه_Directory_Invocation_Map(cap, cte, pdpte, pdpt_خونه.pdpt_خونه, vspace);
}

ساكن فراغ unmapPDPT(asid_t asid, vptr_t vaddr, pdpte_t *pdpt)
{
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;
    pml4e_t *pml4_خونه;

    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    pml4_خونه = lookup_PML4_خونه(پیدا_کړه_ret.vspace_ریښه, vaddr);

    /* check if the PML4 has the PDPT */
    إذا (!(pml4e_ptr_get_present(pml4_خونه) &&
          pml4e_ptr_get_pdpt_بنسټ_پته(pml4_خونه) == pptr_منزل_paddr(pdpt))) {
        أرجع;
    }

    flushPDPT(پیدا_کړه_ret.vspace_ریښه, vaddr, pdpt, asid);

    *pml4_خونه = makeUserPML4EInvalid();
}

ساكن exception_t performX64PDPTInvocationUnmap(cap_t cap, cte_t *ct_خونه)
{
    إذا (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        pdpte_t *pdpt = PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap));
        unmapPDPT(cap_pdpt_cap_get_capPDPTMappedASID(cap),
                  cap_pdpt_cap_get_cap_PDPTMapped_پته(cap),
                  pdpt);
        clear_حافظه((فراغ *)pdpt, cap_get_cap_اندازه_بټونه(cap));
    }

    cap_pdpt_cap_ptr_سېټ_cap_PDPTIs_Mapped(&(ct_خونه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performX64PDPTInvocationMap(cap_t cap, cte_t *ct_خونه, pml4e_t pml4e, pml4e_t *pml4_خونه,
                                               vspace_ریښه_t *vspace)
{
    ct_خونه->cap = cap;
    *pml4_خونه = pml4e;
    invalidate_پاڼه_Structure_Cache_ASID(pptr_منزل_paddr(vspace), cap_pdpt_cap_get_capPDPTMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeX64PDPTInvocation(
    کلمه_t  label,
    کلمه_t اوږدتیا,
    cte_t   *cte,
    cap_t   cap,
    کلمه_t  *بفر)
{
    کلمه_t                  vaddr;
    vm_ځانګړتیاوې_t         attr;
    pml4e_t                *pml4_خونه;
    cap_t                   vspaceCap;
    vspace_ریښه_t          *vspace;
    pml4e_t                 pml4e;
    paddr_t                 paddr;
    asid_t                  asid;

    إذا (label == X86PDPTUnmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            userError("X86PDPT: Cannot unmap if more than one cap exist.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);

        أرجع performX64PDPTInvocationUnmap(cap, cte);
    }

    إذا (label != X86PDPTMap) {
        userError("X86PDPT: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X64PDPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X64PDPT: PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر) & (~ماسک(PML4_شاخص_دننه_واټن));
    attr = vm_ځانګړتیاوې_From_کلمه(getSyscallArg(1, بفر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    إذا (!is_Valid_Native_ریښه(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ریښه_t *)pptr_د_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    إذا (vaddr > USER_سر) {
        userError("X64PDPT: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;

    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کړه_ret.vspace_ریښه != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4_خونه = lookup_PML4_خونه(vspace, vaddr);

    إذا (pml4e_ptr_get_present(pml4_خونه)) {
        current_syscall_error.type = se_L4_Delete_لومړی;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr(PDPTE_PTR((cap_pdpt_cap_get_capPDPTBasePtr(cap))));
    pml4e = makeUserPML4E(paddr, attr);

    cap = cap_pdpt_cap_سېټ_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_pdpt_cap_سېټ_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_pdpt_cap_سېټ_cap_PDPTMapped_پته(cap, vaddr);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع performX64PDPTInvocationMap(cap, cte, pml4e, pml4_خونه, vspace);
}

exception_t decode_X86_ډېرپېښ_MMUInvocation(
    کلمه_t label,
    کلمه_t اوږدتیا,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    کلمه_t *بفر
)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_pml4_cap:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_pdpt_cap:
        أرجع decodeX64PDPTInvocation(label, اوږدتیا, cte, cap, بفر);

    حالة cap_پاڼه_directory_cap:
        أرجع decode_X64_پاڼه_Directory_Invocation(label, اوږدتیا, cte, cap, بفر);

    افتراضي:
        fail("Invalid arch cap type");
    }
}

bool_t ډېرپېښ_Unmap_پاڼه(vm_پاڼه_اندازه_t پاڼه_اندازه, vspace_ریښه_t *vroot, vptr_t vaddr, فراغ *pptr)
{
    إذا (config_سېټ(CONFIG_HUGE_پاڼه) && پاڼه_اندازه == X64_Huge_پاڼه) {
        pdpte_t     *pdpte;
        lookupPDPTSlot_ret_t pdpt_ret = lookupPDPTSlot(vroot, vaddr);
        إذا (pdpt_ret.حالت != EXCEPTION_NONE) {
            أرجع false;
        }
        pdpte = pdpt_ret.pdpt_خونه;


        إذا (!(pdpte_ptr_get_پاڼه_اندازه(pdpte) == pdpte_pdpte_1g
              && pdpte_pdpte_1g_ptr_get_present(pdpte)
              && (pdpte_pdpte_1g_ptr_get_پاڼه_بنسټ_پته(pdpte)
                  == pptr_منزل_paddr(pptr)))) {
            أرجع false;
        }

        *pdpte = makeUserPDPTEInvalid();
        أرجع true;
    }
    fail("Invalid page type");
    أرجع false;
}

ساكن exception_t updatePDPTE(asid_t asid, pdpte_t pdpte, pdpte_t *pdpt_خونه, vspace_ریښه_t *vspace)
{
    *pdpt_خونه = pdpte;
    invalidate_پاڼه_Structure_Cache_ASID(pptr_منزل_paddr(vspace), asid,
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X64_ډېرپېښ_Map(cap_t cap, cte_t *ct_خونه, pdpte_t pdpte, pdpte_t *pdpt_خونه, vspace_ریښه_t *vspace)
{
    ct_خونه->cap = cap;
    أرجع updatePDPTE(cap_چوکاټ_cap_get_cap_FMapped_ASID(cap), pdpte, pdpt_خونه, vspace);
}

بنية create_mapping_pdpte_return {
    exception_t حالت;
    pdpte_t pdpte;
    pdpte_t *pdpt_خونه;
};
تعريفنوع بنية create_mapping_pdpte_return create_mapping_pdpte_return_t;

ساكن create_mapping_pdpte_return_t create_خوندي_Mapping_Entries_PDPTE(paddr_t بنسټ, کلمه_t vaddr, vm_rights_t vmRights,
                                                                    vm_ځانګړتیاوې_t attr,
                                                                    vspace_ریښه_t *vspace)
{
    create_mapping_pdpte_return_t ret;
    lookupPDPTSlot_ret_t          lu_ret;

    lu_ret = lookupPDPTSlot(vspace, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        أرجع ret;
    }
    ret.pdpt_خونه = lu_ret.pdpt_خونه;

    /* check for existing page directory */
    إذا ((pdpte_ptr_get_پاڼه_اندازه(ret.pdpt_خونه) == pdpte_pdpte_pd) &&
        (pdpte_pdpte_pd_ptr_get_present(ret.pdpt_خونه))) {
        current_syscall_error.type = se_L4_Delete_لومړی;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    ret.pdpte = make_User_PDPTEHuge_پاڼه(بنسټ, attr, vmRights);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

exception_t decode_X86_ډېرپېښ_Map_پاڼه(کلمه_t label, vm_پاڼه_اندازه_t پاڼه_اندازه, cte_t *cte, cap_t cap,
                                 vspace_ریښه_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_ځانګړتیاوې_t vm_attr)
{
    إذا (config_سېټ(CONFIG_HUGE_پاڼه) && پاڼه_اندازه == X64_Huge_پاڼه) {
        create_mapping_pdpte_return_t map_ret;

        map_ret = create_خوندي_Mapping_Entries_PDPTE(paddr, vaddr, vm_rights, vm_attr, vroot);
        إذا (map_ret.حالت != EXCEPTION_NONE) {
            أرجع map_ret.حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);

        بدل (label) {
        حالة X86_پاڼه_Map:
            أرجع perform_X64_ډېرپېښ_Map(cap, cte, map_ret.pdpte, map_ret.pdpt_خونه, vroot);

        افتراضي:
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }
    fail("Invalid Page type");
}

#که_تعریف_شوی CONFIG_PRINTING
تعريفنوع بنية read_کلمه_From_VSpace_ret {
    exception_t حالت;
    کلمه_t ارزښت;
} read_کلمه_From_VSpace_ret_t;

ساكن read_کلمه_From_VSpace_ret_t read_کلمه_From_VSpace(vspace_ریښه_t *vspace, کلمه_t vaddr)
{
    read_کلمه_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_خونه;
    lookupPDSlot_ret_t pd_خونه;
    lookupPDPTSlot_ret_t pdpt_خونه;
    paddr_t paddr;
    کلمه_t دننه_واټن;
    pptr_t kernel_vaddr;
    کلمه_t *ارزښت;

    pdpt_خونه = lookupPDPTSlot(vspace, vaddr);
    إذا (pdpt_خونه.حالت == EXCEPTION_NONE &&
        pdpte_ptr_get_پاڼه_اندازه(pdpt_خونه.pdpt_خونه) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_خونه.pdpt_خونه)) {

        paddr = pdpte_pdpte_1g_ptr_get_پاڼه_بنسټ_پته(pdpt_خونه.pdpt_خونه);
        دننه_واټن = vaddr & ماسک(se_L4_Huge_پاڼه_بټونه);
    } وإلا {
        pd_خونه = lookupPDSlot(vspace, vaddr);
        إذا (pd_خونه.حالت == EXCEPTION_NONE &&
            ((pde_ptr_get_پاڼه_اندازه(pd_خونه.pd_خونه) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_خونه.pd_خونه))) {

            paddr = pde_pde_large_ptr_get_پاڼه_بنسټ_پته(pd_خونه.pd_خونه);
            دننه_واټن = vaddr & ماسک(se_L4_Large_پاڼه_بټونه);
        } وإلا {
            pt_خونه = lookupPTSlot(vspace, vaddr);
            إذا (pt_خونه.حالت == EXCEPTION_NONE && pte_ptr_get_present(pt_خونه.pt_خونه)) {
                paddr = pte_ptr_get_پاڼه_بنسټ_پته(pt_خونه.pt_خونه);
                دننه_واټن = vaddr & ماسک(se_L4_پاڼه_بټونه);
            } وإلا {
                ret.حالت = EXCEPTION_LOOKUP_FAULT;
                أرجع ret;
            }
        }
    }


    kernel_vaddr = (کلمه_t)paddr_منزل_pptr(paddr);
    ارزښت = (کلمه_t *)(kernel_vaddr + دننه_واټن);
    ret.حالت = EXCEPTION_NONE;
    ret.ارزښت = *ارزښت;
    أرجع ret;
}

فراغ Arch_user_ډېری_Trace(tcb_t *tptr)
{
    cap_t thread_ریښه;
    vspace_ریښه_t *vspace_ریښه;
    کلمه_t sp;
    صحيح i;

    thread_ریښه = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    إذا (cap_get_capType(thread_ریښه) != cap_pml4_cap) {
        چاپ_کړه("Invalid vspace\n");
        أرجع;
    }

    vspace_ریښه = (vspace_ریښه_t *)pptr_د_cap(thread_ریښه);

    sp = get_ثبتوونکی(tptr, RSP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_کلمه_اندازه_بټونه)) {
        چاپ_کړه("RSP not aligned\n");
        أرجع;
    }

    لكل (i = 0; i < CONFIG_USER_ډېری_TRACE_اوږدتیا; i++) {
        کلمه_t پته = sp + (i * حجم(کلمه_t));
        read_کلمه_From_VSpace_ret_t پایله;
        پایله = read_کلمه_From_VSpace(vspace_ریښه, پته);
        إذا (پایله.حالت == EXCEPTION_NONE) {
            چاپ_کړه("0x%lx: 0x%lx\n", (اوږد)پته, (اوږد)پایله.ارزښت);
        } وإلا {
            چاپ_کړه("0x%lx: INVALID\n", (اوږد)پته);
        }
    }
}
#د_که_پای /* CONFIG_PRINTING */

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

    pde_t pde = pde_pde_large_new(
                    0,                 /* xd                   */
                    ks_User_Log_بفر,   /* page_base_address    */
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
#که_تعریف_شوی CONFIG_HUGE_پاڼه
    x64KSKernelPD[1] = pde;
#وإلا
    x64KSKernelPDs[بټ(PDPT_شاخص_بټونه) - 1][1] = pde;
#د_که_پای
    invalidateTranslationAll(ماسک(CONFIG_MAX_عدد_NODES));

    أرجع EXCEPTION_NONE;
}
#د_که_پای /* CONFIG_KERNEL_LOG_BUFFER */
