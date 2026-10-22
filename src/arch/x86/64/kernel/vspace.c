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
#وصف_ڪريو KERNEL_IS_GLOBAL() (config_سيٽ(CONFIG_KERNEL_SKIM_دري) ? 0 : 1)

/* For the boot code we create two windows into the physical address space
 * One is at the same location as the kernel window, and is placed up high
 * The other is a 1-to-1 mapping of the first 512gb of memory. The purpose
 * of this is to have a 1-to-1 mapping for the low parts of memory, so that
 * when we switch paging on, and are still running at physical addresses,
 * we don't explode. Then we also want the high mappings so we can start
 * running at proper kernel virtual addresses */
pml4e_t boot_pml4[ٻٽو_انگ(PML4_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_صفحو_بٽ)) VISIBLE PHYS_BSS;
pdpte_t boot_pdpt[ٻٽو_انگ(PDPT_اشاريو_بٽ)] ALIGN(ٻٽو_انگ(se_L4_صفحو_بٽ)) VISIBLE PHYS_BSS;

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

BOOT_ڪوڊ bool_t map_kernel_دري(
    uint32_t انگ_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   انگ_drhu,
    paddr_t   *drhu_list
)
{

    uint64_t paddr;
    uint64_t vaddr;

#جيڪڏهن_وصف_ٿيل CONFIG_HUGE_صفحو
    /* using 1 GiB page size */

    /* verify that the kernel window as at the last entry of the PML4 */
    أكد(GET_PML4_اشاريو(PPTR_بنياد) == ٻٽو_انگ(PML4_اشاريو_بٽ) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    أكد(GET_PML4_اشاريو(KERNEL_ELF_بنياد) == ٻٽو_انگ(PML4_اشاريو_بٽ) - 1);
    أكد(GET_PDPT_اشاريو(KERNEL_ELF_بنياد) == ٻٽو_انگ(PML4_اشاريو_بٽ) - 2);
    أكد(GET_PDPT_اشاريو(KDEV_بنياد) == ٻٽو_انگ(PML4_اشاريو_بٽ) - 1);
    أكد(IS_ALIGNED(KERNEL_ELF_بنياد - KERNEL_ELF_PADDR_بنياد, se_L4_Huge_صفحو_بٽ));
    أكد(IS_ALIGNED(KDEV_بنياد, se_L4_Huge_صفحو_بٽ));
    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_اشاريو(PPTR_بنياد)] = pml4e_new(
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
    x64KSKernelPDPT[GET_PDPT_اشاريو(KERNEL_ELF_بنياد)] = pdpte_pdpte_1g_new(
                                                           0, /* xd */
                                                           PADDR_بنياد,
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
    vaddr = PPTR_بنياد;
    لكل (paddr = 0; paddr < PADDR_چوٽي;
         paddr += ٻٽو_انگ(se_L4_Huge_صفحو_بٽ)) {

        صحيح pdpte_اشاريو = GET_PDPT_اشاريو(vaddr);
        x64KSKernelPDPT[pdpte_اشاريو] = pdpte_pdpte_1g_new(
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

        vaddr += ٻٽو_انگ(se_L4_Huge_صفحو_بٽ);
    }

    /* put the PD into the PDPT */
    x64KSKernelPDPT[GET_PDPT_اشاريو(KDEV_بنياد)] = pdpte_pdpte_pd_new(
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

    صحيح pd_اشاريو_2 = 0;
    /* use 2 MiB page size */
    /* verify that the kernel window as at the last entry of the PML4 */
    أكد(GET_PML4_اشاريو(PPTR_بنياد) == ٻٽو_انگ(PML4_اشاريو_بٽ) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    أكد(GET_PML4_اشاريو(KERNEL_ELF_بنياد) == ٻٽو_انگ(PML4_اشاريو_بٽ) - 1);
    أكد(GET_PDPT_اشاريو(KERNEL_ELF_بنياد) == ٻٽو_انگ(PML4_اشاريو_بٽ) - 2);
    أكد(GET_PDPT_اشاريو(KDEV_بنياد) == ٻٽو_انگ(PML4_اشاريو_بٽ) - 1);
    أكد(IS_ALIGNED(KERNEL_ELF_بنياد - KERNEL_ELF_PADDR_بنياد, se_L4_Huge_صفحو_بٽ));
    أكد(IS_ALIGNED(KDEV_بنياد, se_L4_Huge_صفحو_بٽ));

    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_اشاريو(PPTR_بنياد)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_منزل_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    لكل (pd_اشاريو_2 = 0; pd_اشاريو_2 < PADDR_چوٽي >> se_L4_Huge_صفحو_بٽ; pd_اشاريو_2++) {
        /* put the 1GB kernel_base mapping into the PDPT */
        x64KSKernelPDPT[GET_PDPT_اشاريو(PPTR_بنياد) + pd_اشاريو_2] = pdpte_pdpte_pd_new(
                                                                    0, /* xd */
                                                                    kpptr_منزل_paddr(&x64KSKernelPDs[pd_اشاريو_2][0]),
                                                                    0, /* accessed */
                                                                    0, /* cache disabled */
                                                                    0, /* write through */
                                                                    0, /* super user */
                                                                    1, /* read write */
                                                                    1 /* present */
                                                                );
    }

    x64KSKernelPDPT[GET_PDPT_اشاريو(KERNEL_ELF_بنياد)] = pdpte_pdpte_pd_new(
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
    vaddr = PPTR_بنياد;

    لكل (paddr = 0; paddr < PADDR_چوٽي;
         paddr += 0x200000) {

        صحيح pd_اشاريو_2 = GET_PDPT_اشاريو(vaddr) - GET_PDPT_اشاريو(PPTR_بنياد);
        صحيح pde_اشاريو = GET_PD_اشاريو(vaddr);

        x64KSKernelPDs[pd_اشاريو_2][pde_اشاريو] = pde_pde_large_new(
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
    x64KSKernelPDPT[GET_PDPT_اشاريو(KDEV_بنياد)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_منزل_paddr(&x64KSKernelPDs[ٻٽو_انگ(PDPT_اشاريو_بٽ) - 1][0]),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    /* put the PT into the PD */
    x64KSKernelPDs[ٻٽو_انگ(PDPT_اشاريو_بٽ) - 1][0] = pde_pde_pt_new(
                                                      0, /* xd */
                                                      kpptr_منزل_paddr(x64KSKernelPT),
                                                      0, /* accessed */
                                                      0, /* cache_disabled */
                                                      0, /* write_through */
                                                      0, /* super_user */
                                                      1, /* read_write */
                                                      1  /* present */
                                                  );
#جيڪڏهن_پورو

#إذا CONFIG_MAX_انگ_TRACE_POINTS > 0
    /* use the last PD entry as the benchmark log storage.
     * the actual backing physical memory will be filled
     * later by using alloc_region */
    ksLog = (ks_log_داخلا_t *)(KDEV_بنياد + 0x200000 * (ٻٽو_انگ(PD_اشاريو_بٽ) - 1));
#جيڪڏهن_پورو

    /* now map in the kernel devices */
    إذا (!map_kernel_دري_devices(x64KSKernelPT, انگ_ioapic, ioapic_paddrs, انگ_drhu, drhu_list)) {
        أرجع false;
    }

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(x64KSKernelPML4);
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    /* In boot code, so fine to just trash everything here */
    invalidateLocalTranslationAll();
    لکو("Mapping kernel window is done\n");
    أرجع true;
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_SKIM_دري
BOOT_ڪوڊ bool_t map_skim_دري(vptr_t skim_شروعات, vptr_t skim_پڇاڙي)
{
    /* place the PDPT into the PML4 */
    x64KSSKIMPML4[GET_PML4_اشاريو(PPTR_بنياد)] = pml4e_new(
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
    x64KSSKIMPDPT[GET_PDPT_اشاريو(KERNEL_ELF_بنياد)] = pdpte_pdpte_pd_new(
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
    أكد((skim_شروعات % ٻٽو_انگ(se_L4_Large_صفحو_بٽ)) == 0);
    أكد((skim_پڇاڙي % ٻٽو_انگ(se_L4_Large_صفحو_بٽ)) == 0);
    uint64_t paddr = kpptr_منزل_paddr((فراغ *)skim_شروعات);
    لكل (صحيح i = GET_PD_اشاريو(skim_شروعات); i < GET_PD_اشاريو(skim_پڇاڙي); i++) {
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
        paddr += ٻٽو_انگ(se_L4_Large_صفحو_بٽ);
    }
    أرجع true;
}
#جيڪڏهن_پورو

BOOT_ڪوڊ فراغ init_tss(tss_t *tss)
{
    لفظ_t بنياد = (لفظ_t)&x64KSIRQStack[CURRENT_CPU_اشاريو()][IRQ_ڍير_سائيز];
    *tss = tss_new(
               حجم(*tss),   /* io map base */
               0, 0,       /* ist 7 */
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               /* ist 1 is the stack frame we use for interrupts */
               بنياد >> 32, بنياد & 0xffffffff,  /* ist 1 */
               0, 0,       /* rsp 2 */
               0, 0,       /* rsp 1 */
               0, 0        /* rsp 0 */
           );
    /* set the IO map to all 1 to block user IN/OUT instructions */
    memset(&x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KStss.io_map[0], 0xff,
           حجم(x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KStss.io_map));
}

فراغ init_syscall_msrs(فراغ)
{
    x86_wrmsr(IA32_LSTAR_MSR, (uint64_t)&handle_fastsyscall);
    // mask bit 9 in the kernel (which is the interrupt enable bit)
    // also mask bit 8, which is the Trap Flag, to prevent the kernel
    // from single stepping
    x86_wrmsr(IA32_FMASK_MSR, FLAGS_TF | FLAGS_IF);
    x86_wrmsr(IA32_تارو_MSR, ((uint64_t)SEL_CS_0 << 32) | ((uint64_t)SEL_CS_3 << 48));
}

BOOT_ڪوڊ فراغ init_gdt(gdt_داخلا_t *gdt, tss_t *tss)
{

    uint64_t tss_بنياد = (uint64_t)tss;
    gdt_tss_t gdt_tss;

    gdt[GDT_NULL] = gdt_داخلا_gdt_null_new();

    gdt[GDT_CS_0] = gdt_داخلا_gdt_ڪوڊ_new(
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

    gdt[GDT_DS_0] = gdt_داخلا_gdt_معلومات_new(
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

    gdt[GDT_CS_3] = gdt_داخلا_gdt_ڪوڊ_new(
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

    gdt[GDT_DS_3] = gdt_داخلا_gdt_معلومات_new(
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

    gdt[GDT_FS] = gdt_داخلا_gdt_معلومات_new(
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

    gdt[GDT_GS] = gdt_داخلا_gdt_معلومات_new(
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
                  tss_بنياد >> 32,                     /* base 63 - 32 */
                  (tss_بنياد & 0xff000000UL) >> 24,    /* base 31 - 24 */
                  1,                                  /* granularity */
                  0,                                  /* avl */
                  0,                                  /* limit high */
                  1,                                  /* present */
                  0,                                  /* dpl */
                  9,                                  /* desc type */
                  (tss_بنياد & 0xff0000UL) >> 16,      /* base 23-16 */
                  (tss_بنياد & 0xffffUL),              /* base 15 - 0 */
                  حجم(tss_io_t) - 1
              );

    gdt[GDT_TSS].لفظ[0] = gdt_tss.لفظ[0];
    gdt[GDT_TSS + 1].لفظ[0] = gdt_tss.لفظ[1];
}

BOOT_ڪوڊ فراغ init_idt_داخلا(idt_داخلا_t *idt, مداخلت_t مداخلت, فراغ(*handler)(فراغ))
{
    uint64_t handler_addr = (uint64_t)handler;
    uint64_t dpl = 3;

    إذا (مداخلت < int_trap_min && مداخلت != int_software_break_request) {
        dpl = 0;
    }

    idt[مداخلت] = idt_داخلا_مداخلت_gate_new(
                         handler_addr >> 32,                 /* offset 63 - 32 */
                         ((handler_addr >> 16) & 0xffff),
                         1,                                  /* present */
                         dpl,                                /* dpl */
                         1,                                  /* ist */
                         SEL_CS_0,                           /* segment selector */
                         (handler_addr & 0xffff)               /* offset 15 - 0 */
                     );
}

فراغ سيٽ_VMRoot(tcb_t *tcb)
{
    cap_t thread_پاڙ;
    asid_t asid;
    pml4e_t *pml4;
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;
    cr3_t cr3;

    thread_پاڙ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_پاڙ) != cap_pml4_cap ||
        !cap_pml4_cap_get_capPML4IsMapped(thread_پاڙ)) {
        سيٽ_Current_User_VSpace_پاڙ(kpptr_منزل_paddr(X86_GLOBAL_VSPACE_پاڙ), 0);
        أرجع;
    }

    pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنياد_Ptr(thread_پاڙ));
    asid = cap_pml4_cap_get_capPML4MappedASID(thread_پاڙ);
    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (unlikely(ڳوليو_ret.حالت != EXCEPTION_NONE || ڳوليو_ret.vspace_پاڙ != pml4)) {
        سيٽ_Current_User_VSpace_پاڙ(kpptr_منزل_paddr(X86_GLOBAL_VSPACE_پاڙ), 0);
        أرجع;
    }
    cr3 = makeCR3(pptr_منزل_paddr(pml4), asid);
    إذا (getCurrentUserCR3().لفظ[0] != cr3.لفظ[0]) {
        SMP_COND_STATEMENT(tlb_bitmap_سيٽ(pml4, getCurrentCPUIndex());)
        سيٽ_Current_User_CR3(cr3);
    }
}


BOOT_ڪوڊ فراغ init_dtrs(فراغ)
{
    gdt_idt_ptr.حد_بندي = (حجم(gdt_داخلا_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.بنياد = (uint64_t)x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KSgdt;

    /* When we install the gdt it will clobber any value of gs that
     * we have. Since we might be using it for TLS we can stash
     * and unstash any gs value using swapgs
     */
    swapgs();
    x64_install_gdt(&gdt_idt_ptr);
    swapgs();

    gdt_idt_ptr.حد_بندي = (حجم(idt_داخلا_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.بنياد = (uint64_t)x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KSidt;
    x64_install_idt(&gdt_idt_ptr);

    x64_install_ldt(SEL_NULL);

    x64_install_tss(SEL_TSS);
}

BOOT_ڪوڊ فراغ map_it_چوکٺو_cap(cap_t pd_cap, cap_t چوکٺو_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_جو_cap(pd_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt;
    vptr_t vptr = cap_چوکٺو_cap_get_cap_FMapped_پتو(چوکٺو_cap);
    فراغ *pptr = (فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(چوکٺو_cap);

    أكد(cap_چوکٺو_cap_get_cap_FMap_Type(چوکٺو_cap) == X86_MappingVSpace);
    أكد(cap_چوکٺو_cap_get_cap_FMapped_ASID(چوکٺو_cap) != asidInvalid);
    pml4 += GET_PML4_اشاريو(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنياد_پتو(pml4));
    pdpt += GET_PDPT_اشاريو(vptr);
    أكد(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_بنياد_پتو(pdpt));
    pd += GET_PD_اشاريو(vptr);
    أكد(pde_pde_pt_ptr_get_present(pd));
    pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_بنياد_پتو(pd));
    *(pt + GET_PT_اشاريو(vptr)) = pte_new(
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

ساكن BOOT_ڪوڊ فراغ map_it_pdpt_cap(cap_t vspace_cap, cap_t pdpt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_جو_cap(vspace_cap));
    pdpte_t *pdpt = PDPT_PTR(cap_pdpt_cap_get_capPDPTBasePtr(pdpt_cap));
    vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_پتو(pdpt_cap);

    أكد(cap_pdpt_cap_get_capPDPTIsMapped(pdpt_cap));
    *(pml4 + GET_PML4_اشاريو(vptr)) = pml4e_new(
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

BOOT_ڪوڊ فراغ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_جو_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd = PD_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    vptr_t vptr = cap_صفحو_directory_cap_get_cap_PDMapped_پتو(pd_cap);

    أكد(cap_صفحو_directory_cap_get_cap_PDIs_Mapped(pd_cap));
    pml4 += GET_PML4_اشاريو(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنياد_پتو(pml4));
    *(pdpt + GET_PDPT_اشاريو(vptr)) = pdpte_pdpte_pd_new(
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

BOOT_ڪوڊ فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_جو_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt = PT_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(pt_cap);

    أكد(cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(pt_cap));
    pml4 += GET_PML4_اشاريو(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنياد_پتو(pml4));
    pdpt += GET_PDPT_اشاريو(vptr);
    أكد(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_بنياد_پتو(pdpt));
    *(pd + GET_PD_اشاريو(vptr)) = pde_pde_pt_new(
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

BOOT_ڪوڊ فراغ *map_عارضي_boot_صفحو(فراغ *داخلا, uint32_t large_صفحا)
{
    /* this function is for legacy 32-bit systems where the ACPI tables might
     * collide with the kernel window. Here we just assert that the table is
     * in fact in the lower 4GiB region (which is already 1-to-1 mapped) and
     * continue */
    أكد((لفظ_t)داخلا < ٻٽو_انگ(32));
    أرجع داخلا;
}

ساكن BOOT_ڪوڊ cap_t create_it_pdpt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
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

ساكن BOOT_ڪوڊ cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحو_directory_cap_new(
              asid,   /* capPDMappedASID      */
              pptr,   /* capPDBasePtr         */
              1,      /* capPDIsMapped        */
              vptr    /* capPDMappedAddress   */
          );
    map_it_pd_cap(vspace_cap, cap);
    أرجع cap;
}

ساكن BOOT_ڪوڊ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحو_جدول_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );
    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}


BOOT_ڪوڊ لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    لفظ_t n = get_n_paging(it_v_reg, PD_اشاريو_وٿي);
    n += get_n_paging(it_v_reg, PDPT_اشاريو_وٿي);
    n += get_n_paging(it_v_reg, PML4_اشاريو_وٿي);
#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#جيڪڏهن_پورو
    أرجع n;
}

BOOT_ڪوڊ cap_t create_it_پتو_space(cap_t پاڙ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_جڳهه_Pos جڳهه_pos_before;
    se_L4_جڳهه_Pos جڳهه_pos_after;

    جڳهه_pos_before = ndks_boot.جڳهه_pos_cur;
    copyGlobalMappings(PML4_PTR(rootserver.vspace));
    vspace_cap = cap_pml4_cap_new(
                     IT_ASID,        /* capPML4MappedASID */
                     rootserver.vspace,           /* capPML4BasePtr   */
                     1               /* capPML4IsMapped   */
                 );


    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

    /* Create any PDPTs needed for the user land image */
    لكل (vptr = ڦيرو_هيٺ_ڏانهن(it_v_reg.شروعات, PML4_اشاريو_وٿي);
         vptr < it_v_reg.پڇاڙي;
         vptr += ٻٽو_انگ(PML4_اشاريو_وٿي)) {
        إذا (!provide_cap(پاڙ_cnode_cap,
                         create_it_pdpt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    /* Create any PDs needed for the user land image */
    لكل (vptr = ڦيرو_هيٺ_ڏانهن(it_v_reg.شروعات, PDPT_اشاريو_وٿي);
         vptr < it_v_reg.پڇاڙي;
         vptr += ٻٽو_انگ(PDPT_اشاريو_وٿي)) {
        إذا (!provide_cap(پاڙ_cnode_cap,
                         create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    لكل (vptr = ڦيرو_هيٺ_ڏانهن(it_v_reg.شروعات, PD_اشاريو_وٿي);
         vptr < it_v_reg.پڇاڙي;
         vptr += ٻٽو_انگ(PD_اشاريو_وٿي)) {
        إذا (!provide_cap(پاڙ_cnode_cap,
                         create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
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

فراغ copyGlobalMappings(vspace_پاڙ_t *new_vspace)
{
    بنانشان ڊگهو i;
    pml4e_t *vspace = (pml4e_t *)new_vspace;

    /* Copy from the tlbbitmap_pptr so that we copy the default entries of the
     * tlb bitmap (if it exists). If it doesn't exist then this loop
     * will be equivalent to copying from PPTR_BASE
     */
    لكل (i = GET_PML4_اشاريو(TLBBITMAP_PPTR); i < ٻٽو_انگ(PML4_اشاريو_بٽ); i++) {
        vspace[i] = X86_GLOBAL_VSPACE_پاڙ[i];
    }
}

ساكن BOOT_ڪوڊ cap_t create_it_چوکٺو_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large, se_L4_لفظ map_type)
{
    vm_صفحو_سائيز_t چوکٺو_سائيز_2;

    إذا (use_large) {
        چوکٺو_سائيز_2 = X86_Large_صفحو;
    } وإلا {
        چوکٺو_سائيز_2 = X86_Small_صفحو;
    }

    أرجع
        cap_چوکٺو_cap_new(
            asid,                          /* capFMappedASID     */
            pptr,                          /* capFBasePtr        */
            چوکٺو_سائيز_2,                    /* capFSize           */
            map_type,                      /* capFMapType        */
            vptr,                          /* capFMappedAddress  */
            لفظ_From_VMRights(VMReadWrite), /* capFVMRights       */
            0                              /* capFIsDevice       */
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

/* ====================== BOOT CODE FINISHES HERE ======================== */



exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_جڳهه)
{
    asid_map_t asid_map;
#جيڪڏهن_وصف_ٿيل CONFIG_VTX
    إذا (cap_get_capType(vspace_Cap_جڳهه->cap) == cap_ept_pml4_cap) {
        cap_ept_pml4_cap_ptr_سيٽ_cap_PML4_Mapped_ASID(&vspace_Cap_جڳهه->cap, asid);
        cap_ept_pml4_cap_ptr_سيٽ_cap_PML4_Is_Mapped(&vspace_Cap_جڳهه->cap, 1);
        asid_map = asid_map_asid_map_ept_new(cap_ept_pml4_cap_get_cap_PML4_بنياد_Ptr(vspace_Cap_جڳهه->cap));
    } وإلا
#جيڪڏهن_پورو
    {
        أكد(cap_get_capType(vspace_Cap_جڳهه->cap) == cap_pml4_cap);
        cap_pml4_cap_ptr_سيٽ_cap_PML4_Mapped_ASID(&vspace_Cap_جڳهه->cap, asid);
        cap_pml4_cap_ptr_سيٽ_cap_PML4_Is_Mapped(&vspace_Cap_جڳهه->cap, 1);
        asid_map = asid_map_asid_map_vspace_new(cap_pml4_cap_get_cap_PML4_بنياد_Ptr(vspace_Cap_جڳهه->cap));
    }
    poolPtr->array[ASID_هيٺيون(asid)] = asid_map;
    أرجع EXCEPTION_NONE;
}

bool_t CONST is_VTable_پاڙ(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_pml4_cap;
}

bool_t CONST is_Valid_Native_پاڙ(cap_t cap)
{
    أرجع is_VTable_پاڙ(cap) &&
           cap_pml4_cap_get_capPML4IsMapped(cap);
}

ساكن pml4e_t CONST makeUserPML4E(paddr_t paddr, vm_خاصيتون_t vm_attr)
{
    أرجع pml4e_new(
               0,
               paddr,
               0,
               vm_خاصيتون_get_x86_PCDBit(vm_attr),
               vm_خاصيتون_get_x86_PWTBit(vm_attr),
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

ساكن pdpte_t CONST make_User_PDPTEHuge_صفحو(paddr_t paddr, vm_خاصيتون_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pdpte_pdpte_1g_new(
               0,          /* xd               */
               paddr,      /* physical address */
               0,          /* PAT              */
               0,          /* global           */
               0,          /* dirty            */
               0,          /* accessed         */
               vm_خاصيتون_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_خاصيتون_get_x86_PWTBit(vm_attr),  /* write through  */
               SuperUserFromVMRights(vm_rights),       /* super user     */
               WritableFromVMRights(vm_rights),        /* read write     */
               1                                       /* present        */
           );
}

ساكن pdpte_t CONST makeUserPDPTEPageDirectory(paddr_t paddr, vm_خاصيتون_t vm_attr)
{
    أرجع pdpte_pdpte_pd_new(
               0,                      /* xd       */
               paddr,                  /* paddr    */
               0,                      /* accessed */
               vm_خاصيتون_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_خاصيتون_get_x86_PWTBit(vm_attr),  /* write through  */
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

pde_t CONST make_User_PDELarge_صفحو(paddr_t paddr, vm_خاصيتون_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pde_pde_large_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               vm_خاصيتون_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_خاصيتون_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_خاصيتون_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_جدول(paddr_t paddr, vm_خاصيتون_t vm_attr)
{

    أرجع  pde_pde_pt_new(
                0,                                  /* xd               */
                paddr,                              /* pt_base_address  */
                0,                                  /* accessed         */
                vm_خاصيتون_get_x86_PCDBit(vm_attr), /* cache_disabled   */
                vm_خاصيتون_get_x86_PWTBit(vm_attr), /* write_through    */
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_خاصيتون_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pte_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               0,                                              /* global               */
               vm_خاصيتون_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_خاصيتون_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_خاصيتون_get_x86_PWTBit(vm_attr),          /* write_through        */
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


ساكن pml4e_t *lookup_PML4_جڳهه(vspace_پاڙ_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4e = PML4E_PTR(pml4);
    لفظ_t pml4_اشاريو = GET_PML4_اشاريو(vptr);
    أرجع pml4e + pml4_اشاريو;
}

ساكن lookupPDPTSlot_ret_t lookupPDPTSlot(vspace_پاڙ_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4_جڳهه = lookup_PML4_جڳهه(pml4, vptr);
    lookupPDPTSlot_ret_t ret;

    إذا (!pml4e_ptr_get_present(pml4_جڳهه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PML4_اشاريو_وٿي);

        ret.pdpt_جڳهه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pdpte_t *pdpt;
        pdpte_t *pdpt_جڳهه;
        لفظ_t pdpt_اشاريو = GET_PDPT_اشاريو(vptr);
        pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنياد_پتو(pml4_جڳهه));
        pdpt_جڳهه = pdpt + pdpt_اشاريو;

        ret.حالت = EXCEPTION_NONE;
        ret.pdpt_جڳهه = pdpt_جڳهه;
        أرجع ret;
    }
}

lookupPDSlot_ret_t lookupPDSlot(vspace_پاڙ_t *pml4, vptr_t vptr)
{
    lookupPDPTSlot_ret_t pdpt_جڳهه;
    lookupPDSlot_ret_t ret;

    pdpt_جڳهه = lookupPDPTSlot(pml4, vptr);

    إذا (pdpt_جڳهه.حالت != EXCEPTION_NONE) {
        ret.pd_جڳهه = عدم;
        ret.حالت = pdpt_جڳهه.حالت;
        أرجع ret;
    }
    إذا ((pdpte_ptr_get_صفحو_سائيز(pdpt_جڳهه.pdpt_جڳهه) != pdpte_pdpte_pd) ||
        !pdpte_pdpte_pd_ptr_get_present(pdpt_جڳهه.pdpt_جڳهه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PDPT_اشاريو_وٿي);

        ret.pd_جڳهه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pde_t *pd;
        pde_t *pd_جڳهه;
        لفظ_t pd_اشاريو = GET_PD_اشاريو(vptr);
        pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_بنياد_پتو(pdpt_جڳهه.pdpt_جڳهه));
        pd_جڳهه = pd + pd_اشاريو;

        ret.حالت = EXCEPTION_NONE;
        ret.pd_جڳهه = pd_جڳهه;
        أرجع ret;
    }
}

ساكن فراغ flushPD(vspace_پاڙ_t *vspace, لفظ_t vptr, pde_t *pd, asid_t asid)
{
    /* clearing the entire PCID vs flushing the virtual addresses
     * one by one using invplg.
     * choose the easy way, invalidate the PCID
     */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));

}

ساكن فراغ flushPDPT(vspace_پاڙ_t *vspace, لفظ_t vptr, pdpte_t *pdpt, asid_t asid)
{
    /* similar here */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع;
}

فراغ hwASIDInvalidate(asid_t asid, vspace_پاڙ_t *vspace)
{
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
}

فراغ unmap_صفحو_Directory(asid_t asid, vptr_t vaddr, pde_t *pd)
{
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;
    lookupPDPTSlot_ret_t    lu_ret;

    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (ڳوليو_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPDPTSlot(ڳوليو_ret.vspace_پاڙ, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    /* check if the PDPT has the PD */
    إذا (!(pdpte_ptr_get_صفحو_سائيز(lu_ret.pdpt_جڳهه) == pdpte_pdpte_pd &&
          pdpte_pdpte_pd_ptr_get_present(lu_ret.pdpt_جڳهه) &&
          (pdpte_pdpte_pd_ptr_get_pd_بنياد_پتو(lu_ret.pdpt_جڳهه) == pptr_منزل_paddr(pd)))) {
        أرجع;
    }

    flushPD(ڳوليو_ret.vspace_پاڙ, vaddr, pd, asid);

    *lu_ret.pdpt_جڳهه = makeUserPDPTEInvalid();

    invalidate_صفحو_Structure_Cache_ASID(pptr_منزل_paddr(ڳوليو_ret.vspace_پاڙ), asid,
                                     SMP_TERNARY(tlb_bitmap_get(ڳوليو_ret.vspace_پاڙ), 0));
}


ساكن exception_t perform_X64_صفحو_Directory_Invocation_Unmap(cap_t cap, cte_t *ct_جڳهه)
{

    إذا (cap_صفحو_directory_cap_get_cap_PDIs_Mapped(cap)) {
        pde_t *pd = PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(cap));
        unmap_صفحو_Directory(
            cap_صفحو_directory_cap_get_cap_PDMapped_ASID(cap),
            cap_صفحو_directory_cap_get_cap_PDMapped_پتو(cap),
            pd
        );
        clear_يادگيري((فراغ *)pd, cap_get_cap_سائيز_بٽ(cap));
    }

    cap_صفحو_directory_cap_ptr_سيٽ_cap_PDIs_Mapped(&(ct_جڳهه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X64_صفحو_Directory_Invocation_Map(cap_t cap, cte_t *ct_جڳهه, pdpte_t pdpte, pdpte_t *pdpt_جڳهه,
                                                        vspace_پاڙ_t *vspace)
{
    ct_جڳهه->cap = cap;
    *pdpt_جڳهه = pdpte;
    invalidate_صفحو_Structure_Cache_ASID(pptr_منزل_paddr(vspace), cap_صفحو_directory_cap_get_cap_PDMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}


ساكن exception_t decode_X64_صفحو_Directory_Invocation(
    لفظ_t label,
    لفظ_t ڊيگهائي,
    cte_t *cte,
    cap_t cap,
    لفظ_t *بفر
)
{
    لفظ_t              vaddr;
    vm_خاصيتون_t     vm_attr;
    cap_t               vspaceCap;
    vspace_پاڙ_t      *vspace;
    pdpte_t             pdpte;
    paddr_t             paddr;
    asid_t              asid;
    lookupPDPTSlot_ret_t pdpt_جڳهه;

    إذا (label == X86_صفحو_Directory_Unmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_پهريون;
            userError("X86PageDirectory: Cannot unmap if more than one cap exist.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);

        أرجع perform_X64_صفحو_Directory_Invocation_Unmap(cap, cte);
    }

    إذا (label != X86_صفحو_Directory_Map) {
        userError("X64Directory: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ڊيگهائي < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X64PageDirectory: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_صفحو_directory_cap_get_cap_PDIs_Mapped(cap)) {
        userError("X64PageDirectory: PD is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر) & (~نقاب(PDPT_اشاريو_وٿي));
    vm_attr = vm_خاصيتون_From_لفظ(getSyscallArg(1, بفر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    إذا (!is_Valid_Native_پاڙ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_پاڙ_t *)pptr_جو_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    إذا (vaddr > USER_چوٽي) {
        userError("X64PageDirectory: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;

    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (ڳوليو_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ڳوليو_ret.vspace_پاڙ != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pdpt_جڳهه = lookupPDPTSlot(vspace, vaddr);
    إذا (pdpt_جڳهه.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا ((pdpte_ptr_get_صفحو_سائيز(pdpt_جڳهه.pdpt_جڳهه) == pdpte_pdpte_pd &&
         pdpte_pdpte_pd_ptr_get_present(pdpt_جڳهه.pdpt_جڳهه)) ||
        (pdpte_ptr_get_صفحو_سائيز(pdpt_جڳهه.pdpt_جڳهه) == pdpte_pdpte_1g
         && pdpte_pdpte_1g_ptr_get_present(pdpt_جڳهه.pdpt_جڳهه))) {
        current_syscall_error.type = se_L4_Delete_پهريون;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr(PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(cap)));
    pdpte = makeUserPDPTEPageDirectory(paddr, vm_attr);

    cap = cap_صفحو_directory_cap_سيٽ_cap_PDIs_Mapped(cap, 1);
    cap = cap_صفحو_directory_cap_سيٽ_cap_PDMapped_ASID(cap, asid);
    cap = cap_صفحو_directory_cap_سيٽ_cap_PDMapped_پتو(cap, vaddr);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_X64_صفحو_Directory_Invocation_Map(cap, cte, pdpte, pdpt_جڳهه.pdpt_جڳهه, vspace);
}

ساكن فراغ unmapPDPT(asid_t asid, vptr_t vaddr, pdpte_t *pdpt)
{
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;
    pml4e_t *pml4_جڳهه;

    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (ڳوليو_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    pml4_جڳهه = lookup_PML4_جڳهه(ڳوليو_ret.vspace_پاڙ, vaddr);

    /* check if the PML4 has the PDPT */
    إذا (!(pml4e_ptr_get_present(pml4_جڳهه) &&
          pml4e_ptr_get_pdpt_بنياد_پتو(pml4_جڳهه) == pptr_منزل_paddr(pdpt))) {
        أرجع;
    }

    flushPDPT(ڳوليو_ret.vspace_پاڙ, vaddr, pdpt, asid);

    *pml4_جڳهه = makeUserPML4EInvalid();
}

ساكن exception_t performX64PDPTInvocationUnmap(cap_t cap, cte_t *ct_جڳهه)
{
    إذا (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        pdpte_t *pdpt = PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap));
        unmapPDPT(cap_pdpt_cap_get_capPDPTMappedASID(cap),
                  cap_pdpt_cap_get_cap_PDPTMapped_پتو(cap),
                  pdpt);
        clear_يادگيري((فراغ *)pdpt, cap_get_cap_سائيز_بٽ(cap));
    }

    cap_pdpt_cap_ptr_سيٽ_cap_PDPTIs_Mapped(&(ct_جڳهه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performX64PDPTInvocationMap(cap_t cap, cte_t *ct_جڳهه, pml4e_t pml4e, pml4e_t *pml4_جڳهه,
                                               vspace_پاڙ_t *vspace)
{
    ct_جڳهه->cap = cap;
    *pml4_جڳهه = pml4e;
    invalidate_صفحو_Structure_Cache_ASID(pptr_منزل_paddr(vspace), cap_pdpt_cap_get_capPDPTMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeX64PDPTInvocation(
    لفظ_t  label,
    لفظ_t ڊيگهائي,
    cte_t   *cte,
    cap_t   cap,
    لفظ_t  *بفر)
{
    لفظ_t                  vaddr;
    vm_خاصيتون_t         attr;
    pml4e_t                *pml4_جڳهه;
    cap_t                   vspaceCap;
    vspace_پاڙ_t          *vspace;
    pml4e_t                 pml4e;
    paddr_t                 paddr;
    asid_t                  asid;

    إذا (label == X86PDPTUnmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_پهريون;
            userError("X86PDPT: Cannot unmap if more than one cap exist.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);

        أرجع performX64PDPTInvocationUnmap(cap, cte);
    }

    إذا (label != X86PDPTMap) {
        userError("X86PDPT: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ڊيگهائي < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X64PDPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X64PDPT: PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر) & (~نقاب(PML4_اشاريو_وٿي));
    attr = vm_خاصيتون_From_لفظ(getSyscallArg(1, بفر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    إذا (!is_Valid_Native_پاڙ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_پاڙ_t *)pptr_جو_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    إذا (vaddr > USER_چوٽي) {
        userError("X64PDPT: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;

    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (ڳوليو_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ڳوليو_ret.vspace_پاڙ != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4_جڳهه = lookup_PML4_جڳهه(vspace, vaddr);

    إذا (pml4e_ptr_get_present(pml4_جڳهه)) {
        current_syscall_error.type = se_L4_Delete_پهريون;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr(PDPTE_PTR((cap_pdpt_cap_get_capPDPTBasePtr(cap))));
    pml4e = makeUserPML4E(paddr, attr);

    cap = cap_pdpt_cap_سيٽ_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_pdpt_cap_سيٽ_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_pdpt_cap_سيٽ_cap_PDPTMapped_پتو(cap, vaddr);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع performX64PDPTInvocationMap(cap, cte, pml4e, pml4_جڳهه, vspace);
}

exception_t decode_X86_گهڻائي_MMUInvocation(
    لفظ_t label,
    لفظ_t ڊيگهائي,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_pml4_cap:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_pdpt_cap:
        أرجع decodeX64PDPTInvocation(label, ڊيگهائي, cte, cap, بفر);

    حالة cap_صفحو_directory_cap:
        أرجع decode_X64_صفحو_Directory_Invocation(label, ڊيگهائي, cte, cap, بفر);

    افتراضي:
        fail("Invalid arch cap type");
    }
}

bool_t گهڻائي_Unmap_صفحو(vm_صفحو_سائيز_t صفحو_سائيز, vspace_پاڙ_t *vroot, vptr_t vaddr, فراغ *pptr)
{
    إذا (config_سيٽ(CONFIG_HUGE_صفحو) && صفحو_سائيز == X64_Huge_صفحو) {
        pdpte_t     *pdpte;
        lookupPDPTSlot_ret_t pdpt_ret = lookupPDPTSlot(vroot, vaddr);
        إذا (pdpt_ret.حالت != EXCEPTION_NONE) {
            أرجع false;
        }
        pdpte = pdpt_ret.pdpt_جڳهه;


        إذا (!(pdpte_ptr_get_صفحو_سائيز(pdpte) == pdpte_pdpte_1g
              && pdpte_pdpte_1g_ptr_get_present(pdpte)
              && (pdpte_pdpte_1g_ptr_get_صفحو_بنياد_پتو(pdpte)
                  == pptr_منزل_paddr(pptr)))) {
            أرجع false;
        }

        *pdpte = makeUserPDPTEInvalid();
        أرجع true;
    }
    fail("Invalid page type");
    أرجع false;
}

ساكن exception_t updatePDPTE(asid_t asid, pdpte_t pdpte, pdpte_t *pdpt_جڳهه, vspace_پاڙ_t *vspace)
{
    *pdpt_جڳهه = pdpte;
    invalidate_صفحو_Structure_Cache_ASID(pptr_منزل_paddr(vspace), asid,
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X64_گهڻائي_Map(cap_t cap, cte_t *ct_جڳهه, pdpte_t pdpte, pdpte_t *pdpt_جڳهه, vspace_پاڙ_t *vspace)
{
    ct_جڳهه->cap = cap;
    أرجع updatePDPTE(cap_چوکٺو_cap_get_cap_FMapped_ASID(cap), pdpte, pdpt_جڳهه, vspace);
}

بنية create_mapping_pdpte_return {
    exception_t حالت;
    pdpte_t pdpte;
    pdpte_t *pdpt_جڳهه;
};
تعريفنوع بنية create_mapping_pdpte_return create_mapping_pdpte_return_t;

ساكن create_mapping_pdpte_return_t create_محفوظ_Mapping_Entries_PDPTE(paddr_t بنياد, لفظ_t vaddr, vm_rights_t vmRights,
                                                                    vm_خاصيتون_t attr,
                                                                    vspace_پاڙ_t *vspace)
{
    create_mapping_pdpte_return_t ret;
    lookupPDPTSlot_ret_t          lu_ret;

    lu_ret = lookupPDPTSlot(vspace, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        أرجع ret;
    }
    ret.pdpt_جڳهه = lu_ret.pdpt_جڳهه;

    /* check for existing page directory */
    إذا ((pdpte_ptr_get_صفحو_سائيز(ret.pdpt_جڳهه) == pdpte_pdpte_pd) &&
        (pdpte_pdpte_pd_ptr_get_present(ret.pdpt_جڳهه))) {
        current_syscall_error.type = se_L4_Delete_پهريون;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    ret.pdpte = make_User_PDPTEHuge_صفحو(بنياد, attr, vmRights);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

exception_t decode_X86_گهڻائي_Map_صفحو(لفظ_t label, vm_صفحو_سائيز_t صفحو_سائيز, cte_t *cte, cap_t cap,
                                 vspace_پاڙ_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_خاصيتون_t vm_attr)
{
    إذا (config_سيٽ(CONFIG_HUGE_صفحو) && صفحو_سائيز == X64_Huge_صفحو) {
        create_mapping_pdpte_return_t map_ret;

        map_ret = create_محفوظ_Mapping_Entries_PDPTE(paddr, vaddr, vm_rights, vm_attr, vroot);
        إذا (map_ret.حالت != EXCEPTION_NONE) {
            أرجع map_ret.حالت;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);

        بدل (label) {
        حالة X86_صفحو_Map:
            أرجع perform_X64_گهڻائي_Map(cap, cte, map_ret.pdpte, map_ret.pdpt_جڳهه, vroot);

        افتراضي:
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }
    fail("Invalid Page type");
}

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
تعريفنوع بنية read_لفظ_From_VSpace_ret {
    exception_t حالت;
    لفظ_t قدر;
} read_لفظ_From_VSpace_ret_t;

ساكن read_لفظ_From_VSpace_ret_t read_لفظ_From_VSpace(vspace_پاڙ_t *vspace, لفظ_t vaddr)
{
    read_لفظ_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_جڳهه;
    lookupPDSlot_ret_t pd_جڳهه;
    lookupPDPTSlot_ret_t pdpt_جڳهه;
    paddr_t paddr;
    لفظ_t وٿي;
    pptr_t kernel_vaddr;
    لفظ_t *قدر;

    pdpt_جڳهه = lookupPDPTSlot(vspace, vaddr);
    إذا (pdpt_جڳهه.حالت == EXCEPTION_NONE &&
        pdpte_ptr_get_صفحو_سائيز(pdpt_جڳهه.pdpt_جڳهه) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_جڳهه.pdpt_جڳهه)) {

        paddr = pdpte_pdpte_1g_ptr_get_صفحو_بنياد_پتو(pdpt_جڳهه.pdpt_جڳهه);
        وٿي = vaddr & نقاب(se_L4_Huge_صفحو_بٽ);
    } وإلا {
        pd_جڳهه = lookupPDSlot(vspace, vaddr);
        إذا (pd_جڳهه.حالت == EXCEPTION_NONE &&
            ((pde_ptr_get_صفحو_سائيز(pd_جڳهه.pd_جڳهه) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_جڳهه.pd_جڳهه))) {

            paddr = pde_pde_large_ptr_get_صفحو_بنياد_پتو(pd_جڳهه.pd_جڳهه);
            وٿي = vaddr & نقاب(se_L4_Large_صفحو_بٽ);
        } وإلا {
            pt_جڳهه = lookupPTSlot(vspace, vaddr);
            إذا (pt_جڳهه.حالت == EXCEPTION_NONE && pte_ptr_get_present(pt_جڳهه.pt_جڳهه)) {
                paddr = pte_ptr_get_صفحو_بنياد_پتو(pt_جڳهه.pt_جڳهه);
                وٿي = vaddr & نقاب(se_L4_صفحو_بٽ);
            } وإلا {
                ret.حالت = EXCEPTION_LOOKUP_FAULT;
                أرجع ret;
            }
        }
    }


    kernel_vaddr = (لفظ_t)paddr_منزل_pptr(paddr);
    قدر = (لفظ_t *)(kernel_vaddr + وٿي);
    ret.حالت = EXCEPTION_NONE;
    ret.قدر = *قدر;
    أرجع ret;
}

فراغ Arch_user_ڍير_Trace(tcb_t *tptr)
{
    cap_t thread_پاڙ;
    vspace_پاڙ_t *vspace_پاڙ;
    لفظ_t sp;
    صحيح i;

    thread_پاڙ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    إذا (cap_get_capType(thread_پاڙ) != cap_pml4_cap) {
        لکو("Invalid vspace\n");
        أرجع;
    }

    vspace_پاڙ = (vspace_پاڙ_t *)pptr_جو_cap(thread_پاڙ);

    sp = get_رجسٽر(tptr, RSP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_لفظ_سائيز_بٽ)) {
        لکو("RSP not aligned\n");
        أرجع;
    }

    لكل (i = 0; i < CONFIG_USER_ڍير_TRACE_ڊيگهائي; i++) {
        لفظ_t پتو = sp + (i * حجم(لفظ_t));
        read_لفظ_From_VSpace_ret_t نتيجو;
        نتيجو = read_لفظ_From_VSpace(vspace_پاڙ, پتو);
        إذا (نتيجو.حالت == EXCEPTION_NONE) {
            لکو("0x%lx: 0x%lx\n", (ڊگهو)پتو, (ڊگهو)نتيجو.قدر);
        } وإلا {
            لکو("0x%lx: INVALID\n", (ڊگهو)پتو);
        }
    }
}
#جيڪڏهن_پورو /* CONFIG_PRINTING */

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
#جيڪڏهن_وصف_ٿيل CONFIG_HUGE_صفحو
    x64KSKernelPD[1] = pde;
#وإلا
    x64KSKernelPDs[ٻٽو_انگ(PDPT_اشاريو_بٽ) - 1][1] = pde;
#جيڪڏهن_پورو
    invalidateTranslationAll(نقاب(CONFIG_MAX_انگ_NODES));

    أرجع EXCEPTION_NONE;
}
#جيڪڏهن_پورو /* CONFIG_KERNEL_LOG_BUFFER */
