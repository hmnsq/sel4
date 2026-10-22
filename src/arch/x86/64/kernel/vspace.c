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
#تعریف_کن KERNEL_IS_GLOBAL() (config_مجموعه(CONFIG_KERNEL_SKIM_پنجره) ? 0 : 1)

/* For the boot code we create two windows into the physical address space
 * One is at the same location as the kernel window, and is placed up high
 * The other is a 1-to-1 mapping of the first 512gb of memory. The purpose
 * of this is to have a 1-to-1 mapping for the low parts of memory, so that
 * when we switch paging on, and are still running at physical addresses,
 * we don't explode. Then we also want the high mappings so we can start
 * running at proper kernel virtual addresses */
pml4e_t boot_pml4[بیت(PML4_شاخص_بیتها)] ALIGN(بیت(se_L4_صفحه_بیتها)) VISIBLE PHYS_BSS;
pdpte_t boot_pdpt[بیت(PDPT_شاخص_بیتها)] ALIGN(بیت(se_L4_صفحه_بیتها)) VISIBLE PHYS_BSS;

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

BOOT_کد bool_t map_kernel_پنجره(
    uint32_t رقم_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   رقم_drhu,
    paddr_t   *drhu_list
)
{

    uint64_t paddr;
    uint64_t vaddr;

#اگر_تعریف_شده CONFIG_HUGE_صفحه
    /* using 1 GiB page size */

    /* verify that the kernel window as at the last entry of the PML4 */
    أكد(GET_PML4_شاخص(PPTR_پایه) == بیت(PML4_شاخص_بیتها) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    أكد(GET_PML4_شاخص(KERNEL_ELF_پایه) == بیت(PML4_شاخص_بیتها) - 1);
    أكد(GET_PDPT_شاخص(KERNEL_ELF_پایه) == بیت(PML4_شاخص_بیتها) - 2);
    أكد(GET_PDPT_شاخص(KDEV_پایه) == بیت(PML4_شاخص_بیتها) - 1);
    أكد(IS_ALIGNED(KERNEL_ELF_پایه - KERNEL_ELF_PADDR_پایه, se_L4_Huge_صفحه_بیتها));
    أكد(IS_ALIGNED(KDEV_پایه, se_L4_Huge_صفحه_بیتها));
    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_شاخص(PPTR_پایه)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_مقصد_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );
    /* put the 1GB kernel_base mapping into the PDPT */
    x64KSKernelPDPT[GET_PDPT_شاخص(KERNEL_ELF_پایه)] = pdpte_pdpte_1g_new(
                                                           0, /* xd */
                                                           PADDR_پایه,
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
    vaddr = PPTR_پایه;
    لكل (paddr = 0; paddr < PADDR_بالاترین;
         paddr += بیت(se_L4_Huge_صفحه_بیتها)) {

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

        vaddr += بیت(se_L4_Huge_صفحه_بیتها);
    }

    /* put the PD into the PDPT */
    x64KSKernelPDPT[GET_PDPT_شاخص(KDEV_پایه)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_مقصد_paddr(x64KSKernelPD),
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
                           kpptr_مقصد_paddr(x64KSKernelPT),
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
    أكد(GET_PML4_شاخص(PPTR_پایه) == بیت(PML4_شاخص_بیتها) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    أكد(GET_PML4_شاخص(KERNEL_ELF_پایه) == بیت(PML4_شاخص_بیتها) - 1);
    أكد(GET_PDPT_شاخص(KERNEL_ELF_پایه) == بیت(PML4_شاخص_بیتها) - 2);
    أكد(GET_PDPT_شاخص(KDEV_پایه) == بیت(PML4_شاخص_بیتها) - 1);
    أكد(IS_ALIGNED(KERNEL_ELF_پایه - KERNEL_ELF_PADDR_پایه, se_L4_Huge_صفحه_بیتها));
    أكد(IS_ALIGNED(KDEV_پایه, se_L4_Huge_صفحه_بیتها));

    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_شاخص(PPTR_پایه)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_مقصد_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    لكل (pd_شاخص_2 = 0; pd_شاخص_2 < PADDR_بالاترین >> se_L4_Huge_صفحه_بیتها; pd_شاخص_2++) {
        /* put the 1GB kernel_base mapping into the PDPT */
        x64KSKernelPDPT[GET_PDPT_شاخص(PPTR_پایه) + pd_شاخص_2] = pdpte_pdpte_pd_new(
                                                                    0, /* xd */
                                                                    kpptr_مقصد_paddr(&x64KSKernelPDs[pd_شاخص_2][0]),
                                                                    0, /* accessed */
                                                                    0, /* cache disabled */
                                                                    0, /* write through */
                                                                    0, /* super user */
                                                                    1, /* read write */
                                                                    1 /* present */
                                                                );
    }

    x64KSKernelPDPT[GET_PDPT_شاخص(KERNEL_ELF_پایه)] = pdpte_pdpte_pd_new(
                                                           0, /* xd */
                                                           kpptr_مقصد_paddr(&x64KSKernelPDs[0][0]),
                                                           0, /* accessed */
                                                           0, /* cache disable */
                                                           1, /* write through */
                                                           0, /* super user */
                                                           1, /* read write */
                                                           1  /* present */
                                                       );

    paddr = 0;
    vaddr = PPTR_پایه;

    لكل (paddr = 0; paddr < PADDR_بالاترین;
         paddr += 0x200000) {

        صحيح pd_شاخص_2 = GET_PDPT_شاخص(vaddr) - GET_PDPT_شاخص(PPTR_پایه);
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
    x64KSKernelPDPT[GET_PDPT_شاخص(KDEV_پایه)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_مقصد_paddr(&x64KSKernelPDs[بیت(PDPT_شاخص_بیتها) - 1][0]),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    /* put the PT into the PD */
    x64KSKernelPDs[بیت(PDPT_شاخص_بیتها) - 1][0] = pde_pde_pt_new(
                                                      0, /* xd */
                                                      kpptr_مقصد_paddr(x64KSKernelPT),
                                                      0, /* accessed */
                                                      0, /* cache_disabled */
                                                      0, /* write_through */
                                                      0, /* super_user */
                                                      1, /* read_write */
                                                      1  /* present */
                                                  );
#پایان_اگر

#إذا CONFIG_MAX_رقم_TRACE_POINTS > 0
    /* use the last PD entry as the benchmark log storage.
     * the actual backing physical memory will be filled
     * later by using alloc_region */
    ksLog = (ks_log_مدخل_t *)(KDEV_پایه + 0x200000 * (بیت(PD_شاخص_بیتها) - 1));
#پایان_اگر

    /* now map in the kernel devices */
    إذا (!map_kernel_پنجره_devices(x64KSKernelPT, رقم_ioapic, ioapic_paddrs, رقم_drhu, drhu_list)) {
        أرجع false;
    }

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(x64KSKernelPML4);
#پایان_اگر /* ENABLE_SMP_SUPPORT */

    /* In boot code, so fine to just trash everything here */
    invalidateLocalTranslationAll();
    چاپ_کن("Mapping kernel window is done\n");
    أرجع true;
}

#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
BOOT_کد bool_t map_skim_پنجره(vptr_t skim_شروع, vptr_t skim_انتها)
{
    /* place the PDPT into the PML4 */
    x64KSSKIMPML4[GET_PML4_شاخص(PPTR_پایه)] = pml4e_new(
                                                   0, /* xd */
                                                   kpptr_مقصد_paddr(x64KSSKIMPDPT),
                                                   0, /* accessed */
                                                   0, /* cache_disabled */
                                                   0, /* write_through */
                                                   0, /* super_user */
                                                   1, /* read_write */
                                                   1  /* present */
                                               );
    /* place the PD into the kernel_base slot of the PDPT */
    x64KSSKIMPDPT[GET_PDPT_شاخص(KERNEL_ELF_پایه)] = pdpte_pdpte_pd_new(
                                                         0, /* xd */
                                                         kpptr_مقصد_paddr(x64KSSKIMPD),
                                                         0, /* accessed */
                                                         0, /* cache_disabled */
                                                         0, /* write_through */
                                                         0, /* super_user */
                                                         1, /* read_write */
                                                         1  /* present */
                                                     );
    /* map the skim portion into the PD. we expect it to be 2M aligned */
    أكد((skim_شروع % بیت(se_L4_Large_صفحه_بیتها)) == 0);
    أكد((skim_انتها % بیت(se_L4_Large_صفحه_بیتها)) == 0);
    uint64_t paddr = kpptr_مقصد_paddr((فراغ *)skim_شروع);
    لكل (صحيح i = GET_PD_شاخص(skim_شروع); i < GET_PD_شاخص(skim_انتها); i++) {
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
        paddr += بیت(se_L4_Large_صفحه_بیتها);
    }
    أرجع true;
}
#پایان_اگر

BOOT_کد فراغ init_tss(tss_t *tss)
{
    کلمه_t پایه = (کلمه_t)&x64KSIRQStack[CURRENT_CPU_شاخص()][IRQ_پشته_اندازه];
    *tss = tss_new(
               حجم(*tss),   /* io map base */
               0, 0,       /* ist 7 */
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               /* ist 1 is the stack frame we use for interrupts */
               پایه >> 32, پایه & 0xffffffff,  /* ist 1 */
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
    x86_wrmsr(IA32_ستاره_MSR, ((uint64_t)SEL_CS_0 << 32) | ((uint64_t)SEL_CS_3 << 48));
}

BOOT_کد فراغ init_gdt(gdt_مدخل_t *gdt, tss_t *tss)
{

    uint64_t tss_پایه = (uint64_t)tss;
    gdt_tss_t gdt_tss;

    gdt[GDT_NULL] = gdt_مدخل_gdt_null_new();

    gdt[GDT_CS_0] = gdt_مدخل_gdt_کد_new(
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

    gdt[GDT_DS_0] = gdt_مدخل_gdt_داده_new(
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

    gdt[GDT_CS_3] = gdt_مدخل_gdt_کد_new(
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

    gdt[GDT_DS_3] = gdt_مدخل_gdt_داده_new(
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

    gdt[GDT_FS] = gdt_مدخل_gdt_داده_new(
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

    gdt[GDT_GS] = gdt_مدخل_gdt_داده_new(
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
                  tss_پایه >> 32,                     /* base 63 - 32 */
                  (tss_پایه & 0xff000000UL) >> 24,    /* base 31 - 24 */
                  1,                                  /* granularity */
                  0,                                  /* avl */
                  0,                                  /* limit high */
                  1,                                  /* present */
                  0,                                  /* dpl */
                  9,                                  /* desc type */
                  (tss_پایه & 0xff0000UL) >> 16,      /* base 23-16 */
                  (tss_پایه & 0xffffUL),              /* base 15 - 0 */
                  حجم(tss_io_t) - 1
              );

    gdt[GDT_TSS].واژگان[0] = gdt_tss.واژگان[0];
    gdt[GDT_TSS + 1].واژگان[0] = gdt_tss.واژگان[1];
}

BOOT_کد فراغ init_idt_مدخل(idt_مدخل_t *idt, وقفه_t وقفه, فراغ(*handler)(فراغ))
{
    uint64_t handler_addr = (uint64_t)handler;
    uint64_t dpl = 3;

    إذا (وقفه < int_trap_min && وقفه != int_software_break_request) {
        dpl = 0;
    }

    idt[وقفه] = idt_مدخل_وقفه_gate_new(
                         handler_addr >> 32,                 /* offset 63 - 32 */
                         ((handler_addr >> 16) & 0xffff),
                         1,                                  /* present */
                         dpl,                                /* dpl */
                         1,                                  /* ist */
                         SEL_CS_0,                           /* segment selector */
                         (handler_addr & 0xffff)               /* offset 15 - 0 */
                     );
}

فراغ مجموعه_VMRoot(tcb_t *tcb)
{
    cap_t thread_ریشه;
    asid_t asid;
    pml4e_t *pml4;
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;
    cr3_t cr3;

    thread_ریشه = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    إذا (cap_get_capType(thread_ریشه) != cap_pml4_cap ||
        !cap_pml4_cap_get_capPML4IsMapped(thread_ریشه)) {
        مجموعه_Current_User_VSpace_ریشه(kpptr_مقصد_paddr(X86_GLOBAL_VSPACE_ریشه), 0);
        أرجع;
    }

    pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_پایه_Ptr(thread_ریشه));
    asid = cap_pml4_cap_get_capPML4MappedASID(thread_ریشه);
    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (unlikely(پیدا_کن_ret.وضعیت != EXCEPTION_NONE || پیدا_کن_ret.vspace_ریشه != pml4)) {
        مجموعه_Current_User_VSpace_ریشه(kpptr_مقصد_paddr(X86_GLOBAL_VSPACE_ریشه), 0);
        أرجع;
    }
    cr3 = makeCR3(pptr_مقصد_paddr(pml4), asid);
    إذا (getCurrentUserCR3().واژگان[0] != cr3.واژگان[0]) {
        SMP_COND_STATEMENT(tlb_bitmap_مجموعه(pml4, getCurrentCPUIndex());)
        مجموعه_Current_User_CR3(cr3);
    }
}


BOOT_کد فراغ init_dtrs(فراغ)
{
    gdt_idt_ptr.حد = (حجم(gdt_مدخل_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.پایه = (uint64_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSgdt;

    /* When we install the gdt it will clobber any value of gs that
     * we have. Since we might be using it for TLS we can stash
     * and unstash any gs value using swapgs
     */
    swapgs();
    x64_install_gdt(&gdt_idt_ptr);
    swapgs();

    gdt_idt_ptr.حد = (حجم(idt_مدخل_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.پایه = (uint64_t)x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSidt;
    x64_install_idt(&gdt_idt_ptr);

    x64_install_ldt(SEL_NULL);

    x64_install_tss(SEL_TSS);
}

BOOT_کد فراغ map_it_قاب_cap(cap_t pd_cap, cap_t قاب_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_از_cap(pd_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt;
    vptr_t vptr = cap_قاب_cap_get_cap_FMapped_آدرس(قاب_cap);
    فراغ *pptr = (فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(قاب_cap);

    أكد(cap_قاب_cap_get_cap_FMap_Type(قاب_cap) == X86_MappingVSpace);
    أكد(cap_قاب_cap_get_cap_FMapped_ASID(قاب_cap) != asidInvalid);
    pml4 += GET_PML4_شاخص(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_پایه_آدرس(pml4));
    pdpt += GET_PDPT_شاخص(vptr);
    أكد(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_مقصد_pptr(pdpte_pdpte_pd_ptr_get_pd_پایه_آدرس(pdpt));
    pd += GET_PD_شاخص(vptr);
    أكد(pde_pde_pt_ptr_get_present(pd));
    pt = paddr_مقصد_pptr(pde_pde_pt_ptr_get_pt_پایه_آدرس(pd));
    *(pt + GET_PT_شاخص(vptr)) = pte_new(
                                     0,                      /* xd                   */
                                     pptr_مقصد_paddr(pptr),    /* page_base_address    */
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

ساكن BOOT_کد فراغ map_it_pdpt_cap(cap_t vspace_cap, cap_t pdpt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_از_cap(vspace_cap));
    pdpte_t *pdpt = PDPT_PTR(cap_pdpt_cap_get_capPDPTBasePtr(pdpt_cap));
    vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_آدرس(pdpt_cap);

    أكد(cap_pdpt_cap_get_capPDPTIsMapped(pdpt_cap));
    *(pml4 + GET_PML4_شاخص(vptr)) = pml4e_new(
                                         0,                      /* xd                   */
                                         pptr_مقصد_paddr(pdpt),    /* pdpt_base_address    */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_کد فراغ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_از_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd = PD_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    vptr_t vptr = cap_صفحه_directory_cap_get_cap_PDMapped_آدرس(pd_cap);

    أكد(cap_صفحه_directory_cap_get_cap_PDIs_Mapped(pd_cap));
    pml4 += GET_PML4_شاخص(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_پایه_آدرس(pml4));
    *(pdpt + GET_PDPT_شاخص(vptr)) = pdpte_pdpte_pd_new(
                                         0,                      /* xd                   */
                                         pptr_مقصد_paddr(pd),      /* pd_base_address      */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_کد فراغ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_از_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt = PT_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(pt_cap);

    أكد(cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(pt_cap));
    pml4 += GET_PML4_شاخص(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_پایه_آدرس(pml4));
    pdpt += GET_PDPT_شاخص(vptr);
    أكد(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_مقصد_pptr(pdpte_pdpte_pd_ptr_get_pd_پایه_آدرس(pdpt));
    *(pd + GET_PD_شاخص(vptr)) = pde_pde_pt_new(
                                     0,                      /* xd                   */
                                     pptr_مقصد_paddr(pt),      /* pt_base_address      */
                                     0,                      /* accessed             */
                                     0,                      /* cache_disabled       */
                                     0,                      /* write_through        */
                                     1,                      /* super_user           */
                                     1,                      /* read_write           */
                                     1                       /* present              */
                                 );
}

BOOT_کد فراغ *map_موقت_boot_صفحه(فراغ *مدخل, uint32_t large_صفحات)
{
    /* this function is for legacy 32-bit systems where the ACPI tables might
     * collide with the kernel window. Here we just assert that the table is
     * in fact in the lower 4GiB region (which is already 1-to-1 mapped) and
     * continue */
    أكد((کلمه_t)مدخل < بیت(32));
    أرجع مدخل;
}

ساكن BOOT_کد cap_t create_it_pdpt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
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

ساكن BOOT_کد cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحه_directory_cap_new(
              asid,   /* capPDMappedASID      */
              pptr,   /* capPDBasePtr         */
              1,      /* capPDIsMapped        */
              vptr    /* capPDMappedAddress   */
          );
    map_it_pd_cap(vspace_cap, cap);
    أرجع cap;
}

ساكن BOOT_کد cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحه_جدول_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );
    map_it_pt_cap(vspace_cap, cap);
    أرجع cap;
}


BOOT_کد کلمه_t arch_get_n_paging(v_region_t it_v_reg)
{
    کلمه_t n = get_n_paging(it_v_reg, PD_شاخص_جابه_جایی);
    n += get_n_paging(it_v_reg, PDPT_شاخص_جابه_جایی);
    n += get_n_paging(it_v_reg, PML4_شاخص_جابه_جایی);
#اگر_تعریف_شده CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#پایان_اگر
    أرجع n;
}

BOOT_کد cap_t create_it_آدرس_space(cap_t ریشه_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_جایگاه_Pos جایگاه_pos_before;
    se_L4_جایگاه_Pos جایگاه_pos_after;

    جایگاه_pos_before = ndks_boot.جایگاه_pos_cur;
    copyGlobalMappings(PML4_PTR(rootserver.vspace));
    vspace_cap = cap_pml4_cap_new(
                     IT_ASID,        /* capPML4MappedASID */
                     rootserver.vspace,           /* capPML4BasePtr   */
                     1               /* capPML4IsMapped   */
                 );


    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

    /* Create any PDPTs needed for the user land image */
    لكل (vptr = دور_پایین(it_v_reg.شروع, PML4_شاخص_جابه_جایی);
         vptr < it_v_reg.انتها;
         vptr += بیت(PML4_شاخص_جابه_جایی)) {
        إذا (!provide_cap(ریشه_cnode_cap,
                         create_it_pdpt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    /* Create any PDs needed for the user land image */
    لكل (vptr = دور_پایین(it_v_reg.شروع, PDPT_شاخص_جابه_جایی);
         vptr < it_v_reg.انتها;
         vptr += بیت(PDPT_شاخص_جابه_جایی)) {
        إذا (!provide_cap(ریشه_cnode_cap,
                         create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    لكل (vptr = دور_پایین(it_v_reg.شروع, PD_شاخص_جابه_جایی);
         vptr < it_v_reg.انتها;
         vptr += بیت(PD_شاخص_جابه_جایی)) {
        إذا (!provide_cap(ریشه_cnode_cap,
                         create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            أرجع cap_null_cap_new();
        }
    }

    جایگاه_pos_after = ndks_boot.جایگاه_pos_cur;
    ndks_boot.bi_قاب->userImagePaging = (se_L4_جایگاه_Region) {
        جایگاه_pos_before, جایگاه_pos_after
    };
    أرجع vspace_cap;
}

فراغ copyGlobalMappings(vspace_ریشه_t *new_vspace)
{
    بیعلامت دراز i;
    pml4e_t *vspace = (pml4e_t *)new_vspace;

    /* Copy from the tlbbitmap_pptr so that we copy the default entries of the
     * tlb bitmap (if it exists). If it doesn't exist then this loop
     * will be equivalent to copying from PPTR_BASE
     */
    لكل (i = GET_PML4_شاخص(TLBBITMAP_PPTR); i < بیت(PML4_شاخص_بیتها); i++) {
        vspace[i] = X86_GLOBAL_VSPACE_ریشه[i];
    }
}

ساكن BOOT_کد cap_t create_it_قاب_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large, se_L4_کلمه map_type)
{
    vm_صفحه_اندازه_t قاب_اندازه_2;

    إذا (use_large) {
        قاب_اندازه_2 = X86_Large_صفحه;
    } وإلا {
        قاب_اندازه_2 = X86_Small_صفحه;
    }

    أرجع
        cap_قاب_cap_new(
            asid,                          /* capFMappedASID     */
            pptr,                          /* capFBasePtr        */
            قاب_اندازه_2,                    /* capFSize           */
            map_type,                      /* capFMapType        */
            vptr,                          /* capFMappedAddress  */
            کلمه_From_VMRights(VMReadWrite), /* capFVMRights       */
            0                              /* capFIsDevice       */
        );
}

BOOT_کد cap_t create_unmapped_it_قاب_cap(pptr_t pptr, bool_t use_large)
{
    أرجع create_it_قاب_cap(pptr, 0, asidInvalid, use_large, X86_MappingNone);
}

BOOT_کد cap_t create_mapped_it_قاب_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large,
                                           bool_t executable استفادهنشده)
{
    cap_t cap = create_it_قاب_cap(pptr, vptr, asid, use_large, X86_MappingVSpace);
    map_it_قاب_cap(vspace_cap, cap);
    أرجع cap;
}

/* ====================== BOOT CODE FINISHES HERE ======================== */



exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_جایگاه)
{
    asid_map_t asid_map;
#اگر_تعریف_شده CONFIG_VTX
    إذا (cap_get_capType(vspace_Cap_جایگاه->cap) == cap_ept_pml4_cap) {
        cap_ept_pml4_cap_ptr_مجموعه_cap_PML4_Mapped_ASID(&vspace_Cap_جایگاه->cap, asid);
        cap_ept_pml4_cap_ptr_مجموعه_cap_PML4_Is_Mapped(&vspace_Cap_جایگاه->cap, 1);
        asid_map = asid_map_asid_map_ept_new(cap_ept_pml4_cap_get_cap_PML4_پایه_Ptr(vspace_Cap_جایگاه->cap));
    } وإلا
#پایان_اگر
    {
        أكد(cap_get_capType(vspace_Cap_جایگاه->cap) == cap_pml4_cap);
        cap_pml4_cap_ptr_مجموعه_cap_PML4_Mapped_ASID(&vspace_Cap_جایگاه->cap, asid);
        cap_pml4_cap_ptr_مجموعه_cap_PML4_Is_Mapped(&vspace_Cap_جایگاه->cap, 1);
        asid_map = asid_map_asid_map_vspace_new(cap_pml4_cap_get_cap_PML4_پایه_Ptr(vspace_Cap_جایگاه->cap));
    }
    poolPtr->array[ASID_کم(asid)] = asid_map;
    أرجع EXCEPTION_NONE;
}

bool_t CONST is_VTable_ریشه(cap_t cap)
{
    أرجع cap_get_capType(cap) == cap_pml4_cap;
}

bool_t CONST is_Valid_Native_ریشه(cap_t cap)
{
    أرجع is_VTable_ریشه(cap) &&
           cap_pml4_cap_get_capPML4IsMapped(cap);
}

ساكن pml4e_t CONST makeUserPML4E(paddr_t paddr, vm_صفات_t vm_attr)
{
    أرجع pml4e_new(
               0,
               paddr,
               0,
               vm_صفات_get_x86_PCDBit(vm_attr),
               vm_صفات_get_x86_PWTBit(vm_attr),
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

ساكن pdpte_t CONST make_User_PDPTEHuge_صفحه(paddr_t paddr, vm_صفات_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pdpte_pdpte_1g_new(
               0,          /* xd               */
               paddr,      /* physical address */
               0,          /* PAT              */
               0,          /* global           */
               0,          /* dirty            */
               0,          /* accessed         */
               vm_صفات_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_صفات_get_x86_PWTBit(vm_attr),  /* write through  */
               SuperUserFromVMRights(vm_rights),       /* super user     */
               WritableFromVMRights(vm_rights),        /* read write     */
               1                                       /* present        */
           );
}

ساكن pdpte_t CONST makeUserPDPTEPageDirectory(paddr_t paddr, vm_صفات_t vm_attr)
{
    أرجع pdpte_pdpte_pd_new(
               0,                      /* xd       */
               paddr,                  /* paddr    */
               0,                      /* accessed */
               vm_صفات_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_صفات_get_x86_PWTBit(vm_attr),  /* write through  */
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

pde_t CONST make_User_PDELarge_صفحه(paddr_t paddr, vm_صفات_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pde_pde_large_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               vm_صفات_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_صفات_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_صفات_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_جدول(paddr_t paddr, vm_صفات_t vm_attr)
{

    أرجع  pde_pde_pt_new(
                0,                                  /* xd               */
                paddr,                              /* pt_base_address  */
                0,                                  /* accessed         */
                vm_صفات_get_x86_PCDBit(vm_attr), /* cache_disabled   */
                vm_صفات_get_x86_PWTBit(vm_attr), /* write_through    */
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_صفات_t vm_attr, vm_rights_t vm_rights)
{
    أرجع pte_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               0,                                              /* global               */
               vm_صفات_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_صفات_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_صفات_get_x86_PWTBit(vm_attr),          /* write_through        */
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


ساكن pml4e_t *lookup_PML4_جایگاه(vspace_ریشه_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4e = PML4E_PTR(pml4);
    کلمه_t pml4_شاخص = GET_PML4_شاخص(vptr);
    أرجع pml4e + pml4_شاخص;
}

ساكن lookupPDPTSlot_ret_t lookupPDPTSlot(vspace_ریشه_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4_جایگاه = lookup_PML4_جایگاه(pml4, vptr);
    lookupPDPTSlot_ret_t ret;

    إذا (!pml4e_ptr_get_present(pml4_جایگاه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PML4_شاخص_جابه_جایی);

        ret.pdpt_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pdpte_t *pdpt;
        pdpte_t *pdpt_جایگاه;
        کلمه_t pdpt_شاخص = GET_PDPT_شاخص(vptr);
        pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_پایه_آدرس(pml4_جایگاه));
        pdpt_جایگاه = pdpt + pdpt_شاخص;

        ret.وضعیت = EXCEPTION_NONE;
        ret.pdpt_جایگاه = pdpt_جایگاه;
        أرجع ret;
    }
}

lookupPDSlot_ret_t lookupPDSlot(vspace_ریشه_t *pml4, vptr_t vptr)
{
    lookupPDPTSlot_ret_t pdpt_جایگاه;
    lookupPDSlot_ret_t ret;

    pdpt_جایگاه = lookupPDPTSlot(pml4, vptr);

    إذا (pdpt_جایگاه.وضعیت != EXCEPTION_NONE) {
        ret.pd_جایگاه = عدم;
        ret.وضعیت = pdpt_جایگاه.وضعیت;
        أرجع ret;
    }
    إذا ((pdpte_ptr_get_صفحه_اندازه(pdpt_جایگاه.pdpt_جایگاه) != pdpte_pdpte_pd) ||
        !pdpte_pdpte_pd_ptr_get_present(pdpt_جایگاه.pdpt_جایگاه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PDPT_شاخص_جابه_جایی);

        ret.pd_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pde_t *pd;
        pde_t *pd_جایگاه;
        کلمه_t pd_شاخص = GET_PD_شاخص(vptr);
        pd = paddr_مقصد_pptr(pdpte_pdpte_pd_ptr_get_pd_پایه_آدرس(pdpt_جایگاه.pdpt_جایگاه));
        pd_جایگاه = pd + pd_شاخص;

        ret.وضعیت = EXCEPTION_NONE;
        ret.pd_جایگاه = pd_جایگاه;
        أرجع ret;
    }
}

ساكن فراغ flushPD(vspace_ریشه_t *vspace, کلمه_t vptr, pde_t *pd, asid_t asid)
{
    /* clearing the entire PCID vs flushing the virtual addresses
     * one by one using invplg.
     * choose the easy way, invalidate the PCID
     */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));

}

ساكن فراغ flushPDPT(vspace_ریشه_t *vspace, کلمه_t vptr, pdpte_t *pdpt, asid_t asid)
{
    /* similar here */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع;
}

فراغ hwASIDInvalidate(asid_t asid, vspace_ریشه_t *vspace)
{
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
}

فراغ unmap_صفحه_Directory(asid_t asid, vptr_t vaddr, pde_t *pd)
{
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;
    lookupPDPTSlot_ret_t    lu_ret;

    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPDPTSlot(پیدا_کن_ret.vspace_ریشه, vaddr);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        أرجع;
    }

    /* check if the PDPT has the PD */
    إذا (!(pdpte_ptr_get_صفحه_اندازه(lu_ret.pdpt_جایگاه) == pdpte_pdpte_pd &&
          pdpte_pdpte_pd_ptr_get_present(lu_ret.pdpt_جایگاه) &&
          (pdpte_pdpte_pd_ptr_get_pd_پایه_آدرس(lu_ret.pdpt_جایگاه) == pptr_مقصد_paddr(pd)))) {
        أرجع;
    }

    flushPD(پیدا_کن_ret.vspace_ریشه, vaddr, pd, asid);

    *lu_ret.pdpt_جایگاه = makeUserPDPTEInvalid();

    invalidate_صفحه_Structure_Cache_ASID(pptr_مقصد_paddr(پیدا_کن_ret.vspace_ریشه), asid,
                                     SMP_TERNARY(tlb_bitmap_get(پیدا_کن_ret.vspace_ریشه), 0));
}


ساكن exception_t perform_X64_صفحه_Directory_Invocation_Unmap(cap_t cap, cte_t *ct_جایگاه)
{

    إذا (cap_صفحه_directory_cap_get_cap_PDIs_Mapped(cap)) {
        pde_t *pd = PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(cap));
        unmap_صفحه_Directory(
            cap_صفحه_directory_cap_get_cap_PDMapped_ASID(cap),
            cap_صفحه_directory_cap_get_cap_PDMapped_آدرس(cap),
            pd
        );
        clear_حافظه((فراغ *)pd, cap_get_cap_اندازه_بیتها(cap));
    }

    cap_صفحه_directory_cap_ptr_مجموعه_cap_PDIs_Mapped(&(ct_جایگاه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X64_صفحه_Directory_Invocation_Map(cap_t cap, cte_t *ct_جایگاه, pdpte_t pdpte, pdpte_t *pdpt_جایگاه,
                                                        vspace_ریشه_t *vspace)
{
    ct_جایگاه->cap = cap;
    *pdpt_جایگاه = pdpte;
    invalidate_صفحه_Structure_Cache_ASID(pptr_مقصد_paddr(vspace), cap_صفحه_directory_cap_get_cap_PDMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}


ساكن exception_t decode_X64_صفحه_Directory_Invocation(
    کلمه_t label,
    کلمه_t درازی,
    cte_t *cte,
    cap_t cap,
    کلمه_t *میانگیر
)
{
    کلمه_t              vaddr;
    vm_صفات_t     vm_attr;
    cap_t               vspaceCap;
    vspace_ریشه_t      *vspace;
    pdpte_t             pdpte;
    paddr_t             paddr;
    asid_t              asid;
    lookupPDPTSlot_ret_t pdpt_جایگاه;

    إذا (label == X86_صفحه_Directory_Unmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            userError("X86PageDirectory: Cannot unmap if more than one cap exist.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);

        أرجع perform_X64_صفحه_Directory_Invocation_Unmap(cap, cte);
    }

    إذا (label != X86_صفحه_Directory_Map) {
        userError("X64Directory: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (درازی < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X64PageDirectory: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_صفحه_directory_cap_get_cap_PDIs_Mapped(cap)) {
        userError("X64PageDirectory: PD is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, میانگیر) & (~ماسک(PDPT_شاخص_جابه_جایی));
    vm_attr = vm_صفات_From_کلمه(getSyscallArg(1, میانگیر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    إذا (!is_Valid_Native_ریشه(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ریشه_t *)pptr_از_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    إذا (vaddr > USER_بالاترین) {
        userError("X64PageDirectory: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;

    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کن_ret.vspace_ریشه != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pdpt_جایگاه = lookupPDPTSlot(vspace, vaddr);
    إذا (pdpt_جایگاه.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا ((pdpte_ptr_get_صفحه_اندازه(pdpt_جایگاه.pdpt_جایگاه) == pdpte_pdpte_pd &&
         pdpte_pdpte_pd_ptr_get_present(pdpt_جایگاه.pdpt_جایگاه)) ||
        (pdpte_ptr_get_صفحه_اندازه(pdpt_جایگاه.pdpt_جایگاه) == pdpte_pdpte_1g
         && pdpte_pdpte_1g_ptr_get_present(pdpt_جایگاه.pdpt_جایگاه))) {
        current_syscall_error.type = se_L4_Delete_اولی;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr(PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(cap)));
    pdpte = makeUserPDPTEPageDirectory(paddr, vm_attr);

    cap = cap_صفحه_directory_cap_مجموعه_cap_PDIs_Mapped(cap, 1);
    cap = cap_صفحه_directory_cap_مجموعه_cap_PDMapped_ASID(cap, asid);
    cap = cap_صفحه_directory_cap_مجموعه_cap_PDMapped_آدرس(cap, vaddr);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_X64_صفحه_Directory_Invocation_Map(cap, cte, pdpte, pdpt_جایگاه.pdpt_جایگاه, vspace);
}

ساكن فراغ unmapPDPT(asid_t asid, vptr_t vaddr, pdpte_t *pdpt)
{
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;
    pml4e_t *pml4_جایگاه;

    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        أرجع;
    }

    pml4_جایگاه = lookup_PML4_جایگاه(پیدا_کن_ret.vspace_ریشه, vaddr);

    /* check if the PML4 has the PDPT */
    إذا (!(pml4e_ptr_get_present(pml4_جایگاه) &&
          pml4e_ptr_get_pdpt_پایه_آدرس(pml4_جایگاه) == pptr_مقصد_paddr(pdpt))) {
        أرجع;
    }

    flushPDPT(پیدا_کن_ret.vspace_ریشه, vaddr, pdpt, asid);

    *pml4_جایگاه = makeUserPML4EInvalid();
}

ساكن exception_t performX64PDPTInvocationUnmap(cap_t cap, cte_t *ct_جایگاه)
{
    إذا (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        pdpte_t *pdpt = PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap));
        unmapPDPT(cap_pdpt_cap_get_capPDPTMappedASID(cap),
                  cap_pdpt_cap_get_cap_PDPTMapped_آدرس(cap),
                  pdpt);
        clear_حافظه((فراغ *)pdpt, cap_get_cap_اندازه_بیتها(cap));
    }

    cap_pdpt_cap_ptr_مجموعه_cap_PDPTIs_Mapped(&(ct_جایگاه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t performX64PDPTInvocationMap(cap_t cap, cte_t *ct_جایگاه, pml4e_t pml4e, pml4e_t *pml4_جایگاه,
                                               vspace_ریشه_t *vspace)
{
    ct_جایگاه->cap = cap;
    *pml4_جایگاه = pml4e;
    invalidate_صفحه_Structure_Cache_ASID(pptr_مقصد_paddr(vspace), cap_pdpt_cap_get_capPDPTMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeX64PDPTInvocation(
    کلمه_t  label,
    کلمه_t درازی,
    cte_t   *cte,
    cap_t   cap,
    کلمه_t  *میانگیر)
{
    کلمه_t                  vaddr;
    vm_صفات_t         attr;
    pml4e_t                *pml4_جایگاه;
    cap_t                   vspaceCap;
    vspace_ریشه_t          *vspace;
    pml4e_t                 pml4e;
    paddr_t                 paddr;
    asid_t                  asid;

    إذا (label == X86PDPTUnmap) {
        إذا (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            userError("X86PDPT: Cannot unmap if more than one cap exist.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);

        أرجع performX64PDPTInvocationUnmap(cap, cte);
    }

    إذا (label != X86PDPTMap) {
        userError("X86PDPT: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (درازی < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X64PDPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X64PDPT: PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, میانگیر) & (~ماسک(PML4_شاخص_جابه_جایی));
    attr = vm_صفات_From_کلمه(getSyscallArg(1, میانگیر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    إذا (!is_Valid_Native_ریشه(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ریشه_t *)pptr_از_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    إذا (vaddr > USER_بالاترین) {
        userError("X64PDPT: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;

    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (پیدا_کن_ret.vspace_ریشه != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    pml4_جایگاه = lookup_PML4_جایگاه(vspace, vaddr);

    إذا (pml4e_ptr_get_present(pml4_جایگاه)) {
        current_syscall_error.type = se_L4_Delete_اولی;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr(PDPTE_PTR((cap_pdpt_cap_get_capPDPTBasePtr(cap))));
    pml4e = makeUserPML4E(paddr, attr);

    cap = cap_pdpt_cap_مجموعه_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_pdpt_cap_مجموعه_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_pdpt_cap_مجموعه_cap_PDPTMapped_آدرس(cap, vaddr);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع performX64PDPTInvocationMap(cap, cte, pml4e, pml4_جایگاه, vspace);
}

exception_t decode_X86_نما_MMUInvocation(
    کلمه_t label,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    کلمه_t *میانگیر
)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_pml4_cap:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;

    حالة cap_pdpt_cap:
        أرجع decodeX64PDPTInvocation(label, درازی, cte, cap, میانگیر);

    حالة cap_صفحه_directory_cap:
        أرجع decode_X64_صفحه_Directory_Invocation(label, درازی, cte, cap, میانگیر);

    افتراضي:
        fail("Invalid arch cap type");
    }
}

bool_t نما_Unmap_صفحه(vm_صفحه_اندازه_t صفحه_اندازه, vspace_ریشه_t *vroot, vptr_t vaddr, فراغ *pptr)
{
    إذا (config_مجموعه(CONFIG_HUGE_صفحه) && صفحه_اندازه == X64_Huge_صفحه) {
        pdpte_t     *pdpte;
        lookupPDPTSlot_ret_t pdpt_ret = lookupPDPTSlot(vroot, vaddr);
        إذا (pdpt_ret.وضعیت != EXCEPTION_NONE) {
            أرجع false;
        }
        pdpte = pdpt_ret.pdpt_جایگاه;


        إذا (!(pdpte_ptr_get_صفحه_اندازه(pdpte) == pdpte_pdpte_1g
              && pdpte_pdpte_1g_ptr_get_present(pdpte)
              && (pdpte_pdpte_1g_ptr_get_صفحه_پایه_آدرس(pdpte)
                  == pptr_مقصد_paddr(pptr)))) {
            أرجع false;
        }

        *pdpte = makeUserPDPTEInvalid();
        أرجع true;
    }
    fail("Invalid page type");
    أرجع false;
}

ساكن exception_t updatePDPTE(asid_t asid, pdpte_t pdpte, pdpte_t *pdpt_جایگاه, vspace_ریشه_t *vspace)
{
    *pdpt_جایگاه = pdpte;
    invalidate_صفحه_Structure_Cache_ASID(pptr_مقصد_paddr(vspace), asid,
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X64_نما_Map(cap_t cap, cte_t *ct_جایگاه, pdpte_t pdpte, pdpte_t *pdpt_جایگاه, vspace_ریشه_t *vspace)
{
    ct_جایگاه->cap = cap;
    أرجع updatePDPTE(cap_قاب_cap_get_cap_FMapped_ASID(cap), pdpte, pdpt_جایگاه, vspace);
}

بنية create_mapping_pdpte_return {
    exception_t وضعیت;
    pdpte_t pdpte;
    pdpte_t *pdpt_جایگاه;
};
تعريفنوع بنية create_mapping_pdpte_return create_mapping_pdpte_return_t;

ساكن create_mapping_pdpte_return_t create_امن_Mapping_Entries_PDPTE(paddr_t پایه, کلمه_t vaddr, vm_rights_t vmRights,
                                                                    vm_صفات_t attr,
                                                                    vspace_ریشه_t *vspace)
{
    create_mapping_pdpte_return_t ret;
    lookupPDPTSlot_ret_t          lu_ret;

    lu_ret = lookupPDPTSlot(vspace, vaddr);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        أرجع ret;
    }
    ret.pdpt_جایگاه = lu_ret.pdpt_جایگاه;

    /* check for existing page directory */
    إذا ((pdpte_ptr_get_صفحه_اندازه(ret.pdpt_جایگاه) == pdpte_pdpte_pd) &&
        (pdpte_pdpte_pd_ptr_get_present(ret.pdpt_جایگاه))) {
        current_syscall_error.type = se_L4_Delete_اولی;
        ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    ret.pdpte = make_User_PDPTEHuge_صفحه(پایه, attr, vmRights);
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

exception_t decode_X86_نما_Map_صفحه(کلمه_t label, vm_صفحه_اندازه_t صفحه_اندازه, cte_t *cte, cap_t cap,
                                 vspace_ریشه_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_صفات_t vm_attr)
{
    إذا (config_مجموعه(CONFIG_HUGE_صفحه) && صفحه_اندازه == X64_Huge_صفحه) {
        create_mapping_pdpte_return_t map_ret;

        map_ret = create_امن_Mapping_Entries_PDPTE(paddr, vaddr, vm_rights, vm_attr, vroot);
        إذا (map_ret.وضعیت != EXCEPTION_NONE) {
            أرجع map_ret.وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);

        بدل (label) {
        حالة X86_صفحه_Map:
            أرجع perform_X64_نما_Map(cap, cte, map_ret.pdpte, map_ret.pdpt_جایگاه, vroot);

        افتراضي:
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }
    fail("Invalid Page type");
}

#اگر_تعریف_شده CONFIG_PRINTING
تعريفنوع بنية read_کلمه_From_VSpace_ret {
    exception_t وضعیت;
    کلمه_t مقدار;
} read_کلمه_From_VSpace_ret_t;

ساكن read_کلمه_From_VSpace_ret_t read_کلمه_From_VSpace(vspace_ریشه_t *vspace, کلمه_t vaddr)
{
    read_کلمه_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_جایگاه;
    lookupPDSlot_ret_t pd_جایگاه;
    lookupPDPTSlot_ret_t pdpt_جایگاه;
    paddr_t paddr;
    کلمه_t جابه_جایی;
    pptr_t kernel_vaddr;
    کلمه_t *مقدار;

    pdpt_جایگاه = lookupPDPTSlot(vspace, vaddr);
    إذا (pdpt_جایگاه.وضعیت == EXCEPTION_NONE &&
        pdpte_ptr_get_صفحه_اندازه(pdpt_جایگاه.pdpt_جایگاه) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_جایگاه.pdpt_جایگاه)) {

        paddr = pdpte_pdpte_1g_ptr_get_صفحه_پایه_آدرس(pdpt_جایگاه.pdpt_جایگاه);
        جابه_جایی = vaddr & ماسک(se_L4_Huge_صفحه_بیتها);
    } وإلا {
        pd_جایگاه = lookupPDSlot(vspace, vaddr);
        إذا (pd_جایگاه.وضعیت == EXCEPTION_NONE &&
            ((pde_ptr_get_صفحه_اندازه(pd_جایگاه.pd_جایگاه) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_جایگاه.pd_جایگاه))) {

            paddr = pde_pde_large_ptr_get_صفحه_پایه_آدرس(pd_جایگاه.pd_جایگاه);
            جابه_جایی = vaddr & ماسک(se_L4_Large_صفحه_بیتها);
        } وإلا {
            pt_جایگاه = lookupPTSlot(vspace, vaddr);
            إذا (pt_جایگاه.وضعیت == EXCEPTION_NONE && pte_ptr_get_present(pt_جایگاه.pt_جایگاه)) {
                paddr = pte_ptr_get_صفحه_پایه_آدرس(pt_جایگاه.pt_جایگاه);
                جابه_جایی = vaddr & ماسک(se_L4_صفحه_بیتها);
            } وإلا {
                ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
                أرجع ret;
            }
        }
    }


    kernel_vaddr = (کلمه_t)paddr_مقصد_pptr(paddr);
    مقدار = (کلمه_t *)(kernel_vaddr + جابه_جایی);
    ret.وضعیت = EXCEPTION_NONE;
    ret.مقدار = *مقدار;
    أرجع ret;
}

فراغ Arch_user_پشته_Trace(tcb_t *tptr)
{
    cap_t thread_ریشه;
    vspace_ریشه_t *vspace_ریشه;
    کلمه_t sp;
    صحيح i;

    thread_ریشه = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    إذا (cap_get_capType(thread_ریشه) != cap_pml4_cap) {
        چاپ_کن("Invalid vspace\n");
        أرجع;
    }

    vspace_ریشه = (vspace_ریشه_t *)pptr_از_cap(thread_ریشه);

    sp = get_ثبات(tptr, RSP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    إذا (!IS_ALIGNED(sp, se_L4_کلمه_اندازه_بیتها)) {
        چاپ_کن("RSP not aligned\n");
        أرجع;
    }

    لكل (i = 0; i < CONFIG_USER_پشته_TRACE_درازی; i++) {
        کلمه_t آدرس = sp + (i * حجم(کلمه_t));
        read_کلمه_From_VSpace_ret_t نتیجه;
        نتیجه = read_کلمه_From_VSpace(vspace_ریشه, آدرس);
        إذا (نتیجه.وضعیت == EXCEPTION_NONE) {
            چاپ_کن("0x%lx: 0x%lx\n", (دراز)آدرس, (دراز)نتیجه.مقدار);
        } وإلا {
            چاپ_کن("0x%lx: INVALID\n", (دراز)آدرس);
        }
    }
}
#پایان_اگر /* CONFIG_PRINTING */

#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
exception_t benchmark_arch_map_log_میانگیر(کلمه_t قاب_cptr)
{
    lookup_Cap_And_جایگاه_ret_t lu_ret;
    vm_صفحه_اندازه_t قاب_اندازه;
    pptr_t قاب_pptr;

    /* faulting section */
    lu_ret = lookup_Cap_And_جایگاه(گره_STATE(ksCurThread), قاب_cptr);

    إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
        userError("Invalid cap #%lu.", قاب_cptr);
        current_fault = seL4_Fault_CapFault_new(قاب_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_get_capType(lu_ret.cap) != cap_قاب_cap) {
        userError("Invalid cap. Log buffer should be of a frame cap");
        current_fault = seL4_Fault_CapFault_new(قاب_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    قاب_اندازه = cap_قاب_cap_get_cap_FSize(lu_ret.cap);

    إذا (قاب_اندازه != X86_Large_صفحه) {
        userError("Invalid size for log Buffer. The kernel expects at least 1M log buffer");
        current_fault = seL4_Fault_CapFault_new(قاب_cptr, false);

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    قاب_pptr = cap_قاب_cap_get_cap_FBase_Ptr(lu_ret.cap);

    ks_User_Log_میانگیر = pptr_مقصد_paddr((فراغ *) قاب_pptr);

    pde_t pde = pde_pde_large_new(
                    0,                 /* xd                   */
                    ks_User_Log_میانگیر,   /* page_base_address    */
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
#اگر_تعریف_شده CONFIG_HUGE_صفحه
    x64KSKernelPD[1] = pde;
#وإلا
    x64KSKernelPDs[بیت(PDPT_شاخص_بیتها) - 1][1] = pde;
#پایان_اگر
    invalidateTranslationAll(ماسک(CONFIG_MAX_رقم_NODES));

    أرجع EXCEPTION_NONE;
}
#پایان_اگر /* CONFIG_KERNEL_LOG_BUFFER */
