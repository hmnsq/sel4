/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/kernel/tlb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/tlb_bitmap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>

/* When using the SKIM window to isolate the kernel from the user we also need to
 * not use global mappings as having global mappings and entries in the TLB is
 * equivalent, for the purpose of exploitation, to having the mappings in the
 * kernel window */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KERNEL_IS_GLOBAL() (config_ਸੈੱਟ(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ) ? 0 : 1)

/* For the boot code we create two windows into the physical address space
 * One is at the same location as the kernel window, and is placed up high
 * The other is a 1-to-1 mapping of the first 512gb of memory. The purpose
 * of this is to have a 1-to-1 mapping for the low parts of memory, so that
 * when we switch paging on, and are still running at physical addresses,
 * we don't explode. Then we also want the high mappings so we can start
 * running at proper kernel virtual addresses */
pml4e_t boot_pml4[ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਬਿੱਟ)) VISIBLE PHYS_BSS;
pdpte_t boot_pdpt[ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)] ALIGN(ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਬਿੱਟ)) VISIBLE PHYS_BSS;

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

BOOT_ਕੋਡ bool_t map_kernel_ਖਿੜਕੀ(
    uint32_t ਅੰਕ_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   ਅੰਕ_drhu,
    paddr_t   *drhu_list
)
{

    uint64_t paddr;
    uint64_t vaddr;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HUGE_ਪੰਨਾ
    /* using 1 GiB page size */

    /* verify that the kernel window as at the last entry of the PML4 */
    ਪ੍ਰਤਿਜ੍ਞਾ(GET_PML4_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ) == ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    ਪ੍ਰਤਿਜ੍ਞਾ(GET_PML4_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਅਧਾਰ) == ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);
    ਪ੍ਰਤਿਜ੍ਞਾ(GET_PDPT_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਅਧਾਰ) == ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 2);
    ਪ੍ਰਤਿਜ੍ਞਾ(GET_PDPT_ਸੂਚਕਾਂਕ(KDEV_ਅਧਾਰ) == ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(KERNEL_ELF_ਅਧਾਰ - KERNEL_ELF_PADDR_ਅਧਾਰ, se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ));
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(KDEV_ਅਧਾਰ, se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ));
    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_ਮੰਜ਼ਿਲ_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );
    /* put the 1GB kernel_base mapping into the PDPT */
    x64KSKernelPDPT[GET_PDPT_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਅਧਾਰ)] = pdpte_pdpte_1g_new(
                                                           0, /* xd */
                                                           PADDR_ਅਧਾਰ,
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
    vaddr = PPTR_ਅਧਾਰ;
    ਲਈ (paddr = 0; paddr < PADDR_ਸਿਖਰ;
         paddr += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ)) {

        ਪੂਰਨ pdpte_ਸੂਚਕਾਂਕ = GET_PDPT_ਸੂਚਕਾਂਕ(vaddr);
        x64KSKernelPDPT[pdpte_ਸੂਚਕਾਂਕ] = pdpte_pdpte_1g_new(
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

        vaddr += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ);
    }

    /* put the PD into the PDPT */
    x64KSKernelPDPT[GET_PDPT_ਸੂਚਕਾਂਕ(KDEV_ਅਧਾਰ)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_ਮੰਜ਼ਿਲ_paddr(x64KSKernelPD),
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
                           kpptr_ਮੰਜ਼ਿਲ_paddr(x64KSKernelPT),
                           0, /* accessed */
                           0, /* cache_disabled */
                           0, /* write_through */
                           0, /* super_user */
                           1, /* read_write */
                           1  /* present */
                       );
#ਨਹੀਂਤਾਂ

    ਪੂਰਨ pd_ਸੂਚਕਾਂਕ_2 = 0;
    /* use 2 MiB page size */
    /* verify that the kernel window as at the last entry of the PML4 */
    ਪ੍ਰਤਿਜ੍ਞਾ(GET_PML4_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ) == ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    ਪ੍ਰਤਿਜ੍ਞਾ(GET_PML4_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਅਧਾਰ) == ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);
    ਪ੍ਰਤਿਜ੍ਞਾ(GET_PDPT_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਅਧਾਰ) == ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 2);
    ਪ੍ਰਤਿਜ੍ਞਾ(GET_PDPT_ਸੂਚਕਾਂਕ(KDEV_ਅਧਾਰ) == ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1);
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(KERNEL_ELF_ਅਧਾਰ - KERNEL_ELF_PADDR_ਅਧਾਰ, se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ));
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(KDEV_ਅਧਾਰ, se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ));

    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_ਮੰਜ਼ਿਲ_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    ਲਈ (pd_ਸੂਚਕਾਂਕ_2 = 0; pd_ਸੂਚਕਾਂਕ_2 < PADDR_ਸਿਖਰ >> se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ; pd_ਸੂਚਕਾਂਕ_2++) {
        /* put the 1GB kernel_base mapping into the PDPT */
        x64KSKernelPDPT[GET_PDPT_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ) + pd_ਸੂਚਕਾਂਕ_2] = pdpte_pdpte_pd_new(
                                                                    0, /* xd */
                                                                    kpptr_ਮੰਜ਼ਿਲ_paddr(&x64KSKernelPDs[pd_ਸੂਚਕਾਂਕ_2][0]),
                                                                    0, /* accessed */
                                                                    0, /* cache disabled */
                                                                    0, /* write through */
                                                                    0, /* super user */
                                                                    1, /* read write */
                                                                    1 /* present */
                                                                );
    }

    x64KSKernelPDPT[GET_PDPT_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਅਧਾਰ)] = pdpte_pdpte_pd_new(
                                                           0, /* xd */
                                                           kpptr_ਮੰਜ਼ਿਲ_paddr(&x64KSKernelPDs[0][0]),
                                                           0, /* accessed */
                                                           0, /* cache disable */
                                                           1, /* write through */
                                                           0, /* super user */
                                                           1, /* read write */
                                                           1  /* present */
                                                       );

    paddr = 0;
    vaddr = PPTR_ਅਧਾਰ;

    ਲਈ (paddr = 0; paddr < PADDR_ਸਿਖਰ;
         paddr += 0x200000) {

        ਪੂਰਨ pd_ਸੂਚਕਾਂਕ_2 = GET_PDPT_ਸੂਚਕਾਂਕ(vaddr) - GET_PDPT_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ);
        ਪੂਰਨ pde_ਸੂਚਕਾਂਕ = GET_PD_ਸੂਚਕਾਂਕ(vaddr);

        x64KSKernelPDs[pd_ਸੂਚਕਾਂਕ_2][pde_ਸੂਚਕਾਂਕ] = pde_pde_large_new(
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
    x64KSKernelPDPT[GET_PDPT_ਸੂਚਕਾਂਕ(KDEV_ਅਧਾਰ)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_ਮੰਜ਼ਿਲ_paddr(&x64KSKernelPDs[ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1][0]),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    /* put the PT into the PD */
    x64KSKernelPDs[ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1][0] = pde_pde_pt_new(
                                                      0, /* xd */
                                                      kpptr_ਮੰਜ਼ਿਲ_paddr(x64KSKernelPT),
                                                      0, /* accessed */
                                                      0, /* cache_disabled */
                                                      0, /* write_through */
                                                      0, /* super_user */
                                                      1, /* read_write */
                                                      1  /* present */
                                                  );
#ਜੇ_ਅੰਤ

#ਜੇ CONFIG_MAX_ਅੰਕ_TRACE_POINTS > 0
    /* use the last PD entry as the benchmark log storage.
     * the actual backing physical memory will be filled
     * later by using alloc_region */
    ksLog = (ks_log_ਇੰਦਰਾਜ_t *)(KDEV_ਅਧਾਰ + 0x200000 * (ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1));
#ਜੇ_ਅੰਤ

    /* now map in the kernel devices */
    ਜੇ (!map_kernel_ਖਿੜਕੀ_devices(x64KSKernelPT, ਅੰਕ_ioapic, ioapic_paddrs, ਅੰਕ_drhu, drhu_list)) {
        ਵਾਪਸ false;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(x64KSKernelPML4);
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

    /* In boot code, so fine to just trash everything here */
    invalidateLocalTranslationAll();
    ਲਿਖੋ("Mapping kernel window is done\n");
    ਵਾਪਸ true;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
BOOT_ਕੋਡ bool_t map_skim_ਖਿੜਕੀ(vptr_t skim_ਸ਼ੁਰੂ, vptr_t skim_ਸਿਰਾ)
{
    /* place the PDPT into the PML4 */
    x64KSSKIMPML4[GET_PML4_ਸੂਚਕਾਂਕ(PPTR_ਅਧਾਰ)] = pml4e_new(
                                                   0, /* xd */
                                                   kpptr_ਮੰਜ਼ਿਲ_paddr(x64KSSKIMPDPT),
                                                   0, /* accessed */
                                                   0, /* cache_disabled */
                                                   0, /* write_through */
                                                   0, /* super_user */
                                                   1, /* read_write */
                                                   1  /* present */
                                               );
    /* place the PD into the kernel_base slot of the PDPT */
    x64KSSKIMPDPT[GET_PDPT_ਸੂਚਕਾਂਕ(KERNEL_ELF_ਅਧਾਰ)] = pdpte_pdpte_pd_new(
                                                         0, /* xd */
                                                         kpptr_ਮੰਜ਼ਿਲ_paddr(x64KSSKIMPD),
                                                         0, /* accessed */
                                                         0, /* cache_disabled */
                                                         0, /* write_through */
                                                         0, /* super_user */
                                                         1, /* read_write */
                                                         1  /* present */
                                                     );
    /* map the skim portion into the PD. we expect it to be 2M aligned */
    ਪ੍ਰਤਿਜ੍ਞਾ((skim_ਸ਼ੁਰੂ % ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Large_ਪੰਨਾ_ਬਿੱਟ)) == 0);
    ਪ੍ਰਤਿਜ੍ਞਾ((skim_ਸਿਰਾ % ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Large_ਪੰਨਾ_ਬਿੱਟ)) == 0);
    uint64_t paddr = kpptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)skim_ਸ਼ੁਰੂ);
    ਲਈ (ਪੂਰਨ i = GET_PD_ਸੂਚਕਾਂਕ(skim_ਸ਼ੁਰੂ); i < GET_PD_ਸੂਚਕਾਂਕ(skim_ਸਿਰਾ); i++) {
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
        paddr += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Large_ਪੰਨਾ_ਬਿੱਟ);
    }
    ਵਾਪਸ true;
}
#ਜੇ_ਅੰਤ

BOOT_ਕੋਡ ਖਾਲੀ init_tss(tss_t *tss)
{
    ਸ਼ਬਦ_t ਅਧਾਰ = (ਸ਼ਬਦ_t)&x64KSIRQStack[CURRENT_CPU_ਸੂਚਕਾਂਕ()][IRQ_ਢੇਰ_ਮਾਪ];
    *tss = tss_new(
               ਆਕਾਰ(*tss),   /* io map base */
               0, 0,       /* ist 7 */
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               /* ist 1 is the stack frame we use for interrupts */
               ਅਧਾਰ >> 32, ਅਧਾਰ & 0xffffffff,  /* ist 1 */
               0, 0,       /* rsp 2 */
               0, 0,       /* rsp 1 */
               0, 0        /* rsp 0 */
           );
    /* set the IO map to all 1 to block user IN/OUT instructions */
    memset(&x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KStss.io_map[0], 0xff,
           ਆਕਾਰ(x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KStss.io_map));
}

ਖਾਲੀ init_syscall_msrs(ਖਾਲੀ)
{
    x86_wrmsr(IA32_LSTAR_MSR, (uint64_t)&handle_fastsyscall);
    // mask bit 9 in the kernel (which is the interrupt enable bit)
    // also mask bit 8, which is the Trap Flag, to prevent the kernel
    // from single stepping
    x86_wrmsr(IA32_FMASK_MSR, FLAGS_TF | FLAGS_IF);
    x86_wrmsr(IA32_ਤਾਰਾ_MSR, ((uint64_t)SEL_CS_0 << 32) | ((uint64_t)SEL_CS_3 << 48));
}

BOOT_ਕੋਡ ਖਾਲੀ init_gdt(gdt_ਇੰਦਰਾਜ_t *gdt, tss_t *tss)
{

    uint64_t tss_ਅਧਾਰ = (uint64_t)tss;
    gdt_tss_t gdt_tss;

    gdt[GDT_NULL] = gdt_ਇੰਦਰਾਜ_gdt_null_new();

    gdt[GDT_CS_0] = gdt_ਇੰਦਰਾਜ_gdt_ਕੋਡ_new(
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

    gdt[GDT_DS_0] = gdt_ਇੰਦਰਾਜ_gdt_ਅੰਕੜੇ_new(
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

    gdt[GDT_CS_3] = gdt_ਇੰਦਰਾਜ_gdt_ਕੋਡ_new(
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

    gdt[GDT_DS_3] = gdt_ਇੰਦਰਾਜ_gdt_ਅੰਕੜੇ_new(
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

    gdt[GDT_FS] = gdt_ਇੰਦਰਾਜ_gdt_ਅੰਕੜੇ_new(
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

    gdt[GDT_GS] = gdt_ਇੰਦਰਾਜ_gdt_ਅੰਕੜੇ_new(
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
                  tss_ਅਧਾਰ >> 32,                     /* base 63 - 32 */
                  (tss_ਅਧਾਰ & 0xff000000UL) >> 24,    /* base 31 - 24 */
                  1,                                  /* granularity */
                  0,                                  /* avl */
                  0,                                  /* limit high */
                  1,                                  /* present */
                  0,                                  /* dpl */
                  9,                                  /* desc type */
                  (tss_ਅਧਾਰ & 0xff0000UL) >> 16,      /* base 23-16 */
                  (tss_ਅਧਾਰ & 0xffffUL),              /* base 15 - 0 */
                  ਆਕਾਰ(tss_io_t) - 1
              );

    gdt[GDT_TSS].ਸ਼ਬਦ[0] = gdt_tss.ਸ਼ਬਦ[0];
    gdt[GDT_TSS + 1].ਸ਼ਬਦ[0] = gdt_tss.ਸ਼ਬਦ[1];
}

BOOT_ਕੋਡ ਖਾਲੀ init_idt_ਇੰਦਰਾਜ(idt_ਇੰਦਰਾਜ_t *idt, ਰੁਕਾਵਟ_t ਰੁਕਾਵਟ, ਖਾਲੀ(*handler)(ਖਾਲੀ))
{
    uint64_t handler_addr = (uint64_t)handler;
    uint64_t dpl = 3;

    ਜੇ (ਰੁਕਾਵਟ < int_trap_min && ਰੁਕਾਵਟ != int_software_break_request) {
        dpl = 0;
    }

    idt[ਰੁਕਾਵਟ] = idt_ਇੰਦਰਾਜ_ਰੁਕਾਵਟ_gate_new(
                         handler_addr >> 32,                 /* offset 63 - 32 */
                         ((handler_addr >> 16) & 0xffff),
                         1,                                  /* present */
                         dpl,                                /* dpl */
                         1,                                  /* ist */
                         SEL_CS_0,                           /* segment selector */
                         (handler_addr & 0xffff)               /* offset 15 - 0 */
                     );
}

ਖਾਲੀ ਸੈੱਟ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ਜੜ੍ਹ;
    asid_t asid;
    pml4e_t *pml4;
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;
    cr3_t cr3;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    ਜੇ (cap_get_capType(thread_ਜੜ੍ਹ) != cap_pml4_cap ||
        !cap_pml4_cap_get_capPML4IsMapped(thread_ਜੜ੍ਹ)) {
        ਸੈੱਟ_Current_User_VSpace_ਜੜ੍ਹ(kpptr_ਮੰਜ਼ਿਲ_paddr(X86_GLOBAL_VSPACE_ਜੜ੍ਹ), 0);
        ਵਾਪਸ;
    }

    pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(thread_ਜੜ੍ਹ));
    asid = cap_pml4_cap_get_capPML4MappedASID(thread_ਜੜ੍ਹ);
    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (unlikely(ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE || ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != pml4)) {
        ਸੈੱਟ_Current_User_VSpace_ਜੜ੍ਹ(kpptr_ਮੰਜ਼ਿਲ_paddr(X86_GLOBAL_VSPACE_ਜੜ੍ਹ), 0);
        ਵਾਪਸ;
    }
    cr3 = makeCR3(pptr_ਮੰਜ਼ਿਲ_paddr(pml4), asid);
    ਜੇ (getCurrentUserCR3().ਸ਼ਬਦ[0] != cr3.ਸ਼ਬਦ[0]) {
        SMP_COND_STATEMENT(tlb_bitmap_ਸੈੱਟ(pml4, getCurrentCPUIndex());)
        ਸੈੱਟ_Current_User_CR3(cr3);
    }
}


BOOT_ਕੋਡ ਖਾਲੀ init_dtrs(ਖਾਲੀ)
{
    gdt_idt_ptr.ਹੱਦਬੰਦੀ = (ਆਕਾਰ(gdt_ਇੰਦਰਾਜ_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.ਅਧਾਰ = (uint64_t)x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KSgdt;

    /* When we install the gdt it will clobber any value of gs that
     * we have. Since we might be using it for TLS we can stash
     * and unstash any gs value using swapgs
     */
    swapgs();
    x64_install_gdt(&gdt_idt_ptr);
    swapgs();

    gdt_idt_ptr.ਹੱਦਬੰਦੀ = (ਆਕਾਰ(idt_ਇੰਦਰਾਜ_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.ਅਧਾਰ = (uint64_t)x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KSidt;
    x64_install_idt(&gdt_idt_ptr);

    x64_install_ldt(SEL_NULL);

    x64_install_tss(SEL_TSS);
}

BOOT_ਕੋਡ ਖਾਲੀ map_it_ਚੌਖਟਾ_cap(cap_t pd_cap, cap_t ਚੌਖਟਾ_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_ਦਾ_cap(pd_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt;
    vptr_t vptr = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(ਚੌਖਟਾ_cap);
    ਖਾਲੀ *pptr = (ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(ਚੌਖਟਾ_cap);

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਚੌਖਟਾ_cap_get_cap_FMap_Type(ਚੌਖਟਾ_cap) == X86_MappingVSpace);
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(ਚੌਖਟਾ_cap) != asidInvalid);
    pml4 += GET_PML4_ਸੂਚਕਾਂਕ(vptr);
    ਪ੍ਰਤਿਜ੍ਞਾ(pml4e_ptr_get_present(pml4));
    pdpt = paddr_ਮੰਜ਼ਿਲ_pptr(pml4e_ptr_get_pdpt_ਅਧਾਰ_ਪਤਾ(pml4));
    pdpt += GET_PDPT_ਸੂਚਕਾਂਕ(vptr);
    ਪ੍ਰਤਿਜ੍ਞਾ(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_ਮੰਜ਼ਿਲ_pptr(pdpte_pdpte_pd_ptr_get_pd_ਅਧਾਰ_ਪਤਾ(pdpt));
    pd += GET_PD_ਸੂਚਕਾਂਕ(vptr);
    ਪ੍ਰਤਿਜ੍ਞਾ(pde_pde_pt_ptr_get_present(pd));
    pt = paddr_ਮੰਜ਼ਿਲ_pptr(pde_pde_pt_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pd));
    *(pt + GET_PT_ਸੂਚਕਾਂਕ(vptr)) = pte_new(
                                     0,                      /* xd                   */
                                     pptr_ਮੰਜ਼ਿਲ_paddr(pptr),    /* page_base_address    */
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

ਸਥਿਰ BOOT_ਕੋਡ ਖਾਲੀ map_it_pdpt_cap(cap_t vspace_cap, cap_t pdpt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_ਦਾ_cap(vspace_cap));
    pdpte_t *pdpt = PDPT_PTR(cap_pdpt_cap_get_capPDPTBasePtr(pdpt_cap));
    vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_ਪਤਾ(pdpt_cap);

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_pdpt_cap_get_capPDPTIsMapped(pdpt_cap));
    *(pml4 + GET_PML4_ਸੂਚਕਾਂਕ(vptr)) = pml4e_new(
                                         0,                      /* xd                   */
                                         pptr_ਮੰਜ਼ਿਲ_paddr(pdpt),    /* pdpt_base_address    */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_ਕੋਡ ਖਾਲੀ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_ਦਾ_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd = PD_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    vptr_t vptr = cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ਪਤਾ(pd_cap);

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(pd_cap));
    pml4 += GET_PML4_ਸੂਚਕਾਂਕ(vptr);
    ਪ੍ਰਤਿਜ੍ਞਾ(pml4e_ptr_get_present(pml4));
    pdpt = paddr_ਮੰਜ਼ਿਲ_pptr(pml4e_ptr_get_pdpt_ਅਧਾਰ_ਪਤਾ(pml4));
    *(pdpt + GET_PDPT_ਸੂਚਕਾਂਕ(vptr)) = pdpte_pdpte_pd_new(
                                         0,                      /* xd                   */
                                         pptr_ਮੰਜ਼ਿਲ_paddr(pd),      /* pd_base_address      */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_ਕੋਡ ਖਾਲੀ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_ਦਾ_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt = PT_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(pt_cap);

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(pt_cap));
    pml4 += GET_PML4_ਸੂਚਕਾਂਕ(vptr);
    ਪ੍ਰਤਿਜ੍ਞਾ(pml4e_ptr_get_present(pml4));
    pdpt = paddr_ਮੰਜ਼ਿਲ_pptr(pml4e_ptr_get_pdpt_ਅਧਾਰ_ਪਤਾ(pml4));
    pdpt += GET_PDPT_ਸੂਚਕਾਂਕ(vptr);
    ਪ੍ਰਤਿਜ੍ਞਾ(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_ਮੰਜ਼ਿਲ_pptr(pdpte_pdpte_pd_ptr_get_pd_ਅਧਾਰ_ਪਤਾ(pdpt));
    *(pd + GET_PD_ਸੂਚਕਾਂਕ(vptr)) = pde_pde_pt_new(
                                     0,                      /* xd                   */
                                     pptr_ਮੰਜ਼ਿਲ_paddr(pt),      /* pt_base_address      */
                                     0,                      /* accessed             */
                                     0,                      /* cache_disabled       */
                                     0,                      /* write_through        */
                                     1,                      /* super_user           */
                                     1,                      /* read_write           */
                                     1                       /* present              */
                                 );
}

BOOT_ਕੋਡ ਖਾਲੀ *map_ਆਰਜ਼ੀ_boot_ਪੰਨਾ(ਖਾਲੀ *ਇੰਦਰਾਜ, uint32_t large_ਪੰਨੇ)
{
    /* this function is for legacy 32-bit systems where the ACPI tables might
     * collide with the kernel window. Here we just assert that the table is
     * in fact in the lower 4GiB region (which is already 1-to-1 mapped) and
     * continue */
    ਪ੍ਰਤਿਜ੍ਞਾ((ਸ਼ਬਦ_t)ਇੰਦਰਾਜ < ਦੋਆਧਾਰੀ_ਅੰਕ(32));
    ਵਾਪਸ ਇੰਦਰਾਜ;
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_pdpt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_pdpt_cap_new(
              asid,   /* capPDPTMappedASID    */
              pptr,   /* capPDPTBasePtr       */
              1,      /* capPDPTIsMapped      */
              vptr    /* capPDPTMappedAddress */
          );
    map_it_pdpt_cap(vspace_cap, cap);
    ਵਾਪਸ cap;
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ਪੰਨਾ_directory_cap_new(
              asid,   /* capPDMappedASID      */
              pptr,   /* capPDBasePtr         */
              1,      /* capPDIsMapped        */
              vptr    /* capPDMappedAddress   */
          );
    map_it_pd_cap(vspace_cap, cap);
    ਵਾਪਸ cap;
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );
    map_it_pt_cap(vspace_cap, cap);
    ਵਾਪਸ cap;
}


BOOT_ਕੋਡ ਸ਼ਬਦ_t arch_get_n_paging(v_region_t it_v_reg)
{
    ਸ਼ਬਦ_t n = get_n_paging(it_v_reg, PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ);
    n += get_n_paging(it_v_reg, PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ);
    n += get_n_paging(it_v_reg, PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ n;
}

BOOT_ਕੋਡ cap_t create_it_ਪਤਾ_space(cap_t ਜੜ੍ਹ_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_before;
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_after;

    ਥਾਂ_pos_before = ndks_boot.ਥਾਂ_pos_cur;
    copyGlobalMappings(PML4_PTR(rootserver.vspace));
    vspace_cap = cap_pml4_cap_new(
                     IT_ASID,        /* capPML4MappedASID */
                     rootserver.vspace,           /* capPML4BasePtr   */
                     1               /* capPML4IsMapped   */
                 );


    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

    /* Create any PDPTs needed for the user land image */
    ਲਈ (vptr = ਗੇੜਾ_ਥੱਲੇ(it_v_reg.ਸ਼ੁਰੂ, PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ);
         vptr < it_v_reg.ਸਿਰਾ;
         vptr += ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap,
                         create_it_pdpt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            ਵਾਪਸ cap_null_cap_new();
        }
    }

    /* Create any PDs needed for the user land image */
    ਲਈ (vptr = ਗੇੜਾ_ਥੱਲੇ(it_v_reg.ਸ਼ੁਰੂ, PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ);
         vptr < it_v_reg.ਸਿਰਾ;
         vptr += ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap,
                         create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            ਵਾਪਸ cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    ਲਈ (vptr = ਗੇੜਾ_ਥੱਲੇ(it_v_reg.ਸ਼ੁਰੂ, PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ);
         vptr < it_v_reg.ਸਿਰਾ;
         vptr += ਦੋਆਧਾਰੀ_ਅੰਕ(PD_ਸੂਚਕਾਂਕ_ਵਿੱਥ)) {
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap,
                         create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
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

ਖਾਲੀ copyGlobalMappings(vspace_ਜੜ੍ਹ_t *new_vspace)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;
    pml4e_t *vspace = (pml4e_t *)new_vspace;

    /* Copy from the tlbbitmap_pptr so that we copy the default entries of the
     * tlb bitmap (if it exists). If it doesn't exist then this loop
     * will be equivalent to copying from PPTR_BASE
     */
    ਲਈ (i = GET_PML4_ਸੂਚਕਾਂਕ(TLBBITMAP_PPTR); i < ਦੋਆਧਾਰੀ_ਅੰਕ(PML4_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        vspace[i] = X86_GLOBAL_VSPACE_ਜੜ੍ਹ[i];
    }
}

ਸਥਿਰ BOOT_ਕੋਡ cap_t create_it_ਚੌਖਟਾ_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large, se_L4_ਸ਼ਬਦ map_type)
{
    vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ_2;

    ਜੇ (use_large) {
        ਚੌਖਟਾ_ਮਾਪ_2 = X86_Large_ਪੰਨਾ;
    } ਨਹੀਂਤਾਂ {
        ਚੌਖਟਾ_ਮਾਪ_2 = X86_Small_ਪੰਨਾ;
    }

    ਵਾਪਸ
        cap_ਚੌਖਟਾ_cap_new(
            asid,                          /* capFMappedASID     */
            pptr,                          /* capFBasePtr        */
            ਚੌਖਟਾ_ਮਾਪ_2,                    /* capFSize           */
            map_type,                      /* capFMapType        */
            vptr,                          /* capFMappedAddress  */
            ਸ਼ਬਦ_From_VMRights(VMReadWrite), /* capFVMRights       */
            0                              /* capFIsDevice       */
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

/* ====================== BOOT CODE FINISHES HERE ======================== */



exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_ਥਾਂ)
{
    asid_map_t asid_map;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਜੇ (cap_get_capType(vspace_Cap_ਥਾਂ->cap) == cap_ept_pml4_cap) {
        cap_ept_pml4_cap_ptr_ਸੈੱਟ_cap_PML4_Mapped_ASID(&vspace_Cap_ਥਾਂ->cap, asid);
        cap_ept_pml4_cap_ptr_ਸੈੱਟ_cap_PML4_Is_Mapped(&vspace_Cap_ਥਾਂ->cap, 1);
        asid_map = asid_map_asid_map_ept_new(cap_ept_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(vspace_Cap_ਥਾਂ->cap));
    } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
        ਪ੍ਰਤਿਜ੍ਞਾ(cap_get_capType(vspace_Cap_ਥਾਂ->cap) == cap_pml4_cap);
        cap_pml4_cap_ptr_ਸੈੱਟ_cap_PML4_Mapped_ASID(&vspace_Cap_ਥਾਂ->cap, asid);
        cap_pml4_cap_ptr_ਸੈੱਟ_cap_PML4_Is_Mapped(&vspace_Cap_ਥਾਂ->cap, 1);
        asid_map = asid_map_asid_map_vspace_new(cap_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(vspace_Cap_ਥਾਂ->cap));
    }
    poolPtr->array[ASID_ਨੀਵਾਂ(asid)] = asid_map;
    ਵਾਪਸ EXCEPTION_NONE;
}

bool_t CONST is_VTable_ਜੜ੍ਹ(cap_t cap)
{
    ਵਾਪਸ cap_get_capType(cap) == cap_pml4_cap;
}

bool_t CONST is_Valid_Native_ਜੜ੍ਹ(cap_t cap)
{
    ਵਾਪਸ is_VTable_ਜੜ੍ਹ(cap) &&
           cap_pml4_cap_get_capPML4IsMapped(cap);
}

ਸਥਿਰ pml4e_t CONST makeUserPML4E(paddr_t paddr, vm_ਗੁਣ_t vm_attr)
{
    ਵਾਪਸ pml4e_new(
               0,
               paddr,
               0,
               vm_ਗੁਣ_get_x86_PCDBit(vm_attr),
               vm_ਗੁਣ_get_x86_PWTBit(vm_attr),
               1,
               1,
               1
           );
}

ਸਥਿਰ pml4e_t CONST makeUserPML4EInvalid(ਖਾਲੀ)
{
    ਵਾਪਸ pml4e_new(
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

ਸਥਿਰ pdpte_t CONST make_User_PDPTEHuge_ਪੰਨਾ(paddr_t paddr, vm_ਗੁਣ_t vm_attr, vm_rights_t vm_rights)
{
    ਵਾਪਸ pdpte_pdpte_1g_new(
               0,          /* xd               */
               paddr,      /* physical address */
               0,          /* PAT              */
               0,          /* global           */
               0,          /* dirty            */
               0,          /* accessed         */
               vm_ਗੁਣ_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_ਗੁਣ_get_x86_PWTBit(vm_attr),  /* write through  */
               SuperUserFromVMRights(vm_rights),       /* super user     */
               WritableFromVMRights(vm_rights),        /* read write     */
               1                                       /* present        */
           );
}

ਸਥਿਰ pdpte_t CONST makeUserPDPTEPageDirectory(paddr_t paddr, vm_ਗੁਣ_t vm_attr)
{
    ਵਾਪਸ pdpte_pdpte_pd_new(
               0,                      /* xd       */
               paddr,                  /* paddr    */
               0,                      /* accessed */
               vm_ਗੁਣ_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_ਗੁਣ_get_x86_PWTBit(vm_attr),  /* write through  */
               1,                      /* super user */
               1,                      /* read write */
               1                       /* present    */
           );
}

ਸਥਿਰ pdpte_t CONST makeUserPDPTEInvalid(ਖਾਲੀ)
{
    ਵਾਪਸ pdpte_pdpte_pd_new(
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

pde_t CONST make_User_PDELarge_ਪੰਨਾ(paddr_t paddr, vm_ਗੁਣ_t vm_attr, vm_rights_t vm_rights)
{
    ਵਾਪਸ pde_pde_large_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               vm_ਗੁਣ_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ਗੁਣ_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_ਗੁਣ_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_ਸਾਰਣੀ(paddr_t paddr, vm_ਗੁਣ_t vm_attr)
{

    ਵਾਪਸ  pde_pde_pt_new(
                0,                                  /* xd               */
                paddr,                              /* pt_base_address  */
                0,                                  /* accessed         */
                vm_ਗੁਣ_get_x86_PCDBit(vm_attr), /* cache_disabled   */
                vm_ਗੁਣ_get_x86_PWTBit(vm_attr), /* write_through    */
                1,                                  /* super_user       */
                1,                                  /* read_write       */
                1                                   /* present          */
            );
}

pde_t CONST makeUserPDEInvalid(ਖਾਲੀ)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    ਵਾਪਸ pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_ਗੁਣ_t vm_attr, vm_rights_t vm_rights)
{
    ਵਾਪਸ pte_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               0,                                              /* global               */
               vm_ਗੁਣ_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ਗੁਣ_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_ਗੁਣ_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pte_t CONST makeUserPTEInvalid(ਖਾਲੀ)
{
    ਵਾਪਸ pte_new(
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


ਸਥਿਰ pml4e_t *lookup_PML4_ਥਾਂ(vspace_ਜੜ੍ਹ_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4e = PML4E_PTR(pml4);
    ਸ਼ਬਦ_t pml4_ਸੂਚਕਾਂਕ = GET_PML4_ਸੂਚਕਾਂਕ(vptr);
    ਵਾਪਸ pml4e + pml4_ਸੂਚਕਾਂਕ;
}

ਸਥਿਰ lookupPDPTSlot_ret_t lookupPDPTSlot(vspace_ਜੜ੍ਹ_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4_ਥਾਂ = lookup_PML4_ਥਾਂ(pml4, vptr);
    lookupPDPTSlot_ret_t ret;

    ਜੇ (!pml4e_ptr_get_present(pml4_ਥਾਂ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ);

        ret.pdpt_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    } ਨਹੀਂਤਾਂ {
        pdpte_t *pdpt;
        pdpte_t *pdpt_ਥਾਂ;
        ਸ਼ਬਦ_t pdpt_ਸੂਚਕਾਂਕ = GET_PDPT_ਸੂਚਕਾਂਕ(vptr);
        pdpt = paddr_ਮੰਜ਼ਿਲ_pptr(pml4e_ptr_get_pdpt_ਅਧਾਰ_ਪਤਾ(pml4_ਥਾਂ));
        pdpt_ਥਾਂ = pdpt + pdpt_ਸੂਚਕਾਂਕ;

        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ret.pdpt_ਥਾਂ = pdpt_ਥਾਂ;
        ਵਾਪਸ ret;
    }
}

lookupPDSlot_ret_t lookupPDSlot(vspace_ਜੜ੍ਹ_t *pml4, vptr_t vptr)
{
    lookupPDPTSlot_ret_t pdpt_ਥਾਂ;
    lookupPDSlot_ret_t ret;

    pdpt_ਥਾਂ = lookupPDPTSlot(pml4, vptr);

    ਜੇ (pdpt_ਥਾਂ.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ret.pd_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = pdpt_ਥਾਂ.ਸਥਿਤੀ;
        ਵਾਪਸ ret;
    }
    ਜੇ ((pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(pdpt_ਥਾਂ.pdpt_ਥਾਂ) != pdpte_pdpte_pd) ||
        !pdpte_pdpte_pd_ptr_get_present(pdpt_ਥਾਂ.pdpt_ਥਾਂ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ);

        ret.pd_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    } ਨਹੀਂਤਾਂ {
        pde_t *pd;
        pde_t *pd_ਥਾਂ;
        ਸ਼ਬਦ_t pd_ਸੂਚਕਾਂਕ = GET_PD_ਸੂਚਕਾਂਕ(vptr);
        pd = paddr_ਮੰਜ਼ਿਲ_pptr(pdpte_pdpte_pd_ptr_get_pd_ਅਧਾਰ_ਪਤਾ(pdpt_ਥਾਂ.pdpt_ਥਾਂ));
        pd_ਥਾਂ = pd + pd_ਸੂਚਕਾਂਕ;

        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ret.pd_ਥਾਂ = pd_ਥਾਂ;
        ਵਾਪਸ ret;
    }
}

ਸਥਿਰ ਖਾਲੀ flushPD(vspace_ਜੜ੍ਹ_t *vspace, ਸ਼ਬਦ_t vptr, pde_t *pd, asid_t asid)
{
    /* clearing the entire PCID vs flushing the virtual addresses
     * one by one using invplg.
     * choose the easy way, invalidate the PCID
     */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));

}

ਸਥਿਰ ਖਾਲੀ flushPDPT(vspace_ਜੜ੍ਹ_t *vspace, ਸ਼ਬਦ_t vptr, pdpte_t *pdpt, asid_t asid)
{
    /* similar here */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    ਵਾਪਸ;
}

ਖਾਲੀ hwASIDInvalidate(asid_t asid, vspace_ਜੜ੍ਹ_t *vspace)
{
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
}

ਖਾਲੀ unmap_ਪੰਨਾ_Directory(asid_t asid, vptr_t vaddr, pde_t *pd)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;
    lookupPDPTSlot_ret_t    lu_ret;

    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ;
    }

    lu_ret = lookupPDPTSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vaddr);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ;
    }

    /* check if the PDPT has the PD */
    ਜੇ (!(pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.pdpt_ਥਾਂ) == pdpte_pdpte_pd &&
          pdpte_pdpte_pd_ptr_get_present(lu_ret.pdpt_ਥਾਂ) &&
          (pdpte_pdpte_pd_ptr_get_pd_ਅਧਾਰ_ਪਤਾ(lu_ret.pdpt_ਥਾਂ) == pptr_ਮੰਜ਼ਿਲ_paddr(pd)))) {
        ਵਾਪਸ;
    }

    flushPD(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vaddr, pd, asid);

    *lu_ret.pdpt_ਥਾਂ = makeUserPDPTEInvalid();

    invalidate_ਪੰਨਾ_Structure_Cache_ASID(pptr_ਮੰਜ਼ਿਲ_paddr(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ), asid,
                                     SMP_TERNARY(tlb_bitmap_get(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ), 0));
}


ਸਥਿਰ exception_t perform_X64_ਪੰਨਾ_Directory_Invocation_Unmap(cap_t cap, cte_t *ct_ਥਾਂ)
{

    ਜੇ (cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(cap)) {
        pde_t *pd = PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(cap));
        unmap_ਪੰਨਾ_Directory(
            cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(cap),
            cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ਪਤਾ(cap),
            pd
        );
        clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *)pd, cap_get_cap_ਮਾਪ_ਬਿੱਟ(cap));
    }

    cap_ਪੰਨਾ_directory_cap_ptr_ਸੈੱਟ_cap_PDIs_Mapped(&(ct_ਥਾਂ->cap), 0);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_X64_ਪੰਨਾ_Directory_Invocation_Map(cap_t cap, cte_t *ct_ਥਾਂ, pdpte_t pdpte, pdpte_t *pdpt_ਥਾਂ,
                                                        vspace_ਜੜ੍ਹ_t *vspace)
{
    ct_ਥਾਂ->cap = cap;
    *pdpt_ਥਾਂ = pdpte;
    invalidate_ਪੰਨਾ_Structure_Cache_ASID(pptr_ਮੰਜ਼ਿਲ_paddr(vspace), cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    ਵਾਪਸ EXCEPTION_NONE;
}


ਸਥਿਰ exception_t decode_X64_ਪੰਨਾ_Directory_Invocation(
    ਸ਼ਬਦ_t label,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cte_t *cte,
    cap_t cap,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਸ਼ਬਦ_t              vaddr;
    vm_ਗੁਣ_t     vm_attr;
    cap_t               vspaceCap;
    vspace_ਜੜ੍ਹ_t      *vspace;
    pdpte_t             pdpte;
    paddr_t             paddr;
    asid_t              asid;
    lookupPDPTSlot_ret_t pdpt_ਥਾਂ;

    ਜੇ (label == X86_ਪੰਨਾ_Directory_Unmap) {
        ਜੇ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            userError("X86PageDirectory: Cannot unmap if more than one cap exist.");
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);

        ਵਾਪਸ perform_X64_ਪੰਨਾ_Directory_Invocation_Unmap(cap, cte);
    }

    ਜੇ (label != X86_ਪੰਨਾ_Directory_Map) {
        userError("X64Directory: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("X64PageDirectory: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(cap)) {
        userError("X64PageDirectory: PD is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ਬਫਰ) & (~ਨਕਾਬ(PDPT_ਸੂਚਕਾਂਕ_ਵਿੱਥ));
    vm_attr = vm_ਗੁਣ_From_ਸ਼ਬਦ(getSyscallArg(1, ਬਫਰ));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (!is_Valid_Native_ਜੜ੍ਹ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ਜੜ੍ਹ_t *)pptr_ਦਾ_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    ਜੇ (vaddr > USER_ਸਿਖਰ) {
        userError("X64PageDirectory: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;

    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pdpt_ਥਾਂ = lookupPDPTSlot(vspace, vaddr);
    ਜੇ (pdpt_ਥਾਂ.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ ((pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(pdpt_ਥਾਂ.pdpt_ਥਾਂ) == pdpte_pdpte_pd &&
         pdpte_pdpte_pd_ptr_get_present(pdpt_ਥਾਂ.pdpt_ਥਾਂ)) ||
        (pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(pdpt_ਥਾਂ.pdpt_ਥਾਂ) == pdpte_pdpte_1g
         && pdpte_pdpte_1g_ptr_get_present(pdpt_ਥਾਂ.pdpt_ਥਾਂ))) {
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ਮੰਜ਼ਿਲ_paddr(PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(cap)));
    pdpte = makeUserPDPTEPageDirectory(paddr, vm_attr);

    cap = cap_ਪੰਨਾ_directory_cap_ਸੈੱਟ_cap_PDIs_Mapped(cap, 1);
    cap = cap_ਪੰਨਾ_directory_cap_ਸੈੱਟ_cap_PDMapped_ASID(cap, asid);
    cap = cap_ਪੰਨਾ_directory_cap_ਸੈੱਟ_cap_PDMapped_ਪਤਾ(cap, vaddr);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ perform_X64_ਪੰਨਾ_Directory_Invocation_Map(cap, cte, pdpte, pdpt_ਥਾਂ.pdpt_ਥਾਂ, vspace);
}

ਸਥਿਰ ਖਾਲੀ unmapPDPT(asid_t asid, vptr_t vaddr, pdpte_t *pdpt)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;
    pml4e_t *pml4_ਥਾਂ;

    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ;
    }

    pml4_ਥਾਂ = lookup_PML4_ਥਾਂ(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vaddr);

    /* check if the PML4 has the PDPT */
    ਜੇ (!(pml4e_ptr_get_present(pml4_ਥਾਂ) &&
          pml4e_ptr_get_pdpt_ਅਧਾਰ_ਪਤਾ(pml4_ਥਾਂ) == pptr_ਮੰਜ਼ਿਲ_paddr(pdpt))) {
        ਵਾਪਸ;
    }

    flushPDPT(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vaddr, pdpt, asid);

    *pml4_ਥਾਂ = makeUserPML4EInvalid();
}

ਸਥਿਰ exception_t performX64PDPTInvocationUnmap(cap_t cap, cte_t *ct_ਥਾਂ)
{
    ਜੇ (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        pdpte_t *pdpt = PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap));
        unmapPDPT(cap_pdpt_cap_get_capPDPTMappedASID(cap),
                  cap_pdpt_cap_get_cap_PDPTMapped_ਪਤਾ(cap),
                  pdpt);
        clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *)pdpt, cap_get_cap_ਮਾਪ_ਬਿੱਟ(cap));
    }

    cap_pdpt_cap_ptr_ਸੈੱਟ_cap_PDPTIs_Mapped(&(ct_ਥਾਂ->cap), 0);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t performX64PDPTInvocationMap(cap_t cap, cte_t *ct_ਥਾਂ, pml4e_t pml4e, pml4e_t *pml4_ਥਾਂ,
                                               vspace_ਜੜ੍ਹ_t *vspace)
{
    ct_ਥਾਂ->cap = cap;
    *pml4_ਥਾਂ = pml4e;
    invalidate_ਪੰਨਾ_Structure_Cache_ASID(pptr_ਮੰਜ਼ਿਲ_paddr(vspace), cap_pdpt_cap_get_capPDPTMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeX64PDPTInvocation(
    ਸ਼ਬਦ_t  label,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cte_t   *cte,
    cap_t   cap,
    ਸ਼ਬਦ_t  *ਬਫਰ)
{
    ਸ਼ਬਦ_t                  vaddr;
    vm_ਗੁਣ_t         attr;
    pml4e_t                *pml4_ਥਾਂ;
    cap_t                   vspaceCap;
    vspace_ਜੜ੍ਹ_t          *vspace;
    pml4e_t                 pml4e;
    paddr_t                 paddr;
    asid_t                  asid;

    ਜੇ (label == X86PDPTUnmap) {
        ਜੇ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            userError("X86PDPT: Cannot unmap if more than one cap exist.");
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);

        ਵਾਪਸ performX64PDPTInvocationUnmap(cap, cte);
    }

    ਜੇ (label != X86PDPTMap) {
        userError("X86PDPT: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("X64PDPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X64PDPT: PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ਬਫਰ) & (~ਨਕਾਬ(PML4_ਸੂਚਕਾਂਕ_ਵਿੱਥ));
    attr = vm_ਗੁਣ_From_ਸ਼ਬਦ(getSyscallArg(1, ਬਫਰ));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (!is_Valid_Native_ਜੜ੍ਹ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ਜੜ੍ਹ_t *)pptr_ਦਾ_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    ਜੇ (vaddr > USER_ਸਿਖਰ) {
        userError("X64PDPT: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;

    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    pml4_ਥਾਂ = lookup_PML4_ਥਾਂ(vspace, vaddr);

    ਜੇ (pml4e_ptr_get_present(pml4_ਥਾਂ)) {
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ਮੰਜ਼ਿਲ_paddr(PDPTE_PTR((cap_pdpt_cap_get_capPDPTBasePtr(cap))));
    pml4e = makeUserPML4E(paddr, attr);

    cap = cap_pdpt_cap_ਸੈੱਟ_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_pdpt_cap_ਸੈੱਟ_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_pdpt_cap_ਸੈੱਟ_cap_PDPTMapped_ਪਤਾ(cap, vaddr);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ performX64PDPTInvocationMap(cap, cte, pml4e, pml4_ਥਾਂ, vspace);
}

exception_t decode_X86_ਬਹੁਲਕ_MMUInvocation(
    ਸ਼ਬਦ_t label,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਚੋਣ (cap_get_capType(cap)) {

    ਹਾਲਤ cap_pml4_cap:
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;

    ਹਾਲਤ cap_pdpt_cap:
        ਵਾਪਸ decodeX64PDPTInvocation(label, ਲੰਮਾਈ, cte, cap, ਬਫਰ);

    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
        ਵਾਪਸ decode_X64_ਪੰਨਾ_Directory_Invocation(label, ਲੰਮਾਈ, cte, cap, ਬਫਰ);

    ਮੂਲ:
        fail("Invalid arch cap type");
    }
}

bool_t ਬਹੁਲਕ_Unmap_ਪੰਨਾ(vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, vspace_ਜੜ੍ਹ_t *vroot, vptr_t vaddr, ਖਾਲੀ *pptr)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_HUGE_ਪੰਨਾ) && ਪੰਨਾ_ਮਾਪ == X64_Huge_ਪੰਨਾ) {
        pdpte_t     *pdpte;
        lookupPDPTSlot_ret_t pdpt_ret = lookupPDPTSlot(vroot, vaddr);
        ਜੇ (pdpt_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ false;
        }
        pdpte = pdpt_ret.pdpt_ਥਾਂ;


        ਜੇ (!(pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(pdpte) == pdpte_pdpte_1g
              && pdpte_pdpte_1g_ptr_get_present(pdpte)
              && (pdpte_pdpte_1g_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pdpte)
                  == pptr_ਮੰਜ਼ਿਲ_paddr(pptr)))) {
            ਵਾਪਸ false;
        }

        *pdpte = makeUserPDPTEInvalid();
        ਵਾਪਸ true;
    }
    fail("Invalid page type");
    ਵਾਪਸ false;
}

ਸਥਿਰ exception_t updatePDPTE(asid_t asid, pdpte_t pdpte, pdpte_t *pdpt_ਥਾਂ, vspace_ਜੜ੍ਹ_t *vspace)
{
    *pdpt_ਥਾਂ = pdpte;
    invalidate_ਪੰਨਾ_Structure_Cache_ASID(pptr_ਮੰਜ਼ਿਲ_paddr(vspace), asid,
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_X64_ਬਹੁਲਕ_Map(cap_t cap, cte_t *ct_ਥਾਂ, pdpte_t pdpte, pdpte_t *pdpt_ਥਾਂ, vspace_ਜੜ੍ਹ_t *vspace)
{
    ct_ਥਾਂ->cap = cap;
    ਵਾਪਸ updatePDPTE(cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap), pdpte, pdpt_ਥਾਂ, vspace);
}

ਬਣਤਰ create_mapping_pdpte_return {
    exception_t ਸਥਿਤੀ;
    pdpte_t pdpte;
    pdpte_t *pdpt_ਥਾਂ;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ create_mapping_pdpte_return create_mapping_pdpte_return_t;

ਸਥਿਰ create_mapping_pdpte_return_t create_ਸੁਰੱਖਿਅਤ_Mapping_Entries_PDPTE(paddr_t ਅਧਾਰ, ਸ਼ਬਦ_t vaddr, vm_rights_t vmRights,
                                                                    vm_ਗੁਣ_t attr,
                                                                    vspace_ਜੜ੍ਹ_t *vspace)
{
    create_mapping_pdpte_return_t ret;
    lookupPDPTSlot_ret_t          lu_ret;

    lu_ret = lookupPDPTSlot(vspace, vaddr);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        ਵਾਪਸ ret;
    }
    ret.pdpt_ਥਾਂ = lu_ret.pdpt_ਥਾਂ;

    /* check for existing page directory */
    ਜੇ ((pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(ret.pdpt_ਥਾਂ) == pdpte_pdpte_pd) &&
        (pdpte_pdpte_pd_ptr_get_present(ret.pdpt_ਥਾਂ))) {
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        ਵਾਪਸ ret;
    }

    ret.pdpte = make_User_PDPTEHuge_ਪੰਨਾ(ਅਧਾਰ, attr, vmRights);
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

exception_t decode_X86_ਬਹੁਲਕ_Map_ਪੰਨਾ(ਸ਼ਬਦ_t label, vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, cte_t *cte, cap_t cap,
                                 vspace_ਜੜ੍ਹ_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_ਗੁਣ_t vm_attr)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_HUGE_ਪੰਨਾ) && ਪੰਨਾ_ਮਾਪ == X64_Huge_ਪੰਨਾ) {
        create_mapping_pdpte_return_t map_ret;

        map_ret = create_ਸੁਰੱਖਿਅਤ_Mapping_Entries_PDPTE(paddr, vaddr, vm_rights, vm_attr, vroot);
        ਜੇ (map_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ map_ret.ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);

        ਚੋਣ (label) {
        ਹਾਲਤ X86_ਪੰਨਾ_Map:
            ਵਾਪਸ perform_X64_ਬਹੁਲਕ_Map(cap, cte, map_ret.pdpte, map_ret.pdpt_ਥਾਂ, vroot);

        ਮੂਲ:
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
    }
    fail("Invalid Page type");
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ read_ਸ਼ਬਦ_From_VSpace_ret {
    exception_t ਸਥਿਤੀ;
    ਸ਼ਬਦ_t ਮੁੱਲ;
} read_ਸ਼ਬਦ_From_VSpace_ret_t;

ਸਥਿਰ read_ਸ਼ਬਦ_From_VSpace_ret_t read_ਸ਼ਬਦ_From_VSpace(vspace_ਜੜ੍ਹ_t *vspace, ਸ਼ਬਦ_t vaddr)
{
    read_ਸ਼ਬਦ_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_ਥਾਂ;
    lookupPDSlot_ret_t pd_ਥਾਂ;
    lookupPDPTSlot_ret_t pdpt_ਥਾਂ;
    paddr_t paddr;
    ਸ਼ਬਦ_t ਵਿੱਥ;
    pptr_t kernel_vaddr;
    ਸ਼ਬਦ_t *ਮੁੱਲ;

    pdpt_ਥਾਂ = lookupPDPTSlot(vspace, vaddr);
    ਜੇ (pdpt_ਥਾਂ.ਸਥਿਤੀ == EXCEPTION_NONE &&
        pdpte_ptr_get_ਪੰਨਾ_ਮਾਪ(pdpt_ਥਾਂ.pdpt_ਥਾਂ) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_ਥਾਂ.pdpt_ਥਾਂ)) {

        paddr = pdpte_pdpte_1g_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pdpt_ਥਾਂ.pdpt_ਥਾਂ);
        ਵਿੱਥ = vaddr & ਨਕਾਬ(se_L4_Huge_ਪੰਨਾ_ਬਿੱਟ);
    } ਨਹੀਂਤਾਂ {
        pd_ਥਾਂ = lookupPDSlot(vspace, vaddr);
        ਜੇ (pd_ਥਾਂ.ਸਥਿਤੀ == EXCEPTION_NONE &&
            ((pde_ptr_get_ਪੰਨਾ_ਮਾਪ(pd_ਥਾਂ.pd_ਥਾਂ) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_ਥਾਂ.pd_ਥਾਂ))) {

            paddr = pde_pde_large_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pd_ਥਾਂ.pd_ਥਾਂ);
            ਵਿੱਥ = vaddr & ਨਕਾਬ(se_L4_Large_ਪੰਨਾ_ਬਿੱਟ);
        } ਨਹੀਂਤਾਂ {
            pt_ਥਾਂ = lookupPTSlot(vspace, vaddr);
            ਜੇ (pt_ਥਾਂ.ਸਥਿਤੀ == EXCEPTION_NONE && pte_ptr_get_present(pt_ਥਾਂ.pt_ਥਾਂ)) {
                paddr = pte_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pt_ਥਾਂ.pt_ਥਾਂ);
                ਵਿੱਥ = vaddr & ਨਕਾਬ(se_L4_ਪੰਨਾ_ਬਿੱਟ);
            } ਨਹੀਂਤਾਂ {
                ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
                ਵਾਪਸ ret;
            }
        }
    }


    kernel_vaddr = (ਸ਼ਬਦ_t)paddr_ਮੰਜ਼ਿਲ_pptr(paddr);
    ਮੁੱਲ = (ਸ਼ਬਦ_t *)(kernel_vaddr + ਵਿੱਥ);
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ret.ਮੁੱਲ = *ਮੁੱਲ;
    ਵਾਪਸ ret;
}

ਖਾਲੀ Arch_user_ਢੇਰ_Trace(tcb_t *tptr)
{
    cap_t thread_ਜੜ੍ਹ;
    vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ;
    ਸ਼ਬਦ_t sp;
    ਪੂਰਨ i;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    ਜੇ (cap_get_capType(thread_ਜੜ੍ਹ) != cap_pml4_cap) {
        ਲਿਖੋ("Invalid vspace\n");
        ਵਾਪਸ;
    }

    vspace_ਜੜ੍ਹ = (vspace_ਜੜ੍ਹ_t *)pptr_ਦਾ_cap(thread_ਜੜ੍ਹ);

    sp = get_ਰਜਿਸਟਰ(tptr, RSP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ਜੇ (!IS_ALIGNED(sp, se_L4_ਸ਼ਬਦ_ਮਾਪ_ਬਿੱਟ)) {
        ਲਿਖੋ("RSP not aligned\n");
        ਵਾਪਸ;
    }

    ਲਈ (i = 0; i < CONFIG_USER_ਢੇਰ_TRACE_ਲੰਮਾਈ; i++) {
        ਸ਼ਬਦ_t ਪਤਾ = sp + (i * ਆਕਾਰ(ਸ਼ਬਦ_t));
        read_ਸ਼ਬਦ_From_VSpace_ret_t ਨਤੀਜਾ;
        ਨਤੀਜਾ = read_ਸ਼ਬਦ_From_VSpace(vspace_ਜੜ੍ਹ, ਪਤਾ);
        ਜੇ (ਨਤੀਜਾ.ਸਥਿਤੀ == EXCEPTION_NONE) {
            ਲਿਖੋ("0x%lx: 0x%lx\n", (ਲੰਮਾ)ਪਤਾ, (ਲੰਮਾ)ਨਤੀਜਾ.ਮੁੱਲ);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("0x%lx: INVALID\n", (ਲੰਮਾ)ਪਤਾ);
        }
    }
}
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

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

    pde_t pde = pde_pde_large_new(
                    0,                 /* xd                   */
                    ks_User_Log_ਬਫਰ,   /* page_base_address    */
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HUGE_ਪੰਨਾ
    x64KSKernelPD[1] = pde;
#ਨਹੀਂਤਾਂ
    x64KSKernelPDs[ਦੋਆਧਾਰੀ_ਅੰਕ(PDPT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1][1] = pde;
#ਜੇ_ਅੰਤ
    invalidateTranslationAll(ਨਕਾਬ(CONFIG_MAX_ਅੰਕ_NODES));

    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ /* CONFIG_KERNEL_LOG_BUFFER */
