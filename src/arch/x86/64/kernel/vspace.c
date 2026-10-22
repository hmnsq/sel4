/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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
#شامل <mode/kernel/tlb.h>
#شامل <arch/kernel/tlb_bitmap.h>
#شامل <object/structures.h>

/* When using the SKIM window to isolate the kernel from the user we also need to
 * not use global mappings as having global mappings and entries in the TLB is
 * equivalent, for the purpose of exploitation, to having the mappings in the
 * kernel window */
#تعریف_کرو KERNEL_IS_GLOBAL() (config_سیٹ(CONFIG_KERNEL_SKIM_کھڑکی) ? 0 : 1)

/* For the boot code we create two windows into the physical address space
 * One is at the same location as the kernel window, and is placed up high
 * The other is a 1-to-1 mapping of the first 512gb of memory. The purpose
 * of this is to have a 1-to-1 mapping for the low parts of memory, so that
 * when we switch paging on, and are still running at physical addresses,
 * we don't explode. Then we also want the high mappings so we can start
 * running at proper kernel virtual addresses */
pml4e_t boot_pml4[ثنائی_ہندسہ(PML4_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_بٹ)) VISIBLE PHYS_BSS;
pdpte_t boot_pdpt[ثنائی_ہندسہ(PDPT_اشاریہ_بٹ)] ALIGN(ثنائی_ہندسہ(se_L4_صفحہ_بٹ)) VISIBLE PHYS_BSS;

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

BOOT_رمز bool_t map_kernel_کھڑکی(
    uint32_t عدد_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   عدد_drhu,
    paddr_t   *drhu_list
)
{

    uint64_t paddr;
    uint64_t vaddr;

#اگر_تعریف_شدہ CONFIG_HUGE_صفحہ
    /* using 1 GiB page size */

    /* verify that the kernel window as at the last entry of the PML4 */
    أكد(GET_PML4_اشاریہ(PPTR_بنیاد) == ثنائی_ہندسہ(PML4_اشاریہ_بٹ) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    أكد(GET_PML4_اشاریہ(KERNEL_ELF_بنیاد) == ثنائی_ہندسہ(PML4_اشاریہ_بٹ) - 1);
    أكد(GET_PDPT_اشاریہ(KERNEL_ELF_بنیاد) == ثنائی_ہندسہ(PML4_اشاریہ_بٹ) - 2);
    أكد(GET_PDPT_اشاریہ(KDEV_بنیاد) == ثنائی_ہندسہ(PML4_اشاریہ_بٹ) - 1);
    أكد(IS_ALIGNED(KERNEL_ELF_بنیاد - KERNEL_ELF_PADDR_بنیاد, se_L4_Huge_صفحہ_بٹ));
    أكد(IS_ALIGNED(KDEV_بنیاد, se_L4_Huge_صفحہ_بٹ));
    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_اشاریہ(PPTR_بنیاد)] = pml4e_new(
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
    x64KSKernelPDPT[GET_PDPT_اشاریہ(KERNEL_ELF_بنیاد)] = pdpte_pdpte_1g_new(
                                                           0, /* xd */
                                                           PADDR_بنیاد,
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
    vaddr = PPTR_بنیاد;
    برائے (paddr = 0; paddr < PADDR_چوٹی;
         paddr += ثنائی_ہندسہ(se_L4_Huge_صفحہ_بٹ)) {

        صحیح pdpte_اشاریہ = GET_PDPT_اشاریہ(vaddr);
        x64KSKernelPDPT[pdpte_اشاریہ] = pdpte_pdpte_1g_new(
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

        vaddr += ثنائی_ہندسہ(se_L4_Huge_صفحہ_بٹ);
    }

    /* put the PD into the PDPT */
    x64KSKernelPDPT[GET_PDPT_اشاریہ(KDEV_بنیاد)] = pdpte_pdpte_pd_new(
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
#ورنہ

    صحیح pd_اشاریہ_2 = 0;
    /* use 2 MiB page size */
    /* verify that the kernel window as at the last entry of the PML4 */
    أكد(GET_PML4_اشاریہ(PPTR_بنیاد) == ثنائی_ہندسہ(PML4_اشاریہ_بٹ) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    أكد(GET_PML4_اشاریہ(KERNEL_ELF_بنیاد) == ثنائی_ہندسہ(PML4_اشاریہ_بٹ) - 1);
    أكد(GET_PDPT_اشاریہ(KERNEL_ELF_بنیاد) == ثنائی_ہندسہ(PML4_اشاریہ_بٹ) - 2);
    أكد(GET_PDPT_اشاریہ(KDEV_بنیاد) == ثنائی_ہندسہ(PML4_اشاریہ_بٹ) - 1);
    أكد(IS_ALIGNED(KERNEL_ELF_بنیاد - KERNEL_ELF_PADDR_بنیاد, se_L4_Huge_صفحہ_بٹ));
    أكد(IS_ALIGNED(KDEV_بنیاد, se_L4_Huge_صفحہ_بٹ));

    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_اشاریہ(PPTR_بنیاد)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_مقصد_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    برائے (pd_اشاریہ_2 = 0; pd_اشاریہ_2 < PADDR_چوٹی >> se_L4_Huge_صفحہ_بٹ; pd_اشاریہ_2++) {
        /* put the 1GB kernel_base mapping into the PDPT */
        x64KSKernelPDPT[GET_PDPT_اشاریہ(PPTR_بنیاد) + pd_اشاریہ_2] = pdpte_pdpte_pd_new(
                                                                    0, /* xd */
                                                                    kpptr_مقصد_paddr(&x64KSKernelPDs[pd_اشاریہ_2][0]),
                                                                    0, /* accessed */
                                                                    0, /* cache disabled */
                                                                    0, /* write through */
                                                                    0, /* super user */
                                                                    1, /* read write */
                                                                    1 /* present */
                                                                );
    }

    x64KSKernelPDPT[GET_PDPT_اشاریہ(KERNEL_ELF_بنیاد)] = pdpte_pdpte_pd_new(
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
    vaddr = PPTR_بنیاد;

    برائے (paddr = 0; paddr < PADDR_چوٹی;
         paddr += 0x200000) {

        صحیح pd_اشاریہ_2 = GET_PDPT_اشاریہ(vaddr) - GET_PDPT_اشاریہ(PPTR_بنیاد);
        صحیح pde_اشاریہ = GET_PD_اشاریہ(vaddr);

        x64KSKernelPDs[pd_اشاریہ_2][pde_اشاریہ] = pde_pde_large_new(
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
    x64KSKernelPDPT[GET_PDPT_اشاریہ(KDEV_بنیاد)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_مقصد_paddr(&x64KSKernelPDs[ثنائی_ہندسہ(PDPT_اشاریہ_بٹ) - 1][0]),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    /* put the PT into the PD */
    x64KSKernelPDs[ثنائی_ہندسہ(PDPT_اشاریہ_بٹ) - 1][0] = pde_pde_pt_new(
                                                      0, /* xd */
                                                      kpptr_مقصد_paddr(x64KSKernelPT),
                                                      0, /* accessed */
                                                      0, /* cache_disabled */
                                                      0, /* write_through */
                                                      0, /* super_user */
                                                      1, /* read_write */
                                                      1  /* present */
                                                  );
#اگر_ختم

#اگر CONFIG_MAX_عدد_TRACE_POINTS > 0
    /* use the last PD entry as the benchmark log storage.
     * the actual backing physical memory will be filled
     * later by using alloc_region */
    ksLog = (ks_log_اندراج_t *)(KDEV_بنیاد + 0x200000 * (ثنائی_ہندسہ(PD_اشاریہ_بٹ) - 1));
#اگر_ختم

    /* now map in the kernel devices */
    اگر (!map_kernel_کھڑکی_devices(x64KSKernelPT, عدد_ioapic, ioapic_paddrs, عدد_drhu, drhu_list)) {
        واپس false;
    }

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(x64KSKernelPML4);
#اگر_ختم /* ENABLE_SMP_SUPPORT */

    /* In boot code, so fine to just trash everything here */
    invalidateLocalTranslationAll();
    لکھو("Mapping kernel window is done\n");
    واپس true;
}

#اگر_تعریف_شدہ CONFIG_KERNEL_SKIM_کھڑکی
BOOT_رمز bool_t map_skim_کھڑکی(vptr_t skim_آغاز, vptr_t skim_انتہا)
{
    /* place the PDPT into the PML4 */
    x64KSSKIMPML4[GET_PML4_اشاریہ(PPTR_بنیاد)] = pml4e_new(
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
    x64KSSKIMPDPT[GET_PDPT_اشاریہ(KERNEL_ELF_بنیاد)] = pdpte_pdpte_pd_new(
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
    أكد((skim_آغاز % ثنائی_ہندسہ(se_L4_Large_صفحہ_بٹ)) == 0);
    أكد((skim_انتہا % ثنائی_ہندسہ(se_L4_Large_صفحہ_بٹ)) == 0);
    uint64_t paddr = kpptr_مقصد_paddr((خالی *)skim_آغاز);
    برائے (صحیح i = GET_PD_اشاریہ(skim_آغاز); i < GET_PD_اشاریہ(skim_انتہا); i++) {
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
        paddr += ثنائی_ہندسہ(se_L4_Large_صفحہ_بٹ);
    }
    واپس true;
}
#اگر_ختم

BOOT_رمز خالی init_tss(tss_t *tss)
{
    لفظ_t بنیاد = (لفظ_t)&x64KSIRQStack[CURRENT_CPU_اشاریہ()][IRQ_ڈھیر_سائز];
    *tss = tss_new(
               حجم(*tss),   /* io map base */
               0, 0,       /* ist 7 */
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               /* ist 1 is the stack frame we use for interrupts */
               بنیاد >> 32, بنیاد & 0xffffffff,  /* ist 1 */
               0, 0,       /* rsp 2 */
               0, 0,       /* rsp 1 */
               0, 0        /* rsp 0 */
           );
    /* set the IO map to all 1 to block user IN/OUT instructions */
    memset(&x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KStss.io_map[0], 0xff,
           حجم(x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KStss.io_map));
}

خالی init_syscall_msrs(خالی)
{
    x86_wrmsr(IA32_LSTAR_MSR, (uint64_t)&handle_fastsyscall);
    // mask bit 9 in the kernel (which is the interrupt enable bit)
    // also mask bit 8, which is the Trap Flag, to prevent the kernel
    // from single stepping
    x86_wrmsr(IA32_FMASK_MSR, FLAGS_TF | FLAGS_IF);
    x86_wrmsr(IA32_ستارہ_MSR, ((uint64_t)SEL_CS_0 << 32) | ((uint64_t)SEL_CS_3 << 48));
}

BOOT_رمز خالی init_gdt(gdt_اندراج_t *gdt, tss_t *tss)
{

    uint64_t tss_بنیاد = (uint64_t)tss;
    gdt_tss_t gdt_tss;

    gdt[GDT_NULL] = gdt_اندراج_gdt_null_new();

    gdt[GDT_CS_0] = gdt_اندراج_gdt_رمز_new(
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

    gdt[GDT_DS_0] = gdt_اندراج_gdt_ڈیٹا_new(
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

    gdt[GDT_CS_3] = gdt_اندراج_gdt_رمز_new(
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

    gdt[GDT_DS_3] = gdt_اندراج_gdt_ڈیٹا_new(
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

    gdt[GDT_FS] = gdt_اندراج_gdt_ڈیٹا_new(
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

    gdt[GDT_GS] = gdt_اندراج_gdt_ڈیٹا_new(
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
                  tss_بنیاد >> 32,                     /* base 63 - 32 */
                  (tss_بنیاد & 0xff000000UL) >> 24,    /* base 31 - 24 */
                  1,                                  /* granularity */
                  0,                                  /* avl */
                  0,                                  /* limit high */
                  1,                                  /* present */
                  0,                                  /* dpl */
                  9,                                  /* desc type */
                  (tss_بنیاد & 0xff0000UL) >> 16,      /* base 23-16 */
                  (tss_بنیاد & 0xffffUL),              /* base 15 - 0 */
                  حجم(tss_io_t) - 1
              );

    gdt[GDT_TSS].الفاظ[0] = gdt_tss.الفاظ[0];
    gdt[GDT_TSS + 1].الفاظ[0] = gdt_tss.الفاظ[1];
}

BOOT_رمز خالی init_idt_اندراج(idt_اندراج_t *idt, مداخلت_t مداخلت, خالی(*handler)(خالی))
{
    uint64_t handler_addr = (uint64_t)handler;
    uint64_t dpl = 3;

    اگر (مداخلت < int_trap_min && مداخلت != int_software_break_request) {
        dpl = 0;
    }

    idt[مداخلت] = idt_اندراج_مداخلت_gate_new(
                         handler_addr >> 32,                 /* offset 63 - 32 */
                         ((handler_addr >> 16) & 0xffff),
                         1,                                  /* present */
                         dpl,                                /* dpl */
                         1,                                  /* ist */
                         SEL_CS_0,                           /* segment selector */
                         (handler_addr & 0xffff)               /* offset 15 - 0 */
                     );
}

خالی سیٹ_VMRoot(tcb_t *tcb)
{
    cap_t thread_جڑ;
    asid_t asid;
    pml4e_t *pml4;
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;
    cr3_t cr3;

    thread_جڑ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    اگر (cap_get_capType(thread_جڑ) != cap_pml4_cap ||
        !cap_pml4_cap_get_capPML4IsMapped(thread_جڑ)) {
        سیٹ_Current_User_VSpace_جڑ(kpptr_مقصد_paddr(X86_GLOBAL_VSPACE_جڑ), 0);
        واپس;
    }

    pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنیاد_Ptr(thread_جڑ));
    asid = cap_pml4_cap_get_capPML4MappedASID(thread_جڑ);
    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (unlikely(ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE || ڈھونڈو_ret.vspace_جڑ != pml4)) {
        سیٹ_Current_User_VSpace_جڑ(kpptr_مقصد_paddr(X86_GLOBAL_VSPACE_جڑ), 0);
        واپس;
    }
    cr3 = makeCR3(pptr_مقصد_paddr(pml4), asid);
    اگر (getCurrentUserCR3().الفاظ[0] != cr3.الفاظ[0]) {
        SMP_COND_STATEMENT(tlb_bitmap_سیٹ(pml4, getCurrentCPUIndex());)
        سیٹ_Current_User_CR3(cr3);
    }
}


BOOT_رمز خالی init_dtrs(خالی)
{
    gdt_idt_ptr.حد_بندی = (حجم(gdt_اندراج_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.بنیاد = (uint64_t)x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KSgdt;

    /* When we install the gdt it will clobber any value of gs that
     * we have. Since we might be using it for TLS we can stash
     * and unstash any gs value using swapgs
     */
    swapgs();
    x64_install_gdt(&gdt_idt_ptr);
    swapgs();

    gdt_idt_ptr.حد_بندی = (حجم(idt_اندراج_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.بنیاد = (uint64_t)x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KSidt;
    x64_install_idt(&gdt_idt_ptr);

    x64_install_ldt(SEL_NULL);

    x64_install_tss(SEL_TSS);
}

BOOT_رمز خالی map_it_چوکھٹا_cap(cap_t pd_cap, cap_t چوکھٹا_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_کا_cap(pd_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt;
    vptr_t vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(چوکھٹا_cap);
    خالی *pptr = (خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(چوکھٹا_cap);

    أكد(cap_چوکھٹا_cap_get_cap_FMap_Type(چوکھٹا_cap) == X86_MappingVSpace);
    أكد(cap_چوکھٹا_cap_get_cap_FMapped_ASID(چوکھٹا_cap) != asidInvalid);
    pml4 += GET_PML4_اشاریہ(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_بنیاد_پتہ(pml4));
    pdpt += GET_PDPT_اشاریہ(vptr);
    أكد(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_مقصد_pptr(pdpte_pdpte_pd_ptr_get_pd_بنیاد_پتہ(pdpt));
    pd += GET_PD_اشاریہ(vptr);
    أكد(pde_pde_pt_ptr_get_present(pd));
    pt = paddr_مقصد_pptr(pde_pde_pt_ptr_get_pt_بنیاد_پتہ(pd));
    *(pt + GET_PT_اشاریہ(vptr)) = pte_new(
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

ساکن BOOT_رمز خالی map_it_pdpt_cap(cap_t vspace_cap, cap_t pdpt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_کا_cap(vspace_cap));
    pdpte_t *pdpt = PDPT_PTR(cap_pdpt_cap_get_capPDPTBasePtr(pdpt_cap));
    vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_پتہ(pdpt_cap);

    أكد(cap_pdpt_cap_get_capPDPTIsMapped(pdpt_cap));
    *(pml4 + GET_PML4_اشاریہ(vptr)) = pml4e_new(
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

BOOT_رمز خالی map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_کا_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd = PD_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    vptr_t vptr = cap_صفحہ_directory_cap_get_cap_PDMapped_پتہ(pd_cap);

    أكد(cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(pd_cap));
    pml4 += GET_PML4_اشاریہ(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_بنیاد_پتہ(pml4));
    *(pdpt + GET_PDPT_اشاریہ(vptr)) = pdpte_pdpte_pd_new(
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

BOOT_رمز خالی map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_کا_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt = PT_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(pt_cap);

    أكد(cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(pt_cap));
    pml4 += GET_PML4_اشاریہ(vptr);
    أكد(pml4e_ptr_get_present(pml4));
    pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_بنیاد_پتہ(pml4));
    pdpt += GET_PDPT_اشاریہ(vptr);
    أكد(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_مقصد_pptr(pdpte_pdpte_pd_ptr_get_pd_بنیاد_پتہ(pdpt));
    *(pd + GET_PD_اشاریہ(vptr)) = pde_pde_pt_new(
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

BOOT_رمز خالی *map_عارضی_boot_صفحہ(خالی *اندراج_2, uint32_t large_صفحات)
{
    /* this function is for legacy 32-bit systems where the ACPI tables might
     * collide with the kernel window. Here we just assert that the table is
     * in fact in the lower 4GiB region (which is already 1-to-1 mapped) and
     * continue */
    أكد((لفظ_t)اندراج_2 < ثنائی_ہندسہ(32));
    واپس اندراج_2;
}

ساکن BOOT_رمز cap_t create_it_pdpt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_pdpt_cap_new(
              asid,   /* capPDPTMappedASID    */
              pptr,   /* capPDPTBasePtr       */
              1,      /* capPDPTIsMapped      */
              vptr    /* capPDPTMappedAddress */
          );
    map_it_pdpt_cap(vspace_cap, cap);
    واپس cap;
}

ساکن BOOT_رمز cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحہ_directory_cap_new(
              asid,   /* capPDMappedASID      */
              pptr,   /* capPDBasePtr         */
              1,      /* capPDIsMapped        */
              vptr    /* capPDMappedAddress   */
          );
    map_it_pd_cap(vspace_cap, cap);
    واپس cap;
}

ساکن BOOT_رمز cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_صفحہ_جدول_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );
    map_it_pt_cap(vspace_cap, cap);
    واپس cap;
}


BOOT_رمز لفظ_t arch_get_n_paging(v_region_t it_v_reg)
{
    لفظ_t n = get_n_paging(it_v_reg, PD_اشاریہ_انحراف);
    n += get_n_paging(it_v_reg, PDPT_اشاریہ_انحراف);
    n += get_n_paging(it_v_reg, PML4_اشاریہ_انحراف);
#اگر_تعریف_شدہ CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#اگر_ختم
    واپس n;
}

BOOT_رمز cap_t create_it_پتہ_space(cap_t جڑ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_خانہ_Pos خانہ_pos_before;
    se_L4_خانہ_Pos خانہ_pos_after;

    خانہ_pos_before = ndks_boot.خانہ_pos_cur;
    copyGlobalMappings(PML4_PTR(rootserver.vspace));
    vspace_cap = cap_pml4_cap_new(
                     IT_ASID,        /* capPML4MappedASID */
                     rootserver.vspace,           /* capPML4BasePtr   */
                     1               /* capPML4IsMapped   */
                 );


    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

    /* Create any PDPTs needed for the user land image */
    برائے (vptr = باری_نیچے_کو(it_v_reg.آغاز, PML4_اشاریہ_انحراف);
         vptr < it_v_reg.انتہا;
         vptr += ثنائی_ہندسہ(PML4_اشاریہ_انحراف)) {
        اگر (!provide_cap(جڑ_cnode_cap,
                         create_it_pdpt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            واپس cap_null_cap_new();
        }
    }

    /* Create any PDs needed for the user land image */
    برائے (vptr = باری_نیچے_کو(it_v_reg.آغاز, PDPT_اشاریہ_انحراف);
         vptr < it_v_reg.انتہا;
         vptr += ثنائی_ہندسہ(PDPT_اشاریہ_انحراف)) {
        اگر (!provide_cap(جڑ_cnode_cap,
                         create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            واپس cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    برائے (vptr = باری_نیچے_کو(it_v_reg.آغاز, PD_اشاریہ_انحراف);
         vptr < it_v_reg.انتہا;
         vptr += ثنائی_ہندسہ(PD_اشاریہ_انحراف)) {
        اگر (!provide_cap(جڑ_cnode_cap,
                         create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
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

خالی copyGlobalMappings(vspace_جڑ_t *new_vspace)
{
    غیرعلامتی طویل i;
    pml4e_t *vspace = (pml4e_t *)new_vspace;

    /* Copy from the tlbbitmap_pptr so that we copy the default entries of the
     * tlb bitmap (if it exists). If it doesn't exist then this loop
     * will be equivalent to copying from PPTR_BASE
     */
    برائے (i = GET_PML4_اشاریہ(TLBBITMAP_PPTR); i < ثنائی_ہندسہ(PML4_اشاریہ_بٹ); i++) {
        vspace[i] = X86_GLOBAL_VSPACE_جڑ[i];
    }
}

ساکن BOOT_رمز cap_t create_it_چوکھٹا_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large, se_L4_لفظ map_type)
{
    vm_صفحہ_سائز_t چوکھٹا_سائز_2;

    اگر (use_large) {
        چوکھٹا_سائز_2 = X86_Large_صفحہ;
    } ورنہ {
        چوکھٹا_سائز_2 = X86_Small_صفحہ;
    }

    واپس
        cap_چوکھٹا_cap_new(
            asid,                          /* capFMappedASID     */
            pptr,                          /* capFBasePtr        */
            چوکھٹا_سائز_2,                    /* capFSize           */
            map_type,                      /* capFMapType        */
            vptr,                          /* capFMappedAddress  */
            لفظ_From_VMRights(VMReadWrite), /* capFVMRights       */
            0                              /* capFIsDevice       */
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

/* ====================== BOOT CODE FINISHES HERE ======================== */



exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_خانہ)
{
    asid_map_t asid_map;
#اگر_تعریف_شدہ CONFIG_VTX
    اگر (cap_get_capType(vspace_Cap_خانہ->cap) == cap_ept_pml4_cap) {
        cap_ept_pml4_cap_ptr_سیٹ_cap_PML4_Mapped_ASID(&vspace_Cap_خانہ->cap, asid);
        cap_ept_pml4_cap_ptr_سیٹ_cap_PML4_Is_Mapped(&vspace_Cap_خانہ->cap, 1);
        asid_map = asid_map_asid_map_ept_new(cap_ept_pml4_cap_get_cap_PML4_بنیاد_Ptr(vspace_Cap_خانہ->cap));
    } ورنہ
#اگر_ختم
    {
        أكد(cap_get_capType(vspace_Cap_خانہ->cap) == cap_pml4_cap);
        cap_pml4_cap_ptr_سیٹ_cap_PML4_Mapped_ASID(&vspace_Cap_خانہ->cap, asid);
        cap_pml4_cap_ptr_سیٹ_cap_PML4_Is_Mapped(&vspace_Cap_خانہ->cap, 1);
        asid_map = asid_map_asid_map_vspace_new(cap_pml4_cap_get_cap_PML4_بنیاد_Ptr(vspace_Cap_خانہ->cap));
    }
    poolPtr->array[ASID_کم(asid)] = asid_map;
    واپس EXCEPTION_NONE;
}

bool_t CONST is_VTable_جڑ(cap_t cap)
{
    واپس cap_get_capType(cap) == cap_pml4_cap;
}

bool_t CONST is_Valid_Native_جڑ(cap_t cap)
{
    واپس is_VTable_جڑ(cap) &&
           cap_pml4_cap_get_capPML4IsMapped(cap);
}

ساکن pml4e_t CONST makeUserPML4E(paddr_t paddr, vm_اوصاف_t vm_attr)
{
    واپس pml4e_new(
               0,
               paddr,
               0,
               vm_اوصاف_get_x86_PCDBit(vm_attr),
               vm_اوصاف_get_x86_PWTBit(vm_attr),
               1,
               1,
               1
           );
}

ساکن pml4e_t CONST makeUserPML4EInvalid(خالی)
{
    واپس pml4e_new(
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

ساکن pdpte_t CONST make_User_PDPTEHuge_صفحہ(paddr_t paddr, vm_اوصاف_t vm_attr, vm_rights_t vm_rights)
{
    واپس pdpte_pdpte_1g_new(
               0,          /* xd               */
               paddr,      /* physical address */
               0,          /* PAT              */
               0,          /* global           */
               0,          /* dirty            */
               0,          /* accessed         */
               vm_اوصاف_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_اوصاف_get_x86_PWTBit(vm_attr),  /* write through  */
               SuperUserFromVMRights(vm_rights),       /* super user     */
               WritableFromVMRights(vm_rights),        /* read write     */
               1                                       /* present        */
           );
}

ساکن pdpte_t CONST makeUserPDPTEPageDirectory(paddr_t paddr, vm_اوصاف_t vm_attr)
{
    واپس pdpte_pdpte_pd_new(
               0,                      /* xd       */
               paddr,                  /* paddr    */
               0,                      /* accessed */
               vm_اوصاف_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_اوصاف_get_x86_PWTBit(vm_attr),  /* write through  */
               1,                      /* super user */
               1,                      /* read write */
               1                       /* present    */
           );
}

ساکن pdpte_t CONST makeUserPDPTEInvalid(خالی)
{
    واپس pdpte_pdpte_pd_new(
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

pde_t CONST make_User_PDELarge_صفحہ(paddr_t paddr, vm_اوصاف_t vm_attr, vm_rights_t vm_rights)
{
    واپس pde_pde_large_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               vm_اوصاف_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_اوصاف_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_اوصاف_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_جدول(paddr_t paddr, vm_اوصاف_t vm_attr)
{

    واپس  pde_pde_pt_new(
                0,                                  /* xd               */
                paddr,                              /* pt_base_address  */
                0,                                  /* accessed         */
                vm_اوصاف_get_x86_PCDBit(vm_attr), /* cache_disabled   */
                vm_اوصاف_get_x86_PWTBit(vm_attr), /* write_through    */
                1,                                  /* super_user       */
                1,                                  /* read_write       */
                1                                   /* present          */
            );
}

pde_t CONST makeUserPDEInvalid(خالی)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    واپس pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_اوصاف_t vm_attr, vm_rights_t vm_rights)
{
    واپس pte_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               0,                                              /* global               */
               vm_اوصاف_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_اوصاف_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_اوصاف_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pte_t CONST makeUserPTEInvalid(خالی)
{
    واپس pte_new(
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


ساکن pml4e_t *lookup_PML4_خانہ(vspace_جڑ_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4e = PML4E_PTR(pml4);
    لفظ_t pml4_اشاریہ = GET_PML4_اشاریہ(vptr);
    واپس pml4e + pml4_اشاریہ;
}

ساکن lookupPDPTSlot_ret_t lookupPDPTSlot(vspace_جڑ_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4_خانہ = lookup_PML4_خانہ(pml4, vptr);
    lookupPDPTSlot_ret_t ret;

    اگر (!pml4e_ptr_get_present(pml4_خانہ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PML4_اشاریہ_انحراف);

        ret.pdpt_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    } ورنہ {
        pdpte_t *pdpt;
        pdpte_t *pdpt_خانہ;
        لفظ_t pdpt_اشاریہ = GET_PDPT_اشاریہ(vptr);
        pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_بنیاد_پتہ(pml4_خانہ));
        pdpt_خانہ = pdpt + pdpt_اشاریہ;

        ret.حالت_2 = EXCEPTION_NONE;
        ret.pdpt_خانہ = pdpt_خانہ;
        واپس ret;
    }
}

lookupPDSlot_ret_t lookupPDSlot(vspace_جڑ_t *pml4, vptr_t vptr)
{
    lookupPDPTSlot_ret_t pdpt_خانہ;
    lookupPDSlot_ret_t ret;

    pdpt_خانہ = lookupPDPTSlot(pml4, vptr);

    اگر (pdpt_خانہ.حالت_2 != EXCEPTION_NONE) {
        ret.pd_خانہ = تہی;
        ret.حالت_2 = pdpt_خانہ.حالت_2;
        واپس ret;
    }
    اگر ((pdpte_ptr_get_صفحہ_سائز(pdpt_خانہ.pdpt_خانہ) != pdpte_pdpte_pd) ||
        !pdpte_pdpte_pd_ptr_get_present(pdpt_خانہ.pdpt_خانہ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PDPT_اشاریہ_انحراف);

        ret.pd_خانہ = تہی;
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    } ورنہ {
        pde_t *pd;
        pde_t *pd_خانہ;
        لفظ_t pd_اشاریہ = GET_PD_اشاریہ(vptr);
        pd = paddr_مقصد_pptr(pdpte_pdpte_pd_ptr_get_pd_بنیاد_پتہ(pdpt_خانہ.pdpt_خانہ));
        pd_خانہ = pd + pd_اشاریہ;

        ret.حالت_2 = EXCEPTION_NONE;
        ret.pd_خانہ = pd_خانہ;
        واپس ret;
    }
}

ساکن خالی flushPD(vspace_جڑ_t *vspace, لفظ_t vptr, pde_t *pd, asid_t asid)
{
    /* clearing the entire PCID vs flushing the virtual addresses
     * one by one using invplg.
     * choose the easy way, invalidate the PCID
     */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));

}

ساکن خالی flushPDPT(vspace_جڑ_t *vspace, لفظ_t vptr, pdpte_t *pdpt, asid_t asid)
{
    /* similar here */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    واپس;
}

خالی hwASIDInvalidate(asid_t asid, vspace_جڑ_t *vspace)
{
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
}

خالی unmap_صفحہ_Directory(asid_t asid, vptr_t vaddr, pde_t *pd)
{
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;
    lookupPDPTSlot_ret_t    lu_ret;

    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        واپس;
    }

    lu_ret = lookupPDPTSlot(ڈھونڈو_ret.vspace_جڑ, vaddr);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        واپس;
    }

    /* check if the PDPT has the PD */
    اگر (!(pdpte_ptr_get_صفحہ_سائز(lu_ret.pdpt_خانہ) == pdpte_pdpte_pd &&
          pdpte_pdpte_pd_ptr_get_present(lu_ret.pdpt_خانہ) &&
          (pdpte_pdpte_pd_ptr_get_pd_بنیاد_پتہ(lu_ret.pdpt_خانہ) == pptr_مقصد_paddr(pd)))) {
        واپس;
    }

    flushPD(ڈھونڈو_ret.vspace_جڑ, vaddr, pd, asid);

    *lu_ret.pdpt_خانہ = makeUserPDPTEInvalid();

    invalidate_صفحہ_Structure_Cache_ASID(pptr_مقصد_paddr(ڈھونڈو_ret.vspace_جڑ), asid,
                                     SMP_TERNARY(tlb_bitmap_get(ڈھونڈو_ret.vspace_جڑ), 0));
}


ساکن exception_t perform_X64_صفحہ_Directory_Invocation_Unmap(cap_t cap, cte_t *ct_خانہ)
{

    اگر (cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(cap)) {
        pde_t *pd = PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(cap));
        unmap_صفحہ_Directory(
            cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(cap),
            cap_صفحہ_directory_cap_get_cap_PDMapped_پتہ(cap),
            pd
        );
        clear_حافظہ((خالی *)pd, cap_get_cap_سائز_بٹ(cap));
    }

    cap_صفحہ_directory_cap_ptr_سیٹ_cap_PDIs_Mapped(&(ct_خانہ->cap), 0);

    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_X64_صفحہ_Directory_Invocation_Map(cap_t cap, cte_t *ct_خانہ, pdpte_t pdpte, pdpte_t *pdpt_خانہ,
                                                        vspace_جڑ_t *vspace)
{
    ct_خانہ->cap = cap;
    *pdpt_خانہ = pdpte;
    invalidate_صفحہ_Structure_Cache_ASID(pptr_مقصد_paddr(vspace), cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    واپس EXCEPTION_NONE;
}


ساکن exception_t decode_X64_صفحہ_Directory_Invocation(
    لفظ_t label,
    لفظ_t لمبائی,
    cte_t *cte,
    cap_t cap,
    لفظ_t *بفر
)
{
    لفظ_t              vaddr;
    vm_اوصاف_t     vm_attr;
    cap_t               vspaceCap;
    vspace_جڑ_t      *vspace;
    pdpte_t             pdpte;
    paddr_t             paddr;
    asid_t              asid;
    lookupPDPTSlot_ret_t pdpt_خانہ;

    اگر (label == X86_صفحہ_Directory_Unmap) {
        اگر (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("X86PageDirectory: Cannot unmap if more than one cap exist.");
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);

        واپس perform_X64_صفحہ_Directory_Invocation_Unmap(cap, cte);
    }

    اگر (label != X86_صفحہ_Directory_Map) {
        userError("X64Directory: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی) {
        userError("X64PageDirectory: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(cap)) {
        userError("X64PageDirectory: PD is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر) & (~نقاب(PDPT_اشاریہ_انحراف));
    vm_attr = vm_اوصاف_From_لفظ(getSyscallArg(1, بفر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    اگر (!is_Valid_Native_جڑ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_جڑ_t *)pptr_کا_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    اگر (vaddr > USER_چوٹی) {
        userError("X64PageDirectory: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;

    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (ڈھونڈو_ret.vspace_جڑ != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pdpt_خانہ = lookupPDPTSlot(vspace, vaddr);
    اگر (pdpt_خانہ.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر ((pdpte_ptr_get_صفحہ_سائز(pdpt_خانہ.pdpt_خانہ) == pdpte_pdpte_pd &&
         pdpte_pdpte_pd_ptr_get_present(pdpt_خانہ.pdpt_خانہ)) ||
        (pdpte_ptr_get_صفحہ_سائز(pdpt_خانہ.pdpt_خانہ) == pdpte_pdpte_1g
         && pdpte_pdpte_1g_ptr_get_present(pdpt_خانہ.pdpt_خانہ))) {
        current_syscall_error.type = se_L4_Delete_پہلا;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr(PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(cap)));
    pdpte = makeUserPDPTEPageDirectory(paddr, vm_attr);

    cap = cap_صفحہ_directory_cap_سیٹ_cap_PDIs_Mapped(cap, 1);
    cap = cap_صفحہ_directory_cap_سیٹ_cap_PDMapped_ASID(cap, asid);
    cap = cap_صفحہ_directory_cap_سیٹ_cap_PDMapped_پتہ(cap, vaddr);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس perform_X64_صفحہ_Directory_Invocation_Map(cap, cte, pdpte, pdpt_خانہ.pdpt_خانہ, vspace);
}

ساکن خالی unmapPDPT(asid_t asid, vptr_t vaddr, pdpte_t *pdpt)
{
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;
    pml4e_t *pml4_خانہ;

    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        واپس;
    }

    pml4_خانہ = lookup_PML4_خانہ(ڈھونڈو_ret.vspace_جڑ, vaddr);

    /* check if the PML4 has the PDPT */
    اگر (!(pml4e_ptr_get_present(pml4_خانہ) &&
          pml4e_ptr_get_pdpt_بنیاد_پتہ(pml4_خانہ) == pptr_مقصد_paddr(pdpt))) {
        واپس;
    }

    flushPDPT(ڈھونڈو_ret.vspace_جڑ, vaddr, pdpt, asid);

    *pml4_خانہ = makeUserPML4EInvalid();
}

ساکن exception_t performX64PDPTInvocationUnmap(cap_t cap, cte_t *ct_خانہ)
{
    اگر (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        pdpte_t *pdpt = PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap));
        unmapPDPT(cap_pdpt_cap_get_capPDPTMappedASID(cap),
                  cap_pdpt_cap_get_cap_PDPTMapped_پتہ(cap),
                  pdpt);
        clear_حافظہ((خالی *)pdpt, cap_get_cap_سائز_بٹ(cap));
    }

    cap_pdpt_cap_ptr_سیٹ_cap_PDPTIs_Mapped(&(ct_خانہ->cap), 0);

    واپس EXCEPTION_NONE;
}

ساکن exception_t performX64PDPTInvocationMap(cap_t cap, cte_t *ct_خانہ, pml4e_t pml4e, pml4e_t *pml4_خانہ,
                                               vspace_جڑ_t *vspace)
{
    ct_خانہ->cap = cap;
    *pml4_خانہ = pml4e;
    invalidate_صفحہ_Structure_Cache_ASID(pptr_مقصد_paddr(vspace), cap_pdpt_cap_get_capPDPTMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));

    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeX64PDPTInvocation(
    لفظ_t  label,
    لفظ_t لمبائی,
    cte_t   *cte,
    cap_t   cap,
    لفظ_t  *بفر)
{
    لفظ_t                  vaddr;
    vm_اوصاف_t         attr;
    pml4e_t                *pml4_خانہ;
    cap_t                   vspaceCap;
    vspace_جڑ_t          *vspace;
    pml4e_t                 pml4e;
    paddr_t                 paddr;
    asid_t                  asid;

    اگر (label == X86PDPTUnmap) {
        اگر (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("X86PDPT: Cannot unmap if more than one cap exist.");
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);

        واپس performX64PDPTInvocationUnmap(cap, cte);
    }

    اگر (label != X86PDPTMap) {
        userError("X86PDPT: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (لمبائی < 2 || current_extra_caps.excaprefs[0] == تہی) {
        userError("X64PDPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X64PDPT: PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر) & (~نقاب(PML4_اشاریہ_انحراف));
    attr = vm_اوصاف_From_لفظ(getSyscallArg(1, بفر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    اگر (!is_Valid_Native_جڑ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_جڑ_t *)pptr_کا_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    اگر (vaddr > USER_چوٹی) {
        userError("X64PDPT: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret;

    ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
    اگر (ڈھونڈو_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (ڈھونڈو_ret.vspace_جڑ != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    pml4_خانہ = lookup_PML4_خانہ(vspace, vaddr);

    اگر (pml4e_ptr_get_present(pml4_خانہ)) {
        current_syscall_error.type = se_L4_Delete_پہلا;

        واپس EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr(PDPTE_PTR((cap_pdpt_cap_get_capPDPTBasePtr(cap))));
    pml4e = makeUserPML4E(paddr, attr);

    cap = cap_pdpt_cap_سیٹ_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_pdpt_cap_سیٹ_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_pdpt_cap_سیٹ_cap_PDPTMapped_پتہ(cap, vaddr);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس performX64PDPTInvocationMap(cap, cte, pml4e, pml4_خانہ, vspace);
}

exception_t decode_X86_منوال_MMUInvocation(
    لفظ_t label,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    انتخاب (cap_get_capType(cap)) {

    حالت cap_pml4_cap:
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;

    حالت cap_pdpt_cap:
        واپس decodeX64PDPTInvocation(label, لمبائی, cte, cap, بفر);

    حالت cap_صفحہ_directory_cap:
        واپس decode_X64_صفحہ_Directory_Invocation(label, لمبائی, cte, cap, بفر);

    طےشدہ:
        fail("Invalid arch cap type");
    }
}

bool_t منوال_Unmap_صفحہ(vm_صفحہ_سائز_t صفحہ_سائز, vspace_جڑ_t *vroot, vptr_t vaddr, خالی *pptr)
{
    اگر (config_سیٹ(CONFIG_HUGE_صفحہ) && صفحہ_سائز == X64_Huge_صفحہ) {
        pdpte_t     *pdpte;
        lookupPDPTSlot_ret_t pdpt_ret = lookupPDPTSlot(vroot, vaddr);
        اگر (pdpt_ret.حالت_2 != EXCEPTION_NONE) {
            واپس false;
        }
        pdpte = pdpt_ret.pdpt_خانہ;


        اگر (!(pdpte_ptr_get_صفحہ_سائز(pdpte) == pdpte_pdpte_1g
              && pdpte_pdpte_1g_ptr_get_present(pdpte)
              && (pdpte_pdpte_1g_ptr_get_صفحہ_بنیاد_پتہ(pdpte)
                  == pptr_مقصد_paddr(pptr)))) {
            واپس false;
        }

        *pdpte = makeUserPDPTEInvalid();
        واپس true;
    }
    fail("Invalid page type");
    واپس false;
}

ساکن exception_t updatePDPTE(asid_t asid, pdpte_t pdpte, pdpte_t *pdpt_خانہ, vspace_جڑ_t *vspace)
{
    *pdpt_خانہ = pdpte;
    invalidate_صفحہ_Structure_Cache_ASID(pptr_مقصد_paddr(vspace), asid,
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    واپس EXCEPTION_NONE;
}

ساکن exception_t perform_X64_منوال_Map(cap_t cap, cte_t *ct_خانہ, pdpte_t pdpte, pdpte_t *pdpt_خانہ, vspace_جڑ_t *vspace)
{
    ct_خانہ->cap = cap;
    واپس updatePDPTE(cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap), pdpte, pdpt_خانہ, vspace);
}

ساخت create_mapping_pdpte_return {
    exception_t حالت_2;
    pdpte_t pdpte;
    pdpte_t *pdpt_خانہ;
};
قسمتعریف ساخت create_mapping_pdpte_return create_mapping_pdpte_return_t;

ساکن create_mapping_pdpte_return_t create_محفوظ_Mapping_Entries_PDPTE(paddr_t بنیاد, لفظ_t vaddr, vm_rights_t vmRights,
                                                                    vm_اوصاف_t attr,
                                                                    vspace_جڑ_t *vspace)
{
    create_mapping_pdpte_return_t ret;
    lookupPDPTSlot_ret_t          lu_ret;

    lu_ret = lookupPDPTSlot(vspace, vaddr);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = false;
        ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        واپس ret;
    }
    ret.pdpt_خانہ = lu_ret.pdpt_خانہ;

    /* check for existing page directory */
    اگر ((pdpte_ptr_get_صفحہ_سائز(ret.pdpt_خانہ) == pdpte_pdpte_pd) &&
        (pdpte_pdpte_pd_ptr_get_present(ret.pdpt_خانہ))) {
        current_syscall_error.type = se_L4_Delete_پہلا;
        ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        واپس ret;
    }

    ret.pdpte = make_User_PDPTEHuge_صفحہ(بنیاد, attr, vmRights);
    ret.حالت_2 = EXCEPTION_NONE;
    واپس ret;
}

exception_t decode_X86_منوال_Map_صفحہ(لفظ_t label, vm_صفحہ_سائز_t صفحہ_سائز, cte_t *cte, cap_t cap,
                                 vspace_جڑ_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_اوصاف_t vm_attr)
{
    اگر (config_سیٹ(CONFIG_HUGE_صفحہ) && صفحہ_سائز == X64_Huge_صفحہ) {
        create_mapping_pdpte_return_t map_ret;

        map_ret = create_محفوظ_Mapping_Entries_PDPTE(paddr, vaddr, vm_rights, vm_attr, vroot);
        اگر (map_ret.حالت_2 != EXCEPTION_NONE) {
            واپس map_ret.حالت_2;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);

        انتخاب (label) {
        حالت X86_صفحہ_Map:
            واپس perform_X64_منوال_Map(cap, cte, map_ret.pdpte, map_ret.pdpt_خانہ, vroot);

        طےشدہ:
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
    }
    fail("Invalid Page type");
}

#اگر_تعریف_شدہ CONFIG_PRINTING
قسمتعریف ساخت read_لفظ_From_VSpace_ret {
    exception_t حالت_2;
    لفظ_t قدر;
} read_لفظ_From_VSpace_ret_t;

ساکن read_لفظ_From_VSpace_ret_t read_لفظ_From_VSpace(vspace_جڑ_t *vspace, لفظ_t vaddr)
{
    read_لفظ_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_خانہ;
    lookupPDSlot_ret_t pd_خانہ;
    lookupPDPTSlot_ret_t pdpt_خانہ;
    paddr_t paddr;
    لفظ_t انحراف;
    pptr_t kernel_vaddr;
    لفظ_t *قدر;

    pdpt_خانہ = lookupPDPTSlot(vspace, vaddr);
    اگر (pdpt_خانہ.حالت_2 == EXCEPTION_NONE &&
        pdpte_ptr_get_صفحہ_سائز(pdpt_خانہ.pdpt_خانہ) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_خانہ.pdpt_خانہ)) {

        paddr = pdpte_pdpte_1g_ptr_get_صفحہ_بنیاد_پتہ(pdpt_خانہ.pdpt_خانہ);
        انحراف = vaddr & نقاب(se_L4_Huge_صفحہ_بٹ);
    } ورنہ {
        pd_خانہ = lookupPDSlot(vspace, vaddr);
        اگر (pd_خانہ.حالت_2 == EXCEPTION_NONE &&
            ((pde_ptr_get_صفحہ_سائز(pd_خانہ.pd_خانہ) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_خانہ.pd_خانہ))) {

            paddr = pde_pde_large_ptr_get_صفحہ_بنیاد_پتہ(pd_خانہ.pd_خانہ);
            انحراف = vaddr & نقاب(se_L4_Large_صفحہ_بٹ);
        } ورنہ {
            pt_خانہ = lookupPTSlot(vspace, vaddr);
            اگر (pt_خانہ.حالت_2 == EXCEPTION_NONE && pte_ptr_get_present(pt_خانہ.pt_خانہ)) {
                paddr = pte_ptr_get_صفحہ_بنیاد_پتہ(pt_خانہ.pt_خانہ);
                انحراف = vaddr & نقاب(se_L4_صفحہ_بٹ);
            } ورنہ {
                ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
                واپس ret;
            }
        }
    }


    kernel_vaddr = (لفظ_t)paddr_مقصد_pptr(paddr);
    قدر = (لفظ_t *)(kernel_vaddr + انحراف);
    ret.حالت_2 = EXCEPTION_NONE;
    ret.قدر = *قدر;
    واپس ret;
}

خالی Arch_user_ڈھیر_Trace(tcb_t *tptr)
{
    cap_t thread_جڑ;
    vspace_جڑ_t *vspace_جڑ;
    لفظ_t sp;
    صحیح i;

    thread_جڑ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    اگر (cap_get_capType(thread_جڑ) != cap_pml4_cap) {
        لکھو("Invalid vspace\n");
        واپس;
    }

    vspace_جڑ = (vspace_جڑ_t *)pptr_کا_cap(thread_جڑ);

    sp = get_رجسٹر(tptr, RSP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    اگر (!IS_ALIGNED(sp, se_L4_لفظ_سائز_بٹ)) {
        لکھو("RSP not aligned\n");
        واپس;
    }

    برائے (i = 0; i < CONFIG_USER_ڈھیر_TRACE_لمبائی; i++) {
        لفظ_t پتہ = sp + (i * حجم(لفظ_t));
        read_لفظ_From_VSpace_ret_t نتیجہ;
        نتیجہ = read_لفظ_From_VSpace(vspace_جڑ, پتہ);
        اگر (نتیجہ.حالت_2 == EXCEPTION_NONE) {
            لکھو("0x%lx: 0x%lx\n", (طویل)پتہ, (طویل)نتیجہ.قدر);
        } ورنہ {
            لکھو("0x%lx: INVALID\n", (طویل)پتہ);
        }
    }
}
#اگر_ختم /* CONFIG_PRINTING */

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
#اگر_تعریف_شدہ CONFIG_HUGE_صفحہ
    x64KSKernelPD[1] = pde;
#ورنہ
    x64KSKernelPDs[ثنائی_ہندسہ(PDPT_اشاریہ_بٹ) - 1][1] = pde;
#اگر_ختم
    invalidateTranslationAll(نقاب(CONFIG_MAX_عدد_NODES));

    واپس EXCEPTION_NONE;
}
#اگر_ختم /* CONFIG_KERNEL_LOG_BUFFER */
