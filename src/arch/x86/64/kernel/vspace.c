/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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
#ಅನ್ತರ್ಭಾವ <mode/kernel/tlb.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/tlb_bitmap.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>

/* When using the SKIM window to isolate the kernel from the user we also need to
 * not use global mappings as having global mappings and entries in the TLB is
 * equivalent, for the purpose of exploitation, to having the mappings in the
 * kernel window */
#ವ್ಯಾಖ್ಯಾನಿಸು KERNEL_IS_GLOBAL() (config_ಗಣ(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ) ? 0 : 1)

/* For the boot code we create two windows into the physical address space
 * One is at the same location as the kernel window, and is placed up high
 * The other is a 1-to-1 mapping of the first 512gb of memory. The purpose
 * of this is to have a 1-to-1 mapping for the low parts of memory, so that
 * when we switch paging on, and are still running at physical addresses,
 * we don't explode. Then we also want the high mappings so we can start
 * running at proper kernel virtual addresses */
pml4e_t boot_pml4[ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು)) VISIBLE PHYS_BSS;
pdpte_t boot_pdpt[ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)] ALIGN(ಬಿಟ್(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು)) VISIBLE PHYS_BSS;

/* 'gdt_idt_ptr' is declared globally because of a C-subset restriction.
 * It is only used in init_drts(), which therefore is non-reentrant.
 */
gdt_idt_ptr_t gdt_idt_ptr;

BOOT_ಸಂಕೇತ bool_t map_kernel_ಕಿಟಕಿ(
    uint32_t ಅಂಕೆ_ioapic,
    paddr_t   *ioapic_paddrs,
    uint32_t   ಅಂಕೆ_drhu,
    paddr_t   *drhu_list
)
{

    uint64_t paddr;
    uint64_t vaddr;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HUGE_ಪುಟ
    /* using 1 GiB page size */

    /* verify that the kernel window as at the last entry of the PML4 */
    ಪ್ರತಿಜ್ಞಾ(GET_PML4_ಸೂಚ್ಯಂಕ(PPTR_ತಳ) == ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    ಪ್ರತಿಜ್ಞಾ(GET_PML4_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತಳ) == ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);
    ಪ್ರತಿಜ್ಞಾ(GET_PDPT_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತಳ) == ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 2);
    ಪ್ರತಿಜ್ಞಾ(GET_PDPT_ಸೂಚ್ಯಂಕ(KDEV_ತಳ) == ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);
    ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED(KERNEL_ELF_ತಳ - KERNEL_ELF_PADDR_ತಳ, se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು));
    ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED(KDEV_ತಳ, se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು));
    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_ಸೂಚ್ಯಂಕ(PPTR_ತಳ)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_ಗಮ್ಯ_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );
    /* put the 1GB kernel_base mapping into the PDPT */
    x64KSKernelPDPT[GET_PDPT_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತಳ)] = pdpte_pdpte_1g_new(
                                                           0, /* xd */
                                                           PADDR_ತಳ,
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
    vaddr = PPTR_ತಳ;
    ಗಾಗಿ (paddr = 0; paddr < PADDR_ತುದಿ;
         paddr += ಬಿಟ್(se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು)) {

        ಪೂರ್ಣಾಂಕ pdpte_ಸೂಚ್ಯಂಕ = GET_PDPT_ಸೂಚ್ಯಂಕ(vaddr);
        x64KSKernelPDPT[pdpte_ಸೂಚ್ಯಂಕ] = pdpte_pdpte_1g_new(
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

        vaddr += ಬಿಟ್(se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    }

    /* put the PD into the PDPT */
    x64KSKernelPDPT[GET_PDPT_ಸೂಚ್ಯಂಕ(KDEV_ತಳ)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_ಗಮ್ಯ_paddr(x64KSKernelPD),
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
                           kpptr_ಗಮ್ಯ_paddr(x64KSKernelPT),
                           0, /* accessed */
                           0, /* cache_disabled */
                           0, /* write_through */
                           0, /* super_user */
                           1, /* read_write */
                           1  /* present */
                       );
#ಇಲ್ಲದಿದ್ದರೆ

    ಪೂರ್ಣಾಂಕ pd_ಸೂಚ್ಯಂಕ_2 = 0;
    /* use 2 MiB page size */
    /* verify that the kernel window as at the last entry of the PML4 */
    ಪ್ರತಿಜ್ಞಾ(GET_PML4_ಸೂಚ್ಯಂಕ(PPTR_ತಳ) == ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);
    /* verify that the kernel_base is located in the last entry of the PML4,
     * the second last entry of the PDPT, is 1gb aligned and 1gb in size */
    ಪ್ರತಿಜ್ಞಾ(GET_PML4_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತಳ) == ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);
    ಪ್ರತಿಜ್ಞಾ(GET_PDPT_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತಳ) == ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 2);
    ಪ್ರತಿಜ್ಞಾ(GET_PDPT_ಸೂಚ್ಯಂಕ(KDEV_ತಳ) == ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1);
    ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED(KERNEL_ELF_ತಳ - KERNEL_ELF_PADDR_ತಳ, se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು));
    ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED(KDEV_ತಳ, se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು));

    /* place the PDPT into the PML4 */
    x64KSKernelPML4[GET_PML4_ಸೂಚ್ಯಂಕ(PPTR_ತಳ)] = pml4e_new(
                                                     0, /* xd */
                                                     kpptr_ಗಮ್ಯ_paddr(x64KSKernelPDPT),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    ಗಾಗಿ (pd_ಸೂಚ್ಯಂಕ_2 = 0; pd_ಸೂಚ್ಯಂಕ_2 < PADDR_ತುದಿ >> se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು; pd_ಸೂಚ್ಯಂಕ_2++) {
        /* put the 1GB kernel_base mapping into the PDPT */
        x64KSKernelPDPT[GET_PDPT_ಸೂಚ್ಯಂಕ(PPTR_ತಳ) + pd_ಸೂಚ್ಯಂಕ_2] = pdpte_pdpte_pd_new(
                                                                    0, /* xd */
                                                                    kpptr_ಗಮ್ಯ_paddr(&x64KSKernelPDs[pd_ಸೂಚ್ಯಂಕ_2][0]),
                                                                    0, /* accessed */
                                                                    0, /* cache disabled */
                                                                    0, /* write through */
                                                                    0, /* super user */
                                                                    1, /* read write */
                                                                    1 /* present */
                                                                );
    }

    x64KSKernelPDPT[GET_PDPT_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತಳ)] = pdpte_pdpte_pd_new(
                                                           0, /* xd */
                                                           kpptr_ಗಮ್ಯ_paddr(&x64KSKernelPDs[0][0]),
                                                           0, /* accessed */
                                                           0, /* cache disable */
                                                           1, /* write through */
                                                           0, /* super user */
                                                           1, /* read write */
                                                           1  /* present */
                                                       );

    paddr = 0;
    vaddr = PPTR_ತಳ;

    ಗಾಗಿ (paddr = 0; paddr < PADDR_ತುದಿ;
         paddr += 0x200000) {

        ಪೂರ್ಣಾಂಕ pd_ಸೂಚ್ಯಂಕ_2 = GET_PDPT_ಸೂಚ್ಯಂಕ(vaddr) - GET_PDPT_ಸೂಚ್ಯಂಕ(PPTR_ತಳ);
        ಪೂರ್ಣಾಂಕ pde_ಸೂಚ್ಯಂಕ = GET_PD_ಸೂಚ್ಯಂಕ(vaddr);

        x64KSKernelPDs[pd_ಸೂಚ್ಯಂಕ_2][pde_ಸೂಚ್ಯಂಕ] = pde_pde_large_new(
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
    x64KSKernelPDPT[GET_PDPT_ಸೂಚ್ಯಂಕ(KDEV_ತಳ)] = pdpte_pdpte_pd_new(
                                                     0, /* xd */
                                                     kpptr_ಗಮ್ಯ_paddr(&x64KSKernelPDs[ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1][0]),
                                                     0, /* accessed */
                                                     0, /* cache_disabled */
                                                     0, /* write_through */
                                                     0, /* super_user */
                                                     1, /* read_write */
                                                     1  /* present */
                                                 );

    /* put the PT into the PD */
    x64KSKernelPDs[ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1][0] = pde_pde_pt_new(
                                                      0, /* xd */
                                                      kpptr_ಗಮ್ಯ_paddr(x64KSKernelPT),
                                                      0, /* accessed */
                                                      0, /* cache_disabled */
                                                      0, /* write_through */
                                                      0, /* super_user */
                                                      1, /* read_write */
                                                      1  /* present */
                                                  );
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ CONFIG_MAX_ಅಂಕೆ_TRACE_POINTS > 0
    /* use the last PD entry as the benchmark log storage.
     * the actual backing physical memory will be filled
     * later by using alloc_region */
    ksLog = (ks_log_ನಮೂದು_t *)(KDEV_ತಳ + 0x200000 * (ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1));
#ಆದರೆ_ಅಂತ್ಯ

    /* now map in the kernel devices */
    ಆದರೆ (!map_kernel_ಕಿಟಕಿ_devices(x64KSKernelPT, ಅಂಕೆ_ioapic, ioapic_paddrs, ಅಂಕೆ_drhu, drhu_list)) {
        ಹಿಂತಿರುಗಿ false;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    /* initialize the TLB bitmap */
    tlb_bitmap_init(x64KSKernelPML4);
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    /* In boot code, so fine to just trash everything here */
    invalidateLocalTranslationAll();
    ಬರೆ("Mapping kernel window is done\n");
    ಹಿಂತಿರುಗಿ true;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
BOOT_ಸಂಕೇತ bool_t map_skim_ಕಿಟಕಿ(vptr_t skim_ಆರಂಭ, vptr_t skim_ಕೊನೆ)
{
    /* place the PDPT into the PML4 */
    x64KSSKIMPML4[GET_PML4_ಸೂಚ್ಯಂಕ(PPTR_ತಳ)] = pml4e_new(
                                                   0, /* xd */
                                                   kpptr_ಗಮ್ಯ_paddr(x64KSSKIMPDPT),
                                                   0, /* accessed */
                                                   0, /* cache_disabled */
                                                   0, /* write_through */
                                                   0, /* super_user */
                                                   1, /* read_write */
                                                   1  /* present */
                                               );
    /* place the PD into the kernel_base slot of the PDPT */
    x64KSSKIMPDPT[GET_PDPT_ಸೂಚ್ಯಂಕ(KERNEL_ELF_ತಳ)] = pdpte_pdpte_pd_new(
                                                         0, /* xd */
                                                         kpptr_ಗಮ್ಯ_paddr(x64KSSKIMPD),
                                                         0, /* accessed */
                                                         0, /* cache_disabled */
                                                         0, /* write_through */
                                                         0, /* super_user */
                                                         1, /* read_write */
                                                         1  /* present */
                                                     );
    /* map the skim portion into the PD. we expect it to be 2M aligned */
    ಪ್ರತಿಜ್ಞಾ((skim_ಆರಂಭ % ಬಿಟ್(se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು)) == 0);
    ಪ್ರತಿಜ್ಞಾ((skim_ಕೊನೆ % ಬಿಟ್(se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು)) == 0);
    uint64_t paddr = kpptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)skim_ಆರಂಭ);
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = GET_PD_ಸೂಚ್ಯಂಕ(skim_ಆರಂಭ); i < GET_PD_ಸೂಚ್ಯಂಕ(skim_ಕೊನೆ); i++) {
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
        paddr += ಬಿಟ್(se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    }
    ಹಿಂತಿರುಗಿ true;
}
#ಆದರೆ_ಅಂತ್ಯ

BOOT_ಸಂಕೇತ ಶೂನ್ಯ init_tss(tss_t *tss)
{
    ಶಬ್ದ_t ತಳ = (ಶಬ್ದ_t)&x64KSIRQStack[CURRENT_CPU_ಸೂಚ್ಯಂಕ()][IRQ_ರಾಶಿ_ಗಾತ್ರ];
    *tss = tss_new(
               ಗಾತ್ರ(*tss),   /* io map base */
               0, 0,       /* ist 7 */
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               0, 0,
               /* ist 1 is the stack frame we use for interrupts */
               ತಳ >> 32, ತಳ & 0xffffffff,  /* ist 1 */
               0, 0,       /* rsp 2 */
               0, 0,       /* rsp 1 */
               0, 0        /* rsp 0 */
           );
    /* set the IO map to all 1 to block user IN/OUT instructions */
    memset(&x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KStss.io_map[0], 0xff,
           ಗಾತ್ರ(x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KStss.io_map));
}

ಶೂನ್ಯ init_syscall_msrs(ಶೂನ್ಯ)
{
    x86_wrmsr(IA32_LSTAR_MSR, (uint64_t)&handle_fastsyscall);
    // mask bit 9 in the kernel (which is the interrupt enable bit)
    // also mask bit 8, which is the Trap Flag, to prevent the kernel
    // from single stepping
    x86_wrmsr(IA32_FMASK_MSR, FLAGS_TF | FLAGS_IF);
    x86_wrmsr(IA32_ನಕ್ಷತ್ರ_MSR, ((uint64_t)SEL_CS_0 << 32) | ((uint64_t)SEL_CS_3 << 48));
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ init_gdt(gdt_ನಮೂದು_t *gdt, tss_t *tss)
{

    uint64_t tss_ತಳ = (uint64_t)tss;
    gdt_tss_t gdt_tss;

    gdt[GDT_NULL] = gdt_ನಮೂದು_gdt_null_new();

    gdt[GDT_CS_0] = gdt_ನಮೂದು_gdt_ಸಂಕೇತ_new(
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

    gdt[GDT_DS_0] = gdt_ನಮೂದು_gdt_ದತ್ತಾಂಶ_new(
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

    gdt[GDT_CS_3] = gdt_ನಮೂದು_gdt_ಸಂಕೇತ_new(
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

    gdt[GDT_DS_3] = gdt_ನಮೂದು_gdt_ದತ್ತಾಂಶ_new(
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

    gdt[GDT_FS] = gdt_ನಮೂದು_gdt_ದತ್ತಾಂಶ_new(
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

    gdt[GDT_GS] = gdt_ನಮೂದು_gdt_ದತ್ತಾಂಶ_new(
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
                  tss_ತಳ >> 32,                     /* base 63 - 32 */
                  (tss_ತಳ & 0xff000000UL) >> 24,    /* base 31 - 24 */
                  1,                                  /* granularity */
                  0,                                  /* avl */
                  0,                                  /* limit high */
                  1,                                  /* present */
                  0,                                  /* dpl */
                  9,                                  /* desc type */
                  (tss_ತಳ & 0xff0000UL) >> 16,      /* base 23-16 */
                  (tss_ತಳ & 0xffffUL),              /* base 15 - 0 */
                  ಗಾತ್ರ(tss_io_t) - 1
              );

    gdt[GDT_TSS].ಪದಗಳು[0] = gdt_tss.ಪದಗಳು[0];
    gdt[GDT_TSS + 1].ಪದಗಳು[0] = gdt_tss.ಪದಗಳು[1];
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ init_idt_ನಮೂದು(idt_ನಮೂದು_t *idt, ಅಡಚಣೆ_t ಅಡಚಣೆ, ಶೂನ್ಯ(*handler)(ಶೂನ್ಯ))
{
    uint64_t handler_addr = (uint64_t)handler;
    uint64_t dpl = 3;

    ಆದರೆ (ಅಡಚಣೆ < int_trap_min && ಅಡಚಣೆ != int_software_break_request) {
        dpl = 0;
    }

    idt[ಅಡಚಣೆ] = idt_ನಮೂದು_ಅಡಚಣೆ_gate_new(
                         handler_addr >> 32,                 /* offset 63 - 32 */
                         ((handler_addr >> 16) & 0xffff),
                         1,                                  /* present */
                         dpl,                                /* dpl */
                         1,                                  /* ist */
                         SEL_CS_0,                           /* segment selector */
                         (handler_addr & 0xffff)               /* offset 15 - 0 */
                     );
}

ಶೂನ್ಯ ಗಣ_VMRoot(tcb_t *tcb)
{
    cap_t thread_ಬೇರು;
    asid_t asid;
    pml4e_t *pml4;
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    cr3_t cr3;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap;

    ಆದರೆ (cap_get_capType(thread_ಬೇರು) != cap_pml4_cap ||
        !cap_pml4_cap_get_capPML4IsMapped(thread_ಬೇರು)) {
        ಗಣ_Current_User_VSpace_ಬೇರು(kpptr_ಗಮ್ಯ_paddr(X86_GLOBAL_VSPACE_ಬೇರು), 0);
        ಹಿಂತಿರುಗಿ;
    }

    pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_ತಳ_Ptr(thread_ಬೇರು));
    asid = cap_pml4_cap_get_capPML4MappedASID(thread_ಬೇರು);
    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE || ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != pml4)) {
        ಗಣ_Current_User_VSpace_ಬೇರು(kpptr_ಗಮ್ಯ_paddr(X86_GLOBAL_VSPACE_ಬೇರು), 0);
        ಹಿಂತಿರುಗಿ;
    }
    cr3 = makeCR3(pptr_ಗಮ್ಯ_paddr(pml4), asid);
    ಆದರೆ (getCurrentUserCR3().ಪದಗಳು[0] != cr3.ಪದಗಳು[0]) {
        SMP_COND_STATEMENT(tlb_bitmap_ಗಣ(pml4, getCurrentCPUIndex());)
        ಗಣ_Current_User_CR3(cr3);
    }
}


BOOT_ಸಂಕೇತ ಶೂನ್ಯ init_dtrs(ಶೂನ್ಯ)
{
    gdt_idt_ptr.ಎಲ್ಲೆ = (ಗಾತ್ರ(gdt_ನಮೂದು_t) * GDT_ENTRIES) - 1;
    gdt_idt_ptr.ತಳ = (uint64_t)x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KSgdt;

    /* When we install the gdt it will clobber any value of gs that
     * we have. Since we might be using it for TLS we can stash
     * and unstash any gs value using swapgs
     */
    swapgs();
    x64_install_gdt(&gdt_idt_ptr);
    swapgs();

    gdt_idt_ptr.ಎಲ್ಲೆ = (ಗಾತ್ರ(idt_ನಮೂದು_t) * (int_max + 1)) - 1;
    gdt_idt_ptr.ತಳ = (uint64_t)x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KSidt;
    x64_install_idt(&gdt_idt_ptr);

    x64_install_ldt(SEL_NULL);

    x64_install_tss(SEL_TSS);
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_ಚೌಕಟ್ಟು_cap(cap_t pd_cap, cap_t ಚೌಕಟ್ಟು_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_ದ_cap(pd_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt;
    vptr_t vptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(ಚೌಕಟ್ಟು_cap);
    ಶೂನ್ಯ *pptr = (ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(ಚೌಕಟ್ಟು_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMap_Type(ಚೌಕಟ್ಟು_cap) == X86_MappingVSpace);
    ಪ್ರತಿಜ್ಞಾ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(ಚೌಕಟ್ಟು_cap) != asidInvalid);
    pml4 += GET_PML4_ಸೂಚ್ಯಂಕ(vptr);
    ಪ್ರತಿಜ್ಞಾ(pml4e_ptr_get_present(pml4));
    pdpt = paddr_ಗಮ್ಯ_pptr(pml4e_ptr_get_pdpt_ತಳ_ವಿಳಾಸ(pml4));
    pdpt += GET_PDPT_ಸೂಚ್ಯಂಕ(vptr);
    ಪ್ರತಿಜ್ಞಾ(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_ಗಮ್ಯ_pptr(pdpte_pdpte_pd_ptr_get_pd_ತಳ_ವಿಳಾಸ(pdpt));
    pd += GET_PD_ಸೂಚ್ಯಂಕ(vptr);
    ಪ್ರತಿಜ್ಞಾ(pde_pde_pt_ptr_get_present(pd));
    pt = paddr_ಗಮ್ಯ_pptr(pde_pde_pt_ptr_get_pt_ತಳ_ವಿಳಾಸ(pd));
    *(pt + GET_PT_ಸೂಚ್ಯಂಕ(vptr)) = pte_new(
                                     0,                      /* xd                   */
                                     pptr_ಗಮ್ಯ_paddr(pptr),    /* page_base_address    */
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

ಸ್ಥಿರ BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_pdpt_cap(cap_t vspace_cap, cap_t pdpt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_ದ_cap(vspace_cap));
    pdpte_t *pdpt = PDPT_PTR(cap_pdpt_cap_get_capPDPTBasePtr(pdpt_cap));
    vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_ವಿಳಾಸ(pdpt_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_pdpt_cap_get_capPDPTIsMapped(pdpt_cap));
    *(pml4 + GET_PML4_ಸೂಚ್ಯಂಕ(vptr)) = pml4e_new(
                                         0,                      /* xd                   */
                                         pptr_ಗಮ್ಯ_paddr(pdpt),    /* pdpt_base_address    */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_pd_cap(cap_t vspace_cap, cap_t pd_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_ದ_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd = PD_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(pd_cap));
    vptr_t vptr = cap_ಪುಟ_directory_cap_get_cap_PDMapped_ವಿಳಾಸ(pd_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(pd_cap));
    pml4 += GET_PML4_ಸೂಚ್ಯಂಕ(vptr);
    ಪ್ರತಿಜ್ಞಾ(pml4e_ptr_get_present(pml4));
    pdpt = paddr_ಗಮ್ಯ_pptr(pml4e_ptr_get_pdpt_ತಳ_ವಿಳಾಸ(pml4));
    *(pdpt + GET_PDPT_ಸೂಚ್ಯಂಕ(vptr)) = pdpte_pdpte_pd_new(
                                         0,                      /* xd                   */
                                         pptr_ಗಮ್ಯ_paddr(pd),      /* pd_base_address      */
                                         0,                      /* accessed             */
                                         0,                      /* cache_disabled       */
                                         0,                      /* write_through        */
                                         1,                      /* super_user           */
                                         1,                      /* read_write           */
                                         1                       /* present              */
                                     );
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ map_it_pt_cap(cap_t vspace_cap, cap_t pt_cap)
{
    pml4e_t *pml4 = PML4_PTR(pptr_ದ_cap(vspace_cap));
    pdpte_t *pdpt;
    pde_t *pd;
    pte_t *pt = PT_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(pt_cap));
    vptr_t vptr = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(pt_cap);

    ಪ್ರತಿಜ್ಞಾ(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(pt_cap));
    pml4 += GET_PML4_ಸೂಚ್ಯಂಕ(vptr);
    ಪ್ರತಿಜ್ಞಾ(pml4e_ptr_get_present(pml4));
    pdpt = paddr_ಗಮ್ಯ_pptr(pml4e_ptr_get_pdpt_ತಳ_ವಿಳಾಸ(pml4));
    pdpt += GET_PDPT_ಸೂಚ್ಯಂಕ(vptr);
    ಪ್ರತಿಜ್ಞಾ(pdpte_pdpte_pd_ptr_get_present(pdpt));
    pd = paddr_ಗಮ್ಯ_pptr(pdpte_pdpte_pd_ptr_get_pd_ತಳ_ವಿಳಾಸ(pdpt));
    *(pd + GET_PD_ಸೂಚ್ಯಂಕ(vptr)) = pde_pde_pt_new(
                                     0,                      /* xd                   */
                                     pptr_ಗಮ್ಯ_paddr(pt),      /* pt_base_address      */
                                     0,                      /* accessed             */
                                     0,                      /* cache_disabled       */
                                     0,                      /* write_through        */
                                     1,                      /* super_user           */
                                     1,                      /* read_write           */
                                     1                       /* present              */
                                 );
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ *map_ತಾತ್ಕಾಲಿಕ_boot_ಪುಟ(ಶೂನ್ಯ *ನಮೂದು, uint32_t large_ಪುಟಗಳು)
{
    /* this function is for legacy 32-bit systems where the ACPI tables might
     * collide with the kernel window. Here we just assert that the table is
     * in fact in the lower 4GiB region (which is already 1-to-1 mapped) and
     * continue */
    ಪ್ರತಿಜ್ಞಾ((ಶಬ್ದ_t)ನಮೂದು < ಬಿಟ್(32));
    ಹಿಂತಿರುಗಿ ನಮೂದು;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_pdpt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_pdpt_cap_new(
              asid,   /* capPDPTMappedASID    */
              pptr,   /* capPDPTBasePtr       */
              1,      /* capPDPTIsMapped      */
              vptr    /* capPDPTMappedAddress */
          );
    map_it_pdpt_cap(vspace_cap, cap);
    ಹಿಂತಿರುಗಿ cap;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_pd_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ಪುಟ_directory_cap_new(
              asid,   /* capPDMappedASID      */
              pptr,   /* capPDBasePtr         */
              1,      /* capPDIsMapped        */
              vptr    /* capPDMappedAddress   */
          );
    map_it_pd_cap(vspace_cap, cap);
    ಹಿಂತಿರುಗಿ cap;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_pt_cap(cap_t vspace_cap, pptr_t pptr, vptr_t vptr, asid_t asid)
{
    cap_t cap;
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
              asid,   /* capPTMappedASID      */
              pptr,   /* capPTBasePtr         */
              1,      /* capPTIsMapped        */
              vptr    /* capPTMappedAddress   */
          );
    map_it_pt_cap(vspace_cap, cap);
    ಹಿಂತಿರುಗಿ cap;
}


BOOT_ಸಂಕೇತ ಶಬ್ದ_t arch_get_n_paging(v_region_t it_v_reg)
{
    ಶಬ್ದ_t n = get_n_paging(it_v_reg, PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);
    n += get_n_paging(it_v_reg, PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);
    n += get_n_paging(it_v_reg, PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    n += vtd_get_n_paging(&boot_state.rmrr_list);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ n;
}

BOOT_ಸಂಕೇತ cap_t create_it_ವಿಳಾಸ_space(cap_t ಬೇರು_cnode_cap, v_region_t it_v_reg)
{
    cap_t      vspace_cap;
    vptr_t     vptr;
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_before;
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_after;

    ಸ್ಥಾನ_pos_before = ndks_boot.ಸ್ಥಾನ_pos_cur;
    copyGlobalMappings(PML4_PTR(rootserver.vspace));
    vspace_cap = cap_pml4_cap_new(
                     IT_ASID,        /* capPML4MappedASID */
                     rootserver.vspace,           /* capPML4BasePtr   */
                     1               /* capPML4IsMapped   */
                 );


    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadVSpace), vspace_cap);

    /* Create any PDPTs needed for the user land image */
    ಗಾಗಿ (vptr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(it_v_reg.ಆರಂಭ, PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);
         vptr < it_v_reg.ಕೊನೆ;
         vptr += ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap,
                         create_it_pdpt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }
    }

    /* Create any PDs needed for the user land image */
    ಗಾಗಿ (vptr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(it_v_reg.ಆರಂಭ, PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);
         vptr < it_v_reg.ಕೊನೆ;
         vptr += ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap,
                         create_it_pd_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
           ) {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }
    }

    /* Create any PTs needed for the user land image */
    ಗಾಗಿ (vptr = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(it_v_reg.ಆರಂಭ, PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);
         vptr < it_v_reg.ಕೊನೆ;
         vptr += ಬಿಟ್(PD_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ)) {
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap,
                         create_it_pt_cap(vspace_cap, it_alloc_paging(), vptr, IT_ASID))
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

ಶೂನ್ಯ copyGlobalMappings(vspace_ಬೇರು_t *new_vspace)
{
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;
    pml4e_t *vspace = (pml4e_t *)new_vspace;

    /* Copy from the tlbbitmap_pptr so that we copy the default entries of the
     * tlb bitmap (if it exists). If it doesn't exist then this loop
     * will be equivalent to copying from PPTR_BASE
     */
    ಗಾಗಿ (i = GET_PML4_ಸೂಚ್ಯಂಕ(TLBBITMAP_PPTR); i < ಬಿಟ್(PML4_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        vspace[i] = X86_GLOBAL_VSPACE_ಬೇರು[i];
    }
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ cap_t create_it_ಚೌಕಟ್ಟು_cap(pptr_t pptr, vptr_t vptr, asid_t asid, bool_t use_large, se_L4_ಶಬ್ದ map_type)
{
    vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ_2;

    ಆದರೆ (use_large) {
        ಚೌಕಟ್ಟು_ಗಾತ್ರ_2 = X86_Large_ಪುಟ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಚೌಕಟ್ಟು_ಗಾತ್ರ_2 = X86_Small_ಪುಟ;
    }

    ಹಿಂತಿರುಗಿ
        cap_ಚೌಕಟ್ಟು_cap_new(
            asid,                          /* capFMappedASID     */
            pptr,                          /* capFBasePtr        */
            ಚೌಕಟ್ಟು_ಗಾತ್ರ_2,                    /* capFSize           */
            map_type,                      /* capFMapType        */
            vptr,                          /* capFMappedAddress  */
            ಶಬ್ದ_From_VMRights(VMReadWrite), /* capFVMRights       */
            0                              /* capFIsDevice       */
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

/* ====================== BOOT CODE FINISHES HERE ======================== */



exception_t performASIDPoolInvocation(asid_t asid, asid_pool_t *poolPtr, cte_t *vspace_Cap_ಸ್ಥಾನ)
{
    asid_map_t asid_map;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಆದರೆ (cap_get_capType(vspace_Cap_ಸ್ಥಾನ->cap) == cap_ept_pml4_cap) {
        cap_ept_pml4_cap_ptr_ಗಣ_cap_PML4_Mapped_ASID(&vspace_Cap_ಸ್ಥಾನ->cap, asid);
        cap_ept_pml4_cap_ptr_ಗಣ_cap_PML4_Is_Mapped(&vspace_Cap_ಸ್ಥಾನ->cap, 1);
        asid_map = asid_map_asid_map_ept_new(cap_ept_pml4_cap_get_cap_PML4_ತಳ_Ptr(vspace_Cap_ಸ್ಥಾನ->cap));
    } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
        ಪ್ರತಿಜ್ಞಾ(cap_get_capType(vspace_Cap_ಸ್ಥಾನ->cap) == cap_pml4_cap);
        cap_pml4_cap_ptr_ಗಣ_cap_PML4_Mapped_ASID(&vspace_Cap_ಸ್ಥಾನ->cap, asid);
        cap_pml4_cap_ptr_ಗಣ_cap_PML4_Is_Mapped(&vspace_Cap_ಸ್ಥಾನ->cap, 1);
        asid_map = asid_map_asid_map_vspace_new(cap_pml4_cap_get_cap_PML4_ತಳ_Ptr(vspace_Cap_ಸ್ಥಾನ->cap));
    }
    poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = asid_map;
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

bool_t CONST is_VTable_ಬೇರು(cap_t cap)
{
    ಹಿಂತಿರುಗಿ cap_get_capType(cap) == cap_pml4_cap;
}

bool_t CONST is_Valid_Native_ಬೇರು(cap_t cap)
{
    ಹಿಂತಿರುಗಿ is_VTable_ಬೇರು(cap) &&
           cap_pml4_cap_get_capPML4IsMapped(cap);
}

ಸ್ಥಿರ pml4e_t CONST makeUserPML4E(paddr_t paddr, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr)
{
    ಹಿಂತಿರುಗಿ pml4e_new(
               0,
               paddr,
               0,
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PCDBit(vm_attr),
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PWTBit(vm_attr),
               1,
               1,
               1
           );
}

ಸ್ಥಿರ pml4e_t CONST makeUserPML4EInvalid(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ pml4e_new(
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

ಸ್ಥಿರ pdpte_t CONST make_User_PDPTEHuge_ಪುಟ(paddr_t paddr, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr, vm_rights_t vm_rights)
{
    ಹಿಂತಿರುಗಿ pdpte_pdpte_1g_new(
               0,          /* xd               */
               paddr,      /* physical address */
               0,          /* PAT              */
               0,          /* global           */
               0,          /* dirty            */
               0,          /* accessed         */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PWTBit(vm_attr),  /* write through  */
               SuperUserFromVMRights(vm_rights),       /* super user     */
               WritableFromVMRights(vm_rights),        /* read write     */
               1                                       /* present        */
           );
}

ಸ್ಥಿರ pdpte_t CONST makeUserPDPTEPageDirectory(paddr_t paddr, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr)
{
    ಹಿಂತಿರುಗಿ pdpte_pdpte_pd_new(
               0,                      /* xd       */
               paddr,                  /* paddr    */
               0,                      /* accessed */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PCDBit(vm_attr),  /* cache disabled */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PWTBit(vm_attr),  /* write through  */
               1,                      /* super user */
               1,                      /* read write */
               1                       /* present    */
           );
}

ಸ್ಥಿರ pdpte_t CONST makeUserPDPTEInvalid(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ pdpte_pdpte_pd_new(
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

pde_t CONST make_User_PDELarge_ಪುಟ(paddr_t paddr, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr, vm_rights_t vm_rights)
{
    ಹಿಂತಿರುಗಿ pde_pde_large_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* global               */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pde_t CONST make_User_PDEPage_ಕೋಷ್ಟಕ(paddr_t paddr, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr)
{

    ಹಿಂತಿರುಗಿ  pde_pde_pt_new(
                0,                                  /* xd               */
                paddr,                              /* pt_base_address  */
                0,                                  /* accessed         */
                vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PCDBit(vm_attr), /* cache_disabled   */
                vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PWTBit(vm_attr), /* write_through    */
                1,                                  /* super_user       */
                1,                                  /* read_write       */
                1                                   /* present          */
            );
}

pde_t CONST makeUserPDEInvalid(ಶೂನ್ಯ)
{
    /* The bitfield only declares two kinds of PDE entries (page tables or large pages)
     * and an invalid entry should really be a third type, but we can simulate it by
     * creating an invalid (present bit 0) entry of either of the defined types */
    ಹಿಂತಿರುಗಿ pde_pde_pt_new(
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

pte_t CONST makeUserPTE(paddr_t paddr, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr, vm_rights_t vm_rights)
{
    ಹಿಂತಿರುಗಿ pte_new(
               0,                                              /* xd                   */
               paddr,                                          /* page_base_address    */
               0,                                              /* global               */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PATBit(vm_attr),          /* pat                  */
               0,                                              /* dirty                */
               0,                                              /* accessed             */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PCDBit(vm_attr),          /* cache_disabled       */
               vm_ಗುಣಲಕ್ಷಣಗಳು_get_x86_PWTBit(vm_attr),          /* write_through        */
               SuperUserFromVMRights(vm_rights),               /* super_user           */
               WritableFromVMRights(vm_rights),                /* read_write           */
               1                                               /* present              */
           );
}

pte_t CONST makeUserPTEInvalid(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ pte_new(
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


ಸ್ಥಿರ pml4e_t *lookup_PML4_ಸ್ಥಾನ(vspace_ಬೇರು_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4e = PML4E_PTR(pml4);
    ಶಬ್ದ_t pml4_ಸೂಚ್ಯಂಕ = GET_PML4_ಸೂಚ್ಯಂಕ(vptr);
    ಹಿಂತಿರುಗಿ pml4e + pml4_ಸೂಚ್ಯಂಕ;
}

ಸ್ಥಿರ lookupPDPTSlot_ret_t lookupPDPTSlot(vspace_ಬೇರು_t *pml4, vptr_t vptr)
{
    pml4e_t *pml4_ಸ್ಥಾನ = lookup_PML4_ಸ್ಥಾನ(pml4, vptr);
    lookupPDPTSlot_ret_t ret;

    ಆದರೆ (!pml4e_ptr_get_present(pml4_ಸ್ಥಾನ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);

        ret.pdpt_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    } ಇಲ್ಲದಿದ್ದರೆ {
        pdpte_t *pdpt;
        pdpte_t *pdpt_ಸ್ಥಾನ;
        ಶಬ್ದ_t pdpt_ಸೂಚ್ಯಂಕ = GET_PDPT_ಸೂಚ್ಯಂಕ(vptr);
        pdpt = paddr_ಗಮ್ಯ_pptr(pml4e_ptr_get_pdpt_ತಳ_ವಿಳಾಸ(pml4_ಸ್ಥಾನ));
        pdpt_ಸ್ಥಾನ = pdpt + pdpt_ಸೂಚ್ಯಂಕ;

        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ret.pdpt_ಸ್ಥಾನ = pdpt_ಸ್ಥಾನ;
        ಹಿಂತಿರುಗಿ ret;
    }
}

lookupPDSlot_ret_t lookupPDSlot(vspace_ಬೇರು_t *pml4, vptr_t vptr)
{
    lookupPDPTSlot_ret_t pdpt_ಸ್ಥಾನ;
    lookupPDSlot_ret_t ret;

    pdpt_ಸ್ಥಾನ = lookupPDPTSlot(pml4, vptr);

    ಆದರೆ (pdpt_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ret.pd_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = pdpt_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2;
        ಹಿಂತಿರುಗಿ ret;
    }
    ಆದರೆ ((pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ) != pdpte_pdpte_pd) ||
        !pdpte_pdpte_pd_ptr_get_present(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ);

        ret.pd_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    } ಇಲ್ಲದಿದ್ದರೆ {
        pde_t *pd;
        pde_t *pd_ಸ್ಥಾನ;
        ಶಬ್ದ_t pd_ಸೂಚ್ಯಂಕ = GET_PD_ಸೂಚ್ಯಂಕ(vptr);
        pd = paddr_ಗಮ್ಯ_pptr(pdpte_pdpte_pd_ptr_get_pd_ತಳ_ವಿಳಾಸ(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ));
        pd_ಸ್ಥಾನ = pd + pd_ಸೂಚ್ಯಂಕ;

        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ret.pd_ಸ್ಥಾನ = pd_ಸ್ಥಾನ;
        ಹಿಂತಿರುಗಿ ret;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ flushPD(vspace_ಬೇರು_t *vspace, ಶಬ್ದ_t vptr, pde_t *pd, asid_t asid)
{
    /* clearing the entire PCID vs flushing the virtual addresses
     * one by one using invplg.
     * choose the easy way, invalidate the PCID
     */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));

}

ಸ್ಥಿರ ಶೂನ್ಯ flushPDPT(vspace_ಬೇರು_t *vspace, ಶಬ್ದ_t vptr, pdpte_t *pdpt, asid_t asid)
{
    /* similar here */
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    ಹಿಂತಿರುಗಿ;
}

ಶೂನ್ಯ hwASIDInvalidate(asid_t asid, vspace_ಬೇರು_t *vspace)
{
    invalidateASID(vspace, asid, SMP_TERNARY(tlb_bitmap_get(vspace), 0));
}

ಶೂನ್ಯ unmap_ಪುಟ_Directory(asid_t asid, vptr_t vaddr, pde_t *pd)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    lookupPDPTSlot_ret_t    lu_ret;

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ;
    }

    lu_ret = lookupPDPTSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vaddr);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ;
    }

    /* check if the PDPT has the PD */
    ಆದರೆ (!(pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.pdpt_ಸ್ಥಾನ) == pdpte_pdpte_pd &&
          pdpte_pdpte_pd_ptr_get_present(lu_ret.pdpt_ಸ್ಥಾನ) &&
          (pdpte_pdpte_pd_ptr_get_pd_ತಳ_ವಿಳಾಸ(lu_ret.pdpt_ಸ್ಥಾನ) == pptr_ಗಮ್ಯ_paddr(pd)))) {
        ಹಿಂತಿರುಗಿ;
    }

    flushPD(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vaddr, pd, asid);

    *lu_ret.pdpt_ಸ್ಥಾನ = makeUserPDPTEInvalid();

    invalidate_ಪುಟ_Structure_Cache_ASID(pptr_ಗಮ್ಯ_paddr(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು), asid,
                                     SMP_TERNARY(tlb_bitmap_get(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು), 0));
}


ಸ್ಥಿರ exception_t perform_X64_ಪುಟ_Directory_Invocation_Unmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{

    ಆದರೆ (cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(cap)) {
        pde_t *pd = PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(cap));
        unmap_ಪುಟ_Directory(
            cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(cap),
            cap_ಪುಟ_directory_cap_get_cap_PDMapped_ವಿಳಾಸ(cap),
            pd
        );
        clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *)pd, cap_get_cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap));
    }

    cap_ಪುಟ_directory_cap_ptr_ಗಣ_cap_PDIs_Mapped(&(ct_ಸ್ಥಾನ->cap), 0);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_X64_ಪುಟ_Directory_Invocation_Map(cap_t cap, cte_t *ct_ಸ್ಥಾನ, pdpte_t pdpte, pdpte_t *pdpt_ಸ್ಥಾನ,
                                                        vspace_ಬೇರು_t *vspace)
{
    ct_ಸ್ಥಾನ->cap = cap;
    *pdpt_ಸ್ಥಾನ = pdpte;
    invalidate_ಪುಟ_Structure_Cache_ASID(pptr_ಗಮ್ಯ_paddr(vspace), cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}


ಸ್ಥಿರ exception_t decode_X64_ಪುಟ_Directory_Invocation(
    ಶಬ್ದ_t label,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cte_t *cte,
    cap_t cap,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಶಬ್ದ_t              vaddr;
    vm_ಗುಣಲಕ್ಷಣಗಳು_t     vm_attr;
    cap_t               vspaceCap;
    vspace_ಬೇರು_t      *vspace;
    pdpte_t             pdpte;
    paddr_t             paddr;
    asid_t              asid;
    lookupPDPTSlot_ret_t pdpt_ಸ್ಥಾನ;

    ಆದರೆ (label == X86_ಪುಟ_Directory_Unmap) {
        ಆದರೆ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            userError("X86PageDirectory: Cannot unmap if more than one cap exist.");
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);

        ಹಿಂತಿರುಗಿ perform_X64_ಪುಟ_Directory_Invocation_Unmap(cap, cte);
    }

    ಆದರೆ (label != X86_ಪುಟ_Directory_Map) {
        userError("X64Directory: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("X64PageDirectory: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(cap)) {
        userError("X64PageDirectory: PD is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ಬಫರ್) & (~ಮುಖವಾಡ(PDPT_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ));
    vm_attr = vm_ಗುಣಲಕ್ಷಣಗಳು_From_ಶಬ್ದ(getSyscallArg(1, ಬಫರ್));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (!is_Valid_Native_ಬೇರು(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ಬೇರು_t *)pptr_ದ_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    ಆದರೆ (vaddr > USER_ತುದಿ) {
        userError("X64PageDirectory: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pdpt_ಸ್ಥಾನ = lookupPDPTSlot(vspace, vaddr);
    ಆದರೆ (pdpt_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ ((pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ) == pdpte_pdpte_pd &&
         pdpte_pdpte_pd_ptr_get_present(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ)) ||
        (pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ) == pdpte_pdpte_1g
         && pdpte_pdpte_1g_ptr_get_present(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ))) {
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ಗಮ್ಯ_paddr(PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(cap)));
    pdpte = makeUserPDPTEPageDirectory(paddr, vm_attr);

    cap = cap_ಪುಟ_directory_cap_ಗಣ_cap_PDIs_Mapped(cap, 1);
    cap = cap_ಪುಟ_directory_cap_ಗಣ_cap_PDMapped_ASID(cap, asid);
    cap = cap_ಪುಟ_directory_cap_ಗಣ_cap_PDMapped_ವಿಳಾಸ(cap, vaddr);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ perform_X64_ಪುಟ_Directory_Invocation_Map(cap, cte, pdpte, pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ, vspace);
}

ಸ್ಥಿರ ಶೂನ್ಯ unmapPDPT(asid_t asid, vptr_t vaddr, pdpte_t *pdpt)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    pml4e_t *pml4_ಸ್ಥಾನ;

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ;
    }

    pml4_ಸ್ಥಾನ = lookup_PML4_ಸ್ಥಾನ(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vaddr);

    /* check if the PML4 has the PDPT */
    ಆದರೆ (!(pml4e_ptr_get_present(pml4_ಸ್ಥಾನ) &&
          pml4e_ptr_get_pdpt_ತಳ_ವಿಳಾಸ(pml4_ಸ್ಥಾನ) == pptr_ಗಮ್ಯ_paddr(pdpt))) {
        ಹಿಂತಿರುಗಿ;
    }

    flushPDPT(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vaddr, pdpt, asid);

    *pml4_ಸ್ಥಾನ = makeUserPML4EInvalid();
}

ಸ್ಥಿರ exception_t performX64PDPTInvocationUnmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    ಆದರೆ (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        pdpte_t *pdpt = PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap));
        unmapPDPT(cap_pdpt_cap_get_capPDPTMappedASID(cap),
                  cap_pdpt_cap_get_cap_PDPTMapped_ವಿಳಾಸ(cap),
                  pdpt);
        clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *)pdpt, cap_get_cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap));
    }

    cap_pdpt_cap_ptr_ಗಣ_cap_PDPTIs_Mapped(&(ct_ಸ್ಥಾನ->cap), 0);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t performX64PDPTInvocationMap(cap_t cap, cte_t *ct_ಸ್ಥಾನ, pml4e_t pml4e, pml4e_t *pml4_ಸ್ಥಾನ,
                                               vspace_ಬೇರು_t *vspace)
{
    ct_ಸ್ಥಾನ->cap = cap;
    *pml4_ಸ್ಥಾನ = pml4e;
    invalidate_ಪುಟ_Structure_Cache_ASID(pptr_ಗಮ್ಯ_paddr(vspace), cap_pdpt_cap_get_capPDPTMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeX64PDPTInvocation(
    ಶಬ್ದ_t  label,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cte_t   *cte,
    cap_t   cap,
    ಶಬ್ದ_t  *ಬಫರ್)
{
    ಶಬ್ದ_t                  vaddr;
    vm_ಗುಣಲಕ್ಷಣಗಳು_t         attr;
    pml4e_t                *pml4_ಸ್ಥಾನ;
    cap_t                   vspaceCap;
    vspace_ಬೇರು_t          *vspace;
    pml4e_t                 pml4e;
    paddr_t                 paddr;
    asid_t                  asid;

    ಆದರೆ (label == X86PDPTUnmap) {
        ಆದರೆ (!isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            userError("X86PDPT: Cannot unmap if more than one cap exist.");
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);

        ಹಿಂತಿರುಗಿ performX64PDPTInvocationUnmap(cap, cte);
    }

    ಆದರೆ (label != X86PDPTMap) {
        userError("X86PDPT: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("X64PDPT: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
        userError("X64PDPT: PDPT is already mapped to a PML4.");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ಬಫರ್) & (~ಮುಖವಾಡ(PML4_ಸೂಚ್ಯಂಕ_ಸ್ಥಾನಾಂತರ));
    attr = vm_ಗುಣಲಕ್ಷಣಗಳು_From_ಶಬ್ದ(getSyscallArg(1, ಬಫರ್));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (!is_Valid_Native_ಬೇರು(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ಬೇರು_t *)pptr_ದ_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    ಆದರೆ (vaddr > USER_ತುದಿ) {
        userError("X64PDPT: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != vspace) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    pml4_ಸ್ಥಾನ = lookup_PML4_ಸ್ಥಾನ(vspace, vaddr);

    ಆದರೆ (pml4e_ptr_get_present(pml4_ಸ್ಥಾನ)) {
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ಗಮ್ಯ_paddr(PDPTE_PTR((cap_pdpt_cap_get_capPDPTBasePtr(cap))));
    pml4e = makeUserPML4E(paddr, attr);

    cap = cap_pdpt_cap_ಗಣ_cap_PDPTIs_Mapped(cap, 1);
    cap = cap_pdpt_cap_ಗಣ_cap_PDPTMapped_ASID(cap, asid);
    cap = cap_pdpt_cap_ಗಣ_cap_PDPTMapped_ವಿಳಾಸ(cap, vaddr);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ performX64PDPTInvocationMap(cap, cte, pml4e, pml4_ಸ್ಥಾನ, vspace);
}

exception_t decode_X86_ಬಹುಲಕ_MMUInvocation(
    ಶಬ್ದ_t label,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {

    ಸ್ಥಿತಿ cap_pml4_cap:
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;

    ಸ್ಥಿತಿ cap_pdpt_cap:
        ಹಿಂತಿರುಗಿ decodeX64PDPTInvocation(label, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);

    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
        ಹಿಂತಿರುಗಿ decode_X64_ಪುಟ_Directory_Invocation(label, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid arch cap type");
    }
}

bool_t ಬಹುಲಕ_Unmap_ಪುಟ(vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, vspace_ಬೇರು_t *vroot, vptr_t vaddr, ಶೂನ್ಯ *pptr)
{
    ಆದರೆ (config_ಗಣ(CONFIG_HUGE_ಪುಟ) && ಪುಟ_ಗಾತ್ರ == X64_Huge_ಪುಟ) {
        pdpte_t     *pdpte;
        lookupPDPTSlot_ret_t pdpt_ret = lookupPDPTSlot(vroot, vaddr);
        ಆದರೆ (pdpt_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ false;
        }
        pdpte = pdpt_ret.pdpt_ಸ್ಥಾನ;


        ಆದರೆ (!(pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(pdpte) == pdpte_pdpte_1g
              && pdpte_pdpte_1g_ptr_get_present(pdpte)
              && (pdpte_pdpte_1g_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pdpte)
                  == pptr_ಗಮ್ಯ_paddr(pptr)))) {
            ಹಿಂತಿರುಗಿ false;
        }

        *pdpte = makeUserPDPTEInvalid();
        ಹಿಂತಿರುಗಿ true;
    }
    fail("Invalid page type");
    ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ exception_t updatePDPTE(asid_t asid, pdpte_t pdpte, pdpte_t *pdpt_ಸ್ಥಾನ, vspace_ಬೇರು_t *vspace)
{
    *pdpt_ಸ್ಥಾನ = pdpte;
    invalidate_ಪುಟ_Structure_Cache_ASID(pptr_ಗಮ್ಯ_paddr(vspace), asid,
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_X64_ಬಹುಲಕ_Map(cap_t cap, cte_t *ct_ಸ್ಥಾನ, pdpte_t pdpte, pdpte_t *pdpt_ಸ್ಥಾನ, vspace_ಬೇರು_t *vspace)
{
    ct_ಸ್ಥಾನ->cap = cap;
    ಹಿಂತಿರುಗಿ updatePDPTE(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap), pdpte, pdpt_ಸ್ಥಾನ, vspace);
}

ರಚನೆ create_mapping_pdpte_return {
    exception_t ಸ್ಥಿತಿ_2;
    pdpte_t pdpte;
    pdpte_t *pdpt_ಸ್ಥಾನ;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ create_mapping_pdpte_return create_mapping_pdpte_return_t;

ಸ್ಥಿರ create_mapping_pdpte_return_t create_ಸುರಕ್ಷಿತ_Mapping_Entries_PDPTE(paddr_t ತಳ, ಶಬ್ದ_t vaddr, vm_rights_t vmRights,
                                                                    vm_ಗುಣಲಕ್ಷಣಗಳು_t attr,
                                                                    vspace_ಬೇರು_t *vspace)
{
    create_mapping_pdpte_return_t ret;
    lookupPDPTSlot_ret_t          lu_ret;

    lu_ret = lookupPDPTSlot(vspace, vaddr);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        ಹಿಂತಿರುಗಿ ret;
    }
    ret.pdpt_ಸ್ಥಾನ = lu_ret.pdpt_ಸ್ಥಾನ;

    /* check for existing page directory */
    ಆದರೆ ((pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(ret.pdpt_ಸ್ಥಾನ) == pdpte_pdpte_pd) &&
        (pdpte_pdpte_pd_ptr_get_present(ret.pdpt_ಸ್ಥಾನ))) {
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.pdpte = make_User_PDPTEHuge_ಪುಟ(ತಳ, attr, vmRights);
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

exception_t decode_X86_ಬಹುಲಕ_Map_ಪುಟ(ಶಬ್ದ_t label, vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, cte_t *cte, cap_t cap,
                                 vspace_ಬೇರು_t *vroot, vptr_t vaddr, paddr_t paddr, vm_rights_t vm_rights, vm_ಗುಣಲಕ್ಷಣಗಳು_t vm_attr)
{
    ಆದರೆ (config_ಗಣ(CONFIG_HUGE_ಪುಟ) && ಪುಟ_ಗಾತ್ರ == X64_Huge_ಪುಟ) {
        create_mapping_pdpte_return_t map_ret;

        map_ret = create_ಸುರಕ್ಷಿತ_Mapping_Entries_PDPTE(paddr, vaddr, vm_rights, vm_attr, vroot);
        ಆದರೆ (map_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ map_ret.ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);

        ಆಯ್ಕೆ (label) {
        ಸ್ಥಿತಿ X86_ಪುಟ_Map:
            ಹಿಂತಿರುಗಿ perform_X64_ಬಹುಲಕ_Map(cap, cte, map_ret.pdpte, map_ret.pdpt_ಸ್ಥಾನ, vroot);

        ಪೂರ್ವನಿಯೋಜಿತ:
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
    }
    fail("Invalid Page type");
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ read_ಶಬ್ದ_From_VSpace_ret {
    exception_t ಸ್ಥಿತಿ_2;
    ಶಬ್ದ_t ಮೌಲ್ಯ;
} read_ಶಬ್ದ_From_VSpace_ret_t;

ಸ್ಥಿರ read_ಶಬ್ದ_From_VSpace_ret_t read_ಶಬ್ದ_From_VSpace(vspace_ಬೇರು_t *vspace, ಶಬ್ದ_t vaddr)
{
    read_ಶಬ್ದ_From_VSpace_ret_t ret;
    lookupPTSlot_ret_t pt_ಸ್ಥಾನ;
    lookupPDSlot_ret_t pd_ಸ್ಥಾನ;
    lookupPDPTSlot_ret_t pdpt_ಸ್ಥಾನ;
    paddr_t paddr;
    ಶಬ್ದ_t ಸ್ಥಾನಾಂತರ;
    pptr_t kernel_vaddr;
    ಶಬ್ದ_t *ಮೌಲ್ಯ;

    pdpt_ಸ್ಥಾನ = lookupPDPTSlot(vspace, vaddr);
    ಆದರೆ (pdpt_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE &&
        pdpte_ptr_get_ಪುಟ_ಗಾತ್ರ(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ)) {

        paddr = pdpte_pdpte_1g_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pdpt_ಸ್ಥಾನ.pdpt_ಸ್ಥಾನ);
        ಸ್ಥಾನಾಂತರ = vaddr & ಮುಖವಾಡ(se_L4_Huge_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        pd_ಸ್ಥಾನ = lookupPDSlot(vspace, vaddr);
        ಆದರೆ (pd_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE &&
            ((pde_ptr_get_ಪುಟ_ಗಾತ್ರ(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ))) {

            paddr = pde_pde_large_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ);
            ಸ್ಥಾನಾಂತರ = vaddr & ಮುಖವಾಡ(se_L4_Large_ಪುಟ_ಬಿಟ್ಟುಗಳು);
        } ಇಲ್ಲದಿದ್ದರೆ {
            pt_ಸ್ಥಾನ = lookupPTSlot(vspace, vaddr);
            ಆದರೆ (pt_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE && pte_ptr_get_present(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ)) {
                paddr = pte_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pt_ಸ್ಥಾನ.pt_ಸ್ಥಾನ);
                ಸ್ಥಾನಾಂತರ = vaddr & ಮುಖವಾಡ(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
                ಹಿಂತಿರುಗಿ ret;
            }
        }
    }


    kernel_vaddr = (ಶಬ್ದ_t)paddr_ಗಮ್ಯ_pptr(paddr);
    ಮೌಲ್ಯ = (ಶಬ್ದ_t *)(kernel_vaddr + ಸ್ಥಾನಾಂತರ);
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ret.ಮೌಲ್ಯ = *ಮೌಲ್ಯ;
    ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ Arch_user_ರಾಶಿ_Trace(tcb_t *tptr)
{
    cap_t thread_ಬೇರು;
    vspace_ಬೇರು_t *vspace_ಬೇರು;
    ಶಬ್ದ_t sp;
    ಪೂರ್ಣಾಂಕ i;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(tptr, tcbVTable)->cap;

    /* lookup the PD */
    ಆದರೆ (cap_get_capType(thread_ಬೇರು) != cap_pml4_cap) {
        ಬರೆ("Invalid vspace\n");
        ಹಿಂತಿರುಗಿ;
    }

    vspace_ಬೇರು = (vspace_ಬೇರು_t *)pptr_ದ_cap(thread_ಬೇರು);

    sp = get_ರಿಜಿಸ್ಟರ್(tptr, RSP);
    /* check for alignment so we don't have to worry about accessing
     * words that might be on two different pages */
    ಆದರೆ (!IS_ALIGNED(sp, se_L4_ಶಬ್ದ_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        ಬರೆ("RSP not aligned\n");
        ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (i = 0; i < CONFIG_USER_ರಾಶಿ_TRACE_ಉದ್ದಳತೆ; i++) {
        ಶಬ್ದ_t ವಿಳಾಸ = sp + (i * ಗಾತ್ರ(ಶಬ್ದ_t));
        read_ಶಬ್ದ_From_VSpace_ret_t ಫಲಿತಾಂಶ;
        ಫಲಿತಾಂಶ = read_ಶಬ್ದ_From_VSpace(vspace_ಬೇರು, ವಿಳಾಸ);
        ಆದರೆ (ಫಲಿತಾಂಶ.ಸ್ಥಿತಿ_2 == EXCEPTION_NONE) {
            ಬರೆ("0x%lx: 0x%lx\n", (ದೀರ್ಘ)ವಿಳಾಸ, (ದೀರ್ಘ)ಫಲಿತಾಂಶ.ಮೌಲ್ಯ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("0x%lx: INVALID\n", (ದೀರ್ಘ)ವಿಳಾಸ);
        }
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PRINTING */

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

    pde_t pde = pde_pde_large_new(
                    0,                 /* xd                   */
                    ks_User_Log_ಬಫರ್,   /* page_base_address    */
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HUGE_ಪುಟ
    x64KSKernelPD[1] = pde;
#ಇಲ್ಲದಿದ್ದರೆ
    x64KSKernelPDs[ಬಿಟ್(PDPT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು) - 1][1] = pde;
#ಆದರೆ_ಅಂತ್ಯ
    invalidateTranslationAll(ಮುಖವಾಡ(CONFIG_MAX_ಅಂಕೆ_NODES));

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_KERNEL_LOG_BUFFER */
